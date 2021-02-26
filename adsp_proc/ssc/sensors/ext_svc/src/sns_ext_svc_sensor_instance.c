/*=============================================================================
  @file sns_ext_svc_sensor_instance.c

  External Service sensor instance implementation

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include "sns_assert.h"
#include "sns_event_service.h"
#include "sns_ext_svc.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_printf.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_time.h"
#include "sns_types.h"

#include "pb.h"
#include "pb_encode.h"
#include "qmi_client.h"
#include "sns_pb_util.h"

#include "location_service_v02.h"
#include "sns_ext_svc.pb.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/* QMI response callback data */
typedef struct sns_ext_svc_resp_cb_data
{
  uint32_t transaction_id;
  sns_sensor_instance *instance;
} sns_ext_svc_resp_cb_data;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sensor_instance_api::notify_event */
static sns_rc
inst_notify_event(sns_sensor_instance* const this)
{
  UNUSED_VAR(this);
  // This Sensor Instance has no event input
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::init */
static sns_rc
inst_init(sns_sensor_instance* const this, sns_sensor_state const *state)
{
  sns_ext_svc_sensor_state *sensor_state = (sns_ext_svc_sensor_state*)state->state;
  sns_ext_svc_inst_state *inst_state = (sns_ext_svc_inst_state*)this->state->state;

  inst_state->qmi_handle = NULL;
  inst_state->svc_id = -1;
  inst_state->sensor_state = sensor_state;

  return SNS_RC_SUCCESS;
}

/**
 * QMI Error callback.  This would be our notification if the QMI connection
 * was lost.  See qmi_client_error_cb.
 */
static void
error_cb(qmi_client_type user_handle,
    qmi_client_error_type error, void *err_cb_data)
{
  UNUSED_VAR(user_handle);
  sns_sensor_instance *this = (sns_sensor_instance*)err_cb_data;

  sns_std_error_event error_event = sns_std_error_event_init_default;
  error_event.error = (sns_std_error)SNS_RC_INVALID_STATE;

  SNS_INST_PRINTF(ERROR, this, "error_cb error %i", error);

  add_pending_event(this, sns_std_error_event_fields, &error_event,
        SNS_STD_MSGID_SNS_STD_ERROR_EVENT);
}

/* QMI indication callback.  See qmi_client_ind_cb. */
static void
ind_msg_cb(qmi_client_type user_handle, unsigned int msg_id,
    void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data)
{
  UNUSED_VAR(user_handle);
  sns_sensor_instance *this = (sns_sensor_instance*)ind_cb_data;
  sns_ext_svc_inst_state *inst_state = (sns_ext_svc_inst_state*)this->state->state;
  int32_t qmi_err;
  uint32_t ind_len;

  qmi_idl_service_object_type svc_obj = sns_ext_svc_get(
      &inst_state->sensor_state->services, inst_state->svc_id);
  qmi_err = qmi_idl_get_message_c_struct_len(svc_obj,
        QMI_IDL_INDICATION, msg_id, &ind_len);

  void *ind = sns_malloc(SNS_HEAP_MAIN, ind_len);
  SNS_ASSERT(NULL != ind);

  SNS_INST_PRINTF(LOW, this, "Received Indication; len %i", ind_buf_len);

  qmi_err = qmi_idl_message_decode(svc_obj,
      QMI_IDL_INDICATION, msg_id, ind_buf, ind_buf_len, ind, ind_len);
  if(QMI_IDL_LIB_NO_ERR != qmi_err)
  {
    SNS_INST_PRINTF(ERROR, this, "QMI decode error %i", qmi_err);
  }
  else
  {
    sns_ext_svc_ind event = sns_ext_svc_ind_init_default;
    pb_buffer_arg data = (pb_buffer_arg)
      { .buf = ind, .buf_len = ind_len };
    event.payload.funcs.encode = &pb_encode_string_cb;
    event.payload.arg = &data;
    event.msg_id = msg_id;

    add_pending_event(this, sns_ext_svc_ind_fields, &event,
        SNS_EXT_SVC_MSGID_SNS_EXT_SVC_IND);
  }

  sns_free(ind);
}

/**
 * Create a QMI connection to the specified service.
 *
 * @param[i] this Sensor Instance
 * @param[i] timeout Time-out in milliseconds. 0 = no timeout
 * @param[o] qmi_handle QMI Handle created in qmi_client_init_instance
 *
 * @return True on success; false otherwise
 */
static bool
create_client_conn(sns_sensor_instance* const this,
    uint32_t timeout, qmi_client_type *qmi_handle)
{
  bool rv = false;
  sns_ext_svc_inst_state *inst_state = (sns_ext_svc_inst_state*)this->state->state;
  qmi_idl_service_object_type svc_obj = sns_ext_svc_get(
      &inst_state->sensor_state->services, inst_state->svc_id);
  qmi_service_instance instance_id = 0;
  qmi_client_error_type qmi_err;
  qmi_cci_os_signal_type os_params;

  SNS_INST_PRINTF(LOW, this, "Creating client connection %i", inst_state->svc_id);

  qmi_err = qmi_client_init_instance(svc_obj,
      instance_id, ind_msg_cb, this, &os_params, timeout, qmi_handle);

  if(QMI_NO_ERR != qmi_err)
  {
    SNS_INST_PRINTF(ERROR, this, "qmi_client_init_instance error %i", qmi_err);
  }
  else
  {
    qmi_err = qmi_client_register_error_cb(*qmi_handle, error_cb, this);
    if(QMI_NO_ERR != qmi_err)
    {
      SNS_INST_PRINTF(ERROR, this,
        "qmi_client_register_error_cb error %i", qmi_err);
    }
    else
    {
      rv = true;
    }
  }

  if(!rv)
  {
    if(NULL != *qmi_handle)
    {
      qmi_client_release(*qmi_handle);
      *qmi_handle = NULL;
    }
  }

  return rv;
}

/**
 * Handle an incoming QMI response message from the external QMI service.
 */
static void
resp_msg_cb(qmi_client_type user_handle, unsigned int msg_id,
    void *resp_c_struct, unsigned int resp_c_struct_len,
    void *resp_cb_data, qmi_client_error_type transp_err)
{
  UNUSED_VAR(user_handle);
  sns_ext_svc_resp_cb_data *cb_data = (sns_ext_svc_resp_cb_data*)resp_cb_data;
  sns_ext_svc_inst_state *inst_state = (sns_ext_svc_inst_state*)
    cb_data->instance->state->state;

  SNS_INST_PRINTF(LOW, cb_data->instance,
      "Received response; error %i, msg_id %i, size %i, transaction_id %i",
      transp_err, msg_id, resp_c_struct_len, cb_data->transaction_id);

  UNUSED_VAR(inst_state);

  sns_ext_svc_resp event = sns_ext_svc_resp_init_default;
  pb_buffer_arg data = (pb_buffer_arg)
    { .buf = resp_c_struct, .buf_len = resp_c_struct_len };
  event.payload.funcs.encode = &pb_encode_string_cb;
  event.payload.arg = &data;
  event.msg_id = msg_id;
  event.transp_err = transp_err;
  event.has_transaction_id = true;
  event.transaction_id = cb_data->transaction_id;

  add_pending_event(cb_data->instance, sns_ext_svc_resp_fields, &event,
      SNS_EXT_SVC_MSGID_SNS_EXT_SVC_RESP);

  sns_free(resp_c_struct);
  sns_free(cb_data);
}

/**
 * Send a QMI request message.
 *
 * @param[i] qmi_handle QMI connecton created in create_client_conn
 * @param[i] msg_id Message ID corresponding to req_msg
 * @param[i] req_msg Completed request message to be sent
 * @param[i] req_msg_len Length of req_msg
 *
 * @return true on success, false otherwise
 */
static bool
send_qmi_req(sns_sensor_instance *const this, qmi_client_type qmi_handle,
    uint32_t msg_id, uint32_t transaction_id, void *req_msg, uint32_t req_msg_len)
{
  bool rv = false;
  uint32_t resp_len;
  qmi_client_error_type qmi_err;
  sns_ext_svc_inst_state *inst_state = (sns_ext_svc_inst_state*)this->state->state;
  qmi_idl_service_object_type svc_obj = sns_ext_svc_get(
      &inst_state->sensor_state->services, inst_state->svc_id);

  SNS_INST_PRINTF(LOW, this,
      "Sending request payload len %i, transaction_id %i, message_id %i",
      req_msg_len, transaction_id, msg_id);

  qmi_err = qmi_idl_get_message_c_struct_len(svc_obj,
        QMI_IDL_RESPONSE, msg_id, &resp_len);
  if(QMI_NO_ERR != qmi_err)
  {
    SNS_INST_PRINTF(ERROR, this,
        "qmi_idl_get_message_c_struct_len %i", qmi_err);
  }
  else
  {
    qmi_txn_handle txn_handle;
    void *resp = sns_malloc(SNS_HEAP_MAIN, resp_len);
    SNS_ASSERT(NULL != resp);
    sns_ext_svc_resp_cb_data *cb_data =
      sns_malloc(SNS_HEAP_MAIN, sizeof(*cb_data));
    SNS_ASSERT(NULL != cb_data);

    cb_data->transaction_id = transaction_id;
    cb_data->instance = this;

    qmi_err = qmi_client_send_msg_async(
        qmi_handle, msg_id, req_msg, req_msg_len,
        resp, resp_len, resp_msg_cb, cb_data, &txn_handle);

    if(QMI_NO_ERR != qmi_err)
    {
      SNS_INST_PRINTF(ERROR, this,
        "qmi_client_send_msg_async %i", qmi_err);
      sns_free(resp);
      sns_free(cb_data);
    }
    else
    {
      rv = true;
    }
  }

  return rv;
}

/* See sns_sensor_instance_api::set_client_config */
static sns_rc
inst_set_client_config(sns_sensor_instance* const this,
    sns_request const *client_request)
{
  sns_ext_svc_inst_state *inst_state = (sns_ext_svc_inst_state*)this->state->state;
  sns_rc rv = SNS_RC_FAILED;
  pb_buffer_arg data;
  sns_ext_svc_req request = sns_ext_svc_req_init_default;

  request.payload.funcs.decode = pb_decode_string_cb;
  request.payload.arg = &data;

  if(pb_decode_request(client_request, sns_ext_svc_req_fields, NULL, &request) &&
     NULL != sns_ext_svc_get(&inst_state->sensor_state->services, request.svc_id))
  {
    inst_state->svc_id = request.svc_id;

    if(NULL != inst_state->qmi_handle ||
       create_client_conn(this, 100, &inst_state->qmi_handle))
    {
      if(send_qmi_req(this, inst_state->qmi_handle, request.msg_id,
          request.transaction_id, (void*)data.buf, data.buf_len))
        rv = SNS_RC_SUCCESS;
    }

    if(SNS_RC_SUCCESS != rv)
    {
      sns_std_error_event error_event = sns_std_error_event_init_default;
      error_event.error = (sns_std_error)SNS_RC_INVALID_STATE;

      add_pending_event(this, sns_std_error_event_fields, &error_event,
        SNS_STD_MSGID_SNS_STD_ERROR_EVENT);
    }
  }

  return rv;
}

/* See sns_sensor_instance_api::deinit */
static sns_rc
inst_deinit(sns_sensor_instance* const this)
{
  sns_ext_svc_inst_state *inst_state = (sns_ext_svc_inst_state*)this->state->state;

  qmi_client_release(inst_state->qmi_handle);
  remove_pending_events(this);

  return SNS_RC_SUCCESS;
}

/*=============================================================================
  SEE Sensor Instance API Definition
  ===========================================================================*/

sns_sensor_instance_api sns_ext_svc_sensor_instance_api =
{
  .struct_len        = sizeof(sns_sensor_instance_api),
  .init              = &inst_init,
  .deinit            = &inst_deinit,
  .set_client_config = &inst_set_client_config,
  .notify_event      = &inst_notify_event
};
