/*
  Copyright (C) 2011 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/example/toyclient/src/toyclient.c#1 $
  $Author: pwbldsvc $
*/

#include "err.h"
#include "msg.h"
#include "task.h"
#include "apr_list.h"
#include "apr_objmgr.h"
#include "apr_lock.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "aprv2_api_inline.h"
#include "toyserver_msg_if.h"
#include "toyclient_msg_if.h"
#include "toyclient_api.h"
#include "apr_dbg_msg.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

#define TOYCLIENT_CMD_QUEUE_SIZE ( 10 )
#define TOYCLIENT_RSP_QUEUE_SIZE ( 10 )

#define TOYCLIENT_HANDLE_TOTAL_BITS ( 16 )
#define TOYCLIENT_HANDLE_INDEX_BITS ( 5 )
#define TOYCLIENT_NUM_HANDLES ( 1 << TOYCLIENT_HANDLE_INDEX_BITS )

#define TOYCLIENT_APR_TASK_PRIORITY ( 123 )  /* OEM should choose proper thread
                                                priority level. Recommend using
                                                enum define in task.h if it exists. */

/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

#define TOYCLIENT_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

typedef struct toyclient_work_item_t
{
  apr_list_node_t link;
  aprv2_packet_t* packet;
}
  toyclient_work_item_t;

typedef void ( *toyclient_rsp_handler_cb_t ) (
  uint16_t port,
  uint32_t token,
  uint32_t opcode,
  void* payload,
  uint32_t payload_size
);

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

static char_t toyclient_my_thread_name[] = "TOYCLIENT";
static char_t toyclient_my_thread_name_2[] = "TOYCLIENT_2";

static char_t toyclient_my_dns[] = "oem.toyclient";
static char_t toyclient_toyserver_dns[] = "oem.toyserver";

static apr_lock_t toyclient_lock;
static apr_event_t toyclient_control_event;

static apr_objmgr_t toyclient_objmgr;
static apr_objmgr_object_t toyclient_objects[ TOYCLIENT_NUM_HANDLES ];

static apr_list_t toyclient_used_rsp_q;
static apr_list_t toyclient_free_rsp_q;
static toyclient_work_item_t toyclient_rsps[ TOYCLIENT_RSP_QUEUE_SIZE ];

static apr_list_t toyclient_used_cmd_q;
static apr_list_t toyclient_free_cmd_q;
static toyclient_work_item_t toyclient_cmds[ TOYCLIENT_CMD_QUEUE_SIZE ];

static apr_event_t toyclient_work_event;
static apr_thread_t toyclient_thread;
static apr_thread_t toyclient_thread_2;
static apr_event_t toyclient_work_event_2;

static uint32_t toyclient_apr_handle;
static uint16_t toyclient_my_addr;

static apr_event_t toyclient_done_event;

/*****************************************************************************
 * Prototypes                                                                *
 ****************************************************************************/

static int32_t toyclient_run ( void );

/*****************************************************************************
 * Implementations                                                           *
 ****************************************************************************/

static void toyclient_isr_lock_fn ( void )
{
  apr_lock_enter( toyclient_lock );
}

static void toyclient_isr_unlock_fn ( void )
{
  apr_lock_leave( toyclient_lock );
}

static void toyclient_send_command (
  uint16_t to_addr,
  uint16_t to_port,
  uint32_t token,
  uint32_t opcode,
  void* payload,
  uint32_t payload_size,
  toyclient_rsp_handler_cb_t callback
)
{
  int32_t rc;
  apr_objmgr_object_t* obj;

  rc = apr_objmgr_alloc_object( &toyclient_objmgr, &obj );
  TOYCLIENT_PANIC_ON_ERROR( rc );

  obj->type = token;
  obj->any.ptr = callback;

  rc = __aprv2_cmd_alloc_send( toyclient_apr_handle,
                               APRV2_PKT_MSGTYPE_SEQCMD_V,
                               toyclient_my_addr, APR_NULL_V,
                               to_addr, to_port,
                               obj->handle,
                               opcode,
                               payload, payload_size );
  TOYCLIENT_PANIC_ON_ERROR( rc );
    /**< An error robust implementation would retry instead of panicking. */
}

static void toyclient_send_event (
  uint16_t to_addr,
  uint16_t to_port,
  uint32_t token,
  uint32_t opcode,
  void* payload,
  uint32_t payload_size
)
{
  int32_t rc;

  rc = __aprv2_cmd_alloc_send( toyclient_apr_handle,
                               APRV2_PKT_MSGTYPE_EVENT_V,
                               toyclient_my_addr, APR_NULL_V,
                               to_addr, to_port,
                               token,
                               opcode,
                               payload, payload_size );
  TOYCLIENT_PANIC_ON_ERROR( rc );
    /**< An error robust implementation would retry instead of panicking. */
}

static void toyclient_process_rsp_msg (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  apr_objmgr_object_t* obj;

  rc = apr_objmgr_find_object( &toyclient_objmgr, packet->token, &obj );
  TOYCLIENT_PANIC_ON_ERROR( rc );

  if ( obj->any.ptr != NULL )
  {
    ( ( toyclient_rsp_handler_cb_t ) obj->any.ptr )(
      packet->dst_port,
      obj->type,
      packet->opcode,
      APRV2_PKT_GET_PAYLOAD( void, packet ),
      APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header ) );
  }

  apr_objmgr_free_object( &toyclient_objmgr, obj->handle );
  __aprv2_cmd_free( toyclient_apr_handle, packet );
}

static void toyclient_process_work_queue (
  apr_list_t* used_q,
  apr_list_t* free_q
)
{
  int32_t rc;
  toyclient_work_item_t* item;
  aprv2_packet_t* packet;

  for ( ;; )
  {
    rc = apr_list_remove_head( used_q, ( ( apr_list_node_t** ) &item ) );
    if ( rc ) break;

    packet = item->packet;

    switch ( packet->opcode )
    {
    case APRV2_IBASIC_EVT_ACCEPTED:
      __aprv2_cmd_free( toyclient_apr_handle, packet );
      break;

    case APRV2_IBASIC_RSP_RESULT:
    case TOYSERVER_ITEST_RSP_ADD:
      toyclient_process_rsp_msg( packet );
    break;

    case TOYCLIENT_ITEST_CMD_START_TEST:
      APR_DBG_0(APR_DBG_HIGH, "Received test start command");
      apr_event_signal( toyclient_work_event_2 );
      __aprv2_cmd_end_command( toyclient_apr_handle, packet, APR_EOK );
    break;

  default:
    APR_DBG_0(APR_DBG_ERROR, "Received an unsupported command");
      __aprv2_cmd_end_command( toyclient_apr_handle, packet, APR_EUNSUPPORTED );
    break;
  }

    apr_list_add_tail( free_q, &item->link );
  }
}

static int32_t toyclient_worker_thread_fn (
  void* param
)
{
  int32_t rc;

  APR_DBG_0(APR_DBG_LOW, "Entering the worker thread");

  apr_event_create( &toyclient_work_event );
  apr_event_signal( toyclient_control_event );

  for ( ;; )
  {
    rc = apr_event_wait( toyclient_work_event );
    if ( rc ) break;

    APR_DBG_0(APR_DBG_LOW, "Waking up to do work");

    toyclient_process_work_queue( &toyclient_used_rsp_q, &toyclient_free_rsp_q );
    toyclient_process_work_queue( &toyclient_used_cmd_q, &toyclient_free_cmd_q );
  }

  APR_DBG_0(APR_DBG_LOW, "Leaving the worker thread");

  apr_event_destroy( toyclient_work_event );
  apr_event_signal( toyclient_control_event );

  return APR_EOK;
}

static int32_t toyclient_worker_thread_fn_2 (
  void* param
)
{
  int32_t rc;

  APR_DBG_0(APR_DBG_LOW, "Entering the worker thread 2");

  apr_event_create( &toyclient_work_event_2 );
  apr_event_signal( toyclient_control_event );

  for ( ;; )
  {
    rc = apr_event_wait( toyclient_work_event_2 );
    if ( rc ) break;

    APR_DBG_0(APR_DBG_LOW, "Waking up to do work");

    toyclient_run( );
  }

  APR_DBG_0(APR_DBG_LOW, "Leaving the worker thread 2");

  apr_event_destroy( toyclient_work_event_2 );
  apr_event_signal( toyclient_control_event );

  return APR_EOK;
}


static int32_t toyclient_isr_dispatch_cb (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  int32_t rc;
  apr_list_t* used_q;
  apr_list_t* free_q;
  toyclient_work_item_t* item;

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
    used_q = &toyclient_used_rsp_q;
    free_q = &toyclient_free_rsp_q;
  }
  else
  {
    used_q = &toyclient_used_cmd_q;
    free_q = &toyclient_free_cmd_q;
  }

  rc = apr_list_remove_head( free_q, ( ( apr_list_node_t** ) &item ) );
  if ( rc == APR_EOK )
  {
    //__aprv2_cmd_accept_command( toyclient_apr_handle, packet );
    /* Accept command */

    item->packet = packet;
    apr_list_add_tail( used_q, &item->link );
    apr_event_signal( toyclient_work_event );
  }
  else
  {
    APR_DBG_0(APR_DBG_ERROR, "Ran out of free commands");
    __aprv2_cmd_end_command( toyclient_apr_handle, packet, APR_EBUSY );
  }

  return APR_EOK;
}

static void toyclient_create_work_queues (
  apr_list_t* used_q,
  apr_list_t* free_q,
  toyclient_work_item_t* store,
  uint32_t store_size
)
{
  uint32_t index;

  apr_list_init( used_q, toyclient_isr_lock_fn, toyclient_isr_unlock_fn );
  apr_list_init( free_q, toyclient_isr_lock_fn, toyclient_isr_unlock_fn );

  for ( index = 0; index < store_size; ++index )
  {
    apr_list_add_tail( free_q, &store[ index ].link );
  }
}

static int32_t toyclient_init ( void )
{
  int32_t rc;

  { /* Initialize basic OS resources for staging the setup. */
    apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &toyclient_lock );
    apr_event_create( &toyclient_control_event );
  }
  { /* Initialize the handle table. */
    apr_objmgr_setup_params_t setup_args;

    setup_args.table = toyclient_objects;
    setup_args.total_bits = TOYCLIENT_HANDLE_TOTAL_BITS;
    setup_args.index_bits = TOYCLIENT_HANDLE_INDEX_BITS;
    setup_args.lock_fn = toyclient_isr_lock_fn;
    setup_args.unlock_fn = toyclient_isr_unlock_fn;

    apr_objmgr_construct( &toyclient_objmgr, &setup_args );
  }
  { /* Initialize work queues. */
    toyclient_create_work_queues( &toyclient_used_rsp_q, &toyclient_free_rsp_q,
                                  toyclient_rsps, TOYCLIENT_RSP_QUEUE_SIZE );
    toyclient_create_work_queues( &toyclient_used_cmd_q, &toyclient_free_cmd_q,
                                  toyclient_cmds, TOYCLIENT_CMD_QUEUE_SIZE );
  }
  { /* Create a worker thread for processing incoming commands. */
    apr_thread_create( &toyclient_thread, toyclient_my_thread_name,
                                TOYCLIENT_APR_TASK_PRIORITY, NULL, 0,
                       toyclient_worker_thread_fn, NULL );
    apr_event_wait( toyclient_control_event );
  }
  { /* Create a worker thread for running test. */
    apr_thread_create( &toyclient_thread_2, toyclient_my_thread_name_2,
                       TOYCLIENT_APR_TASK_PRIORITY, NULL, 0,
                       toyclient_worker_thread_fn_2, NULL );
    apr_event_wait( toyclient_control_event );
  }
  { /* Register with APR to receive messages. */
    rc = __aprv2_cmd_register2( &toyclient_apr_handle, toyclient_my_dns,
                                sizeof( toyclient_my_dns ), 0,
                                toyclient_isr_dispatch_cb, NULL,
                                &toyclient_my_addr );
    TOYCLIENT_PANIC_ON_ERROR( rc );
  }

  return APR_EOK;
}

static int32_t toyclient_postinit ( void )
{
  return APR_EOK;
}

static int32_t toyclient_predeinit ( void )
{
  return APR_EOK;
}

static int32_t toyclient_deinit ( void )
{
  /* Deregister from APR to stop receiving messages. */
  __aprv2_cmd_deregister( toyclient_apr_handle );

  /* Destroy the test thread. */
  apr_event_signal_abortall( toyclient_work_event_2 );
  apr_event_wait( toyclient_control_event );

  /* Destroy the worker thread for processing incoming commands. */
  apr_event_signal_abortall( toyclient_work_event );
  apr_event_wait( toyclient_control_event );

  /* Deinitialize work queues. */
  apr_list_destroy( &toyclient_free_cmd_q );
  apr_list_destroy( &toyclient_used_cmd_q );
  apr_list_destroy( &toyclient_free_rsp_q );
  apr_list_destroy( &toyclient_used_rsp_q );

  /* Deinitialize the handle table. */
  apr_objmgr_destruct( &toyclient_objmgr );

  /* Deinitialize basic OS resources for staging the setup. */
  apr_event_destroy( toyclient_control_event );
  apr_lock_destroy( toyclient_lock );

  return APR_EOK;
}

static void toyclient_custom_add_rsp_handler_cb (
  uint16_t port,
  uint32_t token,
  uint32_t opcode,
  void* payload,
  uint32_t payload_size
)
{
  toyserver_itest_rsp_add_t* add_rsp;

  APR_DBG_0(APR_DBG_LOW, "Processing the add response");

  for ( ;; )
  {
    /* Verify that the opcode is expected. */
    if ( opcode != TOYSERVER_ITEST_RSP_ADD )
    {
      APR_DBG_0(APR_DBG_ERROR, "Unexpected command response type");
      break;
    }

    /* Verify that the payload size is expected. */
    if ( payload_size != sizeof( toyserver_itest_rsp_add_t ) )
    {
      APR_DBG_0(APR_DBG_ERROR, "Unexpected command response payload size");
      break;
    }

    /* Show the value of the sum. */
    add_rsp = ( ( toyserver_itest_rsp_add_t* ) payload );
    APR_DBG_3(APR_DBG_HIGH, "port = %04X, token = 0x%08X, sum = %d", port, token, add_rsp->sum );

    break;
  }

  apr_event_signal( toyclient_done_event );
}

static int32_t toyclient_run ( void )
{
  int32_t rc;
  uint16_t toyserver_addr;
  toyserver_itest_cmd_add_t add_cmd;

  /* Send a message from the toyclient to the toyserver. In this example the
   * asychronous command is turned into a synchronous sequence by blocking on
   * the done event which will be signalled by the custom command response
   * handler.
   */

  rc = __aprv2_cmd_local_dns_lookup( toyclient_toyserver_dns,
                                     sizeof( toyclient_toyserver_dns ),
                                     &toyserver_addr );
  TOYCLIENT_PANIC_ON_ERROR( rc );

  apr_event_create( &toyclient_done_event );

  add_cmd.a = 11111111;
  add_cmd.b = 22222222;

  APR_DBG_0(APR_DBG_HIGH, "Send add command to server");
  toyclient_send_command( toyserver_addr, APR_NULL_V,
                          0x12345678,
                          TOYSERVER_ITEST_CMD_ADD,
                          &add_cmd, sizeof( add_cmd ),
                          toyclient_custom_add_rsp_handler_cb );
  apr_event_wait( toyclient_done_event );

  apr_event_destroy( toyclient_done_event );

  return APR_EOK;
}

APR_EXTERNAL int32_t toyclient_call (
  toyclient_callindex_enum_t index,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  switch ( index )
  {
  case TOYCLIENT_CALLINDEX_ENUM_INIT:
    rc = toyclient_init( );
    break;

  case TOYCLIENT_CALLINDEX_ENUM_POSTINIT:
    rc = toyclient_postinit( );
    break;

  case TOYCLIENT_CALLINDEX_ENUM_PREDEINIT:
    rc = toyclient_predeinit( );
    break;

  case TOYCLIENT_CALLINDEX_ENUM_DEINIT:
    rc = toyclient_deinit( );
    break;

  case TOYCLIENT_CALLINDEX_ENUM_RUN:
    rc = toyclient_run( );
    break;

  default:
    APR_DBG_1(APR_DBG_ERROR, "Unsupported callindex (%d)", index);
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

