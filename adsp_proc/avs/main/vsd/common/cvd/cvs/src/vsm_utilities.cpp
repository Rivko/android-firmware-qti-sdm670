/*
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvs/src/vsm_utilities.cpp#2 $
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
#include "vsm_apr_router.h"
#include "cvd_log_i.h"
#include "EliteMsg_AfeCustom.h"

//TBD: Need to avoid this extern reference.
extern voice_stream_mgr_t voice_stream_manager;

extern elite_apr_handle_t vsm_apr_handle;
extern uint16_t vsm_mvm_addr;
extern uint16_t vsm_cvp_addr;
extern elite_svc_handle_t *txVoiceMatrix;
extern elite_svc_handle_t *rxVoiceMatrix;

/******************************Utilities **************
*******************************************************/

bool_t VSM_utility_ssr_matches_passive_clients_domain(
  uint8_t domain_id,
  vsm_session_object_t* session_object
)
{
  uint8_t index = 0;

  for ( index = 0; index < VSM_NUM_MAX_PASSIVE_CLIENTS; index ++ )
  {
    if ( APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, session_object->passive_clients[ index ].client_addr ) == domain_id )
    {
      return TRUE;
    }
  }

  return FALSE;
}

ADSPResult VSM_utility_remove_passive_client_info(
  uint16_t client_addr,
  uint16_t client_port,
  vsm_session_object_t* session_object
)
{
  uint8_t index = 0;

  for ( index = 0; index < VSM_NUM_MAX_PASSIVE_CLIENTS; index ++ )
  {
    if ( ( session_object->passive_clients[ index ].client_addr == client_addr ) &&
         ( session_object->passive_clients[ index ].client_port == client_port ) )
    {
      session_object->passive_clients[ index ].client_addr = 0;
      session_object->passive_clients[ index ].client_port = 0;
      break;
    }
  }

  if ( VSM_NUM_MAX_PASSIVE_CLIENTS <= index )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_remove_passive_client_info:" \
         "PASSIVE CONTROL CLIENT not found" );
   return ADSP_EBADPARAM;
  }

  return ADSP_EOK;
}

void VSM_utility_update_passive_client_info(
  elite_apr_packet_t* apr_pkt,
  vsm_session_object_t* session_object
)
{
  uint8_t index = 0;

  for ( index = 0; index < VSM_NUM_MAX_PASSIVE_CLIENTS; index ++ )
  {
    if ( ( session_object->passive_clients[ index ].client_addr == 0 ) &&
         ( session_object->passive_clients[ index ].client_port == 0 ) )
    {
      session_object->passive_clients[ index ].client_addr = apr_pkt->src_addr;
      session_object->passive_clients[ index ].client_port = apr_pkt->src_port;
      break;
    }
  }

  if ( VSM_NUM_MAX_PASSIVE_CLIENTS <= index )
  {
   /* Assuming this condition wont be reached. If it reaches due to genuine reasons,
      we need to increase the limit VSM_NUM_MAX_PASSIVE_CLIENTS
   */
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_update_passive_client_info:" \
         "PASSIVE CONTROL CLIENT array limit exceeds" );
  }

}

bool_t VSM_utility_if_passive_client_already_exists(
  elite_apr_packet_t* apr_pkt,
  vsm_session_object_t* session_object
)
{
  uint8_t index = 0;

  for ( index = 0; index < VSM_NUM_MAX_PASSIVE_CLIENTS; index ++ )
  {
    if ( ( session_object->passive_clients[ index ].client_addr == apr_pkt->src_addr ) &&
         ( session_object->passive_clients[ index ].client_port == apr_pkt->src_port ) )
    {
      return TRUE;
    }
  }

  return FALSE;
}

bool_t vsm_utility_evs_validate_enc_operating_mode (
  uint32_t mode,
  uint32_t bandwidth,
  uint32_t requested_var_voc_tx_sampling_rate
)
{
  bool_t is_valid = FALSE;

  /* Making sure the modes and bandwidths requested are supported */
  switch ( mode )
  {
    case VSM_EVS_OPERATING_MODE_AMRWB_IO_6_60_KBPS:
    case VSM_EVS_OPERATING_MODE_AMRWB_IO_8_85_KBPS:
    case VSM_EVS_OPERATING_MODE_AMRWB_IO_12_65_KBPS:
    case VSM_EVS_OPERATING_MODE_AMRWB_IO_14_25_KBPS:
    case VSM_EVS_OPERATING_MODE_AMRWB_IO_15_85_KBPS:
    case VSM_EVS_OPERATING_MODE_AMRWB_IO_18_25_KBPS:
    case VSM_EVS_OPERATING_MODE_AMRWB_IO_19_85_KBPS:
    case VSM_EVS_OPERATING_MODE_AMRWB_IO_23_05_KBPS:
    case VSM_EVS_OPERATING_MODE_AMRWB_IO_23_85_KBPS:
      {
        if ( VSS_ISTREAM_EVS_VOC_BANDWIDTH_WB == bandwidth )
        {
          is_valid = TRUE;
        }
      }
      break;

    case VSM_EVS_OPERATING_MODE_EVS_5_90_KBPS:
    case VSM_EVS_OPERATING_MODE_EVS_7_20_KBPS:
    case VSM_EVS_OPERATING_MODE_EVS_8_00_KBPS:
      {
        if ( VSS_ISTREAM_EVS_VOC_BANDWIDTH_WB >= bandwidth )
        {
          is_valid = TRUE;
        }
      }
      break;

    case VSM_EVS_OPERATING_MODE_EVS_9_60_KBPS:
    case VSM_EVS_OPERATING_MODE_EVS_13_20_KBPS:
      {
        if ( VSS_ISTREAM_EVS_VOC_BANDWIDTH_SWB >= bandwidth )
        {
          is_valid = TRUE;
        }
      }
      break;

    case VSM_EVS_OPERATING_MODE_EVS_16_40_KBPS:
    case VSM_EVS_OPERATING_MODE_EVS_24_40_KBPS:
      {
        if ( VSS_ISTREAM_EVS_VOC_BANDWIDTH_FB >= bandwidth )
        {
          is_valid = TRUE;
        }
      }
      break;

    case VSM_EVS_OPERATING_MODE_EVS_32_00_KBPS:
    case VSM_EVS_OPERATING_MODE_EVS_48_00_KBPS:
    case VSM_EVS_OPERATING_MODE_EVS_64_00_KBPS:
    case VSM_EVS_OPERATING_MODE_EVS_96_00_KBPS:
    case VSM_EVS_OPERATING_MODE_EVS_128_00_KBPS:
      {
        if ( VSS_ISTREAM_EVS_VOC_BANDWIDTH_NB != bandwidth )
        {
          is_valid = TRUE;
        }
      }
      break;

    default:
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_evs_validate_enc_operating_mode:" \
               "Invalid Bandwidth=%d for mode=%d", bandwidth, mode );
        is_valid = FALSE;
      }
      break;
  }

  /* Need to validate bandwidth relative to Tx sampling rate */
  if ( is_valid )
  {
    is_valid = FALSE;
    switch ( bandwidth )
    {
      case VSS_ISTREAM_EVS_VOC_BANDWIDTH_NB:
        {
          if ( requested_var_voc_tx_sampling_rate >= 8000 )
          {
            is_valid = TRUE;
          }
        }
        break;

      case VSS_ISTREAM_EVS_VOC_BANDWIDTH_WB:
        {
          if ( requested_var_voc_tx_sampling_rate >= 16000 )
          {
            is_valid = TRUE;
          }
        }
        break;

      case VSS_ISTREAM_EVS_VOC_BANDWIDTH_SWB:
        {
          if ( requested_var_voc_tx_sampling_rate >= 32000 )
          {
            is_valid = TRUE;
          }
        }
        break;

      case VSS_ISTREAM_EVS_VOC_BANDWIDTH_FB:
        {
          if ( requested_var_voc_tx_sampling_rate >= 48000 )
          {
            is_valid = TRUE;
          }
        }
        break;

      default:
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_evs_validate_enc_operating_mode:" \
                 "Invalid Bandwidth=%d for Tx Sampling Rate=%d", bandwidth, requested_var_voc_tx_sampling_rate );
          is_valid = FALSE;
        }
        break;
    }
  }

  if ( !is_valid )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_evs_validate_enc_operating_mode:" \
           "Invalid Bandwidth=%d for mode=%d and Tx Sampling Rate=%d",
           bandwidth, mode, requested_var_voc_tx_sampling_rate );
  }

  return is_valid;
}

ADSPResult vsm_utility_playback_control_stop( vsm_session_object_t* session_object )
{
  int32_t rc;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_utility_playback_control_stop:" \
         "playback enabled = %d playback state=%d",
         session_object->active_set.playback.enable_flag, session_object->active_set.playback.state );

  switch ( session_object->active_set.playback.state )
  {
    case VSM_PLAYBACK_STATE_PLAYING:
    {
      session_object->target_set.attach_detach_vocproc_handle =
           session_object->active_set.playback.vpm_port_id;

      rc = vsm_action_send_detach_to_vocproc( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_WAIT_ON_DETACH_VOCPROC;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_PLAYBACK_STATE_WAIT_ON_DETACH_VOCPROC:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_VOCPROC_DETACHED;
      }
    }
    //fall through

    case VSM_PLAYBACK_STATE_VOCPROC_DETACHED:
    {
      rc = vsm_action_update_mixer_port_mapping( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }

      rc = vsm_action_reassign_mixer_inout_ports( session_object, FALSE );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_WAIT_ON_MIXER_RESPONSE;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_PLAYBACK_STATE_WAIT_ON_MIXER_RESPONSE:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_MIXER_CONFIGURED;
      }
    }
    //fall through

    case VSM_PLAYBACK_STATE_MIXER_CONFIGURED:
    {
      rc = vsm_action_stop_vocproc( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_WAIT_ON_STOP_VOCPROC;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_PLAYBACK_STATE_WAIT_ON_STOP_VOCPROC:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_VOCPROC_STOPPED;
      }
    }
    //fall through

    case VSM_PLAYBACK_STATE_VOCPROC_STOPPED:
    {
      rc = vsm_action_destroy_vocproc( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_WAIT_ON_VOCPROC_DESTROY;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_PLAYBACK_STATE_WAIT_ON_VOCPROC_DESTROY:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_IDLE;
        return ADSP_EOK;
      }
    }
    break;

    default:
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_playback_control_stop: Unexpected state" \
         "playback enabled = %d playback state=%d",
         session_object->active_set.playback.enable_flag, session_object->active_set.playback.state );
    }
    break;
  }
  return ADSP_EFAILED;
}


ADSPResult vsm_utility_playback_control_start( vsm_session_object_t* session_object )
{
  int32_t rc;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_utility_playback_control_start:" \
         "playback enabled = %d playback state=%d",
         session_object->active_set.playback.enable_flag, session_object->active_set.playback.state );

  switch ( session_object->active_set.playback.state )
  {
    case VSM_PLAYBACK_STATE_IDLE:
    {
      rc = vsm_action_create_vocproc( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_WAIT_ON_VOCPROC_CREATE;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_PLAYBACK_STATE_WAIT_ON_VOCPROC_CREATE:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_VOCPROC_CREATED;
      }
    }
    //fall through

    case VSM_PLAYBACK_STATE_VOCPROC_CREATED:
    {
      session_object->target_set.attach_detach_vocproc_handle =
           session_object->active_set.playback.vpm_port_id;
      session_object->target_set.attach_detach_vocproc_direction = VSS_IVOCPROC_DIRECTION_TX;

      rc = vsm_action_playback_set_system_config( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_WAIT_ON_PLAYBACK_SET_SYSTEM_CONFIG;
        return ADSP_EPENDING;
      }
    }
    break;
    case VSM_PLAYBACK_STATE_WAIT_ON_PLAYBACK_SET_SYSTEM_CONFIG:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_ATTACH_VOCPROC;
      }
    }

    case VSM_PLAYBACK_STATE_ATTACH_VOCPROC:
    {
      rc = vsm_action_send_attach_to_vocproc( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_WAIT_ON_ATTACH_VOCPROC;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_PLAYBACK_STATE_WAIT_ON_ATTACH_VOCPROC:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_VOCPROC_ATTACHED;
      }
    }
    //fall through

    case VSM_PLAYBACK_STATE_VOCPROC_ATTACHED:
    {
      rc = vsm_action_update_mixer_port_mapping( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }

      rc = vsm_action_reassign_mixer_inout_ports( session_object, TRUE );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_WAIT_ON_MIXER_RESPONSE;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_PLAYBACK_STATE_WAIT_ON_MIXER_RESPONSE:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_MIXER_CONFIGURED;
      }
    }
    //fall through

    case VSM_PLAYBACK_STATE_MIXER_CONFIGURED:
    {
      rc = vsm_action_set_timing_to_vocproc( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_WAIT_ON_SET_TIMING;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_PLAYBACK_STATE_WAIT_ON_SET_TIMING:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_VOCPROC_TIMING_SET;
      }
    }
    //fall through

    case VSM_PLAYBACK_STATE_VOCPROC_TIMING_SET:
    {
      rc = vsm_action_start_vocproc( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_WAIT_ON_START_VOCPROC;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_PLAYBACK_STATE_WAIT_ON_START_VOCPROC:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.playback.state = VSM_PLAYBACK_STATE_PLAYING;
        return ADSP_EOK;
      }
    }
    break;

    default:
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_playback_control_start: Unexpected state" \
             "recording enabled = %d recording state=%d",
             session_object->active_set.playback.enable_flag, session_object->active_set.playback.state );
    }
    break;
  }
  return ADSP_EFAILED;
}

ADSPResult vsm_utility_record_control_stop( vsm_session_object_t* session_object )
{
  int32_t rc;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_utility_record_control_stop:" \
         "recording enabled = %d recording state=%d",
         session_object->active_set.record.enable_flag, session_object->active_set.record.state );

  switch ( session_object->active_set.record.state )
  {
    case VSM_RECORD_STATE_RECORDING:
    {
      rc = vsm_action_stop_record( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.record.state = VSM_RECORD_STATE_WAIT_ON_STOP_RECORDING;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_RECORD_STATE_WAIT_ON_STOP_RECORDING:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.record.state = VSM_RECORD_STATE_RECORDING_STOPPED;
      }
    }
    //fall through

    case VSM_RECORD_STATE_RECORDING_STOPPED:
    {
      rc = vsm_action_disconnect_afe( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.record.state = VSM_RECORD_STATE_WAIT_ON_AFE_DISCONNECT;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_RECORD_STATE_WAIT_ON_AFE_DISCONNECT:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->rx_afe_handle_ptr = NULL;
        session_object->tx_afe_handle_ptr = NULL;
        session_object->active_set.record.state = VSM_RECORD_STATE_IDLE;
        return ADSP_EOK;
      }
    }
    break;

    default:
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_record_control_start: Unexpected state" \
         "recording enabled = %d recording state=%d",
         session_object->active_set.record.enable_flag, session_object->active_set.record.state );
    }
    break;
  }
  return ADSP_EFAILED;
}


ADSPResult vsm_utility_record_control_start( vsm_session_object_t* session_object )
{
  int32_t rc;

  if( NULL == session_object )
  {
    return ADSP_EBADPARAM;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "vsm_utility_record_control_start:" \
         "recording enabled = %d recording state=%d",
         session_object->active_set.record.enable_flag, session_object->active_set.record.state );

  switch ( session_object->active_set.record.state )
  {
    case VSM_RECORD_STATE_IDLE:
    {
      rc = vsm_action_connect_afe( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.record.state = VSM_RECORD_STATE_WAIT_ON_AFE_CONNECT;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_RECORD_STATE_WAIT_ON_AFE_CONNECT:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.record.state = VSM_RECORD_STATE_AFE_CONNECTED;
      }
    }
    //fall through

    case VSM_RECORD_STATE_AFE_CONNECTED:
    {
      rc = vsm_action_start_record( session_object );
      if ( ADSP_FAILED( rc ) )
      {
        return rc;
      }
      else
      {
        session_object->active_set.record.state = VSM_RECORD_STATE_WAIT_ON_START_RECORDING;
        return ADSP_EPENDING;
      }
    }
    break;

    case VSM_RECORD_STATE_WAIT_ON_START_RECORDING:
    {
      if ( ADSP_FAILED( session_object->cmd_control.resp_result ) )
      {
        return session_object->cmd_control.resp_result;
      }
      else
      {
        session_object->active_set.record.state = VSM_RECORD_STATE_RECORDING;
        return ADSP_EOK;
      }
    }
    break;

    default:
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_record_control_start: Unexpected state" \
         "recording enabled = %d recording state=%d",
         session_object->active_set.record.enable_flag, session_object->active_set.record.state );
    }
    break;
  }
  return ADSP_EFAILED;
}

bool_t vsm_utility_verify_param_data_size_aligned (
  uint32_t param_size
)
{
  /* By design, all voice parameter size must be multiple of 4 bytes. */
  if ( param_size & 0x3 )
  {
    return FALSE;
  }

  return TRUE;
}

bool_t vsm_utility_evs_validate_enc_channel_aware_mode(
  uint8_t fec_offset,
  uint8_t fer_rate
)
{
  bool_t is_valid = FALSE;

  switch ( fec_offset )
  {
  case VSM_EVS_FEC_OFFSET_2:
  case VSM_EVS_FEC_OFFSET_3:
  case VSM_EVS_FEC_OFFSET_5:
  case VSM_EVS_FEC_OFFSET_7:
    {
      is_valid = TRUE;
    }
    break;

  default:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_evs_validate_enc_channel_aware_mode: " \
             "Bad FEC Offset value=%d", fec_offset );
      is_valid = FALSE;
    }
    break;
  }

  if ( is_valid )
  {
    switch ( fer_rate )
    {
    case VSM_EVS_FER_RATE_LOW:
    case VSM_EVS_FER_RATE_HIGH:
      {
        is_valid = TRUE;
      }
      break;
    default:
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_evs_validate_enc_channel_aware_mode: " \
               "Bad FER Rate value=%d", fer_rate );
        is_valid = FALSE;
      }
      break;
    }
  }

  return is_valid;
}

/* This utility is not implemented to support all available APIs.
   Developer shalleEnsure the availability of support for a particular opcode before using thsi utility.
*/
bool_t vsm_utility_validate_apr_payload_size( elite_apr_packet_t* apr_pkt )
{
  uint32_t payload_size;
  bool_t result = FALSE;
  uint32_t expected_payload_size = 0;

  payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

  switch( apr_pkt->opcode )
  {
    case VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE:
    {
      expected_payload_size = sizeof( vss_istream_cmd_cdma_set_enc_minmax_rate_t );
    }
    break;

    case VSS_ISTREAM_CMD_SET_ENC_DTX_MODE:
    {
      expected_payload_size = sizeof( vss_istream_cmd_set_enc_dtx_mode_t );
    }
    break;

    case VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION:
    {
      expected_payload_size = sizeof( vss_istream_cmd_cdma_set_enc_rate_modulation_t );
    }
    break;

    case VSS_ISTREAM_CMD_SET_EVS_VOC_ENC_OPERATING_MODE:
    {
      expected_payload_size = sizeof( vss_istream_cmd_set_evs_voc_enc_operating_mode_t );
    }
    break;

    case VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE:
    {
      expected_payload_size = sizeof( vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t );
    }
    break;

    case VSS_IPKTEXG_CMD_OOB_SET_CONFIG:
    {
      expected_payload_size = sizeof( vss_ipktexg_cmd_oob_set_config_t );
    }
    break;

    case VSS_ICOMMON_CMD_GET_PARAM_V2:
    {
      expected_payload_size = sizeof( vss_icommon_cmd_get_param_v2_t );
    }
    break;

    case VSS_ICOMMON_CMD_GET_PARAM_V3:
    {
      expected_payload_size = sizeof( vss_icommon_cmd_get_param_v3_t );
    }
    break;

    case VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS:
    {
      expected_payload_size = sizeof( vss_inotify_cmd_listen_for_event_class_t );
    }
    break;

    case VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS:
    {
      expected_payload_size = sizeof( vss_inotify_cmd_cancel_event_class_t );
    }
    break;

    case VSS_ISTREAM_CMD_SET_DTMF_GENERATION:
    {
      expected_payload_size = sizeof( vss_istream_cmd_set_dtmf_generation_t );
    }
    break;

    case VSS_ISTREAM_CMD_SET_RX_DTMF_DETECTION:
    {
      expected_payload_size = sizeof( vss_istream_cmd_set_rx_dtmf_detection_t );
    }
    break;

    case VSS_IVOLUME_CMD_MUTE_V2:
    {
      expected_payload_size = sizeof( vss_ivolume_cmd_mute_v2_t );
    }
    break;

    case VSS_IHDVOICE_CMD_SET_CONFIG:
    {
      expected_payload_size = sizeof( vss_ihdvoice_cmd_set_config_t );
    }
    break;

    case VSS_ISTREAM_CMD_START_RECORD:
    {
      expected_payload_size = sizeof( vss_istream_cmd_start_record_t );
    }
    break;

    case VSS_IRECORD_CMD_START:
    {
      expected_payload_size = sizeof( vss_irecord_cmd_start_t );
    }
    break;

    case VSS_IPLAYBACK_CMD_START:
    {
      expected_payload_size = sizeof( vss_iplayback_cmd_start_t );
    }
    break;

    case VSS_IPKTEXG_CMD_SET_MODE:
    {
      expected_payload_size = sizeof( vss_ipktexg_cmd_set_mode_t );
    }
    break;

    case VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG:
    {
      expected_payload_size = sizeof( vss_ipktexg_cmd_mailbox_set_config_t );
    }
    break;

  }

  if ( payload_size == expected_payload_size )
  {
    result = TRUE;
  }
  else
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_validate_apr_payload_size(): "
           "expected payload size %d, given payload size = %d", expected_payload_size, payload_size );
  }

  return result;
}

void vsm_utility_cache_enc_operating_mode (
  vsm_session_object_t* session_object,
  void* payload
)
{
  vss_istream_cmd_set_evs_voc_enc_operating_mode_t* in_args;

  if ( payload == NULL )
  {
    return;
  }

  in_args = ( vss_istream_cmd_set_evs_voc_enc_operating_mode_t* )payload;

  if ( VSS_MEDIA_ID_EVS == session_object->active_set.media_id )
  {
    session_object->cached_voc_properties_evs.mode = in_args->mode;
    session_object->cached_voc_properties_evs.bandwidth = in_args->bandwidth;

    /** Following fields are only applicable for encoder_rate  PRIMARY_13_20_kbps. */
    if ( 0x0D != session_object->cached_voc_properties_evs.mode )
    {
      session_object->cached_voc_properties_evs.channel_aware_enabled = VSM_STREAM_PROPERTY_NOT_APPLICABLE_UINT8;
      session_object->cached_voc_properties_evs.fec_offset = VSM_STREAM_PROPERTY_NOT_APPLICABLE_UINT8;
      session_object->cached_voc_properties_evs.fer_rate = VSM_STREAM_PROPERTY_NOT_APPLICABLE_UINT8;
    }

    if ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
    {
      ( void ) vsm_action_publish_evs_params( session_object );
    }
  }

  return;
}

void vsm_utility_cache_channel_aware_mode (
  vsm_session_object_t* session_object,
  void* payload
)
{
  vsm_enc_channel_aware_mode_params_t* in_args;

  if ( payload == NULL )
  {
    return;
  }

  in_args = ( vsm_enc_channel_aware_mode_params_t* )payload;

  if ( VSS_MEDIA_ID_EVS == session_object->active_set.media_id )
  {
    session_object->cached_voc_properties_evs.channel_aware_enabled = in_args->channel_aware_enabled;
    session_object->cached_voc_properties_evs.fec_offset = in_args->fec_offset;
    session_object->cached_voc_properties_evs.fer_rate = in_args->fer_rate;

    if ( VSM_STATE_ENUM_RUN == session_object->state_control.state )
    {
      ( void ) vsm_action_publish_evs_params( session_object );
    }
  }

  return;
}

void vsm_utility_cache_enc_rate ( 
  vsm_session_object_t* session_object, 
  void* payload
)
{
  vss_istream_cmd_voc_amr_set_enc_rate_t* in_args;

  if ( payload == NULL )
  {
    return;
  }
  in_args = ( vss_istream_cmd_voc_amr_set_enc_rate_t* )payload;

  switch ( session_object->active_set.media_id )
  {
    case VSS_MEDIA_ID_13K:
      session_object->cached_voc_properties_qcelp13k.rate = in_args->mode;
      break;

    case VSS_MEDIA_ID_4GV_NB:
      session_object->cached_voc_properties_4gvnb.rate = in_args->mode;
      break;

    case VSS_MEDIA_ID_4GV_WB:
      session_object->cached_voc_properties_4gvwb.rate = in_args->mode;
      break;

    case VSS_MEDIA_ID_4GV_NW:
      session_object->cached_voc_properties_4gvnw.rate = in_args->mode;
      break;

    case VSS_MEDIA_ID_4GV_NW2K:
      session_object->cached_voc_properties_4gvnw2k.rate = in_args->mode;
      break;

    case VSS_MEDIA_ID_AMR_NB:
      session_object->cached_voc_properties_amr.rate = in_args->mode;
      break;

    case VSS_MEDIA_ID_EAMR:
      session_object->cached_voc_properties_eamr.rate = in_args->mode;
      break;

    case VSS_MEDIA_ID_AMR_WB:
      session_object->cached_voc_properties_amrwb.rate = in_args->mode;
      break;
  }

}

void vsm_utility_cache_minmax_rate (
  vsm_session_object_t* session_object, 
  void* payload
)
{
  vss_istream_cmd_cdma_set_enc_minmax_rate_t* in_args;

  if ( payload == NULL )
  {
    return;
  }
  in_args = ( vss_istream_cmd_cdma_set_enc_minmax_rate_t* )payload;

  /* Cache property. */
  switch ( session_object->active_set.media_id )
  {
    case VSS_MEDIA_ID_13K:
      session_object->cached_voc_properties_qcelp13k.min_rate = in_args->min_rate;
      session_object->cached_voc_properties_qcelp13k.max_rate = in_args->max_rate;
      break;

    case VSS_MEDIA_ID_EVRC:
      session_object->cached_voc_properties_evrc.min_rate = in_args->min_rate;
      session_object->cached_voc_properties_evrc.max_rate = in_args->max_rate;
      break;

    case VSS_MEDIA_ID_4GV_NB:
      session_object->cached_voc_properties_4gvnb.min_rate = in_args->min_rate;
      session_object->cached_voc_properties_4gvnb.max_rate = in_args->max_rate;
      break;

    case VSS_MEDIA_ID_4GV_WB:
      session_object->cached_voc_properties_4gvwb.min_rate = in_args->min_rate;
      session_object->cached_voc_properties_4gvwb.max_rate = in_args->max_rate;
      break;

    case VSS_MEDIA_ID_4GV_NW:
      session_object->cached_voc_properties_4gvnw.min_rate = in_args->min_rate;
      session_object->cached_voc_properties_4gvnw.max_rate = in_args->max_rate;
      break;

    case VSS_MEDIA_ID_4GV_NW2K:
      session_object->cached_voc_properties_4gvnw2k.min_rate = in_args->min_rate;
      session_object->cached_voc_properties_4gvnw2k.max_rate = in_args->max_rate;
      break;
  }

}

void vsm_utility_cache_rate_modulation (
  vsm_session_object_t* session_object,
  void* payload
)
{
  vss_istream_cmd_cdma_set_enc_rate_modulation_t* in_args;

  if ( payload == NULL )
  {
    return;
  }
  in_args = ( vss_istream_cmd_cdma_set_enc_rate_modulation_t* )payload;

  switch ( session_object->active_set.media_id )
  {
    case VSS_MEDIA_ID_13K:
      session_object->cached_voc_properties_qcelp13k.reduced_rate_mode = in_args->mode;
      break;

    case VSS_MEDIA_ID_EVRC:
      session_object->cached_voc_properties_evrc.reduced_rate_mode = in_args->mode;
      break;
  }

}

void vsm_utility_cache_dtx_mode (
  vsm_session_object_t* session_object,
  bool_t is_full_ctrl,
  void* payload
)
{
  vss_istream_cmd_set_enc_dtx_mode_t* in_args;

  if ( payload == NULL )
  {
    return;
  }

  in_args = ( vss_istream_cmd_set_enc_dtx_mode_t* )payload;
  if ( FALSE == is_full_ctrl )
  {
    session_object->cached_voc_properties_passive_ctrl.dtx_mode = in_args->enable;
  }
  else 
  {
    switch ( session_object->active_set.media_id )
    {
      case VSS_MEDIA_ID_AMR_NB:
        session_object->cached_voc_properties_amr.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_AMR_WB:
        session_object->cached_voc_properties_amrwb.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_EAMR:
        session_object->cached_voc_properties_eamr.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_4GV_NW:
        session_object->cached_voc_properties_4gvnw.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_4GV_NW2K:
        session_object->cached_voc_properties_4gvnw2k.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_EFR:
        session_object->cached_voc_properties_efr.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_FR:
        session_object->cached_voc_properties_fr.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_HR:
        session_object->cached_voc_properties_hr.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_G711_ALAW:
        session_object->cached_voc_properties_g711_alaw.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_G711_MULAW:
        session_object->cached_voc_properties_g711_mulaw.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_G711_ALAW_V2:
        session_object->cached_voc_properties_g711_alaw_v2.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_G711_MULAW_V2:
        session_object->cached_voc_properties_g711_mulaw_v2.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_G711_LINEAR:
        session_object->cached_voc_properties_g711_linear.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_G729:
        session_object->cached_voc_properties_g729.dtx_mode = in_args->enable;
        break;

      case VSS_MEDIA_ID_EVS:
        session_object->cached_voc_properties_evs.dtx_mode = in_args->enable;
        if( VSM_STATE_ENUM_RUN == session_object->state_control.state )
        {
          ( void ) vsm_action_publish_evs_params( session_object );
        }
        break;
    }
  }

  return;
}

void vsm_utility_get_cached_dtx_mode (
  vsm_session_object_t* session_object,
  vsm_enc_set_dtx_mode_t* out_args
)
{
  if ( NULL == session_object || NULL == out_args )
  {
    return;
  }

  switch ( session_object->active_set.media_id )
  {
    case VSS_MEDIA_ID_AMR_NB:
      out_args->dtx_mode = session_object->cached_voc_properties_amr.dtx_mode;
      break;

    case VSS_MEDIA_ID_AMR_WB:
      out_args->dtx_mode = session_object->cached_voc_properties_amrwb.dtx_mode;
      break;

    case VSS_MEDIA_ID_4GV_NW:
      out_args->dtx_mode = session_object->cached_voc_properties_4gvnw.dtx_mode;
      break;

    case VSS_MEDIA_ID_4GV_NW2K:
      out_args->dtx_mode = session_object->cached_voc_properties_4gvnw2k.dtx_mode;
      break;

    case VSS_MEDIA_ID_EAMR:
      out_args->dtx_mode = session_object->cached_voc_properties_eamr.dtx_mode;
      break;

    case VSS_MEDIA_ID_EFR:
      out_args->dtx_mode = session_object->cached_voc_properties_efr.dtx_mode;
      break;

    case VSS_MEDIA_ID_FR:
      out_args->dtx_mode = session_object->cached_voc_properties_fr.dtx_mode;
      break;

    case VSS_MEDIA_ID_HR:
      out_args->dtx_mode = session_object->cached_voc_properties_hr.dtx_mode;
      break;

    case VSS_MEDIA_ID_G711_ALAW:
      out_args->dtx_mode = session_object->cached_voc_properties_g711_alaw.dtx_mode;
      break;

    case VSS_MEDIA_ID_G711_MULAW:
      out_args->dtx_mode = session_object->cached_voc_properties_g711_mulaw.dtx_mode;
      break;

    case VSS_MEDIA_ID_G711_ALAW_V2:
      out_args->dtx_mode = session_object->cached_voc_properties_g711_alaw_v2.dtx_mode;
      break;

    case VSS_MEDIA_ID_G711_MULAW_V2:
      out_args->dtx_mode = session_object->cached_voc_properties_g711_mulaw_v2.dtx_mode;
      break;

    case VSS_MEDIA_ID_G711_LINEAR:
      out_args->dtx_mode = session_object->cached_voc_properties_g711_linear.dtx_mode;
      break;

    case VSS_MEDIA_ID_G729:
      out_args->dtx_mode = session_object->cached_voc_properties_g729.dtx_mode;
      break;

    case VSS_MEDIA_ID_EVS:
      out_args->dtx_mode = session_object->cached_voc_properties_evs.dtx_mode;
      break;

    default:
      out_args->dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
      break;
  }
  
  if ( VSM_STREAM_PROPERTY_DTX_MODE_DISABLE == 
                   session_object->cached_voc_properties_passive_ctrl.dtx_mode )
  {
    out_args->dtx_mode = VSM_STREAM_PROPERTY_DTX_MODE_DISABLE;
  }
  else if ( out_args->dtx_mode == VSM_STREAM_PROPERTY_NOT_SET_UINT32 && 
            session_object->cached_voc_properties_passive_ctrl.dtx_mode == 
            VSM_STREAM_PROPERTY_DTX_MODE_ENABLE )
  {
    out_args->dtx_mode = VSM_STREAM_PROPERTY_DTX_MODE_ENABLE;
  }
  
  return;
}

void vsm_utility_cache_voc_property (
  vsm_session_object_t* session_object, 
  void* payload,
  vsm_voc_property_enum_t property
)
{
  if ( payload == NULL )
  {
    return;
  }

  switch ( property )
  {
    case VSM_VOC_PROPERTY_ENUM_RATE_MODULATION:
      vsm_utility_cache_rate_modulation( session_object, payload );
      break;

    case VSM_VOC_PROPERTY_ENUM_MINMAX_RATE:
      vsm_utility_cache_minmax_rate( session_object, payload );
      break;

    case VSM_VOC_PROPERTY_ENUM_ENC_RATE:
      vsm_utility_cache_enc_rate( session_object, payload );
      break;

    case VSM_VOC_PROPERTY_ENUM_CHANNEL_AWARE_MODE:
      vsm_utility_cache_channel_aware_mode( session_object, payload );
      break;

    case VSM_VOC_PROPERTY_ENUM_ENC_OPERATING_MODE:
      vsm_utility_cache_enc_operating_mode( session_object, payload );
      break;
  }

}

void vsm_utility_free_session_object(
  vsm_session_object_t** session_ptr
)
{
  if ( NULL == session_ptr )
  {
    return;
  }

  if ( ( NULL != *session_ptr )  )
  {
    int session_index = 0;
    bool_t found_session = FALSE;

    for ( session_index = 0 ; session_index < VSM_MAX_SUPPORTED_SESSION_ID ; session_index++ )
    {
      if ( *session_ptr == voice_stream_manager.session_table[ session_index ] )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vsm_utility_free_session_object:"
               " freeing memory of session_object 0x%X ", *session_ptr );
        qurt_elite_memory_free( ( void* )voice_stream_manager.session_table[session_index] );
        voice_stream_manager.session_table[ session_index ] = NULL;
        *session_ptr = NULL;
        found_session = TRUE;
        break;
      }
    }
    if ( FALSE == found_session )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_free_session_object: session 0x%X not found", *session_ptr );
    }
  }
}

bool_t vsm_utility_is_voc_op_mode_event_received( vsm_session_object_t* session_object )
{
  if ( ( VSM_DIRECTION_RX == session_object->direction ) &&
       ( TRUE == session_object->voc_operating_mode_info.is_rx_mode_received )
     )
  {
    return TRUE;
  }
  else if ( ( session_object->direction == VSM_DIRECTION_TX ) &&
            ( session_object->voc_operating_mode_info.is_tx_mode_received == TRUE )
          )
  {
    return TRUE;
  }
  else if ( ( session_object->direction == VSM_DIRECTION_RX_TX ) &&
            ( session_object->voc_operating_mode_info.is_rx_mode_received == TRUE ) &&
            ( session_object->voc_operating_mode_info.is_tx_mode_received == TRUE )
          )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

void vsm_utility_allocate_timing_payload(
  vsm_session_object_t* session_object,
  int32_t** ret_ptr
)
{
  vsm_set_timing_params_v2_t* timing_param_v2 = NULL;
  *ret_ptr = NULL;

  timing_param_v2 = ( vsm_set_timing_params_v2_t* )qurt_elite_memory_malloc(
                                   sizeof( vsm_set_timing_params_v2_t ), QURT_ELITE_HEAP_DEFAULT );
  if ( NULL == timing_param_v2 )
  { 
    return;
  }

  timing_param_v2->vsid = session_object->target_set.voice_timing.vsid;
  timing_param_v2->mode = session_object->target_set.voice_timing.vfr_mode;
  timing_param_v2->vfr_cycle = session_object->target_set.voice_timing.vfr_cycle_duration_us;

  timing_param_v2->enc_offset = session_object->target_set.voice_timing.enc_offset;
  timing_param_v2->dec_req_offset = session_object->target_set.voice_timing.dec_req_offset;
  timing_param_v2->dec_offset = session_object->target_set.voice_timing.dec_offset;
  timing_param_v2->vptx_delivery_offset = session_object->target_set.voice_timing.vptx_delivery_offset;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: VFR_CONFIG: VSID: 0x%08X, VFR mode: %d,  vfr_cycle_us: %d ",
         timing_param_v2->vsid, timing_param_v2->mode, timing_param_v2->vfr_cycle );

  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VSM: VOICE_TIMING_OFFSETS: enc_offset: %d, dec_req_offset: %d, " 
         "dec_offset = %d, vptx_delivery_offset =%d",
         timing_param_v2->enc_offset, timing_param_v2->dec_req_offset,
         timing_param_v2->dec_offset, timing_param_v2->vptx_delivery_offset );

    *ret_ptr = ( int32_t* )timing_param_v2;
    return;
}

ADSPResult vsm_utility_mailbox_timer_get_time (
  uint64_t* ret_time_us
)
{
  int32_t rc = ADSP_EOK;

  if ( NULL!= ret_time_us )
  {
    rc = vocsvc_avtimer_get_time( ( vocsvc_avtimer_timestamp_t* ) ret_time_us );
  }

  return rc;
}

bool_t vsm_utility_media_id_is_nb_sr (
  uint32_t media_id
)
{
  bool_t rc;

  switch ( media_id )
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
  case VSS_MEDIA_ID_G711_ALAW_V2:
  case VSS_MEDIA_ID_G711_MULAW:
  case VSS_MEDIA_ID_G711_MULAW_V2:
  case VSS_MEDIA_ID_G711_LINEAR:
  case VSS_MEDIA_ID_G729:
  case VSS_MEDIA_ID_G722:
    rc = TRUE;
    break;

  default:
    rc = FALSE;
    break;
  }

  /* VSS_MEDIA_ID_NONE is not considered a valid parameter because
   * system cannot run without a media type.
   */

  return rc;
}

int32_t vsm_utility_determine_target_rx_pp_sr_from_feature (
  uint32_t feature_id,
  uint32_t media_id
)
{
  int32_t rx_pp_sampling_rate = 0;

  switch (feature_id)
  {
    case VSS_ICOMMON_CAL_FEATURE_BEAMR:
      rx_pp_sampling_rate = 16000;
      break;

    default:
      {
        /** Need to expand to higher bands in the future **/
        if ( vsm_utility_media_id_is_nb_sr( media_id ) )
          rx_pp_sampling_rate = 8000;
        else
          rx_pp_sampling_rate = 16000;
      }
      break;
  }

  return rx_pp_sampling_rate;
}

bool_t vsm_utility_vocoder_is_variable (
  uint32_t media_id /* Assumes the media_id is valid. */
)
{
  bool_t rc;

  switch ( media_id )
  {
  case VSS_MEDIA_ID_EVS:
    rc = TRUE;
    break;

  default:
    rc = FALSE;
    break;
  }

  return rc;
}

bool_t vsm_utility_media_id_is_valid (
  uint32_t media_id
)
{
  bool_t rc;

  switch ( media_id )
  {
  case VSS_MEDIA_ID_13K:
  case VSS_MEDIA_ID_EVRC:
  case VSS_MEDIA_ID_4GV_NB:
  case VSS_MEDIA_ID_4GV_WB:
  case VSS_MEDIA_ID_4GV_NW:
  case VSS_MEDIA_ID_4GV_NW2K:
  case VSS_MEDIA_ID_AMR_NB:
  case VSS_MEDIA_ID_AMR_WB:
  case VSS_MEDIA_ID_EAMR:
  case VSS_MEDIA_ID_EFR:
  case VSS_MEDIA_ID_FR:
  case VSS_MEDIA_ID_HR:
  case VSS_MEDIA_ID_PCM_8_KHZ:
  case VSS_MEDIA_ID_PCM_16_KHZ:
  case VSS_MEDIA_ID_PCM_32_KHZ:
  case VSS_MEDIA_ID_PCM_48_KHZ:
  case VSS_MEDIA_ID_G711_ALAW:
  case VSS_MEDIA_ID_G711_ALAW_V2:
  case VSS_MEDIA_ID_G711_MULAW:
  case VSS_MEDIA_ID_G711_MULAW_V2:
  case VSS_MEDIA_ID_G711_LINEAR:
  case VSS_MEDIA_ID_G729:
  case VSS_MEDIA_ID_G722:
  case VSS_MEDIA_ID_EVS:
    rc = TRUE;
    break;

  default:
    rc = FALSE;
    break;
  }

  /* VSS_MEDIA_ID_NONE is not considered a valid parameter because
   * system cannot run without a media type.
   */

  return rc;
}

int32_t vsm_utility_determine_closest_supported_pp_sr (
  uint32_t enc_dec_sr,
  uint32_t* ret_pp_sr
)
{
  switch ( enc_dec_sr )
  {
  case 8000:
    *ret_pp_sr = 8000;
    break;

  case 16000:
    *ret_pp_sr = 16000;
    break;

  case 32000:
    *ret_pp_sr = 32000;
    break;
    
  case 41000:
  case 48000:
    *ret_pp_sr = 48000;
    break;

  default:
    {
      return ADSP_EBADPARAM;
    }
  }

  return ADSP_EOK;
}

/* Determine the target_set enc, dec sampling rates for the EVS media
type.  Parameters ret_dec_sampling_rate and ret_enc_sampling_rate should 
be non-NULL*/
int32_t vsm_utility_evs_sanitize_sampling_rates (
  uint32_t requested_rx_sampling_rate,
  uint32_t requested_tx_sampling_rate,
  uint32_t* ret_dec_sampling_rate,
  uint32_t* ret_enc_sampling_rate
)
{
  int32_t rc = ADSP_EOK;

  if ( ( ret_dec_sampling_rate == NULL ) || ( ret_enc_sampling_rate == NULL ) )
  {
    return ADSP_EFAILED;
  }

  switch ( requested_rx_sampling_rate )
  {
    case 8000:
      {
        *ret_dec_sampling_rate = 8000;
        break;
      }
    case 16000:
      {
        *ret_dec_sampling_rate = 16000;
        break;
      }
    case 32000:
      {
        *ret_dec_sampling_rate = 32000;
        break;
      }
    case 0:
    case 48000:
      {
        *ret_dec_sampling_rate = 48000;
        break;
      }
    default:
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_evs_sanitize_sampling_rates: " \
                                                "Invalid Rx Sampling Rate %d",
                                                requested_rx_sampling_rate );
        rc = ADSP_EBADPARAM;
        break;
      }
  }

  switch ( requested_tx_sampling_rate )
  {
    case 8000:
      {
        *ret_enc_sampling_rate = 8000;
        break;
      }
    case 16000:
      {
        *ret_enc_sampling_rate = 16000;
        break;
      }
    case 32000:
      {
        *ret_enc_sampling_rate = 32000;
        break;
      }
    case 0:
    case 48000:
      {
        *ret_enc_sampling_rate = 48000;
        break;
      }
    default:
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_evs_sanitize_sampling_rates: " \
                                                "Invalid Tx Sampling Rate %d",
                                                requested_tx_sampling_rate );
        rc = ADSP_EBADPARAM;
        break;
      }
  }

  return rc;
}

bool_t vsm_utility_var_voc_sr_is_valid (
  uint32_t sr
)
{
  bool_t rc;

  switch ( sr )
  {
  case 8000:
  case 16000:
  case 32000:
  case 41000:
  case 48000:
    rc = TRUE;
    break;

  default:
    rc = FALSE;
    break;
  }

  return rc;
}

/* Determine the target_set enc, dec and pp sampling rates based on the
 * media type and the client requested variable vocoder sampling rates.
 */
int32_t vsm_utility_determine_target_sampling_rates (
  uint32_t media_id,
  uint32_t requested_var_voc_rx_sampling_rate,
  uint32_t requested_var_voc_tx_sampling_rate,
  uint32_t* ret_dec_sr,
  uint32_t* ret_enc_sr,
  uint32_t* ret_rx_pp_sr,
  uint32_t* ret_tx_pp_sr
)
{
  int32_t rc = APR_EOK;

  if ( ( ret_dec_sr == NULL ) || ( ret_enc_sr == NULL ) ||
       ( ret_rx_pp_sr == NULL ) || ( ret_tx_pp_sr == NULL ) )
  {
    return ADSP_EBADPARAM;
  }

  for ( ;; )
  {
    if ( vsm_utility_media_id_is_valid( media_id ) == FALSE ||
         vsm_utility_var_voc_sr_is_valid( requested_var_voc_rx_sampling_rate ) == FALSE ||
         vsm_utility_var_voc_sr_is_valid( requested_var_voc_tx_sampling_rate ) == FALSE )
    {
      rc = ADSP_EBADPARAM;
      break;
    }

    /* First determine the enc/dec sampling rates. */
    switch ( media_id )
    {
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
        *ret_dec_sr = 8000;
        *ret_enc_sr = 8000;
      }
      break;

    case VSS_MEDIA_ID_4GV_WB:
    case VSS_MEDIA_ID_4GV_NW:
    case VSS_MEDIA_ID_4GV_NW2K:
    case VSS_MEDIA_ID_AMR_WB:
    case VSS_MEDIA_ID_EAMR:
    case VSS_MEDIA_ID_PCM_16_KHZ:
      {
        *ret_dec_sr = 16000;
        *ret_enc_sr = 16000;
      }
      break;

    case VSS_MEDIA_ID_PCM_32_KHZ:
      {
        *ret_dec_sr = 32000;
        *ret_enc_sr = 32000;
      }
      break;
      
    case VSS_MEDIA_ID_PCM_48_KHZ:
      {
        *ret_dec_sr = 48000;
        *ret_enc_sr = 48000;
      }
      break;

    case VSS_MEDIA_ID_EVS:
      {
        rc = vsm_utility_evs_sanitize_sampling_rates( requested_var_voc_rx_sampling_rate, 
                                              requested_var_voc_tx_sampling_rate,
                                              ret_dec_sr, ret_enc_sr );
      }
      break;

    default:
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_determine_target_sampling_rates: " \
               "Invalid Media ID %d", media_id );
        rc = APR_EBADPARAM;
      }
      break;
    }

    if ( rc )
      break;

    /* Now determine the supported closest mataching higher sample rates
     * for the pp based on the enc/dec sample rates.
     */
    rc = vsm_utility_determine_closest_supported_pp_sr( *ret_dec_sr, ret_rx_pp_sr );
    if ( rc )
    {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_determine_target_sampling_rates: " \
               "Error Unexpected %d", rc );
      break;
    }

    rc = vsm_utility_determine_closest_supported_pp_sr( *ret_enc_sr, ret_tx_pp_sr );
    if ( rc )
    {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_determine_target_sampling_rates: " \
               "Error Unexpected %d", rc );
      break;
    }

    break;
  }

  return rc;
}

void vsm_utility_destroy_service(
  vsm_session_object_t* session_object,
  elite_svc_handle_t* svc_handle_ptr
)
{
  qurt_elite_thread_t thread_id;
  int32_t result;
  
  /* Destroy enc first so tx loopback stops before decoder is destroyed. */
  if( ( NULL != session_object ) && ( NULL != svc_handle_ptr ) )
  {
    /* Cache the thread id as context will be lost once destroy message is sent. */
    thread_id = svc_handle_ptr->threadId;

    elite_msg_any_t destroy_msg;
    uint32_t size = sizeof( elite_msg_cmd_destroy_svc_t );
    result = elite_msg_create_msg( &destroy_msg,
               &size,
               ELITE_CMD_DESTROY_SERVICE,
               NULL,  //session_object->cmd_control.respQ,
               NULL,
               ADSP_EOK );
    if( ADSP_FAILED( result ) )
    {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_destroy_service:"
              " FAILED to create EliteMsg : svc_handle(%x)", svc_handle_ptr );
       return;
    }

    result = qurt_elite_queue_push_back( svc_handle_ptr->cmdQ,( uint64_t* )&destroy_msg );
    if ( ADSP_FAILED( result ) )
    {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_destroy_service:"
              " FAILED to push destroy msg to cmdQ, svc_handle(%x)", svc_handle_ptr);
       elite_msg_return_payload_buffer( &destroy_msg );
       return;
    }

    /* Wait for the threads to finish. */
    qurt_elite_thread_join(thread_id, (int*)&result);

    elite_msg_return_payload_buffer( &destroy_msg );
  }
}

void vsm_utility_initiate_fullcontrol_client_info(
  vsm_session_object_t* session_obj,
  vss_istream_cmd_create_full_control_session_t* payload
)
{

  if ( ( NULL == session_obj ) || ( NULL == payload ) )
  {
    return;
  }

  //payload size should have got validated already by now.So no error handling required here.
  switch ( payload->direction )
  {
    case VSM_DIRECTION_RX:
      session_obj->target_set.media_id = payload->dec_media_type;
      session_obj->target_set.system_config.media_id = payload->dec_media_type;
    break;

    case VSM_DIRECTION_TX:
    case VSM_DIRECTION_RX_TX:
    {
      session_obj->target_set.media_id = payload->enc_media_type;
      session_obj->target_set.system_config.media_id = payload->enc_media_type;
    }
    break;

    default:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_initialize_session_variables:"
             " ERROR, unexpected direction...", payload->direction );
    }
    break;
  }
  session_obj->direction = payload->direction;
}

void vsm_utility_initialize_session_variables( vsm_session_object_t* session_obj )
{
  QURT_ELITE_ASSERT( session_obj );
  int32_t rc = ADSP_EOK;
  vss_istream_cmd_create_full_control_session_t* payload = NULL;
  uint32_t payload_size;
  char_t* req_session_name;
  uint32_t req_session_name_size;
  uint32_t dst_len;
  uint8_t index = 0;

  elite_apr_packet_t *apr_pkt = ( elite_apr_packet_t * )( session_obj->cmd_control.cmd_msg.pPayload );

  session_obj->apr_info.client_addr = 0;
  session_obj->apr_info.client_port = 0;

  for( index = 0; index< VSM_NUM_MAX_PASSIVE_CLIENTS; index++ )
  {
    session_obj->passive_clients[ index ].client_addr = 0;
    session_obj->passive_clients[ index ].client_port = 0;
  }

  if ( VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION == apr_pkt->opcode )
  {
    //payload size should have got validated already by now.So no error handling required here.

    payload = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_create_full_control_session_t, apr_pkt );
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );

    req_session_name = ( ( ( char_t* ) payload ) +
                         sizeof( vss_istream_cmd_create_full_control_session_t ) );
    req_session_name_size = ( payload_size -
                            sizeof( vss_istream_cmd_create_full_control_session_t ) );

    session_obj->apr_info.client_addr = apr_pkt->src_addr;
    session_obj->apr_info.client_port = apr_pkt->src_port;
    vsm_utility_initiate_fullcontrol_client_info( session_obj, payload );
  }
  else
  {
    /* If a passive control session is being created first, use deault values for direction and mediaID.
    *  This is required to create dynamic services.
    */
    VSM_utility_update_passive_client_info( apr_pkt, session_obj );

    session_obj->direction = VSM_DIRECTION_RX_TX;
    session_obj->target_set.media_id = VSM_MEDIA_TYPE_NONE;
    req_session_name = APRV2_PKT_GET_PAYLOAD( char_t, apr_pkt );;
    req_session_name_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( apr_pkt->header );
  }

  if ( req_session_name_size > 0 )
  { /* Save the session name. */
    dst_len = MMSTD_MIN( sizeof( session_obj->session_name ), req_session_name_size );
    rc = mmstd_strlcpy( session_obj->session_name, req_session_name, dst_len );

    if ( rc >= dst_len )
    {
      /* Truncation happened. */
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vsm_utility_initialize_session_variables: ERROR, input truncation" );
    }
    MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                         "vsm_utility_initialize_session_variables(): Session portID = 0x%08X, Session name = %s",
                         session_obj->apr_info.self_port, req_session_name );
  }

  session_obj->cmd_control.is_resp_pending = FALSE;
  session_obj->cmd_control.current_action = 0xFFFFFFFF;
  session_obj->cmd_control.resp_result = 0xFFFFFFFF;
  session_obj->cmd_control.is_destroy_cmd_pending = FALSE;

//  session_obj->state_control.goal = VSM_GOAL_ENUM_NONE;
//  session_obj->state_control.action = VSM_ACTION_ENUM_NONE;
  session_obj->state_control.state = VSM_STATE_ENUM_UNINITIALIZED;
  session_obj->state_control.status = 0xFFFFFFFF;

  session_obj->venc = NULL;
  session_obj->vdec = NULL;

  session_obj->mixer_in_port_ptr = NULL;
  session_obj->mixer_out_port_ptr = NULL;
  session_obj->input_mapping_mask = 0;
  session_obj->output_mapping_mask = 0;

  session_obj->tx_afe_drift_ptr = NULL;
  session_obj->tx_afe_drift_ptr = NULL;

  session_obj->rx_afe_handle_ptr = NULL;
  session_obj->tx_afe_handle_ptr = NULL;
  session_obj->aud_ref_port_rx = 0;
  session_obj->aud_ref_port_tx = 0;

  for( int i = 0; i < VSM_MAX_ATTACHED_VOCPROC_CNT; ++i )
  {
    session_obj->target_set.attach_table.rx_device_handle[ i ] = VSM_ATTACHED_VOCPROC_HANDLE_INVALID;
    session_obj->target_set.attach_table.tx_device_handle[ i ] = VSM_ATTACHED_VOCPROC_HANDLE_INVALID;
    session_obj->active_set.attach_table.rx_device_handle[ i ] = VSM_ATTACHED_VOCPROC_HANDLE_INVALID;
    session_obj->active_set.attach_table.tx_device_handle[ i ] = VSM_ATTACHED_VOCPROC_HANDLE_INVALID;

    session_obj->active_set.attach_table.vpm_isready[i] = FALSE;
    session_obj->target_set.attach_table.vpm_isready[i] = FALSE;

    session_obj->active_set.attach_table.vpm_handles[i] = VSM_ATTACHED_VOCPROC_HANDLE_INVALID;
    session_obj->target_set.attach_table.vpm_handles[i] = VSM_ATTACHED_VOCPROC_HANDLE_INVALID;

    session_obj->active_set.attach_table.tx_port_id[i] = APR_NULL_V;
    session_obj->target_set.attach_table.tx_port_id[i] = APR_NULL_V;
    
    session_obj->active_set.attach_table.rx_port_id[i] = APR_NULL_V;
    session_obj->target_set.attach_table.rx_port_id[i] = APR_NULL_V;
    
    session_obj->active_set.attach_table.ec_ref_port_id[i] = APR_NULL_V;
    session_obj->target_set.attach_table.ec_ref_port_id[i] = APR_NULL_V;
  }

  session_obj->target_set.attach_table.vpm_ready_cnt = 0;
  session_obj->active_set.attach_table.vpm_ready_cnt = 0;

  session_obj->active_set.attach_table.rx_device_cnt = APR_NULL_V;
  session_obj->active_set.attach_table.tx_device_cnt = APR_NULL_V;
  session_obj->target_set.attach_table.rx_device_cnt = APR_NULL_V;
  session_obj->target_set.attach_table.tx_device_cnt = APR_NULL_V;

#if 0
  session_obj->master_client_addr = APRV2_PKT_INIT_ADDR_V;
  session_obj->master_client_port = APR_NULL_V;
  session_obj->master_cvs_port = APR_NULL_V;
  session_obj->vsm_stream_addr = APRV2_PKT_INIT_ADDR_V;
  session_obj->vsm_stream_handle = APR_NULL_V;
#endif

  session_obj->attached_mvm_handle = APR_NULL_V;

  session_obj->is_stream_config_changed = FALSE;

  session_obj->voc_operating_mode_info.is_rx_mode_received = FALSE;
  session_obj->voc_operating_mode_info.is_tx_mode_received = FALSE;
  session_obj->voc_operating_mode_info.rx_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
  session_obj->voc_operating_mode_info.tx_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;

  session_obj->is_var_voc_sr_requested = FALSE;
  session_obj->requested_var_voc_rx_sampling_rate = VSM_DEFAULT_VAR_VOC_DEC_SAMPLING_RATE;
  session_obj->requested_var_voc_tx_sampling_rate = VSM_DEFAULT_VAR_VOC_ENC_SAMPLING_RATE;

  session_obj->active_set.media_id = VSS_MEDIA_ID_NONE;
  session_obj->active_set.system_config.network_id = VSS_ICOMMON_CAL_NETWORK_ID_NONE;
  session_obj->active_set.system_config.media_id = VSS_MEDIA_ID_NONE;
  session_obj->active_set.system_config.rx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
  session_obj->active_set.system_config.tx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
  session_obj->active_set.system_config.dec_sr = VSM_DEFAULT_DEC_SR;
  session_obj->active_set.system_config.enc_sr = VSM_DEFAULT_ENC_SR;
  session_obj->active_set.system_config.rx_pp_sr = VSM_DEFAULT_RX_PP_SR;
  session_obj->active_set.system_config.tx_pp_sr = VSM_DEFAULT_TX_PP_SR;
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


  session_obj->active_set.record.enable_flag = VSM_DISABLED;
  session_obj->active_set.record.mode = ( ( uint32_t ) APR_UNDEFINED_ID_V );
  session_obj->active_set.record.port_id = VSS_IRECORD_PORT_ID_DEFAULT;
  session_obj->active_set.record.tx_tap_point = VSS_IRECORD_TAP_POINT_NONE;
  session_obj->active_set.record.rx_tap_point = VSS_IRECORD_TAP_POINT_NONE;
  session_obj->active_set.record.state = VSM_RECORD_STATE_IDLE;

  session_obj->active_set.playback.enable_flag = VSM_DISABLED;
  session_obj->active_set.playback.port_id = VSS_IPLAYBACK_PORT_ID_DEFAULT;
  session_obj->active_set.playback.state = VSM_PLAYBACK_STATE_IDLE;

  session_obj->active_set.attach_detach_vocproc_handle = APR_NULL_V;
  session_obj->active_set.attach_detach_vocproc_direction = APR_NULL_V;
  session_obj->active_set.attach_detach_stream_handle = APR_NULL_V;

  session_obj->target_set.system_config.network_id = VSS_ICOMMON_CAL_NETWORK_ID_NONE;
  session_obj->target_set.system_config.media_id = VSS_MEDIA_ID_NONE;
  session_obj->target_set.system_config.rx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
  session_obj->target_set.system_config.tx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
  session_obj->target_set.system_config.dec_sr = VSM_DEFAULT_DEC_SR;
  session_obj->target_set.system_config.enc_sr = VSM_DEFAULT_ENC_SR;
  session_obj->target_set.system_config.rx_pp_sr = VSM_DEFAULT_RX_PP_SR;
  session_obj->target_set.system_config.tx_pp_sr = VSM_DEFAULT_TX_PP_SR;
  session_obj->target_set.system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;

  session_obj->target_set.system_config.vsid = 0;
  session_obj->target_set.system_config.vfr_mode = VSS_ICOMMON_VFR_MODE_SOFT;

  session_obj->target_set.system_config.call_num = 0;
  session_obj->target_set.voice_timing.enc_offset = 0;
  session_obj->target_set.voice_timing.dec_req_offset = 0;
  session_obj->target_set.voice_timing.dec_offset = 0;

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

  session_obj->target_set.record.enable_flag = VSM_DISABLED;
  session_obj->target_set.record.mode = ( ( uint32_t ) APR_UNDEFINED_ID_V );
  session_obj->target_set.record.port_id = VSS_IRECORD_PORT_ID_DEFAULT;
  session_obj->target_set.record.tx_tap_point = VSS_IRECORD_TAP_POINT_NONE;
  session_obj->target_set.record.rx_tap_point = VSS_IRECORD_TAP_POINT_NONE;
  session_obj->target_set.record.state = VSM_RECORD_STATE_IDLE;


  session_obj->target_set.playback.enable_flag = VSM_DISABLED;
  session_obj->target_set.playback.port_id = VSS_IPLAYBACK_PORT_ID_DEFAULT;
  session_obj->target_set.attach_detach_vocproc_handle = APR_NULL_V;
  session_obj->target_set.attach_detach_stream_handle = APR_NULL_V;

  /* Initiaize stream mute variables. */
  session_obj->active_set.mute.tx_mute_flag = VSS_IVOLUME_MUTE_OFF;
  session_obj->active_set.mute.rx_mute_flag = VSS_IVOLUME_MUTE_OFF;
  session_obj->active_set.mute.tx_ramp_duration = 0;
  session_obj->active_set.mute.rx_ramp_duration = 0;

  session_obj->target_set.mute.tx_mute_flag = VSS_IVOLUME_MUTE_OFF;
  session_obj->target_set.mute.rx_mute_flag = VSS_IVOLUME_MUTE_OFF;
  session_obj->target_set.mute.tx_ramp_duration = 0;
  session_obj->target_set.mute.rx_ramp_duration = 0;

  /* Sampling rate that is configured on Tx mixer port. */
  session_obj->active_tx_sampling_rate = 0;

  /* Initiaize ITTYOOB status variable. */
  session_obj->tty_info.is_ittyoob_registered = FALSE;
  session_obj->tty_info.ittyoob_client_addr = APRV2_PKT_INIT_ADDR_V;
  session_obj->tty_info.ittyoob_client_port = APR_NULL_V;
  session_obj->tty_info.tty_mode = VSS_ITTY_MODE_DISABLED;

  /* Initiaize IAVSYNC client info/delay values. */
  session_obj->avsync_client_rx_info.is_enabled = FALSE;
  session_obj->avsync_client_rx_info.client_addr = APRV2_PKT_INIT_ADDR_V;
  session_obj->avsync_client_rx_info.client_port = APR_NULL_V;

  session_obj->avsync_client_tx_info.is_enabled = FALSE;
  session_obj->avsync_client_tx_info.client_addr = APRV2_PKT_INIT_ADDR_V;
  session_obj->avsync_client_tx_info.client_port = APR_NULL_V;

  session_obj->avsync_delay_info.stream_rx_algorithmic_delay = 0;
  session_obj->avsync_delay_info.stream_tx_algorithmic_delay = 0;
  session_obj->avsync_delay_info.total_rx_delay = 0;
  session_obj->avsync_delay_info.total_tx_delay = 0;

  session_obj->set_rx_dtmf_detect.client_addr = APRV2_PKT_INIT_ADDR_V;
  session_obj->set_rx_dtmf_detect.client_port = APR_NULL_V;
  session_obj->set_rx_dtmf_detect.vsm_port = APR_NULL_V;
  session_obj->set_rx_dtmf_detect.enable_flag = VSM_DISABLED;

  session_obj->set_dtmf_gen.client_addr = APRV2_PKT_INIT_ADDR_V;
  session_obj->set_dtmf_gen.client_port = APR_NULL_V;
  session_obj->set_dtmf_gen.vsm_port = APR_NULL_V;

  session_obj->vpcm_info.is_enabled = FALSE;
  session_obj->vpcm_info.client_addr = APRV2_PKT_INIT_ADDR_V;
  session_obj->vpcm_info.client_handle = APR_NULL_V;
  session_obj->vpcm_info.cvs_handle = APR_NULL_V;
  session_obj->vpcm_info.mem_handle = 0;

  ( void ) mmstd_memset( &session_obj->cached_ui_properties, 0,
                             sizeof( vsm_ui_prop_cache_t ) );
  session_obj->cached_ui_properties.version = 0xFFFFFFFF;
  ( void ) mmstd_memset( &session_obj->stream_param_cache, 0,
                             sizeof( vsm_stream_param_cache_t ) );

  session_obj->packet_exchange_info.mode = VSS_IPKTEXG_MODE_IN_BAND;

  session_obj->packet_exchange_info.oob_info.is_configured = FALSE;
  session_obj->packet_exchange_info.oob_info.config.enc_buf_addr_lsw = 0;
  session_obj->packet_exchange_info.oob_info.config.enc_buf_addr_msw = 0;
  session_obj->packet_exchange_info.oob_info.config.enc_buf_size = 0;
  session_obj->packet_exchange_info.oob_info.config.dec_buf_addr_lsw = 0;
  session_obj->packet_exchange_info.oob_info.config.dec_buf_addr_msw = 0;
  session_obj->packet_exchange_info.oob_info.config.dec_buf_size = 0;
  session_obj->packet_exchange_info.oob_info.config.mem_handle = 0;

  session_obj->packet_exchange_info.mailbox_info.is_configured = FALSE;
  session_obj->packet_exchange_info.mailbox_info.config.cvs_mem_handle = APR_NULL_V;
  session_obj->packet_exchange_info.mailbox_info.config.tx_circ_buf = NULL;
  session_obj->packet_exchange_info.mailbox_info.config.tx_circ_buf_mem_size = 0;
  session_obj->packet_exchange_info.mailbox_info.config.rx_circ_buf = NULL;
  session_obj->packet_exchange_info.mailbox_info.config.rx_circ_buf_mem_size = 0;
  session_obj->packet_exchange_info.mailbox_info.config.max_enc_pkt_size = 0;
  session_obj->packet_exchange_info.mailbox_info.config.max_dec_pkt_size = 0;

  ( void ) mmstd_memset( &session_obj->packet_exchange_info.mailbox_info.stats, 0,
                         sizeof( vsm_mailbox_packet_exchange_stats_t ) );

  { /* BACKWARD COMPATIBILITY */
    session_obj->common_cal.is_registered = FALSE;
    session_obj->common_cal.is_evaluated = FALSE;
    session_obj->common_cal.is_calibrate_needed = FALSE;
    session_obj->common_cal.table_handle = NULL;
    session_obj->common_cal.vsm_mem_handle = 0;
    ( void ) mmstd_memset( &session_obj->common_cal.matching_entries, 0,
                           sizeof( session_obj->common_cal.matching_entries ) );
    session_obj->common_cal.num_matching_entries = 0;
    session_obj->common_cal.set_param_rsp_cnt = 0;
    session_obj->common_cal.set_param_failed_rsp_cnt = 0;
  }

  session_obj->static_cal.is_registered = FALSE;
  session_obj->static_cal.is_calibrate_needed = FALSE;
  session_obj->static_cal.table_handle = APR_NULL_V;
  session_obj->static_cal.query_handle = CVD_CAL_INVALID_HANDLE_V;
  ( void ) mmstd_memset( &session_obj->static_cal.matching_entries, 0,
                         sizeof( session_obj->static_cal.matching_entries ) );
  ( void ) mmstd_memset( session_obj->static_cal.query_key_columns, 0,
                         sizeof( session_obj->static_cal.query_key_columns ) );

  session_obj->eamr_mode_change_notification_info.is_enabled = FALSE;
  session_obj->eamr_mode_change_notification_info.client_addr = APRV2_PKT_INIT_ADDR_V;
  session_obj->eamr_mode_change_notification_info.client_port = APR_NULL_V;
  session_obj->eamr_mode_change_notification_info.mode = VSS_ISTREAM_EAMR_MODE_NARROWBAND;
    /**<
     * The default mode of eAMR is narrowband. Refer to the comments under
     * eamr_mode_change_notification_info in cvs_session_object_t for
     * details.
     */

  session_obj->evs_bandwidth_change_notification_info.is_enabled = FALSE;
  session_obj->evs_bandwidth_change_notification_info.client_addr = APRV2_PKT_INIT_ADDR_V;
  session_obj->evs_bandwidth_change_notification_info.client_port = APR_NULL_V;
  session_obj->evs_bandwidth_change_notification_info.last_received_rx_bandwidth = VSM_STREAM_VOC_BANDWIDTH_EVT_NOT_SET;
  session_obj->evs_bandwidth_change_notification_info.last_sent_rx_bandwidth = VSM_STREAM_VOC_BANDWIDTH_EVT_NOT_SET;    
    /**<
     * The default RX vocoder bandwidth of EVS is not set until the first 
     * VSM_EVT_VOC_OPERATING_MODE_UPDATE when EVS is used. 
     */       

  session_obj->kpps_info.enc = 0;
  session_obj->kpps_info.dec = 0;
  session_obj->kpps_info.dec_pp = 0;
  session_obj->is_kpps_changed = FALSE;
  
  session_obj->is_clk_overridden = FALSE;

  session_obj->packet_logging_info.voice_call_num = 1;
  session_obj->packet_logging_info.rx_packet_seq_num = 0;
  session_obj->packet_logging_info.tx_packet_seq_num = 0;

  memset( &session_obj->tty_state, 0, sizeof( voice_strm_tty_state_t) );

  /* Setting the cached vococoder properties to default. */
  vsm_utility_reset_cached_vocoder_properties( session_obj );

  return;
}

void vsm_utility_reset_cached_vocoder_properties ( 
  vsm_session_object_t* session_obj
)
{
  session_obj->cached_voc_properties_qcelp13k.rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_qcelp13k.min_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_qcelp13k.max_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_qcelp13k.reduced_rate_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_evrc.rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_evrc.min_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_evrc.max_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_evrc.reduced_rate_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_4gvnb.rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_4gvnb.min_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_4gvnb.max_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_4gvwb.rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_4gvwb.min_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_4gvwb.max_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_4gvnw.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_4gvnw.rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_4gvnw.min_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_4gvnw.max_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_4gvnw2k.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_4gvnw2k.rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_4gvnw2k.min_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_4gvnw2k.max_rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_amr.rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_amr.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_amrwb.rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_amrwb.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_eamr.rate = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
  session_obj->cached_voc_properties_eamr.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_efr.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;
   session_obj->cached_voc_properties_fr.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_hr.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_g711_alaw.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_g711_mulaw.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_g711_alaw_v2.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_g711_mulaw_v2.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_g711_linear.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_g729.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  session_obj->cached_voc_properties_evs.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT8;
  session_obj->cached_voc_properties_evs.mode = VSM_STREAM_PROPERTY_NOT_SET_UINT8;
  session_obj->cached_voc_properties_evs.bandwidth = VSM_STREAM_PROPERTY_NOT_SET_UINT8;
  session_obj->cached_voc_properties_evs.channel_aware_enabled = VSM_STREAM_PROPERTY_NOT_APPLICABLE_UINT8;
  session_obj->cached_voc_properties_evs.fec_offset = VSM_STREAM_PROPERTY_NOT_APPLICABLE_UINT8;
  session_obj->cached_voc_properties_evs.fer_rate = VSM_STREAM_PROPERTY_NOT_APPLICABLE_UINT8;

  session_obj->cached_voc_properties_passive_ctrl.dtx_mode = VSM_STREAM_PROPERTY_NOT_SET_UINT32;

  return;
}


bool_t vsm_utility_is_hdvoice_feature_supported(
  uint32_t feature_id
)
{
  bool_t is_feat_supported = FALSE;

  switch ( feature_id )
  {
    case VSS_ICOMMON_CAL_FEATURE_BEAMR:
    {
      is_feat_supported = TRUE;
      break;
    }

    default:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VSM_utility_is_hdvoice_feature_supported():"
             " Un-supported feature ID (0x%08X)", feature_id );

      is_feat_supported = FALSE;
      break;
    }
  }

  return is_feat_supported;
}

bool_t vsm_is_calibration_required ( 
  vsm_session_object_t* session_obj
)
{
  vsm_session_settings_t *active_set = &(session_obj->active_set);
  vsm_session_settings_t *target_set = &(session_obj->target_set);

  if ( ( active_set->system_config.network_id !=  target_set->system_config.network_id ) ||
       ( active_set->system_config.media_id != target_set->system_config.media_id ) ||
       ( active_set->system_config.dec_sr != target_set->system_config.dec_sr ) ||
       ( active_set->system_config.enc_sr != target_set->system_config.enc_sr ) ||
       ( active_set->system_config.rx_voc_op_mode != target_set->system_config.rx_voc_op_mode) ||
       ( active_set->system_config.tx_voc_op_mode != target_set->system_config.tx_voc_op_mode) ||
       ( active_set->system_config.feature != target_set->system_config.feature ) )
  {
    return TRUE;
  }

  return FALSE;
}

uint32_t vsm_utility_validate_and_update_ui_prop_version(
  uint32_t* cached_version,
  uint32_t  new_version
)
{
  uint32_t rc = APR_EOK;

  switch ( *cached_version )
  {
    case CVD_CAL_PARAM_MINOR_VERSION_0:
    case CVD_CAL_PARAM_MINOR_VERSION_1:
    {
       if ( new_version != *cached_version )
       {
          rc = APR_EBADPARAM;
       }
      break;
    }
    default:
    {
      *cached_version = new_version;
      break;
    }
  }

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "vsm_utility_validate_ui_prop_version():rc %d cached_version %d new_version %d",
         rc, *cached_version, new_version );

  return rc;
}

ADSPResult vsm_utility_log_vocoder_packet (
  vsm_session_object_t* session_object,
  elite_apr_packet_t* packet
)
{
  ADSPResult rc = ADSP_EOK;
  elite_apr_packet_t *apr_log_pkt = NULL;
  qurt_elite_queue_t *svc_cmdQ = NULL;
  elite_msg_any_t msg;

  /* Queueing Vocoder Packet Logging command for 0x1804. */
  rc = elite_apr_if_alloc_event( vsm_apr_handle,
         session_object->apr_info.self_addr, session_object->apr_info.self_port,
         session_object->apr_info.self_addr, session_object->apr_info.self_port,
         0, packet->opcode,
         APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header ), &apr_log_pkt );
  if ( NULL != apr_log_pkt )
  {
    memscpy( APRV2_PKT_GET_PAYLOAD( void, apr_log_pkt ), APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header ),
             APRV2_PKT_GET_PAYLOAD( void, packet ), APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header ) );
    msg.unOpCode = ELITE_APR_PACKET;
    msg.pPayload = ( void* )apr_log_pkt;

    if ( ( ADSP_FAILED( rc = VSM_apr_router_get_svc_cmdQ( &svc_cmdQ ) ) ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VSM: fail to get Svc command queue to push log packet" );
      elite_apr_if_free( vsm_apr_handle, apr_log_pkt );
    }
    else if ( ADSP_FAILED( rc = qurt_elite_queue_push_back( svc_cmdQ, ( uint64_t* )( &msg ) ) ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: fail to push log packet to Svc CmdQ 0x%p, port=0x%lx", svc_cmdQ );
      elite_apr_if_free( vsm_apr_handle, apr_log_pkt );
    }
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VSM: apr alloc failed.for logging rc = %d", rc );
  }

  return ADSP_EOK;
}

ADSPResult vsm_utility_log_dec_packet(
  vsm_session_object_t* session_object,
  elite_apr_packet_t* packet
)
{
  vss_ipktexg_evt_in_band_send_dec_buffer_t* dec_buf = NULL;
  uint32_t voc_packet_size;

  dec_buf = APRV2_PKT_GET_PAYLOAD( vss_ipktexg_evt_in_band_send_dec_buffer_t, packet );
  voc_packet_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header ) - sizeof( vss_ipktexg_evt_in_band_send_dec_buffer_t );
  ( void ) cvd_log_commit_data( LOG_ADSP_CVD_STREAM_RX,
             ++session_object->packet_logging_info.rx_packet_seq_num, voc_packet_size,
             ( session_object->attached_mvm_handle << 16 ) | session_object->apr_info.client_port,
             session_object->packet_logging_info.voice_call_num, 0, CVD_LOG_DEC_INPUT_TAP_POINT,
             dec_buf->media_id, ( ( uint8_t* )dec_buf ) + sizeof( vss_ipktexg_evt_in_band_send_dec_buffer_t ) );

  return ADSP_EOK;
}

ADSPResult vsm_utility_log_enc_packet(
  vsm_session_object_t* session_object,
  elite_apr_packet_t* packet
)
{
  vsm_send_enc_packet_v2_t* enc_buf = NULL;
  uint32_t voc_packet_size;

  enc_buf = APRV2_PKT_GET_PAYLOAD( vsm_send_enc_packet_v2_t, packet );
  voc_packet_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header ) - sizeof( vsm_send_enc_packet_v2_t );
  ( void ) cvd_log_commit_data( LOG_ADSP_CVD_STREAM_TX,
             ++session_object->packet_logging_info.tx_packet_seq_num, voc_packet_size,
             ( session_object->attached_mvm_handle << 16 ) | session_object->apr_info.client_port,
             session_object->packet_logging_info.voice_call_num, enc_buf->timestamp_us,
             CVD_LOG_ENC_OUTPUT_TAP_POINT, enc_buf->media_type,
             ( ( uint8_t* )enc_buf ) + sizeof( vsm_send_enc_packet_v2_t ) );

  return ADSP_EOK;
}


/* TBD: Use Macros for vocoder packet sizes. */
ADSPResult vsm_utility_get_max_mailbox_pkt_size (
  uint32_t media_id,
  uint32_t* ret_max_enc_pkt_size,
  uint32_t* ret_max_dec_pkt_size
)
{
  uint32_t max_media_data_size;

  switch ( media_id )
  {
    case VSS_MEDIA_ID_13K:
      max_media_data_size = 35;
      break;

    case VSS_MEDIA_ID_EVRC:
      max_media_data_size = 23;
      break;

    case VSS_MEDIA_ID_4GV_NB:
      max_media_data_size = 23;
      break;
  
    case VSS_MEDIA_ID_4GV_WB:
      max_media_data_size = 23;
      break;
  
    case VSS_MEDIA_ID_4GV_NW:
      max_media_data_size = 23;
      break;
  
    case VSS_MEDIA_ID_4GV_NW2K:
      max_media_data_size = 23;
      break;
  
    case VSS_MEDIA_ID_AMR_NB:
      max_media_data_size = 32;
      break;
  
    case VSS_MEDIA_ID_AMR_WB:
      max_media_data_size = 62;
      break;
  
    case VSS_MEDIA_ID_EAMR:
      max_media_data_size = 32;
      break;
  
    case VSS_MEDIA_ID_EFR:
      max_media_data_size = 32;
      break;
  
    case VSS_MEDIA_ID_FR:
      max_media_data_size = 34;
      break;
  
    case VSS_MEDIA_ID_HR:
      max_media_data_size = 15;
      break;
  
    case VSS_MEDIA_ID_PCM_8_KHZ:
      max_media_data_size = 320;
      break;
  
    case VSS_MEDIA_ID_PCM_16_KHZ:
      max_media_data_size = 640;
      break;
  
    case VSS_MEDIA_ID_PCM_32_KHZ:
      max_media_data_size = 1280;
      break;    
  
    case VSS_MEDIA_ID_PCM_44_1_KHZ:
      max_media_data_size = 1764;
      break;
  
    case VSS_MEDIA_ID_PCM_48_KHZ:
      max_media_data_size = 1920;
      break;
  
    case VSS_MEDIA_ID_G711_ALAW:
      max_media_data_size = 161;
      break;
  
    case VSS_MEDIA_ID_G711_MULAW:
      max_media_data_size = 161;
      break;
  
    case VSS_MEDIA_ID_G711_ALAW_V2:
      max_media_data_size = 322;
      break;
  
    case VSS_MEDIA_ID_G711_MULAW_V2:
      max_media_data_size = 322;
      break;
  
    case VSS_MEDIA_ID_G729:
      max_media_data_size = 22;
      break;

    case VSS_MEDIA_ID_G722:
      max_media_data_size = 161;
      break;

    case VSS_MEDIA_ID_EVS:
     max_media_data_size = 320;
     break;
  
    default:
      return APR_EBADPARAM;
  }

  /* The maximum vocoder packet size includes both the packet header
   * (vss_ipktexg_mailbox_enc_packet_t for encoder and
   * vss_ipktexg_mailbox_dec_packet_t for decoder) and media data. The size
   * is rounded up to multiple of CVS_CACHE_LINE_SIZE to meet the cache line
   * size alignment. */
  if ( ret_max_enc_pkt_size != NULL )
  {
    *ret_max_enc_pkt_size =
      ( ( max_media_data_size + sizeof( vss_ipktexg_mailbox_enc_packet_t ) +
          VSM_CACHE_LINE_SIZE - 1 ) & ( ~ ( VSM_CACHE_LINE_SIZE - 1 ) ) );
  }

  if ( ret_max_dec_pkt_size != NULL )
  {
    *ret_max_dec_pkt_size =
      ( ( max_media_data_size + sizeof( vss_ipktexg_mailbox_dec_packet_t ) +
          VSM_CACHE_LINE_SIZE - 1 ) & ( ~ ( VSM_CACHE_LINE_SIZE - 1 ) ) );
  }

  return APR_EOK;
}

bool vsm_utility_is_mailbox_voc_req_pending (
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* circ_buf,
  uint32_t circ_buf_mem_size,
  uint32_t voc_req_unit_size
)
{
  uint32_t rc  =ADSP_EOK;
  uint32_t circ_buf_data_size;

  for ( ;; )
  {
    if ( NULL == circ_buf )
    {
      VSM_PANIC_ON_ERROR( ADSP_EUNEXPECTED );
      break;
    }

    circ_buf_data_size = 
      ( circ_buf_mem_size - sizeof( vss_ipktexg_mailbox_voc_req_circ_buffer_t ) );

    if ( circ_buf->read_offset == circ_buf->write_offset )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,"VSM: Circular buffer empty" );
      break;
    }

    if ( ( circ_buf->write_offset % voc_req_unit_size ) != 0 )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Write offset %d is not a multiple of request "
             "size %d.", circ_buf->write_offset, voc_req_unit_size );
      break;
    }

    if ( circ_buf->write_offset >= circ_buf_data_size )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Write offset %d is at or beyond the end of the "
             "circular buffer. Total circular buffer size for holding request is %d.", 
             circ_buf->write_offset, circ_buf_data_size );
      break;
    }
  
    if ( circ_buf->read_offset >= circ_buf_data_size )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Read offset %d is corrupted by client, which is "
             "at or beyond the end of the circular buffer. Total circular buffer "
             "size for holding request is %d.", circ_buf->read_offset,
             circ_buf_data_size );
      break;
    }

    return TRUE;
  }
  
  return FALSE;
}

ADSPResult vsm_utility_get_mailbox_enc_buffer (
  vsm_session_object_t* session_obj,
  vss_ipktexg_mailbox_enc_request_t* enc_req,
  vss_ipktexg_mailbox_enc_packet_t** ret_enc_buffer
)
{
  uint32_t rc = ADSP_EOK;
  cvd_virt_addr_t enc_buf_virt_addr;
  vsm_mailbox_packet_exchange_config_t* mailbox_config;
  
  for ( ;; )
  {
    if ( ( enc_req == NULL ) || ( ret_enc_buffer == NULL ) )
    {
      VSM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      rc = APR_EBADPARAM;
      break;
    }

    mailbox_config = &session_obj->packet_exchange_info.mailbox_info.config;

    rc = cvd_mem_mapper_validate_attributes_align(
           mailbox_config->cvs_mem_handle, enc_req->mem_address );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Mis-aligned encoder packet buffer mem_address: "
             "MSW:(0X%08X) LSW:(0X%08X)", (uint32_t) (enc_req->mem_address>>32),
             (uint32_t) (enc_req->mem_address) );
      rc = APR_EFAILED;
      break;
    }

    rc = cvd_mem_mapper_validate_attributes_align(
           mailbox_config->cvs_mem_handle, enc_req->mem_size );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Mis-aligned encoder packet buffer size: %d.",
             enc_req->mem_size );
      rc = APR_EFAILED;
      break;
    }
  
    if ( enc_req->mem_size < mailbox_config->max_enc_pkt_size )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Encoder packet buffer size %d not large enough "
             "to hold max encoder packet size %d.", enc_req->mem_size,
             mailbox_config->max_enc_pkt_size );
      rc = APR_EFAILED;
      break;
    }

    rc = cvd_mem_mapper_validate_mem_is_in_region(
           mailbox_config->cvs_mem_handle, enc_req->mem_address, enc_req->mem_size );
    if ( rc )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Encoder packet buffer is not within range, "
             "mem_handle: 0x%08X, addr: 0x%016X, size: %d.",
             mailbox_config->cvs_mem_handle, enc_req->mem_address, enc_req->mem_size );
      rc = APR_EFAILED;
      break;
    }

    rc = cvd_mem_mapper_get_virtual_addr_v2( mailbox_config->cvs_mem_handle,
                                             enc_req->mem_address,
                                             &enc_buf_virt_addr );
    if ( rc )
    {
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Cannot get virtual address, mem_handle: 0x%08X, addr_MSW:(0X%08X) "
             "addr_LSW:(0X%08X)", mailbox_config->cvs_mem_handle,
             (uint32_t) (enc_req->mem_address>>32), (uint32_t) (enc_req->mem_address) );
      rc = APR_EFAILED;
      break;
    }

    *ret_enc_buffer = (vss_ipktexg_mailbox_enc_packet_t*) enc_buf_virt_addr.ptr;

    break;
  }

  return rc;
}

ADSPResult vsm_utility_get_mailbox_dec_buffer (
  vsm_session_object_t* session_obj,
  vss_ipktexg_mailbox_dec_request_t* dec_req,
  vss_ipktexg_mailbox_dec_packet_t** ret_dec_buffer
)
{
  uint32_t rc = ADSP_EOK;
  cvd_virt_addr_t dec_buf_virt_addr;
  vss_ipktexg_mailbox_dec_packet_t* dec_buffer;
  vsm_mailbox_packet_exchange_config_t* mailbox_config;

  for ( ;; )
  {
    if ( ( session_obj == NULL ) || ( dec_req == NULL ) ||
         ( ret_dec_buffer == NULL ) )
    {
      VSM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      rc = APR_EBADPARAM;
      break;
    }
  
    mailbox_config = &session_obj->packet_exchange_info.mailbox_info.config;
  
    rc = cvd_mem_mapper_validate_attributes_align(
           mailbox_config->cvs_mem_handle, dec_req->mem_address );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Mis-aligned decoder packet buffer mem_address: "
             "MSW:(0X%08X) LSW:(0X%08X)", (uint32_t)(dec_req->mem_address>>32),
             (uint32_t)(dec_req->mem_address) );
      rc = APR_EFAILED;
      break;
    }
  
    rc = cvd_mem_mapper_validate_mem_is_in_region( 
           mailbox_config->cvs_mem_handle, dec_req->mem_address, dec_req->mem_size );
    if ( rc )
    {
      MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Decoder packet buffer is not within range, "
             "mem_handle: 0x%08X, addrMSW:(0X%08X) addrLSW:(0X%08X), size: %d.",
             mailbox_config->cvs_mem_handle, (uint32_t)(dec_req->mem_address>>32),
             (uint32_t)(dec_req->mem_address), dec_req->mem_size );
      rc = APR_EFAILED;
      break;
    }
  
    if ( dec_req->mem_size < sizeof( vss_ipktexg_mailbox_dec_packet_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Invalid decoder packet buffer mem "
             "size %d < decoder packet header size %d.", dec_req->mem_size,
             sizeof( vss_ipktexg_mailbox_dec_packet_t ) );
      rc = APR_EFAILED;
      break;
    }
  
    rc = cvd_mem_mapper_get_virtual_addr_v2( 
           mailbox_config->cvs_mem_handle, dec_req->mem_address, &dec_buf_virt_addr );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Cannot get virtual address, mem_handle: 0x%08X, addr: 0x%016X.",
             mailbox_config->cvs_mem_handle, dec_req->mem_address );
      rc = APR_EFAILED;
      break;
    }

    ( void ) cvd_mem_mapper_cache_invalidate_v2( &dec_buf_virt_addr, dec_req->mem_size );

    dec_buffer = ( ( vss_ipktexg_mailbox_dec_packet_t* ) dec_buf_virt_addr.ptr );

    if ( dec_buffer->minor_version != 0 )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Unsupported decoder packet minor version %d. "
             "Supported minor version is 0. ",  dec_buffer->minor_version );
      rc = APR_EFAILED;
      break;
    }

    if ( ( dec_buffer->media_id != session_obj->active_set.media_id )
         && ( VSS_MEDIA_ID_DTMF != dec_buffer->media_id ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Invalid decoder packet media ID 0x%08X. "
             "Media ID configured on the stream is 0x%08X. ", dec_buffer->media_id,
             session_obj->active_set.media_id );
      rc = APR_EFAILED;
      break;
    }

    if ( dec_buffer->data_size > 
         ( dec_req->mem_size - sizeof( vss_ipktexg_mailbox_dec_packet_t ) ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: Client error: Decoder packet data size %d does not fit within "
             "the decoder packet buffer of size %d.", dec_buffer->data_size,
             dec_req->mem_size );
      rc = APR_EFAILED;
      break;
    }

    *ret_dec_buffer = dec_buffer;

    break;
  }

  return rc;
}

ADSPResult vsm_utility_complete_pending_voc_request (
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* circ_buf,
  uint32_t circ_buf_mem_size,
  uint32_t voc_req_unit_size
)
{
  uint32_t circ_buf_data_size;
  cvd_virt_addr_t read_offset_virt_addr;
  
  if ( NULL == circ_buf )
  {
    VSM_PANIC_ON_ERROR( ADSP_EUNEXPECTED );
    return ADSP_EBADPARAM;
  }

  circ_buf_data_size = 
    ( circ_buf_mem_size - sizeof( vss_ipktexg_mailbox_voc_req_circ_buffer_t ) );

  circ_buf->read_offset = 
    ( ( circ_buf->read_offset + voc_req_unit_size ) % circ_buf_data_size );

  read_offset_virt_addr.ptr = &circ_buf->read_offset;

  ( void ) cvd_mem_mapper_cache_flush_v2( 
             &read_offset_virt_addr, sizeof( circ_buf->read_offset ) );

  return ADSP_EOK;
}

uint32_t vsm_utility_get_no_data_frame_header_type(uint32_t media_id)
{
  uint32_t no_data_frame_type_header = 0;
  switch (media_id)
  {
    case VSS_MEDIA_ID_AMR_NB:
    case VSS_MEDIA_ID_AMR_WB:
      no_data_frame_type_header = AMR_NO_DATA_FRAME_TYPE;
      break;
    case VSS_MEDIA_ID_EVS:
      no_data_frame_type_header = EVS_NO_DATA_FRAME_TYPE;
      break;
    case VSS_MEDIA_ID_G711_ALAW_V2:
    case VSS_MEDIA_ID_G711_MULAW_V2:
      no_data_frame_type_header = G711_NO_DATA_FRAME_TYPE;
      break;
    default: 
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: vsm_utility_get_no_data_frame_header_type error: improper media format %d provided ", media_id);
      break;
  }
  return no_data_frame_type_header;
}
ADSPResult vsm_utility_log_dtmf_mailbox_pkt_gen_no_data_pkt(
  vsm_session_object_t* session_obj,
  vss_ipktexg_mailbox_dec_packet_t* mailbox_dec_packet,
  uint32_t apr_packet_token,
  elite_apr_packet_t** dec_packet
)
{
  elite_apr_packet_t* dec_apr_pkt_ptr = *dec_packet;
  vss_ipktexg_evt_in_band_send_dec_buffer_t* inband_dec_packet = NULL;
  uint32_t no_data_frame_header = 0;

  ( void ) cvd_log_commit_data( LOG_ADSP_CVD_STREAM_RX,
           ++session_obj->packet_logging_info.rx_packet_seq_num, mailbox_dec_packet->data_size,
           ( session_obj->attached_mvm_handle << 16 ) | session_obj->apr_info.client_port,
           session_obj->packet_logging_info.voice_call_num, 0, CVD_LOG_DEC_INPUT_TAP_POINT,
           mailbox_dec_packet->media_id, ( ( uint8_t* )mailbox_dec_packet ) + sizeof( vss_ipktexg_mailbox_dec_packet_t ) );

  ( void ) elite_apr_if_alloc_event( session_obj->apr_info.apr_handle,
           session_obj->apr_info.client_addr, session_obj->apr_info.client_port,
           session_obj->apr_info.self_addr, session_obj->apr_info.self_port,
           apr_packet_token, VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER,
           (CVD_LOG_FRAME_HEADER_SIZE_V + sizeof(vss_ipktexg_evt_in_band_send_dec_buffer_t)), &dec_apr_pkt_ptr );

  inband_dec_packet = APRV2_PKT_GET_PAYLOAD( vss_ipktexg_evt_in_band_send_dec_buffer_t, dec_apr_pkt_ptr );
  inband_dec_packet->media_id = session_obj->active_set.media_id;

  no_data_frame_header = vsm_utility_get_no_data_frame_header_type(session_obj->active_set.media_id);
  ( void ) mmstd_memset((uint8_t*)inband_dec_packet + sizeof(vss_ipktexg_evt_in_band_send_dec_buffer_t), 0, CVD_LOG_FRAME_HEADER_SIZE_V);
  if (no_data_frame_header)
  {
    *((uint8_t*)inband_dec_packet + sizeof(vss_ipktexg_evt_in_band_send_dec_buffer_t)) = ((no_data_frame_header & 0x0F) << 4);
    *dec_packet = dec_apr_pkt_ptr;
    return ADSP_EOK;
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VSM: vsm_utility_log_dtmf_mailbox_pkt_gen_no_data_pkt error: couldnt update frame header ");
    return ADSP_EFAILED;
  }
}