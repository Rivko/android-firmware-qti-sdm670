////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodesplit.cpp
/// @brief Chi node for split
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <inttypes.h>
#include <system/camera_metadata.h>

#include "camxchinodesplit.h"

// NOWHINE FILE CP040: Keyword new not allowed. Use the CAMX_NEW/CAMX_DELETE functions insteads
// NOWHINE FILE NC008: Warning: - Var names should be lower camel case

#undef LOG_TAG
#define LOG_TAG "CHISPLIT"

#define PRINT_METADATA TRUE

ChiNodeInterface g_ChiNodeInterface;    ///< The instance save the CAMX Chi interface
UINT32           g_vendorTagBase = 0;   ///< Chi assigned runtime vendor tag base for the node

/// @todo (CAMX-1854) the major / minor version shall get from CHI
static const UINT32 ChiNodeMajorVersion = 0;    ///< The major version of CHI interface
static const UINT32 ChiNodeMinorVersion = 0;    ///< The minor version of CHI interface

static const CHAR   SplitNodeSectionName[]         = "com.qti.node.split";      ///< The section name for node

static const UINT32 SplitNodeTagBase               = 0;                        ///< Tag base
static const UINT32 SplitNodeTagProcessedFrameNum  = SplitNodeTagBase + 0;     ///< Tag for supported features

///< Supported vendor tag list, it shall align with the definition in g_VendorTagSectionSplit
static const UINT32 g_VendorTagList[] =
{
    SplitNodeTagProcessedFrameNum
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGDATA g_VendorTagSectionSplit[] =
{
    { "ProcessedFrameNumber",   TYPE_INT64, 1 },
};

///< This is an array of all vendor tag section data
static CHIVENDORTAGSECTIONDATA g_VendorTagSplitSection[] =
{
    {
        SplitNodeSectionName,  0,
        sizeof(g_VendorTagSectionSplit) / sizeof(g_VendorTagSectionSplit[0]), g_VendorTagSectionSplit,
        CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    }
};

///< This is an array of all vendor tag section data
static ChiVendorTagInfo g_VendorTagInfoSplit[] =
{
    {
        &g_VendorTagSplitSection[0],
        sizeof(g_VendorTagSplitSection) / sizeof(g_VendorTagSplitSection[0])
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SplitNodeGetCaps
///
/// @brief  Implementation of PFNNODEGETCAPS defined in chinode.h
///
/// @param  pCapsInfo   Pointer to a structure that defines the capabilities supported by the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult SplitNodeGetCaps(
    CHINODECAPSINFO* pCapsInfo)
{
    CDKResult result = CDKResultSuccess;

    if (pCapsInfo == NULL)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pCapsInfo is NULL");
    }

    if (CDKResultSuccess == result)
    {
        if (pCapsInfo->size >= sizeof(CHINODECAPSINFO))
        {
            /// @todo Chi node can split raw buffer into 2 or 3 frames
            /// Depending Sensor SHDR mode(2 exposure or 3 exposure frame)
            /// Hard Coded for 3 frame split
            if (FALSE)
            {
                pCapsInfo->nodeCapsMask |= ChiNodeCapsTwoFrameSplit;
            }
            else
            {
                pCapsInfo->nodeCapsMask |= ChiNodeCapsThreeFrameSplit;
            }
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
/// SplitNodeQueryVendorTag
///
/// @brief  Implementation of PFNCHIQUERYVENDORTAG defined in chinode.h
///
/// @param  pQueryVendorTag Pointer to a structure that returns the exported vendor tag
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult SplitNodeQueryVendorTag(
    CHIQUERYVENDORTAG* pQueryVendorTag)
{
    CDKResult result = CDKResultSuccess;
    result = ChiNodeUtils::QueryVendorTag(pQueryVendorTag, g_VendorTagInfoSplit);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SplitNodeCreate
///
/// @brief  Implementation of PFNNODECREATE defined in chinode.h
///
/// @param  pCreateInfo Pointer to a structure that defines create session information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult SplitNodeCreate(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult       result  = CDKResultSuccess;
    ChiSplitNode*  pNode   = NULL;

    if (NULL == pCreateInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pTagTypeInfo is NULL");
    }

    if ((CDKResultSuccess == result) && (pCreateInfo->size < sizeof(CHINODECREATEINFO)))
    {
        LOG_ERROR(CamxLogGroupChi, "CHINODECREATEINFO is smaller than expected");
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {
        pNode = new ChiSplitNode;
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
/// SplitNodeDestroy
///
/// @brief  Implementation of PFNNODEDESTROY defined in chinode.h
///
/// @param  pDestroyInfo    Pointer to a structure that defines the session destroy information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult SplitNodeDestroy(
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
            ChiSplitNode* pNode = static_cast<ChiSplitNode*>(pDestroyInfo->hNodeSession);
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
/// SplitNodeQueryBufferInfo
///
/// @brief  Implementation of PFNNODEQUERYBUFFERINFO defined in chinode.h
///
/// @param  pQueryBufferInfo    Pointer to a structure to query the input buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult SplitNodeQueryBufferInfo(
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
            ChiSplitNode* pNode = static_cast<ChiSplitNode*>(pQueryBufferInfo->hNodeSession);
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
/// SplitNodeSetBufferInfo
///
/// @brief  Implementation of PFNNODESETBUFFERINFO defined in chinode.h
///
/// @param  pSetBufferInfo  Pointer to a structure with information to set the output buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult SplitNodeSetBufferInfo(
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
            ChiSplitNode* pNode = static_cast<ChiSplitNode*>(pSetBufferInfo->hNodeSession);
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
/// SplitNodeProcRequest
///
/// @brief  Implementation of PFNNODEPROCREQUEST defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult SplitNodeProcRequest(
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
            ChiSplitNode* pNode = static_cast<ChiSplitNode*>(pProcessRequestInfo->hNodeSession);
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
/// SplitNodeSetNodeInterface
///
/// @brief  Implementation of PFCHINODESETNODEINTERFACE defined in chinode.h
///
/// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID SplitNodeSetNodeInterface(
    ChiNodeInterface* pNodeInterface)
{
    CDKResult result = CDKResultSuccess;
    result = ChiNodeUtils::SetNodeInterface(pNodeInterface, SplitNodeSectionName,
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
            pNodeCallbacks->pGetCapabilities         = SplitNodeGetCaps;
            pNodeCallbacks->pQueryVendorTag          = SplitNodeQueryVendorTag;
            pNodeCallbacks->pCreate                  = SplitNodeCreate;
            pNodeCallbacks->pDestroy                 = SplitNodeDestroy;
            pNodeCallbacks->pQueryBufferInfo         = SplitNodeQueryBufferInfo;
            pNodeCallbacks->pSetBufferInfo           = SplitNodeSetBufferInfo;
            pNodeCallbacks->pProcessRequest          = SplitNodeProcRequest;
            pNodeCallbacks->pChiNodeSetNodeInterface = SplitNodeSetNodeInterface;
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
/// ChiSplitNode::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiSplitNode::Initialize(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pCreateInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pCreateInfo is NULL");
    }
    /// @todo (CAMX-1854) Check for Node Capabilities using NodeCapsMask
    if (CDKResultSuccess == result)
    {
        m_hChiSession    = pCreateInfo->hChiSession;
        m_nodeId         = pCreateInfo->nodeId;
        m_nodeCaps       = pCreateInfo->nodeCaps.nodeCapsMask;
        m_nodeFlags      = pCreateInfo->nodeFlags;
    }

    m_processedFrame = 0;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiSplitNode::QueryBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiSplitNode::QueryBufferInfo(
    CHINODEQUERYBUFFERINFO* pQueryBufferInfo)
{
    CDKResult result                     = CDKResultSuccess;

    // set the nodeCaps later
    LOG_INFO(CamxLogGroupChi, "m_nodeCaps = %d", m_nodeCaps);
    switch (m_nodeCaps)
    {
        case ChiNodeCapsTwoFrameSplit:
        case ChiNodeCapsThreeFrameSplit:
            break;
        default:
            result = CDKResultEUnsupported;
            LOG_ERROR(CamxLogGroupChi, "Unsupported Node Cap m_nodeCaps mask = %x", m_nodeCaps);
            break;
    }

    if (CDKResultSuccess == result)
    {

        ChiNodeUtils::DefaultBufferNegotiation(pQueryBufferInfo);

        for (UINT outputIndex = 0; outputIndex < pQueryBufferInfo->numOutputPorts; outputIndex++)
        {
            ChiOutputPortQueryBufferInfo* pOutputPort = &pQueryBufferInfo->pOutputPortQueryInfo[outputIndex];
        }
        for (UINT inputIndex = 0; inputIndex < pQueryBufferInfo->numInputPorts; inputIndex++)
        {
            ChiInputPortQueryBufferInfo* pInputPort = &pQueryBufferInfo->pInputOptions[inputIndex];
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiSplitNode::SetBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiSplitNode::SetBufferInfo(
    CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo)
{
    LOG_INFO(CamxLogGroupChi, "SetBufferInfo Port [%d] Width=%d, Height=%d",
        pSetBufferInfo->portId, pSetBufferInfo->pFormat->width,
        pSetBufferInfo->pFormat->height);
    m_format.width  = pSetBufferInfo->pFormat->width;
    m_format.height = pSetBufferInfo->pFormat->height;
    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiSplitNode::CheckDependency
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ChiSplitNode::CheckDependency(
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

    tag = ANDROID_SENSOR_EXPOSURE_TIME;
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
/// ChiSplitNode::ProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiSplitNode::ProcessRequest(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{
    BOOL satisfied;

    if (IsRealTime())
    {
        satisfied = CheckDependency(pProcessRequestInfo);
    }
    else
    {
        satisfied = TRUE;
    }

    if (TRUE == satisfied)
    {
        SplitImage(pProcessRequestInfo);
        m_processedFrame++;
        UpdateMetaData(pProcessRequestInfo->frameNum);
    }

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiSplitNode::ChiSplitNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiSplitNode::ChiSplitNode()
    : m_hChiSession(NULL)
    , m_nodeId(0)
    , m_nodeCaps(0)
    , m_processedFrame(0)
{
    memset(&m_format, 0, sizeof(CHINODEIMAGEFORMAT));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiSplitNode::~ChiSplitNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiSplitNode::~ChiSplitNode()
{
    m_hChiSession = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiSplitNode::SplitImage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiSplitNode::SplitImage(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{
    CHINODEBUFFERHANDLE * phOutputBuffer = pProcessRequestInfo->phOutputBuffer;
    CHINODEBUFFERHANDLE * phInputBuffer  = pProcessRequestInfo->phInputBuffer;
    LOG_INFO(CamxLogGroupChi, "Enter processed frameNum %" PRIu64 "", pProcessRequestInfo->frameNum);

    UINT32 outPlaneStride  = 0;
    UINT32 inPlaneStride  = 0;
    UINT32 planeindex = 0, imagecountindex =0, outputbufferindex = 0, count = 0;
    UCHAR * inBuff = NULL, * outBuff = NULL,* pInput = NULL;
    UINT32 toggle = 0;

    /// @todo Depend on Shdr mode of sensor
    UINT32 numExpostureFrames = 3;
    UINT32 height[3] = {0, 0, 0};
    UINT32 numHorizontalBlankLineBeforeActualFrameData[3] =
    {
        NUMBER_OF_HORIZONTAL_BLANK_LINE_FOR_HEF,
        NUMBER_OF_HORIZONTAL_BLANK_LINE_FOR_SEF1,
        NUMBER_OF_HORIZONTAL_BLANK_LINE_FOR_SEF2
    };

    if (pProcessRequestInfo->inputNum == 1)
    {
        inBuff = phInputBuffer[0]->pImageList[0].pAddr[0];
        inPlaneStride = phInputBuffer[0]->format.formatParams.rawFormat.stride;
        outPlaneStride = phOutputBuffer[0]->format.formatParams.rawFormat.stride;

        // Cache invalidate on input buffers
        for (UINT32 i = 0; i < pProcessRequestInfo->inputNum; i++)
        {
            g_ChiNodeInterface.pCacheOps(pProcessRequestInfo->phInputBuffer[i], TRUE, FALSE);
        }

        LOG_INFO(CamxLogGroupChi, "inPlaneStride = %d outPlaneStride = %d height = %d",
            inPlaneStride, outPlaneStride, phOutputBuffer[0]->format.height);
        for (UINT32 i = 0; i < (phInputBuffer[0]->format.height/numExpostureFrames); i++)
        {
            for (UINT32 j = 0; j < pProcessRequestInfo->outputNum; j++)
            {
                pInput = (UCHAR *)&inBuff[inPlaneStride * j];
                if (pProcessRequestInfo->outputNum > 1)
                {
                    if ((pInput[0] & HEF_FRAME_MASK) == HEF_FRAME_MASK) // HEF
                    {
                        outputbufferindex = 0;
                    }
                    else if ((pInput[0] & SEF1_FRAME_MASK) == SEF1_FRAME_MASK) // SEF1
                    {
                        outputbufferindex = 1;
                    }
                    else if ((pInput[0] & SEF2_FRAME_MASK) == SEF2_FRAME_MASK) // SEF2
                    {
                        outputbufferindex = 2;
                    }
                    if (i < numHorizontalBlankLineBeforeActualFrameData[outputbufferindex])
                    {
                        continue;
                    }
                    if (height[outputbufferindex] >= phOutputBuffer[0]->format.height)
                    {
                        continue;
                    }
                }
                else
                {
                    outputbufferindex = 0;
                }
                outBuff = phOutputBuffer[outputbufferindex]->pImageList[imagecountindex].pAddr[planeindex];
                memcpy(&outBuff[outPlaneStride * height[outputbufferindex]], &inBuff[inPlaneStride * j] + 16, outPlaneStride);
                height[outputbufferindex]++;
            }
            inBuff = inBuff + (inPlaneStride * numExpostureFrames);
        }

        // Cache clean on ouput buffer
        for (UINT32 i = 0; i < pProcessRequestInfo->outputNum; i++)
        {
            g_ChiNodeInterface.pCacheOps(pProcessRequestInfo->phOutputBuffer[i], FALSE, TRUE);
        }

    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "number of input frame should be 1");
    }
    LOG_INFO(CamxLogGroupChi, "Done frameNum %" PRIu64 "", pProcessRequestInfo->frameNum);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiSplitNode::UpdateMetaData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ChiSplitNode::UpdateMetaData(
    UINT64 requestId)
{
    CHIMETADATAINFO        metadataInfo     = {0};
    const UINT32           tagSize          = sizeof(g_VendorTagSectionSplit) / sizeof(g_VendorTagSectionSplit[0]);
    CHITAGDATA             tagData[tagSize] = { {0} };
    UINT32                 tagList[tagSize];

    metadataInfo.size       = sizeof(CHIMETADATAINFO);
    metadataInfo.chiSession = m_hChiSession;
    metadataInfo.tagNum     = tagSize;
    metadataInfo.pTagList   = &tagList[0];
    metadataInfo.pTagData   = &tagData[0];

    UINT32 index = 0;

    tagList[index]              = g_VendorTagList[index] + g_vendorTagBase;
    tagData[index].size         = sizeof(CHITAGDATA);
    tagData[index].requestId    = requestId;
    tagData[index].pData        = &m_processedFrame;
    tagData[index].dataSize     = g_VendorTagSectionSplit[index].numUnits;

    g_ChiNodeInterface.pSetMetadata(&metadataInfo);

#if PRINT_METADATA
    CHIVENDORTAGBASEINFO vendorTagBase  = {0};
    vendorTagBase.size                  = sizeof(CHIVENDORTAGBASEINFO);
    vendorTagBase.pComponentName        = SplitNodeSectionName;
    vendorTagBase.pTagName              = g_VendorTagSectionSplit[index].pVendorTagName;
    g_ChiNodeInterface.pGetVendorTagBase(&vendorTagBase);
    LOG_VERBOSE(CamxLogGroupChi, "Vendor Tags value shall be same %x %x",
            g_VendorTagList[index] + g_vendorTagBase, vendorTagBase.vendorTagBase);

    // get the updated metadata from CamX and print them out
    VOID* pData = NULL;
    pData = ChiNodeUtils::GetMetaData(requestId, SplitNodeTagProcessedFrameNum + g_vendorTagBase, ChiMetadataDynamic,
        &g_ChiNodeInterface, m_hChiSession);
    if (NULL != pData)
    {
        LOG_VERBOSE(CamxLogGroupChi, "processed frameNum %" PRIu64 " processed frame %" PRIu64,
            *static_cast<UINT64*>(pData), m_processedFrame);
    }
#endif
}

// CAMX_NAMESPACE_END
