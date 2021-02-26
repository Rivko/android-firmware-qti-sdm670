////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodeldcwarp.h
/// @brief Chi node for LdcWarp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXCHINODELdcWarp_H
#define CAMXCHINODELdcWarp_H

#include "chiipedefs.h"
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


// NOWHINE FILE NC004c: Things outside the Camx namespace should be prefixed with Camx/CSL

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Chi node structure for Chi interface.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ChiLdcWarpNode
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
    /// LdcWarpLibIonMalloc
    ///
    /// @brief  Called by library for allocating ion memory
    ///
    /// @param  mmpa_ion_info in specific structure defined below
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID LdcWarpLibIonMalloc(
        mmap_info_ion * p_ion_mmap_info);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LdcWarpLibIonFree
    ///
    /// @brief  Called by library for freeing ion memory
    ///
    /// @param  mmpa_ion_info in specific structure defined below
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID LdcWarpLibIonFree(
        mmap_info_ion * p_ion_mmap_info);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LdcWarpCompletionCallback
    ///
    /// @brief  Callback function from Chinode
    ///
    /// @param  user_data user_data.
    /// @param  err       err returned from lib incase of timeout.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID LdcWarpCompletionCallback(
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
    /// ChiLdcWarpNode
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChiLdcWarpNode();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~ChiLdcWarpNode
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~ChiLdcWarpNode();

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

    int             m_num_ver_x;
    int             m_num_ver_y;
    IMGBUFFERPRIVT* m_pImgBuffInfo;
    IMGMEMHANDLET   m_hMeshBuffer;

private:
    ChiLdcWarpNode(const ChiLdcWarpNode&) = delete;               ///< Disallow the copy constructor
    ChiLdcWarpNode& operator=(const ChiLdcWarpNode&) = delete;    ///< Disallow assignment operator

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LoadLib
    ///
    /// @brief  Load the algo lib and map function pointers
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult LoadLib();

    CHIHANDLE           m_hChiSession;  ///< The Chi session handle
    UINT32              m_nodeId;       ///< The node's Id
    UINT32              m_nodeCaps;     ///< The selected node caps
    CHINODEFLAGS        m_nodeFlags;    ///< Node flags
    CHINODEIMAGEFORMAT  m_format;       ///< The selected format

    UINT64              m_processedFrame;   ///< The count for processed frame

    INT32 m_inputWidth;
    INT32 m_inputHeight;
    INT32 m_outputWidth;
    INT32 m_outputHeight;
    FLOAT m_bytesperpixel_input;
    FLOAT m_bytesperpixel_output;

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

    typedef struct LdcCallback
    {
        void *user_data_Node;
    } LDCCALLBACK;

    ///< Typedefs for interface functions
    typedef int32_t(*LDCWARPLIBINIT) (void **handle, image_warping_config_t* p_image_warping_config);

    typedef int32_t(*LDCWARPLIBDEINIT) (void *handle);

    typedef int32_t (*LDCWARPLIBPROCESS) (void *handle, image_warping_bundle_t *buff_bundle, void *user_data);

    CHILIBRARYHANDLE    m_hLdcWarpLib;              ///< Handle for LdcWarp library.
    LDCWARPLIBINIT      m_ldcWarp_lib_init;         ///< Function pointer for init
    LDCWARPLIBPROCESS   m_ldcWarp_lib_process;      ///< Function pointer for process
    LDCWARPLIBDEINIT    m_ldcWarp_lib_deinit;       ///< Function pointer for deinit

};
#endif // CAMXCHINODELdcWarp_H
