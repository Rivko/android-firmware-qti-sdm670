/*========================================================================

*//** @file Vptx_Svc.cpp

Copyright (c) 2009-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the code for Voice Proc Tx(VPTX) Dynamic service. The
VPTX dynamic service has one thread. It receives commands from Voice Proc
Manager (VPM). VPTX initializes the Pre-Processing algorithms and
processes the data for encoder stream service. Multiple VPTX can be
instantiated and each of them is identified by a session number provided
while creating VPTX.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/src/Vptx_Svc.cpp#5 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/29/09   dc     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Vptx_Svc.h"
#include "voice_delivery.h"

/* -----------------------------------------------------------------------
** extern declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Macros Definitions
** ----------------------------------------------------------------------- */
#define VPTX_SYNC_TICK_MASK               0x80000000
#define VPTX_PROC_TICK_MASK               0x40000000
#define VPTX_VENC_DELIVERY_TICK_MASK      0x20000000
#define VPTX_RESYNC_MASK                  0x10000000
#define VPTX_CMD_MASK                     0x08000000
#define VPTX_BUF_MASK                     0x04000000
#define VPTX_BUF_LB_MASK                  0x02000000
#define VPTX_NEAR_DATA_MASK               0x01000000
#define VPTX_FAR_DATA_MASK                0x00800000
#define VPTX_SYNC_TICK_END_MASK           0x00400000
#define VPTX_PROC_TICK_END_MASK           0x00200000
#define VPTX_DELIVERY_TICK_END_MASK       0x00100000
#define VPTX_VENC_DELIVERY_TICK_END_MASK  0x00080000
#define VPTX_RESPONSE_MASK                0x00040000
#define VPTX_QMI_REG_MASK                 0x00020000
#define VPTX_QMI_IND_MASK                 0x00010000
#define CMD_DATA_MASK                     (VPTX_CMD_MASK | VPTX_NEAR_DATA_MASK | VPTX_FAR_DATA_MASK)
#define CMD_BUF_DATA_MASK                 (VPTX_CMD_MASK | VPTX_BUF_MASK | VPTX_NEAR_DATA_MASK | VPTX_FAR_DATA_MASK)
#define CMD_BUF_MASK                      (VPTX_CMD_MASK | VPTX_BUF_MASK )
#define REF_TIMER_TICK_MASK               (VPTX_SYNC_TICK_MASK | VPTX_PROC_TICK_MASK | VPTX_RESYNC_MASK | VPTX_VENC_DELIVERY_TICK_MASK)
#define ALL_MASK                          (VPTX_SYNC_TICK_MASK | VPTX_PROC_TICK_MASK | VPTX_SYNC_TICK_END_MASK | VPTX_PROC_TICK_END_MASK | VPTX_CMD_MASK | VPTX_BUF_MASK | VPTX_NEAR_DATA_MASK | VPTX_FAR_DATA_MASK | VPTX_BUF_LB_MASK | VPTX_RESYNC_MASK | VPTX_DELIVERY_TICK_END_MASK | VPTX_VENC_DELIVERY_TICK_MASK | VPTX_VENC_DELIVERY_TICK_END_MASK)

// TODO: Remove when API updated
/**  Parameter for enabling internal loopback feature for a particular session.
     Fixed size of LOOPBACK_ENABLE_PARAM_BYTES = 4 bytes.
 */

#define SET_MASK(MASK,FLAG) (MASK) = (FLAG) | (REF_TIMER_TICK_MASK)
#define OR_MASK(MASK,FLAG) ((MASK) |= (FLAG))
#define NOT_MASK(MASK,FLAG) ((MASK) &= ~(FLAG))

// Need to keep a minimum margin between sync and delivery offsets, currently kept as 2.5 ms
#define VPTX_MIN_PROCESSING_MARGIN_IN_US 2500

// VPTX processing time cannot exceed 20 ms
#define VPTX_MAX_PROCESSING_TIME_IN_US 20000

// Need to check whether or not to have vptx delivery tick
// If vptx+venc processing time is greater than this macro then vptx will deliver at vptx delivery tick, else it will be push based
#define MAX_TX_PROCESSING_TIME_IN_US       (2*MAX_TIMER_OFFSET - 1000)           

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)            (((a) > (b)) ? (b) : (a))
#endif
/* -----------------------------------------------------------------------
 ** Constant Declarations
 ** ----------------------------------------------------------------------- */
// maximum data messages in data queue. Must consider both far and near data buffers
// For 20ms processing frame if vptx takes 20ms to process, atleast 20 elements are required
// such that the some buffers are not lost
static const uint32_t MAX_DATA_Q_ELEMENTS = 32;

// maximum number of commands expected ever in command queue.
static const uint32_t MAX_CMD_Q_ELEMENTS = 8;

// maximum number of commands expected ever in response queue.
static const uint32_t MAX_RESP_Q_ELEMENTS = 2;

// How many buffers in output buffer queue? May need to make this configurable...
// Need 2 buffers for vptx for longer CDRX cycles, since for 60 ms, vptx can finish quickly (none topo),
// but enc can take longer to process, so it won't return buffers to matrix output port.
// this means matrix won't consume input buffer to return to vptx and vptx will end up dropping output
static const uint32_t MAX_BUF_Q_ELEMENTS = 2;

// Thread Name
static char_t aTHREAD_NAME[6] = {'V','P','T','$','\0'};

// Thread Stack size -  defined in config
extern const unsigned int VPTX_STACK_SIZE;

/* -----------------------------------------------------------------------
 ** enums
 ** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */
// service create functions
static ADSPResult vptx_svc_create (void **near_handle,
        void **far_handle,
        int8_t voice_or_audio,
        vptx_create_params_v2_t* create_param_ptr,
        vptx_vpm_apr_info_t *apr_info_ptr
        );

// destructor
static void vptx_destroy(vptx_t* pVptx);

// Main work loop for service thread. Pulls msgs off of queues and processes them.
static ADSPResult vptx_thread(void* pInstance);
static ADSPResult vptx_thread_entry(void* pInstance);

// message handler functions
static ADSPResult vptx_connect_dwn_stream_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_disconnect_dwn_stream_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_reconfig_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_run_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_stop_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_set_param_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_get_param_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_destroy_yourself_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_connect_rx_peer_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_disconnect_rx_peer_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_custom_msg( void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_set_mute_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_set_timing_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_set_timingv2_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_set_timingv3_cmd(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_config_host_pcm(void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_apr_cmd(void* pInstance, elite_msg_any_t* pMsg);

// state handler functions
static ADSPResult vptx_sync_tick_handler(void* pInstance);
static ADSPResult vptx_proc_tick_handler(void* pInstance);
static ADSPResult vptx_resync_handler(void* pInstance);
static ADSPResult vptx_sync_tick_end_handler(void* pInstance);
static ADSPResult vptx_proc_tick_end_handler(void* pInstance);
static ADSPResult vptx_delivery_tick_end_handler(void* pInstance);
static ADSPResult vptx_cmd_handler(void* pInstance);
static ADSPResult vptx_buf_handler(void* pInstance);
static ADSPResult vptx_loopback_buf_handler(void* pInstance);
static ADSPResult vptx_data_handler(void* pInstance);
static ADSPResult vptx_response_handler(void* pInstance);
static ADSPResult vptx_venc_delivery_tick_handler(void* pInstance);
static ADSPResult vptx_venc_delivery_tick_end_handler(void* pInstance);


static ADSPResult vptx_qmi_reg_handler(void* pInstance);
static ADSPResult vptx_qmi_ind_handler(void* pInstance);

// Memory management related function
static ADSPResult vptx_out_buf_allocator(vptx_t* pVptx, uint32_t out_buf_samples);

// Initialization and End functions
static ADSPResult vptx_init(vptx_t *pVptx, vptx_create_params_v2_t* create_param_ptr);
static void vptx_end(vptx_t *pVptx);

// Media type related functions
static ADSPResult vptx_set_media_type(vptx_t* pVptx);
static ADSPResult Voice_CheckMediaType(elite_multi_channel_pcm_fmt_blk_t* pRefType, elite_multi_channel_pcm_fmt_blk_t* pMediaFormat);

// Error check functions
static void voice_result_check(ADSPResult nResult, uint32_t session_num);

// Utility functions
static void vptx_vtm_unsubscribe(void *pInstance,Vtm_SubUnsubMsg_t *unsubscribe_ptr, uint32_t mask);
static void vptx_reinit_circbuf(void *pInstance);
static ADSPResult vptx_is_supported(uint32_t topology_id, uint32_t sampling_rate, uint32_t session_num, uint16_t afe_tx_port_id, uint32_t num_channels, uint32_t num_ref_channels);
static void vptx_clear_input_dataq(void* pInstance);
static void vptx_resync(void* pInstance);
static void vptx_far_resync(void* pInstance);
static void vptx_calc_offsets(void* pInstance);

// Init the control flags and variables in vptx so that vptx is ready to run (eg: data sync flags, even odd phase flags, samples slip counters, etc..)
static void vptx_ctrl_params_init(vptx_t* pVptx);
static ADSPResult venc_calc_processing_time(uint32_t dsp_clock_mhz, uint32_t sampling_rate, uint32_t* processing_time);
static ADSPResult vptx_get_kpps_cmd (void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_get_delay_cmd (void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_set_param_v3_cmd (void* pInstance, elite_msg_any_t* pMsg);
static ADSPResult vptx_reg_event_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vptx_unreg_event_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr);
static ADSPResult vptx_set_ep_id(void* instance_ptr, elite_msg_any_t* msg_ptr);


/* -----------------------------------------------------------------------
 ** Message handler
 ** ----------------------------------------------------------------------- */

static elite_svc_msg_handler_func pHandler[] =
{
   vptx_connect_dwn_stream_cmd,    // - VOICEPROC_CONNECT_DWN_STREAM_CMD
   vptx_disconnect_dwn_stream_cmd, // - VOICEPROC_DISCONNECT_DWN_STREAM_CMD
   vptx_reconfig_cmd,              // - VOICEPROC_RECONFIG_CMD
   vptx_run_cmd,                   // - VOICEPROC_OPEN_CMD
   vptx_stop_cmd,                  // - VOICEPROC_CLOSE_PARAM_CMD
   vptx_set_param_cmd,             // - VOICEPROC_SET_PARAM_CMD
   vptx_get_param_cmd,             // - VOICEPROC_GET_PARAM_CMD
   vptx_destroy_yourself_cmd,      // - VOICEPROC_DESTROY_YOURSELF_CMD
   vptx_connect_rx_peer_cmd,       // - VOICEPROC_CONNECT_RX_PEER
   vptx_disconnect_rx_peer_cmd,    // - VOICEPROC_DISCONNECT_RX_PEER
   vptx_set_mute_cmd,              // - VOICEPROC_SET_MUTE_CMD
   vptx_config_host_pcm,           // - VOICEPROC_CONFIG_HOST_PCM
   vptx_set_timing_cmd,            // - VOICEPROC_SET_TIMING_CMD
   vptx_set_timingv2_cmd,          // - VOICEPROC_SET_TIMINGV2_CMD
   vptx_get_kpps_cmd,              // - VOICEPROC_GET_KPPS_CMD
   vptx_set_timingv3_cmd,          // - VOICEPROC_SET_TIMINGV3_CMD
   vptx_get_delay_cmd,             // - VOICEPROC_GET_DELAY_CMD
   vptx_set_param_v3_cmd,              // - VOICEPROC_SET_PARAM_V3_CMD
   vptx_reg_event_cmd,             // - VOICEPROC_REGISTER_EVENT
   vptx_unreg_event_cmd,           // - VOICEPROC_UNREGISTER_EVENT
   vptx_set_ep_id,           // - VOICEPROC_UNREGISTER_EVENT
};

static elite_svc_msg_handler_func pEliteHandler[] =
{
   vptx_custom_msg,                //0  - ELITE_CUSTOM_MSG
   vptx_run_cmd,                   //1  - ELITE_CMD_START_SERVICE
   vptx_destroy_yourself_cmd,      //2  - ELITE_CMD_DESTROY_SERVICE
   vptx_connect_dwn_stream_cmd,    //3  - ELITE_CMD_CONNECT
   vptx_disconnect_dwn_stream_cmd, //4  - ELITE_CMD_DISCONNECT
   elite_svc_unsupported,          //5  - ELITE_CMD_PAUSE
   elite_svc_unsupported,          //6  - ELITE_CMD_RESUME
   elite_svc_unsupported,          //7  - ELITE_CMD_FLUSH
   vptx_set_param_cmd,             //8  - ELITE_CMD_SET_PARAM
   vptx_get_param_cmd,             //9  - ELITE_CMD_GET_PARAM
   elite_svc_unsupported,          //10 - ELITE_DATA_BUFFER
   elite_svc_unsupported,          //11 - ELITE_DATA_MEDIA_TYPE
   elite_svc_unsupported,          //12 - ELITE_DATA_EOS
   elite_svc_unsupported,          //13 - ELITE_DATA_RAW_BUFFER
   vptx_stop_cmd,                  //14 - ELITE_CMD_STOP_SERVICE
   vptx_apr_cmd                    //15 - ELITE_APR_PACKET_OPCODE
};

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */
static ADSPResult (*pVptxHandler[])(void *) =
{
   vptx_sync_tick_handler,
   vptx_proc_tick_handler,
   vptx_venc_delivery_tick_handler,
   vptx_resync_handler,
   vptx_cmd_handler,
   vptx_buf_handler,
   vptx_loopback_buf_handler,
   vptx_data_handler, // Near data
   vptx_data_handler, // Far data
   vptx_sync_tick_end_handler,
   vptx_proc_tick_end_handler,
   vptx_delivery_tick_end_handler,
   vptx_venc_delivery_tick_end_handler,
   vptx_response_handler,             // should never get invoked
   vptx_qmi_reg_handler,              // Invoked when Registration callback signals Vptx .
   vptx_qmi_ind_handler               // Invoked when Sensors indication, callback signals Vptx.

};

/*
 * This function updates channel map. if the channel map is not received from client, default it to
 * mic path channel maps.
 */
static ADSPResult vptx_update_channel_map(vptx_t *pVptx, vptx_create_params_v2_t *create_param_ptr)
{
   ADSPResult result = ADSP_EOK;

   uint8_t mic_channel_map[QUAD] = { PCM_CHANNEL_PRI_MIC, PCM_CHANNEL_SEC_MIC_1,
                                     PCM_CHANNEL_SEC_MIC_2, PCM_CHANNEL_SEC_MIC_3 };

   uint8_t ec_channel_map[HEX] = { PCM_CHANNEL_FAR_REF };

   if(voice_is_channel_map_valid(create_param_ptr->channel_map_near))
   {
      memscpy(&pVptx->session.channel_map_near[0],sizeof(pVptx->session.channel_map_near),
            &create_param_ptr->channel_map_near[0],sizeof(create_param_ptr->channel_map_near));
   }
   else
   {
      //use default mic path channel map
      memscpy(&pVptx->session.channel_map_near[0],sizeof(pVptx->session.channel_map_near),
            mic_channel_map,sizeof(mic_channel_map));
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: near channel map is not received. using defaults. session(%x)",(int)pVptx->session.session_num);
   }

   if(voice_is_channel_map_valid(create_param_ptr->channel_map_far))
   {
      memscpy(&pVptx->session.channel_map_far[0],sizeof(pVptx->session.channel_map_far),
                  &create_param_ptr->channel_map_far[0],sizeof(create_param_ptr->channel_map_far));
   }
   else
   {
      //use default mic path channel map
      memscpy(&pVptx->session.channel_map_far[0],sizeof(pVptx->session.channel_map_far),
            ec_channel_map,sizeof(ec_channel_map));
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: far channel map is not received. using defaults. session(%x)",(int)pVptx->session.session_num);
   }

   return result;
}


static ADSPResult vptx_svc_create (void **near_handle,void **far_handle, int8_t voice_or_audio,
        vptx_create_params_v2_t* create_param_ptr, vptx_vpm_apr_info_t *apr_info_ptr)
{
   // static variable counter to make queue and thread name strings unique
   // limit to 16 bits so it will roll over and and only cost 4 characters in hexadecimal.
   // Queues in existence likely to have unique names, but not required...
   ADSPResult nResult;
   MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_create enter Topology(%lx), sampling_rate(%lx) num_mics(%d) num_ref_channels (%d) session(%lx) ",
          create_param_ptr->topology_id, create_param_ptr->sampling_rate, 
          create_param_ptr->num_mics, create_param_ptr->num_ref_channels, create_param_ptr->session_num  );


   nResult = vptx_is_supported(create_param_ptr->topology_id, create_param_ptr->sampling_rate, create_param_ptr->session_num, create_param_ptr->near_port_id, create_param_ptr->num_mics,create_param_ptr->num_ref_channels);
   if (ADSP_FAILED(nResult))
   {
      MSG_5(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "VCP: vptx_create Invalid Topology(%lx) or Invalid Samping Rate(%lx) or num_channel(%lx) or num_ref_channels (%lx), session(%lx)",
              create_param_ptr->topology_id, create_param_ptr->sampling_rate,create_param_ptr->num_mics, create_param_ptr->num_ref_channels,create_param_ptr->session_num);
      return nResult;
   }

   // allocate instance struct
   vptx_t *pVptx = (vptx_t*) qurt_elite_memory_malloc( sizeof(vptx_t), QURT_ELITE_HEAP_DEFAULT);
   if (!pVptx)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate mem for VPTX struct");
       return ADSP_ENOMEMORY;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx mem allocated, ptr %#x, session(%x)", (int)pVptx, (int)create_param_ptr->session_num);
   // zero out all the fields.
   memset(pVptx, 0, sizeof(vptx_t));

   // directly copy generic apr handle from VPM struct -> should be same for any VPM client
   // todo: pass apr_handle or apr_info as an arg to create?
   pVptx->modules.apr_info_dtmf.apr_handle = pVptx->static_svc_apr_handle;
   pVptx->session.session_num         = create_param_ptr->session_num;
   pVptx->session.audio_instance_flag = voice_or_audio; // tells if its audio instance or not
   pVptx->shared_mem_client   = create_param_ptr->shared_mem_client; // VPM/ADM client token for get virtual address and flush cache operations
   pVptx->static_svc_apr_handle = create_param_ptr->static_svc_apr_handle;

   if (qurt_elite_globalstate.pVoiceTimerCmdQ == NULL)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to get Vtm cmdQ session(%lx)",create_param_ptr->session_num);
      pVptx->timing.vtm_cmdq_ptr = NULL;
   }

   pVptx->timing.vtm_cmdq_ptr = (qurt_elite_globalstate.pVoiceTimerCmdQ);
   pVptx->timing.vds_handle_ptr = vds_handle;
   pVptx->timing.vds_client_token = VDS_CREATE_TOKEN(pVptx->session.session_num, VDS_CLIENT_VPTX_DELIVERY);

   snprintf(pVptx->io.near_in.dataq_name, QURT_ELITE_DEFAULT_NAME_LEN, "Vptx_NearQ%2lx",pVptx->session.session_num);
   snprintf(pVptx->io.far_in.dataq_name , QURT_ELITE_DEFAULT_NAME_LEN, "Vptx_FarQ%2lx",pVptx->session.session_num);
   snprintf(pVptx->cmdq_name     , QURT_ELITE_DEFAULT_NAME_LEN, "Vptx_CmdQ%2lx",pVptx->session.session_num);
   snprintf(pVptx->respq_name     , QURT_ELITE_DEFAULT_NAME_LEN, "Vptx_RespQ%2lx",pVptx->session.session_num);
   snprintf(pVptx->io.output.bufq_name     , QURT_ELITE_DEFAULT_NAME_LEN, "Vptx_BufQ%2lx",pVptx->session.session_num);
   snprintf(pVptx->io.loopback_output.bufq_name   , QURT_ELITE_DEFAULT_NAME_LEN, "Vptx_BufQLb%2lx",pVptx->session.session_num);

   qurt_elite_channel_init(&pVptx->qurt_elite_channel);
   // Create the queues. Use non-blocking queues, since pselect is always used.
   // pselect blocks on any non-masked queue to receive, then can do non-blocking checks.
   // near DataQ for near inputs
   // far DataQ for far inputs
   // CmdQ for cmd inputs
   // bufq_ptr for output buffer

   /* Allocate memory for queues */
   pVptx->near_svc_handle.dataQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_DATA_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );
   pVptx->far_svc_handle.dataQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_DATA_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );
   pVptx->near_svc_handle.cmdQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_CMD_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );
   pVptx->response_q = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_RESP_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );

   /* Allocate memory for signals */
   pVptx->timing.sync_tick_vtm_struct.signal_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   pVptx->timing.sync_tick_vtm_struct.signal_end_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   pVptx->timing.sync_tick_vtm_struct.resync_signal_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);

   pVptx->timing.proc_tick_vtm_struct.signal_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   pVptx->timing.proc_tick_vtm_struct.signal_end_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   pVptx->timing.proc_tick_vtm_struct.resync_signal_ptr = NULL;

   pVptx->timing.venc_delivery_tick_vtm_struct.signal_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);
   pVptx->timing.venc_delivery_tick_vtm_struct.signal_end_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);

   /* Allocate memory for signals */
   /* Delivery tick signal is provided by VDS, don't allocate/free/add to channel */
   pVptx->timing.vptx_delivery_tick_vtm_struct.signal_ptr = NULL;
   pVptx->timing.vptx_delivery_tick_vtm_struct.resync_signal_ptr = NULL;
   pVptx->timing.vptx_delivery_tick_vtm_struct.signal_end_ptr = (qurt_elite_signal_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_signal_t), QURT_ELITE_HEAP_DEFAULT);

   if (NULL == pVptx->near_svc_handle.dataQ
         || NULL == pVptx->far_svc_handle.dataQ
         || NULL == pVptx->near_svc_handle.cmdQ
         || NULL == pVptx->timing.sync_tick_vtm_struct.signal_ptr
         || NULL == pVptx->timing.sync_tick_vtm_struct.resync_signal_ptr
         || NULL == pVptx->timing.sync_tick_vtm_struct.signal_end_ptr
         || NULL == pVptx->timing.proc_tick_vtm_struct.signal_ptr
         || NULL == pVptx->timing.proc_tick_vtm_struct.signal_end_ptr
         || NULL == pVptx->timing.vptx_delivery_tick_vtm_struct.signal_end_ptr
         || NULL == pVptx->timing.venc_delivery_tick_vtm_struct.signal_ptr
         || NULL == pVptx->timing.venc_delivery_tick_vtm_struct.signal_end_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to allocate memory for VpTx Queues/Signals session(%#lx)",pVptx->session.session_num);
      vptx_destroy(pVptx);
      return ADSP_ENOMEMORY;
   }

   if(   ADSP_FAILED(nResult = qurt_elite_queue_init(pVptx->io.near_in.dataq_name, MAX_DATA_Q_ELEMENTS, pVptx->near_svc_handle.dataQ))
         || ADSP_FAILED(nResult = qurt_elite_queue_init(pVptx->io.far_in.dataq_name, MAX_DATA_Q_ELEMENTS, pVptx->far_svc_handle.dataQ))
         || ADSP_FAILED(nResult = qurt_elite_queue_init(pVptx->cmdq_name, MAX_CMD_Q_ELEMENTS, pVptx->near_svc_handle.cmdQ))
         || ADSP_FAILED(nResult = qurt_elite_queue_init(pVptx->respq_name, MAX_RESP_Q_ELEMENTS, pVptx->response_q))
         || ADSP_FAILED(nResult = qurt_elite_channel_addq(&(pVptx->qurt_elite_channel), (pVptx->near_svc_handle.cmdQ), VPTX_CMD_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_addq(&(pVptx->qurt_elite_channel), (pVptx->near_svc_handle.dataQ), VPTX_NEAR_DATA_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_addq(&(pVptx->qurt_elite_channel), (pVptx->far_svc_handle.dataQ), VPTX_FAR_DATA_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_addq(&(pVptx->qurt_elite_channel), (pVptx->response_q), VPTX_RESPONSE_MASK))
         || ADSP_FAILED(nResult = qurt_elite_signal_init(pVptx->timing.sync_tick_vtm_struct.signal_ptr))
         || ADSP_FAILED(nResult = qurt_elite_signal_init(pVptx->timing.sync_tick_vtm_struct.signal_end_ptr))
         || ADSP_FAILED(nResult = qurt_elite_signal_init(pVptx->timing.sync_tick_vtm_struct.resync_signal_ptr))
         || ADSP_FAILED(nResult = qurt_elite_signal_init(pVptx->timing.vptx_delivery_tick_vtm_struct.signal_end_ptr))
         || ADSP_FAILED(nResult = qurt_elite_signal_init(pVptx->timing.venc_delivery_tick_vtm_struct.signal_ptr))
         || ADSP_FAILED(nResult = qurt_elite_signal_init(pVptx->timing.venc_delivery_tick_vtm_struct.signal_end_ptr))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVptx->qurt_elite_channel, (pVptx->timing.sync_tick_vtm_struct.signal_ptr), VPTX_SYNC_TICK_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVptx->qurt_elite_channel, (pVptx->timing.sync_tick_vtm_struct.signal_end_ptr), VPTX_SYNC_TICK_END_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVptx->qurt_elite_channel, (pVptx->timing.proc_tick_vtm_struct.signal_ptr), VPTX_PROC_TICK_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVptx->qurt_elite_channel, (pVptx->timing.proc_tick_vtm_struct.signal_end_ptr), VPTX_PROC_TICK_END_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVptx->qurt_elite_channel, (pVptx->timing.sync_tick_vtm_struct.resync_signal_ptr), VPTX_RESYNC_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVptx->qurt_elite_channel, (pVptx->timing.vptx_delivery_tick_vtm_struct.signal_end_ptr), VPTX_DELIVERY_TICK_END_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVptx->qurt_elite_channel, (pVptx->timing.venc_delivery_tick_vtm_struct.signal_ptr), VPTX_VENC_DELIVERY_TICK_MASK))
         || ADSP_FAILED(nResult = qurt_elite_channel_add_signal(&pVptx->qurt_elite_channel, (pVptx->timing.venc_delivery_tick_vtm_struct.signal_end_ptr), VPTX_VENC_DELIVERY_TICK_END_MASK))
     )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed create Vptx MsgQs !! session(%lx)",pVptx->session.session_num);
      vptx_destroy(pVptx);
      return nResult;
   }
   pVptx->far_svc_handle.cmdQ = pVptx->near_svc_handle.cmdQ;  // Same cmd Q for tx and rx
   pVptx->far_svc_handle.gpQ = NULL;                        // gpQ not used
   pVptx->near_svc_handle.gpQ = NULL;                       // gpQ not used

   nResult = vptx_init(pVptx, create_param_ptr);
   if (ADSP_FAILED(nResult))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Init Vptx !! session(%lx)",pVptx->session.session_num);
      vptx_destroy(pVptx);
      return nResult;
   }

   //store client APR info if the pointer is not null
   if(apr_info_ptr)
   {

      pVptx->vpm_apr_info.self_addr = apr_info_ptr->self_addr;
      pVptx->vpm_apr_info.self_port = apr_info_ptr->self_port;
      pVptx->vpm_apr_info.client_addr = apr_info_ptr->client_addr;
      pVptx->vpm_apr_info.client_port = apr_info_ptr->client_port;
      pVptx->vpm_apr_info.apr_handle = apr_info_ptr->apr_handle;

      MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_create, saving clientAddr(%x) clientPort(%x) self_addr(%x) self_port(%x) apr_handle(%lx)",
            pVptx->vpm_apr_info.client_addr, pVptx->vpm_apr_info.client_port,pVptx->vpm_apr_info.self_addr,pVptx->vpm_apr_info.self_port,pVptx->vpm_apr_info.apr_handle);
   }
   else
   {
      //set to invalid value
      pVptx->vpm_apr_info.apr_handle = 0xFFFFFFFF;
   }

   *near_handle = &(pVptx->near_svc_handle);
   *far_handle  = &(pVptx->far_svc_handle);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_create end %d session(%lx)",nResult,pVptx->session.session_num);
   return ADSP_EOK;
}

ADSPResult vptx_create (void **near_handle, void **far_handle, vptx_create_params_v2_t* create_param_ptr, vptx_vpm_apr_info_t *apr_info_ptr)
{
   ADSPResult nResult;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_create from VPM enter session(%lx)",create_param_ptr->session_num);
   nResult = vptx_svc_create((void**)near_handle, (void **)far_handle, VPTX_VOICE, create_param_ptr, apr_info_ptr);
   return nResult;
}

ADSPResult vptx_aud_create (void **near_handle, void **far_handle, uint16_t afe_tx_port_id,uint16_t afe_rx_port_id, uint32_t topology_id,  uint32_t sampling_rate, uint32_t session_num, uint32_t shared_mem_client)
{
   ADSPResult nResult;
   vptx_create_params_v2_t create_param_v2;
   create_param_v2.near_port_id = afe_tx_port_id;
   create_param_v2.far_port_id = afe_rx_port_id;
   create_param_v2.topology_id = topology_id;
   create_param_v2.sampling_rate = sampling_rate;
   create_param_v2.session_num = session_num;
   create_param_v2.shared_mem_client = shared_mem_client;
   create_param_v2.num_mics = 0;
   create_param_v2.num_ref_channels = 1;


   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_aud_create from ADM enter session(%lx)", session_num);
   nResult = vptx_svc_create((void**)near_handle, (void **)far_handle, VPTX_AUDIO, &create_param_v2, NULL);
   return nResult;
}

ADSPResult vptx_aud_create_v2 (void **near_handle, void **far_handle, vptx_create_params_t* create_param_ptr)
{
   ADSPResult nResult;

   vptx_create_params_v2_t create_param_v2;
   memscpy(&create_param_v2,sizeof(vptx_create_params_v2_t),create_param_ptr,sizeof(vptx_create_params_t));
   // legecy topos support only mono reference.
   // channel mapping for legacy topos will be ignored, so no need to update
   create_param_v2.num_ref_channels= 1;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_aud_create_v2 from ADM enter session(%lx)", create_param_ptr->session_num);


   nResult = vptx_svc_create((void**)near_handle, (void **)far_handle, VPTX_AUDIO, &create_param_v2, NULL);
   return nResult;
}

ADSPResult vptx_aud_create_v3 (void **near_handle, void **far_handle, vptx_create_params_v2_t* create_param_ptr)
{
   ADSPResult nResult;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_aud_create_v3 from ADM enter session(%lx)", create_param_ptr->session_num);
   nResult = vptx_svc_create((void**)near_handle, (void **)far_handle, VPTX_AUDIO, create_param_ptr, NULL);
   return nResult;
}

// destructor
static void vptx_destroy (vptx_t* pVptx)
{
   if (pVptx)
   {
      uint32_t session_num = pVptx->session.session_num;

      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_destroy begin session(%x)",(int)session_num);

      // Free memory for vptx processing and out buffers
      vptx_end(pVptx);

      // call utility function to destroy data Q
      if (NULL != pVptx->near_svc_handle.dataQ)
      {
         elite_svc_deinit_data_queue(pVptx->near_svc_handle.dataQ);
         qurt_elite_memory_free(pVptx->near_svc_handle.dataQ);
         pVptx->near_svc_handle.dataQ = NULL;
      }

      if (NULL != pVptx->far_svc_handle.dataQ)
      {
         elite_svc_deinit_data_queue(pVptx->far_svc_handle.dataQ);
         qurt_elite_memory_free(pVptx->far_svc_handle.dataQ);
         pVptx->far_svc_handle.dataQ = NULL;
      }

      // call utility function to destroy cmd Q
      if (NULL != pVptx->near_svc_handle.cmdQ)
      {
         elite_svc_deinit_cmd_queue(pVptx->near_svc_handle.cmdQ);
         qurt_elite_memory_free(pVptx->near_svc_handle.cmdQ);
         pVptx->near_svc_handle.cmdQ = NULL;
      }

      if (NULL != pVptx->response_q)
      {
         elite_svc_deinit_cmd_queue(pVptx->response_q);
         qurt_elite_memory_free(pVptx->response_q);
         pVptx->response_q = NULL;
      }

      // call utility function to destroy signal
      if (NULL != pVptx->timing.sync_tick_vtm_struct.signal_ptr)
      {
         qurt_elite_signal_deinit(pVptx->timing.sync_tick_vtm_struct.signal_ptr);
         qurt_elite_memory_free(pVptx->timing.sync_tick_vtm_struct.signal_ptr);
         pVptx->timing.sync_tick_vtm_struct.signal_ptr = NULL;
      }
      if (NULL != pVptx->timing.sync_tick_vtm_struct.signal_end_ptr)
      {
         qurt_elite_signal_deinit(pVptx->timing.sync_tick_vtm_struct.signal_end_ptr);
         qurt_elite_memory_free(pVptx->timing.sync_tick_vtm_struct.signal_end_ptr);
         pVptx->timing.sync_tick_vtm_struct.signal_end_ptr = NULL;
      }
      if (NULL != pVptx->timing.proc_tick_vtm_struct.signal_ptr)
      {
         qurt_elite_signal_deinit(pVptx->timing.proc_tick_vtm_struct.signal_ptr);
         qurt_elite_memory_free(pVptx->timing.proc_tick_vtm_struct.signal_ptr);
         pVptx->timing.proc_tick_vtm_struct.signal_ptr = NULL;
      }
      if (NULL != pVptx->timing.proc_tick_vtm_struct.signal_end_ptr)
      {
         qurt_elite_signal_deinit(pVptx->timing.proc_tick_vtm_struct.signal_end_ptr);
         qurt_elite_memory_free(pVptx->timing.proc_tick_vtm_struct.signal_end_ptr);
         pVptx->timing.proc_tick_vtm_struct.signal_end_ptr = NULL;
      }
      if (NULL != pVptx->timing.sync_tick_vtm_struct.resync_signal_ptr)
      {
         qurt_elite_signal_deinit(pVptx->timing.sync_tick_vtm_struct.resync_signal_ptr);
         qurt_elite_memory_free(pVptx->timing.sync_tick_vtm_struct.resync_signal_ptr);
         pVptx->timing.sync_tick_vtm_struct.resync_signal_ptr = NULL;
      }

      // call utility function to destroy signal
      if (NULL != pVptx->timing.vptx_delivery_tick_vtm_struct.signal_end_ptr)
      {
         qurt_elite_signal_deinit(pVptx->timing.vptx_delivery_tick_vtm_struct.signal_end_ptr);
         qurt_elite_memory_free(pVptx->timing.vptx_delivery_tick_vtm_struct.signal_end_ptr);
         pVptx->timing.vptx_delivery_tick_vtm_struct.signal_end_ptr = NULL;
      }

      if (NULL != pVptx->timing.venc_delivery_tick_vtm_struct.signal_ptr)
      {
         qurt_elite_signal_deinit(pVptx->timing.venc_delivery_tick_vtm_struct.signal_ptr);
         qurt_elite_memory_free(pVptx->timing.venc_delivery_tick_vtm_struct.signal_ptr);
         pVptx->timing.venc_delivery_tick_vtm_struct.signal_ptr = NULL;
      }
      if (NULL != pVptx->timing.venc_delivery_tick_vtm_struct.signal_end_ptr)
      {
         qurt_elite_signal_deinit(pVptx->timing.venc_delivery_tick_vtm_struct.signal_end_ptr);
         qurt_elite_memory_free(pVptx->timing.venc_delivery_tick_vtm_struct.signal_end_ptr);
         pVptx->timing.venc_delivery_tick_vtm_struct.signal_end_ptr = NULL;
      }

      // Destroy the qurt_elite_channel
      qurt_elite_channel_destroy(&pVptx->qurt_elite_channel);

      qurt_elite_memory_free (pVptx);

      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_destroy end session(%x)",(int)session_num);
   }
   pVptx = NULL;
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
 * non-inplace module UNTIL WE HIT ECNS MODULE.
 *
 * For ECNS module, stream buffer input for near port will be ouput of previous module.
 * And port 1 will have FAR input buffer. Output of ECNS will be EC_OUTPUT_STRM_BUF.
 * And primary output port will be assigned to OUTPUT_STRM_BUF_2 if its input is INPUT_STRM_BUF_1
 * The intention is to switch the memory locations as EC is non-inplace.
 *
 * after ECNS, switching between OUTPUT_STRM_BUF_1 and OUTPUT_STRM_BUF_2 will be done similar to step 2.
 *
 * Depending on the final module output stream buffer, it will be written to output circular buffer.
 */
ADSPResult vptx_update_io_stream_data(vptx_t *pVptx)
{
   ADSPResult result           = ADSP_EOK;
   voice_capi_io_stream_data_idx_t currentIdx = INPUT_STRM_BUF_1;
   voice_capi_module_t *curr_module_ptr;
   uint32_t i;
   uint32_t ec_input_near_port_idx = INPUT_STRM_BUF_1;
   uint32_t lec_port_idx =  pVptx->ec_capi_info.ecns_lec_output_port_idx;
   uint32_t nr_port_idx =  pVptx->ec_capi_info.ecns_nr_output_port_idx;
   uint32_t pri_out_idx = pVptx->ec_capi_info.ecns_prim_output_port_idx;


   //Ensure AVC should be present after ECNS
   if(pVptx->modules.special_capis[ECNS_CAPI] && pVptx->modules.special_capis[AVCRVE_CAPI])
   {
      if(pVptx->modules.special_capis[AVCRVE_CAPI]->module_index <= pVptx->modules.special_capis[ECNS_CAPI]->module_index)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid AVC location(%ld) compared to ECNS location(%ld) in topology",
               pVptx->modules.special_capis[AVCRVE_CAPI]->module_index,pVptx->modules.special_capis[ECNS_CAPI]->module_index);
         return ADSP_EFAILED;
      }
   }

   //start with SS
   if(pVptx->modules.ss_capi.module_ptr)
   {
      //SS is called at the start if it is created by framework and SS CAPI is inplace
      pVptx->modules.ss_capi.input_buf_index = pVptx->modules.ss_capi.output_buf_index = INPUT_STRM_BUF_1;
   }

   pVptx->ec_near_input_buf_idx = currentIdx;
   // Loop through the modules and set the input and output buffers.
   curr_module_ptr = &pVptx->modules.modules_array[0];
   for (i = 0; i < pVptx->modules.num_modules; i++)
   {
      //check if the module is EC.
      if(pVptx->modules.special_capis[ECNS_CAPI] == curr_module_ptr)
      {
         if( (currentIdx!=INPUT_STRM_BUF_1) && (currentIdx!=INPUT_STRM_BUF_2))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid ECNS port(%d). session(%lx)", (int)currentIdx,pVptx->session.session_num);
            return ADSP_EFAILED;
         }
         pVptx->ec_near_input_buf_idx = ec_input_near_port_idx = currentIdx;
         curr_module_ptr->input_buf_index = EC_INPUT_STRM_BUF; //buffer pointers will be updated later
         curr_module_ptr->output_buf_index = EC_OUTPUT_STRM_BUF; //buffer pointers will be updated later

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ECNS module index in topo = %ld, input_buf_idx=%ld", curr_module_ptr->module_index,ec_input_near_port_idx);

         //update current index with ECNS outputs index
         currentIdx =  (ec_input_near_port_idx==INPUT_STRM_BUF_1)?OUTPUT_STRM_BUF_2:OUTPUT_STRM_BUF_1;
      }
      else
      {
         //update AVC buffer index with ECOUTPUT index
         if(pVptx->modules.special_capis[AVCRVE_CAPI] == curr_module_ptr)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: AVC module index in topo(%ld)", curr_module_ptr->module_index);
            curr_module_ptr->input_buf_index = EC_OUTPUT_STRM_BUF;
            curr_module_ptr->output_buf_index = currentIdx;
         }
         else
         {
            curr_module_ptr->input_buf_index = currentIdx;
            if( (TRUE == curr_module_ptr->is_enabled) && (FALSE == curr_module_ptr->is_in_place) )
            {
               switch(currentIdx)
               {
                  case INPUT_STRM_BUF_1:
                  {
                     currentIdx = INPUT_STRM_BUF_2;
                     break;
                  }
                  case INPUT_STRM_BUF_2:
                  {
                     currentIdx = INPUT_STRM_BUF_1;
                     break;
                  }
                  case OUTPUT_STRM_BUF_1:
                  {
                     currentIdx = OUTPUT_STRM_BUF_2;
                     break;
                  }
                  case OUTPUT_STRM_BUF_2:
                  {
                     currentIdx = OUTPUT_STRM_BUF_1;
                     break;
                  }
                  default:
                  {
                     MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Error!! invalid currentIdx(%d), session(%lx)",(int)currentIdx, pVptx->session.session_num);
                     break;
                  }
               }
            }
            curr_module_ptr->output_buf_index = currentIdx;
         }
      }

      if(pVptx->modules.special_capis[DYN_RESAMP])
      {
         pVptx->modules.special_capis[DYN_RESAMP]->output_buf_index = RESAMPLER_OUTPUT_STRM_BUF;
      }
      //dbg message
      MSG_6(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_update_io_stream_buf(): moduleId(%lx), idx(%ld),is_inplace(%ld), enable(%lx), input_index(%ld), output_index(%ld)",
            curr_module_ptr->module_id, curr_module_ptr->module_index,curr_module_ptr->is_in_place, curr_module_ptr->is_enabled,
            curr_module_ptr->input_buf_index, curr_module_ptr->output_buf_index);

      curr_module_ptr++;
   }

   //update input buffers
   for(i=0; i<pVptx->io.near_in.mediatype.num_channels; i++)
   {
      pVptx->near_input_buf[VPTX_TEMP_BUF_1][i].max_data_len = (pVptx->io.near_in.frame_samples + pVptx->io.prebuffer_samples)<<1;
      pVptx->near_input_buf[VPTX_TEMP_BUF_1][i].data_ptr = (int8_t*)pVptx->io.near_in.process_buf_ptr[i];

      pVptx->near_input_buf[VPTX_TEMP_BUF_2][i].max_data_len = (pVptx->io.near_in.frame_samples + pVptx->io.prebuffer_samples)<<1;
      pVptx->near_input_buf[VPTX_TEMP_BUF_2][i].data_ptr = (int8_t*)pVptx->io.near_in.process_buf_ptr_sec[i];

      if(INPUT_STRM_BUF_1 == ec_input_near_port_idx)
      {
         pVptx->ec_near_input[i] = &pVptx->near_input_buf[VPTX_TEMP_BUF_1][i];
      }
      else
      {
         pVptx->ec_near_input[i] = &pVptx->near_input_buf[VPTX_TEMP_BUF_2][i];
      }
   }

   //FAR input buffer
   for(i=0; i<pVptx->io.far_in.mediatype.num_channels; i++)
   {
       pVptx->far_input[i].data_ptr = (int8_t*)pVptx->io.far_in.process_buf_ptr[i];
       pVptx->far_input[i].max_data_len  = (uint32_t)((pVptx->io.far_in.frame_samples + pVptx->io.prebuffer_samples)<<1);
   }

   //output buffers
   pVptx->output_buf[VPTX_TEMP_BUF_1].max_data_len = pVptx->io.near_in.frame_samples<<1;
   pVptx->output_buf[VPTX_TEMP_BUF_1].data_ptr = pVptx->io.near_in.process_buf_ptr[0];

   pVptx->output_buf[VPTX_TEMP_BUF_2].max_data_len = pVptx->io.near_in.frame_samples<<1;
   pVptx->output_buf[VPTX_TEMP_BUF_2].data_ptr = pVptx->io.near_in.process_buf_ptr_sec[0];

   //Allocate LEC or NR port memory, of ECNS supports it. Port zero output is already allocated
   if(CAPI_V2_DATA_FORMAT_INVALID_VAL != lec_port_idx)
   {
      uint32_t size_bytes = (FRAME_SAMPLES_NB_PER_MS * (pVptx->ec_capi_info.output_port[lec_port_idx].data_format.sampling_rate/8000) * pVptx->io.num_1ms_frame_block_process)<<1;

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Allocating LEC port memory, size(%lu) session(%lx)",size_bytes, pVptx->session.session_num);

      pVptx->lec_output.data_ptr = (int8_t*)qurt_elite_memory_malloc(size_bytes,QURT_ELITE_HEAP_DEFAULT);
      if(!pVptx->lec_output.data_ptr)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate memory for ecns output port(%ld), size(%ld), session(%lx)",i, size_bytes, pVptx->session.session_num);
      }
      pVptx->lec_output.max_data_len = size_bytes;

      pVptx->ec_output[lec_port_idx] = &pVptx->lec_output;
   }
   else
   {
      MSG_1( MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: No memory is allocated for LEC port. session(%lx)", pVptx->session.session_num);
   }

   if(CAPI_V2_DATA_FORMAT_INVALID_VAL != nr_port_idx)
   {
      uint32_t size_bytes = (FRAME_SAMPLES_NB_PER_MS * (pVptx->ec_capi_info.output_port[nr_port_idx].data_format.sampling_rate/8000) * pVptx->io.num_1ms_frame_block_process)<<1;

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Allocating NR port memory, size(%lu) session(%lx)",size_bytes, pVptx->session.session_num);

      pVptx->nr_output.data_ptr = (int8_t*)qurt_elite_memory_malloc(size_bytes,QURT_ELITE_HEAP_DEFAULT);
      if(!pVptx->nr_output.data_ptr)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate memory for ecns output port(%ld), size(%ld), session(%lx)",i, size_bytes, pVptx->session.session_num);
      }
      pVptx->nr_output.max_data_len = size_bytes;

      pVptx->ec_output[nr_port_idx] = &pVptx->nr_output;
   }
   else
   {
      MSG_1( MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: No memory is allocated for NR port. session(%lx)", pVptx->session.session_num);
   }

   //
   if(INPUT_STRM_BUF_1 == ec_input_near_port_idx)
   {
      pVptx->ec_output[pri_out_idx] = &pVptx->output_buf[VPTX_TEMP_BUF_2];
   }
   else
   {
      pVptx->ec_output[pri_out_idx] = &pVptx->output_buf[VPTX_TEMP_BUF_1];
   }

   pVptx->resampler_output.data_ptr =  pVptx->modules.resampler_out_buf;
   pVptx->resampler_output.max_data_len = VOICE_ROUNDTO8(pVptx->io.output.buf_samples << 1);

   //update stream data
   pVptx->input_stream_data[VPTX_TEMP_BUF_1].buf_ptr = &pVptx->near_input_buf[VPTX_TEMP_BUF_1][0];
   pVptx->input_stream_data[VPTX_TEMP_BUF_1].bufs_num = pVptx->io.near_in.mediatype.num_channels;

   pVptx->input_stream_data[VPTX_TEMP_BUF_2].buf_ptr = &pVptx->near_input_buf[VPTX_TEMP_BUF_2][0];
   pVptx->input_stream_data[VPTX_TEMP_BUF_2].bufs_num = pVptx->io.near_in.mediatype.num_channels;

   pVptx->output_stream_data[VPTX_TEMP_BUF_1].buf_ptr = &pVptx->output_buf[0];
   pVptx->output_stream_data[VPTX_TEMP_BUF_1].bufs_num = 1;

   pVptx->output_stream_data[VPTX_TEMP_BUF_2].buf_ptr = &pVptx->output_buf[1];
   pVptx->output_stream_data[VPTX_TEMP_BUF_2].bufs_num = 1;

   pVptx->ec_input_stream_data[VPTX_NEAR_INPUT_PORT].buf_ptr = &pVptx->near_input_buf[pVptx->ec_near_input_buf_idx][0];
   pVptx->ec_input_stream_data[VPTX_NEAR_INPUT_PORT].bufs_num = pVptx->io.near_in.mediatype.num_channels;

   pVptx->ec_input_stream_data[VPTX_FAR_INPUT_PORT].buf_ptr = &pVptx->far_input[0];
   pVptx->ec_input_stream_data[VPTX_FAR_INPUT_PORT].bufs_num = pVptx->io.far_in.mediatype.num_channels;

   if (pVptx->ec_capi_info.force_mono_reference)
   {
      pVptx->ec_input_stream_data[VPTX_FAR_INPUT_PORT].bufs_num =1;
   }

   pVptx->ec_output_stream_data[0].buf_ptr = pVptx->ec_output[0];
   pVptx->ec_output_stream_data[0].bufs_num = 1;

   pVptx->ec_output_stream_data[1].buf_ptr = pVptx->ec_output[1];
   pVptx->ec_output_stream_data[1].bufs_num = 1;

   pVptx->ec_output_stream_data[2].buf_ptr = pVptx->ec_output[2];
   pVptx->ec_output_stream_data[2].bufs_num = 1;

   pVptx->resampler_out_stream_data.buf_ptr = &pVptx->resampler_output;
   pVptx->resampler_out_stream_data.bufs_num = 1;

   {
      capi_v2_stream_data_t *stream_data_ptrs_temp[NUM_STRM_BUF_IDX][ECNS_MAX_OUTPUT_PORT] = {
            {&pVptx->input_stream_data[0],NULL,NULL},
            {&pVptx->input_stream_data[1],NULL,NULL},
            {&pVptx->ec_input_stream_data[VPTX_NEAR_INPUT_PORT],&pVptx->ec_input_stream_data[VPTX_FAR_INPUT_PORT],NULL},
            {&pVptx->ec_output_stream_data[0],&pVptx->ec_output_stream_data[1],&pVptx->ec_output_stream_data[2]},
            {&pVptx->output_stream_data[0],NULL,NULL},
            {&pVptx->output_stream_data[1],NULL,NULL},
            {&pVptx->resampler_out_stream_data,NULL,NULL},
      };

      //SS capi might need far input port as well.
      if (pVptx->modules.ss_capi.module_ptr)
      {
         stream_data_ptrs_temp[INPUT_STRM_BUF_1][1] = &pVptx->ec_input_stream_data[VPTX_FAR_INPUT_PORT];
      }

      memscpy(pVptx->stream_data_ptrs,sizeof(pVptx->stream_data_ptrs),stream_data_ptrs_temp,sizeof(stream_data_ptrs_temp));
   }

   return result;
}

static ADSPResult vptx_init(vptx_t *pVptx, vptx_create_params_v2_t* create_param_ptr)
{
   ADSPResult nResult           = ADSP_EOK;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_init begin session(%x)",(int)pVptx->session.session_num);

   pVptx->near_svc_handle.unSvcId           = ELITE_VOICEPROCTX_SVCID;
   pVptx->far_svc_handle.unSvcId            = ELITE_VOICEPROCTX_SVCID;

   pVptx->session.topology_id                       = create_param_ptr->topology_id;                   // topology
   pVptx->session.num_mics                          = create_param_ptr->num_mics;
   pVptx->session.num_ref_channels                  = create_param_ptr->num_ref_channels;
   pVptx->session.sampling_rate                     = create_param_ptr->sampling_rate;                 // sampling rate
   pVptx->session.nb_sampling_rate_factor           = create_param_ptr->sampling_rate/VOICE_NB_SAMPLING_RATE; // conversion factor from NB sampling rate
   pVptx->session.nb_sampling_rate_factor_output    = (VPTX_AUDIO == pVptx->session.audio_instance_flag) ? VOICE_FB_SAMPLING_RATE/VOICE_NB_SAMPLING_RATE: pVptx->session.nb_sampling_rate_factor;  // conversion factor from NB sampling rate
   pVptx->session.real_time_flag                    = TRUE;                          // real time mode by default
   pVptx->session.vfr_mode                          = VFR_NONE;                      // VFR mode is disabled by default

   pVptx->afe_info.afe_tx_port_id                    = create_param_ptr->near_port_id;
   pVptx->afe_info.afe_rx_port_id                    = create_param_ptr->far_port_id;
   pVptx->afe_info.afe_tx_drift_ptr                  = NULL;
   pVptx->afe_info.afe_rx_drift_ptr                  = NULL;

   pVptx->io.prebuffer_samples                       = PREBUF_SAMPLES(create_param_ptr->sampling_rate);   // 1ms for sample slip buffering, since far and near can drift in opp. direction 1ms is not enough.
   pVptx->io.jitter_buffer_samples                   = JITTERBUF_SAMPLES(create_param_ptr->sampling_rate); // 1ms block size for Jitter buffer in samples

   pVptx->timing.sync_tick_vtm_struct.offset             = 0;
   pVptx->timing.sync_tick_vtm_struct.signal_enable      = 0;
   pVptx->timing.sync_tick_vtm_struct.client_id          = VOICEPROC_TX_NEAR;
   pVptx->timing.sync_tick_vtm_struct.vsid               = 0;
   pVptx->timing.sync_tick_vtm_struct.timing_ver         = VFR_CLIENT_INFO_VER_1;         // version 1 timing by default

   pVptx->timing.proc_tick_vtm_struct.offset             = 0;
   pVptx->timing.proc_tick_vtm_struct.signal_enable      = 0;
   pVptx->timing.proc_tick_vtm_struct.client_id          = VOICEPROC_TX_NEAR_IN;
   pVptx->timing.proc_tick_vtm_struct.vsid               = 0;
   pVptx->timing.proc_tick_vtm_struct.timing_ver         = VFR_CLIENT_INFO_VER_1;         // version 1 timing by default

   pVptx->timing.venc_delivery_tick_vtm_struct.offset             = 8000;                 // default value as per venc code
   pVptx->timing.venc_delivery_tick_vtm_struct.signal_enable      = 0;
   pVptx->timing.venc_delivery_tick_vtm_struct.client_id          = VOICEPROC_TX_VENC_DELIVERY;
   pVptx->timing.venc_delivery_tick_vtm_struct.vsid               = 0;
   pVptx->timing.venc_delivery_tick_vtm_struct.timing_ver         = VFR_CLIENT_INFO_VER_1;         // version 1 timing by default
   pVptx->timing.enc_offset                              = 0;                             // offset is zero by default
   pVptx->timing.sync_offset                             = 0;                             // offset is zero by default
   pVptx->timing.delivery_offset                         = 0;                             // offset is zero by default
   pVptx->timing.vptx_delivery_tick_vtm_struct.offset         = 0;
   pVptx->timing.vptx_delivery_tick_vtm_struct.signal_enable  = 0;
   pVptx->timing.vptx_delivery_tick_vtm_struct.client_id      = VOICEPROC_TX_NEAR_OUT;
   pVptx->timing.vptx_delivery_tick_vtm_struct.vsid           = 0;
   pVptx->timing.vptx_delivery_tick_vtm_struct.timing_ver     = VFR_CLIENT_INFO_VER_1;         // version 1 timing by default
   pVptx->timing.vsid                                    = 0;
   pVptx->timing.use_new_timing_flag                     = FALSE;                         // default old timing
   pVptx->timing.timing_ver                              = VFR_CLIENT_INFO_VER_1;         // default version 1 (no VSID) of timing
   pVptx->max_tx_processing_time                         = 0;

   vptx_ctrl_params_init(pVptx);

   // update the channel mapping information
   vptx_update_channel_map(pVptx, create_param_ptr);

   nResult = vptx_set_media_type(pVptx);
   if (ADSP_FAILED(nResult))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid Vptx topology (%#lx) session(%#lx)",pVptx->session.topology_id,pVptx->session.session_num);
      return nResult;
   }

   //set module_list_locked false
   pVptx->modules.module_list_locked = FALSE;

   pVptx->modules.is_data_threshold_raised = FALSE;

   //retrieve topology based information if required
   if(FALSE == pVptx->reconfig_state.retain_amdb_handle)
   {
      nResult = voice_update_topo_information(pVptx->session.topology_id,
            &pVptx->modules.topo_entry_ptr,
            &pVptx->modules.topo_entry_size,
            &pVptx->modules.topo_entry_handle);
      if (ADSP_FAILED(nResult))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid vptx topology (%#lx) session(%#lx)",pVptx->session.topology_id,pVptx->session.session_num);
         return nResult;
      }
      pVptx->modules.num_modules = pVptx->modules.topo_entry_ptr->topo_def.num_modules;
      pVptx->modules.modules_list = &pVptx->modules.topo_entry_ptr->module_info[0];

      // add one for soft mute
      pVptx->modules.num_modules++;
      
      // add one for Dynamic Resampler
      if((VPTX_AUDIO == pVptx->session.audio_instance_flag) && (pVptx->io.near_in.mediatype.sample_rate != pVptx->io.output.mediatype.sample_rate))
      {
         pVptx->modules.num_modules++;
      }

      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: create command/reconfig to different topology Allocating module memory");
      pVptx->modules.modules_array = (voice_capi_module_t*)qurt_elite_memory_malloc(sizeof(voice_capi_module_t) * pVptx->modules.num_modules, QURT_ELITE_HEAP_DEFAULT);
      if(NULL == pVptx->modules.modules_array)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: failed to allocate memory for module instances. session(%#lx)",pVptx->session.session_num);
         return ADSP_ENOMEMORY;
      }
      memset(pVptx->modules.modules_array,0,sizeof(voice_capi_module_t) * pVptx->modules.num_modules);
   }

   // Allocate memory for module instances
   pVptx->memory.module_size    = (uint32_t*)qurt_elite_memory_malloc(sizeof(uint32_t) * pVptx->modules.num_modules, QURT_ELITE_HEAP_DEFAULT);
   pVptx->memory.guard_band_ptrs    = (int8_t *)qurt_elite_memory_malloc(sizeof(uint64_t *) * pVptx->modules.num_modules, QURT_ELITE_HEAP_DEFAULT); //pointer points to array of guardband pointers
   if( (NULL == pVptx->modules.modules_array) || (NULL == pVptx->memory.module_size) || (NULL == pVptx->memory.guard_band_ptrs) )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: failed to allocate memory for module instances. session(%#lx)",pVptx->session.session_num);
      return ADSP_ENOMEMORY;
   }
   memset(pVptx->memory.module_size,0,sizeof(uint32_t) * pVptx->modules.num_modules);
   memset(pVptx->memory.guard_band_ptrs,0,sizeof(uint64_t *) * pVptx->modules.num_modules);


   voice_cmn_time_profile_init( &pVptx->profiler);

   SET_MASK(pVptx->wait_mask, CMD_DATA_MASK);                                // Always

   if (IS_PSEUDO_PORT_AFE_ID(create_param_ptr->near_port_id))
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Psuedo Port, hence making VPTX non real time/ faster than real time");
      pVptx->session.real_time_flag = FALSE;                                         // non real time mode if pseudo_port
   }

   // Allocate and manage memory required for Circular buffers and Modules
   nResult = vptx_allocate_mem(pVptx);
   if (ADSP_FAILED(nResult))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Allocate Vptx Internal Memory !! session(%x)",(int)pVptx->session.session_num);
      return nResult;
   }

   //allocate LEC and NR memory and update stream data indices
   nResult = vptx_update_io_stream_data(pVptx);
   if (ADSP_FAILED(nResult))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to assign io stream indices !! session(%#lx)",pVptx->session.session_num);
      return nResult;
   }

   //set module_list_locked TRUE
   pVptx->modules.module_list_locked = TRUE;

   // Create BufQ and add to channel
   pVptx->io.output.bufq_ptr = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_BUF_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );
   pVptx->io.loopback_output.bufq_ptr = (qurt_elite_queue_t*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES (MAX_BUF_Q_ELEMENTS), QURT_ELITE_HEAP_DEFAULT );

   if (( NULL == pVptx->io.output.bufq_ptr ) || ( NULL == pVptx->io.loopback_output.bufq_ptr ))
   {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to allocate memory for Vptx Out buf queue/ loopback buf queue !! session(%x)",(int)pVptx->session.session_num);
        return ADSP_ENOMEMORY;
   }

   if(  ADSP_FAILED(nResult = qurt_elite_queue_init(pVptx->io.output.bufq_name, MAX_BUF_Q_ELEMENTS, pVptx->io.output.bufq_ptr))
         || ADSP_FAILED(nResult = qurt_elite_channel_addq(&pVptx->qurt_elite_channel, (pVptx->io.output.bufq_ptr), VPTX_BUF_MASK))
         || ADSP_FAILED(nResult = qurt_elite_queue_init(pVptx->io.loopback_output.bufq_name, MAX_BUF_Q_ELEMENTS, pVptx->io.loopback_output.bufq_ptr))
         || ADSP_FAILED(nResult = qurt_elite_channel_addq(&pVptx->qurt_elite_channel, (pVptx->io.loopback_output.bufq_ptr), VPTX_BUF_LB_MASK))
     )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed create Vptx Out buf queue!! session(%x)",(int)pVptx->session.session_num);
      return nResult;
   }

   // Allocate and queue up the output buffers. NOTE: cannot fill queue to the full
   nResult = vptx_out_buf_allocator(pVptx,pVptx->io.output.buf_samples);
   if (ADSP_FAILED(nResult))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Memory Alloc OutBuf!! session(%x)",(int)pVptx->session.session_num);
      return nResult;
   }

   // Instantiate and initialize processing blocks
   // Memory has to be alocated to modules before the following function is used
   vptx_modules_init(pVptx);
   // DTMF Detection init

   nResult = vptx_setup_sns_qmi(pVptx, VPTX_QMI_REG_MASK, VPTX_QMI_IND_MASK);
   if(ADSP_FAILED(nResult)) { return nResult; }

   // Spin thread either for create or for reinit to new topo, both should have stack_changed as true
    if(pVptx->session.stack_changed)
    {
       int32_t nPriority;
       pVptx->session.stack_changed = FALSE;
       pVptx->session.cached_tid = pVptx->near_svc_handle.threadId;
       aTHREAD_NAME[3] = ((pVptx->session.session_num + 48) & 0xff); // int32_t to ascii conversion
       MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx stack :%ld", pVptx->session.stack_size);

       pVptx->reconfig_state.another_thread_launched = FALSE;
       //if reconfig is pending, then there would be another thread running. set the flag
       if(pVptx->reconfig_state.reconfig_pending)
       {
          MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Reconfig cmd. set join thread flag");
          pVptx->reconfig_state.another_thread_launched = TRUE;
       }

       // Thread priority of vptx should be same as audio COPP thread priority when vptx is invoked in audio path
       // in voice path it should be regular vptx thread priority
       if(VPTX_AUDIO == pVptx->session.audio_instance_flag)
       {
          nPriority = elite_get_thrd_prio(ELITETHREAD_DYNA_PP_SVC_PRIO_ID);
          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VPTX thread priority in audio path is %d session(%x)",(int)nPriority,(int)pVptx->session.session_num);
       }
       else
       {
          nPriority = elite_get_thrd_prio(ELITETHREAD_DYNA_VOICE_PROC_TX_PRIO_ID);
          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VPTX thread priority in voice path is %d session(%x)",(int)nPriority,(int)pVptx->session.session_num);
       }

       if (ADSP_FAILED(nResult = qurt_elite_thread_launch(&(pVptx->near_svc_handle.threadId), aTHREAD_NAME, NULL,
                  pVptx->session.stack_size, nPriority, vptx_thread_entry, (void*)pVptx, QURT_ELITE_HEAP_DEFAULT)))
       {
          MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed create Vptx thread!! session(%lx)", pVptx->session.session_num);
          // update thread lauch status
          pVptx->reconfig_state.another_thread_launched = FALSE;
          pVptx->fatal_state = TRUE;
          return nResult;
       }
       pVptx->far_svc_handle.threadId = pVptx->near_svc_handle.threadId; // Near and Far thread ids are the same
       MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Vptx new thread launched, id1 %d, id2 %d", (int)pVptx->near_svc_handle.threadId, (int)qurt_thread_get_id());
    }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_init end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static void vptx_clear_capi_buf(vptx_t *pVptx)
{
   if( pVptx->nr_output.data_ptr )
   {
      MSG_1( MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: deallocating NR port memory, session(%lx)",pVptx->session.session_num);
      qurt_elite_memory_free(pVptx->nr_output.data_ptr);
      pVptx->nr_output.data_ptr = NULL;
   }
   pVptx->nr_output.actual_data_len = pVptx->nr_output.max_data_len = 0;

   if( pVptx->lec_output.data_ptr )
   {
      MSG_1( MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: deallocating LEC port memory, session(%lx)",pVptx->session.session_num);
      qurt_elite_memory_free(pVptx->lec_output.data_ptr);
      pVptx->lec_output.data_ptr = NULL;
   }
   pVptx->nr_output.actual_data_len = pVptx->nr_output.max_data_len = 0;

   memset(pVptx->near_input_buf,0,sizeof(pVptx->near_input_buf));
   memset(&pVptx->far_input,0,sizeof(pVptx->far_input));
   memset(pVptx->output_buf,0,sizeof(pVptx->output_buf));
   memset(pVptx->ec_near_input,0,sizeof(pVptx->ec_near_input));
   memset(pVptx->ec_output,0,sizeof(pVptx->ec_output));

   memset(pVptx->input_stream_data,0,sizeof(pVptx->input_stream_data));
   memset(pVptx->output_stream_data,0,sizeof(pVptx->output_stream_data));

   memset(pVptx->ec_input_stream_data,0,sizeof(pVptx->ec_input_stream_data));
   memset(pVptx->ec_output_stream_data,0,sizeof(pVptx->ec_output_stream_data));

   memset(pVptx->stream_data_ptrs,0,sizeof(pVptx->stream_data_ptrs));
}

static void vptx_end(vptx_t *pVptx)
{
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_end begin  session(%x)",(int)pVptx->session.session_num);
   vptx_destroy_sns_qmi(pVptx);

   /* disabling DTMF, reset aprInfo */
   pVptx->modules.apr_info_dtmf.self_addr = 0;
   pVptx->modules.apr_info_dtmf.self_port = 0;
   pVptx->modules.apr_info_dtmf.client_addr = 0;
   pVptx->modules.apr_info_dtmf.client_port = 0;
   // Destroy Post-proc modules
   vptx_modules_end(pVptx);

   if(pVptx->io.output.bufq_ptr)
   {
      // call utility function to destroy buf Q
      elite_svc_deinit_buf_queue(pVptx->io.output.bufq_ptr, pVptx->io.output.bufs_allocated);
      qurt_elite_memory_free(pVptx->io.output.bufq_ptr);
      pVptx->io.output.bufq_ptr = NULL;
   }

   if(pVptx->io.loopback_output.bufq_ptr)
   {
      // call utility function to destroy loopback buf Q
      elite_svc_deinit_buf_queue(pVptx->io.loopback_output.bufq_ptr, pVptx->io.loopback_output.bufs_allocated);
      qurt_elite_memory_free(pVptx->io.loopback_output.bufq_ptr);
      pVptx->io.loopback_output.bufq_ptr = NULL;
   }

   pVptx->io.output.bufs_allocated = 0;
   pVptx->io.loopback_output.bufs_allocated = 0;

   if( FALSE == pVptx->reconfig_state.retain_amdb_handle )
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Free module array and topo handle");

      if(pVptx->modules.modules_array)
      {
         qurt_elite_memory_free(pVptx->modules.modules_array);
         pVptx->modules.modules_array = NULL;
      }
      if(ELITE_CMN_TOPO_DB_INVALID_HANDLE != pVptx->modules.topo_entry_handle )
      {
         voice_release_topo_information(pVptx->modules.topo_entry_handle);
         pVptx->modules.topo_entry_handle = ELITE_CMN_TOPO_DB_INVALID_HANDLE;
      }
      pVptx->modules.num_modules = 0; //set number of modules to zero as the memory is freed
   }
   if(pVptx->memory.module_size)
   {
      qurt_elite_memory_free(pVptx->memory.module_size);
      pVptx->memory.module_size = NULL;
   }
   if(pVptx->memory.guard_band_ptrs)
   {
      qurt_elite_memory_free(pVptx->memory.guard_band_ptrs);
      pVptx->memory.guard_band_ptrs = NULL;
   }

   vptx_clear_capi_buf(pVptx);

   // Free memory managers memory
   vptx_allocate_mem_free(pVptx);

   // Free circbuf memory
   vptx_free_circ_bufs(pVptx);

   if(pVptx->io.scratch.data_ptr)
   {
      // scratch buffer is NULL'ed as init will reallocate if required
      pVptx->io.scratch.data_ptr = NULL;
      pVptx->io.scratch.data_size_bytes = 0;
   }

   // print out profiling stats
   if( pVptx->profiler.num_samples > 0)
   {
      MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_end session(%x) topo(%x): profiler microsec: num_samples(%d) max_time(%d) min_time(%d) avg_time(%d) max_cycles(%d)",
            (int)pVptx->session.session_num,
            (int)pVptx->session.topology_id,
            (int)pVptx->profiler.num_samples,
            (int)pVptx->profiler.max_time,
            (int)pVptx->profiler.min_time,
            (int)(pVptx->profiler.total_time / pVptx->profiler.num_samples),
            (int) pVptx->profiler.max_cycles );
   }
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_end end session(%x)",(int)pVptx->session.session_num);
}

static ADSPResult vptx_thread_entry(void* pInstance)
{
   vptx_t* pVptx = (vptx_t*)pInstance;
   int join_status;
   ADSPResult result;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vptx_thread_entry(): VPTx Current context: %d", (int)qurt_thread_get_id());
   if(pVptx->reconfig_state.another_thread_launched)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vptx_thread_entry(): for Reconfig, waiting for join, session(%lx)", pVptx->session.session_num);
      // a new thread was launched, so join the old thread first
      qurt_elite_thread_join(pVptx->session.cached_tid, &join_status);
      // Usually it should return immediately, once it returns, reenter the workloop
      //Can safely clear flag once join returns, since now there is only one thread
      pVptx->reconfig_state.another_thread_launched = FALSE;
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vptx_thread_entry(): for Create, session(%lx)",pVptx->session.session_num);
   }
   // call into workloop
   result = vptx_thread(pInstance);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vptx_thread_entry(): exiting, session(%lx), Current Tid(%d)", pVptx->session.session_num,qurt_thread_get_id());
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

static ADSPResult vptx_thread(void* pInstance)
{
   ADSPResult nResult;                    // general nResult value
   vptx_t *pVptx = (vptx_t*)pInstance;    // instance structure
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_thread(): begin session(%x), Current context: %d",(int)pVptx->session.session_num, (int)qurt_thread_get_id());
   // Enter forever loop
   for(;;)
   {
      // ***************** Wait for the MASK
      // block on any one or more of selected queues to get a msg
      pVptx->rcvd_mask = qurt_elite_channel_wait(&pVptx->qurt_elite_channel, pVptx->wait_mask);

      while(pVptx->rcvd_mask)
      {
         pVptx->bit_pos = voice_get_signal_pos(pVptx->rcvd_mask);

         // De queue and signal clear done in the handler functions.
         nResult = pVptxHandler[pVptx->bit_pos](pVptx);

         if(nResult != ADSP_EOK)
         {
            if (ADSP_ETERMINATED == nResult)
            {
                  MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_thread(): end session(%x), Current context: %d",(int)pVptx->session.session_num, (int)qurt_thread_get_id());

               return ADSP_EOK;
            }
            MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: vptx_thread(): Handler Error bit_pos=%d, res=%d",(int)pVptx->bit_pos, nResult );
         }
         if(pVptx->reconfig_state.another_thread_launched)
         {
            // the old thread will come here, and exit at this point
            MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_thread():Another thread launched. exiting current Tid(%ld) session(%lx), cachedTid(%ld)",qurt_thread_get_id(),pVptx->session.session_num,pVptx->session.cached_tid);
            return ADSP_EOK;
         }

         pVptx->rcvd_mask = qurt_elite_channel_poll(&pVptx->qurt_elite_channel, pVptx->wait_mask);
      } // end of while loop
   } // end of for loop
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "vptx_thread(): exiting Current context: %d", (int)qurt_thread_get_id());
   return 0;
}

static ADSPResult vptx_multichannel_circ_buf_discard_samples(voice_multi_ch_circ_buf_t *structPtr, int32_t numSamples)
{
    ADSPResult result=ADSP_EOK;
    // discarding numSamples from each channel of multi_ch_circ_buf
    for (int32 i=0;i<structPtr->num_channels;i++)
    {
        voice_circbuf_read((circbuf_struct *)&(structPtr->circ_buf[i]), NULL, numSamples,0);
        if (CIRCBUF_SUCCESS != result)
        {
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: failure in discarding samples, ptr(%p) num_channels(%ld), index(%ld)",structPtr,structPtr->num_channels,i);
            return result;
        }
    }

    return result;
}



static ADSPResult voice_multichannel_circ_buf_pad_zeros(voice_multi_ch_circ_buf_t *structPtr, int32_t numSamples, vptx_scratch_data_t *scratch)
{

    if (!scratch->data_ptr)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"scratch data pointer is NULL");
        return ADSP_EFAILED;
    }
    int8* temp_buf1 = scratch->data_ptr;
    int8* temp_buf2 = scratch->data_ptr + (scratch->data_size_bytes >>1);
    ADSPResult result = ADSP_EOK;

    //MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP:  scratch size (%ld) temp_buf1 (%p), temp_buf2 (%p)",scratch->data_size_bytes,temp_buf1,temp_buf2);

    /*
       temp_buf1 is used to save data present in circular buffer
       temp_buf2 content is always zero as this acratch->data_ptr is memset to 0 while going to RUN. This is used pad zeros to circular bufffer
       Since it always has zero a memset 0 is avoided in this function call for the whole buffer.
    */
    for (int32 i=0;i<structPtr->num_channels;i++)
    {
        uint32 unread_samples = structPtr->circ_buf[i].unread_samples;
        // first read unread samples into a temp buf;
        result = voice_circbuf_read((circbuf_struct *)&(structPtr->circ_buf[i]),
                 temp_buf1, unread_samples, (scratch->data_size_bytes >>1));
        if (CIRCBUF_SUCCESS != result)
        {
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_circ_buf_read error, ptr(%p) num_channels(%ld), index(%ld)",structPtr,structPtr->num_channels,i);
        }
        // code to check if zeros are correctly appended
#if defined(LOG_INPUT_DATA) && defined(SIM)

{
        char buffer[100]; // The filename buffer.
        // Put output of ith module in ith file
        snprintf(buffer, sizeof(char) * 100, "data_present_channel_%d.raw", i);
        FILE *fp;
        int16_t *pIn =(int16*) temp_buf1;
        fp = fopen(buffer,"ab"); // in append mode
        fwrite(pIn,sizeof(char),unread_samples<<1,fp);
        fclose(fp);
}

#endif

        // the scratch buf is cleared during vptx_run. Hence temp2_buf will have all zeros.
        result = voice_circbuf_write((circbuf_struct *)&(structPtr->circ_buf[i]),
                 temp_buf2, numSamples);
        if (CIRCBUF_SUCCESS != result)
        {
            MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_circ_buf_write error, in writing zeros ptr(%p) num_channels(%ld), index(%ld), unread samples(%ld) , max samples(%ld) samples(%ld)",
                    structPtr,structPtr->num_channels,i,structPtr->circ_buf[i].unread_samples, structPtr->circ_buf[i].max_samples, numSamples);
        }
        // right back the actual data
        result = voice_circbuf_write((circbuf_struct *)&(structPtr->circ_buf[i]),
                 temp_buf1, unread_samples);

        if (CIRCBUF_SUCCESS != result)
        {
            MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_circ_buf_write error in wirting actual data, ptr(%p) num_channels(%ld), index(%ld), unread samples(%ld) , max samples(%ld) samples(%ld)",
                    structPtr,structPtr->num_channels,i,structPtr->circ_buf[i].unread_samples, structPtr->circ_buf[i].max_samples, numSamples);
        }
#if defined(LOG_INPUT_DATA) && defined(SIM)
{
        circbuf_struct temp_circ = structPtr->circ_buf[i];//make a copy
        char buffer[100]; // The filename buffer.
         // Put output of ith module in ith file
         snprintf(buffer, sizeof(char) * 100, "zero_padded_channel_%d.raw", i);
         FILE *fp;
         int16 temp3[960+48];
         result = voice_circbuf_read((circbuf_struct *)&temp_circ,
                 (int8*)temp3, (unread_samples+numSamples), (960+48) <<1);
                if (CIRCBUF_SUCCESS != result)
                {
                    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP:  dbg voice_circ_buf_read error, ptr(%p) num_channels(%ld), index(%ld)",structPtr,structPtr->num_channels,i);
                    break;
                }

         fp = fopen(buffer,"ab"); // in append mode
         fwrite(temp3,sizeof(char),(unread_samples+numSamples)<<1,fp);
         fclose(fp);
}
#endif

    }
    return result;
}

static ADSPResult vptx_adjust_samples_prior_proc(vptx_t* pVptx)
{
    ADSPResult result=ADSP_EOK;

    uint32 near_samples = (pVptx->io.near_in.circ_buf.circ_buf[0].unread_samples - pVptx->ratematch.ss_buf_samples_near);
    uint32 far_samples =  (pVptx->io.far_in.circ_buf.circ_buf[0].unread_samples - pVptx->ratematch.ss_buf_samples_far);
    //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_proc_tick_handler: number of unread samples in near (%ld) and far (%ld)",near_samples,far_samples);

    uint32_t process_samples = pVptx->proc_start_samples;

    if(near_samples == 0)
    {
	// this means we haven't got a single sample yet from primary path, so no need to adjust anything
	return result;
    }
    if (near_samples == far_samples) // both are equal
    {

        if (near_samples == process_samples)
        {
            // sufficient samples to start process
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: unread samples in near (%ld) and far (%ld) ",near_samples,far_samples);

        }
        else if (near_samples > process_samples)
        {
            // both near and far samples are more than process samples hence the difference has to be discarded
            uint32 excess_samples = near_samples - process_samples;
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: unread samples in near (%ld) and far (%ld) discarding (%ld) samples",near_samples,far_samples,excess_samples);
            vptx_multichannel_circ_buf_discard_samples(&pVptx->io.near_in.circ_buf,excess_samples);
            vptx_multichannel_circ_buf_discard_samples(&pVptx->io.far_in.circ_buf,excess_samples);
            pVptx->io.ull_timestamp += ((excess_samples *1000000) / pVptx->session.sampling_rate);

        }
        else
        {
            //both near and far less than process samples
            // pad zeros and start processing
            uint32 zeros_to_pad = process_samples - near_samples;
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: unread samples in near (%ld) and far (%ld) pad (%ld) zero samples to both near and far",near_samples,far_samples,zeros_to_pad);
            voice_multichannel_circ_buf_pad_zeros(&pVptx->io.near_in.circ_buf,zeros_to_pad,&pVptx->io.scratch);
            voice_multichannel_circ_buf_pad_zeros(&pVptx->io.far_in.circ_buf,zeros_to_pad,&pVptx->io.scratch);
            pVptx->io.ull_timestamp -= ((zeros_to_pad *1000000) / pVptx->session.sampling_rate);
        }

        uint64_t time = qurt_elite_timer_get_time();
        uint64_t cycles = qurt_elite_profile_get_pcycles();
        pVptx->session.vfr_vptx_cntr = pVptx->session.vfr_cycle;
        vptx_process(pVptx);

        time = qurt_elite_timer_get_time() - time;
        cycles = qurt_elite_profile_get_pcycles() - cycles;
        voice_cmn_time_profile_add_data( &pVptx->profiler, (uint32_t) time, cycles);

    }
    else if (near_samples > far_samples)
    {

        //near is greater than far
        if (near_samples > process_samples)
        {
            // if near is greater than process than excess amount of data in near has to be discarded from both near and far
            uint32 excess_samples = near_samples-process_samples;
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: unread samples in near (%ld) and far (%ld)  discarding (%ld) samples",near_samples,far_samples,excess_samples);
            vptx_multichannel_circ_buf_discard_samples(&pVptx->io.near_in.circ_buf,excess_samples);
            vptx_multichannel_circ_buf_discard_samples(&pVptx->io.far_in.circ_buf,excess_samples);
            pVptx->io.ull_timestamp += ((excess_samples *1000000) / pVptx->session.sampling_rate);
            // process should not be called as the both near and far will not have process samples
        }
        else if (near_samples < process_samples)
        {
            // both near and far samples are less than process samples
            // pad (process_samples- near_samples) zeros to both near and far
            uint32 zeros_to_pad = process_samples - near_samples;
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: unread samples in near (%ld) and far (%ld) pad (%ld) zero samples to both near and far",near_samples,far_samples,zeros_to_pad);
            voice_multichannel_circ_buf_pad_zeros(&pVptx->io.near_in.circ_buf,zeros_to_pad,&pVptx->io.scratch);
            voice_multichannel_circ_buf_pad_zeros(&pVptx->io.far_in.circ_buf,zeros_to_pad,&pVptx->io.scratch);
            pVptx->io.ull_timestamp -= ((zeros_to_pad *1000000) / pVptx->session.sampling_rate);
        }
        // do nothing if near_samples == process_samples;
    }
    else
    {
        // far is greater than near
        if (far_samples > process_samples)
        {
            // if far is greater than process than excess amount of data in far has to be discarded from both near and far
            uint32 excess_samples = far_samples-process_samples;
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: unread samples in near (%ld) and far (%ld) discarding (%ld) samples",near_samples,far_samples,excess_samples);
            vptx_multichannel_circ_buf_discard_samples(&pVptx->io.near_in.circ_buf,excess_samples);
            vptx_multichannel_circ_buf_discard_samples(&pVptx->io.far_in.circ_buf,excess_samples);
            pVptx->io.ull_timestamp += ((excess_samples *1000000) / pVptx->session.sampling_rate);
            // process should not be called as the both near and far will not have process samples
        }
        else if (far_samples < process_samples)
        {
            // both near and far samples are less than process samples
            // pad (process_samples- far_samples) zeros to both near and far
            uint32 zeros_to_pad = process_samples - far_samples;
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: unread samples in near (%ld) and far (%ld) pad (%ld) zero samples to both near and far",near_samples,far_samples,zeros_to_pad);
            voice_multichannel_circ_buf_pad_zeros(&pVptx->io.near_in.circ_buf,zeros_to_pad,&pVptx->io.scratch);
            voice_multichannel_circ_buf_pad_zeros(&pVptx->io.far_in.circ_buf,zeros_to_pad,&pVptx->io.scratch);
            pVptx->io.ull_timestamp -= ((zeros_to_pad *1000000) / pVptx->session.sampling_rate);
        }
    }


    return result;

}

static ADSPResult vptx_sync_tick_handler(void *pInstance)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t *pVptx = (vptx_t*)pInstance;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_sync_tick_handler begin session(%x)",(int)pVptx->session.session_num);

   qurt_elite_signal_clear(pVptx->timing.sync_tick_vtm_struct.signal_ptr);
   //Send the ouptut first
   if (
         (TRUE == pVptx->io.near_in.dataflow_on)
         && ((TRUE == pVptx->io.far_in.dataflow_on)||(TRUE  == pVptx->io.force_far_zeros))
         && (pVptx->process_data_flag)
         && (VPTX_AUDIO != pVptx->session.audio_instance_flag)
         && (pVptx->session.real_time_flag)
      )
   {
      if (FALSE == pVptx->timing.ref_time_tick_recvd_flag)
      {
         pVptx->timing.ref_time_tick_recvd_flag = TRUE; // set vfr true
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx ref timer SYNCED, VFR Mode(%d)",pVptx->session.vfr_mode);
      }
      else   // second sync tick onwards
      {
         // if its 10 ms topology then vptx will start collecting data after first sync tick
          pVptx->process_start_flag = TRUE;
      }

   }

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_sync_tick_handler end session(%x)",(int)pVptx->session.session_num);
   return nResult;
}
static ADSPResult vptx_proc_tick_handler(void *pInstance)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t *pVptx = (vptx_t*)pInstance;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_proc_tick_handler begin session(%x)",(int)pVptx->session.session_num);

   qurt_elite_signal_clear(pVptx->timing.proc_tick_vtm_struct.signal_ptr);
   //Send the ouptut first
   if (
         (TRUE == pVptx->io.near_in.dataflow_on)
         && ((TRUE == pVptx->io.far_in.dataflow_on)||(TRUE  == pVptx->io.force_far_zeros))
         && (pVptx->process_data_flag)
         && (VPTX_AUDIO != pVptx->session.audio_instance_flag)
         && (pVptx->session.real_time_flag)
      )
   {
      //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx SSInfoCounter (%d), session(%x)",(int)pVptx->ratematch.ss_info_counter_near,(int)pVptx->session.session_num);

         // The smart tick KPI optimizations are valid only for topologies that operate on 20 ms
         if (20 == pVptx->io.num_1ms_frame_block_process)
         {
             // If its 20 ms topology then vptx process will start now
          if ( FALSE == pVptx->process_start_flag )
          {
             MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx proc tick received, VFR Mode(%d)",pVptx->session.vfr_mode);
             pVptx->process_start_flag = TRUE;
             vptx_adjust_samples_prior_proc(pVptx);
         }
      }
      else if (TRUE == pVptx->timing.ref_time_tick_recvd_flag)
      {
         pVptx->process_start_flag = TRUE;                  
      }

   }

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_sync_tick_handler end session(%x)",(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_sync_tick_end_handler(void *pInstance)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t *pVptx = (vptx_t*)pInstance;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_sync_tick_end_handler begin session(%x)",(int)pVptx->session.session_num);

   qurt_elite_signal_clear(pVptx->timing.sync_tick_vtm_struct.signal_end_ptr);

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_sync_tick_end_handler end session(%x)",(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_proc_tick_end_handler(void *pInstance)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t *pVptx = (vptx_t*)pInstance;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_proc_tick_end_handler begin session(%x)",(int)pVptx->session.session_num);

   qurt_elite_signal_clear(pVptx->timing.proc_tick_vtm_struct.signal_end_ptr);

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_proc_tick_end_handler end session(%x)",(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_delivery_tick_end_handler(void *pInstance)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t *pVptx = (vptx_t*)pInstance;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_delivery_tick_end_handler begin session(%x)",(int)pVptx->session.session_num);

   qurt_elite_signal_clear(pVptx->timing.vptx_delivery_tick_vtm_struct.signal_end_ptr);

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_delivery_tick_end_handler end session(%x)",(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_cmd_handler(void *pInstance)
{
   // if cmd mask, vptx_process Commands
   ADSPResult nResult = ADSP_EFAILED;
   vptx_t *pVptx = (vptx_t*)pInstance;
   elite_msg_any_t msg;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_cmd_handler session(%x)",(int)pVptx->session.session_num);
   nResult = qurt_elite_queue_pop_front(pVptx->near_svc_handle.cmdQ, (uint64_t*) &msg);
   // ***************** Process the msg
   if (ADSP_EOK == nResult)
   {
      const uint32_t cmd_table_size = sizeof(pEliteHandler)/sizeof(pEliteHandler[0]);
      if (msg.unOpCode >= cmd_table_size)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Unsupported message ID 0x%8x!! session(%x)", (int)msg.unOpCode,(int)pVptx->session.session_num);
         return ADSP_EUNSUPPORTED;
      }

      //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: MESSAGE ID is 0x%8x session(%x)", (int)msg.unOpCode,(int)pVptx->session.session_num); 
      if(TRUE == pVptx->fatal_state)
      {
         // accept only destroy and custom msg cmd in fatal state.
         if ((0 == msg.unOpCode) || ( 2 == msg.unOpCode))
         {
            nResult = pEliteHandler[msg.unOpCode](pInstance, &msg);
         }
         else
         {
            nResult = ADSP_EFAILED;
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vptx in fatal state!! result (%d) session(%x)",nResult,(int)pVptx->session.session_num);
            return ADSP_EFAILED; 
         }
      }
      else
      {
         // table lookup to call handling function, with FALSE to indicate processing of msg
         nResult = pEliteHandler[msg.unOpCode](pInstance, &msg);
      }
   }
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_cmd_handler end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_buf_handler(void *pInstance)
{
   // ***************** copy data to output queue and deliver
   ADSPResult nResult = ADSP_EBADPARAM;
   vptx_t *pVptx = (vptx_t*)pInstance;
   qurt_elite_bufmgr_node_t   outBufMgrNode;
   elite_msg_any_t* pPeerDataQMsg;
   qurt_elite_queue_t *bufq_ptr;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_buf_handler begin session(%x)",(int)pVptx->session.session_num);

   if (FALSE == pVptx->process_data_flag) // dont send out any buffer if not in run state
   {
      pVptx->wait_mask &= (~VPTX_BUF_MASK);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx buf handler called in stop state - handling error: removing buf mask , session(%#lx)",pVptx->session.session_num);
      return nResult;
   }
   // check which output buffer invoked the handler and use the appropriate bufq
   nResult = qurt_elite_queue_pop_front(pVptx->io.output.bufq_ptr, (uint64_t*)&outBufMgrNode);
   bufq_ptr = pVptx->io.output.bufq_ptr;

   if (ADSP_EOK == nResult)
   {
      if (pVptx->io.output.send_mediatype_flag)
      {
         elite_msg_data_media_type_apr_t* pMediaTypePayload =
            (elite_msg_data_media_type_apr_t*)outBufMgrNode.pBuffer;

         pMediaTypePayload->pBufferReturnQ    = bufq_ptr;
         pMediaTypePayload->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;
         pMediaTypePayload->unMediaFormatID   = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

         elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk                       =
            (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pMediaTypePayload);
         memset(pMediaFormatBlk,0,sizeof(elite_multi_channel_pcm_fmt_blk_t));

         pMediaFormatBlk->num_channels     = pVptx->io.output.mediatype.num_channels;
         pMediaFormatBlk->sample_rate      = pVptx->io.output.mediatype.sample_rate;
         pMediaFormatBlk->bits_per_sample     = 16;
         pMediaFormatBlk->is_signed           = TRUE;
         pMediaFormatBlk->is_interleaved      = FALSE;

         if (1 == pMediaFormatBlk->num_channels)
         {
            pMediaFormatBlk->channel_mapping[0] = PCM_CHANNEL_C;
         }
         else if (2 == pMediaFormatBlk->num_channels)
         {
            pMediaFormatBlk->channel_mapping[0] = PCM_CHANNEL_L;
            pMediaFormatBlk->channel_mapping[1] = PCM_CHANNEL_R;
         }

         // send pOutputBuf to downstream service.
         pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(
               &outBufMgrNode,
               ELITE_DATA_MEDIA_TYPE,
               NULL, /* do not need response */
               0,    /* token */
               0     /* do not care response nResult*/
               );

         nResult = qurt_elite_queue_push_back(pVptx->downstream_peer_ptr->dataQ, (uint64_t*)pPeerDataQMsg);

         if(ADSP_FAILED(nResult))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed delivering mediatype session(%x)",(int)pVptx->session.session_num);
            (void) elite_msg_push_payload_to_returnq(bufq_ptr, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx delivered media type session(%x)",(int)pVptx->session.session_num);
            pVptx->io.output.send_mediatype_flag = FALSE;

            if (pVptx->session.real_time_flag)
            {
               OR_MASK(pVptx->wait_mask,(VPTX_NEAR_DATA_MASK | VPTX_FAR_DATA_MASK | VPTX_BUF_MASK)); // Add Data Mask
            }
            else
            {
               OR_MASK(pVptx->wait_mask,VPTX_BUF_MASK); // Add BUF Mask
               NOT_MASK(pVptx->wait_mask,(VPTX_NEAR_DATA_MASK | VPTX_FAR_DATA_MASK)); // Remove Data Mask and have buf mask only
            }
         } // end of else
      }
      // 0 indicates even phase, and 1 indicates odd phase
      // 2 is immediately reset to zero
      else if (0 != pVptx->io.out_gen_cnt)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_buf_handler re-adjusting to 20ms phase boundary, no data sent. samples(%x) circ buf samples(%x)session(%x)",(int)(pVptx->io.output.buf_samples), (int)(pVptx->io.output.circ_buf.circ_buf[0].unread_samples),(int)pVptx->session.session_num);
         (void) elite_msg_push_payload_to_returnq(bufq_ptr, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
         OR_MASK(pVptx->wait_mask,(VPTX_NEAR_DATA_MASK | VPTX_FAR_DATA_MASK )); // Add Data Mask
         NOT_MASK(pVptx->wait_mask,VPTX_BUF_MASK); // Remove BUF MASK
      }
      else if ((pVptx->io.output.buf_samples) <= (pVptx->io.output.circ_buf.circ_buf[0].unread_samples))
      {
         // fill the ouptut buffer pointers
         elite_msg_data_buffer_t* pOutDataPayload = (elite_msg_data_buffer_t*) outBufMgrNode.pBuffer;
         //copy input buffer attributes to output buffer
         pOutDataPayload->pResponseQ           = NULL;
         pOutDataPayload->unClientToken        = NULL;
         pOutDataPayload->pBufferReturnQ       = (bufq_ptr);
         pOutDataPayload->nOffset              = 0;
         pOutDataPayload->nActualSize          = (pVptx->io.output.buf_samples * pVptx->io.output.mediatype.num_channels) * sizeof(int16_t);
         if(VPTX_AUDIO != pVptx->session.audio_instance_flag)
         {
            pOutDataPayload->nFlag             =  pOutDataPayload->nFlag | (0x80000000);
            pOutDataPayload->ullTimeStamp      =  pVptx->io.ull_timestamp;
            pVptx->io.ull_timestamp += 20000;
         }
         // copy from output circ buf to output buffer queue
         voice_multi_ch_circ_buf_block_read(&(pVptx->io.output.circ_buf), (int8_t *)&(pOutDataPayload->nDataBuf), (int32_t)(pVptx->io.output.buf_samples * pVptx->io.output.mediatype.num_channels));
         // send pOutputBuf to downstream service.
         pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(
               &outBufMgrNode,
               ELITE_DATA_BUFFER,
               NULL, /* do not need response */
               pVptx->session.vfr_vptx_cntr,    /* token */
               0     /* do not care response nResult*/
               );

         if(--pVptx->session.vfr_vptx_cntr < 0)
         {
            pVptx->session.vfr_vptx_cntr = 0 ;
         }

         // Deliver immediately if audio path/FTRT
         if((!pVptx->session.real_time_flag) || (VPTX_AUDIO == pVptx->session.audio_instance_flag))
         {
            nResult = qurt_elite_queue_push_back(pVptx->downstream_peer_ptr->dataQ, (uint64_t*)pPeerDataQMsg);
         }
         // Queue to VDS if not
         else
         {
            uint32_t buffer_id = VDS_UNTIMED_BUFFER;
            if ( (1 == pVptx->timing.vptx_delivery_tick_vtm_struct.signal_enable)&&(pVptx->session.vfr_vptx_cntr == pVptx->session.vfr_cycle - 1 ))
            {
            buffer_id = VDS_TIMED_BUFFER;
            }
            nResult = voice_cmn_send_vds_elite_request(pVptx->timing.vds_client_id, pVptx->timing.vds_client_token, buffer_id, pVptx->downstream_peer_ptr->dataQ,
                                            pVptx->timing.vds_handle_ptr, pPeerDataQMsg, pVptx->session.session_num);
         }

         if(ADSP_FAILED(nResult))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed deliver buf dwnstrm Dropping! session(%x)",(int)pVptx->session.session_num);
            (void) elite_msg_push_payload_to_returnq(bufq_ptr, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
         }
         else
         {
            //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx delivered data session(%x)",(int)pVptx->session.session_num);
            OR_MASK(pVptx->wait_mask, (VPTX_NEAR_DATA_MASK | VPTX_FAR_DATA_MASK)); // Add Data Mask
            if((pVptx->io.output.buf_samples) > (pVptx->io.output.circ_buf.circ_buf[0].unread_samples))
            {
               NOT_MASK(pVptx->wait_mask,VPTX_BUF_MASK); // Remove BUF MASK
            }
            // Disable this flag in buf handler, so that next buffer is not allowed to invoke buf handler before venc delivery tick
            // Also make sure to disable this flag only for last vptx frame for this cycle
            if ( pVptx->session.vfr_vptx_cntr <= 0)
            {
               pVptx->is_venc_tick_fired = FALSE;
            }
            pVptx->rcvd_mask = 0x0;
         }
      }
      else
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Resync/Error state vptx_buf_handler dropping o/p buf out samples(%x) circ buf samples(%x)session(%x)",(int)(pVptx->io.output.buf_samples), (int)(pVptx->io.output.circ_buf.circ_buf[0].unread_samples),(int)pVptx->session.session_num);
         (void) elite_msg_push_payload_to_returnq(bufq_ptr, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
         OR_MASK(pVptx->wait_mask,(VPTX_NEAR_DATA_MASK | VPTX_FAR_DATA_MASK )); // Add Data Mask
         NOT_MASK(pVptx->wait_mask,VPTX_BUF_MASK); // Remove BUF MASK
      }
   }
   else // there was error reading queue.
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error Popping output buffer %d, %x",nResult,(int)pVptx->session.session_num);
   }
   MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_buf_handler end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_loopback_buf_handler(void *pInstance)
{
   // ***************** copy data to output queue and deliver
   ADSPResult nResult = ADSP_EBADPARAM;
   vptx_t *pVptx = (vptx_t*)pInstance;
   qurt_elite_bufmgr_node_t   outBufMgrNode;
   elite_msg_any_t* pPeerDataQMsg;
   qurt_elite_queue_t *bufq_ptr; // local bufq ptr, can be normal bufq or loopback bufq
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_loopback_buf_handler begin session(%x)",(int)pVptx->session.session_num);

   // check which output buffer invoked the handler and use the appropriate bufq
   nResult = qurt_elite_queue_pop_front(pVptx->io.loopback_output.bufq_ptr, (uint64_t*)&outBufMgrNode);
   bufq_ptr = pVptx->io.loopback_output.bufq_ptr;

   if (ADSP_EOK == nResult)
   {
      if (pVptx->io.loopback_output.send_mediatype_flag)
      {
         elite_msg_data_media_type_apr_t* pMediaTypePayload =
            (elite_msg_data_media_type_apr_t*)outBufMgrNode.pBuffer;

         pMediaTypePayload->pBufferReturnQ    = bufq_ptr;
         pMediaTypePayload->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;
         pMediaTypePayload->unMediaFormatID   = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;
         elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk   = (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pMediaTypePayload);
         memset(pMediaFormatBlk,0,sizeof(elite_multi_channel_pcm_fmt_blk_t));
         pMediaFormatBlk->sample_rate         = pVptx->io.loopback_output.mediatype.sample_rate;
         pMediaFormatBlk->num_channels        = pVptx->io.loopback_output.mediatype.num_channels;
         pMediaFormatBlk->bits_per_sample     = 16;
         pMediaFormatBlk->is_signed           = TRUE;
         pMediaFormatBlk->is_interleaved      = FALSE;

         // send pOutputBuf to downstream service.
         pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(
               &outBufMgrNode,
               ELITE_DATA_MEDIA_TYPE,
               NULL, /* do not need response */
               0,    /* token */
               0     /* do not care response nResult*/
               );

         nResult = qurt_elite_queue_push_back(pVptx->rx_peer_ptr->gpQ, (uint64_t*)pPeerDataQMsg);

         if(ADSP_FAILED(nResult))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed deliver loopback buf dwnstrm Dropping! session(%x)",(int)pVptx->session.session_num);
            (void) elite_msg_push_payload_to_returnq(bufq_ptr, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx loopback delivered media type session(%x)",(int)pVptx->session.session_num);
            pVptx->io.loopback_output.send_mediatype_flag = FALSE;

            OR_MASK(pVptx->wait_mask,VPTX_BUF_LB_MASK); // Add BUF Mask, let the buf handler control all the other masks
         } // end of else
      }
      else if (0 != pVptx->io.out_gen_cnt)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_loopback_buf_handler re-adjusting to 20ms phase boundary, no data sent. samples(%x) circ buf samples(%x)session(%x",(int)(pVptx->io.output.buf_samples), (int)(pVptx->io.output.circ_buf.circ_buf[0].unread_samples),(int)pVptx->session.session_num);
         (void) elite_msg_push_payload_to_returnq(bufq_ptr, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
         NOT_MASK(pVptx->wait_mask,VPTX_BUF_LB_MASK); // Remove LB BUF MASK
      }
      else if ((pVptx->io.output.buf_samples) <= (pVptx->io.loopback_output.circ_buf.circ_buf[0].unread_samples))
      {
         // fill the ouptut buffer pointers
         elite_msg_data_buffer_t* pOutDataPayload = (elite_msg_data_buffer_t*) outBufMgrNode.pBuffer;
         //copy input buffer attributes to output buffer
         pOutDataPayload->pResponseQ           = NULL;
         pOutDataPayload->unClientToken        = NULL;
         pOutDataPayload->pBufferReturnQ       = (bufq_ptr);
         pOutDataPayload->nOffset              = 0;
         pOutDataPayload->nActualSize          = (pVptx->io.output.buf_samples * pVptx->io.loopback_output.mediatype.num_channels) << 1; // Left shift by 2 for 20ms and for bytes

         // copy from output circ buf to output buffer queue
         voice_multi_ch_circ_buf_block_read(&(pVptx->io.loopback_output.circ_buf), (int8_t *)&(pOutDataPayload->nDataBuf), (int32_t)(pVptx->io.output.buf_samples * pVptx->io.loopback_output.mediatype.num_channels));

         // send pOutputBuf to downstream service.
         pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(
               &outBufMgrNode,
               ELITE_DATA_BUFFER,
               NULL, /* do not need response */
               0,    /* token */
               0     /* do not care response nResult*/
               );

         nResult = qurt_elite_queue_push_back(pVptx->rx_peer_ptr->gpQ, (uint64_t*)pPeerDataQMsg);

         if(ADSP_FAILED(nResult))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed deliver loopback buf dwnstrm Dropping! session(%x)",(int)pVptx->session.session_num);
            (void) elite_msg_push_payload_to_returnq(bufq_ptr, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx delivered loopback data session(%x)",(int)pVptx->session.session_num);
            NOT_MASK(pVptx->wait_mask,VPTX_BUF_LB_MASK); // Remove BUF MASK
            pVptx->rcvd_mask = 0x0;
         }
      }
      else
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Resync/Error state vptx_loopback_buf_handler dropping o/p buf out samples(%x) circ buf samples(%x)session(%x",(int)(pVptx->io.output.buf_samples), (int)(pVptx->io.output.circ_buf.circ_buf[0].unread_samples),(int)pVptx->session.session_num);
         (void) elite_msg_push_payload_to_returnq(bufq_ptr, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
         NOT_MASK(pVptx->wait_mask,VPTX_BUF_LB_MASK); // Remove BUF MASK
      }
   }
   else // there was error reading queue.
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error Popping loopback output buffer %d, %x",nResult,(int)pVptx->session.session_num);
   }
   MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_loopback_buf_handler end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_data_handler(void *pInstance)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t *pVptx = (vptx_t*)pInstance;       // instance structure
   elite_msg_any_t nearInBufMsg;
   elite_msg_any_t farInBufMsg;

   // ***************** Pop Input buffers based on the input mask
   memset(&nearInBufMsg, 0, sizeof(elite_msg_any_t));
   memset(&farInBufMsg, 0, sizeof(elite_msg_any_t));

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_data_handler begin session(%x)",(int)pVptx->nSessionNum); //TODO: not needed
   if (pVptx->rcvd_mask & VPTX_NEAR_DATA_MASK ) // check if it was near data input
   {
      // ***************** Read the input near data
      nResult = qurt_elite_queue_pop_front(pVptx->near_svc_handle.dataQ, (uint64_t*)&nearInBufMsg);
      voice_result_check(nResult,(pVptx->session.session_num));
   }

   if (pVptx->rcvd_mask & VPTX_FAR_DATA_MASK)
   {
      // ***************** Read the input far data
      nResult = qurt_elite_queue_pop_front(pVptx->far_svc_handle.dataQ, (uint64_t*)&farInBufMsg );
      voice_result_check(nResult,(pVptx->session.session_num));
   }

   // ***************** Check Media Type
   if (NULL != nearInBufMsg.pPayload)
   {
      if (ELITE_DATA_MEDIA_TYPE == nearInBufMsg.unOpCode)
      { // Check Media Type

         elite_msg_data_media_type_apr_t *pMediaTypePayload =(elite_msg_data_media_type_apr_t *)nearInBufMsg.pPayload;
         if( ELITEMSG_DATA_MEDIA_TYPE_APR == pMediaTypePayload->unMediaTypeFormat &&
               ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM   == pMediaTypePayload->unMediaFormatID)
         {

            elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk =
               (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pMediaTypePayload);

            //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Received Near MEDIA TYPE sampRate(%d)!!", (int)pMediaFormatBlk->sample_rate);
            if (ADSP_EOK != Voice_CheckMediaType(&(pVptx->io.near_in.mediatype),pMediaFormatBlk))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: AFE near_mediatype doesnot match!! session(%x)",(int)pVptx->session.session_num);
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx received mediatype from port %x. session(%x)", (int)pVptx->afe_info.afe_tx_port_id, (int)pVptx->session.session_num);
               // Flag resync for audio instance
               if(VPTX_AUDIO == pVptx->session.audio_instance_flag)
               {
                  vptx_resync(pVptx);
               }
            }
         }
         else
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Vptx received unsupported media type, format %x, type %x, session(%x)", (unsigned int)pMediaTypePayload->unMediaTypeFormat, (unsigned int)pMediaTypePayload->unMediaFormatID, (unsigned int)pVptx->session.session_num);
         }
      } //end of if (ELITE_DATA_MEDIA_TYPE == nearInBufMsg.unOpCode)
      // in case port is stopping
      else if(ELITE_DATA_EOS == nearInBufMsg.unOpCode)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx received EOD from port %x. session(%x)", (int)pVptx->afe_info.afe_tx_port_id, (int)pVptx->session.session_num);
         // Take care of flags for audio_instance
         if(VPTX_AUDIO == pVptx->session.audio_instance_flag)
         {
            pVptx->io.near_in.dataflow_on = FALSE;
         }
      }
   }

   if (NULL != farInBufMsg.pPayload)
   {
      if (ELITE_DATA_MEDIA_TYPE == farInBufMsg.unOpCode)
      {  // Check Media Type
         elite_msg_data_media_type_apr_t *pMediaTypePayload =(elite_msg_data_media_type_apr_t *)farInBufMsg.pPayload;
         if( ELITEMSG_DATA_MEDIA_TYPE_APR == pMediaTypePayload->unMediaTypeFormat &&
               ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM   == pMediaTypePayload->unMediaFormatID)
         {
            elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk =
               (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pMediaTypePayload);
            //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Received Far MEDIA TYPE sampRate(%d)!!", (int)pMediaFormatBlk->sample_rate);

            if (ADSP_EOK != Voice_CheckMediaType(&(pVptx->io.far_in.mediatype),pMediaFormatBlk))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: AFE Near Media type doesnot match!! session(%x)",(int)pVptx->session.session_num);
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx received mediatype from port %x. session(%x)", (int)pVptx->afe_info.afe_rx_port_id, (int)pVptx->session.session_num);
               if(VPTX_AUDIO == pVptx->session.audio_instance_flag)
               {
                  if(pVptx->process_data_flag)
                  {
                     //Signal that far end needs to be resynced. We don't do this right away, since you can have data queued up at near and far end
                     //that is hard to account for. Resync will happen once processing is done
                     pVptx->timing.resync_far_end = TRUE;
                  }
                  else
                  {
                     // no need for full resync, just take care of some flags
                     pVptx->io.force_far_zeros = FALSE;
                  }
               }
            }
         }
         else
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Vptx received unsupported media type, format %x, type %x, session(%x)", (unsigned int)pMediaTypePayload->unMediaTypeFormat, (unsigned int)pMediaTypePayload->unMediaFormatID, (unsigned int)pVptx->session.session_num);
         }
      } //end of if (ELITE_DATA_MEDIA_TYPE == farInBufMsg.unOpCode)
      // in case port is stopping
      else if(ELITE_DATA_EOS == farInBufMsg.unOpCode)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx received EOD from port %x. session(%x)", (int)pVptx->afe_info.afe_rx_port_id, (int)pVptx->session.session_num);
         // set state variables
         // This applies only to audio instance
         if(VPTX_AUDIO == pVptx->session.audio_instance_flag)
         {
            pVptx->io.force_far_zeros = TRUE;
            pVptx->io.far_in.dataflow_on = FALSE;
            //clear circbuf and ss info
            voice_multi_ch_circ_buf_reset(&(pVptx->io.far_in.circ_buf));
            pVptx->ratematch.ss_buf_samples_far = 0;

            capi_v2_buf_t param_data_buf;
            capi_v2_port_info_t port_info;
            port_info.is_valid = TRUE;
            port_info.port_index = 1;  // signifying far port

            param_data_buf.data_ptr = NULL; // This is just to intimate to clear SS state
            param_data_buf.actual_data_len = param_data_buf.max_data_len = 0;
            // calculate number of slip/stuff samples
            if (pVptx->modules.ss_capi.module_ptr)
            {

               pVptx->modules.ss_capi.module_ptr->vtbl_ptr->set_param(pVptx->modules.ss_capi.module_ptr,
                     SS_PARAM_RESET,&port_info,&param_data_buf);
            }
            else
            {
               pVptx->modules.special_capis[ECNS_CAPI]->module_ptr->vtbl_ptr->set_param(pVptx->modules.special_capis[ECNS_CAPI]->module_ptr,
                          SS_PARAM_RESET,&port_info,&param_data_buf);

            }
            //also memset avt drift info to zero
            memset(&(pVptx->ratematch.voice_drift_info_far), 0, sizeof(pVptx->ratematch.voice_drift_info_far));
            //In case media type is immediately followed by EOS before it can be handled
            pVptx->timing.resync_far_end = FALSE;
         }
      }
   }

   // ***************** Copy data to local circular buffer
   // TODO: Check for VFR clock when VFR is enabled
   if (pVptx->process_data_flag)
   {
      //TODO: Check for VFR Sync
      if (  // If near data payload is valid & if data is pcm samples
            (NULL != nearInBufMsg.pPayload)
            && (ELITE_DATA_BUFFER == nearInBufMsg.unOpCode)
         )
      {  // Copy near data if any to circular buffer
         elite_msg_data_buffer_t* pNearInDataPayload = (elite_msg_data_buffer_t*) nearInBufMsg.pPayload;

         // In soft or hard vfr cases, checking for vfr sync is sufficient. For audio path, we either need to have far data available
         // or we need to be forcing zeros (either due to far end not provided or far end port stopped)
         // For FTRT, always copy near end data. Only current use case is in call audio delivery, which is none topo, and has no far ref
         // With smart sync tick, for 20 ms topo, if far data is on, start collecting near data. No need to wait for sync tick
         // for 10 ms topo, it needs to wait for sync tick before starting to collect data
         if (((VPTX_AUDIO == pVptx->session.audio_instance_flag) && ((TRUE == pVptx->io.far_in.dataflow_on)||(TRUE  == pVptx->io.force_far_zeros)))
               || ((20== pVptx->io.num_1ms_frame_block_process) && (TRUE == pVptx->io.far_in.dataflow_on))
               || (TRUE == pVptx->timing.ref_time_tick_recvd_flag)
               || (!pVptx->session.real_time_flag))
         {
            // Update time stamp at the beginning of every vfr cycle
            if((pVptx->io.near_in.circ_buf.circ_buf[0].unread_samples == pVptx->ratematch.ss_buf_samples_near)&&(0 == pVptx->session.vfr_vptx_cntr))
            {
               pVptx->io.ull_timestamp = pNearInDataPayload->ullTimeStamp - pVptx->timing.modules_delay;
	    }
            voice_multi_ch_circ_buf_block_write(&(pVptx->io.near_in.circ_buf), (int8_t*)&(pNearInDataPayload->nDataBuf),
                  (int32_t)(pNearInDataPayload->nActualSize >> 1));
#if defined(LOG_VOICEPROC_DATA) && defined(SIM) // For Sim debugging
            if( pVptx->session.sampling_rate == VOICE_NB_SAMPLING_RATE)
            {
               FILE *fp;
               int16_t *pIn = (int16_t*)&(pNearInDataPayload->nDataBuf);
               fp = fopen("vptx_near_in_nb.raw","a"); // in append mode
               fwrite(pIn,sizeof(int16_t),pNearInDataPayload->nActualSize>>1,fp);
               fclose(fp);
            }
            else
            {
               FILE *fp;
               int16_t *pIn = (int16_t*)&(pNearInDataPayload->nDataBuf);
               fp = fopen("vptx_near_in_wb.raw","a"); // in append mode
               fwrite(pIn,sizeof(int16_t),pNearInDataPayload->nActualSize>>1, fp);
               fclose(fp);
            }
#endif

            //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Copy Near Data to circ Buf %d session(%x)",(int)pVptx->io.near_in.circ_buf.circ_buf[0].unread_samples,(int)pVptx->session.session_num);
         }
         pVptx->io.near_in.dataflow_on = TRUE;
      }

      if (  // If far data payload is valid & if data is pcm samples
            (NULL != farInBufMsg.pPayload)
            && (ELITE_DATA_BUFFER == farInBufMsg.unOpCode)
         )
      {  // Copy far data if any to circular buffer
         elite_msg_data_buffer_t* pFarInDataPayload = (elite_msg_data_buffer_t*) farInBufMsg.pPayload;

         // With smart sync tick, for 20 ms topo, if near data is on, start collecting near data (same as audio instance). No need to wait for sync tick
         // for 10 ms topo, it needs to wait for sync tick before starting to collect data
         if ((((VPTX_AUDIO == pVptx->session.audio_instance_flag)||(20 == pVptx->io.num_1ms_frame_block_process)) && (TRUE == pVptx->io.near_in.dataflow_on))
             || (TRUE == pVptx->timing.ref_time_tick_recvd_flag))
         {
            voice_multi_ch_circ_buf_block_write(&(pVptx->io.far_in.circ_buf), (int8_t*)&(pFarInDataPayload->nDataBuf),
                  (int32_t)(pFarInDataPayload->nActualSize >> 1));
            //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Copy Far Data to circ Buf %d session(%x)",(int)pVptx->circ_buf_far.circ_buf[0].unread_samples,(int)pVptx->session.session_num);
#if defined(LOG_VOICEPROC_DATA) && defined(SIM) // For Sim debugging
            if( pVptx->session.sampling_rate == VOICE_NB_SAMPLING_RATE)
            {
               FILE *fp;
               int16_t *pIn = (int16_t*)(&(pFarInDataPayload->nDataBuf));
               fp = fopen("vptx_far_in_nb.raw","a"); // in append mode
               fwrite(pIn,sizeof(int16_t),pFarInDataPayload->nActualSize>>1,fp);
               fclose(fp);
            }
            else
            {
               FILE *fp;
               int16_t *pIn = (int16_t*)(&(pFarInDataPayload->nDataBuf));
               fp = fopen("vptx_far_in_wb.raw","a"); // in append mode
               fwrite(pIn,sizeof(int16_t),pFarInDataPayload->nActualSize>>1, fp);
               fclose(fp);
            }
#endif
         }
         pVptx->io.far_in.dataflow_on = TRUE;
      }

   }
   //else Dont read any data and ignore all the input buffers

   // ***************** Return buffers
   if (NULL != nearInBufMsg.pPayload)
   {  // Return Near buffer
      nResult = elite_msg_return_payload_buffer ( &nearInBufMsg );
      if (ADSP_FAILED(nResult))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed return Near end buffer session(%x)",(int)pVptx->session.session_num);
      }
      nearInBufMsg.pPayload = NULL;
   }

   if (NULL != farInBufMsg.pPayload)
   {  // Return Far buffer
      nResult = elite_msg_return_payload_buffer ( &farInBufMsg );
      if (ADSP_FAILED(nResult))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed return Far end buffer session(%x)",(int)pVptx->session.session_num);
      }
      farInBufMsg.pPayload = NULL;
   }

   // ***************** Check if both near and far data is enough to set counter for vptx_process data also make sure that process has started
   if ( (pVptx->proc_start_samples <= (pVptx->io.near_in.circ_buf.circ_buf[0].unread_samples - pVptx->ratematch.ss_buf_samples_near))
           && (pVptx->proc_start_samples <= (pVptx->io.far_in.circ_buf.circ_buf[0].unread_samples - pVptx->ratematch.ss_buf_samples_far)||(TRUE  == pVptx->io.force_far_zeros))
           && (( TRUE == pVptx->process_start_flag )||( TRUE == pVptx->timing.ref_time_tick_recvd_flag )))
   {
       pVptx->session.vfr_vptx_cntr = pVptx->session.vfr_cycle;
   }

   //& Start the process if vfr counter is non zero and we have sufficient samples
   /* Also check if  this is a realtime session. If it is not,
    * then VPTx does not subscribe to timer and sync tick wont be received.
    * */
   if (  ( (pVptx->session.vfr_vptx_cntr > 0)|| (VPTX_AUDIO == pVptx->session.audio_instance_flag) || (FALSE == pVptx->session.real_time_flag) )
           && (pVptx->io.near_in.frame_samples <= (pVptx->io.near_in.circ_buf.circ_buf[0].unread_samples - pVptx->ratematch.ss_buf_samples_near))
           && ((pVptx->io.far_in.frame_samples <= (pVptx->io.far_in.circ_buf.circ_buf[0].unread_samples - pVptx->ratematch.ss_buf_samples_far))|| (TRUE  == pVptx->io.force_far_zeros))
      )
   {

     // MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx process triggered near_unread (%ld) far_unread(%ld)",
     //         (pVptx->io.near_in.circ_buf.circ_buf[0].unread_samples - pVptx->ratematch.ss_buf_samples_near),
     //         (pVptx->io.far_in.circ_buf.circ_buf[0].unread_samples - pVptx->ratematch.ss_buf_samples_far));

      uint64_t time = qurt_elite_timer_get_time();
      uint64_t cycles = qurt_elite_profile_get_pcycles();

      vptx_process(pVptx);

      time = qurt_elite_timer_get_time() - time;
      cycles = qurt_elite_profile_get_pcycles() - cycles;

      voice_cmn_time_profile_add_data( &pVptx->profiler, (uint32_t) time, cycles);

      //Do the resync only if far end data was actually received between the time the mediatype came in and the buffer was processed
      // We dont want to disable force_far_zeros until we know that rx device is actually running, and not just port started
      if((TRUE == pVptx->timing.resync_far_end) && (TRUE == pVptx->io.far_in.dataflow_on))
      {
         vptx_far_resync(pVptx);
      }
   }

   // ***************** check if output data is enough, switch to output mask
   if (pVptx->io.output.buf_samples == (pVptx->io.output.circ_buf.circ_buf[0].unread_samples))
   {
      //dbg:      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Setting output buf mask %d  session(%x)",
      //dbg:            (int)pVptx->io.output.circ_buf.circ_buf[0].unread_samples,(int)pVptx->session.session_num);
      if (pVptx->session.real_time_flag)
      {
         OR_MASK(pVptx->wait_mask,(VPTX_NEAR_DATA_MASK | VPTX_FAR_DATA_MASK)); // Add Data Mask
         // (0 == pVptx->timing.venc_delivery_tick_vtm_struct.signal_enable) indicate either
         // AUDIO use case or non real time use case or
         // max tx processing time <= 20 ms or
         // vptx delivery tick is being used
         // In any of the above mentioned scenarios BUF MASK should be enabled here and we should not wait for any other event to enable BUF MASK
         // (TRUE == pVptx->is_venc_tick_fired) indicates that venc delivery tick is already handled for this frame
         // and hence its safe to enable BUF MASK now.
         if((0 == pVptx->timing.venc_delivery_tick_vtm_struct.signal_enable) || (TRUE == pVptx->is_venc_tick_fired))
         {
            OR_MASK(pVptx->wait_mask,VPTX_BUF_MASK);
         }
      }
      else
      {
         NOT_MASK(pVptx->wait_mask,(VPTX_NEAR_DATA_MASK | VPTX_FAR_DATA_MASK)); // Remove Data Mask and have buf mask only
         OR_MASK(pVptx->wait_mask,VPTX_BUF_MASK);      //  Always listen to BUF MASK here for non real time use cases
      }

   }
   // check if loopback circular buffer has enough data
   if ( pVptx->loopback_enable_flag && (pVptx->io.loopback_output.buf_samples == (pVptx->io.loopback_output.circ_buf.circ_buf[0].unread_samples)))
   {
      //dbg:      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Setting output buf mask %d for gpQ loopback session(%x)",
      //dbg:            (int)pVptx->io.loopback_output.circ_buf.circ_buf[0].unread_samples,(int)pVptx->session.session_num);

      OR_MASK(pVptx->wait_mask, VPTX_BUF_LB_MASK);
   }
   return nResult;
}

/* Since we always wait synchronously for responses, this should never get invoked */
static ADSPResult vptx_response_handler(void* pInstance)
{
   vptx_t *pVptx = (vptx_t*)pInstance;       // instance structure
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! vptx_response_handler invoked, session(%x)",(int)pVptx->session.session_num);
   return ADSP_EFAILED;
}

static ADSPResult vptx_resync_handler(void* pInstance)
{
   vptx_t *pVptx = (vptx_t*)pInstance;       // instance structure
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx resync handler invoked, session(%x)",(int)pVptx->session.session_num);
   qurt_elite_signal_clear(pVptx->timing.sync_tick_vtm_struct.resync_signal_ptr);
   pVptx->resync_count++;
   vptx_resync(pVptx);
   // sync to next VFR
   pVptx->timing.ref_time_tick_recvd_flag = FALSE;
   // Clear both these flags in resync to go to default state
   pVptx->is_venc_tick_fired = FALSE;
   pVptx->process_start_flag = FALSE;
   return ADSP_EOK;
}

/* =======================================================================
 **                          Message handler functions
 ** ======================================================================= */

static ADSPResult vptx_destroy_yourself_cmd (void *pInstance, elite_msg_any_t* pMsg)
{
   vptx_destroy((vptx_t*)pInstance);

   elite_msg_finish_msg(pMsg, ADSP_EOK);

   // send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
   return ADSP_ETERMINATED;
}

static ADSPResult vptx_connect_dwn_stream_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   elite_msg_cmd_connect_t* pConnectMsgPayload;
   elite_svc_handle_t *pSvc2Connect;
   vptx_t* pVptx = (vptx_t*)pInstance;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_connect_dwn_stream_cmd begin session(%x)",(int)pVptx->session.session_num);
   if (NULL == pVptx)
   {
      return ADSP_EFAILED;
   }

   if (pVptx->process_data_flag)
   {
      nResult = ADSP_ENOTREADY; // Cannot reconnect in RUN state.
   }

   pConnectMsgPayload = (elite_msg_cmd_connect_t*) (pMsg->pPayload);

   pSvc2Connect = pConnectMsgPayload->pSvcHandle;
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VoiceprocTx connecting to SvcID 0x%8x session(%x)",(int)pSvc2Connect->unSvcId,(int)pVptx->session.session_num);

   // This service only allows one downstream
   if (NULL != pVptx->downstream_peer_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: FAILED, only 1 peer allowed session(%x)",(int)pVptx->session.session_num);
      nResult = ADSP_EUNSUPPORTED;
   }
   else //else accept the connection
   {
      pVptx->downstream_peer_ptr = pSvc2Connect;
   }
   elite_svc_send_ack(pMsg, nResult);

   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_connect_dwn_stream_cmd end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

// Caller is expected to only send this message during a good state!!!
static ADSPResult vptx_disconnect_dwn_stream_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   elite_msg_cmd_connect_t* pDisconnectMsgPayload;
   elite_svc_handle_t *pSvc2Disconnect;
   vptx_t* pVptx = (vptx_t*)pInstance;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx_DisConnectDwnStreamCmd begin session(%x)",(int)pVptx->session.session_num);
   if (NULL == pVptx)
   {
      return ADSP_EFAILED;
   }
   if(pVptx->process_data_flag)
   {
      // Cannot disconnect Vptx down stream service until it was set to stop earlier
      nResult = ADSP_EBUSY;
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ERROR ::Trying to disconnect before Vptx stop command(%d)",(int)pVptx->session.session_num);
   }
   else
   {
      pDisconnectMsgPayload = (elite_msg_cmd_connect_t*) (pMsg->pPayload);

      pSvc2Disconnect = pDisconnectMsgPayload->pSvcHandle ;
      if (pSvc2Disconnect != pVptx->downstream_peer_ptr)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: FAILED, not connected to that service. session(%x)",(int)pVptx->session.session_num);
         nResult = ADSP_EBADPARAM;
      }
      else //else accept the connection
      {
         pVptx->downstream_peer_ptr = NULL;
      }
   }
   elite_svc_send_ack(pMsg, nResult);
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx_DisConnectDwnStreamCmd end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_connect_rx_peer_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EBADPARAM;
   elite_msg_custom_voc_svc_connect_type* pConnectMsgPayload;
   elite_svc_handle_t *pSvc2Connect;
   vptx_t* pVptx = (vptx_t*)pInstance;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_connect_rx_peer_cmd begin session(%x)",(int)pVptx->session.session_num);
   if (NULL == pVptx)
   {
      return ADSP_EFAILED;
   }
   pConnectMsgPayload = (elite_msg_custom_voc_svc_connect_type*) (pMsg->pPayload);

   pSvc2Connect = pConnectMsgPayload->svc_handle_ptr;
   if(VOICEPROC_CONNECT_RX_PEER == pConnectMsgPayload->sec_opcode)
   {
      pVptx->rx_peer_ptr = pSvc2Connect;
      nResult = ADSP_EOK;
   }
   elite_svc_send_ack(pMsg, nResult);

   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_connect_rx_peer_cmd end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_disconnect_rx_peer_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EBADPARAM;
   elite_msg_custom_voc_svc_connect_type* pDisconnectMsgPayload;
   vptx_t* pVptx = (vptx_t*)pInstance;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx_DisConnectRxPeerCmd begin session(%x)",(int)pVptx->session.session_num);
   if (NULL == pVptx)
   {
      return ADSP_EFAILED;
   }
   pDisconnectMsgPayload = (elite_msg_custom_voc_svc_connect_type*) (pMsg->pPayload);
   if(VOICEPROC_DISCONNECT_RX_PEER == pDisconnectMsgPayload->sec_opcode)
   {
      pVptx->rx_peer_ptr = NULL;

      /* if we are disconnecting the Rx peer, loopback should have been disabled already -> flag an error */
      if( TRUE == pVptx->loopback_enable_flag )
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vptx_DisConnectRxPeerCmd loopback left enabled, forcing off : session(%x)",(int)pVptx->session.session_num);
         pVptx->loopback_enable_flag = FALSE;
         nResult = ADSP_EOK;
      }
      else
      {
         nResult = ADSP_EOK;
      }
   }

   elite_svc_send_ack(pMsg, nResult);

   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx_DisConnectRxPeerCmd end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_set_param_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   // TODO: should send an ACK back to caller to avoid race conditions, and to inform success/fail.
   vptx_t* pVptx = (vptx_t*)pInstance;
   ADSPResult nResult = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_set_param_cmd begin session(%x)",(int)pVptx->session.session_num);

   if (VPM_TX_NONE == pVptx->session.topology_id) // no calibration support in none topology
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: No calibration supported on none topology-returning. session(%x)",(int)pVptx->session.session_num);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return nResult;
   }

   if (ELITE_CMD_SET_PARAM == pMsg->unOpCode)
   {
      elite_msg_param_cal_t* pFaddCalPayload = (elite_msg_param_cal_t*) pMsg->pPayload;
      switch (pFaddCalPayload->unParamId)
      {
         case ELITEMSG_PARAM_ID_CAL:
         case ELITEMSG_PARAM_ID_CAL_V2:
         {
            // extract Voice params payload pointer
            uint32_t nPayload_address = (uint32_t)pFaddCalPayload->pnParamData; // address of voice_param_data_t
            uint32_t nPayload_size = pFaddCalPayload->unSize;

            //param size and payload address have to be multiples of 4
            if((0 == nPayload_size)  || (NULL == nPayload_address))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vptx recieved wrong cal! Addr %#x, size %d", (int)nPayload_address, (int)nPayload_size);
               elite_svc_send_ack(pMsg, ADSP_EBADPARAM);
               return ADSP_EOK;
            }

            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx set param, voice_param_data_t address(%x) size(%x)",(int)nPayload_address,(int)pFaddCalPayload->unSize);

            {
               ADSPResult nAggregateResult = ADSP_EOK;
               nAggregateResult = vptx_modules_set_param(pVptx, nPayload_address, nPayload_size,pFaddCalPayload->unParamId );

               vptx_check_set_internal_param(pVptx);

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx sending ack %d", nAggregateResult);
               elite_svc_send_ack(pMsg, nAggregateResult);
            }
            break;
         }
         default:
         {
            elite_svc_send_ack(pMsg, ADSP_EFAILED);
            // No local set param
            return ADSP_EOK;
         }
      } // End of switch case pFaddCalPayload->unParamId
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_set_param_cmd secondary opcode not supported session(%x)",(int)pVptx->session.session_num);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EOK;
   }

   if (1 == pVptx->process_data_flag)
   {
      uint32_t offset_changed_flag=FALSE;
      if((VPTX_AUDIO != pVptx->session.audio_instance_flag) && (pVptx->session.real_time_flag))
      {
         //vcp does offset calcuation in case of old timing
         if (FALSE == pVptx->timing.use_new_timing_flag)
         {
            (void)vptx_aggregate_modules_kpps(pVptx, &offset_changed_flag); // discarding for now since result is always success
            vptx_calc_offsets(pInstance); // recalculate the offsets after set param in run state
         }

         if (TRUE == offset_changed_flag)
         {
            // If old timing use case then fall back to vptx delivery tick design
            pVptx->timing.vptx_delivery_tick_vtm_struct.signal_enable  = 1;
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx sync offset changed, hence resyncing session(%x)", (int)pVptx->session.session_num);
            // Unsubsribe and resubscribe to timer for changed offset
            // This function will wait synchronously for response
            vptx_vtm_unsubscribe(pVptx, &(pVptx->timing.sync_tick_vtm_struct), VPTX_SYNC_TICK_END_MASK);
            vptx_vtm_unsubscribe(pVptx, &(pVptx->timing.venc_delivery_tick_vtm_struct), VPTX_VENC_DELIVERY_TICK_END_MASK);
            //It is possible to have a race condition where the sync signal was already set while unsubscribing
            //In this case the resync will again happen at the wrong time. To guard against this situation, we clear the sync signal here
            qurt_elite_signal_clear(pVptx->timing.sync_tick_vtm_struct.signal_ptr);
            // Need to unsub/sub delivery tick also. The new signal from VDS can be different.
            vptx_vtm_unsubscribe(pVptx, &(pVptx->timing.vptx_delivery_tick_vtm_struct), VPTX_DELIVERY_TICK_END_MASK);
            // Also unsubscribe from VDS to ensure queued requests are removed
            nResult = voice_cmn_send_vds_command(&pVptx->timing.vds_client_id, pVptx->timing.vds_client_token, pVptx->response_q, VDS_UNSUBSCRIBE,
                                      &pVptx->timing.vptx_delivery_tick_vtm_struct.signal_ptr, NULL, pVptx->timing.vds_handle_ptr, pVptx->session.session_num, (1 == pVptx->timing.vptx_delivery_tick_vtm_struct.signal_enable),NULL);

            if (ADSP_FAILED(nResult))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vptx unsubscribe to VDS session(%x)!",(int)pVptx->session.session_num);
               return nResult;
            }
            // Time to resubscribe
            nResult = voice_cmn_send_vds_command(&pVptx->timing.vds_client_id, pVptx->timing.vds_client_token, pVptx->response_q, VDS_SUBSCRIBE,
                                      &pVptx->timing.vptx_delivery_tick_vtm_struct.signal_ptr, NULL, pVptx->timing.vds_handle_ptr, pVptx->session.session_num, (1 == pVptx->timing.vptx_delivery_tick_vtm_struct.signal_enable),NULL);
            if (ADSP_FAILED(nResult))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vptx subscribe to VDS session(%x)!",(int)pVptx->session.session_num);
               return nResult;
            }
            // Now we can safely resubscribe
            pVptx->timing.sync_tick_vtm_struct.offset         = pVptx->timing.sync_offset;
            pVptx->timing.sync_tick_vtm_struct.signal_enable  = 1;
            pVptx->timing.sync_tick_vtm_struct.vfr_mode       = pVptx->session.vfr_mode;
            pVptx->timing.sync_tick_vtm_struct.vsid           = pVptx->timing.vsid;
            pVptx->timing.sync_tick_vtm_struct.timing_ver     = pVptx->timing.timing_ver; //update version of timing used

            if (ADSP_FAILED(nResult = voice_custom_vt_sub_unsub_msg_send(NULL,pVptx->timing.vtm_cmdq_ptr,VOICE_TIMER_SUBSCRIBE,NULL,FALSE,&(pVptx->timing.sync_tick_vtm_struct))))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vptx subscribe to Vtm session(%x)!",(int)pVptx->session.session_num);
               return nResult;
            }

            if (VFR_NONE == pVptx->session.vfr_mode)
            {
               (void)qurt_elite_channel_wait(&pVptx->qurt_elite_channel, VPTX_SYNC_TICK_END_MASK);
               qurt_elite_signal_clear(pVptx->timing.sync_tick_vtm_struct.signal_end_ptr); // clear the signal
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx Done waiting for sync signal from VTM; session (%x)",(int)pVptx->session.session_num);
            }

            pVptx->timing.vptx_delivery_tick_vtm_struct.offset         = pVptx->timing.delivery_offset;
            pVptx->timing.vptx_delivery_tick_vtm_struct.vfr_mode       = pVptx->session.vfr_mode;
            pVptx->timing.vptx_delivery_tick_vtm_struct.vsid           = pVptx->timing.vsid;
            pVptx->timing.vptx_delivery_tick_vtm_struct.timing_ver     = pVptx->timing.timing_ver; //update version of timing used
            if (ADSP_FAILED(nResult = voice_custom_vt_sub_unsub_msg_send(NULL,pVptx->timing.vtm_cmdq_ptr,VOICE_TIMER_SUBSCRIBE,NULL,FALSE,&(pVptx->timing.vptx_delivery_tick_vtm_struct))))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vptx subscribe to Vtm session(%x)!",(int)pVptx->session.session_num);
               return nResult;
            }
            if (VFR_NONE == pVptx->session.vfr_mode)
            {
               (void)qurt_elite_channel_wait(&pVptx->qurt_elite_channel, VPTX_DELIVERY_TICK_END_MASK);
               qurt_elite_signal_clear(pVptx->timing.vptx_delivery_tick_vtm_struct.signal_end_ptr); // clear the signal
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx Done waiting for delivery signal from VTM; session (%x)",(int)pVptx->session.session_num);
            }

            pVptx->timing.venc_delivery_tick_vtm_struct.offset         = pVptx->timing.enc_offset;
            pVptx->timing.venc_delivery_tick_vtm_struct.signal_enable  = 0;
            pVptx->timing.venc_delivery_tick_vtm_struct.vfr_mode       = pVptx->session.vfr_mode;
            pVptx->timing.venc_delivery_tick_vtm_struct.vsid           = pVptx->timing.vsid;
            pVptx->timing.venc_delivery_tick_vtm_struct.timing_ver     = pVptx->timing.timing_ver; //update version of timing used

            if (ADSP_FAILED(nResult = voice_custom_vt_sub_unsub_msg_send(NULL,pVptx->timing.vtm_cmdq_ptr,VOICE_TIMER_SUBSCRIBE,NULL,FALSE,&(pVptx->timing.venc_delivery_tick_vtm_struct))))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vptx subscribe to Vtm session(%x)!",(int)pVptx->session.session_num);
               return nResult;
            }

            if (VFR_NONE == pVptx->session.vfr_mode)
            {
               (void)qurt_elite_channel_wait(&pVptx->qurt_elite_channel, VPTX_VENC_DELIVERY_TICK_END_MASK);
               qurt_elite_signal_clear(pVptx->timing.venc_delivery_tick_vtm_struct.signal_end_ptr); // clear the signal
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx Done waiting for venc delivery  signal from VTM; session (%x)",(int)pVptx->session.session_num);
            }


            vptx_resync(pVptx); // resyc only if the processing time increased
            pVptx->timing.ref_time_tick_recvd_flag = FALSE; //set param not coordinated with time tick, so specifically need to resync
         }
      }
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_set_param_cmd end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_get_param_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   vptx_t* pVptx = (vptx_t*) pInstance;
   ADSPResult nResult = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_get_param_cmd begin session(%x)",(int)pVptx->session.session_num);

   if (VPM_TX_NONE == pVptx->session.topology_id)  // no calibration support in none topology
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: No calibration supported on none topology-returning. session(%x)",(int)pVptx->session.session_num);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return nResult;
   }

   if (ELITE_CMD_GET_PARAM == pMsg->unOpCode)
   {
      elite_msg_param_cal_t* pFaddPayload = (elite_msg_param_cal_t*) pMsg->pPayload;
      uint32_t param_size;
      if( (ELITEMSG_PARAM_ID_CAL == pFaddPayload->unParamId) ||
            (ELITEMSG_PARAM_ID_CAL_V2 == pFaddPayload->unParamId) )
      {
         ADSPResult nLocalResult = ADSP_EUNSUPPORTED;

         nLocalResult = vptx_modules_get_param(pVptx, (void *)pFaddPayload->pnParamData,pFaddPayload->unParamId, &param_size);
         // Need to return actual size filled, static service will populate this field in the get param
         // This is to accomodate cases where the same mod id/param id pair exists on both tx and rx paths
         pFaddPayload->unSize = param_size;

         elite_svc_send_ack(pMsg, nLocalResult);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Tx get param bad fadd id,  result(0x%x) session(0x%lx)\n",
               ADSP_EFAILED, pVptx->session.session_num);
         elite_svc_send_ack(pMsg, ADSP_EFAILED);
         // No local get param
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_get_param_cmd secondary opcode not supported session(0x%x)",(int)pVptx->session.session_num);
      nResult = ADSP_EFAILED;
      elite_svc_send_ack(pMsg, nResult);
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_get_param_cmd end handler result(0x%x) session(0x%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_run_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   // TODO: should send an ACK back to caller to avoid race conditions, and to inform success/fail.
   vptx_t* pVptx = (vptx_t*)pInstance;
   elite_msg_custom_voiceproc_run_payload_t *payload_ptr;
   ADSPResult nResult = ADSP_EOK;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_run_cmd begin session(%x)",(int)pVptx->session.session_num);

   // Check if this is custom RUN.
   // If yes, then extract drift ptr which is passed over from AFE->VPM during AFE connect ack back.
   // This drift ptr is then propogated to Vtm which then gets the drift info in this new scheme
   // compared to old scheme where device id is used as an index into drift array.
   if (ELITE_CUSTOM_MSG == pMsg->unOpCode)
   {
      payload_ptr = (elite_msg_custom_voiceproc_run_payload_t *) pMsg->pPayload;
      if (VOICEPROC_RUN_CMD == payload_ptr->unSecOpCode)
      {
         pVptx->afe_info.afe_tx_drift_ptr = payload_ptr->afe_drift_ptr;
         pVptx->afe_info.afe_rx_drift_ptr = payload_ptr->far_src_drift_ptr;
         pVptx->timing.session_thread_clock_mhz = payload_ptr->session_thread_clock_mhz;
      }
      else
      {
         pVptx->afe_info.afe_tx_drift_ptr = NULL;
         pVptx->afe_info.afe_rx_drift_ptr = NULL;
         pVptx->timing.session_thread_clock_mhz = 0;

         nResult = ADSP_EUNSUPPORTED;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_run_cmd invalid sec opcode(%d), session(%x)",(int)payload_ptr->unSecOpCode,(int)pVptx->session.session_num);
      }
   }

   if (ADSP_EOK == nResult)
   {
      // don't subscribe to voice timer if the instance is in audio path or if running FTRT
      // for in call audio delivery path
      if((VPTX_AUDIO != pVptx->session.audio_instance_flag) && (pVptx->session.real_time_flag))
      {
         uint32_t offset_changed_flag=FALSE;

         // Enable here, If old timing use case or  tx processing time >= 39ms , then fall back to vptx delivery tick design 
         pVptx->timing.vptx_delivery_tick_vtm_struct.signal_enable  = 1;

         //vcp does offset calcuation in case of old timing
         if (FALSE == pVptx->timing.use_new_timing_flag)
         {
            // Aggregate KPPS
            (void)vptx_aggregate_modules_kpps(pVptx, &offset_changed_flag); // discarding for now since result is always success
            // Calculate offsets
            vptx_calc_offsets(pInstance);
         }
         else if((pVptx->max_tx_processing_time <= MAX_TX_PROCESSING_TIME_IN_US)||( pVptx->session.vfr_cycle > 1 ))
         {
            // Disable this signal when its a new timing use case and max tx processing time is <= 40 - 1 ms ( 1 ms margin is used to avoid corner cases due to jitters or sample slip)
            pVptx->timing.vptx_delivery_tick_vtm_struct.signal_enable  = 0;
         }
         pVptx->session.vfr_vptx_cntr = 0; // resetting vptx count per vfr cycle

         pVptx->timing.sync_tick_vtm_struct.offset         = pVptx->timing.sync_offset;
         pVptx->timing.sync_tick_vtm_struct.signal_enable  = (20 == pVptx->io.num_1ms_frame_block_process) ? 0 : 1; // don't listen to buf_start for 20ms topo
         pVptx->timing.sync_tick_vtm_struct.vfr_mode       = pVptx->session.vfr_mode;
         pVptx->timing.sync_tick_vtm_struct.vfr_cycle      = (uint16_t)(pVptx->session.vfr_cycle*MAX_TIMER_OFFSET);
         pVptx->timing.sync_tick_vtm_struct.vsid           = pVptx->timing.vsid;
         pVptx->timing.sync_tick_vtm_struct.timing_ver     = pVptx->timing.timing_ver; //update version of timing used

         if (ADSP_FAILED(nResult = voice_custom_vt_sub_unsub_msg_send(NULL,pVptx->timing.vtm_cmdq_ptr,VOICE_TIMER_SUBSCRIBE,NULL,FALSE,&(pVptx->timing.sync_tick_vtm_struct))))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vptx subscribe to Vtm session(%x), shifting to none vfr",(int)pVptx->session.session_num);
            elite_svc_send_ack(pMsg, nResult); // Cannot proceed, return error
            return nResult;
         }

         pVptx->timing.proc_tick_vtm_struct.offset         = pVptx->timing.proc_offset;
         pVptx->timing.proc_tick_vtm_struct.signal_enable  = 1;
         pVptx->timing.proc_tick_vtm_struct.vfr_mode       = pVptx->session.vfr_mode;
         pVptx->timing.proc_tick_vtm_struct.vfr_cycle      = (uint16_t)(pVptx->session.vfr_cycle*MAX_TIMER_OFFSET);
         pVptx->timing.proc_tick_vtm_struct.vsid           = pVptx->timing.vsid;
         pVptx->timing.proc_tick_vtm_struct.timing_ver     = pVptx->timing.timing_ver; //update version of timing used

         if (ADSP_FAILED(nResult = voice_custom_vt_sub_unsub_msg_send(NULL,pVptx->timing.vtm_cmdq_ptr,VOICE_TIMER_SUBSCRIBE,NULL,FALSE,&(pVptx->timing.proc_tick_vtm_struct))))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vptx subscribe to Vtm session(%x), shifting to none vfr",(int)pVptx->session.session_num);
            elite_svc_send_ack(pMsg, nResult); // Cannot proceed, return error
            return nResult;
         }

         if (VFR_NONE == pVptx->session.vfr_mode)
         {
            (void)qurt_elite_channel_wait(&pVptx->qurt_elite_channel, VPTX_PROC_TICK_END_MASK);
            qurt_elite_signal_clear(pVptx->timing.proc_tick_vtm_struct.signal_end_ptr); // clear the signal
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx Done waiting for proc signal from VTM; session (%x)",(int)pVptx->session.session_num);
         }

         // must subscribe to vds before subscribing to delivery tick
         if(ADSP_FAILED(nResult = voice_cmn_send_vds_command(&pVptx->timing.vds_client_id, pVptx->timing.vds_client_token,
                                     pVptx->response_q, VDS_SUBSCRIBE, &pVptx->timing.vptx_delivery_tick_vtm_struct.signal_ptr,
                                      NULL, pVptx->timing.vds_handle_ptr, pVptx->session.session_num, (1 == pVptx->timing.vptx_delivery_tick_vtm_struct.signal_enable),NULL)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vptx subscribe to VDS session(%x)!",(int)pVptx->session.session_num);
            elite_svc_send_ack(pMsg, nResult); // Cannot proceed, return error
            return nResult;
         }

         pVptx->timing.vptx_delivery_tick_vtm_struct.offset         = pVptx->timing.delivery_offset;
         pVptx->timing.vptx_delivery_tick_vtm_struct.vfr_mode       = pVptx->session.vfr_mode;
         pVptx->timing.vptx_delivery_tick_vtm_struct.vfr_cycle      = (uint16_t)(pVptx->session.vfr_cycle*MAX_TIMER_OFFSET);
         pVptx->timing.vptx_delivery_tick_vtm_struct.vsid           = pVptx->timing.vsid;
         pVptx->timing.vptx_delivery_tick_vtm_struct.timing_ver     = pVptx->timing.timing_ver; //update version of timing used

         if (ADSP_FAILED(nResult = voice_custom_vt_sub_unsub_msg_send(NULL,pVptx->timing.vtm_cmdq_ptr,VOICE_TIMER_SUBSCRIBE,NULL,FALSE,&(pVptx->timing.vptx_delivery_tick_vtm_struct))))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vptx subscribe to Vtm session(%x), shifting to none vfr",(int)pVptx->session.session_num);
            vptx_vtm_unsubscribe(pInstance, &(pVptx->timing.sync_tick_vtm_struct), VPTX_SYNC_TICK_END_MASK);
            elite_svc_send_ack(pMsg, nResult); // Cannot proceed, return error
            return nResult;
         }
         if (VFR_NONE == pVptx->session.vfr_mode)
         {
            (void)qurt_elite_channel_wait(&pVptx->qurt_elite_channel, VPTX_DELIVERY_TICK_END_MASK);
            qurt_elite_signal_clear(pVptx->timing.vptx_delivery_tick_vtm_struct.signal_end_ptr); // clear the signal
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx Done waiting for delivery signal from VTM; session (%x)",(int)pVptx->session.session_num);
         }

         // venc delivery tick is required only when all of the below mentioned cases are true
         // i)   Its not AUDIO or any non real time use case
         // ii)  vptx+venc processing time is greater than 20 ms
         // iii) VPTX delivery tick is not enabled 
         if((pVptx->max_tx_processing_time > pVptx->session.vfr_cycle*MAX_TIMER_OFFSET) && (0 == pVptx->timing.vptx_delivery_tick_vtm_struct.signal_enable))
         {
            pVptx->timing.venc_delivery_tick_vtm_struct.signal_enable  = 1;
         }
         else
         {
            pVptx->timing.venc_delivery_tick_vtm_struct.signal_enable  = 0;
         }

         pVptx->timing.venc_delivery_tick_vtm_struct.offset         = pVptx->timing.enc_offset;
         pVptx->timing.venc_delivery_tick_vtm_struct.vfr_mode       = pVptx->session.vfr_mode;
         pVptx->timing.venc_delivery_tick_vtm_struct.vfr_cycle      = (uint16_t)(pVptx->session.vfr_cycle*MAX_TIMER_OFFSET);
         pVptx->timing.venc_delivery_tick_vtm_struct.vsid           = pVptx->timing.vsid;
         pVptx->timing.venc_delivery_tick_vtm_struct.timing_ver     = pVptx->timing.timing_ver; //update version of timing used

         if (ADSP_FAILED(nResult = voice_custom_vt_sub_unsub_msg_send(NULL,pVptx->timing.vtm_cmdq_ptr,VOICE_TIMER_SUBSCRIBE,NULL,FALSE,&(pVptx->timing.venc_delivery_tick_vtm_struct))))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vptx subscribe to Vtm session(%x), shifting to none vfr",(int)pVptx->session.session_num);
            elite_svc_send_ack(pMsg, nResult); // Cannot proceed, return error
            return nResult;
         }

         if (VFR_NONE == pVptx->session.vfr_mode)
         {
            (void)qurt_elite_channel_wait(&pVptx->qurt_elite_channel, VPTX_VENC_DELIVERY_TICK_END_MASK);
            qurt_elite_signal_clear(pVptx->timing.venc_delivery_tick_vtm_struct.signal_end_ptr); // clear the signal
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx Done waiting for enc delivery signal from VTM; session (%x)",(int)pVptx->session.session_num);
         }


      }
      
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VPTX VENC delivery tick signal enable (%d) and VPTX delivery tick signal enable (%d); session (%x)",
                                           (int)pVptx->timing.venc_delivery_tick_vtm_struct.signal_enable,
                                           (int)pVptx->timing.vptx_delivery_tick_vtm_struct.signal_enable,
                                           (int)pVptx->session.session_num);

      nResult = vptx_allocate_circ_bufs(pVptx);
      if ( nResult != ADSP_EOK )
      {
         pVptx->process_data_flag = 0;
         elite_svc_send_ack(pMsg, nResult); // Cannot proceed, return error
         return nResult;
      }
      else 
      {
         vptx_reinit_circbuf(pInstance); // reset and init circular buffers with required pre-buffering
      }

      // reset the hptimer vs device drift counter
      memset(&(pVptx->ratematch.voice_drift_info_near), 0, sizeof(pVptx->ratematch.voice_drift_info_near));
      memset(&(pVptx->ratematch.voice_drift_info_far), 0, sizeof(pVptx->ratematch.voice_drift_info_far));

      //Call algo reset
      vptx_modules_full_init(pVptx);

      if (NULL != pVptx->downstream_peer_ptr)
      {
         pVptx->process_data_flag    = 1; // Start processing
         pVptx->io.output.send_mediatype_flag  = 1; // Send media type before running
      }
      else
      {
         nResult = ADSP_ENOTREADY; // not ready to run since downstream is not yet connected
      }

      if(( pVptx->loopback_enable_flag) && (NULL != pVptx->rx_peer_ptr))
      {
         pVptx->io.loopback_output.send_mediatype_flag = 1; // enable loop back media type command only if conditions agree
      }

      vptx_aggregate_modules_delay(pVptx);
      SET_MASK(pVptx->wait_mask,CMD_DATA_MASK); // Start with listening to input commands and input data only

      if(ADSP_SUCCEEDED(vptx_register_sns_qmi(pVptx)))
      {
          // Start listening to Sensors Registration signal
          OR_MASK(pVptx->wait_mask,VPTX_QMI_REG_MASK);
          OR_MASK(pVptx->wait_mask,VPTX_QMI_IND_MASK);
      }

      // clear the scratch buf. This is must as at sync tick handler we may require to pad zeros. This buffer is required for that
      if (pVptx->io.scratch.data_ptr)
      {
          memset(pVptx->io.scratch.data_ptr,0,pVptx->io.scratch.data_size_bytes);
      }

   } // end of nResult condition to check if custom_run suceeded

   //clear process data flag when memory is not allocated for modules. This ensure that the incoming data is not processed.
   if((NULL == pVptx->memory.pStartAddr) || (NULL == pVptx->memory.pBufStartAddr))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP:Error! Memory is not allocated for vptx session. Not going to run. session(%x)",(int)pVptx->session.session_num);
      pVptx->process_data_flag = 0;
      nResult = ADSP_EFAILED;
   }

   pVptx->resync_count = 0; // resetting resync count

   elite_svc_send_ack(pMsg, nResult);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_run_cmd end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_reconfig_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   // should send an ACK back to caller to avoid race conditions, and to inform success/fail.
   vptx_t* pVptx = (vptx_t*)pInstance;
   ADSPResult nResult = ADSP_EOK;

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_reconfig_cmd begin session(%x)",(int)pVptx->session.session_num);
   elite_msg_voice_re_config_type *pReConfig= (elite_msg_voice_re_config_type*) pMsg->pPayload;

   if (pVptx->process_data_flag)
   {  // Cannot start processing utill it was set to stop earlier
      nResult = ADSP_ENOTREADY;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Cannot Re-Configure while running session(%x)",(int)pVptx->session.session_num);
   }
   else if (ADSP_FAILED(nResult = vptx_is_supported(pReConfig->topology_id, pReConfig->sampling_rate, pVptx->session.session_num, pReConfig->port_id_tx,pReConfig->num_channels,pReConfig->num_ref_channels)))
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed reconfig Vptx topo(%#lx),SR(%ld), afe port(%#x)!! session(%#lx)",pReConfig->topology_id,pReConfig->sampling_rate,pReConfig->port_id_tx,pVptx->session.session_num);
   }
   else
   {
      MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: reconfig Vptx topo(%x),SR(%d) num_mics(%d) num_ref_channels (%d)!! ",(int)pReConfig->topology_id,(int)pReConfig->sampling_rate, pReConfig->num_channels,pReConfig->num_ref_channels);

      //update reconfig state
      pVptx->reconfig_state.reconfig_pending = TRUE;
      pVptx->reconfig_state.retain_amdb_handle = FALSE;

      //Check if its reconfig to same topo and update handle retain flag.
      if(pVptx->session.topology_id == pReConfig->topology_id)
      {
         pVptx->reconfig_state.retain_amdb_handle = TRUE;
      }

      /* not currently checking if only port changes.  Could optimize to skip End/Init in this case since Init currently
       * doesn't use port
       */
      vptx_end(pVptx);
      // set reconfig pending to true
      vptx_create_params_v2_t create_params={0};
      create_params.topology_id = pReConfig->topology_id;
      create_params.near_port_id = pReConfig->port_id_tx;
      create_params.far_port_id = pReConfig->port_id_rx;
      create_params.sampling_rate = pReConfig->sampling_rate;
      create_params.num_mics = pReConfig->num_channels;
      create_params.num_ref_channels = pReConfig->num_ref_channels;
      memscpy(&create_params.channel_map_near[0],sizeof(create_params.channel_map_near),&pReConfig->channel_map_near[0],sizeof(pReConfig->channel_map_near));
      memscpy(&create_params.channel_map_far[0],sizeof(create_params.channel_map_near),&pReConfig->channel_map_far[0],sizeof(pReConfig->channel_map_far));
      nResult = vptx_init(pVptx, &create_params);
      if (ADSP_FAILED(nResult))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed reconfig Vptx !! session(%x)",(int)pVptx->session.session_num);
         vptx_end(pVptx);
      }
   }

   // re-send HPCM start if required
   if ( (TRUE == pVptx->modules.hpcm_enable_flag) && (NULL != pVptx->modules.special_capis[HPCM]->module_ptr) )
   {
      capi_v2_buf_t param_data_buf;
      capi_v2_port_info_t port_info;
      port_info.is_valid = FALSE;
      param_data_buf.data_ptr = (int8_t *) &pVptx->modules.hpcm_tap_point;
      param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(voice_tap_point_v2_t);
      nResult = pVptx->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(pVptx->modules.special_capis[HPCM]->module_ptr,
            FWK_EXTN_PARAM_ID_HPCM_START, &port_info, &param_data_buf);
      nResult = capi_v2_err_to_adsp_result(nResult);
      if (ADSP_FAILED(nResult))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vptx_reconfig_cmd() - HPCM_START set param to CAPI failed!!");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_reconfig_cmd() - HPCM_START set param to CAPI successful!!");
      }
   }

   //set reconfig pending flag to false.
   pVptx->reconfig_state.reconfig_pending = FALSE;
   pVptx->reconfig_state.retain_amdb_handle = FALSE;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Reconfig cmd end: Tid(%ld) another_thread_launched(%ld)",qurt_thread_get_id(), pVptx->reconfig_state.another_thread_launched);

   //clear event status
   pVptx->voice_activity_status.report_voice_activity = FALSE;

   elite_svc_send_ack(pMsg, nResult);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_reconfig_cmd end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return ADSP_EOK;
}

static void vptx_ctrl_params_init(vptx_t* pVptx)
{
   pVptx->process_data_flag                           = 0; // Stop processing

   pVptx->io.near_in.dataflow_on                      = FALSE;
   pVptx->io.far_in.dataflow_on                       = FALSE;
   pVptx->io.output.send_mediatype_flag               = 0; // resetting
   pVptx->io.loopback_output.send_mediatype_flag      = 0;
   pVptx->io.out_gen_cnt                              = 0;

   pVptx->ratematch.ss_buf_samples_near               = 0; // Reset sample slip stuff requirements
   pVptx->ratematch.ss_buf_samples_far                = 0; // Reset sample slip stuff requirements



   pVptx->timing.resync_far_end                    = FALSE;
   pVptx->timing.ref_time_tick_recvd_flag          = FALSE; // reset timer tick
   pVptx->is_venc_tick_fired                       = FALSE; // reset in stop and init
   pVptx->process_start_flag                       = FALSE; // reset in stop and init

   if((VPTX_AUDIO == pVptx->session.audio_instance_flag) || (AFE_PORT_ID_INVALID  == pVptx->afe_info.afe_rx_port_id))
   {
      //True by default in audio case, till AFE media type is received on far
      // Also need to force zeros in both cases if rx is not connected
      pVptx->io.force_far_zeros = TRUE;
   }
   else
   {
      pVptx->io.force_far_zeros = FALSE;
   }
}

static ADSPResult vptx_stop_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   vptx_t* pVptx = (vptx_t*) pInstance;
   ADSPResult nResult = ADSP_EOK;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_stop_cmd end session(%x)",(int)pVptx->session.session_num);

   SET_MASK(pVptx->wait_mask,CMD_DATA_MASK); // listen to input commands and input data only

   if(ADSP_SUCCEEDED(vptx_deregister_sns_qmi(pVptx)))
   {
        // stop listening to Sensors registration and indication signals
        NOT_MASK(pVptx->wait_mask,VPTX_QMI_REG_MASK);
        NOT_MASK(pVptx->wait_mask,VPTX_QMI_IND_MASK);
   }

   vptx_vtm_unsubscribe(pInstance, &(pVptx->timing.sync_tick_vtm_struct), VPTX_SYNC_TICK_END_MASK);
   vptx_vtm_unsubscribe(pInstance, &(pVptx->timing.proc_tick_vtm_struct), VPTX_PROC_TICK_END_MASK);
   vptx_vtm_unsubscribe(pInstance, &(pVptx->timing.vptx_delivery_tick_vtm_struct), VPTX_DELIVERY_TICK_END_MASK);
   vptx_vtm_unsubscribe(pInstance, &(pVptx->timing.venc_delivery_tick_vtm_struct), VPTX_VENC_DELIVERY_TICK_END_MASK);

   // listen input timing tick and data only
   // listening to data to drop it and free buffer

   // Unsubscribe from VDS
   if((VPTX_AUDIO != pVptx->session.audio_instance_flag) && (pVptx->session.real_time_flag))
   {
      nResult = voice_cmn_send_vds_command(&pVptx->timing.vds_client_id, pVptx->timing.vds_client_token, pVptx->response_q, VDS_UNSUBSCRIBE,
                                         &pVptx->timing.vptx_delivery_tick_vtm_struct.signal_ptr, NULL, pVptx->timing.vds_handle_ptr, pVptx->session.session_num, (1 == pVptx->timing.vptx_delivery_tick_vtm_struct.signal_enable),NULL);

      if (ADSP_FAILED(nResult))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed Vptx unsubscribe to VDS session(%x)!",(int)pVptx->session.session_num);
         return nResult;
      }
   }
   /* guarantee that any queued near/far data buffers are returned, so AFE can free them properly during VPM_STOP processing */
   vptx_clear_input_dataq(pVptx);

   vptx_ctrl_params_init(pVptx);

   // Clear Circular Buffers, should not deliver in stop state
   voice_multi_ch_circ_buf_reset(&(pVptx->io.near_in.circ_buf));
   voice_multi_ch_circ_buf_reset(&(pVptx->io.far_in.circ_buf));
   voice_multi_ch_circ_buf_reset(&(pVptx->io.output.circ_buf));
   voice_multi_ch_circ_buf_reset(&(pVptx->io.loopback_output.circ_buf));

   // Clear drift info pointers
   pVptx->afe_info.afe_tx_drift_ptr = NULL;
   pVptx->afe_info.afe_rx_drift_ptr = NULL;

   // clear dsp clock value
   pVptx->timing.session_thread_clock_mhz = 0;

   elite_svc_send_ack(pMsg, nResult);

   // Check guard band for memory corruption
   nResult = nResult | (voice_check_guard_band((int8_t**)pVptx->memory.guard_band_ptrs, (int16_t)pVptx->modules.num_modules, TX, pVptx->session.session_num));

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_stop_cmd end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_set_mute_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t* pVptx = (vptx_t*)pInstance;
   elite_msg_custom_voc_set_soft_mute_type *pSetMuteCmd = (elite_msg_custom_voc_set_soft_mute_type *) pMsg->pPayload;

   voice_soft_mute_cal_param_t payload_soft_mute;

   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx_SetMute muteCmd(%d) ramp_duration(%ld) session(%lx)",
         pSetMuteCmd->mute, pSetMuteCmd->ramp_duration, pVptx->session.session_num);

   if( pSetMuteCmd->mute > VOICE_MUTE)
   {
      nResult = ADSP_EBADPARAM;
   }
   else
   {
      if ( pVptx->mute != pSetMuteCmd->mute )
      {
         payload_soft_mute.mute = pVptx->mute = pSetMuteCmd->mute;
         payload_soft_mute.ramp_duration = pSetMuteCmd->ramp_duration;

         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Sending soft mute command to module(%lx)",VOICE_MODULE_SOFT_MUTE);

         nResult = vptx_modules_set_param_int(pVptx, VOICE_MODULE_SOFT_MUTE, 0,VOICE_PARAM_SOFT_MUTE, &payload_soft_mute, sizeof(voice_soft_mute_cal_param_t));
         if (ADSP_FAILED(nResult))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error!! soft mute command processing. result(%x)",nResult);
         }
      }
   }

   elite_svc_send_ack(pMsg, nResult);
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx_SetMute end nResult(%d) session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_get_kpps_cmd (void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t* pVptx = (vptx_t*)pInstance;
   elite_msg_custom_kpps_type* kpps_msg_ptr = (elite_msg_custom_kpps_type*) pMsg->pPayload;
   vpm_get_kpps_ack_t* kpps_ptr = (vpm_get_kpps_ack_t*)kpps_msg_ptr->pnKpps;
   uint32_t kpps_changed=FALSE;

   if( kpps_ptr )
   {
      kpps_ptr->vptx_frame_size_us = pVptx->io.num_1ms_frame_block_process * 1000; // vptx framesize in us
      kpps_ptr->vptx_kpps = 0;  // Initialization
      nResult = vptx_aggregate_modules_kpps(pVptx, &kpps_changed);
      if(ADSP_SUCCEEDED(nResult))
      {
         kpps_ptr->vptx_kpps = pVptx->timing.modules_kpps;
      }
   }

   elite_svc_send_ack(pMsg, nResult );
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_get_kpps_cmd end nResult(%d) session(%lx)",nResult,pVptx->session.session_num);
   return nResult;
}

static ADSPResult vptx_get_delay_cmd (void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t* pVptx = (vptx_t*)pInstance;
   elite_msg_custom_delay_type* delay_msg_ptr = (elite_msg_custom_delay_type*) pMsg->pPayload;
   vpm_get_delay_ack_t* delay_ptr = (vpm_get_delay_ack_t*)delay_msg_ptr->delay_ptr;

   if( delay_ptr )
   {
      delay_ptr->vptx_delay = 0;  // Initialization
      nResult = vptx_aggregate_modules_delay(pVptx);
      if(ADSP_SUCCEEDED(nResult))
      {
         delay_ptr->vptx_delay = pVptx->timing.modules_delay + 20000; // TODO: do we need get delay api for Tx now ?.
      }
   }
   else
   {
      nResult = ADSP_EBADPARAM;
   }

   elite_svc_send_ack(pMsg, nResult );
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_get_delay_cmd end nResult(%d) session(%lx)",nResult,pVptx->session.session_num);
   return nResult;
}

//PGtodo: add error checks on params like timingv3_cmd
static ADSPResult vptx_set_timingv2_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t* pVptx = (vptx_t*)pInstance;
   elite_msg_custom_voc_timing_param_type *pSetTimingCmd = (elite_msg_custom_voc_timing_param_type *) pMsg->pPayload;
   vpm_set_timing_params_t* pVfrCmd = (vpm_set_timing_params_t*)pSetTimingCmd->param_data_ptr;

   // Check for Run state
   if (FALSE == pVptx->process_data_flag)
   {
      if (VPTX_AUDIO == pVptx->session.audio_instance_flag)
      {
         // For Audio use case, hard code to None VFR mode.
         pVptx->session.vfr_mode = VFR_NONE;
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VPM_CMD_SET_TIMING_PARAMS,Vptx vfr_mode(%d), cannot be anything other than VFR_NONE(%d), session(%#lx)",pVfrCmd->mode, VFR_NONE, pVptx->session.session_num);
         nResult = ADSP_EBADPARAM;
      }
      else
      {
         pVptx->session.vfr_mode = pVfrCmd->mode;
         pVptx->session.vfr_cycle = 1;
         pVptx->timing.sync_offset = pVfrCmd->vptx_start_offset;
         pVptx->timing.proc_offset = (20 == pVptx->io.num_1ms_frame_block_process) ? pVptx->timing.sync_offset : pVptx->timing.sync_offset + 10000;
         if ( pVptx->timing.proc_offset > 20000 )
         {
            pVptx->timing.proc_offset -= 20000;
         }
         pVptx->timing.delivery_offset = pVfrCmd->vptx_delivery_offset;
         pVptx->timing.enc_offset = pVfrCmd->enc_offset;

         // Select the VFR source
         pVptx->session.vfr_source = voice_cmn_get_vfr_source(pVptx->session.vfr_mode);

         MSG_6(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VPM_CMD_SET_TIMING_PARAMS,Vptx vfr_mode(%x), sync offset(0x%x), delivery offset (0x%x), venc delivery offset (0x%x), process_data(%x), session(%#lx)",
               pVfrCmd->mode, pVfrCmd->vptx_start_offset, pVfrCmd->vptx_delivery_offset, pVfrCmd->enc_offset, pVptx->process_data_flag, pVptx->session.session_num);
         pVptx->timing.use_new_timing_flag = TRUE;
         pVptx->timing.timing_ver = VFR_CLIENT_INFO_VER_1;  // version 1 (no VSID) of timing
         int32_t process_samples_in_us = pVptx->timing.proc_offset - pVfrCmd->vptx_start_offset;
         if ( process_samples_in_us <= 0 )
         {
            process_samples_in_us += MAX_TIMER_OFFSET;
         }
         pVptx->proc_start_samples = (process_samples_in_us/1000)*(pVptx->session.nb_sampling_rate_factor<<3);
         int16_t venc_processing_time = 0;
         int16_t vptx_processing_time = 0;

         venc_processing_time = pVptx->timing.enc_offset - pVptx->timing.delivery_offset;
         if(venc_processing_time <= 0)
         {
            venc_processing_time += MAX_TIMER_OFFSET;
         }

         vptx_processing_time = pVptx->timing.delivery_offset - pVptx->timing.sync_offset;
         if(vptx_processing_time <= 0)
         {
            vptx_processing_time += MAX_TIMER_OFFSET;
         }

         pVptx->max_tx_processing_time = (uint16_t)(venc_processing_time + vptx_processing_time);
         MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VPM_CMD_SET_TIMING_PARAMS vptx_processing_time = (%d), venc_processing_time = (%d) and max_tx_processing_time = (%d) session(%lx)",
                                           (int)vptx_processing_time,
                                           (int)venc_processing_time,
                                           (int)pVptx->max_tx_processing_time,
                                           pVptx->session.session_num);


      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VFR mode can't be changed in RUN session(%lx)",pVptx->session.session_num);
      nResult = ADSP_EBUSY;
   }

   elite_svc_send_ack(pMsg, nResult);
   return nResult;
}

static ADSPResult vptx_set_timing_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t* pVptx = (vptx_t*)pInstance;
   elite_msg_custom_voc_timing_param_type *pSetTimingCmd = (elite_msg_custom_voc_timing_param_type *) pMsg->pPayload;
   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_set_timing_cmd begin session(%x)",pVptx->session.session_num);

   {
      if ((FALSE == pVptx->process_data_flag) && (VPTX_AUDIO != pVptx->session.audio_instance_flag))
      {
         voice_set_timing_params_t* pVfrCmd = (voice_set_timing_params_t*)pSetTimingCmd->param_data_ptr;
         if ((VFR_NONE != pVfrCmd->mode) && (VPTX_AUDIO == pVptx->session.audio_instance_flag))
         {
            pVptx->session.vfr_mode = VFR_NONE;
            nResult = ADSP_EBADPARAM;
            MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VOICE_SET_TIMING_PARAMS,Vptx vfr_mode(%d), cannot be anything other than VFR_NONE(%d), session(%#lx)",pVfrCmd->mode, VFR_NONE, pVptx->session.session_num);
         }
         else
         {
            pVptx->session.vfr_mode = pVfrCmd->mode;
            pVptx->session.vfr_source = voice_cmn_get_vfr_source(pVptx->session.vfr_mode);
            pVptx->timing.enc_offset = pVfrCmd->enc_offset;
            pVptx->timing.use_new_timing_flag = FALSE;
            pVptx->timing.timing_ver = VFR_CLIENT_INFO_VER_1;  // version 1 (no VSID) of timing
         }

         MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VOICE_SET_TIMING_PARAMS,Vptx vfr_mode(%x), enc offset(0x%x), process_data(%x), session(%#lx)",
               pVfrCmd->mode, pVfrCmd->enc_offset, pVptx->process_data_flag, pVptx->session.session_num);
         elite_svc_send_ack(pMsg, nResult );
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VFR mode can't be changed in RUN session(%lx)",pVptx->session.session_num);
         elite_svc_send_ack(pMsg, ADSP_EBUSY);
      }
   }

   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_set_timing_cmd end nResult(%d) session(%x)",nResult,pVptx->session.session_num);
   return nResult;
}


static ADSPResult vptx_custom_msg( void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EFAILED;
   vptx_t* pVptx = (vptx_t*) pInstance;
   elite_msg_custom_header_t *pCustom = (elite_msg_custom_header_t *) pMsg->pPayload;

   if( pCustom->unSecOpCode < VOICEPROC_NUM_MSGS)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx_Cmd, SecOpcode: %d session(%x)",(int)pCustom->unSecOpCode ,(int)pVptx->session.session_num);
      if(TRUE == pVptx->fatal_state)
      {
         // accept only destroy cmd in fatal state.
         if (7 == pCustom->unSecOpCode)
         {
            nResult = pHandler[pCustom->unSecOpCode](pInstance, pMsg);
         }
         else
         {
            nResult = ADSP_EFAILED;
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vptx in fatal state!! result(%x) session(%x)",nResult,(int)pVptx->session.session_num);
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

void vptx_cal_sample_slip_stuff(int16_t ss_num_samp_adj, int32_t prebuffer_samples, int32_t* ss_buf_samples_ptr, uint32_t session_num,near_far_t near_far_path)
{

   if (ss_num_samp_adj < 0) // That means DMA faster than vfr, slip samples
   {
      // Read one from the buffer and decrement ss_buf_samples
      *ss_buf_samples_ptr   = *ss_buf_samples_ptr + ss_num_samp_adj; // arithmetic to decrease buffer samples in prebuffer
      if (0 >= *ss_buf_samples_ptr)
      {
         // Reset Counter
         *ss_buf_samples_ptr = prebuffer_samples;
      }
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx detected Sample Slip count (%d) ss_buf_samples(%#lx),near_far_path(%d),session(%#lx)",ss_num_samp_adj,*ss_buf_samples_ptr,(int)near_far_path,session_num);
   }
   else if (ss_num_samp_adj > 0) // That means DMA is slower than VFR, so stuff samples
   {
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx detected Sample Stuff count (%d) ss_buf_samples(%#lx),near_far_path(%d),session(%#lx)",ss_num_samp_adj,*ss_buf_samples_ptr,(int)near_far_path,session_num);
   }
}

static ADSPResult vptx_apr_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = APR_EOK;
   ADSPResult apr_pkt_result = APR_EOK;
   vptx_t* pVptx = (vptx_t*) pInstance;
   elite_apr_packet_t * pAprPacket = (elite_apr_packet_t*) pMsg->pPayload;
   int32_t rc;

   //dbg: MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_apr_cmd begin, opCode(%x) token(%x) session(%x)",
   //dbg:      (int)elite_apr_if_get_opcode(pAprPacket),(int)elite_apr_if_get_client_token(pAprPacket),(int)pVptx->session.session_num);

   /* parse out the received pakcet.  Note in this current framework we are not prioritizing commands
      that can be completed immediately.  We are simply processing command in order they are received */
   switch(elite_apr_if_get_opcode(pAprPacket))
   {
      case VOICE_EVT_PUSH_HOST_BUF_V2:
         {
            ADSPResult local_result = APR_EOK;
            voice_evt_push_host_pcm_buf_v2_t *push_host_buf_ptr = (voice_evt_push_host_pcm_buf_v2_t *) elite_apr_if_get_payload_ptr(pAprPacket);

            // send PUSH_HOST_BUF event to the HPCM CAPI module
            if (NULL != pVptx->modules.special_capis[HPCM]->module_ptr)
            {
               capi_v2_buf_t param_data_buf;
               capi_v2_port_info_t port_info;
               port_info.is_valid = FALSE;
               param_data_buf.data_ptr = (int8_t *) push_host_buf_ptr;
               param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(fwk_extn_param_id_hpcm_push_host_buf_t);
               local_result = pVptx->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(pVptx->modules.special_capis[HPCM]->module_ptr,
                     FWK_EXTN_PARAM_ID_HPCM_PUSH_HOST_BUF, &port_info, &param_data_buf);
               local_result = capi_v2_err_to_adsp_result(local_result);
               if (ADSP_FAILED(local_result))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! FWK_EXTN_PARAM_ID_HPCM_PUSH_HOST_BUF failed, result(%#x) session(%#lx)", \
                     local_result,pVptx->session.session_num);
               }
            }
            else
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vptx_apr_cmd() - HPCM CAPI module ptr is NULL, session(%lx)",pVptx->session.session_num);
            }

            // free the event since not done internally
            rc = elite_apr_if_free(pVptx->modules.apr_info_hpcm.apr_handle, pAprPacket);
            break;
         }
      case VPM_CMD_SET_TX_DTMF_DETECTION:
         {
            uint16_t client_addr, client_port;
            vpm_set_tx_dtmf_detection_t *pDtmfDetect = ( vpm_set_tx_dtmf_detection_t *) elite_apr_if_get_payload_ptr(pAprPacket);

            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VPM_CMD_SET_TX_DTMF_DETECTION, enable(%ld), session(%lx)", pDtmfDetect->enable, pVptx->session.session_num);

            {
               uint32_t enable = pDtmfDetect->enable;
               if( VPM_TX_DTMF_DETECTION_ENABLE == pDtmfDetect->enable)
               {
                  if( 0 != pVptx->modules.apr_info_dtmf.client_addr  ||
                        0 != pVptx->modules.apr_info_dtmf.client_port)
                  {
                     /* already have a client, should not happen */
                     rc = elite_apr_if_end_cmd( pVptx->modules.apr_info_dtmf.apr_handle, pAprPacket, APR_ENOTREADY );
                     break;
                  }
                  else
                  {
                     /* save self/client info for DTMF status updates.  Client is source address from incoming packet */
                     pVptx->modules.apr_info_dtmf.self_addr = elite_apr_if_get_dst_addr( pAprPacket);
                     pVptx->modules.apr_info_dtmf.self_port = elite_apr_if_get_dst_port( pAprPacket);
                     pVptx->modules.apr_info_dtmf.client_addr = elite_apr_if_get_src_addr( pAprPacket);
                     pVptx->modules.apr_info_dtmf.client_port = elite_apr_if_get_src_port( pAprPacket);

                     MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VPM_CMD_SET_TX_DTMF_DETECTION, saving clientAddr(%x) clientPort(%x) :session(%lx)",
                           pVptx->modules.apr_info_dtmf.client_addr, pVptx->modules.apr_info_dtmf.client_port, pVptx->session.session_num);

                  }
               }
            else /* DTMF detection disable */
            {
               client_addr = elite_apr_if_get_src_addr(pAprPacket);
               client_port = elite_apr_if_get_src_port(pAprPacket);

               /* If the request is to disable, check whether the client
                  is same as the one that issued enable. */
               if (pVptx->modules.apr_info_dtmf.client_addr != client_addr ||
                   pVptx->modules.apr_info_dtmf.client_port != client_port)
               {
                  nResult = ADSP_EBADPARAM;

                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                        "VCP: DTMF detection disable failed, "
                        "Client APR addr (0x%08X) is not same as enabling client APR addr (0x%08X)",
                        client_addr, pVptx->modules.apr_info_dtmf.client_addr);

                  rc = elite_apr_if_end_cmd(pVptx->modules.apr_info_dtmf.apr_handle, pAprPacket, nResult);

                  break;
               }

                  /* disabling DTMF, reset aprInfo */
                  pVptx->modules.apr_info_dtmf.self_addr = 0;
                  pVptx->modules.apr_info_dtmf.self_port = 0;
                  pVptx->modules.apr_info_dtmf.client_addr = 0;
                  pVptx->modules.apr_info_dtmf.client_port = 0;

                  /* End.  Resets circ buffers (erases any pending tone status) TBD: proper place? */
               }

               // check if DTMF module is stubbed
               voice_capi_module_t* curr_module_ptr = pVptx->modules.modules_array;
               bool_t dtmf_found=FALSE;

               for(uint32_t i = 0; i < (int32_t)pVptx->modules.num_modules; i++)
               {
                  if (VOICE_MODULE_TX_DTMF_DETECTION == curr_module_ptr->module_id)
                  {
                     dtmf_found = TRUE;
                     break;
                  }
                  curr_module_ptr++;
               }

               if (!dtmf_found)
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: DTMF detection Module not present in topology !! session(%lx)",pVptx->session.session_num);
                  rc = ADSP_EOK;
               }
               else if (curr_module_ptr->is_virtual_stub)
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: DTMF detection Module is virtually stubbed session(%lx)",pVptx->session.session_num);
                  rc = ADSP_EOK;
               }
               else
               {
                  capi_v2_buf_t param_data_buf;
                  capi_v2_port_info_t port_info;
                  port_info.is_valid = FALSE;
                  param_data_buf.data_ptr = (int8_t *) &enable;
                  param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(enable);
                  if (NULL != pVptx->modules.special_capis[TX_DTMFDET_CAPI])
                  {
                     rc = pVptx->modules.special_capis[TX_DTMFDET_CAPI]->module_ptr->vtbl_ptr->set_param(pVptx->modules.special_capis[TX_DTMFDET_CAPI]->module_ptr,
                           VOICE_PARAM_MOD_ENABLE, &port_info, &param_data_buf);
                     rc = capi_v2_err_to_adsp_result(rc);
                  }
                  else
                  {
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: NULL pointer for DTMF Det CAPI !!");
                     rc = ADSP_EFAILED;
                  }
               }
               if (ADSP_FAILED(rc))
               {
                   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Set Param failed for DTMF Det Enable/Disable !!");
                   rc = elite_apr_if_end_cmd( pVptx->modules.apr_info_dtmf.apr_handle, pAprPacket, APR_EFAILED );
               }
               else
               {
                   rc = elite_apr_if_end_cmd( pVptx->modules.apr_info_dtmf.apr_handle, pAprPacket, APR_EOK );
               }
            }
            break;
         }
      default:
         {
            /* Handle error. */
            if (elite_apr_if_msg_type_is_cmd(pAprPacket))
            {  /* Complete unsupported commands. */
               apr_pkt_result = APR_EUNSUPPORTED;
               rc = elite_apr_if_end_cmd(pVptx->static_svc_apr_handle, pAprPacket, apr_pkt_result);
            }
            else
            {  /* Drop unsupported events. */
               rc = elite_apr_if_free(pVptx->static_svc_apr_handle, pAprPacket);
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx dropped apr evt, apr pkt ptr(%p) session(%#lx)", \
                     pAprPacket, pVptx->session.session_num);
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
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_apr_cmd end, apr result=(%#x), fn result(%#x), session(%#x)", \
            apr_pkt_result, nResult,(int)pVptx->session.session_num);
   }
   return nResult;
}

static ADSPResult vptx_config_host_pcm(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   ADSPResult apr_pkt_result = APR_EOK;
   vptx_t* pVptx = (vptx_t*) pInstance;
   elite_msg_custom_voc_config_host_pcm_type *pConfig = (elite_msg_custom_voc_config_host_pcm_type *) pMsg->pPayload;
   elite_apr_packet_t *apr_packet_ptr = pConfig->apr_packet_ptr;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_config_host_pcm begin session(%x)\n",(int)pVptx->session.session_num);

   // check if HPCM module is present in topology
   if (NULL == pVptx->modules.special_capis[HPCM])
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! HPCM Module is not present in the topology, session(%lx)",pVptx->session.session_num);
      nResult = ADSP_EFAILED;
      apr_pkt_result=APR_EUNSUPPORTED;
      elite_svc_send_ack(pMsg, apr_pkt_result);
      return nResult;
   }

   if( (VOICE_CMD_START_HOST_PCM_V2 == elite_apr_if_get_opcode( apr_packet_ptr)) &&
         ((VOICE_NB_SAMPLING_RATE == pVptx->session.sampling_rate) || (VOICE_WB_SAMPLING_RATE == pVptx->session.sampling_rate) ||
                    (VOICE_SWB_SAMPLING_RATE == pVptx->session.sampling_rate) || (VOICE_FB_SAMPLING_RATE == pVptx->session.sampling_rate))
     )
   {
      voice_start_host_pcm_v2_t    *start_host_pcm_ptr = (voice_start_host_pcm_v2_t *) elite_apr_if_get_payload_ptr( apr_packet_ptr);

      uint8 index;

      // check if default Rx tap point in the list
      for( index = 0; index < start_host_pcm_ptr->num_tap_points; index++)
      {
         if( VOICEPROC_MODULE_TX == start_host_pcm_ptr->tap_points[index].tap_point )
         {
            break;
         }
      }

      // check if valid index found.  TODO: check if tap point already enabled
      if( index < start_host_pcm_ptr->num_tap_points)
      {

         voice_tap_point_v2_t *voice_tap_point_ptr = &start_host_pcm_ptr->tap_points[index];
         if(NULL == voice_tap_point_ptr->mem_map_handle)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Vptx received NULL mem map handle!");
            apr_pkt_result = APR_EBADPARAM;
         }
         else
         {

            /* copy Host Pcm APR info */
            pVptx->modules.apr_info_hpcm.apr_handle        = pConfig->apr_handle;
            pVptx->modules.apr_info_hpcm.self_addr         = elite_apr_if_get_dst_addr( apr_packet_ptr);
            pVptx->modules.apr_info_hpcm.self_port         = elite_apr_if_get_dst_port( apr_packet_ptr);
            pVptx->modules.apr_info_hpcm.client_addr       = elite_apr_if_get_src_addr( apr_packet_ptr);
            pVptx->modules.apr_info_hpcm.client_port       = elite_apr_if_get_src_port( apr_packet_ptr);
            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx setting hpcm context, memmmap handle (%x), client addr (%x), client port (%x)", (unsigned int)voice_tap_point_ptr->mem_map_handle, pVptx->modules.apr_info_hpcm.client_addr, pVptx->modules.apr_info_hpcm.client_port);

            // send HPCM start to HPCM CAPI module
            if (NULL != pVptx->modules.special_capis[HPCM]->module_ptr)
            {
               capi_v2_buf_t param_data_buf;
               capi_v2_port_info_t port_info;
               port_info.is_valid = FALSE;
               param_data_buf.data_ptr = (int8_t *) voice_tap_point_ptr;
               param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(voice_tap_point_v2_t);
               nResult = pVptx->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(pVptx->modules.special_capis[HPCM]->module_ptr,
                     FWK_EXTN_PARAM_ID_HPCM_START, &port_info, &param_data_buf);
               nResult = capi_v2_err_to_adsp_result(nResult);
               if (ADSP_FAILED(nResult))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! HPCM_START set param to CAPI failed!!");
               }
               else
               {
                  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: HPCM_START set param to CAPI successful!!");
                  // cache HPCM tap-point struct to re-apply when necessary such as Vptx reconfig
                  memscpy(&pVptx->modules.hpcm_tap_point, sizeof(voice_tap_point_v2_t), voice_tap_point_ptr, sizeof(voice_tap_point_v2_t));
                  pVptx->modules.hpcm_enable_flag = TRUE;
               }
            }
            else
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vptx_config_host_pcm() - HPCM CAPI module ptr is NULL, session(%lx)",pVptx->session.session_num);
            }
         }
      }
   }
   else if( VOICE_CMD_STOP_HOST_PCM == elite_apr_if_get_opcode( apr_packet_ptr))
   {
      // send HPCM stop to HPCM CAPI module
      if (NULL != pVptx->modules.special_capis[HPCM]->module_ptr)
      {
         capi_v2_buf_t param_data_buf;
         capi_v2_port_info_t port_info;
         port_info.is_valid = FALSE;
         param_data_buf.data_ptr = NULL; // no payload is needed
         param_data_buf.actual_data_len = param_data_buf.max_data_len = 0;
         nResult = pVptx->modules.special_capis[HPCM]->module_ptr->vtbl_ptr->set_param(pVptx->modules.special_capis[HPCM]->module_ptr,
               FWK_EXTN_PARAM_ID_HPCM_STOP, &port_info, &param_data_buf);
         nResult = capi_v2_err_to_adsp_result(nResult);
         if(ADSP_FAILED(nResult))
         {
             MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! HPCM_STOP set param to CAPI failed!!");
         }
         pVptx->modules.hpcm_enable_flag = FALSE;
         memset(&pVptx->modules.hpcm_tap_point,0,sizeof(voice_tap_point_v2_t));
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR! vptx_config_host_pcm() - HPCM CAPI module ptr is NULL, session(%lx)",pVptx->session.session_num);
      }
   }
   else
   {
      apr_pkt_result = APR_EUNSUPPORTED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_config_host_pcm end apr pkt result(%d) session(%x)\n",apr_pkt_result, (int)pVptx->session.session_num);

   elite_svc_send_ack(pMsg, apr_pkt_result);
   return nResult;
}

ADSPResult vptx_write_output(vptx_t* pVptx, int8_t* pProcessedData, int32_t nSamples)
{

   int8_t *out_buf_ptr[MAX_CIRCBUF_CHANNELS] = {NULL}; // for multi channel circ buf

   out_buf_ptr[0] = pProcessedData ;
   out_buf_ptr[1] = pProcessedData ; // will be used only in stereo mode


   // write output to out circular buffer
   voice_multi_ch_circ_buf_write(&(pVptx->io.output.circ_buf), &(out_buf_ptr), nSamples);
   if( pVptx->loopback_enable_flag)
   {
      //dbg: MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx writing loopback data into circ buffer, session(%x) samples(%x)",(int)pVptx->session.session_num,pVptx->io.loopback_output.circ_buf.circ_buf[0].unread_samples);
      voice_multi_ch_circ_buf_write(&(pVptx->io.loopback_output.circ_buf), &(out_buf_ptr), nSamples);
   }

   // Update pVptx->io.out_gen_cnt for maintaining the output phase
   pVptx->io.out_gen_cnt += pVptx->io.near_in.frame_samples;
   if ((OUT_BUF_CNT * FRAME_SAMPLES * pVptx->session.nb_sampling_rate_factor) <= pVptx->io.out_gen_cnt)
   {
      pVptx->io.out_gen_cnt = 0;
   }

#if defined(LOG_VOICEPROC_DATA) && defined(SIM) // For Sim debugging
   if( pVptx->session.sampling_rate == VOICE_NB_SAMPLING_RATE)
   {
      FILE *fp;
      int16_t *pIn = (int16_t*)pProcessedData;
      fp = fopen("vptx_nb.raw","a"); // in append mode
      fwrite(pIn,sizeof(int16_t),nSamples,fp);
      fclose(fp);
   }
   else if( pVptx->session.sampling_rate == VOICE_WB_SAMPLING_RATE)
   {
      FILE *fp;
      int16_t *pIn = (int16_t*)pProcessedData;
      fp = fopen("vptx_wb.raw","a"); // in append mode
      fwrite(pIn,sizeof(int16_t),nSamples, fp);
      fclose(fp);
   }
   else if( pVptx->session.sampling_rate == VOICE_SWB_SAMPLING_RATE)
   {
      FILE *fp;
      int16_t *pIn = (int16_t*)pProcessedData;
      fp = fopen("vptx_swb.raw","a"); // in append mode
      fwrite(pIn,sizeof(int16_t),nSamples, fp);
      fclose(fp);
   }
   else if( pVptx->session.sampling_rate == VOICE_FB_SAMPLING_RATE)
   {
      FILE *fp;
      int16_t *pIn = (int16_t*)pProcessedData;
      fp = fopen("vptx_fb.raw","a"); // in append mode
      fwrite(pIn,sizeof(int16_t),nSamples, fp);
      fclose(fp);
   }
#endif
   return ADSP_EOK;
}

static ADSPResult vptx_is_supported(uint32_t topology_id, uint32_t sampling_rate, uint32_t session_num, uint16_t afe_tx_port_id, uint32_t num_channels, uint32 num_ref_channels)
{
   ADSPResult result = ADSP_EOK;
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
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_create Invalid Samping Rate(%lx), session(%lx)",sampling_rate,session_num);
            return ADSP_EUNSUPPORTED;
            break;
         }
   }

   if(vptx_is_custom_topology(topology_id))
   {
      //custom topology use case, make sure a valid channel count was received
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_create custom Topology (%lx), mic count %lu, reference_channel_count %lu session(%lx)",topology_id, num_channels,num_ref_channels, session_num);
      if(!((num_channels <= HEX ) && (num_ref_channels <= HEX)))
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_create custom Topology (%lx), invalid mic count %lu or invalid ref_channel_count %lu session(%lx)",topology_id, num_channels, num_ref_channels, session_num);
         return ADSP_EUNSUPPORTED;
      }
   }
   else
   {
       if (vptx_validate_sample_rate_for_static_topology(topology_id, sampling_rate))
       {
          MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_create Invalid "
                  "Topology sampling rate combo topology(%lx) sampling rate(%lx), session(%lx)",
                  topology_id,sampling_rate,session_num);
          return ADSP_EUNSUPPORTED;
       }
   }

   if (ADSP_FAILED(result = afe_port_id_validation(afe_tx_port_id)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_create Invalid afe tx port id(%x), session(%lx)",afe_tx_port_id,session_num);
      return ADSP_EUNSUPPORTED;
   }


   return ADSP_EOK;
}

static ADSPResult vptx_set_media_type(vptx_t* pVptx)
{
   // TODO: move code in this function to a table for badger time frame

   // Following init should be based on the Topology id from the VPM
   ADSPResult nResult = ADSP_EOK;
   uint16_t num_10ms_frame_block_output   = OUT_BUF_CNT;

   // Set default Near Media Type
   pVptx->io.near_in.mediatype.num_channels    = 1;
   pVptx->io.near_in.mediatype.bits_per_sample = 16;                    // 16 bit word
   pVptx->io.near_in.mediatype.sample_rate     = pVptx->session.sampling_rate;  // 8k by default
   pVptx->io.near_in.mediatype.is_signed       = 1;                     // signed input
   pVptx->io.near_in.mediatype.is_interleaved  = 0;                     // non-interleaved

   // Set Far Media Type
   pVptx->io.far_in.mediatype.num_channels     = pVptx->session.num_ref_channels;    //always single channel for legacy topologies, actual input from audio custom topologies
   pVptx->io.far_in.mediatype.bits_per_sample  = 16;                    // 16 bit word
   pVptx->io.far_in.mediatype.sample_rate      = pVptx->session.sampling_rate;  // 8k by default
   pVptx->io.far_in.mediatype.is_signed        = 1;                     // signed input
   pVptx->io.far_in.mediatype.is_interleaved   = 0;                     // non-interleaved

   memscpy(&pVptx->io.far_in.mediatype.channel_mapping[0],sizeof(pVptx->io.far_in.mediatype.channel_mapping),&pVptx->session.channel_map_far[0],sizeof(pVptx->session.channel_map_far));
   memscpy(&pVptx->io.near_in.mediatype.channel_mapping[0],sizeof(pVptx->io.near_in.mediatype.channel_mapping),&pVptx->session.channel_map_near[0],sizeof(pVptx->session.channel_map_near));

   // Set Ouput Media Type
   pVptx->io.output.mediatype.sample_rate      = pVptx->session.sampling_rate;
   pVptx->io.output.mediatype.num_channels     = MONO;
   pVptx->io.output.mediatype.bits_per_sample  = 16;                    // 16 bit word
   pVptx->io.output.mediatype.is_signed        = 1;                     // signed input
   pVptx->io.output.mediatype.is_interleaved   = 0;                     // non-interleaved
   if(VPTX_AUDIO == pVptx->session.audio_instance_flag)
   {
      pVptx->io.output.mediatype.sample_rate      = VOICE_FB_SAMPLING_RATE;        // 48k by default
      pVptx->io.output.mediatype.num_channels     = STEREO;
   }

   // Set loopback Media Type
   pVptx->io.loopback_output.mediatype.sample_rate      = pVptx->io.output.mediatype.sample_rate;
   pVptx->io.loopback_output.mediatype.num_channels     = MONO;             // always Mono
   pVptx->io.loopback_output.mediatype.bits_per_sample  = 16;               // 16 bit word
   pVptx->io.loopback_output.mediatype.is_signed        = 1;                // signed input
   pVptx->io.loopback_output.mediatype.is_interleaved   = 0;                // non-interleaved

//update common logging format for reference port for all legacy topology
#if defined(__qdsp6__) && !defined(SIM)
            pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_MONO;
#endif
   // Calculate Input & output Frame size and number of input and output channels
   if(vptx_is_custom_topology(pVptx->session.topology_id))
   {
       // custom topology use case, use supplied channel count and 20 ms frame size
       pVptx->io.near_in.mediatype.num_channels = pVptx->session.num_mics;
       //  pVptx->io.far_in.mediatype.num_channels = pVptx->session.num_ref_channels; // already updated above
       pVptx->io.num_1ms_frame_block_process = 20;
#if defined(__qdsp6__) && !defined(SIM)
            pVptx->log_data_format_near = VOICE_LOG_DATA_FORMAT_PCM_MONO;
#endif
            if(pVptx->session.num_mics > 1)
            {
#if defined(__qdsp6__) && !defined(SIM)
               pVptx->log_data_format_near = VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV;
#endif
            }
            if(pVptx->session.num_mics > 2)
            {
#if defined(__qdsp6__) && !defined(SIM)
               pVptx->log_data_format_near= VOICE_LOG_DATA_FORMAT_PCM_3CHAN_NON_INTLV;
#endif
            }
            if(pVptx->session.num_mics > 3)
            {
#if defined(__qdsp6__) && !defined(SIM)
               pVptx->log_data_format_near= VOICE_LOG_DATA_FORMAT_PCM_4CHAN_NON_INTLV;
#endif
            }

//update the logging format for far
#if defined(__qdsp6__) && !defined(SIM)
            switch (pVptx->session.num_ref_channels)
            {
                case 1 :
                    pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_MONO;
                    break;
                case 2:
                    pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV;
                    break;
                case 3:
                    pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_3CHAN_NON_INTLV;
                    break;
                case 4:
                    pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_4CHAN_NON_INTLV;
                    break;
                case 5:
                    pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_5CHAN_NON_INTLV;
                    break;
                case 6:
                    pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_6CHAN_NON_INTLV;
                    break;
                default:
                    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: invalid number of reference channels (%ld) session(%x)",pVptx->session.num_ref_channels,(int)pVptx->session.session_num);
                    break;
            }

#endif
   }
   else
   {
       vptx_set_media_type_for_static_topology(pVptx);
   }

   pVptx->io.near_in.frame_samples  = FRAME_SAMPLES_NB_PER_MS * pVptx->session.nb_sampling_rate_factor * pVptx->io.num_1ms_frame_block_process;
   pVptx->io.far_in.frame_samples   = FRAME_SAMPLES_NB_PER_MS * pVptx->session.nb_sampling_rate_factor * pVptx->io.num_1ms_frame_block_process;
   pVptx->io.output.buf_samples     = FRAME_SAMPLES * pVptx->session.nb_sampling_rate_factor_output * num_10ms_frame_block_output;   // 20ms output samples always
   pVptx->io.loopback_output.buf_samples     = FRAME_SAMPLES * pVptx->session.nb_sampling_rate_factor_output * num_10ms_frame_block_output;   // 20ms output samples always

   return nResult;
}

static ADSPResult vptx_out_buf_allocator(vptx_t* pVptx, uint32_t out_buf_samples)
{
   ADSPResult nResult = ADSP_EOK;
   int32_t nOutBufSize = (out_buf_samples * pVptx->io.output.mediatype.num_channels) << 1; // samples to bytes conversion
   int32_t nOutBufLoopbackSize = (out_buf_samples * pVptx->io.loopback_output.mediatype.num_channels) << 1; // samples to bytes conversion

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_out_buf_allocator begin session(%x)",(int)pVptx->session.session_num);
   for (uint32_t i = 0; i < MAX_BUF_Q_ELEMENTS; i++)
   {
      //allocate the databuffer payload (metadata + pcm buffer size)
      int32_t nReqSize = GET_ELITEMSG_DATABUF_REQ_SIZE(nOutBufSize);
      int32_t nReqLoopbackSize = GET_ELITEMSG_DATABUF_REQ_SIZE(nOutBufLoopbackSize);
      elite_msg_data_buffer_t* pDataPayload = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc(nReqSize, QURT_ELITE_HEAP_DEFAULT);
      elite_msg_data_buffer_t* pDataPayloadLb = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc(nReqLoopbackSize, QURT_ELITE_HEAP_DEFAULT);

      if (!pDataPayload || !pDataPayloadLb)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to alloc memory for output buffer and/or loopback buffer!! session(%x)",(int)pVptx->session.session_num);
         qurt_elite_memory_free(pDataPayload);   // note that NULL ptr free is fine,
         // hence free both even if anyone fails
         qurt_elite_memory_free(pDataPayloadLb);
         return ADSP_ENOMEMORY;
      }

      pDataPayload->nActualSize = nOutBufSize;
      pDataPayload->nMaxSize    = nOutBufSize;
      pDataPayloadLb->nActualSize = nOutBufLoopbackSize;
      pDataPayloadLb->nMaxSize    = nOutBufLoopbackSize;

      if (ADSP_FAILED(nResult = elite_msg_push_payload_to_returnq(pVptx->io.output.bufq_ptr, (elite_msg_any_payload_t*) pDataPayload)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed fill Vptx buffer queue!! session(%x)",(int)pVptx->session.session_num);
         qurt_elite_memory_free(pDataPayload);
         qurt_elite_memory_free(pDataPayloadLb);
         return nResult;
      }
      pVptx->io.output.bufs_allocated++;

      if (ADSP_FAILED(nResult = elite_msg_push_payload_to_returnq(pVptx->io.loopback_output.bufq_ptr, (elite_msg_any_payload_t*) pDataPayloadLb)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed fill Vptx loopback buffer queue!! session(%x)",(int)pVptx->session.session_num);
         qurt_elite_memory_free(pDataPayloadLb);  // Since the previously successful buffers are already on the queue,
         // they would be freed by elite_svc_deinit_buf_queue utility
         return nResult;
      }
      pVptx->io.loopback_output.bufs_allocated++;
   }
   //dbg: MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"VCP: vptx_out_buf_allocator end nResult=%x session(%x)",nResult,(int)pVptx->session.session_num);
   return nResult;
}

void vptx_circ_buf_init(vptx_t* pVptx, uint32_t nNearCircBufSamples, uint32_t nFarCircBufSamples, uint32_t nOutCircBufSamples, uint32_t nLoopCircBufSamples)
{
   // Initialize Circular Buffer for Near
   voice_multi_ch_circ_buf_init(&(pVptx->io.near_in.circ_buf), (int32_t) pVptx->io.near_in.mediatype.num_channels, (int32_t) nNearCircBufSamples, (int32_t) 16);
   voice_multi_ch_circ_buf_init(&(pVptx->io.far_in.circ_buf), (int32_t) pVptx->io.far_in.mediatype.num_channels, (int32_t) nFarCircBufSamples, (int32_t) 16);
   voice_multi_ch_circ_buf_init(&(pVptx->io.output.circ_buf), (int32_t) pVptx->io.output.mediatype.num_channels, (int32_t) nOutCircBufSamples, (int32_t) 16);
   voice_multi_ch_circ_buf_init(&(pVptx->io.loopback_output.circ_buf), (int32_t) pVptx->io.loopback_output.mediatype.num_channels, (int32_t) nLoopCircBufSamples, (int32_t) 16);
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx circ buf pointers near(%#x),far(%#x),out(%#x), session(%lx)",(unsigned int)&(pVptx->io.near_in.circ_buf), (unsigned int)&(pVptx->io.far_in.circ_buf), (unsigned int)&(pVptx->io.output.circ_buf),pVptx->session.session_num);
}

static void voice_result_check(ADSPResult nResult, uint32_t session_num)
{
   if (ADSP_EOK != nResult)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx operation error nResult %d, session(%lx)",nResult,session_num);
   }
}

static ADSPResult Voice_CheckMediaType(elite_multi_channel_pcm_fmt_blk_t* pRefType, elite_multi_channel_pcm_fmt_blk_t* pMediaFormat)
{
   if (     (pRefType->sample_rate     != pMediaFormat->sample_rate)
         && (pRefType->num_channels    != pMediaFormat->num_channels)
         && (pRefType->bits_per_sample != pMediaFormat->bits_per_sample)
         && (pRefType->is_interleaved  != pMediaFormat->is_interleaved)
         && (pRefType->is_signed       != pMediaFormat->is_signed)
      )
   {
      return ADSP_EFAILED; //TODO: change it to unsupported
   }
   else
   {
      return ADSP_EOK;
   }
}

int32_t vptx_send_dtmf_tone_status(vptx_t* pVptx, void* data_ptr)
{
   vpm_tx_dtmf_detected_t dtmf_tone_status; // low frequency and high frequency
   int32_t rc = ADSP_EOK;
   dtmf_detection_payload_t* dtmf_status_ptr = (dtmf_detection_payload_t*)data_ptr;
   if(0 != dtmf_status_ptr->version)
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Invalid DTMF Det payload version(%d) !",dtmf_status_ptr->version);
       return ADSP_EFAILED;
   }
   for (uint32_t i =0; i < dtmf_status_ptr->no_of_tones_detected; i++)
   {
      dtmf_tone_status.low_freq = dtmf_status_ptr->low_freq[i];
      dtmf_tone_status.high_freq = dtmf_status_ptr->high_freq[i];
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: APR pkt for VPTX DTMF tone status sent to client, session(%lx)",pVptx->session.session_num);

      rc = elite_apr_if_alloc_send_event( pVptx->modules.apr_info_dtmf.apr_handle,
            pVptx->modules.apr_info_dtmf.self_addr,
            pVptx->modules.apr_info_dtmf.self_port,
            pVptx->modules.apr_info_dtmf.client_addr,
            pVptx->modules.apr_info_dtmf.client_port,
            NULL,
            VPM_EVT_TX_DTMF_DETECTED,
            &dtmf_tone_status,
            sizeof(vpm_tx_dtmf_detected_t));

      if (ADSP_FAILED(rc))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to create APR request for VPTX DTMF TONE SEND STATUS  0x%08lx :session(%lx)",rc,pVptx->session.session_num);
      }
   }
   return rc;
}

int32_t vptx_send_tx_speech_activity_status(vptx_t* pVptx, void* data_ptr)
{
   int32_t rc = ADSP_EOK;
   speech_activity_update_payload_t* activity_update = (speech_activity_update_payload_t*)data_ptr;
   vpm_voice_activity_status_update_t new_activity;

   switch(activity_update->activity_class)
   {
      case SPEECH_ACTIVITY_CLASS_BREAK:
      {
         new_activity.activity_type = VPM_VOICE_ACTIVITY_MIC_BREAK;
         //by default set the activity detection flag to true. if the activity is not detected then set it to false
         if(BREAK_ACTIVITY_NOT_DETECTED == activity_update->sub_activity)
         {
            new_activity.activity_type = VPM_VOICE_ACTIVITY_MIC_UNBREAK;
         }
      }
      break;
      case SPEECH_ACTIVITY_CLASS_MUTE:
      {
         new_activity.activity_type = VPM_VOICE_ACTIVITY_TX_MUTE;
         //by default set the activity detection flag to true. if the activity is not detected then set it to false
         if(MUTE_ACTIVITY_NOT_DETECTED == activity_update->sub_activity)
         {
            new_activity.activity_type = VPM_VOICE_ACTIVITY_TX_UNMUTE;
         }
      }
      break;
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_send_tx_speech_activity_status:  unknown activity class(%#lx)",
                  activity_update->activity_class);
         return ADSP_EUNSUPPORTED;
      }
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_send_tx_speech_activity_status:  activity_type(%#lx) is being sent to client.session(%#lx)",
         new_activity.activity_type, pVptx->session.session_num);

   if(pVptx->voice_activity_status.activity_type == new_activity.activity_type )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: no change in activity type session(%#lx)",  pVptx->session.session_num);
      return rc;
   }

   rc = elite_apr_if_alloc_send_event( pVptx->vpm_apr_info.apr_handle,
         pVptx->vpm_apr_info.self_addr,
         pVptx->vpm_apr_info.self_port,
         pVptx->vpm_apr_info.client_addr,
         pVptx->vpm_apr_info.client_port,
         NULL,
         VPM_EVT_VOICE_ACTIVITY_UPDATE,
         &new_activity,
         sizeof(vpm_voice_activity_status_update_t));
   if (ADSP_FAILED(rc))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Failed to create/send APR request for voice activity(%#lx) :session(%#lx)",rc,pVptx->session.session_num);
      return rc;
   }

   return rc;
}

int16_t voice_get_hpcm_phase(vptx_t* pVptx)
{
   if (pVptx->io.near_in.frame_samples == (OUT_BUF_CNT * FRAME_SAMPLES * pVptx->session.nb_sampling_rate_factor))
   {
      return (0 == pVptx->io.out_gen_cnt);
   }
   else // if (pVptx->io.near_in.frame_samples < OUT_BUF_CNT * FRAME_SAMPLES * nb_sampling_rate_factor)
   {
      return ((OUT_BUF_CNT * FRAME_SAMPLES * pVptx->session.nb_sampling_rate_factor) == (pVptx->io.out_gen_cnt + pVptx->io.near_in.frame_samples));
   }
}

void vptx_cal_drift(vptx_t *pVptx, int32_t *inst_drift)
{
   // Read AFE drift and find out the drift
   if (NULL != pVptx->afe_info.afe_tx_drift_ptr) // Audio path doesnt use the VOICEPROC_RUN_CMD, hence need the NULL check
   {
      voice_calc_inst_drift(
            &pVptx->ratematch.voice_drift_info_near,
            pVptx->afe_info.afe_tx_drift_ptr,
            pVptx->session.vfr_source,
            VOICEPROC_TX_NEAR,
            pVptx->session.vfr_mode,
            pVptx->session.session_num,
            pVptx->timing.timing_ver,
            pVptx->timing.vsid,
            inst_drift);

      //discard the drift if resync is detected
      if(pVptx->is_resync_detected_near)
      {
          MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: NEAR resync is detected ignoring inst drift. session(%x)", (int)pVptx->session.session_num);
         *inst_drift = 0;
         pVptx->is_resync_detected_near = FALSE;
      }
   }
   // Only if rx port is connected and currently running
   if ((NULL != pVptx->afe_info.afe_rx_drift_ptr) && (FALSE == pVptx->io.force_far_zeros))
   {
      voice_calc_inst_drift(
            &pVptx->ratematch.voice_drift_info_far,
            pVptx->afe_info.afe_rx_drift_ptr,
            pVptx->session.vfr_source,
            VOICEPROC_TX_FAR,
            pVptx->session.vfr_mode,
            pVptx->session.session_num,
            pVptx->timing.timing_ver,
            pVptx->timing.vsid,
            inst_drift + 1);

      //discard the drift if resync is detected
      if(pVptx->is_resync_detected_far)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: FAR resync is detected ignoring inst drift. session(%x)", (int)pVptx->session.session_num);
         *(inst_drift + 1) = 0;
         pVptx->is_resync_detected_far = FALSE;
      }
   }
}

static void vptx_vtm_unsubscribe(void *pInstance,Vtm_SubUnsubMsg_t *unsubscribe_ptr, uint32_t mask)
{
   vptx_t* pVptx = (vptx_t*) pInstance;
   ADSPResult nResult;

   // Dont subscribe/unsubsribe for audio path and FTRT modes
   if((!pVptx->session.real_time_flag) || (VPTX_AUDIO == pVptx->session.audio_instance_flag))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Audio path/FTRT vptx, no unsubscribe needed session(%x)", (int)pVptx->session.session_num);
      return;
   }

   if ((NULL != unsubscribe_ptr) && (NULL != pInstance))
   {
      if (ADSP_FAILED(nResult = voice_custom_vt_sub_unsub_msg_send(NULL,pVptx->timing.vtm_cmdq_ptr,VOICE_TIMER_UNSUBSCRIBE,NULL,FALSE,unsubscribe_ptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed Vptx unsubscribe to Vtm session(%x)",(int)pVptx->session.session_num);
         return; // return without clearing data, fatal msg -> to help with sim debugging
      }
      // wait syncronously for unsubscribing
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Waiting for Vptx unsubscribe to Vtm, session(%x)",(int)pVptx->session.session_num);
      (void)qurt_elite_channel_wait(&pVptx->qurt_elite_channel, mask);
      qurt_elite_signal_clear(unsubscribe_ptr->signal_end_ptr);
      unsubscribe_ptr->offset         = 0;
      unsubscribe_ptr->signal_enable  = 0;
   }
}

static void vptx_reinit_circbuf(void *pInstance)
{
   vptx_t* pVptx = (vptx_t*) pInstance;
   int8_t *buf_ptr[MAX_CIRCBUF_CHANNELS]; // for multi channel circ buf
   int16_t aLocalBuf[PREBUF_SAMPLES(VOICE_FB_SAMPLING_RATE)]; // local buffer size is set for worst case = 48(1ms at 48K sampling rate)

   // Reset all circular buffers
   voice_multi_ch_circ_buf_reset(&(pVptx->io.near_in.circ_buf));
   voice_multi_ch_circ_buf_reset(&(pVptx->io.far_in.circ_buf));
   voice_multi_ch_circ_buf_reset(&(pVptx->io.output.circ_buf));
   voice_multi_ch_circ_buf_reset(&(pVptx->io.loopback_output.circ_buf));

   // pre-buffering for sample slip/stuff for input buffers
   memset(&aLocalBuf[0], 0, sizeof(aLocalBuf));

   for (uint32 i =0 ; i< MAX_CIRCBUF_CHANNELS; i++)
   {
      buf_ptr[i] = (int8_t*) &(aLocalBuf[0]);
   }

   // write zeros into circular buffer for sample slip/stuff space
   pVptx->ratematch.ss_buf_samples_near = pVptx->io.prebuffer_samples;
   pVptx->ratematch.ss_buf_samples_far = pVptx->ratematch.ss_buf_samples_near ;
   voice_multi_ch_circ_buf_write(&(pVptx->io.near_in.circ_buf), &(buf_ptr), (int32_t)pVptx->ratematch.ss_buf_samples_near);
   voice_multi_ch_circ_buf_write(&(pVptx->io.far_in.circ_buf), &(buf_ptr), (int32_t)pVptx->ratematch.ss_buf_samples_far);
}

static void vptx_clear_input_dataq(void* pInstance)
{
   vptx_t* pVptx = (vptx_t*) pInstance;
   /* guarantee that any queued near/far data buffers are returned, so AFE can free them properly during VPM_STOP processing */
   {
      elite_msg_any_t   dataQMsg;
      elite_msg_any_t   farMediaMsg = {0};
      ADSPResult local_result;
      do
      {
         // Non-blocking MQ receive
         local_result = qurt_elite_queue_pop_front(pVptx->near_svc_handle.dataQ, (uint64_t*)&dataQMsg);
         // return the buffer to its rightful q.
         if (ADSP_EOK == local_result)
         {
            elite_msg_return_payload_buffer(&dataQMsg);
         }
      } while (ADSP_EOK == local_result);

      do
      {
         // Non-blocking MQ receive
         local_result = qurt_elite_queue_pop_front(pVptx->far_svc_handle.dataQ, (uint64_t*)&dataQMsg);
         // return the buffer to its rightful q.
         if (ADSP_EOK == local_result)
         {
            if (ELITE_DATA_MEDIA_TYPE == dataQMsg.unOpCode )
            {
               if(farMediaMsg.unOpCode == ELITE_DATA_MEDIA_TYPE)
               {
                  elite_msg_return_payload_buffer(&farMediaMsg);
               }
               farMediaMsg = dataQMsg;
            }
            else
            {
               elite_msg_return_payload_buffer(&dataQMsg);
            }
         }
      } while (ADSP_EOK == local_result);
//      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Cleared Far Data, pending media type(%x)",farMediaMsg.unOpCode);	  
      if(farMediaMsg.unOpCode == ELITE_DATA_MEDIA_TYPE)
      {
         qurt_elite_queue_push_back(pVptx->far_svc_handle.dataQ,(uint64_t*)&farMediaMsg);
      }
   }
   return;
}

static void vptx_resync(void* pInstance)
{
   vptx_t* pVptx = (vptx_t*) pInstance;
   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: Vptx Resync");
   // if resync issued then drop all data and start collecting new data
   // keep vfr to true, Instantaneous resync

   pVptx->io.out_gen_cnt = 0;
   //reinit ss algo, otherwise it preserves state from last run and interpolates incorrectly

   capi_v2_buf_t param_data_buf;
   capi_v2_port_info_t port_info;
   port_info.is_valid = FALSE;

   param_data_buf.data_ptr = NULL; // This is just to intimate to clear SS state
   param_data_buf.actual_data_len = param_data_buf.max_data_len = 0;

   if (pVptx->modules.ss_capi.module_ptr)  // EC exisits in topology
   {
      pVptx->modules.ss_capi.module_ptr->vtbl_ptr->set_param(pVptx->modules.ss_capi.module_ptr,
            SS_PARAM_RESET,&port_info,&param_data_buf);

   }
   else
   {
      pVptx->modules.special_capis[ECNS_CAPI]->module_ptr->vtbl_ptr->set_param(pVptx->modules.special_capis[ECNS_CAPI]->module_ptr,
            SS_PARAM_RESET,&port_info,&param_data_buf);
   }

   // reset device drift counters
   pVptx->is_resync_detected_near = TRUE;
   pVptx->is_resync_detected_far  = TRUE;
   //memset(&(pVptx->ratematch.voice_drift_info_near), 0, sizeof(pVptx->ratematch.voice_drift_info_near));
   //memset(&(pVptx->ratematch.voice_drift_info_far), 0, sizeof(pVptx->ratematch.voice_drift_info_far));


   vptx_reinit_circbuf(pInstance); // reset and init circular buffers with required pre-buffering
   vptx_clear_input_dataq(pInstance); //make sure no near/far input buffers are held
}

static void vptx_far_resync(void* pInstance)
{
   vptx_t* pVptx = (vptx_t*) pInstance;
   pVptx->timing.resync_far_end = FALSE;
   // Stop forcing zeros
   pVptx->io.force_far_zeros = FALSE;
   //copy over data to circbuf and to ss state. reset circbuf to drop any data that may have come in the meanwhile
   voice_multi_ch_circ_buf_reset(&(pVptx->io.far_in.circ_buf));
   for (uint32_t i = 0 ; i<  pVptx->io.far_in.mediatype.num_channels ; i++)
   {
       memset(pVptx->io.far_in.process_buf_ptr[i], 0, (pVptx->io.near_in.circ_buf.circ_buf[0].unread_samples)<<1);
   }
   voice_multi_ch_circ_buf_block_write(&(pVptx->io.far_in.circ_buf), (int8_t*)pVptx->io.far_in.process_buf_ptr[0], (pVptx->io.near_in.circ_buf.circ_buf[0].unread_samples) * pVptx->io.far_in.mediatype.num_channels );
   pVptx->ratematch.ss_buf_samples_far = pVptx->ratematch.ss_buf_samples_near;

   //reinit ss algo, otherwise it preserves state from last run and interpolates incorrectly

   capi_v2_buf_t param_data_buf;
   capi_v2_port_info_t port_info;
   port_info.is_valid = TRUE;
   port_info.port_index = 1;  // signifying far port

   param_data_buf.data_ptr = NULL; // This is just to intimate to clear SS state
   param_data_buf.actual_data_len = param_data_buf.max_data_len = 0;
   // calculate number of slip/stuff samples
   if (pVptx->modules.ss_capi.module_ptr)
   {

      pVptx->modules.ss_capi.module_ptr->vtbl_ptr->set_param(pVptx->modules.ss_capi.module_ptr,
            SS_PARAM_RESET,&port_info,&param_data_buf);
   }
   else
   {
      pVptx->modules.special_capis[ECNS_CAPI]->module_ptr->vtbl_ptr->set_param(pVptx->modules.special_capis[ECNS_CAPI]->module_ptr,
                 SS_PARAM_RESET,&port_info,&param_data_buf);

   }

   // reset device drift counters
   pVptx->is_resync_detected_far = TRUE;
   //memset(&(pVptx->ratematch.voice_drift_info_far), 0, sizeof(pVptx->ratematch.voice_drift_info_far));


   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx far resync, setting up far ref, samples %d, ss samples %d, ss counter %d session(%x)", (int)pVptx->io.near_in.circ_buf.circ_buf[0].unread_samples, (int)pVptx->ratematch.ss_buf_samples_far, (int)pVptx->session.session_num);
}

static void vptx_calc_offsets(void* pInstance)
{
   vptx_t* pVptx = (vptx_t*)pInstance;
   int32 delivery_offset = 0;
   int32 sync_offset = 0;
   uint32_t venc_processing_time = 0;
   uint32_t venc_kpps = 0;
   uint32_t vptx_processing_time = 0;

   // delivery_offset calculation
   venc_calc_processing_time(pVptx->timing.session_thread_clock_mhz, pVptx->session.sampling_rate, &venc_processing_time);

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx: Venc clock_mhz(%lu) kpps(%d), venc processing time per frame:(%d),frame size:(%d) in ms, session(%x)",
         pVptx->timing.session_thread_clock_mhz, (int)venc_kpps, (int)venc_processing_time, 20, (int)pVptx->session.session_num);
   delivery_offset = pVptx->timing.enc_offset - venc_processing_time;
   pVptx->timing.delivery_offset = ((0 > delivery_offset) ? (20000 + delivery_offset) : delivery_offset); // Mod20 operation for 20ms encoder frame

   vcmn_calc_processing_time(pVptx->timing.session_thread_clock_mhz, pVptx->timing.modules_kpps, pVptx->io.num_1ms_frame_block_process, &vptx_processing_time); // find out the processing time in microsecs for venc
   if(VPTX_MAX_PROCESSING_TIME_IN_US < vptx_processing_time)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! Vptx processing time too high session(%x)", (int)pVptx->session.session_num);
      vptx_processing_time = 19000; //this is just a value that's hopefully high enough
   }
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx modules kpps(%d), Vptx processing time per frame:(%d),frame size:(%d) in ms, session(%x)", (int)pVptx->timing.modules_kpps, (int)vptx_processing_time, pVptx->io.num_1ms_frame_block_process, (int)pVptx->session.session_num);

   //need to ensure that sync and delivery offsets are not too close.
   // If they are, jitters can cause sync handler > delivery handler execution without going through buf handler
   // since the other two handlers have higher priorities. This can cause misses/phase mismatches
   vptx_processing_time = ((vptx_processing_time + VPTX_PROCESSING_MARGIN_IN_US) > VPTX_MIN_PROCESSING_MARGIN_IN_US) ?
      (vptx_processing_time+ VPTX_PROCESSING_MARGIN_IN_US): VPTX_MIN_PROCESSING_MARGIN_IN_US;

   sync_offset = pVptx->timing.enc_offset - (venc_processing_time + vptx_processing_time);
   sync_offset = ((0 > sync_offset) ? (20000 + sync_offset) : sync_offset); // Mod20 operation for 20ms encoder frame
   sync_offset = ((0 > sync_offset) ? (20000 + sync_offset) : sync_offset); // Mod20 operation for 20ms encoder frame
   // consider offset changed only if new processing time is more than old processing time
   pVptx->timing.sync_offset = sync_offset;

   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx offset calculation, dsp_clock(%d), venc_kpps(%d), sync_offset(%d), delivery_offset(%d),  enc_offset(%d), session(%x)",
         (int)pVptx->timing.session_thread_clock_mhz, (int)venc_kpps, pVptx->timing.sync_offset, pVptx->timing.delivery_offset,  pVptx->timing.enc_offset, (int)pVptx->session.session_num);

}

static ADSPResult venc_calc_processing_time(uint32_t dsp_clock_mhz, uint32_t sampling_rate, uint32_t* processing_time)
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

      *processing_time = (uint32_t) voice_txstream_proc_time[sampling_rate_index][clock_index] * VOICE_CACHE_MISS_FACTOR_OVER_1MHZ/1000;
      return ADSP_EOK;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice cmn: NULL pointer for processing timer");
      return ADSP_EBADPARAM;
   }
}

static ADSPResult vptx_set_timingv3_cmd(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t* pVptx = (vptx_t*)pInstance;
   elite_msg_custom_voc_timing_param_type *pSetTimingCmd = (elite_msg_custom_voc_timing_param_type *) pMsg->pPayload;
   vpm_set_timing_params_v2_t* pVfrCmd = (vpm_set_timing_params_v2_t*)pSetTimingCmd->param_data_ptr;

   // Check for Run state
   if (FALSE == pVptx->process_data_flag)
   {
      if (VPTX_AUDIO == pVptx->session.audio_instance_flag)
      {
         // For Audio use case, hard code to None VFR mode.
         pVptx->session.vfr_mode = VFR_NONE;
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VPM_CMD_SET_TIMING_PARAMS_V2,Vptx vfr_mode(%d), cannot be anything other than VFR_NONE(%d), session(%#lx)",pVfrCmd->mode, VFR_NONE, pVptx->session.session_num);
         nResult = ADSP_EBADPARAM;
      }
      else
      {
         //dbg msg is printed in VPM with all params
         /*MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: vptx_set_timingv3_cmd, mode(%d), VSID(0x%lx), \
               vptx_start(%d), vptx_delivery(%d),session_index(%x)",
               pVfrCmd->mode,pVfrCmd->vsid,pVfrCmd->vptx_start_offset,pVfrCmd->vptx_delivery_offset,
               pVptx->session.session_num); */

         // Verify validity of VFR mode. In this version of timing cmd, mode supports only two values - VFR_NONE and VFR_HARD
         // If VFR_HARD, further information is derived from VSID
         if ((VFR_HARD >= pVfrCmd->mode) )
         {
            pVptx->session.vfr_mode = pVfrCmd->mode;
         }
         else
         {
            pVptx->session.vfr_mode = VFR_NONE;
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_set_timingv3_cmd, invalid mode(%x),setting to VFR_NONE,session(%lx)",pVfrCmd->mode,pVptx->session.session_num);
            nResult = ADSP_EBADPARAM;
         }

         pVptx->session.vfr_cycle = pVfrCmd->vfr_cycle/MAX_TIMER_OFFSET;

         // Verify validity of VSID. In VFR_HARD case, VSID should be non-zero. In VFR_NONE case, VSID is don't care.
         if ((VFR_HARD == pVptx->session.vfr_mode) && (0 == pVfrCmd->vsid))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_set_timingv3_cmd, invalid VSID(%lx), session(%lx)",pVfrCmd->vsid,pVptx->session.session_num);
            nResult = ADSP_EBADPARAM;
         }
         else
         {
            pVptx->timing.vsid = pVfrCmd->vsid;
         }
         uint32_t max_time = MAX_TIMER_OFFSET*pVptx->session.vfr_cycle;
         // Verify validity of sync and delivery offsets
         // MIN_TIMER_OFFSET is set to 0, so no need to check if offset is below min because it's unsigned
         if (max_time < pVfrCmd->vptx_start_offset || max_time < pVfrCmd->vptx_delivery_offset || max_time < pVfrCmd->enc_offset)
         {
            pVptx->timing.delivery_offset = 9000; // default to max offset (19ms safe) from default enc pkt delivery offset
            pVptx->timing.sync_offset = 10000;    // default to max offset (19ms safe) from default vptx delivery offset
            pVptx->timing.proc_offset = 12000;    // TBD
            pVptx->timing.enc_offset  = 8000;     // default to max offset (19ms safe) from default venc delivery offset
            MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_set_timingv3_cmd, Invalid sync(%d) or delivery(%d) or venc delivery(%d) offsets, defaulting to 8ms, 9ms and 10ms, session(%lx)", \
                  pVfrCmd->vptx_start_offset,pVfrCmd->vptx_delivery_offset, pVfrCmd->enc_offset, pVptx->session.session_num);
            nResult = ADSP_EBADPARAM;
         }
         else
         {
            pVptx->timing.sync_offset = pVfrCmd->vptx_start_offset;
            pVptx->timing.proc_offset =  pVfrCmd->vptx_proc_start_offset;
            pVptx->timing.delivery_offset = pVfrCmd->vptx_delivery_offset;
            pVptx->timing.enc_offset = pVfrCmd->enc_offset;
         }
         pVptx->timing.use_new_timing_flag = TRUE;
         pVptx->timing.timing_ver = VFR_CLIENT_INFO_VER_2;  // version 2 (VSID) of timing

         int32_t process_samples_in_us = pVfrCmd->vptx_proc_start_offset - pVfrCmd->vptx_start_offset;
         if ( process_samples_in_us <= 0 )
         {
            process_samples_in_us += pVptx->session.vfr_cycle*MAX_TIMER_OFFSET;
         }
         pVptx->proc_start_samples = (process_samples_in_us/1000)*(pVptx->session.nb_sampling_rate_factor<<3);

         int32_t venc_processing_time = 0;
         int32_t vptx_processing_time = 0;
         int32_t vptx_processing_per_frame = 0;

         vptx_processing_time = pVptx->timing.delivery_offset - pVptx->timing.proc_offset;
         if(vptx_processing_time <= 0)
         {
            vptx_processing_time += pVptx->session.vfr_cycle*MAX_TIMER_OFFSET;
         }
         vptx_processing_per_frame = (10 == pVptx->io.num_1ms_frame_block_process) ? vptx_processing_time >> 1: vptx_processing_time;
         venc_processing_time = pVptx->timing.enc_offset - pVptx->timing.sync_offset - vptx_processing_per_frame;
         if(venc_processing_time <= 0)
         {
            venc_processing_time += pVptx->session.vfr_cycle*MAX_TIMER_OFFSET;
            if(venc_processing_time <= 0)
            {
		venc_processing_time += pVptx->session.vfr_cycle*MAX_TIMER_OFFSET;
	    }
         }
         pVptx->max_tx_processing_time = (uint32_t)(pVptx->session.vfr_cycle*MAX(venc_processing_time,vptx_processing_time)+MIN(venc_processing_time,vptx_processing_time));
         MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"VCP: VPM_CMD_SET_TIMING_PARAMS vptx_proc_start_samples = (%d) and max_tx_processing_time = (%d) session(%lx)",
                                           (int)pVptx->proc_start_samples,
                                           (int)pVptx->max_tx_processing_time,
                                           pVptx->session.session_num);

      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VFR mode can't be changed in RUN session(%lx)",pVptx->session.session_num);
      nResult = ADSP_EBUSY;
   }
   elite_svc_send_ack(pMsg, nResult);
   return nResult;
}



/* Handler running in Vptx thread context that ensures successful registration is
 * conveyed to Vptx thread and also to stop listening to any further Sensors
 * registration signals.
 */
ADSPResult vptx_qmi_reg_handler(void* pInstance)
{
   vptx_t *pVptx = (vptx_t*)pInstance;       // instance structure

   vptx_clear_sns_qmi_signal(pVptx);

   NOT_MASK(pVptx->wait_mask,VPTX_QMI_REG_MASK); // Any more Sensors registration signals are redundant.
   return ADSP_EOK;
}


/* Handler running in Vptx thread context that is invoked when an indication
 * is received from Sensors
 */
ADSPResult vptx_qmi_ind_handler(void* pInstance)
{
   vptx_t *pVptx = (vptx_t*)pInstance;       // instance structure

   vptx_handle_sns_qmi_ind(pVptx);
   return ADSP_EOK;
}


void vptx_calibration_cb_func(void* cal_params_payload_ptr, void* cb_data)
{
   ADSPResult result = ADSP_EOK;
   vptx_t* pVptx = (vptx_t*)cb_data;
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

   //dbg: MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx cb invoked, mod id %lx, param id %lx, pointer %lx, size %lx, session(%lx)",
   //           mod_id, param_id, cal_params_ptr->param_data, param_size, pVptx->session.session_num);

   result = vptx_modules_set_param_int(pVptx, mod_id,instance_id, param_id, param_data, param_size);
   if(ADSP_EOK != result && ADSP_EUNSUPPORTED != result)
   {
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Vptx set param error %u, mod %lx, instance %lx param %lx, session(%lx)",
            result, mod_id,instance_id, param_id, pVptx->session.session_num);
   }
   return;
}

static ADSPResult vptx_set_param_v3_cmd (void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   vptx_t* pVptx = (vptx_t*)pInstance;
   elite_msg_custom_set_param_v3_type *payload = (elite_msg_custom_set_param_v3_type*)pMsg->pPayload;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx set_param_v3, handle %lx, session(%lx)", payload->cal_handle, pVptx->session.session_num);

   if (VPM_TX_NONE == pVptx->session.topology_id) // no calibration support in none topology
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: No calibration supported on none topology-returning. session(%x)",(int)pVptx->session.session_num);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return result;
   }

   //call into mvm using mvm_call as an entry point to cvd_cal_query
   vss_imvm_cmd_cal_query_t mvm_payload;
   mvm_payload.query_handle = payload->cal_handle;
   mvm_payload.cb_fn = vptx_calibration_cb_func;
   mvm_payload.client_data = pInstance;

   result = mvm_call(MVM_CMDID_CAL_QUERY, &mvm_payload, sizeof(mvm_payload));
   //Any necessary actions that need to take place once cal is applied, e.g. EC initialization
   vptx_modules_set_param_end(pVptx);
   elite_svc_send_ack(pMsg, result);
   return result;
}

static ADSPResult vptx_reg_event_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   vptx_t* vptx_ptr = (vptx_t*)instance_ptr;
   elite_msg_custom_event_reg_type *payload_ptr = (elite_msg_custom_event_reg_type*) msg_ptr->pPayload;
   uint32_t event_id = payload_ptr->event_id;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VPTx received registration for event (0x%lx)", event_id);

   //All validations are done in VPM so just update the variable.
   switch(event_id)
   {
      case VPM_EVT_VOICE_ACTIVITY_UPDATE:
      {
         // set report voice activity flag to true
         vptx_ptr->voice_activity_status.report_voice_activity = TRUE;
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VPTx received registration for invalid event (0x%lx)", event_id);
      }
   }
   elite_msg_return_payload_buffer( msg_ptr );
   return ADSP_EOK;
}

static ADSPResult vptx_unreg_event_cmd(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   vptx_t* vptx_ptr = (vptx_t*)instance_ptr;
   elite_msg_custom_event_reg_type *payload_ptr = (elite_msg_custom_event_reg_type*) msg_ptr->pPayload;
   uint32_t event_id = payload_ptr->event_id;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: VPTx received unregistration for event (0x%lx)", event_id);

   //All validations are done in VPM so just update the variable.
   switch(event_id)
   {
      case VPM_EVT_VOICE_ACTIVITY_UPDATE:
      {
         // set report voice activity flag to false
         vptx_ptr->voice_activity_status.report_voice_activity = FALSE;
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: Error! VPTx received registration for invalid event (0x%lx)", event_id);
      }
   }
   elite_msg_return_payload_buffer( msg_ptr );
   return ADSP_EOK;
}

static ADSPResult vptx_set_ep_id(void* instance_ptr, elite_msg_any_t* msg_ptr)
{
   ADSPResult result = ADSP_EOK;

   vptx_t* pVptx = (vptx_t*)instance_ptr;   
   elite_msg_custom_set_ep_id *ep_id_payload_ptr = (elite_msg_custom_set_ep_id*) msg_ptr->pPayload;
   
   if(FALSE == pVptx->process_data_flag)
   {
      //STOP state hence accept command. 
      if( (INVALID_PORT != ep_id_payload_ptr->ep_id[0]) && 
         (pVptx->afe_info.afe_tx_port_id != ep_id_payload_ptr->ep_id[0]) )
      {
         //Near port cannot be invalid hence check it. ep[0] is valid and has changed
         pVptx->afe_info.afe_tx_port_id = ep_id_payload_ptr->ep_id[0];
         if (IS_PSEUDO_PORT_AFE_ID(pVptx->afe_info.afe_tx_port_id))
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_set_ep_id(): Psuedo Port, hence making VPTX non real time/ faster than real time");
            pVptx->session.real_time_flag = FALSE; // non real time mode if pseudo_port
         }
         else
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_set_ep_id(): Real Port, hence making VPTX real time");
            pVptx->session.real_time_flag = TRUE;
         }
      }

      //update far port. It can be INVALID port as well so no checks.
      pVptx->afe_info.afe_rx_port_id = ep_id_payload_ptr->ep_id[1];
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_set_ep_id(): Error!! Service in RUN state");
      result = ADSP_ENOTREADY;
   }
   
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_set_ep_id(): VPTx sending ack for set ep id. session (0x%lx). ep_ids [ 0x%hx, 0x%hx ]",
       pVptx->session.session_num, ep_id_payload_ptr->ep_id[0], ep_id_payload_ptr->ep_id[1] );

   elite_svc_send_ack(msg_ptr, result);
   return result;
}

static ADSPResult vptx_venc_delivery_tick_handler(void *pInstance)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t *pVptx = (vptx_t*)pInstance;

   //dbg: MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_venc_delivery_tick_handler begin session(%x)",(int)pVptx->session.session_num);

   qurt_elite_signal_clear(pVptx->timing.venc_delivery_tick_vtm_struct.signal_ptr);

   // pVptx->process_start_flag =  TRUE indicates that VPTX is already done collecting data for first frame
   // Its either going to start vptx process now or it has already started vptx process  (refer sync tick handler to check when this flag is enabled)
   // Enable BUF_MASK here and set "is_venc_tick_fired to TRUE"
   // Buff handler will be invoked right after venc delivery tick handling
   // if vptx is done processing and already has sufficient samples in output circ buff then buffer will be queued to VDS
   // if vptx does not have sufficient samples in output circ buff then buff handler will not be triggered here
   // once vptx has sufficient samples in output circ buff, it will enable BUFF MASK if is_venc_tick_fired == TRUE.
   // After that buff handler will be invoked and buffer will be queued to VDS
   if(TRUE == pVptx->process_start_flag)
   {
      if(pVptx->io.output.buf_samples <= pVptx->io.output.circ_buf.circ_buf[0].unread_samples)
      {
      OR_MASK(pVptx->wait_mask,VPTX_BUF_MASK);
      }
      pVptx->is_venc_tick_fired = TRUE;
   }
   return nResult;
}


static ADSPResult vptx_venc_delivery_tick_end_handler(void *pInstance)
{
   ADSPResult nResult = ADSP_EOK;
   vptx_t *pVptx = (vptx_t*)pInstance;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: vptx_venc_delivery_tick_end_handler begin session(%x)",(int)pVptx->session.session_num);

   qurt_elite_signal_clear(pVptx->timing.venc_delivery_tick_vtm_struct.signal_end_ptr);

   return nResult;
}
