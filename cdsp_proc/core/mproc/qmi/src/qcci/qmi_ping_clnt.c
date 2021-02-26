/******************************************************************************
  @file    qmi_ping_clnt.c
  @brief   QMI ping client

  DESCRIPTION
  QMI ping client
   
  ---------------------------------------------------------------------------
  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <stdlib.h>
#include "comdef.h"
#include "msg.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "test_service_v01.h"
#include "qurt.h"
#include "qurt_sclk.h"

#define QMI_CLNT_WAIT_SIG  0x00010000
#define QMI_CLNT_TIMER_SIG 0x00000001
#define QMI_CLNT_IND_DONE_SIG 0x00000002
#define QMI_CLNT_ASYNC_DONE_SIG 0x00000004

#define QMI_PING_CLIENT_STACK_SIZE  (2048)

static qurt_thread_attr_t  qmi_ping_client_thread_attr;
static qurt_thread_t       qmi_ping_client_tcb;
static char                *qmi_ping_client_stack = NULL;
static qurt_anysignal_t    qmi_ping_client_sig;


static volatile int pending_inds = 0;
static volatile int pending_async = 0;
static int async_mode;

/*=============================================================================
  CALLBACK FUNCTION ping_ind_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when 
  infrastructure receives an indication for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to 
                 identify different services.
 
@param[in]   msg_id              Message ID of the indication

@param[in]  ind_buf              Buffer holding the encoded indication

@param[in]  ind_buf_len          Length of the encoded indication
 
@param[in]  ind_cb_data          Cookie value supplied by the client during registration

*/
/*=========================================================================*/
void ping_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  --pending_inds;
  if(pending_inds == 0)
  {
    qurt_anysignal_set(&qmi_ping_client_sig, QMI_CLNT_IND_DONE_SIG);
  }
}

/*=============================================================================
  CALLBACK FUNCTION ping_rx_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when 
  infrastructure receives an asynchronous response for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to 
                 identify different services.
 
@param[in]   msg_id              Message ID of the response

@param[in]   buf                 Buffer holding the decoded response
 
@param[in]   len                 Length of the decoded response
 
@param[in]   resp_cb_data        Cookie value supplied by the client

@param[in]   transp_err          Error value
 
*/
/*=========================================================================*/
static void ping_rx_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *buf,
 unsigned int                   len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
 )
{
  --pending_async;
  if(pending_async == 0)
  {
    qurt_anysignal_set(&qmi_ping_client_sig,  QMI_CLNT_ASYNC_DONE_SIG);
  }
}

/*=============================================================================
  FUNCTION ping_basic_test
=============================================================================*/
/*!
@brief
  This function sends a number of basic ping messages asynchronously

@param[in]   clnt                Client handle needed to send messages
                                 
@param[in]   txn                 Transaction handle
 
@param[in]   num_pings           Number of pings to send
 
*/
/*=========================================================================*/
int ping_basic_test
(
 qmi_client_type *clnt,
 qmi_txn_handle *txn,
 int num_pings
 )
{
  int i,rc;
  test_ping_req_msg_v01 req;
  test_ping_resp_msg_v01 resp;

  /* Set the value of the basic ping request */
  memset(&req, 0, sizeof(req));
  QMI_MEM_COPY(&req, sizeof(req), "ping", 4);
  pending_async = num_pings;
  qurt_anysignal_clear(&qmi_ping_client_sig, QMI_CLNT_ASYNC_DONE_SIG);

  for (i=0;i<num_pings;++i)
  {
    if(async_mode)
    {
      rc = qmi_client_send_msg_async(*clnt, QMI_TEST_REQ_V01, &req, sizeof(req),
          &resp, sizeof(resp), ping_rx_cb, (void *)2, txn);
      if (rc != 0)
      {
        return rc;
      }
    }
    else
    {
      rc = qmi_client_send_msg_sync(*clnt, QMI_TEST_REQ_V01, &req, sizeof(req),
          &resp, sizeof(resp), 0);
      if (rc != 0)
      {
        return rc;
      }
    }
  }
  /* Wait until all pending async messages have been received */
  while (async_mode && pending_async != 0)
  {
    qurt_anysignal_wait(&qmi_ping_client_sig, QMI_CLNT_ASYNC_DONE_SIG);
    qurt_anysignal_clear(&qmi_ping_client_sig, QMI_CLNT_ASYNC_DONE_SIG);
  }
  return 0;
}

/*=============================================================================
  FUNCTION ping_data_test
=============================================================================*/
/*!
@brief
  This function sends a number of data ping messages asynchronously

@param[in]   clnt                Client handle needed to send messages
                                 
@param[in]   txn                 Transaction handle
 
@param[in]   num_pings           Number of data messages to send
 
@param[in]   msg_size            Size of data messages to send 
 
*/
/*=========================================================================*/
int ping_data_test
(
 qmi_client_type *clnt,
 qmi_txn_handle *txn,
 int num_msgs,
 int msg_size
 )
{
  int i,rc;
  test_data_req_msg_v01 *data_req;
  test_data_resp_msg_v01 *data_resp;
  data_req = (test_data_req_msg_v01*)MALLOC(sizeof(test_data_req_msg_v01));
  if(!data_req) {
    return QMI_CLIENT_ALLOC_FAILURE;
  }
  data_resp = (test_data_resp_msg_v01*)MALLOC(sizeof(test_data_resp_msg_v01));
  if(!data_resp) {
    FREE(data_req);
    return QMI_CLIENT_ALLOC_FAILURE;
  }

  memset( data_req, 0, sizeof(test_data_req_msg_v01) );
  memset( data_resp, 0, sizeof(test_data_resp_msg_v01) );
  data_req->data_len = msg_size;
  pending_async = num_msgs;
  qurt_anysignal_clear(&qmi_ping_client_sig, QMI_CLNT_ASYNC_DONE_SIG);

  for (i=0;i<num_msgs;++i)
  {
    if(async_mode)
    {
      rc = qmi_client_send_msg_async(*clnt, QMI_TEST_DATA_REQ_V01, data_req, sizeof(test_data_req_msg_v01),
          data_resp, sizeof(test_data_resp_msg_v01), ping_rx_cb, (void *)2, txn);
      if (rc != 0){
        FREE(data_req);
        FREE(data_resp);
        return rc;
      }
    }
    else
    {
      rc = qmi_client_send_msg_sync(*clnt, QMI_TEST_DATA_REQ_V01, data_req, sizeof(test_data_req_msg_v01),
          data_resp, sizeof(test_data_resp_msg_v01), 0);
      if (rc != 0){
        FREE(data_req);
        FREE(data_resp);
        return rc;
      }
    }
  }
  /* Wait until all pending async messages have been received */
  while (async_mode && pending_async != 0)
  {
    qurt_anysignal_wait(&qmi_ping_client_sig, QMI_CLNT_ASYNC_DONE_SIG);
    qurt_anysignal_clear(&qmi_ping_client_sig, QMI_CLNT_ASYNC_DONE_SIG);
  }
  FREE(data_req);
  FREE(data_resp);
  return 0;
}

/*=============================================================================
  FUNCTION ping_ind_test
=============================================================================*/
/*!
@brief 
  This function tells the service to send a specified number of indication messages

@param[in]   clnt                Client handle needed to send messages
                                 
@param[in]   txn                 Transaction handle
 
@param[in]   num_inds            Number of indications for the service to send 
 
@param[in]   ind_size            Size of indications for the service to send 
 
@param[in]   delay               Amount of time the server should wait between indications
 
*/
/*=========================================================================*/
int ping_ind_test
(
 qmi_client_type *clnt,
 qmi_txn_handle *txn,
 int num_inds,
 int ind_size,
 int delay
 )
{
  int rc;
  test_data_ind_reg_req_msg_v01 data_ind_reg_req;
  test_data_ind_reg_resp_msg_v01 data_ind_reg_resp;

  /* Set the number of pending indications */
  pending_inds = num_inds;
  qurt_anysignal_clear(&qmi_ping_client_sig, QMI_CLNT_IND_DONE_SIG);
  memset( &data_ind_reg_req, 0, sizeof(test_data_ind_reg_req_msg_v01) );
  memset( &data_ind_reg_resp, 0, sizeof(test_data_ind_reg_resp_msg_v01) );
  data_ind_reg_req.num_inds_valid = 1;
  data_ind_reg_req.num_inds = num_inds;
  /* Send the optional TLVs if these values are passed as arguments */
  if (delay > 0)
  {
    data_ind_reg_req.ms_delay_valid = 1;
    data_ind_reg_req.ms_delay = delay;
  }
  if (ind_size > 0)
  {
    data_ind_reg_req.ind_size_valid = 1;
    data_ind_reg_req.ind_size = ind_size;
  }
  rc = qmi_client_send_msg_sync(*clnt, QMI_TEST_DATA_IND_REG_REQ_V01, &data_ind_reg_req, 
      sizeof(data_ind_reg_req),&data_ind_reg_resp, 
      sizeof(data_ind_reg_resp), 0);
  if (rc != 0){    
    return rc;
  }
  /* Wait until all pending indications have been received */
  while (pending_inds != 0)
  {
    qurt_anysignal_wait(&qmi_ping_client_sig, QMI_CLNT_IND_DONE_SIG);
    qurt_anysignal_clear(&qmi_ping_client_sig, QMI_CLNT_IND_DONE_SIG);
  }
  return 0;
}

static int qmi_ping_test_passed[] = {
  -500, -500, -500, -500, -500, -500
};

void qmi_ping_client_thread(void *unused)
{
  qmi_client_type clnt;
  qmi_txn_handle txn;
  qmi_client_type notifier;
  unsigned int num_services, num_entries=10, i=0, num_services_old=0;
  int rc;
  qmi_cci_os_signal_type os_params;
  qmi_service_info info[10];
  qmi_idl_service_object_type ping_service_object = test_get_service_object_v01();

  os_params.ext_signal = NULL;
  os_params.sig = QMI_CLNT_WAIT_SIG;
  os_params.timer_sig = QMI_CLNT_TIMER_SIG;

  rc = qmi_client_notifier_init(ping_service_object, &os_params, &notifier);

  /* Check if the service is up, if not wait on a signal */
  while(1)
  {
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);

    /* The server has come up, store the information in info variable */
    num_entries=10;
    rc = qmi_client_get_service_list( ping_service_object, info, &num_entries, &num_services);

    if(rc != QMI_NO_ERR || num_services == num_services_old)
      continue;

    num_services_old = num_services;

    for(i = 0; i < num_services; i++)
    {
      qmi_service_instance instance = 0;
      rc = qmi_client_init(&info[i], ping_service_object, ping_ind_cb, NULL, &os_params, &clnt);
      qmi_client_get_instance_id(&info[i],&instance);
      MSG_1(MSG_SSID_ONCRPC, MSG_LEGACY_HIGH,
            "PING: Starting Ping Tests on instance %d", instance);
      qmi_ping_test_passed[0] = rc = ping_basic_test(&clnt,&txn,1);
      if(rc)
      {
        goto bail;
      }

      qmi_ping_test_passed[1] = rc = ping_basic_test(&clnt,&txn,10);
      if(rc)
      {
        goto bail;
      }

      qmi_ping_test_passed[2] = rc = ping_data_test(&clnt, &txn, 1, 1024);
      if(rc) 
      {
        goto bail;
      }

      qmi_ping_test_passed[3] = rc = ping_data_test(&clnt, &txn, 10, 1024);
      if(rc)
      {
        goto bail;
      }

      qmi_ping_test_passed[4] = rc = ping_ind_test(&clnt, &txn, 1, 1024, 10);
      if(rc)
      {
        goto bail;
      }

      qmi_ping_test_passed[5] = rc = ping_ind_test(&clnt, &txn, 10, 1024, 10);
      if(rc)
      {
        goto bail;
      }

bail:
      MSG_2(MSG_SSID_ONCRPC, MSG_LEGACY_HIGH, 
            "PING: Tests on instance %d returned %d", instance, rc);
      qmi_client_release(clnt);
    }
  }
  /* Not reached */
  // rc = qmi_client_release(notifier);
  // MSG_HIGH("PING: qmi_client_release notifier returned %d\n", rc, 0,0);
}

void qmi_ping_client_start(void)
{
  qurt_anysignal_init(&qmi_ping_client_sig);
  qurt_thread_attr_init(&qmi_ping_client_thread_attr);
  qurt_thread_attr_set_name(&qmi_ping_client_thread_attr, "QMI_PING_CLNT");
  qurt_thread_attr_set_priority(&qmi_ping_client_thread_attr, 10);
  qurt_thread_attr_set_stack_size(&qmi_ping_client_thread_attr, QMI_PING_CLIENT_STACK_SIZE);
  qmi_ping_client_stack = malloc(QMI_PING_CLIENT_STACK_SIZE);
  if(!qmi_ping_client_stack)
  {
    /* This might be too early in boot for print messages to be shown */
    return;
  }
  qurt_thread_attr_set_stack_addr(&qmi_ping_client_thread_attr, qmi_ping_client_stack);
  qurt_thread_create(&qmi_ping_client_tcb, &qmi_ping_client_thread_attr, 
      qmi_ping_client_thread, NULL);
}
