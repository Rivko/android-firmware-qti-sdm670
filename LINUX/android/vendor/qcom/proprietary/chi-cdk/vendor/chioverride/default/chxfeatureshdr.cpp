////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxFeatureSHDR.cpp
/// @brief CHX HDR feature class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxadvancedcamerausecase.h"
#include "chxincs.h"
#include "chxfeatureshdr.h"
#include "chxusecase.h"

#define SHDR_MAX_INTERNAL_STREAM 4
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureSHDR::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FeatureSHDR* FeatureSHDR::Create(
    AdvancedCameraUsecase* pUsecase)
{
    FeatureSHDR* pFeature = CHX_NEW FeatureSHDR;

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
/// FeatureSHDR::Pause
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureSHDR::Pause(
    BOOL isForced)
{
    CHX_LOG("Feature Pause isForced =%d", isForced);
    //Implement if required
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureSHDR::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureSHDR::Flush(
    BOOL isInProgress)
{
    CHX_LOG("Feature Flush isInProgress =%d", isInProgress);
    //Implement if required
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureSHDR::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureSHDR::Initialize(
    AdvancedCameraUsecase* pUsecase)
{
    m_pUsecase          = pUsecase;

    m_previewRawReqId       = 0;
    m_lastShutterFrameNum   = 0;
    m_isSnapshotRequested   = FALSE;
    m_pSnapshotStream       = NULL;
    m_pYUVStream            = NULL;
    m_pRealtimeYUVStream    = NULL;
    m_pVideoStream          = NULL;
    m_pPreviewStream        = NULL;
    m_InternalStreamConfig  = NULL;

    m_previewPipelineIndex  = InvalidPipelineIndex;
    m_snapshotPipelineIndex = InvalidPipelineIndex;

    m_bayer2YuvSessionId    = InvalidSessionId;
    m_previewSessionId      = InvalidSessionId;
    m_snapshotSessionId     = InvalidSessionId;

    CHX_LOG("FeatureSHDR m_pRdiStream: %p, m_pBayer2YuvStream: %p",
            m_pRdiStream, m_pBayer2YuvStream);

    ExtensionModule::GetInstance()->GetVendorTagOps(&m_vendorTagOps);
    CHX_LOG("pGetMetaData:%p, pSetMetaData:%p", m_vendorTagOps.pGetMetaData, m_vendorTagOps.pSetMetaData);

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureSHDR::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureSHDR::Destroy(BOOL isForced)
{
    CHX_LOG("FeatureSHDR::Destroy(), isForced %d E.", isForced);

    /// Freeing internally created streams
    for (UINT stream = 0; stream < m_InternalStreamConfig->num_streams; stream++)
    {
        ChxUtils::Free(m_InternalStreamConfig->streams[stream]);
    }
    ChxUtils::Free(m_InternalStreamConfig);
    m_InternalStreamConfig = NULL;

    /// only reset shared stream to NULL
    /// no need to free, as advance will handle it.
    m_pPreviewStream   = NULL;
    m_pRdiStream       = NULL;
    m_pYUVStream       = NULL;
    m_pRealtimeYUVStream = NULL;

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
    for (UINT32 i = 0; i < MaxOutstandingRequests; i++)
    {
        if (NULL != m_pPreviewAppMeta[i])
        {
            ChxUtils::FreeMetaData(m_pPreviewAppMeta[i]);
            m_pPreviewAppMeta[i] = NULL;
        }
    }

    if (NULL != m_pSHDRYUVOutputBufferManager)
    {
        m_pSHDRYUVOutputBufferManager->Destroy();
        m_pSHDRYUVOutputBufferManager = NULL;
    }

    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureSHDR::OverrideUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiUsecase* FeatureSHDR::OverrideUsecase(
    LogicalCameraInfo*              pCameraInfo,
    camera3_stream_configuration_t* pStreamConfig)
{
    (VOID)pCameraInfo;
    (VOID)pStreamConfig;
    CHX_LOG("OverrideUsecase for SHDR");

    m_pChiUsecase = m_pUsecase->GetChiUseCase();
    StreamMap(pStreamConfig);
    CHX_LOG("Video HDR : %d", ExtensionModule::GetInstance()->GetVideoHDRModeType());
    if ((ExtensionModule::GetInstance()->GetVideoHDRModeType() == InSensorVideoMode2YUVFrameSHDR)
        || (ExtensionModule::GetInstance()->GetVideoHDRModeType() == InSensorVideoMode3YUVFrameSHDR)
        || (ExtensionModule::GetInstance()->GetVideoHDRModeType() == InSensorVideoMode3YUVFrameSHDRVCMode))
    {
        OverrideUsecaseYUVSHDR(pCameraInfo, pStreamConfig, ExtensionModule::GetInstance()->GetVideoHDRModeType());
    }
    else
    {
        /// @todo - TargetName to ease the matching of streamId vs Target per pipeline. Need to map the rest of internal streams
        for (UINT target = 0; target < m_pChiUsecase->numTargets; target++)
        {
            if ((ChiFormatRawMIPI == m_pChiUsecase->ppChiTargets[target]->pBufferFormats[0]) &&
                (m_rdiStreamIndex == 0))
            {
                m_rdiStreamIndex   = target;
                m_pRdiTargetBuffer = m_pUsecase->GetTargetBuffer(m_rdiStreamIndex);
            }
            // ToDo: Use Bayer2YUV pipeline for SHDR
            /* else if ((1 == m_pChiUsecase->ppChiTargets[target]->numFormats) &&
                     (ChiFormatYUV420NV12 == m_pChiUsecase->ppChiTargets[target]->pBufferFormats[0]) &&
                     (m_Bayer2YuvStreamIndex == 0))
            {
                /// @todo Need to map Target vs streamId for pipeline
                m_Bayer2YuvStreamIndex   = target;
                m_pBayer2YuvTargetBuffer = m_pUsecase->GetTargetBuffer(m_Bayer2YuvStreamIndex);
            }*/
        }

        m_previewPipelineIndex  = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::SHDRPreviewRawYUVType);
        m_snapshotPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::SHDRSnapShotType);
        CHX_LOG("m_previewPipelineIndex:%d and m_snapshotPipelineIndex:%d", m_previewPipelineIndex, m_snapshotPipelineIndex);
        ChiPipelineTargetCreateDescriptor* pRealtimeDesc   = &m_pChiUsecase->pPipelineTargetCreateDesc[m_previewPipelineIndex];
        pRealtimeDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pPreviewStream;
        pRealtimeDesc->sinkTarget.pTargetPortDesc[1].pTarget->pChiStream   = m_pVideoStream;
        pRealtimeDesc->sinkTarget.pTargetPortDesc[2].pTarget->pChiStream   = m_pYUVStream;
        pRealtimeDesc->sinkTarget.pTargetPortDesc[3].pTarget->pChiStream   = m_pRealtimeYUVStream;

        if (m_snapshotPipelineIndex != InvalidPipelineIndex)
        {
            ChiPipelineTargetCreateDescriptor* pSnapShotDesc                   = &m_pChiUsecase->pPipelineTargetCreateDesc[m_snapshotPipelineIndex];
            pSnapShotDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pYUVStream;
            pSnapShotDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pSnapshotStream;
        }
        CHX_LOG("m_rdiStreamIndex:%d, m_Bayer2YuvStreamIndex:%d",        m_rdiStreamIndex, m_Bayer2YuvStreamIndex);
        CHX_LOG("FeatureSHDR RdiStream: %p, PreviewStream: %p(%dx%d) snapshotstream %p(%dx%d): VideoStream %p(%dx%d) : YUVStream %p(%dx%d) : m_pRealtimeYUVStream %p(%dx%d):  ",
            m_pRdiStream, m_pPreviewStream, m_pPreviewStream->width, m_pPreviewStream->height,
            m_pSnapshotStream, m_pSnapshotStream->width, m_pSnapshotStream->height,
            m_pVideoStream, m_pVideoStream->width, m_pVideoStream->height,
            m_pYUVStream, m_pYUVStream->width, m_pYUVStream->height,
            m_pRealtimeYUVStream, m_pRealtimeYUVStream->width, m_pRealtimeYUVStream->height);

        CHX_LOG("Description %s and %s and %s and %s", pRealtimeDesc->sinkTarget.pTargetPortDesc[0].pTargetName,
                        pRealtimeDesc->sinkTarget.pTargetPortDesc[1].pTargetName,
                        pRealtimeDesc->sinkTarget.pTargetPortDesc[2].pTargetName,
                        pRealtimeDesc->sinkTarget.pTargetPortDesc[3].pTargetName);

    }
    return m_pChiUsecase;
}

VOID FeatureSHDR::OverrideUsecaseYUVSHDR(
    LogicalCameraInfo*              pCameraInfo,
    camera3_stream_configuration_t* pStreamConfig,
    UINT shdr_type)
{
    (VOID)pCameraInfo;
    (VOID)pStreamConfig;
    if ( shdr_type == InSensorVideoMode2YUVFrameSHDR)
    {
        m_previewPipelineIndex  = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::SHDRPreviewYUVType);
        m_snapshotPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::SHDRSnapShotType);
    }
    else if (shdr_type == InSensorVideoMode3YUVFrameSHDR)
    {
        m_previewPipelineIndex  = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::SHDRPreview3FrameYUVType);
        m_snapshotPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::SHDRSnapShotType);
    }
    else if (shdr_type == InSensorVideoMode3YUVFrameSHDRVCMode)
    {
        m_previewPipelineIndex  = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::SHDRPreview3FrameYUVTypeWithHWStats);
        m_snapshotPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::SHDRSnapShotType);
    }

    CHX_LOG("m_previewPipelineIndex:%d and m_snapshotPipelineIndex:%d", m_previewPipelineIndex, m_snapshotPipelineIndex);
    if (m_previewPipelineIndex != InvalidPipelineIndex)
    {
        ChiPipelineTargetCreateDescriptor* pRealtimeDesc   = &m_pChiUsecase->pPipelineTargetCreateDesc[m_previewPipelineIndex];
        ChiStream*          pChiStream;
        for ( UINT i =0 ; i < pRealtimeDesc->sinkTarget.numTargets; i++)
        {
            switch(i)
            {
                case 0:
                    pChiStream = m_pPreviewStream;
                    break;
                case 1:
                    pChiStream = m_pVideoStream;
                    break;
                case 2:
                    pChiStream = m_pYUVStream;
                    break;
                case 3:
                    pChiStream = m_pRealtimeYUVStream;
                    break;

                default :
                    CHX_LOG("unsupported target at descriptor index = %d", i);
                    pChiStream = NULL;
            }
            pRealtimeDesc->sinkTarget.pTargetPortDesc[i].pTarget->pChiStream   = pChiStream;
        }
    }

    if (m_snapshotPipelineIndex != InvalidPipelineIndex)
    {
        ChiPipelineTargetCreateDescriptor* pSnapShotDesc                   = &m_pChiUsecase->pPipelineTargetCreateDesc[m_snapshotPipelineIndex];
        pSnapShotDesc->sourceTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pYUVStream;
        pSnapShotDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream   = m_pSnapshotStream;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureSHDR::PipelineCreated
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureSHDR::PipelineCreated(
    UINT32 sessionId,    ///< Id of session created
    UINT32 pipelineId)   ///< Pipeline of session created
{
    ChiSensorModeInfo*              pSensorInfo     = NULL;
    const ChiPipelineInputOptions*  pInputOptions   = NULL;

    /// @todo need to find the mapping between session's pipelineId vs XMLs pipelineId
    pSensorInfo     = m_pUsecase->GetSessionData(sessionId)->pipelines[0].pPipeline->GetSensorModeInfo();
    pInputOptions   = m_pUsecase->GetSessionData(sessionId)->pipelines[0].pPipeline->GetInputOptions();

    CHX_LOG("FeatureSHDR sessionId:%d, pipelineId: %d, Sensor widthxheight: %d x %d, InputOptions widthxheight: %d x %d",
                  sessionId,
                  pipelineId,
                  pSensorInfo->frameDimension.width, pSensorInfo->frameDimension.height,
                  pInputOptions->bufferOptions.optimalDimension.width,
                  pInputOptions->bufferOptions.optimalDimension.height);

    /// @todo - Need to find means of associating grallocUsage
    switch (m_pUsecase->GetAdvancedPipelineTypeByPipelineId(pipelineId))
    {
        case AdvancedPipelineType::SHDRPreviewRawYUVType:// check if configrdi is called or not required or not
        case AdvancedPipelineType::SHDRPreviewYUVType:
        case AdvancedPipelineType::SHDRPreview3FrameYUVType:
        case AdvancedPipelineType::SHDRPreview3FrameYUVTypeWithHWStats:
        {
            m_pUsecase->ConfigRdiStream(pSensorInfo);
            if (TRUE == m_isSnapshotRequested)
            {
                CHIBufferManagerCreateData createData   = { 0 };
                createData.width                        = m_pYUVStream->width;
                createData.height                       = m_pYUVStream->height;
                createData.format                       = ChiStreamFormatYCbCr420_888;
                createData.producerFlags                = GRALLOC1_PRODUCER_USAGE_CAMERA;
                createData.consumerFlags                = GRALLOC1_CONSUMER_USAGE_CAMERA | GRALLOC1_CONSUMER_USAGE_CPU_READ;
                createData.maxBufferCount               = MinOutputBuffers;
                createData.immediateBufferCount         = 1;
                m_pSHDRYUVOutputBufferManager           = CHIBufferManager::Create("SHDRYUVBufferManager", &createData);
                if (NULL == m_pSHDRYUVOutputBufferManager)
                {
                    CHX_LOG_ERROR("m_pSHDRYUVOutputBufferManager is NULL");
                }
                else
                {
                    m_YUVTargetBuffers.pBufferManager = m_pSHDRYUVOutputBufferManager;
                    m_YUVTargetBuffers.pMutex         = Mutex::Create();
                    for (UINT queueIndex = 0; queueIndex < BufferQueueDepth; queueIndex++)
                    {
                        m_YUVTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer =
                            static_cast<CHISTREAMBUFFER*>(CHX_CALLOC(sizeof(CHISTREAMBUFFER)));

                        if (NULL != m_YUVTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer)
                        {
                            m_YUVTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer->acquireFence = -1;
                            m_YUVTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer->phBuffer     = NULL;
                            m_YUVTargetBuffers.bufferQueue[queueIndex].pRdiOutputBuffer->pStream      = m_pYUVStream;
                        }
                    }
                    m_pYUVTargetBuffer = &m_YUVTargetBuffers;
                }
            }
            break;
        }
        default:
        {
            CHX_LOG("FeatureSHDR pipeline not used in HDR.");
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureSHDR::ExecuteProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult  FeatureSHDR::ExecuteProcessRequest(
    camera3_capture_request_t* pRequest)              ///< Request parameters

{
    CDKResult result              = CDKResultSuccess;
    UINT32    frameIndex          = (pRequest->frame_number % MaxOutstandingRequests);
    UINT32    numBuffers          = 0;
    UINT      snapshotReqIdIndex  = (m_maxSnapshotReqId % MaxOutstandingRequests);
    UINT      snapshotCount       = 0;
    UINT32    outputCount         = 0;
    m_isSnapshotFrame[frameIndex] = FALSE;


    CHX_LOG("SHDR capture request for frame %d with %d output buffers", pRequest->frame_number, pRequest->num_output_buffers);

    m_shutterTimestamp[frameIndex]    = 0;
    for (UINT32 i = 0; i < pRequest->num_output_buffers; i++)
    {
        if (m_pSnapshotStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
        {
            // SnapShot Meta
            CHX_LOG("Snapshot Frame %d", pRequest->frame_number);
            ChxUtils::Memcpy(&m_snapshotBuffers[frameIndex][snapshotCount],
                         &pRequest->output_buffers[i],
                         sizeof(camera3_stream_buffer_t));
             m_isSnapshotFrame[frameIndex]                 = TRUE;
             ChxUtils::FreeMetaData(m_pSnapshotInputMeta[frameIndex]);
             m_pSnapshotInputMeta[frameIndex]    =  // free metadata
                 ChxUtils::AllocateCopyMetaData(reinterpret_cast<const VOID*>(pRequest->settings));

        }
    }
    result = GenerateRDIRequest(pRequest);
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureSHDR::GenerateRDIRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureSHDR::GenerateRDIRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult          result         = CDKResultSuccess;
    CHICAPTUREREQUEST  chiRequest     = { 0 };
    PipelineData*      pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_previewSessionId, 0));
    UINT               requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);
    UINT32             outputCount        = 0;
    camera3_stream_buffer_t outputBuffers[4];
    ChxUtils::Memset(outputBuffers, 0, sizeof(outputBuffers));
    for (UINT32 i = 0; i < pRequest->num_output_buffers; i++)
    {
        if (m_pPreviewStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
        {
            // Capture preview stream
            ChxUtils::Memcpy(&outputBuffers[outputCount],
                    &pRequest->output_buffers[i],
                    sizeof(camera3_stream_buffer_t));
            outputCount++;
        }
        if (m_pYUVStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
        {
            // Capture preview stream
            ChxUtils::Memcpy(&outputBuffers[outputCount],
                    &pRequest->output_buffers[i],
                    sizeof(camera3_stream_buffer_t));
            outputCount++;
        }
        if (m_pRealtimeYUVStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
        {
            // Capture preview stream
            ChxUtils::Memcpy(&outputBuffers[outputCount],
                    &pRequest->output_buffers[i],
                    sizeof(camera3_stream_buffer_t));
            outputCount++;
        }
        if (m_pVideoStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
        {
            // Capture preview stream
            ChxUtils::Memcpy(&outputBuffers[outputCount],
                    &pRequest->output_buffers[i],
                    sizeof(camera3_stream_buffer_t));
            outputCount++;
        }
    }
    if (TRUE == m_isSnapshotFrame[requestIdIndex])
    {
        UINT32 queueIndex = (requestIdIndex % BufferQueueDepth);
        m_pYUVTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer->phBuffer =
            m_pYUVTargetBuffer->pBufferManager->GetImageBuffer()->GetBufferHandle();
        ChxUtils::Memcpy(&outputBuffers[outputCount],
             m_pYUVTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer,
             sizeof(camera3_stream_buffer_t));
        outputBuffers[outputCount].acquire_fence = -1;
        CHX_LOG("SHDR Submitting Request for snapshot");
        // outputBuffers[outputCount].stream        = // preview stream
        outputCount++;
    }

    pPipelineData->seqIdToFrameNum[requestIdIndex] = pRequest->frame_number;
    // CHX_LOG("seqId :%d and frame_num :%d", pPipelineData->seqId, pRequest->frame_number);
    CHX_LOG("SHDR Submitting Request %d and %d and outputCount:%d",
        pPipelineData->seqId, pRequest->frame_number, outputCount);

    const Session* pSession = m_pUsecase->GetSessionData(m_previewSessionId)->pSession;

    chiRequest.frameNumber             = pPipelineData->seqId++;
    chiRequest.hPipelineHandle         = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
    chiRequest.numOutputs              = outputCount;
    chiRequest.pOutputBuffers          = reinterpret_cast<CHISTREAMBUFFER*>(outputBuffers);
    chiRequest.pMetadata               = pRequest->settings;
    chiRequest.pPrivData               = &m_privData[pRequest->frame_number % MaxOutstandingRequests];
    chiRequest.pPrivData->featureType  = FeatureType::HDR;  // change this



    UINT32 sensorModeIndex = pSession->GetSensorModeInfo(0)->modeIndex;
    ChxUtils::FillTuningModeData(const_cast<VOID*>(chiRequest.pMetadata),
                                 pRequest,
                                 sensorModeIndex,
                                 &m_effectModeValue,
                                 &m_sceneModeValue,
                                 &m_tuningFeature1Value,
                                 &m_tuningFeature2Value);

    CHIPIPELINEREQUEST submitRequest = { 0 };
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
    m_pUsecase->SetRequestToFeatureMapping(m_previewSessionId, chiRequest.frameNumber, this);

    submitRequest.pSessionHandle   = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
    submitRequest.numRequests      = 1;
    submitRequest.pCaptureRequests = &chiRequest;

    result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureSHDR::ProcessResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureSHDR::ProcessResult(
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

    CHX_LOG("FeatureSHDR ProcessResult resultFrameNum: %d, sessionId: %d, pResultMetadata: %p, numOutputBuffers: %d",
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
        // IsSnapshot: then only store meta
        if ((m_previewSessionId == pCbData->sessionId) && (TRUE == m_isSnapshotFrame[resultFrameIndex]))
        {
            // UINT32 zslQueueIndex = (resultFrameNum % BufferQueueDepth);
            UINT32 zslQueueIndex = (realtimeReqId % BufferQueueDepth);

            // Create copy because this pointer could be freed by app as soon as result returned
            ChxUtils::FreeMetaData(m_pYUVTargetBuffer->bufferQueue[zslQueueIndex].pMetadata);
            /// @todo This currently copies the debugData pointer, to memory owned by the first pipeline. Should copy
            m_pYUVTargetBuffer->bufferQueue[zslQueueIndex].pMetadata =
                static_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(pMetadata));


            CHX_LOG("zslQueueIndex:%d, meta:%p", zslQueueIndex, m_pYUVTargetBuffer->bufferQueue[zslQueueIndex].pMetadata);

            m_pYUVTargetBuffer->bufferQueue[zslQueueIndex].frameNumber = resultFrameNum; // pResult->frameworkFrameNum;
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
            // UINT32 queueIndex = (resultFrameNum  % BufferQueueDepth);

            // Use RequestId for RDI buffer queue
            UINT32 queueIndex = (realtimeReqId % BufferQueueDepth);

            CHX_LOG("ZSL queue index:%d, frame num:%d, is snapshot:%d",
                queueIndex, resultFrameNum, m_isSnapshotFrame[resultFrameIndex]);


        }
        else if ((m_pYUVStream == pResult->pOutputBuffers[j].pStream) && (TRUE == m_isSnapshotFrame[pResult->frameworkFrameNum % MaxOutstandingRequests]))
        {
            UINT32 requestIdIndex  = (pResult->frameworkFrameNum % MaxOutstandingRequests);
            if (TRUE == m_isSnapshotFrame[requestIdIndex]) // remove
            {
                CHX_LOG("Got YUV buffer for JPEG :%d", pResult->frameworkFrameNum);
                    UINT32 queueIndex = (resultFrameNum % BufferQueueDepth);
                    ChxUtils::Memcpy(m_pYUVTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer,
                             &pResult->pOutputBuffers[j],
                             sizeof(CHISTREAMBUFFER));
                    m_pYUVTargetBuffer->pBufferManager->ReleaseReference(
                        reinterpret_cast<buffer_handle_t*>(pResult->pOutputBuffers[j].phBuffer));
                    m_pYUVTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer->acquireFence = -1;
                    SubmitRequest(m_snapshotSessionId, resultFrameNum, m_pYUVTargetBuffer,
                                                  &m_snapshotBuffers[resultFrameNum % MaxOutstandingRequests][0], 0 /* resultFrameNum */);
             }
        }
        // Otherwise queue a buffer as part of the normal result
        else
        {
            UINT32 queueIndex = (resultFrameNum % MaxOutstandingRequests);
            CHX_LOG("Input Buffers are :%d", pResult->pPrivData->numInputBuffers);
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

            if (m_pSnapshotStream == pResult->pOutputBuffers[j].pStream)
            {
                CHX_LOG("FeatureSHDR Received Final Snapshot(JPEG) stream, reqId:%d, frameNum:%d", offlineReqId, resultFrameNum);
                camera3_stream_buffer_t* pResultBuffer =
                    const_cast<camera3_stream_buffer_t*>(&pUsecaseResult->output_buffers[pUsecaseResult->num_output_buffers++]);
                ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[j], sizeof(camera3_stream_buffer_t));

                isAppResultsAvailable = TRUE;
            }
            if ((m_pPreviewStream == pResult->pOutputBuffers[j].pStream) ||
                    (m_pYUVStream == pResult->pOutputBuffers[j].pStream) ||
                    (m_pRealtimeYUVStream == pResult->pOutputBuffers[j].pStream) ||
                    (m_pVideoStream == pResult->pOutputBuffers[j].pStream))
            {
                CHX_LOG("Sending App Buffer, reqId:%d, frameNum:%d", offlineReqId, resultFrameNum);

                camera3_stream_buffer_t* pResultBuffer =
                    const_cast<camera3_stream_buffer_t*>(&pUsecaseResult->output_buffers[pUsecaseResult->num_output_buffers++]);
                ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[j], sizeof(camera3_stream_buffer_t));
                isAppResultsAvailable = TRUE;
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
/// FeatureSHDR::ProcessMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureSHDR::ProcessMessage(
    const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{

    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        CHX_LOG("FeatureSHDR Ignore shutter message for dual camera!");

        return;
    }

    if (ChiMessageTypeSof == pMessageDescriptor->messageType)
    {
        // SOF notifications are not sent to the HAL3 application
        CHX_LOG("FeatureSHDR Chi Notify SOF frameNum %u framework frameNum %u, timestamp %" PRIu64,
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

        CHX_LOG("FeatureSHDR Shutter Notify. ReqId:%d <-> AppFrameNum:%d, timestamp:%" PRIu64,
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

CDKResult   FeatureSHDR::GetRequestInfo(
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
    return result;
}

INT32 FeatureSHDR::GetRequiredPipelines(
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
        if (ExtensionModule::GetInstance()->GetVideoHDRModeType() == InSensorVideoMode2YUVFrameSHDR)
        {
            pPipelines[index]    = AdvancedPipelineType::SHDRPreviewYUVType;
        }
        else if (ExtensionModule::GetInstance()->GetVideoHDRModeType() == InSensorVideoMode3YUVFrameSHDR)
        {
            pPipelines[index]    = AdvancedPipelineType::SHDRPreview3FrameYUVType;
        }
        else if (ExtensionModule::GetInstance()->GetVideoHDRModeType() == InSensorVideoMode3YUVFrameSHDRVCMode)
        {
            pPipelines[index]    = AdvancedPipelineType::SHDRPreview3FrameYUVTypeWithHWStats;
        }
        else
        {
            pPipelines[index]    = AdvancedPipelineType::SHDRPreviewRawYUVType;
        }
        pipelineGroup[0]     = pPipelines[index];
        cameraId[index]      = wideCameraId;
        m_previewSessionId   = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
        index++;
        if ((ExtensionModule::GetInstance()->GetVideoHDRModeType() == InSensorVideoMode2YUVFrameSHDR) ||
                (ExtensionModule::GetInstance()->GetVideoHDRModeType() == InSensorVideoMode2RAWFrameSHDR) ||
                (ExtensionModule::GetInstance()->GetVideoHDRModeType() == InSensorVideoMode3YUVFrameSHDR) ||
                (ExtensionModule::GetInstance()->GetVideoHDRModeType() == InSensorVideoMode3YUVFrameSHDRVCMode))
        {
            // later add snapshot support for YUV SHDR
            pPipelines[index]    = AdvancedPipelineType::SHDRSnapShotType;
            pipelineGroup[0]     = pPipelines[index];
            cameraId[index]      = wideCameraId;
            m_snapshotSessionId  = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], 1);
            index++;
        }

        m_pUsecase->SetPipelineCameraId(&pPipelines[0], &cameraId[0], index);

        CHX_LOG("m_previewSessionId:%d, m_bayer2YuvSessionId:%d, m_mergeSessionId:%d, m_snapshotSessionId:%d",
            m_previewSessionId,
            m_bayer2YuvSessionId,
            m_mergeSessionId,
            m_snapshotSessionId);

        count = index;
    }

    CHX_LOG("FeatureSHDR::GetRequiredPipelines, required pipeline count:%d", count);
    return count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureSHDR::SubmitRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureSHDR::SubmitRequest(UINT32 sessionIdx, UINT frameNumber,
                                      TargetBuffer* pInputTargetBuffer,
                                      camera3_stream_buffer_t *pOutputBuffer,
                                      UINT32 inputPipelineReqId)
{
    const Session*          pSession       = m_pUsecase->GetSessionData(sessionIdx)->pSession;
    PipelineData*           pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(sessionIdx, 0));
    UINT                    requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);
    UINT32                  frameIndex     = (frameNumber % MaxOutstandingRequests);
    UINT32                  bufferIndex    = (frameNumber % BufferQueueDepth);
    camera3_stream_buffer_t inputBuffer    = { 0 };
    CDKResult       result         = CDKResultSuccess;

    // Create copy so that future updates to input metadata don't free it
    camera_metadata_t*      pRequestMetadata    =
        reinterpret_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(m_pSnapshotInputMeta[frameIndex]));

    pPipelineData->seqIdToFrameNum[requestIdIndex] = frameNumber;

    camera_metadata_t* pRDIMeta = NULL;

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
    if (pRequestMetadata == NULL)
    {
        pRequestMetadata = 
            reinterpret_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(pInputTargetBuffer->bufferQueue[bufferIndex].pMetadata));
    }

    CHICAPTUREREQUEST request = { 0 };
    request.frameNumber       = pPipelineData->seqId++;
    request.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
    request.numInputs         = 1;
    request.numOutputs        = 1;
    request.pInputBuffers     = reinterpret_cast<CHISTREAMBUFFER*>(&inputBuffer);
    request.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(pOutputBuffer);
    request.pMetadata         = pRequestMetadata;


    request.pPrivData              = &m_privData[request.frameNumber % MaxOutstandingRequests];
    request.pPrivData->featureType = FeatureType::HDR;
    request.pPrivData->bufferManagers[0]    = pInputTargetBuffer->pBufferManager;
    request.pPrivData->inputBuffers[0]      = inputBuffer.buffer;
    request.pPrivData->numInputBuffers      = 1;


    CHX_LOG_INFO("Sending SHDR request sessionIdx: %d frameNumber:%d, request:%" PRIu64 " metadata:%p and bufferstream:%p",
        sessionIdx, frameNumber, request.frameNumber, request.pMetadata, request.pOutputBuffers->pStream);
    INT64 sensorTimestamp0 = 0;
    UINT64 sensorTimestamp1 = 0;
    camera_metadata_entry_t entry2 = { 0 };
    if (0 == find_camera_metadata_entry(pRequestMetadata, ANDROID_SENSOR_TIMESTAMP, &entry2))
    {
        sensorTimestamp1 = entry2.data.i64[0];
        CHX_LOG_INFO("ANDROID_SENSOR_TIMESTAMP Sending SHDR request sessionIdx: %d frameNumber:%d, request:%" PRIu64 " metadata:%p and bufferstream:%p and time :%" PRIu64,
              sessionIdx, frameNumber, request.frameNumber, request.pMetadata, request.pOutputBuffers->pStream, sensorTimestamp1);

    }
    camera_metadata_entry_t entry3 = { 0 };
        if (0 == find_camera_metadata_entry(pRequestMetadata, ANDROID_SENSOR_EXPOSURE_TIME, &entry3))
    {
        sensorTimestamp0 = entry3.data.i64[0];
        CHX_LOG_INFO("ANDROID_SENSOR_EXPOSURE_TIME Sending SHDR request sessionIdx: %d frameNumber:%d, request:%" PRIu64 " metadata:%p and bufferstream:%p and time :%" PRId64,
              sessionIdx, frameNumber, request.frameNumber, request.pMetadata, request.pOutputBuffers->pStream, sensorTimestamp0);

    }


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

    CHX_LOG_INFO("Sending SHDR request sessionIdx: %d frameNumber:%d, request:%" PRIu64 " metadata:%p",
        sessionIdx, frameNumber, request.frameNumber, pRequestMetadata);

    m_pUsecase->SetRequestToFeatureMapping(sessionIdx, request.frameNumber, this);

    ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

    ChxUtils::FreeMetaData(pRequestMetadata);
    pRequestMetadata = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureSHDR::StreamMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureSHDR::StreamMap(
    camera3_stream_configuration_t* pStreamConfig)
{
    m_InternalStreamConfig = reinterpret_cast<camera3_stream_configuration_t *>
        (ChxUtils::Calloc(sizeof(camera3_stream_configuration_t)));
    sortFWKStream(pStreamConfig);

    if (m_InternalStreamConfig != NULL)
    {
        m_InternalStreamConfig->streams = static_cast<camera3_stream_t **>
            (ChxUtils::Calloc(sizeof(camera3_stream_t *) * SHDR_MAX_INTERNAL_STREAM));
    }

    for (UINT stream = 0; stream < pStreamConfig->num_streams; stream++)
    {
        if ((m_pSnapshotStream == NULL) && (UsecaseSelector::IsJPEGSnapshotStream(pStreamConfig->streams[stream])))
        {
            m_pSnapshotStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[stream]);
            m_isSnapshotRequested = TRUE;
        }
        else if (m_pPreviewStream == NULL)
        {
            m_pPreviewStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[sortedFWKStreams[0][0]]);
        }
        else if ((m_pVideoStream == NULL) && (UsecaseSelector::IsVideoStream(pStreamConfig->streams[stream])))
        {
            m_pVideoStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[sortedFWKStreams[1][0]]);
        }
        else if ((m_pYUVStream == NULL) && (UsecaseSelector::IsVideoStream(pStreamConfig->streams[stream])))
        {
            // sharing snapshot stream in 3 video usecase
            m_pYUVStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[sortedFWKStreams[2][0]]);
        }
        else if ((m_pRealtimeYUVStream == NULL) && (UsecaseSelector::IsVideoStream(pStreamConfig->streams[stream])))
        {
            // sharing snapshot stream in 3 video usecase
            m_pRealtimeYUVStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[sortedFWKStreams[3][0]]);
        }
    }
    if ((m_pSnapshotStream != NULL) && (m_pYUVStream == NULL))
    {
        m_pYUVStream               = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
        m_InternalStreamConfig->streams[m_InternalStreamConfig->num_streams++] = reinterpret_cast<camera3_stream_t*>(m_pYUVStream);
        ChxUtils::Memcpy(m_pYUVStream, m_pPreviewStream, sizeof(ChiStream));
        m_pYUVStream->width        = m_pSnapshotStream->width;
        m_pYUVStream->height       = m_pSnapshotStream->height;
        m_pYUVStream->format       = ChiStreamFormatYCbCr420_888;
        m_pYUVStream->grallocUsage = GRALLOC1_CONSUMER_USAGE_CAMERA;
    }
    if (m_pSnapshotStream == NULL)
    {
        m_pSnapshotStream                    = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
        m_InternalStreamConfig->streams[m_InternalStreamConfig->num_streams++] = reinterpret_cast<camera3_stream_t*>(m_pSnapshotStream);
        m_pSnapshotStream->format            = ChiStreamFormatBlob;
        m_pSnapshotStream->width             = 320; // smaller size
        m_pSnapshotStream->height            = 240;
        m_pSnapshotStream->maxNumBuffers     = 0;
        m_pSnapshotStream->rotation          = StreamRotationCCW0;
        m_pSnapshotStream->streamType        = ChiStreamTypeBidirectional;
        m_pSnapshotStream->grallocUsage      = GRALLOC1_CONSUMER_USAGE_CAMERA;
    }
    if (m_pVideoStream == NULL)
    {
        m_pVideoStream          = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
        m_InternalStreamConfig->streams[m_InternalStreamConfig->num_streams++] = reinterpret_cast<camera3_stream_t*>(m_pVideoStream);
        ChxUtils::Memcpy(m_pVideoStream, m_pPreviewStream, sizeof(ChiStream));
        m_pVideoStream->width  = 320;
        m_pVideoStream->height = 240;
        m_pVideoStream->format       = ChiStreamFormatYCbCr420_888;
        m_pVideoStream->grallocUsage = GRALLOC1_CONSUMER_USAGE_CAMERA;
    }
    if (m_pYUVStream == NULL)
    {
        m_pYUVStream          = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
        m_InternalStreamConfig->streams[m_InternalStreamConfig->num_streams++] = reinterpret_cast<camera3_stream_t*>(m_pYUVStream);
        ChxUtils::Memcpy(m_pYUVStream, m_pPreviewStream, sizeof(ChiStream));
        m_pYUVStream->width  = 320;
        m_pYUVStream->height = 240;
        m_pYUVStream->format       = ChiStreamFormatYCbCr420_888;
        m_pYUVStream->grallocUsage = GRALLOC1_CONSUMER_USAGE_CAMERA;
    }
    if (m_pRealtimeYUVStream == NULL)
    {
        m_pRealtimeYUVStream          = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
        m_InternalStreamConfig->streams[m_InternalStreamConfig->num_streams++] = reinterpret_cast<camera3_stream_t*>(m_pRealtimeYUVStream);
        ChxUtils::Memcpy(m_pRealtimeYUVStream, m_pPreviewStream, sizeof(ChiStream));
        m_pRealtimeYUVStream->width  = 320;
        m_pRealtimeYUVStream->height = 240;
        m_pRealtimeYUVStream->format       = ChiStreamFormatYCbCr420_888;
        m_pRealtimeYUVStream->grallocUsage = GRALLOC1_CONSUMER_USAGE_CAMERA;
    }
}

VOID FeatureSHDR::sortFWKStream(camera3_stream_configuration_t* pStreamConfig)
{
    UINT index = 0, size = 0;
    // initialize to 0
    for (UINT stream = 0; stream < MAX_NUM_STREAMS; stream++)
    {
        // initialize to 0
        sortedFWKStreams[stream][0] = 0;
        sortedFWKStreams[stream][1] = 0;
    }

    for (UINT stream = 0; stream < pStreamConfig->num_streams; stream++)
    {
        if (UsecaseSelector::IsJPEGSnapshotStream(pStreamConfig->streams[stream]))
        {
            //exclude JPEG from Sorting.
            sortedFWKStreams[stream][0] = stream;
            sortedFWKStreams[stream][1] = 0;
        }
        else
        {
            sortedFWKStreams[stream][0] = stream;
            sortedFWKStreams[stream][1] = (pStreamConfig->streams[stream]->width * pStreamConfig->streams[stream]->height);
        }
    }
    for (UINT stream = 0; stream < (pStreamConfig->num_streams - 1); stream++)
    {
        for (UINT next = stream + 1; next < pStreamConfig->num_streams; next++)
        {
            if (sortedFWKStreams[stream][1] < sortedFWKStreams[next][1])
            {
                index = sortedFWKStreams[stream][0];
                size = sortedFWKStreams[stream][1];
                sortedFWKStreams[stream][0] = sortedFWKStreams[next][0];
                sortedFWKStreams[stream][1] = sortedFWKStreams[next][1];
                sortedFWKStreams[next][0] = index;
                sortedFWKStreams[next][1] = size;
            }
        }
    }
    for (UINT stream = 0; stream < (pStreamConfig->num_streams); stream++)
    {
        CHX_LOG("Sort : %d  %d", sortedFWKStreams[stream][0], sortedFWKStreams[stream][1]);
    }
}


