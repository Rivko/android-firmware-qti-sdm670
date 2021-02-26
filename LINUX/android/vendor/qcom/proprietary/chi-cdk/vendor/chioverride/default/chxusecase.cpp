////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecase.cpp
/// @brief Usecases class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxusecase.h"
#include "chistatspropertydefines.h"
#include "chxutils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::~Usecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Usecase::~Usecase()
{
    // work around cmake warning
    (VOID)PerNumTargetUsecases;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::DestroyObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Usecase::DestroyObject(
    BOOL isForced)
{
    // Flush everything

    // Call the derived usecase object to destroy itself
    Destroy(isForced);
    if (TRUE == isForced)
    {
        FlushLock(FALSE);
    }

    if (NULL != m_pAppResultMutex)
    {
        m_pAppResultMutex->Destroy();
        m_pAppResultMutex = NULL;
    }

    if (NULL != m_pAllResultsMutex)
    {
        m_pAllResultsMutex->Destroy();
        m_pAllResultsMutex = NULL;
    }

    if (NULL != m_pAppResultAvailable)
    {
        m_pAppResultAvailable->Destroy();
        m_pAppResultAvailable = NULL;
    }

    if (NULL != m_pAllResultsAvailable)
    {
        m_pAllResultsAvailable->Destroy();
        m_pAllResultsAvailable = NULL;
    }

    for (UINT i = 0; i < MaxOutstandingRequests; i++)
    {
        if (NULL != m_captureResult[i].output_buffers)
        {
            CHX_FREE(const_cast<camera3_stream_buffer_t*>(m_captureResult[i].output_buffers));
            m_captureResult[i].output_buffers = NULL;
        }
        if (NULL != m_captureResult[i].input_buffer)
        {
            CHX_FREE(const_cast<camera3_stream_buffer_t*>(m_captureResult[i].input_buffer));
            m_captureResult[i].input_buffer = NULL;
        }
    }

    // Free the replaced metadata
    if (NULL != m_pReplacedMetadata)
    {
        free_camera_metadata(m_pReplacedMetadata);
        m_pReplacedMetadata = NULL;
    }
    m_pMapLock->Lock();
    for (UINT i = 0; i < MaxOutstandingRequests; i++)
    {
        if (NULL != m_pendingPCRs[i].output_buffers)
        {
            CHX_FREE(const_cast<camera3_stream_buffer_t*>(m_pendingPCRs[i].output_buffers));
            m_pendingPCRs[i].output_buffers     = NULL;
            m_pendingPCRs[i].num_output_buffers = 0;
        }
    }
    m_pMapLock->Unlock();

    if (m_pMapLock)
    {
        m_pMapLock->Destroy();
        m_pMapLock = NULL;
    }

    m_isFlushEnabled = FALSE;

    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Usecase::Initialize()
{
    CDKResult result = CDKResultSuccess;

    m_nextAppResultFrame  = InvalidFrameNumber;
    m_nextAppMessageFrame = InvalidFrameNumber;
    m_lastAppRequestFrame = InvalidFrameNumber;
    m_lastResultMetadataFrameNum    = -1;

    // This is the first valid frame number in the override module and is incremented sequentially for every new request
    m_chiOverrideFrameNum = 0;

    m_resultProcessingThread.pPrivateData = this;

    m_pAppResultMutex      = Mutex::Create();
    m_pMapLock             = Mutex::Create();
    m_pAllResultsMutex     = Mutex::Create();
    m_pAppResultAvailable  = Condition::Create();
    m_pAllResultsAvailable = Condition::Create();
    m_isFlushEnabled       = FALSE;

    // Allocate replaced metadata
    m_pReplacedMetadata    = allocate_camera_metadata(ReplacedMetadataEntryCapacity, ReplacedMetadataDataCapacity);
    CHX_ASSERT(NULL != m_pReplacedMetadata);
    m_replacedMetadataSize = calculate_camera_metadata_size(ReplacedMetadataEntryCapacity, ReplacedMetadataDataCapacity);

    m_pMapLock->Lock();
    for (UINT i = 0; i < MaxOutstandingRequests; i++)
    {
        m_pendingPCRs[i].num_output_buffers = 0;
        m_pendingPCRs[i].output_buffers     =
            static_cast<camera3_stream_buffer_t*>(CHX_CALLOC(sizeof(camera3_stream_buffer_t) * MaxExternalBuffers));
    }

    for (UINT i = 0; i < MaxOutstandingRequests; i++)
    {
        m_numberOfPendingOutputMetadata[i].FrameworkFrameNumber = 0;
        m_numberOfPendingOutputMetadata[i].isMetadataSent       = FALSE;
    }
    m_pMapLock->Unlock();

    for (UINT i = 0; i < MaxOutstandingRequests; i++)
    {
        m_captureResult[i].output_buffers = static_cast<camera3_stream_buffer_t*>(
            CHX_CALLOC(sizeof(camera3_stream_buffer_t) * MaxExternalBuffers));

        m_captureResult[i].input_buffer   = static_cast<camera3_stream_buffer_t*>(
            CHX_CALLOC(sizeof(camera3_stream_buffer_t)));
    }

    /// @todo Enable Multithreading: We need to enable threading so keeping it and will enable as a follow on
    // result = ChxUtils::ThreadCreate(Usecase::ResultThreadMain,
    //                                 &m_resultProcessingThread,
    //                                 &m_resultProcessingThread.hThreadHandle);
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Usecase::ResultThreadMain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* Usecase::ResultThreadMain(
    VOID* pThreadData)
{
    PerThreadData* pPerThreadData = reinterpret_cast<PerThreadData*>(pThreadData);

    Usecase* pUsecase = reinterpret_cast<Usecase*>(pPerThreadData->pPrivateData);

    pUsecase->ProcessResults();

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::ProcessCaptureRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Usecase::ProcessCaptureRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult result              = CDKResultSuccess;

    if (TRUE == m_isFlushEnabled)
    {
        CHX_LOG(" flush enabled, frame %d", pRequest->frame_number);
        HandleProcessRequestErrorAllPCRs(pRequest);
        return CDKResultSuccess;
    }

    UINT32    chiOverrideFrameNum = GetChiOverrideFrameNum(pRequest->frame_number);

    m_pMapLock->Lock();

    UINT32 resultFrameIndexChi                           = chiOverrideFrameNum % MaxOutstandingRequests;
    m_numAppPendingOutputBuffers[resultFrameIndexChi]    = pRequest->num_output_buffers;

    if(FALSE == m_numberOfPendingOutputMetadata[resultFrameIndexChi].isMetadataSent
            && m_numberOfPendingOutputMetadata[resultFrameIndexChi].FrameworkFrameNumber > 0)
    {
        CHX_LOG("Pending METADATA in PCR  =%d", m_numberOfPendingOutputMetadata[resultFrameIndexChi].FrameworkFrameNumber);
        //We reached max number of outstanding requests but metadata is not sent. Most probably errored out. Send Metadata error for this frame.
        ChiMessageDescriptor        messageDescriptor;

        messageDescriptor.messageType                               = ChiMessageTypeError;
        messageDescriptor.message.errorMessage.frameworkFrameNum    = m_numberOfPendingOutputMetadata[resultFrameIndexChi].FrameworkFrameNumber;
        messageDescriptor.message.errorMessage.errorMessageCode     = MessageCodeResult;
        messageDescriptor.message.errorMessage.pErrorStream         = NULL;

        ReturnFrameworkMessageFlushError((camera3_notify_msg_t*)&messageDescriptor);
    }
    m_numberOfPendingOutputMetadata[resultFrameIndexChi].isMetadataSent = FALSE;
    m_numberOfPendingOutputMetadata[resultFrameIndexChi].FrameworkFrameNumber = pRequest->frame_number;
    CHX_LOG("Saving buffer for chiFrame =%d, requestFrame=%d, NumBuff = %d resultFrameIndexChi=%d",
            resultFrameIndexChi, pRequest->frame_number, pRequest->num_output_buffers, resultFrameIndexChi);

    if (0 != m_pendingPCRs[resultFrameIndexChi].num_output_buffers)
    {
        // old request not returned yet. return result for the same.
        camera3_capture_request_t request = m_pendingPCRs[resultFrameIndexChi];
        CHX_LOG(" flush array not empty FRAME =%d", m_pendingPCRs[resultFrameIndexChi].frame_number);
        HandleProcessRequestErrorAllPCRs(&request);
        m_pendingPCRs[resultFrameIndexChi].num_output_buffers = 0;
    }

    // Set pending output buffers after clearing the previous one
    m_numberOfPendingOutputBuffers[resultFrameIndexChi] = pRequest->num_output_buffers;

    m_PendingMessage[resultFrameIndexChi]                   = TRUE;
    m_pendingPCRs[resultFrameIndexChi].frame_number         = pRequest->frame_number;
    m_pendingPCRs[resultFrameIndexChi].num_output_buffers   = pRequest->num_output_buffers;

    if (m_nextAppMessageFrame == InvalidFrameNumber)
    {
        m_nextAppMessageFrame           = chiOverrideFrameNum; //pRequest->frame_number;
        m_lastAppMessageFrameReceived   = chiOverrideFrameNum; //pRequest->frame_number;
    }

    if (m_nextAppResultFrame == InvalidFrameNumber)
    {
        m_nextAppMessageFrame = chiOverrideFrameNum;
        m_lastAppRequestFrame = chiOverrideFrameNum;
        m_lastResultMetadataFrameNum = m_nextAppMessageFrame - 1;
    }

    for (UINT i = 0; i < pRequest->num_output_buffers; i++)
    {
        if (&pRequest->output_buffers[i] == NULL)
            continue;

        CHX_LOG(" SAVING, frame %d resultFrameIndexChi=%d",
            pRequest->frame_number,
            resultFrameIndexChi);
        ChxUtils::Memcpy(const_cast<camera3_stream_buffer_t*>(
            &m_pendingPCRs[resultFrameIndexChi].output_buffers[i]),
            &pRequest->output_buffers[i],
            sizeof(camera3_stream_buffer_t));
    }

    m_pMapLock->Unlock();

    /// Chi override frame number is what the rest of the override module knows. The original application frame number is only
    /// known to this class and no one else. Hence any result communication to application needs to go thru this class strictly
    pRequest->frame_number = chiOverrideFrameNum;

    /// @todo Better way to handle this instead of per request - assumption is frame_number is incrementing linearly because
    ///       we assume next result expected is always for frame_number "m_nextAppResultFrame + 1"
    if (InvalidFrameNumber == m_nextAppResultFrame)
    {
        m_nextAppResultFrame = chiOverrideFrameNum;
    }

    m_numAppPendingOutputBuffers[chiOverrideFrameNum % MaxOutstandingRequests] = pRequest->num_output_buffers;

    if (NULL != pRequest->settings)
    {
        // Replace the metadata by appending vendor tag for cropRegions
        result = ReplaceRequestMetadata(pRequest->settings);
        if (CDKResultSuccess == result)
        {
            pRequest->settings = m_pReplacedMetadata;
        }
    }

    ChxUtils::AtomicStoreU64(&m_lastAppRequestFrame, chiOverrideFrameNum);

    result = ExecuteCaptureRequest(pRequest);

    // Restore the original metadata
    RestoreRequestMetadata(pRequest);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::ReplaceRequestMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Usecase::ReplaceRequestMetadata(
    const VOID* pMetadata)
{
    CDKResult result = CDKResultSuccess;

    // Save the the original metadata
    m_pOriginalMetadata = pMetadata;

    m_pReplacedMetadata = place_camera_metadata(m_pReplacedMetadata,
                            m_replacedMetadataSize,
                            ReplacedMetadataEntryCapacity,
                            ReplacedMetadataDataCapacity);

    // Add the existing metadata first before appending the new tags
    result = append_camera_metadata(m_pReplacedMetadata, static_cast<const camera_metadata_t*>(pMetadata));

    if (CDKResultSuccess == result)
    {
        // Read the android crop region
        camera_metadata_entry_t entry = { 0 };
        if (0 == find_camera_metadata_entry(m_pReplacedMetadata, ANDROID_SCALER_CROP_REGION, &entry))
        {
            CaptureRequestCropRegions cropRegions;
            cropRegions.userCropRegion.left   = entry.data.i32[0];
            cropRegions.userCropRegion.top    = entry.data.i32[1];
            cropRegions.userCropRegion.width  = entry.data.i32[2];
            cropRegions.userCropRegion.height = entry.data.i32[3];

            cropRegions.pipelineCropRegion = cropRegions.userCropRegion;
            cropRegions.ifeLimitCropRegion = cropRegions.userCropRegion;

            // Set the cropRegions vendor tag data
            ChxUtils::SetVendorTagValue(m_pReplacedMetadata, CropRegions,
                            sizeof(CaptureRequestCropRegions), &cropRegions);
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::RestoreRequestMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Usecase::RestoreRequestMetadata(
    camera3_capture_request_t* pRequest)
{
    if (m_pOriginalMetadata != NULL)
    {
        pRequest->settings = static_cast<const camera_metadata_t*>(m_pOriginalMetadata);
        m_pOriginalMetadata = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::ProcessCaptureResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Usecase::ProcessCaptureResult(
    const CHICAPTURERESULT* pResult)
{
    CDKResult result = CDKResultSuccess;

    ///@ todo Fix preview only case
    (VOID)pResult;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::ProcessMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Usecase::ProcessMessage(
    const CHIMESSAGEDESCRIPTOR* pMessageDescriptor)
{
    CDKResult result = CDKResultSuccess;

    ///@ todo Fix preview only case
    (VOID)pMessageDescriptor;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::ReturnFrameworkResultFlush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Usecase::ReturnFrameworkResultFlush(
    const camera3_capture_result_t* pResult)
{
    camera3_capture_result_t* pOverrideResult       = const_cast<camera3_capture_result_t*>(pResult);
    UINT32                    chiOverrideFrameNum   = pResult->frame_number;
    UINT32                    resultFrameIndexChi   = chiOverrideFrameNum % MaxOutstandingRequests;

    pOverrideResult->frame_number                   = chiOverrideFrameNum;

    CHX_LOG("ReturnFrameworkResultFlush chiOverrideFrameNum %d frame_number %d resultFrameIndexF=%d FRAMEf=%d, BUFFER=%d",
            chiOverrideFrameNum,
            pOverrideResult->frame_number,
            resultFrameIndexChi,
            m_captureResult[resultFrameIndexChi].frame_number,
            m_numberOfPendingOutputBuffers[resultFrameIndexChi]);

    //save number of buffers
    //check for which frame buffer is back
    //if 0 remove request from map
    m_pMapLock->Lock();

    if (m_numberOfPendingOutputBuffers[resultFrameIndexChi] >= pResult->num_output_buffers)
    {
        m_numberOfPendingOutputBuffers[resultFrameIndexChi] -= pResult->num_output_buffers;
    }

    //We are sending error for this request, don't send metadata error again
    for(UINT32 i=0; i < MaxOutstandingRequests; i++)
    {
        if(m_numberOfPendingOutputMetadata[i].FrameworkFrameNumber == pOverrideResult->frame_number)
        {
            CHX_LOG("In flush sending metadata for %d", m_numberOfPendingOutputMetadata[i].FrameworkFrameNumber);
            m_numberOfPendingOutputMetadata[i].isMetadataSent = TRUE;
        }
    }

    CHX_LOG("m_numberOfPendingOutputBuffers = %d", m_numberOfPendingOutputBuffers[resultFrameIndexChi]);
    if (m_numberOfPendingOutputBuffers[resultFrameIndexChi] == 0)
    {
        camera3_capture_request_t request = m_pendingPCRs[resultFrameIndexChi];
        if (0 != request.num_output_buffers)
        {
            CHX_LOG("ReturnFrameworkResultFlush Pending result IN MAP =%d Actual Frame=%d", request.frame_number,
                    pOverrideResult->frame_number);
            if (request.frame_number == pOverrideResult->frame_number)
            {
                m_pendingPCRs[resultFrameIndexChi].num_output_buffers = 0;
            }
        }
    }
    m_pMapLock->Unlock();

    ExtensionModule::GetInstance()->ReturnFrameworkResult(reinterpret_cast<const camera3_capture_result_t*>(pOverrideResult),
                                                          this->m_cameraId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Usecase::HandleProcessRequestErrorAllPCRs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Usecase::HandleProcessRequestErrorAllPCRs(
    camera3_capture_request_t* pRequest)
{
    ChiMessageDescriptor        messageDescriptor;
    UINT numOutputBuffersPending = 0;
    camera3_stream_buffer_t pendingStreamBuffers[16];

    messageDescriptor.messageType                               = ChiMessageTypeError;
    messageDescriptor.message.errorMessage.frameworkFrameNum    = pRequest->frame_number;
    messageDescriptor.message.errorMessage.errorMessageCode     = MessageCodeRequest;
    messageDescriptor.message.errorMessage.pErrorStream         = NULL;

    CHX_LOG("Sending errorMessageCode for frame %d", pRequest->frame_number);

    ReturnFrameworkMessageFlushError((camera3_notify_msg_t*)&messageDescriptor);

    camera3_capture_result_t result;
    result.frame_number       = pRequest->frame_number;
    result.result             = NULL;


    for (UINT i = 0; i < pRequest->num_output_buffers; i++)
    {
        // Send buffers which are non NULL as NULL means, already returned to frameworks in regular call
        if (pRequest->output_buffers[i].buffer != NULL)
        {
            camera3_stream_buffer_t* pStreamBuffer =
                const_cast<camera3_stream_buffer_t*>(&pRequest->output_buffers[i]);
            pStreamBuffer->release_fence            = -1;
            pStreamBuffer->status                   = CAMERA3_BUFFER_STATUS_ERROR;
            // Copy to pendingStreamBuffers
            pendingStreamBuffers[numOutputBuffersPending] = pRequest->output_buffers[i];
            numOutputBuffersPending++;
        }
        CHX_LOG("Flush buffer %p num_output_buffers %d numOutputBuffersPending %d",
            pRequest->output_buffers[i].buffer,
            pRequest->num_output_buffers, numOutputBuffersPending);
    }

    result.num_output_buffers = numOutputBuffersPending;
    result.output_buffers     = reinterpret_cast<const camera3_stream_buffer_t *>(pendingStreamBuffers);

    result.input_buffer       = NULL;
    result.partial_result     = 1;

    Usecase::ReturnFrameworkResultFlush(reinterpret_cast<const camera3_capture_result_t*>(&result));

    if (m_nextAppResultFrame == pRequest->frame_number)
    {
        m_nextAppResultFrame++;
    }

    if (m_lastResultMetadataFrameNum == pRequest->frame_number - 1)
    {
        m_lastResultMetadataFrameNum = pRequest->frame_number;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Usecase::ReturnFrameworkMessageFlushError
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Usecase::ReturnFrameworkMessageFlushError(
    const camera3_notify_msg_t* pMessage)
{
    camera3_notify_msg_t* pOverrideMessage = const_cast<camera3_notify_msg_t*>(pMessage);

    CHX_LOG("Sending errorMessageCode2 for frame IN ReturnFrameworkMessage %d CAMERAID=%d",
            pOverrideMessage->message.error.frame_number, this->m_cameraId);
    ExtensionModule::GetInstance()->ReturnFrameworkMessage(pOverrideMessage, this->m_cameraId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::ReturnFrameworkResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Usecase::ReturnFrameworkResult(
    const camera3_capture_result_t* pResult, UINT32 cameraID)
{
    camera3_capture_result_t* pOverrideResult               = const_cast<camera3_capture_result_t*>(pResult);
    UINT32                    chiOriginalOverrideFrameNum   = pResult->frame_number;
    UINT32                    resultFrameIndexChi           = chiOriginalOverrideFrameNum % MaxOutstandingRequests;

    pOverrideResult->frame_number = GetAppFrameNum(pResult->frame_number);
    CHX_LOG("ReturnFrameworkResult chiOriginalOverrideFrameNum: %d frame_number %d resultFrameIndexF=%d FRAMEf=%d, BUFFER=%d RESULT=%p",
            chiOriginalOverrideFrameNum,
            pOverrideResult->frame_number,
            resultFrameIndexChi,
            m_captureResult[resultFrameIndexChi].frame_number,
            m_numberOfPendingOutputBuffers[resultFrameIndexChi],
            pResult->result);

    //save number of buffers
    //check for which frame buffer is back
    //if 0 remove request from map
    m_pMapLock->Lock();
    if (m_numberOfPendingOutputBuffers[resultFrameIndexChi] >= pResult->num_output_buffers)
    {
        m_numberOfPendingOutputBuffers[resultFrameIndexChi] -= pResult->num_output_buffers;
    }

    if(NULL != pResult->result)
    {
        CHX_LOG("sending metadata for %d", m_numberOfPendingOutputMetadata[resultFrameIndexChi].FrameworkFrameNumber);
        m_numberOfPendingOutputMetadata[resultFrameIndexChi].isMetadataSent = TRUE;
    }
    CHX_LOG("m_numberOfPendingOutputBuffers = %d", m_numberOfPendingOutputBuffers[resultFrameIndexChi]);
    if (m_numberOfPendingOutputBuffers[resultFrameIndexChi] == 0)
     {
         camera3_capture_request_t request = m_pendingPCRs[resultFrameIndexChi];
         if (0 != request.num_output_buffers)
         {
             CHX_LOG("Pending result IN MAP =%d Actual Frame=%d", request.frame_number, pOverrideResult->frame_number);
             if (request.frame_number == pOverrideResult->frame_number)
             {
                 m_pendingPCRs[resultFrameIndexChi].num_output_buffers = 0;
                 CHX_LOG("erasing result IN MAP =%d Actual Frame=%d ", request.frame_number, pOverrideResult->frame_number);
             }
         }
     }
    else if (pResult->num_output_buffers != 0)
    {
        // Set NULL to returned buffer so that it won't be returned again in flush call
           CHX_LOG("pResult->num_output_buffers %d pending buffers %d",
            pResult->num_output_buffers, m_numberOfPendingOutputBuffers[resultFrameIndexChi]);

        camera3_capture_request_t request = m_pendingPCRs[resultFrameIndexChi];
        for (UINT i=0; i < pResult->num_output_buffers; i++ )
        {
            for (UINT j=0; j < request.num_output_buffers; j++ )
            {
                if (pResult->output_buffers[i].stream == request.output_buffers[j].stream)
                {
                    camera3_stream_buffer_t* pStreamBuffer =
                        const_cast<camera3_stream_buffer_t*>(&request.output_buffers[j]);
                    CHX_LOG("pStreamBuffer %p, i %d, j %d buffer %p",
                        pStreamBuffer, i, j, pStreamBuffer->buffer);
                    pStreamBuffer->buffer = NULL;
                    break;
                }
            }
       }
    }

    if (m_numberOfPendingOutputBuffers[resultFrameIndexChi] == 0)
    {
        camera3_capture_request_t request = m_pendingPCRs[resultFrameIndexChi];
        if (0 != request.num_output_buffers)
        {
            CHX_LOG("Pending result IN MAP =%d Actual Frame=%d", request.frame_number, pOverrideResult->frame_number);
            if (request.frame_number == pOverrideResult->frame_number)
            {
                m_pendingPCRs[resultFrameIndexChi].num_output_buffers = 0;
                CHX_LOG("erasing result IN MAP =%d Actual Frame=%d ", request.frame_number, pOverrideResult->frame_number);
            }
        }
    }
    m_pMapLock->Unlock();
    ExtensionModule::GetInstance()->ReturnFrameworkResult(reinterpret_cast<const camera3_capture_result_t*>(pOverrideResult),
                                                          cameraID);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::ReturnFrameworkMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Usecase::ReturnFrameworkMessage(
    const camera3_notify_msg_t* pMessage, UINT32 cameraID)
{
    camera3_notify_msg_t* pOverrideMessage = const_cast<camera3_notify_msg_t*>(pMessage);

    m_pAllResultsMutex->Lock();

    UINT64 resultFrame  = pMessage->message.shutter.frame_number;
    UINT   frameIndex   = (resultFrame % MaxOutstandingRequests);

    CHX_LOG("ResultFrame = %" PRIu64 " FrameIdx=%d type=%d", resultFrame, frameIndex, pOverrideMessage->type);

    if (ChiMessageTypeShutter == pOverrideMessage->type)
    {
        if (TRUE == m_PendingMessage[frameIndex])
        {
            if (pMessage->message.shutter.frame_number > m_lastAppMessageFrameReceived)
            {
                m_lastAppMessageFrameReceived = pMessage->message.shutter.frame_number;
            }

            pOverrideMessage->message.shutter.frame_number = GetAppFrameNum(pMessage->message.shutter.frame_number);

            CHX_LOG("Override ResultFrame = %d m_lastAppMessageFrameReceived = %" PRIu64 " m_nextAppMessageFrame=%" PRIu64,
                pOverrideMessage->message.shutter.frame_number, m_lastAppMessageFrameReceived, m_nextAppMessageFrame);

            ChxUtils::Memcpy(&m_notifyMessage[frameIndex], pOverrideMessage, sizeof(camera3_notify_msg_t));
            m_MessageAvailable[frameIndex] = TRUE;

            if (m_nextAppMessageFrame != InvalidFrameNumber)
            {
                for (UINT64 i = m_nextAppMessageFrame; i <= m_lastAppMessageFrameReceived; i++)
                {
                    frameIndex = (i % MaxOutstandingRequests);
                    CHX_LOG("frameIndex = %d m_MessageAvailable=%d", frameIndex, m_MessageAvailable[frameIndex]);
                    if (m_nextAppMessageFrame == InvalidFrameNumber)
                    {
                        CHX_LOG_INFO("m_nextAppMessageFrame is invalid");
                        break;
                    }

                    if ((TRUE == m_MessageAvailable[frameIndex]) &&
                        (TRUE == m_PendingMessage[frameIndex])   &&
                        (m_nextAppMessageFrame != InvalidFrameNumber))
                    {
                        ExtensionModule::GetInstance()->ReturnFrameworkMessage(&m_notifyMessage[frameIndex], cameraID);
                        m_MessageAvailable[frameIndex] = FALSE;
                        m_PendingMessage[frameIndex]   = FALSE;
                        m_nextAppMessageFrame++;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        m_pAllResultsMutex->Unlock();
        return;
    }
    else if (ChiMessageTypeError == pOverrideMessage->type)
    {
        pOverrideMessage->message.error.frame_number = GetAppFrameNum(pMessage->message.error.frame_number);
        CHX_LOG("Sending errorMessageCode1 for frame IN ReturnFrameworkMessage %d CAMERAID=%d ",
                pOverrideMessage->message.error.frame_number, cameraID);
    }

    ExtensionModule::GetInstance()->ReturnFrameworkMessage(pOverrideMessage, cameraID);
    m_pAllResultsMutex->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::UpdateSensorModeIndex
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Usecase::UpdateSensorModeIndex(
    camera_metadata_t* pMetaData)
{
    CDKResult result = CDKResultSuccess;

    if ((NULL != pMetaData))
    {
        if (FALSE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(pMetaData), SensorModeIndex))
        {
            result = ChxUtils::SetVendorTagValue(pMetaData, SensorModeIndex, 1, &m_selectedSensorModeIndex);
        }
        if (CDKResultSuccess != result)
        {
            CHX_LOG_ERROR("Cant set sensor mode index vendor tag");
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::UpdateFeatureModeIndex
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Usecase::UpdateFeatureModeIndex(
    camera_metadata_t* pMetaData)
{
    CDKResult result = CDKResultSuccess;

    CHX_LOG("%s E", __func__);
    if ((NULL != pMetaData))
    {
        if (FALSE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(pMetaData), Feature1Mode))
        {
            ChiModeFeature1SubModeType featureMode = ChiModeFeature1SubModeType::None;
            if (ExtensionModule::GetInstance()->GetVideoHDRMode())
            {
                featureMode = ChiModeFeature1SubModeType::ISPHDR;
            }
            result = ChxUtils::SetVendorTagValue(pMetaData, Feature1Mode, 1, &featureMode);
        }
        if (CDKResultSuccess != result)
        {
            CHX_LOG_ERROR("Cant set Feature1Mode mode index vendor tag");
        }

        if (FALSE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(pMetaData), Feature2Mode))
        {
            ChiModeFeature2SubModeType featureMode = ChiModeFeature2SubModeType::None;
            result = ChxUtils::SetVendorTagValue(pMetaData, Feature2Mode, 1, &featureMode);
        }
        if (CDKResultSuccess != result)
        {
            CHX_LOG_ERROR("Cant set Feature1Mode mode index vendor tag");
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult Usecase::Flush()
{
    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Usecase::FlushLock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID Usecase::FlushLock(
    BOOL bLock)
{
    if (TRUE == bLock)
    {
        CHX_LOG("FlushLock start");
        m_nextAppMessageFrame   = InvalidFrameNumber;
        m_isFlushEnabled        = TRUE;
    }
    else
    {
        CHX_LOG("FlushUnLock start");
        m_pMapLock->Lock();

        for (UINT i = 0; i < MaxOutstandingRequests; i++)
        {
            camera3_capture_request_t request = m_pendingPCRs[i];
            CHX_LOG("Pending result IN MAP0 =%d", request.frame_number);
            if (0 != request.num_output_buffers)
            {
                CHX_LOG("deleting Pending result IN MAP0 =%d", request.frame_number);
                HandleProcessRequestErrorAllPCRs(&request);
            }
        }

        for (UINT i = 0; i < MaxOutstandingRequests; i++)
        {
            if (0 != m_pendingPCRs[i].num_output_buffers)
            {
                m_pendingPCRs[i].num_output_buffers = 0;
            }
        }

        m_lastAppMessageFrameReceived   = 0;

        m_nextAppResultFrame            = InvalidFrameNumber;
        m_lastAppRequestFrame           = 0;
        m_lastResultMetadataFrameNum    = 0;

        ChxUtils::Memset(&m_MessageAvailable[0], 0, sizeof(m_MessageAvailable));
        ChxUtils::Memset(&m_notifyMessage[0], 0, sizeof(m_notifyMessage));
        ChxUtils::Memset(&m_PendingMessage[0], 0, sizeof(m_PendingMessage));

        m_pMapLock->Unlock();

        //Send all unsent metadata. This is possible if Snapshot metadata is late.
        for (UINT i = 0; i < MaxOutstandingRequests; i++)
        {

            if(FALSE == m_numberOfPendingOutputMetadata[i].isMetadataSent
                    && m_numberOfPendingOutputMetadata[i].FrameworkFrameNumber > 0)
            {
                CHX_LOG("Pending METADATA  =%d", m_numberOfPendingOutputMetadata[i].FrameworkFrameNumber);
                    ChiMessageDescriptor        messageDescriptor;

                    messageDescriptor.messageType                               = ChiMessageTypeError;
                    messageDescriptor.message.errorMessage.frameworkFrameNum    = m_numberOfPendingOutputMetadata[i].FrameworkFrameNumber;
                    messageDescriptor.message.errorMessage.errorMessageCode     = MessageCodeResult;
                    messageDescriptor.message.errorMessage.pErrorStream         = NULL;


                    ReturnFrameworkMessageFlushError((camera3_notify_msg_t*)&messageDescriptor);
            }

        }

        m_isFlushEnabled = FALSE;
    }
    CHX_LOG("FlushLock end");
}
