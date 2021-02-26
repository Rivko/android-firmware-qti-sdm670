/*========================================================================

 *//** @file vdec_svc.cpp

 Copyright (c) 2009-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

 This file contains the code for Voice Decoder (Vdec) Dynamic service. This service
 has two threads. One thread which is the main thread does decode and decoder related
 processing like time-warp etc. Second thread is a worker thread for doing stream
 post-processing. Main thread receives commands from Voice Stream Manager (VSM) and it
 communicates with the worker thread as needed. Vdec initializes the vocoder algorithms
 and processes the data. Multiple Vdec can be instantiated and each of them is identified
 by a session number provided while creating.
  *//*====================================================================== */

/*========================================================================
 Edit History

 $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_dec/src/vdec_svc.cpp#2 $

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 04/22/14   pm    removed vdec pp thread as part of tw module removal task
 8/8/2012   pg     Add worker thread for stream pp to off load main thread for time-warp scheduling
 07/12/10   pg     Major cleanup
 06/03/10   pg     Merge features from main to 8660DevAprV2
 EFR, FR, HR, PCM_NB, 4GV_NB, V13k, G711 A/MLAW, 4GV_WB,ENC-DEC
 pkt loopback
 10/29/09   dc     Created file.
 ========================================================================== */

/* =======================================================================
 INCLUDE FILES FOR MODULE
 ========================================================================== */
#include "vdec_private.h"
#include "adsp_vparams_internal_api.h"
#include "capi_v2_adsp_error_code_converter.h"
#include "capi_v2_library_factory.h"
#include "Elite_fwk_extns_dtmf_detection.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

// Masks for the signals and queues for main thread
#define VDEC_RESYNC_MASK               0x80000000
#define VDEC_REC_BUF_MASK              0x40000000
#define VDEC_DECODE_TICK_MASK          0x20000000
#define VDEC_CMD_MASK                  0x10000000
#define VDEC_BUF_MASK                  0x08000000
#define VDEC_DATA_MASK                 0x04000000
#define VDEC_DECODE_TICK_END_MASK      0x02000000
// no need for handler or to include in any composite masks as this is used for synchronous wait from pp thread
#define VDEC_RESP_MASK                 0x01000000
// not listening actively, no need for handler or to include in any composite masks
#define VDEC_GPQ_DATA_MASK             0x00800000
#define VDEC_VDS_PKT_REQ_TICK_END_MASK 0x00400000

#define VDEC_CMD_TICK_MASK          (VDEC_DECODE_TICK_MASK | \
         VDEC_CMD_MASK |VDEC_RESYNC_MASK)
#define VDEC_ALL_MASK               (VDEC_CMD_TICK_MASK | VDEC_BUF_MASK |\
         VDEC_DECODE_TICK_END_MASK | \
      VDEC_REC_BUF_MASK|VDEC_RESYNC_MASK)

#define VDEC_SS_MULTIFRAME VOICE_SS_MULTI_FRAME_4

#define FR_DEC_FROM_VOICE_PATH         0 // flag to differentiate of unpacking
// of bitstream for FR decoder starting from LSB or MSB

// TODO: Remove when API updated
/**  Parameter for enabling internal loopback feature for a particular session.
  Fixed size of LOOPBACK_ENABLE_PARAM_BYTES = 4 bytes.
 */
#define VOICE_PARAM_LOOPBACK_ENABLE (0x00010E18)

#define RUN_STATE 1
#define STOP_STATE 2

// externs
extern uint32_t vsm_memory_map_client;

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
// How many buffers in output buffer queue? May need to make this configurable...
static const uint32_t MAX_BUF_Q_ELEMENTS = 4;   // this means 3 output buffers available for main
// thread to deliver to worker thread. 3 are needed
// to account TW worst case compression - 8.875ms
// Main Thread name
static char_t aTHREAD_NAME[6] =
{ 'V', 'D', 'C', '$', '\0' };

// Thread stack size
static const uint32_t MIN_THREAD_STACK_SIZE = 24*1024;    //minimum thread stack for vdec
static const uint32_t SERVICE_THREAD_STACK_SIZE = 8*1024; //stack contribution from service



// for logging purposes
#define VDEC_LOG_PKT_SIZE_AMR_NB_MODEM    36
#define VDEC_LOG_PKT_SIZE_AMR_NB_IF2      36
#define VDEC_LOG_PKT_SIZE_EFR_MODEM       36
#define VDEC_LOG_PKT_SIZE_FR_MODEM        36
#define VDEC_LOG_PKT_SIZE_HR_MODEM        36
#define VDEC_LOG_PKT_SIZE_AMR_WB_MODEM    62
#define VDEC_LOG_PKT_SIZE_EVRC_MODEM      22
#define VDEC_LOG_PKT_SIZE_4GV             22
#define VDEC_LOG_PKT_SIZE_13K_MODEM       36
#define VDEC_LOG_PKT_SIZE_G729AB          6

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */
// channel handler functions
static ADSPResult vdec_resync_handler (void* instance_ptr);
static ADSPResult vdec_vfr_decode_handler (void* instance_ptr);
static ADSPResult vdec_cmd_handler (void* instance_ptr);
static ADSPResult vdec_buf_handler (void* instance_ptr);
static ADSPResult vdec_data_handler (void* instance_ptr);

// elite message handler functions
static ADSPResult vdec_custom_msg (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_destroy_yourself_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_connect_dwn_stream_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_disconnect_dwn_stream_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_stop_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_run_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_param_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_get_param_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_media_type (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_apr_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);

// custom message handler functions
static ADSPResult vdec_set_mute_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_timing_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_timingv2_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_timingv3_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_reinit_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_stop_record_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_start_record_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_config_host_pcm (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_register_event (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_unregister_event (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_param_v3 (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_enc_rate (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_voc_param(void* instance_ptr, elite_msg_any_t* msg_ptr);

// Helper functions
static ADSPResult vdec_thread (void* instance_ptr);
static ADSPResult vdec_thread_entry(void* instance_ptr);
static void vdec_destroy (vdec_t* vdec_ptr);
static ADSPResult vdec_process (vdec_t* vdec_ptr);
static ADSPResult vdec_send_dec_pkt_req (vdec_t *vdec_ptr);
static ADSPResult vdec_send_out_buf_downstream (vdec_t* vdec_ptr);
static ADSPResult vdec_init (vdec_t *vdec_ptr);
static ADSPResult vdec_ctrl_params_init (vdec_t *vdec_ptr);
static ADSPResult vdec_process_pkt_data (vdec_t* vdec_ptr);
static ADSPResult vdec_process_oob_pkt_data (vdec_t* vdec_ptr);
static ADSPResult vdec_clear_loopback_gpq (vdec_t *vdec_ptr);
static ADSPResult vdec_read_apr_msg (vdec_t* vdec_ptr, elite_msg_any_t *sMsg);
static ADSPResult vdec_log_vocoder_packet (uint32_t voc_type, uint16_t sampling_rate, uint32_t log_session_id, void *out_ptr, uint16_t size);
void vdec_clear_data_queue (vdec_t *vdec_ptr);
void vdec_vtm_unsubscribe (vdec_t* vdec_ptr, Vtm_SubUnsubMsg_t *data_ptr, uint32_t mask, qurt_elite_channel_t *channel);
void vdec_vtm_subscribe (vdec_t* vdec_ptr, Vtm_SubUnsubMsg_t *data_ptr);
static ADSPResult vdec_allocate_mem (vdec_t* vdec_ptr);
static void vdec_allocate_mem_free (vdec_t* vdec_ptr);
static ADSPResult vdec_set_pp_samp_rate (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_tty_mode (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_ltetty_mode (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_pkt_exchange_mode (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_set_oob_pkt_exchange_config (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_get_kpps_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_aggregate_modules_kpps (void* instance_ptr, uint32_t* kpps_changed);
static ADSPResult vdec_get_delay_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vdec_aggregate_modules_delay (void* instance_ptr);
static ADSPResult vdec_get_vocoder_delay (uint32_t voc_type, uint32_t* delay_ptr);
static ADSPResult vdec_send_mode_notification_v2 (void* instance_ptr);
static ADSPResult vdec_set_param_int (vdec_t* vdec_ptr, uint32_t module_id, uint16_t instance_id, uint32_t param_id, void* payload_address_in, uint32_t param_size);
static ADSPResult vdec_launch_thread(vdec_t* vdec_ptr, bool_t vdec_thread_exists, uint32_t new_stack_size);

//vdec pp related
static ADSPResult vdec_rec_buf_handler (void *instance_ptr);
static ADSPResult vdec_pp_process (vdec_t* vdec_ptr);
static ADSPResult vdec_pp_allocate_mem (vdec_t* vdec_ptr);
static ADSPResult vdec_pp_init (vdec_t* vdec_ptr);
static void vdec_init_rec_circbuf (vdec_t* vdec_ptr);
static void vdec_pp_allocate_mem_free (vdec_t *vdec_ptr);
static ADSPResult vdec_send_mixer_media_type (vdec_t *vdec_ptr);
static ADSPResult vdec_stream_pp_init (vdec_t *vdec_ptr);
static ADSPResult vdec_pp_ctrl_params_init (vdec_t *vdec_ptr);
static void inline vdec_cal_drift (vdec_t *vdec_ptr);
static void vdec_cal_sample_slip_stuff (vdec_t *vdec_ptr, int16_t* slip_stuff_samples);
static int32_t vdec_send_vsm_dtmf_tone_status (vdec_t *vdec_ptr, void* data_ptr);
static ADSPResult vdec_send_afe_media_type (vdec_t *vdec_ptr);
static ADSPResult vdec_send_rec_buf_to_afe (vdec_t *vdec_ptr, qurt_elite_bufmgr_node_t *out_buf_mgr_node_ptr);
static int32_t vdec_send_oobtty_char_accepted (vdec_t *vdec_ptr);

// calibration callback function
static void vdec_calibration_cb_func (void* cal_params_ptr, void* cb_data);

// Capi callback function
capi_v2_err_t vdec_decoder_capi_v2_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr);

// ECALL RX wrapper function
void voice_ecall_rx (vdec_t *vdec_ptr, int16_t *in_out_ptr);

//CAPI helper functions
ADSPResult vdec_decoder_capi_init(vdec_t* vdec_ptr, uint32_t media_type);
ADSPResult vdec_decoder_capi_destroy(vdec_t* vdec_ptr);
ADSPResult vdec_decoder_capi_process(vdec_t* vdec_ptr);
ADSPResult vdec_decoder_capi_set_tty(vdec_t* vdec_ptr, bool_t tty_enable);
ADSPResult vdec_decoder_capi_get_amdb_handle(vdec_t* vdec_ptr, uint32_t media_type);
ADSPResult vdec_stop_start_modules (vdec_t* vdec_ptr, uint32_t state);
ADSPResult vdec_decooder_capi_get_data_threshold(vdec_t* vdec_ptr);
ADSPResult vdec_decoder_capi_set_output_media_fmt(vdec_t *vdec_ptr, uint32_t sampling_rate);
ADSPResult vdec_capi_set_param(voice_capi_module_t *module_info_ptr, uint32_t param_id, void* param_payload_ptr, uint32_t param_size);
ADSPResult vdec_capi_get_param(voice_capi_module_t *module_info_ptr, uint32_t param_id,
                               void* param_payload_ptr, uint32_t buf_size, uint16_t *param_size_ptr);

// Stream capi helper functions
static ADSPResult vdec_stream_pp_capi_create(vdec_t* vdec_ptr);
static ADSPResult vdec_stream_pp_capi_reinit(vdec_t* vdec_ptr);
static ADSPResult vdec_stream_pp_capi_get_amdb_handle(vdec_t* vdec_ptr);
static ADSPResult vdec_stream_pp_capi_get_static_property(vdec_t* vdec_ptr);
static ADSPResult vdec_stream_pp_capi_query_intf_extn(vdec_t* vdec_ptr);
static ADSPResult vdec_stream_pp_capi_init(vdec_t* vdec_ptr);
static ADSPResult vdec_stream_pp_capi_init_lib(vdec_t* vdec_ptr, voice_capi_module_t *module_info_ptr, capi_v2_proplist_t *init_prop_list_ptr);
static ADSPResult vdec_stream_pp_capi_set_input_media_fmt(vdec_t *vdec_ptr);
static ADSPResult vdec_stream_pp_capi_send_algo_reset(vdec_t *vdec_ptr);
static ADSPResult vdec_stream_pp_capi_destroy(vdec_t *vdec_ptr);
static ADSPResult vdec_stream_pp_capi_end(vdec_t *vdec_ptr);
static capi_v2_err_t vdec_stream_pp_capi_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr);

static ADSPResult vdec_log_tty_rx_char(vdec_t *vdec_ptr, uint32_t tty_rx_char);
static ADSPResult vdec_log_ctm_tty_rx_char(vdec_t *vdec_ptr, uint32_t tty_rx_char);
ADSPResult vdec_capi_get_amdb_handle(voice_capi_module_t *module_info_ptr, adsp_amdb_module_handle_info_t *amdb_handle_info_ptr);
boolean vdec_check_stream_pp_capi(uint32_t module_id);
int32_t vdec_send_hpcm_buf_available_evt(vdec_t* vdec_ptr, void* data_ptr);
/* -----------------------------------------------------------------------
 ** Message handlers
 ** -----------------------------------------------------------------------
 */

/* Vdec channel handler */
static ADSPResult (*pVdecHandler[]) (void *) =
{
   vdec_resync_handler,       // handles resync                     - VDEC_RESYNC_MASK
   vdec_rec_buf_handler,      // handles record path                - VDEC_REC_BUF_MASK
   vdec_vfr_decode_handler,   // handles vfr signal for decode      - VDEC_DECODE_TICK_MASK
   vdec_cmd_handler,          // handles cmds                       - VDEC_CMD_MASK
   vdec_buf_handler,          // handles output buffers             - VDEC_BUF_MASK
   vdec_data_handler,         // handles input data buffers         - VDEC_DATA_MASK
};

/* Vdec FADD msg handler */
static elite_svc_msg_handler_func pEliteHandler[] =
{
   vdec_custom_msg,                //0  - ELITE_CUSTOM_MSG
   elite_svc_unsupported,          //1  - ELITE_CMD_START_SERVICE
   vdec_destroy_yourself_cmd,      //2  - ELITE_CMD_DESTROY_SERVICE
   vdec_connect_dwn_stream_cmd,    //3  - ELITE_CMD_CONNECT
   vdec_disconnect_dwn_stream_cmd, //4  - ELITE_CMD_DISCONNECT
   vdec_stop_cmd,                  //5  - ELITE_CMD_PAUSE
   vdec_run_cmd,                   //6  - ELITE_CMD_RESUME
   elite_svc_unsupported,          //7  - ELITE_CMD_FLUSH
   vdec_set_param_cmd,             //8  - ELITE_CMD_SET_PARAM
   vdec_get_param_cmd,             //9  - ELITE_CMD_GET_PARAM
   elite_svc_unsupported,          //10 - ELITE_DATA_BUFFER
   vdec_set_media_type,            //11 - ELITE_DATA_MEDIA_TYPE
   elite_svc_unsupported,          //12 - ELITE_DATA_EOS
   elite_svc_unsupported,          //13 - ELITE_DATA_RAW_BUFFER
   elite_svc_unsupported,          //14 - ELITE_CMD_STOP_SERVICE
   vdec_apr_cmd                    //15 - ELITE_APR_PACKET_OPCODE
};

/* Vdec custom msg handler */
static elite_svc_msg_handler_func pHandler[VDEC_NUM_MSGS] =
{
   vdec_get_kpps_cmd,                // VDEC_GET_KPPS_CMD
   vdec_set_mute_cmd,                // VDEC_SET_MUTE_CMD
   vdec_set_timing_cmd,              // VDEC_SET_TIMING_CMD
   vdec_set_timingv2_cmd,            // VDEC_SET_TIMINGV2_CMD
   vdec_reinit_cmd,                  // VDEC_REINIT_CMD
   vdec_stop_record_cmd,             // VDEC_STOP_RECORD_CMD
   vdec_start_record_cmd,            // VDEC_START_RECORD_CMD_V2
   vdec_config_host_pcm,             // VDEC_CONFIG_HOST_PCM
   vdec_set_timingv3_cmd,            // VDEC_SET_TIMINGV3_CMD
   vdec_get_delay_cmd,               // VDEC_GET_DELAY_CMD
   vdec_set_param_v3,                // VDEC_SET_PARAM_V3
   vdec_set_pp_samp_rate,            // VDEC_SET_STREAM_PP_SAMP_RATE
   vdec_set_tty_mode,                // VDEC_SET_TTY_MODE
   vdec_set_ltetty_mode,             // VDEC_SET_LTETTY_MODE
   vdec_set_pkt_exchange_mode,       // VDEC_SET_PKT_EXCHANGE_MODE
   vdec_set_oob_pkt_exchange_config, // VDEC_SET_OOB_PKT_EXCHANGE_CONFIG
   vdec_register_event,              // VDEC_REGISTER_EVENT
   vdec_unregister_event,            // VDEC_UNREGISTER_EVENT
   vdec_set_enc_rate,                // VDEC_SET_ENC_RATE
   vdec_set_voc_param,               // VDEC_SET_VOC_PARAM
};

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */
static ADSPResult vdec_launch_thread(vdec_t* vdec_ptr, bool_t vdec_thread_exists, uint32_t new_stack_size)
{
   ADSPResult result=ADSP_EOK;

   if( vdec_ptr->thread_info.stack_size != new_stack_size)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_launch_thread(): thread stack size has changes from(%ld) to (%ld). Launching thread. session(%#lx)",
            vdec_ptr->thread_info.stack_size,new_stack_size,vdec_ptr->session_num);

      uint32_t old_stack_size = vdec_ptr->thread_info.stack_size;
      vdec_ptr->thread_info.stack_size = new_stack_size;
      vdec_ptr->thread_info.cached_tid = vdec_ptr->svc_handle.threadId;
      vdec_ptr->thread_info.another_thread_launched = vdec_thread_exists;

      // Launch the thread
      aTHREAD_NAME[3] = ((vdec_ptr->session_num + 48) & 0xff); // int32_t to ascii conversion
      if (ADSP_FAILED(result = qurt_elite_thread_launch(&(vdec_ptr->svc_handle.threadId), aTHREAD_NAME, NULL,
            vdec_ptr->thread_info.stack_size, elite_get_thrd_prio(ELITETHREAD_DYNA_VOICE_DEC_PRIO_ID), vdec_thread_entry, (void*)vdec_ptr, QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: vdec_launch_thread(): Failed create Vdec thread!! session(%#lx)",vdec_ptr->session_num);
         // update thread info and stack size for failure case to prevent older thread from exiting.
         vdec_ptr->thread_info.stack_size = old_stack_size;
         vdec_ptr->thread_info.another_thread_launched = FALSE;
         vdec_ptr->fatal_state = TRUE;
         vdec_decoder_capi_destroy(vdec_ptr);
         return result;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vdec_launch_thread(): No change in thread stack. Not launching thread");
   }
   return result;
}


ADSPResult vdec_create (uint32_t nInputParam,
      voice_strm_apr_info_t *apr_info_ptr,
      voice_strm_tty_state_t *tty_state_ptr, void **ppHandle, uint32_t session_num)
{
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_create enter session(%lx)", session_num);

   // allocate instance struct
   vdec_t *vdec_ptr = (vdec_t*) qurt_elite_memory_malloc (sizeof(vdec_t), QURT_ELITE_HEAP_DEFAULT);
   if (!vdec_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Failed to allocate memory for VDEC struct !!");
      return ADSP_ENOMEMORY;
   }

   // zero out all the fields
   memset (vdec_ptr, 0, sizeof(vdec_t));

   // Initialize instance variables
   vdec_ptr->svc_handle.unSvcId = ELITE_VOICE_DEC_SVCID;
   vdec_ptr->wait_mask = VDEC_CMD_TICK_MASK;
   vdec_ptr->apr_info_ptr = apr_info_ptr;
   vdec_ptr->tty_state_ptr = tty_state_ptr;
   vdec_ptr->session_num = session_num;
   vdec_ptr->modules.apr_info_dtmf.apr_handle = vdec_ptr->apr_info_ptr->apr_handle;
   // initialze qurt_elite channel for main thread. This is required for any destroy to succeed.
   qurt_elite_channel_init(&vdec_ptr->qurt_elite_channel);

   (void) vdec_init (vdec_ptr);

   if (qurt_elite_globalstate.pVoiceTimerCmdQ == NULL)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Failed to get Vtm cmdQ session(%lx)", session_num);
      vdec_destroy (vdec_ptr);
      return ADSP_EUNSUPPORTED;
   }
   vdec_ptr->vtm_cmdq_ptr = qurt_elite_globalstate.pVoiceTimerCmdQ;

   vdec_ptr->vds.vds_handle_ptr = vds_handle;
   vdec_ptr->vds.vds_client_token = VDS_CREATE_TOKEN(vdec_ptr->session_num, VDS_CLIENT_VDEC_PKT_REQUEST);

   result = vdec_allocate_mem (vdec_ptr);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Failed vdec Allocate Memory function (%lx)", session_num);
      vdec_destroy (vdec_ptr);
      return result;
   }

   result = vdec_pp_allocate_mem (vdec_ptr);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Failed vdec pp Allocate Memory function (%lx)", session_num);
      vdec_destroy (vdec_ptr);
      return result;
   }

   // Create stream pp capi modules and intialize.
   result = vdec_stream_pp_capi_create(vdec_ptr);
   if (ADSP_EOK != result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_create: vdec_stream_pp_capi_create failed. result(%ld), session(0x%x)",
            (uint32_t)result, (uint32_t)session_num);
      vdec_destroy(vdec_ptr);
      return result;
   }

   (void) vdec_pp_init (vdec_ptr);

   vdec_init_rec_circbuf (vdec_ptr);

   // make Vdec Q names unique per instance
   snprintf (vdec_ptr->data_q_name, QURT_ELITE_DEFAULT_NAME_LEN, "Vdec_DataQ%2x", (int) session_num);
   snprintf (vdec_ptr->gp_q_name, QURT_ELITE_DEFAULT_NAME_LEN, "Vdec_GpQ%2x", (int) session_num);
   snprintf (vdec_ptr->cmd_q_name, QURT_ELITE_DEFAULT_NAME_LEN, "Vdec_CmdQ%2x", (int) session_num);
   snprintf (vdec_ptr->buf_q_name, QURT_ELITE_DEFAULT_NAME_LEN, "Vdec_BufQ%2x", (int) session_num);
   snprintf (vdec_ptr->resp_q_name, QURT_ELITE_DEFAULT_NAME_LEN, "Vdec_RespQ%2x", (int) session_num);
   snprintf (vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_name,
         QURT_ELITE_DEFAULT_NAME_LEN, "Vdec_Rec_BufQ%2x", (int) session_num);


   // Create the queues. Use non-blocking queues, since pselect is always used.
   // pselect blocks on any non-masked queue to receive, then can do non-blocking checks.
   // DataQ for inputs
   // CmdQ for cmd inputs
   // BufQ for output buffers - no need of a Q, instead use local buffer, todo: need bufQ for enc-dec loopback
   // signal_ptr for 20ms tick
   // signal_end_ptr for handshaking unsubscribe with VoiceTimer

   /* Allocate memory for queues */
   vdec_ptr->svc_handle.dataQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc (QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MAX_DATA_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT);
   vdec_ptr->svc_handle.cmdQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc (QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MAX_CMD_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT);
   vdec_ptr->svc_handle.gpQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc (QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MAX_DATA_GPQ_ELEMENTS), QURT_ELITE_HEAP_DEFAULT);
   vdec_ptr->buf_q_ptr = (qurt_elite_queue_t*) qurt_elite_memory_malloc (QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MAX_CMD_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT);
   vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc (sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_end_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc (sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.resync_signal_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc (sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   vdec_ptr->timing.vtm_sub_unsub_decode_data.resync_signal_ptr = NULL;
   vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr = (qurt_elite_queue_t*) qurt_elite_memory_malloc (QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(PP_MAX_DATA_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT);
   vdec_ptr->resp_q_ptr = (qurt_elite_queue_t*) qurt_elite_memory_malloc (QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(MAX_CMD_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT);

   /* NOTE: This delivery tick signal will be allocated by VDS, don't allocate/free/add to channel here*/
   vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_ptr = NULL;
   vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_end_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);

   if (NULL == vdec_ptr->svc_handle.dataQ
         || NULL == vdec_ptr->svc_handle.cmdQ
         || NULL == vdec_ptr->svc_handle.gpQ
         || NULL == vdec_ptr->buf_q_ptr
         || NULL == vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_ptr
         || NULL == vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_end_ptr
         || NULL == vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_end_ptr
         || NULL == vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr
         || NULL == vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.resync_signal_ptr
         || NULL == vdec_ptr->resp_q_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Failed to allocate memory for Vdec Queues/Signals session(%#lx)", session_num);
      vdec_destroy (vdec_ptr);
      return ADSP_ENOMEMORY;
   }

   if ( ADSP_FAILED(result = qurt_elite_queue_init (vdec_ptr->data_q_name, MAX_DATA_Q_ELEMENTS,vdec_ptr->svc_handle.dataQ))
         || ADSP_FAILED(result = qurt_elite_queue_init(vdec_ptr->cmd_q_name, MAX_CMD_Q_ELEMENTS, vdec_ptr->svc_handle.cmdQ))
         || ADSP_FAILED(result = qurt_elite_queue_init(vdec_ptr->gp_q_name, MAX_DATA_GPQ_ELEMENTS, vdec_ptr->svc_handle.gpQ))
         || ADSP_FAILED(result = qurt_elite_queue_init(vdec_ptr->buf_q_name, MAX_CMD_Q_ELEMENTS, vdec_ptr->buf_q_ptr))
         || ADSP_FAILED(result = qurt_elite_queue_init(vdec_ptr->resp_q_name, MAX_CMD_Q_ELEMENTS, vdec_ptr->resp_q_ptr))
         || ADSP_FAILED(result = qurt_elite_queue_init(vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_name, PP_MAX_DATA_Q_ELEMENTS,vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr))
         || ADSP_FAILED(result = qurt_elite_signal_init(vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_ptr))
         || ADSP_FAILED(result = qurt_elite_signal_init(vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_end_ptr))
         || ADSP_FAILED(result = qurt_elite_signal_init(vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_end_ptr))
         || ADSP_FAILED(result = qurt_elite_signal_init(vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.resync_signal_ptr))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&vdec_ptr->qurt_elite_channel, (vdec_ptr->svc_handle.cmdQ), VDEC_CMD_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&vdec_ptr->qurt_elite_channel, (vdec_ptr->svc_handle.dataQ), VDEC_DATA_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&vdec_ptr->qurt_elite_channel, (vdec_ptr->svc_handle.gpQ), VDEC_GPQ_DATA_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&vdec_ptr->qurt_elite_channel, (vdec_ptr->buf_q_ptr), VDEC_BUF_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&vdec_ptr->qurt_elite_channel, (vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr), VDEC_REC_BUF_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_add_signal(&vdec_ptr->qurt_elite_channel, (vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_ptr), VDEC_DECODE_TICK_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_add_signal(&vdec_ptr->qurt_elite_channel, (vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_end_ptr),VDEC_VDS_PKT_REQ_TICK_END_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_add_signal(&vdec_ptr->qurt_elite_channel, (vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.resync_signal_ptr), VDEC_RESYNC_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_add_signal(&vdec_ptr->qurt_elite_channel, (vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_end_ptr), VDEC_DECODE_TICK_END_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&vdec_ptr->qurt_elite_channel, (vdec_ptr->resp_q_ptr), VDEC_RESP_MASK)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Failed create Vdec MsgQs or Signals!! session(%lx)", session_num);
      vdec_destroy (vdec_ptr);
      return result;
   }

   // Allocate and queue up the output buffers for main thread
   for (uint32_t i = 0; i < MAX_BUF_Q_ELEMENTS - 1; i++)
   {
      //allocate the databuffer payload (metadata + pcm buffer size)
      int32_t req_size = GET_ELITEMSG_DATABUF_REQ_SIZE(BUFFER_SIZE);
      elite_msg_data_buffer_t* data_payload_ptr = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc (req_size, QURT_ELITE_HEAP_DEFAULT);

      if (!data_payload_ptr)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Failed to allocate mem for output buffer in vdec_create, session(%lx)", vdec_ptr->session_num);
         vdec_destroy (vdec_ptr);
         return ADSP_ENOMEMORY;
      }

      data_payload_ptr->nActualSize = BUFFER_SIZE;
      data_payload_ptr->nMaxSize = BUFFER_SIZE;

      vdec_ptr->bufs_allocated++;

      if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq (vdec_ptr->buf_q_ptr, (elite_msg_any_payload_t* ) data_payload_ptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Failed to fill Vdec buf queue session(%lx)", vdec_ptr->session_num);
         qurt_elite_memory_free (data_payload_ptr);
         vdec_destroy (vdec_ptr);
         return result;
      }
   }

   // Allocate and queue up the  recording output buffers
   for (uint32_t i = 0; i < MAX_REC_BUF_Q_ELEMENTS - 1; i++)
   {
      //allocate the databuffer payload (metadata + pcm buffer size)
      int32_t req_size = GET_ELITEMSG_DATABUF_REQ_SIZE(REC_BUFFER_SIZE);
      elite_msg_data_buffer_t* data_payload_ptr = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc (req_size, QURT_ELITE_HEAP_DEFAULT);

      if (!data_payload_ptr)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Out of Memory session(%lx)", vdec_ptr->session_num);
         vdec_destroy (vdec_ptr);
         return ADSP_ENOMEMORY;
      }

      data_payload_ptr->nActualSize = REC_BUFFER_SIZE;
      data_payload_ptr->nMaxSize = REC_BUFFER_SIZE;

      vdec_ptr->modules.vdec_pp_modules.record.rec_bufs_allocated++;
      if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq (vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr, (elite_msg_any_payload_t* ) data_payload_ptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Failed to fill Vdec Rec buf queue session(%lx)", vdec_ptr->session_num);
         qurt_elite_memory_free (data_payload_ptr);
         vdec_destroy (vdec_ptr);
         return result;
      }
   }

   // Launch the main thread
   result = vdec_launch_thread(vdec_ptr,FALSE,MIN_THREAD_STACK_SIZE);
   if(ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to launch vdec thread!! session(%x)",(int)session_num);
      vdec_destroy(vdec_ptr);
      return result;
   }

   *ppHandle = &(vdec_ptr->svc_handle);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_create end %d session(%lx)", result, session_num);
   return ADSP_EOK;
}

// This must not be called when the threads are active
static void vdec_destroy (vdec_t* vdec_ptr)
{
   if (vdec_ptr)
   {
      uint32_t session_num = vdec_ptr->session_num;
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_destroy begin session(%lx)", session_num);

      // free any pending APR packets from modem.
      vdec_clear_data_queue (vdec_ptr);

      // call utility function to destroy data Q
      if (NULL != vdec_ptr->svc_handle.dataQ)
      {
         elite_svc_deinit_data_queue (vdec_ptr->svc_handle.dataQ);
         qurt_elite_memory_free (vdec_ptr->svc_handle.dataQ);
         vdec_ptr->svc_handle.dataQ = NULL;
      }

      // call utility function to destroy resp Q
      if (NULL != vdec_ptr->resp_q_ptr)
      {
         elite_svc_deinit_data_queue (vdec_ptr->resp_q_ptr);
         qurt_elite_memory_free (vdec_ptr->resp_q_ptr);
         vdec_ptr->resp_q_ptr = NULL;
      }

      // call utility function to destroy cmd Q
      if (NULL != vdec_ptr->svc_handle.cmdQ)
      {
         elite_svc_deinit_cmd_queue (vdec_ptr->svc_handle.cmdQ);
         qurt_elite_memory_free (vdec_ptr->svc_handle.cmdQ);
         vdec_ptr->svc_handle.cmdQ = NULL;
      }

      /* free any pending APR packets from vdec in the loopback gpQ */

      if (NULL != vdec_ptr->svc_handle.gpQ)
      {
         vdec_clear_loopback_gpq( vdec_ptr);
         /* now destroy Q */
         qurt_elite_queue_deinit (vdec_ptr->svc_handle.gpQ);
         qurt_elite_memory_free (vdec_ptr->svc_handle.gpQ);
         vdec_ptr->svc_handle.gpQ = NULL;
      }

      // call utility function to destroy buf Q
      if (NULL != vdec_ptr->buf_q_ptr)
      {
         elite_svc_deinit_buf_queue (vdec_ptr->buf_q_ptr, vdec_ptr->bufs_allocated);
         qurt_elite_memory_free (vdec_ptr->buf_q_ptr);
         vdec_ptr->bufs_allocated = 0;
         vdec_ptr->buf_q_ptr = NULL;
      }

      if (NULL != vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_ptr)
      {
         qurt_elite_signal_deinit (vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_ptr);
         qurt_elite_memory_free (vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_ptr);
         vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_ptr = NULL;
      }


      if (NULL != vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_end_ptr)
      {
         qurt_elite_signal_deinit (vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_end_ptr);
         qurt_elite_memory_free (vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_end_ptr);
         vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_end_ptr = NULL;
      }

      if (NULL != vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_end_ptr)
      {
         qurt_elite_signal_deinit (vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_end_ptr);
         qurt_elite_memory_free (vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_end_ptr);
         vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_end_ptr = NULL;
      }

      if (NULL != vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.resync_signal_ptr)
      {
         qurt_elite_signal_deinit (vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.resync_signal_ptr);
         qurt_elite_memory_free (vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.resync_signal_ptr);
         vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.resync_signal_ptr = NULL;
      }

      //record buffer
      if (NULL != vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr)
      {
         elite_svc_deinit_buf_queue (vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr, vdec_ptr->modules.vdec_pp_modules.record.rec_bufs_allocated);
         qurt_elite_memory_free (vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr);
         vdec_ptr->modules.vdec_pp_modules.record.rec_bufs_allocated = 0;
         vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr = NULL;
      }

      vdec_pp_allocate_mem_free (vdec_ptr);

      vdec_stream_pp_capi_destroy(vdec_ptr);

      // Destroy the qurt_elite_channel
      qurt_elite_channel_destroy (&vdec_ptr->qurt_elite_channel);

      // Free any pending packet
      if (NULL != vdec_ptr->data_msg_payload_ptr)
      {
         elite_apr_if_free (vdec_ptr->apr_info_ptr->apr_handle, vdec_ptr->data_msg_payload_ptr);
         vdec_ptr->data_msg_payload_ptr = NULL;
      }

      vdec_allocate_mem_free (vdec_ptr);

      qurt_elite_memory_free (vdec_ptr);

      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_destroy end session(%lx)", session_num);
   }
   vdec_ptr = NULL;
}

static ADSPResult vdec_thread_entry(void* instance_ptr)
{
   ADSPResult result = ADSP_EOK;                           // general result value
   vdec_t *vdec_ptr = (vdec_t*)instance_ptr;    // instance structure
   int join_status;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_thread_entry(): vdec Current context: %d", (int)qurt_thread_get_id());

   if(vdec_ptr->thread_info.another_thread_launched)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_thread_entry(): waiting another thread to join, session(%#lx)", vdec_ptr->session_num);
      // a new thread was launched, so join the old thread first
      qurt_elite_thread_join(vdec_ptr->thread_info.cached_tid, &join_status);
      // Usually it should return immediately, once it returns, reenter the workloop
      //Can safely clear flag once join returns, since now there is only one thread
      vdec_ptr->thread_info.another_thread_launched = FALSE;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_thread_entry(): for Create, session(%lx)",vdec_ptr->session_num);
   }
   // call into workloop
   result = vdec_thread(instance_ptr);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_thread_entry(): exiting, session(%#lx), Current Tid(%d)", vdec_ptr->session_num,qurt_thread_get_id());

   return result;
}


static ADSPResult vdec_thread (void* instance_ptr)
{
   ADSPResult result;
   vdec_t *vdec_ptr = (vdec_t*) instance_ptr;
   uint32_t session_num = vdec_ptr->session_num;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_thread begin session(%lx)", session_num);

   // Enter forever loop
   for (;;)
   {
      // ***************** Wait for the MASK
      // block on any one or more of selected queues and signals
      vdec_ptr->rcvd_mask = qurt_elite_channel_wait (&vdec_ptr->qurt_elite_channel, vdec_ptr->wait_mask);

      while (vdec_ptr->rcvd_mask)
      {
         vdec_ptr->bit_pos = voice_get_signal_pos (vdec_ptr->rcvd_mask);

         // De queue and signal clear done in the handler functions.
         result = pVdecHandler[vdec_ptr->bit_pos] (vdec_ptr);

         if (result != ADSP_EOK)
         {
            if (ADSP_ETERMINATED == result)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_thread end session(%lx)", session_num);
               return ADSP_EOK;
            }
            MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Vdec thread Handler Error bit_pos=%ld, res=%d, session=%ld", vdec_ptr->bit_pos, result, session_num);
         }

         if(vdec_ptr->thread_info.another_thread_launched)
         {
            // the old thread will come here, and exit at this point
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_thread():Another thread launched. exiting current Tid(%ld) session(%lx), cachedTid(%ld)",
                qurt_thread_get_id(),session_num,vdec_ptr->thread_info.cached_tid);
            return ADSP_EOK;
         }

         vdec_ptr->rcvd_mask = qurt_elite_channel_poll (&vdec_ptr->qurt_elite_channel, vdec_ptr->wait_mask);
      }   // end of while loop
   }   // end of for loop
   return 0;
}


static ADSPResult vdec_vfr_decode_handler (void *instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t *vdec_ptr = (vdec_t*) instance_ptr;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_vfr_decode_handler begin session(%lx)", vdec_ptr->session_num);

   if (vdec_ptr->vfr_cycle == ++vdec_ptr->vfr_dec_cntr)
   {
	   qurt_elite_signal_clear (vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_ptr);   // clear the signal
   }

   if (FALSE == vdec_ptr->process_data)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Not in RUN state, this should not happen!! session(%lx)", vdec_ptr->session_num);
      return ADSP_EOK;
   }

   if (IN_BAND == vdec_ptr->pkt_xchg.pkt_exchange_mode)
   {
      vdec_process_pkt_data (vdec_ptr);
   }
   else
   {
      vdec_process_oob_pkt_data (vdec_ptr);
   }
   if ( FALSE == vdec_ptr->loopback_enable)
   {
      if (FALSE == vdec_ptr->pkt_xchg.dec_pkt_ready)
      {
         vdec_ptr->erasure = TRUE;   // if dec pkt didn't arrive, signal erasure
         vdec_ptr->pkt_xchg.pkt_miss_ctr++;   // increment pkt miss counter
         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Vdec Pkt miss count from call start is %ld session(%lx)", vdec_ptr->pkt_xchg.pkt_miss_ctr, vdec_ptr->session_num);
      }
   }
   else
   {
      /* Loopback mode....pull mode -> if a packet is in gpQ, process it.  If not, erasure.
       * perform APR free on any packet pulled off queue, since it was created with APR alloc from Encoder
       */
      elite_msg_any_t in_buf_msg;
      memset (&in_buf_msg, 0, sizeof(elite_msg_any_t));

      // ***************** Pull mode from gpQ
      result = qurt_elite_queue_pop_front (vdec_ptr->svc_handle.gpQ, (uint64_t*) &in_buf_msg);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "VCP: vdec_process gpQ packet not available session(%lx)", vdec_ptr->session_num);

         vdec_ptr->erasure = TRUE;   // if dec pkt didn't arrive, signal erasure
         vdec_ptr->pkt_xchg.pkt_miss_ctr++;   // increment pkt miss counter
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "VCP: vdec_process gpQ packet available session(%lx)", vdec_ptr->session_num);
         vdec_ptr->erasure = FALSE;
         vdec_read_apr_msg (vdec_ptr, &in_buf_msg);
         if (NULL != vdec_ptr->data_msg_payload_ptr)
         {
            vdec_ptr->data_msg_payload_ptr = NULL;
         }

         result = elite_apr_if_free (vdec_ptr->apr_info_ptr->apr_handle, (elite_apr_packet_t *) in_buf_msg.pPayload);
      }
   }
   vdec_ptr->pkt_xchg.dec_pkt_ready = FALSE;   //reset

   //queue next packet request to VDS if previous packet set is already read
   //error msgs are printed inside and return result is ignored because
   //process should happen irrespective of it
   if(vdec_ptr->vfr_cycle == vdec_ptr->vfr_dec_cntr)
   {
   (void) vdec_send_dec_pkt_req (vdec_ptr);
   }

   uint64_t time = qurt_elite_timer_get_time ();
   uint64_t cycles = qurt_elite_profile_get_pcycles ();

   result = vdec_process (vdec_ptr);

   time = qurt_elite_timer_get_time () - time;
   cycles = qurt_elite_profile_get_pcycles () - cycles;

   if (result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_process Error, nothing to deliver session(%lx)", vdec_ptr->session_num);
   }
   else
   {
	  // push the buffer downstream
      result = vdec_send_out_buf_downstream(vdec_ptr);
   }
   voice_cmn_time_profile_add_data (&vdec_ptr->time_profile_struct[VOICE_DEC_PROFILE_DECODER], (uint32_t) time, (uint32_t) cycles);
   if(vdec_ptr->vfr_cycle == vdec_ptr->vfr_dec_cntr)
   {
	   vdec_ptr->vfr_dec_cntr = 0;
   }
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vdec_vfr_decode_handler end session(%lx)",vdec_ptr->session_num);
   return result;
}

static ADSPResult vdec_buf_handler (void *instance_ptr)
{

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vdec_buf_handler begin session(%lx)",vdec_ptr->session_num);

   //this is a dummy function
   //Every Queue in qurt_elite needs to be associated with a channel. So this
   //function is just to meet that requirement. we never wait for this mask, so
   //this function should never be called.
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: This function should never be called. We never wait for buf mask");

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vdec_buf_handler end session(%lx)",vdec_ptr->session_num);
   return ADSP_EOK;
}

static ADSPResult vdec_cmd_handler (void *instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t *vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_any_t msg;
   uint32_t session_num = vdec_ptr->session_num;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_cmd_handler begin session(%lx)", session_num);
   result = qurt_elite_queue_pop_front (vdec_ptr->svc_handle.cmdQ, (uint64_t*) &msg);
   // ***************** Process the msg
   if (ADSP_EOK == result)
   {
      const uint32_t cmdTableSize = sizeof(pEliteHandler) / sizeof(pEliteHandler[0]);
      if (msg.unOpCode >= cmdTableSize)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Unsupported message ID 0x%8lx!! session(%lx)", msg.unOpCode, session_num);
         return ADSP_EUNSUPPORTED;
      }
      // table lookup to call handling function, with FALSE to indicate processing of msg
      if(TRUE == vdec_ptr->fatal_state)
      {
         // in fatal state only accept command for media type or destroy.
         if((2 == msg.unOpCode)||(11 == msg.unOpCode))
      	 {
      	    result = pEliteHandler[msg.unOpCode](instance_ptr, &msg);      	        	 
         }
         else
         {
            result = ADSP_EFAILED;
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vdec in fatal state result=%x session(%lx)",result,session_num);
            return result;
         }
      }
      else
      {
         result = pEliteHandler[msg.unOpCode](instance_ptr, &msg);
      }
   }
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_cmd_handler end result=%d session(%lx)", result, session_num);
   return result;
}

static ADSPResult vdec_custom_msg (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_header_t *pCustom = (elite_msg_custom_header_t *) msg_ptr->pPayload;

   if (pCustom->unSecOpCode < VDEC_NUM_MSGS)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Vdec_Cmd, SecOpcode: %ld session(%lx)", pCustom->unSecOpCode, vdec_ptr->session_num);
      result = pHandler[pCustom->unSecOpCode] (instance_ptr, msg_ptr);
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Unsupported SecOpcode message ID 0x%8lx!! session(%lx)", pCustom->unSecOpCode, vdec_ptr->session_num);
      result = ADSP_EFAILED;
   }
   return result;
}

static ADSPResult vdec_resync_handler (void* instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   qurt_elite_signal_clear (vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.resync_signal_ptr);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec resync, session(%lx)", vdec_ptr->session_num);
   vdec_ptr->resync_count++;

   // keep the new pkt request queued
   (void) vdec_send_dec_pkt_req (vdec_ptr);
   // Vdec itself doesn't need to do anything to handle resync. It just needs to be
   // propagated to Vdec PP which does need to do some handling
   vdec_ptr->modules.vdec_pp_modules.record.ss_info_counter = 0;
   vdec_ptr->modules.vdec_pp_modules.record.ss_multiframe_counter = 0;
   // reset the device drift counters
   vdec_ptr->modules.vdec_pp_modules.record.voice_cmn_drift_info_dec_rec.is_resync_pending = 1;

   return result;

}

static ADSPResult vdec_run_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_run_cmd: begin session(%lx)", vdec_ptr->session_num);

   //KN: TODO: allocate decoder input and output buffer memory dynamically

   // reset stream pp capi modules if,
   // 1. it is a new call
   // 2. vocoder is changed in the same call.
   if (vdec_ptr->reset_voc_flag)
   {
      vdec_stream_pp_capi_send_algo_reset(vdec_ptr);
   }

   if(vdec_ptr->modules.is_decoder_capi)
   {
      //KA: nothing in particular to do here for CAPIs
      vdec_stop_start_modules(vdec_ptr, RUN_STATE);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_run_cmd: invalid media_type(0x%lx)", vdec_ptr->voc_type);
      result = ADSP_EUNSUPPORTED;
      elite_svc_send_ack (msg_ptr, result);
      return result;
   }

   // KN: Following code will not be required once oobtty capi is ready
   /* Enable/Disable LTE TTY for VCO or FULL mode */
   bool_t oobtty_enable = (vdec_ptr->tty_state_ptr->m_oobtty_mode == VSM_TTY_MODE_VCO || vdec_ptr->tty_state_ptr->m_oobtty_mode == VSM_TTY_MODE_FULL);


   // initialize vocoder/tty only when necessary (handover to different media type, or after reinit command)
   // if here,it session is already in STOP state.(RUN or STOP is already is checked in VSM)
   /* handle CDMA TTY/CTM init */
   /* Enable TTY in HCO mode, for CTM negotiation.  Should not effect Rx audio, or 1x TTY */
   bool_t tty_enable = (vdec_ptr->tty_state_ptr->m_etty_mode == VSM_TTY_MODE_VCO || vdec_ptr->tty_state_ptr->m_etty_mode == VSM_TTY_MODE_HCO
         || vdec_ptr->tty_state_ptr->m_etty_mode == VSM_TTY_MODE_FULL);

   if ((tty_enable) && (oobtty_enable))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error ! TTY enabled for both CS and PS mode : session(%lx)", vdec_ptr->session_num);
      elite_svc_send_ack (msg_ptr, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   //VDS returns the signal ptr which is passed to VTM
   result = voice_cmn_send_vds_command (&vdec_ptr->vds.vds_client_id, vdec_ptr->vds.vds_client_token, vdec_ptr->resp_q_ptr, VDS_SUBSCRIBE, &vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_ptr,
         NULL, vdec_ptr->vds.vds_handle_ptr, vdec_ptr->session_num, TRUE,NULL);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Failed Vdec subscribe to VDS session(%lx)!", vdec_ptr->session_num);
   }

   if (NULL == vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_ptr)
   {
      elite_svc_send_ack (msg_ptr, ADSP_EFAILED);
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Vdec subscribe to VDS session(%lx) returned null signal ptr!", vdec_ptr->session_num);
      return ADSP_EFAILED;
   }

   //Subscribe to VTM for decode start tick
   vdec_vtm_subscribe (vdec_ptr, &(vdec_ptr->timing.vtm_sub_unsub_decode_data));
   //Subscribe to VTM for pkt req delivery tick for VDS. The tick will directly go to VDS.
   //For VFR NONE case the delivery tick will be based on one shot timer
   vdec_vtm_subscribe (vdec_ptr, &(vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data));

   //queue first packet request to VDS. Errors are printed inside and return result
   //is ignored because decode will have to happen anyway.
   (void) vdec_send_dec_pkt_req (vdec_ptr);
   vdec_ptr->vfr_dec_cntr = 0;
   // added run cmd contents from pp thread

   //Init and in the case of VFR_NONE pre-fill input circ buf with one frame worth of samples
   //result = vdec_init_pp_circbuf(vdec_ptr);

   vdec_init_rec_circbuf (vdec_ptr);   // initializing recording circular buffer in run cmnd to take of sampling rate change due to BBWE enable

   result = vdec_stream_pp_init (vdec_ptr);
   if (ADSP_FAILED(result))
   {
      elite_svc_send_ack (msg_ptr, result);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: voice_stream_pp_init  failed in run cmnd!!  result(%d) session(%lx) ", result, vdec_ptr->session_num);
      return result;
   }

   //send media type to mixer if sampling rate changed
   if (vdec_get_sampling_rate (vdec_ptr) != vdec_ptr->samp_rate)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Vdec sampling rate change(%d->%d) session(%lx)", vdec_ptr->samp_rate, vdec_get_sampling_rate (vdec_ptr),
            vdec_ptr->session_num);

      vdec_ptr->samp_rate = vdec_get_sampling_rate (vdec_ptr);
   }

   vdec_ptr->wv_nb_samp_factor = (vdec_ptr->samp_rate / VOICE_NB_SAMPLING_RATE);
   // init sample slipping algorithm
   voice_sample_slip_init (&(vdec_ptr->modules.vdec_pp_modules.record.ss_struct), (vdec_ptr->samp_rate_factor * VOICE_FRAME_SIZE_NB_10MS),
         VDEC_SS_MULTIFRAME);

   //set the flag to send media type downstream
   vdec_ptr->send_media_type = TRUE;
   vdec_ptr->modules.vdec_pp_modules.record.send_media_type_rec = TRUE;

   //update sampling rate factor for drift detection
   vdec_ptr->modules.vdec_pp_modules.record.voice_cmn_drift_info_dec_rec.nb_sampling_rate_factor = vdec_ptr->samp_rate_factor;

   //NOTE: reset_voc_flag is used in this thread and also in pp thread before reaching this point.
   //hence setting to false at the end.
   if (vdec_ptr->reset_voc_flag)
   {
      vdec_ptr->reset_voc_flag = FALSE;
   }
   vdec_ptr->process_data = TRUE;   // Start processing - RUN state

   voice_cmn_time_profile_init (&vdec_ptr->time_profile_struct[VOICE_DEC_PROFILE_DECODER]);
   voice_cmn_time_profile_init (&vdec_ptr->time_profile_struct[VOICE_DEC_PROFILE_POST_DEC]);

   vdec_ptr->resync_count=0;
   elite_svc_send_ack (msg_ptr, result);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_run_cmd end result(%d) session(%lx)", result, vdec_ptr->session_num);
   return result;
}

//This command comes in STOP state which means PP thread is also in STOP state.
//Kill pp thread first by sending destroy command and do the remaining stuff here.
static ADSPResult vdec_destroy_yourself_cmd (void *instance_ptr, elite_msg_any_t* msg_ptr)
{

   vdec_destroy ((vdec_t*) instance_ptr);

   // send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
   return ADSP_ETERMINATED;
}


static ADSPResult vdec_set_media_type (void *instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t *vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_data_media_type_header_t* pMediaTypeMsgPayload;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_set_media_type begin session(%lx)", vdec_ptr->session_num);

   /* only handle VocType changes in stop state.  Latch all settings perform init/TTY init/postproc init/postproc
    * sample rate dependent init when transitioning to Run state
    */
   if (TRUE == vdec_ptr->process_data)
   {
      result = ADSP_EBUSY;
      elite_svc_send_ack (msg_ptr, result);
      return result;
   }
   pMediaTypeMsgPayload = (elite_msg_data_media_type_header_t*) msg_ptr->pPayload;

   // set reset flag if we need to reset vocoder.  Latch flag high, only reset when actually processed (Run)
   if (vdec_ptr->voc_type != pMediaTypeMsgPayload->unMediaTypeFormat)
   {
       vdec_ptr->reset_voc_flag = TRUE;

      // KA: destroy existing vocoder CAPI
      if(VSM_MEDIA_TYPE_NONE != vdec_ptr->voc_type)
      {
         if(vdec_ptr->modules.is_decoder_capi)
         {
            vdec_decoder_capi_destroy(vdec_ptr);
         }
         else
         {
            if(NULL != vdec_ptr->modules.decoder.dec_state_ptr)
            {
               qurt_elite_memory_free(vdec_ptr->modules.decoder.dec_state_ptr);
               vdec_ptr->modules.decoder.dec_state_ptr = NULL;
            }
         }
      }

      // KA: Check if the new one is a capi, and if so, initialize it immediately
      vdec_ptr->modules.is_decoder_capi = voice_check_voc_capi(pMediaTypeMsgPayload->unMediaTypeFormat);

      if(vdec_ptr->modules.is_decoder_capi)
      {
         // KA: don't update voctype yet, since vocoder setting may fail, update inside init routine
         result = vdec_decoder_capi_init(vdec_ptr, pMediaTypeMsgPayload->unMediaTypeFormat);
         if(ADSP_FAILED(result))
         {
            vdec_decoder_capi_destroy(vdec_ptr);
            elite_svc_send_ack (msg_ptr, result);
            return result;
         }
         vdec_ptr->sampling_rate_dec = vdec_ptr->modules.dec_capi.output_media_type.data_format.sampling_rate;
         vdec_ptr->samp_rate_factor = (vdec_ptr->sampling_rate_dec / VOICE_NB_SAMPLING_RATE);
         vdec_ptr->frame_samples_dec = VOICE_FRAME_SIZE_NB_20MS * (vdec_ptr->samp_rate_factor);

      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Invalid decoder type: 0x%lx session(%lx)", pMediaTypeMsgPayload->unMediaTypeFormat, vdec_ptr->session_num);
         result = ADSP_EBADPARAM;
         elite_svc_send_ack (msg_ptr, result);
         return result;

      }

      // send input media format to all the stream pp modules if sampling rate is changed.
      if (vdec_ptr->sampling_rate_dec != vdec_ptr->modules.input_media_type.data_format.sampling_rate)
      {
         result = vdec_stream_pp_capi_set_input_media_fmt(vdec_ptr);
         if(ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_set_media_type: failed. result(0x%x), session(0x%lx)",
                  (uint32_t)result, vdec_ptr->session_num);
            elite_svc_send_ack(msg_ptr, result);
            return result;
         }
      }

      //add service thread stack and check if the stack size of encoder is less than minimum
      uint32_t stack_size = SERVICE_THREAD_STACK_SIZE + vdec_ptr->modules.dec_capi.common.stack_size ;
      if(stack_size < MIN_THREAD_STACK_SIZE)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Thread size for encoder_capi(%ld) is less than minimum(%ld). Setting to min value",
               stack_size,MIN_THREAD_STACK_SIZE);
         stack_size = MIN_THREAD_STACK_SIZE;
      }

      result = vdec_launch_thread(vdec_ptr,TRUE,stack_size);
      if(ADSP_FAILED(result))
      {
         elite_svc_send_ack(msg_ptr, result);
         return result;
      }
   }
   else
   {
      vdec_send_mode_notification_v2 (instance_ptr);
   }

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_set_media_type end session(%lx)", vdec_ptr->session_num);
   elite_svc_send_ack (msg_ptr, result);
   return result;

}

void vdec_clear_data_queue (vdec_t *vdec_ptr)
{
   qurt_elite_queue_t* dataq_ptr = vdec_ptr->svc_handle.dataQ;
   elite_msg_any_t dataQMsg;
   ADSPResult result;
   // Clear input DataQ
   if (dataq_ptr)
   {
      do
      {
         // Non-blocking MQ receive
         result = qurt_elite_queue_pop_front (dataq_ptr, (uint64_t*) &dataQMsg);

         // return the buffer to its rightful q.
         if (ADSP_EOK == result)
         {
            vdec_ptr->data_msg_payload_ptr = (elite_apr_packet_t *) dataQMsg.pPayload;
            if (NULL != vdec_ptr->data_msg_payload_ptr)
            {
               elite_apr_if_free (vdec_ptr->apr_info_ptr->apr_handle, vdec_ptr->data_msg_payload_ptr);
               vdec_ptr->data_msg_payload_ptr = NULL;
            }
         }
      }
      while (ADSP_EOK == result);
   }

}

//Need to first STOP the PP thread and then do remaining stuff here.
static ADSPResult vdec_stop_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_stop_cmd begin session(%lx)", vdec_ptr->session_num);

   // add vdec_pp_stop

   (void) vdec_pp_ctrl_params_init (vdec_ptr);

   vdec_ptr->process_data = FALSE;   // Stop processing

   vdec_ptr->wait_mask = VDEC_CMD_TICK_MASK;   // don't listen to output buf

   vdec_vtm_unsubscribe (vdec_ptr, &(vdec_ptr->timing.vtm_sub_unsub_decode_data),
         VDEC_DECODE_TICK_END_MASK, &(vdec_ptr->qurt_elite_channel));
   vdec_vtm_unsubscribe (vdec_ptr, &(vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data),
         VDEC_VDS_PKT_REQ_TICK_END_MASK, &(vdec_ptr->qurt_elite_channel));

   //unsubcribe to VDS only after unsubscribe to VTM
   voice_cmn_send_vds_command (&vdec_ptr->vds.vds_client_id, vdec_ptr->vds.vds_client_token, vdec_ptr->resp_q_ptr, VDS_UNSUBSCRIBE, &vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_ptr, NULL,
         vdec_ptr->vds.vds_handle_ptr, vdec_ptr->session_num, TRUE,NULL);

   vdec_clear_data_queue (vdec_ptr);
   vdec_clear_loopback_gpq (vdec_ptr);

   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Vdec Pkt miss count from call start %ld, total pkts req %ld, session(%lx)", vdec_ptr->pkt_xchg.pkt_miss_ctr, vdec_ptr->pkt_xchg.pkt_req_ctr,
         vdec_ptr->session_num);
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Vdec Pkr req delays: Min = %ld, Max= %ld session(%lx)", vdec_ptr->pkt_xchg.min_pkt_req_delay, vdec_ptr->pkt_xchg.max_pkt_req_delay, vdec_ptr->session_num);
   if (vdec_ptr->pkt_xchg.pkt_exchange_mode == OUT_OF_BAND)
   {
      MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO,
            "VCP: Vdec OOB Pkt miss count from call start %ld, miss due to OOB packet not received in time (%d)," "total pkts req %ld, total packets received %ld session(%lx)",
            vdec_ptr->pkt_xchg.pkt_miss_ctr, (int )vdec_ptr->pkt_xchg.oob_pkt_miss_ctr, vdec_ptr->pkt_xchg.pkt_req_ctr, vdec_ptr->pkt_xchg.oob_pkt_ready_ctr, vdec_ptr->session_num);
   }
   (void) vdec_ctrl_params_init (vdec_ptr);

   // print out profiling stats
   if (vdec_ptr->time_profile_struct[VOICE_DEC_PROFILE_DECODER].num_samples > 0)
   {
      MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_stop session(%lx) voc_type(%x): dec profiler microsec: num_samples(%d) max_time(%d) min_time(%d) avg_time(%d) max_cycles(%d) resync_count(%ld)",
            vdec_ptr->session_num, (int )vdec_ptr->voc_type, (int )vdec_ptr->time_profile_struct[VOICE_DEC_PROFILE_DECODER].num_samples,
            (int )vdec_ptr->time_profile_struct[VOICE_DEC_PROFILE_DECODER].max_time, (int )vdec_ptr->time_profile_struct[VOICE_DEC_PROFILE_DECODER].min_time,
            (int )(vdec_ptr->time_profile_struct[VOICE_DEC_PROFILE_DECODER].total_time / vdec_ptr->time_profile_struct[VOICE_DEC_PROFILE_DECODER].num_samples),
            (int )vdec_ptr->time_profile_struct[VOICE_DEC_PROFILE_DECODER].max_cycles,
             vdec_ptr->resync_count);
   }
   //KA: Tell modules about state change (if we decide to do this)
   vdec_stop_start_modules(vdec_ptr, STOP_STATE);
   // ack back to msg sender
   elite_svc_send_ack (msg_ptr, result);

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_stop_cmd end result(%d) session(%lx)", result, vdec_ptr->session_num);
   return result;
}

static ADSPResult vdec_connect_dwn_stream_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;

   // vdec_pp
   elite_msg_cmd_connect_t* connect_msg_payload_ptr;
   elite_svc_handle_t *svc2Connect_ptr;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_connect_dwn_stream_cmd begin session(%lx)", vdec_ptr->session_num);

   if (vdec_ptr->process_data)
   {
      result = ADSP_ENOTREADY;   // Cannot reconnect in RUN state. todo: needed?
      elite_svc_send_ack (msg_ptr, result);
      return result;
   }

   connect_msg_payload_ptr = (elite_msg_cmd_connect_t*) (msg_ptr->pPayload);
   svc2Connect_ptr = connect_msg_payload_ptr->pSvcHandle;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Vdec connecting to SvcID 0x%8lx session(%lx)", svc2Connect_ptr->unSvcId, vdec_ptr->session_num);

   // This service only allows one downstream
   if (NULL != vdec_ptr->downstream_peer_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Vdec peer connection failed, already connected session(%lx)", vdec_ptr->session_num);
      result = ADSP_EUNSUPPORTED;
   }
   else   //else accept the connection
   {
      vdec_ptr->downstream_peer_ptr = svc2Connect_ptr;
   }
   elite_svc_send_ack (msg_ptr, result);

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_connect_dwn_stream_cmd end result=%d session(%lx)", result, vdec_ptr->session_num);

   return result;
}

static ADSPResult vdec_disconnect_dwn_stream_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;

   elite_msg_cmd_connect_t* disconnect_msg_payload_ptr;
   elite_svc_handle_t *svc2Disconnect_ptr;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_disconnect_dwn_stream_cmd begin session(%lx)", vdec_ptr->session_num);

   disconnect_msg_payload_ptr = (elite_msg_cmd_connect_t*) (msg_ptr->pPayload);
   svc2Disconnect_ptr = disconnect_msg_payload_ptr->pSvcHandle;
   if (svc2Disconnect_ptr != vdec_ptr->downstream_peer_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: FAILED, not connected to that service. session(%lx)", vdec_ptr->session_num);
      result = ADSP_EBADPARAM;
   }
   else   //else accept the dis-connection
   {
      vdec_ptr->downstream_peer_ptr = NULL;
   }

   elite_svc_send_ack (msg_ptr, result);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_disconnect_dwn_stream_cmd end result=%d session(%lx)", result, vdec_ptr->session_num);

   return result;
}

static ADSPResult vdec_set_mute_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;

   elite_msg_custom_voc_set_soft_mute_type *set_mute_cmd_ptr = (elite_msg_custom_voc_set_soft_mute_type *) msg_ptr->pPayload;
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_set_soft_mute begin mute(%d) ramp_duration(%ld) session(%lx)", set_mute_cmd_ptr->mute, set_mute_cmd_ptr->ramp_duration, vdec_ptr->session_num);

   if (set_mute_cmd_ptr->mute > VOICE_MUTE)
   {
      result = ADSP_EBADPARAM;
   }
   else if (vdec_ptr->modules.soft_mute.mute != set_mute_cmd_ptr->mute)
   {
      vdec_ptr->modules.soft_mute.mute = set_mute_cmd_ptr->mute;
      //We cannot initialize the the mute structure because sampling rate is not decided yet.
      //final sampling rate is known only in RUN/vdec_process, so deferring the init until process
      //and copying the cmd payload into local copy.
      vdec_ptr->modules.soft_mute.set_param_cached.mute = set_mute_cmd_ptr->mute;
      vdec_ptr->modules.soft_mute.set_param_cached.ramp_duration = set_mute_cmd_ptr->ramp_duration;
      vdec_ptr->modules.soft_mute.new_mute_cmd = TRUE;
   }

   elite_svc_send_ack (msg_ptr, result);

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_set_soft_mute end result(%d) session(%lx)", result, vdec_ptr->session_num);

   return result;
}

static ADSPResult vdec_get_kpps_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EFAILED;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_kpps_type *kpps_msg_ptr = (elite_msg_custom_kpps_type*) msg_ptr->pPayload;
   vsm_get_kpps_ack_t* kpps_ptr = (vsm_get_kpps_ack_t*) kpps_msg_ptr->pnKpps;
   uint32_t kpps_changed = FALSE;

   if (kpps_ptr)
   {
      kpps_ptr->vdec_kpps = 0;   // Initialization
      result = vdec_aggregate_modules_kpps (instance_ptr, &kpps_changed);

      if (ADSP_SUCCEEDED(result))
      {
         kpps_ptr->vdec_kpps = vdec_ptr->modules.aggregate_kpps;
         kpps_ptr->vdecpp_kpps = 0;
      }
   }
   elite_svc_send_ack (msg_ptr, result);

   return ADSP_EOK;
}

static ADSPResult vdec_get_delay_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EFAILED;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_delay_type *delay_msg_ptr = (elite_msg_custom_delay_type*) msg_ptr->pPayload;
   vsm_get_delay_ack_t* delay_ptr = (vsm_get_delay_ack_t*) delay_msg_ptr->delay_ptr;

   if (delay_ptr)
   {
      delay_ptr->vdec_delay = 0;   // Initialization
      result = vdec_aggregate_modules_delay (instance_ptr);

      if (ADSP_SUCCEEDED(result))
      {
         delay_ptr->vdec_delay = vdec_ptr->modules.aggregate_delay;
      }
   }

   elite_svc_send_ack (msg_ptr, result);

   return ADSP_EOK;
}

//This command comes in STOP state which means PP thread is also in STOP state.
//Hence, no need to forward this cmd to PP thread, safe to process in main thread.
static ADSPResult vdec_set_timing_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_voc_timing_param_type *set_timing_cmd_ptr = (elite_msg_custom_voc_timing_param_type *) msg_ptr->pPayload;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_set_timing_cmd begin  session(%lx)", vdec_ptr->session_num);
   {   //todo: don't allow vfr in run state
      voice_set_timing_params_t* vfr_cmd_ptr = (voice_set_timing_params_t*) set_timing_cmd_ptr->param_data_ptr;
      MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: VOICE_SET_TIMING_PARAMS, dec vfr_mode(%x), dec_req_offset(%x), dec_offset(%x), process_data(%x)", vfr_cmd_ptr->mode, vfr_cmd_ptr->dec_req_offset,
            vfr_cmd_ptr->dec_offset, vdec_ptr->process_data);
      if (FALSE == vdec_ptr->process_data)
      {
         if (VFR_HARD_EXT >= vfr_cmd_ptr->mode)
         {
            vdec_ptr->vfr_mode = vfr_cmd_ptr->mode;
         }
         else
         {
            vdec_ptr->vfr_mode = VFR_NONE;
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: VOICE_SET_TIMING_PARAMS, Invalid dec vfr_mode(%x),setting vfr_mode to VFR_NONE", vfr_cmd_ptr->mode);
            result = ADSP_EBADPARAM;
         }
         // vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.vfr_mode = (uint8_t) vdec_ptr->vfr_mode;
         vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.vfr_mode = (uint8_t) vdec_ptr->vfr_mode;
         vdec_ptr->timing.vtm_sub_unsub_decode_data.vfr_mode = (uint8_t) vdec_ptr->vfr_mode;

         vdec_ptr->vfr_source = voice_cmn_get_vfr_source (vdec_ptr->vfr_mode);

         // MIN_TIMER_OFFSET is set to 0, so no need to check if offset is below min
         if ((MAX_TIMER_OFFSET < vfr_cmd_ptr->dec_req_offset) || (MAX_TIMER_OFFSET < vfr_cmd_ptr->dec_offset))
         {
            //  vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.offset = 3300;   // default 3.3 ms
            vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.offset = 3300;   // default 3.3 ms
            vdec_ptr->timing.vtm_sub_unsub_decode_data.offset = 8300;   // default 8.3ms
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: VOICE_SET_TIMING_PARAMS, Invalid dec_req_offset(%x), dec_offset(%x), Defaulting to 3.3msec, 8.3msec", vfr_cmd_ptr->dec_req_offset,
                  vfr_cmd_ptr->dec_offset);
            result = ADSP_EOK;   // since ISOD/API says we support zero offsets, ack EOK and return EOK.
            // just printing message is good enough.  Changed the MSG to error to hightlight
         }
         else
         {
            //vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.offset = vfr_cmd_ptr->dec_req_offset;   // default 3.3 ms
            vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.offset = vfr_cmd_ptr->dec_req_offset;
            vdec_ptr->timing.vtm_sub_unsub_decode_data.offset = vfr_cmd_ptr->dec_offset;   // default 8.3ms
         }
         //update version of timing used
         //vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.timing_ver =
         //       VFR_CLIENT_INFO_VER_1;
         vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.timing_ver =
            VFR_CLIENT_INFO_VER_1;
         vdec_ptr->timing.vtm_sub_unsub_decode_data.timing_ver = VFR_CLIENT_INFO_VER_1;

         elite_svc_send_ack (msg_ptr, result);

      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Timing can't be changed in RUN session(%lx)", vdec_ptr->session_num);
         result = ADSP_EBUSY;
         elite_svc_send_ack (msg_ptr, result);
      }
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_set_timing_cmd end result(%d) session(%lx)", result, vdec_ptr->session_num);
   return result;
}

//This command comes in STOP state, however for Dec PP offset,
// the command needs to be forwarded to PP thread as well.
static ADSPResult vdec_set_timingv2_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_voc_timing_param_type *set_timing_cmd_ptr = (elite_msg_custom_voc_timing_param_type *) msg_ptr->pPayload;
   vsm_set_timing_params_t* vfr_cmd_ptr = (vsm_set_timing_params_t*) set_timing_cmd_ptr->param_data_ptr;
   MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: VSM_CMD_SET_TIMING_PARAMS Begin, dec vfr_mode(%x), dec_req_offset(%x), dec_offset(%x), process_data(%x)", vfr_cmd_ptr->mode, vfr_cmd_ptr->dec_req_offset,
         vfr_cmd_ptr->dec_offset, vdec_ptr->process_data);

   // Verify stop state of the thread.
   if (FALSE == vdec_ptr->process_data)
   {
      // Verify validity of VFR mode
      if (VFR_HARD_EXT >= vfr_cmd_ptr->mode)
      {
         vdec_ptr->vfr_mode = vfr_cmd_ptr->mode;
      }
      else
      {
         vdec_ptr->vfr_mode = VFR_NONE;
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: VSM_CMD_SET_TIMING_PARAMS, Invalid dec vfr_mode(%x),setting vfr_mode to VFR_NONE", vfr_cmd_ptr->mode);
         result = ADSP_EBADPARAM;
      }

	  vdec_ptr->vfr_cycle = 1;  // default value for 20ms vfr cycle
	  vdec_ptr->timing.vtm_sub_unsub_decode_data.vfr_cycle = MAX_TIMER_OFFSET;
	  vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.vfr_cycle = MAX_TIMER_OFFSET;
      // Update VFR modes everywhere applicable.
      // vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.vfr_mode = (uint8_t) vdec_ptr->vfr_mode;
      vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.vfr_mode = (uint8_t) vdec_ptr->vfr_mode;
      vdec_ptr->timing.vtm_sub_unsub_decode_data.vfr_mode = (uint8_t) vdec_ptr->vfr_mode;

      vdec_ptr->vfr_source = voice_cmn_get_vfr_source (vdec_ptr->vfr_mode);

      // MIN_TIMER_OFFSET is set to 0, so no need to check if offset is below min
      if ((MAX_TIMER_OFFSET < vfr_cmd_ptr->dec_req_offset) || (MAX_TIMER_OFFSET < vfr_cmd_ptr->dec_offset))
      {
         //  vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.offset = 3300;   // default 3.3 ms
         vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.offset = 3300;   // default 3.3 ms
         vdec_ptr->timing.vtm_sub_unsub_decode_data.offset = 8300;   // default 8.3ms
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: VSM_CMD_SET_TIMING_PARAMS, Invalid dec_req_offset(%x), dec_offset(%x), Defaulting to 3.3msec, 8.3msec", vfr_cmd_ptr->dec_req_offset,
               vfr_cmd_ptr->dec_offset);
         result = ADSP_EOK;   // since ISOD/API says we support zero offsets, ack EOK and return EOK.
         // just printing message is good enough.  Changed the MSG to error to hightlight
      }
      else
      {
         // vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.offset = vfr_cmd_ptr->dec_req_offset;   // default 3.3 ms
         vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.offset = vfr_cmd_ptr->dec_req_offset;
         vdec_ptr->timing.vtm_sub_unsub_decode_data.offset = vfr_cmd_ptr->dec_offset;   // default 8.3ms
      }

      //update version of timing used
      //vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.timing_ver =
      //       VFR_CLIENT_INFO_VER_1;
      vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.timing_ver =
         VFR_CLIENT_INFO_VER_1;
      vdec_ptr->timing.vtm_sub_unsub_decode_data.timing_ver = VFR_CLIENT_INFO_VER_1;

      elite_svc_send_ack (msg_ptr, result);

   }

   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: VFR mode can't be changed in RUN session(%lx)", vdec_ptr->session_num);
      result = ADSP_EBUSY;
      elite_svc_send_ack (msg_ptr, result);
   }

   return result;
}

static ADSPResult vdec_set_pkt_exchange_mode (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_pkt_exchange_mode_type *set_pkt_exchange_mode_ptr = (elite_msg_custom_pkt_exchange_mode_type *) msg_ptr->pPayload;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_set_pkt_exchange_mode begin session(%lx)", vdec_ptr->session_num);

   if (FALSE == vdec_ptr->process_data)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: VDEC_SET_PKT_EXCHANGE_MODE ,pkt_exchange_mode(%d) session(%lx)", (int )set_pkt_exchange_mode_ptr->pkt_exchange_mode, vdec_ptr->session_num);
      vdec_ptr->pkt_xchg.pkt_exchange_mode = set_pkt_exchange_mode_ptr->pkt_exchange_mode;
      /* make packet exchange pointer NULL as config command with new addresses must follow a mode command */
      vdec_ptr->pkt_xchg.oob_pkt_exchange_ptr = NULL;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: pkt_exchange_mode mode can't be changed in RUN session(%lx)", vdec_ptr->session_num);
      result = ADSP_EBUSY;
   }
   elite_svc_send_ack (msg_ptr, result);
   return result;

}

static ADSPResult vdec_set_oob_pkt_exchange_config (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_oob_pkt_exchange_config_type *set_oob_pkt_exchange_config_ptr = (elite_msg_custom_oob_pkt_exchange_config_type *) msg_ptr->pPayload;
   vsm_config_packet_exchange_t *pkt_exchange_config_ptr = (vsm_config_packet_exchange_t*) set_oob_pkt_exchange_config_ptr->param_data_ptr;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_set_pkt_exchange_config begin session(%lx)", vdec_ptr->session_num);

   if (TRUE == vdec_ptr->process_data)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: oob_pkt_exchange_config can't be come in RUN session(%lx)", vdec_ptr->session_num);
      result = ADSP_EBUSY;
   }

   else
   {
      vdec_ptr->pkt_xchg.memmap_vdec_oob.unMemMapClient = vsm_memory_map_client;
      vdec_ptr->pkt_xchg.memmap_vdec_oob.unMemMapHandle = pkt_exchange_config_ptr->mem_handle;
      result = elite_mem_map_get_shm_attrib (pkt_exchange_config_ptr->dec_buf_addr_lsw, pkt_exchange_config_ptr->dec_buf_addr_msw, pkt_exchange_config_ptr->dec_buf_size,
            &vdec_ptr->pkt_xchg.memmap_vdec_oob);
      if (ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error! Mapping to virtual address for OOB pkt exchange failed for DECODER %d! session(%lx)", result, vdec_ptr->session_num);
         vdec_ptr->pkt_xchg.oob_pkt_exchange_ptr = NULL;
      }
      else
      {
         vdec_ptr->pkt_xchg.oob_pkt_exchange_ptr = (int32_t *) vdec_ptr->pkt_xchg.memmap_vdec_oob.unVirtAddr;
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: VDEC_SET_OOB_PKT_EXCHANGE_CONFIG ,vdec OOB address(%x) size (%d) session(%lx)", (unsigned int )vdec_ptr->pkt_xchg.oob_pkt_exchange_ptr,
               (int )pkt_exchange_config_ptr->dec_buf_size, vdec_ptr->session_num);
      }
   }
   elite_svc_send_ack (msg_ptr, result);
   return result;

}


static ADSPResult vdec_reinit_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_reinit_cmd begin  session(%lx)", vdec_ptr->session_num);
   {
      if (FALSE == vdec_ptr->process_data)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_reinit_cmd process  session(%lx)", vdec_ptr->session_num);

         // Must reset vocoder on reinit command
          // KA: Not clear why though. media type is set to none, and next time a valid media type is set,
          // the flag will be set to true anyway...
         vdec_ptr->reset_voc_flag = TRUE;

          //destroy decoder CAPI, if one is allocated
          if(vdec_ptr->modules.is_decoder_capi)
          {
            vdec_decoder_capi_destroy(vdec_ptr);
          }

         (void) vdec_init (vdec_ptr);

         //reinit stream pp capi modules
         result = vdec_stream_pp_capi_reinit(vdec_ptr);
         if(ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_reinit:  vdec_stream_pp_capi_reinit: failed. result(0x%ld)", (int)result);
            return result;
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Vdec reinit can't be done in RUN session(%lx)", vdec_ptr->session_num);
         result = ADSP_EBUSY;
      }
   }

   elite_svc_send_ack (msg_ptr, result);

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_reinit_cmd end result(%d) session(%lx)", result, vdec_ptr->session_num);
   return result;
}

static ADSPResult vdec_start_record_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;

   elite_msg_custom_voc_svc_connect_record_type* connect_msg_payload_ptr;
   elite_svc_handle_t *svc_2_connect_ptr;
   connect_msg_payload_ptr = (elite_msg_custom_voc_svc_connect_record_type*) (msg_ptr->pPayload);

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_start_record_cmd opcode(%#lx) session(%#lx)", connect_msg_payload_ptr->sec_opcode, vdec_ptr->session_num);

   svc_2_connect_ptr = connect_msg_payload_ptr->svc_handle_ptr;

   // This service only allows one downstream
   if (NULL != vdec_ptr->modules.vdec_pp_modules.record.afe_handle_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED, only 1 peer allowed session(%lx)", vdec_ptr->session_num);
      result = ADSP_EUNSUPPORTED;
   }
   else   //else accept the connection
   {
      vdec_ptr->modules.vdec_pp_modules.record.afe_handle_ptr = svc_2_connect_ptr;
      vdec_ptr->modules.vdec_pp_modules.record.enable_recording = TRUE;
      vdec_ptr->modules.vdec_pp_modules.record.recording_mode = connect_msg_payload_ptr->record_mode;
      vdec_ptr->modules.vdec_pp_modules.record.rec_first_frame = TRUE;
      vdec_ptr->modules.vdec_pp_modules.record.aud_ref_port = connect_msg_payload_ptr->aud_ref_port;
      vdec_ptr->modules.vdec_pp_modules.record.ss_info_counter = 0;   // reset the sample slip counter
      vdec_ptr->modules.vdec_pp_modules.record.ss_multiframe_counter = 0;   // reset multiframe counter
      vdec_ptr->modules.vdec_pp_modules.record.send_media_type_rec = TRUE;
      // vtm parameters
      vdec_ptr->modules.vdec_pp_modules.record.afe_drift_ptr = connect_msg_payload_ptr->afe_drift_ptr;   // afe drift pointer is stored only at 1 location, hence not in the if condition of vfr_hard

      vdec_init_rec_circbuf (vdec_ptr);

      // reset the hptimer vs device drift counter
      memset (&vdec_ptr->modules.vdec_pp_modules.record.voice_cmn_drift_info_dec_rec, 0, sizeof(vdec_ptr->modules.vdec_pp_modules.record.voice_cmn_drift_info_dec_rec));
      //update sampling rate
      vdec_ptr->modules.vdec_pp_modules.record.voice_cmn_drift_info_dec_rec.nb_sampling_rate_factor = vdec_ptr->samp_rate_factor;
   }

   elite_svc_send_ack (msg_ptr, result);

   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "vdec_start_record_cmd end result=%x session(%lx)",result,vdec_ptr->session_num);
   return result;
}

//This command doesn't have anything for main thread. Hence, forward it to pp thread.
static ADSPResult vdec_stop_record_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;

   elite_msg_custom_voc_svc_connect_type* connect_msg_payload_ptr;
   elite_svc_handle_t *svc_2_connect_ptr;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_stop_record_cmd session(%lx)", vdec_ptr->session_num);

   connect_msg_payload_ptr = (elite_msg_custom_voc_svc_connect_type*) (msg_ptr->pPayload);
   if (VDEC_STOP_RECORD_CMD != connect_msg_payload_ptr->sec_opcode)
   {
      result = ADSP_EBADPARAM;
      elite_svc_send_ack (msg_ptr, result);
      return result;
   }

   svc_2_connect_ptr = NULL;

   // This service only allows one downstream
   vdec_ptr->modules.vdec_pp_modules.record.afe_handle_ptr = svc_2_connect_ptr;
   vdec_ptr->modules.vdec_pp_modules.record.enable_recording = FALSE;
   vdec_ptr->modules.vdec_pp_modules.record.recording_mode = 0;
   voice_circbuf_reset (&(vdec_ptr->modules.vdec_pp_modules.record.circ_struct_decout_rec));

   vdec_ptr->modules.vdec_pp_modules.record.afe_drift_ptr = NULL;

   vdec_ptr->wait_mask &= ~(VDEC_REC_BUF_MASK);   // Once stopped, disable listening to Record buffers

   elite_svc_send_ack (msg_ptr, result);

   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "vdec_stop_record_cmd end result=%x session(%lx)",result,vdec_ptr->session_num);
   return result;
}

static ADSPResult vdec_set_param_int (vdec_t* vdec_ptr, uint32_t module_id, uint16_t instance_id, uint32_t param_id, void* payload_address_in, uint32_t param_size)
{
   ADSPResult result = ADSP_EOK;
   int8_t* calibration_data_pay_load_ptr = (int8_t*) payload_address_in;

   switch (module_id)
   {
      case VOICESTREAM_MODULE_TX:   // listen for Tx module so it's triggered off a single module/param combo.  User sees as putting Tx in loopback
         {
            /* handle loopback param */
            if ( VOICE_PARAM_LOOPBACK_ENABLE == param_id)
            {
               int16_t enable_flag = *((int16_t*) calibration_data_pay_load_ptr);
               if (sizeof(int32_t) != param_size)
               {
                  result = ADSP_EBADPARAM;
               }
               else
               {
                  if ( TRUE == enable_flag)
                  {
                     vdec_ptr->loopback_enable = TRUE;
                     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_set_param_cmd Venc->Vdec Loopback Enable session(%lx)", vdec_ptr->session_num);
                  }
                  else
                  {
                     vdec_ptr->loopback_enable = FALSE;

                     /* if loopback is going off, clear out any queued APR packets and free */
                     vdec_clear_loopback_gpq (vdec_ptr);

                     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_set_param_cmd Venc->Vdec Loopback Disable session(%lx)", vdec_ptr->session_num);
                  }
                  result = ADSP_EOK;
               }
            }
            break;
         }
      case VOICE_MODULE_BEAMR:
         {
            result = vdec_beamr_set_param (vdec_ptr, calibration_data_pay_load_ptr, param_id, param_size);
            break;
         }
      case VOICE_MODULE_DTMF_DETECTION:
         {
            /* not valid to enable DTMF detection via VOICE_CMD_SET_PARAM, must use VOICE_CMD_START_DTMF_DETECT */
            if ( VOICE_PARAM_MOD_ENABLE == param_id)
            {
               result = ADSP_EFAILED;
            }
            else
            {
               result = vdec_capi_set_param (&(vdec_ptr->modules.dtmf_det), param_id, calibration_data_pay_load_ptr, param_size);
            }
            MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_pp_set_param_cmd - DTMF Det mid(%lx) pid(%lx), result=%d session(%lx)", module_id, param_id, result, vdec_ptr->session_num);
            break;
         }
      case VOICE_MODULE_ST:
         {
            result = vdec_capi_set_param (&(vdec_ptr->modules.slow_talk), param_id, calibration_data_pay_load_ptr, param_size);

            if( VSM_MEDIA_TYPE_EVS == vdec_ptr->voc_type )
            {
               // If slowtalk is enabled, ceil to WB operating mode, else use default or vocoder native operating mode.
               if (VSM_VOC_OPERATING_MODE_WB < vdec_ptr->voc_native_op_mode)
               {
                  uint32_t op_mode = vdec_ptr->voc_native_op_mode;

                  if (TRUE == vdec_ptr->modules.slow_talk.is_enabled)
                  {
                     op_mode = VSM_VOC_OPERATING_MODE_WB;
                  }

                  // Now compare to previous setting and notify client only if there's a change.
                  // Also helps when Slowtalk gets disabled on the fly.
                  if(op_mode != vdec_ptr->session_op_mode)
                  {
                     vdec_ptr->session_op_mode = op_mode;
                     vdec_send_mode_notification_v2(vdec_ptr);
                  }
               }
            }
            break;
         }
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Unsupported set dec  module 0x%lx", module_id);
            result = ADSP_EUNSUPPORTED;
            break;
         }
   }
   return result;

}
//set param can have some params used in main thread and some in pp thread.
//params used in main thread, must be handled here.
//params used in pp thread must be handled in pp thread itself.
static ADSPResult vdec_set_param_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_set_param_cmd begin session(%lx)", vdec_ptr->session_num);

   // TODO: should send an ACK back to caller to avoid race conditions, and to inform
   // success/fail.
   if (ELITE_CMD_SET_PARAM == msg_ptr->unOpCode)
   {
      elite_msg_param_cal_t* fadd_payload_ptr = (elite_msg_param_cal_t*) msg_ptr->pPayload;
      if( (ELITEMSG_PARAM_ID_CAL == fadd_payload_ptr->unParamId) ||
            (ELITEMSG_PARAM_ID_CAL_V2 == fadd_payload_ptr->unParamId) )
      {
         uint32_t byte_size_counter = 0;
         int8_t *calibration_data_pay_load_ptr;
         uint32_t nPayloadSize = fadd_payload_ptr->unSize;
         uint32_t payload_address = (uint32_t) fadd_payload_ptr->pnParamData;

         {
            ADSPResult local_result = ADSP_EUNSUPPORTED;  //initialize to unsupported in case no valid module_id is found

            uint32_t module_id, param_id;
            uint16_t instance_id, param_size,header_size;

            // Iterate through the entire payload size and copy all updated parameters
            do
            {

               //update module information based on cal version
               if(ELITEMSG_PARAM_ID_CAL == fadd_payload_ptr->unParamId)
               {
                  voice_param_data_t *param_data_payload_ptr = (voice_param_data_t *)payload_address;
                  module_id = param_data_payload_ptr->module_id;
                  instance_id = 0;
                  param_id = param_data_payload_ptr->param_id;
                  param_size = param_data_payload_ptr->param_size;
                  header_size = sizeof(voice_param_data_t);
               }
               else
               {
                  voice_param_data_v2_t *param_data_payload_ptr = (voice_param_data_v2_t *)payload_address;
                  module_id = param_data_payload_ptr->module_id;
                  instance_id = param_data_payload_ptr->instance_id;
                  param_id = param_data_payload_ptr->param_id;
                  param_size = param_data_payload_ptr->param_size;
                  header_size = sizeof(voice_param_data_v2_t);
               }

               calibration_data_pay_load_ptr = (int8_t*) payload_address + header_size;

               // check to make sure size is a multiple of 4. If not, stop calibrating
               if ((param_size & 0x00000003L) != 0)
               {
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error! Vdec main received bad param size, mod %ld, param %ld, size %ld!!",
                        module_id, (int )param_id, (int )param_size);
                  local_result = ADSP_EBADPARAM;
                  result = ADSP_EOK;
                  break;
               }

               local_result = vdec_set_param_int (vdec_ptr, module_id, instance_id, param_id, calibration_data_pay_load_ptr, param_size);
               if (!((ADSP_EOK == local_result) || (ADSP_EUNSUPPORTED == local_result)))
               {
                  MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "vdec_set_param_cmd(): critical error(%lx) returned by module(%#lx) instance(%d) for pid(%#lx)",
                        local_result, module_id, instance_id, param_id);
                  break;   // in case of any error dont go forward with parsing
               }

               byte_size_counter += (header_size + param_size);
               payload_address += (header_size + param_size);
            }
            while (byte_size_counter < nPayloadSize);

            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "vdec_set_param_cmd(): setparam result(%lx) session(%lx)", local_result, vdec_ptr->session_num);
            elite_svc_send_ack (msg_ptr, local_result);
         }
      }
      else
      {
         elite_svc_send_ack (msg_ptr, ADSP_EFAILED);
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid param id (%lx) session(%lx)",fadd_payload_ptr->unParamId,vdec_ptr->session_num);
      }
   }
   else
   {
      result = ADSP_EFAILED;
      elite_svc_send_ack (msg_ptr, result);
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_set_param_cmd secondary opcode not supported session(%lx)", vdec_ptr->session_num);

   }
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "vdec_set_param_cmd end result=%d session(%lx)", result, vdec_ptr->session_num);
   return ADSP_EOK;
}

//This command doesn't have anything for main thread. Hence, forward it to pp thread.
static ADSPResult vdec_get_param_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   ADSPResult result = ADSP_EOK;
   uint32_t module_id, param_id,*param_size_ptr;
   uint16_t instance_id,header_size = 0;
   elite_msg_param_cal_t* fadd_payload_ptr = (elite_msg_param_cal_t*) msg_ptr->pPayload;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_get_param_cmd begin session(%lx)", vdec_ptr->session_num);

   if (ELITE_CMD_GET_PARAM == msg_ptr->unOpCode)
   {
      ADSPResult local_result = ADSP_EUNSUPPORTED;

      if( (ELITEMSG_PARAM_ID_CAL == fadd_payload_ptr->unParamId) ||
            (ELITEMSG_PARAM_ID_CAL_V2 == fadd_payload_ptr->unParamId))
      {
         //update module information based on cal version
         if (ELITEMSG_PARAM_ID_CAL == fadd_payload_ptr->unParamId)
         {
            voice_param_data_t *param_data_payload_ptr = (voice_param_data_t *)fadd_payload_ptr->pnParamData;
            module_id = param_data_payload_ptr->module_id;
            param_size_ptr = (uint32_t *)&param_data_payload_ptr->param_size;
            instance_id = 0;
            param_id = param_data_payload_ptr->param_id;
            header_size = sizeof(voice_param_data_t);
         }
         else
         {
            voice_param_data_v2_t *param_data_payload_ptr = (voice_param_data_v2_t *)fadd_payload_ptr->pnParamData;
            module_id = param_data_payload_ptr->module_id;
            instance_id = param_data_payload_ptr->instance_id;
            param_size_ptr = &param_data_payload_ptr->param_size;
            param_id = param_data_payload_ptr->param_id;
            header_size = sizeof(voice_param_data_v2_t);
         }

         uint32_t param_size_max = *param_size_ptr - header_size;

         int8_t* calibration_data_pay_load_ptr = (int8_t*) (((int8_t*) fadd_payload_ptr->pnParamData) + header_size);

         {

            // Iterate through the entire payload size and copy all updated parameters
            switch (module_id)
            {
               case VOICE_MODULE_BEAMR:
               {
                  local_result = vdec_beamr_get_param (vdec_ptr, calibration_data_pay_load_ptr, param_id, param_size_max, (uint16_t *)param_size_ptr);
                  break;
               }
               case VOICE_MODULE_ST:
               {
                  local_result = vdec_capi_get_param(&(vdec_ptr->modules.slow_talk), param_id,
                        calibration_data_pay_load_ptr,param_size_max,
                        (uint16_t *)param_size_ptr);
                  MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: GetParam ST param ID(%lx), sessions(%lx)", param_id, vdec_ptr->session_num);
                  break;
               }
               case VOICE_MODULE_DTMF_DETECTION:
               {
                  local_result = vdec_capi_get_param(&(vdec_ptr->modules.dtmf_det), param_id,
                        calibration_data_pay_load_ptr,param_size_max,
                        (uint16_t *)param_size_ptr);
                  MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: GetParam DTMF DET param ID(%lx), sessions(%lx)", param_id, vdec_ptr->session_num);
                  break;
               }
               default:
               {
                  MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Unsupported Vdec module");
                  local_result = ADSP_EUNSUPPORTED;
                  break;
               }
            }

            MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Vdec get param, mod id(0x%lx), instance(%d), param id (0x%lx), result(0x%x) session(0x%lx)",
                  module_id, instance_id, param_id, local_result, vdec_ptr->session_num);
            result = local_result;
         }   // Local scope
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_get_param_cmd : bad fadd id,  result(0x%x) session(0x%lx)\n", ADSP_EFAILED, vdec_ptr->session_num);
         result = ADSP_EFAILED;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_get_param_cmd secondary opcode not supported :(%lx)\n", vdec_ptr->session_num);
      result = ADSP_EFAILED;
   }

   if (ADSP_SUCCEEDED(result))
   {
      fadd_payload_ptr->unSize = *param_size_ptr;
   }
   else
   {
      fadd_payload_ptr->unSize = 0;
   }

   elite_svc_send_ack (msg_ptr, result);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_get_param_cmd end handler result(0x%x) session(0x%lx)", result, vdec_ptr->session_num);
   return ADSP_EOK;
}

static ADSPResult vdec_clear_loopback_gpq (vdec_t *vdec_ptr)
{
   elite_msg_any_t dataQMsg;
   ADSPResult result_q_pop, result_apr;

   do
   {
      // Non-blocking MQ receive
      result_q_pop = qurt_elite_queue_pop_front (vdec_ptr->svc_handle.gpQ, (uint64_t*) &dataQMsg);
      if (ADSP_EOK == result_q_pop)
      {
         result_apr = elite_apr_if_free (vdec_ptr->apr_info_ptr->apr_handle, (elite_apr_packet_t *) dataQMsg.pPayload);
         if (ADSP_FAILED(result_apr))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: ERROR! Elite APR message handler has returned error %d, continuing...", result_apr);
         }
      }
   }
   while (ADSP_EOK == result_q_pop);

   return ADSP_EOK;
}

/* Read APR vocoder packet into local buffer */
static ADSPResult vdec_read_apr_msg (vdec_t* vdec_ptr, elite_msg_any_t *sMsg)
{
   uint32_t nPayloadSize, dec_pkt_size;
   vsm_send_dec_packet_t *pDecPktStruct = NULL;
   int8_t* pDecPkt;
   vdec_ptr->data_msg_payload_ptr = (elite_apr_packet_t *) sMsg->pPayload;
   nPayloadSize = elite_apr_if_get_payload_size (vdec_ptr->data_msg_payload_ptr);

   if (nPayloadSize < sizeof(vsm_send_dec_packet_t))
   { /* The payload is too small to be valid so throw the packet away. */
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: APR pkt payload too small, erasure session(%lx)", vdec_ptr->session_num);
      vdec_ptr->erasure = TRUE;   //signal erasure
   }
   else
   {
      pDecPktStruct = (vsm_send_dec_packet_t *) elite_apr_if_get_payload_ptr (vdec_ptr->data_msg_payload_ptr);
      pDecPkt = (int8_t *) APR_PTR_END_OF(pDecPktStruct, sizeof(vsm_send_dec_packet_t));
      dec_pkt_size = (nPayloadSize - sizeof(vsm_send_dec_packet_t));
      if (0 == dec_pkt_size || (sizeof(vdec_ptr->in_buf) < dec_pkt_size))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Dec pkt size is 0, erasure session(%lx)", vdec_ptr->session_num);
         vdec_ptr->erasure = TRUE;   // signal erasure
      }
      else if (vdec_ptr->voc_type != pDecPktStruct->media_type)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: MediaType mismatch, erasure session(%lx)", vdec_ptr->session_num);
         vdec_ptr->erasure = TRUE;   // signal erasure
      }
      else   //copy dec pkt into local buffer
      {
         vdec_ptr->in_buf_size = dec_pkt_size;
         memsmove (&(vdec_ptr->in_buf[0]), sizeof(vdec_ptr->in_buf), pDecPkt, dec_pkt_size);
         //dbg: MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Copy Data to local buf, bytes(%d) session(%lx)",dec_pkt_size,vdec_ptr->session_num);
      }
   }

   return ADSP_EOK;
}

static ADSPResult vdec_data_handler (void *instance_ptr)
{
   //this is a dummy function
   //Every Queue in qurt_elite needs to be associated with a channel. So this
   //function is just to meet that requirement. we never wait for this mask, so
   //this function should never be called.
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: This function should never be called. We never wait for data mask");

   return ADSP_EOK;
}

static ADSPResult vdec_process_oob_pkt_data (vdec_t* vdec_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_ptr->erasure = FALSE;     // reset  erasure flag
   vsm_oob_pkt_exchange_header_t *pDecPktStruct = NULL;
   int8_t* pDecPkt;

   if ( FALSE == vdec_ptr->loopback_enable)
   {
      if (vdec_ptr->pkt_xchg.oob_pkt_exchange_ptr == NULL)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: ERROR ! OOB packet exchange pointer to shared memory for DECODER is NULL, session(%x)", (int )vdec_ptr->session_num);
         result = ADSP_EFAILED;
         vdec_ptr->erasure = TRUE;   // signal erasure

      }
      else if (vdec_ptr->pkt_xchg.dec_pkt_oob_ready == TRUE)
      {
         //invalidate the cache for read operation
         result = elite_mem_invalidate_cache (&vdec_ptr->pkt_xchg.memmap_vdec_oob);
         // set dec_pkt_oob_ready to FALSE
         vdec_ptr->pkt_xchg.dec_pkt_oob_ready = FALSE;
         pDecPktStruct = (vsm_oob_pkt_exchange_header_t *) vdec_ptr->pkt_xchg.oob_pkt_exchange_ptr;
         pDecPkt = (int8_t *) APR_PTR_END_OF(pDecPktStruct, sizeof(vsm_oob_pkt_exchange_header_t));
         if (vdec_ptr->voc_type != pDecPktStruct->media_type)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: MediaType mismatch for OOB packet exchange, erasure session(%lx)", vdec_ptr->session_num);
            vdec_ptr->erasure = TRUE;   // signal erasure
         }
         else if (0 == pDecPktStruct->size || (sizeof(vdec_ptr->in_buf) < pDecPktStruct->size))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Dec pkt size is 0, erasure session(%lx)", vdec_ptr->session_num);
            vdec_ptr->erasure = TRUE;   // signal erasure
         }
         else
         {
            vdec_ptr->pkt_xchg.dec_pkt_ready = TRUE;
            vdec_ptr->in_buf_size = pDecPktStruct->size;
            memsmove (&(vdec_ptr->in_buf[0]), sizeof(vdec_ptr->in_buf), pDecPkt, pDecPktStruct->size);
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: OOB Dec pkt size received (%d),  session(%lx)", (int )vdec_ptr->in_buf_size, vdec_ptr->session_num);
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: OOB Dec pkt not received in time , erasure session(%lx)", vdec_ptr->session_num);
         vdec_ptr->pkt_xchg.oob_pkt_miss_ctr++;
         vdec_ptr->erasure = TRUE;
      }
   }
   return result;
}

static ADSPResult vdec_process_pkt_data (vdec_t* vdec_ptr)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_any_t in_buf_msg;

   vdec_ptr->erasure = FALSE;     // reset  erasure flag
   if (vdec_ptr->pkt_xchg.dec_pkt_ready)
   {
      vdec_ptr->pkt_xchg.dec_pkt_not_consumed++;
   }

   // ***************** Pop Input buffer
   memset (&in_buf_msg, 0, sizeof(elite_msg_any_t));

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vdec_process_pkt_data begin session(%lx)",vdec_ptr->session_num); //TODO: not needed

   // ***************** Read the input data
   result = qurt_elite_queue_pop_front (vdec_ptr->svc_handle.dataQ, (uint64_t*) &in_buf_msg);
   if (ADSP_EOK != result)
   {
      if (ADSP_ENEEDMORE == result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Vdec pkt not available for decoding, result %d, session(%lx)", result, vdec_ptr->session_num);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Vdec pkt queue pop operation error result %d, session(%lx)", result, vdec_ptr->session_num);
      }
      return result;
   }

   vdec_ptr->pkt_xchg.dec_pkt_ready = TRUE;   // flag to indicate data buffer arrived

   // ***************** Check Media Type
   /* Media type is not valid in Vdec */

   // ***************** Copy data to local buffer
   // If RUN state and if data payload is valid & if data is apr pkt
   if ((vdec_ptr->process_data) && (NULL != in_buf_msg.pPayload) && (ELITE_APR_PACKET == in_buf_msg.unOpCode))
   {
      if ( FALSE == vdec_ptr->loopback_enable)
      {
         vdec_read_apr_msg (vdec_ptr, &in_buf_msg);

         // reset data_msg_payload_ptr.  TODO: is this variable necessary?
         if (NULL != vdec_ptr->data_msg_payload_ptr)
         {
            vdec_ptr->data_msg_payload_ptr = NULL;
         }
      }

      /* run state and APR packet, need to free the APR packet (loopback or not) */
      elite_apr_if_free (vdec_ptr->apr_info_ptr->apr_handle, (elite_apr_packet_t *) in_buf_msg.pPayload);

   }
   else
   {
      if (FALSE == vdec_ptr->process_data)
      {
         /* if not in run state, still need to free the APR packet */
         elite_apr_if_free (vdec_ptr->apr_info_ptr->apr_handle, (elite_apr_packet_t *) in_buf_msg.pPayload);

         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: STOP state, throw away dec pkt  session(%lx)", vdec_ptr->session_num);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Invalid APR pkt session(%lx)", vdec_ptr->session_num);
         vdec_ptr->erasure = TRUE;   //signal erasure
      }
   }

   // todo: change mask, take care of PS call

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vdec_process_pkt_data end session(%lx)",vdec_ptr->session_num); //TODO: not needed
   return result;
}

static ADSPResult vdec_process (vdec_t* vdec_ptr)
{
   ADSPResult result = ADSP_EOK;
   int16_t *out_ptr = (int16_t*) &(vdec_ptr->dec_out_buf[0]);   // pointer to local output buffer
   //MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_process begin");

   if (vdec_ptr->erasure)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Erasure detected in vdec_process: (%ld)\n", vdec_ptr->session_num);
   }


   result = vdec_decoder_capi_process(vdec_ptr);

   if(ADSP_EOK != result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_process: vdec_decoder_capi_process: failed for media_type(0x%lx), result(%ld)",
            vdec_ptr->modules.dec_capi.common.module_id, result);
      return result;
   }

   /* Host PCM processing */
   /* This point is chosen so that client gets pure e-call data before any other stream PP */
   voice_ecall_rx (vdec_ptr, out_ptr);

   //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_process end supported session(%lx)", vdec_ptr->session_num);

   result = vdec_pp_process(vdec_ptr);

   if(ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_process: vdec_pp_process: failed with result(%ld)", result);
   }
   return result;
}

/* utility function to get sampling rate from voc media type */
/* TODO: Use common function */
uint16_t vdec_get_sampling_rate (vdec_t *vdec_ptr)
{
   uint16_t sampling_rate;
   sampling_rate = vdec_ptr->sampling_rate_dec;

   // now update with bbwe status also
   // some of this might need to be revisited later, but for now, this is ok
   if ((sampling_rate == VOICE_NB_SAMPLING_RATE) && ((VOICE_BBWE_WV_V2 == vdec_ptr->voice_module_bbwe) || (VOICE_BBWE_WV_V1 == vdec_ptr->voice_module_bbwe)))
   {
      sampling_rate = VOICE_WB_SAMPLING_RATE;
   }
   return sampling_rate;
}


/*
   static void voice_result_check(ADSPResult result, uint32_t session_num)
   {
   if (ADSP_EOK != result)
   {
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vdec operation error result %d, session(%lx)",result,session_num);
   }
   }
 */
static ADSPResult vdec_init (vdec_t *vdec_ptr)
{
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_init begin session(%lx)", vdec_ptr->session_num);

   // Reset control code params
   (void) vdec_ctrl_params_init (vdec_ptr);

   // Reset params for a new call

   // Fill payload struct for sub/unsub with vtm for pkt req tick
   vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.offset = 3300;   // 3.3 ms
   vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.signal_enable = 1;
   vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.client_id = VOICE_DEC_VDS_PKT_REQ;
   vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.vfr_mode = VFR_NONE;
   vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.vsid = 0;
   vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.timing_ver =
      VFR_CLIENT_INFO_VER_1;
   // Fill payload struct for sub/unsub with vtm for decode tick
   vdec_ptr->timing.vtm_sub_unsub_decode_data.offset = 8300;   // 8.3ms
   vdec_ptr->timing.vtm_sub_unsub_decode_data.signal_enable = 1;
   vdec_ptr->timing.vtm_sub_unsub_decode_data.client_id = VOICE_DEC;
   vdec_ptr->timing.vtm_sub_unsub_decode_data.vfr_mode = VFR_NONE;
   vdec_ptr->timing.vtm_sub_unsub_decode_data.vsid = 0;
   vdec_ptr->timing.vtm_sub_unsub_decode_data.timing_ver = VFR_CLIENT_INFO_VER_1;

   vdec_ptr->pkt_xchg.pkt_exchange_mode = IN_BAND;
   vdec_ptr->pkt_xchg.oob_pkt_exchange_ptr = NULL;
   vdec_ptr->pkt_xchg.dec_pkt_oob_ready = FALSE;
   vdec_ptr->pkt_xchg.max_pkt_req_delay = 0;        //in usec
   vdec_ptr->pkt_xchg.min_pkt_req_delay = 0x7fffffffL;        //in usec
   vdec_ptr->pkt_xchg.pkt_req_ctr = 0;
   vdec_ptr->pkt_xchg.pkt_miss_ctr = 0;
   vdec_ptr->pkt_xchg.oob_pkt_miss_ctr = 0;
   vdec_ptr->pkt_xchg.oob_pkt_ready_ctr = 0;
   vdec_ptr->voc_type = VSM_MEDIA_TYPE_NONE;
   vdec_ptr->voice_module_bbwe = VOICE_BBWE_NONE;   //clear BBWE flag in (RE)INIT
   vdec_ptr->vfr_mode = VFR_NONE;
   vdec_ptr->loopback_enable = FALSE;      // reset on init/reinit.
   vdec_ptr->sampling_rate_dec = VOICE_NB_SAMPLING_RATE;
   vdec_ptr->frame_samples_dec = VOICE_FRAME_SIZE_NB;
   vdec_ptr->samp_rate_factor = 1;
   vdec_ptr->vsid = 0;
   // default mode is none
   vdec_ptr->session_op_mode = VSM_VOC_OPERATING_MODE_NONE;
   vdec_ptr->voc_native_op_mode = VSM_VOC_OPERATING_MODE_NONE;

   vdec_ptr->modules.vdec_pp_modules.record.enable_recording = FALSE;   // should not be reset in init to support handover cases
   vdec_ptr->modules.vdec_pp_modules.record.recording_mode = 0;   // init with 0
   vdec_ptr->modules.vdec_pp_modules.record.afe_drift_ptr = NULL;
   vdec_ptr->modules.vdec_pp_modules.record.ss_info_counter = 0;
   vdec_ptr->modules.vdec_pp_modules.record.aud_ref_port = 0;

   //KA : No vocoder level handling here, since media type is anyway reset to NONE

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "vdec_init end session(%lx)", vdec_ptr->session_num);
   return result;
}

// Reset control code params, should be called for a new call and in STOP state
static ADSPResult vdec_ctrl_params_init (vdec_t *vdec_ptr)
{
   ADSPResult result = ADSP_EOK;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vdec_ctrl_params_init begin session(%lx)",vdec_ptr->session_num);

   vdec_ptr->in_buf_size = 0;
   vdec_ptr->dec_out_size_bytes = 0;
   vdec_ptr->pkt_xchg.dec_pkt_ready = FALSE;
   vdec_ptr->pkt_xchg.dec_pkt_not_consumed = 0;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vdec_ctrl_params_init end session(%lx)",vdec_ptr->session_num);
   return result;
}

static ADSPResult vdec_send_dec_pkt_req (vdec_t *vdec_ptr)
{
   int32_t rc;
   elite_apr_packet_t* packet = NULL;
   ADSPResult result = ADSP_EOK;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vdec_send_dec_pkt_req begin session(%x)",(int)vdec_ptr->session_num);

   if (vdec_ptr->pkt_xchg.pkt_exchange_mode == IN_BAND)
   {
     rc = elite_apr_if_alloc_event( vdec_ptr->apr_info_ptr->apr_handle,
                                    vdec_ptr->apr_info_ptr->self_addr, vdec_ptr->apr_info_ptr->self_port,
                                    vdec_ptr->apr_info_ptr->self_addr, vdec_ptr->apr_info_ptr->self_port,
                                    vdec_ptr->pkt_xchg.pkt_req_ctr++, VSM_EVT_REQ_DEC_PACKET, 0, &packet );
   }
   else
   {
     rc = elite_apr_if_alloc_event( vdec_ptr->apr_info_ptr->apr_handle,
                                    vdec_ptr->apr_info_ptr->self_addr, vdec_ptr->apr_info_ptr->self_port,
                                    vdec_ptr->apr_info_ptr->self_addr, vdec_ptr->apr_info_ptr->self_port,
                                    vdec_ptr->pkt_xchg.pkt_req_ctr++, VSM_EVT_OOB_DEC_BUF_REQUEST, 0, &packet );
   }

   if (rc || (NULL == packet))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Failed to create APR packet for pkt request, res(0x%8lx), packet exchange mode(%d), session(%x)", rc, (int )vdec_ptr->pkt_xchg.pkt_exchange_mode,
            (int )vdec_ptr->session_num);
      return ADSP_EFAILED;
   }
   vds_deliver_apr_packet_t delivery_param;
   delivery_param.client_id = vdec_ptr->vds.vds_client_id;
   delivery_param.client_token = vdec_ptr->vds.vds_client_token;
   delivery_param.apr_handle_ptr = &vdec_ptr->apr_info_ptr->apr_handle;
   delivery_param.apr_packet_ptr = packet;
   delivery_param.delivery_timestamp = 0;
   result = voice_cmn_send_vds_apr_request ( vdec_ptr->vds.vds_handle_ptr, &delivery_param, vdec_ptr->session_num);
   if (ADSP_FAILED(result))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Failed to send APR packet to VDS for pkt req result(%d), pkt exchange mode(%ld), session(%lx)", (unsigned int )result,
            vdec_ptr->pkt_xchg.pkt_exchange_mode, vdec_ptr->session_num);
      elite_apr_if_free (vdec_ptr->apr_info_ptr->apr_handle, packet);
      return result;
   }

   return result;
}

static ADSPResult vdec_send_out_buf_downstream (vdec_t* vdec_ptr)
{
   ADSPResult result = ADSP_EOK;
   qurt_elite_bufmgr_node_t out_buf_mgr_node;

   if (TRUE == vdec_ptr->send_media_type)
   {
      result = vdec_send_mixer_media_type (vdec_ptr);
   }
   // Take next buffer off the Q
   result = qurt_elite_queue_pop_front (vdec_ptr->buf_q_ptr, (uint64_t*) &out_buf_mgr_node);
   if (ADSP_EOK != result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error reading bufq , result(%d) session(%lx)", result, vdec_ptr->session_num);
      return result;
   }
   elite_msg_any_t* peer_data_Q_msg_ptr;
   elite_msg_data_buffer_t* out_data_payload_ptr = (elite_msg_data_buffer_t*) out_buf_mgr_node.pBuffer;
   out_data_payload_ptr->pBufferReturnQ = (vdec_ptr->buf_q_ptr);
   out_data_payload_ptr->nOffset = 0;
   out_data_payload_ptr->nActualSize = vdec_ptr->pp_out_buf_size;

   // Read one Frame from output data
   memscpy (&(out_data_payload_ptr->nDataBuf), out_data_payload_ptr->nMaxSize, vdec_ptr->pp_out_buf, out_data_payload_ptr->nActualSize);

   // send output buf to downstream service
   peer_data_Q_msg_ptr = elite_msg_convt_buf_node_to_msg (&out_buf_mgr_node,
         ELITE_DATA_BUFFER,
         NULL, /* do not need response */
         vdec_ptr->vfr_dec_cntr, /* token */
         0 /* do not care response result*/
         );
   // todo: check for downstreampeer null
   result = qurt_elite_queue_push_back (vdec_ptr->downstream_peer_ptr->dataQ, (uint64_t*) peer_data_Q_msg_ptr);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Failed to deliver buffer dowstream. Dropping session(%lx)", vdec_ptr->session_num);
      (void) elite_msg_push_payload_to_returnq (vdec_ptr->buf_q_ptr, (elite_msg_any_payload_t*)out_buf_mgr_node.pBuffer);
   }

   return result;
}


static ADSPResult vdec_apr_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = APR_EOK;
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   elite_apr_packet_t * pAprPacket;
   int32_t rc = ADSP_EOK;
   vdec_t *vdec_ptr = (vdec_t*) instance_ptr;

   assert(msg_ptr);

   pAprPacket = (elite_apr_packet_t*) msg_ptr->pPayload;

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: VoiceDecApr Message handler Invoked, (Op,Token,Session):(%lx,%lx,%lx)", elite_apr_if_get_opcode (pAprPacket), elite_apr_if_get_client_token (pAprPacket),
         vdec_ptr->session_num);

   /* parse out the received packet.  Note in this current framework we are not prioritizing commands
    * that can be completed immediately.  We are simply processing commands in the order they are received */

   switch (elite_apr_if_get_opcode (pAprPacket))
   {

      //KA: route to dtmf detection capi based on fwk extension
      case VSM_CMD_SET_RX_DTMF_DETECTION:
         {
            vsm_set_rx_dtmf_detection_t *pDtmfDetect = (vsm_set_rx_dtmf_detection_t *) elite_apr_if_get_payload_ptr (pAprPacket);
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VSM_CMD_SET_RX_DTMF_DETECTION, enable(%ld), session(%lx)", pDtmfDetect->enable, vdec_ptr->session_num);

            {
               uint32_t enable = pDtmfDetect->enable;
               if(VSM_RX_DTMF_DETECTION_ENABLE == pDtmfDetect->enable)
               {
                  if( NULL != vdec_ptr->modules.apr_info_dtmf.client_addr ||
                        NULL != vdec_ptr->modules.apr_info_dtmf.client_port)
                  {
                     /* already have a client, should not happen */
                     rc = elite_apr_if_end_cmd(vdec_ptr->apr_info_ptr->apr_handle, pAprPacket, APR_ENOTREADY);
                     break;
                  }
                  else
                  {
                     /* save self/client info for DTMF status updates.  Client is source address from incoming packet */
                     vdec_ptr->modules.apr_info_dtmf.self_addr = elite_apr_if_get_dst_addr(
                           pAprPacket);
                     vdec_ptr->modules.apr_info_dtmf.self_port = elite_apr_if_get_dst_port(
                           pAprPacket);
                     vdec_ptr->modules.apr_info_dtmf.client_addr = elite_apr_if_get_src_addr(
                           pAprPacket);
                     vdec_ptr->modules.apr_info_dtmf.client_port = elite_apr_if_get_src_port(
                           pAprPacket);

                     MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,
                           "VCP: VSM_CMD_SET_RX_DTMF_DETECTION, saving clientAddr(%x) clientPort(%x) :session(%lx)",
                           vdec_ptr->modules.apr_info_dtmf.client_addr, vdec_ptr->modules.apr_info_dtmf.client_port, vdec_ptr->session_num);
                  }
               }
               else
               {
                  /* disabling DTMF, reset aprInfo */
                  vdec_ptr->modules.apr_info_dtmf.self_addr =
                        NULL;
                  vdec_ptr->modules.apr_info_dtmf.self_port =
                        NULL;
                  vdec_ptr->modules.apr_info_dtmf.client_addr =
                        NULL;
                  vdec_ptr->modules.apr_info_dtmf.client_port =
                        NULL;
               }

               // check if DTMF module is stubbed
               if(vdec_ptr->modules.dtmf_det.is_virtual_stub)
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                        "VCP: DTMF detection Module is virtually stubbed session(%lx)",vdec_ptr->session_num);
                  rc = ADSP_EOK;
               }
               else if(vdec_ptr->modules.dtmf_det.module_ptr)
               {
                  capi_v2_buf_t param_data_buf;
                  capi_v2_port_info_t port_info;
                  port_info.is_valid = FALSE;
                  param_data_buf.data_ptr = (int8_t *)&enable;
                  param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(enable);

                  rc = vdec_ptr->modules.dtmf_det.module_ptr->vtbl_ptr->set_param(
                        vdec_ptr->modules.dtmf_det.module_ptr,
                        VOICE_PARAM_MOD_ENABLE,
                        &port_info, &param_data_buf);
                  rc = capi_v2_err_to_adsp_result(rc);
               }
               else
               {
                   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: DTMF detection Module ptr is NULL session(%lx)",vdec_ptr->session_num);
                   rc = ADSP_EBADPARAM;
               }

               if(ADSP_FAILED(rc))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Set Param failed for DTMF Det Enable/Disable !!");
                  rc = elite_apr_if_end_cmd(vdec_ptr->apr_info_ptr->apr_handle, pAprPacket, APR_EFAILED);
               }
               else
               {
                  rc = elite_apr_if_end_cmd(vdec_ptr->apr_info_ptr->apr_handle, pAprPacket, APR_EOK);
               }
            }
            break;
         }
      //KA: route to volte_tty capi
      case VSM_EVT_OUTOFBAND_TTY_RX_CHAR_PUSH:
         {
            vsm_evt_outofband_tty_rx_char_push_t *pLTETTYRx = (vsm_evt_outofband_tty_rx_char_push_t *) elite_apr_if_get_payload_ptr (pAprPacket);
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: VSM_EVT_OUTOFBAND_TTY_RX_CHAR_PUSH, tty_char(%d), session(%lx)", pLTETTYRx->tty_char, vdec_ptr->session_num);
            uint32_t client_token = elite_apr_if_get_client_token (pAprPacket);
            /* Check if previous char push is done */

            tty_char_t ltetty_char;
            ltetty_char.tty_char = (uint32_t)pLTETTYRx->tty_char;

            capi_result = vdec_capi_set_param(&vdec_ptr->modules.ltetty_rx, TTY_PARAM_CHAR, &ltetty_char, sizeof(ltetty_char));

            if(CAPI_V2_EOK == capi_result)
            {
               vdec_ptr->modules.vdec_pp_modules.oobtty.oobtty_rx_char = pLTETTYRx->tty_char;
               vdec_ptr->modules.vdec_pp_modules.oobtty.oobtty_client_token = client_token;
            }
            else
            {
               rc = elite_apr_if_alloc_send_ack_result (vdec_ptr->apr_info_ptr->apr_handle, vdec_ptr->apr_info_ptr->self_addr, vdec_ptr->apr_info_ptr->self_port, vdec_ptr->apr_info_ptr->client_addr,
                     vdec_ptr->apr_info_ptr->client_port, client_token,
                     elite_apr_if_get_opcode (pAprPacket), ADSP_EFAILED);
            }
            // free the event since not done internally
            rc = elite_apr_if_free (vdec_ptr->apr_info_ptr->apr_handle, pAprPacket);
            break;
         }
      case VOICE_EVT_PUSH_HOST_BUF_V2:
         {
            ADSPResult local_result = APR_EOK;

            voice_evt_push_host_pcm_buf_v2_t *push_host_buf_ptr = (voice_evt_push_host_pcm_buf_v2_t *) elite_apr_if_get_payload_ptr(pAprPacket);
            // send PUSH_HOST_BUF event to the HPCM CAPI module
            if(vdec_ptr->modules.special_capis[HPCM]->module_ptr)
            {
               capi_v2_buf_t param_data_buf;
               capi_v2_port_info_t port_info;
               port_info.is_valid = FALSE;
               param_data_buf.data_ptr = (int8_t *) push_host_buf_ptr;
               param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(fwk_extn_param_id_hpcm_push_host_buf_t);
               local_result = vdec_ptr->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(vdec_ptr->modules.special_capis[HPCM]->module_ptr,
                     FWK_EXTN_PARAM_ID_HPCM_PUSH_HOST_BUF, &port_info, &param_data_buf);
               local_result = capi_v2_err_to_adsp_result(local_result);
            }
            if (ADSP_FAILED(local_result))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vdec_apr_cmd() - FWK_EXTN_PARAM_ID_HPCM_PUSH_HOST_BUF set failed, result(%#x) session(%#lx)", \
                     local_result,vdec_ptr->session_num);
            }

            // free the event since not done internally
            rc = elite_apr_if_free (vdec_ptr->modules.apr_info_hpcm.apr_handle, pAprPacket);
            break;
         }
      case VSM_EVT_OOB_DEC_BUF_READY:
         {
            // set flag to indicate OOB pkt is ready
            if (vdec_ptr->pkt_xchg.dec_pkt_oob_ready == TRUE)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Previous OOB pkt not yet read by Decoder, and hence OVERWRITTEN (%lx)", vdec_ptr->session_num);
            }
            vdec_ptr->pkt_xchg.dec_pkt_oob_ready = TRUE;
            vdec_ptr->pkt_xchg.oob_pkt_ready_ctr++;
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: VDEC : VSM_EVT_OOB_DEC_BUF_READY received number (%d) session(%lx)", (int )vdec_ptr->pkt_xchg.oob_pkt_ready_ctr, vdec_ptr->session_num);
            // free the event since not done internally
            rc = elite_apr_if_free (vdec_ptr->apr_info_ptr->apr_handle, pAprPacket);
            break;

         }
      default:
         {
            /* Handle error. */
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Unsupported APR command.session (%lx)", vdec_ptr->session_num);
            if (elite_apr_if_msg_type_is_cmd (pAprPacket))
            { /* Complete unsupported commands. */
               rc = elite_apr_if_end_cmd (vdec_ptr->apr_info_ptr->apr_handle, pAprPacket,
                     APR_EUNSUPPORTED);
            }
            else
            { /* Drop unsupported events. */
               rc = elite_apr_if_free (vdec_ptr->apr_info_ptr->apr_handle, pAprPacket);
            }
            result = ADSP_EUNSUPPORTED;
            break;
         }
   }
   if (ADSP_FAILED(rc))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: ERROR! Elite APR message handler has returned error %d, continuing...", (int )rc);
   }
   return result;
}

static ADSPResult vdec_log_vocoder_packet (uint32_t voc_type, uint16_t sampling_rate, uint32_t log_session_id, void *voc_packet, uint16_t size)
{

#if defined(__qdsp6__) && !defined(SIM)
   int8_t *bufptr[4] =
   {  (int8_t *) voc_packet, NULL, NULL, NULL};
   // log saved input buf and preproc output prior to encode
   voice_log_buffer( bufptr,
         VOICE_LOG_CHAN_PKT_RX_STREAM,
         log_session_id,
         qurt_elite_timer_get_time(),
         VOICE_LOG_DATA_FORMAT_PCM_MONO,
         sampling_rate,
         size,
         &voc_type);// custom data, to send media type info
#endif
   return ADSP_EOK;
}

void vdec_vtm_subscribe (vdec_t* vdec_ptr, Vtm_SubUnsubMsg_t *data_ptr)
{
   ADSPResult result = ADSP_EFAILED;
   //if(VFR_HARD == vdec_cmn_ptr->vfr_mode || VFR_HARD_EXT == vdec_cmn_ptr->vfr_mode)
   {
      if (ADSP_FAILED(result = voice_custom_vt_sub_unsub_msg_send(NULL,vdec_ptr->vtm_cmdq_ptr,VOICE_TIMER_SUBSCRIBE,NULL,FALSE,data_ptr)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Failed Vdec subscribe with vtm memory(%p) Vtm session(%#lx) ", data_ptr, vdec_ptr->session_num);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Vdec subscribed with vtm memory(%p) Vtm session(%#lx) ", data_ptr, vdec_ptr->session_num);
      }
   }
}

void vdec_vtm_unsubscribe (vdec_t* vdec_ptr, Vtm_SubUnsubMsg_t *data_ptr, uint32_t mask, qurt_elite_channel_t *channel)
{
   ADSPResult result = ADSP_EFAILED;
   //if(VFR_HARD == vdec_cmn_ptr->vfr_mode || VFR_HARD_EXT == vdec_cmn_ptr->vfr_mode)
   {
      if (ADSP_FAILED(result = voice_custom_vt_sub_unsub_msg_send(NULL,vdec_ptr->vtm_cmdq_ptr,VOICE_TIMER_UNSUBSCRIBE,NULL,FALSE,data_ptr)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Failed Vdec unsubscribe with vtm memory(%p) Vtm session(%#lx) ", data_ptr, vdec_ptr->session_num);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Vdec unsubscribed with vtm memory(%p) Vtm session(%#lx) ", data_ptr, vdec_ptr->session_num);
      }

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Vdec waiting for unsubscribe to finish - vtm memory(%p) Vtm session(%#lx) ", data_ptr, vdec_ptr->session_num);
      (void) qurt_elite_channel_wait (channel, mask);
      qurt_elite_signal_clear (data_ptr->signal_end_ptr);
   }
}


static ADSPResult vdec_config_host_pcm (void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   ADSPResult apr_pkt_result = APR_EOK;
   vdec_t* vdec_ptr = (vdec_t*) pInstance;
   elite_msg_custom_voc_config_host_pcm_type *pConfig = (elite_msg_custom_voc_config_host_pcm_type *) pMsg->pPayload;
   elite_apr_packet_t *apr_packet_ptr = pConfig->apr_packet_ptr;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_config_host_pcm begin session(%x)\n", (int )vdec_ptr->session_num);

   if ( (VOICE_CMD_START_HOST_PCM_V2 == elite_apr_if_get_opcode (apr_packet_ptr))
         && ((VOICE_NB_SAMPLING_RATE == vdec_ptr->sampling_rate_dec) || (VOICE_WB_SAMPLING_RATE == vdec_ptr->sampling_rate_dec) ||
                                                      (VOICE_SWB_SAMPLING_RATE == vdec_ptr->sampling_rate_dec) || (VOICE_FB_SAMPLING_RATE == vdec_ptr->sampling_rate_dec)))
   {
      voice_start_host_pcm_v2_t *start_host_pcm_ptr = (voice_start_host_pcm_v2_t *) elite_apr_if_get_payload_ptr (apr_packet_ptr);

      uint8 index;

      // check if default Rx stream tap point in the list
      for (index = 0; index < start_host_pcm_ptr->num_tap_points; index++)
      {
         if ( VOICESTREAM_MODULE_RX == start_host_pcm_ptr->tap_points[index].tap_point)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_config_host_pcm: tap point being enabled is (%lx), session(%x)\n", (uint32_t)VOICESTREAM_MODULE_RX, (int )vdec_ptr->session_num);
            break;
         }
      }

      // check if valid index found.  TODO: check if tap point already enabled
      if (index < start_host_pcm_ptr->num_tap_points)
      {

         voice_tap_point_v2_t *voice_tap_point_ptr = &start_host_pcm_ptr->tap_points[index];

         if (NULL == voice_tap_point_ptr->mem_map_handle)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error! Vdec received NULL mem map handle!");
            apr_pkt_result = APR_EBADPARAM;
         }
         else
         {
            /* copy Host Pcm APR info */
            vdec_ptr->modules.apr_info_hpcm.apr_handle        = pConfig->apr_handle;
            vdec_ptr->modules.apr_info_hpcm.self_addr         = elite_apr_if_get_dst_addr(apr_packet_ptr);
            vdec_ptr->modules.apr_info_hpcm.self_port         = elite_apr_if_get_dst_port(apr_packet_ptr);
            vdec_ptr->modules.apr_info_hpcm.client_addr       = elite_apr_if_get_src_addr(apr_packet_ptr);
            vdec_ptr->modules.apr_info_hpcm.client_port       = elite_apr_if_get_src_port(apr_packet_ptr);
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vdec setting hpcm context, memmmap handle (%x), client addr (%x), client port (%x) direction (%x)", 
			        (unsigned int)voice_tap_point_ptr->mem_map_handle, 
					vdec_ptr->modules.apr_info_hpcm.client_addr, 
					vdec_ptr->modules.apr_info_hpcm.client_port,
					voice_tap_point_ptr->direction);

            if(voice_tap_point_ptr->direction & VOICE_HOST_PCM_READ)
            {
              vdec_ptr->modules.hpcm_read_enable = TRUE;
            }
            else
            {
              vdec_ptr->modules.hpcm_read_enable = FALSE;
            }

            if(voice_tap_point_ptr->direction & VOICE_HOST_PCM_WRITE)
            {
              vdec_ptr->modules.hpcm_write_enable = TRUE;
            }
            else
            {
              vdec_ptr->modules.hpcm_write_enable = FALSE;
            }

            // send HPCM start to HPCM CAPI module
            if(vdec_ptr->modules.special_capis[HPCM]->module_ptr)
            {
               capi_v2_buf_t param_data_buf;
               capi_v2_port_info_t port_info;
               port_info.is_valid = FALSE;
               param_data_buf.data_ptr = (int8_t *) voice_tap_point_ptr;
               param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(voice_tap_point_v2_t);
               nResult = vdec_ptr->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(vdec_ptr->modules.special_capis[HPCM]->module_ptr,
                     FWK_EXTN_PARAM_ID_HPCM_START, &port_info, &param_data_buf);
               nResult = capi_v2_err_to_adsp_result(nResult);
               if (ADSP_FAILED(nResult))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! HPCM_START set param to CAPI failed!!");
               }
               else
               {
                  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: HPCM_START set param to CAPI successful!!");
               }
            }
            else
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vdec_config_host_pcm() failed due to NULL pointer error");
               nResult = ADSP_EBADPARAM;
            }
         }
      }   // end of if valid index found
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_config_host_pcm START command NOP - tappoint not found or already enabled, session(%lx)\n", vdec_ptr->session_num);
      }
   }
   else if ( VOICE_CMD_STOP_HOST_PCM == elite_apr_if_get_opcode (apr_packet_ptr))
   {
      vdec_ptr->modules.hpcm_read_enable = 0;  //disable read
      vdec_ptr->modules.hpcm_write_enable = 0; //disable write
      // send HPCM stop to HPCM CAPI module
      if(vdec_ptr->modules.special_capis[HPCM]->module_ptr)
      {
         capi_v2_buf_t param_data_buf;
         capi_v2_port_info_t port_info;
         port_info.is_valid = FALSE;
         param_data_buf.data_ptr = NULL; // no payload is needed
         param_data_buf.actual_data_len = param_data_buf.max_data_len = 0;
         nResult = vdec_ptr->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(vdec_ptr->modules.special_capis[HPCM]->module_ptr,
               FWK_EXTN_PARAM_ID_HPCM_STOP, &port_info, &param_data_buf);
         nResult = capi_v2_err_to_adsp_result(nResult);
         if(ADSP_FAILED(nResult))
         {
             MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! HPCM_STOP set param to CAPI failed!!");
         }
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vdec_config_host_pcm() failed due to NULL pointer error");
         nResult = ADSP_EBADPARAM;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_config_host_pcm failed - opcode or sampling rate unsupported, session(%lx)\n", vdec_ptr->session_num);
      apr_pkt_result = APR_EUNSUPPORTED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_config_host_pcm end apr pkt result(%d) session(%x)\n", apr_pkt_result, (int )vdec_ptr->session_num);

   elite_svc_send_ack (pMsg, apr_pkt_result);

   return nResult;
}

// KA: Modified this function to not allocate decoder instance memory at vdec_create, since
// this will end up using double memory. Instead that memory is allocated for non-capi decoders at run
static ADSPResult vdec_allocate_mem (vdec_t * vdec_ptr)
{
   ADSPResult result = ADSP_EOK;

   //no memory is allocated
   vdec_ptr->vdec_memory.start_addr_ptr = NULL;
   vdec_ptr->vdec_memory.size = 0;
   vdec_ptr->vdec_memory.usage_addr_ptr = NULL;

   return result;
}

static void vdec_allocate_mem_free (vdec_t* vdec_ptr)
{
   if(vdec_ptr->modules.is_decoder_capi)
   {
      vdec_decoder_capi_destroy(vdec_ptr);
   }
   else if(NULL != vdec_ptr->modules.decoder.dec_state_ptr)
   {
      qurt_elite_memory_free(vdec_ptr->modules.decoder.dec_state_ptr);
      vdec_ptr->modules.decoder.dec_state_ptr = NULL;
   }

}

static ADSPResult vdec_set_pp_samp_rate (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_voc_stream_set_sampling_type *set_samp_rate_cmd_ptr = (elite_msg_custom_voc_stream_set_sampling_type *) msg_ptr->pPayload;

   if (vdec_ptr->modules.is_decoder_capi)
   {
      result =  vdec_decoder_capi_set_output_media_fmt(vdec_ptr, set_samp_rate_cmd_ptr->rx_samp_rate);
      if(ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP:vdec_set_pp_samp_rate: failed with result(%ld)", result);
         elite_svc_send_ack (msg_ptr, result);
         return result;
      }
   }
   // send input media format to all the stream pp modules if sampling rate is changed.
   if (vdec_ptr->sampling_rate_dec != vdec_ptr->modules.input_media_type.data_format.sampling_rate)
   {
      result = vdec_stream_pp_capi_set_input_media_fmt(vdec_ptr);
      if(ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_set_pp_samp_rate: vdec_stream_pp_capi_set_input_media_fmt: failed. result(0x%x), session(0x%lx)",
               (uint32_t)result, vdec_ptr->session_num);
         elite_svc_send_ack(msg_ptr, result);
         return result;
      }
   }
   elite_svc_send_ack (msg_ptr, result);
   return result;
}

static ADSPResult vdec_set_tty_mode (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_voc_stream_set_tty_mode *set_tty_mode_cmd_ptr = (elite_msg_custom_voc_stream_set_tty_mode *) msg_ptr->pPayload;

   // send tty mode to ctm rx capi always.
   tty_param_mode_t tty_mode;
   tty_mode.mode = set_tty_mode_cmd_ptr->tty_mode;

   switch (vdec_ptr->voc_type)
   {
      case VSM_MEDIA_TYPE_AMR_WB_MODEM:
      case VSM_MEDIA_TYPE_AMR_NB_MODEM:
      case VSM_MEDIA_TYPE_AMR_NB_IF2:
      case VSM_MEDIA_TYPE_EFR_MODEM:
      case VSM_MEDIA_TYPE_FR_MODEM:
      case VSM_MEDIA_TYPE_HR_MODEM:
      case VSM_MEDIA_TYPE_EAMR:
      case VSM_MEDIA_TYPE_EVS:
      {

         if(TRUE == vdec_ptr->modules.ctm_rx.ctm_rx_capi.intf_extn_list.is_tty_intf_supported)
         {
            result = vdec_capi_set_param(&vdec_ptr->modules.ctm_rx.ctm_rx_capi, TTY_PARAM_MODE, &tty_mode, sizeof(tty_mode));
            if(ADSP_FAILED(result))
            {
               elite_svc_send_ack(msg_ptr, result);
               return result;
            }
         }
         break;
      }
      case VSM_MEDIA_TYPE_13K_MODEM:
      case VSM_MEDIA_TYPE_EVRC_MODEM:
      case VSM_MEDIA_TYPE_4GV_NB_MODEM:
      case VSM_MEDIA_TYPE_4GV_WB_MODEM:
      case VSM_MEDIA_TYPE_4GV_NW_MODEM:
      case VSM_MEDIA_TYPE_4GV_NW:
      case VSM_MEDIA_TYPE_EVRC_NW_2K:
         {
            result = vdec_capi_set_param(&vdec_ptr->modules.dec_capi.common, TTY_PARAM_MODE, &tty_mode, sizeof(tty_mode));
            if(ADSP_FAILED(result))
            {
               elite_svc_send_ack(msg_ptr, result);
               return result;
            }
            break;
         }
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_set_tty_mode: tty mode received for unsupported media_type(0x%x)",(int)vdec_ptr->voc_type);
            break;   // do nothing
         }
   }
   elite_svc_send_ack(msg_ptr, result);
   return result;
}
static ADSPResult vdec_set_ltetty_mode (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_voc_stream_set_tty_mode *set_tty_mode_cmd_ptr = (elite_msg_custom_voc_stream_set_tty_mode *) msg_ptr->pPayload;

   // send tty mode to ltetty rx capi always.
   tty_param_mode_t ltetty_mode;
   ltetty_mode.mode = set_tty_mode_cmd_ptr->tty_mode;

   if(TRUE == vdec_ptr->modules.ltetty_rx.intf_extn_list.is_tty_intf_supported)
   {
      result = vdec_capi_set_param(&vdec_ptr->modules.ltetty_rx, TTY_PARAM_MODE, &ltetty_mode, sizeof(ltetty_mode));
      if(ADSP_FAILED(result))
      {
         elite_svc_send_ack(msg_ptr, result);
         return result;
      }
   }
   elite_svc_send_ack(msg_ptr, result);
   return result;
}

ADSPResult vdec_aggregate_modules_kpps (void* instance_ptr, uint32_t* kpps_changed)
{
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;

   if ((NULL == vdec_ptr) || (NULL == kpps_changed))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " VCP: Voice Decoder thread KPPS aggregator received bad pointers");
      return ADSP_EBADPARAM;
   }

   uint32_t kpps;
   uint32_t aggregate_kpps = 0;

   // Voice Decoder KPPS
   kpps = 0;
   if(vdec_ptr->modules.is_decoder_capi)
   {
      kpps = vdec_ptr->modules.dec_capi.common.kpps;
   }
   else
   {
      vcmn_find_vockpps_table (VOICE_DECODE_KPPS_TABLE, vdec_ptr->voc_type, &kpps);
   }
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Decoder kpps (%d), session number (%lx)", (int )kpps, vdec_ptr->session_num);
   aggregate_kpps += kpps;

   // Host PCM
   kpps = vdec_ptr->modules.hpcm.kpps;;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Host PCM kpps (%d), session number (%lx)", (int )kpps, vdec_ptr->session_num);
   aggregate_kpps += kpps;

   // CTM Rx
   kpps = vdec_ptr->modules.ctm_rx.ctm_rx_capi.kpps;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: ctm rx kpps (%d), session number (0x%lx)", (int )kpps, vdec_ptr->session_num);
   aggregate_kpps += kpps;

   // ltetty Rx
   kpps = 0;
   kpps = vdec_ptr->modules.ltetty_rx.kpps;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: ltetty_rx kpps (%d), session number (0x%lx)", (int)kpps, vdec_ptr->session_num);
   aggregate_kpps += kpps;

   // DTMF Detect
   kpps = 0;
   kpps = vdec_ptr->modules.dtmf_det.kpps;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: dtmf_det kpps (%d), session number (0x%lx)", (int )kpps, vdec_ptr->session_num);
   aggregate_kpps += kpps;

   // Slowtalk
   kpps = 0;
   kpps = vdec_ptr->modules.slow_talk.kpps;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: slow_talk kpps (%d), session number (0x%lx)", (int )kpps, vdec_ptr->session_num);
   aggregate_kpps += kpps;

   // Soft mute
   kpps = vdec_ptr->modules.soft_mute.soft_mute_capi.kpps;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: soft mute kpps (%d), session number (0x%lx)", (int )kpps, vdec_ptr->session_num);
   aggregate_kpps += kpps;

   // TODO: Retaining this margin for safety, will need to be reassessed
   aggregate_kpps += 2000;   // adding extra 1000 kpps to ceil the number to mpps

   // Retaining behavior as it is as of today. That is to report an increase in KPPS only.
   // Once the VOICE_CMD_SET_TIMING_PARAMS commands are removed,
   // this can be modified to (vdec_pp_ptr->modules.aggregate_kpps != aggregate_kpps)
   if (vdec_ptr->modules.aggregate_kpps >= aggregate_kpps)
   {
      *kpps_changed = FALSE;
   }
   else
   {
      *kpps_changed = TRUE;
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec KPPS changed from (%lu) to (%lu), session(%lx)", vdec_ptr->modules.aggregate_kpps, aggregate_kpps, vdec_ptr->session_num);
   }

   // Update state
   vdec_ptr->modules.aggregate_kpps = aggregate_kpps;

   return ADSP_EOK;
}

ADSPResult vdec_aggregate_modules_delay (void* instance_ptr)
{
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;

   if (NULL == vdec_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Voice Decoder delay aggregator received bad pointers");
      return ADSP_EBADPARAM;
   }

   uint32_t delay;
   uint32_t aggregate_delay = 0;

   // Voice Decoder delay
   delay = 0;
   if(vdec_ptr->modules.is_decoder_capi)
   {
      delay = vdec_ptr->modules.dec_capi.common.delay;
   }
   else
   {
      (void) vdec_get_vocoder_delay (vdec_ptr->voc_type, &delay);
   }
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Decoder delay (%d), session number (%lx)", (int )delay, vdec_ptr->session_num);
   aggregate_delay += delay;

   // Update state
   vdec_ptr->modules.aggregate_delay = aggregate_delay;

   return ADSP_EOK;

}

// This command is supported only in STOP state.
// The command needs to be forwarded to PP thread for Dec PP offset.
static ADSPResult vdec_set_timingv3_cmd (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_voc_timing_param_type *set_timing_cmd_ptr = (elite_msg_custom_voc_timing_param_type *) msg_ptr->pPayload;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_set_timingv3_cmd begin session(%lx)", vdec_ptr->session_num);

   // Verify stop state of the thread.
   if (FALSE == vdec_ptr->process_data)
   {
      vsm_set_timing_params_v2_t* vfr_cmd_ptr = (vsm_set_timing_params_v2_t*) set_timing_cmd_ptr->param_data_ptr;
      //dbg msg is printed in VSM with all params
      /*MSG_7(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vdec_set_timingv3_cmd, mode(%d), VSID(0x%lx), \
        enc_offset(%d), decreq_offset(%d), dec_offset(%d),decpp_offset(%d),session(%x)",
        vfr_cmd_ptr->mode,vfr_cmd_ptr->vsid,vfr_cmd_ptr->enc_offset,vfr_cmd_ptr->dec_req_offset,
        vfr_cmd_ptr->dec_offset,vfr_cmd_ptr->decpp_offset,vdec_ptr->session_num); */

      // Verify validity of VFR mode. In this version of timing cmd, mode supports only two values - VFR_NONE and VFR_HARD
      // If VFR_HARD, further information is derived from VSID
      if (VFR_HARD >= vfr_cmd_ptr->mode)
      {
         vdec_ptr->vfr_mode = vfr_cmd_ptr->mode;
      }
      else
      {
         vdec_ptr->vfr_mode = VFR_NONE;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_set_timingv3_cmd, invalid mode(%x),setting to VFR_NONE,session(%lx)", vfr_cmd_ptr->mode, vdec_ptr->session_num);
         result = ADSP_EBADPARAM;
      }

	  vdec_ptr->vfr_cycle = vfr_cmd_ptr->vfr_cycle/MAX_TIMER_OFFSET;
	  vdec_ptr->timing.vtm_sub_unsub_decode_data.vfr_cycle = vfr_cmd_ptr->vfr_cycle;
	  vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.vfr_cycle = vfr_cmd_ptr->vfr_cycle;

      // Update VFR modes everywhere applicable.
      //vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.vfr_mode = (uint8_t) vdec_ptr->vfr_mode;
      vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.vfr_mode = (uint8_t) vdec_ptr->vfr_mode;
      vdec_ptr->timing.vtm_sub_unsub_decode_data.vfr_mode = (uint8_t) vdec_ptr->vfr_mode;

      // Verify validity of VSID. In VFR_HARD case, VSID should be non-zero. In VFR_NONE case, VSID is don't care.
      if ((VFR_HARD == vdec_ptr->vfr_mode) && (0 == vfr_cmd_ptr->vsid))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_set_timingv3_cmd, invalid VSID(%lx), session(%lx)", vfr_cmd_ptr->vsid, vdec_ptr->session_num);
         result = ADSP_EBADPARAM;
      }
      else
      {
         vdec_ptr->vsid = vfr_cmd_ptr->vsid;
      }


      // Update VFR modes everywhere applicable.
      //vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.vsid = vdec_ptr->vsid;
      vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.vsid = vdec_ptr->vsid;
      vdec_ptr->timing.vtm_sub_unsub_decode_data.vsid = vdec_ptr->vsid;

      // Verify validity of offsets
      // MIN_TIMER_OFFSET is set to 0, so no need to check if offset is below min because it's unsigned
      if ((vfr_cmd_ptr->vfr_cycle < vfr_cmd_ptr->dec_req_offset) || (vfr_cmd_ptr->vfr_cycle < vfr_cmd_ptr->dec_offset))
      {
         //vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.offset = 3300;   // default 3.3 ms
         vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.offset = 3300;   // default 3.3 ms
         vdec_ptr->timing.vtm_sub_unsub_decode_data.offset = 8300;    // default 8.3ms
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_set_timingv3_cmd: Invalid offset(s) - dec_req(%d), dec(%d), defaulting to 3.3ms, 8.3ms", vfr_cmd_ptr->dec_req_offset,
               vfr_cmd_ptr->dec_offset);
         result = ADSP_EOK;   // since ISOD/API says we support zero offsets, ack EOK and return EOK.
         // just printing message is good enough.  Changed the MSG to error to hightlight
      }
      else
      {
         // vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.offset = vfr_cmd_ptr->dec_req_offset;   // default 3.3 ms
         vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.offset = vfr_cmd_ptr->dec_req_offset;
         vdec_ptr->timing.vtm_sub_unsub_decode_data.offset = vfr_cmd_ptr->dec_offset;   // default 8.3ms
      }

      //update version of timing used
      //vdec_ptr->timing.vtm_sub_unsub_pkt_req_data.timing_ver =
      //       VFR_CLIENT_INFO_VER_2;
      vdec_ptr->timing.vtm_sub_unsub_vds_pkt_req_data.timing_ver =
         VFR_CLIENT_INFO_VER_2;
      vdec_ptr->timing.vtm_sub_unsub_decode_data.timing_ver = VFR_CLIENT_INFO_VER_2;

      elite_svc_send_ack (msg_ptr, result);

   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Timing can't be changed in RUN, session(%lx)", vdec_ptr->session_num);
      result = ADSP_EBUSY;
      elite_svc_send_ack (msg_ptr, result);
   }

   return result;
}

ADSPResult vdec_get_vocoder_delay (uint32_t voc_type, uint32_t* delay_ptr)
{
   //delay accounted for at encoder side
   *delay_ptr = 0;
   switch (voc_type)
   {
      case VSM_MEDIA_TYPE_AMR_NB_MODEM:
      case VSM_MEDIA_TYPE_AMR_NB_IF2:
         {
            break;
         }
      case VSM_MEDIA_TYPE_AMR_WB_MODEM:
         {
            break;
         }
      case VSM_MEDIA_TYPE_EFR_MODEM:
         {
            break;
         }
      case VSM_MEDIA_TYPE_FR_MODEM:
         {
            break;
         }
      case VSM_MEDIA_TYPE_HR_MODEM:
         {
            break;
         }
         {
            break;
         }
      case VSM_MEDIA_TYPE_EAMR:
         {
            break;
         }
      case VSM_MEDIA_TYPE_EVS:
         {
            break;
         }
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error! Invalid vocoder type for delay (%lx), returning zero", voc_type);
         }
   }
   return ADSP_EOK;
}

static ADSPResult vdec_register_event (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_event_reg_type *payload_ptr = (elite_msg_custom_event_reg_type*) msg_ptr->pPayload;
   uint32_t event_id = payload_ptr->event_id;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Vdec received registration for event %lx", event_id);

   switch (event_id)
   {
      case VSM_EVT_VOC_OPERATING_MODE_UPDATE:
         {
            // set mode detection to true
            vdec_ptr->vocoder_op_detection = 1;
            // Issue default mode event
            vdec_send_mode_notification_v2 (instance_ptr);
            break;
         }
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error! Vdec received registration for invalid event %lx", event_id);
         }
   }
   elite_msg_return_payload_buffer (msg_ptr);
   return ADSP_EOK;
}

static ADSPResult vdec_unregister_event (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_event_reg_type *payload_ptr = (elite_msg_custom_event_reg_type*) msg_ptr->pPayload;
   uint32_t event_id = payload_ptr->event_id;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: Vdec received deregistration for event %lx", event_id);

   switch (event_id)
   {
      case VSM_EVT_VOC_OPERATING_MODE_UPDATE:
         {
            // set mode detection to true
            vdec_ptr->vocoder_op_detection = 0;
            break;
         }
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error! Vdec received deregistration for invalid event %lx", event_id);
         }
   }
   elite_msg_return_payload_buffer (msg_ptr);
   return ADSP_EOK;
}

static ADSPResult vdec_send_mode_notification_v2 (void* instance_ptr)
{
   ADSPResult rc = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;

   if (0 == vdec_ptr->vocoder_op_detection)
   {
      return ADSP_EOK;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_send_mode_notification_v2 begin. op_mode(%lx) session(%lx)",vdec_ptr->session_op_mode, vdec_ptr->session_num);
   vsm_evt_voc_operating_mode_update_t mode_update;

   mode_update.direction = VSM_VOC_OPERATING_MODE_DIRECTION_RX;
   mode_update.reserved = 0;
   mode_update.mode = vdec_ptr->session_op_mode;

   rc = elite_apr_if_alloc_send_event (vdec_ptr->apr_info_ptr->apr_handle, vdec_ptr->apr_info_ptr->self_addr, vdec_ptr->apr_info_ptr->self_port, vdec_ptr->apr_info_ptr->self_addr,
         vdec_ptr->apr_info_ptr->self_port, 0,
         VSM_EVT_VOC_OPERATING_MODE_UPDATE, &mode_update, (sizeof(mode_update)));

   if (ADSP_FAILED(rc))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Failed to create APR request for VSM_EVT_VOC_OPERATING_MODE_UPDATE :session(%lx)", vdec_ptr->session_num);
      return rc;
   }


   //KA: need to figure out how to handle this eamr special mode detection case
   // given that this is a proprietary qcom module, I think it might be ok to just leave
   // this as a special case in fwk for now
   // For eAMR also need to send tx side mode
   if (VSM_MEDIA_TYPE_EAMR == vdec_ptr->voc_type)
   {
      mode_update.direction = VSM_VOC_OPERATING_MODE_DIRECTION_TX;
      mode_update.reserved = 0;
      mode_update.mode = vdec_ptr->session_op_mode;

      rc = elite_apr_if_alloc_send_event (vdec_ptr->apr_info_ptr->apr_handle, vdec_ptr->apr_info_ptr->self_addr, vdec_ptr->apr_info_ptr->self_port, vdec_ptr->apr_info_ptr->self_addr,
            vdec_ptr->apr_info_ptr->self_port, 0,
            VSM_EVT_VOC_OPERATING_MODE_UPDATE, &mode_update, (sizeof(mode_update)));

      if (ADSP_FAILED(rc))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Failed to create APR request for VSM_EVT_VOC_OPERATING_MODE_UPDATE :session(%lx)", vdec_ptr->session_num);
         return rc;
      }

   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Vdec sent the vocoder mode change notification, notified mode(0x%lx), vocoder native mode (0x%lx), session (%lx)",
         mode_update.mode,
         vdec_ptr->voc_native_op_mode,
         vdec_ptr->session_num);
   return rc;
}

static ADSPResult vdec_set_param_v3 (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_set_param_v3_type *payload_ptr = (elite_msg_custom_set_param_v3_type*) msg_ptr->pPayload;
   uint32_t cal_handle = payload_ptr->cal_handle;
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec received set_param_v3, handle %lx, session(%lx)", cal_handle, vdec_ptr->session_num);

   //call into mvm using mvm_call as an entry point to cvd_cal_query
   vss_imvm_cmd_cal_query_t mvm_payload;
   mvm_payload.query_handle = payload_ptr->cal_handle;
   mvm_payload.cb_fn = vdec_calibration_cb_func;
   mvm_payload.client_data = instance_ptr;

   result = mvm_call (MVM_CMDID_CAL_QUERY, &mvm_payload, sizeof(mvm_payload));

   elite_svc_send_ack (msg_ptr, result);

   return result;
}

void vdec_calibration_cb_func (void* cal_params_payload_ptr, void* cb_data)
{
   ADSPResult result = ADSP_EOK;
   vdec_t* vdec_ptr = (vdec_t*) cb_data;

   uint32_t mod_id, param_id, param_size;
   uint16_t instance_id;
   void *param_data;

   uint32_t minor_cal_version = *(uint32_t *)cal_params_payload_ptr;

   if(0 == minor_cal_version)
   {
      cvd_cal_param_t *cal_params_ptr = (cvd_cal_param_t *)cal_params_payload_ptr;
      mod_id = cal_params_ptr->module_id;
      param_id = cal_params_ptr->param_id;
      param_size = cal_params_ptr->param_data_size;
      instance_id = 0;
      param_data = cal_params_ptr->param_data;
   }
   else
   {
      cvd_cal_param_v1_t *cal_params_ptr = (cvd_cal_param_v1_t *)cal_params_payload_ptr;
      mod_id = cal_params_ptr->module_id;
      param_id = cal_params_ptr->param_id;
      param_size = cal_params_ptr->param_data_size;
      instance_id = cal_params_ptr->instance_id;
      param_data = cal_params_ptr->param_data;
   }


   result = vdec_set_param_int (vdec_ptr, mod_id, instance_id, param_id, param_data, param_size);
   if (ADSP_EOK != result && ADSP_EUNSUPPORTED != result)
   {
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec set param error %u, mod %lx,instance %lx, param %lx, session(%lx)",
            result, mod_id, instance_id, param_id, vdec_ptr->session_num);
   }
   return;
}

static ADSPResult vdec_set_enc_rate (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   capi_v2_err_t capi_v2_result = CAPI_V2_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_set_enc_rate_type *payload_ptr = (elite_msg_custom_set_enc_rate_type*) msg_ptr->pPayload;
   uint32_t media_type, encoder_rate;

   // KA: Need to figure out how to handle this. One option is to make 1x vocoders state aware
   // that way fwk always passes rate to the vocoder, and vocoder only raises event in stop state
   // but not in run state
   if (TRUE == vdec_ptr->process_data)
   {
      elite_svc_send_ack (msg_ptr, result);
      return result;
   }
   if(vdec_ptr->modules.is_decoder_capi && vdec_ptr->modules.dec_capi.common.intf_extn_list.is_voc_intf_supported)
   {
      //Only update mode/send notification for set_rate in stop state
      // This is based on the assumption that the COPs will most likely be symmetric
      media_type = payload_ptr->media_type;
      encoder_rate = payload_ptr->encoder_rate;

      switch (media_type)
      {
         case VSM_MEDIA_TYPE_4GV_WB_MODEM:
         case VSM_MEDIA_TYPE_EVRC_NW_2K:
         case VSM_MEDIA_TYPE_4GV_NW_MODEM:
         case VSM_MEDIA_TYPE_4GV_NW:
         {
            if(NULL == vdec_ptr->modules.dec_capi.common.module_ptr)
            {
               result = ADSP_EBADPARAM;
               break;
            }
            capi_v2_port_info_t port_info = {FALSE, FALSE, 0};
            capi_v2_buf_t enc_rate_param;
            voc_param_enc_rate_t enc_rate;

            enc_rate.rate = encoder_rate;
            enc_rate_param.data_ptr = (int8_t *)&enc_rate;
            enc_rate_param.actual_data_len = sizeof(voc_param_enc_rate_t);
            enc_rate_param.max_data_len = sizeof(voc_param_enc_rate_t);

            capi_v2_result = vdec_ptr->modules.dec_capi.common.module_ptr->vtbl_ptr->set_param(vdec_ptr->modules.dec_capi.common.module_ptr, VOC_PARAM_ENC_RATE, &port_info, &enc_rate_param);
            result = capi_v2_err_to_adsp_result(capi_v2_result);
            break;
         }

         default:
         {
            result = ADSP_EOK;
            break;
         }
      }
   }
   elite_svc_send_ack (msg_ptr, result);
   return result;

}


static ADSPResult vdec_set_voc_param (void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   capi_v2_err_t capi_v2_result = CAPI_V2_EOK;
   vdec_t* vdec_ptr = (vdec_t*) instance_ptr;
   elite_msg_custom_set_voc_param_type *payload_ptr = (elite_msg_custom_set_voc_param_type*)msg_ptr->pPayload;
   int8_t *param_data_payload_ptr = (int8_t*) (payload_ptr->payload_address);

   if(NULL == param_data_payload_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_set_voc_param : NULL payload pointer error, session(%lx)", vdec_ptr->session_num);
      result = ADSP_EBADPARAM;
      elite_svc_send_ack(msg_ptr, result);
      return result;
   }

   if(vdec_ptr->modules.is_decoder_capi
      && vdec_ptr->modules.dec_capi.common.intf_extn_list.is_voc_intf_supported
      && (vdec_ptr->modules.dec_capi.common.module_ptr != NULL))
   {
      voice_voc_param_data_t *voc_param_data_ptr = (voice_voc_param_data_t *)param_data_payload_ptr;
      capi_v2_port_info_t port_info = {FALSE, FALSE, 0};
      capi_v2_buf_t voc_param;

      param_data_payload_ptr +=  sizeof(voice_voc_param_data_t);
      voc_param.data_ptr = param_data_payload_ptr;
      voc_param.actual_data_len = voc_param_data_ptr->param_size;
      voc_param.max_data_len = voc_param_data_ptr->param_size;

      capi_v2_result = vdec_ptr->modules.dec_capi.common.module_ptr->vtbl_ptr->set_param(vdec_ptr->modules.dec_capi.common.module_ptr, voc_param_data_ptr->param_id, &port_info, &voc_param);
      result = capi_v2_err_to_adsp_result(capi_v2_result);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_set_voc_param: Invalid mediatype session(%lx)",
            vdec_ptr->session_num);
      result = ADSP_EUNSUPPORTED;
   }

   elite_svc_send_ack (msg_ptr, result);
   return result;

}

static ADSPResult vdec_pp_allocate_mem (vdec_t *vdec_ptr)
{
   ADSPResult result = ADSP_EOK;
   uint32_t total_size;
   uint32_t rec_buf_size, ss_size, scratch_size;

   //in_buf_size=VOICE_ROUNDTO8(PP_IN_CIRC_BUF_SIZE_SAMPLES*2);
   rec_buf_size = VOICE_ROUNDTO8(DEC_OUT_CIRC_BUF_SIZE_REC*MAX_SAMP_RATE_FACTOR*2);
   voice_ss_get_size (&ss_size);
   ss_size = VOICE_ROUNDTO8(ss_size);
   scratch_size = VOICE_ROUNDTO8(VDEC_SCRATCH_MEM_SIZE_IN_BYTES);
   total_size = rec_buf_size + ss_size + scratch_size;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Computed memory requirement for Vdec PP - %ld, session (%lx)", total_size, vdec_ptr->session_num);

   vdec_ptr->vdec_pp_memory.start_addr_ptr = (int8_t *) qurt_elite_memory_malloc (total_size, QURT_ELITE_HEAP_DEFAULT);

   if (NULL == vdec_ptr->vdec_pp_memory.start_addr_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_pp_allocate_mem: Out of Memory!! session(%lx)", vdec_ptr->session_num);
      return ADSP_ENOMEMORY;
   }

   memset (vdec_ptr->vdec_pp_memory.start_addr_ptr, 0, total_size);   // clear the memory before use

   vdec_ptr->vdec_pp_memory.size = total_size;   // size allocated

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_pp_allocate_mem: allocated memory %ld, session (%lx)", vdec_ptr->vdec_pp_memory.size, vdec_ptr->session_num);

   vdec_ptr->vdec_pp_memory.usage_addr_ptr = vdec_ptr->vdec_pp_memory.start_addr_ptr;   // Usage level pointer

   //vdec_ptr->pp_in_circ_buf_ptr = vdec_ptr->vdec_pp_memory.usage_addr_ptr;
   //vdec_ptr->vdec_pp_memory.usage_addr_ptr += in_buf_size;
   //vdec_ptr->vdec_pp_memory.usage_addr_ptr = (int8_t *)VOICE_ROUNDTO8(vdec_ptr->vdec_pp_memory.usage_addr_ptr);

   vdec_ptr->modules.vdec_pp_modules.record.circ_decout_rec_buf_ptr = vdec_ptr->vdec_pp_memory.usage_addr_ptr;
   vdec_ptr->vdec_pp_memory.usage_addr_ptr += rec_buf_size;
   vdec_ptr->vdec_pp_memory.usage_addr_ptr = (int8_t *) VOICE_ROUNDTO8(vdec_ptr->vdec_pp_memory.usage_addr_ptr);

   vdec_ptr->modules.vdec_pp_modules.record.ss_struct_ptr = vdec_ptr->vdec_pp_memory.usage_addr_ptr;
   vdec_ptr->vdec_pp_memory.usage_addr_ptr += ss_size;
   vdec_ptr->vdec_pp_memory.usage_addr_ptr = (int8_t *) VOICE_ROUNDTO8(vdec_ptr->vdec_pp_memory.usage_addr_ptr);
   voice_ss_set_mem (&(vdec_ptr->modules.vdec_pp_modules.record.ss_struct), vdec_ptr->modules.vdec_pp_modules.record.ss_struct_ptr, ss_size);

   vdec_ptr->scratch_ptr = vdec_ptr->vdec_pp_memory.usage_addr_ptr;
   vdec_ptr->vdec_pp_memory.usage_addr_ptr += scratch_size;
   vdec_ptr->vdec_pp_memory.usage_addr_ptr = (int8_t *) VOICE_ROUNDTO8(vdec_ptr->vdec_pp_memory.usage_addr_ptr);

   if ((uint32_t) (vdec_ptr->vdec_pp_memory.usage_addr_ptr - vdec_ptr->vdec_pp_memory.start_addr_ptr) <= vdec_ptr->vdec_pp_memory.size)
   {
      result = ADSP_EOK;
   }
   else
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Error: vdec_pp_allocate_mem: allocated memory %d, memory consumed %d session(%lx)", (int )vdec_ptr->vdec_pp_memory.size,
            (int )(vdec_ptr->vdec_pp_memory.usage_addr_ptr - vdec_ptr->vdec_pp_memory.start_addr_ptr), vdec_ptr->session_num);
      result = ADSP_ENOMEMORY;
      return result;
   }

   return result;
}

static ADSPResult vdec_pp_init (vdec_t *vdec_ptr)   //PG: call this from reinit also
{
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_pp_init begin session(%lx)", vdec_ptr->session_num);

   // Reset control code params
   (void) vdec_pp_ctrl_params_init (vdec_ptr);
   // Reset params for a new call
   vdec_ptr->modules.soft_mute.mute = VOICE_UNMUTE;   // don't cache this

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_pp_init end session(%lx)", vdec_ptr->session_num);
   return result;
}

// Reset control code params, should be called for a new call and in STOP state
static ADSPResult vdec_pp_ctrl_params_init (vdec_t *vdec_ptr)
{
   ADSPResult result = ADSP_EOK;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "vdec_pp_ctrl_params_init begin session(%lx)",vdec_pp_ptr->vdec_cmn_ptr->session_num);

   vdec_ptr->pp_out_buf_size = 0;
   vdec_ptr->modules.vdec_pp_modules.record.send_media_type_rec = FALSE;
   vdec_ptr->send_media_type = FALSE;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "vdec_pp_ctrl_params_init end session(%lx)",vdec_pp_ptr->vdec_cmn_ptr->session_num);
   return result;
}


static void vdec_init_rec_circbuf (vdec_t *vdec_ptr)
{
   if (NULL != vdec_ptr->modules.vdec_pp_modules.record.circ_decout_rec_buf_ptr)
   {
      uint8_t samp_rate_factor;
      int16_t temp_buf[SS_DELAY_RECORD_PATH_DEC * MAX_SAMP_RATE_FACTOR];   // temp 2msec buffer for worst case - fb number of samples
      memset (temp_buf, 0,
            SS_DELAY_RECORD_PATH_DEC * MAX_SAMP_RATE_FACTOR * sizeof(int16_t));   // clear the buffer
      // Init cirucular buffer to 24ms buffer based ont the smapling frequncy

      samp_rate_factor =  vdec_ptr->sampling_rate_dec / (VOICE_NB_SAMPLING_RATE);

      voice_circbuf_init (&(vdec_ptr->modules.vdec_pp_modules.record.circ_struct_decout_rec), (int8_t*) (vdec_ptr->modules.vdec_pp_modules.record.circ_decout_rec_buf_ptr),
            (int32_t) samp_rate_factor * DEC_OUT_CIRC_BUF_SIZE_REC /* 20ms + 2msec delay*/, MONO_VOICE, (int32_t) 16 /*bitperchannel*/
            );
      // add pre-buffering
      voice_circbuf_write (&(vdec_ptr->modules.vdec_pp_modules.record.circ_struct_decout_rec), (int8_t *) &(temp_buf[0]), samp_rate_factor * SS_DELAY_RECORD_PATH_DEC   // add 2ms delay for pre buffering
            );
   }
}

static void vdec_pp_allocate_mem_free (vdec_t *vdec_ptr)
{
   if (NULL != vdec_ptr->modules.vdec_pp_modules.record.ss_struct_ptr)
   {
      voice_ss_end (&(vdec_ptr->modules.vdec_pp_modules.record.ss_struct));
      vdec_ptr->modules.vdec_pp_modules.record.ss_struct_ptr = NULL;
   }

   vdec_ptr->modules.vdec_pp_modules.record.circ_decout_rec_buf_ptr = NULL;

   vdec_ptr->scratch_ptr = NULL;

   qurt_elite_memory_free (vdec_ptr->vdec_pp_memory.start_addr_ptr);
   vdec_ptr->vdec_pp_memory.start_addr_ptr = NULL;
   vdec_ptr->vdec_pp_memory.usage_addr_ptr = NULL;
   vdec_ptr->vdec_pp_memory.size = 0;

}

static void vdec_do_sample_slip_stuff (vdec_t* vdec_ptr, int16_t *in_ptr, int16_t *out_ptr, int16_t slip_stuff_samples)
{
   uint16_t frame_size_10msec = vdec_ptr->frame_samples_dec >> 1;   //size of 10 msec data.
   uint16_t offset_10msec = 0, output_samples;
   uint16_t input_samples = frame_size_10msec;   //input samples = 10 msec frame samples

   uint16_t num_10msec_frames = 2;

   // Process 10 msec frames. adjust samples in second 10 msec frames
   // when num_10msec_frame=1, adjust drift in same frame i.e for nb do 79->79(stuff) or 81->81(slip) correction
   // when num_10msec_frame=2, adjust drift in second 10 msec frame i.e for nb do 80->80 and 79->79(stuff) or 81->81(slip) in second 10 msec frame.
   for (uint16_t i = 0; i < num_10msec_frames; i++)
   {
      output_samples = input_samples - (i * slip_stuff_samples);
      offset_10msec = frame_size_10msec * i;

      //dbg: MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Voice SS vdec: voice_sample_slip_process-in_frame_size=%d,out_frame_size=%d,i=%d",input_samples,output_samples,i);

      (void) voice_sample_slip_process (&(vdec_ptr->modules.vdec_pp_modules.record.ss_struct), &out_ptr[offset_10msec], &in_ptr[offset_10msec], input_samples, output_samples);

      if ((int8_t) vdec_ptr->modules.vdec_pp_modules.record.ss_multiframe_counter > 0)
      {
         //decrease multiframe counter
         vdec_ptr->modules.vdec_pp_modules.record.ss_multiframe_counter -= 1;
      }
   }
}

static ADSPResult vdec_pp_process (vdec_t* vdec_ptr)
{
   ADSPResult result = ADSP_EOK;
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   int16_t *in_ptr = (int16_t*) &(vdec_ptr->dec_out_buf[0]);   // pointer to local input buffer
   int16_t *out_ptr = (int16_t*) &(vdec_ptr->pp_out_buf[0]);   // pointer to local output buffer

   //MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_pp_process begin");

   if (vdec_ptr->process_data)
   {
      if ( TRUE == vdec_ptr->modules.vdec_pp_modules.record.enable_recording)
      {
         int16_t slip_stuff_samples = 0;
         int16_t *local_ss_ptr = (int16_t*) vdec_ptr->scratch_ptr;   // local input buffer

         //calculate drift with respect to av/hp timer
         vdec_cal_drift (vdec_ptr);
         //calculate sample slipping on the pp out put w.r.t external audio reference port
         vdec_cal_sample_slip_stuff (vdec_ptr, &(slip_stuff_samples));
         //conversion from VFR rate to Codec rate
         vdec_do_sample_slip_stuff (vdec_ptr, in_ptr, local_ss_ptr, slip_stuff_samples);
         //copy the output in to circualr buffer
         result = voice_circbuf_write (&vdec_ptr->modules.vdec_pp_modules.record.circ_struct_decout_rec, (int8_t *) local_ss_ptr, (vdec_ptr->frame_samples_dec - slip_stuff_samples));
         if (ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: circular buffer write error for recording session(%lx)", vdec_ptr->session_num);
         }
         vdec_ptr->wait_mask |= VDEC_REC_BUF_MASK;
      }
   }

#if defined(__qdsp6__) && !defined(SIM)
   int16_t *post_dec_in_temp = (int16_t*)vdec_ptr->scratch_ptr;
   memscpy( post_dec_in_temp, VOICE_ROUNDTO8(VDEC_SCRATCH_MEM_SIZE_IN_BYTES) ,in_ptr, vdec_ptr->frame_samples_dec<<1);   // destinatoin is size allocated to scratch buffer  // worst scratch memory size = 22 ms for FB
#endif

   //Copy input to output before starting any processing
   memsmove ((int8_t*) out_ptr, sizeof(vdec_ptr->pp_out_buf), (int8_t*) in_ptr, vdec_ptr->frame_samples_dec << 1);

   capi_v2_buf_t vdec_pp_in_buf = { (int8_t*) out_ptr, vdec_ptr->frame_samples_dec << 1, vdec_ptr->frame_samples_dec << 1};
   capi_v2_stream_data_t vdec_pp_stream_in[1];
   capi_v2_stream_data_t *vdec_pp_stream_in_ptr[] =  { &vdec_pp_stream_in[0] };
   vdec_pp_stream_in[0].bufs_num = 1;
   vdec_pp_stream_in[0].buf_ptr = &vdec_pp_in_buf;

   //CTM Rx processing
   if(TRUE == vdec_ptr->modules.ctm_rx.ctm_rx_capi.intf_extn_list.is_tty_intf_supported)
   {
      if(vdec_ptr->tty_state_ptr->m_sync_recover_rx)
      {
         result = vdec_capi_set_param(&vdec_ptr->modules.ctm_rx.ctm_rx_capi, TTY_PARAM_CTM_RESYNC, NULL, 0);
         if(ADSP_EOK == result)
         {
            vdec_ptr->tty_state_ptr->m_sync_recover_rx = FALSE;
         }
      }
      if(vdec_ptr->tty_state_ptr->m_ctm_character_transmitted)
      {
         vdec_capi_set_param(&vdec_ptr->modules.ctm_rx.ctm_rx_capi, TTY_PARAM_CTM_TX_CHAR_TRANSMITTED, NULL, 0);
      }
   }

   if(vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr)
   {
      capi_result = vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr->vtbl_ptr->process(vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr, vdec_pp_stream_in_ptr, vdec_pp_stream_in_ptr);
      if (CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_pp_process: ctm_rx: process failed. result(0x%lx) session(0x%lx)", result, vdec_ptr->session_num);
      }
   }

   if(vdec_ptr->modules.ltetty_rx.module_ptr)
   {
      capi_result = vdec_ptr->modules.ltetty_rx.module_ptr->vtbl_ptr->process(
            vdec_ptr->modules.ltetty_rx.module_ptr, vdec_pp_stream_in_ptr, vdec_pp_stream_in_ptr);
      if (CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_pp_process: ltetty_rx: process failed. result(0x%lx) session(0x%lx)", result, vdec_ptr->session_num);
      }
   }

   //DTMF detection processing
   if(vdec_ptr->modules.dtmf_det.module_ptr)
   {
      capi_result = vdec_ptr->modules.dtmf_det.module_ptr->vtbl_ptr->process(
             vdec_ptr->modules.dtmf_det.module_ptr, vdec_pp_stream_in_ptr, vdec_pp_stream_in_ptr);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "VCP: vdec_pp_process: dtmf_det: process failed. result(0x%lx) session(0x%lx)", capi_result,vdec_ptr->session_num);
      }
   }

   //SlowTalk processing
   if(vdec_ptr->modules.slow_talk.module_ptr)
   {
      capi_result = vdec_ptr->modules.slow_talk.module_ptr->vtbl_ptr->process(vdec_ptr->modules.slow_talk.module_ptr, vdec_pp_stream_in_ptr, vdec_pp_stream_in_ptr);
      if (CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_pp_process: slow_talk: process failed. result(0x%lx) session(0x%lx)", result,vdec_ptr->session_num);
      }
   }

#if defined(__qdsp6__) && !defined(SIM)
   // log saved input buf and output prior to WV, so both are always at same vocoder sampling rate
   int8_t *buf_ptr[4] =
   {  (int8_t *) post_dec_in_temp, (int8_t *) out_ptr, NULL, NULL};
   uint32_t sampling_rate_log_id;
   sampling_rate_log_id = voice_get_sampling_rate_log_id(vdec_ptr->sampling_rate_dec);
   voice_log_buffer( buf_ptr,
         VOICE_LOG_TAP_POINT_VDEC_OUT,
         (sampling_rate_log_id << 3) | vdec_ptr->session_num,
         qurt_elite_timer_get_time(),
         VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV,
         vdec_ptr->sampling_rate_dec,
         vdec_ptr->frame_samples_dec<<1,
         NULL);
#endif

   if ((TRUE == vdec_ptr->modules.soft_mute.new_mute_cmd) && (vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr != NULL))
   {
      capi_v2_buf_t buf_soft_mute;
      buf_soft_mute.actual_data_len = buf_soft_mute.max_data_len = sizeof(voice_soft_mute_cal_param_t);
      buf_soft_mute.data_ptr = (int8_t*) &vdec_ptr->modules.soft_mute.set_param_cached;
      vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr->vtbl_ptr->set_param (vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr,
            VOICE_PARAM_SOFT_MUTE, NULL, &buf_soft_mute);
      vdec_ptr->modules.soft_mute.new_mute_cmd = FALSE;
   }

   //Apply soft mute. If mute is not enabled it copies the input to output buffer
   vdec_ptr->pp_out_buf_size = (VOICE_FRAME_SIZE_NB_20MS * (vdec_ptr->wv_nb_samp_factor)) << 1;
   if(vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr)
   {
      capi_result = vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr->vtbl_ptr->process (vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr, vdec_pp_stream_in_ptr, vdec_pp_stream_in_ptr);

      if (CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_pp_process: soft_mute_process: failed. result(0x%lx) session(0x%lx)", result,vdec_ptr->session_num);
      }
   }

   //MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_pp_process end");
   result = capi_v2_err_to_adsp_result(capi_result);
   return result;
}

static int32_t vdec_send_oobtty_char_accepted (vdec_t *vdec_ptr)
{
   int32_t rc = ADSP_EOK;

   rc = elite_apr_if_alloc_send_ack_result(vdec_ptr->apr_info_ptr->apr_handle, vdec_ptr->apr_info_ptr->self_addr, vdec_ptr->apr_info_ptr->self_port, vdec_ptr->apr_info_ptr->client_addr,
         vdec_ptr->apr_info_ptr->client_port, vdec_ptr->modules.vdec_pp_modules.oobtty.oobtty_client_token,
         VSM_EVT_OUTOFBAND_TTY_RX_CHAR_PUSH, ADSP_EOK);

   if (ADSP_FAILED(rc))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Failed to create APR request for RX CHAR ACCEPTED event 0x%08lx", rc);
   }
   return rc;
}

static ADSPResult vdec_rec_buf_handler (void *instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   vdec_t *vdec_ptr = (vdec_t *) instance_ptr;
   qurt_elite_bufmgr_node_t out_buf_mgr_node;
   MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "VCP: vdec_rec_buf_handler begin session(%lx)", vdec_ptr->session_num);

   if (FALSE == vdec_ptr->process_data)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Not in RUN state, this should not happen!! session(%lx)", vdec_ptr->session_num);
      return ADSP_ENOTREADY;
   }
   if ( TRUE == vdec_ptr->modules.vdec_pp_modules.record.enable_recording)
   {
      if (TRUE == vdec_ptr->modules.vdec_pp_modules.record.send_media_type_rec)
      {
         result = vdec_send_afe_media_type (vdec_ptr);
      }
      else
      {
         // Take next buffer off the Q
         result = qurt_elite_queue_pop_front (vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr, (uint64_t*) &out_buf_mgr_node);
         if (ADSP_EOK != result)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Error reading buf queue, result(%d) session(%lx)", result, vdec_ptr->session_num);
         }
         else
         {
            result = vdec_send_rec_buf_to_afe (vdec_ptr, &out_buf_mgr_node);
            if (!ADSP_FAILED(result))
            {   // make the flag false if the result was success
               vdec_ptr->modules.vdec_pp_modules.record.rec_first_frame = FALSE;
            }
         }
      }
   }
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "vdec_buf_handler end session(%lx)",vdec_pp_ptr->vdec_cmn_ptr->session_num);
   return result;
}

static ADSPResult vdec_send_afe_media_type (vdec_t *vdec_ptr)
{
   ADSPResult result = ADSP_EOK;
   qurt_elite_bufmgr_node_t buf_mgr_node;
   int32_t actual_size;
   elite_msg_any_t media_type_msg;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_send_afe_media_type begin session(%lx)", vdec_ptr->session_num);

   if (ADSP_FAILED(elite_mem_get_buffer (sizeof(elite_msg_data_media_type_apr_t) + sizeof(elite_multi_channel_pcm_fmt_blk_t), &buf_mgr_node, (int * )&actual_size)))
   {
      return ADSP_ENEEDMORE;
   }

   elite_msg_data_media_type_apr_t* pMediaTypePayload = (elite_msg_data_media_type_apr_t*) buf_mgr_node.pBuffer;

   pMediaTypePayload->pBufferReturnQ = buf_mgr_node.pReturnQ;
   pMediaTypePayload->pResponseQ = NULL;
   pMediaTypePayload->unClientToken = NULL;

   pMediaTypePayload->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;
   pMediaTypePayload->unMediaFormatID = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

   elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk = (elite_multi_channel_pcm_fmt_blk_t*) elite_msg_get_media_fmt_blk (pMediaTypePayload);
   memset (pMediaFormatBlk, 0, sizeof(elite_multi_channel_pcm_fmt_blk_t));

   pMediaFormatBlk->num_channels = 1;
   pMediaFormatBlk->channel_mapping[0] = PCM_CHANNEL_C;
   if (VSM_RECORD_TX_RX_STEREO == vdec_ptr->modules.vdec_pp_modules.record.recording_mode)
   {
      pMediaFormatBlk->num_channels = 2;   // for stereo recording
      pMediaFormatBlk->channel_mapping[0] = PCM_CHANNEL_L;
      pMediaFormatBlk->channel_mapping[1] = PCM_CHANNEL_R;
   }
   pMediaFormatBlk->bits_per_sample = 16;

   pMediaFormatBlk->sample_rate = vdec_ptr->sampling_rate_dec;
   pMediaFormatBlk->is_signed = TRUE;
   pMediaFormatBlk->is_interleaved = FALSE;

   media_type_msg.unOpCode = ELITE_DATA_MEDIA_TYPE;
   media_type_msg.pPayload = (void*) pMediaTypePayload;

   result = qurt_elite_queue_push_back (vdec_ptr->modules.vdec_pp_modules.record.afe_handle_ptr->dataQ, (uint64_t*) &media_type_msg);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Vdec failed to send afe media type in BufHandler() session(%lx)", vdec_ptr->session_num);
      (void) elite_msg_push_payload_to_returnq (buf_mgr_node.pReturnQ, (elite_msg_any_payload_t*) buf_mgr_node.pBuffer);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec delivered media type to AFE session(%lx)", vdec_ptr->session_num);
      vdec_ptr->modules.vdec_pp_modules.record.send_media_type_rec = FALSE;
   }
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec_send_afe_media_type end, result(%d) session(%lx)", result, vdec_ptr->session_num);
   return result;
}

static ADSPResult vdec_send_rec_buf_to_afe (vdec_t *vdec_ptr, qurt_elite_bufmgr_node_t *out_buf_mgr_node_ptr)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_any_t* peer_data_Q_msg_ptr;
   elite_msg_data_buffer_t* out_data_payload_ptr = (elite_msg_data_buffer_t*) out_buf_mgr_node_ptr->pBuffer;
   uint8_t samp_rate_factor = vdec_ptr->samp_rate_factor;
   int32_t sample_rate_ms_samples = samp_rate_factor * 8;
   int32_t output_buf_size = voice_cmn_int_div (vdec_ptr->modules.vdec_pp_modules.record.circ_struct_decout_rec.unread_samples, sample_rate_ms_samples);   // finding the number of 1ms samples

   output_buf_size = (output_buf_size * sample_rate_ms_samples) << 1;   // covert to num samples, then to bytes

   out_data_payload_ptr->pResponseQ = NULL;
   out_data_payload_ptr->unClientToken = NULL;
   out_data_payload_ptr->pBufferReturnQ = (vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr);
   out_data_payload_ptr->nOffset = 0;
   out_data_payload_ptr->nActualSize = output_buf_size;
   if (VSM_RECORD_TX_RX_STEREO == vdec_ptr->modules.vdec_pp_modules.record.recording_mode)
   {
      out_data_payload_ptr->nActualSize = out_data_payload_ptr->nActualSize << 1;
   }

   int16_t *recording_input = (int16_t*) vdec_ptr->scratch_ptr;   //22msec data for FB case
   int16_t *rec_buf_ptr = (int16_t *) (&(out_data_payload_ptr->nDataBuf));
   int i;
   // Read one Frame from output data
   // TOOD: Move this into process?
   {
      if (TRUE == vdec_ptr->modules.vdec_pp_modules.record.rec_first_frame)   // TODO: not sure why this is needed
      {
         // send 20ms of zeros, which will act as pre-buffering to take care of processing jitters
         // pre-buffering for processing jitters should be atleast "max. processing time - min. processing time"
         // since delay is not a concern in recording, we are considering  "max. processing time - min. processing time = 20ms"
         memset (recording_input, 0, output_buf_size);
      }
      else
      {
         voice_circbuf_read (&vdec_ptr->modules.vdec_pp_modules.record.circ_struct_decout_rec, (int8_t*) recording_input, (int32_t) output_buf_size / 2,
               VOICE_ROUNDTO8(VDEC_SCRATCH_MEM_SIZE_IN_BYTES));
         vdec_ptr->wait_mask ^= VDEC_REC_BUF_MASK;   // don't listen to output buf
      }

      if (VSM_RECORD_TX_RX_STEREO == vdec_ptr->modules.vdec_pp_modules.record.recording_mode)
      {
         /* filling the left channel with zeroes for stereo mode recording */
         for (i = 0; i < out_data_payload_ptr->nActualSize / 4; i++)
         {
            rec_buf_ptr[i] = 0;
            rec_buf_ptr[i + out_data_payload_ptr->nActualSize / 4] = recording_input[i];
         }
      }
      else
      {
         for (i = 0; i < out_data_payload_ptr->nActualSize / 2; i++)
         {
            rec_buf_ptr[i] = recording_input[i];
         }
      }
   }

   // send output buf to downstream service
   peer_data_Q_msg_ptr = elite_msg_convt_buf_node_to_msg (out_buf_mgr_node_ptr,
         ELITE_DATA_BUFFER,
         NULL, /* do not need response */
         0, /* token */
         0 /* do not care response result*/
         );
   // todo: check for downstreampeer null
   result = qurt_elite_queue_push_back (vdec_ptr->modules.vdec_pp_modules.record.afe_handle_ptr->dataQ, (uint64_t*) peer_data_Q_msg_ptr);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Failed to deliver buffer dowstream. Dropping session(%lx)", vdec_ptr->session_num);
      (void) elite_msg_push_payload_to_returnq (vdec_ptr->modules.vdec_pp_modules.record.rec_buf_q_ptr, (elite_msg_any_payload_t*) out_buf_mgr_node_ptr->pBuffer);
   }
   else
   {
      //dbg: MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "Delivered buffer dowstream, muteCmd(%d) bytes(%d) session(%lx)",vdec_pp_ptr->modules.mute.mute,vdec_pp_ptr->pp_out_buf_size,vdec_pp_ptr->vdec_cmn_ptr->session_num);
   }
   return result;
}

static ADSPResult vdec_stream_pp_init (vdec_t *vdec_ptr)
{
   //Init properties CAPI_V2_INPUT_MEDIA_FORMAT based on mediatype
   voice_capi_data_format_struct_t voice_std_mediatype;
   capi_v2_err_t result = CAPI_V2_EOK;

   //init mediatype
   voice_std_mediatype.media_format.format_header.data_format = CAPI_V2_FIXED_POINT;
   voice_std_mediatype.data_format.bits_per_sample = 16;
   voice_std_mediatype.data_format.sampling_rate = vdec_get_sampling_rate (vdec_ptr);
   voice_std_mediatype.data_format.data_is_signed = TRUE;
   voice_std_mediatype.data_format.num_channels = 1;

   capi_v2_prop_t init_props[] =
   {
         { CAPI_V2_INPUT_MEDIA_FORMAT, { reinterpret_cast<int8_t*> (&voice_std_mediatype), sizeof(voice_std_mediatype), sizeof(voice_std_mediatype) }, { FALSE, FALSE, 0 } },
   };
   capi_v2_proplist_t init_props_list = { VOICE_SIZE_OF_ARRAY(init_props), init_props };

   if(vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr)
   {
      result = vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr->vtbl_ptr->set_properties (vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr, &init_props_list);
      if (CAPI_V2_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Soft Mute set property failed sampling_rate(%d) session(%lx) ", voice_std_mediatype.data_format.sampling_rate, vdec_ptr->session_num);
      }
   }
   return ADSP_EOK;
}

static ADSPResult vdec_send_mixer_media_type (vdec_t *vdec_ptr)
{
   ADSPResult result = ADSP_EOK;
   qurt_elite_bufmgr_node_t buf_mgr_node;
   int32_t actual_size;
   elite_msg_any_t media_type_msg;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_send_mixer_media_type begin session(%lx)", vdec_ptr->session_num);

   if (ADSP_FAILED(elite_mem_get_buffer (sizeof(elite_msg_data_media_type_apr_t) + sizeof(elite_multi_channel_pcm_fmt_blk_t), &buf_mgr_node, (int * )&actual_size)))
   {
      return ADSP_ENEEDMORE;
   }

   elite_msg_data_media_type_apr_t* pMediaTypePayload = (elite_msg_data_media_type_apr_t*) buf_mgr_node.pBuffer;

   pMediaTypePayload->pBufferReturnQ = buf_mgr_node.pReturnQ;
   pMediaTypePayload->pResponseQ = NULL;
   pMediaTypePayload->unClientToken = NULL;

   pMediaTypePayload->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;
   pMediaTypePayload->unMediaFormatID = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

   elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk = (elite_multi_channel_pcm_fmt_blk_t*) elite_msg_get_media_fmt_blk (pMediaTypePayload);
   memset (pMediaFormatBlk, 0, sizeof(elite_multi_channel_pcm_fmt_blk_t));

   pMediaFormatBlk->num_channels = 1;
   pMediaFormatBlk->bits_per_sample = 16;
   pMediaFormatBlk->sample_rate = vdec_get_sampling_rate (vdec_ptr);
   pMediaFormatBlk->is_signed = TRUE;
   pMediaFormatBlk->is_interleaved = FALSE;
   pMediaFormatBlk->channel_mapping[0] = PCM_CHANNEL_C;

   media_type_msg.unOpCode = ELITE_DATA_MEDIA_TYPE;
   media_type_msg.pPayload = (void*) pMediaTypePayload;

   result = qurt_elite_queue_push_back (vdec_ptr->downstream_peer_ptr->dataQ, (uint64_t*) &media_type_msg);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Vdec failed to send afe media type in BufHandler() session(%lx)", vdec_ptr->session_num);
      (void) elite_msg_push_payload_to_returnq (buf_mgr_node.pReturnQ, (elite_msg_any_payload_t*) buf_mgr_node.pBuffer);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "VCP: vdec delivered media type session(%lx)", vdec_ptr->session_num);
      vdec_ptr->send_media_type = FALSE;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_send_mixer_media_type end, result(%d) session(%lx)", result, vdec_ptr->session_num);
   return result;
}

static void inline vdec_cal_drift (vdec_t *vdec_ptr)
{
   voice_cmn_accu_drift (&vdec_ptr->modules.vdec_pp_modules.record.ss_info_counter, &vdec_ptr->modules.vdec_pp_modules.record.voice_cmn_drift_info_dec_rec,
         vdec_ptr->modules.vdec_pp_modules.record.afe_drift_ptr, vdec_ptr->vfr_source, VOICE_DEC_REC, vdec_ptr->vfr_mode, vdec_ptr->session_num, vdec_ptr->timing.vtm_sub_unsub_decode_data.timing_ver,
         vdec_ptr->vsid);
}

static void vdec_cal_sample_slip_stuff (vdec_t *vdec_ptr, int16_t* slip_stuff_samples)
{
   if (0 != vdec_ptr->modules.vdec_pp_modules.record.ss_info_counter)   //if any samples to correct
   {
      //Check if previous correction is done
      if (0 == vdec_ptr->modules.vdec_pp_modules.record.ss_multiframe_counter)
      {
         //reset multiframe counter
         vdec_ptr->modules.vdec_pp_modules.record.ss_multiframe_counter =
            VDEC_SS_MULTIFRAME;

         if (0 > vdec_ptr->modules.vdec_pp_modules.record.ss_info_counter)
         {
            //This means DMS is faster and we need to do sample stuff
            *slip_stuff_samples = -1;
            //decrease(magnitude)  of info counter
            vdec_ptr->modules.vdec_pp_modules.record.ss_info_counter += (int32_t) 1;   // arithmetic to make the slip stuff counter zero
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: ICCR:: vdec sample stuff !!  count (%d) session(%lx)", *slip_stuff_samples, vdec_ptr->session_num);
         }
         else   // if (0 < vdec_pp_ptr->modules.record.ss_info_counter) .
         {
            //DMA is slow so we need to slip 1 samples
            *slip_stuff_samples = 1;
            //decrease(magnitude)  of info counter
            vdec_ptr->modules.vdec_pp_modules.record.ss_info_counter -= (int32_t) 1;   // arithmetic to make the slip counter zero
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: ICCR:: vdec sample slip !!  count (%d) session(%lx)", *slip_stuff_samples, vdec_ptr->session_num);
         }
      }
   }
}

static int32_t vdec_send_vsm_dtmf_tone_status (vdec_t *vdec_ptr, void* data_ptr)
{
   vsm_rx_dtmf_detected_t dtmf_tone_status;   // low frequency and high frequency
   int32_t rc = ADSP_EOK;
   dtmf_detection_payload_t* dtmf_status_ptr = (dtmf_detection_payload_t*)data_ptr;

   if(0 != dtmf_status_ptr->version)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid DTMF Det payload version(%d) !",dtmf_status_ptr->version);
      return ADSP_EFAILED;
   }
   for(uint32_t i = 0; i < dtmf_status_ptr->no_of_tones_detected; i++)
   {
      dtmf_tone_status.low_freq = dtmf_status_ptr->low_freq[i];
      dtmf_tone_status.high_freq = dtmf_status_ptr->high_freq[i];
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "VCP: VSM_EVT_RX_DTMF_DETECTED: low_freq(%d), high_freq(%d), APR pkt sent from Q6 - A9 session(%lx)",
            dtmf_tone_status.low_freq, dtmf_tone_status.high_freq,
            vdec_ptr->session_num);

      // vsm_rx_dtmf_detected_t has same formatting as the 4 bytes just read from circ buffer
      rc = elite_apr_if_alloc_send_event(
            vdec_ptr->apr_info_ptr->apr_handle,
            vdec_ptr->modules.apr_info_dtmf.self_addr,
            vdec_ptr->modules.apr_info_dtmf.self_port,
            vdec_ptr->modules.apr_info_dtmf.client_addr,
            vdec_ptr->modules.apr_info_dtmf.client_port,
            NULL,
            VSM_EVT_RX_DTMF_DETECTED, &dtmf_tone_status, sizeof(vsm_rx_dtmf_detected_t));

      if(ADSP_FAILED(rc))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Failed to create APR request for DTMF TONE SEND STATUS  0x%08lx", rc);
      }
   }
   return rc;
}


ADSPResult vdec_capi_set_param(voice_capi_module_t *module_info_ptr, uint32_t param_id,
                               void* param_payload_ptr, uint32_t param_size)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   // populate buffer to issue set param to capi modules
   if(module_info_ptr->module_ptr)
   {
      capi_v2_buf_t param_data_buf;
      capi_v2_port_info_t port_info;
      port_info.is_valid = FALSE;
      param_data_buf.data_ptr = (int8_t*)param_payload_ptr;
      param_data_buf.actual_data_len = param_size;
      param_data_buf.max_data_len = param_size;

      capi_result = module_info_ptr->module_ptr->vtbl_ptr->set_param(module_info_ptr->module_ptr,
                                                                     param_id, &port_info,
                                                                     &param_data_buf);

      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "VCP: vdec_capi_set_param: failed. module_id(0x%x), param_id(0x%lx), result(0x%lx)",
               module_info_ptr->module_id, param_id , capi_result);
      }
   }

   return capi_v2_err_to_adsp_result(capi_result);
}

ADSPResult vdec_capi_get_param(voice_capi_module_t *module_info_ptr, uint32_t param_id,
                               void* param_payload_ptr, uint32_t buf_size, uint16_t *param_size_ptr)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   // populate buffer to issue set param to capi modules
   if(module_info_ptr->module_ptr)
   {
      capi_v2_buf_t param_data_buf;
      capi_v2_port_info_t port_info;
      port_info.is_valid = FALSE;
      param_data_buf.data_ptr = (int8_t*)param_payload_ptr;
      param_data_buf.actual_data_len = 0;
      param_data_buf.max_data_len = buf_size;

      capi_result = module_info_ptr->module_ptr->vtbl_ptr->get_param(module_info_ptr->module_ptr,
                                                                  param_id, &port_info,
                                                                  &param_data_buf);
      *param_size_ptr = param_data_buf.actual_data_len;
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_capi_get_param: failed. module_id(0x%x), param_id(0x%lx), result(0x%lx)",
               module_info_ptr->module_id, param_id , capi_result);
      }
   }
   else
   {
      *param_size_ptr = 0;
   }

   return capi_v2_err_to_adsp_result(capi_result);
}

capi_v2_err_t vdec_decoder_capi_v2_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr)
{
   capi_v2_err_t result = CAPI_V2_EOK;
   vdec_decoder_capi_t* dec_capi_ptr = (vdec_decoder_capi_t*)context_ptr;

   if(NULL == context_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_decoder_capi_v2_cb_func: received bad pointer");
      return CAPI_V2_EFAILED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_decoder_capi_v2_cb_func: event_id(%d), media_type(0x%lx)", (int)id, dec_capi_ptr->common.module_id);
   switch(id)
   {
      case CAPI_V2_EVENT_KPPS:
      {
         capi_v2_event_KPPS_t* kpps_ptr = (capi_v2_event_KPPS_t* )event_info_ptr->payload.data_ptr;
         dec_capi_ptr->common.kpps = kpps_ptr->KPPS;
         break;
      }
      case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
      {
         capi_v2_event_algorithmic_delay_t* delay_ptr = (capi_v2_event_algorithmic_delay_t* )event_info_ptr->payload.data_ptr;
         dec_capi_ptr->common.delay = delay_ptr->delay_in_us;
         break;
      }
      case CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED:
      {
         voice_capi_data_format_struct_t* output_media_type = (voice_capi_data_format_struct_t* )event_info_ptr->payload.data_ptr;

         memscpy(&dec_capi_ptr->output_media_type, sizeof(voice_capi_data_format_struct_t), output_media_type, sizeof(voice_capi_data_format_struct_t));

         vdec_t *vdec_ptr = (vdec_t*)dec_capi_ptr->common.svc_ptr;

         if (vdec_ptr)
         {
            vdec_ptr->sampling_rate_dec = vdec_ptr->modules.dec_capi.output_media_type.data_format.sampling_rate;
            vdec_ptr->samp_rate_factor = (vdec_ptr->sampling_rate_dec / VOICE_NB_SAMPLING_RATE);
            vdec_ptr->frame_samples_dec = VOICE_FRAME_SIZE_NB_20MS * (vdec_ptr->samp_rate_factor);
         }

         break;
      }
      case CAPI_V2_EVENT_PORT_DATA_THRESHOLD_CHANGE:
      {
         if(sizeof(capi_v2_port_data_threshold_change_t) == event_info_ptr->payload.actual_data_len)
         {
            capi_v2_port_data_threshold_change_t *data_ptr = (capi_v2_port_data_threshold_change_t*)event_info_ptr->payload.data_ptr;
             if (!event_info_ptr->port_info.is_valid)
             {
                MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_decoder_capi_v2_cb_func: port index not valid");
                event_info_ptr->payload.actual_data_len = 0;
                return CAPI_V2_EBADPARAM;
             }
             if (event_info_ptr->port_info.is_input_port)
             {
                dec_capi_ptr->in_threshold_in_bytes = data_ptr->new_threshold_in_bytes;

                MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_decoder_capi_v2_cb_func: received updated in_threshold_in_bytes(%ld) for media_type(0x%lx)",
                      dec_capi_ptr->in_threshold_in_bytes, dec_capi_ptr->common.module_id);
             }
             else
             {
                dec_capi_ptr->out_threshold_in_bytes = data_ptr->new_threshold_in_bytes;

                MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_decoder_capi_v2_cb_func: received updated out_threshold_in_bytes(%ld) for media_type(0x%lx)",
                      dec_capi_ptr->out_threshold_in_bytes, dec_capi_ptr->common.module_id);
             }
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_decoder_capi_v2_cb_func: event_id(0x%lx): received bad param size (%ld)",
                  (uint32_t)CAPI_V2_EVENT_PORT_DATA_THRESHOLD_CHANGE, event_info_ptr->payload.actual_data_len);
            return CAPI_V2_ENEEDMORE;
         }

         break;
      }
      case CAPI_V2_EVENT_DATA_TO_DSP_SERVICE:
      {
         capi_v2_event_data_to_dsp_service_t *payload = (capi_v2_event_data_to_dsp_service_t* )event_info_ptr->payload.data_ptr;

         switch(payload->param_id)
         {
            case VOC_EVT_OPERATING_MODE_UPDATE:
            {
               voc_evt_operating_mode_update_t *operating_mode_update_ptr = (voc_evt_operating_mode_update_t*)payload->payload.data_ptr;

               dec_capi_ptr->operating_mode.direction = operating_mode_update_ptr->direction;
               dec_capi_ptr->operating_mode.mode      = operating_mode_update_ptr->mode;

               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_decoder_capi_v2_cb_func: operating mode changed to mode(0x%lx), media_type(0x%lx)",
                     dec_capi_ptr->operating_mode.mode , dec_capi_ptr->common.module_id);

               vdec_t* vdec_ptr = (vdec_t*) dec_capi_ptr->common.svc_ptr;
               vdec_ptr->session_op_mode = dec_capi_ptr->operating_mode.mode;

               vdec_ptr->voc_native_op_mode = vdec_ptr->session_op_mode;

               // If slowtalk is enabled, always report WB operating mode.
               // (note that this does not apply for eAMR or 4GV-NW)
               if ((TRUE  == vdec_ptr->modules.slow_talk.is_enabled)
                     && (VSM_VOC_OPERATING_MODE_WB <  vdec_ptr->session_op_mode))
               {
                  vdec_ptr->session_op_mode = VSM_VOC_OPERATING_MODE_WB;
               }

               // Send mode notification event to upper layer
               vdec_send_mode_notification_v2(dec_capi_ptr->common.svc_ptr);

               break;
            }
            case VOC_EVT_DEC_PKT_OVERWRITTEN:
            {
               voc_evt_dec_pkt_owt_t *erasure_event_ptr = (voc_evt_dec_pkt_owt_t*)payload->payload.data_ptr;

               dec_capi_ptr->erasure = TRUE;
               dec_capi_ptr->packet_size = erasure_event_ptr->packet_size;

               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_decoder_capi_v2_cb_func: received erasure event. packet_size(%ld), media_type(0x%lx)",
                     dec_capi_ptr->packet_size , dec_capi_ptr->common.module_id);
               break;
            }
            case TTY_EVT_RX_TTY_DETECTED:
            {
               rx_tty_detected_t *rx_tty_detected_ptr = (rx_tty_detected_t*)payload->payload.data_ptr;

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_decoder_capi_v2_cb_func: received rx_tty_detected_flag(%ld) event.",
                     rx_tty_detected_ptr->rx_tty_detected_flag);

               if((dec_capi_ptr->common.module_id == VSM_MEDIA_TYPE_EVRC_MODEM)||(dec_capi_ptr->common.module_id == VSM_MEDIA_TYPE_13K_MODEM)||
                  (dec_capi_ptr->common.module_id == VSM_MEDIA_TYPE_4GV_NB_MODEM)||(dec_capi_ptr->common.module_id == VSM_MEDIA_TYPE_4GV_WB_MODEM)||
                  (dec_capi_ptr->common.module_id == VSM_MEDIA_TYPE_4GV_NW_MODEM)||(dec_capi_ptr->common.module_id == VSM_MEDIA_TYPE_4GV_NW)||
                  (dec_capi_ptr->common.module_id == VSM_MEDIA_TYPE_EVRC_NW_2K))
               {
                  vdec_t* vdec_ptr = (vdec_t*) dec_capi_ptr->common.svc_ptr;
                  vdec_ptr->tty_state_ptr->m_rx_tty_detected = rx_tty_detected_ptr->rx_tty_detected_flag;
               }
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_decoder_capi_v2_cb_func: received tty event.");
               break;
            }
            default:
            {
               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_decoder_capi_v2_cb_func: received unsupported param_id(%ld), for event(%ld), media_type(0x%lx)",
                     payload->param_id, (uint32_t)CAPI_V2_EVENT_DATA_TO_DSP_SERVICE, dec_capi_ptr->common.module_id);
               break;
            }
         }
         break;
      }
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_decoder_capi_v2_cb_func: received Unsupported event %d", (int)id);
            return CAPI_V2_EUNSUPPORTED;
         }
   }
   return result;
}

void voice_ecall_rx (vdec_t *vdec_ptr, int16_t *in_out_ptr)
{
   if(NULL == vdec_ptr->modules.hpcm.module_ptr)
   {
	   //return if HPCM is stubbed
	   return;
   }

   int16_t ecall_temp_buffer[VOICE_FRAME_SIZE_FB];   // worst case size: 20ms samples at 48KHz

   // Clear ecall_temp_buffer
   memset (&ecall_temp_buffer[0], 0, sizeof(ecall_temp_buffer));

   // If hpcm read enable, client wants to read data from fw.
   // Ecall interface API's are defined for 14-bit input/output
   // Decoder output is in 16-bit format so convert decoder from 16-bit to 14 bit before giving it to client.
   if ((TRUE == vdec_ptr->modules.hpcm_read_enable) && (VOICE_FRAME_SIZE_FB >= vdec_ptr->frame_samples_dec))
   {
      for (uint32_t i=0; i<vdec_ptr->frame_samples_dec; i++)
      {
         ecall_temp_buffer[i] = in_out_ptr[i] >> 2;   // convert input 16-bit to 14-bit
      }
   }

   // call hpcm CAPI process
   {
      capi_v2_buf_t input_buf;
      capi_v2_stream_data_t input_stream;
      capi_v2_stream_data_t *input_stream_buf[1];
      capi_v2_err_t result = CAPI_V2_EOK;

      input_buf.actual_data_len = vdec_ptr->frame_samples_dec<<1;
      input_buf.max_data_len = VOICE_FRAME_SIZE_FB<<1;
      input_buf.data_ptr = (int8_t*)ecall_temp_buffer;

      input_stream.bufs_num = 1;
      input_stream.buf_ptr = &input_buf;

      input_stream_buf[0] = &input_stream;

      result = vdec_ptr->modules.hpcm.module_ptr->vtbl_ptr->process(vdec_ptr->modules.hpcm.module_ptr,input_stream_buf,input_stream_buf);
      if(CAPI_V2_FAILED(result))
      {
		  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! CAPI resampler process(): ERROR!! result(%#lx)",result);
      }
   }

   // If hpcm write enable, client has wwritten data to fw.
   // Ecall interface API's are defined for 14-bit input/output
   // Ecall output is in 14-bit format so convert output from 14-bit to 16 bit before processing in fw
   if ((TRUE == vdec_ptr->modules.hpcm_write_enable) && (VOICE_FRAME_SIZE_FB >= vdec_ptr->frame_samples_dec))
   {
      for (uint32_t i=0; i<vdec_ptr->frame_samples_dec; i++)
      {
         in_out_ptr[i] = ecall_temp_buffer[i] << 2;   // convert output 14-bit to 16-bit
      }
   }
}

// Call get_static_prop of decoder capi
// Initialize memory for decoder capi
// Call init of decoder capi
ADSPResult vdec_decoder_capi_init(vdec_t *vdec_ptr, uint32_t media_type)
{
   ADSPResult result = ADSP_EOK;
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   // Get amdb handle for decoder capi from amdb
   result = vdec_decoder_capi_get_amdb_handle(vdec_ptr, media_type);

   if(ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_decoder_capi_init: vdec_decoder_capi_get_amdb_handle failed. result(0x%ld)", result);
      return result;
   }

   //call get static function for decoder capi
   vdec_ptr->modules.dec_capi.common.module_id = media_type;
   vdec_ptr->modules.dec_capi.common.mem_size_allocated = 0;
   vdec_ptr->modules.dec_capi.common.svc_ptr = vdec_ptr;

   // update vocoder type
   vdec_ptr->voc_type = media_type;

   uint32_t dec_capi_size = voice_capi_get_static_prop (&vdec_ptr->modules.dec_capi.common);

   if(0 == dec_capi_size)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_decoder_capi_init: voice_capi_get_static_prop returned size(%ld) for media_type(0x%lx)",
            dec_capi_size, vdec_ptr->modules.dec_capi.common.module_id);
      return ADSP_EFAILED;
   }

   //ignore the error for intf extension query
   voice_capi_query_intf_ext(&vdec_ptr->modules.dec_capi.common);

   // allocate decoder capi memory and initialize decoder capi
   if (0 != vdec_ptr->modules.dec_capi.common.mem_size_allocated)
   {
      void *mem_ptr = (int8_t *)qurt_elite_memory_malloc(vdec_ptr->modules.dec_capi.common.mem_size_allocated, QURT_ELITE_HEAP_DEFAULT);

      if (NULL == mem_ptr)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_decoder_capi_init: Out of Memory!!");
         return ADSP_ENOMEMORY;
      }

      // populate init prop list for capi modules
      capi_v2_proplist_t init_props_list;

      capi_v2_port_info_t port_info;
      port_info.is_valid = FALSE;

      capi_v2_event_callback_info_t cb_info = { vdec_decoder_capi_v2_cb_func, NULL };
      capi_v2_heap_id_t heap_id = { (uint32_t)QURT_ELITE_HEAP_DEFAULT };
      capi_v2_port_num_info_t port_num = {1,1}; // populate these accordingly

      // Properties that are set at init
      capi_v2_prop_t init_props[] = {
         { CAPI_V2_HEAP_ID,             { reinterpret_cast<int8_t *>(&heap_id), sizeof(heap_id),  sizeof(heap_id) },   port_info },
         { CAPI_V2_EVENT_CALLBACK_INFO, { reinterpret_cast<int8_t *>(&cb_info), sizeof(cb_info),  sizeof(cb_info) },   port_info },
         { CAPI_V2_PORT_NUM_INFO,       { reinterpret_cast<int8_t *>(&port_num), sizeof(port_num), sizeof(port_num) }, port_info },
      };
      // setup init prop lists
      init_props_list.props_num = sizeof(init_props)/sizeof(capi_v2_prop_t);
      init_props_list.prop_ptr = init_props;

      cb_info.event_context = &vdec_ptr->modules.dec_capi;
      vdec_ptr->modules.dec_capi.common.module_ptr = (capi_v2_t*) mem_ptr;

      //Call capi init.
      capi_result = adsp_amdb_capi_v2_init_f((adsp_amdb_capi_v2_t*)vdec_ptr->modules.dec_capi.common.amdb_handle_ptr, vdec_ptr->modules.dec_capi.common.module_ptr, &init_props_list);

      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_decoder_capi_init: media_type(0x%lx) failed to init with error(%ld).",
               vdec_ptr->modules.dec_capi.common.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
      else
      {
         if(vdec_ptr->modules.dec_capi.common.fwk_extn_ptr)
         {
            //KN: TODO: add framework extension handling here
         }
      }

      //Query data threshold if data buffering is true
      if (vdec_ptr->modules.dec_capi.common.requires_data_buffering)
      {
         vdec_decooder_capi_get_data_threshold(vdec_ptr);
      }
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_decoder_capi_init: dec capi returned size(%ld) for media_type(0x%lx)",
            vdec_ptr->modules.dec_capi.common.mem_size_allocated, vdec_ptr->modules.dec_capi.common.module_id);
      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult vdec_decoder_capi_process(vdec_t* vdec_ptr)
{
   ADSPResult adsp_result = ADSP_EOK;
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   capi_v2_stream_data_t input_data;
   capi_v2_stream_data_t output_data;

   capi_v2_stream_data_t *input = &input_data;
   capi_v2_stream_data_t *output = &output_data;

   capi_v2_buf_t in, out;

   input_data.bufs_num = 1;
   input_data.buf_ptr = &in;

   in.data_ptr = (int8_t*)&vdec_ptr->in_buf;
   in.actual_data_len = vdec_ptr->in_buf_size;
   in.max_data_len = vdec_ptr->modules.dec_capi.in_threshold_in_bytes;

   output_data.bufs_num = 1;
   output_data.buf_ptr = &out;

   out.data_ptr = (int8_t*)&vdec_ptr->dec_out_buf;
   out.max_data_len = vdec_ptr->modules.dec_capi.out_threshold_in_bytes;
   out.actual_data_len = 0;

   if(vdec_ptr->erasure)
   {
      in.actual_data_len = 0; // if packet in not received indicate erasure information to decoder capi process by setting actual_data_len = 0;
      vdec_ptr->in_buf[0] = 0; // also clear the header so that it can be converted to proper erasure header type by decoder CAPI
   }

#if 0// defined(LOG_VOICEPROC_DATA) && defined(SIM)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec session(%d).voc_type(0x%lx) io logging. insize(%ld)",
            vdec_ptr->session_num,vdec_ptr->voc_type,in.actual_data_len);
      {
         char buffer[100]; // The filename buffer.
         // Put output of ith module in ith file
         snprintf(buffer, sizeof(char) * 100, "vdec_session_%x_samp_rate_%d_voc_type_%d_input.raw", vdec_ptr->session_num,vdec_ptr->sampling_rate_dec,vdec_ptr->voc_type);
         static FILE *fp_in = NULL;
         if(fp_in == NULL)
         {
            fp_in = fopen(buffer,"rb"); // in append mode
         }
         int16_t *pIn = (int16_t *)in.data_ptr;
         if(fp_in)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_session. reading (%ld)",in.actual_data_len);
            fread(pIn,sizeof(char),in.actual_data_len,fp_in);
         }
         else
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_session. File logging failed");
         }
      }
   }
#endif

   if (vdec_ptr->modules.dec_capi.common.module_ptr)
   {
      capi_result = vdec_ptr->modules.dec_capi.common.module_ptr->vtbl_ptr->process(vdec_ptr->modules.dec_capi.common.module_ptr, &input, &output);

      if (CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_decoder_capi_process: failed for media_type(0x%lx), result(%ld)",
               vdec_ptr->modules.dec_capi.common.module_id, capi_result);
         adsp_result = capi_v2_err_to_adsp_result(capi_result);
         return adsp_result;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_decoder_capi_process: received bad pointer for media_type(0x%lx)",
            vdec_ptr->modules.dec_capi.common.module_id);
      return ADSP_EFAILED;
   }

   if (out.actual_data_len > out.max_data_len)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_decoder_capi_process: returned output actual_data_len(%ld) more than max_data_len(0%ld)",
            out.actual_data_len, out.max_data_len);
      return ADSP_EFAILED;
   }

#if defined(LOG_VOICEPROC_DATA) && defined(SIM)
   {
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec session(%d).voc_type(0x%lx) io logging. insize(%ld), outsize(%ld)",
            vdec_ptr->session_num,vdec_ptr->voc_type,in.actual_data_len,out.actual_data_len);
      {
         char buffer[100]; // The filename buffer.
         // Put output of ith module in ith file
         snprintf(buffer, sizeof(char) * 100, "vdec_capi_session_%x_samp_rate_%d_voc_type_%d_input.raw", vdec_ptr->session_num,vdec_ptr->sampling_rate_dec,vdec_ptr->voc_type);
         FILE *fp;
         int16_t *pIn = (int16_t *)in.data_ptr;
         fp = fopen(buffer,"ab"); // in append mode
         fwrite(pIn,sizeof(char),in.actual_data_len,fp);
         fclose(fp);
      }
      {
         char buffer[100]; // The filename buffer.
         // Put output of ith module in ith file
         snprintf(buffer, sizeof(char) * 100, "vdec_capi_session_%x_samp_rate_%d_voc_type_%d_output.raw", vdec_ptr->session_num,vdec_ptr->sampling_rate_dec,vdec_ptr->voc_type);
         FILE *fp;
         int16_t *pIn = (int16_t *)out.data_ptr;
         fp = fopen(buffer,"ab"); // in append mode
         fwrite(pIn,sizeof(char),out.actual_data_len,fp);
         fclose(fp);
      }
   }
#endif
   // log vocoder packet
   int16_t *in_ptr = (int16_t*)&vdec_ptr->in_buf;
   uint32_t packet_size = in.actual_data_len;

   // check if capi has detected erasure and updated packet content and packet size information
   if (vdec_ptr->modules.dec_capi.erasure)
   {
      packet_size = vdec_ptr->modules.dec_capi.packet_size;

      vdec_ptr->modules.dec_capi.erasure = FALSE;
      vdec_ptr->modules.dec_capi.packet_size = 0;
   }

   // Don't log Rx packet for PCM_8K, PCM_16K, PCM_32K, PCM_48K, G711_ALAW, G711_MLAW and G729AB media type to be consistent with older implementation.
   if ((VSM_MEDIA_TYPE_PCM_8_KHZ != vdec_ptr->voc_type)
         && (VSM_MEDIA_TYPE_PCM_16_KHZ != vdec_ptr->voc_type)
         && (VSM_MEDIA_TYPE_PCM_32_KHZ != vdec_ptr->voc_type)
         && (VSM_MEDIA_TYPE_PCM_48_KHZ != vdec_ptr->voc_type)
         && (VSM_MEDIA_TYPE_G711_ALAW != vdec_ptr->voc_type)
         && (VSM_MEDIA_TYPE_G711_MLAW != vdec_ptr->voc_type)
         && (VSM_MEDIA_TYPE_G729AB != vdec_ptr->voc_type))
   {
      (void)vdec_log_vocoder_packet (vdec_ptr->voc_type, vdec_ptr->sampling_rate_dec, ((vdec_ptr->sampling_rate_dec == 16000) << 3) | vdec_ptr->session_num, in_ptr, packet_size);
   }

   return adsp_result;
}

ADSPResult vdec_decoder_capi_destroy(vdec_t* vdec_ptr)
{
   if(vdec_ptr->modules.dec_capi.common.module_ptr)
   {
      if(vdec_ptr->modules.dec_capi.common.module_ptr->vtbl_ptr)
      {
         vdec_ptr->modules.dec_capi.common.module_ptr->vtbl_ptr->end(vdec_ptr->modules.dec_capi.common.module_ptr);
         vdec_ptr->modules.dec_capi.common.module_ptr->vtbl_ptr = NULL;
      }
      qurt_elite_memory_free (vdec_ptr->modules.dec_capi.common.module_ptr);
      vdec_ptr->modules.dec_capi.common.module_ptr = NULL;
   }
   if(vdec_ptr->modules.dec_capi.amdb_handle_info.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&vdec_ptr->modules.dec_capi.amdb_handle_info, 1);
      vdec_ptr->modules.dec_capi.amdb_handle_info.h.capi_v2_handle = NULL;
   }
   if(vdec_ptr->modules.dec_capi.common.fwk_extn_ptr)
   {
      qurt_elite_memory_free(vdec_ptr->modules.dec_capi.common.fwk_extn_ptr);
      vdec_ptr->modules.dec_capi.common.fwk_extn_ptr = NULL;
   }

   // Need to reset all the service level variables for decoder capi.
   memset(&vdec_ptr->modules.dec_capi, 0, sizeof(vdec_ptr->modules.dec_capi));

   vdec_ptr->voc_type = VSM_MEDIA_TYPE_NONE;
   vdec_ptr->modules.is_decoder_capi = FALSE;

   return ADSP_EOK;
}

ADSPResult vdec_decoder_capi_get_amdb_handle(vdec_t* vdec_ptr, uint32_t media_type)
{
   // Fill the structure to query the handles from AMDB
   vdec_ptr->modules.dec_capi.amdb_handle_info.interface_type = CAPI_V2;
   vdec_ptr->modules.dec_capi.amdb_handle_info.type = AMDB_MODULE_TYPE_DECODER;
   vdec_ptr->modules.dec_capi.amdb_handle_info.id1 = media_type;
   vdec_ptr->modules.dec_capi.amdb_handle_info.id2 = 0;
   vdec_ptr->modules.dec_capi.amdb_handle_info.h.capi_v2_handle = NULL;
   vdec_ptr->modules.dec_capi.amdb_handle_info.result = ADSP_EFAILED;

   adsp_amdb_get_modules_request(&vdec_ptr->modules.dec_capi.amdb_handle_info, 1, NULL, NULL);

   if(ADSP_FAILED(vdec_ptr->modules.dec_capi.amdb_handle_info.result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: vdec_get_amdb_handle: failed to get AMDB handle for media_type(0x%lx)", media_type);
      return vdec_ptr->modules.dec_capi.amdb_handle_info.result;
   }

   if ((CAPI_V2 != vdec_ptr->modules.dec_capi.amdb_handle_info.interface_type) && (STUB != vdec_ptr->modules.dec_capi.amdb_handle_info.interface_type))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: AMDB handle for media_type(0x%lx), has interface type(%d), when expected type(%d).",
            media_type, (int)vdec_ptr->modules.dec_capi.amdb_handle_info.interface_type, (int)CAPI_V2);
      return ADSP_EUNSUPPORTED;
   }

   if (STUB != vdec_ptr->modules.dec_capi.amdb_handle_info.interface_type)
   {
      vdec_ptr->modules.dec_capi.common.amdb_handle_ptr = (void*)vdec_ptr->modules.dec_capi.amdb_handle_info.h.capi_v2_handle;
   }
   else
   {
      vdec_ptr->modules.dec_capi.common.is_virtual_stub = TRUE;

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_decoder_capi_get_amdb_handle: media_type(0x%lx) is virtually stubbed",
            vdec_ptr->modules.dec_capi.common.module_id);
      return ADSP_EUNSUPPORTED;
   }
   return ADSP_EOK;
}

ADSPResult vdec_stop_start_modules (vdec_t* vdec_ptr, uint32_t state)
{
   return ADSP_EOK;
}


ADSPResult vdec_decooder_capi_get_data_threshold (vdec_t* vdec_ptr)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   capi_v2_port_info_t in_port_info, out_port_info;
   capi_v2_port_data_threshold_t in_data_threshold, out_data_threshold;

   in_port_info.is_valid = TRUE;
   in_port_info.is_input_port = TRUE;
   in_port_info.port_index = 0;

   out_port_info.is_valid = TRUE;
   out_port_info.is_input_port = FALSE;
   out_port_info.port_index = 0;

   capi_v2_prop_t get_props[] = {
         { CAPI_V2_PORT_DATA_THRESHOLD,   { reinterpret_cast<int8_t *>(&in_data_threshold),  sizeof(in_data_threshold),  sizeof(in_data_threshold)  },  in_port_info  },
         { CAPI_V2_PORT_DATA_THRESHOLD,   { reinterpret_cast<int8_t *>(&out_data_threshold), sizeof(out_data_threshold), sizeof(out_data_threshold) },  out_port_info },
   };

   capi_v2_proplist_t get_proplist = { VOICE_SIZE_OF_ARRAY(get_props), get_props };

   capi_result = vdec_ptr->modules.dec_capi.common.module_ptr->vtbl_ptr->get_properties(vdec_ptr->modules.dec_capi.common.module_ptr, &get_proplist);

   if (CAPI_V2_EOK != capi_result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_decooder_capi_get_data_threshold: get_prop failed for decoder capi media_type(0x%ld)",
            vdec_ptr->modules.dec_capi.common.module_id);
      return capi_v2_err_to_adsp_result(capi_result);
   }
   vdec_ptr->modules.dec_capi.in_threshold_in_bytes = in_data_threshold.threshold_in_bytes;
   vdec_ptr->modules.dec_capi.out_threshold_in_bytes = out_data_threshold.threshold_in_bytes;
   return ADSP_EOK;
}

ADSPResult vdec_decoder_capi_set_output_media_fmt(vdec_t *vdec_ptr, uint32_t sampling_rate)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   if(NULL == vdec_ptr->modules.dec_capi.common.module_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_decoder_capi_set_output_media_fmt: NULL module pointer error");
      return ADSP_EBADPARAM;
   }
   // populate init prop list for capi modules
   capi_v2_proplist_t init_props_list;

   capi_v2_port_info_t port_info;
   port_info.is_valid = TRUE;
   port_info.is_input_port = FALSE;
   port_info.port_index = 0;

   //update input mediatype
   vdec_ptr->modules.dec_capi.output_media_type.media_format.format_header.data_format = (data_format_t)CAPI_V2_DATA_FORMAT_INVALID_VAL;

   vdec_ptr->modules.dec_capi.output_media_type.data_format.bitstream_format  = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   vdec_ptr->modules.dec_capi.output_media_type.data_format.num_channels      = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   vdec_ptr->modules.dec_capi.output_media_type.data_format.bits_per_sample   = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   vdec_ptr->modules.dec_capi.output_media_type.data_format.data_is_signed    = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   vdec_ptr->modules.dec_capi.output_media_type.data_format.data_interleaving = CAPI_V2_INVALID_INTERLEAVING;
   vdec_ptr->modules.dec_capi.output_media_type.data_format.q_factor          = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   vdec_ptr->modules.dec_capi.output_media_type.data_format.sampling_rate = sampling_rate;

   vdec_ptr->modules.dec_capi.output_media_type.data_format.channel_type[0] = (uint16_t)CAPI_V2_DATA_FORMAT_INVALID_VAL;

   // Properties that are set at init
   capi_v2_prop_t init_props[] = {
         { CAPI_V2_OUTPUT_MEDIA_FORMAT,  { reinterpret_cast<int8_t *>(&vdec_ptr->modules.dec_capi.output_media_type),sizeof(vdec_ptr->modules.dec_capi.output_media_type) }, port_info }
   };

   // setup init prop lists
   init_props_list.props_num = sizeof(init_props)/sizeof(capi_v2_prop_t);
   init_props_list.prop_ptr = init_props;

   //Call capi init.
   capi_result = vdec_ptr->modules.dec_capi.common.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.dec_capi.common.module_ptr, &init_props_list);
   if(CAPI_V2_EOK != capi_result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_decoder_capi_set_output_media_fmt: media_type(0x%lx) failed to send output media fmt with error(%ld).",
            vdec_ptr->modules.dec_capi.common.module_id, capi_result);
      return capi_v2_err_to_adsp_result(capi_result);
   }

   return ADSP_EOK;
}

static ADSPResult vdec_stream_pp_capi_create(vdec_t* vdec_ptr)
{
   ADSPResult result = ADSP_EOK;

   // update module ids for stream pp modules
   vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_id = VOICE_MODULE_CTM_RX;
   vdec_ptr->modules.ltetty_rx.module_id = VOICE_MODULE_LTETTY_RX;
   vdec_ptr->modules.dtmf_det.module_id = VOICE_MODULE_DTMF_DETECTION;
   vdec_ptr->modules.slow_talk.module_id = VOICE_MODULE_ST;
   vdec_ptr->modules.soft_mute.soft_mute_capi.module_id = VOICE_MODULE_SOFT_MUTE;
   vdec_ptr->modules.hpcm.module_id = VOICE_MODULE_HPCM_STREAM_RX;

   //check stream modules for capi or not capi
   vdec_ptr->modules.ctm_rx.ctm_rx_capi.is_capi = vdec_check_stream_pp_capi(vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_id);
   vdec_ptr->modules.ltetty_rx.is_capi = vdec_check_stream_pp_capi(vdec_ptr->modules.ltetty_rx.module_id);
   vdec_ptr->modules.dtmf_det.is_capi = vdec_check_stream_pp_capi(vdec_ptr->modules.dtmf_det.module_id);
   vdec_ptr->modules.slow_talk.is_capi = vdec_check_stream_pp_capi(vdec_ptr->modules.slow_talk.module_id);
   vdec_ptr->modules.soft_mute.soft_mute_capi.is_capi = vdec_check_stream_pp_capi(vdec_ptr->modules.soft_mute.soft_mute_capi.module_id);
   vdec_ptr->modules.hpcm.is_capi = vdec_check_stream_pp_capi(vdec_ptr->modules.hpcm.module_id);

   // update service pointer in capi instance structures.
   vdec_ptr->modules.ctm_rx.ctm_rx_capi.svc_ptr = vdec_ptr;
   vdec_ptr->modules.ltetty_rx.svc_ptr = vdec_ptr;
   vdec_ptr->modules.dtmf_det.svc_ptr = vdec_ptr;
   vdec_ptr->modules.slow_talk.svc_ptr = vdec_ptr;
   vdec_ptr->modules.soft_mute.soft_mute_capi.svc_ptr = vdec_ptr;
   vdec_ptr->modules.hpcm.svc_ptr = vdec_ptr;

   // get amdb handle for stream pp capi modules
   result = vdec_stream_pp_capi_get_amdb_handle(vdec_ptr);
   if (ADSP_EOK != result)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_stream_pp_capi_create: vdec_stream_pp_capi_get_amdb_handle: failed");
      return result;
   }

   // get static properties of stream pp capi modules
   result = vdec_stream_pp_capi_get_static_property(vdec_ptr);
   if (ADSP_EOK != result)
   {
      return result;
   }

   // query interface extension for stream pp capi modules. Ignore error
   vdec_stream_pp_capi_query_intf_extn(vdec_ptr);

   // allocate memory for stream pp capi modules
   vdec_ptr->modules.total_size = vdec_ptr->modules.ctm_rx.ctm_rx_size
                                  + vdec_ptr->modules.ltetty_rx_size
                                  + vdec_ptr->modules.dtmf_det_size
                                  + vdec_ptr->modules.slow_talk_size
                                  + vdec_ptr->modules.soft_mute.soft_mute_size
                                  + vdec_ptr->modules.hpcm_size;

   vdec_ptr->modules.start_addr_ptr = (int8_t*)qurt_elite_memory_malloc(vdec_ptr->modules.total_size, QURT_ELITE_HEAP_DEFAULT);

   if (NULL == vdec_ptr->modules.start_addr_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_stream_pp_capi_init: Failed to allocate memory for vdec stream pp capi modules. total_size(%ld)",
            vdec_ptr->modules.total_size);
      return ADSP_ENOMEMORY;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_stream_pp_capi_init: allocated total_size(%ld) for stream pp capi modules", vdec_ptr->modules.total_size);

   // initialize stream capi modules.
   result = vdec_stream_pp_capi_init(vdec_ptr);

   return result;
}

static ADSPResult vdec_stream_pp_capi_get_amdb_handle(vdec_t* vdec_ptr)
{
   ADSPResult result = ADSP_EOK;

   // Get amdb handle for vdec stram pp capi modules
   // for now call get handle function sequentially for all the stream pp modules.
   // TODO: to update following routine with the loop once all the stream pp modules are updated to capi.

   // get amdb handle for ctm rx capi module
   result |= vdec_capi_get_amdb_handle(&vdec_ptr->modules.ctm_rx.ctm_rx_capi, &vdec_ptr->modules.ctm_rx.ctm_rx_amdb);

   // get amdb handle for ltetty rx capi module
   result |= vdec_capi_get_amdb_handle(&vdec_ptr->modules.ltetty_rx, &vdec_ptr->modules.ltetty_rx_amdb);

   // get amdb handle for DTMF DET capi module
   result |= vdec_capi_get_amdb_handle(&vdec_ptr->modules.dtmf_det, &vdec_ptr->modules.dtmf_det_amdb);

   // get amdb handle for slow talk capi module
   result |= vdec_capi_get_amdb_handle(&vdec_ptr->modules.slow_talk, &vdec_ptr->modules.slow_talk_amdb);

   // get amdb handle for soft mute capi module
   result |= vdec_capi_get_amdb_handle(&vdec_ptr->modules.soft_mute.soft_mute_capi, &vdec_ptr->modules.soft_mute.soft_mute_amdb);

   // get amdb handle for hpcm capi module
   result |= vdec_capi_get_amdb_handle(&vdec_ptr->modules.hpcm, &vdec_ptr->modules.hpcm_amdb);

   return result;
}

ADSPResult vdec_capi_get_amdb_handle(voice_capi_module_t *module_info_ptr, adsp_amdb_module_handle_info_t *amdb_handle_info_ptr)
{
   if ((NULL == module_info_ptr) || (NULL == amdb_handle_info_ptr))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_capi_get_amdb_handle: received NULL pointer for module_info_ptr(0x%p) or amdb_handle_info_ptr(0x%p)",
            module_info_ptr, amdb_handle_info_ptr);
      return ADSP_EFAILED;
   }

   // Fill the structure to query the handles from AMDB
   amdb_handle_info_ptr->interface_type = CAPI_V2;
   amdb_handle_info_ptr->type = AMDB_MODULE_TYPE_GENERIC;
   amdb_handle_info_ptr->id1 = module_info_ptr->module_id;
   amdb_handle_info_ptr->id2 = 0;
   amdb_handle_info_ptr->h.capi_v2_handle = NULL;
   amdb_handle_info_ptr->result = ADSP_EFAILED;

   adsp_amdb_get_modules_request(amdb_handle_info_ptr, 1, NULL, NULL);

   if(ADSP_FAILED(amdb_handle_info_ptr->result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_capi_get_amdb_handle: failed to get AMDB handle for module_id(0x%lx)", module_info_ptr->module_id);
      return amdb_handle_info_ptr->result;
   }

   if ((CAPI_V2 != amdb_handle_info_ptr->interface_type) && (STUB != amdb_handle_info_ptr->interface_type))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_capi_get_amdb_handle: AMDB handle for module_id(0x%lx) has interface type(%d), when expected type(%d).",
            module_info_ptr->module_id, (int)amdb_handle_info_ptr->interface_type, (int)CAPI_V2);
      return ADSP_EUNSUPPORTED;
   }

   if (STUB != amdb_handle_info_ptr->interface_type)
   {
      module_info_ptr->amdb_handle_ptr = (void*)amdb_handle_info_ptr->h.capi_v2_handle;
   }
   else
   {
      module_info_ptr->is_virtual_stub = TRUE;
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_capi_get_amdb_handle: module_id(0x%lx) is virtually stubbed", module_info_ptr->module_id);
   }
   return ADSP_EOK;
}

static ADSPResult vdec_stream_pp_capi_get_static_property(vdec_t* vdec_ptr)
{
   ADSPResult result = ADSP_EOK;

   // Get static properties for vdec stram pp capi modules
   // for now call get static property function sequentially for all the stream pp modules.
   // TODO: to update following routine with the loop once all the stream pp modules are updated to capi.

   if(!vdec_ptr->modules.ctm_rx.ctm_rx_capi.is_virtual_stub)
   {
      vdec_ptr->modules.ctm_rx.ctm_rx_size = voice_capi_get_static_prop (&vdec_ptr->modules.ctm_rx.ctm_rx_capi);
      if(0 == vdec_ptr->modules.ctm_rx.ctm_rx_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               vdec_ptr->modules.ctm_rx.ctm_rx_size, vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_id);
         result |= ADSP_EFAILED;
      }
   }

   if(!vdec_ptr->modules.ltetty_rx.is_virtual_stub)
   {
      vdec_ptr->modules.ltetty_rx_size = voice_capi_get_static_prop (&vdec_ptr->modules.ltetty_rx);
      if(0 == vdec_ptr->modules.ltetty_rx_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               vdec_ptr->modules.ltetty_rx_size, vdec_ptr->modules.ltetty_rx.module_id);
         result |= ADSP_EFAILED;
      }
   }

   if(!vdec_ptr->modules.dtmf_det.is_virtual_stub)
   {
      vdec_ptr->modules.dtmf_det_size = voice_capi_get_static_prop (&vdec_ptr->modules.dtmf_det);
      if(0 == vdec_ptr->modules.dtmf_det_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               vdec_ptr->modules.dtmf_det_size, vdec_ptr->modules.dtmf_det.module_id);
         result |= ADSP_EFAILED;
      }
   }

   if(!vdec_ptr->modules.slow_talk.is_virtual_stub)
   {
      vdec_ptr->modules.slow_talk_size = voice_capi_get_static_prop (&vdec_ptr->modules.slow_talk);
      if(0 == vdec_ptr->modules.slow_talk_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               vdec_ptr->modules.slow_talk_size, vdec_ptr->modules.slow_talk.module_id);
         result |= ADSP_EFAILED;
      }
   }

   if(!vdec_ptr->modules.soft_mute.soft_mute_capi.is_virtual_stub)
   {
      vdec_ptr->modules.soft_mute.soft_mute_size = voice_capi_get_static_prop (&vdec_ptr->modules.soft_mute.soft_mute_capi);
      if(0 == vdec_ptr->modules.soft_mute.soft_mute_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               vdec_ptr->modules.soft_mute.soft_mute_size, vdec_ptr->modules.soft_mute.soft_mute_capi.module_id);
         result |= ADSP_EFAILED;
      }
   }

   if(!vdec_ptr->modules.hpcm.is_virtual_stub)
   {
      vdec_ptr->modules.hpcm_size = voice_capi_get_static_prop (&vdec_ptr->modules.hpcm);
      if(0 == vdec_ptr->modules.hpcm_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               vdec_ptr->modules.hpcm_size, vdec_ptr->modules.hpcm.module_id);
         result |= ADSP_EFAILED;
      }
   }

   return result;
}

static ADSPResult vdec_stream_pp_capi_query_intf_extn(vdec_t* vdec_ptr)
{
   ADSPResult result = ADSP_EOK;

   // query interface extn for vdec stram pp capi modules
   // for now query interface extn sequentially for all the stream pp modules.
   // TODO: to update following routine with the loop once all the stream pp modules are updated to capi.

   result |= voice_capi_query_intf_ext(&vdec_ptr->modules.ctm_rx.ctm_rx_capi);
   result |= voice_capi_query_intf_ext(&vdec_ptr->modules.ltetty_rx);
   return result;
}

static ADSPResult vdec_stream_pp_capi_init(vdec_t* vdec_ptr)
{
   ADSPResult result = ADSP_EOK;

   // populate init prop list for capi modules
   capi_v2_proplist_t init_props_list;

   capi_v2_port_info_t port_info;
   port_info.is_valid = FALSE;
   port_info.port_index = 0;
   port_info.is_input_port = FALSE;

   capi_v2_event_callback_info_t cb_info = { vdec_stream_pp_capi_cb_func, NULL };
   capi_v2_heap_id_t heap_id = { (uint32_t)QURT_ELITE_HEAP_DEFAULT };
   capi_v2_port_num_info_t port_num = {1,1}; // populate these accordingly

   // Properties that are set at init
   capi_v2_prop_t init_props[] = {
         { CAPI_V2_HEAP_ID,             { reinterpret_cast<int8_t *>(&heap_id), sizeof(heap_id),  sizeof(heap_id) },   port_info },
         { CAPI_V2_EVENT_CALLBACK_INFO, { reinterpret_cast<int8_t *>(&cb_info), sizeof(cb_info),  sizeof(cb_info) },   port_info },
         { CAPI_V2_PORT_NUM_INFO,       { reinterpret_cast<int8_t *>(&port_num), sizeof(port_num), sizeof(port_num) }, port_info },
   };

   // setup init prop lists
   init_props_list.props_num = sizeof(init_props)/sizeof(capi_v2_prop_t);
   init_props_list.prop_ptr = init_props;

   if(vdec_ptr->modules.start_addr_ptr)
   {
      vdec_ptr->modules.usage_addr_ptr = vdec_ptr->modules.start_addr_ptr;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_init: received NULL pointer");
      return ADSP_EFAILED;
   }

   if(vdec_ptr->modules.ctm_rx.ctm_rx_size)
   {
      cb_info.event_context = &vdec_ptr->modules.ctm_rx.ctm_rx_capi;
      vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr = (capi_v2_t*) vdec_ptr->modules.usage_addr_ptr;
      vdec_stream_pp_capi_init_lib(vdec_ptr, &vdec_ptr->modules.ctm_rx.ctm_rx_capi, &init_props_list);
      vdec_ptr->modules.usage_addr_ptr += vdec_ptr->modules.ctm_rx.ctm_rx_size;
   }

   if(vdec_ptr->modules.ltetty_rx_size)
   {
      cb_info.event_context = &vdec_ptr->modules.ltetty_rx;
      vdec_ptr->modules.ltetty_rx.module_ptr = (capi_v2_t*) vdec_ptr->modules.usage_addr_ptr;
      vdec_stream_pp_capi_init_lib(vdec_ptr, &vdec_ptr->modules.ltetty_rx, &init_props_list);
      vdec_ptr->modules.usage_addr_ptr += vdec_ptr->modules.ltetty_rx_size;
   }

   if(vdec_ptr->modules.dtmf_det_size)
   {
      cb_info.event_context = &vdec_ptr->modules.dtmf_det;
      vdec_ptr->modules.dtmf_det.module_ptr = (capi_v2_t*) vdec_ptr->modules.usage_addr_ptr;
      vdec_stream_pp_capi_init_lib(vdec_ptr, &vdec_ptr->modules.dtmf_det, &init_props_list);
      vdec_ptr->modules.usage_addr_ptr += vdec_ptr->modules.dtmf_det_size;
   }

   if(vdec_ptr->modules.slow_talk_size)
   {
      cb_info.event_context = &vdec_ptr->modules.slow_talk;
      vdec_ptr->modules.slow_talk.module_ptr = (capi_v2_t*) vdec_ptr->modules.usage_addr_ptr;
      vdec_stream_pp_capi_init_lib(vdec_ptr, &vdec_ptr->modules.slow_talk, &init_props_list);
      vdec_ptr->modules.usage_addr_ptr += vdec_ptr->modules.slow_talk_size;
   }

   if(vdec_ptr->modules.soft_mute.soft_mute_size)
   {
      cb_info.event_context = &vdec_ptr->modules.soft_mute.soft_mute_capi;
      vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr = (capi_v2_t*) vdec_ptr->modules.usage_addr_ptr;
      vdec_stream_pp_capi_init_lib(vdec_ptr, &vdec_ptr->modules.soft_mute.soft_mute_capi, &init_props_list);
      vdec_ptr->modules.usage_addr_ptr += vdec_ptr->modules.soft_mute.soft_mute_size;
   }

   if(vdec_ptr->modules.hpcm_size)
   {
      cb_info.event_context = &vdec_ptr->modules.hpcm;
      vdec_ptr->modules.hpcm.module_ptr = (capi_v2_t*) vdec_ptr->modules.usage_addr_ptr;
      vdec_stream_pp_capi_init_lib(vdec_ptr, &vdec_ptr->modules.hpcm, &init_props_list);
      vdec_ptr->modules.usage_addr_ptr += vdec_ptr->modules.hpcm_size;
   }

   // Sanity check to see the allocated memory was enough
   if ((uint32_t)(vdec_ptr->modules.usage_addr_ptr - vdec_ptr->modules.start_addr_ptr) <= vdec_ptr->modules.total_size)
   {
      result = ADSP_EOK;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_stream_pp_capi_init: Error allocating memory: usage_size(%ld), allocated_size(%ld)",
            (uint32_t)(vdec_ptr->modules.usage_addr_ptr - vdec_ptr->modules.start_addr_ptr), vdec_ptr->modules.total_size);
      result = ADSP_ENOMEMORY;
   }
   return result;
}

static ADSPResult vdec_stream_pp_capi_init_lib(vdec_t* vdec_ptr, voice_capi_module_t *module_info_ptr, capi_v2_proplist_t *init_prop_list_ptr)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   capi_result = adsp_amdb_capi_v2_init_f((adsp_amdb_capi_v2_t*)module_info_ptr->amdb_handle_ptr, module_info_ptr->module_ptr, init_prop_list_ptr);

   if(CAPI_V2_EOK != capi_result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_init_lib: failed for module_id(0x%lx), capi_result(%ld)",
            module_info_ptr->module_id, capi_result);
      module_info_ptr->module_ptr = NULL;
      module_info_ptr->is_enabled = FALSE;
      return capi_v2_err_to_adsp_result(capi_result);
   }
   else
   {
      if(module_info_ptr->fwk_extn_ptr)
      {
         for(uint32_t i = 0; i < module_info_ptr->num_extensions.num_extensions; i++)
         {
            switch(module_info_ptr->fwk_extn_ptr[i].id)
            {
               case FWK_EXTN_HPCM:
               {
                  //just updating the pointer.
                  //hanlding will be done after the module is created.
                  vdec_ptr->modules.special_capis[HPCM] = module_info_ptr;
                  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_stream_pp_capi_init_lib: module(0x%lx) supports fwk_extn(0x%lx)",
                        module_info_ptr->module_id, module_info_ptr->fwk_extn_ptr[i].id);
                  break;
               }
               default:
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_stream_pp_capi_init_lib: received unsupported fwk_extn_id: 0x%lx",
                        module_info_ptr->fwk_extn_ptr[i].id);
                  break;
               }
            }
         }
      }
   }
   return ADSP_EOK;
}

capi_v2_err_t vdec_stream_pp_capi_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr)
{
   capi_v2_err_t result = CAPI_V2_EOK;
   voice_capi_module_t* module_info_ptr = (voice_capi_module_t*)context_ptr;
   vdec_t *vdec_ptr = (vdec_t*)module_info_ptr->svc_ptr;

   if(NULL == context_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_stream_pp_capi_cb_func: received bad pointer");
      return CAPI_V2_EFAILED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vdec_stream_pp_capi_cb_func: received event_id(%d), module_id(0x%lx)",
         (int)id, module_info_ptr->module_id);
   switch(id)
   {
      case CAPI_V2_EVENT_KPPS:
      {
         capi_v2_event_KPPS_t* kpps_ptr = (capi_v2_event_KPPS_t* )event_info_ptr->payload.data_ptr;
         module_info_ptr->kpps = kpps_ptr->KPPS;
         break;
      }
      case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
      {
         capi_v2_event_algorithmic_delay_t* delay_ptr = (capi_v2_event_algorithmic_delay_t* )event_info_ptr->payload.data_ptr;
         module_info_ptr->delay = delay_ptr->delay_in_us;
         break;
      }
      case CAPI_V2_EVENT_GET_LIBRARY_INSTANCE:
      {
         if(event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_get_library_instance_t))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_stream_pp_capi_cb_func: received insufficient size for get library instance, size(0x%lx)",
                  event_info_ptr->payload.actual_data_len);
            return CAPI_V2_EBADPARAM;
         }

         capi_v2_event_get_library_instance_t* get_library_instance = (capi_v2_event_get_library_instance_t*)event_info_ptr->payload.data_ptr;;

         result = capi_v2_library_factory_get_instance(get_library_instance->id, &get_library_instance->ptr);
         if(CAPI_V2_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_stream_pp_capi_cb_func: failed to get library instance with lib_id(0x%x), payload_ptr(0x%p)",
                  get_library_instance->id,get_library_instance->ptr);
            return result;
         }
         break;
      }
      case CAPI_V2_EVENT_PROCESS_STATE:
      {
         uint32_t prev_is_enabled = module_info_ptr->is_enabled;
         module_info_ptr->is_enabled = ((capi_v2_event_process_state_t *)(event_info_ptr->payload.data_ptr))->is_enabled;

         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_stream_pp_capi_cb_func: module_id(0x%lx) process state changed from (%lu) to (%lu)",
               module_info_ptr->module_id, prev_is_enabled, module_info_ptr->is_enabled);

         //If module is slow talk then limit the operating bandwidth to WB
         if(VOICE_MODULE_ST == module_info_ptr->module_id)
         {
            vdec_t *vdec_ptr = (vdec_t*)module_info_ptr->svc_ptr;

            //update session op_mode with vocoder native op mode.
            uint32_t session_op_mode = vdec_ptr->voc_native_op_mode;

            // If slowtalk is enabled, always report WB operating mode.
            // (note that this does not apply for eAMR or 4GV-NW)
            if ((TRUE  == vdec_ptr->modules.slow_talk.is_enabled)
                  && (VSM_VOC_OPERATING_MODE_WB <  vdec_ptr->session_op_mode))
            {
               session_op_mode = VSM_VOC_OPERATING_MODE_WB;
            }

            // Send mode notification event to upper layer if changed
            if(session_op_mode != vdec_ptr->session_op_mode)
            {
               vdec_ptr->session_op_mode = session_op_mode;
               vdec_send_mode_notification_v2(module_info_ptr->svc_ptr);
            }
         }

         break;
      }
      case CAPI_V2_EVENT_DATA_TO_DSP_SERVICE:
      {

         if(event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_data_to_dsp_service_t))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_stream_pp_capi_cb_func: event_id(0x%lx): received bad param size (%ld)",
                  (uint32_t)CAPI_V2_EVENT_DATA_TO_DSP_SERVICE, event_info_ptr->payload.actual_data_len);
            return CAPI_V2_ENEEDMORE;
         }

         capi_v2_event_data_to_dsp_service_t *payload = (capi_v2_event_data_to_dsp_service_t* )event_info_ptr->payload.data_ptr;

         switch(payload->param_id)
         {
            case TTY_EVT_LTETTY_RX_CHAR_ACCEPTED:
            {
               tty_char_t *tty_char_ptr = (tty_char_t*)payload->payload.data_ptr;

               // log tty rx character recevied for ltetty rx processing
               vdec_log_tty_rx_char(vdec_ptr, tty_char_ptr->tty_char);

               // send tty character acceptance event to uppar layer
               result = vdec_send_oobtty_char_accepted(vdec_ptr);
               if (ADSP_FAILED(result))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_cb_func: TTY_EVT_LTETTY_RX_CHAR_ACCEPTED failed.");
               }
               break;
            }
            case TTY_EVT_RX_TTY_DETECTED:
            {
               rx_tty_detected_t *rx_tty_detected_ptr = (rx_tty_detected_t*)payload->payload.data_ptr;

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_stream_pp_capi_cb_func: received rx_tty_detected_flag(%ld) event.",
                     rx_tty_detected_ptr->rx_tty_detected_flag);

               if (VOICE_MODULE_LTETTY_RX == module_info_ptr->module_id)
               {
                  vdec_ptr->tty_state_ptr->m_rx_oobtty_detected = rx_tty_detected_ptr->rx_tty_detected_flag;
               }
               if (VOICE_MODULE_CTM_RX == module_info_ptr->module_id)
               {
                  vdec_ptr->tty_state_ptr->m_rx_tty_detected = rx_tty_detected_ptr->rx_tty_detected_flag;
               }
               break;
            }
            case TTY_EVT_CHAR_DETECTED:
            {
               if (VOICE_MODULE_CTM_RX == module_info_ptr->module_id)
               {
                  tty_char_t *tty_char_ptr = (tty_char_t*)payload->payload.data_ptr;

                  // log tty tx character for ctm tx
                  vdec_log_ctm_tty_rx_char(vdec_ptr, tty_char_ptr->tty_char);
               }
               break;
            }
            case TTY_EVT_CTM_RX_ENQUIRY_DETECTED:
            {
               vdec_ptr->tty_state_ptr->m_enquiry_from_far_end_detected = TRUE;
               break;
            }
            case TTY_EVT_CTM_RX_DETECTED:
            {
               if(FALSE == vdec_ptr->modules.ctm_rx.ctm_from_far_end_detect_notified)
               {
                  vdec_ptr->tty_state_ptr->m_ctm_from_far_end_detected = TRUE;
                  vdec_ptr->modules.ctm_rx.ctm_from_far_end_detect_notified = TRUE;
               }
               break;
            }
            case PARAM_ID_DTMF_TONE_DETECTED:
            {
               vdec_send_vsm_dtmf_tone_status(vdec_ptr, payload->payload.data_ptr);
               break;
            }
            case FWK_EXTN_EVENT_ID_HPCM_HOST_BUF_AVAILABLE:
            {
               vdec_send_hpcm_buf_available_evt(vdec_ptr, payload->payload.data_ptr);
               break;
            }
            default:
            {
               break;
            }
         }
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vdec_stream_pp_capi_cb_func: received unsupported event(%d)", (int)id);
      }
   }
   return result;
}

static ADSPResult vdec_stream_pp_capi_set_input_media_fmt(vdec_t *vdec_ptr)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   // populate init prop list for capi modules
   capi_v2_proplist_t init_props_list;

   capi_v2_port_info_t port_info;
   port_info.is_valid = FALSE;
   port_info.port_index = 0;
   port_info.is_input_port = FALSE;

   //update input mediatype
   vdec_ptr->modules.input_media_type.media_format.format_header.data_format = CAPI_V2_FIXED_POINT;

   vdec_ptr->modules.input_media_type.data_format.bitstream_format  = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   vdec_ptr->modules.input_media_type.data_format.num_channels      = 1;
   vdec_ptr->modules.input_media_type.data_format.bits_per_sample   = 16;
   vdec_ptr->modules.input_media_type.data_format.data_is_signed    = TRUE;
   vdec_ptr->modules.input_media_type.data_format.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
   vdec_ptr->modules.input_media_type.data_format.q_factor          = 15;
   vdec_ptr->modules.input_media_type.data_format.sampling_rate     = vdec_ptr->sampling_rate_dec;

   vdec_ptr->modules.input_media_type.data_format.channel_type[0] = (uint16_t)PCM_CHANNEL_PRI_MIC;

   // Properties that are set at init
   capi_v2_prop_t init_props[] = {
         { CAPI_V2_INPUT_MEDIA_FORMAT,  { reinterpret_cast<int8_t *>(&vdec_ptr->modules.input_media_type),sizeof(vdec_ptr->modules.input_media_type), sizeof(vdec_ptr->modules.input_media_type) }, port_info }
   };

   // setup init prop lists
   init_props_list.props_num = sizeof(init_props)/sizeof(capi_v2_prop_t);
   init_props_list.prop_ptr = init_props;

   // send updated input media format to hpcm capi
   if(vdec_ptr->modules.hpcm.module_ptr)
   {
      capi_result = vdec_ptr->modules.hpcm.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.hpcm.module_ptr, &init_props_list);
      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               vdec_ptr->modules.hpcm.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send updated input media format to ctm rx capi
   if(vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr)
   {
      capi_result = vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr, &init_props_list);
      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send updated input media format to ltetty rx capi
   if(vdec_ptr->modules.ltetty_rx.module_ptr)
   {
      capi_result = vdec_ptr->modules.ltetty_rx.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.ltetty_rx.module_ptr, &init_props_list);
      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               vdec_ptr->modules.ltetty_rx.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send updated input media format to dtmf detect capi
   if(vdec_ptr->modules.dtmf_det.module_ptr)
   {
      capi_result = vdec_ptr->modules.dtmf_det.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.dtmf_det.module_ptr, &init_props_list);
      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               vdec_ptr->modules.dtmf_det.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send updated input media format to slowtalk capi
   if(vdec_ptr->modules.slow_talk.module_ptr)
   {
      capi_result = vdec_ptr->modules.slow_talk.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.slow_talk.module_ptr, &init_props_list);
      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               vdec_ptr->modules.slow_talk.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send updated input media format to soft mute capi
   if(vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr)
   {
      capi_result = vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr, &init_props_list);
      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               vdec_ptr->modules.soft_mute.soft_mute_capi.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   return ADSP_EOK;
}

static ADSPResult vdec_stream_pp_capi_send_algo_reset(vdec_t *vdec_ptr)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   capi_v2_prop_t reset_prop[] = {
      { CAPI_V2_ALGORITHMIC_RESET, { NULL, 0, 0 }, {FALSE, FALSE, 0} },
   };
   capi_v2_proplist_t reset_prop_list = {sizeof(reset_prop)/sizeof(capi_v2_prop_t), reset_prop};

   // send algo_reset to hpcm capi
   if(vdec_ptr->modules.hpcm.module_ptr)
   {
      capi_result = vdec_ptr->modules.hpcm.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.hpcm.module_ptr, &reset_prop_list);
      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
               vdec_ptr->modules.hpcm.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send algo_reset to ctm rx capi
   if(vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr)
   {
      capi_result = vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr, &reset_prop_list);
      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
               vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send algo_reset to ltetty rx capi
   if(vdec_ptr->modules.ltetty_rx.module_ptr)
   {
      capi_result = vdec_ptr->modules.ltetty_rx.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.ltetty_rx.module_ptr, &reset_prop_list);
      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
               vdec_ptr->modules.ltetty_rx.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send algo_reset to dtmf_det capi
   if(vdec_ptr->modules.dtmf_det.module_ptr)
   {
      capi_result = vdec_ptr->modules.dtmf_det.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.dtmf_det.module_ptr, &reset_prop_list);
      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
               vdec_ptr->modules.dtmf_det.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

#if 0
   // Algorithm reset is not required for following modules during handover from one media type to another media type.
   // Initialization of following algorithms if sampling rate is changed is taken care by sending input_media_fmt property
   // when set_media_type command is received.

   // send algo_reset to slow_talk capi
   capi_result = vdec_ptr->modules.slow_talk.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.slow_talk.module_ptr, &reset_prop_list);
   if(CAPI_V2_EOK != capi_result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
            vdec_ptr->modules.slow_talk.module_id, capi_result);
      return capi_v2_err_to_adsp_result(capi_result);
   }

   // send algo_reset to soft mute capi
   capi_result = vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr->vtbl_ptr->set_properties(vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr, &reset_prop_list);
   if(CAPI_V2_EOK != capi_result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
            vdec_ptr->modules.soft_mute.soft_mute_capi.module_id, capi_result);
      return capi_v2_err_to_adsp_result(capi_result);
   }
#endif

   return ADSP_EOK;
}

static ADSPResult vdec_stream_pp_capi_destroy(vdec_t *vdec_ptr)
{
   // free memory allocated by stream capi modules
   vdec_stream_pp_capi_end(vdec_ptr);

   // free memory allocated for stream capi instances by service
   if (vdec_ptr->modules.start_addr_ptr)
   {
      qurt_elite_memory_free (vdec_ptr->modules.start_addr_ptr);
      vdec_ptr->modules.start_addr_ptr = NULL;
      vdec_ptr->modules.usage_addr_ptr = NULL;
   }

   // release amdb handle for stream capi modules
   if(vdec_ptr->modules.hpcm_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&vdec_ptr->modules.hpcm_amdb, 1);
      vdec_ptr->modules.hpcm_amdb.h.capi_v2_handle = NULL;
   }
   if(vdec_ptr->modules.ctm_rx.ctm_rx_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&vdec_ptr->modules.ctm_rx.ctm_rx_amdb, 1);
      vdec_ptr->modules.ctm_rx.ctm_rx_amdb.h.capi_v2_handle = NULL;
   }
   if(vdec_ptr->modules.ltetty_rx_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&vdec_ptr->modules.ltetty_rx_amdb, 1);
      vdec_ptr->modules.ltetty_rx_amdb.h.capi_v2_handle = NULL;
   }
   if(vdec_ptr->modules.dtmf_det_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&vdec_ptr->modules.dtmf_det_amdb, 1);
      vdec_ptr->modules.dtmf_det_amdb.h.capi_v2_handle = NULL;
   }
   if(vdec_ptr->modules.slow_talk_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&vdec_ptr->modules.slow_talk_amdb, 1);
      vdec_ptr->modules.slow_talk_amdb.h.capi_v2_handle = NULL;
   }

   if(vdec_ptr->modules.soft_mute.soft_mute_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&vdec_ptr->modules.soft_mute.soft_mute_amdb, 1);
      vdec_ptr->modules.soft_mute.soft_mute_amdb.h.capi_v2_handle = NULL;
   }


   // free memory allocated for fwk extns in stream capi modules
   if(vdec_ptr->modules.hpcm.fwk_extn_ptr)
   {
      qurt_elite_memory_free(vdec_ptr->modules.hpcm.fwk_extn_ptr);
      vdec_ptr->modules.hpcm.fwk_extn_ptr = NULL;
   }
   if(vdec_ptr->modules.ctm_rx.ctm_rx_capi.fwk_extn_ptr)
   {
      qurt_elite_memory_free(vdec_ptr->modules.ctm_rx.ctm_rx_capi.fwk_extn_ptr);
      vdec_ptr->modules.ctm_rx.ctm_rx_capi.fwk_extn_ptr = NULL;
   }
   if(vdec_ptr->modules.ltetty_rx.fwk_extn_ptr)
   {
      qurt_elite_memory_free(vdec_ptr->modules.ltetty_rx.fwk_extn_ptr);
      vdec_ptr->modules.ltetty_rx.fwk_extn_ptr = NULL;
   }
   if(vdec_ptr->modules.dtmf_det.fwk_extn_ptr)
   {
      qurt_elite_memory_free(vdec_ptr->modules.dtmf_det.fwk_extn_ptr);
      vdec_ptr->modules.dtmf_det.fwk_extn_ptr = NULL;
   }

   if(vdec_ptr->modules.slow_talk.fwk_extn_ptr)
   {
      qurt_elite_memory_free(vdec_ptr->modules.slow_talk.fwk_extn_ptr);
      vdec_ptr->modules.slow_talk.fwk_extn_ptr = NULL;
   }

   if(vdec_ptr->modules.soft_mute.soft_mute_capi.fwk_extn_ptr)
   {
      qurt_elite_memory_free(vdec_ptr->modules.soft_mute.soft_mute_capi.fwk_extn_ptr);
      vdec_ptr->modules.soft_mute.soft_mute_capi.fwk_extn_ptr = NULL;
   }
   return ADSP_EOK;
}

static ADSPResult vdec_stream_pp_capi_end(vdec_t *vdec_ptr)
{

   // free memory allocated by hpcm capi
   if(vdec_ptr->modules.hpcm.module_ptr)
   {
      if(vdec_ptr->modules.hpcm.module_ptr->vtbl_ptr)
      {
         vdec_ptr->modules.hpcm.module_ptr->vtbl_ptr->end(vdec_ptr->modules.hpcm.module_ptr);
         vdec_ptr->modules.hpcm.module_ptr->vtbl_ptr = NULL;
      }
      vdec_ptr->modules.hpcm.module_ptr = NULL;
   }

   // free memory allocated by ctm rx capi
   if(vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr)
   {
      if(vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr->vtbl_ptr)
      {
         vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr->vtbl_ptr->end(vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr);
         vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr->vtbl_ptr = NULL;
      }
      vdec_ptr->modules.ctm_rx.ctm_rx_capi.module_ptr = NULL;
   }
   // free memory allocated by ltetty rx capi
   if(vdec_ptr->modules.ltetty_rx.module_ptr)
   {
      if(vdec_ptr->modules.ltetty_rx.module_ptr->vtbl_ptr)
      {
         vdec_ptr->modules.ltetty_rx.module_ptr->vtbl_ptr->end(vdec_ptr->modules.ltetty_rx.module_ptr);
         vdec_ptr->modules.ltetty_rx.module_ptr->vtbl_ptr = NULL;
      }
      vdec_ptr->modules.ltetty_rx.module_ptr = NULL;
   }

   // reinit the DTMF Detection Module
   vdec_ptr->modules.apr_info_dtmf.self_addr = NULL;
   vdec_ptr->modules.apr_info_dtmf.self_port = NULL;
   vdec_ptr->modules.apr_info_dtmf.client_addr = NULL;
   vdec_ptr->modules.apr_info_dtmf.client_port = NULL;

   // free memory allocated by dtmf det capi
   if(vdec_ptr->modules.dtmf_det.module_ptr)
   {
      if(vdec_ptr->modules.dtmf_det.module_ptr->vtbl_ptr)
      {
         vdec_ptr->modules.dtmf_det.module_ptr->vtbl_ptr->end(vdec_ptr->modules.dtmf_det.module_ptr);
         vdec_ptr->modules.dtmf_det.module_ptr->vtbl_ptr = NULL;
      }
      vdec_ptr->modules.dtmf_det.module_ptr = NULL;
   }

   // free memory allocated by slow talk capi
   if(vdec_ptr->modules.slow_talk.module_ptr)
   {
      if(vdec_ptr->modules.slow_talk.module_ptr->vtbl_ptr)
      {
         vdec_ptr->modules.slow_talk.module_ptr->vtbl_ptr->end(vdec_ptr->modules.slow_talk.module_ptr);
         vdec_ptr->modules.slow_talk.module_ptr->vtbl_ptr = NULL;
      }
      vdec_ptr->modules.slow_talk.module_ptr = NULL;
   }


   // free memory allocated by soft mute capi
   if(vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr)
   {
      if(vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr->vtbl_ptr)
      {
         vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr->vtbl_ptr->end(vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr);
         vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr->vtbl_ptr = NULL;
      }
      vdec_ptr->modules.soft_mute.soft_mute_capi.module_ptr = NULL;
   }
   return ADSP_EOK;
}

static ADSPResult vdec_stream_pp_capi_reinit(vdec_t* vdec_ptr)
{
   ADSPResult result = ADSP_EOK;

   // free memory allocated by stream capi modules
   vdec_stream_pp_capi_end(vdec_ptr);

   // clear stream capi input media type sampling rate
   vdec_ptr->modules.input_media_type.data_format.sampling_rate = 0;

   // initialize stream capi modules again.
   result = vdec_stream_pp_capi_init(vdec_ptr);

   vdec_ptr->modules.ctm_rx.ctm_from_far_end_detect_notified = FALSE;
   vdec_ptr->modules.soft_mute.mute = VOICE_UNMUTE;

   return result;
}

static ADSPResult vdec_log_tty_rx_char(vdec_t *vdec_ptr, uint32_t tty_rx_char)
{
   // log tty character if available
#if defined(__qdsp6__) && !defined(SIM)
       // log tty character if available
       int8_t *bufptr[4] = { (int8_t *) &tty_rx_char, NULL, NULL, NULL };
       voice_log_buffer( bufptr,
                         VOICE_LOG_CHAN_VSM_RX_OOBTTY_CHAR,
                         (((vdec_ptr->sampling_rate_dec == 16000) << 3) | vdec_ptr->session_num),
                         qurt_elite_timer_get_time(),
                         VOICE_LOG_DATA_FORMAT_PCM_MONO,
                         (vdec_ptr->sampling_rate_dec),
                         sizeof(int8_t),
                         NULL);
#endif
   return ADSP_EOK;
}

static ADSPResult vdec_log_ctm_tty_rx_char(vdec_t *vdec_ptr, uint32_t tty_tx_char)
{
   // log tty character if available
#if defined(__qdsp6__) && !defined(SIM)
  int8_t *bufptr[4] = { (int8_t *) &tty_tx_char, NULL, NULL, NULL };
  voice_log_buffer( bufptr,
  VOICE_LOG_CHAN_VSM_RX_CTM_CHAR,
  (((VOICE_NB_SAMPLING_RATE != vdec_ptr->sampling_rate_dec) << 3) | vdec_ptr->session_num),
    qurt_elite_timer_get_time(),
    VOICE_LOG_DATA_FORMAT_PCM_MONO,
    (uint32_t)(vdec_ptr->sampling_rate_dec),
    sizeof(int8_t),
    NULL);
#endif
   return ADSP_EOK;
}

boolean vdec_check_stream_pp_capi(uint32_t module_id)
{
   switch(module_id)
   {
      case VOICE_MODULE_CTM_RX:
      case VOICE_MODULE_LTETTY_RX:
      case VOICE_MODULE_ST:
      case VOICE_MODULE_DTMF_DETECTION:
      case VOICE_MODULE_SOFT_MUTE:
      case VOICE_MODULE_HPCM_STREAM_RX:
      {
         return TRUE;
      }
      default:
      {
         return FALSE;
      }
   }
}

int32_t vdec_send_hpcm_buf_available_evt(vdec_t* vdec_ptr, void* data_ptr)
{
   int32_t rc = ADSP_EOK;

   rc = elite_apr_if_alloc_send_event( vdec_ptr->modules.apr_info_hpcm.apr_handle,
                                       vdec_ptr->modules.apr_info_hpcm.self_addr,
                                       vdec_ptr->modules.apr_info_hpcm.self_port,
                                       vdec_ptr->modules.apr_info_hpcm.client_addr,
                                       vdec_ptr->modules.apr_info_hpcm.client_port,
                                       0, /* token */
                                       VOICE_EVT_HOST_BUF_AVAILABLE_V2,
                                       data_ptr,
                                       sizeof(voice_evt_notify_host_pcm_buf_v2_t) );

   if (ADSP_FAILED(rc))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to create APR request for VOICE_EVT_HOST_BUF_AVAILABLE_V2  0x%08lx :session(%lx)",rc,vdec_ptr->session_num);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VOICE_EVT_HOST_BUF_AVAILABLE_V2 sent to client, session(%lx)",vdec_ptr->session_num);
   }
   return rc;
}
