/*==============================================================================
  @file using_default_callbacks_only.c
  @brief Sample implementation of a_node user that uses only default callbacks.
         User assigns 1 command and 1 response callback handler during a_node
         initializations. a_node user never invokes a_node_register() API and
         completely relies on the default command/response callbacks to handle
         all messages.

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.(QTI)
  All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/apr.common/2.0/a_node/test/src/using_default_callbacks_only.c#1 $

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
  Sample command handler for
  - OPCODE CMD_ADD (0x10)
  - CMD_DIFF (0x11)
  - Everything else (aka incoming message with unknown Opcode)

  Signature of function must comply with a_node_command_handler_t
*/
static int32_t cmd_handler(void *in_msg_ptr,
                           uint32_t in_msg_size,
                           uint32_t in_msg_opcode,
                           void *out_msg_ptr,
                           uint32_t out_msg_size,
                           uint32_t *out_msg_actual_size_ptr,
                           uint32_t *out_msg_opcode_ptr)
{
  int32_t rc = APR_EOK;

  if(NULL == in_msg_ptr              ||
     NULL == out_msg_ptr             ||
     NULL == out_msg_actual_size_ptr ||
     NULL == out_msg_opcode_ptr        )
  {
    /* NULL access */
    return APR_EBADPARAM;
  }

  switch(in_msg_opcode)
  {
    case CMD_ADD:
    {
      cmd_add_t *cmd_add_ptr = (cmd_add_t*)in_msg_ptr;
      rsp_calc_t *rsp_calc_ptr = (rsp_calc_t*)out_msg_ptr;
      if(out_msg_size < sizeof(rsp_calc_t))
      {
        /* Insufficient memory for response */
        rc = APR_ENOMEMORY;
        break;
      }
      *out_msg_actual_size_ptr = sizeof(rsp_calc_t);
      *out_msg_opcode_ptr = RSP_CALC;

      rsp_calc_ptr->result = cmd_add_ptr->a + cmd_add_ptr->b;
    }
    break;
    case CMD_DIFF:
    {
      cmd_diff_t *cmd_diff_ptr = (cmd_diff_t*)in_msg_ptr;
      rsp_calc_t *rsp_calc_ptr = (rsp_calc_t*)out_msg_ptr;
      if(out_msg_size < sizeof(rsp_calc_t))
      {
        /* Insufficient memory for response */
        rc = APR_ENOMEMORY;
        break;
      }
      *out_msg_actual_size_ptr = sizeof(rsp_calc_t);
      *out_msg_opcode_ptr = RSP_CALC;

      rsp_calc_ptr->result = cmd_diff_ptr->a - cmd_diff_ptr->b;
    }
    break;
    default:
    {
      /* Highlight error for propagation */
      rc = APR_EUNSUPPORTED;
      break;
    }
  }

  if(APR_EOK != rc )
  {
    /* Error occurred, need to send error message to client */
    if(out_msg_size < sizeof(aprv2_ibasic_rsp_result_t))
    {
      /* Let a_node know that user wasn't able to handle the error */
      rc = APR_EUNEXPECTED;
    }
    else
    {
      aprv2_ibasic_rsp_result_t *rsp_ibasic_ptr =
                          (aprv2_ibasic_rsp_result_t*)out_msg_ptr;
      *out_msg_opcode_ptr = APRV2_IBASIC_RSP_RESULT;
      *out_msg_actual_size_ptr = sizeof(aprv2_ibasic_rsp_result_t);

      rsp_ibasic_ptr->opcode = in_msg_opcode;
      rsp_ibasic_ptr->status = rc; /* Propagating error */
      rc = APR_EOK; /* Let a_node know that message is processed completely */
    }
  }

  return rc;
}


/*
  Sample response handler for OPCODE RSP_CALC (0x20) and
  APRV2_IBASIC_RSP_RESULT (0x000110E8)

  Signature of function must comply with a_node_response_handler_t
*/
static int32_t rsp_handler(void *msg_ptr,
                           uint32_t msg_size,
                           uint32_t msg_token,
                           uint32_t msg_opcode)
{

  if(NULL == msg_ptr)
  {
    /* NULL access */
    return APR_EBADPARAM;
  }


  switch(msg_opcode)
  {
    case RSP_CALC:
    {
      rsp_calc_t *rsp_calc_ptr = (rsp_calc_t*)msg_ptr;
      if(msg_size < sizeof(rsp_calc_t))
      {
        /* Insufficient space */
        return APR_ENOMEMORY;
      }
      APR_DBG_2(APR_DBG_HIGH, "Result:%d, token:%d",
                rsp_calc_ptr->result, msg_token);
    }
    break;
    case APRV2_IBASIC_RSP_RESULT:
    {
      aprv2_ibasic_rsp_result_t *rsp_ibasic_ptr =
                                (aprv2_ibasic_rsp_result_t*)msg_ptr;
      if(msg_size < sizeof(aprv2_ibasic_rsp_result_t))
      {
        /* Insufficient space */
        return APR_EUNEXPECTED;
      }
      APR_DBG_3(APR_DBG_HIGH, "Err[0x%x] processing command[0x%x], token:%d",
                rsp_ibasic_ptr->status, rsp_ibasic_ptr->opcode, msg_token);
    }
    break;
    default:
    {
      APR_DBG_2(APR_DBG_HIGH,
                "Unsupported response opcode[0x%x] having token:%d",
                msg_opcode, msg_token);
      return APR_EUNSUPPORTED;
    }
  }

  apr_event_signal(test_seq_event);
  return APR_EOK;
}

/*
  Sample scenario 1:
   Step1: Client(1) Initialize with default response handler
   Step2: Server(2) Initialize with default command handler
   Step3: Client(1) sends message CMD_ADD
 */
static void sample_scenario_1()
{
  a_node_t a_node_1, a_node_2;
  cmd_add_t cmd_add;
  apr_event_create(&test_seq_event);

  /* Step1: Client(1) Initialize with default response handler */
  a_node_init(&a_node_1, 1, 123, 4096, 4096, NULL, rsp_handler);

  /* Step2: Server(2) Initialize with default command handler */
  a_node_init(&a_node_2, 2, 123, 4096, 4096, cmd_handler, NULL);

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
   Step1: Client(1) Initialize with default response handler
   Step2: Client(3) Initialize with default response handler
   Step3: Server(2) Initialize with default command handler
   Step4: Client(1) sends message CMD_ADD
   Step5: Client(3) sends message CMD_DIFF
 */
static void sample_scenario_2()
{
  a_node_t a_node_1, a_node_2, a_node_3;
  cmd_add_t cmd_add;
  cmd_diff_t cmd_diff;
  apr_event_create(&test_seq_event);

  /* Step1: Client(1) Initialize with default response handler */
  a_node_init(&a_node_1, 1, 123, 4096, 4096, NULL, rsp_handler);

  /* Step2: Client(2) Initialize with default response handler */
  a_node_init(&a_node_2, 2, 123, 4096, 4096, NULL, rsp_handler);

  /* Step3: Server(3) Initialize with default command handler */
  a_node_init(&a_node_3, 3, 123, 4096, 4096, cmd_handler, NULL);

  /* Step4: Client(1) sends message CMD_ADD */
  cmd_add.a = 123;
  cmd_add.b = 321;
  a_node_send(a_node_1, 777, CMD_ADD,
              &cmd_add,sizeof(cmd_add), 3);
  apr_event_wait(test_seq_event);

  /* Step5: Client(2) sends message CMD_DIFF */
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
void a_node_test_using_default_only_all()
{
  APR_DBG_0(APR_DBG_HIGH, "Test: a_node_test_using_default_only_all :Started");
  sample_scenario_1();
  sample_scenario_2();
  APR_DBG_0(APR_DBG_HIGH, "Test: a_node_test_using_default_only_all :Ended");
}

