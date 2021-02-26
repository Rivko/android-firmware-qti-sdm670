////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeatureyuvcb.cpp
/// @brief CHX YUV callback stream feature class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxadvancedcamerausecase.h"
#include "chxincs.h"
#include "chxfeatureyuvcb.h"
#include "chxusecase.h"

static const UINT ZslShutterLag = 2;
static const UINT InvalidFrameNum  = 0xFFFFFFFF;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureYuvCB::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FeatureYuvCB* FeatureYuvCB::Create(
    AdvancedCameraUsecase* pUsecase)
{
    FeatureYuvCB* pFeature = CHX_NEW FeatureYuvCB;

    if (NULL != pFeature)
    {
        if (CDKResultSuccess != pFeature->Initialize(pUsecase))
        {
            pFeature->Destroy(FALSE);
            pFeature = NULL;
        }
    }

    return pFeature;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureYuvCB::Pause
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureYuvCB::Pause(
    BOOL isForced)
{
    CHX_LOG("Feature pause isForced =%d", isForced);
    //Implement if required
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureYuvCB::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureYuvCB::Flush(
        BOOL isInProgress)
{
    CHX_LOG("Feature Flush isInProgress =%d", isInProgress);
    //Implement if required
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureYuvCB::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureYuvCB::Initialize(
    AdvancedCameraUsecase* pUsecase)
{
    CDKResult    result = CDKResultSuccess;

    m_pUsecase     = pUsecase;
    m_pResultMutex = Mutex::Create();

    m_lastRdiFrameAvailable          = InvalidFrameNum;
    m_lastRealtimeMetadataAvailable  = InvalidFrameNum;
    m_pRdiAndMetaResultMutex         = Mutex::Create();
    m_pRdiAndMetaResultAvailable     = Condition::Create();
    m_offlineRequestWaitingForFrame  = InvalidFrameNum;
    m_maxSnapshotReqId               = 0;
    m_snapshotReqId                  = 0;

    m_pOfflineRequestMutex           = Mutex::Create();
    m_pOfflineRequestAvailable       = Condition::Create();
    m_offlineRequestProcessTerminate = FALSE;

    m_offlineRequestProcessThread.pPrivateData = this;

    result = ChxUtils::ThreadCreate(FeatureYuvCB::RequestThread,
                                    &m_offlineRequestProcessThread,
                                    &m_offlineRequestProcessThread.hThreadHandle);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureYuvCB::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureYuvCB::Destroy(BOOL isForced)
{
    CHX_LOG("isForced %d ", isForced);

    m_pOfflineRequestMutex->Lock();
    m_offlineRequestProcessTerminate = TRUE;
    m_pOfflineRequestAvailable->Signal();
    m_pOfflineRequestMutex->Unlock();

    ChxUtils::ThreadTerminate(m_offlineRequestProcessThread.hThreadHandle);

    m_pYuvCBStream   = NULL;
    m_pPreviewStream = NULL;
    m_pRdiStream     = NULL;
    m_pBlobStream    = NULL;

    if (NULL != m_pResultMutex)
    {
        m_pResultMutex->Destroy();
        m_pResultMutex = NULL;
    }

    if (NULL != m_pOfflineRequestAvailable)
    {
        m_pOfflineRequestAvailable->Destroy();
        m_pOfflineRequestAvailable = NULL;
    }

    if (NULL != m_pOfflineRequestMutex)
    {
        m_pOfflineRequestMutex->Destroy();
        m_pOfflineRequestMutex = NULL;
    }

    if (NULL != m_pRdiAndMetaResultMutex)
    {
        m_pRdiAndMetaResultMutex->Destroy();
        m_pRdiAndMetaResultMutex = NULL;
    }

    if (NULL != m_pRdiAndMetaResultAvailable)
    {
        m_pRdiAndMetaResultAvailable->Destroy();
        m_pRdiAndMetaResultAvailable = NULL;
    }

    for (UINT32 i = 0; i < MaxOutstandingRequests; i++)
    {
        if (NULL != m_pSnapshotInputMeta[i])
        {
            free_camera_metadata(m_pSnapshotInputMeta[i]);
            m_pSnapshotInputMeta[i] = NULL;
        }
    }

    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureYuvCB::OverrideUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiUsecase* FeatureYuvCB::OverrideUsecase(
    LogicalCameraInfo*              pCameraInfo,
    camera3_stream_configuration_t* pStreamConfig)
{
    (VOID)pCameraInfo;
    CHX_LOG("OverrideUsecase for YuvCB and Blob 4 stream");
    CHX_ASSERT(4 == pStreamConfig->num_streams);

    m_pChiUsecase = m_pUsecase->GetChiUseCase();

    for (UINT target = 0; target < m_pChiUsecase->numTargets; target++)
    {
        if (ChiFormatRawMIPI == m_pChiUsecase->ppChiTargets[target]->pBufferFormats[0])
        {
            m_rdiStreamIndex   = target;
            m_pRdiTargetBuffer = m_pUsecase->GetTargetBuffer(m_rdiStreamIndex);
            break;
        }
    }

    m_pPreviewStream        = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::PreviewStream);
    m_pRdiStream            = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::RdiStream);
    m_pYuvCBStream          = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::YuvCBStream);
    m_pBlobStream           = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::SnapshotStream);
    m_pYuvInStream          = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::YuvInputStream);

    m_previewPipelineIndex  = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLPreviewRawType);

    if (static_cast<CHISTREAMFORMAT>(HAL_PIXEL_FORMAT_BLOB) != m_pYuvCBStream->format)
    {
        m_YuvCBPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLSnapshotYUVType);
    }
    else
    {
        CHX_LOG_ERROR("expected only YUV request in YuvCB stream");
    }

    ChiPipelineTargetCreateDescriptor* pYuvCBDesc = &m_pChiUsecase->pPipelineTargetCreateDesc[m_YuvCBPipelineIndex];
    pYuvCBDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStream;
    pYuvCBDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pYuvCBStream;

    m_offlineBlobPipelineIndex =
        m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::InternalZSLYuv2JpegType);
    ChiPipelineTargetCreateDescriptor* pBlobDesc = &m_pChiUsecase->pPipelineTargetCreateDesc[m_offlineBlobPipelineIndex];
    pBlobDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pYuvInStream;
    pBlobDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pBlobStream;

    m_offlineYuvPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLYuv2YuvType);
    ChiPipelineTargetCreateDescriptor* pOfflineYuvDesc = &m_pChiUsecase->pPipelineTargetCreateDesc[m_offlineYuvPipelineIndex];
    pOfflineYuvDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pYuvInStream;
    pOfflineYuvDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pYuvCBStream;

    m_realTimeBlobPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLSnapshotJpegType);
    if (TRUE == ExtensionModule::GetInstance()->UseGPURotationUsecase())
    {
        m_realTimeBlobPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType
                                        (AdvancedPipelineType::ZSLSnapshotJpegGPUType);
    }

    ChiPipelineTargetCreateDescriptor* pSnapshotDesc = &m_pChiUsecase->pPipelineTargetCreateDesc[m_realTimeBlobPipelineIndex];
    pSnapshotDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStream;
    pSnapshotDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pBlobStream;


    return m_pChiUsecase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureYuvCB::PipelineCreated
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureYuvCB::PipelineCreated(
    UINT sessionId,    ///< Id of session created
    UINT pipelineId)   ///< Pipeline of session created
{
    (VOID) pipelineId;
    ChiSensorModeInfo*              pSensorInfo = NULL;

    pSensorInfo = m_pUsecase->GetSessionData(sessionId)->pipelines[0].pPipeline->GetSensorModeInfo();

    m_pUsecase->ConfigRdiStream(pSensorInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureYuvCB::ExecuteProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult  FeatureYuvCB::ExecuteProcessRequest(
    camera3_capture_request_t* pRequest)              ///< Request parameters
{
    CDKResult                 result             = CDKResultSuccess;
    camera3_stream_buffer_t   previewBuffers[2]  = {{0}};
    UINT                      previewCount       = 0;
    UINT                      snapshotCount      = 0;
    UINT                      frameNumber        = pRequest->frame_number;
    UINT32                    sensorModeIndex;
    UINT32                    frameIndex         = (pRequest->frame_number % MaxOutstandingRequests);
    UINT                      snapshotReqIdIndex = (m_maxSnapshotReqId % MaxOutstandingRequests);
    BOOL                      bIsRealTimeRequest = TRUE;

    camera3_stream_buffer_t   offlineBuffers[2]  = { { 0 } };
    UINT                      offlineRequestCnt  = 0;
    UINT                      pipelineIndex      = 0;

    m_isSnapshotFrame[frameIndex]       = FALSE;
    m_isRealTimeBlobFrame[frameIndex]   = FALSE;

    m_snapshotInputNum[snapshotReqIdIndex] = 0;

    sensorModeIndex = m_pUsecase->GetSessionData(m_previewPipelineIndex)->pSession->GetSensorModeInfo()->modeIndex;
    ChxUtils::FillTuningModeData(const_cast<camera_metadata_t*>(pRequest->settings),
                                 pRequest,
                                 sensorModeIndex,
                                 m_pUsecase->GetEffectMode(),
                                 m_pUsecase->GetSceneMode(),
                                 m_pUsecase->GetFeature1Mode(),
                                 m_pUsecase->GetFeature2Mode());

    ChxUtils::FillCameraId(const_cast<camera_metadata_t*>(pRequest->settings), m_pUsecase->GetCameraId());

    for (UINT32 i = 0; i < pRequest->num_output_buffers; i++)
    {
        if (NULL == pRequest->input_buffer)
        {
            bIsRealTimeRequest = TRUE;
            if (m_pPreviewStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
            {
                // Capture preview stream
                ChxUtils::Memcpy(&previewBuffers[previewCount], &pRequest->output_buffers[i], sizeof(camera3_stream_buffer_t));
                previewCount++;
            }
            else if ((m_pYuvCBStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream)) ||
                (m_pBlobStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream)))
            {
                // if current request metadata from frameworks in NULL, then use the previous request metadata
                // stored in previous index of m_pSnapshotInputMeta;
                const camera_metadata_t* pRequestMetadata = pRequest->settings;
                if (NULL == pRequestMetadata && m_maxSnapshotReqId > 0)
                {
                    // First request will have valid metadata.
                    pRequestMetadata = const_cast<camera_metadata_t*>
                        (m_pSnapshotInputMeta[(m_maxSnapshotReqId - 1) % MaxOutstandingRequests]);
                }

                if (NULL == m_pSnapshotInputMeta[snapshotReqIdIndex])
                {
                    m_pSnapshotInputMeta[snapshotReqIdIndex] =
                        static_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(pRequestMetadata));
                }
                else
                {
                    ChxUtils::ResetMetadata(m_pSnapshotInputMeta[snapshotReqIdIndex]);
                    if (NULL != pRequestMetadata)
                    {
                        ChxUtils::MergeMetadata(const_cast<camera_metadata_t*>(pRequestMetadata),
                                                m_pSnapshotInputMeta[snapshotReqIdIndex]);
                    }
                }

                ChxUtils::Memcpy(&m_snapshotBuffers[snapshotReqIdIndex][snapshotCount],
                                 &pRequest->output_buffers[i],
                                 sizeof(camera3_stream_buffer_t));
                snapshotCount++;
                if (m_pBlobStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
                {
                    m_isRealTimeBlobFrame[frameIndex]   = TRUE;
                }
            }
            else
            {
                CHX_LOG_ERROR("YuvCB: Not expected stream");
            }
        }
        else if (m_pBlobStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
        {
            ChxUtils::Memcpy(&offlineBuffers[offlineRequestCnt], &pRequest->output_buffers[i],
                sizeof(camera3_stream_buffer_t));
            bIsRealTimeRequest = FALSE;
            offlineRequestCnt++;
            pipelineIndex       = m_offlineBlobPipelineIndex;
            CHX_LOG("YuvCB submitting to offlineBlob %d", pRequest->frame_number);
        }
        else
        {
            ChxUtils::Memcpy(&offlineBuffers[offlineRequestCnt], &pRequest->output_buffers[i],
                sizeof(camera3_stream_buffer_t));
            bIsRealTimeRequest = FALSE;
            offlineRequestCnt++;
            pipelineIndex       = m_offlineYuvPipelineIndex;
            CHX_LOG("YuvCB submitting to offlineYUV %d", pRequest->frame_number);
        }
    }

    if (TRUE == bIsRealTimeRequest)
    {
        PipelineData* pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_previewSessionId, 0));
        UINT          requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);

        pPipelineData->seqIdToFrameNum[requestIdIndex] = frameNumber;

        CHX_LOG("FeatureYuvCB Realtime AppFrameNum to ReqId: %d <--> %d", frameNumber, pPipelineData->seqId);

        ImageBuffer* pImageBuffer = m_pRdiTargetBuffer->pBufferManager->GetImageBuffer();
        if (NULL != pImageBuffer)
        {
            previewBuffers[previewCount].buffer         = pImageBuffer->GetBufferHandle();
            previewBuffers[previewCount].stream         = reinterpret_cast<camera3_stream_t*>(m_pRdiStream);
            previewBuffers[previewCount].acquire_fence  = -1;
            previewCount++;
        }
        else
        {
            CHX_LOG_ERROR("FeatureYUVCb: GetImageBuffer failed, No RDI buffer for frameNumber %d", frameNumber);
        }

        const Session* pSession = m_pUsecase->GetSessionData(m_previewPipelineIndex)->pSession;

        CHICAPTUREREQUEST request = { 0 };
        request.frameNumber       = pPipelineData->seqId++;
        request.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
        request.numOutputs        = previewCount;
        request.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(previewBuffers);
        request.pMetadata         = pRequest->settings;

        request.pPrivData              = &m_privData[request.frameNumber % MaxOutstandingRequests];
        request.pPrivData->featureType = FeatureType::ZSL;

        CHIPIPELINEREQUEST submitRequest = { 0 };
        submitRequest.pSessionHandle     = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
        submitRequest.numRequests        = 1;
        submitRequest.pCaptureRequests   = &request;

        m_pUsecase->SetRequestToFeatureMapping(m_previewPipelineIndex, request.frameNumber, this);

        if (FALSE == pSession->IsPipelineActive())
        {
            if (ExtensionModule::GetInstance()->GetNumPCRsBeforeStreamOn(const_cast<camera_metadata_t*>(pRequest->settings))
                == pRequest->frame_number)
            {
                result = ExtensionModule::GetInstance()->ActivatePipeline(pSession->GetSessionHandle(),
                                                                          pSession->GetPipelineHandle());
                if (CDKResultSuccess == result)
                {
                    pSession->SetPipelineActivateFlag();
                }
            }
        }

        result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

        if (0 < snapshotCount)
        {
            m_pOfflineRequestMutex->Lock();

            m_isSnapshotFrame[frameIndex]                 = TRUE;
            m_snapshotBufferNum[snapshotReqIdIndex]       = snapshotCount;
            m_snapshotReqIdToFrameNum[snapshotReqIdIndex] = frameNumber;
            m_snapshotInputNum[snapshotReqIdIndex]        = frameNumber - ZslShutterLag;
            m_maxSnapshotReqId++;

            m_pOfflineRequestAvailable->Signal();
            m_pOfflineRequestMutex->Unlock();

            CHX_LOG_ERROR("YUV callback request in ZSL");
        }
    }
    else
    {
        CHICAPTUREREQUEST request = { 0 };

        if (NULL != pRequest->input_buffer)
        {
            request.numInputs     = 1;
            request.pInputBuffers = reinterpret_cast<CHISTREAMBUFFER*>(pRequest->input_buffer);
        }

        const Session* pSession   = m_pUsecase->GetSessionData(pipelineIndex)->pSession;
        request.frameNumber       = pRequest->frame_number;
        request.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
        request.numOutputs        = offlineRequestCnt;
        request.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(offlineBuffers);
        request.pMetadata         = pRequest->settings;
        request.pPrivData         = &m_privData[frameIndex];

        CHIPIPELINEREQUEST submitRequest = { 0 };
        submitRequest.pSessionHandle     = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
        submitRequest.numRequests        = 1;
        submitRequest.pCaptureRequests   = &request;

        m_pUsecase->SetRequestToFeatureMapping(pipelineIndex, request.frameNumber, this);

        if (FALSE == pSession->IsPipelineActive())
        {
            result =
                ExtensionModule::GetInstance()->ActivatePipeline(pSession->GetSessionHandle(),
                                                                    pSession->GetPipelineHandle());
            if (CDKResultSuccess == result)
            {
                pSession->SetPipelineActivateFlag();
            }
        }

        result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureYuvCB::ProcessResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureYuvCB::ProcessResult(
    CHICAPTURERESULT*   pResult,
    VOID*               pPrivateCallbackData)
{
    m_pResultMutex->Lock();

    SessionPrivateData* pCbData               = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    BOOL                isAppResultsAvailable = FALSE;
    UINT32              resultFrameNum        = pResult->frameworkFrameNum;
    UINT32              resultFrameIndex      = resultFrameNum % MaxOutstandingRequests;
    UINT32              rtPipelineReqId       = 0;
    BOOL                bReprocessPipeLine    = FALSE;

    if ((m_YuvCBPipelineIndex == pCbData->sessionId) || (m_realTimeBlobPipelineIndex == pCbData->sessionId))
    {
        resultFrameNum   = m_snapshotReqIdToFrameNum[pResult->frameworkFrameNum % MaxOutstandingRequests];
        resultFrameIndex = resultFrameNum % MaxOutstandingRequests;
    }
    else if (m_previewSessionId == pCbData->sessionId)
    {
        PipelineData*     pPipelineData =
            const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_previewSessionId, 0));

        resultFrameNum = pPipelineData->seqIdToFrameNum[pResult->frameworkFrameNum % MaxOutstandingRequests];
        resultFrameIndex = resultFrameNum % MaxOutstandingRequests;

        rtPipelineReqId = resultFrameNum;
        CHX_LOG("FeatureYuvCB ProcessResult Realtime ReqId to AppFrameNum: %d <--> %d", pResult->frameworkFrameNum,
            resultFrameNum);
    }
    else
    {
        bReprocessPipeLine = TRUE;
    }

    camera3_capture_result_t* pUsecaseResult   = m_pUsecase->GetCaptureResult(resultFrameIndex);

    pUsecaseResult->frame_number = resultFrameNum;

    // If result contain metadata and metadata has not been sent to framework
    if ((NULL != pResult->pResultMetadata) && (FALSE == m_pUsecase->IsMetadataSent(resultFrameIndex)))
    {
        BOOL               isResultMetadataAvailable = FALSE;
        UINT64             timestamp                 = m_pUsecase->GetRequestShutterTimestamp(resultFrameNum);
        camera_metadata_t* pMetadata                 =
            const_cast<camera_metadata_t*>(static_cast<const camera_metadata_t*>(pResult->pResultMetadata));
        // If metadata from offline snapshot session or is not a snapshot frame (only preview active) add to current data
        // If it is not a snapshot frame, just add preview metadata into result queue
        if ((m_YuvCBPipelineIndex == pCbData->sessionId) || (m_realTimeBlobPipelineIndex == pCbData->sessionId) ||
            (FALSE == m_isSnapshotFrame[resultFrameIndex]) || (TRUE == bReprocessPipeLine))
        {
            pUsecaseResult->result = static_cast<const camera_metadata_t*>(pResult->pResultMetadata);
            pUsecaseResult->partial_result = pResult->numPartialMetadata;
            isResultMetadataAvailable = TRUE;
        }

        if (m_previewPipelineIndex == pCbData->sessionId)
        {
            UINT32 zslQueueIndex = (rtPipelineReqId % BufferQueueDepth);
            m_pRdiTargetBuffer->pMutex->Lock();
            if (NULL == m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pMetadata)
            {
                m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pMetadata =
                    static_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(pMetadata));
            }
            else
            {
                ChxUtils::ResetMetadata(m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pMetadata);
                ChxUtils::MergeMetadata(pMetadata, m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pMetadata);
            }

            /// @todo This currently copies the debugData pointer, to memory owned by the first pipeline. Should copy
            m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].frameNumber = resultFrameNum;
            m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isMetadataReady = TRUE;

            if ((TRUE == m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isBufferReady) &&
                (TRUE == m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isMetadataReady))
            {
                m_pRdiTargetBuffer->lastReadySequenceID = rtPipelineReqId;
            }

            m_pRdiTargetBuffer->pMutex->Unlock();

            m_pRdiAndMetaResultMutex->Lock();
            m_lastRealtimeMetadataAvailable = resultFrameNum;

            // If offline snapshot is waiting for current metadata, signal it and unblock.
            if (m_offlineRequestWaitingForFrame == resultFrameNum)
            {
                m_pRdiAndMetaResultAvailable->Signal();
            }
            m_pRdiAndMetaResultMutex->Unlock();
        }

        if (TRUE == bReprocessPipeLine)
        {
            m_pUsecase->SetMetadataAvailable(resultFrameIndex);
            isAppResultsAvailable = TRUE;
        }

        // Override snapshot frame sensor timestamp metadata with shutter event for same frame number
        if ((NULL != pMetadata) && (0 != timestamp))
        {
            CHX_LOG("Update sensor timestamp for frame %d, timestamp = %" PRIu64, resultFrameNum, timestamp);

            UINT32                  SensorTimestampTag = 0x000E0010;
            camera_metadata_entry_t entry              = { 0 };

            INT32 status = find_camera_metadata_entry(pMetadata, SensorTimestampTag, &entry);

            if (-ENOENT == status)
            {
                status = add_camera_metadata_entry(pMetadata, SensorTimestampTag, &timestamp, 1);
            }
            else if (0 == status)
            {
                status = update_camera_metadata_entry(pMetadata, entry.index, &timestamp, 1, NULL);
            }

            if (TRUE == isResultMetadataAvailable)
            {
                m_pUsecase->SetMetadataAvailable(resultFrameIndex);
                isAppResultsAvailable = TRUE;
            }
        }

        // check if flash is required for snapshot
        m_isFlashRequired = m_pUsecase->IsFlashRequired(pMetadata);
    }
    if (TRUE == bReprocessPipeLine)
    {
        if (NULL != &pResult->pInputBuffer[0])
        {
            camera3_stream_buffer_t* pResultInBuffer =
                const_cast<camera3_stream_buffer_t*>(pUsecaseResult->input_buffer);
            ChxUtils::Memcpy(pResultInBuffer, pResult->pInputBuffer, sizeof(camera3_stream_buffer_t));
        }
        isAppResultsAvailable = TRUE;
    }

    for (UINT32 j = 0; j < pResult->numOutputBuffers; j++)
    {
        // If our internal stream, copy the result into the target buffer to be consumed by the offline pipeline
        if (m_pRdiStream == pResult->pOutputBuffers[j].pStream)
        {
            UINT32 zslQueueIndex = (rtPipelineReqId % BufferQueueDepth);

            m_pRdiTargetBuffer->pMutex->Lock();

            if (rtPipelineReqId >= BufferQueueDepth)
            {
                m_pRdiTargetBuffer->pBufferManager->ReleaseReference(
                    reinterpret_cast<buffer_handle_t*>(m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pRdiOutputBuffer->phBuffer));
            }

            ChxUtils::Memcpy(m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pRdiOutputBuffer,
                             &pResult->pOutputBuffers[j],
                             sizeof(CHISTREAMBUFFER));

            m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pRdiOutputBuffer->acquireFence = -1;
            m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isBufferReady                  = TRUE;

            if ((TRUE == m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isBufferReady) &&
                (TRUE == m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isMetadataReady))
            {
                m_pRdiTargetBuffer->lastReadySequenceID = rtPipelineReqId;
            }

            m_pRdiTargetBuffer->pMutex->Unlock();

            m_pRdiAndMetaResultMutex->Lock();
            m_lastRdiFrameAvailable = resultFrameNum;

            // If offline snapshot is waiting for current Rdi frame, signal it and unblock.
            if (m_offlineRequestWaitingForFrame == resultFrameNum)
            {
                m_pRdiAndMetaResultAvailable->Signal();
            }
            m_pRdiAndMetaResultMutex->Unlock();
        }
        // Otherwise queue a buffer as part of the normal result
        else
        {
            m_pUsecase->GetAppResultMutex()->Lock();
            camera3_stream_buffer_t* pResultBuffer =
                const_cast<camera3_stream_buffer_t*>(&pUsecaseResult->output_buffers[pUsecaseResult->num_output_buffers++]);

            ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[j], sizeof(camera3_stream_buffer_t));
            m_pUsecase->GetAppResultMutex()->Unlock();

            if (m_YuvCBPipelineIndex == pCbData->sessionId)
            {
                // Release reference to the input buffers of the request to the SnapshotPipeline
                for(UINT32 i = 0; i < pResult->pPrivData->numInputBuffers; i++)
                {
                    if ((NULL != pResult->pPrivData->bufferManagers[i]) && (NULL != pResult->pPrivData->inputBuffers[i]))
                    {
                        reinterpret_cast<CHIBufferManager*>(pResult->pPrivData->bufferManagers[i])->
                            ReleaseReference(reinterpret_cast<buffer_handle_t*>(pResult->pPrivData->inputBuffers[i]));
                        pResult->pPrivData->bufferManagers[i] = NULL;
                        pResult->pPrivData->inputBuffers[i]   = NULL;
                    }
                }
            }

            isAppResultsAvailable = TRUE;
        }
    }

    if (TRUE == isAppResultsAvailable)
    {
        m_pUsecase->ProcessAndReturnFinishedResults();
    }

    m_pResultMutex->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureYuvCB::ProcessMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureYuvCB::ProcessMessage(
    const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{
    (VOID)pMessageDescriptor;
    (VOID)pPrivateCallbackData;
    //@todo this function to be REMOVED
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureYuvCB::IsPseudoZSL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL FeatureYuvCB::IsPseudoZSL(camera_metadata_t* pMetadata)
{
    // It is Pseudo ZSL
    // if application hasn't set the ZSL mode OR
    // If the snapshot is captured with Flash or Manual 3A gains

    UINT zslMode = 0;
    m_pUsecase->GetZSLMode(&zslMode, pMetadata);

    CHX_LOG("zslMode: %d m_isFlashRequired:%d.", zslMode, m_isFlashRequired);

    return ((0 == zslMode) || (TRUE == m_isFlashRequired));
}

INT32 FeatureYuvCB::GetRequiredPipelines(
    AdvancedPipelineType* pPipelines,
    INT32 size)
{
    INT32 count = 0;
    const INT32 pipelineCount = 4;

    if (NULL != pPipelines && size >= pipelineCount)
    {
        INT32 index = 0;
        AdvancedPipelineType pipelineGroup[1];
        UINT                 cameraId[1];
        UINT                 wideCameraId = m_pUsecase->GetPhysicalCameraId(CameraRoleTypeWide);

        ChiStream* pStream = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::YuvCBStream);
        if (static_cast<CHISTREAMFORMAT>(HAL_PIXEL_FORMAT_BLOB) != pStream->format)
        {
            pPipelines[index] = AdvancedPipelineType::ZSLSnapshotYUVType;
        }
        else
        {
            CHX_LOG_ERROR("NOT expected in YUVInBlobOut usecase");
        }

        pipelineGroup[0]    = pPipelines[index];
        cameraId[0]         = wideCameraId;
        m_YuvCBSessionId    = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);

        m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
        index++;

        if (FALSE == m_pUsecase->IsMultiCameraUsecase())
        {
            pPipelines[index]   = AdvancedPipelineType::ZSLPreviewRawType;
            pipelineGroup[0]    = pPipelines[index];
            cameraId[0]         = wideCameraId;
            m_previewSessionId  = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
            m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
            index++;

            pPipelines[index]       = AdvancedPipelineType::InternalZSLYuv2JpegType;
            pipelineGroup[0]        = pPipelines[index];
            cameraId[0]             = wideCameraId;
            m_offlineBlobSessionId  = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
            m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
            index++;

            pPipelines[index]       = AdvancedPipelineType::ZSLYuv2YuvType;
            pipelineGroup[0]        = pPipelines[index];
            cameraId[0]             = wideCameraId;
            m_offlineYuvSessionId   = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
            m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
            index++;

            pPipelines[index]       = AdvancedPipelineType::ZSLSnapshotJpegType;
            if (TRUE == ExtensionModule::GetInstance()->UseGPURotationUsecase())
            {
                pPipelines[index] = AdvancedPipelineType::ZSLSnapshotJpegGPUType;
            }
            pipelineGroup[0]        = pPipelines[index];
            cameraId[0]             = wideCameraId;
            m_offlineYuvSessionId   = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
            m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
            index++;
        }
        else
        {
            CHX_LOG("Don't use Realtime pipeline in advance usecase for multicamera usecase");
        }
        count = index;
    }

    CHX_LOG("FeatureYuvCB::required pipeline count:%d", count);
    return count;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FeatureYuvCB::RequestThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* FeatureYuvCB::RequestThread(
    VOID* pThreadData)
{
    PerThreadData* pPerThreadData = reinterpret_cast<PerThreadData*>(pThreadData);

    FeatureYuvCB* pFeatureYuvCB = reinterpret_cast<FeatureYuvCB*>(pPerThreadData->pPrivateData);

    pFeatureYuvCB->RequestThreadProcessing();

    return NULL;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FeatureYuvCB::RequestThreadProcessing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureYuvCB::RequestThreadProcessing()
{
    CDKResult result = CDKResultSuccess;
    CHX_LOG_INFO("RequestThreadProcessing Entered");
    while (TRUE)
    {
        m_pOfflineRequestMutex->Lock();
        if (m_offlineRequestProcessTerminate == FALSE)
        {
            m_pOfflineRequestAvailable->Wait(m_pOfflineRequestMutex->GetNativeHandle());
        }
        m_pOfflineRequestMutex->Unlock();

        if (m_offlineRequestProcessTerminate == TRUE)
        {
            break;
        }

        while (m_maxSnapshotReqId > m_snapshotReqId)
        {
            UINT   requestIdIndex    = (m_snapshotReqId % MaxOutstandingRequests);
            UINT   frameNumber       = m_snapshotReqIdToFrameNum[requestIdIndex];
            UINT32 requestFrameIndex = frameNumber % MaxOutstandingRequests;

            // Check again, If we have the requirements for the snapshot frame for the corresponding frameNumber
            if (TRUE == m_isSnapshotFrame[requestFrameIndex])
            {
                UINT                    zslInputFrameNumber = frameNumber;
                camera3_stream_buffer_t snapshotInputBuffer = { 0 };

                UINT32 activePipelineIndex;

                if (TRUE == m_isRealTimeBlobFrame[requestFrameIndex])
                {
                    activePipelineIndex = m_realTimeBlobPipelineIndex;
                }
                else
                {
                    activePipelineIndex = m_YuvCBPipelineIndex;
                }

                // If ZSLmode is enabled, give the recently available SOF frame as input
                if (FALSE == IsPseudoZSL(m_pSnapshotInputMeta[requestIdIndex]))
                {
                    zslInputFrameNumber = (ZslShutterLag < frameNumber) ? m_snapshotInputNum[requestIdIndex] : 0;
                    CHX_LOG_INFO("zslInputFrameNumber set in Non PsuedoZsl case = %u", zslInputFrameNumber);
                }

                // Check whether realtime metadata and Rdi buffer for zslInputFrameNumber are available.
                // If available, go ahead with offline request submission for snapshot.
                // Otherwise, continue to while loop.
                m_pRdiAndMetaResultMutex->Lock();
                if ((m_lastRdiFrameAvailable < zslInputFrameNumber || m_lastRealtimeMetadataAvailable < zslInputFrameNumber) ||
                    (InvalidFrameNum == m_lastRdiFrameAvailable  || InvalidFrameNum == m_lastRealtimeMetadataAvailable))
                {
                    m_offlineRequestWaitingForFrame = zslInputFrameNumber;
                    CHX_LOG_INFO("waiting for rdi and meta of frame %d for triggering snapshot request", zslInputFrameNumber);
                    result = m_pRdiAndMetaResultAvailable->Wait(m_pRdiAndMetaResultMutex->GetNativeHandle());
                    m_pRdiAndMetaResultMutex->Unlock();
                    continue;
                }
                m_pRdiAndMetaResultMutex->Unlock();

                // Create copy so that future updates to input metadata don't free it
                camera_metadata_t*      pSnapshotMetadata =
                    allocate_copy_camera_metadata_checked(m_pSnapshotInputMeta[requestIdIndex],
                                                          get_camera_metadata_size(m_pSnapshotInputMeta[requestIdIndex]));

                m_pUsecase->GetTargetBuffer(zslInputFrameNumber,
                                            m_pRdiTargetBuffer,
                                            pSnapshotMetadata,
                                            &snapshotInputBuffer);

                const Session* pSession = m_pUsecase->GetSessionData(activePipelineIndex)->pSession;

                CHICAPTUREREQUEST snapshotRequest = { 0 };
                snapshotRequest.frameNumber       = m_snapshotReqId++;
                snapshotRequest.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
                snapshotRequest.numInputs         = 1;
                snapshotRequest.numOutputs        = m_snapshotBufferNum[requestIdIndex];
                snapshotRequest.pInputBuffers     = reinterpret_cast<CHISTREAMBUFFER*>(&snapshotInputBuffer);
                snapshotRequest.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(m_snapshotBuffers[requestIdIndex]);
                snapshotRequest.pMetadata         = pSnapshotMetadata;
                snapshotRequest.pPrivData         = &m_privData[snapshotRequest.frameNumber % MaxOutstandingRequests];
                snapshotRequest.pPrivData->featureType = FeatureType::ZSL;

                // Save input buffers info for later releasing reference
                snapshotRequest.pPrivData->inputBuffers[0]      = snapshotInputBuffer.buffer;
                snapshotRequest.pPrivData->bufferManagers[0]    = m_pRdiTargetBuffer->pBufferManager;
                snapshotRequest.pPrivData->numInputBuffers      = 1;

                CHIPIPELINEREQUEST submitRequest = { 0 };
                submitRequest.pSessionHandle     = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
                submitRequest.numRequests        = 1;
                submitRequest.pCaptureRequests   = &snapshotRequest;

                if (FALSE == pSession->IsPipelineActive())
                {
                    CDKResult result = CDKResultSuccess;

                    result = ExtensionModule::GetInstance()->ActivatePipeline(pSession->GetSessionHandle(),
                                                                              pSession->GetPipelineHandle());
                    if (CDKResultSuccess == result)
                    {
                        pSession->SetPipelineActivateFlag();
                    }
                }

                CHX_LOG_INFO("Sending ZSL snapshot frameNumber:%d, request:%" PRIu64 " metadata:%p",
                             frameNumber, snapshotRequest.frameNumber, pSnapshotMetadata);
                m_pUsecase->SetRequestToFeatureMapping(activePipelineIndex, snapshotRequest.frameNumber, this);
                ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

                free_camera_metadata(pSnapshotMetadata);
                pSnapshotMetadata = NULL;
            }
        }
    }
    CHX_LOG_INFO("RequestThreadProcessing Exited");
}
