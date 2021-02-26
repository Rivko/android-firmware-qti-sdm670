////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodedummyrtb.cpp
/// @brief Chi node for dummyrtb
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <inttypes.h>
#include <system/camera_metadata.h>

#include "camxchinodedummyrtb.h"

// NOWHINE FILE CP040: Keyword new not allowed. Use the CAMX_NEW/CAMX_DELETE functions insteads
// NOWHINE FILE NC008: Warning: - Var names should be lower camel case

#undef LOG_TAG
#define LOG_TAG "CHIDUMMYRTB"

#define PRINT_METADATA FALSE

ChiNodeInterface g_ChiNodeInterface;    ///< The instance save the CAMX Chi interface
UINT32           g_vendorTagBase = 0;   ///< Chi assigned runtime vendor tag base for the node

/// @todo (CAMX-1854) the major / minor version shall get from CHI
static const UINT32 ChiNodeMajorVersion = 0;    ///< The major version of CHI interface
static const UINT32 ChiNodeMinorVersion = 0;    ///< The minor version of CHI interface

static const UINT32 ChiNodeCapsDummyRTB       = 1;                    ///< Simply copy the image
static const UINT32 ChiNodeCapsHalfScale    = 1 << 1;               ///< 1/2 downsize
static const UINT32 ChiNodeCapsQuarterScale = 1 << 2;               ///< 1/4 downsize

static const CHAR   DummyRTBNodeSectionName[]         = "com.qti.node.dummyrtb";   ///< The section name for node

static const UINT32 DummyRTBNodeTagBase               = 0;                        ///< Tag base
static const UINT32 DummyRTBNodeTagSupportedFeature   = DummyRTBNodeTagBase + 0;    ///< Tag for supported features
static const UINT32 DummyRTBNodeTagCurrentMode        = DummyRTBNodeTagBase + 1;    ///< Tag to indicated current operation mode
static const UINT32 DummyRTBNodeTagProcessedFrameNum  = DummyRTBNodeTagBase + 2;    ///< Tag to show processed frame's count
static const UINT32 DummyRTBNodeTagFrameDimension     = DummyRTBNodeTagBase + 3;    ///< Tag to show current's frame dimension

///< Supported vendor tag list, it shall align with the definition in g_VendorTagSectionDummyRTB
static const UINT32 g_VendorTagList[] =
{
    DummyRTBNodeTagSupportedFeature,
    DummyRTBNodeTagCurrentMode,
    DummyRTBNodeTagProcessedFrameNum,
    DummyRTBNodeTagFrameDimension
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagSectionDummyRTB[] =
{
    { "SupportedFeature",       TYPE_INT32, 1 },
    { "CurrentMode",            TYPE_BYTE,  1 },
    { "ProcessedFrameNumber",   TYPE_INT64, 1 },
    { "FrameDimension",         TYPE_INT64, 2 },
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGSECTIONDATA g_VendorTagDummyRTBSection[] =
{
    {
        DummyRTBNodeSectionName,  0,
        sizeof(g_VendorTagSectionDummyRTB) / sizeof(g_VendorTagSectionDummyRTB[0]), g_VendorTagSectionDummyRTB,
        CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    }
};

///< This is an array of all vendor tag section data
static ChiVendorTagInfo g_VendorTagInfoDummyRTB[] =
{
    {
        &g_VendorTagDummyRTBSection[0],
        sizeof(g_VendorTagDummyRTBSection) / sizeof(g_VendorTagDummyRTBSection[0])
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummyRTBNodeGetCaps
///
/// @brief  Implementation of PFNNODEGETCAPS defined in chinode.h
///
/// @param  pCapsInfo   Pointer to a structure that defines the capabilities supported by the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyRTBNodeGetCaps(
    CHINODECAPSINFO* pCapsInfo)
{
    CDKResult result = CDKResultSuccess;

    if (pCapsInfo == NULL)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pCapsInfo is NULL");
        return result;
    }

    // dereference pCapsInfo only when it is not NULL
    if (CDKResultSuccess == result)
    {
        if (pCapsInfo->size >= sizeof(CHINODECAPSINFO))
        {
            pCapsInfo->nodeCapsMask = ChiNodeCapsDummyRTB | ChiNodeCapsHalfScale | ChiNodeCapsQuarterScale;
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
/// DummyRTBNodeQueryVendorTag
///
/// @brief  Implementation of PFNCHIQUERYVENDORTAG defined in chinode.h
///
/// @param  pQueryVendorTag Pointer to a structure that returns the exported vendor tag
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyRTBNodeQueryVendorTag(
    CHIQUERYVENDORTAG* pQueryVendorTag)
{
    CDKResult result = CDKResultSuccess;
    result = ChiNodeUtils::QueryVendorTag(pQueryVendorTag, g_VendorTagInfoDummyRTB);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummyRTBNodeCreate
///
/// @brief  Implementation of PFNNODECREATE defined in chinode.h
///
/// @param  pCreateInfo Pointer to a structure that defines create session information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyRTBNodeCreate(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult       result  = CDKResultSuccess;
    ChiDummyRTBNode*  pNode   = NULL;

    if (NULL == pCreateInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pTagTypeInfo is NULL");
        return result;
    }

    if (pCreateInfo->size < sizeof(CHINODECREATEINFO))
    {
        LOG_ERROR(CamxLogGroupChi, "CHINODECREATEINFO is smaller than expected");
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {
        pNode = new ChiDummyRTBNode;
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
/// DummyRTBNodeDestroy
///
/// @brief  Implementation of PFNNODEDESTROY defined in chinode.h
///
/// @param  pDestroyInfo    Pointer to a structure that defines the session destroy information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyRTBNodeDestroy(
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
            ChiDummyRTBNode* pNode = static_cast<ChiDummyRTBNode*>(pDestroyInfo->hNodeSession);
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
/// DummyRTBNodeQueryBufferInfo
///
/// @brief  Implementation of PFNNODEQUERYBUFFERINFO defined in chinode.h
///
/// @param  pQueryBufferInfo    Pointer to a structure to query the input buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyRTBNodeQueryBufferInfo(
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
            ChiDummyRTBNode* pNode = static_cast<ChiDummyRTBNode*>(pQueryBufferInfo->hNodeSession);
            result               = pNode->QueryBufferInfo(pQueryBufferInfo);
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
/// DummyRTBNodeSetBufferInfo
///
/// @brief  Implementation of PFNNODESETBUFFERINFO defined in chinode.h
///
/// @param  pSetBufferInfo  Pointer to a structure with information to set the output buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyRTBNodeSetBufferInfo(
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
            ChiDummyRTBNode* pNode = static_cast<ChiDummyRTBNode*>(pSetBufferInfo->hNodeSession);
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
/// DummyRTBNodeProcRequest
///
/// @brief  Implementation of PFNNODEPROCREQUEST defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummyRTBNodeProcRequest(
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
            ChiDummyRTBNode* pNode = static_cast<ChiDummyRTBNode*>(pProcessRequestInfo->hNodeSession);
            result               = pNode->ProcessRequest(pProcessRequestInfo);
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
/// DummyRTBNodeSetNodeInterface
///
/// @brief  Implementation of PFCHINODESETNODEINTERFACE defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID DummyRTBNodeSetNodeInterface(
    ChiNodeInterface* pNodeInterface)
{
    CDKResult result = CDKResultSuccess;
    result = ChiNodeUtils::SetNodeInterface(pNodeInterface, DummyRTBNodeSectionName,
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
            pNodeCallbacks->pGetCapabilities         = DummyRTBNodeGetCaps;
            pNodeCallbacks->pQueryVendorTag          = DummyRTBNodeQueryVendorTag;
            pNodeCallbacks->pCreate                  = DummyRTBNodeCreate;
            pNodeCallbacks->pDestroy                 = DummyRTBNodeDestroy;
            pNodeCallbacks->pQueryBufferInfo         = DummyRTBNodeQueryBufferInfo;
            pNodeCallbacks->pSetBufferInfo           = DummyRTBNodeSetBufferInfo;
            pNodeCallbacks->pProcessRequest          = DummyRTBNodeProcRequest;
            pNodeCallbacks->pChiNodeSetNodeInterface = DummyRTBNodeSetNodeInterface;
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
/// ChiDummyRTBNode::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummyRTBNode::Initialize(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult result = CDKResultSuccess;

    /// @todo (CAMX-1854) Check for Node Capabilities using NodeCapsMask
    m_hChiSession    = pCreateInfo->hChiSession;
    m_nodeId         = pCreateInfo->nodeId;
    m_nodeCaps       = pCreateInfo->nodeCaps.nodeCapsMask;
    m_processedFrame = 0;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyRTBNode::QueryBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummyRTBNode::QueryBufferInfo(
    CHINODEQUERYBUFFERINFO* pQueryBufferInfo)
{
    CDKResult result                     = CDKResultSuccess;

    // set the nodeCaps later
    switch (m_nodeCaps)
    {
        case ChiNodeCapsHalfScale:
        case ChiNodeCapsQuarterScale:
            result = CDKResultEUnsupported;
            break;
        case ChiNodeCapsDummyRTB:
        default:
            break;
    }

    if (CDKResultSuccess == result)
    {
        ChiNodeUtils::DefaultBufferNegotiation(pQueryBufferInfo);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyRTBNode::SetBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummyRTBNode::SetBufferInfo(
    CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo)
{
    m_format.width  = pSetBufferInfo->pFormat->width;
    m_format.height = pSetBufferInfo->pFormat->height;

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CheckDependency
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChiDummyRTBNode::CheckDependency(
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
/// ChiMemCpyNode::ProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummyRTBNode::ProcessRequest(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{
    BOOL statisfied = CheckDependency(pProcessRequestInfo);
    LOG_ERROR(CamxLogGroupChi, "input number:%dd",pProcessRequestInfo->inputNum);
    if (TRUE == statisfied)
    {

        CHIVENDORTAGBASEINFO vendorTagBase  = { 0 };
        vendorTagBase.size                  = sizeof(CHIVENDORTAGBASEINFO);
        vendorTagBase.pComponentName        = "com.qti.chi.multicamerainputmetadata";
        vendorTagBase.pTagName              = "InputMetadataBokeh";
        g_ChiNodeInterface.pGetVendorTagBase(&vendorTagBase);

        void* pData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
            vendorTagBase.vendorTagBase | InputMetadataSectionMask, ChiMetadataDynamic, &g_ChiNodeInterface, m_hChiSession);

        InputMetadataBokeh* pInputMetaOZ    = static_cast<InputMetadataBokeh*>(pData);

        for (UINT32 i = 0; i < pProcessRequestInfo->inputNum; i++)
        {
            // Cache invalidate on input buffers
            g_ChiNodeInterface.pCacheOps(pProcessRequestInfo->phInputBuffer[i], TRUE, FALSE);

            // This is memcpy dummy RTB node
            // Eventually, when real RTB implementation will be there then we do not need to memcpy
            // Copy the Master corresponding input to output
            if ( pInputMetaOZ->cameraMetadata[i].masterCameraRole == pInputMetaOZ->cameraMetadata[i].cameraRole)
            {
                CopyImage(pProcessRequestInfo->phOutputBuffer[0], pProcessRequestInfo->phInputBuffer[i]);

                // Cache clean on ouput buffer
                g_ChiNodeInterface.pCacheOps(pProcessRequestInfo->phOutputBuffer[0], FALSE, TRUE);

                break;
            }
        }
    }
    m_processedFrame++;
    UpdateMetaData(pProcessRequestInfo->frameNum);

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyRTBNode::ChiDummyRTBNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiDummyRTBNode::ChiDummyRTBNode()
    : m_hChiSession(NULL)
    , m_nodeId(0)
    , m_nodeCaps(0)
    , m_processedFrame(0)
{
    memset(&m_format, 0, sizeof(CHINODEIMAGEFORMAT));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyRTBNode::~ChiDummyRTBNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiDummyRTBNode::~ChiDummyRTBNode()
{
    m_hChiSession = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyRTBNode::CopyImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiDummyRTBNode::CopyImage(
    CHINODEBUFFERHANDLE hOutput,
    CHINODEBUFFERHANDLE hInput)
{
    UINT32 outPlaneStride  = hOutput->format.formatParams.yuvFormat[0].planeStride;
    UINT32 outHeight       = hOutput->format.formatParams.yuvFormat[0].height;
    UINT32 outSliceHeight  = hOutput->format.formatParams.yuvFormat[0].sliceHeight;
    UINT32 inPlaneStride   = hInput->format.formatParams.yuvFormat[0].planeStride;
    UINT32 inHeight        = hInput->format.formatParams.yuvFormat[0].height;
    UINT32 inSliceHeight   = hInput->format.formatParams.yuvFormat[0].sliceHeight;
    UINT32 copyPlaneStride = outPlaneStride > inPlaneStride ? inPlaneStride : outPlaneStride;
    UINT32 copyHeight      = outHeight > inHeight ? inHeight : outHeight;

    memcpy(&hOutput->format, &hInput->format, sizeof(CHIIMAGEFORMAT));
    memcpy(&hOutput->planeSize[0], &hInput->planeSize[0], sizeof(hInput->planeSize));
    memcpy(&hOutput->metadataSize[0], &hInput->metadataSize[0], sizeof(hInput->metadataSize));

    hOutput->imageCount     = hInput->imageCount;
    hOutput->numberOfPlanes = hInput->numberOfPlanes;

    for (UINT i = 0; i < hOutput->imageCount; i++)
    {
        if (((outPlaneStride != inPlaneStride) || (outSliceHeight != inSliceHeight)) &&
            ((hOutput->format.format == YUV420NV12 ) || (hOutput->format.format == YUV420NV21)))
        {
            hOutput->format.formatParams.yuvFormat[0].planeStride = outPlaneStride;
            hOutput->format.formatParams.yuvFormat[1].planeStride = outPlaneStride >> 1;
            hOutput->format.formatParams.yuvFormat[0].sliceHeight = outSliceHeight;
            hOutput->format.formatParams.yuvFormat[1].sliceHeight = outSliceHeight >> 1;
            hOutput->format.formatParams.yuvFormat[0].height      = outHeight;
            hOutput->format.formatParams.yuvFormat[1].height      = outHeight >> 1;

            for (UINT m = 0; m < copyHeight; m++)
            {
                memcpy((hOutput->pImageList[i].pAddr[0] + outPlaneStride * m),
                       (hInput->pImageList[i].pAddr[0] + inPlaneStride * m),
                       copyPlaneStride);
            }

            for (UINT m = 0; m < copyHeight >> 1; m++)
            {
                memcpy((hOutput->pImageList[i].pAddr[1] + outPlaneStride * m),
                       (hInput->pImageList[i].pAddr[1] + inPlaneStride * m),
                       copyPlaneStride);
            }
        }
        else
        {
            for (UINT j = 0; j < hOutput->numberOfPlanes; j++)
            {
                memcpy(hOutput->pImageList[i].pAddr[j], hInput->pImageList[i].pAddr[j], hInput->planeSize[j]);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummyRTBNode::UpdateMetaData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiDummyRTBNode::UpdateMetaData(
    UINT64 requestId)
{
    CHIMETADATAINFO        metadataInfo = {0};
    // This is an example for the RTB node to publish the result metadata.
    const UINT32           tagSize      = sizeof(g_VendorTagSectionDummyRTB) / sizeof(g_VendorTagSectionDummyRTB[0]) + 1;
    CHITAGDATA             tagData[tagSize];
    UINT32                 tagList[tagSize];

    metadataInfo.size       = sizeof(CHIMETADATAINFO);
    metadataInfo.chiSession = m_hChiSession;
    metadataInfo.tagNum     = tagSize;
    metadataInfo.pTagList   = &tagList[0];
    metadataInfo.pTagData   = &tagData[0];

    UINT32 index = 0;

    UINT32  supportedFeature    = ChiNodeCapsDummyRTB;
    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &supportedFeature;
    tagData[index].dataSize     = g_VendorTagSectionDummyRTB[index].numUnits;
    index++;

    UINT32  currentMode         = m_nodeCaps;
    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &currentMode;
    tagData[index].dataSize     = g_VendorTagSectionDummyRTB[index].numUnits;
    index++;

    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &m_processedFrame;
    tagData[index].dataSize     = g_VendorTagSectionDummyRTB[index].numUnits;
    index++;

    UINT32  dimension[2];
    dimension[0]                = m_format.width;
    dimension[1]                = m_format.height;
    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &dimension[0];
    tagData[index].dataSize     = g_VendorTagSectionDummyRTB[index].numUnits;

#if PRINT_METADATA
    g_ChiNodeInterface.pSetMetadata(&metadataInfo);

    CHIVENDORTAGBASEINFO vendorTagBase  = {0};
    vendorTagBase.size                  = sizeof(CHIVENDORTAGBASEINFO);
    vendorTagBase.pComponentName        = DummyRTBNodeSectionName;
    vendorTagBase.pTagName              = g_VendorTagSectionDummyRTB[index].pVendorTagName;
    g_ChiNodeInterface.pGetVendorTagBase(&vendorTagBase);
    LOG_VERBOSE(CamxLogGroupChi, "Vendor Tags value shall be same %x %x",
            g_VendorTagList[index] + g_vendorTagBase, vendorTagBase.vendorTagBase);

    // get the updated metadata from CamX and print them out
    VOID* pData = NULL;
    pData = ChiNodeUtils::GetMetaData(requestId, DummyRTBNodeTagSupportedFeature + g_vendorTagBase, ChiMetadataDynamic,
        &g_ChiNodeInterface, m_hChiSession);
    if (NULL != pData)
    {
        LOG_VERBOSE(CamxLogGroupChi, "supported feature %d supported feature %d",
            *static_cast<UINT32*>(pData), supportedFeature);
    }

    pData = ChiNodeUtils::GetMetaData(requestId, DummyRTBNodeTagCurrentMode + g_vendorTagBase, ChiMetadataDynamic,
        &g_ChiNodeInterface, m_hChiSession);
    if (NULL != pData)
    {
        LOG_VERBOSE(CamxLogGroupChi, "current mode %d current mode %d",
            *static_cast<UINT32*>(pData), currentMode);
    }

    pData = ChiNodeUtils::GetMetaData(requestId, DummyRTBNodeTagProcessedFrameNum + g_vendorTagBase, ChiMetadataDynamic,
        &g_ChiNodeInterface, m_hChiSession);
    if (NULL != pData)
    {
        LOG_VERBOSE(CamxLogGroupChi, "processed frameNum %" PRIu64 " processed frame %" PRIu64,
            *static_cast<UINT64*>(pData), m_processedFrame);
    }

    pData = ChiNodeUtils::GetMetaData(requestId, DummyRTBNodeTagFrameDimension + g_vendorTagBase, ChiMetadataDynamic,
        &g_ChiNodeInterface, m_hChiSession);
    if (NULL != pData)
    {
        UINT32*  local_dimension = (UINT32 *)pData;
        LOG_VERBOSE(CamxLogGroupChi, "frame dimension %d %d frame dimenstion %d %d",
            local_dimension[0], local_dimension[1], dimension[0], dimension[1]);
    }

    // sample of getting Android metadata from CamX
    pData = ChiNodeUtils::GetMetaData(requestId, ANDROID_SENSOR_TIMESTAMP, ChiMetadataDynamic,
        &g_ChiNodeInterface, m_hChiSession);
    if (NULL != pData)
    {
        INT64 timestamp = *static_cast<INT64 *>(pData);
        LOG_VERBOSE(CamxLogGroupChi, "timestamp for current frame %" PRIi64, timestamp);
    }

    // sample of getting static Android metadata from CamX
    pData = ChiNodeUtils::GetMetaData(requestId, ANDROID_CONTROL_AE_AVAILABLE_ANTIBANDING_MODES, ChiMetadataStatic,
        &g_ChiNodeInterface, m_hChiSession);
    if (NULL != pData)
    {
        UINT8*  aeModes = (UINT8 *)pData;
        LOG_VERBOSE(CamxLogGroupChi, "ae banding modes %d %d %d %d", aeModes[0], aeModes[1], aeModes[2], aeModes[3]);
    }
#endif
}

// CAMX_NAMESPACE_END
