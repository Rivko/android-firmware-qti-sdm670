#ifndef VPTX_MODULES_H
#define VPTX_MODULES_H

/*========================================================================

*//** @file Vptx_Modules.h

Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/src/Vptx_Modules.h#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/20/11   dc     Created file.
01/24/12   pg     Updated RTD offsets based on 196/394 MHz from 150/300 MHz.
                  Refer to calculator in share-point to know how the offsets are derived.
========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "Elite.h"
#include "VoiceProcTx.h"
#include "Vptx_Modules.h"
#include "VoiceMsgs.h"
#include "VoiceCmnUtils.h"
#include "adsp_vcmn_api.h"
#include "adsp_vparams_api.h"
#include "adsp_vparams_internal_api.h"
#include "adsp_vpm_api.h"
#include "VoiceTimer.h"
#include "AFEInterface.h"
#include "voice_multi_ch_circ_buf.h"
#include "EliteMsg_Util.h"
#include "adsp_afe_service_commands.h"
#include "voice_proc_mgr.h"
#include "adsp_media_fmt.h"
#if defined(__qdsp6__) && !defined(SIM)
#include "VoiceLoggingUtils.h"
#endif

#include "Voice_SampleSlip.h"
#include "CGenericResamplerLib.h"
#include "mvm_api_i.h" //for calibration utility for set param
#include "voice_module_topo_db_if.h" //for static topology and module database

#include "Elite_fwk_extns_ecns.h"
#include "Elite_fwk_extns_avc_rve_imc.h"
#include "Elite_fwk_extns_avctx_ecns_rx_drc_vol_imc.h"
#include "Elite_fwk_extns_dtmf_detection.h"
#include "voice_capi_v2_sample_slip.h"
#include "voice_capi_utils.h"
#include "Elite_CAPI_V2_types.h"
#include "capi_v2_adsp_error_code_converter.h"
#include "Elite_fwk_extns_hpcm.h"

#include "adsp_audproc_api.h"

extern "C" {
    #include "VolumeControl_api.h"
}

extern "C" {
#include "resample_by2_api.h"
}

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

typedef enum
{
   NEAR = 0,
   FAR
} near_far_t;

enum
{
   VPTX_VOICE,
   VPTX_AUDIO
};

// Frame buffer size in NB, 10ms in samples
static const uint32_t FRAME_SAMPLES = 80;

// Frame buffer size in NB, 1ms in samples
static const uint32_t FRAME_SAMPLES_NB_PER_MS = 8;

// length of output buffer delivery in 10ms
static const uint32_t OUT_BUF_CNT = (20/10);

// Pre buffer size of 1ms
// For sample stuffing
static const uint32_t PRE_BUF = 1;

// Jitter buffer size of 2ms
// 1ms for Sample Slipping
// + 1ms for System Jitters
// + 1ms for to accomdate jump in case of fractional port sampling rates.
static const uint32_t JITTER_BUF = 3;

// 1ms block size for prebuffer in samples
#define PREBUF_SAMPLES(SAMPLING_RATE) ((VPTX_DMA_BLOCK(SAMPLING_RATE)) * (PRE_BUF))

// 1ms block size for Jitter buffer in samples
#define JITTERBUF_SAMPLES(SAMPLING_RATE) ((VPTX_DMA_BLOCK(SAMPLING_RATE)) * (JITTER_BUF))

#define HEX                                 6         // number of channels in 5.1
#define  QUAD                               4         // Number of channels in stereo mode
#define  STEREO                             2         // Number of channels in stereo mode
#define  MONO                               1         // Number of channels in mono mode
#define  MAX_RESAMPLER_OUTPUT_SIZE          ((((OUT_BUF_CNT) * FRAME_SAMPLES) * (VOICE_FB_SAMPLING_RATE))/(VOICE_NB_SAMPLING_RATE))

//input ports are fixes by FW
#define VPTX_NEAR_INPUT_PORT   0
#define VPTX_FAR_INPUT_PORT    1
#define VPTX_MAX_INPUT_PORT    2

//primary/lec/nr ports are based on EC.
#define ECNS_MAX_OUTPUT_PORT   3

#define VPTX_MAX_PORT   ((VPTX_MAX_INPUT_PORT > ECNS_MAX_OUTPUT_PORT)?VPTX_MAX_INPUT_PORT:ECNS_MAX_OUTPUT_PORT)

//temp buffers
#define VPTX_TEMP_BUF_1       0
#define VPTX_TEMP_BUF_2       1
#define VPTX_MAX_TEMP_BUFFERS 2

// minimum stack size
#define VPTX_MIN_STACK_IN_BYTES (10 * 1024)

// stack for vptx itself, i.e. assuming no modules are present
#define VPTX_SELF_STACK_IN_BYTES (8 * 1024)

#define INVALID    0xffffffff

// This contains all the required data for a service instance. Like the data members of a class...
static const uint32_t NAME_LEN = QURT_ELITE_DEFAULT_NAME_LEN;

/* -----------------------------------------------------------------------
** Structures
** ----------------------------------------------------------------------- */
typedef enum
{
   ECNS_CAPI = 0,
   AVCRVE_CAPI,
   TX_DTMFDET_CAPI,
   HPCM,
   DYN_RESAMP,
   NUM_SPCL_CAPIS
}vptx_special_capi_modules_t;

// Structure for managing memory allocation and corruption detection mechanism
typedef struct vptx_memory_t
{
   int8_t          *pStartAddr;
   int8_t          *pBufStartAddr;
   int8_t          *pUsageAddr;
   uint32_t        nSize;
   uint32_t        nSizeBuffs;
   int8_t          *guard_band_ptrs; // Array stores guard band memory pointers of each RX PP module.Used to catch memory corruption.
   uint32_t        *module_size;   // Pointer to the array of memory size stored for Tx modules in bytes. SA:TODO: Add pointer
   int8_t          *pCircBufAddr; // memory allocated for circular buffers
   uint32_t        circ_buf_vfr_cycle; //to check if cirbufs need to be reallocated at set_timing
} vptx_memory_t;

// Structure for managing Vptx/AFE interface
typedef struct vptx_afe_info_t
{
   uint16_t                       afe_tx_port_id;
   uint16_t                       afe_rx_port_id;
   void                           *afe_tx_drift_ptr;                // afe drift pointer for mic
   void                           *afe_rx_drift_ptr;                // afe drift pointer for ref path
} vptx_afe_info_t;


// Structure for managing scheduling of Vptx timeling
typedef struct vptx_timing_config_t
{
   uint32_t                       session_thread_clock_mhz;         // minimum thread clock as given from VPM
   uint32_t                       modules_kpps;                     // variable to store the vptx processing requirements in kpps
   uint16_t                       enc_offset;                       // VFR offset
   uint16_t                       sync_offset;                      // VFR offset
   uint16_t                       proc_offset;                      // VFR offset
   uint16_t                       delivery_offset;                  // VFR offset
   qurt_elite_queue_t             *vtm_cmdq_ptr;                    // pointer to  voice timer cmd Q
   elite_svc_handle_t             *vds_handle_ptr;                  // pointer to  VDS handle
   Vtm_SubUnsubMsg_t              sync_tick_vtm_struct;             // msg payload used to subscribe/unsubscribe timer
   Vtm_SubUnsubMsg_t              proc_tick_vtm_struct;             // msg payload used to subscribe/unsubscribe timer
   Vtm_SubUnsubMsg_t              vptx_delivery_tick_vtm_struct;    // msg payload used to subscribe/unsubscribe timer
   Vtm_SubUnsubMsg_t              venc_delivery_tick_vtm_struct;    // msg payload used to subscribe/unsubscribe timer
   uint32_t                       vds_client_id;                    // client id returned by VDS
   uint32_t                       vds_client_token;                 // client token sent to VDS
   int8_t                         ref_time_tick_recvd_flag;         // Indicates if vfr is being received
   uint32_t                       resync_far_end;                   // Indicates that far end resync is pending
   uint32_t                       vsid;                             // voice system id for hard vfr timing
   uint8_t                        use_new_timing_flag;              // flag to distinguish different versions of timing cmd
   uint8_t                        timing_ver;                       // version of timing to be used
   uint32_t                       modules_delay;                    // variable to store the vptx delay in microseconds
} vptx_timing_info_t;


// Structure for managing Vptx module based processing
// arranged in the order the modules are processed in the topology
typedef struct vptx_modules_t
{

   vptx_vpm_apr_info_t            apr_info_dtmf;                    // info to send/process apr messages with client for DTMF status.  TODO: combine with dtmf_det_struct
   vptx_vpm_apr_info_t            apr_info_hpcm;                    // apr info to send/process apr msgs with client for HPCM
   void                           *generic_resampler_ptr;           // Pointer for the generic resampler base class
   uint32_t                       num_modules;
   voice_capi_module_t            *special_capis[NUM_SPCL_CAPIS];   // pointers of special capi modules
   voice_capi_module_t            ss_capi;                          // SS capi structure, This is required because SS is not part of the topology list. and need seperate handling
                                                                    // SS will be created only if EC not present For eg NONE topo or EC does not support ratematching
   adsp_amdb_module_handle_info_t ss_handle_info;                   // SS module handle information from AMDB
   int8_t*                        resampler_out_buf;                // resampler output buffer for Audio instance
   voice_module_t*                modules_list;
   voice_capi_module_t*           modules_array;
   elite_cmn_topo_db_entry_t*     topo_entry_ptr;
   uint32_t                       topo_entry_size;
   uint32_t                       topo_entry_handle;
   bool_t                         module_list_locked;            // flag to indicate that all the modules are created and initialized.
   adsp_amdb_module_handle_info_t *handle_info_ptr;                 // Module handle information from AMDB
   bool_t                         hpcm_ss_enable;
   bool_t                         hpcm_enable_flag;                // flag for re-sending HPCM enable/start to HPCM CAPI
   voice_tap_point_v2_t           hpcm_tap_point;                  // used to cache HPCM tap-point info for re-enabling when necessary
   bool_t                         is_data_threshold_raised;
   int8_t *                       pScrath_mem;
} vptx_modules_t;

typedef struct ec_capi_module_info_t
{
   capi_v2_port_num_info_t num_port;                                  //number of input and output port for ECNS.
   uint32_t     ecns_prim_output_port_idx;
   uint32_t     ecns_lec_output_port_idx;
   uint32_t     ecns_nr_output_port_idx;
   voice_capi_data_format_struct_t input_port[VPTX_MAX_INPUT_PORT];   //ECNS Near and far input port data format
   voice_capi_data_format_struct_t output_port[ECNS_MAX_OUTPUT_PORT]; //ECNS output port data format
   ecns_monitoring_capabilities_t  ecns_monitoring_info;              //ECNS RTM capabilities.
   ecns_stt_capabilities_t         ecns_stt_info;                     //ECNS STT capabilities.
   ecns_vp3_capabilities_t         ecns_vp3_info;
   ecns_rate_matching_capabilities_t ecns_rate_matching_info;         //ECNS rate matching capabilities
   ecns_output_capabilities_t      ecns_output_capabilities;
   ecns_speech_act_det_capabilities_t ecns_speech_act_det_capabilities;
   bool_t                         is_reset_pending;                   // Used to control ECNS reset.
   ecns_multichannel_ref_capabilities_t ecns_multichannel_ref_info;
   bool_t                          force_mono_reference;
}ec_capi_module_info_t;

// Structure for managing drift detection and correction
typedef struct vptx_ratematching_info_t
{
   voice_afe_drift_info_t               voice_drift_info_near;            // structure for av timer vs device drift tracking
   voice_afe_drift_info_t               voice_drift_info_far;             // structure for av timer vs device drift tracking
   int32_t                        ss_buf_samples_near;              // In samples, indicates the number of pre-buffering samples left in near circbuf
   int32_t                        ss_buf_samples_far;               // In samples, indicates the number of pre-buffering samples left in far circbuf

} vptx_ratematching_info_t;

// Structure encapsulating a Vptx input data port
typedef struct vptx_input_data_t
{
   char_t                             dataq_name[NAME_LEN];         // input data queue name
   elite_multi_channel_pcm_fmt_blk_t  mediatype;                    // input media type
   voice_multi_ch_circ_buf_t          circ_buf;                     // Multi channel circular buffer
   uint32_t                           frame_samples;                // Frame size of  input data in samples
   int8_t                             dataflow_on;                  // Indicates if data input is being received
   int8_t                             *process_buf_ptr[HEX];       // For use as scratch buffer for processing data in vptx_process
   int8_t                             *process_buf_ptr_sec[HEX];   // For use as sec scratch buffer for processing data in vptx_process for non-in-place modules
} vptx_input_data_t;

// Structure encapsulating a Vptx output data port
typedef struct vptx_output_data_t
{
    char_t                              bufq_name[NAME_LEN];        // output buffer queue name
    elite_multi_channel_pcm_fmt_blk_t   mediatype;                  // Output media type
    voice_multi_ch_circ_buf_t           circ_buf;                   // Multi channel circular buffer for output data
    uint32_t                            buf_samples;                // Vptx number of output samples for all cases
    qurt_elite_queue_t                 *bufq_ptr;                   // output buffer queue
    uint32_t                            bufs_allocated;             // Keep track of number of output bufs in circulation
    int8_t                              send_mediatype_flag;        // 1 = need to send media type dwn strm
} vptx_output_data_t;

typedef struct vptx_scratch_data_t
{
    int8_t*                             data_ptr;
    uint32_t                            data_size_bytes;
}vptx_scratch_data_t;

// Structure for managing input/output data exchange
typedef struct vptx_io_t
{
   vptx_input_data_t                 near_in;                          // near data in
   vptx_input_data_t                 far_in;                           // far data in
   vptx_output_data_t                output;                           // output data
   vptx_output_data_t                loopback_output;                  // loopback output data
   vptx_scratch_data_t               scratch;                          // to store temporary data
   int32_t                           prebuffer_samples;                // Dma buffer size in samples
   int32_t                           jitter_buffer_samples;            // Jitter buffer size in samples
   uint32_t                          force_far_zeros;                  // Indicates if the far end ref should be made all zeros
   uint16_t                          num_1ms_frame_block_process;     // Number of 1ms blocks to process
   uint32_t                          out_gen_cnt;                      // counter to regulate 20ms delivery in voice path
   uint64_t                          ull_timestamp;                    // timestamp of the input buffer every vfr cycle
} vptx_io_t;

// Structure for top-level session configuration.  In general, the config that is controlled directly by VPM/CVP in create/reinit,
// or Audio instance create
typedef struct vptx_session_info_t
{
   uint32_t                       topology_id;                      // Any guid
   uint32_t                       num_mics;                         // 1 through 4
   uint32_t                       num_ref_channels;                  // 1 through 6
   uint32_t                       sampling_rate;                    // 8k or 16k or 48k
   uint32_t                       session_num;                      // Session Number
   uint8_t                        nb_sampling_rate_factor;          // 1 or 2 or 6
   uint8_t                        nb_sampling_rate_factor_output;   // 1 or 2 or 6
   int8_t                         real_time_flag;                   // VPTX is non real time if psuedo port or no vfr(audio create)
   int8_t                         audio_instance_flag;              // Indicates if its ADM instance or not
   uint16_t                       vfr_mode;                         // VFR Mode
   uint8_t                        vfr_source;                       // Source to use with AFE for drift detection
   uint32_t                       vfr_cycle;                        // VFR cycle configured for session
   int32_t                        vfr_vptx_cntr;                    // To keep count of vptx process calls every VFR cycle
   uint32_t                       stack_size;                       // stack allocate for the session
   bool_t                         stack_changed;                    // flag to indicate stack has changed during reconfig
   qurt_elite_thread_t            cached_tid;
   uint8_t                        channel_map_near[8];              // channel mapping for near port
   uint8_t                        channel_map_far[8];               // channel mapping for reference port
} vptx_session_info_t;

typedef struct vptx_reconfig_info_t
{
   bool_t                      reconfig_pending;                    //flag to indicate reconfig pending state.
   bool_t                      retain_amdb_handle;               //flag to indicate reconfig to same topo is received.
   bool_t                      another_thread_launched;             //flag to indicate new thread is launched as part of reconfig.
}vptx_reconfig_info_t;

// voice activity info structure
typedef struct vptx_voice_activity_status_t
{
   uint8_t                        report_voice_activity;            // flag to indicate that client has requested event
   uint32_t                       activity_type;                    // activity type
} vptx_voice_activity_status_t;

// structure used to request for registration to qmi
typedef struct vptx_qmi_obj_t
{
   void                                *pObj;
   bool_t                              ind_flag;                    // To mark that atleast one indication is recieved from sensors
   bool_t                              reg_done;                    // To mark that Registration is complete.
} vptx_qmi_obj_t;

// Overall Vptx instance structure
typedef struct vptx_t
{
   elite_svc_handle_t             near_svc_handle;                  // near handle to give out to others - first element
   elite_svc_handle_t             far_svc_handle;                   // far handle to give out to others - second element
   elite_svc_handle_t             *downstream_peer_ptr;             // This service only supports 1 downstream peer
   elite_svc_handle_t             *rx_peer_ptr;                     // param passing and loopback mode
   qurt_elite_queue_t             *response_q;                      // Queue for handling responses

   char_t                         cmdq_name[NAME_LEN];              // input command queue name
   char_t                         respq_name[NAME_LEN];             // response queue name
   qurt_elite_channel_t           qurt_elite_channel;               // Channel for the queues
   uint32_t                       wait_mask;                        // Wait mask
   uint32_t                       rcvd_mask;                        // Received mask
   int32_t                        bit_pos;                          // Bit Position
   uint32_t                       resync_count;                     // keep track of number of resync;

   vptx_session_info_t            session;                          // Structure managing top level session configuration parameters
   vptx_reconfig_info_t           reconfig_state;                   // Structure to store reconfig time optimization info
   vptx_afe_info_t                afe_info;                         // Structure managing AFE interface
   vptx_timing_info_t             timing;                           // Structure managing scheduling
   vptx_io_t                      io;                               // Structure managing Vptx dataflow and buffering
   vptx_memory_t                  memory;                           // Sturcture managing the memory reqd for vptx, and memory corruption detect
   vptx_modules_t                 modules;                          // Structure containing the modules in the processing chain
   vptx_ratematching_info_t       ratematch;                        // Structure managing drift detection and sample slip/stuff
   voice_cmn_time_profile_t       profiler;                         // Structure managing profiling information for collecting stats

   uint32_t                       shared_mem_client;                // Client token to getvirtual address and invalidate cache
   uint32_t                       static_svc_apr_handle;            // APR handle of static service creating this dynamic svc
   uint16_t                       log_data_format_near;                  // Used for packet logging
   uint16_t                       log_data_format_far;                  // Used for packet logging
   vptx_voice_activity_status_t   voice_activity_status;            //voice activity status
   vptx_vpm_apr_info_t            vpm_apr_info;                         // voice proc and client APR info

   // top level session state variables
   bool_t      			  fatal_state;						//indicates whether session is in fatal state.
   uint16_t                       mute;                             // Mute state (0 - Unmute, 1 - Mute)
   int8_t                         process_data_flag;                // Process state (0 - Stop, 1 - Run)
   uint8_t                        loopback_enable_flag;             // Loopback state (0 - Off, 1 - On)
   vptx_qmi_obj_t                 qmi;
   ec_capi_module_info_t          ec_capi_info;

   //stream data as per voice_capi_io_stream_data_idx_t
   //TODO: move to modules structure?
   uint32_t                       ec_near_input_buf_idx;
   capi_v2_buf_t                  near_input_buf[VPTX_MAX_TEMP_BUFFERS][HEX];
   capi_v2_buf_t                  far_input[HEX];

   //output buffers
   capi_v2_buf_t                  output_buf[VPTX_MAX_TEMP_BUFFERS];
   capi_v2_buf_t                  lec_output;
   capi_v2_buf_t                  nr_output;

   //No need to create seperate buffers for ECNS input and output.
   //We can have pointers which will point to the input and output bufferes defined above.
   //With this we dont need to copy the data/size for ECNS.
   capi_v2_buf_t                  *ec_near_input[HEX];
   capi_v2_buf_t                  *ec_output[ECNS_MAX_OUTPUT_PORT];

   //vptx input and output stream data
   capi_v2_stream_data_t input_stream_data[VPTX_MAX_TEMP_BUFFERS];
   capi_v2_stream_data_t output_stream_data[VPTX_MAX_TEMP_BUFFERS];

   //ecns input and output stream data
   capi_v2_stream_data_t ec_input_stream_data[VPTX_MAX_INPUT_PORT];
   capi_v2_stream_data_t ec_output_stream_data[ECNS_MAX_OUTPUT_PORT];

   capi_v2_stream_data_t resampler_out_stream_data;
   capi_v2_buf_t         resampler_output;

   /*
    * capi takes array of pointers of capi_v2_stream_data_t size.
    * stream_data_ptrs is a 2D array of pointers with each row correpsonding to 1D array of pointers.
    * each capi stores input and output buffer indices. And these indices point to proper array
    * of stream data pointers.
    */
   capi_v2_stream_data_t *stream_data_ptrs[NUM_STRM_BUF_IDX][ECNS_MAX_OUTPUT_PORT];

   bool_t    is_venc_tick_fired;
   bool_t    process_start_flag;
   uint32_t  max_tx_processing_time;
   uint32_t  proc_start_samples;  // samples required to start first vptx process in a vfr cycle
   bool_t    is_resync_detected_near;
   bool_t    is_resync_detected_far;
} vptx_t;

/* -----------------------------------------------------------------------
** Function prototypes
** ----------------------------------------------------------------------- */
// Function to write processed output into circular buffer
ADSPResult vptx_allocate_mem(vptx_t* pVptx);
ADSPResult vptx_allocate_mem_free(vptx_t* pVptx);
ADSPResult vptx_allocate_circ_bufs(vptx_t* pVptx);
void vptx_free_circ_bufs(vptx_t* pVptx);
void vptx_modules_init(vptx_t* pVptx); // this function is expected to initialize all the algorithm modules with the data provided by vptx_modules_config and load default parameters appropriately
void vptx_modules_end(vptx_t* pVptx);
ADSPResult vptx_process(vptx_t* pVptx); // Main processing function for converting input buffer
ADSPResult vptx_modules_set_param(vptx_t* pVptx, uint32_t nPayload_address, uint32_t nPayload_size,uint32_t cal_param_id_ver );
ADSPResult vptx_modules_get_param(vptx_t* pVptx, void *cal_param_payload, uint32_t cal_param_id_ver, uint32_t* param_size);
ADSPResult vptx_modules_set_param_int(vptx_t* pVptx, uint32_t mod_id, uint16_t instance_id, uint32_t param_id, void* nPayload_address, uint32_t nPayload_size );
ADSPResult vptx_modules_set_param_end(vptx_t* pVptx);
ADSPResult vptx_aggregate_modules_kpps(vptx_t* pVptx, uint32_t* kpps_changed);
ADSPResult vptx_modules_full_init(vptx_t* pVptx);
ADSPResult vptx_aggregate_modules_delay(vptx_t* pVptx);

ADSPResult vptx_handle_fwk_extension(vptx_t* pVptx,voice_capi_module_t* module_info);
ADSPResult voice_topo_get_module_info_tx(vptx_t* pVptx, uint32_t module_id,uint16_t instance_id, uint32_t param_id, voice_capi_module_t **modules_info);
bool_t vptx_is_ready_for_get_param(vptx_t* pVptx, uint32_t module_id, uint32_t param_id);

#endif//#ifndef VPTX_MODULES_H
