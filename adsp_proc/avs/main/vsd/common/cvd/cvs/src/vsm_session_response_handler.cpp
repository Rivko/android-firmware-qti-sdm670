/*
  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvs/src/vsm_session_response_handler.cpp#1 $
  $Author: pwbldsvc $
*/

#include "qurt_elite.h"

#include "vsm_header_i.h"
#include "vsm_common_i.h"
#include "VoiceDec.h"
#include "VoiceMixerSvc.h"
#include "VoiceCmnUtils.h"
#include "venc_svc.h"
#include "VoiceDec.h"
#include "EliteMsg_AfeCustom.h"
#include "cvd_mem_mapper_i.h"
#include "VoiceLoggingUtils.h"

extern elite_apr_handle_t vsm_apr_handle;
extern uint16_t vsm_mvm_addr;

#if 0
ADSPResult VSM_session_respQ_connect_handler( vsm_session_object_t* session_object )
{

  VSM_utility_default_response_handler( session_object );

  /* free the incoming response */
  elite_msg_release_msg( &session_object->cmd_control.resp_msg );

  return ADSP_EOK;

}
#endif
/************************ Utilities **********************
*******************************************************/

static bool_t VSM_utility_voc_op_mode_is_valid (
  uint32_t voc_op_mode
)
{
  bool_t rc = FALSE;

  switch ( voc_op_mode )
  {
  case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE:
  case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NB:
  case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_WB:
  case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_SWB:
  case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_FB:
    rc = TRUE;
    break;

  default:
    rc = FALSE;
    break;
  }

  return rc;
}

static ADSPResult VSM_utility_voc_op_mode_event_handler(
  vsm_session_object_t* session_object,
  elite_apr_packet_t *apr_pkt
)
{
  ADSPResult rc = ADSP_EOK;
  vsm_evt_voc_operating_mode_update_t* payload;
  vss_istream_evt_voc_operating_mode_update_t voc_op_mode_update;
  uint32_t payload_len;
  bool_t is_update_mvm_required = FALSE;
  bool_t is_notify_eamr_mode_changed = FALSE;
  bool_t is_notify_evs_bandwidth_changed = FALSE;

  if ( ( NULL == session_object ) || ( NULL == apr_pkt ) )
  {
    return ADSP_EBADPARAM;
  }

  payload = APRV2_PKT_GET_PAYLOAD( vsm_evt_voc_operating_mode_update_t, apr_pkt );
  payload_len = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

  if ( payload_len != sizeof( vsm_evt_voc_operating_mode_update_t ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_utility_voc_op_mode_event_handler(): " \
                                            "Invalid payload len: %d, expected len: %d, Drop evt",
                                            payload_len,
                                            sizeof( vsm_evt_voc_operating_mode_update_t ) );
    return ADSP_EBADPARAM;
  }

  if ( VSM_utility_voc_op_mode_is_valid( payload->mode ) == FALSE )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_utility_voc_op_mode_event_handler(): " \
                                            "Invalid vocoder operating mode 0x%08X.",
                                            payload->mode );
    return ADSP_EBADPARAM;
  }

  if ( ( payload->direction != VSM_VOC_OPERATING_MODE_DIRECTION_TX ) &&
       ( payload->direction != VSM_VOC_OPERATING_MODE_DIRECTION_RX ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VSM_utility_voc_op_mode_event_handler(): " \
                                            "Invalid direction 0x%04X .", payload->direction );
    return ADSP_EBADPARAM;
  }

  MSG_5( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VSM: EVENT_VOCODER_OPERATING_MODE: direction = (%d), mode = (0x%08x), " \
         "session_direction %d, resp_result = 0x%X, resp_pending = %d", 
         payload->direction, payload->mode, session_object->direction,
         session_object->cmd_control.resp_result, 
         session_object->cmd_control.is_resp_pending );

  switch ( session_object->direction )
  {
    case VSM_DIRECTION_RX:
    {
      if ( payload->direction == VSM_VOC_OPERATING_MODE_DIRECTION_RX )
      {
        session_object->voc_operating_mode_info.is_rx_mode_received = TRUE;
        session_object->voc_operating_mode_info.rx_mode = payload->mode;
        is_update_mvm_required = TRUE;
      }
    }
    break;

    case VSM_DIRECTION_TX:
    {
      if ( payload->direction == VSM_VOC_OPERATING_MODE_DIRECTION_TX )
      {
        session_object->voc_operating_mode_info.is_tx_mode_received = TRUE;
        session_object->voc_operating_mode_info.tx_mode = payload->mode;
        is_update_mvm_required = TRUE;
      }
    }
    break;

    case VSM_DIRECTION_RX_TX:
    {
      if ( payload->direction == VSM_VOC_OPERATING_MODE_DIRECTION_RX )
      {
        session_object->voc_operating_mode_info.is_rx_mode_received = TRUE;
        session_object->voc_operating_mode_info.rx_mode = payload->mode;
      }

      if ( payload->direction == VSM_VOC_OPERATING_MODE_DIRECTION_TX )
      {
        session_object->voc_operating_mode_info.is_tx_mode_received = TRUE;
        session_object->voc_operating_mode_info.tx_mode = payload->mode;
      }

      if ( ( session_object->voc_operating_mode_info.is_rx_mode_received == TRUE ) &&
           ( session_object->voc_operating_mode_info.is_tx_mode_received == TRUE ) )
      {
        is_update_mvm_required = TRUE;
      }
    }
    break;

    default:
    {
      rc = ADSP_EUNEXPECTED;
    }
    break;
  }

  if ( ( is_update_mvm_required == TRUE ) &&
       ( session_object->attached_mvm_handle != APR_NULL_V ) )
  { /* Send VSS_ISTREAM_EVT_VOC_OPERATING_MODE_UPDATE event to MVM. */
    voc_op_mode_update.rx_mode = session_object->voc_operating_mode_info.rx_mode;
    voc_op_mode_update.tx_mode = session_object->voc_operating_mode_info.tx_mode;

    rc = elite_apr_if_alloc_send_event(
           vsm_apr_handle,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           vsm_mvm_addr, session_object->attached_mvm_handle,
           0, VSS_ISTREAM_EVT_VOC_OPERATING_MODE_UPDATE,
           &voc_op_mode_update, sizeof( voc_op_mode_update ) );
    VSM_COMM_ERROR( rc, vsm_mvm_addr );
  }

  /* Send eAMR mode change event to registered client if the mode is changed.
   * Refer to the comments under eamr_mode_change_notification_info in
   * vsm_session_object_t for details. */
  if ( VSS_MEDIA_ID_EAMR == session_object->active_set.media_id )
  {
    switch ( session_object->voc_operating_mode_info.rx_mode )
    {
    case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NB:
      {
        if ( VSS_ISTREAM_EAMR_MODE_WIDEBAND ==
              session_object->eamr_mode_change_notification_info.mode )
        {
          is_notify_eamr_mode_changed = TRUE;
          session_object->eamr_mode_change_notification_info.mode = 
            VSS_ISTREAM_EAMR_MODE_NARROWBAND;
        }
      }
      break;

    case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_WB:
      {
        if ( VSS_ISTREAM_EAMR_MODE_NARROWBAND == 
             session_object->eamr_mode_change_notification_info.mode )
        {
          is_notify_eamr_mode_changed = TRUE;

          session_object->eamr_mode_change_notification_info.mode = 
            VSS_ISTREAM_EAMR_MODE_WIDEBAND;
        }
      }
      break;

    default:
      break;
    }

    if ( ( is_notify_eamr_mode_changed == TRUE ) &&
         ( VSM_STATE_ENUM_RUN == session_object->state_control.state ) )
    {
      ( void ) vsm_action_send_eamr_mode_event( session_object );
    }
  }

  /* Send EVS bandwidth change event to registered client if the mode is changed.
   * Refer to the comments under evs_bandwidth_change_notification_info in
   * vsm_session_object_t for details. */
  if ( VSS_MEDIA_ID_EVS == session_object->active_set.media_id )
  {
    switch ( session_object->voc_operating_mode_info.rx_mode )
    {
    case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NB:
      {
        if ( VSS_ISTREAM_EVS_VOC_BANDWIDTH_NB != 
               session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth )
        {
          is_notify_evs_bandwidth_changed = TRUE;
          session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth = 
            VSS_ISTREAM_EVS_VOC_BANDWIDTH_NB;
        }
      }
      break;

    case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_WB:
      {
        if ( VSS_ISTREAM_EVS_VOC_BANDWIDTH_WB !=
               session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth )
        {
          is_notify_evs_bandwidth_changed = TRUE;
          session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth = 
            VSS_ISTREAM_EVS_VOC_BANDWIDTH_WB;
        }
      }
      break;

    case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_SWB:
      {
        if ( VSS_ISTREAM_EVS_VOC_BANDWIDTH_SWB !=
               session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth )
        {
          is_notify_evs_bandwidth_changed = TRUE;

          session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth = 
            VSS_ISTREAM_EVS_VOC_BANDWIDTH_SWB;
        }
      }
      break;

      case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_FB:
        {
          if ( VSS_ISTREAM_EVS_VOC_BANDWIDTH_FB != 
                 session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth )
          {
            is_notify_evs_bandwidth_changed = TRUE;

            session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth = 
              VSS_ISTREAM_EVS_VOC_BANDWIDTH_FB;
          }
        }
        break;
        
      default:
        break;
    }

    /** If VSM is not in RUN state these notification will be sent during IDLE->RUN
     * transition. Refer to the comments under evs_bandwidth_change_notification_info
     * in vsm_session_object_t for details. */
    if ( ( TRUE == is_notify_evs_bandwidth_changed ) &&
         ( VSM_STATE_ENUM_RUN == session_object->state_control.state ) )
    {
      ( void ) vsm_action_publish_evs_params( session_object );
      ( void ) vsm_action_send_evs_bandwidth_event( session_object );
    }
  }

  rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
  VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );

  return ADSP_EOK;
}

static ADSPResult VSM_utility_send_dtmf_generation_ended_event_to_client(
  vsm_session_object_t* session_object,
  elite_apr_packet_t *apr_pkt
)
{
  ADSPResult rc = ADSP_EOK;

  if ( ( NULL == session_object ) || ( NULL == apr_pkt ) )
  {
    return ADSP_EBADPARAM;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_send_dtmf_generation_ended_event_to_client:"
         " direction %d", APRV2_PKT_GET_PAYLOAD( vss_istream_evt_dtmf_generation_ended_t, apr_pkt )->direction );

  apr_pkt->dst_addr = session_object->set_dtmf_gen.client_addr;
  apr_pkt->dst_port = session_object->set_dtmf_gen.client_port;
  rc = __aprv2_cmd_forward( vsm_apr_handle, apr_pkt );
  VSM_COMM_ERROR( rc, session_object->set_dtmf_gen.client_addr );

  return ADSP_EOK;
}

static ADSPResult VSM_utility_send_rx_dtmf_detect_event_to_client(
  vsm_session_object_t* session_object,
  elite_apr_packet_t *apr_pkt
)
{
  ADSPResult rc = ADSP_EOK;
  

  if ( ( NULL == session_object ) || ( NULL == apr_pkt ) )
  {
    return ADSP_EBADPARAM;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_send_rx_dtmf_detect_event_to_client:"
         " received low_freq %d  high_freq %d",
         APRV2_PKT_GET_PAYLOAD( vss_istream_evt_rx_dtmf_detected_t, apr_pkt )->low_freq,
         APRV2_PKT_GET_PAYLOAD( vss_istream_evt_rx_dtmf_detected_t, apr_pkt )->high_freq );

  apr_pkt->dst_addr = session_object->set_rx_dtmf_detect.client_addr;
  apr_pkt->dst_port = session_object->set_rx_dtmf_detect.client_port;
  rc = __aprv2_cmd_forward( vsm_apr_handle, apr_pkt );
  VSM_COMM_ERROR( rc, session_object->set_rx_dtmf_detect.client_addr );

  return ADSP_EOK;
}

static ADSPResult VSM_utility_attach_detach_stream_apr_response_handler(
  vsm_session_object_t* session_object,
  elite_apr_packet_t *apr_pkt
)
{
  ADSPResult rc = ADSP_EOK;

  if ( ( NULL == session_object ) || ( NULL == apr_pkt ) )
  {
    return ADSP_EBADPARAM;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_attach_detach_stream_apr_response_handler:"
         " received response token 0x%X  result 0x%X", apr_pkt->token, ADSP_EOK );

  session_object->cmd_control.resp_count++;
  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;
  }

  switch ( apr_pkt->opcode )
  {
#if 0
    case VSS_IVOCPROC_RSP_ATTACH_STREAM:
    {
      session_object->attached_vpm_session_handle =
        APRV2_PKT_GET_PAYLOAD( vss_ivocproc_rsp_attach_stream_t, apr_pkt )->vdsp_session_handle;
      session_object->attached_vpm_session_direction = 
        APRV2_PKT_GET_PAYLOAD( vss_ivocproc_rsp_attach_stream_t, apr_pkt )->direction;
    }
    break;

    case VSS_IVOCPROC_RSP_DETACH_STREAM:
    {
      session_object->attached_vpm_session_handle =
        APRV2_PKT_GET_PAYLOAD( vss_ivocproc_rsp_detach_stream_t, apr_pkt )->vdsp_session_handle;
      session_object->attached_vpm_session_direction = 
        APRV2_PKT_GET_PAYLOAD( vss_ivocproc_rsp_detach_stream_t, apr_pkt )->direction;
    }
    break;
#else

    case VSS_IVOCPROC_RSP_ATTACH_STREAM:
    case VSS_IVOCPROC_RSP_DETACH_STREAM:
    {
      session_object->attached_vpm_outport_mapping = 
        APRV2_PKT_GET_PAYLOAD( vss_ivocproc_rsp_attach_detach_stream_t, apr_pkt )->mixer_outport_mask;
      session_object->attached_vpm_inport_mapping = 
        APRV2_PKT_GET_PAYLOAD( vss_ivocproc_rsp_attach_detach_stream_t, apr_pkt )->mixer_inport_mask;
      session_object->attached_vpm_session_direction = 
        APRV2_PKT_GET_PAYLOAD( vss_ivocproc_rsp_attach_detach_stream_t, apr_pkt )->direction;
    }
    break;

#endif
  }

  session_object->cmd_control.resp_result = ADSP_EOK;

  rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
  VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );

  return ADSP_EOK;

}

static ADSPResult VSM_utility_set_system_config_apr_response_handler(
  vsm_session_object_t* session_object,
  elite_apr_packet_t *apr_pkt
)
{
  ADSPResult rc = ADSP_EOK;


  if ( ( NULL == session_object ) || ( NULL == apr_pkt ) )
  {
    return ADSP_EBADPARAM;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_set_system_config_apr_response_handler: received response token 0x%X ",
           apr_pkt->token );

  if ( TRUE == session_object->cmd_control.is_resp_pending )
  {
    session_object->cmd_control.resp_count++;
  }

  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;
  }
  session_object->cmd_control.resp_result = ADSP_EOK;

  rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
  VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );

  return ADSP_EOK;
}

static ADSPResult VSM_utility_default_apr_response_handler(
  vsm_session_object_t* session_object,
  elite_apr_packet_t *apr_pkt
)
{
  ADSPResult rc = ADSP_EOK;

  if ( ( NULL == session_object ) || ( NULL == apr_pkt ) )
  {
    return ADSP_EBADPARAM;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_default_apr_response_handler: received response token 0x%X  result 0x%X",
           apr_pkt->token, APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, apr_pkt )->status );

  if ( TRUE == session_object->cmd_control.is_resp_pending )
  {
    session_object->cmd_control.resp_count++;
  }

  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;
    if ( VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2 == ( APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, apr_pkt )->opcode ) )
    {
      session_object->active_set.playback.vpm_port_id = apr_pkt->src_port;
    }
  }

  if ( session_object->cmd_control.resp_result == 0xFFFFFFFF )
  {
    session_object->cmd_control.resp_result = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, apr_pkt )->status;;
  }

  if ( ADSP_FAILED( APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, apr_pkt )->status ) )
  {
    session_object->cmd_control.resp_result = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, apr_pkt )->status;
  }

  rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
  VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );

  return ADSP_EOK;
}

static ADSPResult VSM_utility_set_oob_pktexg_config_response_handler( vsm_session_object_t* session_object )
{
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_msg_any_payload_t *param_payload_ptr = ( elite_msg_any_payload_t* )session_object->cmd_control.resp_msg.pPayload;

  elite_msg_custom_oob_pkt_exchange_config_type* pktexcg_payload = ( elite_msg_custom_oob_pkt_exchange_config_type* )session_object->cmd_control.resp_msg.pPayload;
  vsm_config_packet_exchange_t* config = ( vsm_config_packet_exchange_t* )pktexcg_payload->param_data_ptr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_set_oob_pktexg_config_response_handler: received response token 0x%X  result 0x%X",
           param_payload_ptr->unClientToken, param_payload_ptr->unResponseResult );

  session_object->cmd_control.resp_count++;
  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;
    qurt_elite_memory_free( ( void* )config );
  }

  if ( session_object->cmd_control.resp_result == 0xFFFFFFFF )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  if ( ADSP_FAILED( param_payload_ptr->unResponseResult ) )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  return ADSP_EOK;
}

static ADSPResult VSM_utility_get_avsync_delays_response_handler( vsm_session_object_t* session_object )
{
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_msg_any_payload_t *param_payload_ptr = ( elite_msg_any_payload_t* )session_object->cmd_control.resp_msg.pPayload;

  elite_msg_custom_delay_type* get_delay_msg = ( elite_msg_custom_delay_type* )session_object->cmd_control.resp_msg.pPayload;
  vsm_get_delay_ack_t* get_delay_ack = ( vsm_get_delay_ack_t* )get_delay_msg->delay_ptr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_get_avsync_delays_response_handler: received response token 0x%X  result 0x%X",
           param_payload_ptr->unClientToken, param_payload_ptr->unResponseResult );

  session_object->avsync_delay_info.stream_rx_algorithmic_delay = get_delay_ack->vdec_delay;
  session_object->avsync_delay_info.stream_tx_algorithmic_delay = get_delay_ack->venc_delay;

  session_object->cmd_control.resp_count++;
  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VSM: GET_AVSYNC_DELAY_RSP_HANDLER: enc_algo_delay = %d us, dec_algo_delay = %d us",
           get_delay_ack->venc_delay, get_delay_ack->vdec_delay );
    qurt_elite_memory_free( ( void* )get_delay_ack );
  }

  if ( session_object->cmd_control.resp_result == 0xFFFFFFFF )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  if ( ADSP_FAILED( param_payload_ptr->unResponseResult ) )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  return ADSP_EOK;
}

static ADSPResult VSM_utility_config_host_pcm_response_handler( vsm_session_object_t* session_object )
{
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_msg_any_payload_t *param_payload_ptr = ( elite_msg_any_payload_t* )session_object->cmd_control.resp_msg.pPayload;

  elite_msg_custom_voc_config_host_pcm_type* hostpcm_msg = 
       ( elite_msg_custom_voc_config_host_pcm_type* )session_object->cmd_control.resp_msg.pPayload;

  elite_apr_packet_t* apr_pkt = ( elite_apr_packet_t* )hostpcm_msg->apr_packet_ptr;
  uint32_t apr_handle = hostpcm_msg->apr_handle;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_config_host_pcm_response_handler: received response token 0x%X  result 0x%X",
           param_payload_ptr->unClientToken, param_payload_ptr->unResponseResult );

  session_object->cmd_control.resp_count++;
  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;
    elite_apr_if_free( apr_handle, apr_pkt );
  }

  if ( session_object->cmd_control.resp_result == 0xFFFFFFFF )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  /* At least one success is treated as success here.*/
  if ( ADSP_SUCCEEDED( param_payload_ptr->unResponseResult ) )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  return ADSP_EOK;
}

static ADSPResult VSM_utility_get_kpps_response_handler( vsm_session_object_t* session_object )
{
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_msg_any_payload_t *param_payload_ptr = ( elite_msg_any_payload_t* )session_object->cmd_control.resp_msg.pPayload;

  elite_msg_custom_kpps_type* get_kpps_msg = ( elite_msg_custom_kpps_type* )session_object->cmd_control.resp_msg.pPayload;
  vsm_get_kpps_ack_t* get_kpps_ack = ( vsm_get_kpps_ack_t* )get_kpps_msg->pnKpps;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_get_kpps_response_handler: received response token 0x%X  result 0x%X",
           param_payload_ptr->unClientToken, param_payload_ptr->unResponseResult );

  session_object->cmd_control.resp_count++;
  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;

    if ( session_object->kpps_info.enc != get_kpps_ack->venc_kpps )
    {
      session_object->kpps_info.enc = get_kpps_ack->venc_kpps;
      session_object->is_kpps_changed = TRUE;
    }

    if ( session_object->kpps_info.dec != get_kpps_ack->vdec_kpps )
    {
      session_object->kpps_info.dec = get_kpps_ack->vdec_kpps;
      session_object->is_kpps_changed = TRUE;
    }

    if ( session_object->kpps_info.dec_pp != get_kpps_ack->vdecpp_kpps )
    {
      session_object->kpps_info.dec_pp = get_kpps_ack->vdecpp_kpps;
      session_object->is_kpps_changed = TRUE;
    }

    MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VSM_utility_get_kpps_response_handler: received response enc kpps %d, "
           "dec kpps %d, dec_pp kpps %d, is_kpps_changed %d", get_kpps_ack->venc_kpps,
           get_kpps_ack->vdec_kpps, get_kpps_ack->vdecpp_kpps, session_object->is_kpps_changed );

    qurt_elite_memory_free( ( void* ) get_kpps_ack );
  }

  if ( session_object->cmd_control.resp_result == 0xFFFFFFFF )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  if ( ADSP_FAILED( param_payload_ptr->unResponseResult ) )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  return ADSP_EOK;
}

static ADSPResult VSM_utility_set_timing_response_handler( vsm_session_object_t* session_object )
{
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_msg_any_payload_t *param_payload_ptr = ( elite_msg_any_payload_t* )session_object->cmd_control.resp_msg.pPayload;

  elite_msg_custom_voc_timing_param_type* msg_payload = ( elite_msg_custom_voc_timing_param_type* )session_object->cmd_control.resp_msg.pPayload;
  int32_t* param_data_ptr = msg_payload->param_data_ptr;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_get_kpps_response_handler: received response token 0x%X  result 0x%X",
           param_payload_ptr->unClientToken, param_payload_ptr->unResponseResult );

  session_object->cmd_control.resp_count++;
  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;
    qurt_elite_memory_free( ( void* )param_data_ptr );
  }

  if ( session_object->cmd_control.resp_result == 0xFFFFFFFF )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  if ( ADSP_FAILED( param_payload_ptr->unResponseResult ) )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  return ADSP_EOK;
}

static ADSPResult VSM_utility_afe_connect_response_handler( vsm_session_object_t* session_object )
{
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_msg_any_payload_t *param_payload_ptr = ( elite_msg_any_payload_t* )session_object->cmd_control.resp_msg.pPayload;

  elite_msg_custom_afe_connect* afe_connect_ptr = ( elite_msg_custom_afe_connect* )session_object->cmd_control.resp_msg.pPayload;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_afe_connect_response_handler: received response token 0x%X  result 0x%X",
           param_payload_ptr->unClientToken, param_payload_ptr->unResponseResult );

  if ( ADSP_SUCCEEDED( param_payload_ptr->unResponseResult ) )
  {
    if ( VSM_ASYNC_CMD_TOKEN_TX == param_payload_ptr->unClientToken )
    {
      session_object->tx_afe_handle_ptr = (elite_svc_handle_t *) afe_connect_ptr->svc_handle_ptr;
      session_object->tx_afe_drift_ptr = (void *) afe_connect_ptr->afe_drift_ptr;
    }
    else
    {
      session_object->rx_afe_handle_ptr = (elite_svc_handle_t *) afe_connect_ptr->svc_handle_ptr;
      session_object->rx_afe_drift_ptr = (void *) afe_connect_ptr->afe_drift_ptr;
    }
  }

  session_object->cmd_control.resp_count++;
  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;
  }

  if ( session_object->cmd_control.resp_result == 0xFFFFFFFF )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  if ( ADSP_FAILED( param_payload_ptr->unResponseResult ) )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  return ADSP_EOK;
}

static int32_t VSM_utility_update_afe_port_info_processing(
  vsm_session_object_t* session_object,
  elite_apr_packet_t *apr_pkt
)
{
  ADSPResult rc = ADSP_EOK;

  if (( NULL == session_object ) || (NULL == apr_pkt))
  {
    return ADSP_EBADPARAM;
  }

  vss_ivocproc_evt_afe_port_info_t* payload;
  uint32_t payload_size;
  uint32_t  i_index = 0;
  uint32_t  *p_cvp_afe_rx_port_id_array  = NULL;
  uint32_t  *p_cvp_afe_tx_port_id_array  = NULL;
  uint32_t  *p_cvp_afe_ec_ref_port_id_array  = NULL;
  uint16_t  *p_cvp_handle_array    = NULL;

  payload = APRV2_PKT_GET_PAYLOAD( vss_ivocproc_evt_afe_port_info_t, apr_pkt );
  payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

  if ( payload_size != sizeof( vss_ivocproc_evt_afe_port_info_t ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvs_stream_update_afe_port_info_processing(): Unexpected payload size, %d != %d",
                                            payload_size,
                                            sizeof( vss_ivocproc_evt_afe_port_info_t ) );

    return ADSP_EBADPARAM;
  }

  p_cvp_handle_array = (uint16_t *)(session_object->active_set.attach_table.vpm_handles);
  p_cvp_afe_tx_port_id_array = session_object->active_set.attach_table.tx_port_id;
  p_cvp_afe_rx_port_id_array = session_object->active_set.attach_table.rx_port_id;
  p_cvp_afe_ec_ref_port_id_array = session_object->active_set.attach_table.ec_ref_port_id;

  if ( p_cvp_afe_tx_port_id_array && p_cvp_afe_rx_port_id_array && p_cvp_afe_ec_ref_port_id_array )
  {
    for (; i_index < VSM_MAX_ATTACHED_VOCPROC_CNT; i_index++)
    {
      //if (p_cvp_handle_array[i_index] == session_object->target_set.attach_detach_vocproc_handle)
      {
        p_cvp_afe_tx_port_id_array[i_index] = payload->tx_port_id;
        p_cvp_afe_rx_port_id_array[i_index] = payload->rx_port_id;
        p_cvp_afe_ec_ref_port_id_array[i_index] = payload->ec_ref_port_id;
        break;
      }
    }
  }

  ( void ) elite_apr_if_free( vsm_apr_handle, apr_pkt );
  return rc;
}

static ADSPResult VSM_utility_default_response_handler( vsm_session_object_t* session_object )
{
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_msg_any_payload_t *param_payload_ptr = ( elite_msg_any_payload_t* )session_object->cmd_control.resp_msg.pPayload;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_default_response_handler: received response token 0x%X  result 0x%X",
           param_payload_ptr->unClientToken, param_payload_ptr->unResponseResult );

  session_object->cmd_control.resp_count++;
  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;
  }

  if ( session_object->cmd_control.resp_result == 0xFFFFFFFF )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  if ( ADSP_FAILED( param_payload_ptr->unResponseResult ) )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  return ADSP_EOK;
}

/************************ Elite MSG Handlers **********************
*******************************************************************/

ADSPResult VSM_session_respQ_custom_msg_handler( vsm_session_object_t* session_object )
{
  uint32_t result = ADSP_EOK;

  // Validate input params
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_msg_custom_header_t *pPayload = ( elite_msg_custom_header_t* )( session_object->cmd_control.resp_msg.pPayload );

  if ( VSM_ASYNC_CMD_TOKEN_TX == pPayload->unClientToken )
  {
    switch ( pPayload->unSecOpCode )
    {
      case ELITEMSG_CUSTOM_VMX_CFG_OUTPUT_PORTS:
      case VENC_SET_MUTE_CMD:
      case ELITEMSG_CUSTOM_VMX_RUN:
      case ELITEMSG_CUSTOM_VMX_STOP:
      case VENC_REINIT_CMD:
      case VENC_SET_PKT_EXCHANGE_MODE:
      case VENC_START_RECORD_CMD_V2:
      case VENC_STOP_RECORD_CMD:
      case ELITEMSG_CUSTOM_AFEDISCONNECT_REQ:
      case VENC_SET_PARAM_V3:
      case VENC_SET_STREAM_PP_SAMP_RATE:
      case VENC_SET_LTETTY_MODE:
      case VENC_SET_TTY_MODE:
      case VENC_SET_VOC_PARAM:
      {
        ( void )VSM_utility_default_response_handler( session_object );
      }
      break;

      case ELITEMSG_CUSTOM_AFECONNECT_REQ:
      {
        ( void )VSM_utility_afe_connect_response_handler( session_object );
      }
      break;

      case VENC_SET_OOB_PKT_EXCHANGE_CONFIG:
      {
        ( void )VSM_utility_set_oob_pktexg_config_response_handler( session_object );
      }
      break;

      case VENC_GET_KPPS_CMD:
      {
        ( void )VSM_utility_get_kpps_response_handler( session_object );
      }
      break;

      case VENC_GET_DELAY_CMD:
      {
        ( void )VSM_utility_get_avsync_delays_response_handler( session_object );
      }
      break;

      case VENC_SET_TIMINGV2_CMD:
      case VENC_SET_TIMINGV3_CMD:
      {
        ( void )VSM_utility_set_timing_response_handler( session_object );
      }
      break;

      case VENC_CONFIG_HOST_PCM:
      {
        ( void )VSM_utility_config_host_pcm_response_handler( session_object );
      }
      break;

      default:
      {
        elite_msg_any_payload_t *pay_load = ( elite_msg_any_payload_t* ) session_object->cmd_control.resp_msg.pPayload;
        MSG_6( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"VSM_session_respQ_custom_msg_handler: unknown Msg=[0x%lx,0x%lx],bq0x%lx,rq0x%lx,0x%lx,0x%lx",
               session_object->cmd_control.resp_msg.unOpCode, ( uint32_t )( pay_load ),
               ( uint32_t )( pay_load->pBufferReturnQ ), ( uint32_t )( pay_load->pResponseQ ),
               pay_load->unClientToken, pay_load->unResponseResult );
        result = ADSP_EUNSUPPORTED;
      }
      break;
    }
  }
  else if ( VSM_ASYNC_CMD_TOKEN_RX == pPayload->unClientToken )
  {
    switch ( pPayload->unSecOpCode )
    {
      case ELITEMSG_CUSTOM_VMX_CFG_INPUT_PORTS:
      case VDEC_SET_MUTE_CMD:
      case ELITEMSG_CUSTOM_VMX_RUN:
      case ELITEMSG_CUSTOM_VMX_STOP:
      case VDEC_REINIT_CMD:
      case VDEC_SET_PKT_EXCHANGE_MODE:
      case VDEC_START_RECORD_CMD_V2:
      case VDEC_STOP_RECORD_CMD:
      case ELITEMSG_CUSTOM_AFEDISCONNECT_REQ:
      case VDEC_SET_PARAM_V3:
      case VDEC_SET_STREAM_PP_SAMP_RATE:
      case VDEC_SET_LTETTY_MODE:
      case VDEC_SET_TTY_MODE:
      case VDEC_SET_VOC_PARAM:
      {
        ( void )VSM_utility_default_response_handler( session_object );
      }
      break;

      case ELITEMSG_CUSTOM_AFECONNECT_REQ:
      {
        ( void )VSM_utility_afe_connect_response_handler( session_object );
      }
      break;

      case VDEC_SET_OOB_PKT_EXCHANGE_CONFIG:
      {
        ( void )VSM_utility_set_oob_pktexg_config_response_handler( session_object );
      }
      break;

      case VDEC_GET_KPPS_CMD:
      {
        ( void )VSM_utility_get_kpps_response_handler( session_object );
      }
      break;

      case VDEC_GET_DELAY_CMD:
      {
        ( void )VSM_utility_get_avsync_delays_response_handler( session_object );
      }
      break;

      case VDEC_SET_TIMINGV2_CMD:
      case VDEC_SET_TIMINGV3_CMD:
      {
        ( void )VSM_utility_set_timing_response_handler( session_object );
      }
      break;

      case VDEC_CONFIG_HOST_PCM:
      {
        ( void )VSM_utility_config_host_pcm_response_handler( session_object );
      }
      break;

      default:
      {
        elite_msg_any_payload_t *pay_load = ( elite_msg_any_payload_t* ) session_object->cmd_control.resp_msg.pPayload;
        MSG_6( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"VSM_session_respQ_custom_msg_handler: unknown Msg=[0x%lx,0x%lx],bq0x%lx,rq0x%lx,0x%lx,0x%lx",
               session_object->cmd_control.resp_msg.unOpCode, ( uint32_t )( pay_load ),
               ( uint32_t )( pay_load->pBufferReturnQ ), ( uint32_t )( pay_load->pResponseQ ),
               pay_load->unClientToken, pay_load->unResponseResult );
        result = ADSP_EUNSUPPORTED;
      }
      break;
    }
  }
  else
  {
    elite_msg_any_payload_t *pay_load = ( elite_msg_any_payload_t* ) session_object->cmd_control.resp_msg.pPayload;
    MSG_6( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"VSM_session_respQ_custom_msg_handler: Fail Msg=[0x%lx,0x%lx],bq0x%lx,rq0x%lx,0x%lx,0x%lx",
           session_object->cmd_control.resp_msg.unOpCode, ( uint32_t )( pay_load ),
           ( uint32_t )( pay_load->pBufferReturnQ ), ( uint32_t )( pay_load->pResponseQ ),
           pay_load->unClientToken, pay_load->unResponseResult );
    result = ADSP_EUNSUPPORTED;
  }

  /* free the incoming response */
  elite_msg_release_msg( &session_object->cmd_control.resp_msg );

  return ADSP_EOK;
}

ADSPResult VSM_session_respQ_aprpkt_handler( vsm_session_object_t* session_object )
{

  ADSPResult rc = ADSP_EOK;

  QURT_ELITE_ASSERT( session_object );

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.resp_msg.pPayload );

  uint32_t opcode = elite_apr_if_get_opcode( apr_pkt );

  switch ( opcode )
  {
    case VSM_EVT_VOC_OPERATING_MODE_UPDATE:
    {
      rc = VSM_utility_voc_op_mode_event_handler( session_object, apr_pkt );
    }
    break;

    case VSS_IVOCPROC_RSP_ATTACH_STREAM:
    case VSS_IVOCPROC_RSP_DETACH_STREAM:
    {
      rc = VSM_utility_attach_detach_stream_apr_response_handler( session_object, apr_pkt );
    }
    break;

    case VSS_ISTREAM_EVT_DTMF_GENERATION_ENDED:
    {
      rc = VSM_utility_send_dtmf_generation_ended_event_to_client( session_object, apr_pkt );
    }
    break;

    case VSS_ISTREAM_EVT_RX_DTMF_DETECTED:
    {
      rc = VSM_utility_send_rx_dtmf_detect_event_to_client( session_object, apr_pkt );
    }
    break;

    case VSS_ICOMMON_RSP_SET_SYSTEM_CONFIG:
    {
      rc = VSM_utility_set_system_config_apr_response_handler( session_object, apr_pkt );
    }
    break;
	
    case VSS_IVOCPROC_EVT_AFE_PORT_INFO:
    {
      rc = VSM_utility_update_afe_port_info_processing( session_object, apr_pkt );
    }
    break;

    case APRV2_IBASIC_RSP_RESULT:
    {
      rc = VSM_utility_default_apr_response_handler( session_object, apr_pkt );
    }
    break;

    default:
    {
      rc = ADSP_EUNSUPPORTED;
    }
    break;

  }

  if ( ADSP_FAILED( rc ) )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_session_respQ_aprpkt_handler: Command failed"
         "..  opcode 0x%8X, port 0x%8X, error %d", opcode, apr_pkt->dst_port, rc );
    rc = elite_apr_if_free( vsm_apr_handle, apr_pkt );
    VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );
  }

  return ADSP_EOK;
}

ADSPResult VSM_session_respQ_get_param_msg_handler( vsm_session_object_t* session_object )
{

  uint32_t result = ADSP_EOK;
  static uint32_t tx_resp_result = 0xFFFFFFFF;
  static uint32_t rx_resp_result = 0xFFFFFFFF;
  static uint32_t param_payload_size = 0;
  int32_t rc = ADSP_EOK;
  uint16_t client_addr;
  uint8_t* param_address = NULL;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }
  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;

  elite_msg_any_payload_t *param_payload_ptr = ( elite_msg_any_payload_t* )session_object->cmd_control.resp_msg.pPayload;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_session_respQ_get_param_msg_handler: received response "
         "token 0x%X  result 0x%X", param_payload_ptr->unClientToken, param_payload_ptr->unResponseResult );

  elite_msg_param_cal_t* cal_payload_ptr = ( elite_msg_param_cal_t* )session_object->cmd_control.resp_msg.pPayload;

  /* Re-initialize the results for new command sent. */
  if ( 0 == session_object->cmd_control.resp_count )
  {
    uint32_t tx_resp_result = 0xFFFFFFFF;
    uint32_t rx_resp_result = 0xFFFFFFFF;
    param_payload_size = 0;
  }
  session_object->cmd_control.resp_count++;

  if ( ( VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_TX == param_payload_ptr->unClientToken ) ||
       ( VSM_ASYNC_CMD_TOKEN_PARAM_OOB_TX == param_payload_ptr->unClientToken )
     )
  {
    tx_resp_result = param_payload_ptr->unResponseResult;
  }
  else if ( ( VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_RX == param_payload_ptr->unClientToken ) ||
            ( VSM_ASYNC_CMD_TOKEN_PARAM_OOB_RX == param_payload_ptr->unClientToken )
          )
  {
    rx_resp_result = param_payload_ptr->unResponseResult;
  }

  /* As we are sending the command to both venc and vdec,
     and as only the successful response will return a non-zero size,
     we need to save the size of params to be sent to client.
  */
  if ( ( ADSP_EOK == param_payload_ptr->unResponseResult ) && 
       ( 0 == param_payload_size ) )
  {
    param_payload_size = cal_payload_ptr->unSize;
  }

  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;

    if ( ( ADSP_EOK == tx_resp_result ) || ( ADSP_EOK == rx_resp_result ) )
    {
      session_object->cmd_control.resp_result = ADSP_EOK;
      switch ( apr_pkt->opcode )
      {
        case VSS_ICOMMON_CMD_GET_PARAM_V2:
        {
          vss_icommon_cmd_get_param_v2_t* payload;
          voice_get_param_ack_t* get_parm_ack_ptr;
          uint8_t* get_param_payload_ptr;
          elite_apr_packet_t* resp_apr_pkt = NULL;
          uint32_t resp_pkt_size;
          bool_t is_inband = FALSE;

          payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_get_param_v2_t, apr_pkt );

          if ( ( VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_TX == param_payload_ptr->unClientToken ) ||
               ( VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_RX == param_payload_ptr->unClientToken )
             )
          {
            resp_pkt_size = ( payload->mem_size + sizeof( voice_get_param_ack_t ) );
            is_inband = TRUE;
          }
          else
          {
            resp_pkt_size = sizeof( voice_get_param_ack_t );

            /* Extract OOB param address for logging purpose. */
            {
              cvd_virt_addr_t ret_virt_addr;
              rc = cvd_mem_mapper_get_virtual_addr_v2( payload->mem_handle, payload->mem_address,
                                                       &ret_virt_addr );
              if ( rc )
              {
                param_address = NULL;
              }
              else
              {
                param_address = ( uint8_t* )( ret_virt_addr.ptr );
              }
            }
          }

          rc = elite_apr_if_alloc_cmd_rsp(
                             vsm_apr_handle,
                             session_object->apr_info.self_addr,
                             session_object->apr_info.self_port,
                             apr_pkt->src_addr,
                             apr_pkt->src_port,
                             apr_pkt->token,
                             VOICE_RSP_GET_PARAM_ACK,
                             resp_pkt_size,
                             &resp_apr_pkt
                           );
          if ( ADSP_FAILED( rc ) )
          {
            VSM_COMM_ERROR( rc, client_addr );
            break;
          }
          get_parm_ack_ptr = ( voice_get_param_ack_t* )elite_apr_if_get_payload_ptr(resp_apr_pkt);
          get_parm_ack_ptr->status = ADSP_EOK;

          if ( TRUE == is_inband )
          {
            get_param_payload_ptr = ( uint8_t* )( elite_apr_if_get_payload_ptr(resp_apr_pkt) );
            get_param_payload_ptr += sizeof( voice_get_param_ack_t );

            memscpy( get_param_payload_ptr, payload->mem_size,
                     cal_payload_ptr->pnParamData, ( param_payload_size + sizeof( voice_param_data_t ) ));
            /* Free the memory allocated. */
            qurt_elite_memory_free( ( void* )cal_payload_ptr->pnParamData );

            param_address = get_param_payload_ptr;
          }

#if defined(__qdsp6__) && !defined(SIM)
          if ( NULL != param_address )
          {
            int8_t *bufptr[4] = { (int8_t *) param_address, NULL, NULL, NULL };
            voice_log_buffer( bufptr,
                        VOICE_LOG_CHAN_VSM_GET_PARAM,
                        session_object->apr_info.self_port,
                        qurt_elite_timer_get_time(),
                        VOICE_LOG_DATA_FORMAT_PCM_MONO,
                        1,   /* dummy value */
                        param_payload_size + sizeof( voice_param_data_t ),
                        NULL );
          }
#endif
          elite_apr_if_async_send( vsm_apr_handle, resp_apr_pkt);
        }
        break;

        case VSS_ICOMMON_CMD_GET_PARAM_V3:
        {
          vss_icommon_cmd_get_param_v3_t* payload;
          vss_icommon_rsp_get_param_v3_t * get_parm_ack_ptr;
          uint8_t* get_param_payload_ptr;
          elite_apr_packet_t* resp_apr_pkt = NULL;
          uint32_t resp_pkt_size;
          bool_t is_inband = FALSE;

          payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_get_param_v3_t, apr_pkt );

          if ( ( VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_TX == param_payload_ptr->unClientToken ) ||
               ( VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_RX == param_payload_ptr->unClientToken ) )
          {
            resp_pkt_size = ( payload->mem_size + sizeof( vss_icommon_rsp_get_param_v3_t ) );
            is_inband = TRUE;
          }
          else
          {
            resp_pkt_size = sizeof( vss_icommon_rsp_get_param_v3_t );

            /* Extract OOB param address for logging purpose. */
            {
              cvd_virt_addr_t ret_virt_addr;
              rc = cvd_mem_mapper_get_virtual_addr_v2( payload->mem_handle, payload->mem_address,
                                                       &ret_virt_addr );
              if ( rc )
              {
                param_address = NULL;
              }
              else
              {
                param_address = ( uint8_t* )( ret_virt_addr.ptr );
              }
            }
          }

          rc = elite_apr_if_alloc_cmd_rsp(
                           vsm_apr_handle,
                           session_object->apr_info.self_addr,
                           session_object->apr_info.self_port,
                           apr_pkt->src_addr,
                           apr_pkt->src_port,
                           apr_pkt->token,
                           VSS_ICOMMON_RSP_GET_PARAM_V3,
                           resp_pkt_size,
                           &resp_apr_pkt
                         );
          if ( ADSP_FAILED( rc ) )
          {
            VSM_COMM_ERROR( rc, client_addr );
            break;
          }
          get_parm_ack_ptr = ( vss_icommon_rsp_get_param_v3_t * )elite_apr_if_get_payload_ptr(resp_apr_pkt);
          get_parm_ack_ptr->status = ADSP_EOK;

          if ( TRUE == is_inband )
          {
            get_param_payload_ptr = ( uint8_t* )( elite_apr_if_get_payload_ptr(resp_apr_pkt) );
            get_param_payload_ptr += sizeof( vss_icommon_rsp_get_param_v3_t  );

            memscpy( get_param_payload_ptr, payload->mem_size,
                     cal_payload_ptr->pnParamData, ( param_payload_size + sizeof( voice_param_data_v2_t ) ));
            /* Free the memory allocated. */
            qurt_elite_memory_free( ( void* )cal_payload_ptr->pnParamData );

            param_address = get_param_payload_ptr;
          }

#if defined(__qdsp6__) && !defined(SIM)
        if ( NULL != param_address )
        {
          int8_t *bufptr[4] = { (int8_t *) param_address, NULL, NULL, NULL };
          voice_log_buffer( bufptr,
                      VOICE_LOG_CHAN_VSM_GET_PARAM_V2,
                      session_object->apr_info.self_port,
                      qurt_elite_timer_get_time(),
                      VOICE_LOG_DATA_FORMAT_PCM_MONO,
                      1,   /* dummy value */
                      param_payload_size + sizeof( voice_param_data_v2_t ),
                      NULL );
        }
#endif
          elite_apr_if_async_send( vsm_apr_handle, resp_apr_pkt);
        }
        break;

        case VSS_ICOMMON_CMD_GET_UI_PROPERTY:
        {
          vss_icommon_cmd_get_ui_property_t* payload;
          voice_get_param_ack_t* get_parm_ack_ptr;
          uint8_t* get_param_payload_ptr;
          elite_apr_packet_t* resp_apr_pkt = NULL;
          uint32_t resp_pkt_size;

          payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_get_ui_property_t, apr_pkt );

          resp_pkt_size = ( payload->param_size + sizeof( voice_param_data_t ) + sizeof( voice_get_param_ack_t ) );
          rc = elite_apr_if_alloc_cmd_rsp(
                             vsm_apr_handle,
                             session_object->apr_info.self_addr,
                             session_object->apr_info.self_port,
                             apr_pkt->src_addr,
                             apr_pkt->src_port,
                             apr_pkt->token,
                             VSS_ICOMMON_RSP_GET_UI_PROPERTY,
                             resp_pkt_size,
                             &resp_apr_pkt
                           );
           if ( ADSP_FAILED( rc ) )
          {
            VSM_COMM_ERROR( rc, client_addr );
            break;
          }
          get_parm_ack_ptr = ( voice_get_param_ack_t* )elite_apr_if_get_payload_ptr(resp_apr_pkt);
          get_parm_ack_ptr->status = ADSP_EOK;

          get_param_payload_ptr = ( uint8_t* )( elite_apr_if_get_payload_ptr(resp_apr_pkt) );
          get_param_payload_ptr += sizeof( voice_get_param_ack_t );

          memscpy( get_param_payload_ptr, ( payload->param_size + sizeof( voice_param_data_t ) ),
                   cal_payload_ptr->pnParamData, param_payload_size + sizeof( voice_param_data_t ) );
          elite_apr_if_async_send( vsm_apr_handle, resp_apr_pkt);
          /* Free the memory allocated. */
          qurt_elite_memory_free( ( void* )cal_payload_ptr->pnParamData );
        }
        break;

        case VSS_ICOMMON_CMD_GET_UI_PROPERTY_V2:
        {
          vss_icommon_cmd_get_ui_property_v2_t* payload;
          vss_icommon_rsp_get_ui_property_v2_t* get_parm_ack_ptr;
          uint8_t* get_param_payload_ptr;
          elite_apr_packet_t* resp_apr_pkt = NULL;
          uint32_t resp_pkt_size;

          payload = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_get_ui_property_v2_t, apr_pkt );

          resp_pkt_size = ( payload->param_size + sizeof( voice_param_data_v2_t ) + sizeof( voice_get_param_ack_t ) );
          rc = elite_apr_if_alloc_cmd_rsp(
                             vsm_apr_handle,
                             session_object->apr_info.self_addr,
                             session_object->apr_info.self_port,
                             apr_pkt->src_addr,
                             apr_pkt->src_port,
                             apr_pkt->token,
                             VSS_ICOMMON_RSP_GET_UI_PROPERTY_V2,
                             resp_pkt_size,
                             &resp_apr_pkt
                           );
           if ( ADSP_FAILED( rc ) )
          {
            VSM_COMM_ERROR( rc, client_addr );
            break;
          }
          get_parm_ack_ptr = ( vss_icommon_rsp_get_ui_property_v2_t* )elite_apr_if_get_payload_ptr(resp_apr_pkt);
          get_parm_ack_ptr->status = ADSP_EOK;

          get_param_payload_ptr = ( uint8_t* )( elite_apr_if_get_payload_ptr(resp_apr_pkt) );
          get_param_payload_ptr += sizeof( voice_get_param_ack_t );

          memscpy( get_param_payload_ptr, ( payload->param_size + sizeof( voice_param_data_v2_t ) ),
                   cal_payload_ptr->pnParamData, param_payload_size + sizeof( voice_param_data_v2_t ) );
          elite_apr_if_async_send( vsm_apr_handle, resp_apr_pkt);
          /* Free the memory allocated. */
          qurt_elite_memory_free( ( void* )cal_payload_ptr->pnParamData );
        }
        break;
      }
    }
    else
    {
      session_object->cmd_control.resp_result = ADSP_EFAILED;
      session_object->cmd_control.cmd_result = ADSP_EFAILED;

      if ( ( VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_TX == param_payload_ptr->unClientToken ) ||
           ( VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_RX == param_payload_ptr->unClientToken )
         )
      {
        qurt_elite_memory_free( ( void* )cal_payload_ptr->pnParamData );
      }
    }

  }

  /* free the incoming response */
  elite_msg_release_msg( &session_object->cmd_control.resp_msg );

  return ADSP_EOK;
}

ADSPResult VSM_session_respQ_set_param_msg_handler( vsm_session_object_t* session_object )
{

  uint32_t result = ADSP_EOK;

  // Validate input params
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }
  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  elite_msg_any_payload_t *param_payload_ptr = ( elite_msg_any_payload_t* )session_object->cmd_control.resp_msg.pPayload;
  elite_msg_param_cal_t* cal_payload_ptr = ( elite_msg_param_cal_t* )session_object->cmd_control.resp_msg.pPayload;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_session_respQ_set_param_msg_handler: received response token 0x%X  result 0x%X",
           param_payload_ptr->unClientToken, param_payload_ptr->unResponseResult );

  session_object->cmd_control.resp_count++;
  if ( session_object->cmd_control.resp_count == session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = FALSE;

    if ( ( VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_TX == param_payload_ptr->unClientToken ) ||
         ( VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_RX == param_payload_ptr->unClientToken )
       )
    {
      qurt_elite_memory_free( ( void* )cal_payload_ptr->pnParamData );
    }
  }

  if ( session_object->cmd_control.resp_result == 0xFFFFFFFF ) 
  {
    session_object->cmd_control.resp_result = ADSP_EOK;
  }

  if ( ( ADSP_EUNSUPPORTED != param_payload_ptr->unResponseResult ) && 
       ( ADSP_EOK != param_payload_ptr->unResponseResult ) )
  {
    session_object->cmd_control.resp_result = param_payload_ptr->unResponseResult;
  }

  /* free the incoming response */
  elite_msg_release_msg( &session_object->cmd_control.resp_msg );

  return ADSP_EOK;
}

ADSPResult VSM_session_respQ_default_msg_handler( vsm_session_object_t* session_object )
{

  uint32_t result = ADSP_EOK;

  // Validate input params
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  //we can use default response handler for now.
  VSM_utility_default_response_handler( session_object );

  /* free the incoming response */
  elite_msg_release_msg( &session_object->cmd_control.resp_msg );

  return ADSP_EOK;

}

ADSPResult VSM_session_respQ_unsupported_msg_handler( vsm_session_object_t* session_object )
{

  elite_msg_any_payload_t *pay_load = ( elite_msg_any_payload_t* ) session_object->cmd_control.resp_msg.pPayload;
  MSG_6( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"VSM_session_respQ_unsupported_msg_handler: Fail Msg=[0x%lx,0x%lx],bq0x%lx,rq0x%lx,0x%lx,0x%lx",
         session_object->cmd_control.resp_msg.unOpCode, ( uint32_t )( pay_load ),
         ( uint32_t )( pay_load->pBufferReturnQ ), ( uint32_t )( pay_load->pResponseQ ),
         pay_load->unClientToken, pay_load->unResponseResult );

  /* free the incoming response */
  elite_msg_release_msg( &session_object->cmd_control.resp_msg );

  return ADSP_EOK;

}
