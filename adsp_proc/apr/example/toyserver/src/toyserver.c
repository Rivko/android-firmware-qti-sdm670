/*
  Copyright (C) 2011 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/example/toyserver/src/toyserver.c#1 $
  $Author: pwbldsvc $
*/

#include "err.h"
#include "msg.h"
#include "task.h"
#ifdef FEATURE_CENTRALIZED_THREAD_PRIORITY
#include "rcinit.h"
#endif
#include "apr_list.h"
#include "apr_lock.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "aprv2_api_inline.h"
#include "toyserver_api.h"
#include "toyserver_msg_if.h"
#include "apr_dbg_msg.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

#define TOYSERVER_CMD_QUEUE_SIZE ( 10 )
#define TOYSERVER_RSP_QUEUE_SIZE ( 10 )

/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

#define TOYSERVER_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

#define TOYSERVER_APR_TASK_PRIORITY ( 165 )  
/**< OEM should choose proper thread priority level. 
     Recommend using enum define in task.h if it exists. */

typedef struct toyserver_work_item_t
{
  apr_list_node_t link;
  aprv2_packet_t* packet;
}
  toyserver_work_item_t;

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

static apr_lock_t toyserver_lock;
static apr_event_t toyserver_control_event;

static apr_list_t toyserver_used_rsp_q;
static apr_list_t toyserver_free_rsp_q;
static toyserver_work_item_t toyserver_rsps[ TOYSERVER_RSP_QUEUE_SIZE ];

static apr_list_t toyserver_free_cmd_q;
static apr_list_t toyserver_used_cmd_q;
static toyserver_work_item_t toyserver_cmds[ TOYSERVER_CMD_QUEUE_SIZE ];

static char_t toyserver_my_thread_name[] = "TOYSERVER";
static apr_thread_t toyserver_thread;
static apr_event_t toyserver_work_event;

static char_t toyserver_my_dns[] = "oem.toyserver";

static uint32_t toyserver_apr_handle;
static uint16_t toyserver_my_addr;

/*****************************************************************************
 * Implementations                                                           *
 ****************************************************************************/

static void toyserver_isr_lock_fn ( void )
{
  apr_lock_enter( toyserver_lock );
}

static void toyserver_isr_unlock_fn ( void )
{
  apr_lock_leave( toyserver_lock );
}

static void toyserver_process_add_cmd (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  toyserver_itest_cmd_add_t* add_cmd;
  toyserver_itest_rsp_add_t add_rsp;

  APR_DBG_0(APR_DBG_LOW, "Processing the add command");

  /* Verify that the payload size is expected. */
  if ( APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header ) !=
       sizeof( toyserver_itest_cmd_add_t ) )
  {
    __aprv2_cmd_end_command( toyserver_apr_handle, packet, APR_EBADPARAM );
    return;
  }

  /* Reply back to the client the sum of the add operation. */
  add_cmd = APRV2_PKT_GET_PAYLOAD( toyserver_itest_cmd_add_t, packet );

  add_rsp.sum = ( add_cmd->a + add_cmd->b );

  rc = __aprv2_cmd_alloc_send( toyserver_apr_handle,
                               APRV2_PKT_MSGTYPE_CMDRSP_V,
                               packet->dst_addr, packet->dst_port,
                               packet->src_addr, packet->src_port,
                               packet->token,
                               TOYSERVER_ITEST_RSP_ADD,
                               &add_rsp, sizeof( add_rsp ) );
  TOYSERVER_PANIC_ON_ERROR( rc );

  __aprv2_cmd_free( toyserver_apr_handle, packet );
}

static void toyserver_process_work_queue (
  apr_list_t* used_q,
  apr_list_t* free_q
)
{
  int32_t rc;
  toyserver_work_item_t* item;
  aprv2_packet_t* packet;

  for ( ;; )
  {
    rc = apr_list_remove_head( used_q, ( ( apr_list_node_t** ) &item ) );
    if ( rc ) break;

    packet = item->packet;

    switch ( packet->opcode )
    {
    case TOYSERVER_ITEST_CMD_ADD:
      toyserver_process_add_cmd( packet );
      break;

    default:
      APR_DBG_0(APR_DBG_ERROR, "Received an unsupported command");
      __aprv2_cmd_end_command( toyserver_apr_handle, packet, APR_EUNSUPPORTED );
      break;
    }

    apr_list_add_tail( free_q, &item->link );
  }
}

static int32_t toyserver_worker_fn (
  void* param
)
{
  int32_t rc;

  APR_DBG_0(APR_DBG_LOW, "Entering the worker thread");

  apr_event_create( &toyserver_work_event );
  apr_event_signal( toyserver_control_event );

  for ( ;; )
  {
    rc = apr_event_wait( toyserver_work_event );
    if ( rc ) break;

    APR_DBG_0(APR_DBG_LOW, "Waking up to do work");

    toyserver_process_work_queue( &toyserver_used_rsp_q, &toyserver_free_rsp_q );
    toyserver_process_work_queue( &toyserver_used_cmd_q, &toyserver_free_cmd_q );
  }

  APR_DBG_0(APR_DBG_LOW, "Leaving the worker thread");

  apr_event_destroy( toyserver_work_event );
  apr_event_signal( toyserver_control_event );

  return APR_EOK;
}

static int32_t toyserver_isr_dispatch_cb (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  int32_t rc;
  apr_list_t* used_q;
  apr_list_t* free_q;
  toyserver_work_item_t* item;

  APR_DBG_0(APR_DBG_LOW, "Received a message");

  /* Avoid adding additional system delays by processing incoming messages in
   * thread context only.
   */

  /* Separate command messages and response messages to different queues. The
   * client can always re-send commands when the command queue overflows.
   * However, response messages comes only once so it is imperative that they
   * are not lost due to queue overflows.
   */
  if ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header ) == APRV2_PKT_MSGTYPE_CMDRSP_V )
  {
    used_q = &toyserver_used_rsp_q;
    free_q = &toyserver_free_rsp_q;
  }
  else
  {
    used_q = &toyserver_used_cmd_q;
    free_q = &toyserver_free_cmd_q;
  }

  rc = apr_list_remove_head( free_q, ( ( apr_list_node_t** ) &item ) );
  if ( rc == APR_EOK )
  {
    __aprv2_cmd_accept_command( toyserver_apr_handle, packet );
    item->packet = packet;
    apr_list_add_tail( used_q, &item->link );
    apr_event_signal( toyserver_work_event );
  }
  else
  {
    APR_DBG_0(APR_DBG_ERROR, "Ran out of free commands");
    __aprv2_cmd_end_command( toyserver_apr_handle, packet, APR_EBUSY );
  }

  return APR_EOK;
}

static void toyserver_create_work_queues (
  apr_list_t* used_q,
  apr_list_t* free_q,
  toyserver_work_item_t* store,
  uint32_t store_size
)
{
  uint32_t index;

  apr_list_init( used_q, toyserver_isr_lock_fn, toyserver_isr_unlock_fn );
  apr_list_init( free_q, toyserver_isr_lock_fn, toyserver_isr_unlock_fn );

  for ( index = 0; index < store_size; ++index )
  {
    apr_list_add_tail( free_q, &store[ index ].link );
  }
}

static int32_t toyserver_init ( void )
{
  int32_t rc;
  uint32_t prio = 0;

#ifdef FEATURE_CENTRALIZED_THREAD_PRIORITY
  { /* Lookup thread info */
   RCINIT_INFO info_handle = NULL;
   RCINIT_PRIO rcinit_prio = 0;

   info_handle = rcinit_lookup(toyserver_my_thread_name);

   if (!info_handle) 
   {
      ERR_FATAL("TOYSERVER task info not found", 0, 0, 0);
   }
   else
   {
      rcinit_prio = rcinit_lookup_prio_info(info_handle);

      if (rcinit_prio > 255)
      {
        ERR_FATAL("Invalid Priority:%d", rcinit_prio, 0, 0);
      }

      /*
       * Normalizing priority to apr_thread_priority (0=Highest and 255=Lowest)
       * as priorities obtained from rcinit (rcinit_task_prio.csv) follow the
       * opposite i.e. 0=Lowest and 255=Highest.
      */
      prio = 255 - (uint32_t) rcinit_prio;

      /* Omit stack size lookup - APR_THREAD_STACK_SIZE sized stack created on the fly */
   }
  }
#else
  prio = TOYSERVER_APR_TASK_PRIORITY;
#endif

  { /* Initialize basic OS resources for staging the setup. */
    apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &toyserver_lock );
    apr_event_create( &toyserver_control_event );
  }
  { /* Initialize command queues. */
    toyserver_create_work_queues( &toyserver_used_rsp_q, &toyserver_free_rsp_q,
                                  toyserver_rsps, TOYSERVER_RSP_QUEUE_SIZE );
    toyserver_create_work_queues( &toyserver_used_cmd_q, &toyserver_free_cmd_q,
                                  toyserver_cmds, TOYSERVER_CMD_QUEUE_SIZE );
  }
  { /* Create a worker thread for processing incoming commands. */
    apr_thread_create( &toyserver_thread, toyserver_my_thread_name,
                                prio, NULL, 0,
                                toyserver_worker_fn, NULL );
    apr_event_wait( toyserver_control_event );
  }
  { /* Register with APR to receive messages. */
    rc = __aprv2_cmd_register2( &toyserver_apr_handle, toyserver_my_dns,
                                sizeof( toyserver_my_dns ), 0,
                                toyserver_isr_dispatch_cb, NULL,
                                &toyserver_my_addr );
    TOYSERVER_PANIC_ON_ERROR( rc );
  }

  return APR_EOK;
}

static int32_t toyserver_postinit ( void )
{
  return APR_EOK;
}

static int32_t toyserver_predeinit ( void )
{
  return APR_EOK;
}

static int32_t toyserver_deinit ( void )
{
  /* Deregister from APR to stop receiving messages. */
  __aprv2_cmd_deregister( toyserver_apr_handle );

  /* Destroy the worker thread for processing incoming commands. */
  apr_event_signal_abortall( toyserver_work_event );
  apr_event_wait( toyserver_control_event );

  /* Deinitialize command queues. */
  apr_list_destroy( &toyserver_free_cmd_q );
  apr_list_destroy( &toyserver_used_cmd_q );
  apr_list_destroy( &toyserver_free_rsp_q );
  apr_list_destroy( &toyserver_used_rsp_q );

  /* Deinitialize basic OS resources for staging the setup. */
  apr_event_destroy( toyserver_control_event );
  apr_lock_destroy( toyserver_lock );

  return APR_EOK;
}

APR_EXTERNAL int32_t toyserver_call (
  toyserver_callindex_enum_t index,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  switch ( index )
  {
  case TOYSERVER_CALLINDEX_ENUM_INIT:
    rc = toyserver_init( );
    break;

  case TOYSERVER_CALLINDEX_ENUM_POSTINIT:
    rc = toyserver_postinit( );
    break;

  case TOYSERVER_CALLINDEX_ENUM_PREDEINIT:
    rc = toyserver_predeinit( );
    break;

  case TOYSERVER_CALLINDEX_ENUM_DEINIT:
    rc = toyserver_deinit( );
    break;

  default:
    APR_DBG_1(APR_DBG_ERROR, "Unsupported callindex (%d)", index);
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

