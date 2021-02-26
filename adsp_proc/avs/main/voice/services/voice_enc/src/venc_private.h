/*========================================================================

*//** @file VoiceEnc.cpp

Copyright (c) 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the code for Voice Encoder (Venc) Dynamic service. This service
has one thread. It receives commands from Voice Stream Manager (VSM).
Venc initializes the vocoder algorithms and processes the data. Multiple Venc can be
instantiated and each of them is identified by a session number provided
while creating.
*//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_enc/src/venc_private.h#1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  10/30/12  ss   Created file.
  ========================================================================== */
#ifndef VOICEENCPRIVATE_H
#define VOICEENCPRIVATE_H

#include "venc_svc.h"

#include "Elite.h"
#include "VoiceTimerMsgs.h"
#include "EliteMsg_Util.h"
#include "VoiceCmnUtils.h"
#include "Voice_SampleSlip.h"
#include "AFEInterface.h"
#if defined(__qdsp6__) && !defined(SIM)
#include "VoiceLoggingUtils.h"
#endif
#include "VoiceTimer.h"
#include "voice_delivery.h"

/* APR/API related */
#include "EliteAprIf.h"
#include "apr_comdef.h"
#include "adsp_vsm_api.h"
#include "adsp_vcmn_api.h"
#include "adsp_vparams_api.h"
#include "adsp_vparams_internal_api.h"
#include "Elite_fwk_extns_service_state.h"
#include "Elite_fwk_extns_dtmf_gen.h"
#include "adsp_media_fmt.h"

#include "voice_capi_utils.h"
#include "adsp_amdb_static.h"
#include "capi_v2_adsp_error_code_converter.h"
#include "Elite_fwk_extns_hpcm.h"
extern "C" {
#include "voice_resampler.h"
#include "voice_cross_fade_api.h"
}

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

// This contains all the required data for a service instance. Like the data members of a class...
static const uint32_t NAME_LEN = QURT_ELITE_DEFAULT_NAME_LEN;

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
typedef enum
{
   HPCM = 0,
   NUM_SPCL_CAPIS
}venc_special_capi_modules_t;

typedef struct
{
   int8_t                   *start_addr_ptr;
   int8_t                   *usage_addr_ptr;
   uint32_t                 size;
} venc_memory_t;

// Structure for managing Record
typedef struct venc_rec_info_t
{
   // Book keeping
   bool_t                   enable;                       // flag to indicate whether recording is enabled or not
   uint32_t                 mode;                         // in call recording mode
   uint32_t                 bufs_allocated;               // Keep track of number of output bufs
   int8_t                   first_frame;                  // to indicate recording first frame

   // Port Handles
   elite_svc_handle_t       *afe_handle_ptr;              // handle to push the data for recording
   void                     *afe_drift_ptr;               // for afe drift in record
   uint16_t                 aud_ref_port;

   // Queues
   qurt_elite_queue_t       *buf_q_ptr;                   // recording buffer queue
   char_t                   buf_q_name[NAME_LEN];         // output buffer queue name

   // Buffers
   circbuf_struct           circ_struct_encout;           // circular buffer for encoder input
   int8_t                   *circ_encout_buf_ptr;         // pointer to cicular buffer

   // Drift handling
   voice_cmn_drift_info_t   voice_cmn_drift_info;         // structure for av/hp timer vs device drift tracking
   int8_t                   *ss_struct_ptr;               // pointer to sample slip structure
   ss_struct_type_t         ss_struct;                    // Structure for Sample Slip algorithm
   uint8_t                  ss_multiframe_counter;        // Indicate number of 10 msec frames left before present drift correction is done
   int32_t                  ss_info_counter;              // Indicates the number of samples to be slipped or stuffed

} venc_rec_info_t;

// Structure for managing Out of band processing
typedef struct venc_oob_info_t
{
   int32_t                  *pkt_exchange_ptr;            // Out of band packet exchange pointer
   bool                     enc_pkt_consumed;             // flag to know enc pkt consumed by client or not
   uint32_t                 pkt_miss_ctr;                 // stores the count of un succesful OOB pkt delivery due to previous pkt not read by client
   uint32_t                 pkt_consumed_ctr;             // stores the count of pkt consumed by client
   elite_mem_shared_memory_map_t
                            memmap;                       // stores memory address used by VENC for outof band pkt exchange handling
} venc_oob_info_t;

// Structure for managing Vocoders
typedef struct venc_voc_info_t
{
   int64_t*                 enc_state_ptr;                //TODO - to be removed
   uint32_t                 dtx_mode;                     // DTX mode for AMR-NB, AMR-WB, FR, HR and EFR
} venc_voc_info_t;

typedef struct venc_thread_info_t
{
   uint32_t              stack_size;                       // stack allocate for the session
   qurt_elite_thread_t   cached_tid;                       // cached threadId of original thread which spawns second thread.
   bool_t                another_thread_launched;          // flag to indicate new thread is launched as part of reconfig.

}venc_thread_info_t;

typedef struct venc_encoder_capi_t
{
   voice_capi_module_t common;
   uint32_t in_threshold_in_bytes;
   uint32_t out_threshold_in_bytes;
   voice_capi_data_format_struct_t input_media_type;
   voc_evt_operating_mode_update_t operating_mode;
   adsp_amdb_module_handle_info_t amdb_handle_info;
} venc_encoder_capi_t;



// Structure for managing TTY
typedef struct venc_tty_info_t
{
   /* 1x TTY related variables */
   voice_strm_tty_state_t   *state_ptr;                   // pointer to cmn struct used by tx and rx
} venc_tty_info_t;

// Structure for managing CTM TX module
typedef struct venc_ctm_info_t
{
   voice_capi_module_t                  ctm_tx_capi;      // capi instance for ctm tx module
   adsp_amdb_module_handle_info_t       ctm_tx_amdb;      // amdb handle structure for ctm_tx capi module
   uint32_t                             ctm_tx_size;      // memory required in bytes for ctm_tx capi module
   uint16_t                        resync_delay_cnt;      // wait for these many counts before resyncing ctm, each count is 20ms
   uint16_t                              resync_afe;      // wait for these many counts before resyncing ctm, each count is 20ms
   uint16_t                        resync_modem_afe;      // unify modem resync and afe resync for HO cases
} venc_ctm_info_t;

// Structure for managing Pre-processing blocks
typedef struct venc_proc_info_t
{
   CrossFadeCfgType         cross_fade_cfg_struct;
   CrossFadeDataType        cross_fade_data_struct;
   int16_t                  *mute_inp_ptr;                //temp memory required for soft mute
} venc_proc_info_t;

// Structure for managing Pre-processing blocks
typedef struct venc_io_info_t
{
   /* Input buffer */
   int16_t                  *in_buf;                      // local input buffer, todo: remove hardcoding
   uint32_t                 in_buf_size;                  // input buffer size
   /* Output buffer */
   int16_t                  *out_buf;                     // local output buffer, todo: remove hardcoding
   uint32_t                 out_buf_size;                 // output buffer size
   uint16_t                 frame_samples;                // number of samples in a frame
   uint64_t                 ull_timestamp;                // timestamp of input buffer for av time stamp propagation
   uint32_t                 vfr_frame_cntr;
} venc_io_info_t;

// Structure for venc stream pp capis
typedef struct venc_pp_modules_t
{
   voice_capi_module_t                      limiter;      // capi instance for limiter module
   adsp_amdb_module_handle_info_t      limiter_amdb;      // amdb handle structure for limiter capi module
   uint32_t                            limiter_size;      // memory required in bytes for limiter capi module

   voice_capi_module_t                          cng;      // capi instance for limiter module
   adsp_amdb_module_handle_info_t          cng_amdb;      // amdb handle structure for limiter capi module
   uint32_t                                cng_size;      // memory required in bytes for limiter capi module

   venc_ctm_info_t                           ctm_tx;      // Sturcture instance for CTM tx module

   voice_capi_module_t                     dtmf_gen;      // capi instance for dtmf_gen module
   adsp_amdb_module_handle_info_t     dtmf_gen_amdb;      // amdb handle structure for dtmf_gen capi module
   uint32_t                           dtmf_gen_size;      // memory required in bytes for dtmf_gen capi module
   voice_strm_apr_info_t          apr_info_dtmf_gen;      // info to send dtmf_gen ended event

   voice_capi_module_t                    ltetty_tx;      // capi instance for ltetty tx module
   adsp_amdb_module_handle_info_t    ltetty_tx_amdb;      // amdb handle structure for ltetty_tx capi module
   uint32_t                          ltetty_tx_size;      // memory required in bytes for ltetty_tx capi module

   voice_capi_module_t                         hpcm;      // capi instance for host-pcm module
   adsp_amdb_module_handle_info_t         hpcm_amdb;      // amdb handle structure for host-pcm capi module
   uint32_t                               hpcm_size;      // memory required in bytes for host-pcm capi module

   int8_t                           *start_addr_ptr;
   int8_t                           *usage_addr_ptr;
   uint32_t                              total_size;
   voice_capi_data_format_struct_t input_media_type;

   voice_capi_module_t  *special_capis[NUM_SPCL_CAPIS];   // pointers of special capi modules
   voice_strm_apr_info_t              apr_info_hpcm;      // apr info to send/process apr msgs with client for HPCM
   uint16_t                        hpcm_read_enable;
   uint16_t                       hpcm_write_enable;
} venc_pp_modules_t;

// Structure for Managing Voice Encoder
typedef struct venc_t
{
   // Voice Encoder thread specifics
   elite_svc_handle_t       svc_handle;                   // handle to give out to others - first element
   elite_svc_handle_t       *downstream_peer_ptr;         // This service only supports 1 downstream peer
   elite_svc_handle_t       *loopback_handle;             // handle to rx session for loopback mode
   elite_svc_handle_t       *vds_handle_ptr;              // pointer to  VDS handle
   qurt_elite_queue_t       *buf_q_ptr;                   // output buffer queue
   qurt_elite_queue_t       *vtm_cmd_q_ptr;               // pointer to  voice timer cmd Q
   qurt_elite_queue_t       *resp_q_ptr;                  // Queue for handling responses
   qurt_elite_channel_t     qurt_elite_channel;           // Channel for the queues
   char_t                   data_q_name[NAME_LEN];        // input data queue name
   char_t                   cmd_q_name[NAME_LEN];         // input command queue name
   char_t                   buf_q_name[NAME_LEN];         // output buffer queue name
   uint32_t                 wait_mask;                    // Current wait mask
   uint32_t                 rcvd_mask;                    // Received mask
   int32_t                  bit_pos;                      // Bit Position
   uint32_t                 session_num;                  // Session Number

   // Voice Encoder feature structures
   venc_memory_t            memory;                       // Structure for handling memory
   venc_rec_info_t          record;                       // Structure for handling Record
   venc_oob_info_t          oob;                          // Structure for handling Out of Band data
   venc_tty_info_t          tty;                          // Structure for handling TTY
   venc_proc_info_t         proc;                         // Structure for handling all processing blocks
   venc_io_info_t           io;                           // Structure for handling In/Out

   venc_pp_modules_t        modules;                       // structure for voice encoder stream pp capi modules

   // Voice Encoder states
   voice_cmn_time_profile_t time_profile_struct;          // collect timing stats

   uint32_t                 is_encoder_capi;
   bool_t                   is_static_fixed_samp_rate_voc;       //flag to indicate fixed samp_rate static vocoder
   uint32_t                 vocoder_op_mode;              // Current operating mode of vocoder
   uint32_t                 vocoder_op_detection;         // Enable/disable for vocoder mode event
   venc_thread_info_t       thread_info;                  //qurt thread related info
   union{
      venc_voc_info_t          encoder;                   // Structure for handling Vocoders
      venc_encoder_capi_t      encoder_capi;
   };
   int8_t                   process_data;                 // flag indicating the run and running state
   int32_t                  bufs_allocated;               // Keep track of number of output bufs in circulation
   int8_t                   loopback_enable;              // flag to indicate if enc packets should be
   uint32_t                 sampling_rate;                // stores current sampling rate from MediaType message
   uint8_t                  samp_rate_factor;
   uint32_t                 voc_type;                     // vocoder type
   uint16_t                 mute;                         // mute command (0 - Unmute, 1 - Mute, 2 - CNG enable)
   int8_t                   reset_voc_flag;               // flag to indicate if vocoder should be reset next Run()
   uint32_t                 pkt_exchange_mode;            // pkt_exchange_mode
   int8_t                   pkt_ready;                    // flag used to check if pkt is ready for delivery
   uint32_t                 pkt_ctr;                      // count packets, used as client token when sending Enc packets
   uint32_t                 pkt_miss_ctr;                 // counter to track pkt misses
   uint32_t                 vfr_mode;                     // VFR mode (VFR_NONE = 0,
   uint8_t                  vfr_source;
   bool_t                   send_media_type;
   uint32_t                 aggregate_kpps;               // KPPS of all the enabled modules combined.
   uint32_t                 aggregate_delay;              // Delay of all the enabled modules combined.
   bool_t                   fatal_state;

   // Others
   voice_strm_apr_info_t    *apr_info_ptr;                // info to send/process apr messages with client
   Vtm_SubUnsubMsg_t        vtm_sub_unsub_data;           // msg payload used to subscribe/unsubscribe timer
   uint32_t                 vds_client_id;                // client id returned by VDS
   uint32_t                 vds_client_token;             // client token sent to VDS
   uint64_t                 enc_tick_timestamp_us;        // time stamp when encoder delivery offset happens
   char_t                   respq_name[NAME_LEN];         // response queue name
   bool                     pkt_delivery_pending;         // Flag is used to send pkt to VDS after pkt is ready and host consumed prev pkt
   qurt_elite_signal_t      *vds_error_signal_ptr;        // Error signal for VDS to respond back when delivery fails
   uint32_t                 vsid;                         // voice system id for hard vfr timing //PGtodo: refactor further
   uint32_t                 resync_count;                 // keep a track of number of resync
   avtimer_drv_handle_t     avt_drv;
} venc_t;

#endif // #ifndef VOICEENCPRIVATE_H