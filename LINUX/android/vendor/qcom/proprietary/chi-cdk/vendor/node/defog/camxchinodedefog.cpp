////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodesvhdr.cpp
/// @brief Chi node for DummyDefog
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <inttypes.h>
#include <system/camera_metadata.h>

#include "chiispstatsdefs.h"
#include "camxchinodedefog.h"
#include <tuningsetmanager.h>
#include "chiaecinterface.h"
#include <chituningmodeparam.h>
#include "parametertuningtypes.h"
#include "parametertypes.h"

// NOWHINE FILE CP040: Keyword new not allowed. Use the CAMX_NEW/CAMX_DELETE functions insteads
// NOWHINE FILE NC008: Warning: - Var names should be lower camel case

#undef LOG_TAG
#define LOG_TAG "CHIDEFOG"

#define PRINT_METADATA TRUE

ChiNodeInterface g_ChiNodeInterface;    ///< The instance save the CAMX Chi interface
UINT32           g_vendorTagBase = 0;   ///< Chi assigned runtime vendor tag base for the node

const CHAR* pLibName = "libdefog";

#if defined (_LINUX)
const CHAR* pLibPath = "/vendor/lib/camera/components/";
const CHAR* pFileDumpPath = "/data/misc/camera/";
#endif // _LINUX

#if defined (_WIN32)
const CHAR* pLibPath = ".\\";
const CHAR* pFileDumpPath = ".\\";
#endif // _WIN32

/// @todo (CAMX-1854) the major / minor version shall get from CHI
static const UINT32 ChiNodeMajorVersion             = 0;                         ///< The major version of CHI interface
static const UINT32 ChiNodeMinorVersion             = 0;                         ///< The minor version of CHI interface

static const UINT32 ChiNodeCapDefog                 = ChiNodeCapsDefog;           ///< Simply do Defog operation

static const CHAR   DefogNodeSectionName[]          = "com.qti.node.defog";       ///< The section name for node

static const UINT32 DefogNodeTagBase             = 0;                         ///< Tag base
static const UINT32 DefogNodeTagANRStats         = DefogNodeTagBase + 0;  ///< Tag to send ANR stats
static const UINT32 DefogNodeTagLTMStats         = DefogNodeTagBase + 1;  ///< Tag to send LTM stats
static const UINT32 DefogNodeTagGammaStats       = DefogNodeTagBase + 2;  ///< Tag to send Gamma For stats
static const UINT32 DefogNodeTagASFStats         = DefogNodeTagBase + 3;  ///< Tag to send ASF For stats
static const UINT32 DefogNodeTagCVStats          = DefogNodeTagBase + 4;  ///< Tag to send CV For stats
static const UINT32 DefogNodeTagDEStats          = DefogNodeTagBase + 5;  ///< Tag to send DE data to SHDR3

 ///< Supported vendor tag list, it shall align with the definition in g_VendorTagSectionDummyShdr
static const UINT32 g_VendorTagList[] =
{
    DefogNodeTagANRStats,
    DefogNodeTagLTMStats,
    DefogNodeTagGammaStats,
    DefogNodeTagASFStats,
    DefogNodeTagCVStats,
    DefogNodeTagDEStats,
};


///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagSectionDefog[] =
{
    { "ANRStats",            TYPE_BYTE, sizeof(anr_1_0_0::mod_anr10_cct_dataType::cct_dataStruct) },
    { "LTMStats",            TYPE_BYTE, sizeof(ltm_1_3_0::ltm13_rgn_dataType) },
    { "GammaStats",          TYPE_BYTE, sizeof(gamma15_cct_dataType::cct_dataStruct) },
    { "ASFStats",            TYPE_BYTE, sizeof(asf_3_0_0::asf30_rgn_dataType) },
    { "CVStats",             TYPE_BYTE, sizeof(cv_1_2_0::cv12_rgn_dataType) },
    { "DEStats",             TYPE_BYTE, sizeof(shdr3_tuning_de_t) },
};


///< This is an array of all vendor tag section data
static CHIVENDORTAGSECTIONDATA g_VendorTagDefogSection[] =
{
    {
        DefogNodeSectionName,  0,
        sizeof(g_VendorTagSectionDefog) / sizeof(g_VendorTagSectionDefog[0]), g_VendorTagSectionDefog,
        CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    }
};


///< This is an array of all vendor tag section data
static ChiVendorTagInfo g_VendorTagInfoDefog[] =
{
    {
        &g_VendorTagDefogSection[0],
        sizeof(g_VendorTagDefogSection) / sizeof(g_VendorTagDefogSection[0])
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DefogNodeGetCaps
///
/// @brief  Implementation of PFNNODEGETCAPS defined in chinode.h
///
/// @param  pCapsInfo   Pointer to a structure that defines the capabilities supported by the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DefogNodeGetCaps(
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
            pCapsInfo->nodeCapsMask = ChiNodeCapDefog;
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
/// DefogNodeQueryVendorTag
///
/// @brief  Implementation of PFNCHIQUERYVENDORTAG defined in chinode.h
///
/// @param  pQueryVendorTag Pointer to a structure that returns the exported vendor tag
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DefogNodeQueryVendorTag(
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
        result = ChiNodeUtils::QueryVendorTag(pQueryVendorTag, g_VendorTagInfoDefog);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DefogNodeCreate
///
/// @brief  Implementation of PFNNODECREATE defined in chinode.h
///
/// @param  pCreateInfo Pointer to a structure that defines create session information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DefogNodeCreate(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult       result  = CDKResultSuccess;
    ChiDefogNode*  pNode   = NULL;

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
        pNode = new ChiDefogNode;
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
/// DefogNodeDestroy
///
/// @brief  Implementation of PFNNODEDESTROY defined in chinode.h
///
/// @param  pDestroyInfo    Pointer to a structure that defines the session destroy information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DefogNodeDestroy(
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
            ChiDefogNode* pNode = static_cast<ChiDefogNode*>(pDestroyInfo->hNodeSession);
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
/// DefogNodeQueryBufferInfo
///
/// @brief  Implementation of PFNNODEQUERYBUFFERINFO defined in chinode.h
///
/// @param  pQueryBufferInfo    Pointer to a structure to query the input buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DefogNodeQueryBufferInfo(
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
            ChiDefogNode* pNode = static_cast<ChiDefogNode*>(pQueryBufferInfo->hNodeSession);
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
/// DefogNodeSetBufferInfo
///
/// @brief  Implementation of PFNNODESETBUFFERINFO defined in chinode.h
///
/// @param  pSetBufferInfo  Pointer to a structure with information to set the output buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DefogNodeSetBufferInfo(
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
            ChiDefogNode* pNode = static_cast<ChiDefogNode*>(pSetBufferInfo->hNodeSession);
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
/// DefogNodeProcRequest
///
/// @brief  Implementation of PFNNODEPROCREQUEST defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DefogNodeProcRequest(
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
            ChiDefogNode* pNode = static_cast<ChiDefogNode*>(pProcessRequestInfo->hNodeSession);
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
/// DefogNodeSetNodeInterface
///
/// @brief  Implementation of PFCHINODESETNODEINTERFACE defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID DefogNodeSetNodeInterface(
    ChiNodeInterface* pNodeInterface)
{
    CDKResult result = CDKResultSuccess;
    result = ChiNodeUtils::SetNodeInterface(pNodeInterface, DefogNodeSectionName,
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
            pNodeCallbacks->pGetCapabilities         = DefogNodeGetCaps;
            pNodeCallbacks->pQueryVendorTag          = DefogNodeQueryVendorTag;
            pNodeCallbacks->pCreate                  = DefogNodeCreate;
            pNodeCallbacks->pDestroy                 = DefogNodeDestroy;
            pNodeCallbacks->pQueryBufferInfo         = DefogNodeQueryBufferInfo;
            pNodeCallbacks->pSetBufferInfo           = DefogNodeSetBufferInfo;
            pNodeCallbacks->pProcessRequest          = DefogNodeProcRequest;
            pNodeCallbacks->pChiNodeSetNodeInterface = DefogNodeSetNodeInterface;
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
/// ChiDefogNode::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDefogNode::Initialize(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult result = CDKResultSuccess;

    m_hChiSession    = pCreateInfo->hChiSession;
    m_nodeId         = pCreateInfo->nodeId;
    m_nodeCaps       = pCreateInfo->nodeCaps.nodeCapsMask;
    m_nodeFlags      = pCreateInfo->nodeFlags;
    m_processedFrame = 0;
    result           = GetVendorTagBases();
    m_handle         = NULL;
    isSHDRSupported  = FALSE;

    if (CDKResultSuccess == result)
    {
        // Load lib after library is ready
        result       = LoadLib();
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Failed to get Vendor tag bases");
    }

    if (CDKResultSuccess == result)
    {
        m_handle = m_defog_lib_init();  //Initialize make it NULL ; Destroy make it NULL

        if (NULL != m_handle)
        {
            // Attach pointers
            m_DefogParams.param_update_mask  = DefogConfParamGroupNone;
            m_DefogParams.defog_en           = 0;
            m_DefogParams.ce_en              = 0;
            m_p_DefogInternalData = (DefogInternalData*)m_handle;
        }
        m_p_DefogConfigData = new DefogData;
        if (NULL == m_p_DefogConfigData)
        {
            LOG_ERROR(CamxLogGroupChi, "No memory for defog data");
            result = CDKResultENoMemory;
        }
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Load DEFOG algo lib failed");
    }
    memset(&m_DefogParams, 0, sizeof(DefogConfigParams));
    memset(&m_PrevDefogParams, 0, sizeof(PrevDefogConfigParams));

    m_DefogParams.param_update_mask     = DefogConfParamGroupAll;
    result = m_defog_lib_getparams(m_handle, &m_DefogParams);

    if (CDKResultSuccess == result)
    {
        memcpy(&m_PrevDefogParams, &m_DefogParams, sizeof(DefogConfigParams));
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Get DefogParams failed");
    }

    m_DefogParams.param_update_mask      = DefogConfParamGroupNone;
    m_PrevDefogParams.param_update_mask  = DefogConfParamGroupNone;
    m_p_DefogConfigData->defogOutputData.isSettled = 0;
    m_p_DefogConfigData->defogOutputData.fog_scene_probability = 0;
    m_p_DefogConfigData->defogInputData.pLTM13ReserveData = (ltm_1_3_0::chromatix_ltm13_reserveType*)malloc(sizeof(ltm_1_3_0::chromatix_ltm13_reserveType));
    memset(m_p_DefogConfigData->defogInputData.pLTM13ReserveData, 0, sizeof(ltm_1_3_0::chromatix_ltm13_reserveType));
    m_p_DefogConfigData->defogOutputData.pProcessedANR10Data = (anr_1_0_0::mod_anr10_cct_dataType::cct_dataStruct*)malloc(sizeof(anr_1_0_0::mod_anr10_cct_dataType::cct_dataStruct));
    memset(m_p_DefogConfigData->defogOutputData.pProcessedANR10Data, 0, sizeof(anr_1_0_0::mod_anr10_cct_dataType::cct_dataStruct));
    m_p_DefogConfigData->defogOutputData.pProcessedLTM13Data = (ltm_1_3_0::ltm13_rgn_dataType*)malloc(sizeof(ltm_1_3_0::ltm13_rgn_dataType));
    memset(m_p_DefogConfigData->defogOutputData.pProcessedLTM13Data, 0, sizeof(ltm_1_3_0::ltm13_rgn_dataType));
    m_p_DefogConfigData->defogOutputData.pProcessedGamma15Data = (gamma15_cct_dataType::cct_dataStruct*)malloc(sizeof(gamma15_cct_dataType::cct_dataStruct));
    memset( m_p_DefogConfigData->defogOutputData.pProcessedGamma15Data, 0, sizeof(gamma15_cct_dataType::cct_dataStruct));
    m_p_DefogConfigData->defogOutputData.pProcessedCV12Data = (cv_1_2_0::cv12_rgn_dataType*)malloc(sizeof(cv_1_2_0::cv12_rgn_dataType));
    memset(m_p_DefogConfigData->defogOutputData.pProcessedCV12Data, 0, sizeof(cv_1_2_0::cv12_rgn_dataType));
    m_p_DefogConfigData->defogOutputData.pProcessedASF30Data = (asf_3_0_0::asf30_rgn_dataType*)malloc(sizeof(asf_3_0_0::asf30_rgn_dataType));
    memset( m_p_DefogConfigData->defogOutputData.pProcessedASF30Data, 0, sizeof(asf_3_0_0::asf30_rgn_dataType));
    m_p_DefogConfigData->defogInputData.pInterpolationDEData = (shdr3_tuning_de_t*)malloc(sizeof(shdr3_tuning_de_t));
    memset(m_p_DefogConfigData->defogInputData.pInterpolationDEData, 0, sizeof(shdr3_tuning_de_t));
    m_p_DefogConfigData->defogInputData.isADRCEnabled = FALSE;

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDefogNode::QueryBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDefogNode::QueryBufferInfo(
    CHINODEQUERYBUFFERINFO* pQueryBufferInfo)
{
    CDKResult result                     = CDKResultSuccess;

    ChiNodeUtils::DefaultBufferNegotiation(pQueryBufferInfo);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDefogNode::SetBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDefogNode::SetBufferInfo(
    CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo)
{
    CDKResult result = CDKResultSuccess;
    m_format.width  = pSetBufferInfo->pFormat->width;
    m_format.height = pSetBufferInfo->pFormat->height;
    CHIDATASOURCE chiDataSource;
    chiDataSource.dataSourceType     = ChiTuningManager;
    chiDataSource.pHandle            = NULL;
    TuningSetManager* pTuningManager = static_cast<TuningSetManager*>(g_ChiNodeInterface.pGetData(m_hChiSession,
                                                     &chiDataSource,
                                                     NULL,
                                                     NULL));
    if (NULL != pTuningManager)
    {
        tmc_1_0_0::chromatix_tmc10Type* ptmcChromatix = NULL;
        ltm_1_3_0::chromatix_ltm13Type* m_pChromatix  = NULL;
        shdr_3_0_0::shdr30Type* pChromatixYUVSHDR     = NULL;
        ChiTuningModeParameter TMCTuningData;
        TMCTuningData.noOfSelectionParameter = 1;
        TMCTuningData.TuningMode[0].mode          = ChiModeType::Default;
        TMCTuningData.TuningMode[0].subMode       = { 0 };
        BOOL pAdrcEnabled   = FALSE;
        ptmcChromatix = pTuningManager->GetModule_tmc10_sw(
                            reinterpret_cast<TuningMode*>(&TMCTuningData.TuningMode[0]),
                            (sizeof(TMCTuningData.TuningMode)/sizeof(TMCTuningData.TuningMode[0])));

        m_pChromatix = pTuningManager->GetModule_ltm13_ipe(
                             reinterpret_cast<TuningMode*>(&TMCTuningData.TuningMode[0]),
                            (sizeof(TMCTuningData.TuningMode)/sizeof(TMCTuningData.TuningMode[0])));

        pChromatixYUVSHDR = pTuningManager->GetModule_shdr30(
                                reinterpret_cast<TuningMode*>(&TMCTuningData.TuningMode[0]),
                                (sizeof(TMCTuningData.TuningMode)/sizeof(TMCTuningData.TuningMode[0])));

        if (NULL != ptmcChromatix)
        {
            if (TRUE == ptmcChromatix->enable_section.adrc_isp_enable)
            {
                m_p_DefogConfigData->defogInputData.isADRCEnabled = TRUE;
            }
            if(TRUE == ptmcChromatix->chromatix_tmc10_reserve.use_ltm)
            {
                m_p_DefogConfigData->defogInputData.ltm_percentage =
                    ptmcChromatix->chromatix_tmc10_core.mod_tmc10_aec_data->tmc10_rgn_data.ltm_percentage;
            }
        }
        if (NULL != m_pChromatix)
        {
            memcpy(m_p_DefogConfigData->defogInputData.pLTM13ReserveData,
                        &(m_pChromatix->chromatix_ltm13_reserve),sizeof(ltm_1_3_0::chromatix_ltm13_reserveType));
        }
        if (NULL != pChromatixYUVSHDR)
        {
            memcpy(m_p_DefogConfigData->defogInputData.pInterpolationDEData,
                        &(pChromatixYUVSHDR->Detail_Enhancement),sizeof(shdr3_tuning_de_t));
        }
    }
    LOG_ERROR(CamxLogGroupChi, "ADRC Enabled = %d LTM Percentage %f", m_p_DefogConfigData->defogInputData.isADRCEnabled,
                                m_p_DefogConfigData->defogInputData.ltm_percentage);

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDefogNode::CheckDependency
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChiDefogNode::CheckDependency(
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

    tag = m_AWBDataVendorTagBase;
    pData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum, tag, ChiMetadataDynamic,
                                      &g_ChiNodeInterface, m_hChiSession);
    // if the dependent tag is not published yet, set it as dependency
    if (NULL == pData)
    {
        pDependencyInfo->properties[dependencyCount]    = tag;
        pDependencyInfo->offsets[dependencyCount]       = 0;
        dependencyCount++;
    }

    tag = m_Gamma16VendorTagBase;
    pData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum, tag, ChiMetadataDynamic,
                                      &g_ChiNodeInterface, m_hChiSession);
    // if the dependent tag is not published yet, set it as dependency
    if (NULL == pData)
    {
        pDependencyInfo->properties[dependencyCount]    = tag;
        pDependencyInfo->offsets[dependencyCount]       = 0;
        dependencyCount++;
    }


    tag = m_Gamma15VendorTagBase;
    pData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum, tag, ChiMetadataDynamic,
                                      &g_ChiNodeInterface, m_hChiSession);
    // if the dependent tag is not published yet, set it as dependency
    if (NULL == pData)
    {
        pDependencyInfo->properties[dependencyCount]    = tag;
        pDependencyInfo->offsets[dependencyCount]       = 0;
        dependencyCount++;
    }

    if (FALSE == isSHDRSupported)
    {
        tag = m_IHISTDataVendorTagBase;
        pData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum, tag, ChiMetadataDynamic,
                                          &g_ChiNodeInterface, m_hChiSession);
        // if the dependent tag is not published yet, set it as dependency
        if (NULL == pData)
        {
            pDependencyInfo->properties[dependencyCount]    = tag;
            pDependencyInfo->offsets[dependencyCount]       = 0;
            dependencyCount++;
        }
    }
    else
    {
        tag = m_SHDRIHISTDataVendorTagBase;
        pData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum, tag, ChiMetadataDynamic,
                                          &g_ChiNodeInterface, m_hChiSession);
        // if the dependent tag is not published yet, set it as dependency
        if (NULL == pData)
        {
            pDependencyInfo->properties[dependencyCount]    = tag;
            pDependencyInfo->offsets[dependencyCount]       = 0;
            dependencyCount++;
        }
    }

    tag = m_LTMDataVendorTagBase;
    pData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum, tag, ChiMetadataDynamic,
                                      &g_ChiNodeInterface, m_hChiSession);
    // if the dependent tag is not published yet, set it as dependency
    if (NULL == pData)
    {
        pDependencyInfo->properties[dependencyCount]    = tag;
        pDependencyInfo->offsets[dependencyCount]       = 0;
        dependencyCount++;
    }

    tag = m_ASFDataVendorTagBase;
    pData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum, tag, ChiMetadataDynamic,
                                      &g_ChiNodeInterface, m_hChiSession);
    // if the dependent tag is not published yet, set it as dependency
    if (NULL == pData)
    {
        pDependencyInfo->properties[dependencyCount]    = tag;
        pDependencyInfo->offsets[dependencyCount]       = 0;
        dependencyCount++;
    }
    tag = m_CVDataVendorTagBase;
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
/// ChiDefogNode::ProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDefogNode::ProcessRequest(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{
    CDK_UNUSED_PARAM(pProcessRequestInfo);  //to be changed
    CDKResult result   = CDKResultSuccess;
    BOOL satisfied     = FALSE;
    BOOL defogOn       = FALSE;
    BOOL paramsChanged = FALSE;
    UINT32 framenum    = (UINT32)pProcessRequestInfo->frameNum; //remove

/**************************************
1. Read if Defog Meta is On or not ?
2. if off copy input to output directly
3. if on call lib
**************************************/
    VOID*  pDefogEnableMeta      = NULL;
    VOID*  pCEEnableMeta         = NULL;
    // Get App Defog Data
    // currently per frame but should be done only if meta changes
    // need to store previous meta
    memset(&m_DefogParams, 0, sizeof(DefogConfigParams));
    pDefogEnableMeta = ChiNodeUtils::GetMetaData(framenum,
                                            (m_DefogEnableVendorTagBase)|InputMetadataSectionMask,
                                            ChiMetadataDynamic,
                                            &g_ChiNodeInterface,
                                            m_hChiSession);
    if (NULL != pDefogEnableMeta)
    {
        // m_p_DefogConfigData->defogInputData.pISPIQTriggerData->pAECMeta = reinterpret_cast<AECFrameControl*>(pAECMetaData);
        m_DefogParams.defog_en = *reinterpret_cast<BOOL*>(pDefogEnableMeta);
    }
    else
    {
        m_DefogParams.defog_en = m_p_DefogInternalData->defogConfigData.defog_en;
    }

    pCEEnableMeta = ChiNodeUtils::GetMetaData(framenum,
                                            (m_DefogAlgoEnableCETagBase)|InputMetadataSectionMask,
                                            ChiMetadataDynamic,
                                            &g_ChiNodeInterface,
                                            m_hChiSession);
    if (NULL != pCEEnableMeta)
    {
        m_DefogParams.ce_en = *reinterpret_cast<BOOL*>(pCEEnableMeta);
    }
    else
    {
        m_DefogParams.ce_en = m_p_DefogInternalData->defogConfigData.ce_en;
    }

    defogOn = m_DefogParams.defog_en | m_DefogParams.ce_en;
    LOG_VERBOSE(CamxLogGroupChi, "ProcessRequest frame_num = %d defog %d", framenum, defogOn);

    VOID*  pSensorinfo      = NULL;
    pSensorinfo = ChiNodeUtils::GetMetaData(0,
                                      (m_sensorModeInfoVendorTagBase | UsecaseMetadataSectionMask),
                                      ChiMetadataDynamic,
                                      &g_ChiNodeInterface,
                                      m_hChiSession);
    if (NULL != pSensorinfo)
    {
        ChiSensorModeInfo* pSensorModeInfo = reinterpret_cast<ChiSensorModeInfo*>(pSensorinfo);
        m_p_DefogConfigData->defogInputData.frameRate = pSensorModeInfo->frameRate;
        isSHDRSupported = (1 == pSensorModeInfo->sensorModeCaps.u.SHDR);
        m_p_DefogConfigData->defogInputData.isSHDREnabled  = isSHDRSupported;
        LOG_INFO(CamxLogGroupChi, "Sensor frame rate %d SHDR : %d",
            pSensorModeInfo->frameRate, isSHDRSupported);
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Error getting sensor info");
    }
    if (IsRealTime())
    {
        if(TRUE == defogOn)
        {
           satisfied = CheckDependency(pProcessRequestInfo);
        }
    }
    else
    {
        satisfied = TRUE;
    }
    LOG_VERBOSE(CamxLogGroupChi, "satisfied = %d", satisfied);
    if (TRUE == satisfied)
    {
        anr_1_0_0::mod_anr10_cct_dataType::cct_dataStruct     anrInterpolationMetaData;
        ltm_1_3_0::ltm13_rgn_dataType                         ltmInterpolationMetaData;
        gamma15_cct_dataType::cct_dataStruct                  gamma15InterpolationMetaData;
        asf_3_0_0::asf30_rgn_dataType                         asfInterpolationMetaData;
        cv_1_2_0::cv12_rgn_dataType                           cvInterpolationMetaData;
        paramsChanged = SetDeFogConfigParams(pProcessRequestInfo->frameNum);
        // no need to call perframe if value is not changed.
        if (TRUE == paramsChanged)
        {
            m_defog_lib_setparams(m_handle, &m_DefogParams);
        }

        m_p_DefogConfigData->defogInputData.frameNum = framenum;
        m_p_DefogConfigData->defogInputData.DefogInputType = DefogInputTypeGroupNone; //As all the meta is ready OR it and send
        // m_p_DefogConfigData->defogInputData.DefogInputType =
        //    DefogInputTypeGroupAECStats|DefogInputTypeGroupAWBStats|DefogInputTypeGroupIHistStats|
        //    DefogInputTypeGroupDS4|DefogInputTypeGroupInterANR|DefogInputTypeGroupInterLTM|
        //   DefogInputTypeGroupInterGamma|DefogInputTypeGroupInterASF;
        // set dependency on these tags
        VOID*  pAECMetaData      = NULL;
        FLOAT  expRatio          = 1.0F;
        // Get AEC info
        pAECMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          (m_AECDataVendorTagBase),
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
        if (NULL != pAECMetaData)
        {
            m_p_DefogConfigData->defogInputData.AECStatsdata = reinterpret_cast<AECFrameControl*>(pAECMetaData);
            AECFrameControl* pAECData = reinterpret_cast<AECFrameControl*>(pAECMetaData);
            if (TRUE == isSHDRSupported)
            {
                if ((pAECData->exposureInfo[AECAlgoExposureLong].exposureTime != 0.0) &&
                    (pAECData->exposureInfo[AECAlgoExposureShort].exposureTime != 0.0) &&
                    (pAECData->realExposureInfo[AECAlgoExposureLong].exposureTime != 0.0) &&
                    (pAECData->realExposureInfo[AECAlgoExposureShort].exposureTime != 0.0))
                {
                        expRatio = (FLOAT)((FLOAT)pAECData->exposureInfo[AECAlgoExposureLong].exposureTime /
                                    (FLOAT)pAECData->exposureInfo[AECAlgoExposureShort].exposureTime);
                    LOG_INFO(CamxLogGroupChi, "AEC ReqId : %d and expRatio:%f ", (int)pProcessRequestInfo->frameNum, expRatio);
                    LOG_VERBOSE(CamxLogGroupChi, "Lexposure : %lld and Sexposure:%lld",
                        (long long int)pAECData->exposureInfo[AECAlgoExposureLong].exposureTime,
                        (long long int)pAECData->exposureInfo[AECAlgoExposureShort].exposureTime);
                    LOG_INFO(CamxLogGroupChi, "AEC sensor ratio=%f, real ratio=%f",
                            (FLOAT)((FLOAT)pAECData->exposureInfo[AECAlgoExposureLong].exposureTime /(FLOAT)pAECData->exposureInfo[AECAlgoExposureShort].exposureTime),
                            (FLOAT)((FLOAT)pAECData->realExposureInfo[AECAlgoExposureLong].exposureTime /(FLOAT)pAECData->realExposureInfo[AECAlgoExposureShort].exposureTime));
                    m_p_DefogConfigData->defogInputData.sHDRRatio = expRatio;
                }
            }
        }
        else
        {
            LOG_INFO(CamxLogGroupChi, "AEC Meta is NULL");
        }
        VOID*  pADRCinfo      = NULL;
        // Get ADRC info
        pADRCinfo = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          m_ADRCinfoVendorTagBase,
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
        if (NULL != pADRCinfo)
        {
            if(*reinterpret_cast<BOOL*>(pADRCinfo) == FALSE)
            {
                m_p_DefogConfigData->defogInputData.isADRCEnabled = TRUE;
            }
            LOG_VERBOSE(CamxLogGroupChi, "ADRC Enabled %d", m_p_DefogConfigData->defogInputData.isADRCEnabled);
        }
        else
        {
            LOG_INFO(CamxLogGroupChi, "ADRC Meta is NULL");
        }
        VOID*  pAECState      = NULL;
        // Get AEC info
        pAECState = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          ANDROID_CONTROL_AE_STATE,
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
        if (NULL != pAECState)
        {
            if(*reinterpret_cast<BOOL*>(pAECState) == ANDROID_CONTROL_AE_STATE_CONVERGED)
            {
                m_p_DefogConfigData->defogInputData.isAECSettled = TRUE;
            }
            else
            {
                m_p_DefogConfigData->defogInputData.isAECSettled = FALSE;
            }
            LOG_VERBOSE(CamxLogGroupChi, "isAECSettled %d", m_p_DefogConfigData->defogInputData.isAECSettled);
        }
        else
        {
            LOG_INFO(CamxLogGroupChi, "AEC Meta is NULL");
        }
        VOID*  pAWBState      = NULL;
        // Get AEC info
        pAWBState = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          ANDROID_CONTROL_AWB_STATE,
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
        if (NULL != pAWBState)
        {
            if(*reinterpret_cast<UINT32*>(pAWBState) == ANDROID_CONTROL_AWB_STATE_CONVERGED)
            {
                m_p_DefogConfigData->defogInputData.isAWBSettled = TRUE;
            }
            else
            {
                m_p_DefogConfigData->defogInputData.isAWBSettled = FALSE;
            }
            LOG_VERBOSE(CamxLogGroupChi, "isAWBSettled %d", m_p_DefogConfigData->defogInputData.isAWBSettled);
        }
        else
        {
            LOG_INFO(CamxLogGroupChi, "AWB Meta is NULL");
        }

        VOID*  pAWBMetaData      = NULL;
        // Get AWB info
        pAWBMetaData = ReadMetaData(pProcessRequestInfo->frameNum, m_AWBDataVendorTagBase, "m_AWBDataVendorTagBase", ResultPool);
        m_p_DefogConfigData->defogInputData.AWBStatsdata = 
            ((NULL != pAWBMetaData) ? reinterpret_cast<AWBFrameControl*>(pAWBMetaData) : m_p_DefogConfigData->defogInputData.AWBStatsdata);

        VOID*  pIHISTMetaData      = NULL;
        if (TRUE == isSHDRSupported)
        {
            pIHISTMetaData = ReadMetaData(pProcessRequestInfo->frameNum, m_SHDRIHISTDataVendorTagBase, "m_SHDRIHISTDataVendorTagBase", ResultPool);
        }
        else
        {
            pIHISTMetaData = ReadMetaData(pProcessRequestInfo->frameNum, m_IHISTDataVendorTagBase, "m_IHISTDataVendorTagBase", ResultPool);
        }
         m_p_DefogConfigData->defogInputData.pIHistStatsOutput = 
            ((NULL != pIHISTMetaData) ? reinterpret_cast<ParsedIHistStatsOutput*>(pIHISTMetaData) : m_p_DefogConfigData->defogInputData.pIHistStatsOutput);

        if (TRUE == isSHDRSupported)
        {
            VOID* pSHDRSettledMetaData = NULL;
            pSHDRSettledMetaData = ReadMetaData(pProcessRequestInfo->frameNum, m_SHDRSettledVendorTagBase, "m_SHDRSettledVendorTagBase", ResultPool);
            INT32* pStatsSettled = reinterpret_cast<INT32*>(pSHDRSettledMetaData);
            // LOG_INFO(CamxLogGroupChi, "Defog SHDR Settled : %d", *pStatsSettled);
            m_p_DefogConfigData->defogInputData.isSHDRSettled  = *pStatsSettled;
        }


        VOID*  pGamma16InterpolationMetaData      = NULL;
        // Get Gamma Interpolation data info
        pGamma16InterpolationMetaData = ReadMetaData(pProcessRequestInfo->frameNum, m_Gamma16VendorTagBase, "m_Gamma16VendorTagBase", ResultPool);
        if (NULL != pGamma16InterpolationMetaData)
        {
           m_p_DefogConfigData->defogInputData.pInterpolationGamma16Data =
                   reinterpret_cast<gamma_1_6_0::mod_gamma16_channel_dataType*>(pGamma16InterpolationMetaData);
        }
        else
        {
           LOG_INFO(CamxLogGroupChi, "Gamma Interpolation Meta is NULL");
           return CDKResultSuccess;
        }


        VOID*  pGamma15InterpolationMetaData      = NULL;
        // Get Gamma Interpolation data info
        pGamma15InterpolationMetaData = ReadMetaData(pProcessRequestInfo->frameNum,
                                          (m_Gamma15VendorTagBase), "m_Gamma15VendorTagBase", ResultPool);
        if (NULL != pGamma15InterpolationMetaData)
        {
            gamma15InterpolationMetaData =
                *reinterpret_cast<gamma15_cct_dataType::cct_dataStruct*>(pGamma15InterpolationMetaData);

            m_p_DefogConfigData->defogInputData.pInterpolationGamma15Data = &gamma15InterpolationMetaData;
        }

        else
        {
            LOG_INFO(CamxLogGroupChi, "Gamma Interpolation Meta is NULL");
            return CDKResultSuccess;
        }

        VOID*  pANRInterpolationMetaData      = NULL;
        // Get ANR Interpolation data info
        pANRInterpolationMetaData = ReadMetaData(pProcessRequestInfo->frameNum,
                                          (m_ANRDataVendorTagBase), "m_ANRDataVendorTagBase", ResultPool);
        if (NULL != pANRInterpolationMetaData)
        {
            anrInterpolationMetaData = *reinterpret_cast<anr_1_0_0::mod_anr10_cct_dataType::cct_dataStruct*>(pANRInterpolationMetaData);
                   reinterpret_cast<anr10_cct_dataType*>(pANRInterpolationMetaData);
            m_p_DefogConfigData->defogInputData.pInterpolationANR10Data = &anrInterpolationMetaData;

        }
        else
        {
            LOG_INFO(CamxLogGroupChi, "ANR Interpolation Meta is NULL");
            return CDKResultSuccess;
        }

        VOID*  pLTMInterpolationMetaData      = NULL;
        // Get LTM Interpolation data info
        pLTMInterpolationMetaData = ReadMetaData(pProcessRequestInfo->frameNum,
                                          (m_LTMDataVendorTagBase), "m_LTMDataVendorTagBase", ResultPool);
        if (NULL != pLTMInterpolationMetaData)
        {
            ltmInterpolationMetaData = *reinterpret_cast<ltm_1_3_0::ltm13_rgn_dataType*>(pLTMInterpolationMetaData);
            m_p_DefogConfigData->defogInputData.pInterpolationLTM13Data = &ltmInterpolationMetaData;
        }
        else
        {
           LOG_INFO(CamxLogGroupChi, "LTM Interpolation Meta is NULL");
           return CDKResultSuccess;
        }

        VOID*  pASFInterpolationMetaData      = NULL;
        // Get ASF Interpolation data info
        pASFInterpolationMetaData = ReadMetaData(pProcessRequestInfo->frameNum,
                                              (m_ASFDataVendorTagBase), "m_ASFDataVendorTagBase", ResultPool);
        if (NULL != pASFInterpolationMetaData)
        {
            asfInterpolationMetaData =  *reinterpret_cast<asf_3_0_0::asf30_rgn_dataType*>(pASFInterpolationMetaData);
            m_p_DefogConfigData->defogInputData.pInterpolationASF30Data = &asfInterpolationMetaData;
        }
        else
        {
            LOG_INFO(CamxLogGroupChi, "ASF Interpolation Meta is NULL");
            return CDKResultSuccess;
        }
        VOID*  pCVInterpolationMetaData      = NULL;
        // Get CV Interpolation data info
        pCVInterpolationMetaData = ReadMetaData(pProcessRequestInfo->frameNum,
                                              (m_CVDataVendorTagBase), "m_CVDataVendorTagBase", ResultPool);
        if (NULL != pCVInterpolationMetaData)
        {
            cvInterpolationMetaData =  *reinterpret_cast<cv_1_2_0::cv12_rgn_dataType*>(pCVInterpolationMetaData);
            m_p_DefogConfigData->defogInputData.pInterpolationCV12Data = &cvInterpolationMetaData;
        }
        else
        {
            LOG_INFO(CamxLogGroupChi, "CV Interpolation Meta is NULL");
            return CDKResultSuccess;
        }

        m_DS4_buffer.ds4_format = DEFOG_DS4_FORMAT_BLOB;
        m_DS4_buffer.ds4_width  = m_format.width/4;
        m_DS4_buffer.ds4_height = m_format.height/4;
        m_DS4_buffer.ds4_stride = m_DS4_buffer.ds4_width * m_DS4_buffer.ds4_height;
        LOG_VERBOSE(CamxLogGroupChi, "DS4 Fd list :buf:%p and fd:%d",
            reinterpret_cast<uint8_t*>((*pProcessRequestInfo->phInputBuffer)->pImageList[0].pAddr[0]),
            (*pProcessRequestInfo->phInputBuffer)->pImageList[0].fd[0]);

        if (TRUE == defogOn)
        {
            m_defog_lib_process(m_handle, m_p_DefogConfigData);
        }

        CHIMETADATAINFO        metadataInfo         = {0};
        const UINT32           tagSize              = sizeof(g_VendorTagSectionDefog) / sizeof(g_VendorTagSectionDefog[0]) + 3;
        CHITAGDATA             tagData[tagSize]     = { {0} };
        UINT32                 tagList[tagSize]     = {0};

        metadataInfo.size       = sizeof(CHIMETADATAINFO);
        metadataInfo.chiSession = m_hChiSession;
        metadataInfo.tagNum     = tagSize;
        metadataInfo.pTagList   = &tagList[0];
        metadataInfo.pTagData   = &tagData[0];

        UINT32 index = 0;

        tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = pProcessRequestInfo->frameNum;
        if (TRUE == defogOn)
        {
            // if lib gets called with disable lib will copy the data in output
        tagData[index].pData        = m_p_DefogConfigData->defogOutputData.pProcessedANR10Data;
        }
        else
        {
            tagData[index].pData        = m_p_DefogConfigData->defogInputData.pInterpolationANR10Data;
        }
        tagData[index].dataSize     = g_VendorTagSectionDefog[index].numUnits;
        index++;

        tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = pProcessRequestInfo->frameNum;
        if (TRUE == defogOn)
        {
        tagData[index].pData        = m_p_DefogConfigData->defogOutputData.pProcessedLTM13Data;
        }
        else
        {
            tagData[index].pData        = m_p_DefogConfigData->defogInputData.pInterpolationLTM13Data;
        }
        tagData[index].dataSize     = g_VendorTagSectionDefog[index].numUnits;
        index++;

        tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = pProcessRequestInfo->frameNum;
        if (TRUE == defogOn)
        {
        tagData[index].pData        = m_p_DefogConfigData->defogOutputData.pProcessedGamma15Data;
        }
        else
        {
            tagData[index].pData        = m_p_DefogConfigData->defogInputData.pInterpolationGamma15Data;
        }
        tagData[index].dataSize     = g_VendorTagSectionDefog[index].numUnits;
        index++;

        tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = pProcessRequestInfo->frameNum;
        if (TRUE == defogOn)
        {
        tagData[index].pData        = m_p_DefogConfigData->defogOutputData.pProcessedASF30Data;
        }
        else
        {
            tagData[index].pData        = m_p_DefogConfigData->defogInputData.pInterpolationASF30Data;
        }
        tagData[index].dataSize     = g_VendorTagSectionDefog[index].numUnits;
        index++;

        tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = pProcessRequestInfo->frameNum;
        if (TRUE == defogOn)
        {
            tagData[index].pData        = m_p_DefogConfigData->defogOutputData.pProcessedCV12Data;
        }
        else
        {
            tagData[index].pData        = m_p_DefogConfigData->defogInputData.pInterpolationCV12Data;
        }
        if (m_DefogParams.algo == 0)
        {
            tagData[index].pData        = m_p_DefogConfigData->defogInputData.pInterpolationCV12Data;

        }
        tagData[index].dataSize     = g_VendorTagSectionDefog[index].numUnits;
        index++;

        tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = pProcessRequestInfo->frameNum;
        if ((TRUE == defogOn) && (NULL != m_p_DefogConfigData->defogOutputData.pProcessedDEData))
        {
        // @todo : hardcoding destats tuning data till defog algo generates destats.
            //tagData[index].pData        = m_p_DefogConfigData->defogOutputData.pProcessedDEData;
            tagData[index].pData        = m_p_DefogConfigData->defogInputData.pInterpolationDEData;
        }
        else
        {
            tagData[index].pData        = m_p_DefogConfigData->defogInputData.pInterpolationDEData;
        }
        tagData[index].dataSize     = g_VendorTagSectionDefog[index].numUnits;
        index++;


        tagList[index]              = m_DefogisSettledVendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = pProcessRequestInfo->frameNum;
        tagData[index].pData        = &m_p_DefogConfigData->defogOutputData.isSettled;
        tagData[index].dataSize     = 1;
        index++;

        tagList[index]              = m_DefogAlgoFogSceneProbVendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = pProcessRequestInfo->frameNum;
        tagData[index].pData        = &m_p_DefogConfigData->defogOutputData.fog_scene_probability;
        tagData[index].dataSize     = 1;
        index++;

        tagList[index]              = m_DefogGammaStrengthVendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = pProcessRequestInfo->frameNum;
        tagData[index].pData        = &m_p_DefogConfigData->defogOutputData.pProcessedGamma15Data;
        tagData[index].dataSize     = sizeof(gamma15_cct_dataType::cct_dataStruct);

        result = g_ChiNodeInterface.pSetMetadata(&metadataInfo);
        if (CDKResultSuccess != result)
        {
            LOG_ERROR(CamxLogGroupChi, "Error publishing Defog data");
        }
    }
        return result;
}

BOOL ChiDefogNode::SetDeFogConfigParams(UINT64 frameNum)
{
    VOID*  pDefogEnableMeta      = NULL;
    BOOL   paramChanged          = FALSE;
    // Get App Defog Data
    // currently per frame but should be done only if meta changes
    // need to store previous meta
    memset(&m_DefogParams, 0, sizeof(DefogConfigParams));

    pDefogEnableMeta = ReadMetaData(frameNum, m_DefogEnableVendorTagBase, "m_DefogEnableVendorTagBase");
    m_DefogParams.defog_en =
        ((NULL != pDefogEnableMeta) ? *reinterpret_cast<BOOL*>(pDefogEnableMeta) : m_p_DefogInternalData->defogConfigData.defog_en);
    LOG_INFO(CamxLogGroupChi, "Algo App enable = %d and %d", (INT)frameNum, m_DefogParams.defog_en);

    VOID*  pDefogAlgoTypeMeta                                   = NULL;
    // Get App Defog Data
    pDefogAlgoTypeMeta = ReadMetaData(frameNum, m_DefogAlgoVendorTagBase, "m_DefogAlgoVendorTagBase");
    m_DefogParams.algo = ((NULL != pDefogAlgoTypeMeta) ?
        *reinterpret_cast<INT32*>(pDefogAlgoTypeMeta) : m_p_DefogInternalData->defogConfigData.algo);

    VOID*  pDefogAlgoEnableCE                                   = NULL;
    // Get App Defog Data
    pDefogAlgoEnableCE  = ReadMetaData(frameNum, m_DefogAlgoEnableCETagBase, "m_DefogAlgoEnableCETagBase");
    m_DefogParams.ce_en = ((NULL != pDefogAlgoEnableCE) ?
        *reinterpret_cast<BOOL*>(pDefogAlgoEnableCE) : m_p_DefogInternalData->defogConfigData.ce_en);

    VOID*  pDefogAlgoConvMode                                   = NULL;
    // Get App Defog Data
    pDefogAlgoConvMode  = ReadMetaData(frameNum, m_DefogAlgoEnableConvTagBase, "m_DefogAlgoEnableConvTagBase");
    m_DefogParams.convergence_mode = ((NULL != pDefogAlgoConvMode) ?
        *reinterpret_cast<INT32*>(pDefogAlgoConvMode) : m_p_DefogInternalData->defogConfigData.convergence_mode);

    VOID*  pDefogAlgoGainUpControl                              = NULL;
    // Get App Defog Data
    pDefogAlgoGainUpControl  = ReadMetaData(frameNum, m_DefogAlgoGainUpControlBase, "m_DefogAlgoGainUpControlBase");
    m_DefogParams.guc_en = ((NULL != pDefogAlgoGainUpControl) ?
        *reinterpret_cast<BOOL*>(pDefogAlgoGainUpControl) : m_p_DefogInternalData->defogConfigData.guc_en);

    VOID*  pDefogAlgoDccControl                              = NULL;
    // Get App Defog Data
    pDefogAlgoDccControl  = ReadMetaData(frameNum, m_DefogAlgoDccControlBase, "m_DefogAlgoDccControlBase");
    m_DefogParams.dcc_en = ((NULL != pDefogAlgoDccControl) ?
        *reinterpret_cast<BOOL*>(pDefogAlgoDccControl) : m_p_DefogInternalData->defogConfigData.dcc_en);

    VOID*  pDefogAlgoGucStrengthControl                              = NULL;
    // Get App Defog Data
    pDefogAlgoGucStrengthControl  = ReadMetaData(frameNum, m_DefogAlgoGucStrengthBase, "m_DefogAlgoGucStrengthBase");
    m_DefogParams.guc_str = ((NULL != pDefogAlgoGucStrengthControl) ?
        *reinterpret_cast<FLOAT*>(pDefogAlgoGucStrengthControl) : m_p_DefogInternalData->defogConfigData.guc_str);

    VOID*  pDefogAlgoDccDarkStrengthControl                              = NULL;
    // Get App Defog Data
    pDefogAlgoDccDarkStrengthControl  = ReadMetaData(frameNum, m_DefogAlgoDccDarkStrengthBase, "m_DefogAlgoDccDarkStrengthBase");
    m_DefogParams.dcc_dark_str = ((NULL != pDefogAlgoDccDarkStrengthControl) ?
        *reinterpret_cast<FLOAT*>(pDefogAlgoDccDarkStrengthControl) : m_p_DefogInternalData->defogConfigData.dcc_dark_str);

    VOID*  pDefogAlgoDccBrightStrengthControl                              = NULL;
    // Get App Defog Data
    pDefogAlgoDccBrightStrengthControl  = ReadMetaData(frameNum, m_DefogAlgoDccBrightStrengthBase, "m_DefogAlgoDccBrightStrengthBase");
    m_DefogParams.dcc_bright_str = ((NULL != pDefogAlgoDccBrightStrengthControl) ?
        *reinterpret_cast<FLOAT*>(pDefogAlgoDccBrightStrengthControl) : m_p_DefogInternalData->defogConfigData.dcc_bright_str);

    VOID*  pDefogAlgoStrengtheMeta                              = NULL;
    // Get App Defog Data
    pDefogAlgoStrengtheMeta = ReadMetaData(frameNum, m_DefogStrengthVendorTagBase, "m_DefogStrengthVendorTagBase");
    m_DefogParams.strength = ((NULL != pDefogAlgoStrengtheMeta) ?
        *reinterpret_cast<FLOAT*>(pDefogAlgoStrengtheMeta) : m_p_DefogInternalData->defogConfigData.strength);

    VOID*  pDefogAlgoSpeedMeta                                  = NULL;
    // Get App Defog Data
    pDefogAlgoSpeedMeta = ReadMetaData(frameNum, m_DefogSpeedVendorTagBase, "m_DefogSpeedVendorTagBase");
    m_DefogParams.speed = ((NULL != pDefogAlgoSpeedMeta) ?
        *reinterpret_cast<INT32*>(pDefogAlgoSpeedMeta) : m_p_DefogInternalData->defogConfigData.speed);

    VOID*  pDefogDecisionModeSpeedMeta                          = NULL;
    // Get App Defog Data
    pDefogDecisionModeSpeedMeta = ReadMetaData(frameNum, m_DefogDecisionModeVendorTagBase, "m_DefogDecisionModeVendorTagBase");
    m_DefogParams.lp_algo_decision_mode = ((NULL != pDefogDecisionModeSpeedMeta) ?
        *reinterpret_cast<INT32*>(pDefogDecisionModeSpeedMeta): m_p_DefogInternalData->defogConfigData.lp_algo_decision_mode);

    VOID*  pDefogColorCompGainMeta                              = NULL;
    // Get App Defog Data
    pDefogColorCompGainMeta = ReadMetaData(frameNum, m_DefogColorCompGainVendorTagBase, "m_DefogColorCompGainVendorTagBase");
    m_DefogParams.lp_algo_color_comp_gain = ((NULL != pDefogColorCompGainMeta) ?
        *reinterpret_cast<FLOAT*>(pDefogColorCompGainMeta) : m_p_DefogInternalData->defogConfigData.lp_algo_color_comp_gain);

    VOID*  pDefogAdaptiveBrightnessCompEnableMeta               = NULL;
    // Get App Defog Data
    pDefogAdaptiveBrightnessCompEnableMeta = ReadMetaData(frameNum, m_DefogAdaptiveBrightnessCompEnableVendorTagBase,
                                                        "m_DefogAdaptiveBrightnessCompEnableVendorTagBase");
    m_DefogParams.abc_en = ((NULL != pDefogAdaptiveBrightnessCompEnableMeta) ?
        *reinterpret_cast<BOOL*>(pDefogAdaptiveBrightnessCompEnableMeta) : m_p_DefogInternalData->defogConfigData.abc_en);

    VOID*  pDefogAdvanceContractControlEnableMeta               = NULL;
    // Get App Defog Data
    pDefogAdvanceContractControlEnableMeta = ReadMetaData(frameNum, m_DefogAdvanceContractControlEnableVendorTagBase,
                                                        "m_DefogAdvanceContractControlEnableVendorTagBase");
    m_DefogParams.acc_en = ((NULL != pDefogAdvanceContractControlEnableMeta) ?
        *reinterpret_cast<BOOL*>(pDefogAdvanceContractControlEnableMeta) : m_p_DefogInternalData->defogConfigData.acc_en);

    VOID*  pDefogAutoFogSceneDetectionEnableMeta                = NULL;
    // Get App Defog Data
    pDefogAutoFogSceneDetectionEnableMeta = ReadMetaData(frameNum, m_DefogAutoFogSceneDetectionEnableVendorTagBase,
                                                        "m_DefogAutoFogSceneDetectionEnableVendorTagBase");
    m_DefogParams.sd_en = ((NULL != pDefogAutoFogSceneDetectionEnableMeta) ?
        *reinterpret_cast<BOOL*>(pDefogAutoFogSceneDetectionEnableMeta) : m_p_DefogInternalData->defogConfigData.sd_en);

    VOID*  pDefogAutoFogSceneDetectionWith2AEnableMeta          = NULL;
    // Get App Defog Data
    pDefogAutoFogSceneDetectionWith2AEnableMeta = ReadMetaData(frameNum, m_DefogAutoFogSceneDetectionWith2AEnableVendorTagBase,
                                                        "m_DefogAutoFogSceneDetectionWith2AEnableVendorTagBase");
    m_DefogParams.sd_2a_en = ((NULL != pDefogAutoFogSceneDetectionWith2AEnableMeta) ?
        *reinterpret_cast<BOOL*>(pDefogAutoFogSceneDetectionWith2AEnableMeta) : m_p_DefogInternalData->defogConfigData.sd_2a_en);

    VOID*  pDefogDarkThresMeta                                  = NULL;
    // Get App Defog Data
    pDefogDarkThresMeta = ReadMetaData(frameNum, m_DefogDarkThresVendorTagBase, "m_DefogDarkThresVendorTagBase");
    m_DefogParams.defog_dark_thres = ((NULL != pDefogDarkThresMeta) ?
        *reinterpret_cast<UINT32*>(pDefogDarkThresMeta) : m_p_DefogInternalData->defogConfigData.defog_dark_thres);

    VOID*  pDefogBrightThresMeta                                = NULL;
    // Get App Defog Data
    pDefogBrightThresMeta = ReadMetaData(frameNum, m_DefogBrightThresVendorTagBase, "m_DefogBrightThresVendorTagBase");
    m_DefogParams.defog_bright_thres = ((NULL != pDefogBrightThresMeta) ?
        *reinterpret_cast<UINT32*>(pDefogBrightThresMeta) : m_p_DefogInternalData->defogConfigData.defog_bright_thres);

    VOID*  pDefogAdaptiveBrightnessCompStrMeta                 = NULL;
    // Get App Defog Data
    pDefogAdaptiveBrightnessCompStrMeta = ReadMetaData(frameNum, m_DefogAdaptiveBrightnessCompStrVendorTagBase,
                                                            "m_DefogAdaptiveBrightnessCompStrVendorTagBase");
    m_DefogParams.abc_str = ((NULL != pDefogAdaptiveBrightnessCompStrMeta) ?
        *reinterpret_cast<FLOAT*>(pDefogAdaptiveBrightnessCompStrMeta) : m_p_DefogInternalData->defogConfigData.abc_str);

    VOID*  pDefogMaxAdvanceContractControlDarkStrengthMeta      = NULL;
    // Get App Defog Data
    pDefogMaxAdvanceContractControlDarkStrengthMeta = ReadMetaData(frameNum, m_DefogMaxAdvanceContractControlDarkStrengthVendorTagBase,
                                                            "m_DefogMaxAdvanceContractControlDarkStrengthVendorTagBase");
    m_DefogParams.acc_dark_str = ((NULL != pDefogMaxAdvanceContractControlDarkStrengthMeta) ?
        *reinterpret_cast<FLOAT*>(pDefogMaxAdvanceContractControlDarkStrengthMeta) : m_p_DefogInternalData->defogConfigData.acc_dark_str);

    VOID*  pDefogMaxAdvanceContractControlBrightStrengthMeta    = NULL;
    // Get App Defog Data
    pDefogMaxAdvanceContractControlBrightStrengthMeta = ReadMetaData(frameNum, m_DefogMaxAdvanceContractControlBrightStrengthVendorTagBase,
                                                            "m_DefogMaxAdvanceContractControlBrightStrengthVendorTagBase");
    m_DefogParams.acc_bright_str = ((NULL != pDefogMaxAdvanceContractControlBrightStrengthMeta) ?
        *reinterpret_cast<FLOAT*>(pDefogMaxAdvanceContractControlBrightStrengthMeta) : m_p_DefogInternalData->defogConfigData.acc_bright_str);

    VOID*  pDefogDarkLimitMeta                                  = NULL;
    // Get App Defog Data
    pDefogDarkLimitMeta = ReadMetaData(frameNum, m_DefogDarkLimitVendorTagBase, "m_DefogDarkLimitVendorTagBase");
    m_DefogParams.dark_limit = ((NULL != pDefogDarkLimitMeta) ?
        *reinterpret_cast<UINT32*>(pDefogDarkLimitMeta) : m_p_DefogInternalData->defogConfigData.dark_limit);

    VOID*  pDefogBrightLimitMeta                                = NULL;
    // Get App Defog Data
    pDefogBrightLimitMeta = ReadMetaData(frameNum, m_DefogBrightLimitVendorTagBase, "m_DefogBrightLimitVendorTagBase");
    m_DefogParams.bright_limit = ((NULL != pDefogBrightLimitMeta) ?
        *reinterpret_cast<UINT32*>(pDefogBrightLimitMeta) : m_p_DefogInternalData->defogConfigData.bright_limit);

    VOID*  pDefogDarkPreserveMeta                               = NULL;
    // Get App Defog Data
    pDefogDarkPreserveMeta = ReadMetaData(frameNum, m_DefogDarkPreserveVendorTagBase, "m_DefogDarkPreserveVendorTagBase");
    m_DefogParams.dark_preserve = ((NULL != pDefogDarkPreserveMeta) ?
        *reinterpret_cast<UINT32*>(pDefogDarkPreserveMeta) : m_p_DefogInternalData->defogConfigData.dark_preserve);

    VOID*  pDefogBrightPreserveMeta                             = NULL;
    // Get App Defog Data
    pDefogBrightPreserveMeta = ReadMetaData(frameNum, m_DefogBrightPreserveVendorTagBase, "m_DefogBrightPreserveVendorTagBase");
    m_DefogParams.bright_preserve = ((NULL != pDefogBrightPreserveMeta) ?
        *reinterpret_cast<UINT32*>(pDefogBrightPreserveMeta) : m_p_DefogInternalData->defogConfigData.bright_preserve);

    VOID*  pDefogTriggerParamMeta                               = NULL;
    // Get App Defog Data
    pDefogTriggerParamMeta = ReadMetaData(frameNum, m_DefogTriggerParamVendorTagBase, "m_DefogTriggerParamVendorTagBase");
    m_DefogParams.defog_trig_params = ((NULL != pDefogTriggerParamMeta) ?
        *reinterpret_cast<FOG_SCENE_DETECTION_PARAMS*>(pDefogTriggerParamMeta) : m_p_DefogInternalData->defogConfigData.defog_trig_params);

    VOID*  pDefogAlgoceSceneDetectControl                              = NULL;
    // Get App Defog Data
    pDefogAlgoceSceneDetectControl  = ReadMetaData(frameNum, m_DefogAlgoceSceneDetectionBase, "m_DefogAlgoceSceneDetectionBase");
    m_DefogParams.ce_trig_params = ((NULL != pDefogAlgoceSceneDetectControl) ?
        *reinterpret_cast<CE_SCENE_DETECTION_PARAMS*>(pDefogAlgoceSceneDetectControl) : m_p_DefogInternalData->defogConfigData.ce_trig_params);


#if 1

    if (frameNum > 1)
    {
        m_DefogParams.param_update_mask = m_PrevDefogParams.param_update_mask;

        LOG_VERBOSE(CamxLogGroupChi, "Previous (  defog_en = %d ce_en =%d speed = %d convergence_mode = %d sd_en = %d "
                    "sd_2a_en = %d algo = %d strength = %f lp_algo_decision_mode = %d lp_algo_color_comp_gain = %f "
                    "abc_en = %d acc_en = %d defog_dark_thres = %d defog_bright_thres = %d dark_limit = %u bright_limit = %u "
                    "dark_preserve = %u bright_preserve = %u abc_str = %f acc_dark_str = %f acc_bright_str = %f "
                    "guc_en = %d dcc_en = %d, guc_str = %f, dcc_dark_str = %f, dcc_bright_str = %f param_update_mask = %u)",
                    m_PrevDefogParams.defog_en, m_PrevDefogParams.ce_en, m_PrevDefogParams.speed,
                    m_PrevDefogParams.convergence_mode, m_PrevDefogParams.sd_en, m_PrevDefogParams.sd_2a_en,
                    m_PrevDefogParams.algo, m_PrevDefogParams.strength, m_PrevDefogParams.lp_algo_decision_mode,
                    m_PrevDefogParams.lp_algo_color_comp_gain, m_PrevDefogParams.abc_en, m_PrevDefogParams.acc_en,
                    m_PrevDefogParams.defog_dark_thres, m_PrevDefogParams.defog_bright_thres, m_PrevDefogParams.dark_limit,
                    m_PrevDefogParams.bright_limit, m_PrevDefogParams.dark_preserve, m_PrevDefogParams.bright_preserve,
                    m_PrevDefogParams.abc_str, m_PrevDefogParams.acc_dark_str, m_PrevDefogParams.acc_bright_str,
                    m_PrevDefogParams.guc_en, m_PrevDefogParams.dcc_en, m_PrevDefogParams.guc_str,
                    m_PrevDefogParams.dcc_dark_str, m_PrevDefogParams.dcc_bright_str, m_PrevDefogParams.param_update_mask);

        LOG_VERBOSE(CamxLogGroupChi, "current (  defog_en = %d ce_en =%d speed = %d convergence_mode = %d sd_en = %d "
                    "sd_2a_en = %d algo = %d strength = %f lp_algo_decision_mode = %d lp_algo_color_comp_gain = %f "
                    "abc_en = %d acc_en = %d defog_dark_thres = %d defog_bright_thres = %d dark_limit = %u bright_limit = %u "
                    "dark_preserve = %u bright_preserve = %u abc_str = %f acc_dark_str = %f acc_bright_str = %f "
                    "guc_en = %d dcc_en = %d, guc_str = %f, dcc_dark_str = %f, dcc_bright_str = %f param_update_mask = %u)",
                    m_DefogParams.defog_en, m_DefogParams.ce_en, m_DefogParams.speed,
                    m_DefogParams.convergence_mode, m_DefogParams.sd_en, m_DefogParams.sd_2a_en,
                    m_DefogParams.algo, m_DefogParams.strength, m_DefogParams.lp_algo_decision_mode,
                    m_DefogParams.lp_algo_color_comp_gain, m_DefogParams.abc_en, m_DefogParams.acc_en,
                    m_DefogParams.defog_dark_thres, m_DefogParams.defog_bright_thres, m_DefogParams.dark_limit,
                    m_DefogParams.bright_limit, m_DefogParams.dark_preserve, m_DefogParams.bright_preserve,
                    m_DefogParams.abc_str, m_DefogParams.acc_dark_str, m_DefogParams.acc_bright_str,
                    m_DefogParams.guc_en, m_DefogParams.dcc_en, m_DefogParams.guc_str,
                    m_DefogParams.dcc_dark_str, m_DefogParams.dcc_bright_str, m_DefogParams.param_update_mask);


        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.defog_en, &m_DefogParams.defog_en, DefogConfParamGroupDefogEn));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.algo, &m_DefogParams.algo, DefogConfParamGroupAlgo));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.strength, &m_DefogParams.strength, DefogConfParamGroupStrength));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.speed, &m_DefogParams.speed, DefogConfParamGroupSpeed));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.lp_algo_decision_mode, &m_DefogParams.lp_algo_decision_mode, DefogConfParamGroupLPAlgoDecisionMode));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.lp_algo_color_comp_gain, &m_DefogParams.lp_algo_color_comp_gain, DefogConfParamGroupLPColorCompensationGain));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.ce_en, &m_DefogParams.ce_en, DefogConfParamGroupCEEn));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.convergence_mode, &m_DefogParams.convergence_mode, DefogConfParamGroupConvMode));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.guc_en, &m_DefogParams.guc_en, DefogConfParamGroupGUCEn));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.dcc_en, &m_DefogParams.dcc_en, DefogConfParamGroupDCCEn));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.guc_str, &m_DefogParams.guc_str, DefogConfParamGroupGUCStr));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.dcc_dark_str, &m_DefogParams.dcc_dark_str, DefogConfParamGroupDCCDarkStr));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.dcc_bright_str, &m_DefogParams.dcc_bright_str, DefogConfParamGroupDCCBrightStr));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.abc_en, &m_DefogParams.abc_en, DefogConfParamGroupABCEn));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.acc_en, &m_DefogParams.acc_en, DefogConfParamGroupACCEn));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.sd_en, &m_DefogParams.sd_en, DefogConfParamGroupSDEn));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.sd_2a_en, &m_DefogParams.sd_2a_en, DefogConfParamGroupSD2AEn));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.defog_dark_thres, &m_DefogParams.defog_dark_thres, DefogConfParamGroupDefogDarkThres));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.defog_bright_thres, &m_DefogParams.defog_bright_thres, DefogConfParamGroupDefogBrightThres));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.abc_str, &m_DefogParams.abc_str, DefogConfParamGroupABCStr));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.acc_dark_str, &m_DefogParams.acc_dark_str, DefogConfParamGroupACCDarkStr));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.acc_bright_str, &m_DefogParams.acc_bright_str, DefogConfParamGroupACCBrightStr));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.dark_limit, &m_DefogParams.dark_limit, DefogConfParamGroupDarkLimit));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.bright_limit, &m_DefogParams.bright_limit, DefogConfParamGroupBrightLimit));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.dark_preserve, &m_DefogParams.dark_preserve, DefogConfParamGroupDarkPreserve));

        paramChanged = (paramChanged | IsMetaChanged(&m_PrevDefogParams.bright_preserve, &m_DefogParams.bright_preserve, DefogConfParamGroupBrightPreserve));

        if (0 != memcmp(&m_PrevDefogParams.defog_trig_params ,&m_DefogParams.defog_trig_params, sizeof(FOG_SCENE_DETECTION_PARAMS)))
        {
            m_DefogParams.param_update_mask = m_DefogParams.param_update_mask | DefogConfParamGroupAFSDTriggers;
            memcpy(&m_PrevDefogParams.defog_trig_params, &m_DefogParams.defog_trig_params, sizeof(FOG_SCENE_DETECTION_PARAMS));
            paramChanged                    = TRUE;
        }

        if (0 != memcmp(&m_PrevDefogParams.ce_trig_params ,&m_DefogParams.ce_trig_params, sizeof(CE_SCENE_DETECTION_PARAMS)))
        {
            m_DefogParams.param_update_mask = m_DefogParams.param_update_mask | DefogConfParamGroupACESDTriggers;
            memcpy(&m_PrevDefogParams.ce_trig_params, &m_DefogParams.ce_trig_params, sizeof(CE_SCENE_DETECTION_PARAMS));
            paramChanged                    = TRUE;
        }
        if (m_DefogParams.param_update_mask != DefogConfParamGroupNone)
        {
            m_DefogParams.param_update_mask = m_DefogParams.param_update_mask & ~1;

        }
        m_PrevDefogParams.param_update_mask = m_DefogParams.param_update_mask;
    }
    else
    {
        memcpy(&m_PrevDefogParams, &m_DefogParams, sizeof(DefogConfigParams));
#if 0
        m_PrevDefogParams.defog_en                 = m_DefogParams.defog_en;
        m_PrevDefogParams.ce_en                    = m_DefogParams.ce_en;
        m_PrevDefogParams.algo                     = m_DefogParams.algo;
        m_PrevDefogParams.strength                 = m_DefogParams.strength;
        m_PrevDefogParams.speed                    = m_DefogParams.speed;
        m_PrevDefogParams.lp_algo_decision_mode    = m_DefogParams.lp_algo_decision_mode;
        m_PrevDefogParams.lp_algo_color_comp_gain  = m_DefogParams.lp_algo_color_comp_gain;
#endif
    }

#endif
    return paramChanged;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDefogNode::ReadMetaData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* ChiDefogNode::ReadMetaData(UINT64 frameNum, UINT32 tag, const CHAR* tagName, INT MetaPooltype)
{
    UINT32 inputMetaTag = tag;
    if (InputPool == MetaPooltype)
    {
        inputMetaTag            = tag | InputMetadataSectionMask;
    }
    VOID*  pDefodInputMetaVal      = NULL;
    pDefodInputMetaVal = ChiNodeUtils::GetMetaData(frameNum,
                                        inputMetaTag,
                                        ChiMetadataDynamic,
                                        &g_ChiNodeInterface,
                                        m_hChiSession);
    if (NULL == pDefodInputMetaVal)
    {
        LOG_INFO(CamxLogGroupChi, "Read Meta is NULL :Tag : %u Name :%s", tag, tagName);
    }
    return pDefodInputMetaVal;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDefogNode::IsMetaChanged
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChiDefogNode::IsMetaChanged(UINT32 *data1, UINT32 *data2, UINT32 mask)
{
    BOOL metaChange = FALSE;
    if (*data1 != *data2)
    {
        m_DefogParams.param_update_mask = m_DefogParams.param_update_mask | mask;
        *data1                          = *data2;
        metaChange                      = TRUE;
    }
    return metaChange;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDefogNode::IsMetaChanged
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChiDefogNode::IsMetaChanged(BOOL *data1, BOOL *data2, UINT32 mask)
{
    BOOL metaChange = FALSE;
    if (*data1 != *data2)
    {
        m_DefogParams.param_update_mask = m_DefogParams.param_update_mask | mask;
        *data1                          = *data2;
        metaChange                      = TRUE;
    }
    return metaChange;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDefogNode::IsMetaChanged
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChiDefogNode::IsMetaChanged(FLOAT *data1, FLOAT *data2, UINT32 mask)
{
    BOOL metaChange = FALSE;
    if (*data1 != *data2)
    {
        m_DefogParams.param_update_mask = m_DefogParams.param_update_mask | mask;
        *data1                          = *data2;
        metaChange                      = TRUE;
    }
    return metaChange;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDefogNode::GetVendorTagBases
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDefogNode::GetVendorTagBases()
{
    CDKResult            result            = CDKResultSuccess;
    CHIVENDORTAGBASEINFO vendorTagBaseInfo = { 0 };

    // Get the "AEC Frame Control" vendor tag base
    m_AECDataVendorTagBase                                  = GetVendorTagAddr("AECFrameControl",
                                            "org.quic.camera2.statsconfigs");

    // Get ADRC vendor tag base
    m_ADRCinfoVendorTagBase                                 = GetVendorTagAddr("disable",
                                            "org.codeaurora.qcamera3.adrc");

    // Get the "AWB Frame Control" vendor tag base
    m_AWBDataVendorTagBase                                  = GetVendorTagAddr("AWBFrameControl",
                                            "org.quic.camera2.statsconfigs");

    // Get the "IHIST Parsed" vendor tag base
    m_IHISTDataVendorTagBase                                = GetVendorTagAddr("IHistStatsParsed",
                                            "org.quic.camera2.statsconfigs");

    // Get the "IHIST Parsed from YUV SHDR" vendor tag base
    m_SHDRIHISTDataVendorTagBase                                = GetVendorTagAddr("SHDRIHISTStats",
                                            "com.qti.node.shdr3");

    // Get the "IHIST Parsed from YUV SHDR" vendor tag base
    m_SHDRSettledVendorTagBase                                  = GetVendorTagAddr("SHDRSETTLEStats",
                                            "com.qti.node.shdr3");

    // Get the "SensorMode" vendor tag base
    m_SensorModeInfoTagBase                                = GetVendorTagAddr("sensor_mode_info",
                                            "org.codeaurora.qcamera3.sensor_meta_data");

    // Get the "Gamma Interpolation data" vendor tag base
    m_Gamma16VendorTagBase                                  = GetVendorTagAddr("Gamma16Interpolation",
                                            "org.quic.camera.IQInterpolationData");

    // Get the "Gamma Interpolation data" vendor tag base
    m_Gamma15VendorTagBase                                  = GetVendorTagAddr("Gamma15Interpolation",
                                            "org.quic.camera.IQInterpolationData");

    // Get the "ANR Interpolation data" vendor tag base
    m_ANRDataVendorTagBase                                  = GetVendorTagAddr("ANRInterpolation",
                                            "org.quic.camera.IQInterpolationData");

    // Get the "LTM Interpolation data" vendor tag base
    m_LTMDataVendorTagBase                                  = GetVendorTagAddr("LTMInterpolation",
                                            "org.quic.camera.IQInterpolationData");

    // Get the "ASF Interpolation data" vendor tag base
    m_ASFDataVendorTagBase                                  = GetVendorTagAddr("ASFInterpolation",
                                            "org.quic.camera.IQInterpolationData");

    // Get the "CV Interpolation data" vendor tag base
    m_CVDataVendorTagBase                                   = GetVendorTagAddr("CVInterpolation",
                                            "org.quic.camera.IQInterpolationData");

    // Get the "App Defog tag base
    m_DefogEnableVendorTagBase                                  = GetVendorTagAddr("enable");

    // Get the "App Defog tag base
    m_DefogAlgoVendorTagBase                                    = GetVendorTagAddr("algo_type");

    // Get the "App Defog tag base
    m_DefogAlgoEnableCETagBase                                  = GetVendorTagAddr("ce_en");

    // Get the "App Defog tag base
    m_DefogAlgoEnableConvTagBase                                = GetVendorTagAddr("convergence_mode");

    // Get the "App Defog tag base
    m_DefogStrengthVendorTagBase                                = GetVendorTagAddr("strength");

    // Get the "App Defog tag base
    m_DefogDecisionModeVendorTagBase                            = GetVendorTagAddr("algo_decision_mode");

    // Get the "App Defog tag base
    m_DefogSpeedVendorTagBase                                   = GetVendorTagAddr("convergence_speed");

    // Get the "App Defog tag base
    m_DefogColorCompGainVendorTagBase                           = GetVendorTagAddr("lp_color_comp_gain");

    // Get the "App Defog tag base
    m_DefogAdaptiveBrightnessCompEnableVendorTagBase            = GetVendorTagAddr("abc_en");

    // Get the "App Defog tag base
    m_DefogAdvanceContractControlEnableVendorTagBase            = GetVendorTagAddr("acc_en");

    // Get the "App Defog tag base
    m_DefogAutoFogSceneDetectionEnableVendorTagBase             = GetVendorTagAddr("afsd_en");

    // Get the "App Defog tag base
    m_DefogAutoFogSceneDetectionWith2AEnableVendorTagBase       = GetVendorTagAddr("afsd_2a_en");

    // Get the "App Defog tag base
    m_DefogDarkThresVendorTagBase                               = GetVendorTagAddr("defog_dark_thres");

    // Get the "App Defog tag base
    m_DefogBrightThresVendorTagBase                             = GetVendorTagAddr("defog_bright_thres");

    // Get the "App Defog tag base
    m_DefogAdaptiveBrightnessCompStrVendorTagBase              = GetVendorTagAddr("abc_gain");

    // Get the "App Defog tag base
    m_DefogMaxAdvanceContractControlDarkStrengthVendorTagBase   = GetVendorTagAddr("acc_max_dark_str");

    // Get the "App Defog tag base
    m_DefogMaxAdvanceContractControlBrightStrengthVendorTagBase = GetVendorTagAddr("acc_max_bright_str");

    // Get the "App Defog tag base
    m_DefogDarkLimitVendorTagBase                               = GetVendorTagAddr("dark_limit");

    // Get the "App Defog tag base
    m_DefogBrightLimitVendorTagBase                             = GetVendorTagAddr("bright_limit");

    // Get the "App Defog tag base
    m_DefogDarkPreserveVendorTagBase                            = GetVendorTagAddr("dark_preserve");

    // Get the "App Defog tag base
    m_DefogBrightPreserveVendorTagBase                          = GetVendorTagAddr("bright_preserve");

    // Get the "App Defog tag base
    m_DefogTriggerParamVendorTagBase                            = GetVendorTagAddr("trig_params");

    // Get the "App Defog tag base
    m_DefogisSettledVendorTagBase                               = GetVendorTagAddr("isSettled");

    // Get the "App Defog tag base
    m_DefogAlgoFogSceneProbVendorTagBase                        = GetVendorTagAddr("algo_fog_scene_probability");

    // Get the "App Defog tag base
    m_DefogGammaStrengthVendorTagBase                           = GetVendorTagAddr("gamma_strength");

    // Get the "App Defog tag base
    m_DefogAlgoGainUpControlBase                                = GetVendorTagAddr("guc_en");

    // Get the "App Defog tag base
    m_DefogAlgoDccControlBase                                   = GetVendorTagAddr("dcc_en");

    // Get the "App Defog tag base
    m_DefogAlgoGucStrengthBase                                  = GetVendorTagAddr("guc_str");

    // Get the "App Defog tag base
    m_DefogAlgoDccDarkStrengthBase                              = GetVendorTagAddr("dcc_dark_str");

    // Get the "App Defog tag base
    m_DefogAlgoDccBrightStrengthBase                            = GetVendorTagAddr("dcc_bright_str");

    // Get the "App Defog tag base
    m_DefogAlgoceSceneDetectionBase                             = GetVendorTagAddr("ce_trig_params");

    // Get the "Sensor Mode Info"
    result = ChiNodeUtils::GetVendorTagBase("org.codeaurora.qcamera3.sensor_meta_data",
                                            "sensor_mode_info",
                                            &g_ChiNodeInterface,
                                            &vendorTagBaseInfo);
    if (CDKResultSuccess == result)
    {
            m_sensorModeInfoVendorTagBase = vendorTagBaseInfo.vendorTagBase;
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Unable to get sensor meta data vendor tag base");
    }
    // return result;
    return CDKResultSuccess; //mrad remove
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDefogNode::GetVendorTagAddr
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 ChiDefogNode::GetVendorTagAddr(const CHAR* pTagname,
                                        const CHAR* pSectionName)
{
    CDKResult            result            = CDKResultSuccess;
    CHIVENDORTAGBASEINFO vendorTagBaseInfo = { 0 };
    UINT32               tagAddr           = 0;

    // Get the "AEC Frame Control" vendor tag base
    result = ChiNodeUtils::GetVendorTagBase(pSectionName,
                                            pTagname,
                                            &g_ChiNodeInterface,
                                            &vendorTagBaseInfo);
    if (CDKResultSuccess == result)
    {
        tagAddr = vendorTagBaseInfo.vendorTagBase;
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Unable to get Vendor Tag Address of %s.%s",
                    pSectionName, pTagname);
    }
    return tagAddr;
}

/// ChiDefogNode::ChiDefogNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiDefogNode::ChiDefogNode()
    : m_hChiSession(NULL)
    , m_nodeId(0)
    , m_nodeCaps(0)
    , m_processedFrame(0)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDefogNode::~ChiDefogNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiDefogNode::~ChiDefogNode()
{
    if (m_handle)
    {
        m_defog_lib_deinit(m_handle);
        m_handle = NULL;
    }
    m_hChiSession = NULL;
    if(NULL != m_p_DefogConfigData->defogOutputData.pProcessedANR10Data)
    {
        free(m_p_DefogConfigData->defogOutputData.pProcessedANR10Data) ;
    }
    if(NULL != m_p_DefogConfigData->defogOutputData.pProcessedLTM13Data)
    {
        free(m_p_DefogConfigData->defogOutputData.pProcessedLTM13Data) ;
    }
    if(NULL != m_p_DefogConfigData->defogOutputData.pProcessedGamma15Data)
    {
        free(m_p_DefogConfigData->defogOutputData.pProcessedGamma15Data) ;
    }
    if(NULL != m_p_DefogConfigData->defogOutputData.pProcessedCV12Data)
    {
        free(m_p_DefogConfigData->defogOutputData.pProcessedCV12Data) ;
    }
    if(NULL != m_p_DefogConfigData->defogOutputData.pProcessedASF30Data)
    {
        free(m_p_DefogConfigData->defogOutputData.pProcessedASF30Data) ;
    }
    if(NULL != m_p_DefogConfigData->defogInputData.pInterpolationDEData)
    {
        free(m_p_DefogConfigData->defogInputData.pInterpolationDEData) ;
    }
    if(NULL != m_p_DefogConfigData->defogInputData.pLTM13ReserveData)
    {
        free(m_p_DefogConfigData->defogInputData.pLTM13ReserveData) ;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDefogNode::LoadLib
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDefogNode::LoadLib()
{
    CDKResult result = CDKResultSuccess;
    m_hDeFogLib      = NULL;

    m_hDeFogLib = ChiNodeUtils::LibMap(pLibName, pLibPath); //need to free this ?

    if (NULL == m_hDeFogLib)
    {
        LOG_ERROR(CamxLogGroupChi, "Error loading lib %s", pLibName);
        result = CDKResultEUnableToLoad;
    }

    ///< Map function pointers
    if (CDKResultSuccess == result)
    {
        m_defog_lib_init = reinterpret_cast<DEFOGLIBINIT>(ChiNodeUtils::LibGetAddr(m_hDeFogLib,
                                                                                               "DefogInitialize"));

        m_defog_lib_getparams = reinterpret_cast<DEFOGGETPARAMS>(ChiNodeUtils::LibGetAddr(m_hDeFogLib,
                                                                                               "DefogGetParams"));

        m_defog_lib_setparams = reinterpret_cast<DEFOGSETPARAMS>(ChiNodeUtils::LibGetAddr(m_hDeFogLib,
                                                                                               "DefogSetParams"));

        m_defog_lib_process = reinterpret_cast<DEFOGPROCESS>(ChiNodeUtils::LibGetAddr(m_hDeFogLib,
                                                                                               "DefogProcess"));

        m_defog_lib_deinit = reinterpret_cast<DEFOGUNINITIALIZE>(ChiNodeUtils::LibGetAddr(m_hDeFogLib,
                                                                                               "DefogUninitialize"));


        if ((NULL == m_defog_lib_init)       ||
            (NULL == m_defog_lib_setparams)   ||
            (NULL == m_defog_lib_process) ||
            (NULL == m_defog_lib_deinit))
        {
            LOG_ERROR(CamxLogGroupChi,
                      "Error Initializing one or more function pointers in Library");
            result = CDKResultENoSuch;
        }
    }
    return result;
}

// CAMX_NAMESPACE_END
