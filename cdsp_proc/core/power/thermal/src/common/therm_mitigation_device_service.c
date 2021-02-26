/*============================================================================
  @file therm_mitigation_device_service.c

  Thermal mitigation device QMI service implementation

  Copyright (c) 2012-2017 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/thermal/src/common/therm_mitigation_device_service.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "therm_log.h"
#include "qmi_csi.h"
#include "thermal_mitigation_device_service_v01.h"
#include "npa.h"
#include "npa_resource.h"
#include <stringl/stringl.h>
#include "timer.h"
#include "therm_v.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define THERM_QMI_MITIGATION_MAX_CLIENT_NUM 8

typedef struct
{
  unsigned int  device_index;
  void         *client;
} therm_qmi_mitigation_dev_cb_type;

extern const unsigned int therm_mitigation_devices_count;

typedef struct
{
  qmi_client_handle  qmi_client_handle;
  npa_event_handle   event_handle[QMI_TMD_MITIGATION_DEV_LIST_MAX_V01];
  npa_client_handle  client_handle[QMI_TMD_MITIGATION_DEV_LIST_MAX_V01];
  npa_query_handle   query_handle[QMI_TMD_MITIGATION_DEV_LIST_MAX_V01];
  unsigned int       requested_level[QMI_TMD_MITIGATION_DEV_LIST_MAX_V01];
  boolean            connected;
  therm_qmi_mitigation_dev_cb_type  cb_context[QMI_TMD_MITIGATION_DEV_LIST_MAX_V01];
} therm_qmi_mitigation_dev_client_type;

static therm_qmi_mitigation_dev_client_type therm_qmi_mitigation_dev_client[THERM_QMI_MITIGATION_MAX_CLIENT_NUM];

static qmi_csi_service_handle therm_qmi_dev_service_handler = NULL;

typedef union {
  qmi_response_type_v01                                     resp;
  tmd_get_mitigation_device_list_resp_msg_v01               dev_list;
  tmd_set_mitigation_level_resp_msg_v01                     set_level;
  tmd_get_mitigation_level_resp_msg_v01                     get_level;
  tmd_register_notification_mitigation_level_resp_msg_v01   reg_notify;
  tmd_deregister_notification_mitigation_level_resp_msg_v01 dereg_notify;
} therm_qmi_general_resp_v01_type;

/* Cold restriction timeout */
static timer_type       cold_timer;
static timer_group_type cold_timer_group;
static const timetick_type cold_timeout_ms = 30000;
static boolean cold_timeout_disable;
static npa_client_handle  cold_client_handle;

/* Allocate response an indication message bodies outside of stack space */
static therm_qmi_general_resp_v01_type therm_qmi_resp;

static tmd_mitigation_level_report_ind_msg_v01 therm_qmi_ind_msg;

extern const char *therm_mitigation_devices[];

extern npa_node_definition therm_mitigate_node;

static boolean therm_qmi_validate_connection_handle(void *connection_handle);

/*===========================================================================

                     INTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/**
  @brief therm_qmi_dev_send_indication_cb

  This function handles NPA event changes for mitigation resources.  Sends
  indications to registered connections.  Sanitizing function.

  @param  : context: User callback data provided to notify the correct qmi
                     connection
            event_type: Ignored
            data: New mitigation state information.
            data_size: Ignored

  @return : None.
*/
static void therm_qmi_dev_send_indication_cb( void         *context,
                                              unsigned int  event_type,
                                              void         *data,
                                              unsigned int  data_size )
{
  therm_qmi_mitigation_dev_cb_type     *cb_data = NULL;
  therm_qmi_mitigation_dev_client_type *client  = NULL;
  npa_event_data *event_data_ptr                = NULL;
  qmi_csi_error qmi_error;

  if (context == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "indication_cb: NULL context param");
    return;
  }

  if (data == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "indication_cb: NULL data param");
    return;
  }

  cb_data = (therm_qmi_mitigation_dev_cb_type *)context;
  client = (therm_qmi_mitigation_dev_client_type *)cb_data->client;


  if (therm_qmi_validate_connection_handle(cb_data->client) == FALSE)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "indication_cb: Invalid client handle.");
    return;
  }

  event_data_ptr = (npa_event_data*)data;

  if (cb_data->device_index >= therm_mitigation_devices_count)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                     "indication_cb: Invalid index %d",
                     cb_data->device_index);
    return;
  }

  therm_qmi_ind_msg.current_mitigation_level = event_data_ptr->state;
  strlcpy(therm_qmi_ind_msg.mitigation_device.mitigation_dev_id,
          therm_mitigation_devices[cb_data->device_index],
          sizeof(therm_qmi_ind_msg.mitigation_device.mitigation_dev_id));

  qmi_error = qmi_csi_send_ind(client->qmi_client_handle,
                               QMI_TMD_MITIGATION_LEVEL_REPORT_IND_V01,
                               &therm_qmi_ind_msg,
                               sizeof(tmd_mitigation_level_report_ind_msg_v01));
  if (qmi_error != QMI_CSI_NO_ERR)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                     "Failure to send indication, Err: %d",
                     qmi_error);
  }
  else
  {
    therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                     "Indication sent %s change to %d",
                     therm_mitigation_devices[cb_data->device_index],
                     event_data_ptr->state);
  }
}

/**
  @brief therm_qmi_validate_request

  This function validates a request size and the device id.

  @param  : response: qmi_response_type_v01
            idx: message index
            req_c_struct_len: request struct
                                       length
            expected_req_c_struct_len: expected request struct
            length dev_id: Device ID log_str: Unique log string

  @return : TRUE if valid, FALSE otherwise
*/
static boolean therm_qmi_validate_request( qmi_response_type_v01  *response,
                                           unsigned int           *idx,
                                           unsigned int            req_c_struct_len,
                                           unsigned int            expected_req_c_struct_len,
                                           const char             *dev_id,
                                           const char             *log_str)
{
  unsigned int i;
  boolean retval = TRUE;

  do
  {
    /* Validate request. */
    if (req_c_struct_len != expected_req_c_struct_len)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s request wrong size: %d",
                       log_str, req_c_struct_len);
      response->result = QMI_RESULT_FAILURE_V01;
      response->error = QMI_ERR_MALFORMED_MSG_V01;
      retval = FALSE;
      break;
    }

    /* Does cooling device exist. */
    for (i = 0; i < therm_mitigation_devices_count; i++ )
    {
      if (strcasecmp(therm_mitigation_devices[i], dev_id) == 0)
      {
        /* Found a match */
        *idx = i;
        break;
      }
    }

    if (i >= therm_mitigation_devices_count)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                       "%s request on non-existent thermal device",
                       log_str);
      response->result = QMI_RESULT_FAILURE_V01;
      response->error = QMI_ERR_INVALID_ID_V01;
      retval = FALSE;
      break;
    }
  } while (0);

  return retval;
}

/**
  @brief therm_qmi_validate_connection_handle.

  This function validates a connection handle.

  @param  : connection_handle: Handle to validate.

  @return : TRUE if valid, FALSE otherwise
*/
static boolean therm_qmi_validate_connection_handle(void *connection_handle)
{
  unsigned int i;
  boolean retval = TRUE;

  do
  {
    /* Validate handle. */
    if (connection_handle == NULL)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Connection handle NULL");
      retval = FALSE;
      break;
    }

    /* Find empty client slot */
    for (i = 0; i < THERM_QMI_MITIGATION_MAX_CLIENT_NUM; i++)
    {
      if (&therm_qmi_mitigation_dev_client[i] ==
          (therm_qmi_mitigation_dev_client_type *)connection_handle)
      {
        /* Found client */
        break;
      }
    }

    if (i >= THERM_QMI_MITIGATION_MAX_CLIENT_NUM)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid connection handle");
      retval = FALSE;
      break;
    }

  } while (0);

  return retval;
}


/**
  @brief therm_qmi_dev_connect

  This function processes incoming requests for the QMI thermal
  mitigation device services to connect a client.

  @param  : client_handle: Handle used by the infrastructure
                           to identify different services.
            service_cookie: Ignored
            connection_handle: Client connection to created
                               internally.

  @return : None.
*/
static qmi_csi_cb_error therm_qmi_dev_connect(qmi_client_handle  client_handle,
                                              void              *service_cookie,
                                              void             **connection_handle)
{
  unsigned int i;
  qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
  therm_qmi_mitigation_dev_client_type *client = NULL;

  do
  {
    /* Find empty client slot */
    for (i = 0; i < THERM_QMI_MITIGATION_MAX_CLIENT_NUM; i++)
    {
      if (therm_qmi_mitigation_dev_client[i].connected == FALSE)
      {
        /* Found empty slot */
        therm_log_printf(THERM_LOG_LEVEL_INFO, 1, "Fould empty client slot: %d", i);
        break;
      }
    }

    if (i >= THERM_QMI_MITIGATION_MAX_CLIENT_NUM)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "No client connections available");
      retval = QMI_CSI_CB_NO_MEM;
      break;
    }

    /* Fill out slot data */
    memset(&therm_qmi_mitigation_dev_client[i], 0, sizeof(therm_qmi_mitigation_dev_client[0]));
    client = &therm_qmi_mitigation_dev_client[i];

    for (i = 0; i < therm_mitigate_node.resource_count; i++)
    {
      client->client_handle[i] = npa_create_sync_client(therm_mitigate_node.resources[i].name,
                                                       "QMI MITIGATION DEV",
                                                       NPA_CLIENT_REQUIRED);
      client->query_handle[i] = npa_create_query_handle(therm_mitigate_node.resources[i].name);
    }

    client->qmi_client_handle = client_handle;
    client->connected = TRUE;
    *connection_handle = client;
    therm_log_printf(THERM_LOG_LEVEL_INFO, 0, "Connect client");
  } while (0);

  return retval;
}

/**
  @brief therm_qmi_dev_disconnect

  This function processes incoming requests for the QMI thermal
  mitigation device services to disconnect a client. Sanitizing
  function.

  @param  : connection_handle: Client connection to
                               disconnect.
            service_cookie: Ignored

  @return : None.
*/
static void therm_qmi_dev_disconnect(void  *connection_handle,
                                     void  *service_cookie)
{
  unsigned int i;
  therm_qmi_mitigation_dev_client_type *client = connection_handle;

  if (therm_qmi_validate_connection_handle(client) == FALSE)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "Disconnect invalid client");
    return;
  }

  /* Destroy NPA handles */
  for (i = 0; i < therm_mitigate_node.resource_count; i++)
  {
    if (client->event_handle[i] != NULL)
    {
      npa_destroy_event_handle(client->event_handle[i]);
      client->event_handle[i] = NULL;
    }

    if (client->client_handle[i] != NULL)
    {
      npa_destroy_client(client->client_handle[i]);
      client->client_handle[i] = NULL;
    }

    if (client->query_handle[i])
    {
      npa_destroy_query_handle(client->query_handle[i]);
      client->query_handle[i] = NULL;
    }
  }

  /* Free client slot from being connected */
  client->connected = FALSE;
  therm_log_printf(THERM_LOG_LEVEL_INFO, 0, "Disconnect client");

}

/**
  @brief therm_handle_dev_list_req

  This function processes incoming requests for the QMI thermal
  mitigation device services with msg_id
  QMI_TMD_GET_MITIGATION_DEVICE_LIST_REQ_V01.

  @param  : connection_handle: Client connection making request.
            req_handle: QMI request handle
            msg_id: QMI msg request ID
            req_c_struct: QMI request structure
            req_c_struct_len: QMI request structure length
            service_cookie: Ignored

  @return : qmi_csi_cb_error.
*/
static qmi_csi_cb_error therm_handle_dev_list_req(void           *connection_handle,
                                                  qmi_req_handle  req_handle,
                                                  unsigned int    msg_id,
                                                  void           *req_c_struct,
                                                  unsigned int    req_c_struct_len,
                                                  void           *service_cookie)
{
  tmd_get_mitigation_device_list_resp_msg_v01 *response =
    (tmd_get_mitigation_device_list_resp_msg_v01 *)&therm_qmi_resp;
  unsigned int i;

  /* Init response data */
  memset( response, 0,
          sizeof(tmd_get_mitigation_device_list_resp_msg_v01));

  do
  {
    response->mitigation_device_list_valid = 1;
    response->mitigation_device_list_len = therm_mitigation_devices_count;

    therm_log_printf(THERM_LOG_LEVEL_INFO, 0,
                     "Get device list request");

    /* Copy cooling device info over to response. */
    for (i = 0; i < therm_mitigation_devices_count; i++ )
    {
      therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                     "Device %d %s", i, therm_mitigation_devices[i]);
      strlcpy(response->mitigation_device_list[i].mitigation_dev_id.mitigation_dev_id,
              therm_mitigation_devices[i],
              sizeof(response->mitigation_device_list[0].mitigation_dev_id));
      response->mitigation_device_list[i].max_mitigation_level = therm_mitigate_node.resources[i].max;
    }

    response->resp.result = QMI_RESULT_SUCCESS_V01;
    response->resp.error  = QMI_ERR_NONE_V01;
  } while (0);

  qmi_csi_send_resp(req_handle, msg_id, response,
                    sizeof(tmd_get_mitigation_device_list_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/**
  @brief therm_handle_dev_get_level_req

  This function processes incoming requests for the QMI thermal
  mitigation device services with msg_id
  QMI_TMD_GET_MITIGATION_LEVEL_REQ_V01.

  @param  : connection_handle: Client connection making request.
            req_handle: QMI request handle
            msg_id: QMI msg request ID
            req_c_struct: QMI request structure
            req_c_struct_len: QMI request structure length
            service_cookie: Ignored

  @return : qmi_csi_cb_error.
*/
static qmi_csi_cb_error therm_handle_dev_get_level_req(void           *connection_handle,
                                                       qmi_req_handle  req_handle,
                                                       unsigned int    msg_id,
                                                       void           *req_c_struct,
                                                       unsigned int    req_c_struct_len,
                                                       void           *service_cookie)
{
  tmd_get_mitigation_level_req_msg_v01  *request =
    (tmd_get_mitigation_level_req_msg_v01 *)req_c_struct;
  tmd_get_mitigation_level_resp_msg_v01 *response =
    (tmd_get_mitigation_level_resp_msg_v01 *)&therm_qmi_resp;
  therm_qmi_mitigation_dev_client_type *client = connection_handle;
  npa_query_type query_result;

  unsigned int i;

  /* Init response data */
  memset( response, 0, sizeof(tmd_get_mitigation_level_resp_msg_v01) );

  do
  {
    /* Validate request. */
    if (therm_qmi_validate_request(&(response->resp), &i, req_c_struct_len,
                                   sizeof(tmd_get_mitigation_level_req_msg_v01),
                                   request->mitigation_device.mitigation_dev_id,
                                   "Get level") == FALSE)
    {
      break;
    }

    if (npa_query(client->query_handle[i], NPA_QUERY_CURRENT_STATE,
                  &query_result) == NPA_QUERY_SUCCESS)
    {
      response->current_mitigation_level_valid = 1;
      response->current_mitigation_level = (uint8_t)query_result.data.state;
    }
    response->requested_mitigation_level_valid = 1;
    response->requested_mitigation_level = (uint8_t)client->requested_level[i];

    therm_log_printf(THERM_LOG_LEVEL_INFO, 3,
                     "Get level request on %s, current level %d, requested level %d",
                     therm_mitigate_node.resources[i].name,
                     query_result.data.state,
                     client->requested_level[i]);

    response->resp.result = QMI_RESULT_SUCCESS_V01;
    response->resp.error  = QMI_ERR_NONE_V01;
  } while (0);

  qmi_csi_send_resp(req_handle, msg_id, response,
                    sizeof(tmd_get_mitigation_level_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/**
  @brief therm_handle_dev_set_level_req

  This function processes incoming requests for the QMI thermal
  mitigation device services with msg_id
  QMI_TMD_SET_MITIGATION_LEVEL_REQ_V01.

  @param  : connection_handle: Client connection making request.
            req_handle: QMI request handle
            msg_id: QMI msg request ID
            req_c_struct: QMI request structure
            req_c_struct_len: QMI request structure length
            service_cookie: Ignored

  @return : qmi_csi_cb_error.
*/
static qmi_csi_cb_error therm_handle_dev_set_level_req(void           *connection_handle,
                                                       qmi_req_handle  req_handle,
                                                       unsigned int    msg_id,
                                                       void           *req_c_struct,
                                                       unsigned int    req_c_struct_len,
                                                       void           *service_cookie)
{
  tmd_set_mitigation_level_req_msg_v01  *request =
    (tmd_set_mitigation_level_req_msg_v01 *)req_c_struct;
  tmd_set_mitigation_level_resp_msg_v01 *response =
    (tmd_set_mitigation_level_resp_msg_v01 *)&therm_qmi_resp;
  therm_qmi_mitigation_dev_client_type *client = connection_handle;

  unsigned int i;

  /* Init response data */
  memset( response, 0, sizeof(tmd_set_mitigation_level_resp_msg_v01) );

  do
  {
    /* Validate request. */
    if (therm_qmi_validate_request(&(response->resp), &i, req_c_struct_len,
                                   sizeof(tmd_set_mitigation_level_req_msg_v01),
                                   request->mitigation_dev_id.mitigation_dev_id,
                                   "Set level") == FALSE)
    {
      break;
    }

    client->requested_level[i] = request->mitigation_level;
    npa_issue_required_request(client->client_handle[i],
                               client->requested_level[i]);

    therm_log_printf(THERM_LOG_LEVEL_INFO, 2,
                     "Set level request on %s of %d",
                     therm_mitigate_node.resources[i].name,
                     client->requested_level[i]);

    response->resp.result = QMI_RESULT_SUCCESS_V01;
    response->resp.error  = QMI_ERR_NONE_V01;
    /* Cancel cold restriction timeout if dev_id is "cpuv_restriction_cold" */
    if ((cold_timeout_disable == FALSE) &&
        (strcasecmp("cpuv_restriction_cold",
                    request->mitigation_dev_id.mitigation_dev_id) == 0))
    {
      /* Cancel cold restriction timeout */
      timer_clr_64(&cold_timer, T_NONE);
      cold_timeout_disable = TRUE;
      therm_log_printf(THERM_LOG_LEVEL_INFO, 0, "Cancel cold restriction timeout");
      break;
    }
  } while (0);

  qmi_csi_send_resp(req_handle, msg_id, response,
                    sizeof(tmd_set_mitigation_level_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/**
  @brief therm_handle_dev_reg_notify

  This function processes incoming requests for the QMI thermal
  mitigation device services with msg_id
  QMI_TMD_REGISTER_NOTIFICATION_MITIGATION_LEVEL_REQ_V01.

  @param  : connection_handle: Client connection making request.
            req_handle: QMI request handle
            msg_id: QMI msg request ID
            req_c_struct: QMI request structure
            req_c_struct_len: QMI request structure length
            service_cookie: Ignored

  @return : qmi_csi_cb_error.
*/
static qmi_csi_cb_error therm_handle_dev_reg_notify(void           *connection_handle,
                                                    qmi_req_handle  req_handle,
                                                    unsigned int    msg_id,
                                                    void           *req_c_struct,
                                                    unsigned int    req_c_struct_len,
                                                    void           *service_cookie)
{
  tmd_register_notification_mitigation_level_req_msg_v01  *request =
    (tmd_register_notification_mitigation_level_req_msg_v01 *)req_c_struct;
  tmd_register_notification_mitigation_level_resp_msg_v01 *response =
    (tmd_register_notification_mitigation_level_resp_msg_v01 *)&therm_qmi_resp;
  therm_qmi_mitigation_dev_client_type *client = connection_handle;

  unsigned int i;

  /* Init response data */
  memset( response, 0,
          sizeof(tmd_register_notification_mitigation_level_resp_msg_v01) );

  do
  {
    /* Validate request. */
    if (therm_qmi_validate_request(&(response->resp), &i, req_c_struct_len,
                                   sizeof(tmd_register_notification_mitigation_level_req_msg_v01),
                                   request->mitigation_device.mitigation_dev_id,
                                   "Register notify") == FALSE)
    {
      break;
    }

    /* Store data needed to handle chagne event correctly */
    client->cb_context[i].device_index = i;
    client->cb_context[i].client = client;

    client->event_handle[i] =
      npa_create_change_event_cb( therm_mitigate_node.resources[i].name,
                                  "Mitigation Notify",
                                  therm_qmi_dev_send_indication_cb,
                                  (void *) &client->cb_context[i]);

    if (client->event_handle[i] == NULL)
    {
      response->resp.result = QMI_RESULT_FAILURE_V01;
      response->resp.error = QMI_ERR_INTERNAL_V01;
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                       "Failed to register change event handle for %s",
                       therm_mitigate_node.resources[i].name);
      break;
    }

    therm_log_printf(THERM_LOG_LEVEL_INFO, 1,
                     "Registered change event handle for %s",
                     therm_mitigate_node.resources[i].name);

    response->resp.result = QMI_RESULT_SUCCESS_V01;
    response->resp.error  = QMI_ERR_NONE_V01;

  } while (0);

  qmi_csi_send_resp(req_handle, msg_id, response,
                    sizeof(tmd_register_notification_mitigation_level_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}

/**
  @brief therm_handle_dev_dereg_notify

  This function processes incoming requests for the QMI thermal
  mitigation device services with msg_id
  QMI_TMD_DEREGISTER_NOTIFICATION_MITIGATION_LEVEL_REQ_V01.

  @param  : connection_handle: Client connection making request.
            req_handle: QMI request handle
            msg_id: QMI msg request ID
            req_c_struct: QMI request structure
            req_c_struct_len: QMI request structure length
            service_cookie: Ignored

  @return : qmi_csi_cb_error.
*/
static qmi_csi_cb_error therm_handle_dev_dereg_notify(void           *connection_handle,
                                                      qmi_req_handle  req_handle,
                                                      unsigned int    msg_id,
                                                      void           *req_c_struct,
                                                      unsigned int    req_c_struct_len,
                                                      void           *service_cookie)
{
  tmd_deregister_notification_mitigation_level_req_msg_v01  *request =
    (tmd_deregister_notification_mitigation_level_req_msg_v01 *)req_c_struct;
  tmd_deregister_notification_mitigation_level_resp_msg_v01 *response =
    (tmd_deregister_notification_mitigation_level_resp_msg_v01 *)&therm_qmi_resp;
  therm_qmi_mitigation_dev_client_type *client = connection_handle;

  unsigned int i;

  /* Init response data */
  memset( response, 0,
          sizeof(tmd_deregister_notification_mitigation_level_resp_msg_v01) );

  do
  {
    /* Validate request. */
    if (therm_qmi_validate_request(&(response->resp), &i, req_c_struct_len,
                                   sizeof(tmd_deregister_notification_mitigation_level_req_msg_v01),
                                   request->mitigation_device.mitigation_dev_id,
                                   "De-register notify") == FALSE)
    {
      break;
    }

    if (client->event_handle[i] !=  NULL)
    {
      npa_destroy_event_handle(client->event_handle[i]);
      client->event_handle[i] = NULL;
    }

    therm_log_printf(THERM_LOG_LEVEL_INFO, 1,
                     "De-register change event handle for %s",
                     therm_mitigate_node.resources[i].name);

    response->resp.result = QMI_RESULT_SUCCESS_V01;
    response->resp.error  = QMI_ERR_NONE_V01;

  } while (0);

  qmi_csi_send_resp(req_handle, msg_id, response,
                    sizeof(tmd_deregister_notification_mitigation_level_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}


/**
  @brief therm_qmi_dev_process_req

  This function processes incoming requests for the QMI thermal
  mitigation device services handler. Sanitizing function.
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
static qmi_csi_cb_error therm_qmi_dev_process_req(void           *connection_handle,
                                                  qmi_req_handle  req_handle,
                                                  unsigned int    msg_id,
                                                  void           *req_c_struct,
                                                  unsigned int    req_c_struct_len,
                                                  void           *service_cookie)
{
  qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
  qmi_response_type_v01 *response = (qmi_response_type_v01 *)&therm_qmi_resp;

  if ((therm_qmi_validate_connection_handle(connection_handle) == FALSE) ||
      (req_handle == NULL))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                     "Invalid request arguments request id: 0x%x", msg_id);
    response->result = QMI_RESULT_FAILURE_V01;
    response->error  = QMI_ERR_INVALID_MESSAGE_ID_V01;
    return retval;
  }

  switch(msg_id)
  {
    case QMI_TMD_GET_MITIGATION_DEVICE_LIST_REQ_V01:
      retval = therm_handle_dev_list_req(connection_handle, req_handle, msg_id,
                                         req_c_struct, req_c_struct_len,
                                         service_cookie);
      break;
    case QMI_TMD_SET_MITIGATION_LEVEL_REQ_V01:
      retval = therm_handle_dev_set_level_req(connection_handle, req_handle,  msg_id,
                                              req_c_struct, req_c_struct_len,
                                              service_cookie);
      break;
    case QMI_TMD_GET_MITIGATION_LEVEL_REQ_V01:
      retval = therm_handle_dev_get_level_req(connection_handle, req_handle, msg_id,
                                              req_c_struct, req_c_struct_len,
                                              service_cookie);
      break;
    case QMI_TMD_REGISTER_NOTIFICATION_MITIGATION_LEVEL_REQ_V01:
      retval = therm_handle_dev_reg_notify(connection_handle, req_handle, msg_id,
                                           req_c_struct, req_c_struct_len,
                                           service_cookie);
      break;
    case QMI_TMD_DEREGISTER_NOTIFICATION_MITIGATION_LEVEL_REQ_V01:
      retval = therm_handle_dev_dereg_notify(connection_handle, req_handle, msg_id,
                                             req_c_struct, req_c_struct_len,
                                             service_cookie);
      break;
    default:
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "Unhandled mitigation request id: 0x%x",
                       msg_id);
      response->result = QMI_RESULT_FAILURE_V01;
      response->error  = QMI_ERR_INVALID_MESSAGE_ID_V01;
      qmi_csi_send_resp(req_handle, msg_id, response, sizeof(qmi_response_type_v01));
      break;
  }
  return retval;
}

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/**
  @brief therm_qmi_dev_service_init

  This function initializes the QMI thermal mitigation device
  services handler.

  @param  : os_params: OS parameters.

  @return : qmi_csi_service_handle.
*/
qmi_csi_service_handle therm_qmi_dev_service_init(qmi_csi_os_params *os_params,
                                                  qurt_signal_t     *qurt_signal)
{
  qmi_csi_options             therm_qmi_dev_options;
  qmi_idl_service_object_type therm_dev_service_object = tmd_get_service_object_v01();

  QMI_CSI_OPTIONS_INIT(therm_qmi_dev_options);
  QMI_CSI_OPTIONS_SET_INSTANCE_ID(therm_qmi_dev_options, THERMAL_QMI_INSTANCE);
  QMI_CSI_OPTIONS_SET_MAX_OUTSTANDING_INDS(therm_qmi_dev_options,
                                           THERM_QMI_MITIGATION_MAX_CLIENT_NUM);

  if (qmi_csi_register_with_options(therm_dev_service_object, therm_qmi_dev_connect,
                                    therm_qmi_dev_disconnect, therm_qmi_dev_process_req,
                                    NULL, os_params, &therm_qmi_dev_options,
                                    &therm_qmi_dev_service_handler) != QMI_CSI_NO_ERR)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "QMI mitigation service error");
  }
  else
  {
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 0, "QMI mitigation service register okay");

    /* Create NPA client to disable cold restriction after timeout */
    cold_client_handle = npa_create_sync_client("/therm/mitigate/cpuv_restriction_cold",
                                                "cold_timeout", NPA_CLIENT_REQUIRED);
    if (cold_client_handle != NULL)
    {
      /* Set cold restriction timeout timer */
      timer_def_osal(&cold_timer, &cold_timer_group, TIMER_NATIVE_OS_SIGNAL_TYPE,
                     qurt_signal, THERM_QMI_TMD_COLD_RSTR_TIMER_SIG);
      /* Start the sensor sampling polling timer. */
      timer_set_64( &cold_timer, cold_timeout_ms, 0, T_MSEC);
    }
  }

  return therm_qmi_dev_service_handler;
}

/**
  @brief therm_qmi_dev_service_process_sigs

  Process rex signals for qmi dev service.

  @param rcvdSigs
*/
void therm_qmi_dev_service_process_sigs(unsigned int  rcvdSigs,
                                        qurt_signal_t *qurt_signal)
{
  if (rcvdSigs & THERM_QMI_TMD_COLD_RSTR_TIMER_SIG)
  {
    qurt_signal_clear(qurt_signal, THERM_QMI_TMD_COLD_RSTR_TIMER_SIG);
    if (cold_timeout_disable == FALSE)
    {
      npa_cancel_request(cold_client_handle);
      therm_log_printf(THERM_LOG_LEVEL_INFO, 0, "Cold restriction timeout");
      cold_timeout_disable = TRUE;
    }
  }
}
