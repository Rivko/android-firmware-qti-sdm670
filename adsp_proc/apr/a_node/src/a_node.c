/*==============================================================================
  @file a_node.c
  @brief a_node interface implmentation

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.(QTI)
  All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/apr.common/2.0/a_node/src/a_node.c#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  02/25/16    Unni     Creating a_node using Toy Server/Client as reference
==============================================================================*/
#include "a_node_util.h"
#include "DALSys.h"
#include "aprv2_api_inline.h"
#include "apr_dbg_msg.h"
#include "aprv2_rtable_api_i.h"
#include <stdio.h> /* for snprintf */

/* Look-up table using a_node instances using APR port number */
a_node_map_t local_a_nodes;
/* Handle assigned by APR during registration and used for further APR 
   interactions */
uint32_t local_apr_handle;
/* Address returned by APR */
uint16_t local_apr_addr;
/* Remote a_node instance's APR address */
uint16_t remote_apr_addr;

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
                    a_node_response_handler_t default_rsp_callback_func)
{
  int32_t rc = APR_EOK;
  DALResult dal_rc;
  char my_name[A_NODE_THREAD_NAME_MAX_LEN];
  a_node_instance_t* a_node_ptr = NULL;
  uint8_t step = 0;
  uint32_t total_instance_size = 0;  

  /* NULL input check */
  if(NULL == handle_ptr)
  {
    APR_DBG_0(APR_DBG_ERROR, "Init with NULL Hanlde");
    return APR_EBADPARAM;
  }

  /* check whether local apr handles was populated, if not, do it once */
  if(0 == local_apr_handle)
  {
    uint16_t my_domain_id = 0;
    rc = aprv2_rtable_get_default_domain(&my_domain_id);
    switch(my_domain_id)
    {
      /* ADSP domain for MSM chips like 8996 */
      case APRV2_IDS_DOMAIN_ID_ADSP_V:
      /* Modem domain for MDM chips like 9x55 */
      case APRV2_IDS_DOMAIN_ID_MODEM_V:
        rc |= __aprv2_cmd_register2(&local_apr_handle,
                                    A_NODE_DOMAIN_NAME_ADSP,
                                    sizeof(A_NODE_DOMAIN_NAME_ADSP), 0,
                                    a_node_isr_dispatch_cb,
                                    (void*)&local_a_nodes,
                                    &local_apr_addr);
        rc |= __aprv2_cmd_local_dns_lookup(A_NODE_DOMAIN_NAME_APPS,
                                           sizeof(A_NODE_DOMAIN_NAME_APPS),
                                           &remote_apr_addr);
      break;
      case APRV2_IDS_DOMAIN_ID_APPS_V:
        rc |= __aprv2_cmd_register2(&local_apr_handle,
                                    A_NODE_DOMAIN_NAME_APPS,
                                    sizeof(A_NODE_DOMAIN_NAME_APPS), 0,
                                    a_node_isr_dispatch_cb,
                                    (void*)&local_a_nodes,
                                    &local_apr_addr);
        rc |= __aprv2_cmd_local_dns_lookup(A_NODE_DOMAIN_NAME_ADSP,
                                           sizeof(A_NODE_DOMAIN_NAME_ADSP),
                                           &remote_apr_addr);
      break;
      default:
        rc |= APR_EUNSUPPORTED;
      break;
    }
    
    /* Create lock for local a_node map(guid:port) */
    rc |= apr_lock_create(APR_LOCK_TYPE_INTERRUPT, &(local_a_nodes.lock));

    if(rc)
    {
      APR_DBG_2(APR_DBG_ERROR, "Err: A_node:APR register with domain %d,"
                "rc=0x%x",
                my_domain_id, rc);
      return rc;      
    }    
  }

  /* Append my_addr with a_node prefix to create a unique name */
  snprintf(my_name, A_NODE_THREAD_NAME_MAX_LEN, "%s%d",
           A_NODE_THREAD_NAME_PREFIX, (int)guid);

  /* Total size = size of instance structure + space for a response message +
                  space for callback function registrations */
  total_instance_size = sizeof(a_node_instance_t) +
                        max_rsp_msg_size;
  /* Allocate size for instance memory */
  dal_rc = DALSYS_Malloc(total_instance_size, (void **)&a_node_ptr);
  if(DAL_SUCCESS != dal_rc || NULL == a_node_ptr)
  {
    APR_DBG_1(APR_DBG_ERROR, "Err: allocate a_node instance memory of size %d",
              total_instance_size);
    return APR_ENOMEMORY;
  }

  /* Initialize all data to NULL or Zero state ensure safe de-init
     No need to memset zero extended data like list or appended buffer */
  DALSYS_memset(a_node_ptr, 0 , sizeof(a_node_instance_t));
  /* Store instance variables */
  a_node_ptr->guid = guid;
  a_node_ptr->prio = thread_prio;
  a_node_ptr->apr_handle = local_apr_handle;

  if(max_rsp_msg_size > 0)
  {
    /* Following the a_node instance space is response message buffer space */
    a_node_ptr->rsp_msg_buf_ptr = (void*)(a_node_ptr + 1);
    a_node_ptr->rsp_msg_buffer_size = max_rsp_msg_size;
  }

  a_node_ptr->default_cmd_callback_func = default_cmd_callback_func;
  a_node_ptr->default_rsp_callback_func = default_rsp_callback_func;

  /* Using dummy loop to track progress step and break on failure */
  while(1)
  {
    /* Initialize command queues. */
    rc = a_node_util_create_queues(&a_node_ptr->used_cmd_q,
                                   &a_node_ptr->free_cmd_q,
                                   a_node_ptr->cmds, A_NODE_CMD_QUEUE_SIZE);
    if(rc) break;
    step++;

    /* Initialize Response queues. */
    rc = a_node_util_create_queues(&a_node_ptr->used_rsp_q,
                                   &a_node_ptr->free_rsp_q,
                                   a_node_ptr->rsps, A_NODE_RSP_QUEUE_SIZE);
    if(rc) break;
    step++;

    /* Initialize Lock */
    rc = apr_lock_create(APR_LOCK_TYPE_INTERRUPT, &a_node_ptr->lock);
    if(rc) break;
    step++;

    /* Initialize Lock for callbacks */
    rc = apr_lock_create(APR_LOCK_TYPE_MUTEX,
                         &a_node_ptr->registered_callbacks.lock);
    if(rc) break;
    step++;

    /* Initialize Thread status event */
    rc = apr_event_create(&a_node_ptr->thread_status_event);
    if(rc) break;
    step++;

    /* Create a worker thread for processing incoming commands. */
    apr_thread_create(&a_node_ptr->thread, my_name,
                      thread_prio, NULL, stack_size,
                      a_node_workloop, (void*) a_node_ptr);
    if(rc) break;
    step++;

    /* Wait for thread launch to complete */
    rc = apr_event_wait(a_node_ptr->thread_status_event);
    if(rc) break;
    step++;

    /* Leaving client context - cannot wait on this event anymore */
    rc = apr_event_destroy(a_node_ptr->thread_status_event);
    if(rc) break;
    step++;

    /* Put the a_node instance in local list. */
    rc = a_node_map_put(&local_a_nodes, guid, (size_t)a_node_ptr);
    if(rc) break;
    step++;

    /* all done */
    break;
  }

  /* If error occured during initialization */
  if(rc)
  {
    APR_DBG_2(APR_DBG_ERROR,
              "Unable to Initialize resource at step [%d], err[0x%x]",
              step, rc);
    a_node_deinit((a_node_t)a_node_ptr);
    return rc;
  }

  /* Success */
  APR_DBG_1(APR_DBG_HIGH, "Initialized a_node_[%d] Successfully",
            a_node_ptr->guid);

  /* assign newly created handle */
  *handle_ptr = (a_node_t)a_node_ptr;

  return APR_EOK;

}

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
                        a_node_callback_func_t callback_func)
{
  a_node_instance_t* a_node_ptr = (a_node_instance_t*)handle;

  if(NULL == a_node_ptr)
  {
    APR_DBG_0(APR_DBG_HIGH,"Null handle for a_node_deinit");
    return APR_EBADPARAM;
  }

  /* Add the function with associated opcode */
  return a_node_map_put(&a_node_ptr->registered_callbacks,
                        msg_opcode,
                        *(size_t*)&callback_func);
}

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
                    uint8_t dest_guid)
{
  int32_t rc;
  a_node_instance_t* a_node_ptr = (a_node_instance_t*)handle;
  uint16_t dest_addr = 0;
  size_t temp_value = 0;

  if(NULL == a_node_ptr)
  {
    APR_DBG_0(APR_DBG_HIGH,"Null handle for a_node_send");
    return APR_EBADPARAM;
  }

  /* Look up previously cached values */
  rc = a_node_map_get(&local_a_nodes,
                      (uint32_t)dest_guid,
                      &temp_value);
  if(APR_EOK == rc)
  {
    dest_addr = local_apr_addr;
  }
  else
  {
    /* Since there is only two domains valid for A_NODE, desnitaion is fixed.
       If A_NODE extends to multiple domains, then there need to be an 
       advertisement or pre-fixed assignment of which GUID is present in which
       domain; followed by a look up of GUID vs dest_addr using a table */
    dest_addr = remote_apr_addr;
  }   

  return  __aprv2_cmd_alloc_send(a_node_ptr->apr_handle, /* apr handle */
                                 APRV2_PKT_MSGTYPE_SEQCMD_V, /* packet type */
                                 local_apr_addr, /* my apr addr */
                                 a_node_ptr->guid, /* my apr port */
                                 dest_addr, /* dest apr addr */
                                 dest_guid, /* dest apr port */
                                 msg_token, /* apr token */
                                 msg_opcode, /* message opcode */
                                 msg_ptr, /* message */
                                 msg_size ); /* message size */

}

/*
  De-Register the function callback associated with message opcode

  @param [in] handle: Handle to a_node instance
  @param [in] msg_opcode: Opcode used to identify the type of message whose
                          corresponding callback needs to be removed

  @return: 0 on success else failure
*/
int32_t a_node_deregister(a_node_t handle,
                          uint32_t msg_opcode)
{
  a_node_instance_t* a_node_ptr = (a_node_instance_t*)handle;

  if(NULL == a_node_ptr)
  {
    APR_DBG_0(APR_DBG_HIGH,"Null handle for a_node_deinit");
    return APR_EBADPARAM;
  }

  /* Add the function with associated opcode */
  return a_node_map_remove(&a_node_ptr->registered_callbacks,
                           msg_opcode);

}

/*
  De-Initialize a node instance

  @param [in] handle: Handle to a_node instance

  @return: 0 on success else failure
*/
int32_t a_node_deinit(a_node_t handle)
{
  int32_t rc = 0;
  a_node_instance_t* a_node_ptr = (a_node_instance_t*)handle;

  if(NULL == a_node_ptr)
  {
    APR_DBG_0(APR_DBG_HIGH,"Null handle for a_node_deinit");
    return APR_EBADPARAM;
  }

  /* APR cleanups are NULL pointer safe and complete instance data was
   * initialized to NULL or Zero. hence safe to call below de-init functions
   * regardless of initialization status */

  /* De-register from APR to stop receiving messages. */
  rc |= a_node_map_remove(&local_a_nodes, a_node_ptr->guid); 

  /* Signal the worker thread to go to completion */
  rc |= apr_event_create(&a_node_ptr->thread_status_event);
  rc |= apr_event_signal_abortall(a_node_ptr->pending_work_event);

  /* Wait fo thread to self destroy */
  rc |= apr_event_wait(a_node_ptr->thread_status_event);
  rc |= apr_event_destroy(a_node_ptr->thread_status_event);

  /* Thread Destroy */
  rc |= apr_thread_destroy(a_node_ptr->thread);

  /* De-initialize command queues. */
  rc |= apr_list_destroy(&a_node_ptr->free_cmd_q);
  rc |= apr_list_destroy(&a_node_ptr->used_cmd_q);
  rc |= apr_list_destroy(&a_node_ptr->free_rsp_q);
  rc |= apr_list_destroy(&a_node_ptr->used_rsp_q);

  /* De-initialize basic OS resources */
  rc |= apr_lock_destroy(a_node_ptr->lock);
  rc |= apr_lock_destroy(a_node_ptr->registered_callbacks.lock);

  APR_DBG_2(APR_DBG_HIGH,
            "Deinitialized a_node_[%d], err[0x%x]",
            a_node_ptr->guid, rc);

  /* Free up instance memory */
  DALSYS_Free(a_node_ptr->registered_callbacks.entry_ptr);
  DALSYS_Free(a_node_ptr);

  // TODO: Not thread safe but cleans up all resources 
  // if(0 == local_a_nodes.size)
  // {
  //   /* No client left, could de-register a_nodes with apr */
  //   rc |= __aprv2_cmd_deregister(local_apr_handle);
  //   local_apr_handle = local_apr_addr = 0;
  //   rc |= apr_lock_destroy(local_a_nodes.lock);
  // }

  return rc;
}
