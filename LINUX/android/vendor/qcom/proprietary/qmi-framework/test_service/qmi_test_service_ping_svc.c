/******************************************************************************
  -----------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "test_service_v01.h"

/* This struct can hold more information
   about the client that is connecting to the service
   if there is any reason to keep state information
   about clients */
typedef struct {
  qmi_client_handle clnt;
} client_info_type;

/* This struct holds the arguments that need to be passed to the
   send_indications function */
typedef struct {
  client_info_type *clnt;
  qmi_req_handle req_handle;
  int msg_id;
  test_data_ind_reg_req_msg_v01 req_struct;
} ind_type;

/* This struct is used to hold context of the server.  Cleanup is used
   to signal the reader thread to clean up and die, num_requests just tracks
   how many requests the service has handled, and service_handle is passed
   into the qmi_csi_handle_event function
   An instance of this structure is passed to the qmi_csi_register function as
   the service_cookie, and this structure can be tailored to a specific services
   needs */
typedef struct {
  volatile int cleanup;
  qmi_csi_service_handle service_handle;
  int num_requests;
} service_context_type;

/* This struct holds the arguments that need to be passed to the service
   reader thread */
typedef struct {
  service_context_type *service_cookie;
  qmi_csi_os_params *os_params;
} reader_args_type;

static service_context_type service_cookie;

/* ============================================================================
**  Function : test_create_ind_pattern
** ===========================================================================*/
/*!
  @brief
  test_create_ind_pattern function creates an indication pattern to be sent out
  to read.

  @param[in]  IndMsg   Pointer to the Indication message.
  @param[in]  iIndSz   Size of indication message

  @par Side Effects
  None

  @return
  None
*/

static void test_create_ind_pattern(test_data_ind_msg_v01 *IndMsg, int iIndSz)
{
  uint32_t iCount = 0;
  uint32_t iSum = 0;
  uint32_t iMsgLen = 0;

  if (NULL == IndMsg)
  {
    printf("Error: Null Ind Msg\n");
    return;
  }
  /* Set the message len and data */
  iMsgLen = iIndSz/sizeof(uint8_t);
  IndMsg->data_len = iMsgLen;
  for (iCount = 0 ; iCount < iMsgLen ; iCount++)
  {
    IndMsg->data[iCount] = iCount % 256;
  }

  /* Compute the sum */
  for (iCount = 0 ; iCount < IndMsg->data_len; iCount++)
  {
    iSum = iSum + *(IndMsg->data + iCount);
  }
  IndMsg->service_name_valid = 0; /* set to false */
  IndMsg->sum_valid = 1; /* set to true */
  IndMsg->sum = iSum;
  return;
}

/*=============================================================================
  FUNCTION test_send_indications
=============================================================================*/
/*!
@brief
  This function sends a number of indications. It is called by ping_data_ind_registration
  which creates a new thread to handle this function so that the handle_req callback
  is not blocked

@param[in]   params    structure of ind_type that holds the relevant information
                       for the send_indications function to send the correct
                       number of indications to the correct client.

*/
/*=========================================================================*/
static void test_send_indications(ind_type *params)
{
  int i;
  qmi_csi_error resp_err;
  test_data_ind_reg_resp_msg_v01 resp;
  int num_inds,ind_size,ind_delay;
  /* Freed at the end of this function */
  test_data_ind_msg_v01 *ind = (test_data_ind_msg_v01*)malloc(sizeof(test_data_ind_msg_v01));
  if(!ind)
  {
    printf("Indication msg allocation failed\n");
    return;
  }
  memset(ind, 0, sizeof(test_data_ind_msg_v01));
  memset(&resp, 0, sizeof(test_data_ind_reg_resp_msg_v01));
  num_inds = ind_size = ind_delay = 0;
  /* Set values based on if the optional TLVs were sent */
  if(params->req_struct.num_inds_valid)
  {
    num_inds = params->req_struct.num_inds;
  }
  if(params->req_struct.ind_size_valid)
  {
    ind_size = params->req_struct.ind_size;
  }
  if(params->req_struct.ms_delay_valid)
  {
    ind_delay = params->req_struct.ms_delay;
  }

  resp.resp.result = QMI_RESULT_SUCCESS_V01; /* QMI_RESULT_SUCCESS*/
  resp.resp.error = QMI_ERR_NONE_V01;       /* QMI_ERR_NONE */

  resp_err = qmi_csi_send_resp(params->req_handle, params->msg_id,
                &resp, sizeof(resp));
  if(resp_err != QMI_CSI_NO_ERR)
  {
    //printf("qmi_csi_send_resp returned error: %d\n", resp_err);
  }

  ind->data_len = ind_size;
  printf("Indication Response Sent\n");
  /*Create a pattern for indication*/
  test_create_ind_pattern(ind,ind_size);

  for(i=0;i<num_inds;++i)
  {
    // TODO: define something in the target header for sleep
    //usleep(ind_delay);
    //printf("sending indication %d of %d\n",i+1,num_inds);
    resp_err = qmi_csi_send_ind(params->clnt->clnt, QMI_TEST_DATA_IND_V01, ind,
                  sizeof(test_data_ind_msg_v01));
    if(resp_err != QMI_CSI_NO_ERR)
    {
      //printf("qmi_csi_send_ind returned error: %d\n",resp_err);
    }
  }
  printf("Indications Sent: %d\n", i);
  free(ind); /* Malloc at top of this function */
}

/*=============================================================================
  FUNCTION test_response
=============================================================================*/
/*!
@brief
  This function is called when a ping request message is received by
  the service, it sends a pong response

@param[in]   req_c_struct       Decoded request message structure

@param[in]   req_handle         Opaque handle provided by the infrastructure
                                to specify a particular transaction and
                                message.

@retval    QMI_CSI_CB_NO_ERR    Success
@retval    QMI_CSI_CB_...       Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
static qmi_csi_cb_error test_response(client_info_type *clnt_info,
          qmi_req_handle req_handle, int msg_id, void *req_c_struct,
          int req_c_struct_len, void *service_cookie)
{
  qmi_csi_cb_error rc = QMI_CSI_CB_INTERNAL_ERR;
  qmi_csi_error resp_err;

  /* The response message is small, and can safely be declared on the stack.
     See test_ping_data_response for a message that is large and uses
     dynamic allocation.*/
  test_ping_resp_msg_v01 resp;
  //printf("Ping Service: %s\n",((ping_req_msg_v01*)req_c_struct)->ping);
  memset( &resp, 0, sizeof(test_ping_resp_msg_v01) );
  memcpy( &resp, "apps", 4 );
  /* The response is very simple and does not take long to compute, so it is
     safe to send it within the callback context, and does not require dispatch
     to a new thread.  See ping_data_ind_registration to see the dispatch of
     message handling */
  resp_err = qmi_csi_send_resp(req_handle, msg_id, &resp, sizeof(resp));
  if(resp_err != QMI_CSI_NO_ERR)
  {
    //printf("qmi_csi_send_resp returned error: %d\n",resp_err);
  }
  else
  {
    rc = QMI_CSI_CB_NO_ERR;
  }
  return rc;
}

/*=============================================================================
  FUNCTION test_data_response
=============================================================================*/
/*!
@brief
  This function is called when a ping_data request message is received by
  the service, sending a response message with the same sized data field
  as was in the request message.

@param[in]   req_c_struct       Decoded request message structure

@param[in]   req_handle         Opaque handle provided by the infrastructure
                                to specify a particular transaction and
                                message.

@retval    QMI_CSI_CB_NO_ERR    Success
@retval    QMI_CSI_CB_...       Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
static qmi_csi_cb_error test_data_response(client_info_type *clnt_info,
          qmi_req_handle req_handle, int msg_id, void *req_c_struct,
          int req_c_struct_len, void *service_cookie)
{
  uint32_t iCount = 0;
  qmi_csi_cb_error rc = QMI_CSI_CB_INTERNAL_ERR;
  qmi_csi_error resp_err;
  service_context_type *context = (service_context_type*)service_cookie;

  /*Request and response messages*/
  test_data_resp_msg_v01 *resp;
  test_data_req_msg_v01 *req;

  /*Map request info*/
  req = (test_data_req_msg_v01 *)req_c_struct;

  /* responce allocation */
  resp = (test_data_resp_msg_v01*)malloc(sizeof(test_data_resp_msg_v01));
  if(!resp)
    return rc;
  memset(resp, 0, sizeof(test_data_resp_msg_v01));

  /* The response is very simple and does not take long to compute, so it is
     safe to send it within the callback context, and does not require dispatch
     to a new thread.  See ping_data_ind_registration to see the dispatch of
     message handling */

  /* Set the response message, loopback the data */
    for (iCount = 0; iCount < req->data_len; iCount++)
    {
      *(resp->data + iCount)= *(req->data + iCount);
    }
    resp->data_valid = 1; /* Set to true */
    resp->data_len = req->data_len;
    resp->resp.result = QMI_RESULT_SUCCESS_V01; /* QMI_RESULT_SUCCESS */
    resp->resp.error = QMI_ERR_NONE_V01;       /* QMI_ERR_NONES */


  resp_err = qmi_csi_send_resp(req_handle, msg_id, resp,
                sizeof(test_data_resp_msg_v01));
  if(resp_err != QMI_CSI_NO_ERR)
  {
    //printf("qmi_csi_send_resp returned error: %d\n",resp_err);
  }
  else
  {
    rc = QMI_CSI_CB_NO_ERR;
  }

  printf("Response Sent, DataLen: %d\n", resp->data_len);
  /* Check rc, if error print error and return actual qmi_csi_cb_error value */
  free(resp); /* Malloc at top of this function */
  return rc;
}

/*=============================================================================
  FUNCTION test_data_ind_registration
=============================================================================*/
/*!
@brief
  This function is called when a ping_data_ind_reg request message is received
  by the service, sending a response message and then a series of indications
  depending on what information was passed in the request message

@param[in]   req_c_struct       Decoded request message structure

@param[in]   req_handle         Opaque handle provided by the infrastructure
                                to specify a particular transaction and
                                message.

@retval    QMI_CSI_CB_NO_ERR    Success
@retval    QMI_CSI_CB_...       Error, see error codes defined in qmi_csi.h

*/
/*=========================================================================*/
static qmi_csi_cb_error test_data_ind_registration(client_info_type *clnt_info,
  qmi_req_handle req_handle, int msg_id, void *req_c_struct,
  int req_c_struct_len, void *service_cookie)
{
  ind_type ind_params;

  ind_params.req_struct = *(test_data_ind_reg_req_msg_v01*)req_c_struct;
  ind_params.clnt = clnt_info;
  ind_params.req_handle = req_handle;
  ind_params.msg_id = msg_id;
  test_send_indications(&ind_params);

  return QMI_CSI_CB_NO_ERR;
}

/* Define a jump table to handle the dispatch of request handler functions. */
static qmi_csi_cb_error(* const req_handle_table[])
(
 client_info_type         *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                     *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_cookie
) =
{
  NULL,                         /* Request handler for message ID 0x00 */
  &test_response,               /* Request handler for message ID 0x01 */
  &test_data_response,          /* Request handler for message ID 0x02 */
  &test_data_ind_registration   /* Request handler for message ID 0x03 */
};


/*=============================================================================
  CALLBACK FUNCTION test_connect_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure when
  infrastructure receives a request from the client( user of the service).

@param[in]   client_handle       Opaque handle used by the infrastructure to
                                 identify different services.
@param[in]   service_cookie      Service specific data.Service cookie is
                                 registered with the infrastructure during
                                 service registration(qmi_csi_register).
@param[out]  connection_handle   Services should provide this handle to
                                 facilitate the infrastructure to communicate
                                 to each individual service.

@retval    QMI_CSI_NO_ERR        Success
@retval    QMI_CSI_CONN_REFUSED  This error occurs when  limit on MAX number of
                                 clients a service can support is reached.

*/
/*=========================================================================*/
static qmi_csi_cb_error test_connect_cb(qmi_client_handle client_handle,
          void *service_cookie, void **connection_handle)
{
  /* This is where the client handle can be stored if the service wants
     to keep some state information about clients. The connection handle can also
     be modified to facilitate communication with the infrastructure */
  client_info_type *clnt_info;
  /* For any service where it is anticipated that there could be more than one client
     connected at any given time the clients should be stored in some kind of data structure
     to facilitate cleanup and the sending of indications. */
  clnt_info = malloc(sizeof(client_info_type)); /* Freed in ping_disconnect_cb */
  if(!clnt_info)
    return QMI_CSI_CB_CONN_REFUSED;
  clnt_info->clnt = client_handle;
  *connection_handle = clnt_info;
  printf("TestService: A client connected to this service\n");
  return QMI_CSI_CB_NO_ERR;
}

/*=============================================================================
  CALLBACK FUNCTION test_disconnect_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure when the last
  client(user of service ) deregisters with the  QCSI infrastructure.

@param[in]  connection_handle      Service handle used by the infrastructure
                                   to communicate to each individual service.
@param[in]  service_cookie         Service specific data.Service cookie is
                                   registered with the infrastructure during
                                   service registration(qmi_csi_register).
@retval    QMI_CSI_NO_ERR          Success
@retval    QMI_CSI_CONN_BUSY       This error occurs when service might be busy
                                   processing other pending requests.
@retval    QMI_CSI_INVALID_HANDLE  This error occurs when the connection
                                   handle passed is invalid.
*/
/*=========================================================================*/
static void test_disconnect_cb(void *connection_handle, void *service_cookie)
{
  /* Free up memory for the client */
  if(connection_handle)
    free(connection_handle); /* Malloc in ping_connect_cb */

  return;
}

/*=============================================================================
  CALLBACK FUNCTION test_handle_req_cb
=============================================================================*/
/*!
@brief
   This callback is invoked when the infrastructure receives an incoming message.
   The infrastructure decodes the data and gives it to the services

@param[in]  connection_handle      Service handle used by the infrastructure
                                   to communicate to each individual service.
@param[in]  req_handle             Opaque handle provided by the infrastructure
                                   to specify a particular transaction and
                                   message.

@param[in]  msg_id                 Message Id pertaining to a particular
                                   message.
@param[in]  req_c_struct           C struct with the decoded data.
@param[in]  req_c_struct_len       Length of the c struct.
@param[in]  service_cookie         Service specific data.Service cookie is
                                   registered with the infrastructure during
                                   service registration(qmi_csi_register).


@retval    QMI_CSI_NO_ERR          Success
@retval    QMI_IDL_...             Error, see error codes defined in qmi.h
*/
/*=========================================================================*/
static qmi_csi_cb_error test_handle_req_cb(void *connection_handle,
          qmi_req_handle req_handle, unsigned int msg_id, void *req_c_struct,
          unsigned int req_c_struct_len, void *service_cookie)
{
  qmi_csi_cb_error rc = QMI_CSI_CB_INTERNAL_ERR;
  client_info_type *clnt_info = (client_info_type*)connection_handle;

  /* At this point the clnt_info can be tested to verify it is valid
         and any other tests or operations that a particular service would
         like to do on every client request */
  /* A request is being handled, increment the service_cookie num_requests */
  ((service_context_type*)service_cookie)->num_requests++;
  /* Verify that the msg_id received is not greater than the largest known msg_id */
  if((unsigned int)msg_id < (sizeof(req_handle_table) / sizeof(*req_handle_table)))
  {
    /* If the jump table has a valid entry at this message ID, call the handler function */
    if(req_handle_table[msg_id])
    {
      rc = req_handle_table[msg_id](clnt_info, req_handle,msg_id, req_c_struct,
              req_c_struct_len, service_cookie);
    }
    else
    {
      //printf("No function defined to handle request for message ID: %d\n",msg_id);
    }
  }
  else
  {
    //printf("Message ID: %d greater than maximum known message ID.\n",msg_id);
  }

  return rc;
}

/*=============================================================================
  FUNCTION qmi_test_service_register_service
=============================================================================*/
void *qmi_test_service_register_service(qmi_csi_os_params *os_params,
          uint32_t serv_instce)
{
  qmi_csi_options  uSvcOptions;

  /*Set the instance parameter for service */
  uSvcOptions.options_set = QMI_CSI_OPTIONS_INSTANCE_ID_VALID;
  uSvcOptions.instance_id = serv_instce; // TEST_APT_INSTANCE_ID

  qmi_idl_service_object_type test_service_object = test_get_service_object_v01();
  qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;

  rc = qmi_csi_register_with_options(test_service_object, test_connect_cb,
          test_disconnect_cb, test_handle_req_cb, &service_cookie,
          os_params, &uSvcOptions, &service_cookie.service_handle);
  if(rc != QMI_NO_ERR)
    return NULL;
  return service_cookie.service_handle;
}
