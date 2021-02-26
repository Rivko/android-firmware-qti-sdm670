/*==============================================================================
  @file sending_big.c
  @brief Testing Big messages back to back messages to stress test the system

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.(QTI)
  All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/apr.common/2.0/a_node/test/src/sending_big.c#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  03/08/16    Unni     Initial Version
==============================================================================*/

#include "a_node.h"
/* User Message interface - Defined by USER */
#include "aprv2_msg_if.h"
/* APR basic Message interface - For error handling */

/* Debug support */
#include "apr_dbg_msg.h"
/* APR debug logging - Print debug logs on QXDM using APR infrastructure */
#include "apr_event.h"
/* APR events - sequentialize test progress */
#include "apr_errcodes.h"
/* APR error code - Assign meaning for errors */
#include "DALSys.h"
/* For big malloc */

/* Test sequence control event
   Only needed for test synchronization */
static apr_event_t test_seq_event;



#define BIG_MSG 0xB1C

typedef struct 
{
  uint32_t size;
  char data[];
}big_msg_t;


/*
  Sample command handler for incoming message with unknown Opcode

  Signature of function must comply with a_node_command_handler_t
*/
static int32_t cmd_err_handler(void *in_msg_ptr,
                               uint32_t in_msg_size,
                               uint32_t in_msg_opcode,
                               void *out_msg_ptr,
                               uint32_t out_msg_size,
                               uint32_t *out_msg_actual_size_ptr,
                               uint32_t *out_msg_opcode_ptr)
{
  aprv2_ibasic_rsp_result_t *rsp_ibasic_ptr = NULL;

  if(NULL == in_msg_ptr              ||
     NULL == out_msg_ptr             ||
     NULL == out_msg_actual_size_ptr ||
     NULL == out_msg_opcode_ptr        )
  {
    /* NULL access */
    return APR_EBADPARAM;
  }

  if(out_msg_size < sizeof(aprv2_ibasic_rsp_result_t))
  {
    /* Insufficient size */
    return APR_ENOMEMORY;
  }

  /* Using IBASIC response format */
  rsp_ibasic_ptr = (aprv2_ibasic_rsp_result_t*)out_msg_ptr;
  *out_msg_opcode_ptr = APRV2_IBASIC_RSP_RESULT;
  *out_msg_actual_size_ptr = sizeof(aprv2_ibasic_rsp_result_t);

  rsp_ibasic_ptr->opcode = in_msg_opcode;
  rsp_ibasic_ptr->status = APR_EUNSUPPORTED;

  return APR_EOK;
}

/*
  Sample response handler for
  APRV2_IBASIC_RSP_RESULT (0x000110E8)

  Signature of function must comply with a_node_response_handler_t
*/
static int32_t rsp_basic_handler(void *msg_ptr,
                                 uint32_t msg_size,
                                 uint32_t msg_token,
                                 uint32_t msg_opcode)
{
  aprv2_ibasic_rsp_result_t *rsp_ibasic_ptr = NULL;

  if(NULL == msg_ptr)
  {
    /* NULL access */
    APR_DBG_0(APR_DBG_HIGH, "Received NULL response Message");
    return APR_EBADPARAM;
  }

  if(APRV2_IBASIC_RSP_RESULT != msg_opcode)
  {
    /* Unsupported response opcode */
    APR_DBG_1(APR_DBG_HIGH, "Unsupported Response [%x]", msg_opcode);
    return APR_EUNSUPPORTED;
  }

  if(msg_size < sizeof(aprv2_ibasic_rsp_result_t))
  {
    /* Insufficient size */
    APR_DBG_2(APR_DBG_HIGH, "msg size[%d] < IBASIC response size[%d]",
              msg_size,
              sizeof(aprv2_ibasic_rsp_result_t));
    return APR_ENOMEMORY;
  }

  /* Extract IBASIC response */
  rsp_ibasic_ptr = (aprv2_ibasic_rsp_result_t*)msg_ptr;
  APR_DBG_3(APR_DBG_HIGH, "Err[0x%x] processing command[0x%x], token:%d",
            rsp_ibasic_ptr->status, rsp_ibasic_ptr->opcode, msg_token);

  apr_event_signal(test_seq_event);
  return APR_EOK;
}

/*
  Sample command handler for OPCODE CMD_ADD (0x10)

  Signature of function must comply with a_node_command_handler_t
*/
static int32_t cmd_big_handler(void *in_msg_ptr,
                               uint32_t in_msg_size,
                               uint32_t in_msg_opcode,
                               void *out_msg_ptr,
                               uint32_t out_msg_size,
                               uint32_t *out_msg_actual_size_ptr,
                               uint32_t *out_msg_opcode_ptr)
{
  big_msg_t *in_big_msg_ptr = NULL, *out_big_msg_ptr = NULL;
  uint32_t iter = 0;
  uint32_t mask = 0;
  uint32_t sum = 0;
  
  if(in_msg_size < sizeof(big_msg_t) ||
     out_msg_size < sizeof(big_msg_t)  )
  {
    /* Insufficient space */
    return 1;
  }  
  
  if(NULL == in_msg_ptr              ||
     NULL == out_msg_ptr             ||
     NULL == out_msg_actual_size_ptr ||
     NULL == out_msg_opcode_ptr        )
  {
    /* NULL access */
    return 2;
  }
  
  if(BIG_MSG != in_msg_opcode)
  {
    /* Unexpected Opcode */
    return 3;
  }

  if(out_msg_size < in_msg_size)
  {
    /* Unexpected output msg size */
    return 4;
  }

  in_big_msg_ptr = (big_msg_t*)in_msg_ptr;
  out_big_msg_ptr = (big_msg_t*)out_msg_ptr;
  while(iter < in_big_msg_ptr->size)
  {
    out_big_msg_ptr->data[iter] = ~(in_big_msg_ptr->data[iter]);
    sum += in_big_msg_ptr->data[iter];
    mask |= in_big_msg_ptr->data[iter];
    iter++;
  }

  APR_DBG_3(APR_DBG_HIGH, "CMD:Got size[%d] mask[0x%x] sum[%d]",
            iter, mask, sum);  
  
  out_big_msg_ptr->size = iter;
  *out_msg_actual_size_ptr = iter + sizeof(uint32_t);
  *out_msg_opcode_ptr = BIG_MSG;

  return 0;
}

/*
  Sample response handler for OPCODE RSP_CALC (0x20), honoring Token

  Signature of function must comply with a_node_response_handler_t
*/
static int32_t rsp_big_handler(void *msg_ptr,
                               uint32_t msg_size,
                               uint32_t msg_token,
                               uint32_t msg_opcode)
{
  big_msg_t *big_msg_ptr = NULL;
  uint32_t iter = 0, sum = 0, mask = 0;

  if(msg_size < sizeof(big_msg_t))
  {
    /* Insufficient space */
    return 1;
  }

  if(NULL == msg_ptr)
  {
    /* NULL access */
    return 2;
  }

  if(BIG_MSG != msg_opcode)
  {
    /* Unexpected response */
    return 3;
  }

  big_msg_ptr = (big_msg_t*)msg_ptr;

  while(iter < big_msg_ptr->size)
  {
    sum += big_msg_ptr->data[iter];
    mask |= big_msg_ptr->data[iter];
    iter++;
  }

  APR_DBG_3(APR_DBG_HIGH, "RSP:Got size[%d] mask[0x%x] sum[%d]",
            iter, mask, sum);

  apr_event_signal(test_seq_event);
  return 0;
}

/*
  Sample scenario 1:
   Step1: Client(1) Initialize with default response handler
   Step2: Server(2) Initialize with default command handler
   Step3: Client(1) registers response handler for message type BIG_MSG
   Step4: Server(2) registers command handler for message type BIG_MSG
   Step5: Client(1) sends message BIG_MSG
 */
static void sample_scenario_1()
{
  a_node_t a_node_1, a_node_2;
  a_node_callback_func_t callback_func;
  int32_t iter = 0;
  big_msg_t *big_msg_ptr = NULL;
  const uint32_t big_msg_size = 4096 - 32/* APR header */;

  apr_event_create(&test_seq_event);

  /* Step1: Client(1) Initialize with default response handler */
  a_node_init(&a_node_1, 1, 123, 4096, 4096, NULL, rsp_basic_handler);

  /* Step2: Server(2) Initialize with default command handler */
  a_node_init(&a_node_2, 2, 123, 4096, 4096, cmd_err_handler, NULL);

  /* Step3: Client(1) registers response handler for message type BIG_MSG */
  callback_func.response_handler = rsp_big_handler;
  a_node_register(a_node_1, BIG_MSG, callback_func);

  /* Step4: Server(2) registers command handler for message type BIG_MSG */
  callback_func.command_handler = cmd_big_handler;
  a_node_register(a_node_2, BIG_MSG, callback_func);

  /* Populate big data */
  if(DALSYS_Malloc(big_msg_size, (void **)&big_msg_ptr) == DAL_SUCCESS)
  {
    big_msg_ptr->size = big_msg_size - sizeof(uint32_t);
    while(iter<big_msg_ptr->size)
    {
      big_msg_ptr->data[iter++] = 0x0F;
    }
  }

  iter = 20;
  while(iter--)
  {
    /* Step5: Client(1) sends message CMD */
    a_node_send(a_node_1, 777, BIG_MSG,
                big_msg_ptr, big_msg_size, 2);

    apr_event_wait(test_seq_event);
  }

  DALSYS_Free(big_msg_ptr);

  /* De-initialize 2 instance of a_node */
  a_node_deinit(a_node_1);
  a_node_deinit(a_node_2);

  apr_event_destroy(test_seq_event);
}

/*
  Sample scenario 2:
  Step1: Client(1) Initialize with default response handler
  Step2: Client(2) Initialize with default response handler
  Step3: Server(3) Initialize with default command handler
  Step4: Client(1) registers response handler for message type BIG_MSG
  Step5: Client(2) registers response handler for message type BIG_MSG
  Step6: Server(3) registers command handler for message type BIG_MSG
  Step7: Client(1) sends message BIG_MSG
  Step8: Client(2) sends message BIG_MSG
*/
static void sample_scenario_2()
{
  a_node_t a_node_1, a_node_2, a_node_3;
  a_node_callback_func_t callback_func;
  int32_t iter = 0;
  big_msg_t *big_msg_ptr = NULL;
  const uint32_t big_msg_size = 4096 - 32/* APR header */;

  apr_event_create(&test_seq_event);

  /* Step1: Client(1) Initialize with default response handler */
  a_node_init(&a_node_1, 1, 123, 4096, 4096, NULL, rsp_basic_handler);

  /* Step2: Client(2) Initialize with default response handler */
  a_node_init(&a_node_2, 2, 123, 4096, 4096, NULL, rsp_basic_handler);

  /* Step3: Server(3) Initialize with default command handler */
  a_node_init(&a_node_3, 3, 123, 4096, 4096, cmd_err_handler, NULL);

  /* Step4: Client(1) registers response handler for message type BIG_MSG */
  callback_func.response_handler = rsp_big_handler;
  a_node_register(a_node_1, BIG_MSG, callback_func);

  /* Step5: Client(2) registers response handler for message type BIG_MSG */
  callback_func.response_handler = rsp_big_handler;
  a_node_register(a_node_2, BIG_MSG, callback_func);

  /* Step6: Server(3) registers command handler for message type BIG_MSG */
  callback_func.command_handler = cmd_big_handler;
  a_node_register(a_node_3, BIG_MSG, callback_func);


  /* Populate big data */
  if(DALSYS_Malloc(big_msg_size, (void **)&big_msg_ptr) == DAL_SUCCESS)
  {
    big_msg_ptr->size = big_msg_size - sizeof(uint32_t);
    while(iter<big_msg_ptr->size)
    {
      big_msg_ptr->data[iter++] = 0xFF;
    }
  }

  iter = 20;
  while(iter--)
  {
    /* Step7: Client(1) sends message CMD */
    a_node_send(a_node_1, 777, BIG_MSG,
                big_msg_ptr, big_msg_size, 3);

    apr_event_wait(test_seq_event);

    /* Step8: Client(2) sends message CMD */
    a_node_send(a_node_2, 777, BIG_MSG,
                big_msg_ptr, big_msg_size, 3);

    apr_event_wait(test_seq_event);
  }

  DALSYS_Free(big_msg_ptr);

  /* De-initialize 3 instance of a_node */
  a_node_deinit(a_node_1);
  a_node_deinit(a_node_2);
  a_node_deinit(a_node_3);
  apr_event_destroy(test_seq_event);
}


/* Main Test entry */
void a_node_send_big()
{
  APR_DBG_0(APR_DBG_HIGH, "Test: a_node_send_many :Started");
  sample_scenario_1();
  sample_scenario_2();
  APR_DBG_0(APR_DBG_HIGH, "Test: a_node_send_many :Ended");
}

