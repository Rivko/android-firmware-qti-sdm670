////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeaturemultiframe.cpp
/// @brief CHX Multiframe feature class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxadvancedcamerausecase.h"
#include "chxincs.h"
#include "chxfeaturemultiframe.h"
#include "chxusecase.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMultiframe::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FeatureMultiframe* FeatureMultiframe::Create(
    AdvancedCameraUsecase* pUsecase)
{
    FeatureMultiframe* pFeature = CHX_NEW FeatureMultiframe;

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
/// FeatureMultiframe::Pause
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMultiframe::Pause(
    BOOL isForced)
{
    CHX_LOG("Feature pause isForced =%d", isForced);
    //Implement if required
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMultiframe::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMultiframe::Flush(
        BOOL isInProgress)
{
    CHX_LOG("Feature Flush isInProgress =%d", isInProgress);
    //Implement if required
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMultiframe::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMultiframe::Initialize(
    AdvancedCameraUsecase* pUsecase)
{
    m_pUsecase          = pUsecase;

    m_pMergeYuvStream   = static_cast<CHISTREAM*>(CHX_CALLOC(sizeof(CHISTREAM)));

    for (UINT32 i = 0; i < MaxInputPorts; i++)
    {
        m_pMergePipelineInputStreams[i] = static_cast<CHISTREAM*>(CHX_CALLOC(sizeof(CHISTREAM)));
    }

    m_previewRawReqId        = 0;
    m_lastShutterFrameNum    = 0;
    m_numMultiFramesRequired = DefaultSWMFNumFrames;

    m_snapshotSessionId      = InvalidSessionId;
    m_mergeSessionId         = InvalidSessionId;
    m_bayer2YuvSessionId     = InvalidSessionId;
    m_previewSessionId       = InvalidSessionId;
    m_bayer2YuvAuxSessionId  = InvalidSessionId;

    CHX_LOG("FeatureSWMF m_pRdiStream: %p, m_pBayer2YuvStream: %p, m_pMergeYuvStream: %p",
            m_pRdiStream, m_pBayer2YuvStream, m_pMergeYuvStream);

    ExtensionModule::GetInstance()->GetVendorTagOps(&m_vendorTagOps);
    CHX_LOG("pGetMetaData:%p, pSetMetaData:%p", m_vendorTagOps.pGetMetaData, m_vendorTagOps.pSetMetaData);

    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        CHX_LOG("FeatureSWMF Multi camera usecase, cameraId[0]:%d, cameraId[1]:%d",
            m_pUsecase->GetPhysicalCameraId(CameraRoleTypeWide),
            m_pUsecase->GetPhysicalCameraId(CameraRoleTypeTele));
    }

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMultiframe::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMultiframe::Destroy(BOOL isForced)
{

    CHX_LOG("FeatureSWMF::Destroy(), isForced %d E.", isForced);

    /// only reset shared stream to NULL
    /// no need to free, as advance will handle it.
    m_pSnapshotStream  = NULL;
    m_pPreviewStream   = NULL;
    m_pRdiStream       = NULL;
    m_pRdiStreamAux    = NULL;
    m_pBayer2YuvStream = NULL;
    m_pJPEGInputStream = NULL;

    if (NULL != m_pMergeYuvStream)
    {
        CHX_FREE(m_pMergeYuvStream);
        m_pMergeYuvStream = NULL;
    }

    for (UINT32 i = 0; i < MaxInputPorts; i++)
    {
        CHX_FREE(m_pMergePipelineInputStreams[i]);
        m_pMergePipelineInputStreams[i] = NULL;
    }

    m_pChiUsecase = NULL;

    for (UINT32 i = 0; i < MaxOutstandingRequests; i++)
    {
        if (NULL != m_pSnapshotInputMeta[i])
        {
            ChxUtils::FreeMetaData(m_pSnapshotInputMeta[i]);
            m_pSnapshotInputMeta[i] = NULL;
        }
    }

    if (NULL != m_pMergeOutputBufferManager)
    {
        m_pMergeOutputBufferManager->Destroy();
        m_pMergeOutputBufferManager = NULL;
    }

    for (UINT queueIndex = 0; queueIndex < BufferQueueDepth; queueIndex++)
    {
        if (NULL != m_mergeTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer)
        {
            CHX_FREE(m_mergeTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer);
            m_mergeTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer = NULL;
        }
        if (NULL != m_mergeTargetBuffers.bufferQueue[queueIndex].pMetadata)
        {
            free_camera_metadata(m_mergeTargetBuffers.bufferQueue[queueIndex].pMetadata);
            m_mergeTargetBuffers.bufferQueue[queueIndex].pMetadata = NULL;
        }
    }

    if (NULL != m_debugDataOffline.pData)
    {
        CHX_FREE(m_debugDataOffline.pData);
        m_debugDataOffline.pData    = NULL;
        m_debugDataOffline.size     = 0;
    }

    if (NULL != m_mergeTargetBuffers.pMutex)
    {
        m_mergeTargetBuffers.pMutex->Destroy();
        m_mergeTargetBuffers.pMutex = NULL;
    }

    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMultiframe::OverrideUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiUsecase* FeatureMultiframe::OverrideUsecase(
    LogicalCameraInfo*              pCameraInfo,
    camera3_stream_configuration_t* pStreamConfig)
{
    (VOID)pCameraInfo;
    CHX_LOG("FeatureSWMF OverrideUsecase for Multiframe");
    CHX_ASSERT(2 == pStreamConfig->num_streams);

    m_pChiUsecase = m_pUsecase->GetChiUseCase();

    // FEATURE_MULTIFRAME_PORT
    /// @todo - TargetName to ease the matching of streamId vs Target per pipeline. Need to map the rest of internal streams
    for (UINT target = 0; target < m_pChiUsecase->numTargets; target++)
    {
        if ((ChiFormatRawMIPI == m_pChiUsecase->ppChiTargets[target]->pBufferFormats[0]) &&
            (m_rdiStreamIndex == 0))
        {
            m_rdiStreamIndex   = target;
            m_pRdiTargetBuffer = m_pUsecase->GetTargetBuffer(m_rdiStreamIndex);
        }
        else if ((1 == m_pChiUsecase->ppChiTargets[target]->numFormats) &&
                 (ChiFormatYUV420NV12 == m_pChiUsecase->ppChiTargets[target]->pBufferFormats[0]) &&
                 (m_Bayer2YuvStreamIndex == 0))
        {
            /// @todo Need to map Target vs streamId for pipeline
            m_Bayer2YuvStreamIndex   = target;
            m_pBayer2YuvTargetBuffer = m_pUsecase->GetTargetBuffer(m_Bayer2YuvStreamIndex);
        }
    }

    m_snapshotPipelineIndex  = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::InternalZSLYuv2JpegType);
    m_mergePipelineIndex     = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::SWMFMergeYuvType);
    m_bayer2YuvPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLSnapshotYUVType);
    m_previewPipelineIndex   = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLPreviewRawType);

    CHX_LOG("m_previewPipelineIndex:%d, m_bayer2YuvPipelineIndex:%d, m_mergePipelineIndex:%d, m_snapshotPipelineIndex:%d,",
        m_previewPipelineIndex,
        m_bayer2YuvPipelineIndex,
        m_mergePipelineIndex,
        m_snapshotPipelineIndex);

    m_pPreviewStream        = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::PreviewStream);
    m_pRdiStream            = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::RdiStream);
    m_pSnapshotStream       = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::SnapshotStream);
    m_pBayer2YuvStream      = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::Bayer2YuvStream);
    m_pJPEGInputStream      = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::JPEGInputStream);

    ChiPipelineTargetCreateDescriptor* pBayer2YuvDesc   = &m_pChiUsecase->pPipelineTargetCreateDesc[m_bayer2YuvPipelineIndex];
    ChiPipelineTargetCreateDescriptor* pMergeDesc       = &m_pChiUsecase->pPipelineTargetCreateDesc[m_mergePipelineIndex];
    ChiPipelineTargetCreateDescriptor* pSnapshotDesc    = &m_pChiUsecase->pPipelineTargetCreateDesc[m_snapshotPipelineIndex];

    // Multiframe Stage-1 (BayerToYuv)
    pBayer2YuvDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStream;
    pBayer2YuvDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pBayer2YuvStream;

    // HDR Stage-2 (Merge)
    CHX_LOG("FeatureSWMF Merge Pipeline source target:%d", pMergeDesc->sourceTarget.numTargets);
    ChxUtils::Memset(m_mergePipelineInputTargets,  0, sizeof(m_mergePipelineInputTargets));
    ChxUtils::Memset(&m_mergePipelineOutputTarget, 0, sizeof(m_mergePipelineOutputTarget));

    for (UINT32 i = 0; i < pMergeDesc->sourceTarget.numTargets; i++)
    {
        ChxUtils::Memcpy(&m_mergePipelineInputTargets[i],
            pMergeDesc->sourceTarget.pTargetPortDesc[i].pTarget,
            sizeof(ChiTarget));
    }
    ChxUtils::Memcpy(&m_mergePipelineOutputTarget,
        pMergeDesc->sinkTarget.pTargetPortDesc[0].pTarget,
        sizeof(ChiTarget));

    for (UINT32 i = 0; i < pMergeDesc->sourceTarget.numTargets; i++)
    {
        pMergeDesc->sourceTarget.pTargetPortDesc[i].pTarget = &m_mergePipelineInputTargets[i];
        pMergeDesc->sourceTarget.pTargetPortDesc[i].pTarget->pChiStream = m_pMergePipelineInputStreams[i];

        CHX_LOG("[%d], Node:%d, port:%d to input stream:%p",
            i,
            pMergeDesc->sourceTarget.pTargetPortDesc[i].nodeport.nodeId,
            pMergeDesc->sourceTarget.pTargetPortDesc[i].nodeport.nodePortId,
            m_pMergePipelineInputStreams[i]);
    }

    pMergeDesc->sinkTarget.pTargetPortDesc[0].pTarget                   = &m_mergePipelineOutputTarget;
    pMergeDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream       = m_pMergeYuvStream;

    // HDR Stage-3 (Jpeg)
    pSnapshotDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream  = m_pJPEGInputStream;
    pSnapshotDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream    = m_pSnapshotStream;

    CHX_LOG("m_rdiStreamIndex:%d, m_Bayer2YuvStreamIndex:%d, m_mergeStreamIndex:%d",
        m_rdiStreamIndex, m_Bayer2YuvStreamIndex, m_mergeStreamIndex);

    CHX_LOG("FeatureSWMF RdiStream: %p, PreviewStream: %p, SnapshotStream: %p, Bayer2YuvStream: %p, JPEGInputStream:%p",
            m_pRdiStream, m_pPreviewStream, m_pSnapshotStream, m_pBayer2YuvStream, m_pJPEGInputStream);

    // For Aux camera in dual camera usecase
    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        CHX_LOG("FeatureSWMF Override usecase for aux camera");
        m_pRdiStreamAux             = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::RdiStreamAux);
        m_bayer2YuvAuxPipelineIndex =
            m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLSnapshotYUVAuxType);

        ChiPipelineTargetCreateDescriptor* pBayer2YuvAuxDesc =
            &m_pChiUsecase->pPipelineTargetCreateDesc[m_bayer2YuvAuxPipelineIndex];

        pBayer2YuvAuxDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStreamAux;
        pBayer2YuvAuxDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pBayer2YuvStream;

        CHX_LOG("FeatureSWMF RdiStreamAux:%p, m_bayer2YuvAuxPipelineIndex:%d",
            m_pRdiStreamAux, m_bayer2YuvAuxPipelineIndex);
    }

    return m_pChiUsecase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMultiframe::PipelineCreated
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMultiframe::PipelineCreated(
    UINT32 sessionId,    ///< Id of session created
    UINT32 pipelineId)   ///< Pipeline of session created
{
    ChiSensorModeInfo*              pSensorInfo     = NULL;
    const ChiPipelineInputOptions*  pInputOptions   = NULL;

    pSensorInfo   = m_pUsecase->GetSessionData(sessionId)->pipelines[0].pPipeline->GetSensorModeInfo();
    pInputOptions = m_pUsecase->GetSessionData(sessionId)->pipelines[0].pPipeline->GetInputOptions();

    CHX_LOG("FeatureSWMF sessionId:%d, pipelineId: %d, Sensor widthxheight: %d x %d, InputOptions widthxheight: %d x %d",
                  sessionId,
                  pipelineId,
                  pSensorInfo->frameDimension.width, pSensorInfo->frameDimension.height,
                  pInputOptions->bufferOptions.optimalDimension.width,
                  pInputOptions->bufferOptions.optimalDimension.height);

    switch (m_pUsecase->GetAdvancedPipelineTypeByPipelineId(pipelineId))
    {
        case AdvancedPipelineType::InternalZSLYuv2JpegType:
        {
            m_pMergeYuvStream->format           = ChiStreamFormatYCbCr420_888;
            // Create the Merge stream output based on the input buffer requirements to generate the snapshot stream buffer
            m_pMergeYuvStream->width            = pInputOptions->bufferOptions.optimalDimension.width;
            m_pMergeYuvStream->height           = pInputOptions->bufferOptions.optimalDimension.height;
            m_pMergeYuvStream->maxNumBuffers    = 0;
            m_pMergeYuvStream->rotation         = StreamRotationCCW90;
            m_pMergeYuvStream->streamType       = ChiStreamTypeOutput;
            m_pMergeYuvStream->grallocUsage     = 0;

            m_pJPEGInputStream->format           = ChiStreamFormatYCbCr420_888;
            // Create the Merge stream output based on the input buffer requirements to generate the snapshot stream buffer
            m_pJPEGInputStream->width            = pInputOptions->bufferOptions.optimalDimension.width;
            m_pJPEGInputStream->height           = pInputOptions->bufferOptions.optimalDimension.height;
            m_pJPEGInputStream->maxNumBuffers    = 0;
            m_pJPEGInputStream->rotation         = StreamRotationCCW90;
            m_pJPEGInputStream->streamType       = ChiStreamTypeInput;
            m_pJPEGInputStream->grallocUsage     = 0;

            CHX_LOG("MergeYuvStream/JPEGInputStream: %dx%d", m_pMergeYuvStream->width, m_pMergeYuvStream->height);

            /// Create buffer managers
            CHIBufferManagerCreateData createData = { 0 };

            createData.width                = m_pMergeYuvStream->width;
            createData.height               = m_pMergeYuvStream->height;
            createData.format               = ChiStreamFormatYCbCr420_888;
            createData.producerFlags        = GRALLOC1_PRODUCER_USAGE_CAMERA;
            createData.consumerFlags        = GRALLOC1_CONSUMER_USAGE_CAMERA | GRALLOC1_CONSUMER_USAGE_CPU_READ;
            createData.maxBufferCount       = MinOutputBuffers;
            createData.immediateBufferCount = 1;

            m_pMergeOutputBufferManager = CHIBufferManager::Create("SWMFBufferManagaer", &createData);
            if (NULL == m_pMergeOutputBufferManager)
            {
                CHX_LOG_ERROR("m_pMergeOutputBufferManager is NULL");
            }
            else
            {
                m_mergeTargetBuffers.pBufferManager = m_pMergeOutputBufferManager;
                m_mergeTargetBuffers.pMutex         = Mutex::Create();

                for (UINT queueIndex = 0; queueIndex < BufferQueueDepth; queueIndex++)
                {
                    m_mergeTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer =
                        static_cast<CHISTREAMBUFFER*>(CHX_CALLOC(sizeof(CHISTREAMBUFFER)));
                    m_mergeTargetBuffers.bufferQueue[queueIndex].pMetadata = NULL;

                    if (NULL != m_mergeTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer)
                    {
                        m_mergeTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer->acquireFence = -1;
                        m_mergeTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer->phBuffer = NULL;
                        m_mergeTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer->pStream = m_pMergeYuvStream;
                    }
                }

                m_pMergeTargetBuffer = &m_mergeTargetBuffers;
            }
            break;
        }
        case AdvancedPipelineType::SWMFMergeYuvType:
        {
            m_pBayer2YuvStream->format          = ChiStreamFormatYCbCr420_888;
            // Create the Bayer2Yuv stream output based on the input buffer requirements to generate the Merge stream buffer
            m_pBayer2YuvStream->width           = pInputOptions->bufferOptions.optimalDimension.width;
            m_pBayer2YuvStream->height          = pInputOptions->bufferOptions.optimalDimension.height;
            m_pBayer2YuvStream->maxNumBuffers   = 0;
            m_pBayer2YuvStream->rotation        = StreamRotationCCW90;
            m_pBayer2YuvStream->streamType      = ChiStreamTypeOutput;
            m_pBayer2YuvStream->grallocUsage    = 0;

            for (UINT32 i = 0; i < MaxInputPorts; i++)
            {
                ChxUtils::Memcpy(m_pMergePipelineInputStreams[i], m_pBayer2YuvStream, sizeof(ChiStream));
                m_pMergePipelineInputStreams[i]->streamType = ChiStreamTypeInput;
            }

            CHX_LOG("m_pBayer2YuvStream: %dx%d", m_pMergeYuvStream->width, m_pMergeYuvStream->height);
            break;
        }
        case AdvancedPipelineType::ZSLSnapshotYUVType:
        {
            m_pUsecase->ConfigRdiStream(pSensorInfo);
            break;
        }
        default:
        {
            CHX_LOG("FeatureSWMF pipeline not used in SWMF.");
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMultiframe::ExecuteProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult  FeatureMultiframe::ExecuteProcessRequest(
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
    BOOL                      hasSnapshotStream  = FALSE;

    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        /// Assume it always take snapshot in dualcamera usecase.
        UINT32         rtReqId      = pRequest->frame_number;
        RequestMapInfo info         = m_pUsecase->GetRequestMapInfo(rtReqId);
        UINT32         appFrameNum  = info.frameNumber;
        UINT32         appFrameIdx  = (appFrameNum % MaxOutstandingRequests);

        m_masterCameraId            = info.masterCameraID;
        m_activePipelineID          = info.activePipelineID;

        m_isSnapshotFrame[appFrameIdx]         = TRUE;
        m_snapshotBufferNum[appFrameIdx]       = 1;
        m_snapshotReqIdToFrameNum[appFrameIdx] = appFrameNum;

        m_maxSnapshotReqId++;
        m_numBayer2YuvFrames = 0;

        for (UINT32 i = 0; i < pRequest->num_output_buffers; i++)
        {
            if (m_pSnapshotStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
            {
                /// in dual camera usecase, setting here is NULL, no need to copy the meta.
                /// only copy the snapshot buffer
                ChxUtils::Memcpy(&m_snapshotBuffers[appFrameIdx][0],
                                 &pRequest->output_buffers[i],
                                 sizeof(camera3_stream_buffer_t));
                const Session* pSession        = m_pUsecase->GetSessionData(m_bayer2YuvSessionId)->pSession;
                UINT32 sensorModeIndexSnapshot = pSession->GetSensorModeInfo(0)->modeIndex;
                ChxUtils::FillTuningModeData(const_cast<camera_metadata_t*>(pRequest->settings),
                    pRequest,
                    sensorModeIndexSnapshot,
                    m_pUsecase->GetEffectMode(),
                    m_pUsecase->GetSceneMode(),
                    m_pUsecase->GetFeature1Mode(),
                    m_pUsecase->GetFeature2Mode());
                hasSnapshotStream  = TRUE;
            }
        }

        if (FALSE == hasSnapshotStream)
        {
            const Session* pSession       = m_pUsecase->GetSessionData(m_previewSessionId)->pSession;
            UINT32 sensorModeIndexPreview = pSession->GetSensorModeInfo(0)->modeIndex;
            ChxUtils::FillTuningModeData(const_cast<camera_metadata_t*>(pRequest->settings),
                pRequest,
                sensorModeIndexPreview,
                m_pUsecase->GetEffectMode(),
                m_pUsecase->GetSceneMode(),
                m_pUsecase->GetFeature1Mode(),
                m_pUsecase->GetFeature2Mode());
        }

        for (UINT32 i = 0; i < m_numMultiFramesRequired; i++)
        {
            UINT32 reqId = rtReqId - i;
            info         = m_pUsecase->GetRequestMapInfo(reqId);

            CHX_LOG("FeatureSWMF ExecuteProcessRequest for dual camera!, rtReqId:%d, AppFrameNum:%d" \
                    "masterCameraId:%d, activePipelineID:%d",
                    reqId, appFrameNum, m_masterCameraId, m_activePipelineID);

            UINT32 sessionId = 0;
            if (m_masterCameraId == m_pUsecase->GetPhysicalCameraId(CameraRoleTypeWide))
            {
                sessionId = m_bayer2YuvSessionId;
            }
            else if (m_masterCameraId == m_pUsecase->GetPhysicalCameraId(CameraRoleTypeTele))
            {
                sessionId = m_bayer2YuvAuxSessionId;
            }

            const PipelineData* pPipelineData  = m_pUsecase->GetPipelineData(sessionId, 0);
            UINT32              bayer2YuvReqId = pPipelineData->seqId;
            CHX_LOG("AppFrameNum:%d <--> bayer2YuvReqId:%d, input RDI request id:%d, sessionId:%d",
                appFrameNum, bayer2YuvReqId, reqId, sessionId);

            camera3_stream_buffer_t outputBuffer = {0};

            ImageBuffer* pImageBuffer = m_pBayer2YuvTargetBuffer->pBufferManager->GetImageBuffer();
            if (NULL != pImageBuffer)
            {
                outputBuffer.buffer         = pImageBuffer->GetBufferHandle();
                outputBuffer.stream         = reinterpret_cast<camera3_stream_t*>(m_pBayer2YuvStream);
                outputBuffer.acquire_fence  = -1;
            }
            else
            {
                CHX_LOG_ERROR("m_pBayer2YuvTargetBuffer GetImageBuffer failed.");
                result = CDKResultEFailed;
            }

            if (CDKResultSuccess == result)
            {
                SubmitRequest(sessionId, appFrameNum, m_pRdiTargetBuffer, &outputBuffer, reqId);
            }
            else
            {
                CHX_LOG_ERROR("FeatureSWMF request is not submitted to Bayer2YuvPipeline! appFrameNum = %d, sessionId = %d",
                              appFrameNum, sessionId);
                break;
            }
        }
        return result;
    }

    CHX_LOG("FeatureSWMF::ExecuteProcessRequest, frameNum:%d, setting:%p", pRequest->frame_number, pRequest->settings);

    pUsecaseResult->result             = NULL;
    pUsecaseResult->frame_number       = pRequest->frame_number;
    pUsecaseResult->num_output_buffers = 0;

    m_isSnapshotFrame[frameIndex]      = FALSE;
    m_shutterTimestamp[frameIndex]     = 0;

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
            ChxUtils::FreeMetaData(m_pSnapshotInputMeta[frameIndex]);
            m_pSnapshotInputMeta[frameIndex]    =
                ChxUtils::AllocateCopyMetaData(reinterpret_cast<const VOID*>(pRequest->settings));

            ChxUtils::FreeMetaData(m_pBayer2YuvInputMeta[frameIndex]);
            m_pBayer2YuvInputMeta[frameIndex]   =
                ChxUtils::AllocateCopyMetaData(reinterpret_cast<const VOID*>(pRequest->settings));

            ChxUtils::FreeMetaData(m_pMergeInputMeta[frameIndex]);
            m_pMergeInputMeta[frameIndex]       =
                ChxUtils::AllocateCopyMetaData(reinterpret_cast<const VOID*>(pRequest->settings));

            CHX_LOG("Snapshot Frame %d", pRequest->frame_number);
            ChxUtils::Memcpy(&m_snapshotBuffers[frameIndex][snapshotCount],
                             &pRequest->output_buffers[i],
                             sizeof(camera3_stream_buffer_t));
            snapshotCount++;

            const Session* pSession         = m_pUsecase->GetSessionData(m_bayer2YuvSessionId)->pSession;
            UINT32 sensorModeIndexSnapshot  = pSession->GetSensorModeInfo(0)->modeIndex;
            ChxUtils::FillTuningModeData(const_cast<camera_metadata_t*>(pRequest->settings),
                pRequest,
                sensorModeIndexSnapshot,
                m_pUsecase->GetEffectMode(),
                m_pUsecase->GetSceneMode(),
                m_pUsecase->GetFeature1Mode(),
                m_pUsecase->GetFeature2Mode());
            hasSnapshotStream = TRUE;
        }
    }

    if (FALSE == hasSnapshotStream)
    {
        const Session* pSession       = m_pUsecase->GetSessionData(m_previewSessionId)->pSession;
        UINT32 sensorModeIndexPreview = pSession->GetSensorModeInfo(0)->modeIndex;
        ChxUtils::FillTuningModeData(const_cast<camera_metadata_t*>(pRequest->settings),
            pRequest,
            sensorModeIndexPreview,
            m_pUsecase->GetEffectMode(),
            m_pUsecase->GetSceneMode(),
            m_pUsecase->GetFeature1Mode(),
            m_pUsecase->GetFeature2Mode());
    }

    PipelineData* pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_previewSessionId, 0));
    UINT          requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);

    pPipelineData->seqIdToFrameNum[requestIdIndex] = frameNumber;

    CHX_LOG("FeatureSWMF Realtime AppFrameNum to ReqId: %d <--> %d", frameNumber, pPipelineData->seqId);

    m_previewRawReqId    = pPipelineData->seqId;

    UINT32 rdiQueueindex = m_previewRawReqId % BufferQueueDepth;

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
        CHX_LOG_ERROR("FeatureSWMF: GetImageBuffer failed, No RDI buffer for frameNumber %d", frameNumber);
    }

    const Session* pSession   = m_pUsecase->GetSessionData(m_previewSessionId)->pSession;

    CHICAPTUREREQUEST request = { 0 };
    request.frameNumber       = pPipelineData->seqId++;
    request.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
    request.numOutputs        = previewCount;
    request.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(previewBuffers);
    request.pMetadata         = pRequest->settings;

    request.pPrivData                = &m_privData[request.frameNumber % MaxOutstandingRequests];
    request.pPrivData->featureType   = FeatureType::SWMF;

    CHIPIPELINEREQUEST submitRequest = { 0 };
    submitRequest.pSessionHandle     = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
    submitRequest.numRequests        = 1;
    submitRequest.pCaptureRequests   = &request;

    m_pUsecase->SetRequestToFeatureMapping(m_previewSessionId, request.frameNumber, this);

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
        m_isSnapshotFrame[frameIndex]         = TRUE;
        m_snapshotBufferNum[frameIndex]       = snapshotCount;
        m_snapshotReqIdToFrameNum[frameIndex] = frameNumber;
        m_maxSnapshotReqId++;

        m_numBayer2YuvFrames = 0;

        CHX_LOG("FeatureSWMF Snapshot Request received, appFrameNum:%d.", pRequest->frame_number);
        // If we have the requirements for the snapshot frame, kick it off immediately
        if (TRUE == m_isSnapshotFrame[frameIndex])
        {
            for (UINT32 i = 0; i < m_numMultiFramesRequired; i++)
            {
                UINT32 realtimeReqId = pPipelineData->seqId - 1;
                UINT32 reqId = realtimeReqId - i;
                UINT32 queueIndex = reqId % BufferQueueDepth;

                CHX_LOG("FeatureSWMF send buffer to Bayer2YuvPipeline, frame number:%d, buffer:%p, meta:%p", frameNumber,
                    m_pRdiTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer,
                    m_pRdiTargetBuffer->bufferQueue[queueIndex].pMetadata);

                const PipelineData* pPipelineData = m_pUsecase->GetPipelineData(m_bayer2YuvSessionId, 0);
                UINT32 bayer2YuvReqId = pPipelineData->seqId;
                CHX_LOG("AppFrameNum:%d <--> bayer2YuvReqId:%d, input RDI request id:%d",
                    frameNumber, bayer2YuvReqId, reqId);

                camera3_stream_buffer_t outputBuffer = { 0 };

                ImageBuffer* pImageBuffer = m_pBayer2YuvTargetBuffer->pBufferManager->GetImageBuffer();
                if (NULL != pImageBuffer)
                {
                    outputBuffer.buffer         = pImageBuffer->GetBufferHandle();
                    outputBuffer.stream         = reinterpret_cast<camera3_stream_t*>(m_pBayer2YuvStream);
                    outputBuffer.acquire_fence  = -1;
                }
                else
                {
                    CHX_LOG_ERROR("m_pBayer2YuvTargetBuffer GetImageBuffer failed!");
                    result = CDKResultEFailed;
                }

                if (CDKResultSuccess == result)
                {
                    if (FALSE == hasSnapshotStream)
                    {
                        const Session* pSession       = m_pUsecase->GetSessionData(m_previewSessionId)->pSession;
                        UINT32 sensorModeIndexPreview = pSession->GetSensorModeInfo(0)->modeIndex;
                        ChxUtils::FillTuningModeData(const_cast<camera_metadata_t*>(pRequest->settings),
                            pRequest,
                            sensorModeIndexPreview,
                            m_pUsecase->GetEffectMode(),
                            m_pUsecase->GetSceneMode(),
                            m_pUsecase->GetFeature1Mode(),
                            m_pUsecase->GetFeature2Mode());
                    }
                    if (TRUE == hasSnapshotStream)
                    {
                        const Session* pSession        = m_pUsecase->GetSessionData(m_bayer2YuvSessionId)->pSession;
                        UINT32 sensorModeIndexSnapshot = pSession->GetSensorModeInfo(0)->modeIndex;
                        ChxUtils::FillTuningModeData(const_cast<camera_metadata_t*>(m_pRdiTargetBuffer->bufferQueue[queueIndex].pMetadata),
                            pRequest,
                            sensorModeIndexSnapshot,
                            m_pUsecase->GetEffectMode(),
                            m_pUsecase->GetSceneMode(),
                            m_pUsecase->GetFeature1Mode(),
                            m_pUsecase->GetFeature2Mode());
                    }

                    SubmitRequest(m_bayer2YuvSessionId, frameNumber, m_pRdiTargetBuffer,
                        &outputBuffer, reqId);
                }
                else
                {
                    CHX_LOG_ERROR("FeatureSWMF request is not submitted to Bayer2YuvPipeline! frameNumber = %d", frameNumber);
                    break;
                }
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMultiframe::ProcessResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMultiframe::ProcessResult(
    CHICAPTURERESULT*   pResult,
    VOID*               pPrivateCallbackData)
{
    CDKResult           result                = CDKResultSuccess;
    SessionPrivateData* pCbData               = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    BOOL                isAppResultsAvailable = FALSE;
    UINT32              resultFrameNum        = pResult->frameworkFrameNum;
    UINT32              realtimeReqId         = 0;
    UINT32              offlineReqId          = 0;

    if (m_previewSessionId != pCbData->sessionId)
    {
        const PipelineData* pPipelineData = m_pUsecase->GetPipelineData(pCbData->sessionId, 0);
        offlineReqId                      = pResult->frameworkFrameNum;
        resultFrameNum                    = pPipelineData->seqIdToFrameNum[pResult->frameworkFrameNum % MaxOutstandingRequests];
    }
    else if (m_previewSessionId == pCbData->sessionId)
    {
        PipelineData* pPipelineData = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_previewSessionId, 0));
        resultFrameNum              = pPipelineData->seqIdToFrameNum[pResult->frameworkFrameNum % MaxOutstandingRequests];
        realtimeReqId               = pResult->frameworkFrameNum;
        CHX_LOG("Realtime ReqId to AppFrameNum: %d <--> %d", pResult->frameworkFrameNum, resultFrameNum);
    }

    CHX_LOG("FeatureSWMF ProcessResult resultFrameNum: %d, sessionId: %d, pResultMetadata: %p, numOutputBuffers: %d",
            resultFrameNum, pCbData->sessionId, pResult->pResultMetadata, pResult->numOutputBuffers);

    UINT32                    resultFrameIndex = resultFrameNum % MaxOutstandingRequests;
    camera3_capture_result_t* pUsecaseResult   = m_pUsecase->GetCaptureResult(resultFrameIndex);

    pUsecaseResult->frame_number = resultFrameNum;

    // If result contain metadata and metadata has not been sent to framework
    if ((NULL != pResult->pResultMetadata))
    {
        BOOL    isResultMetadataAvailable = FALSE;
        UINT64  timestamp                 = m_shutterTimestamp[resultFrameNum % MaxOutstandingRequests];
        if (0 == timestamp)
        {
            timestamp = m_pUsecase->GetRequestShutterTimestamp(resultFrameNum);
        }

        camera_metadata_t* pMetadata      =
            const_cast<camera_metadata_t*>(static_cast<const camera_metadata_t*>(pResult->pResultMetadata));

        if (m_previewSessionId == pCbData->sessionId)
        {
            UINT32 zslQueueIndex = (realtimeReqId % BufferQueueDepth);

            // Create copy because this pointer could be freed by app as soon as result returned
            ChxUtils::FreeMetaData(m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pMetadata);
            /// @todo This currently copies the debugData pointer, to memory owned by the first pipeline. Should copy
            m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pMetadata =
                static_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(pMetadata));

            CHX_LOG("zslQueueIndex:%d, meta:%p", zslQueueIndex, m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pMetadata);

            m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].frameNumber = resultFrameNum; // pResult->frameworkFrameNum;
        }
        else if ((m_bayer2YuvSessionId == pCbData->sessionId) || (m_bayer2YuvAuxSessionId == pCbData->sessionId))
        {
            UINT32 bufferQueueIdx = (offlineReqId % BufferQueueDepth);

            CHX_LOG("meta for Bayer2Yuv Pipeline, frame number:%d, meta:%p", resultFrameNum, pMetadata);

            // Create copy because this pointer could be freed by app as soon as result returned
            ChxUtils::FreeMetaData(m_pBayer2YuvTargetBuffer->bufferQueue[bufferQueueIdx].pMetadata);
            /// @todo This currently copies the debugData pointer, to memory owned by the first pipeline. Should copy
            m_pBayer2YuvTargetBuffer->bufferQueue[bufferQueueIdx].pMetadata =
                static_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(pMetadata));

            m_pBayer2YuvTargetBuffer->bufferQueue[bufferQueueIdx].frameNumber = resultFrameNum;
        }
        else if (m_mergeSessionId == pCbData->sessionId)
        {
            CHX_LOG("meta for Merge Pipeline, frame number:%d, meta:%p", resultFrameNum, pMetadata);

            UINT32 queueIndex = (resultFrameNum % BufferQueueDepth);

            // Create copy because this pointer could be freed by app as soon as result returned
            ChxUtils::FreeMetaData(m_pMergeTargetBuffer->bufferQueue[queueIndex].pMetadata);
            /// @todo This currently copies the debugData pointer, to memory owned by the first pipeline. Should copy
            m_pMergeTargetBuffer->bufferQueue[queueIndex].pMetadata   =
                static_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(pMetadata));

            m_pMergeTargetBuffer->bufferQueue[queueIndex].frameNumber = resultFrameNum;
        }

        if ((FALSE == m_pUsecase->IsMetadataSent(resultFrameIndex)))
        {
            // Do Not wait for Snapshot frame metadata, Return Preview metadata back to fwk.
            // If we wait for snapshot, and if it takes more time to process, we will block the preview.
            pUsecaseResult->result = static_cast<const camera_metadata_t*>(pResult->pResultMetadata);
            pUsecaseResult->partial_result = pResult->numPartialMetadata;
            isResultMetadataAvailable = TRUE;

            // Override snapshot frame sensor timestamp metadata with shutter event for same frame number
            if ((NULL != pMetadata) && (0 != timestamp))
            {
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

                CHX_LOG("Update sensor timestamp for frame %d, timestamp = %" PRIu64 "  Status %d",
                        resultFrameNum, timestamp, status);

                if (TRUE == isResultMetadataAvailable)
                {
                    m_pUsecase->SetMetadataAvailable(resultFrameIndex);
                    isAppResultsAvailable = TRUE;
                    CHX_LOG("returnmetadata SWMF JPEG metadata：%d", resultFrameNum);
                }
            }
        }
    }

    for (UINT32 j = 0; j < pResult->numOutputBuffers; j++)
    {
        // If our internal stream, copy the result into the target buffer to be consumed by the offline pipeline
        if (m_pRdiStream == pResult->pOutputBuffers[j].pStream)
        {
            // Use RequestId for RDI buffer queue
            UINT32 queueIndex = (realtimeReqId % BufferQueueDepth);

            CHX_LOG("ZSL queue index:%d, frame num:%d, is snapshot:%d",
                queueIndex, resultFrameNum, m_isSnapshotFrame[resultFrameIndex]);

            // Release reference to the buffer that's going to be replaced out of queue
            if (realtimeReqId >= BufferQueueDepth)
            {
                m_pRdiTargetBuffer->pBufferManager->ReleaseReference(
                    reinterpret_cast<buffer_handle_t*>(m_pRdiTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer->phBuffer));
                m_pRdiTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer->acquireFence = -1;
            }

            ChxUtils::Memcpy(m_pRdiTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer,
                             &pResult->pOutputBuffers[j],
                             sizeof(CHISTREAMBUFFER));
        }
        else if (m_pBayer2YuvStream == pResult->pOutputBuffers[j].pStream)
        {
            CHX_LOG("FeatureSWMF Received Bayer2Yuv stream, reqid:%d, frame num:%d", offlineReqId, resultFrameNum);

            // Release reference to the input buffers of the request to Bayer2YuvPipeline
            for (UINT32 i = 0; i < pResult->pPrivData->numInputBuffers; i++)
            {
                if ((NULL != pResult->pPrivData->bufferManagers[i]) && (NULL != pResult->pPrivData->inputBuffers[i]))
                {
                    reinterpret_cast<CHIBufferManager*>(pResult->pPrivData->bufferManagers[i])->
                        ReleaseReference(reinterpret_cast<buffer_handle_t*>(pResult->pPrivData->inputBuffers[i]));
                    pResult->pPrivData->bufferManagers[i] = NULL;
                    pResult->pPrivData->inputBuffers[i]   = NULL;
                }
            }

            UINT32 bufferQueueIdx = (offlineReqId % BufferQueueDepth);
            ChxUtils::Memcpy(m_pBayer2YuvTargetBuffer->bufferQueue[bufferQueueIdx].pRdiOutputBuffer,
                             &pResult->pOutputBuffers[j],
                             sizeof(CHISTREAMBUFFER));
            m_pBayer2YuvTargetBuffer->pBufferManager->ReleaseReference(
                reinterpret_cast<buffer_handle_t*>(pResult->pOutputBuffers[j].phBuffer));
            m_pBayer2YuvTargetBuffer->bufferQueue[bufferQueueIdx].pRdiOutputBuffer->acquireFence = -1;

            m_numBayer2YuvFrames++;
            CHX_LOG("received bayer2Yuv frames:%d", m_numBayer2YuvFrames);

            // submit request to merge pipeline until we got all buffers and frames.
            // because meta alway comes before or along with frames, so here it's sure meta is received.
            if ((TRUE == m_isSnapshotFrame[resultFrameIndex]) && (m_numBayer2YuvFrames == m_numMultiFramesRequired))
            {
                CHX_LOG("FeatureSWMF received all bayer2Yuv Frames, send to merge pipeline..");
                camera3_stream_buffer_t outputBuffer = {0};

                ImageBuffer* pImageBuffer = m_pMergeTargetBuffer->pBufferManager->GetImageBuffer();
                if (NULL != pImageBuffer)
                {
                    outputBuffer.buffer         = pImageBuffer->GetBufferHandle();
                    outputBuffer.stream         = (camera3_stream_t*)m_pMergeYuvStream;
                    outputBuffer.acquire_fence  = -1;
                }
                else
                {
                    CHX_LOG_ERROR("FeatureSWMF GetImageBuffer failed, No RDI buffer for frameNumber %d", resultFrameNum);
                    result = CDKResultEFailed;
                }

                if (CDKResultSuccess == result)
                {
                    PipelineData* pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_mergeSessionId, 0));
                    UINT          requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);
                    UINT32        frameIndex     = (resultFrameNum % MaxOutstandingRequests);

                    camera3_stream_buffer_t inputBuffer[MaxMultiFrames] = { { 0 } };

                    camera_metadata_t*      pRequestMetadata            =
                        reinterpret_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(m_pSnapshotInputMeta[frameIndex]));

                    pPipelineData->seqIdToFrameNum[requestIdIndex]      = resultFrameNum;

                    // find the first ReqId for this snapshot
                    UINT32 firstReqId = offlineReqId - m_numBayer2YuvFrames + 1;
                    CHX_LOG("firstReqId:%d", firstReqId);

                    CHICAPTUREREQUEST request       = { 0 };
                    request.frameNumber             = pPipelineData->seqId++;
                    request.pPrivData               = &m_privData[request.frameNumber % MaxOutstandingRequests];
                    request.pPrivData->featureType  = GetFeatureType();

                    for (UINT32 i = 0; i < m_numBayer2YuvFrames; i++)
                    {
                        CHX_LOG("%d, get input buffer from reqid:%d", i, firstReqId + i);
                        m_pUsecase->GetTargetBuffer(firstReqId + i,
                                                    m_pBayer2YuvTargetBuffer,
                                                    pRequestMetadata,
                                                    &inputBuffer[i]);
                        inputBuffer[i].stream = reinterpret_cast<camera3_stream_t*>(m_pMergePipelineInputStreams[i]);

                        request.pPrivData->inputBuffers[i]      = inputBuffer[i].buffer;
                        request.pPrivData->bufferManagers[i]    = m_pBayer2YuvTargetBuffer->pBufferManager;
                    }
                    request.pPrivData->numInputBuffers = m_numBayer2YuvFrames;

                    const Session*  pSession  = m_pUsecase->GetSessionData(m_mergeSessionId)->pSession;

                    request.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
                    request.numInputs         = m_numBayer2YuvFrames;
                    request.numOutputs        = m_snapshotBufferNum[frameIndex];
                    request.pInputBuffers     = reinterpret_cast<CHISTREAMBUFFER*>(&inputBuffer[0]);
                    request.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(&outputBuffer);
                    request.pMetadata         = pRequestMetadata;

                    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
                    {
                        request.pMetadata     = m_pBayer2YuvTargetBuffer->bufferQueue[firstReqId % BufferQueueDepth].pMetadata;
                    }

                    CHIPIPELINEREQUEST submitRequest    = { 0 };
                    submitRequest.pSessionHandle        = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
                    submitRequest.numRequests           = 1;
                    submitRequest.pCaptureRequests      = &request;

                    CHX_LOG_INFO(
                        "FeatureSWMF Sending SWMF request pipelineIdx: %d frameNumber:%d, request:%" PRIu64 " metadata:%p",
                        m_mergePipelineIndex, resultFrameNum, request.frameNumber, request.pMetadata);

                    m_pUsecase->SetRequestToFeatureMapping(m_mergeSessionId, request.frameNumber, this);

                    ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

                    ChxUtils::FreeMetaData(pRequestMetadata);
                    pRequestMetadata = NULL;
                }
                else
                {
                    CHX_LOG("FeatureSWMF request is not submitted to merge pipeline! frameNumber = %d", resultFrameNum);
                }
            }
        }
        else if (m_pMergeYuvStream == pResult->pOutputBuffers[j].pStream)
        {
            CHX_LOG("FeatureSWMF Received MergeYuvStream stream, reqid:%d, frame num:%d", offlineReqId, resultFrameNum);
            UINT32 queueIndex = (resultFrameNum % BufferQueueDepth);

            ChxUtils::Memcpy(m_pMergeTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer,
                             &pResult->pOutputBuffers[j],
                             sizeof(CHISTREAMBUFFER));

            m_pMergeTargetBuffer->pBufferManager->ReleaseReference(
                reinterpret_cast<buffer_handle_t*>(pResult->pOutputBuffers[j].phBuffer));
            m_pMergeTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer->acquireFence = -1;

            // Release reference to the input buffers of the request to the MergeYUVPipeline
            for (UINT32 i = 0; i < pResult->pPrivData->numInputBuffers; i++)
            {
                if ((NULL != pResult->pPrivData->bufferManagers[i]) && (NULL != pResult->pPrivData->inputBuffers[i]))
                {
                    reinterpret_cast<CHIBufferManager*>(pResult->pPrivData->bufferManagers[i])->
                        ReleaseReference(reinterpret_cast<buffer_handle_t*>(pResult->pPrivData->inputBuffers[i]));
                    pResult->pPrivData->bufferManagers[i] = NULL;
                    pResult->pPrivData->inputBuffers[i]   = NULL;
                }
            }

            // If we have the requirements for the snapshot frame, kick it off immediately
            if (TRUE == m_isSnapshotFrame[resultFrameIndex])
            {
                SubmitRequest(m_snapshotSessionId, resultFrameNum, m_pMergeTargetBuffer,
                              &m_snapshotBuffers[resultFrameNum % MaxOutstandingRequests][0], 0);
            }
        }
        // Otherwise queue a buffer as part of the normal result
        else
        {
            if (m_pSnapshotStream == pResult->pOutputBuffers[j].pStream)
            {
                CHX_LOG("FeatureSWMF Received Final Snapshot(JPEG) stream, reqId:%d, frameNum:%d", offlineReqId, resultFrameNum);
            }

            // Release reference to the input buffers of the request to the SnapshotPipeline
            for (UINT32 i = 0; i < pResult->pPrivData->numInputBuffers; i++)
            {
                if ((NULL != pResult->pPrivData->bufferManagers[i]) && (NULL != pResult->pPrivData->inputBuffers[i]))
                {
                    reinterpret_cast<CHIBufferManager*>(pResult->pPrivData->bufferManagers[i])->
                        ReleaseReference(reinterpret_cast<buffer_handle_t*>(pResult->pPrivData->inputBuffers[i]));
                    pResult->pPrivData->bufferManagers[i] = NULL;
                    pResult->pPrivData->inputBuffers[i]   = NULL;
                }
            }

            m_pUsecase->GetAppResultMutex()->Lock();
            camera3_stream_buffer_t* pResultBuffer =
                const_cast<camera3_stream_buffer_t*>(&pUsecaseResult->output_buffers[pUsecaseResult->num_output_buffers++]);

            ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[j], sizeof(camera3_stream_buffer_t));
            m_pUsecase->GetAppResultMutex()->Unlock();

            isAppResultsAvailable = TRUE;
        }
    }

    if (TRUE == isAppResultsAvailable)
    {
        CHX_LOG("isAppResultsAvailable:%d", isAppResultsAvailable);

            m_pUsecase->ProcessAndReturnFinishedResults();

    }

    if ((1 <= ExtensionModule::GetInstance()->EnableDumpDebugData())    &&
        (NULL != pResult->pResultMetadata)                              &&
        (m_previewSessionId != pCbData->sessionId)                      &&
        (m_mergeSessionId   != pCbData->sessionId))
    {
        // Process debug-data only for offline processing
        m_pUsecase->ProcessDebugData(pResult, pPrivateCallbackData, resultFrameNum);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMultiframe::ProcessMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMultiframe::ProcessMessage(
    const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{
    (VOID)pMessageDescriptor;
    (VOID)pPrivateCallbackData;
    //@todo this function to be REMOVED
}

INT32 FeatureMultiframe::GetRequiredPipelines(
    AdvancedPipelineType* pPipelines,
    INT32 size)
{
    INT32 count = 0;
    const INT32 maxPipelineCount = 8;

    if (NULL != pPipelines && size >= maxPipelineCount)
    {
        INT32 index = 0;
        AdvancedPipelineType pipelineGroup[1];
        UINT                 cameraId[1];
        UINT                 wideCameraId = m_pUsecase->GetPhysicalCameraId(CameraRoleTypeWide);

        pPipelines[index]    = AdvancedPipelineType::InternalZSLYuv2JpegType;
        pipelineGroup[0]     = pPipelines[index];
        cameraId[0]          = wideCameraId;
        m_snapshotSessionId  = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
        m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
        index++;

        pPipelines[index]    = AdvancedPipelineType::SWMFMergeYuvType;
        pipelineGroup[0]     = pPipelines[index];
        cameraId[0]          = wideCameraId;
        m_mergeSessionId     = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
        m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
        index++;

        pPipelines[index]    = AdvancedPipelineType::ZSLSnapshotYUVType;
        pipelineGroup[0]     = pPipelines[index];
        cameraId[0]          = wideCameraId;
        m_bayer2YuvSessionId = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
        m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
        index++;

        if (TRUE == m_pUsecase->IsMultiCameraUsecase())
        {
            UINT teleCameraId   = m_pUsecase->GetPhysicalCameraId(CameraRoleTypeTele);

            pPipelines[index]       = AdvancedPipelineType::ZSLSnapshotYUVAuxType;
            pipelineGroup[0]        = pPipelines[index];
            cameraId[0]             = teleCameraId;
            m_bayer2YuvAuxSessionId = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
            m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
            index++;

            CHX_LOG("FeatureSWMF add Bayer2Yuv pipeline for aux camera(%d), m_bayer2YuvAuxSessionId:%d",
                teleCameraId, m_bayer2YuvAuxSessionId);
        }
        else
        {
            m_bayer2YuvAuxSessionId = InvalidSessionId;
        }

        if (FALSE == m_pUsecase->IsMultiCameraUsecase())
        {
            pPipelines[index]    = AdvancedPipelineType::ZSLPreviewRawType;
            pipelineGroup[0]     = pPipelines[index];
            cameraId[0]          = wideCameraId;
            m_previewSessionId   = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
            m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], 1);
            index++;
        }
        else
        {
            m_previewSessionId = InvalidSessionId;
            CHX_LOG("Don't use Realtime pipeline in advance usecase for multicamera usecase");
        }

        CHX_LOG("m_previewSessionId:%d, m_bayer2YuvSessionId:%d, m_mergeSessionId:%d, m_snapshotSessionId:%d",
            m_previewSessionId,
            m_bayer2YuvSessionId,
            m_mergeSessionId,
            m_snapshotSessionId);

        count = index;
    }

    CHX_LOG("FeatureSWMF::GetRequiredPipelines, required pipeline count:%d", count);
    return count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMultiframe::SubmitRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMultiframe::SubmitRequest(UINT32 sessionIdx, UINT frameNumber,
                                      TargetBuffer* pInputTargetBuffer,
                                      camera3_stream_buffer_t *pOutputBuffer,
                                      UINT32 inputPipelineReqId)
{
    CDKResult       result        = CDKResultSuccess;
    const Session* pSession       = m_pUsecase->GetSessionData(sessionIdx)->pSession;
    PipelineData*  pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(sessionIdx, 0));
    UINT           requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);
    UINT32         frameIndex     = (frameNumber % MaxOutstandingRequests);

    camera3_stream_buffer_t inputBuffer         = { 0 };
    // Create copy so that future updates to input metadata don't free it
    camera_metadata_t*      pRequestMetadata    =
        reinterpret_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(m_pSnapshotInputMeta[frameIndex]));

    pPipelineData->seqIdToFrameNum[requestIdIndex] = frameNumber;

    camera_metadata_t* pRDIMeta                    = NULL;

    CHIBufferManager* pInputBufferManager = NULL;
    if ((TRUE == m_pUsecase->IsMultiCameraUsecase()) &&
        ((sessionIdx == m_bayer2YuvAuxSessionId) || (sessionIdx == m_bayer2YuvSessionId)))
    {
        // here to get RDI buffer from external Queue
        m_pUsecase->GetInputBufferFromRDIQueue(inputPipelineReqId,
                                               m_activePipelineID,
                                               &inputBuffer,
                                               &pRDIMeta);

        CHX_LOG("Get RDI Buffer reqId:%d, masterCameraId:%d, activePipelineID:%d, buffer handle:%p, meta:%p,stream:%p",
            inputPipelineReqId, m_masterCameraId, m_activePipelineID, inputBuffer.buffer, pRDIMeta, inputBuffer.stream);
        pInputBufferManager = m_pUsecase->GetBufferManager(m_activePipelineID);

    }
    else
    {
        if (inputPipelineReqId != 0)
        {
            CHX_LOG("get input buffer from reqid:%d", inputPipelineReqId);
            m_pUsecase->GetTargetBuffer(inputPipelineReqId,
                                        pInputTargetBuffer,
                                        pRequestMetadata,
                                        &inputBuffer);
        }
        else
        {
            m_pUsecase->GetTargetBuffer(frameNumber,
                                        pInputTargetBuffer,
                                        pRequestMetadata,
                                        &inputBuffer);
        }
        pInputBufferManager = pInputTargetBuffer->pBufferManager;
    }

    if (m_snapshotSessionId == sessionIdx)
    {
        CHX_LOG("Override input stream to JPEGInputStream:%p", m_pJPEGInputStream);
        inputBuffer.stream = reinterpret_cast<camera3_stream_t*>(m_pJPEGInputStream);
    }

    CHICAPTUREREQUEST request = { 0 };
    request.frameNumber       = pPipelineData->seqId++;
    request.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
    request.numInputs         = 1;
    request.numOutputs        = m_snapshotBufferNum[frameIndex];
    request.pInputBuffers     = reinterpret_cast<CHISTREAMBUFFER*>(&inputBuffer);
    request.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(pOutputBuffer);
    request.pMetadata         = pRequestMetadata;

    if ((TRUE == m_pUsecase->IsMultiCameraUsecase()) &&
        ((sessionIdx == m_bayer2YuvSessionId) || (sessionIdx == m_bayer2YuvAuxSessionId)))
    {
        request.pMetadata = pRDIMeta;
    }
    else if ((TRUE == m_pUsecase->IsMultiCameraUsecase()) && (NULL == request.pMetadata))
    {
        request.pMetadata = pInputTargetBuffer->bufferQueue[frameNumber % BufferQueueDepth].pMetadata;
        CHX_LOG("Get meta directly from result Queue. pMeta:%p", request.pMetadata);
    }

    request.pPrivData                       = &m_privData[request.frameNumber % MaxOutstandingRequests];
    request.pPrivData->featureType          = FeatureType::SWMF;

    // Save input buffers info for later releasing reference
    request.pPrivData->numInputBuffers      = 1;
    request.pPrivData->inputBuffers[0]      = inputBuffer.buffer;
    request.pPrivData->bufferManagers[0]    = pInputBufferManager;

    // Debug-data deep-copy
    if (TRUE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(pRequestMetadata), DebugDataTag))
    {
        CHAR* pData = NULL;
        ChxUtils::GetVendorTagValue((VOID*)(pRequestMetadata), DebugDataTag, (VOID**)&pData);
        DebugData* pDebug = reinterpret_cast<DebugData*>(pData);

        if ((NULL != pDebug->pData) && (0 < pDebug->size))
        {
            // Allocate debug-data for offline processing
            if (NULL == m_debugDataOffline.pData)
            {
                m_debugDataOffline.pData = CHX_CALLOC(pDebug->size);
                if (NULL != m_debugDataOffline.pData)
                {
                    m_debugDataOffline.size = pDebug->size;
                }
                else
                {
                    m_debugDataOffline.size = 0;
                }
            }
            else if (pDebug->size != m_debugDataOffline.size)
            {
                // This condition is only for protection in case debug-data size is change in run time while camera is
                // already processing. This is not allow for this property at this time.
                CHX_FREE(m_debugDataOffline.pData);
                m_debugDataOffline.pData    = NULL;
                m_debugDataOffline.size     = 0;

                m_debugDataOffline.pData = CHX_CALLOC(pDebug->size);
                if (NULL != m_debugDataOffline.pData)
                {
                    m_debugDataOffline.size = pDebug->size;
                }
                else
                {
                    m_debugDataOffline.size = 0;
                }

            }

            if ((NULL != m_debugDataOffline.pData)  &&
                (m_debugDataOffline.pData != pDebug->pData)) // For snapshot same metadata buffer is use, so avoid copy.
            {
                ChxUtils::Memcpy(m_debugDataOffline.pData, pDebug->pData, pDebug->size);

                result = ChxUtils::SetVendorTagValue(pRequestMetadata,
                                                     DebugDataTag,
                                                     sizeof(DebugData),
                                                     &m_debugDataOffline);
                if (CDKResultSuccess != result)
                {
                    CHX_LOG_ERROR("DebugDataAll: Fail to set debugdata tag in offline input metadata");
                }
            }
        }
    }

    CHIPIPELINEREQUEST submitRequest    = { 0 };
    submitRequest.pSessionHandle        = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
    submitRequest.numRequests           = 1;
    submitRequest.pCaptureRequests      = &request;

    CHX_LOG_INFO("Sending SWMF request sessionIdx: %d frameNumber:%d, request:%" PRIu64 " metadata:%p",
        sessionIdx, frameNumber, request.frameNumber, request.pMetadata);

    m_pUsecase->SetRequestToFeatureMapping(sessionIdx, request.frameNumber, this);

    ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

    ChxUtils::FreeMetaData(pRequestMetadata);
    pRequestMetadata = NULL;
}
