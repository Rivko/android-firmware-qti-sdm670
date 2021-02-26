/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_lte_direct_disc_msg_meta.c
  @brief   qcril qmi - lte direct discovery message meta info

  DESCRIPTION
    Handles lte direct discovery message meta info related functions.

******************************************************************************/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "qcril_qmi_lte_direct_disc_msg_meta.h"
#include <framework/Log.h>
#include <vector>

#define TAG "LTED_DISC"

typedef struct
{
  LteDirectDiscovery_MsgId msg_id;
  LteDirectDiscovery_MsgType msg_type;
  const pb_field_t* msg_pb_fields;
  size_t msg_req_size;
  qcril_evt_e_type msg_event;
  const char* msg_log_str;
} qcril_qmi_lte_direct_disc_msg_meta_type;

static std::vector<qcril_qmi_lte_direct_disc_msg_meta_type> qcril_qmi_lte_direct_disc_msg_meta_data;

void qcril_qmi_lted_init_msg_meta()
{
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_INITIALIZE,LteDirectDiscovery_MsgType_REQUEST,NULL,0,QCRIL_EVT_LTE_DIRECT_DISC_REQ_INITIALIZE,"INITIALIZE REQ"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_INITIALIZE,LteDirectDiscovery_MsgType_RESPONSE,NULL,0,QCRIL_EVT_NONE,"INITIALIZE RESP"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_GET_DEVICE_CAPABILITY,LteDirectDiscovery_MsgType_REQUEST,NULL,0,QCRIL_EVT_LTE_DIRECT_DISC_REQ_GET_DEVICE_CAPABILITY,"GET_DEVICE_CAPABILITY REQ"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_GET_DEVICE_CAPABILITY,LteDirectDiscovery_MsgType_RESPONSE,LteDirectDiscovery_DeviceCapability_fields,sizeof(LteDirectDiscovery_DeviceCapability),QCRIL_EVT_NONE,"GET_DEVICE_CAPABILITY RESP"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_TERMINATE,LteDirectDiscovery_MsgType_REQUEST,LteDirectDiscovery_Terminate_fields,sizeof(LteDirectDiscovery_Terminate),QCRIL_EVT_LTE_DIRECT_DISC_REQ_TERMINATE,"TERMINATE REQ"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_TERMINATE,LteDirectDiscovery_MsgType_RESPONSE,NULL,0,QCRIL_EVT_NONE,"TERMINATE RESP"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_GET_SERVICE_STATUS,LteDirectDiscovery_MsgType_REQUEST,NULL,0,QCRIL_EVT_LTE_DIRECT_DISC_REQ_GET_SERVICE_STATUS,"GET_SERVICE_STATUS REQ"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_GET_SERVICE_STATUS,LteDirectDiscovery_MsgType_RESPONSE,LteDirectDiscovery_ServiceStatus_fields,sizeof(LteDirectDiscovery_ServiceStatus),QCRIL_EVT_NONE,"GET_SERVICE_STATUS RESP"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_PUBLISH,LteDirectDiscovery_MsgType_REQUEST,LteDirectDiscovery_Publish_fields,sizeof(LteDirectDiscovery_Publish),QCRIL_EVT_LTE_DIRECT_DISC_REQ_PUBLISH,"PUBLISH REQ"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_PUBLISH,LteDirectDiscovery_MsgType_RESPONSE,NULL,0,QCRIL_EVT_NONE,"PUBLISH RESP"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_CANCEL_PUBLISH,LteDirectDiscovery_MsgType_REQUEST,LteDirectDiscovery_CancelPublish_fields,sizeof(LteDirectDiscovery_CancelPublish),QCRIL_EVT_LTE_DIRECT_DISC_REQ_CANCEL_PUBLISH,"CANCEL_PUBLISH REQ"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_CANCEL_PUBLISH,LteDirectDiscovery_MsgType_RESPONSE,NULL,0,QCRIL_EVT_NONE,"CANCEL_PUBLISH RESP"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_SUBSCRIBE,LteDirectDiscovery_MsgType_REQUEST,LteDirectDiscovery_Subscribe_fields,sizeof(LteDirectDiscovery_Subscribe),QCRIL_EVT_LTE_DIRECT_DISC_REQ_SUBSCRIBE,"SUBSCRIBE REQ"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_SUBSCRIBE,LteDirectDiscovery_MsgType_RESPONSE,NULL,0,QCRIL_EVT_NONE,"SUBSCRIBE RESP"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_CANCEL_SUBSCRIBE,LteDirectDiscovery_MsgType_REQUEST,LteDirectDiscovery_CancelSubscribe_fields,sizeof(LteDirectDiscovery_CancelSubscribe),QCRIL_EVT_LTE_DIRECT_DISC_REQ_CANCEL_SUBSCRIBE,"CANCEL_SUBSCRIBE REQ"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_REQUEST_CANCEL_SUBSCRIBE,LteDirectDiscovery_MsgType_RESPONSE,NULL,0,QCRIL_EVT_NONE,"CANCEL_SUBSCRIBE RESP"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_UNSOL_RESPONSE_DEVICE_CAPABILITY_CHANGED,LteDirectDiscovery_MsgType_UNSOL_RESPONSE,LteDirectDiscovery_DeviceCapability_fields,sizeof(LteDirectDiscovery_DeviceCapability),QCRIL_EVT_NONE,"UNSOL_RESPONSE_DEVICE_CAPABILITY_CHANGED"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_UNSOL_RESPONSE_SERVICE_STATUS,LteDirectDiscovery_MsgType_UNSOL_RESPONSE,LteDirectDiscovery_ServiceStatus_fields,sizeof(LteDirectDiscovery_ServiceStatus),QCRIL_EVT_NONE,"UNSOL_RESPONSE_SERVICE_STATUS"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_UNSOL_RESPONSE_AUTHORIZATION_RESULT,LteDirectDiscovery_MsgType_UNSOL_RESPONSE,LteDirectDiscovery_AuthorizationResult_fields,sizeof(LteDirectDiscovery_AuthorizationResult),QCRIL_EVT_NONE,"UNSOL_RESPONSE_AUTHORIZATION_RESULT"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_UNSOL_RESPONSE_EXPRESSION_STATUS,LteDirectDiscovery_MsgType_UNSOL_RESPONSE,LteDirectDiscovery_ExpressionStatus_fields,sizeof(LteDirectDiscovery_ExpressionStatus),QCRIL_EVT_NONE,"UNSOL_RESPONSE_EXPRESSION_STATUS"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_UNSOL_RESPONSE_TRANSMISSION_STATUS,LteDirectDiscovery_MsgType_UNSOL_RESPONSE,LteDirectDiscovery_TransmissionStatus_fields,sizeof(LteDirectDiscovery_TransmissionStatus),QCRIL_EVT_NONE,"UNSOL_RESPONSE_TRANSMISSION_STATUS"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_UNSOL_RESPONSE_RECEPTION_STATUS,LteDirectDiscovery_MsgType_UNSOL_RESPONSE,LteDirectDiscovery_ReceptionStatus_fields,sizeof(LteDirectDiscovery_ReceptionStatus),QCRIL_EVT_NONE,"UNSOL_RESPONSE_RECEPTION_STATUS"});
  qcril_qmi_lte_direct_disc_msg_meta_data.push_back({LteDirectDiscovery_MsgId_UNSOL_RESPONSE_MATCH_EVENT,LteDirectDiscovery_MsgType_UNSOL_RESPONSE,LteDirectDiscovery_MatchEvent_fields,sizeof(LteDirectDiscovery_MatchEvent),QCRIL_EVT_NONE,"UNSOL_RESPONSE_MATCH_EVENT"});
}

size_t qcril_qmi_lte_direct_disc_get_msg_size(LteDirectDiscovery_MsgId msg_id, LteDirectDiscovery_MsgType msg_type)
{
  size_t i;
  for (i=0; i<qcril_qmi_lte_direct_disc_msg_meta_data.size(); i++)
  {
    if ( qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_id == msg_id &&
        qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_type == msg_type )
    {
      return qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_req_size;
    }
  }
  return 0;
}

const pb_field_t* qcril_qmi_lte_direct_disc_get_msg_pb_fields(LteDirectDiscovery_MsgId msg_id, LteDirectDiscovery_MsgType msg_type)
{
  size_t i;
  for (i=0; i<qcril_qmi_lte_direct_disc_msg_meta_data.size(); i++)
  {
    if ( qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_id == msg_id &&
        qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_type == msg_type )
    {
      return qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_pb_fields;
    }
  }
  return NULL;
}

const char* qcril_qmi_lte_direct_disc_get_msg_log_str(LteDirectDiscovery_MsgId msg_id, LteDirectDiscovery_MsgType msg_type)
{
  size_t i;
  for (i=0; i<qcril_qmi_lte_direct_disc_msg_meta_data.size(); i++)
  {
    if ( qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_id == msg_id &&
        qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_type == msg_type )
    {
      if (qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_log_str)
      {
        return qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_log_str;
      }
      else
      {
        break;
      }
    }
  }
  return "Unknown msg";
}

qcril_evt_e_type qcril_qmi_lte_direct_disc_get_msg_event(LteDirectDiscovery_MsgId msg_id, LteDirectDiscovery_MsgType msg_type)
{
  size_t i;
  for (i=0; i<qcril_qmi_lte_direct_disc_msg_meta_data.size(); i++)
  {
    if (qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_id == msg_id)
    {
      return qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_event;
    }
  }
  QCRIL_LOG_ERROR("did not find the corresponding event for %s(msg id %d, type %d)", qcril_qmi_lte_direct_disc_get_msg_log_str(msg_id, msg_type), msg_id, msg_type);
  return QCRIL_EVT_NONE;
}

boolean qcril_qmi_lte_direct_disc_is_msg_valid(LteDirectDiscovery_MsgId msg_id, LteDirectDiscovery_MsgType msg_type)
{
  size_t i;
  for (i=0; i<qcril_qmi_lte_direct_disc_msg_meta_data.size(); i++)
  {
    if (qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_id == msg_id  &&
        qcril_qmi_lte_direct_disc_msg_meta_data[i].msg_type == msg_type)
    {
      return TRUE;
    }
  }
  QCRIL_LOG_ERROR("not a valid msg event for %s(msg id %d, type %d)", qcril_qmi_lte_direct_disc_get_msg_log_str(msg_id, msg_type), msg_id, msg_type);
  return FALSE;
}
