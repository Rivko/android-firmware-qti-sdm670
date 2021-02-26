/*
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvs/src/vsm_actions.cpp#7 $
  $Author: pwbldsvc $
*/

#include "vsm_common_i.h"
#include "qurt_elite.h"
#include "mmstd.h"
#include "VoiceStreamMgr.h"
#include "vsm_header_i.h"
#include "venc_svc.h"
#include "VoiceDec.h"
#include "VoiceMixerSvc.h"
#include "VoiceCmnUtils.h"
#include "adsp_vparams_api.h"
#include "cvd_cal_log_i.h"
#include "vocsvc_avtimer_api.h"
#include "AFEInterface.h"
#include "EliteMsg_AfeCustom.h"
#include "Elite_intf_extns_vocoder.h"

#include "vsm_apr_router.h"
#include "cvd_log_i.h"
#include "event.h"

extern elite_apr_handle_t vsm_apr_handle;
extern uint16_t vsm_mvm_addr;
extern uint16_t vsm_cvp_addr;
extern elite_svc_handle_t *txVoiceMatrix;
extern elite_svc_handle_t *rxVoiceMatrix;

//TBD: Make this a VSM struct member
extern uint32_t vsm_memory_map_client;

/************************ Actions **********************
*******************************************************/

ADSPResult vsm_action_cleanup_ssr_domain_passive_clients(
  uint8_t domain_id,
  vsm_session_object_t* session_object
)
{
  uint8_t index = 0;

  for ( index = 0; index < VSM_NUM_MAX_PASSIVE_CLIENTS; index ++ )
  {
    if ( APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, 
         session_object->passive_clients[ index ].client_addr ) == domain_id )
    {
      session_object->passive_clients[ index ].client_addr = 0;
      session_object->passive_clients[ index ].client_port = 0;
      session_object->ref_count--;
    }
  }
  return ADSP_EOK;
}

ADSPResult vsm_action_update_cached_system_sampling_rates(
  vsm_session_object_t* session_object
)
{
  ADSPResult rc = ADSP_EOK;
  uint32_t target_dec_sr;
  uint32_t target_enc_sr;
  uint32_t target_rx_pp_sr;
  uint32_t target_tx_pp_sr;

  rc = vsm_utility_determine_target_sampling_rates(
         session_object->active_set.media_id,
         session_object->requested_var_voc_rx_sampling_rate,
         session_object->requested_var_voc_tx_sampling_rate,
         &target_dec_sr,
         &target_enc_sr,
         &target_rx_pp_sr,
         &target_tx_pp_sr );
  if ( rc )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "vsm_action_update_cached_system_sampling_rates:"
           " Unexpected params Error:: Media ID = 0x%08X or requested_var_voc_rx_sampling_rate = 0x%08X"
           "requested_var_voc_tx_sampling_rate = 0x%08X!!\n", session_object->active_set.media_id,
           session_object->requested_var_voc_rx_sampling_rate, session_object->requested_var_voc_tx_sampling_rate );
  }

  if ( ( session_object->target_set.system_config.dec_sr != target_dec_sr ) ||
       ( session_object->target_set.system_config.enc_sr != target_enc_sr ) )
  {
    session_object->is_stream_config_changed = TRUE;

    /* Save parameters in target set to be picked up by the
     * VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG command sequencer.
     */
    session_object->target_set.system_config.dec_sr = target_dec_sr;
    session_object->target_set.system_config.enc_sr = target_enc_sr;
    session_object->target_set.system_config.rx_pp_sr = target_rx_pp_sr;
    session_object->target_set.system_config.tx_pp_sr = target_tx_pp_sr;
  }

  return rc;
}

ADSPResult vsm_action_forward_command_to_venc(
  vsm_session_object_t* session_object,
  uint32_t opcode
)
{
  ADSPResult rc = ADSP_EOK;
  uint32_t orig_opcode;
  uint32_t client_addr;
  elite_msg_any_t msg;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );
  client_addr = apr_pkt->src_addr;
  orig_opcode = apr_pkt->opcode;

  msg.unOpCode = ELITE_APR_PACKET;
  msg.pPayload = ( void* )apr_pkt;

  if ( NULL != session_object->venc )
  {
    apr_pkt->opcode = opcode;
    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL,"vsm_action_forward_command_to_venc:"
             " qurt_elite_queue_push_back error = %d!!\n",( int )rc );
      apr_pkt->opcode = orig_opcode;
      rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EFAILED );
      VSM_COMM_ERROR( rc, client_addr );
    }
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_FATAL,"vsm_action_forward_command_to_venc: venc is NULL!!\n" );
    rc = elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_ENOTREADY );
    VSM_COMM_ERROR( rc, client_addr );
  }

  return ADSP_EOK;
}

ADSPResult vsm_action_set_tty_mode(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_any_t msg_tx_t;
  elite_msg_any_t msg_rx_t;
  elite_msg_custom_voc_stream_set_tty_mode *payload_ptr = NULL;
  uint32_t size = sizeof( elite_msg_custom_voc_stream_set_tty_mode );

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_tty_mode: tty_mode: %d!!\n", session_object->tty_state.m_etty_mode );

  if ( session_object->venc )
  {
    result = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( result))
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_tty_mode: FAILED to create EliteMsg!!\n" );
      return result;
    }

    payload_ptr = ( elite_msg_custom_voc_stream_set_tty_mode* ) msg_tx_t.pPayload;
    payload_ptr->sec_opcode = VENC_SET_TTY_MODE;
    payload_ptr->tty_mode = session_object->tty_state.m_etty_mode;

    result = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );

    if ( ADSP_FAILED( result ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_tty_mode: FAILED to push to venc cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_tx_t);
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec )
  {
    result = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK );
    if( ADSP_FAILED( result))
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_tty_mode: FAILED to create EliteMsg!!\n" );
      return result;
    }

    payload_ptr = ( elite_msg_custom_voc_stream_set_tty_mode* )msg_rx_t.pPayload;
    payload_ptr->sec_opcode = VDEC_SET_TTY_MODE;
    payload_ptr->tty_mode = session_object->tty_state.m_etty_mode;

    result = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
    if (ADSP_FAILED(result))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_tty_mode: FAILED to push to vdec cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_tty_mode: cmd issued count %d, resp count %d\n",
       session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_set_oobtty_mode(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_any_t msg_tx_t;
  elite_msg_any_t msg_rx_t;
  elite_msg_custom_voc_stream_set_tty_mode *payload_ptr = NULL;
  uint32_t size = sizeof( elite_msg_custom_voc_stream_set_tty_mode );

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_oobtty_mode: oob_tty_mode: %d!!\n", session_object->tty_state.m_oobtty_mode );

  if ( session_object->venc )
  {
    result = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( result))
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_oobtty_mode: FAILED to create EliteMsg!!\n" );
      return result;
    }

    payload_ptr = ( elite_msg_custom_voc_stream_set_tty_mode* ) msg_tx_t.pPayload;
    payload_ptr->sec_opcode = VENC_SET_LTETTY_MODE;
    payload_ptr->tty_mode = session_object->tty_state.m_oobtty_mode;

    result = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );

    if ( ADSP_FAILED( result ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_oobtty_mode: FAILED to push to venc cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_tx_t);
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec )
  {
    result = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK );
    if( ADSP_FAILED( result))
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_oobtty_mode: FAILED to create EliteMsg!!\n" );
      return result;
    }

    payload_ptr = ( elite_msg_custom_voc_stream_set_tty_mode* )msg_rx_t.pPayload;
    payload_ptr->sec_opcode = VDEC_SET_LTETTY_MODE;
    payload_ptr->tty_mode = session_object->tty_state.m_oobtty_mode;

    result = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
    if (ADSP_FAILED(result))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_oobtty_mode: FAILED to push to vdec cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_oobtty_mode: cmd issued count %d, resp count %d\n",
       session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_update_tty_mode(
  vsm_session_object_t* session_object
)
{
  ADSPResult rc = ADSP_EOK;
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  if ( TRUE == session_object->tty_info.is_ittyoob_registered )
  {
    session_object->tty_state.m_oobtty_mode = session_object->tty_info.tty_mode;
    session_object->tty_state.m_etty_mode = VSM_TTY_MODE_OFF;
    rc = vsm_action_set_oobtty_mode( session_object );
  }
  else
  {
    session_object->tty_state.m_oobtty_mode = VSM_TTY_MODE_OFF;
    session_object->tty_state.m_etty_mode = session_object->tty_info.tty_mode;
    rc = vsm_action_set_tty_mode( session_object );
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_update_tty_mode:"
         " OOB TTY registered %d, tty_mode = %d",
         session_object->tty_info.is_ittyoob_registered,
         session_object->tty_info.tty_mode );

  return rc;
}

ADSPResult vsm_action_stop_vocproc(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  rc = elite_apr_if_alloc_send_cmd( vsm_apr_handle,
               session_object->apr_info.self_addr, session_object->apr_info.self_port,
               vsm_cvp_addr, session_object->active_set.playback.vpm_port_id,
               VSM_ASYNC_CMD_TOKEN_VOCPROC, VSS_IVOCPROC_CMD_DISABLE,
               NULL, 0 );

  VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );

  session_object->cmd_control.issued_count++;
  session_object->cmd_control.is_resp_pending = TRUE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_stop_vocproc: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return rc;
}

ADSPResult vsm_action_start_vocproc(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  rc = elite_apr_if_alloc_send_cmd( vsm_apr_handle,
               session_object->apr_info.self_addr, session_object->apr_info.self_port,
               vsm_cvp_addr, session_object->active_set.playback.vpm_port_id,
               VSM_ASYNC_CMD_TOKEN_VOCPROC, VSS_IVOCPROC_CMD_ENABLE,
               NULL, 0 );
  if( ADSP_FAILED( rc ) )
  {
    VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );
  }
  else
  {
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  rc = elite_apr_if_alloc_send_cmd( vsm_apr_handle, 
             session_object->apr_info.self_addr, session_object->apr_info.self_port,
             vsm_cvp_addr, session_object->active_set.playback.vpm_port_id,
             VSM_ASYNC_CMD_TOKEN_VOCPROC,
             VSS_IVOCPROC_CMD_MVM_ENABLE, NULL, 0 );
  if( ADSP_FAILED( rc ) )
  {
    VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );
  }
  else
  {
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_start_vocproc: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return rc;
}

ADSPResult vsm_action_set_timing_to_vocproc(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_ivocproc_cmd_set_voice_timing_t payload;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  payload.vsid = session_object->target_set.voice_timing.vsid;
  payload.vfr_mode = session_object->target_set.voice_timing.vfr_mode;
  payload.vfr_cycle_duration_us = session_object->target_set.voice_timing.vfr_cycle_duration_us;

  /* Rx is dont care for playback usecase*/
  payload.dec_req_offset = 0;
  payload.dec_offset = 0;
  payload.vprx_delivery_offset = 0;

  payload.enc_offset = 0;
  payload.vptx_buf_start_offset = 0;
  payload.vptx_proc_start_offset = 0;
  payload.vptx_delivery_offset = 5000;

  rc = elite_apr_if_alloc_send_cmd( vsm_apr_handle,
               session_object->apr_info.self_addr, session_object->apr_info.self_port,
               vsm_cvp_addr, session_object->active_set.playback.vpm_port_id,
               VSM_ASYNC_CMD_TOKEN_VOCPROC, VSS_IVOCPROC_CMD_SET_VOICE_TIMING,
               &payload, sizeof( vss_ivocproc_cmd_set_voice_timing_t )
              );

  VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );

  session_object->cmd_control.issued_count++;
  session_object->cmd_control.is_resp_pending = TRUE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_timing_to_vocproc: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return rc;
}

ADSPResult vsm_action_destroy_vocproc(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  rc = elite_apr_if_alloc_send_cmd( vsm_apr_handle,
               session_object->apr_info.self_addr, session_object->apr_info.self_port,
               vsm_cvp_addr, session_object->active_set.playback.vpm_port_id,
               VSM_ASYNC_CMD_TOKEN_VOCPROC, APRV2_IBASIC_CMD_DESTROY_SESSION,
               NULL, 0 );

  VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );

  session_object->cmd_control.issued_count++;
  session_object->cmd_control.is_resp_pending = TRUE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_destroy_vocproc: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return rc;
}

ADSPResult vsm_action_create_vocproc(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_ivocproc_cmd_create_full_control_session_v2_t payload;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  payload.direction = VSS_IVOCPROC_DIRECTION_TX;
  payload.tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
  if ( VSS_IPLAYBACK_PORT_ID_DEFAULT == session_object->active_set.playback.port_id )
  {
    payload.tx_port_id = AFE_PORT_ID_VOICE_PLAYBACK_TX;
  }
  else
  {
    payload.tx_port_id = session_object->active_set.playback.port_id;
  }

  payload.rx_port_id = VSS_IVOCPROC_PORT_ID_NONE;
  payload.rx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;

  payload.profile_id = VSS_ICOMMON_CAL_NETWORK_ID_NONE;
  payload.vocproc_mode = VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING;
  payload.ec_ref_port_id = VSS_IVOCPROC_PORT_ID_NONE;

  rc = elite_apr_if_alloc_send_cmd( vsm_apr_handle,
               session_object->apr_info.self_addr, session_object->apr_info.self_port,
               vsm_cvp_addr, 0,
               VSM_ASYNC_CMD_TOKEN_VOCPROC, VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2,
               &payload, sizeof( vss_ivocproc_cmd_create_full_control_session_v2_t )
              );

  VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );

  session_object->cmd_control.issued_count++;
  session_object->cmd_control.is_resp_pending = TRUE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_create_vocproc: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return rc;
}

ADSPResult vsm_action_playback_set_system_config(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;
 
  
  rc = elite_apr_if_alloc_send_cmd( vsm_apr_handle,
               session_object->apr_info.self_addr, session_object->apr_info.self_port,
               vsm_cvp_addr, session_object->active_set.playback.vpm_port_id,
               VSM_ASYNC_CMD_TOKEN_VOCPROC, VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG,
               &session_object->active_set.system_config, sizeof( vss_icommon_cmd_set_system_config_t )
              );

  VSM_COMM_ERROR( rc, session_object->apr_info.self_addr );

  session_object->cmd_control.issued_count++;
  session_object->cmd_control.is_resp_pending = TRUE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_playback_set_system_config: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return rc;
}

ADSPResult vsm_action_stop_record(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  if ( ( session_object->venc ) &&
       ( VSM_TAP_POINT_STREAM_END == session_object->active_set.record.tx_tap_point )
     )
  {
    elite_msg_any_t msg_tx_t;
    elite_msg_custom_voc_svc_connect_type *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_voc_svc_connect_type );

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_stop_record: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = ( elite_msg_custom_voc_svc_connect_type* )msg_tx_t.pPayload;
    payload_ptr->sec_opcode = VENC_STOP_RECORD_CMD;
    payload_ptr->svc_handle_ptr  = NULL;

    rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_stop_record: FAILED to push to venc cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( ( session_object->vdec ) &&
       ( VSM_TAP_POINT_STREAM_END == session_object->active_set.record.rx_tap_point )
     )
  {
    elite_msg_any_t msg_rx_t;
    uint16_t afe_port;
    elite_msg_custom_voc_svc_connect_type *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_voc_svc_connect_type );

    rc = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_stop_record: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = (elite_msg_custom_voc_svc_connect_type*) msg_rx_t.pPayload;
    payload_ptr->sec_opcode = VDEC_STOP_RECORD_CMD;
    payload_ptr->svc_handle_ptr  = NULL;

    rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_stop_record: FAILED to push to vdec cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_stop_record: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_start_record(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  if ( ( session_object->venc ) &&
       ( VSM_TAP_POINT_STREAM_END == session_object->active_set.record.tx_tap_point )
     )
  {
    elite_msg_any_t msg_tx_t;
    elite_msg_custom_voc_svc_connect_record_type *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_voc_svc_connect_record_type );

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_start_record: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = ( elite_msg_custom_voc_svc_connect_record_type* )msg_tx_t.pPayload;
    payload_ptr->svc_handle_ptr  = session_object->tx_afe_handle_ptr;
    payload_ptr->afe_drift_ptr = session_object->tx_afe_drift_ptr;
    payload_ptr->sec_opcode = VENC_START_RECORD_CMD_V2;
    payload_ptr->record_mode  = session_object->active_set.record.mode;
    payload_ptr->aud_ref_port = session_object->aud_ref_port_tx;

    rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_start_record: FAILED to push to venc cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( ( session_object->vdec ) &&
       ( VSM_TAP_POINT_STREAM_END == session_object->active_set.record.rx_tap_point )
     )
  {
    elite_msg_any_t msg_rx_t;
    uint16_t afe_port;
    elite_msg_custom_voc_svc_connect_record_type *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_voc_svc_connect_record_type );

    rc = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_start_record: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = (elite_msg_custom_voc_svc_connect_record_type*) msg_rx_t.pPayload;
    payload_ptr->svc_handle_ptr  = session_object->rx_afe_handle_ptr;
    payload_ptr->afe_drift_ptr = session_object->rx_afe_drift_ptr;
    payload_ptr->sec_opcode = VDEC_START_RECORD_CMD_V2;
    payload_ptr->record_mode  = session_object->active_set.record.mode;
    payload_ptr->aud_ref_port = session_object->aud_ref_port_rx;

    rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_start_record: FAILED to push to vdec cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_start_record: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_disconnect_afe(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  if ( ( session_object->venc ) &&
       ( VSM_TAP_POINT_STREAM_END == session_object->active_set.record.tx_tap_point )
     )
  {
    elite_msg_any_t msg_tx_t;
    uint16_t afe_port;
    elite_msg_custom_afe_connect_t *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_afe_connect_t );

    afe_port = session_object->aud_ref_port_tx;

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_disconnect_afe: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = ( elite_msg_custom_afe_connect_t* )msg_tx_t.pPayload;
    payload_ptr->sec_op_code = ELITEMSG_CUSTOM_AFEDISCONNECT_REQ;
    payload_ptr->afe_id = afe_port;

    payload_ptr->svc_handle_ptr = session_object->tx_afe_handle_ptr;

    rc = qurt_elite_queue_push_back( session_object->vsm_afe_cmd_q_ptr, ( uint64_t* )&msg_tx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_disconnect_afe: FAILED to push to afe cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( ( session_object->vdec ) &&
       ( VSM_TAP_POINT_STREAM_END == session_object->active_set.record.rx_tap_point )
     )
  {
    elite_msg_any_t msg_rx_t;
    uint16_t afe_port;
    elite_msg_custom_afe_connect_t *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_afe_connect_t );

    afe_port = session_object->aud_ref_port_rx;

    rc = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_disconnect_afe: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = ( elite_msg_custom_afe_connect_t* ) msg_rx_t.pPayload;
    payload_ptr->sec_op_code = ELITEMSG_CUSTOM_AFEDISCONNECT_REQ;
    payload_ptr->afe_id = afe_port;
    payload_ptr->svc_handle_ptr = session_object->rx_afe_handle_ptr;

    rc = qurt_elite_queue_push_back( session_object->vsm_afe_cmd_q_ptr, ( uint64_t* )&msg_rx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_disconnect_afe: FAILED to push to afe cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_disconnect_afe: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_connect_afe(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint16_t sampling_rate;
  afe_client_t afe_client;      // tx afe client params
  uint32_t num_channels =  1; //default MONO

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  sampling_rate = voice_get_sampling_rate ( session_object->active_set.media_id, 0, session_object->requested_var_voc_tx_sampling_rate );

  if( VSM_RECORD_TX_RX_STEREO == session_object->active_set.record.mode )
  {
    num_channels = 2; //STEREO
  }

  memset(&afe_client, 0, sizeof(afe_client));
  afe_client.sample_rate      = sampling_rate;
  afe_client.data_path      = AFE_RX_VOC_MIXER_IN;
  afe_client.channel        = num_channels;
  afe_client.bytes_per_channel = 2;
  afe_client.bit_width = 16;
  afe_client.buf_size         = sampling_rate / 50; // 20ms
  afe_client.num_buffers       = 2;
  afe_client.interleaved      = FALSE;
  afe_client.cust_proc.resampler_type = IIR_BASED_SRC;
  afe_client.afe_client_handle        = NULL;
  afe_client.cust_proc.subscribe_to_avt_drift = FALSE; // dont subscribe to AV Timer, Voice Timer will take care of this

  if ( ( session_object->venc ) &&
       ( VSM_TAP_POINT_STREAM_END == session_object->active_set.record.tx_tap_point )
     )
  {
    elite_msg_any_t msg_tx_t;
    uint16_t afe_port;
    elite_msg_custom_afe_connect_t *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_afe_connect_t );

    if( AFE_PORT_ID_INVALID == session_object->active_set.record.port_id )
    {
      afe_port =  AFE_PORT_ID_VOICE_RECORD_TX;
    }
    else
    {
      afe_port = session_object->active_set.record.port_id;
    }
    session_object->aud_ref_port_tx = afe_port;

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_connect_afe: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = ( elite_msg_custom_afe_connect_t* )msg_tx_t.pPayload;
    payload_ptr->sec_op_code = ELITEMSG_CUSTOM_AFECONNECT_REQ;
    payload_ptr->afe_id = afe_port;
    payload_ptr->afe_client   = afe_client;

    rc = qurt_elite_queue_push_back( session_object->vsm_afe_cmd_q_ptr, ( uint64_t* )&msg_tx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_connect_afe: FAILED to push to afe cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( ( session_object->vdec ) &&
       ( VSM_TAP_POINT_STREAM_END == session_object->active_set.record.rx_tap_point )
     )
  {
    elite_msg_any_t msg_rx_t;
    uint16_t afe_port;
    elite_msg_custom_afe_connect_t *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_afe_connect_t );

    if( AFE_PORT_ID_INVALID == session_object->active_set.record.port_id )
    {
      afe_port =  AFE_PORT_ID_VOICE_RECORD_RX;
    }
    else
    {
      afe_port = session_object->active_set.record.port_id;
    }
    session_object->aud_ref_port_rx = afe_port;

    rc = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_connect_afe: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = ( elite_msg_custom_afe_connect_t* ) msg_rx_t.pPayload;
    payload_ptr->sec_op_code = ELITEMSG_CUSTOM_AFECONNECT_REQ;
    payload_ptr->afe_id = afe_port;
    payload_ptr->afe_client = afe_client;

    rc = qurt_elite_queue_push_back( session_object->vsm_afe_cmd_q_ptr, ( uint64_t* )&msg_rx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_connect_afe: FAILED to push to afe cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_connect_afe: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_set_voc_param(
  vsm_session_object_t* session_object,
  vss_istream_voc_param_data_t* param_ptr
)
{
  ADSPResult rc = ADSP_EOK;

  if ( NULL == session_object || NULL == param_ptr )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_voc_param: media_id 0x%08X, param_id 0x%08X\n",
         param_ptr->media_id, param_ptr->param_id );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_voc_param: direction_id %d, param_size %d\n",
         param_ptr->direction_id, param_ptr->param_size );

  if ( param_ptr->direction_id == VSS_ISTREAM_DIRECTION_TX ||
       param_ptr->direction_id == VSS_ISTREAM_DIRECTION_TX_RX )
  {
    if ( session_object->venc )
    {
      elite_msg_any_t msg_tx_t;
      uint32_t size = sizeof( elite_msg_custom_set_voc_param_type );

      MSG(MSG_SSID_DFLT, MSG_LEGACY_MED,"vsm_action_set_voc_param..." );

      rc = elite_msg_create_msg( &msg_tx_t,
                 &size,
                 ELITE_CUSTOM_MSG,
                 session_object->cmd_control.respQ,
                 VSM_ASYNC_CMD_TOKEN_TX,
                 ADSP_EOK );

      if( ADSP_FAILED( rc ) )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_voc_param: FAILED to create EliteMsg!!\n" );
        return rc;
      }
      elite_msg_custom_set_voc_param_type* payload_ptr = ( elite_msg_custom_set_voc_param_type* ) msg_tx_t.pPayload;
      payload_ptr->sec_opcode = VENC_SET_VOC_PARAM;;
      payload_ptr->payload_address = ( void* )param_ptr;
      payload_ptr->payload_size = param_ptr->param_size;

      if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->venc->cmdQ,( uint64_t * )&msg_tx_t ) ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_voc_param:"
               " Unable to push to enc cmdQ, error = %d!!\n", ( int )rc );
        elite_msg_return_payload_buffer( &msg_tx_t );
        return( rc );
      }
      session_object->cmd_control.issued_count++;
      session_object->cmd_control.is_resp_pending = TRUE;
    }
  }

  if ( param_ptr->direction_id == VSS_ISTREAM_DIRECTION_RX ||
       param_ptr->direction_id == VSS_ISTREAM_DIRECTION_TX_RX )
  {
    if ( session_object->vdec )
    {
      elite_msg_any_t msg_rx_t;
      uint32_t size = sizeof( elite_msg_custom_set_voc_param_type );
      rc = elite_msg_create_msg( &msg_rx_t,
                &size,
                ELITE_CUSTOM_MSG,
                session_object->cmd_control.respQ,
                VSM_ASYNC_CMD_TOKEN_RX,
                ADSP_EOK );
      if( ADSP_FAILED( rc ) )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_voc_param: FAILED to create EliteMsg!!\n" );
        return rc;
      }
      elite_msg_custom_set_voc_param_type* payload_ptr = ( elite_msg_custom_set_voc_param_type* ) msg_rx_t.pPayload;
      payload_ptr->sec_opcode = VDEC_SET_VOC_PARAM;;
      payload_ptr->payload_address = ( void* )param_ptr;
      payload_ptr->payload_size = param_ptr->param_size;

      if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ,( uint64_t * )&msg_rx_t ) ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_voc_param:"
              " Unable to push to dec cmdQ, error = %d!!\n", ( int )rc );
        elite_msg_return_payload_buffer( &msg_rx_t );
        return( rc );
      }
      session_object->cmd_control.issued_count++;
      session_object->cmd_control.is_resp_pending = TRUE;
    }
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_voc_param: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_set_param(
  vsm_session_object_t* session_object,
  int32_t* get_param_ptr,
  uint32_t payload_size,
  uint32_t version
)
{
  ADSPResult rc = ADSP_EOK;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  if( session_object->venc )
  {
    elite_msg_any_t msg_tx_t;
    uint32_t size = sizeof( elite_msg_param_cal_t );

    MSG(MSG_SSID_DFLT, MSG_LEGACY_MED,"vsm_action_set_param..." );

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CMD_SET_PARAM,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );

    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_param: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    elite_msg_param_cal_t* payload_ptr = ( elite_msg_param_cal_t* ) msg_tx_t.pPayload;
    if (CVD_CAL_PARAM_MINOR_VERSION_0 == version )
    {
      payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL;
    }
    else
    {
      payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL_V2;
    }
    payload_ptr->pnParamData = (int32_t*)get_param_ptr;
    payload_ptr->unSize = payload_size;

    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->venc->cmdQ,( uint64_t * )&msg_tx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_param:"
            " Unable to push to enc cmdQ, error = %d!!\n", ( int )rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return( rc );
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec )
  {
    elite_msg_any_t msg_rx_t;
    uint32_t size = sizeof( elite_msg_param_cal_t );
    rc = elite_msg_create_msg( &msg_rx_t,
              &size,
              ELITE_CMD_SET_PARAM,
              session_object->cmd_control.respQ,
              VSM_ASYNC_CMD_TOKEN_RX,
              ADSP_EOK );
    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_param: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    elite_msg_param_cal_t* payload_ptr = ( elite_msg_param_cal_t* ) msg_rx_t.pPayload;
    if (CVD_CAL_PARAM_MINOR_VERSION_0 == version )
    {
      payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL;
    }
    else
    {
      payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL_V2;
    }
    payload_ptr->pnParamData = (int32_t*)get_param_ptr;
    payload_ptr->unSize = payload_size;

    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ,( uint64_t * )&msg_rx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_param:"
            " Unable to push to dec cmdQ, error = %d!!\n", ( int )rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return( rc );
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_param: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_get_param(
  vsm_session_object_t* session_object,
  int32_t* get_param_ptr,
  bool_t is_inband,
  uint32_t version
)
{
  ADSPResult rc = ADSP_EOK;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  if( session_object->venc )
  {
    elite_msg_any_t msg_tx_t;
    uint32_t size = sizeof( elite_msg_param_cal_t );
    uint32_t token;

    if ( TRUE ==  is_inband )
    {
      token = VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_TX;
    }
    else
    {
      token = VSM_ASYNC_CMD_TOKEN_PARAM_OOB_TX;
    }

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CMD_GET_PARAM,
               session_object->cmd_control.respQ,
               token,
               ADSP_EOK );

    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_get_param: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    elite_msg_param_cal_t* payload_ptr = ( elite_msg_param_cal_t* ) msg_tx_t.pPayload;
    
    if (CVD_CAL_PARAM_MINOR_VERSION_0 == version )
    {
      payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL;
    }
    else
    {
      payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL_V2;
    }
    payload_ptr->pnParamData = (int32_t*)get_param_ptr;

    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->venc->cmdQ,( uint64_t * )&msg_tx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_get_param:"
            " Unable to push to enc cmdQ, error = %d!!\n", ( int )rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return( rc );
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec )
  {
    elite_msg_any_t msg_rx_t;
    uint32_t size = sizeof( elite_msg_param_cal_t );
    uint32_t token;

    if ( TRUE ==  is_inband )
    {
      token = VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_RX;
    }
    else
    {
      token = VSM_ASYNC_CMD_TOKEN_PARAM_OOB_RX;
    }

    rc = elite_msg_create_msg( &msg_rx_t,
              &size,
              ELITE_CMD_GET_PARAM,
              session_object->cmd_control.respQ,
              token,
              ADSP_EOK );
    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_get_param: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    elite_msg_param_cal_t* payload_ptr = ( elite_msg_param_cal_t* ) msg_rx_t.pPayload;
    
    if (CVD_CAL_PARAM_MINOR_VERSION_0 == version )
    {
      payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL;
    }
    else
    {
      payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL_V2;
    }
    payload_ptr->pnParamData = (int32_t*)get_param_ptr;
    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ,( uint64_t * )&msg_rx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_get_param:"
            " Unable to push to dec cmdQ, error = %d!!\n", ( int )rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return( rc );
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_get_param: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_send_start_stop_host_pcm(
  vsm_session_object_t* session_object,
  elite_apr_packet_t* apr_pkt
)
{
  ADSPResult rc = ADSP_EOK;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  if( session_object->venc )
  {
    elite_msg_any_t msg_tx_t;
    uint32_t size = sizeof( elite_msg_custom_voc_config_host_pcm_type );

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );

    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_send_start_host_pcm: FAILED to create EliteMsg!!\n" );
      return rc;
    }

    elite_msg_custom_voc_config_host_pcm_type* payload_ptr = ( elite_msg_custom_voc_config_host_pcm_type* ) msg_tx_t.pPayload;
    payload_ptr->sec_opcode        = VENC_CONFIG_HOST_PCM;
    payload_ptr->apr_handle        = vsm_apr_handle;
    payload_ptr->apr_packet_ptr    = apr_pkt;

    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->venc->cmdQ,( uint64_t * )&msg_tx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_send_start_host_pcm:"
            " Unable to push to enc cmdQ, error = %d!!\n", ( int )rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return( rc );
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec )
  {
    elite_msg_any_t msg_rx_t;
    uint32_t size = sizeof( elite_msg_custom_voc_config_host_pcm_type );
    rc = elite_msg_create_msg( &msg_rx_t,
              &size,
              ELITE_CUSTOM_MSG,
              session_object->cmd_control.respQ,
              VSM_ASYNC_CMD_TOKEN_RX,
              ADSP_EOK );
    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_send_start_host_pcm: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    elite_msg_custom_voc_config_host_pcm_type* payload_ptr = ( elite_msg_custom_voc_config_host_pcm_type* ) msg_rx_t.pPayload;
    payload_ptr->sec_opcode        = VDEC_CONFIG_HOST_PCM;
    payload_ptr->apr_handle        = vsm_apr_handle;
    payload_ptr->apr_packet_ptr    = apr_pkt;

    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ,( uint64_t * )&msg_rx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_send_start_host_pcm:"
            " Unable to push to dec cmdQ, error = %d!!\n", ( int )rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return( rc );
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_send_start_host_pcm: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_set_enc_rate(
  vsm_session_object_t* session_object,
  uint32_t rate
)
{
  ADSPResult rc = ADSP_EOK;

  if( session_object->venc )
  {
    elite_msg_any_t msg_tx_t;
    uint32_t size = sizeof( elite_msg_custom_set_enc_rate_type );

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );

    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_enc_rate: FAILED to create EliteMsg!!\n" );
      return rc;
    }

    elite_msg_custom_set_enc_rate_type* payload_ptr = ( elite_msg_custom_set_enc_rate_type* ) msg_tx_t.pPayload;
    payload_ptr->sec_opcode = VENC_SET_ENC_RATE;
    payload_ptr->media_type = session_object->active_set.media_id;
    payload_ptr->encoder_rate = rate;

    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->venc->cmdQ,( uint64_t * )&msg_tx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_enc_rate:"
            " Unable to push to enc cmdQ, error = %d!!\n", ( int )rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return( rc );
    }
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_enc_rate: cmd issued enc_rate = %d", rate );

  return ADSP_EOK;

}

void vsm_action_free_session_resources(
  vsm_session_object_t* session_object
)
{
  uint32_t result = ADSP_EOK;
  elite_apr_packet_t* apr_pkt = NULL;
  elite_msg_any_t  cmd_msg;

  if ( NULL != session_object->vdec )
  {
    vsm_utility_destroy_service( session_object, session_object->vdec );
  }

  if ( NULL != session_object->venc )
  {
    vsm_utility_destroy_service( session_object, session_object->venc );
  }

  while ( TRUE )
  {
    /*Flush out any pending command from Queue, as session is getting destroyed.  */
    result = qurt_elite_queue_pop_front( session_object->cmd_control.cmdQ,
                                         (uint64_t*)&cmd_msg );
    if( ADSP_EOK != result )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM: session command queue is flused " );
      break;
    }

    apr_pkt = ( elite_apr_packet_t* )cmd_msg.pPayload;

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: Flushing command opcode[0x%08x]", apr_pkt->opcode );
    elite_apr_if_end_cmd( vsm_apr_handle, apr_pkt, ADSP_EHANDLE );
  }

  vsm_utility_free_session_object( &session_object );

  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "vsm_action_free_session_resources: freed all session resources" );
}

ADSPResult vsm_action_clear_oob_configuration( vsm_session_object_t* session_object )
{
  session_object->packet_exchange_info.mode = VSS_IPKTEXG_MODE_IN_BAND;
  session_object->packet_exchange_info.oob_info.is_configured = FALSE;
  session_object->packet_exchange_info.oob_info.config.mem_handle = 0;
  session_object->packet_exchange_info.oob_info.config.enc_buf_addr_lsw = 0;
  session_object->packet_exchange_info.oob_info.config.enc_buf_addr_msw = 0;
  session_object->packet_exchange_info.oob_info.config.enc_buf_size = 0;
  session_object->packet_exchange_info.oob_info.config.dec_buf_addr_lsw = 0;
  session_object->packet_exchange_info.oob_info.config.dec_buf_addr_msw = 0;
  session_object->packet_exchange_info.oob_info.config.dec_buf_size = 0;

  return ADSP_EOK;
}

ADSPResult vsm_action_clear_mailbox_configuration( vsm_session_object_t* session_object )
{
  session_object->packet_exchange_info.mode = VSS_IPKTEXG_MODE_IN_BAND;
  session_object->packet_exchange_info.mailbox_info.is_configured = FALSE;
  session_object->packet_exchange_info.mailbox_info.config.cvs_mem_handle = APR_NULL_V;
  session_object->packet_exchange_info.mailbox_info.config.tx_circ_buf = NULL;
  session_object->packet_exchange_info.mailbox_info.config.tx_circ_buf_mem_size = 0;
  session_object->packet_exchange_info.mailbox_info.config.rx_circ_buf = NULL;
  session_object->packet_exchange_info.mailbox_info.config.rx_circ_buf_mem_size = 0;
  session_object->packet_exchange_info.mailbox_info.config.max_enc_pkt_size = 0;
  session_object->packet_exchange_info.mailbox_info.config.max_dec_pkt_size = 0;

  return ADSP_EOK;
}

ADSPResult vsm_action_reconfig_mailbox_timing( vsm_session_object_t* session_object )
{
  //TBD: implement this.
  return ADSP_EOK;
}

void vsm_action_ssr_clear_event_class_registrations_oob_tty( vsm_session_object_t* session_object, uint8_t ssr_domain_id )
{

    /* Clear all event class registrations and oob tty registrations for the domain which is restarted.*/
    if ( ( TRUE == session_object->eamr_mode_change_notification_info.is_enabled ) &&
         ( APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, session_object->eamr_mode_change_notification_info.client_addr ) == ssr_domain_id ) )
    {
    /* Reset eamr_mode_change_notification_info. */
    session_object->eamr_mode_change_notification_info.is_enabled = FALSE;
    session_object->eamr_mode_change_notification_info.client_addr = APRV2_PKT_INIT_ADDR_V;
    session_object->eamr_mode_change_notification_info.client_port = APR_NULL_V;
    }

    if ( ( TRUE == session_object->evs_bandwidth_change_notification_info.is_enabled ) && 
         ( APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, session_object->evs_bandwidth_change_notification_info.client_addr ) == ssr_domain_id ) )
    {
    /* Reset evs_bandwidth_change_notification_info. */
    session_object->evs_bandwidth_change_notification_info.is_enabled = FALSE;
    session_object->evs_bandwidth_change_notification_info.client_addr = APRV2_PKT_INIT_ADDR_V;
    session_object->evs_bandwidth_change_notification_info.client_port = APR_NULL_V;
    session_object->evs_bandwidth_change_notification_info.last_sent_rx_bandwidth = VSM_STREAM_VOC_BANDWIDTH_EVT_NOT_SET;
    }

    if ( ( TRUE == session_object->avsync_client_rx_info.is_enabled ) && 
         ( APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, session_object->avsync_client_rx_info.client_addr ) == ssr_domain_id ) )
    {
      /* Reset avsync_client_info. */
      session_object->avsync_client_rx_info.is_enabled = FALSE;
      session_object->avsync_client_rx_info.client_addr = APR_NULL_V;
      session_object->avsync_client_rx_info.client_port = APR_NULL_V;
    }

    if ( ( TRUE == session_object->avsync_client_tx_info.is_enabled ) && 
         ( APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, session_object->avsync_client_tx_info.client_addr ) == ssr_domain_id ) )
    {
      /* Reset avsync_client_tx_info. */
      session_object->avsync_client_tx_info.is_enabled = FALSE;
      session_object->avsync_client_tx_info.client_addr = APR_NULL_V;
      session_object->avsync_client_tx_info.client_port = APR_NULL_V;
    }

    if( ( TRUE == session_object->tty_info.is_ittyoob_registered ) && 
        ( APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, session_object->tty_info.ittyoob_client_addr ) == ssr_domain_id ) )
    {
      session_object->tty_info.is_ittyoob_registered = FALSE;
      /* Clearing the Client Address/Port for routing the OOBTTY EVT for Tx. */
      session_object->tty_info.ittyoob_client_addr = APR_NULL_V;
      session_object->tty_info.ittyoob_client_port = APR_NULL_V;
      session_object->is_stream_config_changed = TRUE;
    }

return;
}

ADSPResult vsm_action_get_avsync_delays(
  vsm_session_object_t* session_object
)
{
  ADSPResult rc_tx = ADSP_EOK;
  ADSPResult rc_rx = ADSP_EOK;
  elite_msg_any_t msg_tx_t, msg_rx_t;
  elite_msg_custom_delay_type* payload_ptr;
  vsm_get_delay_ack_t* resp_payload;
  uint32_t size;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  resp_payload = ( vsm_get_delay_ack_t* )qurt_elite_memory_malloc(
                                   sizeof( vsm_get_delay_ack_t ), QURT_ELITE_HEAP_DEFAULT );
  if ( NULL == resp_payload )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vsm_action_get_avsync_delays: failed to allocate memory for set_param..." );
    return ADSP_ENORESOURCE;
  }

  while( session_object->venc )
  {
    size = sizeof( elite_msg_custom_delay_type );
    rc_tx = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( rc_tx))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_get_avsync_delays:"
            " FAILED to create  EliteMsg,elite_msg_custom_kpps_type  session_port0x%X )",
            ( int )session_object->apr_info.self_port );
      break;
    }
    payload_ptr = (elite_msg_custom_delay_type*) msg_tx_t.pPayload;
    payload_ptr->delay_ptr = ( void* )resp_payload;
    payload_ptr->sec_opcode = VENC_GET_DELAY_CMD;

    if ( ADSP_FAILED( rc_tx = qurt_elite_queue_push_back( session_object->venc->cmdQ,( uint64_t * )&msg_tx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_get_avsync_delays:"
             " Unable to push to venc cmdQ, error = %d!!\n",( int )rc_tx );
      elite_msg_return_payload_buffer( &msg_tx_t );
      break;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
    break;
  }

  while( session_object->vdec != NULL )
  {
    size = sizeof( elite_msg_custom_delay_type );
    rc_rx = elite_msg_create_msg( &msg_rx_t,
              &size,
              ELITE_CUSTOM_MSG,
              session_object->cmd_control.respQ,
              VSM_ASYNC_CMD_TOKEN_RX,
              ADSP_EOK );
    if( ADSP_FAILED( rc_rx  ))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_get_avsync_delays:"
             " FAILED to create elite_msg_custom_kpps_type EliteMsg, session_port 0x%X)",
             ( int ) session_object->apr_info.self_port );
      break;
    }

    payload_ptr = (elite_msg_custom_delay_type*) msg_rx_t.pPayload;
    payload_ptr->delay_ptr = ( void* )resp_payload;
    payload_ptr->sec_opcode = VDEC_GET_DELAY_CMD;

    if ( ADSP_FAILED( rc_rx = qurt_elite_queue_push_back( session_object->vdec->cmdQ,( uint64_t * )&msg_rx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_get_avsync_delays:"
           " Unable to push to vdec cmdQ, error = %d!!\n", ( int ) rc_rx );
      elite_msg_return_payload_buffer( &msg_rx_t );
      break;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
    break;
  }

  if ( 0 == session_object->cmd_control.issued_count )
  {
    qurt_elite_memory_free( ( void* )resp_payload );
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_get_avsync_delays: cmd issued count %d, resp count %d\n",
       session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ( rc_rx |rc_tx );

}

ADSPResult vsm_action_set_voice_timing(
  vsm_session_object_t* session_object
)
{
  ADSPResult rc_tx = ADSP_EOK;
  ADSPResult rc_rx = ADSP_EOK;
  elite_msg_any_t msg_tx_t, msg_rx_t;
  elite_msg_custom_voc_timing_param_type* payload_ptr;
  int32_t* timing_params_ptr = NULL;
  uint32_t size;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  vsm_utility_allocate_timing_payload( session_object, &timing_params_ptr );
  if ( NULL == timing_params_ptr )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: ACTION_SET_VOICE_TIMING: failed to allocate memory for set_param..." );
    return ADSP_ENORESOURCE;
  }

  while( session_object->venc )
  {
    size = sizeof( elite_msg_custom_voc_timing_param_type );
    rc_tx = elite_msg_create_msg( &msg_tx_t, &size, ELITE_CUSTOM_MSG,
                                  session_object->cmd_control.respQ,
                                  VSM_ASYNC_CMD_TOKEN_TX, ADSP_EOK );
    if( ADSP_FAILED( rc_tx ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: ACTION_SET_VOICE_TIMING: FAILED to create  EliteMsg  session_port0x%X )",
            ( int )session_object->apr_info.self_port );
      break;
    }

    payload_ptr = ( elite_msg_custom_voc_timing_param_type* ) msg_tx_t.pPayload;
    payload_ptr->param_data_ptr = ( int32_t* )timing_params_ptr;
    payload_ptr->sec_opcode = VENC_SET_TIMINGV3_CMD;

    if ( ADSP_FAILED( rc_tx = qurt_elite_queue_push_back( session_object->venc->cmdQ,( uint64_t * )&msg_tx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: ACTION_SET_VOICE_TIMING: Unable to push to venc cmdQ, "
             "error = %d!!\n",( int )rc_tx );
      elite_msg_return_payload_buffer( &msg_tx_t );
      break;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
    break;
  }

  while( session_object->vdec != NULL )
  {
    size = sizeof( elite_msg_custom_voc_timing_param_type );
    rc_rx = elite_msg_create_msg( &msg_rx_t, &size, ELITE_CUSTOM_MSG,
                                  session_object->cmd_control.respQ,
                                  VSM_ASYNC_CMD_TOKEN_RX, ADSP_EOK );
    if( ADSP_FAILED( rc_rx  ))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: ACTION_SET_VOICE_TIMING: FAILED to create EliteMsg, session_port 0x%X)",
             ( int ) session_object->apr_info.self_port );
      break;
    }

    payload_ptr = (elite_msg_custom_voc_timing_param_type*) msg_rx_t.pPayload;
    payload_ptr->param_data_ptr = ( int32_t* )timing_params_ptr;
    payload_ptr->sec_opcode = VDEC_SET_TIMINGV3_CMD;

    if ( ADSP_FAILED( rc_rx = qurt_elite_queue_push_back( session_object->vdec->cmdQ,( uint64_t * )&msg_rx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: ACTION_SET_VOICE_TIMING: Unable to push to vdec cmdQ, "
             "error = %d!!\n", ( int ) rc_rx );
      elite_msg_return_payload_buffer( &msg_rx_t );
      break;
    }

    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
    break;
  }

  if ( 0 == session_object->cmd_control.issued_count )
  {
    qurt_elite_memory_free( ( void* )timing_params_ptr );
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: ACTION_SET_VOICE_TIMING: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ( rc_rx |rc_tx );
}

ADSPResult vsm_action_get_kpps(
  vsm_session_object_t* session_object
)
{
  ADSPResult rc_tx = ADSP_EOK;
  ADSPResult rc_rx = ADSP_EOK;
  elite_msg_any_t msg_tx_t, msg_rx_t;
  elite_msg_custom_kpps_type* payload_ptr;
  vsm_get_kpps_ack_t* resp_payload;
  uint32_t size;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  resp_payload = ( vsm_get_kpps_ack_t* )qurt_elite_memory_malloc(
                                   sizeof( vsm_get_kpps_ack_t ), QURT_ELITE_HEAP_DEFAULT );
  if ( NULL == resp_payload )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vsm_action_get_kpps: failed to allocate memory for getkpps payload..." );
    return ADSP_ENORESOURCE;
  }

  while( session_object->venc )
  {
    size = sizeof( elite_msg_custom_kpps_type );
    rc_tx = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( rc_tx))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_get_kpps:"
            " FAILED to create  EliteMsg,elite_msg_custom_kpps_type  session_port0x%X )",
            ( int )session_object->apr_info.self_port );
      break;
    }
    payload_ptr = (elite_msg_custom_kpps_type*) msg_tx_t.pPayload;
    payload_ptr->pnKpps = ( void* )resp_payload;
    payload_ptr->sec_opcode = VENC_GET_KPPS_CMD;

    if ( ADSP_FAILED( rc_tx = qurt_elite_queue_push_back( session_object->venc->cmdQ,( uint64_t * )&msg_tx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_get_kpps:"
             " Unable to push to venc cmdQ, error = %d!!\n",( int )rc_tx );
      elite_msg_return_payload_buffer( &msg_tx_t );
      break;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
    break;
  }

  while( session_object->vdec != NULL )
  {
    size = sizeof( elite_msg_custom_kpps_type );
    rc_rx = elite_msg_create_msg( &msg_rx_t,
              &size,
              ELITE_CUSTOM_MSG,
              session_object->cmd_control.respQ,
              VSM_ASYNC_CMD_TOKEN_RX,
              ADSP_EOK );
    if( ADSP_FAILED( rc_rx  ))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_get_kpps:"
             " FAILED to create elite_msg_custom_kpps_type EliteMsg, session_port 0x%X)",
             ( int ) session_object->apr_info.self_port );
      break;
    }

    payload_ptr = (elite_msg_custom_kpps_type*) msg_rx_t.pPayload;
    payload_ptr->pnKpps = ( void* )resp_payload;
    payload_ptr->sec_opcode = VDEC_GET_KPPS_CMD;

    if ( ADSP_FAILED( rc_rx = qurt_elite_queue_push_back( session_object->vdec->cmdQ,( uint64_t * )&msg_rx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_get_kpps:"
           " Unable to push to vdec cmdQ, error = %d!!\n", ( int ) rc_rx );
      elite_msg_return_payload_buffer( &msg_rx_t );
      break;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
    break;
  }

  if ( 0 == session_object->cmd_control.issued_count )
  {
    qurt_elite_memory_free( ( void* )resp_payload );
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_get_kpps: cmd issued count %d, resp count %d\n",
       session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ( rc_rx |rc_tx );

}

ADSPResult vsm_action_run_pause_services(
  vsm_session_object_t* session_object,
  uint32_t opcode
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  if ( session_object->venc )
  {
    elite_msg_any_t msg_tx_t;
    uint32_t size = sizeof( elite_msg_cmd_run_t );

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               opcode,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_run_pause_services: FAILED to create EliteMsg!!\n" );
      return rc;
    }

    rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_run_pause_services: FAILED to push to venc cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec )
  {
    elite_msg_any_t msg_rx_t;
    uint32_t size = sizeof( elite_msg_custom_vmx_stop_run_t );

    rc = elite_msg_create_msg( &msg_rx_t,
               &size,
               opcode,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_run_pause_services: FAILED to create EliteMsg!!\n" );
      return rc;
    }

    rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_run_pause_services: FAILED to push to vdec cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_run_pause_services: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_run_stop_ports(
  vsm_session_object_t* session_object,
  uint32_t opcode
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  if ( session_object->venc && session_object->mixer_out_port_ptr )
  {
    elite_msg_any_t msg_tx_t;
    elite_msg_custom_vmx_stop_run_t* payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_vmx_stop_run_t );

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_run_stop_ports: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = (elite_msg_custom_vmx_stop_run_t*) msg_tx_t.pPayload;
    payload_ptr->unSecOpCode = opcode;
    payload_ptr->unPortID = session_object->mixer_out_port_ptr->outport_id;
    payload_ptr->unDirection = VMX_OUTPUT_DIRECTION;

    rc = qurt_elite_queue_push_back( txVoiceMatrix->cmdQ, ( uint64_t* )&msg_tx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_run_stop_ports: FAILED to push to venc cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec && session_object->mixer_in_port_ptr )
  {
    elite_msg_any_t msg_rx_t;
    elite_msg_custom_vmx_stop_run_t *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_vmx_stop_run_t );

    rc = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_run_stop_ports: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = (elite_msg_custom_vmx_stop_run_t*) msg_rx_t.pPayload;
    payload_ptr->unSecOpCode = opcode;
    payload_ptr->unPortID = session_object->mixer_in_port_ptr->inport_id;
    payload_ptr->unDirection = VMX_INPUT_DIRECTION;

    rc = qurt_elite_queue_push_back( rxVoiceMatrix->cmdQ, ( uint64_t* )&msg_rx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_run_stop_ports: FAILED to push to vdec cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_run_stop_ports: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_set_tx_rx_mute(
  vsm_session_object_t* session_object,
  uint16_t direction,
  uint16_t tx_ramp_duration,
  uint16_t rx_ramp_duration
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  MSG_5( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_tx_rx_mute: direction %d," \
         " tx_ramp_ms: %d, rx_ramp_ms: %d, tx_mute_flag: %d, rx_mute_flag: %d", direction,
         tx_ramp_duration, rx_ramp_duration, session_object->target_set.mute.tx_mute_flag,
         session_object->target_set.mute.rx_mute_flag );

  if ( ( session_object->venc ) &&
       ( ( VSS_IVOLUME_DIRECTION_TX == direction ) ||
         ( VSM_DIRECTION_RX_TX == direction )
       )
     )
  {
    elite_msg_any_t msg_tx_t;
    elite_msg_custom_voc_set_soft_mute_type* payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_voc_set_soft_mute_type );

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_tx_rx_mute: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = (elite_msg_custom_voc_set_soft_mute_type*) msg_tx_t.pPayload;
    payload_ptr->sec_opcode = VENC_SET_MUTE_CMD;
    payload_ptr->mute       = session_object->target_set.mute.tx_mute_flag;
    payload_ptr->ramp_duration       = tx_ramp_duration;

    rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_tx_rx_mute: FAILED to push to venc cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( ( session_object->vdec ) &&
       ( ( VSS_IVOLUME_DIRECTION_RX == direction ) ||
         ( VSM_DIRECTION_RX_TX == direction )
       )
     )
  {
    elite_msg_any_t msg_rx_t;
    elite_msg_custom_voc_set_soft_mute_type *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_voc_set_soft_mute_type );

    rc = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_tx_rx_mute: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = (elite_msg_custom_voc_set_soft_mute_type*) msg_rx_t.pPayload;
    payload_ptr->sec_opcode = VDEC_SET_MUTE_CMD;
    payload_ptr->mute       = session_object->target_set.mute.rx_mute_flag;
    payload_ptr->ramp_duration = rx_ramp_duration;

    rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_tx_rx_mute: FAILED to push to vdec cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_tx_rx_mute: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_set_ui_properties(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  if ( 0 == session_object->cached_ui_properties.data_len )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_set_ui_properties:"
         " cached UI property length zero!\n" );
    return ADSP_EOK;
  }

  if ( session_object->venc )
  {
    elite_msg_any_t msg_tx_t;
    elite_msg_param_cal_t *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_param_cal_t );

    rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CMD_SET_PARAM,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_ui_properties: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = ( elite_msg_param_cal_t* ) msg_tx_t.pPayload;
    payload_ptr->unSize =  session_object->cached_ui_properties.data_len;
    payload_ptr->pnParamData = ( int32_t* ) session_object->cached_ui_properties.data;
    if ( CVD_CAL_PARAM_MINOR_VERSION_0 == session_object->cached_ui_properties.version ) 
    {
       payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL;
    }
    else
    {
       payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL_V2;
    }

    rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_ui_properties: FAILED to push to venc cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec )
  {
    elite_msg_any_t msg_rx_t;
    elite_msg_param_cal_t *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_param_cal_t );

    rc = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CMD_SET_PARAM,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_ui_properties: FAILED to create EliteMsg!!\n" );
      return rc;
    }
    payload_ptr = ( elite_msg_param_cal_t* ) msg_rx_t.pPayload;
    payload_ptr->unSize =  session_object->cached_ui_properties.data_len;
    payload_ptr->pnParamData = ( int32_t* ) session_object->cached_ui_properties.data;
    if ( CVD_CAL_PARAM_MINOR_VERSION_0 == session_object->cached_ui_properties.version ) 
    {
       payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL;
    }
    else
    {
       payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL_V2;
    }

    rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_ui_properties: FAILED to push to vdec cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_ui_properties: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_hdvoice_enable_disable(
  vsm_session_object_t* session_object,
  bool_t is_enable
)
{
  int32_t rc = ADSP_EOK;
  vsm_hdvoice_enable_param_info_t* hdvoice_enable_param_info;
  uint32_t payload_size = 0;
  uint32_t feature;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  hdvoice_enable_param_info = ( vsm_hdvoice_enable_param_info_t* )qurt_elite_memory_malloc(
                                   sizeof( vsm_hdvoice_enable_param_info_t ), QURT_ELITE_HEAP_DEFAULT );
  if ( NULL == hdvoice_enable_param_info )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vsm_action_hdvoice_enable_disable: failed to allocate memory for set_param..." );
    return ADSP_ENORESOURCE;
  }

  /* Select from Target setting for enable. For disable, select from Active setting.*/
  if ( TRUE == is_enable )
  {
    feature = session_object->target_set.system_config.feature;
  }
  else
  {
    feature = session_object->active_set.system_config.feature;
  }

  /* Get ModuleID corresponding to the feature. */
  switch ( feature )
  {
    case VSS_ICOMMON_CAL_FEATURE_BEAMR:
    {
      hdvoice_enable_param_info->param_hdr.module_id = VOICE_MODULE_BEAMR;
      break;
    }

    default:
    {
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vsm_action_hdvoice_enable_disable(): Un-supported " \
              "Feature ID (0x%08X) to disable(0)/enable(1): %u ",
              feature, is_enable);

       qurt_elite_memory_free((void *)hdvoice_enable_param_info);
       return ADSP_EUNSUPPORTED;
    }
  }

  hdvoice_enable_param_info->param_hdr.param_id = VOICE_PARAM_MOD_ENABLE;
  hdvoice_enable_param_info->param_hdr.param_size = sizeof(uint32_t);
  hdvoice_enable_param_info->param_hdr.reserved = 0;
  hdvoice_enable_param_info->enable = is_enable;
  hdvoice_enable_param_info->reserved = 0;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_hdvoice_enable_disable:"
         " Feature 0x%X, enable %d\n", feature, is_enable );

  if ( session_object->vdec )
  {
    elite_msg_any_t msg_rx_t;
    elite_msg_param_cal_t *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_param_cal_t );

    rc = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CMD_SET_PARAM,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_RX,
               ADSP_EOK);
    if( ADSP_FAILED( rc ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_hdvoice_enable_disable: FAILED to create EliteMsg!!\n" );
      qurt_elite_memory_free( ( void* )hdvoice_enable_param_info );
      return rc;
    }
    payload_ptr = ( elite_msg_param_cal_t* ) msg_rx_t.pPayload;
    payload_ptr->unSize = payload_size;
    payload_ptr->pnParamData = ( int32_t* )hdvoice_enable_param_info;
    payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL;

    rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
    if ( ADSP_FAILED( rc ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_hdvoice_enable_disable: FAILED to push to vdec cmdQ. error %d!!\n", rc );
      elite_msg_return_payload_buffer( &msg_rx_t );
      qurt_elite_memory_free( ( void* )hdvoice_enable_param_info );
      return rc;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_hdvoice_enable_disable: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_calibrate_static(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  cvd_cal_column_t columns[ VSM_NUM_COMMON_CAL_COLUMNS ];
  cvd_cal_key_t cal_key;
  cvd_cal_column_t* query_key_columns;
  uint32_t column_index = 0;
  cvd_cal_log_commit_info_t log_info;
  cvd_cal_log_cal_data_header_t log_info_data;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  for ( ;; )
  {
    session_object->common_cal.num_matching_entries = 0;
    session_object->common_cal.set_param_rsp_cnt = 0;
    session_object->common_cal.set_param_failed_rsp_cnt = 0;

    if ( session_object->static_cal.is_registered == FALSE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vsm_action_calibrate_static():"
          " Calibration data is not registered. Not calibrating." );
      rc = ADSP_EOK;
      break;
    }

    query_key_columns = session_object->static_cal.query_key_columns;

    mmstd_memset( query_key_columns, 0, sizeof( session_object->static_cal.query_key_columns ) );
    query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_NETWORK;
    query_key_columns[ column_index ].value = session_object->target_set.system_config.network_id;
    column_index += 1;

    switch ( session_object->direction )
    {
      case VSM_DIRECTION_TX:
      {
        query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
        query_key_columns[ column_index ].value = session_object->target_set.system_config.enc_sr;
        column_index += 1;

        query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE;
        query_key_columns[ column_index ].value = session_object->target_set.system_config.tx_voc_op_mode;
        column_index += 1;
      }
      break;

      case VSM_DIRECTION_RX:
      {
        query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
        query_key_columns[ column_index ].value = session_object->target_set.system_config.dec_sr;
        column_index += 1;

        query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE;
        query_key_columns[ column_index ].value = session_object->target_set.system_config.rx_voc_op_mode;
        column_index += 1;
      }
      break;

      case VSM_DIRECTION_RX_TX:
      {
        query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
        query_key_columns[ column_index ].value = session_object->target_set.system_config.enc_sr;
        column_index += 1;

        query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
        query_key_columns[ column_index ].value = session_object->target_set.system_config.dec_sr;

        column_index += 1;

        query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE;
        query_key_columns[ column_index ].value = session_object->target_set.system_config.tx_voc_op_mode;
        column_index += 1;

        query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE;
        query_key_columns[ column_index ].value = session_object->target_set.system_config.rx_voc_op_mode;
        column_index += 1;
      }
      break;

      default:
      {
        return ADSP_EFAILED;
      }
      break;
    }

    query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_MEDIA_ID;
    query_key_columns[ column_index ].value = session_object->target_set.system_config.media_id;
    column_index += 1;

    query_key_columns[ column_index ].id = VSS_ICOMMON_CAL_COLUMN_FEATURE;
    query_key_columns[ column_index ].value = session_object->target_set.system_config.feature;

    cal_key.columns = query_key_columns;
    cal_key.num_columns = ( column_index + 1 );

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_calibrate_static: configured for the following settings"
           " Network 0x%08X, Tx PP SR %d, Rx PP SR %d", session_object->target_set.system_config.network_id,
           session_object->target_set.system_config.tx_pp_sr, session_object->target_set.system_config.rx_pp_sr );

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_calibrate_static: configured for the following settings"
           " Tx voc op mode 0x%08X, Rx voc op mode 0x%08X, Media ID 0x%08X", session_object->target_set.system_config.tx_voc_op_mode,
           session_object->target_set.system_config.rx_voc_op_mode, session_object->target_set.system_config.media_id );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_calibrate_static: configured for the following settings "
           ". : Feature 0x%08X.", session_object->target_set.system_config.feature );

    rc = cvd_cal_query_init(
           session_object->static_cal.table_handle, &cal_key,
           session_object->static_cal.matching_entries,
           sizeof( session_object->static_cal.matching_entries ),
           &session_object->static_cal.query_handle );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_calibrate_static():"
             "cvd_cal_query_init failed, rc=0x%08X. Not calibrating.", rc );
      break;
    }

    if ( session_object->venc )
    {
      elite_msg_any_t msg_tx_t;
      elite_msg_custom_set_param_v3_type *payload_ptr = NULL;
      uint32_t size = sizeof( elite_msg_custom_set_param_v3_type );

      rc = elite_msg_create_msg( &msg_tx_t,
             &size,
             ELITE_CUSTOM_MSG,
             session_object->cmd_control.respQ,
             VSM_ASYNC_CMD_TOKEN_TX,
             ADSP_EOK );
      if( ADSP_FAILED( rc ) )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_calibrate_static: FAILED to create EliteMsg!!\n" );
        return rc;
      }
      payload_ptr = (elite_msg_custom_set_param_v3_type*) msg_tx_t.pPayload;
      payload_ptr->sec_opcode = VENC_SET_PARAM_V3;
      payload_ptr->cal_handle = session_object->static_cal.query_handle;

      rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );
      if ( ADSP_FAILED( rc ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_calibrate_static: FAILED to push to venc cmdQ. error %d!!\n", rc );
        elite_msg_return_payload_buffer( &msg_tx_t );
        return rc;
      }
      session_object->cmd_control.issued_count++;
      session_object->cmd_control.is_resp_pending = TRUE;
    }

    if ( session_object->vdec )
    {
      elite_msg_any_t msg_rx_t;
      elite_msg_custom_set_param_v3_type *payload_ptr = NULL;
      uint32_t size = sizeof( elite_msg_custom_set_param_v3_type );

      rc = elite_msg_create_msg( &msg_rx_t,
                 &size,
                 ELITE_CUSTOM_MSG,
                 session_object->cmd_control.respQ,
                 VSM_ASYNC_CMD_TOKEN_RX,
                 ADSP_EOK);
      if( ADSP_FAILED( rc ) )
      {
        MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_calibrate_static: FAILED to create EliteMsg!!\n" );
        return rc;
      }
      payload_ptr = (elite_msg_custom_set_param_v3_type*) msg_rx_t.pPayload;
      payload_ptr->sec_opcode = VDEC_SET_PARAM_V3;
      payload_ptr->cal_handle = session_object->static_cal.query_handle;

      rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
      if ( ADSP_FAILED( rc ) )
      {
        MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_calibrate_static: FAILED to push to vdec cmdQ. error %d!!\n", rc );
        elite_msg_return_payload_buffer( &msg_rx_t );
        return rc;
      }
      session_object->cmd_control.issued_count++;
      session_object->cmd_control.is_resp_pending = TRUE;
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_calibrate_static: cmd issued count %d, resp count %d\n",
           session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

    break;
  }

    /* Log cal data. */
    {
      log_info_data.table_handle = session_object->static_cal.table_handle;
      log_info_data.cal_query_handle = session_object->static_cal.query_handle;
      log_info_data.data_seq_num = 0;

      log_info.instance = ( ( session_object->attached_mvm_handle << 16 ) | 
                            ( session_object->apr_info.self_port ) );
      log_info.call_num = session_object->target_set.system_config.call_num;
      log_info.data_container_id = CVD_CAL_LOG_DATA_CONTAINER_RAW_CAL_OUTPUT;
      log_info.data_container_header_size = sizeof ( log_info_data );
      log_info.data_container_header = &log_info_data;
      log_info.payload_size = 0;
      log_info.payload_buf = NULL;

      cvd_cal_log_data ( ( log_code_type )LOG_ADSP_CVD_CAL_DATA_C, CVD_CAL_LOG_STREAM_STATIC_OUTPUT,
                                ( void* )&log_info, sizeof( log_info ) );
    }

  return rc;
}

/* BACKWARD COMPATIBILITY */
ADSPResult vsm_action_calibrate_common(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  cvd_cal_column_t columns[ VSM_NUM_COMMON_CAL_COLUMNS ];
  cvd_cal_key_t cal_key;
  uint32_t set_param_cnt;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  for ( ;; )
  {
    session_object->common_cal.num_matching_entries = 0;
    session_object->common_cal.set_param_rsp_cnt = 0;
    session_object->common_cal.set_param_failed_rsp_cnt = 0;

    if ( session_object->common_cal.is_registered == FALSE )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_calibrate_common():"
          " Calibration data is not registered. Not calibrating." );
      rc = ADSP_EUNEXPECTED;
      break;
    }

    if ( ( NULL == vsm_memory_map_client ) || ( NULL == session_object->common_cal.vsm_mem_handle ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_calibrate_common():"
          " Memory mapping was not done Or Invalid mem_map handle." );
      rc = ADSP_EUNEXPECTED;
      break;
    }

    mmstd_memset( columns, 0, sizeof( columns ) );
    columns[ 0 ].id = VSS_ICOMMON_CAL_COLUMN_NETWORK;
    columns[ 0 ].value = session_object->target_set.system_config.network_id;

    switch ( session_object->direction )
    {
      case VSM_DIRECTION_RX:
      {
        columns[ 1 ].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
        columns[ 1 ].value = session_object->target_set.system_config.rx_pp_sr;
        cal_key.num_columns = 2;
      }
      break;

      case VSM_DIRECTION_TX:
      {
        columns[ 1 ].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
        columns[ 1 ].value = session_object->target_set.system_config.tx_pp_sr;
        cal_key.num_columns = 2;
      }
      break;

      case VSM_DIRECTION_RX_TX:
      {
        columns[ 1 ].id = VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE;
        columns[ 2 ].id = VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE;
        columns[ 1 ].value = session_object->target_set.system_config.rx_pp_sr;
        columns[ 2 ].value = session_object->target_set.system_config.tx_pp_sr;
        cal_key.num_columns = 3;
      }
      break;

      default:
        return ADSP_EFAILED;
      break;
    }

    cal_key.columns = columns;

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_calibrate_common(): Network 0x%08X, Rx SR %d, Tx SR %d",
                                          columns[0].value,
                                          columns[1].value,
                                          columns[ 2 ].value );

    rc = cvd_cal_query_table(
           session_object->common_cal.table_handle,
           &cal_key, sizeof( session_object->common_cal.matching_entries ),
           session_object->common_cal.matching_entries,
           &session_object->common_cal.num_matching_entries );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_calibrate_common():"
             " cvd_cal_query_table failed, rc=0x%08X. Not calibrating.", rc );
      break;
    }

    if ( session_object->common_cal.num_matching_entries == 0 )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vsm_action_calibrate_common():"
           " Cannot find a matching entry. Not calibrating." );
      rc = ADSP_EOK;
      break;
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_calibrate_common(): Found %d matched entries",
           session_object->common_cal.num_matching_entries );

    for ( set_param_cnt = 0; set_param_cnt < session_object->common_cal.num_matching_entries; ++set_param_cnt )
    {
      uint32_t payload_size = session_object->common_cal.matching_entries[ set_param_cnt ].size;
      uint32_t payload_address_lsw;
      uint32_t payload_address_msw;
      uint32_t payload_virt_address = NULL;
      elite_mem_shared_memory_map_t mem_shared_memory_map = {0};

      cvd_mem_mapper_set_virt_addr_to_uint32( &payload_address_msw, &payload_address_lsw,
                                              session_object->common_cal.matching_entries[ set_param_cnt ].start_ptr );

      mem_shared_memory_map.unMemMapClient = vsm_memory_map_client;
      mem_shared_memory_map.unMemMapHandle = session_object->common_cal.vsm_mem_handle;

      rc = voice_cmn_check_align_size( payload_address_lsw, payload_address_msw, payload_size, 4 );
      if( ADSP_FAILED( rc ) )
      {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_calibrate_common: address has to be 4-byte aligned"
               " and size has to be multiple of 4 bytes, lsw(%lx) msw(%lx) size(%lx)",
               payload_address_lsw, payload_address_msw, payload_size );
        break;
      }

      rc = elite_mem_map_get_shm_attrib( payload_address_lsw,
             payload_address_msw, payload_size, &mem_shared_memory_map );
      if ( ADSP_FAILED( rc ) ) break;

      payload_virt_address = mem_shared_memory_map.unVirtAddr;
 
      if ( session_object->venc )
      {
        elite_msg_any_t msg_tx_t;
        elite_msg_param_cal_t *payload_ptr = NULL;
        uint32_t size = sizeof( elite_msg_param_cal_t );

        rc = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CMD_SET_PARAM,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
        if( ADSP_FAILED( rc ) )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_calibrate_common: FAILED to create EliteMsg!!\n" );
          return rc;
        }
        payload_ptr = (elite_msg_param_cal_t*) msg_tx_t.pPayload;
        payload_ptr->unSize = payload_size;
        payload_ptr->pnParamData = (int32_t*)payload_virt_address;
        payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL;

        rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );
        if ( ADSP_FAILED( rc ) )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_calibrate_common: FAILED to push to venc cmdQ. error %d!!\n", rc );
          elite_msg_return_payload_buffer( &msg_tx_t );
          return rc;
        }
        session_object->cmd_control.issued_count++;
        session_object->cmd_control.is_resp_pending = TRUE;
      }

      if ( session_object->vdec )
      {
        elite_msg_any_t msg_rx_t;
        elite_msg_param_cal_t *payload_ptr = NULL;
        uint32_t size = sizeof( elite_msg_param_cal_t );

        rc = elite_msg_create_msg( &msg_rx_t,
                   &size,
                   ELITE_CMD_SET_PARAM,
                   session_object->cmd_control.respQ,
                   VSM_ASYNC_CMD_TOKEN_RX,
                   ADSP_EOK);
        if( ADSP_FAILED( rc ) )
        {
          MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_calibrate_common: FAILED to create EliteMsg!!\n" );
          return rc;
        }
        payload_ptr = (elite_msg_param_cal_t*) msg_rx_t.pPayload;
        payload_ptr->unSize = payload_size;
        payload_ptr->pnParamData = (int32_t*)payload_virt_address;
        payload_ptr->unParamId = ELITEMSG_PARAM_ID_CAL;

        rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
        if ( ADSP_FAILED( rc ) )
        {
          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_calibrate_common: FAILED to push to vdec cmdQ. error %d!!\n", rc );
          elite_msg_return_payload_buffer( &msg_rx_t );
          return rc;
        }
        session_object->cmd_control.issued_count++;
        session_object->cmd_control.is_resp_pending = TRUE;
      }
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_calibrate_common: cmd issued count %d, resp count %d\n",
           session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

    break;
  }

  return rc;
}

ADSPResult vsm_action_set_oob_pkt_exchange_config(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_any_t msg_tx_t;
  elite_msg_any_t msg_rx_t;
  vsm_config_packet_exchange_t* config;
  elite_msg_custom_oob_pkt_exchange_config_type *payload_ptr = NULL;
  uint32_t size = sizeof( elite_msg_custom_oob_pkt_exchange_config_type );

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  config = ( vsm_config_packet_exchange_t* )qurt_elite_memory_malloc(
                                   sizeof( vsm_config_packet_exchange_t ), QURT_ELITE_HEAP_DEFAULT );
  if ( NULL == config )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vsm_action_set_oob_pkt_exchange_config: failed to allocate memory for oob config..." );
    return ADSP_ENORESOURCE;
  }

  *config = session_object->packet_exchange_info.oob_info.config;

  if ( session_object->venc )
  {
    result = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( result))
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_oob_pkt_exchange_config: FAILED to create EliteMsg!!\n" );
      return result;
    }
    payload_ptr = ( elite_msg_custom_oob_pkt_exchange_config_type* ) msg_tx_t.pPayload;
    payload_ptr->sec_opcode = VENC_SET_OOB_PKT_EXCHANGE_CONFIG;
    payload_ptr->param_data_ptr = config;

    result = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );
    if ( ADSP_FAILED( result ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_oob_pkt_exchange_config:"
             " FAILED to push to venc cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_tx_t);
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec )
  {
    result = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK );
    if( ADSP_FAILED( result))
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_oob_pkt_exchange_config: FAILED to create EliteMsg!!\n" );
      return result;
    }
    payload_ptr = (elite_msg_custom_oob_pkt_exchange_config_type*) msg_rx_t.pPayload;
    payload_ptr->sec_opcode = VDEC_SET_OOB_PKT_EXCHANGE_CONFIG;
    payload_ptr->param_data_ptr = config;

    result = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
    if (ADSP_FAILED(result))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_set_oob_pkt_exchange_config: FAILED to push to vdec cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( 0 == session_object->cmd_control.issued_count )
  {
    qurt_elite_memory_free( ( void* )config );
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_oob_pkt_exchange_config: cmd issued count %d, resp count %d\n",
       session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_set_packet_exchange_mode(
  vsm_session_object_t* session_object,
  uint32_t mode
)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_any_t msg_tx_t;
  elite_msg_any_t msg_rx_t;
  elite_msg_custom_pkt_exchange_mode_type *payload_ptr = NULL;
  uint32_t size = sizeof( elite_msg_custom_pkt_exchange_mode_type );

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  /**
   * Packet exchange b/w CVD and Client via Mailbox/OOB/INBAND but
   * Packet exchange b/w CVD and VCP via InBand. */
  mode = VSS_IPKTEXG_MODE_IN_BAND;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: ACTION_SET_PKTEXG_MODE: mode %d \n", mode );

  if ( session_object->venc )
  {
    result = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( result))
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: ACTION_SET_PKTEXG_MODE: FAILED to create EliteMsg!!\n" );
      return result;
    }

    payload_ptr = ( elite_msg_custom_pkt_exchange_mode_type* ) msg_tx_t.pPayload;
    payload_ptr->sec_opcode = VENC_SET_PKT_EXCHANGE_MODE;
    payload_ptr->pkt_exchange_mode = mode;

    result = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );

    if ( ADSP_FAILED( result ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: ACTION_SET_PKTEXG_MODE: FAILED to push to venc cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_tx_t);
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec )
  {
    result = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK );
    if( ADSP_FAILED( result))
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: ACTION_SET_PKTEXG_MODE: FAILED to create EliteMsg!!\n" );
      return result;
    }

    payload_ptr = ( elite_msg_custom_pkt_exchange_mode_type* )msg_rx_t.pPayload;
    payload_ptr->sec_opcode = VDEC_SET_PKT_EXCHANGE_MODE;
    payload_ptr->pkt_exchange_mode = mode;

    result = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );
    if (ADSP_FAILED(result))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: ACTION_SET_PKTEXG_MODE: FAILED to push to vdec cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: ACTION_SET_PKTEXG_MODE: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_set_cached_voc_params (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint8_t* param_data = NULL;
  vss_istream_voc_param_data_t* voc_param = NULL;
  uint32_t num_params = 0;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  num_params = session_object->stream_param_cache.num_voc_params;
  param_data = session_object->stream_param_cache.data;
  voc_param = ( vss_istream_voc_param_data_t* )param_data;

  while ( num_params )
  {
    if( session_object->venc )
    {
      elite_msg_any_t msg_tx_t;
      uint32_t size = sizeof( elite_msg_custom_set_voc_param_type );

      MSG(MSG_SSID_DFLT, MSG_LEGACY_MED,"cvs_action_set_cached_voc_params..." );

      rc = elite_msg_create_msg( &msg_tx_t,
                 &size,
                 ELITE_CUSTOM_MSG,
                 session_object->cmd_control.respQ,
                 VSM_ASYNC_CMD_TOKEN_TX,
                 ADSP_EOK );

      if( ADSP_FAILED( rc ) )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"cvs_action_set_cached_voc_params: FAILED to create EliteMsg!!\n" );
        return rc;
      }
      elite_msg_custom_set_voc_param_type* payload_ptr = ( elite_msg_custom_set_voc_param_type* ) msg_tx_t.pPayload;
      payload_ptr->sec_opcode = VENC_SET_VOC_PARAM;;
      payload_ptr->payload_address = ( void* )voc_param;
      payload_ptr->payload_size = voc_param->param_size;

      if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->venc->cmdQ,( uint64_t * )&msg_tx_t ) ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"cvs_action_set_cached_voc_params:"
              " Unable to push to enc cmdQ, error = %d!!\n", ( int )rc );
        elite_msg_return_payload_buffer( &msg_tx_t );
        return( rc );
      }
      session_object->cmd_control.issued_count++;
      session_object->cmd_control.is_resp_pending = TRUE;
    }

    if ( session_object->vdec )
    {
      elite_msg_any_t msg_rx_t;
      uint32_t size = sizeof( elite_msg_custom_set_voc_param_type );
      rc = elite_msg_create_msg( &msg_rx_t,
                &size,
                ELITE_CUSTOM_MSG,
                session_object->cmd_control.respQ,
                VSM_ASYNC_CMD_TOKEN_RX,
                ADSP_EOK );
      if( ADSP_FAILED( rc ) )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"cvs_action_set_cached_voc_params: FAILED to create EliteMsg!!\n" );
        return rc;
      }
      elite_msg_custom_set_voc_param_type* payload_ptr = ( elite_msg_custom_set_voc_param_type* ) msg_rx_t.pPayload;
      payload_ptr->sec_opcode = VDEC_SET_VOC_PARAM;;
      payload_ptr->payload_address = ( void* )voc_param;
      payload_ptr->payload_size = voc_param->param_size;

      if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( session_object->vdec->cmdQ,( uint64_t * )&msg_rx_t ) ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"cvs_action_set_cached_voc_params:"
              " Unable to push to dec cmdQ, error = %d!!\n", ( int )rc );
        elite_msg_return_payload_buffer( &msg_rx_t );
        return( rc );
      }
      session_object->cmd_control.issued_count++;
      session_object->cmd_control.is_resp_pending = TRUE;
    }

    num_params--;
    if ( num_params )
    {
      param_data += voc_param->param_size + sizeof( vss_istream_voc_param_data_t );
      voc_param = ( vss_istream_voc_param_data_t* )param_data;
    }
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "cvs_action_set_cached_voc_params: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return APR_EOK;
}

ADSPResult vsm_action_set_cached_enc_rate (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  uint32_t encoder_rate;

  switch ( session_object->active_set.media_id )
  {
    case VSS_MEDIA_ID_13K:
      encoder_rate = session_object->cached_voc_properties_qcelp13k.rate;
      break;

    case VSS_MEDIA_ID_EVRC:
      encoder_rate = session_object->cached_voc_properties_evrc.rate;
      break;

    case VSS_MEDIA_ID_4GV_NB:
      encoder_rate = session_object->cached_voc_properties_4gvnb.rate;
      break;

    case VSS_MEDIA_ID_4GV_WB:
      encoder_rate = session_object->cached_voc_properties_4gvwb.rate;
      break;

    case VSS_MEDIA_ID_4GV_NW:
      encoder_rate = session_object->cached_voc_properties_4gvnw.rate;
      break;

    case VSS_MEDIA_ID_4GV_NW2K:
      encoder_rate = session_object->cached_voc_properties_4gvnw2k.rate;
      break;

    case VSS_MEDIA_ID_AMR_NB:
      encoder_rate = session_object->cached_voc_properties_amr.rate;
      break;

    case VSS_MEDIA_ID_EAMR:
      encoder_rate = session_object->cached_voc_properties_eamr.rate;
      break;

    case VSS_MEDIA_ID_AMR_WB:
      encoder_rate = session_object->cached_voc_properties_amrwb.rate;
      break;

    default:
      encoder_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
      break;
  }

  if ( VSM_STREAM_PROPERTY_NOT_SET_UINT32 == encoder_rate )
  {
    return ADSP_EOK;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vsm_action_set_cached_enc_rate: cached enc rate 0x%x", encoder_rate );

  ( void )vsm_action_set_enc_rate( session_object, encoder_rate );
  return ADSP_EOK;
}


ADSPResult vsm_action_set_cached_enc_operating_mode (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vsm_set_evs_enc_operating_mode_t set_enc_operating_mode;
  void* apr_payload;

  switch ( session_object->active_set.media_id )
  {
    case VSS_MEDIA_ID_EVS:
      set_enc_operating_mode.mode = 
        session_object->cached_voc_properties_evs.mode;
      set_enc_operating_mode.bandwidth = 
        session_object->cached_voc_properties_evs.bandwidth;
      break;

    /* This command applies only to EVS vocoder. 
       Hence avoiding to set for other vocoders.
    */
    default:
      set_enc_operating_mode.mode = VSM_STREAM_PROPERTY_NOT_SET_UINT8;
      set_enc_operating_mode.bandwidth = VSM_STREAM_PROPERTY_NOT_SET_UINT8;
      break;
  }

  if ( ( VSM_STREAM_PROPERTY_NOT_SET_UINT8 == set_enc_operating_mode.mode ) || 
       ( VSM_STREAM_PROPERTY_NOT_SET_UINT8 == set_enc_operating_mode.bandwidth )
     )
  {
    return ADSP_EOK;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: SET_ENC_OPERATING_MODE: cached mode 0x%x, bandwidth 0x%x",
         set_enc_operating_mode.mode, set_enc_operating_mode.bandwidth );

  elite_apr_packet_t* apr_pkt;
  elite_msg_any_t     msg;

  rc = elite_apr_if_alloc_cmd(
         vsm_apr_handle,
         session_object->apr_info.self_addr, session_object->apr_info.self_port,
         session_object->apr_info.self_addr, session_object->apr_info.self_port,
         VSM_ASYNC_CMD_TOKEN_TX, VSM_CMD_SET_EVS_ENC_OPERATING_MODE,
         sizeof( set_enc_operating_mode ), &apr_pkt );
  if( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_cached_enc_operating_mode:"
           " FAILED to allocate APR packet, session_port0x%X )",
          ( int )session_object->apr_info.self_port );
   return ADSP_EFAILED;
  }
  apr_payload = ( void* )( elite_apr_if_get_payload_ptr( apr_pkt ) );
  memscpy( apr_payload, sizeof( set_enc_operating_mode ), &set_enc_operating_mode, sizeof( set_enc_operating_mode ) );

  msg.unOpCode = ELITE_APR_PACKET;
  msg.pPayload = (void *) apr_pkt;
  rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg );
  if ( ADSP_FAILED( rc ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_set_cached_enc_operating_mode: FAILED to set enc operating mode on tx!!\n" );
    elite_msg_return_payload_buffer( &msg );
    return rc;
  }

  session_object->cmd_control.issued_count++;
  return ADSP_EOK;
}


ADSPResult vsm_action_set_cached_channel_aware_mode (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vsm_set_evs_enc_channel_aware_mode_enable_t set_enc_channel_aware_mode;
  vsm_enc_channel_aware_mode_params_t vsm_cached_params;
  void* apr_payload;

  switch ( session_object->active_set.media_id )
  {
    case VSS_MEDIA_ID_EVS:
    {
      vsm_cached_params.channel_aware_enabled =
        session_object->cached_voc_properties_evs.channel_aware_enabled;
      vsm_cached_params.fec_offset = 
        session_object->cached_voc_properties_evs.fec_offset;
      vsm_cached_params.fer_rate = 
        session_object->cached_voc_properties_evs.fer_rate;
    }
    break;

    default:
    {
      /* This command applies only to EVS vocoder. */
      vsm_cached_params.channel_aware_enabled = VSM_STREAM_PROPERTY_NOT_APPLICABLE_UINT8;
      vsm_cached_params.fec_offset = VSM_STREAM_PROPERTY_NOT_APPLICABLE_UINT8;
      vsm_cached_params.fer_rate = VSM_STREAM_PROPERTY_NOT_APPLICABLE_UINT8;
    }
    break;
  }

  if ( ( VSM_STREAM_PROPERTY_NOT_SET_UINT8 == vsm_cached_params.channel_aware_enabled ) ||
       ( VSM_STREAM_PROPERTY_NOT_APPLICABLE_UINT8 == vsm_cached_params.channel_aware_enabled) )
  {
    return ADSP_EOK;
  }

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: ACTION_SET_CHANNEL_AWARENESS: is_enabled[0x%x] fec_offset[0x%x] fer_rate[0x%x]",
         vsm_cached_params.channel_aware_enabled, vsm_cached_params.fec_offset,
         vsm_cached_params.fer_rate );

  elite_apr_packet_t* apr_pkt;
  elite_msg_any_t     msg;

  if ( vsm_cached_params.channel_aware_enabled )
  {
    set_enc_channel_aware_mode.fec_offset = vsm_cached_params.fec_offset;
    set_enc_channel_aware_mode.fer_rate = vsm_cached_params.fer_rate;

    rc = elite_apr_if_alloc_cmd(
           vsm_apr_handle,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           VSM_ASYNC_CMD_TOKEN_TX, VSM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE,
           sizeof( vsm_set_evs_enc_channel_aware_mode_enable_t ), &apr_pkt );
    if( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_cached_channel_aware_mode:"
             " FAILED to allocate APR packet, session_port0x%X )",
            ( int )session_object->apr_info.self_port );
     return ADSP_EFAILED;
    }
    apr_payload = ( void* )( elite_apr_if_get_payload_ptr( apr_pkt ) );
    memscpy( apr_payload, sizeof( vsm_set_evs_enc_channel_aware_mode_enable_t ), 
      &set_enc_channel_aware_mode, sizeof( vsm_set_evs_enc_channel_aware_mode_enable_t ) );
  }
  else
  {
    rc = elite_apr_if_alloc_cmd(
           vsm_apr_handle,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           VSM_ASYNC_CMD_TOKEN_TX, VSM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_DISABLE,
           0, &apr_pkt );
    if( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_cached_channel_aware_mode:"
             " FAILED to allocate APR packet, session_port0x%X )",
            ( int )session_object->apr_info.self_port );
     return ADSP_EFAILED;
    }
  }

  msg.unOpCode = ELITE_APR_PACKET;
  msg.pPayload = (void *) apr_pkt;
  rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg );
  if ( ADSP_FAILED( rc ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_set_cached_channel_aware_mode: FAILED to set channel aware mode on tx!!\n" );
    elite_msg_return_payload_buffer( &msg );
    return rc;
  }

  session_object->cmd_control.issued_count++;
  return ADSP_EOK;
}


ADSPResult vsm_action_set_cached_minmax_rate (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vsm_enc_set_minmax_rate_t minmax_rate;
  void* apr_payload;

  minmax_rate.media_type = session_object->active_set.media_id;

  switch ( session_object->active_set.media_id )
  {
    case VSS_MEDIA_ID_13K:
      minmax_rate.min_rate = session_object->cached_voc_properties_qcelp13k.min_rate;
      minmax_rate.max_rate = session_object->cached_voc_properties_qcelp13k.max_rate;
      break;

    case VSS_MEDIA_ID_EVRC:
      minmax_rate.min_rate = session_object->cached_voc_properties_evrc.min_rate;
      minmax_rate.max_rate = session_object->cached_voc_properties_evrc.max_rate;
      break;

    case VSS_MEDIA_ID_4GV_NB:
      minmax_rate.min_rate = session_object->cached_voc_properties_4gvnb.min_rate;
      minmax_rate.max_rate = session_object->cached_voc_properties_4gvnb.max_rate;
      break;

    case VSS_MEDIA_ID_4GV_WB:
      minmax_rate.min_rate = session_object->cached_voc_properties_4gvwb.min_rate;
      minmax_rate.max_rate = session_object->cached_voc_properties_4gvwb.max_rate;
      break;

    case VSS_MEDIA_ID_4GV_NW:
      minmax_rate.min_rate = session_object->cached_voc_properties_4gvnw.min_rate;
      minmax_rate.max_rate = session_object->cached_voc_properties_4gvnw.max_rate;
      break;

    case VSS_MEDIA_ID_4GV_NW2K:
      minmax_rate.min_rate = session_object->cached_voc_properties_4gvnw2k.min_rate;
      minmax_rate.max_rate = session_object->cached_voc_properties_4gvnw2k.max_rate;
      break;

    /* This property is applicable only for CDMA vocoders.*/
    default:
      minmax_rate.min_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
      minmax_rate.max_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
      break;
  }

  if ( ( VSM_STREAM_PROPERTY_NOT_SET_UINT32 == minmax_rate.min_rate ) ||
       ( VSM_STREAM_PROPERTY_NOT_SET_UINT32 == minmax_rate.max_rate )
     )
  {
    return ADSP_EOK;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: SET_MINMAX_RATE: cached min_rate 0x%x, max_rate 0x%x",
         minmax_rate.min_rate, minmax_rate.max_rate );

  elite_apr_packet_t* apr_pkt;
  elite_msg_any_t     msg;

  rc = elite_apr_if_alloc_cmd(
         vsm_apr_handle,
         session_object->apr_info.self_addr, session_object->apr_info.self_port,
         session_object->apr_info.self_addr, session_object->apr_info.self_port,
         VSM_ASYNC_CMD_TOKEN_TX, VSM_CMD_ENC_SET_MINMAX_RATE,
         sizeof( minmax_rate ), &apr_pkt );
  if( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_cached_minmax_rate:"
           " FAILED to allocate APR packet, session_port0x%X )",
          ( int )session_object->apr_info.self_port );
   return ADSP_EFAILED;
  }

  apr_payload = ( void* )( elite_apr_if_get_payload_ptr( apr_pkt ) );
  memscpy( apr_payload, sizeof( minmax_rate ), &minmax_rate, sizeof( minmax_rate ) );

  msg.unOpCode = ELITE_APR_PACKET;
  msg.pPayload = (void *) apr_pkt;
  rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg );
  if ( ADSP_FAILED( rc ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_set_cached_minmax_rate: FAILED to set min-max rate on tx!!\n" );
    elite_msg_return_payload_buffer( &msg );
    return rc;
  }

  session_object->cmd_control.issued_count++;
  session_object->cmd_control.is_resp_pending = TRUE;
  return ADSP_EOK;
}


ADSPResult vsm_action_set_cached_rate_modulation (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vsm_enc_set_rate_mod_t set_rate_mod;
  void* apr_payload;

  switch ( session_object->active_set.media_id )
  {
    case VSS_MEDIA_ID_13K:
      set_rate_mod.rate_modulation_param = 
        session_object->cached_voc_properties_qcelp13k.reduced_rate_mode;
      break;

    case VSS_MEDIA_ID_EVRC:
      set_rate_mod.rate_modulation_param = 
        session_object->cached_voc_properties_evrc.reduced_rate_mode;
      break;

    /* This command applies only to QCELP-13K and EVRC vocoders. 
       Hence avoiding to set for other vocoders.
    */
    default:
      set_rate_mod.rate_modulation_param = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
      break;
  }

  if ( VSM_STREAM_PROPERTY_NOT_SET_UINT32 == set_rate_mod.rate_modulation_param )
  {
    return ADSP_EOK;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: ACTION_SET_RATE_MODULATION: cached reduced rate mode 0x%x",
         set_rate_mod.rate_modulation_param );

  elite_apr_packet_t* apr_pkt;
  elite_msg_any_t     msg;

  rc = elite_apr_if_alloc_cmd(
         vsm_apr_handle,
         session_object->apr_info.self_addr, session_object->apr_info.self_port,
         session_object->apr_info.self_addr, session_object->apr_info.self_port,
         VSM_ASYNC_CMD_TOKEN_TX, VSM_CMD_ENC_SET_RATE_MOD,
         sizeof( set_rate_mod ), &apr_pkt );
  if( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_cached_rate_modulation:"
           " FAILED to allocate APR packet, session_port0x%X )",
          ( int )session_object->apr_info.self_port );
   return ADSP_EFAILED;
  }
  apr_payload = ( void* )( elite_apr_if_get_payload_ptr( apr_pkt ) );
  memscpy( apr_payload, sizeof( set_rate_mod ), &set_rate_mod, sizeof( set_rate_mod ) );

  msg.unOpCode = ELITE_APR_PACKET;
  msg.pPayload = (void *) apr_pkt;
  rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg );
  if ( ADSP_FAILED( rc ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_set_cached_rate_modulation: FAILED to set rate modulation on tx!!\n" );
    elite_msg_return_payload_buffer( &msg );
    return rc;
  }

  session_object->cmd_control.issued_count++;
  return ADSP_EOK;
}


ADSPResult vsm_action_set_cached_dtx_mode(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vsm_enc_set_dtx_mode_t set_dtx;
  void* apr_payload;

  vsm_utility_get_cached_dtx_mode( session_object , &set_dtx );

  if ( VSM_STREAM_PROPERTY_NOT_SET_UINT32 == set_dtx.dtx_mode )
  {
    return ADSP_EOK;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: ACTION_SET_DTX_MODE: cached DTX value 0x%x", set_dtx.dtx_mode );

  elite_apr_packet_t* apr_pkt;
  elite_msg_any_t     msg;

  rc = elite_apr_if_alloc_cmd(
         vsm_apr_handle,
         session_object->apr_info.self_addr, session_object->apr_info.self_port,
         session_object->apr_info.self_addr, session_object->apr_info.self_port,
         VSM_ASYNC_CMD_TOKEN_TX, VSM_CMD_ENC_SET_DTX_MODE,
         sizeof( set_dtx ), &apr_pkt );
  if( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_cached_dtx_mode:"
           " FAILED to allocate APR packet, session_port0x%X )",
          ( int )session_object->apr_info.self_port );
    return ADSP_EFAILED;
  }
  apr_payload = ( void* )( elite_apr_if_get_payload_ptr( apr_pkt ) );
  memscpy( apr_payload, sizeof( set_dtx ), &set_dtx, sizeof( set_dtx ) );

  msg.unOpCode = ELITE_APR_PACKET;
  msg.pPayload = (void *) apr_pkt;
  rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg );
  if ( ADSP_FAILED( rc ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_set_cached_dtx_mode: FAILED to set DTX mode on tx!!\n" );
    elite_msg_return_payload_buffer( &msg );
    return rc;
  }

  session_object->cmd_control.issued_count++;
  return ADSP_EOK;
}

ADSPResult vsm_action_set_cached_properties(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  ( void )vsm_action_set_cached_dtx_mode( session_object );
  ( void )vsm_action_set_cached_rate_modulation( session_object );
  ( void )vsm_action_set_cached_minmax_rate( session_object );
  ( void )vsm_action_set_cached_channel_aware_mode( session_object );
  ( void )vsm_action_set_cached_enc_operating_mode( session_object );
  ( void )vsm_action_set_cached_enc_rate( session_object );

  if ( 0 != session_object->cmd_control.issued_count )
  {
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: ACTION_SET_CACHED_PROPs: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;

}

ADSPResult vsm_action_reinit_stream(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;

  session_object->common_cal.is_calibrate_needed =
     session_object->common_cal.is_registered; /* BACKWARD COMPATIBILITY */

  session_object->static_cal.is_calibrate_needed =
     session_object->static_cal.is_registered;

  /* Reset the eAMR mode to default (i.e. narrowband) after reinitialize
  * VSM. Refer to the comments under
  * eamr_mode_change_notification_info in cvs_session_object_t for
  * details.
  */
  session_object->eamr_mode_change_notification_info.mode = VSS_ISTREAM_EAMR_MODE_NARROWBAND;

  /* Reset the EVS Bandwidth to default (i.e. super wide-band) after 
   * reinitialize VSM. Refer to the comments under
   * evs_bandwidth_change_notification_info in cvs_session_object_t for
   * details.
  */            
  session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth = 
            VSM_STREAM_VOC_BANDWIDTH_EVT_NOT_SET;            
  session_object->evs_bandwidth_change_notification_info.last_sent_rx_bandwidth = 
            VSM_STREAM_VOC_BANDWIDTH_EVT_NOT_SET;
  session_object->active_set.system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;

  /* on reinit, reset common TTY state info except TTY mode
   *   (keep mode until explicitly changed with SET_TTY_MODE)
   * -> dynamic service will go thru init/reinit to fully reset CTM algorithms
   */
  uint16_t tty_mode = session_object->tty_state.m_etty_mode;
  memset( &session_object->tty_state, 0, sizeof( voice_strm_tty_state_t));
  session_object->tty_state.m_etty_mode = tty_mode;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  /* Reinit Dynamic services. */
  if( session_object->vdec )
  {
    elite_msg_any_t msg_rx_t;
    uint32_t size = sizeof( elite_msg_custom_header_t );

    result = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK );
    if( ADSP_FAILED( result))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_reinit_stream:"
            " FAILED to create matrix_connect_msg_t EliteMsg, session_port0x%X )",
            ( int )session_object->apr_info.self_port );
      return result;
    }
    elite_msg_custom_header_t *payload_ptr = (elite_msg_custom_header_t*) msg_rx_t.pPayload;
    payload_ptr->unSecOpCode = VDEC_REINIT_CMD;

    if ( ADSP_FAILED( result = qurt_elite_queue_push_back( session_object->vdec->cmdQ,( uint64_t * )&msg_rx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_reinit_stream:"
             " Unable to push to rx cmdQ, error = %d!!\n",( int )result );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if( session_object->venc )
  {
    elite_msg_any_t msg_tx_t;
    uint32_t size = sizeof( elite_msg_custom_header_t );

    result = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( result))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_reinit_stream:"
            " FAILED to create matrix_connect_msg_t EliteMsg, session_port0x%X )",
            ( int )session_object->apr_info.self_port );
      return result;
    }
    elite_msg_custom_header_t *payload_ptr = (elite_msg_custom_header_t*) msg_tx_t.pPayload;
    payload_ptr->unSecOpCode = VENC_REINIT_CMD;

    if ( ADSP_FAILED( result = qurt_elite_queue_push_back( session_object->venc->cmdQ,( uint64_t * )&msg_tx_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_reinit_stream:"
             " Unable to push to tx cmdQ, error = %d!!\n",( int )result );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_reinit_stream: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_close_mixer_input_output(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_any_t matrix_connect_msg_t;
  uint32_t size;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  if( session_object->mixer_in_port_ptr != 0 )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_close_mixer_input_output: inport_id %d\n",
           session_object->mixer_in_port_ptr->inport_id );

    size = sizeof( elite_msg_custom_vmx_cfg_inports_t );
    result = elite_msg_create_msg( &matrix_connect_msg_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK );
    if( ADSP_FAILED( result))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_close_mixer_input_output:"
            " FAILED to create matrix_connect_msg_t EliteMsg, session_port0x%X )",
            ( int )session_object->apr_info.self_port );
      return result;
    }

    elite_msg_custom_vmx_cfg_inports_t* inports_payload_ptr =
       ( elite_msg_custom_vmx_cfg_inports_t* )matrix_connect_msg_t.pPayload;
    inports_payload_ptr->unSecOpCode = ELITEMSG_CUSTOM_VMX_CFG_INPUT_PORTS;
    inports_payload_ptr->cfgInPortsPayload.ack_ptr = &session_object->mixer_in_port_ptr;
    inports_payload_ptr->cfgInPortsPayload.index = session_object->mixer_in_port_ptr->inport_id;
    inports_payload_ptr->cfgInPortsPayload.configuration = VMX_CLOSE_INPUT;

    if ( ADSP_FAILED( result = qurt_elite_queue_push_back( rxVoiceMatrix->cmdQ,( uint64_t * )&matrix_connect_msg_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_close_mixer_input_output:"
             " Unable to push to rx matrix cmdQ, error = %d!!\n",( int )result );
      elite_msg_return_payload_buffer( &matrix_connect_msg_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if( session_object->mixer_out_port_ptr != NULL )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_close_mixer_input_output: outport_id %d\n",
           session_object->mixer_out_port_ptr->outport_id );

    size = sizeof( elite_msg_custom_vmx_cfg_outports_t );
    result = elite_msg_create_msg( &matrix_connect_msg_t,
              &size,
              ELITE_CUSTOM_MSG,
              session_object->cmd_control.respQ,
              VSM_ASYNC_CMD_TOKEN_TX,
              ADSP_EOK );
    if( ADSP_FAILED( result  ))
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_close_mixer_input_output:"
             " FAILED to create matrix_connect_msg_t EliteMsg, session_port 0x%X)",
             ( int ) session_object->apr_info.self_port );
      return result;
    }

    elite_msg_custom_vmx_cfg_outports_t* outports_payload_ptr = ( elite_msg_custom_vmx_cfg_outports_t* )matrix_connect_msg_t.pPayload;
    outports_payload_ptr->unSecOpCode = ELITEMSG_CUSTOM_VMX_CFG_OUTPUT_PORTS;
    outports_payload_ptr->cfgOutPortsPayload.ack_ptr = &session_object->mixer_out_port_ptr;
    outports_payload_ptr->cfgOutPortsPayload.index = session_object->mixer_out_port_ptr->outport_id;
    outports_payload_ptr->cfgOutPortsPayload.configuration = VMX_CLOSE_OUTPUT;

    if ( ADSP_FAILED( result = qurt_elite_queue_push_back( txVoiceMatrix->cmdQ,( uint64_t * )&matrix_connect_msg_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_close_mixer_input_output:"
           " Unable to push to tx matrix cmdQ, error = %d!!\n", ( int ) result );
      elite_msg_return_payload_buffer( &matrix_connect_msg_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_close_mixer_input_output: cmd issued count %d, resp count %d\n",
       session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;

}

ADSPResult vsm_action_cleanup_create_errors(
  vsm_session_object_t* session_object
)
{
   MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_cleanup_create_errors: closing mixer in_out..." );
  ( void )vsm_action_close_mixer_input_output( session_object );
  return ADSP_EOK;
}

ADSPResult vsm_action_register_event(
  vsm_session_object_t* session_object,
  uint32_t event_id
)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_any_t msg_rx, msg_tx;
   elite_msg_custom_event_reg_type *payload_ptr = NULL;
   uint32_t size = sizeof(elite_msg_custom_event_reg_type);

   if ( NULL == session_object )
   {
     return ADSP_EBADPARAM;
   }

   MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_register_event: Register for the event 0x%8X", event_id );

   session_object->cmd_control.issued_count = 0;
   session_object->cmd_control.resp_count = 0;
   session_object->cmd_control.resp_result = 0xFFFFFFFF;

   if( session_object->vdec )
   {
     result = elite_msg_create_msg( &msg_rx,
               &size,
               ELITE_CUSTOM_MSG,
               NULL, //no response required
               0,
               ADSP_EOK);
     if( ADSP_FAILED( result ) )
     {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_register_event:"
         " FAILED to create EliteMsg,session 0x%X", session_object->apr_info.self_port );
       return result;
     }
     payload_ptr = (elite_msg_custom_event_reg_type*) msg_rx.pPayload;
     payload_ptr->sec_opcode = VDEC_REGISTER_EVENT;
     payload_ptr->event_id = event_id;

     ( void )qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx );

     if ( ADSP_FAILED( result ) )
     {
       MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_register_event: FAILED to set vocoder type on tx!!\n" );
       elite_msg_return_payload_buffer( &msg_tx );
       return result;
     }
     //TBD: to move this to cmd_control ??
     if ( VSM_EVT_VOC_OPERATING_MODE_UPDATE == event_id )
     {
       session_object->voc_operating_mode_info.is_rx_mode_received = FALSE;
     }
   }

   if( session_object->venc )
   {
     result = elite_msg_create_msg( &msg_tx,
                &size,
                ELITE_CUSTOM_MSG,
                NULL, //no response required
                0,
                ADSP_EOK);
     if( ADSP_FAILED( result ) )
     {
       MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_register_event:"
         " FAILED to create EliteMsg,session 0x%X", session_object->apr_info.self_port );
       return result;
     }
     payload_ptr = ( elite_msg_custom_event_reg_type* ) msg_tx.pPayload;
     payload_ptr->sec_opcode = VENC_REGISTER_EVENT;
     payload_ptr->event_id = event_id;

     ( void )qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx );

     if ( ADSP_FAILED( result ) )
     {
       MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_register_event: FAILED to set vocoder type on tx!!\n" );
       elite_msg_return_payload_buffer( &msg_tx );
       return result;
     }
     //TBD: to move this to cmd_control ??
     if ( VSM_EVT_VOC_OPERATING_MODE_UPDATE == event_id )
     {
       session_object->voc_operating_mode_info.is_tx_mode_received = FALSE;
     }
   }

   MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_register_event: is_tx_mode_received %d, is_rx_mode_received %d\n",
          session_object->voc_operating_mode_info.is_tx_mode_received, session_object->voc_operating_mode_info.is_rx_mode_received );

   return ADSP_EOK;
}

ADSPResult vsm_action_update_mixer_port_mapping(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }
#if 0
  uint16_t vocproc_handle = session_object->attached_vpm_session_handle;

/*  this is a hack   :)
   TBD: Ideally VSM should query VPM for the PORT mapping.
 * for some reason existing code is directly accessing VPM session variables.
 * Need to fix this.
 */
  //Current session index to APR port mapping si same in VSM adn VPM :)
  uint32_t vpm_session_index = ( vocproc_handle & 0x7 );

  if( voiceProcMgr->session[ vpm_session_index] )
  {
    if( voiceProcMgr->session[ vpm_session_index ]->mixer_out_port )
    {
      session_object->attached_vpm_outport_mapping = 1<< ( voiceProcMgr->session[ vpm_session_index ]->mixer_out_port->outport_id );
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_update_mixer_port_mapping:"
            " NULL VPM output port %u, ignoring!",( int ) vpm_session_index );
    }
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_update_mixer_port_mapping:"
         " NULL VPM session %u, ignoring!",( int ) vpm_session_index );
    return ADSP_EFAILED;
  }

  if( voiceProcMgr->session[ vpm_session_index ] )
  {
    if( voiceProcMgr->session[ vpm_session_index ]->mixer_in_port )
    {
      session_object->attached_vpm_inport_mapping = 1 << ( voiceProcMgr->session[vpm_session_index]->mixer_in_port->inport_id );
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_update_mixer_port_mapping:"
            " NULL VPM input port %u, ignoring!",( int ) vpm_session_index );
    }
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_update_mixer_port_mapping:"
           " NULL VPM session %u, ignoring!",( int ) vpm_session_index );
    return ADSP_EFAILED;
  }

#endif
  return ADSP_EOK;
}

ADSPResult vsm_action_send_detach_to_vocproc(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }
  uint16_t vocproc_handle = session_object->target_set.attach_detach_vocproc_handle;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  result = elite_apr_if_alloc_send_cmd(
         vsm_apr_handle, session_object->apr_info.self_addr,
         session_object->apr_info.self_port,
         vsm_cvp_addr, vocproc_handle,
         VSM_ASYNC_CMD_TOKEN_VOCPROC, VSS_IVOCPROC_CMD_DETACH_STREAM,
         NULL, 0 );

  session_object->cmd_control.issued_count++;
  session_object->cmd_control.is_resp_pending = TRUE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_send_detach_to_vocproc: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return result;
}

ADSPResult vsm_action_send_mute_notification(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vss_icommon_evt_voice_activity_update_t vactivity;

  if( TRUE == session_object->target_set.mute.tx_mute_flag )
  {
    vactivity.activity = VSS_ICOMMON_VOICE_ACTIVITY_UI_STREAM_TX_MUTE;
  }
  else
  {
    vactivity.activity = VSS_ICOMMON_VOICE_ACTIVITY_UI_STREAM_TX_UNMUTE;
  }

  rc = elite_apr_if_alloc_send_event(
           vsm_apr_handle,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           vsm_mvm_addr, session_object->attached_mvm_handle,
           0, VSS_ICOMMON_EVT_VOICE_ACTIVITY_UPDATE,
           &vactivity, sizeof( vactivity ) );
  if ( rc  )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_send_mute_notification():"
           " Failed tos end notification  error %d", rc );
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_send_mute_notification: sent event to client\n" );
  }
  return rc;
}

ADSPResult vsm_action_send_ready_event(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;

  rc = elite_apr_if_alloc_send_event(
           vsm_apr_handle,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           session_object->apr_info.client_addr, session_object->apr_info.client_port,
           APR_NULL_V, VSS_ISTREAM_EVT_READY, NULL, 0 );
  if ( rc  )
  {
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_send_ready_event():"
            " Failed tos end notification  error %d", rc );
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_send_ready_event: sent event to client\n" );
  }

  return rc;
}

ADSPResult vsm_action_set_encoder_reset (
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;
  vsm_vocoder_reset_param_info_t* vocoder_reset_param_info = NULL;
  elite_msg_param_cal_t *msg_payload = NULL;
  elite_msg_any_t msg;
  uint32_t msg_payload_size = sizeof( elite_msg_param_cal_t );

  vocoder_reset_param_info = ( vsm_vocoder_reset_param_info_t* )qurt_elite_memory_malloc(
                                   sizeof( vsm_vocoder_reset_param_info_t ), QURT_ELITE_HEAP_DEFAULT );
  if ( NULL == vocoder_reset_param_info )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: handling encoder reset: failed to allocate memory for set_param..." );
    session_object->cmd_control.resp_result = ADSP_EFAILED;
    return ADSP_EFAILED;
  }

  rc = elite_msg_create_msg( &msg, &msg_payload_size,
         ELITE_CMD_SET_PARAM, session_object->cmd_control.respQ,
         VSM_ASYNC_CMD_TOKEN_PARAM_INBAND_TX, ADSP_EOK);
  if( ADSP_FAILED( rc ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "VSM: handling encoder reset: FAILED to create EliteMsg!!\n" );
    qurt_elite_memory_free( (void*)vocoder_reset_param_info );
    session_object->cmd_control.resp_result = ADSP_EFAILED;
    return ADSP_EFAILED;
  }
  
  vocoder_reset_param_info->param_hdr.module_id = session_object->active_set.media_id;
  vocoder_reset_param_info->param_hdr.param_id = VOC_PARAM_VOC_RESET;
  vocoder_reset_param_info->param_hdr.param_size = sizeof(uint32_t);
  vocoder_reset_param_info->param_hdr.reserved = 0;
  vocoder_reset_param_info->encoder_reset = TRUE;
  /* Decoder reset neither required nor implemented hence setting decoder flag flase here. */
  vocoder_reset_param_info->decoder_reset = FALSE;
  
  msg_payload = ( elite_msg_param_cal_t* ) msg.pPayload;
  msg_payload->unSize = sizeof(vsm_vocoder_reset_param_info_t);
  msg_payload->pnParamData = (int32_t*)vocoder_reset_param_info;
  msg_payload->unParamId = ELITEMSG_PARAM_ID_CAL;
  
  rc = qurt_elite_queue_push_back( session_object->venc->cmdQ, (uint64_t*)&msg );
  if ( ADSP_FAILED( rc ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: handling encoder reset: FAILED to push to venc cmdQ. error %d!!\n", rc );
    elite_msg_return_payload_buffer( &msg );
    qurt_elite_memory_free( (void*)vocoder_reset_param_info );
    session_object->cmd_control.resp_result = ADSP_EFAILED;
    return ADSP_EFAILED;
  }

  session_object->cmd_control.issued_count++;
  session_object->cmd_control.is_cmd_pending = TRUE;
  session_object->cmd_control.is_resp_pending = TRUE;

  return ADSP_EOK;
}

ADSPResult vsm_action_send_eamr_mode_event (
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;;
  vss_istream_evt_eamr_mode_changed_t eamr_mode_changed;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  if( TRUE == session_object->eamr_mode_change_notification_info.is_enabled )
  {
    eamr_mode_changed.mode = session_object->eamr_mode_change_notification_info.mode;

    result = elite_apr_if_alloc_send_event(
               vsm_apr_handle,
               session_object->apr_info.self_addr,
               session_object->apr_info.self_port,
               session_object->eamr_mode_change_notification_info.client_addr,
               session_object->eamr_mode_change_notification_info.client_port,
               0, VSS_ISTREAM_EVT_EAMR_MODE_CHANGED,
               &eamr_mode_changed, sizeof( eamr_mode_changed ) );
    VSM_COMM_ERROR( result, session_object->eamr_mode_change_notification_info.client_addr );
  }

  return result;
}

ADSPResult vsm_action_send_evs_bandwidth_event (
  vsm_session_object_t* session_object 
)
{
  ADSPResult result = ADSP_EOK;
  vss_istream_evt_evs_rx_bandwidth_changed_t evs_rx_bandwidth_changed;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  if ( VSM_STREAM_VOC_BANDWIDTH_EVT_NOT_SET == session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "VSM: LISTEN_CLASS_EVS_BANDWIDTH_CHANGE: not yet set by vocoder "
         "while stream is running." );
    return ADSP_EUNEXPECTED;
  }

  if ( ( session_object->evs_bandwidth_change_notification_info.is_enabled == TRUE ) &&
       ( session_object->evs_bandwidth_change_notification_info.last_sent_rx_bandwidth != 
          session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth ) )
  {
    evs_rx_bandwidth_changed.bandwidth = session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth;

    result = elite_apr_if_alloc_send_event(
           vsm_apr_handle,session_object->apr_info.self_addr,
           session_object->apr_info.self_port,
           session_object->evs_bandwidth_change_notification_info.client_addr,
           session_object->evs_bandwidth_change_notification_info.client_port,
           0, VSS_ISTREAM_EVT_EVS_RX_BANDWIDTH_CHANGED,
           &evs_rx_bandwidth_changed, sizeof( evs_rx_bandwidth_changed ) );
    VSM_COMM_ERROR( result, session_object->evs_bandwidth_change_notification_info.client_addr );

    /* Updated last sent Rx Bandwidth */
    session_object->evs_bandwidth_change_notification_info.last_sent_rx_bandwidth = 
      session_object->evs_bandwidth_change_notification_info.last_received_rx_bandwidth;
  }

  return result;
}


ADSPResult vsm_action_send_avsync_rx_delay_event(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  uint32_t rx_delay = session_object->avsync_delay_info.total_rx_delay;
  vss_iavsync_evt_rx_path_delay_t rx_path_delay;

  rx_path_delay.delay_us = rx_delay;

  if( session_object->avsync_client_rx_info.is_enabled == TRUE )
  {
    result = elite_apr_if_alloc_send_event(
               vsm_apr_handle,
               session_object->apr_info.self_addr,
               session_object->apr_info.self_port,
               session_object->avsync_client_rx_info.client_addr,
               session_object->avsync_client_rx_info.client_port,
               APR_NULL_V, VSS_IAVSYNC_EVT_RX_PATH_DELAY,
               &rx_path_delay, sizeof( rx_path_delay ) );
    VSM_COMM_ERROR(result, session_object->avsync_client_rx_info.client_addr );

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VSM: ACTION_SEND_IAVSYNC_RX_PATH_DELAY: Client Addr(0x%04X) Port(0x%04X), "
           "rx_path_delay_us: %d", session_object->avsync_client_rx_info.client_addr,
           session_object->avsync_client_rx_info.client_port, rx_path_delay.delay_us );
  }

  return result;
}

ADSPResult vsm_action_send_enable_notifications_to_clients(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  result = vsm_action_send_avsync_rx_delay_event( session_object );

  if ( VSS_MEDIA_ID_EAMR == session_object->active_set.media_id )
  {
    ( void ) vsm_action_send_eamr_mode_event( session_object );
  }

  if( VSS_MEDIA_ID_EVS == session_object->active_set.media_id )
  {
    ( void ) vsm_action_send_evs_bandwidth_event( session_object );
    ( void ) vsm_action_publish_evs_params( session_object );
  }

  result |= vsm_action_send_ready_event( session_object );
  session_object->packet_exchange_info.oob_info.is_enc_buf_consumed = TRUE;

  return result;
}

ADSPResult vsm_action_send_not_ready_event(
  vsm_session_object_t* session_object
)
{
  int32_t rc = ADSP_EOK;

  rc = elite_apr_if_alloc_send_event(
           vsm_apr_handle,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           session_object->apr_info.client_addr, session_object->apr_info.client_port,
           APR_NULL_V, VSS_ISTREAM_EVT_NOT_READY, NULL, 0 );
    if ( rc  )
    {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_send_not_ready_event():"
              " Failed to send event  error %d", rc );
    }
    else
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_send_not_ready_event: sent event to client\n" );
    }
  return rc;
}

ADSPResult vsm_action_send_disable_notifications_to_clients(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_send_disable_notifications_to_clients(): sending notifications" );

  result |= vsm_action_send_not_ready_event( session_object );

  return result;
}

ADSPResult vsm_action_send_reconfig_event(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  result = elite_apr_if_alloc_send_event(
             vsm_apr_handle,
             session_object->apr_info.self_addr,
             session_object->apr_info.self_port,
             vsm_mvm_addr, session_object->attached_mvm_handle,
             0, VSS_ISTREAM_EVT_RECONFIG,
             NULL, 0 ); 
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_send_reconfig_event: sent event to MVM\n" );

  return result;
}

ADSPResult vsm_action_send_system_config_response(
  vsm_session_object_t* session_object
)
{
  ADSPResult rc = ADSP_EOK;
  vss_icommon_rsp_set_system_config_t set_system_config_rsp;
  uint32_t client_addr;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_object->cmd_control.cmd_msg.pPayload );

  session_object->common_cal.is_calibrate_needed = FALSE; /* BACKWARD COMPATIBILITY */
  session_object->static_cal.is_calibrate_needed = FALSE;
  session_object->is_stream_config_changed = FALSE;

  session_object->active_set.system_config.network_id = session_object->target_set.system_config.network_id;
  session_object->active_set.system_config.media_id = session_object->target_set.system_config.media_id;
  session_object->active_set.system_config.rx_voc_op_mode = session_object->target_set.system_config.rx_voc_op_mode;
  session_object->active_set.system_config.tx_voc_op_mode = session_object->target_set.system_config.tx_voc_op_mode;
  session_object->active_set.system_config.dec_sr = session_object->target_set.system_config.dec_sr;
  session_object->active_set.system_config.enc_sr = session_object->target_set.system_config.enc_sr;
  session_object->active_set.system_config.rx_pp_sr = session_object->target_set.system_config.rx_pp_sr;
  session_object->active_set.system_config.tx_pp_sr = session_object->target_set.system_config.tx_pp_sr;
  session_object->active_set.system_config.feature = session_object->target_set.system_config.feature;

  set_system_config_rsp.enc_kpps = session_object->kpps_info.enc;
  set_system_config_rsp.dec_kpps = session_object->kpps_info.dec;
  set_system_config_rsp.dec_pp_kpps = session_object->kpps_info.dec_pp;
  set_system_config_rsp.enc_sr = session_object->active_set.system_config.enc_sr;
  set_system_config_rsp.dec_sr = session_object->active_set.system_config.dec_sr;
  set_system_config_rsp.stream_media_id = session_object->active_set.media_id;

  client_addr = apr_pkt->dst_addr;
  rc = elite_apr_if_alloc_send_ack(
               vsm_apr_handle, apr_pkt->dst_addr, apr_pkt->dst_port,
               apr_pkt->src_addr, apr_pkt->src_port,
               apr_pkt->token, VSS_ICOMMON_RSP_SET_SYSTEM_CONFIG,
               &set_system_config_rsp, sizeof( set_system_config_rsp ) );
  VSM_COMM_ERROR( rc, client_addr );

  return rc;
}

ADSPResult vsm_action_send_attach_to_vocproc(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;
  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }
  uint16_t vocproc_handle = session_object->target_set.attach_detach_vocproc_handle;
  vss_ivocproc_cmd_attach_stream_t payload;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  payload.direction = session_object->target_set.attach_detach_vocproc_direction;
  result = elite_apr_if_alloc_send_cmd(
         vsm_apr_handle, session_object->apr_info.self_addr,
         session_object->apr_info.self_port,
         vsm_cvp_addr, vocproc_handle,
         VSM_ASYNC_CMD_TOKEN_VOCPROC, VSS_IVOCPROC_CMD_ATTACH_STREAM,
         ( void* )&payload, sizeof( vss_ivocproc_cmd_attach_stream_t ) );

  session_object->cmd_control.issued_count++;
  session_object->cmd_control.is_resp_pending = TRUE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_send_attach_to_vocproc: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return result;
}

ADSPResult vsm_action_set_rx_dtmf_detect_config(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;;
  elite_apr_packet_t* apr_pkt;
  elite_msg_any_t msg;
  vsm_set_rx_dtmf_detection_t* vsm_rx_dtmf_detect;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_rx_dtmf_detect_config: enable %d\n",
         session_object->set_rx_dtmf_detect.enable_flag );

  if ( session_object->vdec )
  {
    result = elite_apr_if_alloc_cmd(
           vsm_apr_handle,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           session_object->apr_info.self_addr, session_object->apr_info.self_port,
           VSM_ASYNC_CMD_TOKEN_RX, VSM_CMD_SET_RX_DTMF_DETECTION,
           sizeof( vsm_set_rx_dtmf_detection_t ), &apr_pkt );
    if( result )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_rx_dtmf_detect_config:"
             " FAILED to allocate APR packet, session_port0x%X )",
            ( int )session_object->apr_info.self_port );
      return ADSP_EFAILED;
    }
    vsm_rx_dtmf_detect = ( vsm_set_rx_dtmf_detection_t* )( elite_apr_if_get_payload_ptr( apr_pkt ) );

    vsm_rx_dtmf_detect->enable = session_object->set_rx_dtmf_detect.enable_flag;

    msg.unOpCode = ELITE_APR_PACKET;
    msg.pPayload = (void *) apr_pkt;
    result = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg );
    if ( ADSP_FAILED( result ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_rx_dtmf_detect_config: FAILED to set DTMF detect mode on Rx!!\n" );
      elite_msg_return_payload_buffer( &msg );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_rx_dtmf_detect_config: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return result;
}

ADSPResult vsm_action_set_media_type(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_media_type: "
         "media_id 0x%08X\n", session_object->target_set.media_id );

  if ( session_object->venc )
  {
    elite_msg_any_t msg_tx_t;
    elite_msg_data_media_type_header_t *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_data_media_type_header_t );

    result = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_DATA_MEDIA_TYPE,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( result ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_media_type: FAILED to create EliteMsg!!\n" );
      return result;
    }
    payload_ptr = ( elite_msg_data_media_type_header_t* ) msg_tx_t.pPayload;
    payload_ptr->unMediaTypeFormat = session_object->target_set.media_id;

    result = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );

    if ( ADSP_FAILED( result ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_media_type: FAILED to push to venc cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec )
  {
    elite_msg_any_t msg_rx_t;
    elite_msg_data_media_type_header_t *payload_ptr = NULL;
    uint32_t size = sizeof(elite_msg_data_media_type_header_t);

    result = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_DATA_MEDIA_TYPE,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK);
    if( ADSP_FAILED( result ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_media_type: FAILED to create EliteMsg!!\n" );
      return result;
    }
    payload_ptr = ( elite_msg_data_media_type_header_t* ) msg_rx_t.pPayload;
    payload_ptr->unMediaTypeFormat = session_object->target_set.media_id;

    result = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );

    if ( ADSP_FAILED( result ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_media_type: FAILED to push to vdec cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_media_type: cmd issued count %d, resp count %d\n",
       session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;

}

ADSPResult vsm_action_connect_rx_downstream(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vsm_action_connect_rx_downstream:"
          " VSM acquired input port on rx matrix! PortID = %d!!\n",
         ( int ) session_object->mixer_in_port_ptr->inport_id );

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  elite_msg_any_t rx_connect_msg_t;
  uint32_t size = sizeof( elite_msg_cmd_connect_t );
  result = elite_msg_create_msg( &rx_connect_msg_t,
        &size,
        ELITE_CMD_CONNECT,
        session_object->cmd_control.respQ,
        VSM_ASYNC_CMD_TOKEN_RX,
        ADSP_EOK);

  if( ADSP_FAILED( result ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vsm_action_connect_rx_downstream: FAILED to create EliteMsg!!\n" );
    return result;
  }

  elite_msg_cmd_connect_t* connect_payload_ptr = ( elite_msg_cmd_connect_t* )rx_connect_msg_t.pPayload;

  //TBD: Amar to check why poiner is requried here.
  connect_payload_ptr->pSvcHandle = &session_object->mixer_in_port_ptr->port_handle;

  result = qurt_elite_queue_push_back( session_object->vdec->cmdQ,(uint64_t*)&rx_connect_msg_t );
  if ( ADSP_FAILED( result ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vsm_action_connect_rx_downstream: FAILED to push to vdec cmdQ. error %d!!\n", result );
    elite_msg_return_payload_buffer( &rx_connect_msg_t);
    return result;
  }
  session_object->cmd_control.issued_count++;
  session_object->cmd_control.is_resp_pending = TRUE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_connect_rx_downstream: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return result;
}

ADSPResult vsm_action_set_pp_sampling_rate(
  vsm_session_object_t* session_object
)
{
  ADSPResult result = ADSP_EOK;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_pp_sampling_rate: Tx SR %d, Rx SR %d\n",
         session_object->requested_var_voc_tx_sampling_rate, session_object->requested_var_voc_rx_sampling_rate );

  if ( session_object->venc )
  {
    elite_msg_any_t msg_tx_t;
    elite_msg_custom_voc_stream_set_sampling_type *payload_ptr = NULL;
    uint32_t size = sizeof( elite_msg_custom_voc_stream_set_sampling_type );

    result = elite_msg_create_msg( &msg_tx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );
    if( ADSP_FAILED( result ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_pp_sampling_rate: FAILED to create EliteMsg!!\n" );
      return result;
    }
    payload_ptr = (elite_msg_custom_voc_stream_set_sampling_type*) msg_tx_t.pPayload;
    payload_ptr->sec_opcode = VENC_SET_STREAM_PP_SAMP_RATE;
    payload_ptr->tx_samp_rate = session_object->requested_var_voc_tx_sampling_rate;
    payload_ptr->rx_samp_rate = session_object->requested_var_voc_rx_sampling_rate;

    result = qurt_elite_queue_push_back( session_object->venc->cmdQ, ( uint64_t* )&msg_tx_t );

    if ( ADSP_FAILED( result ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_pp_sampling_rate: FAILED to push to venc cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_tx_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( session_object->vdec )
  {
    elite_msg_any_t msg_rx_t;
    elite_msg_custom_voc_stream_set_sampling_type *payload_ptr = NULL;
    uint32_t size = sizeof(elite_msg_custom_voc_stream_set_sampling_type);

    result = elite_msg_create_msg( &msg_rx_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_RX,
               ADSP_EOK);
    if( ADSP_FAILED( result ) )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_pp_sampling_rate: FAILED to create EliteMsg!!\n" );
      return result;
    }
    payload_ptr = (elite_msg_custom_voc_stream_set_sampling_type*) msg_rx_t.pPayload;

    payload_ptr->sec_opcode = VDEC_SET_STREAM_PP_SAMP_RATE;
    payload_ptr->tx_samp_rate = session_object->requested_var_voc_tx_sampling_rate;
    payload_ptr->rx_samp_rate = session_object->requested_var_voc_rx_sampling_rate;

    result = qurt_elite_queue_push_back( session_object->vdec->cmdQ, ( uint64_t* )&msg_rx_t );

    if ( ADSP_FAILED( result ) )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_set_pp_sampling_rate: FAILED to push to vdec cmdQ. error %d!!\n", result );
      elite_msg_return_payload_buffer( &msg_rx_t );
      return result;
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_set_pp_sampling_rate: cmd issued count %d, resp count %d\n",
       session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_reconfig_tx_mixer(
  vsm_session_object_t* session_object
)
{
  ADSPResult rc = ADSP_EOK;
  uint16_t sampling_rate;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  /* should be MEDIATYPE_NONE at create -> samplingrate = default 8K */
  sampling_rate = voice_get_sampling_rate( 
                    session_object->active_set.media_id,
                    0                                                /*BeAMR flag for encoder*/,
                    ( uint16_t )session_object->requested_var_voc_tx_sampling_rate );  /*evs default sampling rate*/

  /* Create Tx Output ports */
  if( session_object->venc )
  {
    elite_msg_any_t matrix_connect_msg_t;
    uint32_t size = sizeof( elite_msg_custom_vmx_cfg_outports_t );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,"vsm_action_reconfig_tx_mixer:"
           " configure mixer output port id 0x%08X",
           session_object->mixer_out_port_ptr->outport_id );

    rc = elite_msg_create_msg( &matrix_connect_msg_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );

    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_reconfig_tx_mixer: FAILED to create EliteMsg!!\n" );
      return rc;
    }

    elite_msg_custom_vmx_cfg_outports_t* outports_payload_ptr = ( elite_msg_custom_vmx_cfg_outports_t* )matrix_connect_msg_t.pPayload;

    outports_payload_ptr->unSecOpCode = ELITEMSG_CUSTOM_VMX_CFG_OUTPUT_PORTS;
    outports_payload_ptr->cfgOutPortsPayload.ack_ptr = &session_object->mixer_out_port_ptr;
    outports_payload_ptr->cfgOutPortsPayload.index = session_object->mixer_out_port_ptr->outport_id;;
    outports_payload_ptr->cfgOutPortsPayload.configuration = VMX_RECONFIGURE_OUTPUT;
    outports_payload_ptr->cfgOutPortsPayload.out_sample_rate = sampling_rate / 1000;

    outports_payload_ptr->cfgOutPortsPayload.svc_handle_ptr = session_object->venc;

    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( txVoiceMatrix->cmdQ,( uint64_t * )&matrix_connect_msg_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_reconfig_tx_mixer:"
            " Unable to push to tx matrix cmdQ, error = %d!!\n", ( int )rc );
      elite_msg_return_payload_buffer( &matrix_connect_msg_t );
      return( rc );
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  session_object->active_tx_sampling_rate = sampling_rate;

  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vsm_action_reconfig_tx_mixer: active_tx_sr: %d, requested_var_voc_tx_sr: %d " \
         "cmd issued count: %d, resp count: %d", session_object->active_tx_sampling_rate,
         session_object->requested_var_voc_tx_sampling_rate, 
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );
  
  return ADSP_EOK;

}

ADSPResult vsm_action_reassign_mixer_inout_ports(
  vsm_session_object_t* session_object,
  bool_t is_attach
)
{
  ADSPResult rc =ADSP_EOK;
  uint32_t input_mapping_mask;
  uint32_t output_mapping_mask;

  if ( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  if ( TRUE == is_attach )
  {
    input_mapping_mask = session_object->input_mapping_mask | session_object->attached_vpm_outport_mapping;
  }
  else
  {
    input_mapping_mask = session_object->input_mapping_mask & ( ~session_object->attached_vpm_outport_mapping );
  }

  if( session_object->vdec )
  {
    elite_msg_any_t matrix_connect_msg_t;
    uint32_t size = sizeof( elite_msg_custom_vmx_cfg_inports_t );

    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"vsm_action_reassign_mixer_inout_ports: reassigning mixer input port ..." );

    rc = elite_msg_create_msg( &matrix_connect_msg_t,
           &size,
           ELITE_CUSTOM_MSG,
           session_object->cmd_control.respQ,
           VSM_ASYNC_CMD_TOKEN_RX,
           ADSP_EOK );

    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_reassign_mixer_inout_ports: FAILED to create EliteMsg!!\n" );
      return rc;
    }

    elite_msg_custom_vmx_cfg_inports_t* inports_payload_ptr = ( elite_msg_custom_vmx_cfg_inports_t* )matrix_connect_msg_t.pPayload;

    inports_payload_ptr->unSecOpCode = ELITEMSG_CUSTOM_VMX_CFG_INPUT_PORTS;
    inports_payload_ptr->cfgInPortsPayload.ack_ptr = &session_object->mixer_in_port_ptr;
    inports_payload_ptr->cfgInPortsPayload.index = session_object->mixer_in_port_ptr->inport_id;
    inports_payload_ptr->cfgInPortsPayload.configuration = VMX_REASSIGN_INPUT;

    inports_payload_ptr->cfgInPortsPayload.input_mapping_mask = input_mapping_mask;

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_reassign_mixer_inout_ports: assigning input_mapping_mask %#lx for rx path",
           inports_payload_ptr->cfgInPortsPayload.input_mapping_mask );

    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( rxVoiceMatrix->cmdQ, ( uint64_t * )&matrix_connect_msg_t ) ) )
    {
      elite_msg_return_payload_buffer( &matrix_connect_msg_t );
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_reassign_mixer_inout_ports:"
             " Unable to push to rx matrix cmdQ, error = %d!!\n", ( int ) rc );
      return( rc );
    }
    session_object->input_mapping_mask = input_mapping_mask;
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  if ( TRUE == is_attach )
  {
    output_mapping_mask = session_object->output_mapping_mask | session_object->attached_vpm_inport_mapping;
  }
  else
  {
    output_mapping_mask = session_object->output_mapping_mask & ( ~session_object->attached_vpm_inport_mapping );
  }

  if( session_object->venc )
  {
    elite_msg_any_t matrix_connect_msg_t;
    uint32_t size = sizeof( elite_msg_custom_vmx_cfg_outports_t );

    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_reassign_mixer_inout_ports: reassigning mixer output port ..." );

    rc = elite_msg_create_msg( &matrix_connect_msg_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );

    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_reassign_mixer_inout_ports: FAILED to create EliteMsg!!\n" );
      return rc;
    }

    elite_msg_custom_vmx_cfg_outports_t* outports_payload_ptr = ( elite_msg_custom_vmx_cfg_outports_t* )matrix_connect_msg_t.pPayload;

    outports_payload_ptr->unSecOpCode = ELITEMSG_CUSTOM_VMX_CFG_OUTPUT_PORTS;
    outports_payload_ptr->cfgOutPortsPayload.ack_ptr = &session_object->mixer_out_port_ptr;
    outports_payload_ptr->cfgOutPortsPayload.index = session_object->mixer_out_port_ptr->outport_id;
    outports_payload_ptr->cfgOutPortsPayload.configuration = VMX_REASSIGN_OUTPUT;

    outports_payload_ptr->cfgOutPortsPayload.output_mapping_mask = output_mapping_mask;

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_reassign_mixer_inout_ports:"
           " assigning output_mapping_mask %#lx for tx path",
           outports_payload_ptr->cfgOutPortsPayload.output_mapping_mask );

    outports_payload_ptr->cfgOutPortsPayload.svc_handle_ptr = session_object->venc;

    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( txVoiceMatrix->cmdQ,( uint64_t * )&matrix_connect_msg_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_reassign_mixer_inout_ports:"
            " Unable to push to tx matrix cmdQ, error = %d!!\n", ( int )rc );
      elite_msg_return_payload_buffer( &matrix_connect_msg_t );
      return( rc );
    }
    session_object->output_mapping_mask = output_mapping_mask;
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_reassign_mixer_inout_ports: cmd issued count %d, resp count %d\n",
         session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_create_mixer_input_ouput(
  vsm_session_object_t* session_object
)
{
  ADSPResult rc = ADSP_EOK;
  uint16_t sampling_rate;

  session_object->cmd_control.issued_count = 0;
  session_object->cmd_control.resp_count = 0;
  session_object->cmd_control.resp_result = 0xFFFFFFFF;

  /* Create Rx Input ports */
  if( session_object->vdec )
  {
    elite_msg_any_t matrix_connect_msg_t;
    uint32_t size = sizeof( elite_msg_custom_vmx_cfg_inports_t );

    MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,"vsm_action_create_mixer_input_ouput: creating mixer input port ..." );

    rc = elite_msg_create_msg( &matrix_connect_msg_t,
           &size,
           ELITE_CUSTOM_MSG,
           session_object->cmd_control.respQ,
           VSM_ASYNC_CMD_TOKEN_RX,
           ADSP_EOK );

    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_create_mixer_input_ouput: FAILED to create EliteMsg!!\n" );
      return rc;
    }

    elite_msg_custom_vmx_cfg_inports_t* inports_payload_ptr = ( elite_msg_custom_vmx_cfg_inports_t* )matrix_connect_msg_t.pPayload;

    inports_payload_ptr->unSecOpCode = ELITEMSG_CUSTOM_VMX_CFG_INPUT_PORTS;
    inports_payload_ptr->cfgInPortsPayload.ack_ptr = &session_object->mixer_in_port_ptr;
    inports_payload_ptr->cfgInPortsPayload.index = -1;
    inports_payload_ptr->cfgInPortsPayload.configuration = VMX_NEW_INPUT;
    inports_payload_ptr->cfgInPortsPayload.priority = VMX_INPUT_HIGH_PRIO;
    //Map input_mapping_mask using tx_handles[] array and portIDs from VPM structures
    //rx_handles begin after tx_device_count amount of handles
    inports_payload_ptr->cfgInPortsPayload.input_mapping_mask = session_object->input_mapping_mask = 0;

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_create_mixer_input_ouput: VSM got input_mapping_mask %#lx for rx path",
           inports_payload_ptr->cfgInPortsPayload.input_mapping_mask );

    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( rxVoiceMatrix->cmdQ, ( uint64_t * )&matrix_connect_msg_t ) ) )
    {
      elite_msg_return_payload_buffer( &matrix_connect_msg_t );
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_action_create_mixer_input_ouput:"
             " Unable to push to rx matrix cmdQ, error = %d!!\n", ( int ) rc );
      return( rc );
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  /* should be MEDIATYPE_NONE at create -> samplingrate = default 8K */
  sampling_rate = voice_get_sampling_rate( 
                    session_object->target_set.media_id,
                    0                                                /*BeAMR flag for encoder*/,
                    ( uint16_t )VOICE_FB_SAMPLING_RATE );            /*evs default sampling rate*/

  /* Create Tx Output ports */
  if( session_object->venc )
  {
    elite_msg_any_t matrix_connect_msg_t;
    uint32_t size = sizeof( elite_msg_custom_vmx_cfg_outports_t );

    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_create_mixer_input_ouput: creating mixer output port ..." );

    rc = elite_msg_create_msg( &matrix_connect_msg_t,
               &size,
               ELITE_CUSTOM_MSG,
               session_object->cmd_control.respQ,
               VSM_ASYNC_CMD_TOKEN_TX,
               ADSP_EOK );

    if( ADSP_FAILED( rc ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_create_mixer_input_ouput: FAILED to create EliteMsg!!\n" );
      return rc;
    }

    elite_msg_custom_vmx_cfg_outports_t* outports_payload_ptr = ( elite_msg_custom_vmx_cfg_outports_t* )matrix_connect_msg_t.pPayload;

    outports_payload_ptr->unSecOpCode = ELITEMSG_CUSTOM_VMX_CFG_OUTPUT_PORTS;
    outports_payload_ptr->cfgOutPortsPayload.ack_ptr = &session_object->mixer_out_port_ptr;
    outports_payload_ptr->cfgOutPortsPayload.index = -1;
    outports_payload_ptr->cfgOutPortsPayload.configuration = VMX_NEW_OUTPUT;
    outports_payload_ptr->cfgOutPortsPayload.num_output_bufs = 2;
    outports_payload_ptr->cfgOutPortsPayload.max_bufq_capacity = 4;
    outports_payload_ptr->cfgOutPortsPayload.out_sample_rate = sampling_rate / 1000;
    //tx handles are listed first
    outports_payload_ptr->cfgOutPortsPayload.output_mapping_mask = session_object->output_mapping_mask = 0;

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,"vsm_action_create_mixer_input_ouput:"
           " VSM got output_mapping_mask %#lx for tx path",
           outports_payload_ptr->cfgOutPortsPayload.output_mapping_mask );

    outports_payload_ptr->cfgOutPortsPayload.svc_handle_ptr = session_object->venc;

    if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( txVoiceMatrix->cmdQ,( uint64_t * )&matrix_connect_msg_t ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"vsm_action_create_mixer_input_ouput:"
            " Unable to push to tx matrix cmdQ, error = %d!!\n", ( int )rc );
      elite_msg_return_payload_buffer( &matrix_connect_msg_t );
      return( rc );
    }
    session_object->cmd_control.issued_count++;
    session_object->cmd_control.is_resp_pending = TRUE;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_create_mixer_input_ouput: cmd issued count %d, resp count %d\n",
       session_object->cmd_control.issued_count, session_object->cmd_control.resp_count );

  return ADSP_EOK;
}

ADSPResult vsm_action_create_dynamic_services(
  vsm_session_object_t* session_object
)
{
  ADSPResult rc = ADSP_EOK;
  /* TBD:  venc and vdec are taking the sessions internal member's address as input params
   *       for apr_info and tty. This is illegal.
   *       we shall change it in future. Atleast apr_info need not be a pointer.
   */

  switch( session_object->direction )
  {
    case VSM_DIRECTION_RX:
    case VSM_DIRECTION_RX_TX:
    {
      if ( ADSP_FAILED( rc = vdec_create (NULL,
                               &session_object->apr_info,
                               &session_object->tty_state,        /* shared ttyState info */
                               (void**)&session_object->vdec,
                               (uint32_t)( session_object->apr_info.self_port ) ) ) )
      {
        MSG(MSG_SSID_DFLT, MSG_LEGACY_FATAL,"vsm_action_create_dynamic_services: voice_dec Creation Failed");
        return rc;
      }
    }
    break;

    default:
    break;
  }

  switch( session_object->direction )
  {
    case VSM_DIRECTION_TX:
    case VSM_DIRECTION_RX_TX:
    {
      if (ADSP_FAILED(rc = venc_create (NULL,
                             &session_object->apr_info,
                             &session_object->tty_state,        /* shared ttyState info */
                             session_object->vdec,
                             (void**)&session_object->venc,                /* always give Rx handle to Tx for possible loopback.  NULL if no Rx session.  Loopback enable is dynamic */
                             (uint32_t)( session_object->apr_info.self_port ) ) ) )
      {
        MSG(MSG_SSID_DFLT, MSG_LEGACY_FATAL,"vsm_action_create_dynamic_services: voice_enc Creation Failed");
        return rc;
      }
    }
    break;

    default:
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_FATAL,"vsm_action_create_dynamic_services: "
            "Invalid direction. %d voice_enc Creation Failed", session_object->direction );
      return rc;
    }
    break;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_action_create_dynamic_services: created venc %d, created vdec %d\n",
        ( NULL != session_object->venc ), ( NULL != session_object->vdec ) );

  return ADSP_EOK;
}

ADSPResult vsm_action_send_avsync_tx_timestamp (
  vsm_session_object_t* session_obj,
  uint64_t timestamp_us
)
{
  int32_t rc = ADSP_EOK;
  vss_iavsync_evt_tx_timestamp_t avsync_tx_timestamp;
  avsync_tx_timestamp.timestamp_us = 0;

  if( session_obj->avsync_client_tx_info.is_enabled == TRUE )
  {
    avsync_tx_timestamp.timestamp_us = timestamp_us;
    rc = __aprv2_cmd_alloc_send(
           vsm_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
           session_obj->apr_info.self_addr, session_obj->apr_info.self_port,
           session_obj->avsync_client_tx_info.client_addr,
           session_obj->avsync_client_tx_info.client_port,
           NULL, VSS_IAVSYNC_EVT_TX_TIMESTAMP,
           &avsync_tx_timestamp, sizeof( avsync_tx_timestamp ) );
    VSM_COMM_ERROR( rc, session_obj->avsync_client_tx_info.client_addr );  
  }

  return rc;
}

ADSPResult vsm_action_tx_inband_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
)
{
  int32_t rc = ADSP_EOK;
  uint32_t data_size = 0;
  vsm_send_enc_packet_v2_t* enc_payload = NULL;
  elite_apr_packet_t* inband_enc_packet = NULL;
  vss_ipktexg_evt_in_band_send_enc_buffer_t* inband_enc_payload = NULL;

  for (;;)
  {
    enc_payload = APRV2_PKT_GET_PAYLOAD(vsm_send_enc_packet_v2_t, packet);
    data_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(packet->header)-sizeof(vsm_send_enc_packet_v2_t);
  
    ( void ) vsm_action_send_avsync_tx_timestamp( session_obj, enc_payload->timestamp_us );
  
    rc = elite_apr_if_alloc_event( session_obj->apr_info.apr_handle,
           session_obj->apr_info.self_addr, session_obj->apr_info.self_port,
           session_obj->apr_info.client_addr, session_obj->apr_info.client_port,
           packet->token, VSS_IPKTEXG_EVT_IN_BAND_SEND_ENC_BUFFER,
           data_size + sizeof(vss_ipktexg_evt_in_band_send_enc_buffer_t ), &inband_enc_packet );
    if( NULL == inband_enc_packet )
    {
      VSM_REPORT_FATAL_ON_ERROR(rc);
      break;
    }

    inband_enc_payload = APRV2_PKT_GET_PAYLOAD( vss_ipktexg_evt_in_band_send_enc_buffer_t, inband_enc_packet );
    inband_enc_payload->media_id = enc_payload->media_type;
    ( void ) mmstd_memcpy(
               (uint8_t*)inband_enc_payload + sizeof(vss_ipktexg_evt_in_band_send_enc_buffer_t), data_size,
               (uint8_t*)enc_payload + sizeof(vsm_send_enc_packet_v2_t), data_size );
    rc = elite_apr_if_cmd_fwd( vsm_apr_handle, inband_enc_packet);
    VSM_COMM_ERROR( rc, session_obj->apr_info.client_addr );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "VSM: TX_INBAND_PKTEXG: data_size:%d", data_size );

    break;
  }

  ( void ) vsm_utility_log_enc_packet( session_obj, packet );

  return ADSP_EOK;
}

ADSPResult vsm_action_rx_inband_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
)
{
  int32_t rc = ADSP_EOK;

  ( void ) vsm_utility_log_dec_packet( session_obj, packet );

  /* The idea is keep dataQ depth 1, empty the queue first and then push back the pkt
     and have the latest pkt in case of overflow. */
  elite_msg_any_t prevMsg;
  elite_apr_packet_t *prevMsgPayload;
  rc = qurt_elite_queue_pop_front( session_obj->vdec->dataQ, ( uint64_t* )&prevMsg );
  if ( ADSP_EOK == rc )
  {
    prevMsgPayload = ( elite_apr_packet_t* ) prevMsg.pPayload;
    if ( NULL != prevMsgPayload )
    {
      elite_apr_if_free( vsm_apr_handle, prevMsgPayload );
    }
  }

  //Forward to Dynamic services.
  elite_msg_any_t msg;
  msg.unOpCode = ELITE_APR_PACKET;
  msg.pPayload = ( void* ) packet;
  if ( ADSP_EOK != qurt_elite_queue_push_back( session_obj->vdec->dataQ, ( uint64_t* )&msg ) )
  {
    rc = elite_apr_if_free( vsm_apr_handle, packet );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: RX_INBAND_PKEEXG: qurt_elite_queue_push_back error = %d!!\n", ( int )rc );
  }

  return rc;
}

ADSPResult vsm_action_tx_oob_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
)
{
  int32_t rc = ADSP_EOK;
  vsm_send_enc_packet_v2_t* enc_payload = NULL;
  uint32_t* oob_enc_buf;
  vsm_oob_pkt_exchange_header_t* oob_enc_payload = NULL;
  uint64_t enc_buf_virt_addr = 0;
  uint32_t enc_buf_size;

  for ( ;; )
  {
    if ( FALSE == session_obj->packet_exchange_info.oob_info.is_enc_buf_consumed )
    {
      VSM_REPORT_FATAL_ON_ERROR( ADSP_EFAILED );
      break;
    }

    enc_payload = APRV2_PKT_GET_PAYLOAD(vsm_send_enc_packet_v2_t, packet);
    oob_enc_buf = ( uint32_t* )session_obj->packet_exchange_info.oob_info.config.enc_buf_addr_lsw;
    oob_enc_payload = ( ( vsm_oob_pkt_exchange_header_t* ) oob_enc_buf );

    oob_enc_payload->time_stamp = ( ( uint32_t) enc_payload->timestamp_us);
    oob_enc_payload->media_type = enc_payload->media_type;
    oob_enc_payload->size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(packet->header)-sizeof(vsm_send_enc_packet_v2_t);
    ( void ) mmstd_memcpy(
      ( (uint8_t*)oob_enc_payload + sizeof(vsm_oob_pkt_exchange_header_t) ), oob_enc_payload->size,
      ( (uint8_t*)enc_payload + sizeof( vsm_send_enc_packet_v2_t ) ), oob_enc_payload->size );

    enc_buf_virt_addr = ( uint64_t ) session_obj->packet_exchange_info.oob_info.config.enc_buf_addr_msw;
    enc_buf_virt_addr = ( enc_buf_virt_addr << 32 ) | session_obj->packet_exchange_info.oob_info.config.enc_buf_addr_lsw;
    enc_buf_size = session_obj->packet_exchange_info.oob_info.config.enc_buf_size;
    cvd_mem_mapper_cache_flush( enc_buf_virt_addr, enc_buf_size );

    /* Forward event to client. */
    rc = elite_apr_if_alloc_send_event( session_obj->apr_info.apr_handle,
           session_obj->apr_info.self_addr, session_obj->apr_info.self_port,
           session_obj->apr_info.client_addr, session_obj->apr_info.client_port,
           packet->token, VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_READY, NULL, 0 );
    VSM_REPORT_FATAL_ON_ERROR(rc);

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "VSM: TX_OOB_PKTEXG: data_size=%d", oob_enc_payload->size );

    session_obj->packet_exchange_info.oob_info.is_enc_buf_consumed = FALSE;
    break;
  }

  ( void ) vsm_utility_log_enc_packet( session_obj, packet );

  return rc;
}

ADSPResult vsm_action_rx_oob_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
)
{
  int32_t rc = ADSP_EOK;
  vss_ipktexg_evt_in_band_send_dec_buffer_t* inband_dec_payload = NULL;
  elite_apr_packet_t* dec_packet = NULL;
  vsm_oob_pkt_exchange_header_t* oob_dec_payload = NULL;
  uint32_t* oob_dec_buf = NULL;
  uint64_t dec_buf_virt_addr = 0;
  uint32_t dec_buf_size;

  dec_buf_virt_addr = ( uint64_t ) session_obj->packet_exchange_info.oob_info.config.dec_buf_addr_msw;
  dec_buf_virt_addr = ( dec_buf_virt_addr << 32 ) | session_obj->packet_exchange_info.oob_info.config.dec_buf_addr_lsw;
  dec_buf_size = session_obj->packet_exchange_info.oob_info.config.dec_buf_size;
  cvd_mem_mapper_cache_invalidate( dec_buf_virt_addr, dec_buf_size );

  oob_dec_buf = ( uint32_t* )session_obj->packet_exchange_info.oob_info.config.dec_buf_addr_lsw;
  oob_dec_payload = ( ( vsm_oob_pkt_exchange_header_t* ) oob_dec_buf );

  if( oob_dec_payload->size > (dec_buf_size - sizeof(vsm_oob_pkt_exchange_header_t) ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: RX_OOB_PKTEXG: invalid payload_size=0x%x", oob_dec_payload->size );
    return ADSP_EBADPARAM;
  }

  rc = elite_apr_if_alloc_event( session_obj->apr_info.apr_handle,
         session_obj->apr_info.client_addr, session_obj->apr_info.client_port,
         session_obj->apr_info.self_addr, session_obj->apr_info.self_port,
         packet->token, VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER,
         oob_dec_payload->size + sizeof(vss_ipktexg_evt_in_band_send_dec_buffer_t), &dec_packet );
  if( NULL == dec_packet )
  {
    VSM_REPORT_FATAL_ON_ERROR(rc);
    return ADSP_EFAILED;
  }

  inband_dec_payload = APRV2_PKT_GET_PAYLOAD( vss_ipktexg_evt_in_band_send_dec_buffer_t, dec_packet );
  inband_dec_payload->media_id = oob_dec_payload->media_type;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "VSM: RX_OOB_PKTEXG: data_size:%d", oob_dec_payload->size );

  ( void ) mmstd_memcpy(
             (uint8_t*)inband_dec_payload + sizeof(vss_ipktexg_evt_in_band_send_dec_buffer_t), oob_dec_payload->size,
             (uint8_t*)oob_dec_payload + sizeof(vsm_oob_pkt_exchange_header_t), oob_dec_payload->size );

  ( void ) vsm_utility_log_dec_packet( session_obj, dec_packet );

  elite_msg_any_t msg;
  msg.unOpCode = ELITE_APR_PACKET;
  msg.pPayload = (void*) dec_packet;
  if ( ADSP_EOK != qurt_elite_queue_push_back( session_obj->vdec->dataQ, ( uint64_t* )&msg ) )
  {
    rc = elite_apr_if_free( vsm_apr_handle, dec_packet );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL,
           "VSM: RX_OOB_PKTEXG: qurt_elite_queue_push_back error = %d!!\n", rc);
  }

  return rc;
}

ADSPResult vsm_action_tx_mailbox_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
)
{
  uint32_t rc = ADSP_EFAILED;
  vsm_send_enc_packet_v2_t* enc_payload = NULL;

  bool_t isVocRequestPending = FALSE;
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* tx_circ_buf;
  cvd_virt_addr_t tx_circ_buf_virt_addr;
  vsm_mailbox_packet_exchange_config_t* mailbox_config;
  vss_ipktexg_mailbox_enc_request_t* enc_req;
  vss_ipktexg_mailbox_enc_packet_t* mailbox_enc_packet;
  cvd_virt_addr_t enc_packet_virt_addr;

  for( ;; )
  {
    mailbox_config = &session_obj->packet_exchange_info.mailbox_info.config;

    tx_circ_buf = mailbox_config->tx_circ_buf;
    tx_circ_buf_virt_addr.ptr = tx_circ_buf;
    ( void ) cvd_mem_mapper_cache_invalidate_v2( &tx_circ_buf_virt_addr,
               mailbox_config->tx_circ_buf_mem_size );

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "VSM: TX_MAILBOX_PKTEXG: txWriteOffset=%d, txReadOffset=%d", 
           (uint32_t)tx_circ_buf->write_offset, (uint32_t)tx_circ_buf->read_offset );

    isVocRequestPending = vsm_utility_is_mailbox_voc_req_pending ( tx_circ_buf,
                            mailbox_config->tx_circ_buf_mem_size,
                            sizeof( vss_ipktexg_mailbox_enc_request_t ) );
    if( FALSE == isVocRequestPending )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: TX_MAILBOX_PKTEXG: Client error: No pending encoder request."
           "Dropping encoder packet." );
      rc = ADSP_EFAILED;
      break;
    }

    /* Offset to pending Request. */
    enc_req = (vss_ipktexg_mailbox_enc_request_t*)
              ( ( (uint8_t*)tx_circ_buf ) + sizeof(vss_ipktexg_mailbox_voc_req_circ_buffer_t) + tx_circ_buf->read_offset );

    rc = vsm_utility_get_mailbox_enc_buffer( session_obj, enc_req, &mailbox_enc_packet );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: TX_MAILBOX_PKTEXG: Client error: Failed to get encoder buffer, "
           "rc = 0x%08X. Dropping encoder packet." );

      ( void ) vsm_utility_complete_pending_voc_request (
                 tx_circ_buf, mailbox_config->tx_circ_buf_mem_size,
                 sizeof( vss_ipktexg_mailbox_enc_request_t ) );
      break;
    }

    enc_payload = APRV2_PKT_GET_PAYLOAD(vsm_send_enc_packet_v2_t, packet);

    mailbox_enc_packet->minor_version = 0;
    mailbox_enc_packet->capture_timestamp_us = enc_payload->timestamp_us;
    mailbox_enc_packet->status_mask = 0;
    mailbox_enc_packet->media_id = enc_payload->media_type;
    mailbox_enc_packet->data_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(packet->header)-sizeof(vsm_send_enc_packet_v2_t);

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "VSM: TX_MAILBOX_PKTEXG: data_size=%d", mailbox_enc_packet->data_size );

    ( void ) mmstd_memcpy(
      ( (uint8_t*)mailbox_enc_packet + sizeof(vss_ipktexg_mailbox_enc_packet_t) ), mailbox_enc_packet->data_size,
      ( (uint8_t*)enc_payload + sizeof( vsm_send_enc_packet_v2_t ) ), mailbox_enc_packet->data_size );

    enc_packet_virt_addr.ptr = mailbox_enc_packet;
    ( void ) cvd_mem_mapper_cache_flush_v2( &enc_packet_virt_addr, enc_req->mem_size );

    ( void ) vsm_utility_complete_pending_voc_request( tx_circ_buf,
                mailbox_config->tx_circ_buf_mem_size,
                sizeof( vss_ipktexg_mailbox_enc_request_t ) );

    break;
  }

  ( void ) vsm_utility_log_enc_packet( session_obj, packet );

  return rc;
}

ADSPResult vsm_action_rx_mailbox_pktexg (
  vsm_session_object_t* session_obj,
  elite_apr_packet_t* packet
)
{
  uint32_t rc = ADSP_EOK;
  bool_t isVocRequestPending = FALSE;
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* rx_circ_buf;
  cvd_virt_addr_t rx_circ_buf_virt_addr;
  vsm_mailbox_packet_exchange_config_t* mailbox_config;
  vss_ipktexg_mailbox_dec_request_t* dec_req;
  vss_ipktexg_mailbox_dec_packet_t* mailbox_dec_packet;
  vss_ipktexg_evt_in_band_send_dec_buffer_t* inband_dec_packet = NULL;
  elite_apr_packet_t* dec_packet = NULL;

  uint8_t num_of_dec_pkts_per_vfr_cycle = session_obj->target_set.voice_timing.vfr_cycle_duration_us/20000;
  for( ; num_of_dec_pkts_per_vfr_cycle>0; --num_of_dec_pkts_per_vfr_cycle )
  {
    mailbox_config = &session_obj->packet_exchange_info.mailbox_info.config;
    rx_circ_buf = mailbox_config->rx_circ_buf;
    rx_circ_buf_virt_addr.ptr = rx_circ_buf;

    ( void ) cvd_mem_mapper_cache_invalidate_v2( &rx_circ_buf_virt_addr,
                mailbox_config->rx_circ_buf_mem_size );

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "VSM: RX_MAILBOX_PKTEXG: rxWriteOffset=%d, rxReadOffset=%d", 
           (uint32_t)rx_circ_buf->write_offset, (uint32_t)rx_circ_buf->read_offset );

    isVocRequestPending = vsm_utility_is_mailbox_voc_req_pending(
                            rx_circ_buf, mailbox_config->rx_circ_buf_mem_size,
                            sizeof( vss_ipktexg_mailbox_dec_request_t ) );
    if ( FALSE == isVocRequestPending )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: RX_MAILBOX_PKTEXG: Client error: No pending request." );
      rc = ADSP_EFAILED;
      break;
    }

    dec_req = (vss_ipktexg_mailbox_dec_request_t*)
              ( (uint8_t*)rx_circ_buf + sizeof(vss_ipktexg_mailbox_voc_req_circ_buffer_t) + rx_circ_buf->read_offset );

    rc = vsm_utility_get_mailbox_dec_buffer( session_obj, dec_req, &mailbox_dec_packet );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: RX_MAILBOX_PKTEXG: Client error: Failed to get decoder buffer, "
             "rc = 0x%08X.", rc );
      ( void ) vsm_utility_complete_pending_voc_request(
                 rx_circ_buf, mailbox_config->rx_circ_buf_mem_size, 
                 sizeof( vss_ipktexg_mailbox_dec_request_t ) );
      break;
    }

    if (VSS_MEDIA_ID_DTMF == mailbox_dec_packet->media_id)
    {
      rc = vsm_action_send_dtmf_gen_msg_to_afe(session_obj, (uint8_t *)mailbox_dec_packet, mailbox_dec_packet->data_size, num_of_dec_pkts_per_vfr_cycle);

      ( void )vsm_utility_log_dtmf_mailbox_pkt_gen_no_data_pkt(session_obj, mailbox_dec_packet, packet->token, &dec_packet );
    }
    else
    {
      rc = elite_apr_if_alloc_event( session_obj->apr_info.apr_handle,
             session_obj->apr_info.client_addr, session_obj->apr_info.client_port,
             session_obj->apr_info.self_addr, session_obj->apr_info.self_port,
             packet->token, VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER,
             mailbox_dec_packet->data_size + sizeof(vss_ipktexg_evt_in_band_send_dec_buffer_t), &dec_packet );
      VSM_REPORT_FATAL_ON_ERROR(rc);

      if( NULL != dec_packet )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
               "VSM: RX_MAILBOX_PKTEXG: data_size=%d", mailbox_dec_packet->data_size );

        inband_dec_packet = APRV2_PKT_GET_PAYLOAD( vss_ipktexg_evt_in_band_send_dec_buffer_t, dec_packet );
        inband_dec_packet->media_id = mailbox_dec_packet->media_id;
        ( void ) mmstd_memcpy(
                   (uint8_t*)inband_dec_packet + sizeof(vss_ipktexg_evt_in_band_send_dec_buffer_t),
                   mailbox_dec_packet->data_size,
                   (uint8_t*)mailbox_dec_packet + sizeof(vss_ipktexg_mailbox_dec_packet_t),
                   mailbox_dec_packet->data_size );

        ( void ) vsm_utility_log_dec_packet( session_obj, dec_packet );
      }
    }

    ( void ) vsm_utility_complete_pending_voc_request(
               rx_circ_buf, mailbox_config->rx_circ_buf_mem_size, 
               sizeof( vss_ipktexg_mailbox_dec_request_t ) );

    if( NULL != dec_packet )
    {
      elite_msg_any_t msg;
      msg.unOpCode = ELITE_APR_PACKET;
      msg.pPayload = (void*) dec_packet;
      if ( ADSP_EOK != qurt_elite_queue_push_back( session_obj->vdec->dataQ, ( uint64_t* )&msg ) )
      {
        rc = elite_apr_if_free( vsm_apr_handle, dec_packet );
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL,
               "VSM: RX_MAILBOX_PKTEXG: qurt_elite_queue_push_back error = %d!!\n", rc);
      }
    }
  }

  return rc;
}

void vsm_action_toggle_csm (
  vsm_session_object_t* session_obj,
  bool_t status
)
{
  uint32_t rc = ADSP_EOK;
  vss_icommon_evt_toggle_csm_t param;

  param.status = status;
  rc = elite_apr_if_alloc_send_event(
           vsm_apr_handle,
           session_obj->apr_info.self_addr, session_obj->apr_info.self_port,
           vsm_mvm_addr, session_obj->attached_mvm_handle,
           APR_NULL_V, VSS_ICOMMON_EVT_TOGGLE_CSM, 
           &param, sizeof( vss_icommon_evt_toggle_csm_t ) );
  VSM_REPORT_FATAL_ON_ERROR(rc);
}

ADSPResult vsm_action_publish_evs_params (
  vsm_session_object_t* session_obj
)
{
  vsm_evs_params_t event_payload;

  /* populate and publish EVS params. */
  event_payload.vsid = session_obj->target_set.voice_timing.vsid;
  event_payload.encoder_bandwidth = session_obj->cached_voc_properties_evs.bandwidth;
  event_payload.decoder_bandwidth = session_obj->evs_bandwidth_change_notification_info.last_received_rx_bandwidth;
  event_payload.encoder_dtx = (uint8) session_obj->cached_voc_properties_evs.dtx_mode;
  event_payload.encoder_rate = session_obj->cached_voc_properties_evs.mode;
  event_payload.channel_awareness_mode = session_obj->cached_voc_properties_evs.channel_aware_enabled;
  event_payload.fec_offset = session_obj->cached_voc_properties_evs.fec_offset;
  event_payload.fer_rate = session_obj->cached_voc_properties_evs.fer_rate;

  ( void ) event_report_payload( (event_id_enum_type) VSM_EVENT_VOICE_EVS_PARAMS,
                                  sizeof(event_payload), &event_payload );

  return ADSP_EOK;
}

ADSPResult vsm_action_send_dtmf_gen_msg_to_afe(
  vsm_session_object_t* session_obj,
  uint8_t * mailbox_dec_buf_ptr,
  int32_t mailbox_dec_buf_size,
  int8_t mailbox_buffer_index
)
{
  uint32_t rc = ADSP_EOK;
  vsm_afe_dtmf_tone_generate_payload_t dtmf_payload;
  uint32_t rx_path_delay;
  vocsvc_avtimer_timestamp_t time;
  elite_msg_any_t dtmf_gen_msg_to_afe;
  uint16_t afe_port;
  elite_msg_custom_afe_tone_generate_cfg_t *payload_ptr = NULL;
  uint32_t size = sizeof( elite_msg_custom_afe_tone_generate_cfg_t );
  uint32_t i;
  uint8_t num_of_dec_pkts_per_vfr_cycle = session_obj->target_set.voice_timing.vfr_cycle_duration_us/20000;
  vsm_afe_dtmf_tone_generate_payload_t *temp_ptr = NULL;

  /*send custom command to AFE*/
  for (i = 0; i < VSM_MAX_ATTACHED_VOCPROC_CNT;i++)
  {
    afe_port =  session_obj->active_set.attach_table.rx_port_id[i];
    if (APR_NULL_V != afe_port)
    {
      rc = elite_msg_create_msg( &dtmf_gen_msg_to_afe,
                               &size,
                               ELITE_CUSTOM_MSG,
                               NULL,
                               0,
                               ADSP_EOK );
      if( ADSP_FAILED( rc ) )
      {
        MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM: RX_MAILBOX_PKTEXG: vsm_action_send_dtmf_gen_msg_to_afe:FAILED to create EliteMsg!!\n" );
        return rc;
      }
      rc = vocsvc_avtimer_get_time( &time );
      rx_path_delay = session_obj->avsync_delay_info.total_rx_delay;
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"VSM: RX_MAILBOX_PKTEXG: vsm_action_send_dtmf_gen_msg_to_afe:total rx delay: %d!!\n", rx_path_delay );
      time.timestamp_us = time.timestamp_us + rx_path_delay + (num_of_dec_pkts_per_vfr_cycle - mailbox_buffer_index) * 20000;
      payload_ptr = ( elite_msg_custom_afe_tone_generate_cfg_t* )dtmf_gen_msg_to_afe.pPayload;
      payload_ptr->sec_op_code = ELITEMSG_CUSTOM_TIMED_DTMF_CONFIG;
      payload_ptr->avtimer_timestamp_lsw = (uint32_t)time.timestamp_us;
      payload_ptr->avtimer_timestamp_msw = (uint32_t)(time.timestamp_us >>32);
      
      /* Copy the payload configuration for dtmf tone generation. */
	  mmstd_memset(&dtmf_payload, 0, sizeof(vsm_afe_dtmf_tone_generate_payload_t) );
      temp_ptr = ( vsm_afe_dtmf_tone_generate_payload_t* )( mailbox_dec_buf_ptr + sizeof(vss_ipktexg_mailbox_dec_packet_t) );      
      mmstd_memcpy(&dtmf_payload, sizeof(vsm_afe_dtmf_tone_generate_payload_t),
      							temp_ptr, sizeof(vsm_afe_dtmf_tone_generate_payload_t) );
      payload_ptr->dtmf_duration_in_ms = dtmf_payload.dtmf_duration_in_ms;
      payload_ptr->dtmf_gain = dtmf_payload.dtmf_gain;      
      payload_ptr->dtmf_high_freq = dtmf_payload.dtmf_high_freq;
      payload_ptr->dtmf_low_freq = dtmf_payload.dtmf_low_freq;
      payload_ptr->mix_enable = dtmf_payload.mix_enable;    
      payload_ptr->num_ports = 1;
      payload_ptr->port_ids = afe_port;

      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"VSM: RX_MAILBOX_PKTEXG: vsm_action_send_dtmf_gen_msg_to_afe:sending DTMF gen msg to port: %d!!\n", afe_port );
      rc = qurt_elite_queue_push_back( session_obj->vsm_afe_cmd_q_ptr, ( uint64_t* )&dtmf_gen_msg_to_afe );
      if ( ADSP_FAILED( rc ) )
      {
        MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VSM: RX_MAILBOX_PKTEXG: vsm_action_send_dtmf_gen_msg_to_afe:FAILED to push to afe cmdQ. error %d!!\n", rc );
        elite_msg_return_payload_buffer( &dtmf_gen_msg_to_afe );
        return rc;
      }
    }
  }
  return rc;
}
