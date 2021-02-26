/**========================================================================
 @file vpm_session_state_ctrl.cpp

 @brief This file contains state control utilites for voice proc session object

 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_session_state_ctrl.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_internal.h"
#include "voice_proc_mgr.h"
#include "vpm_session_dyn_svc_cfg_utils.h"
#include "vpm_session_cal_utils.h"
#include "vpm_svc_vp3_utils.h"

#include "vpm_response_handler.h"
#include "vpm_session_utils.h"
#include "vpm_session_dyn_svc_cfg_utils.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

extern elite_svc_handle_t *txVoiceMatrix;
extern elite_svc_handle_t *rxVoiceMatrix;

/* -----------------------------------------------------------------------
** Function definitions
** ----------------------------------------------------------------------- */

static void vpm_log_state_info(vpm_session_object_t *session_obj)
{
  return;
}

/****************************************************************************
 * VPM STATE CONTROL ACTION
 ****************************************************************************/

static ADSPResult vpm_action_create_vocproc(vpm_session_object_t *session_obj)
{
   ADSPResult              result = ADSP_EOK;
   bool_t                  vprx_present = FALSE, vptx_present = FALSE;
   vpm_dyn_svc_info_t      *p_dyn_svc = &session_obj->dyn_svc_info;

   if (NULL == session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_action_create_vocproc(): session obj ptr is NULL");
      return ADSP_EBADPARAM;
   }

   MSG_6(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_action_create_vocproc(): tx_port: 0x%04X, tx_top_id: 0x%08X, tx_pp_sr: %lu," \
         "rx_port: 0x%04X, rx_top_id: 0x%08X, rx_pp_sr: %lu",
         session_obj->target_set.tx_port_id, session_obj->target_set.tx_topology_id,
         session_obj->target_set.system_config.tx_pp_sr, session_obj->target_set.rx_port_id,
         session_obj->target_set.rx_topology_id, session_obj->target_set.system_config.rx_pp_sr);

   MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_action_create_vocproc(): audio_ref_port: 0x%04X, "
         "ec_mode: 0x%08X", session_obj->target_set.ec_ref_port_id,
         session_obj->target_set.vocproc_mode);

   /* Create vpTx */
   if (VSS_IVOCPROC_PORT_ID_NONE != session_obj->target_set.tx_port_id)
   {
      if (ADSP_EOK != (result = vpm_create_tx_dyn_svc(session_obj)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_action_create_vocproc(): Failed to create vpTx dynamic svc");

         return result;
      }

      vptx_present = TRUE;
   }

   /* Create vpRx */
   if (VSS_IVOCPROC_PORT_ID_NONE != session_obj->target_set.rx_port_id)
   {
      if (ADSP_EOK != (result = vpm_create_rx_dyn_svc(session_obj)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_action_create_vocproc(): Failed to create vpRx dynamic svc");

         return result;
      }

      vprx_present = TRUE;
   }

   /* If both vpTx/Rx present, then connect these services */
   if (vptx_present && vprx_present)
   {
      if (ADSP_FAILED(result = vpm_connect_peer_svc(VOICEPROC_CONNECT_RX_PEER,
                                                    p_dyn_svc->tx->cmdQ,
                                                    session_obj->session_rsp_ctrl.p_rsp_queue,
                                                    p_dyn_svc->rx)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_action_create_vocproc(), VoiceProc Tx/Rx connection Failed, result: 0x%lx", result);

         return result;
      }
      else
      {
         MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
             "vpm_action_create_vocproc(), VoiceProc bind Rx/Tx success");
      }
   }

   /* If vpRx present, create vmx output port */
   if (vprx_present)
   {
      if (ADSP_EOK != (result = vpm_configure_rx_mixer_output(session_obj,
                                                              VMX_NEW_OUTPUT,
                                                              session_obj->target_set.system_config.rx_pp_sr)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "Failed to connect to Rx matrix, result: 0x%lx", result);
         return result;
      }

      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_action_create_vocproc(), Successfully connected to rx matrix, vmx_out_port_id: %lu",
            p_dyn_svc->mixer_out_port->outport_id);
   }

   /* Create Tx port and attach Vptx to matrix input */
   if (vptx_present)
   {
      MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_action_create_vocproc(), setting session_idx: 0x%lx, AFE port_id: 0x%lx",
            session_obj->self_apr_port,
            session_obj->target_set.tx_port_id);

      if (ADSP_EOK != (result = vpm_configure_tx_mixer_input(session_obj, VMX_NEW_INPUT)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_FATAL_PRIO,
               "vpm_action_create_vocproc(), Unable to connect to Tx matrix, result: 0x%lx",
               result);

         return result;
      }

      MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
            "vpm_action_create_vocproc(), Connected to Tx matrix,  vmx_in_port_id: 0x%lx",
            p_dyn_svc->mixer_in_port->inport_id);

      if (ADSP_EOK != (result = vpm_connect_vptx_to_vmx_inp_port(p_dyn_svc->tx->cmdQ,
                                                                 session_obj->session_rsp_ctrl.p_rsp_queue,
                                                                 &p_dyn_svc->mixer_in_port->port_handle)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_action_create_vocproc(), Unable to connect vpTx to vmx i/p port, result: 0x%lx",
               result);

      }

   } /* if vpTx present */

   if (ADSP_EOK == result)
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_action_create_vocproc(), vpTx/Rx Dynamic svc initialization SUCCESS");
   }
   else
   {

      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_action_create_vocproc(), vpTx/Rx Dynamic svc initialization FAILED");
   }

   return result;
}

static ADSPResult vpm_action_set_voice_timing(
  vpm_session_object_t *session_obj
)
{
  ADSPResult result = ADSP_EOK;
  vpm_set_timing_params_v2_t *timing_param_v2 = NULL;
  uint32_t session_id = session_obj->self_apr_port;;

  timing_param_v2 =
    (vpm_set_timing_params_v2_t*) qurt_elite_memory_malloc( sizeof(vpm_set_timing_params_v2_t),
                                                            QURT_ELITE_HEAP_DEFAULT );
  if (NULL == timing_param_v2 )
  {
    MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO,
         "VPM: ACTION_SET_VOICE_TIMING: Failed to allocate param payload");
    return ADSP_ENOMEMORY;
  }

  /* Populate the timing parameters based on target_set. */
  timing_param_v2->vsid = session_obj->target_set.voice_timing.vsid;
  timing_param_v2->mode = session_obj->target_set.voice_timing.vfr_mode;
  timing_param_v2->vfr_cycle = session_obj->target_set.voice_timing.vfr_cycle_duration_us;

  timing_param_v2->enc_offset = session_obj->target_set.voice_timing.enc_offset;
  timing_param_v2->vprx_delivery_offset = session_obj->target_set.voice_timing.vprx_delivery_offset;
  timing_param_v2->vptx_start_offset = session_obj->target_set.voice_timing.vptx_buf_start_offset;
  timing_param_v2->vptx_proc_start_offset = session_obj->target_set.voice_timing.vptx_proc_start_offset;
  timing_param_v2->vptx_delivery_offset = session_obj->target_set.voice_timing.vptx_delivery_offset;

  /* Set timing on VPM. */
  MSG_8( MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_action_set_voice_timing: vsid: 0x%08X, vfr_mode: %d, vfr_cycle_us: %d," \
         "vptx_buf_start_us: %d, vptx_proc_start_us: %d, vptx_delivery_us: %d," \
         "enc_offset_us: %d, vprx_delivery_us: %d", timing_param_v2->vsid,
         timing_param_v2->mode, timing_param_v2->vfr_cycle, timing_param_v2->vptx_start_offset,
         timing_param_v2->vptx_proc_start_offset, timing_param_v2->vptx_delivery_offset,
         timing_param_v2->enc_offset, timing_param_v2->vprx_delivery_offset );

  vpm_set_cmd_rsp_pending_status(session_obj, TRUE);

  session_obj->session_rsp_ctrl.p_ack_payload = timing_param_v2;

  if (session_obj->dyn_svc_info.tx)
  {
    result = vpm_send_vfr_mode_to_dyn_svc( timing_param_v2, session_obj->session_rsp_ctrl.p_rsp_queue,
                                           session_obj->dyn_svc_info.tx->cmdQ, VOICEPROC_SET_TIMINGV3_CMD );
    if( ADSP_EOK != result )
    {
      MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO,
           "VPM: ACTION_SET_VOICE_TIMING: Failed to send VFR and TIMING param msg to VpTx");
    }
    else
    {
      session_obj->session_rsp_ctrl.num_cmd_issued++;
    }
  }

   if (session_obj->dyn_svc_info.rx)
   {
      result = vpm_send_vfr_mode_to_dyn_svc( timing_param_v2, session_obj->session_rsp_ctrl.p_rsp_queue,
                                             session_obj->dyn_svc_info.rx->cmdQ, VOICEPROC_SET_TIMINGV3_CMD );
      if( ADSP_EOK != result )
      {
        MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "VPM: ACTION_SET_VOICE_TIMING: Failed to send VFR and TIMING param msg to VpRx");
      }
      else
      {
        session_obj->session_rsp_ctrl.num_cmd_issued++;
      }
   }

   if ( 0 == session_obj->session_rsp_ctrl.num_cmd_issued )
   {
     vpm_set_cmd_rsp_pending_status(session_obj, FALSE);
     qurt_elite_memory_free( timing_param_v2 );
     result = ADSP_EFAILED;
   }

   return result;
}

static ADSPResult vpm_action_enable_vocproc(vpm_session_object_t *session_obj)
{
   ADSPResult result = ADSP_EOK;

   if (NULL == session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_action_enable_vocproc(): Session obj pointer is NULL");

      return ADSP_EFAILED;
   }

   /* Connect with AFE Tx and Far ports */
   if (session_obj->dyn_svc_info.tx)
   {
      if (ADSP_EOK == (result = vpm_connect_afe_tx(session_obj)))
      {
         if (ADSP_EOK != (result = vpm_connect_afe_far(session_obj)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_action_enable_vocproc(): Failed to connect with AFE far port, result: 0x%lx",
                  result);

            return result;
         }
      }
      else
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_action_enable_vocproc(): Failed to connect with AFE Tx port, result: 0x%lx",
               result);

         return result;
      }
   }

   /* Connect with AFE Rx and connect AFE Rx to vpRx */
   if (session_obj->dyn_svc_info.rx)
   {
      if (ADSP_EOK == (result = vpm_connect_afe_rx(session_obj)))
      {
         if (ADSP_EOK != (result = vpm_connect_aferx_to_vprx(session_obj, VOICEPROC_CONNECT_DWN_STREAM_CMD)))
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_action_enable_vocproc(): Failed to connect AFE Rx to VpRx, result: 0x%lx",
                  result);

            return result;
         }
      }
      else
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_action_enable_vocproc(): Failed to connect with AFE Rx port, result: 0x%lx",
               result);

         return result;
      }
   }

   /* Send RUN command to VMX input port */
   if (session_obj->dyn_svc_info.mixer_in_port)
   {
      if (ADSP_EOK != (result = vpm_control_mixer_ports(session_obj,
                                                        session_obj->dyn_svc_info.mixer_in_port->inport_id,
                                                        VMX_INPUT_DIRECTION,
                                                        ELITEMSG_CUSTOM_VMX_RUN,
                                                        txVoiceMatrix->cmdQ)))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_action_enable_vocproc(): Failed to run VMX inport_id: %lu, result: 0x%lx",
               session_obj->dyn_svc_info.mixer_in_port, result);

         return result;
      }
   }

   /* Send RUN command to VMX output port */
   if (session_obj->dyn_svc_info.mixer_out_port)
   {
      if (ADSP_EOK != (result = vpm_control_mixer_ports(session_obj,
                                                        session_obj->dyn_svc_info.mixer_out_port->outport_id,
                                                        VMX_OUTPUT_DIRECTION,
                                                        ELITEMSG_CUSTOM_VMX_RUN,
                                                        rxVoiceMatrix->cmdQ)))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_action_enable_vocproc(): Failed to run VMX outport_id: %lu, result: 0x%lx",
               session_obj->dyn_svc_info.mixer_out_port, result);

         return result;
      }
   }

   // give session clock to run command.  services can use this for worst case offset calculation.  Only give session clock
   // since that is the minimum which individual service may assume.  Concurrency scenarios may change the clock, but
   // sessions will not be made aware.

   if (session_obj->dyn_svc_info.tx)
   {
      if (ADSP_EOK != vpm_send_run_msg_to_dyn_svc(session_obj, VSS_IVOCPROC_DIRECTION_TX))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_action_enable_vocproc(): Failed to run msg to vpTx result: 0x%lx", result);
      }
   }

   if (session_obj->dyn_svc_info.rx)
   {
      if (ADSP_EOK != vpm_send_run_msg_to_dyn_svc(session_obj, VSS_IVOCPROC_DIRECTION_RX))
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_action_enable_vocproc(): Failed to run msg to vpRx result: 0x%lx", result);
      }
   }

   return result;
}

static ADSPResult vpm_action_broadcast_vpm_status(vpm_session_object_t *session_obj,
                                                  uint32_t vpm_status_evt)
{
   ADSPResult                 result = ADSP_EOK;
   apr_list_node_t            *pivot_node;
   apr_list_node_t            *cur_node;
   vpm_attached_stream_item_t *attached_stream;
   uint32_t                   vpm_event;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_action_broadcast_vpm_status(): Session object pointer is NULL");

      return ADSP_EFAILED;
   }

   /* Iterate through attached stream list and send EVENT_READY to each
    * attached stream. */
   pivot_node = &(session_obj->attached_stream_list.dummy);

   MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "vpm_action_broadcast_vpm_status(): Sending broadcast event(0x%lX)"
         " to all attached streams", vpm_status_evt);

   while (APR_EOK == apr_list_get_next(&(session_obj->attached_stream_list),
                                       pivot_node,
                                       &cur_node))
   {
      attached_stream = (vpm_attached_stream_item_t *)cur_node;

      elite_apr_if_alloc_send_event(session_obj->self_apr_handle,
                                    session_obj->self_apr_addr,
                                    session_obj->self_apr_port,
                                    attached_stream->addr,
                                    attached_stream->port,
                                    0,
                                    vpm_status_evt,
                                    NULL,
                                    0);

      pivot_node = cur_node;
   }

   return result;
}

ADSPResult vpm_action_broadcast_afe_port_info(vpm_session_object_t *session_obj)
{
   ADSPResult                 result = ADSP_EOK;
   apr_list_node_t            *pivot_node;
   apr_list_node_t            *cur_node;
   vpm_attached_stream_item_t *attached_stream;
   vss_ivocproc_evt_afe_port_info_t afe_port_info;

   if (!session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_HIGH_PRIO,
          "vpm_action_broadcast_vpm_status(): Session object pointer is NULL");

      return ADSP_EFAILED;
   }

   /* Iterate through attached stream list and send EVENT_READY to each
    * attached stream. */
   pivot_node = &(session_obj->attached_stream_list.dummy);

   MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "vpm_action_broadcast_vpm_status(): Sending broadcast event(0x%lX)"
         " to all attached streams", VSS_IVOCPROC_EVT_AFE_PORT_INFO);

  afe_port_info.tx_port_id = session_obj->active_set.tx_port_id;
  afe_port_info.rx_port_id = session_obj->active_set.rx_port_id;
  afe_port_info.ec_ref_port_id = session_obj->active_set.ec_ref_port_id;

   while (APR_EOK == apr_list_get_next(&(session_obj->attached_stream_list),
                                       pivot_node,
                                       &cur_node))
   {
      attached_stream = (vpm_attached_stream_item_t *)cur_node;

      elite_apr_if_alloc_send_event(session_obj->self_apr_handle,
                                    session_obj->self_apr_addr,
                                    session_obj->self_apr_port,
                                    attached_stream->addr,
                                    attached_stream->port,
                                    0,
                                    VSS_IVOCPROC_EVT_AFE_PORT_INFO,
                                    &afe_port_info,
                                    sizeof(vss_ivocproc_evt_afe_port_info_t));

      pivot_node = cur_node;
   }

   return result;
}

/****************************************************************************
 * VPM STATE MACHINE                                                        *
 ****************************************************************************/


static ADSPResult vpm_state_reset(vpm_session_object_t *session_obj)
{
   ADSPResult   result = ADSP_EOK;

   switch (session_obj->state_ctrl.goal)
   {
      case CVP_GOAL_ENUM_CREATE:
      {
         switch (session_obj->state_ctrl.action)
         {
            case VPM_ACTION_ENUM_NONE:
            case VPM_ACTION_ENUM_SET_CLOCKS:
            {
               /* Set the VPM state to RESET */
               session_obj->state_ctrl.state = VPM_STATE_ENUM_RESET;

               /* Set next sequencer action */
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_CREATE_VOCPROC;

               if ( ADSP_EOK != ( result = vccm_mmpm_request_clocks ( VCCM_CLIENT_ID_CVP, 
                                                                      VPM_SESSION_CREATE_MPPS ) ) ) 
               {
                 session_obj->is_clk_overridden = FALSE;
                 MSG_1( MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_state_reset(): CVP_GOAL_ENUM_CREATE, "
                                       "failed to request clocks with rc(%d)",result );
               }
               else
               {
                 session_obj->is_clk_overridden = TRUE;
               }
               /* Fall through */
            }
            case VPM_ACTION_ENUM_CREATE_VOCPROC:
            {
               /* Set the next seq action */
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_REGISTER_VOICE_ACTIVITY_UPDATE_EVENT;

               if (ADSP_EOK == (result = vpm_action_create_vocproc(session_obj)))
               {
                  /* Update active_set of session config parameters */
                  vpm_update_device_active_set(session_obj);

                  session_obj->state_ctrl.status = ADSP_EOK;
               }
               else
               {
                  /* Update the failure status */
                  session_obj->state_ctrl.status = result;
               }
            }

            case VPM_ACTION_ENUM_REGISTER_VOICE_ACTIVITY_UPDATE_EVENT:
            {
               /* Set the next seq action */
              session_obj->state_ctrl.action = VPM_ACTION_ENUM_REVERT_CLOCKS;
              if ( ADSP_EOK != ( result = vpm_action_register_event( session_obj, VPM_EVT_VOICE_ACTIVITY_UPDATE ) ) )
              {
                MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_state_reset(): CVP_GOAL_ENUM_CREATE, failed to register VOICE ACTIVITY EVENT" );
              }
              /* fall through. */
            }

            case VPM_ACTION_ENUM_REVERT_CLOCKS:
            {
               /* Release the clocks */
               if ( TRUE == session_obj->is_clk_overridden )
               {
                 if ( ADSP_EOK != vccm_mmpm_release_clocks( VCCM_CLIENT_ID_CVP ) )
                 {
                   MSG_1( MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_state_reset(): CVP_GOAL_ENUM_CREATE, "
                                         "failed to release clocks with rc(%d)",result );
                 }
                 session_obj->is_clk_overridden = FALSE;
               }

               if (ADSP_EOK == session_obj->state_ctrl.status)
               {
                  /* Transtion VPM state to IDLE */
                  session_obj->state_ctrl.state = VPM_STATE_ENUM_IDLE;

                  MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                        "vpm_state_reset(): VPM state transition from RESET to IDLE,"
                        " session_id: 0x%lx", session_obj->self_apr_port);
               }

               vpm_complete_goal(session_obj, session_obj->state_ctrl.status);
               break;
            }
            default:
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_state_reset(): CVP_GOAL_ENUM_CREATE, un-supported action: 0x%08X",
                     session_obj->state_ctrl.action);

               vpm_complete_goal(session_obj, ADSP_EFAILED);

               return ADSP_EFAILED;
            }
         } /*  End of switch (session_obj->state_ctrl.action) */
         break;
      }
      case CVP_GOAL_ENUM_NONE:
      {
         /* Do nothing */
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_state_reset(): un-supported goal: 0x%08X",
               session_obj->state_ctrl.goal);

         result = ADSP_EFAILED;

         /* Complete the goal with failed status */
         vpm_complete_goal(session_obj, result);

         break;
      }

   } /* End of switch (session_obj->state_ctrl.goal)*/

   return result;
}

static ADSPResult vpm_state_idle(vpm_session_object_t *session_obj)
{
   ADSPResult  result = ADSP_EOK;

   switch (session_obj->state_ctrl.goal)
   {
      case CVP_GOAL_ENUM_REINIT:
      {
         switch (session_obj->state_ctrl.action)
         {
            case VPM_ACTION_ENUM_NONE:
            case VPM_ACTION_ENUM_CONTINUE:
            case VPM_ACTION_ENUM_REINIT_VOCPROC:
            {
               session_obj->state_ctrl.state = VPM_STATE_ENUM_IDLE;
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_REGISTER_VOICE_ACTIVITY_UPDATE_EVENT;

               result = vpm_reinit_dyn_svcs(session_obj, FALSE);
               /* Fall through */
            }

            case VPM_ACTION_ENUM_REGISTER_VOICE_ACTIVITY_UPDATE_EVENT:
            {
               /* Set the next seq action */
              session_obj->state_ctrl.action = VPM_ACTION_ENUM_COMPLETE;
              if ( ADSP_EOK != ( result = vpm_action_register_event( session_obj, VPM_EVT_VOICE_ACTIVITY_UPDATE ) ) )
              {
                MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_state_idle(): CVP_GOAL_ENUM_REINIT, failed to register VOICE ACTIVITY EVENT" );
              }
              /* fall through. */
            }

            case VPM_ACTION_ENUM_COMPLETE:
            {
               /* Complete the pending command and stay in the same state. */
               vpm_complete_goal(session_obj, result);
               break;
            }
            default:
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_state_idle(): CVP_GOAL_ENUM_REINIT, un-supported action: 0x%08X",
                     session_obj->state_ctrl.action);

               result = ADSP_EFAILED;

               /* Update the goal failure status */
               vpm_complete_goal(session_obj, result);

               break;
            }
         } /* End of switch (session_obj->state_ctrl.action) */
         break;
      } /* case CVP_GOAL_ENUM_REINIT: */

      case CVP_GOAL_ENUM_ENABLE:
      {
         switch (session_obj->state_ctrl.action)
         {
            case VPM_ACTION_ENUM_NONE:
            case VPM_ACTION_ENUM_CONTINUE:
            case VPM_ACTION_ENUM_SET_VOICE_TIMING:
            {
               /* Set the next action */
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_SET_RX_TX_MUTE;

               if (ADSP_EOK == (result = vpm_action_set_voice_timing(session_obj)))
               {
                  /* Update the active set. */
                  session_obj->active_set.system_config.vsid = session_obj->target_set.system_config.vsid;
                  session_obj->active_set.system_config.vfr_mode = session_obj->target_set.system_config.vfr_mode;
                  session_obj->active_set.voice_timing = session_obj->target_set.voice_timing;

                  /* Wait for custom msg  responses */
                  return result;
               }
          } /* Fall through */

            case VPM_ACTION_ENUM_SET_RX_TX_MUTE:
            {
               /* Check the response for previous action */
               if (ADSP_EOK != (result = session_obj->session_rsp_ctrl.status))
               {
                  /* Complete the current goal with failed status */
                  vpm_complete_goal(session_obj, result);
                  return result;
               }

               /* Set the next action */
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_SET_VP3;

               /* When applying cached setting, set the ramp duartion to 0. */
               if (ADSP_EOK == (result = vpm_action_set_mute(session_obj, 0, VOICE_SET_MUTE_TX_AND_RX)))
               {
                  /* Update active_set. */
                  session_obj->active_set.client_tx_mute = session_obj->target_set.client_tx_mute;
                  session_obj->active_set.client_rx_mute = session_obj->target_set.client_rx_mute;

                  /* Wait for custom msg response */
                  return result;
               }
               /* Fall through */
            }
            case VPM_ACTION_ENUM_SET_VP3:
            {
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_SET_UI_PROPERTIES;

               if (ADSP_EOK == (result = vpm_action_set_vp3(session_obj)))
               {
                  /* Wait for set param responses for set Vp3 */
                  return result;
               }
               /* Fall through */
            }
            case VPM_ACTION_ENUM_SET_UI_PROPERTIES:
            {
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_ENABLE_VOCPROC;

               if (ADSP_EOK == (result = vpm_action_set_ui_properties(session_obj)))
               {
                  /* Wait for set param responses for set UI prop */
                  return result;
               }
               /* Fall through */
            }
            case VPM_ACTION_ENUM_ENABLE_VOCPROC:
            {
               session_obj->state_ctrl.state = VPM_STATE_ENUM_IDLE;

               if (ADSP_EOK != (result = session_obj->session_rsp_ctrl.status))
               {
                  /* Complete the current goal with failed status */
                  vpm_complete_goal(session_obj, ADSP_EFAILED);
                  return result;
               }

               if (TRUE == session_obj->is_client_set_sectors)
               {
                  session_obj->state_ctrl.action = VPM_ACTION_ENUM_SET_SOUNDFOCUS_SECTORS;
               }
               else
               {
                  session_obj->state_ctrl.action = VPM_ACTION_ENUM_BROADCASAT_AFE_PORT_INFO;
               }

               session_obj->state_ctrl.status = ADSP_EFAILED;

               if (ADSP_EOK != (result = vpm_action_enable_vocproc(session_obj)))
               {
                  /* Complete the current goal with failed status */
                  vpm_complete_goal(session_obj, result);
                  return result;
               }

               /* Return back to state control.
                  Based on the next action set in above conditional,
                  state_control() will fall through that switch case */
               return ADSP_EIMMEDIATE;
            }
            case VPM_ACTION_ENUM_SET_SOUNDFOCUS_SECTORS:
            {
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_BROADCASAT_AFE_PORT_INFO;

               if (ADSP_EOK == (result = vpm_action_set_soundfocus_sectors(session_obj)))
               {
                  /* Wait for set param responses */
                  return result;
               }

               /* Fall through */

               /* There is no active_set for sector configuration, because
                  1) Sector configuration change will not cause KPPS/BW requirement change
                     (confirmed with VCP team);
                  2) Sector configuration is always pushed down to VPM. CVP does not
                     cache the configuration (Fluence may adjust sector configuration due to
                     various reasons, e.g., two-mic mirroring,and minimum sector restrictions, etc.) */
            }
            case VPM_ACTION_ENUM_BROADCASAT_AFE_PORT_INFO:
            {
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_BROADCAST_READY;
               ( void )vpm_action_broadcast_afe_port_info(session_obj);
               /* Fall through */
            }
            case VPM_ACTION_ENUM_BROADCAST_READY:
            {
               /* State transition from IDLE to RUN */
               session_obj->state_ctrl.state = VPM_STATE_ENUM_RUN;
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_CONTINUE;

               result = vpm_action_broadcast_vpm_status(session_obj, VSS_IVOCPROC_EVT_READY);

               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "vpm_state_idle(): VPM state transition from IDLE to RUN,"
                     " session_id: 0x%lx", session_obj->self_apr_port);

               vpm_complete_goal(session_obj, result);
               break;
            }
            default:
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_state_idle(): CVP_GOAL_ENUM_ENABLE: Un-supported action: 0x%lx",
                     session_obj->state_ctrl.action);

               result = ADSP_EFAILED;

               /* Complete the goal with FAIL status */
               vpm_complete_goal(session_obj, result);

               break;
            }
         } /* End of switch (session_obj->state_ctrl.action) */

         break;
      } /* case CVP_GOAL_ENUM_ENABLE: */

      case CVP_GOAL_ENUM_DESTROY:
      {
         switch (session_obj->state_ctrl.action)
         {
            case VPM_ACTION_ENUM_NONE:
            case VPM_ACTION_ENUM_CONTINUE:
            case VPM_ACTION_ENUM_DESTROY_VOCPROC:
            {
               /* State transition to RESET */
               session_obj->state_ctrl.state = VPM_STATE_ENUM_RESET;
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_CONTINUE;

               /* Destroy dynamic services */
               result = vpm_destroy_session_dyn_svc(session_obj, TRUE, TRUE);

               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "vpm_state_idle(): VPM state transition from IDLE to RESET,"
                     " session_id: 0x%lx", session_obj->self_apr_port);

               /* Complete the pending command and stay in the same state. */
               vpm_complete_goal(session_obj, result);

               break;
            }

            default:
            {
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_state_idle(): CVP_GOAL_ENUM_DESTROY: Un-supported "
                  "action: 0x%lx", session_obj->state_ctrl.action);

               result = ADSP_EFAILED;

               /* Complete goal with failure status */
               vpm_complete_goal(session_obj, result);

               break;
            }
         } /* End of switch (session_obj->state_ctrl.action) */
     } /* case CVP_GOAL_ENUM_DESTROY */

      case CVP_GOAL_ENUM_NONE:
      {
         /* Do nothing */
         break;
      }

      default:
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_state_idle(): Un-supported goal id:  0x%lx",
               session_obj->state_ctrl.goal);
         break;
      }
   } /* End of switch (session_obj->state_ctrl.goal) */

   return result;
}

static ADSPResult vpm_state_run(vpm_session_object_t *session_obj)
{
   ADSPResult result = ADSP_EOK;

   switch (session_obj->state_ctrl.goal)
   {
      case CVP_GOAL_ENUM_REINIT:
      case CVP_GOAL_ENUM_DISABLE:
      case CVP_GOAL_ENUM_DESTROY:
      case CVP_GOAL_ENUM_ENABLE: /* This is a re-enable due to network change. */
      {
         switch (session_obj->state_ctrl.action)
         {
            case VPM_ACTION_ENUM_NONE:
            case VPM_ACTION_ENUM_BROADCAST_NOT_READY:
            {
               session_obj->state_ctrl.state = VPM_STATE_ENUM_RUN;
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_GET_VP3;

               result = vpm_action_broadcast_vpm_status(session_obj, VSS_IVOCPROC_EVT_NOT_READY);

               /* Fall through */
            }
            case VPM_ACTION_ENUM_GET_VP3:
            {
               /* Set the next action */
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_DISABLE_VOCPROC;

               if (ADSP_EOK == (result = vpm_action_get_vp3(session_obj)))
               {
                  /* Return and wait for get param responses */
                  return result;
               }
               /* Fall through */
            }
            case VPM_ACTION_ENUM_DISABLE_VOCPROC:
            {
               /* State transition to IDLE */
               session_obj->state_ctrl.state = VPM_STATE_ENUM_IDLE;
               session_obj->state_ctrl.action = VPM_ACTION_ENUM_COMPLETE;

               result = vpm_action_disable_vocproc(session_obj);

               /* Save the intermediate goal status */
               session_obj->state_ctrl.status = result;

               if ((CVP_GOAL_ENUM_REINIT == session_obj->state_ctrl.goal) ||
                   (CVP_GOAL_ENUM_ENABLE == session_obj->state_ctrl.goal))
               {
                  /* For Re-init / Enable, rest of the cmd seqencer will execute in
                     IDLE state */

                  session_obj->state_ctrl.action = VPM_ACTION_ENUM_CONTINUE;

                  return ADSP_EIMMEDIATE;
               }
               /* Fall through */
            }
            case VPM_ACTION_ENUM_COMPLETE:
            {
               MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
                     "vpm_state_run(): VPM state transition from RUN to IDLE,"
                     " session_id: 0x%lx", session_obj->self_apr_port);

               /* Complete the pending command and stay in the same state. */
               vpm_complete_goal(session_obj, result);

               break;
            }
            default:
            {
               MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                     "vpm_state_run(): goal_id: 0x%lx: Un-supported action: 0x%lx",
                     session_obj->state_ctrl.goal, session_obj->state_ctrl.action);

               result = ADSP_EFAILED;

               /* Complete goal with failure status */
               vpm_complete_goal(session_obj, result);

               break;
            }
         } /* End of switch (session_obj->state_ctrl.action) */

         break;
      }
      case CVP_GOAL_ENUM_NONE:
      {
         /* Do nothing*/
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_state_run(): : Un-supported goal_id: 0x%lx",
               session_obj->state_ctrl.goal);

         result = ADSP_EFAILED;

         /* Complete goal with failure status */
         vpm_complete_goal(session_obj, result);

         break;
      }
   } /* End of switch (session_obj->state_ctrl.goal) */

   return result;
}


ADSPResult vpm_state_control(vpm_session_object_t *session_obj)
{
   ADSPResult result = ADSP_EOK;

   if (NULL == session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_state_control: session obj ptr is NULL");
      return ADSP_EBADPARAM;
   }

   do
   {
      vpm_log_state_info(session_obj);

      switch (session_obj->state_ctrl.state)
      {

         case VPM_STATE_ENUM_RESET:
         {
            result = vpm_state_reset(session_obj);
            break;
         }
         case VPM_STATE_ENUM_IDLE:
         {
            result = vpm_state_idle(session_obj);
            break;
         }
         case VPM_STATE_ENUM_RUN:
         {
            result = vpm_state_run(session_obj);
            break;
         }
         default:
         {
            result = ADSP_EUNEXPECTED;
            break;
         }
      }
   }while (result == ADSP_EIMMEDIATE);

   vpm_log_state_info(session_obj);

   if (result == APR_ECONTINUE)
   { /* TODO: Use a timer to trigger the state machine to run after 5ms. */
   }

   return result;
}

