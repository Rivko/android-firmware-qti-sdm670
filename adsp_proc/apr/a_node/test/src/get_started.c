/*==============================================================================
  @file get_started.c
  @brief A Template that can be used to get started on a custom implementation
         using a_node.

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.(QTI)
  All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/apr.common/2.0/a_node/test/src/get_started.c#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  03/08/16    Unni     Initial Version
==============================================================================*/

#include "a_node.h"
/* a_node interface - API used to invoke a_node functionalities */
#include "aprv2_msg_if.h"
/* APR basic Message interface - For error handling */
#include "apr_errcodes.h"
/* APR error code - Assign meaning for errors */

//#include "test_msg.h" <-- Replace with user defined messages
/* User Message interface - Defined by USER */
#include "apr_dbg_msg.h" //TODO: Replace with user's appropriate debug support
/* APR debug logging - Print debug logs on QXDM using APR infrastructure */

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
  if(NULL == in_msg_ptr              ||
     NULL == out_msg_ptr             ||
     NULL == out_msg_actual_size_ptr ||
     NULL == out_msg_opcode_ptr        )
  {
    /* NULL access */
    return APR_EBADPARAM;
  }

  /* User can implement a custom error handling here. This function is assigned
     to a_node during init and will be called if no registered handlers is found
  */
  if(out_msg_size < sizeof(aprv2_ibasic_rsp_result_t))
  {
    /* Insufficient size */
    return APR_ENOMEMORY;
  }

  /* Using IBASIC response format */
  aprv2_ibasic_rsp_result_t *rsp_ibasic_ptr =
      (aprv2_ibasic_rsp_result_t*)out_msg_ptr;
  *out_msg_opcode_ptr = APRV2_IBASIC_RSP_RESULT;
  *out_msg_actual_size_ptr = sizeof(aprv2_ibasic_rsp_result_t);

  rsp_ibasic_ptr->opcode = in_msg_opcode;
  rsp_ibasic_ptr->status = APR_EUNSUPPORTED;

  /* If client returns anything other than success (APR_EOK i.e. 0), a_node/apr
   * will internally generate APRV2_IBASIC_RSP_RESULT with details exactly as
   * shown above and send it to client */

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
  /*TODO: Below is a simple error printing routine. User can change to more
          advanced client notification or subroutines */

  if(NULL == msg_ptr)
  {
    /* NULL access */
    APR_DBG_0(APR_DBG_ERROR, "Received NULL response Message");
    return APR_EBADPARAM;
  }

  if(APRV2_IBASIC_RSP_RESULT != msg_opcode)
  {
    /* Unsupported response opcode */
    APR_DBG_1(APR_DBG_ERROR, "Unsupported Response [%x]", msg_opcode);
    return APR_EUNSUPPORTED;
  }

  if(msg_size < sizeof(aprv2_ibasic_rsp_result_t))
  {
    /* Insufficient size */
    APR_DBG_2(APR_DBG_ERROR, "msg size[%d] < IBASIC response size[%d]",
              msg_size,
              sizeof(aprv2_ibasic_rsp_result_t));
    return APR_ENOMEMORY;
  }

  /* Extract IBASIC response */
  aprv2_ibasic_rsp_result_t *rsp_ibasic_ptr =
                            (aprv2_ibasic_rsp_result_t*)msg_ptr;

  APR_DBG_3(APR_DBG_HIGH, "Err[0x%x] processing command[0x%x], token:%d",
            rsp_ibasic_ptr->status, rsp_ibasic_ptr->opcode, msg_token);

  return APR_EOK;
}

/*
  Sample command handler for x corresponding to a unique opcode

  Signature of function must comply with a_node_command_handler_t
*/
static int32_t cmd_x_handler(void *in_msg_ptr,
                             uint32_t in_msg_size,
                             uint32_t in_msg_opcode,
                             void *out_msg_ptr,
                             uint32_t out_msg_size,
                             uint32_t *out_msg_actual_size_ptr,
                             uint32_t *out_msg_opcode_ptr)
{
  if(NULL == in_msg_ptr              ||
     NULL == out_msg_ptr             ||
     NULL == out_msg_actual_size_ptr ||
     NULL == out_msg_opcode_ptr        )
  {
    /* NULL access */
    return APR_EBADPARAM;
  }

  //TODO: Implement Command handler below
//  if(CMD_ADD != in_msg_opcode)
//  {
//    /* Unexpected Opcode */
//    return APR_EUNEXPECTED;
//  }
//
//  cmd_add_t *cmd_add_ptr = (cmd_add_t*)in_msg_ptr;
//  rsp_calc_t *rsp_add_ptr = (rsp_calc_t*)out_msg_ptr;
//  *out_msg_actual_size_ptr = sizeof(rsp_calc_t);
//  *out_msg_opcode_ptr = RSP_CALC;
//
//  rsp_add_ptr->result = cmd_add_ptr->a + cmd_add_ptr->b;

  return 0;
}
/*
  Sample response handler for y corresponding to a unique opcode

  Signature of function must comply with a_node_response_handler_t
*/
static int32_t rsp_y_handler(void *msg_ptr,
                             uint32_t msg_size,
                             uint32_t msg_token,
                             uint32_t msg_opcode)
{
  if(NULL == msg_ptr)
  {
    /* NULL access */
    return APR_EBADPARAM;
  }

  //TODO: Implement Response handler below
//  if(RSP_CALC != msg_opcode)
//  {
//    /* Unexpected response */
//    return APR_EUNEXPECTED;
//  }
//
//  if(msg_size < sizeof(rsp_calc_t))
//  {
//    /* Insufficient space */
//    return APR_ENOMEMORY;
//  }
//
//  rsp_calc_t *rsp_add_ptr = (rsp_calc_t*)msg_ptr;
//
//  /* MSG Token can be used to distinguish the responses */
//  /* In this example, we have mapped 777 for SUM and 888 for DIFF. Users are
//     free to overload msg token if they have any custom implementation for
//     their responses (like signaling a thread using token as signal variable,
//     or calling a different subrouting using token as function pointer, etc)
//     and not restricted to using just switch cases */
//  switch(msg_token)
//  {
//    case 777:
//      APR_DBG_2(APR_DBG_HIGH, "Sum:%d, token:%d",
//                    rsp_add_ptr->result, msg_token);
//      break;
//    case 888:
//      APR_DBG_2(APR_DBG_HIGH, "Diff:%d, token:%d",
//                rsp_add_ptr->result, msg_token);
//      break;
//    default:
//      APR_DBG_2(APR_DBG_HIGH, "Result:%d, token:%d",
//                rsp_add_ptr->result, msg_token);
//      break;
//  }

  return 0;
}

/* Main Test entry */
void get_started_main()
{
  /* Instance handle of a_node */
  a_node_t a_node_1;
  /* store return code */
  int32_t rc = APR_EOK;

  /*----------------------------------------------------------------------------
   * Initialization
   *--------------------------------------------------------------------------*/
  /* Assignment of a GUID (Global Unique IDentifier) for this a_node instance */
  uint8_t guid = 1;
  /* Assignment of thread priority for this a_node instance's thread */
  uint8_t thread_prio = 123;
  /* Assignment of stack size for this a_node instance's thread */
  uint32_t stack_size = 8192;
  /* Assignment of maximum number of response messages that should be handled
     concurrently by this a_node instance */
  uint32_t max_rsp_msg_size = 20;

  /* Initialize a_node_1 */
  rc = a_node_init(&a_node_1, guid, thread_prio,
                   stack_size, max_rsp_msg_size,
                   cmd_err_handler, rsp_basic_handler);
  if(rc)
  {
    APR_DBG_2(APR_DBG_ERROR, "Failed a_node_init(%d):0x%x", guid, rc);
    return;
  }
  /*--------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------------
   * Registration
   *--------------------------------------------------------------------------*/
  /* Once command handler is ready to be used, it can be registered */
  /* Assignment of which message had to be handled the cmd_x_handler */
  uint32_t cmd_msg_opcode = 0x10;
  /* Assignment of corresponding callback function */
  a_node_callback_func_t callback_func;
  callback_func.command_handler = cmd_x_handler;

  /* Register a Command Handler */
  rc = a_node_register(a_node_1, cmd_msg_opcode, callback_func);
  if(rc)
  {
    APR_DBG_2(APR_DBG_ERROR, "Failed a_node_register(%d):0x%x",
              cmd_msg_opcode, rc);
    a_node_deinit(a_node_1);
    return;
  }

  /* Before sending any message, all expected Response handlers needs to be
     registered. */
  /* Assignment of which message had to be handled the rsp_y_handler */
  uint32_t rsp_msg_opcode = 0x20;
  /* Assignment of corresponding callback function */
  callback_func.response_handler = rsp_y_handler;

  /* Register a Response Handler */
  rc = a_node_register(a_node_1, rsp_msg_opcode, callback_func);
  if(rc)
  {
    APR_DBG_2(APR_DBG_ERROR, "Failed a_node_register(%d):0x%x",
              rsp_msg_opcode, rc);
    a_node_deinit(a_node_1);
    return;
  }
  /*--------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------------
   * Sending
   *--------------------------------------------------------------------------*/
  /* Populate Message before sending */
  uint32_t msg = 0;
  /* Token that will be recieved by response handler */
  uint32_t msg_token = 0;
  /* Opcode that defines the message to the reciever */
  uint32_t msg_opcode = 0x30;
  /* Reciever's GUID (Global Unique IDentifier) */
  uint8_t dest_guid = 2;

  /* Send a Message */
  rc = a_node_send(a_node_1, msg_token, msg_opcode, &msg, sizeof(msg),
                   dest_guid);
  if(rc)
  {
    APR_DBG_2(APR_DBG_ERROR, "Failed a_node_send(%d):0x%x",
              dest_guid, rc);
    a_node_deinit(a_node_1);
    return;
  }
  /*--------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------------
   * De-registration
   *--------------------------------------------------------------------------*/
  /* Once a command/response handler subroutine isn't available or need to be
     replace with something else, the original handler can be de-registered */

  /* De-Register a Handler */
  rc = a_node_deregister(a_node_1, cmd_msg_opcode);
  if(rc)
  {
    APR_DBG_2(APR_DBG_ERROR, "Failed a_node_deregister(%d):0x%x",
              cmd_msg_opcode, rc);
  }

  /* a_node_deinit() will implicitly clean up all resources assigned to it -
     including all registered handler. Hence, this isn't a mandatory step */
  /*--------------------------------------------------------------------------*/


  /*----------------------------------------------------------------------------
   * De-initialize
   *--------------------------------------------------------------------------*/
  /* De-initialize the instance of a_node */
  rc = a_node_deinit(a_node_1);
  if(rc)
  {
    APR_DBG_2(APR_DBG_ERROR, "Failed a_node_deinit(%d):0x%x",
              guid, rc);
  }
  /*--------------------------------------------------------------------------*/
}

