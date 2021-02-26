/*=============================================================================
  @file sns_client_manager_qmi.c

  Incoming requests to the Framework will arrive via the QMI Client Manager.
  From the Framework perspective, this module functions just as any other
  Sensor would, implementing and using the same Sensors API.  This module
  will translate incoming QMI request messages into Client Request objects,
  and similarly will translate Sensor Events into QMI indications.  This
  translation will be trivial, and this module will only concern itself with
  the requested batch period and the client processor.  The configuration
  request will be passed along to the appropriate Sensor, and this module
  will batch all data received (as appropriate).

  @note All QMI callbacks will be received within the same thread, and hence
  some thread protection is not necessary.  The CM Library mutex must be
  acquired prior to the internal QMI mutex(s).

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
//Qmi headers
#include "qmi_csi.h"
#include "qmi_idl_lib.h"

//Utilities and osa headers
#include "sns_assert.h"
#include "sns_printf_int.h"
#include "sns_types.h"

//Framework specific headers
#include "sns_fw_log.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_osa_lock.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_time.h"

//Client Manager Specific Headers
#include "sns_client_api_v01.h"
#include "sns_client_manager.h"
#include "sns_client_manager_instance.h"
#include "sns_client_manager_qmi.h"
#include "sns_mem_util.h"


/*=============================================================================
  Static Data
  ===========================================================================*/

// Incrementing client ID generator
static uint64_t client_id_next = 0;

//Maintains the count of currently active qmi connections on CM
static int32_t  cm_active_qmi_conn = 0;

/*=============================================================================
  QMI Static Function Definitions
  ===========================================================================*/

/**
 * A client has attempted to connect to this service.  Create an Instance
 * to service this client.
 *
 * @note Only the library lock is acquired, since we never access Sensor/library
 * state from within the CM Instance.
 *
 * @see qmi_csi_connect
 */
static qmi_csi_cb_error cm_connect_cb(
   qmi_client_handle qmi_handle,
   void              *service_cookie,
   void              **connection_handle)
{
  qmi_csi_cb_error rv = QMI_CSI_CB_NO_ERR;
  sns_sensor_instance *instance;
  sns_fw_sensor *this = (sns_fw_sensor*)service_cookie;
  sns_sensor *sensor = &this->sensor;
  SNS_PRINTF(LOW, sensor,  "Received connect callback from QMI (Client ID %" PRIu64 ")",
      client_id_next);

  if(SNS_CM_MAX_QMI_CONN_LIMIT < cm_active_qmi_conn)
  {
    SNS_PRINTF(ERROR, sensor, "Unable to accept the QMI connection. Active conns %d",
        cm_active_qmi_conn);
    rv = QMI_CSI_CB_CONN_REFUSED;
  }
  else
  {
    instance = sensor->cb->create_instance(sensor, sizeof(sns_cm_inst_state));

    if(NULL != instance)
    {
      sns_cm_inst_state *client = (sns_cm_inst_state*)instance->state->state;

      client->qmi_handle = qmi_handle;
      client->client_id = client_id_next++;

      SNS_ASSERT(UINT64_MAX != client_id_next);

      *connection_handle = instance;
      cm_active_qmi_conn += 1;
      SNS_PRINTF(LOW, sensor, "Instance created  for QMI (Client ID %d) QMI Conns %d",
          (uint32_t)(client_id_next), cm_active_qmi_conn);

    }
    else
    {
      SNS_PRINTF(ERROR, sensor, "Unable to create client instance");
      rv = QMI_CSI_CB_CONN_REFUSED;
    }

  }
  return rv;
}

/**
 * A previously connected client has disconnected.  Remove the associated
 * Instance, and clean-up all active data streams.
 *
 * @see qmi_csi_disconnect
 */
static void cm_disconnect_cb(
   void *connection_handle,
   void *service_cookie)
{
  if(NULL == connection_handle || NULL == service_cookie)
  {
    //Need to return here as we can't print anything
    //as the service cookie gives access to sensor.
    return;
  }
  else
  {
    sns_fw_sensor_instance *instance = (sns_fw_sensor_instance*)connection_handle;
    sns_sensor_instance *s_instance = (sns_sensor_instance*)&instance->instance;
    sns_fw_sensor *this = (sns_fw_sensor*)service_cookie;
    sns_sensor *sensor = &this->sensor;
    sns_cm_inst_state *cm_instance = (sns_cm_inst_state*)s_instance->state->state;
    SNS_PRINTF(LOW, sensor, "Received disconnect callback from QMI (Client ID %d)",
        cm_instance->client_id);

    SNS_PRINTF(MED, sensor, "csi_disconnect_cb - conn_handle " SNS_DIAG_PTR
        ", cookie ptr " SNS_DIAG_PTR, connection_handle, service_cookie);

    this->sensor.cb->remove_instance(&instance->instance);
    cm_active_qmi_conn -= 1;
    SNS_PRINTF(MED, sensor, "csi_disconnect_cb active QMI conns %d", cm_active_qmi_conn);
  }
}

/**
 * Send a response to the client.
 *
 * @param[i] client_id
 * @param[i] req_handle QMI Handle as received in request_cb
 * @param[i] result Return code to place in the response message
 * @param[i] sensor data source associated with the stream
 *
 * @return true on success, false otherwise
 */
static bool cm_send_resp(
   uint64_t client_id,
   qmi_req_handle req_handle,
   sns_rc result,
   sns_sensor const *sensor)
{
  qmi_csi_error qmi_err;

  sns_client_resp_msg_v01 resp;
  sns_memset(&resp, 0, sizeof(sns_client_resp_msg_v01));

  resp.client_id_valid = true;
  resp.client_id = client_id;
  resp.result_valid = true;
  resp.result = result;

  qmi_err = qmi_csi_send_resp(req_handle, SNS_CLIENT_RESP_V01,
      &resp, sizeof(resp));

  SNS_PRINTF(LOW, sensor, "Sent resp; handle " SNS_DIAG_PTR ", msg_id %i,qmi err %i",
      req_handle, SNS_CLIENT_RESP_V01, qmi_err);
  SNS_PRINTF(LOW, sensor, "Sent resp; handle " SNS_DIAG_PTR ", msg_id %i,result %i",
      req_handle, SNS_CLIENT_RESP_V01, result);
  return qmi_err == QMI_CSI_NO_ERR;
}

/**
 * Handle a new request from a client.  This request may be the first for a
 * specific SUID, and hence results in the creation of a new data stream.
 * Otherwise, send the request on the appropriate existing stream.
 *
 * @see qmi_csi_process_req
 */
static qmi_csi_cb_error cm_request_cb(
  void           *connection_handle,
  qmi_req_handle req_handle,
  unsigned int   msg_id,
  void           *req_c_struct,
  unsigned int   req_c_struct_len,
  void           *service_cookie)
{
  UNUSED_VAR(req_c_struct_len);
  sns_sensor_instance *instance = (sns_sensor_instance*)connection_handle;
  qmi_csi_cb_error rv = QMI_CSI_CB_UNSUPPORTED_ERR;
  sns_rc rc = SNS_RC_SUCCESS;
  sns_sensor *sensor = &((sns_fw_sensor*)service_cookie)->sensor;
  sns_cm_inst_state *client = (sns_cm_inst_state*)instance->state->state;

  SNS_PRINTF(LOW, sensor, "Received request %i", msg_id);
  SNS_ASSERT(NULL != instance);

  rc = sensor->instance_api->set_client_config(instance,
      &((sns_request){ .request = req_c_struct, .request_len = req_c_struct_len }));
  if(SNS_RC_SUCCESS != rc)
  {
    sns_fw_log_client_api_resp(instance, sensor,
        cm_get_sensor_uid(NULL), client->client_id, rc);
    SNS_PRINTF(ERROR, sensor, "Set client config failed");
  }
  if(cm_send_resp(client->client_id, req_handle, rc, sensor))
    rv = QMI_CSI_CB_NO_ERR;
  else
    rv = QMI_CSI_CB_CONN_REFUSED;

  return rv;
}

/**
 * Handles "resume ind" callback from QMI.  If a client is not processing
 * indications, such that a number are pending in the client's queue, halt
 * sending further indications until the queue is processed.  This callback
 * serves as this notification.
 *
 * @see qmi_csi_resume_ind
 */
static void cm_resume_ind_cb(
    qmi_client_handle client_handle,
    void              *connection_handle,
    void              *service_cookie)
{
  UNUSED_VAR(client_handle);
  sns_fw_sensor *this = (sns_fw_sensor*)service_cookie;
  sns_sensor *sensor = &this->sensor;
  sns_osa_lock_acquire(this->library->library_lock);
#if ENABLE_DEBUG_PRINT
  SNS_PRINTF(ERROR, sensor, "before csi_resume_cb - conn_handle " SNS_DIAG_PTR
      ", cookie ptr " SNS_DIAG_PTR, connection_handle, service_cookie);
#endif
  sns_cm_handle_client_resume((sns_fw_sensor_instance*)connection_handle);
  SNS_PRINTF(HIGH, sensor, " after csi_resume_cb - conn_handle " SNS_DIAG_PTR
      ", cookie ptr " SNS_DIAG_PTR, connection_handle, service_cookie);
  sns_osa_lock_release(this->library->library_lock);
}

/*=============================================================================
  QMI Public Function Definitions
  ===========================================================================*/

/**
 * Register a service with the QMI infrastructure.
 *
 *@param[i] sensor The CM sensor pointer.
 * @return
 * SNS_RC_FAILED - QMI registration error
 * SNS_RC_SUCCESS
 */
sns_rc sns_cm_service_register(sns_sensor *sensor)
{
  sns_cm_state *state = (sns_cm_state*)sensor->state->state;
  qmi_csi_error qmi_err = QMI_CSI_NO_ERR;
  qmi_csi_options qmi_options;

  QMI_CSI_OPTIONS_INIT(qmi_options);
  QMI_CSI_OPTIONS_SET_INSTANCE_ID(qmi_options, 0);
  QMI_CSI_OPTIONS_SET_RESUME_IND_CB(qmi_options, &cm_resume_ind_cb);
  QMI_CSI_OPTIONS_SET_MAX_OUTSTANDING_INDS(qmi_options, 10);
  // PEND: Use QMI_CSI_OPTIONS_SET_LOG_MSG_CB

  qmi_err = qmi_csi_register_with_options(SNS_CLIENT_SVC_get_service_object_v01(),
      &cm_connect_cb, &cm_disconnect_cb, &cm_request_cb, sensor, &state->os_params,
      &qmi_options, &state->service_provider);

  if(QMI_CSI_NO_ERR != qmi_err)
  {
    SNS_PRINTF(ERROR, sensor, "qmi_csi_register_with_options error %i", qmi_err);
    return SNS_RC_FAILED;
  }

  SNS_PRINTF(LOW, sensor, "Register with QMI is succesful %i", qmi_err);
  return SNS_RC_SUCCESS;
}
