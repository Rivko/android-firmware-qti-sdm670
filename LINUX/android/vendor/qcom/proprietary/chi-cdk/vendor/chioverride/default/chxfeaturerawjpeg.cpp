////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeaturerawjpeg.cpp
/// @brief CHX raw + jpeg(simultaneous raw and jpeg capture) feature class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxadvancedcamerausecase.h"
#include "chxincs.h"
#include "chxfeaturerawjpeg.h"
#include "chxusecase.h"

static const UINT InvalidFrameNum  = 0xFFFFFFFF;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureRawJPEG::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FeatureRawJPEG* FeatureRawJPEG::Create(
    AdvancedCameraUsecase* pUsecase)
{
    FeatureRawJPEG* pFeature = CHX_NEW FeatureRawJPEG;

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
/// FeatureRawJPEG::Pause
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureRawJPEG::Pause(
        BOOL isForced)
{
    CHX_LOG("Feature Pause isForced =%d", isForced);
    ChxUtils::AtomicStoreU32(&m_aPauseInProgress, TRUE);
    m_pRdiAndMetaResultMutex->Lock();
    m_pRdiAndMetaResultAvailable->Signal();
    m_pRdiAndMetaResultMutex->Unlock();

    m_pOfflineRequestMutex->Lock();
    m_offlineRequestProcessTerminate = TRUE;
    m_pOfflineRequestAvailable->Signal();
    m_pOfflineRequestMutex->Unlock();
    ChxUtils::ThreadTerminate(m_offlineRequestProcessThread.hThreadHandle);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureRawJPEG::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureRawJPEG::Flush(
        BOOL isInProgress)
{
    CHX_LOG("Feature Flush isInProgress =%d", isInProgress);
    if(TRUE == isInProgress)
    {
        ChxUtils::AtomicStoreU32(&m_aFlushInProgress, TRUE);
    }
    else
    {
        ChxUtils::AtomicStoreU32(&m_aFlushInProgress, FALSE);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureRawJPEG::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureRawJPEG::Initialize(
    AdvancedCameraUsecase* pUsecase)
{
    CDKResult    result = CDKResultSuccess;

    m_pUsecase     = pUsecase;
    m_pResultMutex = Mutex::Create();
    m_pRdiStream   = static_cast<CHISTREAM*>(CHX_CALLOC(sizeof(CHISTREAM)));

    m_lastRdiFrameAvailable          = InvalidFrameNum;
    m_lastRealtimeMetadataAvailable  = InvalidFrameNum;
    m_pRdiAndMetaResultMutex         = Mutex::Create();
    m_pRdiAndMetaResultAvailable     = Condition::Create();
    m_offlineRequestWaitingForFrame  = InvalidFrameNum;

    m_pOfflineRequestMutex           = Mutex::Create();
    m_pOfflineRequestAvailable       = Condition::Create();
    m_offlineRequestProcessTerminate = FALSE;
    m_aPauseInProgress               = FALSE;
    m_aFlushInProgress               = FALSE;

    m_offlineRequestProcessThread.pPrivateData = this;

    result = ChxUtils::ThreadCreate(FeatureRawJPEG::RequestThread,
                                    &m_offlineRequestProcessThread,
                                    &m_offlineRequestProcessThread.hThreadHandle);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureRawJPEG::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureRawJPEG::Destroy(BOOL isForced)
{

    CHX_LOG("FeatureRawJPEG::Destroy(), isForced %d E.", isForced);

    m_pSnapshotStream     = NULL;
    m_pRdiFrameworkStream = NULL;

    if ((TRUE == m_useDummyPreview) && (NULL != m_pPreviewStream))
    {
        CHX_FREE(m_pPreviewStream);
        m_pPreviewStream = NULL;
    }
    else
    {
        m_pPreviewStream = NULL;
    }

    if (NULL != m_pResultMutex)
    {
        m_pResultMutex->Destroy();
        m_pResultMutex = NULL;
    }
    if (NULL != m_pRdiStream)
    {
        CHX_FREE(m_pRdiStream);
        m_pRdiStream = NULL;
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

    if (NULL != m_pChiUsecase)
    {
        CHX_FREE(m_pChiUsecase->pPipelineTargetCreateDesc);
        m_pChiUsecase->pPipelineTargetCreateDesc = NULL;
        CHX_FREE(m_pChiUsecase);
        m_pChiUsecase = NULL;
    }

    for (UINT32 i = 0; i < MaxOutstandingRequests; i++)
    {
        if (NULL != m_pSnapshotInputMeta[i])
        {
            free_camera_metadata(m_pSnapshotInputMeta[i]);
            m_pSnapshotInputMeta[i] = NULL;
        }
    }

    ChxUtils::AtomicStoreU32(&m_aPauseInProgress, FALSE);
    ChxUtils::AtomicStoreU32(&m_aFlushInProgress, FALSE);
    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureRawJPEG::OverrideUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiUsecase* FeatureRawJPEG::OverrideUsecase(
    LogicalCameraInfo*              pCameraInfo,
    camera3_stream_configuration_t* pStreamConfig)
{
    CHX_LOG("Initializing usecase for RawJPEG feature, num_streams:%d", pStreamConfig->num_streams);

    /// @todo - Split up the ZSL pipelines in XML so we can use the default matching
    ChiUsecase*           pZslUsecase        = &Usecases27Target[UsecaseId27Target::UsecaseZSLId];

    UsecaseZSLPipelineIds snapshotPipelineId = UsecaseZSLPipelineIds::ZSLSnapshotJpeg;

    m_isRdiFormatRaw16 = FALSE;
    for (UINT32 i = 0; i < pStreamConfig->num_streams; i++)
    {
        INT format = pStreamConfig->streams[i]->format;
        if (HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED == format ||
            HAL_PIXEL_FORMAT_YCbCr_420_888          == format)
        {
            m_pPreviewStream  = reinterpret_cast<ChiStream*>(pStreamConfig->streams[i]);
        }
        else if (HAL_PIXEL_FORMAT_BLOB == format)
        {
            m_pSnapshotStream  = reinterpret_cast<ChiStream*>(pStreamConfig->streams[i]);
        }
        else if (HAL_PIXEL_FORMAT_RAW10 == format || HAL_PIXEL_FORMAT_RAW16 == format)
        {
            // Save this RDI stream from frameworks.
            // Used later to send the result back to frameworks.
            m_pRdiFrameworkStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[i]);
            if (HAL_PIXEL_FORMAT_RAW16 == format)
            {
                m_isRdiFormatRaw16 = TRUE;
            }
            m_pRdiFrameworkStream->maxNumBuffers = 8;

            // Config the RDI size, this decide the sensor output size.
            // For the JPEG+RAW, the sensor output size decided by the RAW stream size.
            m_pRdiStream->width = pStreamConfig->streams[i]->width;
            m_pRdiStream->height = pStreamConfig->streams[i]->height;
            CHX_LOG("FeatureRawJPEG config RDI stream w*h: %dx%d", m_pRdiStream->width,m_pRdiStream->height);
        }
    }

    if (NULL == m_pPreviewStream)
    {
        CHX_LOG_INFO("preview stream is not configured, use dummy preview stream.");

        m_useDummyPreview = TRUE;
        m_pPreviewStream  = static_cast<CHISTREAM*>(CHX_CALLOC(sizeof(CHISTREAM)));

        if (NULL != m_pPreviewStream)
        {
            m_pPreviewStream->format        = ChiStreamFormatImplDefined;
            m_pPreviewStream->width         = DefaultPreviewWidth;
            m_pPreviewStream->height        = DefaultPreviewHeight;
            m_pPreviewStream->streamType    = ChiStreamTypeOutput;
            m_pPreviewStream->grallocUsage  = 0x00000100;
            m_pPreviewStream->maxNumBuffers = MaxStreamBuffers;
        }
    }

    m_pChiUsecase = static_cast<ChiUsecase*>(CHX_CALLOC(sizeof(ChiUsecase)));
    if (NULL == m_pChiUsecase)
    {
        return m_pChiUsecase;
    }
    m_pChiUsecase->pPipelineTargetCreateDesc = static_cast<ChiPipelineTargetCreateDescriptor*>
                (CHX_CALLOC(2 * sizeof(ChiPipelineTargetCreateDescriptor)));

    m_pChiUsecase->pUsecaseName     = pZslUsecase->pUsecaseName;
    m_pChiUsecase->streamConfigMode = pZslUsecase->streamConfigMode;
    m_pChiUsecase->numTargets       = pZslUsecase->numTargets;
    m_pChiUsecase->ppChiTargets     = pZslUsecase->ppChiTargets;
    m_pChiUsecase->numPipelines     = 2; // Override pipeline count
    if (NULL != m_pChiUsecase->pPipelineTargetCreateDesc)
    {
        m_pChiUsecase->pPipelineTargetCreateDesc[PreviewPipelineIndex] =
            pZslUsecase->pPipelineTargetCreateDesc[UsecaseZSLPipelineIds::ZSLPreviewRaw];
        m_pChiUsecase->pPipelineTargetCreateDesc[SnapshotPipelineIndex] =
            pZslUsecase->pPipelineTargetCreateDesc[snapshotPipelineId];
    }
    UINT cameraIds[2] = {pCameraInfo->ppDeviceInfo[0]->cameraId, pCameraInfo->ppDeviceInfo[0]->cameraId};
    m_pUsecase->SetPipelineToCameraMapping(2, cameraIds);

    for (UINT target = 0; target < m_pChiUsecase->numTargets; target++)
    {
        if (ChiFormatRawMIPI    == m_pChiUsecase->ppChiTargets[target]->pBufferFormats[0] ||
            ChiFormatRawPlain16 == m_pChiUsecase->ppChiTargets[target]->pBufferFormats[0])
        {
            m_rdiStreamIndex   = target;
            m_pRdiTargetBuffer = m_pUsecase->GetTargetBuffer(m_rdiStreamIndex);
            break;
        }
    }
    ChiPipelineTargetCreateDescriptor* pSnapshotDesc = NULL;
    ChiPipelineTargetCreateDescriptor* pPreviewDesc =  NULL;
    if (NULL != m_pChiUsecase->pPipelineTargetCreateDesc)
    {
        pSnapshotDesc = &m_pChiUsecase->pPipelineTargetCreateDesc[SnapshotPipelineIndex];
        pPreviewDesc  = &m_pChiUsecase->pPipelineTargetCreateDesc[PreviewPipelineIndex];
    }
    if ((NULL != pSnapshotDesc) && (NULL != pPreviewDesc))
    {
        pSnapshotDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStream;
        pSnapshotDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pSnapshotStream;

        // Assuming the Raw target has only one format.
        CHX_ASSERT((1 == pPreviewDesc->sinkTarget.pTargetPortDesc[0].pTarget->numFormats) ||
                   (1 == pPreviewDesc->sinkTarget.pTargetPortDesc[1].pTarget->numFormats));

        // Setup the raw target with the RDI stream. Raw has only one buffer format, it will either match here, or
        // not matter in the output format because none of its will match (another assumption)
        if (ChiFormatRawMIPI    == pPreviewDesc->sinkTarget.pTargetPortDesc[0].pTarget->pBufferFormats[0] ||
            ChiFormatRawPlain16 == pPreviewDesc->sinkTarget.pTargetPortDesc[0].pTarget->pBufferFormats[0])
        {
            pPreviewDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStream;
            pPreviewDesc->sinkTarget.pTargetPortDesc[1].pTarget->pChiStream = m_pPreviewStream;
        }
        else
        {
            pPreviewDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pPreviewStream;
            pPreviewDesc->sinkTarget.pTargetPortDesc[1].pTarget->pChiStream = m_pRdiStream;
        }
    }
    return m_pChiUsecase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureRawJPEG::PipelineCreated
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureRawJPEG::PipelineCreated(
    UINT sessionId,    ///< Id of session created
    UINT pipelineId)   ///< Pipeline of session created
{
    (VOID) sessionId;
    (VOID) pipelineId;

    if (TRUE == m_isRdiFormatRaw16)
    {
        m_pRdiStream->format = ChiStreamFormatRaw16;
    }
    else
    {
        m_pRdiStream->format = ChiStreamFormatRaw10;
    }

    // For the JPEG+RAW, RDI stream output based on raw stream size configure from APP
    // so, will not update the RDI stream output here,it have been initialized in the OverrideUsecase
    m_pRdiStream->maxNumBuffers = 0;
    m_pRdiStream->rotation      = StreamRotationCCW90;
    m_pRdiStream->streamType    = ChiStreamTypeOutput;
    m_pRdiStream->grallocUsage  = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureRawJPEG::ExecuteProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult  FeatureRawJPEG::ExecuteProcessRequest(
    camera3_capture_request_t* pRequest)              ///< Request parameters
{
    CDKResult                 result             = CDKResultSuccess;
    camera3_stream_buffer_t   previewBuffers[2]  = {{0}};
    UINT                      previewCount       = 0;
    UINT                      snapshotCount      = 0;
    UINT                      frameNumber        = pRequest->frame_number;
    UINT32                    frameIndex         = (pRequest->frame_number % MaxOutstandingRequests);
    UINT                      snapshotReqIdIndex = (m_maxSnapshotReqId % MaxOutstandingRequests);
    camera3_capture_result_t* pUsecaseResult     = m_pUsecase->GetCaptureResult(frameIndex);
    UINT32                    sensorModeIndex;

    pUsecaseResult->result             = NULL;
    pUsecaseResult->frame_number       = pRequest->frame_number;
    pUsecaseResult->num_output_buffers = 0;

    m_isSnapshotFrame[frameIndex]          = FALSE;
    m_isRdiFrameRequested[frameIndex]      = FALSE;
    m_shutterTimestamp[frameIndex]         = 0;
    ChxUtils::Memset(&m_pRdiFrameworkStreamBuffer[frameIndex], 0, sizeof(camera3_stream_buffer_t));



    sensorModeIndex = m_pUsecase->GetSessionData(PreviewPipelineIndex)->pSession->GetSensorModeInfo()->modeIndex;
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
        if (m_pPreviewStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
        {
            // Capture preview stream
            ChxUtils::Memcpy(&previewBuffers[previewCount], &pRequest->output_buffers[i], sizeof(camera3_stream_buffer_t));
            previewCount++;
        }

        if (m_pSnapshotStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
        {
            if (NULL != m_pSnapshotInputMeta[snapshotReqIdIndex])
            {
                free_camera_metadata(m_pSnapshotInputMeta[snapshotReqIdIndex]);
                m_pSnapshotInputMeta[snapshotReqIdIndex] = NULL;
            }

            // Cache the settings for the reprocess. Settings will be freed by framework as soon as this function returns
            m_pSnapshotInputMeta[snapshotReqIdIndex] =
                allocate_copy_camera_metadata_checked(pRequest->settings, get_camera_metadata_size(pRequest->settings));

            CHX_LOG("Snapshot Frame %d", pRequest->frame_number);
            ChxUtils::Memcpy(&m_snapshotBuffers[snapshotReqIdIndex][snapshotCount],
                             &pRequest->output_buffers[i],
                             sizeof(camera3_stream_buffer_t));
            snapshotCount++;
        }

        if (m_pRdiFrameworkStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
        {
            ChxUtils::Memcpy(&m_pRdiFrameworkStreamBuffer[frameIndex],
                             &pRequest->output_buffers[i],
                             sizeof(camera3_stream_buffer_t));

            m_isRdiFrameRequested[frameIndex] = TRUE;
        }

    }

    // update capture request for raw frame, if there is a request for either Rdi frame or snapshot frame.
    if (TRUE == m_isRdiFrameRequested[frameIndex] || snapshotCount > 0)
    {
        if (TRUE == m_isRdiFrameRequested[frameIndex])
        {
            // If there is buffer provided by framework for Rdi, then
            // override with framework's stream buffer, Raw buffer will be directly updated on this.
            previewBuffers[previewCount].buffer         = m_pRdiFrameworkStreamBuffer[frameIndex].buffer;
            previewBuffers[previewCount].stream         = reinterpret_cast<camera3_stream_t*>(m_pRdiStream);
            previewBuffers[previewCount].acquire_fence  = -1;
            previewCount++;
        }
        else
        {
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
                CHX_LOG_ERROR("FeatureRawJpeg: GetImageBuffer failed, No RDI buffer for frameNumber %d", frameNumber);
            }
        }
    }

    const Session *pSession = m_pUsecase->GetSessionData(PreviewPipelineIndex)->pSession;
    CHICAPTUREREQUEST request = { 0 };
    request.frameNumber       = pRequest->frame_number;
    request.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
    request.numOutputs        = previewCount;
    request.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(previewBuffers);
    request.pMetadata         = pRequest->settings;

    CHIPIPELINEREQUEST submitRequest = { 0 };
    submitRequest.pSessionHandle     = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
    submitRequest.numRequests        = 1;
    submitRequest.pCaptureRequests   = &request;

    if (FALSE == pSession->IsPipelineActive())
    {
        if (ExtensionModule::GetInstance()->GetNumPCRsBeforeStreamOn(const_cast<camera_metadata_t*>(
            pRequest->settings)) == pRequest->frame_number)
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
        m_maxSnapshotReqId++;

        m_pOfflineRequestAvailable->Signal();
        m_pOfflineRequestMutex->Unlock();
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureRawJPEG::ProcessResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureRawJPEG::ProcessResult(
    CHICAPTURERESULT*   pResult,
    VOID*               pPrivateCallbackData)
{
    if ((TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aPauseInProgress))) ||
        (TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aFlushInProgress))))
    {
         CHX_LOG_INFO("ZSL process result return because of cleanup");
         return;
    }
    m_pResultMutex->Lock();

    SessionPrivateData* pCbData               = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    BOOL                isAppResultsAvailable = FALSE;
    UINT32              resultFrameNum        = pResult->frameworkFrameNum;
    UINT32              resultFrameIndex      = resultFrameNum % MaxOutstandingRequests;

    if (SnapshotPipelineIndex == pCbData->sessionId)
    {
        resultFrameNum   = m_snapshotReqIdToFrameNum[pResult->frameworkFrameNum % MaxOutstandingRequests];
        resultFrameIndex = resultFrameNum % MaxOutstandingRequests;
        CHX_LOG("Processing result for snapshot frame %d. Metadata: %p NumBuffers: %d Timestamp: %" PRIu64 " Sent: %d",
                resultFrameNum, pResult->pResultMetadata, pResult->numOutputBuffers, m_shutterTimestamp[resultFrameIndex],
                m_pUsecase->IsMetadataSent(resultFrameIndex));
    }

    camera3_capture_result_t* pUsecaseResult   = m_pUsecase->GetCaptureResult(resultFrameIndex);

    pUsecaseResult->frame_number = resultFrameNum;

    // If result contain metadata and metadata has not been sent to framework
    if ((NULL != pResult->pResultMetadata) && (FALSE == m_pUsecase->IsMetadataSent(resultFrameIndex)))
    {
        BOOL               isResultMetadataAvailable = FALSE;
        UINT64             timestamp                 = m_shutterTimestamp[resultFrameIndex];
        camera_metadata_t* pMetadata                 =
            const_cast<camera_metadata_t*>(static_cast<const camera_metadata_t*>(pResult->pResultMetadata));
        // If metadata from offline snapshot session or is not a snapshot frame (only preview active) add to current data
        // If it is not a snapshot frame, just add preview metadata into result queue
        if ((SnapshotPipelineIndex == pCbData->sessionId) || (FALSE == m_isSnapshotFrame[resultFrameIndex]))
        {
            pUsecaseResult->result = static_cast<const camera_metadata_t*>(pResult->pResultMetadata);
            pUsecaseResult->partial_result = pResult->numPartialMetadata;
            isResultMetadataAvailable = TRUE;
        }

        if(SnapshotPipelineIndex == pCbData->sessionId &&
           TRUE                  == m_isRdiFrameRequested[resultFrameIndex])
        {
            m_pUsecase->GetAppResultMutex()->Lock();
            // Send the Raw buffer result here after we get corresponding jpeg snapshot
            camera3_stream_buffer_t* pResultBuffer =
                const_cast<camera3_stream_buffer_t*>(&pUsecaseResult->output_buffers[pUsecaseResult->num_output_buffers++]);

            ChxUtils::Memcpy(pResultBuffer, &m_pRdiFrameworkStreamBuffer[resultFrameIndex], sizeof(camera3_stream_buffer_t));
            m_pUsecase->GetAppResultMutex()->Unlock();

            isAppResultsAvailable = TRUE;
        }

        if (PreviewPipelineIndex == pCbData->sessionId)
        {
            UINT32 rawJPEGQueueIndex = (resultFrameNum % BufferQueueDepth);

            if (NULL != m_pRdiTargetBuffer->bufferQueue[rawJPEGQueueIndex].pMetadata)
            {
                free_camera_metadata(m_pRdiTargetBuffer->bufferQueue[rawJPEGQueueIndex].pMetadata);
                m_pRdiTargetBuffer->bufferQueue[rawJPEGQueueIndex].pMetadata = NULL;
            }

            /// @todo This currently copies the debugData pointer, to memory owned by the first pipeline. Should copy
            m_pRdiTargetBuffer->bufferQueue[rawJPEGQueueIndex].frameNumber = pResult->frameworkFrameNum;
            // Create copy because this pointer could be freed by app as soon as result returned
            m_pRdiTargetBuffer->bufferQueue[rawJPEGQueueIndex].pMetadata   =
                allocate_copy_camera_metadata_checked(pMetadata, get_camera_metadata_size(pMetadata));

            m_pRdiAndMetaResultMutex->Lock();
            m_lastRealtimeMetadataAvailable    = resultFrameNum;

            // If offline snapshot is waiting for current metadata, signal it and unblock.
            if (m_offlineRequestWaitingForFrame == resultFrameNum)
            {
                m_pRdiAndMetaResultAvailable->Signal();
            }
            m_pRdiAndMetaResultMutex->Unlock();
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
    }

    for (UINT32 j = 0; j < pResult->numOutputBuffers; j++)
    {
        // If our internal stream, copy the result into the target buffer to be consumed by the offline pipeline
        if (m_pRdiStream == pResult->pOutputBuffers[j].pStream &&
            TRUE         == m_isSnapshotFrame[resultFrameIndex])
        {
            UINT32 rawJPEGQueueIndex = (resultFrameNum % BufferQueueDepth);

            // Release reference to the buffer that's going to be replaced out of the queue
            if (resultFrameNum >= BufferQueueDepth)
            {
                m_pRdiTargetBuffer->pBufferManager->ReleaseReference(
                    reinterpret_cast<buffer_handle_t*>(m_pRdiTargetBuffer->bufferQueue[rawJPEGQueueIndex].pRdiOutputBuffer->phBuffer));
            }

            ChxUtils::Memcpy(m_pRdiTargetBuffer->bufferQueue[rawJPEGQueueIndex].pRdiOutputBuffer,
                             &pResult->pOutputBuffers[j],
                             sizeof(CHISTREAMBUFFER));

            m_pRdiTargetBuffer->bufferQueue[rawJPEGQueueIndex].pRdiOutputBuffer->acquireFence = -1;

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

            if (m_pRdiStream == pResult->pOutputBuffers[j].pStream)
            {
                // Send the Rdi frame from here for realtime pipeline. i.e. if there is no request for snapshot.
                ChxUtils::Memcpy(pResultBuffer, &m_pRdiFrameworkStreamBuffer[resultFrameIndex], sizeof(camera3_stream_buffer_t));
            }
            else
            {
                ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[j], sizeof(camera3_stream_buffer_t));

                if (SnapshotPipelineIndex == pCbData->sessionId)
                {
                    // Release reference to the input buffers of the request to SnapshotPipeline
                    for(UINT32 i = 0; i < pResult->pPrivData->numInputBuffers; i++)
                    {
                        if ((NULL != pResult->pPrivData->bufferManagers[i]) &&
                            (NULL != pResult->pPrivData->inputBuffers[i]))
                        {
                            reinterpret_cast<CHIBufferManager*>(pResult->pPrivData->bufferManagers[i])->
                                ReleaseReference(reinterpret_cast<buffer_handle_t*>(pResult->pPrivData->inputBuffers[i]));
                            pResult->pPrivData->bufferManagers[i] = NULL;
                            pResult->pPrivData->inputBuffers[i]   = NULL;
                        }
                    }
                }
            }
            m_pUsecase->GetAppResultMutex()->Unlock();

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
/// FeatureRawJPEG::ProcessMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureRawJPEG::ProcessMessage(
    const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{
    m_pResultMutex->Lock();

    if (ChiMessageTypeSof == pMessageDescriptor->messageType)
    {
        // SOF notifications are not sent to the HAL3 application
        CHX_LOG("RawJPEG Chi Notify SOF frameNum %u framework frameNum %u, timestamp %" PRIu64,
            pMessageDescriptor->message.sofMessage.sofId,
            pMessageDescriptor->message.sofMessage.frameworkFrameNum,
            pMessageDescriptor->message.sofMessage.timestamp);
    }
    else
    {
        SessionPrivateData* pCbData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
        BOOL                dropCb  = FALSE;

        if (ChiMessageTypeShutter == pMessageDescriptor->messageType)
        {
            if (SnapshotPipelineIndex == pCbData->sessionId)
            {
                dropCb = TRUE;
            }
            else
            {
                UINT32 frameNumIndex = (pMessageDescriptor->message.shutterMessage.frameworkFrameNum % MaxOutstandingRequests);
                m_shutterTimestamp[frameNumIndex] = pMessageDescriptor->message.shutterMessage.timestamp;
            }
        }

        if (FALSE == dropCb)
        {
            m_pUsecase->ReturnFrameworkMessage((camera3_notify_msg_t*)pMessageDescriptor, m_pUsecase->GetCameraId());
        }
    }

    m_pResultMutex->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FeatureRawJPEG::RequestThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* FeatureRawJPEG::RequestThread(
    VOID* pThreadData)
{
    PerThreadData* pPerThreadData = reinterpret_cast<PerThreadData*>(pThreadData);

    FeatureRawJPEG* pFeatureRawJPEG = reinterpret_cast<FeatureRawJPEG*>(pPerThreadData->pPrivateData);

    pFeatureRawJPEG->RequestThreadProcessing();

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FeatureRawJPEG::RequestThreadProcessing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureRawJPEG::RequestThreadProcessing()
{
    CDKResult result = CDKResultSuccess;
    CHX_LOG_INFO("FeatureRawJPEG offline RequestThreadProcessing Entered");
    while (1)
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
            UINT                requestIdIndex    = (m_snapshotReqId % MaxOutstandingRequests);
            UINT                frameNumber       = m_snapshotReqIdToFrameNum[requestIdIndex];
            UINT32              requestFrameIndex = frameNumber % MaxOutstandingRequests;

            // Check again, If we have the requirements for the snapshot frame for the corresponding frameNumber
            if (TRUE == m_isSnapshotFrame[requestFrameIndex])
            {
                UINT                    rawJPEGInputFrame   = frameNumber;
                camera3_stream_buffer_t snapshotInputBuffer = { 0 };

                // Create copy so that future updates to input metadata don't free it
                camera_metadata_t*      pSnapshotMetadata   =
                    allocate_copy_camera_metadata_checked(m_pSnapshotInputMeta[requestIdIndex],
                                                          get_camera_metadata_size(m_pSnapshotInputMeta[requestIdIndex]));

                // Check whether realtime metadata and Rdi buffer for rawJPEGInputFrame are available.
                // If available, go ahead with offline request submission for snapshot.
                // Otherwise, continue to while loop.
                m_pRdiAndMetaResultMutex->Lock();
                if ((m_lastRdiFrameAvailable < rawJPEGInputFrame || m_lastRealtimeMetadataAvailable < rawJPEGInputFrame) ||
                    (InvalidFrameNum == m_lastRdiFrameAvailable  || InvalidFrameNum == m_lastRealtimeMetadataAvailable))
                {
                    m_offlineRequestWaitingForFrame = rawJPEGInputFrame;
                    CHX_LOG_INFO("waiting for rdi and meta of frame %d for triggering snapshot request. Last known rdi %d and meta %d",
                        rawJPEGInputFrame, m_lastRdiFrameAvailable, m_lastRealtimeMetadataAvailable);
                    result = m_pRdiAndMetaResultAvailable->Wait(m_pRdiAndMetaResultMutex->GetNativeHandle());
                    m_pRdiAndMetaResultMutex->Unlock();
                    free_camera_metadata(pSnapshotMetadata);
                    pSnapshotMetadata = NULL;

                    if (TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aPauseInProgress)))
                    {
                        CHX_LOG_INFO("Destroy is in progress come out of loop");
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
                m_pRdiAndMetaResultMutex->Unlock();

                m_pUsecase->GetTargetBuffer(rawJPEGInputFrame,
                                            m_pRdiTargetBuffer,
                                            pSnapshotMetadata,
                                            &snapshotInputBuffer);
                if (TRUE == m_isRdiFrameRequested[requestFrameIndex])
                {
                    // If there is buffer provided by framework for Rdi, then
                    // Override Input buffer with framework's stream buffer.
                    // It was already filled by Raw stream in realtime pipeline.
                    snapshotInputBuffer.buffer = m_pRdiFrameworkStreamBuffer[requestFrameIndex].buffer;
                }

                const Session* pSession = m_pUsecase->GetSessionData(SnapshotPipelineIndex)->pSession;

                CHICAPTUREREQUEST snapshotRequest = { 0 };
                snapshotRequest.frameNumber       = m_snapshotReqId++;
                snapshotRequest.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
                snapshotRequest.numInputs         = 1;
                snapshotRequest.numOutputs        = m_snapshotBufferNum[requestIdIndex];
                snapshotRequest.pInputBuffers     = reinterpret_cast<CHISTREAMBUFFER*>(&snapshotInputBuffer);
                snapshotRequest.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(m_snapshotBuffers[requestIdIndex]);
                snapshotRequest.pMetadata         = pSnapshotMetadata;

                // Save input buffers info for later releasing reference
                snapshotRequest.pPrivData = &m_privData[snapshotRequest.frameNumber % MaxOutstandingRequests];
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

                CHX_LOG_INFO("Sending RawJPEG snapshot frameNumber:%d, request:%" PRIu64 " metadata:%p",
                             frameNumber, snapshotRequest.frameNumber, pSnapshotMetadata);

                ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

                free_camera_metadata(pSnapshotMetadata);
                pSnapshotMetadata = NULL;
            }
       }
    }
    CHX_LOG_INFO("RequestThreadProcessing Exited");
}

