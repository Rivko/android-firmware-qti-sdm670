////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodesvhdr.cpp
/// @brief Chi node for DummyShdr
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <inttypes.h>
#include <system/camera_metadata.h>

#include "chiispstatsdefs.h"
#include "camxchinodeshdr.h"
#include <tuningsetmanager.h>
#include "chiaecinterface.h"
#include <chituningmodeparam.h>
#include "parametertuningtypes.h"
#include "parametertypes.h"

// NOWHINE FILE CP040: Keyword new not allowed. Use the CAMX_NEW/CAMX_DELETE functions insteads
// NOWHINE FILE NC008: Warning: - Var names should be lower camel case

#undef LOG_TAG
#define LOG_TAG "CHISVHDR"

#define PRINT_METADATA TRUE
#define INTERPOLATE_GAMMA_RATIO(x, y, z) \
    (((z) - (x)) / ((y) - (x)))

#define LINEAR_INTERPOLATION(v1, v2, ratio) \
    ((v2) + ((ratio) * ((v1) - (v2))))

#define DEFAULT_EXP_RATIO 6.25f
#define EXP_RATIO_50HZ 5.0f
#define EXP_RATIO_60HZ 6.25f

ChiNodeInterface g_ChiNodeInterface;    ///< The instance save the CAMX Chi interface
UINT32           g_vendorTagBase = 0;   ///< Chi assigned runtime vendor tag base for the node

const CHAR* pLibName = "libqmmf_alg_svhdr";

#if defined (_LINUX)
const CHAR* pLibPath = "/vendor/lib/qmmf/alg-plugins/";
const CHAR* pFileDumpPath = "/data/misc/camera/";
#endif // _LINUX

#if defined (_WIN32)
const CHAR* pLibPath = ".\\";
const CHAR* pFileDumpPath = ".\\";
#endif // _WIN32

/// @todo (CAMX-1854) the major / minor version shall get from CHI
static const UINT32 ChiNodeMajorVersion             = 0;                         ///< The major version of CHI interface
static const UINT32 ChiNodeMinorVersion             = 0;                         ///< The minor version of CHI interface

static const UINT32 ChiNodeCapsSvhdr                = ChiNodeCapsSHDR;           ///< Simply do SVHDR operation

static const CHAR   DummyShdrNodeSectionName[]      = "com.qti.node.shdr";       ///< The section name for node

static const UINT32 DummyShdrNodeTagBase            = 0;                         ///< Tag base
static const UINT32 DummyShdrNodeTagBEStats         = DummyShdrNodeTagBase + 0;  ///< Tag to send BE stats
static const UINT32 DummyShdrNodeTagBHISTStats      = DummyShdrNodeTagBase + 1;  ///< Tag to send BHIST stats
static const UINT32 DummyShdrNodeTagBGBEconfigStats = DummyShdrNodeTagBase + 2;  ///< Tag to send BGBEconfig For stats

///< Supported vendor tag list, it shall align with the definition in g_VendorTagSectionDummyShdr
static const UINT32 g_VendorTagList[] =
{
    DummyShdrNodeTagBEStats,
    DummyShdrNodeTagBHISTStats,
    DummyShdrNodeTagBGBEconfigStats
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagSectionDummyShdr[] =
{
    { "SHDRBEStats",         TYPE_BYTE, sizeof(ParsedHDRBEStatsOutput) },
    { "SHDRBHISTStats",      TYPE_BYTE, sizeof(ParsedBHistStatsOutput) },
    { "SHDRBGBEconfigStats", TYPE_BYTE, sizeof(ISPAWBBGStatsConfig) }
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGSECTIONDATA g_VendorTagDummyShdrSection[] =
{
    {
        DummyShdrNodeSectionName,  0,
        sizeof(g_VendorTagSectionDummyShdr) / sizeof(g_VendorTagSectionDummyShdr[0]), g_VendorTagSectionDummyShdr,
        CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    }
};

///< This is an array of all vendor tag section data
static ChiVendorTagInfo g_VendorTagInfoDummyShdr[] =
{
    {
        &g_VendorTagDummyShdrSection[0],
        sizeof(g_VendorTagDummyShdrSection) / sizeof(g_VendorTagDummyShdrSection[0])
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummyShdrNodeGetCaps
///
/// @brief  Implementation of PFNNODEGETCAPS defined in chinode.h
///
/// @param  pCapsInfo   Pointer to a structure that defines the capabilities supported by the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyShdrNodeGetCaps(
    CHINODECAPSINFO* pCapsInfo)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pCapsInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pCapsInfo is NULL");
    }

    if (CDKResultSuccess == result)
    {
        if (pCapsInfo->size >= sizeof(CHINODECAPSINFO))
        {
            pCapsInfo->nodeCapsMask = ChiNodeCapsSvhdr;
            // max(4K) size supported by SHDR lib with 30 fps. SHDR sensor mode may include
            // balanking as well hence interface to provide output resolution of lib
            pCapsInfo->width        = 3840;
            pCapsInfo->height       = 2160;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODECAPSINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummyShdrNodeQueryVendorTag
///
/// @brief  Implementation of PFNCHIQUERYVENDORTAG defined in chinode.h
///
/// @param  pQueryVendorTag Pointer to a structure that returns the exported vendor tag
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyShdrNodeQueryVendorTag(
    CHIQUERYVENDORTAG* pQueryVendorTag)
{
    CDKResult result = CDKResultSuccess;
    if (NULL == pQueryVendorTag)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pQueryVendorTag is NULL");
    }

    if (CDKResultSuccess == result)
    {
        result = ChiNodeUtils::QueryVendorTag(pQueryVendorTag, g_VendorTagInfoDummyShdr);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummyShdrNodeCreate
///
/// @brief  Implementation of PFNNODECREATE defined in chinode.h
///
/// @param  pCreateInfo Pointer to a structure that defines create session information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyShdrNodeCreate(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult       result  = CDKResultSuccess;
    ChiDummyShdrNode*  pNode   = NULL;

    if (NULL == pCreateInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pCreateInfo is NULL");
    }

    if (pCreateInfo->size < sizeof(CHINODECREATEINFO))
    {
        LOG_ERROR(CamxLogGroupChi, "CHINODECREATEINFO is smaller than expected");
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {
        pNode = new ChiDummyShdrNode;
        if (pNode == NULL)
        {
            result = CDKResultENoMemory;
        }
    }

    if (CDKResultSuccess == result)
    {
        result = pNode->Initialize(pCreateInfo);
    }

    if (CDKResultSuccess == result)
    {
        pCreateInfo->phNodeSession = reinterpret_cast<CHIHANDLE*>(pNode);
    }

    if (CDKResultSuccess != result)
    {
        delete pNode;
        pNode = NULL;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummyShdrNodeDestroy
///
/// @brief  Implementation of PFNNODEDESTROY defined in chinode.h
///
/// @param  pDestroyInfo    Pointer to a structure that defines the session destroy information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyShdrNodeDestroy(
    CHINODEDESTROYINFO* pDestroyInfo)
{
    CDKResult result = CDKResultSuccess;

    if ((NULL == pDestroyInfo) || (NULL == pDestroyInfo->hNodeSession))
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pDestroyInfo is NULL");
    }

    if (CDKResultSuccess == result)
    {
        if (pDestroyInfo->size >= sizeof(CHINODEDESTROYINFO))
        {
            ChiDummyShdrNode* pNode = static_cast<ChiDummyShdrNode*>(pDestroyInfo->hNodeSession);

            delete pNode;

            pNode                      = NULL;
            pDestroyInfo->hNodeSession = NULL;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODEDESTROYINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummyShdrNodeQueryBufferInfo
///
/// @brief  Implementation of PFNNODEQUERYBUFFERINFO defined in chinode.h
///
/// @param  pQueryBufferInfo    Pointer to a structure to query the input buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyShdrNodeQueryBufferInfo(
    CHINODEQUERYBUFFERINFO* pQueryBufferInfo)
{
    CDKResult result = CDKResultSuccess;

    if ((NULL == pQueryBufferInfo) || (NULL == pQueryBufferInfo->hNodeSession))
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument");
    }

    if (CDKResultSuccess == result)
    {
        if (pQueryBufferInfo->size >= sizeof(CHINODEQUERYBUFFERINFO))
        {
            ChiDummyShdrNode* pNode = static_cast<ChiDummyShdrNode*>(pQueryBufferInfo->hNodeSession);
            result = pNode->QueryBufferInfo(pQueryBufferInfo);
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODEQUERYBUFFERINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummyShdrNodeSetBufferInfo
///
/// @brief  Implementation of PFNNODESETBUFFERINFO defined in chinode.h
///
/// @param  pSetBufferInfo  Pointer to a structure with information to set the output buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyShdrNodeSetBufferInfo(
    CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo)
{
    CDKResult result = CDKResultSuccess;

    if ((NULL == pSetBufferInfo) || (NULL == pSetBufferInfo->hNodeSession))
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument");
    }

    if (CDKResultSuccess == result)
    {
        if (pSetBufferInfo->size >= sizeof(CHINODESETBUFFERPROPERTIESINFO))
        {
            ChiDummyShdrNode* pNode = static_cast<ChiDummyShdrNode*>(pSetBufferInfo->hNodeSession);
            result = pNode->SetBufferInfo(pSetBufferInfo);
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODESETBUFFERPROPERTIESINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummyShdrNodeProcRequest
///
/// @brief  Implementation of PFNNODEPROCREQUEST defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyShdrNodeProcRequest(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{
    CDKResult result = CDKResultSuccess;

    if ((NULL == pProcessRequestInfo) || (NULL == pProcessRequestInfo->hNodeSession))
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument");
    }

    if (CDKResultSuccess == result)
    {
        if (pProcessRequestInfo->size >= sizeof(CHINODEPROCESSREQUESTINFO))
        {
            ChiDummyShdrNode* pNode = static_cast<ChiDummyShdrNode*>(pProcessRequestInfo->hNodeSession);
            result = pNode->ProcessRequest(pProcessRequestInfo);
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODEPROCESSREQUESTINFO is smaller than expected");
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummyShdrNodeSetNodeInterface
///
/// @brief  Implementation of PFCHINODESETNODEINTERFACE defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID DummyShdrNodeSetNodeInterface(
    ChiNodeInterface* pNodeInterface)
{
    CDKResult result = CDKResultSuccess;
    result = ChiNodeUtils::SetNodeInterface(pNodeInterface, DummyShdrNodeSectionName,
        &g_ChiNodeInterface, &g_vendorTagBase);
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiNodeEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom node.
///
/// This function must be exported by every <library>.so in order for driver to initialize the Node. This function is called
/// during the camera server initialization, which occurs during HAL process start. In addition to communicating the necessary
/// function pointers between Chi and external nodes, this function allows a node to do any initialization work that it
/// would typically do at process init. Anything done here should not be specific to a session, and any variables stored in
/// the node must be protected against multiple sessions accessing it at the same time.
///
/// @param pNodeCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the node.
///                        The node must fill in these function pointers.
///
/// @return VOID.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDK_VISIBILITY_PUBLIC VOID ChiNodeEntry(
    CHINODECALLBACKS* pNodeCallbacks)
{
    if (NULL != pNodeCallbacks)
    {
        if (pNodeCallbacks->majorVersion == ChiNodeMajorVersion &&
            pNodeCallbacks->size >= sizeof(CHINODECALLBACKS))
        {
            pNodeCallbacks->majorVersion             = ChiNodeMajorVersion;
            pNodeCallbacks->minorVersion             = ChiNodeMinorVersion;
            pNodeCallbacks->pGetCapabilities         = DummyShdrNodeGetCaps;
            pNodeCallbacks->pQueryVendorTag          = DummyShdrNodeQueryVendorTag;
            pNodeCallbacks->pCreate                  = DummyShdrNodeCreate;
            pNodeCallbacks->pDestroy                 = DummyShdrNodeDestroy;
            pNodeCallbacks->pQueryBufferInfo         = DummyShdrNodeQueryBufferInfo;
            pNodeCallbacks->pSetBufferInfo           = DummyShdrNodeSetBufferInfo;
            pNodeCallbacks->pProcessRequest          = DummyShdrNodeProcRequest;
            pNodeCallbacks->pChiNodeSetNodeInterface = DummyShdrNodeSetNodeInterface;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Chi API major version doesn't match (%d:%d) vs (%d:%d)",
                pNodeCallbacks->majorVersion, pNodeCallbacks->minorVersion,
                ChiNodeMajorVersion, ChiNodeMinorVersion);
        }
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Invalid Argument: %p", pNodeCallbacks);
    }
}
#ifdef __cplusplus
}
#endif // __cplusplus



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyShdrNode::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummyShdrNode::Initialize(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult result = CDKResultSuccess;

    m_hChiSession    = pCreateInfo->hChiSession;
    m_nodeId         = pCreateInfo->nodeId;
    m_nodeCaps       = pCreateInfo->nodeCaps.nodeCapsMask;
    m_nodeFlags      = pCreateInfo->nodeFlags;
    m_processedFrame = 0;
    result           = GetVendorTagBases();

    if (CDKResultSuccess == result)
    {
        result       = LoadLib();
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Failed to get Vendor tag bases");
    }

    if (CDKResultSuccess == result)
    {
        m_p_shdr_ctx = new SHDRNODECONTEXT;
        if (NULL == m_p_shdr_ctx)
        {
            LOG_ERROR(CamxLogGroupChi, "No memory for shdr context");
            result = CDKResultENoMemory;
        }
        m_bestats = new ParsedHDRBEStatsOutput;
        if (NULL == m_bestats && CDKResultSuccess == result)
        {
            LOG_ERROR(CamxLogGroupChi, "No memory for bestats");
            result = CDKResultENoMemory;
        }
        m_bhist = new ParsedBHistStatsOutput;
        if (NULL == m_bhist && CDKResultSuccess == result)
        {
            LOG_ERROR(CamxLogGroupChi, "No memory for bhiststats");
            result = CDKResultENoMemory;
        }
        m_bgbeconfig = new ISPAWBBGStatsConfig; // Not used in stats
        if (NULL == m_bgbeconfig && CDKResultSuccess == result)
        {
            LOG_ERROR(CamxLogGroupChi, "No memory for bgbeconfig");
            result = CDKResultENoMemory;
        }

        if (CDKResultSuccess == result)
        {
            /* Attach pointers */
            memset(m_p_shdr_ctx, 0,sizeof(SHDRNODECONTEXT));
            memset(m_bestats, 0,sizeof(ParsedHDRBEStatsOutput));
            memset(m_bhist, 0,sizeof(ParsedBHistStatsOutput));
            memset(m_bgbeconfig, 0,sizeof(ISPAWBBGStatsConfig));
            m_p_shdr_ctx->input_param.p_tuning   = &m_p_shdr_ctx->tuning;
            m_p_shdr_ctx->input_param.stats_info = &m_p_shdr_ctx->stats_info;

            m_p_shdr_ctx->init_config.sensor_layout = SHDR_INTERLEAVED_NO_STATUS;
        }
    }

    else
    {
        LOG_ERROR(CamxLogGroupChi, "Load SHDR algo lib failed");
    }

    return result;
}

void ChiDummyShdrNode::ShdrCompletionCallback(void *handle,
    SHDRINPUTPARAM *input_param, int32_t output_fd,
    SHDROUTPUTPARAM *buf_3a, void *user_data)
{
    CDK_UNUSED_PARAM(handle);
    CDK_UNUSED_PARAM(input_param);
    CDK_UNUSED_PARAM(output_fd);
    CDK_UNUSED_PARAM(buf_3a);
    SHDRCALLBACK *p_cback = (SHDRCALLBACK *)user_data;

    if (!p_cback) {
        LOG_ERROR(CamxLogGroupChi, "invalid input arguments p_cback: NULL");
        return;
    }
    if (!p_cback->user_data_frame || !p_cback->user_data_Node)
    {
        LOG_ERROR(CamxLogGroupChi,"invalid input arguments ");
        free(p_cback);
        return;
    }
    ChiDummyShdrNode *pShdr = reinterpret_cast<ChiDummyShdrNode *>(p_cback->user_data_Node);
    CHINODEPROCESSREQUESTDONEINFO info;
    INT framenum                = *(reinterpret_cast<int *>(p_cback->user_data_frame));
    LOG_VERBOSE(CamxLogGroupChi,"Got Callback ShdrCompletionCallback Framenum : %d", framenum);
    info.size                   = sizeof(CHINODEPROCESSREQUESTDONEINFO);
    info.frameNum               = framenum;
    info.result                 = CDKResultSuccess;
    info.hChiSession            = pShdr->m_hChiSession;
    info.isEarlyMetadataDone    = FALSE;
    g_ChiNodeInterface.pProcessRequestDone(&info);

    delete p_cback;
    p_cback = NULL;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyShdrNode::QueryBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummyShdrNode::QueryBufferInfo(
    CHINODEQUERYBUFFERINFO* pQueryBufferInfo)
{
    CDKResult result                     = CDKResultSuccess;

    ChiNodeUtils::DefaultBufferNegotiation(pQueryBufferInfo);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyShdrNode::SetBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummyShdrNode::SetBufferInfo(
    CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo)
{
    CDKResult result = CDKResultSuccess;

    LOG_INFO(CamxLogGroupChi, "SetBufferInfo Width=%d, Height=%d",
        pSetBufferInfo->pFormat->width,
        pSetBufferInfo->pFormat->height);

    m_inputWidth   = pSetBufferInfo->pFormat->width;
    m_inputHeight  = pSetBufferInfo->pFormat->height;
    m_inputHeight  = m_inputHeight /2;
    // m_inputHeight  = 2200;
    pSetBufferInfo->pFormat->width  = 3840;
    pSetBufferInfo->pFormat->height = 2160;

    m_outputWidth  = pSetBufferInfo->pFormat->width;
    m_outputHeight = pSetBufferInfo->pFormat->height;

    m_p_shdr_ctx->init_config.bayer_pattern       = SHDR_BAYER_RGGB;
    m_p_shdr_ctx->init_config.input_bayer_format  = SHDR_BAYER_INPUT_PLAIN16;
    m_p_shdr_ctx->init_config.output_bayer_format = SHDR_BAYER_OUTPUT_PLAIN16;

    switch (m_p_shdr_ctx->init_config.input_bayer_format)
    {
        case SHDR_BAYER_INPUT_MIPI10:
            m_bitsperpixel = 10;
            break;
        case SHDR_BAYER_INPUT_MIPI12:
            m_bitsperpixel = 12;
            break;
        case SHDR_BAYER_INPUT_PLAIN16:
        default:
            m_bitsperpixel = 16;
    }

    LOG_INFO(CamxLogGroupChi, "bitsperpixel =%d", m_bitsperpixel);

    INT32 lib_ret = m_shdr_lib_get_caps(&m_p_shdr_ctx->caps);
    if (SHDR_SUCCESS != lib_ret)
    {
        LOG_ERROR(CamxLogGroupChi, "Can not get capabilites %d", lib_ret);
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {

        LOG_INFO(CamxLogGroupChi, "lib ver %.2f api ver %.2f w %d h %d row pitch %d buf padding %d, sensor layout %d",
            m_p_shdr_ctx->caps.lib_version,
            m_p_shdr_ctx->caps.api_version,
            m_p_shdr_ctx->caps.width,
            m_p_shdr_ctx->caps.height,
            m_p_shdr_ctx->caps.row_pitch,
            m_p_shdr_ctx->caps.buf_padding,
            m_p_shdr_ctx->init_config.sensor_layout);

        LOG_INFO(CamxLogGroupChi, "lef offset %d sef offset %d lib version %.2f",
            m_p_shdr_ctx->input_param.offset[0],
            m_p_shdr_ctx->input_param.offset[1],
            m_p_shdr_ctx->caps.lib_version);

    }


    if (CDKResultSuccess == result)
    {

        CHIDATASOURCE chiDataSource;

        chiDataSource.dataSourceType     = ChiTuningManager;
        chiDataSource.pHandle            = NULL;
        TuningSetManager* pTuningManager = static_cast<TuningSetManager*>(g_ChiNodeInterface.pGetData(m_hChiSession,
                                                  &chiDataSource,
                                                  NULL,
                                                  NULL));

        CAMX_ASSERT(NULL != pTuningManager);

        shdr_1_0_0::shdr10Type* pChromatix = NULL;
        linearization_3_4_0::chromatix_linearization34Type* pLChromatix = NULL;
        if (NULL != pTuningManager)
        {
            ChiTuningModeParameter ShdrTuningData;
            ShdrTuningData.noOfSelectionParameter = 1;
            ShdrTuningData.TuningMode[0].mode          = ChiModeType::Default;
            ShdrTuningData.TuningMode[0].subMode       = { 0 };
            pChromatix = pTuningManager->GetModule_shdr10(
                            reinterpret_cast<TuningMode*>(&ShdrTuningData.TuningMode[0]),
                            (sizeof(ShdrTuningData.TuningMode)/sizeof(ShdrTuningData.TuningMode[0])));
            pLChromatix = pTuningManager->GetModule_linearization34_bps(
                            reinterpret_cast<TuningMode*>(&ShdrTuningData.TuningMode[0]),
                            (sizeof(ShdrTuningData.TuningMode)/sizeof(ShdrTuningData.TuningMode[0])));
           float blackLevel = 48.0;
           if (pLChromatix != NULL)
           {
               linearization_3_4_0::mod_linearization34_drc_gain_dataType* pDRCData = pLChromatix->chromatix_linearization34_core.mod_linearization34_drc_gain_data;
               linearization_3_4_0::mod_linearization34_hdr_aec_dataType* pHDRData = pDRCData->drc_gain_data.mod_linearization34_hdr_aec_data;
               linearization_3_4_0::mod_linearization34_led_idx_dataType* pLEDData = pHDRData->hdr_aec_data.mod_linearization34_led_idx_data;
               linearization_3_4_0::mod_linearization34_aec_dataType* pAECData = pLEDData->led_idx_data.mod_linearization34_aec_data;
               linearization_3_4_0::mod_linearization34_cct_dataType* pCCTData = pAECData->aec_data.mod_linearization34_cct_data;
               linearization_3_4_0::linearization34_rgn_dataType* pRGNData = &pCCTData->linearization34_rgn_data;
               blackLevel = (float)(pRGNData->r_lut_p_tab.r_lut_p[0]/16.0f);
               LOG_INFO(CamxLogGroupChi,"black level %f",blackLevel);
           }
           CAMX_ASSERT(NULL != pChromatix);
            num_gamma_range       = 9; //read from chromatix
            gamma_entries         = 3;
            GTM_GAMMA             = (float **)malloc(num_gamma_range * sizeof(float *));

            for (INT i =0; i < num_gamma_range; i++)
            {
                GTM_GAMMA[i] = (float *)malloc(gamma_entries * sizeof(float));
            }

            for (INT gtm_row = 0; gtm_row < num_gamma_range; gtm_row++)
            {
                GTM_GAMMA[gtm_row][0] = pChromatix->AECTuningData.lut_entries.ExpRatioZone[gtm_row].zoneTrigger.exp_ratio_start;
                GTM_GAMMA[gtm_row][1] = pChromatix->AECTuningData.lut_entries.ExpRatioZone[gtm_row].zoneTrigger.exp_ratio_end;
                GTM_GAMMA[gtm_row][2] = pChromatix->AECTuningData.lut_entries.ExpRatioZone[gtm_row].zoneData.gtm;

            }


            if (NULL != pChromatix)
            {

                LOG_INFO(CamxLogGroupChi, "TuningData ADC_BIT_DEPTH=%d, TM out bit depth =%d"
                    "BAYER_GTM_GAMMA=%f, hdr_dark_n1=%d",
                    pChromatix->BayerProcTuningData.adc_bit_depth,
                    pChromatix->BayerProcTuningData.tm_out_bit_depth,
                    pChromatix->BayerProcTuningData.bayer_gtm_gamma,
                    pChromatix->BayerProcTuningData.hdr_dark_n1);

                LOG_INFO(CamxLogGroupChi, "TuningData hdr_dark_n2_minus_n1_normalization_factor=%d "
                    "hdr_max_weight %u tm_gain=%f ltm_max_gain=%f ltm_comp_target=%f, ltm_inverse_tone_perc=%f",
                    pChromatix->BayerProcTuningData.hdr_dark_n2_minus_n1_normalization_factor,pChromatix->BayerProcTuningData.hdr_max_weight,
                    pChromatix->BayerProcTuningData.tm_gain,pChromatix->LTMData.ltm_max_gain,pChromatix->LTMData.ltm_comp_target,
                    pChromatix->LTMData.ltm_inverse_tone_perc);

                LOG_INFO(CamxLogGroupChi, "TuningData perf_hint=%d num_gpu_passes=%d gpu_ltm_en=%d",
                    static_cast<SHDRPERFHINT>(pChromatix->BayerProcTuningData.perf_hint), pChromatix->BayerProcTuningData.num_gpu_passes,
                    pChromatix->LTMData.gpu_ltm_en);

                LOG_VERBOSE(CamxLogGroupChi,"Auto LTM Params autoltm2_en %d gsn_luma_target %f k2_8bit_pre_weight %f",
                    pChromatix->AutoLTM2Data.autoltm2_en, pChromatix->AutoLTM2Data.gsn_luma_target, pChromatix->AutoLTM2Data.k2_8bit_pre_weight);

                m_p_shdr_ctx->init_config.tuning.ADC_BIT_DEPTH         = pChromatix->BayerProcTuningData.adc_bit_depth;
                m_p_shdr_ctx->init_config.tuning.TM_OUT_BIT_DEPTH      = 14;
                m_p_shdr_ctx->init_config.tuning.BAYER_GTM_GAMMA       = pChromatix->BayerProcTuningData.bayer_gtm_gamma;
                m_p_shdr_ctx->init_config.tuning.HDR_DARK_N1           = pChromatix->BayerProcTuningData.hdr_dark_n1;
                m_p_shdr_ctx->init_config.tuning.HDR_DARK_N2_MINUS_N1_NORMALIZATION_FACTOR =
                    pChromatix->BayerProcTuningData.hdr_dark_n2_minus_n1_normalization_factor;
                m_p_shdr_ctx->init_config.tuning.HDR_MAX_WEIGHT        = pChromatix->BayerProcTuningData.hdr_max_weight;
                m_p_shdr_ctx->init_config.tuning.TM_GAIN               = pChromatix->BayerProcTuningData.tm_gain;

                m_p_shdr_ctx->init_config.tuning.ltm_max_gain          = pChromatix->LTMData.ltm_max_gain;
                m_p_shdr_ctx->init_config.tuning.ltm_comp_target       = pChromatix->LTMData.ltm_comp_target;
                m_p_shdr_ctx->init_config.tuning.ltm_inverse_tone_perc = pChromatix->LTMData.ltm_inverse_tone_perc;

                m_p_shdr_ctx->init_config.perf_hint                    = static_cast<SHDRPERFHINT>(pChromatix->BayerProcTuningData.perf_hint);
                m_p_shdr_ctx->init_config.num_gpu_passes               = pChromatix->BayerProcTuningData.num_gpu_passes;
                m_p_shdr_ctx->init_config.gpu_ltm_en                   = pChromatix->LTMData.gpu_ltm_en;
                m_p_shdr_ctx->init_config.autoltm2_en                  = pChromatix->AutoLTM2Data.autoltm2_en;
                m_p_shdr_ctx->init_config.gsn_luma_target              = pChromatix->AutoLTM2Data.gsn_luma_target;
                m_p_shdr_ctx->init_config.gsn_sigma                    = pChromatix->AutoLTM2Data.gsn_sigma;
                m_p_shdr_ctx->init_config.k2_8bit_pre_weight           = pChromatix->AutoLTM2Data.k2_8bit_pre_weight;
                m_p_shdr_ctx->init_config.flat_hist_en                 = pChromatix->AutoLTM2Data.flat_hist_en;
            }
            else
            {

                LOG_INFO(CamxLogGroupChi, "TuningData pChromatix is NULL, loading default values");

                m_p_shdr_ctx->init_config.tuning.ADC_BIT_DEPTH                             = 10;
                m_p_shdr_ctx->init_config.tuning.TM_OUT_BIT_DEPTH                          = 10;
                m_p_shdr_ctx->init_config.tuning.BAYER_GTM_GAMMA                           = 0.65f;
                m_p_shdr_ctx->init_config.tuning.HDR_DARK_N1                               = 4;
                m_p_shdr_ctx->init_config.tuning.HDR_DARK_N2_MINUS_N1_NORMALIZATION_FACTOR = 500.0f;
                m_p_shdr_ctx->init_config.tuning.HDR_MAX_WEIGHT                            = 4;
                m_p_shdr_ctx->init_config.tuning.TM_GAIN                                   = 1.6f;

                m_p_shdr_ctx->init_config.tuning.ltm_max_gain                              = 0.5f;
                m_p_shdr_ctx->init_config.tuning.ltm_comp_target                           = 0.16f;
                m_p_shdr_ctx->init_config.tuning.ltm_inverse_tone_perc                     = 1.0f;

                m_p_shdr_ctx->init_config.perf_hint                                        = SHDR_PERF_HINT_NORMAL;
                m_p_shdr_ctx->init_config.num_gpu_passes                                   = 1;
                m_p_shdr_ctx->init_config.gpu_ltm_en                                       = 0;

            }

            m_p_shdr_ctx->input_param.pregain_gr                    = 1.85f;
            m_p_shdr_ctx->input_param.pregain_gb                    = 1.54f;
            m_p_shdr_ctx->init_config.tuning.EXPOSURE_RATIO_REAL[0] = 4.0f;
            m_p_shdr_ctx->init_config.tuning.BLACK_LEVEL            = blackLevel;
            m_p_shdr_ctx->init_config.num_exposure                  = 2;
            m_p_shdr_ctx->init_config.enable_deinterlace            = 0;

        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "SHDR TuningData TuningManager == NULL");
        }

        m_p_shdr_ctx->init_config.dst_stride = m_outputWidth*(m_bitsperpixel/8);

        m_p_shdr_ctx->init_config.input_buf = m_p_shdr_ctx->input_buf;
        m_p_shdr_ctx->init_config.input_buf_count = 0;

        m_p_shdr_ctx->init_config.output_buf = m_p_shdr_ctx->output_buf;
        m_p_shdr_ctx->init_config.output_buf_count = 0;

        m_p_shdr_ctx->init_config.output_buf[0].width  = m_outputWidth;
        m_p_shdr_ctx->init_config.output_buf[0].height = m_outputHeight;
        m_p_shdr_ctx->init_config.output_buf[0].stride = m_p_shdr_ctx->init_config.dst_stride;
        m_p_shdr_ctx->init_config.output_buf[0].offset = 0;

        m_p_shdr_ctx->init_config.output_buf[0].buf_size = m_outputWidth*m_outputHeight*(m_bitsperpixel/8);

        m_p_shdr_ctx->init_config.output_buf[0].buf = NULL;
        m_p_shdr_ctx->init_config.output_buf[0].fd = -1;

        LOG_INFO(CamxLogGroupChi, "SetBufferInfo dst_stride=%d Output: buf_size=%d wxh=%dx%d",
            m_p_shdr_ctx->init_config.dst_stride, m_p_shdr_ctx->init_config.output_buf[0].buf_size,
            m_p_shdr_ctx->init_config.output_buf[0].width, m_p_shdr_ctx->init_config.output_buf[0].height);

        m_p_shdr_ctx->init_config.src_stride = m_inputWidth*(m_bitsperpixel/8)*2;

        m_p_shdr_ctx->init_config.input_buf[0].width = m_inputWidth;
        m_p_shdr_ctx->init_config.input_buf[0].height = m_inputHeight;
        m_p_shdr_ctx->init_config.input_buf[0].stride = m_p_shdr_ctx->init_config.src_stride;


        m_p_shdr_ctx->init_config.input_buf[0].offset = 0;

        m_p_shdr_ctx->init_config.input_buf[0].buf_size = (m_inputWidth*2)*m_inputHeight*(m_bitsperpixel/8);
        m_p_shdr_ctx->init_config.input_buf[0].buf = NULL;

        m_p_shdr_ctx->init_config.input_buf[0].fd = -1;

        LOG_INFO(CamxLogGroupChi, "SetBufferInfo src_stride=%d Input: buf_size=%d wxh=%dx%d",
            m_p_shdr_ctx->init_config.src_stride, m_p_shdr_ctx->init_config.input_buf[0].buf_size,
            m_p_shdr_ctx->init_config.input_buf[0].width, m_p_shdr_ctx->init_config.input_buf[0].height);

        m_p_shdr_ctx->input_param.offset[0] = 20*(m_inputWidth*(m_bitsperpixel/8))*2+m_bitsperpixel;
        m_p_shdr_ctx->input_param.offset[1] = 324*(m_inputWidth*(m_bitsperpixel/8))*2+3880*2;

        m_p_shdr_ctx->init_config.offset[0] = m_p_shdr_ctx->input_param.offset[0];
        m_p_shdr_ctx->init_config.offset[1] = m_p_shdr_ctx->input_param.offset[1];

        LOG_INFO(CamxLogGroupChi, "SetBufferInfo LEF offset=%d, SEF offset=%d", m_p_shdr_ctx->init_config.offset[0],
                                                                                 m_p_shdr_ctx->init_config.offset[1]);

        if (NULL == &(m_p_shdr_ctx->handle) || NULL == &(m_p_shdr_ctx->init_config))
        {
            LOG_ERROR(CamxLogGroupChi, "SHDR handle or init_config is NULL");
            result = CDKResultEFailed;
        }
    }

    if (CDKResultSuccess == result)
    {
        INT32 ret_lib = m_shdr_lib_init(&(m_p_shdr_ctx->handle), &(m_p_shdr_ctx->init_config));

        if (SHDR_SUCCESS != ret_lib)
        {
            LOG_ERROR(CamxLogGroupChi, " Can not init library %d", ret_lib);
            result = CDKResultEFailed;
        }
        else
        {
            LOG_INFO(CamxLogGroupChi, "SHDR Library Init Succesful");
        }
    }

    return result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyShdrNode::UpdateTuningData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiDummyShdrNode::UpdateTuningData()
{
        CHIDATASOURCE chiDataSource;

        chiDataSource.dataSourceType     = ChiTuningManager;
        chiDataSource.pHandle            = NULL;
        TuningSetManager* pTuningManager = static_cast<TuningSetManager*>(g_ChiNodeInterface.pGetData(m_hChiSession,
                                                  &chiDataSource,
                                                  NULL,
                                                  NULL));

        CAMX_ASSERT(NULL != pTuningManager);

        shdr_1_0_0::shdr10Type* pChromatix = NULL;
        linearization_3_4_0::chromatix_linearization34Type* pLChromatix = NULL;
        if (NULL != pTuningManager)
        {
            ChiTuningModeParameter ShdrTuningData;
            ShdrTuningData.noOfSelectionParameter = 1;
            ShdrTuningData.TuningMode[0].mode          = ChiModeType::Default;
            ShdrTuningData.TuningMode[0].subMode       = { 0 };
            pChromatix = pTuningManager->GetModule_shdr10(
                            reinterpret_cast<TuningMode*>(&ShdrTuningData.TuningMode[0]),
                            (sizeof(ShdrTuningData.TuningMode)/sizeof(ShdrTuningData.TuningMode[0])));
            pLChromatix = pTuningManager->GetModule_linearization34_bps(
                            reinterpret_cast<TuningMode*>(&ShdrTuningData.TuningMode[0]),
                            (sizeof(ShdrTuningData.TuningMode)/sizeof(ShdrTuningData.TuningMode[0])));
           float blackLevel = 48.0;
           if (pLChromatix != NULL)
           {
               linearization_3_4_0::mod_linearization34_drc_gain_dataType* pDRCData = pLChromatix->chromatix_linearization34_core.mod_linearization34_drc_gain_data;
               linearization_3_4_0::mod_linearization34_hdr_aec_dataType* pHDRData = pDRCData->drc_gain_data.mod_linearization34_hdr_aec_data;
               linearization_3_4_0::mod_linearization34_led_idx_dataType* pLEDData = pHDRData->hdr_aec_data.mod_linearization34_led_idx_data;
               linearization_3_4_0::mod_linearization34_aec_dataType* pAECData = pLEDData->led_idx_data.mod_linearization34_aec_data;
               linearization_3_4_0::mod_linearization34_cct_dataType* pCCTData = pAECData->aec_data.mod_linearization34_cct_data;
               linearization_3_4_0::linearization34_rgn_dataType* pRGNData = &pCCTData->linearization34_rgn_data;
               blackLevel = (float)(pRGNData->r_lut_p_tab.r_lut_p[0]/16.0f);
               LOG_INFO(CamxLogGroupChi,"black level %f",blackLevel);
           }
           CAMX_ASSERT(NULL != pChromatix);
            num_gamma_range       = 9; //read from chromatix
            gamma_entries         = 3;
            GTM_GAMMA             = (float **)malloc(num_gamma_range * sizeof(float *));

            for (INT i =0; i < num_gamma_range; i++)
            {
                GTM_GAMMA[i] = (float *)malloc(gamma_entries * sizeof(float));
            }

            for (INT gtm_row = 0; gtm_row < num_gamma_range; gtm_row++)
            {
                GTM_GAMMA[gtm_row][0] = pChromatix->AECTuningData.lut_entries.ExpRatioZone[gtm_row].zoneTrigger.exp_ratio_start;
                GTM_GAMMA[gtm_row][1] = pChromatix->AECTuningData.lut_entries.ExpRatioZone[gtm_row].zoneTrigger.exp_ratio_end;
                GTM_GAMMA[gtm_row][2] = pChromatix->AECTuningData.lut_entries.ExpRatioZone[gtm_row].zoneData.gtm;

            }


            if (NULL != pChromatix)
            {

                LOG_INFO(CamxLogGroupChi, "TuningData ADC_BIT_DEPTH=%d, TM out bit depth =%d"
                    "BAYER_GTM_GAMMA=%f, hdr_dark_n1=%d",
                    pChromatix->BayerProcTuningData.adc_bit_depth,
                    pChromatix->BayerProcTuningData.tm_out_bit_depth,
                    pChromatix->BayerProcTuningData.bayer_gtm_gamma,
                    pChromatix->BayerProcTuningData.hdr_dark_n1);

                LOG_INFO(CamxLogGroupChi, "TuningData hdr_dark_n2_minus_n1_normalization_factor=%d "
                    "hdr_max_weight %u tm_gain=%f ltm_max_gain=%f ltm_comp_target=%f, ltm_inverse_tone_perc=%f",
                    pChromatix->BayerProcTuningData.hdr_dark_n2_minus_n1_normalization_factor,pChromatix->BayerProcTuningData.hdr_max_weight,
                    pChromatix->BayerProcTuningData.tm_gain,pChromatix->LTMData.ltm_max_gain,pChromatix->LTMData.ltm_comp_target,
                    pChromatix->LTMData.ltm_inverse_tone_perc);

                LOG_INFO(CamxLogGroupChi, "TuningData perf_hint=%d num_gpu_passes=%d gpu_ltm_en=%d",
                    static_cast<SHDRPERFHINT>(pChromatix->BayerProcTuningData.perf_hint), pChromatix->BayerProcTuningData.num_gpu_passes,
                    pChromatix->LTMData.gpu_ltm_en);

                LOG_VERBOSE(CamxLogGroupChi,"Auto LTM Params autoltm2_en %d gsn_luma_target %f k2_8bit_pre_weight %f",
                    pChromatix->AutoLTM2Data.autoltm2_en, pChromatix->AutoLTM2Data.gsn_luma_target, pChromatix->AutoLTM2Data.k2_8bit_pre_weight);

                m_p_shdr_ctx->init_config.tuning.ADC_BIT_DEPTH         = pChromatix->BayerProcTuningData.adc_bit_depth;
                m_p_shdr_ctx->init_config.tuning.TM_OUT_BIT_DEPTH      = 14;
                m_p_shdr_ctx->init_config.tuning.BAYER_GTM_GAMMA       = pChromatix->BayerProcTuningData.bayer_gtm_gamma;
                m_p_shdr_ctx->init_config.tuning.HDR_DARK_N1           = pChromatix->BayerProcTuningData.hdr_dark_n1;
                m_p_shdr_ctx->init_config.tuning.HDR_DARK_N2_MINUS_N1_NORMALIZATION_FACTOR =
                    pChromatix->BayerProcTuningData.hdr_dark_n2_minus_n1_normalization_factor;
                m_p_shdr_ctx->init_config.tuning.HDR_MAX_WEIGHT        = pChromatix->BayerProcTuningData.hdr_max_weight;
                m_p_shdr_ctx->init_config.tuning.TM_GAIN               = pChromatix->BayerProcTuningData.tm_gain;

                m_p_shdr_ctx->init_config.tuning.ltm_max_gain          = pChromatix->LTMData.ltm_max_gain;
                m_p_shdr_ctx->init_config.tuning.ltm_comp_target       = pChromatix->LTMData.ltm_comp_target;
                m_p_shdr_ctx->init_config.tuning.ltm_inverse_tone_perc = pChromatix->LTMData.ltm_inverse_tone_perc;

                m_p_shdr_ctx->init_config.perf_hint                    = static_cast<SHDRPERFHINT>(pChromatix->BayerProcTuningData.perf_hint);
                m_p_shdr_ctx->init_config.num_gpu_passes               = pChromatix->BayerProcTuningData.num_gpu_passes;
                m_p_shdr_ctx->init_config.gpu_ltm_en                   = pChromatix->LTMData.gpu_ltm_en;
                m_p_shdr_ctx->init_config.autoltm2_en                  = pChromatix->AutoLTM2Data.autoltm2_en;
                m_p_shdr_ctx->init_config.gsn_luma_target              = pChromatix->AutoLTM2Data.gsn_luma_target;
                m_p_shdr_ctx->init_config.gsn_sigma                    = pChromatix->AutoLTM2Data.gsn_sigma;
                m_p_shdr_ctx->init_config.k2_8bit_pre_weight           = pChromatix->AutoLTM2Data.k2_8bit_pre_weight;
                m_p_shdr_ctx->init_config.flat_hist_en                 = pChromatix->AutoLTM2Data.flat_hist_en;
            }
            else
            {

                LOG_INFO(CamxLogGroupChi, "TuningData pChromatix is NULL, loading default values");

                m_p_shdr_ctx->init_config.tuning.ADC_BIT_DEPTH                             = 10;
                m_p_shdr_ctx->init_config.tuning.TM_OUT_BIT_DEPTH                          = 10;
                m_p_shdr_ctx->init_config.tuning.BAYER_GTM_GAMMA                           = 0.65f;
                m_p_shdr_ctx->init_config.tuning.HDR_DARK_N1                               = 4;
                m_p_shdr_ctx->init_config.tuning.HDR_DARK_N2_MINUS_N1_NORMALIZATION_FACTOR = 500.0f;
                m_p_shdr_ctx->init_config.tuning.HDR_MAX_WEIGHT                            = 4;
                m_p_shdr_ctx->init_config.tuning.TM_GAIN                                   = 1.6f;

                m_p_shdr_ctx->init_config.tuning.ltm_max_gain                              = 0.5f;
                m_p_shdr_ctx->init_config.tuning.ltm_comp_target                           = 0.16f;
                m_p_shdr_ctx->init_config.tuning.ltm_inverse_tone_perc                     = 1.0f;

                m_p_shdr_ctx->init_config.perf_hint                                        = SHDR_PERF_HINT_NORMAL;
                m_p_shdr_ctx->init_config.num_gpu_passes                                   = 1;
                m_p_shdr_ctx->init_config.gpu_ltm_en                                       = 0;

            }

            m_p_shdr_ctx->input_param.pregain_gr                    = 1.85f;
            m_p_shdr_ctx->input_param.pregain_gb                    = 1.54f;
            m_p_shdr_ctx->init_config.tuning.EXPOSURE_RATIO_REAL[0] = 4.0f;
            m_p_shdr_ctx->init_config.tuning.BLACK_LEVEL            = blackLevel;
            m_p_shdr_ctx->init_config.num_exposure                  = 2;
            m_p_shdr_ctx->init_config.enable_deinterlace            = 0;

        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "SHDR TuningData TuningManager == NULL");
        }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyShdrNode::CheckDependency
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChiDummyShdrNode::CheckDependency(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)

{
    /*
     * The following code illustrate the usage of tag dependency. In the case of Chi node's processing has dependency on other
     * tags and the tags are not published yet, the Chi node could set the pDependency in pProcessRequestInfo and return
     * immediately. The framework will call the ExecuteProcessRequest again once the dependency is met.
     */
    BOOL                satisfied           = TRUE;
    UINT16              dependencyCount     = 0;
    CHIDEPENDENCYINFO*  pDependencyInfo     = pProcessRequestInfo->pDependency;

    VOID* pData = NULL;
    UINT32 tag  = ANDROID_SENSOR_TIMESTAMP;
    pData       = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum, tag, ChiMetadataDynamic,
                                            &g_ChiNodeInterface, m_hChiSession);
    // if the dependent tag is not published yet, set it as dependency
    if (NULL == pData)
    {
        pDependencyInfo->properties[dependencyCount]    = tag;
        pDependencyInfo->offsets[dependencyCount]       = 0;
        dependencyCount++;
    }

    tag = m_AECDataVendorTagBase;
    pData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum, tag, ChiMetadataDynamic,
                                      &g_ChiNodeInterface, m_hChiSession);
    // if the dependent tag is not published yet, set it as dependency
    if (NULL == pData)
    {
        pDependencyInfo->properties[dependencyCount]    = tag;
        pDependencyInfo->offsets[dependencyCount]       = 0;
        dependencyCount++;
    }


    if (dependencyCount > 0)
    {
        pDependencyInfo->count        = dependencyCount;
        pDependencyInfo->hNodeSession = m_hChiSession;
        // uses processSequenceId to determine what state it is in.
        pProcessRequestInfo->pDependency->processSequenceId = 1;
        satisfied = FALSE;
    }

    return satisfied;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyShdrNode::ProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummyShdrNode::ProcessRequest(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{

    CDKResult result     = CDKResultSuccess;
    BOOL isTuningChanged = FALSE;
    SHDRCALLBACK *p_cback = new SHDRCALLBACK;
    VOID*     ptuningChangedMetaData = NULL;
    ptuningChangedMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          (m_tuningEnabledVendorTagBase),
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);

    if (NULL != ptuningChangedMetaData)
    {
        isTuningChanged = *reinterpret_cast<BOOL*>(ptuningChangedMetaData);
        LOG_INFO(CamxLogGroupChi, "Tuning ReqId : %d and changed:%d ", (int)pProcessRequestInfo->frameNum, isTuningChanged);
    }
    else
    {
        LOG_INFO(CamxLogGroupChi, "Tuning Meta is NULL");
    }
    if(isTuningChanged)
    {
        UpdateTuningData();
    }
    if (!p_cback)
    {
        LOG_ERROR(CamxLogGroupChi, "failed: no memory");
        result = CDKResultEFailed;
    }
    BOOL satisfied;

    if (IsRealTime())
    {
        satisfied = CheckDependency(pProcessRequestInfo);
        // satisfied = TRUE;
    }
    else
    {
        satisfied = TRUE;
    }
    if (TRUE == satisfied)
    {
    if ((CDKResultSuccess == result) )
    {
        if (NULL == m_p_shdr_ctx->handle)
        {
            LOG_ERROR(CamxLogGroupChi, "SHDR Handle is NULL");
            result = CDKResultEFailed;
        }

        else
        {
            m_input_buff.width = m_p_shdr_ctx->init_config.input_buf[0].width;
            m_input_buff.height = m_p_shdr_ctx->init_config.input_buf[0].height;
            m_input_buff.stride = m_p_shdr_ctx->init_config.src_stride;
            m_input_buff.buf_size = (*pProcessRequestInfo->phInputBuffer)->planeSize[0];
            m_input_buff.offset = m_p_shdr_ctx->init_config.input_buf[0].offset;
            m_input_buff.buf = reinterpret_cast<uint8_t*>((*pProcessRequestInfo->phInputBuffer)->pImageList[0].pAddr[0]);
            m_input_buff.fd = (*pProcessRequestInfo->phInputBuffer)->pImageList[0].fd[0];

            LOG_INFO(CamxLogGroupChi, "ReqId : %d ProcessRequest input buf w=%d, h=%d, st=%d, buf_size=%d, off=%d,buf=%p,fd=%d",
                (int)pProcessRequestInfo->frameNum, m_input_buff.width, m_input_buff.height, m_input_buff.stride, m_input_buff.buf_size, m_input_buff.offset,
                m_input_buff.buf, m_input_buff.fd);

            INT32 lib_ret = m_shdr_lib_map_buffer(m_p_shdr_ctx->handle, &m_input_buff, TRUE);
            if (SHDR_SUCCESS != lib_ret && SHDR_ALREADY_MAPPED != lib_ret)
            {
                LOG_ERROR(CamxLogGroupChi, "Can not map input buffer %d", lib_ret);
                result = CDKResultEFailed;
            }
        }
    }

    VOID* pAFDMetadata = NULL;
    FLOAT senExpRatio = DEFAULT_EXP_RATIO;
    UINT32 tag  = ANDROID_STATISTICS_SCENE_FLICKER;
    pAFDMetadata =  ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          tag,
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
    UINT8 bandingMode = *static_cast<UINT8*>(pAFDMetadata);
    if (bandingMode == ANDROID_STATISTICS_SCENE_FLICKER_50HZ)
    {
        senExpRatio = EXP_RATIO_50HZ;
    }
    else if (bandingMode == ANDROID_STATISTICS_SCENE_FLICKER_60HZ)
    {
        senExpRatio = EXP_RATIO_60HZ;
    }

    LOG_VERBOSE(CamxLogGroupChi," banding mode %d", bandingMode);


    VOID*  pMetaData      = NULL;
    FLOAT  expRatio       = m_p_shdr_ctx->init_config.tuning.EXPOSURE_RATIO_REAL[0];
    FLOAT  pAWBRGainData  = 1.0;
    FLOAT  pAWBGGainData  = 1.0;
    FLOAT  pAWBBGainData  = 1.0;

    // Get AEC info
    pMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          (m_AECDataVendorTagBase),
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
    if (NULL != pMetaData)
    {
        AECFrameControl* pAECData = reinterpret_cast<AECFrameControl*>(pMetaData);
        expRatio = (FLOAT)((FLOAT)pAECData->exposureInfo[AECAlgoExposureLong].sensitivity /
                        (FLOAT)pAECData->exposureInfo[AECAlgoExposureShort].sensitivity);
        LOG_VERBOSE(CamxLogGroupChi, "AEC ReqId : %d and expRatio:%f ", (int)pProcessRequestInfo->frameNum, expRatio);
        LOG_VERBOSE(CamxLogGroupChi, "Lexposure : %lld and Sexposure:%lld",
            (long long int)pAECData->exposureInfo[AECAlgoExposureLong].sensitivity,
            (long long int)pAECData->exposureInfo[AECAlgoExposureShort].sensitivity);
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "AEC Meta is NULL");
    }
    if (expRatio <=1.0f)
    {
        expRatio = DEFAULT_EXP_RATIO;
    }

    float gtm_gamma = Calc_Gtm_Gamma(expRatio);
    LOG_VERBOSE(CamxLogGroupChi, "expRatio:%f and gtm_gamma :%f", expRatio, gtm_gamma);

    // Get AWB info
    VOID*     pAWBRGainMetaData = NULL;
    pAWBRGainMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          (m_AWBRGainDataVendorTagBase),
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
    if (NULL != pAWBRGainMetaData)
    {
        pAWBRGainData = *(reinterpret_cast<FLOAT*>(pAWBRGainMetaData));
        LOG_VERBOSE(CamxLogGroupChi, "AWB ReqId : %d and rgain:%f", (int)pProcessRequestInfo->frameNum, pAWBRGainData);
    }
    else
    {
            LOG_ERROR(CamxLogGroupChi, "AWB R Gain Meta is NULL using default ");
    }

    // Get AWB info
    VOID*     pAWBGGainMetaData = NULL;
    pAWBGGainMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          (m_AWBGGainDataVendorTagBase),
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
    if (NULL != pAWBGGainMetaData)
    {
        pAWBGGainData = *(reinterpret_cast<FLOAT*>(pAWBGGainMetaData));
        LOG_VERBOSE(CamxLogGroupChi, "AWB ReqId : %d and ggain:%f", (int)pProcessRequestInfo->frameNum, pAWBGGainData);
    }
    else
    {
            LOG_ERROR(CamxLogGroupChi, "AWB G Gain Meta is NULL");
    }

    // Get AWB info
    VOID*     pAWBBGainMetaData = NULL;
    pAWBBGainMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          (m_AWBBGainDataVendorTagBase),
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
    if (NULL != pAWBBGainMetaData)
    {
        pAWBBGainData = *(reinterpret_cast<FLOAT*>(pAWBBGainMetaData));
        LOG_VERBOSE(CamxLogGroupChi, "AWB ReqId : %d and bgain:%f", (int)pProcessRequestInfo->frameNum, pAWBBGainData);
    }
    else
    {
            LOG_ERROR(CamxLogGroupChi, "AWB B Gain Meta is NULL");
    }


    if(CDKResultSuccess == result)
    {
        m_output_buff.width     = m_p_shdr_ctx->init_config.output_buf[0].width;
        m_output_buff.height    = m_p_shdr_ctx->init_config.output_buf[0].height;
        m_output_buff.stride    = m_p_shdr_ctx->init_config.dst_stride;
        m_output_buff.buf_size  = (*pProcessRequestInfo->phOutputBuffer)->planeSize[0];
        m_output_buff.offset    = m_p_shdr_ctx->init_config.output_buf[0].offset;
        m_output_buff.buf       =
                reinterpret_cast<uint8_t*>((*pProcessRequestInfo->phOutputBuffer)->pImageList[0].pAddr[0]);
        m_output_buff.fd        = (*pProcessRequestInfo->phOutputBuffer)->pImageList[0].fd[0];

        LOG_VERBOSE(CamxLogGroupChi, "ProcessRequest output buf w=%d, h=%d, st=%d, buf_size=%d, off=%d, buf=%p,fd=%d",
            m_output_buff.width, m_output_buff.height, m_output_buff.stride, m_output_buff.buf_size, m_output_buff.offset,
            m_output_buff.buf, m_output_buff.fd);

        INT32 lib_ret = m_shdr_lib_map_buffer(m_p_shdr_ctx->handle, &m_output_buff, FALSE);
        if (SHDR_SUCCESS != lib_ret && SHDR_ALREADY_MAPPED != lib_ret)
        {
            LOG_ERROR(CamxLogGroupChi, "Can not map output buffer %d", lib_ret);
            result = CDKResultEFailed;
        }
    }

    if(CDKResultSuccess == result)
    {

        m_p_shdr_ctx->input_param.p_tuning->HDR_DARK_N2_MINUS_N1_NORMALIZATION_FACTOR =
                m_p_shdr_ctx->init_config.tuning.HDR_DARK_N2_MINUS_N1_NORMALIZATION_FACTOR;
        m_p_shdr_ctx->input_param.p_tuning->HDR_MAX_WEIGHT =
                m_p_shdr_ctx->init_config.tuning.HDR_MAX_WEIGHT;
        m_p_shdr_ctx->input_param.p_tuning->TM_GAIN =
                m_p_shdr_ctx->init_config.tuning.TM_GAIN;
        m_p_shdr_ctx->input_param.p_tuning->ADC_BIT_DEPTH =
                m_p_shdr_ctx->init_config.tuning.ADC_BIT_DEPTH;
        m_p_shdr_ctx->input_param.p_tuning->BAYER_GTM_GAMMA = gtm_gamma;
        m_p_shdr_ctx->input_param.p_tuning->EXPOSURE_RATIO_REAL[0]   = expRatio;
        m_p_shdr_ctx->input_param.p_tuning->EXPOSURE_RATIO_SENSOR[0] = expRatio;
        m_p_shdr_ctx->input_param.p_tuning->BLACK_LEVEL =
                m_p_shdr_ctx->init_config.tuning.BLACK_LEVEL;
        m_p_shdr_ctx->input_param.p_tuning->HDR_DARK_N1 =
                m_p_shdr_ctx->init_config.tuning.HDR_DARK_N1;
        m_p_shdr_ctx->input_param.p_tuning->TM_OUT_BIT_DEPTH =
             m_p_shdr_ctx->init_config.tuning.TM_OUT_BIT_DEPTH;
        m_p_shdr_ctx->input_param.p_tuning->ltm_max_gain =
             m_p_shdr_ctx->init_config.tuning.ltm_max_gain;
        m_p_shdr_ctx->input_param.p_tuning->ltm_comp_target =
             m_p_shdr_ctx->init_config.tuning.ltm_comp_target ;
        m_p_shdr_ctx->input_param.p_tuning->ltm_inverse_tone_perc =
              m_p_shdr_ctx->init_config.tuning.ltm_inverse_tone_perc;
        m_p_shdr_ctx->input_param.pregain_gr = (pAWBRGainData) / (pAWBGGainData);
        m_p_shdr_ctx->input_param.pregain_gb = (pAWBBGainData )/ (pAWBGGainData);
        m_p_shdr_ctx->autoltm2_en = m_p_shdr_ctx->init_config.autoltm2_en;
        LOG_INFO(CamxLogGroupChi, "PerFrame[%d] Stats N1_NORMALIZATION_FACTOR:%f, HDR_MAX_WEIGHT:%d"
        " TM_GAIN:%f, ADC_BIT_DEPTH:%d, BAYER_GTM_GAMMA:%f, EXPOSURE_RATIO_REAL:%f, BLACK_LEVEL:%f"
        " HDR_DARK_N1:%d, TM_OUT_BIT_DEPTH:%d, ltm_max_gain:%f, ltm_comp_target:%f"
        " ltm_inverse_tone_perc:%f, pregain_gr:%f, pregain_gb:%f autoltm2_en %d",
        m_p_shdr_ctx->input_param.stats_info->frame_id,
        m_p_shdr_ctx->input_param.p_tuning->HDR_DARK_N2_MINUS_N1_NORMALIZATION_FACTOR,
        m_p_shdr_ctx->input_param.p_tuning->HDR_MAX_WEIGHT,
        m_p_shdr_ctx->input_param.p_tuning->TM_GAIN,
        m_p_shdr_ctx->input_param.p_tuning->ADC_BIT_DEPTH,
        m_p_shdr_ctx->input_param.p_tuning->BAYER_GTM_GAMMA,
        m_p_shdr_ctx->input_param.p_tuning->EXPOSURE_RATIO_REAL[0],
        m_p_shdr_ctx->input_param.p_tuning->BLACK_LEVEL,
        m_p_shdr_ctx->input_param.p_tuning->HDR_DARK_N1,
        m_p_shdr_ctx->input_param.p_tuning->TM_OUT_BIT_DEPTH,
        m_p_shdr_ctx->input_param.p_tuning->ltm_max_gain,
        m_p_shdr_ctx->input_param.p_tuning->ltm_comp_target,
        m_p_shdr_ctx->input_param.p_tuning->ltm_inverse_tone_perc,
        m_p_shdr_ctx->input_param.pregain_gr,
        m_p_shdr_ctx->input_param.pregain_gb,
        m_p_shdr_ctx->autoltm2_en);

        if (m_p_shdr_ctx->init_config.gpu_ltm_en == 1 &&
            m_p_shdr_ctx->autoltm2_en)
        {
            float gain_limit =
            (m_p_shdr_ctx->input_param.p_tuning->EXPOSURE_RATIO_REAL[0] *
             m_p_shdr_ctx->input_param.p_tuning->ltm_max_gain);
             m_p_shdr_ctx->autoltm2_param.max_gain_limit = gain_limit;
             m_p_shdr_ctx->autoltm2_param.gsn_sigma =  m_p_shdr_ctx->init_config.gsn_sigma;
             m_p_shdr_ctx->autoltm2_param.gsn_luma_target =  m_p_shdr_ctx->init_config.gsn_luma_target;
             m_p_shdr_ctx->autoltm2_param.k2_8bit_pre_weight =  m_p_shdr_ctx->init_config.k2_8bit_pre_weight;
             m_p_shdr_ctx->autoltm2_param.flat_hist_en =  m_p_shdr_ctx->init_config.flat_hist_en;
             m_p_shdr_ctx->input_param.p_tuning->ltm_comp_target =
             (m_p_shdr_ctx->g_shdr_k2_8bit / 512.0f);
             LOG_VERBOSE(CamxLogGroupChi,"gain_limit %f g_shdr_k2_8bit %f ltm_comp_target %f" ,gain_limit,
             m_p_shdr_ctx->g_shdr_k2_8bit,
             m_p_shdr_ctx->input_param.p_tuning->ltm_comp_target);
        }

        m_p_shdr_ctx->input_param.input_buf_count = 1;
        m_p_shdr_ctx->input_param.input_fd_list = m_p_shdr_ctx->input_fd_list;

        m_p_shdr_ctx->input_param.stats_info->frame_id = static_cast<int>(pProcessRequestInfo->frameNum);
        m_p_shdr_ctx->input_fd_list[0] = m_input_buff.fd;

        p_cback->user_data_Node = this;
        int frameIdtoSendtoStatsCallback = m_p_shdr_ctx->input_param.stats_info->frame_id;
        p_cback->p_shdr_ctx = m_p_shdr_ctx;
        frameNum[frameIdtoSendtoStatsCallback % MaxRequests] = frameIdtoSendtoStatsCallback;
        p_cback->user_data_frame = &frameNum[frameIdtoSendtoStatsCallback % MaxRequests];

        LOG_VERBOSE(CamxLogGroupChi, "shdr_lib_process for frame id:%d", m_p_shdr_ctx->input_param.stats_info->frame_id);
        INT ret_lib = m_shdr_lib_process(m_p_shdr_ctx->handle,
            &m_p_shdr_ctx->input_param,
            m_output_buff.fd,
            &m_p_shdr_ctx->buf_3a,
            ShdrCompletionCallback,
            (ChiDummyShdrNode::ImgAlgoShdrAECStatsReadyCallback),
            p_cback);
        pProcessRequestInfo->isDelayedRequestDone = TRUE;
        LOG_VERBOSE(CamxLogGroupChi, "Sync shdr_lib_process return =%d", ret_lib);

    }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyShdrNode::GetVendorTagBases
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummyShdrNode::GetVendorTagBases()

{
    CDKResult            result            = CDKResultSuccess;
    CHIVENDORTAGBASEINFO vendorTagBaseInfo = { 0 };

    // Get the "AEC Frame Control" vendor tag base
    result = ChiNodeUtils::GetVendorTagBase("org.quic.camera2.statsconfigs",
                                            "AECFrameControl",
                                            &g_ChiNodeInterface,
                                            &vendorTagBaseInfo);
    if (CDKResultSuccess == result)
    {
        m_AECDataVendorTagBase = vendorTagBaseInfo.vendorTagBase;
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Unable to get AEC Sensor mode");
    }

    if (CDKResultSuccess == result)
    {
        // Get the "AWB Frame Control" vendor tag base
        result = ChiNodeUtils::GetVendorTagBase("org.quic.camera2.statsconfigs",
                                                "AWBFrameControlRGain",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_AWBRGainDataVendorTagBase = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get AEC Sensor mode");
        }
    }

    if (CDKResultSuccess == result)
    {
        // Get the "AWB Frame Control" vendor tag base
        result = ChiNodeUtils::GetVendorTagBase("org.quic.camera2.statsconfigs",
                                                "AWBFrameControlGGain",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_AWBGGainDataVendorTagBase = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get AEC Sensor mode");
        }
    }

    if (CDKResultSuccess == result)
    {
        // Get the "AWB Frame Control" vendor tag base
        result = ChiNodeUtils::GetVendorTagBase("org.quic.camera2.statsconfigs",
                                                "AWBFrameControlBGain",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_AWBBGainDataVendorTagBase = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get AEC Sensor mode");
        }
    }
    if (CDKResultSuccess == result)
    {
        // Get the "Tuning data" vendor tag base
        result = ChiNodeUtils::GetVendorTagBase("org.quic.camera.tuningInfo",
                                                "isTuningChanged",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_tuningEnabledVendorTagBase = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get tuning vendor data");
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyShdrNode::ImgAlgoShdrAECStatsReadyCallback
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiDummyShdrNode::ImgAlgoShdrAECStatsReadyCallback(
    SHDRSTATSINFO *stats,
    VOID * user_data)
{

    CDKResult result = CDKResultSuccess;

    SHDRCALLBACK* p_cback = (SHDRCALLBACK*)user_data;
    SHDRNODECONTEXT* p_shdr_ctx =  p_cback->p_shdr_ctx;
    if (NULL == p_cback || NULL == stats) {
        LOG_ERROR(CamxLogGroupChi, "AECStatsReadyCallback p_cback or stats is NULL");
        result = CDKResultEFailed;
    }

    if(CDKResultSuccess == result)
    {
        ChiDummyShdrNode* pNode = reinterpret_cast<ChiDummyShdrNode*>(p_cback->user_data_Node);
        int* FrameIdinAECStatsReadyCallback = reinterpret_cast<int*>(p_cback->user_data_frame);
        if (NULL == pNode)
        {
            LOG_ERROR(CamxLogGroupChi, "AECStatsReadyCallback pNode == NULL");
        }
        else
        {
            SHDRSTATSINFO* stats_info = (SHDRSTATSINFO*)stats;
            int numberOfBEStatsROIs   = stats_info->num_grid_w * stats_info->num_grid_h;
            float y_sum = 0.0f;

            pNode->m_bgbeconfig->regionWidth  = static_cast<UINT32>(stats_info->num_grid_w);
            pNode->m_bgbeconfig->regionHeight = static_cast<UINT32>(stats_info->num_grid_h);

            for (int i = 0; i < numberOfBEStatsROIs ; i++)
            {
               pNode->m_bestats->SetYChannelData(stats_info->y_avg[i], i);
               y_sum += stats_info->y_avg[i];
            }

            pNode->m_bestats->numROIs     = numberOfBEStatsROIs;
            pNode->m_bestats->flags.usesY = TRUE;
            pNode->m_bestats->flags.onlyY = TRUE;
            pNode->m_bestats->flags.hasSatInfo= FALSE;

            for (int i = 0; i < MAX_SHDR_BHIST_STATS_SIZE; i++)
            {
                pNode->m_bhist->BHistogramStats[i] = static_cast<UINT>(stats_info->bin[i]);
            }

            pNode->m_bhist->numBins     = MAX_SHDR_BHIST_STATS_SIZE;
            pNode->m_bhist->channelType = ColorChannelY;

            LOG_VERBOSE(CamxLogGroupChi, "y_sum %f, numberOfBEStatsROIs is %d, num_grid_w %d"
            "num_grid_h %d, numBins =%d, channelType=%d, frame id =%d satInfo:%d",
                y_sum, numberOfBEStatsROIs, stats_info->num_grid_w, stats_info->num_grid_h,
                pNode->m_bhist->numBins, pNode->m_bhist->channelType, *FrameIdinAECStatsReadyCallback,
                pNode->m_bestats->flags.hasSatInfo);

        }
        if (p_shdr_ctx->init_config.gpu_ltm_en  == 1 &&
            p_shdr_ctx->autoltm2_en)
        {
            pNode->Shdr_Autoltm2_Unity_Gain(stats->bin, MAX_SHDR_BHIST_STATS_SIZE,
            &(p_shdr_ctx->autoltm2_param), &(p_shdr_ctx->g_shdr_k2_8bit));

       }
        CHIMETADATAINFO        metadataInfo     = {0};
        const UINT32           tagSize          = sizeof(g_VendorTagSectionDummyShdr) / sizeof(g_VendorTagSectionDummyShdr[0]);
        CHITAGDATA             tagData[tagSize] = { {0} };
        UINT32                 tagList[tagSize];
        metadataInfo.size       = sizeof(CHIMETADATAINFO);
        metadataInfo.chiSession = pNode->m_hChiSession;
        metadataInfo.tagNum     = tagSize;
        metadataInfo.pTagList   = &tagList[0];
        metadataInfo.pTagData   = &tagData[0];

        UINT32 index = 0;
        tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = *FrameIdinAECStatsReadyCallback;
        tagData[index].pData        = (pNode->m_bestats);
        tagData[index].dataSize     = g_VendorTagSectionDummyShdr[index].numUnits;
        index++;
        tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = *FrameIdinAECStatsReadyCallback;
        tagData[index].pData        = (pNode->m_bhist);
        tagData[index].dataSize     = g_VendorTagSectionDummyShdr[index].numUnits;
        index++;
        tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = *FrameIdinAECStatsReadyCallback;
        tagData[index].pData        = (pNode->m_bgbeconfig);
        tagData[index].dataSize     = g_VendorTagSectionDummyShdr[index].numUnits;

        result = g_ChiNodeInterface.pSetMetadata(&metadataInfo);

        if (CDKResultSuccess != result)
        {
            LOG_ERROR(CamxLogGroupChi, "pSetMetadata is NULL");
        }

    }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyShdrNode::ChiDummyShdrNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiDummyShdrNode::ChiDummyShdrNode()
    : m_hChiSession(NULL)
    , m_nodeId(0)
    , m_nodeCaps(0)
    , m_processedFrame(0)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyShdrNode::~ChiDummyShdrNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiDummyShdrNode::~ChiDummyShdrNode()
{
    if (m_p_shdr_ctx->handle)
    {
        for (INT i = 0; i < num_gamma_range; i++)
        {
            if (NULL != GTM_GAMMA[i])
            {
                free(GTM_GAMMA[i]);
            }
        }
        if (NULL != GTM_GAMMA)
        {
            free(GTM_GAMMA);
        }
        m_shdr_lib_deinit(m_p_shdr_ctx->handle);
        m_p_shdr_ctx->handle = NULL;
    }

    delete m_p_shdr_ctx;
    m_p_shdr_ctx = NULL;

    delete m_bestats;
    m_bestats = NULL;

    delete m_bhist;
    m_bhist = NULL;

    delete m_bgbeconfig;
    m_bgbeconfig = NULL;

    m_hChiSession = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyShdrNode::LoadLib
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummyShdrNode::LoadLib()
{
    CDKResult result = CDKResultSuccess;

    m_hShdrLib = ChiNodeUtils::LibMap(pLibName, pLibPath);

    if (NULL == m_hShdrLib)
    {
        LOG_ERROR(CamxLogGroupChi, "Error loading lib %s", pLibName);
        result = CDKResultEUnableToLoad;
    }

    ///< Map function pointers
    if (CDKResultSuccess == result)
    {
        m_shdr_lib_init = reinterpret_cast<SHDRLIBINIT>(ChiNodeUtils::LibGetAddr(m_hShdrLib,
                                                                                               "shdr_lib_init"));

        m_shdr_lib_get_caps = reinterpret_cast<SHDRLIBGETCAPS>(ChiNodeUtils::LibGetAddr(m_hShdrLib,
                                                                                               "shdr_lib_get_caps"));

        m_shdr_lib_map_buffer = reinterpret_cast<SHDRLIBMAPBUFFER>(ChiNodeUtils::LibGetAddr(m_hShdrLib,
                                                                                               "shdr_map_buffer"));

        m_shdr_lib_process = reinterpret_cast<SHDRLIBPROCESS>(ChiNodeUtils::LibGetAddr(m_hShdrLib,
                                                                                               "shdr_lib_process"));

        m_shdr_lib_deinit = reinterpret_cast<SHDRLIBDEINIT>(ChiNodeUtils::LibGetAddr(m_hShdrLib,
                                                                                                 "shdr_lib_deinit"));

        if ((NULL == m_shdr_lib_init)       ||
            (NULL == m_shdr_lib_get_caps)   ||
            (NULL == m_shdr_lib_map_buffer) ||
            (NULL == m_shdr_lib_process)    ||
            (NULL == m_shdr_lib_deinit))
        {
            LOG_ERROR(CamxLogGroupChi,
                      "Error Initializing one or more function pointers in Library");
            result = CDKResultENoSuch;
        }
    }
    return result;
}

 VOID ChiDummyShdrNode::Autoltm2_Linear_CDF_Gen(
  float* cdf,
  int hist_size,
  int target_luma,
  float sigma) {

  CDK_UNUSED_PARAM(target_luma);
  CDK_UNUSED_PARAM(sigma);

  int x = 0;

  float* hist = (float*)malloc(hist_size * sizeof(int));

  for (x = 0; x < hist_size; x++) {
    hist[x] = 1.0f;
  }

  cdf[0] = hist[0];
  for (x = 1; x < hist_size; x++) {
    cdf[x] = cdf[x - 1] + hist[x];
  }

  float scale = 1.0f / cdf[hist_size - 1];
  for (x = 0; x < hist_size; x++) {
    hist[x] = hist[x] * scale;
  }

  cdf[0] = hist[0];
  for (x = 1; x < hist_size; x++) {
    cdf[x] = cdf[x - 1] + hist[x];
  }
  cdf[0] = 0;

  free(hist);

}

VOID ChiDummyShdrNode::Autoltm2_Gaussian_CDF_Gen(
  float* cdf,
  int hist_size,
  int target_luma,
  float sigma) {

  int x = 0;
  float* hist = (float*)malloc(hist_size * sizeof(int));
  float tmp = 0;

  for (x = 0; x < hist_size; x++) {
    tmp = (float)x - (float)target_luma;
    tmp = tmp / sigma;
    tmp = -1.0f * tmp * tmp;
    hist[x] = (float)exp(tmp);
  }

  cdf[0] = hist[0];
  for (x = 1; x < hist_size; x++) {
    cdf[x] = cdf[x - 1] + hist[x];
  }

  float scale = 1.0f / cdf[hist_size - 1];
  for (x = 0; x < hist_size; x++) {
    hist[x] = hist[x] * scale;
  }

  cdf[0] = hist[0];
  for (x = 1; x < hist_size; x++) {
    cdf[x] = cdf[x - 1] + hist[x];
  }
  cdf[0] = 0;

  free(hist);

}

float ChiDummyShdrNode::Calc_Gtm_Gamma(float expRatio)
{
    float interpolated_gtm_gamma = 0.0;
    if (expRatio <= GTM_GAMMA[0][0])
    {
        return GTM_GAMMA[0][2];
    }
    if (expRatio >= GTM_GAMMA[num_gamma_range - 1][0])
    {
        return GTM_GAMMA[num_gamma_range - 1][2];
    }
    for (INT i = 0; i < num_gamma_range; i++)
    {
        if (expRatio < GTM_GAMMA[i][0])
        {
            if (expRatio <= GTM_GAMMA[i-1][1])
            {
                return GTM_GAMMA[i-1][2];
            }
            else
            {
                //interpolate the expRatio
                float ratio              = INTERPOLATE_GAMMA_RATIO(GTM_GAMMA[i-1][1],
                                                GTM_GAMMA[i][1], expRatio);
                interpolated_gtm_gamma   = LINEAR_INTERPOLATION(GTM_GAMMA[i][2],
                                                GTM_GAMMA[i-1][2], ratio);
                break;
            }
        }
    }
    return interpolated_gtm_gamma;
}

VOID ChiDummyShdrNode::Autoltm2_Histogram_Matching(
  float* cdf2,
  float* hist,
  int hist_size,
  float* gain_lut,
  float* b8bit,
  float* gain_max,
  float gain_limit) {

  int x = 0, y = 0;
  unsigned int total_pixel = 0;
  float temp_y = 0;
  float delta = 0;
  float* cdf1 = (float*)malloc(hist_size * sizeof(float));
  for (x = 0; x < hist_size; x++) {
    cdf1[x] = hist[x];
    total_pixel += cdf1[x];
  }

  for (x = 1; x < hist_size; x++) {
    cdf1[x] = cdf1[x] + cdf1[x - 1];
  }

  for (x = 0; x < hist_size; x++) {
    temp_y = ((float)cdf1[x]) / (float)total_pixel;

    for (y = 1; y < hist_size; y++) {
      if (cdf2[y] > temp_y) {
        goto BREAK;
      }
    }

    if (y == hist_size) y = (hist_size - 1);

  BREAK:
    delta = cdf2[y] - cdf2[y - 1];
    if (delta == 0)
      temp_y = y;
    else
      temp_y = y - (cdf2[y] - temp_y) / delta;

    temp_y = temp_y / (x + 1);

    gain_lut[x] = (temp_y);

    if (gain_lut[x] < 1.0f) gain_lut[x] = 1.0f;
    if (gain_lut[x] > gain_limit) gain_lut[x] = gain_limit;
  }

  free(cdf1);

  float max_gain = 0.0f;
  for (x = 0; x < 40; x++) {
    if (gain_lut[x] > max_gain) {
      max_gain = gain_lut[x];
    }
  }

  for (x = 0; x < hist_size; x++) {
    gain_lut[x] = gain_lut[x] / max_gain;
  }

  if (max_gain < 1.1f) max_gain = 1.1f;

  *gain_max = max_gain;

  float log_0p5 = log(0.5f);
  float offset = log(max_gain) / log_0p5;
  float b = *b8bit;
  float tmp = 0;

  for (x = 40; x < 120; x++) {
    tmp = gain_lut[x];
    tmp = log(tmp) / log_0p5;
    tmp = tmp - offset;
    tmp = tmp / (float)x;
    b = b + tmp;
  }
  b = b / 80.0f;
  *b8bit = b;

  for (x = 0; x < hist_size; x++) {
    gain_lut[x] = max_gain * (float)pow(0.5, (b * (float)x));
  }

  LOG_VERBOSE(CamxLogGroupChi,"autoltm2_histogram_matching -X");
}

/** shdr_autoltm2_unity_gain
 *    @bhist: shdr library handle
 *    @bin_size: configuration params for shdr
 *    @auto_ltm: auto ltm tuning
 *    @k2_8bit: k2 8bit
 *
 * Calculates k2
 *
 * return: 0 on success
 **/
VOID ChiDummyShdrNode::Shdr_Autoltm2_Unity_Gain(
  float* bhist,
  int bhist_size,
  SHDRAUTOLTMTUNINGPARAMS* autoltm2,
  float* k2_8bit) {

  // local variable
  int l_flat_hist_en = autoltm2->flat_hist_en;
  float l_luma = autoltm2->gsn_luma_target;
  float l_sigma = autoltm2->gsn_sigma;
  float l_max_gain_limit = autoltm2->max_gain_limit;
  float l_weight = autoltm2->k2_8bit_pre_weight;
  float k2_8bit_pre = *k2_8bit;
  float k2_8bit_cur = 0.0f;
  float b_8bit = 0.0f;
  float max_gain = 0.0f;
  float constant = 1.0f;

  // local array
  float* cdf = (float*)malloc(bhist_size * sizeof(int));
  float* gain_lut = (float*)malloc(bhist_size * sizeof(int));

  // initialize
  memset(cdf, 0, bhist_size * sizeof(int));
  memset(gain_lut, 0, bhist_size * sizeof(int));

  if (l_flat_hist_en) {
    Autoltm2_Linear_CDF_Gen(cdf, bhist_size, l_luma, l_sigma);
  } else {
    Autoltm2_Gaussian_CDF_Gen(cdf, bhist_size, l_luma, l_sigma);
  }

  Autoltm2_Histogram_Matching(cdf, bhist, bhist_size, gain_lut, &b_8bit,
                              &max_gain, l_max_gain_limit);

  constant = b_8bit * log(0.5);
  constant = (constant == 0) ? 1 : constant;

  k2_8bit_cur = -log(max_gain) / constant;
  k2_8bit_cur = (k2_8bit_cur < 0) ? 0 : (k2_8bit_cur);

  *k2_8bit = (l_weight) * k2_8bit_pre + (1.0 - l_weight) * k2_8bit_cur;

  free(cdf);
  free(gain_lut);

  LOG_VERBOSE(CamxLogGroupChi,"shdr_autoltm2_unity_gain-X");

}


// CAMX_NAMESPACE_END
