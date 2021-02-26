////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecasequadcfa.cpp
/// @brief Quad CFA Usecase class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "chxincs.h"
#include "chxpipeline.h"
#include "chxsession.h"
#include "chxusecase.h"
#include "chxusecasequadcfa.h"
#include "chxutils.h"

#include "chistatspropertydefines.h"

static const UINT32 maxQuadCfaOutputStreamsNum = 4;             ///< output streams number
volatile UINT32 UsecaseQuadCFA::m_aFlushInProgress    = FALSE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UsecaseQuadCFA* UsecaseQuadCFA::Create(
    LogicalCameraInfo*              cameraInfo,    ///< Camera info
    camera3_stream_configuration_t* pStreamConfig) ///< Stream configuration
{
    CDKResult       result          = CDKResultSuccess;
    UsecaseQuadCFA* pUsecaseQuadCFA = CHX_NEW UsecaseQuadCFA;

    if (NULL != pUsecaseQuadCFA)
    {
        result = pUsecaseQuadCFA->Initialize(cameraInfo, pStreamConfig);

        if (CDKResultSuccess != result)
        {
            pUsecaseQuadCFA->Destroy(FALSE);
            pUsecaseQuadCFA = NULL;
        }
    }
    else
    {
        result = CDKResultEFailed;
    }

    return pUsecaseQuadCFA;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseQuadCFA::Destroy(BOOL isForced)
{
    for (UINT i = 0 ; i < MaxSessions; i++)
    {
        if (NULL != m_pSession[i])
        {
            m_pSession[i]->Destroy(isForced);
            m_pSession[i] = NULL;
        }
    }

    for (UINT i = 0 ; i < MaxPipelinesPerSession; i++)
    {
        if (NULL != m_pPipeline[i])
        {
            m_pPipeline[i]->Destroy();
            m_pPipeline[i] = NULL;
        }
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

    for (UINT queueIndex = 0; queueIndex < ZSLQueueDepth; queueIndex++)
    {
        if (NULL != m_QCFARawQueue[queueIndex].pRdiOutputBuffer)
        {
            CHX_FREE(m_QCFARawQueue[queueIndex].pRdiOutputBuffer);
            m_QCFARawQueue[queueIndex].pRdiOutputBuffer = NULL;
        }
    }

    if (NULL != m_pQCFARawStream)
    {
        CHX_FREE(m_pQCFARawStream);
        m_pQCFARawStream = NULL;
    }

    if (NULL != m_pYuvDummyStream)
    {
        CHX_FREE(m_pYuvDummyStream);
        m_pYuvDummyStream = NULL;
    }

    if (NULL != m_pRawResultMutex)
    {
        m_pRawResultMutex->Destroy();
        m_pRawResultMutex = NULL;
    }

    if (NULL != m_pRawResultAvailable)
    {
        m_pRawResultAvailable->Destroy();
        m_pRawResultAvailable = NULL;
    }

    if (NULL != m_pRDIBufferManager)
    {
        m_pRDIBufferManager->Destroy();
        m_pRDIBufferManager = NULL;
    }

    m_pCachedMeta = NULL;
    if (NULL != m_pOfflineInputMeta)
    {
        ChxUtils::FreeMetaData(static_cast<camera_metadata_t*>(m_pOfflineInputMeta));
        m_pOfflineInputMeta = NULL;
    }

    if (NULL != m_debugDataOffline.pData)
    {
        CHX_FREE(m_debugDataOffline.pData);
        m_debugDataOffline.pData    = NULL;
        m_debugDataOffline.size     = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::Flush()
{
    ATRACE_BEGIN("UsecaseQuadCFA::Flush");
    ChxUtils::AtomicStoreU32(&m_aFlushInProgress, TRUE);
    Usecase::FlushLock(TRUE);
    for (UINT session = 0; session < MaxSessions; session++)
    {
        if (NULL != m_pSession[session])
        {
            ExtensionModule::GetInstance()->Flush(m_pSession[session]->GetSessionHandle());
        }
    }
    Usecase::FlushLock(FALSE);
    ChxUtils::AtomicStoreU32(&m_aFlushInProgress, FALSE);
    ATRACE_END();
    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::Initialize(
    LogicalCameraInfo*              cameraInfo,    ///< Camera info
    camera3_stream_configuration_t* pStreamConfig) ///< Stream configuration
{
    CDKResult               result = CDKResultSuccess;

    CHX_LOG("UsecaseQuadCFA::Initialize, E.");

    Usecase::Initialize();

    m_cameraId                   = cameraInfo->ppDeviceInfo[0]->cameraId;
    m_usecaseId                  = UsecaseId::QuadCFA;
    m_lastResultMetadataFrameNum = -1;
    m_debugDataOffline.pData     = NULL;
    m_debugDataOffline.size      = 0;


    ///< hard code the index for now, better to get it from usecase
    m_previewIdx    = QuadCFAPreview;
    m_QCFARawIdx    = QuadCFARealtimeRaw;
    m_offlineIdx    = QuadCFASnapshotJPEG;
    m_OfflineYuvIdx = QuadCFASnapshotYUV;

    for (UINT stream = 0; stream < pStreamConfig->num_streams; stream++)
    {
        CHX_LOG("stream = %p streamType = %d streamFormat = %d streamWidth = %d streamHeight = %d",
                pStreamConfig->streams[stream],
                pStreamConfig->streams[stream]->stream_type,
                pStreamConfig->streams[stream]->format,
                pStreamConfig->streams[stream]->width,
                pStreamConfig->streams[stream]->height);

        switch (pStreamConfig->streams[stream]->format)
        {
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
            m_pYuvStream = reinterpret_cast<CHISTREAM*>(pStreamConfig->streams[stream]);
            break;

        case HAL_PIXEL_FORMAT_BLOB:
            m_pSnapshotStream = reinterpret_cast<CHISTREAM*>(pStreamConfig->streams[stream]);
            break;

        case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:
            m_pPreviewStream = reinterpret_cast<CHISTREAM*>(pStreamConfig->streams[stream]);
            break;

        default:
            CHX_LOG("%s: Invalid Format: %d", __FUNCTION__, pStreamConfig->streams[stream]->format);
        }

        pStreamConfig->streams[stream]->max_buffers = 8;
        pStreamConfig->streams[stream]->usage       = GrallocUsageHwCameraWrite;
    }

    UINT rdiWidth  = 0;
    UINT rdiHeight = 0;
    for (UINT i = 0; i < cameraInfo->m_cameraCaps.numSensorModes; i++)
    {
        if (1 == cameraInfo->pSensorModeInfo[i].sensorModeCaps.u.QuadCFA)
        {
            rdiWidth  = cameraInfo->pSensorModeInfo[i].frameDimension.width;
            rdiHeight = cameraInfo->pSensorModeInfo[i].frameDimension.height;
            break;
        }
    }
    CHX_LOG("Quad CFA Raw size:%dx%d", rdiWidth, rdiHeight);

    UINT sizeBytes                  = sizeof(CHISTREAM);
    m_pQCFARawStream                = static_cast<CHISTREAM*>(CHX_CALLOC(sizeBytes));
    if (NULL != m_pQCFARawStream)
    {
        m_pQCFARawStream->format        = ChiStreamFormatRawOpaque;
        m_pQCFARawStream->width         = rdiWidth;
        m_pQCFARawStream->height        = rdiHeight;
        m_pQCFARawStream->maxNumBuffers = 0;
        m_pQCFARawStream->rotation      = StreamRotationCCW0;
        m_pQCFARawStream->streamType    = ChiStreamTypeBidirectional; // ChiStreamTypeBidirectional;
        m_pQCFARawStream->grallocUsage  = 0;
    }
    // add rdi stream to stream configuration
    camera3_stream_configuration_t localStreamConfig      = *pStreamConfig;
    camera3_stream_t* streams[maxQuadCfaOutputStreamsNum] = {NULL};

    localStreamConfig.streams                        = streams;
    ChxUtils::Memcpy(streams, pStreamConfig->streams, sizeof(camera3_stream_t*) * pStreamConfig->num_streams);
    streams[localStreamConfig.num_streams]           = reinterpret_cast<camera3_stream_t*>(m_pQCFARawStream);
    localStreamConfig.num_streams++;

    // add yuv stream to stream configureation
    m_pYuvDummyStream                      = static_cast<CHISTREAM*>(CHX_CALLOC(sizeBytes));
    if (NULL == m_pYuvDummyStream)
    {
        CHX_LOG("ERROR: no memory!");
        result = CDKResultENoMemory;
        return result;
    }
    m_pYuvDummyStream->format              = ChiStreamFormatYCbCr420_888;
    m_pYuvDummyStream->width               = rdiWidth;
    m_pYuvDummyStream->height              = rdiHeight;
    m_pYuvDummyStream->maxNumBuffers       = 0;
    m_pYuvDummyStream->rotation            = StreamRotationCCW0;
    m_pYuvDummyStream->streamType          = ChiStreamTypeOutput;
    m_pYuvDummyStream->grallocUsage        = 0;
    streams[localStreamConfig.num_streams] = reinterpret_cast<camera3_stream_t*>(m_pYuvDummyStream);
    localStreamConfig.num_streams++;

    m_pChiUsecase = UsecaseSelector::DefaultMatchingUsecase(&localStreamConfig);
    if (NULL == m_pChiUsecase)
    {
        CHX_LOG("ERROR: no matching usecase found!");
        result = CDKResultEFailed;
        return result;
    }

    result = CreateAllPipelineDescriptors();
    if (CDKResultSuccess == result)
    {
        result = StartPiplineAndSession(m_previewIdx);
    }
    else
    {
        CHX_LOG("ERROR: fail to create pipeline descriptors.");
    }

    if (CDKResultSuccess == result)
    {

        for (UINT stream = 0; stream < pStreamConfig->num_streams; stream++)
        {
            switch (pStreamConfig->streams[stream]->format)
            {
            // for framework request yuv output format
            case HAL_PIXEL_FORMAT_YCbCr_420_888:
                StartPiplineAndSession(m_OfflineYuvIdx);
                break;
            // for framework request blob or jpg output format
            case HAL_PIXEL_FORMAT_BLOB:
                StartPiplineAndSession(m_offlineIdx);
                break;

            default:
                CHX_LOG("Invalid Format: %d", pStreamConfig->streams[stream]->format);
            }
        }
    }
    else
    {
        CHX_LOG("ERROR: fail to start preview session/pipeline.");
    }

    BufferManagerCreateData createData = { 0 };
    createData.width                   = rdiWidth;
    createData.height                  = rdiHeight;
    createData.format                  = HAL_PIXEL_FORMAT_RAW10 ;
    createData.producerFlags           = GRALLOC1_PRODUCER_USAGE_CAMERA;
    createData.consumerFlags           = GRALLOC1_CONSUMER_USAGE_CAMERA;
    createData.numBuffers              = ZSLQueueDepth;

    m_pRDIBufferManager                = BufferManager::Create(&createData);

    if (NULL != m_pRDIBufferManager)
    {
        for (UINT queueIndex = 0; queueIndex < ZSLQueueDepth; queueIndex++)
        {
            m_QCFARawQueue[queueIndex].pRdiOutputBuffer =
                static_cast<CHISTREAMBUFFER*>(CHX_CALLOC(sizeof(CHISTREAMBUFFER)));

            if (NULL != m_QCFARawQueue[queueIndex].pRdiOutputBuffer)
            {
                m_QCFARawQueue[queueIndex].pRdiOutputBuffer->acquireFence = -1;
                m_QCFARawQueue[queueIndex].pRdiOutputBuffer->phBuffer     =
                    static_cast<CHIBUFFERHANDLE>(m_pRDIBufferManager->GetBuffer());
            }

            m_QCFARawQueue[queueIndex].isBufferValid = FALSE;
            m_QCFARawQueue[queueIndex].pMetadata     = NULL;
        }
    }
    else
    {
        CHX_LOG("ERROR. fail to create buffer manager.");
        result = CDKResultEFailed;
    }

    m_pRawResultMutex     = Mutex::Create();
    m_pRawResultAvailable = Condition::Create();

    m_pCachedMeta         = NULL;
    m_pOfflineInputMeta   = NULL;
    m_pSnapshotInputMeta  = NULL;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::ExecuteCaptureRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::ExecuteCaptureRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult result  = CDKResultSuccess;
    UINT32 frameIndex = (pRequest->frame_number % MaxOutstandingRequests);

    CHX_LOG("CFA capture request for frame %d with %d output buffers", pRequest->frame_number, pRequest->num_output_buffers);

    ///< Don't support request with both preview + snapshot, as we use RDI only path for snapshot
    if (pRequest->num_output_buffers > 1)
    {
        CHX_LOG("Invalid num output buffer: %d", pRequest->num_output_buffers);
        return CDKResultEFailed;
    }

    m_shutterTimestamp[frameIndex] = 0;
    m_isMetadataSent[frameIndex]   = FALSE;

    if (ChiStreamFormatBlob == pRequest->output_buffers[0].stream->format)
    {
        CHX_LOG("SNAPSHOT request received.");

        if (NULL != m_pCachedMeta)
        {
            m_pOfflineInputMeta = clone_camera_metadata(static_cast<const camera_metadata_t*>(m_pCachedMeta));
            CHX_LOG("clone the last preview result meta as offline input meta");

            if (TRUE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(m_pOfflineInputMeta), DebugDataTag))
            {
                CHAR* pData = NULL;
                ChxUtils::GetVendorTagValue((VOID*)(m_pOfflineInputMeta), DebugDataTag, (VOID**)&pData);
                DebugData* pDebug = reinterpret_cast<DebugData*>(pData);

                if ((NULL != pDebug) && (NULL != pDebug->pData) && (0 < pDebug->size))
                {
                    // Allocate debug-data for offline processing
                    if (NULL == m_debugDataOffline.pData)
                    {
                        m_debugDataOffline.pData    = CHX_CALLOC(pDebug->size);
                        m_debugDataOffline.size     = pDebug->size;
                    }
                    else if (pDebug->size != m_debugDataOffline.size)
                    {
                        // This condition is only for protection in case debug-data size is change in run time while camera is
                        // already processing. This is not allow for this property at this time.
                        CHX_FREE(m_debugDataOffline.pData);
                        m_debugDataOffline.pData    = CHX_CALLOC(pDebug->size);
                        m_debugDataOffline.size     = pDebug->size;
                    }

                    if (NULL != m_debugDataOffline.pData)
                    {
                        ChxUtils::Memcpy(m_debugDataOffline.pData, pDebug->pData, pDebug->size);

                        result = ChxUtils::SetVendorTagValue(m_pOfflineInputMeta,
                                                             DebugDataTag,
                                                             sizeof(DebugData),
                                                             &m_debugDataOffline);
                        if (CDKResultSuccess != result)
                        {
                            CHX_LOG_ERROR("Fail to set debugdata tag in offline input metadata");
                        }
                    }
                }
            }
        }

        m_pSnapshotInputMeta = ChxUtils::AllocateCopyMetaData(pRequest->settings);

        // switch to RDI pipeline
        result = DestroyPiplineAndSession(m_previewIdx);
        if (CDKResultSuccess == result)
        {
            result = StartPiplineAndSession(m_QCFARawIdx);
        }
        else
        {
            CHX_LOG("ERROR: fail to destroy preview pipeline/session.");
        }

        if (CDKResultSuccess == result)
        {
            // submit request on rdi pipeline
            result = GenerateRDIRequest(pRequest);
        }
        else
        {
            CHX_LOG("ERROR: fail to start realtime raw pipeline/session.");
        }

        // we need block here until we got the rdi frame
        CHX_LOG("wait for rdi ready.");
        m_pRawResultMutex->Lock();
        while (FALSE == m_QCFARawQueue[0].isBufferValid ||
               NULL  == m_QCFARawQueue[0].pMetadata)
        {
            m_pRawResultAvailable->Wait(m_pRawResultMutex->GetNativeHandle());
        }
        m_pRawResultMutex->Unlock();
        CHX_LOG("after rdi ready, sending buffer:%p to reprocess", m_QCFARawQueue[0].pRdiOutputBuffer->phBuffer);

        result = GenerateSnapshotRequest(pRequest);

        m_QCFARawQueue[0].isBufferValid = FALSE;  // reset buffer valid flag to false
        m_QCFARawQueue[0].pMetadata     = NULL;

        // switch to preview pipeline again
        result = DestroyPiplineAndSession(m_QCFARawIdx);
        if (CDKResultSuccess == result)
        {
            result = StartPiplineAndSession(m_previewIdx);
        }
        else
        {
            CHX_LOG("ERROR: fail to destroy realtime raw pipeline/session.");
        }
    }
    else if (ChiStreamFormatYCbCr420_888 == pRequest->output_buffers[0].stream->format)
    {
        if (NULL != m_pCachedMeta)
        {
            m_pOfflineInputMeta = clone_camera_metadata(static_cast<const camera_metadata_t*>(m_pCachedMeta));
            CHX_LOG("clone the last preview result meta as offline input meta");
        }

        m_pSnapshotInputMeta = ChxUtils::AllocateCopyMetaData(pRequest->settings);

        // switch to RDI pipeline
        result = DestroyPiplineAndSession(m_previewIdx);

        if (CDKResultSuccess == result)
        {
            result = StartPiplineAndSession(m_QCFARawIdx);
        }
        else
        {
            CHX_LOG("ERROR: fail to destroy preview pipeline/session.");
        }

        if (CDKResultSuccess == result)
        {
            // submit request on rdi pipeline
            result = GenerateRDIRequest(pRequest);
        }
        else
        {
            CHX_LOG("ERROR: fail to start realtime raw pipeline/session.");
        }

        // we need block here until we got the rdi frame
        m_pRawResultMutex->Lock();
        while (FALSE == m_QCFARawQueue[0].isBufferValid ||
               NULL  == m_QCFARawQueue[0].pMetadata)
        {
            m_pRawResultAvailable->Wait(m_pRawResultMutex->GetNativeHandle());
        }
        m_pRawResultMutex->Unlock();
        CHX_LOG("after rdi ready, sending buffer:%p to reprocess", m_QCFARawQueue[0].pRdiOutputBuffer->phBuffer);

        result = GenerateSnapshotYuvRequest(pRequest);

        m_QCFARawQueue[0].isBufferValid = FALSE;  // reset buffer valid flag to false
        m_QCFARawQueue[0].pMetadata     = NULL;
        // switch to preview pipeline again
        result = DestroyPiplineAndSession(m_QCFARawIdx);
        if (CDKResultSuccess == result)
        {
            result = StartPiplineAndSession(m_previewIdx);
        }
        else
        {
            CHX_LOG("ERROR: fail to destroy realtime raw pipeline/session.");
        }
    }
    else
    {
        // for preview request
        CHICAPTUREREQUEST chiRequest = { 0 };

        chiRequest.frameNumber       = pRequest->frame_number;
        chiRequest.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(m_pSession[m_previewIdx]->GetPipelineHandle());
        chiRequest.numOutputs        = pRequest->num_output_buffers;
        chiRequest.pOutputBuffers    =
        reinterpret_cast<CHISTREAMBUFFER*>
        (const_cast<camera3_stream_buffer*>(pRequest->output_buffers));
        chiRequest.pMetadata         = pRequest->settings;

        UINT32 sensorModeIndex = m_pSession[m_previewIdx]->GetSensorModeInfo(0)->modeIndex;
        ChxUtils::FillTuningModeData(const_cast<VOID*>(chiRequest.pMetadata),
                                     pRequest,
                                     sensorModeIndex,
                                     &m_effectModeValue,
                                     &m_sceneModeValue,
                                     &m_tuningFeature1Value,
                                     &m_tuningFeature2Value);
        ChxUtils::FillCameraId(const_cast<camera_metadata_t*>(pRequest->settings), m_cameraId);

        CHIPIPELINEREQUEST submitRequest;

        submitRequest.pSessionHandle   = reinterpret_cast<CHIHANDLE>(m_pSession[m_previewIdx]->GetSessionHandle());
        submitRequest.numRequests      = 1;
        submitRequest.pCaptureRequests = &chiRequest;

        result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::GenerateSnapshotRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::GenerateSnapshotRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult         result     = CDKResultSuccess;

    camera3_stream_buffer_t inputBuffer;
    inputBuffer.acquire_fence    = -1;
    inputBuffer.buffer           = reinterpret_cast<buffer_handle_t*>(m_QCFARawQueue[0].pRdiOutputBuffer->phBuffer);
    inputBuffer.stream           = reinterpret_cast<camera3_stream_t*>(m_pQCFARawStream);

    CHICAPTUREREQUEST chiRequest = { 0 };
    chiRequest.frameNumber       = pRequest->frame_number;
    chiRequest.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(m_pSession[m_offlineIdx]->GetPipelineHandle());
    chiRequest.numInputs         = 1;
    chiRequest.numOutputs        = 1;
    chiRequest.pInputBuffers     = reinterpret_cast<CHISTREAMBUFFER*>(&inputBuffer);
    chiRequest.pOutputBuffers    =
        reinterpret_cast<CHISTREAMBUFFER*>
            (const_cast<camera3_stream_buffer*>(pRequest->output_buffers));
    chiRequest.pMetadata         = m_QCFARawQueue[0].pMetadata;

    if (NULL != m_pSnapshotInputMeta)
    {
        INT status = 0;
        status = ChxUtils::MergeMetadata(m_QCFARawQueue[0].pMetadata,
                                        reinterpret_cast<camera_metadata_t*>(m_pSnapshotInputMeta));
        if (0 != status)
        {
            CHX_LOG("WARNING: can't merge meta data");
        }
        chiRequest.pMetadata = m_pSnapshotInputMeta;
    }
    else
    {
        chiRequest.pMetadata = m_QCFARawQueue[0].pMetadata;
    }

    UINT32 sensorModeIndex = m_pSession[m_offlineIdx]->GetSensorModeInfo(0)->modeIndex;
    ChxUtils::FillTuningModeData(const_cast<VOID*>(chiRequest.pMetadata),
                                pRequest,
                                sensorModeIndex,
                                &m_effectModeValue,
                                &m_sceneModeValue,
                                &m_tuningFeature1Value,
                                &m_tuningFeature2Value);
    ChxUtils::FillCameraId(const_cast<VOID*>(chiRequest.pMetadata), m_cameraId);
    CHX_LOG("sensorModeIndex:%d, camera id:%d", sensorModeIndex, m_cameraId);

    ChiTuningModeParameter* pTunningModeData = NULL;
    ChxUtils::GetVendorTagValue(const_cast<VOID*>(chiRequest.pMetadata),
        TuningMode, reinterpret_cast<VOID **>(&pTunningModeData));
    if (NULL != pTunningModeData)
    {
        for (UINT32 i = 0; i < pTunningModeData->noOfSelectionParameter; i++)
        {
            if (ChiModeType::Usecase == pTunningModeData->TuningMode[i].mode)
            {
                pTunningModeData->TuningMode[i].subMode.usecase = ChiModeUsecaseSubModeType::Snapshot;
            }
        }
    }
    else
    {
        CHX_LOG("WARNING: can't find tunningModeData vendortag in meta");
    }

    CHIPIPELINEREQUEST submitRequest = { 0 };

    submitRequest.pSessionHandle     = reinterpret_cast<CHIHANDLE>(m_pSession[m_offlineIdx]->GetSessionHandle());
    submitRequest.numRequests        = 1;
    submitRequest.pCaptureRequests   = &chiRequest;

    result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

    if (NULL != m_pOfflineInputMeta)
    {
        ChxUtils::FreeMetaData(static_cast<camera_metadata_t*>(m_pOfflineInputMeta));
        m_pOfflineInputMeta = NULL;
    }
    if (NULL != m_pSnapshotInputMeta)
    {
        ChxUtils::FreeMetaData(static_cast<camera_metadata_t*>(m_pSnapshotInputMeta));
        m_pSnapshotInputMeta = NULL;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::GenerateSnapshotRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::GenerateSnapshotYuvRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult         result     = CDKResultSuccess;

    camera3_stream_buffer_t inputBuffer;
    inputBuffer.acquire_fence    = -1;
    inputBuffer.buffer           = reinterpret_cast<buffer_handle_t*>(m_QCFARawQueue[0].pRdiOutputBuffer->phBuffer);
    inputBuffer.stream           = reinterpret_cast<camera3_stream_t*>(m_pQCFARawStream);

    CHICAPTUREREQUEST chiRequest = { 0 };
    chiRequest.frameNumber       = pRequest->frame_number;
    chiRequest.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(m_pSession[m_OfflineYuvIdx]->GetPipelineHandle());
    chiRequest.numInputs         = 1;
    chiRequest.numOutputs        = 1;
    chiRequest.pInputBuffers     = reinterpret_cast<CHISTREAMBUFFER*>(&inputBuffer);
    chiRequest.pOutputBuffers    =
        reinterpret_cast<CHISTREAMBUFFER*>
            (const_cast<camera3_stream_buffer*>(pRequest->output_buffers));
    chiRequest.pMetadata         = m_QCFARawQueue[0].pMetadata;

    if (NULL != m_pSnapshotInputMeta)
    {
        INT status = 0;
        status = ChxUtils::MergeMetadata(reinterpret_cast<camera_metadata_t*>(m_pSnapshotInputMeta),
                                        m_QCFARawQueue[0].pMetadata);
        if (0 != status)
        {
            CHX_LOG("WARNING: can't merge meta data");
        }
        chiRequest.pMetadata = m_QCFARawQueue[0].pMetadata;
    }

    UINT32 sensorModeIndex = m_pSession[m_OfflineYuvIdx]->GetSensorModeInfo(0)->modeIndex;
    ChxUtils::FillTuningModeData(const_cast<VOID*>(chiRequest.pMetadata),
                                pRequest,
                                sensorModeIndex,
                                &m_effectModeValue,
                                &m_sceneModeValue,
                                &m_tuningFeature1Value,
                                &m_tuningFeature2Value);
    ChxUtils::FillCameraId(const_cast<VOID*>(chiRequest.pMetadata), m_cameraId);
    CHX_LOG("sensorModeIndex:%d, camera id:%d", sensorModeIndex, m_cameraId);

    ChiTuningModeParameter* pTunningModeData = NULL;
    ChxUtils::GetVendorTagValue(const_cast<VOID*>(chiRequest.pMetadata),
        TuningMode, reinterpret_cast<VOID **>(&pTunningModeData));
    if (NULL != pTunningModeData)
    {
        for (UINT32 i = 0; i < pTunningModeData->noOfSelectionParameter; i++)
        {
            if (ChiModeType::Usecase == pTunningModeData->TuningMode[i].mode)
            {
                pTunningModeData->TuningMode[i].subMode.usecase = ChiModeUsecaseSubModeType::Snapshot;
            }
        }
    }
    else
    {
        CHX_LOG("WARNING: can't find tunningModeData vendortag in meta");
    }

    CHIPIPELINEREQUEST submitRequest = { 0 };

    submitRequest.pSessionHandle     = reinterpret_cast<CHIHANDLE>(m_pSession[m_OfflineYuvIdx]->GetSessionHandle());
    submitRequest.numRequests        = 1;
    submitRequest.pCaptureRequests   = &chiRequest;

    result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

    if (NULL != m_pOfflineInputMeta)
    {
        ChxUtils::FreeMetaData(static_cast<camera_metadata_t*>(m_pOfflineInputMeta));
        m_pOfflineInputMeta = NULL;
    }
    if (NULL != m_pSnapshotInputMeta)
    {
        ChxUtils::FreeMetaData(static_cast<camera_metadata_t*>(m_pSnapshotInputMeta));
        m_pSnapshotInputMeta = NULL;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::GenerateRDIRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::GenerateRDIRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult         result     = CDKResultSuccess;
    CHICAPTUREREQUEST chiRequest = { 0 };

    camera3_stream_buffer_t outputBuffer;
    outputBuffer.acquire_fence   = -1;
    outputBuffer.buffer          = m_pRDIBufferManager->GetBuffer();
    outputBuffer.stream          = reinterpret_cast<camera3_stream_t*>(m_pQCFARawStream);

    chiRequest.frameNumber       = pRequest->frame_number;
    chiRequest.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(m_pSession[m_QCFARawIdx]->GetPipelineHandle());
    chiRequest.numOutputs        = 1;
    chiRequest.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(&outputBuffer);
    chiRequest.pMetadata         = pRequest->settings;


    UINT32 sensorModeIndex = m_pSession[m_QCFARawIdx]->GetSensorModeInfo(0)->modeIndex;
    ChxUtils::FillTuningModeData(const_cast<VOID*>(chiRequest.pMetadata),
                                 pRequest,
                                 sensorModeIndex,
                                 &m_effectModeValue,
                                 &m_sceneModeValue,
                                 &m_tuningFeature1Value,
                                 &m_tuningFeature2Value);

    CHIPIPELINEREQUEST submitRequest = { 0 };

    submitRequest.pSessionHandle   = reinterpret_cast<CHIHANDLE>(m_pSession[m_QCFARawIdx]->GetSessionHandle());
    submitRequest.numRequests      = 1;
    submitRequest.pCaptureRequests = &chiRequest;

    result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::SessionCbNotifyMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseQuadCFA::SessionCbNotifyMessage(
    const ChiMessageDescriptor* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{
    /// @todo Provide common Get functions to do the below two casts
    SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    UsecaseQuadCFA*   pUsecaseQuadCfa       = static_cast<UsecaseQuadCFA*>(pSessionPrivateData->pUsecase);

    CHX_LOG("frame number:%d, session idx:%d",
        pMessageDescriptor->message.shutterMessage.frameworkFrameNum,
        pSessionPrivateData->sessionId);

    if (ChiMessageTypeSof == pMessageDescriptor->messageType)
    {
        // SOF notifications are not sent to the HAL3 application
        CHX_LOG("Chi Notify SOF frameNum %u framework frameNum %u, timestamp %" PRIu64,
            pMessageDescriptor->message.sofMessage.sofId,
            pMessageDescriptor->message.sofMessage.frameworkFrameNum,
            pMessageDescriptor->message.sofMessage.timestamp);
    }
    else
    {
        // don't send notify message for offline session
        if (pSessionPrivateData->sessionId == QuadCFAPreview ||
            pSessionPrivateData->sessionId == QuadCFARealtimeRaw)
        {
            pUsecaseQuadCfa->SessionProcessMessage(pMessageDescriptor);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::SessionProcessMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseQuadCFA::SessionProcessMessage(
    const ChiMessageDescriptor* pMessageDescriptor)
{
    CHX_LOG("frame number:%d", pMessageDescriptor->message.shutterMessage.frameworkFrameNum);

    UINT32 frameIdx              = (pMessageDescriptor->message.shutterMessage.frameworkFrameNum % MaxOutstandingRequests);
    m_shutterTimestamp[frameIdx] = pMessageDescriptor->message.shutterMessage.timestamp;

    ReturnFrameworkMessage((camera3_notify_msg_t*)pMessageDescriptor, m_cameraId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::SessionCbCaptureResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseQuadCFA::SessionCbCaptureResult(
    ChiCaptureResult* pResult,
    VOID*             pPrivateCallbackData)
{
    if(TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aFlushInProgress)))
    {
        //ignore IR result when in destory and dont process flushed result
        return ;
    }
    SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    UsecaseQuadCFA* pUsecaseQuadCfa         = static_cast<UsecaseQuadCFA*>(pSessionPrivateData->pUsecase);
    UINT sessionIdx                         = pSessionPrivateData->sessionId;

    CHX_LOG("E. session:%d, frame num:%d, result meta:%p, result output buffer:%d",
        sessionIdx, pResult->frameworkFrameNum,
        pResult->pResultMetadata,
        pResult->numOutputBuffers);

    if (QuadCFAPreview == sessionIdx)
    {
        pUsecaseQuadCfa->ProcessPreviewResult(pResult);
    }
    else if (QuadCFARealtimeRaw == sessionIdx)
    {
        pUsecaseQuadCfa->ProcessRdiResult(pResult);
    }
    else if (QuadCFASnapshotJPEG == sessionIdx)
    {
        pUsecaseQuadCfa->ProcessSnapshotResult(pResult);
    }
    else if (QuadCFASnapshotYUV == sessionIdx)
    {
        pUsecaseQuadCfa->ProcessYuvResult(pResult);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::ProcessPreviewResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::ProcessPreviewResult(
    ChiCaptureResult* pResult)
{
    CDKResult result  = CDKResultSuccess;
    UINT32 frameIndex = (pResult->frameworkFrameNum % MaxOutstandingRequests);

    result = UpdateSensorTimestamp(pResult);

    if (pResult->pResultMetadata != NULL)
    {
        m_pCachedMeta = pResult->pResultMetadata;
        CHX_LOG("cache preview metadata, frame number:%d.", pResult->frameworkFrameNum);
    }

    ReturnFrameworkResult(reinterpret_cast<const camera3_capture_result_t*>(pResult), m_cameraId);

    m_numAppPendingOutputBuffers[frameIndex] -= pResult->numOutputBuffers;

    // Moving forward if there is no pending output buffer and metadata
    if (0 == m_numAppPendingOutputBuffers[frameIndex])
    {
        m_nextAppResultFrame++;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::ProcessRdiResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::ProcessRdiResult(
    ChiCaptureResult* pResult)
{
    CDKResult result  = CDKResultSuccess;

    m_pRawResultMutex->Lock();

    if (pResult->pResultMetadata != NULL)
    {
        m_QCFARawQueue[0].pMetadata   = (void *)(pResult->pResultMetadata);
        m_QCFARawQueue[0].frameNumber = pResult->frameworkFrameNum;
        CHX_LOG("store meta for rdi output.");

        if (TRUE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(m_pOfflineInputMeta), DebugDataTag))
        {
            CHAR* pData = NULL;
            ChxUtils::GetVendorTagValue((VOID*)(m_pOfflineInputMeta), DebugDataTag, (VOID**)&pData);
            DebugData* pDebug = reinterpret_cast<DebugData*>(pData);

            if ((NULL != pDebug) && (NULL != pDebug->pData) && (0 < pDebug->size))
            {
                result = ChxUtils::SetVendorTagValue((VOID*)m_QCFARawQueue[0].pMetadata, DebugDataTag, sizeof(DebugData), pDebug);
                if (CDKResultSuccess != result)
                {
                    CHX_LOG_ERROR("fail to set debugdata tag");
                }
            }
       }

        UpdateSensorTimestamp(pResult);

        // send result meta of rdi capture to fwk, so it won't misorder with upcoming preivew request
        // for frame, will send final jpeg to framework
        ChiCaptureResult captureResult = { 0 };

        ChxUtils::Memcpy(&captureResult, pResult, sizeof(ChiCaptureResult));
        captureResult.numOutputBuffers = 0;
        captureResult.pOutputBuffers   = NULL;

        ReturnFrameworkResult(reinterpret_cast<const camera3_capture_result_t*>(&captureResult), m_cameraId);
    }

    for (UINT i = 0 ; i < pResult->numOutputBuffers; i++)
    {
        if (pResult->pOutputBuffers[i].pStream == m_pQCFARawStream)
        {
            ChxUtils::Memcpy(m_QCFARawQueue[0].pRdiOutputBuffer,
                 const_cast<CHISTREAMBUFFER*>(&pResult->pOutputBuffers[i]),
                 sizeof(CHISTREAMBUFFER));
            m_QCFARawQueue[0].pRdiOutputBuffer->acquireFence = -1;

            m_QCFARawQueue[0].frameNumber                    = pResult->frameworkFrameNum;
            m_QCFARawQueue[0].isBufferValid                  = TRUE;
        }
    }

    if (TRUE == m_QCFARawQueue[0].isBufferValid && NULL != m_QCFARawQueue[0].pMetadata)
    {
        CHX_LOG("notify RDI buffer is ready, buffer handler:%p!", m_QCFARawQueue[0].pRdiOutputBuffer->phBuffer);
        m_pRawResultAvailable->Signal();
    }

    m_pRawResultMutex->Unlock();
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::ProcessSnapshotResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::ProcessSnapshotResult(
    ChiCaptureResult* pResult)
{
    CDKResult result  = CDKResultSuccess;
    UINT32 frameIndex = (pResult->frameworkFrameNum % MaxOutstandingRequests);

    result = UpdateSensorTimestamp(pResult);

    if (NULL != pResult->pResultMetadata)
    {
        CHX_LOG("SNAPSHOT result meta received.");
    }

    for (UINT i = 0 ; i < pResult->numOutputBuffers; i++)
    {
        if (pResult->pOutputBuffers[i].pStream == m_pSnapshotStream)
        {
            CHX_LOG("SNAPSHOT buffer received, buffer handler:%p!", pResult->pOutputBuffers[i].phBuffer);

            // don't send input buffer and result meta of jpeg to framework
            //  - input buffer is internal
            //  - result meta of this request already sent to framew work, from RDI capture result
            pResult->pInputBuffer    = NULL;
            pResult->pResultMetadata = NULL;

            ReturnFrameworkResult(reinterpret_cast<const camera3_capture_result_t*>(pResult), m_cameraId);

            m_numAppPendingOutputBuffers[frameIndex] -= pResult->numOutputBuffers;

            // Moving forward if there is no pending output buffer and metadata
            if (0 == m_numAppPendingOutputBuffers[frameIndex])
            {
                m_nextAppResultFrame++;
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::ProcessSnapshotResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::ProcessYuvResult(
    ChiCaptureResult* pResult)
{
    CDKResult result  = CDKResultSuccess;
    UINT32 frameIndex = (pResult->frameworkFrameNum % MaxOutstandingRequests);

    result = UpdateSensorTimestamp(pResult);

    if (NULL != pResult->pResultMetadata)
    {
        CHX_LOG("SNAPSHOT result meta received.");
    }

    for (UINT i = 0 ; i < pResult->numOutputBuffers; i++)
    {
        if (pResult->pOutputBuffers[i].pStream == m_pYuvStream)
        {
            CHX_LOG("Yuv SNAPSHOT buffer received, buffer handler:%p!", pResult->pOutputBuffers[i].phBuffer);

            // don't send input buffer and result meta of jpeg to framework
            //  - input buffer is internal
            //  - result meta of this request already sent to framew work, from RDI capture result
            pResult->pInputBuffer    = NULL;
            pResult->pResultMetadata = NULL;

            ReturnFrameworkResult(reinterpret_cast<const camera3_capture_result_t*>(pResult), m_cameraId);

            m_numAppPendingOutputBuffers[frameIndex] -= pResult->numOutputBuffers;

            // Moving forward if there is no pending output buffer and metadata
            if (0 == m_numAppPendingOutputBuffers[frameIndex])
            {
                m_nextAppResultFrame++;
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::UpdateSensorTimestamp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::UpdateSensorTimestamp(
    ChiCaptureResult* pResult)
{
    CDKResult result  = CDKResultSuccess;
    UINT32 frameIndex = (pResult->frameworkFrameNum % MaxOutstandingRequests);

    if (pResult->pResultMetadata != NULL)
    {
        camera_metadata_entry_t entry          = { 0 };
        static const UINT32 SensorTimestampTag = 917520; // Defined by android
        entry.tag                              = SensorTimestampTag;
        INT32 status                           = 1;

#if defined (_LINUX)
        status = find_camera_metadata_entry((camera_metadata_t*)(pResult->pResultMetadata), entry.tag, &entry);
#endif // _LINUX

        if (-ENOENT == status)
        {
            status = add_camera_metadata_entry((camera_metadata_t*)(pResult->pResultMetadata),
                SensorTimestampTag,
                &m_shutterTimestamp[frameIndex],
                1);
        }
        else if (0 == status)
        {
            status = update_camera_metadata_entry((camera_metadata_t*)(pResult->pResultMetadata),
                entry.index,
                &m_shutterTimestamp[frameIndex],
                1,
                NULL);
        }

        CHX_LOG("Update sensor timestamp for frame %d, timestamp = %" PRIu64,
            pResult->frameworkFrameNum, m_shutterTimestamp[frameIndex]);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::CreateAllPipelineDescriptors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::CreateAllPipelineDescriptors()
{
    CDKResult result = CDKResultSuccess;

    CHX_LOG("numPipelines:%d", m_pChiUsecase->numPipelines);

    for (UINT i = 0; i < m_pChiUsecase->numPipelines; i++)
    {
        result = CreatePipelineDescriptor(i);
        if (CDKResultSuccess != result)
        {
            CHX_LOG("fail to create pipeline descriptor!");
            break;
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::CreatePipelineDescriptor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::CreatePipelineDescriptor(
    UINT32 pipelineIdx)
{
    CDKResult result = CDKResultSuccess;
    UINT32 i         = pipelineIdx;
    ChiPortBufferDescriptor pipelineOutputBuffer[MaxChiStreams];
    ChiPortBufferDescriptor pipelineInputBuffer[MaxChiStreams];

    if (i >= MaxPipelinesPerSession)
    {
        CHX_LOG("Error: pipelineIdx exceed MaxPipelinesPerSession");
        return CDKResultEFailed;
    }

    CHIPIPELINECREATEDESCRIPTOR pipeline_desc = m_pChiUsecase->pPipelineTargetCreateDesc[i].pipelineCreateDesc;
    CHX_LOG("i:%d, creating pipeline:%s E. num node:%d",
        i,
        m_pChiUsecase->pPipelineTargetCreateDesc[i].pPipelineName,
        pipeline_desc.numNodes);

    ChiTargetPortDescriptorInfo* pSinkTarget = &m_pChiUsecase->pPipelineTargetCreateDesc[i].sinkTarget;
    ChiTargetPortDescriptorInfo* pSrcTarget  = &m_pChiUsecase->pPipelineTargetCreateDesc[i].sourceTarget;

    for (UINT sinkIdx = 0; sinkIdx < pSinkTarget->numTargets; sinkIdx++)
    {
        ChiTargetPortDescriptor* pSinkTargetDesc = &pSinkTarget->pTargetPortDesc[sinkIdx];

        CHX_ASSERT(pSinkTargetDesc->pTarget->pChiStream != NULL);
        pipelineOutputBuffer[sinkIdx].pStream    = pSinkTargetDesc->pTarget->pChiStream;
        pipelineOutputBuffer[sinkIdx].nodePort   = pSinkTargetDesc->nodeport;
    }

    for (UINT sourceIdx = 0; sourceIdx < pSrcTarget->numTargets; sourceIdx++)
    {
        ChiTargetPortDescriptor* pSrcTargetDesc = &pSrcTarget->pTargetPortDesc[sourceIdx];

        pipelineInputBuffer[sourceIdx].pStream  = pSrcTargetDesc->pTarget->pChiStream;
        pipelineInputBuffer[sourceIdx].nodePort = pSrcTargetDesc->nodeport;
    }

    m_pPipeline[i] = Pipeline::Create(m_cameraId, PipelineType::Default);

    if (NULL != m_pPipeline[i])
    {
        m_pPipeline[i]->SetOutputBuffers(pSinkTarget->numTargets, &pipelineOutputBuffer[0]);
        m_pPipeline[i]->SetInputBuffers(pSrcTarget->numTargets, &pipelineInputBuffer[0]);
        m_pPipeline[i]->SetPipelineNodePorts(&m_pChiUsecase->pPipelineTargetCreateDesc[i].pipelineCreateDesc);

        result = m_pPipeline[i]->CreateDescriptor();
    }

    CHX_LOG("result:%d, creating pipeline:%s, X.",
        result,
        m_pChiUsecase->pPipelineTargetCreateDesc[i].pPipelineName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::StartPiplineAndSession
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::StartPiplineAndSession(
    UINT32 pipelineIdx)
{
    CDKResult result = CDKResultSuccess;
    UINT32 i         = pipelineIdx;

    CHX_LOG("E. idx:%d", i);

    if (i >= MaxPipelinesPerSession)
    {
        CHX_LOG("Error: pipelineIdx exceed MaxPipelinesPerSession");
        return CDKResultEFailed;
    }

    if (NULL == m_pPipeline[pipelineIdx])
    {
        result = CreatePipelineDescriptor(pipelineIdx);
    }

    if (CDKResultEFailed == result)
    {
        return result;
    }

    ChiCallBacks callbacks            = { 0 };
    callbacks.ChiNotify               = SessionCbNotifyMessage;
    callbacks.ChiProcessCaptureResult = SessionCbCaptureResult;

    if (pipelineIdx >= MaxSessions)
    {
        CHX_LOG_ERROR("Error: pipelineIdx exceed MaxSession");
        result = CDKResultEFailed;
        return result;
    }

    m_perSessionPvtData[pipelineIdx].sessionId  = pipelineIdx;
    m_perSessionPvtData[pipelineIdx].pUsecase   = this;

    m_pSession[pipelineIdx] = Session::Create(&m_pPipeline[pipelineIdx], 1, &callbacks, &m_perSessionPvtData[pipelineIdx]);

    if (NULL == m_pSession[pipelineIdx])
    {
        result = CDKResultEFailed;
    }

    CHX_LOG(" X. result:%d", result);
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseQuadCFA::DestroyPiplineAndSession
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseQuadCFA::DestroyPiplineAndSession(
    UINT32 pipelineIdx)
{
    CDKResult result = CDKResultSuccess;

    CHX_LOG("E. idx:%d", pipelineIdx);

    if (NULL != m_pSession[pipelineIdx])
    {
        m_pSession[pipelineIdx]->Destroy(FALSE);
        m_pSession[pipelineIdx] = NULL;
    }

    CHX_LOG(" X. result:%d", result);
    return result;
}


