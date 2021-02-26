#ifndef VPRX_MODULES_H
#define VPRX_MODULES_H

/****************************************************************************
Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
****************************************************************************/
/*====================================================================== */

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_rx/src/Vprx_Modules.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
21/10/11   ss       Created file.

========================================================================== */
#include "VoiceProcRx.h"
#include "adsp_vcmn_api.h"
#include "adsp_vparams_api.h"
#include "adsp_vparams_internal_api.h"
#include "adsp_media_fmt.h"
#include "voice_delivery.h"
#include "VoiceTimer.h"

#include "Elite.h"
#include "VoiceMsgs.h"
#include "EliteMsg_Util.h"
#include "VoiceCmnUtils.h"
#include "voice_capi_utils.h"
#include "adsp_vpm_api.h"
#include "CGenericResamplerLib.h"

#include "voice_multi_ch_circ_buf.h"
#include "voice_proc_mgr.h"

#include "voice_capi_v2_sample_slip.h"
#include "shared_lib_api.h"
#include "mvm_api_i.h" //for calibration utility for set param

#include "voice_module_topo_db_if.h" //for static topology and module database

#include "voice_capi_utils.h"
#include "Elite_CAPI_V2_types.h"
#include "capi_v2_adsp_error_code_converter.h"
#include "Elite_fwk_extns_hpcm.h"
#if defined(__qdsp6__) && !defined(SIM)
#include "VoiceLoggingUtils.h"
#endif

extern "C" {
}

#define VOICE_PARAM_LOOPBACK_ENABLE (0x00010E18)
#define ROUNDTO8(x) ((((uint32_t)(x) + 7) >> 3) << 3);
#define VPRX_SS_MULTIFRAME VOICE_SS_MULTI_FRAME_4

//process buffer indices
#define PROC_BUF_1       0
#define PROC_BUF_2       1
#define MAX_PROC_BUFS    2

#define VPRX_MAX_CHAN    (2)

#define VPRX_MAX32(x,y)    ((x>y)?x:y)

// This contains all the required data for a service instance. Like the data members of a class...
static const uint32_t NAME_LEN = QURT_ELITE_DEFAULT_NAME_LEN;

// Frame buffer size in NB, 20ms in samples
static const uint32_t FRAME_SAMPLES = VOICE_RX_INPUT_FRAME_LEN * 8;

// DMA buffer size in NB, 1ms, note: this has to be a power of 2
static const uint32_t DMA_SAMPLES = 8;

// Prebuffer size in ms
static const uint32_t PRE_BUF = 1;

// Host PCM processing size, 20 ms NB
static const uint32_t HOST_PCM_FRAME_SAMPLES = 160;

// Buffer size of output circular buffer
static const uint32_t OUTBUF_SAMPLES = FRAME_SAMPLES + 5*DMA_SAMPLES;  // NB 20ms (2msec Vdec process jitter+1msec Sample Stuff+1msec Sample Slip + 1msec System jitter)

#define VPRX_SELF_STACK_IN_BYTES     (8*1024)
#define VPRX_MIN_STACK_IN_BYTES      (10*1024)    //Stack for none topo

//PP modules added by framework
static const uint32_t vprx_pp_modules[] = {
      VOICE_MODULE_SOFT_MUTE,
      VOICE_MODULE_SAMPLE_SLIP
};

//Number of PP modules added by framework
#define VPRX_SOFT_MUTE_INDEX    0
#define VPRX_SAMPLE_SLIP_INDEX  1
#define VPRX_NUM_PP_MODULES (VOICE_SIZE_OF_ARRAY(vprx_pp_modules))


typedef enum
{
   HPCM = 0,
   NUM_SPCL_CAPIS
}vprx_special_capi_modules_t;

typedef struct vprx_timing_info_t
{
   elite_svc_handle_t             *vds_handle_ptr;                  // pointer to  voice timer cmd Q
   Vtm_SubUnsubMsg_t              delivery_tick_vtm_struct;         // msg payload used to subscribe/unsubscribe timer
   uint32_t                       vds_client_id;                    // client id returned by VDS
   uint32_t                       vds_client_token;                 // client token sent to VDS
   uint32_t                       delivery_offset;
   uint32_t                       session_thread_clock_mhz;
   uint32_t                       modules_kpps;
   uint32_t                       dec_offset;
   qurt_elite_queue_t             *vtm_cmdq_ptr;                    // pointer to  voice timer cmd Q
   uint32_t                       vsid;                             // voice system id for hard vfr timing
   uint8_t                        timing_ver;                       // version of timing to be used
   uint8_t                        use_new_timing_flag;              // flag to distinguish different versions of timing cmd
   uint32_t                       modules_delay;                            // total algo+buffering delay added by vprx
   uint32_t                       vfr_vprx_seq;                     // To keep count of vprx calls every VFR cycle
}vprx_timing_info_t;

typedef struct vprx_vpm_apr_info_t
{
   /*The following is necessary information for direct communication with client
    and for talking with APR*/
   uint32_t    apr_handle;
   /**< This is the apr handle that is required in sending ack. */
   uint16_t    self_addr;
   /**< self's address (16bit) */
   uint16_t    self_port;
   /**< slef's port (16bit) */
   uint16_t    client_addr;
   /**< Client's address (16bit)*/
   uint16_t    client_port;
   /**< Client's port (16bit) */
}vprx_vpm_apr_info_t;

// Structure for managing Vprx module based processing
// arranged in the order the modules are processed in the topology
typedef struct vprx_modules_t
{
   bool_t                         module_list_locked;           // flag to indicate that all the modules are created and initialized.
   bool_t                         is_prop_media_fmt;            // flag to indicate that service is propagating media format
   bool_t                         is_processing;                // flag to indicate that service is processing data
   uint32_t                       num_modules;
   voice_module_t*                modules_list;
   voice_capi_module_t*           modules_array;
   elite_cmn_topo_db_entry_t*     topo_entry_ptr;
   uint32_t                       topo_entry_size;
   uint32_t                       topo_entry_handle;
   adsp_amdb_module_handle_info_t *handle_info_ptr;                 // Module handle information from AMDB
   voice_capi_module_t            *special_capis[NUM_SPCL_CAPIS];   // pointers of special capi modules
   vprx_vpm_apr_info_t            apr_info_hpcm;                    // apr info to send/process apr msgs with client for HPCM
   bool_t                         hpcm_ss_enable;
   bool_t                         hpcm_enable_flag;                // flag for re-sending HPCM enable/start to HPCM CAPI
   voice_tap_point_v2_t           hpcm_tap_point;                  // used to cache HPCM tap-point info for re-enabling when necessary 
}vprx_modules_t;

// Structure for managing drift detection and correction
typedef struct vprx_ratematching_info_t
{
   voice_afe_drift_info_t     voice_drift_info;              // structure for av timer vs device drift tracking
}vprx_ratematching_info_t;

// Structure for top-level session configuration.  In general, the config that is controlled directly by VPM/CVP in create/reinit,

typedef struct vprx_session_info_t
{
   uint32_t                   topology_id;                      // Default or None
   uint32_t                   in_sampling_rate;                 // input sampling rate
   uint32_t                   session_num;                      // Session Number
   uint8_t                    in_nb_sampling_rate_factor;       // 1 or 2 or 6
   uint16_t                   vfr_mode;                         // VFR Mode
   uint8_t                    vfr_source;                       // Source to use with AFE for drift detection
   uint32_t                   vfr_cycle;                        // VFR cycle configured for the session
   uint32_t                   stack_size;                       // stack allocate for the session
   bool_t                     stack_changed;                    // flag to indicate stack has changed during reconfig
   qurt_elite_thread_t        cached_tid;
}vprx_session_info_t;

typedef struct vprx_afe_info_t
{
   uint16_t                   afe_rx_port_id;                   // AFE Rx port id
   void                       *afe_drift_ptr;                   // pointer for AFE drift info
}vprx_afe_info_t;

typedef struct vprx_memory_t
{
   int8_t                *pStartAddr;
   int8_t                *pUsageAddr;
   uint32_t              nSize;
   int8_t                *guard_band_ptrs; // pointer of memory which stores guard band memory pointers of each RX PP module.Used to catch memory corruption.
   uint32_t              *module_size;    // pointer of memory size of RX pp modules.
}vprx_memory_t;

typedef struct vprx_input_data_t
{
   char_t                               dataq_name[NAME_LEN];      // input data queue name
   elite_multi_channel_pcm_fmt_blk_t    mediatype;                 // In input media type
   circbuf_struct                       circ_struct;               // Circular buffer structure for In data
   int8_t                               *circ_buf_ptr;             // Circ buffer pointer for In
   int32_t                              frame_samples;             // Frame size of input data in samples
}vprx_input_data_t;

typedef struct vprx_output_data_t
{
   char_t                               bufq_name[NAME_LEN];       // output buffer queue name
   int8_t                               send_mediatype_flag;       // 1 = need to send media type dwn strm
   qurt_elite_queue_t                   *bufq_ptr;                 // output buffer queue
   uint32_t                             bufs_allocated;            // Keep track of number of output bufs in circulation
   elite_multi_channel_pcm_fmt_blk_t    mediatype;                 // Output media type
   voice_multi_ch_circ_buf_t            circ_struct;               // Circular buffer structure for Out data
   uint32_t                             min_frame_samples;         // Minimum output frame size in samples
   int32_t                              buf_samples;               // Frame/Buffer size of Output data in samples
   int32_t                              frame_samples;             // Frame samples size of Output data in sframeamples

}vprx_output_data_t;

typedef struct vprx_io_t
{
   vprx_input_data_t              input;                            //  data in
   vprx_output_data_t             output;                           //  data out
   uint32_t                       min_process_frame_samples;        // Minimum processing frame size in samples
   uint32_t                       max_samp_rate;                    // Maximus sampling rate of all modules
   uint32_t                       max_chan_num;                     // Maximus number of channels of all modules
   void                          *proc_buf_ptr[VPRX_MAX_CHAN];      // Buffer Pointer for voice processing on Rx path of size 20ms + 1ms for SS
   void                          *proc_buf_ptr_sec[VPRX_MAX_CHAN];  // This is to take care of non-in-place modules
}vprx_io_t;

typedef struct vprx_reconfig_info_t
{
   bool_t                      reconfig_pending;                    //flag to indicate reconfig pending state.
   bool_t                      retain_amdb_handle;                  //flag to indicate reconfig to same topo is received.
   bool_t                      another_thread_launched;             //flag to indicate new thread is launched as part of reconfig.
}vprx_reconfig_info_t;

typedef struct vprx_t
{
   elite_svc_handle_t         svc_handle;                          // handle to give out to others - first element
   elite_svc_handle_t         *downstream_peer_ptr;                // This service only supports 1 downstream peer
   qurt_elite_signal_t        *delivery_miss_signal_ptr;                  // signal pointer to take action based on delivery miss
   qurt_elite_signal_t        *resync_response_signal_ptr;                 // signal pointer to handler resync response.
   qurt_elite_queue_t         *respq_ptr;                          // pointer to response queue

   qurt_elite_channel_t       qurt_elite_channel;                  // Channel for the queues
   char_t                     cmdq_name[NAME_LEN];                 // input command queue name
   char_t                     respq_name[NAME_LEN];                // response queue name
   char_t                     gp_dataq_name[NAME_LEN];             // gpq input data queue name
   uint32_t                   wait_mask;                           // Wait mask
   uint32_t                   rcvd_mask;                           // Received mask
   int32_t                    bit_pos;                             // Bit Position
   uint32_t                   resync_count;                        // keep a track of number of resyncs

   voice_cmn_time_profile_t   profiler;                            // collect timing stats
   vprx_timing_info_t         timing;                              // top level timing information structure
   vprx_ratematching_info_t   ratematch;                           // Structure managing drift detection and sample slip/stuff
   vprx_modules_t             modules;                             // Structure containing the modules in the processing chain
   vprx_reconfig_info_t       reconfig_state;                      // Structure to store reconfig time optimization info
   vprx_session_info_t        session;                             // Structure managing top level session configuration parameters
   vprx_afe_info_t            afe_info;                            // Structure managing AFE interface
   vprx_memory_t              memory;                              // Sturcture managing the memory reqd for vprx, and memory corruption detect
   vprx_io_t                  io;                                  // Structure managing Vprx dataflow and buffering

   uint32_t                   shared_mem_client;                   // Client token to getvirtual address and invalidate cache
   uint16_t                   mute;                                // Mute state (0 - Unmute, 1 - Mute)
   int8_t                     process_data_flag;                   // flag indicating the run and running state
   uint8                      loopback_enable_flag;                // loopback enable flag
   capi_v2_buf_t              io_buf[MAX_PROC_BUFS][VPRX_MAX_CHAN];//stream data as per voice_capi_io_stream_data_idx_t
   capi_v2_stream_data_t      io_stream_data[MAX_PROC_BUFS];       //vptx input and output stream data
   bool_t                     is_resync_detected;
   bool_t                     last_delivery_failed;
   bool_t		      fatal_state;
} vprx_t;




ADSPResult vprx_allocate_mem(vprx_t* pVprx, uint32_t in_circ_samples);
ADSPResult vprx_allocate_mem_free(vprx_t* pVprx);
ADSPResult vprx_modules_full_init(vprx_t* pVprx);
void vprx_modules_end(vprx_t* pVprx);
ADSPResult vprx_modules_process(vprx_t* pVprx);
ADSPResult vprx_modules_set_param(vprx_t* pVprx, uint32_t payload_address, uint32_t payload_size,uint32_t cal_param_id_ver );
ADSPResult vprx_modules_get_param(vprx_t* pVprx, void* cal_param_payload, uint32_t cal_param_id_ver, uint16_t* param_size);
ADSPResult vprx_modules_set_param_int(vprx_t* pVprx, uint32_t mod_id, uint16_t instance_id, uint32_t param_id, void* payload_address, uint32_t payload_size );
ADSPResult vprx_aggregate_modules_kpps(vprx_t* pVprx, uint32_t* kpps_changed);
ADSPResult vprx_aggregate_modules_delay(vprx_t* pVprx);

ADSPResult vprx_handle_fwk_extension(vprx_t* pVprx,voice_capi_module_t* module_info);
#endif // VPRX_MODULES_H
