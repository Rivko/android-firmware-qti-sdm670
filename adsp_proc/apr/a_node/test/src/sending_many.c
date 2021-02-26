/*==============================================================================
  @file sending_many.c
  @brief Testing many back to back messages to stress test the system

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.(QTI)
  All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/apr.common/2.0/a_node/test/src/sending_many.c#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  03/08/16    Unni     Initial Version
==============================================================================*/

#include "a_node.h"
/* a_node interface - API used to invoke a_node functionalities */
#include "test_msg.h"
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

/* Test sequence control event
   Only needed for test synchronization */
static apr_event_t test_seq_event;

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
static int32_t cmd_add_handler(void *in_msg_ptr,
                               uint32_t in_msg_size,
                               uint32_t in_msg_opcode,
                               void *out_msg_ptr,
                               uint32_t out_msg_size,
                               uint32_t *out_msg_actual_size_ptr,
                               uint32_t *out_msg_opcode_ptr)
{
  cmd_add_t *cmd_add_ptr = NULL;
  rsp_calc_t *rsp_calc_ptr = NULL;

  if(in_msg_size < sizeof(cmd_add_t) ||
     out_msg_size < sizeof(rsp_calc_t)  )
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
  if(CMD_ADD != in_msg_opcode)
  {
    /* Unexpected Opcode */
    return 3;
  }

  cmd_add_ptr = (cmd_add_t*)in_msg_ptr;
  rsp_calc_ptr = (rsp_calc_t*)out_msg_ptr;
  *out_msg_actual_size_ptr = sizeof(rsp_calc_t);
  *out_msg_opcode_ptr = RSP_CALC;

  rsp_calc_ptr->result = cmd_add_ptr->a + cmd_add_ptr->b;

  return 0;
}

/*
  Sample command handler for OPCODE CMD_SUBSTRACT (0x11)

  Signature of function must comply with a_node_command_handler_t
*/
static int32_t cmd_diff_handler(void *in_msg_ptr,
                                uint32_t in_msg_size,
                                uint32_t in_msg_opcode,
                                void *out_msg_ptr,
                                uint32_t out_msg_size,
                                uint32_t *out_msg_actual_size_ptr,
                                uint32_t *out_msg_opcode_ptr)
{
  cmd_diff_t *cmd_diff_ptr = NULL;
  rsp_calc_t *rsp_calc_ptr = NULL;

  if(in_msg_size < sizeof(cmd_add_t) ||
     out_msg_size < sizeof(rsp_calc_t)  )
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
  if(CMD_DIFF != in_msg_opcode)
  {
    /* Unexpected Opcode */
    return 3;
  }

  cmd_diff_ptr = (cmd_diff_t*)in_msg_ptr;
  rsp_calc_ptr = (rsp_calc_t*)out_msg_ptr;
  *out_msg_actual_size_ptr = sizeof(rsp_calc_t);
  *out_msg_opcode_ptr = RSP_CALC;

  rsp_calc_ptr->result = cmd_diff_ptr->a - cmd_diff_ptr->b;

  return 0;
}

/*
  Sample response handler for OPCODE RSP_CALC (0x20), honoring Token

  Signature of function must comply with a_node_response_handler_t
*/
static int32_t rsp_calc_handler(void *msg_ptr,
                                uint32_t msg_size,
                                uint32_t msg_token,
                                uint32_t msg_opcode)
{
  rsp_calc_t *rsp_calc_ptr = NULL;

  if(msg_size < sizeof(rsp_calc_t))
  {
    /* Insufficient space */
    return 1;
  }

  if(NULL == msg_ptr)
  {
    /* NULL access */
    return 2;
  }

  if(RSP_CALC != msg_opcode)
  {
    /* Unexpected response */
    return 3;
  }


  rsp_calc_ptr = (rsp_calc_t*)msg_ptr;

  /* MSG Token can be used to distinguish the responses */
  /* In this example, we have mapped 777 for SUM and 888 for DIFF. Users are
     free to overload msg token if they have any custom implementation for their
     responses (like signaling a thread using token as signal variable, or
     calling a different subrouting using token as function pointer, etc)
     and not restricted to using just switch cases */
  switch(msg_token)
  {
    case 777:
      APR_DBG_2(APR_DBG_HIGH, "Sum:%d, token:%d",
                rsp_calc_ptr->result, msg_token);
      break;
    case 888:
      APR_DBG_2(APR_DBG_HIGH, "Diff:%d, token:%d",
                rsp_calc_ptr->result, msg_token);
      break;
    default:
      APR_DBG_2(APR_DBG_HIGH, "Result:%d, token:%d",
                rsp_calc_ptr->result, msg_token);
      break;
  }

  apr_event_signal(test_seq_event);
  return 0;
}

/*
  Sample scenario 1:
   Step1: Client(1) Initialize with default response handler
   Step2: Server(2) Initialize with default command handler
   Step3: Client(1) registers response handler for message type RSP_CALC
   Step4: Server(2) registers command handler for message type CMD_ADD
   Step5: Client(1) sends message CMD_ADD
 */
static void sample_scenario_1()
{
  a_node_t a_node_1, a_node_2;
  cmd_add_t cmd_add;
  a_node_callback_func_t callback_func;
  int32_t iter = 500;

  apr_event_create(&test_seq_event);

  /* Step1: Client(1) Initialize with default response handler */
  a_node_init(&a_node_1, 1, 123, 4096, 4096, NULL, rsp_basic_handler);

  /* Step2: Server(2) Initialize with default command handler */
  a_node_init(&a_node_2, 2, 123, 4096, 4096, cmd_err_handler, NULL);

  /* Step3: Client(1) registers response handler for message type RSP_CALC */
  callback_func.response_handler = rsp_calc_handler;
  a_node_register(a_node_1, RSP_CALC, callback_func);

  /* Step4: Server(2) registers command handler for message type CMD_ADD */
  callback_func.command_handler = cmd_add_handler;
  a_node_register(a_node_2, CMD_ADD, callback_func);

  while(iter>0)
  {
    /* Step5: Client(1) sends message CMD */
    cmd_add.a = 123*iter;
    cmd_add.b = 32100/iter;
    a_node_send(a_node_1, 777, CMD_ADD,
                &cmd_add,sizeof(cmd_add), 2);

    iter--;

    /* Handle upto max queue depth */
    apr_event_wait(test_seq_event);
  }

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
  Step4: Client(1) registers response handler for message type RSP_CALC
  Step5: Client(2) registers response handler for message type RSP_CALC
  Step6: Server(3) registers command handler for message type CMD_ADD
  Step7: Server(3) registers command handler for message type CMD_DIFF
  Step8: Client(1) sends message CMD ADD
  Step9: Client(2) sends message CMD DIFF
*/
static void sample_scenario_2()
{
  a_node_t a_node_1, a_node_2, a_node_3;
  cmd_add_t cmd_add;
  cmd_diff_t cmd_diff;
  a_node_callback_func_t callback_func;
  int32_t iter = 500;

  apr_event_create(&test_seq_event);

  /* Step1: Client(1) Initialize with default response handler */
  a_node_init(&a_node_1, 1, 123, 4096, 4096, NULL, rsp_basic_handler);

  /* Step2: Client(2) Initialize with default response handler */
  a_node_init(&a_node_2, 2, 123, 4096, 4096, NULL, rsp_basic_handler);

  /* Step3: Server(3) Initialize with default command handler */
  a_node_init(&a_node_3, 3, 123, 4096, 4096, cmd_err_handler, NULL);

  /* Step4: Client(1) registers response handler for message type RSP_CALC */
  callback_func.response_handler = rsp_calc_handler;
  a_node_register(a_node_1, RSP_CALC, callback_func);

  /* Step5: Client(2) registers response handler for message type RSP_CALC */
  callback_func.response_handler = rsp_calc_handler;
  a_node_register(a_node_2, RSP_CALC, callback_func);

  /* Step6: Server(3) registers command handler for message type CMD_ADD */
  callback_func.command_handler = cmd_add_handler;
  a_node_register(a_node_3, CMD_ADD, callback_func);

  /* Step7: Server(3) registers command handler for message type CMD_DIFF */
  callback_func.command_handler = cmd_diff_handler;
  a_node_register(a_node_3, CMD_DIFF, callback_func);

  while(iter>0)
  {
    /* Step8: Client(1) sends message CMD ADD */
    cmd_add.a = 123000/iter;
    cmd_add.b = 321*iter;
    a_node_send(a_node_1, 777, CMD_ADD,
                &cmd_add,sizeof(cmd_add), 3);

    apr_event_wait(test_seq_event);

    /* Step9: Client(2) sends message CMD DIFF */
    cmd_diff.a = 212*iter;
    cmd_diff.b = 101000/iter;
    a_node_send(a_node_2, 888, CMD_DIFF,
                &cmd_diff,sizeof(cmd_diff), 3);

    iter--;

    /* Handle upto max queue depth */
    apr_event_wait(test_seq_event);
  }



  /* De-initialize 3 instance of a_node */
  a_node_deinit(a_node_1);
  a_node_deinit(a_node_2);
  a_node_deinit(a_node_3);
  apr_event_destroy(test_seq_event);
}


/* Main Test entry */
void a_node_send_many()
{
  APR_DBG_0(APR_DBG_HIGH, "Test: a_node_send_many :Started");
  sample_scenario_1();
  sample_scenario_2();
  APR_DBG_0(APR_DBG_HIGH, "Test: a_node_send_many :Ended");
}

