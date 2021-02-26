/**========================================================================
 @file vpm_session_utils.cpp
 
 @brief This file contains utilites for voice proc session object create/destroy,
 enable/disable, re-init.
 
 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_session_utils.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

#include "adsp_vparams_api.h"
#include "adsp_vcmn_api.h"
#include "adsp_vpm_api.h"
#include "adsp_afe_service_commands.h"
#include "vss_public_if.h"

#include "vccm_api.h"
#include "vss_private_if.h"
#include "cvd_task.h"
#include "cvd_cal_common_i.h"
#include "cvd_cal_log_i.h"

#include "vpm_internal.h"
#include "vpm_types.h"

#include "voice_proc_mgr.h"
#include "vpm_session_dyn_svc_cfg_utils.h"
#include "vpm_svc_apr_data_base.h"
#include "vpm_session_utils.h"
#include "VoiceCmnUtils.h"
#include "mmstd.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Maximum number of commands expected ever in command queue */
static const uint32_t VPM_SESSION_CMD_Q_MAX_ELEMENTS = 32;
static const uint32_t VPM_SESSION_RESP_Q_MAX_ELEMENTS = 32;

/* Declare buffers for maximumm number of sessions */
static char vpm_session_cmd_q[VPM_MAX_NUM_SESSIONS][QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(VPM_SESSION_CMD_Q_MAX_ELEMENTS)];
static char vpm_session_rsp_q[VPM_MAX_NUM_SESSIONS][QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(VPM_SESSION_RESP_Q_MAX_ELEMENTS)];

/* =======================================================================
Function Definitions
========================================================================== */

ADSPResult vpm_check_topo_commit_state(vpm_session_object_t *session_obj)
{
   ADSPResult result = ADSP_EOK;

   /* Check if session creation is complete or not */
   if (VPM_TOPOLOGY_COMMIT_STATE_CREATE == session_obj->topo_commit_state)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_check_topo_commit_state(): Topo commit is not done yet");

      result = ADSP_EFAILED;
   }

   return result;
}

void vpm_set_session_cmd_pending_status(uint32_t vpm_session_idx, bool_t is_pending)
{
   vpm_session_object_t *p_session = p_vpm_svc_global_obj->sessions[vpm_session_idx];

   /* Set the pending flag status (TRUE / FALSE)*/
   p_session->session_cmd_ctrl.is_prev_cmd_pending = is_pending;

   if (TRUE == is_pending)
   {
      /* Remove the session cmdQ from the current signal wait mask */
      p_vpm_svc_global_obj->curr_wait_mask &=\
         ~(p_vpm_svc_global_obj->session_static_info[vpm_session_idx].session_cmdq_mask);

      /* Add the session cmdQ to the pending cmd  mask */
      p_vpm_svc_global_obj->pending_cmd_mask |=\
         p_vpm_svc_global_obj->session_static_info[vpm_session_idx].session_cmdq_mask;
   }
   else /* Not pending or completed */
   {
      /* Add back the session cmdQ to the current signal wait mask */
      p_vpm_svc_global_obj->curr_wait_mask |=\
         p_vpm_svc_global_obj->session_static_info[vpm_session_idx].session_cmdq_mask;

      /* Remove  the session cmdQ from the pending cmd mask */
      p_vpm_svc_global_obj->pending_cmd_mask &=\
         ~(p_vpm_svc_global_obj->session_static_info[vpm_session_idx].session_cmdq_mask);
   }

   return;
}

void vpm_check_goal_completion_status(vpm_session_object_t *session_obj)
{
   elite_apr_packet_t *p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   if (TRUE == session_obj->state_ctrl.is_goal_completed)
   {
      /* Mark the command as completed and start listening to session command Q */
      vpm_set_session_cmd_pending_status(session_obj->session_id, FALSE);

      /* End the APR command with goal status as result */
      elite_apr_if_end_cmd(session_obj->self_apr_addr,
                           p_apr_pkt,
                           session_obj->state_ctrl.status);
   }
   else /* Current goal is not completed */
   {
      /* Set the command status as pending. Remove the session cmd Q from the svc workloop
          wait maask */
      if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
      {
         vpm_set_session_cmd_pending_status(session_obj->session_id, TRUE);
      }
   }

   return;
}

void vpm_update_device_active_set(vpm_session_object_t *session_obj)
{
  if (session_obj == NULL)
  {
     MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_update_device_active_set() failed, NULL session object");
     return;
  }

  session_obj->active_set.tx_port_id = session_obj->target_set.tx_port_id;
  session_obj->active_set.rx_port_id = session_obj->target_set.rx_port_id;
  session_obj->active_set.tx_topology_id = session_obj->target_set.tx_topology_id;
  session_obj->active_set.rx_topology_id = session_obj->target_set.rx_topology_id;
  session_obj->active_set.vocproc_mode = session_obj->target_set.vocproc_mode;
  session_obj->active_set.ec_ref_port_id = session_obj->target_set.ec_ref_port_id;
  session_obj->active_set.rx_in_sample_rate = session_obj->target_set.rx_in_sample_rate;
  session_obj->active_set.num_dev_channels.tx_num_channels = session_obj->target_set.num_dev_channels.tx_num_channels;
  session_obj->active_set.num_dev_channels.rx_num_channels = session_obj->target_set.num_dev_channels.rx_num_channels;

  if ( session_obj->target_set.dev_chan_info.tx.num_channels > 0 ) 
  {
    mmstd_memcpy( &session_obj->active_set.dev_chan_info.tx,  
                  sizeof(session_obj->active_set.dev_chan_info.tx),
                  &session_obj->target_set.dev_chan_info.tx,  
                  sizeof(session_obj->target_set.dev_chan_info.tx) );
  }
  if ( session_obj->target_set.dev_chan_info.rx.num_channels > 0 ) 
  {
    mmstd_memcpy( &session_obj->active_set.dev_chan_info.rx,  
                  sizeof(session_obj->active_set.dev_chan_info.rx),
                  &session_obj->target_set.dev_chan_info.rx,  
                  sizeof(session_obj->target_set.dev_chan_info.rx) );
  }
  if ( session_obj->target_set.dev_chan_info.ec_ref.num_channels > 0 ) 
  {
    mmstd_memcpy( &session_obj->active_set.dev_chan_info.ec_ref,  
                  sizeof(session_obj->active_set.dev_chan_info.ec_ref), 
                  &session_obj->target_set.dev_chan_info.ec_ref,  
                  sizeof(session_obj->target_set.dev_chan_info.ec_ref) );
  }
  return;
}

bool_t vpm_is_hdvoice_feature_supported(uint32_t feature_id)
{
   bool_t is_feat_supported = FALSE;

   switch (feature_id)
   {
      case VSS_ICOMMON_CAL_FEATURE_WIDEVOICE:
      case VSS_ICOMMON_CAL_FEATURE_WIDEVOICE2:
      case VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE:
      {
         is_feat_supported = TRUE;
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "vpm_is_hdvoice_feature_supported(): Un-supported "
               "target feature ID (0x%08X)", feature_id);

         is_feat_supported = FALSE;
         break;
      }
   }

   return is_feat_supported;
}

bool_t vpm_sr_is_valid(uint32_t sample_rate)
{
   bool_t result;

   switch (sample_rate)
   {
      case 8000:
      case 16000:
      case 32000:
      case 41000:
      case 48000:
         result = TRUE;
         break;

      default:
         result = FALSE;
         break;
   }

   return result;
}

bool_t vpm_media_id_is_nb_sr(uint32_t media_id)
{
   bool_t rc;

   switch (media_id)
   {
      case VSS_MEDIA_ID_NONE:
      case VSS_MEDIA_ID_13K:
      case VSS_MEDIA_ID_EVRC:
      case VSS_MEDIA_ID_4GV_NB:
      case VSS_MEDIA_ID_AMR_NB:
      case VSS_MEDIA_ID_EFR:
      case VSS_MEDIA_ID_FR:
      case VSS_MEDIA_ID_HR:
      case VSS_MEDIA_ID_PCM_8_KHZ:
      case VSS_MEDIA_ID_G711_ALAW:
      case VSS_MEDIA_ID_G711_MULAW:
      case VSS_MEDIA_ID_G711_ALAW_V2:
      case VSS_MEDIA_ID_G711_MULAW_V2:
      case VSS_MEDIA_ID_G711_LINEAR:
      case VSS_MEDIA_ID_G729:
      case VSS_MEDIA_ID_G722:
      {
         rc = TRUE;
         break;
      }
      default:
      {
         rc = FALSE;
         break;
      }
   }

   /* VSS_MEDIA_ID_NONE is not considered a valid parameter because
    * system cannot run without a media type.
    */

   return rc;
}

uint32_t vpm_get_sampling_rate(uint32_t media_type_format,
                               uint32_t feature_id,
                               uint16_t evs_sampling_rate)
{
   uint32_t sampling_rate = 0;

   switch (media_type_format)
   {
      case VSM_MEDIA_TYPE_NONE:
      {
         sampling_rate = 0;
         break;
      }
         /* Narrowband */
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
         sampling_rate = VOICE_NB_SAMPLING_RATE;
         break;
      }
      case VSM_MEDIA_TYPE_AMR_NB_MODEM:
      case VSM_MEDIA_TYPE_AMR_NB_IF2:
      {
         sampling_rate = VOICE_NB_SAMPLING_RATE;

         if (VSS_ICOMMON_CAL_FEATURE_BEAMR == feature_id)
         {
            sampling_rate = VOICE_WB_SAMPLING_RATE;
         }
         break;
      }
         /* Wideband */
      case VSM_MEDIA_TYPE_4GV_WB_MODEM:
      case VSM_MEDIA_TYPE_4GV_NW_MODEM:
      case VSM_MEDIA_TYPE_4GV_NW:
      case VSM_MEDIA_TYPE_AMR_WB_MODEM:
      case VSM_MEDIA_TYPE_PCM_16_KHZ:
      case VSM_MEDIA_TYPE_G722:
      case VSM_MEDIA_TYPE_EAMR:
      case VSM_MEDIA_TYPE_EVRC_NW_2K:
      {
         sampling_rate = VOICE_WB_SAMPLING_RATE;
         break;
      }
      case VSM_MEDIA_TYPE_PCM_32_KHZ:
      {
         sampling_rate = VOICE_SWB_SAMPLING_RATE;
         break;
      }
      case VSM_MEDIA_TYPE_PCM_48_KHZ:
      {
         sampling_rate = VOICE_FB_SAMPLING_RATE;
         break;
      }
      case VSM_MEDIA_TYPE_EVS:
      {
         sampling_rate = evs_sampling_rate;
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
               "vpm_get_sampling_rate(): Invalid meda_type: 0x%lx",  media_type_format);
         break;
      }
   }

   return sampling_rate;
}

ADSPResult vpm_get_free_session_idx(voice_proc_mgr_t *pMe,
                                    uint32_t *session_idx)
{
   ADSPResult  result = ADSP_EFAILED;
   uint32_t  session_ctr = 0;

   for (session_ctr = 0; session_ctr < VPM_MAX_NUM_SESSIONS; session_ctr++)
   {
      if (NULL == pMe->sessions[session_ctr])
      {
         *session_idx = session_ctr;

         result = ADSP_EOK;

         break;
      }
   }

   return result;
}


ADSPResult vpm_init_static_session_info(voice_proc_mgr_t *pMe, uint8_t session_idx)
{
   ADSPResult                 result = ADSP_EOK;
   char                       cmd_q_name[QURT_ELITE_DEFAULT_NAME_LEN];
   char                       resp_q_Name[QURT_ELITE_DEFAULT_NAME_LEN];
   int                        sig_bit_pos;
   qurt_elite_queue_t         * p_temp_cmd_q,*p_temp_resp_Q;

   if (!pMe || (session_idx >= VPM_MAX_NUM_SESSIONS))
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "Invalid VPM svc pointer (0x%lx) and/or session_idx (%lu) out of bound",
            pMe, session_idx);

      return result;
   }

   vpm_session_static_info_t  *p_session_static_info = &(pMe->session_static_info[session_idx]);

   /* Create cmd  and resp queue */
   snprintf(cmd_q_name, QURT_ELITE_DEFAULT_NAME_LEN, "VPMsc%d", session_idx);
   snprintf(resp_q_Name, QURT_ELITE_DEFAULT_NAME_LEN, " VPMsc%d", session_idx);

   p_temp_cmd_q = (qurt_elite_queue_t *)&vpm_session_cmd_q[session_idx];
   p_temp_resp_Q = (qurt_elite_queue_t *)&vpm_session_rsp_q[session_idx];

   /* Cmd and Resp queue init for each VPM session */
   if (ADSP_FAILED(result = qurt_elite_queue_init(cmd_q_name, VPM_SESSION_CMD_Q_MAX_ELEMENTS, p_temp_cmd_q)) ||
                            ADSP_FAILED(result = qurt_elite_queue_init(resp_q_Name, VPM_SESSION_RESP_Q_MAX_ELEMENTS, p_temp_resp_Q)))

   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "Failed to create VoiceProgMgr session Cmd and/or Resp queues");
      return result;
   }

   /* Init the state variables */
   p_session_static_info->p_session_cmd_q = p_temp_cmd_q;
   p_session_static_info->p_session_resp_Q = p_temp_resp_Q;

   /* Calcuate the bit mask for this session's command queue.
      Bit 0 and 1 of the channel are reserved for service cmd/rsp Q respectively.
      Session X cmd queue shall take bit 2*x+2 and rep queue takes bit
      2*x+3 */

   sig_bit_pos  = (2 * session_idx + 2);

   p_session_static_info->session_cmdq_mask  = (1 << sig_bit_pos);

   /* Add command queue to channel */
   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel,
                                                    p_session_static_info->p_session_cmd_q,
                                                    p_session_static_info->session_cmdq_mask)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "Failed to add session cmd Q to channel, result: 0x%lx", result);

      return result;
   }

   /* Add response queue to channel */
   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel,
                                                    p_session_static_info->p_session_resp_Q,
                                                    p_session_static_info->session_cmdq_mask << 1)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "Failed to add session response Q to channel, result: 0x%lx", result);

      return result;
   }

   /* Mark the session as invalid */
   pMe->sessions[session_idx] = NULL;

   /* Update the signal wait mask to be used by static service workloop */
   pMe->session_cmd_q_wait_mask |= p_session_static_info->session_cmdq_mask;
   pMe->session_rsp_q_wait_mask |= (p_session_static_info->session_cmdq_mask << 1);

   return ADSP_EOK;
}

/* This function creates all sessions state machine here. */
ADSPResult vpm_create_session_static_info(voice_proc_mgr_t *pMe)
{
   uint32_t  session_idx;

   for (session_idx = 0; session_idx < VPM_MAX_NUM_SESSIONS; session_idx++)
   {
      if (ADSP_FAILED(vpm_init_static_session_info(pMe, session_idx)))
      {
         MSG_1(MSG_SSID_DFLT, DBG_FATAL_PRIO, "Failed to init VoiceProcMgr static session: %lu",
               session_idx);

         return ADSP_EFAILED;
      }
   }

   return ADSP_EOK;
}

/**
 * This function destroy session state assuming this session is
 * no longer in use. This function should only be called when
 * the whole static service is destroyed
 *
 *
 */
ADSPResult vpm_deinit_session_static_info(voice_proc_mgr_t *pMe, uint8_t session_idx)
{
   vpm_session_static_info_t *p_static_info = NULL;

   if (session_idx < VPM_MAX_NUM_SESSIONS)
   {
      p_static_info = &(pMe->session_static_info[session_idx]);
   }

   if (NULL == p_static_info)
   {
      return ADSP_EFAILED;
   }

   elite_svc_deinit_cmd_queue(p_static_info->p_session_cmd_q);
   elite_svc_deinit_cmd_queue(p_static_info->p_session_resp_Q);

   memset(p_static_info, 0, sizeof(vpm_session_static_info_t));

   return ADSP_EOK;
}


ADSPResult vpm_destroy_session_static_info(voice_proc_mgr_t *pMe)
{
   ADSPResult  result = ADSP_EOK;
   uint32_t    session_idx;

   for (session_idx = 0; session_idx < VPM_MAX_NUM_SESSIONS; session_idx++)
   {
      result |= vpm_deinit_session_static_info(pMe, session_idx);
   }

   return result;
}


ADSPResult vpm_add_session(voice_proc_mgr_t *pMe,
                           vpm_session_object_t *session_obj,
                           uint32_t session_idx)
{
   ADSPResult result = ADSP_EOK;

   /* Start listening to session cmd and rsp Q's */
   pMe->curr_wait_mask |= pMe->session_static_info[session_idx].session_cmdq_mask; /* CMD Q */
   pMe->curr_wait_mask |= pMe->session_static_info[session_idx].session_cmdq_mask << 1; /* RSP Q */


   /* Save the session obj pointer and index in obj struct */
   pMe->sessions[session_idx] = session_obj;

   /* Increment session counter */
   pMe->session_cnt++;

   return result;
}

static inline ADSPResult vpm_remove_session(voice_proc_mgr_t *pMe,
                                            vpm_session_object_t *session_obj)
{
   ADSPResult result = ADSP_EOK;
   uint32_t   session_idx = session_obj->session_id;

   if (!pMe->session_cnt)
   {
      return ADSP_EFAILED;
   }

   /* Stop listening to session cmd and rsp Q's */
   pMe->curr_wait_mask &= ~(pMe->session_static_info[session_idx].session_cmdq_mask); /* CMD Q */
   pMe->curr_wait_mask &= ~(pMe->session_static_info[session_idx].session_cmdq_mask << 1); /* RSP Q */

   /* Set the session obj pointer to NULL */
   pMe->sessions[session_obj->session_id] = NULL;

   /* decrement session counter */
   pMe->session_cnt--;

   return result;
}

ADSPResult vpm_get_mute_mode(vpm_session_object_t *session_obj,
                             voice_mute_path_t *mute_mode)
{
   ADSPResult  result = ADSP_EFAILED;
   bool_t      mute_tx = FALSE;

   if ( session_obj->active_set.client_tx_mute != session_obj->target_set.client_tx_mute )
   {
      mute_tx = TRUE;
      *mute_mode = VOICE_SET_MUTE_TX_ONLY;

      result = ADSP_EOK;
   }

   if ( session_obj->active_set.client_rx_mute != session_obj->target_set.client_rx_mute )
   {
      if (TRUE == mute_tx)
      {
         *mute_mode = VOICE_SET_MUTE_TX_AND_RX;
      }
      else
      {
         *mute_mode = VOICE_SET_MUTE_RX_ONLY;
      }

      result = ADSP_EOK;
   }

   return result;
}

/* BACKWARD COMPATIBILITY */
/* Convert legacy network id to new network id + sample rates.
   (Also validate legacy network id in the process.) */

static ADSPResult vpm_convert_legacy_network_id(uint32_t legacy_network_id,
                                                uint32_t *ret_network_id,
                                                uint32_t *ret_rx_pp_sr,
                                                uint32_t *ret_tx_pp_sr)
{
   ADSPResult result = ADSP_EOK;

   if (ret_network_id == NULL)
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_convert_legacy_network_id(): NULL legacy n/w ID: %lu",
            ret_network_id);

      return ADSP_EBADPARAM;
   }

   switch (legacy_network_id)
   {
      case VSS_NETWORK_ID_DEFAULT:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_NONE;
         *ret_rx_pp_sr = VPM_DEFAULT_RX_PP_SR;
         *ret_tx_pp_sr = VPM_DEFAULT_TX_PP_SR;
         break;
      }
      case VSS_NETWORK_ID_CDMA_NB:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_CDMA;
         *ret_rx_pp_sr = 8000;
         *ret_tx_pp_sr = 8000;
         break;
      }
      case VSS_NETWORK_ID_CDMA_WB:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_CDMA;
         *ret_rx_pp_sr = 16000;
         *ret_tx_pp_sr = 16000;
         break;
      }
      case VSS_NETWORK_ID_CDMA_WV:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_CDMA;
         *ret_rx_pp_sr = 16000;
         *ret_tx_pp_sr = 8000;
         break;
      }
      case VSS_NETWORK_ID_GSM_NB:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_GSM;
         *ret_rx_pp_sr = 8000;
         *ret_tx_pp_sr = 8000;
         break;
      }
      case VSS_NETWORK_ID_GSM_WB:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_GSM;
         *ret_rx_pp_sr = 16000;
         *ret_tx_pp_sr = 16000;
         break;
      }
      case VSS_NETWORK_ID_GSM_WV:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_GSM;
         *ret_rx_pp_sr = 16000;
         *ret_tx_pp_sr = 8000;
         break;
      }
      case VSS_NETWORK_ID_WCDMA_NB:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_WCDMA;
         *ret_rx_pp_sr = 8000;
         *ret_tx_pp_sr = 8000;
         break;
      }
      case VSS_NETWORK_ID_WCDMA_WB:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_WCDMA;
         *ret_rx_pp_sr = 16000;
         *ret_tx_pp_sr = 16000;
         break;
      }
      case VSS_NETWORK_ID_WCDMA_WV:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_WCDMA;
         *ret_rx_pp_sr = 16000;
         *ret_tx_pp_sr = 8000;
      }
         break;

      case VSS_NETWORK_ID_VOIP_NB:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_VOIP;
         *ret_rx_pp_sr = 8000;
         *ret_tx_pp_sr = 8000;
         break;
      }
      case VSS_NETWORK_ID_VOIP_WB:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_VOIP;
         *ret_rx_pp_sr = 16000;
         *ret_tx_pp_sr = 16000;
         break;
      }
      case VSS_NETWORK_ID_VOIP_WV:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_VOIP;
         *ret_rx_pp_sr = 16000;
         *ret_tx_pp_sr = 8000;
      }
         break;

      case VSS_NETWORK_ID_LTE_NB:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_LTE;
         *ret_rx_pp_sr = 8000;
         *ret_tx_pp_sr = 8000;
         break;
      }
      case VSS_NETWORK_ID_LTE_WB:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_LTE;
         *ret_rx_pp_sr = 16000;
         *ret_tx_pp_sr = 16000;
         break;
      }
      case VSS_NETWORK_ID_LTE_WV:
      {
         *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_LTE;
         *ret_rx_pp_sr = 16000;
         *ret_tx_pp_sr = 8000;
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_convert_legacy_network_id(): Incorrect legacy n/w ID: %lu",
               legacy_network_id);

         result = ADSP_EBADPARAM;
         break;
      }
   } /* End of switch(legacy_network_id) */

   return result;
}

static ADSPResult vpm_create_session_object(vpm_session_object_t **ret_session_obj)
{
   ADSPResult              result = ADSP_EOK;
   vpm_session_object_t    *session_obj = NULL;
   uint32_t                i;

   if (NULL == ret_session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_create_session_object(): output pointer is NULL");

      return ADSP_EBADPARAM;
   }

   MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_create_session_object(): sizeof( vpm_session_object_t ): %lu bytes",
         sizeof(vpm_session_object_t));

   /* Allocate the session object. */
   if (NULL == (session_obj = (vpm_session_object_t *)qurt_elite_memory_malloc(sizeof(vpm_session_object_t),
                                                                               QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_create_session_object(): Failed to allocate session object");

      return  ADSP_ENOMEMORY;
   }

   /* memset(0) the entire session object */
   memset(session_obj, 0, sizeof(vpm_session_object_t));

   /* Initialize the session object. */

   session_obj->direction = APR_UNDEFINED_ID_V;

   session_obj->widevoice_rx_sr = VPM_DEFAULT_RX_PP_SR;
   session_obj->public_enable = FALSE;
   session_obj->modem_enable = FALSE;
   session_obj->is_sr_set_by_client = FALSE;

   session_obj->is_kpps_changed = FALSE;

   session_obj->vptx_frame_size_us = 0;

   session_obj->sound_device_info.device_pair.direction = VPM_SOUND_DEVICE_DIRECTION_INVALID;

   /* Initialize the session and stream state machine control variables. */
   session_obj->state_ctrl.state = VPM_STATE_ENUM_RESET;
   session_obj->state_ctrl.goal = CVP_GOAL_ENUM_NONE;
   session_obj->state_ctrl.action = VPM_ACTION_ENUM_NONE;

   session_obj->active_set.tx_port_id = VSS_IVOCPROC_PORT_ID_NONE;
   session_obj->active_set.rx_port_id = VSS_IVOCPROC_PORT_ID_NONE;
   session_obj->active_set.tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
   session_obj->active_set.rx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
   session_obj->active_set.vol_step = 0;
   session_obj->active_set.client_num_vol_steps = 0;
   session_obj->active_set.client_rx_mute = VSS_IVOLUME_MUTE_OFF;
   session_obj->active_set.client_tx_mute = VSS_IVOLUME_MUTE_OFF;
   session_obj->active_set.mute_ramp_duration = 0;
   session_obj->active_set.system_config.network_id = VSS_ICOMMON_CAL_NETWORK_ID_NONE;
   session_obj->active_set.system_config.media_id = VSS_MEDIA_ID_NONE;
   session_obj->active_set.system_config.rx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
   session_obj->active_set.system_config.tx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
   session_obj->active_set.system_config.dec_sr = VPM_DEFAULT_DEC_SR;
   session_obj->active_set.system_config.enc_sr = VPM_DEFAULT_ENC_SR;
   session_obj->active_set.system_config.rx_pp_sr = VPM_DEFAULT_RX_PP_SR;
   session_obj->active_set.system_config.tx_pp_sr = VPM_DEFAULT_TX_PP_SR;
   session_obj->active_set.system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;
   session_obj->active_set.system_config.vsid = 0;
   session_obj->active_set.system_config.vfr_mode = VSS_ICOMMON_VFR_MODE_SOFT;

   session_obj->active_set.voice_timing.vsid = 0;
   session_obj->active_set.voice_timing.vfr_mode = VSS_ICOMMON_VFR_MODE_SOFT;
   session_obj->active_set.voice_timing.vfr_cycle_duration_us = 20000;

   session_obj->active_set.voice_timing.enc_offset = 0;
   session_obj->active_set.voice_timing.dec_req_offset = 0;
   session_obj->active_set.voice_timing.dec_offset = 0;
   session_obj->active_set.voice_timing.vprx_delivery_offset = 0;
   session_obj->active_set.voice_timing.vptx_buf_start_offset = 0;
   session_obj->active_set.voice_timing.vptx_proc_start_offset = 0;
   session_obj->active_set.voice_timing.vptx_delivery_offset = 0;

   session_obj->active_set.vocproc_mode = VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING;
   session_obj->active_set.ec_ref_port_id = VSS_IVOCPROC_PORT_ID_NONE;
   session_obj->active_set.rx_in_sample_rate = VPM_DEFAULT_RX_PP_SR;
   session_obj->active_set.num_dev_channels.tx_num_channels = 0;
   session_obj->active_set.num_dev_channels.rx_num_channels = 0;

   /* Initialize device channel info. */
	mmstd_memset(&session_obj->active_set.dev_chan_info.tx, 0,
                           sizeof(session_obj->active_set.dev_chan_info.tx) );
	mmstd_memset(&session_obj->active_set.dev_chan_info.rx, 0,
                           sizeof(session_obj->active_set.dev_chan_info.rx) );
	mmstd_memset(&session_obj->active_set.dev_chan_info.ec_ref, 0,
                           sizeof(session_obj->active_set.dev_chan_info.ec_ref) );

   session_obj->target_set.tx_port_id = VSS_IVOCPROC_PORT_ID_NONE;
   session_obj->target_set.rx_port_id = VSS_IVOCPROC_PORT_ID_NONE;
   session_obj->target_set.tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
   session_obj->target_set.rx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
   session_obj->target_set.vol_step = 0;
   session_obj->target_set.client_num_vol_steps = 0;
   session_obj->target_set.client_rx_mute = VSS_IVOLUME_MUTE_OFF;
   session_obj->target_set.client_tx_mute = VSS_IVOLUME_MUTE_OFF;
   session_obj->target_set.mute_ramp_duration = 0;
   session_obj->target_set.system_config.network_id = VSS_ICOMMON_CAL_NETWORK_ID_NONE;
   session_obj->target_set.system_config.media_id = VSS_MEDIA_ID_NONE;
   session_obj->target_set.system_config.rx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
   session_obj->target_set.system_config.tx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
   session_obj->target_set.system_config.dec_sr = VPM_DEFAULT_DEC_SR;
   session_obj->target_set.system_config.enc_sr = VPM_DEFAULT_ENC_SR;
   session_obj->target_set.system_config.rx_pp_sr = VPM_DEFAULT_RX_PP_SR;
   session_obj->target_set.system_config.tx_pp_sr = VPM_DEFAULT_TX_PP_SR;
   session_obj->target_set.system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;
   session_obj->target_set.system_config.vsid = 0;
   session_obj->target_set.system_config.vfr_mode = VSS_ICOMMON_VFR_MODE_SOFT;
   session_obj->target_set.system_config.call_num = 0;

   session_obj->target_set.voice_timing.vsid = 0;
   session_obj->target_set.voice_timing.vfr_mode = VSS_ICOMMON_VFR_MODE_SOFT;
   session_obj->target_set.voice_timing.vfr_cycle_duration_us = 20000;

   session_obj->target_set.voice_timing.enc_offset = 0;
   session_obj->target_set.voice_timing.dec_req_offset = 0;
   session_obj->target_set.voice_timing.dec_offset = 0;
   session_obj->target_set.voice_timing.vprx_delivery_offset = 0;
   session_obj->target_set.voice_timing.vptx_buf_start_offset = 0;
   session_obj->target_set.voice_timing.vptx_proc_start_offset = 0;
   session_obj->target_set.voice_timing.vptx_delivery_offset = 0;

   session_obj->target_set.vocproc_mode = VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING;
   session_obj->target_set.ec_ref_port_id = VSS_IVOCPROC_PORT_ID_NONE;
   session_obj->is_client_set_sectors = FALSE;
   session_obj->target_set.rx_in_sample_rate = VPM_DEFAULT_RX_PP_SR;

   session_obj->target_set.num_dev_channels.tx_num_channels = 0;
   session_obj->target_set.num_dev_channels.rx_num_channels = 0;

   session_obj->set_tx_dtmf_detect.enable_flag = VPM_DISABLED;

   session_obj->ref_cnt = 1;

   /* Initialize custom topology params. */
   session_obj->topo_commit_state = VPM_TOPOLOGY_COMMIT_STATE_NONE;
   session_obj->topo_param.is_registered = FALSE;

   /* Initialize device channel info. */
	mmstd_memset(&session_obj->target_set.dev_chan_info.tx, 0,
                           sizeof(session_obj->target_set.dev_chan_info.tx) );
	mmstd_memset(&session_obj->target_set.dev_chan_info.rx, 0,
                           sizeof(session_obj->target_set.dev_chan_info.rx) );
	mmstd_memset(&session_obj->target_set.dev_chan_info.ec_ref, 0,
                           sizeof(session_obj->target_set.dev_chan_info.ec_ref) );
   /* Initialize the attached stream list. */
   (void)apr_list_init_v2(&(session_obj->attached_stream_list), NULL, NULL);

   /* Complete initialization. */

   /* Mark the actual object type here to indicate that the object has been
    * fully initialized. Otherwise, on an error the destructor called at
    * the checkpoint handler would inadvertently try to free resources in
    * the object that have not been allocated yet or have already been
    * freed by the clean up sequence.
    */

   *ret_session_obj = session_obj;

   return ADSP_EOK;
}

ADSPResult vpm_construct_session(vpm_session_object_t **ret_session_obj,
                                 elite_msg_any_t *p_msg_pkt)
{
   ADSPResult              result = ADSP_EOK;
   uint32_t                network_id;
   uint32_t                rx_pp_sr;
   uint32_t                tx_pp_sr;
   vpm_session_object_t    *session_obj = NULL;
   elite_apr_packet_t      *apr_msg_pkt;
   char_t                  vpm_mvm_dns[] = "qcom.audio.mvm";

   vss_ivocproc_cmd_create_full_control_session_v3_t  *in_args;

   if (ADSP_FAILED(result = vpm_create_session_object(&session_obj)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_construct_session(): Failed to create session obj");

      goto _bail_out_vpm_construct_session;
   }

   apr_msg_pkt = (elite_apr_packet_t *)(p_msg_pkt->pPayload);

   /* Initialize full control client APR addr/port */
   session_obj->full_ctrl_client_addr = elite_apr_if_get_src_addr(apr_msg_pkt);
   session_obj->full_ctrl_client_port = elite_apr_if_get_src_port(apr_msg_pkt);

   /* Get the MVM APR address */
   if (ADSP_EOK != (result = elite_apr_if_lookup_addr_by_name(vpm_mvm_dns,
                                                              sizeof(vpm_mvm_dns),
                                                              &session_obj->mvm_apr_addr)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_construct_session(): Failed to get MVM apr address");

      goto _bail_out_vpm_construct_session;
   }

   in_args = (vss_ivocproc_cmd_create_full_control_session_v3_t *)elite_apr_if_get_payload_ptr(apr_msg_pkt);

   /* Set the target_set based on command paramters. */
   switch (in_args->profile_id)
   {
      case VSS_NETWORK_ID_DEFAULT:
      case VSS_NETWORK_ID_VOIP_NB:
      case VSS_NETWORK_ID_VOIP_WB:
      case VSS_NETWORK_ID_VOIP_WV:
      case VSS_NETWORK_ID_CDMA_NB:
      case VSS_NETWORK_ID_CDMA_WB:
      case VSS_NETWORK_ID_CDMA_WV:
      case VSS_NETWORK_ID_GSM_NB:
      case VSS_NETWORK_ID_GSM_WB:
      case VSS_NETWORK_ID_GSM_WV:
      case VSS_NETWORK_ID_WCDMA_NB:
      case VSS_NETWORK_ID_WCDMA_WB:
      case VSS_NETWORK_ID_WCDMA_WV:
      case VSS_NETWORK_ID_LTE_NB:
      case VSS_NETWORK_ID_LTE_WB:
      case VSS_NETWORK_ID_LTE_WV:
      {
         /* Convert the legacy network id to the new network_id.
          * (And validate the parameter in the process.)
          */
         if (ADSP_EOK != (result = vpm_convert_legacy_network_id(in_args->profile_id,
                                                                 &network_id,
                                                                 &rx_pp_sr,
                                                                 &tx_pp_sr)))
         {
            goto _bail_out_vpm_construct_session;
         }

         /* Save parameters in target set to be picked up by state machine. */
         session_obj->target_set.system_config.network_id = network_id;
         session_obj->target_set.system_config.rx_pp_sr = rx_pp_sr;
         session_obj->target_set.system_config.tx_pp_sr = tx_pp_sr;

         break;
      }
      case VSS_ICOMMON_CAL_NETWORK_ID_NONE:
      case VSS_ICOMMON_CAL_NETWORK_ID_CDMA:
      case VSS_ICOMMON_CAL_NETWORK_ID_GSM:
      case VSS_ICOMMON_CAL_NETWORK_ID_WCDMA:
      case VSS_ICOMMON_CAL_NETWORK_ID_VOIP:
      case VSS_ICOMMON_CAL_NETWORK_ID_LTE:
      {
         session_obj->target_set.system_config.network_id = in_args->profile_id;
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_construct_session(): Invalid profile ID 0x%08X",
               in_args->profile_id);

         goto _bail_out_vpm_construct_session;
      }
   } /* End of switch (in_args->profile_id) */

   switch (in_args->direction)
   {
      case VSS_IVOCPROC_DIRECTION_TX:
      {
         session_obj->target_set.tx_port_id = in_args->tx_port_id;
         session_obj->target_set.tx_topology_id = in_args->tx_topology_id;
         session_obj->target_set.rx_port_id = VSS_IVOCPROC_PORT_ID_NONE;
         session_obj->target_set.rx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
         session_obj->target_set.vocproc_mode = in_args->vocproc_mode;
         session_obj->target_set.ec_ref_port_id = in_args->ec_ref_port_id;

         { /*Temporary hack. Refer to is_valid_afe_port_provided for details. */
            if (session_obj->target_set.tx_port_id == VSS_IVOCPROC_PORT_ID_NONE)
            {
               session_obj->is_valid_afe_port_provided = FALSE;
               session_obj->target_set.tx_port_id = AFE_PORT_ID_PSEUDOPORT_01;
            }
            else
            {
               session_obj->is_valid_afe_port_provided = TRUE;
            }
         }
         break;
      }
      case VSS_IVOCPROC_DIRECTION_RX:
      {
         session_obj->target_set.tx_port_id = VSS_IVOCPROC_PORT_ID_NONE;
         session_obj->target_set.tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
         session_obj->target_set.rx_port_id = in_args->rx_port_id;
         session_obj->target_set.rx_topology_id = in_args->rx_topology_id;
         session_obj->target_set.vocproc_mode = in_args->vocproc_mode;
         session_obj->target_set.ec_ref_port_id = in_args->ec_ref_port_id;

         { /*Temporary hack. Refer to is_valid_afe_port_provided for details. */
            if (session_obj->target_set.rx_port_id == VSS_IVOCPROC_PORT_ID_NONE)
            {
               session_obj->is_valid_afe_port_provided = FALSE;
               session_obj->target_set.rx_port_id = AFE_PORT_ID_PSEUDOPORT_02;
            }
            else
            {
               session_obj->is_valid_afe_port_provided = TRUE;
            }
         }
         break;
      }
      case VSS_IVOCPROC_DIRECTION_RX_TX:
      {
         session_obj->target_set.tx_port_id = in_args->tx_port_id;
         session_obj->target_set.tx_topology_id = in_args->tx_topology_id;
         session_obj->target_set.rx_port_id = in_args->rx_port_id;
         session_obj->target_set.rx_topology_id = in_args->rx_topology_id;
         session_obj->target_set.vocproc_mode = in_args->vocproc_mode;
         session_obj->target_set.ec_ref_port_id = in_args->ec_ref_port_id;

         { /*Temporary hack. Refer to is_valid_afe_port_provided for details. */
            session_obj->is_valid_afe_port_provided = TRUE;

            if (session_obj->target_set.tx_port_id == VSS_IVOCPROC_PORT_ID_NONE)
            {
               session_obj->is_valid_afe_port_provided = FALSE;
               session_obj->target_set.tx_port_id = AFE_PORT_ID_PSEUDOPORT_01;
            }

            if (session_obj->target_set.rx_port_id == VSS_IVOCPROC_PORT_ID_NONE)
            {
               session_obj->is_valid_afe_port_provided = FALSE;
               session_obj->target_set.rx_port_id = AFE_PORT_ID_PSEUDOPORT_02;
            }
         }
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_construct_session(): Invalid vocproc direction: %lu",
               in_args->direction);

         goto _bail_out_vpm_construct_session;
      }
   } /*End of switch (in_args->direction) */

   /* Save the direction in session object */
   session_obj->direction = in_args->direction;

   /* Populuate the session pointer to return to caller */
   *ret_session_obj = session_obj;

   return ADSP_EOK;

_bail_out_vpm_construct_session:

   /* Set the output pointer to NULL */
   *ret_session_obj = NULL;

   /* Free up the allocated session object memory */
   if (session_obj)
   {
      qurt_elite_memory_free(session_obj);
   }

   return ADSP_EFAILED;
}

ADSPResult vpm_destroy_session(vpm_session_object_t *session_obj)
{
   apr_list_node_t *node = NULL;

   /* Clear attached streams list. */
   while (session_obj->attached_stream_list.size > 0)
   {
      (void)apr_list_get_next(&(session_obj->attached_stream_list),
                              &(session_obj->attached_stream_list.dummy),
                              &node);

      if (node)
      {
         (void)apr_list_delete(&(session_obj->attached_stream_list),
                               node);

         qurt_elite_memory_free(node);
         node = NULL;
      }
   }

   (void)apr_list_destroy(&session_obj->attached_stream_list);


   /* Remove the session object from VPM service session obj list */
   vpm_remove_session(p_vpm_svc_global_obj, session_obj);

   /* Free up the session object memory */
   qurt_elite_memory_free(session_obj);

   return ADSP_EOK;
}

/* Parse OOB topology parameters.
 */
ADSPResult vpm_extract_topology_params(vpm_session_object_t *session_obj,
                                       bool_t error_check_only_flag)
{
   void         *vir_addr;
   uint32_t     parsed_size = 0;
   uint32_t     mem_size;
   uint32_t     param_cnt = 0;

   vpm_create_param_t          *vpm_param_ptr;
   vpm_topology_param_entry_t  *topo_param_entry;

   if (NULL == session_obj)
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_extract_topology_params(): "\
             "session_obj ptr is NULL");

      return ADSP_EFAILED;
   }

   if ((FALSE == error_check_only_flag) &&
       (session_obj->topo_param.is_registered != TRUE))
   {
      MSG(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_extract_topology_params(): "\
             "OOB Topo params not registered");

      /* Extract in-band params if available */
      if (session_obj->target_set.num_dev_channels.tx_num_channels != 0)
      {
         session_obj->topo_param.vpm_param[0].param_id = VSS_PARAM_NUM_DEV_CHANNELS;
         session_obj->topo_param.vpm_param[0].param_size = sizeof(vss_param_num_dev_channels_t);
         session_obj->topo_param.vpm_param[0].param_virt_addr =
                                                                (uint32_t)(&(session_obj->target_set.num_dev_channels));
      }
      return ADSP_EOK;
   }

   vir_addr = session_obj->topo_param.vir_addr;
   mem_size = session_obj->topo_param.mem_size;

   do
   {
      topo_param_entry =\
         (vpm_topology_param_entry_t *)(((uint8_t *)vir_addr) + parsed_size);

      if (topo_param_entry == NULL)
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_extract_topology_params(): "\
                "topo_param Entry is NULL");

         return ADSP_EFAILED;
      }

      if (((((uint8_t *)topo_param_entry) + sizeof(vpm_topology_param_entry_t)) >
              (((uint8_t *)vir_addr) + mem_size)) ||
          ((((uint8_t *)topo_param_entry) + topo_param_entry->param_size) >
              (((uint8_t *)vir_addr) + mem_size)))
      {
         MSG_4(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_extract_topology_params(): "\
                  "Topology_param Entry out Virt Mem Bounds. "\
                  "entry_addr=0x%08x entry_size=0x%08x, virt_addr=0x%08x, virt_mem_size=0x%08x",
               topo_param_entry,
               topo_param_entry->param_size,
               vir_addr, mem_size);

         return ADSP_EFAILED;
      }

      switch (topo_param_entry->module_id)
      {
         case VSS_MODULE_CVD_GENERIC:
         {
            /* Extract Config Params. */
            switch (topo_param_entry->param_id)
            {
               case VSS_PARAM_NUM_DEV_CHANNELS:
               {
                  uint16_t num_mics;
                  uint16_t num_spkr_chans;
                  vss_param_num_dev_channels_t *params_ptr;

                  if (topo_param_entry->param_size != sizeof(vss_param_num_dev_channels_t))
                  {
                     /* In valid topology params. */
                     MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_extract_topology_params(): In valid param size.");
                     return ADSP_EBADPARAM;
                  }

                  params_ptr = (vss_param_num_dev_channels_t *)(((uint8_t *)&topo_param_entry->param_size) +
                                                                sizeof(vss_param_num_dev_channels_t));
                  if (params_ptr == NULL)
                  {
                     /* In valid topology params. */
                     MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_extract_topology_params(): In valid param address.");
                     return ADSP_EBADPARAM;
                  }

                  num_mics = params_ptr->tx_num_channels;
                  num_spkr_chans = params_ptr->rx_num_channels;

                  MSG_2(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_extract_topology_params(): #Mics %d, #Speaker Channels %d",
                        num_mics, num_spkr_chans);

                  /* Error check.*/
                  if ((num_mics < VSS_NUM_DEV_CHANNELS_1) ||
                      (num_mics > VSS_NUM_DEV_CHANNELS_4) ||
                      (num_spkr_chans != VSS_NUM_DEV_CHANNELS_1))
                  {
                     /* In valid topology params. */
                     MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                           "vpm_extract_topology_params(): In valid params: #Mics %d, #Speaker Channels %d",
                           num_mics, num_spkr_chans);

                     return ADSP_EBADPARAM;
                  }

                  if (error_check_only_flag)
                     break;

                  vpm_param_ptr = &session_obj->topo_param.vpm_param[param_cnt];
                  vpm_param_ptr->param_id = topo_param_entry->param_id;
                  vpm_param_ptr->param_size = topo_param_entry->param_size;

                  /* Apply in-band topology params if available. */
                  if (session_obj->target_set.num_dev_channels.tx_num_channels > 0)
                  {
                     vpm_param_ptr->param_virt_addr = (uint32_t)(&(session_obj->target_set.num_dev_channels));
                  }
                  else
                  {
                     vpm_param_ptr->param_virt_addr = (uint32_t)params_ptr;
                  }

                  session_obj->topo_param.vpm_param_size += sizeof(vpm_create_param_t);
                  param_cnt++;

                  break;
               }
               default:
               {
                  MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_extract_topology_params(): "\
                           "Invalid param_id 0x%08X", topo_param_entry->param_id);
                  break;
               }
            }
            break;
         }
         default:
         {
            MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_extract_topology_params(): "\
                     "Invalid mod_id 0x%08X",
                  topo_param_entry->module_id);
            break;
         }
      }

      parsed_size += (topo_param_entry->param_size + sizeof(voice_param_data_t));

   } while (parsed_size < mem_size);

   return ADSP_EOK;
}

bool_t vpm_is_dev_channel_info_modified(vpm_session_object_t *session_obj)
{
   vpm_active_settings_t *active_set = &(session_obj->active_set);
   vpm_target_settings_t *target_set = &(session_obj->target_set);

   if ((active_set->dev_chan_info.tx.num_channels != target_set->dev_chan_info.tx.num_channels) ||
       (active_set->dev_chan_info.tx.bits_per_sample != target_set->dev_chan_info.tx.bits_per_sample) ||
       (active_set->dev_chan_info.rx.num_channels != target_set->dev_chan_info.rx.num_channels) ||
       (active_set->dev_chan_info.rx.bits_per_sample != target_set->dev_chan_info.rx.bits_per_sample) ||
       (active_set->dev_chan_info.ec_ref.num_channels != target_set->dev_chan_info.ec_ref.num_channels) ||
       (active_set->dev_chan_info.ec_ref.bits_per_sample != target_set->dev_chan_info.ec_ref.bits_per_sample ))
   {
      return TRUE;
   }
   for (int i=0; i < VSS_MAX_DEV_CHANNELS; i++ ) 
   {
     if ( active_set->dev_chan_info.tx.channel_mapping[i] != target_set->dev_chan_info.tx.channel_mapping[i] )
     {
       return TRUE;
     }
     if ( active_set->dev_chan_info.rx.channel_mapping[i] != target_set->dev_chan_info.rx.channel_mapping[i] )
     {
       return TRUE;
     }
     if ( active_set->dev_chan_info.ec_ref.channel_mapping[i] != target_set->dev_chan_info.ec_ref.channel_mapping[i] )
     {
       return TRUE;
     }
   }
   return FALSE;
}

bool_t vpm_is_reinit_required(vpm_session_object_t *session_obj)
{
   vpm_active_settings_t *active_set = &(session_obj->active_set);
   vpm_target_settings_t *target_set = &(session_obj->target_set);

   /* If there is a sample rate change, port change or topology change,
      indicate reinit. */
   if ((active_set->system_config.tx_pp_sr != target_set->system_config.tx_pp_sr) ||
       (active_set->system_config.rx_pp_sr != target_set->system_config.rx_pp_sr) ||
       (active_set->tx_port_id != target_set->tx_port_id) ||
       (active_set->rx_port_id != target_set->rx_port_id) ||
       (active_set->tx_topology_id != target_set->tx_topology_id) ||
       (active_set->rx_topology_id != target_set->rx_topology_id) ||
       (active_set->vocproc_mode != target_set->vocproc_mode) ||
       (active_set->ec_ref_port_id != target_set->ec_ref_port_id)||
       (active_set->num_dev_channels.tx_num_channels != target_set->num_dev_channels.tx_num_channels) ||
       vpm_is_dev_channel_info_modified ( session_obj ) )
   {
      return TRUE;
   }
   return FALSE;
}

bool_t vpm_is_calibration_required ( 
  vpm_session_object_t *session_obj
)
{
  vpm_active_settings_t *active_set = &(session_obj->active_set);
  vpm_target_settings_t *target_set = &(session_obj->target_set);

  if ( ( active_set->system_config.network_id !=  target_set->system_config.network_id ) ||
       ( active_set->system_config.media_id != target_set->system_config.media_id ) ||
       ( active_set->system_config.rx_pp_sr != target_set->system_config.rx_pp_sr ) ||
       ( active_set->system_config.tx_pp_sr != target_set->system_config.tx_pp_sr ) ||
       ( active_set->system_config.rx_voc_op_mode != target_set->system_config.rx_voc_op_mode) ||
       ( active_set->system_config.tx_voc_op_mode != target_set->system_config.tx_voc_op_mode) ||
       ( active_set->system_config.feature != target_set->system_config.feature ) )
  {
    return TRUE;
  }

  return FALSE;
}

ADSPResult vpm_destroy_session_dyn_svc(vpm_session_object_t *p_session,
                                       bool_t destroy_vprx,
                                       bool_t destroy_vptx)
{
   ADSPResult           result = ADSP_EOK;
   vpm_dyn_svc_info_t   *p_dyn_svc = &p_session->dyn_svc_info;

   /* if vpRx present and need to be destroyed */
   if (p_dyn_svc->rx && destroy_vprx)
   {
      if (ADSP_EOK != (result = vpm_close_rx_mixer_output(p_session)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_destroy_session_dyn_svc(): Failed to close rx VMX output port");
      }

      /*if vpTx present, then disconnect vpRx <-> vpTx */
      if (p_dyn_svc->tx)
      {
         if (ADSP_EOK != (result |= vpm_connect_peer_svc(VOICEPROC_DISCONNECT_RX_PEER,
                                                         p_dyn_svc->tx->cmdQ,
                                                         p_session->session_rsp_ctrl.p_rsp_queue,
                                                         p_dyn_svc->rx)))
         {
            MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                "vpm_destroy_session_dyn_svc: VoiceProc Tx/Rx connection Failed");
         }
      }

      if (ADSP_EOK != (result |= vpm_destroy_dyn_svcs(p_dyn_svc->rx,
                                                      p_session->session_rsp_ctrl.p_rsp_queue)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_destroy_session_dyn_svc: Failed to destroy vpRx dyn svc");

      }
      else
      {
         p_dyn_svc->rx = NULL;
      }
   }

   /* if vpRx present and need to be destroyed */
   if (p_dyn_svc->tx && destroy_vptx)
   {
      if (ADSP_EOK != (result |= vpm_close_tx_mixer_input(p_session)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_destroy_session_dyn_svc: Failed to close Tx VMX input port");
      }

      if (ADSP_EOK != (result |= vpm_destroy_dyn_svcs(p_dyn_svc->tx,
                                                      p_session->session_rsp_ctrl.p_rsp_queue)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_destroy_session_dyn_svc: Failed to destroy vpTx dyn svc");

      }
      else
      {
         p_dyn_svc->tx = NULL;
      }
   }

   return result;
}


ADSPResult vpm_set_session_params(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   vss_ivocproc_cmd_set_device_v3_t *in_args;

   if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
   {
      /* Set device when the vocproc is running is not allowed. */
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_set_session_params(),  FAILED to set session params, VPM in RUN state");

      return ADSP_EFAILED;
   }

   in_args = (vss_ivocproc_cmd_set_device_v3_t *)elite_apr_if_get_payload_ptr(p_apr_packet);

   switch (session_obj->direction)
   {
      case VSS_IVOCPROC_DIRECTION_TX:
      {
         session_obj->target_set.tx_port_id = in_args->tx_port_id;
         session_obj->target_set.tx_topology_id = in_args->tx_topology_id;
         session_obj->target_set.vocproc_mode = in_args->vocproc_mode;
         session_obj->target_set.ec_ref_port_id = in_args->ec_ref_port_id;

         /*Temporary hack. Refer to is_valid_afe_port_provided for details. */
         if (VSS_IVOCPROC_PORT_ID_NONE == session_obj->target_set.tx_port_id)
         {
            session_obj->is_valid_afe_port_provided = FALSE;
            session_obj->target_set.tx_port_id = AFE_PORT_ID_PSEUDOPORT_01;
         }
         else
         {
            session_obj->is_valid_afe_port_provided = TRUE;
         }
         break;
      }
      case VSS_IVOCPROC_DIRECTION_RX:
      {
         session_obj->target_set.rx_port_id = in_args->rx_port_id;
         session_obj->target_set.rx_topology_id = in_args->rx_topology_id;
         session_obj->target_set.vocproc_mode = in_args->vocproc_mode;
         session_obj->target_set.ec_ref_port_id = in_args->ec_ref_port_id;

         /*Temporary hack. Refer to is_valid_afe_port_provided for details. */
         if (VSS_IVOCPROC_PORT_ID_NONE == session_obj->target_set.rx_port_id)
         {
            session_obj->is_valid_afe_port_provided = FALSE;
            session_obj->target_set.rx_port_id = AFE_PORT_ID_PSEUDOPORT_02;
         }
         else
         {
            session_obj->is_valid_afe_port_provided = TRUE;
         }
         break;
      }
      case VSS_IVOCPROC_DIRECTION_RX_TX:
      {
         session_obj->target_set.tx_port_id = in_args->tx_port_id;
         session_obj->target_set.tx_topology_id = in_args->tx_topology_id;
         session_obj->target_set.rx_port_id = in_args->rx_port_id;
         session_obj->target_set.rx_topology_id = in_args->rx_topology_id;
         session_obj->target_set.vocproc_mode = in_args->vocproc_mode;
         session_obj->target_set.ec_ref_port_id = in_args->ec_ref_port_id;

         /*Temporary hack. Refer to is_valid_afe_port_provided for details. */
         session_obj->is_valid_afe_port_provided = TRUE;

         if (VSS_IVOCPROC_PORT_ID_NONE == session_obj->target_set.tx_port_id)
         {
            session_obj->is_valid_afe_port_provided = FALSE;
            session_obj->target_set.tx_port_id = AFE_PORT_ID_PSEUDOPORT_01;
         }

         if (VSS_IVOCPROC_PORT_ID_NONE == session_obj->target_set.rx_port_id)
         {
            session_obj->is_valid_afe_port_provided = FALSE;
            session_obj->target_set.rx_port_id = AFE_PORT_ID_PSEUDOPORT_02;
         }
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_set_session_params(), Invalid VPM direction: %lu", session_obj->direction);

         result = ADSP_EUNEXPECTED;
         break;
      }
   } /* End of switch (session_obj->direction) */

   session_obj->target_set.ec_ref_port_id = vpm_get_ec_ref_port( session_obj );

   return result;
}

ADSPResult vpm_soundfocus_validate_angles(vss_isoundfocus_cmd_set_sectors_t *set_sectors)
{
   ADSPResult result = ADSP_EOK;
   int32_t     idx;

   if (!set_sectors)
   {
      return APR_EBADPARAM;
   }

   /* start_angles array has eight elements.
      Validate below conditions if the sectors are enabled:
      - Angle range is [0, 359].
      - Angles must be strictly increasing. */
   for (idx = 0; idx < 7; ++idx)
   {
      if ((set_sectors->enables[idx] != VSS_ISOUNDFOCUS_SECTOR_NOT_USED) &&
          (set_sectors->start_angles[idx] > 359))
      {
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_soundfocus_validate_angles(): bad param: "
               "start_angles[%d] = %d",
               idx, set_sectors->start_angles[idx]);

         result = ADSP_EBADPARAM;
         break;
      }

      if ((set_sectors->enables[idx + 1] != VSS_ISOUNDFOCUS_SECTOR_NOT_USED) &&
          (set_sectors->start_angles[idx] >= set_sectors->start_angles[idx + 1]))
      {
         MSG_4(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_soundfocus_validate_angles(): bad param: "
               "start_angles[%d] = %d, start_angles[%d] = %d",
               idx, set_sectors->start_angles[idx],
               idx + 1, set_sectors->start_angles[idx + 1]);

         result = APR_EBADPARAM;
         break;
      }
   }

   if ((set_sectors->enables[7] != VSS_ISOUNDFOCUS_SECTOR_NOT_USED) &&
       (set_sectors->start_angles[7] > 359))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_soundfocus_validate_angles(): bad param: "
            "start_angles[7] = %d ", set_sectors->start_angles[7]);

      result = ADSP_EBADPARAM;
   }

   return result;
}

ADSPResult vpm_create_full_session_v3_cmd_ctrl(voice_proc_mgr_t *pMe)
{
   ADSPResult              result = ADSP_EOK;
   vpm_session_object_t    *session_obj = NULL;
   uint16_t                client_addr;
   uint32_t                i, session_idx;
   elite_apr_packet_t      *p_apr_pkt;

   p_apr_pkt = (elite_apr_packet_t *)(pMe->cmd_ctrl.msg_pkt.pPayload);

   /* TODO: If a session with the same name as this one that already has
    * a full control client exists, reject this command. */

   if (ADSP_FAILED(result = vpm_get_free_session_idx(pMe, &session_idx)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_create_full_session_v3_cmd_ctrl(): Free session object not available");

      elite_apr_if_end_cmd(pMe->apr_handle, p_apr_pkt, result);
      return result;
   }

   if (ADSP_FAILED(result = vpm_construct_session(&session_obj, &pMe->cmd_ctrl.msg_pkt)))
   {
      elite_apr_if_end_cmd(pMe->apr_handle, p_apr_pkt, result);

      return result;
   }

   /* Save the session index */
   session_obj->session_id = session_idx;

   /* Save the svc APR handler/addr in session object */
   session_obj->self_apr_handle = pMe->apr_handle;
   session_obj->self_apr_addr = pMe->apr_addr;
   session_obj->self_apr_port = VPM_GET_APR_PORT_ID(session_idx);

   /* Save the cmd/resp queue in session object for interaction with dynamic service */
   session_obj->session_cmd_ctrl.p_cmd_queue = pMe->session_static_info[session_idx].p_session_cmd_q;
   session_obj->session_rsp_ctrl.p_rsp_queue = pMe->session_static_info[session_idx].p_session_resp_Q;

   /* Update the packet->dst_port to the new session handle. */
   elite_apr_if_set_dst_port(p_apr_pkt, session_obj->self_apr_port);

   session_obj->topo_commit_state = VPM_TOPOLOGY_COMMIT_STATE_CREATE;

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
   session_obj->is_clk_overridden = FALSE;

   /* Add this session to service object */
   vpm_add_session(pMe, session_obj, session_idx);

   /* End APR command */
   elite_apr_if_end_cmd(pMe->apr_handle, p_apr_pkt, ADSP_EOK);

   MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_create_full_session_v3_cmd_ctrl(): Successfully allocated session object, session_id: 0x%lx",
         session_obj->self_apr_port);

   return result;
}

ADSPResult vpm_create_full_session_v2_cmd_ctrl(voice_proc_mgr_t *pMe)
{
   ADSPResult              result = ADSP_EOK;
   vpm_session_object_t    *session_obj;
   uint32_t                i, session_idx;
   elite_apr_packet_t      *p_apr_pkt;

   p_apr_pkt = (elite_apr_packet_t *)(pMe->cmd_ctrl.msg_pkt.pPayload);

   if (ADSP_FAILED(result = vpm_get_free_session_idx(pMe, &session_idx)))
   {
      MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
          "vpm_create_full_session_v2_cmd_ctrl(): Free session object not available");

      elite_apr_if_end_cmd(pMe->apr_handle, p_apr_pkt, result);
      return result;
   }

   if (ADSP_FAILED(result = vpm_construct_session(&session_obj, &pMe->cmd_ctrl.msg_pkt)))
   {
      elite_apr_if_end_cmd(pMe->apr_handle, p_apr_pkt, result);

      return result;
   }

   /* Save the session index */
   session_obj->session_id = session_idx;

   /* Save the svc APR handler/addr in session object */
   session_obj->self_apr_handle = pMe->apr_handle;
   session_obj->self_apr_addr = pMe->apr_addr;
   session_obj->self_apr_port = VPM_GET_APR_PORT_ID(session_idx);

   /* Save the cmd/resp queue in session object for interaction with dynamic services */
   session_obj->session_cmd_ctrl.p_cmd_queue = pMe->session_static_info[session_idx].p_session_cmd_q;
   session_obj->session_rsp_ctrl.p_rsp_queue = pMe->session_static_info[session_idx].p_session_resp_Q;

   /* Update the packet->dst_port to the new session handle. */
   elite_apr_if_set_dst_port(p_apr_pkt, session_obj->self_apr_port);

   /* Create next goal for VPM create */
   vpm_create_next_goal_ctrl(session_obj, CVP_GOAL_ENUM_CREATE);

   result = vpm_state_control(session_obj);

   /* Wait until the job is done. */
   if (session_obj->state_ctrl.is_goal_completed)
   {
#ifndef WINSIM
      /* NOTE: The shared memory related actions fails in SIM mode causing
       *       CVP creation failure. We opt not to check for shared memory
       *       errors now since the PC test code currently has no support for
       *       shared memory operations nor remote shared memory operations.
       */

      result = session_obj->state_ctrl.status;

      /* If the operation failed */
      if (ADSP_EOK != result)
      {
         /* Return the failure to client */
         elite_apr_if_end_cmd(pMe->apr_handle, p_apr_pkt, result);

         /* Destroy the session */
         vpm_destroy_session(session_obj);

         MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_create_full_session_v2_cmd_ctrl(): Failed to create session, result: 0x%lx", result);
      }
      else /* Success */
#endif /* !WINSIM */
      {
         /* Add session object to service session list */
         vpm_add_session(pMe, session_obj, session_idx);

         /* End APR command */
         elite_apr_if_end_cmd(pMe->apr_handle, p_apr_pkt, result);

         MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_create_full_session_v2_cmd_ctrl(): Successfully created session, session_id: 0x%lx",
               session_obj->self_apr_port);
      }
   }

   return result;
}

ADSPResult vpm_create_full_session_cmd_ctrl(voice_proc_mgr_t *pMe)
{
   ADSPResult           result = ADSP_EOK;
   uint32_t             payload_size;
   uint32_t             session_name_size;
   uint8_t              *p_new_packet_payload;
   uint32_t             new_packet_payload_size;
   uint32_t             new_packet_payload_left;
   elite_apr_packet_t   *new_packet;
   elite_apr_packet_t   *curr_packet;
   int32_t              rc;

   vss_ivocproc_cmd_create_full_control_session_t        *in_args;
   vss_ivocproc_cmd_create_full_control_session_v2_t     create_session_v2;

   curr_packet = (elite_apr_packet_t *)pMe->cmd_ctrl.msg_pkt.pPayload;

   /* Get the payload size */
   payload_size = elite_apr_if_get_payload_size(curr_packet);

   if (payload_size < sizeof(vss_ivocproc_cmd_create_full_control_session_t))
   {
      MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_create_full_session_cmd_ctrl(): Unexpected payload size, "
            "%d < %d", payload_size,
            sizeof(vss_ivocproc_cmd_create_full_control_session_t));

      /* End APR command */
      elite_apr_if_end_cmd(pMe->apr_handle, curr_packet, ADSP_EBADPARAM);

      return ADSP_EBADPARAM;
   }

   /* Get the pointer to the payload from APR packet */
   in_args = (vss_ivocproc_cmd_create_full_control_session_t *)elite_apr_if_get_payload_ptr(curr_packet);

   session_name_size = (payload_size - sizeof(*in_args));

   /* Note that we are calling vpm_create_full_session_v2_cmd_ctrl in order
    * to leverage code. We create a new command packet and replace the
    * current command packet in the pending control with the new packet. The
    * new packet contains:
    * 1. vss_ivocproc_cmd_create_full_control_session_v2_t arguments (with the
    *    client specified vocproc properties and with vocproc_mode and
    *    ec_ref_port_id set to default values), and the session name provided
    *    by the client.
    * 2. The opcode for the new packet is the original packet's opcode (i.e.
    *    VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION), so that when
    *    vss_ivocproc_cmd_create_full_control_session_v2_t finishes processing
    *    the packet, we are using the correct opcode when sending a response
    *    back to the client.
    */

   create_session_v2.direction = in_args->direction;
   create_session_v2.tx_port_id = ((uint16_t)in_args->tx_port_id);
   create_session_v2.tx_topology_id = in_args->tx_topology_id;
   create_session_v2.rx_port_id = ((uint16_t)in_args->rx_port_id);
   create_session_v2.rx_topology_id = in_args->rx_topology_id;
   create_session_v2.profile_id = in_args->network_id;
   create_session_v2.vocproc_mode = VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING;
   create_session_v2.ec_ref_port_id = VSS_IVOCPROC_PORT_ID_NONE;

   new_packet_payload_size = (sizeof(create_session_v2) + session_name_size);

   if (APR_EOK != (rc = elite_apr_if_alloc_cmd(pMe->apr_handle,
                                               elite_apr_if_get_src_addr(curr_packet),
                                               elite_apr_if_get_src_port(curr_packet),
                                               elite_apr_if_get_dst_addr(curr_packet),
                                               elite_apr_if_get_dst_port(curr_packet),
                                               elite_apr_if_get_client_token(curr_packet),
                                               elite_apr_if_get_opcode(curr_packet),
                                               new_packet_payload_size,
                                               &new_packet)))
   {
      MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
            "vpm_create_full_session_cmd_ctrl(): Failed to allocate new APR packet for v2 cmd,"
            "result: 0x%lx", rc);

      /* End APR command */
      elite_apr_if_end_cmd(pMe->apr_handle, curr_packet, rc);

      return rc;
   }

   /* Get the new APR packet payload pointer and size */
   p_new_packet_payload = (uint8_t *)elite_apr_if_get_payload_ptr(new_packet);
   new_packet_payload_left = elite_apr_if_get_payload_size(new_packet);

   memscpy(p_new_packet_payload,
           new_packet_payload_left,
           &create_session_v2,
           sizeof(create_session_v2));

   new_packet_payload_left -= sizeof(create_session_v2);

   if (session_name_size > 0)
   {
      memscpy(p_new_packet_payload + sizeof(create_session_v2),
              new_packet_payload_left,
              (((uint8_t *)in_args) + sizeof(*in_args)),
              session_name_size);
   }

   /* Free-up current APR packet */
   elite_apr_if_free(pMe->apr_handle, curr_packet);

   /* Update the APR cmd packet with newly allocated one */
   pMe->cmd_ctrl.msg_pkt.pPayload = (void *)new_packet;

   return vpm_create_full_session_v2_cmd_ctrl(pMe);
}

ADSPResult vpm_topology_commit_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_pkt;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   /* Check if this is new command or previously pending command is being
       executed */
   if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
   {
      MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
            "vpm_topology_commit_cmd_ctrl(): topo_commit_state: %lu", session_obj->topo_commit_state);

      if (ADSP_EOK != (result = vpm_extract_topology_params(session_obj, FALSE)))
      {
         MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_topology_commit_cmd_ctrl(): Invalid topology_params");

         elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

         return result;
      }

      if ((VPM_TOPOLOGY_COMMIT_STATE_CREATE == session_obj->topo_commit_state) &&
          (VPM_STATE_ENUM_RESET == session_obj->state_ctrl.state))
      {
         /* Create next goal for VPM create */
         vpm_create_next_goal_ctrl(session_obj, CVP_GOAL_ENUM_CREATE);
      }
      else /* If commit is for set_device() */
      {
         if ((VPM_TOPOLOGY_COMMIT_STATE_SET_DEVICE == session_obj->topo_commit_state) &&
             (VPM_STATE_ENUM_IDLE == session_obj->state_ctrl.state))
         {
            if (TRUE == vpm_is_reinit_required(session_obj))
            {
               /* Create next goal for VPM reinit */
               vpm_create_next_goal_ctrl(session_obj, CVP_GOAL_ENUM_REINIT);
            }
            else
            {
               MSG(MSG_SSID_DFLT, DBG_MED_PRIO,
                   "vpm_topology_commit_cmd_ctrl(): Re-init not needed");

               session_obj->topo_commit_state = VPM_TOPOLOGY_COMMIT_STATE_NONE;

               elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_EOK);

               return ADSP_EOK;
            }
         }
         else /* Invalid state */
         {
            MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                  "vpm_topology_commit_cmd_ctrl(): Invalid state %d, topo_commit_state = %d",
                  session_obj->session_cmd_ctrl.status,
                  session_obj->topo_commit_state);

            if (session_obj->topo_commit_state == VPM_TOPOLOGY_COMMIT_STATE_SET_DEVICE)
            {
               session_obj->topo_commit_state = VPM_TOPOLOGY_COMMIT_STATE_NONE;
            }

            elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_EFAILED);

            return ADSP_EFAILED;
         }
      }
   } /* if !(is_prev_cmd_pending) */

   /* Call the state control */
   result = vpm_state_control(session_obj);

   if (session_obj->topo_commit_state == VPM_TOPOLOGY_COMMIT_STATE_SET_DEVICE)
   {
      /* If goal completed with ADSP_EOK status */
      if (session_obj->state_ctrl.is_goal_completed &&
          ADSP_EOK == session_obj->state_ctrl.status)
      {
         session_obj->is_vocproc_config_changed = TRUE;
         session_obj->topo_commit_state = VPM_TOPOLOGY_COMMIT_STATE_NONE;
      }
   }
   else
   {
      /* Wait until the job is done. */
      if (session_obj->state_ctrl.is_goal_completed)
      {
         /* NOTE: The shared memory related actions fails in SIM mode causing
          *       CVP creation failure. We opt not to check for shared memory
          *       errors now since the PC test code currently has no support for
          *       shared memory operations nor remote shared memory operations.
          */

         if (ADSP_EOK != session_obj->state_ctrl.status)
         {
            /* Destroy VPM session */
            vpm_destroy_session(session_obj);

            session_obj->state_ctrl.state = VPM_STATE_ENUM_RESET;

            result = ADSP_EFAILED;
         }
         else /*Goal completed successfully */
         {
            session_obj->topo_commit_state = VPM_TOPOLOGY_COMMIT_STATE_NONE;
         }
      }
   }

   MSG_2(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "vpm_topology_commit_cmd_ctrl(): Cmd completed, goal_id: 0x%lx, result: 0x%lx",
         session_obj->state_ctrl.goal,
         session_obj->session_cmd_ctrl.status);

   elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, session_obj->state_ctrl.status);

   return result;
}

ADSPResult vpm_destroy_session_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult           result = ADSP_EOK;
   elite_apr_packet_t   *p_apr_packet = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   /* Check if session creation is complete or not */
   if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
   {
      /* Free up the session object resources */
      vpm_destroy_session(session_obj);

      /* End the APR command*/
      elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_packet, ADSP_EOK);

      return result;
   }

   if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
   {
      if ((VPM_STATE_ENUM_IDLE == session_obj->state_ctrl.state) &&
          (0 == session_obj->attached_mvm_handle))
      {
         /* Make sure disable and detach is called before creating
             the destroy goal.
             The check is only done once because disable/detach/destroy
             commands are processed in the same thread. In future, if they
             are separated into different threads, the check may be moved
             out and then incompleted goal shall be cleared.
         */

         vpm_create_next_goal_ctrl(session_obj, CVP_GOAL_ENUM_DESTROY);
      }
      else
      {
         /* Fail the destroy Command if the VPM state machine is in RUN state
          * or VPM is still attached to MVM.
          * This guarantees that destroy command is not processed while the session
          * object resources may be accessed by other commands (from other threads)
          */
         MSG_2(MSG_SSID_DFLT, DBG_ERROR_PRIO,
               "vpm_destroy_session_cmd_ctrl(): FAILED, either VPM in run/reset state(vpm_state: %d), "
               "and /or MVM is still attached (attached_mvm_handle: 0x%04x)",
               session_obj->state_ctrl.state, session_obj->attached_mvm_handle);

         elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_packet, ADSP_EFAILED);

         return ADSP_EFAILED;
      }
   } /* if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending) */

   /* Destroy Command is not processed until
    * session_obj is not accessed by any other command. */

   /**
    * TODO: On destroy session failure we need to keep the session object
    *       around because we need to continue the reset process. Aborted
    *       CVD session objects need to be queued somewhere for later
    *       destruction. We still have to run the session state control on the
    *       session object until the RESET is complete.
    *
    *       The current handling assumes the destroy operation from the
    *       lower layers are always successful.
    */

   /**
    * Run the CVD session state machine. The CVD session state machine should
    * only need to run when there are pending commands to process.
    */
   result = vpm_state_control(session_obj);

   /* Check the goal completion status. If completed, end the APR command,
       else keep the cmd in pending status until completion.
       Session cmdQ will not be popped until cmd completion */

   vpm_check_goal_completion_status(session_obj);

   if (TRUE == session_obj->state_ctrl.is_goal_completed)
   {
      result = vpm_destroy_session(session_obj);
   }

   MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,
         "vpm_destroy_session_cmd_ctrl(): destroy result: 0x%lx", result);

   return result;
}

ADSPResult vpm_enable_cmd_ctrl(vpm_session_object_t *session_obj,
                               bool_t modem_enable)
{
   ADSPResult              result = ADSP_EOK;
   vpm_cmd_ctrl_t          *p_cmd_ctrl = &session_obj->session_cmd_ctrl;
   elite_apr_packet_t      *p_apr_pkt;
   int32_t                 rc;

   for (;;)
   {
      p_apr_pkt = (elite_apr_packet_t *)p_cmd_ctrl->msg_pkt.pPayload;

      /* Check if session creation is complete or not */
      if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
      {
         /* End the APR command*/
         break;
      }

      /* Temporary hack. Refer to is_valid_afe_port_provided for details. */
      if ( FALSE == session_obj->is_valid_afe_port_provided )
      {
        MSG( MSG_SSID_DFLT, DBG_ERROR_PRIO,
             "vpm_enable_cmd_ctrl(): INVALID AFE PORTS!!");
        result = ADSP_EFAILED;
        break;
      }

      if (FALSE == p_cmd_ctrl->is_prev_cmd_pending)
      {
         if ( modem_enable )
         {
            session_obj->modem_enable = TRUE;
         }
         else
         {
            session_obj->public_enable = TRUE;
         }

         /* The vocproc will be enabled under the following scenerios:
          * 1. This vocproc is not attached to MVM, and
          *    client enables the vocproc.
          * 2. This vocproc is attached to MVM. Both the client and MVM has issued
          *    enable command to this vocproc. There is no configuration change
          *    ever since the last time when VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG
          *    has been processed.
          */
         if ( ( session_obj->public_enable ) && ( session_obj->modem_enable ) )
         {
            if ( ( session_obj->attached_mvm_handle != APR_NULL_V ) &&
                 ( TRUE == session_obj->is_vocproc_config_changed ) )
            {
              rc = elite_apr_if_alloc_send_event(
                     session_obj->self_apr_handle,
                     session_obj->self_apr_addr, session_obj->self_apr_port,
                     session_obj->mvm_apr_addr, session_obj->attached_mvm_handle,
                     0, VSS_IVOCPROC_EVT_RECONFIG, NULL, 0 );
               if ( APR_EOK != rc )
               {
                  MSG_1( MSG_SSID_DFLT, DBG_ERROR_PRIO,
                        "vpm_enable_cmd_ctrl(): Failed to send reconfig evt to "
                        "MVM, result: 0x%lx", rc );

                  result = rc;
               }

               MSG( MSG_SSID_DFLT, DBG_HIGH_PRIO,
                    "vpm_enable_cmd_ctrl(): VPM config changed, sent reconfig evt to MVM");
               break;
            }

            /* Control fall through if ENABLE can be processed */
            vpm_create_next_goal_ctrl(session_obj, CVP_GOAL_ENUM_ENABLE);
         }
         else
         {
           MSG_2( MSG_SSID_DFLT, DBG_HIGH_PRIO,
                  "vpm_enable_cmd_ctrl(): Public enable(%u) or modem enable(%u) "
                  "is not done", session_obj->public_enable, session_obj->modem_enable);
           result = ADSP_EOK;
           break;
         }
      } /* end of if (FALSE == p_cmd_ctrl->is_prev_cmd_pending) */


      /* Perform actions to complete goal set above */
      result = vpm_state_control(session_obj);

      /* Check the goal completion status. If completed, end the APR command,
       else keep the cmd in pending status until completion.
       Session cmdQ will not be popped until cmd completion */
      vpm_check_goal_completion_status(session_obj);

      if (TRUE == session_obj->state_ctrl.is_goal_completed)
      {
         MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
               "vpm_enable_cmd_ctrl(): Cmd completed with result : 0x%lx",
               session_obj->state_ctrl.status);
      }

      return result;

   } /* End of for (;;) */

   /* If any error occured or no action required */
   elite_apr_if_end_cmd(session_obj->self_apr_addr, p_apr_pkt, result);

   return result;
}

ADSPResult vpm_disable_cmd_ctrl(
  vpm_session_object_t *session_obj,
  bool_t modem_disable
)
{
   ADSPResult            result = ADSP_EOK;
   elite_apr_packet_t    *p_apr_pkt;

   p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   if (modem_disable)
   {
      session_obj->modem_enable = FALSE;
   }
   else
   {
      session_obj->public_enable = FALSE;
   }

   if (FALSE == session_obj->session_cmd_ctrl.is_prev_cmd_pending)
   {
     if (VPM_STATE_ENUM_RUN == session_obj->state_ctrl.state)
     {
        vpm_create_next_goal_ctrl(session_obj, CVP_GOAL_ENUM_DISABLE);
     }
     else /* If reset/idle state, do nothing */
     {
       MSG_1( MSG_SSID_DFLT, DBG_MED_PRIO,
              "vpm_disable_cmd_ctrl(): VPM not in RUN state, no action taken, "
              "vpm_state: 0x%lx", session_obj->state_ctrl.state );

       elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, ADSP_EOK);
       return ADSP_EOK;
     }
   }

   result = vpm_state_control(session_obj);

   /* Check the goal completion status. If completed, end the APR command,
       else keep the cmd in pending status until completion.
       Session cmdQ will not be popped until cmd completion */
   vpm_check_goal_completion_status(session_obj);

   if (TRUE == session_obj->state_ctrl.is_goal_completed)
   {
      MSG_1( MSG_SSID_DFLT, DBG_MED_PRIO,
             "vpm_disable_cmd_ctrl(): Cmd completed with result : 0x%lx",
             session_obj->state_ctrl.status );
   }

   return result;
}

ADSPResult vpm_reinit_cmd_ctrl(vpm_session_object_t *session_obj)
{
   ADSPResult result = ADSP_EOK;

   elite_apr_packet_t *p_apr_pkt = (elite_apr_packet_t *)session_obj->session_cmd_ctrl.msg_pkt.pPayload;

   /* Check if session creation is complete or not */
   if (ADSP_EOK != (result = vpm_check_topo_commit_state(session_obj)))
   {
      /* End the APR command*/
      elite_apr_if_end_cmd(session_obj->self_apr_handle, p_apr_pkt, result);

      return result;
   }

   /* Force re-init dynamic services */
   result = vpm_reinit_dyn_svcs(session_obj, TRUE);

   /* End the APR command */
   elite_apr_if_end_cmd(session_obj->self_apr_addr,
                        p_apr_pkt, result);

   MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO,
         "vpm_reinit_cmd_ctrl(): Cmd completed with result : 0x%lx",
         result);

   return result;
}

ADSPResult vpm_parse_generic_module_id_params_and_set_param(vpm_session_object_t *session_obj,
                                                            void *payload_address,
                                                            uint32_t payload_size,
                                                            bool_t *is_generic_params)
{
   ADSPResult result = ADSP_EOK;
	uint16_t  byte_size_counter  = 0;
   voice_param_data_t   *voice_common_set_get_param_ptr = NULL;   
   vss_param_port_endpoint_media_info_t  *port_media_info_ptr = NULL;
   uint8_t  *payload_address_int = (uint8_t *)payload_address;
   vpm_port_media_info_t  *session_obj_afe_port_media_info_ptr = NULL;
   vss_param_vocproc_dev_channel_info_t  *params_ptr = NULL;

					  
	*is_generic_params = FALSE;
   do
   {
      voice_common_set_get_param_ptr = (voice_param_data_t *)payload_address_int;
      if ( NULL == voice_common_set_get_param_ptr )
      {
        result |= ADSP_EUNEXPECTED;
        break;
      }

      switch(voice_common_set_get_param_ptr->module_id)
      {
         case VSS_MODULE_CVD_GENERIC:
         {
			*is_generic_params = TRUE;
            port_media_info_ptr = (vss_param_port_endpoint_media_info_t *)((int8_t *)voice_common_set_get_param_ptr + sizeof(voice_param_data_t));
			params_ptr = (vss_param_vocproc_dev_channel_info_t  *)((int8_t *)voice_common_set_get_param_ptr + sizeof(voice_param_data_t));
            switch(voice_common_set_get_param_ptr->param_id)
            {
               case VSS_PARAM_TX_PORT_ENDPOINT_MEDIA_INFO:
               {                  
                  session_obj_afe_port_media_info_ptr = &(session_obj->tx_port_media_info);                  
               }  
               break;
               case VSS_PARAM_RX_PORT_ENDPOINT_MEDIA_INFO:                     
               {                  
                  session_obj_afe_port_media_info_ptr = &(session_obj->rx_port_media_info);                  
               }
               break;
               case VSS_PARAM_EC_REF_PORT_ENDPOINT_MEDIA_INFO:
               {                  
                  session_obj_afe_port_media_info_ptr = &(session_obj->ec_ref_port_media_info);                  
               }
               break;
               case VSS_PARAM_VOCPROC_TX_CHANNEL_INFO:
               {
                 if (voice_common_set_get_param_ptr->param_size != sizeof(vss_param_vocproc_dev_channel_info_t ))
                 {
                   /* Invalid topology params. */
                   MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param(): Invalid param size.");
                   return ADSP_EBADPARAM;
                 }
                 if (params_ptr == NULL)
                 {
                   /* Invalid topology params. */
                   MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param(): Invalid param address.");
                   return ADSP_EBADPARAM;
                 }

                 MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_parse_generic_module_id_params_and_set_param(): #Mic Channels %d",
                                                    params_ptr->num_channels );

                 /* Error check.*/
                 if ( ( params_ptr->num_channels < VSS_NUM_DEV_CHANNELS_1 ) || 
                      ( params_ptr->num_channels > VSS_MAX_DEV_CHANNELS ) )
                 {
                   /* Invalid topology params. */
                   MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                   "vpm_parse_generic_module_id_params_and_set_param(): Invalid params: #Mic Channels %d ",
                   params_ptr->num_channels );
                   break;
                 }
                 /* Copy the channel info. */
                 mmstd_memcpy( &session_obj->target_set.dev_chan_info.tx, sizeof(session_obj->target_set.dev_chan_info.tx), 
                              params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ) );
               }
               break;
               case VSS_PARAM_VOCPROC_RX_CHANNEL_INFO:
               {
                 if (voice_common_set_get_param_ptr->param_size != sizeof(vss_param_vocproc_dev_channel_info_t ))
                 {
                   /* Invalid topology params. */
                   MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param(): Invalid param size.");
                   return ADSP_EBADPARAM;
                 }
                 if (params_ptr == NULL)
                 {
                   /* Invalid topology params. */
                   MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param(): Invalid param address.");
                   return ADSP_EBADPARAM;
                 }

                 MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_parse_generic_module_id_params_and_set_param(): #Speaker Channels %d",
                                                    params_ptr->num_channels );

                 /* Error check.*/
                 if ( ( params_ptr->num_channels < VSS_NUM_DEV_CHANNELS_1 ) || 
                      ( params_ptr->num_channels > VSS_MAX_DEV_CHANNELS ) )
                 {
                   /* Invalid topology params. */
                   MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                   "vpm_parse_generic_module_id_params_and_set_param(): Invalid params: #speaker Channels %d ",
                   params_ptr->num_channels );
                   break;
                 }
                 /* Copy the channel info. */
                 mmstd_memcpy( &session_obj->target_set.dev_chan_info.rx, sizeof(session_obj->target_set.dev_chan_info.rx), 
                              params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ) );
               }
               break;
               case VSS_PARAM_VOCPROC_EC_REF_CHANNEL_INFO:
               {
                 if (voice_common_set_get_param_ptr->param_size != sizeof(vss_param_vocproc_dev_channel_info_t ))
                 {
                   /* Invalid topology params. */
                   MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param(): Invalid param size.");
                   return ADSP_EBADPARAM;
                 }
                 if (params_ptr == NULL)
                 {
                   /* Invalid topology params. */
                   MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param(): Invalid param address.");
                   return ADSP_EBADPARAM;
                 }

                 MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_parse_generic_module_id_params_and_set_param(): #EC_REF Channels %d",
                                                    params_ptr->num_channels );

                 /* Error check.*/
                 if ( ( params_ptr->num_channels < VSS_NUM_DEV_CHANNELS_1 ) || 
                      ( params_ptr->num_channels > VSS_MAX_DEV_CHANNELS ) )
                 {
                   /* Invalid topology params. */
                   MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                         "vpm_parse_generic_module_id_params_and_set_param(): Invalid params: #ec ref Channels %d ",
                         params_ptr->num_channels );
                   break;
                 }
                 
                 /* Copy the channel info. */
                 mmstd_memcpy( &session_obj->target_set.dev_chan_info.ec_ref, sizeof(session_obj->target_set.dev_chan_info.ec_ref), 
                                params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ) );
               }
               break;
               default:
               {
                  result |= ADSP_EBADPARAM;
                  MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,"vpm_parse_generic_module_id_params_and_set_param(): Invalid param sent: 0x%lx",
                                                      voice_common_set_get_param_ptr->param_id);           
               }
               break;
            }
            if(NULL != session_obj_afe_port_media_info_ptr)
            {
               memscpy((void *)session_obj_afe_port_media_info_ptr,sizeof(vpm_port_media_info_t),(void *)port_media_info_ptr,sizeof(vss_param_port_endpoint_media_info_t)); 
            }
         }
         break;
         default:
         {            
            MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,"vpm_parse_generic_module_id_params_and_set_param(): Invalid module sent: 0x%lx",
                                                      voice_common_set_get_param_ptr->module_id);
         }
         break;
      }
      byte_size_counter += (sizeof(voice_param_data_t) + voice_common_set_get_param_ptr->param_size);
      payload_address_int += (sizeof(voice_param_data_t) + voice_common_set_get_param_ptr->param_size);
   }while(byte_size_counter < payload_size);  
   return result;
}

ADSPResult vpm_parse_generic_module_id_params_and_set_param_v2(vpm_session_object_t *session_obj,
                                                               void *payload_address,
                                                               uint32_t payload_size,
                                                               bool_t *is_generic_params)
{
   ADSPResult result = ADSP_EOK;
   uint16_t  byte_size_counter  = 0;
   voice_param_data_v2_t   *voice_common_set_get_param_ptr = NULL;   
   vss_param_port_endpoint_media_info_t  *port_media_info_ptr = NULL;
   uint8_t  *payload_address_int = (uint8_t *)payload_address;
   vpm_port_media_info_t  *session_obj_afe_port_media_info_ptr = NULL;
   vss_param_vocproc_dev_channel_info_t  *params_ptr = NULL;
                 
   *is_generic_params = FALSE;
   do
   {
      voice_common_set_get_param_ptr = (voice_param_data_v2_t *)payload_address_int;
      if ( NULL == voice_common_set_get_param_ptr )
      {
        result |= ADSP_EUNEXPECTED;
        break;
      }

      switch (voice_common_set_get_param_ptr->module_id)
      {
         case VSS_MODULE_CVD_GENERIC:
         {
			   *is_generic_params = TRUE;
            port_media_info_ptr = (vss_param_port_endpoint_media_info_t *)((int8_t *)voice_common_set_get_param_ptr + sizeof(voice_param_data_v2_t));
  			   params_ptr = (vss_param_vocproc_dev_channel_info_t  *)((int8_t *)voice_common_set_get_param_ptr + sizeof(voice_param_data_v2_t));
            switch(voice_common_set_get_param_ptr->param_id)
            {
               case VSS_PARAM_TX_PORT_ENDPOINT_MEDIA_INFO:
               {                  
                  session_obj_afe_port_media_info_ptr = &(session_obj->tx_port_media_info);                  
               }  
               break;
               case VSS_PARAM_RX_PORT_ENDPOINT_MEDIA_INFO:                     
               {                  
                  session_obj_afe_port_media_info_ptr = &(session_obj->rx_port_media_info);                  
               }
               break;
               case VSS_PARAM_EC_REF_PORT_ENDPOINT_MEDIA_INFO:
               {                  
                  session_obj_afe_port_media_info_ptr = &(session_obj->ec_ref_port_media_info);                  
               }
               break;
               case VSS_PARAM_VOCPROC_TX_CHANNEL_INFO:
               {
						if (voice_common_set_get_param_ptr->param_size != sizeof(vss_param_vocproc_dev_channel_info_t ))
                  {
                     /* Invalid topology params. */
                     MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param_v2(): Invalid param size.");
                     return ADSP_EBADPARAM;
                  }
                  if (params_ptr == NULL)
                  {
                     /* Invalid topology params. */
                     MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param_v2(): Invalid param address.");
                     return ADSP_EBADPARAM;
                  }


                  MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_parse_generic_module_id_params_and_set_param_v2(): #Mic Channels %d",
                        params_ptr->num_channels );

                  /* Error check.*/
                  if ( ( params_ptr->num_channels < VSS_NUM_DEV_CHANNELS_1 ) || 
                       ( params_ptr->num_channels > VSS_MAX_DEV_CHANNELS ) )
                  {
                     /* Invalid topology params. */
                     MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                           "vpm_parse_generic_module_id_params_and_set_param_v2(): Invalid params: #Mic Channels %d ",
                           params_ptr->num_channels );
                     break;
                  }

                  /* Copy dev channel info. */
                  mmstd_memcpy( &session_obj->target_set.dev_chan_info.tx, sizeof(session_obj->target_set.dev_chan_info.tx), 
                                 params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ) );
               }
               break;
               case VSS_PARAM_VOCPROC_RX_CHANNEL_INFO:
               {
						if (voice_common_set_get_param_ptr->param_size != sizeof(vss_param_vocproc_dev_channel_info_t ))
                  {
                     /* Invalid topology params. */
                     MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param_v2(): Invalid param size.");
                     return ADSP_EBADPARAM;
                  }
                  if (params_ptr == NULL)
                  {
                     /* Invalid topology params. */
                     MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param_v2(): Invalid param address.");
                     return ADSP_EBADPARAM;
                  }


                  MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_parse_generic_module_id_params_and_set_param_v2(): #speaker Channels %d",
                        params_ptr->num_channels );

                  /* Error check.*/
                  if ( ( params_ptr->num_channels < VSS_NUM_DEV_CHANNELS_1 ) || 
                       ( params_ptr->num_channels > VSS_MAX_DEV_CHANNELS ) )
                  {
                     /* Invalid topology params. */
                     MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                           "vpm_parse_generic_module_id_params_and_set_param_v2(): Invalid params: #speaker Channels %d ",
                           params_ptr->num_channels );
                     break;
                  }

                  /* Copy dev channel info. */
                  mmstd_memcpy( &session_obj->target_set.dev_chan_info.rx, sizeof(session_obj->target_set.dev_chan_info.rx), 
                                 params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ) );
               }
               break;
               case VSS_PARAM_VOCPROC_EC_REF_CHANNEL_INFO:
               {
						if (voice_common_set_get_param_ptr->param_size != sizeof(vss_param_vocproc_dev_channel_info_t ))
                  {
                     /* Invalid topology params. */
                     MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param_v2(): Invalid param size.");
                     return ADSP_EBADPARAM;
                  }
                  if (params_ptr == NULL)
                  {
                     /* Invalid topology params. */
                     MSG(MSG_SSID_DFLT, DBG_ERROR_PRIO, "vpm_parse_generic_module_id_params_and_set_param_v2(): Invalid param address.");
                     return ADSP_EBADPARAM;
                  }


                  MSG_1(MSG_SSID_DFLT, DBG_MED_PRIO, "vpm_parse_generic_module_id_params_and_set_param_v2(): #EC_REF Channels %d",
                        params_ptr->num_channels );

                  /* Error check.*/
                  if ( ( params_ptr->num_channels < VSS_NUM_DEV_CHANNELS_1 ) || 
                       ( params_ptr->num_channels > VSS_MAX_DEV_CHANNELS ) )
                  {
                     /* Invalid topology params. */
                     MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,
                           "vpm_parse_generic_module_id_params_and_set_param_v2(): Invalid params: #ec ref Channels %d ",
                           params_ptr->num_channels );
                     break;
                  }

                  /* Copy dev channel info. */
                  mmstd_memcpy( &session_obj->target_set.dev_chan_info.ec_ref, sizeof(session_obj->target_set.dev_chan_info.ec_ref), 
                                   params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ) );
               }
               break;
               default:
               {
                  result |= ADSP_EBADPARAM;
                  MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,"vpm_parse_generic_module_id_params_and_set_param(): Invalid param sent: 0x%lx",
                                                      voice_common_set_get_param_ptr->param_id);           
               }
               break;
            }
            if(NULL != session_obj_afe_port_media_info_ptr)
            {
               memscpy((void *)session_obj_afe_port_media_info_ptr,sizeof(vpm_port_media_info_t),(void *)port_media_info_ptr,sizeof(vss_param_port_endpoint_media_info_t)); 
            }
         }
         break;
         default:
         {            
            MSG_1(MSG_SSID_DFLT, DBG_HIGH_PRIO,"vpm_parse_generic_module_id_params_and_set_param(): Invalid module sent: 0x%lx",
                                                      voice_common_set_get_param_ptr->module_id);
         }
         break;
      }
      byte_size_counter += (sizeof(voice_param_data_v2_t) + voice_common_set_get_param_ptr->param_size);
      payload_address_int += (sizeof(voice_param_data_v2_t) + voice_common_set_get_param_ptr->param_size);
   }while(byte_size_counter < payload_size);  

   return result;
}

ADSPResult vpm_parse_generic_module_id_params_and_get_param(vpm_session_object_t *session_obj,
                                                        void *payload_address,
                                                        uint32_t payload_size,
                                                        bool_t *is_generic_params)
{
   ADSPResult result = ADSP_EOK;
   voice_param_data_t   *voice_common_set_get_param_ptr = NULL;      
   vss_param_port_endpoint_media_info_t  *port_media_info_ptr = NULL;
   voice_common_set_get_param_ptr = (voice_param_data_t *)payload_address;
   vpm_port_media_info_t  *session_obj_afe_port_media_info_ptr = NULL;
   vss_param_vocproc_dev_channel_info_t *params_ptr = NULL;

   *is_generic_params = FALSE;
   switch(voice_common_set_get_param_ptr->module_id)
   {
      case VSS_MODULE_CVD_GENERIC:
      {
		 *is_generic_params = TRUE;  
		 params_ptr = (vss_param_vocproc_dev_channel_info_t *)((int8_t *)voice_common_set_get_param_ptr + sizeof(voice_param_data_t));
     port_media_info_ptr = (vss_param_port_endpoint_media_info_t *)((int8_t *)voice_common_set_get_param_ptr + sizeof(voice_param_data_t));              
         switch(voice_common_set_get_param_ptr->param_id)
         {
            case VSS_PARAM_TX_PORT_ENDPOINT_MEDIA_INFO:
            {    
               session_obj_afe_port_media_info_ptr = &(session_obj->tx_port_media_info);     
            }  
            break;
            case VSS_PARAM_RX_PORT_ENDPOINT_MEDIA_INFO:                     
            {                  
               session_obj_afe_port_media_info_ptr = &(session_obj->rx_port_media_info);                  
            }
            break;
            case VSS_PARAM_EC_REF_PORT_ENDPOINT_MEDIA_INFO:
            {                  
               session_obj_afe_port_media_info_ptr = &(session_obj->ec_ref_port_media_info);                  
            }
            break;
            case VSS_PARAM_VOCPROC_TX_CHANNEL_INFO:
            {
              /* Copy dev channel info. */
              mmstd_memcpy( params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ),
                            &session_obj->target_set.dev_chan_info.tx, 
                            sizeof(session_obj->target_set.dev_chan_info.tx) );
              voice_common_set_get_param_ptr->param_size = sizeof( vss_param_vocproc_dev_channel_info_t );
            }
            break;
            case VSS_PARAM_VOCPROC_RX_CHANNEL_INFO:
            {
              /* Copy dev channel info. */
              mmstd_memcpy( params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ),
                            &session_obj->target_set.dev_chan_info.rx, 
                            sizeof(session_obj->target_set.dev_chan_info.rx) );
              voice_common_set_get_param_ptr->param_size = sizeof( vss_param_vocproc_dev_channel_info_t );
            }
            break;
            case VSS_PARAM_VOCPROC_EC_REF_CHANNEL_INFO:
            {
              /* Copy dev channel info. */
              mmstd_memcpy( params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ),
                            &session_obj->target_set.dev_chan_info.ec_ref, 
                            sizeof(session_obj->target_set.dev_chan_info.ec_ref) );
              voice_common_set_get_param_ptr->param_size = sizeof( vss_param_vocproc_dev_channel_info_t );
            }
            break;
            default:
            {
               result |= ADSP_EBADPARAM;
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,"vpm_parse_generic_module_id_params_and_get_param(): Invalid param sent: 0x%lx",
                                                   voice_common_set_get_param_ptr->param_id);           
            }
            break;
         }
         if(NULL != session_obj_afe_port_media_info_ptr)
         {
		   memscpy((void *)port_media_info_ptr,sizeof(vss_param_port_endpoint_media_info_t),(void *)session_obj_afe_port_media_info_ptr,sizeof(vpm_port_media_info_t)); 
           voice_common_set_get_param_ptr->param_size = sizeof(vss_param_port_endpoint_media_info_t);
         }
      }
      break;
      default:
      {
         result |= ADSP_EUNSUPPORTED;
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,"vpm_parse_generic_module_id_params_and_get_param(): Invalid module sent: 0x%lx",
                                                   voice_common_set_get_param_ptr->module_id);
      }
      break;
   }
   return result;
}

ADSPResult vpm_parse_generic_module_id_params_and_get_param_v2(vpm_session_object_t *session_obj,
                                                        void *payload_address,
                                                        uint32_t payload_size,
                                                        bool_t *is_generic_params)
{
   ADSPResult result = ADSP_EOK;
   voice_param_data_v2_t   *voice_common_set_get_param_ptr = NULL;      
   vss_param_port_endpoint_media_info_t  *port_media_info_ptr = NULL;
   voice_common_set_get_param_ptr = (voice_param_data_v2_t *)payload_address;
   vpm_port_media_info_t  *session_obj_afe_port_media_info_ptr = NULL;
   vss_param_vocproc_dev_channel_info_t  *params_ptr = NULL;
				  
   *is_generic_params = FALSE;
   switch(voice_common_set_get_param_ptr->module_id)
   {
      case VSS_MODULE_CVD_GENERIC:
      {
         *is_generic_params = TRUE;
         port_media_info_ptr = (vss_param_port_endpoint_media_info_t *)((int8_t *)voice_common_set_get_param_ptr + sizeof(voice_param_data_v2_t));
         params_ptr = (vss_param_vocproc_dev_channel_info_t *)((int8_t *)voice_common_set_get_param_ptr + sizeof(voice_param_data_v2_t));
         switch(voice_common_set_get_param_ptr->param_id)
         {
            case VSS_PARAM_TX_PORT_ENDPOINT_MEDIA_INFO:
            {                  
               session_obj_afe_port_media_info_ptr = &(session_obj->tx_port_media_info);                  
            }  
            break;
            case VSS_PARAM_RX_PORT_ENDPOINT_MEDIA_INFO:                     
            {                  
               session_obj_afe_port_media_info_ptr = &(session_obj->rx_port_media_info);                  
            }
            break;
            case VSS_PARAM_EC_REF_PORT_ENDPOINT_MEDIA_INFO:
            {                  
               session_obj_afe_port_media_info_ptr = &(session_obj->ec_ref_port_media_info);                  
            }
            break;
            case VSS_PARAM_VOCPROC_TX_CHANNEL_INFO:
            {
              /* Copy dev channel info. */
              mmstd_memcpy( params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ),
                            &session_obj->target_set.dev_chan_info.tx, 
                            sizeof(session_obj->target_set.dev_chan_info.tx) );
              voice_common_set_get_param_ptr->param_size = sizeof( vss_param_vocproc_dev_channel_info_t );
            }
            break;
            case VSS_PARAM_VOCPROC_RX_CHANNEL_INFO:
            {
              /* Copy dev channel info. */
              mmstd_memcpy( params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ),
                            &session_obj->target_set.dev_chan_info.rx, 
                            sizeof(session_obj->target_set.dev_chan_info.rx) );
              voice_common_set_get_param_ptr->param_size = sizeof( vss_param_vocproc_dev_channel_info_t );
            }
            break;
            case VSS_PARAM_VOCPROC_EC_REF_CHANNEL_INFO:
            {
              /* Copy dev channel info. */
              mmstd_memcpy( params_ptr,  sizeof(vss_param_vocproc_dev_channel_info_t ),
                            &session_obj->target_set.dev_chan_info.ec_ref, 
                            sizeof(session_obj->target_set.dev_chan_info.ec_ref) );
              voice_common_set_get_param_ptr->param_size = sizeof( vss_param_vocproc_dev_channel_info_t );
            }
            break;
            default:
            {
               result |= ADSP_EBADPARAM;
               MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,"vpm_parse_generic_module_id_params_and_get_param(): Invalid param sent: 0x%lx",
                                                   voice_common_set_get_param_ptr->param_id);           
            }
            break;
         }
         if(NULL != session_obj_afe_port_media_info_ptr)
         {
            memscpy((void *)port_media_info_ptr,sizeof(vss_param_port_endpoint_media_info_t),
						  (void *)session_obj_afe_port_media_info_ptr,sizeof(vpm_port_media_info_t)); 
			voice_common_set_get_param_ptr->param_size = sizeof(vss_param_port_endpoint_media_info_t);
         }
      }
      break;
      default:
      {
         result |= ADSP_EUNSUPPORTED;
         MSG_1(MSG_SSID_DFLT, DBG_ERROR_PRIO,"vpm_parse_generic_module_id_params_and_get_param(): Invalid module sent: 0x%lx",
                                                   voice_common_set_get_param_ptr->module_id);
      }
      break;
   }
   return result;
}
