/*==========================================================================
ELite Source File

This file implements a generic topology which uses APPI compliant modules.

Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/28/11   ss      Introducing PROCESS_CHECK for Resampler and Downmix (APPI Rev D)
01/12/11   ss      Delay accumulation mechanism for EOS, so that all the
                   intermediate buffers are flushed out before processing EOS
10/19/10   DG      Created file.
=========================================================================== */

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "audproc_topohandler.h"
#include "Elite_CAPI_V2.h"
#include "audproc_topo_bg_thread.h"
#include <stddef.h>
#include "audpp_util.h"
#include "adsp_mtmx_strtr_api.h"
#include "adsp_adm_api.h"
#include "adsp_asm_api.h"
#include "adsp_private_api.h"
#include "audio_basic_op_ext.h"
#include "capi_v2_soft_vol.h"
#include "capi_v2_library_factory.h"
#include "capi_v2_adsp_error_code_converter.h"
#include "Elite_intf_extns_change_media_fmt.h"
#include "Elite_fwk_extns_ecns.h"
#include "adsp_vparams_api.h"

/*---------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))

#define IS_ALIGN_4_BYTE(a) (!(uint32_t(a) & 0x3))

// internal buffer size would be (2 * output frame size) for a given PP service.
// this will ensure while we send out one frame of data to service output buffer,
// another frame size of free space is available for the topology to generate
// data. This data would be worst case buffered up data.
// During steady state buffered data can vary between 0 to one output frame size

#define INTERNAL_BUF_FACTOR 2

static const uint32_t BYTES_PER_SAMPLE_TWO = 2;
static const uint32_t BYTES_PER_SAMPLE_FOUR = 4;
//static const uint32_t NUM_CHANNELS_STEREO = 2;
static const uint32_t TOPO_MAX_IN_STRMS = 2; // This should match AUDPP_MAX_INPUT_PORTS in audproc_comdef.h
static const uint32_t TOPO_PRIMARY_STRM = 0;
static const uint32_t TOPO_FAR_STRM = 1;
static const uint32_t TOPO_INVALID_STRM = 0xFF;

#ifndef LOG_ADSP_VCP_VOICEPROC_RTM_C
#define LOG_ADSP_VCP_VOICEPROC_RTM_C  0x184B
#endif

#ifndef DBG_TOPO_HANDLER
//#define DBG_TOPO_HANDLER
#endif


static const uint32_t far_topo_modules_list[] = {AUDPROC_MODULE_ID_MFC_FAR,AUDPROC_MODULE_ID_SAMPLESLIP_FAR};


struct topo_media_fmt_t;
struct topo_buf_t;
struct topo_graph_t;
struct event_handle_t;
struct topo_media_event_t;
struct topo_port_index_t;
struct module_input_port_t;
struct module_output_port_t;
struct _module_info;
struct topo_buf_manager;
struct topo_struct;

// Define the only media format type supported.
struct topo_media_fmt_t
{
    capi_v2_data_format_header_t h;
    capi_v2_standard_data_format_t f;
};

static const topo_media_fmt_t invalid_media_format = {
        //.h
        {
                CAPI_V2_FIXED_POINT
        },
        //.f
        {
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_DATA_FORMAT_INVALID_VAL,
                CAPI_V2_INVALID_INTERLEAVING,
                { 0 }
        }
};

static const AdspAudioMediaFormatInfo_t invalid_svc_media_format = {
      0,
      0,
      0,
      0,
      0,
      { 0 },
      0
};

static const capi_v2_buf_t empty_buf = {
      NULL, 0, 0
};

static const capi_v2_stream_data_t init_stream_data = {
      { 0, 0, 0, 0, 0, 0, 0},
      0,
      NULL,
      0
};
// Buffer structure and its methods
struct topo_buf_t
{

    capi_v2_stream_flags_t flags;
    int64_t timestamp;
    int8_t *buf_ptrs[CAPI_V2_MAX_CHANNELS];
    uint32_t num_bufs;
    uint32_t size_per_buffer;
    uint32_t valid_data_start; // Index of the first valid byte
    uint32_t valid_data_end;   // Index just beyond the last valid byte
    uint32_t bytes_to_samples_factor; // Num_samples = num_bytes / bytes_to_samples_factor
};
static void topo_buffer_initialize(topo_buf_t *buf_ptr);
static bool_t topo_buffer_is_buf_empty(const topo_buf_t *buf_ptr);
static bool_t topo_buffer_is_buf_full(const topo_buf_t *buf_ptr);
static void topo_buffer_convert_from_ext_input_buffer(AudPP_BufInfo_t *ext_buf_ptr, topo_buf_t *int_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr);
static void topo_buffer_convert_from_ext_output_buffer(AudPP_BufInfo_t *ext_buf_ptr, topo_buf_t *int_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr);
static void topo_buffer_update_external_input_buf_sizes(AudPP_BufInfo_t *ext_buf_ptr, topo_buf_t *int_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr);
static void topo_buffer_update_external_output_buf_sizes(AudPP_BufInfo_t *ext_buf_ptr, topo_buf_t *int_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr);
static uint32_t topo_buffer_bytes_to_samples(const topo_buf_t *buf_ptr, uint32_t bytes);
static uint32_t topo_buffer_samples_to_bytes(const topo_buf_t *buf_ptr, uint32_t samples);
static ADSPResult topo_init_single_module(topo_struct *topo_ptr, const uint32_t index, const audproc_module_info_t *module_info_ptr, const adsp_amdb_module_handle_info_t *handle_info_ptr, AudPP_AudProcType pp_type, uint32_t *stack_size_ptr, bool_t initialize_with_compressed_format);
static ADSPResult common_topo_init_single_module(topo_struct *topo_ptr, const uint32_t index, avcs_module_instance_info_t *module_info_ptr, const adsp_amdb_module_handle_info_t *handle_info_ptr, AudPP_AudProcType pp_type, uint32_t *stack_size_ptr, bool_t initialize_with_compressed_format);
static ADSPResult topo_create_capi_v2_module(topo_struct *topo_ptr, adsp_amdb_capi_v2_t *handle, const uint32_t index, const audproc_module_info_t *module_info_ptr, AudPP_AudProcType pp_type, uint32_t *stack_size_ptr, capi_v2_proplist_t *init_proplist_ptr);
static ADSPResult common_topo_create_capi_v2_module(topo_struct *topo_ptr, adsp_amdb_capi_v2_t *handle, const uint32_t index, avcs_module_instance_info_t *module_info_ptr, AudPP_AudProcType pp_type, uint32_t *stack_size_ptr, capi_v2_proplist_t *init_proplist_ptr);

static void topo_check_ec_module_presence(topo_struct *topo_ptr,adsp_amdb_module_handle_info_t *handle_info_ptr,uint32_t num_modules,uint32_t *ec_module_index);

static bool_t is_sampling_rate_fractional(uint32_t sampling_rate);
static void topo_update_chain_port_data_threshold(topo_struct *topo_ptr, uint32_t port_id);
static bool_t topo_is_data_threshold_valid(uint32_t data_threshold, topo_media_event_t *port_media_event);
static bool_t is_amdb_module_handle_valid(const adsp_amdb_module_handle_info_t *amdb_module_handle_ptr);

//internal buffer related utilities

struct topo_internal_buf_t
{
    int8_t *pBuf;
    int8_t *read_ptr;
    int8_t *write_ptr;
    int8_t *end_ptr;
    uint32_t buf_size;  // Total internal bufsize in bytes
    uint32_t offset_in_samples_read; // This value is per channel
    uint32_t offset_in_samples_write; // This value is per channel
    uint32_t size_in_samples; // This value is per channel
    capi_v2_stream_flags_t flags;
    int64_t timestamp;
};

static ADSPResult topo_internal_buf_create_internal_buffer(topo_struct *topo_ptr);
static void topo_internal_buf_destroy_internal_buffer(topo_struct *topo_ptr);
static ADSPResult topo_internal_buf_copy_to_ext_output_buf(AudPP_BufInfo_t *ext_out_buf_ptr, topo_struct *topo_ptr, AdspAudioMediaFormatInfo_t *old_media_format);
static void topo_internal_buf_update_offsets(topo_internal_buf_t *internal_buf_ptr, topo_buf_t *out_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr);
static void topo_buffer_convert_from_int_output_buffer(topo_internal_buf_t *internal_buf_ptr, topo_buf_t *out_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr);
static uint32_t topo_get_unit_frames_in_module_input_buffer(topo_struct *topo_ptr, uint32_t module_index, uint32_t port_id);

static bool_t topo_is_output_unit_frames_changed(topo_struct *topo_ptr);
static void topo_update_module_output_buffers(topo_struct *topo_ptr, uint32_t module_index);

static const uint32_t TOPO_INVALID_MODULE_INDEX = UINT32_MAX;
// Module index of very first module in second input port
static const uint32_t TOPO_INVALID_MODULE_INDEX_1 = TOPO_INVALID_MODULE_INDEX-1;

// Topology graph structure and its methods
static const uint8_t GRAPH_MAIN_INPUT_PORT_INDEX = 0xC0;
static const uint8_t GRAPH_MAIN_OUTPUT_PORT_INDEX = 0xE0;

struct topo_def_t
{
   uint32_t topology_id;
   uint32_t num_modules;
   avcs_module_instance_info_t *module_id_list;
};

struct topo_graph_t
{
   uint32_t num_modules;

   struct topo_graph_port_t
   {
      uint8_t module_index;
      uint8_t port_index;
   };

   struct topo_graph_node_t
   {
      uint32_t module_id;
      uint32_t instance_id;
      uint8_t num_inputs;
      topo_graph_port_t *inputs_array;
      uint8_t num_outputs;
      topo_graph_port_t *outputs_array;
   } *nodes;
   topo_graph_port_t *connections_array;
};

typedef struct ec_capi_module_info_t
{
   ecns_monitoring_capabilities_t  ecns_monitoring_info;              //ECNS RTM capabilities.
   ecns_stt_capabilities_t         ecns_stt_info;                     //ECNS STT capabilities.
   ecns_vp3_capabilities_t         ecns_vp3_info;
   ecns_rate_matching_capabilities_t ecns_rate_matching_info;         //ECNS rate matching capabilities
   ecns_output_capabilities_t      ecns_output_capabilities;
   ecns_speech_act_det_capabilities_t ecns_speech_act_det_capabilities;
   ecns_multichannel_ref_capabilities_t ecns_multichannel_ref_info;
}ec_capabilities_t;

struct topo_ec_fwk_extn_t
{
    uint8_t is_far_port_active;
    uint8_t module_index;
    uint16_t num_far_modules;
    uint32_t module_id;
    uint32_t instance_id;
    ec_capabilities_t ec_capabilities;
    bool_t *is_far_moudle_in_chain;
};




static void topo_graph_init(topo_graph_t *topo_graph_ptr);
static void topo_graph_deinit(topo_graph_t *topo_graph_ptr);
static ADSPResult topo_graph_set_graph(topo_graph_t *topo_graph_ptr, const topo_def_t *topo_def_ptr);
static ADSPResult common_topo_graph_set_graph(topo_graph_t *topo_graph_ptr, topo_def_t *topo_def_ptr);
static ADSPResult common_topo_graph_set_ec_graph(topo_graph_t *topo_graph_ptr, topo_def_t *topo_def_ptr,topo_ec_fwk_extn_t *ec_fwk_info);
static void topo_graph_free_graph(topo_graph_t *topo_graph_ptr);
static topo_port_index_t topo_graph_get_prev_module(topo_graph_t *topo_graph_ptr, topo_port_index_t current_index);
static topo_port_index_t topo_graph_get_next_module(topo_graph_t *topo_graph_ptr, topo_port_index_t current_index);
static topo_port_index_t topo_graph_get_main_input(topo_graph_t *topo_graph_ptr,uint32_t stream_index);
static topo_port_index_t topo_graph_get_main_output(topo_graph_t *topo_graph_ptr);
static uint32_t topo_graph_num_modules(const topo_graph_t *topo_graph_ptr);
#ifdef TST_TOPO_LAYER
static void topo_graph_print_graph(const topo_graph_t *topo_graph_ptr, uint16_t obj_id);
#endif
static void topo_graph_print_linear_topo(const topo_graph_t *topo_graph_ptr, uint16_t obj_id,uint32_t start_module_index);
static capi_v2_port_num_info_t topo_graph_get_port_num_info(const topo_graph_t *topo_graph_ptr, uint32_t module_index);
static void topo_send_algo_reset(topo_struct *topo_ptr, uint32_t module_idx);

// Event handle structure and its methods
struct event_handle_t
{
   topo_struct *topo_struct_ptr;
   uint32_t module_index;
};
static void topo_event_initialize_handle(topo_struct *topo_ptr, uint32_t module_index);
static uint32_t topo_event_get_module_index(void *handle);
static topo_struct *topo_event_get_struct_ptr(void *handle);


struct topo_media_event_t
{
   bool_t event_is_pending;
   topo_media_fmt_t event;
};
static void topo_set_media_event(topo_media_event_t *downstream_event_ptr, const topo_media_fmt_t *format_to_be_set);
// Module port structure and its methods
// Module port structure and its methods
struct topo_port_index_t
{
   uint32_t module_index;
   uint32_t port_index;
   enum port_type
{
      INPUT_PORT,
      OUTPUT_PORT
   } type;
};
static module_input_port_t *topo_in_port_from_index(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr);
static module_output_port_t *topo_out_port_from_index(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr);
#define ASSERT_PORTS_EQUAL(a, b) QURT_ELITE_ASSERT(((a)->module_index == (b)->module_index) && ((a)->port_index == (b)->port_index) &&((a)->type == (b)->type))


// Module input port structure and its methods
struct module_input_port_t
{
   topo_port_index_t connected_port;
   topo_media_event_t media_event;
   uint32_t           data_threshold;
   bool_t             is_data_available;
};
static void topo_module_input_port_initialize(module_input_port_t *port_ptr);

// Module output port structure and its methods
struct module_output_port_t
{
   topo_port_index_t connected_port;
   topo_media_event_t media_event;
   topo_buf_t topo_buf;
   uint32_t           data_threshold;

};
static void topo_module_output_port_initialize(module_output_port_t *port_ptr);

// The module structure and its methods
typedef struct _module_info
{
   uint32_t        module_id;
   uint32_t        instance_id;
   bool_t          is_in_place;
   bool_t          requires_data_buffering;
   bool_t          is_enabled;
   bool_t          changes_media_format;
   capi_v2_t*      module_ptr;
   uint32_t        KPPS;
   uint32_t        bandwidth;
   uint32_t        delay_in_us;
   uint32_t        headroom;
#ifdef AUDPPSVC_TEST
   uint64_t        count;  // The very first iteration of processing is ignored
   uint64_t        prof_total_cycles;
   uint64_t        prof_num_samples;
   float           prof_peak_mips;
#endif // AUDPPSVC_TEST

   uint32_t num_input_ports;
   module_input_port_t *input_ports_ptr;
   uint32_t num_output_ports;
   module_output_port_t *output_ports_ptr;
   void *port_mem;

   bool_t is_in_chain;
   bool_t disabled_till_next_media_event;

   uint32_t num_fwk_extns;
   capi_v2_framework_extension_id_t *fwk_extn_ptr;
   event_handle_t event_handle;
} module_info;

typedef enum topo_intf_extn_idx_t
{
   INTF_EXTN_CHANGES_MEDIA_FORMAT = 0,
   INTF_EXTN_MAX
} topo_intf_extn_idx_t;

typedef struct topo_module_intf_extn_list_t
{
   capi_v2_interface_extns_list_t intf_extn_num;
   capi_v2_interface_extn_desc_t intf_extn_desc[INTF_EXTN_MAX];
} topo_module_intf_extn_list_t;

static void topo_module_query_intf_extn(module_info *module_info_ptr, adsp_amdb_capi_v2_t *handle);
static void topo_module_alloc_and_query_fwk_extn(topo_struct *topo_ptr, adsp_amdb_capi_v2_t *handle,uint32_t module_index);
static void topo_module_process_fwk_extn(topo_struct *topo_ptr, uint32_t num_modules);
static topo_buf_t *topo_module_get_input_buffer(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr);
static topo_buf_t *topo_module_get_output_buffer(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr);
static topo_port_index_t topo_module_next_port(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr);
static topo_port_index_t topo_module_prev_port(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr);
static ADSPResult topo_module_allocate_ports(module_info *module_ptr);
struct topo_buf_manager;
static void topo_module_free_ports(module_info *module_ptr, topo_buf_manager *buf_mgr_ptr);
static bool_t topo_module_is_propagate_for_disabled_module(const module_info *module_ptr);
static bool_t topo_module_is_disablement_possible(const module_info *module_ptr);
static void topo_module_set_pending_input_media(module_info *module_ptr, uint16_t obj_id);
static void topo_module_destroy(module_info *module_struct_ptr, topo_buf_manager *buf_mgr_ptr);

// Buffer manager structure and methods
static const uint32_t TOPO_BUF_MANAGER_MAX_ELEMENTS = 32;
struct topo_buf_manager
{
    uint32_t max_memory_allocated;
    uint32_t current_memory_allocated;

    struct topo_buf_manager_element
    {
        bool_t present;
        uint32_t size;
        int8_t *ptr;
    } bufs[TOPO_BUF_MANAGER_MAX_ELEMENTS];
};
static void topo_buf_manager_init(topo_buf_manager *buf_mgr_ptr);
static void topo_buf_manager_deinit(topo_buf_manager *buf_mgr_ptr);
static ADSPResult topo_buf_manager_get_bufs(topo_buf_manager *buf_mgr_ptr, topo_buf_t *buf_ptr);
static void topo_buf_manager_return_bufs(topo_buf_manager *buf_mgr_ptr, topo_buf_t *buf_ptr);
static void topo_buf_manager_clear_all_bufs(topo_buf_manager *buf_mgr_ptr);
static void topo_set_headroom(topo_struct *topo_ptr);
static void topo_set_ramp_period_to_virtualizer(topo_struct *topo_ptr, const uint32_t index);
static void topo_set_delay_ptr_virtualizer(topo_struct *topo_ptr);

static void topo_set_ec_far_media_format(topo_struct *topo_ptr,const AdspAudioMediaFormatInfo_t *req_media_format);

static void topo_check_empty_last_buffer(topo_struct *topo_ptr);

static uint32_t INVALID_KPPS_VALUE = UINT32_MAX;
static uint32_t INVALID_BANDWIDTH_VALUE = UINT32_MAX;

struct topo_struct
{
   uint16_t        obj_id;
   uint32_t        ouput_buffer_num_unit_frames;  // size of one unit is returned by elite_svc_get_frame_size function
   uint32_t        main_th_ouput_buffer_num_unit_frames;
   uint32_t        compress_frame_size;

   module_info     *module;  // Array of modules (libraries)
   topo_buf_t    main_input_buffer[TOPO_MAX_IN_STRMS];
   topo_buf_t    main_output_buffer;
   topo_port_index_t first_port[TOPO_MAX_IN_STRMS];
   topo_port_index_t last_port;

   bool_t          module_list_locked;
   bool_t          currently_handling_events;

   // Data format related variables
   void *stream_struct_mem;
   uint32_t max_in_streams;
   uint32_t active_in_streams;
   capi_v2_stream_data_t *in_streams;
   capi_v2_stream_data_t **in_stream_ptrs;
   uint32_t max_out_streams;
   capi_v2_stream_data_t *out_streams;
   capi_v2_stream_data_t **out_stream_ptrs;

   // Data format related variables
   bool_t is_eos_set[TOPO_MAX_IN_STRMS];
   bool_t data_format_received[TOPO_MAX_IN_STRMS];
   AdspAudioMediaFormatInfo_t  in_format[TOPO_MAX_IN_STRMS];
   AdspAudioMediaFormatInfo_t  out_format;

   //Internal Buffer for fractional conversion
   bool_t is_input_output_realtime;
   topo_internal_buf_t internal_buf;

   // EOS variables
   AudPP_BufInfo_t eos_buf;

   // Status variables shared externally
   uint32_t KPPS;
   uint32_t bandwidth;
   uint32_t delay;

   uint32_t        topo_headroom;                   // Specifies the current headroom requirement of the topology in milli-bels.
   uint32_t        stack_size;
   uint32_t        rem_delay_samples;

   // Buffer manager
   topo_buf_manager buf_mgr;

   // Client for notification of events
   topo_event_client_t *event_client;

   // TODO : Create generic fwk_extn_info
   topo_ec_fwk_extn_t *ec_extn_info;
   // The topology graph
   topo_graph_t graph;
};

static const uint32_t NUM_US_IN_SEC = 1000000;
static const uint32_t TOPO_KPPS = 1000;
static const uint32_t TOPO_BANDWIDTH = 0; // The upper layer calculates bandwidth for the input and output
static const uint32_t TOPO_BW_FUDGE_FACTOR_NUM = 4; //use fudge factor of 2.0
static const uint32_t TOPO_BW_FUDGE_FACTOR_DEN = 2;
static const uint32_t TOPO_BW_FUDGE_FACTOR_NUM_90_10 = 3; //use fudge factor of 1.5 when any clients vote is 90% of sum.
static const uint32_t TOPO_BW_FUDGE_FACTOR_DEN_90_10 = 2;
static const uint32_t TOPO_BW_FUDGE_FACTOR_THRESHOLD = 90;

/*Returns the index of first instance of a given Module*/
static ADSPResult topo_find_first_module_instance(const topo_struct *topo_ptr,
      const uint32_t module_id,
      uint32_t *index_ptr);

/*Returns the index of next instance of a given Module.*/
static ADSPResult topo_find_next_module_instance(const topo_struct *topo_ptr,
      const uint32_t module_id,
      uint32_t *index_ptr);

/*Returns the index of instance of a given Module-Instance*/
static ADSPResult topo_find_module_instance(const topo_struct *topo_ptr,
      const uint32_t module_id,
      const uint32_t instance_id,
      uint32_t *index_ptr);

static ADSPResult create_delay_buffer(topo_struct* topo_ptr);
static uint32_t topo_convert_us_to_samples(const uint32_t val_in_us, const uint32_t sample_rate);
static uint64_t div_round(const uint64_t x, const uint32_t y);
static bool_t topo_is_compressed(uint32_t bitstream_format);
static bool_t topo_is_primary_in_port(uint32_t port_id);
static uint32_t topo_calculate_q_factor(const AdspAudioMediaFormatInfo_t *media_fmt_ptr);
static void topo_adsp_media_fmt_to_topo(const AdspAudioMediaFormatInfo_t *adsp_media_fmt_ptr, topo_media_fmt_t *topo_media_fmt_ptr);
static void topo_topo_media_fmt_to_adsp(const topo_media_fmt_t *topo_media_fmt_ptr, AdspAudioMediaFormatInfo_t *adsp_media_fmt_ptr);
static void topo_print_media_format(uint16_t obj_id, const topo_media_fmt_t *media_fmt_ptr);
static void topo_print_module_output_media_format(uint16_t obj_id, uint32_t module_id, uint32_t instance_id, const topo_media_fmt_t *media_fmt_ptr);
static void topo_loop_thru_zero_modules(topo_struct *topo_ptr);
static bool_t topo_zero_modules_enabled(const topo_struct *topo_ptr);
static bool_t topo_zero_modules_created(const topo_struct *topo_ptr);
static bool_t topo_module_list_locked(const topo_struct *topo_ptr);
static bool_t topo_module_pending_reconfig(const topo_struct *topo_ptr, uint32_t start_index);
static void topo_lock_module_list(topo_struct *topo_ptr);
static void topo_unlock_module_list(topo_struct *topo_ptr);
static bool_t topo_handle_pending_events(topo_struct *topo_ptr, uint32_t start_module_index, uint32_t port_id);
static void topo_handle_module_enable_events(topo_struct *topo_ptr, uint32_t module_index);
static void topo_add_to_module_list(topo_struct *topo_ptr, topo_port_index_t *module_port_ptr, const topo_port_index_t *prev_port_ptr);
static void topo_remove_from_module_list(topo_struct *topo_ptr, uint32_t module_index);
static void topo_module_setup_output_buffers(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr);
static ADSPResult topo_process_module(topo_struct *topo_ptr, uint32_t module_index);
static ADSPResult topo_process_module_inplace(topo_struct *topo_ptr, uint32_t module_index);
static bool_t topo_inplace_processing_possible(topo_struct *topo_ptr, uint32_t module_index);
static void topo_update_chain_KPPS(topo_struct *topo_ptr);
static void topo_update_chain_bandwidth(topo_struct *topo_ptr);
static void topo_update_chain_delay(topo_struct *topo_ptr);
static void topo_update_chain_headroom(topo_struct *topo_ptr);
static void topo_raise_output_media_type_event(topo_struct *topo_ptr);
static bool_t topo_is_equal_media_format(const topo_media_fmt_t *f1, const topo_media_fmt_t *f2);
static capi_v2_err_t topo_event_callback(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event);
static event_handle_t *topo_event_get_handle(topo_struct *topo_ptr, uint32_t module_index);
static bool_t topo_module_can_process(topo_struct *topo_ptr, uint32_t module_index, uint32_t *required_in_port_index);
static bool_t topo_check_update_module_enablement(topo_struct *topo_ptr, uint32_t module_index);
static bool_t topo_check_update_module_process_state(topo_struct *topo_ptr, uint32_t module_index);
static void topo_destroy_modules(uint32_t num_modules, module_info modules[], topo_buf_manager *buf_mgr_ptr);
static void topo_setup_connections(topo_struct *topo_ptr);
static ADSPResult topo_allocate_stream_structs(topo_struct *topo_ptr);
static void topo_deallocate_stream_structs(topo_struct *topo_ptr);

#ifdef AUDPPSVC_TEST
static float convert_cycles_to_mips(const uint32_t sample_rate, const uint32_t num_samples, const uint64_t cycles);
#endif

/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
static const uint32_t AUDPROC_TOPO_DEFAULT_STACK_SIZE = 7*1024;

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

static ADSPResult topo_create_obj(const uint16_t obj_id, topo_struct **topo_obj_ptr, const uint32_t ouput_buffer_num_unit_frames, topo_event_client_t *event_client, bool_t is_input_output_realtime)
{
   *topo_obj_ptr = NULL;

   // Creating the topology object
   topo_struct *topo_ptr = (topo_struct*)(qurt_elite_memory_malloc(sizeof (topo_struct), QURT_ELITE_HEAP_DEFAULT));
   if(NULL == topo_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to allocate memory for topology structure", obj_id);
      return ADSP_ENOMEMORY;
   }
   // Initialize the topology object variables
   topo_ptr->obj_id = obj_id;
   topo_ptr->ouput_buffer_num_unit_frames = ouput_buffer_num_unit_frames;
   topo_ptr->main_th_ouput_buffer_num_unit_frames = ouput_buffer_num_unit_frames;
   topo_ptr->compress_frame_size = 0;
   topo_ptr->module = NULL;
   for(uint32_t in_port=0;in_port<TOPO_MAX_IN_STRMS;in_port++)
       topo_buffer_initialize(&topo_ptr->main_input_buffer[in_port]);

   topo_buffer_initialize(&topo_ptr->main_output_buffer);

   for(uint32_t port_id=0;port_id<TOPO_MAX_IN_STRMS;port_id++)
   {
       topo_ptr->first_port[port_id].module_index = TOPO_INVALID_MODULE_INDEX;
       topo_ptr->first_port[port_id].port_index = 0;
       topo_ptr->first_port[port_id].type = topo_port_index_t::INPUT_PORT;
   }
   topo_ptr->last_port.module_index = TOPO_INVALID_MODULE_INDEX;
   topo_ptr->last_port.port_index = 0;
   topo_ptr->last_port.type = topo_port_index_t::OUTPUT_PORT;
   topo_ptr->module_list_locked = FALSE;
   topo_ptr->currently_handling_events = FALSE;
   topo_ptr->stream_struct_mem = NULL;
   topo_ptr->max_in_streams = 0;
   topo_ptr->active_in_streams = 0;
   topo_ptr->in_streams = NULL;
   topo_ptr->in_stream_ptrs = NULL;
   topo_ptr->max_out_streams = 0;
   topo_ptr->out_streams = NULL;
   topo_ptr->out_stream_ptrs = NULL;
   topo_ptr->rem_delay_samples = 0;

   for(uint32_t in_port=0;in_port<TOPO_MAX_IN_STRMS;in_port++)
   {
       topo_ptr->data_format_received[in_port] = FALSE;
       topo_ptr->in_format[in_port] = invalid_svc_media_format;
   }

   topo_ptr->out_format = invalid_svc_media_format;
   topo_ptr->eos_buf.pBuf = NULL;
   topo_ptr->eos_buf.sizeInSamples = 0;
   topo_ptr->eos_buf.offsetInSamples = 0;
   topo_ptr->is_input_output_realtime = is_input_output_realtime;
   memset((void *)&topo_ptr->internal_buf, 0 , sizeof(topo_ptr->internal_buf));
   topo_ptr->KPPS = INVALID_KPPS_VALUE;
   topo_ptr->bandwidth = INVALID_BANDWIDTH_VALUE;
   topo_ptr->delay = 0;
   topo_ptr->topo_headroom = 0;
   topo_buf_manager_init(&topo_ptr->buf_mgr);
   topo_ptr->event_client = event_client;
   topo_graph_init(&topo_ptr->graph);

   topo_ptr->ec_extn_info = NULL;
   *topo_obj_ptr = topo_ptr;

   return ADSP_EOK;
}

QURT_ELITE_HEAP_ID topo_get_heap_id(bool_t use_lpa, AudPP_AudProcType pp_type)
{
   QURT_ELITE_HEAP_ID heap_id = QURT_ELITE_HEAP_DEFAULT;
   if (use_lpa)
   {
      switch (pp_type)
      {
      case DYNA_SVC_PP_TYPE_POPP:
         heap_id = PP_SVC_POPP_ALWAYS_ENABLED_ALGS;
         break;
      case DYNA_SVC_PP_TYPE_COPP:
         heap_id = PP_SVC_COPP_ALWAYS_ENABLED_ALGS;
         break;
      default:
         break;
      }
   }
   return heap_id;
}

static void *topo_allocate_module_memory(topo_struct *topo_ptr, const uint32_t malloc_size,
                                             bool_t use_lpa, AudPP_AudProcType pp_type)
{
   QURT_ELITE_HEAP_ID heap_id = topo_get_heap_id(use_lpa, pp_type);
    return qurt_elite_memory_malloc(malloc_size, heap_id);
}

uint32_t topo_get_bytes_per_sample(uint32_t bits_per_sample)
{
   uint32_t bytes_per_sample = 0;

   switch(bits_per_sample)
   {
      case 16:
         bytes_per_sample = BYTES_PER_SAMPLE_TWO;
         break;
      case 24:
      case 32:
         bytes_per_sample = BYTES_PER_SAMPLE_FOUR;
         break;
      default:
         // we should never hit this case
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported bits_per sample %lu", bits_per_sample);
   }
   return bytes_per_sample;
}

static ADSPResult topo_create_capi_v2_module(topo_struct *topo_ptr, adsp_amdb_capi_v2_t *handle, const uint32_t index, const audproc_module_info_t *module_info_ptr, AudPP_AudProcType pp_type, uint32_t *stack_size_ptr, capi_v2_proplist_t *init_proplist_ptr)
{
    ADSPResult result = ADSP_EOK;
    capi_v2_err_t err_code = CAPI_V2_EOK;

   // Query static properties

    // First query for the properties which are not optional.
   capi_v2_init_memory_requirement_t memory_req = { 0 };
   capi_v2_prop_t req_props[] = {
           { CAPI_V2_INIT_MEMORY_REQUIREMENT, { reinterpret_cast<int8_t*>(&memory_req),   0, sizeof(memory_req)   }, { FALSE, FALSE, 0} },
   };
   capi_v2_proplist_t req_proplist = { SIZE_OF_ARRAY(req_props), req_props };
   err_code = adsp_amdb_capi_v2_get_static_properties_f(handle, init_proplist_ptr, &req_proplist);
   if(CAPI_V2_FAILED(err_code))
   {
       MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module with ID 0x%lX failed in getting some required static properties with %d.", topo_ptr->obj_id, module_info_ptr->module_id, static_cast<int>(err_code));
       return capi_v2_err_to_adsp_result(err_code);
   }

   // Query for the optional properties now.
   capi_v2_init_memory_requirement_t dummy_memory_req = { 0 };
   capi_v2_is_inplace_t is_inplace = { FALSE };
   capi_v2_requires_data_buffering_t req_data_buf = { TRUE };
   capi_v2_stack_size_t stack_size = { 0 };
   capi_v2_num_needed_framework_extensions_t num_extensions = { 0 };
   capi_v2_prop_t optional_props[] = {
           { CAPI_V2_IS_INPLACE,              { reinterpret_cast<int8_t*>(&is_inplace),   0, sizeof(is_inplace)   }, { FALSE, FALSE, 0} },
           { CAPI_V2_REQUIRES_DATA_BUFFERING, { reinterpret_cast<int8_t*>(&req_data_buf), 0, sizeof(req_data_buf) }, { FALSE, FALSE, 0} },
           { CAPI_V2_STACK_SIZE,                       { reinterpret_cast<int8_t*>(&stack_size),     0, sizeof(stack_size)       }, { FALSE, FALSE, 0} },
           { CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS,  { reinterpret_cast<int8_t*>(&num_extensions), 0, sizeof(num_extensions)   }, { FALSE, FALSE, 0} },
           // We still need to query for the memory requirement here since amdb returns an error if it does not find it. This value is ignored.
           { CAPI_V2_INIT_MEMORY_REQUIREMENT, { reinterpret_cast<int8_t*>(&dummy_memory_req),   0, sizeof(dummy_memory_req)   }, { FALSE, FALSE, 0} },
   };
   capi_v2_proplist_t optional_proplist = { SIZE_OF_ARRAY(optional_props), optional_props };
   err_code = adsp_amdb_capi_v2_get_static_properties_f(handle, init_proplist_ptr, &optional_proplist);
   if(CAPI_V2_FAILED(err_code))
   {
       MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX Module with ID 0x%lX failed in getting some optional static properties with %d. Using default values.", topo_ptr->obj_id, module_info_ptr->module_id, static_cast<int>(err_code));
   }

   topo_ptr->module[index].requires_data_buffering = req_data_buf.requires_data_buffering;
   if (topo_ptr->module[index].requires_data_buffering)
   {
       topo_ptr->module[index].is_in_place = FALSE;
   }
   else
   {
       topo_ptr->module[index].is_in_place = is_inplace.is_inplace;
   }

   *stack_size_ptr = (*stack_size_ptr > stack_size.size_in_bytes) ? *stack_size_ptr : stack_size.size_in_bytes;

   // Allocate memory
   void *module_mem = topo_allocate_module_memory(topo_ptr, memory_req.size_in_bytes, module_info_ptr->use_lpm, pp_type);
   if (NULL == module_mem)
   {
       MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module with ID 0x%lX failed to allocate memory. The module requested for %lu bytes.", topo_ptr->obj_id, module_info_ptr->module_id, memory_req.size_in_bytes);
       return ADSP_ENOMEMORY;
   }
   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Module with ID 0x%lX allocated for %lu bytes of memory at location 0x%p. Use LPM = %lu", topo_ptr->obj_id, module_info_ptr->module_id, memory_req.size_in_bytes, module_mem, (uint32_t)(module_info_ptr->use_lpm));

   topo_ptr->module[index].module_ptr = reinterpret_cast<capi_v2_t*>(module_mem);

   // Initialize
   err_code = adsp_amdb_capi_v2_init_f(handle, topo_ptr->module[index].module_ptr, init_proplist_ptr);
   if(CAPI_V2_FAILED(err_code))
   {
       MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module with ID 0x%lX failed in init with %d.", topo_ptr->obj_id, module_info_ptr->module_id, static_cast<int>(err_code));
       qurt_elite_memory_free(topo_ptr->module[index].module_ptr);
       topo_ptr->module[index].module_ptr = NULL;
       return capi_v2_err_to_adsp_result(err_code);
   }

   // query interface extensions supported by the module
   // the function has no return value as the module should work irrespective of the interface extension.
   topo_module_query_intf_extn(&topo_ptr->module[index], handle);

   return result;
}

static ADSPResult common_topo_create_capi_v2_module(topo_struct *topo_ptr, adsp_amdb_capi_v2_t *handle, const uint32_t index, avcs_module_instance_info_t *module_info_ptr, AudPP_AudProcType pp_type, uint32_t *stack_size_ptr, capi_v2_proplist_t *init_proplist_ptr)
{
    ADSPResult result = ADSP_EOK;
    capi_v2_err_t err_code = CAPI_V2_EOK;

    // Query static properties

    // First query for the properties which are not optional.
    capi_v2_init_memory_requirement_t memory_req = { 0 };
    capi_v2_prop_t req_props[] = {
            { CAPI_V2_INIT_MEMORY_REQUIREMENT, { reinterpret_cast<int8_t*>(&memory_req),   0, sizeof(memory_req)   }, { FALSE, FALSE, 0} },
    };
    capi_v2_proplist_t req_proplist = { SIZE_OF_ARRAY(req_props), req_props };
    err_code = adsp_amdb_capi_v2_get_static_properties_f(handle, init_proplist_ptr, &req_proplist);
    if(CAPI_V2_FAILED(err_code))
    {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module with ID 0x%lX failed in getting some required static properties with %d.", topo_ptr->obj_id, module_info_ptr->module_id, static_cast<int>(err_code));
        return capi_v2_err_to_adsp_result(err_code);
    }

    // Query for the optional properties now.
    //capi_v2_init_memory_requirement_t dummy_memory_req = { 0 };
    capi_v2_is_inplace_t is_inplace = { FALSE };
    capi_v2_requires_data_buffering_t req_data_buf = { TRUE };
    capi_v2_stack_size_t stack_size = { 0 };
    capi_v2_num_needed_framework_extensions_t num_extensions = { 0 };
    capi_v2_prop_t optional_props[] = {
            { CAPI_V2_IS_INPLACE,              { reinterpret_cast<int8_t*>(&is_inplace),   0, sizeof(is_inplace)   }, { FALSE, FALSE, 0} },
            { CAPI_V2_REQUIRES_DATA_BUFFERING, { reinterpret_cast<int8_t*>(&req_data_buf), 0, sizeof(req_data_buf) }, { FALSE, FALSE, 0} },
            { CAPI_V2_STACK_SIZE,                       { reinterpret_cast<int8_t*>(&stack_size),     0, sizeof(stack_size)       }, { FALSE, FALSE, 0} },
            { CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS,  { reinterpret_cast<int8_t*>(&num_extensions), 0, sizeof(num_extensions)   }, { FALSE, FALSE, 0} },

    };
    capi_v2_proplist_t optional_proplist = { SIZE_OF_ARRAY(optional_props), optional_props };
    err_code = adsp_amdb_capi_v2_get_static_properties_f(handle, init_proplist_ptr, &optional_proplist);
    if(CAPI_V2_FAILED(err_code))
    {
        MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX Module with ID 0x%lX failed in getting some optional static properties with %d. Using default values.", topo_ptr->obj_id, module_info_ptr->module_id, static_cast<int>(err_code));
    }

    topo_ptr->module[index].requires_data_buffering = req_data_buf.requires_data_buffering;
    if (topo_ptr->module[index].requires_data_buffering)
    {
        topo_ptr->module[index].is_in_place = FALSE;
    }
    else
    {
        topo_ptr->module[index].is_in_place = is_inplace.is_inplace;
    }

    *stack_size_ptr = (*stack_size_ptr > stack_size.size_in_bytes) ? *stack_size_ptr : stack_size.size_in_bytes;

    if (num_extensions.num_extensions != 0)
    {
        topo_ptr->module[index].num_fwk_extns = num_extensions.num_extensions;
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module with ID 0x%lX requires %lu framework extensions. Currently no extensions are supported.",  topo_ptr->obj_id, module_info_ptr->module_id, num_extensions.num_extensions);

    }

    // Allocate memory
    void *module_mem = topo_allocate_module_memory(topo_ptr, memory_req.size_in_bytes, FALSE, pp_type);
    if (NULL == module_mem)
    {
        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with ID 0x%lX-0x%lX "
                "failed to allocate memory. The module requested for %lu bytes.",
                topo_ptr->obj_id, module_info_ptr->module_id, module_info_ptr->instance_id, memory_req.size_in_bytes);
        return ADSP_ENOMEMORY;
    }
    MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Module-Instance with ID 0x%lX-0x%lX"
            " allocated for %lu bytes of memory at location 0x%p.",
            topo_ptr->obj_id, module_info_ptr->module_id, module_info_ptr->instance_id, memory_req.size_in_bytes, module_mem);

    topo_ptr->module[index].module_ptr = reinterpret_cast<capi_v2_t*>(module_mem);

    // Initialize
    err_code = adsp_amdb_capi_v2_init_f(handle, topo_ptr->module[index].module_ptr, init_proplist_ptr);
    if(CAPI_V2_FAILED(err_code))
    {
        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with ID 0x%lX-0x%lX"
                " failed in init with %d.", topo_ptr->obj_id,
                module_info_ptr->module_id, module_info_ptr->instance_id, static_cast<int>(err_code));
        qurt_elite_memory_free(topo_ptr->module[index].module_ptr);
        topo_ptr->module[index].module_ptr = NULL;
        return capi_v2_err_to_adsp_result(err_code);
    }
    // call query intf extn function if module creation is successful.
    // the function has no return value as the module should work irrespective of the interface extension.
    topo_module_query_intf_extn(&topo_ptr->module[index], handle);

    // call query fwk extn function if module creation is successful.
    // the function has no return value as the module should work irrespective of the interface extension.
    if(topo_ptr->module[index].num_fwk_extns)
    {
        topo_module_alloc_and_query_fwk_extn(topo_ptr, handle, index);
        QURT_ELITE_ASSERT(topo_ptr->module[index].fwk_extn_ptr != NULL);
    }
    return result;
}

static ADSPResult topo_init_single_module(topo_struct *topo_ptr,
      const uint32_t index,
      const audproc_module_info_t *module_info_ptr,
      const adsp_amdb_module_handle_info_t *handle_info_ptr,
      AudPP_AudProcType pp_type,
      uint32_t *stack_size_ptr,
      bool_t initialize_with_compressed_format)
{
   capi_v2_port_num_info_t port_info = topo_graph_get_port_num_info(&topo_ptr->graph, index);

   // Set up default values
   topo_ptr->module[index].module_id  = module_info_ptr->module_id;
   topo_ptr->module[index].instance_id = 0;
   topo_ptr->module[index].is_in_place = FALSE;
   topo_ptr->module[index].requires_data_buffering = TRUE;
   topo_ptr->module[index].is_enabled = TRUE;
   topo_ptr->module[index].changes_media_format = FALSE;
   topo_ptr->module[index].module_ptr = NULL;
   topo_ptr->module[index].KPPS       = 0;
   topo_ptr->module[index].bandwidth  = 0;
   topo_ptr->module[index].delay_in_us       = 0;
   topo_ptr->module[index].is_in_chain = FALSE;
   topo_ptr->module[index].disabled_till_next_media_event = FALSE;
   topo_ptr->module[index].headroom = 0;
   topo_ptr->module[index].num_input_ports = port_info.num_input_ports;
   topo_ptr->module[index].num_output_ports = port_info.num_output_ports;
   topo_ptr->module[index].input_ports_ptr = NULL;
   topo_ptr->module[index].output_ports_ptr = NULL;
   topo_ptr->module[index].num_fwk_extns = 0;
   topo_ptr->module[index].fwk_extn_ptr = NULL;
   topo_event_initialize_handle(topo_ptr, index);

   {
      ADSPResult result = topo_module_allocate_ports(&topo_ptr->module[index]);
      if (ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Failed to allocate memory for ports for module %lX.", topo_ptr->obj_id, module_info_ptr->module_id);
         return result;
      }
   }

   // Set up init properties
   capi_v2_heap_id_t heap_id = { topo_get_heap_id(module_info_ptr->use_lpm, pp_type) };
   capi_v2_event_callback_info_t cb_info = { topo_event_callback, topo_event_get_handle(topo_ptr, index) };
   capi_v2_prop_t init_props[] = {
           { CAPI_V2_PORT_NUM_INFO,            { reinterpret_cast<int8_t*>(&port_info),   sizeof(port_info), sizeof(port_info) }, { FALSE, FALSE, 0} },
           { CAPI_V2_HEAP_ID,             { reinterpret_cast<int8_t*>(&heap_id),    sizeof(heap_id),  sizeof(heap_id)  }, { FALSE, FALSE, 0} },
           { CAPI_V2_EVENT_CALLBACK_INFO,  { reinterpret_cast<int8_t*>(&cb_info),     sizeof(cb_info),   sizeof(cb_info)   }, { FALSE, FALSE, 0} },
           { CAPI_V2_CUSTOM_INIT_DATA,  { (int8_t*)(module_info_ptr->init_params), sizeof(module_info_ptr->init_params), sizeof(module_info_ptr->init_params) }, { FALSE, FALSE, 0 } }
   };
   capi_v2_proplist_t init_proplist = { SIZE_OF_ARRAY(init_props), init_props };

   // Create the module
   if(TRUE == is_amdb_module_handle_valid(handle_info_ptr))
   {
      ADSPResult result = ADSP_EOK;
      if(handle_info_ptr->interface_type == CAPI_V2)
      {
         result = topo_create_capi_v2_module(topo_ptr, handle_info_ptr->h.capi_v2_handle, index, module_info_ptr, pp_type, stack_size_ptr, &init_proplist);
      }
      else
      {
         MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Module ID 0x%lX type is not CAPIv2. Type is %d.", topo_ptr->obj_id, module_info_ptr->module_id, handle_info_ptr->interface_type);
         result = ADSP_EFAILED;
      }

      if (ADSP_FAILED(result))
      {
         topo_module_free_ports(&topo_ptr->module[index], &topo_ptr->buf_mgr);
         return result;
      }
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Module ID 0x%lX is not supported.", topo_ptr->obj_id, module_info_ptr->module_id);
      topo_module_free_ports(&topo_ptr->module[index], &topo_ptr->buf_mgr);
      return ADSP_EUNSUPPORTED;
   }


   if (topo_ptr->module[index].requires_data_buffering)
   {
       capi_v2_port_data_threshold_t input, output;

       // Set up the thresholds
       capi_v2_prop_t threshold_props[] = {
           { CAPI_V2_PORT_DATA_THRESHOLD,  { reinterpret_cast<int8_t*>(&input),   0, sizeof(input)   }, { TRUE, TRUE, 0} },
           { CAPI_V2_PORT_DATA_THRESHOLD,  { reinterpret_cast<int8_t*>(&output),   0, sizeof(output)   }, { TRUE, FALSE, 0} },
       };
       capi_v2_proplist_t threshold_proplist = {
               SIZE_OF_ARRAY(threshold_props),
               threshold_props
       };

       capi_v2_err_t err_code = topo_ptr->module[index].module_ptr->vtbl_ptr->get_properties(topo_ptr->module[index].module_ptr, &threshold_proplist);

       if (CAPI_V2_FAILED(err_code))
       {
           MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module with ID 0x%lX returned error %d when getting thresholds.", topo_ptr->obj_id, module_info_ptr->module_id, static_cast<int>(err_code));
           topo_module_destroy(&topo_ptr->module[index], &topo_ptr->buf_mgr);
           return capi_v2_err_to_adsp_result(err_code);
       }

       if (input.threshold_in_bytes != 1)
       {
           MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module with ID 0x%lX returned input threshold %lu. Only a threshold of 1 is supported.", topo_ptr->obj_id, module_info_ptr->module_id, input.threshold_in_bytes);
           topo_module_destroy(&topo_ptr->module[index], &topo_ptr->buf_mgr);
           return ADSP_EUNSUPPORTED;
       }

       if (output.threshold_in_bytes != 1)
       {
           MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module with ID 0x%lX returned output threshold %lu. Only a threshold of 1 is supported.", topo_ptr->obj_id, module_info_ptr->module_id, output.threshold_in_bytes);
           topo_module_destroy(&topo_ptr->module[index], &topo_ptr->buf_mgr);
           return ADSP_EUNSUPPORTED;
       }
   }

   return ADSP_EOK;
}

static ADSPResult common_topo_init_single_module(topo_struct *topo_ptr,
      const uint32_t index,
      avcs_module_instance_info_t *module_info_ptr,
      const adsp_amdb_module_handle_info_t *handle_info_ptr,
      AudPP_AudProcType pp_type,
      uint32_t *stack_size_ptr,
      bool_t initialize_with_compressed_format)
{
   capi_v2_port_num_info_t port_info = topo_graph_get_port_num_info(&topo_ptr->graph, index);

   // Set up default values
   topo_ptr->module[index].module_id  = module_info_ptr->module_id;
   topo_ptr->module[index].instance_id  = (uint32_t)module_info_ptr->instance_id;
   topo_ptr->module[index].is_in_place = FALSE;
   topo_ptr->module[index].requires_data_buffering = TRUE;
   topo_ptr->module[index].is_enabled = TRUE;
   topo_ptr->module[index].changes_media_format = FALSE;
   topo_ptr->module[index].module_ptr = NULL;
   topo_ptr->module[index].KPPS       = 0;
   topo_ptr->module[index].bandwidth  = 0;
   topo_ptr->module[index].delay_in_us       = 0;
   topo_ptr->module[index].is_in_chain = FALSE;
   topo_ptr->module[index].disabled_till_next_media_event = FALSE;
   topo_ptr->module[index].headroom = 0;
   topo_ptr->module[index].num_input_ports = port_info.num_input_ports;
   topo_ptr->module[index].num_output_ports = port_info.num_output_ports;
   topo_ptr->module[index].input_ports_ptr = NULL;
   topo_ptr->module[index].output_ports_ptr = NULL;
   topo_ptr->module[index].num_fwk_extns = 0;
   topo_ptr->module[index].fwk_extn_ptr = NULL;
   topo_event_initialize_handle(topo_ptr, index);

   {
      ADSPResult result = topo_module_allocate_ports(&topo_ptr->module[index]);
      if (ADSP_FAILED(result))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Failed to allocate memory for ports for module 0x%lX-0x%lX.",
                 topo_ptr->obj_id, module_info_ptr->module_id, module_info_ptr->instance_id);
         return result;
      }
   }

   // Set up init properties
   capi_v2_heap_id_t heap_id = { topo_get_heap_id(FALSE, pp_type) };
   capi_v2_event_callback_info_t cb_info = { topo_event_callback, topo_event_get_handle(topo_ptr, index) };
   capi_v2_prop_t init_props[] = {
         { CAPI_V2_PORT_NUM_INFO,            { reinterpret_cast<int8_t*>(&port_info),   sizeof(port_info), sizeof(port_info) }, { FALSE, FALSE, 0} },
         { CAPI_V2_HEAP_ID,             { reinterpret_cast<int8_t*>(&heap_id),    sizeof(heap_id),  sizeof(heap_id)  }, { FALSE, FALSE, 0} },
         { CAPI_V2_EVENT_CALLBACK_INFO,  { reinterpret_cast<int8_t*>(&cb_info),     sizeof(cb_info),   sizeof(cb_info)   }, { FALSE, FALSE, 0} }
   };
   capi_v2_proplist_t init_proplist = { SIZE_OF_ARRAY(init_props), init_props };

   // Create the module
   if(TRUE == is_amdb_module_handle_valid(handle_info_ptr))
   {
      ADSPResult result = ADSP_EOK;
      if(handle_info_ptr->interface_type == CAPI_V2)
      {
         result = common_topo_create_capi_v2_module(topo_ptr, handle_info_ptr->h.capi_v2_handle, index, module_info_ptr, pp_type, stack_size_ptr, &init_proplist);
      }
      else
      {
         MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Module ID 0x%lX type is not CAPIv2. Type is %d.", topo_ptr->obj_id, module_info_ptr->module_id, handle_info_ptr->interface_type);
         result = ADSP_EFAILED;
      }

      if (ADSP_FAILED(result))
      {
         topo_module_free_ports(&topo_ptr->module[index], &topo_ptr->buf_mgr);
         return result;
      }
   }
   else
   {
         MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Module ID 0x%lX is not supported.", topo_ptr->obj_id, module_info_ptr->module_id);
         topo_module_free_ports(&topo_ptr->module[index], &topo_ptr->buf_mgr);
         return ADSP_EUNSUPPORTED;
   }

   if (topo_ptr->module[index].requires_data_buffering)
   {
      capi_v2_port_data_threshold_t input, output;

      // Set up the thresholds
      capi_v2_prop_t threshold_props[] = {
            { CAPI_V2_PORT_DATA_THRESHOLD,  { reinterpret_cast<int8_t*>(&input),   0, sizeof(input)   }, { TRUE, TRUE, 0} },
            { CAPI_V2_PORT_DATA_THRESHOLD,  { reinterpret_cast<int8_t*>(&output),   0, sizeof(output)   }, { TRUE, FALSE, 0} },
      };
      capi_v2_proplist_t threshold_proplist = {
            SIZE_OF_ARRAY(threshold_props),
            threshold_props
      };

      capi_v2_err_t err_code = topo_ptr->module[index].module_ptr->vtbl_ptr->get_properties(topo_ptr->module[index].module_ptr, &threshold_proplist);

      if (CAPI_V2_FAILED(err_code))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module with ID 0x%lX returned error %d when getting thresholds.", topo_ptr->obj_id, module_info_ptr->module_id, static_cast<int>(err_code));
         topo_module_destroy(&topo_ptr->module[index], &topo_ptr->buf_mgr);
         return capi_v2_err_to_adsp_result(err_code);
      }

      if (input.threshold_in_bytes != 1)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module with ID 0x%lX returned input threshold %lu. Only a threshold of 1 is supported.", topo_ptr->obj_id, module_info_ptr->module_id, input.threshold_in_bytes);
         topo_module_destroy(&topo_ptr->module[index], &topo_ptr->buf_mgr);
         return ADSP_EUNSUPPORTED;
      }

      if (output.threshold_in_bytes != 1)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module with ID 0x%lX returned output threshold %lu. Only a threshold of 1 is supported.", topo_ptr->obj_id, module_info_ptr->module_id, output.threshold_in_bytes);
         topo_module_destroy(&topo_ptr->module[index], &topo_ptr->buf_mgr);
         return ADSP_EUNSUPPORTED;
      }
   }

   return ADSP_EOK;
}

ADSPResult topo_init(const audproc_topology_definition_t *def_ptr,
      const uint16_t obj_id,
      const uint32_t ouput_buffer_num_unit_frames,
      topo_struct **topo_obj,
      AudPP_AudProcType pp_type,
      uint32_t *stack_size_ptr,
      topo_event_client_t *event_client_ptr,
      bool_t initialize_with_compressed_format,
      bool_t is_input_output_realtime,
      uint32_t num_active_input_ports)
{
   ADSPResult result = ADSP_EOK;
   topo_struct *topo_ptr = NULL;
   uint32_t num_modules = 0;
   uint32_t num_created_modules = 0;
   uint32_t actual_num_modules = 0;
   uint32_t num_modules_filled = 0;

   const audproc_module_info_t *module_info_ptr = &(def_ptr->module_info[0]);
   adsp_amdb_module_handle_info_t *handle_info_ptr = NULL;
   topo_def_t active_topo_def;

   *topo_obj = NULL;
   active_topo_def.module_id_list = NULL;

   if (NULL == stack_size_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Stack size pointer is NULL.", obj_id);
      result = ADSP_EBADPARAM;
      goto cleanup;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Initializing topology begin", obj_id);

   result = topo_create_obj(obj_id, &topo_ptr, ouput_buffer_num_unit_frames, event_client_ptr,is_input_output_realtime);
   if (ADSP_FAILED(result))
   {
      goto cleanup;
   }
   num_modules = def_ptr->num_modules;
   if(0 != num_modules)
   {
       handle_info_ptr = (adsp_amdb_module_handle_info_t *)qurt_elite_memory_malloc(num_modules * sizeof(adsp_amdb_module_handle_info_t), QURT_ELITE_HEAP_DEFAULT);
       if (handle_info_ptr == NULL)
       {
           MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: "
                 "Failed to allocate memory for the module handler", obj_id);
           result = ADSP_ENOMEMORY;
           goto cleanup;
       }

       while (num_modules_filled < num_modules)
       {
           adsp_amdb_module_handle_info_t *handle_info;
           handle_info = handle_info_ptr + num_modules_filled;

           handle_info->interface_type = CAPI_V2;
           handle_info->type = AMDB_MODULE_TYPE_GENERIC;
           handle_info->id1 = module_info_ptr[num_modules_filled].module_id;
           handle_info->id2 = 0;
           handle_info->h.capi_v2_handle = NULL;
           handle_info->result = ADSP_EFAILED;

           num_modules_filled++;
       }
       /*
        * Note: This call will block till all modules with 'preload = 0' are loaded by the AMDB. This loading
        * happens using a thread pool using threads of very low priority. This can cause the current thread
        * to be blocked because of a low priority thread. If this is not desired, a callback function
        * should be provided that can be used by the AMDB to signal when the modules are loaded. The current
        * thread can then handle other tasks in parallel.
        */
       adsp_amdb_get_modules_request(handle_info_ptr, num_modules_filled, NULL, NULL);

       active_topo_def.module_id_list = (avcs_module_instance_info_t *)qurt_elite_memory_malloc(num_modules * sizeof(avcs_module_instance_info_t), QURT_ELITE_HEAP_DEFAULT);
       if (active_topo_def.module_id_list == NULL)
       {
           MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: "
                 "Failed to allocate memory for the module list", obj_id);
           result = ADSP_ENOMEMORY;
           goto cleanup;
       }
       for (uint32_t j = 0; j < num_modules; j++ )
       {
           if(handle_info_ptr[j].interface_type != STUB)
           {
               active_topo_def.module_id_list[actual_num_modules].module_id = def_ptr->module_info[j].module_id;
               active_topo_def.module_id_list[actual_num_modules].instance_id = 0;
               active_topo_def.module_id_list[actual_num_modules].reserved = 0;
               actual_num_modules++;
           }
           else
           {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                     "P%hX audproc_svc: Module ID 0x%lx is stubbed and removed from the topology ",
                     topo_ptr->obj_id, handle_info_ptr[j].id1);
           }
       }
   }

   active_topo_def.num_modules = actual_num_modules;
   active_topo_def.topology_id = def_ptr->topology_id;

   result = topo_graph_set_graph(&topo_ptr->graph, &active_topo_def);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to create the graph representation.", topo_ptr->obj_id);
      goto cleanup;
   }

   topo_graph_print_linear_topo(&topo_ptr->graph, obj_id,0);

    result = topo_allocate_stream_structs(topo_ptr);
    if (ADSP_FAILED(result))
    {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to create the stream structures.", topo_ptr->obj_id);
       goto cleanup;
    }

    *stack_size_ptr = AUDPROC_TOPO_DEFAULT_STACK_SIZE;

    num_modules = topo_graph_num_modules(&topo_ptr->graph);

    topo_lock_module_list(topo_ptr);
    if((0 != num_modules) && (handle_info_ptr!=NULL))
    {
       topo_ptr->module = (module_info*)qurt_elite_memory_malloc(sizeof(module_info) * num_modules, QURT_ELITE_HEAP_DEFAULT);
       if (NULL == topo_ptr->module)
       {
          MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to allocate modules array", topo_ptr->obj_id);
          topo_destroy(topo_ptr);
          result = ADSP_ENOMEMORY;
          goto cleanup;
       }
          memset(topo_ptr->module, 0, (sizeof(module_info) * num_modules));

       const audproc_module_info_t *module_info_ptr = &(def_ptr->module_info[0]);
       uint32_t stub_module_count = 0;

       /* Create the modules */
       for (uint32_t j = 0; j < num_modules; j++ )
       {

          while(handle_info_ptr[j+stub_module_count].interface_type == STUB)
          {
             stub_module_count++;
          }

          result = topo_init_single_module(topo_ptr, j, &module_info_ptr[j+stub_module_count], handle_info_ptr + j + stub_module_count, pp_type, stack_size_ptr, initialize_with_compressed_format);

          if (ADSP_FAILED(result))
          {
             MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Module with ID 0x%lX failed in init.", topo_ptr->obj_id, module_info_ptr[stub_module_count + j].module_id);
             break;
          }
          num_created_modules++;
       }
       if (ADSP_FAILED(result))
       {
          goto cleanup;
       }
    }
    topo_ptr->stack_size = *stack_size_ptr;

    topo_set_delay_ptr_virtualizer(topo_ptr);
    topo_unlock_module_list(topo_ptr);

    *topo_obj = topo_ptr;
    topo_update_chain_KPPS(topo_ptr);
    topo_update_chain_bandwidth(topo_ptr);
    topo_update_chain_delay(topo_ptr);
    topo_update_chain_headroom(topo_ptr);

   cleanup:

   if (ADSP_FAILED(result))
   {
      if (NULL != topo_ptr)
      {
         if (NULL != topo_ptr->module)
         {
            topo_destroy_modules(num_created_modules, topo_ptr->module, &topo_ptr->buf_mgr);
            qurt_elite_memory_free(topo_ptr->module);
            topo_ptr->module = NULL;
         }
         topo_destroy(topo_ptr);
         topo_ptr = NULL;
      }
   }

   // No need to keep the handles now since the modules are created.
   if(handle_info_ptr!=NULL)
   {
      adsp_amdb_release_handles(handle_info_ptr, def_ptr->num_modules);
      qurt_elite_memory_free(handle_info_ptr);
   }
   if (NULL != active_topo_def.module_id_list)
   {
      qurt_elite_memory_free(active_topo_def.module_id_list);
      active_topo_def.module_id_list = NULL;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Initializing topology End", obj_id);

   return result;
}

ADSPResult common_topo_init(elite_cmn_topo_db_entry_t *def_ptr,
      const uint16_t obj_id,
      const uint32_t ouput_buffer_num_unit_frames,
      topo_struct **topo_obj,
      AudPP_AudProcType pp_type,
      uint32_t *stack_size_ptr,
      topo_event_client_t *event_client_ptr,
      bool_t initialize_with_compressed_format,
      bool_t is_input_output_realtime,
      uint32_t num_active_input_ports)
{
   ADSPResult result = ADSP_EOK;
   topo_struct *topo_ptr = NULL;
   uint32_t num_modules = 0;
   uint32_t num_created_modules = 0;
   uint32_t num_modules_filled = 0;
   uint32_t actual_num_modules = 0;
   uint32_t stub_module_count = 0;
   uint32_t far_topo_module_count = 0;
   //elite_cmn_topo_db_entry_t far_topo;
   elite_cmn_topo_db_entry_t *final_ec_topo_ptr = NULL;
   elite_cmn_topo_db_entry_t *final_topo_def = def_ptr;

   adsp_amdb_module_handle_info_t *handle_info_ptr = NULL;
   topo_def_t active_topo_def;

   memset((void *)&active_topo_def, 0, sizeof(active_topo_def));

   *topo_obj = NULL;

   if (NULL == stack_size_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Stack size pointer is NULL.", obj_id);
      result = ADSP_EBADPARAM;
      goto cleanup;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Initializing topology begin", obj_id);

   result = topo_create_obj(obj_id, &topo_ptr, ouput_buffer_num_unit_frames, event_client_ptr,is_input_output_realtime);
   if (ADSP_FAILED(result))
   {
      goto cleanup;
   }

   num_modules = def_ptr->topo_def.num_modules;
   if(0 != num_modules)
   {
       handle_info_ptr = (adsp_amdb_module_handle_info_t *)qurt_elite_memory_malloc(num_modules * sizeof(adsp_amdb_module_handle_info_t), QURT_ELITE_HEAP_DEFAULT);
       if (handle_info_ptr == NULL)
       {
           MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: "
                 "Failed to allocate memory for the module handler", obj_id);
           result = ADSP_ENOMEMORY;
           goto cleanup;
       }
       while (num_modules_filled < num_modules)
       {
           adsp_amdb_module_handle_info_t *handle_info;
           handle_info = handle_info_ptr + num_modules_filled;

           handle_info->interface_type = CAPI_V2;
           handle_info->type = AMDB_MODULE_TYPE_GENERIC;
           handle_info->id1 = def_ptr->module_info[num_modules_filled].module_id;
           handle_info->id2 = 0;
           handle_info->h.capi_v2_handle = NULL;
           handle_info->result = ADSP_EFAILED;

           num_modules_filled++;
       }

       /*
        * Note: This call will block till all modules with 'preload = 0' are loaded by the AMDB. This loading
        * happens using a thread pool using threads of very low priority. This can cause the current thread
        * to be blocked because of a low priority thread. If this is not desired, a callback function
        * should be provided that can be used by the AMDB to signal when the modules are loaded. The current
        * thread can then handle other tasks in parallel.
        */
       adsp_amdb_get_modules_request(handle_info_ptr, num_modules, NULL, NULL);

       // Check for EC module presence in topo before creating topo
       uint32_t ec_module_num = TOPO_INVALID_MODULE_INDEX;
       topo_check_ec_module_presence(topo_ptr,handle_info_ptr,num_modules,&ec_module_num);

       if(ec_module_num != TOPO_INVALID_MODULE_INDEX)
       {
           topo_ptr->ec_extn_info = (topo_ec_fwk_extn_t *)qurt_elite_memory_malloc(sizeof(topo_ec_fwk_extn_t),QURT_ELITE_HEAP_DEFAULT);
           if (topo_ptr->ec_extn_info == NULL)
           {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: "
                       "Failed to allocate memory for modified final topo", topo_ptr->obj_id  );
               result = ADSP_ENOMEMORY;
               goto cleanup;
           }
           topo_ptr->ec_extn_info->is_far_port_active = FALSE;
           topo_ptr->ec_extn_info->module_index = ec_module_num;
           topo_ptr->ec_extn_info->module_id = (handle_info_ptr+ec_module_num)->id1;
           topo_ptr->ec_extn_info->instance_id = 0;
       }

       if(topo_ptr->ec_extn_info)
       {
           /* TODO : FAR TOPO INFO, HARDCODED FOR NOW */
           uint32_t final_ec_topo_ptr_size;
           adsp_amdb_module_handle_info_t *temp_handle_info_ptr = NULL;

           far_topo_module_count = sizeof(far_topo_modules_list)/sizeof(far_topo_modules_list[0]); //EC_FWK_EXTN_FAR_TOPO_NUM_MODULES;

           final_ec_topo_ptr_size = sizeof(elite_cmn_topo_db_entry_t) + (num_modules + far_topo_module_count - 1) * sizeof(avcs_module_instance_info_t);
           final_ec_topo_ptr = (elite_cmn_topo_db_entry_t*)qurt_elite_memory_malloc(final_ec_topo_ptr_size,QURT_ELITE_HEAP_DEFAULT);
           if (final_ec_topo_ptr == NULL)
           {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: "
                       "Failed to allocate memory for modified final topo", topo_ptr->obj_id  );
               result = ADSP_ENOMEMORY;
               goto cleanup;
           }
           final_ec_topo_ptr->topo_def.version = def_ptr->topo_def.version;
           final_ec_topo_ptr->topo_def.topo_config = def_ptr->topo_def.topo_config;
           final_ec_topo_ptr->topo_def.topology_id = def_ptr->topo_def.topology_id;
           final_ec_topo_ptr->topo_def.num_modules = num_modules + far_topo_module_count;

           for(uint32_t module_num=0;module_num<far_topo_module_count;module_num++)
           {
               final_ec_topo_ptr->module_info[module_num].module_id = far_topo_modules_list[module_num];
               final_ec_topo_ptr->module_info[module_num].instance_id = 0;
               final_ec_topo_ptr->module_info[module_num].reserved = 0;
           }

           topo_ptr->ec_extn_info->num_far_modules = far_topo_module_count;
           topo_ptr->ec_extn_info->module_index += far_topo_module_count;

           for(uint32_t module_num=far_topo_module_count;module_num<final_ec_topo_ptr->topo_def.num_modules;module_num++)
           {
               final_ec_topo_ptr->module_info[module_num].module_id = def_ptr->module_info[module_num - far_topo_module_count].module_id;
               final_ec_topo_ptr->module_info[module_num].instance_id = def_ptr->module_info[module_num - far_topo_module_count].instance_id;
               final_ec_topo_ptr->module_info[module_num].reserved = def_ptr->module_info[module_num - far_topo_module_count].reserved;
           }
           final_topo_def = final_ec_topo_ptr;

           topo_ptr->ec_extn_info->is_far_moudle_in_chain = (bool_t *)qurt_elite_memory_malloc(far_topo_module_count * sizeof(bool_t), QURT_ELITE_HEAP_DEFAULT);
           if (topo_ptr->ec_extn_info->is_far_moudle_in_chain == NULL)
           {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: "
                       "Failed to allocate memory for far topo module state info", topo_ptr->obj_id  );
               result = ADSP_ENOMEMORY;
               goto cleanup;
           }
           for(uint32_t module_num=0;module_num<far_topo_module_count;module_num++)
               topo_ptr->ec_extn_info->is_far_moudle_in_chain[module_num] = TRUE;

           temp_handle_info_ptr = handle_info_ptr;//(adsp_amdb_module_handle_info_t *)qurt_elite_memory_malloc(temp_handle_info_size, QURT_ELITE_HEAP_DEFAULT);


           num_modules = final_topo_def->topo_def.num_modules;
           handle_info_ptr = (adsp_amdb_module_handle_info_t *)qurt_elite_memory_malloc(num_modules * sizeof(adsp_amdb_module_handle_info_t), QURT_ELITE_HEAP_DEFAULT);
           if (handle_info_ptr == NULL)
           {
               // Restore original handle info pointer to be freed in cleanup function
               handle_info_ptr = temp_handle_info_ptr;
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: "
                       "Failed to allocate memory for the module handler", topo_ptr->obj_id  );
               result = ADSP_ENOMEMORY;
               goto cleanup;
           }
           num_modules_filled = 0;
           while (num_modules_filled < num_modules)
           {
               adsp_amdb_module_handle_info_t *handle_info;
               handle_info = handle_info_ptr + num_modules_filled;

               handle_info->interface_type = CAPI_V2;
               handle_info->type = AMDB_MODULE_TYPE_GENERIC;
               handle_info->id1 = final_topo_def->module_info[num_modules_filled].module_id;
               handle_info->id2 = 0;
               handle_info->h.capi_v2_handle = NULL;
               handle_info->result = ADSP_EFAILED;

               num_modules_filled++;
           }
           /*
            * Note: This call will block till all modules with 'preload = 0' are loaded by the AMDB. This loading
            * happens using a thread pool using threads of very low priority. This can cause the current thread
            * to be blocked because of a low priority thread. If this is not desired, a callback function
            * should be provided that can be used by the AMDB to signal when the modules are loaded. The current
            * thread can then handle other tasks in parallel.
            */
           adsp_amdb_get_modules_request(handle_info_ptr, num_modules, NULL, NULL);

           // After getting handles for all modules, release handles of primary stream topo, this will release additional reference counts
           if(temp_handle_info_ptr!=NULL)
           {
              adsp_amdb_release_handles(temp_handle_info_ptr, def_ptr->topo_def.num_modules);
              qurt_elite_memory_free(temp_handle_info_ptr);
              temp_handle_info_ptr = NULL;
           }
       }

       active_topo_def.module_id_list = NULL;
       active_topo_def.module_id_list = (avcs_module_instance_info_t *)qurt_elite_memory_malloc(num_modules * sizeof(avcs_module_instance_info_t), QURT_ELITE_HEAP_DEFAULT);
       if (active_topo_def.module_id_list == NULL)
       {
           MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: "
                 "Failed to allocate memory for the module list", obj_id);
           result = ADSP_ENOMEMORY;
           goto cleanup;
       }
       for (uint32_t j = 0; j < num_modules; j++ )
       {
           if(handle_info_ptr[j].interface_type != STUB)
           {
               active_topo_def.module_id_list[actual_num_modules].module_id = final_topo_def->module_info[j].module_id;
               active_topo_def.module_id_list[actual_num_modules].instance_id = final_topo_def->module_info[j].instance_id;
               active_topo_def.module_id_list[actual_num_modules].reserved = 0;
               actual_num_modules++;
           }
           else
           {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                     "P%hX audproc_svc: Module ID 0x%lx is stubbed and removed from the topology ",
                     topo_ptr->obj_id, handle_info_ptr[j].id1);
               // Adust EC module index and number of modules in far topo if a module is stubbed in chain
               if(topo_ptr->ec_extn_info && (j < topo_ptr->ec_extn_info->module_index))
               {
                   topo_ptr->ec_extn_info->module_index--;
                   if(j < topo_ptr->ec_extn_info->num_far_modules)
                   {
                       topo_ptr->ec_extn_info->num_far_modules--;
                       far_topo_module_count--;
                   }
               }
           }
       }
   }

   active_topo_def.num_modules = actual_num_modules;
   active_topo_def.topology_id = final_topo_def->topo_def.topology_id;

   if(topo_ptr->ec_extn_info)
   {
       QURT_ELITE_ASSERT(num_active_input_ports > 1);
       result = common_topo_graph_set_ec_graph(&topo_ptr->graph, &active_topo_def,topo_ptr->ec_extn_info);
   }
   else
   {
       result = common_topo_graph_set_graph(&topo_ptr->graph, &active_topo_def);
   }
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to create the graph representation.", topo_ptr->obj_id);
      goto cleanup;
   }

   topo_graph_print_linear_topo(&topo_ptr->graph, obj_id,far_topo_module_count);

   result = topo_allocate_stream_structs(topo_ptr);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to create the stream structures.", topo_ptr->obj_id);
      goto cleanup;
   }

   *stack_size_ptr = AUDPROC_TOPO_DEFAULT_STACK_SIZE;

   num_modules = topo_graph_num_modules(&topo_ptr->graph);

   topo_lock_module_list(topo_ptr);
   if((0 != num_modules) && (handle_info_ptr!=NULL))
   {
      topo_ptr->module = (module_info*)qurt_elite_memory_malloc(sizeof(module_info) * num_modules, QURT_ELITE_HEAP_DEFAULT);
      if (NULL == topo_ptr->module)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to allocate modules array", topo_ptr->obj_id);
         topo_destroy(topo_ptr);
         result = ADSP_ENOMEMORY;
         goto cleanup;
      }

      memset(topo_ptr->module, 0, (sizeof(module_info) * num_modules));

      avcs_module_instance_info_t *module_info_ptr = &(final_topo_def->module_info[0]);

      /* Create the modules : num_modules are already adjusted to actual valid modules removing all stubs*/
      for (uint32_t j = 0; j < num_modules; j++ )
      {
         while(handle_info_ptr[j+stub_module_count].interface_type == STUB)
         {
            stub_module_count++;
         }
         result = common_topo_init_single_module(topo_ptr, j, &module_info_ptr[stub_module_count + j], handle_info_ptr + stub_module_count + j, pp_type, stack_size_ptr, initialize_with_compressed_format);
         if (ADSP_FAILED(result))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Module-Instance with ID 0x%lX-0x%lX failed in init.",
                  topo_ptr->obj_id, module_info_ptr[stub_module_count + j].module_id, module_info_ptr[stub_module_count + j].instance_id);
            break;
         }
         num_created_modules++;
      }

      if (ADSP_FAILED(result))
      {
          goto cleanup;
      }
      // Since all modules are created, handle framework extensions for the modules
      topo_module_process_fwk_extn(topo_ptr, num_created_modules);
   }

   topo_ptr->stack_size = *stack_size_ptr;

   topo_set_delay_ptr_virtualizer(topo_ptr);
   topo_unlock_module_list(topo_ptr);

   *topo_obj = topo_ptr;
   topo_update_chain_KPPS(topo_ptr);
   topo_update_chain_bandwidth(topo_ptr);
   topo_update_chain_delay(topo_ptr);
   topo_update_chain_headroom(topo_ptr);

cleanup:

   if (ADSP_FAILED(result))
   {
      if (NULL != topo_ptr)
      {
         if (NULL != topo_ptr->module)
         {
            topo_destroy_modules(num_created_modules, topo_ptr->module, &topo_ptr->buf_mgr);
            qurt_elite_memory_free(topo_ptr->module);
            topo_ptr->module = NULL;
         }
         topo_destroy(topo_ptr);
         topo_ptr = NULL;
      }
   }
   // No need to keep the handles now since the modules are created.
   if(handle_info_ptr!=NULL)
   {
      adsp_amdb_release_handles(handle_info_ptr, final_topo_def->topo_def.num_modules);
      qurt_elite_memory_free(handle_info_ptr);
   }
   if (NULL != active_topo_def.module_id_list)
   {
      qurt_elite_memory_free(active_topo_def.module_id_list);
      active_topo_def.module_id_list = NULL;
   }
   if(NULL != final_ec_topo_ptr)
   {
       qurt_elite_memory_free(final_ec_topo_ptr);
       final_ec_topo_ptr = NULL;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Initializing topology End", obj_id);

   return result;
}
/**
 * Outputs data from any intermediate buffers if present. This
 * can be done at end of stream or before a change in media type
 * or parameters.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] obj_ptr
 *   Pointer to the topology object.
 * @param[out] outbuf_ptr
 *   The output buffer to which the data is to be written.
 * @param[out] buffer_reset_flag_ptr
 *   Indicates if there is still any data left in the
 *   intermediate buffers.
 *
 */
void topo_buffer_reset (topo_struct *topo_ptr,
      AudPP_BufInfo_t* outbuf_ptr,
      AudPP_BufInfo_t* prev_inbuf_ptr,
      bool_t* buffer_reset_flag_ptr,
      bool_t is_bg_thread_created,
      void *bg_thread_handle)
{
   ADSPResult result;

   uint32_t buff_size_in_samples;
   *buffer_reset_flag_ptr = FALSE;

   if(is_bg_thread_created)
   {

       if (NULL == topo_ptr->eos_buf.pBuf)
       {
           uint32_t total_delay_us = 0, delay_in_samples;
           total_delay_us = topo_bg_thread_buffering_delay_us(bg_thread_handle);
           for(uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
           {
               if (topo_ptr->module[i].is_in_chain)
               {
                   total_delay_us += topo_ptr->module[i].delay_in_us;
               }
           }

           delay_in_samples = topo_convert_us_to_samples(total_delay_us, topo_ptr->in_format[TOPO_PRIMARY_STRM].samplingRate);

           topo_ptr->rem_delay_samples = delay_in_samples;

           buff_size_in_samples = (topo_ptr->rem_delay_samples > prev_inbuf_ptr->sizeInSamples)?prev_inbuf_ptr->sizeInSamples:topo_ptr->rem_delay_samples;
           uint32_t bytes_to_alloc = buff_size_in_samples * topo_get_bytes_per_sample(topo_ptr->in_format[TOPO_PRIMARY_STRM].bitsPerSample) * topo_ptr->in_format[TOPO_PRIMARY_STRM].channels;

           int8_t* ptr = (int8_t *)qurt_elite_memory_malloc(bytes_to_alloc, QURT_ELITE_HEAP_DEFAULT);
           if (!ptr)
           {
              MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: Out of Memory while creating delay buffer!");
              *buffer_reset_flag_ptr = TRUE;
              topo_ptr->eos_buf.pBuf = NULL;
              topo_ptr->eos_buf.sizeInSamples = 0;
              topo_ptr->eos_buf.offsetInSamples = 0;
              return;
           }
           memset(ptr, 0, bytes_to_alloc);
           topo_ptr->eos_buf.pBuf = ptr;
       }

       buff_size_in_samples = (topo_ptr->rem_delay_samples > prev_inbuf_ptr->sizeInSamples)?prev_inbuf_ptr->sizeInSamples:topo_ptr->rem_delay_samples;

       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: rem size of delay buffer!  %d", (int)topo_ptr->rem_delay_samples);
       topo_ptr->eos_buf.sizeInSamples = buff_size_in_samples;
       topo_ptr->eos_buf.offsetInSamples = 0;

       topo_ptr->eos_buf.flags = prev_inbuf_ptr->flags;
       if((topo_ptr->eos_buf.flags.is_timestamp_valid) && (topo_ptr->in_format[TOPO_PRIMARY_STRM].samplingRate!=0))
       {
          topo_ptr->eos_buf.timestamp = prev_inbuf_ptr->timestamp +(prev_inbuf_ptr->sizeInSamples *1000000)/topo_ptr->in_format[TOPO_PRIMARY_STRM].samplingRate;
       }

       AudPP_BufInfo_t *inPortBufs[1];
       AudPP_BufInfo_t *outPortBufs[1];
       inPortBufs[0] = &(topo_ptr->eos_buf);
       outPortBufs[0] = outbuf_ptr;

       result = topo_bg_thread_start_process(bg_thread_handle,
               inPortBufs,
                outPortBufs);

       topo_ptr->rem_delay_samples -= buff_size_in_samples;
   }
   else
   {
   if (NULL == topo_ptr->eos_buf.pBuf)
   {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_topo handler: allocating the eos buffer ");
      result = create_delay_buffer(topo_ptr);
      if (ADSP_FAILED(result))
      {
         topo_ptr->eos_buf.pBuf = NULL;
         topo_ptr->eos_buf.sizeInSamples = 0;
         topo_ptr->eos_buf.offsetInSamples = 0;
         *buffer_reset_flag_ptr = TRUE;
         return;
      }
   }

   topo_ptr->eos_buf.flags = prev_inbuf_ptr->flags;
   if((topo_ptr->eos_buf.flags.is_timestamp_valid) && (topo_ptr->in_format[TOPO_PRIMARY_STRM].samplingRate!=0))
   {
      topo_ptr->eos_buf.timestamp = prev_inbuf_ptr->timestamp +(prev_inbuf_ptr->sizeInSamples *1000000)/topo_ptr->in_format[TOPO_PRIMARY_STRM].samplingRate;
   }

   // If there is delay associated with this chain
   // then flush out intermediate buffers into
   // the output buffers, before setting flag
   AudPP_BufInfo_t *inPortBufs[1];
   AudPP_BufInfo_t *outPortBufs[1];
  inPortBufs[0] = &(topo_ptr->eos_buf);
  outPortBufs[0] = outbuf_ptr;
  result = topo_process(topo_ptr,
          1,
          inPortBufs,
          1,
          outPortBufs);

   }

}


void topo_free_eos_buffer (topo_struct *topo_ptr,
      bool_t* buffer_reset_flag_ptr)
{
   *buffer_reset_flag_ptr = FALSE;
    if (NULL != topo_ptr->eos_buf.pBuf)
    {
     qurt_elite_memory_free(topo_ptr->eos_buf.pBuf);
     topo_ptr->eos_buf.pBuf= NULL;
    }
    topo_ptr->eos_buf.sizeInSamples = 0;
    topo_ptr->eos_buf.offsetInSamples = 0;
    *buffer_reset_flag_ptr = TRUE;
}


/**
 * Get the samples buffered at the input of each module in the
 * topology chain. This value is used to correct the offset value
 * during the time-stamp calculation
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] obj_ptr
 *   Pointer to the object structure.
 * @return uint64_t
 *   returns the samples buffered in the topology.
 */
uint64_t topo_get_internal_buffer_delay(topo_struct *topo_ptr)
{
    uint64_t total_buffer_delay = 0;
    uint32_t current_module_index = topo_ptr->last_port.module_index;

    while((topo_ptr->first_port[TOPO_PRIMARY_STRM].module_index != current_module_index) && (current_module_index!=TOPO_INVALID_MODULE_INDEX))
    {
        // The implementation assumes there is only one input port
        // for each module in present use-cases.
        // TODO : Check if input ports condition need to be enabled and modify buffer delay logic
        //QURT_ELITE_ASSERT(1 == topo_ptr->module[current_module_index].num_input_ports);
        uint32_t buffered_samples_at_module_input = 0;
        uint32_t module_input_port_index = 0;
        const topo_port_index_t port_index = { current_module_index, module_input_port_index, topo_port_index_t::INPUT_PORT };

        if(topo_ptr->module[current_module_index].is_in_chain)
        {
            topo_buf_t *in_buf_ptr = topo_module_get_input_buffer(topo_ptr, &port_index);
            uint32_t input_length = in_buf_ptr->valid_data_end - in_buf_ptr->valid_data_start;
            buffered_samples_at_module_input = topo_buffer_bytes_to_samples(in_buf_ptr, input_length);
            total_buffer_delay += ((uint64_t)buffered_samples_at_module_input * 1000000)/
                    topo_ptr->module[current_module_index].input_ports_ptr[module_input_port_index].media_event.event.f.sampling_rate;
        }

        topo_port_index_t prev_port_index = { TOPO_INVALID_MODULE_INDEX, 0, topo_port_index_t::OUTPUT_PORT };
        prev_port_index = topo_graph_get_prev_module(&topo_ptr->graph, port_index);
        current_module_index = prev_port_index.module_index;
    }


    /*MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: total buffer_duration %llu",
            topo_ptr->obj_id, total_buffer_delay);*/

    return total_buffer_delay;
}
/**
 *  This function post-processes input data to output buffer
 *  It also correctly sets the input/output buffer offset after operation.
 * @param[in, out] obj_ptr This points to the current topology
 *          instance.
 * @param[in] inbuf_ptr  This describes the input buffer.
 * @param[in] outbuf_ptr   This describes the output buffer.
 *
 * @return ADSPResult ADSP error code
 */
ADSPResult topo_process(topo_struct *topo_ptr,
        uint32_t num_in_ports,
      AudPP_BufInfo_t *in_bufs_ptr[],
        uint32_t num_out_ports,
      AudPP_BufInfo_t *out_bufs_ptr[])
{
    QURT_ELITE_ASSERT(num_in_ports == topo_ptr->max_in_streams);

    AdspAudioMediaFormatInfo_t oldMediaInfo =  topo_ptr->out_format;
    if (!topo_ptr->data_format_received[TOPO_PRIMARY_STRM])
    {
        return ADSP_ENOTREADY;
    }

    for(uint32_t port_id=0;port_id<num_in_ports;port_id++)
    {
        topo_buf_t *main_input_buffer_ptr = &topo_ptr->main_input_buffer[port_id];

        // Convert the input and output buffers to the appi_buflist_t form.
        topo_buffer_convert_from_ext_input_buffer(in_bufs_ptr[port_id],
                main_input_buffer_ptr,
                &topo_ptr->in_format[port_id]);
    }
    topo_buf_t *main_output_buffer_ptr = &topo_ptr->main_output_buffer;
    AudPP_BufInfo_t *outbuf_ptr = out_bufs_ptr[TOPO_PRIMARY_STRM];

    if (NULL != topo_ptr->internal_buf.pBuf)
    {
        /* if internal buffer is created, the internal buffer has to properly set as topology output buffer.
        Instead of external service buffer, the internal buffer would be the final output for topology*/
        topo_buffer_convert_from_int_output_buffer(&topo_ptr->internal_buf,
                main_output_buffer_ptr,
                &topo_ptr->out_format);

    }
    else
    {
        topo_buffer_convert_from_ext_output_buffer(outbuf_ptr,
                main_output_buffer_ptr,
                &topo_ptr->out_format);
    }

    // Since there can be intermediate buffering, there is no guarantee of the number
    // of samples consumed and produced. So loop till either input buffer
    // is empty or the output buffer is full.
    // consume any more input as this will just increase the data in the internal buffers.
    topo_lock_module_list(topo_ptr);

    bool_t topo_output_media_changed = FALSE;
    if (!topo_zero_modules_enabled(topo_ptr))
    {
        uint32_t current_module_index = topo_ptr->last_port.module_index;

        while (!topo_buffer_is_buf_full(main_output_buffer_ptr))
        {
            QURT_ELITE_ASSERT(TOPO_INVALID_MODULE_INDEX != current_module_index);
            if(current_module_index == topo_ptr->last_port.module_index)
            {
                topo_check_empty_last_buffer(topo_ptr);
            }
            if(topo_buffer_is_buf_full(main_output_buffer_ptr))
            {
                break;
            }

            if (topo_module_pending_reconfig(topo_ptr, current_module_index))
            {
                // Port_id input to this routine need only be TOPO_PRIMARY_STRM as this is not handling input media format
                topo_output_media_changed = topo_handle_pending_events(topo_ptr, current_module_index,TOPO_PRIMARY_STRM);
                if (topo_output_media_changed || topo_zero_modules_enabled(topo_ptr))
                {
                    break;
                }
                else
                {
                    current_module_index = topo_ptr->last_port.module_index;
                }
            }
            else
            {
               module_info *current_module = &topo_ptr->module[current_module_index];
               uint32_t required_in_port_index = TOPO_INVALID_STRM;
		         /*MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: TODO : Checking if module with id 0x%lX-0x%lX and index %d. can process.",
		        		 topo_ptr->obj_id, current_module->module_id, current_module->instance_id,current_module_index);*/
               if (topo_module_can_process(topo_ptr, current_module_index, &required_in_port_index))
               {		   
                  // Process data
                  QURT_ELITE_ASSERT(current_module->is_in_chain);

                     /* MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: TODO : Invoking process for module with id 0x%lX-0x%lX. ",
                             topo_ptr->obj_id, current_module->module_id, current_module->instance_id);*/
                  ADSPResult err_code = topo_process_module(topo_ptr, current_module_index);
                  if (ADSP_FAILED(err_code))
                  {
                     MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Failed while processing module with id 0x%lX-0x%lX. Disabling it.",
                             topo_ptr->obj_id, current_module->module_id, current_module->instance_id);
                     current_module->disabled_till_next_media_event = TRUE;
                     continue;
                  }

                  for (uint32_t i = 0; i < current_module->num_output_ports; i++)
                  {
                      uint32_t req_port_index = TOPO_INVALID_STRM;
                      uint32_t connected_module_index = current_module->output_ports_ptr[i].connected_port.module_index;
					  
                      if ((TOPO_INVALID_MODULE_INDEX != connected_module_index)
                              &&(connected_module_index > current_module_index))
                      {
                         module_info *connected_module = &topo_ptr->module[connected_module_index];
                         if(topo_module_can_process(topo_ptr, connected_module_index,&req_port_index))
                         {
                             current_module_index = connected_module_index;
                         }
                         else if(req_port_index != TOPO_INVALID_STRM)
                         {
                             current_module_index = connected_module->input_ports_ptr[req_port_index].connected_port.module_index;
                         }
                         else
                         { 
                             MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Invalid case of disabled connected module during process loop id 0x%lX-0x%lX. bailing out.",
                                    topo_ptr->obj_id, connected_module->module_id, connected_module->instance_id);
                             break;
                         }						  
                      }
                  }				  
                }
                else
                {				
                    if(required_in_port_index != TOPO_INVALID_STRM)
                    {
                        current_module_index = current_module->input_ports_ptr[required_in_port_index].connected_port.module_index;
                    }
                    else
                    {
                         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Invalid case of disabled module during process loop id 0x%lX-0x%lX. bailing out.",
                                 topo_ptr->obj_id, current_module->module_id, current_module->instance_id);
                        break;
                    }
			         /*MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: TODO : module with id 0x%lX-0x%lX and index %d. cannot process.",
			        		 topo_ptr->obj_id, current_module->module_id, current_module->instance_id,current_module_index);*/

                    if(TOPO_INVALID_MODULE_INDEX == current_module_index)
                    {
                        QURT_ELITE_ASSERT(topo_buffer_is_buf_empty(&topo_ptr->main_input_buffer[TOPO_PRIMARY_STRM]));
                        break;
                    }
                    if(TOPO_INVALID_MODULE_INDEX_1 == current_module_index)
                    {
                        QURT_ELITE_ASSERT(topo_buffer_is_buf_empty(&topo_ptr->main_input_buffer[TOPO_FAR_STRM]));
                        break;
                    }
                }
            }
        }
    }

   if (!topo_output_media_changed) // If the output media changed, the output buffer must not be written to.
   {
      if (topo_zero_modules_enabled(topo_ptr))
      {
         topo_loop_thru_zero_modules(topo_ptr);
      }
   }

    // When processing done, discard input data on all non-active ports
    for(uint32_t port_id=num_in_ports;port_id > topo_ptr->active_in_streams;port_id--)
    {
        uint32_t port_index = port_id - 1;
        topo_ptr->main_input_buffer[port_index].valid_data_start = topo_ptr->main_input_buffer[port_index].valid_data_end = 0;
    }

    // TODO : All input ports empty or either one?
    QURT_ELITE_ASSERT(topo_output_media_changed || ( topo_buffer_is_buf_empty(&topo_ptr->main_input_buffer[TOPO_PRIMARY_STRM]) && topo_buffer_is_buf_empty(&topo_ptr->main_input_buffer[TOPO_FAR_STRM])) || topo_buffer_is_buf_full(main_output_buffer_ptr));

    topo_unlock_module_list(topo_ptr);

    for(uint32_t port_id=0;port_id<num_in_ports;port_id++)
    {
        topo_buf_t *main_input_buffer_ptr = &topo_ptr->main_input_buffer[port_id];
        // Convert the input and output buffers to the appi_buflist_t form.
        topo_buffer_update_external_input_buf_sizes(in_bufs_ptr[port_id],
                main_input_buffer_ptr,
                &topo_ptr->in_format[port_id]);
    }

    if (NULL != topo_ptr->internal_buf.pBuf)
    {
        /* internal buffer pointer have to be updated and the copy to external (service) buffer has is done */
    	topo_internal_buf_update_offsets(&topo_ptr->internal_buf, main_output_buffer_ptr, &oldMediaInfo);
        topo_internal_buf_copy_to_ext_output_buf(outbuf_ptr, topo_ptr, &oldMediaInfo);
    }
    else
    {
    	topo_buffer_update_external_output_buf_sizes(outbuf_ptr, main_output_buffer_ptr, &oldMediaInfo);
    }

    return ADSP_EOK;
}

static ADSPResult topo_process_module(topo_struct *topo_ptr, uint32_t module_index)
{

    if (topo_inplace_processing_possible(topo_ptr, module_index))
    {
        return topo_process_module_inplace(topo_ptr, module_index);
    }

    module_info *module_struct_ptr = &topo_ptr->module[module_index];

    uint32_t min_samples = UINT32_MAX;

    // Set up the structures for the input streams for every port.
    QURT_ELITE_ASSERT(module_struct_ptr->num_input_ports <= topo_ptr->max_in_streams);
    for (uint32_t i = 0; i < module_struct_ptr->num_input_ports; i++)
    {
        const topo_port_index_t port_index = { module_index, i, topo_port_index_t::INPUT_PORT };

        topo_buf_t *in_buf_ptr = topo_module_get_input_buffer(topo_ptr, &port_index);

        uint32_t input_length = in_buf_ptr->valid_data_end - in_buf_ptr->valid_data_start;
        uint32_t samples_in_input = topo_buffer_bytes_to_samples(in_buf_ptr, input_length);
        if ((samples_in_input < min_samples) && topo_is_port_or_stream_active(topo_ptr, i))
        {
            min_samples = samples_in_input;
        }

        if(module_struct_ptr->input_ports_ptr[i].data_threshold > 1)
        {
           if(topo_is_port_or_stream_active(topo_ptr, i))
           {
              if(input_length >= module_struct_ptr->input_ports_ptr[i].data_threshold)
              {
                  input_length = module_struct_ptr->input_ports_ptr[i].data_threshold;
              }
              else
              {
                  MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                       "insufficient number of bytes %lu for input port %lu.",
                        topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id,input_length, i);
                  return ADSP_EBADPARAM;
              }
           }
           else
           {
              input_length = 0;
           }
        }

#ifdef DBG_TOPO_HANDLER
MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
          "number of bytes %lu for input port %lu.",
          topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id,input_length, i);
#endif

        // Set up the input buffers
        topo_ptr->in_streams[i].flags = in_buf_ptr->flags;
        topo_ptr->in_streams[i].timestamp = in_buf_ptr->timestamp;

        if(in_buf_ptr->valid_data_start!=0)
        {
            uint64_t timestamp_offset = ((in_buf_ptr->valid_data_start/in_buf_ptr->bytes_to_samples_factor) * 1000000);
            timestamp_offset/=(module_struct_ptr->input_ports_ptr->media_event.event.f.sampling_rate);
            topo_ptr->in_streams[i].timestamp += timestamp_offset;
        }

        topo_ptr->in_streams[i].bufs_num = in_buf_ptr->num_bufs;
        QURT_ELITE_ASSERT(in_buf_ptr->num_bufs <= CAPI_V2_MAX_CHANNELS);
        capi_v2_buf_t *in_bufs = topo_ptr->in_streams[i].buf_ptr;
        for(uint32_t j = 0; j < in_buf_ptr->num_bufs; j++)
        {
            in_bufs[j].actual_data_len = input_length;
            in_bufs[j].max_data_len = in_buf_ptr->size_per_buffer - in_buf_ptr->valid_data_start;
            in_bufs[j].data_ptr = in_buf_ptr->buf_ptrs[j] + in_buf_ptr->valid_data_start;
        }
    }

    for (uint32_t i = 0; i < module_struct_ptr->num_output_ports; i++)
    {
        const topo_port_index_t port_index = { module_index, i, topo_port_index_t::OUTPUT_PORT } ;

        topo_buf_t *out_buf_ptr = topo_module_get_output_buffer(topo_ptr, &port_index);

        // Rewind the output buffer if it is empty. Also get the buffers from the buffer manager.
        if (topo_buffer_is_buf_empty(out_buf_ptr))
        {
            out_buf_ptr->valid_data_start = out_buf_ptr->valid_data_end = 0;

            if (module_index != topo_ptr->last_port.module_index)
            {
                ADSPResult err_code = topo_buf_manager_get_bufs(&topo_ptr->buf_mgr, out_buf_ptr);
                if (ADSP_FAILED(err_code))
                {
                    MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                            "failed to allocate output buffers for port %lu.",
                            topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id, i);
                    return err_code;
                }
            }
        }

        uint32_t output_length = out_buf_ptr->size_per_buffer - out_buf_ptr->valid_data_end;
        uint32_t samples_in_output = topo_buffer_bytes_to_samples(out_buf_ptr, output_length);
        if (samples_in_output < min_samples)
        {
            min_samples = samples_in_output;
        }
#ifdef DBG_TOPO_HANDLER

MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
       "number of samples %lu for output port %lu.",
      topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id,samples_in_output, i);
#endif

        // update for output port threshold
        if(module_struct_ptr->output_ports_ptr[i].data_threshold > 1)
        {
           if(output_length < module_struct_ptr->output_ports_ptr[i].data_threshold) //  module can not process
           {
               MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                    "incorrect number of samples %lu left in output port %lu.",
                    topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id,samples_in_output, i);
               return ADSP_EBADPARAM;
           }
        }
        // Calculate the input and output lengths. The input length is the number of bytes provided to the module. The output
        // length is the number of bytes available for the module to fill in the output buffer.


        // Set up the input buffers
        if ((1 == module_struct_ptr->num_input_ports) && (1 == module_struct_ptr->num_output_ports))
        {
            topo_ptr->out_streams[i].flags = topo_ptr->in_streams[0].flags; // Default value
            topo_ptr->out_streams[i].timestamp = topo_ptr->in_streams[0].timestamp; // Default value
        }
        else
        {
            // TODO : Set flags and timestamps as per primary port?
            topo_ptr->out_streams[i].flags = init_stream_data.flags; // Default value
            topo_ptr->out_streams[i].timestamp = init_stream_data.timestamp; // Default value
        }
        topo_ptr->out_streams[i].bufs_num = out_buf_ptr->num_bufs;
        QURT_ELITE_ASSERT(out_buf_ptr->num_bufs <= CAPI_V2_MAX_CHANNELS);
        capi_v2_buf_t *out_bufs = topo_ptr->out_streams[i].buf_ptr;
        for(uint32_t j = 0; j < out_buf_ptr->num_bufs; j++)
        {
            out_bufs[j].actual_data_len = 0;
            out_bufs[j].max_data_len = output_length;
            out_bufs[j].data_ptr = out_buf_ptr->buf_ptrs[j] + out_buf_ptr->valid_data_end;
        }
    }

    // Adjust the input buffer length if the module does not requires data buffering. All input buffers must have the
    // same number of samples. Also all output buffers should have enough space for that many samples. So we
    // set the number of samples in each input buffer to be the minimum of the number of valid samples over
    // all input buffers and the number of samples that can be stored over all output buffers.
    if (!module_struct_ptr->requires_data_buffering)
    {
        for (uint32_t i = 0; i < module_struct_ptr->num_input_ports; i++)
        {
           if( (topo_is_port_or_stream_active(topo_ptr, i)) && (module_struct_ptr->input_ports_ptr[i].data_threshold <= 1))
           {
               const topo_port_index_t port_index = { module_index, i, topo_port_index_t::INPUT_PORT };

               topo_buf_t *in_buf_ptr = topo_module_get_input_buffer(topo_ptr, &port_index);
               uint32_t    input_length = topo_buffer_samples_to_bytes(in_buf_ptr, min_samples);
               capi_v2_buf_t *in_bufs = topo_ptr->in_streams[i].buf_ptr;
               for(uint32_t j = 0; j < in_buf_ptr->num_bufs; j++)
               {
                  in_bufs[j].actual_data_len = input_length;
               }
           }
        }
    }

    // Process data
    capi_v2_err_t err_code = CAPI_V2_EOK;
    capi_v2_t *m_ptr = module_struct_ptr->module_ptr;

#ifdef AUDPPSVC_TEST
    uint64_t prof_cycles = hexagon_sim_read_pcycles();
#endif // AUDPPSVC_TEST

    err_code = m_ptr->vtbl_ptr->process(m_ptr, topo_ptr->in_stream_ptrs, topo_ptr->out_stream_ptrs);

#ifdef AUDPPSVC_TEST
    {

        // Diff Current Cycle State against previously acquired to
        // check Cycles elapsed
        prof_cycles = hexagon_sim_read_pcycles() - prof_cycles;
        prof_cycles /= QURT_ELITE_MAX_HW_THREADS;

        const topo_port_index_t port_index = { module_index, 0, topo_port_index_t::OUTPUT_PORT } ;

        topo_buf_t *out_buf_ptr = topo_module_get_output_buffer(topo_ptr, &port_index);

        uint32_t sample_count = topo_ptr->out_stream_ptrs[0]->buf_ptr[0].actual_data_len / out_buf_ptr->bytes_to_samples_factor;

        // Add to Total Cycles
        // Ignoring the very first iteration of the processing
        if (module_struct_ptr->count > 0)
        {
            module_struct_ptr->prof_total_cycles += prof_cycles;
            module_struct_ptr->prof_num_samples += sample_count;

            // Maximum mips
            float mips_this_iteration = convert_cycles_to_mips(48000, sample_count, prof_cycles);
            if (mips_this_iteration > module_struct_ptr->prof_peak_mips)
            {
                module_struct_ptr->prof_peak_mips = mips_this_iteration;
            }
        }
        module_struct_ptr->count++;
    }
#endif// AUDPPSVC_TEST

    if(CAPI_V2_FAILED(err_code))
    {
        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                "failed in process with code %d. Dropping data.",
                topo_ptr->obj_id, module_struct_ptr->module_id, module_struct_ptr->instance_id, static_cast<int>(err_code));
        for (uint32_t i = 0; i < module_struct_ptr->num_input_ports; i++)
        {
            const topo_port_index_t port_index = { module_index, i, topo_port_index_t::INPUT_PORT };

            topo_buf_t *in_buf_ptr = topo_module_get_input_buffer(topo_ptr, &port_index);
            uint32_t    input_length = topo_buffer_samples_to_bytes(in_buf_ptr, min_samples);
            capi_v2_buf_t *in_bufs = topo_ptr->in_streams[i].buf_ptr;
            for(uint32_t j = 0; j < in_buf_ptr->num_bufs; j++)
            {
                in_bufs[j].actual_data_len = input_length;
            }
        }
    }

    bool_t fully_used_buffer_found = FALSE;

    // Update the offsets in the input buffers.
    for (uint32_t i = 0; i < module_struct_ptr->num_input_ports; i++)
    {
        const topo_port_index_t port_index = { module_index, i, topo_port_index_t::INPUT_PORT };

        topo_buf_t *in_buf_ptr = topo_module_get_input_buffer(topo_ptr, &port_index);
        capi_v2_buf_t *in_bufs = topo_ptr->in_streams[i].buf_ptr;

#ifdef DBG_TOPO_HANDLER

MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
          "number of bytes %lu for input port %lu. after process ",
          topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id,in_bufs[0].actual_data_len, i);
#endif

        in_buf_ptr->valid_data_start += in_bufs[0].actual_data_len;
        if (in_buf_ptr->valid_data_start > in_buf_ptr->valid_data_end)
        {
            MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Input buffer read beyond the boundary from Module-Instance with id 0x%lX-0x%lX, port %lu.",
                    topo_ptr->obj_id, module_struct_ptr->module_id, module_struct_ptr->instance_id, i);
            in_buf_ptr->valid_data_start = in_buf_ptr->valid_data_end;
        }
        if (topo_buffer_is_buf_empty(in_buf_ptr))
        {
            fully_used_buffer_found = TRUE;
            in_buf_ptr->valid_data_start = in_buf_ptr->valid_data_end = 0;
            topo_port_index_t prev_port_index = topo_module_prev_port(topo_ptr, &port_index);
            if((prev_port_index.module_index != TOPO_INVALID_MODULE_INDEX) && (prev_port_index.module_index != TOPO_INVALID_MODULE_INDEX_1))
            {
                topo_buf_manager_return_bufs(&topo_ptr->buf_mgr, in_buf_ptr);
            }
        }
        //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: TODO : checking buffering requirements", topo_ptr->obj_id);
        // There should not be any buffering between modules that don't require buffering except at the input of the last module.
        QURT_ELITE_ASSERT(module_struct_ptr->requires_data_buffering
                || ((topo_ptr->first_port[TOPO_PRIMARY_STRM].module_index == module_index) && (topo_ptr->first_port[TOPO_PRIMARY_STRM].port_index == i))
                || ((topo_ptr->first_port[TOPO_FAR_STRM].module_index == module_index) && (topo_ptr->first_port[TOPO_FAR_STRM].port_index == i))
                || ((topo_ptr->last_port.module_index == module_index) && (topo_ptr->last_port.port_index == i))
                || topo_buffer_is_buf_empty(in_buf_ptr));
        //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: TODO : buffering requirements fine", topo_ptr->obj_id);
    }

    // Update the offsets in the output buffers.
    for (uint32_t i = 0; i < module_struct_ptr->num_output_ports; i++)
    {
        const topo_port_index_t port_index = { module_index, i, topo_port_index_t::OUTPUT_PORT };

        topo_buf_t *out_buf_ptr = topo_module_get_output_buffer(topo_ptr, &port_index);

        capi_v2_buf_t *out_bufs = topo_ptr->out_streams[i].buf_ptr;

        out_buf_ptr->flags = topo_ptr->out_streams[i].flags;

        if((out_buf_ptr->flags.is_timestamp_valid) && (out_buf_ptr->valid_data_end == 0))
        {
            if(topo_ptr->in_streams[0].timestamp == topo_ptr->out_streams[i].timestamp)
            {
                if(out_buf_ptr->valid_data_end == 0)
                {
                    topo_ptr->out_streams[i].timestamp = topo_ptr->in_streams[0].timestamp
                            - topo_ptr->module[module_index].delay_in_us;
                }
            }
            out_buf_ptr->timestamp = topo_ptr->out_streams[i].timestamp;
        }

        out_buf_ptr->valid_data_end += out_bufs[0].actual_data_len;
        if (out_buf_ptr->valid_data_end > out_buf_ptr->size_per_buffer)
        {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Output buffer overflowed from Module-Instance with id 0x%lX-0x%lX.",
                    topo_ptr->obj_id, module_struct_ptr->module_id, module_struct_ptr->instance_id);
            out_buf_ptr->valid_data_end = out_buf_ptr->size_per_buffer;
        }
        if (topo_buffer_is_buf_full(out_buf_ptr))
        {
            fully_used_buffer_found = TRUE;
        }
        if (topo_buffer_is_buf_empty(out_buf_ptr))
        {
            if (
                    !(
                            (module_index == topo_ptr->last_port.module_index)
                            && (i == topo_ptr->last_port.port_index)
                    )
            )
            {
                topo_buf_manager_return_bufs(&topo_ptr->buf_mgr, out_buf_ptr);
            }
        }
    }

    // There should not be any buffering between modules that don't require buffering except at the input of the last module.
#ifdef DBG_TOPO_HANDLER
    if (!fully_used_buffer_found)
    {
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: No fully used buffer found after calling process for Module-Instance with id 0x%lX-0x%lX.",
				topo_ptr->obj_id, module_struct_ptr->module_id, module_struct_ptr->instance_id);
        QURT_ELITE_ASSERT(0);
    }
#endif

    return ADSP_EOK;
}

static bool_t topo_inplace_processing_possible(topo_struct *topo_ptr, uint32_t module_index)
{
    module_info *module_struct_ptr = &topo_ptr->module[module_index];

    if (!module_struct_ptr->is_in_place)
    {
        return FALSE;
    }

    if (module_struct_ptr->num_input_ports != 1)
    {
       return FALSE;
    }

    if (module_struct_ptr->num_output_ports != 1)
    {
       return FALSE;
    }

    const topo_port_index_t port_index = { module_index, 0, topo_port_index_t::OUTPUT_PORT};
    topo_buf_t *out_buf_ptr = topo_module_get_output_buffer(topo_ptr, &port_index);

    if (!topo_buffer_is_buf_empty(out_buf_ptr))
    {
        return FALSE;
    }

    if (module_index == topo_ptr->first_port[TOPO_PRIMARY_STRM].module_index || module_index == topo_ptr->first_port[TOPO_FAR_STRM].module_index)
    {
        return FALSE;
    }

    if (module_index == topo_ptr->last_port.module_index)
    {
        return FALSE;
    }

    return TRUE;
}

static ADSPResult topo_process_module_inplace(topo_struct *topo_ptr, uint32_t module_index)
{
   QURT_ELITE_ASSERT(1 == topo_ptr->module[module_index].num_input_ports);
   QURT_ELITE_ASSERT(1 == topo_ptr->module[module_index].num_output_ports);

   topo_port_index_t port_index = { module_index, 0, topo_port_index_t::INPUT_PORT };
   topo_buf_t *in_buf_ptr = topo_module_get_input_buffer(topo_ptr, &port_index);
   port_index.type = topo_port_index_t::OUTPUT_PORT;
   topo_buf_t *out_buf_ptr = topo_module_get_output_buffer(topo_ptr, &port_index);

    module_info *module_struct_ptr = &topo_ptr->module[module_index];

    // Copy the buffer data from the input buffers to the output buffers.
    *out_buf_ptr = *in_buf_ptr;
    out_buf_ptr->valid_data_end = out_buf_ptr->valid_data_start; // Output buffer is empty at this point.

    // Calculate the input and output length. The input length is the number of bytes provided to the module. The output
    // length is the number of bytes available for the module to fill in the output buffer.
    uint32_t process_length = in_buf_ptr->valid_data_end - in_buf_ptr->valid_data_start;

    // Set up the input buffers
    QURT_ELITE_ASSERT(1 <= topo_ptr->max_in_streams);
   topo_ptr->in_streams[0].flags = in_buf_ptr->flags;
   topo_ptr->in_streams[0].timestamp = in_buf_ptr->timestamp;
   topo_ptr->in_streams[0].bufs_num = in_buf_ptr->num_bufs;
   QURT_ELITE_ASSERT(in_buf_ptr->num_bufs <= CAPI_V2_MAX_CHANNELS);
   capi_v2_buf_t *in_bufs = topo_ptr->in_streams[0].buf_ptr;
   for(uint32_t j = 0; j < in_buf_ptr->num_bufs; j++)
   {
       in_bufs[j].actual_data_len = process_length;
       in_bufs[j].max_data_len = in_buf_ptr->size_per_buffer - in_buf_ptr->valid_data_start;
       in_bufs[j].data_ptr = in_buf_ptr->buf_ptrs[j] + in_buf_ptr->valid_data_start;
   }

    // Set up the output buffers
   topo_ptr->out_streams[0].flags = topo_ptr->in_streams[0].flags; // Default value
   topo_ptr->out_streams[0].timestamp = topo_ptr->in_streams[0].timestamp; // Default value
   topo_ptr->out_streams[0].bufs_num = out_buf_ptr->num_bufs;
   QURT_ELITE_ASSERT(out_buf_ptr->num_bufs <= CAPI_V2_MAX_CHANNELS);
   capi_v2_buf_t *out_bufs = topo_ptr->out_streams[0].buf_ptr;
   for(uint32_t j = 0; j < out_buf_ptr->num_bufs; j++)
   {
       out_bufs[j].actual_data_len = 0;
       out_bufs[j].max_data_len = process_length;
       out_bufs[j].data_ptr = out_buf_ptr->buf_ptrs[j] + out_buf_ptr->valid_data_end;
   }

    // Process data
    capi_v2_err_t err_code = CAPI_V2_EOK;
    capi_v2_t *m_ptr = module_struct_ptr->module_ptr;
    err_code = m_ptr->vtbl_ptr->process(m_ptr, topo_ptr->in_stream_ptrs, topo_ptr->out_stream_ptrs);
    if(CAPI_V2_FAILED(err_code))
    {
        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                "failed in process with code %d. Passing data through.",
                topo_ptr->obj_id, module_struct_ptr->module_id, module_struct_ptr->instance_id, static_cast<int>(err_code));

       capi_v2_buf_t *in_bufs = topo_ptr->in_streams[0].buf_ptr;
       for(uint32_t j = 0; j < in_buf_ptr->num_bufs; j++)
       {
           in_bufs[j].actual_data_len = process_length;
       }
       capi_v2_buf_t *out_bufs = topo_ptr->out_streams[0].buf_ptr;
       for(uint32_t j = 0; j < in_buf_ptr->num_bufs; j++)
       {
           out_bufs[j].actual_data_len = process_length;
       }
    }

    if ((in_buf_ptr->valid_data_start + in_bufs[0].actual_data_len) > in_buf_ptr->valid_data_end)
    {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Input data buffer overflowed for the inplace Module-Instance with id 0x%lX-0x%lX.",
                topo_ptr->obj_id, module_struct_ptr->module_id, module_struct_ptr->instance_id);
    }
    if ((in_buf_ptr->valid_data_start + in_bufs[0].actual_data_len) < in_buf_ptr->valid_data_end)
    {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Input data is not fully consumed for the inplace Module-Instance with id 0x%lX-0x%lX. "
                "The rest of the data will be dropped.", topo_ptr->obj_id, module_struct_ptr->module_id, module_struct_ptr->instance_id);
    }

    out_buf_ptr->flags = topo_ptr->out_streams[0].flags;
    out_buf_ptr->timestamp = topo_ptr->out_streams[0].timestamp;
    out_buf_ptr->valid_data_end += out_bufs[0].actual_data_len;
    if (out_buf_ptr->valid_data_end > out_buf_ptr->size_per_buffer)
    {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Output buffer overflowed from Module-Instance with id 0x%lX-0x%lX.",
                topo_ptr->obj_id, module_struct_ptr->module_id, module_struct_ptr->instance_id);
        out_buf_ptr->valid_data_end = out_buf_ptr->size_per_buffer;
    }
    if (out_buf_ptr->valid_data_end != in_buf_ptr->valid_data_end)
    {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Output data is not equal to input for the inplace Module-Instance with id 0x%lX-0x%lX.",
                topo_ptr->obj_id, module_struct_ptr->module_id, module_struct_ptr->instance_id);
        out_buf_ptr->valid_data_end = in_buf_ptr->valid_data_end;
    }

    // Set all the input buffers to NULL since the pointer have now been moved to the output buffer. Also the input buffer is now empty.
    in_buf_ptr->valid_data_start = in_buf_ptr->valid_data_end = 0;
    for (uint32_t i = 0; i < in_buf_ptr->num_bufs; i++)
    {
        in_buf_ptr->buf_ptrs[i] = NULL;
    }

    return ADSP_EOK;
}

/**
 * Function will set the input media format event for the first module of the topology.
 * If no module is present in the topology, it will set the event to the output media format.
 *   - Dependencies: None.
 *   - Side Effects: None.
 *   - Re-entrant: Yes
 *
 * @param[in, out] obj_ptr
 *   Pointer to the object structure.
 *
static void topo_set_input_media_fmt_event (topo_struct *topo_ptr)
{
   topo_media_fmt_t in_format;
   topo_adsp_media_fmt_to_topo(&topo_ptr->in_format, &in_format);

   topo_port_index_t main_input = topo_graph_get_main_input(&topo_ptr->graph);

   // if no module is present in the topology, set output media format event
   if(main_input.module_index == TOPO_INVALID_MODULE_INDEX)
   {
      topo_media_fmt_t out_format;
      topo_adsp_media_fmt_to_topo(&topo_ptr->out_format, &out_format);

      if(!topo_is_equal_media_format(&in_format, &out_format))
      {
         topo_topo_media_fmt_to_adsp(&in_format, &topo_ptr->out_format);
         topo_print_media_format(topo_ptr->obj_id, &in_format);

         topo_raise_output_media_type_event(topo_ptr);
      }
   }
   else
   {
      topo_media_event_t *downstream_media_event = &topo_ptr->module[main_input.module_index].input_ports_ptr[main_input.port_index].media_event;
      topo_set_media_event(downstream_media_event, &in_format);
   }

}
*/
/**
 * Function to change the input media type.
 *   - Dependencies: None
 *   - Side Effects: Will cause some modules to reinitialize
 *     their internal states.
 *   - Re-entrant: Yes
 *
 * @param[in, out] obj_ptr
 *   Pointer to the object structure.
 * @param[in] pInMediaFormat
 *   The input media format.
 *
 */
void topo_change_input_media_type (topo_struct *topo_ptr, const AdspAudioMediaFormatInfo_t *pInMediaFormat,uint32_t port_id)
{
#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Change Input Mediatype Begin", topo_ptr->obj_id);
#endif

    if (!topo_ptr->data_format_received[TOPO_PRIMARY_STRM] && topo_is_primary_in_port(port_id))
    {
        topo_setup_connections(topo_ptr);
        topo_ptr->data_format_received[TOPO_PRIMARY_STRM] = TRUE;
    }
    else if(!topo_ptr->data_format_received[TOPO_PRIMARY_STRM] && !topo_is_primary_in_port(port_id))
    {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc in_port %x: Topology Change Input Mediatype first attempted on non-primary port", topo_ptr->obj_id,port_id);
        return;
    }
    else
    {
        topo_ptr->data_format_received[port_id] = TRUE;
    }

    topo_ptr->in_format[port_id] = *pInMediaFormat;

    // When far end media media format is received and we have ec framework extension, configure format converter with near end media format
    if(topo_ptr->ec_extn_info && (port_id == TOPO_FAR_STRM))
    {
        // Currently required media format is matching primary stream input format, this may change later
        module_info *ec_module_info = &topo_ptr->module[topo_ptr->ec_extn_info->module_index];
        AdspAudioMediaFormatInfo_t req_media_format = topo_ptr->in_format[TOPO_PRIMARY_STRM];
        topo_topo_media_fmt_to_adsp(&ec_module_info->input_ports_ptr[TOPO_PRIMARY_STRM].media_event.event, &req_media_format);
        // Channels are reverted to far end if multi-channel EC ref is supported
        if(topo_ptr->ec_extn_info->ec_capabilities.ecns_multichannel_ref_info.is_multichannel_ref_supported)
        {
            req_media_format.channels = topo_ptr->in_format[TOPO_FAR_STRM].channels;
            for(uint8_t ch_index = 0;ch_index < req_media_format.channels;ch_index++)
            {
                req_media_format.channelMapping[ch_index] = topo_ptr->in_format[TOPO_FAR_STRM].channelMapping[ch_index];
            }
        }
        else
        {
            req_media_format.channels = 1;
            req_media_format.channelMapping[0] = PCM_CHANNEL_C;
        }
        topo_set_ec_far_media_format(topo_ptr,&req_media_format);
        topo_ptr->ec_extn_info->is_far_port_active = TRUE;
    }

    // Passing TOPO_INVALID_MODULE_INDEX to this function causes it to set the input media type of the first module.
    // Media format events are automatically propagated, so this will cause the media format to go down the list of modules.
    topo_handle_pending_events(topo_ptr, TOPO_INVALID_MODULE_INDEX,port_id);


#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Change Input Mediatype End", topo_ptr->obj_id);
#endif
}

/**
 * Function to clear the internal history for all modules. This
 * does not reset the module parameters to their defaults.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] obj_ptr
 *   Pointer to the object structure.
 *
 * @return void
 */
static void topo_set_reset_each_module (topo_struct *topo_ptr)
{
   // The reset set property does not need any parameters.
   capi_v2_prop_t prop;
   prop.id = CAPI_V2_ALGORITHMIC_RESET;
   prop.payload.actual_data_len = 0;
   prop.payload.max_data_len = 0;
   prop.payload.data_ptr = NULL;
   prop.port_info.is_valid = FALSE;
   capi_v2_proplist_t proplist;
   proplist.props_num = 1;
   proplist.prop_ptr = &prop;

   // Loop through all the modules and reset each.
   for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
   {
      capi_v2_err_t err_code;
      capi_v2_t *module_ptr = topo_ptr->module[i].module_ptr;

      err_code = module_ptr->vtbl_ptr->set_properties(module_ptr, &proplist);
      if (CAPI_V2_FAILED(err_code))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with ID %lX "
                 "failed to reset.", topo_ptr->obj_id, topo_ptr->module[i].module_id, topo_ptr->module[i].instance_id);
      }
   }
}

/**
 * Function to clear the internal history for all modules. This
 * does not reset the module parameters to their defaults.
 * This function only sets the internal buffers in the topology to zero
 * and does not return them
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] obj_ptr
 *   Pointer to the object structure.
 *
 * @return ADSPResult
 *   Error code returning status.
 */
void topo_pause_reset (topo_struct *topo_ptr)
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Pause Reset Begin", topo_ptr->obj_id);

   // Loop through all the modules and reset each.
   topo_set_reset_each_module(topo_ptr);

   for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
   {
      for (uint32_t j = 0; j < topo_ptr->module[i].num_output_ports; j++)
      {
         topo_buf_t *buf_ptr = &topo_ptr->module[i].output_ports_ptr[j].topo_buf;
         if (!topo_buffer_is_buf_empty(buf_ptr))
         {
            for( uint32_t k = 0; k < buf_ptr->num_bufs; k++)
            {
               memset(buf_ptr->buf_ptrs[k] + buf_ptr->valid_data_start ,
                     0 ,
                     buf_ptr->valid_data_end - buf_ptr->valid_data_start);
            }
         }
      }
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Pause Reset End", topo_ptr->obj_id);
}

/**
 * Function to clear the internal history for all modules. This
 * does not reset the module parameters to their defaults.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] obj_ptr
 *   Pointer to the object structure.
 *
 * @return ADSPResult
 *   Error code returning status.
 */
void topo_reset (topo_struct *topo_ptr)
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Flush Reset Begin", topo_ptr->obj_id);

   // Loop through all the modules and reset each.
   topo_set_reset_each_module(topo_ptr);

   for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
   {
      for (uint32_t j = 0; j < topo_ptr->module[i].num_output_ports; j++)
      {
         topo_buf_t *buf_ptr = &topo_ptr->module[i].output_ports_ptr[j].topo_buf;
         if (!topo_buffer_is_buf_empty(buf_ptr))
         {
            topo_buf_manager_return_bufs(&topo_ptr->buf_mgr, buf_ptr);
            buf_ptr->valid_data_start = buf_ptr->valid_data_end = 0;
         }
      }
   }

   // If Internal buffer is created for a real time input-output use case involving fractional resampling,
   // the data in it has to be cleared.
   if(NULL != topo_ptr->internal_buf.pBuf)
   {
       uint32_t internal_buf_size = topo_ptr->internal_buf.buf_size;
       memset(topo_ptr->internal_buf.pBuf,0,internal_buf_size);
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Flush Reset End", topo_ptr->obj_id);
}

/**
 * Set the params for the modules in this topology.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] obj_ptr
 *   Pointer to the object structure.
 * @param[in] pPayload
 *   Pointer to the payload of the set params data.
 * @param[in] payloadSize
 *   Size of the payload of the set params data, in bytes.
 * @param[in] mode
 *    FIRST_INSTANCE: Set-Param is done to first instance of a given module.
 *    ZERO_INSTANCE:    Set-Param is done to instance with zero ID of a given module.
 *    ALL_INSTANCES:  Set-Param is done to all the instances of a given module.
 *    GIVEN_INSTANCE: Set-Param is done to given instance of a given module.
 *
 * @return ADSPResult
 *   Error code returning status.
 */
ADSPResult topo_set_param (topo_struct *topo_ptr, void *pPayload, uint32_t payloadSize, topo_set_get_param_mode_t mode)
{
#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Set params Begin.", topo_ptr->obj_id);
#endif
   ADSPResult result = ADSP_EOK;
   uint8_t *pPacket = (uint8_t *)pPayload;
   bool_t  is_ec_set_param = FALSE;

   if(!IS_ALIGN_4_BYTE(pPacket))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Set param pointer is not aligned to 4 bytes. Aborting.", topo_ptr->obj_id);
      return ADSP_EFAILED;
   }

   if(mode >= UNDEFINED_INSTANCE )
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Set param mode is not supported.");
       return ADSP_EFAILED;
   }

   while (payloadSize > 0)
   {
      uint32_t req_payload_size = (mode == GIVEN_INSTANCE)? sizeof(asm_stream_param_data_v3_t) : sizeof(asm_stream_param_data_v2_t);
      if (payloadSize < req_payload_size)
      {
         break;
      }

      uint32_t moduleID;
      uint32_t instanceID;
      uint32_t paramSize;
      uint32_t payloadHeaderSize;
      uint32_t paramID;
      if(mode == GIVEN_INSTANCE)
      {
          asm_stream_param_data_v3_t *pDataPayload = (asm_stream_param_data_v3_t *)(pPacket);
          moduleID = pDataPayload->module_id;
          instanceID = (uint32_t)pDataPayload->instance_id;
          paramSize = pDataPayload->param_size;
          paramID = pDataPayload->param_id;
          payloadHeaderSize = sizeof(asm_stream_param_data_v3_t);
      }
      else
      {
          uint32_t index = 0;
          asm_stream_param_data_v2_t *pDataPayload = (asm_stream_param_data_v2_t *)(pPacket);
          moduleID = pDataPayload->module_id;
          instanceID = 0;
          paramSize = pDataPayload->param_size;
          paramID = pDataPayload->param_id;
          payloadHeaderSize = sizeof(asm_stream_param_data_v2_t);
          if (mode != ZERO_INSTANCE && ADSP_SUCCEEDED(topo_find_first_module_instance(topo_ptr,moduleID,&index)))
          {
              //Instance ID of first instance of the module.
              instanceID = topo_ptr->module[index].instance_id;
          }
      }

      if(topo_ptr->ec_extn_info)
          {
        if(moduleID == topo_ptr->ec_extn_info->module_id)
             {
              is_ec_set_param = TRUE;
             }
          }

      req_payload_size = payloadHeaderSize + paramSize;
      if (payloadSize < req_payload_size)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Insufficient size, "
                 "payload size=%lu, req payload size=%hu.", topo_ptr->obj_id, payloadSize, req_payload_size);
         result = ADSP_ENEEDMORE;
         break;
      }

      uint32_t index;
      ADSPResult errCode = topo_find_module_instance(topo_ptr, moduleID, instanceID, &index);
      do
      {
          if (ADSP_SUCCEEDED(errCode))
          {
             capi_v2_err_t err_code = CAPI_V2_EOK;
             capi_v2_buf_t buf;
             buf.data_ptr = (int8_t *) (pPacket + payloadHeaderSize);
             buf.max_data_len = paramSize;
             buf.actual_data_len = paramSize;
             capi_v2_t *module_ptr = topo_ptr->module[index].module_ptr;
             capi_v2_port_info_t port_info;
             port_info.is_valid = FALSE;

             instanceID = topo_ptr->module[index].instance_id;

             err_code = module_ptr->vtbl_ptr->set_param(module_ptr, paramID, &port_info, &buf);
             if (CAPI_V2_FAILED(err_code))
             {
                MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with ID 0x%lX-0x%lX failed setParam.",
                        topo_ptr->obj_id, moduleID, instanceID);
                result |= capi_v2_err_to_adsp_result(err_code);
             }
             if (moduleID ==  AUDPROC_MODULE_ID_VOL_CTRL)
             {
                 if (paramID ==  AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS)
                 {
                     topo_set_ramp_period_to_virtualizer(topo_ptr, index);
                 }
             }
          }
          else
          {
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Attempted set param "
                       "to unsupported Module-Instance with ID 0x%lx-0x%lx!",
                       topo_ptr->obj_id, moduleID, instanceID);
               break;
          }
      }while( (mode == ALL_INSTANCES) && (ADSP_SUCCEEDED(topo_find_next_module_instance(topo_ptr, moduleID, &index))) );

      pPacket += req_payload_size;
      payloadSize -= req_payload_size;

      if (((paramSize % 4) != 0) && (payloadSize > 0))
      {
         MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: sest param size for Module-Instance with ID 0x%lx-0x%lx, Param ID 0x%lx "
                 "is %lu which is not a multiple of 4. Aborting further processing.", topo_ptr->obj_id, moduleID, instanceID, paramID, paramSize);
         result = ADSP_EFAILED;
         break;
      }
   }

   if((is_ec_set_param) && (topo_ptr->ec_extn_info))
   {
     uint32_t ec_index = topo_ptr->ec_extn_info->module_index;
     do
     {
         capi_v2_prop_t prop;
         capi_v2_err_t err_code;

         prop.id = CAPI_V2_ALGORITHMIC_RESET;
         prop.payload.actual_data_len = 0;            // TODO actual_data_len data len
         prop.payload.max_data_len = 0;               // TODO max data len
         prop.payload.data_ptr = NULL;
         prop.port_info.is_valid = FALSE;
         capi_v2_proplist_t proplist;
         proplist.props_num = 1;
         proplist.prop_ptr = &prop;


         capi_v2_t *module_ptr = topo_ptr->module[ec_index].module_ptr;

          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_set_param: send algo reset to module id %lu", topo_ptr->obj_id, topo_ptr->module[ec_index].module_id);

         err_code = module_ptr->vtbl_ptr->set_properties(module_ptr, &proplist);
         if (CAPI_V2_FAILED(err_code))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with ID %lX "
                 "failed to reset.", topo_ptr->obj_id, topo_ptr->module[ec_index].module_id, topo_ptr->module[ec_index].instance_id);
         }

       }while( (mode == ALL_INSTANCES) && (ADSP_SUCCEEDED(topo_find_next_module_instance(topo_ptr, topo_ptr->ec_extn_info->module_id, &ec_index))) );

   }

#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Set params End.", topo_ptr->obj_id);
#endif
   return result;
}

/**
 * Get the params for one module in this topology.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] obj_ptr
 *   Pointer to the object structure.
 * @param[in] pPayload
 *   Pointer to the payload of the get params data.
 * @param[in] payloadSize
 *   Size of the payload of the get params data, in bytes.
 * @param[in] module_id
 *   The module id for the module whose paramters are to be
 *   read.
 * @param[in] instance_id
 *   The instance id of a module instance whose paramters are to be
 *   read.
 * @param[in] paramId
 *   The param id for the parameters to be read.
 * @param[out] pParamSize
 *   The amount of data filled in the payload buffer, in bytes.
 * @param[in] mode
 *    FIRST_INSTANCE: Get-Param is done to first instance of a given module.
 *    ZERO_INSTANCE:    Get-Param is done to instance with zero ID of a given module.
 *    GIVEN_INSTANCE: Get-Param is done to given instance of a given module.
 *
 * @return ADSPResult
 *   Error code returning status.
 */
ADSPResult topo_get_param (topo_struct *topo_ptr, void *pPayload, const uint16_t payloadSize, const uint32_t module_id, const uint32_t instance_id, const uint32_t paramId, uint32_t *pParamSize, topo_set_get_param_mode_t mode)
{
   ADSPResult result = ADSP_EFAILED;

   // The PP service calls this function during steady state for the sample slip module. Don't print the message in that case.
#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
   if(paramId != AUDPROC_PARAM_ID_SAMPLESLIP_SAMPLE_ADJUST)
   {
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Get params Begin.", topo_ptr->obj_id);
   }
#endif

   uint32_t index = 0;
   switch(mode)
   {
      case GIVEN_INSTANCE:
         result = topo_find_module_instance(topo_ptr, module_id, instance_id, &index);
      break;

      case FIRST_INSTANCE:
         result = topo_find_first_module_instance(topo_ptr, module_id, &index);
      break;

      case ZERO_INSTANCE:
         result = topo_find_module_instance(topo_ptr, module_id, 0, &index);
      break;

      default:
         result = ADSP_EUNSUPPORTED;
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: "
                 "Invalid mode received for get-param.", topo_ptr->obj_id);
      break;
   }

   if (ADSP_SUCCEEDED(result))
   {
      capi_v2_err_t err_code;
      capi_v2_buf_t buf;
      buf.data_ptr = (int8_t *) pPayload;
      buf.max_data_len = payloadSize;
      capi_v2_t *module_ptr = topo_ptr->module[index].module_ptr;
      capi_v2_port_info_t port_info;
      port_info.is_valid = FALSE;

      err_code = module_ptr->vtbl_ptr->get_param(module_ptr, paramId, &port_info, &buf);
      if (CAPI_V2_FAILED(err_code))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: "
            "Module with ID 0x%lX-0x%lX failed getParam.", topo_ptr->obj_id,
            module_id, topo_ptr->module[index].instance_id);
      }

      *pParamSize = buf.actual_data_len;

      result = capi_v2_err_to_adsp_result(err_code);
   }
   else
   {
      if(paramId != AUDPROC_PARAM_ID_SAMPLESLIP_SAMPLE_ADJUST)
      {
          MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: "
                  "Topology Attempt to get unsupported module ID 0x%lx-0x%lx!", topo_ptr->obj_id, module_id, instance_id);
      }
      result = ADSP_EBADPARAM;
   }

   // The PP service calls this function during steady state for the sample slip module. Don't print the message in that case.
#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
   if(paramId != AUDPROC_PARAM_ID_SAMPLESLIP_SAMPLE_ADJUST)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Get params End.", topo_ptr->obj_id);
   }
#endif
   return result;
}

/**
 * Destroys the topology object. After this function is called,
 * the topology object pointer and the vtable pointer become
 * invalid.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] obj_ptr
 *   Pointer to the object structure.
 */
void topo_destroy (topo_struct *topo_ptr)
{
   if (NULL != topo_ptr)
   {
#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Destroy Begin.", topo_ptr->obj_id);
#endif


#ifdef AUDPPSVC_TEST
      {
         uint32_t num_modules = topo_graph_num_modules(&topo_ptr->graph);
          MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"*** MIPS per MODULE: Peak MIPS indicates only MIPS for the iteration with max cycles ***");
          MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"*** MIPS per MODULE: These numbers might not be gold standard, meant for quick ref only ***");
          for (uint32_t i = 0; i < num_modules; i++)
          {
              module_info* module_info_ptr = &topo_ptr->module[i];
              MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Module-Instance with ID: 0x%lx-0x%lx, Average MIPS: %.3f",
                      module_info_ptr->module_id, module_info_ptr->instance_id,
                      convert_cycles_to_mips(48000, module_info_ptr->prof_num_samples, module_info_ptr->prof_total_cycles));
              MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Module-Instance with ID: 0x%lx-0x%lx, Peak MIPS: %.3f",
                      module_info_ptr->module_id, module_info_ptr->instance_id, module_info_ptr->prof_peak_mips);
          }
      }
#endif // AUDPPSVC_TEST

      if(NULL != topo_ptr->module)
      {
         topo_destroy_modules(topo_graph_num_modules(&topo_ptr->graph), topo_ptr->module, &topo_ptr->buf_mgr);
         qurt_elite_memory_free(topo_ptr->module);
      }

      topo_deallocate_stream_structs(topo_ptr);
      topo_graph_deinit(&topo_ptr->graph);
      topo_buf_manager_deinit(&topo_ptr->buf_mgr);

      if(topo_ptr->ec_extn_info)
      {
          if(topo_ptr->ec_extn_info->is_far_moudle_in_chain)
          {
              qurt_elite_memory_free(topo_ptr->ec_extn_info->is_far_moudle_in_chain);
              topo_ptr->ec_extn_info->is_far_moudle_in_chain = NULL;
          }
          qurt_elite_memory_free(topo_ptr->ec_extn_info);
          topo_ptr->ec_extn_info = NULL;
      }

      if (NULL != topo_ptr->eos_buf.pBuf)
      {
         qurt_elite_memory_free(topo_ptr->eos_buf.pBuf);
         topo_ptr->eos_buf.pBuf = NULL;
         topo_ptr->eos_buf.sizeInSamples = 0;
         topo_ptr->eos_buf.offsetInSamples = 0;
      }

      topo_internal_buf_destroy_internal_buffer(topo_ptr);

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology Destroy End, now freeing Topology.", topo_ptr->obj_id);
      qurt_elite_memory_free(topo_ptr);
   }
}

static ADSPResult find_first_module_instance_from_index(const topo_struct *topo_ptr, const uint32_t module_id, uint32_t *index_ptr)
{
    uint32_t search_module_id = topo_get_search_module_id(module_id);

    uint32_t num_modules = topo_graph_num_modules(&topo_ptr->graph);
    for (uint32_t i = *index_ptr; i < num_modules; i++)
    {
        if (search_module_id == topo_ptr->module[i].module_id)
        {
            *index_ptr = i;
            return ADSP_EOK;
        }
    }

    return ADSP_ENOTEXIST;
}

/**
 * Returns the index of a first instance of a module from the module-instance structure
 * on module ID.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] topo_ptr
 *   Pointer to the topology structure.
 * @param[in] module_id
 *   The module ID of the module to be found.
 * @param[out] index_ptr
 *   The index of the module in the modules array.
 *
 * @return ADSPResult
 *   ADSP_EOK if the module was found.
 *   ADSP_ENOTEXIST if the module was not found.
 */
static ADSPResult topo_find_first_module_instance(const topo_struct *topo_ptr, const uint32_t module_id, uint32_t *index_ptr)
{
    uint32_t index = 0;
        ADSPResult result = ADSP_EFAILED;
    uint32_t secondary_module_id = module_id;


    if ( module_id == VOICEPROC_MODULE_TX)
    {
        if(topo_ptr->ec_extn_info && topo_ptr->ec_extn_info->module_id)
            secondary_module_id = topo_ptr->ec_extn_info->module_id;
        else
               return result;
    }

    result = find_first_module_instance_from_index(topo_ptr, secondary_module_id, &index);
    if(ADSP_SUCCEEDED(result)){
        *index_ptr = index;
    }
    return result;
}

/**
 * Returns the index of a next instance of a module from the module-instance structure
 * on module ID.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] topo_ptr
 *   Pointer to the topology structure.
 * @param[in] module_id
 *   The module ID of the module to be found.
 * @param[in-out] index_ptr
 *   input: index to start search.
 *   output:index of next instance.
 *
 * @return ADSPResult
 *   ADSP_EOK if the module was found.
 *   ADSP_ENOTEXIST if the module was not found.
 */
static ADSPResult topo_find_next_module_instance(const topo_struct *topo_ptr, const uint32_t module_id, uint32_t *index_ptr)
{
    uint32_t index = (int32_t)(*index_ptr + 1);
    ADSPResult result = find_first_module_instance_from_index(topo_ptr, module_id, &index);
    if(ADSP_SUCCEEDED(result)){
        *index_ptr = index;
    }
    return result;
}

/**
 * Returns the index of a instance of a module from the module-instance structure
 * on module ID.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] topo_ptr
 *   Pointer to the topology structure.
 * @param[in] module_id
 *   The module ID of the module to be found.
 * @param[in] instance_id
 *   The instance ID of the module to be found.
 * @param[out] index_ptr
 *   The index of the module in the modules array.
 *
 * @return ADSPResult
 *   ADSP_EOK if the module was found.
 *   ADSP_ENOTEXIST if the module was not found.
 */
static ADSPResult topo_find_module_instance(const topo_struct *topo_ptr, const uint32_t module_id, const uint32_t instance_id, uint32_t *index_ptr)
{
    ADSPResult result = ADSP_EFAILED;
    uint32_t index = 0;
    uint32_t secondary_module_id = module_id;

    if ( module_id == VOICEPROC_MODULE_TX)
    {
        if(topo_ptr->ec_extn_info && topo_ptr->ec_extn_info->module_id)
            secondary_module_id = topo_ptr->ec_extn_info->module_id;
        else
            return result;
    }

    result = find_first_module_instance_from_index(topo_ptr, secondary_module_id, (uint32_t*)&index);

    while(ADSP_SUCCEEDED(result) && (topo_ptr->module[index].instance_id != instance_id))
    {
        index++;
        result = find_first_module_instance_from_index(topo_ptr, secondary_module_id, (uint32_t*)&index);
    }

    if (ADSP_SUCCEEDED(result) && topo_ptr->module[index].instance_id == instance_id){
        *index_ptr = index;
    }
    return result;
}

static ADSPResult create_delay_buffer(topo_struct* topo_ptr)
{
    AdspAudioMediaFormatInfo_t primaryInpMediaFmt = topo_ptr->in_format[TOPO_PRIMARY_STRM];
   uint32_t bytes_per_sample = topo_get_bytes_per_sample(primaryInpMediaFmt.bitsPerSample);

   // Calculate the total delay of all the modules in the chain. This is different from the value in delay_ptr, which also includes
   // the buffering delay of the PP service.
   uint32_t alg_delay = 0;
   for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
   {
       if (topo_ptr->module[i].is_in_chain)
       {
           alg_delay += topo_ptr->module[i].delay_in_us;
       }
   }


   if(0 != alg_delay)
   {
      uint32_t delay_in_samples = topo_convert_us_to_samples(alg_delay, primaryInpMediaFmt.samplingRate);
      uint32_t bytes_to_alloc = delay_in_samples * bytes_per_sample * primaryInpMediaFmt.channels;

      int8_t* ptr = (int8_t *)qurt_elite_memory_malloc(bytes_to_alloc, QURT_ELITE_HEAP_DEFAULT);
      if (!ptr)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: Out of Memory while creating delay buffer!");
         return ADSP_ENOMEMORY;
      }
      memset(ptr, 0, bytes_to_alloc);

      topo_ptr->eos_buf.pBuf= ptr;
      topo_ptr->eos_buf.sizeInSamples = delay_in_samples; // Per channel
      topo_ptr->eos_buf.offsetInSamples = 0;
   }
   else
   {
      topo_ptr->eos_buf.pBuf = NULL;
      topo_ptr->eos_buf.sizeInSamples = 0;
      topo_ptr->eos_buf.offsetInSamples = 0;
   }

   return ADSP_EOK;
}

#ifdef AUDPPSVC_TEST
static float convert_cycles_to_mips(const uint32_t sample_rate, const uint32_t num_samples, const uint64_t cycles)
{
   const uint32_t MIN_SAMPLE_COUNT = 50; // If the samples are less than this, the calculation may not be accurate
   if (num_samples < MIN_SAMPLE_COUNT)
   {
      return 0;
   }

   float rate = sample_rate;
   float num_seconds = num_samples / rate;
   float cycles_per_second = cycles / num_seconds;
   float mips = cycles_per_second / 1e6;

   return mips;
}
#endif // AUDPPSVC_TEST

static uint32_t topo_convert_us_to_samples(const uint32_t val_in_us, const uint32_t sample_rate)
{
   uint64_t val = val_in_us;
   val *= sample_rate;

   return uint32_t(div_round(val, NUM_US_IN_SEC));
}

static uint64_t div_round(const uint64_t x, const uint32_t y)
{
   return (x + y/2) / y;
}

bool_t topo_is_module_present(topo_struct *topo_ptr, const uint32_t module_id)
{
   uint32_t index;
   ADSPResult result = topo_find_first_module_instance(topo_ptr, module_id, &index);

   return ADSP_SUCCEEDED(result);
}

bool_t topo_buffer_is_buf_empty(const topo_buf_t *buf_ptr)
{
    return (buf_ptr->valid_data_end == buf_ptr->valid_data_start);
}

bool_t topo_buffer_is_buf_full(const topo_buf_t *buf_ptr)
{
    return (buf_ptr->valid_data_end == buf_ptr->size_per_buffer);
}

static bool_t topo_buffer_is_req_samples_present(topo_struct *topo_ptr, uint32_t module_index, uint32_t port_id)
{
   const topo_port_index_t current_in_port = { module_index, port_id, topo_port_index_t::INPUT_PORT };
   topo_buf_t *buf_ptr = topo_module_get_input_buffer(topo_ptr, &current_in_port);
   uint32_t data_present_in_buffer_in_bytes;
   module_input_port_t *input_ports_ptr  = &topo_ptr->module[module_index].input_ports_ptr[port_id];

   //capi_v2_standard_data_format_t *format_ptr = &input_ports_ptr->media_event.event.f;

   if(topo_buffer_is_buf_empty(buf_ptr))
   {
       return FALSE;
   }
   else if(input_ports_ptr->data_threshold > 1)
   {
      data_present_in_buffer_in_bytes = (buf_ptr->valid_data_end - buf_ptr->valid_data_start);

      if(data_present_in_buffer_in_bytes < input_ports_ptr->data_threshold) // check data threshold per  buffer and data present in bytes per buffer
      {
#ifdef DBG_TOPO_HANDLER

         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_buffer_is_req_samples_present: Module-Instance with id 0x%lX-0x%lX. requires more data on port %lu",
                topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id, port_id);
#endif
         input_ports_ptr->is_data_available = FALSE;  // TODO: take it outside this function
         return FALSE;
      }
   }
   return TRUE;
}


static void topo_buffer_convert_from_ext_input_buffer(AudPP_BufInfo_t *ext_buf_ptr, topo_buf_t *int_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr)
{
   uint32_t bytes_per_sample = topo_get_bytes_per_sample(format_ptr->bitsPerSample);

     int_buf_ptr->flags = (ext_buf_ptr->flags);
     int_buf_ptr->timestamp = ext_buf_ptr->timestamp;

    switch (format_ptr->bitstreamFormat)
    {
    case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2:
    case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3:
    {
        if (1 == format_ptr->isInterleaved)
        {
            int_buf_ptr->num_bufs = 1;
            int_buf_ptr->buf_ptrs[0] = ext_buf_ptr->pBuf;
            int_buf_ptr->size_per_buffer = ext_buf_ptr->sizeInSamples * format_ptr->channels * bytes_per_sample;
            int_buf_ptr->valid_data_start = ext_buf_ptr->offsetInSamples * format_ptr->channels * bytes_per_sample;
            int_buf_ptr->valid_data_end = int_buf_ptr->size_per_buffer;
            int_buf_ptr->bytes_to_samples_factor = format_ptr->channels * bytes_per_sample;
        }
        else
        {
            int_buf_ptr->num_bufs = format_ptr->channels;
            int_buf_ptr->size_per_buffer = ext_buf_ptr->sizeInSamples * bytes_per_sample;
            int_buf_ptr->bytes_to_samples_factor = bytes_per_sample;
            int8_t *ptr = ext_buf_ptr->pBuf;
            for (uint32_t i = 0; i < format_ptr->channels; i++)
            {
                int_buf_ptr->buf_ptrs[i] = ptr;
                ptr += int_buf_ptr->size_per_buffer;
            }

            int_buf_ptr->valid_data_start = ext_buf_ptr->offsetInSamples * bytes_per_sample;
            int_buf_ptr->valid_data_end = int_buf_ptr->size_per_buffer;
        }
        break;
    }
    default:
    {
        int_buf_ptr->num_bufs = 1;
        int_buf_ptr->buf_ptrs[0] = ext_buf_ptr->pBuf;
        int_buf_ptr->size_per_buffer = ext_buf_ptr->sizeInSamples * format_ptr->channels * bytes_per_sample;
        int_buf_ptr->valid_data_start = ext_buf_ptr->offsetInSamples * format_ptr->channels * bytes_per_sample;
        int_buf_ptr->valid_data_end = int_buf_ptr->size_per_buffer;
        int_buf_ptr->bytes_to_samples_factor = 1;
        break;
    }
    }
}

static void topo_buffer_convert_from_int_output_buffer(topo_internal_buf_t *internal_buf_ptr, topo_buf_t *out_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr)
{
   uint32_t bytes_per_sample = topo_get_bytes_per_sample(format_ptr->bitsPerSample);
    switch (format_ptr->bitstreamFormat)
    {
    case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2:
    case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3:
    {
        if (1 == format_ptr->isInterleaved)
        {
            out_buf_ptr->num_bufs = 1;
            out_buf_ptr->buf_ptrs[0] = internal_buf_ptr->pBuf;
            out_buf_ptr->size_per_buffer = internal_buf_ptr->size_in_samples * format_ptr->channels * bytes_per_sample;
            out_buf_ptr->valid_data_start = 0;
            out_buf_ptr->valid_data_end = internal_buf_ptr->offset_in_samples_write * format_ptr->channels * bytes_per_sample;
            out_buf_ptr->bytes_to_samples_factor = format_ptr->channels * bytes_per_sample;
        }
        else
        {
            out_buf_ptr->num_bufs = format_ptr->channels;
            out_buf_ptr->size_per_buffer = (internal_buf_ptr->end_ptr - internal_buf_ptr->write_ptr);
            //MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: capiv2 resampler size per buffer = %d", ((out_buf_ptr->size_per_buffer * 8)/format_ptr->bitsPerSample) );
            out_buf_ptr->bytes_to_samples_factor = bytes_per_sample;
            int8_t *ptr = internal_buf_ptr->write_ptr;
            for (uint32_t i = 0; i < format_ptr->channels; i++)
            {
                out_buf_ptr->buf_ptrs[i] = ptr;
                ptr += (internal_buf_ptr->size_in_samples * INTERNAL_BUF_FACTOR * bytes_per_sample) ;
            }

            out_buf_ptr->valid_data_start = 0;
            out_buf_ptr->valid_data_end = 0;
        }
        break;
    }
    default:
    {
        out_buf_ptr->num_bufs = 1;
        out_buf_ptr->buf_ptrs[0] = internal_buf_ptr->pBuf;
        out_buf_ptr->size_per_buffer = (internal_buf_ptr->size_in_samples* INTERNAL_BUF_FACTOR * format_ptr->channels * bytes_per_sample);
        out_buf_ptr->valid_data_start = 0;
        out_buf_ptr->valid_data_end = (internal_buf_ptr->offset_in_samples_write * format_ptr->channels * bytes_per_sample);
        out_buf_ptr->bytes_to_samples_factor = 1;
        break;
    }
    }
}

static void topo_buffer_convert_from_ext_output_buffer(AudPP_BufInfo_t *ext_buf_ptr, topo_buf_t *int_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr)
{
   uint32_t bytes_per_sample = topo_get_bytes_per_sample(format_ptr->bitsPerSample);
    switch (format_ptr->bitstreamFormat)
    {
    case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2:
    case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3:
    {
        if (1 == format_ptr->isInterleaved)
        {
            int_buf_ptr->num_bufs = 1;
            int_buf_ptr->buf_ptrs[0] = ext_buf_ptr->pBuf;
            int_buf_ptr->size_per_buffer = ext_buf_ptr->sizeInSamples * format_ptr->channels * bytes_per_sample;
            int_buf_ptr->valid_data_start = 0;
            int_buf_ptr->valid_data_end = ext_buf_ptr->offsetInSamples * format_ptr->channels * bytes_per_sample;
            int_buf_ptr->bytes_to_samples_factor = format_ptr->channels * bytes_per_sample;
        }
        else
        {
            int_buf_ptr->num_bufs = format_ptr->channels;
            int_buf_ptr->size_per_buffer = ext_buf_ptr->sizeInSamples * bytes_per_sample;
            int_buf_ptr->bytes_to_samples_factor = bytes_per_sample;
            int8_t *ptr = ext_buf_ptr->pBuf;
            for (uint32_t i = 0; i < format_ptr->channels; i++)
            {
                int_buf_ptr->buf_ptrs[i] = ptr;
                ptr += int_buf_ptr->size_per_buffer;
            }

            int_buf_ptr->valid_data_start = 0;
            int_buf_ptr->valid_data_end = ext_buf_ptr->offsetInSamples * bytes_per_sample;
        }
        break;
    }
    default:
    {
        int_buf_ptr->num_bufs = 1;
        int_buf_ptr->buf_ptrs[0] = ext_buf_ptr->pBuf;
        int_buf_ptr->size_per_buffer = ext_buf_ptr->sizeInSamples * format_ptr->channels * bytes_per_sample;
        int_buf_ptr->valid_data_start = 0;
        int_buf_ptr->valid_data_end = ext_buf_ptr->offsetInSamples * format_ptr->channels * bytes_per_sample;
        int_buf_ptr->bytes_to_samples_factor = 1;
        break;
    }
    }
}

static bool_t data_is_interleaved(const AdspAudioMediaFormatInfo_t *format_ptr)
{
    if((ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2 == format_ptr->bitstreamFormat) || (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3 == format_ptr->bitstreamFormat))
    {
        if (1 == format_ptr->isInterleaved)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static void topo_buffer_update_external_input_buf_sizes(AudPP_BufInfo_t *ext_buf_ptr, topo_buf_t *int_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr)
{
    if (topo_buffer_is_buf_empty(int_buf_ptr))
    {
        ext_buf_ptr->offsetInSamples = ext_buf_ptr->sizeInSamples;
    }
    else
    {
        ext_buf_ptr->offsetInSamples = int_buf_ptr->valid_data_start / topo_get_bytes_per_sample(format_ptr->bitsPerSample);
        if (data_is_interleaved(format_ptr) || topo_is_compressed(format_ptr->bitstreamFormat))
        {
            ext_buf_ptr->offsetInSamples /= format_ptr->channels;
        }
    }
}
static void topo_buffer_update_external_output_buf_sizes(AudPP_BufInfo_t *ext_buf_ptr, topo_buf_t *int_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr)
{
   ext_buf_ptr->flags = int_buf_ptr->flags;
   ext_buf_ptr->timestamp = int_buf_ptr->timestamp;
    ext_buf_ptr->offsetInSamples = int_buf_ptr->valid_data_end / topo_get_bytes_per_sample(format_ptr->bitsPerSample);
    if (data_is_interleaved(format_ptr) || topo_is_compressed(format_ptr->bitstreamFormat))
    {
        ext_buf_ptr->offsetInSamples /= format_ptr->channels;
    }
}

topo_buf_t *topo_module_get_input_buffer(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr)
{
   QURT_ELITE_ASSERT(TOPO_INVALID_MODULE_INDEX != port_index_ptr->module_index);
   QURT_ELITE_ASSERT(TOPO_INVALID_MODULE_INDEX_1 != port_index_ptr->module_index);
   QURT_ELITE_ASSERT(topo_port_index_t::INPUT_PORT == port_index_ptr->type);

   topo_port_index_t prev_port = topo_module_prev_port(topo_ptr, port_index_ptr);
   if (TOPO_INVALID_MODULE_INDEX == prev_port.module_index)
    {
        return &(topo_ptr->main_input_buffer[TOPO_PRIMARY_STRM]);
    }
   else if (TOPO_INVALID_MODULE_INDEX_1 == prev_port.module_index)
    {
       // TODO : Maintain a variable for port index of main stream and use it to get the index of second stream and so on.
        return &(topo_ptr->main_input_buffer[TOPO_FAR_STRM]);
    }
   return topo_module_get_output_buffer(topo_ptr, &prev_port);
}

topo_buf_t *topo_module_get_output_buffer(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr)
{
   QURT_ELITE_ASSERT(TOPO_INVALID_MODULE_INDEX != port_index_ptr->module_index);
   QURT_ELITE_ASSERT(TOPO_INVALID_MODULE_INDEX_1 != port_index_ptr->module_index);
   QURT_ELITE_ASSERT(topo_port_index_t::OUTPUT_PORT == port_index_ptr->type);

   topo_port_index_t next_port = topo_module_next_port(topo_ptr, port_index_ptr);
   if (TOPO_INVALID_MODULE_INDEX == next_port.module_index)
    {
        return &topo_ptr->main_output_buffer;
    }

   return &(topo_out_port_from_index(topo_ptr, port_index_ptr)->topo_buf);
}

static void topo_module_query_intf_extn(module_info *module_info_ptr, adsp_amdb_capi_v2_t *handle)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   if(module_info_ptr==NULL || NULL == handle)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc : topo_module_query_intf_extn: received NULL module info pointer");
      return;
   }

   intf_extn_chg_mf_t payload = {0};

   topo_module_intf_extn_list_t intf_ext_list = {
         {   INTF_EXTN_MAX   },
         {
            {INTF_EXTN_CHG_MF, FALSE, { (int8_t*)&payload, sizeof(intf_extn_chg_mf_t), sizeof(intf_extn_chg_mf_t) } },
         }
   };

   capi_v2_port_info_t port_info = {TRUE, FALSE, 0};

   capi_v2_prop_t static_props[] = {
         { CAPI_V2_INTERFACE_EXTENSIONS,    { reinterpret_cast<int8_t *>(&intf_ext_list), sizeof(intf_ext_list), sizeof(intf_ext_list) },   port_info },
   };

   capi_v2_proplist_t static_proplist = { 1, static_props };
   capi_result = adsp_amdb_capi_v2_get_static_properties_f(handle, NULL, &static_proplist);

   if(CAPI_V2_EOK == capi_result)
   {
      for(uint32_t i = 0; i < intf_ext_list.intf_extn_num.num_extensions; i++)
      {
         if(intf_ext_list.intf_extn_desc[i].is_supported)
         {
            // switch case used to make it easier to add future interface extensions.
            switch(intf_ext_list.intf_extn_desc[i].id)
            {
               case INTF_EXTN_CHG_MF:
               {
                  module_info_ptr->changes_media_format = payload.bits_per_sample || payload.channel_info || payload.sampling_rate;
                  break;
               }
               // we should never reach the default case.
               default:
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc : unsupported intf_extn 0x%lx for module 0x%lx", intf_ext_list.intf_extn_desc[i].id, module_info_ptr->module_id);
                  break;
               }
            }
         }
      }
   }
   else
   {
#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"audproc_svc : topo_module_query_intf_extn not supported for the module 0x%lx.", module_info_ptr->module_id);
#endif
   }
}

static void topo_module_process_fwk_extn(topo_struct *topo_ptr, uint32_t num_modules)
{
    module_info *m=NULL;
    for (uint32_t module_index=0; module_index < num_modules ; module_index++)
    {
        m=&topo_ptr->module[module_index];
        for (uint32_t i=0;i < m->num_fwk_extns; i++)
        {
            switch (m->fwk_extn_ptr[i].id)
            {
            case FWK_EXTN_ECNS:
            {
                QURT_ELITE_ASSERT(topo_ptr->ec_extn_info != NULL);
                if(TRUE ==  topo_ptr->ec_extn_info->ec_capabilities.ecns_monitoring_info.is_rtm_supported)
                {
                    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ecns_rtm_monitoring is supported");

                    capi_v2_err_t result_ecns = CAPI_V2_EOK;
                    capi_v2_log_code_t log_prop_t;
                    log_prop_t.code = LOG_ADSP_VCP_VOICEPROC_RTM_C;   // Diag log code for RTM logging

                    capi_v2_prop_t log_prop[] = {
                            { CAPI_V2_LOG_CODE, { reinterpret_cast<int8_t *>(&log_prop_t), sizeof(capi_v2_log_code_t), sizeof(capi_v2_log_code_t) }, {FALSE, FALSE, 0} },
                    };
                    capi_v2_proplist_t log_prop_list = {sizeof(log_prop)/sizeof(capi_v2_prop_t), log_prop};

                    result_ecns = m->module_ptr->vtbl_ptr->set_properties(m->module_ptr, &log_prop_list);
                    if(CAPI_V2_FAILED(result_ecns))
                    {
                        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"RTM log_code set prop failed for moduleId(0x%x-0x%x), session(%lx)", m->module_id,m->instance_id, topo_ptr->obj_id);
                    }
                }
                break;
            }
            default :
                break;
            }
        }
    }
}

static void topo_module_alloc_and_query_fwk_extn(topo_struct *topo_ptr, adsp_amdb_capi_v2_t *handle,uint32_t module_index)
{
    capi_v2_err_t capi_result = CAPI_V2_EOK;

    module_info *module_info_ptr = &topo_ptr->module[module_index];
    if(module_info_ptr==NULL || NULL == handle)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc : topo_module_query_fwk_extn: received NULL module_info_ptr pointer");
        return;
    }
    QURT_ELITE_ASSERT(module_info_ptr->num_fwk_extns);

    module_info_ptr->fwk_extn_ptr = (capi_v2_framework_extension_id_t *)qurt_elite_memory_malloc(sizeof(capi_v2_framework_extension_id_t) * module_info_ptr->num_fwk_extns, QURT_ELITE_HEAP_DEFAULT);
    if(module_info_ptr->fwk_extn_ptr == NULL)
    {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc : topo_module_query_fwk_extn: malloc failure for module 0x%lx-0x%lx", module_info_ptr->module_id,module_info_ptr->instance_id);
        return;
    }
    // allocate memory for extention id's based on the num of extentions.
    capi_v2_framework_extension_id_t *fwk_extn_ptr = module_info_ptr->fwk_extn_ptr;
    // query extension id's from the module
    capi_v2_prop_t static_props_extn[] = {
            { CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS, { reinterpret_cast<int8_t *>(fwk_extn_ptr), 0, sizeof(capi_v2_framework_extension_id_t) * module_info_ptr->num_fwk_extns }, { FALSE, FALSE, 0} }
    };
    capi_v2_proplist_t optional_proplist = { SIZE_OF_ARRAY(static_props_extn), static_props_extn };

    capi_result = adsp_amdb_capi_v2_get_static_properties_f(handle, NULL, &optional_proplist);
    if(CAPI_V2_EOK != capi_result)
    {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO," get_static_prop: failed for module_id(0x%lx-0x%lx), prop_id: CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS, result(%ld)", module_info_ptr->module_id,module_info_ptr->instance_id, capi_result);
        return;
    }

    for(uint32_t fwk_extn_index=0;fwk_extn_index<module_info_ptr->num_fwk_extns;fwk_extn_index++)
    {
        switch(fwk_extn_ptr[fwk_extn_index].id)
        {
        case FWK_EXTN_ECNS:
        {
            QURT_ELITE_ASSERT(topo_ptr->ec_extn_info != NULL);
            QURT_ELITE_ASSERT(module_index == topo_ptr->ec_extn_info->module_index);
            // get more property statically
            ecns_output_capabilities_t ecns_output_capabilities = { FALSE, FALSE };
            ecns_monitoring_capabilities_t ecns_monitoring_info = { FALSE };
            ecns_rate_matching_capabilities_t ecns_rate_matching_info = { FALSE };
            ecns_vp3_capabilities_t ecns_vp3_info = { FALSE };
            ecns_stt_capabilities_t ecns_stt_info = { FALSE };
            ecns_multichannel_ref_capabilities_t ecns_multichannel_ref_info = {FALSE};
            ecns_speech_act_det_capabilities_t ecns_speech_act_det_capabilities = { FALSE };

            capi_v2_init_memory_requirement_t mem_req = { 0 };
            capi_v2_get_ecns_property_t  ecns_get_prop[]={
                    {   CAPI_V2_PROPERTY_ID_ECNS_OUTPUT_CAPABILITIES, &ecns_output_capabilities },
                    {   CAPI_V2_PROPERTY_ID_ECNS_MONITORING_CAPABILITIES, &ecns_monitoring_info},
                    // TODO : Not needed - only CVD path as of now
                    {   CAPI_V2_PROPERTY_ID_ECNS_VP3_CAPABILITIES, &ecns_vp3_info},
                    // TODO : Specific set param with hardcoded module id handling needed in topo_set_param and get_param
                    {   CAPI_V2_PROPERTY_ID_ECNS_STT_CAPABILITIES, &ecns_stt_info},
                    // TODO : Configure SS based on this
                    {   CAPI_V2_PROPERTY_ID_ECNS_RATE_MATCHING_CAPABILITIES, &ecns_rate_matching_info},
                    {   CAPI_V2_PROPERTY_ID_MULTICHANNEL_REF_CAPABILITIES, &ecns_multichannel_ref_info},
                    // TODO : Handle speech activity detection event from module - only CVD as of now
                    {   CAPI_V2_PROPERTY_ID_SPEECH_ACT_DET_CAPABILITIES, &ecns_speech_act_det_capabilities}
            };

            // SA: Adding CAPI_V2_INIT_MEMORY_REQUIREMENT propId as well because adsp_amdb API require one of the get_static prop to be INIT prop
            capi_v2_prop_t static_ec_capability[]={
                    { CAPI_V2_INIT_MEMORY_REQUIREMENT, { reinterpret_cast<int8_t *>(&mem_req), sizeof(mem_req), sizeof(mem_req) }, { FALSE, FALSE, 0 }      },
                    { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[0]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                    { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[1]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                    { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[2]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                    { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[3]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                    { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[4]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                    { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[5]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
                    { CAPI_V2_CUSTOM_PROPERTY,{ reinterpret_cast<int8_t *>(&ecns_get_prop[6]), 0, sizeof(capi_v2_get_ecns_property_t)}, { FALSE, FALSE, 0 } },
            };

            capi_v2_proplist_t static_ec_proplist = { SIZE_OF_ARRAY(static_ec_capability), static_ec_capability };
            capi_result = adsp_amdb_capi_v2_get_static_properties_f(handle, NULL, &static_ec_proplist);
            if(CAPI_V2_EOK != capi_result)
            {
                MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO," get_static_prop: failed for module_id(0x%lx-0x%lx), prop_id: CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS, result(%ld)", module_info_ptr->module_id,module_info_ptr->instance_id, capi_result);
                return;
            }
            topo_ptr->ec_extn_info->ec_capabilities.ecns_output_capabilities = ecns_output_capabilities;
            topo_ptr->ec_extn_info->ec_capabilities.ecns_monitoring_info = ecns_monitoring_info;
            topo_ptr->ec_extn_info->ec_capabilities.ecns_rate_matching_info = ecns_rate_matching_info;
            topo_ptr->ec_extn_info->ec_capabilities.ecns_vp3_info = ecns_vp3_info;
            topo_ptr->ec_extn_info->ec_capabilities.ecns_stt_info = ecns_stt_info;
            topo_ptr->ec_extn_info->ec_capabilities.ecns_multichannel_ref_info = ecns_multichannel_ref_info;
            topo_ptr->ec_extn_info->ec_capabilities.ecns_speech_act_det_capabilities = ecns_speech_act_det_capabilities;

            uint8_t num_output_port =1;

            if (topo_ptr->ec_extn_info->ec_capabilities.ecns_output_capabilities.output_lec) num_output_port++;
            if (topo_ptr->ec_extn_info->ec_capabilities.ecns_output_capabilities.output_nr) num_output_port++;

            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ECNS custom static property: num output ports(%d)", num_output_port);
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ECNS custom static property: is_monitoring_supported(%d)", topo_ptr->ec_extn_info->ec_capabilities.ecns_monitoring_info.is_rtm_supported);
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ECNS custom static property: is_vp3_supported(%d)", topo_ptr->ec_extn_info->ec_capabilities.ecns_vp3_info.is_vp3_supported);
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ECNS custom static property: is_stt_supported(%d)", topo_ptr->ec_extn_info->ec_capabilities.ecns_stt_info.is_stt_supported);
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ECNS custom static property: rate_matching_info(%d)", topo_ptr->ec_extn_info->ec_capabilities.ecns_rate_matching_info.is_rate_matching_supported);
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ECNS custom static property: multichannel_reference_capability(%d)", topo_ptr->ec_extn_info->ec_capabilities.ecns_multichannel_ref_info.is_multichannel_ref_supported);
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ECNS custom static property: speech_act_det_info(%d)", topo_ptr->ec_extn_info->ec_capabilities.ecns_speech_act_det_capabilities.is_speech_activity_supported);
        }
        break;
        default :
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: "
                    "Framework extension 0x%lX not supported", topo_ptr->obj_id,
                    fwk_extn_ptr[fwk_extn_index].id);
            break;
        }
    }
}

static void topo_adsp_media_fmt_to_topo(const AdspAudioMediaFormatInfo_t *adsp_media_fmt_ptr, topo_media_fmt_t *topo_media_fmt_ptr)
{
   topo_media_fmt_ptr->f.bitstream_format    = adsp_media_fmt_ptr->bitstreamFormat;
   topo_media_fmt_ptr->h.data_format         = topo_is_compressed(adsp_media_fmt_ptr->bitstreamFormat) ? CAPI_V2_IEC61937_PACKETIZED : CAPI_V2_FIXED_POINT;
   topo_media_fmt_ptr->f.num_channels        = adsp_media_fmt_ptr->channels;
   topo_media_fmt_ptr->f.bits_per_sample     = adsp_media_fmt_ptr->bitsPerSample == 16 ? 16 : 32;
   topo_media_fmt_ptr->f.q_factor            = topo_calculate_q_factor(adsp_media_fmt_ptr);
   topo_media_fmt_ptr->f.sampling_rate       = adsp_media_fmt_ptr->samplingRate;
   topo_media_fmt_ptr->f.data_is_signed      = adsp_media_fmt_ptr->isSigned;
   topo_media_fmt_ptr->f.data_interleaving = adsp_media_fmt_ptr->isInterleaved ? CAPI_V2_INTERLEAVED : CAPI_V2_DEINTERLEAVED_UNPACKED;
   for (uint32_t i = 0; i < topo_media_fmt_ptr->f.num_channels; i++)
   {
       topo_media_fmt_ptr->f.channel_type[i] = adsp_media_fmt_ptr->channelMapping[i]; // Conversion from 8 bit to 16 bit here.
   }
}

static bool_t topo_is_compressed(uint32_t bitstream_format)
{
    return ((bitstream_format != ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2) && (bitstream_format != ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3));

}

static bool_t topo_is_primary_in_port(uint32_t port_id)
{
    return (port_id == TOPO_PRIMARY_STRM);
}

static uint32_t topo_calculate_q_factor(const AdspAudioMediaFormatInfo_t *media_fmt_ptr)
{
    if (topo_is_compressed(media_fmt_ptr->bitstreamFormat))
    {
        return CAPI_V2_DATA_FORMAT_INVALID_VAL;
    }

    switch(media_fmt_ptr->bitsPerSample)
    {
    case 16:
        return PCM_16BIT_Q_FORMAT;
    case 24:
        return ELITE_32BIT_PCM_Q_FORMAT;
   case 32:
      return PCM_32BIT_Q_FORMAT;
    default:
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Topo received invalid bits per sample %lu", media_fmt_ptr->bitsPerSample);
        return CAPI_V2_DATA_FORMAT_INVALID_VAL;
    }
}

static void topo_topo_media_fmt_to_adsp(const topo_media_fmt_t *topo_media_fmt_ptr, AdspAudioMediaFormatInfo_t *adsp_media_fmt_ptr)
{
    switch(topo_media_fmt_ptr->h.data_format)
    {
    case CAPI_V2_FIXED_POINT:
        QURT_ELITE_ASSERT(topo_media_fmt_ptr->f.num_channels != CAPI_V2_DATA_FORMAT_INVALID_VAL);
        adsp_media_fmt_ptr->channels = topo_media_fmt_ptr->f.num_channels;

        if (adsp_media_fmt_ptr->channels > SIZE_OF_ARRAY(adsp_media_fmt_ptr->channelMapping))
        {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: %lu channels at topo output, limiting to %d.", adsp_media_fmt_ptr->channels, SIZE_OF_ARRAY(adsp_media_fmt_ptr->channelMapping));
            adsp_media_fmt_ptr->channels = SIZE_OF_ARRAY(adsp_media_fmt_ptr->channelMapping);
        }
        for (uint32_t i = 0; i < adsp_media_fmt_ptr->channels; i++)
        {
            adsp_media_fmt_ptr->channelMapping[i] = topo_media_fmt_ptr->f.channel_type[i]; // Conversion from 16 bit to 8 bit here.
        }

        QURT_ELITE_ASSERT ((topo_media_fmt_ptr->f.bits_per_sample == 16) || (topo_media_fmt_ptr->f.bits_per_sample == 32));
        adsp_media_fmt_ptr->bitsPerSample = topo_media_fmt_ptr->f.bits_per_sample;
        if(topo_media_fmt_ptr->f.bits_per_sample == 32)
        {
           adsp_media_fmt_ptr->bitsPerSample = (topo_media_fmt_ptr->f.q_factor ==  ELITE_32BIT_PCM_Q_FORMAT)? 24 : 32;
        }

        if (topo_media_fmt_ptr->f.sampling_rate != CAPI_V2_DATA_FORMAT_INVALID_VAL)
        {
            adsp_media_fmt_ptr->samplingRate = topo_media_fmt_ptr->f.sampling_rate;
        }
        else
        {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Invalid sampling rate at topo output, setting to 48000.");
            adsp_media_fmt_ptr->samplingRate = 48000;
        }
        if (topo_media_fmt_ptr->f.data_is_signed != CAPI_V2_DATA_FORMAT_INVALID_VAL)
        {
            adsp_media_fmt_ptr->isSigned = topo_media_fmt_ptr->f.data_is_signed;
        }
        else
        {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Invalid is_signed at topo output, setting to TRUE.");
            adsp_media_fmt_ptr->isSigned = 1;
        }
        switch(topo_media_fmt_ptr->f.data_interleaving)
        {
        case CAPI_V2_INTERLEAVED:
            adsp_media_fmt_ptr->isInterleaved = 1;
            break;
        case CAPI_V2_DEINTERLEAVED_UNPACKED:
            adsp_media_fmt_ptr->isInterleaved = 0;
            break;
        default:
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Invalid interleaving at topo output. Only INTERLEAVED(0) and DEINTERLEAVED_UNPACKED(2) supported. Got %lu", static_cast<uint32_t>(topo_media_fmt_ptr->f.data_interleaving));
            adsp_media_fmt_ptr->isInterleaved = 0;
            break;
        }
        if (topo_media_fmt_ptr->f.bitstream_format != CAPI_V2_DATA_FORMAT_INVALID_VAL)
        {
            adsp_media_fmt_ptr->bitstreamFormat = topo_media_fmt_ptr->f.bitstream_format;
        }
        else
        {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audproc_svc: Invalid bitstream format at topo output, setting to PCM.");
            adsp_media_fmt_ptr->bitstreamFormat = ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3;
        }
        break;
    default:
        QURT_ELITE_ASSERT(CAPI_V2_IEC61937_PACKETIZED == topo_media_fmt_ptr->h.data_format);
        adsp_media_fmt_ptr->bitsPerSample = (CAPI_V2_DATA_FORMAT_INVALID_VAL != topo_media_fmt_ptr->f.bits_per_sample) ? topo_media_fmt_ptr->f.bits_per_sample : 16;
      if(adsp_media_fmt_ptr->bitsPerSample == 32)
      {
         adsp_media_fmt_ptr->bitsPerSample = (topo_media_fmt_ptr->f.q_factor ==  ELITE_32BIT_PCM_Q_FORMAT)? 24 : 32;
      }
        adsp_media_fmt_ptr->bitstreamFormat = (CAPI_V2_DATA_FORMAT_INVALID_VAL != topo_media_fmt_ptr->f.bitstream_format) ?topo_media_fmt_ptr->f.bitstream_format : ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3;
        adsp_media_fmt_ptr->isInterleaved = 1; // IEC61937 has to be interleaved for the PP service
        adsp_media_fmt_ptr->isSigned = (CAPI_V2_DATA_FORMAT_INVALID_VAL != topo_media_fmt_ptr->f.data_is_signed) ? topo_media_fmt_ptr->f.data_is_signed : 1;
        adsp_media_fmt_ptr->samplingRate = (CAPI_V2_DATA_FORMAT_INVALID_VAL != topo_media_fmt_ptr->f.sampling_rate) ? topo_media_fmt_ptr->f.sampling_rate : 48000;
        if (CAPI_V2_DATA_FORMAT_INVALID_VAL != topo_media_fmt_ptr->f.num_channels)
        {
            adsp_media_fmt_ptr->channels = topo_media_fmt_ptr->f.num_channels;
            if (adsp_media_fmt_ptr->channels > SIZE_OF_ARRAY(adsp_media_fmt_ptr->channelMapping))
            {
                adsp_media_fmt_ptr->channels = SIZE_OF_ARRAY(adsp_media_fmt_ptr->channelMapping);
            }
            for (uint32_t i = 0; i < adsp_media_fmt_ptr->channels; i++)
            {
                adsp_media_fmt_ptr->channelMapping[i] = topo_media_fmt_ptr->f.channel_type[i]; // Conversion from 16 bit to 8 bit here.
            }
        }
        else
        {
            adsp_media_fmt_ptr->channels = 1;
            adsp_media_fmt_ptr->channelMapping[0] = PCM_CHANNEL_C;

        }
        break;
    }
}


static void topo_print_module_output_media_format(uint16_t obj_id, uint32_t module_id, uint32_t instance_id, const topo_media_fmt_t *media_fmt_ptr)
{
   MSG_7(MSG_SSID_QDSP6, DBG_LOW_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lx raised output media format. The new format is"
         "num_channels: %lu, "
         "bits_per_sample: %lu,"
         "sampling_rate: %lu,"
         " bitstream_format: 0x%lx, ",
         obj_id,
         module_id, instance_id,
         media_fmt_ptr->f.num_channels,
         media_fmt_ptr->f.bits_per_sample,
         media_fmt_ptr->f.sampling_rate ,
         media_fmt_ptr->f.bitstream_format);
}

static void topo_print_media_format(uint16_t obj_id, const topo_media_fmt_t *media_fmt_ptr)
{
    MSG_5(MSG_SSID_QDSP6, DBG_LOW_PRIO, "P%hX audproc_svc: reinit done, final media type is as follows:"
          "num_channels: %lu, "
          "bits_per_sample: %lu,"
          "sampling_rate: %lu,"
          " bitstream_format: 0x%lx, ",
          obj_id,
          media_fmt_ptr->f.num_channels,
          media_fmt_ptr->f.bits_per_sample,
          media_fmt_ptr->f.sampling_rate ,
          media_fmt_ptr->f.bitstream_format);

    MSG_7(MSG_SSID_QDSP6, DBG_LOW_PRIO, "P%hX audproc_svc: Channel mapping for channels 1 to 6: %hu, %hu, %hu, %hu, %hu, %hu", obj_id,
            media_fmt_ptr->f.channel_type[0], media_fmt_ptr->f.channel_type[1], media_fmt_ptr->f.channel_type[2], media_fmt_ptr->f.channel_type[3],
            media_fmt_ptr->f.channel_type[4], media_fmt_ptr->f.channel_type[5]);

    if(media_fmt_ptr->f.num_channels > 6)
    {
       MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO, "P%hX audproc_svc for channels 7 and 8:: Channel mapping: %hu, %hu", obj_id,
             media_fmt_ptr->f.channel_type[6], media_fmt_ptr->f.channel_type[7]);
    }
}

static uint32_t topo_buffer_bytes_to_samples(const topo_buf_t *buf_ptr, uint32_t bytes)
{
    return bytes / buf_ptr->bytes_to_samples_factor;
}
static uint32_t topo_buffer_samples_to_bytes(const topo_buf_t *buf_ptr, uint32_t samples)
{
    return samples * buf_ptr->bytes_to_samples_factor;
}

/**
 * Copies the input buffer to the output buffer. To be called
 * when there are no modules in the topology.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] topo_ptr
 *   Pointer to the topology structure.
 */
static void topo_loop_thru_zero_modules(topo_struct *topo_ptr)
{
    topo_buf_t *input_buf, *output_buf;
    input_buf = &topo_ptr->main_input_buffer[TOPO_PRIMARY_STRM];
    output_buf = &topo_ptr->main_output_buffer;

    QURT_ELITE_ASSERT(input_buf->bytes_to_samples_factor == output_buf->bytes_to_samples_factor);

    if ((output_buf->valid_data_end == 0) && (topo_ptr->in_format[TOPO_PRIMARY_STRM].samplingRate!=0))
    {
       uint64_t timestamp_offset = ((input_buf->valid_data_start/input_buf->bytes_to_samples_factor) * 1000000);
       timestamp_offset/=(topo_ptr->in_format[TOPO_PRIMARY_STRM].samplingRate);
       output_buf->flags = input_buf->flags;
       output_buf->timestamp = input_buf->timestamp + timestamp_offset;
    }

    uint32_t input_data_size = input_buf->valid_data_end - input_buf->valid_data_start;
    uint32_t output_data_size = output_buf->size_per_buffer - output_buf->valid_data_end;
    uint32_t num_bytes_copied = 0;

    QURT_ELITE_ASSERT(input_buf->num_bufs == output_buf->num_bufs);
    for (uint32_t i = 0; i < input_buf->num_bufs; i++)
    {
        num_bytes_copied = memscpy(output_buf->buf_ptrs[i] + output_buf->valid_data_end, output_data_size, input_buf->buf_ptrs[i] + input_buf->valid_data_start, input_data_size);
}

    input_buf->valid_data_start += num_bytes_copied;
    QURT_ELITE_ASSERT(input_buf->valid_data_start <= input_buf->valid_data_end);
    if (input_buf->valid_data_start == input_buf->valid_data_end)
    {
        input_buf->valid_data_start = input_buf->valid_data_end = 0;
    }

    output_buf->valid_data_end += num_bytes_copied;

    QURT_ELITE_ASSERT(output_buf->valid_data_end <= output_buf->size_per_buffer);
}

static bool_t topo_is_media_format_valid(const topo_media_fmt_t *f, uint16_t obj_id, uint32_t module_id, uint32_t instance_id)
{
    QURT_ELITE_ASSERT ((CAPI_V2_FIXED_POINT == f->h.data_format) || (CAPI_V2_IEC61937_PACKETIZED == f->h.data_format));

    if (CAPI_V2_FIXED_POINT == f->h.data_format)
    {
        if ((f->f.bits_per_sample != 16) && (f->f.bits_per_sample != 32))
        {
            MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX"
                    " provided bits_per_sample %lu for data format event. Only 16 and 32 are supported.",
                    obj_id, module_id, instance_id, f->f.bits_per_sample);
            return FALSE;
        }

        if ((f->f.num_channels < 1) || (f->f.num_channels > CAPI_V2_MAX_CHANNELS))
        {
            MSG_6(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX"
                    " provided num_channels %lu for data format event. Only values from %d to %d are supported.",
                    obj_id, module_id, instance_id, f->f.num_channels, 1, CAPI_V2_MAX_CHANNELS);
            return FALSE;
        }
    }

    return TRUE;
}

static capi_v2_err_t topo_event_callback(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_ptr)
{
    topo_struct *topo_ptr = topo_event_get_struct_ptr(context_ptr);
    uint32_t module_index = topo_event_get_module_index(context_ptr);
    uint32_t requried_payload_size = 0;
    capi_v2_err_t result = CAPI_V2_EOK;

#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
    MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX raised event id %lu.",
            topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id, static_cast<uint32_t>(id));
#endif

    switch(id)
    {
    case CAPI_V2_EVENT_KPPS:
    {
        requried_payload_size = sizeof(capi_v2_event_KPPS_t);
        if (event_ptr->payload.actual_data_len < requried_payload_size){
            CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
            break;
        }

        uint32_t new_KPPS = reinterpret_cast<capi_v2_event_KPPS_t*>(event_ptr->payload.data_ptr)->KPPS;
        uint32_t old_KPPS = topo_ptr->module[module_index].KPPS;
        if(new_KPPS!=old_KPPS)
        {
            MSG_5(MSG_SSID_QDSP6, DBG_LOW_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                    "changed KPPS from %lu to %lu.",
                    topo_ptr->obj_id, topo_ptr->module[module_index].module_id,
                    topo_ptr->module[module_index].instance_id, old_KPPS, new_KPPS);
            topo_ptr->module[module_index].KPPS = new_KPPS;
            topo_update_chain_KPPS(topo_ptr);
        }
        break;
    }
    case CAPI_V2_EVENT_BANDWIDTH:
    {
        requried_payload_size = sizeof(capi_v2_event_bandwidth_t);
        if (event_ptr->payload.actual_data_len < requried_payload_size){
            CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
            break;
        }

        capi_v2_event_bandwidth_t *bandwidth_info_ptr = reinterpret_cast<capi_v2_event_bandwidth_t*>(event_ptr->payload.data_ptr);
        uint32_t new_bandwidth = bandwidth_info_ptr->code_bandwidth + bandwidth_info_ptr->data_bandwidth;
        uint32_t old_bandwidth = topo_ptr->module[module_index].bandwidth;
        if(new_bandwidth!=old_bandwidth)
        {
            MSG_5(MSG_SSID_QDSP6, DBG_LOW_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                    "changed bandwidth from %lu to %lu.",
                    topo_ptr->obj_id, topo_ptr->module[module_index].module_id,
                    topo_ptr->module[module_index].instance_id, old_bandwidth, new_bandwidth);
            topo_ptr->module[module_index].bandwidth = new_bandwidth;
            topo_update_chain_bandwidth(topo_ptr);
        }
        break;
    }
    case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
    {
        requried_payload_size = sizeof(capi_v2_event_algorithmic_delay_t);
        if (event_ptr->payload.actual_data_len < requried_payload_size){
            CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
            break;
        }

        uint32_t new_delay = reinterpret_cast<capi_v2_event_algorithmic_delay_t*>(event_ptr->payload.data_ptr)->delay_in_us;
        uint32_t old_delay = topo_ptr->module[module_index].delay_in_us;
        if(new_delay!=old_delay)
        {
            MSG_5(MSG_SSID_QDSP6, DBG_LOW_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                    "changed delay from %lu to %lu.",
                    topo_ptr->obj_id, topo_ptr->module[module_index].module_id,
                    topo_ptr->module[module_index].instance_id, old_delay, new_delay);
            topo_ptr->module[module_index].delay_in_us = new_delay;
            topo_update_chain_delay(topo_ptr);
        }
        break;
    }
    case CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED:
    {
        requried_payload_size = sizeof(topo_media_fmt_t);
        if (event_ptr->payload.actual_data_len < requried_payload_size){
            CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
            break;
        }

        topo_media_fmt_t *format_ptr = reinterpret_cast<topo_media_fmt_t*>(event_ptr->payload.data_ptr);

        if (!event_ptr->port_info.is_valid)
        {
            MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                    "provided invalid port for event id %lu. Only output port 0 is supported.",
                    topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id, static_cast<uint32_t>(id));
            return CAPI_V2_EUNSUPPORTED;
        }
        if (event_ptr->port_info.is_input_port)
        {
            MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                    "provided input port for event id %lu. Only output port 0 is supported.",
                    topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id, static_cast<uint32_t>(id));
            return CAPI_V2_EUNSUPPORTED;
        }
        if(event_ptr->port_info.port_index >= topo_ptr->module[module_index].num_output_ports)
        {
            MSG_6(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                    "provided port %lu for event id %lu. It has only %lu output ports.",
                    topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id,
                    event_ptr->port_info.port_index, static_cast<uint32_t>(id), topo_ptr->module[module_index].num_output_ports);
            return CAPI_V2_EUNSUPPORTED;
        }

        if ((CAPI_V2_FIXED_POINT != format_ptr->h.data_format) && (CAPI_V2_IEC61937_PACKETIZED != format_ptr->h.data_format))
        {
            MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                    "provided data_format %lu for event id %lu. Only port FIXED_POINT(0) and IEC61937_PACKETIZED(3) are supported.",
                    topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id,
                    static_cast<uint32_t>(format_ptr->h.data_format), static_cast<uint32_t>(id));
            return CAPI_V2_EUNSUPPORTED;
        }

        if (!topo_is_media_format_valid(format_ptr, topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id))
        {
            return CAPI_V2_EUNSUPPORTED;
        }

        //MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
        //                    "provided out num channels %lu",
        //                    topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id, format_ptr->f.num_channels);



        topo_ptr->module[module_index].output_ports_ptr[event_ptr->port_info.port_index].media_event.event_is_pending = TRUE;
        topo_ptr->module[module_index].output_ports_ptr[event_ptr->port_info.port_index].media_event.event = *format_ptr;
        // Media format events raised by the module will be handled during the process
        topo_print_module_output_media_format(topo_ptr->obj_id, topo_ptr->module[module_index].module_id,
                topo_ptr->module[module_index].instance_id, format_ptr);
        break;
    }
    case CAPI_V2_EVENT_PROCESS_STATE:
    {
        requried_payload_size = sizeof(capi_v2_event_process_state_t);
        if (event_ptr->payload.actual_data_len < requried_payload_size){
            CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
            break;
        }

        capi_v2_event_process_state_t *payload_ptr = reinterpret_cast<capi_v2_event_process_state_t*>(event_ptr->payload.data_ptr);

        if (!payload_ptr->is_enabled)
        {
            if (!topo_module_is_disablement_possible(&topo_ptr->module[module_index]))
            {
                MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: "
                        "Cannot disable Module-Instance with id 0x%lX-0x%lX.",
                        topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id);
                return CAPI_V2_EUNSUPPORTED;
            }
        }

        bool_t old_process_state = topo_ptr->module[module_index].is_enabled;
        topo_ptr->module[module_index].is_enabled = payload_ptr->is_enabled;

        MSG_5(MSG_SSID_QDSP6, DBG_LOW_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                "changed process state from %hu to %hu.",
                topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id,
                old_process_state, topo_ptr->module[module_index].is_enabled);
        if(payload_ptr->is_enabled)
        {
            if (!topo_module_list_locked(topo_ptr))
            {
                topo_handle_module_enable_events(topo_ptr, module_index);
            }
        }
       // Disable events raised by the module will be handled in the process

       break;
    }
    case CAPI_V2_EVENT_PORT_DATA_THRESHOLD_CHANGE:
    {
        requried_payload_size = sizeof(capi_v2_port_data_threshold_change_t);
        uint32_t port_id;
        if (event_ptr->payload.actual_data_len < requried_payload_size)
        {
            CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
            break;
        }

        capi_v2_port_data_threshold_change_t *payload_ptr = reinterpret_cast<capi_v2_port_data_threshold_change_t*>(event_ptr->payload.data_ptr);
        uint32_t new_data_thresh = payload_ptr->new_threshold_in_bytes;

        if(topo_is_compressed(topo_ptr->in_format[TOPO_PRIMARY_STRM].bitstreamFormat))
        {
            if (payload_ptr->new_threshold_in_bytes != 1)
            {
                MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX Module-Instance with id 0x%lX-0x%lX returned invalid threshold for compressed format %lu. ", topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id, payload_ptr->new_threshold_in_bytes);
                CAPI_V2_SET_ERROR(result,CAPI_V2_EUNSUPPORTED);
                break;
            }
        }
        else if(event_ptr->port_info.is_input_port)  // input port
        {
           port_id = event_ptr->port_info.port_index;
           if(!topo_is_data_threshold_valid(new_data_thresh, &topo_ptr->module[module_index].input_ports_ptr[port_id].media_event))
           {
              MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX ERROR !! Module-Instance with id 0x%lX-0x%lX returned invalid input threshold %lu. ", topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id, payload_ptr->new_threshold_in_bytes);
              break;
           }
           MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX Module-Instance with id 0x%lX-0x%lX returned input threshold %lu. ", topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id, payload_ptr->new_threshold_in_bytes);
           topo_ptr->module[module_index].input_ports_ptr[port_id].data_threshold = new_data_thresh;
           topo_update_chain_port_data_threshold(topo_ptr, port_id);
        }
        else // output port
        {
           port_id = event_ptr->port_info.port_index;
           if(!topo_is_data_threshold_valid(new_data_thresh, &topo_ptr->module[module_index].output_ports_ptr[port_id].media_event))
           {
              MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX ERROR !! Module-Instance with id 0x%lX-0x%lX returned invalid output threshold %lu. ", topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id, payload_ptr->new_threshold_in_bytes);
              break;
           }
           topo_ptr->module[module_index].output_ports_ptr[port_id].data_threshold = new_data_thresh;
        }
        break;
    }
    case CAPI_V2_EVENT_HEADROOM:
    {
        requried_payload_size = sizeof(capi_v2_event_headroom_t);
        if (event_ptr->payload.actual_data_len < requried_payload_size){
            CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
            break;
        }

        uint32_t new_headroom = reinterpret_cast<capi_v2_event_headroom_t*>(event_ptr->payload.data_ptr)->headroom_in_millibels;
#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
        uint32_t old_headroom = topo_ptr->module[module_index].headroom;
        MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX"
                " changed headroom from %lu to %lu.",
                topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id,
                old_headroom, new_headroom);
#endif
        topo_ptr->module[module_index].headroom = new_headroom;
        topo_update_chain_headroom(topo_ptr);

        break;
    }
    case CAPI_V2_EVENT_GET_LIBRARY_INSTANCE:
    {
        requried_payload_size = sizeof(capi_v2_event_get_library_instance_t);
        if (event_ptr->payload.actual_data_len < requried_payload_size){
            CAPI_V2_SET_ERROR(result,CAPI_V2_ENEEDMORE);
            break;
        }

        capi_v2_event_get_library_instance_t *payload_ptr = reinterpret_cast<capi_v2_event_get_library_instance_t*>(event_ptr->payload.data_ptr);

        return capi_v2_library_factory_get_instance(payload_ptr->id, &payload_ptr->ptr);

        break;
    }
	case CAPI_V2_EVENT_DATA_TO_DSP_CLIENT:
	{
		capi_v2_event_data_to_dsp_client_t *payload_ptr = reinterpret_cast<capi_v2_event_data_to_dsp_client_t*>(event_ptr->payload.data_ptr);
		if(NULL == payload_ptr)
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc : Payload ptr received is NULL from module ID %lu\
					 ",topo_ptr->obj_id,topo_ptr->module[module_index].module_id );
			return CAPI_V2_EUNSUPPORTED;
		}

		audproc_event_response_payload_header_t payload_header;
		payload_header.event_id = payload_ptr->param_id;
		payload_header.module_id = topo_ptr->module[module_index].module_id;
		payload_header.instance_id = topo_ptr->module[module_index].instance_id;
		payload_header.reserved = 0;
		payload_header.payload_size = payload_ptr->payload.actual_data_len;


		uint32_t size_of_buf_to_client = sizeof(audproc_event_response_payload_header_t) + payload_header.payload_size;

		int8_t *buf_to_client = NULL;
		buf_to_client = (int8_t*)qurt_elite_memory_malloc(size_of_buf_to_client, QURT_ELITE_HEAP_DEFAULT);
		if(NULL != buf_to_client)
		{
			uint32_t size_copied = memscpy(buf_to_client,sizeof(audproc_event_response_payload_header_t),
											(int8_t*)&payload_header, sizeof(audproc_event_response_payload_header_t));
			memscpy((buf_to_client+size_copied),payload_header.payload_size,payload_ptr->payload.data_ptr, payload_header.payload_size);
			topo_ptr->event_client->vtbl->custom_event(topo_ptr->event_client, (void*)buf_to_client, size_of_buf_to_client);
			qurt_elite_memory_free(buf_to_client);
		}
		else
		{
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX is unable to allocate %lu bytes of memory to send event registration payload",topo_ptr->obj_id, topo_ptr->module[module_index].module_id,topo_ptr->module[module_index].instance_id,size_of_buf_to_client);
		}
		return CAPI_V2_EOK;
	}
    default:
        MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX"
                        " raised unsupported event with id %lu.",
                        topo_ptr->obj_id, topo_ptr->module[module_index].module_id,
                        topo_ptr->module[module_index].instance_id, static_cast<uint32_t>(id));
      return CAPI_V2_EUNSUPPORTED;
    }

    if(CAPI_V2_IS_ERROR_CODE_SET(result,CAPI_V2_ENEEDMORE))
    {
        MSG_6(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with id 0x%lX-0x%lX "
                "provided insufficient size for event id %lu. Required: %lu bytes, provided: %lu bytes.",
                topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id,
                static_cast<uint32_t>(id), sizeof(capi_v2_event_get_library_instance_t), event_ptr->payload.actual_data_len);
        return result;
    }

    return result;
}

static void topo_event_initialize_handle(topo_struct *topo_ptr, uint32_t module_index)
{
   event_handle_t *handle = topo_event_get_handle(topo_ptr, module_index);
   handle->module_index = module_index;
   handle->topo_struct_ptr = topo_ptr;
}

static event_handle_t *topo_event_get_handle(topo_struct *topo_ptr, uint32_t module_index)
{
    return &topo_ptr->module[module_index].event_handle;
}

static uint32_t topo_event_get_module_index(void *handle)
{
    return reinterpret_cast<event_handle_t*>(handle)->module_index;
}

static topo_struct *topo_event_get_struct_ptr(void *handle)
{
    return reinterpret_cast<event_handle_t*>(handle)->topo_struct_ptr;
}

static bool_t topo_zero_modules_enabled(const topo_struct *topo_ptr)
{
   if (TOPO_INVALID_MODULE_INDEX == topo_ptr->first_port[TOPO_PRIMARY_STRM].module_index)
    {
      QURT_ELITE_ASSERT(TOPO_INVALID_MODULE_INDEX == topo_ptr->last_port.module_index);
        return TRUE;
    }
    else
    {
      QURT_ELITE_ASSERT(TOPO_INVALID_MODULE_INDEX != topo_ptr->last_port.module_index);
        return FALSE;
    }
}

static bool_t topo_module_list_locked(const topo_struct *topo_ptr)
{
    return topo_ptr->module_list_locked;
}

static bool_t topo_module_pending_reconfig(const topo_struct *topo_ptr, uint32_t start_index)
{
   if ((topo_ptr->module[start_index].is_enabled && !topo_ptr->module[start_index].disabled_till_next_media_event) != topo_ptr->module[start_index].is_in_chain)
    {
        return TRUE;
    }

   if (topo_ptr->module[start_index].is_in_chain)
   {
      for (uint32_t i = 0; i < topo_ptr->module[start_index].num_output_ports; i++)
      {
         if (topo_ptr->module[start_index].output_ports_ptr[i].media_event.event_is_pending)
    {
        return TRUE;
    }
      }
   }
    return FALSE;
}

static void topo_lock_module_list(topo_struct *topo_ptr)
{
    QURT_ELITE_ASSERT(!topo_module_list_locked(topo_ptr));

    topo_ptr->module_list_locked = TRUE;
}

static void topo_unlock_module_list(topo_struct *topo_ptr)
{
    QURT_ELITE_ASSERT(topo_module_list_locked(topo_ptr));

    if (!topo_zero_modules_created(topo_ptr))
    {
        for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
        {
            topo_handle_module_enable_events(topo_ptr, i);
        }
    }

    topo_ptr->module_list_locked = FALSE;
}

static void topo_handle_module_enable_events(topo_struct *topo_ptr, uint32_t module_index)
{
    //TODO : How to limit check based on module chain i.e,. near module or far module
#if 0
    for(uint32_t port_id=0;port_id < topo_ptr->max_in_streams;port_id++)
    {
        if (!topo_ptr->data_format_received[port_id])
        {
            return;
        }
    }
#else
    if (!topo_ptr->data_format_received[TOPO_PRIMARY_STRM])
    {
        return;
    }
#endif


   if (topo_ptr->currently_handling_events)
   {
      return;
   }

   topo_ptr->currently_handling_events = TRUE;


   module_info *m = topo_ptr->module;
   bool_t module_state_changed = topo_check_update_module_enablement(topo_ptr, module_index);

   if(m[module_index].is_in_chain)
   {
      for (uint32_t j = 0; j < m[module_index].num_output_ports; j++)
      {
         if(module_state_changed)
         {
            m[module_index].output_ports_ptr[j].media_event.event_is_pending = TRUE;
         }
      }
   }
   topo_ptr->currently_handling_events = FALSE;

   topo_update_chain_KPPS(topo_ptr);
   topo_update_chain_bandwidth(topo_ptr);
   topo_update_chain_delay(topo_ptr);
   topo_update_chain_headroom(topo_ptr);
}

static bool_t topo_is_output_unit_frames_changed(topo_struct *topo_ptr)
{
   return (topo_ptr->ouput_buffer_num_unit_frames != topo_ptr->main_th_ouput_buffer_num_unit_frames);
}

static void topo_update_module_output_buffers(topo_struct *topo_ptr, uint32_t module_index)
{
   module_info *module_ptr = &topo_ptr->module[module_index];
   if (module_ptr->is_in_chain)
   {
      for (uint32_t port_idx = 0; port_idx < module_ptr->num_output_ports; port_idx++)
      {
         const topo_port_index_t current_port_index = {module_index, port_idx, topo_port_index_t::OUTPUT_PORT };
         topo_module_setup_output_buffers(topo_ptr, &current_port_index);
      }
   }
   return;
}

static bool_t topo_handle_pending_events(topo_struct *topo_ptr, uint32_t start_module_index,uint32_t port_id)
{
   if (!topo_ptr->data_format_received[port_id])
   {
       return FALSE;
   }

   if (topo_ptr->currently_handling_events)
   {
       return FALSE;
   }

   topo_ptr->currently_handling_events = TRUE;

   topo_media_event_t output_media_event;
   output_media_event.event_is_pending = FALSE;
   memset(&output_media_event.event,0,sizeof(topo_media_fmt_t));

   if (TOPO_INVALID_MODULE_INDEX == start_module_index)
   {
       topo_media_fmt_t in_format;
       topo_adsp_media_fmt_to_topo(&topo_ptr->in_format[port_id], &in_format);

       topo_port_index_t main_input = topo_graph_get_main_input(&topo_ptr->graph,port_id);
       topo_media_event_t *downstream_media_event = (TOPO_INVALID_MODULE_INDEX == main_input.module_index) ? &output_media_event : &topo_ptr->module[main_input.module_index].input_ports_ptr[main_input.port_index].media_event;
       start_module_index = main_input.module_index;
       topo_media_fmt_t new_in_format;
       topo_adsp_media_fmt_to_topo(&topo_ptr->in_format[port_id], &new_in_format);
       topo_set_media_event(downstream_media_event, &new_in_format);
   }

   //MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_handle_pending_events: port id %lu and start module index %lu",topo_ptr->obj_id, port_id, start_module_index);


   module_info *m = topo_ptr->module;

   // The media type must be passed to disabled modules also since some modules get enabled/disabled based on media type.
   for (uint32_t i = start_module_index; i < topo_graph_num_modules(&topo_ptr->graph); i++)
        {
      bool_t propagate_for_disabled_module = topo_module_is_propagate_for_disabled_module(&topo_ptr->module[i]);

      topo_module_set_pending_input_media(&m[i], topo_ptr->obj_id);
      bool_t module_state_changed = topo_check_update_module_process_state(topo_ptr, i);

      if(m[i].is_in_chain)
      {
         for (uint32_t j = 0; j < m[i].num_output_ports; j++)
            {
            if(module_state_changed || m[i].output_ports_ptr[j].media_event.event_is_pending)
            {
               topo_port_index_t current_port = { i, j, topo_port_index_t::OUTPUT_PORT };
               topo_port_index_t connected_port = topo_graph_get_next_module(&topo_ptr->graph, current_port);

               topo_media_event_t *downstream_media_event = (TOPO_INVALID_MODULE_INDEX == connected_port.module_index) ? &output_media_event : &m[connected_port.module_index].input_ports_ptr[connected_port.port_index].media_event;
               topo_set_media_event(downstream_media_event, &m[i].output_ports_ptr[j].media_event.event);
        // Handle enable/disable status
               m[i].output_ports_ptr[j].media_event.event_is_pending = FALSE;
           }
            }
        }
        else
        {
         QURT_ELITE_ASSERT(topo_module_is_disablement_possible(&m[i]));
         if (module_state_changed || propagate_for_disabled_module)
        {
            topo_port_index_t current_port = { i, 0, topo_port_index_t::OUTPUT_PORT };
            topo_port_index_t connected_port = topo_graph_get_next_module(&topo_ptr->graph, current_port);

            topo_media_event_t *downstream_media_event = (TOPO_INVALID_MODULE_INDEX == connected_port.module_index) ? &output_media_event : &m[connected_port.module_index].input_ports_ptr[connected_port.port_index].media_event;
            topo_set_media_event(downstream_media_event, &m[i].input_ports_ptr[0].media_event.event);
            }
        }
        //MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: module ID %lu , inpout num channels %lu output num channels %lu",topo_ptr->obj_id, m[i].module_id, m[i].input_ports_ptr[0].media_event.event.f.num_channels, m[i].output_ports_ptr[0].media_event.event.f.num_channels);


    }

    if(topo_is_output_unit_frames_changed(topo_ptr))
    {
        for (uint32_t i = start_module_index; i < topo_graph_num_modules(&topo_ptr->graph); i++)
        {
           topo_update_module_output_buffers(topo_ptr,i);
        }
    }

    bool_t output_media_format_changed = FALSE;
    {
       topo_media_fmt_t out_format;
       topo_adsp_media_fmt_to_topo(&topo_ptr->out_format, &out_format);
            // Set the media format from the last module.
      if (output_media_event.event_is_pending && !topo_is_equal_media_format(&output_media_event.event, &out_format))
       {
         topo_topo_media_fmt_to_adsp(&output_media_event.event, &topo_ptr->out_format);
         topo_print_media_format(topo_ptr->obj_id, &output_media_event.event);

          topo_raise_output_media_type_event(topo_ptr);
          output_media_format_changed = TRUE;
        }
      output_media_event.event_is_pending = FALSE;
    }
    // if fractional resampling is detected, allocate internal buffer.

    if (port_id == TOPO_PRIMARY_STRM) // TODO : Check if need to allocate buffer for far port and how?
    {
        if (
                ((is_sampling_rate_fractional(topo_ptr->in_format[port_id].samplingRate) ^ is_sampling_rate_fractional(topo_ptr->out_format.samplingRate)) &&
                topo_ptr->is_input_output_realtime && (!topo_isBgThreadReq(topo_ptr)))// || (topo_ptr->max_in_streams > 1)
                )
        {
            //call function to allocate the internal buffer
            ADSPResult result = topo_internal_buf_create_internal_buffer(topo_ptr);
            if (!ADSP_SUCCEEDED(result))
            {
                MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_topo: Out of Memory while creating internal buffer for fractional conversion!");
            }
        }
        else
        {
            if(NULL != topo_ptr->internal_buf.pBuf)
            {
                //topo_internal_buf_copy_remaining_data_to_ext_output_buf();
                topo_internal_buf_destroy_internal_buffer(topo_ptr);
            }
        }
    }

   topo_ptr->currently_handling_events = FALSE;

    topo_update_chain_KPPS(topo_ptr);
    topo_update_chain_bandwidth(topo_ptr);
    topo_update_chain_delay(topo_ptr);
   topo_update_chain_headroom(topo_ptr);

   return output_media_format_changed;
}

static void topo_add_to_module_list(topo_struct *topo_ptr, topo_port_index_t *module_port_ptr, const topo_port_index_t *prev_port_ptr)
{
    uint32_t module_index = module_port_ptr->module_index;
   QURT_ELITE_ASSERT(topo_module_is_disablement_possible(&topo_ptr->module[module_index]));
   QURT_ELITE_ASSERT(topo_port_index_t::OUTPUT_PORT == prev_port_ptr->type);
   bool_t is_ec_module = (topo_ptr->ec_extn_info && (module_index == topo_ptr->ec_extn_info->module_index));
    module_info* m = topo_ptr->module;
   // Do not entertain enable request for far topo modules if ec module is disabled in the chain
   if(topo_ptr->ec_extn_info &&  !m[topo_ptr->ec_extn_info->module_index].is_in_chain)
   {
       if(module_index < topo_ptr->ec_extn_info->num_far_modules)
           return;
   }

   uint32_t prev_module_index = prev_port_ptr->module_index;

   const topo_port_index_t current_in_port = { module_port_ptr->module_index, module_port_ptr->port_index, topo_port_index_t::INPUT_PORT };
   const topo_port_index_t current_out_port = { module_index, 0, topo_port_index_t::OUTPUT_PORT };

    if (!m[module_index].is_in_chain)
    {
        MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Adding Module-Instance with ID 0x%lX-0x%lX to the processing chain.",
                topo_ptr->obj_id, m[module_index].module_id, m[module_index].instance_id);
        const topo_port_index_t next_port = topo_module_next_port(topo_ptr, prev_port_ptr);
        m[module_index].input_ports_ptr[current_in_port.port_index].connected_port = *prev_port_ptr;
        m[module_index].output_ports_ptr[current_out_port.port_index].connected_port = next_port;

        if(TOPO_INVALID_MODULE_INDEX == prev_module_index)
        {
         topo_ptr->first_port[TOPO_PRIMARY_STRM] = current_in_port;
        }
        else if(TOPO_INVALID_MODULE_INDEX_1 == prev_module_index)
        {
         topo_ptr->first_port[TOPO_FAR_STRM] = current_in_port;
        }
        else
        {
            topo_out_port_from_index(topo_ptr, prev_port_ptr)->connected_port = current_in_port;
        }

      if (TOPO_INVALID_MODULE_INDEX == next_port.module_index)
        {
         topo_ptr->last_port = current_out_port;
        }
        else
        {
         topo_in_port_from_index(topo_ptr, &next_port)->connected_port = current_out_port;
        }

        // Enable a module only if all ports are connected, currently TOPO_FAR_STRM is last port
        // TODO : Check how to handle out of order enable call for input ports
        if(current_in_port.port_index == (m[module_index].num_input_ports -1))   //(topo_ptr->max_in_streams-1))
        m[module_index].is_in_chain = TRUE;
        // If module is EC, restore module state
        if(is_ec_module)
        {
            for(uint32_t num_far_modules = 0;num_far_modules < topo_ptr->ec_extn_info->num_far_modules;num_far_modules++)
            {
                m[num_far_modules].is_in_chain = topo_ptr->ec_extn_info->is_far_moudle_in_chain[num_far_modules];
            }
        }
        if(m[module_index].num_input_ports)
            topo_ptr->active_in_streams += (m[module_index].num_input_ports - 1);
        QURT_ELITE_ASSERT(topo_ptr->active_in_streams <= topo_ptr->max_in_streams);
    }
    else
    {
        if(TOPO_INVALID_MODULE_INDEX != prev_module_index || TOPO_INVALID_MODULE_INDEX_1 != prev_module_index)
        {
         ASSERT_PORTS_EQUAL(prev_port_ptr, &current_in_port);
        }
      else
      {
         ASSERT_PORTS_EQUAL(&topo_ptr->first_port[current_in_port.port_index], &current_in_port);
        }
      ASSERT_PORTS_EQUAL(&m[module_index].input_ports_ptr[current_in_port.port_index].connected_port, prev_port_ptr);
    }
}

static void topo_remove_from_module_list(topo_struct *topo_ptr, uint32_t module_index)
{
    QURT_ELITE_ASSERT(topo_module_is_disablement_possible(&topo_ptr->module[module_index]));
    module_info* m = topo_ptr->module;

       // Do not entertain disable request for far topo modules if ec module is disabled in the chain
       if(topo_ptr->ec_extn_info &&  !m[topo_ptr->ec_extn_info->module_index].is_in_chain)
       {
           if(module_index < topo_ptr->ec_extn_info->num_far_modules)
               return;
       }

    // TODO : Handle far data if far chain is disabled?
    if(m[module_index].is_in_chain)
    {
        MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO, "P%hX audproc_svc: Removing Module-Instance with ID 0x%lX-0x%lX from the processing chain.",
                topo_ptr->obj_id, m[module_index].module_id,  m[module_index].instance_id);

        const topo_port_index_t current_in_port = { module_index, 0, topo_port_index_t::INPUT_PORT };
        const topo_port_index_t current_out_port = { module_index, 0, topo_port_index_t::OUTPUT_PORT };

        bool_t is_first_port = FALSE;
        bool_t is_ec_module = (topo_ptr->ec_extn_info && (module_index == topo_ptr->ec_extn_info->module_index));
        if (topo_ptr->first_port[TOPO_PRIMARY_STRM].module_index == module_index)
        {
            topo_ptr->first_port[TOPO_PRIMARY_STRM] = topo_module_next_port(topo_ptr, &current_out_port);
            is_first_port = TRUE;
        }
        if( topo_ptr->first_port[TOPO_FAR_STRM].module_index == module_index)
        {
            // TODO : Check if need to make this invalid in case of single EC module in entire topo
            topo_ptr->first_port[TOPO_FAR_STRM] = topo_module_next_port(topo_ptr, &current_out_port);
            is_first_port = TRUE;
        }
        if(!is_first_port)
        {
            topo_port_index_t prev_port = topo_module_prev_port(topo_ptr, &current_in_port);
            m[prev_port.module_index].output_ports_ptr[prev_port.port_index].connected_port = topo_module_next_port(topo_ptr, &current_out_port);
        }

        if (topo_ptr->last_port.module_index == module_index)
        {
            topo_ptr->last_port = topo_module_prev_port(topo_ptr, &current_in_port);
        }
        else
        {
            topo_port_index_t next_port = topo_module_next_port(topo_ptr, &current_out_port);
            m[next_port.module_index].input_ports_ptr[next_port.port_index].connected_port = topo_module_prev_port(topo_ptr, &current_in_port);
        }
        m[module_index].is_in_chain = FALSE;

        if(is_ec_module)
        {
            for(uint32_t num_far_modules = 0;num_far_modules < topo_ptr->ec_extn_info->num_far_modules;num_far_modules++)
            {
                topo_ptr->ec_extn_info->is_far_moudle_in_chain[num_far_modules] = m[num_far_modules].is_in_chain;
                m[num_far_modules].is_in_chain = FALSE;
            }
        }
        if(m[module_index].num_input_ports)
            topo_ptr->active_in_streams -= (m[module_index].num_input_ports - 1);
    }
}

static void topo_module_setup_output_buffers(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr)
{
   topo_buf_t *out_buf = &topo_ptr->module[port_index_ptr->module_index].output_ports_ptr[port_index_ptr->port_index].topo_buf;

   // Free any previously allocated output buffers
   topo_buf_manager_return_bufs(&topo_ptr->buf_mgr, out_buf);
   topo_buf_manager_clear_all_bufs(&topo_ptr->buf_mgr);
   out_buf->num_bufs = 0;
   uint32_t frame_size_in_samples = 0;
   uint32_t size_per_buffer = 0;
   uint32_t module_out_freq = topo_ptr->module[port_index_ptr->module_index].output_ports_ptr[port_index_ptr->port_index].media_event.event.f.sampling_rate;

   // Set up output buffers
   {
      uint32_t num_bufs_to_alloc = 0;
      topo_media_fmt_t *out_format_ptr = &(topo_out_port_from_index(topo_ptr, port_index_ptr)->media_event.event);

      if ((CAPI_V2_IEC61937_PACKETIZED == out_format_ptr->h.data_format) || (CAPI_V2_RAW_COMPRESSED == out_format_ptr->h.data_format) || (CAPI_V2_INTERLEAVED == out_format_ptr->f.data_interleaving))
      {
         num_bufs_to_alloc = 1;
      }
      else
      {
         QURT_ELITE_ASSERT(out_format_ptr->f.num_channels != CAPI_V2_DATA_FORMAT_INVALID_VAL);
         num_bufs_to_alloc = out_format_ptr->f.num_channels;

      }

      QURT_ELITE_ASSERT (num_bufs_to_alloc <= SIZE_OF_ARRAY(out_buf->buf_ptrs));
      if ((CAPI_V2_IEC61937_PACKETIZED == out_format_ptr->h.data_format) || (CAPI_V2_RAW_COMPRESSED == out_format_ptr->h.data_format))
      {
         out_buf->bytes_to_samples_factor = 1;
         size_per_buffer =  topo_ptr->compress_frame_size * out_buf->bytes_to_samples_factor;
      }
      else
      {
         if (CAPI_V2_INTERLEAVED == out_format_ptr->f.data_interleaving)
         {
            out_buf->bytes_to_samples_factor = out_format_ptr->f.bits_per_sample / 8 * out_format_ptr->f.num_channels;
         }
         else
         {
            out_buf->bytes_to_samples_factor = out_format_ptr->f.bits_per_sample / 8;
         }
         elite_svc_get_frame_size(module_out_freq,&frame_size_in_samples);
         size_per_buffer =  topo_ptr->ouput_buffer_num_unit_frames * frame_size_in_samples * out_buf->bytes_to_samples_factor;
      }

      out_buf->size_per_buffer = size_per_buffer;
      out_buf->valid_data_start = out_buf->valid_data_end = 0;
      out_buf->num_bufs = num_bufs_to_alloc;
   }
}

static bool_t topo_zero_modules_created(const topo_struct *topo_ptr)
{
   return (0 == topo_graph_num_modules(&topo_ptr->graph));
}

static void topo_buf_manager_init(topo_buf_manager *buf_mgr_ptr)
{
    buf_mgr_ptr->current_memory_allocated = buf_mgr_ptr->max_memory_allocated = 0;

    for (uint32_t i = 0; i < SIZE_OF_ARRAY(buf_mgr_ptr->bufs); i++)
    {
        buf_mgr_ptr->bufs[i].present = FALSE;
        buf_mgr_ptr->bufs[i].size = 0;
        buf_mgr_ptr->bufs[i].ptr = NULL;
    }
}

static void topo_buf_manager_deinit(topo_buf_manager *buf_mgr_ptr)
{
#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: topo topo_buf manager max size allocated was: %lu", buf_mgr_ptr->max_memory_allocated);
#endif
    buf_mgr_ptr->max_memory_allocated = 0;

    for (uint32_t i = 0; i < SIZE_OF_ARRAY(buf_mgr_ptr->bufs); i++)
    {
        if (NULL != buf_mgr_ptr->bufs[i].ptr)
        {
            QURT_ELITE_ASSERT(buf_mgr_ptr->bufs[i].present);

            buf_mgr_ptr->current_memory_allocated -= buf_mgr_ptr->bufs[i].size;
            qurt_elite_memory_free(buf_mgr_ptr->bufs[i].ptr);
            buf_mgr_ptr->bufs[i].size = 0;
            buf_mgr_ptr->bufs[i].ptr = NULL;
            buf_mgr_ptr->bufs[i].present = FALSE;
        }
    }

    QURT_ELITE_ASSERT(0 == buf_mgr_ptr->current_memory_allocated);
}

static ADSPResult topo_buf_manager_get_bufs(topo_buf_manager *buf_mgr_ptr, topo_buf_t *buf_ptr)
{
    bool_t allocating_buffers = FALSE;
    uint32_t current_index = 0;
    for (uint32_t i = 0; i < buf_ptr->num_bufs; i++)
    {
        QURT_ELITE_ASSERT(NULL == buf_ptr->buf_ptrs[i]);

        if (!allocating_buffers)
        {
            while(current_index < SIZE_OF_ARRAY(buf_mgr_ptr->bufs))
            {
                if (buf_mgr_ptr->bufs[current_index].present && (buf_ptr->size_per_buffer <= buf_mgr_ptr->bufs[current_index].size))
                {
                    buf_ptr->buf_ptrs[i] = buf_mgr_ptr->bufs[current_index].ptr;
                    buf_mgr_ptr->bufs[current_index].present = FALSE;
                    break;
                }
                else
                {
                    current_index++;
                }
            }

            if (current_index >= SIZE_OF_ARRAY(buf_mgr_ptr->bufs))
            {
                allocating_buffers = TRUE;
                topo_buf_manager_clear_all_bufs(buf_mgr_ptr); // This will clear out any smaller buffers that are still present.
                current_index = 0;
            }
        }

        if (allocating_buffers)
        {
            void *ptr = qurt_elite_memory_malloc(buf_ptr->size_per_buffer, QURT_ELITE_HEAP_DEFAULT);
            if (NULL != ptr)
            {
                buf_ptr->buf_ptrs[i] = (int8_t*)ptr;

                // Search for an empty slot
                while(current_index < SIZE_OF_ARRAY(buf_mgr_ptr->bufs))
                {
                    if (NULL == buf_mgr_ptr->bufs[current_index].ptr)
                    {
                        buf_mgr_ptr->bufs[current_index].size = buf_ptr->size_per_buffer;
                        buf_mgr_ptr->bufs[current_index].ptr = (int8_t*)ptr;
                        buf_mgr_ptr->bufs[current_index].present = FALSE;
                        break;
                    }
                    else
                    {
                        current_index++;
                    }
                }
                // If no empty slot found, don't store this buffer

                buf_mgr_ptr->current_memory_allocated += buf_ptr->size_per_buffer;
                if (buf_mgr_ptr->current_memory_allocated > buf_mgr_ptr->max_memory_allocated)
                {
                    buf_mgr_ptr->max_memory_allocated = buf_mgr_ptr->current_memory_allocated;
                }
            }
            else
            {
                // Cleanup
                topo_buf_manager_return_bufs(buf_mgr_ptr, buf_ptr);
                return ADSP_ENOMEMORY;
            }
        }
    }

    return ADSP_EOK;
}

void topo_reset_port_buffers(topo_struct *topo_ptr, uint32_t port_id)
{
	/*
  if(topo_is_primary_in_port(port_id)) // if it primary port then clear all the buffers as processing will not happen until primary port is started
  {

      for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
        {
           module_info *module_struct_ptr = &topo_ptr->module[i];
           for (uint32_t j = 0; j < module_struct_ptr->num_output_ports; j++)
           {
              const topo_port_index_t port_index = { i, j, topo_port_index_t::OUTPUT_PORT } ;

              topo_buf_t *out_buf_ptr = topo_module_get_output_buffer(topo_ptr, &port_index);

              if (!topo_buffer_is_buf_empty(out_buf_ptr))
              {
                  out_buf_ptr->valid_data_end = out_buf_ptr->valid_data_start = 0;
                 //topo_buf_manager_return_bufs(&topo_ptr->buf_mgr, out_buf_ptr);
              }
           }
         }
      //topo_buf_manager_clear_all_bufs(&topo_ptr->buf_mgr);

  
  }
  */
  if(!topo_is_primary_in_port(port_id))   // for secondary clear buffers for only those modules which are in far topology
  {
     uint32_t start_module_index;

     topo_port_index_t main_input = topo_graph_get_main_input(&topo_ptr->graph,port_id);
     start_module_index = main_input.module_index;

     module_info *m = topo_ptr->module;

     for (uint32_t i = start_module_index; i < topo_ptr->ec_extn_info->num_far_modules; i++)
     {
         module_info *module_struct_ptr = &m[i];
         for (uint32_t j = 0; j < module_struct_ptr->num_output_ports; j++)
         {
             const topo_port_index_t port_index = { i, j, topo_port_index_t::OUTPUT_PORT } ;

             topo_buf_t *out_buf_ptr = topo_module_get_output_buffer(topo_ptr, &port_index);

             if (!topo_buffer_is_buf_empty(out_buf_ptr))
             {
                 out_buf_ptr->valid_data_end = out_buf_ptr->valid_data_start = 0;
                 topo_buf_manager_return_bufs(&topo_ptr->buf_mgr, out_buf_ptr);
             }
         }
         topo_send_algo_reset(topo_ptr, i);
     }
  }

  return;
}

static void topo_send_algo_reset(topo_struct *topo_ptr, uint32_t module_idx)
{

  capi_v2_prop_t prop;
  capi_v2_err_t err_code;

  prop.id = CAPI_V2_ALGORITHMIC_RESET;
  prop.payload.actual_data_len = 0;            // TODO actual_data_len data len
  prop.payload.max_data_len = 0;               // TODO max data len
  prop.payload.data_ptr = NULL;
  prop.port_info.is_valid = FALSE;
  capi_v2_proplist_t proplist;
  proplist.props_num = 1;
  proplist.prop_ptr = &prop;


  module_info *module_struct_ptr = &topo_ptr->module[module_idx];
  capi_v2_t *module_ptr = module_struct_ptr->module_ptr;

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_send_algo_reset: send algo reset to Module-Instance with ID %lX-%lX", topo_ptr->obj_id, module_struct_ptr->module_id, module_struct_ptr->instance_id);

  err_code = module_ptr->vtbl_ptr->set_properties(module_ptr, &proplist);
  if (CAPI_V2_FAILED(err_code))
  {
     MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with ID %lX-%lX"
          "failed to reset.", topo_ptr->obj_id, module_struct_ptr->module_id, module_struct_ptr->instance_id);
  }

  return;
}


static void topo_buf_manager_return_bufs(topo_buf_manager *buf_mgr_ptr, topo_buf_t *buf_ptr)
{
    for (uint32_t i = 0; i < buf_ptr->num_bufs; i++)
    {
        if (NULL != buf_ptr->buf_ptrs[i])
        {
            for (uint32_t j = 0; j < SIZE_OF_ARRAY(buf_mgr_ptr->bufs); j++)
            {
                if (buf_ptr->buf_ptrs[i] == buf_mgr_ptr->bufs[j].ptr)
                {
                    QURT_ELITE_ASSERT(!buf_mgr_ptr->bufs[j].present);
                    QURT_ELITE_ASSERT(buf_mgr_ptr->bufs[j].size >= buf_ptr->size_per_buffer);
                    buf_mgr_ptr->bufs[j].present = TRUE;
                    buf_ptr->buf_ptrs[i] = NULL;
                    break;
                }
            }

            if (NULL != buf_ptr->buf_ptrs[i])
            {
                qurt_elite_memory_free(buf_ptr->buf_ptrs[i]);
                buf_ptr->buf_ptrs[i] = NULL;
                buf_mgr_ptr->current_memory_allocated -= buf_ptr->size_per_buffer;
            }
        }
    }
}

static void topo_buf_manager_clear_all_bufs(topo_buf_manager *buf_mgr_ptr)
{
    if (buf_mgr_ptr->current_memory_allocated > 0)
    {
        for (uint32_t i = 0; i < SIZE_OF_ARRAY(buf_mgr_ptr->bufs); i++)
        {
            if ((NULL != buf_mgr_ptr->bufs[i].ptr) && buf_mgr_ptr->bufs[i].present)
            {
                qurt_elite_memory_free(buf_mgr_ptr->bufs[i].ptr);
                buf_mgr_ptr->current_memory_allocated -= buf_mgr_ptr->bufs[i].size;
                buf_mgr_ptr->bufs[i].ptr = NULL;
                buf_mgr_ptr->bufs[i].size = 0;
                buf_mgr_ptr->bufs[i].present = FALSE;
            }
        }
    }
}

void topo_compressed_reinit_with_output_sample_size(topo_struct *topo_ptr, uint16_t compress_frame_samples)
{
    if (topo_is_compressed(topo_ptr->in_format[TOPO_PRIMARY_STRM].bitstreamFormat))
    {
        uint32_t compress_frame_size = compress_frame_samples * topo_get_bytes_per_sample(topo_ptr->in_format[TOPO_PRIMARY_STRM].bitsPerSample) * topo_ptr->in_format[TOPO_PRIMARY_STRM].channels;
        if (compress_frame_size == topo_ptr->compress_frame_size)
        {
            return;
        }
        topo_ptr->compress_frame_size = compress_frame_size;


        for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
        {
            if (topo_ptr->module[i].is_in_chain)
            {
                for (uint32_t j = 0; j < topo_ptr->module[i].num_output_ports; j++)
                {
                    const topo_port_index_t port_index = {i, j, topo_port_index_t::OUTPUT_PORT};
                    topo_module_setup_output_buffers(topo_ptr, &port_index);
                }
            }
        }
    }
    return ;
}

static void topo_set_headroom(topo_struct *topo_ptr)
{
    struct soft_vol_alg_headroom
    {
       asm_stream_param_data_v2_t param_payload;
       uint32_t headroom;
    } alg_headroom_param;

    alg_headroom_param.param_payload.module_id = AUDPROC_MODULE_ID_VOL_CTRL;
    alg_headroom_param.param_payload.param_id = AUDPROC_PARAM_ID_ALGORITHMIC_HEADROOM;
    alg_headroom_param.param_payload.param_size = sizeof(uint32_t);
    alg_headroom_param.param_payload.reserved = 0;
    alg_headroom_param.headroom = topo_ptr->topo_headroom;

    topo_set_param(topo_ptr, &alg_headroom_param, sizeof(alg_headroom_param), FIRST_INSTANCE);
}

static void topo_set_ramp_period_to_virtualizer(topo_struct *topo_ptr, const uint32_t index)
{
    ADSPResult errCode;
    uint32_t dummy = 0;

    audproc_soft_step_volume_params_t volume_info;
    errCode = topo_get_param(topo_ptr,
               &volume_info,
               sizeof(volume_info),
               topo_ptr->module[index].module_id,
               topo_ptr->module[index].instance_id,
               AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS,
               &dummy,
               GIVEN_INSTANCE);

    uint32_t virtualizer_module_id, virtualizer_volume_ramp_param_id;
    if( ADSP_SUCCEEDED( errCode |
            topo_get_virtualizer_module_id(&virtualizer_module_id) |
            topo_get_virtualizer_volume_ramp_param_id(&virtualizer_volume_ramp_param_id) ) )
    {
        struct virtualizer_ramp_period_set_param_t
        {
           asm_stream_param_data_v2_t param_payload;
           uint32_t period;
        } ramp_period_param;

        ramp_period_param.param_payload.module_id = virtualizer_module_id;
        ramp_period_param.param_payload.param_id = virtualizer_volume_ramp_param_id;
        ramp_period_param.param_payload.param_size = sizeof(uint32_t);
        ramp_period_param.param_payload.reserved = 0;
        ramp_period_param.period = volume_info.period;
        topo_set_param(topo_ptr, &ramp_period_param, sizeof(ramp_period_param), FIRST_INSTANCE);
    }
}

static void topo_set_delay_ptr_virtualizer(topo_struct *topo_ptr)
{
    uint32_t virtualizer_module_id, virtualizer_delay_param_id;
    if( ADSP_SUCCEEDED( topo_get_virtualizer_module_id(&virtualizer_module_id) |
            topo_get_virtualizer_delay_param_id(&virtualizer_delay_param_id) ) )
    {
        struct virtualizer_delay_set_param_t
        {
           asm_stream_param_data_v2_t param_payload;
           uint32_t *delay_ptr;
        } delay_param;

        delay_param.param_payload.module_id = virtualizer_module_id;
        delay_param.param_payload.param_id = virtualizer_delay_param_id;
        delay_param.param_payload.param_size = sizeof(uint32_t*);
        delay_param.param_payload.reserved = 0;
        delay_param.delay_ptr = &(topo_ptr->delay);
        topo_set_param(topo_ptr, &delay_param, sizeof(delay_param), FIRST_INSTANCE);
    }
}

static void topo_set_ec_far_media_format(topo_struct *topo_ptr,const AdspAudioMediaFormatInfo_t *req_media_format)
{
    struct ec_extn_mfc_set_param_t
    {
        asm_stream_param_data_v2_t param_payload;
        audproc_mfc_output_media_fmt_t out_format;
    } ec_extn_mfc_param;

    ec_extn_mfc_param.param_payload.module_id = AUDPROC_MODULE_ID_MFC_FAR;
    ec_extn_mfc_param.param_payload.param_id = AUDPROC_PARAM_ID_MFC_OUTPUT_MEDIA_FORMAT;
    ec_extn_mfc_param.param_payload.param_size = sizeof(audproc_mfc_output_media_fmt_t);
    ec_extn_mfc_param.param_payload.reserved = 0;
    ec_extn_mfc_param.out_format.bits_per_sample = req_media_format->bitsPerSample;
    ec_extn_mfc_param.out_format.num_channels = req_media_format->channels;
    ec_extn_mfc_param.out_format.sampling_rate = req_media_format->samplingRate;
    for(uint8_t ch_index=0;ch_index<ec_extn_mfc_param.out_format.num_channels;ch_index++)
    {
        ec_extn_mfc_param.out_format.channel_type[ch_index] = (uint16_t)req_media_format->channelMapping[ch_index];
    }
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_set_ec_far_media_format: set outpout media fmt on far end num ch %lu", topo_ptr->obj_id, ec_extn_mfc_param.out_format.num_channels);


    topo_set_param(topo_ptr, &ec_extn_mfc_param, sizeof(ec_extn_mfc_param), FIRST_INSTANCE);
}

static void topo_check_empty_last_buffer(topo_struct *topo_ptr)
{
   uint32_t copy_size = 0;

   // associated with this port must be copied into the final output buffer.
   topo_buf_t *port_out_buf_ptr = &topo_ptr->module[topo_ptr->last_port.module_index].output_ports_ptr[topo_ptr->last_port.port_index].topo_buf;

   topo_buf_t *main_out_buf_ptr = &topo_ptr->main_output_buffer;

   if (!topo_buffer_is_buf_empty(port_out_buf_ptr))
   {
      uint32_t output_buf_len = main_out_buf_ptr->size_per_buffer - main_out_buf_ptr->valid_data_end;
      uint32_t port_output_buf_len = port_out_buf_ptr->valid_data_end - port_out_buf_ptr->valid_data_start;

      for (uint32_t ch = 0; ch < port_out_buf_ptr->num_bufs; ch++)
      {
         copy_size = memscpy(&main_out_buf_ptr->buf_ptrs[ch][main_out_buf_ptr->valid_data_end],
               output_buf_len,
               &port_out_buf_ptr->buf_ptrs[ch][port_out_buf_ptr->valid_data_start],
               port_output_buf_len);
      }

      if(main_out_buf_ptr->valid_data_end == 0)
      {
         main_out_buf_ptr->flags =  port_out_buf_ptr->flags;
         module_info *module_struct_ptr = &topo_ptr->module[topo_ptr->last_port.module_index];
         uint64_t timestamp_offset = 0;
         if(main_out_buf_ptr->flags.is_timestamp_valid)
         {
            timestamp_offset = ((port_out_buf_ptr->valid_data_start/port_out_buf_ptr->bytes_to_samples_factor) * 1000000);
            timestamp_offset/=(module_struct_ptr->output_ports_ptr->media_event.event.f.sampling_rate);
            main_out_buf_ptr->timestamp = port_out_buf_ptr->timestamp + timestamp_offset;
         }

      }

      main_out_buf_ptr->valid_data_end += copy_size;
      port_out_buf_ptr->valid_data_start +=copy_size;

      if (copy_size == port_output_buf_len)
      {
         QURT_ELITE_ASSERT(topo_buffer_is_buf_empty(port_out_buf_ptr));
         port_out_buf_ptr->valid_data_start = port_out_buf_ptr->valid_data_end = 0;
         topo_buf_manager_return_bufs(&topo_ptr->buf_mgr, port_out_buf_ptr);
      }
   }
}

static void topo_update_chain_KPPS(topo_struct *topo_ptr)
{
   if (topo_ptr->currently_handling_events)
{
      return;
   }

   uint32_t old_KPPS = topo_ptr->KPPS;
   uint32_t new_KPPS = 0;

    if (topo_ptr->data_format_received[TOPO_PRIMARY_STRM])
    {
        new_KPPS = TOPO_KPPS; // Base number
        for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
        {
            if (topo_ptr->module[i].is_in_chain)
            {
                new_KPPS += topo_ptr->module[i].KPPS;
            }
        }
    }

   if (new_KPPS != old_KPPS)
      {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: PP KPPS vote changed from %lu to %lu.", topo_ptr->obj_id, old_KPPS, new_KPPS);
      topo_ptr->KPPS = new_KPPS;
      if (NULL != topo_ptr->event_client)
      {
         topo_ptr->event_client->vtbl->KPPS(topo_ptr->event_client, new_KPPS);
   }
   }
}

static void topo_update_chain_bandwidth(topo_struct *topo_ptr)
{
   if (topo_ptr->currently_handling_events)
   {
      return;
   }

   uint32_t ff_num = TOPO_BW_FUDGE_FACTOR_NUM;
   uint32_t ff_den = TOPO_BW_FUDGE_FACTOR_DEN;

   uint32_t old_bandwidth = topo_ptr->bandwidth;
   uint32_t new_bandwidth = TOPO_BANDWIDTH; // Base number
   uint32_t max_bandwidth = new_bandwidth;

   uint32_t num_mod_present = 0;

   for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
   {
      if (topo_ptr->module[i].is_in_chain)
      {
            uint32_t bw=topo_ptr->module[i].bandwidth;
          new_bandwidth += bw;
          if (bw>max_bandwidth) max_bandwidth=bw;

          num_mod_present++;
      }
   }

   //check if any modules contribution is 90% of the new_bandwidth.
   if ( (((uint64_t)max_bandwidth*100)/new_bandwidth) >= TOPO_BW_FUDGE_FACTOR_THRESHOLD)
      {
         ff_num = TOPO_BW_FUDGE_FACTOR_NUM_90_10;
         ff_den = TOPO_BW_FUDGE_FACTOR_DEN_90_10;
   }

   if(num_mod_present <= 1)
   {
      ff_num = 1;
      ff_den = 1;
   }

   new_bandwidth = new_bandwidth*ff_num/ff_den;

   if (new_bandwidth != old_bandwidth)
   {
      MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: PP bandwidth vote changed from %lu to %lu. Fudge Factor %lu/%lu",
            topo_ptr->obj_id, old_bandwidth, new_bandwidth,ff_num,ff_den);
      topo_ptr->bandwidth = new_bandwidth;
      if (NULL != topo_ptr->event_client)
      {
         topo_ptr->event_client->vtbl->bandwidth(topo_ptr->event_client, new_bandwidth);
      }
   }
}


static void topo_update_chain_delay(topo_struct *topo_ptr)
{
   if (topo_ptr->currently_handling_events)
{
      return;
   }

   uint32_t old_delay = topo_ptr->delay;
   uint32_t new_delay = 0; // Base number
   for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
   {
      if (topo_ptr->module[i].is_in_chain)
   {
         new_delay += topo_ptr->module[i].delay_in_us;
      }
   }

   if (new_delay != old_delay)
      {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: PP algorithmic delay changed from %lu us to %lu us .", topo_ptr->obj_id, old_delay, new_delay);
      topo_ptr->delay = new_delay;
      if (NULL != topo_ptr->event_client)
         {
         topo_ptr->event_client->vtbl->algorithmic_delay(topo_ptr->event_client, new_delay);
      }
   }
}

static bool_t topo_is_data_threshold_valid(uint32_t data_threshold, topo_media_event_t *port_media_event)
{
   uint32_t frame_size_in_samples = 0;
   uint32_t num_unit_frames = 0;
   elite_svc_get_frame_size(port_media_event->event.f.sampling_rate,&frame_size_in_samples);
   if(CAPI_V2_INTERLEAVED == port_media_event->event.f.data_interleaving) // check for interleaved only
   {
       num_unit_frames = data_threshold/(port_media_event->event.f.num_channels * frame_size_in_samples *(port_media_event->event.f.bits_per_sample/8));
   }
   else
   {
       num_unit_frames = data_threshold/(frame_size_in_samples *(port_media_event->event.f.bits_per_sample/8));
   }

   if(num_unit_frames > MAX_SUPPORTED_DATA_THRESHOLD_UNIT_FRAMES) // put a max limit on data threshold to avoid processing any invalid value
   {
      return FALSE;
   }
   
   return TRUE;
}

static void topo_update_chain_port_data_threshold(topo_struct *topo_ptr, uint32_t port_id)
{
   //if (topo_ptr->currently_handling_events)
   //{
   //   return;
   //}

   uint32_t old_ouput_buffer_num_unit_frames = topo_ptr->ouput_buffer_num_unit_frames;
   uint32_t max_unit_frames = 0;
   uint32_t curr_module_input_data_threshold, curr_module_unit_frames;
   uint32_t frame_size_in_samples = 0;
   module_input_port_t *input_ports_ptr;

   for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
   {
      input_ports_ptr = &topo_ptr->module[i].input_ports_ptr[port_id];
      //if (topo_ptr->module[i].is_in_chain)
      {
         elite_svc_get_frame_size(input_ports_ptr->media_event.event.f.sampling_rate,&frame_size_in_samples);
         curr_module_input_data_threshold = input_ports_ptr->data_threshold;

         if(CAPI_V2_INTERLEAVED == input_ports_ptr->media_event.event.f.data_interleaving) // check for interleaved only
         {
            curr_module_unit_frames = curr_module_input_data_threshold/(input_ports_ptr->media_event.event.f.num_channels * frame_size_in_samples *(input_ports_ptr->media_event.event.f.bits_per_sample/8));
         }
         else
         {
            curr_module_unit_frames = curr_module_input_data_threshold/(frame_size_in_samples *(input_ports_ptr->media_event.event.f.bits_per_sample/8));
         }
         max_unit_frames = (max_unit_frames > curr_module_unit_frames)?max_unit_frames:curr_module_unit_frames;
      }
   }

   if ((max_unit_frames != old_ouput_buffer_num_unit_frames) && (max_unit_frames > 0))	  // TODO : create BG thread even if data threshold is same as main thread
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: PP data threshold changed from %lu  to %lu  .", topo_ptr->obj_id, old_ouput_buffer_num_unit_frames, max_unit_frames);
      topo_ptr->ouput_buffer_num_unit_frames =   max_unit_frames;
      //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: BG topo num unit frames %lu .", topo_ptr->obj_id, topo_ptr->ouput_buffer_num_unit_frames);
   }

   return;
}

static void topo_update_chain_headroom(topo_struct *topo_ptr)
{
   if (topo_ptr->currently_handling_events)
   {
      return;
   }

   uint32_t old_headroom = topo_ptr->topo_headroom;
   uint32_t new_headroom = 0; // Base number
   for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
   {
      if (topo_ptr->module[i].is_in_chain)
      {
         new_headroom += topo_ptr->module[i].headroom;
      }
   }

   if (new_headroom != old_headroom)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: PP headroom changed from %lu mB to %lu mB .", topo_ptr->obj_id, old_headroom, new_headroom);
      topo_ptr->topo_headroom = new_headroom;
      topo_set_headroom(topo_ptr);
   }
}

static void topo_raise_output_media_type_event(topo_struct *topo_ptr)
{
   if (NULL != topo_ptr->event_client)
    {
      topo_ptr->event_client->vtbl->output_media_format(topo_ptr->event_client, &topo_ptr->out_format);
   }
    }

static bool_t topo_is_equal_media_format(const topo_media_fmt_t *f1, const topo_media_fmt_t *f2)
    {
   bool_t is_equal = TRUE;

   is_equal = is_equal && (f1->h.data_format == f2->h.data_format);
   is_equal = is_equal && (f1->f.bits_per_sample == f2->f.bits_per_sample);
   is_equal = is_equal && (f1->f.bitstream_format == f2->f.bitstream_format);
   is_equal = is_equal && (f1->f.data_interleaving == f2->f.data_interleaving);
   is_equal = is_equal && (f1->f.data_is_signed == f2->f.data_is_signed);
   is_equal = is_equal && (f1->f.num_channels == f2->f.num_channels);
   is_equal = is_equal && (f1->f.q_factor == f2->f.q_factor);
   is_equal = is_equal && (f1->f.sampling_rate == f2->f.sampling_rate);

   if (is_equal)
        {
      if (CAPI_V2_DATA_FORMAT_INVALID_VAL != f1->f.num_channels)
      {
      for (uint32_t i = 0; i < f1->f.num_channels; i++)
        {
         is_equal = is_equal && (f1->f.channel_type[i] == f2->f.channel_type[i]);
    }
}
   }

   return is_equal;
}

/**
 * Function to set the start pause command to the volume modules in the topology, during the pause resume.
 * If the topology has one volume module, then that volume module will be set to ramp down during the pause.
 * If the topology has two volume modules, then volume2 module will be set for the soft ramp down during the pause
 * and volume1 will be set to pause state after the ramp down is complete.
 *
 * @param[in, out]
 * topo_ptr : Pointer to the topology instance structure.
 * softPauseParamPayload : pointer to the soft pause parameters of volume 1 module.
 */

void  topo_set_start_pause(topo_struct *topo_ptr , audproc_soft_pause_params_t *soft_pause_param_payload)
{
   uint32_t index;
   ADSPResult err_code;
   asm_stream_param_data_v2_t param_payload;
   err_code = topo_find_first_module_instance(topo_ptr, AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2, &index);


   if (ADSP_SUCCEEDED(err_code))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Pause Processing begin, Volume 2 present in topology",topo_ptr->obj_id);

      struct volume2_soft_pause_params_t
      {
         asm_stream_param_data_v2_t param_payload;
         audproc_soft_pause_params_t soft_pause_param;
      } volume2_soft_pause_params;

      audproc_soft_pause_params_t *soft_pause_param = (audproc_soft_pause_params_t *)soft_pause_param_payload;

      volume2_soft_pause_params.param_payload.module_id = AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2;
      volume2_soft_pause_params.param_payload.param_id = AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS;
      volume2_soft_pause_params.param_payload.param_size = sizeof(audproc_soft_pause_params_t);
      volume2_soft_pause_params.param_payload.reserved = 0;
      volume2_soft_pause_params.soft_pause_param = *soft_pause_param;

      topo_set_param(topo_ptr, &volume2_soft_pause_params, sizeof(volume2_soft_pause_params), FIRST_INSTANCE);

      param_payload.module_id = AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2;
      param_payload.param_id = AUDPROC_PARAM_ID_SOFT_PAUSE_START;
      param_payload.param_size = 0;
      param_payload.reserved = 0;

      topo_set_param(topo_ptr, &param_payload, sizeof(asm_stream_param_data_v2_t), FIRST_INSTANCE);
   }
   else
   {
      param_payload.module_id = AUDPROC_MODULE_ID_VOL_CTRL;
      param_payload.param_id = AUDPROC_PARAM_ID_SOFT_PAUSE_START;
      param_payload.param_size = 0;
      param_payload.reserved = 0;

      topo_set_param(topo_ptr, &param_payload, sizeof(asm_stream_param_data_v2_t), FIRST_INSTANCE);
   }
}

/**
 * Function to change the volume1 module state to pause after the pause ramp is done in the
 * two volume topology. The function also resets all the modules in the topology to flush out the
 * internal buffers of the modules in the topology.
 *
 * @param[in, out]
 * topo_ptr : Pointer to the topology instance structure.
 */

void topo_set_volume1_state_to_pause(topo_struct *topo_ptr)
{
   uint32_t index;
   ADSPResult err_code;
   uint32_t   us_actual_param_size = 0;
   asm_stream_param_data_v2_t param_payload;
   audproc_soft_pause_params_t soft_pause_param_payload;

   err_code = topo_find_first_module_instance(topo_ptr, AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2, &index);

   if (ADSP_SUCCEEDED(err_code))
   {
      err_code = topo_get_param(topo_ptr,
                               &soft_pause_param_payload,
                               sizeof(audproc_soft_pause_params_t), /* ParamMaxSize */
                               AUDPROC_MODULE_ID_VOL_CTRL, /* ModuleId: SoftPause is done via SoftVolumeControls */
                               0, /*Instance Id (ignored as mode is FIRST_INSTANCE).*/
                               AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS, /* ParamId */
                               &us_actual_param_size,
                               FIRST_INSTANCE);


      if (soft_pause_param_payload.enable_flag)
      {
         struct volume_soft_pause_params_t
         {
            asm_stream_param_data_v2_t param_payload;
            audproc_soft_pause_params_t soft_pause_param;
         } volume_soft_pause_params;

         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Pause ramp done, change Volume 1 to Pause state and then reset modules",topo_ptr->obj_id);

         volume_soft_pause_params.param_payload.module_id = AUDPROC_MODULE_ID_VOL_CTRL;
         volume_soft_pause_params.param_payload.param_id = AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS;
         volume_soft_pause_params.param_payload.param_size = sizeof(audproc_soft_pause_params_t);
         volume_soft_pause_params.param_payload.reserved = 0;
         volume_soft_pause_params.soft_pause_param = soft_pause_param_payload;
         volume_soft_pause_params.soft_pause_param.enable_flag = 0; // disable soft pause. set pause immediately

         topo_set_param(topo_ptr, &volume_soft_pause_params, sizeof(volume_soft_pause_params), FIRST_INSTANCE);

         param_payload.module_id = AUDPROC_MODULE_ID_VOL_CTRL;
         param_payload.param_id = AUDPROC_PARAM_ID_SOFT_PAUSE_START;
         param_payload.param_size = 0;
         param_payload.reserved = 0;

         topo_set_param(topo_ptr, &param_payload, sizeof(param_payload), FIRST_INSTANCE);

         volume_soft_pause_params.param_payload.module_id = AUDPROC_MODULE_ID_VOL_CTRL;
         volume_soft_pause_params.param_payload.param_id = AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS;
         volume_soft_pause_params.param_payload.param_size = sizeof(audproc_soft_pause_params_t);
         volume_soft_pause_params.param_payload.reserved = 0;
         volume_soft_pause_params.soft_pause_param = soft_pause_param_payload;

         topo_set_param(topo_ptr, &volume_soft_pause_params, sizeof(volume_soft_pause_params), FIRST_INSTANCE);

         topo_pause_reset (topo_ptr);
      }
    }
}

/**
 * Function to set the ramp on resume for the volumes module in the topology, during the pause resume.
 * If the topology has one volume module, then that volume module will be set to ramp up during the resume.
 * If the topology has two volume modules, then volume1 module will be set for the soft ramp up and volume2
 * will ramped up immediately.
 *
 * @param[in, out]
 * topo_ptr : Pointer to the topology instance structure.
 *
 */

void  topo_set_ramp_on_resume(topo_struct *topo_ptr)
{
   uint32_t index;
   ADSPResult err_code;
   uint32_t   us_actual_param_size = 0;
   asm_stream_param_data_v2_t param_payload;
   audproc_soft_pause_params_t soft_pause_param_payload;

   err_code = topo_find_first_module_instance(topo_ptr, AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2, &index);

   if (ADSP_SUCCEEDED(err_code))
   {
      err_code = topo_get_param(topo_ptr,
                               &soft_pause_param_payload,
                               sizeof(audproc_soft_pause_params_t), /* ParamMaxSize */
                               AUDPROC_MODULE_ID_VOL_CTRL, /* ModuleId: SoftPause is done via SoftVolumeControls */
                               0,    /*Instance Id (ignored as mode is FIRST_INSTANCE).*/
                               AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS, /* ParamId */
                               &us_actual_param_size,
                               FIRST_INSTANCE);


      if (soft_pause_param_payload.enable_flag)
      {
         struct volume_soft_pause_params_t
         {
            asm_stream_param_data_v2_t param_payload;
            audproc_soft_pause_params_t soft_pause_param;
         } volume_soft_pause_params;

         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Set Ramp on Resume to volume 2 with zero period/Set Soft Ramp on Resume to volume 1 ",topo_ptr->obj_id);

         volume_soft_pause_params.param_payload.module_id = AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2;
         volume_soft_pause_params.param_payload.param_id = AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS;
         volume_soft_pause_params.param_payload.param_size = sizeof(audproc_soft_pause_params_t);
         volume_soft_pause_params.param_payload.reserved = 0;
         volume_soft_pause_params.soft_pause_param = soft_pause_param_payload;
         volume_soft_pause_params.soft_pause_param.enable_flag = 0; // disable soft pause. set pause immediately

         topo_set_param(topo_ptr, &volume_soft_pause_params, sizeof(volume_soft_pause_params), FIRST_INSTANCE);
      }

      //setup volume control lib to ramp up after resume
      param_payload.module_id = AUDPROC_MODULE_ID_VOL_CTRL_INSTANCE_2;
      param_payload.param_id = AUDPROC_PARAM_ID_SOFT_PAUSE_SET_RAMP_ON_RESUME;
      param_payload.param_size = 0;
      param_payload.reserved = 0;

      topo_set_param(topo_ptr, &param_payload, sizeof(param_payload), FIRST_INSTANCE);

      param_payload.module_id = AUDPROC_MODULE_ID_VOL_CTRL;
      param_payload.param_id = AUDPROC_PARAM_ID_SOFT_PAUSE_SET_RAMP_ON_RESUME;
      param_payload.param_size = 0;
      param_payload.reserved = 0;

      topo_set_param(topo_ptr, &param_payload, sizeof(param_payload), FIRST_INSTANCE);
   }
   else
   {
      param_payload.module_id = AUDPROC_MODULE_ID_VOL_CTRL;
      param_payload.param_id = AUDPROC_PARAM_ID_SOFT_PAUSE_SET_RAMP_ON_RESUME;
      param_payload.param_size = 0;
      param_payload.reserved = 0;

      topo_set_param(topo_ptr, &param_payload, sizeof(param_payload), FIRST_INSTANCE);
   }
}

ADSPResult topo_send_capiv2_output_media_format (topo_struct *topo_ptr, const AudPPSvcOutputMediaTypeParams_t *pParams, uint32_t port_index)
{
#ifdef PP_TOPO_ENABLE_LOW_LEVEL_MSG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Propose Module Output Media Format", topo_ptr->obj_id);
#endif

   capi_v2_err_t err_code = CAPI_V2_EOK;

   intf_extn_param_id_chg_mf_propose_op_mf_t proposed_media_format;
   proposed_media_format.default_bits_per_sample = pParams->useNativeBitsPerSample;
   proposed_media_format.bits_per_sample = pParams->outputBitsPerSample;
   proposed_media_format.default_sampling_rate = pParams->keepInputSamplingRate;
   proposed_media_format.sampling_rate = pParams->outputSamplingRate;
   proposed_media_format.default_channel_info = pParams->useNativeNumChannels;
   if(!pParams->useNativeNumChannels)
   {
      if(pParams->outputNumChannels == 0 || pParams->outputNumChannels > SIZE_OF_ARRAY(pParams->outChannelMap))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Invalid number of channels %lu", pParams->outputNumChannels);
         return ADSP_EBADPARAM;
      }
      proposed_media_format.num_channels = pParams->outputNumChannels;
      for(uint32_t i = 0; i < pParams->outputNumChannels; i++)
      {
         proposed_media_format.channel_map[i] = (uint16_t)pParams->outChannelMap[i];
      }
   }

   capi_v2_buf_t proposed_media_format_payload = {
         reinterpret_cast<int8_t*>(&proposed_media_format),
         sizeof(proposed_media_format),
         sizeof(proposed_media_format)
   };
   capi_v2_port_info_t port_info = {
         TRUE,
         FALSE,
         port_index
   };

   for(uint32_t i = 0; i < topo_ptr->graph.num_modules; i++)
   {
      if(topo_ptr->module[i].changes_media_format)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Proposing output media format to "
                 "Module-Instance with ID 0x%lX-0x%lX.",
                 topo_ptr->obj_id, topo_ptr->module[i].module_id, topo_ptr->module[i].instance_id);
         //reset the actual data length for every module
         proposed_media_format_payload.actual_data_len = sizeof(proposed_media_format);
         capi_v2_t *module_ptr = topo_ptr->module[i].module_ptr;

         err_code = module_ptr->vtbl_ptr->set_param(module_ptr, INTF_EXTN_PARAM_ID_CHG_MF_PROPOSE_OP_MF, &port_info, &proposed_media_format_payload);
         if (CAPI_V2_FAILED(err_code))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Module-Instance with ID 0x%lX-0x%lX "
                    "failed to set param for propose output media format.",
                    topo_ptr->obj_id, topo_ptr->module[i].module_id, topo_ptr->module[i].instance_id);
         }
      }
   }
   return ADSP_EOK;
}

void topo_gen_out_media_fmt(topo_struct *topo_ptr)
{
   uint32_t start_index = TOPO_INVALID_MODULE_INDEX;

   for(uint32_t i = 0; i < topo_ptr->graph.num_modules; i++)
   {
      start_index = topo_module_pending_reconfig(topo_ptr, i) ? i : start_index;
      if(start_index != TOPO_INVALID_MODULE_INDEX)
      {
          // Apart from input media format routine, all other invocations of this routine need only TOPO_PRIMARY_STRM as port_id
         topo_handle_pending_events(topo_ptr, start_index,TOPO_PRIMARY_STRM);
         break;
      }
   }
}

static ADSPResult topo_module_allocate_ports(module_info *module_ptr)
{
   // Initialize everything to NULL
   module_ptr->input_ports_ptr = NULL;
   module_ptr->output_ports_ptr = NULL;

   // Calculate memory requirement
   uint32_t inport_mem_size = align_to_8_byte(sizeof(module_input_port_t) * module_ptr->num_input_ports);
   uint32_t outport_mem_size = align_to_8_byte(sizeof(module_output_port_t) * module_ptr->num_output_ports);
   uint32_t total_mem = inport_mem_size + outport_mem_size;
   QURT_ELITE_ASSERT(total_mem > 0);

   // Allocate memory
   module_ptr->port_mem = qurt_elite_memory_malloc(total_mem, QURT_ELITE_HEAP_DEFAULT);
   if (NULL == module_ptr->port_mem)
   {
      return ADSP_ENOMEMORY;
   }

   // Divide up the memory
   uint8_t *mem_ptr = (uint8_t*)module_ptr->port_mem;
   if (0 == module_ptr->num_input_ports)
   {
      module_ptr->input_ports_ptr = NULL;
   }
   else
   {
      module_ptr->input_ports_ptr = reinterpret_cast<module_input_port_t*>(mem_ptr);
      mem_ptr += inport_mem_size;
   }

   if (0 == module_ptr->num_output_ports)
   {
      module_ptr->output_ports_ptr = NULL;
   }
   else
   {
      module_ptr->output_ports_ptr = reinterpret_cast<module_output_port_t*>(mem_ptr);
      mem_ptr += outport_mem_size;
   }

   // Initialize all the variables
   for (uint32_t i = 0; i < module_ptr->num_input_ports; i++)
   {
      topo_module_input_port_initialize(&module_ptr->input_ports_ptr[i]);
   }

   for (uint32_t i = 0; i < module_ptr->num_output_ports; i++)
   {
      topo_module_output_port_initialize(&module_ptr->output_ports_ptr[i]);
   }

   return ADSP_EOK;
}

static void topo_module_free_ports(module_info *module_ptr, topo_buf_manager *buf_mgr_ptr)
{
   if (NULL != module_ptr->output_ports_ptr)
   {
      for (uint32_t i = 0; i < module_ptr->num_output_ports; i++)
      {
         if (NULL != module_ptr->output_ports_ptr->topo_buf.buf_ptrs[0])
         {
            topo_buf_manager_return_bufs(buf_mgr_ptr, &module_ptr->output_ports_ptr->topo_buf);
         }
      }
   }
   if (NULL != module_ptr->port_mem)
   {
      qurt_elite_memory_free(module_ptr->port_mem);
   }
}

static void topo_graph_init(topo_graph_t *topo_graph_ptr)
{
   topo_graph_ptr->num_modules = 0;
   topo_graph_ptr->connections_array = NULL;
   topo_graph_ptr->nodes = NULL;
}

static ADSPResult topo_graph_set_graph(topo_graph_t *topo_graph_ptr, const topo_def_t *topo_def_ptr)
{
   topo_graph_free_graph(topo_graph_ptr);

   if (topo_def_ptr->num_modules > 0)
   {
      topo_graph_ptr->num_modules = topo_def_ptr->num_modules;

      const uint8_t INPUT_PORTS_PER_MODULE  = 1;
      const uint8_t OUTPUT_PORTS_PER_MODULE = 1;

      uint32_t connections_array_size = topo_def_ptr->num_modules * (INPUT_PORTS_PER_MODULE + OUTPUT_PORTS_PER_MODULE);

      topo_graph_ptr->connections_array = (topo_graph_t::topo_graph_port_t*)qurt_elite_memory_malloc(connections_array_size * sizeof(*(topo_graph_ptr->connections_array)), QURT_ELITE_HEAP_DEFAULT);
      if (NULL == topo_graph_ptr->connections_array)
      {
         topo_graph_ptr->num_modules = 0;
         return ADSP_ENOMEMORY;
      }

      topo_graph_ptr->nodes = (topo_graph_t::topo_graph_node_t*)qurt_elite_memory_malloc(topo_def_ptr->num_modules * sizeof(*(topo_graph_ptr->nodes)), QURT_ELITE_HEAP_DEFAULT);
      if (NULL == topo_graph_ptr->nodes)
      {
         qurt_elite_memory_free(topo_graph_ptr->connections_array);
         topo_graph_ptr->connections_array = NULL;
         topo_graph_ptr->num_modules = 0;
         return ADSP_ENOMEMORY;
      }

      {
         uint32_t current_connections_index = 0;
         for (uint32_t i = 0; i < topo_def_ptr->num_modules; i++)
         {
            topo_graph_t::topo_graph_node_t *n = &topo_graph_ptr->nodes[i];

            n->module_id = topo_def_ptr->module_id_list[i].module_id;
            n->instance_id = 0;
            n->num_inputs = INPUT_PORTS_PER_MODULE;
            n->inputs_array = &topo_graph_ptr->connections_array[current_connections_index];
            current_connections_index += n->num_inputs;
            n->inputs_array[0].module_index = (i > 0) ? (i - 1) : GRAPH_MAIN_INPUT_PORT_INDEX;
            n->inputs_array[0].port_index = 0;

            n->num_outputs = OUTPUT_PORTS_PER_MODULE;
            n->outputs_array = &topo_graph_ptr->connections_array[current_connections_index];
            current_connections_index += n->num_outputs;
            n->outputs_array[0].module_index = ((i + 1) < topo_def_ptr->num_modules) ? (i + 1) : GRAPH_MAIN_OUTPUT_PORT_INDEX;
            n->outputs_array[0].port_index = 0;
         }
      }
   }

   return ADSP_EOK;
}

ADSPResult topo_get_module_list(const topo_struct *topo_obj, uint32_t* topo_module_list_ptr, uint32_t payload_size, uint32_t secOpCode)
{
   const topo_graph_t *topo_graph_ptr = &(topo_obj->graph);

   uint32_t num_modules, per_instance_payload_size, req_payload_size,start_module_index=0;

   if((secOpCode != ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES) && (secOpCode != ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES_V2))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Un-Supported version for Get module List..", topo_obj->obj_id);
       return ADSP_EUNSUPPORTED;
   }

   start_module_index = (topo_obj->ec_extn_info != NULL) ? topo_obj->ec_extn_info->num_far_modules : start_module_index;

   num_modules = (topo_graph_num_modules(topo_graph_ptr) - start_module_index );
   per_instance_payload_size = ((secOpCode == ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES)?
           sizeof(uint32_t): sizeof(audproc_topology_module_instance_info_t));
   req_payload_size = sizeof(audproc_topology_info_t) + num_modules * per_instance_payload_size;

   if(payload_size < req_payload_size)
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Insufficient size error in topo_get_module_list.", topo_obj->obj_id);
       return ADSP_ENOMEMORY;
   }
   else
   {
      memset(topo_module_list_ptr, 0, req_payload_size);

      if(topo_graph_ptr->nodes != NULL)
      {
         *topo_module_list_ptr = num_modules;//topo_graph_num_modules(topo_graph_ptr);
         topo_module_list_ptr++;
         if(secOpCode == ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES)
         {
             for (uint32_t i = start_module_index; i < topo_graph_num_modules(topo_graph_ptr); i++)
             {
                 const topo_graph_t::topo_graph_node_t *n = &topo_graph_ptr->nodes[i];
                 *topo_module_list_ptr =  n->module_id;
                 topo_module_list_ptr++;
             }
         }
         else
         {
             audproc_topology_module_instance_info_t *module_info_ptr = (audproc_topology_module_instance_info_t*)topo_module_list_ptr;
             for (uint32_t i = start_module_index; i < topo_graph_num_modules(topo_graph_ptr); i++)
             {
                 const topo_graph_t::topo_graph_node_t *n = &topo_graph_ptr->nodes[i];
                 module_info_ptr->module_id =  n->module_id;
                 module_info_ptr->instance_id = (uint16_t)n->instance_id;
                 module_info_ptr->reserved = 0;
                 module_info_ptr++;
             }
         }
      }
   }

   return ADSP_EOK;
}

static ADSPResult common_topo_graph_set_graph(topo_graph_t *topo_graph_ptr, topo_def_t *topo_def_ptr)
{
    topo_graph_free_graph(topo_graph_ptr);

    if (topo_def_ptr->num_modules > 0)
    {
        topo_graph_ptr->num_modules = topo_def_ptr->num_modules;

        const uint8_t INPUT_PORTS_PER_MODULE = 1;
        const uint8_t OUTPUT_PORTS_PER_MODULE = 1;

        uint32_t connections_array_size = topo_def_ptr->num_modules * (INPUT_PORTS_PER_MODULE + OUTPUT_PORTS_PER_MODULE);

        topo_graph_ptr->connections_array = (topo_graph_t::topo_graph_port_t*)qurt_elite_memory_malloc(connections_array_size * sizeof(*(topo_graph_ptr->connections_array)), QURT_ELITE_HEAP_DEFAULT);
        if (NULL == topo_graph_ptr->connections_array)
        {
            topo_graph_ptr->num_modules = 0;
            return ADSP_ENOMEMORY;
        }

        topo_graph_ptr->nodes = (topo_graph_t::topo_graph_node_t*)qurt_elite_memory_malloc(topo_def_ptr->num_modules * sizeof(*(topo_graph_ptr->nodes)), QURT_ELITE_HEAP_DEFAULT);
        if (NULL == topo_graph_ptr->nodes)
        {
            qurt_elite_memory_free(topo_graph_ptr->connections_array);
            topo_graph_ptr->connections_array = NULL;
            topo_graph_ptr->num_modules = 0;
            return ADSP_ENOMEMORY;
        }

        {
            uint32_t current_connections_index = 0;
            for (uint32_t i = 0; i < topo_def_ptr->num_modules; i++)
            {
                topo_graph_t::topo_graph_node_t *n = &topo_graph_ptr->nodes[i];

                n->module_id = topo_def_ptr->module_id_list[i].module_id;
                n->instance_id = topo_def_ptr->module_id_list[i].instance_id;
                n->num_inputs = INPUT_PORTS_PER_MODULE;
                n->inputs_array = &topo_graph_ptr->connections_array[current_connections_index];
                current_connections_index += n->num_inputs;
                n->inputs_array[0].module_index = (i > 0) ? (i - 1) : GRAPH_MAIN_INPUT_PORT_INDEX;
                n->inputs_array[0].port_index = 0;

                n->num_outputs = OUTPUT_PORTS_PER_MODULE;
                n->outputs_array = &topo_graph_ptr->connections_array[current_connections_index];
                current_connections_index += n->num_outputs;
                n->outputs_array[0].module_index = ((i + 1) < topo_def_ptr->num_modules) ? (i + 1) : GRAPH_MAIN_OUTPUT_PORT_INDEX;
                n->outputs_array[0].port_index = 0;
            }
        }
    }
    return ADSP_EOK;
}

static ADSPResult common_topo_graph_set_ec_graph(topo_graph_t *topo_graph_ptr, topo_def_t *topo_def_ptr,topo_ec_fwk_extn_t *ec_fwk_info)
{
    QURT_ELITE_ASSERT(ec_fwk_info != NULL);
    topo_graph_free_graph(topo_graph_ptr);

    if (topo_def_ptr->num_modules > 0)
    {
        topo_graph_ptr->num_modules = topo_def_ptr->num_modules;

        const uint8_t INPUT_PORTS_PER_MODULE = 1;
        const uint8_t INPUT_PORTS_PER_EC_MODULE = 2;
        const uint8_t OUTPUT_PORTS_PER_MODULE = 1;

        uint32_t connections_array_size = topo_def_ptr->num_modules * (INPUT_PORTS_PER_MODULE  + OUTPUT_PORTS_PER_MODULE);
        // Extra input port for EC module in the topo
        connections_array_size += (INPUT_PORTS_PER_EC_MODULE  - INPUT_PORTS_PER_MODULE );

        topo_graph_ptr->connections_array = (topo_graph_t::topo_graph_port_t*)qurt_elite_memory_malloc(connections_array_size * sizeof(*(topo_graph_ptr->connections_array)), QURT_ELITE_HEAP_DEFAULT);
        if (NULL == topo_graph_ptr->connections_array)
        {
            topo_graph_ptr->num_modules = 0;
            return ADSP_ENOMEMORY;
        }

        topo_graph_ptr->nodes = (topo_graph_t::topo_graph_node_t*)qurt_elite_memory_malloc(topo_def_ptr->num_modules * sizeof(*(topo_graph_ptr->nodes)), QURT_ELITE_HEAP_DEFAULT);
        if (NULL == topo_graph_ptr->nodes)
        {
            qurt_elite_memory_free(topo_graph_ptr->connections_array);
            topo_graph_ptr->connections_array = NULL;
            topo_graph_ptr->num_modules = 0;
            return ADSP_ENOMEMORY;
        }
        uint32_t current_connections_index = 0;
        for (uint32_t i = 0; i < topo_def_ptr->num_modules; i++)
        {
            topo_graph_t::topo_graph_node_t *n = &topo_graph_ptr->nodes[i];

            n->module_id = topo_def_ptr->module_id_list[i].module_id;
            n->instance_id = topo_def_ptr->module_id_list[i].instance_id;
            n->num_inputs = (i == ec_fwk_info->module_index) ? INPUT_PORTS_PER_EC_MODULE  : INPUT_PORTS_PER_MODULE ;
            n->inputs_array = &topo_graph_ptr->connections_array[current_connections_index];
            current_connections_index += n->num_inputs;
            /*
             *             _____
             *    -->|         |-->
             *          |_____ |
             *
             */
            for(uint32_t port_id=0;port_id < n->num_inputs; port_id++)
            {
                if(0 == ec_fwk_info->num_far_modules && port_id) // EC module far port - actual check should be port_id == TOPO_FAR_STRM
                    n->inputs_array[port_id].module_index =  GRAPH_MAIN_INPUT_PORT_INDEX+TOPO_FAR_STRM;
                else if(i == ec_fwk_info->num_far_modules) // First module in near chain or single EC module
                {
                    n->inputs_array[port_id].module_index = GRAPH_MAIN_INPUT_PORT_INDEX;
                    if(i && port_id) n->inputs_array[port_id].module_index = i-1;
                }
                else
                {
                    if(port_id) // EC module far port when far topo has modules
                        n->inputs_array[port_id].module_index =  (ec_fwk_info->num_far_modules - 1);
                    else // First module in far port or other regular modules in far or near port
                        n->inputs_array[port_id].module_index =  (i>0) ? (i-1) : (GRAPH_MAIN_INPUT_PORT_INDEX+TOPO_FAR_STRM);
                }
                n->inputs_array[port_id].port_index = 0; // We dont have modules with multiple outputs as of now, hence port index of input ports is always '0'
            }
            n->num_outputs = OUTPUT_PORTS_PER_MODULE;
            n->outputs_array = &topo_graph_ptr->connections_array[current_connections_index];
            current_connections_index += n->num_outputs;
            n->outputs_array[0].module_index = ((i + 1) < topo_def_ptr->num_modules) ? (i + 1) : GRAPH_MAIN_OUTPUT_PORT_INDEX;
            n->outputs_array[0].port_index = 0;
            if( (i+1) == ec_fwk_info->num_far_modules )
            {
                n->outputs_array[0].module_index = ec_fwk_info->module_index;
                n->outputs_array[0].port_index = TOPO_FAR_STRM; // Port index at output signifies the input port index of connected module
            }

        }
    }
    return ADSP_EOK;
}

static void topo_graph_free_graph(topo_graph_t *topo_graph_ptr)
{
   if (NULL != topo_graph_ptr->connections_array)
   {
      qurt_elite_memory_free(topo_graph_ptr->connections_array);
   }
   if (NULL != topo_graph_ptr->nodes)
   {
      qurt_elite_memory_free(topo_graph_ptr->nodes);
   }
   topo_graph_ptr->num_modules = 0;
}

static void topo_graph_deinit(topo_graph_t *topo_graph_ptr)
{
   topo_graph_free_graph(topo_graph_ptr);
}

static topo_port_index_t topo_graph_get_prev_module(topo_graph_t *topo_graph_ptr, topo_port_index_t current_index)
{
   QURT_ELITE_ASSERT(topo_port_index_t::INPUT_PORT == current_index.type);
   QURT_ELITE_ASSERT(current_index.module_index < topo_graph_num_modules(topo_graph_ptr));
   QURT_ELITE_ASSERT(current_index.port_index < topo_graph_ptr->nodes[current_index.module_index].num_inputs);

   topo_port_index_t prev_index;
   prev_index.module_index = topo_graph_ptr->nodes[current_index.module_index].inputs_array[current_index.port_index].module_index;
   if (GRAPH_MAIN_INPUT_PORT_INDEX == prev_index.module_index)
   {
      prev_index.module_index = TOPO_INVALID_MODULE_INDEX;
   }
   if ((GRAPH_MAIN_INPUT_PORT_INDEX + TOPO_FAR_STRM ) == prev_index.module_index)
   {
      prev_index.module_index = TOPO_INVALID_MODULE_INDEX_1;
   }
   prev_index.port_index = topo_graph_ptr->nodes[current_index.module_index].inputs_array[current_index.port_index].port_index;
   prev_index.type = topo_port_index_t::OUTPUT_PORT;

   return prev_index;
}
static topo_port_index_t topo_graph_get_next_module(topo_graph_t *topo_graph_ptr, topo_port_index_t current_index)
{
   QURT_ELITE_ASSERT(topo_port_index_t::OUTPUT_PORT == current_index.type);
   QURT_ELITE_ASSERT(current_index.module_index < topo_graph_num_modules(topo_graph_ptr));
   QURT_ELITE_ASSERT(current_index.port_index < topo_graph_ptr->nodes[current_index.module_index].num_outputs);

   topo_port_index_t next_index;
   next_index.module_index = topo_graph_ptr->nodes[current_index.module_index].outputs_array[current_index.port_index].module_index;
   if (GRAPH_MAIN_OUTPUT_PORT_INDEX == next_index.module_index)
   {
      next_index.module_index = TOPO_INVALID_MODULE_INDEX;
   }

   next_index.port_index = topo_graph_ptr->nodes[current_index.module_index].outputs_array[current_index.port_index].port_index;
   next_index.type = topo_port_index_t::INPUT_PORT;

   return next_index;
}

static topo_port_index_t topo_graph_get_main_input(topo_graph_t *topo_graph_ptr,uint32_t stream_index)
{
   topo_port_index_t main_input;
   main_input.module_index = TOPO_INVALID_MODULE_INDEX;
   main_input.port_index = 0;
   main_input.type = topo_port_index_t::INPUT_PORT;

   for (uint32_t i = 0; i < topo_graph_num_modules(topo_graph_ptr); i++)
   {
      for (uint32_t j = 0; j < topo_graph_ptr->nodes[i].num_inputs; j++)
      {
         if ((GRAPH_MAIN_INPUT_PORT_INDEX+stream_index) == topo_graph_ptr->nodes[i].inputs_array[j].module_index)
         {
            main_input.module_index = i;
            main_input.port_index = j;
            return main_input;
         }
      }
   }
   QURT_ELITE_ASSERT(0 == topo_graph_num_modules(topo_graph_ptr));

   return main_input;
}

static topo_port_index_t topo_graph_get_main_output(topo_graph_t *topo_graph_ptr)
{
   topo_port_index_t main_output;
   main_output.module_index = TOPO_INVALID_MODULE_INDEX;
   main_output.port_index = 0;
   main_output.type = topo_port_index_t::OUTPUT_PORT;

   for (uint32_t i = 0; i < topo_graph_num_modules(topo_graph_ptr); i++)
   {
      for (uint32_t j = 0; j < topo_graph_ptr->nodes[i].num_outputs; j++)
      {
         if (GRAPH_MAIN_OUTPUT_PORT_INDEX == topo_graph_ptr->nodes[i].outputs_array[j].module_index)
         {
            main_output.module_index = i;
            main_output.port_index = j;
            return main_output;
         }
      }
   }

   QURT_ELITE_ASSERT(0 == topo_graph_num_modules(topo_graph_ptr));

   return main_output;
}

static uint32_t topo_graph_num_modules(const topo_graph_t *topo_graph_ptr)
{
   return topo_graph_ptr->num_modules;
   }

static void topo_graph_print_linear_topo(const topo_graph_t *topo_graph_ptr, uint16_t obj_id,uint32_t start_module_index)
{
   uint32_t num_module_instances = topo_graph_num_modules(topo_graph_ptr);
   uint32_t module_id[3], instance_id[3];
   uint32_t array_size = sizeof(module_id)/sizeof(uint32_t);
   uint32_t j = array_size;

   // Main topology is starting from start_module_index, all prior modules are internally created by service and hence not printed
   num_module_instances -= start_module_index;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Graph: The topology is as follows, "
           "Number of module-instances: %lu", obj_id, num_module_instances);

   if(num_module_instances >= array_size)
   {
      for (j = array_size; j < num_module_instances; j+=array_size)
      {
         for (uint32_t i = 0; i < array_size; i++)
         {
            module_id[i] = topo_graph_ptr->nodes[(j - array_size) + (i+start_module_index)].module_id;
            instance_id[i] = topo_graph_ptr->nodes[(j - array_size) + (i+start_module_index)].instance_id;
         }

         MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology module-instance ids "
               "0x%lx-0x%lx --> "
               "0x%lx-0x%lx --> "
               "0x%lx-0x%lx --> ",
               obj_id,
               module_id[0], instance_id[0],
               module_id[1], instance_id[1],
               module_id[2], instance_id[2]);
      }
   }

   uint32_t num_modules_remaining = num_module_instances % array_size;

   if (num_modules_remaining == 0)
      return;

   for (uint32_t i = 0; i < num_modules_remaining; i++)
   {
      module_id[i] = topo_graph_ptr->nodes[(j - array_size) + (i+start_module_index)].module_id;
      instance_id[i] = topo_graph_ptr->nodes[(j - array_size) + (i+start_module_index)].instance_id;
   }
   for (uint32_t i = num_modules_remaining; i < array_size; i++)
   {
       module_id[i] = 0;
       instance_id[i] = 0;
   }
   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Topology module-instance ids "
         "0x%lx-0x%lx --> "
         "0x%lx-0x%lx --> "
         "0x%lx-0x%lx --> ",
         obj_id,
         module_id[0], instance_id[0],
         module_id[1], instance_id[1],
         module_id[2], instance_id[2]);

}
#ifdef TST_TOPO_LAYER
static void topo_graph_print_graph(const topo_graph_t *topo_graph_ptr, uint16_t obj_id)
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Graph:The topology graph is as follows:", obj_id);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Graph:Number of modules: %lu", obj_id, topo_graph_num_modules(topo_graph_ptr));

   for (uint32_t i = 0; i < topo_graph_num_modules(topo_graph_ptr); i++)
   {
      MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Graph: index %lu, Module-Instance with ID 0x%lX-0x%lX: "
              "num_inputs %hhu num_outputs %hhu.",obj_id, i,
              topo_graph_ptr->nodes[i].module_id, topo_graph_ptr->nodes[i].instance_id,
              topo_graph_ptr->nodes[i].num_inputs, topo_graph_ptr->nodes[i].num_outputs);
      for (uint32_t j = 0; j < topo_graph_ptr->nodes[i].num_inputs; j++)
      {
         uint8_t connected_module_index = topo_graph_ptr->nodes[i].inputs_array[j].module_index;

         if (GRAPH_MAIN_INPUT_PORT_INDEX == connected_module_index)
         {
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Graph: 0x%lx-0x%lx port %lu <-- main input port",
                    obj_id, topo_graph_ptr->nodes[i].module_id,topo_graph_ptr->nodes[i].instance_id, j);
         }
         else
         {
            MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Graph: 0x%lx-0x%lx port %lu <--  0x%lx-0x%lx port %hhu ",
                    obj_id, topo_graph_ptr->nodes[i].module_id, topo_graph_ptr->nodes[i].instance_id, j,
                    topo_graph_ptr->nodes[connected_module_index].module_id,
                    topo_graph_ptr->nodes[connected_module_index].instance_id,
                    topo_graph_ptr->nodes[i].inputs_array[j].port_index);
         }
   }
      for (uint32_t j = 0; j < topo_graph_ptr->nodes[i].num_outputs; j++)
      {
         uint8_t connected_module_index = topo_graph_ptr->nodes[i].outputs_array[j].module_index;

         if (GRAPH_MAIN_OUTPUT_PORT_INDEX == connected_module_index)
   {
             MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Graph: 0x%lx-0x%lx port %lu --> main output port",
                     obj_id, topo_graph_ptr->nodes[i].module_id,topo_graph_ptr->nodes[i].instance_id, j);
   }
   else
   {
            MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Graph: 0x%lx-0x%lx port %lu --> 0x%lx-0x%lx port %hhu ",
                    obj_id, topo_graph_ptr->nodes[i].module_id, topo_graph_ptr->nodes[i].instance_id, j,
                    topo_graph_ptr->nodes[connected_module_index].module_id,
                    topo_graph_ptr->nodes[connected_module_index].instance_id,
                    topo_graph_ptr->nodes[i].outputs_array[j].port_index);
         }
      }
   }
}
#endif
static capi_v2_port_num_info_t topo_graph_get_port_num_info(const topo_graph_t *topo_graph_ptr, uint32_t module_index)
{
   QURT_ELITE_ASSERT(module_index < topo_graph_num_modules(topo_graph_ptr));
   capi_v2_port_num_info_t port_num_info;
   port_num_info.num_input_ports = topo_graph_ptr->nodes[module_index].num_inputs;
   port_num_info.num_output_ports = topo_graph_ptr->nodes[module_index].num_outputs;
   return port_num_info;
}

static void topo_buffer_initialize(topo_buf_t *buf_ptr)
         {
   memset(buf_ptr, 0, sizeof(*buf_ptr));
   buf_ptr->bytes_to_samples_factor = 1;
}

static bool_t topo_module_can_process(topo_struct *topo_ptr, uint32_t module_index, uint32_t *required_in_port_index)
{
    if (!topo_ptr->module[module_index].is_in_chain)
    {
        return FALSE;
    }

    module_info *m = &topo_ptr->module[module_index];
    for (uint32_t i = 0; i < m->num_input_ports; i++)
    {
        //const topo_port_index_t current_in_port = { module_index, i, topo_port_index_t::INPUT_PORT };
        if(!topo_buffer_is_req_samples_present(topo_ptr, module_index, i))
        {
#ifdef DBG_TOPO_HANDLER
                MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_module_can_process: Module-Instance with id 0x%lX-0x%lX. does not have sufficient data on port %lu, num input ports %lu",
                topo_ptr->obj_id, m->module_id, m->instance_id, i, m->num_input_ports);
#endif		
            // If primary port doesnt have data, can't process
            if(topo_is_primary_in_port(i))
            {
                *required_in_port_index = i;
                return FALSE;
            }
            // If Non-primary port doesn't have data when port is active, can't process : otherwise, can call process with zero samples
            else if(topo_is_port_or_stream_active(topo_ptr,i))
            {
                *required_in_port_index = i;
                return FALSE;
            }
        }
#ifdef DBG_TOPO_HANDLER
                MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_module_can_process: Module-Instance with id 0x%lX-0x%lX. has sufficient data on port %lu, num input ports %lu",
                topo_ptr->obj_id, m->module_id, m->instance_id, i, m->num_input_ports);
#endif
        m->input_ports_ptr[i].is_data_available = TRUE;
    }

    // We only need to check input ports. Since we go from the end of the chain to the beginning, the output ports will always be empty.
    for (uint32_t i = 0; i < m->num_output_ports; i++)
    {
        const topo_port_index_t current_out_port = { module_index, i, topo_port_index_t::OUTPUT_PORT };

        QURT_ELITE_ASSERT(!topo_buffer_is_buf_full(topo_module_get_output_buffer(topo_ptr, &current_out_port)));
    }

    return TRUE;
}

static bool_t topo_module_is_propagate_for_disabled_module(const module_info *module_ptr)
{
   return topo_module_is_disablement_possible(module_ptr) && module_ptr->input_ports_ptr[0].media_event.event_is_pending;
      }

static bool_t topo_module_is_disablement_possible(const module_info *module_ptr)
{
   return ((1 == module_ptr->num_input_ports) && (1 == module_ptr->num_output_ports));
   }

static void topo_module_set_pending_input_media(module_info *module_ptr, uint16_t obj_id)
{
   const uint32_t TYPICAL_MAX_INPUT_PORTS = 2;
   capi_v2_prop_t props[TYPICAL_MAX_INPUT_PORTS];
   uint32_t current_prop_index = 0;

   capi_v2_err_t err_code = CAPI_V2_EOK;
   //bool_t media_format_changed = FALSE;

   for (uint32_t i = 0; i < module_ptr->num_input_ports; i++)
   {
      if (module_ptr->input_ports_ptr[i].media_event.event_is_pending)
      {
         MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO, "P%hX audproc_svc in_port %lu: Setting input media format for Module-Instance with ID 0x%lX-0x%lX",
                 obj_id, i, module_ptr->module_id, module_ptr->instance_id);
         //media_format_changed = TRUE;

         topo_media_fmt_t *new_format_ptr = &module_ptr->input_ports_ptr[i].media_event.event;

         props[current_prop_index].id = CAPI_V2_INPUT_MEDIA_FORMAT;
         props[current_prop_index].port_info.is_input_port = TRUE;
         props[current_prop_index].port_info.is_valid = TRUE;
         props[current_prop_index].port_info.port_index = i;
         props[current_prop_index].payload.actual_data_len = sizeof(*new_format_ptr);
         props[current_prop_index].payload.max_data_len = sizeof(*new_format_ptr);
         props[current_prop_index].payload.data_ptr = reinterpret_cast<int8_t*>(new_format_ptr);

         module_ptr->input_ports_ptr[i].media_event.event_is_pending = FALSE;
         module_ptr->input_ports_ptr[i].is_data_available = FALSE;

#ifdef DBG_TOPO_HANDLER
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Setting input media format for Module-Instance with ID 0x%lX, num channels %lu, port %lu.",
                 obj_id, module_ptr->module_id,new_format_ptr->f.num_channels, i);
#endif

         current_prop_index++;

         capi_v2_proplist_t proplist = { current_prop_index, props };

         CAPI_V2_SET_ERROR(err_code, module_ptr->module_ptr->vtbl_ptr->set_properties(module_ptr->module_ptr, &proplist));
         current_prop_index = 0;
         if (CAPI_V2_FAILED(err_code))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Module-Instance with ID 0x%lX-0x%lX failed to accept the current media type. Disabling it.",
                    obj_id, module_ptr->module_id, module_ptr->instance_id);
            module_ptr->disabled_till_next_media_event = TRUE;
            module_ptr->input_ports_ptr[i].media_event.event_is_pending = TRUE;
         }
         else
         {
            module_ptr->disabled_till_next_media_event = FALSE;
         }
      }
   }

}

static bool_t topo_check_update_module_enablement(topo_struct *topo_ptr, uint32_t module_index)
{
    bool_t module_state_changed = FALSE;
    module_info *module_ptr = &topo_ptr->module[module_index];
    // Handle enable status
    if(!module_ptr->is_in_chain)
    {
        if (module_ptr->is_enabled && (!module_ptr->disabled_till_next_media_event))
        {
            // Find the first module before this module in the graph that is enabled.
            topo_port_index_t prev_port_index = { TOPO_INVALID_MODULE_INDEX, 0, topo_port_index_t::OUTPUT_PORT };
            for(uint32_t port_id=0;port_id<module_ptr->num_input_ports;port_id++)
            {
                topo_port_index_t current_port_index = { module_index, port_id, topo_port_index_t::INPUT_PORT };
                for (prev_port_index = topo_graph_get_prev_module(&topo_ptr->graph, current_port_index);
                        (prev_port_index.module_index != TOPO_INVALID_MODULE_INDEX && prev_port_index.module_index != TOPO_INVALID_MODULE_INDEX_1);
                        prev_port_index = topo_graph_get_prev_module(&topo_ptr->graph, current_port_index))
                {
                    if (topo_ptr->module[prev_port_index.module_index].is_in_chain)
                    {
                        break;
                    }
                    else
                    {
                        current_port_index.module_index = prev_port_index.module_index;
                        current_port_index.port_index = prev_port_index.port_index;
                    }
                }
                // Restoring current port to current module input port to pass to add module routine
                topo_port_index_t in_port_index = { module_index, port_id, topo_port_index_t::INPUT_PORT };
                topo_add_to_module_list(topo_ptr, &in_port_index, &prev_port_index);
            }
            module_state_changed = TRUE;
        }
    }

    // Set up the output buffers if needed.
    if (module_ptr->is_in_chain)
    {
        for (uint32_t i = 0; i < module_ptr->num_output_ports; i++)
        {
            if(module_ptr->output_ports_ptr[i].media_event.event_is_pending)
            {
                const topo_port_index_t current_port_index = {module_index, i, topo_port_index_t::OUTPUT_PORT };
                topo_module_setup_output_buffers(topo_ptr, &current_port_index);
            }
        }
    }
    return module_state_changed;
}

static bool_t topo_check_update_module_process_state(topo_struct *topo_ptr, uint32_t module_index)
{
    bool_t module_state_changed = FALSE;
    module_info *module_ptr = &topo_ptr->module[module_index];
    // Handle enable/disable status
    if(module_ptr->is_in_chain)
    {
        if (!module_ptr->is_enabled || module_ptr->disabled_till_next_media_event) // TODO: Save this condition in a variable.
        {
            topo_remove_from_module_list(topo_ptr, module_index);
            topo_buf_manager_clear_all_bufs(&topo_ptr->buf_mgr);
            module_state_changed = TRUE;
        }
    }
    else
    {
        if (!(!module_ptr->is_enabled || module_ptr->disabled_till_next_media_event))
        {
            // Find the first module before this module in the graph that is enabled.
            topo_port_index_t prev_port_index = { TOPO_INVALID_MODULE_INDEX, 0, topo_port_index_t::OUTPUT_PORT };
            for(uint32_t port_id=0;port_id<module_ptr->num_input_ports;port_id++)
            {
                topo_port_index_t current_port_index = { module_index, port_id, topo_port_index_t::INPUT_PORT };
                for (prev_port_index = topo_graph_get_prev_module(&topo_ptr->graph, current_port_index);
                        (prev_port_index.module_index != TOPO_INVALID_MODULE_INDEX && prev_port_index.module_index != TOPO_INVALID_MODULE_INDEX_1);
                        prev_port_index = topo_graph_get_prev_module(&topo_ptr->graph, current_port_index))
                {
                    if (topo_ptr->module[prev_port_index.module_index].is_in_chain)
                    {
                        break;
                    }
                    else
                    {
                        current_port_index.module_index = prev_port_index.module_index;
                        current_port_index.port_index = prev_port_index.port_index;
                    }
                }
                topo_port_index_t in_port_index = { module_index, port_id, topo_port_index_t::INPUT_PORT };
                topo_add_to_module_list(topo_ptr, &in_port_index, &prev_port_index);
            }
            module_state_changed = TRUE;
        }
    }

    // Set up the output buffers if needed.
    if (module_ptr->is_in_chain)
    {
        for (uint32_t i = 0; i < module_ptr->num_output_ports; i++)
        {
            if(module_ptr->output_ports_ptr[i].media_event.event_is_pending)
            {
                const topo_port_index_t current_port_index = {module_index, i, topo_port_index_t::OUTPUT_PORT };
                topo_module_setup_output_buffers(topo_ptr, &current_port_index);
            }
        }
    }

    return module_state_changed;
}

static void topo_module_input_port_initialize(module_input_port_t *port_ptr)
{
   port_ptr->connected_port.module_index = TOPO_INVALID_MODULE_INDEX;
   port_ptr->connected_port.port_index = 0;
   port_ptr->connected_port.module_index = topo_port_index_t::OUTPUT_PORT;
   port_ptr->media_event.event_is_pending = FALSE;
   port_ptr->media_event.event = invalid_media_format;
   port_ptr->data_threshold = 0;
   port_ptr->is_data_available = TRUE;
}

static void topo_module_output_port_initialize(module_output_port_t *port_ptr)
{
   port_ptr->connected_port.module_index = TOPO_INVALID_MODULE_INDEX;
   port_ptr->connected_port.port_index = 0;
   port_ptr->connected_port.module_index = topo_port_index_t::INPUT_PORT;
   port_ptr->media_event.event_is_pending = FALSE;
   port_ptr->media_event.event = invalid_media_format;
   topo_buffer_initialize(&port_ptr->topo_buf);
   port_ptr->data_threshold = 0;
}

static module_input_port_t *topo_in_port_from_index(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr)
{
   QURT_ELITE_ASSERT(topo_port_index_t::INPUT_PORT == port_index_ptr->type);
   QURT_ELITE_ASSERT(topo_graph_num_modules(&topo_ptr->graph) > port_index_ptr->module_index);
   QURT_ELITE_ASSERT(topo_ptr->module[port_index_ptr->module_index].num_input_ports > port_index_ptr->port_index);

   return &topo_ptr->module[port_index_ptr->module_index].input_ports_ptr[port_index_ptr->port_index];
}

static module_output_port_t *topo_out_port_from_index(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr)
{
   QURT_ELITE_ASSERT(topo_port_index_t::OUTPUT_PORT == port_index_ptr->type);
   QURT_ELITE_ASSERT(topo_graph_num_modules(&topo_ptr->graph) > port_index_ptr->module_index);
   QURT_ELITE_ASSERT(topo_ptr->module[port_index_ptr->module_index].num_output_ports > port_index_ptr->port_index);

   return &topo_ptr->module[port_index_ptr->module_index].output_ports_ptr[port_index_ptr->port_index];
}

static topo_port_index_t topo_module_next_port(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr)
{
   QURT_ELITE_ASSERT(topo_port_index_t::OUTPUT_PORT == port_index_ptr->type);

   if (TOPO_INVALID_MODULE_INDEX == port_index_ptr->module_index)
   {
      return topo_ptr->first_port[TOPO_PRIMARY_STRM];
   }
   else if (TOPO_INVALID_MODULE_INDEX_1 == port_index_ptr->module_index)
   {
          return topo_ptr->first_port[TOPO_FAR_STRM];
   }
   else
      {
      QURT_ELITE_ASSERT(port_index_ptr->module_index < topo_graph_num_modules(&topo_ptr->graph));
      QURT_ELITE_ASSERT(port_index_ptr->port_index < topo_ptr->module[port_index_ptr->module_index].num_output_ports);
      return topo_ptr->module[port_index_ptr->module_index].output_ports_ptr[port_index_ptr->port_index].connected_port;
      }
   }

static topo_port_index_t topo_module_prev_port(topo_struct *topo_ptr, const topo_port_index_t *port_index_ptr)
{
   QURT_ELITE_ASSERT(topo_port_index_t::INPUT_PORT == port_index_ptr->type);

   if (TOPO_INVALID_MODULE_INDEX == port_index_ptr->module_index)
   {
      return topo_ptr->last_port;
}
   else
{
      QURT_ELITE_ASSERT(port_index_ptr->module_index < topo_graph_num_modules(&topo_ptr->graph));
      QURT_ELITE_ASSERT(port_index_ptr->port_index < topo_ptr->module[port_index_ptr->module_index].num_input_ports);
      return topo_ptr->module[port_index_ptr->module_index].input_ports_ptr[port_index_ptr->port_index].connected_port;
   }
}

static void topo_destroy_modules(uint32_t num_modules, module_info modules[], topo_buf_manager *buf_mgr_ptr)
   {
   for (uint32_t i = 0; i < num_modules; i++)
      {
      topo_module_destroy(&modules[i], buf_mgr_ptr);
      }
   }

static void topo_module_destroy(module_info *module_struct_ptr, topo_buf_manager *buf_mgr_ptr)
{
   capi_v2_t *module_ptr = module_struct_ptr->module_ptr;

   (void) module_ptr->vtbl_ptr->end(module_ptr);

   qurt_elite_memory_free(module_struct_ptr->module_ptr);

   module_struct_ptr->module_ptr = NULL;
   module_struct_ptr->module_id = 0;

   if(module_struct_ptr->fwk_extn_ptr != NULL)
       qurt_elite_memory_free(module_struct_ptr->fwk_extn_ptr);
   module_struct_ptr->fwk_extn_ptr = NULL;

   topo_module_free_ports(module_struct_ptr, buf_mgr_ptr);
}

static void topo_setup_connections(topo_struct *topo_ptr)
{
   // This function sets up all the connections from scratch. This should be done only at init time, before the media format is received.
   QURT_ELITE_ASSERT(!topo_ptr->data_format_received[TOPO_PRIMARY_STRM]);
   for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
   {
      module_info *m = &topo_ptr->module[i];

      for (uint32_t j = 0; j < m->num_input_ports; j++)
      {
         topo_port_index_t current_index = { i, j, topo_port_index_t::INPUT_PORT };
         m->input_ports_ptr[j].connected_port = topo_graph_get_prev_module(&topo_ptr->graph, current_index);
      }

      for (uint32_t j = 0; j < m->num_output_ports; j++)
      {
         topo_port_index_t current_index = { i, j, topo_port_index_t::OUTPUT_PORT };
         m->output_ports_ptr[j].connected_port = topo_graph_get_next_module(&topo_ptr->graph, current_index);
      }
      m->is_in_chain = TRUE;
      // Current active_in_streams is only assumed to be max input ports of a module, need to enhance for separate input ports connecting to separate modules.
      topo_ptr->active_in_streams = (topo_ptr->active_in_streams > m->num_input_ports) ? topo_ptr->active_in_streams : m->num_input_ports;
   }

   QURT_ELITE_ASSERT(topo_ptr->active_in_streams == topo_ptr->max_in_streams);

   topo_ptr->first_port[TOPO_PRIMARY_STRM] = topo_graph_get_main_input(&topo_ptr->graph,TOPO_PRIMARY_STRM);
   if(topo_ptr->ec_extn_info)
       topo_ptr->first_port[TOPO_FAR_STRM] = topo_graph_get_main_input(&topo_ptr->graph,TOPO_FAR_STRM);
   topo_ptr->last_port = topo_graph_get_main_output(&topo_ptr->graph);
}

#ifdef TST_TOPO_LAYER

static ADSPResult topo_graph_set_graph_dag(topo_graph_t *topo_graph_ptr, const topo_tst_graph_helper *topo_def_ptr)
{
   topo_graph_free_graph(topo_graph_ptr);

   if (topo_def_ptr->num_modules() > 0)
   {
      topo_graph_ptr->num_modules = topo_def_ptr->num_modules();

      uint32_t connections_array_size = topo_def_ptr->num_modules() * topo_def_ptr->get_total_ports();

      topo_graph_ptr->connections_array = (topo_graph_t::topo_graph_port_t*)qurt_elite_memory_malloc(connections_array_size * sizeof(*(topo_graph_ptr->connections_array)), QURT_ELITE_HEAP_DEFAULT);
      if (NULL == topo_graph_ptr->connections_array)
      {
         topo_graph_ptr->num_modules = 0;
         return ADSP_ENOMEMORY;
      }

      topo_graph_ptr->nodes = (topo_graph_t::topo_graph_node_t*)qurt_elite_memory_malloc(topo_def_ptr->num_modules() * sizeof(*(topo_graph_ptr->nodes)), QURT_ELITE_HEAP_DEFAULT);
      if (NULL == topo_graph_ptr->nodes)
      {
         qurt_elite_memory_free(topo_graph_ptr->connections_array);
         topo_graph_ptr->connections_array = NULL;
         topo_graph_ptr->num_modules = 0;
         return ADSP_ENOMEMORY;
      }

      {
         uint32_t current_connections_index = 0;
         for (uint32_t i = 0; i < topo_def_ptr->num_modules(); i++)
         {
            topo_graph_t::topo_graph_node_t *n = &topo_graph_ptr->nodes[i];

            n->module_id = topo_def_ptr->get_module_info(i).module_id;
            n->num_inputs = topo_def_ptr->get_module_info(i).inputs.size();
            n->inputs_array = &topo_graph_ptr->connections_array[current_connections_index];
            current_connections_index += n->num_inputs;
            for (uint32_t j = 0; j < n->num_inputs; j++)
            {
               n->inputs_array[j].module_index = topo_def_ptr->get_module_info(i).inputs.at(j).module_index;
               n->inputs_array[j].port_index = topo_def_ptr->get_module_info(i).inputs.at(j).port_index;
            }

            n->num_outputs = topo_def_ptr->get_module_info(i).outputs.size();
            n->outputs_array = &topo_graph_ptr->connections_array[current_connections_index];
            current_connections_index += n->num_outputs;
            for (uint32_t j = 0; j < n->num_outputs; j++)
            {
               n->outputs_array[j].module_index = topo_def_ptr->get_module_info(i).outputs.at(j).module_index;
               n->outputs_array[j].port_index = topo_def_ptr->get_module_info(i).outputs.at(j).port_index;
            }
         }
      }
   }

   return ADSP_EOK;
}

ADSPResult topo_init_dag(const topo_tst_graph_helper &def,
      const uint16_t instance_id,
      const uint32_t max_output_samples,
      topo_struct **topo_obj,
      AudPP_AudProcType pp_type,
      uint32_t *stack_size_ptr,
      topo_event_client_t *event_client_ptr,
      bool_t initialize_with_compressed_format,
        bool_t is_input_output_realtime)
{
   ADSPResult result = ADSP_EOK;
   topo_struct *topo_ptr = NULL;
   uint32_t num_modules = 0;
   uint32_t num_created_modules = 0;

   *topo_obj = NULL;

   if (NULL == stack_size_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Stack size pointer is NULL.", instance_id);
      result = ADSP_EBADPARAM;
      goto cleanup;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Initializing topology begin", instance_id);

   result = topo_create_obj(instance_id, &topo_ptr, max_output_samples, event_client_ptr,is_input_output_realtime);
   if (ADSP_FAILED(result))
   {
      goto cleanup;
   }

   result = topo_graph_set_graph_dag(&topo_ptr->graph, &def);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to create the graph representation.", topo_ptr->obj_id);
      goto cleanup;
   }

   topo_graph_print_graph(&topo_ptr->graph, instance_id);

   result = topo_allocate_stream_structs(topo_ptr);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to create the stream structures.", topo_ptr->obj_id);
      goto cleanup;
   }

   *stack_size_ptr = AUDPROC_TOPO_DEFAULT_STACK_SIZE;

   num_modules = topo_graph_num_modules(&topo_ptr->graph);

   if(0 != num_modules)
   {
      topo_ptr->module = (module_info*)qurt_elite_memory_malloc(sizeof(module_info) * num_modules, QURT_ELITE_HEAP_DEFAULT);
      if (NULL == topo_ptr->module)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to allocate modules array", topo_ptr->obj_id);
         topo_destroy(topo_ptr);
         return ADSP_ENOMEMORY;
      }

      topo_lock_module_list(topo_ptr);

      static const uint32_t MAX_HANDLES_IN_ONE_CALL = 16;
      uint32_t first_module_index = 0;

      while (first_module_index < num_modules)
      {
         adsp_amdb_module_handle_info_t handle_info[MAX_HANDLES_IN_ONE_CALL];
         uint32_t num_modules_filled = 0;

         while ((num_modules_filled < MAX_HANDLES_IN_ONE_CALL) && ((first_module_index + num_modules_filled) < num_modules))
         {
            handle_info[num_modules_filled].interface_type = CAPI_V2;
            handle_info[num_modules_filled].type = AMDB_MODULE_TYPE_GENERIC;
            handle_info[num_modules_filled].id1 = def.get_module_info(first_module_index + num_modules_filled).module_id;
            handle_info[num_modules_filled].id2 = 0;
            handle_info[num_modules_filled].h.capi_v2_handle = NULL;
            handle_info[num_modules_filled].result = ADSP_EFAILED;

            num_modules_filled++;
         }

         adsp_amdb_get_modules_request(handle_info, num_modules_filled, NULL, NULL);

         /* Create the modules */
         for (uint32_t j = 0; j < num_modules_filled; j++ )
         {
            audproc_module_info_t m_info;
            m_info.module_id = def.get_module_info(first_module_index + j).module_id;
            m_info.use_lpm = FALSE;
            memset(m_info.init_params, 0, sizeof(m_info.init_params));

            result = topo_init_single_module(topo_ptr, first_module_index + j, &m_info, &handle_info[j], pp_type, stack_size_ptr, initialize_with_compressed_format);
            if (ADSP_FAILED(result))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Module with ID 0x%lX failed in init.", topo_ptr->obj_id, def.get_module_info(first_module_index + j).module_id);
               break;
            }
            num_created_modules++;
         }

         // No need to keep the handles now since the modules are created.
         adsp_amdb_release_handles(handle_info, num_modules_filled);

         first_module_index += num_modules_filled;

         if (ADSP_FAILED(result))
         {
            goto cleanup;
         }
      }

      topo_set_delay_ptr_virtualizer(topo_ptr);
      topo_unlock_module_list(topo_ptr);
   }

   *topo_obj = topo_ptr;
   topo_update_chain_KPPS(topo_ptr);
   topo_update_chain_bandwidth(topo_ptr);
   topo_update_chain_delay(topo_ptr);
   topo_update_chain_headroom(topo_ptr);

   cleanup:

   if (ADSP_FAILED(result))
   {
      if (NULL != topo_ptr)
      {
         if (NULL != topo_ptr->module)
         {
            topo_destroy_modules(num_created_modules, topo_ptr->module, &topo_ptr->buf_mgr);
            qurt_elite_memory_free(topo_ptr->module);
            topo_ptr->module = NULL;
         }
         topo_destroy(topo_ptr);
         topo_ptr = NULL;
      }
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Initializing topology End", instance_id);

   return result;
}
#endif

static ADSPResult topo_allocate_stream_structs(topo_struct *topo_ptr)
{
   QURT_ELITE_ASSERT(NULL == topo_ptr->stream_struct_mem);
   QURT_ELITE_ASSERT(0 == topo_ptr->max_in_streams);
   QURT_ELITE_ASSERT(NULL == topo_ptr->in_streams);
   QURT_ELITE_ASSERT(NULL == topo_ptr->in_stream_ptrs);


   topo_ptr->max_in_streams = 0;
   topo_ptr->max_out_streams = 0;
   topo_ptr->active_in_streams = 0;
   uint32_t num_modules = topo_graph_num_modules(&topo_ptr->graph);
   for (uint32_t i = 0; i < num_modules; i++ )
   {
      capi_v2_port_num_info_t port_info = topo_graph_get_port_num_info(&topo_ptr->graph, i);
      topo_ptr->max_in_streams = (topo_ptr->max_in_streams < port_info.num_input_ports) ? port_info.num_input_ports : topo_ptr->max_in_streams;
      topo_ptr->max_out_streams = (topo_ptr->max_out_streams < port_info.num_output_ports) ? port_info.num_output_ports : topo_ptr->max_out_streams;
   }

   // Allocate memory for all the stream structures
   uint32_t num_ports = topo_ptr->max_in_streams + topo_ptr->max_out_streams;
   uint32_t stream_struct_size = align_to_8_byte(sizeof(capi_v2_stream_data_t) * num_ports);
   uint32_t stream_struct_ptrs_size = align_to_8_byte(sizeof(capi_v2_stream_data_t*) * num_ports);
   uint32_t bufs_size = align_to_8_byte(sizeof(capi_v2_buf_t) * num_ports * CAPI_V2_MAX_CHANNELS);
   uint32_t total_size = stream_struct_size + stream_struct_ptrs_size + bufs_size;
   if (0 == total_size)
      {
      return ADSP_EOK;
      }
   topo_ptr->stream_struct_mem = qurt_elite_memory_malloc(total_size, QURT_ELITE_HEAP_DEFAULT);
   if (NULL == topo_ptr->stream_struct_mem)
   {
      return ADSP_ENOMEMORY;
   }

   // Divide up the memory
   uint8_t *ptr = reinterpret_cast<uint8_t*>(topo_ptr->stream_struct_mem);
   topo_ptr->in_streams = reinterpret_cast<capi_v2_stream_data_t*>(ptr);
   topo_ptr->out_streams = &topo_ptr->in_streams[topo_ptr->max_in_streams];
   ptr += stream_struct_size;
   topo_ptr->in_stream_ptrs = reinterpret_cast<capi_v2_stream_data_t**>(ptr);
   topo_ptr->out_stream_ptrs = &topo_ptr->in_stream_ptrs[topo_ptr->max_in_streams];
   ptr += stream_struct_ptrs_size;
   for (uint32_t i = 0; i < topo_ptr->max_in_streams; i++)
   {
      topo_ptr->in_stream_ptrs[i] = &topo_ptr->in_streams[i];
      topo_ptr->in_streams[i] = init_stream_data;
      topo_ptr->in_streams[i].buf_ptr = reinterpret_cast<capi_v2_buf_t*>(ptr);
      for (uint32_t j = 0; j < CAPI_V2_MAX_CHANNELS; j++)
      {
         topo_ptr->in_streams[i].buf_ptr[j] = empty_buf;
      }
      ptr += sizeof(capi_v2_buf_t) * CAPI_V2_MAX_CHANNELS;
}
   for (uint32_t i = 0; i < topo_ptr->max_out_streams; i++)
{
      topo_ptr->out_stream_ptrs[i] = &topo_ptr->out_streams[i];
      topo_ptr->out_streams[i] = init_stream_data;
      topo_ptr->out_streams[i].buf_ptr = reinterpret_cast<capi_v2_buf_t*>(ptr);
      for (uint32_t j = 0; j < CAPI_V2_MAX_CHANNELS; j++)
      {
         topo_ptr->out_streams[i].buf_ptr[j] = empty_buf;
      }
      ptr += sizeof(capi_v2_buf_t) * CAPI_V2_MAX_CHANNELS;
   }

   return ADSP_EOK;
}

static void topo_deallocate_stream_structs(topo_struct *topo_ptr)
   {
   if (NULL != topo_ptr->stream_struct_mem)
      {
      qurt_elite_memory_free(topo_ptr->stream_struct_mem);
   }
   topo_ptr->stream_struct_mem = NULL;
   topo_ptr->max_in_streams = 0;
   topo_ptr->active_in_streams = 0;
   topo_ptr->in_streams = NULL;
   topo_ptr->in_stream_ptrs = NULL;
   topo_ptr->max_out_streams = 0;
   topo_ptr->out_streams = NULL;
   topo_ptr->out_stream_ptrs = NULL;
}

static void topo_set_media_event(topo_media_event_t *downstream_event_ptr, const topo_media_fmt_t *format_to_be_set)
{
   downstream_event_ptr->event_is_pending = !topo_is_equal_media_format(&downstream_event_ptr->event, format_to_be_set);
   if (downstream_event_ptr->event_is_pending)
   {
      downstream_event_ptr->event = *format_to_be_set;
   }
}


static bool_t is_sampling_rate_fractional(uint32_t sampling_rate)
{
    bool_t is_sample_rate_fract;

    if (0 == sampling_rate % 1000)
    {
        is_sample_rate_fract = FALSE;
    }
    else
    {
        is_sample_rate_fract = TRUE;
    }
    return is_sample_rate_fract;
}

static ADSPResult topo_internal_buf_create_internal_buffer(topo_struct *topo_ptr)
{
    topo_internal_buf_t *internal_buf_ptr = &topo_ptr->internal_buf;
    uint32_t        ouput_buffer_num_unit_frames = topo_ptr->ouput_buffer_num_unit_frames;
    AdspAudioMediaFormatInfo_t  output_format = topo_ptr->out_format;

   uint32_t bytes_per_sample = topo_get_bytes_per_sample(output_format.bitsPerSample);

    //internal buffersize = twice the output buffer size
    uint32_t unAfeFrameSizeInSamples = 0;
    elite_svc_get_frame_size(output_format.samplingRate,&unAfeFrameSizeInSamples);
    uint32_t new_internal_buf_size = INTERNAL_BUF_FACTOR * (ouput_buffer_num_unit_frames * unAfeFrameSizeInSamples * output_format.channels * bytes_per_sample);
    uint32_t internal_buf_size_in_samples = INTERNAL_BUF_FACTOR * (ouput_buffer_num_unit_frames * unAfeFrameSizeInSamples);

    if(NULL != internal_buf_ptr->pBuf)
    {
        if (new_internal_buf_size != internal_buf_ptr->buf_size)
        {
            topo_internal_buf_destroy_internal_buffer(topo_ptr);
        }
        else
        {
            return ADSP_EOK;
        }
    }

    int8_t* ptr = (int8_t *)qurt_elite_memory_malloc(new_internal_buf_size, QURT_ELITE_HEAP_DEFAULT);
    if (!ptr)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: Out of Memory while creating internal buffer!");
        return ADSP_ENOMEMORY;
    }
    memset(ptr, 0, new_internal_buf_size);

    internal_buf_ptr->pBuf= ptr;
    internal_buf_ptr->read_ptr = ptr;
    internal_buf_ptr->buf_size = new_internal_buf_size;
    internal_buf_ptr->size_in_samples = (internal_buf_size_in_samples/INTERNAL_BUF_FACTOR); // Per channel (at 1ms)
    internal_buf_ptr->offset_in_samples_write = 0;
    internal_buf_ptr->offset_in_samples_read = 0;

    internal_buf_ptr->write_ptr = ptr +  (internal_buf_ptr->size_in_samples * bytes_per_sample);
    internal_buf_ptr->end_ptr = internal_buf_ptr->write_ptr + (internal_buf_ptr->size_in_samples * bytes_per_sample);

    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: successful creation of internal buffer!");

    return ADSP_EOK;
}

static void topo_internal_buf_destroy_internal_buffer(topo_struct *topo_ptr)
{
    topo_internal_buf_t *internal_buf_ptr = &topo_ptr->internal_buf;

    if(NULL != internal_buf_ptr->pBuf)
    {
        qurt_elite_memory_free(internal_buf_ptr->pBuf);
        memset((void *)&topo_ptr->internal_buf, 0, sizeof(topo_ptr->internal_buf));
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: internal buffer destroyed!");
    }
}

static ADSPResult topo_internal_buf_copy_to_ext_output_buf(AudPP_BufInfo_t *ext_out_buf_ptr, topo_struct *topo_ptr, AdspAudioMediaFormatInfo_t *old_media_fmt_ptr)
{
    topo_internal_buf_t    *internal_buf_ptr = &topo_ptr->internal_buf;
    AdspAudioMediaFormatInfo_t *format_ptr = old_media_fmt_ptr;

    //TODO: check logic for interleaved data

    uint32_t channel_offset_in_bytes = 0;
    uint32_t bytes_to_copy_from_internal_buf_per_ch = 0;
    uint32_t empty_space_in_ext_buf_per_ch = 0;
    uint32_t bytes_per_ch_ext_inp_buf = 0;
    uint32_t num_bytes_transferred = 0;
    uint32_t internal_time_offset_in_samples = 0;
    uint32_t internal_num_bytes_transferred = 0;
    uint32_t bytes_to_copy_per_ch = 0;
    uint32_t free_space_to_transfer_per_ch = 0;
    int8_t *src_ptr_in_internal_buf = NULL;
    int8_t *dest_ptr_in_internal_buf = NULL;

    uint32_t bytes_per_sample = topo_get_bytes_per_sample(format_ptr->bitsPerSample);

    uint32_t buffer_fullness_in_samples = (internal_buf_ptr->end_ptr - internal_buf_ptr->read_ptr)/bytes_per_sample;
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: internal buffer buffer_fullness_in_samples = %d",buffer_fullness_in_samples);

    //step1: copy from internal buffer to external output buffer
    channel_offset_in_bytes = (internal_buf_ptr->size_in_samples * INTERNAL_BUF_FACTOR * bytes_per_sample);
    bytes_to_copy_from_internal_buf_per_ch = ((internal_buf_ptr->size_in_samples - internal_buf_ptr->offset_in_samples_read) * bytes_per_sample);
    empty_space_in_ext_buf_per_ch = ((ext_out_buf_ptr->sizeInSamples - ext_out_buf_ptr->offsetInSamples) * bytes_per_sample);
    bytes_per_ch_ext_inp_buf = ext_out_buf_ptr->sizeInSamples * bytes_per_sample;

    int8_t *src_ptr = internal_buf_ptr->read_ptr;
    int8_t *dest_ptr = ext_out_buf_ptr->pBuf + (ext_out_buf_ptr->offsetInSamples * bytes_per_sample);

    num_bytes_transferred = (empty_space_in_ext_buf_per_ch > bytes_to_copy_from_internal_buf_per_ch)? bytes_to_copy_from_internal_buf_per_ch:empty_space_in_ext_buf_per_ch;

    if (buffer_fullness_in_samples > internal_buf_ptr->size_in_samples)
    {
        for (uint8_t i = 0; i < format_ptr->channels; i++)
        {
            memscpy(dest_ptr, empty_space_in_ext_buf_per_ch, src_ptr, bytes_to_copy_from_internal_buf_per_ch);
            dest_ptr += bytes_per_ch_ext_inp_buf;
            src_ptr += channel_offset_in_bytes;
        }
        internal_buf_ptr->offset_in_samples_read += (num_bytes_transferred/bytes_per_sample);

        ext_out_buf_ptr->offsetInSamples += (num_bytes_transferred/bytes_per_sample);
        ext_out_buf_ptr->flags = internal_buf_ptr->flags;
        internal_time_offset_in_samples =((internal_buf_ptr->write_ptr - internal_buf_ptr->read_ptr)/bytes_per_sample);
        ext_out_buf_ptr->timestamp = internal_buf_ptr->timestamp - ((internal_time_offset_in_samples * 1000000)/format_ptr->samplingRate);

        //step2: copy newly generated samples to bottom of internal buffer
        free_space_to_transfer_per_ch = (internal_buf_ptr->offset_in_samples_read * bytes_per_sample);

        src_ptr_in_internal_buf = internal_buf_ptr->pBuf + num_bytes_transferred;
        dest_ptr_in_internal_buf = internal_buf_ptr->read_ptr;

        bytes_to_copy_per_ch = internal_buf_ptr->end_ptr - src_ptr_in_internal_buf;

        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: internal buffer first transfer = %d, %d",free_space_to_transfer_per_ch,bytes_to_copy_per_ch);

        for (uint8_t i = 0; i < format_ptr->channels; i++)
        {
            memsmove(dest_ptr_in_internal_buf, free_space_to_transfer_per_ch,src_ptr_in_internal_buf, bytes_to_copy_per_ch);
            dest_ptr_in_internal_buf += channel_offset_in_bytes;
            src_ptr_in_internal_buf += channel_offset_in_bytes;
        }

        internal_num_bytes_transferred = (free_space_to_transfer_per_ch > bytes_to_copy_per_ch)? bytes_to_copy_per_ch:free_space_to_transfer_per_ch;
        internal_buf_ptr->offset_in_samples_write = internal_num_bytes_transferred/bytes_per_sample;
        internal_buf_ptr->write_ptr = internal_buf_ptr->pBuf + internal_num_bytes_transferred;
    }
    else
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: internal buffer empty, skipping the external transfer");
        internal_buf_ptr->write_ptr = internal_buf_ptr->pBuf + (buffer_fullness_in_samples * bytes_per_sample);
    }

    internal_buf_ptr->offset_in_samples_read = 0;
    internal_buf_ptr->offset_in_samples_write = 0;
    internal_buf_ptr->end_ptr = internal_buf_ptr->pBuf + INTERNAL_BUF_FACTOR * (internal_buf_ptr->size_in_samples * bytes_per_sample);

    return ADSP_EOK;
}

static void topo_internal_buf_update_offsets(topo_internal_buf_t *internal_buf_ptr, topo_buf_t *out_buf_ptr, const AdspAudioMediaFormatInfo_t *format_ptr)
{
    internal_buf_ptr->flags = out_buf_ptr->flags;
    internal_buf_ptr->timestamp = out_buf_ptr->timestamp;

    internal_buf_ptr->offset_in_samples_write = out_buf_ptr->valid_data_end / topo_get_bytes_per_sample(format_ptr->bitsPerSample);
    internal_buf_ptr->end_ptr = internal_buf_ptr->write_ptr + out_buf_ptr->valid_data_end;
    if (data_is_interleaved(format_ptr) || topo_is_compressed(format_ptr->bitstreamFormat))
    {
        internal_buf_ptr->offset_in_samples_write /= format_ptr->channels;
    }
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"audproc_svc: internal buffer offsetInSamples after processing = %d",internal_buf_ptr->offset_in_samples_write);
}

static void  topo_check_ec_module_presence(topo_struct *topo_ptr,adsp_amdb_module_handle_info_t *handle_info_ptr,uint32_t num_modules,uint32_t *ec_module_index)
{
       uint32_t module_num = 0;
       for(module_num=0;module_num<num_modules;module_num++)
       {
           adsp_amdb_module_handle_info_t *handle_info_temp = handle_info_ptr+module_num;
      if(CAPI_V2 == handle_info_temp->interface_type && (TRUE == is_amdb_module_handle_valid(handle_info_temp)))
           {
               // Query for the optional properties now.
               capi_v2_num_needed_framework_extensions_t num_extensions = { 0 };
               capi_v2_prop_t optional_props[] = {
                       { CAPI_V2_NUM_NEEDED_FRAMEWORK_EXTENSIONS,  { reinterpret_cast<int8_t*>(&num_extensions), 0, sizeof(num_extensions)   }, { FALSE, FALSE, 0} }
               };
               capi_v2_proplist_t optional_proplist = { SIZE_OF_ARRAY(optional_props), optional_props };
               capi_v2_err_t err_code = adsp_amdb_capi_v2_get_static_properties_f(handle_info_temp->h.capi_v2_handle, NULL, &optional_proplist);
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX Module with ID 0x%lX needs %d framework extensions", topo_ptr->obj_id, handle_info_temp->id1, num_extensions.num_extensions);
               if(CAPI_V2_FAILED(err_code))
               {
                   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX Module with ID 0x%lX failed in getting some optional static properties with %d. Using default values.", topo_ptr->obj_id, handle_info_temp->id1, static_cast<int>(err_code));
               }
               if(num_extensions.num_extensions > 0)
               {
                   // allocate memory for extention id's based on the num of extentions.
                   capi_v2_framework_extension_id_t fwk_extn_ptr[num_extensions.num_extensions];
                          // query extension id's from the module
                       capi_v2_prop_t static_props_extn[] = {
                               { CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS, { reinterpret_cast<int8_t *>(fwk_extn_ptr), 0, sizeof(capi_v2_framework_extension_id_t) * num_extensions.num_extensions }, { FALSE, FALSE, 0} },
                       };
                       //optional_proplist = { SIZE_OF_ARRAY(static_props_extn), static_props_extn };
                       optional_proplist.props_num = SIZE_OF_ARRAY(static_props_extn);
                       optional_proplist.prop_ptr  = static_props_extn;

                       err_code = adsp_amdb_capi_v2_get_static_properties_f(handle_info_temp->h.capi_v2_handle, NULL, &optional_proplist);
                       if(CAPI_V2_EOK != err_code)
                       {
                           MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO," get_static_prop: failed for module_id(0x%lx), prop_id: CAPI_V2_NEEDED_FRAMEWORK_EXTENSIONS, result(%ld)", handle_info_temp->id1, err_code);
                       }
                       else
                       {
                           for(uint32_t i = 0; i < num_extensions.num_extensions; i++)
                           {
                               if(FWK_EXTN_ECNS == fwk_extn_ptr[i].id)
                               {
                                   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO," get_static_prop: module_id(0x%lx) would need extension_id(0x%lx)", handle_info_temp->id1, fwk_extn_ptr[i].id);
                                   *ec_module_index = module_num;
                                   return;
                               }
                           }
                       }
               }
           }
       }
       return;
}

static bool_t is_amdb_module_handle_valid(const adsp_amdb_module_handle_info_t *amdb_module_handle_ptr)
{
   if(ADSP_FAILED(amdb_module_handle_ptr->result) || (NULL == amdb_module_handle_ptr->h.capi_v2_handle))
   {
      return FALSE;
   }
   return TRUE;
}

void topo_handle_eos(topo_struct *topo_ptr, uint32_t port_id, bool_t is_eos)
{
    topo_ptr->is_eos_set[port_id] = is_eos;

    if(port_id == TOPO_FAR_STRM && topo_ptr->ec_extn_info)
    {
            topo_ptr->ec_extn_info->is_far_port_active  = topo_ptr->data_format_received[port_id] && (!is_eos);
    }
    return;
}

bool_t topo_is_port_or_stream_active(topo_struct *topo_ptr, uint32_t port_id)
{
    return (topo_ptr->data_format_received[port_id] && (!topo_ptr->is_eos_set[port_id]));
}

uint32_t topo_bg_th_out_buff_num_unit_frames(topo_struct *topo_ptr)
{
    return topo_ptr->ouput_buffer_num_unit_frames;
}

uint32_t topo_main_th_out_buff_num_unit_frames(topo_struct *topo_ptr)
{
    return topo_ptr->main_th_ouput_buffer_num_unit_frames;
}


uint32_t topo_get_algo_delay(topo_struct *topo_ptr)
{
    return topo_ptr->delay;
}

bool_t topo_isBgThreadReq(topo_struct *topo_ptr)
{
    return (topo_ptr->main_th_ouput_buffer_num_unit_frames != topo_ptr->ouput_buffer_num_unit_frames);
}

uint32_t topo_get_stack_size(topo_struct *topo_ptr)
{
    return topo_ptr->stack_size;
}

bool_t topo_is_ec_present(topo_struct *topo_ptr)
{
    return (topo_ptr->ec_extn_info != NULL);
}

bool_t topo_is_port_req_more_data(topo_struct *topo_ptr, uint32_t port_id)
{
   bool_t flag = FALSE;

   if(!topo_is_port_or_stream_active(topo_ptr,port_id))
   {
       return FALSE;
   }

   for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
   {
       module_info *m = &topo_ptr->module[i];
       if(m->is_in_chain)
       {
           if(port_id < m->num_input_ports)
           {
              if(m->input_ports_ptr[port_id].is_data_available == FALSE)
              {
#ifdef DBG_TOPO_HANDLER
                MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_is_port_req_more_data: Module-Instance with id 0x%lX-0x%lX. requires more data on port %lu",
                topo_ptr->obj_id, m->module_id, m->instance_id, port_id);
#endif
                  flag = TRUE;
                  break;
              }
           }
       }
   }
   return flag;
}


bool_t topo_any_module_with_main_th_unit_frames(topo_struct *topo_ptr, uint32_t port_id)
{
    bool_t flag = FALSE;

    for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
    {
        //module_info *m = &topo_ptr->module[i];
        if(topo_ptr->main_th_ouput_buffer_num_unit_frames ==
           topo_get_unit_frames_in_module_input_buffer(topo_ptr, i, port_id))
        {
           flag = TRUE;
		   break;
		}
    }
    return flag;
}

static uint32_t topo_get_unit_frames_in_module_input_buffer(topo_struct *topo_ptr, uint32_t module_index, uint32_t port_id)
{
   const topo_port_index_t current_in_port = { module_index, port_id, topo_port_index_t::INPUT_PORT };
   topo_buf_t *buf_ptr = topo_module_get_input_buffer(topo_ptr, &current_in_port);
   module_input_port_t *input_ports_ptr  = &topo_ptr->module[module_index].input_ports_ptr[port_id];
   uint32_t   samples_unit_frame = 0;
   uint32_t num_unit_frames = 0;

   capi_v2_standard_data_format_t *format_ptr = &input_ports_ptr->media_event.event.f;
   elite_svc_get_frame_size(format_ptr->sampling_rate,&samples_unit_frame);
   num_unit_frames = (buf_ptr->valid_data_end - buf_ptr->valid_data_start)/(samples_unit_frame * (format_ptr->bits_per_sample/8)) ;

#ifdef DBG_TOPO_HANDLER
                   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_get_unit_frames_in_module_input_buffer: Module-Instance with id 0x%lX-0x%lX. has %lu unit frames of data on port %lu",
                   topo_ptr->obj_id, topo_ptr->module[module_index].module_id, topo_ptr->module[module_index].instance_id, num_unit_frames, port_id);
#endif   
   
   if(format_ptr->data_interleaving == CAPI_V2_INTERLEAVED)
   {
       num_unit_frames = num_unit_frames/format_ptr->num_channels;
   }

   return num_unit_frames;
}


void topo_print_module_buffers_samples(topo_struct *topo_ptr)
{
    topo_buf_t *buf_ptr;
    uint32_t size;

    for (uint32_t i = 0; i < topo_graph_num_modules(&topo_ptr->graph); i++)
    {
        module_info *m = &topo_ptr->module[i];
        if(m->is_in_chain)
        {
           for(uint32_t port_id = 0; port_id < m->num_input_ports; port_id++)
           {
               const topo_port_index_t current_in_port = { i, port_id, topo_port_index_t::INPUT_PORT };
               buf_ptr = topo_module_get_input_buffer(topo_ptr, &current_in_port);
               size = buf_ptr->valid_data_end - buf_ptr->valid_data_start;
               if(size != 0)
               {
#ifdef DBG_TOPO_HANDLER
                   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX topo_get_module_buffers_samples: Module-Instance with id 0x%lX-0x%lX. has %lu bytes of data on port %lu",
                   topo_ptr->obj_id, m->module_id, m->instance_id, size, port_id);
#endif
               }
           }
       }
   }

   return;
}


#if 0 // Can be used in test layer to enable or disable a module from topo layer
static void topo_module_enable(topo_struct *topo_ptr,module_info *module_info_ptr,bool_t is_enable)
{
    ADSPResult err_code = ADSP_EOK;
    struct topo_module_disable_param_t
    {
        asm_stream_param_data_v3_t param_payload;
        audproc_enable_param_t enable_param;
    } topo_module_enable_param;

    topo_module_enable_param.param_payload.module_id = module_info_ptr->module_id;
    topo_module_enable_param.param_payload.instance_id = module_info_ptr->instance_id;
    topo_module_enable_param.param_payload.param_id = AUDPROC_PARAM_ID_ENABLE;
    topo_module_enable_param.param_payload.param_size = sizeof(audproc_enable_param_t);
    topo_module_enable_param.param_payload.reserved = 0;
    topo_module_enable_param.enable_param.enable = is_enable;

    err_code = topo_set_param(topo_ptr, &topo_module_enable_param, sizeof(topo_module_enable_param), GIVEN_INSTANCE);
    if(ADSP_FAILED(err_code))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO," audproc_svc : Failure in setting enable(disable) param for module_id(0x%lx) ", module_info_ptr->module_id);
    }

}
#endif
ADSPResult topo_module_register_events(topo_struct *topo_ptr, uint32_t event_id, uint32_t module_id,  uint16_t instance_id, uint32_t config_mask)
{
	uint32_t index = 0;
	bool_t is_registered;
	capi_v2_prop_t prop;
	capi_v2_register_event_to_dsp_client_t payload;
	capi_v2_err_t err_code;
	capi_v2_t *module_ptr;

	if(config_mask & AUDPROC_MODULE_REGISTER_EVENT_FLAG)
	{
		is_registered = TRUE;
	}
	else
	{
		is_registered = FALSE; /* this means that the property is de registered */
	}

	/* set up the register event propid */
	prop.id = CAPI_V2_REGISTER_EVENT_DATA_TO_DSP_CLIENT;
	prop.payload.actual_data_len = sizeof(capi_v2_register_event_to_dsp_client_t);
	prop.payload.max_data_len    = sizeof(capi_v2_register_event_to_dsp_client_t);
	prop.port_info.is_valid      = FALSE;

	payload.event_id     = event_id;
	payload.is_registered  = is_registered;

	prop.payload.data_ptr = reinterpret_cast<int8_t*>(&payload);

	capi_v2_proplist_t proplist;
	proplist.props_num = 1;
	proplist.prop_ptr = &prop;


	ADSPResult errCode = topo_find_module_instance(topo_ptr, module_id, instance_id, &index);
	if(ADSP_SUCCEEDED(errCode))
	{
		module_ptr = topo_ptr->module[index].module_ptr;
		err_code   = module_ptr->vtbl_ptr->set_properties(module_ptr,&proplist);

		if(CAPI_V2_FAILED(err_code))
		{
			return  capi_v2_err_to_adsp_result(err_code);
		}
	}
	else
	{
		return errCode;
	}
	return ADSP_EOK;
}
uint32_t topo_module_get_num_modules(topo_struct *topo_ptr)
{
	if(topo_ptr)
	{
		uint32_t num_modules = topo_graph_num_modules(&topo_ptr->graph);
		return num_modules;
	}
	else
	{
		return 0;
	}
}
uint32_t topo_module_get_module_id(topo_struct *topo_ptr, uint32_t index)
{
	return topo_ptr->module[index].module_id;
}

uint32_t topo_module_get_instance_id(topo_struct *topo_ptr, uint32_t index)
{
	return topo_ptr->module[index].instance_id;
}


ADSPResult topo_module_find_module_instance_presence(const topo_struct *topo_ptr, const uint32_t module_id, const uint32_t instance_id)
{
	ADSPResult result = ADSP_EOK;
	uint32_t dummy_var = 0;
	result = topo_find_module_instance(topo_ptr, module_id, instance_id, &dummy_var);
	return result;
}
