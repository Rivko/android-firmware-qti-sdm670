/*=============================================================================
  @file sns_qmi_client.c

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <inttypes.h>
#include "sns_client.pb.h"
#include "sns_client_api_v01.h"
#include "sns_client.h"
#include "sns_osa.h"
#include "qmi_client.h"
#include "pb_encode.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/* An outgoing request which is awaiting the corresponding response message. */
typedef struct sns_request
{
  struct sns_client *client;
  /* Client registered callback */
  sns_client_resp resp_cb;
  void *resp_cb_data;
} sns_request;

/* A client connection; a client process may open multiple QMI connections. */
typedef struct sns_client
{
  /* Client registered callbacks */
  sns_client_ind ind_cb;
  void *ind_cb_data;
  sns_client_error error_cb;
  void *error_cb_data;

  /* QMI Client Handle */
  qmi_client_type qmi_handle;
} sns_client;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* QMI indication callback.  See qmi_client_ind_cb. */
static void
client_ind_cb(qmi_client_type user_handle, unsigned int msg_id,
    void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data)
{
  UNUSED_VAR(user_handle);
  sns_client *client = ind_cb_data;
  size_t ind_len = sizeof(sns_client_report_ind_msg_v01);
  sns_client_report_ind_msg_v01 *ind = malloc(ind_len);
  int32_t qmi_err;

  SNS_ASSERT(NULL != ind);

  // Extract the Protocol Buffer encoded message from the outer QMI/IDL  message
  qmi_err = qmi_idl_message_decode(SNS_CLIENT_SVC_get_service_object_v01(),
      QMI_IDL_INDICATION, msg_id, ind_buf, ind_buf_len, ind, ind_len);
  if(QMI_IDL_LIB_NO_ERR != qmi_err)
  {
    SNS_LOG(ERROR, "QMI decode error %i", qmi_err);
  }
  else
  {
    SNS_LOG(VERBOSE, "Indication from client ID %"PRIu64, ind->client_id);

    client->ind_cb(client, ind->payload, ind->payload_len, client->ind_cb_data);
  }

  free(ind);
}

/**
 * Allocate and initialize the response callback handler state for a request.
 *
 * @param[i] client
 * @param[i] resp_cb Callback to be called upon response receipt
 * @param[i] resp_cb_data Optional callback to be delivered
 *
 * @return Newly create request objct
 */
static sns_request*
create_request(sns_client *client, sns_client_resp resp_cb,
    void *resp_cb_data)
{
  sns_request *request;

  request = sns_malloc(sizeof(*request));
  SNS_ASSERT(NULL != request);

  request->resp_cb = resp_cb;
  request->resp_cb_data = resp_cb_data;
  request->client = client;

  return request;
}

/**
 * Handle an incoming QMI response message from the Sensors Service.
 */
static void
client_resp_cb(qmi_client_type user_handle, unsigned int msg_id,
    void *resp_c_struct, unsigned int resp_c_struct_len,
    void *resp_cb_data, qmi_client_error_type transp_err)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(msg_id);
  UNUSED_VAR(resp_c_struct_len);
  sns_request *request = resp_cb_data;
  sns_client_resp_msg_v01 *resp = resp_c_struct;
  sns_std_error err = SNS_STD_ERROR_NO_ERROR;

  if(NULL != resp && resp->result_valid)
    err = resp->result;
  else if(QMI_NO_ERR != transp_err)
    err = SNS_STD_ERROR_FAILED;

  SNS_LOG(VERBOSE, "Response from client %"PRIu64, resp->client_id);

  if(NULL != request->resp_cb)
    request->resp_cb(request->client, err, request->resp_cb_data);

  sns_free(request);
  sns_free(resp_c_struct);
}

/**
 * An error occurred; typically means the SLPI has restarted, and the client
 * should attempt to reconnect.
 */
static void
client_error_cb(qmi_client_type user_handle,
    qmi_client_error_type error, void *err_cb_data)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(error);
  sns_client *client = err_cb_data;

  SNS_LOG(VERBOSE, "Error from client");

  // PEND: Convert QMI transport error
  client->error_cb(client, SNS_STD_ERROR_INVALID_STATE, client->error_cb_data);
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

int
sns_client_init(sns_client **client_out, uint32_t timeout,
    sns_client_ind ind_cb, void *ind_cb_data,
    sns_client_error error_cb, void *error_cb_data)
{
  qmi_idl_service_object_type service_obj = SNS_CLIENT_SVC_get_service_object_v01();
  qmi_service_instance instance_id = 0;
  qmi_client_error_type qmi_err;
  qmi_cci_os_signal_type os_params;
  int rv = -1;
  sns_client *client;

  client = sns_malloc(sizeof(*client));
  SNS_ASSERT(NULL != client);
  client->ind_cb = ind_cb;
  client->ind_cb_data = ind_cb_data;
  client->error_cb = error_cb;
  client->error_cb_data = error_cb_data;

  qmi_err = qmi_client_init_instance(service_obj,
      instance_id, client_ind_cb, client, &os_params, timeout, &client->qmi_handle);
  if(QMI_NO_ERR != qmi_err)
  {
    SNS_LOG(ERROR, "qmi_client_init_instance error %i", qmi_err);
  }
  else
  {
    qmi_err = qmi_client_register_error_cb(client->qmi_handle, client_error_cb, NULL);

    if(QMI_NO_ERR != qmi_err)
      SNS_LOG(ERROR, "qmi_client_register_error_cb error %i", qmi_err);
    else
      rv = 0;
  }

  if(0 != rv)
  {
    if(NULL != client->qmi_handle)
      qmi_client_release(client->qmi_handle);

    sns_free(client);
    client = NULL;
  }

  SNS_LOG(VERBOSE, "sns_client_init %p", client);

  *client_out = client;
  return rv;
}

int
sns_client_deinit(sns_client *client)
{
  qmi_client_release(client->qmi_handle);
  sns_free(client);

  SNS_LOG(VERBOSE, "sns_client_deinit complete %p", client);
  return 0;
}

int
sns_client_send(sns_client *client, sns_client_request_msg *msg,
    sns_client_resp resp_cb, void *resp_cb_data)
{
  int rv = 0;
  pb_ostream_t stream;
  sns_client_req_msg_v01 *req_msg;

  req_msg = sns_malloc(sizeof(*req_msg));
  SNS_ASSERT(NULL != req_msg);

  stream = pb_ostream_from_buffer(req_msg->payload, sizeof(req_msg->payload));
  if(!pb_encode(&stream, sns_client_request_msg_fields, msg))
  {
    SNS_LOG(ERROR, "pb_encode error: %s", PB_GET_ERROR(&stream));
    rv = -1;
  }
  else
  {
    qmi_txn_handle txn_handle;
    qmi_client_error_type qmi_err;
    sns_request *request;
    size_t resp_len = sizeof(sns_client_resp_msg_v01);
    void *resp;

    resp = sns_malloc(resp_len);
    SNS_ASSERT(NULL != resp);
    request = create_request(client, resp_cb, resp_cb_data);

    req_msg->payload_len = stream.bytes_written;
    qmi_err = qmi_client_send_msg_async(
        client->qmi_handle, SNS_CLIENT_REQ_V01, req_msg, sizeof(*req_msg),
        resp, resp_len, client_resp_cb, request, &txn_handle);

    if(QMI_NO_ERR != qmi_err)
    {
      SNS_LOG(ERROR, "qmi_client_send_msg_async error %i", qmi_err);
      sns_free(resp);
      sns_free(request);
      rv = -2;
    }
  }

  sns_free(req_msg);
  return rv;
}
