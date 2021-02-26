////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxadvancedcamerausecase.cpp
/// @brief Usecases class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "chistatspropertydefines.h"
#include "chxincs.h"
#include "chxpipeline.h"
#include "chxsession.h"
#include "chxusecase.h"
#include "chxadvancedcamerausecase.h"

#include "g_pipelines.h"

#include "system/camera_metadata.h"

extern CHICONTEXTOPS g_chiContextOps;

const INT32 InvalidSessionId            = -1;
static const UINT32 TIMEOUT_BUFFER_WAIT = 600;

inline BOOL IsThisClonedStream(ChiStream** ppClonedStream, ChiStream* pActualStream, UINT *pIndex)
{
    BOOL bIsCloned = FALSE;
    for (UINT i = 0; i < MaxChiStreams; i++)
    {
        if (ppClonedStream[i] == pActualStream)
        {
            bIsCloned = TRUE;
            *pIndex   = i;
            break;
        }
    }
    return bIsCloned;
}


ChiUsecase* pAdvancedUsecase  = &Usecases27Target[UsecaseId27Target::UsecaseZSLId];
ChiUsecase* pZslTuningUsecase = &Usecases3Target[UsecaseId3Target::UsecaseZSLTuningId];

const INT32 AdvancedPipelineIdMapping[] =
{
    UsecaseZSLPipelineIds::ZSLSnapshotJpeg,             // ZSLSnapshotJpegType = 0
    UsecaseZSLPipelineIds::ZSLSnapshotYUV,              // ZSLSnapshotYUVType = 1
    UsecaseZSLPipelineIds::InternalZSLYuv2Jpeg,         // InternalZSLYuv2JpegType,        // YUV to JPEG
    UsecaseZSLPipelineIds::InternalZSLYuv2JpegMFNR,     // InternalZSLYuv2JpegMFNRType     // YUV to JPEG for MFNR
    UsecaseZSLPipelineIds::Merge3YuvCustomTo1Yuv,       // Merge3YuvCustomTo1YuvType,      // merge 3 YUV to 1 YUV
    UsecaseZSLPipelineIds::ZSLPreviewRaw,               // ZSLPreviewRawType,
    UsecaseZSLPipelineIds::ZSLPreviewRawYUV,            // ZSLPreviewRawYUVType,
    UsecaseZSLPipelineIds::MfnrPrefilter,               // MFNRPrefilterType,              // MFNR prefilter
    UsecaseZSLPipelineIds::MfnrBlend,                   // MFNRBlendType,                  // MFNR blend
    UsecaseZSLPipelineIds::MfnrPostFilter,              // MFNRPostFilterType,             // MFNR post filter
    UsecaseZSLPipelineIds::SWMFMergeYuv,                // SWMFMergeYuvType,               // SWMF merge pipeline
    UsecaseZSLPipelineIds::ZSLSnapshotYUVAux,           // ZSLSnapshotYUVAuxType           // BPS + IPE for Aux
    UsecaseZSLPipelineIds::InternalZSLYuv2JpegMFNRAux,  // InternalZSLYuv2JpegMFNRTypeAux  // YUV to JPEG for MFNR
    UsecaseZSLPipelineIds::MfnrPrefilterAux,            // MFNRPrefilterAuxType,           // MFNR prefilter Aux
    UsecaseZSLPipelineIds::MfnrBlendAux,                // MFNRBlendAuxType,               // MFNR blend Aux
    UsecaseZSLPipelineIds::MfnrPostFilterAux,           // MFNRPostFilterAuxType,          // MFNR post filter Aux
    UsecaseZSLPipelineIds::ZSLYuv2Yuv,                  // YUV to YUV reprocess
    UsecaseZSLPipelineIds::ZSLSnapshotJpegGPU,          // ZSLSnapshotJpegGPUType,
    UsecaseZSLPipelineIds::RealTimeSHDR,                // RealTimeSHDR,
    UsecaseZSLPipelineIds::SHDRSnapshot,                // SHDRSnapshot,
    UsecaseZSLPipelineIds::RealTimeYUVSHDR,             // RealTimeYUVSHDR,
    UsecaseZSLPipelineIds::RealTime3RAWFrameYUVSHDR,    // RealTime3RAWFrameYUVSHDR,
    UsecaseZSLPipelineIds::RealTime3FrameYUVSHDRWithHWStats,// RealTime3FrameYUVSHDRWithHWStats,
};

const INT32 AdvancedPipelineIdTuningMapping[] =
{
    UsecaseZSLTuningPipelineIds::ZSLSnapshotJpegTuning,  // ZSLSnapshotJpegType = 0
    UsecaseZSLTuningPipelineIds::ZSLSnapshotYUVTuning,   // ZSLSnapshotYUVType = 1
    -1,                                                  // InternalZSLYuv2JpegType,        // YUV to JPEG
    -1,                                                  // Merge3YuvCustomTo1YuvType,      // merge 3 YUV to 1 YUV
    UsecaseZSLTuningPipelineIds::ZSLPreviewRawTuning     // ZSLPreviewRawType,
};

// Update the AdvancedPipelineIdMapping if the AdvancedPipelineType is changed
CHX_STATIC_ASSERT((sizeof(AdvancedPipelineIdMapping) / sizeof(AdvancedPipelineIdMapping[0]) ==
    AdvancedPipelineType::PipelineCount));

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class CameraUsecaseBaseBase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::~CameraUsecaseBase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CameraUsecaseBase::~CameraUsecaseBase()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::DestroyDeferResource
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::DestroyDeferResource()
{
    CDKResult result = CDKResultSuccess;

    WaitForDeferThread();

    if (NULL != m_pDeferOfflineDoneMutex)
    {
        m_pDeferOfflineDoneMutex->Destroy();
        m_pDeferOfflineDoneMutex = NULL;
    }

    if (NULL != m_pDeferOfflineDoneCondition)
    {
        m_pDeferOfflineDoneCondition->Destroy();
        m_pDeferOfflineDoneCondition = NULL;
    }

    m_deferOfflineSessionDone       = FALSE;
    m_deferOfflineThreadCreateDone  = FALSE;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::Destroy(
    BOOL isForced)
{
    ATRACE_BEGIN("CameraUsecaseBase::Destroy");

    DestroyDeferResource();

    for (UINT session = 0; session < MaxSessions; session++)
    {
        SessionData* pData = &m_sessions[session];

        if (NULL != pData->pSession)
        {
            pData->pSession->Destroy(isForced);
            pData->pSession = NULL;
        }

        for (UINT pipeline = 0; pipeline < pData->numPipelines; pipeline++)
        {
            for (UINT stream = 0; stream < pData->pipelines[pipeline].numStreams; stream++)
            {
                // Anything we need to delete with streams?
                pData->pipelines[pipeline].pStreams[stream] = NULL;
            }

            if (NULL != pData->pipelines[pipeline].pPipeline)
            {
                pData->pipelines[pipeline].pPipeline->Destroy();
                pData->pipelines[pipeline].pPipeline = NULL;
            }
        }
    }

    for (UINT i = 0; i < MaxChiStreams; i++)
    {
        if (NULL != m_pClonedStream[i])
        {
            CHX_FREE(m_pClonedStream[i]);
        }
    }

    if (NULL != m_pPipelineToCamera)
    {
        CHX_FREE(m_pPipelineToCamera);
        m_pPipelineToCamera = NULL;
    }

    if (NULL != m_pPipelineToSession)
    {
        CHX_FREE(m_pPipelineToSession);
        m_pPipelineToSession = NULL;
    }

    for (UINT target = 0; target < MaxChiStreams; target++)
    {
        for (UINT queueIndex = 0; queueIndex < BufferQueueDepth; queueIndex++)
        {
            if (NULL != m_targetBuffers[target].bufferQueue[queueIndex].pRdiOutputBuffer)
            {
                CHX_FREE(m_targetBuffers[target].bufferQueue[queueIndex].pRdiOutputBuffer);
                m_targetBuffers[target].bufferQueue[queueIndex].pRdiOutputBuffer = NULL;
            }
            if (NULL != m_targetBuffers[target].bufferQueue[queueIndex].pMetadata)
            {
                free_camera_metadata(m_targetBuffers[target].bufferQueue[queueIndex].pMetadata);
                m_targetBuffers[target].bufferQueue[queueIndex].pMetadata = NULL;
            }
        }

        if (NULL != m_targetBuffers[target].pBufferManager)
        {
            m_targetBuffers[target].pBufferManager->Destroy();
            m_targetBuffers[target].pBufferManager = NULL;
        }

        if (NULL != m_targetBuffers[target].pMutex)
        {
            m_targetBuffers[target].pMutex->Destroy();
            m_targetBuffers[target].pMutex = NULL;
        }
    }

    ATRACE_END();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::DeferOfflineSessionThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* CameraUsecaseBase::DeferOfflineSessionThread(VOID * pThreadData)
{
    PerThreadData* pPerThreadData         = reinterpret_cast<PerThreadData*>(pThreadData);
    CameraUsecaseBase* pCameraUsecaseBase = reinterpret_cast<CameraUsecaseBase*>(pPerThreadData->pPrivateData);
    pCameraUsecaseBase->DeferOfflineSession();

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::StartDeferThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::StartDeferThread()
{
    CDKResult result          = CDKResultSuccess;

    if (FALSE == m_deferOfflineThreadCreateDone)
    {
        m_deferOfflineSessionThreadData.pPrivateData = this;
        CHX_LOG("StartDeferThread!");

        result = ChxUtils::ThreadCreate(CameraUsecaseBase::DeferOfflineSessionThread, &m_deferOfflineSessionThreadData,
                                        &m_deferOfflineSessionThreadData.hThreadHandle);

        pthread_detach(m_deferOfflineSessionThreadData.hThreadHandle);

        m_pDeferOfflineDoneMutex->Lock();
        m_deferOfflineThreadCreateDone = TRUE;
        m_pDeferOfflineDoneMutex->Unlock();

        if (CDKResultSuccess != result)
        {
            m_deferOfflineSessionThreadData.pPrivateData = NULL;
            m_deferOfflineSessionThreadData.hThreadHandle = INVALID_THREAD_HANDLE;
            CHX_LOG_ERROR("Create defer thread failed!");
        }
    }
    else
    {
        CHX_LOG_WARN("Warning:Advanced offline related session has been created!");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::WaitForDeferThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::WaitForDeferThread()
{
    CDKResult result = CDKResultSuccess;

    if (TRUE == m_deferOfflineThreadCreateDone)
    {
        m_pDeferOfflineDoneMutex->Lock();
        if ((FALSE == m_deferOfflineSessionDone))
        {
            result = m_pDeferOfflineDoneCondition->TimedWait(m_pDeferOfflineDoneMutex->GetNativeHandle(), 1000);
            if (CDKResultSuccess != result)
            {
                CHX_LOG_ERROR("WaitForDeferThread timed out!");
            }

        }
        m_pDeferOfflineDoneMutex->Unlock();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::DeferOfflineSession
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::DeferOfflineSession()
{
    ATRACE_BEGIN("DeferOfflineSession");

    CDKResult result            = CDKResultSuccess;
    BOOL      bReprocessUsecase = FALSE;
    m_pDeferOfflineDoneMutex->Lock();
    m_deferOfflineSessionDone = FALSE;
    m_pDeferOfflineDoneMutex->Unlock();

    result = CreateOfflineSessions(m_pCallBacks);
    if(CDKResultSuccess != result)
    {
        CHX_LOG_ERROR("Failed to create session");
    }
    m_pDeferOfflineDoneMutex->Lock();
    m_deferOfflineSessionDone = TRUE;
    m_pDeferOfflineDoneCondition->Signal();
    m_pDeferOfflineDoneMutex->Unlock();

    ATRACE_END();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::Initialize(
    ChiCallBacks* pCallbacks)
{
    ATRACE_BEGIN("CameraUsecaseBase::Initialize");

    CDKResult result            = Usecase::Initialize();
    BOOL      bReprocessUsecase = FALSE;

    m_lastResultMetadataFrameNum = -1;
    m_effectModeValue            = ANDROID_CONTROL_EFFECT_MODE_OFF;
    m_sceneModeValue             = ANDROID_CONTROL_SCENE_MODE_DISABLED;
    m_rtSessionIndex             = InvalidId;

    m_deferOfflineThreadCreateDone = FALSE;
    m_pDeferOfflineDoneMutex       = Mutex::Create();
    m_pDeferOfflineDoneCondition   = Condition::Create();
    m_deferOfflineSessionDone      = FALSE;
    m_pCallBacks                   = pCallbacks;
    m_GpuNodePresence              = FALSE;
    m_debugLastResultFrameNumber   = static_cast<UINT32>(-1);
    m_numSessions = 0;

    // Default to 1-1 mapping of sessions and pipelines
    if (0 == m_numSessions)
    {
        m_numSessions = m_pChiUsecase->numPipelines;
    }

    CHX_ASSERT(0 != m_numSessions);


    if (CDKResultSuccess == result)
    {
        ChxUtils::Memset(m_pClonedStream, 0, (sizeof(ChiStream*)*MaxChiStreams));
        ChxUtils::Memset(m_pFrameworkOutStreams, 0, (sizeof(ChiStream*)*MaxChiStreams));
        ChxUtils::Memset(&m_sessions, 0 , (sizeof(SessionData)*MaxSessions));
        ChxUtils::Memset(&m_perSessionPvtData, 0, (sizeof(SessionPrivateData)*MaxSessions));
        ChxUtils::Memset(&m_targetBuffers, 0 , (sizeof(TargetBuffer)*MaxChiStreams));
        m_bCloningNeeded            = FALSE;
        m_numberOfOfflineStreams    = 0;

        for (UINT i = 0; i < m_pChiUsecase->numPipelines; i++)
        {
            if (m_pChiUsecase->pPipelineTargetCreateDesc[i].sourceTarget.numTargets > 0)
            {
                bReprocessUsecase = TRUE;
            }
        }

        for (UINT i = 0; i < m_pChiUsecase->numPipelines; i++)
        {
            if (TRUE == m_pChiUsecase->pPipelineTargetCreateDesc[i].pipelineCreateDesc.isRealTime)
            {
                // Cloning of streams needs when source target stream is enabled and
                // all the streams are connected in both real time and offline pipelines
                // excluding the input stream count
                m_bCloningNeeded = bReprocessUsecase && (UsecaseId::PreviewZSL != m_usecaseId) &&
                    (m_pChiUsecase->pPipelineTargetCreateDesc[i].sinkTarget.numTargets == (m_pChiUsecase->numTargets - 1));
                if (TRUE == m_bCloningNeeded)
                {
                    break;
                }
            }
        }
        CHX_LOG("m_bCloningNeeded = %d", m_bCloningNeeded );

        for (UINT i = 0; i < m_pChiUsecase->numPipelines; i++)
        {
            // use mapping if available, otherwise default to 1-1 mapping
            UINT sessionId  = (NULL != m_pPipelineToSession) ? m_pPipelineToSession[i] : i;
            UINT pipelineId = m_sessions[sessionId].numPipelines++;
            CHX_LOG("Creating Pipeline %s at index %u for session %u, session's pipeline %u, camera id:%d",
                m_pChiUsecase->pPipelineTargetCreateDesc[i].pPipelineName, i, sessionId, pipelineId, m_pPipelineToCamera[i]);

            result = CreatePipeline(m_pPipelineToCamera[i],
                                    &m_pChiUsecase->pPipelineTargetCreateDesc[i],
                                    &m_sessions[sessionId].pipelines[pipelineId]);

            if (FALSE == m_GpuNodePresence)
            {
                for (UINT nodeIndex = 0;
                        nodeIndex < m_pChiUsecase->pPipelineTargetCreateDesc[i].pipelineCreateDesc.numNodes; nodeIndex++)
                {
                    UINT32 nodeIndexId =
                                m_pChiUsecase->pPipelineTargetCreateDesc[i].pipelineCreateDesc.pNodes->nodeId;
                    if (255 == nodeIndexId)
                    {
                        if (NULL != m_pChiUsecase->pPipelineTargetCreateDesc[i].pipelineCreateDesc.pNodes->pNodeProperties)
                        {
                            const CHAR* gpuNodePropertyValue = "com.qti.node.gpu";
                            const CHAR* nodePropertyValue = (const CHAR*)
                                m_pChiUsecase->pPipelineTargetCreateDesc[i].pipelineCreateDesc.pNodes->pNodeProperties->pValue;
                            if (!strcmp(gpuNodePropertyValue, nodePropertyValue))
                            {
                                m_GpuNodePresence = TRUE;
                                break;
                            }
                        }
                    }
                }
            }

            PipelineCreated(sessionId, i);
        }

        if (CDKResultSuccess == result)
        {
            StartDeferThread();
            result = CreateRTSessions(pCallbacks);
        }
    }


    // Create the buffer queues for all usecase internal streams/buffers/targets

    for (UINT i = 0; i < m_pChiUsecase->numTargets; i++)
    {
        if (TRUE == StreamIsInternal(m_pChiUsecase->ppChiTargets[i]->pChiStream))
        {
            CHX_LOG("CameraUsecaseBase::Initialize, create chi buffers for targets [%d/%d], chistream:%p, format:%d, %dx%d",
                i,
                m_pChiUsecase->numTargets,
                m_pChiUsecase->ppChiTargets[i]->pChiStream,
                m_pChiUsecase->ppChiTargets[i]->pChiStream->format,
                m_pChiUsecase->ppChiTargets[i]->pChiStream->width,
                m_pChiUsecase->ppChiTargets[i]->pChiStream->height);

            CHIBufferManagerCreateData createData = { 0 };

            createData.width                = m_pChiUsecase->ppChiTargets[i]->pChiStream->width;
            createData.height               = m_pChiUsecase->ppChiTargets[i]->pChiStream->height;
            createData.format               = m_pChiUsecase->ppChiTargets[i]->pChiStream->format;
            createData.producerFlags        = GRALLOC1_PRODUCER_USAGE_CAMERA;
            createData.consumerFlags        = GRALLOC1_CONSUMER_USAGE_CAMERA;
            // Why ZSLQueueDepth * 2 + 6 for maxBufferCount:
            //  ZSL queue will be filled up with buffers and extra 6 buffers are needed to keep the preview running
            //  ZSLQueueDepth + 6 (maxHalRequests) is the minimum buffer count to support preview
            //  In case there is a MFNR snapshot request which taks all the RDIs from the zsl queue,
            //  we need to fill up the queue again with 'ZSLQueueDepth' RDIs buffers for the next snapshot.
            UINT32 enabledAdvanceFeatures = 0;

            enabledAdvanceFeatures = ExtensionModule::GetInstance()->GetAdvanceFeatureMask();
            if (AdvanceFeatureMFNR == (enabledAdvanceFeatures & AdvanceFeatureMFNR))
            {
                createData.maxBufferCount = ((ZSLQueueDepth * 2) + 6);
            }
            else
            {
                createData.maxBufferCount = (ZSLQueueDepth + 6);
            }
            createData.immediateBufferCount = CHIImmediateBufferCountZSL;

            CHAR bufferManagerName[MaxStringLength64];
            CdkUtils::SNPrintF(bufferManagerName, sizeof(bufferManagerName), "AdvancedCameraBufferManager_%d", i);
            m_targetBuffers[i].pBufferManager      = CHIBufferManager::Create(bufferManagerName, &createData);
            m_targetBuffers[i].pMutex              = Mutex::Create();
            m_targetBuffers[i].lastReadySequenceID = INVALIDSEQUENCEID;
            m_targetBuffers[i].queueDepth          = ZSLQueueDepth;

            for (UINT queueIndex = 0; queueIndex < BufferQueueDepth; queueIndex++)
            {
                m_targetBuffers[i].bufferQueue[queueIndex].pRdiOutputBuffer =
                    static_cast<CHISTREAMBUFFER*>(CHX_CALLOC(sizeof(CHISTREAMBUFFER)));

                if (NULL != m_targetBuffers[i].bufferQueue[queueIndex].pRdiOutputBuffer)
                {
                    m_targetBuffers[i].bufferQueue[queueIndex].pRdiOutputBuffer->acquireFence = -1;
                    m_targetBuffers[i].bufferQueue[queueIndex].pRdiOutputBuffer->phBuffer = NULL;
                    m_targetBuffers[i].bufferQueue[queueIndex].pRdiOutputBuffer->pStream =
                        m_pChiUsecase->ppChiTargets[i]->pChiStream;
                }
            }
        }
    }

    ATRACE_END();
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::CreateSessions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::CreateRTSessions(
    ChiCallBacks* pCallbacks)
{
    CDKResult result = CDKResultSuccess;
    BOOL RealtimeSession = FALSE;

    for (INT sessionId = m_numSessions-1; sessionId >= 0; sessionId--)
    {
        CHX_LOG("Creating Session %d", sessionId);
        RealtimeSession = FALSE;

        Pipeline* pPipelines[MaxPipelinesPerSession] = {0};

        m_sessions[sessionId].rtPipelineIndex = InvalidId;

        // Accumulate the pipeline pointers to an array to pass the session creation
        for (UINT pipelineId = 0; pipelineId < m_sessions[sessionId].numPipelines; pipelineId++)
        {
            // CHX_LOG("MFNR CreateSessions pPipelineData->pPipeline %p sessionId %d pipelineId %d",
                // m_sessions[sessionId].pipelines[pipelineId].pPipeline, sessionId, pipelineId);

            if (TRUE == m_sessions[sessionId].pipelines[pipelineId].pPipeline->IsRealTime())
            {
                // Only set the rtPipelineIndex for the first realtime pipeline
                if (InvalidId == m_sessions[sessionId].rtPipelineIndex)
                {
                    m_pDeferOfflineDoneMutex->Lock();
                    m_sessions[sessionId].rtPipelineIndex = pipelineId;
                    m_pDeferOfflineDoneMutex->Unlock();
                }
                // Only set m_rtSessionIdex for the first realtime session
                if (InvalidId == m_rtSessionIndex)
                {
                    m_pDeferOfflineDoneMutex->Lock();
                    m_rtSessionIndex = sessionId;
                    m_pDeferOfflineDoneMutex->Unlock();
                }
                RealtimeSession = TRUE;
            }

            pPipelines[pipelineId] = m_sessions[sessionId].pipelines[pipelineId].pPipeline;
        }

        if (TRUE == RealtimeSession)
        {
            CHX_LOG("Create RT session");

            m_perSessionPvtData[sessionId].sessionId = sessionId;
            m_perSessionPvtData[sessionId].pUsecase = this;

            if (NULL == m_sessions[sessionId].pSession)
            {
                m_sessions[sessionId].pSession = Session::Create(pPipelines,
                                                                 m_sessions[sessionId].numPipelines,
                                                                 &pCallbacks[sessionId],
                                                                 &m_perSessionPvtData[sessionId]);
            }

            if (NULL == m_sessions[sessionId].pSession)
            {
                CHX_LOG_ERROR("Failed to create session, sessionId: %d", sessionId);
                result = CDKResultEFailed;
            }
            else
            {
                CHX_LOG("success Creating Session %d", sessionId);
            }
        }
    }

    if (result != CDKResultSuccess)
    {
        for (INT sessionId = m_numSessions - 1; sessionId >= 0; sessionId--)
        {
            if (NULL != m_sessions[sessionId].pSession)
            {
                m_sessions[sessionId].pSession->Destroy(TRUE);
                m_sessions[sessionId].pSession = NULL;
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::CreateOfflineSessions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::CreateOfflineSessions(
    ChiCallBacks* pCallbacks)
{
    CDKResult result = CDKResultSuccess;
    BOOL RealtimeSession = FALSE;

    for (INT sessionId = m_numSessions - 1; sessionId >= 0; sessionId--)
    {
        CHX_LOG("Creating Session %d", sessionId);
        RealtimeSession = FALSE;

        Pipeline* pPipelines[MaxPipelinesPerSession] = { 0 };

        m_sessions[sessionId].rtPipelineIndex = InvalidId;

        // Accumulate the pipeline pointers to an array to pass the session creation
        for (UINT pipelineId = 0; pipelineId < m_sessions[sessionId].numPipelines; pipelineId++)
        {
            if (TRUE == m_sessions[sessionId].pipelines[pipelineId].pPipeline->IsRealTime())
            {
                // Only set the rtPipelineIndex for the first realtime pipeline
                if (InvalidId == m_sessions[sessionId].rtPipelineIndex)
                {
                    m_pDeferOfflineDoneMutex->Lock();
                    m_sessions[sessionId].rtPipelineIndex = pipelineId;
                    m_pDeferOfflineDoneMutex->Unlock();
                }
                // Only set m_rtSessionIdex for the first realtime session
                if (InvalidId == m_rtSessionIndex)
                {
                    m_pDeferOfflineDoneMutex->Lock();
                    m_rtSessionIndex = sessionId;
                    m_pDeferOfflineDoneMutex->Unlock();
                }
                RealtimeSession = TRUE;
            }

            pPipelines[pipelineId] = m_sessions[sessionId].pipelines[pipelineId].pPipeline;
        }

        if (FALSE == RealtimeSession)
        {
            CHX_LOG("Create offline session in defer thread");
            m_perSessionPvtData[sessionId].sessionId = sessionId;
            m_perSessionPvtData[sessionId].pUsecase = this;

            if (NULL == m_sessions[sessionId].pSession)
            {
                m_sessions[sessionId].pSession = Session::Create(pPipelines,
                                                                 m_sessions[sessionId].numPipelines,
                                                                 &pCallbacks[sessionId],
                                                                 &m_perSessionPvtData[sessionId]);
            }

            if (NULL == m_sessions[sessionId].pSession)
            {
                CHX_LOG_ERROR("Failed to create session, sessionId: %d", sessionId);
                result = CDKResultEFailed;
            }
            else
            {
                CHX_LOG("success Creating Session %d", sessionId);
            }
        }
    }

    if (result != CDKResultSuccess)
    {
        for (INT sessionId = m_numSessions - 1; sessionId >= 0; sessionId--)
        {
            if (NULL != m_sessions[sessionId].pSession)
            {
                m_sessions[sessionId].pSession->Destroy(TRUE);
                m_sessions[sessionId].pSession = NULL;
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::CreatePipeline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::CreatePipeline(
    UINT32                              cameraId,
    ChiPipelineTargetCreateDescriptor*  pPipelineDesc,
    PipelineData*                       pPipelineData)
{
    CDKResult result = CDKResultSuccess;

    pPipelineData->pPipeline = Pipeline::Create(cameraId, PipelineType::Default);

    if (NULL != pPipelineData->pPipeline)
    {
        UINT                         numStreams  = 0;
        ChiTargetPortDescriptorInfo* pSinkTarget = &pPipelineDesc->sinkTarget;
        ChiTargetPortDescriptorInfo* pSrcTarget  = &pPipelineDesc->sourceTarget;

        ChiPortBufferDescriptor pipelineOutputBuffer[MaxChiStreams];
        ChiPortBufferDescriptor pipelineInputBuffer[MaxChiStreams];

        ChxUtils::Memset(pipelineOutputBuffer, 0, sizeof(pipelineOutputBuffer));
        ChxUtils::Memset(pipelineInputBuffer, 0, sizeof(pipelineInputBuffer));

        for (UINT sinkIdx = 0; sinkIdx < pSinkTarget->numTargets; sinkIdx++)
        {
            ChiTargetPortDescriptor* pSinkTargetDesc = &pSinkTarget->pTargetPortDesc[sinkIdx];
            if ((pSrcTarget->numTargets > 0) && (TRUE == m_bCloningNeeded))
            {
                m_pFrameworkOutStreams[m_numberOfOfflineStreams] = pSinkTargetDesc->pTarget->pChiStream;
                m_pClonedStream[m_numberOfOfflineStreams]        = static_cast<CHISTREAM*>(CHX_CALLOC(sizeof(CHISTREAM)));

                ChxUtils::Memcpy(m_pClonedStream[m_numberOfOfflineStreams], pSinkTargetDesc->pTarget->pChiStream, sizeof(CHISTREAM));

                pipelineOutputBuffer[sinkIdx].pStream   = m_pClonedStream[m_numberOfOfflineStreams];
                pipelineOutputBuffer[sinkIdx].nodePort  = pSinkTargetDesc->nodeport;
                pPipelineData->pStreams[numStreams++]   = pipelineOutputBuffer[sinkIdx].pStream;
                m_numberOfOfflineStreams++;
            }
            else
            {
                pipelineOutputBuffer[sinkIdx].pStream   = pSinkTargetDesc->pTarget->pChiStream;
                pipelineOutputBuffer[sinkIdx].nodePort  = pSinkTargetDesc->nodeport;
                pPipelineData->pStreams[numStreams++]   = pipelineOutputBuffer[sinkIdx].pStream;
            }
        }

        for (UINT sourceIdx = 0; sourceIdx < pSrcTarget->numTargets; sourceIdx++)
        {
            UINT                     i              = 0;
            ChiTargetPortDescriptor* pSrcTargetDesc = &pSrcTarget->pTargetPortDesc[sourceIdx];

            pipelineInputBuffer[sourceIdx].pStream = pSrcTargetDesc->pTarget->pChiStream;
            pipelineInputBuffer[sourceIdx].nodePort = pSrcTargetDesc->nodeport;

            for (i = 0; i < numStreams; i++)
            {
                if (pPipelineData->pStreams[i] == pipelineInputBuffer[sourceIdx].pStream)
                {
                    break;
                }
            }
            if (numStreams == i)
            {
                pPipelineData->pStreams[numStreams++] = pipelineInputBuffer[sourceIdx].pStream;
            }
        }

        pPipelineData->pPipeline->SetOutputBuffers(pSinkTarget->numTargets, &pipelineOutputBuffer[0]);
        pPipelineData->pPipeline->SetInputBuffers(pSrcTarget->numTargets, &pipelineInputBuffer[0]);
        pPipelineData->pPipeline->SetPipelineNodePorts(&pPipelineDesc->pipelineCreateDesc);

        pPipelineData->numStreams       = numStreams;

        result = pPipelineData->pPipeline->CreateDescriptor();
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::DestroyInternalBufferQueue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::DestroyInternalBufferQueue(
    UINT32 bufferID)
{
    if (MaxChiStreams > bufferID)
    {
        TargetBuffer* pTargetBuffer = &m_MCTargetBuffer[bufferID];

        if (NULL != pTargetBuffer->pBufferManager)
        {
            pTargetBuffer->pBufferManager->Destroy();
            pTargetBuffer->pBufferManager = NULL;
        }

        if( NULL != pTargetBuffer->pMutex)
        {
            pTargetBuffer->pMutex->Destroy();
            pTargetBuffer->pMutex = NULL;
        }

        if (NULL != pTargetBuffer->pCondition)
        {
            pTargetBuffer->pCondition->Destroy();
            pTargetBuffer->pCondition = NULL;
        }

        for (UINT32 i = 0; i< pTargetBuffer->queueDepth; i++)
        {
            if (NULL != pTargetBuffer->bufferQueue[i].pRdiOutputBuffer)
            {
                ChxUtils::Free(pTargetBuffer->bufferQueue[i].pRdiOutputBuffer);
                pTargetBuffer->bufferQueue[i].pRdiOutputBuffer = NULL;
            }

            if (NULL != pTargetBuffer->bufferQueue[i].pMetadata)
            {
                free_camera_metadata(pTargetBuffer->bufferQueue[i].pMetadata);
                pTargetBuffer->bufferQueue[i].pMetadata = NULL;
            }

            pTargetBuffer->bufferQueue[i].isBufferReady   = FALSE;
            pTargetBuffer->bufferQueue[i].isMetadataReady = FALSE;
        }

        pTargetBuffer->queueDepth = 0;
    }
    else
    {
        CHX_LOG("invalid bufferID:%d", bufferID);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::DestroyAllInternalBufferQueues
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::DestroyAllInternalBufferQueues()
{
    for (UINT32 i = 0; i < m_targetBuffersCount; i++)
    {
        DestroyInternalBufferQueue(i);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::AddNewInternalBufferQueue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::AddNewInternalBufferQueue(
    ChiStream*                  pChiStream,
    CHIBufferManagerCreateData* pBufferCreateData,
    UINT32*                     pBufferID,
    const CHAR*                 pBufferManagerName)
{
    CHX_ASSERT(NULL != pChiStream);
    CHX_ASSERT(NULL != pBufferCreateData);
    CHX_ASSERT(NULL != pBufferID);
    CHX_ASSERT(NULL != pBufferManagerName);

    CDKResult result = CDKResultSuccess;

    if (MaxChiStreams <= m_targetBuffersCount)
    {
        CHX_LOG_ERROR("Can't add new internal buffer queue!");
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {
        m_MCTargetBuffer[m_targetBuffersCount].pBufferManager = CHIBufferManager::Create(pBufferManagerName, pBufferCreateData);
        m_MCTargetBuffer[m_targetBuffersCount].pMutex         = Mutex::Create();
        m_MCTargetBuffer[m_targetBuffersCount].pCondition     = Condition::Create();
        m_MCTargetBuffer[m_targetBuffersCount].queueDepth     = ZSLQueueDepth;

        for (UINT32 i = 0; i < m_MCTargetBuffer[m_targetBuffersCount].queueDepth; i++)
        {
            m_MCTargetBuffer[m_targetBuffersCount].bufferQueue[i].pRdiOutputBuffer =
                static_cast<CHISTREAMBUFFER*>(CHX_CALLOC(sizeof(CHISTREAMBUFFER)));

            if (NULL != m_MCTargetBuffer[m_targetBuffersCount].bufferQueue[i].pRdiOutputBuffer)
            {
                m_MCTargetBuffer[m_targetBuffersCount].bufferQueue[i].pRdiOutputBuffer->acquireFence    = -1;
                m_MCTargetBuffer[m_targetBuffersCount].bufferQueue[i].pRdiOutputBuffer->phBuffer        = NULL;
                m_MCTargetBuffer[m_targetBuffersCount].bufferQueue[i].pRdiOutputBuffer->pStream         = pChiStream;
                m_MCTargetBuffer[m_targetBuffersCount].bufferQueue[i].pMetadata                         = NULL;
                m_MCTargetBuffer[m_targetBuffersCount].bufferQueue[i].isBufferReady                     = FALSE;
                m_MCTargetBuffer[m_targetBuffersCount].bufferQueue[i].isMetadataReady                   = FALSE;

                CHX_LOG("create target buffer:%d,%p,%p", i,
                        m_MCTargetBuffer[m_targetBuffersCount].bufferQueue[i].pRdiOutputBuffer,
                        m_MCTargetBuffer[m_targetBuffersCount].bufferQueue[i].pRdiOutputBuffer->phBuffer);
            }
        }
    }

    *pBufferID = m_targetBuffersCount;
    m_targetBuffersCount++;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::IsRDIBufferMetaReadyForInput
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CameraUsecaseBase::IsRDIBufferMetaReadyForInput(
        UINT32 frameNumber,
        UINT32 pipelineIndex)
{
    BOOL isBufferMetaReady = TRUE;

    TargetBuffer* pTargetBuffer = &m_MCTargetBuffer[pipelineIndex];
    UINT32 bufferIndex          = frameNumber % (pTargetBuffer->queueDepth);

    pTargetBuffer->pMutex->Lock();
    if ((FALSE == pTargetBuffer->bufferQueue[bufferIndex].isBufferReady) ||
        (FALSE == pTargetBuffer->bufferQueue[bufferIndex].isMetadataReady))
    {
        isBufferMetaReady = FALSE;
    }
    pTargetBuffer->pMutex->Unlock();

    return isBufferMetaReady;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::ReleaseRDIBufferReference
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::ReleaseRDIBufferReference(
    UINT32              pipelineIndex,
    buffer_handle_t*    pBufferHandle)
{
    CHX_ASSERT(MaxChiStreams > pipelineIndex);
    CHX_ASSERT(NULL != pBufferHandle);

    CDKResult result            = CDKResultSuccess;
    TargetBuffer* pTargetBuffer = &m_MCTargetBuffer[pipelineIndex];

    result = pTargetBuffer->pBufferManager->ReleaseReference(
        reinterpret_cast<buffer_handle_t*>(pBufferHandle));

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::GetBufferManager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHIBufferManager* CameraUsecaseBase::GetBufferManager(
    UINT32 pipelineIndex)
{
    CHX_ASSERT(MaxChiStreams > pipelineIndex);

    TargetBuffer* pTargetBuffer = &m_MCTargetBuffer[pipelineIndex];
    return pTargetBuffer->pBufferManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::GetInputBufferFromRDIQueue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::GetInputBufferFromRDIQueue(
    UINT32                      frameNumber,
    UINT32                      pipelineIndex,
    camera3_stream_buffer_t*    pInputBuffer,
    camera_metadata_t**         ppMetadata)
{
    CDKResult result            = CDKResultSuccess;
    TargetBuffer* pTargetBuffer = &m_MCTargetBuffer[pipelineIndex];
    UINT32 bufferIndex          = frameNumber % (pTargetBuffer->queueDepth);

    if (NULL == pInputBuffer)
    {
        CHX_LOG_ERROR("pInputBuffer is NULL");
        result = CDKResultEFailed;
    }

    pTargetBuffer->pMutex->Lock();

    if ((FALSE == pTargetBuffer->bufferQueue[bufferIndex].isBufferReady) ||
        (FALSE == pTargetBuffer->bufferQueue[bufferIndex].isMetadataReady))
    {
        CHX_LOG_ERROR("buffer or metadata (framenumber:%d,buffervalid:%d, metdatavalid:%d) is not ready!",
            frameNumber,
            pTargetBuffer->bufferQueue[bufferIndex].isBufferReady,
            pTargetBuffer->bufferQueue[bufferIndex].isMetadataReady);
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {
        ChxUtils::Memcpy(pInputBuffer,
                         pTargetBuffer->bufferQueue[bufferIndex].pRdiOutputBuffer,
                         sizeof(camera3_stream_buffer_t));

        pTargetBuffer->pBufferManager->AddReference(
            reinterpret_cast<buffer_handle_t*>(pTargetBuffer->bufferQueue[bufferIndex].pRdiOutputBuffer->phBuffer));

        *ppMetadata = pTargetBuffer->bufferQueue[bufferIndex].pMetadata;

        pInputBuffer->acquire_fence = -1;
    }
    pTargetBuffer->pMutex->Unlock();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::UpdateBufferReadyForRDIQueue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::UpdateBufferReadyForRDIQueue(
    CHISTREAMBUFFER*    pBuffer,
    UINT32              frameNumber,
    UINT32              pipelineIndex,
    BOOL                isBufferReady)
{
    CHX_ASSERT(NULL != pBuffer);
    CHX_ASSERT(MaxChiStreams > pipelineIndex);

    CDKResult result            = CDKResultSuccess;
    TargetBuffer* pTargetBuffer = &m_MCTargetBuffer[pipelineIndex];
    UINT32 bufferIndex          = frameNumber % (pTargetBuffer->queueDepth);

    if (CDKResultSuccess == result)
    {
        pTargetBuffer->pMutex->Lock();

        // Release reference to the RDI buffer that's going to be replaced out of queue
        if ((frameNumber >= pTargetBuffer->queueDepth) &&
            (NULL != pTargetBuffer->bufferQueue[bufferIndex].pRdiOutputBuffer->phBuffer))
        {
            pTargetBuffer->pBufferManager->ReleaseReference(
                        reinterpret_cast<buffer_handle_t*>(pTargetBuffer->bufferQueue[bufferIndex].pRdiOutputBuffer->phBuffer));
        }
        ChxUtils::Memcpy(pTargetBuffer->bufferQueue[bufferIndex].pRdiOutputBuffer,
                         pBuffer,
                         sizeof(CHISTREAMBUFFER));

        pTargetBuffer->bufferQueue[bufferIndex].isBufferReady = isBufferReady;
        if ((TRUE == isBufferReady) && (TRUE == pTargetBuffer->bufferQueue[bufferIndex].isMetadataReady))
        {
            pTargetBuffer->lastReadySequenceID = frameNumber;
        }
        pTargetBuffer->pCondition->Signal();
        pTargetBuffer->pMutex->Unlock();
    }

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::FillMetadataForRDIQueue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
camera_metadata_t* CameraUsecaseBase::FillMetadataForRDIQueue(
    UINT32                      frameNumber,
    UINT32                      pipelineIndex,
    const camera_metadata_t*    pMetadata)
{
    CHX_ASSERT(NULL != pMetadata);
    CHX_ASSERT(MaxChiStreams > pipelineIndex);

    CDKResult           result          = CDKResultSuccess;
    TargetBuffer*       pTargetBuffer   = &m_MCTargetBuffer[pipelineIndex];
    UINT32              bufferIndex     = frameNumber % (pTargetBuffer->queueDepth);
    camera_metadata_t*  pReturnMetadata = NULL;

    if (NULL == pMetadata)
    {
        CHX_LOG_ERROR("pMetadata is NULL");
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {
        pTargetBuffer->pMutex->Lock();

        if (NULL == pTargetBuffer->bufferQueue[bufferIndex].pMetadata)
        {
            pTargetBuffer->bufferQueue[bufferIndex].pMetadata =
                allocate_copy_camera_metadata_checked(pMetadata, get_camera_metadata_size(pMetadata));
        }
        else
        {
            ChxUtils::ResetMetadata(pTargetBuffer->bufferQueue[bufferIndex].pMetadata);
            MergeMetadata(pTargetBuffer->bufferQueue[bufferIndex].pMetadata, pMetadata);
        }
        pTargetBuffer->bufferQueue[bufferIndex].isMetadataReady = TRUE;
        if (TRUE == pTargetBuffer->bufferQueue[bufferIndex].isBufferReady)
        {
            pTargetBuffer->lastReadySequenceID = frameNumber;
        }
        pReturnMetadata = pTargetBuffer->bufferQueue[bufferIndex].pMetadata;
        pTargetBuffer->pMutex->Unlock();
    }

    return pReturnMetadata;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::GetOutputBufferFromRDIQueue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::GetOutputBufferFromRDIQueue(
    UINT32                      pipelineIndex,
    camera3_stream_buffer_t*    pOutputbuffer)
{
    CDKResult       result          = CDKResultSuccess;
    TargetBuffer*   pTargetBuffer   = &m_MCTargetBuffer[pipelineIndex];

    if (NULL == pOutputbuffer)
    {
        CHX_LOG_ERROR("pOutputBuffer is NULL");
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {
        ImageBuffer* pImageBuffer = pTargetBuffer->pBufferManager->GetImageBuffer();

        if (NULL != pImageBuffer)
        {
            pOutputbuffer->buffer = pImageBuffer->GetBufferHandle();

            pOutputbuffer->stream = reinterpret_cast<camera3_stream_t*>(
                pTargetBuffer->bufferQueue[0].pRdiOutputBuffer->pStream);

            pOutputbuffer->acquire_fence = -1;
        }
        else
        {
            result = CDKResultEFailed;
            CHX_LOG_ERROR("GetOutputBufferFromRDIQueue GetImageBuffer failed! pipelineIndex = %d", pipelineIndex);
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::WaitForBufferMetaReady
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::WaitForBufferMetaReady(
    UINT32 frameNumber,
    UINT32 pipelineIndex)
{
    CDKResult     result        = CDKResultSuccess;
    TargetBuffer* pTargetBuffer = &m_MCTargetBuffer[pipelineIndex];
    UINT32        bufferIndex   = frameNumber % pTargetBuffer->queueDepth;

    pTargetBuffer->pMutex->Lock();
    if (FALSE == pTargetBuffer->bufferQueue[bufferIndex].isBufferReady)
    {
        result = pTargetBuffer->pCondition->TimedWait(pTargetBuffer->pMutex->GetNativeHandle(), TIMEOUT_BUFFER_WAIT);
    }

    if ((FALSE == pTargetBuffer->bufferQueue[bufferIndex].isBufferReady) && (CDKResultSuccess == result))
    {
        result = CDKResultETimeout;
    }

    pTargetBuffer->pMutex->Unlock();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::GetTargetBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::GetTargetBuffer(
    UINT32                   frameNumber,
    TargetBuffer*            pTargetBuffer,
    camera_metadata_t*       pMetadata,
    camera3_stream_buffer_t* inputBuffer)
{
    UINT32 bufferIndex = frameNumber % BufferQueueDepth;

    pTargetBuffer->pMutex->Lock();

    CHX_LOG("Merging metadata %p into %p to use for RDI", pMetadata, pTargetBuffer->bufferQueue[bufferIndex].pMetadata);

    if (NULL != pMetadata && NULL == pTargetBuffer->bufferQueue[bufferIndex].pMetadata)
    {
        pTargetBuffer->bufferQueue[bufferIndex].pMetadata =
            static_cast<camera_metadata_t*>(ChxUtils::AllocateCopyMetaData(reinterpret_cast<const VOID*>(pMetadata)));
    }
    else if (NULL != pMetadata)
    {
        // Now that the queue is locked we can grab out the saved metadata to pass on to reprocess
        MergeMetadata(pMetadata, pTargetBuffer->bufferQueue[bufferIndex].pMetadata);
    }

    // Set the input buffer from the ZSL queue
    ChxUtils::Memcpy(inputBuffer,
                     pTargetBuffer->bufferQueue[bufferIndex].pRdiOutputBuffer,
                     sizeof(camera3_stream_buffer_t));

    // Mark the buffer as used so it won't be used again for the next snapshot request
    pTargetBuffer->bufferQueue[bufferIndex].isBufferUsed = TRUE;

    pTargetBuffer->pBufferManager->AddReference(
        reinterpret_cast<buffer_handle_t*>(pTargetBuffer->bufferQueue[bufferIndex].pRdiOutputBuffer->phBuffer));

    pTargetBuffer->pMutex->Unlock();

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::GetZSLMode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::GetZSLMode(
    UINT32*                  zslMode,
    camera_metadata_t*       pMetadata)
{

    *zslMode = ChxUtils::GetZSLMode(pMetadata);
    CHX_LOG("ZslMode value %d", *zslMode);

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::GetZSLMode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CameraUsecaseBase::IsFlashRequired(
    camera_metadata_t*  pMetadata)
{
    BOOL isFlashRequired = FALSE;
    if (TRUE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(pMetadata), IsFlashRequiredTag))
    {
        INT32* pData = NULL;
        ChxUtils::GetVendorTagValue(pMetadata, IsFlashRequiredTag, reinterpret_cast<VOID**>(&pData));
        if (NULL != pData)
        {
            isFlashRequired = (1 == *pData) ? TRUE : FALSE;
            CHX_LOG("isFlashRequired value %d", *pData);
        }
    }

    return isFlashRequired;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::ExecuteCaptureRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::ExecuteCaptureRequest(
    camera3_capture_request_t* pRequest)
{
    // Base implementation finds the buffers that go to each output and invokes SubmitRequest for each pipeline with outputs
    // If the advanced class wishes to use this function, but not invoke all the pipelines, the output produced from the disired
    // inactive pipeline should be removed from the pRequest->output_buffers
    CDKResult result = CDKResultSuccess;

    CHX_LOG("CameraUsecaseBase::ExecuteCaptureRequest for frame %d with %d output buffers",
        pRequest->frame_number, pRequest->num_output_buffers);

    static const UINT32 NumOutputBuffers = 5;

    UINT frameIndex = pRequest->frame_number % MaxOutstandingRequests;

    m_isMetadataSent[frameIndex]      = FALSE;
    m_isMetadataAvailable[frameIndex] = FALSE;

    if (InvalidId != m_rtSessionIndex)
    {
        UINT32 rtPipeline = m_sessions[m_rtSessionIndex].rtPipelineIndex;

        if (InvalidId != rtPipeline)
        {
            m_selectedSensorModeIndex =
                m_sessions[m_rtSessionIndex].pipelines[rtPipeline].pPipeline->GetSensorModeInfo()->modeIndex;
            result = UpdateSensorModeIndex(const_cast<camera_metadata_t*>(pRequest->settings));
        }
    }

    for (UINT session = 0; session < MaxSessions; session++)
    {
        BOOL bIsOffline = FALSE;
        for (UINT pipeline = 0; pipeline < m_sessions[session].numPipelines; pipeline++)
        {
            if (NULL != pRequest->input_buffer)
            {
                bIsOffline = TRUE;
                WaitForDeferThread();
                // Skip submitting to realtime pipelines when an input buffer is provided
                if (TRUE == m_sessions[session].pipelines[pipeline].pPipeline->IsRealTime())
                {
                    continue;
                }
            }
            else
            {
                // Skip submitting to offline pipelines when an input buffer is not provided
                if (FALSE == m_sessions[session].pipelines[pipeline].pPipeline->IsRealTime())
                {
                    continue;
                }
            }

            camera3_stream_buffer_t   outputBuffers[NumOutputBuffers] = { { 0 } };
            UINT32                    outputCount                     = 0;
            PipelineData*             pPipelineData                   = &m_sessions[session].pipelines[pipeline];
            for (UINT32 buffer = 0; buffer < pRequest->num_output_buffers; buffer++)
            {
                for (UINT stream = 0; stream < pPipelineData->numStreams; stream++)
                {
                    if ( (TRUE == bIsOffline) &&
                        (FALSE == m_sessions[session].pipelines[pipeline].pPipeline->IsRealTime()) &&
                        (TRUE  == m_bCloningNeeded) )
                    {
                        UINT index = 0;
                        if (TRUE == IsThisClonedStream(m_pClonedStream, pPipelineData->pStreams[stream], &index))
                        {
                            if ((reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[buffer].stream) ==
                                m_pFrameworkOutStreams[index]))
                            {
                                ChxUtils::Memcpy(&outputBuffers[outputCount],
                                    &pRequest->output_buffers[buffer],
                                    sizeof(camera3_stream_buffer_t));
                                outputBuffers[outputCount].stream = reinterpret_cast<camera3_stream_t*>(pPipelineData->pStreams[stream]);
                                outputCount++;
                            }
                        }
                    }
                    else
                    {
                        if (reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[buffer].stream) ==
                            pPipelineData->pStreams[stream])
                        {
                            ChxUtils::Memcpy(&outputBuffers[outputCount],
                                             &pRequest->output_buffers[buffer],
                                             sizeof(camera3_stream_buffer_t));
                            outputCount++;
                        }
                    }
                }
            }

            if (0 < outputCount)
            {
                CHICAPTUREREQUEST request = { 0 };
                UINT32            sensorModeIndex;

                if (NULL != pRequest->input_buffer)
                {
                    request.numInputs     = 1;
                    request.pInputBuffers = reinterpret_cast<CHISTREAMBUFFER*>(pRequest->input_buffer);
                }

                request.frameNumber       = pRequest->frame_number;
                request.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(
                                                m_sessions[session].pSession->GetPipelineHandle());
                request.numOutputs        = outputCount;
                request.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(outputBuffers);
                request.pMetadata         = pRequest->settings;
                request.pPrivData         = &m_privData[frameIndex];

                sensorModeIndex           = m_sessions[session].pSession->GetSensorModeInfo(pipeline)->modeIndex;
                ChxUtils::FillTuningModeData(const_cast<VOID*>(request.pMetadata), pRequest, sensorModeIndex,
                    &m_effectModeValue, &m_sceneModeValue, &m_tuningFeature1Value, &m_tuningFeature2Value);
                ChxUtils::FillCameraId(const_cast<VOID*>(request.pMetadata), GetCameraId());

                CHIPIPELINEREQUEST submitRequest = { 0 };
                submitRequest.pSessionHandle     = reinterpret_cast<CHIHANDLE>(
                                                        m_sessions[session].pSession->GetSessionHandle());
                submitRequest.numRequests        = 1;
                submitRequest.pCaptureRequests   = &request;

                if (FALSE == m_sessions[session].pSession->IsPipelineActive())
                {
                    if (((TRUE == m_sessions[session].pipelines[pipeline].pPipeline->IsRealTime()) &&
                       (ExtensionModule::GetInstance()->GetNumPCRsBeforeStreamOn(const_cast<camera_metadata_t*>(pRequest->settings))
                           == pRequest->frame_number)) ||
                       (FALSE  == m_sessions[session].pipelines[pipeline].pPipeline->IsRealTime()))
                    {
                        result =
                            ExtensionModule::GetInstance()->ActivatePipeline(m_sessions[session].pSession->GetSessionHandle(),
                                                                             m_sessions[session].pSession->GetPipelineHandle());
                        if (CDKResultSuccess == result)
                        {
                            m_sessions[session].pSession->SetPipelineActivateFlag();
                        }
                    }
                }

                CHX_LOG("Submitting request to Session %d Pipeline %d outputCount=%d", session, pipeline, outputCount);
                result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::ReplaceMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::MergeMetadata(
    camera_metadata_t*       pDst,
    const camera_metadata_t* pSrc)
{
    CDKResult result = CDKResultSuccess;
    INT       status = 0;

    status = ChxUtils::MergeMetadata(const_cast<camera_metadata_t*>(pSrc), pDst);

    if (0 == status) // Indicates success
    {
        // Add default bps sensor metdata for gain if not present
        if (FALSE == ChxUtils::IsVendorTagPresent(pDst, SensorBpsGain))
        {
            CHX_LOG("BPS gain not present. Filling with default");
            UINT  sensorBpsModeIndex = 0;
            ChxUtils::SetVendorTagValue(pDst, SensorBpsModeIndex, 1, &sensorBpsModeIndex);
        }

        // Add default bps sensor metdata for id if not present
        if (FALSE == ChxUtils::IsVendorTagPresent(pDst, SensorBpsModeIndex))
        {
            CHX_LOG("BPS Mode not present. Filling with default");
            FLOAT sensorBpsGainValue = 1.0f;
            ChxUtils::SetVendorTagValue(pDst, SensorBpsGain, 1, &sensorBpsGainValue);
        }

        // Add DebugData
        if (FALSE == ChxUtils::IsVendorTagPresent(pDst, DebugDataTag))
        {
            CHX_LOG("DebugData was not present in the original request. Reprocess likely to fail");
        }
    }
    else
    {
        CHX_LOG("Couldnt update ZSL buffer metadata tags");
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::SessionCbNotifyMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::SessionCbNotifyMessage(
    const ChiMessageDescriptor* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{
    /// @todo Provide common Get functions to do the below two casts
    SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    CameraUsecaseBase*  pCameraUsecase      = static_cast<CameraUsecaseBase*>(pSessionPrivateData->pUsecase);

    pCameraUsecase->SessionProcessMessage(pMessageDescriptor, pSessionPrivateData);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::SessionCbCaptureResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::SessionCbCaptureResult(
    ChiCaptureResult* pCaptureResult,
    VOID*             pPrivateCallbackData)
{
    SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    CameraUsecaseBase*  pCameraUsecase      = static_cast<CameraUsecaseBase*>(pSessionPrivateData->pUsecase);

    for (UINT stream = 0; stream < pCaptureResult->numOutputBuffers; stream++)
    {
        UINT index = 0;
        if (TRUE == IsThisClonedStream(pCameraUsecase->m_pClonedStream, pCaptureResult->pOutputBuffers[stream].pStream, &index))
        {
            CHISTREAMBUFFER* pTempStreamBuffer  = const_cast<CHISTREAMBUFFER*>(&pCaptureResult->pOutputBuffers[stream]);
            pTempStreamBuffer->pStream          = pCameraUsecase->m_pFrameworkOutStreams[index];
        }
    }

    pCameraUsecase->SessionProcessResult(pCaptureResult, pSessionPrivateData);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::SessionProcessMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::SessionProcessMessage(
    const ChiMessageDescriptor* pMessageDescriptor,
    const SessionPrivateData*   pSessionPrivateData)
{
    (void)pSessionPrivateData;
    if (ChiMessageTypeSof == pMessageDescriptor->messageType)
    {
        // SOF notifications are not sent to the HAL3 application
        CHX_LOG("ZSL Chi Notify SOF frameNum %u framework frameNum %u, timestamp %" PRIu64,
            pMessageDescriptor->message.sofMessage.sofId,
            pMessageDescriptor->message.sofMessage.frameworkFrameNum,
            pMessageDescriptor->message.sofMessage.timestamp);
    }
    else if (ChiMessageTypeError == pMessageDescriptor->messageType)
    {
        ProcessErrorMessage(pMessageDescriptor);
    }
    else
    {
        ReturnFrameworkMessage((camera3_notify_msg_t*)pMessageDescriptor, m_cameraId);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::SessionProcessResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::SessionProcessResult(
    ChiCaptureResult*         pResult,
    const SessionPrivateData* pSessionPrivateData)
{
    (void)pSessionPrivateData;
    UINT32              resultFrameNum          = pResult->frameworkFrameNum;
    UINT32              resultFrameIndex        = resultFrameNum % MaxOutstandingRequests;
    BOOL                isAppResultsAvailable   = FALSE;
    camera3_capture_result_t* pInternalResult   = reinterpret_cast<camera3_capture_result_t*>(pResult);
    camera3_capture_result_t* pUsecaseResult    = this->GetCaptureResult(resultFrameIndex);

    pUsecaseResult->frame_number                = resultFrameNum;
    camera_metadata_t* pMetadata                =
        const_cast<camera_metadata_t*>(static_cast<const camera_metadata_t*>(pResult->pResultMetadata));

    // Fill all the info in m_captureResult and call ProcessAndReturnFinishedResults to send the meta
    // callback in sequence
    m_pAppResultMutex->Lock();
    for (UINT i = 0; i < pResult->numOutputBuffers; i++)
    {
        camera3_stream_buffer_t* pResultBuffer =
            const_cast<camera3_stream_buffer_t*>(&pUsecaseResult->output_buffers[i + pUsecaseResult->num_output_buffers]);

        ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[i], sizeof(camera3_stream_buffer_t));
        isAppResultsAvailable = TRUE;
    }
    pUsecaseResult->num_output_buffers += pResult->numOutputBuffers;
    m_pAppResultMutex->Unlock();

    if (NULL != &pResult->pInputBuffer[0])
    {
        camera3_stream_buffer_t* pResultInBuffer =
            const_cast<camera3_stream_buffer_t*>(pUsecaseResult->input_buffer);
        ChxUtils::Memcpy(pResultInBuffer, pResult->pInputBuffer, sizeof(camera3_stream_buffer_t));
        isAppResultsAvailable = TRUE;
    }

    if (NULL != pMetadata)
    {
        SetMetadataAvailable(resultFrameIndex);
        pUsecaseResult->result          = static_cast<const camera_metadata_t*>(pResult->pResultMetadata);
        pUsecaseResult->partial_result  = pResult->numPartialMetadata;
        isAppResultsAvailable           = TRUE;
    }

    if (TRUE == isAppResultsAvailable)
    {
        ProcessAndReturnFinishedResults();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::ProcessAndReturnFinishedResults
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::ProcessAndReturnFinishedResults()
{
    m_pAppResultMutex->Lock();
    for (UINT64 resultFrame = m_nextAppResultFrame; resultFrame <= m_lastAppRequestFrame; resultFrame++)
    {
        UINT frameIndex        = (resultFrame % MaxOutstandingRequests);
        BOOL metadataAvailable = ((NULL != m_captureResult[frameIndex].result)         &&
                                  (0    != m_captureResult[frameIndex].partial_result) &&
                                  (TRUE == m_isMetadataAvailable[frameIndex])) ? TRUE : FALSE;
        BOOL metadataReturn    = ((TRUE                         == metadataAvailable) &&
                                  (m_lastResultMetadataFrameNum == static_cast<INT64>(resultFrame - 1))) ? TRUE : FALSE;
        BOOL bufferReturn      = (0 < m_captureResult[frameIndex].num_output_buffers) ? TRUE : FALSE;
        BOOL inBufferReturn    = FALSE;
        if (NULL != m_captureResult[frameIndex].input_buffer)
        {
            inBufferReturn    = (NULL == m_captureResult[frameIndex].input_buffer->stream) ? FALSE : TRUE;
        }
        if ((TRUE == bufferReturn) || (TRUE == metadataReturn))
        {
            CHX_ASSERT(0 < m_numAppPendingOutputBuffers[frameIndex]);

            if (TRUE == metadataReturn)
            {
                CHX_LOG("Frame %" PRIu64 ": Returning metadata partial result %d for app frame %d",
                        resultFrame,
                        m_captureResult[frameIndex].partial_result,
                        m_captureResult[frameIndex].frame_number);
                camera3_capture_result_t result = { 0 };
                result.frame_number             = m_captureResult[frameIndex].frame_number;
                result.partial_result           = m_captureResult[frameIndex].partial_result;
                result.result                   = m_captureResult[frameIndex].result;
                Usecase::ReturnFrameworkResult(&result, m_cameraId);
            }

            if ((TRUE == metadataReturn) &&
                (m_captureResult[frameIndex].partial_result == ExtensionModule::GetInstance()->GetNumMetadataResults()))
            {
                m_isMetadataSent[frameIndex]            = TRUE;
                // Reset the meta related info in corresponding capture result
                m_captureResult[frameIndex].result      = NULL;
                m_isMetadataAvailable[frameIndex]       = FALSE;
            }

            if ((TRUE == bufferReturn) && (TRUE ==  m_isMetadataSent[frameIndex]))
            {
                CHX_LOG("Frame %" PRIu64 ": Returning buffer result for %d output buffers for app frame %d",
                        resultFrame,
                        m_captureResult[frameIndex].num_output_buffers,
                        m_captureResult[frameIndex].frame_number);

                for (UINT i = 0; i < m_captureResult[frameIndex].num_output_buffers; i++)
                {
                    CHX_LOG("\tStream %p, W: %d, H: %d",
                            m_captureResult[frameIndex].output_buffers[i].stream,
                            m_captureResult[frameIndex].output_buffers[i].stream->width,
                            m_captureResult[frameIndex].output_buffers[i].stream->height);
                }

                camera3_capture_result_t result = { 0 };

                result.frame_number       = m_captureResult[frameIndex].frame_number;
                result.num_output_buffers = m_captureResult[frameIndex].num_output_buffers;
                result.output_buffers     = m_captureResult[frameIndex].output_buffers;

                ReturnFrameworkResult(&result, m_cameraId);

                // Decrease the pending output buffers number by the number of output buffer sent now.
                // Reset the num_output_buffers in capture result too.
                m_numAppPendingOutputBuffers[frameIndex]      -= m_captureResult[frameIndex].num_output_buffers;

                // send the input buffer seperately once all corresponding outut buffers of that
                // request are sent
                if ((TRUE == inBufferReturn) && (0 == m_numAppPendingOutputBuffers[frameIndex]))
                {
                    ChxUtils::Memset(&result, 0, sizeof(camera3_capture_result_t));

                    result.frame_number   = m_captureResult[frameIndex].frame_number;
                    result.input_buffer   = m_captureResult[frameIndex].input_buffer;

                    CHX_LOG("Frame %" PRIu64 ": Returning input buffer", resultFrame);

                    ReturnFrameworkResult(&result, m_cameraId);

                    ChxUtils::Memset(const_cast<camera3_stream_buffer_t*>(m_captureResult[frameIndex].input_buffer),
                        0, sizeof(camera3_stream_buffer_t));

                }
                m_captureResult[frameIndex].num_output_buffers = 0;
            }
        }

        // Handles the error cases which can set m_isMetadataSent, but may do it out of order. This function
        // steps through the requests, including ones in error, and will move m_lastResultMetadataFrameNum here
        if ((TRUE == m_isMetadataSent[frameIndex]) && (m_lastResultMetadataFrameNum == static_cast<INT64>(resultFrame - 1)))
        {
            m_lastResultMetadataFrameNum++;
        }

        // Moving forward if there is no pending output buffer and metadata
        if ((0 == m_numAppPendingOutputBuffers[frameIndex]) && (TRUE == m_isMetadataSent[frameIndex]))
        {
            CHX_LOG("Frame %" PRIu64 " has returned all results", resultFrame);

            // Don't increment m_nextAppResultFrame if we are not done with that
            // frame yet
            if (resultFrame == m_nextAppResultFrame)
            {
                m_nextAppResultFrame++;
                CHX_LOG("Advanced next result frame to %" PRIu64, m_nextAppResultFrame);
            }
        }
    }
    m_pAppResultMutex->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::ProcessErrorMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::ProcessErrorMessage(
    const ChiMessageDescriptor* pMessageDescriptor)
{
    CHX_ASSERT(ChiMessageTypeError == pMessageDescriptor->messageType);

    UINT64 resultFrame = pMessageDescriptor->message.errorMessage.frameworkFrameNum;
    UINT64 frameIndex  = resultFrame % MaxOutstandingRequests;

    switch (pMessageDescriptor->message.errorMessage.errorMessageCode)
    {
        case MessageCodeRequest:
            CHX_LOG("Frame %" PRIu64 ": Request error. Only this error will be reported", resultFrame);
            m_isMetadataSent[frameIndex]             = TRUE;
            break;
        case MessageCodeResult:
            CHX_LOG("Frame %" PRIu64 ": Metadata error", resultFrame);
            m_isMetadataSent[frameIndex]             = TRUE;
            break;
        case MessageCodeBuffer:
            CHX_LOG("Frame %" PRIu64 ": Buffer error for stream %p", resultFrame, pMessageDescriptor->message.errorMessage.pErrorStream);
            CHX_ASSERT(0 < m_numAppPendingOutputBuffers[frameIndex]);
            break;
        case MessageCodeDevice:
            CHX_LOG("Not handling device errors");
            break;
    }

    if ((0 == m_numAppPendingOutputBuffers[frameIndex]) && (TRUE == m_isMetadataSent[frameIndex]))
    {
        CHX_LOG("Frame %" PRIu64 " has returned all results (with some errors)", resultFrame);

        // Only increment m_nextAppResultFrame if we are done it
        if (resultFrame == m_nextAppResultFrame)
        {
            m_nextAppResultFrame++;
            CHX_LOG("Advanced next result frame to %" PRIu64, m_nextAppResultFrame);
        }
    }

    // If we move m_nextAppResultFrame without moving this, the loop in ProcessAndReturnFinishedResults won't catch it
    if ((m_lastResultMetadataFrameNum == static_cast<INT64>(resultFrame - 1)) && (TRUE == m_isMetadataSent[frameIndex]))
    {
        m_lastResultMetadataFrameNum++;
    }

    ReturnFrameworkMessage((camera3_notify_msg_t*)pMessageDescriptor, m_cameraId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CameraUsecaseBase::Flush()
{
    ATRACE_BEGIN("CameraUsecaseBase::Flush");
    Usecase::FlushLock(TRUE);

    for (UINT session = 0; session < MaxSessions; session++)
    {
        if (NULL != m_sessions[session].pSession)
        {
            ExtensionModule::GetInstance()->Flush(m_sessions[session].pSession->GetSessionHandle());
        }
    }
    Usecase::FlushLock(FALSE);
    ATRACE_END();
    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::DumpDebugData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::DumpDebugData(
    const VOID*     pDebugData,
    const SIZE_T    sizeDebugData,
    const UINT32    sessionId,
    const UINT32    cameraId,
    const UINT32    resultFrameNumber)
{
    CHAR dumpFilename[256];
    CHAR suffix[] = "bin";
    const CHAR parserString[19] = "QTI Debug Metadata";
    DebugDataStartHeader metadataHeader;
    char timeBuf[128]= { 0 };
    time_t currentTime;
    time (&currentTime);
    struct tm* timeInfo = localtime (&currentTime);

    if (NULL != timeInfo) {
        strftime(timeBuf, sizeof(timeBuf), "%Y%m%d_%H%M%S", timeInfo);
    }

    if (resultFrameNumber == m_debugLastResultFrameNumber)
    {
        m_debugMultiPassCount++;
    }
    else
    {
        m_debugMultiPassCount = 0;
    }

    CdkUtils::SNPrintF(dumpFilename, sizeof(dumpFilename),
                       "%s/%s_debug-data_session[%u]_cameraId[%u]_req[%u]_cnt[%u].%s",
                       FileDumpPath,
                       timeBuf, sessionId, cameraId, resultFrameNumber, m_debugMultiPassCount, suffix);

    metadataHeader.dataSize                = sizeof(parserString) + sizeof(metadataHeader) + sizeDebugData;
    metadataHeader.majorRevision           = 1;
    metadataHeader.minorRevision           = 1;
    metadataHeader.patchRevision           = 0;
    metadataHeader.SWMajorRevision         = 1;
    metadataHeader.SWMinorRevision         = 0;
    metadataHeader.SWPatchRevision         = 0;
    metadataHeader.featureDesignator[0]    = 'R';
    metadataHeader.featureDesignator[1]    = 'C';

    CHX_LOG_INFO("DebugDataAll: dumpFilename: %s, pDebugData: %p, sizeDebugData: %zu, sizeMeta: %u [0x%x]",
                 dumpFilename, pDebugData, sizeDebugData, metadataHeader.dataSize, metadataHeader.dataSize);

    FILE* pFile = CdkUtils::FOpen(dumpFilename, "wb");
    if (NULL != pFile)
    {
        CdkUtils::FWrite(parserString, sizeof(parserString), 1, pFile);
        CdkUtils::FWrite(&metadataHeader, sizeof(metadataHeader), 1, pFile);
        CdkUtils::FWrite(pDebugData, sizeDebugData, 1, pFile);
        CdkUtils::FClose(pFile);
    }
    else
    {
        CHX_LOG_ERROR("DebugDataAll: Debug data failed to open for writing: %s", dumpFilename);
    }

    m_debugLastResultFrameNumber = resultFrameNumber;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CameraUsecaseBase::ProcessDebugData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CameraUsecaseBase::ProcessDebugData(
    const CHICAPTURERESULT* pResult,
    const VOID*             pPrivateCallbackData,
    const UINT32            resultFrameNum)
{
    const SessionPrivateData* pCbData = static_cast<const SessionPrivateData*>(pPrivateCallbackData);
    if ((NULL != pResult->pResultMetadata))
    {
        const camera_metadata_t* pMetadata  =
            const_cast<camera_metadata_t*>(static_cast<const camera_metadata_t*>(pResult->pResultMetadata));
        if (TRUE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(pMetadata), DebugDataTag))
        {
            CHAR* pData = NULL;
            ChxUtils::GetVendorTagValue((VOID*)(pMetadata), DebugDataTag, (VOID**)&pData);

            if (NULL != pData)
            {
                DebugData* pDebug = reinterpret_cast<DebugData*>(pData);
                if ((NULL != pDebug->pData) && (0 < pDebug->size))
                {
                    DumpDebugData(pDebug->pData,
                                  pDebug->size,
                                  pCbData->sessionId,
                                  pCbData->pUsecase->GetCameraId(),
                                  resultFrameNum);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class AdvancedCameraUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::~AdvancedCameraUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AdvancedCameraUsecase::~AdvancedCameraUsecase()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AdvancedCameraUsecase* AdvancedCameraUsecase::Create(
    LogicalCameraInfo*              pCameraInfo,   ///< Camera info
    camera3_stream_configuration_t* pStreamConfig, ///< Stream configuration
    UsecaseId                       usecaseId)     ///< Identifier for usecase function
{
    CDKResult              result                 = CDKResultSuccess;
    AdvancedCameraUsecase* pAdvancedCameraUsecase = CHX_NEW AdvancedCameraUsecase;

    if (NULL != pAdvancedCameraUsecase)
    {
        result = pAdvancedCameraUsecase->Initialize(pCameraInfo, pStreamConfig, usecaseId);

        if (CDKResultSuccess != result)
        {
            pAdvancedCameraUsecase->Destroy(FALSE);
            pAdvancedCameraUsecase = NULL;
        }
    }
    else
    {
        result = CDKResultEFailed;
    }

    return pAdvancedCameraUsecase;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AdvancedCameraUsecase::Flush()
{
    CHX_LOG("AdvancedCameraUsecase::Flush");
    //Pause features to take any actions on flushed buffers
    for (UINT32 i = 0; i < m_enabledFeaturesCount; i++)
    {
        CHX_LOG("AdvancedCameraUsecase FLUSH started FEATURE TYPE=%d", m_enabledFeatures[i]->GetFeatureType());
        m_enabledFeatures[i]->Flush(TRUE);
    }

    CameraUsecaseBase::Flush();

    for (UINT32 i = 0; i < m_enabledFeaturesCount; i++)
    {
        CHX_LOG("AdvancedCameraUsecase FLUSH Done FEATURE TYPE=%d", m_enabledFeatures[i]->GetFeatureType());
        m_enabledFeatures[i]->Flush(FALSE);
    }
    return CDKResultSuccess;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID AdvancedCameraUsecase::Destroy(BOOL isForced)
{
    //Pause features to take any actions on flushed buffers
    if (0 != m_enabledFeaturesCount)
    {
        for (UINT32 i = 0; i < m_enabledFeaturesCount; i++)
        {
            CHX_LOG("AdvancedCameraUsecase Pause FEATURE TYPE=%d", m_enabledFeatures[i]->GetFeatureType());
            m_enabledFeatures[i]->Pause(FALSE);
        }
    }

    if (NULL != m_pRawJPEG)
    {
        m_pRawJPEG->Pause(FALSE);
    }
    m_pRawJPEG = NULL;
    CameraUsecaseBase::Destroy(isForced);

    if (NULL != m_pResultMutex)
    {
        m_pResultMutex->Destroy();
        m_pResultMutex = NULL;
    }

    if ((NULL != m_pRdiStream) && (FALSE == m_isRdiStreamImported))
    {
        /// multi camera usecase will handle RDI stream release
        if (FALSE == IsMultiCameraUsecase())
        {
            CHX_FREE(m_pRdiStream);
            m_pRdiStream = NULL;
        }
    }
    m_pRdiStream = NULL;
    if (NULL != m_pBayer2YuvStream)
    {
        CHX_FREE(m_pBayer2YuvStream);
        m_pBayer2YuvStream = NULL;
    }

    if (NULL != m_pJPEGInputStream)
    {
        CHX_FREE(m_pJPEGInputStream);
        m_pJPEGInputStream = NULL;
    }

    m_pSnapshotStream   = NULL;
    m_pPreviewStream    = NULL;
    m_pYuvInStream      = NULL;
    m_pYuvCBStream      = NULL;
    m_pVideoStream      = NULL;
    m_pRdiStreamAux     = NULL;
    if (0 != m_enabledFeaturesCount)
    {
        for (UINT32 i = 0; i < m_enabledFeaturesCount; i++)
        {
            m_enabledFeatures[i]->Destroy(isForced);
            m_enabledFeatures[i] = NULL;
        }
        m_enabledFeaturesCount = 0;

        CHX_FREE(m_pChiUsecase->pPipelineTargetCreateDesc);
        m_pChiUsecase->pPipelineTargetCreateDesc = NULL;
        CHX_FREE(m_pChiUsecase);
        m_pChiUsecase = NULL;
    }

    if (NULL != m_pRawJPEG)
    {
        m_pRawJPEG->Destroy(isForced);
    }

    if (NULL != m_pCallbacks)
    {
        CHX_FREE(m_pCallbacks);
    }

    if (NULL != m_pLastSettingsWithIrLedOn)
    {
        CHX_LOG_INFO("Freeing last known request setting");
        free_camera_metadata(m_pLastSettingsWithIrLedOn);
        m_pLastSettingsWithIrLedOn = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AdvancedCameraUsecase::Initialize(
    LogicalCameraInfo*              pCameraInfo,   ///< Camera info
    camera3_stream_configuration_t* pStreamConfig, ///< Stream configuration
    UsecaseId                       usecaseId)     ///< Identifier for the usecase function
{
    ATRACE_BEGIN("AdvancedCameraUsecase::Initialize");
    CDKResult result = CDKResultSuccess;

    m_usecaseId                     = usecaseId;
    m_cameraId                      = pCameraInfo->cameraId;
    m_pLogicalCameraInfo            = pCameraInfo;

    m_pResultMutex                  = Mutex::Create();

    ExtensionModule::GetInstance()->GetVendorTagOps(&m_vendorTagOps);
    CHX_LOG("pGetMetaData:%p, pSetMetaData:%p", m_vendorTagOps.pGetMetaData, m_vendorTagOps.pSetMetaData);

    ChxUtils::Memset(m_enabledFeatures, 0, sizeof(m_enabledFeatures));
    m_enabledFeaturesCount = 0;
    m_firstFrameAfterIrLedOn = 0;
    m_pLastSettingsWithIrLedOn = NULL;
    m_pRawJPEG = NULL;
    m_pPipelineToSession = NULL;
    m_pPipelineToCamera = NULL;
    if (TRUE == IsMultiCameraUsecase())
    {
        CHX_ASSERT(m_pRdiStream != NULL);

        if (NULL == m_pRdiStream)
        {
            CHX_LOG("[ERROR] m_pRdiStream must be not NULL in multi camera case.");
            result = CDKResultEFailed;
        }
        else
        {
            m_isRdiStreamImported   = TRUE;
            CHX_LOG("m_pRdiStream, addr:%p, %dx%d, chistream wrapper:%p",
                m_pRdiStream, m_pRdiStream->width, m_pRdiStream->height, m_pRdiStream->pPrivateInfo);
        }
    }
    else
    {
        m_pRdiStream                = static_cast<CHISTREAM*>(CHX_CALLOC(sizeof(CHISTREAM)));
        m_isRdiStreamImported       = FALSE;
    }

    m_pBayer2YuvStream              = static_cast<CHISTREAM*>(CHX_CALLOC(sizeof(CHISTREAM)));
    m_pJPEGInputStream              = static_cast<CHISTREAM*>(CHX_CALLOC(sizeof(CHISTREAM)));

    for (UINT32 i = 0; i < AdvancedPipelineType::PipelineCount; i++)
    {
        m_pipelineToSession[i] = InvalidSessionId;
    }
    m_realtimeSessionId = static_cast<UINT32>(InvalidSessionId);

    CHX_LOG("AdvancedCameraUsecase::Initialize, usecaseId:%d", m_usecaseId);
    CHX_LOG("CHI Input Stream Configs:");

    for (UINT stream = 0; stream < pStreamConfig->num_streams; stream++)
    {
        CHX_LOG("\tstream = %p streamType = %d streamFormat = %d streamWidth = %d streamHeight = %d",
            pStreamConfig->streams[stream],
            pStreamConfig->streams[stream]->stream_type,
            pStreamConfig->streams[stream]->format,
            pStreamConfig->streams[stream]->width,
            pStreamConfig->streams[stream]->height);
    }

    FeatureSetup(pStreamConfig);

    if (CDKResultSuccess == result)
    {
        result = SelectUsecaseConfig(pCameraInfo, pStreamConfig);
    }

    if ((NULL != m_pChiUsecase) && (CDKResultSuccess == result))
    {
        CHX_LOG("Usecase %s selected", m_pChiUsecase->pUsecaseName);

        m_pCallbacks = static_cast<ChiCallBacks*>(CHX_CALLOC(sizeof(ChiCallBacks) * m_pChiUsecase->numPipelines));

        CHX_LOG("Pipelines need to create in advance usecase:%d", m_pChiUsecase->numPipelines);
        for (UINT i = 0; i < m_pChiUsecase->numPipelines; i++)
        {
            CHX_LOG("[%d/%d], pipeline name:%s, pipeline type:%d, session id:%d, camera id:%d",
                i,
                m_pChiUsecase->numPipelines,
                m_pChiUsecase->pPipelineTargetCreateDesc[i].pPipelineName,
                GetAdvancedPipelineTypeByPipelineId(i),
                (NULL != m_pPipelineToSession) ? m_pPipelineToSession[i] : i,
                m_pPipelineToCamera[i]);
        }

        if (NULL != m_pCallbacks)
        {
            for (UINT i = 0; i < m_pChiUsecase->numPipelines; i++)
            {
                m_pCallbacks[i].ChiNotify               = AdvancedCameraUsecase::ProcessMessageCb;
                m_pCallbacks[i].ChiProcessCaptureResult = AdvancedCameraUsecase::ProcessResultCb;
            }

            result = CameraUsecaseBase::Initialize(m_pCallbacks);
        }
        PostUsecaseCreation(pStreamConfig);
    }
    else
    {
        result = CDKResultEFailed;
    }

    ATRACE_END();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::PreUsecaseSelection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AdvancedCameraUsecase::FeatureSetup(
    camera3_stream_configuration_t* pStreamConfig)  ///< Stream configuration
{
    (void)pStreamConfig;
    CDKResult result = CDKResultSuccess;

    if ((UsecaseId::PreviewZSL == m_usecaseId) || (UsecaseId::YUVInBlobOut == m_usecaseId) ||
         (UsecaseId::VideoLiveShot == m_usecaseId))
    {
        SelectFeatures(pStreamConfig);
    }
    else if (UsecaseId::MultiCamera == m_usecaseId)
    {
        SelectFeatures(pStreamConfig);
    }
    else if (UsecaseId::RawJPEG == m_usecaseId)
    {
        m_pRawJPEG = FeatureRawJPEG::Create(this);

        if (NULL == m_pRawJPEG)
        {
            result = CDKResultEFailed;
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::SelectUsecaseConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AdvancedCameraUsecase::SelectUsecaseConfig(
    LogicalCameraInfo*              pCameraInfo,   ///< Camera info
    camera3_stream_configuration_t* pStreamConfig)  ///< Stream configuration
{
    CDKResult result               = CDKResultSuccess;
    UINT32    idealRawEnable       = 0;
    UINT32    eisDGEnable          = 0;
    UINT32    LDCEnable            = 0;
    BOOL      bEISDGEnabled        = FALSE;
    BOOL      bCustom1EISDGEnabled = FALSE;
    BOOL      bCustom2EISDGEnabled = FALSE;

    idealRawEnable = ExtensionModule::GetInstance()->GetIdealRawEnable();
    LDCEnable      = ExtensionModule::GetInstance()->GetLDCEnable();
    eisDGEnable    = ExtensionModule::GetInstance()->GetEISDGEnable();

    if (0 != eisDGEnable)
    {
        if (2 == eisDGEnable)
        {
            // Force EISDG usecase, select single stream EIS usecase only
            pStreamConfig->operation_mode = pStreamConfig->operation_mode | StreamConfigModeQTIEISDG;
        }

        if ((pStreamConfig->operation_mode & StreamConfigModeQTIEISDG) == StreamConfigModeQTIEISDG)
        {
            bEISDGEnabled = TRUE;
        }

        if ((pStreamConfig->operation_mode & StreamConfigModeQTIEISDGCustom1) == StreamConfigModeQTIEISDGCustom1)
        {
            if ((pStreamConfig->operation_mode & StreamConfigModeQTIEISDGCustom2) == StreamConfigModeQTIEISDGCustom2)
            {
                bCustom2EISDGEnabled = TRUE;
            }
            else
            {
                bCustom1EISDGEnabled = TRUE;
            }
        }

    }

    if ((UsecaseId::PreviewZSL == m_usecaseId) || (UsecaseId::YUVInBlobOut == m_usecaseId) ||
        (UsecaseId::VideoLiveShot == m_usecaseId))
    {
        BuildUsecase(pCameraInfo, pStreamConfig);
    }
    else if (UsecaseId::MultiCamera == m_usecaseId)
    {
        CHX_LOG("Build usecase for multicamera usecase");
        BuildUsecase(pCameraInfo, pStreamConfig);
    }
    else if (UsecaseId::RawJPEG == m_usecaseId)
    {
        m_pChiUsecase = m_pRawJPEG->OverrideUsecase(pCameraInfo, pStreamConfig);
    }
    else
    {
        CHX_LOG("Initializing using default usecase matching ");
        m_cameraId    = pCameraInfo->ppDeviceInfo[0]->cameraId;
        if(idealRawEnable && (pStreamConfig->num_streams == 2))
        {
            m_pChiUsecase = &Usecases2Target[UsecasePreviewIdealRawId];
            for (UINT targetInfoIdx = 0; targetInfoIdx < 2; targetInfoIdx++)
            {
                ChiStream* pStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[targetInfoIdx]);
                ChiTarget* pTargetInfo = m_pChiUsecase->ppChiTargets[targetInfoIdx];
                pTargetInfo->pChiStream = pStream;
            }
            CHX_LOG("Ideal Raw Enabled through overrride");
        }
        else if((bEISDGEnabled || bCustom1EISDGEnabled) && (pStreamConfig->num_streams == 1))
        {
            m_pChiUsecase = &Usecases1Target[UsecasePreviewEISDGId];
            for (UINT targetInfoIdx = 0; targetInfoIdx < 1; targetInfoIdx++)
            {
                ChiStream* pStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[targetInfoIdx]);
                ChiTarget* pTargetInfo = m_pChiUsecase->ppChiTargets[targetInfoIdx];
                pTargetInfo->pChiStream = pStream;
            }
            CHX_LOG("EIS DG enabled for single stream");
        }
        else if(bCustom1EISDGEnabled && (pStreamConfig->num_streams == 4))
        {
            m_pChiUsecase = &Usecases4Target[UsecaseJPEGEncodeLiveRawAndJpegSnapshotWithEISDGId];
            CHX_LOG("EIS DG enabled for main stream for Custom1. Total 4 streams numTargets = %d", m_pChiUsecase->numTargets);
            for (UINT targetInfoIdx = 0; targetInfoIdx < 4 ; targetInfoIdx++)
            {
                ChiStream* pStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[targetInfoIdx]);
                ChiTarget* pTargetInfo = m_pChiUsecase->ppChiTargets[targetInfoIdx];
                pTargetInfo->pChiStream = pStream;
            }
            CHX_LOG("EIS DG enabled for main stream for Custom1. Total 4 streams");
        }
        else if(bCustom1EISDGEnabled && (pStreamConfig->num_streams == 3))
        {
            m_pChiUsecase = &Usecases3Target[UsecaseJPEGEncodeLiveSnapshotWithEISDGId];
            for (UINT targetInfoIdx = 0; targetInfoIdx < 3; targetInfoIdx++)
            {
                ChiStream* pStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[targetInfoIdx]);
                ChiTarget* pTargetInfo = m_pChiUsecase->ppChiTargets[targetInfoIdx];
                pTargetInfo->pChiStream = pStream;
            }
            CHX_LOG("EIS DG enabled for main stream for Custom1. Total 3 streams");
        }
        else if(bCustom1EISDGEnabled && (pStreamConfig->num_streams == 2))
        {
            m_pChiUsecase = &Usecases2Target[UsecaseVideoWithEISDGId];
            for (UINT targetInfoIdx = 0; targetInfoIdx < 2; targetInfoIdx++)
            {
                ChiStream* pStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[targetInfoIdx]);
                ChiTarget* pTargetInfo = m_pChiUsecase->ppChiTargets[targetInfoIdx];
                pTargetInfo->pChiStream = pStream;
            }
            CHX_LOG("EIS DG enabled for main stream for Custom1 total 2 streams");
        }
        else if(bCustom2EISDGEnabled && (pStreamConfig->num_streams == 2))
        {
            m_pChiUsecase = &Usecases2Target[UsecasePreviewEISDGLDCTwoStreamId];
            for (UINT targetInfoIdx = 0; targetInfoIdx < 2; targetInfoIdx++)
            {
                ChiStream* pStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[targetInfoIdx]);
                ChiTarget* pTargetInfo = m_pChiUsecase->ppChiTargets[targetInfoIdx];
                pTargetInfo->pChiStream = pStream;
            }
            CHX_LOG("EIS DG enabled for Custom2 on both streams");
        }
        else if(bCustom2EISDGEnabled && (pStreamConfig->num_streams == 1))
        {
            m_pChiUsecase = &Usecases1Target[UsecasePreviewEISDGLDCId];
            for (UINT targetInfoIdx = 0; targetInfoIdx < 1; targetInfoIdx++)
            {
                ChiStream* pStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[targetInfoIdx]);
                ChiTarget* pTargetInfo = m_pChiUsecase->ppChiTargets[targetInfoIdx];
                pTargetInfo->pChiStream = pStream;
            }
            CHX_LOG("EIS DG enabled for Custom2 on single stream");
        }
        else if (TRUE == LDCEnable)
        {
            if (pStreamConfig->num_streams == 1)
            {
                m_pChiUsecase = &Usecases1Target[UsecasePreviewLDCId];
                for (UINT targetInfoIdx = 0; targetInfoIdx < pStreamConfig->num_streams; targetInfoIdx++)
                {
                    ChiStream* pStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[targetInfoIdx]);
                    ChiTarget* pTargetInfo = m_pChiUsecase->ppChiTargets[targetInfoIdx];
                    pTargetInfo->pChiStream = pStream;
                }
                CHX_LOG("Single stream LDC Enabled through overrride");
            }
            else if (pStreamConfig->num_streams == 2)
            {
                m_pChiUsecase = &Usecases2Target[UsecasePreviewLDCTwoStreamId];
                for (UINT targetInfoIdx = 0; targetInfoIdx < pStreamConfig->num_streams; targetInfoIdx++)
                {
                    ChiStream* pStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[targetInfoIdx]);
                    ChiTarget* pTargetInfo = m_pChiUsecase->ppChiTargets[targetInfoIdx];
                    pTargetInfo->pChiStream = pStream;
                }
                CHX_LOG("Two stream LDC Enabled through overrride");
            }
        }
        else
        {
            if ( TRUE == pCameraInfo->m_cameraCaps.sensorCaps.isYUVCamera)
            {
                m_pChiUsecase = UsecaseSelector::DefaultMatchingUsecaseforYUVCamera(pStreamConfig);
            }
            else
            {
                m_pChiUsecase = UsecaseSelector::DefaultMatchingUsecase(pStreamConfig);
                 CHX_LOG("Ideal Raw Enabled through overrride1");
            }
        }
        UINT* cameraIds     = NULL;
        UINT  pipelineNum;
        if (NULL != m_pChiUsecase)
        {
            pipelineNum = m_pChiUsecase->numPipelines;
            cameraIds = static_cast<UINT*>(CHX_CALLOC(sizeof(UINT) * pipelineNum));
            if (NULL != cameraIds)
            {
                for (UINT i = 0; i < pipelineNum; i++)
                {
                    cameraIds[i] = m_cameraId;
                }
                result = SetPipelineToCameraMapping(pipelineNum, cameraIds);
                CHX_FREE(cameraIds);
                cameraIds = NULL;
            }
            else
            {
                CHX_LOG("No memory allocated for cameraIds");
                result = CDKResultENoMemory;
            }
        }
        else
        {
            CHX_LOG("No proper usecase selected");
            result = CDKResultEFailed;
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::PipelineCreated
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID AdvancedCameraUsecase::PipelineCreated(
    UINT sessionId,    ///< Id of session created
    UINT pipelineId)   ///< Pipeline of session create
{
    CHX_LOG_ERROR("MULTIFRAME AdvancedCameraUsecase pipelineId: %d", pipelineId);
    if ((UsecaseId::PreviewZSL == m_usecaseId) || (UsecaseId::YUVInBlobOut == m_usecaseId) ||
        (UsecaseId::VideoLiveShot == m_usecaseId))
    {
        for (UINT32 i = 0; i < m_enabledFeaturesCount; i++)
        {
            m_enabledFeatures[i]->PipelineCreated(sessionId, pipelineId);
        }
    }
    else if (UsecaseId::MultiCamera == m_usecaseId)
    {
        for (UINT32 i = 0; i < m_enabledFeaturesCount; i++)
        {
            m_enabledFeatures[i]->PipelineCreated(sessionId, pipelineId);
        }
    }
    else if (UsecaseId::RawJPEG == m_usecaseId)
    {
        m_pRawJPEG->PipelineCreated(sessionId, pipelineId);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::PostUsecaseCreation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AdvancedCameraUsecase::PostUsecaseCreation(
    camera3_stream_configuration_t* pStreamConfig)  ///< Stream configuration
{
    (void)pStreamConfig;
    CDKResult result = CDKResultSuccess;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::StreamIsInternal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL AdvancedCameraUsecase::StreamIsInternal(
    ChiStream* pStream)
{
    BOOL isRDI = FALSE;

    for (UINT32 i = 0; i < m_enabledFeaturesCount; i++)
    {
        if ((NULL != m_enabledFeatures[i]) &&  TRUE == m_enabledFeatures[i]->StreamIsInternal(pStream))
        {
            isRDI = TRUE;
            break;
        }
    }

    if (NULL != m_pRawJPEG)
    {
        isRDI = m_pRawJPEG->StreamIsInternal(pStream);
    }

    if ((TRUE == IsMultiCameraUsecase()) &&
        ((pStream == m_pRdiStream) || (pStream == m_pRdiStreamAux)) &&
        (TRUE == m_isRdiStreamImported))
    {
        CHX_LOG("RDI buffer Queue is from child class (usecasemc), override is internal to false here.");
        isRDI = FALSE;
    }

    return isRDI;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::ExecuteCaptureRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AdvancedCameraUsecase::ExecuteCaptureRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult result     = CDKResultSuccess;
    UINT      frameIndex = pRequest->frame_number % MaxOutstandingRequests;
    Feature*  pFeature   = m_pActiveFeature;

    CHX_LOG("AdvancedCameraUsecase::ExecuteCaptureRequest %u %u", pRequest->frame_number, frameIndex);

    // swapping JPEG thumbnail size params
    const ExtensionModule*      pExtModule = ExtensionModule::GetInstance();
    BOOL isGpuOverrideSetting = pExtModule->UseGPUDownscaleUsecase() || pExtModule->UseGPURotationUsecase();
    if( (TRUE == m_GpuNodePresence) && (TRUE == isGpuOverrideSetting))
    {
        if (NULL != pRequest->settings)
        {
            // ANDROID_JPEG_ORIENTATION
            INT32 JpegOrientation    = 0;
            CDKResult result         = m_vendorTagOps.pGetMetaData(
                                        const_cast<VOID*>
                                        (reinterpret_cast<const VOID*>(pRequest->settings)),
                                        ANDROID_JPEG_ORIENTATION,
                                        &JpegOrientation,
                                        sizeof(INT32));

            if (CDKResultSuccess == result)
            {
                INT32*                  pIntentJpegSize = NULL;

                if (JpegOrientation % 180)
                {
                    JPEGThumbnailSize thumbnailSizeGet, thumbnailSizeSet;
                    CDKResult result = m_vendorTagOps.pGetMetaData(
                                        const_cast<VOID*>
                                        (reinterpret_cast<const VOID*>(pRequest->settings)),
                                        ANDROID_JPEG_THUMBNAIL_SIZE,
                                        &thumbnailSizeGet,
                                        sizeof(JPEGThumbnailSize));

                    if (CDKResultSuccess == result)
                    {
                        thumbnailSizeSet.JpegThumbnailSize_0 = thumbnailSizeGet.JpegThumbnailSize_1;
                        thumbnailSizeSet.JpegThumbnailSize_1 = thumbnailSizeGet.JpegThumbnailSize_0;

                        CDKResult result = m_vendorTagOps.pSetMetaData(
                                            const_cast<VOID*>
                                            (reinterpret_cast<const VOID*>(pRequest->settings)),
                                            ANDROID_JPEG_THUMBNAIL_SIZE,
                                            &thumbnailSizeSet,
                                            sizeof(JPEGThumbnailSize));

                        if (CDKResultSuccess == result)
                        {
                            CDKResult result = m_vendorTagOps.pGetMetaData(
                                                const_cast<VOID*>
                                                (reinterpret_cast<const VOID*>(pRequest->settings)),
                                                ANDROID_JPEG_THUMBNAIL_SIZE,
                                                &thumbnailSizeGet,
                                                sizeof(JPEGThumbnailSize));
                        }
                    }
                }
            }
        }
    }
    // exchange JPEG thumbnail

    camera_metadata_entry_t entry      = { 0 };
    UINT32 modeValue                   = ANDROID_CONTROL_EFFECT_MODE_OFF;
    if (NULL != pRequest->settings)
    {
        UINT32    tag         = 0;
        UINT32    ir_mode     = 0;

        result = m_vendorTagOps.pQueryVendorTagLocation(
                 "org.codeaurora.qcamera3.ir_led",
                 "mode",
                 &tag);
        if (CDKResultSuccess == result)
        {
            result = m_vendorTagOps.pGetMetaData(
                     const_cast<VOID*>
                     (reinterpret_cast<const VOID*>(pRequest->settings)),
                     tag,
                     &ir_mode,
                     sizeof(ir_mode));


            if ((ir_mode == 1) && (m_firstFrameAfterIrLedOn == 0)) // IR ON
            {
                m_firstFrameAfterIrLedOn = pRequest->frame_number + 3;
                CHX_LOG_INFO("ir_mode ON @request = %d", pRequest->frame_number);
            }
            else if (ir_mode == 0) // IR OFF
            {
                CHX_LOG_INFO("ir_mode OFF @request = %d", pRequest->frame_number);
                if (m_firstFrameAfterIrLedOn !=0) {  // Check IR LED is ON before OFF

                    // change effect to Mono
                    ChxUtils::SetEffectMode(const_cast<camera_metadata_t*>(pRequest->settings), &modeValue);
                    m_firstFrameAfterIrLedOn = 0;
                }
            }
            else if (ir_mode == 2) // IR AUTO
            {
                /* No action*/
            }
        }
    }

    if (m_firstFrameAfterIrLedOn != 0)
    {
        if (pRequest->settings == NULL)
        {
            pRequest->settings = m_pLastSettingsWithIrLedOn;
        }
        else
        {
            // change effect to Mono
            modeValue = ANDROID_CONTROL_EFFECT_MODE_MONO;
            ChxUtils::SetEffectMode(const_cast<camera_metadata_t*>(pRequest->settings), &modeValue);

            if (NULL != m_pLastSettingsWithIrLedOn)
            {
                free_camera_metadata(m_pLastSettingsWithIrLedOn);
                m_pLastSettingsWithIrLedOn = NULL;
            }

            // Copying Current frame settings
            m_pLastSettingsWithIrLedOn  = allocate_copy_camera_metadata_checked(pRequest->settings,
                    get_camera_metadata_size(pRequest->settings));
        }

        if (m_firstFrameAfterIrLedOn == pRequest->frame_number)
        {
            UINT32    tag         = 0;
            UINT32    ir_ready     = TRUE;

            result = m_vendorTagOps.pQueryVendorTagLocation(
                     "org.codeaurora.qcamera3.ir_led",
                     "ready",
                     &tag);
            if (CDKResultSuccess == result)
            {
                CHX_LOG_INFO("ir_mode READY @request = %d", pRequest->frame_number);
                result = m_vendorTagOps.pSetMetaData(
                            const_cast<camera_metadata_t*>(pRequest->settings),
                            tag,
                            &ir_ready,
                            sizeof(ir_ready));
            }
        }
    }

    m_isMetadataSent[frameIndex]                = FALSE;
    m_isMetadataAvailable[frameIndex]           = FALSE;
    m_shutterTimestamp[frameIndex]              = 0;

    camera3_capture_result_t* pUsecaseResult    = this->GetCaptureResult(frameIndex);

    pUsecaseResult->result                      = NULL;
    pUsecaseResult->frame_number                = pRequest->frame_number;
    pUsecaseResult->num_output_buffers          = 0;

    result = UpdateFeatureModeIndex(const_cast<camera_metadata_t*>(pRequest->settings));
    if (TRUE ==
        ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(pRequest->settings), VideoHDR10Mode))
    {
        VOID* pData = NULL;
        StreamHDRMode  HDRMode = StreamHDRMode::HDRModeNone;
        ChxUtils::GetVendorTagValue(reinterpret_cast<const VOID*>(pRequest->settings),
            VideoHDR10Mode,
            reinterpret_cast<VOID**>(&pData));
        if (NULL != pData)
        {
            HDRMode = *(static_cast<StreamHDRMode*>(pData));
            if (StreamHDRMode::HDRModeHDR10 == HDRMode)
            {
                m_tuningFeature2Value = static_cast<UINT32>(ChiModeFeature2SubModeType::HDR10);
            }
            else if (StreamHDRMode::HDRModeHLG == HDRMode)
            {
                m_tuningFeature2Value = static_cast<UINT32>(ChiModeFeature2SubModeType::HLG);
            }
            else
            {
                m_tuningFeature2Value = 0;
            }
        }
    }

    if (TRUE == hasSnapshotStreamRequest(pRequest))
    {
        WaitForDeferThread();
    }

    if (TRUE == AdvacnedFeatureEnabled())
    {
        for (UINT32 i = 0; i < pRequest->num_output_buffers; i++)
        {
            if (m_pSnapshotStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
            {
                pFeature = SelectFeatureToExecuteCaptureRequest(pRequest);
            }
        }
        if (NULL != pFeature)
        {
            m_shutterTimestamp[frameIndex] = 0;
            result = pFeature->ExecuteProcessRequest(pRequest);
        }
    }
    else if (NULL != m_pRawJPEG)
    {
        result = m_pRawJPEG->ExecuteProcessRequest(pRequest);
    }
    else
    {
        result = CameraUsecaseBase::ExecuteCaptureRequest(pRequest);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::ParseResultMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult AdvancedCameraUsecase::ParseResultMetadata(
    CHICAPTURERESULT*           pResult)
{
    // check preview result meta, like asd output, aec/awb result.
    UINT32    tag         = 0;
    CDKResult result      = CDKResultSuccess;
    INT32     isLLSNeeded = 0;

    if (NULL == pResult->pResultMetadata)
    {
        CHX_LOG_ERROR("pResultMetadata is NULL");
        result = CDKResultEInvalidArg;
    }

    if (CDKResultSuccess == result)
    {
        result = m_vendorTagOps.pQueryVendorTagLocation(
                 "org.quic.camera2.asdresults",
                 "ASDResults",
                 &tag);

        if (CDKResultSuccess == result)
        {
            result = m_vendorTagOps.pGetMetaData(
                     const_cast<VOID*>(pResult->pResultMetadata),
                     tag,
                     &m_asdResult,
                     sizeof(ASDOutput));
        }

        result = m_vendorTagOps.pQueryVendorTagLocation(
            "com.qti.stats_control",
            "is_lls_needed",
            &tag);

        if (CDKResultSuccess == result)
        {
            result = m_vendorTagOps.pGetMetaData(
                reinterpret_cast<camera_metadata_t*>(const_cast<VOID*>(pResult->pResultMetadata)),
                tag,
                &isLLSNeeded,
                sizeof(INT32));

            CHX_LOG("is_lls_needed:%d, vendor tag:%x", isLLSNeeded, tag);
            m_isLLSNeeded = (isLLSNeeded == 1 ) ? TRUE : FALSE;
        }

        m_isFlashRequired =
            IsFlashRequired(reinterpret_cast<camera_metadata_t*>(const_cast<VOID*>(pResult->pResultMetadata)));

    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::ProcessResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID AdvancedCameraUsecase::ProcessResult(
    CHICAPTURERESULT*           pResult,
    VOID*                       pPrivateCallbackData)
{
    if (TRUE == AdvacnedFeatureEnabled())
    {
        SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
        UINT32              sessionId           = pSessionPrivateData->sessionId;

        if ((NULL != pResult->pResultMetadata) && (sessionId == m_realtimeSessionId))
        {
            ParseResultMetadata(pResult);
        }

        m_pResultMutex->Lock();

        Feature* pFeature = FindFeatureToProcessResult(pResult, pPrivateCallbackData);
        if (NULL != pFeature)
        {
            pFeature->ProcessResult(pResult, pPrivateCallbackData);
        }
        else
        {
            CHX_LOG_ERROR("pFeature is NULL.");
        }

        m_pResultMutex->Unlock();
    }
    else if (NULL != m_pRawJPEG)
    {
        m_pRawJPEG->ProcessResult(pResult, pPrivateCallbackData);
    }
    else
    {
        m_pResultMutex->Lock();
        CameraUsecaseBase::SessionCbCaptureResult(pResult, pPrivateCallbackData);
        m_pResultMutex->Unlock();
    }

    if (2 <= ExtensionModule::GetInstance()->EnableDumpDebugData())
    {
        // Process debug-data
        ProcessDebugData(pResult, pPrivateCallbackData, pResult->frameworkFrameNum);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AdvancedCameraUsecase::ProcessMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID AdvancedCameraUsecase::ProcessMessage(
    const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{
    if (TRUE == AdvacnedFeatureEnabled())
    {
        m_pResultMutex->Lock();

        if (ChiMessageTypeSof == pMessageDescriptor->messageType)
        {
            // SOF notifications are not sent to the HAL3 application
            CHX_LOG("ZSL Chi Notify SOF frameNum %u framework frameNum %u, timestamp %" PRIu64,
                pMessageDescriptor->message.sofMessage.sofId,
                pMessageDescriptor->message.sofMessage.frameworkFrameNum,
                pMessageDescriptor->message.sofMessage.timestamp);
        }
        else if (ChiMessageTypeShutter == pMessageDescriptor->messageType)
        {
            SessionPrivateData* pCbData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
            BOOL                dropCb  = TRUE;
            Feature* pFeature = GetFeatureFromRequestMapping(pCbData->sessionId,
                                                             pMessageDescriptor->message.shutterMessage.frameworkFrameNum);
            if (NULL != pFeature && FeatureType::HDR == pFeature->GetFeatureType())
            {
                CHX_LOG("FeatureHDR to handle shutter message it self.");
                pFeature->ProcessMessage(pMessageDescriptor, pPrivateCallbackData);
            }
            else
            {
                if ((m_pipelineToSession[AdvancedPipelineType::ZSLPreviewRawType] == static_cast<INT32>(pCbData->sessionId)) ||
                    (m_pipelineToSession[AdvancedPipelineType::ZSLPreviewRawYUVType] ==
                        static_cast<INT32>(pCbData->sessionId)))
                {
                    UINT32 rtResultId = pMessageDescriptor->message.shutterMessage.frameworkFrameNum;
                    UINT32 reqIdIndex = (rtResultId % MaxOutstandingRequests);

                    PipelineData*  pPipelineData = const_cast<PipelineData*>(GetPipelineData(pCbData->sessionId, 0));
                    UINT32 resultFrameNum = pPipelineData->seqIdToFrameNum[reqIdIndex];

                    CHX_LOG("Shutter notify Realtime ReqId to AppFrameNum: %d <--> %d", rtResultId, resultFrameNum);

                    UINT32 frameNumIndex = (resultFrameNum % MaxOutstandingRequests);
                    if (0 == m_shutterTimestamp[frameNumIndex])
                    {
                        m_shutterTimestamp[frameNumIndex] = pMessageDescriptor->message.shutterMessage.timestamp;

                        ChiMessageDescriptor* pOverrideMessageDescriptor = const_cast<ChiMessageDescriptor*>(pMessageDescriptor);
                        pOverrideMessageDescriptor->message.shutterMessage.frameworkFrameNum = resultFrameNum;

                        dropCb = FALSE;
                    }
                    else
                    {
                        CHX_LOG_WARN("Already have same shutter for appFrameNum:%d", resultFrameNum);
                    }
                }
                else if ((m_pipelineToSession[AdvancedPipelineType::ZSLYuv2YuvType] == static_cast<INT32>(pCbData->sessionId)) ||
                    (m_pipelineToSession[AdvancedPipelineType::InternalZSLYuv2JpegType] ==
                        static_cast<INT32>(pCbData->sessionId)))
                {
                    dropCb = FALSE;
                }

                if (FALSE == dropCb)
                {
                    ReturnFrameworkMessage((camera3_notify_msg_t*)pMessageDescriptor, GetCameraId());
                }
            }
        }
        else if (ChiMessageTypeError == pMessageDescriptor->messageType)
        {
            ProcessErrorMessage(pMessageDescriptor);
        }
        else
        {
            ReturnFrameworkMessage((camera3_notify_msg_t*)pMessageDescriptor, GetCameraId());
        }

        m_pResultMutex->Unlock();
    }
    else if (NULL != m_pRawJPEG)
    {
        m_pRawJPEG->ProcessMessage(pMessageDescriptor, pPrivateCallbackData);
    }
    else
    {
        CameraUsecaseBase::SessionCbNotifyMessage(pMessageDescriptor, pPrivateCallbackData);
    }
}

// START of OEM to change section
VOID AdvancedCameraUsecase::SelectFeatures(camera3_stream_configuration_t* pStreamConfig)
{
    // OEM to change
    // this function to decide which features to run per the current pStreamConfig and static settings
    (VOID)pStreamConfig;
    INT32  index                  = 0;
    UINT32 enabledAdvanceFeatures = 0;
    ChiSensorModeInfo* pAllModes          = NULL;
    UINT32             modeCount          = 0;

    enabledAdvanceFeatures = ExtensionModule::GetInstance()->GetAdvanceFeatureMask();
    CHX_LOG("SelectFeatures(), enabled feature mask:%x", enabledAdvanceFeatures);
    ExtensionModule::GetInstance()->GetAllSensorModes(m_cameraId, &modeCount, &pAllModes);
    UINT32 enablevideoHDR  = ExtensionModule::GetInstance()->GetVideoHDRMode();
    BOOL   isSHDRSupported = FALSE;
    /* By default ZSL is supported */
    for (UINT32 i = 0; i < modeCount; i++)
    {
        if (pAllModes[i].sensorModeCaps.u.SHDR == 1)
        {
            isSHDRSupported = TRUE;
        }
    }

    CHX_LOG("num features selected:%d and camid :%d and modecount : %d and enablevideohdr:%d: and isdhdrsupported: %d",
        index, m_cameraId, modeCount, enablevideoHDR, isSHDRSupported);

    if ((AdvanceFeatureSHDR == (enabledAdvanceFeatures & AdvanceFeatureSHDR)) ||
        (enablevideoHDR && isSHDRSupported))
    {
        CHX_LOG("Enabled SHDR feature");
        enabledAdvanceFeatures = 0x10;
        m_enabledFeatures[index] = FeatureSHDR::Create(this);
        index++;
    }

    if (UsecaseId::YUVInBlobOut == m_usecaseId)
    {
        m_enabledFeatures[index] = FeatureYuvCB::Create(this);
        index++;
    }
    else
    {
        if (AdvanceFeatureZSL == (enabledAdvanceFeatures & AdvanceFeatureZSL))
        {
            if (FALSE == IsMultiCameraUsecase())
            {
                // for the ZSL tuning, only enable FeatureZSL
                if (UsecaseZSLTuningId == static_cast<UsecaseId3Target>(ExtensionModule::GetInstance()->OverrideUseCase()))
                {
                    m_enabledFeatures[index] = FeatureZSL::Create(this);
                    index++;
                }
                else
                {
                    m_enabledFeatures[index] = FeatureZSL::Create(this);
                    index++;
                }
            }
        }
    }

    if (AdvanceFeatureMFNR == (enabledAdvanceFeatures & AdvanceFeatureMFNR))
    {
        m_enabledFeatures[index] = FeatureMFNR::Create(this);
        index++;
    }

    if (AdvanceFeatureHDR == (enabledAdvanceFeatures & AdvanceFeatureHDR))
    {
        m_enabledFeatures[index] = FeatureHDR::Create(this);
        index++;
    }

    if (AdvanceFeatureSWMF == (enabledAdvanceFeatures & AdvanceFeatureSWMF))
    {
        m_enabledFeatures[index] = FeatureMultiframe::Create(this);
        index++;
    }

    CHX_LOG("num features selected:%d", index);

    m_enabledFeaturesCount = index;
    m_pActiveFeature = m_enabledFeatures[0];
}

Feature* AdvancedCameraUsecase::SelectFeatureToExecuteCaptureRequest(camera3_capture_request_t* pRequest)
{
    // OEM to change
    // @todo add logic to select the feature to run the request
    Feature* pFeature = NULL;
    if (m_enabledFeaturesCount == 0)
        return NULL;

#if DYNAMIC_FEATURE_SWITCH
    static UINT i = 1;
    // enable feature count = enable features + normal capture
    UINT32 enableFeatureCnt = m_enabledFeaturesCount + 1;

    if (FALSE == IsMultiCameraUsecase())
    {
        enableFeatureCnt = m_enabledFeaturesCount;
    }

    if (m_enabledFeatures[i % enableFeatureCnt] != NULL)
    {
        pFeature = m_enabledFeatures[i % enableFeatureCnt];
    }

    i++;
#else
    UINT32 disableASDProcessing = 0;
    disableASDProcessing = ExtensionModule::GetInstance()->DisableASDProcessing();

    // select feature by ASD result, OEM can change
    if (0 == disableASDProcessing)
    {
        if ((TRUE == m_asdResult.detected[ASDPortrait]) &&
            (m_asdResult.severity[ASDPortrait] > ThresholdOfASDSeverity))
        {
            pFeature = FindFeaturePerType(FeatureType::MFNR);
        }
        else if ((TRUE == m_asdResult.detected[ASDBacklight]) &&
                 (m_asdResult.severity[ASDBacklight] > ThresholdOfASDSeverity))
        {
            pFeature = FindFeaturePerType(FeatureType::HDR);
        }
        else if ((TRUE == m_asdResult.detected[ASDLandscape]) &&
                 (m_asdResult.severity[ASDLandscape] > ThresholdOfASDSeverity))
        {
            pFeature = FindFeaturePerType(FeatureType::ZSL);
        }
        else if ((TRUE == m_asdResult.detected[ASDSnow]) &&
                 (m_asdResult.severity[ASDSnow] > ThresholdOfASDSeverity))
        {
            pFeature = FindFeaturePerType(FeatureType::SWMF);
        }
        else if ((TRUE ==  m_asdResult.detected[ASDHDR]) &&
                 (m_asdResult.severity[ASDHDR] > ThresholdOfASDSeverity))
        {
            pFeature = FindFeaturePerType(FeatureType::HDR);
        }
    }

    if ((NULL == pFeature) && (NULL != m_enabledFeatures[0]))
    {
            pFeature = m_enabledFeatures[0];
            CHX_LOG("ASD detect result is not available, select default feature");
    }
#endif

    if (NULL != pFeature)
    {
        CHX_LOG("Select Feature %p type %d for request:%d", pFeature, pFeature->GetFeatureType(),
            pRequest->frame_number);
    }
    else
    {
        CHX_LOG_ERROR("no feature selected for request=%d", pRequest->frame_number);
    }

    return pFeature;
}
// END of OEM to change section

VOID AdvancedCameraUsecase::SetupSharedPipeline(
    ChiPipelineTargetCreateDescriptor* pPipeline,
    AdvancedPipelineType pipelineType)
{
    CHX_LOG("SetupSharedPipeline for %p type %d", pPipeline, pipelineType);

    /// @todo: For ZSLSnapshotYUVType/InternalZSLYuv2JpegType,
    /// in each feature, it will set input/output stream for required pipeline,
    /// but better to move it to here, as they're shared pipelines.

    switch (pipelineType)
    {
    case AdvancedPipelineType::ZSLPreviewRawType:
    {
        // Setup the raw target with the RDI stream. Raw has only one buffer format, it will either match here, or
        // not matter in the output format because none of its will match (another assumption)
        if (ChiFormatRawMIPI == pPipeline->sinkTarget.pTargetPortDesc[0].pTarget->pBufferFormats[0])
        {
            pPipeline->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStream;
            pPipeline->sinkTarget.pTargetPortDesc[1].pTarget->pChiStream = m_pPreviewStream;
        }
        else
        {
            pPipeline->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pPreviewStream;
            pPipeline->sinkTarget.pTargetPortDesc[1].pTarget->pChiStream = m_pRdiStream;
        }
    }
        break;
    case AdvancedPipelineType::ZSLPreviewRawYUVType:
    {
        for (UINT32 stream = 0; stream < 3; stream++)
        {
            if (ChiFormatRawMIPI == pPipeline->sinkTarget.pTargetPortDesc[stream].pTarget->pBufferFormats[0])
            {
                pPipeline->sinkTarget.pTargetPortDesc[stream].pTarget->pChiStream = m_pRdiStream;
            }
            // if IPE video port
            else if ((65538 == pPipeline->sinkTarget.pTargetPortDesc[stream].nodeport.nodeId) &&
                ((9 == pPipeline->sinkTarget.pTargetPortDesc[stream].nodeport.nodePortId)))
            {
                pPipeline->sinkTarget.pTargetPortDesc[stream].pTarget->pChiStream = m_pVideoStream;
            }
            // if IPE preview port
            else if ((65538 == pPipeline->sinkTarget.pTargetPortDesc[stream].nodeport.nodeId) &&
                ((8 == pPipeline->sinkTarget.pTargetPortDesc[stream].nodeport.nodePortId)))
            {
                pPipeline->sinkTarget.pTargetPortDesc[stream].pTarget->pChiStream = m_pPreviewStream;
            }
        }
    }
        break;

    default:
        break;
    }
}

ChiStream*  AdvancedCameraUsecase::GetSharedStream(
    SharedStreamType streamType)
{
    ChiStream* pStream = NULL;
    switch (streamType)
    {
    case AdvancedCameraUsecase::SnapshotStream:
    {
        pStream = m_pSnapshotStream;
        break;
    }
    case AdvancedCameraUsecase::PreviewStream:
    {
        pStream = m_pPreviewStream;
        break;
    }
    case AdvancedCameraUsecase::RdiStream:
    {
        pStream = m_pRdiStream;
        break;
    }
    case AdvancedCameraUsecase::RdiStreamAux:
    {
        pStream = m_pRdiStreamAux;
        break;
    }
    case AdvancedCameraUsecase::Bayer2YuvStream:
    {
        pStream = m_pBayer2YuvStream;
        break;
    }
    case AdvancedCameraUsecase::JPEGInputStream:
    {
        pStream = m_pJPEGInputStream;
        break;
    }
    case AdvancedCameraUsecase::YuvCBStream:
    {
        pStream = m_pYuvCBStream;
        break;
    }
    case AdvancedCameraUsecase::VideoStream:
    {
        pStream = m_pVideoStream;
        break;
    }
    case AdvancedCameraUsecase::YuvInputStream:
    {
        pStream = m_pYuvInStream;
        break;
    }
    default:
        CHX_LOG_ERROR("Unhandled stream type %d", streamType);
        break;
    }

    return pStream;
}

VOID AdvancedCameraUsecase::BuildUsecase(
    LogicalCameraInfo*              pCameraInfo,
    camera3_stream_configuration_t* pStreamConfig)
{
    ChiStream*  pTempStream;
    if (UsecaseId::VideoLiveShot == m_usecaseId)
    {
        for (UINT32 stream = 0; stream < pStreamConfig->num_streams; stream++)
        {
            pTempStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[stream]);

            if ((ChiStreamTypeOutput == pTempStream->streamType) &&
                (ChiStreamFormatBlob == pTempStream->format))
            {
                m_pSnapshotStream = pTempStream;
            }
            else if ((ChiStreamTypeOutput   == pTempStream->streamType) &&
                     (GRALLOC_USAGE_HW_VIDEO_ENCODER & pTempStream->grallocUsage))
            {
                m_pVideoStream = pTempStream;
            }
            else
            {
                m_pPreviewStream = pTempStream;
            }
        }
    }
    else if (UsecaseId::YUVInBlobOut == m_usecaseId)
    {
        UINT32  inputStreamWidth  = 0;
        UINT32  inputStreamHeight = 0;
        for (UINT32 stream = 0; stream < pStreamConfig->num_streams; stream++)
        {
            pTempStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[stream]);
            if ((ChiStreamTypeInput == pTempStream->streamType) &&
                (ChiStreamFormatYCbCr420_888 == pTempStream->format))
            {
                m_pYuvInStream      = pTempStream;
                inputStreamWidth    = m_pYuvInStream->width;
                inputStreamHeight   = m_pYuvInStream->height;
                break;
            }
        }
        for (UINT32 stream = 0; stream < pStreamConfig->num_streams; stream++)
        {
            pTempStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[stream]);
            if ((ChiStreamTypeOutput         == pTempStream->streamType) &&
                (ChiStreamFormatYCbCr420_888 == pTempStream->format) &&
                (inputStreamWidth            == pTempStream->width) &&
                (inputStreamHeight           == pTempStream->height))
            {
                m_pYuvCBStream = pTempStream;
            }
            else if ((ChiStreamTypeOutput == pTempStream->streamType) &&
                     (ChiStreamFormatBlob == pTempStream->format))
            {
                m_pSnapshotStream = pTempStream;
            }
            else if ((ChiStreamTypeInput          == pTempStream->streamType) &&
                     (ChiStreamFormatYCbCr420_888 == pTempStream->format))
            {
                m_pYuvInStream = pTempStream;
            }
            else
            {
                if ((ChiStreamTypeOutput == pTempStream->streamType) &&
                        ((ChiStreamFormatImplDefined == pTempStream->format) ||
                         (ChiStreamFormatYCbCr420_888 == pTempStream->format)))
                {
                    m_pPreviewStream = pTempStream;
                }
                else
                {
                    CHX_LOG_ERROR("Unknown stream type for YUVInBlobOut usecase");
                }
            }
        }
    }
    else
    {
        if (pStreamConfig->num_streams == 1)
        {
            m_pPreviewStream  = reinterpret_cast<ChiStream*>(pStreamConfig->streams[0]);
        }
        else
        {
        m_pPreviewStream  = reinterpret_cast<ChiStream*>(pStreamConfig->streams[0]);
        m_pSnapshotStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[1]);

        // Preview output is up to the SOC, camera directly feeds display, so we match the preview to the stream with format
        if (ChiStreamFormatImplDefined == m_pSnapshotStream->format)
        {
            m_pPreviewStream  = reinterpret_cast<ChiStream*>(pStreamConfig->streams[1]);
            m_pSnapshotStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[0]);
        }
        }
    }

    CHX_LOG("BuildUsecase, E. preview stream:%p, wrapper:%p",
        m_pPreviewStream,  m_pPreviewStream->pPrivateInfo);

    m_pChiUsecase                   = static_cast<ChiUsecase*>(CHX_CALLOC(sizeof(ChiUsecase)));
    if (NULL == m_pChiUsecase)
    {
        CHX_LOG("No memory allocated for usecase");
        return;
    }
    if (UsecaseZSLTuningId == static_cast<UsecaseId3Target>(ExtensionModule::GetInstance()->OverrideUseCase()))
        ChxUtils::Memcpy(m_pChiUsecase, pZslTuningUsecase, sizeof(ChiUsecase));
    else
        ChxUtils::Memcpy(m_pChiUsecase, pAdvancedUsecase, sizeof(ChiUsecase));


    // query the unique pipeline ids and insert it to the pipelines array
    AdvancedPipelineType    pipelines[AdvancedPipelineType::PipelineCount];
    AdvancedPipelineType    featurePipelines[AdvancedPipelineType::PipelineCount];
    UINT32                  pipelineCount = 0;
    UINT32                  featurePipelineCount = 0;
    for (UINT32 i = 0; i < m_enabledFeaturesCount; i++)
    {
        featurePipelineCount = m_enabledFeatures[i]->GetRequiredPipelines(
            &featurePipelines[0], AdvancedPipelineType::PipelineCount);

        // insert to the unique pipeline array
        for (UINT32 n = 0; n < featurePipelineCount; n++)
        {
            UINT32 m = 0;
            for (m = 0; m < pipelineCount; m++)
            {
                if (pipelines[m] == featurePipelines[n])
                    break;
            }
            if (m == pipelineCount)
            {
                // not found, add it to the end
                pipelines[m] = featurePipelines[n];
                pipelineCount++;
            }
        }
    }

    m_pChiUsecase->pPipelineTargetCreateDesc = static_cast<ChiPipelineTargetCreateDescriptor*>
            (CHX_CALLOC(pipelineCount * sizeof(ChiPipelineTargetCreateDescriptor)));
    if (NULL == m_pChiUsecase->pPipelineTargetCreateDesc)
    {
        CHX_LOG("No memory allocated for pipeline descriptor");
        return;
    }
    m_pipelineCount             = pipelineCount;
    m_pChiUsecase->numPipelines = pipelineCount;

    UINT    cameraIds[AdvancedPipelineType::PipelineCount]          = {0};
    UINT32  pipelineToSession[AdvancedPipelineType::PipelineCount]  = {0};
    for (UINT32 i = 0; i < pipelineCount; i++)
    {
        m_pChiUsecase->pPipelineTargetCreateDesc[i] = GetPipelineDescriptorFromUsecase(pAdvancedUsecase, pipelines[i]);
        m_pipelineStatus[i].pipelineId              = pipelines[i];
        m_pipelineStatus[i].used                    = TRUE;

        cameraIds[i]            = m_pipelineToCameraId[pipelines[i]];
        pipelineToSession[i]    = m_pipelineToSession[pipelines[i]];

        if (FALSE == IsMultiCameraUsecase())
        {
            SetupSharedPipeline(&m_pChiUsecase->pPipelineTargetCreateDesc[i], pipelines[i]);
        }
        else
        {
            CHX_LOG("Don't create realtime pipeline in multi camera usecase.");
        }

        if ((AdvancedPipelineType::ZSLPreviewRawType    == pipelines[i]) ||
            (AdvancedPipelineType::ZSLPreviewRawYUVType == pipelines[i]))
        {
            m_realtimeSessionId = m_pipelineToSession[pipelines[i]];
            CHX_LOG("realtime session id:%d", m_realtimeSessionId);
        }
    }

    SetPipelineToSessionMapping(m_uniqueSessionId, pipelineCount, pipelineToSession);
    SetPipelineToCameraMapping(pipelineCount, cameraIds);

    for (UINT32 i = 0; i < m_enabledFeaturesCount; i++)
    {
        CHX_LOG("feature index:%d",i);
        m_enabledFeatures[i]->OverrideUsecase(pCameraInfo, pStreamConfig);
    }
}

Feature* AdvancedCameraUsecase::FindFeatureToProcessResult(
    CHICAPTURERESULT*           pResult,
    VOID*                       pPrivateCallbackData)
{
    SessionPrivateData* pCbData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    Feature* pFeature = NULL;

    if (NULL != pResult->pPrivData && pResult->pPrivData->featureType != FeatureType::UNINIT)
    {
        pFeature = FindFeaturePerType(static_cast<FeatureType>(pResult->pPrivData->featureType));
    }
    else
    {
        pFeature = GetFeatureFromRequestMapping(pCbData->sessionId, pResult->frameworkFrameNum);
    }

    if (NULL != pFeature)
    {
        CHX_LOG("ProcessResult pPrivData %p, sessionId %d frameNum %d feature type: %d",
            pResult->pPrivData, pCbData->sessionId, pResult->frameworkFrameNum, pFeature->GetFeatureType());
    }

    return pFeature;
}

Feature*  AdvancedCameraUsecase::FindFeaturePerType(FeatureType type)
{
    Feature* pFeature = NULL;
    for (UINT32 i = 0; i < m_enabledFeaturesCount; i++)
    {
        pFeature = m_enabledFeatures[i];
        if (pFeature->GetFeatureType() == type)
            break;
        else
            pFeature = NULL;
    }

    return pFeature;
}

ChiPipelineTargetCreateDescriptor AdvancedCameraUsecase::GetPipelineDescriptorFromUsecase(
    ChiUsecase* pChiUsecase,
    AdvancedPipelineType id)
{
    if (UsecaseZSLTuningId == static_cast<UsecaseId3Target>(ExtensionModule::GetInstance()->OverrideUseCase()))
        return pChiUsecase->pPipelineTargetCreateDesc[AdvancedPipelineIdTuningMapping[id]];
    else
        return pChiUsecase->pPipelineTargetCreateDesc[AdvancedPipelineIdMapping[id]];
}

ChiPipelineTargetCreateDescriptor* AdvancedCameraUsecase::GetPipelineDescriptor(
    AdvancedPipelineType type)
{
    ChiPipelineTargetCreateDescriptor* pDesc = NULL;
    INT32 index = GetPipelineIdByAdvancedPipelineType(type);
    if (-1 != index)
        pDesc = &m_pChiUsecase->pPipelineTargetCreateDesc[index];

    return pDesc;
}

INT32 AdvancedCameraUsecase::GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType type)
{
    INT32 index = -1;
    for (UINT32 i = 0; i < m_pipelineCount; i++)
    {
        if (m_pipelineStatus[i].pipelineId == type)
        {
            index = i;
            break;
        }
    }

    return index;
}

INT32  AdvancedCameraUsecase::GetAdvancedPipelineTypeByPipelineId(INT32 pipelineId)
{
    return m_pipelineStatus[pipelineId].pipelineId;
}

UINT64 AdvancedCameraUsecase::GetRequestShutterTimestamp(UINT64 frameNumber)
{
    UINT32 resultFrameIndex = frameNumber % MaxOutstandingRequests;
    return m_shutterTimestamp[resultFrameIndex];
}

VOID     AdvancedCameraUsecase::SetRequestToFeatureMapping(UINT sessionId, UINT64 requestId, Feature* pFeature)
{
    const UINT size = sizeof(m_sessionRequestIdFeatureMapping) / sizeof(SessionRequestIdFeatureMapping);
    UINT       index = m_sessionRequestIdFeatureMappingCount % size;

    m_sessionRequestIdFeatureMapping[index].sessionId = sessionId;
    m_sessionRequestIdFeatureMapping[index].requestId = requestId;
    m_sessionRequestIdFeatureMapping[index].pFeature  = pFeature;

    CHX_LOG("Current index %d RequestCount %d sessionId %d requestId %" PRIu64 " pFeature %p type %d",
                  index, m_sessionRequestIdFeatureMappingCount,
                  sessionId, requestId, pFeature, pFeature->GetFeatureType());

    m_sessionRequestIdFeatureMappingCount++;
}

Feature* AdvancedCameraUsecase::GetFeatureFromRequestMapping(UINT sessionId, UINT64 requestId)
{
    const UINT size = sizeof(m_sessionRequestIdFeatureMapping) / sizeof(SessionRequestIdFeatureMapping);
    Feature*   pFeature = NULL;
    UINT       index = 0;
    for (index = 0; index < size; index++)
    {
        if (m_sessionRequestIdFeatureMapping[index].sessionId == sessionId &&
            m_sessionRequestIdFeatureMapping[index].requestId == requestId)
        {
            pFeature = m_sessionRequestIdFeatureMapping[index].pFeature;
            break;
        }
    }
    if (NULL != pFeature)
    {
        CHX_LOG("Look for feature with current index %d sessionId %d requestId %" PRIu64 " pFeature %p type %d",
                  index, sessionId, requestId, pFeature, pFeature->GetFeatureType());
    }
    return pFeature;
}

/*
    * return a unique session id for the given pipeline group
    * if any id in the pipelineGroup already has the session id, then
    * all of the pipeline will share this session id
    *
    * return a new session id if none of the pipeline id was associated with any session
    * return the existing if one of the pipeline id is already associated with a session
    */
UINT32 AdvancedCameraUsecase::GetUniqueSessionId(
    AdvancedPipelineType*   pipelineGroup,
    UINT32                  pipelineCount)
{
    INT32 sessionId = InvalidSessionId;
    UINT32 uniqueId = 0;
    for (UINT32 i = 0; i < pipelineCount; i++)
    {
        if (m_pipelineToSession[pipelineGroup[i]] != -1)
            sessionId = m_pipelineToSession[pipelineGroup[i]];
    }

    if (sessionId != InvalidSessionId)
    {
        uniqueId = sessionId;
    }
    else
    {
        uniqueId = m_uniqueSessionId++;
    }

    for (UINT32 i = 0; i < pipelineCount; i++)
    {
        m_pipelineToSession[pipelineGroup[i]] = uniqueId;
    }

    CHX_LOG("return uniqueSessionId: %d", uniqueId);
    return uniqueId;
}

/*
 * Set the cameraId for each pipeline
 */
VOID  AdvancedCameraUsecase::SetPipelineCameraId(
    AdvancedPipelineType*   pipelineGroup,
    UINT32*                 cameraId,
    UINT32                  pipelineCount)
{
    for (UINT32 i = 0; i < pipelineCount; i++)
    {
        m_pipelineToCameraId[pipelineGroup[i]] = cameraId[i];
    }

    return;
}

UINT AdvancedCameraUsecase::GetPhysicalCameraId(CameraRoleType role)
{
    UINT cameraId = 0;
    if (NULL != m_pLogicalCameraInfo)
    {
        switch (role)
        {
        case CameraRoleTypeWide:
            if (m_pLogicalCameraInfo->numPhysicalCameras > 0)
            {
                cameraId = m_pLogicalCameraInfo->ppDeviceInfo[0]->cameraId;
            }
            break;
        case CameraRoleTypeTele:
            if (m_pLogicalCameraInfo->numPhysicalCameras > 1)
            {
                cameraId = m_pLogicalCameraInfo->ppDeviceInfo[1]->cameraId;
            }
        default:
            break;
        }
    }
    else
    {
        CHX_LOG_ERROR("NULL m_pLogicalCameraInfo");
    }

    CHX_LOG("Camera Role %d camera Id %d", role, cameraId);
    return cameraId;
}

VOID    AdvancedCameraUsecase::ConfigRdiStream(ChiSensorModeInfo* pSensorInfo)
{
    // configure the shared Rdi stream
    if ((pSensorInfo->frameDimension.width > m_pRdiStream->width) ||
        (pSensorInfo->frameDimension.height > m_pRdiStream->height))
    {
        m_pRdiStream->format        = ChiStreamFormatRaw10;
        if (UsecaseZSLTuningId == static_cast<UsecaseId3Target>(ExtensionModule::GetInstance()->OverrideUseCase()))
            m_pRdiStream->format    = ChiStreamFormatRaw16;;

        // Create the RDI stream output based on the input buffer requirements to generate the snapshot stream buffer
        m_pRdiStream->width         = pSensorInfo->frameDimension.width;
        m_pRdiStream->height        = pSensorInfo->frameDimension.height;
        m_pRdiStream->maxNumBuffers = 0;
        m_pRdiStream->rotation      = StreamRotationCCW90;
        m_pRdiStream->streamType    = ChiStreamTypeOutput;
        m_pRdiStream->grallocUsage  = 0;

        CHX_LOG("ConfigRdiStream, wxh:%dx%d", m_pRdiStream->width, m_pRdiStream->height);
    }
    else
    {
        CHX_LOG("RdiStream already configured, wxh:%dx%d", m_pRdiStream->width, m_pRdiStream->height);
    }
}
