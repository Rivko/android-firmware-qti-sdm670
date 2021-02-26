////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxchinodesvhdr.h
/// @brief Chi node for DummyShdr
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXCHINODEDummyShdr_H
#define CAMXCHINODEDummyShdr_H

#define MAX_EXP_ENTRIES 4
#define MAX_INPUT_HDR_FRAMES 2
#define MAX_OUTPUT_HDR_FRAMES 1
#define MAX_SHDR_BG_STATS_SIZE 1024
#define MAX_SHDR_BHIST_STATS_SIZE 256
#define BAYERPROC_MAX_USED_BUFS 16
#define SHDR_MAX_NUM_BUFFERS 32
#define SHDR_MAX_INPUT_FRAMES 2

#include "chinode.h"
#include "camxchinodeutil.h"

static const UINT32 MaxRequests   = 64; ///< Max Requests


// NOWHINE FILE NC004c: Things outside the Camx namespace should be prefixed with Camx/CSL

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Chi node structure for Chi interface.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ChiDummyShdrNode
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
        /// Update tuning data
        ///
        /// @brief Update tuning daa when live tuning is enabled and tuning data is changed
        ///
        /// @return CDKResultSuccess if success or appropriate error code.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID UpdateTuningData();

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

   FLOAT Calc_Gtm_Gamma(float expRatio);

   VOID Autoltm2_Histogram_Matching(
        float* cdf2,
        float* hist,
        int hist_size,
        float* gain_lut,
        float* b8bit,
        float* gain_max,
        float gain_limit);

   VOID Autoltm2_Gaussian_CDF_Gen(
        float* cdf,
        int hist_size,
        int target_luma,
        float sigma);

   VOID Autoltm2_Linear_CDF_Gen(
         float* cdf,
         int hist_size,
         int target_luma,
         float sigma);

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

    typedef struct ShdrStatsInfo
    {
        int frame_id;
        int num_grid_w;
        int num_grid_h;
        float y_avg[MAX_SHDR_BG_STATS_SIZE];
        float bin[MAX_SHDR_BHIST_STATS_SIZE];
    } SHDRSTATSINFO;

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
        SHDRSTATSINFO *stats,
        VOID * user_data);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChiDummyShdrNode
    ///
    /// @brief  Constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChiDummyShdrNode();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~ChiDummyShdrNode
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~ChiDummyShdrNode();


private:
    ChiDummyShdrNode(const ChiDummyShdrNode&) = delete;               ///< Disallow the copy constructor
    ChiDummyShdrNode& operator=(const ChiDummyShdrNode&) = delete;    ///< Disallow assignment operator

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

    ParsedHDRBEStatsOutput* m_bestats;
    ParsedBHistStatsOutput* m_bhist;
    ISPAWBBGStatsConfig*    m_bgbeconfig;

    INT32 m_inputWidth;
    INT32 m_inputHeight;
    INT32 m_outputWidth;
    INT32 m_outputHeight;
    INT32 m_bitsperpixel;

    typedef struct ShdrBuffer
    {
        int32_t width;
        int32_t height;
        int32_t stride;
        uint32_t buf_size;
        uint32_t offset;
        uint8_t *buf;
        int32_t fd;
    } SHDRBUFFER;

    SHDRBUFFER          m_input_buff;       ///< Input buffer for this request
    SHDRBUFFER          m_output_buff;       ///< Output buffer for this request


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

    /**** shdr supported sensor layout formats ***/
    typedef enum ShdrSensorLayout
    {
        SHDR_TWO_SEPARATE_FRAMES = 0,
        SHDR_INTERLEAVED_WITH_STATUS = 1,
        SHDR_INTERLEAVED_NO_STATUS = 2,
        SHDR_INTERLEAVED_NO_STATUS_2LINES = 3,
        SHDR_INTERLEAVED_NO_STATUS_2LINES_NO_BLANK = 4,
        SHDR_INTERLEAVED_NO_STATUS_1LINES_NO_BLANK = 5
    } SHDRSENSORLAYOUT;

    /**** shdr supported bayer input formats ***/
    typedef enum ShdrInputBayerFormat
    {
        SHDR_BAYER_INPUT_PLAIN16 = 0,
        SHDR_BAYER_INPUT_MIPI10 = 1,
        SHDR_BAYER_INPUT_MIPI12 = 2,
        SHDR_BAYER_INPUT_MAX = 0xFF
    } SHDRINPUTBAYERFORMAT;

    /**** shdr supported bayer output formats ***/
    typedef enum ShdrOutputBayerFormat
    {
        SHDR_BAYER_OUTPUT_PLAIN16 = 0,
        SHDR_BAYER_OUTPUT_MIPI10 = 1,
        SHDR_BAYER_OUTPUT_MIPI12 = 2,
        SHDR_BAYER_OUTPUT_MIPI14 = 3,
        SHDR_BAYER_OUTPUT_MAX = 0xFF
    } SHDROUTPUTBAYERFORMAT;

    /**** shdr supported bayer pattern ***/
    typedef enum ShdrBayerPattern
    {
        SHDR_BAYER_GRBG = 0,
        SHDR_BAYER_RGGB = 1,
        SHDR_BAYER_BGGR = 2,
        SHDR_BAYER_GBRG = 3,
        SHDR_BAYER_MAX = 0xFF
    } SHDRBAYERPATTERN;

    typedef struct ShdrTuningParams
    {
        int ADC_BIT_DEPTH;
        int start_offset;
        int SVHDR_ADDITIONAL_LATENCY_Bayer_Proc;
        float EXPOSURE_RATIO_REAL[MAX_EXP_ENTRIES];
        float EXPOSURE_RATIO_SENSOR[MAX_EXP_ENTRIES];
        int TM_OUT_BIT_DEPTH;
        float BLACK_LEVEL;
        float BAYER_GTM_GAMMA;
        int HDR_DARK_N1;
        float HDR_DARK_N2_MINUS_N1_NORMALIZATION_FACTOR;
        int HDR_DARK_N2;
        float HDR_DARK_N3_MINUS_N2_NORMALIZATION_FACTOR;
        int HDR_DARK_N3;
        float HDR_DARK_N4_MINUS_N3_NORMALIZATION_FACTOR;
        int HDR_MAX_WEIGHT;
        float TM_GAIN;
        float ltm_max_gain;
        float ltm_comp_target;
        float ltm_inverse_tone_perc;
    } SHDRTUNINGPARAMS;

    typedef struct ShdrAutoLtmTuningParams
    {
        int   flat_hist_en;
        float max_gain_limit;
        float gsn_luma_target;
        float gsn_sigma;
        float k2_8bit_pre_weight;
    } SHDRAUTOLTMTUNINGPARAMS;

    /**** shdr performance voting levels for DCVS ***/
    typedef enum ShdrPerfHint
    {
        SHDR_PERF_HINT_LOW = 0,
        SHDR_PERF_HINT_NORMAL = 1,
        SHDR_PERF_HINT_HIGH = 2
    } SHDRPERFHINT;

    /*** shdr init parameters   ***/
    typedef struct ShdrInitConfig
    {
        SHDRSENSORLAYOUT sensor_layout;
        SHDRINPUTBAYERFORMAT input_bayer_format;
        SHDROUTPUTBAYERFORMAT output_bayer_format;
        SHDRBAYERPATTERN bayer_pattern;  // GRBG, RGGB, BGGR, GBRG
        SHDRTUNINGPARAMS tuning;
        int32_t input_buf_count;
        int32_t output_buf_count;
        SHDRBUFFER *input_buf;
        SHDRBUFFER *output_buf;
        SHDRPERFHINT perf_hint;  // use SHDR_PERF_HINT_NORMAL for best power
                                   // and performance trade off
        uint32_t enable_deinterlace;
        int32_t gpu_ltm_en;
        int32_t
        num_gpu_passes;  // Use 1 for best power(needs latest OpenCL driver: use
                       // build
                       // MSM8976.LA.1.0-00265-MSM8976.64.PM8952.SLD.GWL.INT-1
                       // or later), 2 for best performance.
        int32_t num_exposure;
        int32_t offset[MAX_EXP_ENTRIES];
        uint32_t src_stride;
        uint32_t dst_stride;
        uint32_t autoltm2_en;
        float gsn_luma_target;
        float gsn_sigma;
        float k2_8bit_pre_weight;
        uint32_t flat_hist_en;
    } SHDRINITCONFIG;

    /****  shdr process input params structure ***/
    typedef struct ShdrInputParam
    {
        int32_t input_buf_count;
        int32_t *input_fd_list;
        float pregain_gb;
        float pregain_gr;
        int32_t offset[MAX_EXP_ENTRIES];
        int32_t num_exposure;
        SHDRTUNINGPARAMS *p_tuning;
        SHDRSTATSINFO *stats_info;
    } SHDRINPUTPARAM;

    /**** shdr process output params ***/
    typedef struct ShdrOutputParam
    {
        int32_t data_size_3a;
        void *buf_3a;
        void *hdr_buf;
    } SHDROUTPUTPARAM;

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
        // img_frame_ops_t frame_ops;
        SHDRINITCONFIG init_config;
        SHDRBUFFER input_buf[SHDR_MAX_NUM_BUFFERS];
        SHDRBUFFER output_buf[SHDR_MAX_NUM_BUFFERS];
        SHDRINPUTPARAM input_param;
        INT32 input_fd_list[SHDR_MAX_INPUT_FRAMES];
        SHDROUTPUTPARAM buf_3a;
        INT32 offset[SHDR_MAX_INPUT_FRAMES];
        SHDRTUNINGPARAMS tuning;
        SHDRSTATSINFO stats_info;
        SHDRAUTOLTMTUNINGPARAMS autoltm2_param;
        float g_shdr_k2_8bit;
        bool autoltm2_en;
        float cdf[MAX_SHDR_BHIST_STATS_SIZE];
        float gain_lut[MAX_SHDR_BHIST_STATS_SIZE];
        float hist[MAX_SHDR_BHIST_STATS_SIZE];
        INT32 (*callback)(void *user_data);
    } SHDRNODECONTEXT;

    /** shdr_callback_t
     *   @callback: Pointer to callback func
     *   @userdata: pointer to userdata.
     *   @p_shdr_ctx: pointer to Shdr context structure
     *   Shdr callback structure
     **/
    typedef struct ShdrCallback
    {
        int (*callback)(void *user_data);
        void *user_data_Node;
        void *user_data_frame;
        SHDRNODECONTEXT* p_shdr_ctx;
    } SHDRCALLBACK;

    /**** return status from shdr library interface ***/
    typedef enum ShdrStatus
    {
        SHDR_ERR_INVALID_INPUT = -3,
        SHDR_ERR_NOT_SUPPORTED = -2,
        SHDR_ERR_NO_MEMORY = -1,
        SHDR_FAIL = 0,
        SHDR_SUCCESS = 1,
        SHDR_ALREADY_MAPPED = 2,
        SHDR_STATUS_MAX = 0xFF
    } SHDRSTATUS;

    /**** shdr de-interleave input params ****/
    typedef struct ShdrDeinterleaveInfo
    {
        int32_t width;
        /* to be filled */

    } SHDR_DEINTERLEAVEINFO;



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
    typedef int32_t(*SHDRLIBINIT) (void **handle, SHDRINITCONFIG *params);


    /** shdr_lib_get_caps
    *    @caps: query capabilities data
    *
    * shdr lib fills it's capability and requirements
    *
    * return: SHDR_SUCCESS on success, SHDR_FAIL when fails
    **/
    typedef int32_t(*SHDRLIBGETCAPS) (SHDRNODECAPS *caps);

    /** shdr_map_buffer
    *    @handle: shdr library handle
    *    @buffer: buffer descriptor
    *    @input: true if buffer is input and false if buffer is output
    *
    * Maps buffer
    *
    * return: SHDR_SUCCESS on success, SHDR_FAIL when fails
    **/
    typedef int32_t(*SHDRLIBMAPBUFFER) (void *handle, SHDRBUFFER *buffer, BOOL input);

    /** shdr_lib_deinit
    *    @handle: shdr lib handle
    *
    * Deinitializes shdr module
    *
    * return: nothing
    **/
    typedef int32_t(*SHDRLIBDEINIT) (void *handle);


    /** shdr_lib_process
    *	 @shdr_handle: handle to shdr lib
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
    typedef int32_t(*SHDRLIBPROCESS) (void *handle, SHDRINPUTPARAM *input_param, int32_t output_fd,
        SHDROUTPUTPARAM *buf_3a,
        void(*shdr_completion_callback)(
            void *handle, SHDRINPUTPARAM *input_param, int32_t output_fd,
            SHDROUTPUTPARAM *buf_3a, void *user_data),
        VOID (*aec_stats_ready_callback)(SHDRSTATSINFO *stats, VOID *user_data),
        void *user_data);

    VOID Shdr_Autoltm2_Unity_Gain(
        float* bhist,
        int bhist_size,
        SHDRAUTOLTMTUNINGPARAMS* autoltm2,
        float* k2_8bit);

    CHILIBRARYHANDLE        m_hShdrLib;                               ///< Handle for Shdr library.
    SHDRLIBINIT             m_shdr_lib_init;                          ///< Function pointer for eis2_initialize
    SHDRLIBDEINIT           m_shdr_lib_deinit;                        ///< Function pointer
    SHDRLIBGETCAPS          m_shdr_lib_get_caps;                      ///< Function pointer
    SHDRLIBMAPBUFFER        m_shdr_lib_map_buffer;                    ///< Function pointer
    SHDRLIBPROCESS          m_shdr_lib_process;                       ///< Function pointer

    SHDRNODECONTEXT*        m_p_shdr_ctx;                             ///< p_shdr_ctx
    UINT32                  m_AECDataVendorTagBase;
    UINT32                  m_AWBRGainDataVendorTagBase;
    UINT32                  m_AWBGGainDataVendorTagBase;
    UINT32                  m_AWBBGainDataVendorTagBase;
    UINT32                  m_frameCountVendorTagBase;
    UINT32                  m_tuningEnabledVendorTagBase;
    float                   **GTM_GAMMA;
    int                     num_gamma_range;
    int                     gamma_entries;

public:
    static void ShdrCompletionCallback(void *handle,
        SHDRINPUTPARAM *input_param, int32_t output_fd,
        SHDROUTPUTPARAM *buf_3a, void *user_data);
    UINT32 frameNum[MaxRequests];
};
#endif // CAMXCHINODEDummyShdr_H
