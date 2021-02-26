/******************************************************************************
#  Copyright (c) 2012, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_oemhook_agent.cc
  @brief   qcril qmi - oem socket

  DESCRIPTION
    Handles oem socket related functions

******************************************************************************/

#define __STDC_FORMAT_MACROS 1

#include "qcril_qmi_oemhook_service.h"

#include <pthread.h>
#include <sys/un.h>

#include <framework/legacy.h>
#include <framework/qcril_event.h>

#include "qcril_qmi_generic_socket.h"
#include "qcril_qmi_oemhook_agent.h"
#include "qcril_qmi_oem_reqlist.h"
#include "qcril_qmi_oem_events.h"

#ifdef QMI_RIL_UTF
extern "C" uint32 qcril_get_time_milliseconds();
#include <unistd.h>
#endif

#define TAG "OEM_SOCKET"
#include "qcrili.h"
#include <arpa/inet.h>
#include "framework/Log.h"


#include "qcril_reqlist.h"
#include "qcril_qmi_oem_misc.h"
#include "qcril_qmi_oem_msg_meta.h"
#include "qcril_qmi_oem_packing.h"

extern "C" RIL_Token qcril_qmi_oem_convert_oem_token_to_ril_token(uint32_t oem_token);
extern "C" uint32_t qcril_qmi_oem_free_and_convert_ril_token_to_oem_token(RIL_Token ril_token);

#if defined(FEATURE_TARGET_GLIBC_x86) || defined(QMI_RIL_UTF)
   extern "C" size_t strlcat(char *, const char *, size_t);
   extern "C" size_t strlcpy(char *, const char *, size_t);
#endif

using namespace vendor::qti::hardware::radio::qcrilhook::V1_0::implementation;

/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

/* Length of the integer values used in OEM messages */
#define QCRIL_QMI_OEM_INT_VAL_LEN               4

pthread_mutex_t      oem_lock_mutex;
/* Determines if oemhook service is registered */
uint8_t oemhookServiceRegistered = 0;

/* Oemhook agent class */
class OemhookAgent
{
public:
    static  OemhookAgent* getOemhookAgent();
    boolean sendOemhookMessageResponse(qcril_instance_id_e_type oem_socket_instance_id, int token,
                                       RIL_Errno error, const void* msg, size_t msg_len, int message_id);
    boolean sendOemhookMessageUnsol(qcril_instance_id_e_type oem_socket_instance_id, const void* msg, size_t msg_len);
    void processOemhookMessageRequest(qcril_instance_id_e_type oem_socket_instance_id, int serial,
                                      unsigned char *data, size_t data_len);

private: // data member
    static OemhookAgent* sInstance;

};

/* Initialization of static instance variable */
OemhookAgent* OemhookAgent::sInstance = NULL;

/*===========================================================================
  FUNCTION  OemhookAgent::getOemhookAgent
===========================================================================*/
/*!
    @brief get oem socket agent instance
*/
/*=========================================================================*/
OemhookAgent* OemhookAgent::getOemhookAgent
(
  void
)
{
  QCRIL_LOG_FUNC_ENTRY();
  pthread_mutex_lock (&oem_lock_mutex);
  if (!sInstance)
  {
    sInstance = new OemhookAgent();
    QCRIL_LOG_INFO("OemhookAgent instance initialized");
  }
  pthread_mutex_unlock (&oem_lock_mutex);

  QCRIL_LOG_FUNC_RETURN();
  return sInstance;

} // OemhookAgent::getOemhookAgent

/* Utility method to send oemhook indication/unsolicited
 * message to telephony through oemhook HIDL service
 */
boolean OemhookAgent::sendOemhookMessageUnsol
(
  qcril_instance_id_e_type oem_socket_instance_id,
  const void* msg,
  size_t      msg_len
)
{
    boolean        ret = true;
    size_t         final_len =  (size_t) QCRIL_QMI_OEM_INT_VAL_LEN + msg_len;
    int            unsol_hook_raw = RIL_UNSOL_OEM_HOOK_RAW;
    //buffer ownership goes to HIDL (in OemHookImpl) and is freed when destructor is called
    unsigned char* send_buffer = new unsigned char[final_len];
    unsigned char* tmp_msg = send_buffer;

    QCRIL_LOG_FUNC_ENTRY();
    if (send_buffer)
    {
        /* Unsolicited oemhook message has following message format
           [RIL_UNSOL_OEM_HOOK_RAW (4)]
           [OEM_NAME(8)] [Message Id (4) ] [Payload Length (4) ] [Payload]
         */
        memcpy( tmp_msg, &unsol_hook_raw, QCRIL_QMI_OEM_INT_VAL_LEN);
        tmp_msg += QCRIL_QMI_OEM_INT_VAL_LEN;
        if (msg_len && msg)
        {
          memcpy( tmp_msg, msg, msg_len);
        }
        qcril_qmi_print_hex(send_buffer, final_len);
        // Send indication to oemhook HIDL service
        sendOemhookIndication(oem_socket_instance_id, send_buffer, final_len);
    }
    else
    {
      QCRIL_LOG_ERROR("could not allocate memory");
      ret = false;
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET((int) ret);
    return ret;
} //sendOemhookMessageUnsol

/* Utility method to send oemhook solicited message
 * response to telephony through oemhook HIDL service
 */
boolean OemhookAgent::sendOemhookMessageResponse
(
  qcril_instance_id_e_type oem_socket_instance_id,
  int         token,
  RIL_Errno   error,
  const void* msg,
  size_t      msg_len,
  int         message_id
)
{
    boolean ret = true;
    //buffer ownership goes to HIDL (in OemHookImpl) and is freed when destructor is called
    unsigned char* send_buffer = new unsigned char[msg_len];

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("send message .....final len %zu, error: %d, messageID:%d", msg_len, error, message_id);
    if (send_buffer)
    {
      /* Response message has following message format
         [Message id(4) ] [Payload Length (4) ] [Payload]
         */
      if (msg_len && msg)
      {
        memcpy(send_buffer, msg, msg_len);
      }
      qcril_qmi_print_hex(send_buffer, msg_len);
      // Send response to oemhook HIDL service
      sendOemhookResponse(oem_socket_instance_id, token, error, send_buffer, msg_len);
    }
    else
    {
      QCRIL_LOG_ERROR("could not allocate memory");
      ret = false;
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET((int) ret);
    return ret;
} // sendOemhookMessageResponse

/*
 * Utility method to process the oemHookRawRequest coming
 * from oem hook service
 */
void OemhookAgent::processOemhookMessageRequest
(
    qcril_instance_id_e_type oem_socket_instance_id,
    int serial,
    unsigned char *data,
    size_t data_len
)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_request_params_type param;
    qmi_ril_oem_hook_request_details_type    oem_hook_req_details;
    qcril_request_resp_params_type           resp;
    RIL_Token                                token;
    qcril_event_id_t*                        entry_ptr = NULL;
    RIL_Errno                                audit_result = RIL_E_GENERIC_FAILURE;

    QCRIL_LOG_INFO(" .....received %zu bytes with serial=%d", data_len, serial);
    memset(&param, 0, sizeof(param));
    param.instance_id = oem_socket_instance_id;

    do {

      /* Oemhook Request has the following message format
         [OEM_NAME(8)] [Request Id (4)]
         [Payload Length (4)] [Payload]
       */
      if (data_len < (QCRIL_OTHER_OEM_NAME_LENGTH + QCRIL_QMI_OEM_INT_VAL_LEN))
      {
        // The request is not supported
        audit_result = RIL_E_REQUEST_NOT_SUPPORTED;
        break;
      }

      qcril_qmi_print_hex(data, data_len);
      token = qcril_qmi_oem_convert_oem_token_to_ril_token(serial/*request token*/);

      param.t           = (void *)token;
      param.modem_id    = QCRIL_DEFAULT_MODEM_ID;

      // Check OEM identifier and request ID
      if (!qmi_ril_get_req_details_from_oem_req(&oem_hook_req_details,
                                               &audit_result,
                                                data,
                                               &param,
                                                data_len))
      {
        break;
      }

      // oem hook qmi idl tunneling
      if ( oem_hook_req_details.is_qmi_tunneling )
      {
        if (!qmi_ril_parse_oem_req_tunnelled_message(&oem_hook_req_details,
                                                     &audit_result,
                                                     &param))
        {
          break;
        }
      }

      QCRIL_LOG_INFO("..... tunneling req %d, qmi_proto req %d", oem_hook_req_details.is_qmi_tunneling, oem_hook_req_details.is_qmi_proto);
      // oem hook proto
      if ( oem_hook_req_details.is_qmi_proto )
      {
        QCRIL_LOG_INFO("Received proto request : %s (%d)",
                qcril_log_lookup_event_name(oem_hook_req_details.hook_req_event),
                oem_hook_req_details.hook_req);

        switch (oem_hook_req_details.hook_req)
        {
          case QCRIL_REQ_HOOK_PROTOBUF_MSG:
            qmi_ril_parse_oem_req_proto_message(&oem_hook_req_details,
                                                     &audit_result,
                                                     &param);
            break;
          case QCRIL_REQ_HOOK_LTE_DIRECT_DISC_REQ:
            qmi_ril_parse_oem_req_lte_direct_disc(&oem_hook_req_details,
                                                     &audit_result,
                                                     &param);
            break;

          default:
            QCRIL_LOG_ERROR( "Unknown request received %d", oem_hook_req_details.hook_req);
            break;
        }
      }
    } while (0);

    if ( RIL_E_SUCCESS == audit_result && !oem_hook_req_details.is_qmi_proto)
    {
      if ( qcril_hash_table_lookup( param.event_id, &entry_ptr ) != E_SUCCESS || NULL == entry_ptr )
      {
        // The request is not supported
        audit_result = RIL_E_REQUEST_NOT_SUPPORTED;
      }
      else
      {
        if ( qcril_dispatch_event( entry_ptr, &param ) == E_NOT_ALLOWED )
        {
          audit_result = RIL_E_RADIO_NOT_AVAILABLE;
        }
      }

      if( oem_hook_req_details.is_qmi_tunneling && param.data )
      {
        qcril_free(param.data);
      }
    }

    if ( RIL_E_SUCCESS != audit_result )
    {
      qcril_default_request_resp_params( param.instance_id, param.t, param.event_id, audit_result, &resp );
      resp.request_id_android = RIL_REQUEST_OEM_HOOK_RAW;
      QCRIL_LOG_ERROR("Send error response serial=%d", serial);
      qcril_send_request_response( &resp );
    }
    QCRIL_LOG_FUNC_RETURN();
    return;
} // process_incoming_oemhook_message

void processIncomingOemhookMessage
(
    qcril_instance_id_e_type oem_socket_instance_id,
    int serial,
    unsigned char *data,
    size_t data_len
)
{
    QCRIL_LOG_FUNC_ENTRY();
    /*TODO: Remove this in phase 2: qcril_qmi_oem_socket_agent::get_oem_socket_agent()->
      send_message_unsol(OEM__MSG_TYPE__UNSOL_RESPONSE, msg, msg_len);
     */
    if (oemhookServiceRegistered) {
        OemhookAgent::getOemhookAgent()->processOemhookMessageRequest(oem_socket_instance_id, serial, data, data_len);
    } else {
        QCRIL_LOG_ERROR("processIncomingOemhookMessage: oemhook service not registered");
    }
    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================
  FUNCTION  qcril_qmi_oemhook_agent_init
===========================================================================*/
/*!
    @brief Initialize and start listening on oem socket
*/
/*=========================================================================*/
void qcril_qmi_oemhook_agent_init
(
  void
)
{
    QCRIL_LOG_FUNC_ENTRY();
    if (!oemhookServiceRegistered) {
        // Register oem hook service
        qcril_instance_id_e_type oemHookInstanceId = qmi_ril_get_process_instance_id();
        oemhookServiceRegistered = oemHookRegisterService(oemHookInstanceId);
        pthread_mutex_init( &oem_lock_mutex, NULL );

        qcril_qmi_oem_init_msg_meta();
        qcril_qmi_oem_events_init();
    }
    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_oemhook_agent_init

/*===========================================================================
  FUNCTION  qcril_qmi_oemhook_agent_send_unsol
===========================================================================*/
/*!
    @brief Send unsolicited response message
*/
/*=========================================================================*/
void qcril_qmi_oemhook_agent_send_unsol
(
  qcril_instance_id_e_type oem_socket_instance_id,
  const void *msg,
  size_t msg_len
)
{
    QCRIL_LOG_FUNC_ENTRY();
    /*TODO: Remove this in phase 2: qcril_qmi_oem_socket_agent::get_oem_socket_agent()->
      send_message_unsol(OEM__MSG_TYPE__UNSOL_RESPONSE, msg, msg_len);
     */
    if (oemhookServiceRegistered) {
        OemhookAgent::getOemhookAgent()->sendOemhookMessageUnsol(oem_socket_instance_id, msg, msg_len);
    } else {
        QCRIL_LOG_ERROR("qcril_qmi_oemhook_agent_send_unsol: oemhook service not registered");
    }
    QCRIL_LOG_FUNC_RETURN();

} // qcril_qmi_oemhook_agent_send_unsol

/*===========================================================================
  FUNCTION  qcril_qmi_oemhook_agent_send
===========================================================================*/
/*!
    @brief Send response message
*/
/*=========================================================================*/
void qcril_qmi_oemhook_agent_send
(
  qcril_instance_id_e_type oem_socket_instance_id,
  RIL_Token   token,
  int         message_id,
  RIL_Errno   error,
  const void  *msg,
  size_t      msg_len
)
{
    int req_token;
    QCRIL_LOG_FUNC_ENTRY();

    if (message_id >= 0) {
    qcril_reqlist_free( oem_socket_instance_id, token );
    req_token = qcril_qmi_oem_free_and_convert_ril_token_to_oem_token(token);
    /*TODO: Remove this in phase 2qcril_qmi_oem_socket_agent::get_oem_socket_agent()
      ->send_message(req_token,OEM__MSG_TYPE__RESPONSE,message_id,error,msg,msg_len);
    */
    if (oemhookServiceRegistered) {
        OemhookAgent::getOemhookAgent()->sendOemhookMessageResponse(oem_socket_instance_id, req_token, error, msg, msg_len, message_id);
    } else {
        QCRIL_LOG_ERROR("qcril_qmi_oemhook_agent_send: oemhook service not registered");
    }
    }

    QCRIL_LOG_FUNC_RETURN();

} // qcril_qmi_oemhook_agent_send

/*===========================================================================
  FUNCTION  qcril_qmi_oem_socket_proto_send
===========================================================================*/
/*!
    @brief Send response for proto message
*/
/*=========================================================================*/
void qcril_qmi_oem_socket_proto_send
(
  RIL_Token proto_token,
  QtiTelephonyService_RILOEMMessageId message_id,
  QtiTelephonyService_RILOEMMessageType type,
  boolean has_error,
  QtiTelephonyService_RILOEMError oem_proto_error,
  const void* msg,
  int msg_len
)
{
    RIL_Token original_ril_token;
    uint32_t original_oem_token;
    int is_qmi_proto_response = FALSE;
    RIL_Errno   ril_error;
    qmi_ril_oem_hook_response_context_type * oem_hook_response_ctx = NULL;
    qmi_ril_oem_hook_response_context_type * prev_iter = NULL;
    size_t packed_msg_len;

    static uint8_t send_buffer[8 * 1024];
    QCRIL_NOTUSED(msg_len);

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_DEBUG("Proto token = %d", proto_token);

    pthread_mutex_lock( &qmi_ril_oem_hook_overview.overview_lock_mutex );
    oem_hook_response_ctx = qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root;

    memset(send_buffer, 0, sizeof(send_buffer));

    while ( NULL != oem_hook_response_ctx && !is_qmi_proto_response )
    {
        QCRIL_LOG_DEBUG("proto token = %d, original token = %d", oem_hook_response_ctx->proto_token, oem_hook_response_ctx->original_token);
        if ( proto_token == oem_hook_response_ctx->proto_token )
        {  // match
            is_qmi_proto_response = TRUE;
            original_ril_token = oem_hook_response_ctx->original_token;
            if ( NULL != prev_iter )
            {
                prev_iter->next = oem_hook_response_ctx->next;
            }
            else
            { // top
                qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root = oem_hook_response_ctx->next;
            }
        }
        else
        {
            prev_iter = oem_hook_response_ctx;
            oem_hook_response_ctx = oem_hook_response_ctx->next;
        }
    }

    pthread_mutex_unlock( &qmi_ril_oem_hook_overview.overview_lock_mutex );

    if ( oem_hook_response_ctx )
    {
        qcril_free( oem_hook_response_ctx );
    }

    if(!is_qmi_proto_response)
    {
        QCRIL_LOG_DEBUG("Not a proto request response %d", proto_token);
        return;
    }

    qcril_reqlist_free( QCRIL_DEFAULT_INSTANCE_ID , original_ril_token );
    original_oem_token = qcril_qmi_oem_free_and_convert_ril_token_to_oem_token(original_ril_token);
    ril_error = qcril_qmi_oem_proto_map_oem_error_to_ril_error(oem_proto_error);

    packed_msg_len = qcril_qmi_oem_proto_pack_msg_and_tag(msg,
                                  qcril_qmi_oem_free_and_convert_ril_token_to_proto_token(proto_token),
                                  type,
                                  message_id,
                                  has_error,
                                  oem_proto_error,
                                  send_buffer,
                                  sizeof(send_buffer));

    if (oemhookServiceRegistered) {
        OemhookAgent::getOemhookAgent()->sendOemhookMessageResponse(QCRIL_DEFAULT_INSTANCE_ID,
            original_oem_token, ril_error,
            send_buffer, packed_msg_len, QCRIL_REQ_HOOK_PROTOBUF_MSG);
    } else {
        QCRIL_LOG_ERROR("qcril_qmi_oemhook_agent_send: oemhook service not registered");
    }

    QCRIL_LOG_FUNC_RETURN();
}

//=========================================================================
// qmi_ril_oem_hook_get_request_id
//===========================================================================
RIL_Errno qmi_ril_oem_hook_get_request_id
(
  uint16        service_id,
  uint16        message_id,
  uint32        *request_id,
  const char    **log_ind
)
{
  RIL_Errno  result = RIL_E_SUCCESS;
  const char * msg_returned = NULL;

  if( service_id == QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_VT )
  {
     switch (message_id )
     {
        case  IMS_VT_DIAL_CALL_REQ_V01:
            *request_id  = QCRIL_REQ_HOOK_VT_DIAL_CALL;
            msg_returned = "QCRIL_EVT_HOOK_VT_DIAL_CALL";
            break;

        case  IMS_VT_END_CALL_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_VT_END_CALL;
            msg_returned = "QCRIL_EVT_HOOK_VT_END_CALL";
            break;

        case  IMS_VT_ANSWER_CALL_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_VT_ANSWER_CALL;
            msg_returned = "QCRIL_EVT_HOOK_VT_ANSWER_CALL";
            break;

        case  IMS_VT_GET_CALL_INFO_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_VT_GET_CALL_INFO;
            msg_returned = "QCRIL_EVT_HOOK_VT_GET_CALL_INFO";
            break;

        default:
            QCRIL_LOG_ERROR("invalid messsage-id = %d received",  message_id);
            result = RIL_E_INVALID_ARGUMENTS;
            break;
     }
  }
  else if ( service_id == QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_PRESENCE )
  {
     switch (message_id )
     {
        case  QMI_IMSP_GET_ENABLER_STATE_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_IMS_ENABLER_STATE_REQ;
            msg_returned = "QCRIL_EVT_HOOK_IMS_ENABLER_STATE_REQ";
            break;

        case  QMI_IMSP_SEND_PUBLISH_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_IMS_SEND_PUBLISH_REQ;
            msg_returned = "QCRIL_EVT_HOOK_IMS_SEND_PUBLISH_REQ";
            break;

        case QMI_IMSP_SEND_PUBLISH_XML_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_IMS_SEND_PUBLISH_XML_REQ;
            msg_returned = "QCRIL_EVT_HOOK_IMS_SEND_PUBLISH_XML_REQ";
            break;

        case  QMI_IMSP_SEND_UNPUBLISH_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_IMS_SEND_UNPUBLISH_REQ;
            msg_returned = "QCRIL_EVT_HOOK_IMS_SEND_UNPUBLISH_REQ";
            break;

        case  QMI_IMSP_SEND_SUBSCRIBE_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_IMS_SEND_SUBSCRIBE_REQ;
            msg_returned = "QCRIL_EVT_HOOK_IMS_SEND_SUBSCRIBE_REQ";
            break;

        case  QMI_IMSP_SEND_SUBSCRIBE_XML_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_IMS_SEND_SUBSCRIBE_XML_REQ;
            msg_returned = "QCRIL_EVT_HOOK_IMS_SEND_SUBSCRIBE_XML_REQ";
            break;

        case  QMI_IMSP_SEND_UNSUBSCRIBE_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_IMS_SEND_UNSUBSCRIBE_REQ;
            msg_returned = "QCRIL_EVT_HOOK_IMS_SEND_UNSUBSCRIBE_REQ";
            break;

        case  QMI_IMSP_SET_NOTIFY_FMT_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_IMS_SET_NOTIFY_FMT_REQ_V01;
            msg_returned = "QCRIL_EVT_HOOK_IMS_SET_NOTIFY_FMT_REQ_V01";
            break;

        case  QMI_IMSP_GET_NOTIFY_FMT_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_IMS_GET_NOTIFY_FMT_REQ_V01;
            msg_returned = "QCRIL_EVT_HOOK_IMS_GET_NOTIFY_FMT_REQ_V01";
            break;

        case  QMI_IMSP_SET_EVENT_REPORT_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_IMS_SET_EVENT_REPORT_REQ_V01;
            msg_returned = "QCRIL_EVT_HOOK_IMS_SET_EVENT_REPORT_REQ_V01";
            break;

        case  QMI_IMSP_GET_EVENT_REPORT_REQ_V01:
            *request_id =  QCRIL_REQ_HOOK_IMS_GET_EVENT_REPORT_REQ_V01;
            msg_returned = "QCRIL_EVT_HOOK_IMS_GET_EVENT_REPORT_REQ_V01";
            break;

        default:
            QCRIL_LOG_ERROR("invalid messsage-id = %d received",  message_id);
            result = RIL_E_INVALID_ARGUMENTS;
            break;
      }
  }
  else if( service_id == QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_EMBMS )
  {
    if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_EMBMS))
    {
      switch (message_id )
      {
         case  QMI_EMBMS_ENABLE_EMBMS_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_ENABLE;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_ENABLE";
             break;

         case  QMI_EMBMS_DISABLE_EMBMS_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_DISABLE;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_DISABLE";
             break;

         case  QMI_EMBMS_ACTIVATE_TMGI_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_ACTIVATE_TMGI;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_ACTIVATE_TMGI";
             break;

         case  QMI_EMBMS_DEACTIVATE_TMGI_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_DEACTIVATE_TMGI;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_DEACTIVATE_TMGI";
             break;

         case  QMI_EMBMS_GET_AVAILABLE_TMGI_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_GET_AVAILABLE_TMGI;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_GET_AVAILABLE_TMGI";
             break;

         case  QMI_EMBMS_GET_ACTIVE_TMGI_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_GET_ACTIVE_TMGI;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_GET_ACTIVE_TMGI";
             break;

         case  QMI_EMBMS_UPDATE_CONTENT_DESC_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_CONTENT_DESC_UPDATE;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_CONTENT_DESC_UPDATE";
             break;

         case  QMI_EMBMS_GET_INTERESTED_TMGI_LIST_RESP_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_SEND_INTERESTED_TMGI_LIST;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_SEND_INTERESTED_TMGI_LIST";
             break;

         case  QMI_EMBMS_ENABLE_RSSI_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_ENABLE_RSSI;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_ENABLE_RSSI";
             break;

         case  QMI_EMBMS_DISABLE_RSSI_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_DISABLE_RSSI;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_DISABLE_RSSI";
             break;

         case  QMI_EMBMS_GET_COVERAGE_STATE_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_GET_COVERAGE_STATE;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_GET_COVERAGE_STATE";
             break;

         case  QMI_EMBMS_GET_RSSI_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_GET_RSSI;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_GET_RSSI";
             break;

         case  QMI_EMBMS_SET_SNTP_TIME_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_SET_SNTP_TIME;
              msg_returned = "QCRIL_EVT_HOOK_EMBMS_SET_SNTP_TIME";
              break;

         case  QMI_EMBMS_GET_SIB16_COVERAGE_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_GET_SIB16_COVERAGE;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_GET_SIB16_COVERAGE";
             break;

         case  QMI_EMBMS_GET_UTC_TIME_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_GET_UTC_TIME;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_GET_UTC_TIME";
             break;

         case  QMI_EMBMS_GET_EMBMS_SERVICE_STATE_REQ_V01:
             *request_id =  QCRIL_REQ_HOOK_EMBMS_GET_EMBMS_SVC_STATE;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_GET_EMBMS_SVC_STATE";
             break;

         case  QMI_EMBMS_ACTIVATE_DEACTIVATE_TMGI_REQ_V01:
             *request_id = QCRIL_REQ_HOOK_EMBMS_ACTIVATE_DEACTIVATE_TMGI;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_ACTIVATE_DEACTIVATE_TMGI";
             break;

         case  QMI_EMBMS_GET_ACTIVE_LOG_PACKET_IDS_REQ_V01:
             *request_id = QCRIL_REQ_HOOK_EMBMS_GET_ACTIVE_LOG_PACKET_IDS;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_GET_ACTIVE_LOG_PACKET_IDS";
             break;

         case  QMI_EMBMS_DELIVER_LOG_PACKET_REQ_V01:
             *request_id = QCRIL_REQ_HOOK_EMBMS_DELIVER_LOG_PACKET;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_DELIVER_LOG_PACKET";
             break;

         case  QMI_EMBMS_GET_E911_STATE_REQ_V01:
             *request_id = QCRIL_REQ_HOOK_EMBMS_GET_E911_STATE;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_GET_E911_STATE";
             break;

         case  QMI_EMBMS_GET_SIB_PLMN_REQ_V01:
             *request_id = QCRIL_REQ_HOOK_EMBMS_GET_SIB_PLMN;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_GET_SIB_PLMN";
             break;

         case  QMI_EMBMS_GET_EMBMS_STATUS_REQ_V01:
             *request_id = QCRIL_REQ_HOOK_EMBMS_GET_EMBMS_STATUS;
             msg_returned = "QCRIL_EVT_HOOK_EMBMS_GET_EMBMS_STATUS";
             break;

         default:
             QCRIL_LOG_ERROR("invalid messsage-id = %d received",  message_id);
             result = RIL_E_INVALID_ARGUMENTS;
             break;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("EMBMS service is not supported");
      result = RIL_E_INVALID_ARGUMENTS;
    }
  }
  else if ( service_id == QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_QTUNER )
  {
    switch( message_id )
    {
      case QMI_Qtuner_SET_RFM_SCENARIO_REQ_V01:
        *request_id =  QCRIL_REQ_HOOK_SET_RFM_SCENARIO_REQ;
        msg_returned = "QCRIL_EVT_HOOK_SET_RFM_SCENARIO_REQ";
        break;

      case QMI_Qtuner_GET_RFM_SCENARIO_REQ_V01:
        *request_id =  QCRIL_REQ_HOOK_GET_RFM_SCENARIO_REQ;
        msg_returned = "QCRIL_EVT_HOOK_GET_RFM_SCENARIO_REQ";
        break;

      case QMI_Qtuner_GET_PROVISIONED_TABLE_REVISION_REQ_V01:
        *request_id = QCRIL_REQ_HOOK_GET_PROVISIONED_TABLE_REVISION_REQ;
        msg_returned = "QCRIL_EVT_HOOK_GET_PROVISIONED_TABLE_REVISION_REQ";
        break;

      default:
        QCRIL_LOG_ERROR("invalid messsage-id = %d received",  message_id);
        result = RIL_E_INVALID_ARGUMENTS;
        break;
    }
  }
  else
  {
     QCRIL_LOG_ERROR("invalid service-id = %d received", service_id);
      result = RIL_E_INVALID_ARGUMENTS;
   }

  *log_ind = msg_returned;
  return result;
}

//=========================================================================
// qmi_ril_parse_oem_hook_header
//===========================================================================
RIL_Errno qmi_ril_parse_oem_hook_header ( unsigned char *data, qmi_ril_oem_hook_request_details_type *outcome )
{
  RIL_Errno res  = RIL_E_GENERIC_FAILURE;
  char oem_name [ QCRIL_OTHER_OEM_NAME_LENGTH + 2 ];

  uint32 index = 0;
  int cmd_id = 0;
  qmi_ril_oem_hook_request_details_type   outcome_data;

  memset( &outcome_data, 0, sizeof( outcome_data ) );
  do
  {
    if(data == NULL)
    {
       QCRIL_LOG_ERROR( "data is NULL");
       break;
    }

    // decode the raw string to find out the oem name string data[0 - 7], 8 bytes
    if( strncmp( (char *)data, QCRIL_HOOK_OEM_NAME, QCRIL_OTHER_OEM_NAME_LENGTH ) != 0 )
    {
       memcpy( oem_name, &data[index], QCRIL_OTHER_OEM_NAME_LENGTH );
       oem_name[QCRIL_OTHER_OEM_NAME_LENGTH] = '\0';
       QCRIL_LOG_ERROR( "Mismatch in oem_name between received=%s and expected=%s", oem_name, QCRIL_HOOK_OEM_NAME);
       break;
    }

    // incrementing the index by OEM name size i.e 9 bytes
    index += (uint32)QCRIL_OTHER_OEM_NAME_LENGTH;

    // decode the raw string to find out command id, data[9 - 12], 4 bytes
    memcpy( &cmd_id, &data[index], QCRIL_OTHER_OEM_REQUEST_ID_LEN );

    QCRIL_LOG_DEBUG("command-id = %d", cmd_id);

    if( cmd_id >= QCRIL_REQ_HOOK_MAX )
    {
       QCRIL_LOG_ERROR( "Received un expected command id = %lu", cmd_id );
       break;
    }

    switch ( cmd_id )
    {
      case QCRIL_REQ_HOOK_REQ_GENERIC:
        outcome_data.is_qmi_tunneling = TRUE;
        outcome_data.is_oem_hook    = TRUE;
        outcome_data.hook_req       = cmd_id;
        break;
      case QCRIL_REQ_HOOK_LTE_DIRECT_DISC_REQ:
      case QCRIL_REQ_HOOK_PROTOBUF_MSG:
        outcome_data.is_qmi_proto = TRUE;
        outcome_data.is_oem_hook    = TRUE;
        outcome_data.hook_req       = cmd_id;
        break;
      case QCRIL_REQ_HOOK_NV_READ:                    // fall through
      case QCRIL_REQ_HOOK_NV_WRITE:                   // fall through
      case QCRIL_REQ_HOOK_DATA_GO_DORMANT:            // fall through
      case QCRIL_REQ_HOOK_ME_DEPERSONALIZATION:       // fall through
      case QCRIL_REQ_HOOK_CSG_PERFORM_NW_SCAN:
      case QCRIL_REQ_HOOK_CSG_SET_SYS_SEL_PREF:
      case QCRIL_REQ_HOOK_CSG_GET_SYS_INFO:
      case QCRIL_REQ_HOOK_UICC_VOLTAGE_STATUS_REQ:
      case QCRIL_REQ_HOOK_PERSONALIZATION_REACTIVATE_REQ:
      case QCRIL_REQ_HOOK_PERSONALIZATION_STATUS_REQ:
      case QCRIL_REQ_HOOK_ENTER_DEPERSONALIZATION_REQ:
      case QCRIL_REQ_HOOK_SLOT_GET_CARD_STATE_REQ:
      case QCRIL_REQ_HOOK_SLOT_GET_ATR_REQ:
      case QCRIL_REQ_HOOK_SLOT_SEND_APDU_REQ:
      case QCRIL_REQ_HOOK_SWITCH_SLOT_REQ:
      case QCRIL_REQ_HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ:
      case QCRIL_REQ_HOOK_SLOT_CARD_POWER_REQ:
      case QCRIL_REQ_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ:
      case QCRIL_REQ_HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ:
      case QCRIL_REQ_HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ:
      case QCRIL_REQ_HOOK_GET_SLOTS_STATUS_REQ:
      // DSDS/DSDA requests
      case QCRIL_REQ_HOOK_SET_TUNE_AWAY:              // fall through
      case QCRIL_REQ_HOOK_GET_TUNE_AWAY:              // fall through
      case QCRIL_REQ_HOOK_SET_PAGING_PRIORITY:        // fall through
      case QCRIL_REQ_HOOK_GET_PAGING_PRIORITY:        // fall through
      case QCRIL_REQ_HOOK_SET_DEFAULT_VOICE_SUB:      // fall through
      case QCRIL_REQ_HOOK_SET_LOCAL_CALL_HOLD:        // fall through
      case QCRIL_REQ_HOOK_GET_MODEM_CAPABILITY:
      case QCRIL_REQ_HOOK_UPDATE_SUB_BINDING:
      case QCRIL_REQ_HOOK_REJECT_INCOMING_CALL_CAUSE_21:
      case QCRIL_REQ_HOOK_SET_LTE_TUNE_AWAY:
      case QCRIL_REQ_HOOK_GET_ADN_RECORD:
      case QCRIL_REQ_HOOK_UPDATE_ADN_RECORD:
        outcome_data.is_oem_hook    = TRUE;
        outcome_data.hook_req       = cmd_id;
        break;

      default:
        QCRIL_LOG_ERROR( "Received un-handled/oem command id = %lu", cmd_id );
        outcome_data.is_oem_hook    = TRUE;
        outcome_data.hook_req       = cmd_id;
        break;
    }

    // incrementing the index by command id size i.e 4 bytes
    index += (uint32)QCRIL_OTHER_OEM_REQUEST_ID_LEN;

    // decode the raw string to find the length of the payload, data[13 - 16],  4 bytes
    memcpy( &outcome_data.hook_req_len, &data[index], QCRIL_OTHER_OEM_REQUEST_DATA_LEN );

    // Convert the received hook request into internal event and store
    outcome_data.hook_req_event = qcril_qmi_oem_get_event(outcome_data.hook_req);
    // finally
    res = RIL_E_SUCCESS;

  } while ( FALSE );

  QCRIL_LOG_DEBUG( "res = %d", res );
  if ( RIL_E_SUCCESS == res )
  {
    *outcome = outcome_data;
    QCRIL_LOG_DEBUG( "is_tunneling = %d, is_oemhook = %d, event_id  = %d, is_proto = %d",
                     outcome->is_qmi_tunneling, outcome->is_oem_hook, outcome->hook_req, outcome_data.is_qmi_proto );
  }
  return res;
} //  qmi_ril_parse_oem_hook_header


/*===========================================================================
  FUNCTION  qcril_qmi_oem_convert_oem_token_to_ril_token
===========================================================================*/
/*!
    @brief Convert oem token to RIL token
*/
/*=========================================================================*/
RIL_Token qcril_qmi_oem_convert_oem_token_to_ril_token(uint32_t oem_token)
{
  RIL_Token ret = qcril_malloc(sizeof(uint32_t));
  if (NULL != ret)
  {
    uint32_t *tmp = (uint32_t*) ret;
    *tmp = oem_token ^ 0xc0000000;
  }
  return ret;
} // qcril_qmi_oem_convert_oem_token_to_ril_token

/*===========================================================================
  FUNCTION  qcril_qmi_oem_free_and_convert_ril_token_to_oem_token
===========================================================================*/
/*!
    @brief get oem token from RIL token and free RIL token
*/
/*=========================================================================*/
uint32_t qcril_qmi_oem_free_and_convert_ril_token_to_oem_token(RIL_Token ril_token)
{
  uint32_t ret = 0xFFFFFFFF;
  if (ril_token)
  {
      ret = (*((uint32_t *) ril_token)) ^ 0xc0000000;
      QCRIL_LOG_INFO("oem token: %d", ret);
      qcril_free((void*) ril_token);
  }
  else
  {
      QCRIL_LOG_ERROR("ril_token is NULL");
  }

  return ret;
} // qcril_qmi_oem_free_and_convert_ril_token_to_oem_token


/*===========================================================================
  FUNCTION qmi_ril_parse_oem_req_proto_message
===========================================================================*/
/*!
    @brief Process OEM request message
*/
/*=========================================================================*/
boolean qmi_ril_parse_oem_req_proto_message
(
  qmi_ril_oem_hook_request_details_type  *oem_hook_req_details,
  RIL_Errno                              *audit_result,
  qcril_request_params_type              *param
)
{
    boolean ret = TRUE;
    qmi_ril_oem_hook_response_context_type*  oem_hook_qmi_idl_resp_track;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        if (!oem_hook_req_details || !audit_result || !param)
        {
            if (audit_result)
            {
                *audit_result = RIL_E_GENERIC_FAILURE;
            }
            ret = FALSE;
            break;
        }

        size_t unpacked_msg_size = 0;

        QtiTelephonyService_RILOEMMessage *msg_tag_ptr = NULL;
        void *msg_data_ptr = NULL;

        qcril_evt_e_type oem_hook_proto_req;
        QCRIL_LOG_INFO("unpacking Message tag");

        msg_tag_ptr = qcril_qmi_oem_proto_unpack_msg_tag((uint8_t*)param->data, param->datalen);

        if(msg_tag_ptr)
        {
            if(msg_tag_ptr->has_error)
             {
               QCRIL_LOG_INFO( "msg: %s, type: %d, message_id: %d, error: %d",
                               qcril_qmi_oem_proto_get_msg_log_str(msg_tag_ptr->id, msg_tag_ptr->type),
                               msg_tag_ptr->type,
                               msg_tag_ptr->id,
                               msg_tag_ptr->error);
             }
             else
             {
               QCRIL_LOG_INFO( "msg: %s, type: %d, message_id: %d",
                               qcril_qmi_oem_proto_get_msg_log_str(msg_tag_ptr->id, msg_tag_ptr->type),
                               msg_tag_ptr->type,
                               msg_tag_ptr->id);
             }

            qcril_qmi_oem_proto_parse_packed_msg(msg_tag_ptr->type, msg_tag_ptr->id, msg_tag_ptr->payload, param->datalen, &msg_data_ptr,
                        &unpacked_msg_size, &oem_hook_proto_req);

            oem_hook_qmi_idl_resp_track = (qmi_ril_oem_hook_response_context_type*)qcril_malloc( sizeof( *oem_hook_qmi_idl_resp_track )  );
            if ( NULL == oem_hook_qmi_idl_resp_track )
            {
                QCRIL_LOG_ERROR( "QMI IDL - tracker allocation failure" );
                *audit_result = RIL_E_NO_MEMORY;
                ret = FALSE;
                break;
            }
            memset( oem_hook_qmi_idl_resp_track, 0, sizeof( *oem_hook_qmi_idl_resp_track ) );
            oem_hook_qmi_idl_resp_track->original_token     = param->t;
            oem_hook_qmi_idl_resp_track->proto_token     = qcril_qmi_oem_convert_proto_token_to_ril_token(msg_tag_ptr->token);

            pthread_mutex_lock( &qmi_ril_oem_hook_overview.overview_lock_mutex );
            oem_hook_qmi_idl_resp_track->next   = qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root;
            qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root  = oem_hook_qmi_idl_resp_track;
            pthread_mutex_unlock( &qmi_ril_oem_hook_overview.overview_lock_mutex );

            qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                               QCRIL_DEFAULT_MODEM_ID,
                               QCRIL_DATA_NOT_ON_STACK,
                               (qcril_evt_e_type) oem_hook_proto_req,
                               msg_data_ptr,
                               unpacked_msg_size,
                               oem_hook_qmi_idl_resp_track->proto_token );

            *audit_result = RIL_E_SUCCESS;
            QCRIL_LOG_INFO("Freeing msg_tag_ptr after queing the request");
            qcril_qmi_npb_release(QtiTelephonyService_RILOEMMessage_fields, msg_tag_ptr);
            qcril_free(msg_tag_ptr);
        }
        else
        {
            *audit_result = RIL_E_GENERIC_FAILURE;
            ret = FALSE;
        }
    } while (FALSE);

    QCRIL_LOG_FUNC_RETURN();
    return ret;
}

/*===========================================================================
  FUNCTION qmi_ril_parse_oem_req_lte_direct_disc
===========================================================================*/
/*!
    @brief Process Lte Direct Discovery OEM request message
*/
/*=========================================================================*/
boolean qmi_ril_parse_oem_req_lte_direct_disc
(
 qmi_ril_oem_hook_request_details_type  *oem_hook_req_details,
 RIL_Errno                              *audit_result,
 qcril_request_params_type              *param
)
{
    boolean ret = TRUE;
    qmi_ril_oem_hook_response_context_type*  oem_hook_qmi_idl_resp_track;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        if (!oem_hook_req_details || !audit_result || !param)
        {
            if (audit_result)
            {
                *audit_result = RIL_E_GENERIC_FAILURE;
            }
            ret = FALSE;
            break;
        }

        size_t unpacked_msg_size = 0;

        LteDirectDiscovery_MsgHeader *msg_tag_ptr = NULL;
        void *msg_data_ptr = NULL;

        qcril_evt_e_type lted_req;
        QCRIL_LOG_INFO("unpacking Message tag");

        msg_tag_ptr = qcril_qmi_lte_direct_disc_unpack_msg_tag(
                (uint8_t*)param->data, param->datalen);

        if(msg_tag_ptr)
        {
            QCRIL_LOG_INFO("msg: %s, type: %d, message_id: %d, error: %d",
                    qcril_qmi_lte_direct_disc_get_msg_log_str(
                        msg_tag_ptr->id, msg_tag_ptr->type),
                    msg_tag_ptr->type,
                    msg_tag_ptr->id,
                    msg_tag_ptr->error);

            qcril_qmi_lte_direct_disc_parse_packed_msg(msg_tag_ptr->type,
                    msg_tag_ptr->id, msg_tag_ptr->payload,
                    param->datalen, &msg_data_ptr,
                    &unpacked_msg_size, &lted_req);

            oem_hook_qmi_idl_resp_track = (qmi_ril_oem_hook_response_context_type*)qcril_malloc(
                    sizeof(*oem_hook_qmi_idl_resp_track));
            if (NULL == oem_hook_qmi_idl_resp_track)
            {
                QCRIL_LOG_ERROR("QMI IDL - tracker allocation failure");
                *audit_result = RIL_E_GENERIC_FAILURE;
                ret = FALSE;
                break;
            }
            memset(oem_hook_qmi_idl_resp_track, 0, sizeof(*oem_hook_qmi_idl_resp_track));
            oem_hook_qmi_idl_resp_track->original_token = param->t;
            oem_hook_qmi_idl_resp_track->proto_token    =
                qcril_qmi_lte_direct_disc_convert_lted_token_to_ril_token(msg_tag_ptr->token);

            QCRIL_LOG_INFO("original_token = 0x%x, proto_token = 0x%x",
                    oem_hook_qmi_idl_resp_track->original_token,
                    oem_hook_qmi_idl_resp_track->proto_token);

            pthread_mutex_lock( &qmi_ril_oem_hook_overview.overview_lock_mutex );
            oem_hook_qmi_idl_resp_track->next   =
                qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root;
            qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root  =
                oem_hook_qmi_idl_resp_track;
            pthread_mutex_unlock( &qmi_ril_oem_hook_overview.overview_lock_mutex );

            qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                    QCRIL_DEFAULT_MODEM_ID,
                    QCRIL_DATA_NOT_ON_STACK,
                    (qcril_evt_e_type) lted_req,
                    msg_data_ptr,
                    unpacked_msg_size,
                    oem_hook_qmi_idl_resp_track->proto_token );

            *audit_result = RIL_E_SUCCESS;
            //QCRIL_LOG_INFO("Freeing msg_tag_ptr after queing the request");
            //qcril_qmi_npb_release(QtiTelephonyService_RILOEMMessage_fields, msg_tag_ptr);
            //qcril_free(msg_tag_ptr);
        }
        else
        {
            *audit_result = RIL_E_GENERIC_FAILURE;
            ret = FALSE;
        }
    } while (FALSE);

    QCRIL_LOG_FUNC_RETURN();
    return ret;
}

/*===========================================================================
  FUNCTION  qcril_qmi_oem_socket_lte_direct_disc_send
===========================================================================*/
/*!
    @brief Send response for proto message
*/
/*=========================================================================*/
void qcril_qmi_oem_socket_lte_direct_disc_send
(
 RIL_Token                  token,
 LteDirectDiscovery_MsgType type,
 LteDirectDiscovery_MsgId   message_id,
 LteDirectDiscovery_Error   error,
 const void                 *msg,
 int                        msg_len
)
{
    RIL_Token original_ril_token;
    uint32_t original_oem_token;
    int is_qmi_proto_response = FALSE;
    RIL_Errno   ril_error;
    qmi_ril_oem_hook_response_context_type * oem_hook_response_ctx = NULL;
    qmi_ril_oem_hook_response_context_type * prev_iter = NULL;
    size_t packed_msg_len;

    static uint8_t send_buffer[8 * 1024];

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_DEBUG("token = %d", token);

    pthread_mutex_lock( &qmi_ril_oem_hook_overview.overview_lock_mutex );
    oem_hook_response_ctx = qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root;

    memset(send_buffer, 0, sizeof(send_buffer));

    while (NULL != oem_hook_response_ctx && !is_qmi_proto_response)
    {
        QCRIL_LOG_DEBUG("proto token = %d, original token = %d",
                oem_hook_response_ctx->proto_token,
                oem_hook_response_ctx->original_token);
        if (token == oem_hook_response_ctx->proto_token)
        {  // match
            is_qmi_proto_response = TRUE;
            original_ril_token = oem_hook_response_ctx->original_token;
            if (NULL != prev_iter)
            {
                prev_iter->next = oem_hook_response_ctx->next;
            }
            else
            { // top
                qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root =
                    oem_hook_response_ctx->next;
            }
        }
        else
        {
            prev_iter = oem_hook_response_ctx;
            oem_hook_response_ctx = oem_hook_response_ctx->next;
        }
    }

    pthread_mutex_unlock( &qmi_ril_oem_hook_overview.overview_lock_mutex );

    if ( oem_hook_response_ctx )
    {
        qcril_free( oem_hook_response_ctx );
    }

    if(!is_qmi_proto_response)
    {
        QCRIL_LOG_DEBUG("Not a proto request response %d", token);
        return;
    }

    qcril_reqlist_free( QCRIL_DEFAULT_INSTANCE_ID , token );
    original_oem_token = qcril_qmi_oem_free_and_convert_ril_token_to_oem_token(original_ril_token);
    ril_error = RIL_E_SUCCESS;

    packed_msg_len = qcril_qmi_lte_direct_disc_pack_msg_tag(msg, msg_len,
            qcril_qmi_lte_direct_disc_free_and_convert_ril_token_to_lted_token(token),
            type,
            message_id,
            error,
            send_buffer,
            sizeof(send_buffer));

    if (type == LteDirectDiscovery_MsgType_RESPONSE)
    {
      if (oemhookServiceRegistered) {
        OemhookAgent::getOemhookAgent()->sendOemhookMessageResponse(QCRIL_DEFAULT_INSTANCE_ID,
            original_oem_token, ril_error,
            send_buffer, packed_msg_len, QCRIL_REQ_HOOK_LTE_DIRECT_DISC_REQ);
      } else {
        QCRIL_LOG_ERROR("qcril_qmi_oemhook_agent_send: oemhook service not registered");
      }
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================
  FUNCTION  qcril_qmi_oem_socket_lte_direct_disc_send_unsol
===========================================================================*/
/*!
    @brief Send response for proto message
*/
/*=========================================================================*/
void qcril_qmi_oem_socket_lte_direct_disc_send_unsol
(
 LteDirectDiscovery_MsgType type,
 LteDirectDiscovery_MsgId   message_id,
 LteDirectDiscovery_Error   error,
 const void                 *msg,
 int                        msg_len
)
{
    size_t packed_msg_len;

    static uint8_t send_buffer[8 * 1024];

    QCRIL_LOG_FUNC_ENTRY();

    memset(send_buffer, 0, sizeof(send_buffer));

    packed_msg_len = qcril_qmi_lte_direct_disc_pack_msg_tag(msg, msg_len,
            10, // dummy token TODO
            type,
            message_id,
            error,
            send_buffer,
            sizeof(send_buffer));

    if (type == LteDirectDiscovery_MsgType_UNSOL_RESPONSE)
    {
        uint32 unsol_event = QCRIL_REQ_HOOK_UNSOL_LTE_DIRECT_DISC;
        uint32 data_len    = packed_msg_len;
        char  *data        = (char *)send_buffer;
        char  *payload = NULL;
        uint32 index = 0;
        uint32 payload_len = 0;

        payload = (char *) qcril_malloc(QCRIL_OTHER_OEM_NAME_LENGTH +
                sizeof(unsol_event) +
                sizeof(data_len) + data_len );
        if ( NULL != payload )
        {
            memcpy( payload, QCRIL_HOOK_OEM_NAME, QCRIL_OTHER_OEM_NAME_LENGTH );
            index += QCRIL_OTHER_OEM_NAME_LENGTH;

            memcpy( &payload[index], &unsol_event, sizeof(unsol_event) );
            index += sizeof(unsol_event);

            memcpy( &payload[index], &data_len, sizeof(data_len) );
            index += sizeof(data_len);

            memcpy( &payload[index], data, data_len );
            index += data_len;

            payload_len = index;

            //qcril_qmi_oem_socket_send_unsol(qmi_ril_get_process_instance_id(), payload, payload_len);

            if (oemhookServiceRegistered) {
              OemhookAgent::getOemhookAgent()->sendOemhookMessageUnsol(QCRIL_DEFAULT_INSTANCE_ID, payload, payload_len);
            } else {
              QCRIL_LOG_ERROR("qcril_qmi_oemhook_agent_send_unsol: oemhook service not registered");
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================
  FUNCTION qmi_ril_parse_oem_req_tunnelled_message
===========================================================================*/
/*!
    @brief Process OEM request message
*/
/*=========================================================================*/
boolean qmi_ril_parse_oem_req_tunnelled_message
(
  qmi_ril_oem_hook_request_details_type  *oem_hook_req_details,
  RIL_Errno                              *audit_result,
  qcril_request_params_type              *param
)
{
  uint16_t                                *uint16_param;
  const char                              *evt_name;
  RIL_Errno                                info_fetch_result;
  qmi_idl_service_object_type              qmi_idl_tunneling_service_object;
  qmi_client_error_type                    idl_err;
  uint32_t                                 substituted_data_len;
  void                                    *substituted_data;
  qmi_ril_oem_hook_response_context_type*  oem_hook_qmi_idl_resp_track;

  if (!oem_hook_req_details || !audit_result || !param)
  {
    return FALSE;
  }

  *audit_result = RIL_E_INTERNAL_ERR;

  //ignore the modem id size
  param->data =  (char * )param->data + OEM_HOOK_QMI_TUNNELING_REQ_MODEM_ID_SIZE;

  //extract the service -id
  uint16_param = (uint16_t *) param->data;
  oem_hook_req_details->ril_idl_service_id = *uint16_param;
  param->data =  (char * )param->data + OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE;

  //extract the message -id
  uint16_param = (uint16_t *) param->data;
  oem_hook_req_details->ril_idl_message_id = *uint16_param;
  param->data =  (char * )param->data + OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE;

  param->datalen = param->datalen - OEM_HOOK_QMI_TUNNELING_REQ_MODEM_ID_SIZE
                                                 - OEM_HOOK_QMI_TUNNELING_SVC_ID_SIZE
                                                 - OEM_HOOK_QMI_TUNNELING_MSG_ID_SIZE;

  evt_name = NULL;
  info_fetch_result = qmi_ril_oem_hook_get_request_id( oem_hook_req_details->ril_idl_service_id,
                                                       oem_hook_req_details->ril_idl_message_id,
                                                       (uint32*)&oem_hook_req_details->hook_req,
                                                       &evt_name );
  QCRIL_LOG_DEBUG( "qmi_ril_oem_hook_get_request_id() returned %d", info_fetch_result );
  if ( RIL_E_SUCCESS != info_fetch_result )
  {
    *audit_result = info_fetch_result;
    return FALSE;
  }
  oem_hook_req_details->hook_req_event = qcril_qmi_oem_get_event(oem_hook_req_details->hook_req);
  QCRIL_LOG_DEBUG( " sevice-id = %d, message-id = %d, request_id = %d",
                  oem_hook_req_details->ril_idl_service_id,
                  oem_hook_req_details->ril_idl_message_id,
                  oem_hook_req_details->hook_req );
  if ( NULL != evt_name )
  {
    QCRIL_LOG_DEBUG( "qmi_idl_tunneling: inclined to invoke evt-name = %s", evt_name );
  }
  else
  {
    QCRIL_LOG_DEBUG( "qmi_idl_tunneling: inclined to onvoke evt-name unknown");
  }
  param->event_id_android = oem_hook_req_details->hook_req;
  param->event_id = oem_hook_req_details->hook_req_event;

  // print the recieved TLV byte stream
  QCRIL_LOG_DEBUG("oemhook tlv only byte stream");
  qcril_qmi_print_hex((unsigned char *)param->data,  param->datalen);

  // convert payload if any to readable form
  qmi_idl_tunneling_service_object = qmi_ril_oem_hook_qmi_idl_tunneling_get_service_object( (qmi_ril_oem_hook_qmi_tunneling_service_id_type)oem_hook_req_details->ril_idl_service_id );

  if ( NULL != qmi_idl_tunneling_service_object )
  {
    idl_err = qmi_idl_get_message_c_struct_len( qmi_idl_tunneling_service_object, QMI_IDL_REQUEST, oem_hook_req_details->ril_idl_message_id, &substituted_data_len  );

    QCRIL_LOG_DEBUG("decoded msg len ret = %d, input length = %d", substituted_data_len, param->datalen);
    QCRIL_LOG_DEBUG("srvc_id = %d, msg_id = %d", oem_hook_req_details->ril_idl_service_id, oem_hook_req_details->ril_idl_message_id);

    if ( oem_hook_req_details->ril_idl_service_id == QMI_RIL_OEM_HOOK_QMI_TUNNELING_SERVICE_VT && oem_hook_req_details->ril_idl_message_id == IMS_VT_GET_CALL_INFO_REQ_V01 )
    {
       if ( substituted_data_len != 4 )
       {
          QCRIL_LOG_DEBUG("change max length to 4 due to  idl issue");
          substituted_data_len = 4;
       }
    }

    if ( QMI_NO_ERR == idl_err )
    {
      if (substituted_data_len > 0 )
      {
        // expected non empty payload
        substituted_data = qcril_malloc( substituted_data_len );

        if ( NULL != substituted_data )
        {
            idl_err = qmi_idl_message_decode( qmi_idl_tunneling_service_object,
                                    QMI_IDL_REQUEST,
                                    oem_hook_req_details->ril_idl_message_id,
                                    param->data,
                                    param->datalen,
                                    substituted_data,
                                    substituted_data_len );

            QCRIL_LOG_DEBUG("substituated msg len  = %d, input length = %d", substituted_data_len, param->datalen);

            if ( QMI_NO_ERR == idl_err )
            {
              param->data      = substituted_data;
              param->datalen   = substituted_data_len;
            }
            else
            {
              QCRIL_LOG_ERROR( "QMI IDL - request decode error %d", (int) idl_err  );
              qcril_free(substituted_data);
              *audit_result = RIL_E_INVALID_ARGUMENTS;
              return FALSE;
            }
        }
        else
        {
          QCRIL_LOG_ERROR( "QMI IDL - substituted data decoded request allocation failed for len %d", (int) substituted_data_len  );
          *audit_result = RIL_E_NO_MEMORY;
          return FALSE;
        }
      }
      else
      { // empty payload
        param->data      = NULL;
        param->datalen   = QMI_RIL_ZERO;
      }
    }
    else
    {
      QCRIL_LOG_ERROR( "QMI IDL - request decode error len %d", (int) idl_err );
      *audit_result = RIL_E_INVALID_ARGUMENTS;
      return FALSE;
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "QMI IDL - NULL service object" );
    return FALSE;
  }

  // store token, service, message -d,  as they are required while sending the response.
  oem_hook_qmi_idl_resp_track = (qmi_ril_oem_hook_response_context_type*)qcril_malloc( sizeof( *oem_hook_qmi_idl_resp_track )  );
  if ( NULL == oem_hook_qmi_idl_resp_track )
  {
    QCRIL_LOG_ERROR( "QMI IDL - tracker allocation failure" );
    *audit_result = RIL_E_NO_MEMORY;
    return FALSE;
  }
  memset( oem_hook_qmi_idl_resp_track, 0, sizeof( *oem_hook_qmi_idl_resp_track ) );
  oem_hook_qmi_idl_resp_track->original_token     = param->t;
  oem_hook_qmi_idl_resp_track->ril_idl_service_id = oem_hook_req_details->ril_idl_service_id;
  oem_hook_qmi_idl_resp_track->ril_idl_message_id = oem_hook_req_details->ril_idl_message_id;
  oem_hook_qmi_idl_resp_track->ril_request_id     = oem_hook_req_details->hook_req_event;
  *audit_result = RIL_E_SUCCESS;

  pthread_mutex_lock( &qmi_ril_oem_hook_overview.overview_lock_mutex );
  oem_hook_qmi_idl_resp_track->next   = qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root;
  qmi_ril_oem_hook_overview.qmi_ril_oem_hook_qmi_tunneling_reqs_root  = oem_hook_qmi_idl_resp_track;
  pthread_mutex_unlock( &qmi_ril_oem_hook_overview.overview_lock_mutex );

  return TRUE;
}

/* This function should be implemented by OEMs. This function will receive the events
 * meant to be implemented as an extension of QC OEM HOOK implementation
 * EVENT REFACTOR: move this to a separate file.
 */
void qcrilhook_oem
(
  int           instance_id,
  int           request,
  char          *data,
  size_t        data_len,
  RIL_Token     t
)
{
  /* nothing to do here, has to done by oem */
    QCRIL_NOTUSED(instance_id);
    QCRIL_NOTUSED(request);
    QCRIL_NOTUSED(data);
    QCRIL_NOTUSED(data_len);
    QCRIL_NOTUSED(t);
} /* end of qcrilhook_oem */


/*===========================================================================
  FUNCTION  qmi_ril_get_req_details_from_oem_req
===========================================================================*/
/*!
    @brief Process OEM request message
*/
/*=========================================================================*/
boolean qmi_ril_get_req_details_from_oem_req
(
  qmi_ril_oem_hook_request_details_type  *oem_hook_req_details,
  RIL_Errno                              *audit_result,
  unsigned char                          *data,
  qcril_request_params_type              *param,
  size_t                                     recv_byte_num
)
{
  if (!oem_hook_req_details || !audit_result || !data || !param)
  {
    return FALSE;
  }

  // parse the OEM hook request to distinguish between internal or customer specific requests
  if ( ( RIL_E_SUCCESS == qmi_ril_parse_oem_hook_header( data, oem_hook_req_details ) ) && oem_hook_req_details->is_oem_hook )
  {
    if ( ( ( oem_hook_req_details->hook_req > QCRIL_REQ_HOOK_BASE ) && ( oem_hook_req_details->hook_req < QCRIL_REQ_HOOK_MAX ) ) ||
         ( ( oem_hook_req_details->hook_req > QCRIL_REQ_OEM_BASE ) && ( oem_hook_req_details->hook_req < QCRIL_REQ_OEM_MAX ) ) )
    {
      // This is an OEM_HOOK request, Convert it to look like a internal RIL REQUEST
      // Move data pointer past the QCRILHook header and re-adjusting the length
      param->event_id = oem_hook_req_details->hook_req_event;
      param->event_id_android = oem_hook_req_details->hook_req;
      param->data = (char *)data+ QCRIL_HOOK_HEADER_SIZE;
      param->datalen = oem_hook_req_details->hook_req_len;
      if ( ( oem_hook_req_details->hook_req > QCRIL_REQ_OEM_BASE ) && ( oem_hook_req_details->hook_req < QCRIL_REQ_OEM_MAX ) )
      {
        // externally provided by an OEM, otherwise will be dispatched via qcril_dispatch_event()
        QCRIL_LOG_DEBUG("diverting inbound OEM HOOK request to external handler");
        qcrilhook_oem( param->instance_id, param->event_id_android, (char *)data, recv_byte_num, param->t);
        *audit_result = RIL_E_SUCCESS;
        return FALSE;
      }
      *audit_result = RIL_E_SUCCESS;
    }
    else
    {
      // The request is not supported
      *audit_result = RIL_E_REQUEST_NOT_SUPPORTED;
      return FALSE;
    }
  }
  else
  {
    // The request is not supported
    *audit_result = RIL_E_REQUEST_NOT_SUPPORTED;
    return FALSE;
  }

  return TRUE;
}
