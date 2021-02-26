#ifndef __A_NODE_H__
#define __A_NODE_H__

/*==============================================================================
  @file a_node.h
  @brief Interface for a_node user

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.(QTI)
  All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/apr.common/2.0/a_node/inc/a_node.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  02/25/16    Unni     Creating a_node using Toy Server/Client as reference
==============================================================================*/

#include "apr_comdef.h"

/*==============================================================================
   Type definitions
==============================================================================*/

/* Handle to a node instance */
typedef void* a_node_t;



/*
  Callback function signature for command handler

  @param [in] in_msg_ptr: Pointer to received message
  @param [in] in_msg_size: Size of received message
  @param [in] in_msg_opcode: Opcode to received message
  @param [out] out_msg_ptr: Pointer of message to be send
  @param [in] out_msg_size: Size available for message to be send
  @param [out] out_msg_actual_size_ptr: Indicate the size (<=out_msg_size)
                                        used for sending message
  @param [out] out_msg_opcode_ptr: Indicate the opcode of the out going message

  @return: 0 on success else failure
*/
typedef int32_t (*a_node_command_handler_t)(void *in_msg_ptr,
                                            uint32_t in_msg_size,
                                            uint32_t in_msg_opcode,
                                            void *out_msg_ptr,
                                            uint32_t out_msg_size,
                                            uint32_t *out_msg_actual_size_ptr,
                                            uint32_t *out_msg_opcode_ptr);


/*
  Callback function signature for response handler

  @param [in] msg_ptr: Pointer to received message
  @param [in] msg_size: Size of received message
  @param [in] msg_token: Token used while sending the message
  @param [in] msg_opcode: Opcode to received message

  @return: 0 on success else failure
*/
typedef int32_t (*a_node_response_handler_t)(void *msg_ptr,
                                             uint32_t msg_size,
                                             uint32_t msg_token,
                                             uint32_t msg_opcode);

/*
  Callback function signature

  @brief a_node user can define custom message handler using this callback
  function signature and register with a_node. During registration, a callback
  will be associated with a message opcode. When a_node instance receives a
  message, the registered callback will be called with the following parameters

  Callback function could be either a command handler or a response handler
*/
typedef union
{
  a_node_command_handler_t command_handler;
  a_node_response_handler_t response_handler;
}a_node_callback_func_t;

/*==============================================================================
   Function declarations
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
  Initialize a node instance

  @param [out] hanlde_ptr: Newly created a_node instance handle
  @param [in] guid: Global Unique id to identify this a_node instance
  @param [in] thread_prio: Priority that needs to be assigned to a_node thread
  @param [in] stack_size: Stack size that needs to be assigned to a_node thread
  @param [in] max_rsp_msg_size: Buffer size that need to be allocate for the
                biggest response message from this a_node. Only one instance of
                response message is allocated as only one message is processed
                at a time.
  @param [in] default_cmd_callback_func: Register default command handler. On
                receiving a command message with opcode not matching with any
                registered callback, the default_cmd_callback_func is called.
  @param [in] default_rsp_callback_func: Register default response handler. On
                receiving a response message with opcode not matching with any
                registered callback, the default_rsp_callback_func is called.

  @return: 0 on success else failure
*/
int32_t a_node_init(a_node_t* handle_ptr,
                    uint8_t guid,
                    uint8_t thread_prio,
                    uint32_t stack_size,
                    uint32_t max_rsp_msg_size,
                    a_node_command_handler_t default_cmd_callback_func,
                    a_node_response_handler_t default_rsp_callback_func);

/*
  Register a callback function corresponding to a message opcode

  @param [in] handle: Handle to a_node instance
  @param [in] msg_opcode: Opcode used to identify the type of message to handle
  @param [in] callback_func: Function that needs to be called for above
                             message opcode

  @return: 0 on success else failure
*/
int32_t a_node_register(a_node_t handle,
                        uint32_t msg_opcode,
                        a_node_callback_func_t callback_func);

/*
  Send a message

  @param [in] handle: Handle to a_node instance
  @param [in] msg_token: Unique token associated with message and it's response
  @param [in] msg_opcode: Opcode used to identify the type of message being send
  @param [in] msg_ptr: Message that needs to be send
  @param [in] msg_size: Size of message that needs to be send
  @param [in] dest_guid: Destination a_node's global unique id, used for routing

  @return: 0 on success else failure
*/
int32_t a_node_send(a_node_t handle,
                    uint32_t msg_token,
                    uint32_t msg_opcode,
                    void* msg_ptr,
                    uint32_t msg_size,
                    uint8_t dest_guid);


/*
  De-Register the function callback associated with message opcode

  @param [in] handle: Handle to a_node instance
  @param [in] msg_opcode: Opcode used to identify the type of message whose
                          corresponding callback needs to be removed

  @return: 0 on success else failure
*/
int32_t a_node_deregister(a_node_t handle,
                          uint32_t msg_opcode);

/*
  De-Initialize a node instance

  @param [in] handle: Handle to a_node instance

  @return: 0 on success else failure
*/
int32_t a_node_deinit(a_node_t handle);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* __A_NODE_H__ */

