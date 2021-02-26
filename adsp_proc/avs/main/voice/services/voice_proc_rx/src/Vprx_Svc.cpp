/*========================================================================

*//** @file Vprx_Svc.cpp

Copyright (c) 2009-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the code for Voice Proc Rx(VPRX) Dynamic service. The
VPRX dynamic service has one thread. It receives commands from Voice Proc
Manager (VPM). VPRX initializes the Pre-Processing algorithms and
processes the data for encoder stream service. Multiple VPRX can be
instantiated and each of them is identified by a session number provided
while creating VPRX.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/multimedia2/Audio/vocproc/VoiceDevRx/...

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/29/09   dc     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Elite.h"
#include "VoiceProcRx.h"
#include "Vprx_Modules.h"
#include "Vprx_Svc.h"
#include "VoiceMsgs.h"
#include "EliteMsg_Util.h"
#include "VoiceCmnUtils.h"
#include "adsp_vcmn_api.h"
#include "adsp_vparams_api.h"
#include "adsp_vparams_internal_api.h"
#include "adsp_media_fmt.h"
#include "adsp_vpm_api.h"
#include "voice_proc_mgr.h"

#if defined(__qdsp6__) && !defined(SIM)
#include "VoiceLoggingUtils.h"
#endif

extern "C" {
}

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/


// TODO: Remove when API updated
/**  Parameter for enabling internal loopback feature for a particular session.
     Fixed size of LOOPBACK_ENABLE_PARAM_BYTES = 4 bytes.
 */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define VPRX_DELIVERY_MISS_MASK       0x80000000
#define VPRX_RESYNC_MASK              0x40000000
#define VPRX_RESYNC_RESPONSE_MASK     0x20000000
#define VPRX_CMD_MASK                 0x10000000
#define VPRX_BUF_MASK                 0x08000000
#define VPRX_DATA_MASK                0x04000000
#define VPRX_GPQ_DATA_MASK            0x02000000
#define VPRX_DELIVERY_TICK_END_MASK   0x01000000
#define VPRX_RESPQ_MASK               0x00800000
#define VFR_CMD_DATA_MASK        ( VPRX_CMD_MASK | VPRX_DATA_MASK | VPRX_GPQ_DATA_MASK | VPRX_RESYNC_MASK | VPRX_DELIVERY_MISS_MASK | VPRX_RESYNC_RESPONSE_MASK)
#define VFR_CMD_BUF_DATA_MASK    ( VPRX_CMD_MASK | VPRX_BUF_MASK | VPRX_DATA_MASK | VPRX_GPQ_DATA_MASK | VPRX_RESYNC_MASK )
#define ALL_MASK                 ( VPRX_CMD_MASK | VPRX_BUF_MASK | VPRX_DATA_MASK | VPRX_GPQ_DATA_MASK | VPRX_RESYNC_MASK | VPRX_RESPQ_MASK  | VPRX_DELIVERY_MISS_MASK | VPRX_RESYNC_RESPONSE_MASK)

// VPRX processing time cannot exceed 20 ms
#define VPRX_MAX_PROCESSING_TIME_IN_US 20000

// maximum data messages in data queue.
static const uint32_t MAX_DATA_Q_ELEMENTS = 16;

// maximum number of commands expected ever in command queue.
static const uint32_t MAX_CMD_Q_ELEMENTS = 8;

// How many buffers in output buffer queue? May need to make this configurable...
static const uint32_t MAX_BUF_Q_ELEMENTS = 2;

// max number of responses
static const uint32_t MAX_RESP_Q_ELEMENTS = 2;

// Thread Name
static char_t aTHREAD_NAME[6] = {'V','P','R','$','\0'};

// Thread Stack size
extern const unsigned int VPRX_STACK_SIZE;


/* -----------------------------------------------------------------------
** Function prototypes
** ----------------------------------------------------------------------- */

// destructor
static void vprx_destroy(vprx_t* pVprx);

// Main work loop for service thread. Pulls msgs off of queues and processes them.
static ADSPResult vprx_thread(void* pInstance);

//wrapper for vprx thread to join another thread if running
static ADSPResult vprx_thread_entry(void* pInstance);

// message handler functions
static ADSPResult vprx_connect_dwn_stream_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_disconnect_dwn_stream_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_re_config_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_run_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_stop_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_set_param_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_get_param_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_destroy_yourself_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_custom_msg( void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_set_mute_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_set_timing_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_set_timingv2_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_set_timingv3_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_config_host_pcm(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_apr_cmd(void* pInstance, elite_msg_any_t* pMsg);

// state handler functions
static ADSPResult vprx_cmd_handler(void* pInstance);
static ADSPResult vprx_buf_handler(void* pInstance);
static ADSPResult vprx_data_handler(void* pInstance);
static ADSPResult vprx_process(void *pInstance);
static ADSPResult vprx_delivery_tick_end_handler(void *pInstance);
static ADSPResult vprx_response_handler(void *pInstance);
static ADSPResult vprx_resync_handler(void *pInstance);
static ADSPResult vprx_delivery_miss_response_handler(void *pInstance);
static ADSPResult vprx_resync_response_handler(void *pInstance);
static ADSPResult vprx_start_oneshot_timer(vprx_t* pVprx, uint16_t seq_offset);

// Memory management related function
static ADSPResult vprx_out_buf_allocator(vprx_t* pVprx, uint32_t out_buf_samples);

// Initialization and End functions
static ADSPResult vprx_init(vprx_t *pVprx, vprx_create_params_t *create_param_ptr);
static void vprx_end(vprx_t *pVprx);

// Media type related functions
static void vprx_set_media_type(vprx_t* pVprx,vprx_create_params_t *create_param_ptr);
static ADSPResult voice_check_media_t(elite_multi_channel_pcm_fmt_blk_t* pRefType, elite_multi_channel_pcm_fmt_blk_t* pMediaFormat);

// Error check functions
static void voice_result_check(ADSPResult result, uint32_t session_num);

// extract and accumulate drift info with av/hp timer
static void vprx_vtm_unsubscribe(void *pInstance,Vtm_SubUnsubMsg_t *unsubscribe_ptr, uint32_t mask);

static ADSPResult vprx_is_supported(uint32_t topology_id, uint32_t sampling_rate, uint32_t session_num, uint16_t afe_rx_port_id, uint32_t num_channels);
static ADSPResult vprx_vds_send_resync(void* pInstance);
static void vprx_calc_offsets(void* pInstance);
static ADSPResult vdec_calc_processing_time(uint32_t dsp_clock_mhz, uint32_t sampling_rate, uint32_t* processing_time, uint16_t vfr_mode);
static ADSPResult vprx_get_kpps_cmd (void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_get_delay_cmd (void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_unsupported_cmd (void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vprx_set_param_v3_cmd (void* pInstance, elite_msg_any_t* pMsg);

/* -----------------------------------------------------------------------
** Message handler
** ----------------------------------------------------------------------- */

static elite_svc_msg_handler_func pHandler[VOICEPROC_NUM_MSGS] =
{
   vprx_connect_dwn_stream_cmd,    // - VOICEPROC_CONNECT_DWN_STREAM_CMD
   vprx_disconnect_dwn_stream_cmd, // - VOICEPROC_DISCONNECT_DWN_STREAM_CMD
   vprx_re_config_cmd,             // - VOICEPROC_RECONFIG_CMD
   vprx_run_cmd,                   // - VOICEPROC_OPEN_CMD
   vprx_stop_cmd,                  // - VOICEPROC_CLOSE_PARAM_CMD
   vprx_set_param_cmd,             // - VOICEPROC_SET_PARAM_CMD
   vprx_get_param_cmd,             // - VOICEPROC_GET_PARAM_CMD
   vprx_destroy_yourself_cmd,      // - VOICEPROC_DESTROY_YOURSELF_CMD
   vprx_unsupported_cmd,           // - VOICEPROC_CONNECT_RX_PEER_DUMMY
   vprx_unsupported_cmd,           // - VOICEPROC_CONNECT_TX_PEER_DUMMY
   vprx_set_mute_cmd,              // - VOICEPROC_SET_MUTE_CMD
   vprx_config_host_pcm,           // - VOICEPROC_CONFIG_HOST_PCM
   vprx_set_timing_cmd,            // - VOICEPROC_SET_TIMING_CMD
   vprx_set_timingv2_cmd,          // - VOICEPROC_SET_TIMINGV2_CMD
   vprx_get_kpps_cmd,              // - VOICEPROC_GET_KPPS_CMD
   vprx_set_timingv3_cmd,          // - VOICEPROC_SET_TIMINGV3_CMD
   vprx_get_delay_cmd,             // - VOICEPROC_GET_DELAY_CMD
   vprx_set_param_v3_cmd           // - VOICEPROC_SET_PARAM_V3_CMD
};

static elite_svc_msg_handler_func pEliteHandler[] =
{
   vprx_custom_msg,           //0  - ELITE_CUSTOM_MSG
   vprx_run_cmd,              //1  - ELITE_CMD_START_SERVICE
   vprx_destroy_yourself_cmd, //2  - ELITE_CMD_DESTROY_SERVICE
   elite_svc_unsupported,     //3  - ELITE_CMD_CONNECT
   elite_svc_unsupported,     //4  - ELITE_CMD_DISCONNECT
   elite_svc_unsupported,     //5  - ELITE_CMD_PAUSE
   elite_svc_unsupported,     //6  - ELITE_CMD_RESUME
   elite_svc_unsupported,     //7  - ELITE_CMD_FLUSH
   vprx_set_param_cmd,        //8  - ELITE_CMD_SET_PARAM
   vprx_get_param_cmd,        //9  - ELITE_CMD_GET_PARAM
   elite_svc_unsupported,     //10 - ELITE_DATA_BUFFER
   elite_svc_unsupported,     //11 - ELITE_DATA_MEDIA_TYPE
   elite_svc_unsupported,     //12 - ELITE_DATA_EOS
   elite_svc_unsupported,     //13 - ELITE_DATA_RAW_BUFFER
   vprx_stop_cmd,             //14 - ELITE_CMD_STOP_SERVICE
   vprx_apr_cmd               //15 - ELITE_APR_PACKET_OPCODE
};

/* =======================================================================
**                          Function Definitions
** ======================================================================= */
static ADSPResult (*pVprxHandler[])(void *) =
{
   vprx_delivery_miss_response_handler,
   vprx_resync_handler,
   vprx_resync_response_handler,
   vprx_cmd_handler,
   vprx_buf_handler,
   vprx_data_handler,
   vprx_data_handler,   /* loopback gpQ */
   vprx_delivery_tick_end_handler, //should never get invoked
   vprx_response_handler //should never get invoked
};


ADSPResult vprx_create (void **handle,
      vprx_create_params_t *create_param_ptr)
{
   // static variable counter to make queue and thread name strings unique
   // limit to 16 bits so it will roll over and and only cost 4 characters in hexadecimal.
   // Queues in existence likely to have unique names, but not required...
   ADSPResult nResult;

   //store variables locally
   uint16_t afe_rx_port_id = create_param_ptr->afe_port_id;
   uint32_t topology_id    = create_param_ptr->topology_id;
   uint32_t sampling_rate  = create_param_ptr->sampling_rate;
   uint32_t session_num    = create_param_ptr->session_num;
   uint32_t shared_mem_client = create_param_ptr->shared_mem_client;

   MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_create enter Topology(%lx) samping_rate(%ld) num_channels(%ld) session(%lx)",
               topology_id, sampling_rate, create_param_ptr->num_channels, session_num);

   if(1 != create_param_ptr->num_channels)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx received num_channels(%ld). Setting it to 1", create_param_ptr->num_channels);
      create_param_ptr->num_channels = 1;
   }

   nResult = vprx_is_supported(topology_id, sampling_rate, session_num, afe_rx_port_id,create_param_ptr->num_channels);
   if (ADSP_FAILED(nResult))
   {
      MSG_4(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: vprx_create Invalid Topology(%lx) or Invalid Samping Rate(%lx) or num_channel(0x%ld), session(%lx)",
            topology_id, sampling_rate,create_param_ptr->num_channels,session_num);
      return nResult;
   }

   // allocate instance struct
   vprx_t *pVprx = (vprx_t*) qurt_elite_memory_malloc( sizeof(vprx_t), QURT_ELITE_HEAP_DEFAULT);
   if (!pVprx)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate memory for VPRX struct");
       return ADSP_ENOMEMORY;
   }

   // zero out all the fields.
   memset(pVprx, 0, sizeof(vprx_t));

   pVprx->shared_mem_client = shared_mem_client; // VPM/ADM client token for get virtual address and flush cache operations
   pVprx->session.session_num = session_num;
   if(qurt_elite_globalstate.pVoiceTimerCmdQ == NULL)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to get Vtm cmdQ session(%lx)",session_num);
      pVprx->timing.vtm_cmdq_ptr = NULL;
   }
   pVprx->timing.vtm_cmdq_ptr = qurt_elite_globalstate.pVoiceTimerCmdQ;
   pVprx->timing.vds_handle_ptr = vds_handle;
   pVprx->timing.vds_client_token = VDS_CREATE_TOKEN(pVprx->session.session_num, VDS_CLIENT_VPRX_DELIVERY);

   snprintf(pVprx->io.input.dataq_name, QURT_ELITE_DEFAULT_NAME_LEN, "Vprx_InQ%2x",(int)session_num);
   snprintf(pVprx->gp_dataq_name , QURT_ELITE_DEFAULT_NAME_LEN, "Vprx_GpqQ%2x" ,(int)session_num);
   snprintf(pVprx->cmdq_name     , QURT_ELITE_DEFAULT_NAME_LEN, "Vprx_CmdQ%2x" ,(int)session_num);
   snprintf(pVprx->io.output.bufq_name     , QURT_ELITE_DEFAULT_NAME_LEN, "Vprx_BufQ%2x" ,(int)session_num);
   snprintf(pVprx->respq_name     , QURT_ELITE_DEFAULT_NAME_LEN, "Vprx_RespQ%2x" ,(int)session_num);

   qurt_elite_channel_init(&pVprx->qurt_elite_channel);
   // Create the queues. Use non-blocking queues, since pselect is always used.
   // pselect blocks on any non-masked queue to receive, then can do non-blocking checks.
   // DataQ for inputs
   // gpQ far inputs
   // CmdQ for cmd inputs
   // bufq_ptr for output buffer
   /* Allocate memory for queues */
   pVprx->svc_handle.dataQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_DATA_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );
   pVprx->svc_handle.gpQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_DATA_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );
   pVprx->svc_handle.cmdQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_CMD_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );
   pVprx->respq_ptr = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_RESP_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );

   /* Allocate memory for signals */
   pVprx->delivery_miss_signal_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   pVprx->resync_response_signal_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);

   pVprx->timing.delivery_tick_vtm_struct.signal_ptr = NULL; // this is populated by signal returned from VDS
   pVprx->timing.delivery_tick_vtm_struct.signal_end_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   pVprx->timing.delivery_tick_vtm_struct.resync_signal_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == pVprx->svc_handle.dataQ
         || NULL == pVprx->svc_handle.gpQ
         || NULL == pVprx->svc_handle.cmdQ
         || NULL == pVprx->respq_ptr
         || NULL == pVprx->delivery_miss_signal_ptr
         || NULL == pVprx->resync_response_signal_ptr
         || NULL == pVprx->timing.delivery_tick_vtm_struct.resync_signal_ptr
         || NULL == pVprx->timing.delivery_tick_vtm_struct.signal_end_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to allocate memory for VpRx Queues/Signals session(%#lx)",session_num);
      vprx_destroy(pVprx);
      return ADSP_ENOMEMORY;
   }

   if(   ADSP_FAILED(nResult = qurt_elite_queue_init(pVprx->io.input.dataq_name, MAX_DATA_Q_ELEMENTS, pVprx->svc_handle.dataQ))
         || ADSP_FAILED(nResult = qurt_elite_queue_init(pVprx->gp_dataq_name, MAX_DATA_Q_ELEMENTS, pVprx->svc_handle.gpQ))
         || ADSP_FAILED(nResult = qurt_elite_queue_init(pVprx->cmdq_name, MAX_CMD_Q_ELEMENTS, pVprx->svc_handle.cmdQ))
         || ADSP_FAILED(nResult = qurt_elite_queue_init(pVprx->respq_name, MAX_RESP_Q_ELEMENTS, pVprx->respq_ptr))
         || ADSP_FAILED(nResult = qurt_elite_channel_addq(&pVprx->qurt_elite_channel, (pVprx->svc_handle.cmdQ), VPRX_CMD_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_addq(&pVprx->qurt_elite_channel, (pVprx->svc_handle.dataQ), VPRX_DATA_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_addq(&pVprx->qurt_elite_channel, (pVprx->svc_handle.gpQ), VPRX_GPQ_DATA_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_addq(&pVprx->qurt_elite_channel, (pVprx->respq_ptr), VPRX_RESPQ_MASK))
         || ADSP_FAILED(nResult = qurt_elite_signal_init(pVprx->timing.delivery_tick_vtm_struct.signal_end_ptr))
         || ADSP_FAILED(nResult = qurt_elite_signal_init(pVprx->timing.delivery_tick_vtm_struct.resync_signal_ptr))
         || ADSP_FAILED(nResult = qurt_elite_signal_init(pVprx->delivery_miss_signal_ptr))
         || ADSP_FAILED(nResult = qurt_elite_signal_init(pVprx->resync_response_signal_ptr))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVprx->qurt_elite_channel, (pVprx->timing.delivery_tick_vtm_struct.resync_signal_ptr), VPRX_RESYNC_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVprx->qurt_elite_channel, (pVprx->timing.delivery_tick_vtm_struct.signal_end_ptr), VPRX_DELIVERY_TICK_END_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVprx->qurt_elite_channel, (pVprx->delivery_miss_signal_ptr), VPRX_DELIVERY_MISS_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVprx->qurt_elite_channel, (pVprx->resync_response_signal_ptr), VPRX_RESYNC_RESPONSE_MASK))
     )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed create Vprx MsgQs !! session(%lx)",session_num);
      vprx_destroy(pVprx);
      return nResult;
   }
   // Save shared object instantiation function pointers.

   nResult = vprx_init(pVprx, create_param_ptr);
   if (ADSP_FAILED(nResult))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed Init Vprx !! session(%lx)",session_num);
      vprx_destroy(pVprx);
      return nResult;
   }

   *handle = &(pVprx->svc_handle);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_create end %d session(%lx)",nResult,session_num);
   return ADSP_EOK;
}

// destructor
static void vprx_destroy (vprx_t* pVprx)
{
   if (pVprx)
   {
      uint32_t session_num = pVprx->session.session_num;

      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_destroy begin session(%x)",(int)session_num);
      // Free memory for vprx processing and out buffers
      vprx_end(pVprx);

      // call utility function to destroy data Q
      if (NULL != pVprx->svc_handle.dataQ)
      {
         elite_svc_deinit_data_queue(pVprx->svc_handle.dataQ);
         qurt_elite_memory_free(pVprx->svc_handle.dataQ);
         pVprx->svc_handle.dataQ = NULL;
      }

      if (NULL != pVprx->svc_handle.gpQ)
      {
         elite_svc_deinit_data_queue(pVprx->svc_handle.gpQ);
         qurt_elite_memory_free(pVprx->svc_handle.gpQ);
         pVprx->svc_handle.gpQ = NULL;
      }

      // call utility function to destroy cmd Q
      if (NULL != pVprx->svc_handle.cmdQ)
      {
         elite_svc_deinit_cmd_queue(pVprx->svc_handle.cmdQ);
         qurt_elite_memory_free(pVprx->svc_handle.cmdQ);
         pVprx->svc_handle.cmdQ = NULL;
      }

      if (NULL != pVprx->respq_ptr)
      {
         elite_svc_deinit_cmd_queue(pVprx->respq_ptr);
         qurt_elite_memory_free(pVprx->respq_ptr);
         pVprx->respq_ptr = NULL;
      }

      // call utility function to destroy signal
      if (NULL != pVprx->delivery_miss_signal_ptr)
      {
         qurt_elite_signal_deinit(pVprx->delivery_miss_signal_ptr);
         qurt_elite_memory_free(pVprx->delivery_miss_signal_ptr);
         pVprx->delivery_miss_signal_ptr = NULL;
      }

      if (NULL != pVprx->resync_response_signal_ptr)
      {
         qurt_elite_signal_deinit(pVprx->resync_response_signal_ptr);
         qurt_elite_memory_free(pVprx->resync_response_signal_ptr);
         pVprx->resync_response_signal_ptr = NULL;
      }
      if (NULL != pVprx->timing.delivery_tick_vtm_struct.signal_end_ptr)
      {
         qurt_elite_signal_deinit(pVprx->timing.delivery_tick_vtm_struct.signal_end_ptr);
         qurt_elite_memory_free(pVprx->timing.delivery_tick_vtm_struct.signal_end_ptr);
         pVprx->timing.delivery_tick_vtm_struct.signal_end_ptr = NULL;
      }
      if (NULL != pVprx->timing.delivery_tick_vtm_struct.resync_signal_ptr)
      {
         qurt_elite_signal_deinit(pVprx->timing.delivery_tick_vtm_struct.resync_signal_ptr);
         qurt_elite_memory_free(pVprx->timing.delivery_tick_vtm_struct.resync_signal_ptr);
         pVprx->timing.delivery_tick_vtm_struct.resync_signal_ptr = NULL;
      }

      // Destroy the qurt_elite_channel
      qurt_elite_channel_destroy(&pVprx->qurt_elite_channel);

      qurt_elite_memory_free (pVprx);

      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_destroy end session(%lx)",session_num);
   }
   pVprx = NULL;
}

/*
 * The input to the first module will be the INPUT_STRM_BUF_1. If  the module is inplace,
 * its output will also be the same buffer. The input and output will be set to the same
 * value till a non-inplace module is found.
 *
 * For the first non-inplace module, the output buffer will be  set to the  INPUT_STRM_BUF_2
 * buffer. This buffer will be used as input and  output till the next non-inplace module.
 * The output buffer for the next non-inplace module will be set  to INPUT_STRM_BUF_1. This
 * procedure will continue, with the buffers being  switched between tmp1 and tmp2 at each
 * non-inplace module UNTIL we reach the end.
 *
 * Depending on the final module output stream buffer, it will be written to output circular buffer.
 */
ADSPResult vprx_update_io_stream_data(vprx_t *pVprx, uint32_t start_module_index)
{
   ADSPResult result           = ADSP_EOK;

   voice_capi_module_t *curr_module_ptr;
   uint32_t i;
   voice_capi_io_stream_data_idx_t currentIdx = INPUT_STRM_BUF_1;
   curr_module_ptr = &pVprx->modules.modules_array[start_module_index];

   if(start_module_index != 0)
   {
      currentIdx = curr_module_ptr->input_buf_index;
   }

   // Loop through the modules and set the input and output buffers.
   for (i = start_module_index; i < pVprx->modules.num_modules; i++)
   {
      curr_module_ptr->input_buf_index = currentIdx;
      if( (TRUE == curr_module_ptr->is_enabled) && (FALSE == curr_module_ptr->is_in_place) )
      {
         switch(currentIdx)
         {
            case INPUT_STRM_BUF_1:
            {
               currentIdx = INPUT_STRM_BUF_2;
            }
            break;
            case INPUT_STRM_BUF_2:
            {
               currentIdx = INPUT_STRM_BUF_1;
            }
            break;
            default:
            {
               MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Error!! invalid currentIdx(%d), session(%lx)",(int)currentIdx, pVprx->session.session_num);
            }
         }
      }
      curr_module_ptr->output_buf_index = currentIdx;

      //dbg message
      MSG_6(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vprx_update_io_stream_buf(): moduleId(%lx), idx(%ld),is_inplace(%ld), enable(%lx), input_index(%ld), output_index(%ld)",
            curr_module_ptr->module_id, curr_module_ptr->module_index,curr_module_ptr->is_in_place, curr_module_ptr->is_enabled,
            curr_module_ptr->input_buf_index, curr_module_ptr->output_buf_index);

      curr_module_ptr++;
   }
   return result;

}

static ADSPResult vprx_init(vprx_t *pVprx, vprx_create_params_t *create_param_ptr)
{
   ADSPResult nResult             = ADSP_EOK;
   uint32_t in_circ_samples;

   uint16_t afe_rx_port_id = create_param_ptr->afe_port_id;
   uint32_t topology_id    = create_param_ptr->topology_id;
   uint32_t sampling_rate  = create_param_ptr->sampling_rate;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_init begin session(%x)",pVprx->session.session_num);
   pVprx->session.in_nb_sampling_rate_factor           = sampling_rate/VOICE_NB_SAMPLING_RATE;                        // conversion factor from NB sampling rate
   pVprx->io.input.frame_samples                    = FRAME_SAMPLES * pVprx->session.in_nb_sampling_rate_factor;   // 20ms
   pVprx->io.output.min_frame_samples               = DMA_SAMPLES * pVprx->session.in_nb_sampling_rate_factor;     // 1ms
   pVprx->io.min_process_frame_samples              = FRAME_SAMPLES * pVprx->session.in_nb_sampling_rate_factor;   // 20ms
   in_circ_samples                                  = (FRAME_SAMPLES + DMA_SAMPLES) * pVprx->session.in_nb_sampling_rate_factor; // For sample slipping
   pVprx->afe_info.afe_rx_port_id                   = afe_rx_port_id;
   pVprx->session.in_sampling_rate                  = sampling_rate;                 // sampling rate
   pVprx->session.topology_id                       = topology_id;                   // topology
   pVprx->wait_mask                                 = VFR_CMD_DATA_MASK;             // Always
   pVprx->svc_handle.unSvcId                        = ELITE_VOICEPROCRX_SVCID;
   pVprx->process_data_flag                         = 0;                             // Dont start processing wait for the run command
   pVprx->io.output.send_mediatype_flag             = 1;                             // default - 0, set to 1 at Run
   pVprx->mute                                      = VOICE_UNMUTE;                    // don't cache this
   pVprx->session.vfr_mode                          = VFR_NONE;                      // VFR mode is disabled by default
   pVprx->timing.vsid                               = 0;
   pVprx->timing.use_new_timing_flag                = FALSE;                         // default old timing
   pVprx->timing.timing_ver                         = VFR_CLIENT_INFO_VER_1;         // default to version 1 of timing

   //NOTE: don't reset peer.  This would implicitly detach during reinit.  Set to NULL at create only.  [pVprx->downstream_peer_ptr       = NULL;]

   voice_cmn_time_profile_init( &pVprx->profiler);

   pVprx->io.output.frame_samples = pVprx->io.input.frame_samples; // Default output buffer size is same as input buffer size

   vprx_set_media_type(pVprx, create_param_ptr);

   //set module list locked flag and media fmt propagation flag
   pVprx->modules.module_list_locked = FALSE;
   pVprx->modules.is_prop_media_fmt  = FALSE;

   //retrieve topology based information if required
   if(FALSE == pVprx->reconfig_state.retain_amdb_handle)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: create command: querying topo db");
      nResult = voice_update_topo_information(pVprx->session.topology_id,
            &pVprx->modules.topo_entry_ptr,
            &pVprx->modules.topo_entry_size,
            &pVprx->modules.topo_entry_handle);
      if (ADSP_FAILED(nResult))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid Vprx topology (%#lx) session(%#lx)",pVprx->session.topology_id,pVprx->session.session_num);
         return nResult;
      }
      //Update topo parameters
      pVprx->modules.num_modules = pVprx->modules.topo_entry_ptr->topo_def.num_modules;
      pVprx->modules.modules_list = &pVprx->modules.topo_entry_ptr->module_info[0];
      //Add PP modules - SOFT_MUTE and Sample slip
      pVprx->modules.num_modules += VPRX_NUM_PP_MODULES;

      pVprx->modules.modules_array = (voice_capi_module_t*)qurt_elite_memory_malloc(sizeof(voice_capi_module_t) * pVprx->modules.num_modules, QURT_ELITE_HEAP_DEFAULT);
      if(NULL == pVprx->modules.modules_array)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: failed to allocate memory for module instances. session(%#lx)",pVprx->session.session_num);
         return ADSP_ENOMEMORY;
      }
      memset(pVprx->modules.modules_array,0,sizeof(voice_capi_module_t) * pVprx->modules.num_modules);
   }

   pVprx->memory.module_size    = (uint32_t*)qurt_elite_memory_malloc(sizeof(uint32_t) * pVprx->modules.num_modules, QURT_ELITE_HEAP_DEFAULT);
   pVprx->memory.guard_band_ptrs    = (int8_t *)qurt_elite_memory_malloc(sizeof(uint64_t *) * pVprx->modules.num_modules, QURT_ELITE_HEAP_DEFAULT); //pointer points to array of guardband pointers
   if( (NULL == pVprx->modules.modules_array) || (NULL == pVprx->memory.module_size) || (NULL == pVprx->memory.guard_band_ptrs) )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: failed to allocate memory for module instances. session(%#lx)",pVprx->session.session_num);
      return ADSP_ENOMEMORY;
   }
   memset(pVprx->memory.module_size,0,sizeof(uint32_t) * pVprx->modules.num_modules);
   memset(pVprx->memory.guard_band_ptrs,0,sizeof(uint64_t *) * pVprx->modules.num_modules);

   // Allocate and manage memory required for Circular buffers and Modules
   nResult = vprx_allocate_mem(pVprx,in_circ_samples);
   if (ADSP_FAILED(nResult))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Allocate Vprx Internal Memory !! session(%lx)",pVprx->session.session_num);
      return nResult;
   }

   //update stream data indices
   nResult = vprx_update_io_stream_data(pVprx,0);
   if (ADSP_FAILED(nResult))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to assign io stream indices !! session(%#lx)",pVprx->session.session_num);
      return nResult;
   }

   //since all the module initialization is done set the module_list_locked
   pVprx->modules.module_list_locked = TRUE;

   // Initialize input circular buffer
   voice_circbuf_init(&(pVprx->io.input.circ_struct), (int8_t*)(pVprx->io.input.circ_buf_ptr),
         (int32_t)in_circ_samples,MONO_VOICE,
         (int32_t)16 /*bitperchannel*/);

   /* Allocate memory for queue */
   pVprx->io.output.bufq_ptr = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_BUF_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );

   if ( NULL == pVprx->io.output.bufq_ptr )
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: failed to allocate memory for VPRX bufq. session(%#lx)",pVprx->session.session_num);
       return ADSP_ENOMEMORY;
   }
   // Create BufQ and add to channel
   if(  ADSP_FAILED(nResult = qurt_elite_queue_init(pVprx->io.output.bufq_name, MAX_BUF_Q_ELEMENTS, pVprx->io.output.bufq_ptr))
         || ADSP_FAILED(nResult = qurt_elite_channel_addq(&pVprx->qurt_elite_channel, (pVprx->io.output.bufq_ptr), VPRX_BUF_MASK))
     )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed create Vprx Out buf queue!! session(%lx)",pVprx->session.session_num);
      return nResult;
   }

   //Allocate and queue up the output buffers.
   nResult = vprx_out_buf_allocator(pVprx,pVprx->io.output.circ_struct.circ_buf[0].max_samples);
   if (ADSP_FAILED(nResult))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Memory Alloc OutBuf!! session(%lx)",pVprx->session.session_num);
      return nResult;
   }

   // Spin thread either for create or for reinit to new topo, both should have stack_changed as true
    if(pVprx->session.stack_changed)
    {
       pVprx->session.stack_changed = FALSE;
       pVprx->session.cached_tid = pVprx->svc_handle.threadId;
       aTHREAD_NAME[3] = ((pVprx->session.session_num + 48) & 0xff); // int32_t to ascii conversion
       MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx stack :%ld", pVprx->session.stack_size);

       pVprx->reconfig_state.another_thread_launched = FALSE;
       //if reconfig is pending, then there would be another thread running. set the flag
       if(pVprx->reconfig_state.reconfig_pending)
       {
          MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Reconfig cmd. set join thread flag, session(%lx)",pVprx->session.session_num);
          pVprx->reconfig_state.another_thread_launched = TRUE;
       }

       if (ADSP_FAILED(nResult = qurt_elite_thread_launch(&(pVprx->svc_handle.threadId), aTHREAD_NAME, NULL,
             pVprx->session.stack_size, elite_get_thrd_prio(ELITETHREAD_DYNA_VOICE_PROC_RX_PRIO_ID), vprx_thread_entry, (void*)pVprx, QURT_ELITE_HEAP_DEFAULT)))
       {
          MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed create Vprx thread!! session(%lx)", pVprx->session.session_num);
          //update thread lauch info
          pVprx->reconfig_state.another_thread_launched = FALSE;
          pVprx->fatal_state = TRUE;
          return nResult;
       }
       MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Vprx new thread launched, id1 %d, id2 %d", (int)pVprx->svc_handle.threadId, (int)qurt_thread_get_id());
    }
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_init begin nResult=%x session(%x)",nResult,pVprx->session.session_num);
   return nResult;
}

static void vprx_end(vprx_t *pVprx)
{
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_end begin session(%x)",pVprx->session.session_num);
   // Destroy Post-proc modules
   vprx_modules_end(pVprx);

   /* guarantee that any queued data buffers are returned, so AFE can free them properly for Reinit process */
   elite_msg_any_t   dataQMsg;
   ADSPResult nResult;
   do
   {
      // Non-blocking MQ receive
      nResult = qurt_elite_queue_pop_front(pVprx->svc_handle.dataQ, (uint64_t*)&dataQMsg);
      // return the buffer to its rightful q.
      if (ADSP_EOK == nResult)
      {
         elite_msg_return_payload_buffer(&dataQMsg);
      }
   } while (ADSP_EOK == nResult);

   do
   {
      // Non-blocking MQ receive
      nResult = qurt_elite_queue_pop_front(pVprx->svc_handle.gpQ, (uint64_t*)&dataQMsg);
      // return the buffer to its rightful q.
      if (ADSP_EOK == nResult)
      {
         elite_msg_return_payload_buffer(&dataQMsg);
      }
   } while (ADSP_EOK == nResult);

   // call utility function to destroy buf Q
   if(pVprx->io.output.bufq_ptr)
   {
      elite_svc_deinit_buf_queue(pVprx->io.output.bufq_ptr, pVprx->io.output.bufs_allocated);
      qurt_elite_memory_free(pVprx->io.output.bufq_ptr);
      pVprx->io.output.bufq_ptr = NULL;
   }
   pVprx->io.output.bufs_allocated = 0;

   if( FALSE == pVprx->reconfig_state.retain_amdb_handle )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Free module array and topo handle, session(%lx)",pVprx->session.session_num);

      if(pVprx->modules.modules_array)
      {
         qurt_elite_memory_free(pVprx->modules.modules_array);
         pVprx->modules.modules_array = NULL;
      }

      if(ELITE_CMN_TOPO_DB_INVALID_HANDLE != pVprx->modules.topo_entry_handle )
      {
         voice_release_topo_information(pVprx->modules.topo_entry_handle);
         pVprx->modules.topo_entry_handle = ELITE_CMN_TOPO_DB_INVALID_HANDLE;
      }
      pVprx->modules.num_modules = 0; //set to zero as the modules array is freed
   }
   if(pVprx->memory.module_size)
   {
      qurt_elite_memory_free(pVprx->memory.module_size);
      pVprx->memory.module_size = NULL;
   }
   if(pVprx->memory.guard_band_ptrs)
   {
      qurt_elite_memory_free(pVprx->memory.guard_band_ptrs);
      pVprx->memory.guard_band_ptrs = NULL;
   }


   memset(pVprx->io_buf,0,sizeof(pVprx->io_buf));
   memset(pVprx->io_stream_data,0,sizeof(pVprx->io_stream_data));

   // Free memory managers memory
   vprx_allocate_mem_free(pVprx);

   // print out profiling stats
   if( pVprx->profiler.num_samples > 0)
   {
      MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_end session(%x) topo(%x): profiler microsec: num_samples(%d) max_time(%d) min_time(%d) avg_time(%d) max_cycles(%d)",
            (int)pVprx->session.session_num,
            (int)pVprx->session.topology_id,
            (int)pVprx->profiler.num_samples,
            (int)pVprx->profiler.max_time,
            (int)pVprx->profiler.min_time,
            (int)(pVprx->profiler.total_time / pVprx->profiler.num_samples),
            (int) pVprx->profiler.max_cycles  );
   }


   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_end end session(%x)",pVprx->session.session_num);
}

static ADSPResult vprx_thread_entry(void* pInstance)
{
      vprx_t* pVprx = (vprx_t*)pInstance;
      int join_status;
      ADSPResult result;
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vprx_thread_entry(): VPRx Current context: %d", (int)qurt_thread_get_id());
      if(pVprx->reconfig_state.another_thread_launched)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vprx_thread_entry(): for Reconfig, waiting for join, session(%lx)", pVprx->session.session_num);
         // a new thread was launched, so join the old thread first
         qurt_elite_thread_join(pVprx->session.cached_tid, &join_status);
         // Usually it should return immediately, once it returns, reenter the workloop
         //Can safely clear flag once join returns, since now there is only one thread
         pVprx->reconfig_state.another_thread_launched = FALSE;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vprx_thread_entry(): for Create, session(%lx)",pVprx->session.session_num);
      }
      // call into workloop
      result = vprx_thread(pInstance);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vprx_thread_entry(): exiting, session(%lx), Current Tid(%d)", pVprx->session.session_num,qurt_thread_get_id());
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

static ADSPResult vprx_thread(void* pInstance)
{
   ADSPResult nResult;                    // general nResult value
   vprx_t *pVprx = (vprx_t*)pInstance;    // instance structure
   uint32_t session_num = pVprx->session.session_num;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_thread(): begin session(%lx)",session_num);
   // Enter forever loop
   for(;;)
   {
      // ***************** Wait for the MASK
      // block on any one or more of selected queues to get a msg
      pVprx->rcvd_mask = qurt_elite_channel_wait(&pVprx->qurt_elite_channel, pVprx->wait_mask);

      while(pVprx->rcvd_mask)
      {
         pVprx->bit_pos = voice_get_signal_pos(pVprx->rcvd_mask);

         // De queue and signal clear done in the handler functions.
         nResult = pVprxHandler[pVprx->bit_pos](pVprx);

         if(nResult != ADSP_EOK)
         {
            if (ADSP_ETERMINATED == nResult)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_thread(): end session(%lx)",session_num);
               return ADSP_EOK;
            }
            MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: vprx_thread(): Handler Error bit_pos=%ld, res=%d, session(%lx)",pVprx->bit_pos, nResult, session_num );
         }
         if(pVprx->reconfig_state.another_thread_launched)
         {
            // the old thread will come here, and exit at this point
            MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_thread():Another thread launched. exiting current Tid(%ld) session(%lx), cachedTid(%ld)",qurt_thread_get_id(),pVprx->session.session_num,pVprx->session.cached_tid);
            return ADSP_EOK;
         }

         pVprx->rcvd_mask = qurt_elite_channel_poll(&pVprx->qurt_elite_channel, pVprx->wait_mask);
      } // end of while loop
   } // end of for loop
   return 0;
}


static ADSPResult vprx_cmd_handler(void *pInstance)
{
   // if cmd mask, vprx_process Commands
   ADSPResult nResult = ADSP_EFAILED;
   vprx_t *pVprx = (vprx_t*)pInstance;
   uint32_t session_num = pVprx->session.session_num;
   elite_msg_any_t msg;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_cmd_handler session(%lx)",session_num);
   nResult = qurt_elite_queue_pop_front(pVprx->svc_handle.cmdQ, (uint64_t*) &msg);
   // ***************** Process the msg
   if (ADSP_EOK == nResult)
   {
      const uint32_t cmdTableSize = sizeof(pEliteHandler)/sizeof(pEliteHandler[0]);
      if (msg.unOpCode >= cmdTableSize)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Unsupported message ID 0x%8lx!! session(%lx)", msg.unOpCode,session_num);
         return ADSP_EUNSUPPORTED;
      }
	  
      if(TRUE == pVprx->fatal_state)
      {
         // accept only destroy and custom msg cmd in fatal state.
         if ((0 == msg.unOpCode) || ( 2 == msg.unOpCode))
         {
            nResult = pEliteHandler[msg.unOpCode](pInstance, &msg);
         }
         else
         {
            nResult = ADSP_EFAILED;
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vprx in fatal state!! result(%d) session(%x)",nResult,(int)pVprx->session.session_num);
            return nResult; 
         }
      }
      else
      {
         // table lookup to call handling function, with FALSE to indicate processing of msg
         nResult = pEliteHandler[msg.unOpCode](pInstance, &msg);
      }
   }
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_cmd_handler end nResult=%x session(%lx)",nResult,session_num);
   return nResult;
}

static ADSPResult vprx_buf_handler_pop_buf(vprx_t *pVprx, qurt_elite_bufmgr_node_t* buf_node_ptr)
{
   ADSPResult result = ADSP_EOK;

   //pop the buffer from the bufq.
   result = qurt_elite_queue_pop_front(pVprx->io.output.bufq_ptr, (uint64_t*)buf_node_ptr);
   if(ADSP_SUCCEEDED(result))
   {
      //check if the size of the buffer is sufficient to send current data
      elite_msg_data_buffer_t* pOutDataPayload = (elite_msg_data_buffer_t*) buf_node_ptr->pBuffer;
      uint32_t out_circ_buf_size = ( pVprx->io.output.circ_struct.circ_buf[0].max_samples *
            pVprx->io.output.mediatype.num_channels  )<<1;

      if((uint32_t)pOutDataPayload->nMaxSize != out_circ_buf_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_buf_handler_pop_buf(): buf max_size(%ld) is not matching with out_circ_buf size(%ld). Re-allocating buffer",
               pOutDataPayload->nMaxSize,out_circ_buf_size);
         int32_t req_size = GET_ELITEMSG_DATABUF_REQ_SIZE(out_circ_buf_size);

         //free the buffer and then allocate new buffer
         qurt_elite_memory_free(pOutDataPayload);

         pOutDataPayload =  (elite_msg_data_buffer_t*) qurt_elite_memory_malloc(req_size, QURT_ELITE_HEAP_DEFAULT);
         if(NULL == pOutDataPayload)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_buf_handler_pop_buf(): failed to allocate memory of size(%ld)",
                  req_size);
            return ADSP_ENOMEMORY;
         }

         pOutDataPayload->nActualSize = 0;           // No data in buffer by default
         pOutDataPayload->nMaxSize    = out_circ_buf_size;
         buf_node_ptr->pBuffer = pOutDataPayload;  //pBufferReturnQ was already updated
      }
   }

   return result;
}

static ADSPResult vprx_buf_handler(void *pInstance)
{
   // ***************** copy data to output queue and deliver
   ADSPResult nResult = ADSP_EOK;
   vprx_t *pVprx = (vprx_t*)pInstance;
   qurt_elite_bufmgr_node_t   outBufMgrNode;
   elite_msg_any_t* pPeerDataQMsg;
   int32_t output_samples=0;

   if (FALSE == pVprx->process_data_flag) // send output buffer only if in run state
   {
      pVprx->wait_mask &= (~VPRX_BUF_MASK);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx Buf handler called in stop state - handling error: removing buf mask , session(%#lx)",pVprx->session.session_num);
      return nResult;
   }

   if (pVprx->io.output.send_mediatype_flag)
   {
      elite_msg_any_t msg;
      uint32_t nPayloadSize = sizeof(elite_msg_data_media_type_apr_t) + sizeof(elite_multi_channel_pcm_fmt_blk_t);
      nResult = elite_msg_create_msg(&msg,&nPayloadSize, ELITE_DATA_MEDIA_TYPE, NULL,0,0 ) ;
      if (ADSP_FAILED(nResult))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Fail to create media fmt message");
         return nResult;
      }

      elite_msg_data_media_type_apr_t* pMediaTypePayload = (elite_msg_data_media_type_apr_t*)msg.pPayload;

      pMediaTypePayload->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;
      pMediaTypePayload->unMediaFormatID   = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

      elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk =
            (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pMediaTypePayload);

      memset(pMediaFormatBlk,0,sizeof(elite_multi_channel_pcm_fmt_blk_t));

      pMediaFormatBlk->num_channels        = pVprx->io.output.mediatype.num_channels;
      pMediaFormatBlk->bits_per_sample     = 16;
      pMediaFormatBlk->sample_rate         = pVprx->io.output.mediatype.sample_rate;
      pMediaFormatBlk->is_signed           = TRUE;
      pMediaFormatBlk->is_interleaved      = FALSE;

      if(pVprx->io.output.mediatype.num_channels == 1)
      {
         pMediaFormatBlk->channel_mapping[0] = PCM_CHANNEL_C;
      }
      else if(pVprx->io.output.mediatype.num_channels == 2)
      {
         pMediaFormatBlk->channel_mapping[0] = PCM_CHANNEL_L;
         pMediaFormatBlk->channel_mapping[1] = PCM_CHANNEL_R;
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_buf_handler(): Error!! invalid output num_channel(%ld) , session(%#lx)",pVprx->io.output.mediatype.num_channels,pVprx->session.session_num);
      }

      nResult = qurt_elite_queue_push_back(pVprx->downstream_peer_ptr->dataQ, (uint64_t*)&msg);
      if(ADSP_FAILED(nResult))
      {
         //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: pVprx->downstream_peer_ptr(%p) pVprx->downstream_peer_ptr->dataQ(%p)",
         //     pVprx->downstream_peer_ptr,pVprx->downstream_peer_ptr->dataQ);

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed deliver buf dwnstrm Dropping! result(%lx) session(%lx)",nResult,pVprx->session.session_num);
         (void) elite_msg_return_payload_buffer(&msg);
      }
      else
      {
         //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx delivered media type session(%x)",pVprx->session.session_num);
         pVprx->wait_mask |= VPRX_BUF_MASK;
         pVprx->io.output.send_mediatype_flag = 0;
      }
   }
   else if ((pVprx->io.output.min_frame_samples) <= (pVprx->io.output.circ_struct.circ_buf[0].unread_samples))
   {
      nResult = vprx_buf_handler_pop_buf(pVprx,&outBufMgrNode); //qurt_elite_queue_pop_front(pVprx->io.output.bufq_ptr, (uint64_t*)&outBufMgrNode);
      if (ADSP_EOK == nResult)
      {
         // NOTE: AFE can only handle samples which are multiple of 1ms/DMA_SAMPLES
         // Samples to be sent to AFE will be LCM(DMA samples, samples unread in output circ buffer)
         // min_out_frame_samples is always 1ms/DMA_SAMPLES(NB/WB). Since DMA_SAMPLES is power of 2,
         // the output samples can be : (samples unread in output circ buf) & ~((2^n)-1), where n is 3 or 4 here.
         // ie. if we are in NB mode and we have 159 samples in output buf, we will send out 152 samples, i.e 19ms,
         // rest will be lying in output circular buffer.
         // cons : If afe starts later than vprx, vprx output buffers can be held by afe and this will introduce
         // an extra delay (> 19/20ms)
         output_samples = ((pVprx->io.output.circ_struct.circ_buf[0].unread_samples)/ pVprx->io.output.min_frame_samples)* pVprx->io.output.min_frame_samples;

         // fill the ouptut buffer pointers
         elite_msg_data_buffer_t* pOutDataPayload = (elite_msg_data_buffer_t*) outBufMgrNode.pBuffer;
         //copy input buffer attributes to output buffer
         pOutDataPayload->pResponseQ           = NULL;
         pOutDataPayload->unClientToken        = NULL;
         pOutDataPayload->pBufferReturnQ       = (pVprx->io.output.bufq_ptr);
         pOutDataPayload->nOffset              = 0;
         pOutDataPayload->nActualSize          = (output_samples * pVprx->io.output.mediatype.num_channels) << 1 ; // for bytes
         // Following are not supported right now
         // pOutDataPayload->nFlag                = pInDataPayload->nFlag;
         // pOutDataPayload->llTimeStamp          = pInDataPayload->llTimeStamp;

         // copy from output circ buf to output buffer queue
         voice_multi_ch_circ_buf_block_read(&(pVprx->io.output.circ_struct),
               (int8_t*)&(pOutDataPayload->nDataBuf),
               output_samples*pVprx->io.output.mediatype.num_channels
         );

         // send pOutputBuf to downstream service.
         pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(
               &outBufMgrNode,
               ELITE_DATA_BUFFER,
               NULL, /* do not need response */
               0,    /* token */
               0     /* do not care response nResult*/
         );
         //MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx delivering data(%d), remaining samples (%d) session(%x)",output_samples, pVprx->io.output.circ_struct.circ_buf[0].unread_samples, pVprx->session.session_num);
         // Queue to VDS with timed buffer if this is first buffer of this vfr cycle or last buffer delivery was missed
         // Also last_delivery_flag is used for identifying first buffer queue after session start and this should also be sent with timed buffer.
         uint32_t buffer_id = VDS_UNTIMED_BUFFER;
         if ((pVprx->last_delivery_failed)||(1 == pVprx->timing.vfr_vprx_seq))
         {
            buffer_id = VDS_TIMED_BUFFER;
         }
         nResult = voice_cmn_send_vds_elite_request(pVprx->timing.vds_client_id, pVprx->timing.vds_client_token, buffer_id, pVprx->downstream_peer_ptr->dataQ,
               pVprx->timing.vds_handle_ptr, pPeerDataQMsg, pVprx->session.session_num);

         //nResult = qurt_elite_queue_push_back(pVprx->downstream_peer_ptr->dataQ, (uint64_t*)pPeerDataQMsg);
         if(ADSP_FAILED(nResult))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Error! Failed to queue to VDS, result %d session(%lx)", (int)nResult, pVprx->session.session_num);
            (void) elite_msg_push_payload_to_returnq(pVprx->io.output.bufq_ptr, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
         }
         else if ((pVprx->io.output.min_frame_samples) <= (pVprx->io.output.circ_struct.circ_buf[0].unread_samples))
         {  // more data left to be sent out
            pVprx->wait_mask |= VPRX_BUF_MASK;
         }
         else
         {
            pVprx->wait_mask &= (~VPRX_BUF_MASK);
            pVprx->rcvd_mask = 0x0;
         }
         pVprx->last_delivery_failed = FALSE;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error state vprx_cmd_handler dropping o/p buf session(%lx)",pVprx->session.session_num);
         (void) elite_msg_push_payload_to_returnq(pVprx->io.output.bufq_ptr, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
         pVprx->wait_mask &= (~VPRX_BUF_MASK);
      }
   }
   else // there was error reading queue.
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error Popping output buffer %d, %lx",nResult,pVprx->session.session_num);
   }
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_buf_handler end nResult=%x session(%x)",nResult,pVprx->session.session_num);

   /*
   if(qurt_elite_channel_poll(&pVprx->qurt_elite_channel,VPRX_DATA_MASK))
   {
       MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Back to back data present in VPRX, hitting corner case");

   }*/

   return nResult;
}

static ADSPResult vprx_data_handler(void *pInstance)
{
   ADSPResult nResult = ADSP_EOK;
   vprx_t *pVprx = (vprx_t*)pInstance;       // instance structure
   elite_msg_any_t InDataMsg;
   uint8           fDataQ = FALSE;           // flag to know if we popped from dataQ or gpQ
   // ***************** Pop Input buffers based on the input mask
   // Clear the play load
   InDataMsg.pPayload = NULL;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vprx_data_handler begin session(%x)",pVprx->session.session_num); //TODO: not needed
   if (pVprx->rcvd_mask & VPRX_DATA_MASK ) // check if it was in data input
   {
      // ***************** Read the input data
      nResult = qurt_elite_queue_pop_front(pVprx->svc_handle.dataQ, (uint64_t*)&InDataMsg);
      voice_result_check(nResult,(pVprx->session.session_num));
      fDataQ = TRUE;
   }
   else if( pVprx->rcvd_mask & VPRX_GPQ_DATA_MASK) // check for loopback data
   {
      // ***************** Read the gpQ input data
      nResult = qurt_elite_queue_pop_front(pVprx->svc_handle.gpQ, (uint64_t*)&InDataMsg);
      voice_result_check(nResult,(pVprx->session.session_num));
      //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vprx_data_handler received gpQ data, session(%x)",pVprx->session.session_num); //TODO: not needed
   }

   // ***************** Check Media Type
   if ( // If data payload is valid & if data is a mediatype
         (NULL != InDataMsg.pPayload)
         && (ELITE_DATA_MEDIA_TYPE == InDataMsg.unOpCode)
      )
   {
      if (ELITE_DATA_MEDIA_TYPE == InDataMsg.unOpCode)
      { // Check Media Type

         elite_msg_data_media_type_apr_t *pMediaTypePayload =(elite_msg_data_media_type_apr_t *)InDataMsg.pPayload;
         if( ELITEMSG_DATA_MEDIA_TYPE_APR == pMediaTypePayload->unMediaTypeFormat &&
             ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM   == pMediaTypePayload->unMediaFormatID)
         {
            // TODO: Check which Q (gpQ or dataQ) the MT was sent on...handle gpQ MT differently.
            //    Non-loopback mode:  Mixer output media type should match configured media type of Vprx processing
            //    Loopback Mode:  Tx media type sent to Rx for loopback may not match Vprx processing.  Flag error?  Resample?
            elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk =
                 (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pMediaTypePayload);

            //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Received In MEDIA TYPE sampRate(%d)!!", pMediaFormatBlk->sample_rate);
            if (ADSP_EOK != voice_check_media_t(&(pVprx->io.input.mediatype),pMediaFormatBlk))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: AFE in_mediatype doesnot match!! session(%lx)",pVprx->session.session_num);
            }
         }
      } //end of if (ELITE_DATA_MEDIA_TYPE == InDataMsg.unOpCode)
   }

   // ***************** Copy data to local circular buffer
   // TODO: Check for VFR clock when VFR is enabled
   if (pVprx->process_data_flag)
   {
      //TODO: Check for VFR Sync
      if (  // If data payload is valid & if data is pcm samples
            (NULL != InDataMsg.pPayload)
            && (ELITE_DATA_BUFFER == InDataMsg.unOpCode)
         )
      {  // Copy in data if any to circular buffer

         // This code serves as mux on 2 input queues (dataQ and gpQ), for downstream processing
         // When in loopback mode, we should wait until gpQ data comes.  When not in loopback mode we should wait until dataQ comes (fDataQ)
         if( ((TRUE  == pVprx->loopback_enable_flag) && ( FALSE == fDataQ)) ||
             ((FALSE == pVprx->loopback_enable_flag) && ( TRUE  == fDataQ)))
         {
            elite_msg_data_buffer_t* pInDataPayload = (elite_msg_data_buffer_t*) InDataMsg.pPayload;
            // Check if this is first buffer after going to run, then make sure to deliver it on proper tick
            if (( 0 == pVprx->timing.vfr_vprx_seq )&&( 1 < pInDataPayload->unClientToken )&&( 1 < pVprx->session.vfr_cycle ))
            {
				vprx_start_oneshot_timer(pVprx, pInDataPayload->unClientToken - 2);
                pVprx->last_delivery_failed = TRUE;
            }
            pVprx->timing.vfr_vprx_seq = pInDataPayload->unClientToken;
            voice_circbuf_write(&(pVprx->io.input.circ_struct), (int8_t*)&(pInDataPayload->nDataBuf),
                     (int32_t)(pInDataPayload->nActualSize >> 1));
            //dbg: MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: Copy In Data to circ Buf unread(%d) fDataQ(%d) session(%x)",pVprx->io.input.circ_struct.unread_samples,fDataQ,pVprx->session.session_num);
         }
      }
   }

   // ***************** Return buffers
   if (NULL != InDataMsg.pPayload)
   {  // Return In buffer
      nResult = elite_msg_return_payload_buffer ( &InDataMsg );
      if (ADSP_FAILED(nResult))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed return In end buffer session(%lx)",pVprx->session.session_num);
      }
      InDataMsg.pPayload = NULL;
   }

   // now call process
   if ((pVprx->io.min_process_frame_samples <= (pVprx->io.input.circ_struct.unread_samples)) && (pVprx->process_data_flag))
   {
      vprx_process(pVprx);
   }
   /* Indicate to CSM that VPRX is done for this cycle */
   //call into mvm using mvm_call as an entry point
   if ( pVprx->timing.vfr_vprx_seq == pVprx->session.vfr_cycle )
   {
      vss_imvm_csm_cmd_request_vote_t mvm_payload;
      mvm_payload.client = MVM_CSM_CLIENT_ENUM_VPRX;
      mvm_payload.vote = FALSE;

      mvm_call(VSS_IMVM_CSM_CMD_REQUEST_VOTE, &mvm_payload, sizeof(mvm_payload));
   }
   /* end of indication call */
   return nResult;
}

static ADSPResult vprx_process(void *pInstance)
{
   ADSPResult nResult = ADSP_EOK;
   vprx_t *pVprx = (vprx_t*)pInstance;       // instance structure

   {

      uint64_t time = qurt_elite_timer_get_time();
      uint64_t cycles = qurt_elite_profile_get_pcycles();

      vprx_modules_process(pVprx);

      time = qurt_elite_timer_get_time() - time;
      cycles = qurt_elite_profile_get_pcycles() - cycles;

      voice_cmn_time_profile_add_data( &pVprx->profiler, (uint32_t) time, cycles);

   }

   // ***************** check if output data is enough, switch to output mask

   if (pVprx->io.output.min_frame_samples <= (pVprx->io.output.circ_struct.circ_buf[0].unread_samples))
   {
      //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Setting output buf mask %d session(%x)",pVprx->io.output.circ_struct.unread_samples,pVprx->session.session_num);
      pVprx->wait_mask |= VPRX_BUF_MASK;
   }

   return nResult;
}

/* Since we always wait synchronously for responses, this should never get invoked */
static ADSPResult vprx_response_handler(void* pInstance)
{
   vprx_t *pVprx = (vprx_t*)pInstance;       // instance structure
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! vprx_response_handler invoked, session(%x)",(int)pVprx->session.session_num);
   return ADSP_EFAILED;
}

/* Since we always wait synchronously for responses, this should never get invoked */
static ADSPResult vprx_delivery_tick_end_handler(void *pInstance)
{
   ADSPResult nResult = ADSP_EOK;
   vprx_t *pVprx = (vprx_t*)pInstance;
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! vprx_delivery_tick_end_handler invoked, session(%x)",(int)pVprx->session.session_num);

   qurt_elite_signal_clear(pVprx->timing.delivery_tick_vtm_struct.signal_end_ptr);
   pVprx->timing.delivery_tick_vtm_struct.offset         = 0;
   pVprx->timing.delivery_tick_vtm_struct.signal_enable  = 0;

   return nResult;
}

static ADSPResult vprx_reset_sample_slip(void* pInstance)
{

      vprx_t *pVprx = (vprx_t*)pInstance;
      ADSPResult result = ADSP_EOK;


      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Reinitializing drift counters, session(%x)",(int)pVprx->session.session_num);

      //memset(&pVprx->ratematch.voice_drift_info, 0, sizeof(pVprx->ratematch.voice_drift_info));
      pVprx->is_resync_detected = TRUE;
      capi_v2_buf_t param_data_buf;
      capi_v2_port_info_t port_info;
      port_info.is_valid = FALSE;

      //get ss capi pointer. It is at the end of the chain
      voice_capi_module_t *ss_module_ptr = pVprx->modules.modules_array + pVprx->modules.num_modules-1;
      QURT_ELITE_ASSERT(ss_module_ptr->module_id == VOICE_MODULE_SAMPLE_SLIP);

      param_data_buf.data_ptr = NULL;
      param_data_buf.actual_data_len = param_data_buf.max_data_len = 0;
      if(ss_module_ptr->module_ptr)
      {
         capi_v2_err_t capi_result = ss_module_ptr->module_ptr->vtbl_ptr->set_param(ss_module_ptr->module_ptr,
               SS_PARAM_RESET,&port_info,&param_data_buf);
         if(CAPI_V2_FAILED(capi_result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: SS_PARAM_RESET failed for SS CAPI, session (%#lx)",pVprx->session.session_num);
         }
      }
      return result;
 }


static ADSPResult vprx_reset_output(void* pInstance)
{

    vprx_t *pVprx = (vprx_t*)pInstance;
    ADSPResult result = ADSP_EOK;
    // clear circular buffer and add 1 ms of prebuffering
    voice_multi_ch_circ_buf_reset(&(pVprx->io.output.circ_struct ));

    int16 prebuffering[FRAME_SAMPLES];
    int8_t *buf_ptr[MAX_CIRCBUF_CHANNELS]; // for multi channel circ buf
    memset(prebuffering,0,sizeof(prebuffering));

    for(uint32_t i=0; i < pVprx->io.output.mediatype.num_channels; i++)
    {
        buf_ptr[i] = (int8_t*) &(prebuffering[0]);
    }

    voice_multi_ch_circ_buf_write(&pVprx->io.output.circ_struct, &buf_ptr, (int32_t)(PRE_BUF * pVprx->io.output.min_frame_samples));

    //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: DBG vprx_reset_output unread samples (%ld)",(int)pVprx->io.output.circ_struct.circ_buf[0].unread_samples);

    if (pVprx->wait_mask & VPRX_BUF_MASK)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP:BUF mask is set ... reseting it as data is cleared, session(%x)",(int)pVprx->session.session_num);
        pVprx->wait_mask &= (~VPRX_BUF_MASK);
    }
    return result;

}

static ADSPResult vprx_delivery_miss_response_handler(void* pInstance)
{
    vprx_t *pVprx = (vprx_t*)pInstance;
    ADSPResult result = ADSP_EOK;
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_delivery_miss_response_handler, session(%x)",(int)pVprx->session.session_num);

    qurt_elite_signal_clear(pVprx->delivery_miss_signal_ptr);
    /*
     * if at this point RESYNC signal is set but vprx has not yet handled it. This is possible is vprx was busy processing past buffer, when resync signal was set.
     * But before it could handle resync, delivery miss happened and vprx_delivery_miss_response_handler() got triggered.
     * After vprx completes processing of past buffer, it will have delivery_miss_response_signal,resync signal, buf_mask (as the buffer was processed)
     * Since delivery_miss_response_signal is at highest priority, we will check resync signal and if that is set, no need to reset output because
     * resync_response handling will take care. Only current buffer will be dropped in VDS due to resync
     * If we dont do this then we will end up resetting current output buffer and next buffer will be dropped in VDS due to resync hence resulting in 40 ms of gap at output
     */
    if(qurt_elite_channel_poll(&pVprx->qurt_elite_channel,VPRX_RESYNC_MASK))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: resync signal is already  set !! do nothing, session(%x)",(int)pVprx->session.session_num);
        return result;
    }
    // no need to start oneshot timer as long as we dont receive new buffer after session start
    // similarly dont start oneshot timer for last miss of vfr cycle
    if((pVprx->timing.vfr_vprx_seq != 0)&&(pVprx->timing.vfr_vprx_seq != pVprx->session.vfr_cycle))
    {
      vprx_start_oneshot_timer(pVprx, pVprx->timing.vfr_vprx_seq - 1 );
      pVprx->last_delivery_failed = TRUE;
    }

    if( qurt_elite_channel_poll(&pVprx->qurt_elite_channel,VPRX_BUF_MASK)  && 
             (pVprx->wait_mask & VPRX_BUF_MASK)   ) 
    {
       //qurt_elite_channel_poll call is to take care of AFE port stop cases
       //If buf mask is set and signal is set, then we are here because of delivery miss due to processing overshoot
       //Queue dummy request to VDS to calculate overshoot time

       elite_msg_any_t msg = {0};
       voice_cmn_send_vds_elite_request(pVprx->timing.vds_client_id, pVprx->timing.vds_client_token, VDS_INACTIVE_BUFFER, pVprx->downstream_peer_ptr->dataQ,
             pVprx->timing.vds_handle_ptr, &msg, pVprx->session.session_num);
    }

    vprx_reset_sample_slip(pVprx);
    vprx_reset_output(pVprx);

    return result;
}

static ADSPResult vprx_start_oneshot_timer(vprx_t* pVprx, uint16_t seq_offset)
{
    ADSPResult result = ADSP_EOK;
    //VTM start one shot timer for next delivery, if pending for this VFR cycle
    pVprx->timing.delivery_tick_vtm_struct.signal_enable  = 1;
    pVprx->timing.delivery_tick_vtm_struct.vfr_mode  = (uint8_t)pVprx->session.vfr_mode;
    pVprx->timing.delivery_tick_vtm_struct.offset  = (uint16_t)(pVprx->timing.delivery_offset + seq_offset*MAX_TIMER_OFFSET);
    pVprx->timing.delivery_tick_vtm_struct.vsid    = pVprx->timing.vsid;
    pVprx->timing.delivery_tick_vtm_struct.timing_ver = pVprx->timing.timing_ver;

    if (ADSP_FAILED(result = voice_custom_vt_sub_unsub_msg_send(NULL,pVprx->timing.vtm_cmdq_ptr,VOICE_TIMER_START_ONESHOT,NULL,FALSE,&(pVprx->timing.delivery_tick_vtm_struct))))
    {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vprx start oneshot timer to Vtm session(%lx)",pVprx->session.session_num);
    }
    return result;
}

static ADSPResult vprx_resync_response_handler(void* pInstance)
{
    vprx_t *pVprx = (vprx_t*)pInstance;
    ADSPResult result = ADSP_EOK;
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_resync_response_handler, session(%x)",(int)pVprx->session.session_num);
    qurt_elite_signal_clear(pVprx->resync_response_signal_ptr);

    if (pVprx->wait_mask & VPRX_BUF_MASK)
    {
        // if BUF mask is set no need to clear buffer but just append required amount of zeros

        uint32 unread_samples,num_samples_required;
        unread_samples = pVprx->io.output.circ_struct.circ_buf[0].unread_samples; //unread samples should be same for all channels.
        num_samples_required = (FRAME_SAMPLES + DMA_SAMPLES) * pVprx->session.in_nb_sampling_rate_factor;

        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP:BUF mask is set with unread samples %ld",unread_samples);
        vprx_reset_sample_slip(pVprx);
        if (unread_samples>num_samples_required)
        {
            // this should never happen
            uint16 discard_samples = unread_samples - num_samples_required;
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO," VCP: circular buffer samples are more than required, retaining %ld discarding %ld, session(%x)",
                    num_samples_required,discard_samples,(int)pVprx->session.session_num);
            for (int32 i=0;i<pVprx->io.output.circ_struct.num_channels;i++)
            {
                voice_circbuf_read((circbuf_struct *)&(pVprx->io.output.circ_struct.circ_buf[i]), NULL, discard_samples,0);
                if (CIRCBUF_SUCCESS != result)
                {
                    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: failure in discarding samples in VPRX, index(%ld)",i);
                    return result;
                }
            }
        }
        else
        {
            uint32 num_zero_padding = num_samples_required - unread_samples;
            int16 buffer[2*VOICE_FB_SAMPLING_RATE/1000] = {0};   // buffer of 2 ms duration as the number of unread samples can be 152-168

            int8_t *buf_ptr[MAX_CIRCBUF_CHANNELS]; // for multi channel circ buf

            for(uint32_t i=0; i < pVprx->io.output.mediatype.num_channels; i++)
            {
               buf_ptr[i] = (int8_t*) &(buffer[0]);
            }

            if (CIRCBUF_SUCCESS != voice_multi_ch_circ_buf_prefill(&pVprx->io.output.circ_struct,&buf_ptr,num_zero_padding))
            {
                vprx_reset_output(pVprx);
            }

        }

    }
    else
    {
        vprx_reset_sample_slip(pVprx);
        vprx_reset_output(pVprx);
    }
    return result;
}

static ADSPResult vprx_resync_handler(void* pInstance)
{
    vprx_t *pVprx = (vprx_t*)pInstance;
    ADSPResult result = ADSP_EOK;
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx resync, session(%x)",(int)pVprx->session.session_num);
    qurt_elite_signal_clear(pVprx->timing.delivery_tick_vtm_struct.resync_signal_ptr);

    //incrementing resync count
    pVprx->resync_count++;
    // Issue resync to vds
    if(ADSP_FAILED(result = vprx_vds_send_resync(pInstance)))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! vprx vds resync failed, session(%x)",(int)pVprx->session.session_num);
    }

    return result;
}
/* =======================================================================
 **                          Message handler functions
 ** ======================================================================= */

static ADSPResult vprx_destroy_yourself_cmd (void *pInstance, elite_msg_any_t* pMsg)
{
   vprx_destroy((vprx_t*)pInstance);

   elite_msg_finish_msg(pMsg, ADSP_EOK);

   // send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
   return ADSP_ETERMINATED;
}

static ADSPResult vprx_connect_dwn_stream_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   elite_msg_custom_voc_svc_connect_type* pConnectMsgPayload;
   elite_svc_handle_t *pSvc2Connect;
   vprx_t* pVprx = (vprx_t*)pInstance;

   if (NULL == pVprx)
   {
      return ADSP_EFAILED;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_connect_dwn_stream_cmd session(%lx)",pVprx->session.session_num);

   if (pVprx->process_data_flag)
   {
      nResult = ADSP_ENOTREADY; // Cannot reconnect in RUN state. TODO: Needed?
   }

   pConnectMsgPayload = (elite_msg_custom_voc_svc_connect_type*) (pMsg->pPayload);
   if (VOICEPROC_CONNECT_DWN_STREAM_CMD != pConnectMsgPayload->sec_opcode)
   {
      nResult = ADSP_EBADPARAM;
      elite_svc_send_ack(pMsg, nResult);
      return nResult;
   }

   pSvc2Connect = pConnectMsgPayload->svc_handle_ptr;
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VoiceprocRx connecting to SvcID 0x%8x session(%x)",pSvc2Connect->unSvcId,pVprx->session.session_num);

   // This service only allows one downstream
   if (NULL != pVprx->downstream_peer_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: FAILED, only 1 peer allowed session(%lx)",pVprx->session.session_num);
      nResult = ADSP_EUNSUPPORTED;
   }
   else //else accept the connection
   {
      pVprx->downstream_peer_ptr = pSvc2Connect;
//      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: pVprx->downstream_peer_ptr(%p) pVprx->downstream_peer_ptr->dataQ(%p)",
//	      pVprx->downstream_peer_ptr,pVprx->downstream_peer_ptr->dataQ);
   }
   elite_svc_send_ack(pMsg, nResult);

   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_connect_dwn_stream_cmd end nResult=%x session(%x)",nResult,pVprx->session.session_num);
   return nResult;
}

// Caller is expected to only send this message during a good state!!!
static ADSPResult vprx_disconnect_dwn_stream_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   elite_msg_custom_voc_svc_connect_type* pDisconnectMsgPayload;
   elite_svc_handle_t *pSvc2Disconnect;
   vprx_t* pVprx = (vprx_t*)pInstance;

   if (NULL == pVprx)
   {
      return ADSP_EFAILED;
   }
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vprx_DisConnectDwnStreamCmd session(%lx)",pVprx->session.session_num);

   if(pVprx->process_data_flag)
   {
      // Cannot disconnect Vprx down stream service until it was set to stop earlier
      nResult = ADSP_EBUSY;
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ERROR ::Trying to disconnect before Vprx stop command(%ld)",pVprx->session.session_num);
   }
   else
   {
      pDisconnectMsgPayload = (elite_msg_custom_voc_svc_connect_type*) (pMsg->pPayload);
      if (VOICEPROC_DISCONNECT_DWN_STREAM_CMD != pDisconnectMsgPayload->sec_opcode)
      {
         nResult = ADSP_EBADPARAM;
         elite_svc_send_ack(pMsg, nResult);
         return nResult;
      }

      pSvc2Disconnect = pDisconnectMsgPayload->svc_handle_ptr ;
      if (pSvc2Disconnect != pVprx->downstream_peer_ptr)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: FAILED, not connected to that service. session(%lx)",pVprx->session.session_num);
         nResult = ADSP_EBADPARAM;
      }
      else //else accept the connection
      {
         pVprx->downstream_peer_ptr = NULL;
      }
   }

   elite_svc_send_ack(pMsg, nResult);
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vprx_DisConnectDwnStreamCmd end nResult=%x session(%x)",nResult,pVprx->session.session_num);
   return nResult;
}

static ADSPResult vprx_set_param_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   // TODO: should send an ACK back to caller to avoid race conditions, and to inform success/fail.
   vprx_t* pVprx = (vprx_t*)pInstance;
   ADSPResult nResult = ADSP_EOK;
   elite_msg_param_cal_t* pFaddCalPayload;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_set_param_cmd begin session(%lx)",pVprx->session.session_num);

   if (NULL == pMsg)
   {
      return ADSP_EBADPARAM;
   }

   pFaddCalPayload = (elite_msg_param_cal_t*) pMsg->pPayload;

   if (VPM_RX_NONE == pVprx->session.topology_id)  // no calibration support in none topology
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: calibartion not supported in none topology, returning. session(%lx)\n",pVprx->session.session_num);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return nResult;
   }

   bool_t cal_set_param = FALSE;

   if (ELITE_CMD_SET_PARAM == pMsg->unOpCode)
   {
      switch (pFaddCalPayload->unParamId)
      {
         case ELITEMSG_PARAM_ID_CAL:
         case ELITEMSG_PARAM_ID_CAL_V2:
            {
               cal_set_param = TRUE;

               // Extract address of Cal data and size info
               uint32_t payload_address = (uint32_t)pFaddCalPayload->pnParamData; // address of voice_param_data_t
               uint32_t payload_size = pFaddCalPayload->unSize;

               //param size and payload address have to be multiples of 4
               if((0 == payload_size)  || (NULL == payload_address ))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vprx recieved wrong size cal! Addr %#x, size %d", (int)payload_address, (int)payload_size);
                  elite_svc_send_ack(pMsg, ADSP_EBADPARAM);
                  return ADSP_EOK;
               }

               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx set param, voice_param_data_t address(%x) size(%x)",(int)payload_address,(int)payload_size);

               {
                  ADSPResult aggregate_result = ADSP_EOK;
                  aggregate_result = vprx_modules_set_param(pVprx, payload_address, payload_size, pFaddCalPayload->unParamId);
                  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx sending ack %d", aggregate_result);
                  elite_svc_send_ack(pMsg, aggregate_result);
               }
               break;
            }
         default :
            {
               elite_svc_send_ack(pMsg, ADSP_EBADPARAM); // TODO: is this if check needed
               return ADSP_EOK;
            }
      }
   } // if (ELITE_CMD_SET_PARAM == pMsg->unOpCode)
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_set_param_cmd secondary opcode not supported session(%x)",(int)pVprx->session.session_num);
      elite_svc_send_ack(pMsg, ADSP_EFAILED); // TODO: is this if check needed
      return ADSP_EOK;
   }

   // Check if offset recalculation is needed
   // Ignore the case when we are receiving parameters
   // from Tx path (do it only for calibration/tuning cases.
   if(pVprx->process_data_flag && TRUE == cal_set_param)
   {
      uint32_t offset_changed_flag = 0;

      //vcp does offset calcuation in case of old timing
      if (FALSE == pVprx->timing.use_new_timing_flag)
      {
         (void)vprx_aggregate_modules_kpps(pVprx, &offset_changed_flag); // discarding for now since result is always success
         vprx_calc_offsets(pVprx);
      }

      if(offset_changed_flag)
      {
         // If offset is increased, just update it in the timer struct. There is no need for special handling
         // Essentially if the set param happens after the current buffer is done, we don't really care about the changed offset
         // If the change happens before delivery tick fires, this delivery is on time, but the next one is later wrt to the previous one
         // If the tick fires after the change, this delivery is delayed wrt to previous, but then timing is setup correctly
         // On the other hand, if set param happens before processing, there are two possibilities. One is that the offset is updated before VFR
         // fires, so the next tick is at the right time. This is good. On the other hand, the timer could already be started before the offset is
         // updated. This is not so good. Now it is possible that vprx cannot finish on time for the old tick, and VDS will have nothing to deliver.
         // This buffer will get queued at VDS, but there should be no delay since the next buffer request from vprx will overwrite this one.
         // Vprx can also finish on time, in which case this buffer is delivered as per the old timing. This is not a problem in terms of introducing
         // delay either.
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx offset changed, updating delivery offset from %d to %d session(%x)",
               pVprx->timing.delivery_tick_vtm_struct.offset, (int)pVprx->timing.delivery_offset, (int)pVprx->session.session_num);
         pVprx->timing.delivery_tick_vtm_struct.offset  = (int16_t)pVprx->timing.delivery_offset;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: No change in vprx offset, session(%x)", (int)pVprx->session.session_num);
      }
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_set_param_cmd end nResult=%x session(%lx)",nResult,pVprx->session.session_num);
   return nResult;
}

static ADSPResult vprx_get_param_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   vprx_t* pVprx = (vprx_t*) pInstance;
   ADSPResult nResult = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_get_param_cmd begin :(%lx)\n",pVprx->session.session_num);

   if (VPM_RX_NONE == pVprx->session.topology_id)  // no calibration support in none topology
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: calibartion not supported in none topology, returning. session(%lx)\n",pVprx->session.session_num);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return nResult;
   }

   if (ELITE_CMD_GET_PARAM == pMsg->unOpCode)
   {
      elite_msg_param_cal_t* pFaddPayload = (elite_msg_param_cal_t*) pMsg->pPayload;
      uint16_t param_size;
      if( (ELITEMSG_PARAM_ID_CAL == pFaddPayload->unParamId) ||
            (ELITEMSG_PARAM_ID_CAL_V2 == pFaddPayload->unParamId) )
      {
         ADSPResult nLocalResult    = ADSP_EUNSUPPORTED;

          // Local scope
         nLocalResult = vprx_modules_get_param(pVprx, (void *)pFaddPayload->pnParamData,pFaddPayload->unParamId, &param_size);

         // Need to return actual size filled, static service will populate this field in the get param
         // This is to accomodate cases where the same mod id/param id pair exists on both tx and rx paths
         pFaddPayload->unSize = param_size;

         elite_svc_send_ack(pMsg, nLocalResult);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Rx get param bad fadd id,  result(0x%x) session(0x%lx)\n",
               ADSP_EFAILED, pVprx->session.session_num);
         elite_svc_send_ack(pMsg, ADSP_EFAILED);
         // No local get param
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_get_param_cmd secondary opcode not supported :(%lx)\n",pVprx->session.session_num);
      nResult = ADSP_EFAILED;
      elite_svc_send_ack(pMsg, nResult);
   }


   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_get_param_cmd end handler result(0x%x) session(0x%lx)",nResult, pVprx->session.session_num);
   return nResult;
}

static ADSPResult vprx_run_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   // TODO: should send an ACK back to caller to avoid race conditions, and to inform success/fail.
   vprx_t* pVprx = (vprx_t*)pInstance;
   elite_msg_custom_voiceproc_run_payload_t *payload_ptr;
   ADSPResult nResult = ADSP_EOK;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_run_cmd begin session(%lx)",pVprx->session.session_num);
   // TODO: remove the following from RUN, not needed.

   if (NULL == pVprx->downstream_peer_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Vprx cannot run with NULL downstream peer! session(%x)", (int)pVprx->session.session_num);
      nResult = ADSP_ENOTREADY;
      elite_svc_send_ack(pMsg, nResult);
      return nResult; // not ready to run since downstream is not yet connected
   }

   // Check if this is custom RUN.
   // If yes, then extract drift ptr which is passed over from AFE->VPM during AFE connect ack back.
   // This drift ptr is then propogated to Vtm which then gets the drift info in this new scheme
   // compared to old scheme where device id is used as an index into drift array.
   if (ELITE_CUSTOM_MSG == pMsg->unOpCode)
   {
      payload_ptr = (elite_msg_custom_voiceproc_run_payload_t *) pMsg->pPayload;
      if (VOICEPROC_RUN_CMD == payload_ptr->unSecOpCode)
      {
         pVprx->afe_info.afe_drift_ptr = payload_ptr->afe_drift_ptr;
         pVprx->timing.session_thread_clock_mhz = payload_ptr->session_thread_clock_mhz;
      }
      else
      {
         pVprx->afe_info.afe_drift_ptr = NULL;
         nResult = ADSP_EUNSUPPORTED;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_run_cmd invalid sec opcode(%d), session(%x)",(int)payload_ptr->unSecOpCode,(int)pVprx->session.session_num);
      }
   }

   if (ADSP_EOK == nResult)
   {
      uint32_t offset_changed_flag;

      //vcp does offset calcuation in case of old timing
      if (FALSE == pVprx->timing.use_new_timing_flag)
      {
         (void)vprx_aggregate_modules_kpps(pVprx, &offset_changed_flag); // discarding for now since result is always success
         // Get offset and subscribe for delivery tick
         vprx_calc_offsets(pVprx);
      }

      if (ADSP_FAILED(voice_cmn_send_vds_command(&pVprx->timing.vds_client_id, pVprx->timing.vds_client_token,
                                     pVprx->respq_ptr, VDS_SUBSCRIBE, &pVprx->timing.delivery_tick_vtm_struct.signal_ptr,
                                     pVprx->delivery_miss_signal_ptr, pVprx->timing.vds_handle_ptr, pVprx->session.session_num, TRUE,pVprx->resync_response_signal_ptr)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vprx subscribe to VDS, result(%d) session(%x)", (int)nResult, (int)pVprx->session.session_num);
         elite_svc_send_ack(pMsg, nResult);
         return nResult;
      }
      pVprx->timing.vfr_vprx_seq = 0;  // reset vprx sequence for session start
      //VTM subscribe
      pVprx->timing.delivery_tick_vtm_struct.signal_enable  = 1;
      pVprx->timing.delivery_tick_vtm_struct.vfr_mode  = (uint8_t)pVprx->session.vfr_mode;
      pVprx->timing.delivery_tick_vtm_struct.vfr_cycle  = (uint16_t)(pVprx->session.vfr_cycle*MAX_TIMER_OFFSET);
      pVprx->timing.delivery_tick_vtm_struct.offset  = pVprx->timing.delivery_offset;
      pVprx->timing.delivery_tick_vtm_struct.vsid    = pVprx->timing.vsid;
      pVprx->timing.delivery_tick_vtm_struct.timing_ver = pVprx->timing.timing_ver;

      if (ADSP_FAILED(nResult = voice_custom_vt_sub_unsub_msg_send(NULL,pVprx->timing.vtm_cmdq_ptr,VOICE_TIMER_SUBSCRIBE,NULL,FALSE,&(pVprx->timing.delivery_tick_vtm_struct))))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vprx subscribe to Vtm session(%lx)",pVprx->session.session_num);
      }
      if (VFR_NONE == pVprx->session.vfr_mode)
      {
         (void) qurt_elite_channel_wait(&pVprx->qurt_elite_channel, VPRX_DELIVERY_TICK_END_MASK);
         qurt_elite_signal_clear(pVprx->timing.delivery_tick_vtm_struct.signal_end_ptr); // clear the signal
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx Done waiting for delivery signal from VTM; session (%x)", (int)pVprx->session.session_num);
      }
      pVprx->wait_mask = VFR_CMD_DATA_MASK;  // Start with listening to input commands and input data only
   } // end of nResult condition to check if custom_run suceeded

   {
      // pre-buffering to avoid under-runs during sample slip cases.
      // As AFE accepts block sizes multiple of 1msec, we cannot send 159 samples during sample slip cases
      // We can send only 152samples (19msec). This prebuffering would help in handling the 1msec gaps.
      // one msec is for SS and 2msec is to handler system jitters.
      int16 prebuffering[FRAME_SAMPLES];
      int8_t *buf_ptr[MAX_CIRCBUF_CHANNELS]; // for multi channel circ buf
      memset(prebuffering,0,sizeof(prebuffering));

      for(uint32_t i=0; i < pVprx->io.output.mediatype.num_channels; i++)
      {
         buf_ptr[i] = (int8_t*) &(prebuffering[0]);
      }

      voice_multi_ch_circ_buf_write(&pVprx->io.output.circ_struct, &buf_ptr, (int32_t)(PRE_BUF * pVprx->io.output.min_frame_samples));

   }

   nResult = vprx_modules_full_init(pVprx);   //This function initializes all the modules with fresh calibration at the start of every call
   if (ADSP_FAILED(nResult))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx modules Init Error - ignoring");
   }

   pVprx->process_data_flag    = 1; // Start processing
   pVprx->io.output.send_mediatype_flag  = 1; // Send media type before running
   pVprx->resync_count = 0;                   // re-setting resync count
   elite_svc_send_ack(pMsg, nResult);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_run_cmd end nResult=%x session(%lx)",nResult,pVprx->session.session_num);
   return nResult;
}

static ADSPResult vprx_re_config_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   // should send an ACK back to caller to avoid race conditions, and to inform success/fail.
   vprx_t* pVprx = (vprx_t*)pInstance;
   ADSPResult nResult = ADSP_EOK;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_re_config_cmd begin session(%lx)",pVprx->session.session_num);
   elite_msg_voice_re_config_type *pReConfig= (elite_msg_voice_re_config_type*) pMsg->pPayload;

   if(1 != pReConfig->num_channels)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_re_config_cmd enter num_channels(%ld). Setting it to 1", pReConfig->num_channels);
      pReConfig->num_channels = 1;
   }

   nResult = vprx_is_supported(pReConfig->topology_id, pReConfig->sampling_rate, pVprx->session.session_num, pVprx->afe_info.afe_rx_port_id,pReConfig->num_channels);
   if (ADSP_FAILED(nResult))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_re_config_cmd Invalid Topology(%lx) or Invalid Samping Rate(%lx), session(%lx)",pReConfig->topology_id, pReConfig->sampling_rate,pVprx->session.session_num);
   }
   else if (pVprx->process_data_flag)
   {  // Cannot start processing utill it was set to stop earlier
      nResult = ADSP_ENOTREADY;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Can't Re-Configure while running session(%lx)",pVprx->session.session_num);
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: reconfig Vprx topo(%lx),SR(%ld)!! ",pReConfig->topology_id,pReConfig->sampling_rate);

      //update reconfig state
      pVprx->reconfig_state.reconfig_pending = TRUE;
      pVprx->reconfig_state.retain_amdb_handle = FALSE;

      //Check if its recondig to same topo and update handle retain flag.
      if(pVprx->session.topology_id == pReConfig->topology_id)
      {
         pVprx->reconfig_state.retain_amdb_handle = TRUE;
      }

      /* not currently checking if only port changes.  Could optimize to skip End/Init in this case since Init currently
       * doesn't use port
       */
      vprx_end(pVprx);

      vprx_create_params_t create_param = {0};
      create_param.afe_port_id   = pReConfig->port_id_rx;
      create_param.sampling_rate = pReConfig->sampling_rate;
      create_param.topology_id   = pReConfig->topology_id;
      //create_param.num_channels   = pReConfig->num_channels; ignore num channels
      memscpy(&create_param.channel_map[0],sizeof(create_param.channel_map),&pReConfig->channel_map_near[0],sizeof(pReConfig->channel_map_near[0])*pReConfig->num_channels);
      nResult = vprx_init(pVprx, &create_param);
      if (ADSP_FAILED(nResult))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed reconfig Vprx !! session(%lx)",pVprx->session.session_num);
         vprx_end(pVprx);
      }
   }

   // re-send HPCM start if required
   if ( (TRUE == pVprx->modules.hpcm_enable_flag) && (NULL != pVprx->modules.special_capis[HPCM]->module_ptr) )
   {
      capi_v2_buf_t param_data_buf;
      capi_v2_port_info_t port_info;
      port_info.is_valid = FALSE;
      param_data_buf.data_ptr = (int8_t *) &pVprx->modules.hpcm_tap_point;
      param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(voice_tap_point_v2_t);
      nResult = pVprx->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(pVprx->modules.special_capis[HPCM]->module_ptr,
            FWK_EXTN_PARAM_ID_HPCM_START, &port_info, &param_data_buf);
      nResult = capi_v2_err_to_adsp_result(nResult);
      if (ADSP_FAILED(nResult))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vprx_re_config_cmd() - HPCM_START set param to CAPI failed!!");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_re_config_cmd() - HPCM_START set param to CAPI successful!!");
      }
   }

   //set reconfig pending flag to false.
   pVprx->reconfig_state.reconfig_pending = FALSE;
   pVprx->reconfig_state.retain_amdb_handle = FALSE;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Reconfig cmd end: Tid(%ld) another_thread_launched(%ld)",qurt_thread_get_id(), pVprx->reconfig_state.another_thread_launched);

   elite_svc_send_ack(pMsg, nResult);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_re_config_cmd end nResult=%x session(%lx)",nResult,pVprx->session.session_num);
   return ADSP_EOK;
}

static ADSPResult vprx_stop_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   vprx_t* pVprx = (vprx_t*) pInstance;
   ADSPResult nResult = ADSP_EOK;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_stop_cmd begin session(%lx)",pVprx->session.session_num);
   pVprx->wait_mask = VFR_CMD_DATA_MASK;  // Start listening to input commands and input data only
   //unsubscribe from delivery tick
   vprx_vtm_unsubscribe(pInstance, &(pVprx->timing.delivery_tick_vtm_struct), VPRX_DELIVERY_TICK_END_MASK );
   // unsubscribe from vds
   if (ADSP_FAILED(voice_cmn_send_vds_command(&pVprx->timing.vds_client_id, pVprx->timing.vds_client_token,
                                     pVprx->respq_ptr, VDS_UNSUBSCRIBE, &pVprx->timing.delivery_tick_vtm_struct.signal_ptr,
                                     pVprx->delivery_miss_signal_ptr, pVprx->timing.vds_handle_ptr, pVprx->session.session_num, TRUE,pVprx->resync_response_signal_ptr)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vprx unsubscribe to VDS, result(%d) session(%x)", (int)nResult, (int)pVprx->session.session_num);
   }
   // listen input timing tick and data only
   // listening to data to drop it and free buffer
   pVprx->process_data_flag    = 0; // Stop processing
   pVprx->io.output.send_mediatype_flag  = 1; // setting

   // Clear Circular Buffers, should not deliver in stop state
   voice_circbuf_reset(&(pVprx->io.input.circ_struct));
   voice_multi_ch_circ_buf_reset(&(pVprx->io.output.circ_struct ));

   elite_svc_send_ack(pMsg, nResult);

   // Check guard band for memory corruption
   nResult =  nResult | (voice_check_guard_band((int8_t**)pVprx->memory.guard_band_ptrs, (int16_t)pVprx->modules.num_modules, RX, pVprx->session.session_num));

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_stop_cmd end nResult=%x session(%lx)",nResult,pVprx->session.session_num);
   return nResult;
}

static ADSPResult vprx_set_mute_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   vprx_t* pVprx = (vprx_t*)pInstance;
   elite_msg_custom_voc_set_soft_mute_type *pSetMuteCmd = (elite_msg_custom_voc_set_soft_mute_type *) pMsg->pPayload;
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vprx_SetMute muteCmd(%d), ramp_duration(%ld), session(%lx)",
         pSetMuteCmd->mute,pSetMuteCmd->ramp_duration,pVprx->session.session_num);
   voice_soft_mute_cal_param_t payload_soft_mute;

   if( pSetMuteCmd->mute > VOICE_MUTE)
   {
      nResult = ADSP_EBADPARAM;
   }
   else
   {
      if ( pVprx->mute != pSetMuteCmd->mute )
      {
         payload_soft_mute.mute = pVprx->mute = pSetMuteCmd->mute; //can be removed later on
         payload_soft_mute.ramp_duration = pSetMuteCmd->ramp_duration;

         MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Sending soft mute command");

         nResult = vprx_modules_set_param_int(pVprx, VOICE_MODULE_SOFT_MUTE, 0, VOICE_PARAM_SOFT_MUTE, &payload_soft_mute, sizeof(voice_soft_mute_cal_param_t));
         if (ADSP_FAILED(nResult))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error!! soft mute command processing. result(%x)",nResult);
         }
      }
   }

   elite_svc_send_ack(pMsg, nResult);
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vprx_SetMute end nResult(%d) session(%x)",nResult,pVprx->session.session_num);
   return nResult;
}

static ADSPResult vprx_set_timing_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   vprx_t* pVprx = (vprx_t*)pInstance;
   elite_msg_custom_voc_timing_param_type *pSetTimingCmd = (elite_msg_custom_voc_timing_param_type *) pMsg->pPayload;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_set_timing_cmd begin session(%x)",pVprx->session.session_num);
   if (FALSE == pVprx->process_data_flag)
   {
      voice_set_timing_params_t* pVfrCmd = (voice_set_timing_params_t*)pSetTimingCmd->param_data_ptr;
      MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VOICE_SET_TIMING_PARAMS,Vprx vfr_mode(%x), offset(%x), process_data_flag(%x)",pVfrCmd->mode, pVfrCmd->enc_offset, pVprx->process_data_flag);
      pVprx->session.vfr_mode = pVfrCmd->mode;
      pVprx->session.vfr_source = voice_cmn_get_vfr_source(pVprx->session.vfr_mode);
      pVprx->timing.dec_offset = pVfrCmd->dec_offset;
      pVprx->timing.use_new_timing_flag = FALSE;
      pVprx->timing.timing_ver = VFR_CLIENT_INFO_VER_1; // timing without VSID
      elite_svc_send_ack(pMsg, ADSP_EOK);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VFR mode can't be changed in RUN session(%lx)",pVprx->session.session_num);
      elite_svc_send_ack(pMsg, ADSP_EBUSY);
   }

   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_set_timing_cmd end nResult(%d) session(%x)",nResult,pVprx->session.session_num);
   return nResult;
}

static ADSPResult vprx_set_timingv2_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   vprx_t* pVprx = (vprx_t*)pInstance;
   elite_msg_custom_voc_timing_param_type *pSetTimingCmd = (elite_msg_custom_voc_timing_param_type *) pMsg->pPayload;
   vpm_set_timing_params_t* pVfrCmd = (vpm_set_timing_params_t*)pSetTimingCmd->param_data_ptr;

   // Check for Run state.
   if (FALSE == pVprx->process_data_flag)
   {
      pVprx->session.vfr_mode = pVfrCmd->mode;
      pVprx->session.vfr_cycle = 1;
      pVprx->timing.delivery_offset= pVfrCmd->vprx_delivery_offset;

      // Select VFR source
      pVprx->session.vfr_source = voice_cmn_get_vfr_source(pVprx->session.vfr_mode);
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VPM_CMD_SET_TIMING_PARAMS,Vprx vfr_mode(%x), delivery_offset(%x)",pVfrCmd->mode, pVfrCmd->vprx_delivery_offset);
      pVprx->timing.timing_ver = VFR_CLIENT_INFO_VER_1; // timing without VSID
      pVprx->timing.use_new_timing_flag = TRUE;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VFR mode can't be changed in RUN session(%lx)",pVprx->session.session_num);
      nResult = ADSP_EBUSY;
   }

   elite_svc_send_ack(pMsg, nResult);
   return ADSP_EOK;
}

static ADSPResult vprx_get_kpps_cmd (void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   vprx_t* pVprx = (vprx_t*)pInstance;
   elite_msg_custom_kpps_type* kpps_msg_ptr = (elite_msg_custom_kpps_type*) pMsg->pPayload;
   vpm_get_kpps_ack_t* kpps_ptr = (vpm_get_kpps_ack_t*)kpps_msg_ptr->pnKpps;
   uint32_t kpps_changed=FALSE;

   if( kpps_ptr )
   {
      kpps_ptr->vprx_kpps = 0;  // Initialization
      nResult = vprx_aggregate_modules_kpps(pVprx, &kpps_changed);
      if(ADSP_SUCCEEDED(nResult))
      {
         kpps_ptr->vprx_kpps = pVprx->timing.modules_kpps;
      }
   }

   elite_svc_send_ack(pMsg, nResult );
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_get_kpps_cmd end nResult(%d) session(%lx)",nResult,pVprx->session.session_num);
   return nResult;
}

static ADSPResult vprx_get_delay_cmd (void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   vprx_t* pVprx = (vprx_t*)pInstance;
   elite_msg_custom_delay_type* delay_msg_ptr = (elite_msg_custom_delay_type*) pMsg->pPayload;
   vpm_get_delay_ack_t* delay_ptr = (vpm_get_delay_ack_t*)delay_msg_ptr->delay_ptr;

   if( delay_ptr )
   {
      delay_ptr->vprx_delay = 0;  // Initialization
      result = vprx_aggregate_modules_delay(pVprx);
      if(ADSP_SUCCEEDED(result))
      {
         delay_ptr->vprx_delay = pVprx->timing.modules_delay;
      }
   }
   else
   {
      result = ADSP_EBADPARAM;
   }

   elite_svc_send_ack(pMsg, result );
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_get_delay_cmd end result(%d) session(%lx)",result,pVprx->session.session_num);
   return result;
}

static ADSPResult vprx_unsupported_cmd (void* pInstance, elite_msg_any_t* pMsg)
{
   vprx_t* pVprx = (vprx_t*) pInstance;
   elite_svc_send_ack(pMsg, ADSP_EUNSUPPORTED);
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_unsupported_cmd message session(%lx)",pVprx->session.session_num);
   return ADSP_EOK;
}

static ADSPResult vprx_custom_msg( void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EFAILED;
   vprx_t* pVprx = (vprx_t*) pInstance;
   elite_msg_custom_header_t *pCustom = (elite_msg_custom_header_t *) pMsg->pPayload;

   if( pCustom->unSecOpCode < VOICEPROC_NUM_MSGS)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vprx_Cmd, SecOpcode: %ld session(%lx)",pCustom->unSecOpCode ,pVprx->session.session_num);
      if(TRUE == pVprx->fatal_state)
      {
         // accept only destroy or reconfig cmd when in fatal state.
         if (7 == pCustom->unSecOpCode)
         {
            nResult = pHandler[pCustom->unSecOpCode](pInstance, pMsg);
         }
         else
         {
            nResult = ADSP_EFAILED;
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vprx in fatal state!!result(%d) session(%x)",nResult,(int)pVprx->session.session_num);
            return nResult; 
         }
      }
      else
      {
         nResult = pHandler[pCustom->unSecOpCode](pInstance, pMsg);
      }
   }
   return nResult;
}

static ADSPResult vprx_apr_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = APR_EOK;
   ADSPResult apr_pkt_result = APR_EOK;
   vprx_t* pVprx = (vprx_t*) pInstance;
   elite_apr_packet_t * pAprPacket = (elite_apr_packet_t*) pMsg->pPayload;
   int32_t rc;

   //dbg: MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vprx_apr_cmd begin, opCode(%x) token(%x) session(%x)",
   //dbg:     elite_apr_if_get_opcode(pAprPacket),elite_apr_if_get_client_token(pAprPacket),pVprx->session.session_num);

   /* parse out the received pakcet.  Note in this current framework we are not prioritizing commands
      that can be completed immediately.  We are simply processing command in order they are received */
   switch(elite_apr_if_get_opcode(pAprPacket))
   {
      case VOICE_EVT_PUSH_HOST_BUF_V2:
         {
            ADSPResult local_result = APR_EOK;

            voice_evt_push_host_pcm_buf_v2_t *push_host_buf_ptr = (voice_evt_push_host_pcm_buf_v2_t *) elite_apr_if_get_payload_ptr(pAprPacket);

            // send PUSH_HOST_BUF event to the HPCM CAPI module
            if (NULL != pVprx->modules.special_capis[HPCM]->module_ptr)
            {
               capi_v2_buf_t param_data_buf;
               capi_v2_port_info_t port_info;
               port_info.is_valid = FALSE;
               param_data_buf.data_ptr = (int8_t *) push_host_buf_ptr;
               param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(fwk_extn_param_id_hpcm_push_host_buf_t);
               local_result = pVprx->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(pVprx->modules.special_capis[HPCM]->module_ptr,
                     FWK_EXTN_PARAM_ID_HPCM_PUSH_HOST_BUF, &port_info, &param_data_buf);
               local_result = capi_v2_err_to_adsp_result(local_result);
            if (ADSP_FAILED(local_result))
            {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! FWK_EXTN_PARAM_ID_HPCM_PUSH_HOST_BUF failed, result(%#x) session(%#lx)", \
                     local_result,pVprx->session.session_num);
            }
            }
            else
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vprx_apr_cmd() - HPCM CAPI module ptr is NULL, session(%lx)",pVprx->session.session_num);
            }

            // free the event since not done internally
            rc = elite_apr_if_free(pVprx->modules.apr_info_hpcm.apr_handle, pAprPacket);
            //dbg:MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx free apr pkt(%#x) handle(%#x)",pAprPacket,pVprx->modules.apr_info_hpcm.apr_handle); //DC:

            break;
         }
      default:
         {
            /* Handle error. */
            if (elite_apr_if_msg_type_is_cmd(pAprPacket))
            {  /* Complete unsupported commands. */
               apr_pkt_result = APR_EUNSUPPORTED;
               rc = elite_apr_if_end_cmd(pVprx->modules.apr_info_hpcm.apr_handle, pAprPacket, apr_pkt_result);
            }
            else
            {  /* Drop unsupported events. */
               rc = elite_apr_if_free(pVprx->modules.apr_info_hpcm.apr_handle, pAprPacket);
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx dropped apr evt, apr pkt ptr(%p) session(%#lx)", \
                     pAprPacket, pVprx->session.session_num);
            }
            break;
         }
   }

   if (ADSP_FAILED(rc))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! Elite APR message handler has returned error %d, continuing...", (int)rc);
   }

   if( ADSP_FAILED( nResult))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_apr_cmd end, apr result(%#x) fn result(%#x) session(%#lx)", \
            apr_pkt_result, nResult, pVprx->session.session_num);
   }
   return nResult;
}

static ADSPResult vprx_config_host_pcm(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK, apr_result = APR_EOK;
   ADSPResult apr_pkt_result = APR_EOK;
   vprx_t* pVprx = (vprx_t*) pInstance;
   elite_msg_custom_voc_config_host_pcm_type *pConfig = (elite_msg_custom_voc_config_host_pcm_type *) pMsg->pPayload;
   elite_apr_packet_t *apr_packet_ptr = pConfig->apr_packet_ptr;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_config_host_pcm begin session(%x)\n",pVprx->session.session_num);

   // currently we only allow one tap point, so there is no need to manage multiple tap points.

   // check if HPCM module is present in topology
   if (NULL == pVprx->modules.special_capis[HPCM])
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! HPCM Module is not present in the topology, session(%lx)",pVprx->session.session_num);
      nResult = ADSP_EFAILED;
      apr_pkt_result=APR_EUNSUPPORTED;
      elite_svc_send_ack(pMsg, apr_pkt_result);
      return nResult;
   }


   if( VOICE_CMD_START_HOST_PCM_V2 == elite_apr_if_get_opcode( apr_packet_ptr))
   {
      {
         voice_start_host_pcm_v2_t    *start_host_pcm_ptr = (voice_start_host_pcm_v2_t *) elite_apr_if_get_payload_ptr( apr_packet_ptr);

         uint8 index;

         // check if default Rx tap point in the list
         for( index = 0; index < start_host_pcm_ptr->num_tap_points; index++)
         {
            if( VOICEPROC_MODULE_RX == start_host_pcm_ptr->tap_points[index].tap_point )
            {
               break;
            }
         }

         // check if valid index found and the tap point is off
         if( index < start_host_pcm_ptr->num_tap_points)
         {
            voice_tap_point_v2_t *voice_tap_point_ptr = &start_host_pcm_ptr->tap_points[index];
            if(NULL == voice_tap_point_ptr->mem_map_handle)
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Vprx received NULL mem map handle!");
               nResult = ADSP_EBADPARAM;
            }
            else
            {
               /* copy Host Pcm APR info  */
               pVprx->modules.apr_info_hpcm.apr_handle        = pConfig->apr_handle;
               pVprx->modules.apr_info_hpcm.self_addr         = elite_apr_if_get_dst_addr( apr_packet_ptr);
               pVprx->modules.apr_info_hpcm.self_port         = elite_apr_if_get_dst_port( apr_packet_ptr);
               pVprx->modules.apr_info_hpcm.client_addr       = elite_apr_if_get_src_addr( apr_packet_ptr);
               pVprx->modules.apr_info_hpcm.client_port       = elite_apr_if_get_src_port( apr_packet_ptr);

               MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx setting hpcm context, memmmap handle (%x), client addr (%x), client port (%x)", (unsigned int)voice_tap_point_ptr->mem_map_handle, pVprx->modules.apr_info_hpcm.client_addr, pVprx->modules.apr_info_hpcm.client_port);

               // send HPCM start to HPCM CAPI module
               if (NULL != pVprx->modules.special_capis[HPCM]->module_ptr)
               {
                  capi_v2_buf_t param_data_buf;
                  capi_v2_port_info_t port_info;
                  port_info.is_valid = FALSE;
                  param_data_buf.data_ptr = (int8_t *) voice_tap_point_ptr;
                  param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(voice_tap_point_v2_t);
                  nResult = pVprx->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(pVprx->modules.special_capis[HPCM]->module_ptr,
                        FWK_EXTN_PARAM_ID_HPCM_START, &port_info, &param_data_buf);
                  nResult = capi_v2_err_to_adsp_result(nResult);
                  if(ADSP_FAILED(nResult))
                  {
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! HPCM_START set param to CAPI failed!!");
                  }
                  else
                  {
                     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: HPCM_START set param to CAPI successful!!");
                     // cache HPCM tap-point struct to re-apply when necessary such as Vprx reconfig
                     memscpy(&pVprx->modules.hpcm_tap_point, sizeof(voice_tap_point_v2_t), voice_tap_point_ptr, sizeof(voice_tap_point_v2_t));
                     pVprx->modules.hpcm_enable_flag = TRUE;
                  }
               }
               else
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vprx_config_host_pcm() - HPCM CAPI module ptr is NULL, session(%lx)",pVprx->session.session_num);
               }
            }
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_config_host_pcm begin START command NOP session(%lx)\n",pVprx->session.session_num);
         }
      }
   }
   else if( VOICE_CMD_STOP_HOST_PCM == elite_apr_if_get_opcode( apr_packet_ptr))
   {
      // send HPCM stop to HPCM CAPI module
      if (NULL != pVprx->modules.special_capis[HPCM]->module_ptr)
      {
         capi_v2_buf_t param_data_buf;
         capi_v2_port_info_t port_info;
         port_info.is_valid = FALSE;
         param_data_buf.data_ptr = NULL; // no payload is needed
         param_data_buf.actual_data_len = param_data_buf.max_data_len = 0;
         nResult = pVprx->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(pVprx->modules.special_capis[HPCM]->module_ptr,
               FWK_EXTN_PARAM_ID_HPCM_STOP, &port_info, &param_data_buf);
         nResult = capi_v2_err_to_adsp_result(nResult);
         if(ADSP_FAILED(nResult))
         {
             MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! HPCM_STOP set param to CAPI failed!!");
         }
         pVprx->modules.hpcm_enable_flag = FALSE;
         memset(&pVprx->modules.hpcm_tap_point,0,sizeof(voice_tap_point_v2_t));
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vprx_config_host_pcm() - HPCM CAPI module ptr is NULL, session(%lx)",pVprx->session.session_num);
      }
   }
   else
   {
      nResult = ADSP_EFAILED;
      apr_result = APR_EBADPARAM;
   }


   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_config_host_pcm end result(%d) session(%x)\n",nResult,pVprx->session.session_num);

   elite_svc_send_ack(pMsg, apr_result);
   return nResult;
}

static void vprx_set_media_type(vprx_t* pVprx, vprx_create_params_t *create_param_ptr)
{
   // Following init should be based on the Topology id from the VPM

   if(1 != create_param_ptr->num_channels)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VPRx ouput num_channels = %d. Ignoring session(%x)\n",
            create_param_ptr->num_channels, pVprx->session.session_num);
   }

   pVprx->io.input.mediatype.num_channels    = 1;                     // single mic implementation
   pVprx->io.input.mediatype.bits_per_sample = 16;                    // 16 bit word
   pVprx->io.input.mediatype.sample_rate     = pVprx->session.in_sampling_rate;  // 8k by default
   pVprx->io.input.mediatype.is_signed       = 1;                     // signed input
   pVprx->io.input.mediatype.is_interleaved  = 0;                     // non-interleaved

   if(voice_is_channel_map_valid(&create_param_ptr->channel_map[0]))
   {
      pVprx->io.input.mediatype.channel_mapping[0] = create_param_ptr->channel_map[0];
   }
   else
   {
      pVprx->io.input.mediatype.channel_mapping[0] = PCM_CHANNEL_C;
   }

   pVprx->io.output.mediatype.num_channels    = 1;                     //always single channel
   pVprx->io.output.mediatype.bits_per_sample = 16;                    // 16 bit word
   pVprx->io.output.mediatype.sample_rate     = pVprx->session.in_sampling_rate;  // 8k by default
   pVprx->io.output.mediatype.is_signed       = 1;                     // signed input
   pVprx->io.output.mediatype.is_interleaved  = 0;                     // non-interleaved
   pVprx->io.output.mediatype.channel_mapping[0] = pVprx->io.input.mediatype.channel_mapping[0];  //will be overwritten during media format propagation

}

static ADSPResult vprx_out_buf_allocator(vprx_t* pVprx, uint32_t nOutCircBufSamples)
{
   ADSPResult nResult = ADSP_EOK;
   int32_t nOutBufSize = (nOutCircBufSamples *pVprx->io.output.mediatype.num_channels) << 1; // samples to bytes conversion

   //dbg: MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vprx_out_buf_allocator begin. num_output_channels(%lu),nOutBufSize(%ld), session(%x)",
   //      pVprx->io.output.mediatype.num_channels,nOutBufSize,pVprx->session.session_num);

   for (uint32_t i = 0; i < MAX_BUF_Q_ELEMENTS; i++)
   {
      //allocate the databuffer payload (metadata + pcm buffer size)
      int32_t nReqSize = GET_ELITEMSG_DATABUF_REQ_SIZE(nOutBufSize);
      elite_msg_data_buffer_t* pDataPayload = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc(nReqSize, QURT_ELITE_HEAP_DEFAULT);

      if (!pDataPayload)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to allocate memory for output data buffer!! session(%lx)",pVprx->session.session_num);
         return ADSP_ENOMEMORY;
      }

      pDataPayload->nActualSize = 0;           // No data in buffer by default
      pDataPayload->nMaxSize    = nOutBufSize;
      pVprx->io.output.bufs_allocated++;

      if (ADSP_FAILED(nResult = elite_msg_push_payload_to_returnq(pVprx->io.output.bufq_ptr, (elite_msg_any_payload_t*) pDataPayload)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed fill Vprx buffer queue!! session(%lx)",pVprx->session.session_num);
         qurt_elite_memory_free(pDataPayload);
         return nResult;
      }
   }
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vprx_out_buf_allocator end nResult=%x session(%x)",nResult,pVprx->session.session_num);
   return nResult;
}

static void voice_result_check(ADSPResult nResult, uint32_t session_num)
{
   if (ADSP_EOK != nResult)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx operation error nResult %d, session(%lx)",nResult,session_num);
   }
}

static ADSPResult voice_check_media_t(elite_multi_channel_pcm_fmt_blk_t* pRefType, elite_multi_channel_pcm_fmt_blk_t* pMediaFormat)
{
   if (     (pRefType->sample_rate     != pMediaFormat->sample_rate)
         || (pRefType->num_channels    != pMediaFormat->num_channels)
         || (pRefType->bits_per_sample != pMediaFormat->bits_per_sample)
         || (pRefType->is_interleaved  != pMediaFormat->is_interleaved)
         || (pRefType->is_signed       != pMediaFormat->is_signed)
      )
   {
      return ADSP_EFAILED;
   }
   else
   {
      return ADSP_EOK;
   }
}

void vprx_cal_drift(vprx_t * pVprx, int32_t *inst_drift)
{
   // Conversion from VFR rate to Codec rate
   voice_calc_inst_drift(&pVprx->ratematch.voice_drift_info,
         pVprx->afe_info.afe_drift_ptr,
         pVprx->session.vfr_source,
         VOICEPROC_RX,
         pVprx->session.vfr_mode,
         pVprx->session.session_num,
         pVprx->timing.timing_ver,
         pVprx->timing.vsid,
         inst_drift);

      //discard the drift if resync is detected
      if(pVprx->is_resync_detected)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VPRx resync is detected, ignoring inst drift. session(%x)", (int)pVprx->session.session_num);
         *inst_drift = 0;
         pVprx->is_resync_detected = FALSE;
      }
}

static void vprx_vtm_unsubscribe(void *pInstance,Vtm_SubUnsubMsg_t *unsubscribe_ptr, uint32_t mask)
{
   vprx_t* pVprx = (vprx_t*) pInstance;
   ADSPResult nResult;
   if ((NULL != unsubscribe_ptr) && (NULL != pInstance))
   {
      if (ADSP_FAILED(nResult = voice_custom_vt_sub_unsub_msg_send(NULL,pVprx->timing.vtm_cmdq_ptr,VOICE_TIMER_UNSUBSCRIBE,NULL,FALSE,unsubscribe_ptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vprx unsubscribe to Vtm session(%x)",(int)pVprx->session.session_num);
         return; // return without clearing data, fatal msg -> to help with sim debugging
      }
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Waiting for Vprx unsubscribe to Vtm, session(%x)",(int)pVprx->session.session_num);
      // wait syncronously for unsubscribing
      (void)qurt_elite_channel_wait(&pVprx->qurt_elite_channel, mask);
      qurt_elite_signal_clear(unsubscribe_ptr->signal_end_ptr);
      unsubscribe_ptr->offset         = 0;
      unsubscribe_ptr->signal_enable  = 0;
   }
}

static ADSPResult vprx_is_supported(uint32_t topology_id, uint32_t sampling_rate, uint32_t session_num, uint16_t afe_rx_port_id, uint32_t num_channels)
{
   if(1 != num_channels)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_create Invalid num of channels(%ld), session(%lx)",num_channels,session_num);
      return ADSP_EUNSUPPORTED;
   }

   switch (sampling_rate)
   {
      case(VOICE_NB_SAMPLING_RATE):
      case(VOICE_WB_SAMPLING_RATE):
      case(VOICE_FB_SAMPLING_RATE):
      case(VOICE_SWB_SAMPLING_RATE):
         {
            break; // all is dandy
         }
      default:
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_create Invalid Samping Rate(%ld), session(%lx)",sampling_rate,session_num);
            return ADSP_EUNSUPPORTED;
            break;
         }
   }

   if(vprx_is_custom_topology(topology_id))
   {
       //custom topology case.
       MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vprx_create custom Toplogy (%lx), session(%lx)",topology_id,session_num);
   }

   return ADSP_EOK;
}

static void vprx_calc_offsets(void* pInstance)
{
   vprx_t* pVprx = (vprx_t*)pInstance;
   int32 delivery_offset = 0;
   uint32_t vdec_processing_time = 0;
   uint32_t vdec_kpps = 0;
   uint32_t vprx_processing_time = 0;

   // delivery_offset calculation
   vdec_calc_processing_time(pVprx->timing.session_thread_clock_mhz, pVprx->session.in_sampling_rate, &vdec_processing_time, pVprx->session.vfr_mode); // find out the processing time in microsecs for vdec+vdec_pp
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx: Vdec clock_mhz(%lu) kpps(%u), vdec processing time per frame:(%d), session(%x)",
         pVprx->timing.session_thread_clock_mhz, (unsigned int)vdec_kpps, (int)vdec_processing_time, (int)pVprx->session.session_num);

   // sync_offset calculation
   vcmn_calc_processing_time(pVprx->timing.session_thread_clock_mhz, pVprx->timing.modules_kpps, 20, &vprx_processing_time); // find out the processing time in microsecs for vprx
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx modules kpps(%d), Vprx processing time per frame:(%d), session(%x)", (int)pVprx->timing.modules_kpps, (int)vprx_processing_time, (int)pVprx->session.session_num);

   if(VPRX_MAX_PROCESSING_TIME_IN_US < vprx_processing_time)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Vprx processing time too high session(%x)", (int)pVprx->session.session_num);
      vprx_processing_time = 19000; //this is just a value that's hopefully high enough
   }

   delivery_offset = pVprx->timing.dec_offset + vdec_processing_time + vprx_processing_time + VPRX_PROCESSING_MARGIN_IN_US; //add margin
   delivery_offset = ((20000 < delivery_offset) ? (delivery_offset - 20000) : delivery_offset); // Mod20 operation for 20ms encoder frame
   delivery_offset = ((20000 < delivery_offset) ? (delivery_offset - 20000) : delivery_offset); // Mod20 operation for 20ms encoder frame

   pVprx->timing.delivery_offset = delivery_offset;

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx offset calculation, dsp_clock(%d), vdec_kpps(%d), delivery_offset(%d),  dec_offset(%d), session(%x)",
         (int)pVprx->timing.session_thread_clock_mhz, (int)vdec_kpps, (int)pVprx->timing.delivery_offset, (int)pVprx->timing.dec_offset, (int)pVprx->session.session_num);

}

static ADSPResult vprx_vds_send_resync(void* pInstance)
{
   vprx_t* pVprx = (vprx_t*)pInstance;
   ADSPResult result;
   elite_msg_any_t msg;
   vds_resync_t* resync_params_ptr;
   uint32_t msg_size = sizeof(elite_msg_custom_vds_resync_type);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx VDS resync, session(%x)", (int)pVprx->session.session_num);
   result = elite_msg_create_msg(&msg,
         &msg_size,
         ELITE_CUSTOM_MSG,
         pVprx->respq_ptr,
         NULL,
         ADSP_EOK);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vprx failed to create message, result %d, session(%x)", result, (int)pVprx->session.session_num);
      return result;
   }
   elite_msg_custom_vds_resync_type *vds_resync_msg = (elite_msg_custom_vds_resync_type*)msg.pPayload;
   resync_params_ptr = &vds_resync_msg->resync_params;
   vds_resync_msg->unSecOpCode = VDS_RESYNC;
   resync_params_ptr->client_token = pVprx->timing.vds_client_token;
   // Populate this value regardless of subscribe/unsubscribe. Subscribe will just ignore it anyway
   resync_params_ptr->client_id = pVprx->timing.vds_client_id;
   // Issue command
   result = qurt_elite_queue_push_back(pVprx->timing.vds_handle_ptr->cmdQ, (uint64_t*)&msg);
   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vprx failed to deliver command, result %d, session(%x)", result, (int)pVprx->session.session_num);
      elite_msg_return_payload_buffer(&msg);
      return result;
   }
   // Wait for ack
   result = elite_svc_wait_for_ack(&msg);
   if(ADSP_FAILED(result) || ADSP_FAILED(vds_resync_msg->unResponseResult))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vprx wait failed, result %d, session(%x)", result, (int)pVprx->session.session_num);
      elite_msg_return_payload_buffer(&msg);
      return ADSP_EFAILED;
   }

   // Return buffer
   elite_msg_return_payload_buffer(&msg);

   return result;
}

static ADSPResult vdec_calc_processing_time(uint32_t dsp_clock_mhz, uint32_t sampling_rate, uint32_t* processing_time, uint16_t vfr_mode)
{
   uint32_t sampling_rate_index=0;
   uint32_t clock_index=0;

   if (NULL != processing_time)
   {
      // Get Sampling Rate index
      if (VOICE_NB_SAMPLING_RATE == sampling_rate)
      {
         sampling_rate_index=0;
      }
      else if (VOICE_WB_SAMPLING_RATE == sampling_rate)
      {
         sampling_rate_index=1;
      }
      else
      {
         sampling_rate_index=2;
      }

      // Get Clock index
      if (VOICE_PER_THREAD_MINCLK == dsp_clock_mhz)
      {
         clock_index=0;
      }
      else if (VOICE_PER_THREAD_MIDCLK == dsp_clock_mhz)
      {
         clock_index=1;
      }
      else // MAXCLK
      {
         clock_index=2;
      }

      if (VFR_NONE == vfr_mode)
      {
         *processing_time = (uint32_t) voice_rxstream_proc_time_novfr[sampling_rate_index][clock_index] * VOICE_CACHE_MISS_FACTOR_OVER_1MHZ/1000;
      }
      else
      {
         *processing_time = (uint32_t) voice_rxstream_proc_time[sampling_rate_index][clock_index] * VOICE_CACHE_MISS_FACTOR_OVER_1MHZ/1000;
      }
      return ADSP_EOK;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice cmn: NULL pointer for processing timer");
      return ADSP_EBADPARAM;
   }
}

static ADSPResult vprx_set_timingv3_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   vprx_t* pVprx = (vprx_t*)pInstance;
   elite_msg_custom_voc_timing_param_type *pSetTimingCmd = (elite_msg_custom_voc_timing_param_type *) pMsg->pPayload;
   vpm_set_timing_params_v2_t* pVfrCmd = (vpm_set_timing_params_v2_t*)pSetTimingCmd->param_data_ptr;

   // Check for Run state.
   if (FALSE == pVprx->process_data_flag)
   {
      //dbg msg is printed in VPM with all params
      /*MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vprx_set_timingv3_cmd, mode(%d), VSID(0x%lx), \
        vptx_delivery(%d),session_index(%x)",
        pVfrCmd->mode,pVfrCmd->vsid,pVfrCmd->vprx_delivery_offset,
        pVprx->session.session_num); */

      // Verify validity of VFR mode. In this version of timing cmd, mode supports only two values - VFR_NONE and VFR_HARD
      // If VFR_HARD, further information is derived from VSID
      if ((VFR_HARD >= pVfrCmd->mode) )
      {
         pVprx->session.vfr_mode = pVfrCmd->mode;
      }
      else
      {
         pVprx->session.vfr_mode = VFR_NONE;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_set_timingv3_cmd, invalid mode(%x),setting to VFR_NONE,session(%lx)",pVfrCmd->mode,pVprx->session.session_num);
         nResult = ADSP_EBADPARAM;
      }

      pVprx->session.vfr_cycle = pVfrCmd->vfr_cycle/MAX_TIMER_OFFSET;

      // Verify validity of VSID. In VFR_HARD case, VSID should be non-zero. In VFR_NONE case, VSID is don't care.
      if ((VFR_HARD == pVprx->session.vfr_mode) && (0 == pVfrCmd->vsid))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_set_timingv3_cmd, invalid VSID(%lx), session(%lx)",pVfrCmd->vsid,pVprx->session.session_num);
         nResult = ADSP_EBADPARAM;
      }
      else
      {
         pVprx->timing.vsid = pVfrCmd->vsid;
      }

      // Verify validity of delivery offset
      // MIN_TIMER_OFFSET is set to 0, so no need to check if offset is below min because it's unsigned
      if (pVfrCmd->vfr_cycle < pVfrCmd->vprx_delivery_offset)
      {
         pVprx->timing.delivery_offset = 7300; // default to max offset (19ms for safety) from default decode offset (8300)
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx_set_timingv3_cmd, Invalid delivery(%d) offset, defaulting to 7.3ms, session(%lx)", \
               pVfrCmd->vprx_delivery_offset,pVprx->session.session_num);
         nResult = ADSP_EBADPARAM;
      }
      else
      {
         pVprx->timing.delivery_offset = pVfrCmd->vprx_delivery_offset;
      }
      pVprx->timing.timing_ver = VFR_CLIENT_INFO_VER_2; // timing with VSID
      pVprx->timing.use_new_timing_flag = TRUE;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Timing can't be changed in RUN session(%lx)",pVprx->session.session_num);
      nResult = ADSP_EBUSY;
   }

   elite_svc_send_ack(pMsg, nResult);
   return ADSP_EOK;
}


void vprx_calibration_cb_func(void* cal_params_payload_ptr, void* cb_data)
{
   ADSPResult result = ADSP_EOK;
   vprx_t* pVprx = (vprx_t*)cb_data;
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

   result = vprx_modules_set_param_int(pVprx, mod_id, instance_id, param_id, param_data, param_size);
   if(ADSP_EOK != result && ADSP_EUNSUPPORTED != result)
   {
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vprx set param error %u, mod %lx,instance %lx, param %lx, session(%lx)",
          result, mod_id, instance_id,param_id, pVprx->session.session_num);
   }
   return;
}

static ADSPResult vprx_set_param_v3_cmd (void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   vprx_t* pVprx = (vprx_t*)pInstance;
   elite_msg_custom_set_param_v3_type *payload = (elite_msg_custom_set_param_v3_type*)pMsg->pPayload;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vprx set_param_v3, handle %lx, session(%lx)", payload->cal_handle, pVprx->session.session_num);

   if (VPM_RX_NONE == pVprx->session.topology_id)  // no calibration support in none topology
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: calibration not supported in none topology, returning. session(%lx)\n",pVprx->session.session_num);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return result;
   }

   //call into mvm using mvm_call as an entry point to cvd_cal_query
   vss_imvm_cmd_cal_query_t mvm_payload;
   mvm_payload.query_handle = payload->cal_handle;
   mvm_payload.cb_fn = vprx_calibration_cb_func;
   mvm_payload.client_data = pInstance;

   result = mvm_call(MVM_CMDID_CAL_QUERY, &mvm_payload, sizeof(mvm_payload));

   elite_svc_send_ack(pMsg, result);
   return result;
}
