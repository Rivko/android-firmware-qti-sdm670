////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodesvhdr.cpp
/// @brief Chi node for YUVSHDR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <inttypes.h>
#include <system/camera_metadata.h>

#include "chiispstatsdefs.h"
#include "camxchinodeyuvshdr.h"
#include "chiaecinterface.h"
#include <chituningmodeparam.h>
#include "parametertuningtypes.h"
#include "parametertypes.h"
#include <linux/msm_ion.h>
#include <sys/mman.h>
#include <cutils/properties.h>


// NOWHINE FILE CP040: Keyword new not allowed. Use the CAMX_NEW/CAMX_DELETE functions insteads
// NOWHINE FILE NC008: Warning: - Var names should be lower camel case

#undef LOG_TAG
#define LOG_TAG "CHISVHDR3"

#define PRINT_METADATA TRUE
#define INTERPOLATE_GAMMA_RATIO(x, y, z) \
    (((z) - (x)) / ((y) - (x)))

#define LINEAR_INTERPOLATION(v1, v2, ratio) \
    ((v2) + ((ratio) * ((v1) - (v2))))

#define DEFAULT_EXP_RATIO 6.25f
#define EXP_RATIO_50HZ 5.0f
#define EXP_RATIO_60HZ 6.25f

#define YUVSHDR_NODE_IN_FS_PORT_ID             0
#define YUVSHDR_NODE_IN_DS4_PORT_ID            1
#define YUVSHDR_NODE_IN_DS16_PORT_ID           2
#define YUVSHDR_NODE_EXP_PORT_ID_OFFS          3
#define YUVSHDR_NODE_OUT_FS_PORT_ID            0
#define YUVSHDR_NODE_OUT_DS4_PORT_ID           1
#define YUVSHDR_NODE_OUT_DS16_PORT_ID          2

#define DOWNSCALE_FRAMES                       3    ///< 3 frames full, ds4, ds16

ChiNodeInterface g_ChiNodeInterface;    ///< The instance save the CAMX Chi interface
UINT32           g_vendorTagBase = 0;   ///< Chi assigned runtime vendor tag base for the node

const CHAR* pLibName = "libshdr3";

#if defined (_LINUX)
const CHAR* pLibPath = "/vendor/lib/camera/components/";
const CHAR* pFileDumpPath = "/data/misc/camera/";
#endif // _LINUX

#if defined (_WIN32)
const CHAR* pLibPath = ".\\";
const CHAR* pFileDumpPath = ".\\";
#endif // _WIN32

/// @todo (CAMX-1854) the major / minor version shall get from CHI
static const UINT32 ChiNodeMajorVersion           = 0;                       ///< The major version of CHI interface
static const UINT32 ChiNodeMinorVersion           = 0;                       ///< The minor version of CHI interface

static const UINT32 ChiNodeCapsSvhdr              = ChiNodeCapsSHDR3;        ///< Simply do SVHDR operation

static const CHAR   YUVSHDRNodeSectionName[]      = "com.qti.node.shdr3";    ///< The section name for node

static const UINT32 YUVSHDRNodeTagBase            = 0;                       ///< Tag base
static const UINT32 YUVSHDRNodeTagHDRBHISTStats   = YUVSHDRNodeTagBase + 0;  ///< Tag to send BHIST stats
static const UINT32 YUVSHDRNodeTagIHISTStats      = YUVSHDRNodeTagBase + 1;  ///< Tag to send IHIST stats
static const UINT32 YUVSHDRNodeTagSettledStats    = YUVSHDRNodeTagBase + 2;  ///< Tag to send Settled Status to Defog
static const UINT32 YUVSHDRNodeAlgoStatus         = YUVSHDRNodeTagBase + 3;  ///< Tag to send Algo Status
static const UINT32 YUVSHDRNodeInterpolatedTmParam = YUVSHDRNodeTagBase + 4;  ///< Tag to send Interpolated Tuning TM Param


static const UINT ChiNodeInputFull            = 0;
static const UINT ChiNodeINPUTDS4             = 1;
static const UINT ChiNodeINPUTDS16            = 2;
static const UINT ChiNodeINPUTDS64            = 3;

static const UINT32 DefaultRequestQueueDepth  = 12;
static const UINT32 NUMRawSHDRVendorTag       = 3;

///< Supported vendor tag list, it shall align with the definition in g_VendorTagSectionYUVSHDR
static const UINT32 g_VendorTagList[] =
{
    YUVSHDRNodeTagHDRBHISTStats,
    YUVSHDRNodeTagIHISTStats,
    YUVSHDRNodeTagSettledStats,
    YUVSHDRNodeAlgoStatus,
    YUVSHDRNodeInterpolatedTmParam,
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagSectionYUVSHDR[] =
{
    { "SHDRHDRBHISTStats",   TYPE_BYTE, sizeof(ParsedHDRBHistStatsOutput) },
    { "SHDRIHISTStats",      TYPE_BYTE, sizeof(ParsedIHistStatsOutput) },
    { "SHDRSETTLEStats",     TYPE_INT32, 1 },
    { "SHDRAlgoStatus",      TYPE_INT32, 1},
    { "SHDRTmParam",         TYPE_FLOAT, 8},
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGSECTIONDATA g_VendorTagYUVSHDRSection[] =
{
    {
        YUVSHDRNodeSectionName,  0,
        sizeof(g_VendorTagSectionYUVSHDR) / sizeof(g_VendorTagSectionYUVSHDR[0]), g_VendorTagSectionYUVSHDR,
        CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    }
};

///< This is an array of all vendor tag section data
static ChiVendorTagInfo g_VendorTagInfoYUVSHDR[] =
{
    {
        &g_VendorTagYUVSHDRSection[0],
        sizeof(g_VendorTagYUVSHDRSection) / sizeof(g_VendorTagYUVSHDRSection[0])
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// YUVSHDRNodeGetCaps
///
/// @brief  Implementation of PFNNODEGETCAPS defined in chinode.h
///
/// @param  pCapsInfo   Pointer to a structure that defines the capabilities supported by the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult YUVSHDRNodeGetCaps(
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
/// YUVSHDRNodeQueryVendorTag
///
/// @brief  Implementation of PFNCHIQUERYVENDORTAG defined in chinode.h
///
/// @param  pQueryVendorTag Pointer to a structure that returns the exported vendor tag
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult YUVSHDRNodeQueryVendorTag(
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
        result = ChiNodeUtils::QueryVendorTag(pQueryVendorTag, g_VendorTagInfoYUVSHDR);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// YUVSHDRNodeCreate
///
/// @brief  Implementation of PFNNODECREATE defined in chinode.h
///
/// @param  pCreateInfo Pointer to a structure that defines create session information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult YUVSHDRNodeCreate(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult       result  = CDKResultSuccess;
    ChiYUVSHDRNode*  pNode   = NULL;

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
        pNode = new ChiYUVSHDRNode;
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
/// YUVSHDRNodeDestroy
///
/// @brief  Implementation of PFNNODEDESTROY defined in chinode.h
///
/// @param  pDestroyInfo    Pointer to a structure that defines the session destroy information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult YUVSHDRNodeDestroy(
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
            ChiYUVSHDRNode* pNode = static_cast<ChiYUVSHDRNode*>(pDestroyInfo->hNodeSession);

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
/// YUVSHDRNodeQueryBufferInfo
///
/// @brief  Implementation of PFNNODEQUERYBUFFERINFO defined in chinode.h
///
/// @param  pQueryBufferInfo    Pointer to a structure to query the input buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult YUVSHDRNodeQueryBufferInfo(
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
            ChiYUVSHDRNode* pNode = static_cast<ChiYUVSHDRNode*>(pQueryBufferInfo->hNodeSession);
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
/// YUVSHDRNodeSetBufferInfo
///
/// @brief  Implementation of PFNNODESETBUFFERINFO defined in chinode.h
///
/// @param  pSetBufferInfo  Pointer to a structure with information to set the output buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult YUVSHDRNodeSetBufferInfo(
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
            ChiYUVSHDRNode* pNode = static_cast<ChiYUVSHDRNode*>(pSetBufferInfo->hNodeSession);
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
/// YUVSHDRNodeProcRequest
///
/// @brief  Implementation of PFNNODEPROCREQUEST defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult YUVSHDRNodeProcRequest(
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
            ChiYUVSHDRNode* pNode = static_cast<ChiYUVSHDRNode*>(pProcessRequestInfo->hNodeSession);
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
/// YUVSHDRNodeSetNodeInterface
///
/// @brief  Implementation of PFCHINODESETNODEINTERFACE defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID YUVSHDRNodeSetNodeInterface(
    ChiNodeInterface* pNodeInterface)
{
    CDKResult result = CDKResultSuccess;
    result = ChiNodeUtils::SetNodeInterface(pNodeInterface, YUVSHDRNodeSectionName,
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
            pNodeCallbacks->pGetCapabilities         = YUVSHDRNodeGetCaps;
            pNodeCallbacks->pQueryVendorTag          = YUVSHDRNodeQueryVendorTag;
            pNodeCallbacks->pCreate                  = YUVSHDRNodeCreate;
            pNodeCallbacks->pDestroy                 = YUVSHDRNodeDestroy;
            pNodeCallbacks->pQueryBufferInfo         = YUVSHDRNodeQueryBufferInfo;
            pNodeCallbacks->pSetBufferInfo           = YUVSHDRNodeSetBufferInfo;
            pNodeCallbacks->pProcessRequest          = YUVSHDRNodeProcRequest;
            pNodeCallbacks->pChiNodeSetNodeInterface = YUVSHDRNodeSetNodeInterface;
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
/// ChiYUVSHDRNode::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiYUVSHDRNode::Initialize(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult result = CDKResultSuccess;

    m_InterpolationResourceAllocated = FALSE;
    CHAR   prop[PROPERTY_VALUE_MAX];
    property_get("persist.vendor.camera.livetuning", prop, "0");
    enableLiveTuning = atoi(prop);

    shdrAlgoType                = YUVSHDR_ALGO_2FRAME;
    m_hChiSession               = pCreateInfo->hChiSession;
    m_nodeId                    = pCreateInfo->nodeId;
    m_nodeCaps                  = pCreateInfo->nodeCaps.nodeCapsMask;
    m_nodeFlags                 = pCreateInfo->nodeFlags;
    m_processedFrame            = 0;
    m_IsDefog_Enabled           = FALSE;
    m_DefogEnableStartFrameNum  = 1;
    result                      = GetVendorTagBases();

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


        if (CDKResultSuccess == result)
        {
            /* Attach pointers */
            memset(m_p_shdr_ctx, 0,sizeof(SHDRNODECONTEXT));
        }
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Load SHDR algo lib failed");
    }

    return result;
}

void ChiYUVSHDRNode::ShdrCompletionCallback(void *handle, SHDR3INPUTPARAM *input_param, INT ihist_ready, void *user_data)
{
    CDK_UNUSED_PARAM(handle);
    CDK_UNUSED_PARAM(input_param);
    SHDRCALLBACK *p_cback = (SHDRCALLBACK *)user_data;
    UINT *max_val;
    UINT shift_bit = 0;
    UINT ihist_to16bit_ratio = 0;

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
    ChiYUVSHDRNode *pShdr = reinterpret_cast<ChiYUVSHDRNode *>(p_cback->user_data_Node);
    CHINODEPROCESSREQUESTDONEINFO info;
    INT framenum                = *(reinterpret_cast<int *>(p_cback->user_data_frame));
    LOG_INFO(CamxLogGroupChi,"Got Callback ShdrCompletionCallback Framenum : %d", framenum);

    ParsedIHistStatsOutput      ihist;
    memset(&ihist,           0x0, sizeof(ParsedIHistStatsOutput));
    max_val = std::max_element(input_param->stats_info->ihist, input_param->stats_info->ihist + MAX_SHDR_IHIST_STATS_SIZE);
    ihist_to16bit_ratio = ((*max_val)>>16);
    while (ihist_to16bit_ratio)
    {
        shift_bit++;
        ihist_to16bit_ratio >>= 1;
    }
    LOG_VERBOSE(CamxLogGroupChi,"Framenum : %d, max_val : %d, shift_bit : %d", framenum, *max_val, shift_bit);

    for (int i = 0; i < MAX_SHDR_IHIST_STATS_SIZE; i++)
    {
        if (shift_bit == 0)
        {
            ihist.imageHistogram.YCCHistogram[i] = static_cast<UINT16>(input_param->stats_info->ihist[i]);
        }
        else
        {
            ihist.imageHistogram.YCCHistogram[i] = static_cast<UINT16>((input_param->stats_info->ihist[i] >> shift_bit));
        }
    }
    ihist.numBins = 256; // ihist ready
    if (ihist_ready == IHIST_NOT_READY_SIGNAL)
    {
        ihist.numBins = 0; // ihist_ready == IHIST_NOT_READY_SIGNAL(-3) is ihist not ready
        LOG_VERBOSE(CamxLogGroupChi,"Framenum : %d, ihist not ready.", framenum);
    }
    else
    {
        LOG_VERBOSE(CamxLogGroupChi,"Framenum : %d, ihist ready.", framenum);
    }

    CHIMETADATAINFO        metadataInfo     = {0};
    const UINT32           tagSize          = 1;
    CHITAGDATA             tagData[tagSize] = { {0} };
    UINT32                 tagList[tagSize];
    metadataInfo.size                       = sizeof(CHIMETADATAINFO);
    metadataInfo.chiSession                 = pShdr->m_hChiSession;
    metadataInfo.tagNum                     = tagSize;
    metadataInfo.pTagList                   = &tagList[0];
    metadataInfo.pTagData                   = &tagData[0];

    UINT32 index                = 0;
    tagList[index]              = g_VendorTagList[1] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = framenum;
    tagData[index].pData        = &ihist;
    tagData[index].dataSize     = g_VendorTagSectionYUVSHDR[1].numUnits;
    g_ChiNodeInterface.pSetMetadata(&metadataInfo);


    info.size                   = sizeof(CHINODEPROCESSREQUESTDONEINFO);
    info.frameNum               = framenum;
    // need to add logic to return fail here
    info.result                 = CDKResultSuccess;
    info.hChiSession            = pShdr->m_hChiSession;
    info.isEarlyMetadataDone    = FALSE;
    g_ChiNodeInterface.pProcessRequestDone(&info);

    delete p_cback;
    p_cback = NULL;

}

CDKResult ChiYUVSHDRNode::QueryBufferInfo(
    CHINODEQUERYBUFFERINFO* pQueryBufferInfo)
{
    CDKResult result                     = CDKResultSuccess;
    if (pQueryBufferInfo->numInputPorts == 3 * DOWNSCALE_FRAMES)
    {
        shdrAlgoType = YUVSHDR_ALGO_3FRAME;
    }

    DefaultBufferNegotiation(pQueryBufferInfo);
    for (UINT32 i = 0; i < pQueryBufferInfo->numInputPorts; i++) {

    LOG_INFO(CamxLogGroupChi, "%d:Input Port [%d] min(%dx%d) max(%dx%d) opt(%dx%d), align(%dx%d)",i,
        pQueryBufferInfo->pInputOptions[i].inputPortId,
        pQueryBufferInfo->pInputOptions[i].inputBufferOption.minW,
        pQueryBufferInfo->pInputOptions[i].inputBufferOption.minH,
        pQueryBufferInfo->pInputOptions[i].inputBufferOption.maxW,
        pQueryBufferInfo->pInputOptions[i].inputBufferOption.maxH,
        pQueryBufferInfo->pInputOptions[i].inputBufferOption.optimalW,
        pQueryBufferInfo->pInputOptions[i].inputBufferOption.optimalH,
        pQueryBufferInfo->pInputOptions[i].inputBufferOption.planeAlignment[0].strideAlignment,
        pQueryBufferInfo->pInputOptions[i].inputBufferOption.planeAlignment[0].scanlineAlignment);
    }

    return result;
}

VOID ChiYUVSHDRNode::DefaultBufferNegotiation(
    CHINODEQUERYBUFFERINFO* pQueryBufferInfo)
{
    UINT      optimalInputWidth = 0;
    UINT      optimalInputHeight = 0;
    UINT32    minInputWidth = 0;
    UINT32    minInputHeight = 0;
    UINT32    maxInputWidth = 0xffff;
    UINT32    maxInputHeight = 0xffff;
    UINT      perOutputPortOptimalWidth = 0;
    UINT      perOutputPortOptimalHeight = 0;
    UINT32    perOutputPortMinWidth = 0;
    UINT32    perOutputPortMinHeight = 0;
    UINT32    perOutputPortMaxWidth = 0xffff;
    UINT32    perOutputPortMaxHeight = 0xffff;
    UINT32    isDS4Buffer            = 0;

    if (NULL != pQueryBufferInfo)
    {
        for (UINT outputIndex = 0; outputIndex < pQueryBufferInfo->numOutputPorts; outputIndex++)
        {
            ChiOutputPortQueryBufferInfo* pOutputPort = &pQueryBufferInfo->pOutputPortQueryInfo[outputIndex];

            perOutputPortOptimalWidth = 0;
            perOutputPortOptimalHeight = 0;
            isDS4Buffer                = 0;

            for (UINT input = 0; input < pOutputPort->numConnectedInputPorts; input++)
            {

                ChiNodeBufferRequirement* pInputPortRequirement = &pOutputPort->pBufferRequirement[input];
                if ( pInputPortRequirement->maxW == 0 || pInputPortRequirement->maxH == 0)
                {
                    break;
                }
                // Todo: Logic to detect DS4 buffer to skip buffer negotiation for that
                isDS4Buffer++;
                if (isDS4Buffer > 1)
                {
                    break;
                }
                LOG_INFO(CamxLogGroupChi, "Output dim (%d x %d) (%d x %d)(%d x %d)",
                        pInputPortRequirement->maxW, pInputPortRequirement->maxH,
                        pInputPortRequirement->minW, pInputPortRequirement->minH,
                        pInputPortRequirement->optimalW, pInputPortRequirement->optimalH);

                // Optimal width per port is the super resolution of all the connected destination ports' optimal needs .
                perOutputPortOptimalWidth = (pInputPortRequirement->optimalW >= perOutputPortOptimalWidth) ?
                    pInputPortRequirement->optimalW : perOutputPortOptimalWidth;
                perOutputPortOptimalHeight = (pInputPortRequirement->optimalH >= perOutputPortOptimalHeight) ?
                    pInputPortRequirement->optimalH : perOutputPortOptimalHeight;
                perOutputPortMinWidth = (pInputPortRequirement->minW >= perOutputPortMinWidth) ?
                    pInputPortRequirement->minW : perOutputPortMinWidth;
                perOutputPortMinHeight = (pInputPortRequirement->minH >= perOutputPortMinHeight) ?
                    pInputPortRequirement->minH : perOutputPortMinHeight;
                perOutputPortMaxWidth = (pInputPortRequirement->maxW <= perOutputPortMaxWidth) ?
                    pInputPortRequirement->maxW : perOutputPortMaxWidth;
                perOutputPortMaxHeight = (pInputPortRequirement->maxH <= perOutputPortMaxHeight) ?
                    pInputPortRequirement->maxH : perOutputPortMaxHeight;
            }

            // Ensure optimal dimensions are within min and max dimensions. There are chances that the optimal dimension goes
            // over the max. Correct for the same
            perOutputPortOptimalWidth = (((perOutputPortOptimalWidth) <= (perOutputPortMinWidth)) ? (perOutputPortMinWidth) :
                (((perOutputPortOptimalWidth) >= (perOutputPortMaxWidth)) ? (perOutputPortMaxWidth) :
                (perOutputPortOptimalWidth)));

            perOutputPortOptimalHeight = (((perOutputPortOptimalHeight) <= (perOutputPortMinHeight)) ?
                (perOutputPortMinHeight) :
                (((perOutputPortOptimalHeight) >= (perOutputPortMaxHeight)) ?
                (perOutputPortMaxHeight) :
                    (perOutputPortOptimalHeight)));

            pOutputPort->outputBufferOption.minW = perOutputPortMinWidth;
            pOutputPort->outputBufferOption.minH = perOutputPortMinHeight;
            pOutputPort->outputBufferOption.maxW = perOutputPortMaxWidth;
            pOutputPort->outputBufferOption.maxH = perOutputPortMaxHeight;
            pOutputPort->outputBufferOption.optimalW = perOutputPortOptimalWidth;
            pOutputPort->outputBufferOption.optimalH = perOutputPortOptimalHeight;

            optimalInputWidth = ((perOutputPortOptimalWidth > optimalInputWidth) ?
                perOutputPortOptimalWidth : optimalInputWidth);
            optimalInputHeight = ((perOutputPortOptimalHeight > optimalInputHeight) ?
                perOutputPortOptimalHeight : optimalInputHeight);
            minInputWidth = ((perOutputPortMinWidth > minInputWidth) ?
                perOutputPortMinWidth : minInputWidth);
            minInputHeight = ((perOutputPortMinHeight > minInputHeight) ?
                perOutputPortMinHeight : minInputHeight);
            maxInputWidth = ((perOutputPortMaxWidth < maxInputWidth) ? perOutputPortMaxWidth : maxInputWidth);
            maxInputHeight = ((perOutputPortMaxHeight < maxInputHeight) ? perOutputPortMaxHeight : maxInputHeight);

            LOG_VERBOSE(CamxLogGroupChi, "Input:  (w x h) optimal %d x %d min %d x %d max %d x %d", optimalInputWidth,
                optimalInputHeight, minInputWidth, minInputHeight, maxInputWidth, maxInputHeight);
            LOG_VERBOSE(CamxLogGroupChi, "Output: (w x h) optimal %d x %d min %d x %d max %d x %d", perOutputPortOptimalWidth,
                perOutputPortOptimalHeight, perOutputPortMinWidth, perOutputPortMinHeight,
                perOutputPortMaxWidth, perOutputPortMaxHeight);
        }

        for (UINT inputIndex = 0; inputIndex < pQueryBufferInfo->numInputPorts; inputIndex++)
        {
            ChiInputPortQueryBufferInfo* pInputOptions = &pQueryBufferInfo->pInputOptions[inputIndex];

            pInputOptions->inputBufferOption.minW = minInputWidth;
            pInputOptions->inputBufferOption.minH = minInputHeight;
            pInputOptions->inputBufferOption.maxW = maxInputWidth;
            pInputOptions->inputBufferOption.maxH = maxInputHeight;
            pInputOptions->inputBufferOption.optimalW = optimalInputWidth;
            pInputOptions->inputBufferOption.optimalH = optimalInputHeight;
        }
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Invalide argument: pQueryBufferInfo is NULL");
    }
}


VOID FillHardcodedTuning(SHDR3TUNING *tuning)
{
    memset(tuning, 0 ,sizeof(SHDR3TUNING));
    LOG_INFO(CamxLogGroupChi, "FillHardcodedTuning ");

    tuning->autoLTM3_enable=0;
    tuning->autoLTM3_param.max_gain_limit=1.0;
    tuning->autoLTM3_param.center=-0.360;
    tuning->autoLTM3_param.region=19.0;
    tuning->autoLTM3_param.pre_weight=0.8;
    tuning->autoLTM3_param.boost_upper_bound=-1.0;
    tuning->autoLTM3_param.boost_lower_bound=-5.0;
    tuning->autoLTM3_param.dark_start=10;
    tuning->autoLTM3_param.dark_end=40;
    tuning->autoLTM3_param.midtone_start=100;
    tuning->autoLTM3_param.midtone_end=130;
    tuning->autoLTM3_param.highlight_start=160;
    tuning->autoLTM3_param.highlight_end=190;
    tuning->LTM_enable=1;
    tuning->WB_r_gain=1.767928;
    tuning->WB_b_gain=1.971897;
    tuning->eplp_param.sigma=0.5;
    tuning->eplp_param.radius=7;
    tuning->eplp_param.eps=0.0000001;
    tuning->eplp_param.iterations=1;
    tuning->tm_param.gamma=-2.0;
    tuning->tm_param.gamma_region=0.0;
    tuning->tm_param.alpha=0.0;
    tuning->tm_param.highlight_compression_strength=-4.0;
    tuning->tm_param.boost_strength=-3.0;
    tuning->tm_param.factor=0.0;
    tuning->tm_param.max_total_gain=0.0;
    tuning->tm_param.pre_gamma=0.7;
    tuning->tm_param.max_gain=0.7;
    tuning->tm_param.min_gain=1.0;
    tuning->tm_param.method=6;
    tuning->tm_param.desat_th=1.0;
    tuning->tm_param.detail_smoothing_weight_pass_full=1.0;
    tuning->tm_param.detail_smoothing_weight_pass_dc4 =1.0;
    tuning->fast_param.hybird_heavy_percentage = 0.2;
    tuning->fast_param.fast_mode = FAST_MODE_HYBRID;
    tuning->blc_param.factor = 0.1;
    tuning->blc_param.up_limit = 256;
    tuning->de_param.enable_detail_enhancement=1;
    tuning->de_param.luma_adapation_a=-1.0;
    tuning->de_param.luma_adapation_b=1.0;
    tuning->de_param.luma_adapation_c=0.0;
    tuning->de_param.regulation_fat_thin=5.0;
    tuning->de_param.regulation_center=-1.2;
    tuning->fusion_param.EXPOSURE_RATIO_REAL[0]=7.880320;
    tuning->fusion_param.EXPOSURE_RATIO_REAL[1]=1.0;
    tuning->fusion_param.EXPOSURE_RATIO_REAL[2]=1.0;
    tuning->fusion_param.EXPOSURE_RATIO_REAL[3]=1.0;
    tuning->fusion_param.EXPOSURE_RATIO_SENSOR[0]=7.880320;
    tuning->fusion_param.EXPOSURE_RATIO_SENSOR[1]=1.0;
    tuning->fusion_param.EXPOSURE_RATIO_SENSOR[2]=1.0;
    tuning->fusion_param.EXPOSURE_RATIO_SENSOR[3]=1.0;
    tuning->fusion_param.enable_motion_adaptation=1;
    tuning->fusion_param.enable_debug_pair=0;
    tuning->fusion_param.morphology_decision=1;
    tuning->fusion_param.ma_luma_th1_exp[0]=767.0;
    tuning->fusion_param.ma_luma_th1_exp[1]=767.0;
    tuning->fusion_param.ma_luma_th1_exp[2]=767.0;
    tuning->fusion_param.ma_luma_th1_exp[3]=767.0;
    tuning->fusion_param.ma_luma_th2_exp[0]=1023.0;
    tuning->fusion_param.ma_luma_th2_exp[1]=1023.0;
    tuning->fusion_param.ma_luma_th2_exp[2]=1023.0;
    tuning->fusion_param.ma_luma_th2_exp[3]=1023.0;
    tuning->fusion_param.ma_diff_th1_exp[0]=70.0;
    tuning->fusion_param.ma_diff_th1_exp[1]=0.0;
    tuning->fusion_param.ma_diff_th1_exp[2]=0.0;
    tuning->fusion_param.ma_diff_th1_exp[3]=0.0;
    tuning->fusion_param.ma_diff_th2_exp[0]=140.0;
    tuning->fusion_param.ma_diff_th2_exp[1]=0.0;
    tuning->fusion_param.ma_diff_th2_exp[2]=0.0;
    tuning->fusion_param.ma_diff_th2_exp[3]=0.0;
    tuning->fusion_param.motion_factor[0]=1.0;
    tuning->fusion_param.motion_factor[1]=1.0;
    tuning->fusion_param.motion_factor[2]=1.0;
    tuning->fusion_param.blur_factor[0]=1.0;
    tuning->fusion_param.blur_factor[1]=1.0;
    tuning->fusion_param.blur_factor[2]=1.0;
    tuning->fusion_param.enable_ma_blurring[0]=1;
    tuning->fusion_param.enable_ma_blurring[1]=1;
    tuning->fusion_param.enable_ma_blurring[2]=1;
    tuning->fusion_param.enable_ma_multipass[0]=1;
    tuning->fusion_param.enable_ma_multipass[1]=1;
    tuning->fusion_param.enable_ma_multipass[2]=1;
    tuning->fusion_param.multipass_me_factor=4.0;
    //tuning->ENABLE_DEBUG_MODE=0;
}
static inline void apply_rt(FLOAT* _x, FLOAT* _y, FLOAT* rt)
{
    FLOAT x = *_x, y = *_y;
    FLOAT fw = x*rt[6] + y* rt[7] + rt[8];
    *_x = (x*rt[0] + y* rt[1] + rt[2]) / fw;
    *_y = (x*rt[3] + y* rt[4] + rt[5]) / fw;
}

VOID ChiYUVSHDRNode::FillIdenticalMesh(FLOAT* mesh, FLOAT total_margin,
    UINT input_width, UINT input_height,
    UINT mesh_num_x, UINT mesh_num_y)
{
    UINT x, y;
    FLOAT rt_prime[9];
    FLOAT* ptr = mesh;
    rt_prime[0] = 1.0f / total_margin;
    rt_prime[1] = 0.0f;
    rt_prime[2] = (1.0f - 1.0f / total_margin) / 2.0f * (FLOAT)input_width;
    rt_prime[3] = 0.0f;
    rt_prime[4] = 1.0f / total_margin;
    rt_prime[5] = (1.0f - 1.0f / total_margin) / 2.0f * (FLOAT)input_height;
    rt_prime[6] = 0.0f;
    rt_prime[7] = 0.0f;
    rt_prime[8] = 1.0f;

    INT i;
    FLOAT mired_input_width = 1.0f / (FLOAT)input_width;
    FLOAT mired_input_height = 1.0f / (FLOAT)input_height;

    for (y = 0; y < mesh_num_y; y++)
        for (x = 0; x < mesh_num_x; x++)
        {
            FLOAT x_f = (FLOAT)x*((FLOAT)input_width / (FLOAT)(mesh_num_x - 1));
            FLOAT y_f = (FLOAT)y*((FLOAT)input_height / (FLOAT)(mesh_num_y - 1));
            apply_rt(&x_f, &y_f, rt_prime);
            ptr[0] = x_f*mired_input_width;
            ptr[1] = y_f*mired_input_height;
            ptr += 2;
        }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::SetBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiYUVSHDRNode::SetBufferInfo(
    CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo)
{
    CDKResult result = CDKResultSuccess;

    LOG_INFO(CamxLogGroupChi, "SetBufferInfo Port [%d] Width=%d, Height=%d",
    pSetBufferInfo->portId,
        pSetBufferInfo->pFormat->width,
        pSetBufferInfo->pFormat->height);

    SHDR3INITCONFIG *init_config = &m_p_shdr_ctx->init_config;

    switch(pSetBufferInfo->portId)
    {
        case ChiNodeInputFull:
            init_config->fs_width        = pSetBufferInfo->pFormat->width;
            init_config->fs_height       = pSetBufferInfo->pFormat->height;
            init_config->fs_stride       = pSetBufferInfo->pFormat->width;
            init_config->fs_scanline     = pSetBufferInfo->pFormat->height; // need to change align with 64
        break;
        case ChiNodeINPUTDS4:
            init_config->dc4_width       = pSetBufferInfo->pFormat->width;
            init_config->dc4_height      = pSetBufferInfo->pFormat->height;
            init_config->dc4_stride      = pSetBufferInfo->pFormat->width;
            init_config->dc4_scanline    = pSetBufferInfo->pFormat->height;
        break;
        case ChiNodeINPUTDS16:
            init_config->dc16_width      = pSetBufferInfo->pFormat->width;
            init_config->dc16_height     = pSetBufferInfo->pFormat->height;
            init_config->dc16_stride     = pSetBufferInfo->pFormat->width;
            init_config->dc16_scanline   = pSetBufferInfo->pFormat->height;

            init_config->num_exposure               = 2;
            if (shdrAlgoType == YUVSHDR_ALGO_3FRAME)
            {
                init_config->num_exposure           = 3;
            }
            init_config->number_of_input_buffers    = 10;
            init_config->number_of_output_buffers   = 10;
            init_config->ds_flag                    = 2;
            init_config->perf_ind                   = 0;
            init_config->input_frame_format         = SHDR_INPUT_IMG_FORMAT_TP10;
            init_config->dump_print_en              = 1;
            init_config->frame_rate                 = 30;

            InitializeWarpMesh(init_config->num_exposure);
            SHDR3TUNING *tuning                     = &m_p_shdr_ctx->tuning;
            CHIDATASOURCE chiDataSource;
            chiDataSource.dataSourceType     = ChiTuningManager;
            chiDataSource.pHandle            = NULL;
            TuningSetManager* pTuningManager = static_cast<TuningSetManager*>(g_ChiNodeInterface.pGetData(m_hChiSession,
                                                      &chiDataSource,
                                                      NULL,
                                                      NULL));
            LOG_INFO(CamxLogGroupChi, "Tuning Manager : %p", pTuningManager);
            CAMX_ASSERT(NULL != pTuningManager);
            // Get Sensor Mode info
            VOID*     pSensorModeMetaData = NULL;
            pSensorModeMetaData = ChiNodeUtils::GetMetaData(0,
                                          (m_SHDR_sensorMode|UsecaseMetadataSectionMask),
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
            if (NULL != pSensorModeMetaData)
            {
                sensorMode = *(reinterpret_cast<UINT32*>(pSensorModeMetaData));
                LOG_INFO(CamxLogGroupChi, "SensorMode %d", sensorMode);
            }
            else
            {
                LOG_ERROR(CamxLogGroupChi, "SensorMode Not Available");
            }
            if (NULL != pTuningManager)
            {
                ReadTuningParams(pTuningManager , tuning);
            }
            if (CDKResultSuccess == result)
            {
                SHDR3INPUTPARAM input_param;
                memset(&input_param, 0 , sizeof(input_param));
                m_p_shdr_ctx->stats_info  = (SHDR3STATSINFO *)malloc(sizeof(SHDR3STATSINFO));
                // Only tuning is needed for lib init in input_param
                input_param.tuning          = tuning;
                // input_param.stats_info      = stats_info;
                INT32 ret_lib               = m_shdr_lib_init(&(m_p_shdr_ctx->handle), init_config, &input_param);

                if (0 != ret_lib)
                {
                    LOG_ERROR(CamxLogGroupChi, " Can not init library %d", ret_lib);
                    result = CDKResultEFailed;
                }
                else
                {
                    LOG_INFO(CamxLogGroupChi, "SHDR Library Init Succesful");
                }
            }
        break;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::updateShdrTuning
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VOID ChiYUVSHDRNode::updateShdrTuning(shdr3_tuning_t *pTuning)
{
    CHIDATASOURCE chiDataSource;
    chiDataSource.dataSourceType     = ChiTuningManager;
    chiDataSource.pHandle            = NULL;
    TuningSetManager* pTuningManager = static_cast<TuningSetManager*>(g_ChiNodeInterface.pGetData(m_hChiSession,
                                                      &chiDataSource,
                                                      NULL,
                                                      NULL));
	ChiTuningModeParameter                      Shdr3TuningData;
    shdr_3_0_0::shdr30Type* pChromatixYUVSHDR   = NULL;
    Shdr3TuningData.noOfSelectionParameter      = 1;
    Shdr3TuningData.TuningMode[0].mode          = ChiModeType::Default;
    Shdr3TuningData.TuningMode[0].subMode       = { 0 };
    TuningMode  tuningMode[2];
    tuningMode[0].mode          = ModeType::Default;
    tuningMode[0].subMode       = { 0 };
    tuningMode[1].mode          = ModeType::Sensor;
    tuningMode[1].subMode.value = sensorMode;
    pChromatixYUVSHDR            = pTuningManager->GetModule_shdr30(tuningMode, (sizeof(tuningMode)/sizeof(tuningMode[0])));
    pTuning->autoLTM3_param.max_gain_limit           = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_max_gain_limit;
    pTuning->autoLTM3_param.center                   = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_center;
    pTuning->autoLTM3_param.region                   = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_region;
    pTuning->autoLTM3_param.pre_weight               = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_pre_weight;
    pTuning->autoLTM3_param.boost_upper_bound        = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_boost_upper_bound;
    pTuning->autoLTM3_param.boost_lower_bound        = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_boost_lower_bound;
    pTuning->autoLTM3_param.dark_start               = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_dark_start;
    pTuning->autoLTM3_param.dark_end                 = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_dark_end;
    pTuning->autoLTM3_param.midtone_start            = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_midtone_start;
    pTuning->autoLTM3_param.midtone_end              = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_midtone_end;
    pTuning->autoLTM3_param.highlight_start          = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_highlight_start;
    pTuning->autoLTM3_param.highlight_end            = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_highlight_end;
    pTuning->LTM_enable                              = pChromatixYUVSHDR->LTM.gpu_LTM_enable;
    pTuning->eplp_param.sigma                        = pChromatixYUVSHDR->LTM.EPLP_sigma;
    pTuning->eplp_param.radius                       = pChromatixYUVSHDR->LTM.EPLP_radius;
    pTuning->eplp_param.eps                          = pChromatixYUVSHDR->LTM.low_pass_filter_LUT.exp_ratio_zone->zone.EPLP_eps; // not matching
    pTuning->eplp_param.iterations                   = pChromatixYUVSHDR->LTM.EPLP_iterations;
    pTuning->tm_param.gamma                          = pChromatixYUVSHDR->LTM.TM_gamma;
    pTuning->tm_param.gamma_region                   = pChromatixYUVSHDR->LTM.TM_gamma_region;
    pTuning->tm_param.alpha                          = pChromatixYUVSHDR->LTM.TM_alpha;
    pTuning->tm_param.highlight_compression_strength = pChromatixYUVSHDR->LTM.TM_highlight_compression_strength;
    pTuning->tm_param.boost_strength                 = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone->zone.TM_boost_strength;
    pTuning->tm_param.factor                         = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone->zone.TM_factor;
    pTuning->tm_param.max_total_gain                 = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone->zone.TM_max_total_gain;
    pTuning->tm_param.pre_gamma                      = pChromatixYUVSHDR->LTM.gamma_LUT.exp_ratio_zone->zone.TM_pre_gamma;
    pTuning->tm_param.max_gain                       = pChromatixYUVSHDR->LTM.maxgain_LUT.exp_ratio_zone->zone.TM_maxgain;
    pTuning->tm_param.min_gain                       = pChromatixYUVSHDR->LTM.mingain_LUT.exp_ratio_zone->zone.TM_mingain;
    pTuning->tm_param.method                         = pChromatixYUVSHDR->LTM.TM_method;
    pTuning->tm_param.desat_th                       = pChromatixYUVSHDR->LTM.desaturate_LUT.exp_ratio_zone->zone.TM_desat_th;
    pTuning->tm_param.detail_smoothing_weight_pass_full = pChromatixYUVSHDR->LTM.TM_detail_smoothing_weight_pass_full;
    pTuning->tm_param.detail_smoothing_weight_pass_dc4  = pChromatixYUVSHDR->LTM.TM_detail_smoothing_weight_pass_dc4;
    pTuning->fast_param.hybird_heavy_percentage         = pChromatixYUVSHDR->Fast_Mode.FAST_hybrid_heavy_percentage;
    pTuning->fast_param.fast_mode                       = (shdr3_fast_mode_t)pChromatixYUVSHDR->Fast_Mode.FAST_mode;
    pTuning->blc_param.factor                           = pChromatixYUVSHDR->BLC.BLC_factor;
    pTuning->blc_param.up_limit                         = pChromatixYUVSHDR->BLC.BLC_up_limit;
    pTuning->anti_banding_enable                        = pChromatixYUVSHDR->Anti_Banding.AB_anti_banding_enable;
    pTuning->de_param.enable_detail_enhancement      = pChromatixYUVSHDR->Detail_Enhancement.DE_enable_detail_enhancement;
    pTuning->de_param.luma_adapation_a               = pChromatixYUVSHDR->Detail_Enhancement.DE_luma_adapation_a;
    pTuning->de_param.luma_adapation_b               = pChromatixYUVSHDR->Detail_Enhancement.DE_luma_adapation_b;
    pTuning->de_param.luma_adapation_c               = pChromatixYUVSHDR->Detail_Enhancement.DE_luma_adapation_c;
    pTuning->de_param.regulation_fat_thin            = pChromatixYUVSHDR->Detail_Enhancement.DE_regulation_fat_thin;
    pTuning->de_param.regulation_center              = pChromatixYUVSHDR->Detail_Enhancement.DE_regulation_center;

    pTuning->fusion_param.enable_motion_adaptation   = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_motion_adaptation;
    pTuning->fusion_param.enable_debug_pair          = pChromatixYUVSHDR->DEBUG_MODE.FUSION_enable_debug_pair;
    pTuning->fusion_param.morphology_decision        = pChromatixYUVSHDR->Motion_Adaptation.FUSION_morphology_decision;
    pTuning->fusion_param.ma_luma_th1_exp[0]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp0;
    pTuning->fusion_param.ma_luma_th1_exp[1]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp1;
    pTuning->fusion_param.ma_luma_th1_exp[2]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp2;
    pTuning->fusion_param.ma_luma_th1_exp[3]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp3;
    pTuning->fusion_param.ma_luma_th2_exp[0]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp0;
    pTuning->fusion_param.ma_luma_th2_exp[1]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp1;
    pTuning->fusion_param.ma_luma_th2_exp[2]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp2;
    pTuning->fusion_param.ma_luma_th2_exp[3]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp3;
    pTuning->fusion_param.motion_factor[0]           = pChromatixYUVSHDR->Motion_Adaptation.FUSION_motion_factor0;
    pTuning->fusion_param.motion_factor[1]           = pChromatixYUVSHDR->Motion_Adaptation.FUSION_motion_factor1;
    pTuning->fusion_param.motion_factor[2]           = pChromatixYUVSHDR->Motion_Adaptation.FUSION_motion_factor2;
    pTuning->fusion_param.blur_factor[0]             = pChromatixYUVSHDR->Motion_Adaptation.FUSION_blur_factor0;
    pTuning->fusion_param.blur_factor[1]             = pChromatixYUVSHDR->Motion_Adaptation.FUSION_blur_factor1;
    pTuning->fusion_param.blur_factor[2]             = pChromatixYUVSHDR->Motion_Adaptation.FUSION_blur_factor2;
    pTuning->fusion_param.enable_ma_blurring[0]      = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_blurring0;
    pTuning->fusion_param.enable_ma_blurring[1]      = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_blurring1;
    pTuning->fusion_param.enable_ma_blurring[2]      = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_blurring2;
    pTuning->fusion_param.enable_ma_multipass[0]     = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_multipass0;
    pTuning->fusion_param.enable_ma_multipass[1]     = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_multipass1;
    pTuning->fusion_param.enable_ma_multipass[2]     = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_multipass2;
    pTuning->fusion_param.ma_diff_th1_exp[0]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th1_exp0;
    pTuning->fusion_param.ma_diff_th1_exp[1]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th1_exp1;
    pTuning->fusion_param.ma_diff_th1_exp[2]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th1_exp2;
    pTuning->fusion_param.ma_diff_th1_exp[3]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th1_exp3;
    pTuning->fusion_param.ma_diff_th2_exp[0]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th2_exp0;
    pTuning->fusion_param.ma_diff_th2_exp[1]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th2_exp1;
    pTuning->fusion_param.ma_diff_th2_exp[2]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th2_exp2;
    pTuning->fusion_param.ma_diff_th2_exp[3]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th2_exp3;
    pTuning->fusion_param.multipass_me_factor        = pChromatixYUVSHDR->Motion_Adaptation.me_factor_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_multipass_me_factor;
    //    pTuning->ENABLE_DEBUG_MODE                       = pChromatixYUVSHDR->DEBUG_MODE.ENABLE_DEBUG_MODE;
        memcpy(pTuning->fusion_param.ma_reserved_tables,
            pChromatixYUVSHDR->Motion_Adaptation.ma_level_based_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_reserved_tables_tab.FUSION_ma_reserved_tables, (64 * sizeof(FLOAT)));
        memcpy(pTuning->anti_banding_reserved,
            pChromatixYUVSHDR->Anti_Banding.AB_anti_banding_reserved.reserved, (16 * sizeof(FLOAT)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::ReadTuningParams
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiYUVSHDRNode::ReadTuningParams(TuningSetManager* pTuningManager, SHDR3TUNING *pTuning)
{
    CDK_UNUSED_PARAM(pTuning);  // remove all unused param
    CDK_UNUSED_PARAM(pTuningManager);



    ChiTuningModeParameter                      Shdr3TuningData;
    shdr_3_0_0::shdr30Type* pChromatixYUVSHDR   = NULL;
    Shdr3TuningData.noOfSelectionParameter      = 1;
    Shdr3TuningData.TuningMode[0].mode          = ChiModeType::Default;
    Shdr3TuningData.TuningMode[0].subMode       = { 0 };
    TuningMode  tuningMode[2];
    tuningMode[0].mode          = ModeType::Default;
    tuningMode[0].subMode       = { 0 };
    tuningMode[1].mode          = ModeType::Sensor;
    tuningMode[1].subMode.value = sensorMode;
    pChromatixYUVSHDR            = pTuningManager->GetModule_shdr30(tuningMode, (sizeof(tuningMode)/sizeof(tuningMode[0])));
    if (NULL == pChromatixYUVSHDR)
    {
        FillHardcodedTuning(pTuning);
    }
    else
    {
        LOG_INFO(CamxLogGroupChi, "Tuning Params Chromaix : %p and shdr_enable :%d antibanding :%d",
            pChromatixYUVSHDR, pChromatixYUVSHDR->enable_section.shdr_enable, pChromatixYUVSHDR->Anti_Banding.AB_anti_banding_enable);
#if 1

        memset(pTuning, 0 ,sizeof(SHDR3TUNING));
        // pTuning->autoLTM3_enable                         = pChromatixYUVSHDR->enable_section.shdr_enable;
        pTuning->autoLTM3_enable                         = 0; //enable afterwards

        pTuning->autoLTM3_param.max_gain_limit           = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_max_gain_limit;
        pTuning->autoLTM3_param.center                   = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_center;
        pTuning->autoLTM3_param.region                   = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_region;
        pTuning->autoLTM3_param.pre_weight               = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_pre_weight;
        pTuning->autoLTM3_param.boost_upper_bound        = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_boost_upper_bound;
        pTuning->autoLTM3_param.boost_lower_bound        = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_boost_lower_bound;
        pTuning->autoLTM3_param.dark_start               = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_dark_start;
        pTuning->autoLTM3_param.dark_end                 = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_dark_end;
        pTuning->autoLTM3_param.midtone_start            = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_midtone_start;
        pTuning->autoLTM3_param.midtone_end              = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_midtone_end;
        pTuning->autoLTM3_param.highlight_start          = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_highlight_start;
        pTuning->autoLTM3_param.highlight_end            = pChromatixYUVSHDR->AutoLTM.autoLTM_LUT.lux_idx_zone->zone.autoLTM3_highlight_end;
        pTuning->LTM_enable                              = pChromatixYUVSHDR->LTM.gpu_LTM_enable;
        pTuning->WB_r_gain                               = 1.767928;
        pTuning->WB_b_gain                               = 1.971897;
        pTuning->eplp_param.sigma                        = pChromatixYUVSHDR->LTM.EPLP_sigma;
        pTuning->eplp_param.radius                       = pChromatixYUVSHDR->LTM.EPLP_radius;
        pTuning->eplp_param.eps                          = pChromatixYUVSHDR->LTM.low_pass_filter_LUT.exp_ratio_zone->zone.EPLP_eps; // not matching
        pTuning->eplp_param.iterations                   = pChromatixYUVSHDR->LTM.EPLP_iterations;
        pTuning->tm_param.gamma                          = pChromatixYUVSHDR->LTM.TM_gamma;
        pTuning->tm_param.gamma_region                   = pChromatixYUVSHDR->LTM.TM_gamma_region;
        pTuning->tm_param.alpha                          = pChromatixYUVSHDR->LTM.TM_alpha;
        pTuning->tm_param.highlight_compression_strength = pChromatixYUVSHDR->LTM.TM_highlight_compression_strength;
        pTuning->tm_param.boost_strength                 = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone->zone.TM_boost_strength;
        pTuning->tm_param.factor                         = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone->zone.TM_factor;
        pTuning->tm_param.max_total_gain                 = pChromatixYUVSHDR->LTM.tone_mapping_LUT.exp_ratio_zone->zone.TM_max_total_gain;
        pTuning->tm_param.pre_gamma                      = pChromatixYUVSHDR->LTM.gamma_LUT.exp_ratio_zone->zone.TM_pre_gamma;
        pTuning->tm_param.max_gain                       = pChromatixYUVSHDR->LTM.maxgain_LUT.exp_ratio_zone->zone.TM_maxgain;
        pTuning->tm_param.min_gain                       = pChromatixYUVSHDR->LTM.mingain_LUT.exp_ratio_zone->zone.TM_mingain;
        pTuning->tm_param.method                         = pChromatixYUVSHDR->LTM.TM_method;
        pTuning->tm_param.desat_th                       = pChromatixYUVSHDR->LTM.desaturate_LUT.exp_ratio_zone->zone.TM_desat_th;
        pTuning->tm_param.detail_smoothing_weight_pass_full = pChromatixYUVSHDR->LTM.TM_detail_smoothing_weight_pass_full;
        pTuning->tm_param.detail_smoothing_weight_pass_dc4  = pChromatixYUVSHDR->LTM.TM_detail_smoothing_weight_pass_dc4;
        pTuning->fast_param.hybird_heavy_percentage         = pChromatixYUVSHDR->Fast_Mode.FAST_hybrid_heavy_percentage;
        pTuning->fast_param.fast_mode                    = (shdr3_fast_mode_t)pChromatixYUVSHDR->Fast_Mode.FAST_mode;
        pTuning->blc_param.factor                        = pChromatixYUVSHDR->BLC.BLC_factor;
        pTuning->blc_param.up_limit                      = pChromatixYUVSHDR->BLC.BLC_up_limit;
        LOG_INFO(CamxLogGroupChi, "blc :%f  %f ", pTuning->blc_param.factor, pTuning->blc_param.up_limit);

        pTuning->anti_banding_enable                     = pChromatixYUVSHDR->Anti_Banding.AB_anti_banding_enable;
        pTuning->de_param.enable_detail_enhancement      = pChromatixYUVSHDR->Detail_Enhancement.DE_enable_detail_enhancement;
        pTuning->de_param.luma_adapation_a               = pChromatixYUVSHDR->Detail_Enhancement.DE_luma_adapation_a;
        pTuning->de_param.luma_adapation_b               = pChromatixYUVSHDR->Detail_Enhancement.DE_luma_adapation_b;
        pTuning->de_param.luma_adapation_c               = pChromatixYUVSHDR->Detail_Enhancement.DE_luma_adapation_c;
        pTuning->de_param.regulation_fat_thin            = pChromatixYUVSHDR->Detail_Enhancement.DE_regulation_fat_thin;
        pTuning->de_param.regulation_center              = pChromatixYUVSHDR->Detail_Enhancement.DE_regulation_center;
        pTuning->fusion_param.EXPOSURE_RATIO_REAL[0]     = 7.880320;
        pTuning->fusion_param.EXPOSURE_RATIO_REAL[1]     = 1.0;
        pTuning->fusion_param.EXPOSURE_RATIO_REAL[2]     = 1.0;
        pTuning->fusion_param.EXPOSURE_RATIO_REAL[3]     = 1.0;
        pTuning->fusion_param.EXPOSURE_RATIO_SENSOR[0]   = 7.880320;
        pTuning->fusion_param.EXPOSURE_RATIO_SENSOR[1]   = 1.0;
        pTuning->fusion_param.EXPOSURE_RATIO_SENSOR[2]   = 1.0;
        pTuning->fusion_param.EXPOSURE_RATIO_SENSOR[3]   = 1.0;
        pTuning->fusion_param.enable_motion_adaptation   = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_motion_adaptation;
        pTuning->fusion_param.enable_debug_pair          = pChromatixYUVSHDR->DEBUG_MODE.FUSION_enable_debug_pair;
        pTuning->fusion_param.morphology_decision        = pChromatixYUVSHDR->Motion_Adaptation.FUSION_morphology_decision;
        pTuning->fusion_param.ma_luma_th1_exp[0]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp0;
        pTuning->fusion_param.ma_luma_th1_exp[1]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp1;
        pTuning->fusion_param.ma_luma_th1_exp[2]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp2;
        pTuning->fusion_param.ma_luma_th1_exp[3]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th1_exp3;
        pTuning->fusion_param.ma_luma_th2_exp[0]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp0;
        pTuning->fusion_param.ma_luma_th2_exp[1]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp1;
        pTuning->fusion_param.ma_luma_th2_exp[2]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp2;
        pTuning->fusion_param.ma_luma_th2_exp[3]         = pChromatixYUVSHDR->Motion_Adaptation.luma_threshold_LUT.exp_ratio_zone->zone.FUSION_ma_luma_th2_exp3;
        pTuning->fusion_param.motion_factor[0]           = pChromatixYUVSHDR->Motion_Adaptation.FUSION_motion_factor0;
        pTuning->fusion_param.motion_factor[1]           = pChromatixYUVSHDR->Motion_Adaptation.FUSION_motion_factor1;
        pTuning->fusion_param.motion_factor[2]           = pChromatixYUVSHDR->Motion_Adaptation.FUSION_motion_factor2;
        pTuning->fusion_param.blur_factor[0]             = pChromatixYUVSHDR->Motion_Adaptation.FUSION_blur_factor0;
        pTuning->fusion_param.blur_factor[1]             = pChromatixYUVSHDR->Motion_Adaptation.FUSION_blur_factor1;
        pTuning->fusion_param.blur_factor[2]             = pChromatixYUVSHDR->Motion_Adaptation.FUSION_blur_factor2;
        pTuning->fusion_param.enable_ma_blurring[0]      = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_blurring0;
        pTuning->fusion_param.enable_ma_blurring[1]      = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_blurring1;
        pTuning->fusion_param.enable_ma_blurring[2]      = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_blurring2;
        pTuning->fusion_param.enable_ma_multipass[0]     = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_multipass0;
        pTuning->fusion_param.enable_ma_multipass[1]     = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_multipass1;
        pTuning->fusion_param.enable_ma_multipass[2]     = pChromatixYUVSHDR->Motion_Adaptation.FUSION_enable_ma_multipass2;
        pTuning->fusion_param.ma_diff_th1_exp[0]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th1_exp0;
        pTuning->fusion_param.ma_diff_th1_exp[1]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th1_exp1;
        pTuning->fusion_param.ma_diff_th1_exp[2]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th1_exp2;
        pTuning->fusion_param.ma_diff_th1_exp[3]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th1_exp3;
        pTuning->fusion_param.ma_diff_th2_exp[0]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th2_exp0;
        pTuning->fusion_param.ma_diff_th2_exp[1]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th2_exp1;
        pTuning->fusion_param.ma_diff_th2_exp[2]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th2_exp2;
        pTuning->fusion_param.ma_diff_th2_exp[3]         = pChromatixYUVSHDR->Motion_Adaptation.diff_threshold_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_diff_th2_exp3;
        pTuning->fusion_param.multipass_me_factor        = pChromatixYUVSHDR->Motion_Adaptation.me_factor_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_multipass_me_factor;
        //pTuning->ENABLE_DEBUG_MODE                       = pChromatixYUVSHDR->DEBUG_MODE.ENABLE_DEBUG_MODE;
        memcpy(pTuning->fusion_param.ma_reserved_tables,
            pChromatixYUVSHDR->Motion_Adaptation.ma_level_based_LUT.exp_ratio_zone->zone.gain_zone->zone.FUSION_ma_reserved_tables_tab.FUSION_ma_reserved_tables, (64 * sizeof(FLOAT)));
        memcpy(pTuning->anti_banding_reserved,
            pChromatixYUVSHDR->Anti_Banding.AB_anti_banding_reserved.reserved, (16 * sizeof(FLOAT)));

        CAMX_ASSERT(NULL != pChromatixYUVSHDR);
#endif
        SetupInterpolationData(pChromatixYUVSHDR);
        m_InterpolationResourceAllocated = TRUE;
    }
    m_tuning_de_param = pTuning->de_param;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::InitializeWarpMesh
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiYUVSHDRNode::InitializeWarpMesh(UINT numExposure)
{
    unsigned int dewarp_mesh_size           = DEWARP_NUM_MESH_H*DEWARP_NUM_MESH_V * 2;
    for (UINT req = 0; req < DefaultRequestQueueDepth; req++)
    {
        for (UINT exp = 0; exp < numExposure; exp++)
        {
            m_p_shdr_ctx->warp_mesh[req][exp].mesh.width       = DEWARP_NUM_MESH_H;
            m_p_shdr_ctx->warp_mesh[req][exp].mesh.height      = DEWARP_NUM_MESH_V;
            m_p_shdr_ctx->warp_mesh[req][exp].mesh.stride      = DEWARP_NUM_MESH_H * 2 * sizeof(float);
            m_p_shdr_ctx->warp_mesh[req][exp].mesh.buf_size    = dewarp_mesh_size * sizeof(float);
            m_p_shdr_ctx->warp_mesh[req][exp].mesh.offset      = 0;
            m_p_shdr_ctx->warp_mesh[req][exp].x                = DEWARP_NUM_MESH_H;
            m_p_shdr_ctx->warp_mesh[req][exp].y                = DEWARP_NUM_MESH_V;
            warp_buf_handle[req][exp]                          = 0; // change with shdr header provided handle.
            m_p_shdr_ctx->warp_mesh[req][exp].mesh.buf         =
                    (unsigned char*)AllocIonBuffer((size_t)m_p_shdr_ctx->warp_mesh[req][exp].mesh.buf_size,
                                &(m_p_shdr_ctx->warp_mesh[req][exp].mesh.fd), &warp_buf_handle[req][exp]);
            FullfillIdenticalMesh((float*)(m_p_shdr_ctx->warp_mesh[req][exp].mesh.buf),
                        1.0f, m_p_shdr_ctx->init_config.fs_width, m_p_shdr_ctx->init_config.fs_height,
                        m_p_shdr_ctx->init_config.fs_width, m_p_shdr_ctx->init_config.fs_height,
                        DEWARP_NUM_MESH_H, DEWARP_NUM_MESH_V);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::CheckDependency
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChiYUVSHDRNode::CheckDependency(
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

    if (IsDefogEnabled(pProcessRequestInfo->frameNum))
    {
        if ((pProcessRequestInfo->frameNum - m_DefogEnableStartFrameNum) >= Defog_Latency)
        {
            UINT32 tag  = m_DEDataVendorTagBase;
            pData       = ChiNodeUtils::GetMetaData((pProcessRequestInfo->frameNum -Defog_Latency) , tag, ChiMetadataDynamic,
                                                    &g_ChiNodeInterface, m_hChiSession);

            if (NULL == pData)
            {
                pDependencyInfo->properties[dependencyCount]    = tag;
                pDependencyInfo->offsets[dependencyCount]       = Defog_Latency;
                dependencyCount++;
            }
        }
    }
    /*
    // Fixme
    tag = m_AECDataVendorTagBase;
    pData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum, tag, ChiMetadataDynamic,
                                      &g_ChiNodeInterface, m_hChiSession);
    // if the dependent tag is not published yet, set it as dependency
    if (NULL == pData)
    {
        pDependencyInfo->properties[dependencyCount]    = tag;
        pDependencyInfo->offsets[dependencyCount]       = 0;
        dependencyCount++;
        LOG_ERROR(CamxLogGroupChi, "AEC");
    }*/


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

BOOL ChiYUVSHDRNode::IsDefogEnabled(UINT64 framenum)
{
    // IS CE also required?
    VOID*  pDefogEnableMeta      = NULL;
    BOOL   IsDefogEnable;
    pDefogEnableMeta = ChiNodeUtils::GetMetaData(framenum,
                                            (m_DefogEnableVendorTagBase)|InputMetadataSectionMask,
                                            ChiMetadataDynamic,
                                            &g_ChiNodeInterface,
                                            m_hChiSession);
    if (NULL != pDefogEnableMeta)
    {
        IsDefogEnable =  *reinterpret_cast<BOOL*>(pDefogEnableMeta);
        if (IsDefogEnable == FALSE)
        {
            m_IsDefog_Enabled          = FALSE;
            return FALSE;
        }
        if (m_IsDefog_Enabled == FALSE)
        {
            m_DefogEnableStartFrameNum = framenum;
            m_IsDefog_Enabled          = TRUE;
        }
        return TRUE;
    }
    return FALSE;
}


CDKResult ChiYUVSHDRNode::FillBufferInfo(CHINODEPROCESSREQUESTINFO* pProcessRequestInfo,
SHDR3INPUTPARAM *input_param)
{
    CDKResult result = CDKResultSuccess;
    if (pProcessRequestInfo->inputNum % pProcessRequestInfo->outputNum) {
        LOG_ERROR(CamxLogGroupChi, "Input output port number mismatch %d %d",
        pProcessRequestInfo->inputNum, pProcessRequestInfo->outputNum);
        return CDKResultEFailed;
    }
    UINT numExp = m_p_shdr_ctx->init_config.num_exposure;
    UINT portId;
    for (UINT i = 0; i < numExp; i++) {
        input_param->offset_x[i] = 0;//fixme
        input_param->offset_y[i] = 0;//fixme
        portId = YUVSHDR_NODE_IN_FS_PORT_ID + i * YUVSHDR_NODE_EXP_PORT_ID_OFFS;
        if (CDKResultSuccess !=
            FillSingleBuffer(pProcessRequestInfo->phInputBuffer[portId],
                             &input_param->input_buffer[i])) {

            LOG_ERROR(CamxLogGroupChi, "Input port[%d] error", portId);
            result = CDKResultEFailed;
            break;
        }

        portId = YUVSHDR_NODE_IN_DS4_PORT_ID + i * YUVSHDR_NODE_EXP_PORT_ID_OFFS;
        if (CDKResultSuccess !=
            FillSingleBuffer(pProcessRequestInfo->phInputBuffer[portId],
                             &input_param->in_dc4_addr[i])) {
            LOG_ERROR(CamxLogGroupChi, "Input port[%d] error", portId);
            result = CDKResultEFailed;
            break;
        }
        portId = YUVSHDR_NODE_IN_DS16_PORT_ID + i * YUVSHDR_NODE_EXP_PORT_ID_OFFS;
        if (CDKResultSuccess !=
            FillSingleBuffer(pProcessRequestInfo->phInputBuffer[portId],
                             &input_param->in_dc16_addr[i])) {
            LOG_ERROR(CamxLogGroupChi, "Input port[%d] error", portId);
            result = CDKResultEFailed;
            break;
        }
    }

    portId = YUVSHDR_NODE_OUT_FS_PORT_ID;
    if (result == CDKResultSuccess && CDKResultSuccess !=
        FillSingleBuffer(pProcessRequestInfo->phOutputBuffer[portId],
                         &input_param->output_buffer)) {

        LOG_ERROR(CamxLogGroupChi, "Output port[%d] error", portId);
        result = CDKResultEFailed;
    }

    portId = YUVSHDR_NODE_OUT_DS4_PORT_ID;
    if (result == CDKResultSuccess && CDKResultSuccess !=
        FillSingleBuffer(pProcessRequestInfo->phOutputBuffer[portId],
                         &input_param->out_dc4_addr)) {
        LOG_ERROR(CamxLogGroupChi, "Output port[%d] error", portId);
        result = CDKResultEFailed;
    }
    portId = YUVSHDR_NODE_OUT_DS16_PORT_ID;
    if (result == CDKResultSuccess && CDKResultSuccess !=
        FillSingleBuffer(pProcessRequestInfo->phOutputBuffer[portId],
                         &input_param->out_dc16_addr)) {
        LOG_ERROR(CamxLogGroupChi, "Output port[%d] error", portId);
        result = CDKResultEFailed;
    }
    return result;
}

VOID ChiYUVSHDRNode::FillBuffer(SHDR3BUFFER* pLibInputBuffer, CHINODEBUFFERHANDLE pInputBuffer)
{
    pLibInputBuffer->width      = pInputBuffer->format.width;
    pLibInputBuffer->height     = pInputBuffer->format.height;
    if (PD10 == pInputBuffer->format.format)
    {
        pLibInputBuffer->stride     = (CamX::Utils::AlignGeneric32(CamX::Utils::EvenCeilingUINT32(pInputBuffer->format.width /2) * 8, 192))/4;
    }
    else
    {
        pLibInputBuffer->stride     = pInputBuffer->format.width;
    }
    pLibInputBuffer->buf_size   = pInputBuffer->planeSize[0] + pInputBuffer->planeSize[1];
    pLibInputBuffer->offset     = 0;  //m_p_shdr_ctx->init_config.input_buf[0].offset;
    pLibInputBuffer->buf        = pInputBuffer->pImageList[0].pAddr[0];
    pLibInputBuffer->fd         = pInputBuffer->pImageList[0].fd[0];
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::ProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiYUVSHDRNode::ProcessRequest(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{

    CDKResult result = CDKResultSuccess;
    SHDRCALLBACK *p_cback = new SHDRCALLBACK;
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
    }
    LOG_VERBOSE(CamxLogGroupChi, "ReqId : %d ProcessRequest num Input :%d and Output :%d",
            (int)pProcessRequestInfo->frameNum, pProcessRequestInfo->inputNum, pProcessRequestInfo->outputNum);

    UINT inNum = pProcessRequestInfo->inputNum;
    UINT outNum = pProcessRequestInfo->outputNum;
    m_p_shdr_ctx->input_param.tuning      = &m_p_shdr_ctx->tuning;
    m_p_shdr_ctx->input_param.stats_info  = m_p_shdr_ctx->stats_info;

    // ToDo: If only FULL out is supported.
    UINT j =0;
    LOG_VERBOSE(CamxLogGroupChi, "ReqId : %d buffer :%p and %p ",
        (int)pProcessRequestInfo->frameNum,
        (pProcessRequestInfo->phInputBuffer[0])->pImageList[0].pAddr[0],
        (pProcessRequestInfo->phInputBuffer[3])->pImageList[0].pAddr[0]);

    UINT numExposure      = m_p_shdr_ctx->init_config.num_exposure;
    UINT downscaleFlag    = m_p_shdr_ctx->init_config.ds_flag;             // check if this can be used
    for (UINT i = 0, j =0; i < m_p_shdr_ctx->init_config.num_exposure; i++) // change the logic
    {
        m_p_shdr_ctx->input_param.offset_x[i]                = 0;
        m_p_shdr_ctx->input_param.offset_y[i]                = 0;

        memcpy(&(m_p_shdr_ctx->input_param.warp_mesh[i]),
                &(m_p_shdr_ctx->warp_mesh[pProcessRequestInfo->frameNum % DefaultRequestQueueDepth][i]),
                sizeof(SHDR3WARPMESH));
        FillBuffer(&(m_p_shdr_ctx->input_param.input_buffer[i]), pProcessRequestInfo->phInputBuffer[j++]);
        FillBuffer(&(m_p_shdr_ctx->input_param.in_dc4_addr[i]) , pProcessRequestInfo->phInputBuffer[j++]);
        FillBuffer(&(m_p_shdr_ctx->input_param.in_dc16_addr[i]), pProcessRequestInfo->phInputBuffer[j++]);
        LOG_VERBOSE(CamxLogGroupChi, "ReqId : %d ProcessRequest input buf w=%d, h=%d, st=%d, buf_size=%d,buf=%p,fd=%d",
            (int)pProcessRequestInfo->frameNum, m_p_shdr_ctx->input_param.input_buffer[i].width,
            m_p_shdr_ctx->input_param.input_buffer[i].height, m_p_shdr_ctx->input_param.input_buffer[i].stride,
            m_p_shdr_ctx->input_param.input_buffer[i].buf_size, m_p_shdr_ctx->input_param.input_buffer[i].buf,
            m_p_shdr_ctx->input_param.input_buffer[i].fd);

        LOG_VERBOSE(CamxLogGroupChi, "ReqId : %d ProcessRequest DS4 buf w=%d, h=%d, st=%d, buf_size=%d,buf=%p,fd=%d",
                (int)pProcessRequestInfo->frameNum, m_p_shdr_ctx->input_param.in_dc4_addr[i].width,
                m_p_shdr_ctx->input_param.in_dc4_addr[i].height, m_p_shdr_ctx->input_param.in_dc4_addr[i].stride,
                m_p_shdr_ctx->input_param.in_dc4_addr[i].buf_size, m_p_shdr_ctx->input_param.in_dc4_addr[i].buf,
                m_p_shdr_ctx->input_param.in_dc4_addr[i].fd);

        LOG_VERBOSE(CamxLogGroupChi, "ReqId : %d ProcessRequest DS16 buf w=%d, h=%d, st=%d, buf_size=%d,buf=%p,fd=%d",
                (int)pProcessRequestInfo->frameNum, m_p_shdr_ctx->input_param.in_dc16_addr[i].width,
                m_p_shdr_ctx->input_param.in_dc16_addr[i].height, m_p_shdr_ctx->input_param.in_dc16_addr[i].stride,
                m_p_shdr_ctx->input_param.in_dc16_addr[i].buf_size, m_p_shdr_ctx->input_param.in_dc16_addr[i].buf,
                m_p_shdr_ctx->input_param.in_dc16_addr[i].fd);
    }
    FillBuffer(&(m_p_shdr_ctx->input_param.output_buffer), pProcessRequestInfo->phOutputBuffer[0]); // change
    FillBuffer(&(m_p_shdr_ctx->input_param.out_dc4_addr) , pProcessRequestInfo->phOutputBuffer[1]);
    FillBuffer(&(m_p_shdr_ctx->input_param.out_dc16_addr), pProcessRequestInfo->phOutputBuffer[2]);

    VOID*  pMetaData      = NULL;

    FLOAT  expRatio             = m_p_shdr_ctx->input_param.tuning->fusion_param.EXPOSURE_RATIO_REAL[0];
    FLOAT  expRatio1            = m_p_shdr_ctx->input_param.tuning->fusion_param.EXPOSURE_RATIO_REAL[0];
    FLOAT  sensorexpRatio       = m_p_shdr_ctx->input_param.tuning->fusion_param.EXPOSURE_RATIO_REAL[0];
    FLOAT  sensorexpRatio1      = m_p_shdr_ctx->input_param.tuning->fusion_param.EXPOSURE_RATIO_REAL[0];
    FLOAT  pAWBRGainData  = 1.0;
    FLOAT  pAWBGGainData  = 1.0;
    FLOAT  pAWBBGainData  = 1.0;
    FLOAT  luxIndex       = 1.0;
    FLOAT  AecShortExpGain = 1.0;

    // Get AEC info
    pMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          (m_AECDataVendorTagBase),
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
    if (NULL != pMetaData)
    {
        AECFrameControl* pAECData = reinterpret_cast<AECFrameControl*>(pMetaData);

        if ((pAECData->exposureInfo[AECAlgoExposureLong].sensitivity != 0.0) &&
            (pAECData->exposureInfo[AECAlgoExposureShort].sensitivity != 0.0))
        {
            if (shdrAlgoType == YUVSHDR_ALGO_2FRAME)
            {
               if((pAECData->realExposureInfo[AECAlgoExposureLong].sensitivity != 0.0) &&
                  (pAECData->realExposureInfo[AECAlgoExposureShort].sensitivity != 0.0))
               {

                    sensorexpRatio = (FLOAT)((FLOAT)pAECData->exposureInfo[AECAlgoExposureLong].sensitivity /
                                        (FLOAT)pAECData->exposureInfo[AECAlgoExposureShort].sensitivity);
                    expRatio       = (FLOAT)((FLOAT)pAECData->realExposureInfo[AECAlgoExposureLong].sensitivity /
                                        (FLOAT)pAECData->realExposureInfo[AECAlgoExposureShort].sensitivity);
                    LOG_VERBOSE(CamxLogGroupChi, "AEC ReqId : %d and expRatio:%f sensorexpRatio:%f and lux idx : %f AecShortExpGain :%f",
                        (int)pProcessRequestInfo->frameNum, expRatio, sensorexpRatio, pAECData->luxIndex, pAECData->exposureInfo[AECAlgoExposureShort].linearGain);
                    LOG_VERBOSE(CamxLogGroupChi, "Lexposure : %lld and Sexposure:%lld Sensor Lexposure : %lld and Sexposure:%lld",
                        (long long int)pAECData->exposureInfo[AECAlgoExposureLong].exposureTime,
                        (long long int)pAECData->exposureInfo[AECAlgoExposureShort].exposureTime,
                        (long long int)pAECData->realExposureInfo[AECAlgoExposureLong].exposureTime,
                        (long long int)pAECData->realExposureInfo[AECAlgoExposureLong].exposureTime);
                    luxIndex = pAECData->luxIndex;
                    AecShortExpGain = pAECData->exposureInfo[AECAlgoExposureShort].linearGain;
               }
            }
            else if (shdrAlgoType == YUVSHDR_ALGO_3FRAME)
            {
                if (pAECData->exposureInfo[AECAlgoExposureSafe].sensitivity != 0.0)
                {
                    sensorexpRatio = (FLOAT)((FLOAT)pAECData->exposureInfo[AECAlgoExposureLong].sensitivity /
                            (FLOAT)pAECData->exposureInfo[AECAlgoExposureSafe].sensitivity);
                    sensorexpRatio1 = (FLOAT)((FLOAT)pAECData->exposureInfo[AECAlgoExposureSafe].sensitivity /
                            (FLOAT)pAECData->exposureInfo[AECAlgoExposureShort].sensitivity);
                    expRatio = (FLOAT)((FLOAT)pAECData->realExposureInfo[AECAlgoExposureLong].sensitivity /
                        (FLOAT)pAECData->realExposureInfo[AECAlgoExposureSafe].sensitivity);
                    expRatio1 = (FLOAT)((FLOAT)pAECData->realExposureInfo[AECAlgoExposureSafe].sensitivity /
                        (FLOAT)pAECData->realExposureInfo[AECAlgoExposureShort].sensitivity);
                    LOG_VERBOSE(CamxLogGroupChi, "AEC ReqId : %d and expRatio:%f %f  sensorexpRatio:%f %f and lux idx : %f AecShortExpGain :%f",
                        (int)pProcessRequestInfo->frameNum, expRatio, expRatio1, sensorexpRatio, sensorexpRatio1,
                         pAECData->luxIndex, pAECData->exposureInfo[AECAlgoExposureShort].linearGain);
                    LOG_VERBOSE(CamxLogGroupChi, "Lexposure : %lld and %lld  Sexposure:%lld Sensor Lexposure : %lld and %lld Sexposure:%lld",
                        (long long int)pAECData->exposureInfo[AECAlgoExposureLong].exposureTime,
                        (long long int)pAECData->exposureInfo[AECAlgoExposureSafe].exposureTime,
                        (long long int)pAECData->exposureInfo[AECAlgoExposureShort].exposureTime,
                        (long long int)pAECData->realExposureInfo[AECAlgoExposureLong].exposureTime,
                        (long long int)pAECData->realExposureInfo[AECAlgoExposureSafe].exposureTime,
                        (long long int)pAECData->realExposureInfo[AECAlgoExposureShort].exposureTime);
                    luxIndex = pAECData->luxIndex;
                    AecShortExpGain = pAECData->exposureInfo[AECAlgoExposureShort].linearGain;
                }
            }
        }
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "AEC Meta is NULL");
    }
    if (expRatio <=1.0f)
    {
        expRatio = DEFAULT_EXP_RATIO;
    }

    m_p_shdr_ctx->input_param.tuning->fusion_param.EXPOSURE_RATIO_REAL[0]   = expRatio;
    m_p_shdr_ctx->input_param.tuning->fusion_param.EXPOSURE_RATIO_SENSOR[0] = sensorexpRatio;
    if (shdrAlgoType == YUVSHDR_ALGO_3FRAME)
    {

        m_p_shdr_ctx->input_param.tuning->fusion_param.EXPOSURE_RATIO_REAL[1]   = expRatio1;
        m_p_shdr_ctx->input_param.tuning->fusion_param.EXPOSURE_RATIO_SENSOR[1] = sensorexpRatio1;
    }

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
    m_p_shdr_ctx->input_param.tuning->WB_r_gain = (pAWBRGainData) / (pAWBGGainData);
    m_p_shdr_ctx->input_param.tuning->WB_b_gain = (pAWBBGainData )/ (pAWBGGainData);

    if (enableLiveTuning)
    {
        updateShdrTuning(m_p_shdr_ctx->input_param.tuning);
    }
        trigger_interpolation_update(m_p_shdr_ctx->input_param.tuning, expRatio, AecShortExpGain, luxIndex);

    if (((m_IsDefog_Enabled) && ((pProcessRequestInfo->frameNum - m_DefogEnableStartFrameNum) >= Defog_Latency)))
    {
        VOID*  pDEData  = NULL;
        pDEData         = ChiNodeUtils::GetMetaData((pProcessRequestInfo->frameNum-Defog_Latency), m_DEDataVendorTagBase, ChiMetadataDynamic,
                                                &g_ChiNodeInterface, m_hChiSession);
        if (NULL == pDEData)
        {
            LOG_ERROR(CamxLogGroupChi, "DE data should not be NULL in Defog , Hardcoding to Default Meta");
            m_p_shdr_ctx->input_param.tuning->de_param = m_tuning_de_param;
        }
        else
        {
            memcpy(&(m_p_shdr_ctx->input_param.tuning->de_param), pDEData, sizeof(shdr3_tuning_de_t));
            // m_p_shdr_ctx->input_param.tuning->de_param = m_tuning_de_param;
        }
    }
    else
    {
        m_p_shdr_ctx->input_param.tuning->de_param = m_tuning_de_param;
    }

    LOG_VERBOSE(CamxLogGroupChi, "ReqID : %d WB :%f and %f expratio :%f", (int)pProcessRequestInfo->frameNum,
        m_p_shdr_ctx->input_param.tuning->WB_r_gain,
        m_p_shdr_ctx->input_param.tuning->WB_b_gain, m_p_shdr_ctx->input_param.tuning->fusion_param.EXPOSURE_RATIO_REAL[0]);


    LOG_VERBOSE(CamxLogGroupChi, "ReqId : %d ProcessRequest output buf w=%d, h=%d, st=%d, buf_size=%d,buf=%p,fd=%d",
        (int)pProcessRequestInfo->frameNum, m_p_shdr_ctx->input_param.output_buffer.width,
        m_p_shdr_ctx->input_param.output_buffer.height, m_p_shdr_ctx->input_param.output_buffer.stride,
        m_p_shdr_ctx->input_param.output_buffer.buf_size, m_p_shdr_ctx->input_param.output_buffer.buf,
        m_p_shdr_ctx->input_param.output_buffer.fd);


    LOG_VERBOSE(CamxLogGroupChi, "ReqId : %d ProcessRequest Ds4 buf w=%d, h=%d, st=%d, buf_size=%d,buf=%p,fd=%d",
        (int)pProcessRequestInfo->frameNum, m_p_shdr_ctx->input_param.out_dc4_addr.width,
        m_p_shdr_ctx->input_param.out_dc4_addr.height, m_p_shdr_ctx->input_param.out_dc4_addr.stride,
        m_p_shdr_ctx->input_param.out_dc4_addr.buf_size, m_p_shdr_ctx->input_param.out_dc4_addr.buf,
        m_p_shdr_ctx->input_param.out_dc4_addr.fd);


    LOG_VERBOSE(CamxLogGroupChi, "ReqId : %d ProcessRequest Ds16 buf w=%d, h=%d, st=%d, buf_size=%d,buf=%p,fd=%d",
        (int)pProcessRequestInfo->frameNum, m_p_shdr_ctx->input_param.out_dc16_addr.width,
        m_p_shdr_ctx->input_param.out_dc16_addr.height, m_p_shdr_ctx->input_param.out_dc16_addr.stride,
        m_p_shdr_ctx->input_param.out_dc16_addr.buf_size, m_p_shdr_ctx->input_param.out_dc16_addr.buf,
        m_p_shdr_ctx->input_param.out_dc16_addr.fd);
    LOG_VERBOSE(CamxLogGroupChi, "stats_info ptr :%p", m_p_shdr_ctx->input_param.stats_info);


    p_cback->user_data_Node          = this;
    int frameIdtoSendtoStatsCallback = static_cast<int>(pProcessRequestInfo->frameNum);
    p_cback->p_shdr_ctx              = m_p_shdr_ctx;
    frameNum[frameIdtoSendtoStatsCallback % MaxRequests] = frameIdtoSendtoStatsCallback;
    p_cback->user_data_frame         = &frameNum[frameIdtoSendtoStatsCallback % MaxRequests];
    m_p_shdr_ctx->input_param.stats_info->frame_id = frameIdtoSendtoStatsCallback;

    LOG_VERBOSE(CamxLogGroupChi, "shdr_lib_process for frame id:%d", m_p_shdr_ctx->input_param.stats_info->frame_id);
    {
        FLOAT tuningParam[8];
        tuningParam[0]       = m_p_shdr_ctx->input_param.tuning->tm_param.pre_gamma;
        tuningParam[1]       = m_p_shdr_ctx->input_param.tuning->tm_param.boost_strength;
        tuningParam[2]       = m_p_shdr_ctx->input_param.tuning->tm_param.factor;
        tuningParam[3]       = m_p_shdr_ctx->input_param.tuning->tm_param.max_total_gain;
        tuningParam[4]       = m_p_shdr_ctx->input_param.tuning->tm_param.max_gain;
        tuningParam[5]       = m_p_shdr_ctx->input_param.tuning->tm_param.min_gain;
        tuningParam[6]       = m_p_shdr_ctx->input_param.tuning->fusion_param.ma_luma_th1_exp[0];
        tuningParam[7]       = m_p_shdr_ctx->input_param.tuning->fusion_param.ma_luma_th2_exp[0];

        LOG_INFO(CamxLogGroupChi, "ProcessRequest frame id:%d Params %f %f %f %f %f %f %f %f",
            m_p_shdr_ctx->input_param.stats_info->frame_id, tuningParam[0],
            tuningParam[1], tuningParam[2], tuningParam[3], tuningParam[4], tuningParam[5],
            tuningParam[6], tuningParam[7]);

        CHIMETADATAINFO        metadataInfo     = {0};
        const UINT32           tagSize          = 1;
        CHITAGDATA             tagData[tagSize] = { {0} };
        UINT32                 tagList[tagSize];
        metadataInfo.size                       = sizeof(CHIMETADATAINFO);
        metadataInfo.chiSession                 = m_hChiSession;
        metadataInfo.tagNum                     = tagSize;
        metadataInfo.pTagList                   = &tagList[0];
        metadataInfo.pTagData                   = &tagData[0];
        UINT32 index = 0;
        tagList[index]              = g_VendorTagList[4] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = m_p_shdr_ctx->input_param.stats_info->frame_id;
        tagData[index].pData        = tuningParam;
        tagData[index].dataSize     = g_VendorTagSectionYUVSHDR[4].numUnits;
        result = g_ChiNodeInterface.pSetMetadata(&metadataInfo);
    }

    INT ret_lib = m_shdr_lib_process(m_p_shdr_ctx->handle,
        &m_p_shdr_ctx->input_param,
        ShdrCompletionCallback,
        ImgAlgoShdrAECStatsReadyCallback,
        p_cback);
    if (CDKResultSuccess != ret_lib)
    {
        result = CDKResultEFailed;
    }
    pProcessRequestInfo->isDelayedRequestDone = TRUE;
    LOG_INFO(CamxLogGroupChi, "ProcessRequest frame id:%d Sync shdr_lib_process return =%d",
        m_p_shdr_ctx->input_param.stats_info->frame_id, ret_lib);

    if (CDKResultSuccess == ret_lib)
    {
        CHIMETADATAINFO        metadataInfo     = {0};
        const UINT32           tagSize          = 1;
        CHITAGDATA             tagData[tagSize] = { {0} };
        UINT32                 tagList[tagSize];
        metadataInfo.size                       = sizeof(CHIMETADATAINFO);
        metadataInfo.chiSession                 = m_hChiSession;
        metadataInfo.tagNum                     = tagSize;
        metadataInfo.pTagList                   = &tagList[0];
        metadataInfo.pTagData                   = &tagData[0];
        UINT32 index = 0;
        tagList[index]              = g_VendorTagList[3] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = m_p_shdr_ctx->input_param.stats_info->frame_id;
        tagData[index].pData        = &ret_lib;
        tagData[index].dataSize     = g_VendorTagSectionYUVSHDR[3].numUnits;
        result = g_ChiNodeInterface.pSetMetadata(&metadataInfo);
    }

    }

    VOID*         pGPUStatsData = NULL;
    pGPUStatsData               = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          (m_ShdrGpuVendorTagBase | UsecaseMetadataSectionMask),
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
    if (NULL != pGPUStatsData)
    {
        m_isGPUStatsEnable = *(reinterpret_cast<BOOL*>(pGPUStatsData));
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "GPU  Meta is NULL");
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::AllocIonBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID* ChiYUVSHDRNode::AllocIonBuffer(size_t buf_size, int* fd, INT32 *handle)
{
    CDK_UNUSED_PARAM(buf_size);
    CDK_UNUSED_PARAM(fd);

    struct ion_allocation_data  alloc;
    struct ion_fd_data          ion_info_fd;
    int                         rc;
    // native_handle_t *nh_test;
    if (m_IonFd <= 0) {
        m_IonFd = open("/dev/ion", O_RDONLY);
    }
    if (m_IonFd <= 0) {
        LOG_ERROR(CamxLogGroupChi, "Ion dev open failed %s\n", strerror(errno));
        return NULL;
    }
    memset(&alloc, 0, sizeof(alloc));

    alloc.len           = (size_t)(buf_size);
    alloc.len           = (alloc.len + 4095U) & (~4095U);
    alloc.align         = 4096;
    alloc.flags         = ION_FLAG_CACHED;
    alloc.heap_id_mask  = ION_HEAP(ION_SYSTEM_HEAP_ID);
    rc = ioctl(m_IonFd, ION_IOC_ALLOC, &alloc);
    if (rc < 0) {
        LOG_ERROR(CamxLogGroupChi, "ION allocation failed %s with rc = %d \n", strerror(errno), rc);
        return NULL;
    }
    memset(&ion_info_fd, 0, sizeof(ion_info_fd));
    ion_info_fd.handle = alloc.handle;
    *handle            = (INT32)alloc.handle;
    rc = ioctl(m_IonFd, ION_IOC_SHARE, &ion_info_fd);
    if (rc < 0) {
        LOG_ERROR(CamxLogGroupChi, "ION map failed %s\n", strerror(errno));
        return NULL;
    }
    *fd = ion_info_fd.fd;
    // req_meminfo->ion_handle = ion_info_fd.handle;
    // LOGD("%s ION FD %d len %d\n", __func__, ion_info_fd.fd, alloc.len);
    // req_meminfo->size = alloc.len;
    //nh_test = new private_handle_t (ion_info_fd.fd, alloc.len, 0, 0, 0, width, height);
    /* nh_test = native_handle_create(2, 4);
    nh_test->data[0] = ion_info_fd.fd;
    nh_test->data[1] = 0;
    nh_test->data[2] = 0;
    nh_test->data[3] = 0;
    nh_test->data[4] = alloc.len;
    nh_test->data[5] = 0; */
    unsigned char* vaddr = (unsigned char *)mmap(NULL,
        alloc.len,
        PROT_READ  | PROT_WRITE,
        MAP_SHARED,
        ion_info_fd.fd,
        0);
    if(MAP_FAILED == vaddr)
    {
        printf("mmap() failed\n");
        close(ion_info_fd.fd);
    }
    // return nh_test;
    return vaddr;
}


VOID ChiYUVSHDRNode::FullfillIdenticalMesh(float* mesh, float total_margin,
    unsigned int input_width, unsigned int input_height,
    unsigned int output_width, unsigned int output_height,
    unsigned int mesh_num_x, unsigned int mesh_num_y)
{
    CDK_UNUSED_PARAM(output_width);
    CDK_UNUSED_PARAM(output_height);

    unsigned int x, y;
    float rt_prime[9];
    float* ptr = mesh;

    rt_prime[0] = 1.0f / total_margin;
    rt_prime[1] = 0.0f;
    rt_prime[2] = (1.0f - 1.0f / total_margin) / 2.0f * (float)input_width;
    rt_prime[3] = 0.0f;
    rt_prime[4] = 1.0f / total_margin;
    rt_prime[5] = (1.0f - 1.0f / total_margin) / 2.0f * (float)input_height;
    rt_prime[6] = 0.0f;
    rt_prime[7] = 0.0f;
    rt_prime[8] = 1.0f;

    int i;

    float mired_input_width  = 1.0f / (float)input_width;
    float mired_input_height = 1.0f / (float)input_height;

    for (y = 0; y < mesh_num_y; y++)
    {
        for (x = 0; x < mesh_num_x; x++)
        {
            float x_f = (float)x*((float)input_width / (float)(mesh_num_x - 1));
            float y_f = (float)y*((float)input_height / (float)(mesh_num_y - 1));
            apply_rt(&x_f, &y_f, rt_prime);
            ptr[0] = x_f*mired_input_width;
            ptr[1] = y_f*mired_input_height;
            ptr += 2;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::GetVendorTagBases
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiYUVSHDRNode::GetVendorTagBases()
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
        // Get the "DE stats from defog" vendor tag base
        result = ChiNodeUtils::GetVendorTagBase("com.qti.node.defog",
                                                "DEStats",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_DEDataVendorTagBase = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get DE stats");
        }
    }

    if (CDKResultSuccess == result)
    {
        // Get the "DE stats from defog" vendor tag base
        result = ChiNodeUtils::GetVendorTagBase("org.quic.camera.defog",
                                                "enable",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_DefogEnableVendorTagBase = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get Defog Enable");
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
        // Get the "AEC Frame Control" vendor tag base
        result = ChiNodeUtils::GetVendorTagBase("com.qti.node.shdr",
                                                "SHDRBEStats",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_SHDR_beTag = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get SHDRBEStats");
        }
    }
    if (CDKResultSuccess == result)
    {
        // Get the "AWB Frame Control" vendor tag base
        result = ChiNodeUtils::GetVendorTagBase("com.qti.node.shdr",
                                                "SHDRBHISTStats",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_SHDR_histTag = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get SHDRBHISTStats");
        }
    }

    if (CDKResultSuccess == result)
    {
        // Get the "IHIST Parsed" vendor tag base
        result = ChiNodeUtils::GetVendorTagBase("com.qti.node.shdr",
                                                "SHDRBGBEconfigStats",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_SHDR_configTag = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get SHDRBGBEconfigStats");
        }
    }

    if (CDKResultSuccess == result)
    {
        // Get the "IHIST Parsed" vendor tag base
        result = ChiNodeUtils::GetVendorTagBase("org.codeaurora.qcamera3.sensor_meta_data",
                                                "current_mode",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_SHDR_sensorMode = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get SHDRBGBEconfigStats");
        }
    }

    if (CDKResultSuccess == result)
    {
        // Get the "GPU Stats" vendor tag base
        result = ChiNodeUtils::GetVendorTagBase("org.quic.camera.shdrinfo",
                                                "isGPUStats",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_ShdrGpuVendorTagBase = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get GPU Stats");
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::ImgAlgoShdrAECStatsReadyCallback
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiYUVSHDRNode::ImgAlgoShdrAECStatsReadyCallback(
    SHDR3STATSINFO  *stats,
    VOID * user_data)
{
    CDK_UNUSED_PARAM(user_data);
    CDKResult result = CDKResultSuccess;
    ParsedHDRBEStatsOutput      bestats;
    ParsedBHistStatsOutput      bhist;
    ISPAWBBGStatsConfig         bgbeconfig;
    ParsedBHistStatsOutput      nonuniformbhist;
//    ParsedIHistStatsOutput      ihist;

    memset(&bestats,         0x0, sizeof(ParsedHDRBEStatsOutput));
    memset(&bhist,           0x0, sizeof(ParsedBHistStatsOutput));
    memset(&bgbeconfig,      0x0, sizeof(ISPAWBBGStatsConfig));
    memset(&nonuniformbhist, 0x0, sizeof(ParsedBHistStatsOutput));
//    memset(&ihist,           0x0, sizeof(ParsedIHistStatsOutput));
    //TODO: Pack it in unified data structure

/*
    com.qti.node.shdr.SHDRBEStats                ParsedHDRBEStatsOutput      bestats      SetAlgoBayerGridValue
    com.qti.node.shdr.SHDRBHISTStats             ParsedBHistStatsOutput      bhist        SetAlgoBayerHistValue
    com.qti.node.shdr.SHDRBGBEconfigStats        ISPAWBBGStatsConfig         bgbeconfig
    com.qti.node.shdr3.SHDRHDRBHISTStats         ParsedHDRBHistStatsOutput   hdrbhist     SetAlgoBayerHDRBHistValue
    com.qti.node.shdr3.SHDRIHISTStats            ParsedIHistStatsOutput      ihist
*/

    SHDRCALLBACK* p_cback = (SHDRCALLBACK*)user_data;
    SHDRNODECONTEXT* p_shdr_ctx =  p_cback->p_shdr_ctx;
    INT* FrameIdAEC = reinterpret_cast<int*>(p_cback->user_data_frame);
    INT shdrSettled = 0;

    if (NULL == p_cback || NULL == stats)
    {
        LOG_ERROR(CamxLogGroupChi, "AECStatsReadyCallback p_cback or stats is NULL");
        result = CDKResultEFailed;
    }

    if(CDKResultSuccess == result)
    {
        ChiYUVSHDRNode* pNode = reinterpret_cast<ChiYUVSHDRNode*>(p_cback->user_data_Node);
        if (NULL == pNode)
        {
            LOG_ERROR(CamxLogGroupChi, "AECStatsReadyCallback pNode == NULL");
        }
        if (pNode->m_isGPUStatsEnable != TRUE)
        {
            // For HW stats need not process GPU Meta
            return;
        }
        else
        {
            SHDR3STATSINFO* stats_info = (SHDR3STATSINFO*)stats;
            LOG_ERROR(CamxLogGroupChi, "Check STATS :%p", stats_info);

            LOG_ERROR(CamxLogGroupChi, "Check STATS :%d and %u and %u",
                (INT)stats_info->frame_id, stats_info->num_bg_grid_w, stats_info->num_bg_grid_h);


            int numberOfBEStatsROIs   = stats_info->num_bg_grid_w * stats_info->num_bg_grid_h;
            FLOAT r_sum = 0.0f;

            for (int i = 0; i < numberOfBEStatsROIs ; i++)
            {
               HDRBEChannelData data;
               data.BCount  = data.GRCount = data.GBCount = data.RCount = 1;
               data.BSum    = stats_info->b_avg[i];
               data.RSum    = stats_info->r_avg[i];
               data.GBSum   = stats_info->g_avg[i];
               data.GRSum   = stats_info->g_avg[i];
               // data.GSum = stats_info->g_avg[i];
               bestats.SetChannelData(data, i, FALSE);
               r_sum        += stats_info->r_avg[i];
            }

            bestats.numROIs     = numberOfBEStatsROIs;
            bestats.flags.usesY = FALSE;
            bestats.flags.onlyY = FALSE;
            bestats.flags.hasSatInfo= FALSE;

            for (int i = 0; i < MAX_SHDR_BHIST_STATS_SIZE; i++)
            {
                bhist.BHistogramStats[i] = static_cast<UINT>(stats_info->bin[i]);
                //bhist.BHistogramStats[i]   = i;
            }

            bhist.numBins     = MAX_SHDR_BHIST_STATS_SIZE;
            bhist.channelType = ColorChannelY;

             shdrSettled = stats_info->sHDR_settled;

             LOG_VERBOSE(CamxLogGroupChi, "r_sum %f, numberOfBEStatsROIs is %d, num_grid_w %d"
                    "num_grid_h %d, numBins =%d, channelType=%d, satInfo:%d settled:%d and frame_num :%d",
                    r_sum, numberOfBEStatsROIs, stats_info->num_bg_grid_w, stats_info->num_bg_grid_h,
                    bhist.numBins, bhist.channelType, bestats.flags.hasSatInfo, shdrSettled, *FrameIdAEC);


            bgbeconfig.regionWidth  = static_cast<UINT32>(stats_info->num_bg_grid_w);
            bgbeconfig.regionHeight = static_cast<UINT32>(stats_info->num_bg_grid_h);

            BGBEConfig* pBGConfig = &bgbeconfig.AWBBGConfig;

            pBGConfig->horizontalNum = 16; // tocheck ?
            pBGConfig->verticalNum   = 16;
            pBGConfig->ROI.left      = 0;
            pBGConfig->ROI.top       = 0;
            pBGConfig->ROI.width     = 1920;
            pBGConfig->ROI.height    = 1080;


            for (int i = 0; i < MAX_SHDR_BHIST_STATS_NONUNIFROM_SIZE; i++)
            {
                nonuniformbhist.BHistogramStats[i] = static_cast<UINT>(stats_info->bin_vhist2[i]);
            }
            nonuniformbhist.numBins = MAX_SHDR_BHIST_STATS_NONUNIFROM_SIZE;
            nonuniformbhist.channelType = ColorChannelY;

           /* for (int i = 0; i < MAX_SHDR_IHIST_STATS_SIZE; i++)
            {
                ihist.imageHistogram.YCCHistogram[i] = static_cast<UINT>(stats_info->ihist[i]); // need to check with yihe
            }
            ihist.numBins =  MAX_SHDR_IHIST_STATS_SIZE;
            LOG_INFO(CamxLogGroupChi, "frame_num :%d IHIST stats are :%d and %d",*FrameIdAEC, ihist.imageHistogram.YCCHistogram[30],
                    ihist.imageHistogram.YCCHistogram[70]); */
        }

        INT* FrameIdinAECStatsReadyCallback = reinterpret_cast<INT*>(p_cback->user_data_frame);

        CHIMETADATAINFO        metadataInfo     = {0};
        const UINT32           yuvTagSize       = (sizeof(g_VendorTagSectionYUVSHDR) / sizeof(g_VendorTagSectionYUVSHDR[0]));
        // IHIST        : Filled after fusion from SHDRCompletionCallback
        // SHDRLibStatus: Filled Synchronoulsy, IPE submit require only if Lib generated success O/P
        const UINT32           tagSize          = NUMRawSHDRVendorTag + yuvTagSize -3;
        CHITAGDATA             tagData[tagSize] = { {0} };
        UINT32                 tagList[tagSize];
        metadataInfo.size                       = sizeof(CHIMETADATAINFO);
        metadataInfo.chiSession                 = pNode->m_hChiSession;
        metadataInfo.tagNum                     = tagSize;
        metadataInfo.pTagList                   = &tagList[0];
        metadataInfo.pTagData                   = &tagData[0];

        UINT32 index = 0;
        tagList[index]              = pNode->m_SHDR_beTag;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = *FrameIdinAECStatsReadyCallback;
        tagData[index].pData        = &bestats;
        tagData[index].dataSize     = sizeof(ParsedHDRBEStatsOutput);
        index++;
        tagList[index]              = pNode->m_SHDR_histTag;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = *FrameIdinAECStatsReadyCallback;
        tagData[index].pData        = &bhist;
        tagData[index].dataSize     = sizeof(ParsedBHistStatsOutput);
        index++;
        tagList[index]              = pNode->m_SHDR_configTag;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = *FrameIdinAECStatsReadyCallback;
        tagData[index].pData        = &bgbeconfig;
        tagData[index].dataSize     = sizeof(ISPAWBBGStatsConfig);
        index++;
        tagList[index]              = g_VendorTagList[0] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = *FrameIdinAECStatsReadyCallback;
        tagData[index].pData        = &nonuniformbhist;
        tagData[index].dataSize     = g_VendorTagSectionYUVSHDR[0].numUnits;
        index++;
        tagList[index]              = g_VendorTagList[2] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = *FrameIdinAECStatsReadyCallback;
        tagData[index].pData        = &shdrSettled;
        tagData[index].dataSize     = g_VendorTagSectionYUVSHDR[2].numUnits;
        /*index++;
        tagList[index]              = g_VendorTagList[1] + g_vendorTagBase;
        tagData[index].size         = sizeof(CHITAGDATA);
        tagData[index].requestId    = *FrameIdinAECStatsReadyCallback;
        tagData[index].pData        = &ihist;
        tagData[index].dataSize     = g_VendorTagSectionYUVSHDR[1].numUnits;*/


        result = g_ChiNodeInterface.pSetMetadata(&metadataInfo);
        BOOL dumpOverrideEnable = FALSE; //make it false
        if (dumpOverrideEnable)
        {
            pNode->DumpMetaAfterSetMeta(*FrameIdinAECStatsReadyCallback, metadataInfo);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::ChiYUVSHDRNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiYUVSHDRNode::ChiYUVSHDRNode()
    : m_hChiSession(NULL)
    , m_nodeId(0)
    , m_nodeCaps(0)
    , m_processedFrame(0)
{
    m_IonFd = -1;
}

VOID ChiYUVSHDRNode::DumpMetaAfterSetMeta(INT frameNum, CHIMETADATAINFO metadataInfo)
{
    VOID* pData = NULL;
    CDKResult result = CDKResultSuccess;
    CDK_UNUSED_PARAM(frameNum);
    CDK_UNUSED_PARAM(metadataInfo);


    // UINT32 tag  = g_VendorTagList[0] + g_vendorTagBase;
    UINT32 tag1  = m_SHDR_beTag;
    pData       = ChiNodeUtils::GetMetaData(frameNum, tag1, ChiMetadataDynamic,
                                            &g_ChiNodeInterface, metadataInfo.chiSession);
    if (NULL != pData)
    {
        ParsedHDRBEStatsOutput* pAECData = reinterpret_cast<ParsedHDRBEStatsOutput*>(pData);

        LOG_VERBOSE(CamxLogGroupChi, "BG Meta :%u and %d and %d and %d", pAECData->numROIs, pAECData->flags.usesY,
            pAECData->flags.onlyY, pAECData->flags.hasSatInfo);
    }

    UINT32 tag2  = m_SHDR_histTag;
    pData       = ChiNodeUtils::GetMetaData(frameNum, tag2, ChiMetadataDynamic,
                                            &g_ChiNodeInterface, metadataInfo.chiSession);
    if (NULL != pData)
    {
        ParsedBHistStatsOutput* pAECData1 = reinterpret_cast<ParsedBHistStatsOutput*>(pData);

        LOG_VERBOSE(CamxLogGroupChi, "Linear Bhist data :%u and %d and %u and %u and %u and %u",
            pAECData1->numBins, (INT)pAECData1->channelType, pAECData1->BHistogramStats[0],
            pAECData1->BHistogramStats[1], pAECData1->BHistogramStats[11], pAECData1->BHistogramStats[12]);
    }

        UINT32 tag3  = m_SHDR_configTag;
        pData       = ChiNodeUtils::GetMetaData(frameNum, tag3, ChiMetadataDynamic,
                                                &g_ChiNodeInterface, metadataInfo.chiSession);
        if (NULL != pData)
        {
            ISPAWBBGStatsConfig* pAWBData1 = reinterpret_cast<ISPAWBBGStatsConfig*>(pData);
            LOG_VERBOSE(CamxLogGroupChi, "pAWBData1 :regionWidth%u and regionHeight:%d",
                pAWBData1->regionWidth, pAWBData1->regionHeight);
        }


    if (CDKResultSuccess != result)
    {
        LOG_ERROR(CamxLogGroupChi, "pSetMetadata is NULL");
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::~ChiYUVSHDRNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiYUVSHDRNode::~ChiYUVSHDRNode()
{
    if (!m_p_shdr_ctx)
        return;
    if (TRUE == m_InterpolationResourceAllocated)
    {
        trigger_interpolation_release();
    }
    if (m_p_shdr_ctx->handle)
    {
        m_shdr_lib_deinit(m_p_shdr_ctx->handle);
        m_p_shdr_ctx->handle = NULL;
    }
    free (m_p_shdr_ctx->stats_info);
    delete m_p_shdr_ctx;
    ReleaseWarpMem();
    close(m_IonFd);
    m_IonFd = -1;
    m_p_shdr_ctx = NULL;

    m_hChiSession = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::FreeIonAlloc
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiYUVSHDRNode::FreeIonAlloc(size_t size, void* vaddr, INT handle)
{
    struct ion_handle_data handle_data;
    handle_data.handle = (ion_user_handle_t)handle;
    munmap(vaddr, size);
    ioctl(m_IonFd, ION_IOC_FREE, &handle_data);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::ReleaseWarpMem
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiYUVSHDRNode::ReleaseWarpMem()
{
    for (UINT req = 0; req < DefaultRequestQueueDepth; req++)
    {
        for (UINT i = 0; i < m_p_shdr_ctx->init_config.num_exposure; i++)
        {
            FreeIonAlloc(m_p_shdr_ctx->warp_mesh[req][i].mesh.buf_size,
                m_p_shdr_ctx->warp_mesh[req][i].mesh.buf,
                warp_buf_handle[req][i]);
                close(m_p_shdr_ctx->warp_mesh[req][i].mesh.fd);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiYUVSHDRNode::LoadLib
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiYUVSHDRNode::LoadLib()
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
                                                                                               "shdr3_lib_init"));

        m_shdr_lib_process = reinterpret_cast<SHDRLIBPROCESS>(ChiNodeUtils::LibGetAddr(m_hShdrLib,
                                                                                               "shdr3_lib_process"));

        m_shdr_lib_deinit = reinterpret_cast<SHDRLIBDEINIT>(ChiNodeUtils::LibGetAddr(m_hShdrLib,
                                                                                                 "shdr3_lib_deinit"));

        if ((NULL == m_shdr_lib_init)       ||
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

