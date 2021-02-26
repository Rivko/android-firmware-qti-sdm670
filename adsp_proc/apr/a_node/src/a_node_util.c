/*==============================================================================
  @file a_node_util.c
  @brief Utility used by a_node

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.(QTI)
  All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/apr.common/2.0/a_node/src/a_node_util.c#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  02/25/16    Unni     Creating a_node using Toy Server/Client as reference
==============================================================================*/

#include "aprv2_api_inline.h"
#include "a_node_util.h"
#include "apr_dbg_msg.h"
#include "DALSys.h"

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
                                  uint32_t store_size)
{
    uint32_t index;
    int32_t rc = APR_EOK;

    /* NULL Pointer check before dereference */
    if(NULL == store_ptr)
    {
      APR_DBG_0(APR_DBG_ERROR, "Null store_ptrinitializing a node Qs");
      return APR_EBADPARAM;
    }

    /* Init list */
    rc |= apr_list_init(used_q_ptr, NULL , NULL);
    rc |= apr_list_init(free_q_ptr, NULL , NULL);

    if(rc)
    {
      APR_DBG_1(APR_DBG_ERROR, "Err initializing a node Qs[0x%x]", rc);
      return rc;
    }

    /* Populating free items */
    for(index = 0; index < store_size; ++index)
    {
      rc |= apr_list_add_tail(free_q_ptr, &store_ptr[ index ].link);
    }

    if(rc)
    {
      APR_DBG_1(APR_DBG_ERROR, "Err populating free Qs[0x%x]", rc);
      return rc;
    }

    /* All success */
    return APR_EOK;
}


/*
  Put key & value entry to list

  @param [in] map_ptr: Pointer to Map
  @param [in] key: Key for the entry
  @param [in] value: Value corresponding to the above key

  @return: 0 on success else failure
*/
int32_t a_node_map_put(a_node_map_t* map_ptr,
                       uint32_t key,
                       size_t value)
{
  uint16_t iter = 0, pos = 0;

  /* NULL pointer access */
  if(NULL == map_ptr)
  {
    APR_DBG_0(APR_DBG_ERROR, "Add with NULL map_ptr");
    return APR_EBADPARAM;
  }

  /* Lock list during lookup/modification */
  apr_lock_enter(map_ptr->lock);

  /* Find the position */
  pos = map_ptr->size;
  while(iter < map_ptr->size)
  {
    if(map_ptr->entry_ptr[iter].key == key )
    {
      /* Not Allowed */
      APR_DBG_1(APR_DBG_ERROR, "Adding new key denied,"
                "need to remove previously mapped for key 0x%x",
                key);
      apr_lock_leave(map_ptr->lock);
      return APR_EDENIED;
    }
    else if(map_ptr->entry_ptr[iter].key > key)
    {
      /* Found */
      pos = iter;
      break;
    }
    iter++;
  }

  /* Need to increase the size */
  if(map_ptr->size==map_ptr->max_size)
  {
    /* Realloc (i.e. Malloc, Copy, Free old instance) */
    DALResult dal_rc;
    void* temp_ptr = NULL;
    map_ptr->max_size += sizeof(entry_t)*A_NODE_MAP_INCR_SIZE;

    dal_rc = DALSYS_Malloc(map_ptr->max_size, &temp_ptr);
    if(DAL_SUCCESS != dal_rc || NULL == temp_ptr)
    {
      /* Alloc fail, bail out*/
      APR_DBG_1(APR_DBG_ERROR, 
                "Err: cannot grow a_node map to size %d",
                map_ptr->max_size);
      map_ptr->max_size -= sizeof(entry_t)*A_NODE_MAP_INCR_SIZE;
      apr_lock_leave(map_ptr->lock);
      return APR_ENOMEMORY;
    }
    if(map_ptr->size > 0)
    {
      /* Copy and free old instance */
      DALSYS_memscpy(temp_ptr, map_ptr->max_size,
                     map_ptr->entry_ptr, map_ptr->size);
      DALSYS_Free(map_ptr->entry_ptr);
    }
    map_ptr->entry_ptr = temp_ptr;
  }

  /* Increment the list size */
  map_ptr->size++;

  /* Push rest of the entries */
  iter = map_ptr->size-1;
  while(iter > pos)
  {
    map_ptr->entry_ptr[iter].value = map_ptr->entry_ptr[iter-1].value;
    map_ptr->entry_ptr[iter].key =  map_ptr->entry_ptr[iter-1].key;
    iter--;
  }

  /* Now insert the new entry */
  map_ptr->entry_ptr[pos].value = value;
  map_ptr->entry_ptr[pos].key = key;

  apr_lock_leave(map_ptr->lock);
  return APR_EOK;
}

/*
  Remove an entry from Map

  @param [in] map_ptr: Pointer to callback list
  @param [in] key: Key for the entry to be removed

  @return: 0 on success else failure
*/
int32_t a_node_map_remove(a_node_map_t* map_ptr,
                           uint32_t key)
{
  uint16_t iter = 0;

  /* NULL pointer access */
  if(NULL == map_ptr)
  {
    APR_DBG_0(APR_DBG_ERROR, "Remove with NULL map_ptr");
    return APR_EBADPARAM;
  }

  /* Lock list during lookup/modification */
  apr_lock_enter(map_ptr->lock);

  /* Find the position */
  while(iter < map_ptr->size)
  {
    if(map_ptr->entry_ptr[iter].key == key )
    {
      /* Found */
      break;
    }
    iter++;
  }

  /* Not Found */
  if(iter == map_ptr->size)
  {
    APR_DBG_1(APR_DBG_ERROR, "Unable to find entry for key 0x%x to remove",
              key);
    apr_lock_leave(map_ptr->lock);
    return APR_ENOTEXIST;
  }

  /* decrement the list size */
  map_ptr->size--;
  /* Push back rest of the entries */
  while(iter < map_ptr->size)
  {
    map_ptr->entry_ptr[iter].value = map_ptr->entry_ptr[iter+1].value;
    map_ptr->entry_ptr[iter].key =  map_ptr->entry_ptr[iter+1].key;
    iter++;
  }

  if(0 == map_ptr->size)
  {
    /* No entries left, just Free */
    DALSYS_Free(map_ptr->entry_ptr);
    map_ptr->entry_ptr = NULL;
    map_ptr->max_size = 0;
  }

// Incremental reduction - gets messy if a MALLOC fails in between
// if(A_NODE_MAP_INCR_SIZE == map_ptr->max_size - map_ptr->size)
// {
//   map_ptr->max_size -= sizeof(entry_t)*A_NODE_MAP_INCR_SIZE;
//   if(0 == map_ptr->max_size)
//   {
//     /* No entries left, just Free */
//     DALSYS_Free(map_ptr->entry_ptr);
//   }
//   else
//   {
//     /* Realloc (i.e. Malloc, Copy, Free old instance) */
//     DALResult dal_rc;
//     void* temp_ptr = NULL;    
//     dal_rc = DALSYS_Malloc(map_ptr->max_size, &temp_ptr);
//     if(DAL_SUCCESS != dal_rc || NULL == temp_ptr)
//     {
//       /* Alloc fail, bail out*/
//       APR_DBG_1(APR_DBG_ERROR, 
//                 "Err: cannot reduce a_node map to size %d",
//                 map_ptr->max_size);
//       map_ptr->max_size += sizeof(entry_t)*A_NODE_MAP_INCR_SIZE;
//       apr_lock_leave(map_ptr->lock);
//       return APR_ENOMEMORY;
//     }
//     /* Copy and free old instance */
//     DALSYS_memscpy(temp_ptr, map_ptr->max_size,
//                    map_ptr->entry_ptr, map_ptr->size);      
//     map_ptr->entry_ptr = temp_ptr;
//   }
// }

  /* Success */
  apr_lock_leave(map_ptr->lock);
  return APR_EOK;
}

/*
  Get Value from Map for a corresponding key

  @param [in] list_ptr: Pointer to Map
  @param [in] key: Key for the entry to be queried
  @param [out] value_ptr: Returned value corresponding to above key

  @return: Corresponding Callback function on success else NULL on failure
*/
int32_t a_node_map_get(a_node_map_t* map_ptr,
                       uint32_t key,
                       size_t* value_ptr)
{
  int32_t min,max,mid;

  /* NULL pointer access */
  if(NULL == map_ptr || NULL == value_ptr)
  {
    APR_DBG_2(APR_DBG_ERROR, "Get with NULL map_ptr %x, callback_ptr %x",
              map_ptr, value_ptr);
    return APR_EBADPARAM;
  }

  /* Lock list during lookup/modification */
  apr_lock_enter(map_ptr->lock);

  /* Using negative limit for comparison */
  max = (int32_t)map_ptr->size-1;
  min = 0;

  /* Binary search */
  while(min<=max)
  {
    /* Find mid way */
    mid = (max+min)>>1;
    if(map_ptr->entry_ptr[mid].key == key )
    {
      /* Found */
      apr_lock_leave(map_ptr->lock);
      *value_ptr = map_ptr->entry_ptr[mid].value;
      return APR_EOK;
    }
    else if(map_ptr->entry_ptr[mid].key < key)
    {
      min = mid + 1;
    }
    else
    {
      max = mid - 1;
    }
  }

  /* Not Found */
  apr_lock_leave(map_ptr->lock);
  return APR_ENOTEXIST;
}

/*
   Utility to process command message and send response message

  @param [in] pkt_ptr: Packets which has the command that needs to be processed
  @param [in] callback_func: Registered callback to handle the above command
  @param [in] a_node_ptr: instance of a_node


  @return: 0 on success else failure

 */
static int32_t a_node_process_cmd(aprv2_packet_t* pkt_ptr,
                                  a_node_command_handler_t callback_func,
                                  a_node_instance_t* a_node_ptr)
{
  /* Process incoming message and generate response */
  int32_t rc;
  void *in_msg_ptr = APRV2_PKT_GET_PAYLOAD(void, pkt_ptr);
  uint32_t in_msg_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(pkt_ptr->header);
  void *out_msg_ptr = a_node_ptr->rsp_msg_buf_ptr;
  uint32_t out_msg_size = a_node_ptr->rsp_msg_buffer_size;
  uint32_t out_msg_actual_size = 0;
  uint32_t out_msg_opcode;
  rc = callback_func(in_msg_ptr, in_msg_size,
                     pkt_ptr->opcode,
                     out_msg_ptr, out_msg_size,
                     &out_msg_actual_size,
                     &out_msg_opcode);
  if(rc)
  {
    /* Error in callback, forward error to client */
    APR_DBG_2(APR_DBG_ERROR, "Error processing msg in a_node_[%d], [0x%x]",
              a_node_ptr->guid, rc);
    rc = __aprv2_cmd_end_command(a_node_ptr->apr_handle, pkt_ptr, rc);
  }
  else
  {
    /* Success, send response to client */
    rc = __aprv2_cmd_alloc_send(a_node_ptr->apr_handle,
                                APRV2_PKT_MSGTYPE_CMDRSP_V,
                                pkt_ptr->dst_addr, pkt_ptr->dst_port,
                                pkt_ptr->src_addr, pkt_ptr->src_port,
                                pkt_ptr->token,
                                out_msg_opcode,
                                out_msg_ptr, out_msg_actual_size );
    rc |=  __aprv2_cmd_free(a_node_ptr->apr_handle, pkt_ptr);
  }

  return rc;
}
/*
   Utility to process response message

  @param [in] pkt_ptr: Packets which has the response that needs to be processed
  @param [in] callback_func: Registered callback to handle the above response
  @param [in] a_node_ptr: instance of a_node


  @return: 0 on success else failure

 */
static int32_t a_node_process_rsp(aprv2_packet_t* pkt_ptr,
                                  a_node_response_handler_t callback_func,
                                  a_node_instance_t* a_node_ptr)
{
  /* process response message and free packet */
  int32_t rc;
  void *in_msg_ptr = APRV2_PKT_GET_PAYLOAD(void, pkt_ptr);
  uint32_t in_msg_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(pkt_ptr->header);
  rc = callback_func(in_msg_ptr, in_msg_size, pkt_ptr->token, pkt_ptr->opcode);
  if(rc)
  {
    /* Error in callback, forward error to client */
    APR_DBG_2(APR_DBG_ERROR, "Error processing msg in a_node_[%d], [0x%x]",
              a_node_ptr->guid, rc);
  }
  rc =  __aprv2_cmd_free(a_node_ptr->apr_handle, pkt_ptr);
  return rc;
}

/*
  Utility to process a node used queue items and push them into free queue

  @param [in] used_q_ptr: Queue of used packets
  @param [in] free_q_ptr: Queue of free packets

  @return: 0 on success else failure
*/
static void a_node_process_work_queue(apr_list_t* used_q_ptr,
                                      apr_list_t* free_q_ptr,
                                      a_node_instance_t* a_node_ptr)
{
  int32_t rc;
  linked_packet_t* item_ptr;
  aprv2_packet_t* pkt_ptr;
  size_t temp_value;
  a_node_callback_func_t callback_func;
  uint32_t apr_field = 0;

  /* Process all available items in used queue */
  while(1)
  {
    apr_lock_enter(a_node_ptr->lock);
    rc = apr_list_remove_head(used_q_ptr, ((apr_list_node_t**)&item_ptr));
    apr_lock_leave(a_node_ptr->lock);
    if(rc) break;

    /* Get packet from used queue item */
    pkt_ptr = item_ptr->packet;
    apr_field = APR_GET_FIELD( APRV2_PKT_MSGTYPE, pkt_ptr->header );

    /* Find any registered Handlers */
    rc = a_node_map_get(&a_node_ptr->registered_callbacks,
                        pkt_ptr->opcode,
                        &temp_value);
    if(APR_EOK == rc)
    {
      callback_func = *(a_node_callback_func_t*)&temp_value;
    }
    else
    {
      /* If not found, check for default */
      if(APRV2_PKT_MSGTYPE_SEQCMD_V == apr_field)
      {
        if(NULL != a_node_ptr->default_cmd_callback_func)
        {
          /* Get Default Command Handler */
          callback_func.command_handler = a_node_ptr->default_cmd_callback_func;
          rc = APR_EOK;
        }
      }
      else
      {
        if(NULL != a_node_ptr->default_rsp_callback_func)
        {
          /* Get Response Command Handler */
          callback_func.response_handler =
                                          a_node_ptr->default_rsp_callback_func;
          rc = APR_EOK;
        }
      }
    }

    if(APR_EOK == rc)
    {
      if(APRV2_PKT_MSGTYPE_SEQCMD_V == apr_field)
      {
        /* Handle command */
        rc = a_node_process_cmd(pkt_ptr,
                                callback_func.command_handler,
                                a_node_ptr);
      }
      else
      {
        /* Handle response */
        rc = a_node_process_rsp(pkt_ptr,
                                callback_func.response_handler,
                                a_node_ptr);
      }
    }
    else
    {
      /* Unknown opcode, forward error to client */
      APR_DBG_3(APR_DBG_ERROR,
                "No Handler found for msg with opcode[0x%x] in a_node_[%d], "
                "search err[0x%x]", pkt_ptr->opcode, a_node_ptr->guid, rc);
      rc = __aprv2_cmd_end_command(a_node_ptr->apr_handle,
                                   pkt_ptr,
                                   APR_EUNSUPPORTED);
    }

    if(rc)
    {
      /* Error sending reply */
      APR_DBG_3(APR_DBG_ERROR, "Error sending reply for msg opcode[0x%x],"
                "token[0x%x] from a_node_[%d]",
                pkt_ptr->opcode, pkt_ptr->token, a_node_ptr->guid);
      APR_DBG_1(APR_DBG_ERROR, "Error sending reply rc [0x%x]",rc);
    }

    /* Add processed item to free queue */
    apr_lock_enter(a_node_ptr->lock);
    apr_list_add_tail(free_q_ptr, &item_ptr->link );
    apr_lock_leave(a_node_ptr->lock);
  }
}

/*
  Generic a node thread routine (aka workloop)

  @param [in] param: This is the parameter available during a thread launch.
                     Expected parameter is the a_node instance address for which
                     the thread was launched

  @return: 0 on success else failure
*/
int32_t a_node_workloop(void* param)
{
  int32_t rc;
  a_node_instance_t* a_node_ptr = (a_node_instance_t*)param ;

  APR_DBG_1(APR_DBG_HIGH, "Entering a_node_[%d] worker thread",
            a_node_ptr->guid);

  /* Need to create working event on working thread (Note:DALSYS limitation) */
  apr_event_create(&a_node_ptr->pending_work_event);

  /* Thread launch status indication */
  apr_event_signal(a_node_ptr->thread_status_event);

  /* continuous working loop */
  while(1)
  {
    /* wait for pending work event */
    rc = apr_event_wait(a_node_ptr->pending_work_event);
    if ( rc ) break;

    APR_DBG_1(APR_DBG_MEDIUM, "Waking up a_node_[%d] to do work",
              a_node_ptr->guid);

    /* Process cmd queue and response queue if any data is present */
    a_node_process_work_queue(&a_node_ptr->used_rsp_q,
                              &a_node_ptr->free_rsp_q,
                              a_node_ptr);
    a_node_process_work_queue(&a_node_ptr->used_cmd_q,
                              &a_node_ptr->free_cmd_q,
                              a_node_ptr);
  }

  APR_DBG_2(APR_DBG_HIGH, "Leaving a_node_[%d] worker thread, result[0x%x]",
            a_node_ptr->guid, rc);

  /* Destroy event created locally */
  apr_event_destroy(a_node_ptr->pending_work_event);

  /* Signal De-init to continue */
  apr_event_signal(a_node_ptr->thread_status_event);

  return APR_EOK;
}

/*
  Generic a node Interrupt Service Routine callback

  @param [in] packet: Pointer to the incoming message (packet) to receive.
  @param [in] dispatch_data: This is the parameter available during a thread
                             ISR dispatcher. Expected parameter is the a_node
                             instance address for which the interrupt needs to
                             be serviced

  @return: None
*/
int32_t a_node_isr_dispatch_cb(aprv2_packet_t* packet, void* dispatch_data)
{
  int32_t rc;
  apr_list_t* used_q_ptr;
  apr_list_t* free_q_ptr;
  linked_packet_t* item_ptr;
  a_node_instance_t* a_node_ptr = NULL;
  size_t entry = NULL;
  a_node_map_t* local_a_nodes_ptr = (a_node_map_t*)dispatch_data;

  rc = a_node_map_get(local_a_nodes_ptr, packet->dst_port, &entry);
  if(rc || NULL == entry)
  {
    APR_DBG_2(APR_DBG_ERROR, "a_node_[%d] not found, rc=0x%x",
              packet->dst_port, rc);
    __aprv2_cmd_end_command(0, packet, APR_ENOSERVICE);
    return APR_EOK;
  }
  a_node_ptr = (a_node_instance_t*)entry;

//  Avoid logging in APR ISR context
//  APR_DBG_1(APR_DBG_LOW, "a_node_[%d] received message",
//            a_node_ptr->guid);

  /* Avoid adding additional system delays by processing incoming messages in
   * thread context only.
   */

  /* Separate command messages and response messages to different queues. The
   * client can always re-send commands when the command queue overflows.
   * However, response messages comes only once so it is imperative that they
   * are not lost due to queue overflows.
   */
  if(APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header ) ==
     APRV2_PKT_MSGTYPE_CMDRSP_V )
  {
    used_q_ptr = &a_node_ptr->used_rsp_q;
    free_q_ptr = &a_node_ptr->free_rsp_q;
  }
  else
  {
    used_q_ptr = &a_node_ptr->used_cmd_q;
    free_q_ptr = &a_node_ptr->free_cmd_q;
  }

  apr_lock_enter(a_node_ptr->lock);
  rc = apr_list_remove_head(free_q_ptr, ((apr_list_node_t**) &item_ptr));
  apr_lock_leave(a_node_ptr->lock);

  if(rc == APR_EOK)
  {
    item_ptr->packet = packet;
    apr_lock_enter(a_node_ptr->lock);
    apr_list_add_tail(used_q_ptr, &item_ptr->link);
    apr_lock_leave(a_node_ptr->lock);

    apr_event_signal(a_node_ptr->pending_work_event);
  }
  else
  {
    APR_DBG_1(APR_DBG_ERROR, "a_node_[%d] Ran out of free commands",
              a_node_ptr->guid);
    __aprv2_cmd_end_command(a_node_ptr->apr_handle, packet, APR_EBUSY);
  }

  return APR_EOK;
}


