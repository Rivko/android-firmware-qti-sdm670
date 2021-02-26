/*========================================================================
 Elite

 This file contains an example service belonging to the Elite framework.

Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
 Edit History

 $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_dec/src/vdec_private.h#2 $

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 07/16/12   pg      Created file. Shared between VoiceDec.cpp and voice_dec_pp.cpp

 ========================================================================== */
#ifndef VOICEDECPRIVATE_H
#define VOICEDECPRIVATE_H

/* =======================================================================
 INCLUDE FILES FOR MODULE
 ========================================================================== */

#include "Elite.h"
#include "VoiceDec.h"
#include "VoiceTimerMsgs.h"
#include "EliteMsg_Util.h"
#include "VoiceCmnUtils.h"
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
#include "adsp_media_fmt.h"
/* PP related header files */
#include "adsp_vpm_api.h"
#include "adsp_vcmn_api.h"
#include "adsp_vparams_api.h"
#include "Voice_SampleSlip.h"
#include "AFEInterface.h"
#include "voice_capi_utils.h"
#include "adsp_amdb_static.h"
#include "adsp_vparams_internal_api.h"
#include "Elite_fwk_extns_hpcm.h"
extern "C"
{
#include "resample_by2_api.h"
}

// circular buffer size at input of worker thread (TW output is written into this)
// Assume min compression (less than and close to a frame) happens, samples are not
// sent downstream as they are less than a frame. Then worst expansion happens to
// 2.5 times frame. circular buffer needs to accomodate, frame * 3.5 samples
#define PP_IN_CIRC_BUF_SIZE_SAMPLES       1120 //in samples

#define DEC_STATE_MEMORY_IN_BYTES   8*1024 //8k bytes which takes care of all decoders except 4GV

#define MAX_SAMP_RATE_FACTOR 6          //(FB/NB)

#define MAX_FRAME_SIZE        960    // at 48k for 20msframe boundary

#define SS_DELAY_RECORD_PATH_DEC            16   // 2msec delay in NB : 1ms for pre-buffering and 1ms for system jitter
// scalable to current sampling rate
// NB default 20msec + 4msec for delay for sample slipping 2ms for pre buffering for slipping & 2 ms to prevent overflow due to stuffing

#define DEC_OUT_CIRC_BUF_SIZE_REC      (160+SS_DELAY_RECORD_PATH_DEC)

// worst scratch memory size = 22 ms for FB. This should match with maximum scratch
// usage in vdec. currently recording scratch buffer requires maximum i.e 22 ms
// extra 2 ms is for sample slipping (1ms) and process jitter(1ms)
// NOTE : this size is in bytes
#define VDEC_SCRATCH_MEM_SIZE_IN_BYTES   (22*48*2)

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
// Maximum data messages in data queue. The size should be 1 to avoid any possibility of extra delay to RTD
static const uint32_t MAX_DATA_Q_ELEMENTS = 4; // To support up to VFR cycle 60
// Maximum data messages in gpq.
static const uint32_t MAX_DATA_GPQ_ELEMENTS = 8;

// Maximum number of commands expected ever in command queue.
static const uint32_t MAX_CMD_Q_ELEMENTS = 8;
// Buffer size. Assume service knows what size buffer it needs to deliver.
// If it needs to be told, have to add a command to be informed.
static const uint32_t BUFFER_SIZE = (1920 + 192);
// 22 ms in bytes for worst case 48 KHz
static const uint32_t REC_BUFFER_SIZE = BUFFER_SIZE * 2;
// recording require twice size for stereo output
static const uint32_t PP_MAX_BUF_Q_ELEMENTS = 2;   // this means 1 output buffer available for pp
// thread to deliver downstream
static const uint32_t MAX_REC_BUF_Q_ELEMENTS = 4;

static const uint32_t NAME_LEN = QURT_ELITE_DEFAULT_NAME_LEN;

static const uint32_t PP_MAX_DATA_Q_ELEMENTS = 8;

enum
{
   VOICE_DEC_PROFILE_DECODER,
   VOICE_DEC_PROFILE_POST_DEC
};

typedef enum
{
   HPCM = 0,
   NUM_SPCL_CAPIS
}vdec_special_capi_modules_t;

typedef struct
{
   int8_t *start_addr_ptr;
   int8_t *usage_addr_ptr;
   uint32_t size;
} vdec_memory_t;

// Structure for managing packet exchange related info
typedef struct vdec_pkt_xchg_t
{
   uint32_t                        pkt_exchange_mode;      // mode of pkt exchange
   uint32_t                        pkt_req_ctr;            // pkt requests counter, can be used as client token when req dec packets
   uint32_t                        pkt_miss_ctr;           // counter to track pkt misses
   int8_t                          dec_pkt_ready;          // flag used to check if pkt is ready for consumption by decoder
   uint32_t                        dec_pkt_not_consumed;   // counter to track unconsumed pkts
   int32_t                         *oob_pkt_exchange_ptr;  // out of band pkt exchange pointer
   bool                            dec_pkt_oob_ready;      // flag to indicate oob dec pkt is ready for dec to consume
   elite_mem_shared_memory_map_t   memmap_vdec_oob;        // stores memory address used by VDEC for outof band pkt exchange handling
   uint32_t                        oob_pkt_miss_ctr;       // counts the number of packets not received in time
   uint32_t                        oob_pkt_ready_ctr;      // counter tracking no. of buf ready events
   int32_t                         max_pkt_req_delay;      // in usec
   int32_t                         min_pkt_req_delay;      // in usec
   int64_t                         curr_pkt_req_time;      // in usec
   int64_t                         curr_pkt_recv_time;     // in usec
} vdec_pkt_xchg_t;

// Structure for managing timing related info
typedef struct vdec_timing_t
{

   Vtm_SubUnsubMsg_t    vtm_sub_unsub_vds_pkt_req_data;   // msg payload used to subscribe/unsubscribe to vtm for pkt req by VDS
   Vtm_SubUnsubMsg_t         vtm_sub_unsub_decode_data;   // msg payload used to subscribe/unsubscribe timer for decode

} vdec_timing_t;

typedef struct vdec_thread_info_t
{

   uint32_t              stack_size;                       // stack allocate for the session
   qurt_elite_thread_t   cached_tid;                       // cached threadId of original thread which spawns second thread.
   bool_t                another_thread_launched;          // flag to indicate new thread is launched as part of reconfig.

}vdec_thread_info_t;

typedef struct vdec_decoder_capi_t
{
   voice_capi_module_t common;
   uint32_t erasure;
   uint32_t packet_size;
   uint32_t in_threshold_in_bytes;
   uint32_t out_threshold_in_bytes;
   voice_capi_data_format_struct_t output_media_type;
   voc_evt_operating_mode_update_t operating_mode;
   adsp_amdb_module_handle_info_t amdb_handle_info;

}vdec_decoder_capi_t;

// Structure for managing decoders data //TODO - to be removed
typedef struct vdec_decoder_t
{
   //common variables
   int64_t               *dec_state_ptr;              // allocate union of all decoder structures = 2k bytes //PG: correct comment

} vdec_decoder_t;

typedef struct vdec_rec_info_t
{
   bool_t                 enable_recording;             // flag to indicate recording is enabled
   uint32_t               recording_mode;               // in call recording mode
   int8_t                 rec_first_frame;              // to indicate recording first frame
   uint16_t               aud_ref_port;                 // afe port id for recording
   void                   *afe_drift_ptr;               // to get drift info for record
   elite_svc_handle_t     *afe_handle_ptr;              // handle to push the data for recording
   char_t                 rec_buf_q_name[NAME_LEN];     // recording buffer queue name
   circbuf_struct         circ_struct_decout_rec;       // circular buffer for decoder output
   int8_t                 *circ_decout_rec_buf_ptr;     // pointer to cicular buffer
   int8_t                 *ss_struct_ptr;               // pointer to sample slip structure
   ss_struct_type_t       ss_struct;                    // Structure for Sample Slip algorithm
   int32_t                ss_info_counter;              // Indicates the number of samples to be slipped or stuffed
   uint8_t                ss_multiframe_counter;        // Indicates the number of 10msec frames before last drift correction is done
   voice_cmn_drift_info_t voice_cmn_drift_info_dec_rec;  // structure for av/hp timer vs device drift tracking
   qurt_elite_queue_t     *rec_buf_q_ptr;                // output buffer queue
   int32_t                rec_bufs_allocated;            // Keep track of number of output bufs in circulation
   int8_t                 send_media_type_rec;          // flag to indicate whether to send MediaType downstream
} vdec_rec_info_t;

typedef struct vdec_mute_info_t
{
   voice_capi_module_t               soft_mute_capi;  // structure for stream mute
   adsp_amdb_module_handle_info_t    soft_mute_amdb;  // amdb handle structure for soft_mute capi module
   uint32_t                          soft_mute_size;  // memory required in bytes for soft mute capi module
   uint16_t                            new_mute_cmd;  // flag to indicate new mute cmd received
   uint16_t                                    mute;  // mute command (0 - Unmute, 1 - Mute)
   voice_soft_mute_cal_param_t     set_param_cached;
} vdec_mute_info_t;

typedef struct vdec_ctm_info_t
{
   voice_capi_module_t                  ctm_rx_capi;  // capi instance for ctm rx module
   adsp_amdb_module_handle_info_t       ctm_rx_amdb;  // amdb handle structure for ctm_rx capi module
   uint32_t                             ctm_rx_size;  // memory required in bytes for ctm_rx capi module
   uint8_t         ctm_from_far_end_detect_notified;
} vdec_ctm_info_t;


typedef struct vdec_oobtty_info_t
{
   int16_t                 oobtty_rx_char;            // interface variable for holding character in rx char push event
   uint32_t                oobtty_client_token;       // variable to hold client token received during OOB TTY process
} vdec_oobtty_info_t;

// Structure for managing Vdec pp module based processing
typedef struct vdec_pp_modules_t
{
   vdec_oobtty_info_t        oobtty;
   vdec_rec_info_t           record;
   uint32_t                  aggregate_kpps;           // KPPS of all the enabled modules combined.
   uint32_t                  aggregate_delay;          // Delay of all the enabled modules combined.

} vdec_pp_modules_t;

typedef struct vdec_modules_t
{
   uint32_t                  is_decoder_capi;
   vdec_decoder_t            decoder;
   vdec_decoder_capi_t       dec_capi;
   uint32_t                  aggregate_kpps;       // KPPS of all the enabled modules combined.
   uint32_t                  aggregate_delay;      // Delay of all the enabled modules combined.
   vdec_pp_modules_t         vdec_pp_modules;      // structure for all post processing modules in vdec

   // vdec stream pp capi modules
   vdec_ctm_info_t                           ctm_rx;   // Sturcture instance for ctm rx module

   voice_capi_module_t                    ltetty_rx;   // capi instance for ltetty rx module
   adsp_amdb_module_handle_info_t    ltetty_rx_amdb;   // amdb handle structure for ltetty_rx capi module
   uint32_t                          ltetty_rx_size;   // memory required in bytes for ltetty_tx capi module

   voice_capi_module_t                    slow_talk;   // capi instance for slow talk module
   adsp_amdb_module_handle_info_t    slow_talk_amdb;   // amdb handle structure for slow talk capi module
   uint32_t                          slow_talk_size;   // memory required in bytes for slow talk capi module

   voice_capi_module_t                     dtmf_det;   // capi instance for dtmf det module
   adsp_amdb_module_handle_info_t     dtmf_det_amdb;   // amdb handle structure for dtmf det capi module
   uint32_t                           dtmf_det_size;   // memory required in bytes for dtmf det capi module
   voice_strm_apr_info_t              apr_info_dtmf;   // info to send/process apr messages with client for DTMF status

   voice_capi_module_t                         hpcm;   // capi instance for hpcm module
   adsp_amdb_module_handle_info_t         hpcm_amdb;   // amdb handle structure for hpcm capi module
   uint32_t                               hpcm_size;   // memory required in bytes for hpcm capi module
   voice_strm_apr_info_t              apr_info_hpcm;   // info to send/process apr messages with client for hpcm

   vdec_mute_info_t                       soft_mute;   // Sturcture instance for soft mute rx module

   int8_t                           *start_addr_ptr;
   int8_t                           *usage_addr_ptr;
   uint32_t                              total_size;
   voice_capi_data_format_struct_t input_media_type;

   voice_capi_module_t  *special_capis[NUM_SPCL_CAPIS];   // pointers of special capi modules
   uint16_t                        hpcm_read_enable;
   uint16_t                       hpcm_write_enable;
} vdec_modules_t;

// Structure for managing VDS related data
typedef struct vdec_vds_info_t
{
   elite_svc_handle_t      *vds_handle_ptr;                // pointer to VDS handle
   uint32_t                vds_client_id;                  // client id returned by VDS
   uint32_t                vds_client_token;               // client token sent to VDS
} vdec_vds_info_t;

typedef struct vdec_t
{
   //vdec framework related
   elite_svc_handle_t      svc_handle;                  // handle to give out to others - first element
   qurt_elite_channel_t    qurt_elite_channel;          // Channel for the queues for main thread
   char_t                  cmd_q_name[NAME_LEN];        // input command queue name
   char_t                  data_q_name[NAME_LEN];       // input data queue name
   char_t                  buf_q_name[NAME_LEN];        // output buffer queue name
   char_t                  gp_q_name[NAME_LEN];         // gpQ data queue name
   char_t                  resp_q_name[NAME_LEN];       // response queue for acks
   qurt_elite_queue_t      *buf_q_ptr;                  // output buffer queue
   qurt_elite_queue_t      *resp_q_ptr;                 // queue for acks
   int32_t                 bufs_allocated;              // Keep track of number of output bufs in circulation
   uint32_t                prev_wait_mask;              // Previous wait mask
   uint32_t                wait_mask;                   // Current wait mask for main thread
   uint32_t                rcvd_mask;                   // Received mask for main thread
   int32_t                 bit_pos;                     // Bit Position
   elite_apr_packet_t      *data_msg_payload_ptr;       // pointer to most recent msg (apr dec pkt) payload popped from dataQ
   int64_t                 in_buf[(VOICE_FRAME_SIZE_FB * sizeof(int16_t)) / sizeof(int64_t)];   // local input buffer
   uint32_t                in_buf_size;                 // input buffer size
   int64_t                 dec_out_buf[3 * VOICE_FRAME_SIZE_FB * sizeof(int16_t)/ sizeof(int64_t)];   // local output buffer for decoder, accounts TW
   uint32_t                dec_out_size_bytes;          // output buffer size in bytes for dec buffer
   uint16_t                loopback_enable;             // flag for internal loopback mode.  (will prevent sending dec pkt requests to modem)

   vdec_pkt_xchg_t         pkt_xchg;
   vdec_timing_t           timing;
   vdec_memory_t           vdec_memory;                 // memory management structure for vdec modules
   vdec_memory_t           vdec_pp_memory;
   vdec_modules_t          modules;
   vdec_vds_info_t         vds;
   uint32_t                session_num;                 // Session Number
   int8_t                  process_data;                // flag indicating the run and running state
   uint32_t                vfr_mode;                    // VFR mode (VFR_NONE = 0,VFR_HARD = 1, VFR_HARD_EXT=2)
   uint32_t                voc_type;                    // vocoder type
   uint8_t                 vfr_source;                  // in terms of AFE enum
   uint16_t                sampling_rate_dec;           // sampling rate of decoder output
   int8_t                  samp_rate_factor;
   uint16_t                frame_samples_dec;           // frame size of decoder output in samples
   int8_t                  reset_voc_flag;              // flag to indicate if vocoder should be reset next Run()
   voice_strm_apr_info_t   *apr_info_ptr;               // info to send/process apr messages with client for voc packet exchange
   voice_strm_tty_state_t  *tty_state_ptr;              // pointer to cmn struct used by tx and rx
   qurt_elite_queue_t      *vtm_cmdq_ptr;               // pointer to  voice timer cmd Q
   voice_cmn_time_profile_t time_profile_struct[2];      // collect timing stats
   uint32_t                 vsid;                        // voice system id for hard vfr timing
//   uint8_t                  ctm_reset_flag;              //flag for CTM TTY init
   voice_bbwe_feature_t     voice_module_bbwe;           // voice_module_bbwe  : determines which BBWE algorithm is enabled
   uint32_t                 pp_out_buf_size;             // output buffer size in bytes for PP buffer
   elite_svc_handle_t       *downstream_peer_ptr;        // this service only supports 1 downstream peer
   int64_t                  pp_out_buf[VOICE_FRAME_SIZE_FB * sizeof(int16_t) / sizeof(int64_t)];   // local output buffer for PP
   uint16_t                 samp_rate;                  // stores sampling rate running after widevoice
   uint8_t                  wv_nb_samp_factor;          // NB sampling rate factor after WV
   int8_t                   *scratch_ptr;               // pointer to scratch buffer
   int8_t                   send_media_type;            // flag to indicate whether to send MediaType downstream
   uint32_t                 session_op_mode;            // Prevailing operating mode of the session ( It is set by a combination of factors including
                                                        // the vocoder and modules such as Slowtalk which operate natively at 16KHz)
   uint32_t                 voc_native_op_mode;         // This is either the default operating mode or the one advertized by the vocoder.
                                                        // Slowtalk can also control the session operating mode. In order to sustain the
                                                        // right operating mode across a Slowtalk toggle, this field needs to be maintained.
                                                        // This is required only for operating modes equal or above WB (for EVS and not eAMR, for example).
   uint32_t                 vocoder_op_detection;       // Enable/disable for vocoder mode event
   uint32_t                 erasure;                    // flag to indicate erasure
   uint32_t                 resync_count;               // keep a track of number of resync
   uint32_t                 vfr_cycle;                  // vfr_cycle added for CDRX optimization
   uint32_t                 vfr_dec_cntr;               // counter for vdec_vfr_decode calls
   vdec_thread_info_t       thread_info;                //qurt thread related info
   bool_t 		            fatal_state;
} vdec_t;

uint16_t vdec_get_sampling_rate( vdec_t *vdec_ptr);
ADSPResult vdec_rec_vfr_end_handler (void* instance_ptr);
void vdec_vtm_subscribe (vdec_t* vdec_ptr, Vtm_SubUnsubMsg_t *data_ptr);
void vdec_vtm_unsubscribe (vdec_t* vdec_ptr, Vtm_SubUnsubMsg_t *data_ptr,uint32_t mask, qurt_elite_channel_t *channel);
ADSPResult vdec_init_pp_circbuf (vdec_t* vdec_ptr);

ADSPResult vdec_beamr_set_param (vdec_t* vdec_ptr, int8_t *cal_payload, uint32_t pid, uint16 param_size);
ADSPResult vdec_beamr_get_param (vdec_t* vdec_ptr, int8_t *cal_payload, uint32_t pid, uint16 buffer_size, uint16_t *param_size);


#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//#ifndef VOICEDECPRIVATE_H

