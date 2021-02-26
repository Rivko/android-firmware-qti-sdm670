////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodedummysat.cpp
/// @brief Chi node for dummysat
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <inttypes.h>
#include <system/camera_metadata.h>

#include "camxchinodedummysat.h"
#include "chiifedefs.h"

// NOWHINE FILE CP040: Keyword new not allowed. Use the CAMX_NEW/CAMX_DELETE functions insteads
// NOWHINE FILE NC008: Warning: - Var names should be lower camel case

#undef LOG_TAG
#define LOG_TAG "CHIDUMMYSAT"

#define PRINT_METADATA FALSE

ChiNodeInterface g_ChiNodeInterface;    ///< The instance save the CAMX Chi interface
UINT32           g_vendorTagBase = 0;   ///< Chi assigned runtime vendor tag base for the node

/// @todo (CAMX-1854) the major / minor version shall get from CHI
static const UINT32 ChiNodeMajorVersion = 0;    ///< The major version of CHI interface
static const UINT32 ChiNodeMinorVersion = 0;    ///< The minor version of CHI interface

static const UINT32 ChiNodeCapsDummySAT       = 1;                    ///< Simply copy the image
static const UINT32 ChiNodeCapsHalfScale    = 1 << 1;               ///< 1/2 downsize
static const UINT32 ChiNodeCapsQuarterScale = 1 << 2;               ///< 1/4 downsize

static const CHAR   DummySATNodeSectionName[]         = "com.qti.node.dummysat";   ///< The section name for node

static const UINT32 DummySATNodeTagBase               = 0;                        ///< Tag base
static const UINT32 DummySATNodeTagSupportedFeature   = DummySATNodeTagBase + 0;    ///< Tag for supported features
static const UINT32 DummySATNodeTagCurrentMode        = DummySATNodeTagBase + 1;    ///< Tag to indicated current operation mode
static const UINT32 DummySATNodeTagProcessedFrameNum  = DummySATNodeTagBase + 2;    ///< Tag to show processed frame's count
static const UINT32 DummySATNodeTagFrameDimension     = DummySATNodeTagBase + 3;    ///< Tag to show current's frame dimension

///< Supported vendor tag list, it shall align with the definition in g_VendorTagSectionDummySAT
static const UINT32 g_VendorTagList[] =
{
    DummySATNodeTagSupportedFeature,
    DummySATNodeTagCurrentMode,
    DummySATNodeTagProcessedFrameNum,
    DummySATNodeTagFrameDimension
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagSectionDummySAT[] =
{
    { "SupportedFeature",       TYPE_INT32, 1 },
    { "CurrentMode",            TYPE_BYTE,  1 },
    { "ProcessedFrameNumber",   TYPE_INT64, 1 },
    { "FrameDimension",         TYPE_INT64, 2 },
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGSECTIONDATA g_VendorTagDummySATSection[] =
{
    {
        DummySATNodeSectionName,  0,
        sizeof(g_VendorTagSectionDummySAT) / sizeof(g_VendorTagSectionDummySAT[0]), g_VendorTagSectionDummySAT,
        CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    }
};

///< This is an array of all vendor tag section data
static ChiVendorTagInfo g_VendorTagInfoDummySAT[] =
{
    {
        &g_VendorTagDummySATSection[0],
        sizeof(g_VendorTagDummySATSection) / sizeof(g_VendorTagDummySATSection[0])
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummySATNodeGetCaps
///
/// @brief  Implementation of PFNNODEGETCAPS defined in chinode.h
///
/// @param  pCapsInfo   Pointer to a structure that defines the capabilities supported by the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummySATNodeGetCaps(
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
            pCapsInfo->nodeCapsMask = ChiNodeCapsDummySAT | ChiNodeCapsHalfScale | ChiNodeCapsQuarterScale;
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
/// DummySATNodeQueryVendorTag
///
/// @brief  Implementation of PFNCHIQUERYVENDORTAG defined in chinode.h
///
/// @param  pQueryVendorTag Pointer to a structure that returns the exported vendor tag
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummySATNodeQueryVendorTag(
    CHIQUERYVENDORTAG* pQueryVendorTag)
{
    CDKResult result = CDKResultSuccess;
    result = ChiNodeUtils::QueryVendorTag(pQueryVendorTag, g_VendorTagInfoDummySAT);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DummySATNodeCreate
///
/// @brief  Implementation of PFNNODECREATE defined in chinode.h
///
/// @param  pCreateInfo Pointer to a structure that defines create session information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummySATNodeCreate(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult       result  = CDKResultSuccess;
    ChiDummySATNode*  pNode   = NULL;

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
        pNode = new ChiDummySATNode;
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
/// DummySATNodeDestroy
///
/// @brief  Implementation of PFNNODEDESTROY defined in chinode.h
///
/// @param  pDestroyInfo    Pointer to a structure that defines the session destroy information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummySATNodeDestroy(
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
            ChiDummySATNode* pNode = static_cast<ChiDummySATNode*>(pDestroyInfo->hNodeSession);
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
/// DummySATNodeQueryBufferInfo
///
/// @brief  Implementation of PFNNODEQUERYBUFFERINFO defined in chinode.h
///
/// @param  pQueryBufferInfo    Pointer to a structure to query the input buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummySATNodeQueryBufferInfo(
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
            ChiDummySATNode* pNode = static_cast<ChiDummySATNode*>(pQueryBufferInfo->hNodeSession);
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
/// DummySATNodeSetBufferInfo
///
/// @brief  Implementation of PFNNODESETBUFFERINFO defined in chinode.h
///
/// @param  pSetBufferInfo  Pointer to a structure with information to set the output buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummySATNodeSetBufferInfo(
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
            ChiDummySATNode* pNode = static_cast<ChiDummySATNode*>(pSetBufferInfo->hNodeSession);
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
/// DummySATNodeProcRequest
///
/// @brief  Implementation of PFNNODEPROCREQUEST defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DummySATNodeProcRequest(
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
            ChiDummySATNode* pNode = static_cast<ChiDummySATNode*>(pProcessRequestInfo->hNodeSession);
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
/// DummySATNodeSetNodeInterface
///
/// @brief  Implementation of PFCHINODESETNODEINTERFACE defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID DummySATNodeSetNodeInterface(
    ChiNodeInterface* pNodeInterface)
{
    CDKResult result = CDKResultSuccess;
    result = ChiNodeUtils::SetNodeInterface(pNodeInterface, DummySATNodeSectionName,
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
            pNodeCallbacks->pGetCapabilities         = DummySATNodeGetCaps;
            pNodeCallbacks->pQueryVendorTag          = DummySATNodeQueryVendorTag;
            pNodeCallbacks->pCreate                  = DummySATNodeCreate;
            pNodeCallbacks->pDestroy                 = DummySATNodeDestroy;
            pNodeCallbacks->pQueryBufferInfo         = DummySATNodeQueryBufferInfo;
            pNodeCallbacks->pSetBufferInfo           = DummySATNodeSetBufferInfo;
            pNodeCallbacks->pProcessRequest          = DummySATNodeProcRequest;
            pNodeCallbacks->pChiNodeSetNodeInterface = DummySATNodeSetNodeInterface;
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
/// ChiDummySATNode::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummySATNode::Initialize(
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
/// ChiDummySATNode::QueryBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummySATNode::QueryBufferInfo(
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
        case ChiNodeCapsDummySAT:
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
/// ChiDummySATNode::SetBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummySATNode::SetBufferInfo(
    CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo)
{
    m_format.width  = pSetBufferInfo->pFormat->width;
    m_format.height = pSetBufferInfo->pFormat->height;

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummySATNode::ProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDummySATNode::ProcessRequest(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{
    m_resultDataOZ.outputShiftPreview.horizonalShift  = 0;
    m_resultDataOZ.outputShiftPreview.verticalShift   = 0;
    m_resultDataOZ.outputShiftSnapshot.horizonalShift = 0;
    m_resultDataOZ.outputShiftSnapshot.verticalShift  = 0;
    m_resultDataOZ.refResForOutputShift.width  = 1920;
    m_resultDataOZ.refResForOutputShift.height = 1080;
    m_resultDataOZ.outputCrop.top              = 0;
    m_resultDataOZ.outputCrop.left             = 0;
    m_resultDataOZ.outputCrop.width            = 1920;
    m_resultDataOZ.outputCrop.height           = 1080;
    m_resultDataOZ.lowPowerMode[0].cameraRole  = CameraRoleTypeWide;
    m_resultDataOZ.lowPowerMode[0].isEnabled   = FALSE;
    m_resultDataOZ.lowPowerMode[1].cameraRole  = CameraRoleTypeTele;
    m_resultDataOZ.lowPowerMode[1].isEnabled   = FALSE;

    CHIVENDORTAGBASEINFO vendorTagBase = { 0 };
    vendorTagBase.size = sizeof(CHIVENDORTAGBASEINFO);
    vendorTagBase.pComponentName = "com.qti.chi.multicamerainputmetadata";
    vendorTagBase.pTagName = "InputMetadataOpticalZoom";
    g_ChiNodeInterface.pGetVendorTagBase(&vendorTagBase);

    void* pData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
        vendorTagBase.vendorTagBase | InputMetadataSectionMask, ChiMetadataDynamic, &g_ChiNodeInterface, m_hChiSession);

    InputMetadataOpticalZoom* pInputMetaOZ = static_cast<InputMetadataOpticalZoom*>(pData);

    // Cache invalidate on input buffers
    for (UINT32 i = 0; i < pProcessRequestInfo->inputNum; i++)
    {
        g_ChiNodeInterface.pCacheOps(pProcessRequestInfo->phInputBuffer[i], TRUE, FALSE);
    }

    if ((NULL != pInputMetaOZ) && (MAX_LINKED_SESSIONS > pInputMetaOZ->numInputs))
    {
        // Single Zone - Only one input is present
        if (CameraRoleTypeWide == pInputMetaOZ->cameraMetadata[0].cameraRole)
        {
            LOG_ERROR(CamxLogGroupChi, "Dummy SAT node LPM - Only one stream, Wide camera selected");
            m_resultDataOZ.masterCamera = CameraRoleTypeWide;
            m_resultDataOZ.recommendedMasterCamera = CameraRoleTypeWide;
        }
        else if (CameraRoleTypeTele == pInputMetaOZ->cameraMetadata[1].cameraRole)
        {
            LOG_ERROR(CamxLogGroupChi, "Dummy SAT node LPM - Only one stream, Tele camera selected");
            m_resultDataOZ.masterCamera = CameraRoleTypeTele;
            m_resultDataOZ.recommendedMasterCamera = CameraRoleTypeTele;
        }
        CopyImage(pProcessRequestInfo->phOutputBuffer[0], pProcessRequestInfo->phInputBuffer[0]);
    }
    else
    {
        // Dual Zone - Both the inputs are present
        if ((NULL != pInputMetaOZ))
        {

            // Copy the input from master
            if (CameraRoleTypeWide == pInputMetaOZ->cameraMetadata[0].masterCameraRole)
            {
                LOG_ERROR(CamxLogGroupChi, "Dummy SAT node - Wide camera selected");
                CopyImage(pProcessRequestInfo->phOutputBuffer[0], pProcessRequestInfo->phInputBuffer[0]);
            }
            else
            {
                LOG_ERROR(CamxLogGroupChi, "Dummy SAT node - Tele camera selected");
                CopyImage(pProcessRequestInfo->phOutputBuffer[0], pProcessRequestInfo->phInputBuffer[1]);
            }

            // Set the master camera depending on crop region
            if((pInputMetaOZ->cameraMetadata[0].userCropRegion.left > 800) ||
                (pInputMetaOZ->cameraMetadata[0].userCropRegion.top > 800))
            {
                m_resultDataOZ.masterCamera            = CameraRoleTypeTele;
                m_resultDataOZ.recommendedMasterCamera = CameraRoleTypeTele;
            }
            else
            {
                m_resultDataOZ.masterCamera            = CameraRoleTypeWide;
                m_resultDataOZ.recommendedMasterCamera = CameraRoleTypeWide;
            }
        }
    }

    // Cache clean on ouput buffer
    for (UINT32 i = 0; i < pProcessRequestInfo->outputNum; i++)
    {
        g_ChiNodeInterface.pCacheOps(pProcessRequestInfo->phOutputBuffer[i], FALSE, TRUE);
    }

    LOG_ERROR(CamxLogGroupChi, "Dummy SAT node current master %d recommended master %d",
        m_resultDataOZ.masterCamera, m_resultDataOZ.recommendedMasterCamera);

    m_processedFrame++;
    UpdateMetaData(pProcessRequestInfo->frameNum);

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummySATNode::ChiDummySATNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiDummySATNode::ChiDummySATNode()
    : m_hChiSession(NULL)
    , m_nodeId(0)
    , m_nodeCaps(0)
    , m_processedFrame(0)
{
    memset(&m_format, 0, sizeof(CHINODEIMAGEFORMAT));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummySATNode::~ChiDummySATNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiDummySATNode::~ChiDummySATNode()
{
    m_hChiSession = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDummySATNode::CopyImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiDummySATNode::CopyImage(
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
            ((YUV420NV12 == hOutput->format.format) || (YUV420NV21 == hOutput->format.format) ||
            (RawPlain16 == hOutput->format.format)))
        {
            hOutput->format.formatParams.yuvFormat[0].planeStride = outPlaneStride;
            hOutput->format.formatParams.yuvFormat[0].sliceHeight = outSliceHeight;
            hOutput->format.formatParams.yuvFormat[0].height      = outHeight;

            for (UINT m = 0; m < copyHeight; m++)
            {
                memcpy((hOutput->pImageList[i].pAddr[0] + outPlaneStride * m),
                       (hInput->pImageList[i].pAddr[0] + inPlaneStride * m),
                       copyPlaneStride);
            }

            if (1 < hOutput->numberOfPlanes)
            {
                hOutput->format.formatParams.yuvFormat[1].planeStride = outPlaneStride >> 1;
                hOutput->format.formatParams.yuvFormat[1].sliceHeight = outSliceHeight >> 1;
                hOutput->format.formatParams.yuvFormat[1].height      = outHeight >> 1;

                for (UINT m = 0; m < copyHeight >> 1; m++)
                {
                    memcpy((hOutput->pImageList[i].pAddr[1] + outPlaneStride * m),
                           (hInput->pImageList[i].pAddr[1] + inPlaneStride * m),
                           copyPlaneStride);
                }
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
/// ChiDummySATNode::UpdateMetaData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiDummySATNode::UpdateMetaData(
    UINT64 requestId)
{
    CHIMETADATAINFO        metadataInfo = {0};
    // This is an example for the SAT node to publish the result metadata.
    const UINT32           tagSize      = sizeof(g_VendorTagSectionDummySAT) / sizeof(g_VendorTagSectionDummySAT[0]) + 2;
    CHITAGDATA             tagData[tagSize];
    UINT32                 tagList[tagSize];

    metadataInfo.size       = sizeof(CHIMETADATAINFO);
    metadataInfo.chiSession = m_hChiSession;
    metadataInfo.tagNum     = tagSize;
    metadataInfo.pTagList   = &tagList[0];
    metadataInfo.pTagData   = &tagData[0];

    UINT32 index = 0;

    UINT32  supportedFeature    = ChiNodeCapsDummySAT;
    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &supportedFeature;
    tagData[index].dataSize     = g_VendorTagSectionDummySAT[index].numUnits;
    index++;

    UINT32  currentMode         = m_nodeCaps;
    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &currentMode;
    tagData[index].dataSize     = g_VendorTagSectionDummySAT[index].numUnits;
    index++;

    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &m_processedFrame;
    tagData[index].dataSize     = g_VendorTagSectionDummySAT[index].numUnits;
    index++;

    UINT32  dimension[2];
    dimension[0]                = m_format.width;
    dimension[1]                = m_format.height;
    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &dimension[0];
    tagData[index].dataSize     = g_VendorTagSectionDummySAT[index].numUnits;

    // This is an example for the SAT node to publish the result metadata.
    CHIVENDORTAGBASEINFO vendorTagBase = { 0 };
    vendorTagBase.size           = sizeof(CHIVENDORTAGBASEINFO);
    vendorTagBase.pComponentName = "com.qti.chi.multicameraoutputmetadata";
    vendorTagBase.pTagName       = "OutputMetadataOpticalZoom";
    g_ChiNodeInterface.pGetVendorTagBase(&vendorTagBase);

    if (0 != vendorTagBase.vendorTagBase)
    {
        index++;
        tagList[index] = vendorTagBase.vendorTagBase;
        tagData[index].size = sizeof(CHITAGDATA);
        tagData[index].requestId = requestId;
        tagData[index].pData = &m_resultDataOZ;
        tagData[index].dataSize = sizeof(OutputMetadataOpticalZoom);
    }

    // This is an example for the SAT node to residual crop info
    VOID*        pData           = NULL;
    CHIRECT      residualCrop    = { 0, 0, 1000, 1000 };

    vendorTagBase                = { 0 };
    vendorTagBase.size           = sizeof(CHIVENDORTAGBASEINFO);
    vendorTagBase.pComponentName = "com.qti.cropregions";
    vendorTagBase.pTagName       = "ChiNodeResidualCrop";
    g_ChiNodeInterface.pGetVendorTagBase(&vendorTagBase);

    CHIVENDORTAGBASEINFO ifeCropTagBase = { 0 };
    ifeCropTagBase.size                 = sizeof(CHIVENDORTAGBASEINFO);
    ifeCropTagBase.pComponentName       = "org.quic.camera.ifecropinfo";
    ifeCropTagBase.pTagName             = "ResidualCrop";
    g_ChiNodeInterface.pGetVendorTagBase(&ifeCropTagBase);

    if (0 != ifeCropTagBase.vendorTagBase)
    {
        pData = ChiNodeUtils::GetMetaData(requestId, ifeCropTagBase.vendorTagBase,
            ChiMetadataDynamic, &g_ChiNodeInterface, m_hChiSession);

            if (NULL == pData)
            {
                pData = ChiNodeUtils::GetMetaData(requestId, ifeCropTagBase.vendorTagBase | InputMetadataSectionMask,
                    ChiMetadataDynamic, &g_ChiNodeInterface, m_hChiSession);
            }
    }

    if ((NULL != pData) && (0 != vendorTagBase.vendorTagBase))
    {
        IFECropInfo* ifeResidualCrop = static_cast<IFECropInfo *>(pData);

        residualCrop.left   = ifeResidualCrop->fullPath.left;
        residualCrop.top    = ifeResidualCrop->fullPath.top;
        residualCrop.width  = ifeResidualCrop->fullPath.width;
        residualCrop.height = ifeResidualCrop->fullPath.height;
    }

    if (0 != vendorTagBase.vendorTagBase)
    {
        index++;
        tagList[index] = vendorTagBase.vendorTagBase;
        tagData[index].size = sizeof(CHITAGDATA);
        tagData[index].requestId = requestId;
        tagData[index].pData = &residualCrop;
        tagData[index].dataSize = sizeof(CHIRECT);
    }

    g_ChiNodeInterface.pSetMetadata(&metadataInfo);

#if PRINT_METADATA
    CHIVENDORTAGBASEINFO vendorTagBase  = {0};
    vendorTagBase.size                  = sizeof(CHIVENDORTAGBASEINFO);
    vendorTagBase.pComponentName        = DummySATNodeSectionName;
    vendorTagBase.pTagName              = g_VendorTagSectionDummySAT[index].pVendorTagName;
    g_ChiNodeInterface.pGetVendorTagBase(&vendorTagBase);
    LOG_VERBOSE(CamxLogGroupChi, "Vendor Tags value shall be same %x %x",
            g_VendorTagList[index] + g_vendorTagBase, vendorTagBase.vendorTagBase);

    // get the updated metadata from CamX and print them out
    VOID* pData = NULL;
    pData = ChiNodeUtils::GetMetaData(requestId, DummySATNodeTagSupportedFeature + g_vendorTagBase, ChiMetadataDynamic,
        &g_ChiNodeInterface, m_hChiSession);
    if (NULL != pData)
    {
        LOG_VERBOSE(CamxLogGroupChi, "supported feature %d supported feature %d",
            *static_cast<UINT32*>(pData), supportedFeature);
    }

    pData = ChiNodeUtils::GetMetaData(requestId, DummySATNodeTagCurrentMode + g_vendorTagBase, ChiMetadataDynamic,
        &g_ChiNodeInterface, m_hChiSession);
    if (NULL != pData)
    {
        LOG_VERBOSE(CamxLogGroupChi, "current mode %d current mode %d",
            *static_cast<UINT32*>(pData), currentMode);
    }

    pData = ChiNodeUtils::GetMetaData(requestId, DummySATNodeTagProcessedFrameNum + g_vendorTagBase, ChiMetadataDynamic,
        &g_ChiNodeInterface, m_hChiSession);
    if (NULL != pData)
    {
        LOG_VERBOSE(CamxLogGroupChi, "processed frameNum %" PRIu64 " processed frame %" PRIu64,
            *static_cast<UINT64*>(pData), m_processedFrame);
    }

    pData = ChiNodeUtils::GetMetaData(requestId, DummySATNodeTagFrameDimension + g_vendorTagBase, ChiMetadataDynamic,
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
