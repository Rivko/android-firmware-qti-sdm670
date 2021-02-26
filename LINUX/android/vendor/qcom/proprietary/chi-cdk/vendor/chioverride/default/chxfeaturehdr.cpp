////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeaturehdr.cpp
/// @brief CHX HDR feature class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxadvancedcamerausecase.h"
#include "chxincs.h"
#include "chxfeaturehdr.h"
#include "chxusecase.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureHDR::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FeatureHDR* FeatureHDR::Create(
    AdvancedCameraUsecase* pUsecase)
{
    FeatureHDR* pFeature = CHX_NEW FeatureHDR;

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
/// FeatureHDR::Pause
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureHDR::Pause(
    BOOL isForced)
{
    CHX_LOG("Feature Pause isForced =%d", isForced);
    //Implement if required
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureHDR::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureHDR::Flush(
    BOOL isInProgress)
{
    CHX_LOG("Feature Flush isInProgress =%d", isInProgress);
    //Implement if required
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureHDR::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureHDR::Initialize(
    AdvancedCameraUsecase* pUsecase)
{
    m_pUsecase          = pUsecase;

    m_pMergeYuvStream   = static_cast<CHISTREAM*>(CHX_CALLOC(sizeof(CHISTREAM)));

    for (UINT32 i = 0; i < MaxInputPorts; i++)
    {
        m_pMergePipelineInputStreams[i] = static_cast<CHISTREAM*>(CHX_CALLOC(sizeof(CHISTREAM)));
    }

    m_previewRawReqId       = 0;
    m_lastShutterFrameNum   = 0;

    m_snapshotSessionId     = InvalidSessionId;
    m_mergeSessionId        = InvalidSessionId;
    m_bayer2YuvSessionId    = InvalidSessionId;
    m_previewSessionId      = InvalidSessionId;
    m_bayer2YuvAuxSessionId = InvalidSessionId;

    CHX_LOG("FeatureHDR m_pRdiStream: %p, m_pBayer2YuvStream: %p, m_pMergeYuvStream: %p",
            m_pRdiStream, m_pBayer2YuvStream, m_pMergeYuvStream);

    ExtensionModule::GetInstance()->GetVendorTagOps(&m_vendorTagOps);
    CHX_LOG("pGetMetaData:%p, pSetMetaData:%p", m_vendorTagOps.pGetMetaData, m_vendorTagOps.pSetMetaData);

    m_numAeBracketFrames = 3;
    m_expValues[0] = -6;
    m_expValues[1] = 0;
    m_expValues[2] = 6;
    CHX_LOG("default m_numAeBracketFrames:%d, ev bracket (%d, %d, %d)",
        m_numAeBracketFrames, m_expValues[0], m_expValues[1], m_expValues[2]);

    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        CHX_LOG("FeatureHDR Multi camera usecase, cameraId[0]:%d, cameraId[1]:%d",
            m_pUsecase->GetPhysicalCameraId(CameraRoleTypeWide),
            m_pUsecase->GetPhysicalCameraId(CameraRoleTypeTele));
    }

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureHDR::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureHDR::Destroy(BOOL isForced)
{
    CHX_LOG("FeatureHDR::Destroy(), isForced %d E.", isForced);

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

    for (UINT32 i = 0; i < MaxMultiFrames; i++)
    {
        if (NULL != m_pOverrideAppSetting[i])
        {
            ChxUtils::FreeMetaData(m_pOverrideAppSetting[i]);
            m_pOverrideAppSetting[i] = NULL;
        }
    }

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
/// FeatureHDR::OverrideUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiUsecase* FeatureHDR::OverrideUsecase(
    LogicalCameraInfo*              pCameraInfo,
    camera3_stream_configuration_t* pStreamConfig)
{
    (VOID)pCameraInfo;
    CHX_LOG("OverrideUsecase for HDR");
    CHX_ASSERT(2 == pStreamConfig->num_streams);

    m_pChiUsecase = m_pUsecase->GetChiUseCase();

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
    m_mergePipelineIndex     = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::Merge3YuvCustomTo1YuvType);
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

    // HDR Stage-1 (BayerToYuv)
    pBayer2YuvDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStream;
    pBayer2YuvDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pBayer2YuvStream;

    // HDR Stage-2 (Merge)
    CHX_LOG("Merge Pipeline source target:%d", pMergeDesc->sourceTarget.numTargets);
    ChxUtils::Memset(m_mergePipelineInputTargets, 0, sizeof(m_mergePipelineInputTargets));
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

    pMergeDesc->sinkTarget.pTargetPortDesc[0].pTarget = &m_mergePipelineOutputTarget;
    pMergeDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream       = m_pMergeYuvStream;

    // HDR Stage-3 (Jpeg)
    pSnapshotDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream  = m_pJPEGInputStream;
    pSnapshotDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream    = m_pSnapshotStream;

    CHX_LOG("m_rdiStreamIndex:%d, m_Bayer2YuvStreamIndex:%d, m_mergeStreamIndex:%d",
        m_rdiStreamIndex, m_Bayer2YuvStreamIndex, m_mergeStreamIndex);

    CHX_LOG("FeatureHDR RdiStream: %p, PreviewStream: %p, SnapshotStream: %p, Bayer2YuvStream: %p, JPEGInputStream:%p",
            m_pRdiStream, m_pPreviewStream, m_pSnapshotStream, m_pBayer2YuvStream, m_pJPEGInputStream);

    // For Aux camera in dual camera usecase
    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        CHX_LOG("FeatureHDR Override usecase for aux camera");
        m_pRdiStreamAux             = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::RdiStreamAux);
        m_bayer2YuvAuxPipelineIndex =
            m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLSnapshotYUVAuxType);

        ChiPipelineTargetCreateDescriptor* pBayer2YuvAuxDesc =
            &m_pChiUsecase->pPipelineTargetCreateDesc[m_bayer2YuvAuxPipelineIndex];

        pBayer2YuvAuxDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStreamAux;
        pBayer2YuvAuxDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pBayer2YuvStream;

        CHX_LOG("FeatureHDR RdiStreamAux:%p, m_bayer2YuvAuxPipelineIndex:%d",
            m_pRdiStreamAux, m_bayer2YuvAuxPipelineIndex);
    }

    return m_pChiUsecase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureHDR::PipelineCreated
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureHDR::PipelineCreated(
    UINT32 sessionId,    ///< Id of session created
    UINT32 pipelineId)   ///< Pipeline of session created
{
    ChiSensorModeInfo*              pSensorInfo     = NULL;
    const ChiPipelineInputOptions*  pInputOptions   = NULL;

    /// @todo need to find the mapping between session's pipelineId vs XMLs pipelineId
    pSensorInfo     = m_pUsecase->GetSessionData(sessionId)->pipelines[0].pPipeline->GetSensorModeInfo();
    pInputOptions   = m_pUsecase->GetSessionData(sessionId)->pipelines[0].pPipeline->GetInputOptions();

    CHX_LOG("FeatureHDR sessionId:%d, pipelineId: %d, Sensor widthxheight: %d x %d, InputOptions widthxheight: %d x %d",
                  sessionId,
                  pipelineId,
                  pSensorInfo->frameDimension.width, pSensorInfo->frameDimension.height,
                  pInputOptions->bufferOptions.optimalDimension.width,
                  pInputOptions->bufferOptions.optimalDimension.height);

    // FEATURE_AEBRACKET_PORT
    /// @todo - Need to find means of associating grallocUsage
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

            m_pMergeOutputBufferManager = CHIBufferManager::Create("HDRMergeBufferManager", &createData);
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
        case AdvancedPipelineType::Merge3YuvCustomTo1YuvType:
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
        case AdvancedPipelineType::ZSLSnapshotYUVAuxType:
        {
            // for aux camera, RDI stream always from mc usecase, just do sanity check here
            if (NULL != m_pRdiStreamAux)
            {
                CHX_LOG("RdiStreamAux already configured, wxh:%dx%d", m_pRdiStreamAux->width, m_pRdiStreamAux->height);
            }
            else
            {
                CHX_LOG("Warning: RdiStreamAux is still NULL!");
            }
            break;
        }
        default:
        {
            CHX_LOG("FeatureHDR pipeline not used in HDR.");
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureHDR::ExecuteProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult  FeatureHDR::ExecuteProcessRequest(
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

    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        RequestMapInfo requestMapInfo = m_pUsecase->GetRequestMapInfo(pRequest->frame_number);
        m_masterCameraId              = requestMapInfo.masterCameraID;
        m_activePipelineID            = requestMapInfo.activePipelineID;

        if (requestMapInfo.frameNumber != m_snapshotAppFrameNum)
        {
            CHX_LOG("ERROR: AppFrameNumbre not match! requestMapInfo.frameNumber:%d, m_snapshotAppFrameNum:%d",
                requestMapInfo.frameNumber, m_snapshotAppFrameNum);
            result = CDKResultEFailed;
        }

        CHX_LOG("FeatureHDR Snapshot Request for dual camera!, rtReqId:%d, AppFrameNum:%d," \
            "masterCameraId:%d, activePipelineID:%d",
            pRequest->frame_number, m_snapshotAppFrameNum, m_masterCameraId, m_activePipelineID);

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
            m_snapshotAppFrameNum, bayer2YuvReqId, pRequest->frame_number, sessionId);

        camera3_stream_buffer_t outputBuffer = {0};
        m_pBayer2YuvTargetBuffer->bufferQueue[bayer2YuvReqId % BufferQueueDepth].pRdiOutputBuffer->phBuffer =
            m_pBayer2YuvTargetBuffer->pBufferManager->GetImageBuffer()->GetBufferHandle();
        ChxUtils::Memcpy(&outputBuffer,
                         m_pBayer2YuvTargetBuffer->bufferQueue[bayer2YuvReqId % BufferQueueDepth].pRdiOutputBuffer,
                         sizeof(camera3_stream_buffer_t));
        outputBuffer.acquire_fence = -1;

        SubmitRequest(sessionId, m_snapshotAppFrameNum, m_pRdiTargetBuffer,
                      &outputBuffer, pRequest->frame_number);

        return result;
    }

    CHX_LOG("FeatureHDR::ExecuteProcessRequest, frameNum:%d, setting:%p", pRequest->frame_number, pRequest->settings);

    pUsecaseResult->result             = NULL;
    pUsecaseResult->frame_number       = pRequest->frame_number;
    pUsecaseResult->num_output_buffers = 0;


    m_isSkipPreview[frameIndex]       = FALSE;
    m_isSnapshotFrame[frameIndex]     = FALSE;
    m_shutterTimestamp[frameIndex]    = 0;

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
            /*ChxUtils::FreeMetaData(m_pSnapshotInputMeta[snapshotReqIdIndex]);
            m_pSnapshotInputMeta[snapshotReqIdIndex]    =
                ChxUtils::AllocateCopyMetaData(reinterpret_cast<const VOID*>(pRequest->settings));

            ChxUtils::FreeMetaData(m_pBayer2YuvInputMeta[snapshotReqIdIndex]);
            m_pBayer2YuvInputMeta[snapshotReqIdIndex]   =
                ChxUtils::AllocateCopyMetaData(reinterpret_cast<const VOID*>(pRequest->settings));

            ChxUtils::FreeMetaData(m_pMergeInputMeta[snapshotReqIdIndex]);
            m_pMergeInputMeta[snapshotReqIdIndex]       =
                ChxUtils::AllocateCopyMetaData(reinterpret_cast<const VOID*>(pRequest->settings));*/

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
        }
    }

    if (snapshotCount == 0)
    {
        PipelineData* pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_previewSessionId, 0));
        UINT          requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);

        pPipelineData->seqIdToFrameNum[requestIdIndex] = frameNumber;

        CHX_LOG("FeatureHDR Realtime AppFrameNum to ReqId: %d <--> %d", frameNumber, pPipelineData->seqId);

        m_previewRawReqId = pPipelineData->seqId;

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
            CHX_LOG_ERROR("FeatureHDR: GetImageBuffer failed, No RDI buffer for frameNumber %d", frameNumber);
        }

        const Session* pSession = m_pUsecase->GetSessionData(m_previewSessionId)->pSession;

        CHICAPTUREREQUEST request = { 0 };
        request.frameNumber       = pPipelineData->seqId++;
        request.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
        request.numOutputs        = previewCount;
        request.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(previewBuffers);
        request.pMetadata         = pRequest->settings;

        request.pPrivData              = &m_privData[request.frameNumber % MaxOutstandingRequests];
        request.pPrivData->featureType = FeatureType::HDR;

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

        CHX_LOG("Preview/RDI request, appFrameNum:%d <--> real time pipeline ReqId:%d",
            pRequest->frame_number, m_previewRawReqId);
    }
    else
    {
        m_isSnapshotFrame[frameIndex]                 = TRUE;
        m_snapshotBufferNum[frameIndex]       = snapshotCount;
        m_snapshotReqIdToFrameNum[frameIndex] = frameNumber;
        m_isSkipPreview[frameIndex]           = TRUE;
        m_numBayer2YuvFrames                  = 0;
        m_maxSnapshotReqId++;

        CHX_LOG("FeatureHDR Snapshot Request received, appFrameNum:%d frame index %d",
                pRequest->frame_number, frameIndex);

        result = GenerateAEBracketRequest(pRequest);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureHDR::ProcessResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureHDR::ProcessResult(
    CHICAPTURERESULT*   pResult,
    VOID*               pPrivateCallbackData)
{
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
        PipelineData*     pPipelineData =
            const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_previewSessionId, 0));
        resultFrameNum = pPipelineData->seqIdToFrameNum[pResult->frameworkFrameNum % MaxOutstandingRequests];
        realtimeReqId = pResult->frameworkFrameNum;
        CHX_LOG("Realtime ReqId to AppFrameNum: %d <--> %d", pResult->frameworkFrameNum, resultFrameNum);
    }

    CHX_LOG("FeatureHDR ProcessResult resultFrameNum: %d, sessionId: %d, pResultMetadata: %p, numOutputBuffers: %d",
            resultFrameNum, pCbData->sessionId, pResult->pResultMetadata, pResult->numOutputBuffers);

    UINT32                    resultFrameIndex = resultFrameNum % MaxOutstandingRequests;
    camera3_capture_result_t* pUsecaseResult   = m_pUsecase->GetCaptureResult(resultFrameIndex);

    pUsecaseResult->frame_number = resultFrameNum;

    // If result contain metadata and metadata has not been sent to framework
    if ((NULL != pResult->pResultMetadata))
    {
        BOOL               isResultMetadataAvailable = FALSE;

        UINT64             timestamp                 = m_shutterTimestamp[resultFrameNum % MaxOutstandingRequests];
        if (0 == timestamp)
        {
            timestamp                 = m_pUsecase->GetRequestShutterTimestamp(resultFrameNum);
        }

        camera_metadata_t* pMetadata                 =
            const_cast<camera_metadata_t*>(static_cast<const camera_metadata_t*>(pResult->pResultMetadata));

        if (m_previewSessionId == pCbData->sessionId)
        {
            // UINT32 zslQueueIndex = (resultFrameNum % BufferQueueDepth);
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

            m_pBayer2YuvTargetBuffer->bufferQueue[bufferQueueIdx].frameNumber = pResult->frameworkFrameNum;
        }
        else if (m_mergeSessionId == pCbData->sessionId)
        {
            CHX_LOG("meta for Merge Pipeline, frame number:%d, meta:%p", resultFrameNum, pMetadata);

            UINT32 queueIndex = (resultFrameNum % BufferQueueDepth);

            // Create copy because this pointer could be freed by app as soon as result returned
            ChxUtils::FreeMetaData(m_pMergeTargetBuffer->bufferQueue[queueIndex].pMetadata);
            /// @todo This currently copies the debugData pointer, to memory owned by the first pipeline. Should copy
            m_pMergeTargetBuffer->bufferQueue[queueIndex].pMetadata =
                static_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(pMetadata));

            m_pMergeTargetBuffer->bufferQueue[queueIndex].frameNumber = pResult->frameworkFrameNum;
        }

        if ((FALSE == m_pUsecase->IsMetadataSent(resultFrameIndex)))
        {
            // Do Not wait for Snapshot frame metadata, Return Preview metadata back to fwk
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

                CHX_LOG("Update sensor timestamp for frame %d, timestamp = %" PRIu64 "  Status %d", resultFrameNum, timestamp, status);

                if (TRUE == isResultMetadataAvailable)
                {
                    m_pUsecase->SetMetadataAvailable(resultFrameIndex);
                    isAppResultsAvailable = TRUE;

                    CHX_LOG("returnmetadata HDR JPEG metadata：%d", resultFrameNum);
                }
            }
        }
    }

    for (UINT32 j = 0; j < pResult->numOutputBuffers; j++)
    {
        // If our internal stream, copy the result into the target buffer to be consumed by the offline pipeline
        if (m_pRdiStream == pResult->pOutputBuffers[j].pStream)
        {
            UINT32 zslQueueIndex = (realtimeReqId % BufferQueueDepth);

            m_pRdiTargetBuffer->pMutex->Lock();

            // Release reference to the buffer that's going to be replaced out of the queue
            if (realtimeReqId >= BufferQueueDepth)
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
                m_pRdiTargetBuffer->lastReadySequenceID = realtimeReqId;
            }

            m_pRdiTargetBuffer->pMutex->Unlock();

            // If we have the requirements for the snapshot frame, kick it off immediately
            if (TRUE == m_isSnapshotFrame[resultFrameIndex])
            {
                CHX_LOG("FeatureHDR send buffer to Bayer2YuvPipelineIndex, frame number:%d, buffer:%p, meta:%p", resultFrameNum,
                    m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pRdiOutputBuffer,
                    m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pMetadata);

                const PipelineData* pPipelineData = m_pUsecase->GetPipelineData(m_bayer2YuvSessionId, 0);
                UINT32 bayer2YuvReqId = pPipelineData->seqId;
                CHX_LOG("AppFrameNum:%d <--> bayer2YuvReqId:%d, input RDI request id:%d",
                    resultFrameNum, bayer2YuvReqId, realtimeReqId);

                CHX_LOG("m_pBayer2YuvTargetBuffer:%p, pRdiOutputBuffer:%p",
                    m_pBayer2YuvTargetBuffer,
                    m_pBayer2YuvTargetBuffer->bufferQueue[bayer2YuvReqId % BufferQueueDepth].pRdiOutputBuffer);
                camera3_stream_buffer_t outputBuffer = {0};
                m_pBayer2YuvTargetBuffer->bufferQueue[bayer2YuvReqId % BufferQueueDepth].pRdiOutputBuffer->phBuffer =
                    m_pBayer2YuvTargetBuffer->pBufferManager->GetImageBuffer()->GetBufferHandle();
                ChxUtils::Memcpy(&outputBuffer,
                     m_pBayer2YuvTargetBuffer->bufferQueue[bayer2YuvReqId % BufferQueueDepth].pRdiOutputBuffer,
                     sizeof(camera3_stream_buffer_t));
                outputBuffer.acquire_fence = -1;
                SubmitRequest(m_bayer2YuvSessionId, resultFrameNum, m_pRdiTargetBuffer,
                              &outputBuffer, realtimeReqId);
            }
        }
        else if (m_pBayer2YuvStream == pResult->pOutputBuffers[j].pStream)
        {
            CHX_LOG("FeatureHDR Received Bayer2Yuv stream, reqid:%d, frame num:%d", offlineReqId, resultFrameNum);

            UINT32 bufferQueueIdx = (offlineReqId % BufferQueueDepth);
            ChxUtils::Memcpy(m_pBayer2YuvTargetBuffer->bufferQueue[bufferQueueIdx].pRdiOutputBuffer,
                             &pResult->pOutputBuffers[j],
                             sizeof(CHISTREAMBUFFER));
            m_pBayer2YuvTargetBuffer->pBufferManager->ReleaseReference(
                reinterpret_cast<buffer_handle_t*>(pResult->pOutputBuffers[j].phBuffer));
            m_pBayer2YuvTargetBuffer->bufferQueue[bufferQueueIdx].pRdiOutputBuffer->acquireFence = -1;

            m_numBayer2YuvFrames++;
            CHX_LOG("received bayer2Yuv frames:%d", m_numBayer2YuvFrames);

            // Release reference to the input buffers of the request to the Bayer2YuvPipeline
            for(UINT32 i = 0; i < pResult->pPrivData->numInputBuffers; i++)
            {
                if ((NULL != pResult->pPrivData->bufferManagers[i]) && (NULL != pResult->pPrivData->inputBuffers[i]))
                {
                    reinterpret_cast<CHIBufferManager*>(pResult->pPrivData->bufferManagers[i])->ReleaseReference(
                        reinterpret_cast<buffer_handle_t*>(pResult->pPrivData->inputBuffers[i]));
                    pResult->pPrivData->bufferManagers[i] = NULL;
                    pResult->pPrivData->inputBuffers[i]   = NULL;
                }
            }

            // submit request to merge pipeline until we got all buffers and frames.
            // because meta alway comes before or along with frames, so here it's sure meta is received.
            if ((TRUE == m_isSnapshotFrame[resultFrameIndex]) && (m_numBayer2YuvFrames == m_numAeBracketFrames))
            {
                CHX_LOG("FeatureHDR received all bayer2Yuv Frames, send to merge pipeline..");

                camera3_stream_buffer_t outputBuffer = {0};
                m_pMergeTargetBuffer->bufferQueue[resultFrameNum % BufferQueueDepth].pRdiOutputBuffer->phBuffer =
                    m_pMergeTargetBuffer->pBufferManager->GetImageBuffer()->GetBufferHandle();
                ChxUtils::Memcpy(&outputBuffer,
                     m_pMergeTargetBuffer->bufferQueue[resultFrameNum % BufferQueueDepth].pRdiOutputBuffer,
                     sizeof(camera3_stream_buffer_t));
                outputBuffer.acquire_fence = -1;
                // SubmitRequest(MergePipelineIndex, resultFrameNum, m_pBayer2YuvTargetBuffer,
                //              &outputBuffer, offlineReqId);
                {
                    PipelineData*       pPipelineData   =
                        const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_mergeSessionId, 0));
                    UINT                requestIdIndex  = (pPipelineData->seqId % MaxOutstandingRequests);
                    UINT32              frameIndex      = (resultFrameNum % MaxOutstandingRequests);

                    camera3_stream_buffer_t inputBuffer[MaxMultiFrames] = { { 0 } };

                    camera_metadata_t*      pRequestMetadata    =
                        reinterpret_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(m_pSnapshotInputMeta[frameIndex]));

                    pPipelineData->seqIdToFrameNum[requestIdIndex] = resultFrameNum;

                    // find the first ReqId for this snapshot
                    UINT32 firstReqId = offlineReqId - m_numBayer2YuvFrames + 1;
                    CHX_LOG("firstReqId:%d", firstReqId);

                    CHICAPTUREREQUEST request       = { 0 };
                    request.pPrivData               = &m_privData[request.frameNumber % MaxOutstandingRequests];
                    request.pPrivData->featureType  = FeatureType::HDR;

                    for (UINT32 i = 0; i < m_numBayer2YuvFrames; i++)
                    {
                        CHX_LOG("%d, get input buffer from reqid:%d", i, firstReqId + i);
                        m_pUsecase->GetTargetBuffer(firstReqId + i,
                                                    m_pBayer2YuvTargetBuffer,
                                                    pRequestMetadata,
                                                    &inputBuffer[i]);
                        inputBuffer[i].stream = reinterpret_cast<camera3_stream_t*>(m_pMergePipelineInputStreams[i]);
                        request.pPrivData->bufferManagers[i]    = m_pBayer2YuvTargetBuffer->pBufferManager;
                        request.pPrivData->inputBuffers[i]      = inputBuffer[i].buffer;
                    }
                    request.pPrivData->numInputBuffers = m_numBayer2YuvFrames;

                    const Session*          pSession            = m_pUsecase->GetSessionData(m_mergeSessionId)->pSession;

                    request.frameNumber       = pPipelineData->seqId++;
                    request.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
                    request.numInputs         = m_numBayer2YuvFrames;
                    request.numOutputs        = m_snapshotBufferNum[frameIndex];
                    request.pInputBuffers     = reinterpret_cast<CHISTREAMBUFFER*>(&inputBuffer[0]);
                    request.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(&outputBuffer);
                    request.pMetadata         = pRequestMetadata;
                    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
                    {
                        request.pMetadata = m_pBayer2YuvTargetBuffer->bufferQueue[firstReqId % BufferQueueDepth].pMetadata;
                    }

                    CHIPIPELINEREQUEST submitRequest    = { 0 };
                    submitRequest.pSessionHandle        = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
                    submitRequest.numRequests           = 1;
                    submitRequest.pCaptureRequests      = &request;

                    CHX_LOG_INFO(
                        "FeatureHDR Sending HDR request pipelineIdx: %d frameNumber:%d, request:%" PRIu64 " metadata:%p",
                        m_mergePipelineIndex, resultFrameNum, request.frameNumber, request.pMetadata);

                    m_pUsecase->SetRequestToFeatureMapping(m_mergeSessionId, request.frameNumber, this);

                    ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

                    ChxUtils::FreeMetaData(pRequestMetadata);
                    pRequestMetadata = NULL;
                }
            }
        }
        else if (m_pMergeYuvStream == pResult->pOutputBuffers[j].pStream)
        {
            CHX_LOG("FeatureHDR Received MergeYuvStream stream, reqid:%d, frame num:%d", offlineReqId, resultFrameNum);
            UINT32 queueIndex = (resultFrameNum % BufferQueueDepth);

            ChxUtils::Memcpy(m_pMergeTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer,
                             &pResult->pOutputBuffers[j],
                             sizeof(CHISTREAMBUFFER));
            m_pMergeTargetBuffer->pBufferManager->ReleaseReference(
                reinterpret_cast<buffer_handle_t*>(pResult->pOutputBuffers[j].phBuffer));
            m_pMergeTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer->acquireFence = -1;

            // Release reference to the input buffers of the request to the MergeYuvPipeline
            for(UINT32 i = 0; i < pResult->pPrivData->numInputBuffers; i++)
            {
                if ((NULL != pResult->pPrivData->bufferManagers[i]) && (NULL != pResult->pPrivData->inputBuffers[i]))
                {
                    reinterpret_cast<CHIBufferManager*>(pResult->pPrivData->bufferManagers[i])->ReleaseReference(
                        reinterpret_cast<buffer_handle_t*>(pResult->pPrivData->inputBuffers[i]));
                    pResult->pPrivData->bufferManagers[i] = NULL;
                    pResult->pPrivData->inputBuffers[i]   = NULL;
                }
            }

            // If we have the requirements for the snapshot frame, kick it off immediately
            if (TRUE == m_isSnapshotFrame[resultFrameIndex])
            {
                SubmitRequest(m_snapshotSessionId, resultFrameNum, m_pMergeTargetBuffer,
                              &m_snapshotBuffers[resultFrameNum % MaxOutstandingRequests][0], 0 /* resultFrameNum */);
            }
        }
        // Otherwise queue a buffer as part of the normal result
        else
        {
            UINT32 queueIndex = (resultFrameNum % MaxOutstandingRequests);
            if (m_pSnapshotStream == pResult->pOutputBuffers[j].pStream)
            {
                CHX_LOG("FeatureHDR Received Final Snapshot(JPEG) stream, reqId:%d, frameNum:%d", offlineReqId, resultFrameNum);
            }

            m_pUsecase->GetAppResultMutex()->Lock();
            camera3_stream_buffer_t* pResultBuffer =
                const_cast<camera3_stream_buffer_t*>(&pUsecaseResult->output_buffers[pUsecaseResult->num_output_buffers++]);

            ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[j], sizeof(camera3_stream_buffer_t));
            m_pUsecase->GetAppResultMutex()->Unlock();

            if ((m_pPreviewStream == pResult->pOutputBuffers[j].pStream) &&
                (TRUE             == m_isSkipPreview[queueIndex]))
            {
                CHX_LOG("Feature HDR, skip the preview for index: %d", queueIndex);
                m_isSkipPreview[queueIndex] = FALSE;
                ChxUtils::SkipPreviewFrame(pResultBuffer);
            }

            isAppResultsAvailable = TRUE;

            // Release reference to the input buffers of the request to the SnapshotPipeline
            for(UINT32 i = 0; i < pResult->pPrivData->numInputBuffers; i++)
            {
                if ((NULL != pResult->pPrivData->bufferManagers[i]) && (NULL != pResult->pPrivData->inputBuffers[i]))
                {
                    reinterpret_cast<CHIBufferManager*>(pResult->pPrivData->bufferManagers[i])->ReleaseReference(
                        reinterpret_cast<buffer_handle_t*>(pResult->pPrivData->inputBuffers[i]));
                    pResult->pPrivData->bufferManagers[i] = NULL;
                    pResult->pPrivData->inputBuffers[i]   = NULL;
                }
            }
        }
    }

    if (TRUE == isAppResultsAvailable)
    {
        CHX_LOG("isAppResultsAvailable:%d", isAppResultsAvailable);

        m_pUsecase->ProcessAndReturnFinishedResults();

    }

    if ((1 <= ExtensionModule::GetInstance()->EnableDumpDebugData())    &&
        (NULL != pResult->pResultMetadata)                              &&
        ((m_bayer2YuvSessionId  == pCbData->sessionId)                  ||
         (m_snapshotSessionId   == pCbData->sessionId)))
    {
        // Process debug-data only bayer2YUV & Snapshot sessions
        m_pUsecase->ProcessDebugData(pResult, pPrivateCallbackData, resultFrameNum);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureHDR::ProcessMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureHDR::ProcessMessage(
    const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{

    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        CHX_LOG("FeatureHDR Ignore shutter message for dual camera!");

        return;
    }

    if (ChiMessageTypeSof == pMessageDescriptor->messageType)
    {
        // SOF notifications are not sent to the HAL3 application
        CHX_LOG("FeatureHDR Chi Notify SOF frameNum %u framework frameNum %u, timestamp %" PRIu64,
            pMessageDescriptor->message.sofMessage.sofId,
            pMessageDescriptor->message.sofMessage.frameworkFrameNum,
            pMessageDescriptor->message.sofMessage.timestamp);
    }
    else
    {
        SessionPrivateData* pCbData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
        BOOL                dropCb  = TRUE;

        if (m_previewSessionId != pCbData->sessionId)
        {
            return;
        }

        UINT32 rtResultId = pMessageDescriptor->message.shutterMessage.frameworkFrameNum;
        UINT32 reqIdIndex = (rtResultId % MaxOutstandingRequests);
        PipelineData*  pPipelineData = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(pCbData->sessionId, 0));
        UINT32 resultFrameNum = pPipelineData->seqIdToFrameNum[reqIdIndex];

        CHX_LOG("FeatureHDR Shutter Notify. ReqId:%d <-> AppFrameNum:%d, timestamp:%" PRIu64,
            pMessageDescriptor->message.shutterMessage.frameworkFrameNum,
            resultFrameNum,
            pMessageDescriptor->message.shutterMessage.timestamp);

        if (m_lastShutterFrameNum != 0 && resultFrameNum <= m_lastShutterFrameNum)
        {
            CHX_LOG("%s, current:%d, last:%d", __func__, resultFrameNum, m_lastShutterFrameNum);
            return;
        }

        m_lastShutterFrameNum = resultFrameNum;

        ChiMessageDescriptor* pOverrideMessageDescriptor = const_cast<ChiMessageDescriptor*>(pMessageDescriptor);
        pOverrideMessageDescriptor->message.shutterMessage.frameworkFrameNum = resultFrameNum;

        if (ChiMessageTypeShutter == pMessageDescriptor->messageType)
        {
            if (m_previewSessionId == pCbData->sessionId)
            {
                UINT32 frameNumIndex = (resultFrameNum % MaxOutstandingRequests);
                m_shutterTimestamp[frameNumIndex] = pMessageDescriptor->message.shutterMessage.timestamp;
                dropCb = FALSE;
            }
        }

        if (FALSE == dropCb)
        {
            m_pUsecase->ReturnFrameworkMessage((camera3_notify_msg_t*)pMessageDescriptor, m_pUsecase->GetCameraId());
        }
    }

}

CDKResult   FeatureHDR::GetRequestInfo(
    camera3_capture_request_t*  pRequest,
    FeatureRequestInfo*                pOutputRequests)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pOutputRequests)
    {
        CHX_LOG("pOutputRequests is NULL.");
        result = CDKResultEInvalidArg;
    }

    CHX_LOG("GetRequestInfo for AppFrameNum:%d", pRequest->frame_number);
    UINT32 frameIndex         = (pRequest->frame_number % MaxOutstandingRequests);

    for (UINT32 i = 0; i < pRequest->num_output_buffers; i++)
    {
        if (m_pSnapshotStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
        {
            CHX_LOG("FeatureHDR Request on Snapshot stream:%p", m_pSnapshotStream);

            m_isSnapshotFrame[frameIndex]         = TRUE;
            m_snapshotBufferNum[frameIndex]       = 1;
            m_snapshotReqIdToFrameNum[frameIndex] = pRequest->frame_number;
            m_maxSnapshotReqId++;

            ChxUtils::FreeMetaData(m_pSnapshotInputMeta[frameIndex]);
            m_pSnapshotInputMeta[frameIndex]    =
                ChxUtils::AllocateCopyMetaData(reinterpret_cast<const VOID*>(pRequest->settings));

            ChxUtils::Memcpy(&m_snapshotBuffers[frameIndex][0],
                             &pRequest->output_buffers[i],
                             sizeof(camera3_stream_buffer_t));

            m_snapshotAppFrameNum = pRequest->frame_number;

            m_numBayer2YuvFrames = 0;
        }
    }

    if (CDKResultSuccess == result)
    {
        result = GenerateAEBracketSettings(pRequest);
    }

    if (CDKResultSuccess == result)
    {
        ChxUtils::Memset(pOutputRequests, 0, sizeof(FeatureRequestInfo));

        for (UINT32 i = 0; i < m_numAeBracketFrames; i++)
        {
            ChxUtils::Memcpy(&pOutputRequests->request[i], pRequest, sizeof(camera3_capture_request));
            pOutputRequests->request[i].settings = m_pOverrideAppSetting[i];
        }
        pOutputRequests->numOfRequest = m_numAeBracketFrames;
        pOutputRequests->isReturnResult[m_firstNormalExpIdx] = TRUE;

        CHX_LOG("m_numAeBracketFrames:%d, isReturnResult[%d]:%d",
            m_numAeBracketFrames, m_firstNormalExpIdx, pOutputRequests->isReturnResult[m_firstNormalExpIdx]);
    }

    return result;
}

INT32 FeatureHDR::GetRequiredPipelines(
    AdvancedPipelineType * pPipelines,
    INT32 size)
{
    INT32 count = 0;
    const INT32 MaxPipelineCount = 8;

    if (NULL != pPipelines && size >= MaxPipelineCount)
    {
        INT32 index = 0;
        AdvancedPipelineType pipelineGroup[1];
        UINT                 cameraId[MaxPipelineCount];
        UINT                 wideCameraId = m_pUsecase->GetPhysicalCameraId(CameraRoleTypeWide);

        pPipelines[index]    = AdvancedPipelineType::InternalZSLYuv2JpegType;
        pipelineGroup[0]     = pPipelines[index];
        cameraId[index]      = wideCameraId;
        m_snapshotSessionId  = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
        index++;

        pPipelines[index]    = AdvancedPipelineType::Merge3YuvCustomTo1YuvType;
        pipelineGroup[0]     = pPipelines[index];
        cameraId[index]      = wideCameraId;
        m_mergeSessionId     = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
        index++;

        pPipelines[index]    = AdvancedPipelineType::ZSLSnapshotYUVType;
        pipelineGroup[0]     = pPipelines[index];
        cameraId[index]      = wideCameraId;
        m_bayer2YuvSessionId = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
        index++;

        if (TRUE == m_pUsecase->IsMultiCameraUsecase())
        {
            UINT teleCameraId   = m_pUsecase->GetPhysicalCameraId(CameraRoleTypeTele);

            pPipelines[index]       = AdvancedPipelineType::ZSLSnapshotYUVAuxType;
            pipelineGroup[0]        = pPipelines[index];
            cameraId[index]         = teleCameraId;
            m_bayer2YuvAuxSessionId = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
            index++;

            CHX_LOG("FeatureHDR add Bayer2Yuv pipeline for aux camera(%d), m_bayer2YuvAuxSessionId:%d",
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
            cameraId[index]      = wideCameraId;
            m_previewSessionId   = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
            index++;
        }
        else
        {
            m_previewSessionId = InvalidSessionId;
            CHX_LOG("Don't use Realtime pipeline in advance usecase for multicamera usecase");
        }

        m_pUsecase->SetPipelineCameraId(&pPipelines[0], &cameraId[0], index);

        CHX_LOG("m_previewSessionId:%d, m_bayer2YuvSessionId:%d, m_mergeSessionId:%d, m_snapshotSessionId:%d",
            m_previewSessionId,
            m_bayer2YuvSessionId,
            m_mergeSessionId,
            m_snapshotSessionId);

        count = index;
    }

    CHX_LOG("FeatureHDR::GetRequiredPipelines, required pipeline count:%d", count);
    return count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureHDR::SubmitRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureHDR::SubmitRequest(UINT32 sessionIdx, UINT frameNumber,
                                      TargetBuffer* pInputTargetBuffer,
                                      camera3_stream_buffer_t *pOutputBuffer,
                                      UINT32 inputPipelineReqId)
{
    const Session*          pSession       = m_pUsecase->GetSessionData(sessionIdx)->pSession;
    PipelineData*           pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(sessionIdx, 0));
    UINT                    requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);
    UINT32                  frameIndex     = (frameNumber % MaxOutstandingRequests);
    camera3_stream_buffer_t inputBuffer    = { 0 };
    CDKResult       result         = CDKResultSuccess;

    // Create copy so that future updates to input metadata don't free it
    camera_metadata_t*      pRequestMetadata    =
        reinterpret_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(m_pSnapshotInputMeta[frameIndex]));

    pPipelineData->seqIdToFrameNum[requestIdIndex] = frameNumber;

    camera_metadata_t* pRDIMeta = NULL;

    if ((TRUE == m_pUsecase->IsMultiCameraUsecase()) &&
        ((sessionIdx == m_bayer2YuvSessionId) || (sessionIdx == m_bayer2YuvAuxSessionId)))
    {
        // here to get RDI buffer from external Queue
        m_pUsecase->GetInputBufferFromRDIQueue(inputPipelineReqId, m_activePipelineID,
            &inputBuffer,
            &pRDIMeta);
        CHX_LOG("Get RDI Buffer from RDIQueue, reqId:%d, masterCameraId:%d, activePipelineID:%d, buffer handle:%p, meta:%p",
            inputPipelineReqId, m_masterCameraId, m_activePipelineID, inputBuffer.buffer, pRDIMeta);
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
        (sessionIdx == m_bayer2YuvSessionId || sessionIdx == m_bayer2YuvAuxSessionId))
    {
        request.pMetadata = pRDIMeta;
    }

    request.pPrivData                       = &m_privData[request.frameNumber % MaxOutstandingRequests];
    request.pPrivData->featureType          = FeatureType::HDR;
    request.pPrivData->bufferManagers[0]    = pInputTargetBuffer->pBufferManager;
    request.pPrivData->inputBuffers[0]      = inputBuffer.buffer;
    request.pPrivData->numInputBuffers      = 1;

    CHIPIPELINEREQUEST submitRequest    = { 0 };
    submitRequest.pSessionHandle        = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
    submitRequest.numRequests           = 1;
    submitRequest.pCaptureRequests      = &request;

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

    CHX_LOG_INFO("Sending HDR request sessionIdx: %d frameNumber:%d, request:%" PRIu64 " metadata:%p",
        sessionIdx, frameNumber, request.frameNumber, pRequestMetadata);

    m_pUsecase->SetRequestToFeatureMapping(sessionIdx, request.frameNumber, this);

    ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

    ChxUtils::FreeMetaData(pRequestMetadata);
    pRequestMetadata = NULL;
}

CDKResult FeatureHDR::GenerateAEBracketRequest(camera3_capture_request_t * pRequest)
{
    CDKResult result            = CDKResultSuccess;
    UINT8     sceneMode         = ANDROID_CONTROL_SCENE_MODE_HDR;
    INT32     expCompensation   = 0;
    UINT8     aeLock            = 1;
    UINT32    firstNormalExpIdx = 0;
    UINT32    outputCount       = 0;

    camera3_stream_buffer_t outputBuffers[2];

    // m_numAeBracketFrames = 1;

    CHX_LOG("E. num frames for ae bracket:%d", m_numAeBracketFrames);

    // find the first normal exposure index, will use it as framework request,
    // also return it's shutter/meta to framework
    for (UINT32 request = 0; request < m_numAeBracketFrames; request++)
    {
        if (m_expValues[request] == 0)
        {
            firstNormalExpIdx = request;
            break;
        }
    }

    PipelineData* pPipelineData = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_previewSessionId, 0));
    for (UINT32 request = 0; request < m_numAeBracketFrames; request++)
    {
        ChxUtils::Memset(outputBuffers, 0, sizeof(outputBuffers));
        outputCount = 0;

        UINT requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);

        pPipelineData->seqIdToFrameNum[requestIdIndex] = pRequest->frame_number;

        CHX_LOG("Realtime AppFrameNum to ReqId: %d <--> %d", pRequest->frame_number, pPipelineData->seqId);

        m_previewRawReqId = pPipelineData->seqId;

        outputBuffers[outputCount].buffer         = m_pRdiTargetBuffer->pBufferManager->GetImageBuffer()->GetBufferHandle();
        outputBuffers[outputCount].stream         = reinterpret_cast<camera3_stream_t*>(m_pRdiStream);
        outputBuffers[outputCount].acquire_fence  = -1;
        outputCount++;

        // also add preview for ev +0 capture, if it is present in the request
        if ((firstNormalExpIdx == request) && (pRequest->num_output_buffers > 1)) {
            for (UINT32 i = 0; i < pRequest->num_output_buffers; i++)
            {
                if (m_pPreviewStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
                {
                    // Capture preview stream
                    ChxUtils::Memcpy(&outputBuffers[outputCount],
                        &pRequest->output_buffers[i],
                        sizeof(camera3_stream_buffer_t));

                    outputCount++;
                    break;
                }
            }
        }

        /// update setting for ae bracket: ae_lock / ev +0 / hdr_scene_mode
        expCompensation = m_expValues[request];

        m_vendorTagOps.pSetMetaData(
            const_cast<camera_metadata_t*>(pRequest->settings),
            ANDROID_CONTROL_AE_EXPOSURE_COMPENSATION,
            &expCompensation,
            sizeof(expCompensation));

        //m_vendorTagOps.pSetMetaData(
        //    const_cast<camera_metadata_t*>(pRequest->settings),
        //    ANDROID_CONTROL_AE_LOCK,
        //    &aeLock,
        //    sizeof(aeLock));

        /// hardcode for ae bracket mode, should be set from app
        UINT32 tag = 0;
        CDKResult result = 0;
        result = m_vendorTagOps.pQueryVendorTagLocation(
            "org.codeaurora.qcamera3.ae_bracket",
            "mode",
            &tag);

        if (CDKResultSuccess == result) {
            UINT8 AEBracketMode = 1;
            m_vendorTagOps.pSetMetaData(
                const_cast<camera_metadata_t*>(pRequest->settings),
                tag,
                &AEBracketMode,
                sizeof(AEBracketMode));
        }

        CHX_LOG("%s, AE Bracketing request [%d/%d], realtimeReqId:%d -> appFrameNum:%d, expCompensation:%d, output buffers:%d",
            __func__,
            request,
            m_numAeBracketFrames,
            m_previewRawReqId,
            pRequest->frame_number,
            expCompensation,
            outputCount);

        const Session* pSession = m_pUsecase->GetSessionData(m_previewSessionId)->pSession;

        CHICAPTUREREQUEST realTimeRequest       = { 0 };
        realTimeRequest.frameNumber             = pPipelineData->seqId++;
        realTimeRequest.hPipelineHandle         = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
        realTimeRequest.numOutputs              = outputCount;
        realTimeRequest.pOutputBuffers          = reinterpret_cast<CHISTREAMBUFFER*>(outputBuffers);
        realTimeRequest.pMetadata               = pRequest->settings;
        realTimeRequest.pPrivData               = &m_privData[realTimeRequest.frameNumber % MaxOutstandingRequests];
        realTimeRequest.pPrivData->featureType  = FeatureType::HDR;

        CHIPIPELINEREQUEST submitRequest;
        submitRequest.pSessionHandle    = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
        submitRequest.numRequests       = 1;
        submitRequest.pCaptureRequests  = &realTimeRequest;

        m_pUsecase->SetRequestToFeatureMapping(m_previewSessionId, realTimeRequest.frameNumber, this);

        result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);
    }

    CHX_LOG("%s, X. result:%d", __func__, result);
    return result;

}

CDKResult FeatureHDR::GenerateAEBracketSettings(camera3_capture_request_t * pRequest)
{
    CDKResult result            = CDKResultSuccess;
    UINT8     sceneMode         = ANDROID_CONTROL_SCENE_MODE_HDR;
    INT32     expCompensation   = 0;
    UINT8     aeLock            = 1;
    UINT32    firstNormalExpIdx = 0;
    UINT32    outputCount       = 0;

    camera3_stream_buffer_t outputBuffers[2];

    // m_numAeBracketFrames = 1;

    CHX_LOG("GenerateAEBracketSettings E. num frames for ae bracket:%d", m_numAeBracketFrames);

    // find the first normal exposure index, will use it as framework request,
    // also return it's shutter/meta to framework
    for (UINT32 request = 0; request < m_numAeBracketFrames; request++)
    {
        if (m_expValues[request] == 0)
        {
            m_firstNormalExpIdx = request;
            CHX_LOG("m_firstNormalExpIdx: %d", m_firstNormalExpIdx);
            break;
        }
    }

    for (UINT32 request = 0; request < m_numAeBracketFrames; request++)
    {
        if (NULL != m_pOverrideAppSetting[request])
        {
            ChxUtils::FreeMetaData(m_pOverrideAppSetting[request]);
            m_pOverrideAppSetting[request] = NULL;
        }

        m_pOverrideAppSetting[request] =
            allocate_copy_camera_metadata_checked(pRequest->settings, get_camera_metadata_size(pRequest->settings));

        /// update setting for ae bracket: ae_lock / ev +0 / hdr_scene_mode
        expCompensation = m_expValues[request];

        m_vendorTagOps.pSetMetaData(
            const_cast<camera_metadata_t*>(m_pOverrideAppSetting[request]),
            ANDROID_CONTROL_AE_EXPOSURE_COMPENSATION,
            &expCompensation,
            sizeof(expCompensation));

        //m_vendorTagOps.pSetMetaData(
        //    const_cast<camera_metadata_t*>(m_pOverrideAppSetting[request]),
        //    ANDROID_CONTROL_AE_LOCK,
        //    &aeLock,
        //    sizeof(aeLock));

        /// hardcode for ae bracket mode, should be set from app
        UINT32 tag = 0;
        CDKResult result = 0;
        result = m_vendorTagOps.pQueryVendorTagLocation(
            "org.codeaurora.qcamera3.ae_bracket",
            "mode",
            &tag);

        if (CDKResultSuccess == result) {
            UINT8 AEBracketMode = 1;
            m_vendorTagOps.pSetMetaData(
                const_cast<camera_metadata_t*>(m_pOverrideAppSetting[request]),
                tag,
                &AEBracketMode,
                sizeof(AEBracketMode));
        }

        CHX_LOG("%s, AE Bracketing request [%d/%d], appFrameNum:%d, expCompensation:%d",
            __func__,
            request,
            m_numAeBracketFrames,
            pRequest->frame_number,
            expCompensation);
    }

    CHX_LOG("%s, X. result:%d", __func__, result);
    return result;
}
