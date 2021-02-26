/**========================================================================
 @file vpm_response_handler.cpp
 
 @brief This file contains handlers for respones from peer service for both
 the static service and dynamic session objects
 
 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_response_handler.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

#include "voice_proc_mgr.h"
#include "vpm_internal.h"
#include "adsp_vpm_api.h"
#include "VoiceMsgs.h"
#include "vss_private_if.h"
#include "VoiceLoggingUtils.h"

#include "vpm_svc_vp3_utils.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/** Process custom msg on respq  */
ADSPResult vpm_svc_cmdq_cust_msg_handler(void *p_svc_inst,
                                         elite_msg_any_t *p_msg_pkt)
{
   ADSPResult                 result = ADSP_EOK;
   voice_proc_mgr_t           *p_vpm_svc = (voice_proc_mgr_t *)p_svc_inst;
   elite_msg_custom_header_t  *p_msg_payload = (elite_msg_custom_header_t  *)p_msg_pkt->pPayload;

   MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "VoiceProcMgr: Received custom msg with opcode 0x%lx",
         p_msg_payload->unSecOpCode);


   switch (p_msg_payload->unSecOpCode)
   {
      /* No commands supported */
      default:
      {
         result = ADSP_EUNSUPPORTED;
         elite_msg_ack_msg(p_msg_pkt, result);
         break;
      }
   }

   return result;
}
/** Process custom msg on respq  */
ADSPResult vpm_svc_cmd_rsp_handler(void *p_vpm_instance,
                                   elite_msg_any_t *p_msg_pkt)
{
   ADSPResult                 result = ADSP_EOK;
   elite_apr_packet_t         *p_rsp_apr_packet;
   elite_apr_packet_t         *p_cmd_apr_packet;
   aprv2_ibasic_rsp_result_t  *p_apr_rsp_result;
   voice_proc_mgr_t           *pMe = (voice_proc_mgr_t *)p_vpm_instance;
   uint32_t                   apr_rsp_status;

   MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_svc_cmd_rsp_handler(): Svc rspQ received response/event opcode: 0x%lx",
         p_msg_pkt->unOpCode);

   switch (p_msg_pkt->unOpCode)
   {
      case ELITE_APR_PACKET:
      {
         p_rsp_apr_packet = (elite_apr_packet_t *)p_msg_pkt->pPayload;

         switch (elite_apr_if_get_opcode(p_rsp_apr_packet))
         {
            case APRV2_IBASIC_RSP_RESULT:
            {
               /* Get the APR response result */
               p_apr_rsp_result = (aprv2_ibasic_rsp_result_t *)elite_apr_if_get_payload_ptr(p_rsp_apr_packet);

               /* Increment the response received counter */
               pMe->rsp_ctrl.num_rsp_received++;

               /* Get the basic response status */
               if (APR_EOK != (apr_rsp_status = p_apr_rsp_result->status))
               {
                  pMe->rsp_ctrl.num_rsp_failed++;
               }

               MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "vpm_svc_cmd_rsp_handler(): Received response for APR cmd opcode: 0x%lx, rsp_status: 0x%lx",
                     p_apr_rsp_result->opcode, apr_rsp_status);

               /* Free up the response APR packet */
               elite_apr_if_free(pMe->apr_handle, p_rsp_apr_packet);

               /* If all the responses received for num commands issued by static service */
               if (pMe->rsp_ctrl.num_rsp_received == pMe->rsp_ctrl.num_cmd_issued)
               {
                  /* Add back the service cmdQ to the current signal wait mask */
                  pMe->curr_wait_mask |= pMe->svc_cmd_q_wait_mask;

                  /* If all the responses failed, return error */
                  if (pMe->rsp_ctrl.num_rsp_received == pMe->rsp_ctrl.num_rsp_failed)
                  {
                     result = ADSP_EFAILED;
                  }

                  /* Get the original command packet for which response is received */
                  p_cmd_apr_packet = (elite_apr_packet_t *)pMe->cmd_ctrl.msg_pkt.pPayload;

                  /* End the APR command which initiated the response handling */
                  elite_apr_if_end_cmd(pMe->apr_handle, p_cmd_apr_packet, result);
               }
               break;
            } /* End of case APRV2_IBASIC_RSP_RESULT: */

         } /* End of switch (elite_apr_if_get_opcode(p_apr_packet)) */

      } /* End of case ELITE_APR_PACKET: */

   } /* switch (p_msg_pkt->unOpCode) */

   return result;
}

void vpm_check_and_update_rsp_status(vpm_session_object_t *p_session,
                                     uint32_t rsp_status)
{
   /* Increment the response received counter */
   p_session->session_rsp_ctrl.num_rsp_received++;

   /* Aggregate the response result */
   if (ADSP_EUNSUPPORTED != rsp_status)
   {
      p_session->session_rsp_ctrl.status |= rsp_status;
   }
   else
   {
      p_session->session_rsp_ctrl.num_unsupported_ecode++;
   }

   /* Increment the failed response counter for any error code */
   if (ADSP_EOK != rsp_status)
   {
      /* Increment the failed response counter */
      p_session->session_rsp_ctrl.num_rsp_failed++;
   }

   /* If number of response rcvd equal to number of cmds issued,
      set the response pending flag to FALSE */
   if (p_session->session_rsp_ctrl.num_rsp_received ==
          p_session->session_rsp_ctrl.num_cmd_issued)
   {
      /* Clear the response pending flag */
      p_session->session_rsp_ctrl.is_prev_cmd_rsp_pending = FALSE;

      if (p_session->session_rsp_ctrl.num_unsupported_ecode ==
                  p_session->session_rsp_ctrl.num_cmd_issued)
      {
         /* If all the response are with un-supported error */
         p_session->session_rsp_ctrl.status = ADSP_EUNSUPPORTED;
      }

      /* If all the responses received, call the response handler if available */
      if (NULL != p_session->session_rsp_ctrl.p_rsp_hndlr_fn)
      {
         p_session->session_rsp_ctrl.p_rsp_hndlr_fn((void *)p_session);
      }

      /* Free up the allocated ACK packet, if any */
      if (NULL != p_session->session_rsp_ctrl.p_ack_payload)
      {
         /* Check if the response payload is of APR type */
         if (TRUE == p_session->session_rsp_ctrl.is_ack_payload_type_apr)
         {
            elite_apr_if_free(p_session->self_apr_handle,
                              (elite_apr_packet_t *)p_session->session_rsp_ctrl.p_ack_payload);
         }
         else
         {
            /* Normal malloc'd payload */
            qurt_elite_memory_free(p_session->session_rsp_ctrl.p_ack_payload);
         }

         p_session->session_rsp_ctrl.p_ack_payload = NULL;
      }
   }

   return;
}

ADSPResult vpm_get_delay_cmd_rsp_handler(void *context_ptr)
{
   ADSPResult                    result = ADSP_EOK;
   vpm_dyn_svc_info_t            *p_dyn_svc;
   vpm_get_delay_ack_t           *delay_ptr;
   uint32_t                      tx_afe_delay, rx_afe_delay;
   elite_msg_custom_delay_type   *p_msg_payload;
   aprv2_ibasic_rsp_result_t     rsp_payload;
   uint32_t                      opcode;
   void                          *p_apr_msg_payload;
   uint32_t                      apr_payload_size;
   vpm_session_object_t          *p_session = (vpm_session_object_t *)context_ptr;
   int32_t                       rc;

   p_dyn_svc = &p_session->dyn_svc_info;

   elite_apr_packet_t            *p_apr_packet =\
      (elite_apr_packet_t *)p_session->session_cmd_ctrl.msg_pkt.pPayload;

   p_msg_payload = (elite_msg_custom_delay_type *)p_session->session_rsp_ctrl.msg_pkt.pPayload;

   delay_ptr = (vpm_get_delay_ack_t *)p_msg_payload->delay_ptr;

   vss_ivocproc_rsp_get_avsync_delays_t vp_avsync_delay;

   /* Clear the response payload */
   memset(&vp_avsync_delay, 0, sizeof(vp_avsync_delay));

   /* If the response failed, send basic response */
   if (ADSP_EOK != (result = p_session->session_rsp_ctrl.status))
   {
      rsp_payload.opcode = elite_apr_if_get_opcode(p_apr_packet);
      rsp_payload.status = result;

      opcode = APRV2_IBASIC_RSP_RESULT;
      apr_payload_size = sizeof(rsp_payload);
      p_apr_msg_payload = (void *)&rsp_payload;
   }
   else
   {
      /* Retrieve the delay Rx/Tx delay information from ACK payload */
      if (p_dyn_svc->tx)
      {
         /* Get the algo delay from vpTx */
         vp_avsync_delay.vp_tx_algorithmic_delay = delay_ptr->vptx_delay;

         /* Get the AFE Tx data path and h/w delay */
         tx_afe_delay = p_dyn_svc->tx_afe_delay + p_dyn_svc->tx_afe_hw_delay;

         MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
               "vpm_get_delay_cmd_rsp_handler(): vptx_algo_delay: %lu, afe_tx_port_and_hw_delay: %lu",
               vp_avsync_delay.vp_tx_algorithmic_delay, tx_afe_delay);

         /* Aggreated the overall Tx delay */
         vp_avsync_delay.vp_tx_algorithmic_delay += tx_afe_delay;
      }

      if (p_dyn_svc->rx)
      {
         /* Get the algo delay from vpRx */
         vp_avsync_delay.vp_rx_algorithmic_delay = delay_ptr->vprx_delay;

         /* Get the AFE Rx data path and h/w delay */
         rx_afe_delay = p_dyn_svc->rx_afe_delay + p_dyn_svc->rx_afe_hw_delay;

         MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
               "vpm_get_delay_cmd_rsp_handler(): vprx_algo_delay: %lu, afe_rx_port_and_hw_delay: %lu",
               vp_avsync_delay.vp_rx_algorithmic_delay, rx_afe_delay);

         /* Aggreated the overall Rx delay */
         vp_avsync_delay.vp_rx_algorithmic_delay += rx_afe_delay;
      }

      opcode = VSS_IVOCPROC_RSP_GET_AVSYNC_DELAYS;
      apr_payload_size = sizeof(vp_avsync_delay);
      p_apr_msg_payload = (void *)&vp_avsync_delay;
   }

   /* Send the APR command response back to MVM */
   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(p_session->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_apr_packet),
                                                    elite_apr_if_get_dst_port(p_apr_packet),
                                                    elite_apr_if_get_src_addr(p_apr_packet),
                                                    elite_apr_if_get_src_port(p_apr_packet),
                                                    elite_apr_if_get_client_token(p_apr_packet),
                                                    opcode,
                                                    p_apr_msg_payload,
                                                    apr_payload_size)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_get_delay_cmd_rsp_handler(): Failed to send Get avsync delay ack to MVM, result: 0x%lx",
            rc);
   }
   else
   {
      MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
          "vpm_get_delay_cmd_rsp_handler(): Succesfully sent get avsync delay ack to MVM");
   }

   return result;
}

ADSPResult vpm_get_kpps_cmd_rsp_handler(void *context_ptr)
{
   ADSPResult                    result = ADSP_EOK;
   vpm_get_kpps_ack_t            *p_get_kpps_ack;
   elite_msg_custom_kpps_type    *p_get_kpps_cmd;
   vpm_session_object_t          *session_obj;

   session_obj = (vpm_session_object_t *)context_ptr;

   p_get_kpps_cmd = (elite_msg_custom_kpps_type *)session_obj->session_rsp_ctrl.msg_pkt.pPayload;

   p_get_kpps_ack = (vpm_get_kpps_ack_t *)p_get_kpps_cmd->pnKpps;

   if (ADSP_EOK == (result = session_obj->session_rsp_ctrl.status))
   {
      if (session_obj->kpps_info.vp_tx != p_get_kpps_ack->vptx_kpps)
      {
         session_obj->kpps_info.vp_tx = p_get_kpps_ack->vptx_kpps;
         session_obj->is_kpps_changed = TRUE;
      }

      if (session_obj->kpps_info.vp_rx != p_get_kpps_ack->vprx_kpps)
      {
         session_obj->kpps_info.vp_rx = p_get_kpps_ack->vprx_kpps;
         session_obj->is_kpps_changed = TRUE;
      }
      session_obj->vptx_frame_size_us = p_get_kpps_ack->vptx_frame_size_us;

      MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_kpps_cmd_rsp_handler(): vptx_kpps = %d, vprx_kpps = %d, "
            "vptx_frame_size = %d us", p_get_kpps_ack->vptx_kpps, 
            p_get_kpps_ack->vprx_kpps, p_get_kpps_ack->vptx_frame_size_us);

   }
   else
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_get_kpps_cmd_rsp_handler(): Get KPPS cmd ack failed, result: 0x%lx", result);
   }

   return ADSP_EOK;
}

ADSPResult vpm_get_param_cmd_rsp_fwd(void *context_ptr)
{
   ADSPResult                    result = ADSP_EOK;;
   uint32_t                      payload_len;
   elite_apr_packet_t            *p_orig_apr_pkt;
   vpm_session_object_t          *session_obj;
   elite_apr_packet_t            *p_rsp_pkt;
   uint8_t                       *payload;
   int32_t                       rc;

   if (!context_ptr)
   {
      return ADSP_EFAILED;
   }

   /* Get the pointer to session object */
   session_obj = (vpm_session_object_t *)context_ptr;

   /* Get the original command APR packet */
   p_orig_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;


   /* Get the pointer to reponse APR packet from stream service */
   p_rsp_pkt = (elite_apr_packet_t *)session_obj->session_rsp_ctrl.msg_pkt.pPayload;

   /* If the get param is for GET_UI_PROPERTY, translate the response opcode to RSP_GET_UI_PROP */
   if (elite_apr_if_get_opcode(p_orig_apr_pkt) == VSS_ICOMMON_CMD_GET_UI_PROPERTY)
   {
      p_rsp_pkt->opcode = VSS_ICOMMON_RSP_GET_UI_PROPERTY;
   }

   /* Get response payload pointer and size */
   payload = (uint8_t *)elite_apr_if_get_payload_ptr(p_rsp_pkt);
   payload_len = elite_apr_if_get_payload_size(p_rsp_pkt);

   if (payload_len >= 16)
   {
      MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_param_cmd_rsp_fwd(): "
            "Payload[0-2]=0x%08x, 0x%08x, 0x%08x",
            *(uint32_t *)(payload + sizeof(voice_param_data_t)),
            *(uint32_t *)(payload + sizeof(voice_param_data_t) + sizeof(uint32_t)),
            *(uint32_t *)(payload + sizeof(voice_param_data_t) + (2 * sizeof(uint32_t))));

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_param_cmd_rsp_fwd(): "
            "Payload[3]=0x%08x",
            *(uint32_t *)(payload + sizeof(voice_param_data_t) + (3 * sizeof(uint32_t))));
   }
   else if (payload_len >= 4)
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_param_cmd_rsp_fwd(): "
            "Payload[0]=0x%08x",
            *(uint32_t *)(payload + sizeof(voice_param_data_t)));
   }

   MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
         "CVD_CAL_MSG_PARAM: vpm_get_param_cmd_rsp_fwd(): "
         "Get Param Response Token=%d", elite_apr_if_get_client_token(p_orig_apr_pkt));

   /* Send the ACK back to HLOS */
   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_orig_apr_pkt),
                                                    elite_apr_if_get_dst_port(p_orig_apr_pkt),
                                                    elite_apr_if_get_src_addr(p_orig_apr_pkt),
                                                    elite_apr_if_get_src_port(p_orig_apr_pkt),
                                                    elite_apr_if_get_client_token(p_orig_apr_pkt),
                                                    elite_apr_if_get_opcode(p_rsp_pkt),
                                                    elite_apr_if_get_payload_ptr(p_rsp_pkt),
                                                    elite_apr_if_get_payload_size(p_rsp_pkt))))
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_param_cmd_rsp_fwd(): Failed to send get param cmd ACK to client, result: 0x%lx",
            rc);

      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult vpm_get_param_v3_cmd_rsp_fwd(void *context_ptr)
{
   ADSPResult                    result = ADSP_EOK;;
   uint32_t                      payload_len;
   elite_apr_packet_t            *p_orig_apr_pkt;
   vpm_session_object_t          *session_obj;
   elite_apr_packet_t            *p_rsp_pkt;
   uint8_t                       *payload;
   int32_t                       rc;

   if (!context_ptr)
   {
      return ADSP_EFAILED;
   }

   /* Get the pointer to session object */
   session_obj = (vpm_session_object_t *)context_ptr;

   /* Get the original command APR packet */
   p_orig_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;


   /* Get the pointer to reponse APR packet from stream service */
   p_rsp_pkt = (elite_apr_packet_t *)session_obj->session_rsp_ctrl.msg_pkt.pPayload;

   /* If the get param is for GET_UI_PROPERTY, translate the response opcode to RSP_GET_UI_PROP */
   if (elite_apr_if_get_opcode(p_orig_apr_pkt) == VSS_ICOMMON_CMD_GET_UI_PROPERTY_V2)
   {
      p_rsp_pkt->opcode = VSS_ICOMMON_RSP_GET_UI_PROPERTY_V2;
   }
   else if (elite_apr_if_get_opcode(p_orig_apr_pkt) == VSS_ICOMMON_CMD_GET_PARAM_V3)
   {
      p_rsp_pkt->opcode = VSS_ICOMMON_RSP_GET_PARAM_V3;
   }

   /* Get response payload pointer and size */
   payload = (uint8_t *)elite_apr_if_get_payload_ptr(p_rsp_pkt);
   payload_len = elite_apr_if_get_payload_size(p_rsp_pkt);

   if (payload_len >= 16)
   {
      MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_param_v3_cmd_rsp_fwd(): "
            "Payload[0-2]=0x%08x, 0x%08x, 0x%08x",
            *(uint32_t *)(payload + sizeof(voice_param_data_v2_t)),
            *(uint32_t *)(payload + sizeof(voice_param_data_v2_t) + sizeof(uint32_t)),
            *(uint32_t *)(payload + sizeof(voice_param_data_v2_t) + (2 * sizeof(uint32_t))));

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_param_v3_cmd_rsp_fwd(): "
            "Payload[3]=0x%08x",
            *(uint32_t *)(payload + sizeof(voice_param_data_v2_t) + (3 * sizeof(uint32_t))));
   }
   else if (payload_len >= 4)
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_param_v3_cmd_rsp_fwd(): "
            "Payload[0]=0x%08x",
            *(uint32_t *)(payload + sizeof(voice_param_data_v2_t)));
   }

   MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
         "CVD_CAL_MSG_PARAM: vpm_get_param_v3_cmd_rsp_fwd(): "
         "Get Param Response Token=%d", elite_apr_if_get_client_token(p_orig_apr_pkt));

   /* Send the ACK back to HLOS */
   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_orig_apr_pkt),
                                                    elite_apr_if_get_dst_port(p_orig_apr_pkt),
                                                    elite_apr_if_get_src_addr(p_orig_apr_pkt),
                                                    elite_apr_if_get_src_port(p_orig_apr_pkt),
                                                    elite_apr_if_get_client_token(p_orig_apr_pkt),
                                                    elite_apr_if_get_opcode(p_rsp_pkt),
                                                    elite_apr_if_get_payload_ptr(p_rsp_pkt),
                                                    elite_apr_if_get_payload_size(p_rsp_pkt))))
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_param_v3_cmd_rsp_fwd(): Failed to send get param cmd ACK to client, result: 0x%lx",
            rc);

      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult vpm_get_param_cmd_rsp_handler(void *context_ptr)
{
   ADSPResult                    result = ADSP_EOK;
   uint32_t                      payload_len;
   elite_apr_packet_t            *p_apr_pkt;
   vpm_session_object_t          *session_obj;
   vss_icommon_rsp_get_param_t   get_param_rsp_payload;
   aprv2_ibasic_rsp_result_t     basic_rsp_payload;
   void                          *p_apr_rsp_payload;
   uint32_t                      apr_rsp_payload_size;
   uint32_t                      opcode;
   vss_icommon_rsp_get_param_t   *p_get_param_ack;
   int32_t                       rc;
   vss_icommon_cmd_get_param_v2_t *payload;
   cvd_virt_addr_t               param_data_virt_addr;
   int8_t                        *p_log_buf = NULL;
   voice_param_data_t            *param_data = NULL;
   if (!context_ptr)
   {
      return ADSP_EFAILED;
   }

   session_obj = (vpm_session_object_t *)context_ptr;

   /* Get the original command payload */
   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   payload = (vss_icommon_cmd_get_param_v2_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

   /* Check if at least 1 get param cmd returned SUCCESS or
      all the get params returned EUNSUPPORTED error */
   if ((ADSP_EOK == session_obj->session_rsp_ctrl.status) ||
       (ADSP_EUNSUPPORTED == session_obj->session_rsp_ctrl.status))
   {
      /* Set the client ack opcode */
      opcode = VSS_ICOMMON_RSP_GET_PARAM;

      /* If in band */
      if (!payload->mem_handle)
      {
         /* Get the pointer to in-band response payload */
         p_get_param_ack = (vss_icommon_rsp_get_param_t *)session_obj->session_rsp_ctrl.p_ack_payload;

         /* Update the get param aggreated response */
         p_get_param_ack->status = session_obj->session_rsp_ctrl.status;

         p_apr_rsp_payload = (void *)p_get_param_ack;
         apr_rsp_payload_size = payload->mem_size + sizeof(vss_icommon_rsp_get_param_t);

         p_log_buf = ((int8_t *)p_get_param_ack) + sizeof(vss_icommon_rsp_get_param_t);
      }
      else /* out band */
      {
         /* Get the pointer to out-band response payload */
         p_get_param_ack = &get_param_rsp_payload;

         /* Set the get param aggregated response */
         p_get_param_ack->status = session_obj->session_rsp_ctrl.status;

         p_apr_rsp_payload = (void *)p_get_param_ack;

         apr_rsp_payload_size = sizeof(vss_icommon_rsp_get_param_t);

         rc = cvd_mem_mapper_get_virtual_addr_v2(payload->mem_handle, payload->mem_address,
                                                 &param_data_virt_addr);

         /* Flush the cache for outband payload */
         rc = cvd_mem_mapper_cache_flush_v2(&param_data_virt_addr, payload->mem_size);

         p_log_buf = (int8_t *)param_data_virt_addr.ptr;
      }
      param_data = ( voice_param_data_t* ) p_log_buf;

#if defined(__qdsp6__) && !defined(SIM)
      if (p_log_buf)
      {
         int8_t *bufptr[4] = { p_log_buf, NULL, NULL, NULL };

         /* In get param ack, dynamic service returns the size of the param ID
            requested. For logging, complete payload size including voice param
            data header needs to be considered */

         voice_log_buffer(bufptr,
                          VOICE_LOG_CHAN_VPM_GET_PARAM,
                          session_obj->self_apr_port, /* Session Id */
                          qurt_elite_timer_get_time(),
                          VOICE_LOG_DATA_FORMAT_PCM_MONO,
                          1,   /* dummy value */
                          param_data->param_size + sizeof(voice_param_data_t), 
                          NULL);
      }
#endif

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_param_cmd_rsp_handler(): "
            "Get Param Response Token: %lu", elite_apr_if_get_client_token(p_apr_pkt));
   }
   else /* Any other error */
   {
      /* Send basic response */
      opcode = APRV2_IBASIC_RSP_RESULT;
      p_apr_rsp_payload = &basic_rsp_payload;
      apr_rsp_payload_size = sizeof(aprv2_ibasic_rsp_result_t);

      /* Get the original get param v2 command opcode */
      basic_rsp_payload.opcode = elite_apr_if_get_opcode(p_apr_pkt);
      basic_rsp_payload.status = ADSP_EFAILED;

      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_param_cmd_rsp_handler(): Dyn svcs ack'd error: 0x%lx",
            session_obj->session_rsp_ctrl.status);
   }

   /* Send the ACK back to HLOS */
   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_apr_pkt),
                                                    elite_apr_if_get_dst_port(p_apr_pkt),
                                                    elite_apr_if_get_src_addr(p_apr_pkt),
                                                    elite_apr_if_get_src_port(p_apr_pkt),
                                                    elite_apr_if_get_client_token(p_apr_pkt),
                                                    opcode,
                                                    p_apr_rsp_payload,
                                                    apr_rsp_payload_size)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_param_cmd_rsp_handler(): Failed to send get param cmd ACK to client, result: 0x%lx",
            rc);

      result = ADSP_EFAILED;
   }

   return result;
}


ADSPResult vpm_get_ui_prop_cmd_rsp_handler(void *context_ptr)
{
   ADSPResult                             result = ADSP_EOK;
   uint32_t                               payload_len;
   elite_apr_packet_t                     *p_apr_pkt;
   vpm_session_object_t                   *session_obj;
   vss_icommon_rsp_get_ui_property_t      get_param_rsp_payload;
   aprv2_ibasic_rsp_result_t              basic_rsp_payload;
   void                                   *p_apr_rsp_payload = NULL;
   uint32_t                               apr_rsp_payload_size = 0;
   uint32_t                               opcode;
   vss_icommon_rsp_get_ui_property_t      *p_get_param_ack;
   int32_t                                rc;
   vss_icommon_cmd_get_ui_property_t      *payload;
   cvd_virt_addr_t                        param_data_virt_addr;
   int8_t                                 *p_log_buf = NULL;
   voice_param_data_t                     *param_data = NULL;
   if (!context_ptr)
   {
      return ADSP_EFAILED;
   }

   session_obj = (vpm_session_object_t *)context_ptr;

   /* Get the original command payload */
   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   payload = (vss_icommon_cmd_get_ui_property_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

   /* Check if at least 1 get param cmd returned SUCCESS or
      all the get params returned EUNSUPPORTED error */
   if ((ADSP_EOK == session_obj->session_rsp_ctrl.status) ||
       (ADSP_EUNSUPPORTED == session_obj->session_rsp_ctrl.status))
   {
      /* Set the client ack opcode */
      opcode = VSS_ICOMMON_RSP_GET_UI_PROPERTY;
      {
         /* Get the pointer to in-band response payload */
         p_get_param_ack = (vss_icommon_rsp_get_ui_property_t *)session_obj->session_rsp_ctrl.p_ack_payload;

         if ( NULL != p_get_param_ack )
         {
            /* Update the get param aggreated response */
            p_get_param_ack->status = session_obj->session_rsp_ctrl.status;

            p_apr_rsp_payload = (void *)p_get_param_ack;
            apr_rsp_payload_size = payload->param_size + sizeof(vss_icommon_rsp_get_ui_property_t);

            p_log_buf = ((int8_t *)p_get_param_ack) + sizeof(vss_icommon_rsp_get_ui_property_t);
         }
      }
      param_data = ( voice_param_data_t* )p_log_buf;
#if defined(__qdsp6__) && !defined(SIM)
      if (p_log_buf)
      {
         int8_t *bufptr[4] = { p_log_buf, NULL, NULL, NULL };

         /* In get param ack, dynamic service returns the size of the param ID
            requested. For logging, complete payload size including voice param
            data header needs to be considered */

         voice_log_buffer(bufptr,
                          VOICE_LOG_CHAN_VPM_GET_PARAM,
                          session_obj->self_apr_port, /* Session Id */
                          qurt_elite_timer_get_time(),
                          VOICE_LOG_DATA_FORMAT_PCM_MONO,
                          1,   /* dummy value */
                          (param_data->param_size + sizeof(voice_param_data_t)), 
                          NULL);
      }
#endif

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_ui_prop_cmd_rsp_handler(): "
            "Get Param Response Token: %lu", elite_apr_if_get_client_token(p_apr_pkt));
   }
   else /* Any other error */
   {
      /* Send basic response */
      opcode = APRV2_IBASIC_RSP_RESULT;
      p_apr_rsp_payload = &basic_rsp_payload;
      apr_rsp_payload_size = sizeof(aprv2_ibasic_rsp_result_t);

      /* Get the original get param v2 command opcode */
      basic_rsp_payload.opcode = elite_apr_if_get_opcode(p_apr_pkt);
      basic_rsp_payload.status = ADSP_EFAILED;

      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_ui_prop_cmd_rsp_handler(): Dyn svcs ack'd error: 0x%lx",
            session_obj->session_rsp_ctrl.status);
   }

   /* Send the ACK back to HLOS */
   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_apr_pkt),
                                                    elite_apr_if_get_dst_port(p_apr_pkt),
                                                    elite_apr_if_get_src_addr(p_apr_pkt),
                                                    elite_apr_if_get_src_port(p_apr_pkt),
                                                    elite_apr_if_get_client_token(p_apr_pkt),
                                                    opcode,
                                                    p_apr_rsp_payload,
                                                    apr_rsp_payload_size)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_ui_prop_cmd_rsp_handler(): Failed to send get param cmd ACK to client, result: 0x%lx",
            rc);

      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult vpm_get_ui_prop_v2_cmd_rsp_handler(void *context_ptr)
{
   ADSPResult                    result = ADSP_EOK;
   uint32_t                      payload_len;
   elite_apr_packet_t            *p_apr_pkt;
   vpm_session_object_t          *session_obj;
   vss_icommon_rsp_get_ui_property_v2_t   get_param_rsp_payload;
   aprv2_ibasic_rsp_result_t     basic_rsp_payload;
   void                          *p_apr_rsp_payload = NULL;
   uint32_t                      apr_rsp_payload_size = 0;
   uint32_t                      opcode;
   vss_icommon_rsp_get_ui_property_v2_t   *p_get_param_ack;
   int32_t                       rc;
   vss_icommon_cmd_get_ui_property_v2_t *payload;
   cvd_virt_addr_t               param_data_virt_addr;
   int8_t                        *p_log_buf = NULL;
   voice_param_data_v2_t         *param_data = NULL;

   if (!context_ptr)
   {
      return ADSP_EFAILED;
   }

   session_obj = (vpm_session_object_t *)context_ptr;

   /* Get the original command payload */
   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   payload = (vss_icommon_cmd_get_ui_property_v2_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

   /* Check if at least 1 get param cmd returned SUCCESS or
      all the get params returned EUNSUPPORTED error */
   if ((ADSP_EOK == session_obj->session_rsp_ctrl.status) ||
       (ADSP_EUNSUPPORTED == session_obj->session_rsp_ctrl.status))
   {
      /* Set the client ack opcode */
      opcode = VSS_ICOMMON_RSP_GET_UI_PROPERTY_V2;
      {
         /* Get the pointer to in-band response payload */
         p_get_param_ack = (vss_icommon_rsp_get_ui_property_v2_t *)session_obj->session_rsp_ctrl.p_ack_payload;
         if ( NULL != p_get_param_ack ) 
         {
            /* Update the get param aggreated response */
            p_get_param_ack->status = session_obj->session_rsp_ctrl.status;

            p_apr_rsp_payload = (void *)p_get_param_ack;
            apr_rsp_payload_size = payload->param_size + sizeof(vss_icommon_rsp_get_ui_property_v2_t);

            p_log_buf = ((int8_t *)p_get_param_ack) + sizeof(vss_icommon_rsp_get_ui_property_v2_t);
         }
      }
      param_data = ( voice_param_data_v2_t* )p_log_buf;

#if defined(__qdsp6__) && !defined(SIM)
      if (p_log_buf)
      {
         int8_t *bufptr[4] = { p_log_buf, NULL, NULL, NULL };

         /* In get param ack, dynamic service returns the size of the param ID
            requested. For logging, complete payload size including voice param
            data header needs to be considered */

         voice_log_buffer(bufptr,
                          VOICE_LOG_CHAN_VPM_GET_PARAM_V2,
                          session_obj->self_apr_port, /* Session Id */
                          qurt_elite_timer_get_time(),
                          VOICE_LOG_DATA_FORMAT_PCM_MONO,
                          1,   /* dummy value */
                          (param_data->param_size + sizeof(voice_param_data_v2_t)), 
                          NULL);
      }
#endif

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_ui_prop_v2_cmd_rsp_handler(): "
            "Get Param Response Token: %lu", elite_apr_if_get_client_token(p_apr_pkt));
   }
   else /* Any other error */
   {
      /* Send basic response */
      opcode = APRV2_IBASIC_RSP_RESULT;
      p_apr_rsp_payload = &basic_rsp_payload;
      apr_rsp_payload_size = sizeof(aprv2_ibasic_rsp_result_t);

      /* Get the original get param v2 command opcode */
      basic_rsp_payload.opcode = elite_apr_if_get_opcode(p_apr_pkt);
      basic_rsp_payload.status = ADSP_EFAILED;

      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_ui_prop_v2_cmd_rsp_handler(): Dyn svcs ack'd error: 0x%lx",
            session_obj->session_rsp_ctrl.status);
   }

   /* Send the ACK back to HLOS */
   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_apr_pkt),
                                                    elite_apr_if_get_dst_port(p_apr_pkt),
                                                    elite_apr_if_get_src_addr(p_apr_pkt),
                                                    elite_apr_if_get_src_port(p_apr_pkt),
                                                    elite_apr_if_get_client_token(p_apr_pkt),
                                                    opcode,
                                                    p_apr_rsp_payload,
                                                    apr_rsp_payload_size)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_ui_prop_v2_cmd_rsp_handler(): Failed to send get param cmd ACK to client, result: 0x%lx",
            rc);

      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult vpm_get_param_v3_cmd_rsp_handler(void *context_ptr)
{
   ADSPResult                     result = ADSP_EOK;
   uint32_t                       payload_len;
   elite_apr_packet_t             *p_apr_pkt;
   vpm_session_object_t           *session_obj;
   vss_icommon_rsp_get_param_v3_t get_param_rsp_payload;
   aprv2_ibasic_rsp_result_t      basic_rsp_payload;
   void                           *p_apr_rsp_payload;
   uint32_t                       apr_rsp_payload_size;
   uint32_t                       opcode;
   vss_icommon_rsp_get_param_v3_t    *p_get_param_ack;
   int32_t                        rc;
   vss_icommon_cmd_get_param_v3_t *payload;
   cvd_virt_addr_t                param_data_virt_addr;
   int8_t                         *p_log_buf = NULL;
   voice_param_data_v2_t            *param_data = NULL;

   if (!context_ptr)
   {
      return ADSP_EFAILED;
   }

   session_obj = (vpm_session_object_t *)context_ptr;

   /* Get the original command payload */
   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   payload = (vss_icommon_cmd_get_param_v3_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

   /* Check if at least 1 get param cmd returned SUCCESS or
      all the get params returned EUNSUPPORTED error */
   if ((ADSP_EOK == session_obj->session_rsp_ctrl.status) ||
       (ADSP_EUNSUPPORTED == session_obj->session_rsp_ctrl.status))
   {
      /* Set the client ack opcode */
      opcode = VSS_ICOMMON_RSP_GET_PARAM_V3;

      /* If in band */
      if (!payload->mem_handle)
      {
         /* Get the pointer to in-band response payload */
         p_get_param_ack = (vss_icommon_rsp_get_param_v3_t *)session_obj->session_rsp_ctrl.p_ack_payload;

         /* Update the get param aggreated response */
         p_get_param_ack->status = session_obj->session_rsp_ctrl.status;

         p_apr_rsp_payload = (void *)p_get_param_ack;
         apr_rsp_payload_size = payload->mem_size + sizeof(vss_icommon_rsp_get_param_v3_t);

         p_log_buf = ((int8_t *)p_get_param_ack) + sizeof(vss_icommon_rsp_get_param_v3_t);
      }
      else /* out band */
      {
         /* Get the pointer to out-band response payload */
         p_get_param_ack = &get_param_rsp_payload;

         /* Set the get param aggregated response */
         p_get_param_ack->status = session_obj->session_rsp_ctrl.status;

         p_apr_rsp_payload = (void *)p_get_param_ack;

         apr_rsp_payload_size = sizeof(vss_icommon_rsp_get_param_v3_t);

         rc = cvd_mem_mapper_get_virtual_addr_v2(payload->mem_handle, payload->mem_address,
                                                 &param_data_virt_addr);

         /* Flush the cache for outband payload */
         rc = cvd_mem_mapper_cache_flush_v2(&param_data_virt_addr, payload->mem_size);

         p_log_buf = (int8_t *)param_data_virt_addr.ptr;
      }
      param_data = ( voice_param_data_v2_t * ) p_log_buf;

#if defined(__qdsp6__) && !defined(SIM)
      if (p_log_buf)
      {
         int8_t *bufptr[4] = { p_log_buf, NULL, NULL, NULL };

         /* In get param ack, dynamic service returns the size of the param ID
            requested. For logging, complete payload size including voice param
            data header needs to be considered */

         voice_log_buffer(bufptr,
                          VOICE_LOG_CHAN_VPM_GET_PARAM_V2,
                          session_obj->self_apr_port, /* Session Id */
                          qurt_elite_timer_get_time(),
                          VOICE_LOG_DATA_FORMAT_PCM_MONO,
                          1,   /* dummy value */
                          param_data->param_size + sizeof(voice_param_data_v2_t),
                          NULL);
      }
#endif

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_param_v3_cmd_rsp_handler(): "
            "Get Param Response Token: %lu", elite_apr_if_get_client_token(p_apr_pkt));
   }
   else /* Any other error */
   {
      /* Send basic response */
      opcode = APRV2_IBASIC_RSP_RESULT;
      p_apr_rsp_payload = &basic_rsp_payload;
      apr_rsp_payload_size = sizeof(aprv2_ibasic_rsp_result_t);

      /* Get the original get param v2 command opcode */
      basic_rsp_payload.opcode = elite_apr_if_get_opcode(p_apr_pkt);
      basic_rsp_payload.status = ADSP_EFAILED;

      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "CVD_CAL_MSG_PARAM: vpm_get_param_v3_cmd_rsp_handler(): Dyn svcs ack'd error: 0x%lx",
            session_obj->session_rsp_ctrl.status);
   }

   /* Send the ACK back to HLOS */
   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_apr_pkt),
                                                    elite_apr_if_get_dst_port(p_apr_pkt),
                                                    elite_apr_if_get_src_addr(p_apr_pkt),
                                                    elite_apr_if_get_src_port(p_apr_pkt),
                                                    elite_apr_if_get_client_token(p_apr_pkt),
                                                    opcode,
                                                    p_apr_rsp_payload,
                                                    apr_rsp_payload_size)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_param_v3_cmd_rsp_handler(): Failed to send get param cmd ACK to client, result: 0x%lx",
            rc);

      result = ADSP_EFAILED;
   }

   return result;
}

ADSPResult vpm_vp3_get_data_cmd_rsp_fn(void *context_ptr)
{
   ADSPResult                 result = ADSP_EOK;
   vpm_session_object_t       *session_obj;
   elite_apr_packet_t         *p_apr_pkt;
   vss_ivp3_cmd_get_data_t    *payload;
   cvd_virt_addr_t            param_data_virt_addr;
   int32_t                    rc;
   aprv2_ibasic_rsp_result_t  basic_rsp;

   session_obj = (vpm_session_object_t *)context_ptr;

   /* Original get param command APR packet */
   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   /* Get the pointer to get param payload */
   payload = (vss_ivp3_cmd_get_data_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

   rc = cvd_mem_mapper_get_virtual_addr_v2(payload->mem_handle, payload->mem_address,
                                           &param_data_virt_addr);
   if (rc)
   {
      MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_vp3_get_data_cmd_rsp_fn(): Cannot get virtual address,"
            " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
            (uint32_t)payload->mem_address, (uint32_t)(payload->mem_address >> 32));

      return ADSP_EFAILED;
   }

   /* Flush the cache */
   rc = cvd_mem_mapper_cache_flush_v2(&param_data_virt_addr, payload->mem_size);

   /* Get the original APR command's opcode */
   basic_rsp.opcode = elite_apr_if_get_opcode(p_apr_pkt);
   basic_rsp.status = session_obj->session_rsp_ctrl.status;

   /* Send the ACK back to the client */
   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_apr_pkt),
                                                    elite_apr_if_get_dst_port(p_apr_pkt),
                                                    elite_apr_if_get_src_addr(p_apr_pkt),
                                                    elite_apr_if_get_src_port(p_apr_pkt),
                                                    elite_apr_if_get_client_token(p_apr_pkt),
                                                    APRV2_IBASIC_RSP_RESULT,
                                                    &basic_rsp,
                                                    sizeof(basic_rsp))))
   {

      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_vp3_get_data_cmd_rsp_fn(): Failed to send ACK to client for get_data() cmd, result: 0x%08X",
            rc);
   }

   MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
       "vpm_vp3_get_data_cmd_rsp_fn(): Successfully sent ACK to client for VP3 get_data() cmd");

   return result;
}

ADSPResult vpm_set_vp3_param_transition_rsp_fn(void *context_ptr)
{
   ADSPResult                    result = ADSP_EOK;
   vpm_session_object_t          *session_obj;

   session_obj = (vpm_session_object_t *)context_ptr;

   /* If completed action failed, log the error. */
   if (ADSP_EOK != session_obj->session_rsp_ctrl.status)
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_set_vp3_param_transition_rsp_fn(): Command failed with result:(0x%08X). Ignoring it.",
            session_obj->session_rsp_ctrl.status);
      /* The Dynamic services returns EBADPARAM for Vp3 setparam and getparam if the VP3 mode
       * bit is not set in Fluence mode word ( from ACDB). This error should be ignored by CVD 
       */
      session_obj->session_rsp_ctrl.status = ADSP_EOK;
   }

   /* make cache empty */
   session_obj->shared_heap.vp3_cache.data_len = 0;

   memset(&session_obj->shared_heap.vp3_cache, 0,
          sizeof(session_obj->shared_heap.vp3_cache));

   return result;
}

ADSPResult vpm_get_vp3_param_transition_rsp_fn(void *context_ptr)
{
   ADSPResult                    result = ADSP_EOK;
   vpm_session_object_t          *session_obj;
   voice_param_data_t            *vp3_data;
   cvd_cal_log_vp3_data_header_t log_info_vp3;
   cvd_cal_log_commit_info_t     log_info;

   session_obj = (vpm_session_object_t *)context_ptr;
   MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "vpm_get_vp3_param_transition_rsp_fn(): Num of failed rsp:(%d), Num of getparam issued:(%d)",
         session_obj->session_rsp_ctrl.num_rsp_failed, session_obj->session_rsp_ctrl.num_cmd_issued);

   /* VpRx returnes EUNSUPPORTED for Vp3 Set/Get param So proceed if at least 1(VpTx) get param succeeded */
   if (session_obj->session_rsp_ctrl.num_rsp_failed <
          session_obj->session_rsp_ctrl.num_cmd_issued)
   {
      vp3_data = ((voice_param_data_t *)session_obj->shared_heap.vp3_cache.data);

      session_obj->shared_heap.vp3_cache.data_len = (sizeof(voice_param_data_t) +
                                                     vp3_data->param_size); /* header + data */

      /* Save VP3 data into global VP3 cache if param size is greater than zero. 
         If the param size is zero it means a failure */
      if ((TRUE == session_obj->sound_device_info.is_available) &&
          (vp3_data->param_size != 0))
      {
         result = vpm_update_global_vp3_db(session_obj);
      }

      /* Log VP3 Data */
      log_info_vp3.direction = session_obj->sound_device_info.device_pair.direction;
      log_info_vp3.rx_device_id = session_obj->sound_device_info.device_pair.rx_device_id;
      log_info_vp3.tx_device_id = session_obj->sound_device_info.device_pair.tx_device_id;

      log_info.instance = ((session_obj->attached_mvm_handle << 16) |
                              (session_obj->self_apr_port));
      log_info.call_num = session_obj->target_set.system_config.call_num;
      log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_RAW_VP3;
      log_info.data_container_header_size = sizeof(log_info_vp3);
      log_info.data_container_header = &log_info_vp3;
      log_info.payload_size = session_obj->shared_heap.vp3_cache.data_len;
      log_info.payload_buf = session_obj->shared_heap.vp3_cache.data;

      cvd_cal_log_data(LOG_ADSP_CVD_VP3_C, CVD_CAL_LOG_RAW_VP3_DATA_OUTPUT_TAP_POINT,
                             (void *)&log_info, sizeof(log_info));

   }
   else
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_get_vp3_param_transition_rsp_fn(): GET_PARAM failed status 0x%08X",
            session_obj->session_rsp_ctrl.status);
   }

   return result;
}

ADSPResult vpm_get_soundfocus_sectors_cmd_rsp_fn(void *context_ptr)
{
   ADSPResult                 result = ADSP_EOK;
   vpm_session_object_t       *session_obj;
   int32_t                    rc;
   uint32_t                   payload_size;
   uint8_t                    *payload;
   vpm_soundfocus_param_t     *p_soundfocus_param;
   elite_apr_packet_t         *p_apr_pkt;
   uint32_t                   opcode;
   void                       *p_rsp_pkt;
   uint32_t                   rsp_payload_size;
   aprv2_ibasic_rsp_result_t  basic_rsp;

   vss_isoundfocus_rsp_get_sectors_t get_sectors_rsp;

   session_obj = (vpm_session_object_t *)context_ptr;

   /* Get the original command payload */
   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   /* Retrieve the pointer to ACK payload */
   p_soundfocus_param = (vpm_soundfocus_param_t *)session_obj->session_rsp_ctrl.p_ack_payload;

   /* Send basic response if error in response or invalid payload */
   if ((ADSP_EOK != session_obj->session_rsp_ctrl.status) ||
       (p_soundfocus_param->module_id != VOICEPROC_MODULE_TX) ||
       (p_soundfocus_param->param_id != VOICE_PARAM_FLUENCE_SOUNDFOCUS))
   {
      /* Pupulate the response payload */
      opcode = APRV2_IBASIC_RSP_RESULT;
      p_rsp_pkt = &basic_rsp;
      rsp_payload_size = sizeof(basic_rsp);

      /* Populate basic response payload */
      basic_rsp.opcode = VSS_ISOUNDFOCUS_CMD_GET_SECTORS;
      basic_rsp.status = session_obj->session_rsp_ctrl.status;

      MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_get_soundfocus_sectors_cmd_rsp_fn(): "
            "FAILED, status = (0x%08x), module_id: 0x%lx, param_id: 0x%lx",
            session_obj->session_rsp_ctrl.status,
            p_soundfocus_param->module_id,
            p_soundfocus_param->param_id);
   }
   else /* At least 1 get param succeeded */
   {
      /* Pupulate the response payload */
      opcode = VSS_ISOUNDFOCUS_RSP_GET_SECTORS;
      p_rsp_pkt = &get_sectors_rsp;
      rsp_payload_size = sizeof(get_sectors_rsp);

      /* Read the data from get param rsp payload and populate client rsp payload */
      memscpy(get_sectors_rsp.start_angles,
              sizeof(get_sectors_rsp.start_angles),
              p_soundfocus_param->param_data.start_angles,
              sizeof(p_soundfocus_param->param_data.start_angles));

      memscpy(get_sectors_rsp.enables,
              sizeof(get_sectors_rsp.enables),
              p_soundfocus_param->param_data.enables,
              sizeof(p_soundfocus_param->param_data.enables));

      get_sectors_rsp.gain_step = p_soundfocus_param->param_data.gain_step;

      MSG_4(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_soundfocus_sectors_cmd_rsp_fn(): enables[0-3] = "
            "[%d, %d, %d, %d]", get_sectors_rsp.enables[0], get_sectors_rsp.enables[1],
            get_sectors_rsp.enables[2], get_sectors_rsp.enables[3]);

      MSG_4(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_soundfocus_sectors_cmd_rsp_fn(): enables[4-7] = "
            "[%d, %d, %d, %d]", get_sectors_rsp.enables[4], get_sectors_rsp.enables[5],
            get_sectors_rsp.enables[6], get_sectors_rsp.enables[7]);

      MSG_4(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_soundfocus_sectors_cmd_rsp_fn(): start_angles[0-3] = "
            "[%d, %d, %d, %d]", get_sectors_rsp.start_angles[0], get_sectors_rsp.start_angles[1],
            get_sectors_rsp.start_angles[2], get_sectors_rsp.start_angles[3]);

      MSG_4(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_soundfocus_sectors_cmd_rsp_fn(): start_angles[4-7] = "
            "[%d, %d, %d, %d]", get_sectors_rsp.start_angles[4], get_sectors_rsp.start_angles[5],
            get_sectors_rsp.start_angles[6], get_sectors_rsp.start_angles[7]);

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_soundfocus_sectors_cmd_rsp_fn(): gain_step = %d", get_sectors_rsp.gain_step);
   }

   /* Send the ack back to client */
   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_apr_pkt),
                                                    elite_apr_if_get_dst_port(p_apr_pkt),
                                                    elite_apr_if_get_src_addr(p_apr_pkt),
                                                    elite_apr_if_get_src_port(p_apr_pkt),
                                                    elite_apr_if_get_client_token(p_apr_pkt),
                                                    opcode,
                                                    p_rsp_pkt,
                                                    rsp_payload_size)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_get_soundfocus_sectors_cmd_rsp_fn(): "
            "Failed to send ack to client, result: 0x%08x", rc);
   }

   return result;
}

ADSPResult vpm_sourcetrack_get_activity_cmd_rsp_fn(void *context_ptr)
{
   ADSPResult                 result = ADSP_EOK;
   vpm_session_object_t       *session_obj;
   elite_apr_packet_t         *p_apr_pkt;
   aprv2_ibasic_rsp_result_t  basic_rsp;
   int32_t                    rc;

   cvd_cal_log_commit_info_t log_info;
   cvd_cal_log_cal_data_header_t log_info_data;

   session_obj = (vpm_session_object_t *)context_ptr;

   /* Get the original command payload */
   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   /* Populate the basic response payload */
   basic_rsp.opcode = VSS_ISOURCETRACK_CMD_GET_ACTIVITY;

   /* If at least 1 get param succeeded */
   if (ADSP_EOK == session_obj->session_rsp_ctrl.status)
   {
      MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
          "vpm_sourcetrack_get_activity_cmd_rsp_fn(): "
          "get activity successfully");

      basic_rsp.status = ADSP_EOK;

      /* Log the sound activity data. */
      log_info_data.table_handle = 0;
      log_info_data.cal_query_handle = 0;
      log_info_data.data_seq_num = 0;

      log_info.instance =  ((session_obj->attached_mvm_handle << 16) |
                               (session_obj->self_apr_port));
      log_info.call_num = session_obj->target_set.system_config.call_num;
      log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_RAW_CAL_OUTPUT;
      log_info.data_container_header_size = sizeof(log_info_data);
      log_info.data_container_header = &log_info_data;
      log_info.payload_buf = (void *)&session_obj->shared_heap.sourcetrack_activity.activity_data;
      log_info.payload_size = sizeof(vss_isourcetrack_activity_data_t);

      cvd_cal_log_data( ( log_code_type ) LOG_ADSP_CVD_CAL_DATA_C, 
                        CVD_CAL_LOG_SOUNDFOCUS_ACTIVITY_DATA,
                            (void *)&log_info, sizeof(log_info));

      /* Copy data to client mapped shared memory. */
      memscpy(session_obj->sourcetrack_mem_addr.ptr,
              sizeof(vss_isourcetrack_activity_data_t),
              ((void *)&session_obj->shared_heap.sourcetrack_activity.activity_data),
              sizeof(vss_isourcetrack_activity_data_t));

      /* Flush the cache */
      (void)cvd_mem_mapper_cache_flush_v2(&session_obj->sourcetrack_mem_addr,
                                          sizeof(vss_isourcetrack_activity_data_t));
   }
   else
   {
      /* At least 1 command failed with error other than ADSP_EUNSUPPORTED */
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_sourcetrack_get_activity_cmd_rsp_fn(): "
            "get activity failed, rc=(0x%08x)",
            session_obj->session_rsp_ctrl.status);

      basic_rsp.status = session_obj->session_rsp_ctrl.status;
   }

   /* Send the ack back to client */
   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_apr_pkt),
                                                    elite_apr_if_get_dst_port(p_apr_pkt),
                                                    elite_apr_if_get_src_addr(p_apr_pkt),
                                                    elite_apr_if_get_src_port(p_apr_pkt),
                                                    elite_apr_if_get_client_token(p_apr_pkt),
                                                    APRV2_IBASIC_RSP_RESULT,
                                                    &basic_rsp,
                                                    sizeof(basic_rsp))))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_sourcetrack_get_activity_cmd_rsp_fn(): "
            "FAILED to send ACK to client, rc=(0x%08x)", rc);
   }

   return result;
}

/** Process custom msg on respq  */
ADSPResult vpm_session_cmd_rsp_handler(void *p_session_obj,
                                       elite_msg_any_t *p_msg_pkt)
{
   ADSPResult                 result = ADSP_EOK;
   vpm_session_object_t       *p_session = (vpm_session_object_t *)p_session_obj;
   elite_msg_custom_header_t  *p_msg_payload = (elite_msg_custom_header_t  *)p_msg_pkt->pPayload;
   elite_apr_packet_t         *p_apr_packet;
   aprv2_ibasic_rsp_result_t  *p_apr_rsp_result;
   uint32_t                   rsp_status;
   bool_t                     is_rsp_pending = TRUE;
   bool_t                     is_cmd_rsp_failed = FALSE;
   uint32_t                   session_id;

   vss_icommon_rsp_get_param_t    *p_get_param_rsp;
   vss_icommon_rsp_get_param_v3_t *p_get_param_rsp_v3;

   session_id = p_session->self_apr_port;

   MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO, "VoiceProcMgr [0x%lx]: Received response/event opcode: 0x%lx",
         session_id, p_msg_pkt->unOpCode);

   switch (p_msg_pkt->unOpCode)
   {
      case ELITE_APR_PACKET:
      {
         p_apr_packet = (elite_apr_packet_t *)p_msg_pkt->pPayload;

         switch (elite_apr_if_get_opcode(p_apr_packet))
         {
            case APRV2_IBASIC_RSP_RESULT:
            {
               p_apr_rsp_result = (aprv2_ibasic_rsp_result_t *)elite_apr_if_get_payload_ptr(p_apr_packet);

               /* Get the basic response status */
               rsp_status = p_apr_rsp_result->status;

               MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "VoiceProcMgr [0x%lx]: Received response for APR cmd opcode: 0x%lx, rsp_status: 0x%lx",
                     session_id, p_apr_rsp_result->opcode, rsp_status);
               break;
            }
            case VSS_ICOMMON_RSP_GET_PARAM:
            {
               p_get_param_rsp = (vss_icommon_rsp_get_param_t *)elite_apr_if_get_payload_ptr(p_apr_packet);

               /* Get the get param cmd response status */
               rsp_status = p_get_param_rsp->status;

               MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "VoiceProcMgr [0x%lx]: Received response for get param cmd rsp_status: 0x%lx",
                     session_id, rsp_status);
               break;
            }
            case VSS_ICOMMON_RSP_GET_PARAM_V3:
            {
               p_get_param_rsp_v3 = (vss_icommon_rsp_get_param_v3_t *)elite_apr_if_get_payload_ptr(p_apr_packet);

               /* Get the get param cmd response status */
               rsp_status = p_get_param_rsp_v3->status;

               MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "VoiceProcMgr [0x%lx]: Received response for get param V3 cmd rsp_status: 0x%lx",
                     session_id, rsp_status);
               break;
            }
            default:
            {
               MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "VoiceProcMgr [0x%lx]: Received response for Invalid APR cmd opcode: 0x%lx",
                     session_id, p_msg_pkt->unOpCode);

               result = ADSP_EUNSUPPORTED;
               break;
            }
         }
      }
      case ELITE_CMD_SET_PARAM:
      case ELITE_CMD_GET_PARAM:
      {
         /* Get the elite msg response status */
         rsp_status = p_msg_payload->unResponseResult;
         break;
      }
      case ELITE_CUSTOM_MSG:
      {
         switch (p_msg_payload->unSecOpCode)
         {
            case VOICEPROC_CONFIG_HOST_PCM:
            case VOICEPROC_SET_TIMINGV2_CMD:
            case VOICEPROC_SET_TIMINGV3_CMD:
            case VOICEPROC_SET_MUTE_CMD:
            case VOICEPROC_GET_KPPS_CMD:
            case VOICEPROC_GET_DELAY_CMD:
            case VOICEPROC_SET_PARAM_V3_CMD:
            case VOICEPROC_REGISTER_EVENT:
            {
               /* Get the elite msg response status */
               rsp_status = p_msg_payload->unResponseResult;

               MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "VoiceProcMgr [0x%lx]: Received response for custom msg sec opcode: 0x%lx, rsp_status: 0x%lx",
                     session_id, p_msg_payload->unSecOpCode, rsp_status);

               result = ADSP_EOK;
               break;
            }
            default:
            {
               MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                     "VoiceProcMgr [0x%lx]: Received unsupported sec opcode (0x%lX) for custom msg",
                     session_id, p_msg_payload->unSecOpCode);

               result = ADSP_EUNSUPPORTED;
               break;
            }
         } /* End of switch (p_msg_payload->unSecOpCode) */

         break;
      }
      default:
      {
         MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
               "VoiceProcMgr [0x%lx]: Received unsupported opcode (0x%lX) for elite msg",
               session_id, p_msg_pkt->unOpCode);

         result = ADSP_EUNSUPPORTED;
         break;
      }
   } /* End of switch (p_msg_pkt->unOpCode) */

   /* If no validaion errors, check and update the response status */
   if (ADSP_EOK == result)
   {
      (void)vpm_check_and_update_rsp_status(p_session, rsp_status);
   }

   /* Release the msg packet */
   if (ELITE_APR_PACKET == p_msg_pkt->unOpCode)
   {
      elite_apr_if_free(p_session->self_apr_handle, p_apr_packet);
   }
   else /* Elite msg */
   {
      elite_msg_release_msg(p_msg_pkt);
   }

   return result;
}


ADSPResult vpm_vp3_get_size_cmd_ctrl_rsp_fn(void *contex_ptr)
{
   ADSPResult                 result = ADSP_EOK;
   elite_apr_packet_t         *p_apr_pkt;
   void                       *payload_address;
   uint32_t                   payload_size;
   uint32_t                   opcode;
   voice_param_data_t         *p_voice_param;
   vss_ivp3_rsp_get_size_t    *p_vp3_get_size_rsp;
   uint32_t                   align = CVD_MEM_MAPPER_SUPPORTED_CACHE_LINE_SIZE;
   aprv2_ibasic_rsp_result_t  basic_rsp;
   vpm_session_object_t       *session_obj;
   bool_t                     is_rsp_failed = FALSE;
   int32_t                    rc;

   if (!contex_ptr)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_vp3_get_size_cmd_ctrl_rsp_fn (): Input context pointer is NULL");

      return ADSP_EFAILED;
   }

   /* Get the pointer to session object */
   session_obj = (vpm_session_object_t *)contex_ptr;

   /* Original get param command APR packet */
   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   /* Check if all the responses failed */
   if (ADSP_EOK != session_obj->session_rsp_ctrl.status)
   {
      is_rsp_failed = TRUE;
   }

   if (session_obj->session_rsp_ctrl.p_ack_payload)
   {
      p_voice_param = (voice_param_data_t *)session_obj->session_rsp_ctrl.p_ack_payload;

      /* Check if the param size is valid */
      if (p_voice_param->param_size < sizeof(vss_ivp3_rsp_get_size_t))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_vp3_get_size_cmd_ctrl_rsp_fn (): Size of parameter "
               "is not proper: %lu bytes", p_voice_param->param_size);

         is_rsp_failed = TRUE;
      }
   }
   else
   {
      /* Ack payload pointer is NULL */
      is_rsp_failed = TRUE;
   }

   if (TRUE == is_rsp_failed)
   {
      /* Send the command failure reponse to client */
      basic_rsp.opcode = VSS_IVP3_CMD_GET_SIZE;
      basic_rsp.status = session_obj->session_rsp_ctrl.status;

      payload_address = &basic_rsp;
      payload_size = sizeof(basic_rsp);
      opcode = APRV2_IBASIC_RSP_RESULT;
   }
   else /* At lease one response succeeded */
   {
      p_voice_param = (voice_param_data_t *)session_obj->session_rsp_ctrl.p_ack_payload;

      p_vp3_get_size_rsp = (vss_ivp3_rsp_get_size_t *)((uint8_t *)p_voice_param) +\
         sizeof(voice_param_data_t);

      if (p_vp3_get_size_rsp->size != 0)
      {
         p_vp3_get_size_rsp->size = (((p_vp3_get_size_rsp->size) & 0x0000FFFF) +
                                     sizeof(voice_param_data_t));
      }

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_vp3_get_size_cmd_ctrl_rsp_fn(): vp3 size before "
            "alignment: %lu bytes", p_vp3_get_size_rsp->size);

      p_vp3_get_size_rsp->size =\
         ((p_vp3_get_size_rsp->size + align - 1) & (~(align - 1)));

      payload_address = (void *)p_vp3_get_size_rsp;
      payload_size = sizeof(vss_ivp3_rsp_get_size_t);
      opcode = VSS_IVP3_RSP_GET_SIZE;
   }

   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_apr_pkt),
                                                    elite_apr_if_get_dst_port(p_apr_pkt),
                                                    elite_apr_if_get_src_addr(p_apr_pkt),
                                                    elite_apr_if_get_src_port(p_apr_pkt),
                                                    elite_apr_if_get_client_token(p_apr_pkt),
                                                    opcode,
                                                    payload_address,
                                                    payload_size)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_vp3_get_size_cmd_ctrl_rsp_fn(): Failed to send get_size() cmd ACK to client, result: 0x%lx",
            rc);
   }

   return result;
}
