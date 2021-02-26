/******************************************************************************
  @file    qmi_ping_svc.c
  @brief   OS Common implementation of the QMI Test service

  DESCRIPTION
  OS Common implementation of the QMI test service.
  
  ---------------------------------------------------------------------------
  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "test_service_v01.h"

/* XXX This is an internal header to QMI-FW. If using this source 
 * as an example, please do not include this in your 
 * implementation. You should provide your own macro
 * for QMI_MEM_COPY */
#include "qmi_common.h"
/* XXX This is an internal header to QMI-FW. If using this source 
 * as an example, please do not include this in your 
 * implementation. You should provide your own macro
 * for MALLOC, CALLOC & FREE */
#include "qmi_csi_target.h"

/* This struct holds the arguments that need to be passed to the send_indications function */
typedef struct {
  qmi_req_handle req_handle;
  int msg_id;
  test_data_ind_reg_req_msg_v01 req_struct;
  test_data_ind_msg_v01 *ind;
  int num_inds;
  int inds_done;
}ind_type;

/* This struct can hold more information 
   about the client that is connecting to the service
   if there is any reason to keep state information
   about clients */
typedef struct {
  qmi_client_handle clnt;
  ind_type ind_test;
}client_info_type;

/* This struct is used to hold context of the server.  Cleanup is used 
   to signal the reader thread to clean up and die, num_requests just tracks
   how many requests the service has handled, and service_handle is passed
   into the qmi_csi_handle_event function
   An instance of this structure is passed to the qmi_csi_register function as
   the service_cookie, and this structure can be tailored to a specific services needs */
typedef struct {
  volatile int cleanup;
  qmi_csi_service_handle service_handle;
  int num_requests;
}service_context_type;

/* This struct holds the arguments that need to be passed to the service reader thread */
typedef struct {
  service_context_type *service_cookie;
  qmi_csi_os_params *os_params;
}reader_args_type;

static service_context_type service_cookie;

#ifndef QMI_PING_SERVICE_INSTANCE
#define QMI_PING_SERVICE_INSTANCE 0
#endif

unsigned int qmi_ping_service_instance = QMI_PING_SERVICE_INSTANCE;

/*=============================================================================
  FUNCTION send_indications
=============================================================================*/
/*!
@brief
  This function sends a number of indications. It is called by ping_data_ind_registration
  which creates a new thread to handle this function so that the handle_req callback is not
  blocked 

@param[in]   params              structure of ind_type that holds the relevant information 
                                 for the send_indications function to send the correct
                                 number of indications to the correct client.

*/
/*=========================================================================*/
static qmi_csi_cb_error send_indications(client_info_type *clnt)
{
  qmi_csi_error resp_err;
  test_data_ind_reg_resp_msg_v01 resp;
  int num_inds = 10,ind_size = 32;//,ind_delay = 0;
  test_data_ind_msg_v01 *ind;
  ind_type *params = NULL;

  if(!clnt)
    return QMI_CSI_CB_CONN_REFUSED;

  params = &clnt->ind_test;
  if(params->msg_id == 0)
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  if(!clnt->ind_test.ind)
  {
    /* First time */
    params->ind = 
               (test_data_ind_msg_v01*)CALLOC(1, sizeof(test_data_ind_msg_v01));
    if(!params->ind)
      return QMI_CSI_CB_NO_MEM;

    memset( &resp, 0, sizeof(test_data_ind_reg_resp_msg_v01) );
    /* Set values based on if the optional TLVs were sent */
    if(params->req_struct.num_inds_valid)
    {
      num_inds = params->req_struct.num_inds;
    }

    if(params->req_struct.ind_size_valid)
    {
      ind_size = params->req_struct.ind_size;
    }

    /*if(params->req_struct.ms_delay_valid)
    {
      ind_delay = params->req_struct.ms_delay;
    }*/

    params->ind->data_len = ind_size;
    params->num_inds = num_inds;
    params->inds_done = 0;

    resp_err = qmi_csi_send_resp( params->req_handle, params->msg_id, &resp, sizeof(resp) ); 
    if(resp_err != QMI_CSI_NO_ERR)
    {
      FREE(params->ind);
      params->ind = NULL;
      params->num_inds = params->inds_done = 0;
      params->msg_id = 0;
      params->req_handle = 0;
      return QMI_CSI_CB_CONN_REFUSED;
    }
  }

  ind = params->ind;

  for(; params->inds_done < params->num_inds; params->inds_done++)
  {
    // TODO: define something in the target header for sleep
    //usleep(ind_delay);
    //printf("sending indication %d of %d\n",i+1,num_inds);
    resp_err = qmi_csi_send_ind( clnt->clnt, QMI_TEST_DATA_IND_V01, ind, sizeof(test_data_ind_msg_v01) );
    if(resp_err == QMI_CSI_CONN_BUSY)
    {
      break;
    }
    else if(resp_err != QMI_CSI_NO_ERR)
    {
      /* Stop sending. */
      // printf("Sending indication returned %d\n", resp_err);
      params->inds_done = params->num_inds;
      break;
    }
  }

  if(params->inds_done == params->num_inds)
  {
    FREE(ind);
    params->ind = NULL;
    params->num_inds = params->inds_done = 0;
    params->msg_id = 0;
    params->req_handle = 0;
    memset(&params->req_struct, 0, sizeof(params->req_struct));
  }
  return QMI_CSI_CB_NO_ERR;
}

/*=============================================================================
  FUNCTION ping_response
=============================================================================*/
/*!
@brief
  This function is called when a ping request message is received by
  the service, it sends a pong response

@param[in]   req_c_struct        Decoded request message structure
                                 
@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.

@retval    QMI_CSI_CB_NO_ERR        Success 
@retval    QMI_CSI_CB_...           Error, see error codes defined in qmi_csi.h 

*/
/*=========================================================================*/
static qmi_csi_cb_error ping_response
(
  void                     *conn_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
  )
{
  qmi_csi_cb_error rc = QMI_CSI_CB_INTERNAL_ERR;
  qmi_csi_error resp_err;
  /* The response message is small, and can safely be declared on the stack.
         See ping_data_response for a message that is large and uses dynamic allocation.*/
  test_ping_resp_msg_v01 resp; 
  //printf("Ping Service: %s\n",((ping_req_msg_v01*)req_c_struct)->ping);
  memset( &resp, 0, sizeof(test_ping_resp_msg_v01) );
  QMI_MEM_COPY( resp.pong, sizeof(resp.pong), "pong", 4 );
  resp.pong_valid = 1;
  /* The response is very simple and does not take long to compute, so it is
         safe to send it within the callback context, and does not require dispatch
         to a new thread.  See ping_data_ind_registration to see the dispatch of
         message handling */
  resp_err = qmi_csi_send_resp( req_handle, msg_id, &resp, sizeof(resp) );      
  if(resp_err != QMI_CSI_NO_ERR)
  {
    //printf("qmi_csi_send_resp returned error: %d\n",resp_err);
  }else{
    rc = QMI_CSI_CB_NO_ERR;
  }
  return rc;
}

/*=============================================================================
  FUNCTION ping_data_response
=============================================================================*/
/*!
@brief
  This function is called when a ping_data request message is received by
  the service, sending a response message with the same sized data field
  as was in the request message.

@param[in]   req_c_struct        Decoded request message structure
                                 
@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.
 
@retval    QMI_CSI_CB_NO_ERR        Success 
@retval    QMI_CSI_CB_...           Error, see error codes defined in qmi_csi.h  

*/
/*=========================================================================*/
static qmi_csi_cb_error ping_data_response
(
  void                     *conn_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
  )
{
  qmi_csi_cb_error rc = QMI_CSI_CB_INTERNAL_ERR;
  qmi_csi_error resp_err;
  //service_context_type *context = (service_context_type*)service_cookie;
  test_data_req_msg_v01 *req = (test_data_req_msg_v01 *)req_c_struct;
  //printf("Ping Data Len: %d\n",((ping_data_req_msg_v01*)req_c_struct)->data_len);
  /* The response message is large, so is allocated dynamically.*/
  test_data_resp_msg_v01 *resp;

  if(!req_c_struct || !req_c_struct_len)
  {
    return rc;
  }

  /* Freed at the end of this function */
  resp = (test_data_resp_msg_v01*)MALLOC(sizeof(test_data_resp_msg_v01));
  if(!resp)
  {
    return QMI_CSI_CB_NO_MEM;
  }
  memset( resp, 0, sizeof(test_data_resp_msg_v01) );
  resp->data_len = req->data_len;
  resp->data_valid = 1;
  if(resp->data_len > 0 && resp->data_len <= TEST_MED_DATA_SIZE_V01 )
  {
    QMI_MEM_COPY(resp->data, TEST_MED_DATA_SIZE_V01, req->data, resp->data_len);
  }
  /* The response is very simple and does not take long to compute, so it is
         safe to send it within the callback context, and does not require dispatch
         to a new thread.  See ping_data_ind_registration to see the dispatch of
         message handling */
  resp_err = qmi_csi_send_resp( req_handle, msg_id, resp, sizeof(test_data_resp_msg_v01) );
  if(resp_err != QMI_CSI_NO_ERR)
  {
    //printf("qmi_csi_send_resp returned error: %d\n",resp_err);
  }else{
    rc = QMI_CSI_CB_NO_ERR;
  }
  /* Check rc, if error print error and return actual qmi_csi_cb_error value */
  FREE(resp); /* Malloc at top of this function */
  return rc;
}

/*=============================================================================
  FUNCTION ping_large_data_response
=============================================================================*/
/*!
@brief
  This function is called when a ping_large_data request message is received by
  the service, sending a response message with the same sized data field
  as was in the request message.

@param[in]   req_c_struct        Decoded request message structure
                                 
@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.
 
@retval    QMI_CSI_CB_NO_ERR        Success 
@retval    QMI_CSI_CB_...           Error, see error codes defined in qmi_csi.h  

*/
/*=========================================================================*/
static qmi_csi_cb_error ping_large_data_response
(
  void                     *conn_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
  )
{
  qmi_csi_cb_error rc = QMI_CSI_CB_INTERNAL_ERR;
  qmi_csi_error resp_err;
  //service_context_type *context = (service_context_type*)service_cookie;
  test_large_data_req_msg_v01 *req = (test_large_data_req_msg_v01 *)req_c_struct;
  //printf("Ping Data Len: %d\n",((ping_data_req_msg_v01*)req_c_struct)->data_len);
  /* The response message is large, so is allocated dynamically.*/
  test_large_data_resp_msg_v01 *resp;

  if(!req_c_struct || !req_c_struct_len)
  {
    return rc;
  }

  /* Freed at the end of this function */
  resp = (test_large_data_resp_msg_v01*)MALLOC(sizeof(test_large_data_resp_msg_v01));
  if(!resp)
  {
    return QMI_CSI_CB_NO_MEM;
  }
  memset( resp, 0, sizeof(test_large_data_resp_msg_v01) );
  resp->data_len = req->data_len;
  resp->data_valid = 1;
  if(resp->data_len > 0 && resp->data_len <= TEST_LARGE_MAX_DATA_SIZE_V01 )
  {
     QMI_MEM_COPY(resp->data, TEST_LARGE_MAX_DATA_SIZE_V01, req->data, resp->data_len);
  }
  /* The response is very simple and does not take long to compute, so it is
         safe to send it within the callback context, and does not require dispatch
         to a new thread.  See ping_data_ind_registration to see the dispatch of
         message handling */
  resp_err = qmi_csi_send_resp( req_handle, msg_id, resp, sizeof(test_large_data_resp_msg_v01) );
  if(resp_err != QMI_CSI_NO_ERR)
  {
    //printf("qmi_csi_send_resp returned error: %d\n",resp_err);
  }else{
    rc = QMI_CSI_CB_NO_ERR;
  }
  /* Check rc, if error print error and return actual qmi_csi_cb_error value */
  FREE(resp); /* Malloc at top of this function */
  return rc;
}


/*=============================================================================
  FUNCTION ping_data_ind_registration
=============================================================================*/
/*!
@brief
  This function is called when a ping_data_ind_reg request message is received
  by the service, sending a response message and then a series of indications
  depending on what information was passed in the request message

@param[in]   req_c_struct        Decoded request message structure
                                 
@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.
 
@retval    QMI_CSI_CB_NO_ERR        Success 
@retval    QMI_CSI_CB_...           Error, see error codes defined in qmi_csi.h   

*/
/*=========================================================================*/
static qmi_csi_cb_error ping_data_ind_registration
(
  void                     *conn_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
  )
{
  ind_type *ind_params;
  client_info_type         *clnt_info = (client_info_type *)conn_handle;
  
  if(!req_c_struct)
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  ind_params = &clnt_info->ind_test;
  ind_params->req_struct = *(test_data_ind_reg_req_msg_v01*)req_c_struct;
  ind_params->req_handle = req_handle;
  ind_params->msg_id = msg_id;

  return send_indications(clnt_info);
}

static qmi_csi_req_handler_tbl_type req_handle_table[] = 
{
  {QMI_TEST_REQ_V01, 1, ping_response},
  {QMI_TEST_DATA_REQ_V01, 1, ping_data_response},
  {QMI_TEST_LARGE_DATA_REQ_V01, 1, ping_large_data_response},
  {QMI_TEST_DATA_IND_REG_REQ_V01, 1, ping_data_ind_registration}
};

/*=============================================================================
  CALLBACK FUNCTION ping_connect_cb
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
static qmi_csi_cb_error ping_connect_cb
(
  qmi_client_handle         client_handle,
  void                      *service_cookie,
  void                      **connection_handle
  )
{
  /* This is where the client handle can be stored if the service wants
         to keep some state information about clients. The connection handle can also
         be modified to facilitate communication with the infrastructure */
  client_info_type *clnt_info;

  if(!connection_handle)
    return QMI_CSI_CB_CONN_REFUSED;

  /* For any service where it is anticipated that there could be more than one client
         connected at any given time the clients should be stored in some kind of data structure
         to facilitate cleanup and the sending of indications. */
  clnt_info = CALLOC(1, sizeof(client_info_type)); /* Freed in ping_disconnect_cb */
  if(!clnt_info)
    return QMI_CSI_CB_CONN_REFUSED;
  clnt_info->clnt = client_handle;

  *connection_handle = clnt_info;
  return QMI_CSI_CB_NO_ERR;
}

/*=============================================================================
  CALLBACK FUNCTION ping_disconnect_cb
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
static void ping_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_cookie
 )
{
  /* Free up memory for the client */
  if(connection_handle)
    FREE(connection_handle); /* Malloc in ping_connect_cb */
  
  return;
}

/*=============================================================================
  CALLBACK FUNCTION ping_resume_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure when the client
  gets unblocked and is now capable of receiving indications. Note that this
  callback will be called only if a previous call to qmi_csi_send_ind*() fails
  with an error code: QMI_CSI_CONN_BUSY.

@param[in]  client_handle          Handle used by the infrastructure to identify
                                   each client (Passed in by the connect_cb).
@param[in]  connection_handle      Cookie specified by the service to identify
                                   each client (output parameter of connect_cb).
@param[in]  service_cookie         Service specific data.Service cookie is 
                                   registered with the infrastructure during 
                                   service registration(qmi_csi_register).                     
@retval    None
*/
/*=========================================================================*/
static void ping_resume_cb
(
  qmi_client_handle client_handle,
  void *connection_handle,
  void *service_cookie
)
{
  client_info_type *clnt_info = (client_info_type *)connection_handle;

  if(clnt_info->ind_test.num_inds > 0)
  {
    (void)send_indications(clnt_info);
  }
}

/*=============================================================================
  FUNCTION qmi_ping_register_service
=============================================================================*/
void *qmi_ping_register_service(qmi_csi_os_params *os_params)
{
  qmi_idl_service_object_type ping_service_object = test_get_service_object_v01();
  qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;
  qmi_csi_options options;
  
  if(!ping_service_object || !os_params)
  {
    return NULL;
  }
  QMI_CSI_OPTIONS_INIT(options);
  QMI_CSI_OPTIONS_SET_INSTANCE_ID(options, qmi_ping_service_instance);
  QMI_CSI_OPTIONS_SET_RESUME_IND_CB(options, ping_resume_cb);
  QMI_CSI_OPTIONS_SET_REQ_HANDLER_TBL(options, req_handle_table,
      (uint16_t)(sizeof(req_handle_table)/sizeof(req_handle_table[0])));
  
  rc = qmi_csi_register_with_options(ping_service_object, ping_connect_cb, 
      ping_disconnect_cb, NULL, &service_cookie, os_params, 
      &options, &service_cookie.service_handle);
  if(rc != QMI_CSI_NO_ERR)
    return NULL;
  return service_cookie.service_handle;
}

