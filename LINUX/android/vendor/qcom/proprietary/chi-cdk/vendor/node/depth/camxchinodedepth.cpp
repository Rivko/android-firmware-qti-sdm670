////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodedepth.cpp
/// @brief Chi node for depth
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <inttypes.h>
#include <system/camera_metadata.h>
#include "chi.h"
#include "chistatsproperty.h"
#include "camxchinodedepth.h"

// Support only Y8, Y16 and POINTCLOUD
#define MAX_DEPTH_OUTPUT_FORMATS 3

ChiNodeInterface g_ChiNodeInterface;     ///< The instance save the CAMX Chi interface

static const UINT32 ChiNodeMajorVersion = 0;     ///< The major version of CHI interface
static const UINT32 ChiNodeMinorVersion = 0;     ///< The minor version of CHI interface

static const UINT32 ChiNodeCapsIR         = 1;          ///< IR
static const UINT32 ChiNodeCapsDepth16    = 1 << 1;     ///< Depth16
static const UINT32 ChiNodeCapsPointCloud = 1 << 2;     ///< Point cloud

static const CHAR   DepthNodeSectionName[] = "com.qti.node.depth";     ///< The section name for node

static CHIVENDORTAGDATA g_VendorTagSectionAECData[] =
{
    { "AECData", 0, sizeof(ChiAECData) },
};

static CHIVENDORTAGDATA g_VendorTagSectionAECDataPublisherPresent[] =
{
    { "AECDataPublisherPresent", 0, 1 },
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthNodeGetCaps
///
/// @brief  Implementation of PFNNODEGETCAPS defined in chinode.h
///
/// @param  pCapsInfo  Pointer to a structure that defines the capabilities supported by the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DepthNodeGetCaps(
    CHINODECAPSINFO* pCapsInfo)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == pCapsInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pCapsInfo is NULL");
    }

    // dereference pCapsInfo only when it is not NULL
    if (CDKResultSuccess == result)
    {
        if (pCapsInfo->size >= sizeof(CHINODECAPSINFO))
        {
            pCapsInfo->nodeCapsMask = ChiNodeCapsIR | ChiNodeCapsDepth16 | ChiNodeCapsPointCloud;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODECAPSINFO is smaller than expected, %u < %zu",
                      pCapsInfo->size, sizeof(CHINODECAPSINFO));
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthNodeCreate
///
/// @brief  Implementation of PFNNODECREATE defined in chinode.h
///
/// @param  pCreateInfo  Pointer to a structure that defines create session information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DepthNodeCreate(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult      result = CDKResultSuccess;
    ChiDepthNode*  pNode = NULL;

    if (NULL == pCreateInfo)
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pTagTypeInfo is NULL");
    }

    if (CDKResultSuccess == result)
    {
        if (pCreateInfo->size < sizeof(CHINODECREATEINFO))
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODECREATEINFO is smaller than expected, %u < %zu",
                      pCreateInfo->size, sizeof(CHINODECREATEINFO));
            result = CDKResultEFailed;
        }
    }

    if (CDKResultSuccess == result)
    {
        pNode = new ChiDepthNode;
        if (NULL == pNode)
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
        LOG_ERROR(CamxLogGroupChi, "DepthNodeCreate failed, %d", result);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthNodeDestroy
///
/// @brief  Implementation of PFNNODEDESTROY defined in chinode.h
///
/// @param  pDestroyInfo  Pointer to a structure that defines the session destroy information for the node.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DepthNodeDestroy(
    CHINODEDESTROYINFO* pDestroyInfo)
{
    CDKResult result = CDKResultSuccess;

    if ((NULL == pDestroyInfo) || (NULL == pDestroyInfo->hNodeSession))
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pDestroyInfo %p", pDestroyInfo);
    }

    if (CDKResultSuccess == result)
    {
        if (pDestroyInfo->size >= sizeof(CHINODEDESTROYINFO))
        {
            ChiDepthNode* pNode = static_cast<ChiDepthNode*>(pDestroyInfo->hNodeSession);
            delete pNode;

            pNode = NULL;
            pDestroyInfo->hNodeSession = NULL;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODEDESTROYINFO is smaller than expected, %u < %zu",
                      pDestroyInfo->size, sizeof(CHINODEDESTROYINFO));
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthNodeQueryBufferInfo
///
/// @brief  Implementation of PFNNODEQUERYBUFFERINFO defined in chinode.h
///
/// @param  pQueryBufferInfo  Pointer to a structure to query the input buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DepthNodeQueryBufferInfo(
    CHINODEQUERYBUFFERINFO* pQueryBufferInfo)
{
    CDKResult result = CDKResultSuccess;

    if ((NULL == pQueryBufferInfo) || (NULL == pQueryBufferInfo->hNodeSession))
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument: pQueryBufferInfo %p", pQueryBufferInfo);
    }

    if (CDKResultSuccess == result)
    {
        if (pQueryBufferInfo->size >= sizeof(CHINODEQUERYBUFFERINFO))
        {
            ChiDepthNode* pNode = static_cast<ChiDepthNode*>(pQueryBufferInfo->hNodeSession);
            result = pNode->QueryBufferInfo(pQueryBufferInfo);
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODEQUERYBUFFERINFO is smaller than expected, %u < %zu",
                      pQueryBufferInfo->size, sizeof(CHINODEQUERYBUFFERINFO));
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthNodeSetBufferInfo
///
/// @brief  Implementation of PFNNODESETBUFFERINFO defined in chinode.h
///
/// @param  pSetBufferInfo  Pointer to a structure with information to set the output buffer resolution and type.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DepthNodeSetBufferInfo(
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
            ChiDepthNode* pNode = static_cast<ChiDepthNode*>(pSetBufferInfo->hNodeSession);
            result = pNode->SetBufferInfo(pSetBufferInfo);
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODESETBUFFERPROPERTIESINFO is smaller than expected, %u < %zu",
                      pSetBufferInfo->size, sizeof(CHINODESETBUFFERPROPERTIESINFO));
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthNodeProcRequest
///
/// @brief  Implementation of PFNNODEPROCREQUEST defined in chinode.h
///
/// @param  pProcessRequestInfo  Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DepthNodeProcRequest(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{
    CDKResult result = CDKResultSuccess;

    LOG_VERBOSE(CamxLogGroupChi, "DepthNodeProcRequest");
    if ((NULL == pProcessRequestInfo) || (NULL == pProcessRequestInfo->hNodeSession))
    {
        result = CDKResultEInvalidPointer;
        LOG_ERROR(CamxLogGroupChi, "Invalid argument");
    }

    if (CDKResultSuccess == result)
    {
        if (pProcessRequestInfo->size >= sizeof(CHINODEPROCESSREQUESTINFO))
        {
            ChiDepthNode* pNode = static_cast<ChiDepthNode*>(pProcessRequestInfo->hNodeSession);
            result = pNode->ProcessRequest(pProcessRequestInfo);
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "CHINODEPROCESSREQUESTINFO is smaller than expected, %u < %zu",
                      pProcessRequestInfo->size, sizeof(CHINODEPROCESSREQUESTINFO));
            result = CDKResultEFailed;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthNodeSetNodeInterface
///
/// @brief  Implementation of PFCHINODESETNODEINTERFACE defined in chinode.h
///
/// @param  pProcessRequestInfo  Pointer to a structure that defines the information required for processing a request.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static VOID DepthNodeSetNodeInterface(
    ChiNodeInterface* pNodeInterface)
{
    CDKResult result = CDKResultSuccess;

    result = ChiNodeUtils::SetNodeInterface(pNodeInterface, DepthNodeSectionName,
                                            &g_ChiNodeInterface, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DepthNodePostPipelineCreate
///
/// @brief  Implementation of PFNPOSTPIPELINECREATE defined in chinode.h
///
/// @param  hChiHandle  Handle to the Chi depth node object.
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DepthNodePostPipelineCreate(
     CHIHANDLE hChiHandle)
{
    CDKResult     result = CDKResultSuccess;
    ChiDepthNode* pNode;

    if (NULL != hChiHandle)
    {
        pNode = static_cast<ChiDepthNode*>(hChiHandle);
        result = pNode->PostPipelineCreate();
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Invalid Chi Handle");
        result = CDKResultEInvalidPointer;
    }
    return result;
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
            pNodeCallbacks->size         >= sizeof(CHINODECALLBACKS))
        {
            pNodeCallbacks->majorVersion             = ChiNodeMajorVersion;
            pNodeCallbacks->minorVersion             = ChiNodeMinorVersion;
            pNodeCallbacks->pGetCapabilities         = DepthNodeGetCaps;
            pNodeCallbacks->pCreate                  = DepthNodeCreate;
            pNodeCallbacks->pDestroy                 = DepthNodeDestroy;
            pNodeCallbacks->pQueryBufferInfo         = DepthNodeQueryBufferInfo;
            pNodeCallbacks->pSetBufferInfo           = DepthNodeSetBufferInfo;
            pNodeCallbacks->pProcessRequest          = DepthNodeProcRequest;
            pNodeCallbacks->pChiNodeSetNodeInterface = DepthNodeSetNodeInterface;
            pNodeCallbacks->pPostPipelineCreate      = DepthNodePostPipelineCreate;
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
/// ChiMemCpyNode::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDepthNode::Initialize(
    CHINODECREATEINFO* pCreateInfo)
{
    CDKResult result;

    /// @todo (CAMX-1854) Check for Node Capabilities using NodeCapsMask
    LOG_VERBOSE(CamxLogGroupChi, "Initialize CHI depth node");
    m_hChiSession = pCreateInfo->hChiSession;
    m_nodeId      = pCreateInfo->nodeId;
    m_nodeCaps    = pCreateInfo->nodeCaps.nodeCapsMask;

    result = GetVendorTagBases();

    if (CDKResultSuccess == result)
    {
        /// "Signal" that this node will publish AEC Data
        CHIMETADATAINFO metadataInfo = { 0 };
        const UINT32    tagSize      = sizeof(g_VendorTagSectionAECDataPublisherPresent) /
           sizeof(g_VendorTagSectionAECDataPublisherPresent[0]);
        BOOL publishAECData          = TRUE;
        CHITAGDATA      tagData[tagSize];
        UINT32          tagList[tagSize];

        metadataInfo.size       = sizeof(CHIMETADATAINFO);
        metadataInfo.chiSession = m_hChiSession;
        metadataInfo.tagNum     = tagSize;
        metadataInfo.pTagList   = &tagList[0];
        metadataInfo.pTagData   = &tagData[0];
        tagList[0]              = m_AECDataPublisherPresentVendorTagBase | UsecaseMetadataSectionMask;
        tagData[0].size         = sizeof(CHITAGDATA);
        tagData[0].requestId    = 0;
        tagData[0].pData        = &publishAECData;
        tagData[0].dataSize     = g_VendorTagSectionAECDataPublisherPresent[0].numUnits;

        result = g_ChiNodeInterface.pSetMetadata(&metadataInfo);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDepthNode::QueryBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDepthNode::QueryBufferInfo(
    CHINODEQUERYBUFFERINFO* pQueryBufferInfo)
{
    CDKResult result = CDKResultSuccess;

    if (CDKResultSuccess == result)
    {
        ChiNodeUtils::DefaultBufferNegotiation(pQueryBufferInfo);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDepthNode::SetBufferInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDepthNode::SetBufferInfo(
    CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo)
{
    LOG_VERBOSE(CamxLogGroupChi, "%s: wxh = %ux%u", __func__, pSetBufferInfo->pFormat->width, pSetBufferInfo->pFormat->height);

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDepthNode::ProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDepthNode::ProcessRequest(
    CHINODEPROCESSREQUESTINFO* pProcessRequestInfo)
{
    CDKResult             result               = CDKResultSuccess;
    UINT32                outputImageLength;
    WrapperDepthCamConfig config;
    UINT                  numOutputs           = 0;
    VOID*                 pMetaData            = NULL;

    WrapperDepthOutput outputs[MAX_DEPTH_OUTPUT_FORMATS];
    WrapperDepthFormat depthFormat;

    if (FALSE == m_conversionLibInited)
    {
        return CDKResultEFailed;
    }

    // Check if AE is enabled
    camera_metadata_enum_android_control_ae_mode aeMode = ANDROID_CONTROL_AE_MODE_ON;
    pMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          ANDROID_CONTROL_AE_MODE | InputMetadataSectionMask,
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
    if (NULL != pMetaData)
    {
        aeMode = *reinterpret_cast<camera_metadata_enum_android_control_ae_mode*>(pMetaData);
    }
    else
    {
        aeMode = ANDROID_CONTROL_AE_MODE_OFF;
    }
    BOOL autoExposure = (ANDROID_CONTROL_AE_MODE_OFF != aeMode);

    // Get exposure time
    UINT64 exposureTime = 0;

    if (TRUE == autoExposure)
    {
        pMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                              ANDROID_SENSOR_EXPOSURE_TIME,
                                              ChiMetadataDynamic,
                                              &g_ChiNodeInterface,
                                              m_hChiSession);
        if (NULL != pMetaData)
        {
            exposureTime = *reinterpret_cast<UINT64*>(pMetaData);
        }
        config.in_out_depth_map_ae_val = exposureTime;
        config.in_depth_map_exp_mode = WRAPPER_EXP_MODE_AE;
    }
    else
    {
        // Read manual exposure time
        pMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                              ANDROID_SENSOR_EXPOSURE_TIME | InputMetadataSectionMask,
                                              ChiMetadataDynamic,
                                              &g_ChiNodeInterface,
                                              m_hChiSession);
        if (NULL != pMetaData)
        {
            exposureTime = *reinterpret_cast<UINT64*>(pMetaData);
        }
        config.in_depth_map_me_val = exposureTime;
        config.in_depth_map_exp_mode = WRAPPER_EXP_MODE_ME;
    }

    // Timestamp
    UINT64 timestamp = 0;
    pMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          ANDROID_SENSOR_TIMESTAMP,
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
    if (NULL != pMetaData)
    {
        timestamp = *reinterpret_cast<UINT64*>(pMetaData);
        config.in_out_depth_map_timestamp = timestamp;
    }

    // Frame Id
    config.in_depth_map_frame_num = 0;
    pMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          m_frameCountVendorTagBase,
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
    if (NULL != pMetaData)
    {
        CHITIMESTAMPINFO* pTimestampInfo;

        pTimestampInfo = reinterpret_cast<CHITIMESTAMPINFO*>(pMetaData);
        config.in_depth_map_frame_num = pTimestampInfo->frameId;
    }

    // FPS
    INT32 fps[2] = { 0, 0 };
    pMetaData = ChiNodeUtils::GetMetaData(pProcessRequestInfo->frameNum,
                                          ANDROID_CONTROL_AE_TARGET_FPS_RANGE | InputMetadataSectionMask,
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);
    if (NULL != pMetaData)
    {
        INT32* _fps = reinterpret_cast<INT32*>(pMetaData);

        config.in_depth_map_fps_range.min_fps       = _fps[0];
        config.in_depth_map_fps_range.video_min_fps = _fps[0];
        fps[0]                                      = _fps[0];
        config.in_depth_map_fps_range.max_fps       = _fps[1];
        config.in_depth_map_fps_range.video_max_fps = _fps[1];
        fps[1]                                      = _fps[1];
    }

    LOG_VERBOSE(CamxLogGroupChi,
                "Input: inputNum=%d, imageCount=%d, numPlanes=%d, wxh=%dx%d, format=%d,"
                " pAddr=%p, size=%zu, exposure=%" PRIu64 ", timestamp=%" PRIu64 ", frameCount=%" PRIu64 ", autoExposure=%s,"
                " fps[%d:%d]",
                pProcessRequestInfo->inputNum,
                pProcessRequestInfo->phInputBuffer[0]->imageCount,
                pProcessRequestInfo->phInputBuffer[0]->numberOfPlanes,
                pProcessRequestInfo->phInputBuffer[0]->format.width,
                pProcessRequestInfo->phInputBuffer[0]->format.height,
                pProcessRequestInfo->phInputBuffer[0]->format.format,
                pProcessRequestInfo->phInputBuffer[0]->pImageList[0].pAddr[0],
                pProcessRequestInfo->phInputBuffer[0]->planeSize[0],
                exposureTime, timestamp, config.in_depth_map_frame_num,
                (autoExposure ? "True":"False"), fps[0], fps[1]);

    // Sanity check
    numOutputs = pProcessRequestInfo->outputNum;
    if (numOutputs > MAX_DEPTH_OUTPUT_FORMATS)
    {
        LOG_WARN(CamxLogGroupChi, "Received too many output formats!");
        numOutputs = MAX_DEPTH_OUTPUT_FORMATS;
    }

    for (UINT i = 0; i < numOutputs; ++i)
    {
        LOG_VERBOSE(CamxLogGroupChi, "Output: outputNum=%u, imageCount=%u, numPlanes=%u, wxh=%ux%u, format=%d, pAddr=%p,"
                    " size=%zu",
                    i,
                    pProcessRequestInfo->phOutputBuffer[i]->imageCount,
                    pProcessRequestInfo->phOutputBuffer[i]->numberOfPlanes,
                    pProcessRequestInfo->phOutputBuffer[i]->format.width,
                    pProcessRequestInfo->phOutputBuffer[i]->format.height,
                    pProcessRequestInfo->phOutputBuffer[i]->format.format,
                    pProcessRequestInfo->phOutputBuffer[i]->pImageList[0].pAddr[0],
                    pProcessRequestInfo->phOutputBuffer[i]->planeSize[0]);

        switch (pProcessRequestInfo->phOutputBuffer[i]->format.format)
        {
        case Y8:
            outputs[i].format = WRAPPER_CAM_FORMAT_IR;
            outputs[i].out_image_length =
                pProcessRequestInfo->phOutputBuffer[i]->planeSize[0];
            outputs[i].out_depth_image =
                pProcessRequestInfo->phOutputBuffer[i]->pImageList[0].pAddr[0];
            break;

        case Y16:
            outputs[i].format = WRAPPER_CAM_FORMAT_DEPTH16;
            outputs[i].out_image_length =
                pProcessRequestInfo->phOutputBuffer[i]->planeSize[0];
            outputs[i].out_depth_image =
                pProcessRequestInfo->phOutputBuffer[i]->pImageList[0].pAddr[0];
            break;

        default:
            outputs[i].format = WRAPPER_CAM_FORMAT_NONE;
            break;
        }
    }

    bool libraryResult = m_processFrame(m_DepthMapWrapper,
                                        (const INT8*)pProcessRequestInfo->phInputBuffer[0]->pImageList[0].pAddr[0],
                                        &config,
                                        pProcessRequestInfo->outputNum,
                                        outputs);
    result = (libraryResult) ? CDKResultSuccess : CDKResultEFailed;

    // Always publish
    if (autoExposure)
    {
        PublishExposureData(pProcessRequestInfo->frameNum, config.in_out_depth_map_ae_val);
    }
    else
    {
        PublishExposureData(pProcessRequestInfo->frameNum, config.in_depth_map_me_val);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDepthNode::PostPipelineCreate
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDepthNode::PostPipelineCreate()
{
    CDKResult result = CDKResultEFailed;

    if (TRUE == m_vendorTagsInited)
    {
        result = InitConversionLib();
    }

    if (CDKResultSuccess == result)
    {
        result = PublishExposureData(0, m_initialExposureTime);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDepthNode::ChiDepthNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiDepthNode::ChiDepthNode()
    : m_hChiSession(NULL)
    , m_nodeId(0)
    , m_nodeCaps(0)
{
    m_vendorTagsInited                     = FALSE;
    m_conversionLibInited                  = FALSE;
    m_sensorModeInfoVendorTagBase          = 0;
    m_depthStreamDimensionVendorTagBase    = 0;
    m_AECDataVendorTagBase                 = 0;
    m_AECDataPublisherPresentVendorTagBase = 0;
    m_frameCountVendorTagBase              = 0;
    m_DepthMapWrapper                      = NULL;
    m_hDepthLib                            = NULL;
    m_createDepthMapWrapper                = NULL;
    m_destroyDepthMapWrapper               = NULL;
    m_processFrame                         = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDepthNode::~ChiDepthNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiDepthNode::~ChiDepthNode()
{
    if (NULL != m_DepthMapWrapper)
    {
        m_destroyDepthMapWrapper(m_DepthMapWrapper);
        m_DepthMapWrapper = NULL;
    }

    if (NULL != m_hDepthLib)
    {
        CDKResult result = CDKResultSuccess;
        result = ChiNodeUtils::LibUnmap(m_hDepthLib);
        if (CDKResultSuccess != result)
        {
            LOG_ERROR(CamxLogGroupChi, "Uninitialize Failed to unmap lib %s: %d", "libdepthmapwrapper.so", result);
        }
        m_hDepthLib = NULL;
    }
    m_hChiSession = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDepthNode::GetVendorTagBases
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDepthNode::GetVendorTagBases()
{
    CDKResult            result            = CDKResultSuccess;
    CHIVENDORTAGBASEINFO vendorTagBaseInfo = { 0 };

    // Get the "sensor_mode_info" vendor tag base
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

    if (CDKResultSuccess == result)
    {
        result = ChiNodeUtils::GetVendorTagBase("org.quic.camera.streamDimension",
                                                "depth",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_depthStreamDimensionVendorTagBase = vendorTagBaseInfo.vendorTagBase;
        }
        else

        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get stream dimension vendor tag base");
        }
    }

    if (CDKResultSuccess == result)
    {
        result = ChiNodeUtils::GetVendorTagBase("org.quic.camera.AECDataPublisherPresent",
                                                "AECDataPublisherPresent",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_AECDataPublisherPresentVendorTagBase = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get AEC data publisher present vendor tag base");
        }
    }

    if (CDKResultSuccess == result)
    {
        result = ChiNodeUtils::GetVendorTagBase("org.quic.camera.AECData",
                                                "AECData",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_AECDataVendorTagBase = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get AEC data vendor tag base");
        }
    }

    if (CDKResultSuccess == result)
    {
        result = ChiNodeUtils::GetVendorTagBase("org.quic.camera.qtimer",
                                                "timestamp",
                                                &g_ChiNodeInterface,
                                                &vendorTagBaseInfo);
        if (CDKResultSuccess == result)
        {
            m_frameCountVendorTagBase = vendorTagBaseInfo.vendorTagBase;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Unable to get frame count vendor tag base");
        }
    }

    if (CDKResultSuccess == result)
    {
        m_vendorTagsInited = TRUE;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDepthNode::InitConversionLib
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDepthNode::InitConversionLib()
{
    CDKResult result    = CDKResultSuccess;
    VOID*     pMetaData = NULL;

    // Get sensor mode info
    pMetaData = ChiNodeUtils::GetMetaData(0,
                                          (m_sensorModeInfoVendorTagBase | UsecaseMetadataSectionMask),
                                          ChiMetadataDynamic,
                                          &g_ChiNodeInterface,
                                          m_hChiSession);

    if (NULL != pMetaData)
    {
        ChiSensorModeInfo* pSensorModeInfo = reinterpret_cast<ChiSensorModeInfo*>(pMetaData);

        LOG_VERBOSE(CamxLogGroupChi, "Sensor Output Dimensions %ux%u", pSensorModeInfo->frameDimension.width,
                    pSensorModeInfo->frameDimension.height);
        m_inputSize.width = pSensorModeInfo->frameDimension.width;
        m_inputSize.height = pSensorModeInfo->frameDimension.height;
    }
    else
    {
        LOG_ERROR(CamxLogGroupChi, "Error getting depth input dimension");
        result = CDKResultEFailed;
    }

    if (CDKResultSuccess == result)
    {
        pMetaData = ChiNodeUtils::GetMetaData(0,
                                              (m_depthStreamDimensionVendorTagBase | UsecaseMetadataSectionMask),
                                              ChiMetadataDynamic,
                                              &g_ChiNodeInterface,
                                              m_hChiSession);

        if (NULL != pMetaData)
        {
            ChiBufferDimension* pdepthStreamDimension = reinterpret_cast<ChiBufferDimension*>(pMetaData);

            LOG_VERBOSE(CamxLogGroupChi, "Depth Output Dimensions %ux%u", pdepthStreamDimension->width,
                        pdepthStreamDimension->height);
            m_outputSize.width = pdepthStreamDimension->width;
            m_outputSize.height = pdepthStreamDimension->height;
        }
        else
        {
            LOG_ERROR(CamxLogGroupChi, "Error getting depth output dimension");
            result = CDKResultEFailed;
        }
    }

    if (CDKResultSuccess == result)
    {
        m_hDepthLib = ChiNodeUtils::LibMap("libdepthmapwrapper", "/vendor/lib/camera/components/");
        if (NULL == m_hDepthLib)
        {
            LOG_ERROR(CamxLogGroupChi, "Error loading lib %s", "libdepthmapwrapper.so");
            result = CDKResultEFailed;
        }
        else
        {
            m_createDepthMapWrapper = reinterpret_cast<CREATEDEPTHMAPWRAPPER>(
                ChiNodeUtils::LibGetAddr(m_hDepthLib, "DepthMapWrapperCreate"));
            m_destroyDepthMapWrapper = reinterpret_cast<DESTROYDEPTHMAPWRAPPER>(
                ChiNodeUtils::LibGetAddr(m_hDepthLib, "DepthMapWrapperDestroy"));
            m_processFrame = reinterpret_cast<PROCESSFRAME>(ChiNodeUtils::LibGetAddr(m_hDepthLib,
                                                                                     "DepthMapWrapperProcessFrame"));
            if (NULL == m_createDepthMapWrapper || NULL == m_destroyDepthMapWrapper || NULL == m_processFrame)
            {
                LOG_ERROR(CamxLogGroupChi, "Error getting function address(es) from library: %s", "libdepthmapwrapper.so");
                result = CDKResultEFailed;
            }
            else
            {
                m_DepthMapWrapper = m_createDepthMapWrapper(m_inputSize.width,
                                                            m_inputSize.height,
                                                            m_outputSize.width,
                                                            m_outputSize.height,
                                                            &m_initialExposureTime,
                                                            &m_initialSensitivity);
                if (NULL == m_DepthMapWrapper)
                {
                    LOG_ERROR(CamxLogGroupChi, "Error creating depth map wrapper");
                    result = CDKResultEFailed;
                }
                else
                {
                    m_conversionLibInited = TRUE;
                }
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiDepthNode::PublishExposureData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ChiDepthNode::PublishExposureData(
    UINT64 requestId,
    UINT64 exposureTime)
{
    CDKResult       result       = CDKResultSuccess;
    CHIMETADATAINFO metadataInfo = { 0 };
    const UINT32    tagSize      = sizeof(g_VendorTagSectionAECData) / sizeof(g_VendorTagSectionAECData[0]);
    UINT32          index        = 0;
    CHITAGDATA      tagData[tagSize];
    UINT32          tagList[tagSize];

    metadataInfo.size       = sizeof(CHIMETADATAINFO);
    metadataInfo.chiSession = m_hChiSession;
    metadataInfo.tagNum     = tagSize;
    metadataInfo.pTagList   = &tagList[0];
    metadataInfo.pTagData   = &tagData[0];

    CHIAECDATA AECData;
    memset(&AECData, 0, sizeof(CHIAECDATA));
    AECData.exposureTime = exposureTime;
    AECData.sensitivity  = 0;

    tagList[index]           = m_AECDataVendorTagBase | UsecaseMetadataSectionMask;
    tagData[index].size      = sizeof(CHITAGDATA);
    tagData[index].requestId = requestId;
    tagData[index].pData     = &AECData;
    tagData[index].dataSize  = g_VendorTagSectionAECData[index].numUnits;

    result = g_ChiNodeInterface.pSetMetadata(&metadataInfo);
    if (CDKResultSuccess != result)
    {
        LOG_ERROR(CamxLogGroupChi, "Error publishing AEC data");
    }
    return result;
}

