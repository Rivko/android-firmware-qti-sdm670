/******************************************************************************
  @file    qcril_qmi_ims_misc.c
  @brief   qcril qmi - ims misc

  DESCRIPTION
    Utility functions for ims socket.

  ---------------------------------------------------------------------------

  Copyright (c) 2012-2015,2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/

#include "qcril_qmi_ims_misc.h"
#include "qcril_log.h"
#include "qcrili.h"
#include "qcril_qmi_npb_utils.h"
#include "data_system_determination_v01.h"
#include "ip_multimedia_subsystem_settings_v01.h"

#define WLAN_HANDOVER_NO_LTE_FAILURE_CODE_STRING "CD-04"

//===========================================================================
// qcril_qmi_ims_map_event_to_request
//===========================================================================
ims_MsgId qcril_qmi_ims_map_event_to_request(int event)
{
  ims_MsgId ret;

  switch ( event )
  {
    case QCRIL_EVT_IMS_SOCKET_IMS_REGISTRATION_STATE:
      ret = ims_MsgId_REQUEST_IMS_REGISTRATION_STATE;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_DIAL:
      ret = ims_MsgId_REQUEST_DIAL;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_ANSWER:
      ret = ims_MsgId_REQUEST_ANSWER;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_HANGUP:
      ret = ims_MsgId_REQUEST_HANGUP;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_LAST_CALL_FAIL_CAUSE:
      ret = ims_MsgId_REQUEST_LAST_CALL_FAIL_CAUSE;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_CURRENT_CALLS:
      ret = ims_MsgId_REQUEST_GET_CURRENT_CALLS;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_HANGUP_WAITING_OR_BACKGROUND:
      ret = ims_MsgId_REQUEST_HANGUP_WAITING_OR_BACKGROUND;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_HANGUP_FOREGROUND_RESUME_BACKGROUND:
      ret = ims_MsgId_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
      ret = ims_MsgId_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_CONFERENCE:
        ret = ims_MsgId_REQUEST_CONFERENCE;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_EXIT_ECBM:
        ret = ims_MsgId_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_EXPLICIT_CALL_TRANSFER:
        ret = ims_MsgId_REQUEST_EXPLICIT_CALL_TRANSFER;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_DTMF:
    case QCRIL_EVT_IMS_SOCKET_REQ_STOP_CONT_DTMF:
        ret = ims_MsgId_REQUEST_DTMF;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_DTMF_START:
        ret = ims_MsgId_REQUEST_DTMF_START;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_DTMF_STOP:
        ret = ims_MsgId_REQUEST_DTMF_STOP;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_MODIFY_CALL_INITIATE:
        ret = ims_MsgId_REQUEST_MODIFY_CALL_INITIATE;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_MODIFY_CALL_CONFIRM:
        ret = ims_MsgId_REQUEST_MODIFY_CALL_CONFIRM;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_QUERY_CLIP:
        ret = ims_MsgId_REQUEST_QUERY_CLIP;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_CLIR:
        ret = ims_MsgId_REQUEST_GET_CLIR;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_CLIR:
        ret = ims_MsgId_REQUEST_SET_CLIR;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_QUERY_CALL_FORWARD_STATUS:
        ret = ims_MsgId_REQUEST_QUERY_CALL_FORWARD_STATUS;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_CALL_FORWARD_STATUS:
        ret = ims_MsgId_REQUEST_SET_CALL_FORWARD_STATUS;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_QUERY_CALL_WAITING:
        ret = ims_MsgId_REQUEST_QUERY_CALL_WAITING;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_CALL_WAITING:
        ret = ims_MsgId_REQUEST_SET_CALL_WAITING;
        break;

    case QCRIL_EVT_IMS_SOCKET_REQ_IMS_REG_STATE_CHANGE:
      ret = ims_MsgId_REQUEST_IMS_REG_STATE_CHANGE;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_SUPP_SVC_NOTIFICATION:
      ret = ims_MsgId_REQUEST_SET_SUPP_SVC_NOTIFICATION;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_ADD_PARTICIPANT:
      ret = ims_MsgId_REQUEST_ADD_PARTICIPANT;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_QUERY_SERVICE_STATUS:
      ret = ims_MsgId_REQUEST_QUERY_SERVICE_STATUS;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_SERVICE_STATUS:
      ret = ims_MsgId_REQUEST_SET_SERVICE_STATUS;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SUPP_SVC_STATUS:
      ret = ims_MsgId_REQUEST_SUPP_SVC_STATUS;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_CALL_DEFLECTION:
      ret = ims_MsgId_REQUEST_DEFLECT_CALL;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_COLR:
      ret = ims_MsgId_REQUEST_GET_COLR;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_COLR:
      ret = ims_MsgId_REQUEST_SET_COLR;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_QUERY_VT_CALL_QUALITY:
      ret = ims_MsgId_REQUEST_QUERY_VT_CALL_QUALITY;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_VT_CALL_QUALITY:
      ret = ims_MsgId_REQUEST_SET_VT_CALL_QUALITY;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_HOLD:
      ret = ims_MsgId_REQUEST_HOLD;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_RESUME:
      ret = ims_MsgId_REQUEST_RESUME;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_TTY_MODE:
      ret = ims_MsgId_REQUEST_SEND_UI_TTY_MODE;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_RTP_STATISTICS:
      ret = ims_MsgId_REQUEST_GET_RTP_STATISTICS;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_RTP_ERROR_STATISTICS:
      ret = ims_MsgId_REQUEST_GET_RTP_ERROR_STATISTICS;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_WIFI_CALLING_STATUS:
      ret = ims_MsgId_REQUEST_GET_WIFI_CALLING_STATUS;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_WIFI_CALLING_STATUS:
      ret = ims_MsgId_REQUEST_SET_WIFI_CALLING_STATUS;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_IMS_CONFIG:
      ret = ims_MsgId_REQUEST_SET_IMS_CONFIG;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_CONFIG:
      ret = ims_MsgId_REQUEST_GET_IMS_CONFIG;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_GEO_LOC:
      ret = ims_MsgId_REQUEST_SEND_GEOLOCATION_INFO;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_VOWIFI_CALL_QUALITY:
      ret = ims_MsgId_REQUEST_GET_VOWIFI_CALL_QUALITY;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_VOPS_INFO:
      ret = ims_MsgId_REQUEST_GET_VOPS_INFO;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_SSAC_INFO:
      ret = ims_MsgId_REQUEST_GET_SSAC_INFO;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_VOLTE_PREF:
      ret = ims_MsgId_REQUEST_SET_VOLTE_PREF;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_VOLTE_PREF:
      ret = ims_MsgId_REQUEST_GET_VOLTE_PREF;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_HANDOVER_CONFIG:
      ret = ims_MsgId_REQUEST_GET_HANDOVER_CONFIG;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SET_HANDOVER_CONFIG:
      ret = ims_MsgId_REQUEST_SET_HANDOVER_CONFIG;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_SUB_CONFIG:
      ret = ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_SEND_RTT_MSG:
      ret = ims_MsgId_REQUEST_SEND_RTT_MSG;
      break;

    case QCRIL_EVT_IMS_SOCKET_REQ_CANCEL_MODIFY_CALL:
      ret = ims_MsgId_REQUEST_CANCEL_MODIFY_CALL;
      break;

    default:
      QCRIL_LOG_DEBUG("didn't find direct mapping for event %d", event);
      if ( event > QCRIL_EVT_IMS_SOCKET_REQ_BASE && event < QCRIL_EVT_IMS_SOCKET_REQ_MAX )
      {
        ret = event - QCRIL_EVT_IMS_SOCKET_REQ_BASE;
      }
      else
      {
        ret = ims_MsgId_UNKNOWN_REQ;
      }
  }

  QCRIL_LOG_INFO("event %d mapped to ims_msg %d", event, ret);
  return ret;
} // qcril_qmi_ims_map_event_to_request

//===========================================================================
// qcril_qmi_ims_map_request_to_event
//===========================================================================
qcril_evt_e_type qcril_qmi_ims_map_request_to_event(ims_MsgId msg_id)
{
  qcril_evt_e_type ret = QCRIL_EVT_NONE;

  switch ( msg_id )
  {
    case ims_MsgId_REQUEST_IMS_REGISTRATION_STATE:
      ret = QCRIL_EVT_IMS_SOCKET_IMS_REGISTRATION_STATE;
      break;

    case ims_MsgId_REQUEST_DIAL:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_DIAL;
      break;

    case ims_MsgId_REQUEST_ANSWER:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_ANSWER;
      break;

    case ims_MsgId_REQUEST_HANGUP:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_HANGUP;
      break;

    case ims_MsgId_REQUEST_LAST_CALL_FAIL_CAUSE:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_LAST_CALL_FAIL_CAUSE;
      break;

    case ims_MsgId_REQUEST_GET_CURRENT_CALLS:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_CURRENT_CALLS;
      break;

    case ims_MsgId_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_HANGUP_WAITING_OR_BACKGROUND;
      break;

    case ims_MsgId_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_HANGUP_FOREGROUND_RESUME_BACKGROUND;
      break;

    case ims_MsgId_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE;
      break;

    case ims_MsgId_REQUEST_CONFERENCE:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_CONFERENCE;
        break;

    case ims_MsgId_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_EXIT_ECBM;
        break;

    case ims_MsgId_REQUEST_EXPLICIT_CALL_TRANSFER:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_EXPLICIT_CALL_TRANSFER;
        break;

    case ims_MsgId_REQUEST_DTMF:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_DTMF;
        break;

    case ims_MsgId_REQUEST_DTMF_START:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_DTMF_START;
        break;

    case ims_MsgId_REQUEST_DTMF_STOP:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_DTMF_STOP;
        break;

    case ims_MsgId_REQUEST_MODIFY_CALL_INITIATE:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_MODIFY_CALL_INITIATE;
        break;

    case ims_MsgId_REQUEST_MODIFY_CALL_CONFIRM:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_MODIFY_CALL_CONFIRM;
        break;

    case ims_MsgId_REQUEST_QUERY_CLIP:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_QUERY_CLIP;
        break;

    case ims_MsgId_REQUEST_GET_CLIR:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_CLIR;
        break;

    case ims_MsgId_REQUEST_SET_CLIR:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_CLIR;
        break;

    case ims_MsgId_REQUEST_QUERY_CALL_FORWARD_STATUS:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_QUERY_CALL_FORWARD_STATUS;
        break;

    case ims_MsgId_REQUEST_SET_CALL_FORWARD_STATUS:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_CALL_FORWARD_STATUS;
        break;

    case ims_MsgId_REQUEST_QUERY_CALL_WAITING:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_QUERY_CALL_WAITING;
        break;

    case ims_MsgId_REQUEST_SET_CALL_WAITING:
        ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_CALL_WAITING;
        break;

    case ims_MsgId_REQUEST_IMS_REG_STATE_CHANGE:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_IMS_REG_STATE_CHANGE;
      break;

    case ims_MsgId_REQUEST_SET_SUPP_SVC_NOTIFICATION:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_SUPP_SVC_NOTIFICATION;
      break;

    case ims_MsgId_REQUEST_ADD_PARTICIPANT:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_ADD_PARTICIPANT;
      break;

    case ims_MsgId_REQUEST_QUERY_SERVICE_STATUS:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_QUERY_SERVICE_STATUS;
      break;

    case ims_MsgId_REQUEST_SET_SERVICE_STATUS:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_SERVICE_STATUS;
      break;

    case ims_MsgId_REQUEST_SUPP_SVC_STATUS:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SUPP_SVC_STATUS;
      break;

    case ims_MsgId_REQUEST_DEFLECT_CALL:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_CALL_DEFLECTION;
      break;

    case ims_MsgId_REQUEST_GET_COLR:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_COLR;
      break;

    case ims_MsgId_REQUEST_SET_COLR:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_COLR;
      break;

    case ims_MsgId_REQUEST_QUERY_VT_CALL_QUALITY:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_QUERY_VT_CALL_QUALITY;
      break;

    case ims_MsgId_REQUEST_SET_VT_CALL_QUALITY:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_VT_CALL_QUALITY;
      break;

    case ims_MsgId_REQUEST_HOLD:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_HOLD;
      break;

    case ims_MsgId_REQUEST_RESUME:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_RESUME;
      break;

    case ims_MsgId_REQUEST_SEND_UI_TTY_MODE:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_TTY_MODE;
      break;

    case ims_MsgId_REQUEST_GET_RTP_STATISTICS:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_RTP_STATISTICS;
      break;

    case ims_MsgId_REQUEST_GET_RTP_ERROR_STATISTICS:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_RTP_ERROR_STATISTICS;
      break;

    case ims_MsgId_REQUEST_SET_IMS_CONFIG:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_IMS_CONFIG;
      break;

    case ims_MsgId_REQUEST_GET_IMS_CONFIG:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_CONFIG;
      break;

    case ims_MsgId_REQUEST_SEND_GEOLOCATION_INFO:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_GEO_LOC;
      break;

    case ims_MsgId_REQUEST_GET_VOWIFI_CALL_QUALITY:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_VOWIFI_CALL_QUALITY;
      break;

    case ims_MsgId_REQUEST_GET_VOPS_INFO:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_VOPS_INFO;
      break;

    case ims_MsgId_REQUEST_GET_SSAC_INFO:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_SSAC_INFO;
      break;

    case ims_MsgId_REQUEST_SET_VOLTE_PREF:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_VOLTE_PREF;
      break;

    case ims_MsgId_REQUEST_GET_VOLTE_PREF:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_VOLTE_PREF;
      break;

    case ims_MsgId_REQUEST_GET_HANDOVER_CONFIG:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_HANDOVER_CONFIG;
      break;

    case ims_MsgId_REQUEST_SET_HANDOVER_CONFIG:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SET_HANDOVER_CONFIG;
      break;

    case ims_MsgId_REQUEST_GET_IMS_SUB_CONFIG:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_SUB_CONFIG;
      break;

    case ims_MsgId_REQUEST_SEND_RTT_MSG:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_SEND_RTT_MSG;
      break;

    case ims_MsgId_REQUEST_CANCEL_MODIFY_CALL:
      ret = QCRIL_EVT_IMS_SOCKET_REQ_CANCEL_MODIFY_CALL;
      break;

    default:
      QCRIL_LOG_DEBUG("didn't find direct mapping for msg_id %d", msg_id);
      ret = QCRIL_EVT_NONE;
      break;
  }

  QCRIL_LOG_INFO("msg_id %d mapped to qcril_event %d", msg_id, ret);
  return ret;
} // qcril_qmi_ims_map_event_to_request
//===========================================================================
// qcril_qmi_ims_map_ril_error_to_ims_error
//===========================================================================
ims_Error qcril_qmi_ims_map_ril_error_to_ims_error(int ril_error)
{
  ims_Error ret;

  switch ( ril_error )
  {
    case RIL_E_SUCCESS:
      ret = ims_Error_E_SUCCESS;
      break;

    case RIL_E_RADIO_NOT_AVAILABLE:
      ret = ims_Error_E_RADIO_NOT_AVAILABLE;
      break;

    case RIL_E_GENERIC_FAILURE:
      ret = ims_Error_E_GENERIC_FAILURE;
      break;

    case RIL_E_REQUEST_NOT_SUPPORTED:
      ret = ims_Error_E_REQUEST_NOT_SUPPORTED;
      break;

    case RIL_E_CANCELLED:
      ret = ims_Error_E_CANCELLED;
      break;

    case RIL_E_HOLD_RESUME_FAILED:
      ret = ims_Error_E_HOLD_RESUME_FAILED;
      break;
    case RIL_E_HOLD_RESUME_CANCELED:
      ret = ims_Error_E_HOLD_RESUME_CANCELED;
      break;
    case RIL_E_REINVITE_COLLISION:
      ret = ims_Error_E_REINVITE_COLLISION;
      break;

    case RIL_E_FDN_CHECK_FAILURE:
      ret = ims_Error_E_FDN_CHECK_FAILURE;
      break;

    default:
      ret = ims_Error_E_GENERIC_FAILURE;
  }

  QCRIL_LOG_INFO("ril error %d mapped to ims error %d", ril_error, ret);
  return ret;
} // qcril_qmi_ims_map_ril_error_to_ims_error
//===========================================================================
// qcril_qmi_ims_map_radio_config_error_to_ims_error
//===========================================================================
ims_Error qcril_qmi_ims_map_radio_config_error_to_ims_error(qcril_qmi_radio_config_error_type radio_config_error)
{
  ims_Error ret;

  switch ( radio_config_error )
  {
    case QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS:
      ret = ims_Error_E_SUCCESS;
      break;

    case QCRIL_QMI_RADIO_CONFIG_ERROR_REQUEST_NOT_SUPPORTED:
      ret = ims_Error_E_REQUEST_NOT_SUPPORTED;
      break;

    default:
      ret = ims_Error_E_GENERIC_FAILURE;
      break;
  }

  QCRIL_LOG_INFO("radio config error %d mapped to ims error %d", radio_config_error, ret);
  return ret;
}// qcril_qmi_ims_map_radio_config_error_to_ims_error

//===========================================================================
// qcril_qmi_ims_map_ril_reg_state_to_ims_reg_state
//===========================================================================
ims_Registration_RegState qcril_qmi_ims_map_ril_reg_state_to_ims_reg_state(int state)
{
  ims_Registration_RegState ret;

  switch ( state )
  {
    case 0:
      ret = ims_Registration_RegState_NOT_REGISTERED;
      break;

    case 1:
      ret = ims_Registration_RegState_REGISTERED;
      break;

    default:
      ret = ims_Registration_RegState_NOT_REGISTERED;
  }

  QCRIL_LOG_INFO("ril reg state %d mapped to ims reg state %d", state, ret);

  return ret;
} // qcril_qmi_ims_map_ril_reg_state_to_ims_reg_state

//===========================================================================
// qcril_qmi_ims_map_ril_call_type_to_ims_call_type
//===========================================================================
ims_CallType qcril_qmi_ims_map_ril_call_type_to_ims_call_type(RIL_Call_Type call_type)
{
  ims_CallType ret;

  switch ( call_type )
  {
    case RIL_CALL_TYPE_VOICE:
      ret = ims_CallType_CALL_TYPE_VOICE;
      break;

    case RIL_CALL_TYPE_VS_TX:
      ret = ims_CallType_CALL_TYPE_VT_TX;
      break;

    case RIL_CALL_TYPE_VS_RX:
      ret = ims_CallType_CALL_TYPE_VT_RX;
      break;

    case RIL_CALL_TYPE_VT:
      ret = ims_CallType_CALL_TYPE_VT;
      break;

    case RIL_CALL_TYPE_VT_NODIR:
      ret = ims_CallType_CALL_TYPE_VT_NODIR;
      break;

    default:
      ret = (ims_CallType)call_type; // do not do any mapping if it does not fill in any above categories.
  }

  QCRIL_LOG_INFO("ril call_type %d mapped to ims call_type %d", call_type, ret);

  return ret;
} // qcril_qmi_ims_map_ril_call_type_to_ims_call_type

//===========================================================================
// qcril_qmi_ims_map_ril_call_domain_to_ims_call_domain
//===========================================================================
ims_CallDomain qcril_qmi_ims_map_ril_call_domain_to_ims_call_domain(RIL_Call_Domain call_domain)
{
  ims_CallDomain ret;

  switch ( call_domain )
  {
    case RIL_CALL_DOMAIN_UNKNOWN:
      ret = ims_CallDomain_CALL_DOMAIN_UNKNOWN;
      break;

    case RIL_CALL_DOMAIN_CS:
      ret = ims_CallDomain_CALL_DOMAIN_CS;
      break;

    case RIL_CALL_DOMAIN_PS:
      ret = ims_CallDomain_CALL_DOMAIN_PS;
      break;

    case RIL_CALL_DOMAIN_AUTOMATIC:
      ret = ims_CallDomain_CALL_DOMAIN_AUTOMATIC;
      break;

    default:
      ret = ims_CallDomain_CALL_DOMAIN_UNKNOWN;
  }

  QCRIL_LOG_INFO("ril call_domain %d mapped to ims call_domain %d", call_domain, ret);

  return ret;
} // qcril_qmi_ims_map_ril_call_domain_to_ims_call_domain

//===========================================================================
// qcril_qmi_ims_map_ims_call_type_to_ril_call_type
//===========================================================================
RIL_Call_Type qcril_qmi_ims_map_ims_call_type_to_ril_call_type(boolean has_call_type, ims_CallType call_type)
{
  RIL_Call_Type ret = RIL_CALL_TYPE_VOICE;

  if (has_call_type)
  {
     switch ( call_type )
     {
       case ims_CallType_CALL_TYPE_VOICE:
         ret = RIL_CALL_TYPE_VOICE;
         break;

       case ims_CallType_CALL_TYPE_VT_TX:
         ret = RIL_CALL_TYPE_VS_TX;
         break;

       case ims_CallType_CALL_TYPE_VT_RX:
         ret = RIL_CALL_TYPE_VS_RX;
         break;

       case ims_CallType_CALL_TYPE_VT:
         ret = RIL_CALL_TYPE_VT;
         break;

       case ims_CallType_CALL_TYPE_VT_NODIR:
         ret = RIL_CALL_TYPE_VT_NODIR;
         break;

       default:
         ret = RIL_CALL_TYPE_VOICE;
     }
  }

  QCRIL_LOG_INFO("ims has_call_type %d, call_type %d mapped to ril call_type %d", has_call_type, call_type, ret);

  return ret;
} // qcril_qmi_ims_map_ims_call_type_to_ril_call_type

//===========================================================================
// qcril_qmi_ims_map_ims_call_domain_to_ril_call_domain
//===========================================================================
RIL_Call_Domain qcril_qmi_ims_map_ims_call_domain_to_ril_call_domain(boolean has_call_domain, ims_CallDomain call_domain)
{
  RIL_Call_Domain ret = RIL_CALL_DOMAIN_UNKNOWN;

  if (has_call_domain)
  {
     switch ( call_domain )
     {
       case ims_CallDomain_CALL_DOMAIN_UNKNOWN:
         ret = RIL_CALL_DOMAIN_UNKNOWN;
         break;

       case ims_CallDomain_CALL_DOMAIN_CS:
         ret = RIL_CALL_DOMAIN_CS;
         break;

       case ims_CallDomain_CALL_DOMAIN_PS:
         ret = RIL_CALL_DOMAIN_PS;
         break;

       case ims_CallDomain_CALL_DOMAIN_AUTOMATIC:
         ret = RIL_CALL_DOMAIN_AUTOMATIC;
         break;

       default:
         ret = RIL_CALL_DOMAIN_UNKNOWN;
     }
  }

  QCRIL_LOG_INFO("ims has_call_domain %d, call_domain %d mapped to ims call_domain %d", has_call_domain, call_domain, ret);

  return ret;
} // qcril_qmi_ims_map_ims_call_domain_to_ril_call_domain

//===========================================================================
// qcril_qmi_ims_convert_ims_token_to_ril_token
//===========================================================================
RIL_Token qcril_qmi_ims_convert_ims_token_to_ril_token(uint32_t ims_token)
{
  RIL_Token ret = qcril_malloc(sizeof(uint32_t));
  if (NULL != ret)
  {
    uint32_t *tmp = (uint32_t*) ret;
    *tmp = ims_token ^ 0x80000000;
  }
  return ret;
} // qcril_qmi_ims_convert_ims_token_to_ril_token

//===========================================================================
// qcril_qmi_ims_free_and_convert_ril_token_to_ims_token
//===========================================================================
uint32_t qcril_qmi_ims_free_and_convert_ril_token_to_ims_token(RIL_Token ril_token)
{
  uint32_t ret = 0xFFFFFFFF;
  if (ril_token)
  {
      ret = (*((uint32_t *) ril_token)) ^ 0x80000000;
      QCRIL_LOG_INFO("ims token: %d", ret);
      qcril_free((void*) ril_token);
  }
  else
  {
      QCRIL_LOG_INFO("ril_token is NULL");
  }

  return ret;
} // qcril_qmi_ims_free_and_convert_ril_token_to_ims_token

//===========================================================================
// qcril_qmi_ims_translate_ims_calldetails_to_ril_calldetails
//===========================================================================
void qcril_qmi_ims_translate_ims_calldetails_to_ril_calldetails(const ims_CallDetails *ims_data, RIL_Call_Details* ril_data)
{
  if (NULL == ril_data || NULL == ims_data)
  {
    QCRIL_LOG_ERROR("ril_data or ims_data is NULL");
    if (NULL != ril_data)
    {
       QCRIL_LOG_INFO("ril_data is not NULL, set it to default value");
       ril_data->callType = qcril_qmi_ims_map_ims_call_type_to_ril_call_type(FALSE, 0);
       ril_data->callDomain = qcril_qmi_ims_map_ims_call_domain_to_ril_call_domain(FALSE, 0);
    }
  }
  else
  {
    ril_data->callType = qcril_qmi_ims_map_ims_call_type_to_ril_call_type(
            ims_data->has_callType,
            ims_data->callType);
    ril_data->callDomain = qcril_qmi_ims_map_ims_call_domain_to_ril_call_domain(
            ims_data->has_callDomain,
            ims_data->callDomain);
  }
} // qcril_qmi_ims_translate_ims_calldetails_to_ril_calldetails

//===========================================================================
// qcril_qmi_ims_translate_ims_calldetails_to_ril_calldetails
//===========================================================================
void qcril_qmi_ims_translate_ril_callmodify_to_ims_callmodify(const RIL_Call_Modify* ril_data, ims_CallModify* ims_data)
{
  if (NULL == ril_data || NULL == ims_data)
  {
    QCRIL_LOG_ERROR("ril_data or ims_data is NULL");
    if (NULL != ims_data)
    {
       QCRIL_LOG_INFO("ims_data is not NULL, set it to default value");
       ims_data->has_callIndex = FALSE;
       ims_data->has_callDetails = FALSE;
    }
  }
  else
  {
    ims_data->has_callIndex = TRUE;
    ims_data->callIndex = ril_data->callIndex;
    ims_data->has_callDetails = TRUE;
    qcril_qmi_ims_translate_ril_calldetails_to_ims_calldetails(ril_data->callDetails,
           &ims_data->callDetails);
  }
} // qcril_qmi_ims_translate_ril_callmodify_to_ims_callmodify

//===========================================================================
// qcril_qmi_ims_translate_ril_calldetails_to_ims_calldetails
//===========================================================================
void qcril_qmi_ims_translate_ril_calldetails_to_ims_calldetails(const RIL_Call_Details* ril_data, ims_CallDetails* ims_data)
{
  if (NULL == ril_data || NULL == ims_data)
  {
    QCRIL_LOG_ERROR("ril_data or ims_data is NULL");
    if (NULL != ims_data)
    {
       QCRIL_LOG_INFO("ims_data is not NULL, set it to default value");
       ims_data->has_callType = FALSE;
       ims_data->has_callDomain = FALSE;
    }
  }
  else
  {
    ims_data->has_callType = TRUE;
    ims_data->callType = qcril_qmi_ims_map_ril_call_type_to_ims_call_type(ril_data->callType);
    ims_data->has_callDomain = TRUE;
    ims_data->callDomain = qcril_qmi_ims_map_ril_call_domain_to_ims_call_domain(ril_data->callDomain);
  }
} // qcril_qmi_ims_translate_ril_calldetails_to_ims_calldetails

//===========================================================================
// qcril_qmi_ims_translate_ril_callforwdinfo_to_ims_callforwdinfo
//===========================================================================
void qcril_qmi_ims_translate_ril_callforwdinfo_to_ims_callforwdinfo
(
 const qcril_qmi_voice_callforwd_info_param_u_type* ril_data,
 int num,
 voice_time_type_v02 *call_fwd_start_time,
 voice_time_type_v02 *call_fwd_end_time,
 ims_CallForwardInfoList* ims_data
)
{
  int i;
  boolean failed = FALSE;

  if (NULL == ril_data || NULL == ims_data || num < 0)
  {
    QCRIL_LOG_ERROR("ril_data or ims_data is NULL, or num < 0");
  }
  else
  {
    do
    {
      memset(ims_data, 0, sizeof(ims_CallForwardInfoList));

      if (num > 0)
      {
        ims_data->info.arg = qcril_malloc(sizeof(ims_CallForwardInfoList_CallForwardInfo*) *
                (num+1));
        if (NULL == ims_data->info.arg)
        {
          QCRIL_LOG_FATAL("malloc failed");
          failed = TRUE;
          break;
        }
        for (i = 0; i < num; i++)
        {
          ims_CallForwardInfoList_CallForwardInfo* cfi = qcril_malloc(
                  sizeof(ims_CallForwardInfoList_CallForwardInfo));
          if (NULL == cfi)
          {
            QCRIL_LOG_FATAL("malloc failed");
            failed = TRUE;
            break;
          }

          cfi->has_status = TRUE;
          ((ims_CallForwardInfoList_CallForwardInfo**)ims_data->info.arg)[i] = cfi;

          cfi->status = ril_data[i].status;

          cfi->has_reason = TRUE;
          cfi->reason = ril_data[i].reason;

          cfi->has_service_class = TRUE;
          cfi->service_class = ril_data[i].service_class;

          cfi->has_toa = TRUE;
          cfi->toa = ril_data[i].toa;

          if (ril_data[i].number)
          {
            cfi->number.arg = qmi_ril_util_str_clone(ril_data[i].number);
          }

          cfi->has_time_seconds = TRUE;
          cfi->time_seconds = ril_data[i].no_reply_timer;

          if (cfi->reason == QCRIL_QMI_VOICE_CCFC_REASON_UNCOND)
          {
            if (call_fwd_start_time)
            {
              cfi->has_callFwdTimerStart = TRUE;
              qcril_qmi_ims_translate_voice_time_type_to_ims_callfwdtimerinfo(call_fwd_start_time,
                      &cfi->callFwdTimerStart);
            }
            if (call_fwd_end_time)
            {
              cfi->has_callFwdTimerEnd = TRUE;
              qcril_qmi_ims_translate_voice_time_type_to_ims_callfwdtimerinfo(call_fwd_end_time,
                      &cfi->callFwdTimerEnd);
            }
          }
        }
      }
    } while (FALSE);
  }

  if (failed && ims_data)
  {
    qcril_qmi_npb_release(ims_CallForwardInfoList_fields, ims_data);
    memset(ims_data, 0, sizeof(ims_CallForwardInfoList));
  }
} // qcril_qmi_ims_translate_ril_callforwdinfo_to_ims_callforwdinfo

//===========================================================================
// qcril_qmi_ims_translate_ril_service_status_class_to_ims_callwaitinginfo
//===========================================================================
void qcril_qmi_ims_translate_ril_service_status_class_to_ims_callwaitinginfo(int service_status, int service_class, ims_CallWaitingInfo* ims_data)
{
  if (NULL == ims_data)
  {
    QCRIL_LOG_ERROR("ims_data is NULL");
  }
  else
  {
    ims_data->has_service_status = TRUE;
    ims_data->service_status = service_status;

    if (service_status)
    {
      ims_data->has_service_class = TRUE;
      ims_data->service_class.has_service_Class = TRUE;
      ims_data->service_class.service_Class = service_class;
    }
    else
    {
      ims_data->has_service_class = FALSE;
    }
  }
} // qcril_qmi_ims_translate_ril_service_status_class_to_ims_callwaitinginfo

//===========================================================================
// qcril_qmi_ims_map_qmi_ims_reg_state_to_ims_reg_state
//===========================================================================
ims_Registration_RegState qcril_qmi_ims_map_qmi_ims_reg_state_to_ims_reg_state(uint8_t ims_registered)
{
   ims_Registration_RegState ret;

   switch(ims_registered)
   {
   case IMSA_STATUS_NOT_REGISTERED_V01:
       ret = ims_Registration_RegState_NOT_REGISTERED;
       break;
   case IMSA_STATUS_REGISTERING_V01:
       ret = ims_Registration_RegState_REGISTERING;
       break;
   case IMSA_STATUS_REGISTERED_V01:
       ret = ims_Registration_RegState_REGISTERED;
       break;
   default:
       ret = ims_Registration_RegState_NOT_REGISTERED;
       break;
   }

   QCRIL_LOG_INFO("qmi ims_reg_state %d mapped to ims ims_reg_state %d", ims_registered, ret);

   return ret;
} // qcril_qmi_ims_map_qmi_ims_reg_state_to_ims_reg_state

//===========================================================================
// qcril_qmi_ims_translate_ril_suppsvcnotification_to_ims_suppsvcnotification
//===========================================================================
void qcril_qmi_ims_translate_ril_suppsvcnotification_to_ims_suppsvcnotification(const RIL_SuppSvcNotification* ril_data, ims_SuppSvcNotification* ims_data)
{
  if (NULL == ril_data || NULL == ims_data)
  {
    QCRIL_LOG_ERROR("ril_data or ims_data is NULL");
    if (NULL != ims_data)
    {
       QCRIL_LOG_INFO("ims_data is not NULL, set it to default value");
       ims_data->has_notificationType = FALSE;
       ims_data->has_code = FALSE;
       ims_data->has_index = FALSE;
       ims_data->has_type = FALSE;
       ims_data->number.arg = NULL;
    }
  }
  else
  {
    ims_data->has_notificationType = TRUE;
    ims_data->notificationType = ril_data->notificationType;

    ims_data->has_code = TRUE;
    ims_data->code = ril_data->code;

    ims_data->has_index = TRUE;
    ims_data->index = ril_data->index;

    ims_data->has_type = TRUE;
    ims_data->type = ril_data->type;

    ims_data->number.arg = (void*)(ril_data->number);
  }
}

//===========================================================================
// qcril_qmi_ims_map_call_mode_to_ims_radiotechtype
//===========================================================================
ims_RadioTechType qcril_qmi_ims_map_call_mode_to_ims_radiotechtype(call_mode_enum_v02 call_mode)
{
  ims_RadioTechType ims_rat;

  switch(call_mode)
  {
    case CALL_MODE_LTE_V02:
      ims_rat = ims_RadioTechType_RADIO_TECH_LTE;
      break;

    case CALL_MODE_WLAN_V02:
      ims_rat = ims_RadioTechType_RADIO_TECH_IWLAN;
      break;

    default:
      ims_rat = ims_RadioTechType_RADIO_TECH_UNKNOWN;
      break;
  }

  return ims_rat;
}

//===========================================================================
// qcril_qmi_ims_make_ims_info
//===========================================================================
ims_Info *qcril_qmi_ims_make_ims_info
(
 ims_CallType calltype,
 ims_StatusType status,
 uint32_t restrictcause,
 boolean networkmode_valid,
 ims_RadioTechType networkmode,
 boolean rtt_capabilities_info_valid,
 rtt_mode_type_v02 rtt_capabilities_info
)
{
  ims_Info *ims_info = qcril_malloc(sizeof(ims_Info));

  if (ims_info)
  {
    QCRIL_LOG_INFO("calltype = %d, status = %d, restrictcause = %d, "
                   "networkmode_valid = %d, networkmode = %d\n",
                   calltype, status, restrictcause, networkmode_valid, networkmode);

    ims_info->has_isValid = TRUE;
    ims_info->isValid = TRUE;

    ims_info->has_callType = TRUE;
    ims_info->callType = calltype;

    ims_info->has_status = TRUE;
    ims_info->status = status;

    ims_info->has_restrictCause = TRUE;
    ims_info->restrictCause = restrictcause;

    if (networkmode_valid)
    {
      ims_info->accTechStatus.arg = qcril_malloc((1+1) * sizeof(ims_StatusForAccessTech *));

      if (ims_info->accTechStatus.arg)
      {
        ims_StatusForAccessTech *acc_tech_status = qcril_malloc(sizeof(ims_StatusForAccessTech));
        if (acc_tech_status)
        {
          ((ims_StatusForAccessTech**)ims_info->accTechStatus.arg)[0] = acc_tech_status;
          acc_tech_status->has_networkMode = TRUE;
          acc_tech_status->networkMode = networkmode;
          acc_tech_status->has_status = TRUE;
          acc_tech_status->status = status;
          acc_tech_status->has_restrictionCause = TRUE;
          acc_tech_status->restrictionCause = restrictcause;
        }
      }
    }

    if ( rtt_capabilities_info_valid )
    {
      ims_info->has_rttMode = qcril_qmi_ims_map_qcril_rtt_mode_to_ims(rtt_capabilities_info,
                                                                      &ims_info->rttMode);
    }
  }
  return ims_info;
}

//===========================================================================
// qcril_qmi_ims_translate_ril_callcapabilities_to_ims_srvstatusinfo
//===========================================================================
void qcril_qmi_ims_translate_ril_callcapabilities_to_ims_srvstatusinfo
(
 const uint8_t ril_data_valid,
 const voice_ip_call_capabilities_info_type_v02* ril_data,
 const uint8 rtt_capabilities_info_valid,
 const rtt_mode_type_v02 rtt_capabilities_info,
 ims_SrvStatusList* ims_data,
 ims_CallType current_call_type,
 call_mode_enum_v02 call_mode
)
{
  ims_RadioTechType networkmode = qcril_qmi_ims_map_call_mode_to_ims_radiotechtype(call_mode);

  if ((!ril_data_valid && !rtt_capabilities_info_valid &&
        (networkmode == ims_RadioTechType_RADIO_TECH_UNKNOWN)) ||
      NULL == ims_data)
  {
    QCRIL_LOG_ERROR("Invalid parameters");
  }
  else
  {
    int n_srvstatusinfo    = 2; // 2 - one for voip capability and one for VT capability
    boolean call_type_found = FALSE;
    ims_CallType calltype;
    ims_StatusType status;
    if (current_call_type != ims_CallType_CALL_TYPE_UNKNOWN)
    {
      n_srvstatusinfo++;
    }
    n_srvstatusinfo++; // for NULL pointer
    ims_data->SrvStatusInfo.arg = qcril_malloc( (n_srvstatusinfo+1) * sizeof(ims_Info*));

    if(NULL != ims_data->SrvStatusInfo.arg)
    {
      ims_Info **ims_info_iter = (ims_Info **)ims_data->SrvStatusInfo.arg;
      if (ril_data_valid && ril_data)
      {
        if ( (VOICE_CALL_ATTRIB_TX_V02 == ril_data->audio_attrib) ||
             (VOICE_CALL_ATTRIB_RX_V02 == ril_data->audio_attrib) ||
             ((VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02) == ril_data->audio_attrib) )
        {
          calltype = ims_CallType_CALL_TYPE_VOICE;
          status = ims_StatusType_STATUS_ENABLED;
        }
        else
        {
          calltype = ims_CallType_CALL_TYPE_VOICE;
          status = ims_StatusType_STATUS_DISABLED;
        }
        *ims_info_iter = qcril_qmi_ims_make_ims_info(calltype, status, ril_data->audio_cause,
                                                     (current_call_type == calltype),
                                                     networkmode,
                                                     rtt_capabilities_info_valid,
                                                     rtt_capabilities_info);
        if ( current_call_type == calltype )
        {
          call_type_found = TRUE;
        }

        if ( NULL != *ims_info_iter )
        {
          QCRIL_LOG_INFO("calltype: %d status: %d restrictcause: %d",
                         (*ims_info_iter)->callType,
                         (*ims_info_iter)->status,
                         (*ims_info_iter)->restrictCause);
        }
        ims_info_iter++;

        if ( VOICE_CALL_ATTRIB_TX_V02 == ril_data->video_attrib )
        {
          calltype = ims_CallType_CALL_TYPE_VT_TX;
          status = ims_StatusType_STATUS_PARTIALLY_ENABLED;
        }
        else if ( VOICE_CALL_ATTRIB_RX_V02 == ril_data->video_attrib )
        {
          calltype = ims_CallType_CALL_TYPE_VT_RX;
          status = ims_StatusType_STATUS_PARTIALLY_ENABLED;
        }
        else if ( (VOICE_CALL_ATTRIB_TX_V02 | VOICE_CALL_ATTRIB_RX_V02) == ril_data->video_attrib )
        {
          calltype = ims_CallType_CALL_TYPE_VT;
          status = ims_StatusType_STATUS_ENABLED;
        }
        else
        {
          calltype = ims_CallType_CALL_TYPE_VT;
          status = ims_StatusType_STATUS_DISABLED;
        }
        *ims_info_iter = qcril_qmi_ims_make_ims_info(calltype, status, ril_data->video_cause,
                                                     (current_call_type == calltype),
                                                     networkmode,
                                                     rtt_capabilities_info_valid,
                                                     rtt_capabilities_info);
        if (current_call_type == calltype)
        {
          call_type_found = TRUE;
        }
        if (NULL != *ims_info_iter)
        {
          QCRIL_LOG_INFO("calltype: %d status: %d restrictcause: %d",
                         (*ims_info_iter)->callType,
                         (*ims_info_iter)->status,
                         (*ims_info_iter)->restrictCause);
        }
        ims_info_iter++;
      }
      if (current_call_type != ims_CallType_CALL_TYPE_UNKNOWN &&
          !call_type_found)
      {
        *ims_info_iter = qcril_qmi_ims_make_ims_info(current_call_type,
                                                     ims_StatusType_STATUS_ENABLED,
                                                     0,
                                                     TRUE,
                                                     networkmode,
                                                     rtt_capabilities_info_valid,
                                                     rtt_capabilities_info);
        if( NULL != *ims_info_iter )
        {
          QCRIL_LOG_INFO("calltype: %d status: %d restrictcause: %d",
                         (*ims_info_iter)->callType,
                         (*ims_info_iter)->status,
                         (*ims_info_iter)->restrictCause);
        }
        ims_info_iter++;
      }
    }
  }
}

//===========================================================================
// qcril_qmi_ims_create_ims_info
//===========================================================================
ims_Info* qcril_qmi_ims_create_ims_info(
    ims_CallType type,
    imsa_service_status_enum_v01 status,
    boolean rat_valid,
    imsa_service_rat_enum_v01 rat
)
{
    boolean failure = FALSE;
    ims_Info *ims_info_ptr = NULL;

    do
    {
        ims_info_ptr = qcril_malloc(sizeof(*ims_info_ptr));
        if (NULL == ims_info_ptr)
        {
            failure = TRUE;
            break;
        }
        ims_info_ptr->has_isValid = TRUE;
        ims_info_ptr->isValid = TRUE;

        ims_info_ptr->has_callType = TRUE;
        ims_info_ptr->callType = type;

        size_t num_of_acc_tech_status = 1; // only have one possible rat per current IMSA design
        ims_info_ptr->accTechStatus.arg =
            qcril_malloc( sizeof(ims_StatusForAccessTech*) * (num_of_acc_tech_status+1));
        if (NULL == ims_info_ptr->accTechStatus.arg)
        {
            failure = TRUE;
            break;
        }

        ((ims_StatusForAccessTech**)ims_info_ptr->accTechStatus.arg)[0] =
                qcril_malloc(sizeof(ims_StatusForAccessTech));
        if (NULL == ((ims_StatusForAccessTech**)ims_info_ptr->accTechStatus.arg)[0])
        {
            failure = TRUE;
            break;
        }
        ims_StatusForAccessTech *acc_tech_status =
                ((ims_StatusForAccessTech**)ims_info_ptr->accTechStatus.arg)[0];

        acc_tech_status->has_status = TRUE;
        acc_tech_status->has_registered = TRUE;
        acc_tech_status->registered.has_state = TRUE;
        switch (status)
        {
            case IMSA_NO_SERVICE_V01:
                acc_tech_status->status = ims_StatusType_STATUS_DISABLED;
                acc_tech_status->registered.state = ims_Registration_RegState_NOT_REGISTERED;
                break;
            case IMSA_LIMITED_SERVICE_V01:
                acc_tech_status->status = ims_StatusType_STATUS_PARTIALLY_ENABLED;
                acc_tech_status->registered.state = ims_Registration_RegState_REGISTERED;
                break;
            case IMSA_FULL_SERVICE_V01:
                acc_tech_status->status = ims_StatusType_STATUS_ENABLED;
                acc_tech_status->registered.state = ims_Registration_RegState_REGISTERED;
                break;
            default:
                QCRIL_LOG_DEBUG("no matched status");
                acc_tech_status->status = ims_StatusType_STATUS_ENABLED;
                acc_tech_status->registered.state = ims_Registration_RegState_REGISTERED;
        }

        if (rat_valid)
        {
            acc_tech_status->has_networkMode = qcril_qmi_ims_map_imsa_rat_to_ims_rat(
                rat, &acc_tech_status->networkMode);
        }
    } while (FALSE);

    if (failure)
    {
        qcril_qmi_ims_free_ims_info(ims_info_ptr);
        ims_info_ptr = NULL;
    }
    else
    {
        QCRIL_LOG_INFO( "calltype: %d %d, networkmode: %d, %d, "
                        "restrictioncause: %d, %d, status: %d, %d, reg state: %d, %d",
                ims_info_ptr->has_callType,
                ims_info_ptr->callType,
                ((ims_StatusForAccessTech**)ims_info_ptr->accTechStatus.arg)[0]->has_networkMode,
                ((ims_StatusForAccessTech**)ims_info_ptr->accTechStatus.arg)[0]->networkMode,
                ((ims_StatusForAccessTech**)
                ims_info_ptr->accTechStatus.arg)[0]->has_restrictionCause,
                ((ims_StatusForAccessTech**)ims_info_ptr->accTechStatus.arg)[0]->restrictionCause,
                ((ims_StatusForAccessTech**)ims_info_ptr->accTechStatus.arg)[0]->has_status,
                ((ims_StatusForAccessTech**)ims_info_ptr->accTechStatus.arg)[0]->status,
                ((ims_StatusForAccessTech**)
                ims_info_ptr->accTechStatus.arg)[0]->registered.has_state,
                ((ims_StatusForAccessTech**)
                ims_info_ptr->accTechStatus.arg)[0]->registered.state );
    }
    return ims_info_ptr;
} // qcril_qmi_ims_create_ims_info

//===========================================================================
// qcril_qmi_ims_create_ims_srvstatusinfo
//===========================================================================
ims_SrvStatusList* qcril_qmi_ims_create_ims_srvstatusinfo(const qcril_qmi_imsa_srv_status_type* qmi_data)
{
    ims_SrvStatusList* ims_srv_status_list_ptr = NULL;
    if (NULL == qmi_data)
    {
        QCRIL_LOG_DEBUG("qmi_data is NULL");
    }
    else
    {
        boolean failure = FALSE;
        do
        {
            ims_srv_status_list_ptr = qcril_malloc(sizeof(*ims_srv_status_list_ptr));

            if (NULL == ims_srv_status_list_ptr)
            {
                failure = TRUE;
                break;
            }

            uint32_t num_of_info = qmi_data->sms_service_status_valid +
                                   qmi_data->voip_service_status_valid +
                                   // we need to fill three types if vt status is valid
                                   qmi_data->vt_service_status_valid * 3 +
                                   qmi_data->ut_service_status_valid;

            if (num_of_info > 0)
            {
                ims_srv_status_list_ptr->SrvStatusInfo.arg = qcril_malloc( sizeof(ims_Info*) *
                        (num_of_info+1));
            }

            if (NULL == ims_srv_status_list_ptr->SrvStatusInfo.arg)
            {
                failure = TRUE;
                break;
            }

            ims_Info** ims_info_iter = (ims_Info**)ims_srv_status_list_ptr->SrvStatusInfo.arg;
            int idx = 0;
            if (qmi_data->sms_service_status_valid)
            {
                ims_info_iter[idx] = qcril_qmi_ims_create_ims_info(
                                                                  ims_CallType_CALL_TYPE_SMS,
                                                                  qmi_data->sms_service_status,
                                                                  qmi_data->sms_service_rat_valid,
                                                                  qmi_data->sms_service_rat );
                if (NULL == ims_info_iter[idx])
                {
                    failure = TRUE;
                    break;
                }
                idx++;
            }

            if (qmi_data->voip_service_status_valid)
            {
                ims_info_iter[idx] = qcril_qmi_ims_create_ims_info(
                                                                  ims_CallType_CALL_TYPE_VOICE,
                                                                  qmi_data->voip_service_status,
                                                                  qmi_data->voip_service_rat_valid,
                                                                  qmi_data->voip_service_rat );
                if (NULL == ims_info_iter[idx])
                {
                    failure = TRUE;
                    break;
                }
                idx++;
            }

            if (qmi_data->vt_service_status_valid)
            {
                ims_info_iter[idx] = qcril_qmi_ims_create_ims_info(
                                                                  ims_CallType_CALL_TYPE_VT,
                                                                  qmi_data->vt_service_status,
                                                                  qmi_data->vt_service_rat_valid,
                                                                  qmi_data->vt_service_rat );
                if (NULL == ims_info_iter[idx])
                {
                    failure = TRUE;
                    break;
                }
                idx++;

                ims_info_iter[idx] = qcril_qmi_ims_create_ims_info(
                                                                  ims_CallType_CALL_TYPE_VT_TX,
                                                                  qmi_data->vt_service_status,
                                                                  qmi_data->vt_service_rat_valid,
                                                                  qmi_data->vt_service_rat );
                if (NULL == ims_info_iter[idx])
                {
                    failure = TRUE;
                    break;
                }
                idx++;

                ims_info_iter[idx] = qcril_qmi_ims_create_ims_info(
                                                                  ims_CallType_CALL_TYPE_VT_RX,
                                                                  qmi_data->vt_service_status,
                                                                  qmi_data->vt_service_rat_valid,
                                                                  qmi_data->vt_service_rat );
                if (NULL == ims_info_iter[idx])
                {
                    failure = TRUE;
                    break;
                }
                idx++;
            }

            if (qmi_data->ut_service_status_valid)
            {
                ims_info_iter[idx] = qcril_qmi_ims_create_ims_info(
                                                                  ims_CallType_CALL_TYPE_UT,
                                                                  qmi_data->ut_service_status,
                                                                  qmi_data->ut_service_rat_valid,
                                                                  qmi_data->ut_service_rat );
                if (NULL == ims_info_iter[idx])
                {
                    failure = TRUE;
                    break;
                }
                idx++;
            }
        } while (FALSE);

        if (failure)
        {
            qcril_qmi_ims_free_srvstatuslist(ims_srv_status_list_ptr);
            ims_srv_status_list_ptr = NULL;
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return ims_srv_status_list_ptr;
} // qcril_qmi_ims_create_ims_srvstatusinfo

//===========================================================================
// qcril_qmi_ims_free_srvstatuslist
//===========================================================================
void qcril_qmi_ims_free_srvstatuslist(ims_SrvStatusList* ims_srv_status_list_ptr)
{
    if (NULL != ims_srv_status_list_ptr)
    {
        qcril_qmi_npb_release(ims_SrvStatusList_fields, ims_srv_status_list_ptr);
        qcril_free(ims_srv_status_list_ptr);
    }
    else
    {
        QCRIL_LOG_DEBUG("ims_srv_status_list_ptr is NULL")   ;
    }
} // qcril_qmi_ims_free_srvstatuslist

//===========================================================================
// qcril_qmi_ims_free_ims_info
//===========================================================================
void qcril_qmi_ims_free_ims_info(ims_Info* ims_info_ptr)
{
    if (NULL != ims_info_ptr)
    {
        qcril_qmi_npb_release(ims_Info_fields, ims_info_ptr);
        qcril_free(ims_info_ptr);
    }
    else
    {
        QCRIL_LOG_DEBUG("ims_info_ptr is NULL")   ;
    }
} // qcril_qmi_ims_free_ims_info

//===========================================================================
// qcril_qmi_ims_map_imsa_rat_to_ims_rat
//===========================================================================
boolean qcril_qmi_ims_map_imsa_rat_to_ims_rat
(
 imsa_service_rat_enum_v01 imsa_rat,
 ims_RadioTechType        *ims_rat
)
{
  boolean result = FALSE;

  if (ims_rat != NULL)
  {
    result = TRUE;
    switch (imsa_rat)
    {
      case IMSA_WLAN_V01:
        *ims_rat = ims_RadioTechType_RADIO_TECH_WIFI;
        break;
      case IMSA_IWLAN_V01:
        *ims_rat = ims_RadioTechType_RADIO_TECH_IWLAN;
        break;
      case IMSA_WWAN_V01:
        *ims_rat = ims_RadioTechType_RADIO_TECH_LTE;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
} // qcril_qmi_ims_map_imsa_rat_to_ims_rat

//===========================================================================
// qcril_qmi_ims_create_ims_handover_from_imsa_rat_info
//===========================================================================
ims_Handover* qcril_qmi_ims_create_ims_handover_from_imsa_rat_info(const imsa_rat_handover_status_info_v01* qmi_data)
{
    ims_Handover* ims_handover_ptr = NULL;
    if (NULL == qmi_data)
    {
        QCRIL_LOG_DEBUG("qmi_data is NULL");
    }
    else
    {
        boolean failure = FALSE;
        do
        {
            ims_handover_ptr = qcril_malloc(sizeof(*ims_handover_ptr));

            if (NULL == ims_handover_ptr)
            {
                failure = TRUE;
                break;
            }

            ims_handover_ptr->has_type = TRUE;
            switch(qmi_data->rat_ho_status)
            {
            case IMSA_STATUS_RAT_HO_SUCCESS_V01:
                ims_handover_ptr->type = ims_Handover_Msg_Type_COMPLETE_SUCCESS;
                break;
            case IMSA_STATUS_RAT_HO_FAILURE_V01:
                ims_handover_ptr->type = ims_Handover_Msg_Type_COMPLETE_FAIL;
                break;
            case IMSA_STATUS_RAT_HO_NOT_TRIGGERED_V01:
                ims_handover_ptr->type = ims_Handover_Msg_Type_NOT_TRIGGERED;
                break;
            case IMSA_STATUS_RAT_HO_NOT_TRIGGERED_MOBILE_DATA_OFF_V01:
                ims_handover_ptr->type = ims_Handover_Msg_Type_NOT_TRIGGERED_MOBILE_DATA_OFF;
                break;
            default:
                ims_handover_ptr->type = ims_Handover_Msg_Type_COMPLETE_FAIL;
                break;
            }

            ims_handover_ptr->has_srcTech = qcril_qmi_ims_map_imsa_rat_to_ims_rat(
                qmi_data->source_rat, &ims_handover_ptr->srcTech);

            ims_handover_ptr->has_targetTech = qcril_qmi_ims_map_imsa_rat_to_ims_rat(
                qmi_data->target_rat, &ims_handover_ptr->targetTech);

            if (strlen(qmi_data->cause_code))
            {
                /* Error is reported when the handover is NOT_TRIGGERED while the device is on active
                 * Wifi call and the wifi Rssi is nearing threshold roveout (-85dbm) and there is
                 * no qualified LTE network to handover to. Modem sends "CD-04:No Available qualified
                 * mobile network". Here it is decoded and sent as errorcode(CD-04) and errormessage
                 * to telephony.
                 */
                if( (qmi_data->rat_ho_status == IMSA_STATUS_RAT_HO_NOT_TRIGGERED_V01) &&
                    (strncmp(qmi_data->cause_code, WLAN_HANDOVER_NO_LTE_FAILURE_CODE_STRING,
                           strlen(WLAN_HANDOVER_NO_LTE_FAILURE_CODE_STRING)) == 0) )
                {
                    //Copy the code to errorcode string
                    ims_handover_ptr->errorCode.arg = qcril_malloc(
                            strlen(WLAN_HANDOVER_NO_LTE_FAILURE_CODE_STRING) + 1);
                    if(ims_handover_ptr->errorCode.arg != NULL)
                    {
                        strlcpy(ims_handover_ptr->errorCode.arg, WLAN_HANDOVER_NO_LTE_FAILURE_CODE_STRING,
                                strlen(WLAN_HANDOVER_NO_LTE_FAILURE_CODE_STRING) + 1);
                        QCRIL_LOG_DEBUG("handover error code: %s", ims_handover_ptr->errorCode.arg);
                    }

                    size_t errormessage_start = strlen(WLAN_HANDOVER_NO_LTE_FAILURE_CODE_STRING);

                    //Check for the delimeter ":" after the errorcode
                    // and discard any extra spaces
                    while(errormessage_start < strlen(qmi_data->cause_code))
                    {
                        if(strncmp(&(qmi_data->cause_code)[errormessage_start], ":", strlen(":")) == 0)
                        {
                            errormessage_start++;
                            while((strncmp(&(qmi_data->cause_code)[errormessage_start],
                                           " ",
                                           strlen(" ")) == 0) &&
                                  errormessage_start < strlen(qmi_data->cause_code))
                            {
                                errormessage_start++;
                            }
                            break;
                        }
                        errormessage_start++;
                    }
                    //The string after the delimeter ":" is the error message
                    if(errormessage_start < strlen(qmi_data->cause_code))
                    {
                        ims_handover_ptr->errorMessage.arg = qcril_malloc(
                                (strlen(qmi_data->cause_code) + 1) - errormessage_start);
                        if(ims_handover_ptr->errorMessage.arg)
                        {
                            strlcpy(ims_handover_ptr->errorMessage.arg,
                                    &(qmi_data->cause_code)[errormessage_start],
                                    ((strlen(qmi_data->cause_code) + 1)- errormessage_start));
                            QCRIL_LOG_DEBUG("handover error message: %s",
                                    ims_handover_ptr->errorMessage.arg);
                        }
                    }
                }
                else
                {
                    ims_handover_ptr->has_hoExtra = TRUE;

                    ims_handover_ptr->hoExtra.has_type = TRUE;
                    ims_handover_ptr->hoExtra.type = ims_Extra_Type_LTE_TO_IWLAN_HO_FAIL;

                    qcril_binary_data_type *bin_data = qcril_malloc(sizeof(*bin_data));
                    if (NULL == bin_data)
                    {
                        failure = TRUE;
                        break;
                    }
                    bin_data->data = qcril_malloc(strlen(qmi_data->cause_code));
                    bin_data->len = strlen(qmi_data->cause_code);
                    if(bin_data->data)
                    {
                        memcpy( bin_data->data,
                                qmi_data->cause_code,
                                bin_data->len );
                    }
                    ims_handover_ptr->hoExtra.extraInfo.arg = (void*)bin_data;
                }
            }
        } while (FALSE);

        if (failure)
        {
            qcril_qmi_ims_free_ims_handover(ims_handover_ptr);
            ims_handover_ptr = NULL;
        }
    }

    return ims_handover_ptr;
} // qcril_qmi_ims_create_ims_handover_from_imsa_rat_info
//===========================================================================
// qcril_qmi_ims_free_ims_handover
//===========================================================================
void qcril_qmi_ims_free_ims_handover(ims_Handover* ims_handover_ptr)
{
    if (NULL != ims_handover_ptr)
    {
        qcril_qmi_npb_release(ims_Handover_fields, ims_handover_ptr);
        qcril_free(ims_handover_ptr);
    }
    else
    {
        QCRIL_LOG_DEBUG("ims_handover_ptr is NULL");
    }
} // qcril_qmi_ims_free_ims_handover
//===========================================================================
// qcril_qmi_ims_free_ims_registration
//===========================================================================
void qcril_qmi_ims_free_ims_registration(ims_Registration* ims_reg_ptr)
{
    if(NULL != ims_reg_ptr)
    {
        qcril_qmi_npb_release(ims_Registration_fields, ims_reg_ptr);
        qcril_free(ims_reg_ptr);
    }
    else
    {
        QCRIL_LOG_DEBUG("ims_reg_ptr is NULL");
    }
}// qcril_qmi_ims_free_ims_registration
//===========================================================================
// qcril_qmi_ims_map_ril_failcause_to_ims_failcause
//===========================================================================
ims_CallFailCause qcril_qmi_ims_map_ril_failcause_to_ims_failcause(RIL_LastCallFailCause ril_failcause, int ims_extended_error_code)
{
  ims_CallFailCause ret = (ims_CallFailCause) ril_failcause;

  if( CALL_FAIL_ERROR_UNSPECIFIED == ril_failcause )
  {
    switch ( ims_extended_error_code )
    {
      case CALL_END_CAUSE_MULTIPLE_CHOICES_V02:
      case CALL_END_CAUSE_MOVED_PERMANENTLY_V02:
      case CALL_END_CAUSE_MOVED_TEMPORARILY_V02:
      case CALL_END_CAUSE_USE_PROXY_V02:
      case CALL_END_CAUSE_ALTERNATE_SERVICE_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_REDIRECTED;
        break;

      case CALL_END_CAUSE_BAD_REQ_WAIT_INVITE_V02:
      case CALL_END_CAUSE_BAD_REQ_WAIT_REINVITE_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_BAD_REQUEST;
        break;

      case CALL_END_CAUSE_SIP_403_FORBIDDEN_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_FORBIDDEN;
        break;

      case CALL_END_CAUSE_INVALID_REMOTE_URI_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_NOT_FOUND;
        break;

      case CALL_END_CAUSE_UNSUPPORTED_URI_SCHEME_V02:
      case CALL_END_CAUSE_REMOTE_UNSUPP_MEDIA_TYPE_V02:
      case CALL_END_CAUSE_BAD_EXTENSION_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_NOT_SUPPORTED;
        break;

      case CALL_END_CAUSE_NETWORK_NO_RESP_TIME_OUT_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_REQUEST_TIMEOUT;
        break;

      case CALL_END_CAUSE_PEER_NOT_REACHABLE_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_TEMPORARILY_UNAVAILABLE;
        break;

      case CALL_END_CAUSE_ADDRESS_INCOMPLETE_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_BAD_ADDRESS;
        break;

      case CALL_END_CAUSE_USER_BUSY_V02:
      case CALL_END_CAUSE_BUSY_EVERYWHERE_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_BUSY;
        break;

      case CALL_END_CAUSE_REQUEST_TERMINATED_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_REQUEST_CANCELLED;
        break;

      case CALL_END_CAUSE_NOT_ACCEPTABLE_V02:
      case CALL_END_CAUSE_NOT_ACCEPTABLE_HERE_V02:
      case CALL_END_CAUSE_SESS_DESCR_NOT_ACCEPTABLE_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_NOT_ACCEPTABLE;
        break;

      case CALL_END_CAUSE_GONE_V02:
      case CALL_END_CAUSE_DOES_NOT_EXIST_ANYWHERE_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_NOT_REACHABLE;
        break;

      case CALL_END_CAUSE_SERVER_INTERNAL_ERROR_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_SERVER_INTERNAL_ERROR;
        break;

      case CALL_END_CAUSE_NO_NETWORK_RESP_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_SERVICE_UNAVAILABLE;
        break;

      case CALL_END_CAUSE_SERVER_TIME_OUT_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_SERVER_TIMEOUT;
        break;

      case CALL_END_CAUSE_CALL_REJECTED_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_USER_REJECTED;
        break;

      case CALL_END_CAUSE_ANSWERED_ELSEWHERE_V02:
      case CALL_END_CAUSE_CALL_DEFLECTED_V02:
        ret = ims_CallFailCause_CALL_FAIL_ANSWERED_ELSEWHERE;
        break;

      case CALL_END_CAUSE_LOW_BATTERY_V02:
        ret = ims_CallFailCause_CALL_FAIL_LOW_BATTERY;
        break;

      case CALL_END_CAUSE_FALLBACK_TO_CS_V02:
        ret = ims_CallFailCause_CALL_FAIL_CS_RETRY_REQUIRED;
        break;

      case CALL_END_CAUSE_NOT_IMPLEMENTED_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_SERVER_NOT_IMPLEMENTED;
        break;

      case CALL_END_CAUSE_BAD_GATEWAY_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_SERVER_BAD_GATEWAY;
        break;

      case CALL_END_CAUSE_VERSION_NOT_SUPPORTED_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_SERVER_VERSION_UNSUPPORTED;
        break;

      case CALL_END_CAUSE_MESSAGE_TOO_LARGE_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_SERVER_MESSAGE_TOOLARGE;
        break;

      case CALL_END_CAUSE_PRECONDITION_FAILURE_V02:
        ret = ims_CallFailCause_CALL_FAIL_SIP_SERVER_PRECONDITION_FAILURE;
        break;

      case CALL_END_CAUSE_HO_NOT_FEASIBLE_V02:
        ret = ims_CallFailCause_CALL_FAIL_HO_NOT_FEASIBLE;
        break;

      case CALL_END_CAUSE_CALL_PULLED_V02:
        ret = ims_CallFailCause_CALL_FAIL_CAUSE_CALL_PULLED;
        break;

      case CALL_END_CAUSE_CALL_PULL_OUT_OF_SYNC_V02:
        ret = ims_CallFailCause_CALL_FAIL_PULL_OUT_OF_SYNC;
        break;

      case CALL_END_CAUSE_HOLD_RESUME_FAILED_V02:
        ret = ims_CallFailCause_CALL_FAIL_HOLD_RESUME_FAILED;
        break;

      case CALL_END_CAUSE_HOLD_RESUME_CANCELED_V02:
        ret = ims_CallFailCause_CALL_FAIL_HOLD_RESUME_CANCELED;
        break;

      case CALL_END_CAUSE_REINVITE_COLLISION_V02:
        ret = ims_CallFailCause_CALL_FAIL_HOLD_REINVITE_COLLISION;
        break;

      case CALL_END_CAUSE_SSAC_REJECT_V02:
        ret = ims_CallFailCause_CALL_FAIL_ACCESS_CLASS_BLOCKED;
        break;

      default:
        ret = ims_CallFailCause_CALL_FAIL_ERROR_UNSPECIFIED;
        break;
    }
  }

  QCRIL_LOG_INFO("RIL_LastCallFailCause %d with extended error code %d mapped to ims_CallFailCause %d", ril_failcause, ims_extended_error_code, ret);
  return ret;
}

//===========================================================================
// qcril_qmi_ims_map_qmi_call_state_to_ims_conf_call_state
//===========================================================================
boolean qcril_qmi_ims_map_qmi_call_state_to_ims_conf_call_state(call_state_enum_v02 qmi_state, ims_ConfCallState *ims_state_ptr)
{
    boolean success = TRUE;
    if (ims_state_ptr)
    {
        switch (qmi_state)
        {
        case CALL_STATE_ORIGINATING_V02:
        case CALL_STATE_CC_IN_PROGRESS_V02:
        case CALL_STATE_ALERTING_V02:
        case CALL_STATE_CONVERSATION_V02:
            *ims_state_ptr = ims_ConfCallState_FOREGROUND;
            break;

        case CALL_STATE_HOLD_V02:
            *ims_state_ptr = ims_ConfCallState_BACKGROUND;
            break;

        case CALL_STATE_INCOMING_V02:
        case CALL_STATE_WAITING_V02:
            *ims_state_ptr = ims_ConfCallState_RINGING;
            break;

        default:
            success = FALSE;
        }
    }
    else
    {
        success = FALSE;
    }
    return success;
}

/*===========================================================================

  FUNCTION:  qcril_qmi_voice_map_qmi_reason_to_ims_facility

===========================================================================*/
/*!
    @brief
    Maps reason code from QMI Voice Get Call Barring response message
    to corresponding IMS ims_SuppSvcFacilityType.

    @return
    Success: ims_SuppSvcFacilityType.
    Error:   0
*/
/*=========================================================================*/
ims_SuppSvcFacilityType qcril_qmi_voice_map_qmi_reason_to_ims_facility
(
  /* Reason code from QMI Voice Get Call Barring response message */
  voice_cc_sups_result_reason_enum_v02 reason
)
{
  switch (reason)
  {

    /* Bar All Outgoing Calls */
    case VOICE_CC_SUPS_RESULT_REASON_BARR_ALLOUTGOING_V02:
      return ims_SuppSvcFacilityType_FACILITY_BAOC;

    /* Bar All Outgoing International Calls */
    case VOICE_CC_SUPS_RESULT_REASON_BARR_OUTGOINGINT_V02:
      return ims_SuppSvcFacilityType_FACILITY_BAOIC;

    /* Bar all Outgoing International Calls except those
       directed to home PLMN country */
    case VOICE_CC_SUPS_RESULT_REASON_BARR_OUTGOINGINTEXTOHOME_V02:
      return ims_SuppSvcFacilityType_FACILITY_BAOICxH;

    /* Bar All Incoming Calls */
    case VOICE_CC_SUPS_RESULT_REASON_BARR_ALLINCOMING_V02:
      return ims_SuppSvcFacilityType_FACILITY_BAIC;

    /* Bar All Incoming Calls when Roaming outside
       the home PLMN country */
    case VOICE_CC_SUPS_RESULT_REASON_BARR_INCOMINGROAMING_V02:
      return ims_SuppSvcFacilityType_FACILITY_BAICr;

    /* Bar All incoming & outgoing Calls */
    case VOICE_CC_SUPS_RESULT_REASON_BARR_ALLBARRING_V02:
      return ims_SuppSvcFacilityType_FACILITY_BA_ALL;

    /* Bar All Outgoing Calls */
    case VOICE_CC_SUPS_RESULT_REASON_BARR_ALLOUTGOINGBARRING_V02:
      return ims_SuppSvcFacilityType_FACILITY_BA_MO;

    /* Bar All Incoming Calls */
    case VOICE_CC_SUPS_RESULT_REASON_BARR_ALLINCOMINGBARRING_V02:
      return ims_SuppSvcFacilityType_FACILITY_BA_MT;

    /* Bar Specific Incoming Calls */
    case VOICE_CC_SUPS_RESULT_REASON_BARR_INCOMING_NUMBER_V02:
      return ims_SuppSvcFacilityType_FACILITY_BS_MT;

    /* Bar Anonymous Incoming Calls */
    case VOICE_CC_SUPS_RESULT_REASON_BARR_INCOMING_ANONYMOUS_V02:
      return ims_SuppSvcFacilityType_FACILITY_BAICa;

    default:
      return 0;
  }
} /* qcril_qmi_voice_map_qmi_reason_to_ims_facility */

//===========================================================================
// qcril_qmi_ims_map_ril_call_substate_to_ims_call_substate
//===========================================================================
ims_CallSubstate qcril_qmi_ims_map_ril_call_substate_to_ims_call_substate
(
 RIL_Call_Sub_State  ril_call_substate
)
{
  ims_CallSubstate ims_call_substate = ims_CallSubstate_CALL_SUBSTATE_NONE;
  if (ril_call_substate ==
          (RIL_CALL_SUB_STATE_AUDIO_CONNECTED_SUSPENDED |
           RIL_CALL_SUB_STATE_VIDEO_CONNECTED_SUSPENDED))
  {
    ims_call_substate = (ims_CallSubstate_CALL_SUBSTATE_AUDIO_CONNECTED_SUSPENDED |
                         ims_CallSubstate_CALL_SUBSTATE_VIDEO_CONNECTED_SUSPENDED);
  }
  else
  {
    switch (ril_call_substate)
    {
      case RIL_CALL_SUB_STATE_AUDIO_CONNECTED_SUSPENDED:
        ims_call_substate = ims_CallSubstate_CALL_SUBSTATE_AUDIO_CONNECTED_SUSPENDED;
        break;
      case RIL_CALL_SUB_STATE_VIDEO_CONNECTED_SUSPENDED:
        ims_call_substate = ims_CallSubstate_CALL_SUBSTATE_VIDEO_CONNECTED_SUSPENDED;
        break;
      case RIL_CALL_SUB_STATE_AVP_RETRY:
        ims_call_substate = ims_CallSubstate_CALL_SUBSTATE_AVP_RETRY;
        break;
      case RIL_CALL_SUB_STATE_MEDIA_PAUSED:
        ims_call_substate = ims_CallSubstate_CALL_SUBSTATE_MEDIA_PAUSED;
        break;
      case RIL_CALL_SUB_STATE_UNDEFINED:
      default:
        ims_call_substate = ims_CallSubstate_CALL_SUBSTATE_NONE;
        break;
    }
  }
  return ims_call_substate;
} /* qcril_qmi_ims_map_ril_call_substate_to_ims_call_substate */

//============================================================================
// FUNCTION: qcril_qmi_sms_map_qmi_mwi_msg_type_to_ims_msg_type
//
// DESCRIPTION:
// Convert qmi wms mwi message type to ims mwi message type
//============================================================================
ims_MwiMessageType qcril_qmi_sms_map_qmi_mwi_msg_type_to_ims_msg_type
(
 transport_mwi_wms_message_type_enum_v01 mwi_wms_msg_type
)
{
  ims_MwiMessageType ims_mwi_msg_type;
  switch(mwi_wms_msg_type)
  {
    case TRANSPORT_MWI_MESSAGE_TYPE_VOICEMAIL_V01:
      ims_mwi_msg_type = ims_MwiMessageType_MWI_MSG_VOICE;
      break;

    case TRANSPORT_MWI_MESSAGE_TYPE_VIDEOMAIL_V01:
      ims_mwi_msg_type = ims_MwiMessageType_MWI_MSG_VIDEO;
      break;

    case TRANSPORT_MWI_MESSAGE_TYPE_FAX_V01:
      ims_mwi_msg_type = ims_MwiMessageType_MWI_MSG_FAX;
      break;

    case TRANSPORT_MWI_MESSAGE_TYPE_PAGER_V01:
      ims_mwi_msg_type = ims_MwiMessageType_MWI_MSG_PAGER;
      break;

    case TRANSPORT_MWI_MESSAGE_TYPE_MULTIMEDIA_V01:
      ims_mwi_msg_type = ims_MwiMessageType_MWI_MSG_MULTIMEDIA;
      break;

    case TRANSPORT_MWI_MESSAGE_TYPE_TEXT_V01:
      ims_mwi_msg_type = ims_MwiMessageType_MWI_MSG_TEXT;
      break;

    default:
      ims_mwi_msg_type = ims_MwiMessageType_MWI_MSG_NONE;
      break;
  }
  QCRIL_LOG_INFO("qmi mwi_wms_msg_type %d mapped to ims mwi_msg_type %d",
          mwi_wms_msg_type, ims_mwi_msg_type);
  return ims_mwi_msg_type;
}

//============================================================================
// FUNCTION: qcril_qmi_sms_map_qmi_mwi_priority_to_ims_priority
//
// DESCRIPTION:
// Convert qmi wms mwi priority to ims mwi priority
//============================================================================
ims_MwiPriority qcril_qmi_sms_map_qmi_mwi_priority_to_ims_priority
(
 wms_mwi_priority_type_enum_v01 wms_mwi_priority
)
{
  ims_MwiPriority mwi_priority;
  switch(wms_mwi_priority)
  {
    case WMS_LOW_PRIORITY_V01:
      mwi_priority = ims_MwiPriority_MWI_MSG_PRIORITY_LOW;
      break;

    case WMS_NORMAL_PRIORITY_V01:
      mwi_priority = ims_MwiPriority_MWI_MSG_PRIORITY_NORMAL;
      break;

    case WMS_URGENT_PRIORITY_V01:
      mwi_priority = ims_MwiPriority_MWI_MSG_PRIORITY_URGENT;
      break;

    case WMS_UNKNOWN_PRIORITY_V01:
    default:
      mwi_priority = ims_MwiPriority_MWI_MSG_PRIORITY_UNKNOWN;
      break;
  }
  QCRIL_LOG_INFO("qmi wms_mwi_priority %d mapped to ims mwi_priority %d",
          wms_mwi_priority, mwi_priority);
  return mwi_priority;
}

boolean qcril_qmi_ims_translate_ims_ttymodetype_to_qmi_tty_mode
(
 ims_Tty_Mode_Type mode,
 tty_mode_enum_v02 *tty_mode
)
{
  boolean result = FALSE;
  if (tty_mode)
  {
    result = TRUE;
    switch(mode)
    {
      case ims_Tty_Mode_Type_TTY_MODE_FULL:
        *tty_mode = TTY_MODE_FULL_V02;
        break;

      case ims_Tty_Mode_Type_TTY_MODE_VCO:
        *tty_mode = TTY_MODE_VCO_V02;
        break;

      case ims_Tty_Mode_Type_TTY_MODE_HCO:
        *tty_mode = TTY_MODE_HCO_V02;
        break;

      case ims_Tty_Mode_Type_TTY_MODE_OFF:
        *tty_mode = TTY_MODE_OFF_V02;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//============================================================================
// FUNCTION: qcril_qmi_ims_translate_ims_callfwdtimerinfo_to_voice_call_fwd_timer_type
//
// DESCRIPTION:
// Convert ims call fwd timer info to qmi voice call fwd timer
//============================================================================
boolean qcril_qmi_ims_translate_ims_callfwdtimerinfo_to_voice_time_type
(
 const ims_CallFwdTimerInfo *callfwdtimerinfo,
 voice_time_type_v02 *call_fwd_timer
)
{
  boolean result = FALSE;

  if (callfwdtimerinfo && call_fwd_timer)
  {
    if (callfwdtimerinfo->has_year)
    {
        call_fwd_timer->year = callfwdtimerinfo->year;
    }
    if (callfwdtimerinfo->has_month)
    {
        call_fwd_timer->month = callfwdtimerinfo->month;
    }
    if (callfwdtimerinfo->has_day)
    {
        call_fwd_timer->day = callfwdtimerinfo->day;
    }
    if (callfwdtimerinfo->has_hour)
    {
        call_fwd_timer->hour = callfwdtimerinfo->hour;
    }
    if (callfwdtimerinfo->has_minute)
    {
        call_fwd_timer->minute = callfwdtimerinfo->minute;
    }
    if (callfwdtimerinfo->has_second)
    {
        call_fwd_timer->second = callfwdtimerinfo->second;
    }
    if (callfwdtimerinfo->has_timezone)
    {
        call_fwd_timer->time_zone = callfwdtimerinfo->timezone;
    }
    result = TRUE;
  }

  return result;
}

boolean qcril_qmi_ims_translate_voice_time_type_to_ims_callfwdtimerinfo
(
 const voice_time_type_v02 *call_fwd_timer,
 ims_CallFwdTimerInfo *callfwdtimerinfo
)
{
  boolean result = FALSE;

  if (callfwdtimerinfo && call_fwd_timer)
  {
    callfwdtimerinfo->has_year = TRUE;
    callfwdtimerinfo->year     = call_fwd_timer->year;
    callfwdtimerinfo->has_month = TRUE;
    callfwdtimerinfo->month     = call_fwd_timer->month;
    callfwdtimerinfo->has_day = TRUE;
    callfwdtimerinfo->day     = call_fwd_timer->day;
    callfwdtimerinfo->has_hour = TRUE;
    callfwdtimerinfo->hour     = call_fwd_timer->hour;
    callfwdtimerinfo->has_minute = TRUE;
    callfwdtimerinfo->minute     = call_fwd_timer->minute;
    callfwdtimerinfo->has_second = TRUE;
    callfwdtimerinfo->second     = call_fwd_timer->second;
    callfwdtimerinfo->has_timezone = TRUE;
    callfwdtimerinfo->timezone     = call_fwd_timer->time_zone;
    result = TRUE;
  }

  return result;
}

//===========================================================================
// qcril_qmi_ims_map_ims_failcause_qmi_reject_cause
//===========================================================================
int32_t qcril_qmi_ims_map_ims_failcause_qmi_reject_cause
(
  ims_CallFailCause failcause
)
{
  int32_t res = INVALID_NEGATIVE_ONE;

  switch(failcause)
  {
    case ims_CallFailCause_CALL_FAIL_BLACKLISTED_CALL_ID:
      res = VOICE_REJECT_CAUSE_BLACKLISTED_CALL_ID_V02;
      break;

// Modem expects user busy, when the user rejects the call
// Reason for inverted mapping.

    case ims_CallFailCause_CALL_FAIL_USER_BUSY:
      res = VOICE_REJECT_CAUSE_USER_REJECT_V02;
      break;

    case ims_CallFailCause_CALL_FAIL_USER_REJECT:
      res = VOICE_REJECT_CAUSE_USER_BUSY_V02;
      break;

    case ims_CallFailCause_CALL_FAIL_LOW_BATTERY:
      res = VOICE_REJECT_CAUSE_LOW_BATTERY_V02;
      break;

    default:
      res = INVALID_NEGATIVE_ONE;
  }
  return res;
}

//===========================================================================
// qcril_qmi_ims_map_ims_failcause_qmi_reject_cause
//===========================================================================
ims_CauseCodes qcril_qmi_ims_map_qmi_call_modified_cause_to_ims_cause_code
(
  voice_call_modified_cause_enum_v02 call_modified_cause
)
{
  ims_CauseCodes cause_code = ims_CauseCodes_CAUSE_CODE_UNSPECIFIED;

  switch(call_modified_cause)
  {
    case VOICE_CALL_MODIFIED_CAUSE_UPGRADE_DUE_TO_LOCAL_REQ_V02:
      cause_code = ims_CauseCodes_CAUSE_CODE_SESSION_MODIFY_UPGRADE_LOCAL_REQ;
      break;

    case VOICE_CALL_MODIFIED_CAUSE_UPGRADE_DUE_TO_REMOTE_REQ_V02:
      cause_code = ims_CauseCodes_CAUSE_CODE_SESSION_MODIFY_UPGRADE_REMOTE_REQ;
      break;

    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ_V02:
      cause_code = ims_CauseCodes_CAUSE_CODE_SESSION_MODIFY_DOWNGRADE_LOCAL_REQ;
      break;

    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ_V02:
      cause_code = ims_CauseCodes_CAUSE_CODE_SESSION_MODIFY_DOWNGRADE_REMOTE_REQ;
      break;

    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT_V02:
      cause_code = ims_CauseCodes_CAUSE_CODE_SESSION_MODIFY_DOWNGRADE_RTP_TIMEOUT;
      break;

    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_QOS_V02:
      cause_code = ims_CauseCodes_CAUSE_CODE_SESSION_MODIFY_DOWNGRADE_QOS;
      break;

    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS_V02:
      cause_code = ims_CauseCodes_CAUSE_CODE_SESSION_MODIFY_DOWNGRADE_PACKET_LOSS;
      break;

    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT_V02:
      cause_code = ims_CauseCodes_CAUSE_CODE_SESSION_MODIFY_DOWNGRADE_LOW_THRPUT;
      break;

    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION_V02:
      cause_code = ims_CauseCodes_CAUSE_CODE_SESSION_MODIFY_DOWNGRADE_THERM_MITIGATION;
      break;

    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR_V02:
      cause_code = ims_CauseCodes_CAUSE_CODE_SESSION_MODIFY_DOWNGRADE_GENERIC_ERROR;
      break;

    case VOICE_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_LIPSYNC_V02:
      cause_code = ims_CauseCodes_CAUSE_CODE_SESSION_MODIFY_DOWNGRADE_LIPSYNC;
      break;

    case VOICE_CALL_MODIFIED_CAUSE_NONE_V02:
    default:
      cause_code = ims_CauseCodes_CAUSE_CODE_UNSPECIFIED;
      break;
  }

  QCRIL_LOG_INFO("qmi call_modified_cause %d mapped to ims cause_code %d",
          call_modified_cause, cause_code);

  return cause_code;
}

//===========================================================================
// qcril_qmi_ims_map_wificallingstatus_to_ims_settings_wfc_status
//===========================================================================
uint8_t qcril_qmi_ims_map_wificallingstatus_to_ims_settings_wfc_status
(
 ims_WifiCallingStatus             status,
 ims_settings_wfc_status_enum_v01 *wifi_call
)
{
  uint8_t result = FALSE;
  if (wifi_call)
  {
    result = TRUE;
    switch (status)
    {
      case ims_WifiCallingStatus_WIFI_NOT_SUPPORTED:
        *wifi_call = IMS_SETTINGS_WFC_STATUS_NOT_SUPPORTED_V01;
        break;
      case ims_WifiCallingStatus_WIFI_STATUS_ON:
        *wifi_call = IMS_SETTINGS_WFC_STATUS_ON_V01;
        break;
      case ims_WifiCallingStatus_WIFI_STATUS_OFF:
        *wifi_call = IMS_SETTINGS_WFC_STATUS_OFF_V01;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_ims_settings_wfc_status_to_wificallingstatus
//===========================================================================
boolean qcril_qmi_ims_map_ims_settings_wfc_status_to_wificallingstatus
(
 ims_settings_wfc_status_enum_v01 wifi_call,
 ims_WifiCallingStatus           *status
)
{
  boolean result = FALSE;
  if (status)
  {
    result = TRUE;
    switch (wifi_call)
    {
      case IMS_SETTINGS_WFC_STATUS_NOT_SUPPORTED_V01:
        *status = ims_WifiCallingStatus_WIFI_NOT_SUPPORTED;
        break;
      case IMS_SETTINGS_WFC_STATUS_ON_V01:
        *status = ims_WifiCallingStatus_WIFI_STATUS_ON;
        break;
      case IMS_SETTINGS_WFC_STATUS_OFF_V01:
        *status = ims_WifiCallingStatus_WIFI_STATUS_OFF;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_wificallingpreference_to_ims_settings_wfc_preference
//===========================================================================
uint8_t qcril_qmi_ims_map_wificallingpreference_to_ims_settings_wfc_preference
(
 ims_WifiCallingPreference        preference,
 ims_settings_wfc_preference_v01 *wifi_call_preference
)
{
  uint8_t result = FALSE;
  if (wifi_call_preference)
  {
    result = TRUE;
    switch (preference)
    {
      case ims_WifiCallingPreference_WIFI_PREF_NONE:
        *wifi_call_preference = IMS_SETTINGS_WFC_CALL_PREF_NONE_V01;
        break;
      case ims_WifiCallingPreference_WIFI_PREFERRED:
        *wifi_call_preference = IMS_SETTINGS_WFC_WLAN_PREFERRED_V01;
        break;
      case ims_WifiCallingPreference_WIFI_ONLY:
        *wifi_call_preference = IMS_SETTINGS_WFC_WLAN_ONLY_V01;
        break;
      case ims_WifiCallingPreference_CELLULAR_PREFERRED:
        *wifi_call_preference = IMS_SETTINGS_WFC_CELLULAR_PREFERRED_V01;
        break;
      case ims_WifiCallingPreference_CELLULAR_ONLY:
        *wifi_call_preference = IMS_SETTINGS_WFC_CELLULAR_ONLY_V01;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_ims_settings_wfc_preference_to_wificallingpreference
//===========================================================================
boolean qcril_qmi_ims_map_ims_settings_wfc_preference_to_wificallingpreference
(
 ims_settings_wfc_preference_v01 wifi_call_preference,
 ims_WifiCallingPreference      *preference
)
{
  boolean result = FALSE;
  if (preference)
  {
    result = TRUE;
    switch (wifi_call_preference)
    {
      case IMS_SETTINGS_WFC_CALL_PREF_NONE_V01:
        *preference = ims_WifiCallingPreference_WIFI_PREF_NONE;
        break;
      case IMS_SETTINGS_WFC_WLAN_PREFERRED_V01:
        *preference = ims_WifiCallingPreference_WIFI_PREFERRED;
        break;
      case IMS_SETTINGS_WFC_WLAN_ONLY_V01:
        *preference = ims_WifiCallingPreference_WIFI_ONLY;
        break;
      case IMS_SETTINGS_WFC_CELLULAR_PREFERRED_V01:
        *preference = ims_WifiCallingPreference_CELLULAR_PREFERRED;
        break;
      case IMS_SETTINGS_WFC_CELLULAR_ONLY_V01:
        *preference = ims_WifiCallingPreference_CELLULAR_ONLY;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_radio_config_settings_resp_to_ims_config_failcause
//===========================================================================
ims_ConfigFailureCause qcril_qmi_ims_map_radio_config_settings_resp_to_ims_config_failcause
(
  qcril_qmi_radio_config_settings_resp_type radio_config_error
)
{
  ims_ConfigFailureCause ret = ims_ConfigFailureCause_CONFIG_OTHER_INTERNAL_ERR;

  switch(radio_config_error)
  {
    case QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR:
      ret = ims_ConfigFailureCause_CONFIG_NO_ERR;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_IMS_NOT_READY:
      ret = ims_ConfigFailureCause_CONFIG_IMS_NOT_READY;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_FILE_NOT_AVAILABLE:
      ret = ims_ConfigFailureCause_CONFIG_FILE_NOT_AVAILABLE;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_READ_FAILED:
      ret = ims_ConfigFailureCause_CONFIG_READ_FAILED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_WRITE_FAILED:
      ret = ims_ConfigFailureCause_CONFIG_WRITE_FAILED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_OTHER_INTERNAL_ERR:
      ret = ims_ConfigFailureCause_CONFIG_OTHER_INTERNAL_ERR;
      break;

    default:
      ret = ims_ConfigFailureCause_CONFIG_OTHER_INTERNAL_ERR;
      break;
  }

  QCRIL_LOG_INFO("radio config error: %d mapped to ims config failcause %d",
                 radio_config_error, ret);

  return ret;
}//qcril_qmi_ims_map_radio_config_settings_resp_to_ims_config_failcause


//===========================================================================
// qcril_qmi_ims_map_ims_config_to_radio_config_item
//===========================================================================
qcril_qmi_radio_config_item_type qcril_qmi_ims_map_ims_config_to_radio_config_item
(
   ims_ConfigItem ims_config_item
)
{
  QCRIL_LOG_FUNC_ENTRY();
  qcril_qmi_radio_config_item_type ret = QCRIL_QMI_RADIO_CONFIG_INVALID;

  switch(ims_config_item)
  {
    case ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRMODESET:
      ret = QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_MODE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRWBMODESET:
      ret = QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_WB_MODE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_SESSION_TIMER:
      ret = QCRIL_QMI_RADIO_CONFIG_VOIP_SESSION_EXPIRY_TIMER;
      break;

    case ims_ConfigItem_CONFIG_ITEM_MIN_SESSION_EXPIRY:
      ret = QCRIL_QMI_RADIO_CONFIG_VOIP_MIN_SESSION_EXPIRY;
      break;

    case ims_ConfigItem_CONFIG_ITEM_CANCELLATION_TIMER:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_OPERATOR_MODE_A;
      break;

    case ims_ConfigItem_CONFIG_ITEM_T_DELAY:
      ret = QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_T_DELAY;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SILENT_REDIAL_ENABLE:
      ret = QCRIL_QMI_RADIO_CONFIG_VOIP_SILENT_REDIAL_ENABLED;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_T1_TIMER:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T1;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_T2_TIMER:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T2;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_TF_TIMER:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TF;
      break;

    case ims_ConfigItem_CONFIG_ITEM_VLT_SETTING_ENABLED:
      ret = QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOLTE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_LVC_SETTING_ENABLED:
      ret = QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VT;
      break;

    case ims_ConfigItem_CONFIG_ITEM_DOMAIN_NAME:
      ret = QCRIL_QMI_RADIO_CONFIG_USER_IMS_DOMAIN_NAME;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SMS_FORMAT:
      ret = QCRIL_QMI_RADIO_CONFIG_SMS_FORMAT;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SMS_OVER_IP:
      ret = QCRIL_QMI_RADIO_CONFIG_SMS_OVER_IP;
      break;

    case ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER:
      ret = QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXPIRY_TIMER;
      break;

    case ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER_EXTENDED:
      ret = QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXTENDED_EXPIRY_TIMER;
      break;

    case ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION:
      ret = QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITIES_CACHE_EXPIRATION;
      break;

    case ims_ConfigItem_CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION:
      ret = QCRIL_QMI_RADIO_CONFIG_PRESENCE_AVAILABILITY_CACHE_EXPIRATION;
      break;

    case ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL:
      ret = QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_INTERVAL;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH:
      ret = QCRIL_QMI_RADIO_CONFIG_PRESENCE_MINIMUM_PUBLISH_INTERVAL;
      break;

    case ims_ConfigItem_CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL:
      ret = QCRIL_QMI_RADIO_CONFIG_PRESENCE_MAXIMUM_SUBSCRIPTION_LIST_ENTRIES;
      break;

    case ims_ConfigItem_CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP:
      ret = QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_LIST_SUBSCRIPTION_EXPIRY_TIMER;
      break;

    case ims_ConfigItem_CONFIG_ITEM_GZIP_FLAG:
      ret = QCRIL_QMI_RADIO_CONFIG_PRESENCE_GZIP_ENABLED;
      break;

    case ims_ConfigItem_CONFIG_ITEM_EAB_SETTING_ENABLED:
      ret = QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_PRESENCE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_MOBILE_DATA_ENABLED:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_MOBILE_DATA_ENABLED;
      break;

    case ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED:
      ret = QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL;
      break;

    case ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING:
      ret = QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_ROAMING;
      break;

    case ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_MODE:
      ret = QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_PREFERENCE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TA_VALUE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TB_VALUE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TD_VALUE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TE_VALUE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TG_VALUE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TH_VALUE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TI_VALUE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TJ;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TK_VALUE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_KEEP_ALIVE_ENABLED:
      ret = QCRIL_QMI_RADIO_CONFIG_SIP_KEEPALIVE_ENABLED;
      break;

    case ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC:
      ret = QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_BASE_TIME;
      break;

    case ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC:
      ret = QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_MAX_TIME;
      break;

    case ims_ConfigItem_CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS:
      ret = QCRIL_QMI_RADIO_CONFIG_PRESENCE_VOLTE_USER_OPTED_IN_STATUS;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SMS_PSI:
      ret = QCRIL_QMI_RADIO_CONFIG_SMS_PSI;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SPEECH_START_PORT:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_START_PORT;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SPEECH_END_PORT:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_END_PORT;
      break;

    case ims_ConfigItem_CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_OCTET_ALIGNED_DYNAMIC_PT;
      break;

    case ims_ConfigItem_CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_BANDWIDTH_EFFICIENT_DYNAMIC_PT;
      break;

    case ims_ConfigItem_CONFIG_ITEM_AMR_OCTET_ALIGNED_PT:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_OCTET_ALIGNED_DYNAMIC_PT;
      break;

    case ims_ConfigItem_CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_BANDWIDTH_EFFICIENT_DYNAMIC_PT;
      break;

    case ims_ConfigItem_CONFIG_ITEM_DTMF_WB_PT:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_WB_DYNAMIC_POINT;
      break;

    case ims_ConfigItem_CONFIG_ITEM_DTMF_NB_PT:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_NB_DYNAMIC_PT;
      break;

    case ims_ConfigItem_CONFIG_ITEM_AMR_DEFAULT_MODE:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_DEFAULT_MODE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_LBO_PCSCF_ADDRESS:
      ret = QCRIL_QMI_RADIO_CONFIG_REG_MGR_CONFIG_REG_MGR_PRIMARY_CSCF;
      break;

    case ims_ConfigItem_CONFIG_ITEM_EMERGENCY_CALL_TIMER:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_EMERG_CALL_TIMER;
      break;

    case ims_ConfigItem_CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED:
      ret = QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_DISCOVERY_ENABLE;
      break;

    case ims_ConfigItem_CONFIG_ITEM_VIDEO_QUALITY:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_VIDEO_QUALITY;
      break;

    case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE1:
      ret = QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD1;
      break;

    case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE2:
      ret = QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD2;
      break;

    case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE3:
      ret = QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD3;
      break;

    case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_1x:
      ret = QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_1X_THRESHOLD;
      break;

    case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_A:
      ret = QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WIFI_THRESHOLDA;
      break;

    case ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_B:
      ret = QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WIFI_THRESHOLDB;
      break;

    case ims_ConfigItem_CONFIG_ITEM_T_EPDG_LTE:
      ret = QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WLAN_TO_WWAN_HYS_TIMER;
      break;

    case ims_ConfigItem_CONFIG_ITEM_T_EPDG_1x:
      ret = QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WLAN_TO_1X_HYS_TIMER;
      break;

    case ims_ConfigItem_CONFIG_ITEM_T_EPDG_WIFI:
      ret = QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WWAN_TO_WLAN_HYS_TIMER;
      break;

    case ims_ConfigItem_CONFIG_ITEM_VWF_SETTING_ENABLED:
      ret = QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOWIFI;
      break;

    case ims_ConfigItem_CONFIG_ITEM_VCE_SETTING_ENABLED:
      ret = QCRIL_QMI_RADIO_CONFIG_QIPCALL_VICE_ENABLED;
      break;

    case ims_ConfigItem_CONFIG_ITEM_RTT_SETTING_ENABLED:
      ret = QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_RTT;
      break;

    case ims_ConfigItem_CONFIG_ITEM_SMS_APP:
      ret = QCRIL_QMI_RADIO_CONFIG_RCS_SMS_APP;
      break;

    case ims_ConfigItem_CONFIG_ITEM_VVM_APP:
      ret = QCRIL_QMI_RADIO_CONFIG_RCS_VVM_APP;
      break;

    default:
      ret = QCRIL_QMI_RADIO_CONFIG_INVALID;
      break;

  }
  QCRIL_LOG_INFO("Mapped ims config: %d to radio config: %d", ims_config_item, ret);
  QCRIL_LOG_FUNC_RETURN();
  return ret;
} // qcril_qmi_ims_map_ims_config_to_radio_config_item

//===========================================================================
// qcril_qmi_ims_map_radio_config_to_ims_config_item
//===========================================================================
ims_ConfigItem qcril_qmi_ims_map_radio_config_to_ims_config_item
(
   qcril_qmi_radio_config_item_type radio_config_item
)
{
  QCRIL_LOG_FUNC_ENTRY();
  ims_ConfigItem ret = ims_ConfigItem_CONFIG_ITEM_NONE;

  switch(radio_config_item)
  {
    case QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_MODE:
      ret = ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRMODESET;
      break;

    case QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_WB_MODE:
      ret = ims_ConfigItem_CONFIG_ITEM_VOCODER_AMRWBMODESET;
      break;

    case QCRIL_QMI_RADIO_CONFIG_VOIP_SESSION_EXPIRY_TIMER:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_SESSION_TIMER;
      break;

    case QCRIL_QMI_RADIO_CONFIG_VOIP_MIN_SESSION_EXPIRY:
      ret = ims_ConfigItem_CONFIG_ITEM_MIN_SESSION_EXPIRY;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_OPERATOR_MODE_A:
      ret = ims_ConfigItem_CONFIG_ITEM_CANCELLATION_TIMER;
      break;

    case QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_T_DELAY:
      ret = ims_ConfigItem_CONFIG_ITEM_T_DELAY;
      break;

    case QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_BASE_TIME:
      ret = ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_BASE_TIME_SEC;
      break;

    case QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_MAX_TIME:
      ret = ims_ConfigItem_CONFIG_ITEM_REGISTRATION_RETRY_MAX_TIME_SEC;
      break;

    case QCRIL_QMI_RADIO_CONFIG_VOIP_SILENT_REDIAL_ENABLED:
      ret = ims_ConfigItem_CONFIG_ITEM_SILENT_REDIAL_ENABLE;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T1:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_T1_TIMER;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T2:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_T2_TIMER;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TF:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_TF_TIMER;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TA_VALUE:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_REQ_RETX_INTERVAL_MSEC;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TB_VALUE:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_WAIT_TIME_MSEC;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TD_VALUE:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_WAIT_TIME_MSEC;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TE_VALUE:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_INTERVAL_MSEC;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TG_VALUE:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_INVITE_RSP_RETX_INTERVAL_MSEC;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TH_VALUE:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RECEIPT_WAIT_TIME_MSEC;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TI_VALUE:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_ACK_RETX_WAIT_TIME_MSEC;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TJ:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_REQ_RETX_WAIT_TIME_MSEC;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TK_VALUE:
      ret = ims_ConfigItem_CONFIG_ITEM_SIP_NON_INVITE_RSP_RETX_WAIT_TIME_MSEC;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SIP_KEEPALIVE_ENABLED:
      ret = ims_ConfigItem_CONFIG_ITEM_KEEP_ALIVE_ENABLED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOLTE:
      ret = ims_ConfigItem_CONFIG_ITEM_VLT_SETTING_ENABLED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VT:
      ret = ims_ConfigItem_CONFIG_ITEM_LVC_SETTING_ENABLED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_USER_IMS_DOMAIN_NAME:
      ret = ims_ConfigItem_CONFIG_ITEM_DOMAIN_NAME;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SMS_FORMAT:
      ret = ims_ConfigItem_CONFIG_ITEM_SMS_FORMAT;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SMS_OVER_IP:
      ret = ims_ConfigItem_CONFIG_ITEM_SMS_OVER_IP;
      break;

    case QCRIL_QMI_RADIO_CONFIG_SMS_PSI:
      ret = ims_ConfigItem_CONFIG_ITEM_SMS_PSI;
      break;

    case QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXPIRY_TIMER:
      ret = ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER;
      break;

    case QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXTENDED_EXPIRY_TIMER:
      ret = ims_ConfigItem_CONFIG_ITEM_PUBLISH_TIMER_EXTENDED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITIES_CACHE_EXPIRATION:
      ret = ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_CACHE_EXPIRATION;
      break;

    case QCRIL_QMI_RADIO_CONFIG_PRESENCE_AVAILABILITY_CACHE_EXPIRATION:
      ret = ims_ConfigItem_CONFIG_ITEM_AVAILABILITY_CACHE_EXPIRATION;
      break;

    case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_INTERVAL:
      ret = ims_ConfigItem_CONFIG_ITEM_CAPABILITIES_POLL_INTERVAL;
      break;

    case QCRIL_QMI_RADIO_CONFIG_PRESENCE_MINIMUM_PUBLISH_INTERVAL:
      ret = ims_ConfigItem_CONFIG_ITEM_SOURCE_THROTTLE_PUBLISH;
      break;

    case QCRIL_QMI_RADIO_CONFIG_PRESENCE_MAXIMUM_SUBSCRIPTION_LIST_ENTRIES:
      ret = ims_ConfigItem_CONFIG_ITEM_MAX_NUM_ENTRIES_IN_RCL;
      break;

    case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_LIST_SUBSCRIPTION_EXPIRY_TIMER:
      ret = ims_ConfigItem_CONFIG_ITEM_CAPAB_POLL_LIST_SUB_EXP;
      break;

    case QCRIL_QMI_RADIO_CONFIG_PRESENCE_GZIP_ENABLED:
      ret = ims_ConfigItem_CONFIG_ITEM_GZIP_FLAG;
      break;

    case QCRIL_QMI_RADIO_CONFIG_PRESENCE_VOLTE_USER_OPTED_IN_STATUS:
      ret = ims_ConfigItem_CONFIG_ITEM_VOLTE_USER_OPT_IN_STATUS;
      break;

    case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_DISCOVERY_ENABLE:
      ret = ims_ConfigItem_CONFIG_ITEM_CAPABILITY_DISCOVERY_ENABLED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_PRESENCE:
      ret = ims_ConfigItem_CONFIG_ITEM_EAB_SETTING_ENABLED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_MOBILE_DATA_ENABLED:
      ret = ims_ConfigItem_CONFIG_ITEM_MOBILE_DATA_ENABLED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_START_PORT:
      ret = ims_ConfigItem_CONFIG_ITEM_SPEECH_START_PORT;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_END_PORT:
      ret = ims_ConfigItem_CONFIG_ITEM_SPEECH_END_PORT;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_OCTET_ALIGNED_DYNAMIC_PT:
      ret = ims_ConfigItem_CONFIG_ITEM_AMR_WB_OCTET_ALIGNED_PT;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_BANDWIDTH_EFFICIENT_DYNAMIC_PT:
      ret = ims_ConfigItem_CONFIG_ITEM_AMR_WB_BANDWIDTH_EFFICIENT_PT;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_OCTET_ALIGNED_DYNAMIC_PT:
      ret = ims_ConfigItem_CONFIG_ITEM_AMR_OCTET_ALIGNED_PT;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_BANDWIDTH_EFFICIENT_DYNAMIC_PT:
      ret = ims_ConfigItem_CONFIG_ITEM_AMR_BANDWIDTH_EFFICIENT_PT;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_WB_DYNAMIC_POINT:
      ret = ims_ConfigItem_CONFIG_ITEM_DTMF_WB_PT;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_NB_DYNAMIC_PT:
      ret = ims_ConfigItem_CONFIG_ITEM_DTMF_NB_PT;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_DEFAULT_MODE:
      ret = ims_ConfigItem_CONFIG_ITEM_AMR_DEFAULT_MODE;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_EMERG_CALL_TIMER:
      ret = ims_ConfigItem_CONFIG_ITEM_EMERGENCY_CALL_TIMER;
      break;

    case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL:
      ret = ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ENABLED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_ROAMING:
      ret = ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_ROAMING;
      break;

    case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_PREFERENCE:
      ret = ims_ConfigItem_CONFIG_ITEM_VOICE_OVER_WIFI_MODE;
      break;

    case QCRIL_QMI_RADIO_CONFIG_REG_MGR_CONFIG_REG_MGR_PRIMARY_CSCF:
      ret = ims_ConfigItem_CONFIG_ITEM_LBO_PCSCF_ADDRESS;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_VIDEO_QUALITY:
      ret = ims_ConfigItem_CONFIG_ITEM_VIDEO_QUALITY;
      break;

    case QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD1:
      ret = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE1;
      break;

    case QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD2:
      ret = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE2;
      break;

    case QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD3:
      ret = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_LTE3;
      break;

    case QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_1X_THRESHOLD:
      ret = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_1x;
      break;

    case QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WIFI_THRESHOLDA:
      ret = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_A;
      break;

    case QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WIFI_THRESHOLDB:
      ret = ims_ConfigItem_CONFIG_ITEM_THRESHOLD_WIFI_B;
      break;

    case QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WLAN_TO_WWAN_HYS_TIMER:
      ret = ims_ConfigItem_CONFIG_ITEM_T_EPDG_LTE;
      break;

    case QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WLAN_TO_1X_HYS_TIMER:
      ret = ims_ConfigItem_CONFIG_ITEM_T_EPDG_1x;
      break;

    case QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WWAN_TO_WLAN_HYS_TIMER:
      ret = ims_ConfigItem_CONFIG_ITEM_T_EPDG_WIFI;
      break;

    case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOWIFI:
      ret = ims_ConfigItem_CONFIG_ITEM_VWF_SETTING_ENABLED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_QIPCALL_VICE_ENABLED:
      ret = ims_ConfigItem_CONFIG_ITEM_VCE_SETTING_ENABLED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_RTT:
      ret = ims_ConfigItem_CONFIG_ITEM_RTT_SETTING_ENABLED;
      break;

    case QCRIL_QMI_RADIO_CONFIG_RCS_SMS_APP:
      ret = ims_ConfigItem_CONFIG_ITEM_SMS_APP;
      break;

    case QCRIL_QMI_RADIO_CONFIG_RCS_VVM_APP:
      ret = ims_ConfigItem_CONFIG_ITEM_VVM_APP;
      break;

    default:
      ret = ims_ConfigItem_CONFIG_ITEM_NONE;
      break;

  }
  QCRIL_LOG_INFO("Mapped radio config item: %d to ims config item: %d", radio_config_item, ret);
  QCRIL_LOG_FUNC_RETURN();
  return ret;
} // qcril_qmi_ims_map_radio_config_to_ims_config_item

//===========================================================================
// qcril_qmi_ims_map_wificallingpreference_to_dsd_rat_preference
//===========================================================================
boolean qcril_qmi_ims_map_wificallingpreference_to_dsd_rat_preference
(
 ims_WifiCallingPreference  preference,
 int                       *dsd_rat_preference
)
{
  boolean result = FALSE;
  if (dsd_rat_preference)
  {
    result = TRUE;
    switch (preference)
    {
      case ims_WifiCallingPreference_WIFI_PREF_NONE:
        *dsd_rat_preference = DSD_RAT_PREFERENCE_INACTIVE_V01;
        break;
      case ims_WifiCallingPreference_WIFI_PREFERRED:
        *dsd_rat_preference = DSD_RAT_PREFERENCE_WIFI_PREFERRED_V01;
        break;
      case ims_WifiCallingPreference_WIFI_ONLY:
        *dsd_rat_preference = DSD_RAT_PREFERENCE_WIFI_ONLY_V01;
        break;
      case ims_WifiCallingPreference_CELLULAR_PREFERRED:
        *dsd_rat_preference = DSD_RAT_PREFERENCE_CELLULAR_PREFERRED_V01;
        break;
      case ims_WifiCallingPreference_CELLULAR_ONLY:
        *dsd_rat_preference = DSD_RAT_PREFERENCE_CELLULAR_ONLY_V01;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_ims_settings_wfc_preference_to_dsd_rat_preference
//===========================================================================
boolean qcril_qmi_ims_map_ims_settings_wfc_preference_to_dsd_rat_preference
(
 qcril_ims_setting_wfc_preference_type wifi_call_preference,
 int                            *dsd_rat_preference
)
{
  boolean result = FALSE;
  if (dsd_rat_preference)
  {
    result = TRUE;
    switch (wifi_call_preference)
    {
      case QCRIL_IMS_SETTING_WFC_PREF_NONE:
        *dsd_rat_preference = DSD_RAT_PREFERENCE_INACTIVE_V01;
        break;
      case QCRIL_IMS_SETTING_WFC_WLAN_PREFERRED:
        *dsd_rat_preference = DSD_RAT_PREFERENCE_WIFI_PREFERRED_V01;
        break;
      case QCRIL_IMS_SETTING_WFC_WLAN_ONLY:
        *dsd_rat_preference = DSD_RAT_PREFERENCE_WIFI_ONLY_V01;
        break;
      case QCRIL_IMS_SETTING_WFC_CELLULAR_PREFERRED:
        *dsd_rat_preference = DSD_RAT_PREFERENCE_CELLULAR_PREFERRED_V01;
        break;
      case QCRIL_IMS_SETTING_WFC_CELLULAR_ONLY:
        *dsd_rat_preference = DSD_RAT_PREFERENCE_CELLULAR_ONLY_V01;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_ims_ect_type_to_qmi_ect_type
//===========================================================================
boolean qcril_qmi_ims_map_ims_ect_type_to_qmi_ect_type
(
 ims_EctType ims_ectType,
 voip_ect_type_enum_v02 *qmi_ectType
)
{
  boolean result = TRUE;

  switch (ims_ectType)
  {
    case ims_EctType_BLIND_TRANSFER:
      *qmi_ectType = ECT_TYPE_BLIND_TRANSFER_V02;
      break;

    case ims_EctType_ASSURED_TRANSFER:
      *qmi_ectType = ECT_TYPE_ASSURED_TRANSFER_V02;
      break;

    case ims_EctType_CONSULTATIVE_TRANSFER:
      *qmi_ectType = ECT_TYPE_CONSULTATIVE_TRANSFER_V02;
      break;

    default:
      result = FALSE;
      break;
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_qmi_wifi_quality_to_ims_wifi_quality
//===========================================================================
boolean qcril_qmi_ims_map_qmi_wifi_quality_to_ims_wifi_quality
(
 imsa_vowifiprofile_status_enum_v01 qmi_wifi_quality,
 int                                *ims_wifi_quality
)
{
  boolean result = FALSE;
  if (ims_wifi_quality)
  {
    result = TRUE;
    switch (qmi_wifi_quality)
    {
      case IMSA_SERVICE_VOWIFI_QUALITIY_EXCELLENT_V01:
        *ims_wifi_quality = ims_voWiFiQuality_VOWIFI_QUALITY_EXCELLENT;
        break;
      case IMSA_SERVICE_VOWIFI_QUALITIY_FAIR_V01:
        *ims_wifi_quality = ims_voWiFiQuality_VOWIFI_QUALITY_FAIR;
        break;
      case IMSA_SERVICE_VOWIFI_QUALITIY_BAD_V01:
        *ims_wifi_quality = ims_voWiFiQuality_VOWIFI_QUALITY_BAD;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_qmi_conf_parti_op_to_ims_conf_parti_op
//===========================================================================
boolean qcril_qmi_ims_map_qmi_conf_parti_op_to_ims_conf_parti_op
(
 conf_participant_operation_enum_v02 qmi_operation,
 ims_ConfParticipantOperation       *ims_operation
)
{
  boolean result = FALSE;
  if (ims_operation)
  {
    result = TRUE;
    switch (qmi_operation)
    {
      case VOICE_CONF_PARTICIPANT_ADD_V02:
        *ims_operation = ims_ConfParticipantOperation_ADD;
        break;
      case VOICE_CONF_PARTICIPANT_REMOVE_V02:
        *ims_operation = ims_ConfParticipantOperation_REMOVE;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  QCRIL_LOG_INFO("result = %s, mapped qmi_operation: %d to ims_operation: %d",
      result ? "TRUE" : "FALSE", qmi_operation,
      (result && ims_operation) ? *ims_operation : -1);
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_qmi_block_reason_to_ims_block_reason
//===========================================================================
ims_BlockReasonType qcril_qmi_ims_map_qmi_block_reason_to_ims_block_reason
(
 imsa_block_reason_type_mask_v01 qmi_block_reason
)
{
    switch (qmi_block_reason)
    {
      case IMSA_BLOCK_REASON_PDP_FAILURE_BIT_MASK_V01:
        return ims_BlockReasonType_BLOCK_REASON_PDP_FAILURE;
      case IMSA_BLOCK_REASON_REGISTRATION_FAILURE_BIT_MASK_V01:
        return ims_BlockReasonType_BLOCK_REASON_REGISTRATION_FAILURE;
      case IMSA_BLOCK_REASON_HANDOVER_FAILURE_BIT_MASK_V01:
        return ims_BlockReasonType_BLOCK_REASON_HANDOVER_FAILURE;
      case IMSA_BLOCK_REASON_OTHER_FAILURE_BIT_MASK_V01:
      default:
        return ims_BlockReasonType_BLOCK_REASON_OTHER_FAILURE;
    }
}

//===========================================================================
// qcril_qmi_ims_map_ims_volte_user_pref_to_qmi_nas_voice_domain_pref
//===========================================================================
boolean qcril_qmi_ims_map_ims_volte_user_pref_to_qmi_nas_voice_domain_pref
(
 ims_VoltePref                       ims_volte_user_pref,
 nas_voice_domain_pref_enum_type_v01 *qmi_voice_domain_pref
)
{
  boolean result = FALSE;
  if (qmi_voice_domain_pref)
  {
    result = TRUE;
    switch (ims_volte_user_pref)
    {
      case ims_VoltePref_VOLTE_PREF_DISABLED:
        *qmi_voice_domain_pref = NAS_VOICE_DOMAIN_PREF_CS_PREF_V01;
        break;
      case ims_VoltePref_VOLTE_PREF_ENABLED:
        *qmi_voice_domain_pref = NAS_VOICE_DOMAIN_PREF_PS_PREF_V01;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_qmi_nas_voice_domain_pref_to_ims_volte_user_pref
//===========================================================================
boolean qcril_qmi_ims_map_qmi_nas_voice_domain_pref_to_ims_volte_user_pref
(
 nas_voice_domain_pref_enum_type_v01 qmi_voice_domain_pref,
 ims_VoltePref                       *ims_volte_user_pref
)
{
  boolean result = FALSE;
  if (ims_volte_user_pref)
  {
    result = TRUE;
    switch (qmi_voice_domain_pref)
    {
      case NAS_VOICE_DOMAIN_PREF_CS_PREF_V01:
        *ims_volte_user_pref = ims_VoltePref_VOLTE_PREF_DISABLED;
        break;
      case NAS_VOICE_DOMAIN_PREF_PS_PREF_V01:
        *ims_volte_user_pref = ims_VoltePref_VOLTE_PREF_ENABLED;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_ims_settings_ho_config_to_ims_ho_config
//===========================================================================
boolean qcril_qmi_ims_map_ims_settings_ho_config_to_ims_ho_config
(
 ims_settings_handover_option_enum_v01 qmi_imss_ho_config,
 ims_HandoverConfig           *ims_ho_config
)
{
  boolean result = FALSE;
  if (ims_ho_config)
  {
    result = TRUE;
    switch (qmi_imss_ho_config)
    {
      case IMS_SETTINGS_HO_ENABLED_ALL_V01:
        *ims_ho_config = ims_HandoverConfig_HO_ENABLED_ALL;
        break;
      case IMS_SETTINGS_HO_DISABLED_ALL_V01:
        *ims_ho_config = ims_HandoverConfig_HO_DISABLED_ALL;
        break;
      case IMS_SETTINGS_HO_ENABLED_WLAN_TO_WWAN_ONLY_V01:
        *ims_ho_config = ims_HandoverConfig_HO_ENABLED_WLAN_TO_WWAN_ONLY;
        break;
      case IMS_SETTINGS_HO_ENABLED_WWAN_TO_WLAN_ONLY_V01:
        *ims_ho_config = ims_HandoverConfig_HO_ENABLED_WWAN_TO_WLAN_ONLY;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_map_ims_settings_ho_config_to_ims_ho_config
//===========================================================================
boolean qcril_qmi_ims_map_ims_ho_config_to_qmi_imss_ho_config
(
 ims_HandoverConfig           ims_ho_config,
 ims_settings_handover_option_enum_v01 *qmi_imss_ho_config
)
{
  boolean result = FALSE;
  if (qmi_imss_ho_config)
  {
    result = TRUE;
    switch (ims_ho_config)
    {
      case ims_HandoverConfig_HO_ENABLED_ALL:
        *qmi_imss_ho_config = IMS_SETTINGS_HO_ENABLED_ALL_V01;
        break;
      case ims_HandoverConfig_HO_DISABLED_ALL:
        *qmi_imss_ho_config = IMS_SETTINGS_HO_DISABLED_ALL_V01;
        break;
      case ims_HandoverConfig_HO_ENABLED_WLAN_TO_WWAN_ONLY:
        *qmi_imss_ho_config = IMS_SETTINGS_HO_ENABLED_WLAN_TO_WWAN_ONLY_V01;
        break;
      case ims_HandoverConfig_HO_ENABLED_WWAN_TO_WLAN_ONLY:
        *qmi_imss_ho_config = IMS_SETTINGS_HO_ENABLED_WWAN_TO_WLAN_ONLY_V01;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  return result;
}

//===========================================================================
// qcril_qmi_ims_is_msg_supported
//===========================================================================
boolean qcril_qmi_ims_is_msg_supported
(
  int message_id, uint8_t supported_msgs[], uint32_t supported_msgs_len
)
{
  uint32_t index = message_id/8;
  uint8_t bit_position = message_id - index * 8;
  uint8_t bit_position_mask = 0x01 << bit_position;

  if(index < supported_msgs_len)
  {
    return ((supported_msgs[index] & bit_position_mask) == bit_position_mask);
  }
  else
  {
    return FALSE;
  }
}

//===========================================================================
// qcril_qmi_call_end_type_to_ims_reg_failure
//===========================================================================
ims_RegFailureReasonType qcril_qmi_call_end_type_to_ims_reg_failure
(
  imsa_verbose_call_end_reason_type_enum_v01 qmi_block_reason
)
{
  ims_RegFailureReasonType result = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_UNSPECIFIED;
  switch (qmi_block_reason)
  {
    case IMSA_VCER_TYPE_UNSPECIFIED_V01:
      result = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_UNSPECIFIED;
      break;
    case IMSA_VCER_TYPE_MOBILE_IP_V01:
      result = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_MOBILE_IP;
      break;
    case IMSA_VCER_TYPE_INTERNAL_V01:
      result = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_INTERNAL;
      break;
    case IMSA_VCER_TYPE_CALL_MANAGER_DEFINED_V01:
      result = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_CALL_MANAGER_DEFINED;
      break;
    case IMSA_VCER_TYPE_3GPP_SPEC_DEFINED_V01:
      result = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_3GPP_SPEC_DEFINED;
      break;
    case IMSA_VCER_TYPE_PPP_V01:
      result = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_PPP;
      break;
    case IMSA_VCER_TYPE_EHRPD_V01:
      result = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_EHRPD;
      break;
    case IMSA_VCER_TYPE_IPV6_V01:
      result = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_IPV6;
      break;
    case IMSA_VCER_TYPE_IWLAN_V01:
      result = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_IWLAN;
      break;
    case IMSA_VCER_TYPE_HANDOFF_V01:
      result = ims_RegFailureReasonType_REG_FAILURE_REASON_TYPE_HANDOFF;
      break;
    default:
      break;
  }
  return result;
}
//===========================================================================
// qcril_qmi_ims_map_ims_rtt_mode_to_qcril
//===========================================================================
bool qcril_qmi_ims_map_ims_rtt_mode_to_qcril
(
  ims_RttMode rtt_mode_ims,
  rtt_mode_type_v02 *rtt_mode_qcril
)
{
  bool ret = FALSE;
  if (rtt_mode_qcril != NULL)
  {
    switch (rtt_mode_ims)
    {
      case ims_RttMode_RTT_MODE_FULL:
        *rtt_mode_qcril = RTT_MODE_FULL_V02;
        ret = TRUE;
      break;
      case ims_RttMode_RTT_MODE_DISABLED:
        *rtt_mode_qcril = RTT_MODE_DISABLED_V02;
        ret = TRUE;
      break;
      default:
        *rtt_mode_qcril = RTT_MODE_DISABLED_V02;
        ret = FALSE;
      break;
    }
  }
  return ret;
}

//===========================================================================
// qcril_qmi_ims_map_qcril_rtt_mode_to_ims
// ===========================================================================
bool qcril_qmi_ims_map_qcril_rtt_mode_to_ims
(
  rtt_mode_type_v02 rtt_mode_qcril,
  ims_RttMode* rtt_mode_ims
)
{
  bool ret = FALSE;
  if (rtt_mode_ims != NULL)
  {
    switch (rtt_mode_qcril)
    {
      case RTT_MODE_FULL_V02:
        *rtt_mode_ims = ims_RttMode_RTT_MODE_FULL;
        ret = TRUE;
        break;
      case RTT_MODE_DISABLED_V02:
        *rtt_mode_ims = ims_RttMode_RTT_MODE_DISABLED;
        ret = TRUE;
        break;
      default:
        *rtt_mode_ims = ims_RttMode_RTT_MODE_DISABLED;
        ret = FALSE;
        break;
    }
  }
  return ret;
}
