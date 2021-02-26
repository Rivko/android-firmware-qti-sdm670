////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodeimagewarp.h
/// @brief Chi node for ImageWarp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXCHINODEImageWarp_H
#define CAMXCHINODEImageWarp_H

#include "chiipedefs.h"
#include "chiifedefs.h"
#include "chi.h"

#include "camxchinodeutil.h"
#include "chinode.h"

#if ANDROID
#include <sys/ioctl.h>
#include <include/linux/ion.h>
#include <sys/mman.h>
#endif

#ifndef ION_SYSTEM_HEAP_ID
#define ION_SYSTEM_HEAP_ID 25
#endif

#include "chieisdefs.h"

// NOWHINE FILE NC004c: Things outside the Camx namespace should be prefixed with Camx/CSL

/// @brief structure to contain all the vendor tag Ids that ImageWarp uses
struct ImageWarpVendorTags
{
    UINT32 residualCropTagId;               ///< IFE Residual Crop Info Vendor Tag Id
    UINT32 appliedCropTagId;                ///< IFE Applied Crop Info Vendor Tag Id
    UINT32 mountAngleTagId;                 ///< Camera Sensor Mount Angle Vendor Tag Id
    UINT32 cameraPositionTagId;             ///< Camera Sensor Position Vendor Tag Id
    UINT32 sensorInfoTagId;                 ///< Sensor Info Vendor Tag Id
    UINT32 currentSensorModeTagId;          ///< Sensor's current mode index Vendor Tag Id
    UINT32 scalerInfoTagId;                 ///< IFE Scaler Info Vendor Tag Id
    UINT32 currentFLLTagId;                 ///< Sensor current FLL Vendor Tag Id
};

/// @brief structure to contain all the per sensor data required for Mesh Fusion algortihm
struct MeshFusionPerSensorData
{
    CHINODEIMAGEFORMAT inputSize;               ///< Input Image size to EIS DG
    CHINODEIMAGEFORMAT outputSize;              ///< Output Image size from EIS DG
    CHIRECT            active_array;            ///< Sensor active array
    CHIRECT            sensorDimension;         ///< Output Sensor Dimension Rect
    UINT32             mountAngle;              ///< Sensor mount angle
    INT32              cameraPosition;          ///< Camera sensor position
    UINT32             targetFPS;               ///< Target Frame Rate
    UINT32             fll;                     ///< Sensor Frame Length Line
    UINT32             llpclk;                  ///< Sensor Line Length Pixel Clock
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Chi node structure for Chi interface.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ChiImageWarpNode
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialization required to create a node
    ///
    /// @param  pCreateInfo Pointer to a structure that defines create session information for the node.
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult Initialize(
        CHINODECREATEINFO* pCreateInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Destroying a node
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// QueryBufferInfo
    ///
    /// @brief  Implementation of PFNNODEQUERYBUFFERINFO defined in chinode.h
    ///
    /// @param  pQueryBufferInfo    Pointer to a structure to query the input buffer resolution and type.
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult QueryBufferInfo(
        CHINODEQUERYBUFFERINFO* pQueryBufferInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetBufferInfo
    ///
    /// @brief  Implementation of PFNNODESETBUFFERINFO defined in chinode.h
    ///
    /// @param  pSetBufferInfo  Pointer to a structure with information to set the output buffer resolution and type.
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult SetBufferInfo(
        CHINODESETBUFFERPROPERTIESINFO* pSetBufferInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PostPipelineCreate
    ///
    /// @brief  Implementation of PFNPOSTPIPELINECREATE defined in chinode.h
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult PostPipelineCreate();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ProcessRequest
    ///
    /// @brief  Implementation of PFNNODEPROCREQUEST defined in chinode.h
    ///
    /// @param  pProcessRequestInfo Pointer to a structure that defines the information required for processing a request.
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult ProcessRequest(
        CHINODEPROCESSREQUESTINFO* pProcessRequestInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetVendorTagBase
    ///
    /// @brief  Get vendor tag base
    ///
    /// @return CDKResultSuccess or error code
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult GetVendorTagBases();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CheckDependency
    ///
    /// @brief  Check if the dependencies are satisfied. Set the dependency if the dependency is not met.
    ///
    /// @param  pProcessRequestInfo   Pointer to a structure that defines the information required for processing a request.
    ///
    /// @return TRUE if dependency is satisfied, FALSE otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL CheckDependency(
        CHINODEPROCESSREQUESTINFO* pProcessRequestInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsRealTime
    ///
    /// @brief  Method to query if node is part of realtime pipeline
    ///
    /// @return TRUE or FALSE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL IsRealTime() const
    {
        return m_nodeFlags.isRealTime;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ImageWarpLibIonMalloc
    ///
    /// @brief  Called by library for allocating ion memory
    ///
    /// @param  mmpa_ion_info in specific structure defined below
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID ImageWarpLibIonMalloc(
        mmap_info_ion * p_ion_mmap_info);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ImageWarpLibIonFree
    ///
    /// @brief  Called by library for freeing ion memory
    ///
    /// @param  mmpa_ion_info in specific structure defined below
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID ImageWarpLibIonFree(
        mmap_info_ion * p_ion_mmap_info);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ImageWarpCompletionCallback
    ///
    /// @brief  Callback function from ImageWarp Processing
    ///
    /// @param  user_data user_data.
    /// @param  err       err returned from lib incase of timeout.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID ImageWarpCompletionCallback(
        VOID * user_data,
        int err);

    /// @brief Image memory handle
    struct imgMemHandle
    {
        INT32   bufferFd;       ///< buffer fd
        UCHAR*  pVirtualAddr;   ///< buffer virtual addr
        VOID*   phBufferHandle; ///< buffer handle
        UINT    length;         ///< buffer length
    };
    typedef imgMemHandle IMGMEMHANDLET;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ImageBufferGet
    ///
    /// @brief  Utility function to get ion buffer for warp map
    ///
    /// @param  length    length of the buffer
    /// @param  pHandle   buffer handle
    ///
    /// @return CDKResultSuccess or error code
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult ImageBufferGet(
        INT32               length,
        IMGMEMHANDLET*      pHandle);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ImageBufferOpen
    ///
    /// @brief  Utility function to open ion buffer
    ///
    /// @return File descriptor or error code
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 ImageBufferOpen();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiImageWarpNode
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChiImageWarpNode();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~ChiImageWarpNode
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~ChiImageWarpNode();

    /// @brief Buffer structure
    struct imgBuffer
    {
        struct ion_allocation_data      allocData;          ///< ION allocation data
        INT32                           buffFd;             ///< buffer fd
        INT32                           ionFd;              ///< ION fd
        UINT8*                          pAddr;              ///< virtual addr
        INT32                           cached;             ///< flag indicating cached or not
    };
    typedef imgBuffer IMGBUFFERT;

    /// @brief Data structure to hold generic data for the img buffer.
    struct imgBufferPriv
    {
        INT32                     ionFd;          ///< ion device fd
        INT32                     refCount;       ///< ref count for no of clients opening ion device
    };
    typedef imgBufferPriv IMGBUFFERPRIVT;

    void *                 m_p_handle;
    void **                m_handle;
    image_warping_config_t m_config_warp;
    mmap_info_ion          m_ion_mmap_info;
    image_warping_bundle_t m_buff_bundle;
    image_warp_buffer_t    m_input_buf;
    image_warp_buffer_t    m_output_buf;
    image_warp_buffer_t    m_mesh_buf;

    int                    m_num_ver_x;
    int                    m_num_ver_y;
    IMGBUFFERPRIVT*        m_pImgBuffInfo;
    IMGMEMHANDLET          m_hMeshBuffer;

    void **                m_pp_handle;
    mesh_config_t          m_meshfusion_config;
    mesh_fusion_win_t      m_sensor_window;
    mesh_fusion_win_t      m_isp_window;

private:
    ChiImageWarpNode(const ChiImageWarpNode&) = delete;               ///< Disallow the copy constructor
    ChiImageWarpNode& operator=(const ChiImageWarpNode&) = delete;    ///< Disallow assignment operator

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// UpdateAdaptWindow
    ///
    /// @brief  Update ISP and sensor window to EISDG algo
    ///
    /// @param  requestId  Process request Id
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult UpdateAdaptWindow(
        UINT64 requestId);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LoadLib
    ///
    /// @brief  Load the algo lib and map function pointers
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult LoadLib();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FillVendorTagIds
    ///
    /// @brief  Query all the vendor tag locations that EISv2 uses and save them
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult FillVendorTagIds();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetEisDGMargin
    ///
    /// @brief  Get EISDGMargin
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult GetEisDGMargin();

    CHIHANDLE           m_hChiSession;  ///< The Chi session handle
    UINT32              m_nodeId;       ///< The node's Id
    UINT32              m_nodeCaps;     ///< The selected node caps
    CHINODEFLAGS        m_nodeFlags;    ///< Node flags
    ChiFormat           m_outputFormat; ///< The output port format

    UINT64              m_processedFrame;   ///< The count for processed frame

    INT32              m_inputWidth;
    INT32              m_inputHeight;
    INT32              m_outputWidth;
    INT32              m_outputHeight;
    FLOAT              m_bytesperpixel_input;
    FLOAT              m_bytesperpixel_output;
    MarginRequest      m_EISMarginRequest;
    void *             m_handle_Fusion;

    typedef enum PerformanceFlag
    {
        OpenGL_es_implementation = 0,
        OpenCL_implementation = 1,
        OpenGL_es_with_separate_plane_rendering = 2,
        OpenCLPerfTurboClk_implementation = 3
    } PERFORMANCEFLAG;

    typedef enum DebugLogFlag
    {
        NoDebuglogInfo = 0,
        BasicDebuglogInfo = 1,
        AdvancedDebuglogInfo = 2
    } DEBUGLOGFLAG;

    typedef struct ImageWarpCallback
    {
        void *user_data_Node;
    } IMAGEWARPCALLBACK;

    ///< Typedefs for interface functions
    typedef int32_t (*IMAGEWARPLIBINIT) (void **handle, image_warping_config_t* p_image_warping_config);

    typedef int32_t (*IMAGEWARPLIBDEINIT) (void *handle);

    typedef int32_t (*IMAGEWARPLIBPROCESS) (void *handle, image_warping_bundle_t *buff_bundle, void *user_data);

    typedef int32_t (*MESHFUSIONLIBINIT) (void** handle);

    typedef int32_t (*MESHFUSIONLIBADAPTWINDOW) (void * handle, mesh_config_t* config,
                                                       mesh_fusion_win_t* sensor_win, mesh_fusion_win_t* isp_win);

    typedef int32_t (*MESHFUSIONLIBPROCESS) (void* handle, mesh_config_t* config);

    typedef int32_t (*MESHFUSIONLIBDEINIT) (void* handle);
    CHILIBRARYHANDLE            m_hImageWarpLib;                  ///< Handle for ImageWarp library.
    IMAGEWARPLIBINIT            m_ImageWarp_lib_init;             ///< Function pointer for init
    IMAGEWARPLIBPROCESS         m_ImageWarp_lib_process;          ///< Function pointer for process
    IMAGEWARPLIBDEINIT          m_ImageWarp_lib_deinit;           ///< Function pointer for deinit

    CHILIBRARYHANDLE            m_hmeshFusionLib;               ///< Handle for MeshFusion library.
    MESHFUSIONLIBINIT           m_meshFusion_lib_init;          ///< Function pointer for init
    MESHFUSIONLIBADAPTWINDOW    m_meshFusion_lib_adapt_window;  ///< Function pointer for adapt window
    MESHFUSIONLIBPROCESS        m_meshFusion_lib_process;       ///< Function pointer for process
    MESHFUSIONLIBDEINIT         m_meshFusion_lib_deinit;        ///< Function pointer for deinit

    MeshFusionPerSensorData     m_perSensorData;                ///< Per camera data
};
#endif // CAMXCHINODEImageWarp_H
