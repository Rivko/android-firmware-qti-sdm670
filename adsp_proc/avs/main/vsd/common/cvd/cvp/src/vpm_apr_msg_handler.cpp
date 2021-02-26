/**========================================================================
 @file vpm_apr_msg_handler.cpp
 
 @brief This file contains APR command handler for both static service and
 session objects. 
 
 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_apr_msg_handler.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "qurt_elite.h"

#include "vpm_types.h"
#include "vpm_svc_apr_if.h"
#include "vpm_svc_apr_data_base.h"
#include "vpm_session_utils.h"
#include "vpm_session_dyn_svc_cfg_utils.h"
#include "vpm_internal.h"

#include "adsp_vcmn_api.h"
#include "VoiceCmnUtils.h"

#include "vpm_session_dev_config_utils.h"
#include "vpm_session_cal_utils.h"

#include "vpm_response_handler.h"
#include "vpm_svc_vp3_utils.h"

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/


/****************************************************************************
 * Function Declarations
 ****************************************************************************/

static ADSPResult vpm_ssr_cleanup_cmd_ctrl(voice_proc_mgr_t *pMe)
{
   ADSPResult              result = ADSP_EOK;
   elite_apr_packet_t      *p_apr_pkt;
   elite_apr_packet_t      *p_dst_apr_packet = NULL;
   vss_issr_cmd_cleanup_t  *in_args;
   uint32_t                session_idx = 0, session_cnt = 0;
   vpm_session_object_t    *session_obj;
   elite_msg_any_t         msg_pkt;
   int32_t                 rc;
   uint32_t                payload_size;

   /* Note:
    *
    * 1. The current implementation has a limitation where it only handles
    *    Modem SSR properly - it cleans up only the CVP features that can be
    *    enabled by Modem client. The only such feature is VPCM. The way that
    *    VPCM is enabled by the Modem is that Modem gets a CVP handle from HLOS
    *    and use that CVP handle to start VPCM.
    *
    * 2. In general, we don't guarantee that we will handle SSR properly if
    *    resource handles (such as session handles) are shared between
    *    processors. However, this VPCM use case is a special and known use
    *    case, and we are required to handle this.
    */

   /* Get the pointer to APR packet */
   p_apr_pkt = (elite_apr_packet_t *)pMe->cmd_ctrl.msg_pkt.pPayload;

   /* Get payload size */
   payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

   if (payload_size != sizeof(vss_issr_cmd_cleanup_t))
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_ssr_cleanup_cmd_ctrl(): Unexpected payload size, "
            "%d != expected: %d", payload_size,
            sizeof(vss_issr_cmd_cleanup_t));

      /* End APR command */
      elite_apr_if_end_cmd(pMe->apr_handle, p_apr_pkt, ADSP_EBADPARAM);

      return ADSP_EBADPARAM;
   }

   /* Get the pointer to the payload from APR packet */
   in_args = (vss_issr_cmd_cleanup_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

   /* Clear response tracking counters */
   pMe->rsp_ctrl.num_cmd_issued = 0;
   pMe->rsp_ctrl.num_rsp_received = 0;
   pMe->rsp_ctrl.num_rsp_failed = 0;
   pMe->rsp_ctrl.is_prev_cmd_rsp_pending = FALSE;

   while ((session_idx < VPM_MAX_NUM_SESSIONS) &&
          (session_cnt < pMe->session_cnt))
   {
      session_obj = pMe->sessions[session_idx];

      /* Check whether the session_obj corresponding to session_idx is valid. */
      if (NULL != session_obj)
      {
         session_cnt++;
         session_idx++;

         /* Check if host PCM is enabled for this session and clean up command
            has been issued by the client which started this host PCM session */
         if ((TRUE == session_obj->vpcm_info.is_enabled) &&
             (APR_GET_FIELD(APRV2_PKT_DOMAIN_ID,
                            session_obj->vpcm_info.client_addr) == in_args->domain_id))
         {
            /* Source APR port is kept NULL so that APR dispatcher can route the
               APR IBASIC response to service command queue */

            if (APR_EOK != (rc = elite_apr_if_alloc_cmd(session_obj->self_apr_handle,
                                                        session_obj->self_apr_addr,
                                                        0,
                                                        session_obj->self_apr_addr,
                                                        session_obj->self_apr_port,
                                                        0  /* Token, dont care */,
                                                        VSS_IVPCM_CMD_STOP,
                                                        0, /* Dummy */
                                                        &p_dst_apr_packet)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_ssr_cleanup_cmd_ctrl(): Failed to allocate APR packet to send"
                     "stop vcpm cmd to session, result; 0x%lx", rc);

               /* Move onto next session */
               continue;
            }
            else if (NULL == p_dst_apr_packet)
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                   "vpm_ssr_cleanup_cmd_ctrl(): Allocated APR packet pointer is NULL");

               /* Move onto next session */
               continue;
            }
         }
         else
         {
            /* Move onto next session */
            continue;
         }
         /* Populate elite msg payload */
         msg_pkt.unOpCode = ELITE_APR_PACKET;
         msg_pkt.pPayload = (void *)p_dst_apr_packet;

         /* Push host PCM stop cmd to session command queue */
         if (ADSP_EOK != (result = qurt_elite_queue_push_back(session_obj->session_cmd_ctrl.p_cmd_queue,
                                                              (uint64_t *)(&msg_pkt))))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_ssr_cleanup_cmd_ctrl(): Failed to push to session cmdQ, session_id: 0x%lx",
                  session_obj->self_apr_port);

            /* Free up allocated APR packet */
            elite_apr_if_free(session_obj->self_apr_handle, p_dst_apr_packet);

            /* Move onto next session */
            continue;
         }

         /* Increment command issued counter for service.
            Note that only 1 APR command is issued per session object. */
         pMe->rsp_ctrl.num_cmd_issued++;

         /* Store the allocated rsp payload info in response control */
         pMe->rsp_ctrl.p_ack_payload = (void *)p_dst_apr_packet;
         pMe->rsp_ctrl.is_ack_payload_type_apr = TRUE;
      }
      else
      {
         ++session_idx;
      }
   }

   /* If no command issued, end the APR command */
   if (!pMe->rsp_ctrl.num_cmd_issued)
   {
      /* End the APR command */
      elite_apr_if_end_cmd(pMe->apr_handle, p_apr_pkt, ADSP_EOK);
   }
   else
   {
      /* At least 1 command has been issued to session */
      /* Remove the service cmdQ from the current signal wait mask */
      pMe->curr_wait_mask &= ~(pMe->svc_cmd_q_wait_mask);
   }

   /* Return and wait for command responses, if issued, else cmd is completed here */
   return result;
}

ADSPResult vpm_svc_cmdq_apr_msg_handler(void *p_svc_instance,
                                        elite_msg_any_t *p_msg_pkt)
{
   ADSPResult              result = ADSP_EOK;
   elite_apr_packet_t      *apr_pkt_ptr;
   voice_proc_mgr_t        *pMe = (voice_proc_mgr_t *)p_svc_instance;
   uint32_t                opcode;

   apr_pkt_ptr = (elite_apr_packet_t *)(pMe->cmd_ctrl.msg_pkt.pPayload);

   opcode = elite_apr_if_get_opcode(apr_pkt_ptr);

   switch (opcode)
   {
      case VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION:
      {
         result = vpm_create_full_session_cmd_ctrl(pMe);
         break;
      }
      case VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2:
      {
         result = vpm_create_full_session_v2_cmd_ctrl(pMe);
         break;
      }
      case VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V3:
      {
         result = vpm_create_full_session_v3_cmd_ctrl(pMe);
         break;
      }
      case VOICE_CMD_SHARED_MEM_MAP_REGIONS:
      {
         result = voice_shared_memory_map_regions_process(pMe->memory_map_client,
                                                          pMe->apr_handle,
                                                          apr_pkt_ptr);
         break;
      }
      case VOICE_CMD_SHARED_MEM_UNMAP_REGIONS:
      {
         result = elite_mem_shared_memory_un_map_regions_cmd_handler(pMe->memory_map_client,
                                                                     pMe->apr_handle,
                                                                     apr_pkt_ptr);
         break;
      }
      case VSS_ISSR_CMD_CLEANUP:
      {
         result = vpm_ssr_cleanup_cmd_ctrl(pMe);
         break;
      }
      default:
      {
         result = ADSP_EUNSUPPORTED;

         /* End the APR command */
         elite_apr_if_end_cmd(pMe->apr_handle, apr_pkt_ptr, result);
         break;
      }
   } /* End of switch (pkt_ptr->opcode) */

   if (ADSP_EOK == result)
   {
      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_svc_cmdq_apr_msg_handler(), APR command with opcode: 0x%8X SUCCESS", opcode);
   }
   else
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_svc_cmdq_apr_msg_handler(), APR command with opcode: 0x%8X FAILED, result: 0x%lx",
            opcode, result);
   }

   return result;
}

static ADSPResult vpm_set_device_v2_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   uint32_t             payload_size;
   elite_apr_packet_t   *p_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         payload_size = elite_apr_if_get_payload_size(p_apr_packet);

         if (payload_size != sizeof(vss_ivocproc_cmd_set_device_v2_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_device_v2_cmd_ctrl(): Invalid payload size(%lu) != expected (%lu)",
                  payload_size, sizeof(vss_ivocproc_cmd_set_device_v2_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* Update session parameters */
         if (ADSP_EOK != (result = vpm_set_session_params(session_obj)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_set_device_v2_cmd_ctrl(): Failed to set session params");

            /* End the APR command */
            break;
         }

         session_obj->is_client_set_sectors = FALSE;

         /* Check if re-init is required due to sesion param change */
         if (TRUE == vpm_is_reinit_required(session_obj))
         {
            vpm_create_next_goal_ctrl(session_obj, CVP_GOAL_ENUM_REINIT);
         }
         else
         {
            MSG(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_set_device_v2_cmd_ctrl(): VPM re-init not reqired");

            /* Nothing to do, end APR command */
            result = ADSP_EOK;
            break;
         }
      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will fall through here if VPM RE-INIT is required */

      result = vpm_state_control(session_obj);

      /* Check the goal completion status. If completed, end the APR command,
        else keep the cmd in pending status until completion.
        Session cmdQ will not be popped until cmd completion */
      vpm_check_goal_completion_status(session_obj);

      if (TRUE == session_obj->state_ctrl.is_goal_completed)
      {
         if (ADSP_EOK == session_obj->state_ctrl.status)
         {
            session_obj->is_vocproc_config_changed = TRUE;
         }

         MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_set_device_v2_cmd_ctrl(): Cmd completed with result : 0x%lx",
               session_obj->state_ctrl.status);
      }

      return ADSP_EOK;
   } /* End of for (;;) */

   /* Execution will reach here in case of any error or no action required */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_packet, result);

   return result;
}

static ADSPResult vpm_set_device_v3_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;
   uint32_t             i, payload_size;

   for (;;)
   {
      p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      if (session_obj->topo_commit_state != VPM_TOPOLOGY_COMMIT_STATE_NONE)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_set_device_v3_cmd_ctrl(): Unexpected topo_commit_state %d",
               session_obj->topo_commit_state);

         /* End the APR command */
         result = ADSP_EFAILED;
         break;
      }

      /* Get the payload size */
      payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

      if (payload_size != sizeof(vss_ivocproc_cmd_set_device_v3_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_set_device_v3_cmd_ctrl(): Incorrect payload size: %lu != expected: %lu",
               payload_size, sizeof(vss_ivocproc_cmd_set_device_v3_t));

         /* End the APR command */
         result = ADSP_EFAILED;
         break;
      }

      /* Update the session params */
      if (ADSP_EOK != (result = vpm_set_session_params(session_obj)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_set_device_v3_cmd_ctrl(): Failed to set session params");

         /* End the APR command */
         break;
      }

      /* Update the TOPO commit state */
      session_obj->topo_commit_state = VPM_TOPOLOGY_COMMIT_STATE_SET_DEVICE;

      /* Clear in-band topology params */
      session_obj->active_set.num_dev_channels.rx_num_channels = 0;
      session_obj->active_set.num_dev_channels.tx_num_channels = 0;
      session_obj->target_set.num_dev_channels.rx_num_channels = 0;
      session_obj->target_set.num_dev_channels.tx_num_channels = 0;

      /* Clear VPM topology params */
      for (i = 0; i < VPM_MAX_NUM_TOPO_PARAMS; i++)
      {
         session_obj->topo_param.vpm_param[i].param_size = 0;
         session_obj->topo_param.vpm_param[i].param_virt_addr = NULL;
      }

      /* End the APR command */
      break;
   }

   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_set_device_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   int32_t              rc;
   uint8_t              *p_new_packet_payload;
   uint32_t             new_apr_packet_size;
   elite_apr_packet_t   *p_new_apr_packet;
   elite_apr_packet_t   *p_curr_apr_packet;


   vss_ivocproc_cmd_set_device_t       *in_args;
   vss_ivocproc_cmd_set_device_v2_t    set_device_v2;

   if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
   {
      p_curr_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      in_args = (vss_ivocproc_cmd_set_device_t *)elite_apr_if_get_payload_ptr(p_curr_apr_packet);

      /* Note that we are calling vpm_set_device_v2_cmd_ctrl in order to leverage
       * code. We create a new command packet and replace the current command
       * packet in the pending control with the new packet. The new packet
       * contains:
       * 1. vss_ivocproc_cmd_set_device_v2_t arguments (with the client specified
       *    vocproc properties and with vocproc_mode and ec_ref_port_id set to
       *    default values).
       * 2. The opcode for the new packet is the original packet's opcode (i.e.
       *    VSS_IVOCPROC_CMD_SET_DEVICE), so that when cvp_set_device_v2_cmd_ctrl
       *    finishes processing the packet, we are using the correct opcode when
       *    sending a response back to the client.
       */

      set_device_v2.tx_port_id = ((uint16_t)in_args->tx_port_id);
      set_device_v2.tx_topology_id = in_args->tx_topology_id;
      set_device_v2.rx_port_id = ((uint16_t)in_args->rx_port_id);
      set_device_v2.rx_topology_id = in_args->rx_topology_id;
      set_device_v2.vocproc_mode = VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING;
      set_device_v2.ec_ref_port_id = VSS_IVOCPROC_PORT_ID_NONE;

      if (APR_EOK != (rc = elite_apr_if_alloc_cmd(session_obj->self_apr_handle,
                                                  elite_apr_if_get_src_addr(p_curr_apr_packet),
                                                  elite_apr_if_get_src_port(p_curr_apr_packet),
                                                  elite_apr_if_get_dst_addr(p_curr_apr_packet),
                                                  elite_apr_if_get_dst_port(p_curr_apr_packet),
                                                  elite_apr_if_get_client_token(p_curr_apr_packet),
                                                  elite_apr_if_get_opcode(p_curr_apr_packet),
                                                  sizeof(set_device_v2),
                                                  &p_new_apr_packet)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_set_device_cmd_ctrl(): Failed to allocate APR packet for v2 cmd, result: 0x%lx", rc);

         elite_apr_if_end_cmd(session_obj->self_apr_handle, p_curr_apr_packet, rc);

         return rc;
      }

      p_new_packet_payload = (uint8_t *)elite_apr_if_get_payload_ptr(p_new_apr_packet);
      new_apr_packet_size = elite_apr_if_get_payload_size(p_new_apr_packet);

      /* Copy the curr APR packet content in new packet */
      memscpy(p_new_packet_payload,
              new_apr_packet_size,
              &set_device_v2,
              sizeof(set_device_v2));

      /* Free-up current APR packet */
      elite_apr_if_free(session_obj->self_apr_handle, p_curr_apr_packet);

      /* Update the APR packet with newly allocated one */
      session_obj->session_cmd_ctrl.msg_pkt.pPayload = (void *)p_new_apr_packet;

   } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

   return vpm_set_device_v2_cmd_ctrl(session_obj);
}

static ADSPResult vpm_attach_stream_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   uint32_t             payload_size;
   uint32_t             strm_cntr;
   bool_t               is_already_attached = FALSE;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;

   vss_ivocproc_cmd_attach_stream_t  *in_args;
   vpm_attached_stream_item_t        *stream_item = NULL;
   apr_list_node_t                   *pivot_node;
   apr_list_node_t                   *cur_node;

   vss_ivocproc_rsp_attach_detach_stream_t  rsp_attach_stream;

   for (;;)
   {
      p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

      /* Validate in-bound payload size. */
      if (payload_size != sizeof(vss_ivocproc_cmd_attach_stream_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_attach_stream_cmd_ctrl(): Unexpected payload size, %d != %d",
               payload_size, sizeof(vss_ivocproc_cmd_attach_stream_t));

         /* End APR command */
         result = ADSP_EBADPARAM;
         break;
      }

      in_args = (vss_ivocproc_cmd_attach_stream_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

      /* Validate direction of attaching stream. */
      /* TODO: Currently we enforce that stream direction must exactly match
       * vocproc direction. This logic probably needs revision. */
      if (in_args->direction != session_obj->direction)
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_attach_stream_cmd_ctrl(): Unexpected direction, %d != expected (%d)",
               in_args->direction, session_obj->direction);

         /* End APR command */
         result = ADSP_EBADPARAM;
         break;
      }

      /* If not attaching a stream that has already been attached, store the new stream
       * handle and address and reply with VPM handle and direction. Otherwise reply
       * with APR_EALREADY. */
      pivot_node = &(session_obj->attached_stream_list.dummy);

      for (strm_cntr = 0; strm_cntr < session_obj->attached_stream_list.size; strm_cntr++)
      {
         (void)apr_list_get_next(&session_obj->attached_stream_list,
                                 pivot_node, &cur_node);

         stream_item = (vpm_attached_stream_item_t *)cur_node;

         if (stream_item->addr == elite_apr_if_get_src_addr(p_apr_pkt) &&
             stream_item->port == elite_apr_if_get_src_port(p_apr_pkt))
         {
            is_already_attached = TRUE;
            break;
         }

         pivot_node = cur_node;
      }

      /* If stream is not attached */
      if (FALSE == is_already_attached)
      {
         /* Store attaching stream handle and address. */
         if (NULL == (stream_item = (vpm_attached_stream_item_t *)qurt_elite_memory_malloc(sizeof(vpm_attached_stream_item_t),
                                                                                           QURT_ELITE_HEAP_DEFAULT)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_attach_stream_cmd_ctrl(): Failed to allocate attached_stream_item");

            /* End APR command */
            result = ADSP_ENOMEMORY;
            break;
         }

         stream_item->addr = elite_apr_if_get_src_addr(p_apr_pkt);
         stream_item->port = elite_apr_if_get_src_port(p_apr_pkt);

         /* Clear the ACK payload */
         memset(&rsp_attach_stream, 0, sizeof(rsp_attach_stream));

         /* Respond with VPM direction and  VMX in/out port masks */
         rsp_attach_stream.direction = session_obj->direction;

         if (session_obj->dyn_svc_info.mixer_in_port)
         {
            rsp_attach_stream.mixer_inport_mask = 1 << (session_obj->dyn_svc_info.mixer_in_port->inport_id);
         }

         if (session_obj->dyn_svc_info.mixer_out_port)
         {
            rsp_attach_stream.mixer_outport_mask = 1 << (session_obj->dyn_svc_info.mixer_out_port->outport_id);
         }

         MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_attach_stream_cmd_ctrl(): VMX inport mask: 0x%lX, VMX outport mask: 0x%lX, direction: %lu",
               rsp_attach_stream.mixer_inport_mask,
               rsp_attach_stream.mixer_outport_mask,
               rsp_attach_stream.direction);

         /* Allocate and send the ACK for attach stream cmd */
         if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                          elite_apr_if_get_dst_addr(p_apr_pkt),
                                                          elite_apr_if_get_dst_port(p_apr_pkt),
                                                          elite_apr_if_get_src_addr(p_apr_pkt),
                                                          elite_apr_if_get_src_port(p_apr_pkt),
                                                          elite_apr_if_get_client_token(p_apr_pkt),
                                                          VSS_IVOCPROC_RSP_ATTACH_STREAM,
                                                          &rsp_attach_stream,
                                                          sizeof(rsp_attach_stream))))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_attach_stream_cmd_ctrl(): Failed to allocate VSS_IVOCPROC_RSP_ATTACH_STREAM"
                  "ACK APR packet, result: %lu", rc);

            /* End the APR command */
            result = rc;
            break;
         }

         /** If VPM is running, send EVT_READY to VSM. This is required
           * for the case if VPM is attached after VPM is enabled. 
           * Ideally, VPM should be attached first and then enabled. 
           */
         if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
         {
            if (APR_EOK != (rc = elite_apr_if_alloc_send_event(session_obj->self_apr_handle,
                                                               session_obj->self_apr_addr,
                                                               session_obj->self_apr_port,
                                                               elite_apr_if_get_src_addr(p_apr_pkt),
                                                               elite_apr_if_get_src_port(p_apr_pkt),
                                                               0,
                                                               VSS_IVOCPROC_EVT_READY,
                                                               NULL,
                                                               0)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_attach_stream_cmd_ctrl(): Failed to send VSS_IVOCPROC_EVT_READY to VSM"
                     "result: 0x%lx", rc);

               /* End APR command */
               result = rc;
               break;
            }
            /* this utility would raise the AFE port events to all the attached streams. 
            So,if a vocproc is attached to mutliple streams, then same AFE port information would be 
            sent to all attached streams (twice or more based on the existing for loop count)
            Please note, this evt_ready is raised for all streams as per the existing loop.
            This way was prefered so that we can confine the broadcast AFE port info into one utility and 
            make code look less clumsy.
            */
            ( void )vpm_action_broadcast_afe_port_info(session_obj);
         }

         /* Initilize the allocated node */
         (void)apr_list_init_node((apr_list_node_t *)stream_item);

         /* Add this node to attached stream list  */
         (void)apr_list_add_tail(&(session_obj->attached_stream_list),
                                 &(stream_item->link));

         /* Free incoming packet. */
         elite_apr_if_free(session_obj->self_apr_handle, p_apr_pkt);

         return ADSP_EOK;
      }
      else
      {
         /* Respond with ADSP_EALREADY. */

         MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
               "vpm_attach_stream_cmd_ctrl(): stream with apr port (0x%lx) already attached",
               elite_apr_if_get_src_port(p_apr_pkt));

         /* End the APR command */
         result = ADSP_EALREADY;
         break;
      }
   } /* End of for (;;) */

   /* Control will reach here if any error occured */

   if (stream_item)
   {
      /* Free up the allocated node */
      qurt_elite_memory_free(stream_item);
   }

   /* End the APR command with failure status */
   elite_apr_if_end_cmd(session_obj->self_apr_addr,
                        p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_detach_stream_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   uint32_t             strm_cntr;
   apr_list_node_t      *pivot_node;
   apr_list_node_t      *cur_node;
   bool_t               is_attached = FALSE;
   elite_apr_packet_t   *p_apr_packet;
   int32_t              rc;

   vpm_attached_stream_item_t       *stream_item;
   vss_ivocproc_rsp_attach_detach_stream_t   rsp_detach_stream;

   for (;;)
   {
      p_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      /* Look for this stream in the attached stream list and delete it. */
      pivot_node = &session_obj->attached_stream_list.dummy;

      for (strm_cntr = 0; strm_cntr < session_obj->attached_stream_list.size; strm_cntr++)
      {
         (void)apr_list_get_next(&session_obj->attached_stream_list,
                                 pivot_node, &cur_node);

         stream_item = (vpm_attached_stream_item_t *)cur_node;

         if (stream_item->addr == elite_apr_if_get_src_addr(p_apr_packet) &&
             stream_item->port == elite_apr_if_get_src_port(p_apr_packet))
         {
            (void)apr_list_delete(&(session_obj->attached_stream_list),
                                  cur_node);

            is_attached = TRUE;
            qurt_elite_memory_free(stream_item);

            break;
         }

         pivot_node = cur_node;
      }

      if (TRUE == is_attached)
      {
         /* Clear the ACK payload */
         memset(&rsp_detach_stream, 0, sizeof(rsp_detach_stream));

         /* Respond with VPM direction and  VMX in/out port masks */
         rsp_detach_stream.direction = session_obj->direction;

         if (session_obj->dyn_svc_info.mixer_in_port)
         {
            rsp_detach_stream.mixer_inport_mask = 1 << (session_obj->dyn_svc_info.mixer_in_port->inport_id);
         }

         if (session_obj->dyn_svc_info.mixer_out_port)
         {
            rsp_detach_stream.mixer_outport_mask = 1 << (session_obj->dyn_svc_info.mixer_out_port->outport_id);
         }

         MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_detach_stream_cmd_ctrl(): VMX inport mask: 0x%lX, VMX outport mask: 0x%lX, direction: %lu",
               rsp_detach_stream.mixer_inport_mask,
               rsp_detach_stream.mixer_outport_mask,
               rsp_detach_stream.direction);

         if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                          elite_apr_if_get_dst_addr(p_apr_packet),
                                                          elite_apr_if_get_dst_port(p_apr_packet),
                                                          elite_apr_if_get_src_addr(p_apr_packet),
                                                          elite_apr_if_get_src_port(p_apr_packet),
                                                          elite_apr_if_get_client_token(p_apr_packet),
                                                          VSS_IVOCPROC_RSP_DETACH_STREAM,
                                                          &rsp_detach_stream,
                                                          sizeof(rsp_detach_stream))))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_detach_stream_cmd_ctrl(): Failed to send ACK for detach stream cmd"
                  "result: 0x%lx", rc);

            /* End the APR command */
            result = rc;
            break;
         }

         /* Free incoming packet. */
         elite_apr_if_free(session_obj->self_apr_handle, p_apr_packet);

         return ADSP_EOK;

      }
      else
      {
         /* Respond with ADSP_EALREADY. */

         MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
               "vpm_detach_stream_cmd_ctrl(): stream with apr port (0x%lx)"
               "is not attached or already detached",
               elite_apr_if_get_src_port(p_apr_packet));

         result = ADSP_EALREADY;
      }

      break;
   } /* End of for (;;) */

   /* End the APR command */
   elite_apr_if_end_cmd(session_obj->self_apr_addr,
                        p_apr_packet, result);

   return result;
}

/* This function sets the total number of volume steps. When the number
 * of volume steps is greater than the number of indicies in the volume
 * calibration table, interpolation is required.
 */
static ADSPResult vpm_set_number_of_volume_steps_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   int32_t              rc;
   uint32_t             payload_size;
   elite_apr_packet_t   *p_apr_pkt;

   vss_ivolume_cmd_set_number_of_steps_t *in_args;

   for (;;)
   {
      p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

      /* Validate the inbound payload size. */
      if (payload_size != sizeof(vss_ivolume_cmd_set_number_of_steps_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_set_number_of_volume_steps_cmd_ctrl(): Unexpected size, "
               "%d != %d", payload_size, sizeof(vss_ivolume_cmd_set_number_of_steps_t));

         /* End the APR command */
         result = ADSP_EBADPARAM;
         break;
      }

      in_args = (vss_ivolume_cmd_set_number_of_steps_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

      if (TRUE == session_obj->volume_cal.is_registered)
      {
         /* BACKWARD COMPATIBILITY */
         /* Validate payload. */
         if (in_args->value < session_obj->volume_cal.num_vol_indices)
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_number_of_volume_steps_cmd_ctrl(): Invalid number of "
                  "volume steps %lu < min %lu", in_args->value,
                  session_obj->volume_cal.num_vol_indices);

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }
      }

      if (TRUE == session_obj->dynamic_cal.is_registered)
      {
         /* Validate payload. */
         if (in_args->value < session_obj->dynamic_cal.num_vol_indices)
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_number_of_volume_steps_cmd_ctrl(): Invalid number of "
                  "volume steps %d < min %d", in_args->value,
                  session_obj->dynamic_cal.num_vol_indices);

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }
      }

      session_obj->target_set.client_num_vol_steps = in_args->value;
      session_obj->is_client_set_num_vol_steps = TRUE;

      if (session_obj->active_set.client_num_vol_steps !=
             session_obj->target_set.client_num_vol_steps)
      {
         session_obj->volume_cal.is_calibrate_needed = TRUE; /* BACKWARD COMPATIBILITY */
         session_obj->dynamic_cal.is_calibrate_needed = TRUE;
         session_obj->is_vocproc_config_changed = TRUE;
      }

      break;
   } /* End of for (;;) */


   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}


/* This function sets the volume index. The volume index is used to indexing
 * into the registered volume/dynamic calibration table. The corresponding
 * entry in the table is used for volume calibration. Volume setting will be
 * applied immediately( i.e. ramp duration is 0 ) when the session is in RUN
 * state.
 */
static ADSPResult vpm_set_rx_vol_index_cmd_ctrl(vpm_session_object_t  *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   uint32_t             payload_size;

   vss_ivocproc_cmd_set_rx_volume_index_t *in_args;

   for (;;)
   {
      p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Validate the inbound payload size. */
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         if (payload_size != sizeof(vss_ivocproc_cmd_set_rx_volume_index_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_rx_vol_index_cmd_ctrl(): Unexpected size, %d != %d",
                  payload_size, sizeof(vss_ivocproc_cmd_set_rx_volume_index_t));

            /* End APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         in_args = (vss_ivocproc_cmd_set_rx_volume_index_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

         if (TRUE == session_obj->volume_cal.is_registered)
         {
            /* BACKWARD COMPATIBILITY */
            /* Validate payload. */
            if (in_args->vol_index >= session_obj->volume_cal.num_vol_indices)
            {
               MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_set_rx_vol_index_cmd_ctrl(): Unexpected "
                     "vol_index(%d) > MAX(%d)", in_args->vol_index,
                     session_obj->volume_cal.num_vol_indices);

               /* End APR command */
               result = ADSP_EBADPARAM;
               break;
            }
         }

         if (TRUE == session_obj->dynamic_cal.is_registered)
         {
            /* Validate payload. */
            if (in_args->vol_index >= session_obj->dynamic_cal.num_vol_indices)
            {
               MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_set_rx_vol_index_cmd_ctrl(): Unexpected "
                     "vol_index(%d) > MAX(%d)", in_args->vol_index,
                     session_obj->dynamic_cal.num_vol_indices);

               result = ADSP_EBADPARAM;
               break;
            }
         }

         session_obj->target_set.vol_step = in_args->vol_index;

         /* Check if calibration is required. */
         if (session_obj->target_set.vol_step == session_obj->active_set.vol_step)
         {
            /* No volume change. No calibration required. */
            result = ADSP_EALREADY;
            break;
         }

         /* If in RUN state, calibrate for volume. Note that changing the volume
          * level at RUN time will not cause additional module to be
          * enabled/disabled (confimred with VCP team). This is because by design,
          * each volume level in the volume calibration table uses the same set of
          * module enable/disable parameters, and volume level change only changes
          * the gain-dependent parameter data values. Therefore, there will not be
          * any KPPS requirement change due to volume level change.
          *
          * If in IDLE state, leave settings in the target_set. They will be
          * picked up when we go to RUN (either upon receiving a
          * VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG command if the vocproc is attached
          * to MVM, or upon IDLE to RUN transition if the vocproc is not attached
          * to MVM).
          */
         if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
         {
            if (TRUE == session_obj->volume_cal.is_registered)
            {
               /* BACKWARD COMPATIBILITY */
               if (ADSP_EOK != (result = vpm_calibrate_volume(session_obj, 0)))
               {
                  /* End the APR command */
                  result = ADSP_EFAILED;
                  break;
               }
               else
               {
                  /* Commands have been sent, wait for response. */

                  /* Set the command pending status to true */
                  vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);
                  return ADSP_EOK;
               }
            }
            else if (TRUE == session_obj->dynamic_cal.is_registered)
            {
               if (ADSP_EOK != (result = vpm_calibrate_dynamic(session_obj, 0, VPM_DYNAMIC_CAL_USECASE_GAIN)))
               {
                  /* End the APR command */
                  result = ADSP_EFAILED;
                  break;
               }
               else
               {
                  /* Set the command pending status to true and wait for set
                     param respones */
                  vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);
                  return ADSP_EOK;
               }
            }
            else
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                   "vpm_set_rx_vol_index_cmd_ctrl(): Calibration not registered.");

               /* End the APR command */
               result = ADSP_EFAILED;
               break;
            }
         }
         else
         {
            /* If VPM not in run state */
            session_obj->volume_cal.is_calibrate_needed = TRUE; /* BACKWARD COMPATIBILITY */
            session_obj->dynamic_cal.is_calibrate_needed = TRUE;
            session_obj->is_vocproc_config_changed = TRUE;

            /* End the APR command */
            break;
         }
      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Execution will reach here after getting all the
         set param responses from dyn service */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* If dynamic cal is registered */
      if (TRUE == session_obj->dynamic_cal.is_registered)
      {
         /* De-init calibration query */
         cvd_cal_query_deinit(session_obj->dynamic_cal.query_handle);
         session_obj->dynamic_cal.query_handle = CVD_CAL_INVALID_HANDLE_V;
      }

      /* Get the aggreagated response result for set param cmds */
      result = session_obj->session_rsp_ctrl.status;

      break;
   } /* End of for (;;) */

   /* End the APR command */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}

/* This function sets the volume step. When the number of volume
 * steps is greater than the number of indicies in the volume
 * calibration table, volume interpolation is required. Volume
 * setting will be applied gradually during the ramp_duration_ms.
 */
static ADSPResult vpm_set_volume_step_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   uint32_t             payload_size;

   vss_ivolume_cmd_set_step_t *in_args;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;


   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Volume/dynamic calibration table has not been registered yet. */
         if ((FALSE == session_obj->volume_cal.is_registered) &&
             (FALSE == session_obj->dynamic_cal.is_registered))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_set_volume_step_cmd_ctrl(): Volume or dynamic cal table is "
                "not registered yet.");

            /* End the APR command */
            result = ADSP_EFAILED;
            break;
         }

         /* Validate the inbound payload size. */
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         if (payload_size != sizeof(vss_ivolume_cmd_set_step_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_volume_step_cmd_ctrl(): Incorrect cmd payload size, %d != expected (%d)",
                  payload_size, sizeof(vss_ivolume_cmd_set_step_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* Get the cmd payload pointer */
         in_args = (vss_ivolume_cmd_set_step_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

         /* Validate payload. */
         if ((in_args->direction != VSS_IVOLUME_DIRECTION_RX) ||
             (in_args->value >= session_obj->target_set.client_num_vol_steps))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_volume_step_cmd_ctrl(): Bad parameter: Volume "
                  "direction = %d step number = %d", in_args->direction,
                  in_args->value);

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         session_obj->target_set.vol_step = in_args->value;

         /* Check if calibration is required. */
         if ((session_obj->target_set.vol_step == session_obj->active_set.vol_step) &&
             (session_obj->target_set.client_num_vol_steps == session_obj->active_set.client_num_vol_steps))
         {
            MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                "vpm_set_volume_step_cmd_ctrl(): No change in target vol step");

            /* No volume change. No calibration required. */
            result = ADSP_EALREADY;
            break;
         }

         /* If in RUN state, calibrate for volume. Note that changing the volume
          * level at RUN time will not cause additional module to be
          * enabled/disabled (confimred with VCP team). This is because by design,
          * each volume level in the volume calibration table uses the same set of
          * module enable/disable parameters, and volume level change only changes
          * the gain-dependent parameter data values. Therefore, there will not be
          * any KPPS requirement change due to volume level change.
          *
          * If in IDLE state, leave settings in the target_set. They will be
          * picked up when we go to RUN (either upon receiving a
          * VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG command if the vocproc is attached
          * to MVM, or upon IDLE to RUN transition if the vocproc is not attached
          * to MVM).
          */
         if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
         {
            if (TRUE == session_obj->volume_cal.is_registered)
            {
               /* BACKWARD COMPATIBILITY */
               if (ADSP_EOK != (result = vpm_calibrate_volume(session_obj,
                                                              in_args->ramp_duration_ms)))
               {
                  /* End the APR command */
                  result = ADSP_EFAILED;
                  break;
               }
               else
               {
                  /* Commands have been sent, wait for response. */

                  /* Set the command pending status */
                  vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

                  /* Return and wait for cmd rsps from dyn svcs for vol cal */
                  return result;
               }
            }
            else if (TRUE == session_obj->dynamic_cal.is_registered)
            {
               if (ADSP_EOK != (result = vpm_calibrate_dynamic(session_obj,
                                                               in_args->ramp_duration_ms,
                                                               VPM_DYNAMIC_CAL_USECASE_GAIN)))
               {
                  /* End the APR command */
                  result = ADSP_EFAILED;
                  break;
               }
               else
               {
                  /* Commands have been sent, wait for response. */

                  /* Set the command pending status */
                  vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

                  /* Return and wait for cmd rsps from dyn svcs for vol cal */
                  return result;
               }
            }
         }
         else /* VPM not in RUN state, nothing to do */
         {
            session_obj->volume_cal.is_calibrate_needed = TRUE; /* BACKWARD COMPATIBILITY */
            session_obj->dynamic_cal.is_calibrate_needed = TRUE;
            session_obj->is_vocproc_config_changed = TRUE;

            /* End the APR command */
            result = ADSP_EOK;
            break;
         }
      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after receiving resps for all vol cal commands */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      if (TRUE == session_obj->dynamic_cal.is_registered)
      {
         cvd_cal_query_deinit(session_obj->dynamic_cal.query_handle);
         session_obj->dynamic_cal.query_handle = CVD_CAL_INVALID_HANDLE_V;
      }

      /* Get the aggregated rsp for set param commands */
      result = session_obj->session_rsp_ctrl.status;

      break;
   } /* End of for (;;) */

   /* End the APR command */
   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

/* This function applies mute setting immediately( i.e. ramp duration is 0 ). */
static ADSPResult vpm_set_mute_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;
   uint32_t             payload_size;
   voice_mute_path_t    mute_mode;

   vss_ivocproc_cmd_set_mute_t *in_args;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Validate the inbound payload size. */

         in_args = (vss_ivocproc_cmd_set_mute_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         if (payload_size != sizeof(vss_ivocproc_cmd_set_mute_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_mute_cmd_ctrl(): Unexpected params, direction: %d, "
                  "mute_flag: %d", in_args->direction, in_args->mute_flag);

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* Validate payload content. */
         if ((in_args->direction != VSS_IVOCPROC_DIRECTION_TX &&
              in_args->direction != VSS_IVOCPROC_DIRECTION_RX &&
              in_args->direction != VSS_IVOCPROC_DIRECTION_RX_TX) ||
             (in_args->mute_flag != VPM_MUTE_ENABLE &&
              in_args->mute_flag != VPM_MUTE_DISABLE))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_mute_cmd_ctrl(): Unexpected params, direction: %d, "
                  "mute_flag: %d", in_args->direction, in_args->mute_flag);

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         switch (in_args->direction)
         {
           case VSS_IVOCPROC_DIRECTION_TX:
           {
             session_obj->target_set.client_tx_mute = ((bool_t)in_args->mute_flag);
             break;
           }
           case VSS_IVOCPROC_DIRECTION_RX:
           {
             session_obj->target_set.client_rx_mute = ((bool_t)in_args->mute_flag);
             break;
           }
           case VSS_IVOCPROC_DIRECTION_RX_TX:
           {
             session_obj->target_set.client_tx_mute = ((bool_t)in_args->mute_flag);
             session_obj->target_set.client_rx_mute = ((bool_t)in_args->mute_flag);
             break;
           }
           default:
           {
              /* Direction already validated above, end the APR cmd however */
             result = ADSP_EFAILED;
             break;
           }
         }

         /* Check if there is any change in the client/mvm mute settings */
         if ( (session_obj->active_set.client_tx_mute == session_obj->target_set.client_tx_mute) &&
             (session_obj->active_set.client_rx_mute == session_obj->target_set.client_rx_mute))
         {

            MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                "vpm_set_mute_cmd_ctrl(): No change in mute settings");

            /* No change in mute settings, end the APR command */
            result = ADSP_EOK;
            break;
         }

         session_obj->target_set.mute_ramp_duration = 0;

         /* If in RUN state, set mute on dynamic services
          * If in IDLE state, leave settings in the target_set.
          * They will be picked up when state transitions to RUN. */
         if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
         {
            if (ADSP_EOK == (result = vpm_get_mute_mode(session_obj, &mute_mode)))
            {
               /* Send mute cmd to dynamic services */
               if (ADSP_EOK == (result = vpm_action_set_mute(session_obj,
                                                             session_obj->target_set.mute_ramp_duration,
                                                             mute_mode)))
               {
                  /* Set the command pending status */
                  vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

                  /* Wait for custom msg response */
                  return ADSP_EOK;
               }
               else
               {
                  MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                        "vpm_set_mute_cmd_ctrl(): Failed to send mute cmd to dyn svcs, result: 0x%lx",
                        result);

                  /* End the cmd with failure status */
                  break;
               }
            }
            else
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_set_mute_cmd_ctrl(): Failed to determine mute mode, result: 0x%lx",
                     result);

               /* If mute mode cannot be determined, end the APR command */
               break;
            }
         }
         else
         {
            /* Nothing to do, end the APR command */
            break;
         }

      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here if at least 1 msg was sent to dynamic service and
         response has been recieved for set mute cmds */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Get the aggregated resposne status */
      result = session_obj->session_rsp_ctrl.status;;

      /* Check if at least 1 set param response returned SUCCESS */
      if (ADSP_EOK == session_obj->session_rsp_ctrl.status)
      {
         /* Update active_set for mute settings  */
         session_obj->active_set.client_tx_mute = session_obj->target_set.client_tx_mute;
         session_obj->active_set.client_rx_mute = session_obj->target_set.client_rx_mute;
         session_obj->active_set.mute_ramp_duration = session_obj->target_set.mute_ramp_duration;
      }

      break;

   } /* End of for (;;) */

   /* End the APR command */
   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

/* This function applies mute setting gradually over the specified ramp duration. */
static ADSPResult vpm_volume_mute_v2_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   uint32_t             payload_size;
   voice_mute_path_t    mute_mode;
   vss_icommon_evt_voice_activity_update_t vactivity;
   vss_ivolume_cmd_mute_v2_t *in_args;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Validate the inbound payload size. */
         in_args = (vss_ivolume_cmd_mute_v2_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         if (payload_size != sizeof(vss_ivolume_cmd_mute_v2_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_volume_mute_v2_cmd_ctrl(): Incorrect payload size: %lu != expected (%lu)",
                  payload_size, sizeof(vss_ivolume_cmd_mute_v2_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* Validate payload content. */
         if (((in_args->direction != VSS_IVOLUME_DIRECTION_TX) &&
              (in_args->direction != VSS_IVOLUME_DIRECTION_RX)) ||
             ((in_args->mute_flag != VSS_IVOLUME_MUTE_OFF) &&
              (in_args->mute_flag != VSS_IVOLUME_MUTE_ON)))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_volume_mute_v2_cmd_ctrl(): Unexpected params, "
                  " direction: %d, mute_flag: %d", in_args->direction,
                  in_args->mute_flag);

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* Check if the cmd issued by MVM */
         switch (in_args->direction)
         {
           case VSS_IVOLUME_DIRECTION_TX:
           {
             session_obj->target_set.client_tx_mute = ((bool_t)in_args->mute_flag);
             break;
           }
           case VSS_IVOLUME_DIRECTION_RX:
           {
             session_obj->target_set.client_rx_mute = ((bool_t)in_args->mute_flag);
             break;
           }
           default:
           {
             /* End the APR command */
             result = ADSP_EBADPARAM;
             break;
           }
         }

         /* Check if the mute settings changed */
         if ( (session_obj->active_set.client_tx_mute == session_obj->target_set.client_tx_mute) &&
             (session_obj->active_set.client_rx_mute == session_obj->target_set.client_rx_mute))
         {
            MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                "vpm_set_mute_cmd_ctrl(): No change in mute settings");

            /* End the APR command */
            result = ADSP_EOK;
            break;
         }

         session_obj->target_set.mute_ramp_duration = in_args->ramp_duration_ms;

         /* If in RUN state, set mute on dynamic services
          * If in IDLE state, leave settings in the target_set.
          * They will be picked up when state transitions to RUN. */
         if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
         {
            if (ADSP_EOK == (result = vpm_get_mute_mode(session_obj, &mute_mode)))
            {
               /* Send mute cmd to dynamic services */
               if (ADSP_EOK == (result = vpm_action_set_mute(session_obj,
                                                             session_obj->target_set.mute_ramp_duration,
                                                             mute_mode)))
               {
                  /* Set the command pending status */
                  vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

                  /* Wait for custom msg response */
                  return ADSP_EOK;
               }
               else
               {
                  MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                        "vpm_volume_mute_v2_cmd_ctrl(): Failed to send mute cmd to dyn svcs, result: 0x%lx",
                        result);

                  /* End the cmd with failure status */
                  break;
               }
            }
            else
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_volume_mute_v2_cmd_ctrl(): Failed to determine mute mode, result: 0x%lx",
                     result);

               /* If mute mode cannot be determined, end the command */
               break;
            }
         }
         else
         {
            /* Nothing to do, end the APR command */
            break;
         }
      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here if at least 1 msg was sent to dynamic service and
         response has been recieved for set mute cmds */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Get the aggregated resposne status */
      result = session_obj->session_rsp_ctrl.status;;

      /* Check if at least 1 set param response returned SUCCESS */
      if (ADSP_EOK == session_obj->session_rsp_ctrl.status)
      {
         /* Update active_set for mute settings  */
         session_obj->active_set.client_tx_mute = session_obj->target_set.client_tx_mute;
         session_obj->active_set.client_rx_mute = session_obj->target_set.client_rx_mute;
         session_obj->active_set.mute_ramp_duration = session_obj->target_set.mute_ramp_duration;
      }

      {
        /* Send notification of UI MUTE/UNMUTE voice activity to MVM. */
        if( TRUE == session_obj->target_set.client_tx_mute )
        {
          vactivity.activity = VSS_ICOMMON_VOICE_ACTIVITY_UI_VOCPROC_TX_MUTE;
        }
        else
        {
          vactivity.activity = VSS_ICOMMON_VOICE_ACTIVITY_UI_VOCPROC_TX_UNMUTE;
        }

        result = elite_apr_if_alloc_send_event(
                   session_obj->self_apr_handle,
                   session_obj->self_apr_addr, session_obj->self_apr_port,
                   session_obj->mvm_apr_addr, session_obj->attached_mvm_handle,
                   0, VSS_ICOMMON_EVT_VOICE_ACTIVITY_UPDATE,
                   &vactivity, sizeof( vactivity ) );
        if ( ADSP_EOK != result )
        {
          MSG_1( MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_volume_mute_v2_cmd_ctrl(): Failed to send"
                 " VSS_ICOMMON_EVT_VOICE_ACTIVITY_UPDATE to MVM result: 0x%lx", result );
        }
      }

      break;

   } /* End of for (;;) */

   /* End the APR command */
   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_vpcm_start_v2_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;
   uint32_t             num_tap_points;
   uint32_t             payload_len;
   uint32_t             expected_payload_len;
   uint32_t             tap_point_index;
   uint32_t             target_payload_len;
   uint32_t             cvd_mem_handle;
   uint32_t             vpm_mem_handle;
   elite_apr_packet_t   *p_dst_apr_packet = NULL;
   uint8_t              *p_dst_payload;
   uint32_t             dst_payload_rem_size;

   vss_ivpcm_cmd_start_v2_t   *payload;
   vss_ivpcm_tap_point_t      *p_vpcm_tap_points;
   voice_tap_point_v2_t       dst_tap_point;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Only one START command is allowed. */
         if (TRUE == session_obj->vpcm_info.is_enabled)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_vpcm_start_v2_cmd_ctrl(): VPCM already started");

            /* End the APR command */
            result = ADSP_EALREADY;
            break;
         }

         /* Get the APR payload pointer and size */
         payload = (vss_ivpcm_cmd_start_v2_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_len = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate inbound payload size */
         if (payload_len < sizeof(vss_ivpcm_cmd_start_v2_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_start_v2_cmd_ctrl(): Unexpected payload size, %d < %d",
                  payload_len, sizeof(vss_ivpcm_cmd_start_v2_t));

            result = ADSP_EBADPARAM;
            break;
         }

         num_tap_points = payload->num_tap_points;

         if (0 == num_tap_points)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_vpcm_start_v2_cmd_ctrl(): Invalid num tap points(0)");

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* expected_payload_len = ( sizeof( mem_handle ) + sizeof( num_tap_points ) +
          *                        ( num_tap_points * sizeof( each tap point ) ) ).
          */
         expected_payload_len = (sizeof(uint32_t) + sizeof(uint32_t) +
                                 (num_tap_points * sizeof(vss_ivpcm_tap_point_t)));

         if (payload_len != expected_payload_len)
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_start_v2_cmd_ctrl(): Invalid data. Payload len: %d, "
                  "expected len: %d", payload_len, expected_payload_len);

            result = ADSP_EBADPARAM;
            break;
         }

         cvd_mem_handle = payload->mem_handle;

         rc = cvd_mem_mapper_validate_handle(cvd_mem_handle);
         if (rc)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_start_v2_cmd_ctrl(): Invalid mem_handle: 0x%08X",
                  cvd_mem_handle);

            result = ADSP_EHANDLE;
            break;
         }

         (void)cvd_mem_mapper_get_vpm_mem_handle(cvd_mem_handle, &vpm_mem_handle);

         p_vpcm_tap_points = ((vss_ivpcm_tap_point_t *)
                              (((uint8_t *)payload) + sizeof(vss_ivpcm_cmd_start_v2_t)));

         /* target_payload_len = ( sizeof( num_tap_points ) +
                                           ( num_tap_points * sizeof( each tap point ) ) ). */
         target_payload_len = (sizeof(uint32_t) +
                               (num_tap_points * sizeof(voice_tap_point_v2_t)));

         if (APR_EOK != (rc = elite_apr_if_alloc_cmd(session_obj->self_apr_handle,
                                                     session_obj->self_apr_addr,
                                                     session_obj->self_apr_port,
                                                     0  /* Dstn addr don't care */,
                                                     0  /* Dstn port don't care*/,
                                                     0  /* Token, dont care */,
                                                     VOICE_CMD_START_HOST_PCM_V2,
                                                     target_payload_len,
                                                     &p_dst_apr_packet)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_start_v2_cmd_ctrl(): Failed to allocate APR packet for sending"
                  "vpcm start to dyn svcs, result: 0x%lx ", rc);

            /* End the APR command */
            result = rc;
            break;
         }
         else if (NULL == p_dst_apr_packet)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_vpcm_start_v2_cmd_ctrl(): Allocated APR packet pointer is NULL");

            result = ADSP_EFAILED;
            break;
         }

         /* Get the APR payload pointer and size of newly allocated packet */
         p_dst_payload = (uint8_t *)elite_apr_if_get_payload_ptr(p_dst_apr_packet);
         dst_payload_rem_size = elite_apr_if_get_payload_size(p_dst_apr_packet);

         /* Copy number of tap points. */
         memscpy(p_dst_payload, dst_payload_rem_size,
                 &num_tap_points, sizeof(num_tap_points));

         /* Decrement the remaining payload size */
         dst_payload_rem_size -= sizeof(num_tap_points);

         /* Advance the dstn payload pointer */
         p_dst_payload += sizeof(num_tap_points);

         for (tap_point_index = 0; tap_point_index < num_tap_points; ++tap_point_index)
         {
            switch (p_vpcm_tap_points->tap_point)
            {
               case VSS_IVPCM_TAP_POINT_TX_DEFAULT:
               {
                  dst_tap_point.tap_point = VOICEPROC_MODULE_TX;
                  break;
               }
               case VSS_IVPCM_TAP_POINT_RX_DEFAULT:
               {
                  dst_tap_point.tap_point = VOICEPROC_MODULE_RX;
                  break;
               }
               default:
               {
                  MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                        "vpm_vpcm_start_v2_cmd_ctrl(): Invalid tap point: 0x%8X",
                        p_vpcm_tap_points->tap_point);

                  result = ADSP_EBADPARAM;

                  goto __bailout_vpcm_start_v2_cmd_ctrl;
               }
            }

            switch (p_vpcm_tap_points->direction)
            {
               case VSS_IVPCM_TAP_POINT_DIR_OUT:
               {
                  dst_tap_point.direction = VOICE_HOST_PCM_READ;
                  break;
               }
               case VSS_IVPCM_TAP_POINT_DIR_IN:
               {
                  dst_tap_point.direction = VOICE_HOST_PCM_WRITE;
                  break;
               }
               case VSS_IVPCM_TAP_POINT_DIR_OUT_IN:
               {
                  dst_tap_point.direction = (VOICE_HOST_PCM_READ | VOICE_HOST_PCM_WRITE);
                  break;
               }
               default:
               {
                  MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                        "vpm_vpcm_start_v2_cmd_ctrl(): Invalid dir: %d",
                        p_vpcm_tap_points->direction);

                  result = ADSP_EBADPARAM;

                  goto __bailout_vpcm_start_v2_cmd_ctrl;
               }
            }

            dst_tap_point.sampling_rate = p_vpcm_tap_points->sampling_rate;
            dst_tap_point.duration_ms = p_vpcm_tap_points->duration;
            dst_tap_point.reserved = 0;
            dst_tap_point.mem_map_handle = cvd_mem_handle;

            /* Copy each tap point structure to destination payload. */
            memscpy(p_dst_payload, dst_payload_rem_size,
                    &dst_tap_point, sizeof(dst_tap_point));

            /* Decrement the rem size */
            dst_payload_rem_size -= sizeof(dst_tap_point);

            /* Advance the dstn payload pointer */
            p_dst_payload += sizeof(dst_tap_point);

            /* Advance to the next tap point. */
            p_vpcm_tap_points += 1;

         } /* End of for loop (tap_point_index) */


         /* Set the command pending status */
         vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

         /* Set the command rsp pending status */
         vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

         /* Save the allocated APR packet info in response ctrl */
         session_obj->session_rsp_ctrl.p_ack_payload = (void *)p_dst_apr_packet;
         session_obj->session_rsp_ctrl.is_ack_payload_type_apr = TRUE;

         /* Send custom msg to dyn service to start host PCM */
         if (ADSP_EOK != (result = vpm_send_vcpm_cfg_msg_to_dyn_svc(session_obj,
                                                                    vpm_mem_handle,
                                                                    p_dst_apr_packet)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_start_v2_cmd_ctrl(): Failed to send start cmd to dyn svcs, result; 0x%lx",
                  result);

            /* Free up the allocated APR packet */
            elite_apr_if_free(session_obj->self_apr_handle, p_dst_apr_packet);

            /* Clear the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            /* Clear the command rsp pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

            break;
         }

         /* Return and wait for custom msg responses */
         return ADSP_EOK;

      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */


      /* Control will reach here after receving all the responses */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Get the aggregated cust msg response status */
      result = session_obj->session_rsp_ctrl.status;

      /* Check if at least 1 response returned success */
      if (ADSP_EOK == session_obj->session_rsp_ctrl.status)
      {
         /* Save VPCM info in session object */
         session_obj->vpcm_info.client_addr = elite_apr_if_get_src_addr(p_apr_pkt);
         session_obj->vpcm_info.client_handle = elite_apr_if_get_src_port(p_apr_pkt);
         session_obj->vpcm_info.is_enabled = TRUE;

         payload = (vss_ivpcm_cmd_start_v2_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

         session_obj->vpcm_info.mem_handle = payload->mem_handle;
      }

      /* End the APR command */
      elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

      return result;

   } /* End of for (;;) */

__bailout_vpcm_start_v2_cmd_ctrl:

   if (p_dst_apr_packet)
   {
      /* Free up the allocated APR packet for dyn service */
      elite_apr_if_free(session_obj->self_apr_handle, p_dst_apr_packet);
   }

   /* End the APR command, if there is any error */
   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_vpcm_stop_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_dst_apr_packet = NULL;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Only one STOP command is allowed. */
         if (FALSE == session_obj->vpcm_info.is_enabled)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_vpcm_stop_cmd_ctrl(): VPCM not started");

            result = ADSP_EALREADY;
            break;
         }

         if (APR_EOK != (rc = elite_apr_if_alloc_cmd(session_obj->self_apr_handle,
                                                     session_obj->self_apr_addr,
                                                     session_obj->self_apr_port,
                                                     0  /* Dstn addr don't care */,
                                                     0  /* Dstn port don't care*/,
                                                     0  /* Token, dont care */,
                                                     VOICE_CMD_STOP_HOST_PCM,
                                                     0,
                                                     &p_dst_apr_packet)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_stop_cmd_ctrl(): Failed to allocate APR packet to send"
                  "stop vcpm cmd to dyn svcs, result; 0x%lx", rc);

            /* End the APR command */
            result = rc;
            break;
         }

         /* Set the command pending status */
         vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

         /* Set the command rsp pending status */
         vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

         /* Save the allocated APR packet info in response ctrl */
         session_obj->session_rsp_ctrl.p_ack_payload = (void *)p_dst_apr_packet;
         session_obj->session_rsp_ctrl.is_ack_payload_type_apr = TRUE;

         /* Send custom msg to dyn service to start host PCM */
         if (ADSP_EOK != (result = vpm_send_vcpm_cfg_msg_to_dyn_svc(session_obj,
                                                                    0,
                                                                    p_dst_apr_packet)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_start_v2_cmd_ctrl(): Failed to send start cmd to dyn svcs,"
                  " result; 0x%lx", result);

            /* Free up allocated APR packet */
            elite_apr_if_free(session_obj->self_apr_handle, p_dst_apr_packet);

            /* Clear the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            /* Clear the command rsp pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

            /* End the APR command */
            break;
         }

         /* Return and wait for custom msg responses */
         return ADSP_EOK;

      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after receving all the responses */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      result = session_obj->session_rsp_ctrl.status;

      /* Check if at least 1 response returned success */
      if (ADSP_EOK == result)
      {
         /* Save VPCM info in session object */
         session_obj->vpcm_info.client_addr = 0;
         session_obj->vpcm_info.client_handle = 0;
         session_obj->vpcm_info.is_enabled = FALSE;
         session_obj->vpcm_info.mem_handle = 0;
      }

      /* End the APR command */
      elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

      return result;

   } /* End of for (;;) */

   /* End APR cmd if any error occurs */
   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

/* BACKWARD COMPATIBILITY */
static ADSPResult vpm_register_calibration_data_v2_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   int32_t              rc;
   uint32_t             payload_len;
   cvd_virt_addr_t      cal_virt_addr;
   uint16_t             client_addr;
   elite_apr_packet_t   *p_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   cvd_cal_table_descriptor_t cal_table_descriptor;
   vss_ivocproc_cmd_register_calibration_data_v2_t *payload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (TRUE == session_obj->common_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_calibration_data_v2_cmd_ctrl(): Calibration data "
             "already registered");

         /* End the APR command */
         result = ADSP_EALREADY;
         break;
      }

      if (TRUE == session_obj->static_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_calibration_data_v2_cmd_ctrl(): Calibration already "
             "registered via static cal registration command.");

         /* End the APR command */
         result = ADSP_EALREADY;
         break;
      }

      if ((TRUE == session_obj->is_device_config_registered) &&
          (session_obj->hdvoice_config_info.hdvoice_config_hdr.sys_config_list_head != NULL))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_calibration_data_v2_cmd_ctrl(): Feature Configuration "
             "Module in Device Config is incompatible. Removing Feature Config Data.");

         (void)vpm_clear_feature_config(session_obj);
      }

      /* Get the APR payload pointer and size */
      payload = (vss_ivocproc_cmd_register_calibration_data_v2_t *)elite_apr_if_get_payload_ptr(p_apr_packet);
      payload_len = elite_apr_if_get_payload_size(p_apr_packet);

      if (payload_len < sizeof(vss_ivocproc_cmd_register_calibration_data_v2_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_register_calibration_data_v2_cmd_ctrl(): Unexpected payload "
               "size, %d !< %d", payload_len,
               sizeof(vss_ivocproc_cmd_register_calibration_data_v2_t));

         result = ADSP_EBADPARAM;
         break;
      }

      if (ADSP_EOK != (result = vpm_validate_calib_payload(payload->cal_mem_handle,
                                                           payload->cal_mem_size,
                                                           payload->cal_mem_address)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_register_calibration_data_v2_cmd_ctrl(): Failed to validate calib payload:"
               " result: 0x%lx", result);

         /* End the APR command */
         break;
      }

      rc = cvd_mem_mapper_get_virtual_addr_v2(payload->cal_mem_handle,
                                              payload->cal_mem_address,
                                              &cal_virt_addr);
      if (rc)
      {
         MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_register_calibration_data_v2_cmd_ctrl(): Cannot get virtual address,"
               " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->cal_mem_handle,
               (uint32_t)payload->cal_mem_address, (uint32_t)(payload->cal_mem_address >> 32));

         result = ADSP_EFAILED;
         break;
      }
      else
      {
         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_register_calibration_data_v2_cmd_ctrl(): cal memory virtual address 0x%016X, size %d",
               cal_virt_addr.ptr, payload->cal_mem_size);
      }

      (void)cvd_mem_mapper_get_vpm_mem_handle(payload->cal_mem_handle,
                                              &session_obj->common_cal.vpm_mem_handle);

      /* Invalidate the cache before reading cal data from shared mem */
      (void)cvd_mem_mapper_cache_invalidate_v2(&cal_virt_addr, payload->cal_mem_size);

      cal_table_descriptor.start_ptr = cal_virt_addr.ptr;
      cal_table_descriptor.size = payload->cal_mem_size;
      cal_table_descriptor.data_mem_handle = payload->cal_mem_handle;
      cal_table_descriptor.indexing_mem_handle = APR_NULL_V;
      cal_table_descriptor.num_columns = payload->num_columns;
      cal_table_descriptor.columns = ((cvd_cal_column_descriptor_t *)
                                      (((uint8_t *)payload) +
                                       sizeof(vss_ivocproc_cmd_register_calibration_data_v2_t)));

      rc = cvd_cal_parse_table(0, 0, &cal_table_descriptor,
                               &session_obj->common_cal.table_handle);
      if (rc)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_calibration_data_v2_cmd_ctrl(): Error in parsing calibration table");

         result = ADSP_EFAILED;
         break;
      }

      session_obj->common_cal.is_registered = TRUE;
      session_obj->common_cal.is_calibrate_needed = TRUE;

      session_obj->is_vocproc_config_changed = TRUE;

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG: vpm_register_calibration_data_v2_cmd_ctrl() SUCCESS: Table size reported: %d",
            cal_table_descriptor.size);

      break;
   } /*End of for (;;) */

   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_packet, result);

   return result;
}

/* BACKWARD COMPATIBILITY */
static ADSPResult vpm_deregister_calibration_data_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   int32_t              rc;
   elite_apr_packet_t   *p_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      if (FALSE == session_obj->common_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_deregister_calibration_data_cmd_ctrl(): Calibration not registered");

         result = ADSP_EALREADY;
         break;
      }

      if (APR_EOK != (rc = cvd_cal_discard_table(session_obj->common_cal.table_handle)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_deregister_calibration_data_cmd_ctrl(): Failed to deregister calibration data");

         result = ADSP_EFAILED;
         break;
      }

      /* Clear the common calib flags */
      session_obj->common_cal.is_registered = FALSE;
      session_obj->common_cal.is_calibrate_needed = FALSE;

      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_deregister_calibration_data_cmd_ctrl(): Successfully deregistered cal data");

      break;
   } /* End of  for (;;) */

   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_packet, result);
   return result;
}

/* BACKWARD COMPATIBILITY */
static ADSPResult vpm_register_volume_calibration_data_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   int32_t              rc;
   cvd_virt_addr_t      cal_virt_addr;
   uint32_t             min_vol_index;
   uint32_t             max_vol_index;
   uint32_t             checkpoint = 0;
   uint16_t             client_addr;
   uint32_t             payload_len;
   elite_apr_packet_t   *p_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   vss_ivocproc_cmd_register_volume_calibration_data_t *payload;
   cvd_cal_table_descriptor_t cal_table_descriptor;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         if (TRUE == session_obj->volume_cal.is_registered)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_register_volume_calibration_data_cmd_ctrl(): Calibration already registered");

            result = ADSP_EALREADY;
            break;
         }

         if (TRUE == session_obj->dynamic_cal.is_registered)
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_register_volume_calibration_data_cmd_ctrl(): Calibration already registered via dynamic cal registration command.");

            result = ADSP_EALREADY;
            break;
         }

         if ((TRUE == session_obj->is_device_config_registered) &&
             (NULL != session_obj->hdvoice_config_info.hdvoice_config_hdr.sys_config_list_head))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_register_volume_calibration_data_cmd_ctrl(): Feature Configuration Module in Device Config is incompatible."
                " Removing Feature Config Data.");

            (void)vpm_clear_feature_config(session_obj);
         }

         /* Get the APR payload pointer and payload size */
         payload = (vss_ivocproc_cmd_register_volume_calibration_data_t *)\
            elite_apr_if_get_payload_ptr(p_apr_packet);

         payload_len = elite_apr_if_get_payload_size(p_apr_packet);

         if (payload_len < sizeof(vss_ivocproc_cmd_register_volume_calibration_data_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_register_volume_calibration_data_cmd_ctrl(): Unexpected payload size, %d < expected: %d",
                  payload_len, sizeof(vss_ivocproc_cmd_register_volume_calibration_data_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         if (ADSP_EOK != (result = vpm_validate_calib_payload(payload->cal_mem_handle,
                                                              payload->cal_mem_size,
                                                              payload->cal_mem_address)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_register_volume_calibration_data_cmd_ctrl(): Failed to validate calib payload:"
                  " result: 0x%lx", result);

            /* End the APR command */
            break;
         }

         if (APR_EOK != (rc = cvd_mem_mapper_get_virtual_addr_v2(payload->cal_mem_handle,
                                                                 payload->cal_mem_address,
                                                                 &cal_virt_addr)))
         {
            MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_register_volume_calibration_data_cmd_ctrl(): Cannot get virtual address,"
                  " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->cal_mem_handle,
                  (uint32_t)payload->cal_mem_address, (uint32_t)(payload->cal_mem_address >> 32));

            result = ADSP_EFAILED;
            break;
         }

         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_register_volume_calibration_data_cmd_ctrl(): cal memory virtual address 0x%016X, size %d",
               cal_virt_addr.ptr, payload->cal_mem_size);

         (void)cvd_mem_mapper_get_vpm_mem_handle(payload->cal_mem_handle,
                                                 &session_obj->volume_cal.vpm_mem_handle);

         /* Invalidate the cache before reading data from shared meme */
         (void)cvd_mem_mapper_cache_invalidate_v2(&cal_virt_addr, payload->cal_mem_size);

         cal_table_descriptor.start_ptr = cal_virt_addr.ptr;
         cal_table_descriptor.size = payload->cal_mem_size;
         cal_table_descriptor.data_mem_handle = payload->cal_mem_handle;
         cal_table_descriptor.indexing_mem_handle = APR_NULL_V;
         cal_table_descriptor.num_columns = payload->num_columns;
         cal_table_descriptor.columns = ((cvd_cal_column_descriptor_t *)
                                         (((uint8_t *)payload) +
                                          sizeof(vss_ivocproc_cmd_register_volume_calibration_data_t)));

         if (APR_EOK != (rc = cvd_cal_parse_table(0, 0, &cal_table_descriptor,
                                                  &session_obj->volume_cal.table_handle)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_register_volume_calibration_data_cmd_ctrl(): Error in parsing calibration table");

            result = ADSP_EFAILED;
            break;
         }

         checkpoint = 1;

         /* Determine volume indices. */
         /* Assumption: same volume indices for all networks. */
         if (APR_EOK != (rc = cvd_cal_find_min_max_column_value(session_obj->volume_cal.table_handle,
                                                                VSS_ICOMMON_CAL_COLUMN_VOLUME_INDEX,
                                                                &min_vol_index,
                                                                &max_vol_index)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_register_volume_calibration_data_cmd_ctrl(): "\
                   "Cannot find min/max volume indices");

            result = ADSP_EBADPARAM;
            break;
         }

         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_register_volume_calibration_data_cmd_ctrl(): "\
                  "min volume index %d, max volume index %d",
               min_vol_index, max_vol_index);

         session_obj->volume_cal.min_vol_index = min_vol_index;
         session_obj->volume_cal.max_vol_index = max_vol_index;
         session_obj->volume_cal.num_vol_indices = (max_vol_index - min_vol_index + 1);

         if (FALSE == session_obj->is_client_set_num_vol_steps)
         {
            session_obj->target_set.client_num_vol_steps = session_obj->volume_cal.num_vol_indices;
         }

         if (ADSP_EOK != (result = vpm_find_vol_table_format(session_obj,
                                                             &session_obj->volume_cal.is_v1_format)))
         {
            MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                "vpm_register_volume_calibration_data_cmd_ctrl(): Cannot find table format info");

            session_obj->volume_cal.is_v1_format = FALSE; /* Assume v2 version if not able to determin. */

            result = ADSP_EOK;
         }

         session_obj->volume_cal.is_registered = TRUE;

         MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
               "CVD_CAL_MSG: vpm_register_volume_calibration_data_cmd_ctrl(): Successfully registered, Table size reported: %d",
               cal_table_descriptor.size);

         checkpoint = 2;

         if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
         {
            if (ADSP_EOK == (result = vpm_calibrate_volume(session_obj, 0)))
            {
               /* Set the command status to pending. Remove the session cmdQ
                   from service wait mask. Wait for all the responses */
               vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

               return ADSP_EOK;
            }
            else
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                   "vpm_register_volume_calibration_data_cmd_ctrl(): Failed to apply volume calib");

               /* End the APR command */
               break;
            }
         }
         else /* VPM not running */
         {
            MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                "vpm_register_volume_calibration_data_cmd_ctrl(): volume not applied,"
                "VPM not in RUN state");

            session_obj->volume_cal.is_calibrate_needed = TRUE;
            session_obj->is_vocproc_config_changed = TRUE;

            /* End the APR command */
            elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_packet, ADSP_EOK);
            return ADSP_EOK;
         }

      } /* if prev command pending*/

      /* Control will reach here after receiving all the responses
          if vol calib was applied */

      /* Remove the command pending status, as it is completed */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* End the APR command with aggregated response results */
      result = session_obj->session_rsp_ctrl.status;

      /* End the APR command */
      elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_packet, result);

      return result;

   } /* for (;;)*/

   /* Executing will reach here in case of any intermediate failures */
   switch (checkpoint)
   {
      case 2:
      {
         session_obj->volume_cal.is_registered = FALSE;
         /*-fallthru */
      }
      case 1:
      {
         rc = cvd_cal_discard_table(session_obj->volume_cal.table_handle);
      } /*-fallthru */
      default:
      {
         elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_packet, result);
         break;
      }
   }

   return result;
}

/* BACKWARD COMPATIBILITY */
static ADSPResult vpm_deregister_volume_calibration_data_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;


   for (;;)
   {
      p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      if (FALSE == session_obj->volume_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_deregister_volume_calibration_data_cmd_ctrl(): Calibration not registered");

         result = ADSP_EALREADY;
         break;
      }

      if (APR_EOK != (rc = cvd_cal_discard_table(session_obj->volume_cal.table_handle)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_deregister_volume_calibration_data_cmd_ctrl(): Failed to deregister calibration data");

         result = ADSP_EFAILED;
         break;
      }

      session_obj->volume_cal.is_registered = FALSE;
      session_obj->volume_cal.is_calibrate_needed = FALSE;

      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_deregister_volume_calibration_data_cmd_ctrl(): Successfully deregistered");

      break;
   } /* End of for (;;) */

   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}

/* Register OOB topology parameters.
 */
static ADSPResult vpm_register_topology_params_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   int32_t               rc;
   uint32_t              payload_len;
   uint16_t              client_addr;
   cvd_virt_addr_t       vir_addr;
   elite_apr_packet_t    *p_apr_pkt;

   vss_ivocproc_cmd_topology_register_params_t  *payload_ptr;

   for (;;)
   {
      p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      if (!p_apr_pkt)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_topology_params_cmd_ctrl(): NULL APR packet pointer");

         result = ADSP_EFAILED;
         break;
      }

      if (TRUE == session_obj->topo_param.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_topology_params_cmd_ctrl(): topology params already registered.");

         result = ADSP_EALREADY;
      }
      else if (VPM_TOPOLOGY_COMMIT_STATE_NONE == session_obj->topo_commit_state)
      {
         /* Registering topology params is not allowed after commit. */
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_register_topology_params_cmd_ctrl(): not allowed.");

         result = ADSP_EFAILED;
      }

      if (ADSP_EOK != result)
      {
         /* End the APR command if any error caught in above conditionals */
         break;
      }

      /* Get the APR payload pointer and size */
      payload_ptr = (vss_ivocproc_cmd_topology_register_params_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
      payload_len = elite_apr_if_get_payload_size(p_apr_pkt);

      /* Validate the inbound payload size */
      if (payload_len < sizeof(vss_ivocproc_cmd_topology_register_params_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_register_topology_params_cmd_ctrl(): "
               "Unexpected payload size, %d < %d",
               payload_len,
               sizeof(vss_ivocproc_cmd_register_static_calibration_data_t));

         /* End the APR command */
         return ADSP_EBADPARAM;
      }

      /* Validate the calib payload */
      if (ADSP_EOK != (result = vpm_validate_calib_payload(payload_ptr->mem_handle,
                                                           payload_ptr->mem_size,
                                                           payload_ptr->mem_address)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_register_topology_params_cmd_ctrl(): Calib payload validation failed"
               "result: 0x%lx", result);

         /* End the APR command */
         break;
      }

      /* Get the virtual addrs of shared mem */
      if (ADSP_EOK != (result = cvd_mem_mapper_get_virtual_addr_v2(payload_ptr->mem_handle,
                                                                   payload_ptr->mem_address,
                                                                   &vir_addr)))
      {
         MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_register_topology_params_cmd_ctrl(): Cannot get virtual address,"
               " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload_ptr->mem_handle,
               (uint32_t)payload_ptr->mem_address, (uint32_t)(payload_ptr->mem_address >> 32));

         /* End the APR command */
         break;
      }
      else
      {
         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_register_topology_params_cmd_ctrl(): param memory virtual address 0x%016X, size %d",
               vir_addr.ptr, payload_ptr->mem_size);
      }

      /* Invalidate the cache before reading shared mem data */
      (void)cvd_mem_mapper_cache_invalidate_v2(&vir_addr, payload_ptr->mem_size);

      session_obj->topo_param.vir_addr = vir_addr.ptr;
      session_obj->topo_param.mem_size = payload_ptr->mem_size;

      if (ADSP_EOK != (result = vpm_extract_topology_params(session_obj, TRUE)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_topology_params_cmd_ctrl(): Invalid topology params");

         session_obj->topo_param.vir_addr = NULL;
         session_obj->topo_param.mem_size = 0;

         /* End the APR command */
         break;
      }

      session_obj->topo_param.is_registered = TRUE;

      MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
          "vpm_register_topology_params_cmd_ctrl(): Successfully registered");

      break;
   } /* End of for ( ;; ) */

   elite_apr_if_end_cmd(session_obj->self_apr_handle,
                        p_apr_pkt, result);

   return result;
}

/* Deregister OOB topology parameters.
 */
static ADSPResult vpm_deregister_topology_params_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;

   for (;;)
   {
      p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      if (FALSE == session_obj->topo_param.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_deregister_topology_params_cmd_ctrl(): topology params not registered.");

         result = ADSP_EALREADY;
         break;
      }

      session_obj->topo_param.is_registered = FALSE;
      session_obj->topo_param.mem_size = 0;
      session_obj->topo_param.vir_addr = NULL;

      MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
          "vpm_deregister_topology_params_cmd_ctrl(): topology params successfully de-registered.");

      break;
   }

   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_set_ui_property_cmd_processing(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;
   uint32_t             payload_size;
   bool_t               is_cached = FALSE;
   uint8_t              *cur_ui_prop_cache_slot;
   uint32_t             remaining_ui_prop_cache_len;
   uint8_t              *destination_ui_prop_cache_slot = NULL;
   uint8_t              *remaining_ui_prop;
   uint32_t             remaining_ui_prop_len;
   uint32_t             cached_ui_prop_len;

   voice_param_data_t   *cached_ui_prop;
   voice_set_param_v2_t set_param;

   vss_icommon_cmd_set_ui_property_t *payload;
   uint32_t version = 0;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Get the APR payload pointer and size */
         payload = (vss_icommon_cmd_set_ui_property_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate inbound payload size */
         if (payload_size < sizeof(vss_icommon_cmd_set_ui_property_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_cmd_processing(): Unexpected payload size, "
                  "%d < %d", payload_size, sizeof(vss_icommon_cmd_set_ui_property_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* Validate payload size */
         if (payload->param_size & 0x3)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_cmd_processing(): Invalid UI param data "
                  "size, %d", payload->param_size);

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         if (payload->param_size > VPM_MAX_UI_PROP_DATA_LEN)
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_cmd_processing(): Invalid param data "
                  "length %d > Max length %d", payload->param_size,
                  VPM_MAX_UI_PROP_DATA_LEN);

            result = ADSP_EBADPARAM;
            break;
         }

         if (payload_size != (payload->param_size + sizeof(voice_param_data_t)))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_cmd_processing(): Invalid payload "
                  "size %d != %d", payload_size,
                  (payload->param_size + sizeof(voice_param_data_t)));

            result = ADSP_EBADPARAM;
            break;
         }

         MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
               "vpm_set_ui_property_cmd_processing(): Module Id 0x%08X / Param Id "
               "0x%08X received", payload->module_id, payload->param_id);

         session_obj->shared_heap.ui_prop_cache.version = version;
         cur_ui_prop_cache_slot = session_obj->shared_heap.ui_prop_cache.data;
         remaining_ui_prop_cache_len = sizeof(session_obj->shared_heap.ui_prop_cache.data);
         remaining_ui_prop_len = session_obj->shared_heap.ui_prop_cache.data_len;

         /* Cache UI properties. */
         while (remaining_ui_prop_cache_len > 0)
         {
            cached_ui_prop = ((voice_param_data_t *)cur_ui_prop_cache_slot);
            cached_ui_prop_len = (cached_ui_prop->param_size + sizeof(voice_param_data_t));

            if ((cached_ui_prop->module_id == payload->module_id) &&
                (cached_ui_prop->param_id == payload->param_id))
            {
               /* Value already exists. Cache it. */
               if (cached_ui_prop_len != payload_size) /* New UI property data size is different than the cached UI property data. */
               {
                  /* Shuffle the UI property cache to accommodate the new size. */
                  remaining_ui_prop_len -= cached_ui_prop_len;
                  remaining_ui_prop = (cur_ui_prop_cache_slot + cached_ui_prop_len);

                  while (remaining_ui_prop_len >= cached_ui_prop_len)
                  {
                     memscpy(cur_ui_prop_cache_slot, remaining_ui_prop_cache_len,
                             remaining_ui_prop, cached_ui_prop_len);

                     remaining_ui_prop_len -= cached_ui_prop_len;
                     remaining_ui_prop += cached_ui_prop_len;
                     cur_ui_prop_cache_slot += cached_ui_prop_len;
                     remaining_ui_prop_cache_len -= cached_ui_prop_len;
                  }

                  if (remaining_ui_prop_len != 0)
                  {
                     memscpy(cur_ui_prop_cache_slot, remaining_ui_prop_cache_len,
                             remaining_ui_prop, remaining_ui_prop_len);

                     cur_ui_prop_cache_slot += remaining_ui_prop_len;
                     remaining_ui_prop_cache_len -= remaining_ui_prop_len;
                  }

                  memscpy(cur_ui_prop_cache_slot, remaining_ui_prop_cache_len,
                          payload, payload_size);

                  if (cached_ui_prop_len < payload_size) /* Increase the cached data length due to increased UI prop data. */
                  {
                     session_obj->shared_heap.ui_prop_cache.data_len += (payload_size - cached_ui_prop_len);
                  }
                  else /* Decrease the cached data length due to decreased UI prop data. */
                  {
                     session_obj->shared_heap.ui_prop_cache.data_len -= (cached_ui_prop_len - payload_size);

                     /* Set the decreased part of the cache that used to contain data to zero. */
                     memset((cur_ui_prop_cache_slot + payload_size), 0,
                            (cached_ui_prop_len - payload_size));
                  }
               }
               else
               {
                  memscpy(cur_ui_prop_cache_slot, remaining_ui_prop_cache_len,
                          payload, payload_size);
               }

               destination_ui_prop_cache_slot = cur_ui_prop_cache_slot;
               is_cached = TRUE;
               break;
            }

            /* Save the free slot. */
            if ((!cached_ui_prop->module_id) && (!cached_ui_prop->param_id))
            {
               destination_ui_prop_cache_slot = cur_ui_prop_cache_slot;
               break;
            }

            cur_ui_prop_cache_slot += cached_ui_prop_len;
            remaining_ui_prop_cache_len -= cached_ui_prop_len;
            remaining_ui_prop_len -= cached_ui_prop_len;
         } /* while (remaining_ui_prop_cache_len > 0) */

         /* If not already cached, copy the parameter to the first free slot */
         if ((FALSE == is_cached) &&
             (destination_ui_prop_cache_slot != NULL) &&
             (session_obj->shared_heap.ui_prop_cache.num_ui_prop != VPM_MAX_UI_PROP))
         {
            memscpy(destination_ui_prop_cache_slot,
                    remaining_ui_prop_cache_len,
                    payload, payload_size);

            session_obj->shared_heap.ui_prop_cache.data_len += payload_size;
            session_obj->shared_heap.ui_prop_cache.num_ui_prop++;
            is_cached = TRUE;
         }

         if (FALSE == is_cached)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_cmd_processing(): Reached Maximum cached "
                  "entries %d", VPM_MAX_UI_PROP);

            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_cmd_processing(): Error caching Module id "
                  "0x%08X Param id 0x%08X", payload->module_id, payload->param_id);
            destination_ui_prop_cache_slot = ( uint8_t* )payload;
         }

         /* Set the command pending status */
         vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

         /* Set the cmd response pending status */
         vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

         /* Send the set param commands to dynamic services */
         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                   destination_ui_prop_cache_slot,
                                                                   payload_size,
                                                                   ELITE_CMD_SET_PARAM,
                                                                   CVD_CAL_PARAM_MINOR_VERSION_0)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_set_ui_property_cmd_processing(): Failed to send set param cmd to dyn svcs");

            /* Clear the cmd response pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

            /* Clear the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            /* End the APR command */
            break;
         }

         /* Return and wait for the set param responses */
         return ADSP_EOK;

      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after getting responses for all setparams commands */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Get the aggregated response status */
      result = session_obj->session_rsp_ctrl.status;

      /* End the APR command */
      elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

      return result;
   }

   /* In case of any error */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}


static ADSPResult vpm_set_ui_property_v2_cmd_processing(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;
   uint32_t             payload_size;
   bool_t               is_cached = FALSE;
   uint8_t              *cur_ui_prop_cache_slot;
   uint32_t             remaining_ui_prop_cache_len;
   uint8_t              *destination_ui_prop_cache_slot = NULL;
   uint8_t              *remaining_ui_prop;
   uint32_t             remaining_ui_prop_len;
   uint32_t             cached_ui_prop_len;

   voice_param_data_v2_t   *cached_ui_prop;

   vss_icommon_cmd_set_ui_property_v2_t *payload;
   uint32_t version = 1;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Get the APR payload pointer and size */
         payload = (vss_icommon_cmd_set_ui_property_v2_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate inbound payload size */
         if (payload_size < sizeof(vss_icommon_cmd_set_ui_property_v2_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_v2_cmd_processing(): Unexpected payload size, "
                  "%d < %d", payload_size, sizeof(vss_icommon_cmd_set_ui_property_v2_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* Validate payload size */
         if (payload->param_size & 0x3)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_v2_cmd_processing(): Invalid UI param data "
                  "size, %d", payload->param_size);

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         if (payload->param_size > VPM_MAX_UI_PROP_DATA_LEN)
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_v2_cmd_processing(): Invalid param data "
                  "length %d > Max length %d", payload->param_size,
                  VPM_MAX_UI_PROP_DATA_LEN);

            result = ADSP_EBADPARAM;
            break;
         }

         if (payload_size != (payload->param_size + sizeof(voice_param_data_v2_t)))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_v2_cmd_processing(): Invalid payload "
                  "size %d != %d", payload_size,
                  (payload->param_size + sizeof(voice_param_data_v2_t)));

            result = ADSP_EBADPARAM;
            break;
         }

         MSG_3(MSG_SSID_DFLT, DBG_HIGH_PRIO,
               "vpm_set_ui_property_v2_cmd_processing(): Module Id 0x%08X / Param Id "
               "0x%08X / instance Id 0x%08X received", payload->module_id, payload->param_id, payload->instance_id );

         session_obj->shared_heap.ui_prop_cache.version = version;
         cur_ui_prop_cache_slot = session_obj->shared_heap.ui_prop_cache.data;
         remaining_ui_prop_cache_len = sizeof(session_obj->shared_heap.ui_prop_cache.data);
         remaining_ui_prop_len = session_obj->shared_heap.ui_prop_cache.data_len;

         /* Cache UI properties. */
         while (remaining_ui_prop_cache_len > 0)
         {
            cached_ui_prop = ((voice_param_data_v2_t *)cur_ui_prop_cache_slot);
            cached_ui_prop_len = (cached_ui_prop->param_size + sizeof(voice_param_data_v2_t));

            if ( ( cached_ui_prop->module_id == payload->module_id ) &&
                 ( cached_ui_prop->param_id == payload->param_id ) &&
                 ( cached_ui_prop->instance_id == payload->instance_id ) )
            {
               /* Value already exists. Cache it. */
               if (cached_ui_prop_len != payload_size) /* New UI property data size is different than the cached UI property data. */
               {
                  /* Shuffle the UI property cache to accommodate the new size. */
                  remaining_ui_prop_len -= cached_ui_prop_len;
                  remaining_ui_prop = (cur_ui_prop_cache_slot + cached_ui_prop_len);

                  while (remaining_ui_prop_len >= cached_ui_prop_len)
                  {
                     memscpy(cur_ui_prop_cache_slot, remaining_ui_prop_cache_len,
                             remaining_ui_prop, cached_ui_prop_len);

                     remaining_ui_prop_len -= cached_ui_prop_len;
                     remaining_ui_prop += cached_ui_prop_len;
                     cur_ui_prop_cache_slot += cached_ui_prop_len;
                     remaining_ui_prop_cache_len -= cached_ui_prop_len;
                  }

                  if ( remaining_ui_prop_len != 0 )
                  {
                     memscpy(cur_ui_prop_cache_slot, remaining_ui_prop_cache_len,
                             remaining_ui_prop, remaining_ui_prop_len);

                     cur_ui_prop_cache_slot += remaining_ui_prop_len;
                     remaining_ui_prop_cache_len -= remaining_ui_prop_len;
                  }

                  memscpy(cur_ui_prop_cache_slot, remaining_ui_prop_cache_len,
                          payload, payload_size);

                  if (cached_ui_prop_len < payload_size) /* Increase the cached data length due to increased UI prop data. */
                  {
                     session_obj->shared_heap.ui_prop_cache.data_len += (payload_size - cached_ui_prop_len);
                  }
                  else /* Decrease the cached data length due to decreased UI prop data. */
                  {
                     session_obj->shared_heap.ui_prop_cache.data_len -= (cached_ui_prop_len - payload_size);

                     /* Set the decreased part of the cache that used to contain data to zero. */
                     memset((cur_ui_prop_cache_slot + payload_size), 0,
                            (cached_ui_prop_len - payload_size));
                  }
               }
               else
               {
                  memscpy(cur_ui_prop_cache_slot, remaining_ui_prop_cache_len,
                          payload, payload_size);
               }

               destination_ui_prop_cache_slot = cur_ui_prop_cache_slot;
               is_cached = TRUE;
               break;
            }

            /* Save the free slot. */
            if ( ( !cached_ui_prop->module_id ) && ( !cached_ui_prop->param_id ) && ( !cached_ui_prop->instance_id ) )
            {
               destination_ui_prop_cache_slot = cur_ui_prop_cache_slot;
               break;
            }

            cur_ui_prop_cache_slot += cached_ui_prop_len;
            remaining_ui_prop_cache_len -= cached_ui_prop_len;
            remaining_ui_prop_len -= cached_ui_prop_len;
         } /* while (remaining_ui_prop_cache_len > 0) */

         /* If not already cached, copy the parameter to the first free slot */
         if ((FALSE == is_cached) &&
             (destination_ui_prop_cache_slot != NULL) &&
             (session_obj->shared_heap.ui_prop_cache.num_ui_prop != VPM_MAX_UI_PROP))
         {
            memscpy(destination_ui_prop_cache_slot,
                    remaining_ui_prop_cache_len,
                    payload, payload_size);

            session_obj->shared_heap.ui_prop_cache.data_len += payload_size;
            session_obj->shared_heap.ui_prop_cache.num_ui_prop++;
            is_cached = TRUE;
         }

         if ( FALSE == is_cached )
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_v2_cmd_processing(): Reached Maximum cached "
                  "entries %d", VPM_MAX_UI_PROP);

            MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_ui_property_v2_cmd_processing(): Error caching Module id "
                  "0x%08X Param id 0x%08X Instance id 0x%08X", 
                  payload->module_id, payload->param_id, payload->instance_id);
            destination_ui_prop_cache_slot = (uint8_t*)payload;
         }

         /* Set the command pending status */
         vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

         /* Set the cmd response pending status */
         vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

         /* Send the set param commands to dynamic services */
         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                   destination_ui_prop_cache_slot,
                                                                   payload_size,
                                                                   ELITE_CMD_SET_PARAM,
                                                                   CVD_CAL_PARAM_MINOR_VERSION_1)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_set_ui_property_v2_cmd_processing(): Failed to send set param cmd to dyn svcs");

            /* Clear the cmd response pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

            /* Clear the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            /* End the APR command */
            break;
         }

         /* Return and wait for the set param responses */
         return ADSP_EOK;

      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after getting responses for all setparams commands */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Get the aggregated response status */
      result = session_obj->session_rsp_ctrl.status;

      /* End the APR command */
      elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

      return result;
   }

   /* In case of any error */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_get_ui_property_cmd_processing(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   elite_apr_packet_t   *p_rsp_packet;
   int32_t              rc;
   uint32_t             payload_size;
   bool_t               ui_prop_exists = FALSE;
   uint8_t              *cur_ui_prop_cache_slot;
   uint32_t             remaining_ui_prop_cache_len;
   uint32_t             get_ui_status;
   uint8_t              *rsp_payload;
   uint32_t             rsp_payload_size;
   uint32_t             rsp_payload_left;
   voice_param_data_t   *cached_ui_prop;
   void                 *p_get_param_rsp;
   voice_param_data_t   *p_voice_param_data;

   vss_icommon_cmd_get_ui_property_t *payload;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Get the APR payload pointer and size */
         payload = (vss_icommon_cmd_get_ui_property_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate inbound payload size */
         if (payload_size != sizeof(vss_icommon_cmd_get_ui_property_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_get_ui_property_cmd_processing(): Unexpected payload size, "
                  "%d != %d", payload_size, sizeof(vss_icommon_cmd_get_ui_property_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         if (payload->param_size & 3)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_get_ui_property_cmd_processing(): Invalid UI param data size, "
                  "%d", payload->param_size);

            result = ADSP_EBADPARAM;
            break;
         }

         remaining_ui_prop_cache_len = session_obj->shared_heap.ui_prop_cache.data_len;
         cur_ui_prop_cache_slot = session_obj->shared_heap.ui_prop_cache.data;

         while (remaining_ui_prop_cache_len != 0)
         {
            cached_ui_prop = ((voice_param_data_t *)cur_ui_prop_cache_slot);

            if ((cached_ui_prop->module_id == payload->module_id) &&
                (cached_ui_prop->param_id == payload->param_id))
            {
               ui_prop_exists = TRUE;
               get_ui_status = 0;
               break;
            }

            cur_ui_prop_cache_slot += (cached_ui_prop->param_size +
                                       sizeof(voice_param_data_t));
            remaining_ui_prop_cache_len -= (cached_ui_prop->param_size +
                                            sizeof(voice_param_data_t));
         }

         /* If UI prop is already available */
         if (TRUE == ui_prop_exists)
         {
            rsp_payload_size = (sizeof(vss_icommon_rsp_get_ui_property_t) +
                                cached_ui_prop->param_size);

            if (APR_EOK != (rc = elite_apr_if_alloc_cmd_rsp(session_obj->self_apr_handle,
                                                            elite_apr_if_get_dst_addr(p_apr_pkt),
                                                            elite_apr_if_get_dst_port(p_apr_pkt),
                                                            elite_apr_if_get_src_addr(p_apr_pkt),
                                                            elite_apr_if_get_src_port(p_apr_pkt),
                                                            elite_apr_if_get_client_token(p_apr_pkt),
                                                            VSS_ICOMMON_RSP_GET_UI_PROPERTY,
                                                            rsp_payload_size,
                                                            &p_rsp_packet)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_get_ui_property_cmd_processing(): Failed to allocate rsp payload for Get UI prop cmd"
                     "result: 0x%lx", rc);

               /* End the APR command */
               result = rc;
               break;
            }

            /* Get the pointer to allocated payload and it's size */
            rsp_payload = (uint8_t *)elite_apr_if_get_payload_ptr(p_rsp_packet);
            rsp_payload_left = elite_apr_if_get_payload_size(p_rsp_packet);

            /* Copy the response status field */
            memscpy(rsp_payload, rsp_payload_left,
                    &get_ui_status, sizeof(get_ui_status));

            rsp_payload_left -= sizeof(get_ui_status);

            /* Copy param data header and actual payload contents */
            memscpy((rsp_payload + sizeof(get_ui_status)), rsp_payload_left,
                    cached_ui_prop, (cached_ui_prop->param_size + sizeof(voice_param_data_t)));

            /* ACK back to the client */
            elite_apr_if_cmd_fwd(session_obj->self_apr_handle, p_rsp_packet);

            /* Free up the existing cmd apr packet */
            elite_apr_if_free(session_obj->self_apr_addr, p_apr_pkt);

            return ADSP_EOK;
         }
         else
         {
            /* If the UI property is not cached, get it from dyn svcs. */

            MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "CVD_CAL_MSG_PARAM: Sending GET_PARAM_V2 to VCP. "
                  "vpm_get_ui_property_cmd_processing(): Token=%d Param ID=0x%08x",
                  p_apr_pkt->token, payload->param_id);

            /* Calculate the get param rsp payload size */
            payload_size = payload->param_size + sizeof(voice_param_data_t) + sizeof(voice_get_param_ack_t);

            /* Allocate the response payload for in-band get param */
            if (NULL == (p_get_param_rsp = qurt_elite_memory_malloc(payload_size,
                                                                    QURT_ELITE_HEAP_DEFAULT)))
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                   "vpm_get_ui_property_cmd_processing(): Failed to allocate in-band get param rsp payload");

               result = ADSP_EFAILED;
               break;
            }

            /* Get the pointer to start of voice param data */
            p_voice_param_data = (voice_param_data_t *)((uint8_t *)p_get_param_rsp + sizeof(voice_get_param_ack_t));

            p_voice_param_data->module_id = payload->module_id;
            p_voice_param_data->param_id = payload->param_id;

            /* Decrease the length of response status */
            p_voice_param_data->param_size = (payload_size - sizeof(voice_get_param_ack_t));

            /* Set the cmd rsp pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

            /* Set the cmd rsp pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

            /* Set the response handler for get param commands */
            session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_get_ui_prop_cmd_rsp_handler;

            /* Set the pointer to ack payload */
            session_obj->session_rsp_ctrl.p_ack_payload = p_voice_param_data;

            /* Send the get param cmd to dyn services */
            if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                      p_voice_param_data,
                                                                      p_voice_param_data->param_size,
                                                                      ELITE_CMD_GET_PARAM,
                                                                      CVD_CAL_PARAM_MINOR_VERSION_0)))
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                   "vpm_get_ui_property_cmd_processing(): Failed to send get param cmd to dyn svcs");

               /* Clear the cmd rsp pending status */
               vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

               /* Clear the cmd rsp pending status */
               vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

               /* Free the allocated packet */
               qurt_elite_memory_free(p_get_param_rsp);

               /* End the APR command */
               break;
            }

            MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "CVD_CAL_MSG_PARAM: Sent GET_PARAM_V2 to vpTx/Rx dyn svcs"
                  "vpm_get_ui_property_cmd_processing(): MID: 0x%08x, PID: 0x%08x",
                  payload->module_id, payload->param_id);

            /* Return and wait for get param responses */

            result = ADSP_EOK;

         }
      } /* End of if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after getting responses for all the get params */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Get the aggregated response status */
      result = session_obj->session_rsp_ctrl.status;

      /* Free the original APR command pkt */
      elite_apr_if_free(session_obj->self_apr_handle, p_apr_pkt);

      return result;

   } /* for (;;) */

   /* In case of any error */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_get_ui_property_v2_cmd_processing(vpm_session_object_t *session_obj)
{
   ADSPResult            result = ADSP_EOK;
   elite_apr_packet_t    *p_apr_pkt;
   elite_apr_packet_t    *p_rsp_packet = NULL;
   int32_t               rc;
   uint32_t              payload_size;
   bool_t                ui_prop_exists = FALSE;
   uint8_t               *cur_ui_prop_cache_slot;
   uint32_t              remaining_ui_prop_cache_len;
   uint32_t              get_ui_status;
   uint8_t               *rsp_payload;
   uint32_t              rsp_payload_size;
   uint32_t              rsp_payload_left;
   voice_param_data_v2_t *cached_ui_prop;
   void                  *p_get_param_rsp;
   voice_param_data_v2_t    *p_voice_param_data;

   vss_icommon_cmd_get_ui_property_v2_t *payload;
   uint32_t version = 1;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Get the APR payload pointer and size */
         payload = (vss_icommon_cmd_get_ui_property_v2_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate inbound payload size */
         if (payload_size != sizeof(vss_icommon_cmd_get_ui_property_v2_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_get_ui_property_v2_cmd_processing(): Unexpected payload size, "
                  "%d != %d", payload_size, sizeof(vss_icommon_cmd_get_ui_property_v2_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         if (payload->param_size & 3)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_get_ui_property_v2_cmd_processing(): Invalid UI param data size, "
                  "%d", payload->param_size);

            result = ADSP_EBADPARAM;
            break;
         }

         remaining_ui_prop_cache_len = session_obj->shared_heap.ui_prop_cache.data_len;
         cur_ui_prop_cache_slot = session_obj->shared_heap.ui_prop_cache.data;

         while (remaining_ui_prop_cache_len != 0)
         {
            cached_ui_prop = ((voice_param_data_v2_t *)cur_ui_prop_cache_slot);

            if ((cached_ui_prop->module_id == payload->module_id) &&
                (cached_ui_prop->param_id == payload->param_id) &&
                (cached_ui_prop->instance_id == payload->instance_id)) 
            {
               ui_prop_exists = TRUE;
               get_ui_status = 0;
               break;
            }

            cur_ui_prop_cache_slot += (cached_ui_prop->param_size +
                                       sizeof(voice_param_data_v2_t));
            remaining_ui_prop_cache_len -= (cached_ui_prop->param_size +
                                            sizeof(voice_param_data_v2_t));
         }

         /* If UI prop is already available */
         if (TRUE == ui_prop_exists)
         {
            rsp_payload_size = (sizeof(vss_icommon_rsp_get_ui_property_v2_t) +
                                cached_ui_prop->param_size);

            if (APR_EOK != (rc = elite_apr_if_alloc_cmd_rsp(session_obj->self_apr_handle,
                                                            elite_apr_if_get_dst_addr(p_apr_pkt),
                                                            elite_apr_if_get_dst_port(p_apr_pkt),
                                                            elite_apr_if_get_src_addr(p_apr_pkt),
                                                            elite_apr_if_get_src_port(p_apr_pkt),
                                                            elite_apr_if_get_client_token(p_apr_pkt),
                                                            VSS_ICOMMON_RSP_GET_UI_PROPERTY_V2,
                                                            rsp_payload_size,
                                                            &p_rsp_packet)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_get_ui_property_v2_cmd_processing(): Failed to allocate rsp payload for Get UI prop cmd"
                     "result: 0x%lx", rc);

               /* End the APR command */
               result = rc;
               break;
            }

            /* Get the pointer to allocated payload and it's size */
            rsp_payload = (uint8_t *)elite_apr_if_get_payload_ptr(p_rsp_packet);
            rsp_payload_left = elite_apr_if_get_payload_size(p_rsp_packet);

            /* Copy the response status field */
            memscpy(rsp_payload, rsp_payload_left,
                    &get_ui_status, sizeof(get_ui_status));

            rsp_payload_left -= sizeof(get_ui_status);

            /* Copy param data header and actual payload contents */
            memscpy((rsp_payload + sizeof(get_ui_status)), rsp_payload_left,
                    cached_ui_prop, (cached_ui_prop->param_size + sizeof(voice_param_data_v2_t)));

            /* ACK back to the client */
            elite_apr_if_cmd_fwd(session_obj->self_apr_handle, p_rsp_packet);

            /* Free up the existing cmd apr packet */
            elite_apr_if_free(session_obj->self_apr_addr, p_apr_pkt);

            return ADSP_EOK;
         }
         else
         {
            /* If the UI property is not cached, get it from dyn svcs. */

            MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "CVD_CAL_MSG_PARAM: Sending GET_PARAM_V2 to VCP. "
                  "vpm_get_ui_property_v2_cmd_processing(): Token=%d Param ID=0x%08x",
                  p_apr_pkt->token, payload->param_id);

            /* Calculate the get param rsp payload size */
            payload_size = payload->param_size + sizeof(voice_param_data_v2_t) + sizeof(vss_icommon_rsp_get_param_v3_t);

            /* Allocate the response payload for in-band get param */
            if (NULL == (p_get_param_rsp = qurt_elite_memory_malloc(payload_size,
                                                                    QURT_ELITE_HEAP_DEFAULT)))
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                   "vpm_get_ui_property_v2_cmd_processing(): Failed to allocate in-band get param rsp payload");

               result = ADSP_EFAILED;
               break;
            }

            /* Get the pointer to start of voice param data */
            p_voice_param_data = (voice_param_data_v2_t *)((uint8_t *)p_get_param_rsp + sizeof(vss_icommon_rsp_get_param_v3_t));

            p_voice_param_data->module_id = payload->module_id;
            p_voice_param_data->param_id = payload->param_id;
            p_voice_param_data->instance_id = payload->instance_id;

            /* Decrease the length of response status */
            p_voice_param_data->param_size = (payload_size - sizeof(vss_icommon_rsp_get_param_v3_t));

            /* Set the cmd rsp pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

            /* Set the cmd rsp pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

            /* Set the response handler for get param commands */
            session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_get_ui_prop_v2_cmd_rsp_handler;

            /* Set the pointer to ack payload */
            session_obj->session_rsp_ctrl.p_ack_payload = p_get_param_rsp;

            /* Send the get param cmd to dyn services */
            if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                      p_voice_param_data,
                                                                      p_voice_param_data->param_size,
                                                                      ELITE_CMD_GET_PARAM,
                                                                      CVD_CAL_PARAM_MINOR_VERSION_1)))
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                   "vpm_get_ui_property_v2_cmd_processing(): Failed to send get param cmd to dyn svcs");

               /* Clear the cmd rsp pending status */
               vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

               /* Clear the cmd rsp pending status */
               vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

               /* Free the allocated packet */
               qurt_elite_memory_free(p_get_param_rsp);

               /* End the APR command */
               break;
            }

            MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_get_ui_property_v2_cmd_processing: Sent GET_PARAM_V2 to vpTx/Rx dyn svcs"
                  "MID: 0x%08x, PID: 0x%08x, IID: 0x%08x ",
                  payload->module_id, payload->param_id, payload->instance_id );

            /* Return and wait for get param responses */

            result = ADSP_EOK;

         }
      } /* End of if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after getting responses for all the get params */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Get the aggregated response status */
      result = session_obj->session_rsp_ctrl.status;

      /* Free the original APR command pkt */
      elite_apr_if_free(session_obj->self_apr_handle, p_apr_pkt);

      return result;

   } /* for (;;) */

   /* In case of any error */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_register_static_calibration_data_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult                    result = ADSP_EOK;
   int32_t                       rc;
   uint32_t                      payload_len;
   cvd_cal_table_descriptor_t    cal_table_descriptor;
   cvd_virt_addr_t               cal_virt_addr;
   cvd_cal_log_commit_info_t     log_info;
   cvd_cal_log_table_header_t    log_info_table;
   elite_apr_packet_t            *p_apr_pkt;

   vss_ivocproc_cmd_register_static_calibration_data_t *payload;

   bool_t is_mapped_memory_in_range = FALSE;
   /* Flag to indicate if memory range of mem_map command
    * and cal registration are in sync */

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (TRUE == session_obj->static_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_static_calibration_data_cmd_ctrl(): Calibration data already registered.");

         result = ADSP_EALREADY;
         break;
      }

      if (TRUE == session_obj->common_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_static_calibration_data_cmd_ctrl():"
             " Cal data already registered via deprecated cal registration command.");

         result = ADSP_EALREADY;
         break;
      }

      if ((TRUE == session_obj->is_device_config_registered) &&
          (NULL == session_obj->hdvoice_config_info.hdvoice_config_hdr.sys_config_list_head))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_static_calibration_data_cmd_ctrl():"
             " WideVoice Module in Device Config is incompatible with registration. "
             " Resetting to Default Rx Sampling Rate.");

         session_obj->widevoice_rx_sr = VPM_DEFAULT_RX_PP_SR;

         if (TRUE == vpm_media_id_is_nb_sr(session_obj->active_set.system_config.media_id))
         {
            if (session_obj->active_set.system_config.rx_pp_sr > VPM_DEFAULT_RX_PP_SR)
            {
               session_obj->target_set.system_config.rx_pp_sr = VPM_DEFAULT_RX_PP_SR;
               session_obj->is_vocproc_config_changed = TRUE;
            }
         }
      }

      /* Get the APR payload pointer and size */
      payload = (vss_ivocproc_cmd_register_static_calibration_data_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
      payload_len = elite_apr_if_get_payload_size(p_apr_pkt);

      if (payload_len < sizeof(vss_ivocproc_cmd_register_static_calibration_data_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_register_static_calibration_data_cmd_ctrl(): Unexpected payload size, %d < %d",
               payload_len, sizeof(vss_ivocproc_cmd_register_static_calibration_data_t));

         result = ADSP_EBADPARAM;
         break;
      }

      if (ADSP_EOK != (result = vpm_validate_calib_payload(payload->cal_mem_handle,
                                                           payload->cal_mem_size,
                                                           payload->cal_mem_address)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_register_static_calibration_data_cmd_ctrl(): Failed to validate calib payload: result: 0x%lx",
               result);

         /* End the APR command */
         break;
      }

      rc = cvd_mem_mapper_get_virtual_addr_v2(payload->cal_mem_handle,
                                              payload->cal_mem_address,
                                              &cal_virt_addr);
      if (rc)
      {
         MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_register_static_calibration_data_cmd_ctrl(): Cannot get virtual address,"
               " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->cal_mem_handle,
               (uint32_t)payload->cal_mem_address, (uint32_t)(payload->cal_mem_address >> 32));

         result = ADSP_EFAILED;
         break;
      }
      else
      {
         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_register_static_calibration_data_cmd_ctrl(): cal memory virtual address 0x%016X, size %d",
               cal_virt_addr.ptr, payload->cal_mem_size);
      }

      is_mapped_memory_in_range = TRUE;
      (void)cvd_mem_mapper_cache_invalidate_v2(&cal_virt_addr, payload->cal_mem_size);

      cal_table_descriptor.start_ptr = cal_virt_addr.ptr;
      cal_table_descriptor.size = payload->cal_mem_size;
      cal_table_descriptor.data_mem_handle = payload->cal_mem_handle;
      cal_table_descriptor.indexing_mem_handle = APR_NULL_V;
      cal_table_descriptor.num_columns = payload->num_columns;
      cal_table_descriptor.columns = ((cvd_cal_column_descriptor_t *)
                                      (((uint8_t *)payload) +
                                       sizeof(vss_ivocproc_cmd_register_static_calibration_data_t)));

      rc = cvd_cal_parse_table_v2(&cal_table_descriptor,
                                  &session_obj->static_cal.table_handle);
      if (rc)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_static_calibration_data_cmd_ctrl(): Error in parsing calibration table");

         result = ADSP_EFAILED;
         break;
      }

      session_obj->static_cal.is_registered = TRUE;
      session_obj->static_cal.is_calibrate_needed = TRUE;

      session_obj->is_vocproc_config_changed = TRUE;

      /* Log vocproc static cal table. */
      if (TRUE == is_mapped_memory_in_range)
      {
         log_info_table.table_handle = session_obj->static_cal.table_handle;

         log_info.instance = ((session_obj->attached_mvm_handle << 16) |
                                 (session_obj->self_apr_port));
         log_info.call_num = session_obj->target_set.system_config.call_num;
         log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_TABLE;
         log_info.data_container_header_size = sizeof(log_info_table);
         log_info.data_container_header = &log_info_table;
         log_info.payload_size = cal_table_descriptor.size;
         log_info.payload_buf = cal_table_descriptor.start_ptr;

         cvd_cal_log_data((log_code_type)LOG_ADSP_CVD_CAL_DATA_C, CVD_CAL_LOG_VOCPROC_STATIC_TABLE,
                                (void *)&log_info, sizeof(log_info));
      }

      break;

   } /* End of for (;;) */

   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_deregister_static_calibration_data_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      if (FALSE == session_obj->static_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_deregister_static_calibration_data_cmd_ctrl(): Calibration not registered.");

         result = ADSP_EALREADY;
         break;
      }

      rc = cvd_cal_discard_table_v2(session_obj->static_cal.table_handle);
      if (rc)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_deregister_static_calibration_data_cmd_ctrl(): Failed to deregister calibration data.");

         result = ADSP_EFAILED;
         break;
      }
      else
      {
         session_obj->static_cal.is_registered = FALSE;
         session_obj->static_cal.is_calibrate_needed = FALSE;
      }

      break;
   }

   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_register_dynamic_calibration_data_cmd_ctrl(vpm_session_object_t *session_obj)
{
  ADSPResult           result = ADSP_EOK;
  elite_apr_packet_t   *p_apr_pkt;
  uint32_t             payload_len;
  cvd_virt_addr_t      cal_virt_addr;
  uint32_t             min_vol_index;
  uint32_t             max_vol_index;
  uint32_t             checkpoint = 0;

  cvd_cal_log_commit_info_t     log_info;
  cvd_cal_log_table_header_t    log_info_table;
  cvd_cal_table_descriptor_t    cal_table_descriptor;

  vss_ivocproc_cmd_register_dynamic_calibration_data_t *payload;

  p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

  for (;;)
  {
    /* Check if session creation is complete or not */
    if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
    {
       /* End the APR command*/
       break;
    }

    if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
    {
      if (TRUE == session_obj->dynamic_cal.is_registered)
      {
        MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_register_dynamic_calibration_data_cmd_ctrl(): Calibration already registered.");

        result = ADSP_EALREADY;
        break;
      }

      if (TRUE == session_obj->volume_cal.is_registered)
      {
        MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_register_dynamic_calibration_data_cmd_ctrl(): "
            "Calibration already registered via deprecated volume cal registration command.");

        result = ADSP_EALREADY;
        break;
      }

      if ((TRUE == session_obj->is_device_config_registered) &&
          (NULL == session_obj->hdvoice_config_info.hdvoice_config_hdr.sys_config_list_head))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_dynamic_calibration_data_cmd_ctrl(): WideVoice Module in Device Config"
             " is incompatible with registration.  Resetting to Default Rx Sampling Rate.");

         session_obj->widevoice_rx_sr = VPM_DEFAULT_RX_PP_SR;

         if (TRUE == vpm_media_id_is_nb_sr(session_obj->active_set.system_config.media_id))
         {
            if (session_obj->active_set.system_config.rx_pp_sr > VPM_DEFAULT_RX_PP_SR)
            {
               session_obj->target_set.system_config.rx_pp_sr = VPM_DEFAULT_RX_PP_SR;
               session_obj->is_vocproc_config_changed = TRUE;
            }
         }
      }

      /* Get the APR payload pointer and size */
      payload = (vss_ivocproc_cmd_register_dynamic_calibration_data_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
      payload_len = elite_apr_if_get_payload_size(p_apr_pkt);

      if (payload_len < sizeof(vss_ivocproc_cmd_register_dynamic_calibration_data_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_register_dynamic_calibration_data_cmd_ctrl():"
               " Unexpected payload size, %d < %d",
               payload_len, sizeof(vss_ivocproc_cmd_register_dynamic_calibration_data_t));

         result = ADSP_EBADPARAM;
         break;
      }

      if (ADSP_EOK != (result = vpm_validate_calib_payload(payload->cal_mem_handle,
                                                           payload->cal_mem_size,
                                                           payload->cal_mem_address)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_register_dynamic_calibration_data_cmd_ctrl(): Failed to validate calib payload: result: 0x%lx",
               result);

         /* End the APR command */
         break;
      }

      if (ADSP_EOK != (result = cvd_mem_mapper_get_virtual_addr_v2(payload->cal_mem_handle,
                                                                   payload->cal_mem_address,
                                                                   &cal_virt_addr)))
      {
         MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_register_dynamic_calibration_data_cmd_ctrl(): Cannot get virtual address,"
               " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->cal_mem_handle,
               (uint32_t)payload->cal_mem_address, (uint32_t)(payload->cal_mem_address >> 32));

         break;
      }
      else
      {
         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_register_dynamic_calibration_data_cmd_ctrl(): cal memory virtual address 0x%016X, size %d",
               cal_virt_addr.ptr, payload->cal_mem_size);
      }

      (void)cvd_mem_mapper_cache_invalidate_v2(&cal_virt_addr, payload->cal_mem_size);

      cal_table_descriptor.start_ptr = cal_virt_addr.ptr;
      cal_table_descriptor.size = payload->cal_mem_size;
      cal_table_descriptor.data_mem_handle = payload->cal_mem_handle;
      cal_table_descriptor.indexing_mem_handle = APR_NULL_V;
      cal_table_descriptor.num_columns = payload->num_columns;
      cal_table_descriptor.columns = ((cvd_cal_column_descriptor_t *)
                                      (((uint8_t *)payload) +
                                       sizeof(vss_ivocproc_cmd_register_dynamic_calibration_data_t)));

      result = cvd_cal_parse_table_v2(&cal_table_descriptor,
                                  &session_obj->dynamic_cal.table_handle);

      /* Log vocproc dynamic cal table. */
      log_info_table.table_handle = session_obj->dynamic_cal.table_handle;

      log_info.instance = ((session_obj->attached_mvm_handle << 16) |
                              (session_obj->self_apr_port));
      log_info.call_num = session_obj->target_set.system_config.call_num;
      log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_TABLE;
      log_info.data_container_header_size = sizeof(log_info_table);
      log_info.data_container_header = &log_info_table;
      log_info.payload_size = cal_table_descriptor.size;
      log_info.payload_buf = cal_table_descriptor.start_ptr;

      cvd_cal_log_data((log_code_type)LOG_ADSP_CVD_CAL_DATA_C, CVD_CAL_LOG_VOCPROC_DYNAMIC_TABLE,
                             (void *)&log_info, sizeof(log_info));

      if (ADSP_EOK != result)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_dynamic_calibration_data_cmd_ctrl(): Error in parsing calibration table");
         break;
      }

      checkpoint = 1;

      /* Determine volume indices. */
      /* Assumption: same volume indices for all networks, all vocoders and all
       * vocoder operating mode. */
      result = cvd_cal_find_min_max_column_value_v2(session_obj->dynamic_cal.table_handle,
                                                    VSS_ICOMMON_CAL_COLUMN_RX_VOLUME_INDEX,
                                                    &min_vol_index,
                                                    &max_vol_index);
      if (ADSP_EOK != result)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_register_dynamic_calibration_data_cmd_ctrl(): "\
                "Cannot find min/max volume indices");
         break;
      }

      MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_register_dynamic_calibration_data_cmd_ctrl(): "\
               "min volume index %d, max volume index %d",
            min_vol_index, max_vol_index);

      session_obj->dynamic_cal.min_vol_index = min_vol_index;
      session_obj->dynamic_cal.max_vol_index = max_vol_index;
      session_obj->dynamic_cal.num_vol_indices = (max_vol_index - min_vol_index + 1);

      if (FALSE == session_obj->is_client_set_num_vol_steps)
      {
         session_obj->target_set.client_num_vol_steps = session_obj->dynamic_cal.num_vol_indices;
      }
      result = cvd_cal_find_vol_cal_format(session_obj->dynamic_cal.table_handle,
                                           &session_obj->dynamic_cal.is_v1_vol_format);
      if (ADSP_EOK != result)
      {
         MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO, "vpm_register_dynamic_calibration_data_cmd_ctrl(): "\
                "Cannot find table format info");

         session_obj->dynamic_cal.is_v1_vol_format = FALSE; /* Assume v2 version if not able to determine. */
         result = ADSP_EOK; /* This is not an error */
      }

      session_obj->dynamic_cal.is_registered = TRUE;

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "CVD_CAL_MSG: PVC: vpm_register_dynamic_calibration_data_cmd_ctrl(): SUCCESS, Table size reported: %d",
            cal_table_descriptor.size);

      checkpoint = 2;

      if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
      {
        if (ADSP_EOK != (result = vpm_calibrate_dynamic(session_obj, 0, VPM_DYNAMIC_CAL_USECASE_GAIN)))
        {
          /* End the command */
          result = ADSP_EFAILED;
          break;
        }
        else
        {
          /* Set command pending status to true and wait for response. */
          vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);
          return ADSP_EOK;
        }
      }
      else
      {
        /* No need to apply cal now. End the APR command */
        session_obj->dynamic_cal.is_calibrate_needed = TRUE;
        session_obj->is_vocproc_config_changed = TRUE;
      } /* end of if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state) else */
    } /* end of if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

    /* Execution will reach here after receiving all the cmd resps.
       CVD_CAL_INVALID_HANDLE_V check is to ensure query deinit happens only if vpm_calibrate_dynamic() is called*/
    if (CVD_CAL_INVALID_HANDLE_V != session_obj->dynamic_cal.query_handle)
    {
      /* De-init calibration query */
      cvd_cal_query_deinit(session_obj->dynamic_cal.query_handle);
      session_obj->dynamic_cal.query_handle = CVD_CAL_INVALID_HANDLE_V;
    }

    /* Clear the command pending status */
    vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

    /* End the APR command */
    elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

    return result;
  }

  /* Control will reach here if any error occurs during command processing */
  switch (checkpoint)
  {
    case 2:
    {
       session_obj->dynamic_cal.is_registered = FALSE;
       /*-fallthru */
    }
    case 1:
    {
       (void)cvd_cal_discard_table_v2(session_obj->dynamic_cal.table_handle);
       /*-fallthru */
    }
    default:
    {
       elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);
       break;
    }
  }

  return result;
}

static ADSPResult vpm_deregister_dynamic_calibration_data_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {

      if (FALSE == session_obj->dynamic_cal.is_registered)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_deregister_dynamic_calibration_data_cmd_ctrl(): Calibration not registered.");

         result = ADSP_EALREADY;
         break;
      }

      rc = cvd_cal_discard_table_v2(session_obj->dynamic_cal.table_handle);
      if (rc)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_deregister_dynamic_calibration_data_cmd_ctrl(): Failed to deregister calibration data.");

         result = ADSP_EFAILED;
         break;
      }
      else
      {
         session_obj->dynamic_cal.is_registered = FALSE;
         session_obj->dynamic_cal.is_calibrate_needed = FALSE;
      }

      break;
   } /* End of for (;;) */

   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_set_sampling_rate_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   uint32_t             requested_rx_sr;
   uint32_t             requested_tx_sr;
   uint32_t             payload_size;

   vss_ivocproc_cmd_set_sampling_rate_t *in_args;

   for (;;)
   {
      p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

      /* Validate inbound payload size */
      if (payload_size != sizeof(vss_ivocproc_cmd_set_sampling_rate_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_set_sampling_rate_cmd_ctrl(): Incorrect cmd payload size: %lu != expected (%lu)",
               payload_size, sizeof(vss_ivocproc_cmd_set_sampling_rate_t));

         /* End the APR command */
         result = ADSP_EBADPARAM;
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
         {
            /* Setting sampling rate when the vocproc is running is not allowed. */
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_set_sampling_rate_cmd_ctrl(): Vocproc is running.");

            /* End the APR command */
            result = ADSP_EFAILED;
            break;
         }

         in_args = (vss_ivocproc_cmd_set_sampling_rate_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

         requested_rx_sr = session_obj->target_set.system_config.rx_pp_sr;
         requested_tx_sr = session_obj->target_set.system_config.tx_pp_sr;

         switch (session_obj->direction)
         {
            case VSS_IVOCPROC_DIRECTION_RX:
            {
               requested_rx_sr = in_args->rx;
               break;
            }
            case VSS_IVOCPROC_DIRECTION_TX:
            {
               requested_tx_sr = in_args->tx;
               break;
            }
            case VSS_IVOCPROC_DIRECTION_RX_TX:
            {
               requested_rx_sr = in_args->rx;
               requested_tx_sr = in_args->tx;
               break;
            }
            default:
            {
               /* Should not hit */
               break;
            }
         }

         if ((vpm_sr_is_valid(requested_rx_sr) == FALSE) ||
             (vpm_sr_is_valid(requested_tx_sr) == FALSE))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_sampling_rate_cmd_ctrl(): Invalid Rx or Tx sampling "\
                     "rate, Rx: %lu, Tx: %lu",
                  requested_rx_sr, requested_tx_sr);

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         session_obj->is_sr_set_by_client = TRUE;

         /* Check if there is any change in VPM session sample rate */
         if ((session_obj->target_set.system_config.rx_pp_sr != requested_rx_sr) ||
             (session_obj->target_set.system_config.tx_pp_sr != requested_tx_sr))
         {
            session_obj->is_vocproc_config_changed = TRUE;

            /* Client settings take precedence over MVM settings. Active Set is
             * being updted in State: IDLE and Goal: REINIT
            */
            session_obj->target_set.system_config.rx_pp_sr = requested_rx_sr;
            session_obj->target_set.system_config.tx_pp_sr = requested_tx_sr;
            session_obj->target_set.rx_in_sample_rate = requested_rx_sr;

            /* Re-init vocproc */
            vpm_create_next_goal_ctrl(session_obj, CVP_GOAL_ENUM_REINIT);
         }
         else
         {
            MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                "vpm_set_sampling_rate_cmd_ctrl(): No change in target tx/rx sampling rate");

            result = ADSP_EOK;
            break;
         }
      } /* End of if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */


      result = vpm_state_control(session_obj);

      /* Check the goal completion status. If completed, end the APR command,
          else keep the cmd in pending status until completion.
          Session cmdQ will not be popped until cmd completion */
      vpm_check_goal_completion_status(session_obj);

      return result;
   } /* End of for (;;) */

   /* If any error occurs */
   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_set_system_config_cmd_ctrl(
  vpm_session_object_t* session_obj
)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;
   uint32_t             payload_size;

   vss_icommon_cmd_set_system_config_t *in_args;
   vss_icommon_rsp_set_system_config_t set_system_config_rsp;

   /* Check if session creation is complete or not */
   if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
   {
      /* End the APR command */
      elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_packet, ADSP_EFAILED);

      return ADSP_EFAILED;
   }

   if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
   {
      if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
      {
         /* Setting system config when the vocproc is running is not allowed. */
        MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_set_system_config_cmd_ctrl(): Vocproc is running.");

         elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_packet, ADSP_EFAILED);
         return ADSP_EFAILED;
      }

      payload_size = elite_apr_if_get_payload_size(p_apr_packet);

      if (payload_size != sizeof(vss_icommon_cmd_set_system_config_t))
      {
         /* Setting system config when the vocproc is running is not allowed. */
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_set_system_config_cmd_ctrl(): Invalid payload size, expected: %lu, actual: %lu",
               sizeof(vss_icommon_cmd_set_system_config_t), payload_size);

         elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_packet, ADSP_EBADPARAM);
         return ADSP_EBADPARAM;
      }

      /* Get the pointer to APR cmd payload */
      in_args = (vss_icommon_cmd_set_system_config_t *)elite_apr_if_get_payload_ptr(p_apr_packet);

      session_obj->target_set.system_config.network_id = in_args->network_id;
      session_obj->target_set.system_config.media_id = in_args->media_id;
      session_obj->target_set.system_config.rx_voc_op_mode = in_args->rx_voc_op_mode;
      session_obj->target_set.system_config.tx_voc_op_mode = in_args->tx_voc_op_mode;
      session_obj->target_set.system_config.feature = in_args->feature;
      session_obj->target_set.system_config.vsid = in_args->vsid;
      session_obj->target_set.system_config.vfr_mode = in_args->vfr_mode;
      session_obj->target_set.system_config.call_num = in_args->call_num;

      /* Update target sampling rates if they have not been set by client. */
      if (FALSE == session_obj->is_sr_set_by_client)
      {
        /* For custom BWE the input SR of vprx is same as its SR */
        if( VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE == 
            session_obj->target_set.system_config.feature )
        {
          session_obj->target_set.rx_in_sample_rate = in_args->rx_pp_sr;
        }
        else /* WV modules receive input same SR as dec SR */
        {
        session_obj->target_set.rx_in_sample_rate = in_args->dec_sr;
        }

        session_obj->target_set.system_config.rx_pp_sr = in_args->rx_pp_sr;
        session_obj->target_set.system_config.tx_pp_sr = in_args->tx_pp_sr;
        /* Take max of ( system config Rx samplig rate ) and ( Widevoice Rx
         * sampling rate. */
        if (session_obj->widevoice_rx_sr > in_args->rx_pp_sr)
        {
          session_obj->target_set.system_config.rx_pp_sr = session_obj->widevoice_rx_sr;
        }
      }
      else 
      {
        MSG_3( MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_set_system_config_cmd_ctrl(): Sample rate set by client "
               "tx_sample_rate=%d, rx_sample_rate=%d, rx_in_sample_rate=%d",
               session_obj->target_set.system_config.tx_pp_sr,
               session_obj->target_set.system_config.rx_pp_sr,
               session_obj->target_set.rx_in_sample_rate );
      }

      if( TRUE == vpm_is_calibration_required( session_obj ) )
      {
        /* BACKWARD COMPATIBILITY */
        session_obj->common_cal.is_calibrate_needed = session_obj->common_cal.is_registered;
        session_obj->volume_cal.is_calibrate_needed = session_obj->volume_cal.is_registered;

        session_obj->static_cal.is_calibrate_needed = session_obj->static_cal.is_registered;
        session_obj->dynamic_cal.is_calibrate_needed = session_obj->dynamic_cal.is_registered;
      }

      /* Init the cmd sequencer action */
      session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_REINIT_VOCPROC;

      /* Set the command status to pending */
      vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

   }

   /* Run the command sequencer for applying system config */
   for (;;)
   {
      switch (session_obj->session_cmd_ctrl.cmd_seq_action)
      {
         case VPM_ACTION_ENUM_REINIT_VOCPROC:
         {
           if (ADSP_EOK != (result = vpm_reinit_dyn_svcs(session_obj, FALSE)))
           {
              /* Re-init failed, end the APR command */
              break;
           }

           /* Set the next action */
           session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_WAIT_REINIT_DONE;

           /* Fall through */
         }
         case VPM_ACTION_ENUM_WAIT_REINIT_DONE:
         {
            /* Check if feature ID has changed */
            if ( session_obj->active_set.system_config.feature !=\
                   session_obj->target_set.system_config.feature )
            {
               if(session_obj->active_set.system_config.feature == VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE)
               {
                 /* Set the active feature to NONE for custom BWE modules and make way for the new feature. No need to send disable */
                 session_obj->active_set.system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;
               }

               /* Check if we need to disable any already enabled feature first,
                  because a new feature is requested or existing feature needs to be disabled */
               if (session_obj->active_set.system_config.feature != VSS_ICOMMON_CAL_FEATURE_NONE)
               {
                  /* Some feature was enabled earlier or target feature ID is NONE, then
                     disable the feature first.*/
                  session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_HDVOICE_DISABLE;
               }
               else if(session_obj->target_set.system_config.feature == VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE)
               {
                 /* Custom BWE modules need not be enabled. Set the next action */
                 session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_COMMON;
               }
               else if (session_obj->target_set.system_config.feature != VSS_ICOMMON_CAL_FEATURE_NONE)
               {
                  /* New feature needs to be enabled */
                  session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_HDVOICE_ENABLE;
               }
            }
            else /* No change in feature config */
            {
               /* Set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_COMMON;
            }

            /* Switch to next action to be executed */
            continue;
         }
         case VPM_ACTION_ENUM_HDVOICE_DISABLE:
         {
            if (ADSP_EOK != (result = vpm_send_hdvoice_enable_disable_msg(session_obj, FALSE)))
            {
               /* Failed to send disable msg to dyn svc, set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_COMMON;
               continue;
            }

            /* Set the next action */
            session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_HDVOICE_DISABLE_DONE;

            /* Return and wait for set param responses */
            return ADSP_EOK;
         }
         case VPM_ACTION_ENUM_HDVOICE_DISABLE_DONE:
         {
            /* Check the set param response status */
            if (ADSP_EOK != (result = session_obj->session_rsp_ctrl.status))
            {
               MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_set_system_config_cmd_ctrl(): WV disable, dyn svc ack'd error,"
                     "feature_id: 0x%lx, ack result: 0x%lx",
                     session_obj->active_set.system_config.feature, result);

               /* Set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_COMMON;
               continue;
            }

            /* Set the active feature to NONE after disable */
            session_obj->active_set.system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;

            /* If a new feature is requesed, enable it now. Custom BWE modules need not be enabled */
            if ( (session_obj->target_set.system_config.feature != VSS_ICOMMON_CAL_FEATURE_NONE) && 
                 (session_obj->target_set.system_config.feature != VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE) )
            {
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_HDVOICE_ENABLE;
            }
            else
            {
               /* Continue with all the calibrations */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_COMMON;
            }

            /* Switch to next action to be executed */
            continue;
         }
         case VPM_ACTION_ENUM_HDVOICE_ENABLE:
         {
            if (ADSP_EOK != (result = vpm_send_hdvoice_enable_disable_msg(session_obj, TRUE)))
            {
               /* Set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_COMMON;
               continue;
            }
            else
            {
               /* Set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_HDVOICE_ENABLE_DONE;

               /* Wait for set param response */
               return ADSP_EOK;
            }
         }
         case VPM_ACTION_ENUM_HDVOICE_ENABLE_DONE:
         {
            /* Check the set param response status */
            if (ADSP_EOK != (result = session_obj->session_rsp_ctrl.status))
            {
               MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_set_system_config_cmd_ctrl(): WV Enable, dyn svc ack'd error,"
                     "feature_id: 0x%lx, ack result: 0x%lx",
                     session_obj->target_set.system_config.feature, result);

               /* Set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_COMMON;
               continue;
            }

            /* Update the active feature ID after successful enable of WV */
            session_obj->active_set.system_config.feature =\
               session_obj->target_set.system_config.feature;

            /* Set the next action */
            session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_COMMON;

            /* Fall through */
         }
         case VPM_ACTION_ENUM_CALIBRATE_COMMON:
         {
            /* Set the next action */
            session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_VOLUME;

            /* BACKWARD COMPATIBILITY */
            /* Apply the common calibration data to VPM. */
            if (FALSE == session_obj->common_cal.is_calibrate_needed)
            {
               /* Skip applying common calibration as well as the WV/FENS
                * calibration (since WV/FENS are part of the common calibration).
                */
               continue;
            }

            /* If common calib failed, skip to volume calib */
            if (ADSP_EOK != (result = vpm_calibrate_common(session_obj)))
            {
               continue;
            }
            else
            {
               /* Return and wait for calib set param resposnes */
               return ADSP_EOK;
            }
         }
         case VPM_ACTION_ENUM_CALIBRATE_VOLUME:
         {
            /* BACKWARD COMPATIBILITY */
            /* Apply the volume calibration data to VPM. */

            /* Set next action. IF  volume calib fails or succeed, next action remains the same */
            session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_STATIC;

            /* If calib not needed, skip to next action */
            if (FALSE == session_obj->volume_cal.is_calibrate_needed)
            {
               continue;
            }

            /* When applying cached setting, set the ramp duartion to 0. */
            /* If calib fails, skip to next action */
            if (ADSP_EOK != (result = vpm_calibrate_volume(session_obj, 0)))
            {
               continue;
            }

            /* Return and wait for all set param responses for WVFENS cal */
            return ADSP_EOK;
         }
         case VPM_ACTION_ENUM_CALIBRATE_STATIC:
         {
            /* Apply the static calibration data to VPM. */
            if (FALSE == session_obj->static_cal.is_calibrate_needed)
            {
               /* Skip applying static calibration and finding the WV/FENS
                * calibration from the static calibration table. */

               /* Set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_DYNAMIC;

               continue;
            }

            if (ADSP_EOK != (result = vpm_calibrate_static(session_obj)))
            {
               /* Skip finding the WV/FENS calibration from the static calibration
                * table. */

               /* Set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_DYNAMIC;

               continue;
            }
            else
            {
               /* Set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_DYNAMIC;
               /* Wait for responses for set param for static cal */
               return ADSP_EOK;
            }
         }
         case VPM_ACTION_ENUM_CALIBRATE_DYNAMIC:
         {
            session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_GET_KPPS;
            (void)cvd_cal_query_deinit(session_obj->static_cal.query_handle);
            session_obj->static_cal.query_handle = CVD_CAL_INVALID_HANDLE_V;
            /* Apply the dynamic calibration data to VPM. */
            if (FALSE == session_obj->dynamic_cal.is_calibrate_needed)
            {
               /* Skip applying dynamic calibration and finding the FENS
                * calibration from the dynamic calibration table.
                */

               /* Set the next action */
               continue;
            }

            /* When applying cached setting, set the ramp duartion to 0. */
            if (ADSP_EOK != (result = vpm_calibrate_dynamic(session_obj, 0, VPM_DYNAMIC_CAL_USECASE_GENERIC)))
            {
               continue;
            }
            else /* calib success */
            {
               /* Wait for set param responses for all the set param */
               return ADSP_EOK;
            }
         }
         case VPM_ACTION_ENUM_GET_KPPS:
         {
            /* Set the next action */
            session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;
            (void)cvd_cal_query_deinit(session_obj->dynamic_cal.query_handle);
            session_obj->dynamic_cal.query_handle = CVD_CAL_INVALID_HANDLE_V;

            /* Retrieve the VPM's KPPS requirement. */
            if (ADSP_EOK != (result = vpm_send_get_kpps_cmd_to_dyn_svc(session_obj)))
            {
               /* Skip to next action */
               continue;
            }
            else
            {
               /* Wait for command response */
               return ADSP_EOK;
            }
         }
         case VPM_ACTION_ENUM_COMPLETE:
         {
            /* End the sequencer. */
            session_obj->common_cal.is_calibrate_needed = FALSE; /* BACKWARD COMPATIBILITY */
            session_obj->volume_cal.is_calibrate_needed = FALSE; /* BACKWARD COMPATIBILITY */
            session_obj->static_cal.is_calibrate_needed = FALSE;
            session_obj->dynamic_cal.is_calibrate_needed = FALSE;
            session_obj->is_vocproc_config_changed = FALSE;
            /* Reset the KPPS change flag */
            session_obj->is_kpps_changed = FALSE;

            /* Update active set of system config params */
            session_obj->active_set.system_config.network_id = session_obj->target_set.system_config.network_id;
            session_obj->active_set.system_config.media_id = session_obj->target_set.system_config.media_id;
            session_obj->active_set.system_config.rx_voc_op_mode = session_obj->target_set.system_config.rx_voc_op_mode;
            session_obj->active_set.system_config.tx_voc_op_mode = session_obj->target_set.system_config.tx_voc_op_mode;
            session_obj->active_set.system_config.tx_pp_sr = session_obj->target_set.system_config.tx_pp_sr;
            session_obj->active_set.system_config.rx_pp_sr = session_obj->target_set.system_config.rx_pp_sr;
            session_obj->active_set.system_config.feature = session_obj->target_set.system_config.feature;

            set_system_config_rsp.vp_rx_kpps = session_obj->kpps_info.vp_rx;
            set_system_config_rsp.vp_tx_kpps = session_obj->kpps_info.vp_tx;
            set_system_config_rsp.tx_pp_sr = session_obj->active_set.system_config.tx_pp_sr;
            set_system_config_rsp.rx_pp_sr = session_obj->active_set.system_config.rx_pp_sr;
            set_system_config_rsp.vocproc_tx_topology_id = session_obj->active_set.tx_topology_id;
            set_system_config_rsp.vocproc_rx_topology_id = session_obj->active_set.rx_topology_id;
            if ( session_obj->active_set.dev_chan_info.tx.num_channels > 0 ) 
            {
              set_system_config_rsp.tx_num_channels = session_obj->active_set.dev_chan_info.tx.num_channels;
            }
            else
            {
              set_system_config_rsp.tx_num_channels = session_obj->active_set.num_dev_channels.tx_num_channels;
            }
            set_system_config_rsp.tx_afe_port_sr = session_obj->tx_port_media_info.sampling_rate;
            set_system_config_rsp.rx_afe_port_sr = session_obj->rx_port_media_info.sampling_rate;
            set_system_config_rsp.ec_ref_afe_port_sr = session_obj->ec_ref_port_media_info.sampling_rate;

            set_system_config_rsp.vptx_frame_size_us = session_obj->vptx_frame_size_us;

            /* Retrieve MPPS and bus BW scaling factors from the device config table,
             * if one is registered with CVP. */
            vpm_get_clock_scale_factors( session_obj, &set_system_config_rsp );

            elite_apr_if_alloc_send_ack(session_obj->self_apr_addr,
                                        elite_apr_if_get_dst_addr(p_apr_packet),
                                        elite_apr_if_get_dst_port(p_apr_packet),
                                        elite_apr_if_get_src_addr(p_apr_packet),
                                        elite_apr_if_get_src_port(p_apr_packet),
                                        elite_apr_if_get_client_token(p_apr_packet),
                                        VSS_ICOMMON_RSP_SET_SYSTEM_CONFIG,
                                        &set_system_config_rsp,
                                        sizeof(set_system_config_rsp));

            /* Free the existing APR payload */
            elite_apr_if_free(session_obj->self_apr_addr, p_apr_packet);

            /* Remove the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            return ADSP_EOK;
         }
         default:
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_system_config_cmd_ctrl(): Invalid cmd seq state: 0x%lx",
                  session_obj->session_cmd_ctrl.cmd_seq_action);

            /* End the APR command */
            result = ADSP_EFAILED;
            break;
         }
      } /* switch (session_obj->session_cmd_ctrl.action) */

      break;
   } /* End of for (;;) */

   /* Clear the command pending status */
   vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

   /* End the APR command */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_packet, result);

   return result;
}


static ADSPResult vpm_set_dynamic_system_config_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult              result = ADSP_EOK;
   elite_apr_packet_t      *p_apr_pkt;
   int32_t                 rc;
   uint32_t                payload_size;
   vpm_cmd_ctrl_t          *p_cmd_ctrl;

   vss_icommon_cmd_set_dynamic_system_config_t *in_args;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   /* Check if session creation is complete or not */
   if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
   {
      /* End the APR command */
      elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_EFAILED);

      return ADSP_EFAILED;
   }

   p_cmd_ctrl = &session_obj->session_cmd_ctrl;

   if (FALSE == p_cmd_ctrl->is_prev_cmd_pending)
   {
      payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

      /* Validate inbound payload size */
      if (payload_size  != sizeof(vss_icommon_cmd_set_dynamic_system_config_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_set_dynamic_system_config_cmd_ctrl(): Incorrect cmd payload size: %lu != expected: %lu",
               payload_size, sizeof(vss_icommon_cmd_set_dynamic_system_config_t));

         elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_EBADPARAM);

         return ADSP_EBADPARAM;
      }

      in_args = (vss_icommon_cmd_set_dynamic_system_config_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

      session_obj->target_set.system_config.rx_voc_op_mode = in_args->rx_voc_op_mode;
      session_obj->target_set.system_config.tx_voc_op_mode = in_args->tx_voc_op_mode;
      session_obj->target_set.system_config.feature = in_args->feature_id;

      if( FALSE == session_obj->is_sr_set_by_client )
      {
        session_obj->target_set.system_config.rx_pp_sr = in_args->rx_pp_sr;
      }

      if( TRUE  == vpm_is_calibration_required( session_obj ) )
      {
         session_obj->dynamic_cal.is_calibrate_needed = session_obj->dynamic_cal.is_registered;
      }

      /* Init the cmd seq action for applying dynamic config */
      p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_CALIBRATE_DYNAMIC;

      /* Set the command status to pending */
      vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

   } /* End of if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */


   for (;;)
   {
      switch (p_cmd_ctrl->cmd_seq_action)
      {
         case VPM_ACTION_ENUM_CALIBRATE_DYNAMIC:
         {
            /* Set the next action */
            p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;
            /* Apply the dynamic calibration data to VPM. */
            if (FALSE == session_obj->dynamic_cal.is_calibrate_needed)
            {
               /* Skip applying dynamic calibration and finding the FENS
                * calibration from the dynamic calibration table.
                */
               continue;
            }

            if (ADSP_EOK != (result = vpm_calibrate_dynamic(session_obj, 0, VPM_DYNAMIC_CAL_USECASE_VOCODER)))
            {
               continue;
            }
            else
            {
               /* Wait for set param responses for dyn cal */
               return ADSP_EOK;
            }
         }
         case VPM_ACTION_ENUM_COMPLETE:
         {
            /* End the sequencer. */
            (void)cvd_cal_query_deinit(session_obj->dynamic_cal.query_handle);
            session_obj->dynamic_cal.query_handle = CVD_CAL_INVALID_HANDLE_V;
            session_obj->dynamic_cal.is_calibrate_needed = FALSE;

            session_obj->active_set.system_config.rx_voc_op_mode =\
               session_obj->target_set.system_config.rx_voc_op_mode;

            session_obj->active_set.system_config.tx_voc_op_mode =\
               session_obj->target_set.system_config.tx_voc_op_mode;

            session_obj->active_set.system_config.feature =\
               session_obj->target_set.system_config.feature;

            session_obj->active_set.system_config.rx_pp_sr =\
               session_obj->target_set.system_config.rx_pp_sr;

            result = ADSP_EOK;

            /* End the APR command */
            elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

            /* Set the command status to pending */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            return result;
         }
         default:
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_dynamic_system_config_cmd_ctrl(): Unexpected cmd seq action: 0x%lx, ",
                  p_cmd_ctrl->cmd_seq_action);

            elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_EFAILED);

            return ADSP_EFAILED;

         }
      } /* End of switch (p_cmd_ctrl->cmd_seq_action) */

      break;
   } /* End of for (;;) */

   return result;
}

static ADSPResult vpm_set_param_v2_cmd_ctrl(vpm_session_object_t  *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   ADSPResult           result2 = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t               rc;
   uint32_t              payload_size;
   uint32_t              expected_payload_size;
   uint8_t              *param_data_ptr = NULL;
   cvd_virt_addr_t       param_data_virt_addr;
   voice_set_param_v2_t  set_param;
   elite_apr_packet_t    *dst_packet;
   uint8_t               *dst_payload;
   uint32_t              dst_payload_size;
   uint32_t              dst_payload_left;
   uint32_t              vpm_mem_handle;
   uint32_t              param_id;
   uint16_t              param_size;
   uint8_t              *param_payload_data_ptr;
   bool_t               is_generic_params = FALSE;

   vpm_cmd_ctrl_t                   *p_cmd_ctrl;
   vss_icommon_cmd_set_param_v2_t   *payload;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   p_cmd_ctrl = &session_obj->session_cmd_ctrl;

   for (;;)
   {
      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Get the APR payload pointer and size */
         payload = (vss_icommon_cmd_set_param_v2_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate payload size */
         if (payload_size < sizeof(vss_icommon_cmd_set_param_v2_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_param_v2_cmd_ctrl(): Unexpected payload size, %lu < expected: %lu",
                  payload_size, sizeof(vss_icommon_cmd_set_param_v2_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* Validate the param mem size */
         if (payload->mem_size & 0x3)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_param_v2_cmd_ctrl(): Invalid param data size, %lu", payload->mem_size);

            result = ADSP_EBADPARAM;
            break;
         }

         /* Check if in-band payload */
         if (0 == payload->mem_handle)
         {
            /* Parameter data is in-band. */
            /* Expected size = set param header + payload size */
            expected_payload_size = (sizeof(vss_icommon_cmd_set_param_v2_t) + payload->mem_size);

            if (payload_size != expected_payload_size)
            {
               MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_set_param_v2_cmd_ctrl(): Unexpected payload size, %d != %d",
                     payload_size, expected_payload_size);

               /* End the APR command */
               result = ADSP_EBADPARAM;
               break;
            }

            /* Get the pointer to the payload */
            param_data_ptr = ((uint8_t *)payload);

            /* Point param data ptr to start of voice_param_data_t */
            param_data_ptr += sizeof(vss_icommon_cmd_set_param_v2_t);
            vpm_mem_handle = 0;

            param_id = *(uint32_t *)(param_data_ptr +  sizeof(uint32_t));
            param_size = *(uint32_t *)(param_data_ptr + (2 * (sizeof(uint32_t))));
            param_payload_data_ptr = (uint8_t *)(param_data_ptr + (3 * (sizeof(uint32_t))));

            MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "CVD_CAL_MSG_PARAM: vpm_set_param_v2_cmd_ctrl(): Param ID[0]=0x%08x",
                  param_id);

            if (param_size >= 16)
            {
               MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "CVD_CAL_MSG_PARAM: vpm_set_param_v2_cmd_ctrl(): Payload[0-2]=0x%08x, "\
                        "0x%08x, 0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t)),
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t) + sizeof(uint32_t)),
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t) + (2 * sizeof(uint32_t))));

               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "CVD_CAL_MSG_PARAM: vpm_set_param_v2_cmd_ctrl(): Payload[3]=0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t) + (3 * sizeof(uint32_t))));
            }
            else if (param_size >= 4)
            {
               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "CVD_CAL_MSG_PARAM: vpm_set_param_v2_cmd_ctrl(): Payload[0]=0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t)));
            }
         }
         else /* Parameter data is out-of-band. */
         {
            if (ADSP_EOK != (result = vpm_validate_calib_payload(payload->mem_handle,
                                                                 payload->mem_size,
                                                                 payload->mem_address)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_set_param_v2_cmd_ctrl(): Failed to validate calib payload: result: 0x%lx",
                     result);

               /* End the APR command */
               break;
            }

            (void)cvd_mem_mapper_get_vpm_mem_handle(payload->mem_handle, &vpm_mem_handle);

            rc = cvd_mem_mapper_get_virtual_addr_v2(payload->mem_handle,
                                                    payload->mem_address,
                                                    &param_data_virt_addr);
            if (rc)
            {
               MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_set_param_v2_cmd_ctrl(): Cannot get virtual address,"
                     " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
                     (uint32_t)payload->mem_address, (uint32_t)(payload->mem_address >> 32));

               result = ADSP_EFAILED;
               break;
            }

            /* Invalidate the cache */
            (void)cvd_mem_mapper_cache_invalidate_v2(&param_data_virt_addr, payload->mem_size);

            param_id = *(uint32_t *)((uint8_t *)param_data_virt_addr.ptr +  sizeof(uint32_t));
            param_size = *(uint32_t *)((uint8_t *)param_data_virt_addr.ptr + (2 * (sizeof(uint32_t))));
            param_payload_data_ptr =  ((uint8_t *)param_data_virt_addr.ptr + (3 * (sizeof(uint32_t))));

            param_data_ptr = (uint8_t *)((uint32_t)param_data_virt_addr.ptr);

            MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "CVD_CAL_MSG_PARAM: vpm_set_param_v2_cmd_ctrl(): Param ID[0]=0x%08x",
                  param_id);

            if (param_size >= 16)
            {
               MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO, "CVD_CAL_MSG_PARAM: vpm_set_param_v2_cmd_ctrl(): Payload[0-2]=0x%08x, "\
                        "0x%08x, 0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t)),
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t) + sizeof(uint32_t)),
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t) + (2 * sizeof(uint32_t))));

               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "CVD_CAL_MSG_PARAM: vpm_set_param_v2_cmd_ctrl(): Payload[3]=0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t) + (3 * sizeof(uint32_t))));
            }
            else if (param_size >= 4)
            {
               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "CVD_CAL_MSG_PARAM: vpm_set_param_v2_cmd_ctrl(): Payload[0]=0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t)));
            }
         } /*End of else  Parameter data is out-of-band. */

         dst_payload_size = payload->mem_size;
         
         /* check for VSS_MODULE_CVD_GENERIC mid-pids */
         result2 = vpm_parse_generic_module_id_params_and_set_param(session_obj,
                                                                    param_data_ptr,
                                                                    dst_payload_size,
                                                                    &is_generic_params);         
         if (is_generic_params)
         {
            MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                  "vpm_set_param_v2_cmd_ctrl(): set param payload has generic module params. not sending to dyn svc");
            elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result2);
            return ADSP_EOK;
         }

         /* Set the command response pending status */
         vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                   param_data_ptr,
                                                                   dst_payload_size,
                                                                   ELITE_CMD_SET_PARAM,
                                                                   CVD_CAL_PARAM_MINOR_VERSION_0)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_param_v2_cmd_ctrl(): Failed to send set param to dyn svcs, result: 0x%lx",
                  result);
         }

         /* Check if at least 1 set param sent successfully */
         if (!session_obj->session_rsp_ctrl.num_cmd_issued)
         {
            /* Clear the command response pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

            elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_EFAILED);

            return ADSP_EFAILED;
         }
         else /* At least 1 set param succeeded */
         {
            /* Set the command status to pending */
            vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

            /* Set the command sequencer next action */
            p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_WAIT_SETPARAM_DONE;

            /* Return and wait for all the set param responses*/
            return ADSP_EOK;
         }

      } /* End of if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)*/

      /* Control will reach here after getting responses for all set params */

      /* If this vocproc is running and it is attached to an MVM session, send
       * VSS_IVOCPROC_EVT_RECONFIG event to MVM if there is a KPPS change
       * due to set_param.
       * Note:
       * 1. The vocproc's handling for VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG is
       *    optimized such that calibration data is not applied unless there
       *    is a need. Therefore we won't overwrite the params that has been
       *    set by set_param with calibration data.
       * 2. It is only possible to receive a set_param from external CVD
       *    client when this vocproc is in RUN state, because set_param is
       *    only expected to be used for real-time calibration purpose.
       */

      for (;;)
      {
         switch (p_cmd_ctrl->cmd_seq_action)
         {
            case VPM_ACTION_ENUM_WAIT_SETPARAM_DONE:
            {
               /* If VPM running , MVM is attached and set param succeeded */
               if ((VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state) &&
                   (APR_NULL_V != session_obj->attached_mvm_handle) &&
                   (ADSP_EOK == session_obj->session_rsp_ctrl.status))
               {
                  p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_GET_KPPS;
               }
               else
               {
                  /* Else nothing to do, end the cmd sequencer */
                  p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;
                  continue;
               }
               /*-fallthru */
            }
            case VPM_ACTION_ENUM_GET_KPPS:
            {
               /* Retrieve the VPM's KPPS requirement. */
               if (ADSP_EOK != (result = vpm_send_get_kpps_cmd_to_dyn_svc(session_obj)))
               {
                  /* Set the next action */
                  p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;

                  /* Skip to next action */
                  continue;
               }
               else
               {
                  /* Set the next action */
                  p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_GET_KPPS_DONE;

                  MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                      "CVD_CAL_MSG_PARAM: vpm_set_param_v2_cmd_ctrl(): sent get_kpps to VPM");

                  /* Return and Wait for command response */
                  return ADSP_EOK;
               }

               /*-fallthru */
            }
            case VPM_ACTION_ENUM_GET_KPPS_DONE:
            {
               if (TRUE == session_obj->is_kpps_changed)
               {
                  /* Send reconfig event to MVM */
                  elite_apr_if_alloc_send_event(session_obj->self_apr_handle,
                                                session_obj->self_apr_addr,
                                                session_obj->self_apr_port,
                                                session_obj->mvm_apr_addr,
                                                session_obj->attached_mvm_handle,
                                                0,
                                                VSS_IVOCPROC_EVT_RECONFIG,
                                                NULL,
                                                0);

                  MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                      "CVD_CAL_MSG_PARAM: vpm_set_param_v2_cmd_ctrl(): sent reconfig to MVM");

                  /* Reset the KPPS change flag */
                  session_obj->is_kpps_changed = FALSE;
               }

               /* Set the next action */
               p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;

               /*-fallthru */
            }
            case VPM_ACTION_ENUM_COMPLETE:
            {
               /* End the command sequencer. */

               /* Clear the command pending status */
               vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

               /* Get the aggregated set param response */
               result = session_obj->session_rsp_ctrl.status;

               /* End the APR command */
               elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "CVD_CAL_MSG_PARAM: vpm_set_param_v2_cmd_ctrl(): COMPLETED, result: 0x%lx",
                     result);

               return ADSP_EOK;
            }
            default:
            {
               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "vpm_set_param_v2_cmd_ctrl(): Unexpected cmd seq state: 0x%lx", p_cmd_ctrl->cmd_seq_action);

               /* Clear the command pending status */
               vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

               elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_EFAILED);
               return ADSP_EFAILED;

            }
         } /* End of switch (p_cmd_ctrl->cmd_seq_action) */

         break;
      } /* End of inner for (;;) */

      break;
   } /* End of outer for (;;) */

   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_set_param_v3_cmd_ctrl(vpm_session_object_t  *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   ADSPResult           result2 = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t               rc;
   uint32_t              payload_size;
   uint32_t              expected_payload_size;
   uint8_t              *param_data_ptr = NULL;
   cvd_virt_addr_t       param_data_virt_addr;
   voice_set_param_v2_t  set_param;
   elite_apr_packet_t    *dst_packet;
   uint8_t               *dst_payload;
   uint32_t              dst_payload_size;
   uint32_t              dst_payload_left;
   uint32_t              vpm_mem_handle;
   uint32_t              param_id;
   uint16_t              instance_id;
   uint16_t              param_size;
   uint8_t              *param_payload_data_ptr;
   bool_t               is_generic_params = FALSE;

   vpm_cmd_ctrl_t                   *p_cmd_ctrl;
   vss_icommon_cmd_set_param_v3_t   *payload;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   p_cmd_ctrl = &session_obj->session_cmd_ctrl;
   #define INSTANCE_ID_OFFSET ( sizeof(uint32_t) )
   #define PARAM_ID_OFFSET ( 2 * sizeof(uint32_t) )
   #define PARAM_SIZE_OFFSET ( 3 * sizeof(uint32_t) )
   #define PARAM_DATA_OFFSET ( 4 * sizeof(uint32_t) )

   for (;;)
   {
      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Get the APR payload pointer and size */
         payload = (vss_icommon_cmd_set_param_v3_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate payload size */
         if (payload_size < sizeof(vss_icommon_cmd_set_param_v3_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_param_v3_cmd_ctrl(): Unexpected payload size, %lu < expected: %lu",
                  payload_size, sizeof(vss_icommon_cmd_set_param_v3_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* Validate the param mem size */
         if (payload->mem_size & 0x3)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_param_v3_cmd_ctrl(): Invalid param data size, %lu", payload->mem_size);

            result = ADSP_EBADPARAM;
            break;
         }

         /* Check if in-band payload */
         if (0 == payload->mem_handle)
         {
            /* Parameter data is in-band. */
            /* Expected size = set param header + payload size */
            expected_payload_size = (sizeof(vss_icommon_cmd_set_param_v3_t) + payload->mem_size);

            if (payload_size != expected_payload_size)
            {
               MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_set_param_v3_cmd_ctrl(): Unexpected payload size, %d != %d",
                     payload_size, expected_payload_size);

               /* End the APR command */
               result = ADSP_EBADPARAM;
               break;
            }

            /* Get the pointer to the payload */
            param_data_ptr = ((uint8_t *)payload);

            /* Point param data ptr to start of voice_param_data_t */
            param_data_ptr += sizeof(vss_icommon_cmd_set_param_v3_t);
            vpm_mem_handle = 0;

            instance_id = *(uint16_t *)(param_data_ptr +  INSTANCE_ID_OFFSET );
            param_id = *(uint32_t *)(param_data_ptr +  PARAM_ID_OFFSET );
            param_size = *(uint32_t *)(param_data_ptr + PARAM_SIZE_OFFSET );
            param_payload_data_ptr = (uint8_t *)(param_data_ptr + PARAM_DATA_OFFSET );

            MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "CVD_CAL_MSG_PARAM: vpm_set_param_v3_cmd_ctrl(): Param ID[0]=0x%08x instance ID[0]=0x%08x",
                  param_id, instance_id);

            if (param_size >= 16)
            {
               MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "CVD_CAL_MSG_PARAM: vpm_set_param_v3_cmd_ctrl(): Payload[0-2]=0x%08x, "\
                        "0x%08x, 0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_v2_t)),
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_v2_t) + sizeof(uint32_t)),
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_v2_t) + (2 * sizeof(uint32_t))));

               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "CVD_CAL_MSG_PARAM: vpm_set_param_v3_cmd_ctrl(): Payload[3]=0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_v2_t) + (3 * sizeof(uint32_t))));
            }
            else if (param_size >= 4)
            {
               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "CVD_CAL_MSG_PARAM: vpm_set_param_v3_cmd_ctrl(): Payload[0]=0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t)));
            }
         }
         else /* Parameter data is out-of-band. */
         {
            if (ADSP_EOK != (result = vpm_validate_calib_payload(payload->mem_handle,
                                                                 payload->mem_size,
                                                                 payload->mem_address)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_set_param_v3_cmd_ctrl(): Failed to validate calib payload: result: 0x%lx",
                     result);

               /* End the APR command */
               break;
            }

            (void)cvd_mem_mapper_get_vpm_mem_handle(payload->mem_handle, &vpm_mem_handle);

            rc = cvd_mem_mapper_get_virtual_addr_v2(payload->mem_handle,
                                                    payload->mem_address,
                                                    &param_data_virt_addr);
            if (rc)
            {
               MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_set_param_v3_cmd_ctrl(): Cannot get virtual address,"
                     " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
                     (uint32_t)payload->mem_address, (uint32_t)(payload->mem_address >> 32));

               result = ADSP_EFAILED;
               break;
            }

            /* Invalidate the cache */
            (void)cvd_mem_mapper_cache_invalidate_v2(&param_data_virt_addr, payload->mem_size);

            param_id = *(uint32_t *)((uint8_t *)param_data_virt_addr.ptr +  PARAM_ID_OFFSET );
            param_size = *(uint32_t *)((uint8_t *)param_data_virt_addr.ptr + PARAM_SIZE_OFFSET );
            param_payload_data_ptr =  ((uint8_t *)param_data_virt_addr.ptr + PARAM_DATA_OFFSET );

            param_data_ptr = (uint8_t *)((uint32_t)param_data_virt_addr.ptr);

            MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "CVD_CAL_MSG_PARAM: vpm_set_param_v3_cmd_ctrl(): Param ID[0]=0x%08x",
                  param_id);

            if (param_size >= 16)
            {
               MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO, "CVD_CAL_MSG_PARAM: vpm_set_param_v3_cmd_ctrl(): Payload[0-2]=0x%08x, "\
                        "0x%08x, 0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_v2_t)),
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_v2_t) + sizeof(uint32_t)),
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_v2_t) + (2 * sizeof(uint32_t))));

               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "CVD_CAL_MSG_PARAM: vpm_set_param_v3_cmd_ctrl(): Payload[3]=0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_v2_t) + (3 * sizeof(uint32_t))));
            }
            else if (param_size >= 4)
            {
               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "CVD_CAL_MSG_PARAM: vpm_set_param_v3_cmd_ctrl(): Payload[0]=0x%08x",
                     *(uint32_t *)(param_payload_data_ptr + sizeof(voice_param_data_t)));
            }
         } /*End of else  Parameter data is out-of-band. */

         dst_payload_size = payload->mem_size;
         
         /* check for VSS_MODULE_CVD_GENERIC mid-pids */
         result2 = vpm_parse_generic_module_id_params_and_set_param_v2(session_obj,
                                                                       param_data_ptr,
                                                                       dst_payload_size,
                                                                       &is_generic_params);         
         if (is_generic_params)
         {
            MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                  "vpm_set_param_v3_cmd_ctrl(): set param payload has generic module params. not sending to dyn svc");
            elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result2);
            return ADSP_EOK;
         }

         /* Set the command response pending status */
         vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                   param_data_ptr,
                                                                   dst_payload_size,
                                                                   ELITE_CMD_SET_PARAM,
                                                                   CVD_CAL_PARAM_MINOR_VERSION_1)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_set_param_v3_cmd_ctrl(): Failed to send set param to dyn svcs, result: 0x%lx",
                  result);
         }

         /* Check if at least 1 set param sent successfully */
         if (!session_obj->session_rsp_ctrl.num_cmd_issued)
         {
            /* Clear the command response pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

            elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_EFAILED);

            return ADSP_EFAILED;
         }
         else /* At least 1 set param succeeded */
         {
            /* Set the command status to pending */
            vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

            /* Set the command sequencer next action */
            p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_WAIT_SETPARAM_DONE;

            /* Return and wait for all the set param responses*/
            return ADSP_EOK;
         }

      } /* End of if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)*/

      /* Control will reach here after getting responses for all set params */

      /* If this vocproc is running and it is attached to an MVM session, send
       * VSS_IVOCPROC_EVT_RECONFIG event to MVM if there is a KPPS change
       * due to set_param.
       * Note:
       * 1. The vocproc's handling for VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG is
       *    optimized such that calibration data is not applied unless there
       *    is a need. Therefore we won't overwrite the params that has been
       *    set by set_param with calibration data.
       * 2. It is only possible to receive a set_param from external CVD
       *    client when this vocproc is in RUN state, because set_param is
       *    only expected to be used for real-time calibration purpose.
       */

      for (;;)
      {
         switch (p_cmd_ctrl->cmd_seq_action)
         {
            case VPM_ACTION_ENUM_WAIT_SETPARAM_DONE:
            {
               /* If VPM running , MVM is attached and set param succeeded */
               if ((VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state) &&
                   (APR_NULL_V != session_obj->attached_mvm_handle) &&
                   (ADSP_EOK == session_obj->session_rsp_ctrl.status))
               {
                  p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_GET_KPPS;
               }
               else
               {
                  /* Else nothing to do, end the cmd sequencer */
                  p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;
                  continue;
               }
               /*-fallthru */
            }
            case VPM_ACTION_ENUM_GET_KPPS:
            {
               /* Retrieve the VPM's KPPS requirement. */
               if (ADSP_EOK != (result = vpm_send_get_kpps_cmd_to_dyn_svc(session_obj)))
               {
                  /* Set the next action */
                  p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;

                  /* Skip to next action */
                  continue;
               }
               else
               {
                  /* Set the next action */
                  p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_GET_KPPS_DONE;

                  MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                      "CVD_CAL_MSG_PARAM: vpm_set_param_v3_cmd_ctrl(): sent get_kpps to VPM");

                  /* Return and Wait for command response */
                  return ADSP_EOK;
               }

               /*-fallthru */
            }
            case VPM_ACTION_ENUM_GET_KPPS_DONE:
            {
               if (TRUE == session_obj->is_kpps_changed)
               {
                  /* Send reconfig event to MVM */
                  elite_apr_if_alloc_send_event(session_obj->self_apr_handle,
                                                session_obj->self_apr_addr,
                                                session_obj->self_apr_port,
                                                session_obj->mvm_apr_addr,
                                                session_obj->attached_mvm_handle,
                                                0,
                                                VSS_IVOCPROC_EVT_RECONFIG,
                                                NULL,
                                                0);

                  MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                      "CVD_CAL_MSG_PARAM: vpm_set_param_v3_cmd_ctrl(): sent reconfig to MVM");

                  /* Reset the KPPS change flag */
                  session_obj->is_kpps_changed = FALSE;
               }

               /* Set the next action */
               p_cmd_ctrl->cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;

               /*-fallthru */
            }
            case VPM_ACTION_ENUM_COMPLETE:
            {
               /* End the command sequencer. */

               /* Clear the command pending status */
               vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

               /* Get the aggregated set param response */
               result = session_obj->session_rsp_ctrl.status;

               /* End the APR command */
               elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "CVD_CAL_MSG_PARAM: vpm_set_param_v3_cmd_ctrl(): COMPLETED, result: 0x%lx",
                     result);

               return ADSP_EOK;
            }
            default:
            {
               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "vpm_set_param_v3_cmd_ctrl(): Unexpected cmd seq state: 0x%lx", p_cmd_ctrl->cmd_seq_action);

               /* Clear the command pending status */
               vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

               elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_EFAILED);
               return ADSP_EFAILED;

            }
         } /* End of switch (p_cmd_ctrl->cmd_seq_action) */

         break;
      } /* End of inner for (;;) */

      break;
   } /* End of outer for (;;) */

   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   #undef INSTANCE_ID_OFFSET 
   #undef PARAM_ID_OFFSET 
   #undef PARAM_SIZE_OFFSET 
   #undef PARAM_DATA_OFFSET 

   return result;
}


static ADSPResult vpm_get_param_v2_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   ADSPResult           result2 = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;
   uint32_t             payload_size;
   uint64_t             param_data_virt_addr = 0;
   voice_get_param_v2_t get_param;
   uint32_t             vpm_mem_handle;
   void                 *p_get_param_rsp;
   voice_param_data_t   *p_voice_param_data;
   bool_t               is_generic_params;

   vss_icommon_cmd_get_param_v2_t *payload;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Get the APR payload pointer and size */
         payload = (vss_icommon_cmd_get_param_v2_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate the inbound payload size */
         if (payload_size != sizeof(vss_icommon_cmd_get_param_v2_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_get_param_v2_cmd_ctrl(): Unexpected payload size, %d != expected: %d",
                  payload_size, sizeof(vss_icommon_cmd_get_param_v2_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* Set the command pending status */
         vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

         /* Set command rsp pending status */
         vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

         /* Set the get param response handler */
         session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_get_param_cmd_rsp_fwd;

         /* Check if at least 1 get param was issued */
         if (session_obj->session_rsp_ctrl.num_cmd_issued)
         {
            return ADSP_EOK;
         }

         /* Validate payload size */
         if (payload->mem_size & 0x3)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_get_param_v2_cmd_ctrl(): Invalid param data size, %d",
                  payload->mem_size);

            result = ADSP_EBADPARAM;
            break;
         }

         /* If in-band payload */
         if (!payload->mem_handle)
         {
            /* Parameter data is in-band. */
            vpm_mem_handle = 0;

            /* Allocate the response payload for in-band get param */
            /* Expected payload_size = Payload size (Voice param header + data) + status field */
            if (NULL == (p_get_param_rsp = qurt_elite_memory_malloc(payload->mem_size + sizeof(voice_get_param_ack_t),
                                                                    QURT_ELITE_HEAP_DEFAULT)))
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                   "vpm_get_param_v2_cmd_ctrl(): Failed to allocate in-band get param rsp payload");

               result = ADSP_EFAILED;
               break;
            }
            else
            {
               /* Get the pointer to start of voice param data. Payload starts with ACK status */
               p_voice_param_data = (voice_param_data_t *)((uint8_t *)p_get_param_rsp +
                                                           sizeof(voice_get_param_ack_t));

               /* Save the pointer to APR packet in response control */
               session_obj->session_rsp_ctrl.p_ack_payload = p_get_param_rsp;
            }
         }
         else /* Out of band */
         {
            /* Parameter data is out-of-band. */

            if (ADSP_EOK != (result = vpm_validate_calib_payload(payload->mem_handle,
                                                                 payload->mem_size,
                                                                 payload->mem_address)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_register_device_config_cmd_ctrl(): Failed to validate calib payload: result: 0x%lx",
                     result);

               /* End the APR command */
               break;
            }

            (void)cvd_mem_mapper_get_vpm_mem_handle(payload->mem_handle, &vpm_mem_handle);

            rc = cvd_mem_mapper_validate_attributes_align(payload->mem_handle, payload->mem_size);

            if (rc)
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_get_param_v2_cmd_ctrl():"
                     " Mis-aligned mem size: 0x%08X", payload->mem_size);

               result = ADSP_EBADPARAM;
               break;
            }

            rc = cvd_mem_mapper_get_virtual_addr(payload->mem_handle,
                                                 payload->mem_address,
                                                 &param_data_virt_addr);
            if (rc)
            {
               MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_get_param_v2_cmd_ctrl(): Cannot get virtual address,"
                     " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
                     (uint32_t)payload->mem_address, (uint32_t)(payload->mem_address >> 32));

               result = ADSP_EFAILED;
               break;
            }

            /* Get the payload pointer */
            p_voice_param_data = (voice_param_data_t *)((uint32_t)param_data_virt_addr);

         } /* End of else (out of band)*/


         /* Populate the voice data param header */
         p_voice_param_data->module_id = payload->module_id;
         p_voice_param_data->param_id = payload->param_id;
         p_voice_param_data->param_size = payload->mem_size;
         p_voice_param_data->reserved = 0;

         /* check for VSS_MODULE_CVD_GENERIC mid-pids */
         result2 = vpm_parse_generic_module_id_params_and_get_param(session_obj,
                                                                    p_voice_param_data,
                                                                    payload->mem_size,
                                                                    &is_generic_params);

         if (is_generic_params)
         {            
            MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                  "vpm_get_param_v2_cmd_ctrl(): get param payload has generic module params. not sending to dyn svc");
            elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result2);
            session_obj->session_rsp_ctrl.status = result2;   
            (void)vpm_get_param_cmd_rsp_handler((void *)session_obj);
            return ADSP_EOK;
         }
         else
         {
            MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
               "CVD_CAL_MSG_PARAM: Sending GET_PARAM_V2 to dyn svcs. "
               "vpm_get_param_v2_cmd_ctrl(): Token: %d Param ID: 0x%08x, mem_size: %lu",
               elite_apr_if_get_client_token(p_apr_pkt), p_voice_param_data->param_id,
               p_voice_param_data->param_size);
         }         

         /* Note that command rsp pending status is set before stream get param */
         /* Set the get param response handler */
         session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_get_param_cmd_rsp_handler;

         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                   p_voice_param_data,
                                                                   payload->mem_size,
                                                                   ELITE_CMD_GET_PARAM,
                                                                   CVD_CAL_PARAM_MINOR_VERSION_0)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "CVD_CAL_MSG_PARAM: vpm_get_param_v2_cmd_ctrl():"
                  " Failed to send get param msg to dyn svcs, result: 0x%lx",
                  result);
         }

         /* Check if at least 1 set param was issued */
         if (!session_obj->session_rsp_ctrl.num_cmd_issued)
         {
            /* Clear the cmd rsp pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

            /* Clear the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            result = ADSP_EFAILED;
            break;
         }

         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "CVD_CAL_MSG_PARAM: Sent GET_PARAM_V2 to dyn svcs. vpm_get_param_v2_cmd_ctrl():"
               " MID: 0x%08x, PID: x%08x",
               payload->module_id, payload->param_id);

         /* Else wait for get param responses */

         return ADSP_EOK;

      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after getting respones for all the
         get params */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Free the current command apr packet */
      elite_apr_if_free(session_obj->self_apr_handle, p_apr_pkt);

      return result;

   } /* End of for (;;) */

   /* In case of error, end the APR command */

   /* Set the command pending status */
   vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

   /* Set command rsp pending status */
   vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return  result;
}

static ADSPResult vpm_get_param_v3_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   uint32_t             payload_size;
   uint64_t             param_data_virt_addr = 0;
   uint32_t             vpm_mem_handle;
   void                 *p_get_param_rsp;
   voice_param_data_v2_t   *p_voice_param_data;
   bool_t               is_generic_params;

   vss_icommon_cmd_get_param_v3_t *payload;
   uint32_t version = 1;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Get the APR payload pointer and size */
         payload = (vss_icommon_cmd_get_param_v3_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate the inbound payload size */
         if (payload_size != sizeof(vss_icommon_cmd_get_param_v3_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_get_param_v3_cmd_ctrl(): Unexpected payload size, %d != expected: %d",
                  payload_size, sizeof(vss_icommon_cmd_get_param_v3_t));

            /* End the APR command */
            result = ADSP_EBADPARAM;
            break;
         }

         /* Set the command pending status */
         vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

         /* Set command rsp pending status */
         vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

         /* Set the get param response handler */
         session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_get_param_v3_cmd_rsp_fwd;

         /* Check if at least 1 get param was issued */
         if (session_obj->session_rsp_ctrl.num_cmd_issued)
         {
            return ADSP_EOK;
         }

         /* Validate payload size */
         if (payload->mem_size & 0x3)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_get_param_v3_cmd_ctrl(): Invalid param data size, %d",
                  payload->mem_size);

            result = ADSP_EBADPARAM;
            break;
         }

         /* If in-band payload */
         if (!payload->mem_handle)
         {
            /* Parameter data is in-band. */
            vpm_mem_handle = 0;

            /* Allocate the response payload for in-band get param */
            /* Expected payload_size = Payload size (Voice param header + data) + status field */
            if (NULL == (p_get_param_rsp = qurt_elite_memory_malloc(payload->mem_size + sizeof(vss_icommon_rsp_get_param_v3_t),
                                                                    QURT_ELITE_HEAP_DEFAULT)))
            {
               MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                   "vpm_get_param_v3_cmd_ctrl(): Failed to allocate in-band get param rsp payload");

               result = ADSP_EFAILED;
               break;
            }
            else
            {
               /* Get the pointer to start of voice param data. Payload starts with ACK status */
               p_voice_param_data = (voice_param_data_v2_t *)((uint8_t *)p_get_param_rsp +
                                                           sizeof(vss_icommon_rsp_get_param_v3_t));

               /* Save the pointer to APR packet in response control */
               session_obj->session_rsp_ctrl.p_ack_payload = p_get_param_rsp;
            }
         }
         else /* Out of band */
         {
            /* Parameter data is out-of-band. */

            if (ADSP_EOK != (result = vpm_validate_calib_payload(payload->mem_handle,
                                                                 payload->mem_size,
                                                                 payload->mem_address)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_get_param_v3_cmd_ctrl(): Failed to validate calib payload: result: 0x%lx",
                     result);

               /* End the APR command */
               break;
            }

            (void)cvd_mem_mapper_get_vpm_mem_handle(payload->mem_handle, &vpm_mem_handle);

            result = cvd_mem_mapper_validate_attributes_align(payload->mem_handle, payload->mem_size);

            if ( result )
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_get_param_v3_cmd_ctrl():"
                     " Mis-aligned mem size: 0x%08X", payload->mem_size);

               result = ADSP_EBADPARAM;
               break;
            }

            result = cvd_mem_mapper_get_virtual_addr(payload->mem_handle,
                                                 payload->mem_address,
                                                 &param_data_virt_addr);
            if ( result )
            {
               MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_get_param_v3_cmd_ctrl(): Cannot get virtual address,"
                     " mem_handle: 0x%08X, addr: lsw: 0x%08X, msw: 0x%08X", payload->mem_handle,
                     (uint32_t)payload->mem_address, (uint32_t)(payload->mem_address >> 32));

               result = ADSP_EFAILED;
               break;
            }

            /* Get the payload pointer */
            p_voice_param_data = (voice_param_data_v2_t *)((uint32_t)param_data_virt_addr);

         } /* End of else (out of band)*/


         /* Populate the voice data param header */
         p_voice_param_data->module_id = payload->module_id;
         p_voice_param_data->instance_id = payload->instance_id;
         p_voice_param_data->reserved = 0;
         p_voice_param_data->param_id = payload->param_id;
         p_voice_param_data->param_size = payload->mem_size;

         /* check for VSS_MODULE_CVD_GENERIC mid-pids */
         result = vpm_parse_generic_module_id_params_and_get_param_v2(session_obj,
                                                                    p_voice_param_data,
                                                                    payload->mem_size,
                                                                    &is_generic_params);

         if (is_generic_params)
         {            
            MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                  "vpm_get_param_v3_cmd_ctrl(): get param payload has generic module params. not sending to dyn svc");
            elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);
            session_obj->session_rsp_ctrl.status = result;   
            (void)vpm_get_param_v3_cmd_rsp_handler((void *)session_obj);
            return ADSP_EOK;
         }
         else
         {
            MSG_3(MSG_SSID_DFLT, DBG_MED_PRIO,
               "CVD_CAL_MSG_PARAM: Sending GET_PARAM_V3 to dyn svcs. "
               "vpm_get_param_v3_cmd_ctrl(): Token: %d Param ID: 0x%08x, mem_size: %lu",
               elite_apr_if_get_client_token(p_apr_pkt), p_voice_param_data->param_id,
               p_voice_param_data->param_size);
         }         

         /* Note that command rsp pending status is set before stream get param */
         /* Set the get param response handler */
         session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_get_param_v3_cmd_rsp_handler;

         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc( session_obj,
                                                                    p_voice_param_data,
                                                                    payload->mem_size,
                                                                    ELITE_CMD_GET_PARAM,
                                                                    CVD_CAL_PARAM_MINOR_VERSION_1 )))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "CVD_CAL_MSG_PARAM: vpm_get_param_v3_cmd_ctrl():"
                  " Failed to send get param msg to dyn svcs, result: 0x%lx",
                  result);
         }

         /* Check if at least 1 set param was issued */
         if (!session_obj->session_rsp_ctrl.num_cmd_issued)
         {
            /* Clear the cmd rsp pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

            /* Clear the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            result = ADSP_EFAILED;
            break;
         }

         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "CVD_CAL_MSG_PARAM: Sent GET_PARAM_V3 to dyn svcs. vpm_get_param_v3_cmd_ctrl():"
               " MID: 0x%08x, PID: x%08x",
               payload->module_id, payload->param_id);

         /* Else wait for get param responses */

         return ADSP_EOK;

      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after getting respones for all the
         get params */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Free the current command apr packet */
      elite_apr_if_free(session_obj->self_apr_handle, p_apr_pkt);

      return result;

   } /* End of for (;;) */

   /* In case of error, end the APR command */

   /* Set the command pending status */
   vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

   /* Set command rsp pending status */
   vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return  result;
}

static ADSPResult vpm_mvm_attach_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   elite_apr_port_t     src_apr_port;

   for (;;)
   {
      p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      src_apr_port = elite_apr_if_get_src_port(p_apr_pkt);

      if (session_obj->attached_mvm_handle == src_apr_port)
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_mvm_attach_cmd_ctrl(): VPM is already "
               "attached to the MVM session handle: 0x%04X.",
               src_apr_port);

         /* End the APR command */
         result = ADSP_EALREADY;
         break;
      }

      /* Check if this VPM session is already attached to MVM */
      if (session_obj->attached_mvm_handle != APR_NULL_V)
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_mvm_attach_cmd_ctrl(): VPM already attached "
               "to MVM session handle: 0x%04X, cannot attach "
               "to new MVM session handle: 0x%04X",
               session_obj->attached_mvm_handle,
               src_apr_port);

         /* End the APR command */
         result = ADSP_EFAILED;
         break;
      }

      /* Save the MVM handle*/
      session_obj->attached_mvm_handle = src_apr_port;

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_mvm_attach_cmd_ctrl(): MVM attach success, MVM handle: 0x%lx",
            session_obj->attached_mvm_handle);

      result = ADSP_EOK;
      break;
   }

   /* End the APR command */
   elite_apr_if_end_cmd(session_obj->self_apr_handle,
                        p_apr_pkt, result);

   return result;
}


static ADSPResult vpm_mvm_detach_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (NULL == session_obj->attached_mvm_handle)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_mvm_detach_cmd_ctrl(): Vocproc is currently not attached "\
                "to any MVM session handle.");

         result = ADSP_EALREADY;
         break;
      }

      /* Check if the detach is called for MVM session already attached */
      if (session_obj->attached_mvm_handle != elite_apr_if_get_src_port(p_apr_packet))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_mvm_detach_cmd_ctrl(): Vocproc is currently attached to "\
                  "the MVM session handle 0x%04X, and the caller MVM session's "\
                  "handle is 0x%04X.",
               session_obj->attached_mvm_handle, elite_apr_if_get_src_port(p_apr_packet));

         result = ADSP_EFAILED;
         break;
      }

      /* Set the MVM handle to NULL */
      if (ADSP_EOK == result)
      {
         session_obj->attached_mvm_handle = APR_NULL_V;
      }

      MSG(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_mvm_detach_cmd_ctrl(): MVM detach success");

      break;
   } /* End of for ( ;; ) */

   /* End the APR command */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_packet, result);

   return result;
}

static ADSPResult vpm_set_voice_timing_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_packet = NULL;
   uint32_t             payload_size;
   vss_ivocproc_cmd_set_voice_timing_t *in_args;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      p_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;
      payload_size = elite_apr_if_get_payload_size(p_apr_packet);

      /* Validate inbound payload size */
      if (payload_size != sizeof(vss_ivocproc_cmd_set_voice_timing_t))
      {
        MSG_3( MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "VPM: SET_VOICE_TIMING_CMD: Invalid payload size, opcode: 0x%08X, "
               "valid_size: %lu, actual_size: %lu", elite_apr_if_get_opcode(p_apr_packet),
               sizeof(vss_ivocproc_cmd_set_voice_timing_t), payload_size );
         result = ADSP_EBADPARAM;
         break;
      }

      if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
      {
        MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "VPM: SET_VOICE_TIMING_CMD: cannot be applied in RUN state");
         result = ADSP_EFAILED;
         break;
      }

      in_args = (vss_ivocproc_cmd_set_voice_timing_t *)elite_apr_if_get_payload_ptr(p_apr_packet);
      session_obj->target_set.voice_timing = *in_args;

      break;
   }

   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_packet, result);

   return result;
}

static ADSPResult vpm_get_avsync_delays_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_packet;
   void                 *rsp_payload;
   uint32_t             session_id;

   p_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   session_id = session_obj->self_apr_port;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         /* Response with ADSP_ENOTREADY if VPM is not running. */
         if (session_obj->state_ctrl.state != VPM_STATE_ENUM_RUN)
         {
            MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
                  "vpm_get_avsync_delays_cmd_ctrl(): VPM not in RUN state, vpm_state: %lu, session_id: 0x%lx",
                  session_obj->state_ctrl.state, session_id);

            /* End the APR command */
            result = ADSP_ENOTREADY;
            break;
         }

         /* Allocate the get delay response payload */
         if (NULL == (rsp_payload = qurt_elite_memory_malloc(sizeof(vpm_get_delay_ack_t),
                                                             QURT_ELITE_HEAP_DEFAULT)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_get_avsync_delays_cmd_ctrl(): FAILED to allocate get delay cmd rsp payload");

            /* End the APR command */
            result = ADSP_ENOMEMORY;
            break;
         }

         /* Set the command status to pending */
         vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

         /* Set the command response pending status to TRUE  */
         vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

         /* Set the function pointer for response handler */
         session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_get_delay_cmd_rsp_handler;

         /* Set the ack payload */
         session_obj->session_rsp_ctrl.p_ack_payload = rsp_payload;

         if (session_obj->dyn_svc_info.tx)
         {
            if (ADSP_EOK != (result = vpm_send_get_delay_cmd_to_dyn_svc(session_obj->session_rsp_ctrl.p_rsp_queue,
                                                                        session_obj->dyn_svc_info.tx->cmdQ,
                                                                        rsp_payload,
                                                                        VSS_IVOCPROC_DIRECTION_TX)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_get_avsync_delays_cmd_ctrl(): Failed to send get delay cmd to vpTx, result: 0x%lx",
                     result);
            }
            else
            {
               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "vpm_send_get_delay_cmd_to_dyn_svc(): Successfully sent to vpTx, session_id: 0x%lx", session_id);

               session_obj->session_rsp_ctrl.num_cmd_issued++;
            }
         }

         if (session_obj->dyn_svc_info.rx)
         {
            if (ADSP_EOK != (result = vpm_send_get_delay_cmd_to_dyn_svc(session_obj->session_rsp_ctrl.p_rsp_queue,
                                                                        session_obj->dyn_svc_info.rx->cmdQ,
                                                                        rsp_payload,
                                                                        VSS_IVOCPROC_DIRECTION_RX)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_get_avsync_delays_cmd_ctrl(): Failed to send get delay cmd to vpRx, result: 0x%lx",
                     result);
            }
            else
            {
               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "vpm_send_get_delay_cmd_to_dyn_svc(): Successfully sent to vpRx, session_id: 0x%lx",
                     session_id);

               session_obj->session_rsp_ctrl.num_cmd_issued++;
            }
         }

         /* If none of the command send succeeded, then  clear pending state  */
         if (!session_obj->session_rsp_ctrl.num_cmd_issued)
         {
            /* Clear the cmd response pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

            /* Clear the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            /* Free the response memory */
            qurt_elite_memory_free(rsp_payload);

            /* End the APR command */
            result = ADSP_EFAILED;
            break;
         }
         else /* At least one cmd succeeded */
         {
            /* Return and wait for get param responses from dyn
               services */
            return ADSP_EOK;
         }
      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after getting respones for all the cmds
         sent to dyn svcs */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* The get_delay() response is fwd'd to MVM from response handler.
         Only free the current cmd APR packet here. 
         Note that cmd packet is used in response handler
         for getting client port/addr info, so its freed in the end. */
      elite_apr_if_free(session_obj->self_apr_handle, p_apr_packet);

      return result;
   } /* End of for (;;) */

   /* End if the APR command, if any error occurs  */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_packet, result);

   return result;
}

static ADSPResult vpm_get_hdvoice_config_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;
   uint32_t             payload_size;

   vss_ihdvoice_cmd_get_config_t    *in_args;
   vss_ihdvoice_rsp_get_config_t    hdvoice_config_entry;

   hdvoice_config_entry.enable_mode = VSS_PARAM_FEATURE_FORCED_OFF;
   hdvoice_config_entry.rx_pp_sr = VSS_PARAM_SAMPLING_RATE_ANY;
   hdvoice_config_entry.tx_pp_sr = VSS_PARAM_SAMPLING_RATE_ANY;
   hdvoice_config_entry.feature_id = VSS_ICOMMON_CAL_FEATURE_NONE;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   /* Check if session creation is complete or not */
   if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
   {
      /* End the APR command*/
      elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, ADSP_EFAILED);

      return ADSP_EFAILED;
   }

   /* Get the APR payload pointer and size */
   in_args = (vss_ihdvoice_cmd_get_config_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
   payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

   /* Determining if HD Voice should be enabled by searching Device Config table based off MVM system config */
   MSG_7( MSG_SSID_DFLT, DBG_MED_PRIO, 
          "vpm_get_hdvoice_config_cmd_ctrl(): Determining if HD Voice should be enabled" \
          "Network: 0x%08X, TX PP SR: %d, RX PP SR: %d, TX voc op mode: 0x%08X, " \
          "RX voc op mode: 0x%08X, Media ID: 0x%08X, Feature: 0x%08X", 
          in_args->network_id, in_args->tx_pp_sr, in_args->rx_pp_sr,
          in_args->tx_voc_op_mode, in_args->rx_voc_op_mode, in_args->media_id, in_args->feature_id );


   (void)vpm_helper_search_hdvoice_config(session_obj, in_args, &hdvoice_config_entry);

   MSG_4(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_get_hdvoice_config_cmd_ctrl(): Found the feature(0x%08x)" \
         "Enable Mode: %d, RX PP SR: %d, TX PP SR: %d",
         hdvoice_config_entry.feature_id, hdvoice_config_entry.enable_mode,
         hdvoice_config_entry.rx_pp_sr, hdvoice_config_entry.tx_pp_sr );

   /* Sent the ACK back to client */
   if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                    elite_apr_if_get_dst_addr(p_apr_pkt),
                                                    elite_apr_if_get_dst_port(p_apr_pkt),
                                                    elite_apr_if_get_src_addr(p_apr_pkt),
                                                    elite_apr_if_get_src_port(p_apr_pkt),
                                                    elite_apr_if_get_client_token(p_apr_pkt),
                                                    VSS_IHDVOICE_RSP_GET_CONFIG,
                                                    &hdvoice_config_entry,
                                                    sizeof(hdvoice_config_entry))))
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_get_hdvoice_config_cmd_ctrl(): Failed to send ACK for get config, "
            "result: 0x%lx", rc);

      /* End the APR command */
      elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, rc);

      return rc;
   }

   /* Free the existing APR packet */
   elite_apr_if_free(session_obj->self_apr_handle, p_apr_pkt);

   return result;
}

static ADSPResult vpm_set_hdvoice_config_cmd_control(vpm_session_object_t *session_obj)
{
   ADSPResult                       result = ADSP_EOK;
   elite_apr_packet_t               *p_apr_pkt;
   vss_ihdvoice_cmd_set_config_t    *in_args;
   uint32_t                         payload_size;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_set_hdvoice_config_cmd_control(): Session obj pointer is NULL");

      return ADSP_EBADPARAM;
   }

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
   {
      /* Get the APR payload size */
      payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

      /* Validate the inbound payload size. */
      if (payload_size != sizeof(vss_ihdvoice_cmd_set_config_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_set_hdvoice_config_cmd_control(): Invalid payload size, %lu != expected %lu",
               payload_size, sizeof(vss_ihdvoice_cmd_set_config_t));

         result = ADSP_EBADPARAM;

         /* End the APR command */
         elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

         return result;
      }

      /* Get the APR payload pointer */
      in_args = (vss_ihdvoice_cmd_set_config_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

      session_obj->target_set.system_config.feature = in_args->feature_id;

      /* Check if there is any change in feature config */
      if (session_obj->target_set.system_config.feature ==\
             session_obj->active_set.system_config.feature)
      {
         MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_set_hdvoice_config_cmd_control(): No change in target "
               "feature config: 0x%lx", session_obj->active_set.system_config.feature);

         session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;
      }
      else
      {
         if (VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE == session_obj->active_set.system_config.feature)
         {
           session_obj->active_set.system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;
         }

         /* If no feature enabled previously, directly start with enabling the feature */
         if (VSS_ICOMMON_CAL_FEATURE_NONE == session_obj->active_set.system_config.feature)
         {
           if(session_obj->target_set.system_config.feature != VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE)
           {
            session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_HDVOICE_ENABLE;
         }
         else
         {
             /* Update the feature ID in active set */
             session_obj->active_set.system_config.feature =\
                session_obj->target_set.system_config.feature;
             /* Check if VPM is in RUN state and MVM is attached */
             if ((VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state) &&
                 (APR_NULL_V != session_obj->attached_mvm_handle))
             {
                session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_GET_KPPS;
             }
             else
             {
                session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;
             }
           }
         }
         else
         {
            /* WV v1 or v2 is already enabled, and needs to be disabled first */
            session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_HDVOICE_DISABLE;
         }
      }

      /* Set the command status to pending */
      vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

   } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

   /* Run the command sequencer for applying system config */
   for (;;)
   {
      switch (session_obj->session_cmd_ctrl.cmd_seq_action)
      {
         case VPM_ACTION_ENUM_HDVOICE_DISABLE:
         {
            /* Send WV disable msg to vpRx */
            if (ADSP_EOK != (result = vpm_send_hdvoice_enable_disable_msg(session_obj, FALSE)))
            {
               /* End the APR command */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_ERR_HNDLR;
               continue;
            }
            else
            {
               /* Set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_HDVOICE_DISABLE_DONE;

               /* Wait for set param responses */
               return ADSP_EOK;
            }
         }
         case VPM_ACTION_ENUM_HDVOICE_DISABLE_DONE:
         {
            /* Check the set param response status */
            if (ADSP_EOK != (result = session_obj->session_rsp_ctrl.status))
            {
               /* Dyn svc ack'd back with error, end APR command */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_ERR_HNDLR;
               continue;
            }

            /* Set the active feature ID to NONE after successful disable */
            session_obj->active_set.system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;

            /* Check if target feature ID is NONE, or,
               if it is non-none, then is it supported */
            if ((VSS_ICOMMON_CAL_FEATURE_NONE == session_obj->target_set.system_config.feature) ||
                (FALSE == vpm_is_hdvoice_feature_supported(session_obj->target_set.system_config.feature)))
            {
               /* No new feature needs to be enabled, query the KPPS from dyn svcs */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_GET_KPPS;
            }
            else if(VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE == session_obj->target_set.system_config.feature)
            {
              /* Update the feature ID in active set */
              session_obj->active_set.system_config.feature =\
                 session_obj->target_set.system_config.feature;
              /* Check if VPM is in RUN state and MVM is attached */
              if ((VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state) &&
                  (APR_NULL_V != session_obj->attached_mvm_handle))
              {
                 session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_GET_KPPS;
              }
              else
              {
                 session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;
              }
            }
            else
            {
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_HDVOICE_ENABLE;
            }

            /* Jump to next action */
            continue;
         }
         case VPM_ACTION_ENUM_HDVOICE_ENABLE:
         {
            /* Send WV Enable msg to vpRx  */
            if (ADSP_EOK != (result = vpm_send_hdvoice_enable_disable_msg(session_obj, TRUE)))
            {
               /* End the APR command */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_ERR_HNDLR;
               continue;
            }
            else
            {
               /* Set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_HDVOICE_ENABLE_DONE;

               /* Return and wait for set param responses */
               return ADSP_EOK;
            }
         }
         case VPM_ACTION_ENUM_HDVOICE_ENABLE_DONE:
         {
            /* Check the set param response status */
            if (ADSP_EOK != (result = session_obj->session_rsp_ctrl.status))
            {
               /* End the APR command */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_ERR_HNDLR;
               continue;
            }

            /* Update the feature ID in active set */
            session_obj->active_set.system_config.feature =\
               session_obj->target_set.system_config.feature;

            /* Check if VPM is in RUN state and MVM is attached */
            if ((VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state) &&
                (APR_NULL_V != session_obj->attached_mvm_handle))
            {
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_GET_KPPS;
            }
            else
            {
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;
            }

            /* Jump to next action */
            continue;
         }
         case VPM_ACTION_ENUM_GET_KPPS:
         {
            /* Retrieve the VPM's KPPS requirement. */
            if (ADSP_EOK != (result = vpm_send_get_kpps_cmd_to_dyn_svc(session_obj)))
            {
               /* End the APR command */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_ERR_HNDLR;
               continue;
            }
            else
            {
               /* Set the next action */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_GET_KPPS_DONE;

               MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                   "vpm_set_hdvoice_config_cmd_control(): sent get_kpps to VPM");

               /* Return and Wait for command response */
               return ADSP_EOK;
            }
         }
         case VPM_ACTION_ENUM_GET_KPPS_DONE:
         {
            /* Check the set param response status */
            if (ADSP_EOK != (result = session_obj->session_rsp_ctrl.status))
            {
               /* End the APR command */
               session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_ERR_HNDLR;
               continue;
            }

            /* Below flag is set to TRUE only if get kpps cmd response is success */
            if (TRUE == session_obj->is_kpps_changed)
            {
               /* Send reconfig event to MVM */
               elite_apr_if_alloc_send_event(session_obj->self_apr_handle,
                                             session_obj->self_apr_addr,
                                             session_obj->self_apr_port,
                                             session_obj->mvm_apr_addr,
                                             session_obj->attached_mvm_handle,
                                             0,
                                             VSS_IVOCPROC_EVT_RECONFIG,
                                             NULL,
                                             0);

               MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                   "vpm_set_hdvoice_config_cmd_control(): sent reconfig to MVM");

               /* Reset the KPPS change flag */
               session_obj->is_kpps_changed = FALSE;
            }

            /* Set the next action */
            session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_COMPLETE;

            result = session_obj->session_rsp_ctrl.status;

            /*-fallthru */
         }
         case VPM_ACTION_ENUM_COMPLETE:
         {
            /* End the APR command */
            elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

            /* Clear the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                  "vpm_set_hdvoice_config_cmd_control(): COMPLETED, result: 0x%lx", result);

            return result;
         }
         case VPM_ACTION_ENUM_ERR_HNDLR:
         {
            /* Clear the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            /* End the APR command */
            elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

            return result;
         }
         default:
         {
            MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                  "vpm_set_hdvoice_config_cmd_control(): Unexpected cmd seq action: 0x%lx",
                  session_obj->session_cmd_ctrl.cmd_seq_action);

            result = ADSP_EFAILED;

            /* End the APR command */
            session_obj->session_cmd_ctrl.cmd_seq_action = VPM_ACTION_ENUM_ERR_HNDLR;
            continue;
         }

      } /*switch (session_obj->session_cmd_ctrl.cmd_seq_action) */

   } /* End of for(;;) */

   return result;
}

static ADSPResult vpm_soundfocus_set_sectors_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;
   uint32_t             payload_size;

   vss_isoundfocus_cmd_set_sectors_t *in_args;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         in_args = (vss_isoundfocus_cmd_set_sectors_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate the inbound payload size. */
         if (payload_size != sizeof(vss_isoundfocus_cmd_set_sectors_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_soundfocus_set_sectors_cmd_ctrl(): Unexpected size, %d != %d",
                  payload_size, sizeof(vss_isoundfocus_cmd_set_sectors_t));

            result = ADSP_EBADPARAM;
            break;
         }

         /* Validate payload. */
         if (ADSP_EOK != (result = vpm_soundfocus_validate_angles(in_args)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_soundfocus_set_sectors_cmd_ctrl(): Bad param");

            result = ADSP_EBADPARAM;
            break;
         }

         session_obj->target_set.soundfocus_sectors = *in_args;
         session_obj->is_client_set_sectors = TRUE;

         /* If in RUN state, set the sectors on VPM. Note that changing the
          * sectors at RUN time will not cause any KPPS requirement change
          * (confimred with VCP team).
          *
          * If in IDLE state, leave settings in the target_set. They will be
          * picked up when we go to RUN (either upon receiving a
          * VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG command if the vocproc is attached
          * to MVM, or upon IDLE to RUN transition if the vocproc is not attached
          * to MVM).
          */
         if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
         {
            if (ADSP_EOK == (result = vpm_action_set_soundfocus_sectors(session_obj)))
            {
               /* Set the command pending status */
               vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

               /* Wait for set param responses from dyn svcs */
               return ADSP_EOK;
            }
            else /* Action failed */
            {
               /* End APR command */
               break;
            }
         }
         else
         {
            /* VPM not in RUN state, just end the APR command */
            result = ADSP_EOK;
            break;
         }
      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after getting all the set param responses */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Get the aggregated response status */
      result = session_obj->session_rsp_ctrl.status;

      /* End the APR command */
      break;

   } /* End of for(;;) */

   /* End the APR command */
   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_soundfocus_get_sectors_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult              result = ADSP_EOK;
   elite_apr_packet_t      *p_apr_pkt;
   vpm_soundfocus_param_t  *p_sound_focus_param;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         if (NULL == (p_sound_focus_param = (vpm_soundfocus_param_t *)qurt_elite_memory_malloc(sizeof(vpm_soundfocus_param_t),
                                                                                               QURT_ELITE_HEAP_DEFAULT)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_soundfocus_get_sectors_cmd_ctrl(): Failed to allocate payload for inband get param");

            result = ADSP_ENOMEMORY;
            break;
         }

         MSG(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_soundfocus_get_sectors_cmd_ctrl(): Sending GET_PARAM_V2 to dyn services");

         /* Poupulate the set param payload */
         p_sound_focus_param->module_id = VOICEPROC_MODULE_TX;
         p_sound_focus_param->param_id = VOICE_PARAM_FLUENCE_SOUNDFOCUS;
         p_sound_focus_param->param_size = sizeof(vpm_soundfocus_param_data_t);
         p_sound_focus_param->reserved = 0;

         /* Set the command pending status */
         vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

         /* Set the command response pending status */
         vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

         /* Set the response payload pointer */
         session_obj->session_rsp_ctrl.p_ack_payload = (void *)p_sound_focus_param;

         /* Set the response handler function pointer */
         session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_get_soundfocus_sectors_cmd_rsp_fn;

         /* Send set param command to dynamic services */
         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                   p_sound_focus_param,
                                                                   sizeof(vpm_soundfocus_param_t),
                                                                   ELITE_CMD_SET_PARAM,
                                                                   CVD_CAL_PARAM_MINOR_VERSION_0)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_action_set_soundfocus_sectors(): "
                  "Failed to send set param cmd to dyn svcs, result: 0x%lx", result);

            /* Clear the command response pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

            /* Clear the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            break;
         }

         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_soundfocus_get_sectors_cmd_ctrl(): Sending GET_PARAM_V2 to dyn svcs"
               "MID: 0x%08x, PID: 0x%08x", p_sound_focus_param->module_id,
               p_sound_focus_param->param_id);

         /* Get param commands sent successfully. Now wait for respones */

         return ADSP_EOK;
      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)) */

      /* Control will reach here after getting responses for all get param cmds */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Get the aggregated response status */
      result = session_obj->session_rsp_ctrl.status;

      /* Free the existing cmd APR command */
      elite_apr_if_free(session_obj->self_apr_addr, p_apr_pkt);

      return result;
   } /* for (;;) */

   /* In case of any errors */

   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_sourcetrack_get_activity_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;
   uint16_t             client_addr;
   uint32_t             vpm_mem_handle;
   cvd_virt_addr_t      param_data_virt_addr;
   voice_param_data_t   *p_param_data;
   uint32_t             payload_size;

   vss_isourcetrack_cmd_get_activity_t *p_get_activity;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         p_get_activity = (vss_isourcetrack_cmd_get_activity_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
         payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

         /* Validate inbound payload size */
         if (payload_size != sizeof(vss_isourcetrack_cmd_get_activity_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_sourcetrack_get_activity_cmd_ctrl(): Invalid payload size %d != expected (%d)",
                  payload_size, sizeof(vss_isourcetrack_cmd_get_activity_t));

            result = ADSP_EBADPARAM;
            break;
         }

         /* Parameter validation. */
         if (p_get_activity->mem_size < sizeof(vss_isourcetrack_activity_data_t))
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_sourcetrack_get_activity_cmd_ctrl(): mem size is too small: %d < expected: %lu",
                  p_get_activity->mem_size, sizeof(vss_isourcetrack_activity_data_t));

            result = ADSP_EBADPARAM;
            break;
         }

         if (ADSP_EOK != (result = vpm_validate_calib_payload(p_get_activity->mem_handle,
                                                              p_get_activity->mem_size,
                                                              p_get_activity->mem_address)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_sourcetrack_get_activity_cmd_ctrl(): Failed to validate calib payload,"
                  " result: 0x%lx", result);

            /* End the APR command */
            break;
         }

         rc = cvd_mem_mapper_validate_attributes_align(p_get_activity->mem_handle,
                                                       p_get_activity->mem_size);
         if (rc)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_sourcetrack_get_activity_cmd_ctrl(): Mis-aligned mem size: 0x%08X",
                  p_get_activity->mem_size);

            result = ADSP_EBADPARAM;
            break;
         }

         (void)cvd_mem_mapper_get_vpm_mem_handle(p_get_activity->mem_handle, &vpm_mem_handle);

         rc = cvd_mem_mapper_get_virtual_addr_v2(p_get_activity->mem_handle,
                                                 p_get_activity->mem_address,
                                                 &param_data_virt_addr);
         if (rc)
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_sourcetrack_get_activity_cmd_ctrl(): Cannot get virtual address, "
                  "mem_handle: 0x%08X, addr: 0x%016X",
                  p_get_activity->mem_handle, p_get_activity->mem_address);

            result = ADSP_EBADPARAM;
            break;
         }

         session_obj->sourcetrack_mem_addr = param_data_virt_addr;

         p_param_data = (voice_param_data_t *)&session_obj->shared_heap.sourcetrack_activity;

         /* Populate the param data payload */
         p_param_data->module_id = VOICEPROC_MODULE_TX;
         p_param_data->param_id = VOICE_PARAM_FLUENCE_SOURCETRACKING;
         p_param_data->param_size = p_get_activity->mem_size;
         p_param_data->reserved = 0;

         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_sourcetrack_get_activity_cmd_ctrl(): "
               "addr: 0x%08X, size: %d",
               p_param_data, p_param_data->param_size);

         /* set command rsp to pending status */
         vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

         /* Set the command pending status */
         vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);

         /* Set the response handler function pointer */
         session_obj->session_rsp_ctrl.p_rsp_hndlr_fn = vpm_sourcetrack_get_activity_cmd_rsp_fn;

         /* Send get param command to dynamic services */
         if (ADSP_EOK != (result = vpm_set_get_param_to_dyn_svc(session_obj,
                                                                   p_param_data,
                                                                   p_get_activity->mem_size,
                                                                   ELITE_CMD_GET_PARAM,
                                                                   CVD_CAL_PARAM_MINOR_VERSION_0)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_sourcetrack_get_activity_cmd_ctrl(): Failed to send get param msg to dyn svcs,"
                  "result: 0x%lx", result);

            /* Clear command rsp to pending status */
            vpm_set_cmd_rsp_pending_status(session_obj, FALSE);

            /* Clear the command pending status */
            vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

            /* End the APR command */
            break;
         }

         MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_sourcetrack_get_activity_cmd_ctrl(): Sent GET_PARAM_V2 to dyn svcs"
               "MID: 0x%08X, PID: 0x%08X",
               p_param_data->module_id, p_param_data->param_id);

         /* Return and wait for get param responses */
         return ADSP_EOK;

      } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

      /* Control will reach here after getting responses for all the get params */

      /* Clear the command pending status */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* Get the aggregated response status */
      result = session_obj->session_rsp_ctrl.status;

      /* End the APR command */
      elite_apr_if_free(session_obj->self_apr_addr, p_apr_pkt);

      return result;

   } /* End of for (;;) */

   /* In case of any error occurs */
   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

/* Set device channel count information.
 */
static ADSPResult vpm_set_dev_channels_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   uint32_t             payload_size;

   vss_ivocproc_cmd_topology_set_dev_channels_t *in_args;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      payload_size = elite_apr_if_get_payload_size(p_apr_pkt);

      if (payload_size != sizeof(vss_ivocproc_cmd_topology_set_dev_channels_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_set_dev_channels_cmd_ctrl(): Incorrect cmd payload size: %lu != expected(%lu)",
               payload_size, sizeof(vss_ivocproc_cmd_topology_set_dev_channels_t));

         result = ADSP_EBADPARAM;
         break;
      }

      if (VPM_TOPOLOGY_COMMIT_STATE_NONE == session_obj->topo_commit_state)
      {
         /* Setting topology params is not allowed after commit. */
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_set_dev_channels_cmd_ctrl(): Cannot set topo params after topo commit");

         result = ADSP_EFAILED;
         break;
      }

      in_args = (vss_ivocproc_cmd_topology_set_dev_channels_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

      /* Input validation  */
      if ((in_args->tx_num_channels < VSS_NUM_DEV_CHANNELS_1) ||
          (in_args->tx_num_channels > VSS_NUM_DEV_CHANNELS_4) ||
          (in_args->rx_num_channels != VSS_NUM_DEV_CHANNELS_1))
      {
         /* In valid topology params. */
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "cvp_extract_topology_params(): In valid params: #Mics: %lu, #Speaker Channels: %lu",
               in_args->tx_num_channels, in_args->rx_num_channels);

         result = ADSP_EBADPARAM;
         break;
      }

      /* Save the topology parameters */
      session_obj->target_set.num_dev_channels.tx_num_channels = in_args->tx_num_channels;
      session_obj->target_set.num_dev_channels.rx_num_channels = in_args->rx_num_channels;

      MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_set_dev_channels_cmd_ctrl(): tx_num_channels = %d, rx_num_channels = %d",
            in_args->tx_num_channels, in_args->rx_num_channels);

      break;
   } /* End of for (;;) */

   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

static ADSPResult vpm_vpcm_evt_push_buffer(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;
   int32_t              rc;
   uint32_t             payload_len;
   uint64_t             out_buf_virt_addr = 0;
   uint64_t             in_buf_virt_addr = 0;
   uint32_t             cvd_mem_handle;
   uint32_t             handle;
   elite_msg_any_t      msg_pkt;
   elite_apr_packet_t   *p_dyn_svc_apr_pkt = NULL;

   voice_evt_push_host_pcm_buf_v2_t    *p_push_buf_event;
   vss_ivpcm_evt_push_buffer_v2_t      *payload;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {
      /* Read access to vpcm_info/vocproc_handle. */
      if (FALSE == session_obj->vpcm_info.is_enabled)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_vpcm_evt_push_buffer(): VPCM is not enabled");

         result = ADSP_ENOTREADY;
         break;
      }

      /* Get the APR payload pointer and size */
      payload = (vss_ivpcm_evt_push_buffer_v2_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);
      payload_len = elite_apr_if_get_payload_size(p_apr_pkt);

      /* Validate inbound payload size */
      if (payload_len != sizeof(vss_ivpcm_evt_push_buffer_v2_t))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_vpcm_evt_push_buffer(): Invalid data. "
               "Payload len: %lu, expected len: %lu", payload_len,
               sizeof(vss_ivpcm_evt_push_buffer_v2_t));
         break;
      }

      cvd_mem_handle = session_obj->vpcm_info.mem_handle;

      if (payload->push_buf_mask & VSS_IVPCM_PUSH_BUFFER_MASK_OUTPUT_BUFFER)
      {
         /* Validate the calib payload */
         if (ADSP_EOK != (result = vpm_validate_calib_payload(cvd_mem_handle,
                                                              payload->out_buf_mem_size,
                                                              payload->out_buf_mem_address)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_evt_push_buffer(): Shared mem region validation failed"
                  "result: 0x%lx", result);

            /* End the APR command */
            break;
         }

         rc = cvd_mem_mapper_validate_attributes_align(cvd_mem_handle, payload->out_buf_mem_size);
         if (rc)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_evt_push_buffer(): Mis-aligned "
                  "output buf mem size: %d", payload->out_buf_mem_size);

            result = ADSP_EBADPARAM;
            break;
         }

         /* Get the Virtual address */
         rc = cvd_mem_mapper_get_virtual_addr(cvd_mem_handle,
                                              payload->out_buf_mem_address,
                                              &out_buf_virt_addr);
         if (rc)
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_evt_push_buffer(): Cannot get "
                  "virtual address, mem_handle 0x%08X, out_buf_mem_address 0x%016X",
                  cvd_mem_handle, payload->out_buf_mem_address);

            result = ADSP_EFAILED;
            break;
         }
      }

      if (payload->push_buf_mask & VSS_IVPCM_PUSH_BUFFER_MASK_INPUT_BUFFER)
      {
         /* Validate the calib payload */
         if (ADSP_EOK != (result = vpm_validate_calib_payload(cvd_mem_handle,
                                                              payload->in_buf_mem_size,
                                                              payload->in_buf_mem_address)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_evt_push_buffer(): Shared mem region validation failed"
                  "result: 0x%lx", result);

            /* End the APR command */
            break;
         }

         /* Get the virtual address */
         rc = cvd_mem_mapper_get_virtual_addr(cvd_mem_handle,
                                              payload->in_buf_mem_address,
                                              &in_buf_virt_addr);
         if (rc)
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_evt_push_buffer(): Cannot get "
                  "virtual address, mem_handle: 0x%08X, in_buf_mem_address: "
                  "0x%016X", cvd_mem_handle, payload->in_buf_mem_address);

            result = ADSP_EFAILED;
            break;
         }

         /* Invalidate the cache */
         cvd_mem_mapper_cache_invalidate(in_buf_virt_addr, payload->in_buf_mem_size);
      }

      /* Allocate APR packet to be fwd'd to dyn svcs */
      if (APR_EOK != (rc = elite_apr_if_alloc_cmd(session_obj->self_apr_handle,
                                                  session_obj->self_apr_addr,
                                                  session_obj->self_apr_port,
                                                  elite_apr_if_get_dst_addr(p_apr_pkt),    /* Don't care */
                                                  elite_apr_if_get_dst_port(p_apr_pkt),    /* Don't care */
                                                  elite_apr_if_get_client_token(p_apr_pkt),
                                                  VOICE_EVT_PUSH_HOST_BUF_V2,
                                                  sizeof(voice_evt_push_host_pcm_buf_v2_t),
                                                  &p_dyn_svc_apr_pkt)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_vpcm_evt_push_buffer(): Failed to allocate APR packet for dyn svc cmd fwd,"
               " result; 0x%lx", rc);

         result = rc;
         break;
      }
      else if (NULL == p_dyn_svc_apr_pkt)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_vpcm_evt_push_buffer(): Allocated APR packet pointer is NULL");

         result = ADSP_EFAILED;
         break;
      }

      /* Get payload pointer to newly allocated APR packet */
      p_push_buf_event = (voice_evt_push_host_pcm_buf_v2_t *)elite_apr_if_get_payload_ptr(p_dyn_svc_apr_pkt);

      switch (payload->tap_point)
      {
         case VSS_IVPCM_TAP_POINT_TX_DEFAULT:
         {
            p_push_buf_event->tap_point = VOICEPROC_MODULE_TX;
            break;
         }
         case VSS_IVPCM_TAP_POINT_RX_DEFAULT:
         {
            p_push_buf_event->tap_point = VOICEPROC_MODULE_RX;
            break;
         }
         default:
         {
            /* Drop and free the push buffer event. */
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_evt_push_buffer(): Invalid tap "
                  "point: 0x%08X", payload->tap_point);

            result = APR_EBADPARAM;
            break;
         }
      } /* End of switch (payload->tap_point) */

      if (ADSP_EOK != result)
      {
         break;
      }

      /* Populate the newly allocated APR packet */
      p_push_buf_event->mask = payload->push_buf_mask;
      p_push_buf_event->rd_buff_addr_lsw = ((uint32_t)out_buf_virt_addr);
      p_push_buf_event->rd_buff_addr_msw = ((uint32_t)(out_buf_virt_addr >> 32));
      p_push_buf_event->wr_buff_addr_lsw = ((uint32_t)in_buf_virt_addr);
      p_push_buf_event->wr_buff_addr_msw = ((uint32_t)(in_buf_virt_addr >> 32));
      p_push_buf_event->rd_buff_size = payload->out_buf_mem_size;
      p_push_buf_event->wr_buff_size = payload->in_buf_mem_size;
      p_push_buf_event->sampling_rate = payload->sampling_rate;
      p_push_buf_event->wr_num_chan = payload->num_in_channels;

      /* Populate elite_msg_any fields */
      msg_pkt.unOpCode = ELITE_APR_PACKET;
      msg_pkt.pPayload = (void *)p_dyn_svc_apr_pkt;

      /* Push the APR packet to vpRx/Tx dyn svc */
      if (VSS_IVPCM_TAP_POINT_TX_DEFAULT == payload->tap_point)
      {
         if (session_obj->dyn_svc_info.tx)
         {
            if (ADSP_EOK != (result = qurt_elite_queue_push_back(session_obj->dyn_svc_info.tx->cmdQ,
                                                                 (uint64_t *)&msg_pkt)))
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_vpcm_evt_push_buffer(): Failed to push APR packet to vpTx, result: 0x%lx",
                     result);

               break;
            }
         }
      }
      else if (VSS_IVPCM_TAP_POINT_RX_DEFAULT == payload->tap_point)
      {
         if (ADSP_EOK != (result = qurt_elite_queue_push_back(session_obj->dyn_svc_info.rx->cmdQ,
                                                              (uint64_t *)&msg_pkt)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_evt_push_buffer(): Failed to push APR packet to vpRx, result: 0x%lx",
                  result);

            break;
         }
      }

      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_vpcm_evt_push_buffer(): Pushed the APR packet to dyn svc, tap_point: 0x%lx",
            payload->tap_point);

      break;

   } /* End of for (;;)*/

   /* Free up the newly allocated APR packet, if any error occurs */
   if (p_dyn_svc_apr_pkt &&
       (ADSP_EOK != result))
   {
      elite_apr_if_free(session_obj->self_apr_addr, p_dyn_svc_apr_pkt);
   }

   /* Free the existing APR packet.
      No ACK required for VSS_IVPCM_EVT_PUSH_BUFFER_V2  */
   elite_apr_if_free(session_obj->self_apr_addr, p_apr_pkt);

   return result;
}

static ADSPResult vpm_vpcm_notify_buf_available_evt_to_client(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   int32_t              rc;
   elite_apr_packet_t   *p_apr_pkt;
   uint64_t             out_buf_virt_addr;
   uint64_t             out_buf_mem_addr;
   uint64_t             in_buf_virt_addr;
   uint64_t             in_buf_mem_addr;

   vss_ivpcm_evt_notify_v2_t           notify_event;
   voice_evt_notify_host_pcm_buf_v2_t  *payload;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   for (;;)
   {

      payload = (voice_evt_notify_host_pcm_buf_v2_t *)elite_apr_if_get_payload_ptr(p_apr_pkt);

      switch (payload->tap_point)
      {
         case VOICEPROC_MODULE_TX:
         {
            notify_event.tap_point = VSS_IVPCM_TAP_POINT_TX_DEFAULT;
            break;
         }
         case VOICEPROC_MODULE_RX:
         {
            notify_event.tap_point = VSS_IVPCM_TAP_POINT_RX_DEFAULT;
            break;
         }
         default:
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_notify_buf_available_evt_to_client(): Invalid "
                  "tap point: 0x%08X", payload->tap_point);

            result = ADSP_EBADPARAM;
            break;
         }
      }

      notify_event.notify_mask = payload->mask;

      if (notify_event.notify_mask & VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER)
      {
         out_buf_virt_addr = payload->rd_buff_addr_msw;
         out_buf_virt_addr = ((out_buf_virt_addr << 32) | payload->rd_buff_addr_lsw);

         rc = cvd_mem_mapper_get_mem_addr(session_obj->vpcm_info.mem_handle,
                                          out_buf_virt_addr, &out_buf_mem_addr);
         if (rc)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_notify_buf_available_evt_to_client(): Unable to get "
                  "mem_addr for out_buf_virt_addr: 0x%016X", out_buf_virt_addr);

            result = rc;
            break;
         }

         notify_event.out_buf_mem_address = out_buf_mem_addr;

         /* Flush the cache */
         cvd_mem_mapper_cache_flush(out_buf_virt_addr, payload->rd_buff_size);
      }

      if (notify_event.notify_mask & VSS_IVPCM_NOTIFY_MASK_INPUT_BUFFER)
      {
         in_buf_virt_addr = payload->wr_buff_addr_msw;
         in_buf_virt_addr = ((in_buf_virt_addr << 32) | payload->wr_buff_addr_lsw);

         rc = cvd_mem_mapper_get_mem_addr(session_obj->vpcm_info.mem_handle,
                                          in_buf_virt_addr, &in_buf_mem_addr);
         if (rc)
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_vpcm_notify_buf_available_evt_to_client(): Unable to "
                  "get mem_addr for in_buf_virt_addr: 0x%016X", in_buf_virt_addr);

            result = rc;
            break;
         }

         notify_event.in_buf_mem_address = in_buf_mem_addr;
      }

      notify_event.filled_out_size = payload->rd_buff_size;
      notify_event.request_buf_size = payload->wr_buff_size;
      notify_event.sampling_rate = payload->sampling_rate;

      /* For OUTPUT only mode or dual mic OUTPUT-INPUT mode, write buffer size
         and hence request buffer size will be zero and need to be derived from
         read buffer size. */
      if (0 == notify_event.request_buf_size)
      {
         /* Derive request buffer size from read buffer size. */
         notify_event.request_buf_size = payload->rd_buff_size;

         /* If the output buffer mask is not set, set the filled out size to zero. */
         if (0 == (notify_event.notify_mask & VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER))
         {
            notify_event.filled_out_size = 0;
         }
      }

      /* If the mode is NONE or INPUT only, number of output channels is derived
         from num of write channels from FW. */
      if ((notify_event.notify_mask &
              (VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER | VSS_IVPCM_NOTIFY_MASK_INPUT_BUFFER)) == 0
          || (((notify_event.notify_mask & VSS_IVPCM_NOTIFY_MASK_INPUT_BUFFER) != 0)
              && ((notify_event.notify_mask & VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER) == 0)))
      {
         /* Number of expected input channels. */
         notify_event.num_out_channels = payload->wr_num_chan;
      }
      else /* If OUTPUT only or OUTPUT-INPUT mode. */
      {
         /* Number of output channels.  */
         notify_event.num_out_channels = payload->rd_num_chan;
      }

      if (APR_EOK != (rc = elite_apr_if_alloc_send_ack(session_obj->self_apr_handle,
                                                       session_obj->self_apr_addr,
                                                       session_obj->self_apr_port,
                                                       session_obj->vpcm_info.client_addr,
                                                       session_obj->vpcm_info.client_handle,
                                                       elite_apr_if_get_client_token(p_apr_pkt),
                                                       VSS_IVPCM_EVT_NOTIFY_V2,
                                                       &notify_event,
                                                       sizeof(notify_event))))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_vpcm_notify_buf_available_evt_to_client(): Failed to"
               "allocate APR ack packet, result: 0x%lx", rc);

         result = rc;
      }

      break;
   }

   elite_apr_if_free(session_obj->self_apr_handle, p_apr_pkt);

   return result;
}

ADSPResult vpm_session_cmdq_apr_msg_handler(void *p_session_inst, elite_msg_any_t *p_msg_pkt)
{
   ADSPResult              result = ADSP_EOK;
   vpm_session_object_t    *p_session = (vpm_session_object_t *)p_session_inst;
   elite_apr_packet_t      *apr_pkt_ptr;
   uint32_t                opcode;

   apr_pkt_ptr = (elite_apr_packet_t *)(p_msg_pkt->pPayload);

   opcode = elite_apr_if_get_opcode(apr_pkt_ptr);

   switch (opcode)
   {
      case VSS_IVOCPROC_CMD_TOPOLOGY_COMMIT:
      {
         result = vpm_topology_commit_cmd_ctrl(p_session);
         break;
      }
      case APRV2_IBASIC_CMD_DESTROY_SESSION:
      {
         result = vpm_destroy_session_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_ENABLE:
      {
         result = vpm_enable_cmd_ctrl(p_session, FALSE);
         break;
      }
      case VSS_IVOCPROC_CMD_MVM_ENABLE:
      {
         result = vpm_enable_cmd_ctrl(p_session, TRUE);
         break;
      }
      case VSS_IVOCPROC_CMD_DISABLE:
      {
         result = vpm_disable_cmd_ctrl(p_session, FALSE);
         break;
      }
      case VSS_IVOCPROC_CMD_MVM_DISABLE:
      {
         result = vpm_disable_cmd_ctrl(p_session, TRUE);
         break;
      }
      case VSS_IVOCPROC_CMD_SET_DEVICE:
      {
         result = vpm_set_device_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_SET_DEVICE_V2:
      {
         result = vpm_set_device_v2_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_SET_DEVICE_V3:
      {
         result = vpm_set_device_v3_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_ATTACH_STREAM:
      {
         result = vpm_attach_stream_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_DETACH_STREAM:
      {
         result = vpm_detach_stream_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS:
      {
         result = vpm_set_number_of_volume_steps_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX:
      {
         result = vpm_set_rx_vol_index_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOLUME_CMD_SET_STEP:
      {
         result = vpm_set_volume_step_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_SET_MUTE:
      {
         result = vpm_set_mute_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOLUME_CMD_MUTE_V2:
      {
         result = vpm_volume_mute_v2_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_REGISTER_CALIBRATION_DATA_V2:
      {
         /* BACKWARD COMPATIBILITY */
         result = vpm_register_calibration_data_v2_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_DEREGISTER_CALIBRATION_DATA:
      {
         /* BACKWARD COMPATIBILITY */
         result = vpm_deregister_calibration_data_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_REGISTER_VOLUME_CALIBRATION_DATA:
      {
         /* BACKWARD COMPATIBILITY */
         result = vpm_register_volume_calibration_data_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_DEREGISTER_VOLUME_CALIBRATION_DATA:
      {
         /* BACKWARD COMPATIBILITY */
         result = vpm_deregister_volume_calibration_data_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG:
      {
         result = vpm_register_device_config_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_DEREGISTER_DEVICE_CONFIG:
      {
         result = vpm_deregister_device_config_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_REGISTER_STATIC_CALIBRATION_DATA:
      {
         result = vpm_register_static_calibration_data_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_DEREGISTER_STATIC_CALIBRATION_DATA:
      {
         result = vpm_deregister_static_calibration_data_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_REGISTER_DYNAMIC_CALIBRATION_DATA:
      {
         result = vpm_register_dynamic_calibration_data_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_DEREGISTER_DYNAMIC_CALIBRATION_DATA:
      {
         result = vpm_deregister_dynamic_calibration_data_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_SET_SAMPLING_RATE:
      {
         result = vpm_set_sampling_rate_cmd_ctrl(p_session);
         break;
      }
      case VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG:
      {
         result = vpm_set_system_config_cmd_ctrl(p_session);
         break;
      }
      case VSS_ICOMMON_CMD_SET_DYNAMIC_SYSTEM_CONFIG:
      {
         result = vpm_set_dynamic_system_config_cmd_ctrl(p_session);
         break;
      }
      case VSS_ICOMMON_CMD_SET_PARAM_V2:
      {
         result = vpm_set_param_v2_cmd_ctrl(p_session);
         break;
      }
      case VSS_ICOMMON_CMD_GET_PARAM_V2:
      {
         result = vpm_get_param_v2_cmd_ctrl(p_session);
         break;
      }
      case VSS_ICOMMON_CMD_SET_PARAM_V3:
      {
         result = vpm_set_param_v3_cmd_ctrl(p_session);
         break;
      }
      case VSS_ICOMMON_CMD_GET_PARAM_V3:
      {
         result = vpm_get_param_v3_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVP3_CMD_GET_DATA:
      {
         result = vpm_vp3_get_data_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVP3_CMD_SET_DATA:
      {
         result = vpm_vp3_set_data_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVP3_CMD_GET_SIZE:
      {
         result = vpm_vp3_get_size_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_REINIT:
      {
         result = vpm_reinit_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_MVM_ATTACH:
      {
         result = vpm_mvm_attach_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_MVM_DETACH:
      {
         result = vpm_mvm_detach_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_SET_VOICE_TIMING:
      {
         result = vpm_set_voice_timing_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_GET_AVSYNC_DELAYS:
      {
         result = vpm_get_avsync_delays_cmd_ctrl(p_session);
         break;
      }
      case VSS_IHDVOICE_CMD_GET_CONFIG:
      {
         result = vpm_get_hdvoice_config_cmd_ctrl(p_session);
         break;
      }
      case VSS_IHDVOICE_CMD_SET_CONFIG:
      {
         result = vpm_set_hdvoice_config_cmd_control(p_session);
         break;
      }
      case VSS_ISOUNDFOCUS_CMD_SET_SECTORS:
      {
         result = vpm_soundfocus_set_sectors_cmd_ctrl(p_session);
         break;
      }
      case VSS_ISOUNDFOCUS_CMD_GET_SECTORS:
      {
         result = vpm_soundfocus_get_sectors_cmd_ctrl(p_session);
         break;
      }
      case VSS_ISOURCETRACK_CMD_GET_ACTIVITY:
      {
         result = vpm_sourcetrack_get_activity_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_TOPOLOGY_SET_DEV_CHANNELS:
      {
         result = vpm_set_dev_channels_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_TOPOLOGY_REGISTER_PARAMS:
      {
         result = vpm_register_topology_params_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVOCPROC_CMD_TOPOLOGY_DEREGISTER_PARAMS:
      {
         result = vpm_deregister_topology_params_cmd_ctrl(p_session);
         break;
      }
      case APRV2_IBASIC_EVT_ACCEPTED:
      {
         result = elite_apr_if_free(p_session->self_apr_handle, apr_pkt_ptr);
         break;
      }
      case VSS_ICOMMON_CMD_SET_UI_PROPERTY:
      {
         result = vpm_set_ui_property_cmd_processing(p_session);
         break;
      }
      case VSS_ICOMMON_CMD_GET_UI_PROPERTY:
      {
         result = vpm_get_ui_property_cmd_processing(p_session);
         break;
      }
      case VSS_ICOMMON_CMD_SET_UI_PROPERTY_V2:
      {
         result = vpm_set_ui_property_v2_cmd_processing(p_session);
         break;
      }
      case VSS_ICOMMON_CMD_GET_UI_PROPERTY_V2:
      {
         result = vpm_get_ui_property_v2_cmd_processing(p_session);
         break;
      }
      case VSS_IVPCM_CMD_START_V2:
      {
         result = vpm_vpcm_start_v2_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVPCM_CMD_STOP:
      {
         result = vpm_vpcm_stop_cmd_ctrl(p_session);
         break;
      }
      case VSS_IVPCM_EVT_PUSH_BUFFER_V2:
      {
         result = vpm_vpcm_evt_push_buffer(p_session);
         break;
      }
      case VOICE_EVT_HOST_BUF_AVAILABLE_V2:
      {
         result = vpm_vpcm_notify_buf_available_evt_to_client(p_session);
         break;
      }
      case VPM_EVT_VOICE_ACTIVITY_UPDATE:
      {
        MSG_2( MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_session_cmdq_apr_msg_handler(): Received Voice activity update event"
               "session_id 0x%08X eventID 0x%08X", p_session->session_id,
               APRV2_PKT_GET_PAYLOAD( vss_icommon_evt_voice_activity_update_t, apr_pkt_ptr )->activity );

        apr_pkt_ptr->opcode = VSS_ICOMMON_EVT_VOICE_ACTIVITY_UPDATE;
        apr_pkt_ptr->src_addr = apr_pkt_ptr->dst_addr;
        apr_pkt_ptr->src_port = apr_pkt_ptr->dst_port;
        apr_pkt_ptr->dst_addr = p_session->mvm_apr_addr;
        apr_pkt_ptr->dst_port = p_session->attached_mvm_handle;

        ( void )elite_apr_if_cmd_fwd( p_session->self_apr_handle, apr_pkt_ptr);
        break;
      }
      default:
      {
         /* Usupported sequential command. Ack back with unsupported. */
         result = ADSP_EUNSUPPORTED;
         elite_apr_if_end_cmd(p_session->self_apr_handle, apr_pkt_ptr, result);
         break;
      }

   } /* End of switch (opcode) */

   if (ADSP_EOK == result)
   {
      MSG_4(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "VoiceProcMgr [0x%lx], APR command with opcode: 0x%8X, is_cmd_pending: %lu, result: 0x%lx",
            p_session->self_apr_port, opcode, 
            p_session->session_cmd_ctrl.is_prev_cmd_pending,
            result);
   }
   else
   {
      MSG_3(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "VoiceProcMgr [0x%lx], APR command with opcode: 0x%8X FAILED, result: 0x%lx",
            p_session->self_apr_port, opcode, result);
   }

   return result;
}
