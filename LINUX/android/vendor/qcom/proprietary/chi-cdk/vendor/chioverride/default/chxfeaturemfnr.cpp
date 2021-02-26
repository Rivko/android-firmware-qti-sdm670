////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeaturemfnr.cpp
/// @brief CHX mfnr feature class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cmath>

#include "chi.h"
#include "chiispstatsdefs.h"
#include "chistatsproperty.h"
#include "chxadvancedcamerausecase.h"
#include "chxfeaturemfnr.h"
#include "chxincs.h"
#include "chxusecase.h"
#include "chxutils.h"


extern CHICONTEXTOPS g_chiContextOps;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FeatureMFNR* FeatureMFNR::Create(
    AdvancedCameraUsecase* pUsecase)
{
    FeatureMFNR* pFeature = CHX_NEW FeatureMFNR;

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
/// FeatureMFNR::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::Initialize(
    AdvancedCameraUsecase* pUsecase)
{
    CDKResult result = CDKResultSuccess;

    m_pUsecase              = pUsecase;
    m_pMfnrResultMutex      = Mutex::Create();
    m_pMfnrResultAvailable  = Condition::Create();

    m_pApplicationInputMeta = allocate_camera_metadata(ReplacedMetadataEntryCapacity, ReplacedMetadataDataCapacity);
    m_pInterStageMetadata   = allocate_camera_metadata(ReplacedMetadataEntryCapacity, ReplacedMetadataDataCapacity);

    m_pRDIResultMutex       = Mutex::Create();
    m_pRDIResultAvailable   = Condition::Create();

    m_mfnrTotalNumFrames    = MfnrDefaultInputFrames;

    // Debug-Data for offline processing
    m_debugDataOffline.pData        = NULL;
    m_debugDataOffline.size         = 0;

    /// As now preview keeps going during snapshot,
    /// if the MFNR snpashot reprocess is long, the input RDI buffer could be overwritten by up-coming preivew request.
    /// there's no such issue in normal case, but we can easily meet this problem when frame dump is eanbled.
    /// add a flag here, if need to dump frame, probably need to block upcoming preview.
    m_blockPreviewForSnapshot = FALSE;

    if (TRUE == m_blockPreviewForSnapshot)
    {
        m_pSnapshotResultMutex     = Mutex::Create();
        m_pSnapshotResultAvailable = Condition::Create();
    }

    InitializeInternalStreams(CameraType::Wide);
    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        InitializeInternalStreams(CameraType::Tele);
    }

    ChxUtils::Memset(&m_preFilterStageResult, 0, sizeof(m_preFilterStageResult));
    ChxUtils::Memset(&m_scaleStageResult, 0, sizeof(m_scaleStageResult));
    ChxUtils::Memset(&m_blendStageResult, 0, sizeof(m_blendStageResult));
    ChxUtils::Memset(&m_postFilterStageResult, 0, sizeof(m_postFilterStageResult));
    ChxUtils::Memset(&m_bpsRegResultBuffer, 0, sizeof(m_bpsRegResultBuffer));

    m_pOfflineRequestMutex           = Mutex::Create();
    CHX_ASSERT(NULL != m_pOfflineRequestMutex);
    m_pOfflineRequestAvailable       = Condition::Create();
    CHX_ASSERT(NULL != m_pOfflineRequestAvailable);

    m_offlineRequestProcessTerminate = FALSE;
    m_aPauseInProgress               = FALSE;
    m_aFlushInProgress               = FALSE;

    m_offlineRequestProcessThread.pPrivateData = this;

    m_pPerfLockManager = PerfLockManager::Create();
    if (NULL == m_pPerfLockManager)
    {
        CHX_LOG_ERROR("Failed to create perflock manager");
    }

    m_captureRequest = { 0 };

    result = ChxUtils::ThreadCreate(FeatureMFNR::RequestThread,
                                    &m_offlineRequestProcessThread,
                                    &m_offlineRequestProcessThread.hThreadHandle);
    CHX_ASSERT(CDKResultSuccess != result);

    return result;
}

VOID FeatureMFNR::InitializeInternalStreams(CameraType type)
{
    for (UINT i = 0; i < MfnrReferenceMax; i++)
    {
        m_pPrefilterOutStream[type][i] = (CHISTREAM*)(CHX_CALLOC(sizeof(CHISTREAM)));
        CHX_ASSERT(NULL != m_pPrefilterOutStream[type][i]);
        m_pBlendOutStream[type][i]     = (CHISTREAM*)(CHX_CALLOC(sizeof(CHISTREAM)));
        CHX_ASSERT(NULL != m_pBlendOutStream[type][i]);
        m_pBlendInStream[type][i]      = (CHISTREAM*)(CHX_CALLOC(sizeof(CHISTREAM)));
        CHX_ASSERT(NULL != m_pBlendInStream[type][i]);
        m_pScaleInStream[type][i]      = (CHISTREAM*)(CHX_CALLOC(sizeof(CHISTREAM)));
        CHX_ASSERT(NULL != m_pScaleInStream[type][i]);

        if (MfnrReferenceFull == i)
        {
            m_pMfnrPostFilterOutStream[type] = (CHISTREAM*)(CHX_CALLOC(sizeof(CHISTREAM)));
            CHX_ASSERT(NULL != m_pMfnrPostFilterOutStream[type]);
        }
    }
    m_pMfnrBpsRegOutStream[type] = (CHISTREAM*)(CHX_CALLOC(sizeof(CHISTREAM)));
    CHX_ASSERT(NULL != m_pMfnrBpsRegOutStream[type]);
    m_pMfnrBpsRegInStream[type]  = (CHISTREAM*)(CHX_CALLOC(sizeof(CHISTREAM)));
    CHX_ASSERT(NULL != m_pMfnrBpsRegInStream[type]);

    CHX_LOG("FeatureMFNR::InitializeMultiCamera role %d m_pRdiStream: %p", type, m_pRdiStream[type]);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMfnr::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::Flush(
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
/// FeatureMfnr::Pause
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::Pause(
       BOOL isForced)
{
    CHX_LOG_INFO("FeatureMfnr::Pause(), isForced %d E.", isForced);
    ChxUtils::AtomicStoreU32(&m_aPauseInProgress, TRUE);
    m_pMfnrResultMutex->Lock();
    m_resultsAvailable = TRUE;
    m_pMfnrResultAvailable->Signal();
    m_pMfnrResultMutex->Unlock();

    m_pOfflineRequestMutex->Lock();
    m_offlineRequestProcessTerminate = TRUE;
    m_pOfflineRequestAvailable->Signal();
    m_pOfflineRequestMutex->Unlock();

    ChxUtils::ThreadTerminate(m_offlineRequestProcessThread.hThreadHandle);
    CHX_LOG_INFO("FeatureMfnr::Pause(), isForced %d X.", isForced);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::Destroy(BOOL isForced)
{
    CHX_LOG("FeatureMFNR::Destroy(), isForced %d E.", isForced);

    m_pSnapshotStream = NULL;
    m_pPreviewStream  = NULL;

    for (int cameraType = CameraType::Wide; cameraType < CameraType::Count; cameraType++)
    {
        m_pRdiStream[cameraType] = NULL;

        for (UINT i = 0; i < MfnrReferenceMax; i++)
        {
            if (m_pPrefilterOutStream[cameraType][i] != NULL)
            {
                CHX_FREE(m_pPrefilterOutStream[cameraType][i]);
                m_pPrefilterOutStream[cameraType][i] = NULL;
            }

            if (m_pBlendOutStream[cameraType][i] != NULL)
            {
                CHX_FREE(m_pBlendOutStream[cameraType][i]);
                m_pBlendOutStream[cameraType][i] = NULL;
            }

            if (m_pBlendInStream[cameraType][i] != NULL)
            {
                CHX_FREE(m_pBlendInStream[cameraType][i]);
                m_pBlendInStream[cameraType][i] = NULL;
            }
            if (m_pScaleInStream[cameraType][i] != NULL)
            {
                CHX_FREE(m_pScaleInStream[cameraType][i]);
                m_pScaleInStream[cameraType][i] = NULL;
            }
        }

        if (m_pMfnrPostFilterOutStream[cameraType] != NULL)
        {
            CHX_FREE(m_pMfnrPostFilterOutStream[cameraType]);
            m_pMfnrPostFilterOutStream[cameraType] = NULL;
        }
        if (m_pMfnrBpsRegOutStream[cameraType] != NULL)
        {
            CHX_FREE(m_pMfnrBpsRegOutStream[cameraType]);
            m_pMfnrBpsRegOutStream[cameraType] = NULL;
        }

        if (m_pMfnrBpsRegInStream[cameraType] != NULL)
        {
            CHX_FREE(m_pMfnrBpsRegInStream[cameraType]);
            m_pMfnrBpsRegInStream[cameraType] = NULL;
        }

        for (UINT j = 0; j < MfnrReferenceMax; j++)
        {
            if (NULL != m_pMfnrBufferManager[cameraType][MfnrStagePrefilter][j])
            {
                m_pMfnrBufferManager[cameraType][MfnrStagePrefilter][j]->Destroy();
                m_pMfnrBufferManager[cameraType][MfnrStagePrefilter][j] = NULL;
            }
            if (NULL != m_pMfnrBufferManager[cameraType][MfnrStageBlend][j])
            {
                m_pMfnrBufferManager[cameraType][MfnrStageBlend][j]->Destroy();
                m_pMfnrBufferManager[cameraType][MfnrStageBlend][j] = NULL;
            }
        }


        if (m_pMfnrBpsRegOutBufferManager[cameraType] != NULL)
        {
            m_pMfnrBpsRegOutBufferManager[cameraType]->Destroy();
            m_pMfnrBpsRegOutBufferManager[cameraType] = NULL;
        }
    }

    if (m_pInterStageMetadata != NULL)
    {
        ChxUtils::FreeMetaData(m_pInterStageMetadata);
        m_pInterStageMetadata = NULL;
    }

    if (NULL != m_debugDataOffline.pData)
    {
        CHX_FREE(m_debugDataOffline.pData);
        m_debugDataOffline.pData    = NULL;
        m_debugDataOffline.size     = 0;
    }
    if (NULL != m_debugDataOfflineSnapshot.pData)
    {
        CHX_FREE(m_debugDataOfflineSnapshot.pData);
        m_debugDataOfflineSnapshot.pData    = NULL;
        m_debugDataOfflineSnapshot.size     = 0;
    }

    // Mutex destroy
    if (NULL != m_pMfnrResultMutex)
    {
        m_pMfnrResultMutex->Destroy();
        m_pMfnrResultMutex = NULL;
    }

    // Result destroy
    if (NULL != m_pMfnrResultAvailable)
    {
        m_pMfnrResultAvailable->Destroy();
        m_pMfnrResultAvailable = NULL;
    }

    // Mutex destroy
    if (NULL != m_pRDIResultMutex)
    {
        m_pRDIResultMutex->Destroy();
        m_pRDIResultMutex = NULL;
    }

    // Condition destroy
    if (NULL != m_pRDIResultAvailable)
    {
        m_pRDIResultAvailable->Destroy();
        m_pRDIResultAvailable = NULL;
    }

    // Mutex destroy
    if (NULL != m_pSnapshotResultMutex)
    {
        m_pSnapshotResultMutex->Destroy();
        m_pSnapshotResultMutex = NULL;
    }

    // Condition destroy
    if (NULL != m_pSnapshotResultAvailable)
    {
        m_pSnapshotResultAvailable->Destroy();
        m_pSnapshotResultAvailable = NULL;
    }

    if (NULL != m_pApplicationInputMeta)
    {
        free_camera_metadata(m_pApplicationInputMeta);
        m_pApplicationInputMeta = NULL;
    }

    if (NULL != m_pPerfLockManager)
    {
        m_pPerfLockManager->Destroy();
        m_pPerfLockManager = NULL;
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

    m_pChiUsecase = NULL;
    ChxUtils::AtomicStoreU32(&m_aPauseInProgress, FALSE);
    ChxUtils::AtomicStoreU32(&m_aFlushInProgress, FALSE);

    // free m_captureRequest output_buffers memory
    if (NULL != m_captureRequest.output_buffers)
    {
        CHX_FREE(const_cast<VOID*>(static_cast<const VOID*>(m_captureRequest.output_buffers)));
        m_captureRequest.output_buffers = NULL;
    }

    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::GetTargetIndex
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT FeatureMFNR::GetTargetIndex(ChiTargetPortDescriptorInfo* pTargets, const char* pTargetName)
{
    INT index = -1;

    for (UINT i = 0; i < pTargets->numTargets; i++)
    {
        if (0 == strcmp(pTargetName, pTargets->pTargetPortDesc[i].pTargetName))
        {
            index = i;
            break;
        }
    }

    return index;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::GetOutputBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::GetOutputBuffer(
    CHIBufferManager*           pBufferManager,
    ChiStream*                  pChiStream,
    camera3_stream_buffer_t*    pOutputBuffer)
{
    CDKResult result = CDKResultSuccess;

    pOutputBuffer->buffer = pBufferManager->GetImageBuffer()->GetBufferHandle();
    pOutputBuffer->acquire_fence = -1;
    pOutputBuffer->stream = (camera3_stream_t*)pChiStream;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::OverrideUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiUsecase* FeatureMFNR::OverrideUsecase(
    LogicalCameraInfo*              pCameraInfo,
    camera3_stream_configuration_t* pStreamConfig)
{
    (VOID)pCameraInfo;

    CameraType                         type         = CameraType::Wide;
    ChiPipelineTargetCreateDescriptor* pPreviewDesc = NULL;
    INT32                              previewPipelineIndex;

    CHX_LOG("OverrideUsecase for MFNR");

    CHX_ASSERT(2 == pStreamConfig->num_streams);

    m_captureRequest.output_buffers = static_cast<camera3_stream_buffer_t*>(
        CHX_CALLOC(sizeof(camera3_stream_buffer_t) * pStreamConfig->num_streams));

    m_pPreviewStream   = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::PreviewStream);
    m_pRdiStream[type] = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::RdiStream);
    m_pSnapshotStream  = m_pUsecase->GetSharedStream(AdvancedCameraUsecase::SnapshotStream);

    m_pChiUsecase      = m_pUsecase->GetChiUseCase();

    // FEATURE_MULTIFRAME_PORT
    /// @todo - TargetName to ease the matching of streamId vs Target per pipeline. Need to map the rest of internal streams
    for (UINT target = 0; target < m_pChiUsecase->numTargets; target++)
    {
        if (ChiFormatRawMIPI == m_pChiUsecase->ppChiTargets[target]->pBufferFormats[0])
        {
            m_rdiStreamIndex   = target;
            m_pRdiTargetBuffer = m_pUsecase->GetTargetBuffer(m_rdiStreamIndex);
            break;
        }
    }

    previewPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType::ZSLPreviewRawType);
    pPreviewDesc         = &m_pChiUsecase->pPipelineTargetCreateDesc[previewPipelineIndex];

    CHX_LOG("preview idx:%d, previewDesc:%p", previewPipelineIndex, pPreviewDesc);

    if (FALSE == m_pUsecase->IsMultiCameraUsecase())
    {
        // Assuming the Raw target has only one format.
        CHX_ASSERT((1 == pPreviewDesc->sinkTarget.pTargetPortDesc[0].pTarget->numFormats) ||
                   (1 == pPreviewDesc->sinkTarget.pTargetPortDesc[1].pTarget->numFormats));

        // Setup the raw target with the RDI stream. Raw has only one buffer format, it will either match here, or
        // not matter in the output format because none of its will match (another assumption)
        if (ChiFormatRawMIPI == pPreviewDesc->sinkTarget.pTargetPortDesc[0].pTarget->pBufferFormats[0])
        {
            pPreviewDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStream[type];
            pPreviewDesc->sinkTarget.pTargetPortDesc[1].pTarget->pChiStream = m_pPreviewStream;
        }
        else
        {
            pPreviewDesc->sinkTarget.pTargetPortDesc[0].pTarget->pChiStream = m_pPreviewStream;
            pPreviewDesc->sinkTarget.pTargetPortDesc[1].pTarget->pChiStream = m_pRdiStream[type];
        }
    }
    else
    {
        CHX_LOG("Realtime pipeline is not enabled/used in the advance usecase for multicamera");
    }

    SetupInternalPipelines(CameraType::Wide);
    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        SetupInternalPipelines(CameraType::Tele);
    }

    return m_pChiUsecase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::SetupInternalMFNRPreFilterPipeline - pre-filter pipeline (#1)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::SetupInternalMFNRPreFilterPipeline(CameraType type)
{
    AdvancedPipelineType               pipelineType;
    UINT                               mfnrFullRefIndex;
    UINT                               mfnrDs4Index;
    UINT                               mfnrBpsRegOutUIndex;
    INT32                              prefilterPipelineIndex;
    ChiPipelineTargetCreateDescriptor* pPrefilterPipelineDesc = NULL;
    const CHAR*                        pTargetName            = NULL;
    ChiTargetPortDescriptorInfo*       pSinkTarget            = NULL;
    ChiTargetPortDescriptorInfo*       pSrcTarget             = NULL;

    pipelineType           = (type == CameraType::Wide) ?
                                 AdvancedPipelineType::MFNRPrefilterType :
                                 AdvancedPipelineType::MFNRPrefilterAuxType;

    prefilterPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(pipelineType);
    pPrefilterPipelineDesc = &m_pChiUsecase->pPipelineTargetCreateDesc[prefilterPipelineIndex];

    // Source Port(s)
    pSrcTarget             = &pPrefilterPipelineDesc->sourceTarget;

    pSrcTarget->pTargetPortDesc[0].pTarget->pChiStream = m_pRdiStream[type];

    // Sink Port(s)
    pSinkTarget         = &pPrefilterPipelineDesc->sinkTarget;

    pTargetName         = (type == CameraType::Wide) ? "TARGET_BUFFER_FULL_OUT_REF" : "TARGET_BUFFER_FULL_OUT_REF_AUX";
    mfnrFullRefIndex    = GetTargetIndex(pSinkTarget, pTargetName);
    pSinkTarget->pTargetPortDesc[mfnrFullRefIndex].pTarget->pChiStream    = m_pPrefilterOutStream[type][MfnrReferenceFull];

    pTargetName         = (type == CameraType::Wide) ? "TARGET_BUFFER_DS4_OUT_REF" : "TARGET_BUFFER_DS4_OUT_REF_AUX";
    mfnrDs4Index        = GetTargetIndex(pSinkTarget, pTargetName);
    pSinkTarget->pTargetPortDesc[mfnrDs4Index].pTarget->pChiStream        = m_pPrefilterOutStream[type][MfnrReferenceDS4];

    pTargetName         = (type == CameraType::Wide) ? "TARGET_BUFFER_REG_OUT" : "TARGET_BUFFER_REG_OUT_AUX";
    mfnrBpsRegOutUIndex = GetTargetIndex(pSinkTarget, pTargetName);
    pSinkTarget->pTargetPortDesc[mfnrBpsRegOutUIndex].pTarget->pChiStream = m_pMfnrBpsRegOutStream[type];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::SetupInternalMFNRBlendPipeline - Blend pipeline (#2)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::SetupInternalMFNRBlendPipeline(CameraType type)
{
    AdvancedPipelineType               pipelineType;
    INT32                              blendPipelineIndex;
    UINT                               mfnrDs4RefIndex;
    UINT                               mfnrInFullRefIndex;
    UINT                               mfnrInDs4RefIndex;
    UINT                               mfnrInRawIndex;
    UINT                               mfnrInRegOutIndex;
    UINT                               mfnrFullRefIndex;
    UINT                               mfnrDs4Index;
    ChiPipelineTargetCreateDescriptor* pBlendPipelineDesc = NULL;
    const CHAR*                        pTargetName        = NULL;
    ChiTargetPortDescriptorInfo*       pSinkTarget        = NULL;
    ChiTargetPortDescriptorInfo*       pSrcTarget         = NULL;

    pipelineType       = (type == CameraType::Wide) ?
                             AdvancedPipelineType::MFNRBlendType :
                             AdvancedPipelineType::MFNRBlendAuxType;

    blendPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(pipelineType);
    pBlendPipelineDesc = &m_pChiUsecase->pPipelineTargetCreateDesc[blendPipelineIndex];

    // Source Port(s)
    pSrcTarget         = &pBlendPipelineDesc->sourceTarget;

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_DS4_SCALE_IN_REF" : "TARGET_BUFFER_DS4_SCALE_IN_REF_AUX";
    mfnrDs4RefIndex    = GetTargetIndex(pSrcTarget, pTargetName);
    pSrcTarget->pTargetPortDesc[mfnrDs4RefIndex].pTarget->pChiStream    = m_pScaleInStream[type][MfnrReferenceDS4];

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_FULL_IN_REF" : "TARGET_BUFFER_FULL_IN_REF_AUX";
    mfnrInFullRefIndex = GetTargetIndex(pSrcTarget, pTargetName);
    pSrcTarget->pTargetPortDesc[mfnrInFullRefIndex].pTarget->pChiStream = m_pBlendInStream[type][MfnrReferenceFull];

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_DS4_IN_REF" : "TARGET_BUFFER_DS4_IN_REF_AUX";
    mfnrInDs4RefIndex  = GetTargetIndex(pSrcTarget, pTargetName);
    pSrcTarget->pTargetPortDesc[mfnrInDs4RefIndex].pTarget->pChiStream  = m_pBlendInStream[type][MfnrReferenceDS4];

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_RAW" : "TARGET_BUFFER_RAW_AUX";
    mfnrInRawIndex     = GetTargetIndex(pSrcTarget, pTargetName);
    pSrcTarget->pTargetPortDesc[mfnrInRawIndex].pTarget->pChiStream     = m_pRdiStream[type];

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_REG_IN" : "TARGET_BUFFER_REG_IN_AUX";
    mfnrInRegOutIndex  = GetTargetIndex(pSrcTarget, pTargetName);
    pSrcTarget->pTargetPortDesc[mfnrInRegOutIndex].pTarget->pChiStream  = m_pMfnrBpsRegInStream[type];

    // Sink Ports
    pSinkTarget        = &pBlendPipelineDesc->sinkTarget;

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_FULL_OUT_REF" : "TARGET_BUFFER_FULL_OUT_REF_AUX";
    mfnrFullRefIndex   = GetTargetIndex(pSinkTarget, pTargetName);
    pSinkTarget->pTargetPortDesc[mfnrFullRefIndex].pTarget->pChiStream  = m_pBlendOutStream[type][MfnrReferenceFull];

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_DS4_OUT_REF" : "TARGET_BUFFER_DS4_OUT_REF_AUX";
    mfnrDs4Index       = GetTargetIndex(pSinkTarget, pTargetName);
    pSinkTarget->pTargetPortDesc[mfnrDs4Index].pTarget->pChiStream      = m_pBlendOutStream[type][MfnrReferenceDS4];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::SetupInternalMFNRBlendPipeline - post-filter pipeline (#3)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::SetupInternalMFNRPostFilterPipeline(CameraType type)
{
    AdvancedPipelineType               pipelineType;
    UINT                               mfnrDs4RefIndex;
    UINT                               mfnrInFullRefIndex;
    UINT                               mfnrInDs4RefIndex;
    UINT                               mfnrInRawIndex;
    UINT                               mfnrInRegOutIndex;
    INT32                              postfilterPipelineIndex;
    ChiPipelineTargetCreateDescriptor* pPostfilterPipelineDesc = NULL;
    ChiTargetPortDescriptorInfo*       pSinkTarget             = NULL;
    ChiTargetPortDescriptorInfo*       pSrcTarget              = NULL;
    const CHAR*                        pTargetName             = NULL;

    pipelineType            = (type == CameraType::Wide) ?
                                  AdvancedPipelineType::MFNRPostFilterType :
                                  AdvancedPipelineType::MFNRPostFilterAuxType;

    postfilterPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(pipelineType);
    pPostfilterPipelineDesc = &m_pChiUsecase->pPipelineTargetCreateDesc[postfilterPipelineIndex];

    // Source Port(s)
    pSrcTarget         = &pPostfilterPipelineDesc->sourceTarget;

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_DS4_SCALE_IN_REF" : "TARGET_BUFFER_DS4_SCALE_IN_REF_AUX";
    mfnrDs4RefIndex    = GetTargetIndex(pSrcTarget, pTargetName);
    pSrcTarget->pTargetPortDesc[mfnrDs4RefIndex].pTarget->pChiStream    = m_pScaleInStream[type][MfnrReferenceDS4];

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_FULL_IN_REF" : "TARGET_BUFFER_FULL_IN_REF_AUX";
    mfnrInFullRefIndex = GetTargetIndex(pSrcTarget, pTargetName);
    pSrcTarget->pTargetPortDesc[mfnrInFullRefIndex].pTarget->pChiStream = m_pBlendInStream[type][MfnrReferenceFull];

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_DS4_IN_REF" : "TARGET_BUFFER_DS4_IN_REF_AUX";
    mfnrInDs4RefIndex  = GetTargetIndex(pSrcTarget, pTargetName);
    pSrcTarget->pTargetPortDesc[mfnrInDs4RefIndex].pTarget->pChiStream  = m_pBlendInStream[type][MfnrReferenceDS4];

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_RAW" : "TARGET_BUFFER_RAW_AUX";
    mfnrInRawIndex     = GetTargetIndex(pSrcTarget, pTargetName);
    pSrcTarget->pTargetPortDesc[mfnrInRawIndex].pTarget->pChiStream     = m_pRdiStream[type];

    pTargetName        = (type == CameraType::Wide) ? "TARGET_BUFFER_REG_IN" : "TARGET_BUFFER_REG_IN_AUX";
    mfnrInRegOutIndex  = GetTargetIndex(pSrcTarget, pTargetName);
    pSrcTarget->pTargetPortDesc[mfnrInRegOutIndex].pTarget->pChiStream  = m_pMfnrBpsRegInStream[type];

    // Sink Port(s)
    pSinkTarget        = &pPostfilterPipelineDesc->sinkTarget;

    pSinkTarget->pTargetPortDesc[0].pTarget->pChiStream = m_pMfnrPostFilterOutStream[type];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::SetupInternalMFNRBlendPipeline - jpeg pipeline (#4 | final)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::SetupInternalMFNRSnapshotPipeline(CameraType type)
{
    AdvancedPipelineType               pipelineType;
    INT32                              snapshotPipelineIndex;
    ChiPipelineTargetCreateDescriptor* pSnapshotDesc = NULL;
    const CHAR*                        pTargetName   = NULL;
    ChiTargetPortDescriptorInfo*       pSinkTarget   = NULL;
    ChiTargetPortDescriptorInfo*       pSrcTarget    = NULL;

    pipelineType          = (type == CameraType::Wide) ?
                                AdvancedPipelineType::InternalZSLYuv2JpegMFNRType :
                                AdvancedPipelineType::InternalZSLYuv2JpegMFNRAuxType;

    snapshotPipelineIndex = m_pUsecase->GetPipelineIdByAdvancedPipelineType(pipelineType);
    pSnapshotDesc         = &m_pChiUsecase->pPipelineTargetCreateDesc[snapshotPipelineIndex];

    // Source Port(s)
    pSrcTarget            = &pSnapshotDesc->sourceTarget;

    pSrcTarget->pTargetPortDesc[0].pTarget->pChiStream  = m_pMfnrPostFilterOutStream[type];

    // Sink Port(s)
    pSinkTarget           = &pSnapshotDesc->sinkTarget;

    pSinkTarget->pTargetPortDesc[0].pTarget->pChiStream = m_pSnapshotStream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::SetupInternalPipelines
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::SetupInternalPipelines(CameraType type)
{
    m_pRdiStream[type] = m_pUsecase->GetSharedStream((type == CameraType::Wide) ?
                             AdvancedCameraUsecase::RdiStream :
                             AdvancedCameraUsecase::RdiStreamAux);

    SetupInternalMFNRPreFilterPipeline(type);

    SetupInternalMFNRBlendPipeline(type);

    SetupInternalMFNRPostFilterPipeline(type);

    SetupInternalMFNRSnapshotPipeline(type);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::PipelineCreated
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::PipelineCreated(
    UINT sessionId,    ///< Id of session created
    UINT pipelineId)   ///< Pipeline of session created
{
    CDKResult                      result            = CDKResultSuccess;
    ChiSensorModeInfo*             pSensorInfo       = NULL;
    const ChiPipelineInputOptions* pInputOptions     = NULL;

    // FEATURE_MULTIFRAME_PORT
    /// @todo need to find the mapping between session's pipelineId vs XMLs pipelineId
    UINT                           sessionPipelineId = m_pUsecase->GetSessionData(sessionId)->numPipelines - 1;

    pSensorInfo   = m_pUsecase->GetSessionData(sessionId)->pipelines[sessionPipelineId].pPipeline->GetSensorModeInfo();
    pInputOptions = m_pUsecase->GetSessionData(sessionId)->pipelines[sessionPipelineId].pPipeline->GetInputOptions();

    CHX_LOG_INFO("MFNR pipelineId: %d, Sensor widthxheight: %d x %d, InputOptions widthxheight: %d x %d",
        pipelineId,
        pSensorInfo->frameDimension.width,
        pSensorInfo->frameDimension.height,
        pInputOptions->bufferOptions.optimalDimension.width,
        pInputOptions->bufferOptions.optimalDimension.height);

    // FEATURE_MULTIFRAME_PORT
    /// @todo - Need to find means of associating grallocUsage
    switch (m_pUsecase->GetAdvancedPipelineTypeByPipelineId(pipelineId))
    {
    case AdvancedPipelineType::InternalZSLYuv2JpegMFNRType:
    case AdvancedPipelineType::InternalZSLYuv2JpegMFNRAuxType:
        {
            UINT       prefilterWidth  = pSensorInfo->frameDimension.width;
            UINT       prefilterHeight = pSensorInfo->frameDimension.height;
            UINT32     scale           = 1;
            UINT32     width           = 0;
            UINT32     height          = 0;
            CameraType type            = CameraType::Wide;

            if (m_pUsecase->GetAdvancedPipelineTypeByPipelineId(pipelineId) ==
                AdvancedPipelineType::InternalZSLYuv2JpegMFNRAuxType)
            {
                type = CameraType::Tele;
            }

            for (UINT i = 0; i < MfnrReferenceMax; i++)
            {
                CHISTREAMFORMAT format       = (i == MfnrReferenceFull) ? ChiStreamFormatUBWCTP10 : ChiStreamFormatPD10;
                GrallocUsage    grallocUsage = 0;

                scale  = static_cast<UINT>(pow(4, i));
                width  = ChxUtils::EvenCeilingUINT32(ChxUtils::AlignGeneric32(prefilterWidth, scale) / scale);
                height = ChxUtils::EvenCeilingUINT32(ChxUtils::AlignGeneric32(prefilterHeight, scale) / scale);
                if (ChiStreamFormatUBWCTP10 == format)
                {
                    grallocUsage = GRALLOC1_PRODUCER_USAGE_CAMERA |
                        GRALLOC1_CONSUMER_USAGE_CAMERA |
                        GRALLOC1_PRODUCER_USAGE_PRIVATE_0 |
                        (1ULL << 27);
                }
                m_pPrefilterOutStream[type][i]->format        = format;
                m_pPrefilterOutStream[type][i]->width         = width;
                m_pPrefilterOutStream[type][i]->height        = height;
                m_pPrefilterOutStream[type][i]->maxNumBuffers = 0;
                m_pPrefilterOutStream[type][i]->rotation      = StreamRotationCCW0;
                m_pPrefilterOutStream[type][i]->streamType    = ChiStreamTypeOutput;
                m_pPrefilterOutStream[type][i]->grallocUsage  = grallocUsage;

                *m_pBlendOutStream[type][i]           = *m_pPrefilterOutStream[type][i];
                *m_pBlendInStream[type][i]            = *m_pBlendOutStream[type][i];
                *m_pScaleInStream[type][i]            = *m_pBlendOutStream[type][i];
                m_pBlendInStream[type][i]->streamType = ChiStreamTypeInput;
                m_pScaleInStream[type][i]->streamType = ChiStreamTypeInput;
                if (i == MfnrReferenceFull)
                {
                    *m_pMfnrPostFilterOutStream[type]        = *m_pPrefilterOutStream[type][i];
                    m_pMfnrPostFilterOutStream[type]->width  = m_pSnapshotStream->width;
                    m_pMfnrPostFilterOutStream[type]->height = m_pSnapshotStream->height;
                    m_pMfnrPostFilterOutStream[type]->format =(CHISTREAMFORMAT)HAL_PIXEL_FORMAT_YCrCb_420_SP;
                }
            }

            width  = ChxUtils::EvenCeilingUINT32(prefilterWidth / 3);
            height = ChxUtils::EvenCeilingUINT32(prefilterHeight / 3);

            m_pMfnrBpsRegOutStream[type]->format        = ChiStreamFormatYCbCr420_888;
            m_pMfnrBpsRegOutStream[type]->width         = width;
            m_pMfnrBpsRegOutStream[type]->height        = height;
            m_pMfnrBpsRegOutStream[type]->maxNumBuffers = 0;
            m_pMfnrBpsRegOutStream[type]->rotation      = StreamRotationCCW0;
            m_pMfnrBpsRegOutStream[type]->streamType    = ChiStreamTypeOutput;
            m_pMfnrBpsRegOutStream[type]->grallocUsage  = 0;
            *m_pMfnrBpsRegInStream[type]                = *m_pMfnrBpsRegOutStream[type];
            m_pMfnrBpsRegInStream[type]->streamType     = ChiStreamTypeInput;

            break;
        }
    case AdvancedPipelineType::MFNRPrefilterType:
        {
            CameraType type = CameraType::Wide;

            m_pRdiStream[type]->format        = ChiStreamFormatRaw10;
            // Create the RDI stream output based on the input buffer requirements to generate the Bayer stream buffer
            m_pRdiStream[type]->width         = pSensorInfo->frameDimension.width;
            m_pRdiStream[type]->height        = pSensorInfo->frameDimension.height;
            m_pRdiStream[type]->maxNumBuffers = 0;
            m_pRdiStream[type]->rotation      = StreamRotationCCW90;
            m_pRdiStream[type]->streamType    = ChiStreamTypeOutput;
            m_pRdiStream[type]->grallocUsage  = 0;

            break;
        }
    case AdvancedPipelineType::MFNRPostFilterType:
    case AdvancedPipelineType::MFNRPostFilterAuxType:
        {
            UINT32     SensorWidth  = pSensorInfo->frameDimension.width;
            UINT32     SendorHeight = pSensorInfo->frameDimension.height;
            UINT32     scale        = 1;
            UINT32     width        = 0;
            UINT32     height       = 0;
            CameraType type         = CameraType::Wide;

            if (m_pUsecase->GetAdvancedPipelineTypeByPipelineId(pipelineId) == AdvancedPipelineType::MFNRPostFilterAuxType)
            {
                type = CameraType::Tele;
            }

            for (UINT i = 0; i < MfnrReferenceMax; i++)
            {
                CHIBufferManagerCreateData createPreFilterBuffers = { 0 };

                scale  = static_cast<UINT>(pow(4, i));
                width  = ChxUtils::EvenCeilingUINT32(ChxUtils::AlignGeneric32(SensorWidth, scale) / scale);
                height = ChxUtils::EvenCeilingUINT32(ChxUtils::AlignGeneric32(SendorHeight, scale) / scale);

                createPreFilterBuffers.width                = (m_pPrefilterOutStream[type][i]->format == ChiStreamFormatPD10) ?
                                                                  (width * 4) : width;
                createPreFilterBuffers.height               = height;
                createPreFilterBuffers.format               = m_pPrefilterOutStream[type][i]->format;
                createPreFilterBuffers.producerFlags        = GRALLOC1_PRODUCER_USAGE_CAMERA |
                                                              GRALLOC1_PRODUCER_USAGE_CPU_READ |
                                                              GRALLOC1_PRODUCER_USAGE_CPU_WRITE;
                createPreFilterBuffers.consumerFlags        = GRALLOC1_CONSUMER_USAGE_CAMERA |
                                                              GRALLOC1_CONSUMER_USAGE_CPU_READ;
                createPreFilterBuffers.immediateBufferCount = 0;
                createPreFilterBuffers.maxBufferCount       = MfnrMaxPreFilterStageBuffers;
                if (ChiStreamFormatUBWCTP10 == m_pPrefilterOutStream[type][i]->format)
                {
                    createPreFilterBuffers.producerFlags |= GRALLOC1_PRODUCER_USAGE_PRIVATE_0 | (1ULL << 27);
                    createPreFilterBuffers.consumerFlags |= GRALLOC1_CONSUMER_USAGE_PRIVATE_0 | (1ULL << 27);
                }

                m_pMfnrBufferManager[type][MfnrStagePrefilter][i] = CHIBufferManager::Create(mfnrBufferManagerNames[i], &createPreFilterBuffers);
                if (m_pMfnrBufferManager[type][MfnrStagePrefilter][i] == NULL)
                {
                    CHX_LOG_ERROR("[ERROR] m_pMfnrBufferManager MfnrStagePrefilter %d allocate fail!!", i);
                    result = CDKResultENoMemory;
                }
                else
                {
                    CHX_LOG_INFO("[SUCCESS] m_pMfnrBufferManager MfnrStagePrefilter success");
                }
            }

            CHIBufferManagerCreateData createBpsRegOutBuffers = { 0 };

            createBpsRegOutBuffers.width                = m_pMfnrBpsRegOutStream[type]->width;
            createBpsRegOutBuffers.height               = m_pMfnrBpsRegOutStream[type]->height;
            createBpsRegOutBuffers.format               = m_pMfnrBpsRegOutStream[type]->format;
            createBpsRegOutBuffers.producerFlags        = GRALLOC1_PRODUCER_USAGE_CAMERA |
                                                          GRALLOC1_PRODUCER_USAGE_CPU_READ |
                                                          GRALLOC1_PRODUCER_USAGE_CPU_WRITE;
            createBpsRegOutBuffers.consumerFlags        = GRALLOC1_CONSUMER_USAGE_CAMERA |
                                                          GRALLOC1_CONSUMER_USAGE_CPU_READ;
            createBpsRegOutBuffers.immediateBufferCount = 0;
            createBpsRegOutBuffers.maxBufferCount       = MfnrMaxBpsRegOutBuffers;

            m_pMfnrBpsRegOutBufferManager[type] = CHIBufferManager::Create("MfnrBpsRegOutBufferManager", &createBpsRegOutBuffers);

            if (m_pMfnrBpsRegOutBufferManager[type] == NULL) {
                CHX_LOG_ERROR("[ERROR] m_pMfnrBpsRegOutBufferManager allocate fail!!");
                result = CDKResultENoMemory;
            }
            else
            {
                CHX_LOG_INFO("[SUCCESS] m_pMfnrBpsRegOutBufferManager success");
            }
            break;
        }
    default:
        {
            CHX_LOG_ERROR("[ERROR] Unknown advanced pipeline type!");
            result = CDKResultEFailed;;
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::ExecuteProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult  FeatureMFNR::ExecuteProcessRequest(
    camera3_capture_request_t* pRequest)         ///< Request parameters
{
    CDKResult                 result             = CDKResultSuccess;
    camera3_stream_buffer_t   previewBuffers[2]  = {{0}};
    UINT                      previewCount       = 0;
    UINT                      snapshotCount      = 0;
    UINT32                    frameNumber        = pRequest->frame_number;
    UINT32                    frameIndex         = (pRequest->frame_number % MaxOutstandingRequests);
    UINT                      snapshotReqIdIndex = (m_maxSnapshotReqId % MaxOutstandingRequests);
    UINT32                    rtPipelineReqId    = 0;
    CameraType                type               = CameraType::Wide;

    m_isLLSSnapshot = FALSE;

    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        RequestMapInfo requestMapInfo = { 0 };

        m_internalRequestId  = pRequest->frame_number;
        requestMapInfo       = m_pUsecase->GetRequestMapInfo(pRequest->frame_number);
        m_requestFrameNumber = requestMapInfo.frameNumber;

        CHX_LOG("FeatureMFNR internalReqId:%d, appFrameNum:%d", m_internalRequestId, m_requestFrameNumber);

        if (0 != m_activeCameraId)
        {
            type = CameraType::Tele;
        }
    }

    if (TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aPauseInProgress)))
    {
        m_resultsAvailable = FALSE;
        m_pMfnrResultMutex->Unlock();
        CHX_LOG_INFO("MFNR pre filter scale process return because of cleanup");
        return CDKResultSuccess;
    }
    if (FALSE == m_pUsecase->IsMultiCameraUsecase())
    {
        camera3_capture_result_t* pUsecaseResult = m_pUsecase->GetCaptureResult(frameIndex);

        pUsecaseResult->result             = NULL;
        pUsecaseResult->frame_number       = pRequest->frame_number;
        pUsecaseResult->num_output_buffers = 0;

        m_isSnapshotFrame[frameIndex]      = FALSE;

        m_sensorModeIndex = m_pUsecase->GetSessionData(m_realtime)->pSession->GetSensorModeInfo()->modeIndex;

        ChxUtils::FillTuningModeData(const_cast<camera_metadata_t*>(pRequest->settings),
            pRequest,
            m_sensorModeIndex,
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
                ChxUtils::Memcpy(&previewBuffers[previewCount],
                                 &pRequest->output_buffers[i],
                                 sizeof(camera3_stream_buffer_t));
                previewCount++;
            }

            if (m_pSnapshotStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
            {
                ChxUtils::FreeMetaData(m_pSnapshotInputMeta[snapshotReqIdIndex]);
                m_pSnapshotInputMeta[snapshotReqIdIndex]    =
                    ChxUtils::AllocateCopyMetaData(reinterpret_cast<const VOID*>(pRequest->settings));

                CHX_LOG("Snapshot Frame %d", pRequest->frame_number);
                m_requestFrameNumber = pRequest->frame_number;

                ChxUtils::Memcpy(&m_snapshotBuffers[snapshotReqIdIndex][snapshotCount],
                                 &pRequest->output_buffers[i],
                                 sizeof(camera3_stream_buffer_t));

                UINT32 reqId = m_pRdiTargetBuffer->lastReadySequenceID;
                m_mfnrTotalNumFrames = GetMFNRTotalFramesByGain(m_pRdiTargetBuffer->bufferQueue[reqId % BufferQueueDepth].pMetadata);

                CHX_LOG("lastReadySequenceID:%d, m_mfnrTotalNumFrames:%d", reqId, m_mfnrTotalNumFrames);

                snapshotCount++;
            }
        }

        PipelineData* pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_realtime, 0));
        UINT          requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);

        pPipelineData->seqIdToFrameNum[requestIdIndex] = frameNumber;

        CHX_LOG("Realtime AppFrameNum to ReqId: %d <--> %d", frameNumber, pPipelineData->seqId);

        rtPipelineReqId = pPipelineData->seqId;

        ImageBuffer* pImageBuffer = m_pRdiTargetBuffer->pBufferManager->GetImageBuffer();
        if (NULL != pImageBuffer)
        {
            previewBuffers[previewCount].buffer         = pImageBuffer->GetBufferHandle();
            previewBuffers[previewCount].stream         = reinterpret_cast<camera3_stream_t*>(m_pRdiStream[type]);
            previewBuffers[previewCount].acquire_fence  = -1;
            previewCount++;
        }
        else
        {
            CHX_LOG_ERROR("FeatureMFNR: GetImageBuffer failed, No RDI buffer for frameNumber %d", frameNumber);
        }

        const Session* pSession = m_pUsecase->GetSessionData(m_realtime)->pSession;

        CHICAPTUREREQUEST captureRequest = { 0 };

        captureRequest.frameNumber       = pPipelineData->seqId++;
        captureRequest.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
        captureRequest.numOutputs        = previewCount;
        captureRequest.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(previewBuffers);
        captureRequest.pMetadata         = pRequest->settings;

        CHIPIPELINEREQUEST submitRequest = { 0 };
        submitRequest.pSessionHandle     = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
        submitRequest.numRequests        = 1;
        submitRequest.pCaptureRequests   = &captureRequest;

        m_pUsecase->SetRequestToFeatureMapping(m_realtime, captureRequest.frameNumber, this);

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

        if ((0 < snapshotCount) && (TRUE == m_pUsecase->IsLLSNeeded()))
        {
            // set m_isLLSSnapshot to TRUE here, check this flag all through the code,
            // because m_pUsecase->IsLLSNeeded() might change during the snapshot.
            m_isLLSSnapshot = TRUE;

            CHX_LOG_INFO("FeatureMFNR Trigger LLS snapshot");
            result = TriggerInternalLLSRequests(pRequest);
        }
    }
    else
    {
        for (UINT32 i = 0; i < pRequest->num_output_buffers; i++)
        {
            if (m_pSnapshotStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
            {
                ChxUtils::FreeMetaData(m_pSnapshotInputMeta[snapshotReqIdIndex]);
                m_pSnapshotInputMeta[snapshotReqIdIndex] =
                    ChxUtils::AllocateCopyMetaData(reinterpret_cast<const VOID*>(pRequest->settings));

                CHX_LOG("Snapshot Frame %d", pRequest->frame_number);

                ChxUtils::Memcpy(&m_snapshotBuffers[snapshotReqIdIndex][snapshotCount],
                                 &pRequest->output_buffers[i],
                                 sizeof(camera3_stream_buffer_t));
                snapshotCount++;
            }
        }

        if ((0 == snapshotCount) && (NULL != pRequest->output_buffers))
        {
            // this means the final snapshot buffer is not included in the request from usecasemc
            // fallback to use snapshot buffer saved during GetRequestInfo()
            pRequest->num_output_buffers = 1;
            ChxUtils::Memcpy(const_cast<camera3_stream_buffer_t*>(&pRequest->output_buffers[0]),
                             &m_snapshotBuffers[snapshotReqIdIndex][0],
                             sizeof(camera3_stream_buffer_t));

            CHX_LOG("use previous saved snapshot buffer");

            snapshotCount++;
        }
    }

    if ((CDKResultSuccess == result) && (0 < snapshotCount))
    {
        CHX_LOG_ERROR("Snapshot in MFNR HW Multiframe");

        m_pOfflineRequestMutex->Lock();

        m_isSnapshotFrame[frameIndex]                 = TRUE;
        m_snapshotBufferNum[snapshotReqIdIndex]       = snapshotCount;
        m_snapshotReqIdToFrameNum[snapshotReqIdIndex] = frameNumber;
        // Do Deep Copy if framework uses same request for future requests
        ChxUtils::DeepCopyCamera3CaptureRequest(pRequest, &m_captureRequest);

        if ((FALSE == m_pUsecase->IsMultiCameraUsecase()) && (TRUE == m_isLLSSnapshot))
        {
            // single camera, LLS snapshot usecase,
            // wait till all RDI buffers ready, then trigger offline reprocess
            result = WaitRDIResultsReady();

            PipelineData* pPipelineData   = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_realtime, 0));
            UINT32        lastReqId       = pPipelineData->seqId - 1;

            m_captureRequest.frame_number = lastReqId;
            CHX_LOG("FeatureMFNR last request id:%d", lastReqId);
        }

        m_pOfflineRequestAvailable->Signal();

        m_pOfflineRequestMutex->Unlock();

        if ((FALSE == m_pUsecase->IsMultiCameraUsecase()) && (TRUE == m_isLLSSnapshot))
        {
            if (TRUE == m_blockPreviewForSnapshot)
            {
                CHX_LOG("FeatureMFNR LLS wait final snapshot ready");

                m_pSnapshotResultMutex->Lock();

                while (FALSE == m_snapshotResultAvailable)
                {
                    m_pSnapshotResultAvailable->Wait(m_pSnapshotResultMutex->GetNativeHandle());
                }
                m_snapshotResultAvailable = FALSE;

                m_pSnapshotResultMutex->Unlock();

                CHX_LOG("FeatureMFNR LLS snapshot ready, accept new requests");
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::TriggerInternalLLSRequests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::TriggerInternalLLSRequests(
    camera3_capture_request_t*  pRequest)
{
    CDKResult result = CDKResultSuccess;

    // already sumbmit one request with preview + RDI, count that one also.
    UINT32                   totalFramesRequired    = GetRequiredInputFrames();
    UINT32                   internalRequestsNum    = totalFramesRequired - 1;
    const UINT32             maxInternalRequestsNum = MfnrMaxInputRDIFrames - 1;
    const camera_metadata_t* pLLSSettings[maxInternalRequestsNum] = { 0 };

    CHX_LOG("FeatureMFNR TriggerInternalLLSRequests(), total required RDI frames:%d", totalFramesRequired);

    if (maxInternalRequestsNum < internalRequestsNum)
    {
        CHX_LOG_ERROR("invalide internalRequestsNum:%d", internalRequestsNum);
        result = CDKResultEInvalidArg;
    }

    if (CDKResultSuccess == result)
    {
        /// Generate internal request settings
        result = GenerateLLSRequestSettings(pRequest->settings,
                                            internalRequestsNum,
                                            &pLLSSettings[0]);

        const Session* pSession       = m_pUsecase->GetSessionData(m_realtime)->pSession;
        PipelineData*  pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_realtime, 0));

        /// Submit request(s) to realtime pipeline
        for (UINT32 i = 0; i < internalRequestsNum; i++)
        {
            /// update seqId <-> appFrameNum mapping
            UINT reqIdIdx = (pPipelineData->seqId % MaxOutstandingRequests);
            pPipelineData->seqIdToFrameNum[reqIdIdx] = pRequest->frame_number;

            /// get RDI buffer
            camera3_stream_buffer_t outputBuffer = { 0 };
            ImageBuffer*            pImageBuffer = m_pRdiTargetBuffer->pBufferManager->GetImageBuffer();

            if (NULL != pImageBuffer)
            {
                outputBuffer.buffer         = pImageBuffer->GetBufferHandle();
                outputBuffer.stream         = reinterpret_cast<camera3_stream_t*>(m_pRdiStream[CameraType::Wide]);
                outputBuffer.acquire_fence  = -1;
            }
            else
            {
                CHX_LOG_ERROR("TriggerInternalLLSRequests GetImageBuffer failed! AppFrameNum = %d, rtReqId = %d",
                    pRequest->frame_number, pPipelineData->seqId);
                result = CDKResultEFailed;
                break;
            }

            if (CDKResultSuccess == result)
            {
                CHX_LOG("FeatureMFNR submit internal RDI requet AppFrameNum:%d <--> rtReqId:%d",
                    pRequest->frame_number, pPipelineData->seqId);

                CHICAPTUREREQUEST request = { 0 };
                request.frameNumber       = pPipelineData->seqId++;
                request.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(pSession->GetPipelineHandle());
                request.numOutputs        = 1; // only RDI buffer
                request.pOutputBuffers    = reinterpret_cast<CHISTREAMBUFFER*>(&outputBuffer);
                request.pMetadata         = pLLSSettings[i];

                CHIPIPELINEREQUEST submitRequest = { 0 };
                submitRequest.pSessionHandle     = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
                submitRequest.numRequests        = 1;
                submitRequest.pCaptureRequests   = &request;

                m_pUsecase->SetRequestToFeatureMapping(m_realtime, request.frameNumber, this);
                result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

                if (CDKResultSuccess != result)
                {
                    CHX_LOG_ERROR("TriggerInternalLLSRequests SubmitRequest failed!");
                    break;
                }
            }
        }

        if (CDKResultSuccess == result)
        {
            UINT32 lastReqId = pPipelineData->seqId - 1;

            m_triggerMFNRReprocess[lastReqId % MaxOutstandingRequests] = TRUE;

            CHX_LOG("lastReqId:%d, set m_triggerMFNRReprocess[%d] to TRUE", lastReqId, lastReqId % MaxOutstandingRequests);
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::GenerateLLSRequestSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::GenerateLLSRequestSettings(
    const camera_metadata_t*    pInputSetting,
    UINT32                      numFrames,
    const camera_metadata_t**   pOutputSettingArray)
{
    CDKResult result = CDKResultSuccess;

    for (UINT32 i = 0; i < numFrames; i++)
    {
        // settings are same for all the RDI request, so simple pointing to the original meta,
        // no copy here, so no need to care about free metadata
        pOutputSettingArray[i] = pInputSetting;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::WaitRDIResultsReady
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::WaitRDIResultsReady()
{
    CDKResult result = CDKResultSuccess;

    CHX_LOG("FeatureMFNR LLS wait for RDI buffers/metadatas ready");

    m_pRDIResultMutex->Lock();
    while (FALSE == m_allRDIResultsAvaliable)
    {
        m_pRDIResultAvailable->Wait(m_pRDIResultMutex->GetNativeHandle());
    }
    m_allRDIResultsAvaliable = FALSE;
    m_pRDIResultMutex->Unlock();

    CHX_LOG("FeatureMFNR LLS RDI buffers/metadatas are ready");

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::GetRequestInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::GetRequestInfo(
    camera3_capture_request_t*  pRequest,
    FeatureRequestInfo*         pOutputRequests)
{
    CDKResult result = CDKResultSuccess;

    ChxUtils::ResetMetadata(m_pApplicationInputMeta);
    ChxUtils::MergeMetadata(const_cast<camera_metadata_t*>(pRequest->settings), m_pApplicationInputMeta);

    UINT32 snapshotReqIdIndex = (m_maxSnapshotReqId % MaxOutstandingRequests);

    for (UINT32 i = 0; i < pRequest->num_output_buffers; i++)
    {
        if (m_pSnapshotStream == reinterpret_cast<CHISTREAM*>(pRequest->output_buffers[i].stream))
        {
            CHX_LOG("Copy Snapshot buffer, appFrameNum %d", pRequest->frame_number);
            ChxUtils::Memcpy(&m_snapshotBuffers[snapshotReqIdIndex][0],
                &pRequest->output_buffers[i],
                sizeof(camera3_stream_buffer_t));
         }
    }

    if (TRUE == m_pUsecase->IsLLSNeeded())
    {
        UINT32                   totalFramesRequired                 = GetRequiredInputFrames();
        const camera_metadata_t* pLLSSettings[MfnrMaxInputRDIFrames] = { 0 };

        CHX_LOG("FeatureMFNR GetRequestInfo(), total required RDI frames:%d", totalFramesRequired);

        if (MfnrMaxInputRDIFrames < totalFramesRequired)
        {
            CHX_LOG_ERROR("invalide totalFramesRequired:%d", totalFramesRequired);
            result = CDKResultEInvalidArg;
        }

        if (CDKResultSuccess == result)
        {
            result = GenerateLLSRequestSettings(m_pApplicationInputMeta,
                                                totalFramesRequired,
                                                &pLLSSettings[0]);
        }

        if ((CDKResultSuccess == result) && (NULL != pOutputRequests))
        {
            ChxUtils::Memset(pOutputRequests, 0, sizeof(FeatureRequestInfo));

            for (UINT32 i = 0; i < totalFramesRequired; i++)
            {
                ChxUtils::Memcpy(&pOutputRequests->request[i], pRequest, sizeof(camera3_capture_request));
                pOutputRequests->request[i].settings = pLLSSettings[i];
            }

            pOutputRequests->numOfRequest      = totalFramesRequired;
            pOutputRequests->isReturnResult[0] = TRUE;

            CHX_LOG("total input RDI frame required:%d, isReturnResult[0]:%d",
                totalFramesRequired,
                pOutputRequests->isReturnResult[0]);
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::FetchVendorTagsForAnchorFrameSelection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::FetchVendorTagsForAnchorFrameSelection(
    UINT32* pVendorTagLocationFocusValue,
    UINT32* pVendorTagLocationBuckets,
    UINT32* pVendorTagLocationStats)
{
    CDKResult  result       = CDKResultSuccess;
    CHITAGSOPS vendorTagOps = { 0 };

    CHX_ASSERT(NULL != pVendorTagLocationFocusValue);
    CHX_ASSERT(NULL != pVendorTagLocationBuckets);
    CHX_ASSERT(NULL != pVendorTagLocationStats);

    *pVendorTagLocationFocusValue = 0;
    *pVendorTagLocationBuckets    = 0;
    *pVendorTagLocationStats      = 0;

    ExtensionModule::GetInstance()->GetVendorTagOps(&vendorTagOps);

    result = vendorTagOps.pQueryVendorTagLocation(
                 "org.quic.camera.focusvalue",
                 "FocusValue",
                 pVendorTagLocationFocusValue);
    if (CDKResultSuccess == result)
    {
        result = vendorTagOps.pQueryVendorTagLocation(
                     "org.codeaurora.qcamera3.histogram",
                     "buckets",
                     pVendorTagLocationBuckets);
        if (CDKResultSuccess == result)
        {
            result = vendorTagOps.pQueryVendorTagLocation(
                         "org.codeaurora.qcamera3.histogram",
                         "stats",
                         pVendorTagLocationStats);
            if (CDKResultSuccess != result)
            {
                CHX_LOG_ERROR("Finding vendor tag location for Stats failed");
            }
        }
        else
        {
            CHX_LOG_ERROR("Finding vendor tag location for buckets failed");
        }
    }
    else
    {
        CHX_LOG_ERROR("Finding vendor tag location for FocusValue failed");
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::PopulateAnchorFrameSelectionDataFromVTags
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::PopulateAnchorFrameSelectionDataFromVTags(
    AnchorFrameSelectionData* pAnchorFrameSelectionData)
{
    CDKResult              result                      = CDKResultSuccess;
    UINT32                 vendorTagLocationFocusValue = 0;
    UINT32                 vendorTagLocationBuckets    = 0;
    UINT32                 vendorTagLocationStats      = 0;
    camera_metadata_entry  focusValue                  = { 0 };
    camera_metadata_entry  bucketValue                 = { 0 };
    camera_metadata_entry  statsValue                  = { 0 };

    m_numOfImagesCaptured = 0;

    CHX_ASSERT(NULL != pAnchorFrameSelectionData);

    result = FetchVendorTagsForAnchorFrameSelection(
                 &vendorTagLocationFocusValue,
                 &vendorTagLocationBuckets,
                 &vendorTagLocationStats);

    if (CDKResultSuccess == result)
    {
        UINT32 zslQueueIndex = 0;
        UINT32 frameIndex    = m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].frameNumber % MaxOutstandingRequests;

        for (zslQueueIndex = 0; zslQueueIndex < (BufferQueueDepth - 1); zslQueueIndex++)
        {
            camera_metadata_t* pMetaData = reinterpret_cast<camera_metadata_t*>(
                                               m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pMetadata);

            if (NULL != pMetaData)
            {
                m_numOfImagesCaptured++;

                // Find an entry with given tag value. If not found, returns -ENOENT. Otherwise,
                // returns entry contents like get_camera_metadata_entry.

                result = find_camera_metadata_entry(pMetaData, vendorTagLocationFocusValue, &focusValue);
                if (CDKResultSuccess == result)
                {
                    result = find_camera_metadata_entry(pMetaData, vendorTagLocationBuckets, &bucketValue);
                    if (CDKResultSuccess == result)
                    {
                        result = find_camera_metadata_entry(pMetaData, vendorTagLocationStats, &statsValue);
                        if (CDKResultSuccess != result)
                        {
                            CHX_LOG_ERROR("failed finding an entry for stats");
                        }
                    }
                    else
                    {
                        CHX_LOG_ERROR("failed finding an entry for buckets");
                    }
                }
                else
                {
                    CHX_LOG_ERROR("failed finding an entry for FocusValue");
                }

                pAnchorFrameSelectionData->timestamps[zslQueueIndex] = m_shutterTimestamp[frameIndex];
                pAnchorFrameSelectionData->focusValue[zslQueueIndex] = *(focusValue.data.f);

                if ((MaxBHistBinNum  == *(bucketValue.data.i32)) ||
                    (NumHDRBHistBins == *(bucketValue.data.i32)))
                {
                    /// @todo (CAMX-4025) - Need to change this block for binning 1024 into 256
                    HDRBHistStatsOutput* pStatsOut = reinterpret_cast<HDRBHistStatsOutput*>(statsValue.data.i32);

                    pAnchorFrameSelectionData->histogram[zslQueueIndex] = pStatsOut->greenHistogram;
                }
            }
        }
    }
    else
    {
        CHX_LOG_ERROR("Finding vendor tag location for Stats failed");
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::CalculateBrightness
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 FeatureMFNR::CalculateBrightness(
    UINT32*  histogram,
    UINT     minHistrogramBin,
    UINT     maxHistrogramBin)
{
    UINT32 counter          = 0;
    UINT32 medianCounter    = 0;
    UINT32 resultBrightness = maxHistrogramBin + 1;

    // Calculate the sum of the histogram's relevant bins
    for (UINT i = minHistrogramBin; i <= maxHistrogramBin; i++)
    {
        medianCounter += histogram[i];
    }

    // Calculate median brightness: accumulate until half the sum
    medianCounter /= 2;

    for (UINT i = minHistrogramBin; i <= maxHistrogramBin; i++)
    {
        counter += histogram[i];

        if (counter >= medianCounter)
        {
            resultBrightness = i;
            break;
        }
    }
    return resultBrightness;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::PopulateAnchorFrameSelectionData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::PopulateAnchorFrameSelectionData(VOID)
{
    CDKResult                result                   = CDKResultSuccess;
    AnchorFrameSelectionData anchorFrameSelectionData = { { 0 } };

    result = PopulateAnchorFrameSelectionDataFromVTags(&anchorFrameSelectionData);
    if (CDKResultSuccess != result)
    {
        CHX_LOG_WARN("Failed in populating vendor tag data into Anchor Frame selection data");
    }

    /// @todo (CAMX-4025) - Why the following hard coded data for anchor frame selection?
    anchorFrameSelectionData.anchorFrameSelectionMode = AnchorFrameSelectionMode::Lighting;
    anchorFrameSelectionData.minHistrogramBin         = 0;
    anchorFrameSelectionData.maxHistrogramBin         = 255;
    anchorFrameSelectionData.numOfImagesToBlend       = 3;
    anchorFrameSelectionData.desiredAnchorFrameIndex  = m_mfnrTotalNumFrames - 1;
    anchorFrameSelectionData.anchorFrameTimeRange     = 200000000;
    anchorFrameSelectionData.brightnessTolerance      = 3;
    anchorFrameSelectionData.removeExpectedBadImages  = FALSE;

    // Set initial processing order
    for (UINT32 i = 0; i < anchorFrameSelectionData.numOfImagesToBlend; ++i)
    {
        m_processingOrder[i]       = i;
        // m_mfnrZslQAnchorIndices[i] = i;
    }

    switch (anchorFrameSelectionData.anchorFrameSelectionMode)
    {
        case AnchorFrameSelectionMode::Sharpness:
        {
            // Choose the sharpest image
            UINT32 anchorFrameIndex    = anchorFrameSelectionData.desiredAnchorFrameIndex;
            UINT64 timeDifference      = 0;
            BOOL   hasBetterFocusValue = FALSE;

            for (UINT32 i = 0; i < m_numOfImagesCaptured; i++)
            {
                timeDifference = abs(static_cast<INT64>(anchorFrameSelectionData.timestamps[i]) -
                                     static_cast<INT64>(anchorFrameSelectionData.timestamps[anchorFrameIndex]));
                if (timeDifference <= anchorFrameSelectionData.anchorFrameTimeRange)
                {
                    hasBetterFocusValue = (anchorFrameSelectionData.focusValue[i] >
                                           anchorFrameSelectionData.focusValue[anchorFrameIndex]);
                    if (TRUE == hasBetterFocusValue)
                    {
                        anchorFrameIndex = i;
                    }
                }
            }

            m_processingOrder[0]                = anchorFrameIndex;
            m_processingOrder[anchorFrameIndex] = 0;

            // Sort the remaining images by sharpness
            std::sort(&m_processingOrder[1],
                      &m_processingOrder[anchorFrameSelectionData.numOfImagesToBlend],
                      [&](INT i1, INT i2) {
                          return static_cast<BOOL>(anchorFrameSelectionData.focusValue[i1] >
                                                   anchorFrameSelectionData.focusValue[i2]);
                      });
        }
        break;

        case AnchorFrameSelectionMode::Lighting:
        {
            INT    brightness[BufferQueueDepth - 1]        = { 0 };
            INT    blendingPotential[BufferQueueDepth - 1] = { 0 };
            BOOL   hasBlendingPotential                    = FALSE;
            UINT32 anchorFrameIndex                        = anchorFrameSelectionData.desiredAnchorFrameIndex;
            UINT64 timeDifference                          = 0;
            BOOL   isBetter                                = FALSE;

            // Calculate brightness
            for (UINT32 i = 0; i < m_numOfImagesCaptured; i++)
            {
                brightness[i] = CalculateBrightness(anchorFrameSelectionData.histogram[i],
                                                    anchorFrameSelectionData.minHistrogramBin,
                                                    anchorFrameSelectionData.maxHistrogramBin);
            }

            // Calculate blending potential with respect to neighbouring frames
            for (UINT32 i1 = 0; i1 < m_numOfImagesCaptured; i1++)
            {
                for (UINT i2 = 0; i2 < i1; i2++)
                {
                    hasBlendingPotential = abs(static_cast<INT>(brightness[i1]) - static_cast<INT>(brightness[i2])) <
                                               static_cast<INT>(anchorFrameSelectionData.brightnessTolerance);
                    if (TRUE == hasBlendingPotential)
                    {
                        blendingPotential[i1]++;
                        blendingPotential[i2]++;
                    }
                }
            }

            // Choose the sharpest image that has maximum blending potential
            for (UINT32 i = 0; i < m_numOfImagesCaptured; i++)
            {
                timeDifference = abs(static_cast<INT64>(anchorFrameSelectionData.timestamps[i]) -
                                     static_cast<INT64>(anchorFrameSelectionData.timestamps[anchorFrameIndex]));
                if (timeDifference <= anchorFrameSelectionData.anchorFrameTimeRange)
                {
                    isBetter = (blendingPotential[i] > blendingPotential[anchorFrameIndex]);

                    if (blendingPotential[i] == blendingPotential[anchorFrameIndex])
                    {
                        isBetter = (anchorFrameSelectionData.focusValue[i] >
                                    anchorFrameSelectionData.focusValue[anchorFrameIndex]);
                    }

                    if (TRUE == isBetter)
                    {
                        anchorFrameIndex = i;
                    }
                }
            }

            m_processingOrder[0]                = anchorFrameIndex;
            m_processingOrder[anchorFrameIndex] = 0;

            // Sort such that potentially blended images are first; otherwise, by sharpness
            std::sort(&m_processingOrder[1],
                      &m_processingOrder[anchorFrameSelectionData.numOfImagesToBlend],
                      [&](INT i1, INT i2) {
                          BOOL blended1 = abs(brightness[anchorFrameIndex] - brightness[i1]) <
                                              static_cast<INT>(anchorFrameSelectionData.brightnessTolerance);
                          BOOL blended2 = abs(brightness[anchorFrameIndex] - brightness[i2]) <
                                              static_cast<INT>(anchorFrameSelectionData.brightnessTolerance);

                          if (blended1 != blended2)
                          {
                              return blended1;
                          }
                          else
                          {
                              return static_cast<BOOL>(anchorFrameSelectionData.focusValue[i1] >
                                                       anchorFrameSelectionData.focusValue[i2]);
                          }
                      });

            if (FALSE == anchorFrameSelectionData.removeExpectedBadImages)
            {
                std::fill(&m_processingOrder[blendingPotential[anchorFrameIndex] + 1],
                          &m_processingOrder[anchorFrameSelectionData.numOfImagesToBlend],
                          -1);
            }
        }
        break;

        case AnchorFrameSelectionMode::TimeStamp:
        default:
        {
            UINT32 anchorFrameIndex = anchorFrameSelectionData.desiredAnchorFrameIndex;

            // Choose the desired image
            m_processingOrder[0]                = anchorFrameIndex;
            m_processingOrder[anchorFrameIndex] = 0;

            // Sort the other images by timestamp closeness
            std::sort(&m_processingOrder[1],
                      &m_processingOrder[anchorFrameSelectionData.numOfImagesToBlend],
                      [&](INT i1, INT i2) {
                          return static_cast<BOOL>(anchorFrameSelectionData.timestamps[i1] <
                                                   anchorFrameSelectionData.timestamps[i2]);
                      });
        }
        break;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::CreateMFNRInputInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::CreateMFNRInputInfo(
    MFNRInputInfo*             pInputInfo,
    camera3_capture_request_t* pRequest)
{
    CDKResult result = CDKResultSuccess;

    if (FALSE == m_pUsecase->IsMultiCameraUsecase())
    {
        UINT32 internalFrameNumber = m_pRdiTargetBuffer->lastReadySequenceID;
        UINT32 zslQueueIndex       = 0;

        // Get Snapshot buffers from capture request output buffers
        GetRequestInfo(pRequest, NULL);

        CHX_LOG("lastReadyFramenumber:%d",internalFrameNumber);
        CHX_ASSERT(INVALIDSEQUENCEID != internalFrameNumber);

        pInputInfo->numOfBuffers = m_mfnrTotalNumFrames;

        /// @todo (CAMX-4025): m_pRdiTargetBuffer (struct TargetBuffer) does not have a member
        ///                    indicating the number of buffers captured/available. This isn't
        ///                    the same as the BufferQueueDepth or the queueDepth(e.g., is set
        ///                    CameraUsecaseBase::AddNewInternalBufferQueue() for reference).
        ///                    OR is it assumed that the m_pRdiTargetBuffer, will have all the
        ///                    BufferQueueDepth buffers already availale before reaching this
        ///                    point?

        // Get the input buffer from ZSL queue, traversing in reverse order
        // starting with the most recent RDI buffer sequence id available
        for (UINT32 frameNumber = 0, i = 0; i < m_mfnrTotalNumFrames; i++)
        {
            frameNumber = (i > internalFrameNumber) ? (0) : (internalFrameNumber - i);

            m_pUsecase->GetTargetBuffer(frameNumber,
                            m_pRdiTargetBuffer,
                            NULL,
                            reinterpret_cast<camera3_stream_buffer_t*>(&(pInputInfo->bufferArray[i])));

            zslQueueIndex            = frameNumber % BufferQueueDepth;
            pInputInfo->pMetadata[i] = ChxUtils::AllocateCopyMetaData(m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].pMetadata);
        }
    }
    else
    {
        UINT32         internalFrameNumber = pRequest->frame_number;
        RequestMapInfo requestInfo         = m_pUsecase->GetRequestMapInfo(internalFrameNumber);
        UINT32         pipelineID          = requestInfo.activePipelineID;

        m_activeCameraId    = requestInfo.masterCameraID;
        internalFrameNumber = m_pUsecase->GetLastReadyFrameNum(pipelineID);

        CHX_LOG("lastReadyFramenumber:%d",internalFrameNumber);

        pInputInfo->numOfBuffers = m_mfnrTotalNumFrames;

        for (UINT32 frameNumber = 0, i = 0; i < m_mfnrTotalNumFrames; i++)
        {
            frameNumber = (i > internalFrameNumber) ? (0) : (internalFrameNumber - i);

            pInputInfo->pMetadata[i] = NULL;
            m_pUsecase->GetInputBufferFromRDIQueue(frameNumber,
                            pipelineID,
                            reinterpret_cast<camera3_stream_buffer_t*>(&(pInputInfo->bufferArray[i])),
                            reinterpret_cast<camera_metadata_t**>(&(pInputInfo->pMetadata[i])));
            pInputInfo->pMetadata[i] = ChxUtils::AllocateCopyMetaData(pInputInfo->pMetadata[i]);
        }
    }

    // dump MFNR input buffer/meta info
    for (UINT32 i = 0; i < pInputInfo->numOfBuffers; ++i)
    {
        CHX_LOG("[%d/%d] stream:%p, buffer handle:%p, meta:%p",
            i, pInputInfo->numOfBuffers,
            pInputInfo->bufferArray[i].pStream, pInputInfo->bufferArray[i].phBuffer, pInputInfo->pMetadata[i]);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::SelectFixedFrameOrder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::SelectFixedFrameOrder(
    UINT* pFrameNumber,
    UINT  totalNumerOfFrames)
{
    CDKResult result = CDKResultSuccess;

    switch (totalNumerOfFrames)
    {
        case 5:
            pFrameNumber[4] = 4;
            // fall-thru
        case 4:
            pFrameNumber[3] = 3;
            // fall-thru
        case 3:
            pFrameNumber[2] = 2;
            pFrameNumber[1] = 1;
            pFrameNumber[0] = 0;
            break;
        default:
            CHX_LOG_ERROR("MFNR Total Number of Frames %d in NOT in the range [3..5]", totalNumerOfFrames);
            result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::SelectNonFixedFrameOrder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::SelectNonFixedFrameOrder(
    UINT* pFrameNumber,
    UINT  totalNumerOfFrames)
{
    CDKResult result = CDKResultSuccess;

    CHX_ASSERT(NULL != pFrameNumber);
    CHX_ASSERT(0    != totalNumerOfFrames);

    // While populating the anchor frame selection data
    // - m_numOfImagesCaptured: will be set, and
    // - m_processingOrder[0] : will have anchor frame index

    m_numOfImagesCaptured = 0;

    result = PopulateAnchorFrameSelectionData();
    if (CDKResultSuccess != result)
    {
        CHX_LOG_ERROR("Failed in populating anchor frame selection data");
    }

    return result;
}

CDKResult FeatureMFNR::SelectMFNRAnchorFrameAndFrameOrder(
    MFNRInputInfo*                 mfnrInputInfo,               // MFNR Input Info.
    camera3_stream_buffer_t*       selectionInputBuffer,        // Out: MFNR Anchor Selection Buffer
    size_t                         selectionInputBufferSize,    // In:  # of MFNR Anchor Selection Buffer
    camera_metadata_t**            ppMergedMetadata,            // Out: Merged Metadata of the Anchor Frame
    AnchorFrameSelectionAlgorithm* anchorSelectionAlgorithm)    // Out: MFNR Anchor Selection Algorithm Option
{
    // Also uses member field m_processingOrder

    CDKResult result = CDKResultSuccess;

    CHX_ASSERT(NULL != mfnrInputInfo);
    CHX_ASSERT(NULL != selectionInputBuffer);
    CHX_ASSERT(0    != selectionInputBufferSize);
    CHX_ASSERT(NULL != anchorSelectionAlgorithm);

    *anchorSelectionAlgorithm = static_cast<AnchorFrameSelectionAlgorithm>(
                                    ExtensionModule::GetInstance()->EnableMFNRAnchorSelectionAlgorithm());

    switch (*anchorSelectionAlgorithm)
    {
        case AnchorFrameSelectionAlgorithm::None:
        {
            UINT32 zslInputFrameIndex = 0;

            // anchor frame as input
            ChxUtils::Memcpy(&selectionInputBuffer[zslInputFrameIndex],
                             &(mfnrInputInfo->bufferArray[zslInputFrameIndex]),
                             selectionInputBufferSize);

            break;
        }
        case AnchorFrameSelectionAlgorithm::Fixed:
        {
            UINT32 zslInputFrameIndex = 0;

            ChxUtils::Memset(m_processingOrder, 0, sizeof(m_processingOrder));
            CHX_ASSERT(m_mfnrTotalNumFrames <= (sizeof(m_processingOrder) / sizeof(m_processingOrder[0])));

            result = SelectFixedFrameOrder(&m_processingOrder[0], m_mfnrTotalNumFrames);
            if (CDKResultSuccess != result)
            {
                CHX_LOG_ERROR("Failed in fixed anchor frame selection and frame order");
                result = CDKResultEFailed;
            }

            zslInputFrameIndex = m_processingOrder[0];

            // anchor frame as input
            ChxUtils::Memcpy(&selectionInputBuffer[0],
                             &(mfnrInputInfo->bufferArray[zslInputFrameIndex]),
                             selectionInputBufferSize);

            break;
        }
        case AnchorFrameSelectionAlgorithm::NonFixed:
        {
            UINT32 zslInputFrameIndex = 0;

            ChxUtils::Memset(m_processingOrder, 0, sizeof(m_processingOrder));
            CHX_ASSERT(m_mfnrTotalNumFrames <= (sizeof(m_processingOrder) / sizeof(m_processingOrder[0])));

            result = SelectNonFixedFrameOrder(&m_processingOrder[0], m_mfnrTotalNumFrames);
            if (CDKResultSuccess != result)
            {
                CHX_LOG_ERROR("Failed in non-fixed anchor frame selection and frame order");
                result = CDKResultEFailed;
            }

            // RDI ZSL queue source buffer selection logic - implement any source frame picking logic here
            zslInputFrameIndex = m_processingOrder[0] % BufferQueueDepth;

            /// @todo (CAMX-4025) - Need to return the complete set of input buffers [0..{3|5}] order?!!

            // anchor frame as input from the ZSL queue and associated metadata (if requested)
            ChxUtils::Memcpy(&selectionInputBuffer[0],
                             &(mfnrInputInfo->bufferArray[zslInputFrameIndex]),
                             selectionInputBufferSize);

            if (NULL != ppMergedMetadata)
            {
                *ppMergedMetadata = reinterpret_cast<camera_metadata_t*>(
                                        &(mfnrInputInfo->pMetadata[zslInputFrameIndex]));
            }

            break;
        }
        default: // Unknown/Invalid option
        {
            CHX_LOG_ERROR("Unknown/Invalid anchor frame selection and frame order option");
            result = CDKResultEFailed;

            break;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::SubmitOfflineMfnrRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::SubmitOfflineMfnrRequest(
    UINT32                     appFrameNumber,
    camera3_capture_request_t* pRequest,
    UINT32                     rtPipelineReqId)
{
    (VOID)rtPipelineReqId;

    CDKResult                     result                   = CDKResultSuccess;
    camera3_stream_buffer_t       inputBuffers[8]          = { { 0 } };
    camera3_stream_buffer_t       outputBuffers[8]         = { { 0 } };
    camera_metadata_t*            pMergedMetadata          = NULL;
    UINT32                        numstageInputs           = 0;
    UINT32                        numStageOutputs          = 0;
    UINT                          outputIndex              = 0;
    UINT                          inputIndex               = 0;
    VOID*                         pRequestMetadata         = NULL;
    Session*                      pOfflineSession          = NULL;
    CameraType                    type                     = CameraType::Wide;
    MFNRInputInfo                 mfnrInputInfo            = { };
    AnchorFrameSelectionAlgorithm anchorSelectionAlgorithm = AnchorFrameSelectionAlgorithm::None;
    UINT32                        feature2Mode;

    CreateMFNRInputInfo(&mfnrInputInfo, pRequest);

    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        if (0 != m_activeCameraId)
        {
            type = CameraType::Tele;
        }
    }

    if (NULL != m_pPerfLockManager)
    {
        // Re-acquire the MFNR perf lock for 1 sec
        m_pPerfLockManager->ReleasePerfLock(PERF_LOCK_MFNR_SNAPSHOT);
        m_pPerfLockManager->AcquirePerfLock(PERF_LOCK_MFNR_SNAPSHOT, 1000);
    }

    CHX_LOG("--------------------------- Mfnr pre-filter stage -------------------------- ");
    numstageInputs                   = 1;
    numStageOutputs                  = 3;
    m_remainingPrefilterStageResults = 1;

    m_numExpectedStageBuffers        = numStageOutputs;
    outputIndex                      = 0;
    inputIndex                       = 0;
    ChxUtils::Memset(&m_preFilterStageResult, 0, sizeof(m_preFilterStageResult));

    result = SelectMFNRAnchorFrameAndFrameOrder(&mfnrInputInfo,
                 inputBuffers,
                 sizeof(camera3_stream_buffer_t),
                 NULL,
                 &anchorSelectionAlgorithm);
    CHX_ASSERT(CDKResultSuccess != result);

    if ((NULL != m_pApplicationInputMeta) && (NULL != mfnrInputInfo.pMetadata[0]))
    {
        UINT8      preFilterFlashState = 0;
        CHITAGSOPS tagOps              = { 0 };

        g_chiContextOps.pTagOps(&tagOps);

        // FlashState needs to be stored at prefilter stage
        // and used during snapshot request
        tagOps.pGetMetaData((CHIHANDLE)mfnrInputInfo.pMetadata[0],
                             ANDROID_FLASH_STATE,
                             &preFilterFlashState,
                             sizeof(preFilterFlashState));
        tagOps.pSetMetaData((CHIHANDLE)m_pApplicationInputMeta,
                             ANDROID_FLASH_STATE,
                            &preFilterFlashState,
                             sizeof(preFilterFlashState));
    }

    ChxUtils::ResetMetadata(m_pInterStageMetadata);
    if (NULL != m_pApplicationInputMeta)
    {
        ChxUtils::MergeMetadata(m_pApplicationInputMeta, m_pInterStageMetadata);
    }
    ChxUtils::MergeMetadata(mfnrInputInfo.pMetadata[0], m_pInterStageMetadata);

    GetOutputBuffer(m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceFull],
        m_pBlendOutStream[type][MfnrReferenceFull],
        &outputBuffers[0]);

    GetOutputBuffer(m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceDS4],
        m_pBlendOutStream[type][MfnrReferenceDS4],
        &outputBuffers[1]);

    GetOutputBuffer(m_pMfnrBpsRegOutBufferManager[type],
        m_pMfnrBpsRegOutStream[type],
        &outputBuffers[2]);

    feature2Mode = static_cast<UINT32>(ChiModeFeature2SubModeType::MFNRBlend);
    ChxUtils::FillTuningModeData(m_pInterStageMetadata,
        pRequest,
        m_sensorModeIndex,
        m_pUsecase->GetEffectMode(),
        m_pUsecase->GetSceneMode(),
        m_pUsecase->GetFeature1Mode(),
        &feature2Mode);

    // Debug-data deep-copy
    if (TRUE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(m_pInterStageMetadata), DebugDataTag))
    {
        CHAR* pData = NULL;
        ChxUtils::GetVendorTagValue((VOID*)(m_pInterStageMetadata), DebugDataTag, (VOID**)&pData);
        DebugData* pDebug = reinterpret_cast<DebugData*>(pData);

        if ((NULL != pDebug->pData) && (0 < pDebug->size))
        {
            CHX_LOG("Mfnr pre-filter stage:RT: pData: %p, size: %zu", pDebug->pData, pDebug->size);
            // Allocate debug-data for offline processing
            if (NULL == m_debugDataOffline.pData)
            {
                m_debugDataOffline.pData = CHX_CALLOC(pDebug->size);

                if (NULL != m_debugDataOffline.pData)
                {
                    m_debugDataOfflineSnapshot.pData = CHX_CALLOC(pDebug->size);
                    if (NULL != m_debugDataOfflineSnapshot.pData)
                    {
                        // Success allocating both offline debug-data
                        m_debugDataOffline.size             = pDebug->size;
                        m_debugDataOfflineSnapshot.size     = pDebug->size;
                    }
                    else
                    {
                        CHX_FREE(m_debugDataOffline.pData);
                        m_debugDataOffline.pData            = NULL;
                        m_debugDataOffline.size             = 0;
                    }
                }

            }
            else if (pDebug->size != m_debugDataOffline.size)
            {
                // This condition is only for protection in case debug-data size is change in run time while camera is
                // already processing. This is not allow for this property at this time.
                CHX_FREE(m_debugDataOffline.pData);
                m_debugDataOffline.pData    = NULL;
                m_debugDataOffline.size     = 0;
                CHX_FREE(m_debugDataOfflineSnapshot.pData);
                m_debugDataOfflineSnapshot.pData    = NULL;
                m_debugDataOfflineSnapshot.size     = 0;

                m_debugDataOffline.pData = CHX_CALLOC(pDebug->size);
                if (NULL != m_debugDataOffline.pData)
                {
                    m_debugDataOfflineSnapshot.pData = CHX_CALLOC(pDebug->size);
                    if (NULL != m_debugDataOfflineSnapshot.pData)
                    {
                        // Success allocating both offline debug-data
                        m_debugDataOffline.size             = pDebug->size;
                        m_debugDataOfflineSnapshot.size     = pDebug->size;
                    }
                    else
                    {
                        CHX_FREE(m_debugDataOffline.pData);
                        m_debugDataOffline.pData            = NULL;
                        m_debugDataOffline.size             = 0;
                    }
                }

            }

            if (NULL != m_debugDataOffline.pData)
            {
                ChxUtils::Memcpy(m_debugDataOffline.pData, pDebug->pData, pDebug->size);

                result = ChxUtils::SetVendorTagValue(m_pInterStageMetadata,
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

    PublishMFNRTotalFrames(m_pInterStageMetadata);
    m_resultsAvailable = FALSE;

    ExecuteMfnrRequest(MfnrStagePrefilter,
        appFrameNumber,
        numStageOutputs,
        &outputBuffers[0],
        numstageInputs,
        &inputBuffers[0],
        m_pInterStageMetadata);

    m_pMfnrResultMutex->Lock();

    // Wait for all prefilter results to come back - 3 output buffers: Full, DS4, BpsRegOut and Metadata
    while (FALSE == m_resultsAvailable)
    {
        m_pMfnrResultAvailable->Wait(m_pMfnrResultMutex->GetNativeHandle());
    }

    if (TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aPauseInProgress)))
    {
        m_resultsAvailable = FALSE;
        m_pMfnrResultMutex->Unlock();
        CHX_LOG_INFO("MFNR blend process return because of cleanup");
        return CDKResultSuccess;
    }

    if (FALSE == m_pUsecase->IsMultiCameraUsecase())
    {
        m_pRdiTargetBuffer->pBufferManager->ReleaseReference(
            reinterpret_cast<buffer_handle_t*>(mfnrInputInfo.bufferArray[0].phBuffer));
    }
    else
    {
        RequestMapInfo requestInfo = m_pUsecase->GetRequestMapInfo(pRequest->frame_number);
        UINT32  pipelineID         = requestInfo.activePipelineID;
        m_pUsecase->ReleaseRDIBufferReference(pipelineID,
                                              reinterpret_cast<buffer_handle_t*>(mfnrInputInfo.bufferArray[0].phBuffer));
    }

    m_resultsAvailable = FALSE;

    m_pMfnrResultMutex->Unlock();

    // Deactivate pre-filter pipeline
    CHX_LOG("Deactivate MfnrStagePrefilter - DeactivateModeReleaseBuffer");

    pOfflineSession = m_pUsecase->GetSessionData(m_offline)->pSession;

    result = ExtensionModule::GetInstance()->DeactivatePipeline(pOfflineSession->GetSessionHandle(),
        pOfflineSession->GetPipelineHandle(GetPipelineIndex(MfnrStagePrefilter)), CHIDeactivateModeReleaseBuffer);

    if (CDKResultSuccess != result)
    {
        CHX_LOG_ERROR("Deactivate MfnrStagePrefilter failed!");
    }

    for (UINT i = 0; i < (m_mfnrTotalNumFrames - 2); i++)
    {
        CHX_LOG("------------------------------- Mfnr blend stage ----------------------------");

        numstageInputs               = 5;               // Rdi, BpsRegOut, PreFilter Full/DS4/DS4
        numStageOutputs              = 2;               // Full, DS4
        m_numExpectedStageBuffers    = numStageOutputs; // Full, DS4
        outputIndex                  = 0;
        inputIndex                   = 0;

        m_remainingBlendStageResults = 1;

        ChxUtils::Memset(&m_blendStageResult, 0, sizeof(m_blendStageResult));

        if ((AnchorFrameSelectionAlgorithm::Fixed == anchorSelectionAlgorithm)  ||
            (AnchorFrameSelectionAlgorithm::NonFixed == anchorSelectionAlgorithm))
        {
            ChxUtils::Memcpy(&inputBuffers[inputIndex++], &mfnrInputInfo.bufferArray[m_processingOrder[i + 1]],
                sizeof(camera3_stream_buffer_t));
        }
        else if (AnchorFrameSelectionAlgorithm::None == anchorSelectionAlgorithm)
        {
            ChxUtils::Memcpy(&inputBuffers[inputIndex++], &mfnrInputInfo.bufferArray[i + 1],
                sizeof(camera3_stream_buffer_t));
        }

        ChxUtils::ResetMetadata(m_pInterStageMetadata);
        ChxUtils::MergeMetadata(m_pApplicationInputMeta, m_pInterStageMetadata);

        if ((AnchorFrameSelectionAlgorithm::Fixed == anchorSelectionAlgorithm)  ||
            (AnchorFrameSelectionAlgorithm::NonFixed == anchorSelectionAlgorithm))
        {
            ChxUtils::MergeMetadata(mfnrInputInfo.pMetadata[m_processingOrder[i + 1]], m_pInterStageMetadata);
        }
        else if (AnchorFrameSelectionAlgorithm::None == anchorSelectionAlgorithm)
        {
            ChxUtils::MergeMetadata(mfnrInputInfo.pMetadata[i + 1], m_pInterStageMetadata);
        }

        ChxUtils::Memcpy(&inputBuffers[inputIndex], &m_bpsRegResultBuffer, sizeof(camera3_stream_buffer_t));
        inputBuffers[inputIndex].acquire_fence = -1;
        inputBuffers[inputIndex++].stream      = (camera3_stream_t *)m_pMfnrBpsRegInStream[type];

        ChxUtils::Memcpy(&inputBuffers[inputIndex],
            &m_preFilterStageResult.refOutputBuffer[MfnrReferenceFull],
            sizeof(camera3_stream_buffer_t));
        inputBuffers[inputIndex].acquire_fence = -1;
        inputBuffers[inputIndex++].stream      = (camera3_stream_t *)m_pBlendInStream[type][MfnrReferenceFull];

        ChxUtils::Memcpy(&inputBuffers[inputIndex],
            &m_preFilterStageResult.refOutputBuffer[MfnrReferenceDS4],
            sizeof(camera3_stream_buffer_t));
        inputBuffers[inputIndex].acquire_fence = -1;
        inputBuffers[inputIndex++].stream      = (camera3_stream_t *)m_pBlendInStream[type][MfnrReferenceDS4];

        ChxUtils::Memcpy(&inputBuffers[inputIndex],
            &m_preFilterStageResult.refOutputBuffer[MfnrReferenceDS4],
            sizeof(camera3_stream_buffer_t));
        inputBuffers[inputIndex].acquire_fence = -1;
        inputBuffers[inputIndex++].stream = (camera3_stream_t *)m_pScaleInStream[type][MfnrReferenceDS4];

        GetOutputBuffer(m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceFull],
            m_pBlendOutStream[type][MfnrReferenceFull],
            &outputBuffers[outputIndex++]);

        GetOutputBuffer(m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceDS4],
            m_pBlendOutStream[type][MfnrReferenceDS4],
            &outputBuffers[outputIndex++]);

        feature2Mode = static_cast<UINT32>(ChiModeFeature2SubModeType::MFNRBlend);
        ChxUtils::FillTuningModeData(m_pInterStageMetadata,
            pRequest,
            m_sensorModeIndex,
            m_pUsecase->GetEffectMode(),
            m_pUsecase->GetSceneMode(),
            m_pUsecase->GetFeature1Mode(),
            &feature2Mode);

        if (TRUE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(m_pInterStageMetadata), DebugDataTag))
        {
            CHAR* pData = NULL;
            ChxUtils::GetVendorTagValue((VOID*)(m_pInterStageMetadata), DebugDataTag, (VOID**)&pData);
            DebugData* pDebug = reinterpret_cast<DebugData*>(pData);

            if ((NULL != pDebug->pData) && (0 < pDebug->size))
            {
                CHX_LOG("Mfnr blend stage:RT: %u: pData: %p, size: %zu", i, pDebug->pData, pDebug->size);
                if ((NULL != m_debugDataOffline.pData)          &&
                    (pDebug->size == m_debugDataOffline.size))
                {
                    ChxUtils::Memcpy(m_debugDataOffline.pData, pDebug->pData, pDebug->size);

                    result = ChxUtils::SetVendorTagValue(m_pInterStageMetadata,
                                                         DebugDataTag,
                                                         sizeof(DebugData),
                                                         &m_debugDataOffline);
                    if (CDKResultSuccess != result)
                    {
                        CHX_LOG_ERROR("DebugDataAll: Fail to set debugdata tag in offline input metadata");
                    }
                }
                else
                {
                    CHX_LOG_ERROR("DebugDataAll: Fail to set offline debug-data");
                }
            }
        }

        PublishMFNRTotalFrames(m_pInterStageMetadata);

        m_resultsAvailable = FALSE;

        ExecuteMfnrRequest(MfnrStageBlend,
            appFrameNumber,
            numStageOutputs,
            &outputBuffers[0],
            numstageInputs,
            &inputBuffers[0],
            m_pInterStageMetadata);


        m_pMfnrResultMutex->Lock();

        // Wait for all post-prefilter scale results to come back - 2 output buffers: DS16, DS64, and Metadata
        while (FALSE == m_resultsAvailable)
        {
            m_pMfnrResultAvailable->Wait(m_pMfnrResultMutex->GetNativeHandle());
        }

        if (TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aPauseInProgress)))
        {
            m_resultsAvailable = FALSE;
            m_pMfnrResultMutex->Unlock();
            CHX_LOG_INFO("MFNR blend scale process return because of cleanup");
            return CDKResultSuccess;
        }

        // Release reference to prefilter input buffers
        m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceFull]->ReleaseReference(
            reinterpret_cast<buffer_handle_t*>(m_preFilterStageResult.refOutputBuffer[MfnrReferenceFull].phBuffer));
        m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceDS4]->ReleaseReference(
            reinterpret_cast<buffer_handle_t*>(m_preFilterStageResult.refOutputBuffer[MfnrReferenceDS4].phBuffer));

        // Copy blend Full and DS4 Results
        ChxUtils::Memcpy(&m_preFilterStageResult.refOutputBuffer[MfnrReferenceFull],
            &m_blendStageResult.refOutputBuffer[MfnrReferenceFull],
            sizeof(CHISTREAMBUFFER));
        ChxUtils::Memcpy(&m_preFilterStageResult.refOutputBuffer[MfnrReferenceDS4],
            &m_blendStageResult.refOutputBuffer[MfnrReferenceDS4],
            sizeof(CHISTREAMBUFFER));

        if (FALSE == m_pUsecase->IsMultiCameraUsecase())
        {
            m_pRdiTargetBuffer->pBufferManager->ReleaseReference(
                reinterpret_cast<buffer_handle_t*>(mfnrInputInfo.bufferArray[i + 1].phBuffer));
        }
        else
        {
            RequestMapInfo requestInfo = m_pUsecase->GetRequestMapInfo(pRequest->frame_number);
            UINT32  pipelineID         = requestInfo.activePipelineID;
            m_pUsecase->ReleaseRDIBufferReference(pipelineID,
                                                  reinterpret_cast<buffer_handle_t*>(mfnrInputInfo.bufferArray[i + 1].phBuffer));
        }

        m_resultsAvailable = FALSE;

        m_pMfnrResultMutex->Unlock();

        if (TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aPauseInProgress)))
       {
            CHX_LOG_INFO("MFNR post scale process return because of cleanup");
           return CDKResultSuccess;
       }
    }

    // Deactivate blend pipeline
    CHX_LOG_INFO("Deactivate MfnrStageBlend - DeactivateModeReleaseBuffer");

    result = ExtensionModule::GetInstance()->DeactivatePipeline(pOfflineSession->GetSessionHandle(),
        pOfflineSession->GetPipelineHandle(GetPipelineIndex(MfnrStageBlend)), CHIDeactivateModeReleaseBuffer);

    if (CDKResultSuccess != result)
    {
        CHX_LOG_ERROR("Deactivate MfnrStageBlend failed!");
    }

    CHX_LOG("------------------------------- Mfnr post-filter stage ----------------------------");

    numstageInputs                    = 5;               // Rdi, BpsRegOut, PreFilter Full/DS4/DS4
    numStageOutputs                   = 1;               // Full
    m_numExpectedStageBuffers         = numStageOutputs; // Full
    outputIndex                       = 0;
    inputIndex                        = 0;

    m_remainingPostFilterStageResults = 1;

    ChxUtils::Memset(&m_postFilterStageResult, 0, sizeof(m_postFilterStageResult));

    if ((AnchorFrameSelectionAlgorithm::Fixed == anchorSelectionAlgorithm)  ||
        (AnchorFrameSelectionAlgorithm::NonFixed == anchorSelectionAlgorithm))
    {
        ChxUtils::Memcpy(&inputBuffers[inputIndex++],
            &mfnrInputInfo.bufferArray[m_processingOrder[m_mfnrTotalNumFrames - 1]],
            sizeof(camera3_stream_buffer_t));
    }
    else if (AnchorFrameSelectionAlgorithm::None == anchorSelectionAlgorithm)
    {
        ChxUtils::Memcpy(&inputBuffers[inputIndex++],
            &mfnrInputInfo.bufferArray[m_mfnrTotalNumFrames - 1],
            sizeof(camera3_stream_buffer_t));
    }

    ChxUtils::ResetMetadata(m_pInterStageMetadata);
    ChxUtils::MergeMetadata(m_pApplicationInputMeta, m_pInterStageMetadata);

    if ((AnchorFrameSelectionAlgorithm::Fixed == anchorSelectionAlgorithm)  ||
        (AnchorFrameSelectionAlgorithm::NonFixed == anchorSelectionAlgorithm))
    {
        ChxUtils::MergeMetadata(mfnrInputInfo.pMetadata[m_processingOrder[m_mfnrTotalNumFrames - 1]],
            m_pInterStageMetadata);
    }
    else if (AnchorFrameSelectionAlgorithm::None == anchorSelectionAlgorithm)
    {
        ChxUtils::MergeMetadata(mfnrInputInfo.pMetadata[m_mfnrTotalNumFrames - 1],
            m_pInterStageMetadata);
    }

    ChxUtils::Memcpy(&inputBuffers[inputIndex], &m_bpsRegResultBuffer, sizeof(camera3_stream_buffer_t));
    inputBuffers[inputIndex].acquire_fence = -1;
    inputBuffers[inputIndex++].stream      = (camera3_stream_t *)m_pMfnrBpsRegInStream[type];

    ChxUtils::Memcpy(&inputBuffers[inputIndex],
        &m_preFilterStageResult.refOutputBuffer[MfnrReferenceFull],
        sizeof(camera3_stream_buffer_t));
    inputBuffers[inputIndex].acquire_fence = -1;
    inputBuffers[inputIndex++].stream      = (camera3_stream_t *)m_pBlendInStream[type][MfnrReferenceFull];

    ChxUtils::Memcpy(&inputBuffers[inputIndex],
        &m_preFilterStageResult.refOutputBuffer[MfnrReferenceDS4],
        sizeof(camera3_stream_buffer_t));
    inputBuffers[inputIndex].acquire_fence = -1;
    inputBuffers[inputIndex++].stream      = (camera3_stream_t *)m_pBlendInStream[type][MfnrReferenceDS4];

    ChxUtils::Memcpy(&inputBuffers[inputIndex],
        &m_preFilterStageResult.refOutputBuffer[MfnrReferenceDS4],
        sizeof(camera3_stream_buffer_t));
    inputBuffers[inputIndex].acquire_fence = -1;
    inputBuffers[inputIndex++].stream = (camera3_stream_t *)m_pScaleInStream[type][MfnrReferenceDS4];

    GetOutputBuffer(m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceFull],
        m_pMfnrPostFilterOutStream[type],
        &outputBuffers[outputIndex++]);

    feature2Mode = static_cast<UINT32>(ChiModeFeature2SubModeType::MFNRPostFilter);
    ChxUtils::FillTuningModeData(m_pInterStageMetadata,
        pRequest,
        m_sensorModeIndex,
        m_pUsecase->GetEffectMode(),
        m_pUsecase->GetSceneMode(),
        m_pUsecase->GetFeature1Mode(),
        &feature2Mode);

    if (TRUE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(m_pInterStageMetadata), DebugDataTag))
    {
        CHAR* pData = NULL;
        ChxUtils::GetVendorTagValue((VOID*)(m_pInterStageMetadata), DebugDataTag, (VOID**)&pData);
        DebugData* pDebug = reinterpret_cast<DebugData*>(pData);

        if ((NULL != pDebug->pData) && (0 < pDebug->size))
        {
            CHX_LOG("Mfnr post-filter stage: pData: %p, size: %zu", pDebug->pData, pDebug->size);
            if ((NULL != m_debugDataOffline.pData)          &&
                (pDebug->size == m_debugDataOffline.size))
            {
                ChxUtils::Memcpy(m_debugDataOffline.pData, pDebug->pData, pDebug->size);
                ChxUtils::Memcpy(m_debugDataOfflineSnapshot.pData, pDebug->pData, pDebug->size);

                result = ChxUtils::SetVendorTagValue(m_pInterStageMetadata,
                                                     DebugDataTag,
                                                     sizeof(DebugData),
                                                     &m_debugDataOffline);
                if (CDKResultSuccess != result)
                {
                    CHX_LOG_ERROR("DebugDataAll: Fail to set debugdata tag in offline input metadata");
                }
            }
            else
            {
                CHX_LOG_ERROR("DebugDataAll: Fail to set offline debug-data");
            }
        }
    }

    PublishMFNRTotalFrames(m_pInterStageMetadata);
    m_resultsAvailable = FALSE;

    ExecuteMfnrRequest(MfnrStagePostfilter,
        appFrameNumber,
        numStageOutputs,
        &outputBuffers[0],
        numstageInputs,
        &inputBuffers[0],
        m_pInterStageMetadata);

    m_pMfnrResultMutex->Lock();

    // Wait for all post-prefilter scale results to come back - 2 output buffers: DS16, DS64, and Metadata
    while (FALSE == m_resultsAvailable)
    {
        m_pMfnrResultAvailable->Wait(m_pMfnrResultMutex->GetNativeHandle());
    }

    if (TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aPauseInProgress)))
    {
        m_resultsAvailable = FALSE;
        m_pMfnrResultMutex->Unlock();
        CHX_LOG_INFO("MFNR snapshot return because of cleanup");
        return CDKResultSuccess;
    }
    // Release reference to prefilter input buffers
    m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceFull]->ReleaseReference(
        reinterpret_cast<buffer_handle_t*>(m_preFilterStageResult.refOutputBuffer[MfnrReferenceFull].phBuffer));
    m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceDS4]->ReleaseReference(
        reinterpret_cast<buffer_handle_t*>(m_preFilterStageResult.refOutputBuffer[MfnrReferenceDS4].phBuffer));
    m_pMfnrBpsRegOutBufferManager[type]->ReleaseReference(
        reinterpret_cast<buffer_handle_t*>(m_bpsRegResultBuffer.phBuffer));

    if (FALSE == m_pUsecase->IsMultiCameraUsecase())
    {
        m_pRdiTargetBuffer->pBufferManager->ReleaseReference(
            reinterpret_cast<buffer_handle_t*>(mfnrInputInfo.bufferArray[m_mfnrTotalNumFrames - 1].phBuffer));
    }
    else
    {
        RequestMapInfo requestInfo = m_pUsecase->GetRequestMapInfo(pRequest->frame_number);
        UINT32  pipelineID         = requestInfo.activePipelineID;
        m_pUsecase->ReleaseRDIBufferReference(pipelineID,
                                              reinterpret_cast<buffer_handle_t*>(mfnrInputInfo.bufferArray[m_mfnrTotalNumFrames - 1].phBuffer));
    }

    m_resultsAvailable = FALSE;

    m_pMfnrResultMutex->Unlock();

    // Deactivate pre-filter pipeline
    CHX_LOG_INFO("Deactivate MfnrStagePostfilter - DeactivateModeReleaseBuffer");

    result = ExtensionModule::GetInstance()->DeactivatePipeline(pOfflineSession->GetSessionHandle(),
        pOfflineSession->GetPipelineHandle(GetPipelineIndex(MfnrStagePostfilter)), CHIDeactivateModeReleaseBuffer);

    if (CDKResultSuccess != result)
    {
        CHX_LOG_ERROR("Deactivate MfnrStagePostfilter failed!");
    }

    CHX_LOG("------------------------------- Mfnr snapshot stage ----------------------------");

    if ((NULL != m_pApplicationInputMeta) && (NULL != m_pInterStageMetadata))
    {
        UINT8      appFlashState = 0;
        CHITAGSOPS tagOps        = { 0 };

        g_chiContextOps.pTagOps(&tagOps);

        // Restore FlashState from prefliter stage
        tagOps.pGetMetaData((CHIHANDLE)m_pApplicationInputMeta,
                             ANDROID_FLASH_STATE,
                             &appFlashState,
                             sizeof(appFlashState));
        tagOps.pSetMetaData((CHIHANDLE)m_pInterStageMetadata,
                             ANDROID_FLASH_STATE,
                             &appFlashState,
                             sizeof(appFlashState));
    }

    if (TRUE == ChxUtils::IsVendorTagPresent(reinterpret_cast<const VOID*>(m_pInterStageMetadata), DebugDataTag))
    {
        CHAR* pData = NULL;
        ChxUtils::GetVendorTagValue((VOID*)(m_pInterStageMetadata), DebugDataTag, (VOID**)&pData);
        DebugData* pDebug = reinterpret_cast<DebugData*>(pData);

        if ((NULL != pDebug->pData) && (0 < pDebug->size))
        {
            CHX_LOG("Mfnr snapshot stage: pData: %p, size: %zu", pDebug->pData, pDebug->size);
            if ((NULL != m_debugDataOfflineSnapshot.pData)          &&
                (pDebug->size == m_debugDataOfflineSnapshot.size))
            {
                // Use offline snapshot data
                result = ChxUtils::SetVendorTagValue(m_pInterStageMetadata,
                                                     DebugDataTag,
                                                     sizeof(DebugData),
                                                     &m_debugDataOfflineSnapshot);
                if (CDKResultSuccess != result)
                {
                    CHX_LOG_ERROR("DebugDataAll: Fail to set debugdata tag in offline input metadata");
                }
            }
            else
            {
                CHX_LOG_ERROR("DebugDataAll: Fail to set offline debug-data");
            }
        }
    }

    numstageInputs                  = 1;
    numStageOutputs                 = 1;
    m_remainingSnapshotStageResults = 1;
    m_numExpectedStageBuffers       = numStageOutputs;
    m_postFilterStageResult.refOutputBuffer[MfnrReferenceFull].pStream = m_pMfnrPostFilterOutStream[type];

    ExecuteMfnrRequest(MfnrStageSnpashot,
        appFrameNumber,
        numStageOutputs,
        &m_snapshotBuffers[m_maxSnapshotReqId % MaxOutstandingRequests][0],
        numstageInputs,
        (camera3_stream_buffer_t *)&m_postFilterStageResult.refOutputBuffer[MfnrReferenceFull],
        m_pInterStageMetadata);

    m_pMfnrResultMutex->Lock();
    // Wait for all snapshot stage results
    while (FALSE == m_resultsAvailable)
    {
        m_pMfnrResultAvailable->Wait(m_pMfnrResultMutex->GetNativeHandle());
    }

    m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceFull]->ReleaseReference(
        reinterpret_cast<buffer_handle_t*>(m_postFilterStageResult.refOutputBuffer[MfnrReferenceFull].phBuffer));

    // Free all MFNR buffers to reduce memory
    m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceFull]->FreeBuffers(TRUE);
    m_pMfnrBufferManager[type][MfnrStagePrefilter][MfnrReferenceDS4]->FreeBuffers(TRUE);
    m_pMfnrBpsRegOutBufferManager[type]->FreeBuffers(TRUE);
    m_resultsAvailable = FALSE;

    m_pMfnrResultMutex->Unlock();
    for (UINT32 i = 0; i < mfnrInputInfo.numOfBuffers; i++)
    {
        ChxUtils::FreeMetaData(mfnrInputInfo.pMetadata[i]);
        mfnrInputInfo.pMetadata[i] = NULL;
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::ExecuteMfnrRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult FeatureMFNR::ExecuteMfnrRequest(
    MFNRStage                pipelineStage,
    UINT32                   frameNumber,
    UINT32                   numOutputs,
    camera3_stream_buffer_t* pOutputBuffers,
    UINT32                   numInputs,
    camera3_stream_buffer_t* pInputBuffers,
    const VOID*              pSettings)

{
    CDKResult          result         = CDKResultSuccess;
    CHIPIPELINEREQUEST submitRequest  = { 0 };
    CHICAPTUREREQUEST  captureRequest = { 0 };
    const Session*     pSession       = m_pUsecase->GetSessionData(m_offline)->pSession;
    PipelineData*      pPipelineData  = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_offline,
                                                                                  GetPipelineIndex(pipelineStage)));
    UINT               requestIdIndex = (pPipelineData->seqId % MaxOutstandingRequests);

    pPipelineData->seqIdToFrameNum[requestIdIndex] = frameNumber;

    captureRequest.frameNumber     = pPipelineData->seqId++;
    captureRequest.hPipelineHandle = reinterpret_cast<CHIPIPELINEHANDLE>(
                                         pSession->GetPipelineHandle(GetPipelineIndex(pipelineStage)));
    captureRequest.numInputs       = numInputs;
    captureRequest.numOutputs      = numOutputs;
    captureRequest.pInputBuffers   = reinterpret_cast<CHISTREAMBUFFER*>(pInputBuffers);
    captureRequest.pOutputBuffers  = reinterpret_cast<CHISTREAMBUFFER*>(pOutputBuffers);
    captureRequest.pMetadata       = pSettings;

    captureRequest.pPrivData       = &m_privData[captureRequest.frameNumber % MaxOutstandingRequests];
    captureRequest.pPrivData->featureType = FeatureType::MFNR;

    submitRequest.pSessionHandle   = reinterpret_cast<CHIHANDLE>(pSession->GetSessionHandle());
    submitRequest.numRequests      = 1;
    submitRequest.pCaptureRequests = &captureRequest;

    CHX_LOG_INFO("Sending MFNR request application-frameNumber: %d, sessionId: %d, session-requestId: %d,",
        frameNumber, m_offline, (UINT)captureRequest.frameNumber);

    if (FALSE == pSession->IsPipelineActive())
    {
        CDKResult result = CDKResultSuccess;

        result = ExtensionModule::GetInstance()->ActivatePipeline(pSession->GetSessionHandle(),
            pSession->GetPipelineHandle(GetPipelineIndex(pipelineStage)));
        if (CDKResultSuccess == result)
        {
            pSession->SetPipelineActivateFlag();
        }
    }
    m_pUsecase->SetRequestToFeatureMapping(m_offline, captureRequest.frameNumber, this);
    result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::ProcessResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::ProcessResult(
    CHICAPTURERESULT*   pResult,
    VOID*               pPrivateCallbackData)
{
    if ((TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aPauseInProgress))) ||
              (TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aFlushInProgress))))
    {
        CHX_LOG_INFO("MFNR process result return because of cleanup");
        return;
    }

    SessionPrivateData* pCbData               = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    BOOL                isAppResultsAvailable = FALSE;
    UINT32              resultFrameNum        = pResult->frameworkFrameNum;
    UINT32              numstageInputs        = 0;
    UINT32              numStageOutputs       = 0;
    UINT32              rtPipelineReqId       = 0;
    CameraType          type                  = CameraType::Wide;

    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        RequestMapInfo requestMapInfo = m_pUsecase->GetRequestMapInfo(m_internalRequestId);
        if (requestMapInfo.masterCameraID != 0)
        {
            type = CameraType::Tele;
        }
        resultFrameNum = requestMapInfo.frameNumber;
    }

    // CHX_LOG("MFNR HW session id: %d", pCbData->sessionId);
    if (m_offline == pCbData->sessionId)
    {
        const PipelineData* pPipelineData = NULL;

        if (m_remainingPrefilterStageResults > 0)
        {
            pPipelineData = m_pUsecase->GetPipelineData(pCbData->sessionId, GetPipelineIndex(MfnrStagePrefilter));
        }
        if (m_remainingBlendStageResults > 0)
        {
            pPipelineData = m_pUsecase->GetPipelineData(pCbData->sessionId, GetPipelineIndex(MfnrStageBlend));
        }
        if (m_remainingPostFilterStageResults > 0)
        {
            pPipelineData = m_pUsecase->GetPipelineData(pCbData->sessionId, GetPipelineIndex(MfnrStagePostfilter));
        }
        if (m_remainingSnapshotStageResults > 0)
        {
            pPipelineData = m_pUsecase->GetPipelineData(pCbData->sessionId, GetPipelineIndex(MfnrStageSnpashot));
        }

        if (NULL != pPipelineData)
        {
            resultFrameNum  = pPipelineData->seqIdToFrameNum[pResult->frameworkFrameNum % MaxOutstandingRequests];
        }
    }
    else if (m_realtime == pCbData->sessionId)
    {
        PipelineData* pPipelineData = const_cast<PipelineData*>(m_pUsecase->GetPipelineData(m_realtime, 0));

        resultFrameNum  = pPipelineData->seqIdToFrameNum[pResult->frameworkFrameNum % MaxOutstandingRequests];
        rtPipelineReqId = pResult->frameworkFrameNum;

        CHX_LOG("Realtime ReqId to AppFrameNum: %d <--> %d", pResult->frameworkFrameNum, resultFrameNum);
    }

    CHX_LOG("ProcessResult resultFrameNum: %d, sessionId: %d, pResultMetadata: %p, numOutputBuffers: %d pResult->fmwkFrNo %d",
            resultFrameNum, pCbData->sessionId, pResult->pResultMetadata, pResult->numOutputBuffers, pResult->frameworkFrameNum);

    UINT32                    resultFrameIndex = resultFrameNum % MaxOutstandingRequests;
    camera3_capture_result_t* pUsecaseResult   = m_pUsecase->GetCaptureResult(resultFrameIndex);

    pUsecaseResult->frame_number = resultFrameNum;

    // If result contain metadata and metadata has not been sent to framework
    if ((NULL != pResult->pResultMetadata))
    {
        BOOL               isResultMetadataAvailable = FALSE;
        UINT64             timestamp                 = m_pUsecase->GetRequestShutterTimestamp(resultFrameNum);
        camera_metadata_t* pMetadata                 =
            const_cast<camera_metadata_t*>(static_cast<const camera_metadata_t*>(pResult->pResultMetadata));

        if (m_realtime == pCbData->sessionId)
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

            m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].frameNumber = resultFrameNum;
            m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isMetadataReady = TRUE;

            if ((TRUE == m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isBufferReady) &&
                (TRUE == m_pRdiTargetBuffer->bufferQueue[zslQueueIndex].isMetadataReady))
            {
                m_pRdiTargetBuffer->lastReadySequenceID = rtPipelineReqId;
            }

            m_pRdiTargetBuffer->pMutex->Unlock();
        }

        if (FALSE == m_pUsecase->IsMetadataSent(resultFrameIndex))
        {
            // Do Not wait for Snapshot frame metadata, Return Preview metadata back to fwk.
            // If we wait for snapshot, and if it takes more time to process, we will block the preview.
            pUsecaseResult->result         = static_cast<const camera_metadata_t*>(pResult->pResultMetadata);
            pUsecaseResult->partial_result = pResult->numPartialMetadata;
            isResultMetadataAvailable      = TRUE;

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
                    isAppResultsAvailable                   = TRUE;
                }
            }
        }
    }

    if (pResult->numOutputBuffers > 0)
    {
        for (UINT i = 0; i < pResult->numOutputBuffers; i++)
        {
            // If our internal stream, copy the result into the target buffer to be consumed by the offline pipeline
            if (m_pRdiStream[type] == pResult->pOutputBuffers[i].pStream)
            {
                UINT32 queueIndex = (rtPipelineReqId % BufferQueueDepth);

                m_pRdiTargetBuffer->pMutex->Lock();

                if (rtPipelineReqId >= BufferQueueDepth)
                {
                    m_pRdiTargetBuffer->pBufferManager->ReleaseReference(
                        reinterpret_cast<buffer_handle_t*>(m_pRdiTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer->phBuffer));
                }

                ChxUtils::Memcpy(m_pRdiTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer,
                                 &pResult->pOutputBuffers[i],
                                 sizeof(CHISTREAMBUFFER));

                m_pRdiTargetBuffer->bufferQueue[queueIndex].pRdiOutputBuffer->acquireFence = -1;
                m_pRdiTargetBuffer->bufferQueue[queueIndex].isBufferReady                  = TRUE;

                if ((TRUE == m_pRdiTargetBuffer->bufferQueue[queueIndex].isBufferReady) &&
                    (TRUE == m_pRdiTargetBuffer->bufferQueue[queueIndex].isMetadataReady))
                {
                    m_pRdiTargetBuffer->lastReadySequenceID = rtPipelineReqId;
                }

                m_pRdiTargetBuffer->pMutex->Unlock();

                if ((TRUE == m_isSnapshotFrame[resultFrameIndex]) &&
                    (TRUE == m_isLLSSnapshot) &&
                    (TRUE == m_triggerMFNRReprocess[rtPipelineReqId % MaxOutstandingRequests]))
                {
                    m_triggerMFNRReprocess[rtPipelineReqId % MaxOutstandingRequests] = FALSE;
                    CHX_LOG("FeatureMFNR LLS signal request thead");

                    m_pRDIResultMutex->Lock();
                    m_allRDIResultsAvaliable = TRUE;
                    m_pRDIResultAvailable->Signal();
                    m_pRDIResultMutex->Unlock();
                }
            }
            else if ((m_remainingPrefilterStageResults > 0) && (m_offline == pCbData->sessionId))
            {   // There will be 3 buffers per prefilter stage results - Full, DS4, BpRegOut
                if (pResult->pOutputBuffers[i].pStream == m_pBlendOutStream[type][MfnrReferenceFull])
                {
                    CHX_LOG_INFO("MFNR-SNAPSHOT: Received FULL output buffer");

                    ChxUtils::Memcpy(&m_preFilterStageResult.refOutputBuffer[MfnrReferenceFull],
                        &pResult->pOutputBuffers[i],
                        sizeof(CHISTREAMBUFFER));
                    m_numExpectedStageBuffers--;

                }
                else if (pResult->pOutputBuffers[i].pStream == m_pBlendOutStream[type][MfnrReferenceDS4])
                {
                    CHX_LOG_INFO("MFNR-SNAPSHOT: Received DS4 output buffer");

                    ChxUtils::Memcpy(&m_preFilterStageResult.refOutputBuffer[MfnrReferenceDS4],
                        &pResult->pOutputBuffers[i],
                        sizeof(CHISTREAMBUFFER));
                    m_numExpectedStageBuffers--;
                }
                else if (pResult->pOutputBuffers[i].pStream == m_pMfnrBpsRegOutStream[type])
                {
                    CHX_LOG_INFO("MFNR-SNAPSHOT: Received BPS Reg output buffer");

                    ChxUtils::Memcpy(&m_bpsRegResultBuffer, &pResult->pOutputBuffers[i], sizeof(CHISTREAMBUFFER));
                    m_numExpectedStageBuffers--;
                }
                if (m_numExpectedStageBuffers == 0)
                {
                    CHX_LOG_INFO("MFNR-SNAPSHOT: All Prefilter stage output buffers received");

                    m_remainingPrefilterStageResults--;
                    m_pMfnrResultMutex->Lock();
                    m_resultsAvailable = TRUE;
                    m_pMfnrResultAvailable->Signal();
                    m_pMfnrResultMutex->Unlock();

                }
            }
            else if ((m_remainingBlendStageResults > 0) && (m_offline == pCbData->sessionId))
            {
                if (pResult->pOutputBuffers[i].pStream == m_pBlendOutStream[type][MfnrReferenceFull])
                {
                    CHX_LOG_INFO("MFNR-SNAPSHOT: Received Blend Full output buffer");

                    ChxUtils::Memcpy(&m_blendStageResult.refOutputBuffer[MfnrReferenceFull],
                        &pResult->pOutputBuffers[i],
                        sizeof(CHISTREAMBUFFER));
                    m_numExpectedStageBuffers--;
                }
                else if (pResult->pOutputBuffers[i].pStream == m_pBlendOutStream[type][MfnrReferenceDS4])
                {
                    CHX_LOG_INFO("MFNR-SNAPSHOT: Received Blend DS4 output buffer");

                    ChxUtils::Memcpy(&m_blendStageResult.refOutputBuffer[MfnrReferenceDS4],
                        &pResult->pOutputBuffers[i],
                        sizeof(CHISTREAMBUFFER));
                    m_numExpectedStageBuffers--;
                }
                if (m_numExpectedStageBuffers == 0)
                {
                    CHX_LOG_INFO("MFNR-SNAPSHOT: All Blend stage output buffers received");
                    m_remainingBlendStageResults--;
                    m_pMfnrResultMutex->Lock();
                    m_resultsAvailable = TRUE;
                    m_pMfnrResultAvailable->Signal();
                    m_pMfnrResultMutex->Unlock();
                }

            }
            else if ((m_remainingPostFilterStageResults > 0) && (m_offline == pCbData->sessionId))
            {
                if (pResult->pOutputBuffers[i].pStream == m_pMfnrPostFilterOutStream[type])
                {
                    CHX_LOG_INFO("MFNR-SNAPSHOT: Received Postfilter Full output buffer");

                    ChxUtils::Memcpy(&m_postFilterStageResult.refOutputBuffer[MfnrReferenceFull],
                        &pResult->pOutputBuffers[0],
                        sizeof(CHISTREAMBUFFER));
                    m_numExpectedStageBuffers--;
                }
                if (m_numExpectedStageBuffers == 0)
                {
                    CHX_LOG_INFO("MFNR-SNAPSHOT: All Postfilter stage output buffers received");
                    m_remainingPostFilterStageResults--;
                    m_pMfnrResultMutex->Lock();
                    m_resultsAvailable = TRUE;
                    m_pMfnrResultAvailable->Signal();
                    m_pMfnrResultMutex->Unlock();
                }
            }
            else if ((m_remainingSnapshotStageResults > 0) && (m_offline == pCbData->sessionId))
            {
                CHX_LOG_INFO("MFNR-SNAPSHOT: Received Snapshot output buffer");
                CHX_LOG_INFO("MFNR-SNAPSHOT: copy snapshot result in output buffer atframe %d",
                    pUsecaseResult->frame_number);

                if (TRUE == m_blockPreviewForSnapshot)
                {
                    m_pSnapshotResultMutex->Lock();
                    m_snapshotResultAvailable = TRUE;
                    m_pSnapshotResultAvailable->Signal();
                    m_pSnapshotResultMutex->Unlock();
                }

                // queue a buffer as part of the normal result
                m_pUsecase->GetAppResultMutex()->Lock();
                camera3_stream_buffer_t* pResultBuffer =
                    const_cast<camera3_stream_buffer_t*>(
                        &pUsecaseResult->output_buffers[pUsecaseResult->num_output_buffers++]);

                ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[i], sizeof(camera3_stream_buffer_t));
                m_pUsecase->GetAppResultMutex()->Unlock();

                isAppResultsAvailable = TRUE;
                m_numExpectedStageBuffers--;
                if (m_numExpectedStageBuffers == 0)
                {
                    CHX_LOG_INFO("MFNR-SNAPSHOT: snapshot stage output buffers received");
                    m_remainingSnapshotStageResults--;
                    m_pMfnrResultMutex->Lock();
                    m_resultsAvailable = TRUE;
                    m_pMfnrResultAvailable->Signal();
                    m_pMfnrResultMutex->Unlock();
                }

            }
            else
            {
                // queue a buffer as part of the normal result
                m_pUsecase->GetAppResultMutex()->Lock();
                camera3_stream_buffer_t* pResultBuffer =
                    const_cast<camera3_stream_buffer_t*>(
                        &pUsecaseResult->output_buffers[pUsecaseResult->num_output_buffers++]);

                ChxUtils::Memcpy(pResultBuffer, &pResult->pOutputBuffers[i], sizeof(camera3_stream_buffer_t));
                m_pUsecase->GetAppResultMutex()->Unlock();

                isAppResultsAvailable = TRUE;
            }

        }
    }

    if (TRUE == isAppResultsAvailable)
    {
        CHX_LOG_ERROR("ProcessAndReturnFinishedResults for frame: %d", pUsecaseResult->frame_number);
        m_pUsecase->ProcessAndReturnFinishedResults();
    }

    if ((1 <= ExtensionModule::GetInstance()->EnableDumpDebugData()) &&
        (NULL != pResult->pResultMetadata)                           &&
        (m_offline == pCbData->sessionId))
    {
        // Process debug-data only for offline processing, this will get only MFNR buffers
        m_pUsecase->ProcessDebugData(pResult, pPrivateCallbackData, resultFrameNum);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FeatureMFNR::ProcessMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::ProcessMessage(
    const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{
    (VOID)pMessageDescriptor;
    (VOID)pPrivateCallbackData;
    //@todo this function to be REMOVED
}

INT32 FeatureMFNR::GetRequiredPipelines(
    AdvancedPipelineType* pPipelines,
    INT32                 size)
{
    INT32 count               = 0;
    const INT32 pipelineCount = 11;

    if (NULL != pPipelines && size >= pipelineCount)
    {
        AdvancedPipelineType pipelineGroup[pipelineCount];
        UINT                 cameraId[pipelineCount];
        UINT                 wideCameraId       = m_pUsecase->GetPhysicalCameraId(CameraRoleTypeWide);
        INT32                pipelineGroupIndex = 0;
        INT32                index              = 0;

        pPipelines[index]                   = AdvancedPipelineType::InternalZSLYuv2JpegMFNRType;
        cameraId[pipelineGroupIndex]        = wideCameraId;
        pipelineGroup[pipelineGroupIndex++] = pPipelines[index];
        index++;

        pPipelines[index]                   = AdvancedPipelineType::MFNRPrefilterType;
        cameraId[pipelineGroupIndex]        = wideCameraId;
        pipelineGroup[pipelineGroupIndex++] = pPipelines[index];
        index++;

        pPipelines[index]                   = AdvancedPipelineType::MFNRBlendType;
        cameraId[pipelineGroupIndex]        = wideCameraId;
        pipelineGroup[pipelineGroupIndex++] = pPipelines[index];
        index++;

        pPipelines[index]                   = AdvancedPipelineType::MFNRPostFilterType;
        cameraId[pipelineGroupIndex]        = wideCameraId;
        pipelineGroup[pipelineGroupIndex++] = pPipelines[index];
        index++;

        if (TRUE == m_pUsecase->IsMultiCameraUsecase())
        {
            UINT teleCameraId = m_pUsecase->GetPhysicalCameraId(CameraRoleTypeTele);

            pPipelines[index]                   = AdvancedPipelineType::InternalZSLYuv2JpegMFNRAuxType;
            cameraId[pipelineGroupIndex]        = teleCameraId;
            pipelineGroup[pipelineGroupIndex++] = pPipelines[index];
            index++;

            pPipelines[index] = AdvancedPipelineType::MFNRPrefilterAuxType;
            cameraId[pipelineGroupIndex]        = teleCameraId;
            pipelineGroup[pipelineGroupIndex++] = pPipelines[index];
            index++;

            pPipelines[index] = AdvancedPipelineType::MFNRBlendAuxType;
            cameraId[pipelineGroupIndex]        = teleCameraId;
            pipelineGroup[pipelineGroupIndex++] = pPipelines[index];
            index++;

            pPipelines[index] = AdvancedPipelineType::MFNRPostFilterAuxType;
            cameraId[pipelineGroupIndex]        = teleCameraId;
            pipelineGroup[pipelineGroupIndex++] = pPipelines[index];
            index++;

        }

        // group the offline session together and get the session id
        m_offline          = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], pipelineGroupIndex);
        m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], pipelineGroupIndex);
        pipelineGroupIndex = 0;

        if (FALSE == m_pUsecase->IsMultiCameraUsecase())
        {
            pPipelines[index]                   = AdvancedPipelineType::ZSLPreviewRawType;
            cameraId[pipelineGroupIndex]        = wideCameraId;
            pipelineGroup[pipelineGroupIndex++] = pPipelines[index];
            // get the preview session id
            m_realtime          = m_pUsecase->GetUniqueSessionId(&pipelineGroup[0], pipelineGroupIndex);
            m_pUsecase->SetPipelineCameraId(&pipelineGroup[0], &cameraId[0], pipelineGroupIndex);
            pipelineGroupIndex  = 0;
            index++;
        }
        else
        {
            m_realtime = 0xFFFFFFFF;
            CHX_LOG("Don't use Realtime pipeline in advance usecase for multicamera usecase");
        }

        count = index;
    }

    CHX_LOG("FeatureMFNR::GetRequiredPipelines, required pipeline count:%d", count);

    CHX_ASSERT(pipelineCount == count);
    return count;
}

UINT FeatureMFNR::GetPipelineIndex(
    MFNRStage pipelineStage)
{
    UINT index = pipelineStage;

    if (TRUE == m_pUsecase->IsMultiCameraUsecase())
    {
        if (m_activeCameraId != 0)
        {
            index = MfnrStageMax + pipelineStage;
        }

    }

    CHX_LOG("pipeline index %d for stage %d", index, pipelineStage);

    return index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseZSLCommonReprocess::RequestThread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* FeatureMFNR::RequestThread(
    VOID* pThreadData)
{
    PerThreadData* pPerThreadData = reinterpret_cast<PerThreadData*>(pThreadData);
    FeatureMFNR*   pCameraUsecase = reinterpret_cast<FeatureMFNR*>(pPerThreadData->pPrivateData);

    pCameraUsecase->RequestThreadProcessing();

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FeatureMfnr::RequestThreadProcessing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID FeatureMFNR::RequestThreadProcessing()
{
    UINT32 index            = 0;
    UINT32 rtPipelineReqId  = 0;

    CHX_LOG_INFO("MFNR Request Thread Processing Entered");


    while (TRUE)
    {
        m_pOfflineRequestMutex->Lock();

        /// @todo (CAMX-4025): Code below appeared to handle 2|3 aspects of the problem
        ///                    at the same time.
        ///
        ///                    1. Termination of a thread upon receiving a signal after
        ///                       setting m_offlineRequestProcessTerminate to TRUE
        ///                    2. Processing of requests while termination of thread is
        ///                       not yet requested
        ///                    3. Skip processing of frames, when thread termination is
        ///                       requested
        ///
        ///                    However it, does not handle spurious wakeups allowed for
        ///                    wait on conditional variables, also skips wait() thus it
        ///                    would be lead to the incorrrect invocation of the method
        ///                    SubmitOfflineMfnrRequest() with possible junk data.
        ///
        ///                    Instead it should wait for an event in a loop, until the
        ///                    event signal is ready positively.  i.e., signal() should
        ///                    match a wait() in progress.
        if (FALSE == m_offlineRequestProcessTerminate)
        {
            m_pOfflineRequestAvailable->Wait(m_pOfflineRequestMutex->GetNativeHandle());
        }

        // m_offlineRequestProcessTerminate would be set to TRUE while waiting
        if (TRUE == m_offlineRequestProcessTerminate)
        {
            m_pOfflineRequestMutex->Unlock();
            break;
        }

        /// @todo (CAMX-4025): Code below has to process  MFNR requests in a while loop
        ///                    as long as there are pending frames, then wait for a new
        ///                    signal at the beginning of our loop; i.e., while (TRUE).
        rtPipelineReqId = 0;
        SubmitOfflineMfnrRequest(m_requestFrameNumber, &m_captureRequest, rtPipelineReqId);
        m_maxSnapshotReqId++;

        m_pOfflineRequestMutex->Unlock();
    }

    CHX_LOG_INFO("MFNR Thread Exited");
}

UINT FeatureMFNR::GetMFNRTotalFramesByGain(
    camera_metadata_t* pMeta)
{
    UINT       totalMFNRNumFrames      = MfnrDefaultInputFrames;
    UINT32     metadataAECFrameControl = 0;
    CHITAGSOPS tagOps                  = { 0 };

    g_chiContextOps.pTagOps(&tagOps);

    CDKResult result = tagOps.pQueryVendorTagLocation("org.quic.camera2.statsconfigs",
                                                      "AECFrameControl",
                                                      &metadataAECFrameControl);
    if (CDKResultSuccess == result)
    {
        AECFrameControl frameCtrl = { };

        result = tagOps.pGetMetaData(pMeta,
                                     metadataAECFrameControl,
                                     &frameCtrl,
                                     sizeof(AECFrameControl));

        if (CDKResultSuccess == result)
        {
            FLOAT realGain = frameCtrl.exposureInfo[ExposureIndexSafe].linearGain;

            CHX_LOG_INFO("#2 AEC Gain received = %f", realGain);

            // need update from latest tuning xml file
            if (realGain <= 6.5f)
            {
                totalMFNRNumFrames = 3;
            }
            else if (realGain <= 8.0f)
            {
                totalMFNRNumFrames = 4;
            }
            else if (realGain <= 10.f)
            {
                totalMFNRNumFrames = 5;
            }
            else if (realGain <= 15.0f)
            {
                totalMFNRNumFrames = 6;
            }
            else if (realGain <= 40.0f)
            {
                totalMFNRNumFrames = 7;
            }
            else if (realGain > 50.0f)
            {
                totalMFNRNumFrames = 8;
            }
        }
        else
        {
            CHX_LOG_WARN("Cannot get AEC frame info from metadata");
        }

        CHX_LOG_INFO("Total number of MFNR Frames = %d", totalMFNRNumFrames);
    }
    else
    {
        CHX_LOG_WARN("Not able to obtain AEC gain for calculation of Total MFNR frames");
    }

    return totalMFNRNumFrames;
}

CDKResult FeatureMFNR::PublishMFNRTotalFrames(
    camera_metadata_t* pMeta)
{
    UINT32     metadataMFNRTotalNumFrames = 0;
    CHITAGSOPS tagOps                     = { 0 };

    g_chiContextOps.pTagOps(&tagOps);

    CDKResult result = tagOps.pQueryVendorTagLocation("org.quic.camera2.mfnrconfigs",
                                                      "MFNRTotalNumFrames",
                                                      &metadataMFNRTotalNumFrames);

    if (CDKResultSuccess == result)
    {
        result = tagOps.pSetMetaData(pMeta,
                                     metadataMFNRTotalNumFrames,
                                     &m_mfnrTotalNumFrames,
                                     sizeof(UINT));
        if (CDKResultSuccess != result)
        {
            CHX_LOG_WARN("Cannot set MFNR Total Number of Frames info into metadata");
        }
    }
    else
    {
        CHX_LOG_WARN("Cannot query MFNR vendor tag info from metadata");
    }

    return result;
}
