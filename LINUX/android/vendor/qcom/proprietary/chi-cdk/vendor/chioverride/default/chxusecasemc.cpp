////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecasemc.cpp
/// @brief Usecases class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxusecase.h"
#include "chxusecasemc.h"
#include "chistatspropertydefines.h"
#include "chxutils.h"
#include "chxmulticamcontroller.h"

#define ATRACE_TAG (ATRACE_TAG_CAMERA | ATRACE_TAG_HAL)
#include <cutils/trace.h>
volatile UINT32 UsecaseMultiCamera::m_aFlushInProgress    = FALSE;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::~UsecaseMultiCamera
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UsecaseMultiCamera::~UsecaseMultiCamera()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UsecaseMultiCamera* UsecaseMultiCamera::Create(
    LogicalCameraInfo*              cameraInfo,    ///< Camera info
    camera3_stream_configuration_t* pStreamConfig) ///< Stream configuration
{
    CDKResult             result                = CDKResultSuccess;
    UsecaseMultiCamera* pUsecaseMultiCamera = CHX_NEW UsecaseMultiCamera;

    if (NULL != pUsecaseMultiCamera)
    {
        result = pUsecaseMultiCamera->Initialize(cameraInfo, pStreamConfig);

        if (CDKResultSuccess != result)
        {
            pUsecaseMultiCamera->Destroy(FALSE);
            pUsecaseMultiCamera = NULL;
        }
    }
    else
    {
        result = CDKResultEFailed;
    }

    return pUsecaseMultiCamera;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseMultiCamera::RemapPipeline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 UsecaseMultiCamera::RemapPipelineIndex(
    UINT32              pipelineIndexFromUsecase,
    DualCameraUsecase   usecase)
{
    UINT32 pipelineIndex = INVALID_PIPELINEID;

    if (usecase == UsecaseSAT)
    {
        switch (pipelineIndexFromUsecase)
        {
            case RealtimePreviewSAT0:
                pipelineIndex = REALTIMEPIPELINEMAIN;
                break;
            case RealtimePreviewSAT1:
                pipelineIndex = REALTIMEPIPELINEAUX;
                break;
            case SATOfflinePreview:
                pipelineIndex = OFFLINEPREVIEWPIPELINE;
                break;
            case SATSnapshotYUV0:
                pipelineIndex = OFFLINEYUVPIPELINE;
                break;
            case SATSnapshotYUV1:
                pipelineIndex = OFFLINEYUVPIPELINEAUX;
                break;
            case SATOfflineSnapshot:
                pipelineIndex = OFFLINEFUSIONPIPELINE;
                break;
            case SATJpegEncode:
                pipelineIndex = OFFLINEJPEGPIPELINE;
                break;
            case SATOfflineRAW16:
                pipelineIndex = OFFLINERAW16PIPELINE;
                break;
            default:
                CHX_LOG_ERROR("ERROR: Pipeline index is more than expected");
        }
    }
    else if (usecase == UsecaseRTB)
    {
        switch (pipelineIndexFromUsecase)
        {
            case RealtimePreview0:
                pipelineIndex = REALTIMEPIPELINEMAIN;
                break;
            case RealtimePreview1:
                pipelineIndex = REALTIMEPIPELINEAUX;
                break;
            case RTBOfflinePreview:
                pipelineIndex = OFFLINEPREVIEWPIPELINE;
                break;
            case RTBSnapshotYUV0:
                pipelineIndex = OFFLINEYUVPIPELINE;
                break;
            case RTBSnapshotYUV1:
                pipelineIndex = OFFLINEYUVPIPELINEAUX;
                break;
            case RTBOfflineSnapshot:
                pipelineIndex = OFFLINEFUSIONPIPELINE;
                break;
            case RTBJpegEncode:
                pipelineIndex = OFFLINEJPEGPIPELINE;
                break;
            default:
                CHX_LOG_ERROR("ERROR: Pipeline index is more than expected");
        }
    }
    else
    {
        CHX_LOG_ERROR("ERROR: Cannot come here undefined usecase");
    }

    return pipelineIndex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::Destroy(BOOL isForced)
{
    DestroyDeferResource();

    DestroyOfflineProcessResource();
    // AdvancedCameraUsecase just initialize in SAT mode, so destroy it in SAT mode.
    if (canEnableAdvanceFeature())
    {
        AdvancedCameraUsecase::Destroy(isForced);
    }

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

    DestroyAllInternalBufferQueues();

    for (UINT i = 0 ; i < MAX_REALTIME_PIPELINE ; i++)
    {
        if (NULL != m_pRTBPreviewStream[i])
        {
            ChxUtils::Free(m_pRTBPreviewStream[i]);
            m_pRTBPreviewStream[i] = NULL;
        }

        if (NULL != m_pRTPreviewStream[i])
        {
            ChxUtils::Free(m_pRTPreviewStream[i]);
            m_pRTPreviewStream[i] = NULL;
        }
        if (NULL != m_pPreviewBufferManager[i])
        {
            m_pPreviewBufferManager[i]->Destroy();
            m_pPreviewBufferManager[i] = NULL;
        }
        if (NULL != m_pRTRDIStream[i])
        {
            ChxUtils::Free(m_pRTRDIStream[i]);
            m_pRTRDIStream[i] = NULL;
        }
        if (NULL != m_pRTRaw16Stream[i])
        {
            ChxUtils::Free(m_pRTRaw16Stream[i]);
            m_pRTRaw16Stream[i] = NULL;
        }
        if (NULL != m_pInputRaw16Stream[i])
        {
            ChxUtils::Free(m_pInputRaw16Stream[i]);
            m_pInputRaw16Stream[i] = NULL;
        }
        if (NULL != m_pRDI16BufferManager[i])
        {
            m_pRDI16BufferManager[i]->Destroy();
            m_pRDI16BufferManager[i] = NULL;
        }
        if (NULL != m_pRTBSnapshotStream[i])
        {
            ChxUtils::Free(m_pRTBSnapshotStream[i]);
            m_pRTBSnapshotStream[i] = NULL;
        }
        if (NULL != m_pRTYUVStream[i])
        {
            ChxUtils::Free(m_pRTYUVStream[i]);
            m_pRTYUVStream[i] = NULL;
        }
        if (NULL != m_pYUVBufferManager[i])
        {
            m_pYUVBufferManager[i]->Destroy();
            m_pYUVBufferManager[i] = NULL;
        }
        if (NULL != m_pJPEGStream[i])
        {
            ChxUtils::Free(m_pJPEGStream[i]);
            m_pJPEGStream[i] = NULL;
        }
        if (NULL != m_pDummyStream[i])
        {
            ChxUtils::Free(m_pDummyStream[i]);
            m_pDummyStream[i] = NULL;
        }
    }

    if (m_pFusionSnapshotBufferManager != NULL)
    {
        m_pFusionSnapshotBufferManager->Destroy();
        m_pFusionSnapshotBufferManager = NULL;
    }

    if (m_pSnapshotFusionStream != NULL)
    {
        ChxUtils::Free(m_pSnapshotFusionStream);
        m_pSnapshotFusionStream = NULL;
    }

    // Free the input metadata for the offline pipeline for preview
    if (NULL != m_pOfflinePipelineInputMetadataPreview)
    {
        free_camera_metadata(m_pOfflinePipelineInputMetadataPreview);
        m_pOfflinePipelineInputMetadataPreview = NULL;
    }
    // Free the input metadata for the offline pipeline for snapshot
    if (NULL != m_pOfflinePipelineInputMetadataSnapshot)
    {
        free_camera_metadata(m_pOfflinePipelineInputMetadataSnapshot);
        m_pOfflinePipelineInputMetadataSnapshot = NULL;
    }
    // Free the input metadata for the offline pipeline for RAW16
    if (NULL != m_pOfflinePipelineInputMetadataRAW16)
    {
        free_camera_metadata(m_pOfflinePipelineInputMetadataRAW16);
        m_pOfflinePipelineInputMetadataRAW16 = NULL;
    }

    for (UINT i = 0; i < MAX_REALTIME_PIPELINE; i++)
    {
        if (m_pStickyMetadata[i])
        {
            free_camera_metadata(m_pStickyMetadata[i]);
            m_pStickyMetadata[i] = NULL;
        }
    }

    MultiCamControllerManager::GetInstance()->DestroyController(m_pMultiCamController);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::processRAW16Frame
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::processRAW16Frame(
    const CHICAPTURERESULT* pResult)
{
    CDKResult result                 = CDKResultSuccess;
    UINT32 pipelineIndex             = pResult->pPrivData->requestIndex;
    UINT32 internalFrameNum          = pResult->frameworkFrameNum;
    UINT32 internalFrameNumIndex     = internalFrameNum %  MaxOutstandingRequests;
    BOOL   hasRAW16Stream            = FALSE;
    ChiStreamBuffer* RAW16Buffer     = NULL;

    if (MAX_REALTIME_PIPELINE <= pipelineIndex)
    {
        CHX_LOG_ERROR("Result: Cannot Accept pipeline index of more than 1 frame = %d", pResult->frameworkFrameNum);
        // Early return
        return CDKResultEFailed;
    }

    for (UINT32 i = 0; i < pResult->numOutputBuffers; i++)
    {
        if (IsRdiRAW16Stream(pResult->pOutputBuffers[i].pStream))
        {
            hasRAW16Stream = TRUE;
            RAW16Buffer    = const_cast<ChiStreamBuffer *>(&pResult->pOutputBuffers[i]);
            m_pRDI16BufferManager[pipelineIndex]->ReleaseReference(
                reinterpret_cast<buffer_handle_t*>(RAW16Buffer->phBuffer));
            break;
        }
    }

    if (NULL != pResult->pResultMetadata)
    {
        // Receive Realtime pipeline result
        if ((REALTIMEPIPELINEMAIN == pipelineIndex) || (REALTIMEPIPELINEAUX == pipelineIndex))
        {
            rtRAW16Buffer[internalFrameNumIndex].pMetadata[pipelineIndex] =
                static_cast<VOID*>(clone_camera_metadata(static_cast<const camera_metadata_t*>(pResult->pResultMetadata)));
            rtRAW16Buffer[internalFrameNumIndex].valid[pipelineIndex]      |= META_READY_FLAG;
        }

        CHX_LOG("RAW16 pipeline idx:%d meta is ready,value:%x", pipelineIndex,
            rtRAW16Buffer[internalFrameNumIndex].valid[pipelineIndex]);
    }

    if ((TRUE == hasRAW16Stream) && (NULL != RAW16Buffer))
    {
        // Receive Realtime pipeline result
        if ((REALTIMEPIPELINEMAIN == pipelineIndex) || (REALTIMEPIPELINEAUX == pipelineIndex))
        {
            rtRAW16Buffer[internalFrameNumIndex].frameNumber = internalFrameNum;

            ChxUtils::Memcpy(&rtRAW16Buffer[internalFrameNumIndex].buffer[pipelineIndex],
                RAW16Buffer,
                sizeof(CHISTREAMBUFFER));
            rtRAW16Buffer[internalFrameNumIndex].buffer[pipelineIndex].pStream = m_pInputRaw16Stream[REALTIMEPIPELINEMAIN];
            rtRAW16Buffer[internalFrameNumIndex].valid[pipelineIndex]         |= BUF_READY_FLAG;;

        }
        CHX_LOG("RAW16 pipeline idx:%d buffer is ready,value:%x", pipelineIndex,
            rtRAW16Buffer[internalFrameNumIndex].valid[pipelineIndex]);
    }

    // trigger reprocessing
    camera3_stream_buffer_t output[2];
    CHICAPTUREREQUEST request   = {0};
    UINT32 buffercount          = 0;
    output[buffercount]         =  m_appRAW16Buffer[internalFrameNumIndex];
    buffercount++;

    request.frameNumber             = rtRAW16Buffer[internalFrameNumIndex].frameNumber;
    request.hPipelineHandle         = reinterpret_cast<CHIPIPELINEHANDLE>(
                                            m_pSession[OFFLINERAW16SESSION]->GetPipelineHandle(0));
    request.numOutputs              = buffercount;
    request.pOutputBuffers          = reinterpret_cast<CHISTREAMBUFFER*>(
                                            output);
    request.pPrivData               = &privRAW16Offline[internalFrameNumIndex];
    request.pPrivData->streamIndex  = OFFLINERAW16PIPELINE;

    multicam_result_metadata_t multiCamResultMetadata;
    multiCamResultMetadata.frameNum            = pResult->frameworkFrameNum;

    ///< Set number of inputs depending on pipeline active or not
    if ((TRUE == rtRequest[internalFrameNumIndex].rawRequest[REALTIMEPIPELINEMAIN]) &&
        (TRUE == rtRequest[internalFrameNumIndex].rawRequest[REALTIMEPIPELINEAUX]))
    {
        ///< if two result are ready, send offline request
        if ((rtRAW16Buffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] & META_READY_FLAG) &&
            (rtRAW16Buffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] & BUF_READY_FLAG) &&
            (rtRAW16Buffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX] & BUF_READY_FLAG) &&
            (rtRAW16Buffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX] & META_READY_FLAG))
        {
            request.numInputs                          = 2;
            request.pInputBuffers                      = rtRAW16Buffer[internalFrameNumIndex].buffer;

            AppendOfflineMetadata(pResult->frameworkFrameNum,
                    rtRAW16Buffer[internalFrameNumIndex].pMetadata[0],
                    rtRAW16Buffer[internalFrameNumIndex].pMetadata[1],
                    m_pOfflinePipelineInputMetadataRAW16,
                    FALSE);

            request.pMetadata                          = m_pOfflinePipelineInputMetadataRAW16;

            CHX_LOG("OfflineRAW16: both buffers for reprocessing frameNum = %" PRIu64 " buffercount %d",
                request.frameNumber, buffercount);

            GenerateInternalRequest(OFFLINERAW16SESSION, 1, &request);

            rtRAW16Buffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] = 0;
            rtRAW16Buffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX]  = 0;

            if (rtRAW16Buffer[internalFrameNumIndex].pMetadata[REALTIMEPIPELINEMAIN] != NULL)
            {
                free_camera_metadata(static_cast<camera_metadata_t*>(
                    rtRAW16Buffer[internalFrameNumIndex].pMetadata[REALTIMEPIPELINEMAIN]));
            }
            if (rtRAW16Buffer[internalFrameNumIndex].pMetadata[REALTIMEPIPELINEAUX] != NULL)
            {
                free_camera_metadata(static_cast<camera_metadata_t*>(
                    rtRAW16Buffer[internalFrameNumIndex].pMetadata[REALTIMEPIPELINEAUX]));
            }
        }
    }
    else if (TRUE == rtRequest[internalFrameNumIndex].rawRequest[pipelineIndex])
    {
        ///< single Pipeline is active
        if ((rtRAW16Buffer[internalFrameNumIndex].valid[pipelineIndex] & META_READY_FLAG)  &&
            (rtRAW16Buffer[internalFrameNumIndex].valid[pipelineIndex] & BUF_READY_FLAG))
        {
            request.numInputs                          = 1;
            request.pInputBuffers                      = &rtRAW16Buffer[internalFrameNumIndex].buffer[pipelineIndex];

            AppendOfflineMetadata(pResult->frameworkFrameNum,
                    rtRAW16Buffer[internalFrameNumIndex].pMetadata[pipelineIndex],
                    NULL,
                    m_pOfflinePipelineInputMetadataRAW16,
                    FALSE);

            request.pMetadata                          = m_pOfflinePipelineInputMetadataRAW16;

            CHX_LOG("OfflineRAW16: single buffer for reprocessing frameNum = %" PRIu64 " buffercount %d",
                request.frameNumber, buffercount);

            GenerateInternalRequest(OFFLINERAW16SESSION, 1, &request);

            rtRAW16Buffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] = 0;
            rtRAW16Buffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX]  = 0;

            if (rtRAW16Buffer[internalFrameNumIndex].pMetadata[pipelineIndex] != NULL)
            {
                free_camera_metadata(static_cast<camera_metadata_t*>(
                    rtRAW16Buffer[internalFrameNumIndex].pMetadata[pipelineIndex]));
            }
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::processRDIFrame
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::processRDIFrame(
    const ChiCaptureResult* pResult)
{
    camera3_stream_buffer_t   output[2];
    CHICAPTUREREQUEST request   = {0};
    UINT32 numOutBuffer         = 0;
    UINT32 pipelineIndex;
    UINT32 sessionIndex;
    BOOL   hasRDIStream             = FALSE;
    BOOL   hasRDIRaw16Stream        = FALSE;
    ChiStreamBuffer* RDIBuffer      = NULL;
    ChiStreamBuffer* RDIRaw16Buffer = NULL;
    UINT32 internalFrameNum         = pResult->frameworkFrameNum;
    UINT32 internalFrameNumIndex    = internalFrameNum % MaxOutstandingRequests;

    if (NULL == pResult->pPrivData)
    {
        CHX_LOG_ERROR("Result: Cannot Accept NULL private data here for = %d", internalFrameNum);
        // Early return
        return;
    }
    else
    {
        pipelineIndex = pResult->pPrivData->streamIndex;
    }

    for (UINT32 i = 0; i < pResult->numOutputBuffers; i++)
    {
        if (IsRdiStream(pResult->pOutputBuffers[i].pStream))
        {
            hasRDIStream = TRUE;
            RDIBuffer    = const_cast<ChiStreamBuffer*>(&pResult->pOutputBuffers[i]);
            UpdateBufferReadyForRDIQueue(RDIBuffer, internalFrameNum, pipelineIndex, TRUE);
            CHX_LOG("[MultiCamera RDI] RDI MIPI RAW 10 found pipelineIndex %d", pipelineIndex);
        }

        if (IsRdiRAW16Stream(pResult->pOutputBuffers[i].pStream))
        {
            hasRDIRaw16Stream = TRUE;
            RDIRaw16Buffer    = const_cast<ChiStreamBuffer *>(&pResult->pOutputBuffers[i]);
            CHX_LOG("[MultiCamera RDI] RDI RAW 16 found pipelineIndex %d", pipelineIndex);
        }
    }

    // RDI(MIPI RAW 10) request for getting offline YUV snapshot
    // when RDI buffer and metadata is ready, it can go on for offline YUV snapshot request.
    if (hasRDIStream && IsRDIBufferMetaReadyForInput(internalFrameNum, pipelineIndex))
    {
        if (pipelineIndex == REALTIMEPIPELINEMAIN)
        {
            sessionIndex = OFFLINERDISESSION;
        }
        else
        {
            sessionIndex = OFFLINERDISESSIONAUX;
        }

        if (TRUE == rtRequest[internalFrameNumIndex].snapRequest[pipelineIndex])
        {
            CHX_LOG("[MultiCamera RDI] snapshot Request : frameNum :%d",
                internalFrameNum);

            if ((NULL == m_requestIDToFrameNumber[internalFrameNumIndex].pFeature)
                && (FALSE == m_requestIDToFrameNumber[internalFrameNumIndex].needTriggerOfflineProcess))
            {
                GenerateSnapshotRequest(internalFrameNum, internalFrameNum, pipelineIndex);
            }
            else if ((m_requestIDToFrameNumber[internalFrameNumIndex].activePipelineID == pipelineIndex)
                && (FALSE == m_requestIDToFrameNumber[internalFrameNumIndex].needTriggerOfflineProcess))
            {
                UINT32 appFrameNum   = m_requestIDToFrameNumber[internalFrameNumIndex].frameNumber;
                UINT32 appFrameIndex = appFrameNum % MaxOutstandingRequests;
                CHX_LOG("appFrameNum:%d, triggerOfflineReprocess[%d]=%d",
                    appFrameNum,
                    internalFrameNumIndex,
                    m_requestIDToFrameNumber[internalFrameNumIndex].triggerOfflineReprocess);

                if ((NULL != m_requestIDToFrameNumber[internalFrameNumIndex].pFeature) &&
                    (TRUE == m_requestIDToFrameNumber[internalFrameNumIndex].triggerOfflineReprocess))
                {
                    CHX_LOG("[MultiCamera RDI]:Send Offline feature(type:%d) request!",
                        m_requestIDToFrameNumber[internalFrameNumIndex].pFeature->GetFeatureType());

                    // no input buffer for snapshot request, feature will get input buffer from RDI queue
                    camera3_capture_request_t requestForFeature = { 0 };
                    requestForFeature.frame_number              = internalFrameNum;
                    requestForFeature.input_buffer              = NULL;
                    requestForFeature.num_output_buffers        = 1;
                    requestForFeature.settings                  = NULL;
                    requestForFeature.output_buffers            = &m_appSnapshotBuffer[internalFrameNumIndex];
                    m_requestIDToFrameNumber[internalFrameNumIndex].pFeature->ExecuteProcessRequest(&requestForFeature);

                    rtRequest[internalFrameNumIndex].snapRequest[pipelineIndex]             = FALSE;
                    m_requestIDToFrameNumber[internalFrameNumIndex].pFeature                = NULL;
                    m_requestIDToFrameNumber[internalFrameNumIndex].triggerOfflineReprocess = FALSE;
                }

            }
        }
    }
    // RDI RAW16 request for getting offline RAW16
    if ((TRUE == hasRDIRaw16Stream) && (NULL != RDIRaw16Buffer))
    {
        processRAW16Frame(pResult);
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::RealtimeCaptureResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::RealtimeCaptureResult(
    ChiCaptureResult* pResult)
{
    UINT32 pipelineIndex;
    UINT32 internalFrameNum      = pResult->frameworkFrameNum;
    UINT32 internalFrameNumIndex = internalFrameNum %  MaxOutstandingRequests;

    if (NULL == pResult->pPrivData)
    {
        CHX_LOG_ERROR("Result: Cannot Accept NULL private data here for = %d", pResult->frameworkFrameNum);
        // Early return
        return;
    }
    else
    {
        pipelineIndex = pResult->pPrivData->streamIndex;
    }

    if ((pipelineIndex == rtRequest[internalFrameNumIndex].masterPipelineIndex) &&
        (NULL != pResult->pResultMetadata))
    {
        AdvancedCameraUsecase::ParseResultMetadata(pResult);
    }

    NotifyResultsAvailable(pResult);

    processRDIFrame(pResult);

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::AppendOfflineMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::AppendOfflineMetadata(
    UINT32 frameNum,
    VOID *pMetadata1,
    VOID *pMetadata2,
    camera_metadata_t *pResultMetadata,
    BOOL isSnapshotMeta)
{
    CDKResult result              = CDKResultSuccess;

    UINT metadataCount = 1;

    multicam_result_metadata_t multiCamResultMetadata;
    multiCamResultMetadata.frameNum         = frameNum;

    if (NULL != pMetadata2)
    {
        metadataCount++;
    }

    multiCamResultMetadata.numResults       = metadataCount;
    multiCamResultMetadata.ppResultMetadata =
        static_cast<VOID**>(CHX_CALLOC(DualCamCount * sizeof(VOID*)));

    if (NULL != multiCamResultMetadata.ppResultMetadata)
    {
        multiCamResultMetadata.ppResultMetadata[0] = pMetadata1;
        multiCamResultMetadata.ppResultMetadata[1] = pMetadata2;
    }
    else
    {
        CHX_LOG_ERROR("CALLOC failed for ppResultMetadata");
        CHX_ASSERT(NULL != multiCamResultMetadata.ppResultMetadata);
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {
        m_pMultiCamController->FillOfflinePipelineInputMetadata(&multiCamResultMetadata,
            pResultMetadata, isSnapshotMeta);

        if (NULL != multiCamResultMetadata.ppResultMetadata)
        {
            CHX_FREE(multiCamResultMetadata.ppResultMetadata);
            multiCamResultMetadata.ppResultMetadata = NULL;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::SubmitJpegRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::SubmitJpegRequest(
    UINT32 frameNum,
    CHISTREAMBUFFER *inputBuffer,
    CHISTREAMBUFFER *JpegBuffer,
    const VOID *pMetadata)
{
    CDKResult result              = CDKResultSuccess;

    UINT32 internalFrameNumIndex  = m_jpegRequestCount %  MaxOutstandingRequests;

    m_jpegRequestId[internalFrameNumIndex].requestId = frameNum;
    m_jpegRequestId[internalFrameNumIndex].stream    = JpegBuffer->pStream;

    JpegBuffer->pStream = m_pTargetSnapshotStream;

    CHICAPTUREREQUEST request               = {0};
    request.frameNumber                     = m_jpegRequestCount;
    request.hPipelineHandle                 = reinterpret_cast<CHIPIPELINEHANDLE>(
            m_pSession[OFFLINERTBJPEGSESSION]->GetPipelineHandle(0));

    request.numOutputs                     = 1;
    request.pOutputBuffers                 = JpegBuffer;
    request.numInputs                      = 1;
    request.pInputBuffers                  = inputBuffer;

    request.pMetadata                      = pMetadata;
    request.pPrivData                      = &privJPEGOffline[internalFrameNumIndex];
    request.pPrivData->streamIndex         = OFFLINEJPEGPIPELINE;

    CHX_LOG("OfflineSnapshot: send JPEG request frame = %d m_jpegRequestCount = %" PRIu64,
        frameNum, m_jpegRequestCount);
    result = GenerateInternalRequest(OFFLINERTBJPEGSESSION, 1, &request);
    m_jpegRequestCount++;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::OfflineSnapshotResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::OfflineSnapshotResult(
    const ChiCaptureResult* pResult)
{
    UINT32 pipelineIndex;
    const CHISTREAMBUFFER* YUVBuffer = NULL;
    UINT32 internalFrameNum    = pResult->frameworkFrameNum;
    UINT32 internalFrameNumIndex  = internalFrameNum %  MaxOutstandingRequests;

    if (NULL == pResult->pPrivData)
    {
        CHX_LOG_ERROR("Result: Cannot Accept NULL private data here for = %d", pResult->frameworkFrameNum);
        // Early return
        return;
    }
    else
    {
        pipelineIndex = pResult->pPrivData->streamIndex;
    }

    for (UINT i = 0; i < pResult->numOutputBuffers; i++)
    {
        if ((TRUE == m_isPostViewNeeded) &&
            (pResult->pOutputBuffers[i].pStream == m_pTargetPostviewStream))
        {
            UINT32 applicationFrameNum             = m_requestIDToFrameNumber[internalFrameNumIndex].frameNumber;
            UINT32 applicationFrameNumIndex        = applicationFrameNum % MaxOutstandingRequests;
            camera3_capture_result_t *pAppResult   = GetCaptureResult(applicationFrameNumIndex);
            pAppResult->frame_number               = applicationFrameNum;

            m_pAppResultMutex->Lock();
            camera3_stream_buffer_t* pResultBuffer =
                    const_cast<camera3_stream_buffer_t*>(&pAppResult->output_buffers[pAppResult->num_output_buffers++]);

            ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[i], sizeof(camera3_stream_buffer_t));
            m_pAppResultMutex->Unlock();

            ProcessAndReturnFinishedResults();
            CHX_LOG("Thumbnail Callback: Sent to application i =%d", i);
        }
        else
        {
           YUVBuffer = const_cast <CHISTREAMBUFFER *>(
                (CHISTREAMBUFFER *)&pResult->pOutputBuffers[i]);
        }
    }

    if ((REALTIMEPIPELINEMAIN == pipelineIndex) || (REALTIMEPIPELINEAUX == pipelineIndex))
    {
        if (NULL != pResult->pResultMetadata)
        {
            rtSnapshotBuffer[internalFrameNumIndex].valid[pipelineIndex]  |= META_READY_FLAG;
            CHX_LOG("OfflineSnapshotResult pipeline idx:%d meta is ready,value:%x",
                pipelineIndex, rtSnapshotBuffer[internalFrameNumIndex].valid[pipelineIndex]);
            rtSnapshotBuffer[internalFrameNumIndex].pMetadata[pipelineIndex] = const_cast<VOID*>(pResult->pResultMetadata);
        }
    }

    ///< Receive Realtime pipeline result
    if ((NULL != YUVBuffer) && ((REALTIMEPIPELINEMAIN == pipelineIndex) || (REALTIMEPIPELINEAUX == pipelineIndex)))
    {
        rtSnapshotBuffer[internalFrameNumIndex].frameNumber = internalFrameNum;

        if (pResult->numOutputBuffers > 0)
        {
            // Release reference to the input RDI buffers - regular snapshot without mfnr case
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

            m_pYUVBufferManager[pipelineIndex]->ReleaseReference(
                reinterpret_cast<buffer_handle_t*>(YUVBuffer->phBuffer));
            ChxUtils::Memcpy(&rtSnapshotBuffer[internalFrameNumIndex].buffer[pipelineIndex],
                 const_cast<CHISTREAMBUFFER*>(YUVBuffer),
                 sizeof(CHISTREAMBUFFER));
            rtSnapshotBuffer[internalFrameNumIndex].buffer[pipelineIndex].pStream = m_pJPEGStream[pipelineIndex];
            rtSnapshotBuffer[internalFrameNumIndex].valid[pipelineIndex]         |= BUF_READY_FLAG;;
            CHX_LOG("OfflineSnapshotResult pipeline idx:%d buffer is ready,stream:%p,valid:%x", pipelineIndex,
                rtSnapshotBuffer[internalFrameNumIndex].buffer[pipelineIndex].pStream,
                rtSnapshotBuffer[internalFrameNumIndex].valid[pipelineIndex]);
        }
    }

    ///< Set number of inputs depending on pipeline active or not
    if ((TRUE == rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEMAIN]) &&
        (TRUE == rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEAUX]))
    {
        if ((m_bokehWideCapture) &&
            (rtSnapshotBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] & META_READY_FLAG) &&
            (rtSnapshotBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] & BUF_READY_FLAG) &&
            (NULL != m_appWideJpegBuffer[internalFrameNumIndex].buffer))
        {
            CHISTREAMBUFFER jpegBuffer;

            ChxUtils::Memcpy(&jpegBuffer, &m_appWideJpegBuffer[internalFrameNumIndex],
                    sizeof(CHISTREAMBUFFER));
            jpegBuffer.acquireFence = -1;
            jpegBuffer.pStream      = m_pTargetWideJpegStream;

            AppendOfflineMetadata(internalFrameNum,
                        rtSnapshotBuffer[internalFrameNumIndex].pMetadata[REALTIMEPIPELINEMAIN],
                        rtSnapshotBuffer[internalFrameNumIndex].pMetadata[REALTIMEPIPELINEAUX],
                        m_pOfflinePipelineInputMetadataSnapshot,
                        TRUE);

            SubmitJpegRequest(pResult->frameworkFrameNum,
                    &rtSnapshotBuffer[internalFrameNumIndex].buffer[REALTIMEPIPELINEMAIN],
                    &jpegBuffer,
                    m_pOfflinePipelineInputMetadataSnapshot);
        }

        ///< if two result are ready, send offline request
        if ((rtSnapshotBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] & META_READY_FLAG) &&
            (rtSnapshotBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] & BUF_READY_FLAG) &&
            (rtSnapshotBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX] & META_READY_FLAG) &&
            (rtSnapshotBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX] & BUF_READY_FLAG))
        {
            rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEMAIN] = FALSE;
            rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEAUX]  = FALSE;

            // trigger reprocessing
            CHICAPTUREREQUEST request               = {0};
            request.frameNumber                     = rtSnapshotBuffer[internalFrameNumIndex].frameNumber;
            request.hPipelineHandle                 = reinterpret_cast<CHIPIPELINEHANDLE>(
                m_pSession[OFFLINEFUSIONSNAPSHOTSESSION]->GetPipelineHandle(0));

            request.numOutputs                      = 1;
            // Get buffer to hold fusion image
            camera3_stream_buffer_t   outputBuffer;
            outputBuffer.acquire_fence              = -1;
            outputBuffer.buffer                     = m_pFusionSnapshotBufferManager->GetImageBuffer()->GetBufferHandle();
            outputBuffer.stream                     = reinterpret_cast<camera3_stream_t*>(m_pSnapshotFusionStream);
            request.pOutputBuffers                  = reinterpret_cast<CHISTREAMBUFFER*>(&outputBuffer);

            request.numInputs                       = 2;
            request.pInputBuffers                   = rtSnapshotBuffer[internalFrameNumIndex].buffer;

            AppendOfflineMetadata(internalFrameNum,
                    rtSnapshotBuffer[internalFrameNumIndex].pMetadata[REALTIMEPIPELINEMAIN],
                    rtSnapshotBuffer[internalFrameNumIndex].pMetadata[REALTIMEPIPELINEAUX],
                    m_pOfflinePipelineInputMetadataSnapshot,
                    TRUE);

            request.pMetadata                      = m_pOfflinePipelineInputMetadataSnapshot;
            request.pPrivData                      = &privSnapshotOffline[internalFrameNumIndex];
            request.pPrivData->streamIndex         = OFFLINEFUSIONPIPELINE;

            CHX_LOG("OfflineSnapshot: send capture fusion request in dual zone");
            GenerateInternalRequest(OFFLINEFUSIONSNAPSHOTSESSION, 1, &request);

            rtSnapshotBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] = 0;
            rtSnapshotBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX]  = 0;
        }
    }
    else if (TRUE == rtRequest[internalFrameNumIndex].snapRequest[pipelineIndex])
    {
        ///< single Pipeline is active, directly send to JPEG session
        if ((rtSnapshotBuffer[internalFrameNumIndex].valid[pipelineIndex] & META_READY_FLAG) &&
            (rtSnapshotBuffer[internalFrameNumIndex].valid[pipelineIndex] & BUF_READY_FLAG))
        {
            rtRequest[internalFrameNumIndex].snapRequest[pipelineIndex] = FALSE;

            AppendOfflineMetadata(internalFrameNum,
                    rtSnapshotBuffer[internalFrameNumIndex].pMetadata[pipelineIndex],
                    NULL,
                    m_pOfflinePipelineInputMetadataSnapshot,
                    TRUE);

            SubmitJpegRequest(rtSnapshotBuffer[internalFrameNumIndex].frameNumber,
                    &rtSnapshotBuffer[internalFrameNumIndex].buffer[pipelineIndex],
                    reinterpret_cast<CHISTREAMBUFFER*>(&m_appSnapshotBuffer[internalFrameNumIndex]),
                    m_pOfflinePipelineInputMetadataSnapshot);

            rtSnapshotBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] = 0;
            rtSnapshotBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX]  = 0;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::OfflineFusionSnapshotResult
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::OfflineFusionSnapshotResult(
    const ChiCaptureResult* pResult)
{
    UINT32 internalFrameNum    = pResult->frameworkFrameNum;
    UINT32 internalFrameNumIndex  = internalFrameNum % MaxOutstandingRequests;

    if (NULL != pResult->pResultMetadata)
    {
        m_FusionResultBuffer[internalFrameNumIndex].pMetadata[0] = const_cast<VOID*>(pResult->pResultMetadata);
        m_FusionResultBuffer[internalFrameNumIndex].valid[0] |= META_READY_FLAG;
    }

    if (0 < pResult->numOutputBuffers)
    {
        ChxUtils::Memcpy(&(m_FusionResultBuffer[internalFrameNumIndex].buffer[0]),
            pResult->pOutputBuffers, sizeof(CHISTREAMBUFFER));
        m_FusionResultBuffer[internalFrameNumIndex].buffer[0].pStream = m_pSnapshotFusionStream;
        m_FusionResultBuffer[internalFrameNumIndex].valid[0] |= BUF_READY_FLAG;
    }

    if ((m_FusionResultBuffer[internalFrameNumIndex].valid[0] & BUF_READY_FLAG) &&
        (m_FusionResultBuffer[internalFrameNumIndex].valid[0] & META_READY_FLAG))
    {
        SubmitJpegRequest(pResult->frameworkFrameNum, &(m_FusionResultBuffer[internalFrameNumIndex].buffer[0]),
             reinterpret_cast<CHISTREAMBUFFER*>(&m_appSnapshotBuffer[internalFrameNumIndex]),
             m_FusionResultBuffer[internalFrameNumIndex].pMetadata[0]);

        m_pFusionSnapshotBufferManager->ReleaseReference(
            reinterpret_cast<buffer_handle_t*>(m_FusionResultBuffer[internalFrameNumIndex].buffer[0].phBuffer));

        m_FusionResultBuffer[internalFrameNumIndex].valid[0] = FALSE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::OfflineRTBJPEGResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::OfflineRTBJPEGResult(
    ChiCaptureResult* pResult)
{
    UINT32 pipelineIndex;
    UINT32 internalFrameNum          = m_jpegRequestId[pResult->frameworkFrameNum%MaxOutstandingRequests].requestId;
    UINT32 internalFrameNumIndex     = internalFrameNum %  MaxOutstandingRequests;
    UINT32 applicationFrameNum       = m_requestIDToFrameNumber[internalFrameNumIndex].frameNumber;
    UINT32 applicationFrameNumIndex  = applicationFrameNum % MaxOutstandingRequests;
    if (NULL == pResult->pPrivData)
    {
        CHX_LOG_ERROR("Result: Cannot Accept NULL private data here for = %d", pResult->frameworkFrameNum);
        // Early return
        return;
    }
    else
    {
        pipelineIndex = pResult->pPrivData->streamIndex;
    }
    camera3_capture_result_t *pAppResult   = GetCaptureResult(applicationFrameNumIndex);

    if ((NULL != pResult->pResultMetadata) && (TRUE == m_requestIDToFrameNumber[internalFrameNumIndex].isMetaReturnNeeded))
    {
        pAppResult->frame_number   = applicationFrameNum;
        pAppResult->partial_result = pResult->numPartialMetadata;
        pAppResult->result         = const_cast<camera_metadata_t *>(static_cast<const camera_metadata_t *>(pResult->pResultMetadata));

        {
            UINT32                  SensorTimestampTag = 0x000E0010;
            camera_metadata_entry_t entry              = { 0 };
            camera_metadata_t* pMetadata               =
                        const_cast<camera_metadata_t*>(static_cast<const camera_metadata_t*>(pResult->pResultMetadata));
            UINT64             timestamp = m_shutterTimestamp[applicationFrameNumIndex];

            if (0 != timestamp)
            {
                INT32 status = find_camera_metadata_entry(pMetadata, SensorTimestampTag, &entry);

                if (-ENOENT == status)
                {
                    status = add_camera_metadata_entry(pMetadata, SensorTimestampTag, &timestamp, 1);
                }
                else if (0 == status)
                {
                    status = update_camera_metadata_entry(pMetadata, entry.index, &timestamp, 1, NULL);
                }
                SetMetadataAvailable(applicationFrameNumIndex);
                ProcessAndReturnFinishedResults();
            }
            else
            {
                CHX_LOG("Warning: timestamp is 0!");
            }
        }
    }

    if (pResult->numOutputBuffers != 0)
    {
        pAppResult->frame_number               = applicationFrameNum;
        camera3_stream_buffer_t* pResultBuffer = NULL;
        CHISTREAMBUFFER *outputBuffer          = NULL;

        m_pAppResultMutex->Lock();
        for (UINT32 j = 0; j < pResult->numOutputBuffers; j++)
        {
            pResultBuffer         =
                const_cast<camera3_stream_buffer_t*>(&pAppResult->output_buffers[pAppResult->num_output_buffers++]);

            outputBuffer          = const_cast<CHISTREAMBUFFER *>(&pResult->pOutputBuffers[j]);
            outputBuffer->pStream = m_jpegRequestId[pResult->frameworkFrameNum%MaxOutstandingRequests].stream;

            ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[j], sizeof(camera3_stream_buffer_t));
        }
        m_pAppResultMutex->Unlock();

        ProcessAndReturnFinishedResults();
        CHX_LOG("Snapshot Callback: Send to application:%d",internalFrameNum);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::OfflinePreviewResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::OfflinePreviewResult(
    ChiCaptureResult* pResult)
{
    UINT32 pipelineIndex;
    UINT32 internalFrameNum          = pResult->frameworkFrameNum;
    UINT32 internalFrameNumIndex     = internalFrameNum %  MaxOutstandingRequests;
    UINT32 applicationFrameNum       = m_requestIDToFrameNumber[internalFrameNumIndex].frameNumber;
    UINT32 applicationFrameNumIndex  = applicationFrameNum % MaxOutstandingRequests;

    if (NULL == pResult->pPrivData)
    {
        CHX_LOG_ERROR("Result: Cannot Accept NULL private data here for = %d", pResult->frameworkFrameNum);
        // Early return
        return;
    }
    else
    {
        pipelineIndex = pResult->pPrivData->streamIndex;
    }
    camera3_capture_result_t *pAppResult   = GetCaptureResult(applicationFrameNumIndex);

    if (pResult->pResultMetadata)
    {
        if ((TRUE == m_requestIDToFrameNumber[internalFrameNumIndex].isMetaReturnNeeded) &&
            (FALSE == m_requestIDToFrameNumber[internalFrameNumIndex].isSnapshotReturnNeeded))
        {
            pAppResult->frame_number   = applicationFrameNum;
            pAppResult->partial_result = pResult->numPartialMetadata;
            pAppResult->result         = const_cast<camera_metadata_t *>(static_cast<const camera_metadata_t *>(pResult->pResultMetadata));

            {
                UINT32                  SensorTimestampTag = 0x000E0010;
                camera_metadata_entry_t entry              = { 0 };
                camera_metadata_t* pMetadata                 =
                            const_cast<camera_metadata_t*>(static_cast<const camera_metadata_t*>(pResult->pResultMetadata));
                UINT64             timestamp = m_shutterTimestamp[applicationFrameNum % MaxOutstandingRequests];
                INT32 status = find_camera_metadata_entry(pMetadata, SensorTimestampTag, &entry);

                if (-ENOENT == status)
                {
                    status = add_camera_metadata_entry(pMetadata, SensorTimestampTag, &timestamp, 1);
                }
                else if (0 == status)
                {
                    status = update_camera_metadata_entry(pMetadata, entry.index, &timestamp, 1, NULL);
                }
            }

            SetMetadataAvailable(applicationFrameNumIndex);
            ProcessAndReturnFinishedResults();
        }

        // Process the result metadata with controller
        m_pMultiCamController->ProcessResultMetadata(const_cast<VOID*>(pResult->pResultMetadata));
    }

    if (pResult->numOutputBuffers != 0)
    {
        pAppResult->frame_number               = applicationFrameNum;
        camera3_stream_buffer_t* pResultBuffer = NULL;

        m_pAppResultMutex->Lock();
        for (UINT32 j = 0; j < pResult->numOutputBuffers; j++)
        {
            pResultBuffer =
                const_cast<camera3_stream_buffer_t*>(&pAppResult->output_buffers[pAppResult->num_output_buffers++]);

            ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[j], sizeof(camera3_stream_buffer_t));

            if ((m_pTargetPreviewStream == reinterpret_cast<CHISTREAM*>(pResultBuffer->stream)) &&
                (TRUE == m_requestIDToFrameNumber[internalFrameNumIndex].isSkipPreview))
            {
                m_requestIDToFrameNumber[internalFrameNumIndex].isSkipPreview = FALSE;

                CHX_LOG("skip the frame for display! internalFrameNum:%d, AppFrameNum:%d",
                    internalFrameNum, applicationFrameNum);
                ChxUtils::SkipPreviewFrame(pResultBuffer);
            }
        }
        m_pAppResultMutex->Unlock();

        ProcessAndReturnFinishedResults();

        //after first frame is coming, we can start defer thread to do other resource finalize
        if ((internalFrameNum == 1) && (FALSE == m_deferSnapshotSessionDone) &&
            (NULL == m_deferSnapSessionThreadData.pPrivateData))
        {
            StartDeferThread();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::OfflineRAW16Result
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::OfflineRAW16Result(
    ChiCaptureResult* pResult)
{
    UINT32 internalFrameNum          = pResult->frameworkFrameNum;
    UINT32 internalFrameNumIndex     = internalFrameNum %  MaxOutstandingRequests;
    UINT32 applicationFrameNum       = m_requestIDToFrameNumber[internalFrameNumIndex].frameNumber;
    UINT32 applicationFrameNumIndex  = applicationFrameNum % MaxOutstandingRequests;
    if (NULL == pResult->pPrivData)
    {
        CHX_LOG_ERROR("Result: Cannot Accept NULL private data here for = %d", pResult->frameworkFrameNum);
        // Early return
        return;
    }

    if ((pResult->numOutputBuffers != 0) && (m_pTargetRAW16Stream == pResult->pOutputBuffers[0].pStream))
    {
        CHX_LOG("RAW16 callback send to application FrameNum %d", applicationFrameNum);
        camera3_capture_result_t *pAppResult   = GetCaptureResult(applicationFrameNumIndex);
        pAppResult->frame_number               = applicationFrameNum;
        camera3_stream_buffer_t* pResultBuffer = NULL;

        m_pAppResultMutex->Lock();
        for (UINT32 j = 0; j < pResult->numOutputBuffers; j++)
        {
            pResultBuffer =
                const_cast<camera3_stream_buffer_t*>(&pAppResult->output_buffers[pAppResult->num_output_buffers++]);

            ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[j], sizeof(camera3_stream_buffer_t));
        }
        m_pAppResultMutex->Unlock();

        ProcessAndReturnFinishedResults();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::ProcessNotifyMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::ProcessRealTimeNotifyMessage(
    const ChiMessageDescriptor* pMessageDescriptor)
{
    UINT32 pipelineIndex         = pMessageDescriptor->pPrivData->requestIndex;
    UINT32 internalFrameNum      = pMessageDescriptor->message.shutterMessage.frameworkFrameNum;
    UINT32 internalFrameNumIndex = internalFrameNum %  MaxOutstandingRequests;

    // Real time shutter notification to framework based on master pipeline index
    if (pipelineIndex == rtRequest[internalFrameNumIndex].masterPipelineIndex &&
        (TRUE == m_requestIDToFrameNumber[internalFrameNumIndex].isShutterReturnNeeded))
    {
        m_requestIDToFrameNumber[internalFrameNumIndex].isShutterReturnNeeded = FALSE;
        UINT32 applicationFrameNum      = m_requestIDToFrameNumber[internalFrameNumIndex].frameNumber;
        UINT32 applicationFrameNumIndex = applicationFrameNum % MaxOutstandingRequests;
        ChiMessageDescriptor* pTempMessage = const_cast<ChiMessageDescriptor*>(pMessageDescriptor);
        pTempMessage->message.shutterMessage.frameworkFrameNum = applicationFrameNum;

        camera3_notify_msg_t message;
        message.type                         = pMessageDescriptor->messageType;
        message.message.shutter.frame_number = applicationFrameNum;
        message.message.shutter.timestamp    = pMessageDescriptor->message.shutterMessage.timestamp;

        m_shutterTimestamp[applicationFrameNumIndex] = pMessageDescriptor->message.shutterMessage.timestamp;
        camera3_capture_result_t *pAppResult   = GetCaptureResult(applicationFrameNumIndex);
        if ((FALSE == IsMetadataSent(applicationFrameNumIndex)) &&
            (NULL != pAppResult->result))
        {
            UINT32                  SensorTimestampTag = 0x000E0010;
            camera_metadata_entry_t entry              = { 0 };

            camera_metadata_t* pMetadata = const_cast<camera_metadata_t*>(pAppResult->result);
            UINT64             timestamp = m_shutterTimestamp[applicationFrameNumIndex];

            INT32 status = find_camera_metadata_entry(pMetadata, SensorTimestampTag, &entry);

            if (-ENOENT == status)
            {
                status = add_camera_metadata_entry(pMetadata, SensorTimestampTag, &timestamp, 1);
            }
            else if (0 == status)
            {
                status = update_camera_metadata_entry(pMetadata, entry.index, &timestamp, 1, NULL);
            }
            SetMetadataAvailable(applicationFrameNumIndex);
            ProcessAndReturnFinishedResults();
        }

        Usecase::ReturnFrameworkMessage((camera3_notify_msg_t*)pMessageDescriptor, m_cameraId);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::SessionCbNotifyMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::SessionCbNotifyMessage(
    const ChiMessageDescriptor* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{
    SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    UsecaseMultiCamera* pUsecase            = static_cast<UsecaseMultiCamera*>(pSessionPrivateData->pUsecase);

    if ((NULL == pMessageDescriptor) ||
        (ChiMessageTypeSof == pMessageDescriptor->messageType) ||
        (NULL == pMessageDescriptor->pPrivData))
    {
        // Sof notifications are not sent to the HAL3 application and so ignore
        if ((NULL == pMessageDescriptor) ||
            ((ChiMessageTypeSof != pMessageDescriptor->messageType) &&
             (NULL == pMessageDescriptor->pPrivData)))
        {
            CHX_LOG_ERROR("Result: Cannot Accept NULL private data for Notify");
        }
        else if (ChiMessageTypeSof == pMessageDescriptor->messageType)
        {
            CHX_LOG("Chi Notify SOF frameNum %u framework frameNum %u, timestamp %" PRIu64,
                pMessageDescriptor->message.sofMessage.sofId,
                pMessageDescriptor->message.sofMessage.frameworkFrameNum,
                pMessageDescriptor->message.sofMessage.timestamp);
        }

        return;
    }

    UINT32 internalFrameNum      = pMessageDescriptor->message.shutterMessage.frameworkFrameNum;
    UINT32 internalFrameNumIndex = internalFrameNum %  MaxOutstandingRequests;

    CHX_LOG("Notify Session: %d Pipeline: %d frameNum: %d Type: %d Timestamp: %" PRIu64,
        pSessionPrivateData->sessionID,
        pMessageDescriptor->pPrivData->streamIndex,
        internalFrameNum,
        pMessageDescriptor->messageType, pMessageDescriptor->message.shutterMessage.timestamp);

    if (pSessionPrivateData->sessionID == REALTIMESESSIONIDX)
    {
        pUsecase->ProcessRealTimeNotifyMessage(pMessageDescriptor);
    }
    else
    {
        //  Send only the realtime pipeline 0 shutter message
        if ((pSessionPrivateData->sessionID == OFFLINESESSIONIDX) ||
            (pSessionPrivateData->sessionID == OFFLINERTBJPEGSESSION))
        {
            pUsecase->m_shutterFrameNum = internalFrameNum;
        }
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::SessionCbCaptureResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::SessionCbCaptureResult(
    ChiCaptureResult* pResult,
    VOID*             pPrivateCallbackData)
{
    SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    UsecaseMultiCamera* pUsecase            = static_cast<UsecaseMultiCamera*>(pSessionPrivateData->pUsecase);

    UINT32 pipelineIndex;

    if(TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aFlushInProgress)))
    {
        //Don't process flushed results.
        return;
    }

    if (NULL == pResult->pPrivData)
    {
        CHX_LOG_ERROR("Result: Cannot Accept NULL private data here for = %d", pResult->frameworkFrameNum);
        // Early return
        return;
    }
    else
    {
        pipelineIndex = pResult->pPrivData->streamIndex;
    }

    CHX_LOG("Result: Session = %d Pipeline = %d FrameNum: %d buffer - %p, status - %d numbuf = %d partial = %d input = %p",
            pSessionPrivateData->sessionID, pipelineIndex, pResult->frameworkFrameNum, pResult->pOutputBuffers[0].phBuffer,
            pResult->pOutputBuffers[0].bufferStatus, pResult->numOutputBuffers,
            pResult->numPartialMetadata, pResult->pInputBuffer);

    if (REALTIMESESSIONIDX == pSessionPrivateData->sessionID)
    {
        pUsecase->RealtimeCaptureResult(pResult);
    }
    else if (OFFLINESESSIONIDX == pSessionPrivateData->sessionID)
    {
        pUsecase->OfflinePreviewResult(pResult);
    }
    else if (OFFLINERDISESSION == pSessionPrivateData->sessionID ||
             OFFLINERDISESSIONAUX == pSessionPrivateData->sessionID)
    {
        pUsecase->OfflineSnapshotResult(pResult);
    }
    else if (OFFLINEFUSIONSNAPSHOTSESSION == pSessionPrivateData->sessionID)
    {
        pUsecase->OfflineFusionSnapshotResult(pResult);
    }
    else if (OFFLINERTBJPEGSESSION == pSessionPrivateData->sessionID)
    {
        pUsecase->OfflineRTBJPEGResult(pResult);
    }
    else if (OFFLINERAW16SESSION == pSessionPrivateData->sessionID)
    {
        pUsecase->OfflineRAW16Result(pResult);
    }
    else
    {
        CHX_LOG_ERROR("Unknown session ID %d ", pSessionPrivateData->sessionID);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::DeferSnapSessionThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* UsecaseMultiCamera::DeferSnapSessionThread(VOID * pThreadData)
{
    PerThreadData*      pPerThreadData      = reinterpret_cast<PerThreadData*>(pThreadData);
    UsecaseMultiCamera* pDualCamZoomUsecase = reinterpret_cast<UsecaseMultiCamera*>(pPerThreadData->pPrivateData);

    pDualCamZoomUsecase->DeferSnapSession();

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::StartDeferThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::StartDeferThread()
{
    CDKResult result = CDKResultSuccess;
    m_pDeferSnapDoneMutex->Lock();
    m_deferSnapshotThreadCreateDone = FALSE;
    m_pDeferSnapDoneMutex->Unlock();

    /// it need to check if defer snapshot session is created done.
    if ((FALSE == m_deferSnapshotSessionDone) &&
            (NULL == m_deferSnapSessionThreadData.pPrivateData))
    {
        m_deferSnapSessionThreadData.pPrivateData = this;

        result = ChxUtils::ThreadCreate(UsecaseMultiCamera::DeferSnapSessionThread, &m_deferSnapSessionThreadData,
            &m_deferSnapSessionThreadData.hThreadHandle);

        if (CDKResultSuccess != result)
        {
            m_deferSnapSessionThreadData.pPrivateData  = NULL;
            m_deferSnapSessionThreadData.hThreadHandle = INVALID_THREAD_HANDLE;
            CHX_LOG_ERROR("Create defer thread failed!");
        }
        else
        {
            m_pDeferSnapDoneMutex->Lock();
            m_deferSnapshotThreadCreateDone = TRUE;
            m_pDeferSnapDoneMutex->Unlock();
        }
    }
    else
    {
        CHX_LOG_WARN("Warning:snapshot related session has been created!");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::InitializeDeferResouce
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::InitializeDeferResource()
{
    m_pDeferSnapDoneMutex      = Mutex::Create();
    m_pDeferSnapDoneCondition  = Condition::Create();
    m_pDeferSyncMutex          = Mutex::Create();
    m_deferSnapshotSessionDone = FALSE;
    m_deferSnapshotThreadCreateDone = FALSE;
    m_deferedCameraID          = INVALID_CAMERAID;
    ChxUtils::Memset(&m_deferSnapSessionThreadData, 0, sizeof(m_deferSnapSessionThreadData));
    m_deferSnapSessionThreadData.hThreadHandle = INVALID_THREAD_HANDLE;

    for (UINT i = 0; i < MAX_DUAL_CAMERA_SESSION; i++)
    {
        m_pSession[i] = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::DestroyDeferResource
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::DestroyDeferResource()
{
    CDKResult result = CDKResultSuccess;
    result = WaitForDeferSnapThread();

    // terminate snapshot deferred thread.
    if ((INVALID_THREAD_HANDLE != m_deferSnapSessionThreadData.hThreadHandle) &&
        (TRUE == m_deferSnapshotThreadCreateDone) &&
        (CDKResultSuccess == result ))
    {
        ChxUtils::ThreadTerminate(m_deferSnapSessionThreadData.hThreadHandle);
    }

    if (NULL != m_pDeferSnapDoneMutex)
    {
        m_pDeferSnapDoneMutex->Destroy();
        m_pDeferSnapDoneMutex = NULL;
    }

    if (NULL != m_pDeferSnapDoneCondition)
    {
        m_pDeferSnapDoneCondition->Destroy();
        m_pDeferSnapDoneCondition = NULL;
    }

    if (NULL != m_pDeferSyncMutex)
    {
        m_pDeferSyncMutex->Destroy();
        m_pDeferSyncMutex = NULL;
    }

    m_deferSnapshotSessionDone      = FALSE;
    m_deferSnapshotThreadCreateDone = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::WaitForDeferThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::WaitForDeferSnapThread()
{
    CDKResult result = CDKResultSuccess;

    if(TRUE == m_deferSnapshotThreadCreateDone)
    {
        m_pDeferSnapDoneMutex->Lock();
        if ((FALSE == m_deferSnapshotSessionDone))
        {
            result = m_pDeferSnapDoneCondition->TimedWait(m_pDeferSnapDoneMutex->GetNativeHandle(), 1000);
            if(CDKResultSuccess != result)
            {
                CHX_LOG_ERROR("WaitForDeferSnapThread timed out!");
            }
        }
        m_pDeferSnapDoneMutex->Unlock();
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::DeferSnapSession
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::DeferSnapSession()
{
    ATRACE_BEGIN("DeferSnapSession");
    CDKResult result = CDKResultSuccess;
    CHX_LOG_INFO("DeferSnapSession:enter");

    // Create offline related session
    result = CreateOfflineSession();

    if (CDKResultSuccess == result)
    {
        // Allocate buffer for snapshot related session
        CHAR bufferManagerName[MaxStringLength64];
        for (UINT i = 0; i < MAX_REALTIME_PIPELINE; i++)
        {
            CdkUtils::SNPrintF(bufferManagerName, sizeof(bufferManagerName), "MCYUVBufferManager_%d", i);
            if (i == REALTIMEPIPELINEMAIN)
            {
                m_pYUVBufferManager[i]               =
                    CHIBufferManager::Create("MCYUVBufferManager_MAIN", &m_yuvSnapBufferCreateData[i]);
            }
            else
            {
                m_pYUVBufferManager[i]               =
                    CHIBufferManager::Create("MCYUVBufferManager_AUX", &m_yuvSnapBufferCreateData[i]);
            }
            // Allocate RAW16 buffers for SAT and RAW16 is needed
            if ((UsecaseSAT == m_dualCameraUsecase) && (TRUE == m_isRaw16Needed))
            {
                CdkUtils::SNPrintF(bufferManagerName, sizeof(bufferManagerName), "MCRDI16BufferManager_%d", i);
                m_pRDI16BufferManager[i]         = CHIBufferManager::Create(bufferManagerName, &m_rdi16BufferCreateData[i]);
            }
        }

        m_pFusionSnapshotBufferManager = CHIBufferManager::Create("MCFusionBufferManager", &m_fusionBufferCreateData);
        m_deferSnapshotSessionDone = TRUE;
        m_pDeferSnapDoneMutex->Unlock();
        m_pDeferSnapDoneCondition->Signal();
    }
    else
    {
        m_pDeferSnapDoneMutex->Unlock();
        m_pDeferSnapDoneCondition->Signal();
        m_deferSnapshotSessionDone = FALSE;
        CHX_LOG_ERROR("Create offline pipeline failed!");
    }

    ATRACE_END();
    CHX_LOG_INFO("DeferSnapSession:done");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::ActivateDeactivateRealtimePipeline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::ActivateDeactivateRealtimePipeline(
    active_cameras_t* activeCameras,
    UINT64            requestId)
{
    CDKResult result = CDKResultSuccess;
    for (UINT32 i = 0; i < MAX_REALTIME_PIPELINE; i++)
    {
        if ((TRUE == activeCameras[i].isActive) && (FALSE == m_pSession[REALTIMESESSIONIDX]->IsPipelineActive(i)))
        {
            // Activate pipeline if LPM decision is to activate by checking the current state
            CHX_LOG_INFO("Activating Pipeline %d", i);
            ATRACE_BEGIN("ActivatePipeline");
            result = ExtensionModule::GetInstance()->ActivatePipeline(m_pSession[REALTIMESESSIONIDX]->GetSessionHandle(),
                m_pSession[REALTIMESESSIONIDX]->GetPipelineHandle(i));
            ATRACE_END();
            if (CDKResultSuccess == result)
            {
                CHX_LOG_INFO("Success activating for pipeline %d", i);
                m_pSession[REALTIMESESSIONIDX]->SetPipelineActivateFlag(i);
                m_stickyMetaNeeded[i] = TRUE;
            }

        }
        else if ((FALSE == activeCameras[i].isActive) && (TRUE == m_pSession[REALTIMESESSIONIDX]->IsPipelineActive(i)))
        {
            // If pipeline is active and LPM says to deactivate then wait for all the inflight requests to finish and then
            // deactivate
            if (m_pendingRequest[i].requestId == 0)
            {
                m_pendingRequest[i].pipelineIndex = i;
                m_pendingRequest[i].requestId = requestId - 1;
            }
            else if (TRUE == m_pendingRequest[i].isReturnedResult)
            {
                CHX_LOG_INFO("Deactivating Pipeline %d", i);
                result = ExtensionModule::GetInstance()->DeactivatePipeline(m_pSession[REALTIMESESSIONIDX]->GetSessionHandle(),
                    m_pSession[REALTIMESESSIONIDX]->GetPipelineHandle(i),
                    CHIDeactivateModeSensorStandby);
                if (CDKResultSuccess == result)
                {
                    CHX_LOG_INFO("Deactivating success Pipeline %d", i);
                    m_pSession[REALTIMESESSIONIDX]->SetPipelineDeactivate(i);
                }

                m_pendingRequest[i].requestId = 0;
                m_pendingRequest[i].isReturnedResult = FALSE;
            }
        }

        // Memset the m_pendingRequest info if state is active and previous deactivate is under process
        if ((TRUE == activeCameras[i].isActive) && (0 != m_pendingRequest[i].requestId))
        {
            ChxUtils::Memset(&m_pendingRequest[i], 0, sizeof(PendingRequestInfo));
        }
    }

    if (NULL == m_pSession[OFFLINESESSIONIDX])
    {
        m_perSessionPvtData[OFFLINESESSIONIDX].sessionID = OFFLINESESSIONIDX;
        m_perSessionPvtData[OFFLINESESSIONIDX].pUsecase = this;
        ChiCallBacks callbacks = {
            .ChiNotify = SessionCbNotifyMessage,
            .ChiProcessCaptureResult = SessionCbCaptureResult
        };
        m_pSession[OFFLINESESSIONIDX] = Session::Create(&m_pPipeline[OFFLINEPREVIEWPIPELINE], 1,
            &callbacks, &m_perSessionPvtData[OFFLINESESSIONIDX]);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::CreateMultiController(
        LogicalCameraInfo*              pCameraInfo,             ///< Camera Info
        camera3_stream_configuration_t* pStreamConfig,          ///< Stream configuration
        DualCameraUsecase dualCameraUsecase)
{
    CDKResult result = CDKResultSuccess;
    // Get multi-cam controller object
    mcc_create_data_t mccCreateData = { 0 };
    mccCreateData.numBundledCameras = pCameraInfo->numPhysicalCameras;
    CHX_ASSERT(DualCamCount == pCameraInfo->numPhysicalCameras);
    cam_info_t camInfo[DualCamCount];

    for (UINT32 i = 0; i < mccCreateData.numBundledCameras; ++i)
    {
        camInfo[i].camId       = pCameraInfo->ppDeviceInfo[i]->cameraId;
        camInfo[i].pChiCamInfo = pCameraInfo->ppDeviceInfo[i]->m_pDeviceCaps;

        UINT32 selectedSensorMode = 0;

        camInfo[i].sensorOutDimension.width  = pCameraInfo->pSensorModeInfo[selectedSensorMode].frameDimension.width;
        camInfo[i].sensorOutDimension.height = pCameraInfo->pSensorModeInfo[selectedSensorMode].frameDimension.height;
        UINT32 xStart                        = pCameraInfo->pSensorModeInfo[selectedSensorMode].frameDimension.left;
        UINT32 yStart                        = pCameraInfo->pSensorModeInfo[selectedSensorMode].frameDimension.top;

        camInfo[i].fovRectIFE.left   = 0;
        camInfo[i].fovRectIFE.top    = 0;
        camInfo[i].fovRectIFE.width  = camInfo[i].pChiCamInfo->sensorCaps.activeArray.width - (2 * xStart);
        camInfo[i].fovRectIFE.height = camInfo[i].pChiCamInfo->sensorCaps.activeArray.height - (2 * yStart);

        mccCreateData.pBundledCamInfo[i] = &camInfo[i];
    }

    stream_config_t streamConfig = { 0 };
    streamConfig.numStreams      = pStreamConfig->num_streams;
    streamConfig.pStreamInfo     =
        static_cast<stream_info_t*>(CHX_CALLOC(pStreamConfig->num_streams * sizeof(stream_info_t)));

    CHX_ASSERT(NULL != streamConfig.pStreamInfo);

    if (NULL != streamConfig.pStreamInfo)
    {
        for (UINT32 i = 0; i < streamConfig.numStreams; ++i)
        {
            streamConfig.pStreamInfo[i].streamType             = pStreamConfig->streams[i]->stream_type;
            streamConfig.pStreamInfo[i].usage                  = pStreamConfig->streams[i]->usage;
            streamConfig.pStreamInfo[i].streamDimension.width  = pStreamConfig->streams[i]->width;
            streamConfig.pStreamInfo[i].streamDimension.height = pStreamConfig->streams[i]->height;
        }

        mccCreateData.pStreamConfig         = &streamConfig;
        mccCreateData.logicalCameraId       = 0;
        switch(dualCameraUsecase)
        {
            case UsecaseRTB:
                mccCreateData.desiredControllerMode = CONTROLLER_TYPE_RTB;
                break;
            case UsecaseSAT:
                mccCreateData.desiredControllerMode = CONTROLLER_TYPE_DUALFOV;
                break;
            default:
                mccCreateData.desiredControllerMode = CONTROLLER_TYPE_RTB;
                break;
        }

        m_pMultiCamController = MultiCamControllerManager::GetInstance()->GetController(&mccCreateData);

        if (NULL != m_pMultiCamController)
        {
            // Allocate input metadata buffer for the offline pipeline for preview
            m_pOfflinePipelineInputMetadataPreview =
                allocate_camera_metadata(ReplacedMetadataEntryCapacity, ReplacedMetadataDataCapacity);
            if(NULL == m_pOfflinePipelineInputMetadataPreview)
            {
                CHX_LOG_ERROR("allocate_camera_metadata fails for preview meta data");
                result = CDKResultEFailed;
            }

            // Allocate input metadata buffer for the offline pipeline for snapshot
            m_pOfflinePipelineInputMetadataSnapshot =
                allocate_camera_metadata(ReplacedMetadataEntryCapacity, ReplacedMetadataDataCapacity);
            if(NULL == m_pOfflinePipelineInputMetadataSnapshot)
            {
                CHX_LOG_ERROR("allocate_camera_metadata fails for snapshot meta data");
                result = CDKResultEFailed;
            }

            // Allocate input metadata buffer for the offline pipeline for RAW16
            m_pOfflinePipelineInputMetadataRAW16 =
                allocate_camera_metadata(ReplacedMetadataEntryCapacity, ReplacedMetadataDataCapacity);
            if (NULL == m_pOfflinePipelineInputMetadataRAW16)
            {
                CHX_LOG_ERROR("allocate_camera_metadata fails for RAW16 meta data");
                result = CDKResultEFailed;
            }

        }
        else
        {
            result = CDKResultEFailed;
        }

        CHX_FREE(streamConfig.pStreamInfo);
        streamConfig.pStreamInfo = NULL;
    }
    else
    {
        CHX_LOG_ERROR("Allocate stream configure buffer failed!");
        result = CDKResultEFailed;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::CreateOfflineSession
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::CreateOfflineSession()
{
    CDKResult result = CDKResultSuccess;
    ChiCallBacks callbacks = { 0 };
    UINT numOfPipeline = 1;
    UINT pipelineIndex = OFFLINEYUVPIPELINE;

    callbacks.ChiNotify               = SessionCbNotifyMessage;
    callbacks.ChiProcessCaptureResult = SessionCbCaptureResult;

    for (UINT i = OFFLINERDISESSION; i < MAX_DUAL_CAMERA_SESSION; i++ )
    {
        m_perSessionPvtData[i].sessionID  = i;
        m_perSessionPvtData[i].pUsecase   = this;

        if (NULL == m_pSession[i])
        {
            if ((OFFLINERAW16SESSION == i) && ((UsecaseRTB == m_dualCameraUsecase) || (FALSE == m_isRaw16Needed)))
            {
                // No need to create RAW16session if its RTB or RAW16 not needed cases
                continue;
            }
            m_pSession[i] = Session::Create(&m_pPipeline[pipelineIndex],
                numOfPipeline, &callbacks, &m_perSessionPvtData[i]);
            if (NULL == m_pSession[i])
            {
               CHX_LOG_ERROR("Failed to creat realtime Session with multiple pipeline");
               result = CDKResultEFailed;
               break;
            }
        }
        else
        {
            /// just warning, not fatal error.
            CHX_LOG_WARN("Warning:Session :%d has been created in other place!", i);
        }

        pipelineIndex += numOfPipeline;
    }

    if (CDKResultSuccess != result)
    {
        for (UINT i = OFFLINERDISESSION; i < MAX_DUAL_CAMERA_SESSION; i++)
        {
            if (NULL != m_pSession[i])
            {
                m_pSession[i]->Destroy(TRUE);
                m_pSession[i] = NULL;
            }
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::CreatePipeline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::CreatePipeline(
    ChxPipelineCreateData* pCreateData)
{
    CDKResult result             = CDKResultSuccess;

    CHX_ASSERT(NULL != pCreateData);
    CHX_ASSERT(NULL != pCreateData->pPipelineTargetCreateDescriptor);

    ChiPortBufferDescriptor outputBuffer[MaxChiStreams];
    ChiPortBufferDescriptor inputBuffer[MaxChiStreams];

    ChxUtils::Memset(outputBuffer, 0 , sizeof(outputBuffer));
    ChxUtils::Memset(inputBuffer, 0 , sizeof(inputBuffer));

    UINT pipelineIndex = pCreateData->pipelineIndex;

    m_pPipeline[pipelineIndex] = Pipeline::Create(pCreateData->cameraID,
        PipelineType::Default);

    if (NULL != m_pPipeline[pipelineIndex])
    {
        ChiTargetPortDescriptorInfo* pSinkTarget   = &pCreateData->pPipelineTargetCreateDescriptor->sinkTarget;
        ChiTargetPortDescriptorInfo* pSrcTarget    = &pCreateData->pPipelineTargetCreateDescriptor->sourceTarget;
        ChiPipelineCreateDescriptor* pPipelineDesp = &pCreateData->pPipelineTargetCreateDescriptor->pipelineCreateDesc;

        //check if numbers of input stream and output stream match numbers of sink target and source target

        if (pCreateData->numOfSinkStreams != pSinkTarget->numTargets)
        {
            CHX_LOG_ERROR("create pipeline:%s mismatch between numOfSinkStreams:%d and numofSinkTarget:%d!",
                pCreateData->pPipelineTargetCreateDescriptor->pPipelineName,
                pCreateData->numOfSinkStreams,
                pSinkTarget->numTargets);
        }

        if(pCreateData->numOfSourceStreams != pSrcTarget->numTargets)
        {
            CHX_LOG_ERROR("create pipeline:%s mismatch between numOfSourceStreams:%d and numofSourceTarget:%d!",
                pCreateData->pPipelineTargetCreateDescriptor->pPipelineName,
                pCreateData->numOfSourceStreams,
                pSrcTarget->numTargets);
        }

        if (CDKResultSuccess == result)
        {
            for (UINT i = 0 ; i < pCreateData->numOfSourceStreams ; i++)
            {
                inputBuffer[i].nodePort = pSrcTarget->pTargetPortDesc[i].nodeport;
                inputBuffer[i].pStream  = pCreateData->pSourceChiStream[i];
                pSrcTarget->pTargetPortDesc[i].pTarget->pChiStream = pCreateData->pSourceChiStream[i];
            }
            for (UINT i = 0 ; i < pCreateData->numOfSinkStreams ; i++)
            {
                outputBuffer[i].nodePort = pSinkTarget->pTargetPortDesc[i].nodeport;
                outputBuffer[i].pStream  = pCreateData->pSinkChiStream[i];
                pSinkTarget->pTargetPortDesc[i].pTarget->pChiStream = pCreateData->pSinkChiStream[i];
            }

            m_pPipeline[pipelineIndex]->SetOutputBuffers(pCreateData->numOfSinkStreams,
                &outputBuffer[0]);
            m_pPipeline[pipelineIndex]->SetInputBuffers(pCreateData->numOfSourceStreams,
                &inputBuffer[0]);
            m_pPipeline[pipelineIndex]->SetPipelineNodePorts(pPipelineDesp);

            // This is a multi-sensor use case, it needs more hardware resource for two realtime
            // pipeline, therefore, limit HW resource for each pipeline.
            m_pPipeline[pipelineIndex]->SetPipelineResourcePolicy(HWResourceMinimal);

            // here we just set defer flag for SAT usecase
            if (UsecaseSAT == m_dualCameraUsecase)
            {
                if (REALTIMEPIPELINEMAIN == pipelineIndex || REALTIMEPIPELINEAUX == pipelineIndex)
                {
                    m_pPipeline[pipelineIndex]->SetDeferFinalizeFlag(TRUE);
                }
            }

            result = m_pPipeline[pipelineIndex]->CreateDescriptor();
            if (CDKResultSuccess != result)
            {
                CHX_LOG_ERROR("Create pipeline %s descriptor failed!",
                    pCreateData->pPipelineTargetCreateDescriptor->pPipelineName);
            }
        }
    }
    else
    {
        CHX_LOG_ERROR("Create pipeline %d failed", pCreateData->pipelineIndex);
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::CreatePipeline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::FillPipelineCreateData(
        ChiUsecase*            pChiUsecase,
        LogicalCameraInfo*     pCameraInfo,
        UINT                   index,
        ChxPipelineCreateData* pCreateData)
{
    CDKResult result = CDKResultSuccess;
    ChiTargetPortDescriptorInfo* pSinkTarget = &pChiUsecase->pPipelineTargetCreateDesc[index].sinkTarget;
    ChiTargetPortDescriptorInfo* pSrcTarget  = &pChiUsecase->pPipelineTargetCreateDesc[index].sourceTarget;
    ChiPipelineCreateDescriptor* pPipeline   = &pChiUsecase->pPipelineTargetCreateDesc[index].pipelineCreateDesc;
    CHX_LOG_INFO("pipeline Index:%d, name: %s, targetNum:%d,sourceNums:%d",index,
        pChiUsecase->pPipelineTargetCreateDesc[index].pPipelineName,
        pSinkTarget->numTargets, pSrcTarget->numTargets);

    ChiTargetPortDescriptor* pSinkTargetDesc = NULL;
    ChiTargetPortDescriptor* pSrcTargetDesc  = NULL;
    UINT sinkIndex                           = 0;
    UINT sourceIndex                         = 0;
    UINT remappedPipelineIndex               = RemapPipelineIndex(index, m_dualCameraUsecase);
    pCreateData->pipelineIndex               = remappedPipelineIndex;
    pCreateData->cameraID                    = pCameraInfo->ppDeviceInfo[0]->cameraId;
    pCreateData->numOfSinkStreams            = 0;
    pCreateData->numOfSourceStreams          = 0;
    CHX_LOG("input Pipeline Index: %d, Remapped Index: %d", index, remappedPipelineIndex);
    switch (remappedPipelineIndex)
    {
        case REALTIMEPIPELINEMAIN:
        case REALTIMEPIPELINEAUX:
            pCreateData->cameraID      = pCameraInfo->ppDeviceInfo[remappedPipelineIndex]->cameraId;
            //Real time Preview
            pCreateData->pSinkChiStream[sinkIndex++] = m_pRTPreviewStream[remappedPipelineIndex];
            //Real time RDI
            pCreateData->pSinkChiStream[sinkIndex++] = m_pRTRDIStream[remappedPipelineIndex];
            //Real time RDI RAW16
            if (UsecaseSAT == m_dualCameraUsecase)
            {
                pCreateData->pSinkChiStream[sinkIndex++] = m_pRTRaw16Stream[remappedPipelineIndex];
            }

            break;

        case OFFLINEPREVIEWPIPELINE:
            // offline RAW16 pipeline  sink stream
            pCreateData->pSinkChiStream[sinkIndex++] = m_pTargetPreviewStream;

            if (UsecaseSAT == m_dualCameraUsecase)
            {
                if (TRUE == m_isVideoNeeded)
                {
                    pCreateData->pSinkChiStream[sinkIndex++] = m_pTargetVideoStream;
                }
                else
                {
                    pCreateData->pSinkChiStream[sinkIndex++] = m_pDummyStream[REALTIMEPIPELINEMAIN];;
                }
            }

            // offline preview pipeline source stream
            pCreateData->pSourceChiStream[sourceIndex++] = m_pRTBPreviewStream[REALTIMEPIPELINEMAIN];
            pCreateData->pSourceChiStream[sourceIndex++] = m_pRTBPreviewStream[REALTIMEPIPELINEAUX];

            break;

        case OFFLINEYUVPIPELINE:
            //Offline YUV Snapshot pipeline main

            pCreateData->pSinkChiStream[sinkIndex++]     = m_pRTYUVStream[REALTIMEPIPELINEMAIN];
            if (TRUE == m_isPostViewNeeded)
            {
                pCreateData->pSinkChiStream[sinkIndex++] = m_pTargetPostviewStream;
            }
            else
            {
                pCreateData->pSinkChiStream[sinkIndex++] = m_pDummyStream[REALTIMEPIPELINEMAIN];
            }

            pCreateData->pSourceChiStream[sourceIndex++] = m_pRTRDIStream[REALTIMEPIPELINEMAIN];

            break;

        case OFFLINEYUVPIPELINEAUX:
            //Offline YUV Snapshot pipeline aux
            pCreateData->cameraID                    = pCameraInfo->ppDeviceInfo[REALTIMEPIPELINEAUX]->cameraId;

            pCreateData->pSinkChiStream[sinkIndex++] = m_pRTYUVStream[REALTIMEPIPELINEAUX];

            if (TRUE == m_isPostViewNeeded)
            {
                pCreateData->pSinkChiStream[sinkIndex++] = m_pTargetPostviewStream;
            }
            else
            {
                pCreateData->pSinkChiStream[sinkIndex++] = m_pDummyStream[REALTIMEPIPELINEAUX];
            }

            pCreateData->pSourceChiStream[sourceIndex++] = m_pRTRDIStream[REALTIMEPIPELINEAUX];
            break;

        case OFFLINEFUSIONPIPELINE:
            pCreateData->pSinkChiStream[sinkIndex++]     = m_pSnapshotFusionStream;

            pCreateData->pSourceChiStream[sourceIndex++] = m_pJPEGStream[REALTIMEPIPELINEMAIN];
            pCreateData->pSourceChiStream[sourceIndex++] = m_pJPEGStream[REALTIMEPIPELINEAUX];
            break;

        case OFFLINEJPEGPIPELINE:
            // Offline Bokeh SNAPSHOT

            // Pass the CameraID corresponding to maximum resolution as logical camera contains the max jpeg size of both
            pCreateData->cameraID                        = UsecaseSelector::FindMaxResolutionCameraID(pCameraInfo);

            pCreateData->pSinkChiStream[sinkIndex++]     = m_pTargetSnapshotStream;
            pCreateData->pSourceChiStream[sourceIndex++] = m_pSnapshotFusionStream;
            break;

        case OFFLINERAW16PIPELINE:
            // offline preview pipeline  sink stream
            if (TRUE == m_isRaw16Needed)
            {
                pCreateData->pSinkChiStream[sinkIndex++] = m_pTargetRAW16Stream;
            }
            else
            {
                // Add RAW16 dummy stream to avoid this pipeline creation failure if RAW stream is not there
                pCreateData->pSinkChiStream[sinkIndex++] = m_pRTRaw16Stream[REALTIMEPIPELINEMAIN];
            }

            // offline preview pipeline source stream
            pCreateData->pSourceChiStream[sourceIndex++] = m_pInputRaw16Stream[REALTIMEPIPELINEMAIN];
            pCreateData->pSourceChiStream[sourceIndex++] = m_pInputRaw16Stream[REALTIMEPIPELINEAUX];

            break;

        default:
            CHX_LOG_ERROR("ERROR: Pipeline index is more than expected");
            result = CDKResultSuccess;
            break;

    }

    pCreateData->numOfSinkStreams                = sinkIndex;
    pCreateData->numOfSourceStreams              = sourceIndex;
    pCreateData->pPipelineTargetCreateDescriptor = &pChiUsecase->pPipelineTargetCreateDesc[index];
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::Flush()
{
    ChxUtils::AtomicStoreU32(&m_aFlushInProgress, TRUE);
    Usecase::FlushLock(TRUE);
    for (UINT session = 0; session < MaxSessions; session++)
    {
        if (NULL != m_sessions[session].pSession)
        {
            ExtensionModule::GetInstance()->Flush(m_sessions[session].pSession->GetSessionHandle());
        }

        if (NULL != m_pSession[session])
        {
            ExtensionModule::GetInstance()->Flush(m_pSession[session]->GetSessionHandle());
        }
    }
    m_flushLock = TRUE;
    Usecase::FlushLock(FALSE);
    m_flushLock = FALSE;
    ChxUtils::AtomicStoreU32(&m_aFlushInProgress, FALSE);
    return CDKResultSuccess;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::Initialize(
    LogicalCameraInfo*              pCameraInfo,   ///< Camera info
    camera3_stream_configuration_t* pStreamConfig) ///< Stream configuration
{
    CDKResult result             = CDKResultSuccess;
    ChiUsecase* pChiUsecase      = NULL;
    m_isPostViewNeeded           = FALSE;
    m_isVideoNeeded              = FALSE;
    UINT32 remappedPipelineIndex = 0;
    UINT32 RDIWidth[MAX_REALTIME_PIPELINE];
    UINT32 RDIHeight[MAX_REALTIME_PIPELINE];
    UINT32 RDIMaxWidth  = 0;
    UINT32 RDIMaxHeight = 0;
    UINT32 RAW16Width[MAX_REALTIME_PIPELINE];
    UINT32 RAW16Height[MAX_REALTIME_PIPELINE];
    UINT32 SATEnable;

    ChxUtils::Memset(RDIWidth, 0, sizeof(RDIWidth));
    ChxUtils::Memset(RDIHeight, 0, sizeof(RDIHeight));
    ChxUtils::Memset(RAW16Width, 0, sizeof(RAW16Width));
    ChxUtils::Memset(RAW16Height, 0, sizeof(RAW16Height));

    // Initialize the target streams to Null
    m_pTargetPreviewStream  = NULL;
    m_pTargetSnapshotStream = NULL;
    m_pTargetVideoStream    = NULL;
    m_pTargetPostviewStream = NULL;
    m_pTargetRAW16Stream    = NULL;
    m_pRTRaw16Stream[0]     = NULL;
    m_pRTRaw16Stream[1]     = NULL;
    m_pInputRaw16Stream[0]  = NULL;
    m_pInputRaw16Stream[1]  = NULL;
    m_pRDI16BufferManager[0] = NULL;
    m_pRDI16BufferManager[1] = NULL;

    SATEnable = ExtensionModule::GetInstance()->GetDCSATMode();
    m_kernelFrameSyncEnable = ExtensionModule::GetInstance()->GetDCFrameSyncMode();
    ExtensionModule::GetInstance()->GetVendorTagOps(&m_vendorTagOps);

    ChiPortBufferDescriptor pipelineOutputBuffer[MaxChiStreams];
    ChiPortBufferDescriptor pipelineInputBuffer[MaxChiStreams];

    ChxUtils::Memset(pipelineOutputBuffer, 0, sizeof(pipelineOutputBuffer));
    ChxUtils::Memset(pipelineInputBuffer, 0, sizeof(pipelineInputBuffer));
    ChxUtils::Memset(m_pendingRequest, 0, MAX_REALTIME_PIPELINE * sizeof(PendingRequestInfo));
    ChxUtils::Memset(m_stickyMetaNeeded, 0, MAX_REALTIME_PIPELINE * sizeof(BOOL));
    ChxUtils::Memset(m_jpegRequestId, 0, sizeof(m_jpegRequestId));

    m_bokehWideCapture = FALSE;
    m_jpegRequestCount = 0;

    m_usecaseId = UsecaseId::MultiCamera;
    m_cameraId = pCameraInfo->cameraId;

    CHX_LOG("%s, E.", __func__);

    /// initialize defer related resource.
    InitializeDeferResource();

    /// Work around. Accepting only preview
    for (UINT stream = 0; stream < pStreamConfig->num_streams; stream++)
    {
        pStreamConfig->streams[stream]->max_buffers = 8;
    }

    m_shutterFrameNum = -1;

    for (UINT stream = 0; stream < pStreamConfig->num_streams; stream++)
    {
        CHX_LOG("stream = %p streamType = %d streamFormat = %d streamWidth = %d streamHeight = %d usage = 0x%x",
            pStreamConfig->streams[stream],
            pStreamConfig->streams[stream]->stream_type,
            pStreamConfig->streams[stream]->format,
            pStreamConfig->streams[stream]->width,
            pStreamConfig->streams[stream]->height,
            pStreamConfig->streams[stream]->usage);
        if (HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED == pStreamConfig->streams[stream]->format)
        {
            if (UsecaseSelector::IsVideoStream(pStreamConfig->streams[stream]))
            {
                m_pTargetVideoStream = reinterpret_cast<CHISTREAM*>(pStreamConfig->streams[stream]);
                CHX_LOG("stream = %d m_pTargetVideoStream %p", stream, m_pTargetVideoStream);
                m_isVideoNeeded = TRUE;
            }
            else
            {
                m_pTargetPreviewStream = reinterpret_cast<CHISTREAM*>(pStreamConfig->streams[stream]);
                // Set SW_READ_OFTEN usage flag for NV12 output from SW CHI node(RTB/SAT)
                if (NULL != m_pTargetPreviewStream)
                {
                    m_pTargetPreviewStream->grallocUsage |= GRALLOC_USAGE_SW_READ_OFTEN;
                    CHX_LOG("stream = %d m_pTargetPreviewStream %p", stream, m_pTargetPreviewStream);
                }
                else
                {
                    result = CDKResultEFailed;
                    CHX_LOG("Error: m_pTargetPreviewStream is NULL");
                    break;
                }
            }
        }
        else if (HAL_PIXEL_FORMAT_BLOB == pStreamConfig->streams[stream]->format)
        {
            CHX_ASSERT(pCameraInfo->numPhysicalCameras == 2);

            if (NULL == m_pTargetSnapshotStream)
            {
                m_pTargetSnapshotStream = reinterpret_cast<CHISTREAM*>(pStreamConfig->streams[stream]);

                UsecaseSelector::getSensorDimension(pCameraInfo->ppDeviceInfo[0]->cameraId,
                    pStreamConfig,
                    &RDIWidth[0],
                    &RDIHeight[0],
                    1);

                UsecaseSelector::getSensorDimension(pCameraInfo->ppDeviceInfo[1]->cameraId,
                    pStreamConfig,
                    &RDIWidth[1],
                    &RDIHeight[1],
                    1);
                if ((RDIWidth[0] * RDIHeight[0]) > (RDIWidth[1] * RDIHeight[1]))
                {
                    RDIMaxWidth  = RDIWidth[0];
                    RDIMaxHeight = RDIHeight[0];
                }
                else
                {
                    RDIMaxWidth  = RDIWidth[1];
                    RDIMaxHeight = RDIHeight[1];
                }
                CHX_LOG("stream = %d m_pTargetSnapshotStream %p", stream, m_pTargetSnapshotStream);
            }
            else
            {
                m_pTargetWideJpegStream = reinterpret_cast<CHISTREAM*>(pStreamConfig->streams[stream]);
                m_bokehWideCapture      = TRUE;
                CHX_LOG("Wide Snapshot: stream = %d m_pTargetWideJpegStream %p",
                        stream, m_pTargetSnapshotStream);
            }
        }
        else if (HAL_PIXEL_FORMAT_YCbCr_420_888 == pStreamConfig->streams[stream]->format)
        {
            result = CDKResultEFailed;
            CHX_LOG_ERROR("Need to enable support for YUV stream");
            break;
        }
        else if (HAL_PIXEL_FORMAT_RAW16 == pStreamConfig->streams[stream]->format)
        {
            m_pTargetRAW16Stream = reinterpret_cast<CHISTREAM*>(pStreamConfig->streams[stream]);
            m_isRaw16Needed = TRUE;
            CHX_LOG("stream = %d m_pTargetRAW16Stream %p", stream, m_pTargetRAW16Stream);
        }
    }

    if (FALSE == m_isRaw16Needed)
    {
        // Dummy RAW16 size to avoid the pipeline creation failure
        RAW16Width[REALTIMEPIPELINEMAIN]  = RDIWidth[REALTIMEPIPELINEMAIN];
        RAW16Height[REALTIMEPIPELINEMAIN] = RDIHeight[REALTIMEPIPELINEMAIN];

        RAW16Width[REALTIMEPIPELINEAUX]   = RDIWidth[REALTIMEPIPELINEAUX];
        RAW16Height[REALTIMEPIPELINEAUX]  = RDIHeight[REALTIMEPIPELINEAUX];
    }
    else if (NULL != m_pTargetRAW16Stream)
    {
        RAW16Width[REALTIMEPIPELINEMAIN]  = m_pTargetRAW16Stream->width;
        RAW16Height[REALTIMEPIPELINEMAIN] = m_pTargetRAW16Stream->height;

        RAW16Width[REALTIMEPIPELINEAUX]   = m_pTargetRAW16Stream->width;
        RAW16Height[REALTIMEPIPELINEAUX]  = m_pTargetRAW16Stream->height;
    }

    // @todo: Need to change to APP dimension when application support is added
    RAW16Width[REALTIMEPIPELINEMAIN]  = RDIWidth[REALTIMEPIPELINEMAIN];
    RAW16Height[REALTIMEPIPELINEMAIN] = RDIHeight[REALTIMEPIPELINEMAIN];

    RAW16Width[REALTIMEPIPELINEAUX]   = RDIWidth[REALTIMEPIPELINEAUX];
    RAW16Height[REALTIMEPIPELINEAUX]  = RDIHeight[REALTIMEPIPELINEAUX];

    CHX_LOG("Main Raw16 widxHei %dx%d Aux widxHei %dx%d", RAW16Width[REALTIMEPIPELINEMAIN],
        RAW16Height[REALTIMEPIPELINEMAIN], RAW16Width[REALTIMEPIPELINEAUX], RAW16Height[REALTIMEPIPELINEAUX]);

    if (TRUE == SATEnable)
    {
        pChiUsecase   = &(Usecases13Target[UsecaseSATId]);
        m_dualCameraUsecase = UsecaseSAT;
        CHX_LOG("SAT/DUAL FOV Preview with frame sync mode %d", m_kernelFrameSyncEnable);
    }
    else
    {
        ///< @todo (CAMX-1445) Need Add another logical camera id to choose SAT/ SAT video
        pChiUsecase   = &(Usecases9Target[UsecaseRTBId]);
        m_dualCameraUsecase = UsecaseRTB;
        CHX_LOG("RTB Preview with frame sync mode %d", m_kernelFrameSyncEnable);;
    }

    if (((TRUE == m_isRaw16Needed) || (TRUE == m_isVideoNeeded)) && (UsecaseRTB == m_dualCameraUsecase))
    {
        CHX_LOG_ERROR("ERROR: RAW16 or Video recording is not supported in RTB usecase");
        result = CDKResultEFailed;
    }

    if ((NULL != pChiUsecase) && (CDKResultSuccess == result))
    {
        CHX_LOG("[MultiCamera] usecase name = %s, numPipelines = %d numTargets = %d",
            pChiUsecase->pUsecaseName, pChiUsecase->numPipelines, pChiUsecase->numTargets);

        if (m_pTargetPreviewStream == NULL)
        {
            CHX_LOG_ERROR("m_pTargetPreviewStream is NULL, returning result as failed");
            result = CDKResultEFailed;
            return result;
        }

        ///< allocate stream for realtimepreview1 and realtimepreview2
        CHIBufferManagerCreateData previewData     = {0};
        previewData.format                      = m_pTargetPreviewStream->format;
        previewData.width                       = m_pTargetPreviewStream->width;
        previewData.height                      = m_pTargetPreviewStream->height;
        previewData.consumerFlags               = GRALLOC1_CONSUMER_USAGE_CAMERA;
        previewData.producerFlags               = GRALLOC1_PRODUCER_USAGE_CAMERA;
        previewData.maxBufferCount              = ZSLQueueDepth * 2;
        previewData.immediateBufferCount        = CHIImmediateBufferCountZSL;

        for(UINT i=0 ; i < MAX_REALTIME_PIPELINE ; i++)
        {
            m_pRTBPreviewStream[i]  = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
            m_pRTPreviewStream[i]   = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
            m_pDummyStream[i]       = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
            m_pRTBSnapshotStream[i] = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
            m_pRTYUVStream[i]       = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
            m_pJPEGStream[i]        = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));

            // if Video enable, it needes to check which stream size is bigger, and use bigger
            // stream size for realtime preview pipeline output stream.
            if (m_isVideoNeeded &&
                (m_pTargetVideoStream->width * m_pTargetVideoStream->height) >
                (m_pTargetPreviewStream->width * m_pTargetPreviewStream->height))
            {
                ChxUtils::Memcpy(m_pRTBPreviewStream[i], m_pTargetVideoStream, sizeof(CHISTREAM));
                ChxUtils::Memcpy(m_pRTPreviewStream[i], m_pTargetVideoStream, sizeof(CHISTREAM));
                ChxUtils::Memcpy(m_pDummyStream[i], m_pTargetVideoStream, sizeof(CHISTREAM));
                previewData.format                      = m_pTargetVideoStream->format;
                previewData.width                       = m_pTargetVideoStream->width;
                previewData.height                      = m_pTargetVideoStream->height;
            }
            else
            {
                ChxUtils::Memcpy(m_pRTBPreviewStream[i], m_pTargetPreviewStream, sizeof(CHISTREAM));
                ChxUtils::Memcpy(m_pRTPreviewStream[i], m_pTargetPreviewStream, sizeof(CHISTREAM));
                ChxUtils::Memcpy(m_pDummyStream[i], m_pTargetPreviewStream, sizeof(CHISTREAM));
            }
            m_pDummyStream[i]->width = DUMMYWIDTH;
            m_pDummyStream[i]->height = DUMMYHEIGHT;
            m_pDummyStream[i]->format = ChiStreamFormatYCbCr420_888;
            ChxUtils::Memcpy(m_pRTYUVStream[i], m_pTargetSnapshotStream, sizeof(CHISTREAM));
            m_pRTYUVStream[i]->format = ChiStreamFormatYCbCr420_888;

            // Assign RDI dimensions to YUV output as BOKEH/SAT node needs the uncropped image
            // and also need to handle the asymmetric configuration
            // For Asymmetric W X H for MAIN & AUX assign max W X H so, that two YUV streams would be
            // of same size and thus SAT Node can handle it.
            if (UsecaseSAT == m_dualCameraUsecase)
            {
                m_pRTYUVStream[i]->width  = RDIMaxWidth;
                m_pRTYUVStream[i]->height = RDIMaxHeight;
            }
            else
            {
                m_pRTYUVStream[i]->width  = RDIWidth[i];
                m_pRTYUVStream[i]->height = RDIHeight[i];
            }

            m_pRTYUVStream[i]->grallocUsage  = (GRALLOC1_PRODUCER_USAGE_CAMERA |
                                                GRALLOC1_CONSUMER_USAGE_CAMERA |
                                                GRALLOC1_CONSUMER_USAGE_CPU_READ);

            ChxUtils::Memcpy(m_pJPEGStream[i], m_pRTYUVStream[i], sizeof(CHISTREAM));

            m_yuvSnapBufferCreateData[i]                         = {0};
            m_yuvSnapBufferCreateData[i].format                  = m_pTargetPreviewStream->format;
            m_yuvSnapBufferCreateData[i].width                   = m_pRTYUVStream[i]->width;
            m_yuvSnapBufferCreateData[i].height                  = m_pRTYUVStream[i]->height;
            m_yuvSnapBufferCreateData[i].consumerFlags           = GRALLOC1_CONSUMER_USAGE_CAMERA;
            m_yuvSnapBufferCreateData[i].producerFlags           = GRALLOC1_PRODUCER_USAGE_CAMERA;
            // maxBufferCount is increased from 2 to 5 because when OEMs add chinodes to the offline pipeline
            // 2 buffers are insufficient during burst snapshot as the pipeline takes longer to process
            // RTB will not support burst.
            if (UsecaseSAT == m_dualCameraUsecase)
            {
                m_yuvSnapBufferCreateData[i].maxBufferCount          = 5;
            }
            else
            {
                m_yuvSnapBufferCreateData[i].maxBufferCount          = 2;
            }
            m_yuvSnapBufferCreateData[i].immediateBufferCount    = 1;

            m_pRTRDIStream[i]                = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
            m_pRTRDIStream[i]->format        = ChiStreamFormatRawOpaque;
            m_pRTRDIStream[i]->width         = RDIWidth[i];
            m_pRTRDIStream[i]->height        = RDIHeight[i];
            m_pRTRDIStream[i]->maxNumBuffers = 0;
            m_pRTRDIStream[i]->rotation      = StreamRotationCCW0;
            m_pRTRDIStream[i]->streamType    = ChiStreamTypeBidirectional;
            m_pRTRDIStream[i]->grallocUsage  = 0;
            ChxUtils::Memcpy(m_pRTBSnapshotStream[i], m_pRTRDIStream[i], sizeof(CHISTREAM));

            m_rdiBufferCreateData[i]                         = {0};
            m_rdiBufferCreateData[i].format                  = HAL_PIXEL_FORMAT_RAW10;
            m_rdiBufferCreateData[i].width                   = RDIWidth[i];
            m_rdiBufferCreateData[i].height                  = RDIHeight[i];
            m_rdiBufferCreateData[i].consumerFlags           = GRALLOC1_CONSUMER_USAGE_CAMERA;
            m_rdiBufferCreateData[i].producerFlags           = GRALLOC1_PRODUCER_USAGE_CAMERA;
            // Why ZSLQueueDepth * 2 + 6 for maxBufferCount:
            //  ZSL queue will be filled up with buffers and extra 6 buffers are needed to keep the preview running
            //  ZSLQueueDepth + 6 (maxHalRequests) is the minimum buffer count to support preview
            //  In case there is a MFNR snapshot request which taks all the RDIs from the zsl queue,
            //  we need to fill up the queue again with 'ZSLQueueDepth' RDIs buffers for the next snapshot.
            m_rdiBufferCreateData[i].maxBufferCount          = ((ZSLQueueDepth * 2) + 6);
            m_rdiBufferCreateData[i].immediateBufferCount    = CHIImmediateBufferCountZSL;

            if (UsecaseSAT == m_dualCameraUsecase)
            {
                m_pRTRaw16Stream[i]                = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
                m_pRTRaw16Stream[i]->format        = ChiStreamFormatRaw16;
                m_pRTRaw16Stream[i]->width         = RAW16Width[i];
                m_pRTRaw16Stream[i]->height        = RAW16Height[i];
                m_pRTRaw16Stream[i]->maxNumBuffers = 0;
                m_pRTRaw16Stream[i]->rotation      = StreamRotationCCW0;
                m_pRTRaw16Stream[i]->streamType    = ChiStreamTypeBidirectional;
                m_pRTRaw16Stream[i]->grallocUsage  = 0;

                m_pInputRaw16Stream[i]             = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
                ChxUtils::Memcpy(m_pInputRaw16Stream[i], m_pRTRaw16Stream[i], sizeof(CHISTREAM));

                // Allocate RAW16 buffers if RAW16 stream is present in configurestreams
                if (TRUE == m_isRaw16Needed)
                {
                    m_rdi16BufferCreateData[i]                         = {0};
                    m_rdi16BufferCreateData[i].format                  = HAL_PIXEL_FORMAT_RAW16;
                    m_rdi16BufferCreateData[i].width                   = RAW16Width[i];
                    m_rdi16BufferCreateData[i].height                  = RAW16Height[i];
                    m_rdi16BufferCreateData[i].consumerFlags           = GRALLOC1_CONSUMER_USAGE_CAMERA;
                    m_rdi16BufferCreateData[i].producerFlags           = GRALLOC1_PRODUCER_USAGE_CAMERA;
                    m_rdi16BufferCreateData[i].maxBufferCount          = ((ZSLQueueDepth * 2) + 6);
                    m_rdi16BufferCreateData[i].immediateBufferCount    = CHIImmediateBufferCountZSL;
                }
            }
        }

        m_pSnapshotFusionStream        = static_cast<CHISTREAM*>(ChxUtils::Calloc(sizeof(CHISTREAM)));
        if (NULL != m_pSnapshotFusionStream)
        {
            // Pass the maximum resolution to snapshot fusion pipeline
            if ((m_pJPEGStream[REALTIMEPIPELINEMAIN]->width * m_pJPEGStream[REALTIMEPIPELINEMAIN]->height) >
                (m_pJPEGStream[REALTIMEPIPELINEAUX]->width * m_pJPEGStream[REALTIMEPIPELINEAUX]->height))
            {
                ChxUtils::Memcpy(m_pSnapshotFusionStream, m_pJPEGStream[REALTIMEPIPELINEMAIN], sizeof(CHISTREAM));
            }
            else
            {
                ChxUtils::Memcpy(m_pSnapshotFusionStream, m_pJPEGStream[REALTIMEPIPELINEAUX], sizeof(CHISTREAM));
            }
            m_pSnapshotFusionStream->grallocUsage  = (GRALLOC1_PRODUCER_USAGE_CAMERA |
                                                      GRALLOC1_CONSUMER_USAGE_CAMERA |
                                                      GRALLOC1_CONSUMER_USAGE_CPU_READ);

            m_fusionBufferCreateData                      = { 0 };
            m_fusionBufferCreateData.format               = m_pTargetPreviewStream->format;
            m_fusionBufferCreateData.width                = m_pSnapshotFusionStream->width;
            m_fusionBufferCreateData.height               = m_pSnapshotFusionStream->height;
            m_fusionBufferCreateData.consumerFlags        = GRALLOC1_CONSUMER_USAGE_CAMERA;
            m_fusionBufferCreateData.producerFlags        = GRALLOC1_PRODUCER_USAGE_CAMERA;
            m_fusionBufferCreateData.maxBufferCount       = 2;
            m_fusionBufferCreateData.immediateBufferCount = 1;
        }
        else
        {
            CHX_LOG_ERROR("m_pSnapshotFusionStream allocation failure");
            result = CDKResultEFailed;
        }

        CHX_LOG_INFO("MAIN YUV widxHei %dx%d AUX YUV widxHei %dx%d Fusion widxHei %dx%d Final JPEG widxhei %dx%d",
            m_yuvSnapBufferCreateData[REALTIMEPIPELINEMAIN].width,
            m_yuvSnapBufferCreateData[REALTIMEPIPELINEMAIN].height,
            m_yuvSnapBufferCreateData[REALTIMEPIPELINEAUX].width,
            m_yuvSnapBufferCreateData[REALTIMEPIPELINEAUX].height,
            m_fusionBufferCreateData.width,
            m_fusionBufferCreateData.height,
            m_pTargetSnapshotStream->width,
            m_pTargetSnapshotStream->height);

        for (UINT i = 0; i < pChiUsecase->numPipelines; i++)
        {
            ChiTargetPortDescriptorInfo* pSourceTarget =
                &pChiUsecase->pPipelineTargetCreateDesc[i].sourceTarget;
            ChiTargetPortDescriptorInfo* pSinkTarget =
                &pChiUsecase->pPipelineTargetCreateDesc[i].sinkTarget;

            CHX_LOG("Pipeline[%d] name = %s, Source Target num = %d, Sink Target num = %d",
                i, pChiUsecase->pPipelineTargetCreateDesc[i].pPipelineName,
                pSourceTarget->numTargets,
                pSinkTarget->numTargets);

            CHAR bufferManagerName[MaxStringLength64];
            UINT remappedPipelineIndex = RemapPipelineIndex(i, m_dualCameraUsecase);
            if ((REALTIMEPIPELINEMAIN == remappedPipelineIndex) ||
                (REALTIMEPIPELINEAUX == remappedPipelineIndex))
            {
                // allocate realtime buffer according to target buffer format
                for (UINT32 target = 0; target < pSinkTarget->numTargets; target++)
                {
                    if (ChiFormatP010 == pSinkTarget->pTargetPortDesc[target].pTarget->pBufferFormats[0])
                    {
                        // To P010, we can use YUV420_888 format with double width to get correct buffer allocation.
                        previewData.format                                 = ChiStreamFormatYCbCr420_888;
                        previewData.width                                  = m_pTargetPreviewStream->width * 2;
                        m_pRTPreviewStream[remappedPipelineIndex]->format  = ChiStreamFormatP010;
                        m_pRTBPreviewStream[remappedPipelineIndex]->format = ChiStreamFormatP010;
                    }
                }

                CdkUtils::SNPrintF(bufferManagerName, sizeof(bufferManagerName),
                    "MCPreviewBufferManager_%d", remappedPipelineIndex);
                m_pPreviewBufferManager[remappedPipelineIndex] =
                    CHIBufferManager::Create(bufferManagerName, &previewData);
            }
        }

        // Assign stream type
        for (UINT i = 0; i < MAX_REALTIME_PIPELINE; i++)
        {
            CHAR bufferManagerName[MaxStringLength64];
            CdkUtils::SNPrintF(bufferManagerName, sizeof(bufferManagerName), "MCRDIBufferManager_%d", i);
            AddNewInternalBufferQueue(m_pRTRDIStream[i], &m_rdiBufferCreateData[i], &m_RDIBufferID[i], bufferManagerName);

            m_pRTPreviewStream[i]->streamType   = ChiStreamTypeBidirectional;
            m_pRTBPreviewStream[i]->streamType  = ChiStreamTypeInput;
            m_pDummyStream[i]->streamType       = ChiStreamTypeOutput;
            m_pRTYUVStream[i]->streamType       = ChiStreamTypeOutput;
            m_pRTBSnapshotStream[i]->streamType = ChiStreamTypeInput;
            m_pJPEGStream[i]->streamType        = ChiStreamTypeInput;

            if (UsecaseSAT == m_dualCameraUsecase)
            {
                m_pRTRaw16Stream[i]->streamType     = ChiStreamTypeBidirectional;
                m_pInputRaw16Stream[i]->streamType  = ChiStreamTypeInput;
            }
        }
        if (NULL != m_pSnapshotFusionStream)
        {
            m_pSnapshotFusionStream->streamType = ChiStreamTypeBidirectional;
        }

        ChxPipelineCreateData pipelineCreateData;
        for (UINT i = 0; i < pChiUsecase->numPipelines; i++)
        {
            ChxUtils::Memset(&pipelineCreateData, 0, sizeof(ChxPipelineCreateData));
            FillPipelineCreateData(pChiUsecase, pCameraInfo, i, &pipelineCreateData);
            CreatePipeline(&pipelineCreateData);
        }
    }
    else
    {
        CHX_LOG_ERROR("pChiUsecase is NULL or stream config is not supported");
        result = CDKResultEFailed;
    }


    if (CDKResultSuccess == result)
    {
        // here start a new thread to do offline session create.
        StartDeferThread();

        // here just create one realtime session, and defer all offline related pipeline and session create.
        ChiCallBacks callbacks = { 0 };

        callbacks.ChiNotify               = SessionCbNotifyMessage;
        callbacks.ChiProcessCaptureResult = SessionCbCaptureResult;

        m_perSessionPvtData[REALTIMESESSIONIDX].sessionID  = REALTIMESESSIONIDX;
        m_perSessionPvtData[REALTIMESESSIONIDX].pUsecase   = this;

        m_pSession[REALTIMESESSIONIDX] = Session::Create(&m_pPipeline[REALTIMESESSIONIDX],
            MAX_REALTIME_PIPELINE, &callbacks, &m_perSessionPvtData[REALTIMESESSIONIDX]);
        if (NULL == m_pSession[REALTIMESESSIONIDX])
        {
           CHX_LOG_ERROR("Failed to creat realtime Session with multiple pipeline");
           result = CDKResultEFailed;
        }


        if (CDKResultSuccess == result)
        {
            // don't initialize advance features in video mode
            if (canEnableAdvanceFeature())
            {
                result = InitializeAdvanceFeatures(pCameraInfo, pStreamConfig);
                CHX_LOG("m_pVideo;%p,usecase:%d", m_pTargetVideoStream, m_dualCameraUsecase);
            }
            else
            {
                result = Usecase::Initialize();
            }
        }


        if (CDKResultSuccess == result)
        {
            result = CreateMultiController(pCameraInfo, pStreamConfig, m_dualCameraUsecase);
            if (CDKResultSuccess == result)
            {
                // Allocate the sticky meta data
                for (UINT i = 0; i < MAX_REALTIME_PIPELINE; i++)
                {
                    m_pStickyMetadata[i] = allocate_camera_metadata(ReplacedMetadataEntryCapacity,
                        ReplacedMetadataDataCapacity);
                    if(NULL == m_pStickyMetadata[i])
                    {
                        CHX_LOG_ERROR("allocate_camera_metadata fails for sticky meta data");
                        result = CDKResultEFailed;
                    }
                }
            }
        }
    }

    CreateOfflineProcessResource();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::InitializeAdvanceFeatures
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::InitializeAdvanceFeatures(
    LogicalCameraInfo*              pCameraInfo,
    camera3_stream_configuration_t* pStreamConfig)
{
    CDKResult result = CDKResultSuccess;

    CHX_LOG("InitializeAdvanceFeatures: E.");

    // @todo: move to a funciton to pass CHIStreams to advance usecase
    // use camera id 0 first.
    m_pRdiStream = m_pRTRDIStream[0];
    m_pRdiStreamAux = m_pRTRDIStream[1];

    result = AdvancedCameraUsecase::Initialize(
        pCameraInfo,
        pStreamConfig,
        UsecaseId::MultiCamera);

    CHX_LOG("InitializeAdvanceFeatures: X.");
    return CDKResultSuccess;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::ExecuteCaptureRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::ExecuteCaptureRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult result = CDKResultSuccess;

    UINT32 applicationFrameNum       = pRequest->frame_number;
    UINT32 applicationFrameNumIndex  = applicationFrameNum % MaxOutstandingRequests;
    // Request to generate preview for now
    for (UINT i = 0; i < pRequest->num_output_buffers; i++)
    {
        CHX_LOG("Request : output buffers :%d frameNum :%d acquireFence: %d , ReleaseFence: %d Buffer: %p, status: %d",
            pRequest->num_output_buffers, pRequest->frame_number, pRequest->output_buffers[i].acquire_fence,
            pRequest->output_buffers[i].release_fence, pRequest->output_buffers[i].buffer,
            pRequest->output_buffers[i].status);
    }

    camera3_capture_result_t* pUsecaseResult = GetCaptureResult(applicationFrameNumIndex);

    pUsecaseResult->result                           = NULL;
    pUsecaseResult->frame_number                     = applicationFrameNum;
    pUsecaseResult->num_output_buffers               = 0;
    m_isMetadataSent[applicationFrameNumIndex]       = FALSE;
    m_isMetadataAvailable[applicationFrameNumIndex]  = FALSE;
    m_shutterTimestamp[applicationFrameNumIndex]     = 0;

    result = UpdateFeatureModeIndex(const_cast<camera_metadata_t*>(pRequest->settings));

    controller_result_t mccResult = m_pMultiCamController->GetResult();
    Feature* pFeature = NULL;

    if (TRUE == canEnableAdvanceFeature())
    {
        // Check if the capture request is a snapshot request
        for (UINT32 stream = 0; stream < pRequest->num_output_buffers; stream++) {
            if ((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetSnapshotStream)
            {
                m_isLLSSnapshot = (TRUE == IsLLSNeeded()) ? TRUE : FALSE;
                CHX_LOG("snapshot request, appFrameNum:%d, m_isLLSSnapshot:%d", applicationFrameNum, m_isLLSSnapshot);

                // Get the feature to support the snapshot
                pFeature = SelectFeatureToExecuteCaptureRequest(pRequest);

                // Currently only support the snapshot from HDR or MFNR feature.
                if ((NULL != pFeature) && (FeatureType::HDR != pFeature->GetFeatureType()) &&
                    (FeatureType::MFNR != pFeature->GetFeatureType()) &&
                    (FeatureType::SWMF != pFeature->GetFeatureType()))
                {
                    pFeature = NULL;
                }
                break;
            }
        }
    }
    UINT32 internalFrameNum          = m_realtimeRequestID;
    UINT32 internalFrameNumIndex     = internalFrameNum %  MaxOutstandingRequests;

    // @note: currently the feature only support wide sensor
    if ((NULL != pFeature) && canEnableAdvanceFeature())
    {
        if ((FeatureType::HDR == pFeature->GetFeatureType()) ||
            ((FeatureType::MFNR == pFeature->GetFeatureType()) && (TRUE == m_isLLSSnapshot)))
        {
            if (FeatureType::MFNR == pFeature->GetFeatureType())
            {
                CHX_LOG("Trigger LLS snapshot for MFNR:%d", internalFrameNum);
            }

            FeatureRequestInfo requestInfo;
            pFeature->GetRequestInfo(pRequest, &requestInfo);
            for (UINT32 i = 0; i < requestInfo.numOfRequest; i++)
            {
                internalFrameNum          = m_realtimeRequestID++;
                internalFrameNumIndex     = internalFrameNum %  MaxOutstandingRequests;
                requestInfo.request[i].frame_number = internalFrameNum;

                m_requestIDToFrameNumber[internalFrameNumIndex] = { 0 };
                m_requestIDToFrameNumber[internalFrameNumIndex].frameNumber             = applicationFrameNum;
                m_requestIDToFrameNumber[internalFrameNumIndex].masterCameraID          = mccResult.masterCameraId;
                m_requestIDToFrameNumber[internalFrameNumIndex].pFeature                = pFeature;
                m_requestIDToFrameNumber[internalFrameNumIndex].isMetaReturnNeeded      = requestInfo.isReturnResult[i];
                m_requestIDToFrameNumber[internalFrameNumIndex].isPreviewReturnNeeded   = requestInfo.isReturnResult[i];
                m_requestIDToFrameNumber[internalFrameNumIndex].isSnapshotReturnNeeded  = requestInfo.isReturnResult[i];
                m_requestIDToFrameNumber[internalFrameNumIndex].isSkipPreview           = FALSE;

                if ((FeatureType::HDR == pFeature->GetFeatureType()) && (TRUE == requestInfo.isReturnResult[i]))
                {
                    CHX_LOG("requestIdx:%d, skip preview frame for HDR, AppFrameNum:%d, internalFrameNum:%d",
                        i, applicationFrameNum, internalFrameNum);
                    m_requestIDToFrameNumber[internalFrameNumIndex].isSkipPreview = TRUE;
                }

                if (0 == i)
                {
                    m_requestIDToFrameNumber[internalFrameNumIndex].isShutterReturnNeeded = TRUE;
                }
                else
                {
                    m_requestIDToFrameNumber[internalFrameNumIndex].isShutterReturnNeeded = FALSE;
                }

                if (FeatureType::HDR == pFeature->GetFeatureType())
                {
                    m_requestIDToFrameNumber[internalFrameNumIndex].triggerOfflineReprocess = TRUE;
                }
                else
                {
                    m_requestIDToFrameNumber[internalFrameNumIndex].triggerOfflineReprocess = FALSE;
                }

                camera3_stream_buffer_t buffer;
                if (FALSE == requestInfo.isReturnResult[i])
                {
                    requestInfo.request[i].num_output_buffers = 1;
                    ChxUtils::Memset(&buffer, 0, sizeof(camera3_stream_buffer_t));
                    if (mccResult.masterCameraId == m_pSession[REALTIMESESSIONIDX]->GetCameraId(REALTIMEPIPELINEMAIN))
                    {
                        CHX_LOG("using REALTIMEPIPELINEMAIN RDI stream");
                        m_pTargetRDIStream = m_pRTRDIStream[REALTIMEPIPELINEMAIN];
                    }
                    else if (mccResult.masterCameraId == m_pSession[REALTIMESESSIONIDX]->GetCameraId(REALTIMEPIPELINEAUX))
                    {
                        CHX_LOG("using REALTIMEPIPELINEAUX RDI stream");
                        m_pTargetRDIStream = m_pRTRDIStream[REALTIMEPIPELINEAUX];
                    }
                    else
                    {
                        CHX_LOG_ERROR("no active realtime pipeline for request:%d", internalFrameNum);
                    }
                    buffer.stream = reinterpret_cast<camera3_stream_t*>(m_pTargetRDIStream);
                    requestInfo.request[i].output_buffers = &buffer;
                }
                if (mccResult.masterCameraId == m_pSession[REALTIMESESSIONIDX]->GetCameraId(REALTIMEPIPELINEMAIN))
                {
                    m_requestIDToFrameNumber[internalFrameNumIndex].activePipelineID = REALTIMEPIPELINEMAIN;
                }
                else
                {
                    m_requestIDToFrameNumber[internalFrameNumIndex].activePipelineID = REALTIMEPIPELINEAUX;
                }
                if (FeatureType::HDR == pFeature->GetFeatureType())
                {
                    CHX_LOG("send HDR subrequest:framenum:%d, requestID:%d, isreturn:%d",
                        applicationFrameNum, internalFrameNum, requestInfo.isReturnResult[i]);
                }
                else
                {
                    CHX_LOG("send LLS-MFNR subrequest:framenum:%d, requestID:%d, isreturn:%d",
                        applicationFrameNum, internalFrameNum, requestInfo.isReturnResult[i]);
                }

                result = GenerateRealtimeRequest(&requestInfo.request[i]);
                if (TRUE == isOfflineProcessRequired(pRequest, &m_requestIDToFrameNumber[internalFrameNumIndex]))
                {
                    TriggerOfflineRequest(internalFrameNum, pRequest);
                }

            }

            /// For LLS + MFNR, wait until ALL RDI buffers are ready, then trigger MFNR reprocess.
            /// So only set trigger reprocess flag for the last reqId
            if (FeatureType::MFNR == pFeature->GetFeatureType())
            {
                m_requestIDToFrameNumber[internalFrameNumIndex].triggerOfflineReprocess = TRUE;
                CHX_LOG("ReqId:%d, m_requestIDToFrameNumber[%d].triggerOfflineReprocess = TRUE ",
                    internalFrameNum, internalFrameNumIndex);
            }
        }
        else if (((FeatureType::MFNR == pFeature->GetFeatureType()) && (FALSE == m_isLLSSnapshot)) ||
                  (FeatureType::SWMF == pFeature->GetFeatureType()))
        {
            CHX_LOG("Send MFNR/SWMF request:%d", internalFrameNum);

            FeatureRequestInfo requestInfo;
            FeatureMFNR* pFeatureMFNR = reinterpret_cast<FeatureMFNR*>(pFeature);

            pFeatureMFNR->GetRequestInfo(pRequest, &requestInfo);

            camera3_capture_request_t request;
            ChxUtils::Memcpy(&request, pRequest, sizeof(camera3_capture_request_t));
            internalFrameNum          = m_realtimeRequestID++;
            internalFrameNumIndex     = internalFrameNum %  MaxOutstandingRequests;
            request.frame_number      = internalFrameNum;

            m_requestIDToFrameNumber[internalFrameNumIndex]                         = { 0 };
            m_requestIDToFrameNumber[internalFrameNumIndex].frameNumber             = applicationFrameNum;
            m_requestIDToFrameNumber[internalFrameNumIndex].masterCameraID          = mccResult.masterCameraId;
            m_requestIDToFrameNumber[internalFrameNumIndex].pFeature                = pFeature;
            m_requestIDToFrameNumber[internalFrameNumIndex].triggerOfflineReprocess = TRUE;
            m_requestIDToFrameNumber[internalFrameNumIndex].isMetaReturnNeeded      = TRUE;
            m_requestIDToFrameNumber[internalFrameNumIndex].isPreviewReturnNeeded   = hasPreviewStreamRequest(pRequest);;
            m_requestIDToFrameNumber[internalFrameNumIndex].isShutterReturnNeeded   = TRUE;
            m_requestIDToFrameNumber[internalFrameNumIndex].isSnapshotReturnNeeded  = TRUE;
            m_requestIDToFrameNumber[internalFrameNumIndex].isSkipPreview           = FALSE;

            if (mccResult.masterCameraId == m_pSession[REALTIMESESSIONIDX]->GetCameraId(REALTIMEPIPELINEMAIN))
            {
                m_requestIDToFrameNumber[internalFrameNumIndex].activePipelineID = REALTIMEPIPELINEMAIN;
            }
            else
            {
                m_requestIDToFrameNumber[internalFrameNumIndex].activePipelineID = REALTIMEPIPELINEAUX;
            }
            if (TRUE == isOfflineProcessRequired(pRequest, &m_requestIDToFrameNumber[internalFrameNumIndex]))
            {
                TriggerOfflineRequest(internalFrameNum, pRequest);
            }
            result = GenerateRealtimeRequest(&request);

        }
    }
    else
    {
        camera3_capture_request_t request;
        ChxUtils::Memcpy(&request, pRequest, sizeof(camera3_capture_request_t));
        internalFrameNum          = m_realtimeRequestID++;
        internalFrameNumIndex     = internalFrameNum %  MaxOutstandingRequests;
        request.frame_number      = internalFrameNum;

        m_requestIDToFrameNumber[internalFrameNumIndex]                         = { 0 };
        m_requestIDToFrameNumber[internalFrameNumIndex].frameNumber             = applicationFrameNum;
        m_requestIDToFrameNumber[internalFrameNumIndex].masterCameraID          = mccResult.masterCameraId;
        m_requestIDToFrameNumber[internalFrameNumIndex].pFeature                = NULL;
        m_requestIDToFrameNumber[internalFrameNumIndex].triggerOfflineReprocess = FALSE;
        m_requestIDToFrameNumber[internalFrameNumIndex].isMetaReturnNeeded      = TRUE;
        m_requestIDToFrameNumber[internalFrameNumIndex].isPreviewReturnNeeded   = hasPreviewStreamRequest(pRequest);;
        m_requestIDToFrameNumber[internalFrameNumIndex].isShutterReturnNeeded   = TRUE;
        m_requestIDToFrameNumber[internalFrameNumIndex].isSnapshotReturnNeeded  = hasSnapshotStreamRequest(pRequest);
        m_requestIDToFrameNumber[internalFrameNumIndex].isSkipPreview           = FALSE;

        result = GenerateRealtimeRequest(&request);
        if (TRUE == isOfflineProcessRequired(pRequest, &m_requestIDToFrameNumber[internalFrameNumIndex]))
        {
            TriggerOfflineRequest(internalFrameNum, pRequest);
        }

    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::NotifyResultsAvailable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::NotifyResultsAvailable(
    const CHICAPTURERESULT* pResult)
{
    // ashvind - this function submits the offline preview request.
    CDKResult result                 = CDKResultSuccess;
    UINT32 pipelineIndex             = pResult->pPrivData->requestIndex;
    UINT32 internalFrameNum          = pResult->frameworkFrameNum;
    UINT32 internalFrameNumIndex     = internalFrameNum %  MaxOutstandingRequests;
    BOOL   hasPreviewStream          = FALSE;
    ChiStreamBuffer* rtPreviewBuffer = NULL;

    if (MAX_REALTIME_PIPELINE <= pipelineIndex)
    {
        CHX_LOG_ERROR("Result: Cannot Accept pipeline index of more than 1 frame = %d", pResult->frameworkFrameNum);
        // Early return
        return CDKResultEFailed;
    }

    //Update pending results for camera to deactivate if any.
    for (UINT32 i = 0; i < MAX_REALTIME_PIPELINE; i++)
    {
        if ((0 != m_pendingRequest[i].requestId) && (i == pipelineIndex))
        {
            CHX_LOG_INFO("Check if requestid matched Pipeline = %d, requID = %d ResultFrame=%d",
                pipelineIndex, m_pendingRequest[i].requestId, internalFrameNum);
            if (internalFrameNum >= m_pendingRequest[i].requestId)
            {
                CHX_LOG_INFO("All the results are done and so setting true to deactivate pipeline ");
                m_pendingRequest[i].isReturnedResult = TRUE;
            }
        }
    }


    for (UINT32 i = 0; i < pResult->numOutputBuffers; i++)
    {
        if (IsRTPreviewStream(pResult->pOutputBuffers[i].pStream))
        {
            hasPreviewStream = TRUE;
            rtPreviewBuffer  = const_cast<ChiStreamBuffer *>(&pResult->pOutputBuffers[i]);
        }
    }

    if (NULL != pResult->pResultMetadata)
    {
        // Receive Realtime pipeline result
        if (REALTIMEPIPELINEMAIN == pipelineIndex)
        {
            rtBuffer[internalFrameNumIndex].pMetadata[pipelineIndex] =
                FillMetadataForRDIQueue(internalFrameNum, pipelineIndex,
                    static_cast<const camera_metadata_t*>(pResult->pResultMetadata));

            rtBuffer[internalFrameNumIndex].valid[pipelineIndex]      |= META_READY_FLAG;
        }
        else if (REALTIMEPIPELINEAUX == pipelineIndex)
        {
            rtBuffer[internalFrameNumIndex].pMetadata[pipelineIndex] =
                FillMetadataForRDIQueue(internalFrameNum, pipelineIndex,
                    static_cast<const camera_metadata_t*>(pResult->pResultMetadata));

            rtBuffer[internalFrameNumIndex].valid[pipelineIndex]      |= META_READY_FLAG;
        }

        CHX_LOG("Preview pipeline idx:%d meta is ready,value:%x", pipelineIndex,
            rtBuffer[internalFrameNumIndex].valid[pipelineIndex]);
    }

    if ((TRUE == hasPreviewStream) && (NULL != rtPreviewBuffer))
    {
        // Receive Realtime pipeline result
        if (REALTIMEPIPELINEMAIN == pipelineIndex)
        {
            rtBuffer[internalFrameNumIndex].frameNumber = internalFrameNum;
            if (pResult->numOutputBuffers > 0)
            {
                ChxUtils::Memcpy(&rtBuffer[internalFrameNumIndex].buffer[pipelineIndex],
                    rtPreviewBuffer,
                    sizeof(CHISTREAMBUFFER));
                m_pPreviewBufferManager[pipelineIndex]->ReleaseReference(
                    reinterpret_cast<buffer_handle_t*>(rtPreviewBuffer->phBuffer));
                rtBuffer[internalFrameNumIndex].buffer[pipelineIndex].pStream = m_pRTBPreviewStream[REALTIMEPIPELINEMAIN];
                rtBuffer[internalFrameNumIndex].valid[pipelineIndex]         |= BUF_READY_FLAG;;
            }
        }
        else if (REALTIMEPIPELINEAUX == pipelineIndex)
        {
            rtBuffer[internalFrameNumIndex].frameNumber = internalFrameNum;
            if (pResult->numOutputBuffers > 0)
            {
                ChxUtils::Memcpy(&rtBuffer[internalFrameNumIndex].buffer[pipelineIndex],
                    rtPreviewBuffer,
                    sizeof(CHISTREAMBUFFER));
                m_pPreviewBufferManager[pipelineIndex]->ReleaseReference(
                    reinterpret_cast<buffer_handle_t*>(rtPreviewBuffer->phBuffer));
                rtBuffer[internalFrameNumIndex].buffer[pipelineIndex].pStream = m_pRTBPreviewStream[REALTIMEPIPELINEAUX];
                rtBuffer[internalFrameNumIndex].valid[pipelineIndex]         |= BUF_READY_FLAG;
            }
        }
        CHX_LOG("Preview pipeline idx:%d buffer is ready,value:%x", pipelineIndex,
            rtBuffer[internalFrameNumIndex].valid[pipelineIndex]);
    }

    // trigger reprocessing
    camera3_stream_buffer_t output[2];
    CHICAPTUREREQUEST request   = {0};
    UINT32 buffercount          = 0;
    output[buffercount]         =  m_appPreviewBuffer[internalFrameNumIndex]; // TARGET_PREVIEW
    buffercount++;
    if (TRUE == videoRequest[internalFrameNumIndex])
    {
        output[buffercount] =  m_appVideoBuffer[internalFrameNumIndex]; // TARGET_VIDEO
        buffercount++;
    }

    request.frameNumber             = rtBuffer[internalFrameNumIndex].frameNumber;
    request.hPipelineHandle         = reinterpret_cast<CHIPIPELINEHANDLE>(
                                            m_pSession[OFFLINESESSIONIDX]->GetPipelineHandle(0));
    request.numOutputs              = buffercount;
    request.pOutputBuffers          = reinterpret_cast<CHISTREAMBUFFER*>(
                                            output);
    request.pPrivData               = &privPreviewOffline[internalFrameNumIndex];
        request.pPrivData->streamIndex  = OFFLINEPREVIEWPIPELINE;

    ///< Set number of inputs depending on pipeline active or not
    if ((TRUE == rtRequest[internalFrameNumIndex].prevRequest[REALTIMEPIPELINEMAIN]) &&
        (TRUE == rtRequest[internalFrameNumIndex].prevRequest[REALTIMEPIPELINEAUX]))
    {
        ///< if two result are ready, send offline request
        if ((rtBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] & META_READY_FLAG) &&
            (rtBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] & BUF_READY_FLAG) &&
            (rtBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX] & BUF_READY_FLAG) &&
            (rtBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX] & META_READY_FLAG))
        {
            request.numInputs                          = 2;
            request.pInputBuffers                      = rtBuffer[internalFrameNumIndex].buffer;

            AppendOfflineMetadata(pResult->frameworkFrameNum,
                    rtBuffer[internalFrameNumIndex].pMetadata[0],
                    rtBuffer[internalFrameNumIndex].pMetadata[1],
                    m_pOfflinePipelineInputMetadataPreview,
                    FALSE);

            request.pMetadata                          = m_pOfflinePipelineInputMetadataPreview;

            CHX_LOG("OfflinePreview: both buffers for reprocessing frameNum = %" PRIu64 " buffercount %d",
                request.frameNumber, buffercount);

            GenerateInternalRequest(OFFLINESESSIONIDX, 1, &request);

            rtBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] = 0;
            rtBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX]  = 0;
        }
    }
    else if (TRUE == rtRequest[internalFrameNumIndex].prevRequest[pipelineIndex])
    {
        /// < single Pipeline is active
        if ((rtBuffer[internalFrameNumIndex].valid[pipelineIndex] & META_READY_FLAG)  &&
            (rtBuffer[internalFrameNumIndex].valid[pipelineIndex] & BUF_READY_FLAG))
        {
            request.numInputs                          = 1;
            request.pInputBuffers                      = &rtBuffer[internalFrameNumIndex].buffer[pipelineIndex];

            AppendOfflineMetadata(pResult->frameworkFrameNum,
                    rtBuffer[internalFrameNumIndex].pMetadata[pipelineIndex],
                    NULL,
                    m_pOfflinePipelineInputMetadataPreview,
                    FALSE);

            request.pMetadata                          = m_pOfflinePipelineInputMetadataPreview;

            CHX_LOG("OfflinePreview: single buffer for reprocessing frameNum = %" PRIu64 " buffercount %d",
                request.frameNumber, buffercount);

            GenerateInternalRequest(OFFLINESESSIONIDX, 1, &request);

            rtBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEMAIN] = 0;
            rtBuffer[internalFrameNumIndex].valid[REALTIMEPIPELINEAUX]  = 0;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::ProcessResults
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::ProcessResults()
{

}

CDKResult UsecaseMultiCamera::GenerateInternalRequest(
    UINT32 sessionId, UINT numRequest, CHICAPTUREREQUEST* pRequest)
{
    CDKResult         result = CDKResultSuccess;

    CHIPIPELINEREQUEST submitRequest;

    for (UINT32 i = 0; i < numRequest; i++)
    {
        CHX_LOG("Request SessionID %d, i %d, PipelineIndex %d, frame %" PRIu64 ", numInputs %d, numOutputs %d",
            sessionId, i, pRequest[i].pPrivData->streamIndex,
            pRequest[i].frameNumber, pRequest[i].numInputs, pRequest[i].numOutputs);
    }
    submitRequest.pSessionHandle   = reinterpret_cast<CHIHANDLE>(m_pSession[sessionId]->GetSessionHandle());
    submitRequest.numRequests      = numRequest;
    submitRequest.pCaptureRequests = pRequest;
    result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::GeneratePreviewRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::GenerateRealtimeRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult result              = CDKResultSuccess;
    controller_result_t mccResult = m_pMultiCamController->GetResult();

    camera3_stream_buffer_t mainBuffer[3];
    camera3_stream_buffer_t auxBuffer[3];
    camera3_stream_buffer_t *buffer = NULL;
    UINT32 camID                    = 0;
    UINT32 numMainBufs              = 0;
    UINT32 numAuxBufs               = 0;
    BOOL   LPMDecisionChanged       = FALSE;

    CHICAPTUREREQUEST chiRequest[2] = { {0}, {0} };
    CHICAPTUREREQUEST *request      = NULL;

    // first we need to store request output;
    UINT32 internalFrameNum      = pRequest->frame_number;
    UINT32 internalFrameNumIndex = internalFrameNum % MaxOutstandingRequests;

    // Check whether previous and current mcc result has any change to send the DC vendor tag settings in metadata
    if ((m_prevMCCResult.activeCameras[0].isActive != mccResult.activeCameras[0].isActive) ||
        (m_prevMCCResult.activeCameras[1].isActive != mccResult.activeCameras[1].isActive) ||
        (m_prevMCCResult.masterCameraId != mccResult.masterCameraId))
    {
        LPMDecisionChanged = TRUE;

        CHX_LOG("LPMDecisionChanged previous state (%d, %d) current state (%d, %d) previous master %d, current master %d",
            m_prevMCCResult.activeCameras[0].isActive, m_prevMCCResult.activeCameras[1].isActive,
            mccResult.activeCameras[0].isActive, mccResult.activeCameras[1].isActive,
            m_prevMCCResult.masterCameraId, mccResult.masterCameraId);
    }

    // Reset previous MCC result with latest result
    m_prevMCCResult = mccResult;

    if ((NULL != pRequest->settings) || (TRUE == LPMDecisionChanged))
    {
        CHX_LOG("settings not NULL or LPMDecisionChanged, internalFrameNum %d", internalFrameNum);
        // Translate request settings
        multicam_req_settings multiCamSettings;
        multiCamSettings.numSettingsBuffers = DualCamCount;
        multiCamSettings.kernelFrameSyncEnable = m_kernelFrameSyncEnable;
        multiCamSettings.ppSettings    = static_cast<VOID**>(CHX_CALLOC(multiCamSettings.numSettingsBuffers * sizeof(VOID*)));
        if ((NULL != pRequest->settings) && (NULL != multiCamSettings.ppSettings))
        {
            multiCamSettings.ppSettings[0]  = static_cast<VOID*>(const_cast<camera_metadata_t*>(pRequest->settings));
            multiCamSettings.ppSettings[1]  = allocate_copy_camera_metadata_checked(pRequest->settings,
                get_camera_metadata_size(pRequest->settings));
        }
        else
        {
            if (NULL != multiCamSettings.ppSettings)
            {
                multiCamSettings.ppSettings[0]  =
                    static_cast<VOID*>(const_cast<camera_metadata_t*>(m_pStickyMetadata[REALTIMEPIPELINEMAIN]));
                multiCamSettings.ppSettings[1]  =
                    static_cast<VOID*>(const_cast<camera_metadata_t*>(m_pStickyMetadata[REALTIMEPIPELINEAUX]));
            }
        }
        multiCamSettings.frameNum           = pRequest->frame_number;
        // Send current mcc result which ensures same master & active camera info throught out the life of this request
        multiCamSettings.currentRequestMCCResult = mccResult;

        if (CDKResultSuccess != m_pMultiCamController->TranslateRequestSettings(&multiCamSettings))
        {
            CHX_LOG_ERROR("Error in translating request settings");
        }
        else
        {
            // Merge metadata only if settings are not null
            if ((NULL != pRequest->settings) && (NULL != multiCamSettings.ppSettings))
            {
                ChxUtils::MergeMetadata(multiCamSettings.ppSettings[REALTIMEPIPELINEMAIN],
                    static_cast<VOID*>(m_pStickyMetadata[REALTIMEPIPELINEMAIN]));
                ChxUtils::MergeMetadata(multiCamSettings.ppSettings[REALTIMEPIPELINEAUX],
                    static_cast<VOID*>(m_pStickyMetadata[REALTIMEPIPELINEAUX]));
            }
        }

        // Free the allocated meta data which is used for translated settings
        if ((NULL != pRequest->settings) && (NULL != multiCamSettings.ppSettings) && (NULL != multiCamSettings.ppSettings[REALTIMEPIPELINEAUX]))
        {
            free_camera_metadata(static_cast<camera_metadata_t*>(multiCamSettings.ppSettings[REALTIMEPIPELINEAUX]));
        }
    }

    /// Initialize postview, videoRequest and rtRequest
    postviewRequest[internalFrameNumIndex]                              = FALSE;
    videoRequest[internalFrameNumIndex]                                 = FALSE;

    rtRequest[internalFrameNumIndex] = { 0 };
    rtRequest[internalFrameNumIndex].masterPipelineIndex =
        m_pSession[REALTIMESESSIONIDX]->GetPipelineIndex(mccResult.masterCameraId);

    CHX_ASSERT(rtRequest[internalFrameNumIndex].masterPipelineIndex < MAX_REALTIME_PIPELINE); // PipelineIndex should be 0 or 1

    for (UINT32 i = 0; i < MAX_REALTIME_PIPELINE; i++)
    {
        if (m_pSession[REALTIMESESSIONIDX]->GetCameraId(REALTIMEPIPELINEMAIN) ==
            mccResult.activeCameras[i].cameraId)
        {
            rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEMAIN] = mccResult.activeCameras[i].isActive;
        }
        if (m_pSession[REALTIMESESSIONIDX]->GetCameraId(REALTIMEPIPELINEAUX)  ==
            mccResult.activeCameras[i].cameraId)
        {
            rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEAUX] = mccResult.activeCameras[i].isActive;
        }

        // when feature or flash is enabled, keep only one(master) pipeline in active status and
        // also disable the fusion
        if ((NULL != m_requestIDToFrameNumber[internalFrameNumIndex].pFeature) ||
            (TRUE == m_isFlashRequired))
        {
            UINT32 masterPipelineIndex = rtRequest[internalFrameNumIndex].masterPipelineIndex;
            if (REALTIMEPIPELINEMAIN == masterPipelineIndex)
            {
                if (TRUE == rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEMAIN])
                {
                    rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEAUX] = FALSE;
                }
            }
            else
            {
                if (TRUE == rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEAUX])
                {
                    rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEMAIN] = FALSE;
                }
            }
            // disable the fusion
            mccResult.snapshotFusion = FALSE;
        }
        CHX_LOG("camera id %d, is active %d, main id %d, aux id %d",
            mccResult.activeCameras[i].cameraId, mccResult.activeCameras[i].isActive,
            m_pSession[REALTIMESESSIONIDX]->GetCameraId(REALTIMEPIPELINEMAIN),
            m_pSession[REALTIMESESSIONIDX]->GetCameraId(REALTIMEPIPELINEAUX));
    }

    CHX_LOG("snapshotFusion %d, masterPipelineIndex %d",
        mccResult.snapshotFusion, rtRequest[internalFrameNumIndex].masterPipelineIndex);
    CHX_LOG("active request:%d,%d,frameNUM:%d",rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEMAIN],
        rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEAUX], internalFrameNum);

    camID = REALTIMEPIPELINEMAIN;

    // if snapshot session is not created or created is not done.
    if (FALSE == m_deferSnapshotSessionDone)
    {
        if (hasSnapshotStreamRequest(pRequest))
        {
            m_pDeferSnapDoneMutex->Lock();
            /// wait snapshot session create done
            while (FALSE == m_deferSnapshotSessionDone)
            {
                m_pDeferSnapDoneCondition->Wait(m_pDeferSnapDoneMutex->GetNativeHandle());
            }
            m_pDeferSnapDoneMutex->Unlock();
        }
    }

    ActivateDeactivateRealtimePipeline(mccResult.activeCameras, internalFrameNum);

    m_appWideJpegBuffer[internalFrameNumIndex].buffer = NULL;

    for( UINT32 stream = 0 ; stream < pRequest->num_output_buffers; stream++){
        CHX_LOG("pRequest->output_buffers[stream].stream = %p stream = %d", pRequest->output_buffers[stream].stream, stream);
        if ((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetPreviewStream)
        {
            ChxUtils::Memcpy(&m_appPreviewBuffer[internalFrameNumIndex],
                &pRequest->output_buffers[stream], sizeof(camera3_stream_buffer_t));
            if (TRUE == rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEMAIN])
            {
                CHX_LOG("Preview real time request for Main= %p", m_pRTPreviewStream[camID]);
                buffer = &mainBuffer[numMainBufs];
                buffer->acquire_fence = -1;
                buffer->buffer = m_pPreviewBufferManager[camID]->GetImageBuffer()->GetBufferHandle();
                buffer->stream = reinterpret_cast<camera3_stream_t*>(m_pRTPreviewStream[camID]);
                numMainBufs++;

                /// always request RDI buffer for realtime request
                CHX_LOG("RDI RAW MIPI 10 Real time request for Main = %p", m_pRTRDIStream[camID]);
                buffer                = &mainBuffer[numMainBufs];
                GetOutputBufferFromRDIQueue(camID, buffer);
                numMainBufs++;

                rtRequest[internalFrameNumIndex].prevRequest[REALTIMEPIPELINEMAIN] = 1;
            }
        }
        else if ((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetSnapshotStream)
        {

            ChxUtils::Memcpy(&m_appSnapshotBuffer[internalFrameNumIndex],
                &pRequest->output_buffers[stream], sizeof(camera3_stream_buffer_t));
            if (((TRUE == mccResult.snapshotFusion) ||
                (REALTIMEPIPELINEMAIN == rtRequest[internalFrameNumIndex].masterPipelineIndex)) &&
                (TRUE == rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEMAIN]))
            {
                CHX_LOG("Snapshot acquire = %d release = %d", m_appSnapshotBuffer[internalFrameNumIndex].acquire_fence,
                    m_appSnapshotBuffer[internalFrameNumIndex].release_fence);

                /// for video liveshot usecase, it will not request preview stream when request snapshot
                if (1 == pRequest->num_output_buffers)
                {
                    CHX_LOG("RDI RAW MIPI 10 Real time request for Main = %p", m_pRTRDIStream[camID]);
                    buffer                = &mainBuffer[numMainBufs];
                    GetOutputBufferFromRDIQueue(camID, buffer);
                    numMainBufs++;
                }

                rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEMAIN] = 1;

            }
        }
        else if ((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetWideJpegStream)
        {
            ChxUtils::Memcpy(&m_appWideJpegBuffer[internalFrameNumIndex],
                &pRequest->output_buffers[stream], sizeof(camera3_stream_buffer_t));
        }
        else if (((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetRAW16Stream))
        {
            CHX_LOG("RAW16 Requested = %p", m_pTargetRAW16Stream);
            ChxUtils::Memcpy(&m_appRAW16Buffer[internalFrameNumIndex],
                &pRequest->output_buffers[stream], sizeof(camera3_stream_buffer_t));

            if(UsecaseSAT == m_dualCameraUsecase)
            {
                CHX_LOG("RDI 16 Real time request for Main = %p", m_pRTRaw16Stream[camID]);
                buffer                = &mainBuffer[numMainBufs];
                buffer->acquire_fence = -1;
                buffer->buffer        = m_pRDI16BufferManager[camID]->GetImageBuffer()->GetBufferHandle();
                buffer->stream        = reinterpret_cast<camera3_stream_t*>(m_pRTRaw16Stream[camID]);
                numMainBufs++;
                rtRequest[internalFrameNumIndex].rawRequest[REALTIMEPIPELINEMAIN] = 1;
            }
        }
        else if (((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetPostviewStream))
        {
            CHX_LOG("Thumbnail Requested = %p", m_pTargetPostviewStream);
            ChxUtils::Memcpy(&m_appPostviewBuffer[internalFrameNumIndex],
                &pRequest->output_buffers[stream], sizeof(camera3_stream_buffer_t));
            CHX_LOG("Thumbnail acquire = %d release = %d", m_appPostviewBuffer[internalFrameNumIndex].acquire_fence,
                m_appPostviewBuffer[internalFrameNumIndex].release_fence);
            postviewRequest[internalFrameNumIndex] = 1;
        }
        else if ((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetVideoStream)
        {
            CHX_LOG("Video Requested = %p", m_pTargetVideoStream);
            ChxUtils::Memcpy(&m_appVideoBuffer[internalFrameNumIndex],
                &pRequest->output_buffers[stream], sizeof(camera3_stream_buffer_t));
            CHX_LOG("Video acquire = %d release = %d", m_appVideoBuffer[internalFrameNumIndex].acquire_fence,
                m_appVideoBuffer[internalFrameNumIndex].release_fence);
            videoRequest[internalFrameNumIndex] = 1;
        }
        else if (((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetRDIStream) &&
            (m_pTargetRDIStream == m_pRTRDIStream[REALTIMEPIPELINEMAIN]))
        {
            if ((TRUE == mccResult.snapshotFusion) ||
                (REALTIMEPIPELINEMAIN == rtRequest[internalFrameNumIndex].masterPipelineIndex))
            {
                buffer                = &mainBuffer[numMainBufs];
                GetOutputBufferFromRDIQueue(camID, buffer);
                numMainBufs++;

                rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEMAIN] = 1;
            }
        }
        else
        {
            CHX_LOG("New stream???");
        }
    }

    if (NULL != pRequest->settings)
    {
        UINT32 sensorModeIndex = m_pSession[REALTIMESESSIONIDX]->GetSensorModeInfo(camID)->modeIndex;
        ChxUtils::FillTuningModeData(const_cast<VOID*>(reinterpret_cast<const VOID*>(m_pStickyMetadata[REALTIMEPIPELINEMAIN])),
            pRequest, sensorModeIndex, &m_effectModeValue[camID], &m_sceneModeValue[camID],
            &m_tuningFeature1Value[camID], &m_tuningFeature2Value[camID]);
    }

    request                          = &chiRequest[camID];
    request->frameNumber             = internalFrameNum;
    request->hPipelineHandle         = reinterpret_cast<CHIPIPELINEHANDLE>(
                                       m_pSession[REALTIMESESSIONIDX]->GetPipelineHandle(camID));
    request->numOutputs              = numMainBufs;
    request->pOutputBuffers          = reinterpret_cast<CHISTREAMBUFFER*>(
                                       const_cast<camera3_stream_buffer*>(mainBuffer));
    request->pMetadata               = pRequest->settings;
    ///< Add sticky if app settings is not NULL or if its 1st request after wakeup or LPM decision changed
    if ((NULL != pRequest->settings) || (TRUE == m_stickyMetaNeeded[REALTIMEPIPELINEMAIN]) || (TRUE == LPMDecisionChanged))
    {
        request->pMetadata           = m_pStickyMetadata[REALTIMEPIPELINEMAIN];
    }
    request->pPrivData               = &privRealTime1[internalFrameNumIndex % MaxOutstandingRequests];
    request->pPrivData->streamIndex  = REALTIMEPIPELINEMAIN;

    ///< generate request for realtime1
    camID = REALTIMEPIPELINEAUX;

    for (UINT32 stream = 0; stream < pRequest->num_output_buffers; stream++) {
        if ((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetPreviewStream)
        {
            if (TRUE == rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEAUX])
            {
                CHX_LOG("Preview real time request for Aux= %p", m_pRTPreviewStream[camID]);
                buffer                = &auxBuffer[numAuxBufs];
                buffer->acquire_fence = -1;
                buffer->buffer        = m_pPreviewBufferManager[camID]->GetImageBuffer()->GetBufferHandle();
                buffer->stream        = reinterpret_cast<camera3_stream_t*>(m_pRTPreviewStream[camID]);
                numAuxBufs++;

                CHX_LOG("RDI RAW MIPI 10 Real time request for Aux= %p", m_pRTRDIStream[camID]);
                buffer                = &auxBuffer[numAuxBufs];
                GetOutputBufferFromRDIQueue(camID, buffer);
                numAuxBufs++;

                rtRequest[internalFrameNumIndex].prevRequest[REALTIMEPIPELINEAUX] = 1;
            }
        }
        else if ((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetSnapshotStream)
        {
            if (((TRUE == mccResult.snapshotFusion) ||
                (REALTIMEPIPELINEAUX == rtRequest[internalFrameNumIndex].masterPipelineIndex)) &&
                (TRUE == rtRequest[internalFrameNumIndex].isActive[REALTIMEPIPELINEAUX]))
            {
                /// for video liveshot usecase, it will not request preview stream when request snapshot
                if (1 == pRequest->num_output_buffers)
                {
                    CHX_LOG("RDI RAW MIPI 10 Real time request for Aux= %p", m_pRTRDIStream[camID]);
                    buffer                = &auxBuffer[numAuxBufs];
                    GetOutputBufferFromRDIQueue(camID, buffer);
                    numAuxBufs++;
                }

                rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEAUX] = 1;
            }
        }
        else if (((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetRAW16Stream))
        {
            if(UsecaseSAT == m_dualCameraUsecase)
            {
                CHX_LOG("RDI 16 Real time request for Aux= %p", m_pRTRaw16Stream[camID]);
                buffer                = &auxBuffer[numAuxBufs];
                buffer->acquire_fence = -1;
                buffer->buffer        = m_pRDI16BufferManager[camID]->GetImageBuffer()->GetBufferHandle();
                buffer->stream        = reinterpret_cast<camera3_stream_t*>(m_pRTRaw16Stream[camID]);
                numAuxBufs++;

                rtRequest[internalFrameNumIndex].rawRequest[REALTIMEPIPELINEAUX] = 1;
            }
        }
        else if (((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetPostviewStream))
        {
            CHX_LOG("Thumbnail Requested = %p", m_pTargetPostviewStream);
        }
        else if ((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetVideoStream)
        {
            CHX_LOG("Video Requested = %p", m_pTargetVideoStream);
        }
        else if (((CHISTREAM *)pRequest->output_buffers[stream].stream == m_pTargetRDIStream) &&
            (m_pTargetRDIStream == m_pRTRDIStream[REALTIMEPIPELINEAUX]))
        {
            if ((TRUE == mccResult.snapshotFusion) ||
                (REALTIMEPIPELINEAUX == rtRequest[internalFrameNumIndex].masterPipelineIndex))
            {
                CHX_LOG("RDI Real time request for Aux= %p", m_pRTRDIStream[camID]);

                buffer                = &auxBuffer[numAuxBufs];
                GetOutputBufferFromRDIQueue(camID, buffer);
                numAuxBufs++;

                rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEAUX] = 1;

            }
        }
        else
        {
            CHX_LOG("New stream???");
        }
    }

    if (NULL != pRequest->settings)
    {
        UINT32 sensorModeIndex = m_pSession[REALTIMESESSIONIDX]->GetSensorModeInfo(camID)->modeIndex;
        ChxUtils::FillTuningModeData(const_cast<VOID*>(reinterpret_cast<const VOID*>(m_pStickyMetadata[REALTIMEPIPELINEAUX])),
            pRequest, sensorModeIndex, &m_effectModeValue[camID], &m_sceneModeValue[camID],
            &m_tuningFeature1Value[camID], &m_tuningFeature2Value[camID]);
    }

    request                         = &chiRequest[camID];
    request->frameNumber            = internalFrameNum;
    request->hPipelineHandle        = reinterpret_cast<CHIPIPELINEHANDLE>(
        m_pSession[REALTIMESESSIONIDX]->GetPipelineHandle(camID));
    request->numOutputs             = numAuxBufs;
    request->pOutputBuffers         = reinterpret_cast<CHISTREAMBUFFER*>(
        const_cast<camera3_stream_buffer*>(auxBuffer));
    request->pMetadata              = pRequest->settings;
    ///< Add sticky if app settings is not NULL or if its 1st request after wakeup or LPM decision changed
    if ((NULL != pRequest->settings) || (TRUE == m_stickyMetaNeeded[REALTIMEPIPELINEAUX]) || (TRUE == LPMDecisionChanged))
    {
        request->pMetadata          = m_pStickyMetadata[REALTIMEPIPELINEAUX];
    }
    request->pPrivData              = &privRealTime2[internalFrameNum % MaxOutstandingRequests];
    request->pPrivData->streamIndex = REALTIMEPIPELINEAUX;

    ///< send two request together
    if ((0 < numMainBufs) && (0 < numAuxBufs))
    {
        ///< send two request together
        CHX_LOG("send both the request numMainBufs %d numAuxBufs %d internalFrameNum %d",
            numMainBufs, numAuxBufs, internalFrameNum);
        result = GenerateInternalRequest(REALTIMESESSIONIDX, MAX_REALTIME_PIPELINE, &chiRequest[0]);
    }
    else if (0 < numMainBufs)
    {
        ///< send only the REALTIMEPIPELINEMAIN
        CHX_LOG("send only the MAIN request numMainBufs %d internalFrameNum %d", numMainBufs, internalFrameNum);
        result = GenerateInternalRequest(REALTIMESESSIONIDX, 1, &chiRequest[REALTIMEPIPELINEMAIN]);
    }
    else if (0 < numAuxBufs)
    {
        ///< send only the REALTIMEPIPELINEAUX
        CHX_LOG("send only the AUX request numAuxBufs %d internalFrameNum %d", numAuxBufs, internalFrameNum);
        result = GenerateInternalRequest(REALTIMESESSIONIDX, 1, &chiRequest[REALTIMEPIPELINEAUX]);
    }
    else
    {
        CHX_LOG("should not come here");
    }

    ///< Reset the MAIN sticky metadata if its already sent in the request
    if (numMainBufs)
    {
        ChxUtils::ResetMetadata(m_pStickyMetadata[REALTIMEPIPELINEMAIN]);
        m_stickyMetaNeeded[REALTIMEPIPELINEMAIN] = FALSE;
    }

    ///< Reset the AUX sticky metadata if its already sent in the request
    if (numAuxBufs)
    {
        ChxUtils::ResetMetadata(m_pStickyMetadata[REALTIMEPIPELINEAUX]);
        m_stickyMetaNeeded[REALTIMEPIPELINEAUX] = FALSE;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::IsRdiStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseMultiCamera::IsRdiStream(
        CHISTREAM* pStream) const                ///< Stream to check
{
    BOOL isRDI = FALSE;
    for (UINT32 i = 0; i < MAX_REALTIME_PIPELINE; i++)
    {
        if (m_pRTRDIStream[i] == pStream)
        {
            isRDI = TRUE;
            break;
        }
    }
    return isRDI;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::IsRdiRAW16Stream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseMultiCamera::IsRdiRAW16Stream(
        CHISTREAM* pStream) const                ///< Stream to check
{
    BOOL isRDI = FALSE;
    for (UINT32 i = 0; i < MAX_REALTIME_PIPELINE; i++)
    {
        if (m_pRTRaw16Stream[i] == pStream)
        {
            isRDI = TRUE;
            break;
        }
    }
    return isRDI;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::IsRTPreviewStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseMultiCamera::IsRTPreviewStream(
        CHISTREAM* pStream) const                ///< Stream to check
{
    BOOL isRTPreview = FALSE;
    for (UINT32 i = 0; i < MAX_REALTIME_PIPELINE; i++)
    {
        if (m_pRTPreviewStream[i] == pStream)
        {
            isRTPreview = TRUE;
            break;
        }
    }
    return isRTPreview;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::CreateOfflineProcessResource
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseMultiCamera::CreateOfflineProcessResource()
{
    CDKResult result = CDKResultSuccess;
    if (NULL == m_offlineRequestProcessThread.pPrivateData)
    {
        m_offlineRequestProcessThread.pPrivateData = this;

        m_pOfflineRequestMutex           = Mutex::Create();
        m_pOfflineRequestAvailable       = Condition::Create();
        m_offlineRequestProcessTerminate = FALSE;

        result = ChxUtils::ThreadCreate(UsecaseMultiCamera::OfflineRequestThread,
                                        &m_offlineRequestProcessThread,
                                        &m_offlineRequestProcessThread.hThreadHandle);
        if (CDKResultSuccess != result)
        {
            m_offlineRequestProcessThread = {0};
            if (NULL != m_pOfflineRequestMutex)
            {
                m_pOfflineRequestMutex->Destroy();
                m_pOfflineRequestMutex = NULL;
            }

            if (NULL != m_pOfflineRequestAvailable)
            {
                m_pOfflineRequestAvailable->Destroy();
                m_pOfflineRequestAvailable = NULL;
            }
        }
    }
    return result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseMultiCamera::DestroyOfflineProcessResource
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::DestroyOfflineProcessResource()
{
    if (NULL != m_offlineRequestProcessThread.pPrivateData)
    {
        m_pOfflineRequestMutex->Lock();
        m_offlineRequestProcessTerminate = TRUE;
        m_pOfflineRequestAvailable->Signal();
        m_pOfflineRequestMutex->Unlock();

        ChxUtils::ThreadTerminate(m_offlineRequestProcessThread.hThreadHandle);
        m_offlineRequestProcessThread = {0};
    }

    if (NULL != m_pOfflineRequestMutex)
    {
        m_pOfflineRequestMutex->Destroy();
        m_pOfflineRequestMutex = NULL;
    }

    if (NULL != m_pOfflineRequestAvailable)
    {
        m_pOfflineRequestAvailable->Destroy();
        m_pOfflineRequestAvailable = NULL;
    }

    //avoid potential memoryleak.
    for (UINT32 i = 0 ; i < MaxOutstandingRequests; i++)
    {
        if (NULL != m_pAppSnapshotMetadata[i])
        {
            free_camera_metadata(m_pAppSnapshotMetadata[i]);
            m_pAppSnapshotMetadata[i] = NULL;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseMultiCamera::RequestThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* UsecaseMultiCamera::OfflineRequestThread(
    VOID* pThreadData)
{
    PerThreadData* pPerThreadData = reinterpret_cast<PerThreadData*>(pThreadData);

    UsecaseMultiCamera* pMultiCamera = reinterpret_cast<UsecaseMultiCamera*>(pPerThreadData->pPrivateData);

    pMultiCamera->OfflineRequestProcess();

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseMultiCamera::OfflineRequestProcess
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::OfflineRequestProcess()
{
    CDKResult result = CDKResultSuccess;
    CHX_LOG_INFO("OfflineRequestProcess Entered");
    while (TRUE)
    {
        m_pOfflineRequestMutex->Lock();
        if (m_offlineRequestProcessTerminate == FALSE)
        {
            m_pOfflineRequestAvailable->TimedWait(m_pOfflineRequestMutex->GetNativeHandle(), 500);
        }
        m_pOfflineRequestMutex->Unlock();

        if (m_offlineRequestProcessTerminate == TRUE)
        {
            break;
        }

        while ((m_maxSnapshotReqId > m_snapshotReqId) && (FALSE == m_offlineRequestProcessTerminate))
        {
            UINT   snapshotReqIdIndex     = (m_snapshotReqId % MaxOutstandingRequests);
            UINT32 internalFrameNum       = m_snapshotRequestInfo[snapshotReqIdIndex].frameNumber;
            UINT32 internalFrameNumIndex  = internalFrameNum % MaxOutstandingRequests;
            RequestMapInfo requestMapInfo = m_requestIDToFrameNumber[internalFrameNumIndex];
            UINT32 pipelineIndex          = requestMapInfo.activePipelineID;
            UINT32 inputFrameNum          = m_snapshotRequestInfo[snapshotReqIdIndex].inputRDIFrameNumber;
            CDKResult result              = CDKResultSuccess;

            CHX_LOG_INFO("send offline request:%d, snapshotReqId:%d", internalFrameNum, m_snapshotReqId);

            if ((NULL == m_requestIDToFrameNumber[internalFrameNumIndex].pFeature))
            {

                if (TRUE == rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEMAIN])
                {
                    do
                    {
                        result = WaitForBufferMetaReady(inputFrameNum, REALTIMEPIPELINEMAIN);

                        if ((CDKResultETimeout == result) && ((TRUE == m_offlineRequestProcessTerminate) ||
                            (TRUE == m_flushLock)))
                        {
                            CHX_LOG_ERROR("wait for buffer ready failed: inputFramenubmer:%d, pipelineIndex:%d",
                                inputFrameNum, REALTIMEPIPELINEMAIN);
                            break;
                        }
                    }
                    while (result != CDKResultSuccess);

                    if (CDKResultSuccess == result)
                    {
                        CHX_LOG_INFO("wait for buffer ready:%d, pipelineIndex:%d done",inputFrameNum, REALTIMEPIPELINEMAIN);
                        GenerateSnapshotRequest(inputFrameNum, internalFrameNum, REALTIMEPIPELINEMAIN);
                    }
                }

                if ((CDKResultSuccess == result) &&
                    (TRUE == rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEAUX]))
                {
                    do
                    {
                        result = WaitForBufferMetaReady(inputFrameNum, REALTIMEPIPELINEAUX);

                        // if the thread destroy is called, break;
                        if ((CDKResultETimeout == result) && ((TRUE == m_offlineRequestProcessTerminate) ||
                            (TRUE == m_flushLock)))
                        {
                            CHX_LOG_ERROR("wait for buffer ready failed: inputFramenubmer:%d, pipelineIndex:%d",
                                inputFrameNum, REALTIMEPIPELINEAUX);
                            break;
                        }
                    }
                    while (result != CDKResultSuccess);

                    if (CDKResultSuccess == result)
                    {
                        CHX_LOG_INFO("wait for buffer ready:%d, pipelineIndex:%d,done",inputFrameNum, REALTIMEPIPELINEAUX);
                        GenerateSnapshotRequest(inputFrameNum, internalFrameNum, REALTIMEPIPELINEAUX);
                    }
                }

                // application metadata has been merged into RDI metadata, and will be sent to following pipeline
                // free it
                if (CDKResultSuccess == result)
                {
                    UINT32 snapIdx = m_snapshotReqId % MaxOutstandingRequests;
                    if (NULL != m_pAppSnapshotMetadata[snapIdx])
                    {
                        free_camera_metadata(m_pAppSnapshotMetadata[snapIdx]);
                        m_pAppSnapshotMetadata[snapIdx] = NULL;
                    }
                }

            }
            else if ((NULL != m_requestIDToFrameNumber[internalFrameNumIndex].pFeature) &&
                (TRUE == m_requestIDToFrameNumber[internalFrameNumIndex].triggerOfflineReprocess))
            {

                UINT32 activePipeline = m_requestIDToFrameNumber[internalFrameNumIndex].activePipelineID;
                do
                {
                    result = WaitForBufferMetaReady(inputFrameNum, activePipeline);

                    if ((CDKResultETimeout == result) && ((TRUE == m_offlineRequestProcessTerminate) ||
                        (TRUE == m_flushLock)))
                    {
                        CHX_LOG_ERROR("wait for buffer ready failed: inputFramenubmer:%d, pipelineIndex:%d",
                            inputFrameNum, activePipeline);
                        break;
                    }
                }
                while (result != CDKResultSuccess);

                if (CDKResultSuccess == result)
                {
                    CHX_LOG_INFO("wait for buffer ready:%d, pipelineIndex:%d,done",inputFrameNum, activePipeline);
                    camera3_capture_request_t requestForFeature = { 0 };
                    requestForFeature.frame_number              = internalFrameNum;
                    requestForFeature.input_buffer              = NULL;
                    requestForFeature.num_output_buffers        = 1;
                    requestForFeature.settings                  = NULL;
                    requestForFeature.output_buffers            = &m_appSnapshotBuffer[internalFrameNumIndex];
                    m_requestIDToFrameNumber[internalFrameNumIndex].pFeature->ExecuteProcessRequest(&requestForFeature);

                    rtRequest[internalFrameNumIndex].snapRequest[pipelineIndex]             = FALSE;
                    m_requestIDToFrameNumber[internalFrameNumIndex].pFeature                = NULL;
                    m_requestIDToFrameNumber[internalFrameNumIndex].triggerOfflineReprocess = FALSE;
                }
            }

            if (CDKResultSuccess == result)
            {
                CHX_LOG_INFO("offline request:%d, snapshotReqId:%d is sent!", internalFrameNum, m_snapshotReqId);
                m_snapshotReqId ++;
            }

            if ((CDKResultSuccess != result) && ((TRUE == m_offlineRequestProcessTerminate) || (TRUE == m_flushLock)))
            {
                CHX_LOG_INFO("Set Snap Req ID to Last Snap Req if its in terminating %d or flush is in progress %d",
                    m_offlineRequestProcessTerminate, m_flushLock);
                m_snapshotReqId = m_maxSnapshotReqId;
            }
        }
    }
    CHX_LOG_INFO("OfflineRequestProcess Exited");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseMultiCamera::TriggerOfflineRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::TriggerOfflineRequest(UINT32 requestNum,
    camera3_capture_request_t* pRequest)
{
    CDKResult result = CDKResultSuccess;

    CHX_LOG_INFO("TriggerOffineRequest :%d Entered", requestNum);
    UINT32 snapshotReqIdIndex    = (m_maxSnapshotReqId % MaxOutstandingRequests);
    UINT32 internalFrameNumIndex = requestNum % MaxOutstandingRequests;
    m_pOfflineRequestMutex->Lock();
    m_snapshotRequestInfo[snapshotReqIdIndex] = {0};
    m_snapshotRequestInfo[snapshotReqIdIndex].frameNumber         = requestNum;

    /// snapshot metadata is used for ZSL feature.
    /// Advance Feature will reserve snapshot metadata by themselves.
    if (NULL == m_requestIDToFrameNumber[internalFrameNumIndex].pFeature)
    {
        UINT32 isZSLMode = 0;
        GetZSLMode(&isZSLMode, const_cast<camera_metadata_t*>(pRequest->settings));

        if ((0 != isZSLMode) && (FALSE == m_isFlashRequired))
        {
            if(NULL != m_pAppSnapshotMetadata[snapshotReqIdIndex])
            {
                free_camera_metadata(m_pAppSnapshotMetadata[snapshotReqIdIndex]);
                m_pAppSnapshotMetadata[snapshotReqIdIndex] = NULL;
            }

            m_pAppSnapshotMetadata[snapshotReqIdIndex] =
                allocate_copy_camera_metadata_checked(pRequest->settings, get_camera_metadata_size(pRequest->settings));

            UINT32 lastReadyRDIMain = 0xFFFFFFFF;
            UINT32 lastReadyRDIAux  = 0xFFFFFFFF;

            if (TRUE == rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEMAIN])
            {
                lastReadyRDIMain = GetLastReadyFrameNum(REALTIMEPIPELINEMAIN);
            }

            if (TRUE == rtRequest[internalFrameNumIndex].snapRequest[REALTIMEPIPELINEMAIN])
            {
                lastReadyRDIAux  = GetLastReadyFrameNum(REALTIMEPIPELINEAUX);
            }

            UINT32 lastReadyRDI     =
                ((lastReadyRDIMain < lastReadyRDIAux) ? lastReadyRDIMain : lastReadyRDIAux);

            m_snapshotRequestInfo[snapshotReqIdIndex].inputRDIFrameNumber = lastReadyRDI;
        }
        else
        {
            m_snapshotRequestInfo[snapshotReqIdIndex].inputRDIFrameNumber = requestNum;
        }
    }
    else
    {
       if (TRUE == m_requestIDToFrameNumber[internalFrameNumIndex].triggerOfflineReprocess)
       {
            m_snapshotRequestInfo[snapshotReqIdIndex].inputRDIFrameNumber = requestNum;
            Feature* pFeature = m_requestIDToFrameNumber[internalFrameNumIndex].pFeature;
            UINT32 lastReadyRDI =
                GetLastReadyFrameNum(m_requestIDToFrameNumber[internalFrameNumIndex].activePipelineID);
            //todo: it needs to get last ready RDI metadata to check how many frames is needed
            // currently it is constant
            camera_metadata_t* pLastReadyMetadata = NULL;;
            UINT32 requiredFrameNum = pFeature->GetRequiredFramesForSnapshot(pLastReadyMetadata);

            if ((FeatureType::MFNR == pFeature->GetFeatureType()) && (FALSE == m_isLLSSnapshot))
            {
                if (lastReadyRDI < requiredFrameNum)
                {
                    m_snapshotRequestInfo[snapshotReqIdIndex].inputRDIFrameNumber = requiredFrameNum;
                }
                else
                {
                    m_snapshotRequestInfo[snapshotReqIdIndex].inputRDIFrameNumber = lastReadyRDI;
                }
            }
            else if (FeatureType::SWMF == pFeature->GetFeatureType())
            {
                if (requestNum < requiredFrameNum)
                {
                    m_snapshotRequestInfo[snapshotReqIdIndex].inputRDIFrameNumber = requiredFrameNum;
                }
                else
                {
                    m_snapshotRequestInfo[snapshotReqIdIndex].inputRDIFrameNumber = requestNum;
                }
            }
       }
    }
    m_maxSnapshotReqId++;
    m_pOfflineRequestAvailable->Signal();
    m_pOfflineRequestMutex->Unlock();
    CHX_LOG_INFO("TriggerOffineRequest Exited: maxSnapshotReqId:%d", m_maxSnapshotReqId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseMultiCamera::GenerateSnapshotRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseMultiCamera::GenerateSnapshotRequest(
        UINT32 inputFrameNum,
        UINT32 frameNumber,
        UINT32 pipelineIndex)
{
    UINT32 internalFrameNumIndex           = inputFrameNum % MaxOutstandingRequests;
    camera3_stream_buffer_t RDIInputBuffer = { 0 };
    CHICAPTUREREQUEST request              = {0};
    UINT32         sessionIndex            = OFFLINERDISESSION;
    UINT32         numOutBuffer            = 0;
    camera3_stream_buffer_t output[2];

    if (pipelineIndex == REALTIMEPIPELINEAUX)
    {
        sessionIndex = OFFLINERDISESSIONAUX;
    }

    request.frameNumber              = frameNumber;
    request.hPipelineHandle          = reinterpret_cast<CHIPIPELINEHANDLE>(
        m_pSession[sessionIndex]->GetPipelineHandle(0));
    request.numInputs                = 1;

    numOutBuffer = 1;
    if ((TRUE == postviewRequest[internalFrameNumIndex]) &&
        (pipelineIndex == rtRequest[internalFrameNumIndex].masterPipelineIndex))
    {
        numOutBuffer = 2;
    }

    request.numOutputs               = numOutBuffer;
    output[0].acquire_fence          = -1;
    output[0].buffer                 = m_pYUVBufferManager[pipelineIndex]->GetImageBuffer()->GetBufferHandle();
    output[0].stream                 = reinterpret_cast<camera3_stream_t*>(m_pRTYUVStream[pipelineIndex]);
    if (1 < numOutBuffer)
    {
        output[1] = m_appPostviewBuffer[internalFrameNumIndex];
    }

    request.pOutputBuffers          = reinterpret_cast<CHISTREAMBUFFER*>(output);

    for (UINT i = 0; i < request.numOutputs; i++)
    {
        CHX_LOG("[MulitCamera RDI] Request : frameNum: %" PRIu64 " acquireFence: %d , ReleaseFence: %d Buffer: %p, status: %d",
            request.frameNumber, request.pOutputBuffers[i].acquireFence,
            request.pOutputBuffers[i].releaseFence, request.pOutputBuffers[i].phBuffer,
            request.pOutputBuffers[i].bufferStatus);
    }

    camera_metadata_t *pMetadata = NULL;
    camera_metadata_t *pRDIMetadata = NULL;
    GetInputBufferFromRDIQueue(inputFrameNum, pipelineIndex,
        &RDIInputBuffer,
        &(pRDIMetadata));
    // Update the scaler crop region for snapshot
    m_pMultiCamController->UpdateScalerCropForSnapshot(pRDIMetadata);

    rtSnapshotBuffer[internalFrameNumIndex].pMetadata[pipelineIndex] = pRDIMetadata;
    if (inputFrameNum != frameNumber)
    {
        UINT32 snapIdx = m_snapshotReqId % MaxOutstandingRequests;
        pMetadata = allocate_camera_metadata(ReplacedMetadataEntryCapacity, ReplacedMetadataDataCapacity);
        if (NULL != pMetadata)
        {
            ChxUtils::MergeMetadata(m_pAppSnapshotMetadata[snapIdx], pMetadata);
            ChxUtils::MergeMetadata(pRDIMetadata, pMetadata);
            ChxUtils::MergeMetadata(pMetadata, pRDIMetadata);
        }
        else
        {
            CHX_LOG("pMetadata alloc failed,no Memory");
        }
        request.pMetadata     = pMetadata;
    }
    else
    {
        request.pMetadata     = pRDIMetadata;
    }

    request.pInputBuffers = reinterpret_cast<CHISTREAMBUFFER*>(&RDIInputBuffer);;

    if (pipelineIndex == REALTIMEPIPELINEMAIN)
    {
        request.pPrivData = &privRDIOffline1[internalFrameNumIndex];
    }
    else
    {
        request.pPrivData = &privRDIOffline2[internalFrameNumIndex];
    }
    request.pPrivData->streamIndex          = pipelineIndex;
    request.pPrivData->numInputBuffers      = 1;
    request.pPrivData->inputBuffers[0]      = RDIInputBuffer.buffer;
    request.pPrivData->bufferManagers[0]    = GetBufferManager(pipelineIndex);
    CHX_LOG("[MultiCamera RDI] Send normal request:pipelineindex:%d, sessionIndex %d, pMetadata %p",
            pipelineIndex, sessionIndex, request.pMetadata);
    GenerateInternalRequest(sessionIndex, 1, &request);

    if (NULL != pMetadata)
    {
        free_camera_metadata(pMetadata);
        pMetadata = NULL;
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseMultiCamera::isOfflineProcessRequired
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseMultiCamera::isOfflineProcessRequired(
    camera3_capture_request_t* pRequest,
    RequestMapInfo*            pRequestMapInfo)
{
    (VOID)pRequest;
    BOOL isOfflineRequired     = FALSE;
    UINT32 isZSLMode           = 0;
    BOOL isFlashRequired       = FALSE;
    BOOL isMeetZSLCondition    = FALSE;

    if (NULL != pRequestMapInfo)
    {
        // for all advance feature, it need to be handled in offline thread
        if (NULL != pRequestMapInfo->pFeature)
        {
            if (TRUE == pRequestMapInfo->triggerOfflineReprocess)
            {
                isOfflineRequired = TRUE;
            }
            else
            {
                isOfflineRequired = FALSE;
            }
        }
        else
        {
            if (TRUE == pRequestMapInfo->isSnapshotReturnNeeded)
            {
                isOfflineRequired = TRUE;
            }
            else
            {
                isOfflineRequired = FALSE;
            }
        }
        pRequestMapInfo->needTriggerOfflineProcess = isOfflineRequired;
    }

    return isOfflineRequired;
}

