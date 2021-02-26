////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodesvhdr.h
/// @brief Chi node for YUVSHDR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXCHINODEYUVSHDR_H
#define CAMXCHINODEYUVSHDR_H

/*
#define MAX_EXP_ENTRIES 4
#define MAX_INPUT_HDR_FRAMES 2
#define MAX_OUTPUT_HDR_FRAMES 1
#define MAX_SHDR_BG_STATS_SIZE 1024
#define MAX_SHDR_BHIST_STATS_SIZE 256
#define BAYERPROC_MAX_USED_BUFS 16
*/
#define SHDR_MAX_NUM_BUFFERS 32
#define SHDR_MAX_INPUT_FRAMES 2
#define MAX_REQ               12
#define YUVSHDR_ALGO_2FRAME   1
#define YUVSHDR_ALGO_3FRAME   2


#include "chinode.h"
#include "camxchinodeutil.h"
#include "shdr3.h"
#include "tuningsetmanager.h"
#include "trigger_util.h"



static const UINT32 MaxRequests   = 64; ///< Max Requests
static const UINT32 Defog_Latency = 4;

typedef shdr3_tuning_focus_iir_t  SHDR3TUNINGFOCUSIIR;
typedef shdr3_tuning_focus_fir_t  SHDR3TUNINGFOCUSFIR;
typedef focus_win_t               FOCUSWIN;
typedef shdr3_stats_info_t        SHDR3STATSINFO;
typedef shdr3_buffer_t            SHDR3BUFFER;
typedef shdr3_tuning_eplp_t       SHDR3TUNINGEPLP;
typedef shdr3_tuning_tm_t         SHDR3TUNINGTM;
typedef shdr3_tuning_de_t         SHDR3TUNINGDE;
typedef shdr3_tuning_fusion_t     SHDR3TUNINGFUSION;
typedef autoltm3_tuning_t         AUTOLTM3TUNING;
typedef shdr3_tuning_t            SHDR3TUNING;
typedef shdr3_warp_mesh_t         SHDR3WARPMESH;
typedef shdr3_init_config_t       SHDR3INITCONFIG;
typedef shdr3_input_param_t       SHDR3INPUTPARAM;


// NOWHINE FILE NC004c: Things outside the Camx namespace should be prefixed with Camx/CSL

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Chi node structure for Chi interface.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ChiYUVSHDRNode
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
    /// DefaultBufferNegotiation
    ///
    /// @brief  Implement the default buffer negotiation logic.
    ///
    /// @param  pQueryBufferInfo  Pointer to a structure that has the information of required output/input buffer
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID DefaultBufferNegotiation(CHINODEQUERYBUFFERINFO* pQueryBufferInfo);

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
    /// ImgAlgoShdrAECStatsReadyCallback
    ///
    /// @brief  Callback function from Chinode where Stats vendor tags are filled
    ///
    /// @param  stats stats.
    /// @param  user_data user_data.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID ImgAlgoShdrAECStatsReadyCallback(
        SHDR3STATSINFO  *stats,
        VOID * user_data);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DumpMetaAfterSetMeta
    ///
    /// @brief  Dumping Meta After SetMetaDats
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VOID DumpMetaAfterSetMeta(INT frameNum, CHIMETADATAINFO metadataInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiYUVSHDRNode
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChiYUVSHDRNode();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~ChiYUVSHDRNode
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~ChiYUVSHDRNode();


private:
    ChiYUVSHDRNode(const ChiYUVSHDRNode&) = delete;               ///< Disallow the copy constructor
    ChiYUVSHDRNode& operator=(const ChiYUVSHDRNode&) = delete;    ///< Disallow assignment operator

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LoadLib
    ///
    /// @brief  Load the algo lib and map function pointers
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult LoadLib();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AllocIonBuffer
    ///
    /// @brief  Allocate ion buffer
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VOID* AllocIonBuffer(size_t size, int* fd, INT32 *handle);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FullfillIdenticalMesh
    ///
    /// @brief  Allocate ion buffer
    ///
    /// @return CDKResultSuccess if success or appropriate error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VOID FullfillIdenticalMesh(float* mesh, float total_margin, //mention all the params in comment
            unsigned int input_width, unsigned int input_height,
            unsigned int output_width, unsigned int output_height,
            unsigned int mesh_num_x, unsigned int mesh_num_y);



    CHIHANDLE           m_hChiSession;  ///< The Chi session handle
    UINT32              m_nodeId;       ///< The node's Id
    UINT32              m_nodeCaps;     ///< The selected node caps
    CHINODEFLAGS        m_nodeFlags;    ///< Node flags
    CHINODEIMAGEFORMAT  m_format;       ///< The selected format

    UINT64              m_processedFrame;   ///< The count for processed frame
    BOOL                m_isGPUStatsEnable; ///< GPU enablement

    /// @brief Shdr capabilities, to be filled by shdr lib
        typedef struct ShdrNodeCaps
    {
        FLOAT api_version;
        FLOAT lib_version;
        INT32 process_time; /* library processing time in ms */
        UINT32 width;
        UINT32 height;
        UINT32 row_pitch; /* required for ION buffer allocation*/
        UINT32 buf_padding;
    } SHDRNODECAPS;

    /** frameproc_comp_t
     *   @handle: Pinter to library handler
     *   @caps: Shdr capabilities.
     *   @frame_ops: Pointer to bayerproc frame ops
     *   @init_config: Init config structure.
     *   @input_buf: Input buffers holder array.
     *   @output_buf: Output buffers holder array.
     *   @input_sensor_gain: Array of input sensor gains.
     *   @input_exposure_time: Array of input exposure times.
     *   @input_param: Input param holder.
     *   @input_fd_list: Input fd array.
     *   @buf_3a: Buf 3a struct holder.
     *
     *
     **/
    /// @brief Shdr context structure
    typedef struct ShdrNodeContext
    {
        VOID *handle;
        SHDRNODECAPS caps;
        INT32 (*callback)(VOID *user_data);
        SHDR3INITCONFIG init_config;
        SHDR3TUNING tuning;
        SHDR3INPUTPARAM input_param;
        SHDR3WARPMESH warp_mesh[MAX_REQ][MAX_EXP_ENTRIES];
        SHDR3STATSINFO *stats_info;
    } SHDRNODECONTEXT;

    /** shdr_callback_t
     *   @callback: Pointer to callback func
     *   @userdata: pointer to userdata.
     *   @p_shdr_ctx: pointer to Shdr context structure
     *   Shdr callback structure
     **/
    typedef struct ShdrCallback
    {
        INT (*callback)(VOID *user_data);
        VOID *user_data_Node;
        VOID *user_data_frame;
        SHDRNODECONTEXT* p_shdr_ctx;
    } SHDRCALLBACK;

    ///< Typedefs for SHDR interface functions

    /******************************************************/

    /** shdr_lib_init
    *    @lib_handle: shdr library handle
    *    @params: configuration params for shdr
    *
    * Initializes shdr library
    *
    * return: SHDR_SUCCESS on success, SHDR_FAIL when fails
    **/
    typedef INT(*SHDRLIBINIT) (VOID **handle, SHDR3INITCONFIG *params, SHDR3INPUTPARAM *input_param);


    /** shdr_lib_deinit
    *    @handle: shdr lib handle
    *
    * Deinitializes shdr module
    *
    * return: nothing
    **/
    typedef INT(*SHDRLIBDEINIT) (VOID *handle);


    /** shdr_lib_process
    *    @shdr_handle: handle to shdr lib
    *   @shdr_input_param_t: pointer to shdr input params
    *   @output_fd: fd handle to output frame
    *   @shdr_out_params: output pointer to shdr 3a stats
    *   @ user_data: any client data to be passed back from
    shdr_completion_callback
    *   @shdr_completion_callback: function pointer to be called back upon GPU shdr
    processing
    *   completion for non-blocking shdr_lib_process support use this for best
    performance/watt
    *   setting shdr_completion_callback to NULL will cause shdr_lib_process to
    block until GPU
    *   shdr processing is complete.

    * main shdr function to process image data
    *
    * return: SHDR_SUCCESS on success, SHDR_FAIL when fails
    **/
    typedef INT(*SHDRLIBPROCESS) (VOID *handle, SHDR3INPUTPARAM *input_param,
    VOID (*shdr_completion_callback)(VOID *handle, SHDR3INPUTPARAM *input_param, INT ihist_ready, VOID *user_data),
    VOID (*aec_stats_ready_callback)(SHDR3STATSINFO *stats, VOID *user_data),
    VOID *user_data);

    CHILIBRARYHANDLE        m_hShdrLib;                               ///< Handle for Shdr library.
    SHDRLIBINIT             m_shdr_lib_init;                          ///< Function pointer for eis2_initialize
    SHDRLIBDEINIT           m_shdr_lib_deinit;                        ///< Function pointer
    SHDRLIBPROCESS          m_shdr_lib_process;                       ///< Function pointer
    BOOL                    m_IsDefog_Enabled;                        ///< defog enablement
    SHDR3TUNINGDE           m_tuning_de_param;                        ///< de tuning params
    SHDRNODECONTEXT*        m_p_shdr_ctx;                             ///< shdr lib context
    UINT32                  m_AECDataVendorTagBase;                   ///< AEC stats data
    UINT32                  m_AWBRGainDataVendorTagBase;              ///< AWB R Channel data
    UINT32                  m_DEDataVendorTagBase;                    ///< DE stats generated in Defog
    UINT32                  m_DefogEnableVendorTagBase;               ///< Defog Enable or not
    UINT64                  m_DefogEnableStartFrameNum;               ///< Defog Enable or not
    UINT32                  m_AWBGGainDataVendorTagBase;              ///< AWB G Channel data
    UINT32                  m_AWBBGainDataVendorTagBase;              ///< AWB B Channel data
    UINT32                  m_frameCountVendorTagBase;                ///< Lib FrameNum
    INT32                   m_IonFd;                                  ///< Ion FD
    INT32                   warp_buf_handle[MAX_REQ][MAX_EXP_ENTRIES];///< warp buffer handle
    BOOL                    m_InterpolationResourceAllocated;         ///< Resource Allocated for Interpolation
    INT32                   shdrAlgoType;                             ///< shdr algo type :  2 frame or 3 frame
    UINT32                  sensorMode;
    BOOL                    enableLiveTuning;                         ////< Enable Live Tuning for SHDR
    UINT32                  m_ShdrGpuVendorTagBase;                   ////< GPU stats Data

    CDKResult FillSingleBuffer(CHINODEBUFFERHANDLE phBuffer, SHDR3BUFFER *buffer);
    CDKResult FillBufferInfo(CHINODEPROCESSREQUESTINFO* pProcessRequestInfo,
        SHDR3INPUTPARAM *input_param);
    VOID FillIdenticalMesh(FLOAT* mesh, FLOAT total_margin,
        UINT input_width, UINT input_height,
        UINT mesh_num_x, UINT mesh_num_y);
    BOOL IsDefogEnabled(UINT64 framenum);

    VOID ReleaseWarpMem();
    VOID FreeIonAlloc(size_t size, void* vaddr, INT handle);
    VOID InitializeWarpMesh(UINT numExposure);

    VOID FillBuffer(SHDR3BUFFER* pLibInputBuffer, CHINODEBUFFERHANDLE pInputBuffer);
    VOID ReadTuningParams(TuningSetManager* pTuningManager, SHDR3TUNING *pTuning);
    VOID updateShdrTuning(shdr3_tuning_t *pTuning);
    UINT                        m_SHDR_beTag;                          ///< SHDR be Tag
    UINT                        m_SHDR_histTag;                        ///< SHDR hist Tag
    UINT                        m_SHDR_configTag;                      ///< SHDR congig Tag
    UINT                        m_SHDR_sensorMode;                      ///< Sensor Mode tag

public:
    static VOID ShdrCompletionCallback(VOID *handle, SHDR3INPUTPARAM *input_param, INT ihist_ready, VOID *user_data);
    UINT32 frameNum[MaxRequests];
};
#endif // CAMXCHINODEYUVSHDR_H
