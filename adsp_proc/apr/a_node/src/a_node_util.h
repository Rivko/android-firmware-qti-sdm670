/*==============================================================================
  @file a_node_util.h
  @brief Utility Interface used by a_node

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.(QTI)
  All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/apr.common/2.0/a_node/src/a_node_util.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  02/25/16    Unni     Creating a_node using Toy Server/Client as reference
==============================================================================*/
#include "apr_comdef.h"
#include "apr_list.h"
#include "apr_lock.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "aprv2_packet.h"
#include "a_node.h"
#include <stddef.h> /* for size_t */

/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/
/* Prefix used for a_node thread names */
#define A_NODE_THREAD_NAME_PREFIX  "a_node_"
/* Max length of thread name */
#define A_NODE_THREAD_NAME_MAX_LEN 10
/* Domain Name in APPS */
#define A_NODE_DOMAIN_NAME_APPS  "qcom.anode.apps"
/* Domain Name in Modem */
#define A_NODE_DOMAIN_NAME_ADSP  "qcom.anode.adsp"
/* Size of Command Queue */
#define A_NODE_CMD_QUEUE_SIZE 8
/* Size of Response Queue */
#define A_NODE_RSP_QUEUE_SIZE 8
/* Incremental growth size of entries in Map  */
#define A_NODE_MAP_INCR_SIZE 8

/* Structure used to encapsulate packet (data) and link with associated queue */
typedef struct
{
  apr_list_node_t link;
  /* APR type nodes link in a linked list - must be first element */

  aprv2_packet_t* packet;
  /* Reference to APR Packet */
}linked_packet_t;

/* Structure used store callback function with associated opcode */
typedef struct
{
  uint32_t key;
  /* Key of a map entry */

  size_t value;
  /* Value corresponding to the above key*/

}entry_t;

/* Structure used store callback function with associated opcode */
typedef struct
{
  uint16_t max_size;
  /* Maximum number of callback functions that can be registered */

  uint16_t size;
  /* number of callback functions that is registered */

  apr_lock_t lock;
  /* Lock */

  entry_t* entry_ptr;
  /* list of Key-Value paired entries */

}a_node_map_t;

/* Structure used to keep hold variables of an a_node instance */
typedef struct
{
  /********** 8 Bytes data **********/
  uint8_t guid;
  /* global unique instance id of this a_node */

  uint8_t prio;
  /* a_node thread priority*/

  /********** 16 Bytes data **********/

  /********** >=32 Bytes data **********/
  uint32_t apr_handle;
  /* Handle assigned by APR during registration and used for further
     APR interactions */

  apr_lock_t lock;
  /* Lock */

  apr_thread_t thread;
  /* Handle for a_node thread */

  apr_event_t thread_status_event;
  /* Event used to indicate whether when thread is running */

  apr_event_t pending_work_event;
  /* Event used to indicate if need task is present for worker thread */

  apr_list_t used_cmd_q;
  /* Used Command queue */

  apr_list_t free_cmd_q;
  /* Free Command queue */

  linked_packet_t cmds[A_NODE_CMD_QUEUE_SIZE];
  /* Link list of all command packets */

  apr_list_t used_rsp_q;
  /* Used Response queue */

  apr_list_t free_rsp_q;
  /* Free Response queue */

  linked_packet_t rsps[A_NODE_RSP_QUEUE_SIZE];
  /* Link list of all response packets */

  void* rsp_msg_buf_ptr;
  /* Buffer to cache response message to be used in command callback functions */

  uint32_t rsp_msg_buffer_size;
  /* Size of rsp_msg_buffer */

  a_node_map_t registered_callbacks;
  /* List to organized callback functions based on OPCODE */

  a_node_command_handler_t default_cmd_callback_func;
  /* Default Command callback function */

  a_node_response_handler_t default_rsp_callback_func;
  /* Default Response callback function */

}a_node_instance_t;


/*
  Utility to initialize a node queues

  @param [in] used_q_ptr: Queue of used packets
  @param [in] free_q_ptr: Queue of free packets
  @param [in] store_ptr: List of all available packets out of which used and
                         free gets assigned
  @param [in] store_size: Total number of all available packets

  @return: 0 on success else failure
*/
int32_t a_node_util_create_queues(apr_list_t* used_q_ptr,
                                  apr_list_t* free_q_ptr,
                                  linked_packet_t* store_ptr,
                                  uint32_t store_size);

/*
  Generic a node thread routine (aka workloop)

  @param [in] param: This is the parameter available during a thread launch.
                     Expected parameter is the a_node instance address for which
                     the thread was launched

  @return: 0 on success else failure
*/
int32_t a_node_workloop(void* param);

/*
  Generic a node Interrupt Service Routine callback

  @param [in] packet: Pointer to the incoming message (packet) to receive.
  @param [in] dispatch_data: This is the parameter available during a thread
                             ISR dispatcher. Expected parameter is the a_node
                             instance address for which the interrupt needs to
                             be serviced

  @return: 0 on success else failure
*/
int32_t a_node_isr_dispatch_cb(aprv2_packet_t* packet, void* dispatch_data);

/*
  Put key & value entry to Map

  @param [in] map_ptr: Pointer to Map
  @param [in] key: Key for the entry
  @param [in] value: Value corresponding to the above key

  @return: 0 on success else failure
*/
int32_t a_node_map_put(a_node_map_t* map_ptr,
                       uint32_t key,
                       size_t value);

/*
  Remove an entry from Map

  @param [in] map_ptr: Pointer to callback list
  @param [in] key: Key for the entry to be removed

  @return: 0 on success else failure
*/
int32_t a_node_map_remove(a_node_map_t* map_ptr,
                           uint32_t key);

/*
  Get Value from Map for a corresponding key

  @param [in] list_ptr: Pointer to Map
  @param [in] key: Key for the entry to be queried
  @param [out] value_ptr: Returned value corresponding to above key

  @return: Corresponding Callback function on success else NULL on failure
*/
int32_t a_node_map_get(a_node_map_t* map_ptr,
                       uint32_t key,
                       size_t* value_ptr);
