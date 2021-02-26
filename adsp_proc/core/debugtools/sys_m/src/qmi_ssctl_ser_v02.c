/*============================================================================
  @file qmi_ssctl_v02_ser.c

  Sub System Control(SSCTL_v02) QMI service implementation
  
  Copyright (c) 2014-17 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================
                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/debugtools/sys_m/src/qmi_ssctl_ser_v02.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/16   nk      Added changes to introduce delay through QXDM command before entering into STM 
09/10/14   rks     initial SSCTL v02 service implementation

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/



#include "comdef.h"
#include "qmi_csi.h"
#include "subsystem_control_v02.h"
#include "stringl.h"
#include "tms_utils_msg.h"
#include "sys_m.h"
#include "sys_m_sizes.h"
#include "rcinit.h"

#include "qurt.h"
#include "qurt_event.h"

#define MSG_SEP_CHAR ':'
#define SSR_STRING "ssr:"

#define BEFORE_POWERUP "before_powerup"
#define AFTER_POWERUP "after_powerup"
#define BEFORE_SHUTDOWN "before_shutdown"
#define AFTER_SHUTDOWN "after_shutdown"

#define SYS_M_QMI_v02_SIG     0x00100000

typedef struct
{
   qmi_client_handle  qmi_client_handle;
   boolean            connected;
} ssctl_v02_ser_client_type;

static ssctl_v02_ser_client_type qmi_sys_m_client;
static qmi_csi_service_handle ssctl_v02_qmi_ser_handler = NULL;
static qurt_anysignal_t qmi_ssctl_ser_sig;
static qmi_response_type_v01 sys_m_qmi_resp;

/**
  @brief ssctl_v02_ser_validate_connection_handle.

  This function validates a connection handle.

  @param  : connection_handle: Handle to validate.

  @return : TRUE if valid, FALSE otherwise
*/
static boolean ssctl_v02_ser_validate_connection_handle(void *connection_handle)
{
   /* Validate handle. */
   if (connection_handle == NULL)
   {
      TMS_MSG_HIGH("Connection handle NULL");
      return FALSE;
   }
   if (&qmi_sys_m_client ==
       (ssctl_v02_ser_client_type *)connection_handle)
   {
      return TRUE;
   }
   else{
      return FALSE;
  }
}


static const char* ssctl_v02_get_ssr_event(ssctl_ssr_event_enum_type_v02 event)
{
   switch(event)
   {
   case SSCTL_SSR_EVENT_BEFORE_POWERUP_V02:
      return BEFORE_POWERUP;
   case SSCTL_SSR_EVENT_AFTER_POWERUP_V02:
      return AFTER_POWERUP;
   case SSCTL_SSR_EVENT_BEFORE_SHUTDOWN_V02:
      return BEFORE_SHUTDOWN;
   case SSCTL_SSR_EVENT_AFTER_SHUTDOWN_V02:
      return AFTER_SHUTDOWN;
   default:
      TMS_MSG_ERROR_1("Invalid SSR event: 0x%x", event);
      break;
   }
   return NULL;
}
/**
  @brief ssctl_v02_ser_process_req

  This function processes incoming requests for the QMI sys_m
  services handler. Sanitizing function.
  req_c_struct and req_c_struct_len are validated in handling
  functions.

  @param  : connection_handle: Client connection making request.
            req_handle: QMI request handle
            msg_id: QMI msg request ID
            req_c_struct: QMI request structure
            req_c_struct_len: QMI request structure length
            service_cookie: Ignored

  @return : qmi_csi_cb_error.
*/

static qmi_csi_cb_error ssctl_v02_ser_process_req(void *connection_handle,
                                          qmi_req_handle  req_handle,
                                          unsigned int    msg_id,
                                          void           *req_c_struct,
                                          unsigned int    req_c_struct_len,
                                          void           *service_cookie)
{
   qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
   qmi_response_type_v01 *response = (qmi_response_type_v01 *)&sys_m_qmi_resp;
   if ((ssctl_v02_ser_validate_connection_handle(connection_handle) == FALSE) ||
      (req_handle == NULL))
   {
      TMS_MSG_ERROR_1("Invalid request arguments request id: 0x%x", msg_id);
      response->result = QMI_RESULT_FAILURE_V01;
      response->error  = QMI_ERR_INVALID_MESSAGE_ID_V01;
      return retval;
  }

   TMS_MSG_HIGH_1("SSCTL_V02 msg_id: 0x%x", msg_id);

   switch(msg_id)
   {
      case QMI_SSCTL_RESTART_REQ_V02:
      {
         qmi_ssctl_restart_resp_msg_v02 *response = (qmi_ssctl_restart_resp_msg_v02 *)&sys_m_qmi_resp;
         qmi_ssctl_restart_ind_msg_v02 ind;
         qmi_csi_error err;

         response->resp.result = QMI_RESULT_SUCCESS_V01;;
         response->resp.error  = QMI_ERR_NONE_V01;
         qmi_csi_send_resp(req_handle, msg_id, response, sizeof(qmi_ssctl_restart_resp_msg_v02));

         sys_m_shutdown(SYS_M_QMI);
         err = qmi_csi_send_ind((qmi_sys_m_client.qmi_client_handle),
                      QMI_SSCTL_RESTART_READY_IND_V02,
                      &ind,sizeof(qmi_ssctl_restart_ind_msg_v02));
         TMS_MSG_HIGH_1("QMI send error: %d", err);
         sys_m_shutdown_terminate_in_STM(sys_m_shutdown_delay_secs);
      }
      break;

      case QMI_SSCTL_SHUTDOWN_REQ_V02:
      {
         qmi_ssctl_shutdown_resp_msg_v02 *response = (qmi_ssctl_shutdown_resp_msg_v02 *)&sys_m_qmi_resp;
         qmi_ssctl_shutdown_ind_msg_v02 ind;
         qmi_csi_error err;

         response->resp.result = QMI_RESULT_SUCCESS_V01;;
         response->resp.error  = QMI_ERR_NONE_V01;
         qmi_csi_send_resp(req_handle, msg_id, response, sizeof(qmi_ssctl_shutdown_resp_msg_v02));

         sys_m_shutdown(SYS_M_QMI);

         err = qmi_csi_send_ind((qmi_sys_m_client.qmi_client_handle),
                           QMI_SSCTL_SHUTDOWN_READY_IND_V02,
                           &ind,sizeof(qmi_ssctl_shutdown_ind_msg_v02));


         TMS_MSG_HIGH_1("QMI send error: %d", err);
         sys_m_shutdown_terminate_in_STM(sys_m_shutdown_delay_secs);
      }
      break;

      case QMI_SSCTL_GET_FAILURE_REASON_REQ_V02:
      {
         qmi_ssctl_get_failure_reason_resp_msg_v02 sfr_resp;

         sfr_resp.error_message_valid = FALSE;
         sfr_resp.resp.result = QMI_RESULT_SUCCESS_V01;
         sfr_resp.resp.error  = QMI_ERR_NOT_SUPPORTED_V01;
         qmi_csi_send_resp(req_handle, msg_id, &sfr_resp, sizeof(qmi_ssctl_get_failure_reason_resp_msg_v02));
      }
      break;

      case QMI_SSCTL_SUBSYS_EVENT_REQ_V02:
      {
        qmi_ssctl_subsys_event_resp_msg_v02 ss_evt_rsp;
        qmi_ssctl_subsys_event_comp_ind_msg_v02 ind;
        size_t copy_size = 0;
        RCEVT_HANDLE handle = NULL;
        qmi_ssctl_subsys_event_req_msg_v02 *ss_evt_req_msg = (qmi_ssctl_subsys_event_req_msg_v02*)(req_c_struct);
        char ss_msg_str[SUBSYS_EVT_SIZE];
        int ss_msg_str_rem_sz = SUBSYS_EVT_SIZE;
        char *temp = (char *)(&ss_msg_str);
        const char* event_str = NULL;
        ss_evt_rsp.resp.result = QMI_RESULT_SUCCESS_V01;
        ss_evt_rsp.resp.error  = QMI_ERR_NONE_V01;


        /*need to make event case insensitive*/
        qmi_csi_send_resp(req_handle, msg_id, &ss_evt_rsp, sizeof(qmi_ssctl_subsys_event_resp_msg_v02));

        copy_size = memscpy(temp, EXTRA_SSR_CHAR_ADDN, SSR_STRING, sizeof(SSR_STRING));

        temp = (char*)temp + copy_size - 1;
        ss_msg_str_rem_sz -= copy_size;
        copy_size = memscpy(temp, QMI_SSCTL_SUB_SYS_NAME_LENGTH_V02,
                            ss_evt_req_msg->subsys_name, strlen(ss_evt_req_msg->subsys_name) +1);

        if(copy_size > 0){
            temp = (char*)temp + copy_size;
           *(temp - 1) = MSG_SEP_CHAR;
        }
        ss_msg_str_rem_sz -= copy_size;

        event_str = ssctl_v02_get_ssr_event(ss_evt_req_msg->event);

        if (event_str != NULL){
           copy_size = memscpy(temp, ss_msg_str_rem_sz,
                            event_str, MAX_EVENT_SIZE);
        }

        TMS_MSG_SPRINTF_HIGH_1("SSCTL SUBSYS_EVENT: %s", ss_msg_str);
        handle = rcevt_create_name(ss_msg_str);
        sys_m_signal_modules(handle);
        qmi_csi_send_ind((qmi_sys_m_client.qmi_client_handle),
                  QMI_SSCTL_SUBSYS_EVENT_COMP_IND_V02,
                  &ind,sizeof(qmi_ssctl_subsys_event_comp_ind_msg_v02));

      }
      break;

      default:
      {
         TMS_MSG_ERROR_1("Unhandled QMI request id: 0x%x", msg_id);

         response->result = QMI_RESULT_FAILURE_V01;
         response->error  = QMI_ERR_INVALID_MESSAGE_ID_V01;
      }
      qmi_csi_send_resp(req_handle, msg_id, response, sizeof(qmi_response_type_v01));
      break;
   }
   return retval;
}

/**
  @brief ssctl_v02_ser_connect

  This function processes incoming requests for the sys_m
  QMI services to connect a client.

  @param  : client_handle: Handle used by the infrastructure
                           to identify different services.
            service_cookie: Ignored
            connection_handle: Client connection to created
                               internally.

  @return : None.
*/
static qmi_csi_cb_error ssctl_v02_ser_connect(qmi_client_handle  client_handle,
                                           void              *service_cookie,
                                           void             **connection_handle)
{
   qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
   ssctl_v02_ser_client_type *client = (ssctl_v02_ser_client_type*)(&qmi_sys_m_client);

   /* Find empty client slot */
   if (client->connected == FALSE)
   {
      /* Found empty slot */
      client->qmi_client_handle = client_handle;
      client->connected = TRUE;
      *connection_handle = client;
      TMS_MSG_HIGH("Client connected");
   }
   else
   {
      TMS_MSG_ERROR("No client connections available");
      retval = QMI_CSI_CB_NO_MEM;
   }
   return retval;
}

/**
  @brief ssctl_v02_ser_disconnect

  This function processes incoming requests for the QMI sys_m
   services to disconnect a client. Sanitizing
  function.

  @param  : connection_handle: Client connection to
                               disconnect.
            service_cookie: Ignored

  @return : None.
*/
static void ssctl_v02_ser_disconnect(void  *connection_handle,
                                 void  *service_cookie)
{
   ssctl_v02_ser_client_type *client = connection_handle;

   if (ssctl_v02_ser_validate_connection_handle(connection_handle) == FALSE)
   {
      return;
   }

   if (client->connected== FALSE)
   {
      return;
   }

   /* Free client slot from being connected */
   client->connected = FALSE;
   TMS_MSG_HIGH("Client disconnected");
}

/**
  @brief ssctl_v02_qmi_ser_init

  This function initializes the QMI sys_m
  services handler.

  @param  : os_params: OS parameters.

  @return : qmi_csi_service_handle.
*/
static qmi_csi_service_handle ssctl_v02_qmi_ser_init(qmi_csi_os_params *os_params)
{
  qmi_idl_service_object_type ssctl_v02_qmi_ser_object = ssctl_get_service_object_v02();
  qmi_csi_options options;
  unsigned int ser_instance_id = SSCTL_SVC + SYSM_CHIP_ID*QMI_SSCTL_SERVICE_INSTANCE_OFFSET_V02;

  QMI_CSI_OPTIONS_INIT(options);
  QMI_CSI_OPTIONS_SET_INSTANCE_ID(options,ser_instance_id);

  if (qmi_csi_register_with_options(  ssctl_v02_qmi_ser_object, ssctl_v02_ser_connect,
                                      ssctl_v02_ser_disconnect, ssctl_v02_ser_process_req,
                                      NULL, os_params, &options, &ssctl_v02_qmi_ser_handler )
      != QMI_CSI_NO_ERR)
  {
    TMS_MSG_ERROR("QMI service error");
  } 
  else
  {
    TMS_MSG_HIGH("QMI service registration success");
  }

  return ssctl_v02_qmi_ser_handler;
}

/**
 *
 * @brief sys_m_qmi_task
 *
 * System Monitor main task
 *
 * @param param: Task init parameter
 *
*/
void sys_m_qmi_task(dword param)
{
  unsigned int mask_fired=0;
  qmi_csi_os_params os_params_v02;
  qmi_csi_service_handle service_handle_SSCTL_v02;

  memset(&os_params_v02, 0, sizeof(qmi_csi_os_params));
  os_params_v02.sig = SYS_M_QMI_v02_SIG;
  os_params_v02.signal = &qmi_ssctl_ser_sig;

  qurt_anysignal_init(&qmi_ssctl_ser_sig);

  service_handle_SSCTL_v02 = ssctl_v02_qmi_ser_init(&os_params_v02);
  //block for start signal
  rcinit_handshake_startup();    

  //task forever loop
  for (;;)
  {
    mask_fired = qurt_anysignal_wait(os_params_v02.signal, SYS_M_QMI_v02_SIG);
    if(mask_fired & SYS_M_QMI_v02_SIG )
    {
      qurt_anysignal_clear(os_params_v02.signal, SYS_M_QMI_v02_SIG);
      qmi_csi_handle_event(service_handle_SSCTL_v02, &os_params_v02);
    }
  }
}
