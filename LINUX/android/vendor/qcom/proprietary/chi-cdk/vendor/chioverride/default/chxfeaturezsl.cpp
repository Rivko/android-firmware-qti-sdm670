////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeaturezsl.cpp
/// @brief CHX ZSL feature class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxadvancedcamerausecase.h"
#include "chxincs.h"
#include "chxfeaturezsl.h"
#include "chxusecase.h"

static const UINT ZslShutterLag        = 2;
static const UINT InvalidFrameNum      = 0xFFFFFFFF;
static const UINT RdiBufferWaitTimeout = 1000; // in ms

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureZSL::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FeatureZSL* FeatureZSL::Create(
    AdvancedCameraUsecase* pUsecase)
{
    FeatureZSL* pFeature = CHX_NEW FeatureZSL;

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
/// FeatureZSL::Pause
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureZSL::Pause(
        BOOL isForced)
{
    CHX_LOG_INFO("FeatureZSL::Pause(), isForced %d E.", isForced);
    ChxUtils::AtomicStoreU32(&m_aPauseInProgress, TRUE);
    m_pRdiAndMetaResultMutex->Lock();
    m_pRdiAndMetaResultAvailable->Signal();
    m_pRdiAndMetaResultMutex->Unlock();

    m_pOfflineRequestMutex->Lock();
    m_offlineRequestProcessTerminate = TRUE;
    m_pOfflineRequestAvailable->Signal();
    m_pOfflineRequestMutex->Unlock();

    if (NULL != m_offlineRequestProcessThread.pPrivateData)
    {
        CHX_LOG("Terminating offline thread in featureZSL");
        ChxUtils::ThreadTerminate(m_offlineRequestProcessThread.hThreadHandle);
        CHX_LOG("Terminated offline thread");
        m_offlineRequestProcessThread = { 0 };
    }
    CHX_LOG_INFO("FeatureZSL::Pause(), isForced %d X.", isForced);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureZSL::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureZSL::Flush(
        BOOL isInProgress)
{
    CHX_LOG("Feature Flush isInProgress =%d", isInProgress);
    if(TRUE == isInProgress)
    {
        ChxUtils::AtomicStoreU32(&m_aFlushInProgress, TRUE);
        m_pRdiAndMetaResultMutex->Lock();
        m_pRdiAndMetaResultAvailable->Signal();
        m_pRdiAndMetaResultMutex->Unlock();
    }
    else
    {
        ChxUtils::AtomicStoreU32(&m_aFlushInProgress, FALSE);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureZSL::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureZSL::Initialize(
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

    m_pOfflineRequestMutex           = Mutex::Create();
    m_pOfflineRequestAvailable       = Condition::Create();
    m_offlineRequestProcessTerminate = FALSE;
    m_aPauseInProgress               = FALSE;
    m_aFlushInProgress               = FALSE;

    m_offlineRequestProcessThread.pPrivateData = this;

    result = ChxUtils::ThreadCreate(FeatureZSL::RequestThread,
                                    &m_offlineRequestProcessThread,
                                    &m_offlineRequestProcessThread.hThreadHandle);
    if (CDKResultSuccess != result)
    {
        CHX_LOG("Offline request thread create failed with result %d", result);
        m_offlineRequestProcessThread = { 0 };
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureZSL::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureZSL::Destroy(
    BOOL isForced)
{
    CHX_LOG("isForced %d", isForced);

    m_pSnapshotStream   = NULL;
    m_pPreviewStream    = NULL;
    m_pRdiStream        = NULL;
    m_pVideoStream      = NULL;

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

    ChxUtils::AtomicStoreU32(&m_aFlushInProgress, FALSE);
    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureZSL::OverrideUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiUsecase* FeatureZSL::OverrideUsecase(
    LogicalCameraInfo*              pCameraInfo,
    camera3_stream_configuration_t* pStreamConfig)
{
    (VOID)pCameraInfo;
    CHX_LOG("OverrideUsecase for ZSL, num_streams = %d", pStreamConfig->num_streams);

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
    m_pSnapshotStream       = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::SnapshotStream);
    m_pVideoStream          = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::VideoStream);

    if (NULL != m_pVideoStream)
    {
        m_previewPipelineIndex  = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLPreviewRawYUVType);
        m_continuousRdiCapture  = FALSE;
    }
    else
    {
        m_previewPipelineIndex  = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLPreviewRawType);
        m_continuousRdiCapture  = TRUE;
    }

    CHX_LOG("m_continuousRdiCapture = %d", m_continuousRdiCapture);

    m_snapshotPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLSnapshotJpegType);
    if (static_cast<CHISTREAMFORMAT>(HAL_PIXEL_FORMAT_BLOB) != m_pSnapshotStream->format)
    {
        m_snapshotPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLSnapshotYUVType);
    }
    else if (TRUE == ExtensionModule::GetInstance()->UseGPURotationUsecase())
    {
        m_snapshotPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLSnapshotJpegGPUType);
    }

    ChiPipelineTargetCreateDescriptor* pSnapshotDesc = &m_pChiUsecase->pPipelineTargetCreateDesc[m_snapshotPipelineIndex];
    pSnapshotDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStream;
    pSnapshotDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pSnapshotStream;

    return m_pChiUsecase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureZSL::PipelineCreated
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureZSL::PipelineCreated(
    UINT sessionId,    ///< Id of session created
    UINT pipelineId)   ///< Pipeline of session created
{
    (VOID) pipelineId;
    ChiSensorModeInfo*              pSensorInfo = NULL;

    pSensorInfo = m_pUsecase->GetSessionData(sessionId)->pipelines[0].pPipeline->GetSensorModeInfo();

    m_pUsecase->ConfigRdiStream(pSensorInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureZSL::ExecuteProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult  FeatureZSL::ExecuteProcessRequest(
    camera3_capture_request_t* pRequest)              ///< Request parameters
{
    CDKResult                  result              = CDKResultSuccess;
    camera3_stream_buffer_t    previewBuffers[3]   = {{0}};
    UINT                       previewCount        = 0;
    UINT                       snapshotCount       = 0;
    UINT                       frameNumber         = pRequest->frame_number;
    UINT32                     sensorModeIndex;
    UINT32                     frameIndex          = (pRequest->frame_number % MaxOutstandingRequests);
    UINT                       snapshotReqIdIndex  = (m_maxSnapshotReqId % MaxOutstandingRequests);
    ChiModeFeature2SubModeType currentFeature2Mode = static_cast<ChiModeFeature2SubModeType>(*(m_pUsecase->GetFeature2Mode()));

    if(TRUE == m_isFlashRequired)
    {
        m_pUsecase->SetFeature2Mode(static_cast<UINT32>(ChiModeFeature2SubModeType::FLASH));
    }
    else
    {
        m_pUsecase->SetFeature2Mode(static_cast<UINT32>(currentFeature2Mode));
    }

    m_isSnapshotFrame[frameIndex]     = FALSE;
    m_isSkipPreview[frameIndex]       = FALSE;

    m_snapshotInputNum[snapshotReqIdIndex] = 0;

    sensorModeIndex = m_pUsecase->GetSessionData(m_previewPipelineIndex)->pSession->GetSensorModeInfo()->modeIndex;
    ChxUtils::FillTuningModeData(const_cast<camera_metadata_t*>(pRequest->settings),
                                 pRequest,
                                 sensorModeIndex,
                                 m_pUsecase->GetEffectMode(),
                                 m_pUsecase->GetSceneMode(),
                                 m_pUsecase->GetFeature1Mode(),
                                 m_pUsecase->GetFeature2Mode());

    m_pUsecase->SetFeature2Mode(static_cast<UINT32>(currentFeature2Mode));

    ChxUtils::FillCameraId(const_cast<camera_metadata_t*>(pRequest->settings), m_pUsecase->GetCameraId());

    for (UINT32 i = 0; i < pRequest->num_output_buffers; i++)
    {
        if ((m_pPreviewStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream)) ||
            (m_pVideoStream   == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream)))
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
    }

    PipelineData* pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_previewSessionId, 0));
    UINT          requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);

    pPipelineData->seqIdToFrameNum[requestIdIndex] = frameNumber;

    CHX_LOG("FeatureZSL Realtime AppFrameNum to ReqId: %d <--> %d", frameNumber, pPipelineData->seqId);

    if ((TRUE == m_continuousRdiCapture) || (0 < snapshotCount))
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
            CHX_LOG_ERROR("FeatureZSL: GetImageBuffer failed, No RDI buffer for frameNumber %d", frameNumber);
        }
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

        if (TRUE == m_pUsecase->IsLLSNeeded())
        {
            m_isSkipPreview[frameIndex]                   = TRUE;
            CHX_LOG("FeatureZSL Trigger LLS snapshot, use future RDI frame");
        }

        m_isSnapshotFrame[frameIndex]                 = TRUE;
        m_snapshotBufferNum[snapshotReqIdIndex]       = snapshotCount;
        m_snapshotReqIdToFrameNum[snapshotReqIdIndex] = frameNumber;
        m_snapshotInputNum[snapshotReqIdIndex]        = frameNumber - ZslShutterLag;
        m_maxSnapshotReqId++;
        m_pOfflineRequestAvailable->Signal();
        m_pOfflineRequestMutex->Unlock();

        CHX_LOG_ERROR("Snapshot in ZSL");
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureZSL::ProcessResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureZSL::ProcessResult(
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
    UINT32              rtPipelineReqId         = 0;

    if (m_snapshotPipelineIndex == pCbData->sessionId)
    {
        resultFrameNum   = m_snapshotReqIdToFrameNum[pResult->frameworkFrameNum % MaxOutstandingRequests];
        resultFrameIndex = resultFrameNum % MaxOutstandingRequests;
        CHX_LOG("Processing result for snapshot frame %d. Metadata: %p NumBuffers: %d Timestamp: %" PRIu64 " Sent: %d",
                resultFrameNum, pResult->pResultMetadata, pResult->numOutputBuffers,
                m_pUsecase->GetRequestShutterTimestamp(resultFrameNum),
                m_pUsecase->IsMetadataSent(resultFrameIndex));
    }
    else if (m_previewSessionId == pCbData->sessionId)
    {
        PipelineData*     pPipelineData =
            const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_previewSessionId, 0));

        resultFrameNum = pPipelineData->seqIdToFrameNum[pResult->frameworkFrameNum % MaxOutstandingRequests];
        resultFrameIndex = resultFrameNum % MaxOutstandingRequests;

        rtPipelineReqId = pResult->frameworkFrameNum;
        CHX_LOG("FeatureZSL Realtime ReqId to AppFrameNum: %d <--> %d", pResult->frameworkFrameNum, resultFrameNum);
    }

    camera3_capture_result_t* pUsecaseResult   = m_pUsecase->GetCaptureResult(resultFrameIndex);

    pUsecaseResult->frame_number = resultFrameNum;

    // If result contain metadata and metadata has not been sent to framework
    if (NULL != pResult->pResultMetadata)
    {
        BOOL               isResultMetadataAvailable = FALSE;
        UINT64             timestamp                 = m_pUsecase->GetRequestShutterTimestamp(resultFrameNum);
        camera_metadata_t* pMetadata                 =
            const_cast<camera_metadata_t*>(static_cast<const camera_metadata_t*>(pResult->pResultMetadata));

        // Do Not wait for Snapshot frame metadata, Return Preview metadata back to fwk
        // If we wait for snapshot, and if it takes more time to process, we will block the preview.
        pUsecaseResult->result = static_cast<const camera_metadata_t*>(pResult->pResultMetadata);
        pUsecaseResult->partial_result = pResult->numPartialMetadata;
        isResultMetadataAvailable = TRUE;

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
            if ((m_offlineRequestWaitingForFrame <= resultFrameNum) &&
                (m_lastRdiFrameAvailable != InvalidFrameNum) &&
                (m_lastRdiFrameAvailable >= resultFrameNum))
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

        // check if flash is required for snapshot
        m_isFlashRequired = m_pUsecase->IsFlashRequired(pMetadata);
    }

    for (UINT32 j = 0; j < pResult->numOutputBuffers; j++)
    {
        // If our internal stream, copy the result into the target buffer to be consumed by the offline pipeline
        if (m_pRdiStream == pResult->pOutputBuffers[j].pStream)
        {
            UINT32 zslQueueIndex = (rtPipelineReqId % BufferQueueDepth);

            m_pRdiTargetBuffer->pMutex->Lock();

            // Release reference to the buffer that's going to be replaced out of the queue
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

            m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isBufferUsed                   = FALSE;

            if ((TRUE == m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isBufferReady) &&
                (TRUE == m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isMetadataReady))
            {
                m_pRdiTargetBuffer->lastReadySequenceID = rtPipelineReqId;
            }

            m_pRdiTargetBuffer->pMutex->Unlock();

            m_pRdiAndMetaResultMutex->Lock();
            m_lastRdiFrameAvailable = resultFrameNum;
            CHX_LOG("m_lastRdiFrameAvailable = %d", m_lastRdiFrameAvailable);

            // If offline snapshot is waiting for current Rdi frame, signal it and unblock.
            if ((m_offlineRequestWaitingForFrame <= resultFrameNum) &&
                (m_lastRealtimeMetadataAvailable != InvalidFrameNum) &&
                (m_lastRealtimeMetadataAvailable >= resultFrameNum))
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

            if (m_snapshotPipelineIndex == pCbData->sessionId)
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

            if ((m_pPreviewStream == pResult->pOutputBuffers[j].pStream) &&
                (TRUE == m_isSkipPreview[resultFrameNum % MaxOutstandingRequests]))
            {
                CHX_LOG("skip the frame for display! resultFrameNum:%d",
                    resultFrameNum);
                ChxUtils::SkipPreviewFrame(pResultBuffer);
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
/// FeatureZSL::ProcessMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureZSL::ProcessMessage(
    const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{
    (VOID)pMessageDescriptor;
    (VOID)pPrivateCallbackData;
    //@todo this function to be REMOVED
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureZSL::IsPseudoZSL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL FeatureZSL::IsPseudoZSL(camera_metadata_t* pMetadata)
{
    // It is Pseudo ZSL
    // if application hasn't set the ZSL mode OR
    // If the snapshot is captured with Flash or Manual 3A gains

    UINT zslMode = 0;
    m_pUsecase->GetZSLMode(&zslMode, pMetadata);

    CHX_LOG("zslMode: %d m_isFlashRequired:%d.", zslMode, m_isFlashRequired);

    return ((0 == zslMode) || (TRUE == m_isFlashRequired));
}

INT32 FeatureZSL::GetRequiredPipelines(
    AdvancedPipelineType* pPipelines,
    INT32 size)
{
    INT32 count = 0;
    const INT32 pipelineCount = 2;

    if (NULL != pPipelines && size >= pipelineCount)
    {
        INT32 index = 0;
        AdvancedPipelineType pipelineGroup[1];
        UINT                 cameraId[1];
        UINT                 wideCameraId = m_pUsecase->GetPhysicalCameraId(CameraRoleTypeWide);

        pPipelines[index] = AdvancedPipelineType::ZSLSnapshotJpegType;
        // If the format isnt blob->JPEG fallback to YUV
        ChiStream* pStream = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::SnapshotStream);
        if (static_cast<CHISTREAMFORMAT>(HAL_PIXEL_FORMAT_BLOB) != pStream->format)
        {
            pPipelines[index] = AdvancedPipelineType::ZSLSnapshotYUVType;
        }
        else if (TRUE == ExtensionModule::GetInstance()->UseGPURotationUsecase())
        {
            pPipelines[index] = AdvancedPipelineType::ZSLSnapshotJpegGPUType;
        }
        pipelineGroup[0]    = pPipelines[index];
        cameraId[0]         = wideCameraId;
        m_snapshotSessionId = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
        m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
        index++;

        if (FALSE == m_pUsecase->IsMultiCameraUsecase())
        // if (TRUE)
        {
            if (NULL == m_pUsecase->GetSharedStream(AdvancedCameraUsecase::VideoStream))
            {
                pPipelines[index] = AdvancedPipelineType::ZSLPreviewRawType;
            }
            else
            {
                pPipelines[index] = AdvancedPipelineType::ZSLPreviewRawYUVType;
            }
            pipelineGroup[0]    = pPipelines[index];
            cameraId[0]         = wideCameraId;
            m_previewSessionId = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
            m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
            index++;
        }
        else
        {
            // m_previewSessionId = 0xFFFFFFFF;
            CHX_LOG("Don't use Realtime pipeline in advance usecase for multicamera usecase");
        }

        count = index;
    }

    CHX_LOG("FeatureZSL::GetRequiredPipelines, required pipeline count:%d video stream=%p", count,
        m_pUsecase->GetSharedStream(AdvancedCameraUsecase::VideoStream));
    return count;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FeatureZSL::RequestThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* FeatureZSL::RequestThread(
    VOID* pThreadData)
{
    PerThreadData* pPerThreadData = reinterpret_cast<PerThreadData*>(pThreadData);

    FeatureZSL* pFeatureZSL = reinterpret_cast<FeatureZSL*>(pPerThreadData->pPrivateData);

    pFeatureZSL->RequestThreadProcessing();

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FeatureZSL::RequestThreadProcessing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureZSL::RequestThreadProcessing()
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

        while ((m_maxSnapshotReqId > m_snapshotReqId) &&
               (FALSE == m_offlineRequestProcessTerminate))
        {
            UINT   requestIdIndex    = (m_snapshotReqId % MaxOutstandingRequests);
            UINT   frameNumber       = m_snapshotReqIdToFrameNum[requestIdIndex];
            UINT32 requestFrameIndex = frameNumber % MaxOutstandingRequests;

            // Check again, If we have the requirements for the snapshot frame for the corresponding frameNumber
            if (TRUE == m_isSnapshotFrame[requestFrameIndex])
            {
                UINT                    zslInputFrameNumber = frameNumber;
                camera3_stream_buffer_t snapshotInputBuffer = { 0 };

                // Create copy so that future updates to input metadata don't free it
                camera_metadata_t*      pSnapshotMetadata =
                    allocate_copy_camera_metadata_checked(m_pSnapshotInputMeta[requestIdIndex],
                                                          get_camera_metadata_size(m_pSnapshotInputMeta[requestIdIndex]));
                // If ZSLmode is enabled, give the recently available SOF frame as input
                if (FALSE == IsPseudoZSL(pSnapshotMetadata))
                {
                    zslInputFrameNumber = (ZslShutterLag < frameNumber) ? m_snapshotInputNum[requestIdIndex] : 0;
                    CHX_LOG_INFO("zslInputFrameNumber set in Non PsuedoZsl case = %u", zslInputFrameNumber);
                }

                if (TRUE == m_pUsecase->IsLLSNeeded())
                {
                    // If LLS snapshot is needed, we should pick the right rdi frame, which has longer shutter and smaller gain
                    zslInputFrameNumber = frameNumber;
                    CHX_LOG("LLS needed, pick RDI buffer with reqId:%d", frameNumber);
                }

                // If the selected buffer has already been used, move to the next buffer
                m_pRdiAndMetaResultMutex->Lock();
                if ((m_lastRdiFrameAvailable >= zslInputFrameNumber) &&
                    (TRUE == m_pRdiTargetBuffer->bufferQueue[zslInputFrameNumber % m_pRdiTargetBuffer->queueDepth].isBufferUsed))
                {
                    zslInputFrameNumber += 1;
                }
                CHX_LOG("zslInputFrameNumber = %d", zslInputFrameNumber);

                // Check whether realtime metadata and Rdi buffer for zslInputFrameNumber are available.
                // If available, go ahead with offline request submission for snapshot.
                // Otherwise, continue to while loop.

                if ((m_lastRdiFrameAvailable < zslInputFrameNumber || m_lastRealtimeMetadataAvailable < zslInputFrameNumber) ||
                    (InvalidFrameNum == m_lastRdiFrameAvailable  || InvalidFrameNum == m_lastRealtimeMetadataAvailable))
                {
                    m_offlineRequestWaitingForFrame = zslInputFrameNumber;

                    CHX_LOG_INFO("waiting for rdi and meta of frame %d for triggering snapshot request", zslInputFrameNumber);

                    result = m_pRdiAndMetaResultAvailable->TimedWait(m_pRdiAndMetaResultMutex->GetNativeHandle(), WAIT_BUFFER_TIMEOUT);

                    if (CDKResultSuccess != result)
                    {
                        CHX_LOG_ERROR("FeatureZSL: wait rdi and meta timeout! frameNumber=%d, zslInputFrameNumber=%d", frameNumber, zslInputFrameNumber);
                    }

                    if ((TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aPauseInProgress))) ||
                        (TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aFlushInProgress))))
                    {
                        CHX_LOG_INFO("Destroy is in progress come out of loop");

                        m_snapshotReqId = m_maxSnapshotReqId;

                        free_camera_metadata(pSnapshotMetadata);
                        pSnapshotMetadata = NULL;

                        m_pRdiAndMetaResultMutex->Unlock();
                        break;
                    }
                }
                m_pRdiAndMetaResultMutex->Unlock();

                if (CDKResultSuccess == result)
                {
                    m_pUsecase->GetTargetBuffer(zslInputFrameNumber,
                                                m_pRdiTargetBuffer,
                                                pSnapshotMetadata,
                                                &snapshotInputBuffer);

                    const Session* pSession = m_pUsecase->GetSessionData(m_snapshotPipelineIndex)->pSession;

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
                    if (FALSE == m_pUsecase->GetFlushStatus())
                    {
                        CHX_LOG_INFO("Sending ZSL snapshot frameNumber:%d, request:%" PRIu64 " metadata:%p",
                                     frameNumber, snapshotRequest.frameNumber, pSnapshotMetadata);
                        m_pUsecase->SetRequestToFeatureMapping(m_snapshotPipelineIndex, snapshotRequest.frameNumber, this);
                        ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);
                    }
                }
                free_camera_metadata(pSnapshotMetadata);
                pSnapshotMetadata = NULL;
            }
        }
    }
    CHX_LOG_INFO("RequestThreadProcessing Exited");
}
