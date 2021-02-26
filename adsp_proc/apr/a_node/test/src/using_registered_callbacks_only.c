/*==============================================================================
  @file using_default_callbacks_only.c
  @brief Sample implementation of a_node user that uses only callbacks that
         are registered via a_node_register() API. User doesn't assign any
         default command/response callback during initialization. User ensure
         that all needed response handlers are registered before sending a
         message (even basic response handler to handle erroneous cases).

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.(QTI)
  All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/apr.common/2.0/a_node/test/src/using_registered_callbacks_only.c#1 $

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

/* Test sequence control event
   Only needed for test synchronization */
static apr_event_t test_seq_event;

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
  cmd_diff_t *cmd_diff_ptr = (cmd_diff_t*)in_msg_ptr;
  rsp_calc_t *rsp_calc_ptr = (rsp_calc_t*)out_msg_ptr;

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
  Sample response handler for OPCODE RSP_CALC (0x20)

  Signature of function must comply with a_node_response_handler_t
*/
static int32_t rsp_add_handler(void *msg_ptr,
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
  APR_DBG_2(APR_DBG_HIGH, "Sum:%d, token:%d",
            rsp_calc_ptr->result, msg_token);

  apr_event_signal(test_seq_event);
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
   Step1: Client(1) registers response handler for message type RSP_CALC
   Step2: Server(2) registers command handler for message type CMD_ADD
   Step3: Client(1) sends message CMD_ADD
 */
static void sample_scenario_1()
{
  a_node_t a_node_1, a_node_2;
  cmd_add_t cmd_add;
  a_node_callback_func_t callback_func;
  apr_event_create(&test_seq_event);

  /* Initialize 2 instance of a_node */
  a_node_init(&a_node_1, 1, 123, 4096, 4096, NULL, NULL);
  a_node_init(&a_node_2, 2, 123, 4096, 4096, NULL, NULL);

  /* Step1: Register response handler at Client(1) */
  callback_func.response_handler = rsp_add_handler;
  a_node_register(a_node_1, RSP_CALC, callback_func);

  /* Step2: Register command handler at Server(2) */
  callback_func.command_handler = cmd_add_handler;
  a_node_register(a_node_2, CMD_ADD, callback_func);

  /* Step3: Client(1) sends message CMD */
  cmd_add.a = 123;
  cmd_add.b = 321;
  a_node_send(a_node_1, 777, CMD_ADD,
              &cmd_add,sizeof(cmd_add), 2);

  apr_event_wait(test_seq_event);

  /* De-initialize 2 instance of a_node */
  a_node_deinit(a_node_1);
  a_node_deinit(a_node_2);

  apr_event_destroy(test_seq_event);
}

/*
  Sample scenario 2:
   Step1: Client(1) registers response handler for message type RSP_CALC
   Step2: Client(2) registers response handler for message type RSP_CALC
   Step3: Server(3) registers command handler for message type CMD_ADD
   Step4: Server(3) registers command handler for message type CMD_DIFF
   Step5: Client(1) sends message CMD_ADD
   Step6: Client(2) sends message CMD_DIFF
 */
static void sample_scenario_2()
{
  a_node_t a_node_1, a_node_2, a_node_3;
  cmd_add_t cmd_add;
  cmd_diff_t cmd_diff;
  a_node_callback_func_t callback_func;
  apr_event_create(&test_seq_event);

  /* Initialize 2 instance of a_node */
  a_node_init(&a_node_1, 1, 123, 4096, 4096, NULL, NULL);
  a_node_init(&a_node_2, 2, 123, 4096, 4096, NULL, NULL);
  a_node_init(&a_node_3, 3, 123, 4096, 4096, NULL, NULL);

  /* Step1: Register response handler at Client(1) */
  callback_func.response_handler = rsp_calc_handler;
  a_node_register(a_node_1, RSP_CALC, callback_func);

  /* Step2: Register response handler at Client(2) */
  callback_func.response_handler = rsp_calc_handler;
  a_node_register(a_node_2, RSP_CALC, callback_func);

  /* Step3: Register command handler CMD_ADD at Server(3) */
  callback_func.command_handler = cmd_add_handler;
  a_node_register(a_node_3, CMD_ADD, callback_func);

  /* Step4: Register command handler CMD_DIFF at Server(3) */
  callback_func.command_handler = cmd_diff_handler;
  a_node_register(a_node_3, CMD_DIFF, callback_func);

  /* Step5: Client(1) sends message CMD_ADD */
  cmd_add.a = 123;
  cmd_add.b = 321;
  a_node_send(a_node_1, 777, CMD_ADD,
              &cmd_add,sizeof(cmd_add), 3);
  apr_event_wait(test_seq_event);

  /* Step6: Client(2) sends message CMD_DIFF */
  cmd_diff.a = 212;
  cmd_diff.b = 101;
  a_node_send(a_node_2, 888, CMD_DIFF,
              &cmd_diff,sizeof(cmd_diff), 3);
  apr_event_wait(test_seq_event);

  /* De-initialize 2 instance of a_node */
  a_node_deinit(a_node_1);
  a_node_deinit(a_node_2);
  a_node_deinit(a_node_3);

  apr_event_destroy(test_seq_event);
}

/* Main Test entry */
void a_node_test_using_reg_only_all()
{
  APR_DBG_0(APR_DBG_HIGH, "Test: a_node_test_using_reg_only_all :Started");
  sample_scenario_1();
  sample_scenario_2();
  APR_DBG_0(APR_DBG_HIGH, "Test: a_node_test_using_reg_only_all :Ended");
}

