/*========================================================================

*//** @file VoiceEnc.cpp

Copyright (c) 2009-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the code for Voice Encoder (Venc) Dynamic service. This service
has one thread. It receives commands from Voice Stream Manager (VSM).
Venc initializes the vocoder algorithms and processes the data. Multiple Venc can be
instantiated and each of them is identified by a session number provided
while creating.
*//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_enc/src/venc_svc.cpp#3 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  10/26/10  pg    use VAD1
  06/25/10  pg   Major cleanup
  06/03/10  pg   Merge features from main to 8660DevAprV2
  EFR, FR, HR, PCM_NB, 4GV_NB, V13k, G711 A/MLAW, 4GV_WB,
  ENC-DEC pkt loopback
  11/16/09  pg   Created file.
  ========================================================================== */

/* =======================================================================
   INCLUDE FILES FOR MODULE
   ========================================================================== */
#include "venc_private.h"
#include "capi_v2_library_factory.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

// Masks for the signals and queues
#define VENC_RESYNC_MASK            0x80000000
#define VENC_TICK_MASK              0x40000000
#define VENC_CMD_MASK               0x20000000
#define VENC_VDS_RESPONSE_MASK      0x10000000
#define VENC_BUF_MASK               0x08000000
#define VENC_REC_BUF_MASK           0x04000000
#define VENC_DATA_MASK              0x02000000
#define VENC_TICK_END_MASK          0x01000000
#define VENC_RESPONSE_MASK          0x00800000
#define VENC_CMD_DATA_TICK_MASK     (VENC_TICK_MASK | VENC_CMD_MASK | VENC_DATA_MASK | VENC_RESYNC_MASK | VENC_VDS_RESPONSE_MASK)
#define VENC_ALL_MASK               (VENC_TICK_MASK | VENC_CMD_MASK | VENC_DATA_MASK | VENC_TICK_END_MASK | VENC_REC_BUF_MASK | VENC_RESYNC_MASK | VENC_VDS_RESPONSE_MASK)
#define VENC_CMD_DATA_MASK          (VENC_CMD_MASK | VENC_DATA_MASK )
#define FULL_RATE                   4
#define HALF_RATE                   3
#define QUARTER_RATE                2
#define SILENCE_RATE                1
#define ENC_STATE_MEMORY_IN_BYTES   100*1024 // worst case for EVS encoder

#define VENC_SS_MULTIFRAME   VOICE_SS_MULTI_FRAME_4


// TODO: Remove when API updated
/**  Parameter for enabling internal loopback feature for a particular session.
  Fixed size of LOOPBACK_ENABLE_PARAM_BYTES = 4 bytes.
 */
#define VOICE_PARAM_LOOPBACK_ENABLE (0x00010E18)


// externs
extern uint32_t vsm_memory_map_client;

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
// Maximum data messages in data queue. Expectation is upstream svc will send 20ms buffers.
static const uint32_t MAX_DATA_Q_ELEMENTS = 4;

// Maximum number of commands expected ever in command queue.
static const uint32_t MAX_CMD_Q_ELEMENTS = 8;

// How many buffers in recording buffer queue? May need to make this configurable...
static const uint32_t MAX_REC_BUF_Q_ELEMENTS = 3;

// maximum number of commands expected ever in response queue.
static const uint32_t MAX_RESP_Q_ELEMENTS = 2;

// Buffer size. Assume service knows what size buffer it needs to deliver.
// If it needs to be told, have to add a command to be informed.
static const uint32_t REC_BUFFER_SIZE = 2*(1920 + 192);
//22 ms for worst case for each set in stereo mode

// Thread name
static char_t aTHREAD_NAME[6] = {'V','E','C','$','\0'};

// Thread stack size
static const uint32_t MIN_THREAD_STACK_SIZE = 24*1024;    //minimum thread stack for venc
static const uint32_t SERVICE_THREAD_STACK_SIZE = 8*1024; //stack contribution from service

// Each count is 20ms, maximum delay after which ctm resync should be set
static const uint32_t MAX_RESYNC_DELAY = 4;

/* flag to differentiate of packing of bitstream for FR encoder starting from LSB or MSB*/
#define FR_ENC_FROM_VOICE_PATH         0

#define SS_DELAY_RECORD_PATH            16   // 2msec delay in NB : 1ms for pre-buffering and 1ms for system jitter

#define MAX_SAMP_RATE_FACTOR 6          //(FB/NB)

#define SS_DELAY_REC_PATH_NB 16 //2msec

// NB default 20msec + 4msec for delay for sample slipping 2ms for pre buffering for slipping & 2 ms to prevent overflow due to stuffing
#define ENC_OUT_CIRC_BUF_SIZE_REC      (160+SS_DELAY_RECORD_PATH)



/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */
// destructor
static void venc_destroy(venc_t* venc_ptr);

// Main work loop for service thread. Pulls msgs off of queues and processes them.
static ADSPResult venc_thread(void* instance_ptr);
static ADSPResult venc_thread_entry(void* instance_ptr);

// Main processing function for converting input buffer to output buffer
static ADSPResult venc_process(venc_t* venc_ptr);

// message handler functions
static ADSPResult venc_connect_dwn_stream_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_disconnect_dwn_stream_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_run_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_stop_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_param_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_get_param_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_media_type(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_destroy_yourself_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);

static ADSPResult venc_custom_msg( void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_apr_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_mute_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_pp_samp_rate(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_tty_mode(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_ltetty_mode(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_timing_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_timingv2_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_timingv3_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_reinit_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_start_record_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_stop_record_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_send_rec_buf_to_afe(venc_t* venc_ptr,qurt_elite_bufmgr_node_t *out_buf_mgr_node_ptr);
static ADSPResult venc_send_afe_media_type(venc_t *venc_ptr);
static ADSPResult venc_config_host_pcm(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_pkt_exchange_mode(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_oob_pkt_exchange_config(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_register_event(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_unregister_event(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_param_v3(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_enc_rate(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_set_param_int(venc_t* venc_ptr, uint32_t module_id, uint16_t instance_id, uint32_t param_id, void* payload_address_in, uint32_t param_size);
static ADSPResult venc_set_voc_param(void* instance_ptr, elite_msg_any_t* msg_ptr);

// state handler functions
static ADSPResult venc_vfr_handler(void* instance_ptr);
static ADSPResult venc_cmd_handler(void* instance_ptr);
static ADSPResult venc_buf_handler(void* instance_ptr);
static ADSPResult venc_rec_buf_handler(void* instance_ptr);
static ADSPResult venc_data_handler(void* instance_ptr);
static ADSPResult venc_vfr_end_handler(void* instance_ptr);
static ADSPResult venc_resync_handler(void *instance_ptr);
static ADSPResult venc_response_handler(void* instance_ptr);

// Memory management related function
static ADSPResult venc_allocate_mem(venc_t * venc_ptr);
static void venc_allocate_mem_free(venc_t * venc_ptr);

// Initialization and End functions
static ADSPResult venc_modules_init(venc_t * venc_ptr);
static ADSPResult venc_init(venc_t *venc_ptr);
static ADSPResult venc_enc_params_init(venc_t *venc_ptr);
static ADSPResult venc_ctrl_params_init(venc_t *venc_ptr);

// Error check functions
static void voice_result_check(ADSPResult result, uint32_t session_num);

// Helper functions
static int32_t venc_send_dtmf_gen_ended(venc_t *venc_ptr);
static ADSPResult venc_send_ltetty_tx_char_detected(venc_t *venc_ptr, uint32_t tty_tx_char);
static ADSPResult venc_send_mode_notification_v2(void* instance_ptr);
static ADSPResult venc_launch_thread(venc_t* venc_ptr, bool_t venc_thread_exists, uint32_t new_stack_size);

// APR buffer delivery
static ADSPResult venc_deliver_pkt(venc_t* venc_ptr, void *buffer_ptr, uint32_t nBufSize);

// wrapper for logging DSP format vocoder packet
static ADSPResult  venc_log_vocoder_packet( uint32_t voc_type, uint32_t log_session_id, void *out_ptr, uint16_t size, uint16_t sampling_rate);

// function to init the record circular buffer with 2ms pre-buffering
static void venc_init_rec_circbuf(venc_t* venc_ptr);

// calculate drift with respect to av/hp timer
static void inline venc_cal_drift(venc_t* venc_ptr);
// subscribe functions to voice timer for hard vfr
static void venc_vtm_subscribe(venc_t* venc_ptr,Vtm_SubUnsubMsg_t *data_ptr);
static void venc_vtm_unsubscribe(venc_t* venc_ptr,Vtm_SubUnsubMsg_t *data_ptr,uint32_t mask);

//voice delivery functions
static ADSPResult venc_vds_response_handler(void* instance_ptr);
static ADSPResult venc_get_kpps_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_aggregate_modules_kpps(void* instance_ptr, uint32_t* kpps_changed);
static ADSPResult venc_get_delay_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult venc_aggregate_modules_delay(void* instance_ptr);

// calibration callback function
static void venc_calibration_cb_func(void* cal_params_ptr, void* cb_data);

// ECALL TX wrapper function
static void voice_ecall_tx (venc_t *venc_ptr, int16_t *in_out_ptr);

//CAPI helper functions
static ADSPResult venc_encoder_capi_destroy(venc_t* venc_ptr);
static ADSPResult venc_encoder_capi_init(venc_t* venc_ptr, uint32_t media_type);

static ADSPResult venc_encoder_capi_process(venc_t* venc_ptr, int16_t* in_ptr, int16_t* out_ptr, uint32_t buf_size);
static ADSPResult venc_capi_set_param(voice_capi_module_t *module_info_ptr, uint32_t param_id, void* param_payload_ptr, uint32_t param_size,uint32_t buf_size);
static ADSPResult venc_capi_get_param(voice_capi_module_t *module_info_ptr, uint32_t param_id, void* param_payload_ptr, uint32_t buf_size,uint16_t *param_size);
static ADSPResult venc_encoder_capi_get_data_threshold (venc_t* venc_ptr);
static ADSPResult venc_encoder_capi_set_media_fmt(venc_t *venc_ptr, uint32_t sampling_rate);
static ADSPResult venc_encoder_capi_get_amdb_handle(venc_t* venc_ptr, uint32_t media_type);

static capi_v2_err_t venc_encoder_capi_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr);
static uint32_t voice_enc_get_default_sampling_rate( uint32_t voc_type);

// Stream capi helper functions
static ADSPResult venc_stream_pp_capi_create(venc_t* venc_ptr);
static ADSPResult venc_stream_pp_capi_reinit(venc_t* venc_ptr);
static ADSPResult venc_stream_pp_capi_get_amdb_handle(venc_t* venc_ptr);
static ADSPResult venc_stream_pp_capi_get_static_property(venc_t* venc_ptr);
static ADSPResult venc_stream_pp_capi_query_intf_extn(venc_t* venc_ptr);
static ADSPResult venc_stream_pp_capi_init(venc_t* venc_ptr);
static ADSPResult venc_stream_pp_capi_init_lib(venc_t* venc_ptr, voice_capi_module_t *module_info_ptr, capi_v2_proplist_t *init_prop_list_ptr);
static ADSPResult venc_stream_pp_capi_set_input_media_fmt(venc_t *venc_ptr);
static ADSPResult venc_stream_pp_capi_send_algo_reset(venc_t *venc_ptr);
static ADSPResult venc_stream_pp_capi_destroy(venc_t *venc_ptr);
static ADSPResult venc_stream_pp_capi_end(venc_t *venc_ptr);
static capi_v2_err_t venc_stream_pp_capi_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr);
static ADSPResult venc_log_ltetty_tx_char(venc_t *venc_ptr, uint32_t tty_tx_char);
static ADSPResult venc_log_ctm_tty_tx_char(venc_t *venc_ptr, uint32_t tty_tx_char);

ADSPResult voice_capi_get_amdb_handle(voice_capi_module_t *module_info_ptr, adsp_amdb_module_handle_info_t *amdb_handle_info_ptr);
boolean venc_check_stream_pp_capi(uint32_t module_id);
int32_t venc_send_hpcm_buf_available_evt(venc_t* venc_ptr, void* data_ptr);
/* -----------------------------------------------------------------------
 ** Message handler
 ** ----------------------------------------------------------------------- */
/* Venc custom msg handler */
static elite_svc_msg_handler_func pHandler[VENC_NUM_MSGS] =
{
   venc_get_kpps_cmd,                // VENC_GET_KPPS_CMD
   venc_set_mute_cmd,                // VENC_SET_MUTE_CMD
   venc_set_timing_cmd,              // VENC_SET_TIMING_CMD
   venc_set_timingv2_cmd,            // VENC_SET_TIMINGV2_CMD
   venc_reinit_cmd,                  // VENC_REINIT_CMD
   venc_stop_record_cmd,             // VENC_STOP_RECORD_CMD
   venc_start_record_cmd,            // VENC_START_RECORD_CMD_V2
   venc_config_host_pcm,             // VENC_CONFIG_HOST_PCM
   venc_set_pp_samp_rate,            // VENC_SET_STREAM_PP_SAMP_RATE
   venc_set_tty_mode,                // VENC_SET_TTY_MODE
   venc_set_ltetty_mode,             // VENC_SET_LTETTY_MODE
   venc_set_pkt_exchange_mode,       // VENC_SET_PKT_EXCHANGE_MODE
   venc_set_oob_pkt_exchange_config, // VENC_SET_OOB_PKT_EXCHANGE_CONFIG
   venc_set_timingv3_cmd,            // VENC_SET_TIMINGV3_CMD
   venc_get_delay_cmd,               // VENC_GET_DELAY_CMD
   venc_register_event,              // VENC_REGISTER_EVENT
   venc_unregister_event,            // VENC_UNREGISTER_EVENT
   venc_set_param_v3,                // VENC_SET_PARAM_V3
   venc_set_enc_rate,                // VENC_SET_ENC_RATE
   venc_set_voc_param,               // VENC_SET_VOC_PARAM
};

/* Venc FADD msg handler */
static elite_svc_msg_handler_func pEliteHandler[] =
{
   venc_custom_msg,              //0  - ELITE_CUSTOM_MSG
   elite_svc_unsupported,          //1  - ELITE_CMD_START_SERVICE
   venc_destroy_yourself_cmd,     //2  - ELITE_CMD_DESTROY_SERVICE
   venc_connect_dwn_stream_cmd,    //3  - ELITE_CMD_CONNECT
   venc_disconnect_dwn_stream_cmd, //4  - ELITE_CMD_DISCONNECT
   venc_stop_cmd,                //5  - ELITE_CMD_PAUSE
   venc_run_cmd,                 //6  - ELITE_CMD_RESUME
   elite_svc_unsupported,          //7  - ELITE_CMD_FLUSH
   venc_set_param_cmd,            //8  - ELITE_CMD_SET_PARAM
   venc_get_param_cmd,            //9  - ELITE_CMD_GET_PARAM
   elite_svc_unsupported,          //10 - ELITE_DATA_BUFFER
   venc_set_media_type,           //11 - ELITE_DATA_MEDIA_TYPE
   elite_svc_unsupported,          //12 - ELITE_DATA_EOS
   elite_svc_unsupported,          //13 - ELITE_DATA_RAW_BUFFER
   elite_svc_unsupported,          //14 - ELITE_CMD_STOP_SERVICE
   venc_apr_cmd                  //15 - ELITE_APR_PACKET_OPCODE
};

/* Venc channel handler */
static ADSPResult (*pVencHandler[])(void *) =
{
   venc_resync_handler, //handles resync
   venc_vfr_handler, // handles vfr signal
   venc_cmd_handler, // handles cmds
   venc_vds_response_handler, //VDS response handler for failed delivery
   venc_buf_handler, // handles output buffers
   venc_rec_buf_handler,
   venc_data_handler,// handles input data buffers
   venc_vfr_end_handler, // handles hand shake signal from voicetimer for unsubscribe
   venc_response_handler     // should never get invoked because of sync wait
};

// global variables for enc-dec pkt loopback
int32_t gVoiceEncDecLB = 0;
int8_t gaVoiceEncDecLBBuf[640] = {0}; // accounts for wb also

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

static ADSPResult venc_launch_thread(venc_t* venc_ptr, bool_t venc_thread_exists, uint32_t new_stack_size)
{
   ADSPResult result=ADSP_EOK;

   if( venc_ptr->thread_info.stack_size != new_stack_size)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_launch_thread(): thread stack size has changes from(%ld) to (%ld). Launching thread. session(%#lx)",
            venc_ptr->thread_info.stack_size,new_stack_size,venc_ptr->session_num);

      uint32_t old_stack_size = venc_ptr->thread_info.stack_size;
      venc_ptr->thread_info.stack_size = new_stack_size;
      venc_ptr->thread_info.cached_tid = venc_ptr->svc_handle.threadId;
      venc_ptr->thread_info.another_thread_launched = venc_thread_exists;

      // Launch the thread
      aTHREAD_NAME[3] = ((venc_ptr->session_num + 48) & 0xff); // int32_t to ascii conversion
      if (ADSP_FAILED(result = qurt_elite_thread_launch(&(venc_ptr->svc_handle.threadId), aTHREAD_NAME, NULL,
            venc_ptr->thread_info.stack_size, elite_get_thrd_prio(ELITETHREAD_DYNA_VOICE_ENC_PRIO_ID), venc_thread_entry, (void*)venc_ptr, QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: venc_launch_thread(): Failed create Venc thread!! session(%#lx)",venc_ptr->session_num);

       	 //update thread info and stack size for failure case to prevent older thread           from exiting. Set fatal state flag
	 venc_ptr->thread_info.stack_size = old_stack_size;
	 venc_ptr->thread_info.another_thread_launched = FALSE;
	 venc_ptr->fatal_state = TRUE;
	 venc_encoder_capi_destroy(venc_ptr);

         return result;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_launch_thread(): No change in thread stack. Not launching thread");
   }
   return result;
}

ADSPResult venc_create (uint32_t nInputParam,
      voice_strm_apr_info_t  *apr_info_ptr,
      voice_strm_tty_state_t *tty_state_ptr,
      elite_svc_handle_t *loopback_handle,
      void **ppHandle,uint32_t session_num)
{
   ADSPResult result=ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_create enter session(%x)",(int)session_num);

   // allocate instance struct
   venc_t *venc_ptr = (venc_t*) qurt_elite_memory_malloc( sizeof(venc_t), QURT_ELITE_HEAP_DEFAULT);
   if (!venc_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate memory for venc struct !!");
      return ADSP_ENOMEMORY;
   }

   // zero out all the fields
   memset(venc_ptr, 0, sizeof(venc_t));

   // Initialize instance variables
   venc_ptr->svc_handle.unSvcId = ELITE_VOICE_ENC_SVCID;
   venc_ptr->wait_mask = VENC_CMD_DATA_MASK;
   venc_ptr->apr_info_ptr = apr_info_ptr;
   venc_ptr->loopback_handle = loopback_handle;
   venc_ptr->tty.state_ptr       = tty_state_ptr;
   venc_ptr->session_num = session_num;

   // initialze qurt_elite channel
   qurt_elite_channel_init(&venc_ptr->qurt_elite_channel);

   venc_ptr->modules.ctm_tx.resync_modem_afe = FALSE;
   venc_ptr->record.enable = FALSE; // should not be reset in init to support handover cases
   venc_ptr->record.mode = 0; // init with 0
   (void) venc_init(venc_ptr);
   // directly copy generic handle -> should be same for any VSM client
   venc_ptr->modules.apr_info_dtmf_gen.apr_handle = venc_ptr->apr_info_ptr->apr_handle;

   if(qurt_elite_globalstate.pVoiceTimerCmdQ == NULL)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to get Vtm cmdQ session(%x)",(int)session_num);
      venc_ptr->vtm_cmd_q_ptr = NULL;
   }
   venc_ptr->vtm_cmd_q_ptr = qurt_elite_globalstate.pVoiceTimerCmdQ;

   venc_ptr->vds_handle_ptr = vds_handle;
   venc_ptr->vds_client_token = VDS_CREATE_TOKEN(venc_ptr->session_num, VDS_CLIENT_VENC_DELIVERY);

   // for recording
   venc_ptr->record.afe_drift_ptr = NULL;
   venc_ptr->record.ss_info_counter = 0;
   venc_ptr->record.aud_ref_port = 0;

   result = venc_allocate_mem(venc_ptr);

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Venc Allocate Memory function (%lx)",session_num);
      venc_destroy(venc_ptr);
      return result;
   }

   // Create stream pp capi modules and intialize.
   result = venc_stream_pp_capi_create(venc_ptr);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_create: venc_stream_pp_capi_create failed. result(%ld), session(0x%x)",
            (uint32_t)result, (uint32_t)session_num);
      venc_destroy(venc_ptr);
      return result;
   }

   //default initialize soft-mute module
   cross_fade_init_default ( &(venc_ptr->proc.cross_fade_cfg_struct) );
   venc_ptr->proc.cross_fade_cfg_struct.iConvergeNumSamples = 0;
   venc_ptr->proc.cross_fade_cfg_struct.iTotalPeriodMsec = 0;
   cross_fade_init ( &(venc_ptr->proc.cross_fade_cfg_struct),&(venc_ptr->proc.cross_fade_data_struct),8000/*default sampling rate*/);

   result= venc_modules_init(venc_ptr);

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Venc init failed (%lx)",session_num);
      venc_destroy(venc_ptr);
      return result;
   }

   // make Venc Q names unique per instance
   snprintf(venc_ptr->data_q_name    , QURT_ELITE_DEFAULT_NAME_LEN, "Venc_DataQ%2x",(int)session_num);
   snprintf(venc_ptr->cmd_q_name     , QURT_ELITE_DEFAULT_NAME_LEN, "Venc_CmdQ%2x" ,(int)session_num);
   snprintf(venc_ptr->buf_q_name     , QURT_ELITE_DEFAULT_NAME_LEN, "Venc_BufQ%2x" ,(int)session_num);
   snprintf(venc_ptr->record.buf_q_name , QURT_ELITE_DEFAULT_NAME_LEN, "Venc_Rec_BufQ%2x" ,(int)session_num);
   snprintf(venc_ptr->respq_name     , QURT_ELITE_DEFAULT_NAME_LEN, "Venc_RespQ%2lx",session_num);

   // Create the queues. Use non-blocking queues, since pselect is always used.
   // pselect blocks on any non-masked queue to receive, then can do non-blocking checks.
   // DataQ for inputs
   // CmdQ for cmd inputs
   // BufQ for output buffers - no need of a Q, instead use local buffer, todo: need bufQ for enc-dec loopback
   // signal_ptr for 20ms tick
   // signal_end_ptr for handshaking unsubscribe with VoiceTimer

   /* Allocate memory for queues/signals */
   venc_ptr->svc_handle.dataQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_DATA_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );
   venc_ptr->svc_handle.cmdQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_CMD_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );
   venc_ptr->record.buf_q_ptr = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_CMD_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );
   venc_ptr->resp_q_ptr = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_RESP_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );
   /* This tick signal will be allocated by VDS, don't allocate/free/add to channel here*/
   venc_ptr->vtm_sub_unsub_data.signal_ptr = NULL;
   venc_ptr->vtm_sub_unsub_data.signal_end_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   venc_ptr->vtm_sub_unsub_data.resync_signal_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   venc_ptr->vds_error_signal_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == venc_ptr->svc_handle.dataQ
         || NULL == venc_ptr->svc_handle.cmdQ
         || NULL == venc_ptr->record.buf_q_ptr
         || NULL == venc_ptr->resp_q_ptr
         || NULL == venc_ptr->vtm_sub_unsub_data.signal_end_ptr
         || NULL == venc_ptr->vtm_sub_unsub_data.resync_signal_ptr
         || NULL == venc_ptr->vds_error_signal_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to allocate memory for Venc Queues/Signals session(%#lx)",session_num);
      venc_destroy(venc_ptr);
      return ADSP_ENOMEMORY;
   }

   if(   ADSP_FAILED(result = qurt_elite_queue_init(venc_ptr->data_q_name, MAX_DATA_Q_ELEMENTS, venc_ptr->svc_handle.dataQ))
         || ADSP_FAILED(result = qurt_elite_queue_init(venc_ptr->cmd_q_name, MAX_CMD_Q_ELEMENTS, venc_ptr->svc_handle.cmdQ))
         || ADSP_FAILED(result = qurt_elite_queue_init(venc_ptr->respq_name, MAX_RESP_Q_ELEMENTS, venc_ptr->resp_q_ptr))
         || ADSP_FAILED(result = qurt_elite_signal_init(venc_ptr->vtm_sub_unsub_data.signal_end_ptr))
         || ADSP_FAILED(result = qurt_elite_signal_init(venc_ptr->vtm_sub_unsub_data.resync_signal_ptr))
         || ADSP_FAILED(result = qurt_elite_signal_init(venc_ptr->vds_error_signal_ptr))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&venc_ptr->qurt_elite_channel, (venc_ptr->svc_handle.cmdQ), VENC_CMD_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&venc_ptr->qurt_elite_channel, (venc_ptr->svc_handle.dataQ), VENC_DATA_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&venc_ptr->qurt_elite_channel, (venc_ptr->resp_q_ptr), VENC_RESPONSE_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_add_signal(&venc_ptr->qurt_elite_channel, (venc_ptr->vtm_sub_unsub_data.signal_end_ptr), VENC_TICK_END_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_add_signal(&venc_ptr->qurt_elite_channel, (venc_ptr->vtm_sub_unsub_data.resync_signal_ptr), VENC_RESYNC_MASK))
         || ADSP_FAILED(result = qurt_elite_channel_add_signal(&venc_ptr->qurt_elite_channel, (venc_ptr->vds_error_signal_ptr), VENC_VDS_RESPONSE_MASK))
         || ADSP_FAILED(result = qurt_elite_queue_init(venc_ptr->record.buf_q_name, MAX_CMD_Q_ELEMENTS,
               venc_ptr->record.buf_q_ptr))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&venc_ptr->qurt_elite_channel, (venc_ptr->record.buf_q_ptr),
               VENC_REC_BUF_MASK))

     )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed create Venc MsgQs or Signals!! session(%x)",(int)session_num);
      venc_destroy(venc_ptr);
      return result;
   }
   // Allocate and queue up the  recording output buffers
   for (uint32_t i = 0; i < MAX_REC_BUF_Q_ELEMENTS-1; i++)
   {
      //allocate the databuffer payload (metadata + pcm buffer size)
      int32_t req_size = GET_ELITEMSG_DATABUF_REQ_SIZE(REC_BUFFER_SIZE);
      elite_msg_data_buffer_t* data_payload_ptr = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc(req_size, QURT_ELITE_HEAP_DEFAULT);

      if (!data_payload_ptr)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to allocate memory for recording output buffer, session(%lx)",venc_ptr->session_num);
         venc_destroy(venc_ptr);
         return ADSP_ENOMEMORY;
      }

      data_payload_ptr->nActualSize = REC_BUFFER_SIZE;
      data_payload_ptr->nMaxSize    = REC_BUFFER_SIZE;

      venc_ptr->record.bufs_allocated++;

      if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(venc_ptr->record.buf_q_ptr, (elite_msg_any_payload_t*) data_payload_ptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to fill Venc Rec buf queue session(%lx)",venc_ptr->session_num);
         qurt_elite_memory_free(data_payload_ptr);
         venc_destroy(venc_ptr);
         return result;
      }
   }

   result = venc_launch_thread(venc_ptr,FALSE,MIN_THREAD_STACK_SIZE);
   if(ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to launch venc thread!! session(%x)",(int)session_num);
      venc_destroy(venc_ptr);
      return result;
   }

   *ppHandle = &(venc_ptr->svc_handle);
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_create end %d, wait_mask=%x, session(%x)",(int)result,(int)venc_ptr->wait_mask,(int)session_num);
   return ADSP_EOK;
}

// destructor
static void venc_destroy (venc_t* venc_ptr)
{
   if (venc_ptr)
   {
      uint32_t session_num = venc_ptr->session_num;

      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_destroy begin session(%x)",(int)session_num);

      // call utility function to destroy data Q
      if(NULL != venc_ptr->svc_handle.dataQ)
      {
         elite_svc_deinit_data_queue(venc_ptr->svc_handle.dataQ);
         qurt_elite_memory_free(venc_ptr->svc_handle.dataQ);
         venc_ptr->svc_handle.dataQ = NULL;
      }

      // call utility function to destroy cmd Q
      if(NULL != venc_ptr->svc_handle.cmdQ)
      {
         elite_svc_deinit_cmd_queue(venc_ptr->svc_handle.cmdQ);
         qurt_elite_memory_free(venc_ptr->svc_handle.cmdQ);
         venc_ptr->svc_handle.cmdQ = NULL;
      }


      // call utility function to destroy response Q
      if(NULL != venc_ptr->resp_q_ptr)
      {
         elite_svc_deinit_cmd_queue(venc_ptr->resp_q_ptr);
         qurt_elite_memory_free(venc_ptr->resp_q_ptr);
         venc_ptr->resp_q_ptr = NULL;
      }

      // call utility function to destroy signal
      if(NULL != venc_ptr->vtm_sub_unsub_data.signal_end_ptr)
      {
         qurt_elite_signal_deinit(venc_ptr->vtm_sub_unsub_data.signal_end_ptr);
         qurt_elite_memory_free(venc_ptr->vtm_sub_unsub_data.signal_end_ptr);
         venc_ptr->vtm_sub_unsub_data.signal_end_ptr = NULL;
      }


      if(NULL != venc_ptr->vtm_sub_unsub_data.resync_signal_ptr)
      {
         qurt_elite_signal_deinit(venc_ptr->vtm_sub_unsub_data.resync_signal_ptr);
         qurt_elite_memory_free(venc_ptr->vtm_sub_unsub_data.resync_signal_ptr);
         venc_ptr->vtm_sub_unsub_data.resync_signal_ptr = NULL;
      }

      if(NULL != venc_ptr->vds_error_signal_ptr)
      {
         qurt_elite_signal_deinit(venc_ptr->vds_error_signal_ptr);
         qurt_elite_memory_free(venc_ptr->vds_error_signal_ptr);
         venc_ptr->vds_error_signal_ptr = NULL;
      }

      if(NULL != venc_ptr->record.buf_q_ptr)
      {
         elite_svc_deinit_buf_queue(venc_ptr->record.buf_q_ptr, venc_ptr->record.bufs_allocated);
         qurt_elite_memory_free(venc_ptr->record.buf_q_ptr);
         venc_ptr->record.bufs_allocated = 0;
         venc_ptr->record.buf_q_ptr = NULL;
      }

      // Destroy the qurt_elite_channel
      qurt_elite_channel_destroy(&venc_ptr->qurt_elite_channel);

      venc_allocate_mem_free(venc_ptr);

      venc_stream_pp_capi_destroy(venc_ptr);

      qurt_elite_memory_free(venc_ptr);

      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_destroy end session(%x)",(int)session_num);
   }
   venc_ptr = NULL;
}

static ADSPResult venc_thread_entry(void* instance_ptr)
{
   ADSPResult result = ADSP_EOK;                           // general result value
   venc_t *venc_ptr = (venc_t*)instance_ptr;    // instance structure
   int join_status;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: venc_thread_entry(): Venc Current context: %d", (int)qurt_thread_get_id());

   if(venc_ptr->thread_info.another_thread_launched)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: venc_thread_entry(): waiting another thread to join, session(%#lx)", venc_ptr->session_num);
      // a new thread was launched, so join the old thread first
      qurt_elite_thread_join(venc_ptr->thread_info.cached_tid, &join_status);
      // Usually it should return immediately, once it returns, reenter the workloop
      //Can safely clear flag once join returns, since now there is only one thread
      venc_ptr->thread_info.another_thread_launched = FALSE;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: venc_thread_entry(): for Create, session(%lx)",venc_ptr->session_num);
   }
   // call into workloop
   result = venc_thread(instance_ptr);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: venc_thread_entry(): exiting, session(%#lx), Current Tid(%d)", venc_ptr->session_num,qurt_thread_get_id());

   return result;
}

/**
 * This function is the main work loop for the service. VFR signals and Commands
 * are handled with the highest priority. Data processing is handled only when command
 * queue is empty.
 *
 * The thread listens for command and data queues simultaneously (commands always
 * prioritized). When a data buffer is received, it is stored and processed when enough
 * data and the thread switches to listening for command queue and output buffer
 * queue. As soon as an output buffer is received, the data is copied  and delivered
 * downstream.
 */

static ADSPResult venc_thread(void* instance_ptr)
{
   ADSPResult result;                    // general result value
   venc_t *venc_ptr = (venc_t*)instance_ptr;    // instance structure
   uint32_t session_num = venc_ptr->session_num;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_thread begin session(%lx)",session_num);
   // Enter forever loop
   for(;;)
   {
      // ***************** Wait for the MASK
      // block on any one or more of selected queues and signals
      venc_ptr->rcvd_mask = qurt_elite_channel_wait(&venc_ptr->qurt_elite_channel, venc_ptr->wait_mask);

      while(venc_ptr->rcvd_mask)
      {
         venc_ptr->bit_pos = voice_get_signal_pos(venc_ptr->rcvd_mask);

         // De queue and signal clear done in the handler functions.
         result = pVencHandler[venc_ptr->bit_pos](venc_ptr);

         if(result != ADSP_EOK)
         {
            if (ADSP_ETERMINATED == result)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_thread end session(%lx)",session_num);
               return ADSP_EOK;
            }
            MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: Venc thread Handler Error bit_pos=%d,res=%d, session(%lx)",(int)venc_ptr->bit_pos, (int)result, session_num );
         }
         if(venc_ptr->thread_info.another_thread_launched)
         {
            // the old thread will come here, and exit at this point
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_thread():Another thread launched. exiting current Tid(%ld) session(%lx), cachedTid(%ld)",
                qurt_thread_get_id(),session_num,venc_ptr->thread_info.cached_tid);
            return ADSP_EOK;
         }
         venc_ptr->rcvd_mask = qurt_elite_channel_poll(&venc_ptr->qurt_elite_channel, venc_ptr->wait_mask);
      } // end of while loop
   } // end of for loop
   return 0;
}

static ADSPResult venc_vfr_handler(void *instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t *venc_ptr = (venc_t*)instance_ptr;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: venc_vfr_handler begin session(%x)",(int)venc_ptr->session_num);
   //qurt_elite_signal_clear(venc_ptr->vtm_sub_unsub_data.signal_ptr);
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: venc_vfr_handler end session(%x)",(int)venc_ptr->session_num);

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Error! venc_vfr_handler should not get invoked, session(%x)",(int)venc_ptr->session_num);
   return result;
}

static ADSPResult venc_buf_handler(void *instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t *venc_ptr = (venc_t*)instance_ptr;
   MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: venc_buf_handler begin session(%x)",(int)venc_ptr->session_num);

   MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: venc_buf_handler end session(%x)",(int)venc_ptr->session_num);
   return result;
}

static ADSPResult venc_rec_buf_handler(void *instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t *venc_ptr = (venc_t*)instance_ptr;
   qurt_elite_bufmgr_node_t out_buf_mgr_node;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_buf_handler begin session(%x)",venc_ptr->session_num);

   if (FALSE == venc_ptr->process_data)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Not in RUN state, this should not happen!! session(%lx)",venc_ptr->session_num);
      return ADSP_ENOTREADY;
   }
   // Take next buffer off the Q
   if ( TRUE == venc_ptr->record.enable )
   {
      if (TRUE == venc_ptr->send_media_type)
      {
         result = venc_send_afe_media_type(venc_ptr);
      }
      else
      {
         result = qurt_elite_queue_pop_front(venc_ptr->record.buf_q_ptr, (uint64_t*)&out_buf_mgr_node );
         if (ADSP_EOK != result)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error reading buf queue, result(%d) session(%lx)",result,venc_ptr->session_num);
         }
         else
         {
            result = venc_send_rec_buf_to_afe(venc_ptr, &out_buf_mgr_node);
            if (!ADSP_FAILED(result))
            {  // make the flag false if the result was success
               venc_ptr->record.first_frame = FALSE;
            }
         }
      }
   }
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_buf_handler end session(%x)",venc_ptr->session_num);
   return result;
}

static ADSPResult venc_vfr_end_handler(void *instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t *venc_ptr = (venc_t*)instance_ptr;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_vfr_end_handler begin session(%x)",(int)venc_ptr->session_num);

   qurt_elite_signal_clear(venc_ptr->vtm_sub_unsub_data.signal_end_ptr);

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_vfr_end_handler end session(%x)",(int)venc_ptr->session_num);
   return result;
}

static ADSPResult venc_resync_handler(void *instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t *venc_ptr = (venc_t*)instance_ptr;
   qurt_elite_signal_clear(venc_ptr->vtm_sub_unsub_data.resync_signal_ptr);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Venc resync from AFE, session(%x)", (int)venc_ptr->session_num);
   venc_ptr->resync_count++;
   // Start couting for resync. If already counting, this will reset the count
   venc_ptr->modules.ctm_tx.resync_delay_cnt = 1;
   // Clear recording ss_info if applicable
   if(venc_ptr->record.enable)
   {
      venc_ptr->record.ss_info_counter  = 0;
      venc_ptr->record.ss_multiframe_counter = 0;
      // reset device drift counter
      venc_ptr->record.voice_cmn_drift_info.is_resync_pending = 1;
   }
   return result;
}

/* Since we always wait synchronously for responses, this should never get invoked */
static ADSPResult venc_response_handler(void* instance_ptr)
{
   venc_t *venc_ptr = (venc_t*)instance_ptr;
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! venc_response_handler invoked, session(%x)",(int)venc_ptr->session_num);
   return ADSP_EFAILED;
}

static ADSPResult venc_vds_send_drop_now(void* instance_ptr)
{
   venc_t *venc_ptr = (venc_t*)instance_ptr;
   ADSPResult result;
   elite_msg_any_t msg;
   vds_resync_t* resync_params_ptr;
   uint32_t msg_size = sizeof(elite_msg_custom_vds_resync_type);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VENC VDS drop now, session(%x)", (int)venc_ptr->session_num);
   result = elite_msg_create_msg(&msg,
         &msg_size,
         ELITE_CUSTOM_MSG,
         venc_ptr->resp_q_ptr,
         NULL,
         ADSP_EOK);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VENC failed to create message, result %d, session(%x)", result, (int)venc_ptr->session_num);
      return result;
   }
   elite_msg_custom_vds_resync_type *vds_resync_msg = (elite_msg_custom_vds_resync_type*)msg.pPayload;
   resync_params_ptr = &vds_resync_msg->resync_params;
   vds_resync_msg->unSecOpCode = VDS_DROP_NOW;   
   resync_params_ptr->client_token = venc_ptr->vds_client_token;
   resync_params_ptr->client_id = venc_ptr->vds_client_id;
   // Issue command
   result = qurt_elite_queue_push_back(venc_ptr->vds_handle_ptr->cmdQ, (uint64_t*)&msg);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VENC failed to deliver command, result %d, session(%x)", result, (int)venc_ptr->session_num);
      elite_msg_return_payload_buffer(&msg);
      return result;
   }
   // Wait for ack
   result = elite_svc_wait_for_ack(&msg);
   if(ADSP_FAILED(result) || ADSP_FAILED(vds_resync_msg->unResponseResult))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VENC wait failed, result %d, session(%x)", result, (int)venc_ptr->session_num);
      elite_msg_return_payload_buffer(&msg);
      return ADSP_EFAILED;
   }

   // Return buffer
   elite_msg_return_payload_buffer(&msg);

   return result;
}

static ADSPResult venc_vds_response_handler(void* instance_ptr)
{
   venc_t *venc_ptr = (venc_t*)instance_ptr;

   qurt_elite_signal_clear(venc_ptr->vds_error_signal_ptr); //clear the signal

   if(OUT_OF_BAND == venc_ptr->pkt_exchange_mode)
   {
      if(FALSE == venc_ptr->oob.enc_pkt_consumed)
      {
         if(FALSE == venc_ptr->pkt_delivery_pending)         // delivery miss is due to venc taking more time
         {
            //Send a signal to VDS to drop the buffer if already queued. this is required to avoid 
            //timestamp corruption when venc misses two packet deliveries back to back. 
            venc_vds_send_drop_now(venc_ptr);
            venc_ptr->oob.enc_pkt_consumed = TRUE;    // Set the flag to true, as we have received one buff consumed corresponding to which we have not delivered any packet yet
         }

         venc_ptr->pkt_delivery_pending = FALSE;      // make it false as we dont want venc to queue data to VDS when OOB consumed event is delayed
         venc_ptr->oob.pkt_miss_ctr++;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! unexpected response from VDS failed delivery, session(%x)",(int)venc_ptr->session_num);
      }
   }
   else
   {
      venc_ptr->pkt_miss_ctr++;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VDS Failed to deliver Venc pkt, session(%x)",
         (int)venc_ptr->session_num);
   return ADSP_EOK;
}

static void venc_cal_sample_slip_stuff(venc_t* venc_ptr, int16_t* slip_stuff_samples)
{
   //Check if any samples needs to be corrected
   if(0 != venc_ptr->record.ss_info_counter)
   {
      //Check if we are done with previous correction
      if(0 == venc_ptr->record.ss_multiframe_counter)
      {
         //reset multi frame counter
         venc_ptr->record.ss_multiframe_counter = VENC_SS_MULTIFRAME;

         if(0 > venc_ptr->record.ss_info_counter)
         {
            // This means DMS is faster, then stuff one samples
            *slip_stuff_samples = -1;

            //decrease (magniture) of ss_info counter
            venc_ptr->record.ss_info_counter += (int32_t)1; // arithmetic to make the slip stuff counter zero
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ICCR:: venc sample stuff !!  count (%d) session(%x)",*slip_stuff_samples,(int)venc_ptr->session_num);
         }
         else  // venc_ptr->record.ss_info_counter>0
         {
            // This means DMS is slow, then slip one samples
            *slip_stuff_samples = 1;
            //decrease (magniture) of ss_info counter
            venc_ptr->record.ss_info_counter -= (int32_t)1; // arithmetic to make the slip counter zero
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ICCR:: venc sample slip !!  count (%d) session(%x)",*slip_stuff_samples,(int)venc_ptr->session_num);
         }
      }
   }//  if(0 != venc_ptr->record.ss_info_counter)
}

static void inline venc_cal_drift(venc_t* venc_ptr)
{
   voice_cmn_accu_drift(&venc_ptr->record.ss_info_counter,
         &venc_ptr->record.voice_cmn_drift_info,
         venc_ptr->record.afe_drift_ptr,
         venc_ptr->vfr_source,
         VOICE_ENC_REC,
         venc_ptr->vfr_mode,
         venc_ptr->session_num,
         venc_ptr->vtm_sub_unsub_data.timing_ver,
         venc_ptr->vsid);
}

static ADSPResult venc_cmd_handler(void *instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t *venc_ptr = (venc_t*)instance_ptr;
   uint32_t session_num = venc_ptr->session_num;
   elite_msg_any_t msg;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_cmd_handler begin session(%lx)",session_num);
   result = qurt_elite_queue_pop_front(venc_ptr->svc_handle.cmdQ, (uint64_t*) &msg);
   // ***************** Process the msg
   if (ADSP_EOK == result)
   {
      const uint32_t cmdTableSize = sizeof(pEliteHandler)/sizeof(pEliteHandler[0]);
      if (msg.unOpCode >= cmdTableSize)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Unsupported message ID 0x%8lx!! session(%lx)", msg.unOpCode,session_num);
         return ADSP_EUNSUPPORTED;
      }
      // table lookup to call handling function, with FALSE to indicate processing of msg
      if(TRUE == venc_ptr->fatal_state)
      {
         // in fatal state only accept command for media type or destroy.
         if((2 == msg.unOpCode)||(11 == msg.unOpCode))
         {
            result = pEliteHandler[msg.unOpCode](instance_ptr, &msg);
         }
         else
         {
            result = ADSP_EFAILED;
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Venc in fatal state result=%x session(%lx)",result,session_num);
            return result;
       	 }
      }
      else
      {
         result = pEliteHandler[msg.unOpCode](instance_ptr, &msg);
      }
   }
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_cmd_handler end result=%x session(%lx)",result,session_num);
   return result;
}

static ADSPResult venc_custom_msg( void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*) instance_ptr;
   elite_msg_custom_header_t *pCustom = (elite_msg_custom_header_t *) msg_ptr->pPayload;

   if( pCustom->unSecOpCode < VENC_NUM_MSGS)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Venc_Cmd, SecOpcode: %ld session(%x)",pCustom->unSecOpCode ,(int)venc_ptr->session_num);
      result = pHandler[pCustom->unSecOpCode](instance_ptr, msg_ptr);
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Unsupported SecOpcode message ID 0x%8lx!! session(%x)", pCustom->unSecOpCode,(int)venc_ptr->session_num);
      result = ADSP_EFAILED;
   }
   return result;
}

static ADSPResult venc_run_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   ADSPResult result = ADSP_EOK;
   uint8_t tty_enable = FALSE;
   uint8_t oobtty_enable = FALSE;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_run_cmd begin session(%x)",(int)venc_ptr->session_num);

   // reset stream pp capi modules if,
   // 1. it is a new call
   // 2. vocoder is changed in the same call.
   if (venc_ptr->reset_voc_flag)
   {
      venc_stream_pp_capi_send_algo_reset(venc_ptr);
   }

   // if here,it session is already in STOP state.(RUN or STOP is already is checked in VSM)

   /* Must handle all vocoder init, post proc init, TTY init, sample rate dependent init here!! */
   /* TODO:  appears that some post proc not setup via CommitFormat! */

   if (OUT_OF_BAND == venc_ptr->pkt_exchange_mode)
   {
      venc_ptr->oob.enc_pkt_consumed = TRUE;    // force the flag to true to send first packet
   }

   if(TRUE == venc_ptr->modules.cng.fwk_extn_list.is_cng_fwk_extn_supported)
   {
      voc_param_cng_tx_mute_t tx_mute;
      tx_mute.is_mute = (venc_ptr->mute == VOICE_MUTE)? TRUE: FALSE;
      venc_capi_set_param(&venc_ptr->modules.cng, VOC_PARAM_CNG_TX_MUTE, &tx_mute,
                          sizeof(voc_param_cng_tx_mute_t), sizeof(voc_param_cng_tx_mute_t));
   }

   voc_param_cng_is_tty_mode_full_t tty_mode;
   tty_mode.is_tty_mode_full = (venc_ptr->tty.state_ptr->m_etty_mode == VSM_TTY_MODE_FULL) ? TRUE : FALSE;

#ifdef SIM
   /* todo: Need to find an ISOD supported way to disable the muting (or CNG) for TTY loopback SIM testing */
   if(TRUE == venc_ptr->modules.cng.fwk_extn_list.is_cng_fwk_extn_supported)
   {
      tty_mode.is_tty_mode_full = FALSE;

      venc_capi_set_param(&venc_ptr->modules.cng, VOC_PARAM_CNG_IS_TTY_MODE_FULL, &tty_mode,
                          sizeof(voc_param_cng_is_tty_mode_full_t), sizeof(voc_param_cng_is_tty_mode_full_t));
   }
#else
   /* enable Tx CNG muting when Rx TTY detected only for TTY FULL mode */
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Tx CNG Mute on Rx TTY Det: Enable(%d)", tty_mode.is_tty_mode_full);
   venc_capi_set_param(&venc_ptr->modules.cng, VOC_PARAM_CNG_IS_TTY_MODE_FULL, &tty_mode,
                       sizeof(voc_param_cng_is_tty_mode_full_t), sizeof(voc_param_cng_is_tty_mode_full_t));
#endif

   // init sample slipping algorithm
   voice_sample_slip_init(&(venc_ptr->record.ss_struct),((venc_ptr->samp_rate_factor)*VOICE_FRAME_SIZE_NB_10MS),VENC_SS_MULTIFRAME);

   //update nb sampling factor for drift detection
   venc_init_rec_circbuf(venc_ptr);
   venc_ptr->record.voice_cmn_drift_info.nb_sampling_rate_factor = venc_ptr->samp_rate_factor;

   /* latch all settings when going into Run state */
   oobtty_enable = (venc_ptr->tty.state_ptr->m_oobtty_mode == VSM_TTY_MODE_HCO || venc_ptr->tty.state_ptr->m_oobtty_mode == VSM_TTY_MODE_FULL);


   /* handle CDMA TTY/CTM init */
   tty_enable = (venc_ptr->tty.state_ptr->m_etty_mode == VSM_TTY_MODE_HCO || venc_ptr->tty.state_ptr->m_etty_mode == VSM_TTY_MODE_FULL);

   /* For CTM (GSM/UMTS voc), if we are in VCO mode, we still need to enable TTY algorithm.  This
    * is because when the far end sends CTM, and Rx TTY algorithm detects, we need to acknowledge this
    * with a CTM ack.  If not the far end could revert to bypass mode.
    */
   //KA: This stuff to be taken care of with 1x TTY/CTM migration to CAPI
   switch(venc_ptr->voc_type)
   {
      case VSM_MEDIA_TYPE_AMR_NB_MODEM:
      case VSM_MEDIA_TYPE_AMR_NB_IF2:
      case VSM_MEDIA_TYPE_AMR_WB_MODEM:
      case VSM_MEDIA_TYPE_EFR_MODEM:
      case VSM_MEDIA_TYPE_FR_MODEM:
      case VSM_MEDIA_TYPE_HR_MODEM:
      case VSM_MEDIA_TYPE_EAMR:
      case VSM_MEDIA_TYPE_EVS:
         {
            //KA: This can stay here since it's CTM specific. However, a general PARAM_TTY_ENABLE should be issued to all
            //vocoders, with gsm vocoders simply ignoring it and returning success
            // also enable TTY algorithm for CTM VCO  case, in addition to normal HCO and FULL cases
            if( venc_ptr->tty.state_ptr->m_etty_mode == VSM_TTY_MODE_VCO)
            {
               tty_enable = TRUE;
            }
            break;
         }
      default:
         {
            break; // do nothing
         }
   }
   // TTY should not be enable for both CS and PS mode
   if ((tty_enable)&&(oobtty_enable))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error ! Both CS and PS TTY enabled ! : session(%lx)",venc_ptr->session_num);
      elite_svc_send_ack(msg_ptr, ADSP_EFAILED);
      return ADSP_EFAILED;
   }
   // initialize vocoder only when necessary (handover to different media type, or after reinit command)
   if(venc_ptr->is_encoder_capi)
   {
      //If current encoder is a capi, it has already been allocated at set_media_type. nothing else needs to happen here
      // if it isnt, then enc_state_ptr may need to be allocated
      venc_ptr->reset_voc_flag = FALSE;
   }
   else
   {
      if( venc_ptr->reset_voc_flag )
      {
         venc_ptr->reset_voc_flag = FALSE;

         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Init venc in run cmd : session(%lx)",venc_ptr->session_num);
         venc_ptr->send_media_type = TRUE;


         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Invalid encoder type: 0x%x",(int)venc_ptr->voc_type);
         venc_ptr->voc_type = VSM_MEDIA_TYPE_NONE;
         result = ADSP_EBADPARAM;
         /** terminate processing here itself if vocoder type was wrong **/
         elite_svc_send_ack(msg_ptr, result);
         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_run_cmd end result=%x  session(%x)",(int)result,(int)venc_ptr->session_num);
         return result;
      }
   }

   // Subscribe to VDS. Should be done before VTM subscribe so that signal is created first
   result = voice_cmn_send_vds_command(&venc_ptr->vds_client_id, venc_ptr->vds_client_token, venc_ptr->resp_q_ptr, VDS_SUBSCRIBE,
         &venc_ptr->vtm_sub_unsub_data.signal_ptr, venc_ptr->vds_error_signal_ptr, venc_ptr->vds_handle_ptr, venc_ptr->session_num, (VFR_NONE != venc_ptr->vfr_mode),NULL);
   if(ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Venc subscribe to VDS, session(%x)!",(int)venc_ptr->session_num);
      return result;
   }

   // Subscribe to VoiceTimer. todo: move to SetVfrMode and sub only for hard vfr and handle != NULL
   venc_ptr->vtm_sub_unsub_data.signal_enable  = (VFR_NONE == venc_ptr->vfr_mode)? 0: 1;
   venc_vtm_subscribe(venc_ptr,&(venc_ptr->vtm_sub_unsub_data));
   venc_ptr->enc_tick_timestamp_us = 0;
   if (VFR_NONE == venc_ptr->vfr_mode)
   {
      (void) qurt_elite_channel_wait(&(venc_ptr->qurt_elite_channel), VENC_TICK_END_MASK);
      qurt_elite_signal_clear(venc_ptr->vtm_sub_unsub_data.signal_end_ptr); //clear the signal
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VoiceEnc::Run mediaType(%lx),ttyEnable(%d)",venc_ptr->voc_type,tty_enable);
   venc_aggregate_modules_delay(venc_ptr);
   if ( ADSP_EOK == result )
   {
      venc_ptr->wait_mask |= (VENC_TICK_MASK | VENC_RESYNC_MASK | VENC_VDS_RESPONSE_MASK);     // Begin listening for ticks/resync
      venc_ptr->process_data    = 1; // Start processing - RUN state
   }

   // Send service state information to encoder capi module if module has requrested
   if((venc_ptr->is_encoder_capi) && (venc_ptr->encoder_capi.common.fwk_extn_list.is_service_state_fwk_extn_supported))
   {
      service_state_payload_t venc_service_state;
      venc_service_state.main_state = SERVICE_RUN_STATE;

      venc_capi_set_param(&venc_ptr->encoder_capi.common, PARAM_ID_SERVICE_STATE, &venc_service_state, sizeof(venc_service_state), sizeof(venc_service_state));
   }
   // subscribe to avtimer for some time stamp calculations
   avtimer_open_param_t open_param;
   open_param.client_name = (char*)"VEC"; // use the same client name as VTM to avoid extra registrations
   open_param.flag = 0;
   if (ADSP_EOK != (result = avtimer_drv_hw_open(&(venc_ptr->avt_drv), &open_param)))
   {
     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to Open AVTimer Driver for venc session(%x)",(int)venc_ptr->session_num);
   }
   voice_cmn_time_profile_init( &venc_ptr->time_profile_struct);
   venc_ptr->resync_count = 0;
   elite_svc_send_ack(msg_ptr, result);
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_run_cmd end result=%x, wait_mask=%x, session(%x)",(int)result,(int)venc_ptr->wait_mask,(int)venc_ptr->session_num);
   return result;
}

static ADSPResult venc_destroy_yourself_cmd (void *instance_ptr, elite_msg_any_t* msg_ptr)
{
   venc_destroy((venc_t*)instance_ptr);

   // send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
   return ADSP_ETERMINATED;
}

static ADSPResult venc_set_media_type(void *instance_ptr, elite_msg_any_t* msg_ptr)
{

   ADSPResult result = ADSP_EOK;
   venc_t *venc_ptr = (venc_t*)instance_ptr;
   elite_msg_data_media_type_header_t* media_type_msg_payload;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_media_type begin session(%x)",(int)venc_ptr->session_num);

   /* only handle VocType changes in stop state.  Latch all settings perform init/TTY init/postproc init/postproc
    * sample rate dependent init when transitioning to Run state
    */
   if( TRUE == venc_ptr->process_data)
   {
      result = ADSP_EBUSY;
   }
   else
   {
      media_type_msg_payload = (elite_msg_data_media_type_header_t*) msg_ptr->pPayload;
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_media_type: old_MT(%lx) new_MT(%lx)",venc_ptr->voc_type, media_type_msg_payload->unMediaTypeFormat);
      // set reset flag if we need to reset vocoder.  Latch flag high, only reset when actually processed (Run)
      if( venc_ptr->voc_type != media_type_msg_payload->unMediaTypeFormat)
      {
         venc_ptr->reset_voc_flag = TRUE;

         if(venc_ptr->is_encoder_capi)
         {
            result = venc_encoder_capi_destroy(venc_ptr);
         }
         else
         {
            // Media type change and hence reset encoder config params
            (void) venc_enc_params_init(venc_ptr);

            //Optionally free up memory from enc_state_ptr if allocated
            // this is just for temporary memory saving, to ensure that both aren't simultaneously allocated
            if(venc_ptr->encoder.enc_state_ptr)
            {
               qurt_elite_memory_free(venc_ptr->encoder.enc_state_ptr);
               venc_ptr->encoder.enc_state_ptr = NULL;
            }
         }

         //check if the VOC is a CAPI
         venc_ptr->is_encoder_capi = voice_check_voc_capi(media_type_msg_payload->unMediaTypeFormat);

         if(venc_ptr->is_encoder_capi)
         {
            /*
             * Create encoder capi, if there is a failure, return error and set to none..
             */

            result = venc_encoder_capi_init(venc_ptr, media_type_msg_payload->unMediaTypeFormat);
            if(ADSP_FAILED(result))
            {
               venc_encoder_capi_destroy(venc_ptr);
               elite_svc_send_ack(msg_ptr, result);
               return result;
            }
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Unsupported non-capi media type 0x%x",(int)media_type_msg_payload->unMediaTypeFormat);
            result = ADSP_EBADPARAM;
            elite_svc_send_ack(msg_ptr, result);
            return result;
         }

         // send input media format to all the stream pp modules if sampling rate is changed.
         if (venc_ptr->sampling_rate != venc_ptr->modules.input_media_type.data_format.sampling_rate)
         {
            result = venc_stream_pp_capi_set_input_media_fmt(venc_ptr);
            if(ADSP_EOK != result)
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_set_media_type: venc_stream_pp_capi_set_input_media_fmt: failed. result(0x%x), session(0x%lx)",
                     (uint32_t)result, venc_ptr->session_num);
               elite_svc_send_ack(msg_ptr, result);
               return result;
            }
         }

         //add service thread stack and check if the stack size of encoder is less than minimum
         uint32_t stack_size = SERVICE_THREAD_STACK_SIZE + venc_ptr->encoder_capi.common.stack_size ;
         if(stack_size < MIN_THREAD_STACK_SIZE)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Thread size for encoder_capi(%ld) is less than minimum(%ld). Setting to min value",
                  stack_size,MIN_THREAD_STACK_SIZE);
            stack_size = MIN_THREAD_STACK_SIZE;
         }

         result = venc_launch_thread(venc_ptr,TRUE,stack_size);
         if(ADSP_FAILED(result))
         {
            elite_svc_send_ack(msg_ptr, result);
            return result;
         }
      }
      else
      {
         // Send event if vocoder mediatype has not changed
         venc_send_mode_notification_v2(instance_ptr);
      }
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_media_type end  session(%x), result(%lx)",(int)venc_ptr->session_num,result);
   elite_svc_send_ack(msg_ptr, result);
   return result;
}

static ADSPResult venc_set_pp_samp_rate(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_voc_stream_set_sampling_type *set_samp_rate_cmd_ptr = (elite_msg_custom_voc_stream_set_sampling_type *) msg_ptr->pPayload;

   if(venc_ptr->is_encoder_capi)
   {
      if(venc_ptr->encoder_capi.input_media_type.data_format.sampling_rate != set_samp_rate_cmd_ptr->tx_samp_rate)
      {
         result =  venc_encoder_capi_set_media_fmt(venc_ptr,set_samp_rate_cmd_ptr->tx_samp_rate);
         if(ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_set_pp_samp_rate: set_media_fmt: failed with result(%ld)", result);
            elite_svc_send_ack (msg_ptr, result);
            return result;
         }
      }
      venc_ptr->sampling_rate = venc_ptr->encoder_capi.input_media_type.data_format.sampling_rate;
      venc_ptr->samp_rate_factor = venc_ptr->sampling_rate/VOICE_NB_SAMPLING_RATE;
      venc_ptr->io.frame_samples = VOICE_FRAME_SIZE_NB_20MS*(venc_ptr->samp_rate_factor);
   }
   // send input media format to all the stream pp modules if sampling rate is changed.
   if (venc_ptr->sampling_rate != venc_ptr->modules.input_media_type.data_format.sampling_rate)
   {
      result = venc_stream_pp_capi_set_input_media_fmt(venc_ptr);
      if(ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_set_media_type: venc_stream_pp_capi_set_input_media_fmt: failed. result(0x%x), session(0x%lx)",
               (uint32_t)result, venc_ptr->session_num);
         elite_svc_send_ack(msg_ptr, result);
         return result;
      }
   }
   elite_svc_send_ack(msg_ptr, result);
   return result;
}

static ADSPResult venc_set_tty_mode(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_voc_stream_set_tty_mode *set_tty_mode_cmd_ptr = (elite_msg_custom_voc_stream_set_tty_mode *) msg_ptr->pPayload;

   // send tty mode to ctm tx capi always.
   tty_param_mode_t tty_mode;
   tty_mode.mode = set_tty_mode_cmd_ptr->tty_mode;

   switch(venc_ptr->voc_type)
   {
      case VSM_MEDIA_TYPE_AMR_NB_MODEM:
      case VSM_MEDIA_TYPE_AMR_NB_IF2:
      case VSM_MEDIA_TYPE_AMR_WB_MODEM:
      case VSM_MEDIA_TYPE_EFR_MODEM:
      case VSM_MEDIA_TYPE_FR_MODEM:
      case VSM_MEDIA_TYPE_HR_MODEM:
      case VSM_MEDIA_TYPE_EAMR:
      case VSM_MEDIA_TYPE_EVS:
      {
         if(TRUE == venc_ptr->modules.ctm_tx.ctm_tx_capi.intf_extn_list.is_tty_intf_supported)
         {
            result = venc_capi_set_param(&venc_ptr->modules.ctm_tx.ctm_tx_capi, TTY_PARAM_MODE, &tty_mode, sizeof(tty_mode), sizeof(tty_mode));
            if(ADSP_FAILED(result))
            {
               elite_svc_send_ack(msg_ptr, result);
               return result;
            }
         }
         break;
      }
      //KA: All this to be converted to just set_param once 1x TTY is migrated
      case VSM_MEDIA_TYPE_13K_MODEM:
      case VSM_MEDIA_TYPE_EVRC_MODEM:
      case VSM_MEDIA_TYPE_4GV_NB_MODEM:
      case VSM_MEDIA_TYPE_4GV_WB_MODEM:
      case VSM_MEDIA_TYPE_4GV_NW_MODEM:
      case VSM_MEDIA_TYPE_4GV_NW:
      case VSM_MEDIA_TYPE_EVRC_NW_2K:
      {
         result = venc_capi_set_param(&venc_ptr->encoder_capi.common, TTY_PARAM_MODE, &tty_mode, sizeof(tty_mode), sizeof(tty_mode));
         if(ADSP_FAILED(result))
         {
            elite_svc_send_ack(msg_ptr, result);
            return result;
         }
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_set_tty_mode: tty mode received for unsupported media_type(0x%x)",(int)venc_ptr->voc_type);
         break; // do nothing
      }
   }
   elite_svc_send_ack(msg_ptr, result);
   return result;
}

static ADSPResult venc_set_ltetty_mode(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_voc_stream_set_tty_mode *set_tty_mode_cmd_ptr = (elite_msg_custom_voc_stream_set_tty_mode *) msg_ptr->pPayload;

   // send tty mode to ltetty tx capi always.
   tty_param_mode_t ltetty_mode;
   ltetty_mode.mode = set_tty_mode_cmd_ptr->tty_mode;

   if(TRUE == venc_ptr->modules.ltetty_tx.intf_extn_list.is_tty_intf_supported)
   {
      result = venc_capi_set_param(&venc_ptr->modules.ltetty_tx, TTY_PARAM_MODE, &ltetty_mode, sizeof(ltetty_mode), sizeof(ltetty_mode));
      if(ADSP_FAILED(result))
      {
         elite_svc_send_ack(msg_ptr, result);
         return result;
      }
   }
   elite_svc_send_ack(msg_ptr, result);
   return result;
}

static ADSPResult venc_stop_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   venc_t* venc_ptr = (venc_t*) instance_ptr;
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_stop_cmd begin session(%x)",(int)venc_ptr->session_num);

   venc_ptr->process_data    = 0; // Stop processing

   venc_vtm_unsubscribe(venc_ptr,&(venc_ptr->vtm_sub_unsub_data),VENC_TICK_END_MASK);

   // Unsubscribe from VDS. Should be done after timer unsubscribe
   result = voice_cmn_send_vds_command(&venc_ptr->vds_client_id, venc_ptr->vds_client_token, venc_ptr->resp_q_ptr, VDS_UNSUBSCRIBE,
         &venc_ptr->vtm_sub_unsub_data.signal_ptr, venc_ptr->vds_error_signal_ptr, venc_ptr->vds_handle_ptr, venc_ptr->session_num, TRUE,NULL);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed Venc unsubscribe to VDS session(%x)!",(int)venc_ptr->session_num);
      return result;
   }

   venc_ptr->wait_mask &= ~(VENC_TICK_MASK | VENC_RESYNC_MASK | VENC_REC_BUF_MASK | VENC_VDS_RESPONSE_MASK) ; // Stop listening to Output buffers, ticks and errors from VDS.

   // Reset control code params
   (void) venc_ctrl_params_init(venc_ptr);
   avtimer_drv_hw_close(venc_ptr->avt_drv);
   // OOB statistics
   if (venc_ptr->pkt_exchange_mode == OUT_OF_BAND)
   {
      MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Venc OOB Pkt miss count from call start %ld, miss due previous pkt not consumed (%d) total pkts sent %ld, "
            "total packets consumed by client %ld session(%lx)",venc_ptr->pkt_miss_ctr, (int)venc_ptr->oob.pkt_miss_ctr, venc_ptr->pkt_ctr, venc_ptr->oob.pkt_consumed_ctr,venc_ptr->session_num);
   }
   // print out profiling stats
   if( venc_ptr->time_profile_struct.num_samples > 0)
   {
      MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_stop session(%x) voc_type(%x): profiler microsec: num_samples(%d) max_time(%d) min_time(%d) avg_time(%d) max_cycles(%d) resync count(%ld)",
            (int)venc_ptr->session_num,
            (int)venc_ptr->voc_type,
            (int)venc_ptr->time_profile_struct.num_samples,
            (int)venc_ptr->time_profile_struct.max_time,
            (int)venc_ptr->time_profile_struct.min_time,
            (int)(venc_ptr->time_profile_struct.total_time / venc_ptr->time_profile_struct.num_samples),
            (int) venc_ptr->time_profile_struct.max_cycles,
            (int) venc_ptr->resync_count  );
   }

   //if soft mute is in ramping state, it should be disabled in stop command and bring into
   //the target state (either MUTE/UNMUTE) immediately.Because we dont know after how much
   //time run issued and it makes no sense to carry the ramp state after run.
   CrossFadeDataType *mute_data_ptr = &(venc_ptr->proc.cross_fade_data_struct);

   mute_data_ptr->state = STEADY_STATE;

   // Send service state information to encoder capi module if module has requrested
   if((venc_ptr->is_encoder_capi) && (venc_ptr->encoder_capi.common.fwk_extn_list.is_service_state_fwk_extn_supported))
   {
      service_state_payload_t venc_service_state_t;
      venc_service_state_t.main_state = SERVICE_STOP_STATE;

      venc_capi_set_param(&venc_ptr->encoder_capi.common, PARAM_ID_SERVICE_STATE, &venc_service_state_t, sizeof(venc_service_state_t), sizeof(venc_service_state_t));
   }

   // ack back to msg sender
   elite_svc_send_ack(msg_ptr, result);

   if (TRUE == venc_ptr->record.enable)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_stop_cmd, In call conversation Record has not been stopped");
   }

   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_stop_cmd end result=%x, wait_mask=%x, session(%x)",(int)result,(int)venc_ptr->wait_mask,(int)venc_ptr->session_num);
   return result;
}

// todo: use fadd msg ?
static ADSPResult venc_connect_dwn_stream_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_cmd_connect_t* connect_msg_payload_ptr;
   elite_svc_handle_t *svc2_connect_ptr;
   venc_t* venc_ptr = (venc_t*)instance_ptr;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_connect_dwn_stream_cmd begin  session(%x)",(int)venc_ptr->session_num);

   if (venc_ptr->process_data)
   {
      result = ADSP_ENOTREADY; // Cannot reconnect in RUN state. todo: needed?
      elite_svc_send_ack(msg_ptr, result);
      return result;
   }

   connect_msg_payload_ptr = (elite_msg_cmd_connect_t*) (msg_ptr->pPayload);
   svc2_connect_ptr = connect_msg_payload_ptr->pSvcHandle;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Venc connecting to SvcID 0x%8x session(%x)",(int)svc2_connect_ptr->unSvcId,(int)venc_ptr->session_num);

   // This service only allows one downstream
   if (NULL != venc_ptr->downstream_peer_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Venc peer connection failed, already connected  session(%x)",(int)venc_ptr->session_num);
      result = ADSP_EUNSUPPORTED;
   }
   else //else accept the connection
   {
      venc_ptr->downstream_peer_ptr = svc2_connect_ptr;
   }
   elite_svc_send_ack(msg_ptr, result);

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_connect_dwn_stream_cmd end result=%x  session(%x)",(int)result,(int)venc_ptr->session_num);
   return result;
}

static ADSPResult venc_disconnect_dwn_stream_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_cmd_connect_t* disconnect_msg_payload_ptr;
   elite_svc_handle_t *svc2_disconnect_ptr;
   venc_t* venc_ptr = (venc_t*)instance_ptr;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Venc_DisConnectDwnStreamCmd begin  session(%x)",(int)venc_ptr->session_num);

   disconnect_msg_payload_ptr = (elite_msg_cmd_connect_t*) (msg_ptr->pPayload);
   svc2_disconnect_ptr = disconnect_msg_payload_ptr->pSvcHandle ;
   if (svc2_disconnect_ptr != venc_ptr->downstream_peer_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: FAILED, not connected to that service. session(%x)",(int)venc_ptr->session_num);
      result = ADSP_EBADPARAM;
   }
   else //else accept the dis-connection
   {
      venc_ptr->downstream_peer_ptr = NULL;
   }

   elite_svc_send_ack(msg_ptr, result);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Venc_DisConnectDwnStreamCmd end result=%x  session(%x)",(int)result,(int)venc_ptr->session_num);
   return result;
}

static ADSPResult venc_set_param_int(venc_t* venc_ptr, uint32_t module_id, uint16_t instance_id, uint32_t param_id, void* payload_address_in, uint32_t param_size)
{
   ADSPResult result = ADSP_EOK;
   int8_t* calibration_data_pay_load_ptr = (int8_t*)payload_address_in;

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_set_param_int. mid(%#lx) pid(%#lx) psize(%#lx)",module_id,param_id,param_size);

   //check if set param is for encoder
   if(module_id == venc_ptr->voc_type)
   {
      result = venc_capi_set_param(&venc_ptr->encoder_capi.common, param_id, calibration_data_pay_load_ptr, param_size, param_size);
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_set_param_int. encoder(%#lx) completed set pid(%#lx) with result(%#lx)",
        module_id,param_id,result);
      return result;
   }

   switch (module_id)
   {
      case VOICESTREAM_MODULE_TX:
      {
         /* handle loopback param */
         if( VOICE_PARAM_LOOPBACK_ENABLE == param_id)
         {
            int16_t enable_flag = *((int16_t*)calibration_data_pay_load_ptr);
            if (sizeof(int32_t) != param_size)
            {
               result = ADSP_EBADPARAM;
            }
            else
            {
               if( TRUE == enable_flag)
               {
                  venc_ptr->loopback_enable           = TRUE;
                  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_set_param_cmd Venc->Vdec Loopback Enable session(%x)",(int)venc_ptr->session_num);
               }
               else
               {
                  venc_ptr->loopback_enable           = FALSE;
                  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_set_param_cmd Venc->Vdec Loopback Disable session(%x)",(int)venc_ptr->session_num);
               }
               result = ADSP_EOK;
            }
         }
         break;
      }
      case VOICE_MODULE_ENC_CNG:
      {
         result = venc_capi_set_param(&venc_ptr->modules.cng, param_id, calibration_data_pay_load_ptr,
               param_size, param_size);
         break;
      }
      case VOICE_MODULE_TX_STREAM_LIMITER:
      {
         result = venc_capi_set_param(&venc_ptr->modules.limiter, param_id, calibration_data_pay_load_ptr,
               param_size, param_size);
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Unsupported set enc module 0x%lx", module_id);
         result = ADSP_EUNSUPPORTED;
         break;
      }
   }
   return result;
}

static ADSPResult venc_set_param_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_param_cmd begin  session(%x)",(int)venc_ptr->session_num);

   if (ELITE_CMD_SET_PARAM == msg_ptr->unOpCode)
   {
      elite_msg_param_cal_t* fadd_payload_ptr = (elite_msg_param_cal_t*) msg_ptr->pPayload;
      if( (ELITEMSG_PARAM_ID_CAL == fadd_payload_ptr->unParamId) ||
            (ELITEMSG_PARAM_ID_CAL_V2 == fadd_payload_ptr->unParamId) )
      {
         uint32_t byte_size_counter=0;
         int8_t *calibration_data_pay_load_ptr;
         uint32_t nPayloadSize = fadd_payload_ptr->unSize;
         uint32_t payload_address = (uint32_t) fadd_payload_ptr->pnParamData;

         {
            ADSPResult local_result = ADSP_EUNSUPPORTED;
            uint32_t module_id, param_id;
            uint16_t instance_id, param_size,header_size;
            /* initialize to unsupported in case no valid module_id is found */

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

               calibration_data_pay_load_ptr = (int8_t*)payload_address + header_size;

               local_result = venc_set_param_int(venc_ptr, module_id, instance_id, param_id, calibration_data_pay_load_ptr, param_size);
               if (!((ADSP_EOK == local_result) || (ADSP_EUNSUPPORTED == local_result)))
               {
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: set_param failed for module(%lx) param id (%lx) session(%lx). Aborting",
                        module_id,param_id,venc_ptr->session_num);
                  break; // in case of any error dont go forward with parsing
               }
               byte_size_counter += (header_size + param_size);
               payload_address += (header_size + param_size);
            } while(byte_size_counter < nPayloadSize);
            elite_svc_send_ack(msg_ptr, local_result);
         }
      }
      else
      {
         elite_svc_send_ack(msg_ptr, ADSP_EFAILED);
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid param id (%lx) session(%lx)",fadd_payload_ptr->unParamId,venc_ptr->session_num);
      }
   }
   else
   {
      result = ADSP_EFAILED;
      elite_svc_send_ack(msg_ptr, result);
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_param_cmd secondary opcode not supported session(%lx)",venc_ptr->session_num);

   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_param_cmd end result=%x  session(%x)",(int)result,(int)venc_ptr->session_num);
   return ADSP_EOK;
}
// todo: add meat
static ADSPResult venc_get_param_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   venc_t* venc_ptr = (venc_t*) instance_ptr;
   ADSPResult result = ADSP_EOK;
   uint32_t module_id, param_id,*param_size_ptr;
   uint16_t instance_id,header_size = 0;
   elite_msg_param_cal_t* fadd_payload_ptr = (elite_msg_param_cal_t*) msg_ptr->pPayload;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_get_param_cmd begin session(%lx)",venc_ptr->session_num);

   if (ELITE_CMD_GET_PARAM == msg_ptr->unOpCode)
   {
      ADSPResult local_result = ADSP_EUNSUPPORTED;
      if( (ELITEMSG_PARAM_ID_CAL == fadd_payload_ptr->unParamId) ||
            (ELITEMSG_PARAM_ID_CAL_V2 == fadd_payload_ptr->unParamId))
      {
         //update module information based on cal version
         if(ELITEMSG_PARAM_ID_CAL == fadd_payload_ptr->unParamId)
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

         // Extract address of Cal data and size info
         // param_max_size in the interface includes the size of the param data as well as the header
         // so need to subtract header size to ensure param fits in allocated memory
         uint32_t param_size_max = *param_size_ptr - header_size;
         int8_t* calibration_data_pay_load_ptr = (int8_t*)(((int8_t*)fadd_payload_ptr->pnParamData) + header_size);

         {  // TODO: put the following in a big function which includes all the mods and parameters function
            switch (module_id)
            {
               case VOICESTREAM_MODULE_TX:
               {
                  if( VOICE_PARAM_LOOPBACK_ENABLE == param_id)
                  {
                     *param_size_ptr = sizeof(int32_t);

                     if( *param_size_ptr > param_size_max)
                     {
                        local_result = ADSP_ENOMEMORY;
                     }
                     else
                     {
                        *((int16_t*)calibration_data_pay_load_ptr) = (int16_t) venc_ptr->loopback_enable;
                        // Clear reserved word
                        *((int16_t*)calibration_data_pay_load_ptr+1) = 0;
                        local_result = ADSP_EOK;
                     }
                  }
                  break;
               }
               case VOICE_MODULE_ENC_CNG:
               {
                  local_result = venc_capi_get_param(&venc_ptr->modules.cng, param_id, calibration_data_pay_load_ptr, param_size_max, (uint16_t *)param_size_ptr );
                  break;
               }
               case VOICE_MODULE_TX_STREAM_LIMITER:
               {

                  local_result = venc_capi_get_param(&venc_ptr->modules.limiter, param_id, calibration_data_pay_load_ptr, param_size_max, (uint16_t *)param_size_ptr );
                  break;
               }
               default:
               {
                  MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Unsupported Venc module");
                  local_result = ADSP_EUNSUPPORTED;
                  break;
               }
            }
            MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Venc get param, mod id(0x%lx), instance(%d), param id (0x%lx), result(0x%x) session(0x%lx)",
                  module_id, instance_id, param_id, local_result, venc_ptr->session_num);
            result = local_result;
         } // Local scope
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Venc get param bad fadd id,  result(0x%x) session(0x%lx)\n",
               ADSP_EFAILED, venc_ptr->session_num);
         result = ADSP_EFAILED;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_get_param_cmd secondary opcode not supported :(%lx)\n",venc_ptr->session_num);
      result = ADSP_EFAILED;
   }

   if(ADSP_SUCCEEDED(result))
   {
      fadd_payload_ptr->unSize = *param_size_ptr;
   }
   else
   {
      fadd_payload_ptr->unSize = 0;
   }
   elite_svc_send_ack(msg_ptr, result);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_get_param_cmd end handler result(0x%x) session(0x%x)",result,(int)venc_ptr->session_num);
   return ADSP_EOK;
}

static ADSPResult venc_get_kpps_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EFAILED;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_kpps_type *kpps_msg_ptr= (elite_msg_custom_kpps_type*) msg_ptr->pPayload;
   vsm_get_kpps_ack_t* kpps_ptr = (vsm_get_kpps_ack_t*)kpps_msg_ptr->pnKpps;
   uint32_t kpps_changed=FALSE;

   if( kpps_ptr )
   {
      kpps_ptr->venc_kpps = 0;  // Initialization
      result = venc_aggregate_modules_kpps(instance_ptr, &kpps_changed);

      if(ADSP_SUCCEEDED(result))
      {
         kpps_ptr->venc_kpps = venc_ptr->aggregate_kpps;
      }
   }
   elite_svc_send_ack(msg_ptr, result);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_get_kpps_cmd end result(%d) session(%x)",(int)result,(int)venc_ptr->session_num);
   return ADSP_EOK;
}

static ADSPResult venc_get_delay_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EFAILED;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_delay_type *delay_msg_ptr= (elite_msg_custom_delay_type*) msg_ptr->pPayload;
   vsm_get_delay_ack_t* delay_ptr = (vsm_get_delay_ack_t*)delay_msg_ptr->delay_ptr;

   if( delay_ptr )
   {
      delay_ptr->venc_delay = 0;  // Initialization
      result = venc_aggregate_modules_delay(instance_ptr);

      if(ADSP_SUCCEEDED(result))
      {
         delay_ptr->venc_delay = venc_ptr->aggregate_delay;
      }
   }
   elite_svc_send_ack(msg_ptr, result);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_get_delay_cmd end result(%d) session(%x)",(int)result,(int)venc_ptr->session_num);
   return ADSP_EOK;
}


static ADSPResult venc_set_mute_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_voc_set_soft_mute_type *set_mute_cmd_ptr = (elite_msg_custom_voc_set_soft_mute_type *) msg_ptr->pPayload;
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_soft_mute begin mute(%d),ramp_duration(%d), session(%x)",
         (int)set_mute_cmd_ptr->mute,(int)set_mute_cmd_ptr->ramp_duration,(int)venc_ptr->session_num);

   if( set_mute_cmd_ptr->mute > VOICE_MUTE)
   {
      result = ADSP_EBADPARAM;
   }
   else if (venc_ptr->mute != set_mute_cmd_ptr->mute)
   {
      venc_ptr->proc.cross_fade_cfg_struct.iConvergeNumSamples = 0;
      venc_ptr->proc.cross_fade_cfg_struct.iTotalPeriodMsec = set_mute_cmd_ptr->ramp_duration;

      //handling soft mute-init upon receiving new command
      CrossFadeCfgType *mute_cfg_ptr = &(venc_ptr->proc.cross_fade_cfg_struct);
      CrossFadeDataType *mute_data_ptr = &(venc_ptr->proc.cross_fade_data_struct);
      if ( 0 != mute_cfg_ptr->iTotalPeriodMsec )
      {
         cross_fade_init( mute_cfg_ptr,mute_data_ptr,venc_ptr->sampling_rate );
         mute_data_ptr->state = CROSSFADE_STATE;
      }
      else
      {
         mute_data_ptr->state = STEADY_STATE;
      }

      // Set this state on Comfort Noise Generation
      venc_ptr->mute = set_mute_cmd_ptr->mute;
   }
   elite_svc_send_ack(msg_ptr, result);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_soft_mute end result(%d) session(%x)",(int)result,(int)venc_ptr->session_num);
   return result;
}


static ADSPResult venc_set_timing_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_voc_timing_param_type *set_timing_cmd_ptr = (elite_msg_custom_voc_timing_param_type *) msg_ptr->pPayload;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_timing_cmd begin session(%lx)",venc_ptr->session_num);

   if (FALSE == venc_ptr->process_data)
   {
      voice_set_timing_params_t* vfr_cmd_ptr = (voice_set_timing_params_t*)set_timing_cmd_ptr->param_data_ptr;
      MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VOICE_SET_TIMING_PARAMS,enc vfr_mode(%x), enc offset(%x), process_data(%x)",vfr_cmd_ptr->mode, vfr_cmd_ptr->enc_offset, venc_ptr->process_data);

      if ((VFR_HARD_EXT >= vfr_cmd_ptr->mode) )
      {
         venc_ptr->vfr_mode = vfr_cmd_ptr->mode;
      }
      else
      {
         venc_ptr->vfr_mode = VFR_NONE;
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VOICE_SET_TIMING_PARAMS, Invalid enc vfr_mode(%x),setting vfr_mode to VFR_NONE",vfr_cmd_ptr->mode);
         result = ADSP_EBADPARAM;
      }
      venc_ptr->vtm_sub_unsub_data.vfr_mode = (uint8_t)venc_ptr->vfr_mode;

      venc_ptr->vfr_source = voice_cmn_get_vfr_source(venc_ptr->vfr_mode);

      // MIN_TIMER_OFFSET is set to 0, so no need to check if offset is below min
      if (MAX_TIMER_OFFSET < vfr_cmd_ptr->enc_offset)
      {
         venc_ptr->vtm_sub_unsub_data.offset = 8000; // default 8ms
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VOICE_SET_TIMING_PARAMS, Invalid enc_offset(%x), Defaulting to 8msec",vfr_cmd_ptr->enc_offset);
         result = ADSP_EBADPARAM;
      }
      else
      {
         venc_ptr->vtm_sub_unsub_data.offset = vfr_cmd_ptr->enc_offset;
      }

      //update version of timing
      venc_ptr->vtm_sub_unsub_data.timing_ver = VFR_CLIENT_INFO_VER_1;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VFR mode can't be changed in RUN session(%lx)",venc_ptr->session_num);
      result = ADSP_EBUSY;
   }

   elite_svc_send_ack(msg_ptr, result);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_timing_cmd end result(%d) session(%lx)",result,venc_ptr->session_num);
   return result;
}
static ADSPResult venc_set_timingv2_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_voc_timing_param_type *set_timing_cmd_ptr = (elite_msg_custom_voc_timing_param_type *) msg_ptr->pPayload;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_timingv2_cmd begin session(%lx)",venc_ptr->session_num);

   // Verify Stop state of thread before updating timing values.
   if (FALSE == venc_ptr->process_data)
   {
      vsm_set_timing_params_t* vfr_cmd_ptr = (vsm_set_timing_params_t*)set_timing_cmd_ptr->param_data_ptr;
      MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VSM_CMD_SET_TIMING_PARAMS,enc vfr_mode(%x), enc offset(%x), process_data(%x)",vfr_cmd_ptr->mode, vfr_cmd_ptr->enc_offset, venc_ptr->process_data);

      // Verify validity of VFR mode
      if ((VFR_HARD_EXT >= vfr_cmd_ptr->mode) )
      {
         venc_ptr->vfr_mode = vfr_cmd_ptr->mode;
      }
      else
      {
         venc_ptr->vfr_mode = VFR_NONE;
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VSM_CMD_SET_TIMING_PARAMS, Invalid enc vfr_mode(%x),setting vfr_mode to VFR_NONE",vfr_cmd_ptr->mode);
         result = ADSP_EBADPARAM;
      }
      venc_ptr->vtm_sub_unsub_data.vfr_mode = (uint8_t)venc_ptr->vfr_mode;
      venc_ptr->vtm_sub_unsub_data.vfr_cycle = MAX_TIMER_OFFSET;

      // Select VFR source
      venc_ptr->vfr_source = voice_cmn_get_vfr_source(venc_ptr->vfr_mode);

      // MIN_TIMER_OFFSET is set to 0, so no need to check if offset is below min
      if (MAX_TIMER_OFFSET < vfr_cmd_ptr->enc_offset)
      {
         venc_ptr->vtm_sub_unsub_data.offset = 8000; // default 8ms
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VSM_CMD_SET_TIMING_PARAMS, Invalid enc_offset(%x), Defaulting to 8msec",vfr_cmd_ptr->enc_offset);
         result = ADSP_EBADPARAM;
      }
      else
      {
         venc_ptr->vtm_sub_unsub_data.offset = vfr_cmd_ptr->enc_offset;
      }

      //update version of timing
      venc_ptr->vtm_sub_unsub_data.timing_ver = VFR_CLIENT_INFO_VER_1;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VFR mode can't be changed in RUN session(%lx)",venc_ptr->session_num);
      result = ADSP_EBUSY;
   }

   elite_svc_send_ack(msg_ptr, result);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VSM_CMD_SET_TIMING_PARAMS end result(%d) session(%lx)",result,venc_ptr->session_num);
   return ADSP_EOK;
}

static ADSPResult venc_set_pkt_exchange_mode(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_pkt_exchange_mode_type *set_pkt_exchange_mode_ptr = (elite_msg_custom_pkt_exchange_mode_type *) msg_ptr->pPayload;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_pkt_exchange_mode begin session(%lx)",venc_ptr->session_num);

   if (FALSE == venc_ptr->process_data)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VENC_SET_PKT_EXCHANGE_MODE ,pkt_exchange_mode(%d) session(%lx)",
            (int)set_pkt_exchange_mode_ptr->pkt_exchange_mode,venc_ptr->session_num);
      venc_ptr->pkt_exchange_mode = set_pkt_exchange_mode_ptr-> pkt_exchange_mode;
      /* make packet exchange pointer NULL as config command with new addresses must follow a mode command */
      venc_ptr->oob.pkt_exchange_ptr = NULL;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: pkt_exchange_mode mode can't be changed in RUN session(%lx)",venc_ptr->session_num);
      result = ADSP_EBUSY;
   }
   elite_svc_send_ack(msg_ptr, result);
   return result;


}

static ADSPResult venc_set_oob_pkt_exchange_config(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_oob_pkt_exchange_config_type *set_oob_pkt_exchange_config_ptr = (elite_msg_custom_oob_pkt_exchange_config_type *) msg_ptr->pPayload;
   vsm_config_packet_exchange_t* pkt_exchange_config_ptr = (vsm_config_packet_exchange_t*)set_oob_pkt_exchange_config_ptr->param_data_ptr;


   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_pkt_exchange_config begin session(%lx)",venc_ptr->session_num);

   if (venc_ptr->process_data == TRUE)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: oob_pkt_exchange_config can't be come in RUN session(%lx) ",venc_ptr->session_num);
      result = ADSP_EBUSY;
   }
   else
   {
      venc_ptr->oob.memmap.unMemMapClient = vsm_memory_map_client;
      venc_ptr->oob.memmap.unMemMapHandle = pkt_exchange_config_ptr->mem_handle;
      result = elite_mem_map_get_shm_attrib(pkt_exchange_config_ptr->enc_buf_addr_lsw,
            pkt_exchange_config_ptr->enc_buf_addr_msw,
            pkt_exchange_config_ptr->enc_buf_size,
            &venc_ptr->oob.memmap);
      if( ADSP_FAILED( result ))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Mapping to virtual address for OOB pkt exchange failed for ENCODER result %d! session(%lx)",
               result,venc_ptr->session_num);
      }
      else
      {
         venc_ptr->oob.pkt_exchange_ptr = (int32_t *)venc_ptr->oob.memmap.unVirtAddr;
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VENC_SET_OOB_PKT_EXCHANGE_CONFIG ,venc OOB address(%#x)  size (%d) session(%lx) ",
               (unsigned int)venc_ptr->oob.pkt_exchange_ptr, (int)pkt_exchange_config_ptr->enc_buf_size,venc_ptr->session_num);
      }
   }
   elite_svc_send_ack(msg_ptr, result);
   return result;

}

static ADSPResult venc_reinit_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_reinit_cmd begin session(%lx)",venc_ptr->session_num);

   if (FALSE == venc_ptr->process_data)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_reinit_cmd process session(%lx)",venc_ptr->session_num);

      // Must reset vocoder on reinit command
      venc_ptr->reset_voc_flag = TRUE;

      //reinit stream pp capi modules
      result = venc_stream_pp_capi_reinit(venc_ptr);
      if(ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_reinit:  venc_stream_pp_capi_reinit: failed. result(0x%ld)", (int)result);
         return result;
      }

      //destroy encoder if CAPI
      if(venc_ptr->is_encoder_capi)
      {
         result = venc_encoder_capi_destroy(venc_ptr);
         if(ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Venc failed to destroy enc instance, error %d!", (int)result);
            return result;
         }
      }

      (void) venc_init(venc_ptr);

      venc_ptr->wait_mask = VENC_CMD_DATA_MASK;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Venc Reinit can't be done in RUN session(%lx)",venc_ptr->session_num);
      result = ADSP_EBUSY;
   }

   elite_svc_send_ack(msg_ptr, result);
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_reinit_cmd end result(%d), wait_mask=%lx, session(%lx)",result,venc_ptr->wait_mask,venc_ptr->session_num);
   return result;
}

static ADSPResult venc_start_record_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_custom_voc_svc_connect_record_type* connect_msg_payload_ptr;
   elite_svc_handle_t *svc_2_connect_ptr;
   venc_t* venc_ptr = (venc_t*)instance_ptr;

   connect_msg_payload_ptr = (elite_msg_custom_voc_svc_connect_record_type*) (msg_ptr->pPayload);

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_start_record_cmd opcode(%#lx)session(%#lx)",connect_msg_payload_ptr->sec_opcode, venc_ptr->session_num);

   svc_2_connect_ptr = connect_msg_payload_ptr->svc_handle_ptr;


   // This service only allows one downstream
   if (NULL != venc_ptr->record.afe_handle_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: FAILED, only 1 peer allowed session(%lx)",
            venc_ptr->session_num);
      result = ADSP_EUNSUPPORTED;
   }
   else //else accept the connection
   {
      venc_ptr->record.afe_handle_ptr = svc_2_connect_ptr;
      venc_ptr->record.enable = TRUE;
      venc_ptr->record.first_frame = TRUE;
      venc_ptr->record.ss_info_counter = 0; // reset drift info counter
      venc_ptr->record.ss_multiframe_counter = 0; //reset multiframe counter
      venc_ptr->record.mode = connect_msg_payload_ptr->record_mode;
      venc_ptr->record.aud_ref_port = connect_msg_payload_ptr->aud_ref_port;  // VSM abstracts out the v1 and v2 version of the api
      // vtm subscribe parameters
      venc_ptr->record.afe_drift_ptr  = connect_msg_payload_ptr->afe_drift_ptr; // afe drift pointer is stored only at 1 location, hence not in the if condition of vfr_hard

      venc_init_rec_circbuf(venc_ptr);

      // reset the hptimer vs device drift counter
      memset(&venc_ptr->record.voice_cmn_drift_info, 0, sizeof(venc_ptr->record.voice_cmn_drift_info));
      //update nb_sampling rate factor..
      venc_ptr->record.voice_cmn_drift_info.nb_sampling_rate_factor = venc_ptr->samp_rate_factor;
   }
   elite_svc_send_ack(msg_ptr, result);

   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_start_record_cmd_v2 end result=%x, wait_mask=%lx, session(%lx)",result,venc_ptr->wait_mask,venc_ptr->session_num);
   return result;
}

static ADSPResult venc_stop_record_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_custom_voc_svc_connect_type* connect_msg_payload_ptr;
   elite_svc_handle_t *svc_2_connect_ptr;
   venc_t* venc_ptr = (venc_t*)instance_ptr;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_stop_record_cmd session(%lx)",venc_ptr->session_num);

   connect_msg_payload_ptr = (elite_msg_custom_voc_svc_connect_type*) (msg_ptr->pPayload);
   if (VENC_STOP_RECORD_CMD != connect_msg_payload_ptr->sec_opcode)
   {
      result = ADSP_EBADPARAM;
      elite_svc_send_ack(msg_ptr, result);
      return result;
   }

   svc_2_connect_ptr = NULL;

   // This service only allows one downstream
   venc_ptr->record.afe_handle_ptr = svc_2_connect_ptr;
   venc_ptr->record.enable = FALSE;
   venc_ptr->record.mode = 0;
   voice_circbuf_reset(&(venc_ptr->record.circ_struct_encout));

   venc_ptr->record.afe_drift_ptr = NULL;

   venc_ptr->wait_mask &= ~(VENC_REC_BUF_MASK ) ; // Stop listening to Rec Output buffers and Ticks

   elite_svc_send_ack(msg_ptr, result);

   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_stop_record_cmd end result=%x, wait_mask=%lx, session(%lx)",result,venc_ptr->wait_mask,venc_ptr->session_num);
   return result;
}


static ADSPResult venc_data_handler(void *instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t *venc_ptr = (venc_t*)instance_ptr;       // instance structure
   elite_msg_any_t in_buf_msg;

   // ***************** Pop Input buffer
   memset(&in_buf_msg, 0, sizeof(elite_msg_any_t));

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: venc_data_handler begin  session(%x)",(int)venc_ptr->session_num); //TODO: not needed

   // ***************** Read the input data
   result = qurt_elite_queue_pop_front(venc_ptr->svc_handle.dataQ, (uint64_t*)&in_buf_msg);
   voice_result_check(result,(venc_ptr->session_num));

   // ***************** Check Media Type
   if (NULL != in_buf_msg.pPayload)
   {
      if (ELITE_DATA_MEDIA_TYPE == in_buf_msg.unOpCode)// If input data  payload is valid & if data is a mediatype
      {
         // Check Media Type
         elite_msg_data_media_type_apr_t *media_type_payload_ptr =(elite_msg_data_media_type_apr_t *)in_buf_msg.pPayload;
         if( ELITEMSG_DATA_MEDIA_TYPE_APR == media_type_payload_ptr->unMediaTypeFormat &&
               ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM   == media_type_payload_ptr->unMediaFormatID)
         {

            elite_multi_channel_pcm_fmt_blk_t *media_format_blk_ptr =
               (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(media_type_payload_ptr);

            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Received MEDIA TYPE sampRate(%ld)  session(%x)", \
                  media_format_blk_ptr->sample_rate,(int)venc_ptr->session_num);

            if( media_format_blk_ptr->num_channels      != 1     ||
                  media_format_blk_ptr->is_interleaved != FALSE ||
                  media_format_blk_ptr->bits_per_sample != 16    ||
                  media_format_blk_ptr->is_signed      != TRUE  ||
                  (venc_ptr->sampling_rate != media_format_blk_ptr->sample_rate)
              )
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Received Invalid MT message: (%x)",(int)venc_ptr->session_num);
               venc_ptr->sampling_rate = VOICE_NB_SAMPLING_RATE;  // fall back to 8k
               venc_ptr->io.frame_samples = VOICE_FRAME_SIZE_NB_20MS;
            }

         }
      }

      // ***************** Copy data to local buffer
      // If RUN state and if data payload is valid & if data is pcm samples
      if ((venc_ptr->process_data) &&
            (ELITE_DATA_BUFFER == in_buf_msg.unOpCode))
      {
         // Copy data if any to local buffer
         elite_msg_data_buffer_t* in_data_payload_ptr = (elite_msg_data_buffer_t*) in_buf_msg.pPayload;
         venc_ptr->io.in_buf_size = in_data_payload_ptr->nActualSize;
         venc_ptr->io.ull_timestamp = in_data_payload_ptr->ullTimeStamp - venc_ptr->aggregate_delay;
         venc_ptr->io.vfr_frame_cntr = in_data_payload_ptr->unClientToken;

         //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Copy Data vfr_frame_cntr(%d) session(%x)",venc_ptr->io.vfr_frame_cntr,(int)venc_ptr->session_num);

         memsmove(&(venc_ptr->io.in_buf[0]), VOICE_ROUNDTO8((VOICE_FRAME_SIZE_NB_20MS*VOICE_FB_SAMPLING_RATE/VOICE_NB_SAMPLING_RATE)<<1), &(in_data_payload_ptr->nDataBuf),in_data_payload_ptr->nActualSize);
         //dbg: MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: Copy Data in_buf_size(%d) to local buf  session(%x)",(int)venc_ptr->io.in_buf_size,(int)venc_ptr->session_num);
      }
      //else Dont read any data and ignore all the input buffers

      // ***************** Return buffers
      result = elite_msg_return_payload_buffer ( &in_buf_msg );
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed return buffer session(%x)",(int)venc_ptr->session_num);
      }
      in_buf_msg.pPayload = NULL;
   }

   // ***************** Check sanity and call venc_process
   // check if RUN state
   if (venc_ptr->process_data)
   {
      if((((uint32_t)((venc_ptr->samp_rate_factor)*VOICE_FRAME_SIZE_NB_20MS)<<1) == venc_ptr->io.in_buf_size))
      {

         uint64_t time = qurt_elite_timer_get_time();
         uint64_t cycles = qurt_elite_profile_get_pcycles();

         // Count the delay
         if ((venc_ptr->modules.ctm_tx.resync_delay_cnt) || venc_ptr->tty.state_ptr->m_sync_recover_tx)
         {
            venc_ptr->modules.ctm_tx.resync_delay_cnt++;
         }
         if (venc_ptr->modules.ctm_tx.resync_delay_cnt >= MAX_RESYNC_DELAY)
         {
            venc_ptr->modules.ctm_tx.resync_modem_afe = TRUE;
            venc_ptr->modules.ctm_tx.resync_delay_cnt = 0;
         }

         venc_process(venc_ptr);

         if(venc_ptr->pkt_ready && (0 != venc_ptr->io.out_buf_size))
         {
            venc_ptr->pkt_ready = FALSE;
            result = venc_deliver_pkt(venc_ptr, &(venc_ptr->io.out_buf[0]), venc_ptr->io.out_buf_size);//call deliver packet
            if(ADSP_FAILED(result))
            {
               venc_ptr->pkt_miss_ctr++;
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to send Venc pkt req to VDS, result=%d session(%x)",
                     (int)result,(int)venc_ptr->session_num);
            } // pkt_ctr already counts pkt delivered
            if (gVoiceEncDecLB != 0)
            {
               //move to global shared buffer for encoder to decoder loopback
               memsmove(gaVoiceEncDecLBBuf, sizeof(gaVoiceEncDecLBBuf), &(venc_ptr->io.out_buf[0]),venc_ptr->io.out_buf_size);
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: LB mode on - move enc pkt to global buf, bytes = %d session(%x)", (int)venc_ptr->io.out_buf_size,(int)venc_ptr->session_num);
            }
         }
         time = qurt_elite_timer_get_time() - time;
         cycles = qurt_elite_profile_get_pcycles() - cycles;

         voice_cmn_time_profile_add_data( &venc_ptr->time_profile_struct, (uint32_t) time, cycles);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid data buffer  session(%x)",(int)venc_ptr->session_num);
      }
   }

   return result;
}

static void venc_do_sample_slip_struff(venc_t* venc_ptr, int16_t *input_ptr, int16_t *output_ptr, int16_t slip_stuff_samples)
{
   uint16_t frame_size_10msec = venc_ptr->io.in_buf_size >> 2;   //10 msec data.
   uint16_t offset_10msec = 0, output_samples;
   uint16_t input_samples = frame_size_10msec;   //input samples = 10 msec frame samples

   uint16_t num_10msec_frames = 2;

   // Process 10 msec frames. adjust samples in second 10 msec frames
   // when num_10msec_frame=1, adjust drift in same frame i.e for nb do 79->79(stuff) or 81->81(slip) correction
   // when num_10msec_frame=2, adjust drift in second 10 msec frame i.e for nb do 80->80 and 79->79(stuff) or 81->81(slip) in second 10 msec frame.
   for(uint16_t i=0; i < num_10msec_frames ; i++)
   {
      output_samples = input_samples - (i*slip_stuff_samples);
      offset_10msec = frame_size_10msec*i;

      //dbg: MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Voice SS venc: voice_sample_slip_process-in_frame_size=%d,out_frame_size=%d,i=%d",input_samples,output_samples,i);

      (void)voice_sample_slip_process(&(venc_ptr->record.ss_struct),&output_ptr[offset_10msec],&input_ptr[offset_10msec],input_samples,output_samples);

      if((int8_t)venc_ptr->record.ss_multiframe_counter > 0)
      {
         venc_ptr->record.ss_multiframe_counter -= 1;
      }
   }
}

ADSPResult venc_encoder_capi_process(venc_t* venc_ptr, int16_t* in_ptr, int16_t* out_ptr, uint32_t buf_size)
{
   ADSPResult adsp_result=ADSP_EOK;
   capi_v2_err_t result = CAPI_V2_EOK;

   /*
    * SA: TODO: move all this to venc_create so we dont do this each frame.
    * That means we need to update the data_ptr whenever it is updated like op media fmt, set pp samp rate
    */
   capi_v2_stream_data_t input_data;
   capi_v2_stream_data_t output_data;

   capi_v2_stream_data_t *input[] = { &input_data };
   capi_v2_stream_data_t *output[] = { &output_data };

   capi_v2_buf_t in_buf, out_buf;

   input_data.bufs_num = 1;
   input_data.buf_ptr = &in_buf;

   in_buf.data_ptr = (int8_t*)in_ptr;
   in_buf.actual_data_len = in_buf.max_data_len = buf_size;

   output_data.bufs_num = 1;
   output_data.buf_ptr = &out_buf;

   out_buf.data_ptr = (int8_t*)out_ptr;
   out_buf.actual_data_len = 0;

   //setting it to the allocated buffer size.
   out_buf.max_data_len = VOICE_ROUNDTO8(VOICE_FRAME_SIZE_FB<<1);   //1920 bytes

#if defined(LOG_VOICEPROC_DATA) && defined(SIM)
   {
      //read input from file
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc session(%d).voc_type(0x%lx) io logging. insize(%ld), outsize(%ld)",
            venc_ptr->session_num,venc_ptr->voc_type,buf_size,out_buf.actual_data_len);
      {
         char buffer[100]; // The filename buffer.
         // Put output of ith module in ith file
         snprintf(buffer, sizeof(char) * 100, "venc_session_%x_samp_rate_%d_voc_type_%d_input.raw", venc_ptr->session_num,venc_ptr->sampling_rate,venc_ptr->voc_type);
         static FILE *fp_in = NULL;
         int16_t *pIn = in_ptr;
         if(NULL == fp_in)
         {
            fp_in = fopen(buffer,"rb"); // in append mode
         }
         if(fp_in)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc session. reading (%ld)",buf_size);
            fread(pIn,sizeof(char),buf_size,fp_in);
         }
         else
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc session. File logging failed");
         }
      }
   }
#endif

   if (venc_ptr->encoder_capi.common.module_ptr)
   {
      result =  venc_ptr->encoder_capi.common.module_ptr->vtbl_ptr->process(venc_ptr->encoder_capi.common.module_ptr, input, output);

      if(CAPI_V2_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: venc_process(). process() failed for encoder result(0x%lx) for mediatype(0x%lx)",result,venc_ptr->voc_type);
         venc_ptr->io.out_buf_size = 0;
         return ADSP_EFAILED;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_process(). recieved bad pointer for media_type(0x%lx)", venc_ptr->voc_type);
      return ADSP_EFAILED;
   }

   if(0 == out_buf.actual_data_len || venc_ptr->encoder_capi.out_threshold_in_bytes < out_buf.actual_data_len)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: venc_process(). Invalid outputsize(%ld) for mediatype(0x%lx)",out_buf.actual_data_len,venc_ptr->voc_type);
      venc_ptr->io.out_buf_size = 0;
      return ADSP_EFAILED;
   }

#if defined(LOG_VOICEPROC_DATA) && defined(SIM)
   {
      //write input and output to file
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc session(%d).voc_type(0x%lx) io logging. insize(%ld), outsize(%ld)",
            venc_ptr->session_num,venc_ptr->voc_type,buf_size,out_buf.actual_data_len);
      {
         char buffer[100]; // The filename buffer.
         // Put output of ith module in ith file
         snprintf(buffer, sizeof(char) * 100, "venc_capi_session_%x_samp_rate_%d_voc_type_%d_input.raw", venc_ptr->session_num,venc_ptr->sampling_rate,venc_ptr->voc_type);
         FILE *fp;
         int16_t *pIn = in_ptr;
         fp = fopen(buffer,"ab"); // in append mode
         fwrite(pIn,sizeof(char),buf_size,fp);
         fclose(fp);
      }
      {
         char buffer[100]; // The filename buffer.
         // Put output of ith module in ith file
         snprintf(buffer, sizeof(char) * 100, "venc_capi_session_%x_samp_rate_%d_voc_type_%d_output.raw", venc_ptr->session_num,venc_ptr->sampling_rate,venc_ptr->voc_type);
         FILE *fp;
         int16_t *pIn = out_ptr;
         fp = fopen(buffer,"ab"); // in append mode
         fwrite(pIn,sizeof(char),out_buf.actual_data_len,fp);
         fclose(fp);
      }
   }
#endif

   venc_ptr->io.out_buf_size = out_buf.actual_data_len;
   return adsp_result;
}


static ADSPResult venc_process(venc_t* venc_ptr)
{
   ADSPResult result;
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   int16_t *in_ptr = &(venc_ptr->io.in_buf[0]);   // pointer to local input buffer
   int16_t *out_ptr = &(venc_ptr->io.out_buf[0]); // pointer to local output buffer
   CrossFadeCfgType *mute_cfg_ptr = &(venc_ptr->proc.cross_fade_cfg_struct);
   CrossFadeDataType *mute_data_ptr = &(venc_ptr->proc.cross_fade_data_struct);

   // Call venc process function
   capi_v2_buf_t venc_pp_in_buf = { (int8_t*) in_ptr, venc_ptr->io.in_buf_size, venc_ptr->io.in_buf_size};
   capi_v2_stream_data_t venc_pp_stream_in[1];
   capi_v2_stream_data_t *venc_pp_stream_in_ptr[] =  { &venc_pp_stream_in[0] };
   venc_pp_stream_in[0].bufs_num = 1;
   venc_pp_stream_in[0].buf_ptr = &venc_pp_in_buf;

   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: venc_process begin I(%d) session(%x)",(int)venc_ptr->io.in_buf_size,(int)venc_ptr->session_num);

#if defined(__qdsp6__) && !defined(SIM)
   int16_t enc_in_temp[VOICE_FRAME_SIZE_FB];
   memscpy( enc_in_temp, sizeof(enc_in_temp), in_ptr, venc_ptr->io.in_buf_size);
#endif
   // update venc delivery time stamp for this frame for VFR none case
   if( ( venc_ptr->vtm_sub_unsub_data.vfr_mode == VFR_NONE ) &&
         (venc_ptr->io.vfr_frame_cntr == 1) )
   {
      venc_ptr->enc_tick_timestamp_us = venc_ptr->vtm_sub_unsub_data.avtimer_timestamp_us + venc_ptr->vtm_sub_unsub_data.offset;
      if ( venc_ptr->enc_tick_timestamp_us < avtimer_drv_get_time( venc_ptr->avt_drv ))
      {
	     venc_ptr->enc_tick_timestamp_us += venc_ptr->vtm_sub_unsub_data.vfr_cycle;
      }
   }
   else
   {
      venc_ptr->enc_tick_timestamp_us = 0;
   }

   //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_process begin del avtime stamp %lld", venc_ptr->enc_tick_timestamp_us );

   // Apply Tx stream Limiter
   if(venc_ptr->modules.limiter.module_ptr)
   {
      capi_result = venc_ptr->modules.limiter.module_ptr->vtbl_ptr->process(venc_ptr->modules.limiter.module_ptr, venc_pp_stream_in_ptr, venc_pp_stream_in_ptr);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_process: limiter_process: failed, result(0x%x)", uint32_t(capi_result));
      }
   }

   venc_ptr->pkt_ready = TRUE;

   /* for any TTY call, print debug of flag used for muting */
   if( (venc_ptr->tty.state_ptr->m_etty_mode != VSM_TTY_MODE_OFF)||(venc_ptr->tty.state_ptr->m_oobtty_mode != VSM_TTY_MODE_OFF))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: venc_process RxTTYDet(%d)",(int)(venc_ptr->tty.state_ptr->m_rx_tty_detected ||venc_ptr->tty.state_ptr->m_rx_oobtty_detected));
   }

   /* update comfort noise with the updated state of Rx TTY from common TTY structure */
   if(TRUE == venc_ptr->modules.cng.fwk_extn_list.is_cng_fwk_extn_supported)
   {
      voc_param_cng_rx_tty_detected_t rx_tty;
      rx_tty.is_detected = venc_ptr->tty.state_ptr->m_rx_tty_detected;
      venc_capi_set_param(&venc_ptr->modules.cng, VOC_PARAM_CNG_RX_TTY_DETECTED, &rx_tty,
                          sizeof(voc_param_cng_rx_tty_detected_t),
                          sizeof(voc_param_cng_rx_tty_detected_t));
   }

   // Comfort Noise wrapper to handle generation of zeroes or CNG based on Mute/CNGLVB settings
   // Create a copy for Crossfading
   memscpy(venc_ptr->proc.mute_inp_ptr, VOICE_FRAME_SIZE_FB << 1, in_ptr, venc_ptr->io.in_buf_size);
   if(venc_ptr->modules.cng.module_ptr)
   {
      capi_result = venc_ptr->modules.cng.module_ptr->vtbl_ptr->process(venc_ptr->modules.cng.module_ptr,
                                                                   venc_pp_stream_in_ptr,
                                                                   venc_pp_stream_in_ptr);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_process: cng_process: failed, result(0x%x)", uint32_t(capi_result));
      }
   }
   else if(VOICE_MUTE == venc_ptr->mute)  // if CNG is stubbed then handle mute here.
   {
      memset(venc_pp_stream_in_ptr[0]->buf_ptr[0].data_ptr, 0, venc_pp_stream_in_ptr[0]->buf_ptr[0].actual_data_len);
   }

   //handling soft mute
   if ( VOICE_MUTE == venc_ptr->mute )
   {
      if(TRUE == venc_ptr->modules.cng.fwk_extn_list.is_cng_fwk_extn_supported)
      {
         // If in Mute state, send setting to Comfort Noise wrapper, so that
         // zeroes or CNG can be generated from next frame onwards.
         voc_param_cng_tx_mute_t tx_mute;
         tx_mute.is_mute = VOICE_MUTE;
         venc_capi_set_param(&venc_ptr->modules.cng, VOC_PARAM_CNG_TX_MUTE, &tx_mute,
                             sizeof(voc_param_cng_tx_mute_t), sizeof(voc_param_cng_tx_mute_t));
      }
      cross_fade_process(mute_cfg_ptr, mute_data_ptr, venc_ptr->proc.mute_inp_ptr, in_ptr, in_ptr,
                         (venc_ptr->io.in_buf_size) / 2);
   }
   else  // Unmute
   {
      // If in Unmute state, send setting to Comfort Noise wrapper after reaching steady state (of crossfade algorithm)
      // so that zeroes or CNG can be mixed until completely unmuted.
      if(STEADY_STATE == mute_data_ptr->state)
      {
         if(TRUE == venc_ptr->modules.cng.fwk_extn_list.is_cng_fwk_extn_supported)
         {
            voc_param_cng_tx_mute_t tx_mute;
            tx_mute.is_mute = VOICE_UNMUTE;
            venc_capi_set_param(&venc_ptr->modules.cng, VOC_PARAM_CNG_TX_MUTE, &tx_mute,
                                sizeof(voc_param_cng_tx_mute_t), sizeof(voc_param_cng_tx_mute_t));
         }
      }
      cross_fade_process(mute_cfg_ptr, mute_data_ptr, in_ptr, venc_ptr->proc.mute_inp_ptr, in_ptr,
                         (venc_ptr->io.in_buf_size) / 2);
   }

   // half duplex muting forced after crossfading/CNG
   if( (TRUE == venc_ptr->tty.state_ptr->m_rx_tty_detected &&
            VSM_TTY_MODE_FULL == venc_ptr->tty.state_ptr->m_etty_mode)||
         ( TRUE == venc_ptr->tty.state_ptr->m_rx_oobtty_detected &&
           VSM_TTY_MODE_FULL == venc_ptr->tty.state_ptr->m_oobtty_mode))
   {
      memset( in_ptr, 0, venc_ptr->io.in_buf_size);
   }

   /* DTMF Gen*/
   if(venc_ptr->modules.dtmf_gen.module_ptr)
   {
      capi_result = venc_ptr->modules.dtmf_gen.module_ptr->vtbl_ptr->process(venc_ptr->modules.dtmf_gen.module_ptr, venc_pp_stream_in_ptr, venc_pp_stream_in_ptr);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_process: dtmf_gen process: failed, result(0x%x)", uint32_t(capi_result));
      }
   }
   // ctm tx process handling
   if ((TRUE == venc_ptr->modules.ctm_tx.resync_modem_afe))
   {
      if(TRUE == venc_ptr->modules.ctm_tx.ctm_tx_capi.intf_extn_list.is_tty_intf_supported)
      {
         result = venc_capi_set_param(&venc_ptr->modules.ctm_tx.ctm_tx_capi, TTY_PARAM_CTM_RESYNC, NULL, 0, 0);
         if(ADSP_EOK == result)
         {
            venc_ptr->tty.state_ptr->m_sync_recover_tx = FALSE;
            venc_ptr->modules.ctm_tx.resync_modem_afe = FALSE;
         }
      }
   }
   if(TRUE == venc_ptr->modules.ctm_tx.ctm_tx_capi.intf_extn_list.is_tty_intf_supported)
   {
      if (venc_ptr->tty.state_ptr->m_ctm_from_far_end_detected)
      {
         // No error handling here. it is already taken care in venc_capi_set_param() function
         result = venc_capi_set_param(&venc_ptr->modules.ctm_tx.ctm_tx_capi, TTY_PARAM_CTM_RX_DETECTED, NULL, 0, 0);
      }
      if(venc_ptr->tty.state_ptr->m_enquiry_from_far_end_detected)
      {
         result = venc_capi_set_param(&venc_ptr->modules.ctm_tx.ctm_tx_capi, TTY_PARAM_CTM_RX_ENQUIRY_DETECTED, NULL, 0, 0);
         if(ADSP_EOK == result)
         {
            venc_ptr->tty.state_ptr->m_enquiry_from_far_end_detected = FALSE;
         }
      }
   }

   if(venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr)
   {
      capi_result = venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr->vtbl_ptr->process(venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr, venc_pp_stream_in_ptr, venc_pp_stream_in_ptr);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_process: ctm_tx_process: failed, result(0x%x)", uint32_t(capi_result));
      }
   }


   if(venc_ptr->modules.ltetty_tx.module_ptr)
   {
      capi_result = venc_ptr->modules.ltetty_tx.module_ptr->vtbl_ptr->process(venc_ptr->modules.ltetty_tx.module_ptr, venc_pp_stream_in_ptr, venc_pp_stream_in_ptr);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_process: ltetty_tx_process: failed, result(0x%x)", uint32_t(capi_result));
      }
   }
   /* Host PCM processing */
   /* This point is chosen so that e-call data will be captured in the log and in-call rec */
   /* e-call data from client -> qxdm log -> TTY detection -> in-call rec -> encoder       */
   voice_ecall_tx (venc_ptr, in_ptr);


#if defined(__qdsp6__) && !defined(SIM)
   // log saved input buf and preproc output prior to encode
   int8_t *bufptr[4] = { (int8_t *) enc_in_temp, (int8_t *) in_ptr, NULL, NULL };
   uint32_t sampling_rate_log_id;
   sampling_rate_log_id = voice_get_sampling_rate_log_id(venc_ptr->sampling_rate);
   voice_log_buffer( bufptr,
         VOICE_LOG_TAP_POINT_VENC_IN,
         (sampling_rate_log_id << 3) | venc_ptr->session_num,
         qurt_elite_timer_get_time(),
         VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV,
         venc_ptr->sampling_rate,
         venc_ptr->io.in_buf_size,
         NULL);
#endif

   if (TRUE == venc_ptr->record.enable )
   {
      int16_t slip_stuff_samples = 0;
      int16_t local_ss_buf[966];            // local input buffer, todo: remove hardcoding

      // calculate av/hp timer drift
      venc_cal_drift(venc_ptr);
      // Calculate how many samples to slip/stuff in this frame
      venc_cal_sample_slip_stuff(venc_ptr, &(slip_stuff_samples));
      // apply sample slipping on the encoder input to match rate with record afe port
      venc_do_sample_slip_struff(venc_ptr,in_ptr,&local_ss_buf[0],slip_stuff_samples);
      // store in the circular buffer to be sent out
      voice_circbuf_write(&venc_ptr->record.circ_struct_encout,(int8_t *)&local_ss_buf[0],(venc_ptr->io.in_buf_size/2 - slip_stuff_samples));
   }

   // vocoder encoder processing
   result = venc_encoder_capi_process(venc_ptr, in_ptr, out_ptr, venc_ptr->io.in_buf_size);
   if(ADSP_EOK != result)
   {
      return result;
   }

   /* log the MVS payload if not a PCM/G711 media type */
   if( venc_ptr->voc_type != VSM_MEDIA_TYPE_PCM_8_KHZ &&
         venc_ptr->voc_type != VSM_MEDIA_TYPE_PCM_16_KHZ &&
         venc_ptr->voc_type != VSM_MEDIA_TYPE_PCM_32_KHZ &&
         venc_ptr->voc_type != VSM_MEDIA_TYPE_PCM_48_KHZ &&
         venc_ptr->voc_type != VSM_MEDIA_TYPE_G711_ALAW &&
         venc_ptr->voc_type != VSM_MEDIA_TYPE_G711_MLAW)
   {
      venc_log_vocoder_packet( venc_ptr->voc_type,
            ((venc_ptr->sampling_rate== 16000) << 3) | venc_ptr->session_num,
            out_ptr, venc_ptr->io.out_buf_size, (venc_ptr->sampling_rate));
   }

   if (TRUE == venc_ptr->record.enable )
   {
      venc_ptr->wait_mask |= VENC_REC_BUF_MASK;
   }
   return ADSP_EOK;
}


static ADSPResult venc_apr_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = APR_EOK;
   venc_t* venc_ptr = (venc_t*) instance_ptr;
   elite_apr_packet_t * apr_packet_ptr = (elite_apr_packet_t*) msg_ptr->pPayload;
   int32_t rc = ADSP_EOK;

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_apr_cmd begin, opCode(%lx) token(%lx) session(%x)",
         elite_apr_if_get_opcode(apr_packet_ptr),elite_apr_if_get_client_token(apr_packet_ptr),(int)venc_ptr->session_num);

   /* parse out the received pakcet.  Note in this current framework we are not prioritizing commands
      that can be completed immediately.  We are simply processing command in order they are received */
   switch(elite_apr_if_get_opcode(apr_packet_ptr))
   {
      case VSM_EVT_OOB_ENC_BUF_CONSUMED:
      {
         if ( venc_ptr->oob.enc_pkt_consumed == TRUE)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP:  UNEXPECTED EVENT as VENC did not deliver the previous pkt  for session (%d)",
                  (int)venc_ptr->session_num);
         }
         // set flag to indicate OOB pkt is consumed so as to send next pkt
         venc_ptr->oob.enc_pkt_consumed= TRUE;
         if(venc_ptr->pkt_delivery_pending == TRUE)
         { //Since Packet was ready before VSM_EVT_OOB_ENC_BUF_CONSUMED event, queue the packet to VDS now.
            //venc_ptr->pkt_delivery_pending is set to FALSE inside venc_deliver_pkt
            result = venc_deliver_pkt(venc_ptr, &(venc_ptr->io.out_buf[0]), venc_ptr->io.out_buf_size);
            if(ADSP_FAILED(result))
            {
               venc_ptr->pkt_miss_ctr++;
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to send Venc pkt to VDS, result=%d session(%x)",
                     (int)result,(int)venc_ptr->session_num);
            } // pkt_ctr already counts pkt delivered
         }
         venc_ptr->oob.pkt_consumed_ctr++;
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VENC: VSM_EVT_OOB_ENC_BUF_CONSUMED number (%d) for session (%d)",
               (int)venc_ptr->oob.pkt_consumed_ctr,(int)venc_ptr->session_num);
         // free the event since not done internally
         rc = elite_apr_if_free(venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr);
         break;
      }
      case VSM_CMD_ENC_SET_MINMAX_RATE:
      {
         vsm_enc_set_minmax_rate_t *min_max_cmd_ptr = (vsm_enc_set_minmax_rate_t *) elite_apr_if_get_payload_ptr( apr_packet_ptr);
         MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VSM_CMD_ENC_SET_MINMAX_RATE, mediaType(%lx), \
                  min_rate(%lx), max_rate(%lx), process_data(%x) session(%x)",min_max_cmd_ptr->media_type, \
                  min_max_cmd_ptr->min_rate, min_max_cmd_ptr->max_rate, venc_ptr->process_data,(int)venc_ptr->session_num);

         if(venc_ptr->is_encoder_capi && venc_ptr->encoder_capi.common.intf_extn_list.is_voc_intf_supported)
         {
            voc_param_enc_minmax_rate_t minmax;
            minmax.min_rate = min_max_cmd_ptr->min_rate;
            minmax.max_rate = min_max_cmd_ptr->max_rate;
            result = venc_capi_set_param(&venc_ptr->encoder_capi.common, VOC_PARAM_ENC_MINMAX_RATE, &minmax, sizeof(minmax), sizeof(minmax));
         }
         else
         {
            //check min_rate/max_rate within valid range or not (1 to 4 )
            if (((min_max_cmd_ptr->min_rate <= FULL_RATE) && (min_max_cmd_ptr->min_rate >= SILENCE_RATE )) &&
                  ((min_max_cmd_ptr->max_rate <= FULL_RATE) && (min_max_cmd_ptr->max_rate >= SILENCE_RATE )))
            {
            }
            else
            {
               MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Invalid Min or Max rates  session(%x)",(int)venc_ptr->session_num);
               result = ADSP_EUNSUPPORTED;
            }
         }
         rc = elite_apr_if_end_cmd(venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr, result );
         break;
      }
      case VSM_CMD_ENC_SET_RATE_MOD:
      {
         vsm_enc_set_rate_mod_t *enc_rate_mod_cmd_ptr = (vsm_enc_set_rate_mod_t *) elite_apr_if_get_payload_ptr( apr_packet_ptr);
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VSM_CMD_ENC_SET_RATE_MOD, enc_rate_mod(%lx), \
                  process_data(%x) session(%x)",enc_rate_mod_cmd_ptr->rate_modulation_param, \
                  venc_ptr->process_data,(int)venc_ptr->session_num);
         if(venc_ptr->is_encoder_capi && venc_ptr->encoder_capi.common.intf_extn_list.is_voc_intf_supported)
         {
            voc_param_enc_rate_mod_t ratemod;
            ratemod.rate_modulation_param = enc_rate_mod_cmd_ptr->rate_modulation_param;
            result = venc_capi_set_param(&venc_ptr->encoder_capi.common, VOC_PARAM_ENC_RATE_MOD, &ratemod, sizeof(ratemod), sizeof(ratemod));
         }
         rc = elite_apr_if_end_cmd(venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr, result );
         break;
      }
      case VSM_CMD_ENC_SET_DTX_MODE:
      {
         vsm_enc_set_dtx_mode_t *dtx_mode_cmd_ptr = (vsm_enc_set_dtx_mode_t*) elite_apr_if_get_payload_ptr( apr_packet_ptr );
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VSM_CMD_ENC_SET_DTX_MODE, dtx_mode(%lx), \
                  process_data(%x) session(%x)", dtx_mode_cmd_ptr->dtx_mode, \
                  venc_ptr->process_data,(int)venc_ptr->session_num);
         if(venc_ptr->is_encoder_capi && venc_ptr->encoder_capi.common.intf_extn_list.is_voc_intf_supported)
         {
            voc_param_enc_dtx_t dtx;
            dtx.dtx_mode = dtx_mode_cmd_ptr->dtx_mode;
            result = venc_capi_set_param(&venc_ptr->encoder_capi.common, VOC_PARAM_ENC_DTX, &dtx, sizeof(dtx), sizeof(dtx));
         }
         else
         {
            if(dtx_mode_cmd_ptr->dtx_mode > 1)
            {
               MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Venc received invalid DTX mode %ld, process_data(%x) session(%lx)",
                     dtx_mode_cmd_ptr->dtx_mode, venc_ptr->process_data, venc_ptr->session_num);
               rc = elite_apr_if_end_cmd(venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr, ADSP_EBADPARAM );
               break;
            }
            venc_ptr->encoder.dtx_mode = dtx_mode_cmd_ptr->dtx_mode;
         }
         rc = elite_apr_if_end_cmd(venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr, ADSP_EOK );
         break;
      }
      case VSM_CMD_SET_DTMF_GENERATION:
      {
         /* check if DTMF generation is set in correct direction */
         vsm_set_dtmf_generation_t *dtmf_gen_ptr = (vsm_set_dtmf_generation_t *)elite_apr_if_get_payload_ptr( apr_packet_ptr );

         // save self/client info for DTMF Gen ended event.  Client is source address from incoming packet
         venc_ptr->modules.apr_info_dtmf_gen.self_addr = elite_apr_if_get_dst_addr( apr_packet_ptr);
         venc_ptr->modules.apr_info_dtmf_gen.self_port = elite_apr_if_get_dst_port( apr_packet_ptr);
         venc_ptr->modules.apr_info_dtmf_gen.client_addr = elite_apr_if_get_src_addr( apr_packet_ptr);
         venc_ptr->modules.apr_info_dtmf_gen.client_port = elite_apr_if_get_src_port( apr_packet_ptr);
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VSM_CMD_SET_DTMF_GENERATION, saving clientAddr(%x) clientPort(%x) :session(%lx)",
               venc_ptr->modules.apr_info_dtmf_gen.client_addr, venc_ptr->modules.apr_info_dtmf_gen.client_port,venc_ptr->session_num);

         result = venc_capi_set_param(&venc_ptr->modules.dtmf_gen, FWK_EXTN_PARAM_ID_DTMF_GEN_SET_DTMF_GENERATION, dtmf_gen_ptr, sizeof(vsm_set_dtmf_generation_t), sizeof(vsm_set_dtmf_generation_t));

         if(ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VSM_CMD_SET_DTMF_GENERATION: Set param failed = %lx, session(%x)", result, (int)venc_ptr->session_num);
            venc_ptr->modules.apr_info_dtmf_gen.self_addr = 0;
            venc_ptr->modules.apr_info_dtmf_gen.self_port = 0;
            venc_ptr->modules.apr_info_dtmf_gen.client_addr = 0;
            venc_ptr->modules.apr_info_dtmf_gen.client_port = 0;
            rc = elite_apr_if_end_cmd( venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr, APR_EBADPARAM);
         }
         else
         {
            rc = elite_apr_if_end_cmd(venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr, APR_EOK );
         }
         break;
      }
      case VOICE_EVT_PUSH_HOST_BUF_V2:
      {
         ADSPResult local_result = APR_EOK;

         voice_evt_push_host_pcm_buf_v2_t *push_host_buf_ptr = (voice_evt_push_host_pcm_buf_v2_t *) elite_apr_if_get_payload_ptr(apr_packet_ptr);

         // send PUSH_HOST_BUF event to the HPCM CAPI module
         if(venc_ptr->modules.special_capis[HPCM]->module_ptr)
         {
            capi_v2_buf_t param_data_buf;
            capi_v2_port_info_t port_info;
            port_info.is_valid = FALSE;
            param_data_buf.data_ptr = (int8_t *) push_host_buf_ptr;
            param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(fwk_extn_param_id_hpcm_push_host_buf_t);
            local_result = venc_ptr->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(venc_ptr->modules.special_capis[HPCM]->module_ptr,
                  FWK_EXTN_PARAM_ID_HPCM_PUSH_HOST_BUF, &port_info, &param_data_buf);
            local_result = capi_v2_err_to_adsp_result(local_result);
         }

         if (ADSP_FAILED(local_result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! venc_apr_cmd() - FWK_EXTN_PARAM_ID_HPCM_PUSH_HOST_BUF set failed, result(%#x) session(%#lx)", \
                  local_result,venc_ptr->session_num);
         }

         // free the event since not done internally
         rc = elite_apr_if_free(venc_ptr->modules.apr_info_hpcm.apr_handle, apr_packet_ptr);
         break;
      }
      case VSM_CMD_SET_EVS_ENC_OPERATING_MODE:
      {
         vsm_set_evs_enc_operating_mode_t *evs_enc_operating_mode_cmd_ptr = (vsm_set_evs_enc_operating_mode_t *) elite_apr_if_get_payload_ptr( apr_packet_ptr);
         if(venc_ptr->is_encoder_capi && venc_ptr->encoder_capi.common.intf_extn_list.is_voc_intf_supported)
         {
            voc_param_enc_mode_t enc_mode;

            enc_mode.rate = evs_enc_operating_mode_cmd_ptr->mode;
            enc_mode.bandwidth = evs_enc_operating_mode_cmd_ptr->bandwidth;
            result = venc_capi_set_param(&venc_ptr->encoder_capi.common, VOC_PARAM_ENC_MODE, &enc_mode, sizeof(enc_mode), sizeof(enc_mode));
         }
         else
         {
            result = ADSP_EUNSUPPORTED;
         }

         rc = elite_apr_if_end_cmd(venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr, result );
         break;
      }
      case VSM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE:
      {
         vsm_set_evs_enc_channel_aware_mode_enable_t *evs_enc_hannel_aware_mode_enable_cmd_ptr = (vsm_set_evs_enc_channel_aware_mode_enable_t *) elite_apr_if_get_payload_ptr( apr_packet_ptr);
         if(venc_ptr->is_encoder_capi && venc_ptr->encoder_capi.common.intf_extn_list.is_voc_intf_supported)
         {
            voc_param_enc_channel_aware_mode_t ca_mode;

            ca_mode.mode = TRUE;
            ca_mode.fec_offset = evs_enc_hannel_aware_mode_enable_cmd_ptr->fec_offset;
            ca_mode.fer_rate = evs_enc_hannel_aware_mode_enable_cmd_ptr->fer_rate;
            result = venc_capi_set_param(&venc_ptr->encoder_capi.common, VOC_PARAM_ENC_CHANNEL_AWARE_MODE, &ca_mode, sizeof(ca_mode), sizeof(ca_mode));
         }
         else
         {
            result = ADSP_EUNSUPPORTED;
         }
         rc = elite_apr_if_end_cmd(venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr, result );
         break;
      }
      case VSM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_DISABLE:
      {
         if(venc_ptr->is_encoder_capi && venc_ptr->encoder_capi.common.intf_extn_list.is_voc_intf_supported)
         {
            voc_param_enc_channel_aware_mode_t ca_mode;

            ca_mode.mode = FALSE;
            //initialize these with Zero.
            ca_mode.fec_offset = 0;
            ca_mode.fer_rate = 0;
            result = venc_capi_set_param(&venc_ptr->encoder_capi.common, VOC_PARAM_ENC_CHANNEL_AWARE_MODE, &ca_mode, sizeof(ca_mode), sizeof(ca_mode));
         }
         else
         {
            result = ADSP_EUNSUPPORTED;
         }
         rc = elite_apr_if_end_cmd(venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr, result );
         break;
      }
      default:
      {
         /* Handle error. */
         if (elite_apr_if_msg_type_is_cmd(apr_packet_ptr))
         {  /* Complete unsupported commands. */
            rc = elite_apr_if_end_cmd(venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr, APR_EUNSUPPORTED);
         }
         else
         {  /* Drop unsupported events. */
            rc = elite_apr_if_free(venc_ptr->apr_info_ptr->apr_handle, apr_packet_ptr);
         }
         result = ADSP_EUNSUPPORTED;
         break;
      }
   }

   if (ADSP_FAILED(rc))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! Elite APR message handler has returned error %d, continuing...", (int)rc);
   }
   MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: venc_apr_cmd end, result=%d session(%x)", \
         result,(int)venc_ptr->session_num);
   return result;
}

static ADSPResult venc_deliver_pkt(venc_t *venc_ptr, void *buffer, uint32_t buf_size)
{
   int32_t rc = 0;
   uint8_t* content;
   elite_apr_packet_t* packet=NULL;
   vsm_send_enc_packet_v2_t* enc_buffer;
   ADSPResult result=ADSP_EOK;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: venc_deliver_pkt begin session(%x)",(int)venc_ptr->session_num);

   if (IN_BAND == venc_ptr->pkt_exchange_mode)
   {
      rc = elite_apr_if_alloc_event( venc_ptr->apr_info_ptr->apr_handle,
            venc_ptr->apr_info_ptr->self_addr,
            venc_ptr->apr_info_ptr->self_port,
            venc_ptr->apr_info_ptr->self_addr,
            venc_ptr->apr_info_ptr->self_port,
            venc_ptr->pkt_ctr++,
            VSM_EVT_SEND_ENC_PACKET,
            ( sizeof( vsm_send_enc_packet_v2_t ) + buf_size ),
            &packet );

      if(rc || (NULL == packet))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to create APR packet to deliver Venc pkt 0x%8x session(%lx)",(int)rc,
               venc_ptr->session_num);
         return ADSP_EFAILED;
      }

      enc_buffer = (vsm_send_enc_packet_v2_t *) elite_apr_if_get_payload_ptr( packet );
      enc_buffer->media_type = venc_ptr->voc_type;
      enc_buffer->timestamp_us = venc_ptr->io.ull_timestamp;
      content = APR_PTR_END_OF( enc_buffer, sizeof( vsm_send_enc_packet_v2_t ) );
      memscpy( content,buf_size, buffer, buf_size );
      vds_deliver_apr_packet_t delivery_param;
      delivery_param.client_id = venc_ptr->vds_client_id;
      delivery_param.client_token = venc_ptr->vds_client_token;
      delivery_param.apr_handle_ptr = &venc_ptr->apr_info_ptr->apr_handle;
      delivery_param.apr_packet_ptr = packet;
      delivery_param.delivery_timestamp = venc_ptr->enc_tick_timestamp_us;
      result = voice_cmn_send_vds_apr_request(venc_ptr->vds_handle_ptr, &delivery_param, venc_ptr->session_num);
      if(ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to send APR packet to deliver Venc pkt to VDS result(0x%x) session(%x)",
               (unsigned int)result,(int)venc_ptr->session_num);
         elite_apr_if_free( venc_ptr->apr_info_ptr->apr_handle, packet );
         return result;
      }
   }
   else if (venc_ptr->oob.pkt_exchange_ptr == NULL)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ERROR!OOB packet exchange pointer to shared memory for ENCODER is NULL, session(%x)",
            (int)venc_ptr->session_num);
      rc = ADSP_EFAILED;
   }
   else if (venc_ptr->oob.enc_pkt_consumed == TRUE)
   {
      vsm_oob_pkt_exchange_header_t* enc_buffer_header;

      enc_buffer_header= (vsm_oob_pkt_exchange_header_t *) venc_ptr->oob.pkt_exchange_ptr;
      enc_buffer_header->media_type = venc_ptr->voc_type;
      enc_buffer_header->size = buf_size;
      enc_buffer_header->time_stamp=(uint32_t)qurt_elite_timer_get_time_in_msec();
      content = APR_PTR_END_OF( enc_buffer_header, sizeof( vsm_oob_pkt_exchange_header_t ) );
      memscpy( content, (venc_ptr->oob.memmap.unMemSize)*sizeof(int32_t) - sizeof( vsm_oob_pkt_exchange_header_t ), buffer, buf_size );
      result = elite_mem_flush_cache(&venc_ptr->oob.memmap);        // flushing cache
      if( ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: cache flush failed, result=%x session(%x)",(int)result,(int)venc_ptr->session_num);
      }

      rc = elite_apr_if_alloc_event( venc_ptr->apr_info_ptr->apr_handle,
            venc_ptr->apr_info_ptr->self_addr,
            venc_ptr->apr_info_ptr->self_port,
            venc_ptr->apr_info_ptr->self_addr,
            venc_ptr->apr_info_ptr->self_port,
            venc_ptr->pkt_ctr++,
            VSM_EVT_OOB_ENC_BUF_READY,
            0,
            &packet );

      if(rc || (NULL == packet))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to create APR packet to deliver Venc pkt 0x%8x session(%lx)",(int)rc,
               venc_ptr->session_num);
         return ADSP_EFAILED;
      }
      vds_deliver_apr_packet_t delivery_param;
      delivery_param.client_id = venc_ptr->vds_client_id;
      delivery_param.client_token = venc_ptr->vds_client_token;
      delivery_param.apr_handle_ptr = &venc_ptr->apr_info_ptr->apr_handle;
      delivery_param.apr_packet_ptr = packet;
      delivery_param.delivery_timestamp = venc_ptr->enc_tick_timestamp_us;
      result = voice_cmn_send_vds_apr_request(venc_ptr->vds_handle_ptr, &delivery_param, venc_ptr->session_num);
      if(ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to queue Venc pkt delivery request to VDS result 0x%x session(%x)",
               (unsigned int)result,(int)venc_ptr->session_num);
         elite_apr_if_free( venc_ptr->apr_info_ptr->apr_handle, packet );
         return result;
      }

      venc_ptr->oob.enc_pkt_consumed = FALSE;
      venc_ptr->pkt_delivery_pending = FALSE;

   }
   else
   {
      // Start listening to errors from VDS. If VDS reports an error it implies that
      // client took way too long. If that is the case, instead of holding on to the
      // stale packet  be sent, drop it and prepare to send the next current packet.
      // As a side effect, packets will continue to be dropped if client continues to
      // delay its processing (and thereby its CONSUMED event to the DSP).

      venc_ptr->pkt_delivery_pending = TRUE;
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Previous Encoder Pkt not yet consumed by HOST :session(%x)",(int)venc_ptr->session_num);
   }

   /* If we are in a loopback with valid loopback handle, generate a new APR packet,
    * then queue the apr pointer to the peer (rx decoder) gpQ for servicing.  If operation fails, free the APR packet here.
    */

   if( TRUE == venc_ptr->loopback_enable && NULL != venc_ptr->loopback_handle)
   {

      ADSPResult       result;
      elite_msg_any_t  msg;
      vsm_send_enc_packet_t* enc_buffer_lb;

      rc = elite_apr_if_alloc_event( venc_ptr->apr_info_ptr->apr_handle,
            venc_ptr->apr_info_ptr->self_addr,
            venc_ptr->apr_info_ptr->self_port,
            venc_ptr->apr_info_ptr->client_addr,
            venc_ptr->apr_info_ptr->client_port,
            venc_ptr->pkt_ctr++,
            VSM_EVT_SEND_ENC_PACKET,
            ( sizeof( vsm_send_enc_packet_t ) + buf_size ),
            &packet );

      if ( rc || NULL == packet)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to create APR packet to deliver Venc pkt 0x%8x session(%lx)",(int)rc,
               venc_ptr->session_num);
         return ADSP_EFAILED;
      }

      enc_buffer_lb = (vsm_send_enc_packet_t *) elite_apr_if_get_payload_ptr( packet );
      enc_buffer_lb->media_type = venc_ptr->voc_type;
      content = APR_PTR_END_OF( enc_buffer_lb, sizeof( vsm_send_enc_packet_t ) );
      memscpy( content, buf_size, buffer, buf_size );


      msg.unOpCode = ELITE_APR_PACKET;
      msg.pPayload = (void *) packet;

      result = qurt_elite_queue_push_back(venc_ptr->loopback_handle->gpQ,(uint64_t*)&msg);

      if( ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_deliver_pkt to Dec gpQ failed, result=%x session(%x)",(int)rc,(int)venc_ptr->session_num);
         rc = elite_apr_if_free( venc_ptr->apr_info_ptr->apr_handle, packet );
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: venc_deliver_pkt to Dec gpQ rc(%x) session(%x)",(int)rc,(int)venc_ptr->session_num);
      }
   }

   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: venc_deliver_pkt end, result=%x session(%x)",(int)rc,(int)venc_ptr->session_num);
   return rc;
}

static void voice_result_check(ADSPResult result, uint32_t session_num)
{
   if (ADSP_EOK != result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Venc operation error result %d, session(%lx)",(int)result,session_num);
   }
}

static ADSPResult venc_init(venc_t *venc_ptr)
{
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_init begin session(%x)",(int)venc_ptr->session_num);
   // Reset encoder config params
   (void) venc_enc_params_init(venc_ptr);
   // Reset control code params
   (void) venc_ctrl_params_init(venc_ptr);
   // Reset params for a new call
   venc_ptr->vtm_sub_unsub_data.offset = 8000;
   venc_ptr->vtm_sub_unsub_data.signal_enable = 1;
   venc_ptr->vtm_sub_unsub_data.client_id = VOICE_ENC;
   venc_ptr->vtm_sub_unsub_data.vfr_mode = VFR_NONE;
   venc_ptr->vtm_sub_unsub_data.vsid = 0;
   venc_ptr->vtm_sub_unsub_data.timing_ver = VFR_CLIENT_INFO_VER_1;
   venc_ptr->pkt_exchange_mode = IN_BAND;
   venc_ptr->oob.pkt_exchange_ptr = NULL;
   venc_ptr->oob.enc_pkt_consumed = FALSE;
   venc_ptr->pkt_delivery_pending = FALSE;
   venc_ptr->pkt_ctr = 0;
   venc_ptr->pkt_miss_ctr = 0;
   venc_ptr->oob.pkt_consumed_ctr = 0;
   venc_ptr->oob.pkt_miss_ctr = 0;
   venc_ptr->voc_type = VSM_MEDIA_TYPE_NONE;
   venc_ptr->vfr_mode = VFR_NONE;
   venc_ptr->mute    = VOICE_UNMUTE;  // don't cache mute flag for a fresh call
   venc_ptr->loopback_enable = FALSE;  //reset on init/reinit
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_init end session(%x)",(int)venc_ptr->session_num);
   venc_ptr->proc.cross_fade_cfg_struct.iTotalPeriodMsec = 0;
   venc_ptr->proc.cross_fade_cfg_struct.iConvergeNumSamples = 0;
   venc_ptr->sampling_rate = VOICE_NB_SAMPLING_RATE;
   venc_ptr->io.frame_samples = VOICE_FRAME_SIZE_NB_20MS;
   venc_ptr->samp_rate_factor = 1;
   venc_ptr->vsid = 0;
   venc_ptr->vocoder_op_mode = VSM_VOC_OPERATING_MODE_NONE;

   return result;
}

//KA: these should be encapsulated within each CAPI. This function can be removed once that's done.
// Reset encoder config params, should be called for a new call and HO to another vocoder
static ADSPResult venc_enc_params_init(venc_t *venc_ptr)
{
   ADSPResult result = ADSP_EOK;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_enc_params_init begin session(%x)",(int)venc_ptr->session_num);
   if(FALSE == venc_ptr->is_encoder_capi)
   {
      venc_ptr->encoder.dtx_mode = 0;
   }

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_enc_params_init end session(%x)",(int)venc_ptr->session_num);
   return result;
}

// Reset control code params, should be called for a new call and STOP state
static ADSPResult venc_ctrl_params_init(venc_t *venc_ptr)
{
   ADSPResult result = ADSP_EOK;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_ctrl_params_init begin session(%x)",(int)venc_ptr->session_num);
   venc_ptr->io.in_buf_size = 0;
   venc_ptr->io.out_buf_size = 0;
   venc_ptr->pkt_ready = 0;
   venc_ptr->modules.ctm_tx.resync_delay_cnt = 0;
   venc_ptr->modules.ctm_tx.resync_afe = FALSE;
   venc_ptr->send_media_type = FALSE;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_ctrl_params_init end session(%x)",(int)venc_ptr->session_num);
   return result;
}

static ADSPResult venc_send_rec_buf_to_afe(venc_t* venc_ptr,qurt_elite_bufmgr_node_t *out_buf_mgr_node_ptr)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_any_t* peer_data_Q_msg_ptr;
   uint8_t samp_rate_factor = venc_ptr->samp_rate_factor;
   int32_t sample_rate_ms_samples = samp_rate_factor * 8;
   int32_t output_buf_size = voice_cmn_int_div(venc_ptr->record.circ_struct_encout.unread_samples, sample_rate_ms_samples); // finding the number of 1ms samples

   output_buf_size = (output_buf_size * sample_rate_ms_samples) << 1; // covert to num samples, then to bytes

   elite_msg_data_buffer_t* out_data_payload_ptr = (elite_msg_data_buffer_t*) out_buf_mgr_node_ptr->pBuffer;
   out_data_payload_ptr->pResponseQ           = NULL;
   out_data_payload_ptr->unClientToken        = NULL;
   out_data_payload_ptr->pBufferReturnQ       = (venc_ptr->record.buf_q_ptr);
   out_data_payload_ptr->nOffset              = 0;
   out_data_payload_ptr->nActualSize          = output_buf_size;

   if(VSM_RECORD_TX_RX_STEREO == venc_ptr->record.mode)
   {
      out_data_payload_ptr->nActualSize          = out_data_payload_ptr->nActualSize<<1;
   }
   int16_t *out_ptr = (int16 *)(&(out_data_payload_ptr->nDataBuf));
   int16_t recording_input[ENC_OUT_CIRC_BUF_SIZE_REC*MAX_SAMP_RATE_FACTOR]; //20msec data for WB case
   int16_t *in_ptr = &recording_input[0];// pointer to local input buffer
   int16_t i;

   if(TRUE == venc_ptr->record.first_frame) // TODO: not sure why this is needed
   {
      // send 20ms of zeros, which will act as pre-buffering to take care of processing jitters
      // pre-buffering for processing jitters should be atleast "max. processing time - min. processing time"
      // since delay is not a concern in recording, we are considering  "max. processing time - min. processing time = 20ms"
      memset(in_ptr,0,output_buf_size);
   }
   else
   {
      voice_circbuf_read(&venc_ptr->record.circ_struct_encout,(int8_t*)in_ptr,(int32_t) output_buf_size/2, sizeof(recording_input));
      venc_ptr->wait_mask ^= VENC_REC_BUF_MASK; // don't listen to output buf

   }


   if(VSM_RECORD_TX_RX_STEREO == venc_ptr->record.mode)
   {
      for ( i=0;i<(out_data_payload_ptr->nActualSize>>2);i++)
      {
         out_ptr[i] = in_ptr[i];
         out_ptr[i+(out_data_payload_ptr->nActualSize>>2)] = 0;
      }
   }
   else
   {
      for ( i=0;i<(out_data_payload_ptr->nActualSize)/2;i++)
      {
         out_ptr[i] = in_ptr[i];
      }
   }

   // send output buf to downstream service
   peer_data_Q_msg_ptr = elite_msg_convt_buf_node_to_msg(
         out_buf_mgr_node_ptr,
         ELITE_DATA_BUFFER,
         NULL, /* do not need response */
         0,    /* token */
         0     /* do not care response result*/
         );
   // todo: check for downstreampeer null
   result = qurt_elite_queue_push_back(venc_ptr->record.afe_handle_ptr->dataQ, (uint64_t*)peer_data_Q_msg_ptr);
   if(ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to deliver buffer dowstream. Dropping session(%lx)",venc_ptr->session_num);
      (void) elite_msg_push_payload_to_returnq(venc_ptr->record.buf_q_ptr, (elite_msg_any_payload_t*) out_buf_mgr_node_ptr->pBuffer);
   }
   else
   {
      MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: VENC_RECORD Delivered buffer dowstream, muteCmd(%d) bytes(%ld) session(%lx)",venc_ptr->mute,output_buf_size,venc_ptr->session_num);
   }
   return result;
}

static ADSPResult venc_send_afe_media_type(venc_t *venc_ptr)
{
   ADSPResult result = ADSP_EOK;
   qurt_elite_bufmgr_node_t buf_mgr_node;
   int32_t actual_size;
   elite_msg_any_t media_type_msg;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_send_afe_media_type begin session(%lx)",venc_ptr->session_num);

   if (ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_data_media_type_apr_t) + sizeof( elite_multi_channel_pcm_fmt_blk_t), &buf_mgr_node, (int *)&actual_size)))
   {
      return ADSP_ENEEDMORE;
   }

   elite_msg_data_media_type_apr_t* pMediaTypePayload =
      (elite_msg_data_media_type_apr_t*)buf_mgr_node.pBuffer;

   pMediaTypePayload->pBufferReturnQ = buf_mgr_node.pReturnQ;
   pMediaTypePayload->pResponseQ = NULL;
   pMediaTypePayload->unClientToken = NULL;

   pMediaTypePayload->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;
   pMediaTypePayload->unMediaFormatID   = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

   elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk =
      (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pMediaTypePayload);
   memset(pMediaFormatBlk,0,sizeof(elite_multi_channel_pcm_fmt_blk_t));

   pMediaFormatBlk->num_channels           = 1;
   pMediaFormatBlk->channel_mapping[0]     = PCM_CHANNEL_C;
   if(VSM_RECORD_TX_RX_STEREO == venc_ptr->record.mode)
   {
      pMediaFormatBlk->num_channels           = 2; // stereo mode recording
      pMediaFormatBlk->channel_mapping[0]     = PCM_CHANNEL_L;
      pMediaFormatBlk->channel_mapping[1]     = PCM_CHANNEL_R;
   }
   pMediaFormatBlk->bits_per_sample      = 16;
   pMediaFormatBlk->sample_rate         = venc_ptr->sampling_rate;

   pMediaFormatBlk->is_signed           = TRUE;
   pMediaFormatBlk->is_interleaved      = FALSE;

   media_type_msg.unOpCode = ELITE_DATA_MEDIA_TYPE;
   media_type_msg.pPayload = (void*) pMediaTypePayload;

   result = qurt_elite_queue_push_back(venc_ptr->record.afe_handle_ptr->dataQ, (uint64_t*)&media_type_msg );
   if(ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed deliver buf dwnstrm Dropping! session(%lx)",venc_ptr->session_num);
      (void) elite_msg_push_payload_to_returnq(buf_mgr_node.pReturnQ, (elite_msg_any_payload_t*) buf_mgr_node.pBuffer);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc delivered media type to AFE session(%lx)",venc_ptr->session_num);
      venc_ptr->send_media_type = FALSE;
   }
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_send_afe_media_type end, result(%d) session(%lx)",result,venc_ptr->session_num);
   return result;
}

static int32_t venc_send_dtmf_gen_ended(venc_t *venc_ptr)
{
   vsm_evt_dtmf_generation_ended_t dtmf_gen_ended; // direction
   int32_t rc = ADSP_EOK;
   if( venc_ptr->modules.apr_info_dtmf_gen.self_addr == 0 &&
       venc_ptr->modules.apr_info_dtmf_gen.self_port == 0 &&
       venc_ptr->modules.apr_info_dtmf_gen.client_addr == 0 &&
       venc_ptr->modules.apr_info_dtmf_gen.client_port == 0)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VSM_EVT_DTMF_GENERATION_ENDED APR : Unexpected event, no packet present to raise event :session(%lx)",venc_ptr->session_num);
      rc = ADSP_EUNEXPECTED;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VSM_EVT_DTMF_GENERATION_ENDED APR pkt sent from Q6 - A9 session(%lx)",venc_ptr->session_num);
      dtmf_gen_ended.direction = VSM_SET_DTMF_GEN_TX;
      rc = elite_apr_if_alloc_send_event( venc_ptr->apr_info_ptr->apr_handle,
            venc_ptr->modules.apr_info_dtmf_gen.self_addr,
            venc_ptr->modules.apr_info_dtmf_gen.self_port,
            venc_ptr->modules.apr_info_dtmf_gen.client_addr,
            venc_ptr->modules.apr_info_dtmf_gen.client_port,
            NULL,
            VSM_EVT_DTMF_GENERATION_ENDED,
            &dtmf_gen_ended,
            sizeof(vsm_evt_dtmf_generation_ended_t));

      if (ADSP_FAILED(rc))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to create APR request for VSM_EVT_DTMF_GENERATION_ENDED  0x%08lx :session(%lx)",rc,venc_ptr->session_num);
      }
   }
   return rc;
}

static ADSPResult venc_send_ltetty_tx_char_detected(venc_t *venc_ptr, uint32_t tty_tx_char)
{
   vsm_evt_outofband_tty_tx_char_detected_t tx_char_detected;
   int32_t rc;
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VSM_EVT_OUTOFBAND_TTY_TX_CHAR_DETECTED APR pkt sent from VSM to client, session(%lx)",venc_ptr->session_num);
      tx_char_detected.tty_char = (uint16_t)tty_tx_char;
      rc = (ADSPResult) elite_apr_if_alloc_send_event( venc_ptr->apr_info_ptr->apr_handle,
            venc_ptr->apr_info_ptr->self_addr,
            venc_ptr->apr_info_ptr->self_port,
            venc_ptr->apr_info_ptr->client_addr,
            venc_ptr->apr_info_ptr->client_port,
            NULL, /* token */
            VSM_EVT_OUTOFBAND_TTY_TX_CHAR_DETECTED,
            &tx_char_detected,
            sizeof(vsm_evt_outofband_tty_tx_char_detected_t));

      if (ADSP_FAILED(rc))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to create APR request for VSM_EVT_OUTOFBAND_TTY_TX_CHAR_DETECTED  0x%08lx :session(%lx)",rc,venc_ptr->session_num);
      }
   }

   return rc;
}

static ADSPResult  venc_log_vocoder_packet( uint32_t voc_type, uint32_t log_session_id, void *voc_packet, uint16_t size, uint16_t sampling_rate)
{
#if defined(__qdsp6__) && !defined(SIM)
   int8_t *bufptr[4] = { (int8_t *) voc_packet, NULL, NULL, NULL };
   // log saved input buf and preproc output prior to encode
   voice_log_buffer( bufptr,
         VOICE_LOG_CHAN_PKT_TX_STREAM,
         log_session_id,
         qurt_elite_timer_get_time(),
         VOICE_LOG_DATA_FORMAT_PCM_MONO,
         sampling_rate,
         size,
         &voc_type);   // custom data, to send media type info
#endif

   return ADSP_EOK;
}

static void venc_init_rec_circbuf(venc_t* venc_ptr)
{
   if (NULL != venc_ptr->record.circ_encout_buf_ptr)
   {
      uint8_t samp_rate_factor;
      int16_t temp_buf[SS_DELAY_RECORD_PATH*MAX_SAMP_RATE_FACTOR]; // temp 2msec buffer for worst case - fb number of samples
      memset(temp_buf,0,SS_DELAY_RECORD_PATH*MAX_SAMP_RATE_FACTOR*sizeof(int16_t)); // clear the buffer

      samp_rate_factor=venc_ptr->samp_rate_factor;

      voice_circbuf_init(&(venc_ptr->record.circ_struct_encout), (int8_t*)(venc_ptr->record.circ_encout_buf_ptr),
            (int32_t)samp_rate_factor*ENC_OUT_CIRC_BUF_SIZE_REC /* 20ms + 4msec delay*/,
            MONO_VOICE,
            (int32_t)16 /*bitperchannel*/
            );
      voice_circbuf_write(&(venc_ptr->record.circ_struct_encout),
            (int8_t *)&(temp_buf[0]),
            samp_rate_factor*SS_DELAY_RECORD_PATH // add 2ms delay for pre buffering
            );
   }
}

static void venc_vtm_subscribe(venc_t* venc_ptr,Vtm_SubUnsubMsg_t *data_ptr)
{
   ADSPResult result = ADSP_EFAILED;
   if(VFR_MODE_END > venc_ptr->vfr_mode)
   {
      if (ADSP_FAILED(result = voice_custom_vt_sub_unsub_msg_send(NULL,venc_ptr->vtm_cmd_q_ptr,VOICE_TIMER_SUBSCRIBE,NULL,FALSE,data_ptr)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed Venc subscribe with vtm memory(%p) Vtm session(%#x) ",data_ptr, (int)venc_ptr->session_num);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Venc subscribed with vtm memory(%p) Vtm session(%#x) ",data_ptr, (int)venc_ptr->session_num);
      }
   }
}

static void venc_vtm_unsubscribe(venc_t* venc_ptr,Vtm_SubUnsubMsg_t *data_ptr,uint32_t mask)
{
   ADSPResult result = ADSP_EFAILED;
   if(VFR_MODE_END > venc_ptr->vfr_mode)
   {
      if (ADSP_FAILED(result = voice_custom_vt_sub_unsub_msg_send(NULL,venc_ptr->vtm_cmd_q_ptr,VOICE_TIMER_UNSUBSCRIBE,NULL,FALSE,data_ptr)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed Venc unsubscribe with vtm memory(%p) Vtm session(%#x) ",data_ptr, (int)venc_ptr->session_num);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Venc unsubscribed with vtm memory(%p) Vtm session(%#x) ",data_ptr, (int)venc_ptr->session_num);
      }

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Venc waiting for unsubscribe to finish - vtm memory(%p) Vtm session(%#x) ",data_ptr, (int)venc_ptr->session_num);
      (void)qurt_elite_channel_wait(&venc_ptr->qurt_elite_channel, mask);
      qurt_elite_signal_clear(data_ptr->signal_end_ptr);
   }
}

static ADSPResult venc_config_host_pcm(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   ADSPResult apr_pkt_result = APR_EOK;
   venc_t* venc_ptr = (venc_t*) pInstance;
   elite_msg_custom_voc_config_host_pcm_type *pConfig = (elite_msg_custom_voc_config_host_pcm_type *) pMsg->pPayload;
   elite_apr_packet_t *apr_packet_ptr = pConfig->apr_packet_ptr;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_config_host_pcm begin session(%x)\n",(int)venc_ptr->session_num);

   if( (VOICE_CMD_START_HOST_PCM_V2 == elite_apr_if_get_opcode( apr_packet_ptr)) &&
         ((VOICE_NB_SAMPLING_RATE == venc_ptr->sampling_rate) || (VOICE_WB_SAMPLING_RATE == venc_ptr->sampling_rate) ||
                                                                  (VOICE_SWB_SAMPLING_RATE == venc_ptr->sampling_rate) || (VOICE_FB_SAMPLING_RATE == venc_ptr->sampling_rate))
     )
   {
      voice_start_host_pcm_v2_t    *start_host_pcm_ptr = (voice_start_host_pcm_v2_t *) elite_apr_if_get_payload_ptr( apr_packet_ptr);

      uint8 index;

      // check if default Tx stream tap point in the list
      for( index = 0; index < start_host_pcm_ptr->num_tap_points; index++)
      {
         if( VOICESTREAM_MODULE_TX == start_host_pcm_ptr->tap_points[index].tap_point )
         {
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_config_host_pcm: tap point being enabled is (%lx), session(%x)\n",(uint32_t)VOICESTREAM_MODULE_TX,(int)venc_ptr->session_num);
            break;
         }
      }

      // check if valid index found.  TODO: check if tap point already enabled
      if( index < start_host_pcm_ptr->num_tap_points)
      {

         voice_tap_point_v2_t *voice_tap_point_ptr = &start_host_pcm_ptr->tap_points[index];

         if(NULL == voice_tap_point_ptr->mem_map_handle)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Venc received NULL mem map handle!");
            apr_pkt_result = APR_EBADPARAM;
         }
         else
         {
            /* copy Host Pcm APR info */
            venc_ptr->modules.apr_info_hpcm.apr_handle        = pConfig->apr_handle;
            venc_ptr->modules.apr_info_hpcm.self_addr         = elite_apr_if_get_dst_addr(apr_packet_ptr);
            venc_ptr->modules.apr_info_hpcm.self_port         = elite_apr_if_get_dst_port(apr_packet_ptr);
            venc_ptr->modules.apr_info_hpcm.client_addr       = elite_apr_if_get_src_addr(apr_packet_ptr);
            venc_ptr->modules.apr_info_hpcm.client_port       = elite_apr_if_get_src_port(apr_packet_ptr);
            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Venc setting hpcm context, memmmap handle (%x), client addr (%x), client port (%x) direction %d",
                (unsigned int)voice_tap_point_ptr->mem_map_handle,
                venc_ptr->modules.apr_info_hpcm.client_addr,
                venc_ptr->modules.apr_info_hpcm.client_port,
                voice_tap_point_ptr->direction);

            if(voice_tap_point_ptr->direction & VOICE_HOST_PCM_READ)
            {
              venc_ptr->modules.hpcm_read_enable = TRUE;
            }
            else
            {
              venc_ptr->modules.hpcm_read_enable = FALSE;
            }

            if(voice_tap_point_ptr->direction & VOICE_HOST_PCM_WRITE)
            {
              venc_ptr->modules.hpcm_write_enable = TRUE;
            }
            else
            {
              venc_ptr->modules.hpcm_write_enable = FALSE;
            }

            // send HPCM start to HPCM CAPI module
            if(venc_ptr->modules.special_capis[HPCM]->module_ptr)
            {
               capi_v2_buf_t param_data_buf;
               capi_v2_port_info_t port_info;
               port_info.is_valid = FALSE;
               param_data_buf.data_ptr = (int8_t *) voice_tap_point_ptr;
               param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(voice_tap_point_v2_t);
               nResult = venc_ptr->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(venc_ptr->modules.special_capis[HPCM]->module_ptr,
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
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! venc_config_host_pcm() failed due to NULL pointer error");
               nResult = ADSP_EBADPARAM;
            }
         }
      } // end of if valid index found
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_config_host_pcm START command NOP - tappoint not found or already enabled, session(%lx)\n",venc_ptr->session_num);
      }
   }
   else if( VOICE_CMD_STOP_HOST_PCM == elite_apr_if_get_opcode( apr_packet_ptr))
   {
      venc_ptr->modules.hpcm_read_enable = 0;  //disable read
      venc_ptr->modules.hpcm_write_enable = 0; //disable write
      // send HPCM stop to HPCM CAPI module
      if(venc_ptr->modules.special_capis[HPCM]->module_ptr)
      {
         capi_v2_buf_t param_data_buf;
         capi_v2_port_info_t port_info;
         port_info.is_valid = FALSE;
         param_data_buf.data_ptr = NULL; // no payload is needed
         param_data_buf.actual_data_len = param_data_buf.max_data_len = 0;
         nResult = venc_ptr->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(venc_ptr->modules.special_capis[HPCM]->module_ptr,
               FWK_EXTN_PARAM_ID_HPCM_STOP, &port_info, &param_data_buf);
         nResult = capi_v2_err_to_adsp_result(nResult);
         if(ADSP_FAILED(nResult))
         {
             MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! HPCM_STOP set param to CAPI failed!!");
         }
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! venc_config_host_pcm() failed due to NULL pointer error");
         nResult = ADSP_EBADPARAM;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_config_host_pcm failed - opcode or sampling rate unsupported, session(%lx)\n",venc_ptr->session_num);
      apr_pkt_result = APR_EUNSUPPORTED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_config_host_pcm end apr pkt result(%d) session(%x)\n",apr_pkt_result, (int)venc_ptr->session_num);

   elite_svc_send_ack(pMsg, apr_pkt_result);

   return nResult;
}

static ADSPResult venc_allocate_mem(venc_t * venc_ptr)
{
   ADSPResult result = ADSP_EOK;
   uint32_t total_size;
   uint32_t ss_size, rec_buf_size,inp_buf_size,out_buf_size,mute_inp_size;
   uint16_t sampling_rate_factor = VOICE_FB_SAMPLING_RATE/VOICE_NB_SAMPLING_RATE; //considering VOICE_FB_SAMPLING_RATE for max mem allocation

   inp_buf_size = VOICE_ROUNDTO8(VOICE_FRAME_SIZE_FB<<1);   //worst case allocation @ FB 20ms i.e. 1920 bytes
   out_buf_size = VOICE_ROUNDTO8(VOICE_FRAME_SIZE_FB<<1);   //worst case allocation @ FB 20ms i.e. 1920 bytes
   rec_buf_size = VOICE_ROUNDTO8(inp_buf_size+((2*SS_DELAY_REC_PATH_NB*sampling_rate_factor)<<1));
   voice_ss_get_size(&ss_size);
   ss_size = VOICE_ROUNDTO8(ss_size);
   mute_inp_size= VOICE_FRAME_SIZE_FB << 1;   // worst case allocation

   total_size = inp_buf_size
      + out_buf_size
      + rec_buf_size
      + ss_size
      + mute_inp_size;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Computed memory requirement%d",(int)total_size);

   venc_ptr->memory.start_addr_ptr = (int8_t *)qurt_elite_memory_malloc(total_size, QURT_ELITE_HEAP_DEFAULT);
   if (NULL == venc_ptr->memory.start_addr_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to allocate memory for venc modules!! session(%lx)",venc_ptr->session_num);
      return ADSP_ENOMEMORY;
   }
   memset( venc_ptr->memory.start_addr_ptr, 0, total_size); // clear the memory before use

   venc_ptr->memory.size = total_size; // size allocated

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Allocated memory %d", (int)venc_ptr->memory.size);

   venc_ptr->memory.usage_addr_ptr = venc_ptr->memory.start_addr_ptr; // Usage level pointer

   venc_ptr->proc.mute_inp_ptr = (int16_t *)venc_ptr->memory.usage_addr_ptr;
   venc_ptr->memory.usage_addr_ptr += VOICE_ROUNDTO8(mute_inp_size); // UsageAddr is a byte pointer

   venc_ptr->io.in_buf = (int16_t *)venc_ptr->memory.usage_addr_ptr;
   venc_ptr->memory.usage_addr_ptr += VOICE_ROUNDTO8(inp_buf_size); // UsageAddr is a byte pointer

   venc_ptr->io.out_buf = (int16_t *)venc_ptr->memory.usage_addr_ptr;
   venc_ptr->memory.usage_addr_ptr += VOICE_ROUNDTO8(out_buf_size); // UsageAddr is a byte pointer

   venc_ptr->record.circ_encout_buf_ptr = venc_ptr->memory.usage_addr_ptr;
   venc_ptr->memory.usage_addr_ptr += VOICE_ROUNDTO8(rec_buf_size); // UsageAddr is a byte pointer

   venc_ptr->record.ss_struct_ptr = venc_ptr->memory.usage_addr_ptr;
   venc_ptr->memory.usage_addr_ptr += VOICE_ROUNDTO8(ss_size); // UsageAddr is a byte pointer

   // Sanity check to see the allocated memory was enough
   if ((uint32_t)(venc_ptr->memory.usage_addr_ptr - venc_ptr->memory.start_addr_ptr) <= venc_ptr->memory.size)
   {
      result = ADSP_EOK;
   }
   else
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Error: allocated memory %d, memory consumed %d session(%x)",(int)venc_ptr->memory.size,
            (int)(venc_ptr->memory.usage_addr_ptr - venc_ptr->memory.start_addr_ptr),
            (int)venc_ptr->session_num);
      result = ADSP_ENOMEMORY;
      return result;
   }

   voice_ss_set_mem(&(venc_ptr->record.ss_struct),venc_ptr->record.ss_struct_ptr,ss_size);

   return result;
}

static void venc_allocate_mem_free(venc_t * venc_ptr)
{

   venc_ptr->io.in_buf = NULL;
   venc_ptr->io.out_buf = NULL;
   venc_ptr->record.circ_encout_buf_ptr = NULL;

   if(NULL !=  venc_ptr->record.ss_struct_ptr)
   {
      voice_ss_end(&(venc_ptr->record.ss_struct));
      venc_ptr->record.ss_struct_ptr = NULL;
   }

   //destroy vocoder
   if(venc_ptr->is_encoder_capi)
   {
      venc_encoder_capi_destroy(venc_ptr);
   }
   else
   {
      if(venc_ptr->encoder.enc_state_ptr)
      {
         qurt_elite_memory_free(venc_ptr->encoder.enc_state_ptr);
         venc_ptr->encoder.enc_state_ptr = NULL;
      }
   }
   venc_ptr->proc.mute_inp_ptr = NULL;

   qurt_elite_memory_free(venc_ptr->memory.start_addr_ptr);
   venc_ptr->memory.start_addr_ptr = NULL;
   venc_ptr->memory.usage_addr_ptr = NULL;
   venc_ptr->memory.size = 0;


}



ADSPResult venc_modules_init(venc_t* venc_ptr)
{


   ADSPResult result = ADSP_EOK;

   venc_init_rec_circbuf(venc_ptr);

   return result;


}

static ADSPResult venc_aggregate_modules_kpps(void* instance_ptr, uint32_t* kpps_changed)
{
   venc_t* venc_ptr = (venc_t*)instance_ptr;

   if ((NULL == venc_ptr) || (NULL == kpps_changed))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Voice Encoder KPPS aggregator received bad pointers");
      return ADSP_EBADPARAM;
   }

   uint32_t kpps;
   uint32_t aggregate_kpps=0;

   // DTMF PCM
   kpps = venc_ptr->modules.dtmf_gen.kpps;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: DTMF Gen kpps (%d), session number (%lx)", (int)kpps, venc_ptr->session_num);
   aggregate_kpps += kpps;

   // Host PCM
   kpps = 0;
   kpps = venc_ptr->modules.hpcm.kpps;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Host PCM kpps (%d), session number (%lx)", (int)kpps, venc_ptr->session_num);
   aggregate_kpps += kpps;

   // CTM Tx
   kpps = 0;
   kpps = venc_ptr->modules.ctm_tx.ctm_tx_capi.kpps;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ctm_tx kpps (%d), session number (%lx)", (int)kpps, venc_ptr->session_num);
   aggregate_kpps += kpps;

   // ltetty_tx
   kpps = 0;
   kpps = venc_ptr->modules.ltetty_tx.kpps;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: ltetty_tx kpps (%d), session number (%lx)", (int)kpps, venc_ptr->session_num);
   aggregate_kpps += kpps;

   // Limiter Tx
   kpps = 0;
   kpps = venc_ptr->modules.limiter.is_enabled == TRUE ? venc_ptr->modules.limiter.kpps : 0;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Limiter Tx kpps (%d), session number (%lx)", (int)kpps, venc_ptr->session_num);
   aggregate_kpps += kpps;

   // CNG Tx
   kpps = 0;
   kpps = venc_ptr->modules.cng.kpps;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "CNG Tx kpps (%d), session number (%lx)", (int)kpps, venc_ptr->session_num);
   aggregate_kpps += kpps;

   // Voice Encoder KPPS
   kpps = 0;
   if(venc_ptr->is_encoder_capi)
   {
      kpps = venc_ptr->encoder_capi.common.kpps;
   }
   else
   {
      vcmn_find_vockpps_table(VOICE_ENCODE_KPPS_TABLE, venc_ptr->voc_type, &kpps);
   }
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Encoder kpps (%d), session number (%lx)", (int)kpps, venc_ptr->session_num);
   aggregate_kpps += kpps;

   // TODO: Retaining this margin for safety, will need to be reassessed
   aggregate_kpps += 1000; // adding extra 1000 kpps to ceil the number to mpps

   // Retaining behavior as it is as of today. That is to report an increase in KPPS only.
   // Once the VOICE_CMD_SET_TIMING_PARAMS commands are removed,
   // this can be modified to (venc_ptr->aggregate_kpps != aggregate_kpps)
   if (venc_ptr->aggregate_kpps >= aggregate_kpps)
   {
      *kpps_changed = FALSE;
   }
   else
   {
      *kpps_changed = TRUE;
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VEnc KPPS changed from (%lu) to (%lu), session(%lx)", venc_ptr->aggregate_kpps, aggregate_kpps, venc_ptr->session_num);
   }

   // Update state
   venc_ptr->aggregate_kpps = aggregate_kpps;

   return ADSP_EOK;
}

static ADSPResult venc_aggregate_modules_delay(void* instance_ptr)
{
   venc_t* venc_ptr = (venc_t*)instance_ptr;

   if ((NULL == venc_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Voice Encoder delay aggregator received bad pointers");
      return ADSP_EBADPARAM;
   }

   uint32_t delay;
   uint32_t aggregate_delay=0;

   //Stream limiter delay
   delay = venc_ptr->modules.limiter.is_enabled == TRUE ? venc_ptr->modules.limiter.delay : 0;
   aggregate_delay += delay;

   //CNG delay
   delay = venc_ptr->modules.cng.delay;
   aggregate_delay += delay;

   // Voice Encoder Delay
   delay = 0;
   if(venc_ptr->is_encoder_capi)
   {
      delay = venc_ptr->encoder_capi.common.delay;
   }
   aggregate_delay += delay;

   // Update state
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Encoder delay (%d), session number (%lx)", (int)aggregate_delay, venc_ptr->session_num);
   venc_ptr->aggregate_delay = aggregate_delay;

   return ADSP_EOK;
}

static ADSPResult venc_set_timingv3_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_voc_timing_param_type *set_timing_cmd_ptr = (elite_msg_custom_voc_timing_param_type *) msg_ptr->pPayload;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_timingv3_cmd begin session(%lx)",venc_ptr->session_num);

   // Verify Stop state of thread before updating timing values.
   if (FALSE == venc_ptr->process_data)
   {
      vsm_set_timing_params_v2_t* vfr_cmd_ptr = (vsm_set_timing_params_v2_t*)set_timing_cmd_ptr->param_data_ptr;

      //dbg msg is printed in VSM with all params
      /*MSG_7(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_timingv3_cmd, mode(%d), VSID(0x%lx), \
        enc_offset(%d), decreq_offset(%d), dec_offset(%d),decpp_offset(%d),session(%x)",
        vfr_cmd_ptr->mode,vfr_cmd_ptr->vsid,vfr_cmd_ptr->enc_offset,vfr_cmd_ptr->dec_req_offset,
        vfr_cmd_ptr->dec_offset,vfr_cmd_ptr->decpp_offset,venc_ptr->session_num); */

      // Verify validity of VFR mode. In this version of timing cmd, mode supports only two values - VFR_NONE and VFR_HARD
      // If VFR_HARD, further information is derived from VSID
      if ((VFR_HARD >= vfr_cmd_ptr->mode) )
      {
         venc_ptr->vfr_mode = vfr_cmd_ptr->mode;
      }
      else
      {
         venc_ptr->vfr_mode = VFR_NONE;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_set_timingv3_cmd, invalid mode(%x),setting to VFR_NONE,session(%lx)",vfr_cmd_ptr->mode,venc_ptr->session_num);
         result = ADSP_EBADPARAM;
      }

      venc_ptr->vtm_sub_unsub_data.vfr_cycle = vfr_cmd_ptr->vfr_cycle;

      // Verify validity of VSID. In VFR_HARD case, VSID should be non-zero. In VFR_NONE case, VSID is don't care.
      if ((VFR_HARD == venc_ptr->vfr_mode) && (0 == vfr_cmd_ptr->vsid))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_set_timingv3_cmd, invalid VSID(%lx), session(%lx)",vfr_cmd_ptr->vsid,venc_ptr->session_num);
         result = ADSP_EBADPARAM;
      }
      else
      {
         venc_ptr->vsid = vfr_cmd_ptr->vsid;
      }

      // Verify validity of enc_offset
      // MIN_TIMER_OFFSET is set to 0, so no need to check if offset is below min because it's unsigned
      if (vfr_cmd_ptr->vfr_cycle < vfr_cmd_ptr->enc_offset)
      {
         venc_ptr->vtm_sub_unsub_data.offset = 8000; // default 8ms
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_set_timingv3_cmd, Invalid enc_offset(%x), defaulting to 8ms, session(%lx)", \
               vfr_cmd_ptr->enc_offset,venc_ptr->session_num);
         result = ADSP_EBADPARAM;
      }
      else
      {
         venc_ptr->vtm_sub_unsub_data.offset = vfr_cmd_ptr->enc_offset;
      }

      //write into vtm subscribe struct
      venc_ptr->vtm_sub_unsub_data.vfr_mode = (uint8_t)venc_ptr->vfr_mode;
      venc_ptr->vtm_sub_unsub_data.vsid = venc_ptr->vsid;
      venc_ptr->vtm_sub_unsub_data.timing_ver = VFR_CLIENT_INFO_VER_2; //update version of timing used
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Timing can't be changed in RUN, session(%lx)",venc_ptr->session_num);
      result = ADSP_EBUSY;
   }

   elite_svc_send_ack(msg_ptr, result);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_set_timingv3_cmd end result(%d) session(%lx)",result,venc_ptr->session_num);
   return ADSP_EOK;
}


static ADSPResult venc_register_event(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_event_reg_type *payload_ptr = (elite_msg_custom_event_reg_type*) msg_ptr->pPayload;
   uint32_t event_id = payload_ptr->event_id;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Venc received registration for event %lx", event_id);

   switch(event_id)
   {
      case VSM_EVT_VOC_OPERATING_MODE_UPDATE:
         {
            // set mode detection to true
            venc_ptr->vocoder_op_detection = 1;
            // Issue default mode event
            venc_send_mode_notification_v2(instance_ptr);
            break;
         }
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Venc received registration for invalid event %lx", event_id);
         }
   }
   elite_msg_return_payload_buffer( msg_ptr );
   return ADSP_EOK;
}

static ADSPResult venc_unregister_event(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_event_reg_type *payload_ptr = (elite_msg_custom_event_reg_type*) msg_ptr->pPayload;
   uint32_t event_id = payload_ptr->event_id;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Venc received deregistration for event %lx", event_id);

   switch(event_id)
   {
      case VSM_EVT_VOC_OPERATING_MODE_UPDATE:
         {
            // set mode detection to true
            venc_ptr->vocoder_op_detection = 0;
            break;
         }
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Venc received deregistration for invalid event %lx", event_id);
         }
   }
   elite_msg_return_payload_buffer( msg_ptr );
   return ADSP_EOK;
}

static ADSPResult venc_send_mode_notification_v2(void* instance_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;

   if(0 == venc_ptr->vocoder_op_detection)
   {
      return ADSP_EOK;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_send_mode_notification_v2 begin session(%lx)",venc_ptr->session_num);
   vsm_evt_voc_operating_mode_update_t mode_update;

   // EAMR is always sent by decoder
   if(VSM_MEDIA_TYPE_EAMR == venc_ptr->voc_type)
   {
      return ADSP_EOK;
   }

   mode_update.direction = VSM_VOC_OPERATING_MODE_DIRECTION_TX;
   mode_update.reserved = 0;
   mode_update.mode = venc_ptr->vocoder_op_mode;

   result = elite_apr_if_alloc_send_event( venc_ptr->apr_info_ptr->apr_handle,
         venc_ptr->apr_info_ptr->self_addr,
         venc_ptr->apr_info_ptr->self_port,
         venc_ptr->apr_info_ptr->self_addr,
         venc_ptr->apr_info_ptr->self_port,
         0,
         VSM_EVT_VOC_OPERATING_MODE_UPDATE,
         &mode_update,
         (sizeof(mode_update)));

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to create APR request for VSM_EVT_VOC_OPERATING_MODE_UPDATE :session(%lx)",venc_ptr->session_num);
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Venc sent the vocoder mode change notification, session(0x%lx), mode(0x%lx)",venc_ptr->session_num, mode_update.mode);
   }
   return result;
}

static ADSPResult venc_set_param_v3(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_set_param_v3_type *payload_ptr = (elite_msg_custom_set_param_v3_type*) msg_ptr->pPayload;
   uint32_t cal_handle = payload_ptr->cal_handle;
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Venc received set_param_v3, handle %lx, session(%lx)", cal_handle, venc_ptr->session_num);

   //call into mvm using mvm_call as an entry point to cvd_cal_query
   vss_imvm_cmd_cal_query_t mvm_payload;
   mvm_payload.query_handle = payload_ptr->cal_handle;
   mvm_payload.cb_fn = venc_calibration_cb_func;
   mvm_payload.client_data = instance_ptr;

   result = mvm_call(MVM_CMDID_CAL_QUERY, &mvm_payload, sizeof(mvm_payload));

   elite_svc_send_ack(msg_ptr, result);
   return result;
}

void venc_calibration_cb_func(void* cal_params_payload_ptr, void* cb_data)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)cb_data;

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


   result = venc_set_param_int(venc_ptr, mod_id, instance_id, param_id, param_data, param_size);
   if(ADSP_EOK != result && ADSP_EUNSUPPORTED != result)
   {
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc set param error %u, mod %lx,instance %lx, param %lx, session(%lx)",
            result, mod_id, instance_id, param_id, venc_ptr->session_num);
   }
   return;
}

static ADSPResult venc_set_enc_rate(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_set_enc_rate_type *payload_ptr = (elite_msg_custom_set_enc_rate_type*)msg_ptr->pPayload;
   uint32_t encoder_rate;

   encoder_rate = payload_ptr->encoder_rate;

   if(venc_ptr->is_encoder_capi)
   {
      voc_param_enc_rate_t enc_rate;
      enc_rate.rate = encoder_rate;
      result =  venc_capi_set_param(&venc_ptr->encoder_capi.common, VOC_PARAM_ENC_RATE, &enc_rate, sizeof(enc_rate), sizeof(enc_rate));
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VSM_CMD_ENC_SET_RATE: Invalid mediatype session(%lx)",
            venc_ptr->session_num);
      result = ADSP_EUNSUPPORTED;
   }
   elite_svc_send_ack(msg_ptr, result);
   return result;
}

static void voice_ecall_tx (venc_t *venc_ptr, int16_t *in_out_ptr)
{
	if(NULL == venc_ptr->modules.hpcm.module_ptr)
	{
		// HPCM is stubbed
		return;
	}
   int16_t ecall_temp_buffer[VOICE_FRAME_SIZE_FB];   // worst case size: 20ms samples at 48KHz

   // Clear ecall_temp_buffer
   memset (&ecall_temp_buffer[0], 0, sizeof(ecall_temp_buffer));

   // If hpcm read enable, client wants to read data from fw.
   // Ecall interface API's are defined for 14-bit input/output
   // Input is in 16-bit format so convert input from 16-bit to 14 bit before giving it to client.
   if (TRUE == venc_ptr->modules.hpcm_read_enable)
   {
      for( uint32_t i=0; i<venc_ptr->io.frame_samples; i++)
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

      input_buf.actual_data_len = venc_ptr->io.frame_samples<<1;
      input_buf.max_data_len = VOICE_FRAME_SIZE_FB<<1;
      input_buf.data_ptr = (int8_t*)ecall_temp_buffer;

      input_stream.bufs_num = 1;
      input_stream.buf_ptr = &input_buf;

      input_stream_buf[0] = &input_stream;

      result = venc_ptr->modules.hpcm.module_ptr->vtbl_ptr->process(venc_ptr->modules.hpcm.module_ptr,input_stream_buf,input_stream_buf);

      if(CAPI_V2_FAILED(result))
      {
		  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! CAPI resampler process(): ERROR!! result(%#lx)",result);
      }
   }

   // If hpcm write enable, client has wwritten data to fw.
   // Ecall interface API's are defined for 14-bit input/output
   // Ecall output is in 14-bit format so convert output from 14-bit to 16 bit before processing in fw
   if (TRUE == venc_ptr->modules.hpcm_write_enable)
   {
      for( uint32_t i=0; i<venc_ptr->io.frame_samples; i++)
      {
         in_out_ptr[i] = ecall_temp_buffer[i] << 2;   // convert output 14-bit to 16-bit
      }
   }

}

ADSPResult venc_encoder_capi_destroy(venc_t* venc_ptr)
{
   if(venc_ptr->encoder_capi.common.module_ptr)
   {
      if(venc_ptr->encoder_capi.common.module_ptr->vtbl_ptr)
      {
         venc_ptr->encoder_capi.common.module_ptr->vtbl_ptr->end(venc_ptr->encoder_capi.common.module_ptr);
         venc_ptr->encoder_capi.common.module_ptr->vtbl_ptr = NULL;
      }
      qurt_elite_memory_free (venc_ptr->encoder_capi.common.module_ptr);
      venc_ptr->encoder_capi.common.module_ptr = NULL;
   }

   if(venc_ptr->encoder_capi.amdb_handle_info.h.capi_v2_handle)
   {
      adsp_amdb_release_handles(&venc_ptr->encoder_capi.amdb_handle_info, 1);
      venc_ptr->encoder_capi.amdb_handle_info.h.capi_v2_handle = NULL;
   }

   if(venc_ptr->encoder_capi.common.fwk_extn_ptr)
   {
      qurt_elite_memory_free(venc_ptr->encoder_capi.common.fwk_extn_ptr);
      venc_ptr->encoder_capi.common.fwk_extn_ptr = NULL;
   }

   // Need to reset all the service level variables for encoder capi.
   memset(&venc_ptr->encoder_capi, 0, sizeof(venc_ptr->encoder_capi));

   venc_ptr->voc_type = VSM_MEDIA_TYPE_NONE;
   venc_ptr->vocoder_op_mode = VSM_VOC_OPERATING_MODE_NONE;
   venc_ptr->is_encoder_capi = FALSE;

   return ADSP_EOK;
}

ADSPResult venc_encoder_capi_get_amdb_handle(venc_t* venc_ptr, uint32_t media_type)
{
   // Fill the structure to query the handles from AMDB
   venc_ptr->encoder_capi.amdb_handle_info.interface_type = CAPI_V2;
   venc_ptr->encoder_capi.amdb_handle_info.type = AMDB_MODULE_TYPE_ENCODER;
   venc_ptr->encoder_capi.amdb_handle_info.id1 = media_type;
   venc_ptr->encoder_capi.amdb_handle_info.id2 = 0;
   venc_ptr->encoder_capi.amdb_handle_info.h.capi_v2_handle = NULL;
   venc_ptr->encoder_capi.amdb_handle_info.result = ADSP_EFAILED;

   adsp_amdb_get_modules_request(&venc_ptr->encoder_capi.amdb_handle_info, 1, NULL, NULL);

   if(ADSP_FAILED(venc_ptr->encoder_capi.amdb_handle_info.result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: venc_encoder_capi_get_amdb_handle: failed to get AMDB handle for media_type(0x%lx)", media_type);
      return venc_ptr->encoder_capi.amdb_handle_info.result;
   }

   if ((CAPI_V2 != venc_ptr->encoder_capi.amdb_handle_info.interface_type) && (STUB != venc_ptr->encoder_capi.amdb_handle_info.interface_type))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP:  AMDB handle for media_type(0x%lx), has interface type(%d), when expected type(%d).",
            media_type, (int)venc_ptr->encoder_capi.amdb_handle_info.interface_type, (int)CAPI_V2);
      return ADSP_EUNSUPPORTED;
   }

   //IF the vocoder capi is STUB, then we need to return error for mediatype
   if (STUB == venc_ptr->encoder_capi.amdb_handle_info.interface_type)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP:  AMDB handle for media_type(0x%lx), is stubbed",
            media_type, (int)venc_ptr->encoder_capi.amdb_handle_info.interface_type);
      venc_ptr->encoder_capi.common.amdb_handle_ptr = NULL;
      return ADSP_EUNSUPPORTED;
   }
   else
   {
      venc_ptr->encoder_capi.common.amdb_handle_ptr = (void*)venc_ptr->encoder_capi.amdb_handle_info.h.capi_v2_handle;
   }
   return ADSP_EOK;
}

ADSPResult venc_encoder_capi_init(venc_t* venc_ptr, uint32_t media_type)
{
   ADSPResult result = ADSP_EOK;
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   void *mem_ptr = NULL;

   //Initialize variables
   venc_ptr->encoder_capi.common.module_id = media_type;
   venc_ptr->voc_type = media_type;
   venc_ptr->encoder_capi.common.svc_ptr = (void *)venc_ptr;
   venc_ptr->encoder_capi.common.mem_size_allocated = 0;


   // Get amdb handle for encoder capi from amdb
   result = venc_encoder_capi_get_amdb_handle(venc_ptr, media_type);
   if(ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_encoder_capi_init: failed. result(0x%ld)", result);
      return result;
   }

   uint32_t enc_capi_size = voice_capi_get_static_prop (&venc_ptr->encoder_capi.common);

   if(0 == enc_capi_size)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_encoder_capi_init: voice_capi_get_static_prop returned size(%ld) for media_type(0x%lx)",
            enc_capi_size, venc_ptr->encoder_capi.common.module_id);
      return ADSP_EFAILED;
   }

   //ignore the error for intf extension query
   voice_capi_query_intf_ext(&venc_ptr->encoder_capi.common);

   // allocate encoder capi memory and initialize encoder capi
   mem_ptr = (int8_t *)qurt_elite_memory_malloc(venc_ptr->encoder_capi.common.mem_size_allocated, QURT_ELITE_HEAP_DEFAULT);

   if (NULL == mem_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_encoder_capi_init: Out of Memory!!");
      return ADSP_ENOMEMORY;
   }

   {
      // populate init prop list for capi modules
      capi_v2_proplist_t init_props_list;

      capi_v2_port_info_t port_info;
      port_info.is_valid = FALSE;

      capi_v2_event_callback_info_t cb_info = { venc_encoder_capi_cb_func, NULL };
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

      cb_info.event_context = &venc_ptr->encoder_capi;
      venc_ptr->encoder_capi.common.module_ptr = (capi_v2_t*) mem_ptr;

      //Call capi init.
      capi_result = adsp_amdb_capi_v2_init_f((adsp_amdb_capi_v2_t*)venc_ptr->encoder_capi.common.amdb_handle_ptr, venc_ptr->encoder_capi.common.module_ptr, &init_props_list);

      if(CAPI_V2_EOK != capi_result)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_encoder_capi_init: media_type(0x%lx) failed to init with error(%ld).",
               venc_ptr->encoder_capi.common.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
      else
      {
         if(venc_ptr->encoder_capi.common.fwk_extn_ptr)
         {
            for (uint32_t i=0; i < venc_ptr->encoder_capi.common.num_extensions.num_extensions; i++)
            {
               switch (venc_ptr->encoder_capi.common.fwk_extn_ptr[i].id)
               {
                  case FWK_EXTN_SERVICE_STATE:
                  {
                     //ignore the error for fwk extension handler
                     voice_capi_service_state_fwk_extn_handler(&venc_ptr->encoder_capi.common);
                     break;
                  }
                  case FWK_EXTN_DTMF_GEN:
                  {
                     // TODO : To execute VSM_CMD_SET_DTMF_GENERATION command only if this framework extension is supported.
                     break;
                  }
                  default:
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_encoder_capi_init: received unsupported fwk_extn_id: 0x%lx",
                           venc_ptr->encoder_capi.common.fwk_extn_ptr[i].id);
                     break;
                  }
               }
            }
         }
      }
   }

   /*
    * If the vocoder is static or has known sampling rate, then the send init_media format now.
    * else it will be set as part of SET_PP_SAMP_RATE handling
    */
   uint32_t sampling_rate = voice_enc_get_default_sampling_rate(venc_ptr->voc_type);

   if(VOICE_INVALID_SAMPLING_RATE != sampling_rate)
   {
      result = venc_encoder_capi_set_media_fmt(venc_ptr,sampling_rate);
      if(ADSP_SUCCEEDED(result))
      {
         //set venc samping rates
         venc_ptr->sampling_rate = venc_ptr->encoder_capi.input_media_type.data_format.sampling_rate;
         venc_ptr->samp_rate_factor = venc_ptr->sampling_rate/VOICE_NB_SAMPLING_RATE;
         venc_ptr->io.frame_samples = VOICE_FRAME_SIZE_NB_20MS*(venc_ptr->samp_rate_factor);
      }
   }

   //Query data threshold if data buffering is true
   if (venc_ptr->encoder_capi.common.requires_data_buffering)
   {
      result = venc_encoder_capi_get_data_threshold(venc_ptr);
   }

   return result;
}

ADSPResult venc_encoder_capi_set_media_fmt(venc_t *venc_ptr,uint32_t sampling_rate)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   if(NULL == venc_ptr->encoder_capi.common.module_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_encoder_capi_set_media_fmt: NULL module pointer error");
      return ADSP_EBADPARAM;
   }
   // populate init prop list for capi modules
   capi_v2_proplist_t init_props_list;

   capi_v2_port_info_t port_info = {FALSE, FALSE, 0};

   //update input mediatype
   venc_ptr->encoder_capi.input_media_type.media_format.format_header.data_format = CAPI_V2_FIXED_POINT;

   venc_ptr->encoder_capi.input_media_type.data_format.bitstream_format  = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   venc_ptr->encoder_capi.input_media_type.data_format.num_channels      = 1;
   venc_ptr->encoder_capi.input_media_type.data_format.bits_per_sample   = 16;
   venc_ptr->encoder_capi.input_media_type.data_format.data_is_signed    = TRUE;
   venc_ptr->encoder_capi.input_media_type.data_format.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
   venc_ptr->encoder_capi.input_media_type.data_format.q_factor          = 15;
   venc_ptr->encoder_capi.input_media_type.data_format.sampling_rate = sampling_rate;

   venc_ptr->encoder_capi.input_media_type.data_format.channel_type[0] = (uint16_t)PCM_CHANNEL_PRI_MIC;

   // Properties that are set at init
   capi_v2_prop_t init_props[] = {
         { CAPI_V2_INPUT_MEDIA_FORMAT,  { reinterpret_cast<int8_t *>(&venc_ptr->encoder_capi.input_media_type),sizeof(venc_ptr->encoder_capi.input_media_type), sizeof(venc_ptr->encoder_capi.input_media_type) }, port_info }
   };

   // setup init prop lists
   init_props_list.props_num = sizeof(init_props)/sizeof(capi_v2_prop_t);
   init_props_list.prop_ptr = init_props;

   //Call capi init.
   capi_result = venc_ptr->encoder_capi.common.module_ptr->vtbl_ptr->set_properties(venc_ptr->encoder_capi.common.module_ptr,&init_props_list);
   if(CAPI_V2_EOK != capi_result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_encoder_capi_set_media_fmt: media_type(0x%lx) failed to send init media fmt with error(%ld).",
            venc_ptr->encoder_capi.common.module_id, capi_result);
      return capi_v2_err_to_adsp_result(capi_result);
   }

   return ADSP_EOK;
}

ADSPResult venc_encoder_capi_get_data_threshold (venc_t* venc_ptr)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   if(NULL == venc_ptr->encoder_capi.common.module_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_encoder_capi_set_media_fmt: NULL module pointer error");
      return ADSP_EBADPARAM;
   }
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

   capi_result = venc_ptr->encoder_capi.common.module_ptr->vtbl_ptr->get_properties(venc_ptr->encoder_capi.common.module_ptr, &get_proplist);

   if (CAPI_V2_EOK != capi_result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_encoder_capi_get_data_threshold: get_prop failed for encoder capi media_type(0x%ld)",
            venc_ptr->voc_type);
      return capi_v2_err_to_adsp_result(capi_result);
   }

   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_encoder_capi_get_data_threshold: IO threshold (%ld) (%ld)",
         in_data_threshold.threshold_in_bytes, out_data_threshold.threshold_in_bytes);

   venc_ptr->encoder_capi.in_threshold_in_bytes = in_data_threshold.threshold_in_bytes;
   venc_ptr->encoder_capi.out_threshold_in_bytes = out_data_threshold.threshold_in_bytes;

   return ADSP_EOK;
}

capi_v2_err_t venc_encoder_capi_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr)
{
   capi_v2_err_t result = CAPI_V2_EOK;
   venc_encoder_capi_t* enc_capi_ptr = (venc_encoder_capi_t*)context_ptr;
   venc_t *venc_ptr = (venc_t *)enc_capi_ptr->common.svc_ptr;
   if(NULL == context_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_capi_cb_func: received bad pointer");
      return CAPI_V2_EFAILED;
   }
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_capi_cb_func invoked, event_id(%d), media_type(0x%lx)", (int)id, enc_capi_ptr->common.module_id);
   switch(id)
   {
      case CAPI_V2_EVENT_KPPS:
      {
         capi_v2_event_KPPS_t* kpps_ptr = (capi_v2_event_KPPS_t* )event_info_ptr->payload.data_ptr;
         enc_capi_ptr->common.kpps = kpps_ptr->KPPS;
         break;
      }
      case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
      {
         capi_v2_event_algorithmic_delay_t* delay_ptr = (capi_v2_event_algorithmic_delay_t* )event_info_ptr->payload.data_ptr;
         enc_capi_ptr->common.delay = delay_ptr->delay_in_us;
         break;
      }
      case CAPI_V2_EVENT_GET_LIBRARY_INSTANCE:
      {
         if(event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_get_library_instance_t))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc received insufficient size for get library instance, size(0x%lx)",event_info_ptr->payload.actual_data_len);
            return CAPI_V2_EBADPARAM;
         }

         capi_v2_event_get_library_instance_t* get_library_instance = (capi_v2_event_get_library_instance_t*)event_info_ptr->payload.data_ptr;;

         result = capi_v2_library_factory_get_instance(get_library_instance->id,&get_library_instance->ptr);
         if(CAPI_V2_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc failed to get library instance with lib_id(0x%x), payload_ptr(0x%p)",get_library_instance->id,get_library_instance->ptr);
            return result;
         }
         break;
      }
      case CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED:
      {
         break;
      }
      case CAPI_V2_EVENT_PORT_DATA_THRESHOLD_CHANGE:
      {
         if(sizeof(capi_v2_port_data_threshold_change_t) == event_info_ptr->payload.actual_data_len)
         {
            if(venc_ptr->voc_type != enc_capi_ptr->common.module_id)
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_capi_cb_func: unsupported vent ID is received by non-enc capi");
               return CAPI_V2_EUNSUPPORTED;
            }
            capi_v2_port_data_threshold_change_t *data_ptr = (capi_v2_port_data_threshold_change_t*)event_info_ptr->payload.data_ptr;
             if (!event_info_ptr->port_info.is_valid)
             {
                MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_encoder_capi_v2_cb_func: port index not valid");
                event_info_ptr->payload.actual_data_len = 0;
                return CAPI_V2_EBADPARAM;
             }
             if (event_info_ptr->port_info.is_input_port)
             {
                enc_capi_ptr->in_threshold_in_bytes = data_ptr->new_threshold_in_bytes;

                MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_encoder_capi_v2_cb_func: received updated in_threshold_in_bytes(%ld) for media_type(0x%lx)",
                      enc_capi_ptr->in_threshold_in_bytes, enc_capi_ptr->common.module_id);
             }
             else
             {
                enc_capi_ptr->out_threshold_in_bytes = data_ptr->new_threshold_in_bytes;

                MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_encoder_capi_v2_cb_func: received updated out_threshold_in_bytes(%ld) for media_type(0x%lx)",
                      enc_capi_ptr->out_threshold_in_bytes, enc_capi_ptr->common.module_id);
             }
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_encoder_capi_v2_cb_func: event_id(0x%lx): received bad param size (%ld)",
                  (uint32_t)CAPI_V2_EVENT_PORT_DATA_THRESHOLD_CHANGE, event_info_ptr->payload.actual_data_len);
            return CAPI_V2_ENEEDMORE;
         }
         break;
      }
      case CAPI_V2_EVENT_DATA_TO_DSP_SERVICE:
      {
         if(venc_ptr->voc_type != enc_capi_ptr->common.module_id)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_capi_cb_func: unsupported vent ID is received by non-enc capi");
            return CAPI_V2_EUNSUPPORTED;
         }

         capi_v2_event_data_to_dsp_service_t *payload = (capi_v2_event_data_to_dsp_service_t* )event_info_ptr->payload.data_ptr;

         switch(payload->param_id)
         {
            case VOC_EVT_OPERATING_MODE_UPDATE:
            {
               voc_evt_operating_mode_update_t *operating_mode_update_ptr = (voc_evt_operating_mode_update_t*)payload->payload.data_ptr;

               enc_capi_ptr->operating_mode.direction = operating_mode_update_ptr->direction;
               enc_capi_ptr->operating_mode.mode      = operating_mode_update_ptr->mode;

               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_capi_cb_func: operating mode changed to mode(0x%lx), media_type(0x%lx)",
                     enc_capi_ptr->operating_mode.mode , enc_capi_ptr->common.module_id);

               venc_ptr->vocoder_op_mode = enc_capi_ptr->operating_mode.mode;

               venc_send_mode_notification_v2(enc_capi_ptr->common.svc_ptr);
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
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Unsupported event %d", (int)id);
         return CAPI_V2_EBADPARAM;
      }
   }
   return result;
}

uint32_t voice_enc_get_default_sampling_rate( uint32_t voc_type)
{
    uint32_t samplingRate = VOICE_INVALID_SAMPLING_RATE;
    switch( voc_type)
    {
       case VSM_MEDIA_TYPE_13K_MODEM:
       case VSM_MEDIA_TYPE_EVRC_MODEM:
       case VSM_MEDIA_TYPE_4GV_NB_MODEM:
       case VSM_MEDIA_TYPE_EFR_MODEM:
       case VSM_MEDIA_TYPE_FR_MODEM:
       case VSM_MEDIA_TYPE_HR_MODEM:
       case VSM_MEDIA_TYPE_G711_ALAW:
       case VSM_MEDIA_TYPE_G711_MLAW:
       case VSM_MEDIA_TYPE_G711_ALAW_V2:
       case VSM_MEDIA_TYPE_G711_MLAW_V2:
       case VSM_MEDIA_TYPE_PCM_8_KHZ:
       case VSM_MEDIA_TYPE_G729AB:
       {
          samplingRate = VOICE_NB_SAMPLING_RATE;
          break;
       }
       case VSM_MEDIA_TYPE_AMR_NB_MODEM:
       case VSM_MEDIA_TYPE_AMR_NB_IF2:
       {
          samplingRate = VOICE_NB_SAMPLING_RATE;
          break;
       }
       case VSM_MEDIA_TYPE_4GV_WB_MODEM:
       case VSM_MEDIA_TYPE_4GV_NW_MODEM:
       case VSM_MEDIA_TYPE_4GV_NW:
       case VSM_MEDIA_TYPE_AMR_WB_MODEM:
       case VSM_MEDIA_TYPE_PCM_16_KHZ:
       case VSM_MEDIA_TYPE_G722:
       case VSM_MEDIA_TYPE_EAMR:
       case VSM_MEDIA_TYPE_EVRC_NW_2K:
       {
          samplingRate = VOICE_WB_SAMPLING_RATE;
          break;
       }
       case VSM_MEDIA_TYPE_PCM_32_KHZ:
       {
          samplingRate = VOICE_SWB_SAMPLING_RATE;
          break;
       }
       case VSM_MEDIA_TYPE_PCM_48_KHZ:
       {
          samplingRate = VOICE_FB_SAMPLING_RATE;
          break;
       }
       case VSM_MEDIA_TYPE_EVS:
       {
          samplingRate = VOICE_FB_SAMPLING_RATE;
          break;
       }
       default:
       {
          break;
       }
    }
    return samplingRate;
}

ADSPResult venc_capi_set_param(voice_capi_module_t *module_info_ptr, uint32_t param_id, void* param_payload_ptr, uint32_t param_size, uint32_t buf_size)
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
      param_data_buf.max_data_len = buf_size;

      capi_result = module_info_ptr->module_ptr->vtbl_ptr->set_param(module_info_ptr->module_ptr, param_id, &port_info, &param_data_buf);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_capi_set_param: failed. module_id(0x%x), param_id(0x%lx), result(0x%lx)",
               module_info_ptr->module_id, param_id , capi_result);
      }
   }

   return capi_v2_err_to_adsp_result(capi_result);
}

ADSPResult venc_capi_get_param(voice_capi_module_t *module_info_ptr, uint32_t param_id, void* param_payload_ptr, uint32_t buf_size, uint16_t *param_size)
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

      capi_result = module_info_ptr->module_ptr->vtbl_ptr->get_param(module_info_ptr->module_ptr, param_id, &port_info, &param_data_buf);

      *param_size = param_data_buf.actual_data_len;  //update size, in case of error it will be ignored
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_capi_get_param: failed. module_id(0x%x), param_id(0x%lx), result(0x%lx)",
               module_info_ptr->module_id, param_id , capi_result);
      }
   }
   else
   {
      *param_size  = 0;
   }

      return capi_v2_err_to_adsp_result(capi_result);
   }

static ADSPResult venc_stream_pp_capi_create(venc_t* venc_ptr)
{
   ADSPResult result = ADSP_EOK;

   // update module ids for stream pp modules
   venc_ptr->modules.limiter.module_id = VOICE_MODULE_TX_STREAM_LIMITER;
   venc_ptr->modules.cng.module_id = VOICE_MODULE_ENC_CNG;
   venc_ptr->modules.ctm_tx.ctm_tx_capi.module_id = VOICE_MODULE_CTM_TX;
   venc_ptr->modules.ltetty_tx.module_id = VOICE_MODULE_LTETTY_TX;
   venc_ptr->modules.dtmf_gen.module_id = VOICE_MODULE_DTMF_GEN;
   venc_ptr->modules.hpcm.module_id = VOICE_MODULE_HPCM_STREAM_TX;

   // check stream pp module is capi or not
   venc_ptr->modules.limiter.is_capi = venc_check_stream_pp_capi(venc_ptr->modules.limiter.module_id);
   venc_ptr->modules.cng.is_capi = venc_check_stream_pp_capi(venc_ptr->modules.cng.module_id);
   venc_ptr->modules.ctm_tx.ctm_tx_capi.is_capi = venc_check_stream_pp_capi(venc_ptr->modules.ctm_tx.ctm_tx_capi.module_id);
   venc_ptr->modules.ltetty_tx.is_capi = venc_check_stream_pp_capi(venc_ptr->modules.ltetty_tx.module_id);
   venc_ptr->modules.dtmf_gen.is_capi = venc_check_stream_pp_capi(venc_ptr->modules.dtmf_gen.module_id);
   venc_ptr->modules.hpcm.is_capi = venc_check_stream_pp_capi(venc_ptr->modules.hpcm.module_id);

   // update service pointer in capi instance structures.
   venc_ptr->modules.limiter.svc_ptr = venc_ptr;
   venc_ptr->modules.cng.svc_ptr = venc_ptr;
   venc_ptr->modules.ctm_tx.ctm_tx_capi.svc_ptr = venc_ptr;
   venc_ptr->modules.ltetty_tx.svc_ptr = venc_ptr;
   venc_ptr->modules.dtmf_gen.svc_ptr = venc_ptr;
   venc_ptr->modules.hpcm.svc_ptr = venc_ptr;


   // get amdb handle for stream pp capi modules
   result = venc_stream_pp_capi_get_amdb_handle(venc_ptr);
   if(ADSP_FAILED(result))
   {
      return result;
   }

   // get static properties of stream pp capi modules
   result = venc_stream_pp_capi_get_static_property(venc_ptr);
   if (ADSP_FAILED(result))
   {
      return result;
   }

   // query interface extension for stream pp capi modules. Ignore error
   venc_stream_pp_capi_query_intf_extn(venc_ptr);

   // allocate memory for stream pp capi modules
   venc_ptr->modules.total_size = venc_ptr->modules.limiter_size
                                  + venc_ptr->modules.cng_size
                                  + venc_ptr->modules.ctm_tx.ctm_tx_size
                                  + venc_ptr->modules.ltetty_tx_size
                                  + venc_ptr->modules.dtmf_gen_size
                                  + venc_ptr->modules.hpcm_size;

   venc_ptr->modules.start_addr_ptr = (int8_t*)qurt_elite_memory_malloc(venc_ptr->modules.total_size, QURT_ELITE_HEAP_DEFAULT);

   if (NULL == venc_ptr->modules.start_addr_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_stream_pp_capi_init: Failed to allocate memory for venc stream pp capi modules. total_size(%ld)",
            venc_ptr->modules.total_size);
      return ADSP_ENOMEMORY;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_stream_pp_capi_init: allocated total_size(%ld) for stream pp capi modules", venc_ptr->modules.total_size);

   // initialize stream capi modules.
   result = venc_stream_pp_capi_init(venc_ptr);

   return result;
}

static ADSPResult venc_stream_pp_capi_get_amdb_handle(venc_t* venc_ptr)
{
   ADSPResult result = ADSP_EOK;

   // Get amdb handle for venc stram pp capi modules
   // for now call get handle function sequentially for all the stream pp modules.
   // TODO: to update following routine with the loop once all the stream pp modules are updated to capi.

   // get amdb handle for capi module
   result |= voice_capi_get_amdb_handle(&venc_ptr->modules.limiter, &venc_ptr->modules.limiter_amdb);
   result |= voice_capi_get_amdb_handle(&venc_ptr->modules.cng, &venc_ptr->modules.cng_amdb);
   result |= voice_capi_get_amdb_handle(&venc_ptr->modules.ctm_tx.ctm_tx_capi, &venc_ptr->modules.ctm_tx.ctm_tx_amdb);
   result |= voice_capi_get_amdb_handle(&venc_ptr->modules.ltetty_tx, &venc_ptr->modules.ltetty_tx_amdb);
   result |= voice_capi_get_amdb_handle(&venc_ptr->modules.dtmf_gen, &venc_ptr->modules.dtmf_gen_amdb);
   result |= voice_capi_get_amdb_handle(&venc_ptr->modules.hpcm, &venc_ptr->modules.hpcm_amdb);

   return result;
}

static ADSPResult venc_stream_pp_capi_get_static_property(venc_t* venc_ptr)
{
   ADSPResult result = ADSP_EOK;

   // Get static properties for venc stram pp capi modules
   // for now call get static property function sequentially for all the stream pp modules.
   // TODO: to update following routine with the loop once all the stream pp modules are updated to capi.

   if(!venc_ptr->modules.limiter.is_virtual_stub)
   {
      venc_ptr->modules.limiter_size = voice_capi_get_static_prop (&venc_ptr->modules.limiter);
      if(0 == venc_ptr->modules.limiter_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               venc_ptr->modules.limiter_size, venc_ptr->modules.limiter.module_id);
         result |= ADSP_EFAILED;
      }
   }

   if(!venc_ptr->modules.cng.is_virtual_stub)
   {
      venc_ptr->modules.cng_size = voice_capi_get_static_prop (&venc_ptr->modules.cng);
      if(0 == venc_ptr->modules.cng_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               venc_ptr->modules.cng_size, venc_ptr->modules.cng.module_id);
         result |= ADSP_EFAILED;
      }
   }

   if(!venc_ptr->modules.ctm_tx.ctm_tx_capi.is_virtual_stub)
   {
      venc_ptr->modules.ctm_tx.ctm_tx_size = voice_capi_get_static_prop (&venc_ptr->modules.ctm_tx.ctm_tx_capi);
      if(0 == venc_ptr->modules.ctm_tx.ctm_tx_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               venc_ptr->modules.ctm_tx.ctm_tx_size, venc_ptr->modules.ctm_tx.ctm_tx_capi.module_id);
         result |= ADSP_EFAILED;
      }
   }

   if(!venc_ptr->modules.ltetty_tx.is_virtual_stub)
   {
      venc_ptr->modules.ltetty_tx_size = voice_capi_get_static_prop (&venc_ptr->modules.ltetty_tx);
      if(0 == venc_ptr->modules.ltetty_tx_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               venc_ptr->modules.ltetty_tx_size, venc_ptr->modules.ltetty_tx.module_id);
         result |= ADSP_EFAILED;
      }
   }

   if(!venc_ptr->modules.dtmf_gen.is_virtual_stub)
   {
      venc_ptr->modules.dtmf_gen_size = voice_capi_get_static_prop (&venc_ptr->modules.dtmf_gen);
      if(0 == venc_ptr->modules.dtmf_gen_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               venc_ptr->modules.dtmf_gen_size, venc_ptr->modules.dtmf_gen.module_id);
         result |= ADSP_EFAILED;
      }
   }

   if(!venc_ptr->modules.hpcm.is_virtual_stub)
   {
      venc_ptr->modules.hpcm_size = voice_capi_get_static_prop (&venc_ptr->modules.hpcm);
      if(0 == venc_ptr->modules.hpcm_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_get_static_property: voice_capi_get_static_prop returned size(%ld) for module_id(0x%lx)",
               venc_ptr->modules.hpcm_size, venc_ptr->modules.hpcm.module_id);
         result |= ADSP_EFAILED;
      }
   }

   return result;
}

static ADSPResult venc_stream_pp_capi_query_intf_extn(venc_t* venc_ptr)
{
   ADSPResult result = ADSP_EOK;

   // query interface extn for venc stream pp capi modules
   // for now query interface extn sequentially for all the stream pp modules.
   // TODO: to update following routine with the loop once all the stream pp modules are updated to capi.

   result |= voice_capi_query_intf_ext(&venc_ptr->modules.limiter);
   result |= voice_capi_query_intf_ext(&venc_ptr->modules.cng);
   result |= voice_capi_query_intf_ext(&venc_ptr->modules.ctm_tx.ctm_tx_capi);
   result |= voice_capi_query_intf_ext(&venc_ptr->modules.ltetty_tx);
   result |= voice_capi_query_intf_ext(&venc_ptr->modules.dtmf_gen);

   return result;
}

static ADSPResult venc_stream_pp_capi_init(venc_t* venc_ptr)
{
   ADSPResult result = ADSP_EOK;

   // populate init prop list for capi modules
   capi_v2_proplist_t init_props_list;

   capi_v2_port_info_t port_info = {FALSE, FALSE, 0};

   capi_v2_event_callback_info_t cb_info = { venc_stream_pp_capi_cb_func, NULL };
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

   if(venc_ptr->modules.start_addr_ptr)
   {
      venc_ptr->modules.usage_addr_ptr = venc_ptr->modules.start_addr_ptr;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_init: received NULL pointer");
      return ADSP_EFAILED;
   }

   if(venc_ptr->modules.limiter_size)
   {
      cb_info.event_context = &venc_ptr->modules.limiter;
      venc_ptr->modules.limiter.module_ptr = (capi_v2_t*) venc_ptr->modules.usage_addr_ptr;
      venc_stream_pp_capi_init_lib(venc_ptr, &venc_ptr->modules.limiter, &init_props_list);
      venc_ptr->modules.usage_addr_ptr += venc_ptr->modules.limiter_size;
   }

   if(venc_ptr->modules.cng_size)
   {
      cb_info.event_context = &venc_ptr->modules.cng;
      venc_ptr->modules.cng.module_ptr = (capi_v2_t*) venc_ptr->modules.usage_addr_ptr;
      venc_stream_pp_capi_init_lib(venc_ptr, &venc_ptr->modules.cng, &init_props_list);
      venc_ptr->modules.usage_addr_ptr += venc_ptr->modules.cng_size;
   }

   if(venc_ptr->modules.ctm_tx.ctm_tx_size)
   {
      cb_info.event_context = &venc_ptr->modules.ctm_tx.ctm_tx_capi;
      venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr = (capi_v2_t*) venc_ptr->modules.usage_addr_ptr;
      venc_stream_pp_capi_init_lib(venc_ptr, &venc_ptr->modules.ctm_tx.ctm_tx_capi, &init_props_list);
      venc_ptr->modules.usage_addr_ptr += venc_ptr->modules.ctm_tx.ctm_tx_size;
   }

   if(venc_ptr->modules.ltetty_tx_size)
   {
      cb_info.event_context = &venc_ptr->modules.ltetty_tx;
      venc_ptr->modules.ltetty_tx.module_ptr = (capi_v2_t*) venc_ptr->modules.usage_addr_ptr;
      venc_stream_pp_capi_init_lib(venc_ptr, &venc_ptr->modules.ltetty_tx, &init_props_list);
      venc_ptr->modules.usage_addr_ptr += venc_ptr->modules.ltetty_tx_size;
   }

   if(venc_ptr->modules.dtmf_gen_size)
   {
      cb_info.event_context = &venc_ptr->modules.dtmf_gen;
      venc_ptr->modules.dtmf_gen.module_ptr = (capi_v2_t*) venc_ptr->modules.usage_addr_ptr;
      venc_stream_pp_capi_init_lib(venc_ptr, &venc_ptr->modules.dtmf_gen, &init_props_list);
      venc_ptr->modules.usage_addr_ptr += venc_ptr->modules.dtmf_gen_size;
   }

   if(venc_ptr->modules.hpcm_size)
   {
      cb_info.event_context = &venc_ptr->modules.hpcm;
      venc_ptr->modules.hpcm.module_ptr = (capi_v2_t*) venc_ptr->modules.usage_addr_ptr;
      venc_stream_pp_capi_init_lib(venc_ptr, &venc_ptr->modules.hpcm, &init_props_list);
      venc_ptr->modules.usage_addr_ptr += venc_ptr->modules.hpcm_size;
   }

   // Sanity check to see the allocated memory was enough
   if ((uint32_t)(venc_ptr->modules.usage_addr_ptr - venc_ptr->modules.start_addr_ptr) <= venc_ptr->modules.total_size)
   {
      result = ADSP_EOK;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_stream_pp_capi_init: Error allocating memory: usage_size(%ld), allocated_size(%ld)",
            (uint32_t)(venc_ptr->modules.usage_addr_ptr - venc_ptr->modules.start_addr_ptr), venc_ptr->modules.total_size);
      result = ADSP_ENOMEMORY;
   }
   return result;
}

static ADSPResult venc_stream_pp_capi_init_lib(venc_t* venc_ptr, voice_capi_module_t *module_info_ptr, capi_v2_proplist_t *init_prop_list_ptr)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   capi_result = adsp_amdb_capi_v2_init_f((adsp_amdb_capi_v2_t*)module_info_ptr->amdb_handle_ptr, module_info_ptr->module_ptr, init_prop_list_ptr);

   if(CAPI_V2_FAILED(capi_result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_init_lib: failed for module_id(0x%lx), capi_result(%ld)",
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
               case FWK_EXTN_CNG:
               {
                  //ignore the error for fwk extension handler
                  voice_capi_cng_fwk_extn_handler(module_info_ptr);
                  break;
               }
               case FWK_EXTN_HPCM:
               {
                  //just updating the pointer.
                  //hanlding will be done after the module is created.
                  venc_ptr->modules.special_capis[HPCM] = module_info_ptr;
                  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_stream_pp_capi_init_lib: module(0x%lx) supports fwk_extn(0x%lx)",
                        module_info_ptr->module_id, module_info_ptr->fwk_extn_ptr[i].id);
                  break;
               }
               default:
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_stream_pp_capi_init_lib: received unsupported fwk_extn_id: 0x%lx",
                        module_info_ptr->fwk_extn_ptr[i].id);
                  break;
               }
            }
         }
      }
   }

   return ADSP_EOK;
}

static capi_v2_err_t venc_stream_pp_capi_cb_func(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_info_ptr)
{
   capi_v2_err_t result = CAPI_V2_EOK;
   voice_capi_module_t* module_info_ptr = (voice_capi_module_t*)context_ptr;
   venc_t *venc_ptr = (venc_t *)module_info_ptr->svc_ptr;
   if(NULL == context_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_stream_pp_capi_cb_func: received bad pointer");
      return CAPI_V2_EFAILED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_stream_pp_capi_cb_func:, event_id(%d), module_id(0x%lx)", (int)id, module_info_ptr->module_id);
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
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_stream_pp_capi_cb_func: received insufficient size for get library instance, size(0x%lx)",
                  event_info_ptr->payload.actual_data_len);
            return CAPI_V2_EBADPARAM;
         }

         capi_v2_event_get_library_instance_t* get_library_instance = (capi_v2_event_get_library_instance_t*)event_info_ptr->payload.data_ptr;;

         result = capi_v2_library_factory_get_instance(get_library_instance->id, &get_library_instance->ptr);
         if(CAPI_V2_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_stream_pp_capi_cb_func: failed to get library instance with lib_id(0x%x), payload_ptr(0x%p)",
                  get_library_instance->id, get_library_instance->ptr);
            return result;
         }
         break;
      }
      case CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED:
      {
         break;
      }
      case CAPI_V2_EVENT_PROCESS_STATE:
      {
         capi_v2_event_process_state_t *process_check_ptr = (capi_v2_event_process_state_t*)event_info_ptr->payload.data_ptr;
         module_info_ptr->is_enabled = (uint32_t)process_check_ptr->is_enabled;

         break;
      }
      case CAPI_V2_EVENT_DATA_TO_DSP_SERVICE:
      {

         if(event_info_ptr->payload.actual_data_len < sizeof(capi_v2_event_data_to_dsp_service_t))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_stream_pp_capi_cb_func: event_id(0x%lx): received bad param size (%ld)",
                  (uint32_t)CAPI_V2_EVENT_DATA_TO_DSP_SERVICE, event_info_ptr->payload.actual_data_len);
            return CAPI_V2_ENEEDMORE;
         }
         capi_v2_event_data_to_dsp_service_t *payload = (capi_v2_event_data_to_dsp_service_t* )event_info_ptr->payload.data_ptr;

         switch(payload->param_id)
         {
            case TTY_EVT_CHAR_DETECTED:
            {
               tty_char_t *tty_char_ptr = (tty_char_t*)payload->payload.data_ptr;

               // send tty tx charecter to uppar layer if character is received from ltetty tx module
               if(VOICE_MODULE_LTETTY_TX == module_info_ptr->module_id)
               {
                  // log tty tx character for ltetty tx
                  venc_log_ltetty_tx_char(venc_ptr, tty_char_ptr->tty_char);

                  result = venc_send_ltetty_tx_char_detected(venc_ptr, tty_char_ptr->tty_char);
                  if (ADSP_FAILED(result))
                  {
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_stream_pp_capi_cb_func: venc_send_ltetty_tx_char_detected failed");
                  }
               }
               if(VOICE_MODULE_CTM_TX == module_info_ptr->module_id)
               {
                  // log tty tx character for ctm tx
                  venc_log_ctm_tty_tx_char(venc_ptr, tty_char_ptr->tty_char);
               }
               break;
            }
            case TTY_EVT_CTM_CHAR_TRANSMITTED:
            {
               venc_ptr->tty.state_ptr->m_ctm_character_transmitted = TRUE;

               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: venc_stream_pp_capi_cb_func: TTY_EVT_CTM_CHAR_TRANSMITTED received.");
               break;
            }
            case FWK_EXTN_EVENT_ID_DTMF_GEN_END_DETECTED:
            {
               result = (capi_v2_err_t) venc_send_dtmf_gen_ended(venc_ptr);
               break;
            }
            case FWK_EXTN_EVENT_ID_HPCM_HOST_BUF_AVAILABLE:
            {
               venc_send_hpcm_buf_available_evt(venc_ptr, payload->payload.data_ptr);
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
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: venc_stream_pp_capi_cb_func: received Unsupported event %d", (int)id);
         return CAPI_V2_EBADPARAM;
      }
   }
   return result;
}

static ADSPResult venc_stream_pp_capi_set_input_media_fmt(venc_t *venc_ptr)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;
   // populate init prop list for capi modules
   capi_v2_proplist_t init_props_list;

   capi_v2_port_info_t port_info = {FALSE, FALSE, 0};

   //update input mediatype
   venc_ptr->modules.input_media_type.media_format.format_header.data_format = CAPI_V2_FIXED_POINT;

   venc_ptr->modules.input_media_type.data_format.bitstream_format  = CAPI_V2_DATA_FORMAT_INVALID_VAL;
   venc_ptr->modules.input_media_type.data_format.num_channels      = 1;
   venc_ptr->modules.input_media_type.data_format.bits_per_sample   = 16;
   venc_ptr->modules.input_media_type.data_format.data_is_signed    = TRUE;
   venc_ptr->modules.input_media_type.data_format.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
   venc_ptr->modules.input_media_type.data_format.q_factor          = 15;
   venc_ptr->modules.input_media_type.data_format.sampling_rate     = venc_ptr->sampling_rate;

   venc_ptr->modules.input_media_type.data_format.channel_type[0] = (uint16_t)PCM_CHANNEL_PRI_MIC;

   // Properties that are set at init
   capi_v2_prop_t init_props[] = {
         { CAPI_V2_INPUT_MEDIA_FORMAT,  { reinterpret_cast<int8_t *>(&venc_ptr->modules.input_media_type),sizeof(venc_ptr->modules.input_media_type), sizeof(venc_ptr->modules.input_media_type) }, port_info }
   };

   // setup init prop lists
   init_props_list.props_num = sizeof(init_props)/sizeof(capi_v2_prop_t);
   init_props_list.prop_ptr = init_props;

   // send updated input media format to limiter capi
   if(venc_ptr->modules.limiter.module_ptr)
   {
      capi_result = venc_ptr->modules.limiter.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.limiter.module_ptr, &init_props_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.limiter.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send updated input media format to CNG capi
   if(venc_ptr->modules.cng.module_ptr)
   {
      capi_result = venc_ptr->modules.cng.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.cng.module_ptr, &init_props_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.cng.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send updated input media format to CTM TX capi
   if(venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr)
   {
      capi_result = venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr, &init_props_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.ctm_tx.ctm_tx_capi.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send updated input media format to LTE TX  capi
   if(venc_ptr->modules.ltetty_tx.module_ptr)
   {
      capi_result = venc_ptr->modules.ltetty_tx.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.ltetty_tx.module_ptr, &init_props_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.ltetty_tx.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send updated input media format to DTMF_GEN  capi
   if(venc_ptr->modules.dtmf_gen.module_ptr)
   {
      capi_result = venc_ptr->modules.dtmf_gen.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.dtmf_gen.module_ptr, &init_props_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.dtmf_gen.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send updated input media format to HPCM  capi
   if(venc_ptr->modules.hpcm.module_ptr)
   {
      capi_result = venc_ptr->modules.hpcm.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.hpcm.module_ptr, &init_props_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_capi_set_input_media_fmt: failed to send input media fmt to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.hpcm.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   return ADSP_EOK;
}

static ADSPResult venc_stream_pp_capi_send_algo_reset(venc_t *venc_ptr)
{
   capi_v2_err_t capi_result = CAPI_V2_EOK;

   capi_v2_prop_t reset_prop[] = {
      { CAPI_V2_ALGORITHMIC_RESET, { NULL, 0, 0 }, {FALSE, FALSE, 0} },
   };
   capi_v2_proplist_t reset_prop_list = {sizeof(reset_prop)/sizeof(capi_v2_prop_t), reset_prop};

   // send algo_reset to limiter capi
   if(venc_ptr->modules.limiter.module_ptr)
   {
      capi_result = venc_ptr->modules.limiter.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.limiter.module_ptr, &reset_prop_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.limiter.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send algo_reset to cng capi
   if(venc_ptr->modules.cng.module_ptr)
   {
      capi_result = venc_ptr->modules.cng.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.cng.module_ptr, &reset_prop_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.cng.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send algo_reset to ctm tx capi
   if(venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr)
   {
      capi_result = venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr, &reset_prop_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.ctm_tx.ctm_tx_capi.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send algo_reset to ltetty tx capi
   if(venc_ptr->modules.ltetty_tx.module_ptr)
   {
      capi_result = venc_ptr->modules.ltetty_tx.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.ltetty_tx.module_ptr, &reset_prop_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.ltetty_tx.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send algo_reset to dtmf_gen capi
   if(venc_ptr->modules.dtmf_gen.module_ptr)
   {
      capi_result = venc_ptr->modules.dtmf_gen.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.dtmf_gen.module_ptr, &reset_prop_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.dtmf_gen.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   // send algo_reset to hpcm capi
   if(venc_ptr->modules.hpcm.module_ptr)
   {
      capi_result = venc_ptr->modules.hpcm.module_ptr->vtbl_ptr->set_properties(venc_ptr->modules.hpcm.module_ptr, &reset_prop_list);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_stream_pp_capi_send_algo_reset: failed to algo_reset to module_id(0x%lx), result(%ld)",
               venc_ptr->modules.hpcm.module_id, capi_result);
         return capi_v2_err_to_adsp_result(capi_result);
      }
   }

   return ADSP_EOK;
}

static ADSPResult venc_stream_pp_capi_destroy(venc_t *venc_ptr)
{
   // free memory allocated by stream capi modules
   venc_stream_pp_capi_end(venc_ptr);

   // free memory allocated for stream capi instances by service
   if (venc_ptr->modules.start_addr_ptr)
   {
      qurt_elite_memory_free (venc_ptr->modules.start_addr_ptr);
      venc_ptr->modules.start_addr_ptr = NULL;
      venc_ptr->modules.usage_addr_ptr = NULL;
   }

   // release amdb handle for stream capi modules
   if(venc_ptr->modules.limiter_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&venc_ptr->modules.limiter_amdb, 1);
      venc_ptr->modules.limiter_amdb.h.capi_v2_handle = NULL;
   }
   if(venc_ptr->modules.cng_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&venc_ptr->modules.cng_amdb, 1);
      venc_ptr->modules.cng_amdb.h.capi_v2_handle = NULL;
   }
   if(venc_ptr->modules.ctm_tx.ctm_tx_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&venc_ptr->modules.ctm_tx.ctm_tx_amdb, 1);
      venc_ptr->modules.ctm_tx.ctm_tx_amdb.h.capi_v2_handle = NULL;
   }
   if(venc_ptr->modules.ltetty_tx_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&venc_ptr->modules.ltetty_tx_amdb, 1);
      venc_ptr->modules.ltetty_tx_amdb.h.capi_v2_handle = NULL;
   }
   if(venc_ptr->modules.dtmf_gen_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&venc_ptr->modules.dtmf_gen_amdb, 1);
      venc_ptr->modules.dtmf_gen_amdb.h.capi_v2_handle = NULL;
   }
   if(venc_ptr->modules.hpcm_amdb.h.capi_v2_handle)
   {
      adsp_amdb_release_handles (&venc_ptr->modules.hpcm_amdb, 1);
      venc_ptr->modules.hpcm_amdb.h.capi_v2_handle = NULL;
   }

   // free memory allocated for fwk extns in stream capi modules
   if(venc_ptr->modules.limiter.fwk_extn_ptr)
   {
      qurt_elite_memory_free(venc_ptr->modules.limiter.fwk_extn_ptr);
      venc_ptr->modules.limiter.fwk_extn_ptr = NULL;
   }
   if(venc_ptr->modules.cng.fwk_extn_ptr)
   {
      qurt_elite_memory_free(venc_ptr->modules.cng.fwk_extn_ptr);
      venc_ptr->modules.cng.fwk_extn_ptr = NULL;
   }
   if(venc_ptr->modules.ctm_tx.ctm_tx_capi.fwk_extn_ptr)
   {
      qurt_elite_memory_free(venc_ptr->modules.ctm_tx.ctm_tx_capi.fwk_extn_ptr);
      venc_ptr->modules.ctm_tx.ctm_tx_capi.fwk_extn_ptr = NULL;
   }
   if(venc_ptr->modules.ltetty_tx.fwk_extn_ptr)
   {
      qurt_elite_memory_free(venc_ptr->modules.ltetty_tx.fwk_extn_ptr);
      venc_ptr->modules.ltetty_tx.fwk_extn_ptr = NULL;
   }
   if(venc_ptr->modules.dtmf_gen.fwk_extn_ptr)
   {
      qurt_elite_memory_free(venc_ptr->modules.dtmf_gen.fwk_extn_ptr);
      venc_ptr->modules.dtmf_gen.fwk_extn_ptr = NULL;
   }
   if(venc_ptr->modules.hpcm.fwk_extn_ptr)
   {
      qurt_elite_memory_free(venc_ptr->modules.hpcm.fwk_extn_ptr);
      venc_ptr->modules.hpcm.fwk_extn_ptr = NULL;
   }

   return ADSP_EOK;
}

static ADSPResult venc_stream_pp_capi_end(venc_t *venc_ptr)
{
   // free memory allocated by limiter capi
   if(venc_ptr->modules.limiter.module_ptr)
   {
      if(venc_ptr->modules.limiter.module_ptr->vtbl_ptr)
      {
         venc_ptr->modules.limiter.module_ptr->vtbl_ptr->end(venc_ptr->modules.limiter.module_ptr);
         venc_ptr->modules.limiter.module_ptr->vtbl_ptr = NULL;
      }
      venc_ptr->modules.limiter.module_ptr = NULL;
   }

   // free memory allocated by CNG capi
   if(venc_ptr->modules.cng.module_ptr)
   {
      if(venc_ptr->modules.cng.module_ptr->vtbl_ptr)
      {
         venc_ptr->modules.cng.module_ptr->vtbl_ptr->end(venc_ptr->modules.cng.module_ptr);
         venc_ptr->modules.cng.module_ptr->vtbl_ptr = NULL;
      }
      venc_ptr->modules.cng.module_ptr = NULL;
   }

   // free memory allocated by CTM TX capi
   if(venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr)
   {
      if(venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr->vtbl_ptr)
      {
         venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr->vtbl_ptr->end(venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr);
         venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr->vtbl_ptr = NULL;
      }
      venc_ptr->modules.ctm_tx.ctm_tx_capi.module_ptr = NULL;
   }

   // free memory allocated by LTETTY TX capi
   if(venc_ptr->modules.ltetty_tx.module_ptr)
   {
      if(venc_ptr->modules.ltetty_tx.module_ptr->vtbl_ptr)
      {
         venc_ptr->modules.ltetty_tx.module_ptr->vtbl_ptr->end(venc_ptr->modules.ltetty_tx.module_ptr);
         venc_ptr->modules.ltetty_tx.module_ptr->vtbl_ptr = NULL;
      }
      venc_ptr->modules.ltetty_tx.module_ptr = NULL;
   }

   // free memory allocated by DTMF GEN capi
   if(venc_ptr->modules.dtmf_gen.module_ptr)
   {
      if(venc_ptr->modules.dtmf_gen.module_ptr->vtbl_ptr)
      {
         venc_ptr->modules.dtmf_gen.module_ptr->vtbl_ptr->end(venc_ptr->modules.dtmf_gen.module_ptr);
         venc_ptr->modules.dtmf_gen.module_ptr->vtbl_ptr = NULL;
      }
      venc_ptr->modules.dtmf_gen.module_ptr = NULL;
   }

   // free memory allocated by HPCM capi
   if(venc_ptr->modules.hpcm.module_ptr)
   {
      if(venc_ptr->modules.hpcm.module_ptr->vtbl_ptr)
      {
         venc_ptr->modules.hpcm.module_ptr->vtbl_ptr->end(venc_ptr->modules.hpcm.module_ptr);
         venc_ptr->modules.hpcm.module_ptr->vtbl_ptr = NULL;
      }
      venc_ptr->modules.hpcm.module_ptr = NULL;
   }

   return ADSP_EOK;
}


static ADSPResult venc_log_ltetty_tx_char(venc_t *venc_ptr, uint32_t tty_tx_char)
{
   // log tty character if available
#if defined(__qdsp6__) && !defined(SIM)
   int8_t *bufptr[4] = { (int8_t *) &tty_tx_char, NULL, NULL, NULL };
   voice_log_buffer( bufptr,
         VOICE_LOG_CHAN_VSM_TX_OOBTTY_CHAR,
         (((venc_ptr->sampling_rate == 16000) << 3) | venc_ptr->session_num),
         qurt_elite_timer_get_time(),
         VOICE_LOG_DATA_FORMAT_PCM_MONO,
         (venc_ptr->sampling_rate),
         sizeof(int8_t),
         NULL);
#endif
   return ADSP_EOK;
}

static ADSPResult venc_log_ctm_tty_tx_char(venc_t *venc_ptr, uint32_t tty_tx_char)
{
   // log tty character if available
#if defined(__qdsp6__) && !defined(SIM)

   // log tty character if available
   int8_t *bufptr[4] = { (int8_t *) &tty_tx_char, NULL, NULL, NULL };
   voice_log_buffer( bufptr,
         VOICE_LOG_CHAN_VSM_TX_CTM_CHAR,
         (((VOICE_NB_SAMPLING_RATE != venc_ptr->sampling_rate) << 3) | venc_ptr->session_num),
         qurt_elite_timer_get_time(),
         VOICE_LOG_DATA_FORMAT_PCM_MONO,
         (uint32_t)(venc_ptr->sampling_rate),
         sizeof(int8_t),
         NULL);
#endif
   return ADSP_EOK;
}

ADSPResult voice_capi_get_amdb_handle(voice_capi_module_t *module_info_ptr, adsp_amdb_module_handle_info_t *amdb_handle_info_ptr)
{
   if ((NULL == module_info_ptr) || (NULL == amdb_handle_info_ptr))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: voice_capi_get_amdb_handle: received NULL pointer for module_info_ptr(0x%p) or amdb_handle_info_ptr(0x%p)",
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
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_capi_get_amdb_handle: failed to get AMDB handle for module_id(0x%lx)", module_info_ptr->module_id);
      return amdb_handle_info_ptr->result;
   }

   if ((CAPI_V2 != amdb_handle_info_ptr->interface_type) && (STUB != amdb_handle_info_ptr->interface_type))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: voice_capi_get_amdb_handle: AMDB handle for module_id(0x%lx) has interface type(%d), when expected type(%d).",
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
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_capi_get_amdb_handle: module_id(0x%lx) is virtually stubbed", module_info_ptr->module_id);
   }
   return ADSP_EOK;
}

static ADSPResult venc_stream_pp_capi_reinit(venc_t* venc_ptr)
{
   ADSPResult result = ADSP_EOK;

   // free memory allocated by stream capi modules
   venc_stream_pp_capi_end(venc_ptr);

   // clear stream capi input media type sampling rate
   venc_ptr->modules.input_media_type.data_format.sampling_rate = 0;

   // initialize stream capi modules again.
   result = venc_stream_pp_capi_init(venc_ptr);

   return result;
}

boolean venc_check_stream_pp_capi(uint32_t module_id)
{
   switch(module_id)
   {
      case VOICE_MODULE_TX_STREAM_LIMITER:
      case VOICE_MODULE_ENC_CNG:
      case VOICE_MODULE_CTM_TX:
      case VOICE_MODULE_LTETTY_TX:
      case VOICE_MODULE_DTMF_GEN:
      case VOICE_MODULE_HPCM_STREAM_TX:
      {
         return TRUE;
      }
      default:
      {
         return FALSE;
      }
   }
}

int32_t venc_send_hpcm_buf_available_evt(venc_t* venc_ptr, void* data_ptr)
{
   int32_t rc = ADSP_EOK;

   rc = elite_apr_if_alloc_send_event( venc_ptr->modules.apr_info_hpcm.apr_handle,
                                       venc_ptr->modules.apr_info_hpcm.self_addr,
                                       venc_ptr->modules.apr_info_hpcm.self_port,
                                       venc_ptr->modules.apr_info_hpcm.client_addr,
                                       venc_ptr->modules.apr_info_hpcm.client_port,
                                       0, /* token */
                                       VOICE_EVT_HOST_BUF_AVAILABLE_V2,
                                       data_ptr,
                                       sizeof(voice_evt_notify_host_pcm_buf_v2_t) );

   if (ADSP_FAILED(rc))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to create APR request for VOICE_EVT_HOST_BUF_AVAILABLE_V2  0x%08lx :session(%lx)",rc,venc_ptr->session_num);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VOICE_EVT_HOST_BUF_AVAILABLE_V2 sent to client, session(%lx)",venc_ptr->session_num);
   }
   return rc;
}

static ADSPResult venc_set_voc_param(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;
   venc_t* venc_ptr = (venc_t*)instance_ptr;
   elite_msg_custom_set_voc_param_type *payload_ptr = (elite_msg_custom_set_voc_param_type*)msg_ptr->pPayload;
   int8_t *param_data_payload_ptr = (int8_t*) (payload_ptr->payload_address);

   if(NULL == param_data_payload_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: venc_set_voc_param : NULL payload pointer error, session(%lx)", venc_ptr->session_num);
      result = ADSP_EBADPARAM;
      elite_svc_send_ack(msg_ptr, result);
      return result;
   }

   if(venc_ptr->is_encoder_capi)
   {
      voice_voc_param_data_t *voc_param_data_ptr = (voice_voc_param_data_t *)param_data_payload_ptr;
      param_data_payload_ptr +=  sizeof(voice_voc_param_data_t);

      result = venc_capi_set_param(&venc_ptr->encoder_capi.common, voc_param_data_ptr->param_id, param_data_payload_ptr,
                                voc_param_data_ptr->param_size, voc_param_data_ptr->param_size);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: venc_set_voc_param: Invalid mediatype session(%lx)",
            venc_ptr->session_num);
      result = ADSP_EUNSUPPORTED;
   }

   elite_svc_send_ack(msg_ptr, result);
   return result;
}
