/******************************************************************************
  -----------------------------------------------------------------------------
  Copyright (c) 2012,2015,2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_test_service_clnt_common.h"
#include "test_service_v01.h"

static volatile int test_completed = 0;
static volatile uint32_t rx_err = 0;
static struct itimerval  timer_test_completion;
static struct itimerval rx_timeout;
static int pending_async = 0;
pthread_mutex_t async_count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t async_count_cond = PTHREAD_COND_INITIALIZER;

static inline int zero_timer(struct itimerval *val)
{
  val->it_value.tv_sec = 0;
  val->it_value.tv_usec = 0;
  val->it_interval.tv_sec = 0;
  val->it_interval.tv_usec = 0;

  return setitimer(ITIMER_REAL, val, NULL);
}

static inline int set_timer_ms(struct itimerval *val, int ms)
{
  val->it_value.tv_sec = ms/1000;
  val->it_value.tv_usec = (ms*1000) % 1000000;
  val->it_interval.tv_sec = 0;
  val->it_interval.tv_usec = 0;

  return setitimer(ITIMER_REAL, val, NULL);
}

/*===========================================================================
  FUNCTION   test_completion_timer_cb_func
===========================================================================*/
/*!
@brief
  Test is complete
@return
  N/A

@note

*/
/*=========================================================================*/
static void test_completion_timer_cb_func(int a)
{
  int ret;
  test_completed=1;

  ret = zero_timer(&timer_test_completion);
  if(ret)
  {
    perror("setitimer");
  }
}


/*=============================================================================
  CALLBACK FUNCTION test_clnt_error_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives an REMOVE SERVER message from the modem
*/
void test_error_cb(
  qmi_client_type clnt,
  qmi_client_error_type error,
  void *error_cb_data)
{
  printf("%s: with %d called for %p\n", __func__, error, (void *)clnt);
  pthread_mutex_lock(&async_count_mutex);
  pending_async = 0;
  pthread_cond_signal(&async_count_cond);
  pthread_mutex_unlock(&async_count_mutex);
}

/*===========================================================================
  FUNCTION   rx_timeout_cb_func
===========================================================================*/
/*!
@brief
  This callback function will be called when timer expires.
  Used for testing delete_async_txn test
@return
  N/A

@note

*/
/*=========================================================================*/
static void rx_timeout_cb_func(int a)
{
  int ret;

  printf("TIMEOUT Kicked in!\n");
  printf("async pending: %d\n", pending_async);

  pending_async = 0;
  pthread_cond_signal(&async_count_cond);
}

/*=============================================================================
  CALLBACK FUNCTION test_rx_cb
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
void test_rx_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *buf,
 unsigned int                   len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
 )
{
  int i;
  uint32_t rx_csum = 0, calc_csum = 0;
  test_data_resp_msg_v01 *data_resp = (test_data_resp_msg_v01 *)buf;
  uint8_t *byte_data = (uint8_t *)(data_resp->data);

  memcpy(&rx_csum, byte_data, sizeof(uint32_t));
#ifdef QMI_TEST_CHECKSUM
  for (i = 0; i < data_resp->data_len; i++)
    calc_csum = calc_csum ^ byte_data[i + 4];

  if (rx_csum != calc_csum)
  {
    printf("rx_csum (%d) != calc_csum (%d)\n", rx_csum, calc_csum);
    printf("RX_data\n");
    for(i = 0; i < data_resp->data_len; i++)
    {
      printf("%02x ", byte_data[i]);
    }
  }
#endif

  pthread_mutex_lock(&async_count_mutex);
  pending_async--;
  pthread_cond_signal(&async_count_cond);
  pthread_mutex_unlock(&async_count_mutex);
}

/* ============================================================================
**  FUNCTION test_get_raw_buffer
** ==========================================================================*/
/*!
@brief
  This function is called to allocate, initialize memory for raw message.

@param[in,out]  buf       Address to a Pointer of the buffer.

@param[in]      len       Pointer to the buffer length.

@param[in]      msg_id    Message ID.

@param[in]      idl_type  IDL Type.

@par Dependencies
  None

@par Side Effects
  Allocate memory from heap.

@return
  0 if SUCCESS
  1 if FAILURE

*/
/*===========================================================================*/
static int test_get_raw_buffer(
  void         **buf,
  uint32_t     *len,
  uint32_t     msg_id,
  qmi_idl_type_of_message_type idl_type)
{
  qmi_idl_service_object_type test_svc_obj = NULL;
  qmi_client_error_type ret  = QMI_NO_ERR;

  if (NULL == len)
  {
    printf("Length cannot be NULL\n");
    return 1;
  }

  /* Get the service object for the test service */
  test_svc_obj = test_get_service_object_v01();
  if (!test_svc_obj)
  {
    printf("test_get_service_object_v01 returned NULL\n");
    return 1;
  }

  /*Get the length of raw buffer that need to be allocated */
  ret = qmi_idl_get_max_message_len(test_svc_obj, idl_type,
            msg_id, (uint32_t *)len);
  if (QMI_NO_ERR != ret)
  {
    printf("qmi_idl_get_max_message_len for msg failed\n");
    return 1;
  }

  /* Allocate memory to buffer, freed by called function */
  *buf = (void *)calloc(1, *len);
  if (NULL == *buf)
  {
    printf("Allocating buffer failed, calloc returned");
    return 1;
  }
  return 0;
}

/* ============================================================================
**  FUNCTION   test_encode_raw_msg
** ========================================================================== */
/*
@brief : test_encode_raw_msg
  This function is called to encode message into raw format to be sent
  to service point via call to qmi_client_send_raw_msg_sync
  or qmi_client_send_raw_async.

@param[in]   clnt            Handle used by the infrastructure to
                             identify different clients.

@param[in]   req_msg         Pointer to request message.

@param[in]   buf             Address to a pointer of the raw request.

@param[in]   len             Pointer to the length of raw request.

@par Dependencies
  None

@par Side Effects
  None

@return
  0 if SUCCESS
  1 if FAILURE

*/
/* ========================================================================== */
static int test_encode_raw_msg(qmi_client_type clnt,
              test_data_req_msg_v01 *req_msg, void **buf, uint32_t *len)
{
  qmi_client_error_type ret = QMI_NO_ERR;
  uint32_t encoded_len = 0;
  int rc;

  /* Allocate memory for raw request buffer */
  rc = test_get_raw_buffer(buf, len, QMI_TEST_DATA_REQ_V01, QMI_IDL_REQUEST);
  if (rc)
  {
    printf("Allocating buffer, test_get_raw_buffer returned");
    return 1;
  }

  /*Encode C message to raw format */
  ret = qmi_client_message_encode(clnt, QMI_IDL_REQUEST, QMI_TEST_DATA_REQ_V01,
            req_msg, sizeof(test_data_req_msg_v01), *buf, *len, &encoded_len);
  if (QMI_NO_ERR != ret)
  {
    printf("Message encoding failed, returned:%d\n", ret);
    free(*buf);
    return 1;
  }

  *len = encoded_len;
  return 0;
}

/* ============================================================================
**  FUNCTION  test_decode_raw_msg
** ========================================================================== */
/*
@brief :
This function is called to decode raw message received from the service.
Service echoes the data sent by client, along with the response status.

@param[in]   clnt        Handle used by the infrastructure to
                         identify different clients.

@param[in]   msg_id      Message ID.

@param[in]   resp_msg    Pointer to Decoded Message.

@param[in]   buf         Pointer to the raw response.

@param[in]   len         Length of the response.

@par Dependencies
  None

@par Side Effects
  None

@return
  0 if SUCCESS
  1 if FAILURE

*/
/* ========================================================================== */
static int test_decode_raw_msg(qmi_client_type clnt, uint32_t msg_id,
              test_data_resp_msg_v01  *resp_msg, void *buf, uint32_t len)
{
  qmi_client_error_type  ret  = QMI_NO_ERR;

  if (NULL == clnt)
  {
    printf("Client Handle passed is NULL\n");
    return 1;
  }
  if (NULL == buf)
  {
    printf("Buffer is NULL\n");
    return 1;
  }

  /* Decode the raw message to response message type */
  ret = qmi_client_message_decode(clnt, QMI_IDL_RESPONSE, msg_id, buf, len,
          (void *)resp_msg, sizeof(test_data_resp_msg_v01));
  if (QMI_NO_ERR != ret)
  {
    printf("Message Decoding Failed, returned:%d\n", ret);
    return 1;
  }
  return 0;
}

/*=============================================================================
  CALLBACK FUNCTION test_raw_rx_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives an asynchronous raw response for this client

@param[in]   user_handle        Opaque handle used by the infrastructure to
                                identify different services.

@param[in]   msg_id             Message ID of the response

@param[in]   buf                Buffer holding the decoded response

@param[in]   len                Length of the decoded response

@param[in]   resp_cb_data       Cookie value supplied by the client

@param[in]   transp_err         Error value

*/
/*=========================================================================*/
void test_raw_rx_cb(qmi_client_type user_handle, unsigned int msg_id,
        void *buf, unsigned int len, void *resp_cb_data,
        qmi_client_error_type transp_err)
{
  int i, rc;
  uint32_t rx_csum = 0, calc_csum = 0;
  test_data_resp_msg_v01 *data_resp;

  /* Allocate C Msg Buffer */
  data_resp = (test_data_resp_msg_v01 *)calloc(1, sizeof(test_data_resp_msg_v01));
  if(!data_resp)
  {
    printf("data_resp memory alloc failure!\n");
    rx_err++;
    goto endofcb;
  }

  /*Decode Raw Message*/
  rc = test_decode_raw_msg(user_handle,QMI_TEST_DATA_RESP_V01, data_resp, buf, (uint32_t)len);
  if (1 == rc)
  {
    printf("Message decode failed!\n");
    rx_err++;
    goto endofcb;
  }

  uint8_t *byte_data = (uint8_t *)(data_resp->data);
  memcpy(&rx_csum, byte_data, sizeof(uint32_t));

#ifdef QMI_TEST_CHECKSUM
  for (i = 0; i < data_resp->data_len; i++)
    calc_csum = calc_csum ^ byte_data[i + 4];

  if (rx_csum != calc_csum)
  {
    printf("rx_csum (%d) != calc_csum (%d)\n", rx_csum, calc_csum);
    printf("RX_data\n");
    for(i = 0; i < data_resp->data_len; i++)
    {
      printf("%02x ", byte_data[i]);
    }
  }
  printf("Data decode verified.\n");
#endif
endofcb:
  pthread_mutex_lock(&async_count_mutex);
  pending_async--;
  pthread_cond_signal(&async_count_cond);
  pthread_mutex_unlock(&async_count_mutex);
  free(data_resp);
}

/*===========================================================================
  FUNCTION  qmi_test_clnt_common_sync_basic_test
===========================================================================*/
/*!
@brief
  Common sync basic test
  Runs a qmi test sync basic test for with parameters:
  test_length : max number of qmi transactions
  data_size_bytes: size of data block to send
  test_duration_ms: duration of test

@return
  0 for pass, -1 for fail
  returns statistics of test in  qmi_test_clnt_results_type

@note
  - Dependencies  qmi_test_clnt_results_type must be pre-allocated

  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_sync_basic_test(
  qmi_client_type *clnt,
  uint32_t test_length,
  uint32_t test_duration_ms,
  uint32_t verbose,
  qmi_test_clnt_results_type *results
  )
{
  uint32_t i, ret, errcnt = 0;
  struct timespec time_info_start;
  struct timespec time_info_end;
  struct timespec time_info_last;
  struct timespec time_info_current;
  uint32_t num_loops = 0;
  test_ping_req_msg_v01 req;
  test_ping_resp_msg_v01 resp;

  test_completed = 0;

  req.client_name_valid = 0;
  memcpy(&req, "ping", 4);
  /* If the test is specified with a test_duration_ms
     Then execute the test for this duration, if not,
     loop for test_length */

  clock_gettime(CLOCK_REALTIME,&time_info_last);
  if((0 == test_duration_ms) && (0 == test_length))
  {
    return 0;
  }
  else if(0 == test_duration_ms)
  {
    clock_gettime(CLOCK_REALTIME,&time_info_start);
    for(i = 0;i < test_length; i++)
    {
      ret = qmi_client_send_msg_sync(*clnt, QMI_TEST_REQ_V01, &req, sizeof(req),
                                      &resp, sizeof(resp), 0);
      if (ret != 0)
      {
        printf("TEST: send_msg_sync error: %d\n",ret);
        errcnt++;
      }
    }
    clock_gettime(CLOCK_REALTIME,&time_info_end);
    qmi_test_clnt_common_calc_stats_timespec((test_length - errcnt), 4, 4,
                                           &time_info_start, &time_info_end, results);
  }
  else
  {
    signal(SIGALRM, test_completion_timer_cb_func);
    ret = set_timer_ms(&timer_test_completion, test_duration_ms);
    if(ret) {
      perror("setitimer");
      errcnt++;
      return -errcnt;
    }

    /* Test ran for a fixed period of time*/
    clock_gettime(CLOCK_REALTIME,&time_info_start);
    while((0 == test_completed) && (0 == errcnt))
    {
      ret = qmi_client_send_msg_sync(*clnt, QMI_TEST_REQ_V01, &req, sizeof(req),
                                      &resp, sizeof(resp), 0);
      if (ret != 0)
      {
        printf("TEST: send_msg_sync error: %d\n",ret);
        errcnt++;
      }
      num_loops++;

      if(verbose)
      {
        clock_gettime(CLOCK_REALTIME,&time_info_current);
        if(time_info_current.tv_sec > (time_info_last.tv_sec))
        {
          time_info_last.tv_sec  = time_info_current.tv_sec ;
          printf("QMI txns completed: %5d Num Errors:%d\n",(int)(num_loops - errcnt),(int)errcnt);
        }
      }

#ifdef FEATURE_QMI_TEST_TIMER_BASED_TEST
      if(num_loops >= test_length)
      {
        test_completed = 1;
      }
#endif
    }
    clock_gettime(CLOCK_REALTIME,&time_info_end);
    if(errcnt == 0)
    {
      qmi_test_clnt_common_calc_stats_timespec(num_loops, 4, 4,
                                             &time_info_start, &time_info_end, results);
      results->rc = -errcnt;
    }
    else
    {
      memset(results, 0, sizeof(qmi_test_clnt_results_type));
      results->rc = -errcnt;
    }
    ret = zero_timer(&timer_test_completion);
    if(ret) {
      perror("setitimer");
      errcnt++;
      return -errcnt;
    }
  }
  return(-errcnt);
}


/*===========================================================================
  FUNCTION  qmi_test_clnt_common_sync_data_test
===========================================================================*/
/*!
@brief
  Common sync data test
  Runs a qmi test sync data test for with parameters:
  test_length : max number of qmi transactions
  data_size_bytes: size of data block to send
  test_duration_ms: duration of test

@return
  0 for pass, -1 for fail
  returns statistics of test in  qmi_test_clnt_results_type

@note
  - Dependencies  qmi_test_clnt_results_type must be pre-allocated

  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_sync_data_test(
  qmi_client_type *clnt,
  uint32_t test_length,
  uint32_t data_size_bytes,
  uint32_t test_duration_ms,
  uint32_t verbose,
  qmi_test_clnt_results_type *results
  )
{
  uint32_t sum = 0, ret_sum;
  uint32_t i, ret, errcnt = 0;
  struct timespec time_info_start;
  struct timespec time_info_end;
  struct timespec time_info_last;
  struct timespec time_info_current;
  uint32_t num_loops = 0;
  uint8_t byte_data = 0;
  test_data_req_msg_v01 *data_req;
  test_data_resp_msg_v01 *data_resp;

  test_completed = 0;
  data_req = (test_data_req_msg_v01 *)calloc(1, sizeof(test_data_req_msg_v01));
  if(!data_req)
    return -1;
  data_resp = (test_data_resp_msg_v01 *)calloc(1, sizeof(test_data_resp_msg_v01));
  if(!data_resp)
  {
    free(data_req);
    return -1;
  }

  /* Setup req data block & checksum to it*/
  for(i = 0; i < data_size_bytes; i++)
  {
    byte_data = (uint8_t)(rand() & 0xff);
    data_req->data[i + 4] = byte_data;
    sum = sum ^ byte_data;
  }
  memcpy(data_req->data, &sum, sizeof(uint32_t));
  data_req->data_len = (data_size_bytes + 4);

  /* If the test is specified with a test_duration_ms
     Then execute the test for this duration, if not,
     loop for test_length */

  clock_gettime(CLOCK_REALTIME,&time_info_last);
  if((0 == test_duration_ms) && (0 == test_length))
  {
    free(data_req);
    free(data_resp);
    return 0;
  }
  else if(0 == test_duration_ms)
  {
    clock_gettime(CLOCK_REALTIME,&time_info_start);
    for(i = 0;i < test_length; i++)
    {
      ret = qmi_client_send_msg_sync(*clnt, QMI_TEST_DATA_REQ_V01, data_req, sizeof(test_data_req_msg_v01),
                                      data_resp, sizeof(test_data_resp_msg_v01), 0);
      if (ret != 0)
      {
        printf("TEST: send_msg_sync error: %d\n",ret);
        errcnt++;
      }
      memcpy(&ret_sum, data_resp->data, sizeof(uint32_t));
#ifdef QMI_TEST_CHECKSUM
      if (ret_sum != sum)
      {
        printf("TEST: send_msg_sync_error: Checksum mismatch\n");
        printf("tx_csum (%d) != rx_csum(%d)\n", sum, ret_sum);
        errcnt++;
      }
#endif
    }
    clock_gettime(CLOCK_REALTIME,&time_info_end);
    qmi_test_clnt_common_calc_stats_timespec((test_length - errcnt), data_size_bytes, data_size_bytes,
                                           &time_info_start, &time_info_end, results);
  }
  else
  {
    signal(SIGALRM, test_completion_timer_cb_func);
    ret = set_timer_ms(&timer_test_completion, test_duration_ms);
    if(ret)
    {
      perror("setitimer");
      errcnt++;
      free(data_req);
      free(data_resp);
      return -errcnt;
    }

    /* Test ran for a fixed period of time*/
    clock_gettime(CLOCK_REALTIME,&time_info_start);
    while((0 == test_completed) && (0 == errcnt))
    {
      ret = qmi_client_send_msg_sync(*clnt, QMI_TEST_DATA_REQ_V01, data_req, sizeof(test_data_req_msg_v01),
                                      data_resp, sizeof(test_data_resp_msg_v01), 0);
      if (ret != 0)
      {
        printf("TEST: send_msg_sync error: %d\n",ret);
        errcnt++;
      }
      memcpy(&ret_sum, data_resp->data, sizeof(uint32_t));
#ifdef QMI_TEST_CHECKSUM
      if (ret_sum != sum)
      {
        printf("TEST: send_msg_sync_error: Checksum mismatch\n");
        printf("tx_csum (%d) != rx_csum(%d)\n", sum, ret_sum);
        errcnt++;
      }
#endif
      num_loops++;

      if(verbose)
      {
        clock_gettime(CLOCK_REALTIME,&time_info_current);
        if(time_info_current.tv_sec > (time_info_last.tv_sec))
        {
          time_info_last.tv_sec  = time_info_current.tv_sec ;
          printf("QMI txns completed: %5d Num Errors:%d\n",(int)(num_loops - errcnt),(int)errcnt);
        }
      }

#ifdef FEATURE_QMI_TEST_TIMER_BASED_TEST
      if(num_loops >= test_length)
      {
        test_completed = 1;
      }
#endif
    }
    clock_gettime(CLOCK_REALTIME,&time_info_end);
    if(errcnt == 0)
    {
      qmi_test_clnt_common_calc_stats_timespec(num_loops, data_size_bytes , data_size_bytes,
                                             &time_info_start, &time_info_end, results);
      results->rc = -errcnt;
    }
    else
    {
      memset(results, 0, sizeof(qmi_test_clnt_results_type));
      results->rc = -errcnt;
    }
    ret = zero_timer(&timer_test_completion);
    if(ret)
    {
      perror("setitimer");
      errcnt++;
      free(data_req);
      free(data_resp);
      return -errcnt;
    }
  }
  free(data_req);
  free(data_resp);

  return(-errcnt);
}


/*===========================================================================
  FUNCTION  qmi_test_clnt_common_async_data_test
===========================================================================*/
/*!
@brief
  Common async data test
  Runs a qmi test async data test for with parameters:
  test_length : max number of qmi transactions
  data_size_bytes: size of data block to send
  test_duration_ms: duration of test

@return
  0 for pass, -1 for fail
  returns statistics of test in  qmi_test_clnt_results_type

@note
  - Dependencies  qmi_test_clnt_results_type must be pre-allocated

  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_async_data_test(
  qmi_client_type *clnt,
  qmi_txn_handle *txn,
  uint32_t test_length,
  uint32_t data_size_bytes,
  uint32_t test_duration_ms,
  uint32_t verbose,
  qmi_test_clnt_results_type *results
  )
{
  uint32_t sum = 0;
  uint32_t i, ret, errcnt = 0, cb_data = 2;
  struct timespec time_info_start;
  struct timespec time_info_end;
  struct timespec time_info_last;
  struct timespec time_info_current;
  uint32_t num_loops = 0;
  uint8_t byte_data = 0;
  test_data_req_msg_v01 *data_req;
  test_data_resp_msg_v01 *data_resp;

  test_completed = 0;
  data_req = (test_data_req_msg_v01 *)calloc(1, sizeof(test_data_req_msg_v01));
  if(!data_req)
    return -1;
  data_resp = (test_data_resp_msg_v01 *)calloc(1, sizeof(test_data_resp_msg_v01));
  if(!data_resp)
  {
    free(data_req);
    return -1;
  }

  /* Setup req data block & checksum to it*/
  for(i = 0; i < data_size_bytes; i++)
  {
    byte_data = (uint8_t)(rand() & 0xff);
    data_req->data[i + 4] = byte_data;
    sum = sum ^ byte_data;
  }
  memcpy(data_req->data, &sum, sizeof(uint32_t));
  data_req->data_len = (data_size_bytes + 4);

  /* If the test is specified with a test_duration_ms
     Then execute the test for this duration, if not,
     loop for test_length */

  clock_gettime(CLOCK_REALTIME,&time_info_last);
  if((0 == test_duration_ms) && (0 == test_length))
  {
    free(data_req);
    free(data_resp);
    return 0;
  }
  else if(0 == test_duration_ms)
  {
    clock_gettime(CLOCK_REALTIME,&time_info_start);
    for(i = 0;i < test_length; i++)
    {
      ret = qmi_client_send_msg_async(*clnt, QMI_TEST_DATA_REQ_V01, data_req, sizeof(test_data_req_msg_v01),
                                      data_resp, sizeof(test_data_resp_msg_v01), test_rx_cb, (void*)&cb_data, txn);
      if (ret != 0)
      {
        printf("TEST: send_msg_async error: %d\n",ret);
        errcnt++;
      }
      else
      {
        pthread_mutex_lock(&async_count_mutex);
        pending_async++;
        pthread_mutex_unlock(&async_count_mutex);
      }
    }
    pthread_mutex_lock(&async_count_mutex);
    while (pending_async)
      pthread_cond_wait(&async_count_cond, &async_count_mutex);
    pthread_mutex_unlock(&async_count_mutex);

    clock_gettime(CLOCK_REALTIME,&time_info_end);
    qmi_test_clnt_common_calc_stats_timespec((test_length - errcnt), data_size_bytes, data_size_bytes,
                                           &time_info_start, &time_info_end, results);
  }
  else
  {
    signal(SIGALRM, test_completion_timer_cb_func);
    ret = set_timer_ms(&timer_test_completion, test_duration_ms);
    if(ret)
    {
      perror("setitimer");
      errcnt++;
      free(data_req);
      free(data_resp);
      return -errcnt;
    }

    /* Test ran for a fixed period of time*/
    clock_gettime(CLOCK_REALTIME,&time_info_start);
    while((0 == test_completed) && (0 == errcnt))
    {
      ret = qmi_client_send_msg_async(*clnt, QMI_TEST_DATA_REQ_V01, data_req, sizeof(test_data_req_msg_v01),
                                      data_resp, sizeof(test_data_resp_msg_v01), test_rx_cb, (void*)&cb_data, txn);
      if (ret != 0)
      {
        printf("TEST: send_msg_async error: %d\n",ret);
        errcnt++;
      }
      else
      {
        pthread_mutex_lock(&async_count_mutex);
        pending_async++;
        pthread_mutex_unlock(&async_count_mutex);
      }
      num_loops++;

      if(verbose)
      {
        clock_gettime(CLOCK_REALTIME,&time_info_current);
        if(time_info_current.tv_sec > (time_info_last.tv_sec))
        {
          time_info_last.tv_sec  = time_info_current.tv_sec ;
          printf("QMI txns completed: %5d Num Errors:%d\n",(int)(num_loops - (errcnt + pending_async)),(int)errcnt);
        }
      }

#ifdef FEATURE_QMI_TEST_TIMER_BASED_TEST
      if(num_loops >= test_length)
      {
        test_completed = 1;
      }
#endif
    }
    pthread_mutex_lock(&async_count_mutex);
    while (pending_async)
      pthread_cond_wait(&async_count_cond, &async_count_mutex);
    pthread_mutex_unlock(&async_count_mutex);

    clock_gettime(CLOCK_REALTIME,&time_info_end);

    if(errcnt == 0)
    {
      qmi_test_clnt_common_calc_stats_timespec(num_loops, data_size_bytes , data_size_bytes,
                                             &time_info_start, &time_info_end, results);
      results->rc = -errcnt;
    }
    else
    {
      memset(results, 0, sizeof(qmi_test_clnt_results_type));
      results->rc = -errcnt;
    }
    ret = zero_timer(&timer_test_completion);
    if(ret)
    {
      perror("setitimer");
      errcnt++;
      free(data_req);
      free(data_resp);
      return -errcnt;
    }
  }
  free(data_req);
  free(data_resp);

  return(-errcnt);
}


/*===========================================================================
  FUNCTION  qmi_test_clnt_common_sync_raw_data_test
===========================================================================*/
/*!
@brief
  Common sync raw data test
  Runs a qmi test sync data test for with parameters:
  test_length : max number of qmi transactions
  data_size_bytes: size of data block to send
  test_duration_ms: duration of test

@return
  0 if SUCCESS
  -1 if FAILURE
  returns statistics of test in  qmi_test_clnt_results_type

@note
  - Dependencies  qmi_test_clnt_results_type must be pre-allocated

  - Side Effects NONE
*/
/*=========================================================================*/
int qmi_test_clnt_common_sync_raw_data_test( qmi_client_type *clnt,
      uint32_t test_length, uint32_t data_size_bytes,
      uint32_t test_duration_ms, uint32_t verbose,
      qmi_test_clnt_results_type *results)
{
  uint32_t sum = 0, ret_sum;
  uint32_t i, ret, errcnt = 0;
  struct timespec time_info_start;
  struct timespec time_info_end;
  struct timespec time_info_last;
  struct timespec time_info_current;
  uint32_t num_loops = 0;
  uint8_t byte_data = 0;

  /*data info*/
  test_data_req_msg_v01 *data_req;
  test_data_resp_msg_v01 *data_resp;

  /*raw buffer info*/
  uint32_t    iRespBufLen = 0;
  uint32_t    iReqBufLen = 0;
  uint32_t    iRespBufRecvLen = 0;
  void      *pReqBuf  = NULL;
  void      *pRespBuf = NULL;
  int       rc;

  test_completed = 0;

  /* Allocate C Msg Buffer */
  data_req = (test_data_req_msg_v01 *)calloc(1, sizeof(test_data_req_msg_v01));
  if(!data_req)
  {
    printf("Error: data_req memory alloc failure!\n");
    return -1;
  }
  data_resp = (test_data_resp_msg_v01 *)calloc(1, sizeof(test_data_resp_msg_v01));
  if(!data_resp)
  {
    printf("Error: data_resp memory alloc failure!\n");
    free(data_req);
    return -1;
  }

  /* Setup req data block & checksum to it*/
  for(i = 0; i < data_size_bytes; i++)
  {
    byte_data = (uint8_t)(rand() & 0xff);
    data_req->data[i + 4] = byte_data;
    sum = sum ^ byte_data;
  }
  memcpy(data_req->data, &sum, sizeof(uint32_t));
  data_req->data_len = (data_size_bytes + 4);

  /* Encode C Msg to raw format */
  rc = test_encode_raw_msg(*clnt, data_req, &pReqBuf, &iReqBufLen);
  if (-1 == rc)
  {
    printf("Error: data encode failed!\n");
    free(data_req);
    free(data_resp);
    return -1;
  }

  /* Allocate memory for raw response buffer */
  rc = test_get_raw_buffer(&pRespBuf, &iRespBufLen, QMI_TEST_DATA_RESP_V01, QMI_IDL_RESPONSE);
  if (-1 == rc)
  {
    printf("Error: allocating resp buffer!\n");
    free(data_req);
    free(data_resp);
    free(pReqBuf);
    return -1;
  }

  /* If the test is specified with a test_duration_ms
   * Then execute the test for this duration, if not,
   * loop for test_length
   */
  clock_gettime(CLOCK_REALTIME,&time_info_last);
  if((0 == test_duration_ms) && (0 == test_length))
  {
    printf("Error: test_duration && test_length zero!");
    free(data_req);
    free(data_resp);
    free(pReqBuf);
    free(pRespBuf);
    return -1;
  }
  else if(0 == test_duration_ms)
  {
    clock_gettime(CLOCK_REALTIME,&time_info_start);
    for(i = 0;i < test_length; i++)
    {
      /*send raw message*/
      ret = qmi_client_send_raw_msg_sync(*clnt, QMI_TEST_DATA_REQ_V01, pReqBuf,
              iReqBufLen, pRespBuf, iRespBufLen, &iRespBufRecvLen, 0);
      if (ret != 0)
      {
        printf("Error: send_raw_msg_sync error: %d\n",ret);
        errcnt++;
        continue;
      }

      /*Decode Raw Message*/
      rc = test_decode_raw_msg(*clnt,QMI_TEST_DATA_RESP_V01, data_resp,
              pRespBuf, iRespBufRecvLen);
      if (-1 == rc)
      {
        printf("Error: Message decode failed!\n");
        errcnt++;
        continue;
      }
      memcpy(&ret_sum, data_resp->data, sizeof(uint32_t));
#ifdef QMI_TEST_CHECKSUM
      if (ret_sum != sum)
      {
        printf("Error: send_raw_msg_sync_error: Checksum mismatch\n");
        printf("tx_csum (%d) != rx_csum(%d)\n", sum, ret_sum);
        errcnt++;
      }
#endif
    }
    clock_gettime(CLOCK_REALTIME,&time_info_end);
    qmi_test_clnt_common_calc_stats_timespec((test_length - errcnt),
        data_size_bytes, data_size_bytes, &time_info_start,
        &time_info_end, results);
  }
  else
  {
    signal(SIGALRM, test_completion_timer_cb_func);
    ret = set_timer_ms(&timer_test_completion, test_duration_ms);
    if(ret)
    {
      perror("setitimer");
      errcnt++;
      free(data_req);
      free(data_resp);
      free(pReqBuf);
      free(pRespBuf);
      return -errcnt;
    }

    /* Test ran for a fixed period of time*/
    clock_gettime(CLOCK_REALTIME,&time_info_start);
    while((0 == test_completed) && (0 == errcnt))
    {
      /*send raw message*/
      ret = qmi_client_send_raw_msg_sync(*clnt, QMI_TEST_DATA_REQ_V01, pReqBuf,
              iReqBufLen, pRespBuf, iRespBufLen, &iRespBufRecvLen, 0);
      if (ret != 0)
      {
        printf("Error: send_raw_msg_sync error: %d\n",ret);
        errcnt++;
        goto bypass;
      }

      /*Decode Raw Message*/
      rc = test_decode_raw_msg(*clnt,QMI_TEST_DATA_RESP_V01, data_resp,
              pRespBuf,iRespBufRecvLen);
      if (-1 == rc)
      {
        printf("Error: Message decode failed!\n");
        errcnt++;
        goto bypass;
      }
      memcpy(&ret_sum, data_resp->data, sizeof(uint32_t));
#ifdef QMI_TEST_CHECKSUM
      if (ret_sum != sum)
      {
        printf("Error: send_msg_sync_error: Checksum mismatch\n");
        printf("tx_csum (%d) != rx_csum(%d)\n", sum, ret_sum);
        errcnt++;
      }
#endif
bypass:
      num_loops++;
      if(verbose)
      {
        clock_gettime(CLOCK_REALTIME,&time_info_current);
        if(time_info_current.tv_sec > (time_info_last.tv_sec))
        {
          time_info_last.tv_sec = time_info_current.tv_sec;
          printf("QMI txns completed: %5d Num Errors:%d\n",
              (int)(num_loops - errcnt), (int)errcnt);
        }
      }

#ifdef FEATURE_QMI_TEST_TIMER_BASED_TEST
      if(num_loops >= test_length)
        test_completed = 1;
#endif
    }
    clock_gettime(CLOCK_REALTIME,&time_info_end);
    if(errcnt == 0)
    {
      qmi_test_clnt_common_calc_stats_timespec(num_loops, data_size_bytes,
        data_size_bytes, &time_info_start, &time_info_end, results);
      results->rc = -errcnt;
    }
    else
    {
      memset(results, 0, sizeof(qmi_test_clnt_results_type));
      results->rc = -errcnt;
    }
    ret = zero_timer(&timer_test_completion);
    if(ret)
    {
      perror("setitimer");
      errcnt++;
      free(data_req);
      free(data_resp);
      free(pReqBuf);
      free(pRespBuf);
      return -errcnt;
    }
  }
  free(data_req);
  free(data_resp);
  free(pReqBuf);
  free(pRespBuf);

  return(- errcnt );
}

/*===========================================================================
  FUNCTION  qmi_test_clnt_common_async_raw_data_test
===========================================================================*/
/*!
@brief
  Common async raw data test
  Runs a qmi test async raw data test for with parameters:
  test_length : max number of qmi transactions
  data_size_bytes: size of data block to send
  test_duration_ms: duration of test

@return
  0 if SUCCESS
  -1 if FAILURE
  returns statistics of test in  qmi_test_clnt_results_type

@note
  - Dependencies  qmi_test_clnt_results_type must be pre-allocated

  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_async_raw_data_test(qmi_client_type *clnt,
      qmi_txn_handle *txn, uint32_t test_length, uint32_t data_size_bytes,
      uint32_t test_duration_ms, uint32_t verbose,
      qmi_test_clnt_results_type *results)
{
  uint32_t sum = 0;
  uint32_t i, ret, errcnt = 0;
  struct timespec time_info_start;
  struct timespec time_info_end;
  struct timespec time_info_last;
  struct timespec time_info_current;
  uint32_t num_loops = 0;
  uint8_t byte_data = 0;

  /*data info*/
  test_data_req_msg_v01 *data_req;

  /*raw buffer info*/
  uint32_t    iRespBufLen = 0;
  uint32_t    iReqBufLen = 0;
  void      *pReqBuf  = NULL;
  void      *pRespBuf = NULL;
  int       rc;

  test_completed = 0; /*Reset test completed*/
  rx_err = 0; /*Reset the async error count*/
  pending_async = 0; /*Reset pending txns to zero*/

  /* Allocate C Msg Buffer */
  data_req = (test_data_req_msg_v01 *)calloc(1, sizeof(test_data_req_msg_v01));
  if(!data_req)
  {
    printf("Error: data_req memory alloc failure!\n");
    return -1;
  }

  /* Setup req data block & checksum to it*/
  for(i = 0; i < data_size_bytes; i++)
  {
    byte_data = (uint8_t)(rand() & 0xff);
    data_req->data[i + 4] = byte_data;
    sum = sum ^ byte_data;
  }
  memcpy(data_req->data, &sum, sizeof(uint32_t));
  data_req->data_len = (data_size_bytes + 4);

  /* Encode C Msg to raw format */
  rc = test_encode_raw_msg(*clnt, data_req, &pReqBuf, &iReqBufLen);
  if (-1 == rc)
  {
    printf("Error: data encode failed!\n");
    free(data_req);
    return -1;
  }

  /* Allocate memory for raw response buffer */
  rc = test_get_raw_buffer(&pRespBuf, &iRespBufLen, QMI_TEST_DATA_RESP_V01,
          QMI_IDL_RESPONSE);
  if (-1 == rc)
  {
    printf("Error: allocating resp buffer!\n");
    free(data_req);
    free(pReqBuf);
    return -1;
  }

  /* If the test is specified with a test_duration_ms
   Then execute the test for this duration, if not,
   loop for test_length */
  clock_gettime(CLOCK_REALTIME,&time_info_last);
  if((0 == test_duration_ms) && (0 == test_length))
  {
    printf("Error: test_duration && test_length zero!");
    free(data_req);
    free(pReqBuf);
    free(pRespBuf);
    return -1;
  }
  else if(0 == test_duration_ms)
  {
    clock_gettime(CLOCK_REALTIME,&time_info_start);
    for(i = 0;i < test_length; i++)
    {
      ret = qmi_client_send_raw_msg_async(*clnt, QMI_TEST_DATA_REQ_V01, pReqBuf,
              iReqBufLen, pRespBuf, iRespBufLen, test_raw_rx_cb, (void *)&sum,
              txn);
      if (ret != 0)
      {
        printf("Error: send_raw_msg_async error: %d\n",ret);
        errcnt++;
      }
      else
      {
        pthread_mutex_lock(&async_count_mutex);
        pending_async++;
        pthread_mutex_unlock(&async_count_mutex);
      }
    }
    pthread_mutex_lock(&async_count_mutex);
    while (pending_async)
      pthread_cond_wait(&async_count_cond, &async_count_mutex);
    pthread_mutex_unlock(&async_count_mutex);
    clock_gettime(CLOCK_REALTIME,&time_info_end);
    errcnt = errcnt + rx_err;
    qmi_test_clnt_common_calc_stats_timespec((test_length - errcnt),
        data_size_bytes, data_size_bytes, &time_info_start, &time_info_end,
        results);
  }
  else
  {
    signal(SIGALRM, test_completion_timer_cb_func);
    ret = set_timer_ms(&timer_test_completion, test_duration_ms);
    if(ret)
    {
      perror("setitimer");
      errcnt = errcnt + rx_err;
      errcnt++;
      free(data_req);
      free(pReqBuf);
      free(pRespBuf);
      return -errcnt;
    }

    /* Test ran for a fixed period of time*/
    clock_gettime(CLOCK_REALTIME,&time_info_start);
    while((0 == test_completed) && (0 == errcnt))
    {
      /*send raw message*/
      ret = qmi_client_send_raw_msg_async(*clnt, QMI_TEST_DATA_REQ_V01, pReqBuf,
              iReqBufLen, pRespBuf, iRespBufLen, test_raw_rx_cb, (void *)&sum,
              txn);
      if (ret != 0)
      {
        printf("Error: send_raw_msg_async error: %d\n",ret);
        errcnt++;
      }
      else
      {
        pthread_mutex_lock(&async_count_mutex);
        pending_async++;
        pthread_mutex_unlock(&async_count_mutex);
      }
      num_loops++;

      if(verbose)
      {
        clock_gettime(CLOCK_REALTIME,&time_info_current);
        if(time_info_current.tv_sec > (time_info_last.tv_sec))
        {
          time_info_last.tv_sec = time_info_current.tv_sec;
          printf("QMI txns completed: %5d Num Errors:%d\n",
              (int)(num_loops - (errcnt + pending_async + rx_err)),
              (int)(errcnt + rx_err));
        }
      }

#ifdef FEATURE_QMI_TEST_TIMER_BASED_TEST
      if(num_loops >= test_length)
        test_completed = 1;
#endif
    }
    pthread_mutex_lock(&async_count_mutex);
    while (pending_async)
      pthread_cond_wait(&async_count_cond, &async_count_mutex);
    pthread_mutex_unlock(&async_count_mutex);

    clock_gettime(CLOCK_REALTIME, &time_info_end);
    errcnt = errcnt + rx_err;

    if(errcnt == 0)
    {
      qmi_test_clnt_common_calc_stats_timespec(num_loops, data_size_bytes,
          data_size_bytes, &time_info_start, &time_info_end, results);
      results->rc = -errcnt;
    }
    else
    {
      memset(results, 0, sizeof(qmi_test_clnt_results_type));
      results->rc = -errcnt;
    }
    ret = zero_timer(&timer_test_completion);
    if(ret)
    {
      perror("setitimer");
      errcnt++;
      free(data_req);
      free(pReqBuf);
      free(pRespBuf);
      return -errcnt;
    }
  }
  free(data_req);
  free(pReqBuf);
  free(pRespBuf);

  return -errcnt;
}

/*===========================================================================
  FUNCTION  qmi_test_clnt_common_delete_async_txn_test
===========================================================================*/
/*!
@brief
  Common delete async txn test
  Runs a qmi test async raw data test for with parameters:
  test_length : max number of qmi transactions
  data_size_bytes: size of data block to send
  test_duration_ms: duration of test
  and deletes the txn before it completes

@return
  0 if SUCCESS
  -1 if FAILURE

@note
  - Side Effects
*/
/*=========================================================================*/
int qmi_test_clnt_common_delete_async_txn_test(qmi_client_type *clnt,
        qmi_txn_handle *txn, uint32_t test_length, uint32_t data_size_bytes,
        int time_out_ms)
{
  uint32_t sum = 0;
  uint32_t i, ret, errcnt = 0;
  uint32_t num_loops = 0;
  uint8_t byte_data = 0;

  /*data info*/
  test_data_req_msg_v01 *data_req;

  /*raw buffer info*/
  uint32_t iRespBufLen = 0;
  uint32_t iReqBufLen = 0;
  void *pReqBuf  = NULL;
  void *pRespBuf = NULL;
  int rc;

  rx_err = 0; /*Reset the async error count*/
  pending_async = 0; /*Reset pending txns to zero*/

  /* Allocate C Msg Buffer */
  data_req = (test_data_req_msg_v01 *)calloc(1, sizeof(test_data_req_msg_v01));
  if(!data_req)
  {
    printf("Error: data_req memory alloc failure!\n");
    return -1;
  }

  /* Setup req data block & checksum to it*/
  for(i = 0; i < data_size_bytes; i++)
  {
    byte_data = (uint8_t)(rand() & 0xff);
    data_req->data[i + 4] = byte_data;
    sum = sum ^ byte_data;
  }
  memcpy(data_req->data, &sum, sizeof(uint32_t));
  data_req->data_len = (data_size_bytes + 4);

  /* Encode C Msg to raw format */
  rc = test_encode_raw_msg(*clnt, data_req, &pReqBuf, &iReqBufLen);
  if (-1 == rc)
  {
    printf("Error: data encode failed!\n");
    free(data_req);
    return -1;
  }

  /* Allocate memory for raw response buffer */
  rc = test_get_raw_buffer(&pRespBuf, &iRespBufLen, QMI_TEST_DATA_RESP_V01, QMI_IDL_RESPONSE);
  if (-1 == rc)
  {
      printf("Error: allocating resp buffer!\n");
      free(data_req);
      free(pReqBuf);
      return -1;
  }

  /* If the test is specified with a test_duration_ms
   Then execute the test for this duration, if not,
   loop for test_length */

  if(0 < test_length)
  {
    /*Set TIMEOUT callback. This cb will get called when timer expires*/
    signal(SIGALRM, rx_timeout_cb_func);
    ret = set_timer_ms(&rx_timeout, time_out_ms);
    if(ret)
    {
      perror("TimeOutTimer");
      printf("Error: set_timer returned error\n");
      errcnt = errcnt + rx_err;
      errcnt++;
      free(data_req);
      free(pReqBuf);
      free(pRespBuf);
      return -errcnt;
    }
    for(i = 0;i < test_length; i++)
    {
      ret = qmi_client_send_raw_msg_async(*clnt, QMI_TEST_DATA_REQ_V01, pReqBuf,
              iReqBufLen, pRespBuf, iRespBufLen, test_raw_rx_cb, (void *)&sum,
              txn);
      if (ret != 0)
      {
        printf("Error: send_raw_msg_async error: %d\n",ret);
        errcnt++;
      }
      else
      {
        pending_async++;
      }
      /* Delete the async txn */
      ret = qmi_client_delete_async_txn(*clnt,*txn);
      if (QMI_NO_ERR != ret)
      {
        printf("Error: Failed to cancel/delete async msg %d\n",ret);
        free(data_req);
        free(pReqBuf);
        free(pRespBuf);
        return -1;
      }
      else
      {
        printf("Requested for delete async txn:%d\n", i);
      }
    }
    while (pending_async)
      pthread_cond_wait(&async_count_cond, &async_count_mutex);

    errcnt = errcnt + rx_err;
    ret = zero_timer(&timer_test_completion);
    if(ret)
    {
      perror("setitimer");
      printf("Error: zero_timer returned error\n");
      errcnt++;
      free(data_req);
      free(pReqBuf);
      free(pRespBuf);
      return -errcnt;
    }
  }
  else
  {
    printf("Error: Invalid test length passed!\n");
  }
  printf("async txns tried: %d\n", test_length);
  free(data_req);
  free(pReqBuf);
  free(pRespBuf);
  return -errcnt;
}

/*===========================================================================
  FUNCTION  qmi_test_clnt_common_async_invalid_msg_test
===========================================================================*/
/*!
@brief
  Common async invalid message test
  Runs a qmi test async data test for with parameters:
  test_length : max number of qmi transactions
  data_size_bytes: size of data block to send
  test_duration_ms: duration of test
  and invalid message type is passsed.

@return
  0 for pass, -1 for fail

@note
  - None
*/
/*=========================================================================*/
int qmi_test_clnt_common_async_invalid_msg_test(qmi_client_type *clnt,
        qmi_txn_handle *txn, uint32_t test_length, uint32_t data_size_bytes,
        int time_out_ms)
{
  uint32_t sum = 0;
  uint32_t i, ret;
  uint8_t byte_data = 0;

  /*C structure data*/
  test_data_req_msg_v01 *data_req;
  test_data_resp_msg_v01 *data_resp;

  pending_async = 0;
  rx_err = 0;

  /*Allocate buffers*/
  data_req = (test_data_req_msg_v01 *)calloc(1, sizeof(test_data_req_msg_v01));
  if(!data_req)
  {
    printf("Error: data_req buffer alloc failed!\n");
    return -1;
  }
  data_resp = (test_data_resp_msg_v01 *)calloc(1, sizeof(test_data_resp_msg_v01));
  if(!data_resp)
  {
    printf("Error: data_resp buffer alloc failed!\n");
    free(data_req);
    return -1;
  }

  /* Setup req data block & checksum to it*/
  for(i = 0; i < data_size_bytes; i++)
  {
    byte_data = (uint8_t)(rand() & 0xff);
    data_req->data[i + 4] = byte_data;
    sum = sum ^ byte_data;
  }
  memcpy(data_req->data, &sum, sizeof(uint32_t));
  data_req->data_len = (data_size_bytes + 4);

  if(0 < test_length)
  {
    /*Set TIMEOUT callback. This cb will get called when timer expires*/
    signal(SIGALRM, rx_timeout_cb_func);
    ret = set_timer_ms(&rx_timeout, time_out_ms);
    if(ret)
    {
      perror("TimeOutTimer");
      printf("Error: set_timer returned error\n");
      free(data_req);
      free(data_resp);
      return -1;
    }
    /*Sending async message with invalid message ID*/
    for(i = 0;i < test_length; i++)
    {
      ret = qmi_client_send_msg_async(*clnt, TEST_INVALID_MSG_ID, data_req,
              sizeof(test_data_req_msg_v01), data_resp,
              sizeof(test_data_resp_msg_v01), test_rx_cb, (void *)&sum, txn);
      if (QMI_NO_ERR == ret)
      {
        printf("Error: async message sent with invalid message!\n");
        printf("       Verify the resp buffer");
      }
      else
      {
        printf("Good: Sending async invalid message failed.\n");
      }
      /*increasing pending_async just to wait for any response*/
      pending_async++;
    }

    /*Even though sending async message failed. Lets wait for timeout to kick in
    If at all any async message is passed, we should see pending async message
    decrease by certain number. Which is considered as test fail*/
    printf("Checking if any response is received!\n");
    while (pending_async)
      pthread_cond_wait(&async_count_cond, &async_count_mutex);

    /*Zero the timer set*/
    ret = zero_timer(&timer_test_completion);
        if(ret)
        {
          perror("setitimer");
          printf("Error: zero_timer returned error\n");
          free(data_req);
          free(data_resp);
          return -1;
        }
  }
  else
  {
    printf("Error: Invalid test length passed!\n");
  }
  free(data_req);
  free(data_resp);

  return 0;
}

/*===========================================================================
  FUNCTION  qmi_test_clnt_common_calc_stats_timespec
===========================================================================*/
/*!
@brief
  Calculate Stats of test using timespec of start and end times

@return
  returns statistics of test in  qmi_test_clnt_results_type

@note
  - Dependencies  qmi_test_clnt_results_type must be pre-allocated

  - Side Effects
*/
/*=========================================================================*/
void qmi_test_clnt_common_calc_stats_timespec(
  uint32_t num_loops,
  uint32_t data_size_bytes,
  uint32_t ret_data_size_bytes,
  struct timespec *start_time_info,
  struct timespec *end_time_info,
  qmi_test_clnt_results_type *results)
{
  double start_time_us;
  double end_time_us;
  double delta_us;
  double test_duration_ms;
  uint32_t num_bytes;

  num_bytes = (data_size_bytes + ret_data_size_bytes + 8) * num_loops;
  end_time_us = (double)end_time_info->tv_nsec / (double)1000 + (double)end_time_info->tv_sec * (double)1000000;
  start_time_us = (double)start_time_info->tv_nsec / (double)1000 + (double)start_time_info->tv_sec * (double)1000000;

  delta_us = end_time_us - start_time_us;
  test_duration_ms = (delta_us)/1000;

  results->data_transfer_rate_kbps = (double)(num_bytes ) / (double)test_duration_ms;
  results->num_bytes_transfered = (data_size_bytes + ret_data_size_bytes) * num_loops;
  results->num_fwd_bytes_per_txn = data_size_bytes;
  results->num_loops_completed = num_loops;
  results->num_ret_bytes_per_txn = ret_data_size_bytes;
  results->latency_ms = (double)test_duration_ms/(double)num_loops;
  results->test_duration_ms = test_duration_ms;
}
