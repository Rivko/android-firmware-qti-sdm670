/**========================================================================
 @file vpm_svc.cpp
 
 @brief This file contains public API's for Voice Proc Manager static 
 service create, run and destroy. Also top level routines for handling
 commands and responses  for both static service and
 dynamic session objects. 
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_svc.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_svc_apr_if.h"
#include "vpm_svc_apr_data_base.h"
#include "vpm_session_utils.h"
#include "vpm_svc_vp3_utils.h"

#include "audio_basic_op_ext.h"
#include "mvm_api_i.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* VPM Thread name */
static char_t VPM_THREAD_NAME[] = "VPM";

/* Thread stack size in bytes */
static const uint32_t VPM_THREAD_STACK_SIZE = (12 * 1024);

/* VPM service command queue */
static char_t vpm_svc_cmd_q_name[] = "VPM_SVC_CMD_Q";
static const uint32_t vpm_svc_cmd_q_mask = 0x00000001UL;

/* VPM service response queue */
static char_t vpm_svc_rsp_q_name[] = "VPM_SVC_RESP_Q";
static const uint32_t vpm_svc_rsp_q_mask = 0x00000002UL;

static const uint32_t vpm_session_cmd_q_sig_start_bit_pos = 2;

static const uint32_t vpm_session_rsp_q_sig_start_bit_pos = 3;

/* VPM global service object */
static voice_proc_mgr_t  g_vpm_svc_object;

/* Pointer to VPM global service object */
voice_proc_mgr_t *p_vpm_svc_global_obj;

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Function Declarations
** ----------------------------------------------------------------------- */

static int voice_proc_mgr_work_loop(void *pInstance);

void voice_proc_mgr_destroy(voice_proc_mgr_t *pMe);

/**< System command queue and response queue handlers */
static ADSPResult  voice_proc_mgr_process_svc_cmd_q(void *p_svc_inst);
static ADSPResult voice_proc_mgr_process_svc_rsp_q(void *p_svc_inst);

/**< Service start / destroy svc command handlers */
static ADSPResult  voice_proc_mgr_start_svc_handler(void *p_svc_inst, elite_msg_any_t *p_msg_pkt);
static ADSPResult  voice_proc_mgr_destroy_svc_handler(void *p_svc_inst, elite_msg_any_t *p_msg_pkt);

/**< Session command queue and response queue handlers */
static ADSPResult voice_proc_mgr_process_session_cmd_q(vpm_session_object_t *p_session_obj);
static ADSPResult voice_proc_mgr_process_session_resp_q(vpm_session_object_t *p_session_obj);


ADSPResult vpm_svc_cmdq_cust_msg_handler(void *p_svc_inst,
                                         elite_msg_any_t *p_msg_pkt);

ADSPResult vpm_svc_cmd_rsp_handler(void *p_vpm_instance,
                                   elite_msg_any_t *p_msg_pkt);

ADSPResult vpm_svc_cmdq_apr_msg_handler(void *p_svc_instance, elite_msg_any_t *p_msg_pkt);

ADSPResult vpm_session_cmd_rsp_handler(void *p_session,
                                       elite_msg_any_t *p_msg_pkt);


//ADSPResult vpm_session_cmdq_cust_msg_handler(void *p_session_inst, elite_msg_any_t *p_msg_pkt);

ADSPResult vpm_session_cmdq_apr_msg_handler(void *p_session_inst, elite_msg_any_t *p_msg_pkt);



/* -----------------------------------------------------------------------
** Function Pointer Table for VPM command disaptch
** ----------------------------------------------------------------------- */

/**< Msg handler for static service cmdQ */
static voice_proc_mgr_svc_cmd_q_hndlr_f p_vpm_svc_cmdq_hndlr[] =
{
   vpm_svc_cmdq_cust_msg_handler,      /**< ELITE_CUSTOM_MSG */
   voice_proc_mgr_start_svc_handler,   /**< ELITE_CMD_START_SERVICE */
   voice_proc_mgr_destroy_svc_handler, /**< ELITE_CMD_DESTROY_SERVICE */
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   vpm_svc_cmdq_apr_msg_handler        /**< APR Packet handler */
};

/**< Msg handler for static service rspQ */
static voice_proc_mgr_svc_cmd_q_hndlr_f p_vpm_svc_rspq_hndlr[] =
{
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   vpm_svc_cmd_rsp_handler        /**< APR Packet handler */
};


/**< Msg handler for session resp Q */
static voice_proc_mgr_session_resp_q_hndlr_f p_vpm_session_respq_hndlr[] =
{
   vpm_session_cmd_rsp_handler,        //0 - ELITE_CUSTOM_MSG
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   elite_svc_unsupported,
   vpm_session_cmd_rsp_handler,        //8- ELITE_CMD_SET_PARAM
   vpm_session_cmd_rsp_handler,        //9- ELITE_CMD_GET_PARAM
   elite_svc_unsupported,              // ELITE_DATA_BUFFER
   elite_svc_unsupported,              // ELITE_DATA_MEDIA_TYPE
   elite_svc_unsupported,              // ELITE_DATA_EOS
   elite_svc_unsupported,              // ELITE_DATA_RAW_BUFFER
   elite_svc_unsupported,              // ELITE_CMD_STOP_SERVICE
   vpm_session_cmd_rsp_handler,        // ELITE_APR_PACKET
};

/* Msg handler for session command Q*/
static voice_proc_mgr_session_cmd_q_hndlr_f p_vpm_session_cmdq_hndlr[] =
{
   elite_svc_unsupported,             //0 - ELITE_CUSTOM_MSG
   elite_svc_unsupported,             //1 - ELITE_CMD_START_SERVICE
   elite_svc_unsupported,             //2 - ELITE_CMD_DESTROY_SERVICE
   elite_svc_unsupported,             //3 - ELITE_CMD_CONNECT
   elite_svc_unsupported,             //4 - ELITE_CMD_DISCONNECT
   elite_svc_unsupported,             //5 - ELITE_CMD_PAUSE
   elite_svc_unsupported,             //6 - ELITE_CMD_RESUME
   elite_svc_unsupported,             //7 - ELITE_CMD_FLUSH
   elite_svc_unsupported,             //8 - ELITE_CMD_SET_PARAM
   elite_svc_unsupported,             //9 - ELITE_CMD_GET_PARAM
   elite_svc_unsupported,             //A - ELITE_DATA_BUFFER
   elite_svc_unsupported,             //B - ELITE_DATA_MEDIA_TYPE
   elite_svc_unsupported,             //C - ELITE_DATA_EOS
   elite_svc_unsupported,             //D - ELITE_DATA_RAW_BUFFER
   elite_svc_unsupported,             //E - ELITE_CMD_STOP_SERVICE
   vpm_session_cmdq_apr_msg_handler   //F - ELITE_APR_PACKET
};

/* -----------------------------------------------------------------------
** Function Definitions
** ----------------------------------------------------------------------- */

static inline uint32_t vpm_map_channel_idx_to_session_idx(uint32_t sig_bit_pos)
{
   return ((sig_bit_pos >> 1) - 1);
}

ADSPResult voice_proc_mgr_create(uint32_t inputParam, void **handle)
{
   ADSPResult           result = ADSP_EOK;
   qurt_elite_queue_t   *p_q_temp;

   MSG(MSG_SSID_DFLT, DBG_MED_PRIO, "voice_proc_mgr_create: Creating static service..");

   /* Memset zero the service global object */
   memset(&g_vpm_svc_object, 0, sizeof(voice_proc_mgr_t));

   /* Init the global pointer to be shared across */
   p_vpm_svc_global_obj = (voice_proc_mgr_t *)&g_vpm_svc_object;

   voice_proc_mgr_t *pMe = p_vpm_svc_global_obj;

   /* Create service cmd queue */
   p_q_temp = (qurt_elite_queue_t *)pMe->svc_cmd_q;

   if (ADSP_FAILED(result = qurt_elite_queue_init(vpm_svc_cmd_q_name,
                                                  VPM_MAX_SVC_CMD_Q_ELEMENTS, p_q_temp)))

   {
      MSG(MSG_SSID_DFLT, DBG_FATAL_PRIO, "VoiceProcMgr: Failed to initialize service command queue");

      return result;
   }

   /* Init the cmdQ pointer in svc handle */
   pMe->svc_handle.cmdQ = p_q_temp;

   /* Init the VPM svc command queue qurt_elite global data base */
   qurt_elite_globalstate.pVPMSvcCmdQ = pMe->svc_handle.cmdQ;

   /* Update command Q in commond control for service */
   pMe->cmd_ctrl.p_cmd_queue = pMe->svc_handle.cmdQ;

   /* Save the wait mask in state variable */
   pMe->svc_cmd_q_wait_mask = vpm_svc_cmd_q_mask;

   /* Initialize service response queue */
   p_q_temp = (qurt_elite_queue_t *)pMe->svc_rsp_q;

   if (ADSP_FAILED(result = qurt_elite_queue_init(vpm_svc_rsp_q_name,
                                                  VPM_MAX_SVC_RSP_Q_ELEMENTS, p_q_temp)))

   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "VoiceProcMgr: Failed to initialize VoiceProcMgr response queue, result: %lu", result);

      return result;
   }

   /* Update response Q in response control for service */
   pMe->rsp_ctrl.p_rsp_queue = (qurt_elite_queue_t *)pMe->svc_rsp_q;

   /* Set up channel */
   qurt_elite_channel_init(&pMe->channel);

   /* Add service command queue to the channel */
   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel,
                                                    pMe->svc_handle.cmdQ,
                                                    vpm_svc_cmd_q_mask)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "VoiceProcMgr: Failed to add svc cmdq to channel, result: %lu",
            result);

      return result;
   }

   /* Add service response queue to the channel */
   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, p_q_temp, vpm_svc_rsp_q_mask)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "VoiceProcMgr: Failed to add svc rsp queue to channel, result: %lu", result);

      return result;
   }

   /* Populate me */
   pMe->svc_handle.unSvcId = ELITE_VOICE_DEVICE_SVCID;

   /* Create all session states here */
   if (ADSP_FAILED(result = vpm_create_session_static_info(pMe)))
   {
      return result;
   }

   /* Once session is created, initialize data base */
   if (ADSP_FAILED(result = voice_proc_mgr_apr_router_init(pMe)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "Failed to init APR router, result: %lu", result);

      return result;
   }

   /* Launch the thread */
   if (ADSP_FAILED(result = qurt_elite_thread_launch(&(pMe->svc_handle.threadId),
                                                     VPM_THREAD_NAME, NULL,
                                                     VPM_THREAD_STACK_SIZE,
                                                     elite_get_thrd_prio(ELITETHREAD_CVP_LOW_PRIO_ID),
                                                     voice_proc_mgr_work_loop,
                                                     (void *)pMe,
                                                     QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "Failed to create VocProcMgr Thread!");

      return result;
   }

   /* Populate service handler ptr to be returned to the caller */
   *handle = &(pMe->svc_handle);

   /* Register with qurt_elite memory map */
   qurt_elite_memorymap_register(&pMe->memory_map_client);

   /* MMPM client registration */
   (void)vccm_mmpm_register(VCCM_CLIENT_ID_CVP);

   /* Init global VP3 management */
   if (ADSP_EOK != (result = vpm_svc_vp3_init(&pMe->vp3_info)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "Failed to allocat VP3 management memory ");

      return result;
   }

   MSG(MSG_SSID_DFLT, DBG_MED_PRIO, "Voice Proc Mangager Created Successfully");

   return ADSP_EOK;
}

/* Destroy yourself*/
void voice_proc_mgr_destroy(voice_proc_mgr_t *pMe)
{
   (void)vpm_destroy_session_static_info(pMe);

   /* Destroy VP3 management */
   vpm_svc_vp3_deinit(&pMe->vp3_info);

   /* De-init command queue */
   elite_svc_deinit_cmd_queue(pMe->svc_handle.cmdQ);

   /* Channel destroy */
   qurt_elite_channel_destroy(&pMe->channel);

   /* Deregister from ADSP PM */
   (void)vccm_mmpm_deregister(VCCM_CLIENT_ID_CVP);

   return;
}

static void vpm_evaluate_sleep_possibility( voice_proc_mgr_t *pMe )
{
   uint32_t                session_idx;
   vpm_session_object_t    *p_session = NULL;
   vss_imvm_csm_cmd_request_vote_t params;
   bool_t sleep = TRUE;

   for ( session_idx = 0; session_idx < VPM_MAX_NUM_SESSIONS; session_idx++ ) 
   {
      p_session = pMe->sessions[session_idx];
      if ( NULL == p_session )
      {
         continue;
      }
      else if ( p_session->session_cmd_ctrl.is_prev_cmd_pending || p_session->session_rsp_ctrl.is_prev_cmd_rsp_pending )
      {
         sleep = FALSE;
      }
   }
   if ( ( TRUE == pMe->vpm_csm_vote )  && ( TRUE == sleep ) )
   {
     params.client = MVM_CSM_CLIENT_ENUM_VPM;
     params.vote = FALSE;
     mvm_call(VSS_IMVM_CSM_CMD_REQUEST_VOTE, &params, sizeof( params ) );
     pMe->vpm_csm_vote = FALSE;
   }
}

static ADSPResult voice_proc_mgr_process_channels(voice_proc_mgr_t *pMe, uint32_t channel_status)
{
   ADSPResult              result = ADSP_EOK;
   uint32_t                end_sig_bit;
   uint32_t                running_signal_mask;
   uint32_t                sig_bit_pos;
   vpm_session_object_t    *p_session = NULL;
   uint32_t                session_idx;
   vss_imvm_csm_cmd_request_vote_t params;

   if (!channel_status)
   {
      return ADSP_EOK;
   }

   /* Process service response queue first */
   if (channel_status & vpm_svc_rsp_q_mask)
   {
      result = voice_proc_mgr_process_svc_rsp_q(pMe);
   }

   /* Process service command Q */
   if (channel_status & vpm_svc_cmd_q_mask)
   {
      result = voice_proc_mgr_process_svc_cmd_q(pMe);

      /*Service is destroyed, return immediately */
      if (ADSP_ETERMINATED == result)
         return result;
   }

   /* Get the position of first MS bit set in updated channel status */
   end_sig_bit = (VPM_MAX_NUM_CHANNEL_BITS - s32_cl0_s32(channel_status));

   /* Process all the session's response Q first */
   for (sig_bit_pos = vpm_session_rsp_q_sig_start_bit_pos;
        sig_bit_pos < end_sig_bit;
        sig_bit_pos += 2)
   {
      running_signal_mask = (1 << sig_bit_pos);

      if (channel_status & running_signal_mask)
      {
         /* Validate the session index within supported range */
         if (VPM_MAX_NUM_SESSIONS <=
                (session_idx = vpm_map_channel_idx_to_session_idx(sig_bit_pos)))
         {
            break;
         }
         if ( FALSE == pMe->vpm_csm_vote )
         {
            params.client = MVM_CSM_CLIENT_ENUM_VPM;
            params.vote = TRUE;
            mvm_call(VSS_IMVM_CSM_CMD_REQUEST_VOTE, &params, sizeof( params ) );
            pMe->vpm_csm_vote = TRUE;
         }

         result = voice_proc_mgr_process_session_resp_q(pMe->sessions[session_idx]);
      }
   }

   /* Process all the session's command Q now */
   for (sig_bit_pos = vpm_session_cmd_q_sig_start_bit_pos;
        sig_bit_pos < end_sig_bit;
        sig_bit_pos += 2)
   {
      running_signal_mask = (1 << sig_bit_pos);

      if (channel_status & running_signal_mask)
      {
         /* Validate the session index within supported range */
         if (VPM_MAX_NUM_SESSIONS <=
                (session_idx = vpm_map_channel_idx_to_session_idx(sig_bit_pos)))
         {
            break;
         }

         if ( FALSE == pMe->vpm_csm_vote )
         {
            params.client = MVM_CSM_CLIENT_ENUM_VPM;
            params.vote = TRUE;
            mvm_call(VSS_IMVM_CSM_CMD_REQUEST_VOTE, &params, sizeof( params ) );
            pMe->vpm_csm_vote = TRUE;
         }
         result = voice_proc_mgr_process_session_cmd_q(pMe->sessions[session_idx]);
      }

   } /* End of for loop */
   vpm_evaluate_sleep_possibility( pMe );

   return result;
}

/* This function is the main work loop for the service. Commands from SC
  are handled with the highest priority, followed by MatrixMixer,PP and Decoder
  service.
 */
static int voice_proc_mgr_work_loop(void *pInstance)
{
   ADSPResult        result = ADSP_EOK;

   /* Service Instance */
   voice_proc_mgr_t  *pMe = (voice_proc_mgr_t *)pInstance;

   /* Set up mask for listening to the service cmd & rsp queues */
   pMe->curr_wait_mask |= vpm_svc_cmd_q_mask;
   pMe->curr_wait_mask |= vpm_svc_rsp_q_mask;

   /* Set the pending command to 0, no commands pending at svc start */
   pMe->pending_cmd_mask = 0;

   MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "Entering VPM workloop...");

   /* Enter forever loop */
   for (;;)
   {
      /* Block on any one or more of selected queues to get a msg */
      pMe->output_mask = qurt_elite_channel_wait(&(pMe->channel), pMe->curr_wait_mask);

      MSG_1(MSG_SSID_DFLT, DBG_LOW_PRIO, "Signal received = %lx !", pMe->output_mask);

      /* Exhaust all response queues first */
      uint32_t resp_q_status = 0;

      while ((resp_q_status = qurt_elite_channel_poll(&pMe->channel,
                                                      pMe->session_rsp_q_wait_mask)))
      {
         result = voice_proc_mgr_process_channels(pMe, resp_q_status);

         /* If Service is destroyed, return immediately */
         if (result == ADSP_ETERMINATED)
         {
            return ADSP_EOK;
         }
      }

      /* Process other queues and also any response queues now */
      pMe->output_mask = qurt_elite_channel_poll(&pMe->channel, pMe->curr_wait_mask);

      /* Handle any new command and/or previously pending commands */
      result = voice_proc_mgr_process_channels(pMe,
                                               pMe->output_mask | pMe->pending_cmd_mask);

      /* If Service is destroyed, return immediately */
      if (result == ADSP_ETERMINATED)
         return ADSP_EOK;

   } /* forever loop */

   return result;
}


/* Message Handler to ELITE_CMD_START_SERVICE in sys Q */
static ADSPResult  voice_proc_mgr_start_svc_handler(void *p_svc_instance,
                                                    elite_msg_any_t *p_msg_pkt)
{
   ADSPResult        result;
   voice_proc_mgr_t  *pMe = (voice_proc_mgr_t *)p_svc_instance;

   /* Look for AFE queue */
   if (NULL == qurt_elite_globalstate.pAfeStatSvcCmdQ)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "VoiceProcMgr: AFE svc cmdQ is NULL");

      voice_proc_mgr_destroy(pMe);
      return ADSP_EFAILED;
   }

   pMe->p_afe_svc_cmd_q = qurt_elite_globalstate.pAfeStatSvcCmdQ;

   /****************************************************************
    ** Register ISR call back function with APR
   ******************************************************************/

   elite_apr_handle_t   handle = 0;
   char                 vpm_my_dns[] = "qcom.audio.cvp";
   uint32_t             vpm_name_size = strlen(vpm_my_dns);
   uint16_t             ret_addr = 0;

   if (ADSP_FAILED(result = elite_apr_if_register_by_name(&handle,
                                                          &ret_addr,
                                                          vpm_my_dns,
                                                          vpm_name_size,
                                                          &voice_proc_mgr_apr_cb_func, NULL)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "VoiceProcMgr: Fail to register VPM with APR, result: 0x%8x", result);
      return result;
   }

   if (ADSP_FAILED(result = voice_proc_mgr_set_apr_handle(handle)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "VoiceProcMgr: Fail to set handle 0x%8x", result);
   }

   /* Save the APR handle and addr in service object */
   pMe->apr_handle = handle;
   pMe->apr_addr = ret_addr;

   /* Finish the msg */
   elite_msg_finish_msg(p_msg_pkt, ADSP_EOK);

   MSG(MSG_SSID_DFLT, DBG_MED_PRIO, "VoiceProcMgr: StartSvc  Leave");

   return result;

}

/* Message handler ELITE_CMD_DESTROY_SERVICE */
static ADSPResult voice_proc_mgr_destroy_svc_handler(void *p_svc_instance,
                                                     elite_msg_any_t *p_msg_pkt)
{
   voice_proc_mgr_t  *pMe = (voice_proc_mgr_t *)p_svc_instance;

   MSG(MSG_SSID_DFLT, DBG_MED_PRIO, "VoiceProcMgr: DestroySvc  Enter");

   /* Make a copy of msg because on destroy, pMe is freed */
   elite_msg_any_t msg = pMe->cmd_ctrl.msg_pkt;

   voice_proc_mgr_destroy(pMe);

   elite_msg_finish_msg(p_msg_pkt, ADSP_EOK);

   MSG(MSG_SSID_DFLT, DBG_MED_PRIO, "VoiceProcMgr: DestroySvc  Leave");

   /* Send ADSP_ETERMINATED so calling routine knows the
      destroyer has been invoked. */

   return ADSP_ETERMINATED;
}

/* =======================================================================
**                          Servce CmdQ/RspQ processing
** ======================================================================= */

/* Do not process Systerm Reserve Q. Just report error. */
static ADSPResult voice_proc_mgr_process_svc_rsp_q(void *p_svc_instance)
{
   ADSPResult           result = ADSP_EOK;
   elite_msg_any_t      msg_pkt;
   voice_proc_mgr_t     *pMe = (voice_proc_mgr_t *)p_svc_instance;

   MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "VoiceProcMgr: Processing service response queue");

   qurt_elite_queue_t *p_rsp_q = (qurt_elite_queue_t *)pMe->svc_rsp_q;

   if (ADSP_FAILED(result = qurt_elite_queue_pop_front(p_rsp_q, (uint64_t *)&msg_pkt)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "VoiceProcMgr: Failed to dequeue from event queue");
      return result;
   }

   /* Populate the msg payload */
   pMe->rsp_ctrl.msg_pkt = msg_pkt;

   /* Call the service response queue handler */
   return p_vpm_svc_rspq_hndlr[msg_pkt.unOpCode](pMe, &msg_pkt);
}

static ADSPResult  voice_proc_mgr_process_svc_cmd_q(void *p_svc_instance)
{
   ADSPResult           result;
   elite_msg_any_t      msg_pkt;
   voice_proc_mgr_t     *pMe = (voice_proc_mgr_t *)p_svc_instance;

   /* Pop the message buffer from the queue */
   if (ADSP_EOK != (result = qurt_elite_queue_pop_front(pMe->svc_handle.cmdQ,
                                                        (uint64_t *)&msg_pkt)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "voice_proc_mgr_process_svc_cmd_q():"
            " Failed to pop buf from svc cmdQ, result: 0x%lx",
            result);

      return result;
   }

   /* Populate the msg payload */
   pMe->cmd_ctrl.msg_pkt = msg_pkt;

   /* Call the system command queue handler */
   return p_vpm_svc_cmdq_hndlr[msg_pkt.unOpCode](pMe, &msg_pkt);
}

/* =======================================================================
**                          Session CmdQ and RespQ processing
** ======================================================================= */
static ADSPResult voice_proc_mgr_process_session_cmd_q(vpm_session_object_t *p_session)
{
   ADSPResult              result = ADSP_EOK;
   elite_msg_any_t         msg_pkt;
   vpm_cmd_ctrl_t         *p_cmd_ctl;


   /* Check if CMD received on a session that's not created yet. */
   if (NULL == p_session)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "voice_proc_mgr_process_session_cmd_q():"
          " Received CMD on invalid/NULL session");

      return ADSP_EFAILED;
   }

   p_cmd_ctl = &p_session->session_cmd_ctrl;

   /*  Pop the cmdQ only if there is no previous pending command */
   if (FALSE == p_cmd_ctl->is_prev_cmd_pending)
   {
      /* Pop the next msg buffer from the command queue */
      if (ADSP_EOK != (result = qurt_elite_queue_pop_front(p_cmd_ctl->p_cmd_queue,
                                                           (uint64_t *)&(msg_pkt))))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "voice_proc_mgr_process_session_cmd_q(): Failed to pop msg from cmdQ, result: 0x%lx",
               p_session->session_id,  result);

         return result;
      }

      p_cmd_ctl->msg_pkt = msg_pkt;
   }

   /* If previous command's resp is not pending, call the cmdQ handler */
   if (FALSE == p_session->session_rsp_ctrl.is_prev_cmd_rsp_pending)
   {
      result = p_vpm_session_cmdq_hndlr[p_cmd_ctl->msg_pkt.unOpCode]((void *)p_session, &p_cmd_ctl->msg_pkt);
   }

   return result;
}

static ADSPResult voice_proc_mgr_process_session_resp_q(vpm_session_object_t *p_session)
{
   ADSPResult                    result = ADSP_EOK;
   elite_msg_any_t               msg_pkt;
   vpm_cmd_rsp_ctrl_t            *p_rsp_ctl = &p_session->session_rsp_ctrl;

   /* Check if response is received on a session that's not created yet.
      This condition should not hit. */
   if (NULL == p_session)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "voice_proc_mgr_process_session_resp_q():"
          " Received response/event on invalid session");

      return ADSP_EFAILED;
   }


   /* Take next msg off the response Q */
   if (ADSP_EOK != (result = qurt_elite_queue_pop_front(p_rsp_ctl->p_rsp_queue,
                                                        (uint64_t *)&(msg_pkt))))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "VoiceProcMgr [%u]: Failed to pop buf from respQ",
            p_session->session_id);

      return result;
   }

   /* Save the response packet in session response control object */
   p_session->session_rsp_ctrl.msg_pkt = msg_pkt;

   return p_vpm_session_respq_hndlr[msg_pkt.unOpCode]((void *)p_session, &msg_pkt);
}
