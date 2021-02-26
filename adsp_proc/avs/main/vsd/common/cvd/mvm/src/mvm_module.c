/*
  Copyright (c) 2009-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/mvm/src/mvm_module.c#4 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include <stddef.h>
#include "msg.h"
#include "err.h"

#ifndef WINSIM
#include "sys_m_messages.h"
#include "rcecb.h"
#endif /* WINSIM */

#include "mmstd.h"

#include "apr_errcodes.h"
#include "apr_list.h"
#include "apr_memmgr.h"
#include "apr_objmgr.h"
#include "apr_lock.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "apr_misc.h"
#include "apr_log.h"
#include "aprv2_ids_domains.h"
#include "aprv2_api_inline.h"
#include "aprv2_msg_if.h"

#include "Elite.h"
#include "qurt_elite.h"
#include "VoiceTimerMsgs.h"

#include "adsp_vcmn_api.h"
#include "adsp_vparams_api.h"
#include "adsp_vpm_api.h"

#include "vocsvc_avtimer_api.h"
#include "vss_public_if.h"
#include "vss_private_if.h"
#include "cvd_task.h"
#include "vss_common_public_if.h"
#include "cvd_utils_common_i.h"

#include "mvm_ccm_api_i.h"
#include "vccm_api.h"

#include "mvm_api_i.h"
#include "mvm_i.h"

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

static char_t mvm_my_dns[] = "qcom.audio.mvm";
static uint16_t mvm_my_addr;
  /**< MVM address is set at initialization. */

static char_t mvm_cvs_dns[] = "qcom.audio.cvs";
static uint16_t mvm_cvs_addr;
  /**< CVS address is set at initialization. */

static char_t mvm_vpm_dns[] = "qcom.audio.cvp";
static uint16_t mvm_vpm_addr;
  /**< VPM address is set at initialization. */

static char_t mvm_vsm_dns[] = "qcom.audio.cvs";
static uint16_t mvm_vsm_addr;
  /**< VSM address is set at initialization. */
static char_t mvm_cvd_vfr_dns[] = "qcom.audio.cvdvfr";
static uint16_t mvm_cvd_vfr_addr;
  /**< CVD VFR address is set at initialization. */

static char_t mvm_version[] = "2.3";
  /**<
   * The current MVM (CVD) version string. The format of the string is x.y,
   * where x is the major (architecture) version and y is the minor
   * (feature-set) version. This version string will be returned to the client
   * in response to the VSS_IVERSION_CMD_GET command.
   *
   * Version Information - New feature support
   * Version 2.2: Voice custom topology support & custom topology clock control.
   */

static uint32_t mvm_debug_call_num = 0;
/**< To track the number of call made. */

static bool_t mvm_csm_vote = FALSE;
/**< To track the MVM to CSM. */

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/

/* Lock Management */
static apr_lock_t mvm_int_lock;
static apr_lock_t mvm_task_lock;

/* Heap Management */
static uint8_t mvm_heap_pool[ MVM_HEAP_SIZE_V ];
static apr_memmgr_type mvm_heapmgr;

/* Object Management */
static apr_objmgr_object_t mvm_object_table[ MVM_MAX_OBJECTS_V ];
static apr_objmgr_t mvm_objmgr;

/* Command Queue Management */
static mvm_work_item_t mvm_cmd_pool[ MVM_NUM_COMMANDS_V ];
static apr_list_t mvm_free_cmd_q;
static apr_list_t mvm_task_incoming_cmd_q;

/* Task Management */
static apr_event_t mvm_thread_event;
/* For medium priority task. */
static apr_thread_t mvm_task_handle;
static apr_event_t mvm_task_event;
static uint8_t mvm_med_task_stack[ MVM_MED_TASK_STACK_SIZE ];

/* For creating and destroying tasks. */
static mvm_thread_state_enum_t mvm_task_state = MVM_THREAD_STATE_ENUM_INIT;

/* Session Management */
/* For medium priority task. */
static mvm_pending_control_t mvm_task_pending_ctrl;

/* Subsystem Restart (SSR) Management */
static mvm_pending_control_t mvm_ssr_pending_ctrl;
static mvm_ssr_cleanup_cmd_tracking_t mvm_ssr_cleanup_cmd_tracking;
static mvm_ssr_mem_handle_tracking_t mvm_ssr_mem_handle_tracking;

/* APR Resource */
static uint32_t mvm_apr_handle;

/* Session tracking */
static mvm_generic_item_t mvm_session_list_pool[ MVM_MAX_SESSIONS ];
static apr_list_t mvm_session_list_free_q;
static apr_list_t mvm_session_q;

/* Global client info who listen for voice acitity update events.
   This is a service level support to broadcast voice activity events
   during a voice call. */
static mvm_voice_activity_client_info_t voice_activity_client = { FALSE, APR_NULL_V,
                                                                  APR_NULL_V };
static bool_t mvm_csm_status = TRUE;

char_t *mvm_state_table[MVM_STATE_ENUM_MAX] = 
{
  "UNINITIALIZED    ",
  "RESET_ENTRY      ",
  "RESET            ",
  "INIT_ENTRY       ",
  "INIT             ",
  "VOCPROC_RUN_ENTRY",
  "VOCPROC_RUN      ",
  "STREAM_RUN_ENTRY ",
  "STREAM_RUN       ",
  "RUN_ENTRY        ",
  "RUN              ",
  "INVALID          "
};

char_t *mvm_goal_table[MVM_GOAL_ENUM_MAX] = 
{
  "UNINITIALIZED   ",
  "NONE            ",
  "CREATE          ",
  "DESTROY         ",
  "VOCPROC_DISABLE ",
  "VOCPROC_RECONFIG",
  "VOCPROC_ENABLE  ",
  "STREAM_DISABLE  ",
  "STREAM_RECONFIG ",
  "STREAM_ENABLE   ",
  "INVALID         "
};

char_t *mvm_action_table[MVM_ACTION_ENUM_MAX] = 
{
  "UNINITIALIZED",
  "NONE",
  "COMPLETE",
  "CONTINUE",

  "VOCPROC_REINIT",
  "VOCPROC_LOAD_COUNT_RESET",
  "VOCPROC_SET_SYSTEM_CONFIG",
  "VOCPROC_SET_TIMING_OFFSET",
  "VOCPROC_ENABLE",
  "VOCPROC_DISABLE",
  "VOCPROC_GET_HDVOICE",
  "VOCPROC_SET_HDVOICE",
  "VOCPROC_GET_AVSYNC_DELAYS",

  "STREAM_REINIT",
  "STREAM_LOAD_COUNT_RESET",
  "STREAM_SET_SYSTEM_CONFIG",
  "STREAM_SET_TIMING_OFFSET",
  "STREAM_SET_AVSYNC_DELAYS",
  "STREAM_DISABLE",
  "STREAM_ENABLE",
  "STREAM_SET_HDVOICE",
  "STREAM_SET_TTY_MODE",
  "STREAM_ENCODER_RESET",

  "EVALUATE_TIMING_PARAMS",
  "CALCULATE_TIMING_OFFSETS",
  "CALCULATE_AVSYNC_DELAYS",
  "DETACH_VOCPROCS_FROM_STREAMS",
  "CCM_KPPS_UPDATE",
  "CSM_CONFIG_UPDATE",

  "INVALID"
};

/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

static int32_t mvm_free_object (
  mvm_object_t* object
);

static bool_t mvm_is_voc_op_mode_valid (
  uint32_t voc_op_mode
);

static int32_t mvm_action_open_avtimer ( 
  mvm_session_object_t* session_obj 
);

static int32_t mvm_action_close_avtimer ( 
  mvm_session_object_t* session_obj 
);

static int32_t mvm_action_reconfig_stream (
  mvm_session_object_t* session_obj
);

static int32_t mvm_action_reconfig_vocproc (
  mvm_session_object_t* session_obj
);

static int32_t mvm_action_set_dymamic_system_config (
  mvm_session_object_t* session_obj
);

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

static void mvm_int_lock_fn ( void )
{
  ( void ) apr_lock_enter( mvm_int_lock );
}

static void mvm_int_unlock_fn ( void )
{
  ( void ) apr_lock_leave( mvm_int_lock );
}

static void mvm_med_task_lock_fn ( void )
{
  ( void ) apr_lock_enter( mvm_task_lock );
}

static void mvm_med_task_unlock_fn ( void )
{
  ( void ) apr_lock_leave( mvm_task_lock );
}

static void mvm_signal_run ( void )
{
  /* Trigger the session state control to run. */
  ( void ) apr_event_signal( mvm_task_event );
  return;
}

static int32_t mvm_get_object
(
  uint32_t handle,
  mvm_object_t** ret_obj
)
{
  int32_t rc;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = apr_objmgr_find_object( &mvm_objmgr, handle, &objmgr_obj );
  if ( rc )
  {
    return APR_EFAILED;
  }

  *ret_obj = ( ( mvm_object_t* ) objmgr_obj->any.ptr );

  return APR_EOK;
}

static int32_t mvm_typecast_object (
  apr_objmgr_object_t* store,
  mvm_object_type_enum_t type,
  mvm_object_t** ret_obj
)
{
  mvm_object_t* obj;

  if ( ( store == NULL ) || ( ret_obj == NULL ) )
  {
    return APR_EBADPARAM;
  }

  obj = ( ( mvm_object_t* ) store->any.ptr );

  if ( ( obj == NULL ) || ( obj->header.type != type ) )
  {
    return APR_EFAILED;
  }

  *ret_obj = obj;

  return APR_EOK;
}

static int32_t mvm_get_typed_object (
  uint32_t handle,
  mvm_object_type_enum_t type,
  mvm_object_t** ret_obj
)
{
  int32_t rc;
  apr_objmgr_object_t* store;

  if ( NULL == handle ) 
  {
     return APR_EBADPARAM;
  }
  rc = apr_objmgr_find_object( &mvm_objmgr, handle, &store );
  if ( rc ) return rc;

  rc = mvm_typecast_object( store, type, ret_obj );
  if ( rc ) return rc;

  return APR_EOK;
}

/****************************************************************************
 * PENDING COMMAND ROUTINES                                                 *
 ****************************************************************************/

static int32_t mvm_pending_control_init (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc;

  if ( ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = apr_list_init_v2( &ctrl->cmd_q, NULL, NULL );
  if ( rc )
  {
    return APR_EFAILED;
  }

  ctrl->state = MVM_PENDING_CMD_STATE_ENUM_FETCH;
  ctrl->packet = NULL;
  ctrl->pendjob_obj = NULL;

  return APR_EOK;
}

static int32_t mvm_pending_control_destroy (
  mvm_pending_control_t* ctrl
)
{
#if 0 /* There's nothing to destroy if everything is in a good state. */
  int32_t rc;
#endif /* 0 */

  if ( ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

#if 0 /* There's nothing to destroy if everything is in a good state. */
  rc = apr_list_destroy( &ctrl->cmd_q );
  if ( rc )
  {
    return APR_EFAILED;
  }

  ctrl->state = MVM_PENDING_CMD_STATE_ENUM_UNINITIALIZED;
  ctrl->packet = NULL;
  ctrl->pendjob_obj = NULL;
#endif /* 0 */

  return APR_EOK;
}

/****************************************************************************
 * CVD WORK QUEUE ROUTINES                                                  *
 ****************************************************************************/

static void mvm_queue_pending_packet (
  apr_list_t* pending_cmd_q,
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_work_item_t* work_item;
  uint16_t client_addr;

  if ( pending_cmd_q == NULL )
  {
    return;
  }

  if ( packet == NULL )
  { /* We should assert that the packet can't be NULL. */
    return;
  }

  for ( ;; )
  {
    { /* Get a free command structure. */
      rc = apr_list_remove_head( &mvm_free_cmd_q,
                                 ( ( apr_list_node_t** ) &work_item ) );
      if ( rc )
      { /* No free command structure is available. */
        rc = APR_EBUSY;
        break;
      }
    }

    { /* Queue the incoming command to the pending command queue. We don't
       * need to signal do work because after the incoming command
       * handler is done the pending command handler routine will be
       * called. */
      work_item->packet = packet;
      ( void ) apr_list_add_tail( pending_cmd_q, &work_item->link );
    }

    return;
  }

  { /* Try reporting the error. */
    client_addr = packet->src_addr;

    rc = __aprv2_cmd_end_command( mvm_apr_handle, packet, rc );
    MVM_COMM_ERROR( rc, client_addr );
  }
}

static void mvm_queue_incoming_packet (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_work_item_t* work_item;
  apr_list_t* incoming_cmd_q;
  uint16_t client_addr;

  if ( packet == NULL )
  { /* We should assert that the packet can't be NULL. */
    return;
  }

  incoming_cmd_q = &mvm_task_incoming_cmd_q;
  
  for ( ;; )
  {
    { /* Get a free command structure. */
      rc = apr_list_remove_head( &mvm_free_cmd_q,
                                 ( ( apr_list_node_t** ) &work_item ) );
      if ( rc )
      { /* No free command structure is available. */
        rc = APR_EBUSY;
        break;
      }
    }

    { /* Report command acceptance when requested. */
      rc = __aprv2_cmd_accept_command( mvm_apr_handle, packet );
      if ( rc )
      { /* Can't report so abort the command. */
        ( void ) apr_list_add_tail( &mvm_free_cmd_q, &work_item->link );
        break;
      }
    }

    { /* Queue the new command to the incoming command queue and signal do
       * work. */
      work_item->packet = packet;
      ( void ) apr_list_add_tail( incoming_cmd_q, &work_item->link );

      mvm_signal_run();
    }

    return;
  }

  { /* Try reporting the error. */
    client_addr = packet->src_addr;

    rc = __aprv2_cmd_end_command( mvm_apr_handle, packet, rc );
    MVM_COMM_ERROR( rc, client_addr );
  }
}

/****************************************************************************
 * MVM DEFAULT RESPONSE HANDLER ROUTINES                                    *
 ****************************************************************************/

static void mvm_default_event_rsp_fn (
  aprv2_packet_t* packet
)
{
  /* The default event handler just drops the packet. A specific event
   * handler routine should be written to do something more useful.
   */
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

static void mvm_set_default_response_table (
  mvm_response_fn_table_t table
)
{
  int i;

  if ( table == NULL )
  {
    return;
  }

  /* Initialize the state response handler function table. */
  for ( i = 0; i < MVM_RESPONSE_FN_ENUM_MAX; ++i )
  {
    table[ i ] = mvm_default_event_rsp_fn;
  }
}

static void mvm_simple_result_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_simple_job_object_t* obj;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &obj ) );
  if ( rc == APR_EOK )
  {
    obj->is_completed = TRUE;
    obj->status = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet )->status;

    /* If completed command failed, log the error. */
    if ( obj->status != APR_EOK )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mvm_simple_result_rsp_fn(): Command 0x%08X failed with result 0x%08X",
             APRV2_PKT_GET_PAYLOAD(aprv2_ibasic_rsp_result_t, packet)->opcode,
             obj->status );
    }
  }

  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

/* Destroy the simple job object once done. This is primarily useful for
 * analyzing/debugging the flow of jobs in logs.
 */
static void mvm_simple_self_destruct_result_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_simple_job_object_t* simple_job;
  uint32_t status;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &simple_job ) );
  if ( rc == APR_EOK )
  {
    /* This rsp function should NOT be used for commands returning anything
     * other than APRV2_IBASIC_RSP_RESULT (i.e. custom responses). However,
     * for robustness' sake, we do this check before interpreting the payload.
     */
    if ( packet->opcode == APRV2_IBASIC_RSP_RESULT )
    {
      status = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet )->status;

      /* If completed command failed, log the error. */
      if ( status != APR_EOK )
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "MVM: SIMPLE_SELF_DESTRUCT: OPCODE:0x%08X STATUS:0x%08X",
               APRV2_PKT_GET_PAYLOAD(aprv2_ibasic_rsp_result_t, packet)->opcode,
               status );
      }
    }

    ( void ) mvm_free_object( ( mvm_object_t* ) simple_job );
  }

  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

/* Response function to count the responses for the commands issued by MVM to
 * itself in each of the cleanup sequencer state, when doing cleanup due to the
 * subsystem where MVM clients reside is being restarted.
 */
static void mvm_ssr_cleanup_cmd_result_count_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  uint32_t command;
  uint32_t status;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &job_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  /* If the last operation failed, log the error. */
  command = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet )->opcode;
  status = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet )->status;

  if ( status != APR_EOK )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_ssr_cleanup_cmd_result_count_rsp_fn(): Command 0x%08X failed "
           "with 0x%08X", command, status );
  }

  /* Count the number of command responses received. */
  mvm_ssr_cleanup_cmd_tracking.rsp_cnt++;

  ( void ) mvm_free_object( ( mvm_object_t* ) job_obj );
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

/****************************************************************************
 * CVD OBJECT CREATION AND DESTRUCTION ROUTINES                             *
 ****************************************************************************/

static int32_t mvm_mem_alloc_object (
  uint32_t size,
  mvm_object_t** ret_object
)
{
  int32_t rc;
  mvm_object_t* ctrl;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_object == NULL )
  {
    return APR_EBADPARAM;
  }

  { /* Allocate memory for the new CVD object. */
    ctrl = apr_memmgr_malloc( &mvm_heapmgr, size );
    if ( ctrl == NULL )
    {
      return APR_ENORESOURCE;
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "mvm_mem_alloc_object(): ObjectsLeft=(%d)", mvm_objmgr.free_q.size );

    /* Allocate a new handle for the CVD object. */
    rc = apr_objmgr_alloc_object( &mvm_objmgr, &objmgr_obj );
    if ( rc )
    {
      apr_memmgr_free( &mvm_heapmgr, ctrl );
      return APR_ENORESOURCE;
    }

    /* Link the CVD object to the handle. */
    objmgr_obj->any.ptr = ctrl;

    ( void ) mmstd_memset( ctrl, 0xFD, size );

    /* Initialize the base CVD object header. */
    ctrl->header.handle = objmgr_obj->handle;
    ctrl->header.type = MVM_OBJECT_TYPE_ENUM_UNINITIALIZED;
  }

  *ret_object = ctrl;

  return APR_EOK;
}

static int32_t mvm_mem_free_object (
  mvm_object_t* object
)
{
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &mvm_objmgr, object->header.handle );
  apr_memmgr_free( &mvm_heapmgr, object );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "mvm_mem_free_object(): ObjectsLeft=(%d)", mvm_objmgr.free_q.size );

  return APR_EOK;
}

static int32_t mvm_create_session_object (
  mvm_session_object_t** ret_session_obj
)
{
  int32_t rc;
  mvm_session_object_t* session_obj;
  uint32_t i;

  if ( ret_session_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = mvm_mem_alloc_object( sizeof( mvm_session_object_t ),
                             ( ( mvm_object_t** ) &session_obj ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  { /* Initialize the session object. */
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &session_obj->lock );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "MVM: CREATE_SESSION: Failed to create lock, rc = (0x%08x)", rc );
      ( void ) mvm_free_object( ( mvm_object_t* )session_obj );
      return APR_ENORESOURCE;
    }

    session_obj->header.type = MVM_OBJECT_TYPE_ENUM_SESSION;

    ( void ) mmstd_memset( session_obj->generic_pool, 0,
                           sizeof( session_obj->generic_pool ) );

    /* Initialize free generic objects queue. */
    ( void ) apr_list_init_v2( &session_obj->free_item_q, NULL, NULL );
    for ( i = 0; i < MVM_MAX_GENERIC_OBJECTS_PER_SESSION_V; ++i )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &session_obj->generic_pool[ i ] );
      ( void ) apr_list_add_tail(
                 &session_obj->free_item_q,
                 ( ( apr_list_node_t* ) &session_obj->generic_pool[i] ) );
    }

    session_obj->self_addr = mvm_my_addr;

    ( void ) mmstd_memset( session_obj->session_name, 0,
                           sizeof( session_obj->session_name ) );

    session_obj->is_dual_control = FALSE;
    session_obj->is_hdvoice_ui_enabled = MVM_HDVOICE_UI_UNUSED;
    session_obj->is_beamr_enabled = TRUE;
    session_obj->is_wv2_enabled = TRUE;

    session_obj->master_addr = APRV2_PKT_INIT_ADDR_V;
    session_obj->master_port = APR_NULL_V;

    ( void ) apr_list_init_v2( &session_obj->indirection_q, NULL, NULL );

    /* Initialize the stream list. */
    ( void ) apr_list_init_v2( &session_obj->stream_q, NULL, NULL );
    session_obj->stream_rsp_cnt = 0;

    /* Initialize the vocproc list. */
    ( void ) apr_list_init_v2( &session_obj->vocproc_q, NULL, NULL );
    session_obj->vocproc_rsp_cnt = 0;

    session_obj->stream_attach_detach_vocproc_rsp_cnt = 0;
    session_obj->detach_all_rsp_cnt = 0;

    session_obj->requested_var_voc_rx_sampling_rate = MVM_DEFAULT_VAR_VOC_DEC_SAMPLING_RATE;
    session_obj->requested_var_voc_tx_sampling_rate = MVM_DEFAULT_VAR_VOC_ENC_SAMPLING_RATE;

    /* Initialize the session and stream state machine control variables. */
    session_obj->session_ctrl.statejob_handle = APR_NULL_V;

    session_obj->session_ctrl.state = MVM_STATE_ENUM_RESET;
    session_obj->session_ctrl.goal = MVM_GOAL_ENUM_NONE;
    session_obj->session_ctrl.action = MVM_ACTION_ENUM_NONE;
    session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

    session_obj->avsync_vocproc_delay.vp_rx_normalized_total_delay = 0;
    session_obj->avsync_vocproc_delay.vp_tx_normalized_total_delay = 0;
    session_obj->avsync_vocproc_delay.max_vp_rx_algorithmic_delay = 0;
    session_obj->avsync_vocproc_delay.max_vp_tx_algorithmic_delay = 0;

    session_obj->system_config.network_id = VSS_ICOMMON_CAL_NETWORK_ID_NONE;
    session_obj->system_config.media_id = VSS_MEDIA_ID_NONE;
    session_obj->system_config.rx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
    session_obj->system_config.tx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
    session_obj->system_config.dec_sr = MVM_DEFAULT_DEC_SR;
    session_obj->system_config.enc_sr = MVM_DEFAULT_ENC_SR;
    session_obj->system_config.rx_pp_sr = MVM_DEFAULT_RX_PP_SR;
    session_obj->system_config.tx_pp_sr = MVM_DEFAULT_TX_PP_SR;
    session_obj->system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;
    session_obj->system_config.vsid = 0;
    session_obj->system_config.vfr_mode = VSS_ICOMMON_VFR_MODE_SOFT;
    session_obj->system_config.call_num = 0;

    session_obj->vfr_info.is_subscribed = FALSE;
    session_obj->vfr_info.vfr_start_reference_us = 0;
    session_obj->vfr_info.vfr_mode = VSS_ICOMMON_VFR_MODE_SOFT;
    session_obj->vfr_info.vsid = 0;
    session_obj->vfr_info.vfr_cycle_duration_us = MVM_DEFAULT_VFR_CYCLE_DURATION_US;

    session_obj->voice_timing.is_timing_ref_available = FALSE;
    session_obj->voice_timing.tx_ref_timestamp_us = 0;
    session_obj->voice_timing.rx_ref_timestamp_us = 0;
    session_obj->voice_timing.enc_offset = 0;
    session_obj->voice_timing.dec_req_offset = 0;
    session_obj->voice_timing.dec_offset = 0;
    session_obj->voice_timing.vprx_delivery_offset = 0;
    session_obj->voice_timing.vptx_delivery_offset = 0;
    session_obj->voice_timing.vptx_proc_start_offset = 0;
    session_obj->voice_timing.vptx_buf_start_offset = 0;

    session_obj->clock_and_concurrency_config.is_multi_session = FALSE;
    session_obj->clock_and_concurrency_config.total_core_kpps = 0;

    session_obj->stream_vocproc_load.num_nb_streams = 0;
    session_obj->stream_vocproc_load.num_wb_streams = 0;
    session_obj->stream_vocproc_load.num_swb_streams = 0;
    session_obj->stream_vocproc_load.num_fb_plus_streams = 0;
    session_obj->stream_vocproc_load.num_nb_vocprocs = 0;
    session_obj->stream_vocproc_load.num_wb_vocprocs = 0;
    session_obj->stream_vocproc_load.num_swb_vocprocs = 0;
    session_obj->stream_vocproc_load.num_fb_plus_vocprocs = 0;
    session_obj->stream_vocproc_load.total_kpps = 0;
    session_obj->stream_vocproc_load.total_stream_kpps = 0;
    session_obj->stream_vocproc_load.total_vocproc_kpps = 0;
    session_obj->stream_vocproc_load.vocproc_rx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
    session_obj->stream_vocproc_load.vocproc_tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
    session_obj->stream_vocproc_load.stream_media_id = VSS_MEDIA_ID_NONE;
    session_obj->stream_vocproc_load.enc_kpps = 0;
    session_obj->stream_vocproc_load.dec_kpps = 0;
    session_obj->stream_vocproc_load.dec_pp_kpps = 0;
    session_obj->stream_vocproc_load.vp_rx_kpps = 0;
    session_obj->stream_vocproc_load.vp_tx_kpps = 0;
    session_obj->stream_vocproc_load.tx_num_channels = 0;
    session_obj->stream_vocproc_load.tx_mpps_scale_factor = 0;
    session_obj->stream_vocproc_load.tx_bw_scale_factor = 0;
    session_obj->stream_vocproc_load.rx_mpps_scale_factor = 0;
    session_obj->stream_vocproc_load.rx_bw_scale_factor = 0;
    session_obj->stream_vocproc_load.vptx_frame_size_us = 0;

    session_obj->tty_mode = 0; /* 0 = disable TTY. */

    session_obj->modem_state = MVM_CLIENT_STATE_ENUM_STOP;
    session_obj->apps_state = MVM_CLIENT_STATE_ENUM_STOP;

    session_obj->is_stream_reinit_required = FALSE;
    session_obj->is_stream_reconfig_required = TRUE;

    session_obj->is_vocproc_reinit_required = FALSE;
    session_obj->is_vocproc_reconfig_required = TRUE;
  }

  *ret_session_obj = session_obj;

  return APR_EOK;
}

static int32_t mvm_create_indirection_object (
  uint16_t client_addr,
  char_t* req_session_name,
  uint32_t req_session_name_size,
  uint32_t access_bits,
  mvm_indirection_object_t** ret_indirect_obj,
  mvm_session_object_t** ret_session_obj
)
{
  int32_t rc;
  uint32_t result;
  mvm_indirection_object_t* indirect_obj;
  mvm_session_object_t* session_obj = NULL;
  mvm_generic_item_t* generic_item;

  if ( ( ret_indirect_obj == NULL ) || ( ret_session_obj == NULL ) ||
       ( req_session_name_size > MVM_MAX_SESSION_NAME_SIZE ) )
  {
    return APR_EBADPARAM;
  }

  /* See if a session with this name has already been created. */
  if ( req_session_name_size > 0 )
  {
    generic_item = ( ( mvm_generic_item_t* ) &mvm_session_q.dummy );

    for ( ;; )
    {
      rc = apr_list_get_next( &mvm_session_q,
                              ( ( apr_list_node_t* ) generic_item ),
                              ( ( apr_list_node_t** ) &generic_item ) );
      if ( rc ) break;

      rc = mvm_get_typed_object( generic_item->handle,
                                 MVM_OBJECT_TYPE_ENUM_SESSION,
                                 ( ( mvm_object_t** ) &session_obj ) );
      MVM_PANIC_ON_ERROR( rc );

      if ( mmstd_strncmp( session_obj->session_name,
                          sizeof( session_obj->session_name ),
                          req_session_name,
                          req_session_name_size ) == 0 )
      {
        /* Found a session with the requested name. */
        break;
      }
      else
      {
        /* Keep looking. */
        session_obj = NULL;
      }
    }
  }

  rc = mvm_mem_alloc_object( sizeof( mvm_indirection_object_t ),
                             ( ( mvm_object_t** ) &indirect_obj ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  if ( session_obj == NULL )
  { /* Didn't find a session with the requested name, so create a new session. */
    if ( mvm_session_q.size >= MVM_MAX_SESSIONS )
    {
      rc = mvm_free_object( ( mvm_object_t* ) indirect_obj );
      MVM_PANIC_ON_ERROR( rc );

      return APR_ENORESOURCE;
    }

    rc = mvm_create_session_object( &session_obj );
    if ( rc )
    {
      rc = mvm_free_object( ( mvm_object_t* ) indirect_obj );
      MVM_PANIC_ON_ERROR( rc );

      return APR_ENORESOURCE;
    }

    if ( req_session_name != NULL )
    {
      /* Save the session name. */
      uint32_t dst_len = MMSTD_MIN( sizeof( session_obj->session_name ),
                                    req_session_name_size );
      result = mmstd_strlcpy( session_obj->session_name, req_session_name,
                              dst_len );

      if ( result >= dst_len )
      { /* Truncation happened. */
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mvm_create_indirection_object: ERROR, input truncation" );

        rc = mvm_free_object( ( mvm_object_t* ) session_obj );
        MVM_PANIC_ON_ERROR( rc );
        rc = mvm_free_object( ( mvm_object_t* ) indirect_obj );
        MVM_PANIC_ON_ERROR( rc );

        return APR_EBADPARAM;
      }
    }

    /* Add the new session to the session tracking list. */
    rc = apr_list_remove_head( &mvm_session_list_free_q,
                               ( ( apr_list_node_t** ) &generic_item ) );

    generic_item->handle = session_obj->header.handle;

    ( void ) apr_list_add_tail( &mvm_session_q, &generic_item->link );
  }

  { /* Initialize the indirection object. */
    indirect_obj->header.type = MVM_OBJECT_TYPE_ENUM_INDIRECTION;

    indirect_obj->client_addr = client_addr;
    indirect_obj->session_handle = session_obj->header.handle;
    indirect_obj->access_bits = access_bits;
  }

  { /* Associate the indirection object to the session object. */
    rc = apr_list_remove_head( &session_obj->free_item_q,
                               ( ( apr_list_node_t** ) &generic_item ) );
    MVM_PANIC_ON_ERROR( rc );

    generic_item->handle = indirect_obj->header.handle;

    ( void ) apr_list_add_tail( &session_obj->indirection_q, &generic_item->link );
  }

  *ret_indirect_obj = indirect_obj;
  *ret_session_obj = session_obj;

  return APR_EOK;
}

/* This function assumes the input parameters are good. */
static int32_t mvm_find_object_from_session (
  apr_list_t* list,
  uint32_t handle,
  mvm_generic_item_t** ret_generic_item
)
{
  int32_t rc;
  mvm_generic_item_t* generic_item;

  generic_item = ( ( mvm_generic_item_t* ) &list->dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( list, ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc )
    {
      return APR_EFAILED;
    }

    if ( generic_item->handle == handle )
    {
      break;
    }
  }

  *ret_generic_item = generic_item;

  return APR_EOK;
}

static int32_t mvm_destroy_indirection_object (
  mvm_indirection_object_t* indirect_obj
)
{
  int32_t rc;
  mvm_session_object_t* session_obj;
  mvm_generic_item_t* generic_item;

  rc = mvm_get_typed_object( indirect_obj->session_handle,
                             MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  MVM_PANIC_ON_ERROR( rc );

 /* Disassociate the indirection object from the session object. */
  rc = mvm_find_object_from_session( &session_obj->indirection_q,
                                     indirect_obj->header.handle,
                                     &generic_item );
  MVM_PANIC_ON_ERROR( rc );

  ( void ) apr_list_delete( &session_obj->indirection_q, &generic_item->link );
  ( void ) apr_list_add_tail( &session_obj->free_item_q, &generic_item->link );

  if ( session_obj->indirection_q.size == 0 )
  { /* Free the session object when there are no more references and
     * an remove from global sessions list.*/
    generic_item = ( ( mvm_generic_item_t* ) &mvm_session_q.dummy );
    for ( ;; )
    {
      rc = apr_list_get_next( &mvm_session_q,
                              ( ( apr_list_node_t* ) generic_item ),
                              ( ( apr_list_node_t** ) &generic_item ) );
      if ( rc )
        break;

      if ( generic_item->handle == session_obj->header.handle )
      {
        apr_list_delete ( &mvm_session_q, ( apr_list_node_t* ) generic_item );
        apr_list_add_tail( &mvm_session_list_free_q, &generic_item->link );
        break;
      }
    }

    ( void ) mvm_free_object( ( mvm_object_t* ) session_obj );
  }

  return APR_EOK;
}

static int32_t mvm_destroy_session_object (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_generic_item_t* generic_item;
  mvm_indirection_object_t* indirect_object;

  /* Remove session object. */
  generic_item = ( ( mvm_generic_item_t* ) &mvm_session_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &mvm_session_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc )
      break;

    if ( generic_item->handle == session_obj->header.handle )
    {
      apr_list_delete ( &mvm_session_q, ( apr_list_node_t* ) generic_item );
      apr_list_add_tail( &mvm_session_list_free_q, &generic_item->link );
      break;
    }
  }

  /* Remove indirect object. */
  generic_item = ( ( mvm_generic_item_t* )&session_obj->indirection_q.dummy );
  rc = apr_list_get_next( &session_obj->indirection_q,
                          ( ( apr_list_node_t* ) generic_item ),
                          ( ( apr_list_node_t** ) &generic_item ) );
  if ( rc )
  {
    /* At this point, we should have exact one item here. */
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_destroy_session_object(): failed to get indirect object, "
           "rc = (0x%08x)", rc );
  }
  else
  {
    ( void ) mvm_get_typed_object( generic_item->handle,
                                   MVM_OBJECT_TYPE_ENUM_INDIRECTION,
                                  ( ( mvm_object_t** ) &indirect_object ) );
    ( void ) apr_list_delete( &session_obj->indirection_q, &generic_item->link );
    ( void ) apr_list_add_tail( &session_obj->free_item_q, &generic_item->link );
    ( void ) mvm_mem_free_object( ( mvm_object_t* ) indirect_object );
  }

  ( void ) apr_lock_destroy( session_obj->lock );
  ( void ) mvm_free_object( ( mvm_object_t* ) session_obj );

  return APR_EOK;
}

static int32_t mvm_create_simple_job_object (
  uint32_t context_handle,
  mvm_simple_job_object_t** ret_job_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;

  if ( ret_job_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = mvm_mem_alloc_object( sizeof( mvm_simple_job_object_t ),
                             ( ( mvm_object_t** ) &job_obj ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    job_obj->header.type = MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB;

    job_obj->context_handle = context_handle;
    mvm_set_default_response_table( job_obj->fn_table );
    job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] = mvm_simple_result_rsp_fn;
    job_obj->is_accepted = FALSE;
    job_obj->is_completed = FALSE;
    job_obj->status = APR_UNDEFINED_ID_V;
  }

  *ret_job_obj = job_obj;

  return APR_EOK;
}

static int32_t mvm_create_sequencer_job_object (
  mvm_sequencer_job_object_t** ret_job_obj
)
{
  int32_t rc;
  mvm_sequencer_job_object_t* job_obj;

  if ( ret_job_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = mvm_mem_alloc_object( sizeof( mvm_sequencer_job_object_t ),
                             ( ( mvm_object_t** ) &job_obj ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  { /* Initialize the pending job object. */
    job_obj->header.type = MVM_OBJECT_TYPE_ENUM_SEQUENCER_JOB;

    job_obj->state = APR_NULL_V;
    job_obj->subjob_obj = NULL;
    job_obj->status = APR_UNDEFINED_ID_V;
  }

  *ret_job_obj = job_obj;

  return APR_EOK;
}

static int32_t mvm_free_object (
  mvm_object_t* object
)
{
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Perform object-specific clean-up necessary. */
  switch ( object->header.type )
  {
   case MVM_OBJECT_TYPE_ENUM_INDIRECTION:
     ( void ) mvm_destroy_indirection_object( &object->indirection );
     break;

   case MVM_OBJECT_TYPE_ENUM_UNINITIALIZED:
   case MVM_OBJECT_TYPE_ENUM_SESSION:
   case MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB:
   case MVM_OBJECT_TYPE_ENUM_SEQUENCER_JOB:
   case MVM_OBJECT_TYPE_ENUM_INVALID:
   default:
     break;
  }

  /* Free the object memory and object handle. */
  ( void ) mvm_mem_free_object( object );

  return APR_EOK;
}

/****************************************************************************
 * MVM SUBSYSTEM RESTART (SSR) HELPER FUNCTIONS                             *
 ****************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CLEANUP HELPER FUNCTIONS                                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Stops each of the MVM session on behalf of MVM clients who reside in a
 * subsystem that is being restarted.
 *
 * TODO: Current implementation only handles Modem SSR properly and it issues
 * VSS_IMVM_CMD_MODEM_STOP_VOICE on behalf of Modem full control clients. In
 * the future, we need to save the client address whenever the client
 * "enables"/"starts" a voice feature, and "disable"/"stop" the feature on
 * behalf of the client, once we know that the client's subsystem is being
 * restarted.
 */
static int32_t mvm_ssr_stop_voice (
  uint8_t domain_id
)
{
  int32_t rc;
  mvm_generic_item_t* generic_item_1;
  mvm_generic_item_t* generic_item_2;
  mvm_session_object_t* session_obj;
  mvm_indirection_object_t* indirection_obj;
  mvm_simple_job_object_t* job_obj;

  mvm_ssr_cleanup_cmd_tracking.num_cmd_issued = 0;
  mvm_ssr_cleanup_cmd_tracking.rsp_cnt = 0;

  generic_item_1 = ( ( mvm_generic_item_t* ) &mvm_session_q.dummy );

  for ( ;; )
  {
    rc = apr_list_get_next( &mvm_session_q,
                            ( ( apr_list_node_t* ) generic_item_1 ),
                            ( ( apr_list_node_t** ) &generic_item_1 ) );
    if ( rc ) break;

    rc = mvm_get_typed_object( generic_item_1->handle,
                               MVM_OBJECT_TYPE_ENUM_SESSION,
                               ( ( mvm_object_t** ) &session_obj ) );
    MVM_PANIC_ON_ERROR( rc );

    generic_item_2 = ( ( mvm_generic_item_t* ) &session_obj->indirection_q.dummy );

    for ( ;; )
    {
      rc = apr_list_get_next( &session_obj->indirection_q,
                              ( ( apr_list_node_t* ) generic_item_2 ),
                              ( ( apr_list_node_t** ) &generic_item_2 ) );
      if ( rc ) break;

      rc = mvm_get_typed_object( generic_item_2->handle,
                                 MVM_OBJECT_TYPE_ENUM_INDIRECTION,
                                 ( ( mvm_object_t** ) &indirection_obj ) );

      if ( ( APR_GET_FIELD( APRV2_PKT_DOMAIN_ID,
                            indirection_obj->client_addr ) == domain_id ) &&
           ( APR_GET_FIELD( MVM_INDIRECTION_ACCESS_FULL_CONTROL,
                            indirection_obj->access_bits ) ) )
      {
        rc = mvm_create_simple_job_object( APR_NULL_V, &job_obj );
        MVM_PANIC_ON_ERROR( rc );
        job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
          mvm_ssr_cleanup_cmd_result_count_rsp_fn;

        rc = __aprv2_cmd_alloc_send(
               mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
               mvm_my_addr, APR_NULL_V,
               mvm_my_addr, ( ( uint16_t ) indirection_obj->header.handle ),
               job_obj->header.handle,
               VSS_IMVM_CMD_MODEM_STOP_VOICE,
               NULL, 0 );
        MVM_COMM_ERROR( rc, mvm_my_addr );

        mvm_ssr_cleanup_cmd_tracking.num_cmd_issued++;

        break;
      }
    }
  }

  return APR_EOK;
}

/* For each of the MVM sessions, detach the streams currently attached to it on
 * behalf of MVM clients who reside in a subsystem that is being restarted.
 *
 * TODO: Current implementation only handles Modem SSR properly. It assumes
 * that for dual controlled voice use case, only MVM full control Modem clients
 * will attach streams to MVM. Based on this assumption, the current
 * implementation detaches all the streams attached to each MVM session on
 * behalf of full control MVM clients who reside in a subsystem that is being
 * restarted. In the future, whenever the client attaches a stream, we need to
 * save the client address, and detach the stream once we know that the
 * client's subsystem is being restarted.
 */
static int32_t mvm_ssr_detach_stream (
  uint8_t domain_id
)
{
  int32_t rc;
  mvm_generic_item_t* generic_item_1;
  mvm_generic_item_t* generic_item_2;
  mvm_generic_item_t* generic_item_3;
  mvm_session_object_t* session_obj;
  mvm_indirection_object_t* indirection_obj;
  mvm_simple_job_object_t* job_obj;
  vss_imvm_cmd_detach_stream_t detach_stream;

  mvm_ssr_cleanup_cmd_tracking.num_cmd_issued = 0;
  mvm_ssr_cleanup_cmd_tracking.rsp_cnt = 0;

  generic_item_1 = ( ( mvm_generic_item_t* ) &mvm_session_q.dummy );

  for ( ;; )
  {
    rc = apr_list_get_next( &mvm_session_q,
                            ( ( apr_list_node_t* ) generic_item_1 ),
                            ( ( apr_list_node_t** ) &generic_item_1 ) );
    if ( rc ) break;

    rc = mvm_get_typed_object( generic_item_1->handle,
                               MVM_OBJECT_TYPE_ENUM_SESSION,
                               ( ( mvm_object_t** ) &session_obj ) );
    MVM_PANIC_ON_ERROR( rc );

    generic_item_2 = ( ( mvm_generic_item_t* ) &session_obj->indirection_q.dummy );

    for ( ;; )
    {
      rc = apr_list_get_next( &session_obj->indirection_q,
                              ( ( apr_list_node_t* ) generic_item_2 ),
                              ( ( apr_list_node_t** ) &generic_item_2 ) );
      if ( rc ) break;

      rc = mvm_get_typed_object( generic_item_2->handle,
                                 MVM_OBJECT_TYPE_ENUM_INDIRECTION,
                                 ( ( mvm_object_t** ) &indirection_obj ) );

      if ( ( APR_GET_FIELD( APRV2_PKT_DOMAIN_ID,
                            indirection_obj->client_addr ) == domain_id ) &&
           ( APR_GET_FIELD( MVM_INDIRECTION_ACCESS_FULL_CONTROL,
                            indirection_obj->access_bits ) ) )
      {
        generic_item_3 = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );

        for ( ;; )
        {
          rc = apr_list_get_next( &session_obj->stream_q,
                                  ( ( apr_list_node_t* ) generic_item_3 ),
                                  ( ( apr_list_node_t** ) &generic_item_3 ) );
          if ( rc ) break;

          detach_stream.handle = ( ( uint16_t ) generic_item_3->handle );

          rc = mvm_create_simple_job_object( APR_NULL_V, &job_obj );
          MVM_PANIC_ON_ERROR( rc );
          job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
            mvm_ssr_cleanup_cmd_result_count_rsp_fn;

          rc = __aprv2_cmd_alloc_send(
                 mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                 mvm_my_addr, APR_NULL_V,
                 mvm_my_addr, ( ( uint16_t ) indirection_obj->header.handle ),
                 job_obj->header.handle,
                 VSS_IMVM_CMD_DETACH_STREAM,
                 &detach_stream, sizeof( detach_stream ) );
          MVM_COMM_ERROR( rc, mvm_my_addr );

          mvm_ssr_cleanup_cmd_tracking.num_cmd_issued++;
        }

        break;
      }
    }
  }

  return APR_EOK;
}

/* Destroy the MVM session control handles (indirection handles) on behalf of
 * MVM clients who reside in a subsystem that is being restarted.
 */
static int32_t mvm_ssr_destroy_session (
  uint8_t domain_id
)
{
  int32_t rc;
  mvm_generic_item_t* generic_item_1;
  mvm_generic_item_t* generic_item_2;
  mvm_session_object_t* session_obj;
  mvm_indirection_object_t* indirection_obj;
  mvm_simple_job_object_t* job_obj;

  mvm_ssr_cleanup_cmd_tracking.num_cmd_issued = 0;
  mvm_ssr_cleanup_cmd_tracking.rsp_cnt = 0;

  generic_item_1 = ( ( mvm_generic_item_t* ) &mvm_session_q.dummy );

  for ( ;; )
  {
    rc = apr_list_get_next( &mvm_session_q,
                            ( ( apr_list_node_t* ) generic_item_1 ),
                            ( ( apr_list_node_t** ) &generic_item_1 ) );
    if ( rc ) break;

    rc = mvm_get_typed_object( generic_item_1->handle,
                               MVM_OBJECT_TYPE_ENUM_SESSION,
                               ( ( mvm_object_t** ) &session_obj ) );
    MVM_PANIC_ON_ERROR( rc );

    generic_item_2 = ( ( mvm_generic_item_t* ) &session_obj->indirection_q.dummy );

    for ( ;; )
    {
      rc = apr_list_get_next( &session_obj->indirection_q,
                              ( ( apr_list_node_t* ) generic_item_2 ),
                              ( ( apr_list_node_t** ) &generic_item_2 ) );
      if ( rc ) break;

      rc = mvm_get_typed_object( generic_item_2->handle,
                                 MVM_OBJECT_TYPE_ENUM_INDIRECTION,
                                 ( ( mvm_object_t** ) &indirection_obj ) );

      if ( APR_GET_FIELD( APRV2_PKT_DOMAIN_ID,
                          indirection_obj->client_addr ) == domain_id )
      {
        rc = mvm_create_simple_job_object( APR_NULL_V, &job_obj );
        MVM_PANIC_ON_ERROR( rc );
        job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
          mvm_ssr_cleanup_cmd_result_count_rsp_fn;

        rc = __aprv2_cmd_alloc_send(
               mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
               mvm_my_addr, APR_NULL_V,
               mvm_my_addr, ( ( uint16_t ) indirection_obj->header.handle ),
               job_obj->header.handle,
               APRV2_IBASIC_CMD_DESTROY_SESSION,
               NULL, 0 );
        MVM_COMM_ERROR( rc, mvm_my_addr );

        mvm_ssr_cleanup_cmd_tracking.num_cmd_issued++;
      }
    }
  }

  return APR_EOK;
}

/* Unmap memory on behalf of MVM clients who reside in a subsystem that is
 * being restarted.
 */
static int32_t mvm_ssr_unmap_memory (
  uint8_t domain_id
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_ssr_mem_handle_tracking_item_t* mem_handle_tracking_item;
  vss_imemory_cmd_unmap_t mem_unmap;

  mvm_ssr_cleanup_cmd_tracking.num_cmd_issued = 0;
  mvm_ssr_cleanup_cmd_tracking.rsp_cnt = 0;

  mem_handle_tracking_item =
    ( ( mvm_ssr_mem_handle_tracking_item_t* )
      &mvm_ssr_mem_handle_tracking.used_q.dummy );

  for ( ;; )
  {
    rc = apr_list_get_next(
           &mvm_ssr_mem_handle_tracking.used_q,
           ( ( apr_list_node_t* ) mem_handle_tracking_item ),
           ( ( apr_list_node_t** ) &mem_handle_tracking_item ) );
    if ( rc ) break;

    if ( APR_GET_FIELD( APRV2_PKT_DOMAIN_ID,
                        mem_handle_tracking_item->client_addr ) == domain_id )
    {
      rc = mvm_create_simple_job_object( APR_NULL_V, &job_obj );
      MVM_PANIC_ON_ERROR( rc );
      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_ssr_cleanup_cmd_result_count_rsp_fn;

      mem_unmap.mem_handle = mem_handle_tracking_item->mem_handle;

      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             mvm_my_addr, APR_NULL_V,
             mvm_my_addr, APR_NULL_V,
             job_obj->header.handle,
             VSS_IMEMORY_CMD_UNMAP,
             &mem_unmap, sizeof( mem_unmap ) );
      MVM_COMM_ERROR( rc, mvm_my_addr );

      mvm_ssr_cleanup_cmd_tracking.num_cmd_issued++;
    }
  }

  return APR_EOK;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * SSR CALLBACKS                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static void mvm_ssr_modem_after_shutdown_handler ( void )
{
  int32_t rc;
  vss_issr_cmd_cleanup_t cleanup_arg;
  mvm_simple_job_object_t* job_obj;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "mvm_ssr_modem_after_shutdown_handler is called." );

  rc = mvm_create_simple_job_object( APR_NULL_V, &job_obj );
  MVM_PANIC_ON_ERROR( rc );
  job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
    mvm_simple_self_destruct_result_rsp_fn;

  cleanup_arg.domain_id = APRV2_IDS_DOMAIN_ID_MODEM_V;

  rc = __aprv2_cmd_alloc_send(
         mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         mvm_my_addr, APR_NULL_V,
         mvm_my_addr, APR_NULL_V,
         job_obj->header.handle,
         VSS_ISSR_CMD_CLEANUP,
         &cleanup_arg, sizeof( cleanup_arg ) );
  MVM_COMM_ERROR( rc, mvm_my_addr );
}

static void mvm_ssr_modem_after_powerup_handler ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "mvm_ssr_modem_after_powerup_handler() is called." );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MEM HANDLE TRACKING FUNCTIONS                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static int32_t mvm_ssr_track_mem_handle (
  uint16_t client_addr,
  uint32_t mem_handle
)
{
  int32_t rc;
  mvm_ssr_mem_handle_tracking_item_t* mem_handle_tracking_item;

  for ( ;; )
  {
    rc = apr_list_remove_head( &mvm_ssr_mem_handle_tracking.free_q,
                               ( ( apr_list_node_t** ) &mem_handle_tracking_item ) );
    if ( rc ) break;

    mem_handle_tracking_item->client_addr = client_addr;
    mem_handle_tracking_item->mem_handle = mem_handle;

    ( void ) apr_list_add_tail( &mvm_ssr_mem_handle_tracking.used_q,
                                &mem_handle_tracking_item->link );

    break;
  }

  return rc;
}

static int32_t mvm_ssr_untrack_mem_handle (
  uint32_t mem_handle
)
{
  int32_t rc;
  mvm_ssr_mem_handle_tracking_item_t* mem_handle_tracking_item;

  mem_handle_tracking_item =
    ( ( mvm_ssr_mem_handle_tracking_item_t* )
      &mvm_ssr_mem_handle_tracking.used_q.dummy );

  for ( ;; )
  {
    rc = apr_list_get_next(
           &mvm_ssr_mem_handle_tracking.used_q,
           ( ( apr_list_node_t* ) mem_handle_tracking_item ),
           ( ( apr_list_node_t** ) &mem_handle_tracking_item ) );
    if ( rc ) break;

    if ( mem_handle_tracking_item->mem_handle == mem_handle )
    {
      ( void ) apr_list_delete(
                 &mvm_ssr_mem_handle_tracking.used_q,
                 &mem_handle_tracking_item->link );
      ( void ) apr_list_add_tail(
                 &mvm_ssr_mem_handle_tracking.free_q,
                 &mem_handle_tracking_item->link );
      break;
    }
  }

  return rc;
}

/****************************************************************************
 * MVM SYSTEM CONFIGURATION-RELATED HELPER FUNCTIONS                        *
 ****************************************************************************/
static bool_t mvm_is_network_id_valid (
  uint32_t network_id
)
{
  bool_t rc = FALSE;

  switch ( network_id )
  {
  case VSS_ICOMMON_CAL_NETWORK_ID_NONE:
  case VSS_ICOMMON_CAL_NETWORK_ID_CDMA:
  case VSS_ICOMMON_CAL_NETWORK_ID_GSM:
  case VSS_ICOMMON_CAL_NETWORK_ID_WCDMA:
  case VSS_ICOMMON_CAL_NETWORK_ID_VOIP:
  case VSS_ICOMMON_CAL_NETWORK_ID_LTE:
    rc = TRUE;
    break;

  default:
    rc = FALSE;
    break;
  }

  return rc;
}

static bool_t mvm_is_media_id_valid (
  uint32_t media_id
)
{
  bool_t rc = FALSE;

  switch ( media_id )
  {
  case VSS_MEDIA_ID_13K:
  case VSS_MEDIA_ID_EVRC:
  case VSS_MEDIA_ID_4GV_NB:
  case VSS_MEDIA_ID_4GV_WB:
  case VSS_MEDIA_ID_4GV_NW:
  case VSS_MEDIA_ID_4GV_NW2K:
  case VSS_MEDIA_ID_AMR_NB:
  case VSS_MEDIA_ID_AMR_WB:
  case VSS_MEDIA_ID_EAMR:
  case VSS_MEDIA_ID_EFR:
  case VSS_MEDIA_ID_FR:
  case VSS_MEDIA_ID_HR:
  case VSS_MEDIA_ID_PCM_8_KHZ:
  case VSS_MEDIA_ID_PCM_16_KHZ:
  case VSS_MEDIA_ID_PCM_32_KHZ:
  case VSS_MEDIA_ID_PCM_48_KHZ:
  case VSS_MEDIA_ID_G711_ALAW:
  case VSS_MEDIA_ID_G711_MULAW:
  case VSS_MEDIA_ID_G711_ALAW_V2:
  case VSS_MEDIA_ID_G711_MULAW_V2:
  case VSS_MEDIA_ID_G711_LINEAR:
  case VSS_MEDIA_ID_G729:
  case VSS_MEDIA_ID_G722:
  case VSS_MEDIA_ID_EVS:
    rc = TRUE;
    break;

  default:
    rc = FALSE;
    break;
  }

  return rc;
}

static bool_t mvm_is_var_sr_supported (
  uint32_t media_id
)
{
  bool_t rc = FALSE;

  switch ( media_id )
  {
    case VSS_MEDIA_ID_EVS:
      rc = TRUE;
      break;

    default:
      break;
  }

  return rc;
}

static bool_t mvm_is_voc_sample_rate_valid (
  uint32_t sample_rate
)
{
  bool_t rc = FALSE;

  switch ( sample_rate )
  {
    case 0: /* 0 is Defaulted to 48 kHz for EVS */
    case 8000:
    case 16000:
    case 32000:
    case 41000:
    case 48000:
      rc = TRUE;
      break;

    default:
      break;
  }

  return rc;
}

static bool_t mvm_is_voc_op_mode_valid (
  uint32_t voc_op_mode
)
{
  bool_t rc = FALSE;

  switch ( voc_op_mode )
  {
    case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE:
    case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NB:
    case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_WB:
    case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_SWB:
    case VSS_ICOMMON_CAL_VOC_OPERATING_MODE_FB:
      rc = TRUE;
      break;
  
    default:
      break;
  }

  return rc;
}

/* BACKWARD COMPATIBILITY */
/* Convert legacy network id to new network id + sample rates.
 * (Also validate legacy network id in the process.)
 */
static int32_t mvm_convert_legacy_network_id (
  uint32_t legacy_network_id,
  uint32_t* ret_network_id,
  uint32_t* ret_rx_pp_sr,
  uint32_t* ret_tx_pp_sr
)
{
  int32_t rc;

  if ( ret_network_id == NULL )
  {
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
    return APR_EBADPARAM;
  }

  switch ( legacy_network_id )
  {
  case VSS_NETWORK_ID_DEFAULT:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_NONE;
      *ret_rx_pp_sr = MVM_DEFAULT_RX_PP_SR;
      *ret_tx_pp_sr = MVM_DEFAULT_TX_PP_SR;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_CDMA_NB:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_CDMA;
      *ret_rx_pp_sr = 8000;
      *ret_tx_pp_sr = 8000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_CDMA_WB:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_CDMA;
      *ret_rx_pp_sr = 16000;
      *ret_tx_pp_sr = 16000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_CDMA_WV:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_CDMA;
      *ret_rx_pp_sr = 16000;
      *ret_tx_pp_sr = 8000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_GSM_NB:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_GSM;
      *ret_rx_pp_sr = 8000;
      *ret_tx_pp_sr = 8000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_GSM_WB:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_GSM;
      *ret_rx_pp_sr = 16000;
      *ret_tx_pp_sr = 16000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_GSM_WV:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_GSM;
      *ret_rx_pp_sr = 16000;
      *ret_tx_pp_sr = 8000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_WCDMA_NB:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_WCDMA;
      *ret_rx_pp_sr = 8000;
      *ret_tx_pp_sr = 8000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_WCDMA_WB:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_WCDMA;
      *ret_rx_pp_sr = 16000;
      *ret_tx_pp_sr = 16000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_WCDMA_WV:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_WCDMA;
      *ret_rx_pp_sr = 16000;
      *ret_tx_pp_sr = 8000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_VOIP_NB:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_VOIP;
      *ret_rx_pp_sr = 8000;
      *ret_tx_pp_sr = 8000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_VOIP_WB:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_VOIP;
      *ret_rx_pp_sr = 16000;
      *ret_tx_pp_sr = 16000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_VOIP_WV:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_VOIP;
      *ret_rx_pp_sr = 16000;
      *ret_tx_pp_sr = 8000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_LTE_NB:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_LTE;
      *ret_rx_pp_sr = 8000;
      *ret_tx_pp_sr = 8000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_LTE_WB:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_LTE;
      *ret_rx_pp_sr = 16000;
      *ret_tx_pp_sr = 16000;
      rc = APR_EOK;
    }
    break;

  case VSS_NETWORK_ID_LTE_WV:
    {
      *ret_network_id = VSS_ICOMMON_CAL_NETWORK_ID_LTE;
      *ret_rx_pp_sr = 16000;
      *ret_tx_pp_sr = 8000;
      rc = APR_EOK;
    }
    break;

  default:
    rc = APR_EBADPARAM;
    break;
  }

  return rc;
}

static int32_t mvm_determine_closest_supported_pp_sr (
  mvm_session_object_t* session_obj
)
{
  switch ( session_obj->system_config.dec_sr )
  {
    case 8000:
      session_obj->system_config.rx_pp_sr = 8000;
      break;

    case 16000:
      session_obj->system_config.rx_pp_sr = 16000;
      break;

    case 32000:
      session_obj->system_config.rx_pp_sr = 32000;
      break;

    case 48000:
      session_obj->system_config.rx_pp_sr = 48000;
      break;

    default:
      MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      return APR_EBADPARAM;
  }

  switch ( session_obj->system_config.enc_sr )
  {
    case 8000:
      session_obj->system_config.tx_pp_sr = 8000;
      break;

    case 16000:
      session_obj->system_config.tx_pp_sr = 16000;
      break;

    case 32000:
      session_obj->system_config.tx_pp_sr = 32000;
      break;

    case 48000:
      session_obj->system_config.tx_pp_sr = 48000;
      break;

    default:
      MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      return APR_EBADPARAM;
  }

  return APR_EOK;
}

/* Determine the target_set enc, dec sampling rates for the EVS media
type. Parameters ret_dec_sampling_rate and ret_enc_sampling_rate should
be non-NULL*/
static int32_t mvm_evs_sanitize_sampling_rates (
  mvm_session_object_t* session_obj,
  uint32_t requested_rx_sampling_rate,
  uint32_t requested_tx_sampling_rate
)
{
  int32_t rc = APR_EOK;

  switch ( requested_rx_sampling_rate )
  {
    case 8000:
      session_obj->system_config.dec_sr = 8000;
      break;

    case 16000:
      session_obj->system_config.dec_sr = 16000;
      break;

    case 32000:
      session_obj->system_config.dec_sr = 32000;
      break;

    case 0:
    case 48000:
      session_obj->system_config.dec_sr = 48000;
      break;

    default:
      rc |= APR_EBADPARAM;
      break;
  }

  switch ( requested_tx_sampling_rate )
  {
    case 8000:
      session_obj->system_config.enc_sr = 8000;
      break;

    case 16000:
      session_obj->system_config.enc_sr = 16000;
      break;

    case 32000:
      session_obj->system_config.enc_sr = 32000;
      break;

    case 0:
    case 48000:
      session_obj->system_config.enc_sr = 48000;
      break;

    default:
      rc |= APR_EBADPARAM;
      break;
  }

  return rc;
}

/* Determine the target_set enc, dec and pp sampling rates based on the
 * media type and the client requested variable vocoder sampling rates.
 */
static int32_t mvm_determine_target_sampling_rates (
  mvm_session_object_t* session_obj,
  uint32_t requested_var_voc_rx_sampling_rate,
  uint32_t requested_var_voc_tx_sampling_rate
)
{
  int32_t rc = APR_EOK;

  for ( ;; )
  {
    if ( mvm_is_media_id_valid( session_obj->system_config.media_id ) == FALSE ||
         mvm_is_voc_sample_rate_valid( requested_var_voc_rx_sampling_rate ) == FALSE ||
         mvm_is_voc_sample_rate_valid( requested_var_voc_tx_sampling_rate ) == FALSE )
    {
      rc = APR_EBADPARAM;
      break;
    }

    /* First determine the enc/dec sampling rates. */
    switch ( session_obj->system_config.media_id )
    {
      case VSS_MEDIA_ID_13K:
      case VSS_MEDIA_ID_EVRC:
      case VSS_MEDIA_ID_4GV_NB:
      case VSS_MEDIA_ID_AMR_NB:
      case VSS_MEDIA_ID_EFR:
      case VSS_MEDIA_ID_FR:
      case VSS_MEDIA_ID_HR:
      case VSS_MEDIA_ID_PCM_8_KHZ:
      case VSS_MEDIA_ID_G711_ALAW:
      case VSS_MEDIA_ID_G711_MULAW:
      case VSS_MEDIA_ID_G711_ALAW_V2:
      case VSS_MEDIA_ID_G711_MULAW_V2:
      case VSS_MEDIA_ID_G711_LINEAR:
      case VSS_MEDIA_ID_G729:
      case VSS_MEDIA_ID_G722:
        session_obj->system_config.dec_sr = 8000;
        session_obj->system_config.enc_sr = 8000;
        break;

      case VSS_MEDIA_ID_4GV_WB:
      case VSS_MEDIA_ID_4GV_NW:
      case VSS_MEDIA_ID_4GV_NW2K:
      case VSS_MEDIA_ID_AMR_WB:
      case VSS_MEDIA_ID_EAMR:
      case VSS_MEDIA_ID_PCM_16_KHZ:
        session_obj->system_config.dec_sr = 16000;
        session_obj->system_config.enc_sr = 16000;
        break;

      case VSS_MEDIA_ID_PCM_32_KHZ:
        session_obj->system_config.dec_sr = 32000;
        session_obj->system_config.enc_sr = 32000;
        break;

      case VSS_MEDIA_ID_PCM_48_KHZ:
        session_obj->system_config.dec_sr = 48000;
        session_obj->system_config.enc_sr = 48000;
        break;

      case VSS_MEDIA_ID_EVS:
        rc = mvm_evs_sanitize_sampling_rates( session_obj,
                                              requested_var_voc_rx_sampling_rate,
                                              requested_var_voc_tx_sampling_rate );
        break;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        rc = APR_EBADPARAM;
        break;
    }

    if ( rc ) 
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mvm_determine_target_sampling_rates(): Invalid Rx Sampling "
             "Rate %d or Tx Sampling Rate %d", requested_var_voc_rx_sampling_rate,
             requested_var_voc_tx_sampling_rate );
      break;
    }

    /* Now determine the supported closest mataching higher sample rates
     * for the pp based on the enc/dec sample rates. */
    ( void ) mvm_determine_closest_supported_pp_sr( session_obj );

    break;
  }

  return rc;
}

/****************************************************************************
 * MVM STATE MACHINE HELPER FUNCTIONS                                       *
 ****************************************************************************/

static int32_t mvm_do_complete_goal (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  aprv2_ibasic_rsp_result_t res;

  /* Complete the pending command and stay in the same state. */
  res.opcode = APR_UNDEFINED_ID_V;
  res.status = session_obj->session_ctrl.status;
  rc = __aprv2_cmd_alloc_send(
         mvm_apr_handle, APRV2_PKT_MSGTYPE_CMDRSP_V,
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ), /* This src_port is used for debugging only. */
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ), /* This dst_port is used for debugging only. */
         session_obj->session_ctrl.statejob_handle,
         APRV2_IBASIC_RSP_RESULT, &res, sizeof( res ) );
  MVM_COMM_ERROR( rc, session_obj->self_addr );

  session_obj->session_ctrl.statejob_handle = APR_NULL_V;
  session_obj->session_ctrl.goal = MVM_GOAL_ENUM_NONE;
  session_obj->session_ctrl.action = MVM_ACTION_ENUM_NONE;
  session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

  return APR_EOK;
}

static void mvm_log_state_info (
  mvm_session_object_t* session_obj
)
{
  static mvm_state_enum_t prev_state = MVM_STATE_ENUM_UNINITIALIZED;
  static mvm_goal_enum_t prev_goal = MVM_GOAL_ENUM_UNINITIALIZED;
  static mvm_action_enum_t prev_action = MVM_ACTION_ENUM_UNINITIALIZED;

  if ( ( session_obj->session_ctrl.state >= MVM_STATE_ENUM_MAX ) ||
       ( session_obj->session_ctrl.goal >= MVM_GOAL_ENUM_MAX ) ||
       ( session_obj->session_ctrl.action >= MVM_ACTION_ENUM_MAX ) )
  {
    MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "MVM:(0x%08X)  STATE: %d  GOAL: %d  ACTION: %d UNEXPECTED",
           session_obj->header.handle, session_obj->session_ctrl.state,
           session_obj->session_ctrl.goal, session_obj->session_ctrl.action );
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
  }

  if ( ( prev_state != session_obj->session_ctrl.state ) ||
       ( prev_goal != session_obj->session_ctrl.goal ) ||
       ( prev_action != session_obj->session_ctrl.action ) )
  {
    MSG_SPRINTF_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
                   "MVM:(0x%08X)  STATE: %s  GOAL: %s  ACTION: %s",
                   session_obj->header.handle,
                   mvm_state_table[session_obj->session_ctrl.state],
                   mvm_goal_table[session_obj->session_ctrl.goal],
                   mvm_action_table[session_obj->session_ctrl.action] );

    prev_state = session_obj->session_ctrl.state;
    prev_goal = session_obj->session_ctrl.goal;
    prev_action = session_obj->session_ctrl.action;
  }

  return;
}

static int32_t mvm_calculate_timing_offsets (
  mvm_session_object_t* session_obj
)
{
  mvm_voice_timing_t* target_timing;
  uint32_t mvm_max_voice_timing_offset;
  uint16_t vptx_frame_size;
  uint8_t num_of_voice_frame_per_vfr_cycle;
  uint32_t tx_proc_time_per_vfr_cycle = 0;
  uint32_t rx_proc_time_per_voice_frame;

  target_timing = &session_obj->voice_timing;
  num_of_voice_frame_per_vfr_cycle = session_obj->vfr_info.vfr_cycle_duration_us/MVM_VOICE_FRAME_SIZE_US;
  vptx_frame_size = session_obj->stream_vocproc_load.vptx_frame_size_us;
  if( session_obj->voice_proc_delays.vptx_proc > session_obj->voice_proc_delays.venc_proc )
  {
    tx_proc_time_per_vfr_cycle = (num_of_voice_frame_per_vfr_cycle*session_obj->voice_proc_delays.vptx_proc) +
                                 session_obj->voice_proc_delays.venc_proc;
  }
  else
  {
    tx_proc_time_per_vfr_cycle = (num_of_voice_frame_per_vfr_cycle*session_obj->voice_proc_delays.venc_proc) +
                                 session_obj->voice_proc_delays.vptx_proc;
  }
  rx_proc_time_per_voice_frame = session_obj->voice_proc_delays.vdec_proc + session_obj->voice_proc_delays.vprx_proc;

  MSG_5( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: VOICE_TIMING_OFFSETS: vfr_mode: %d, vfr_cycle_duration: %d us, " \
         "tx_proc_time_per_vfr_cycle(adjusted): %d us, rx_proc_time_per_frame: %d us, vptx_frame_size: %d us",
         session_obj->vfr_info.vfr_mode, session_obj->vfr_info.vfr_cycle_duration_us,
         tx_proc_time_per_vfr_cycle, rx_proc_time_per_voice_frame, vptx_frame_size );

  /* All voice timing offsets will be adjusted such that they don't exceed
   * mvm_max_voice_timing_offset. Refer to the comments under
   * MVM_MAX_VOICE_TIMING_OFFSET_MARGIN_US for details.*/
  mvm_max_voice_timing_offset =
    session_obj->vfr_info.vfr_cycle_duration_us - MVM_MAX_VOICE_TIMING_OFFSET_MARGIN_US;


  /* ENC_OFSSET. */
  if ( target_timing->enc_offset > mvm_max_voice_timing_offset )
  {
    target_timing->enc_offset = mvm_max_voice_timing_offset;
  }

  /* DEC_REQUEST_OFSSET. */
  if ( target_timing->dec_req_offset > mvm_max_voice_timing_offset )
  {
    /* Note that if we need to adjust the dec_req_offset, we must also
     * adjust the dec_offset base on the amount of adjustment to be made
     * to dec_req_offset. */
    target_timing->dec_offset +=
      ( session_obj->vfr_info.vfr_cycle_duration_us - target_timing->dec_req_offset );
    target_timing->dec_req_offset = 0;
    target_timing->dec_offset =
      ( target_timing->dec_offset % session_obj->vfr_info.vfr_cycle_duration_us );
  }

  /* DEC_OFSSET. */
  if ( target_timing->dec_offset > mvm_max_voice_timing_offset )
  {
    target_timing->dec_offset = 0;
  }

  /* VPRX_DELIVERY_OFFSET. */
  target_timing->vprx_delivery_offset = target_timing->dec_offset + rx_proc_time_per_voice_frame;
  if ( target_timing->vprx_delivery_offset > session_obj->vfr_info.vfr_cycle_duration_us )
  {
      target_timing->vprx_delivery_offset = 
        ( target_timing->vprx_delivery_offset % session_obj->vfr_info.vfr_cycle_duration_us );
  }
  if ( target_timing->vprx_delivery_offset > mvm_max_voice_timing_offset )
  {
    target_timing->vprx_delivery_offset = 0;
  }

   /* VPTX_BUF_START_OFFSET. */
   target_timing->vptx_buf_start_offset = target_timing->enc_offset +
                                          ( session_obj->vfr_info.vfr_cycle_duration_us *2 ) - 
                                          ( session_obj->voice_proc_delays.venc_proc +
                                            session_obj->voice_proc_delays.vptx_proc/(MVM_VOICE_FRAME_SIZE_US/vptx_frame_size) );
   if ( target_timing->vptx_buf_start_offset > session_obj->vfr_info.vfr_cycle_duration_us )
   {
       target_timing->vptx_buf_start_offset = 
         ( target_timing->vptx_buf_start_offset % session_obj->vfr_info.vfr_cycle_duration_us );
   }
   if ( target_timing->vptx_buf_start_offset > mvm_max_voice_timing_offset )
   {
     target_timing->vptx_buf_start_offset = mvm_max_voice_timing_offset;
   }

   /* VPTX_PROC_START_OFFSET. */
  target_timing->vptx_proc_start_offset = target_timing->enc_offset + 
                                          (session_obj->vfr_info.vfr_cycle_duration_us*2) - 
                                          tx_proc_time_per_vfr_cycle;
  if ( target_timing->vptx_proc_start_offset > session_obj->vfr_info.vfr_cycle_duration_us )
  {
      target_timing->vptx_proc_start_offset = 
        ( target_timing->vptx_proc_start_offset % session_obj->vfr_info.vfr_cycle_duration_us );
  }
  if ( target_timing->vptx_proc_start_offset > mvm_max_voice_timing_offset )
  {
    target_timing->vptx_proc_start_offset = mvm_max_voice_timing_offset;
  }

  /* VPTX_DELIVERY_OFFSET. */ 
  target_timing->vptx_delivery_offset =  target_timing->vptx_proc_start_offset +
                                         session_obj->voice_proc_delays.vptx_proc;
  if ( target_timing->vptx_delivery_offset > session_obj->vfr_info.vfr_cycle_duration_us )
  {
      target_timing->vptx_delivery_offset = 
        ( target_timing->vptx_delivery_offset % session_obj->vfr_info.vfr_cycle_duration_us );
  }
  if ( target_timing->vptx_delivery_offset > mvm_max_voice_timing_offset )
  {
    target_timing->vptx_delivery_offset = mvm_max_voice_timing_offset;
  }

  MSG_7( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: VOICE_TIMING_OFFSETS: vptx_buf_start_offset:(%d), vptx_proc_start_offset:(%d), "
         "vptx_delivery_offset:(%d), enc_offset:(%d), dec_req_offset:(%d), dec_offset:(%d), "
         "vprx_delivery_offset:(%d) ", target_timing->vptx_buf_start_offset,
         target_timing->vptx_proc_start_offset, target_timing->vptx_delivery_offset,
         target_timing->enc_offset, target_timing->dec_req_offset, target_timing->dec_offset,
         target_timing->vprx_delivery_offset);

  return APR_EOK;
}

/****************************************************************************
 * MVM RESPONSE AND EVENT FUNCTION HANLDERS                                 *
 ****************************************************************************/

static void mvm_stream_attach_detach_vocproc_result_count_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_session_object_t* session_obj;
  uint32_t status;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &job_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  rc = mvm_get_typed_object( job_obj->context_handle, MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  /* If the last attach/detach operation failed, log the error. */
  status = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet )->status;

  if ( status != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_stream_attach_detach_vocproc_result_count_rsp_fn(): Last "
           "attach/detach failed with 0x%08X", status );
  }

  /* Count the number of attach_vocproc responses received from the stream
   * (or the nubmer of streams that have responded). */
  session_obj->stream_attach_detach_vocproc_rsp_cnt++;

  ( void ) mvm_free_object( ( mvm_object_t* ) job_obj );
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

static void mvm_vocproc_dynamic_reconfig_result_count_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_session_object_t* session_obj;
  uint32_t status;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &job_obj ) );
  MVM_PANIC_ON_ERROR( rc );
  rc = mvm_get_typed_object( job_obj->context_handle, MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  /* If the last vocproc dynamic reconfig failed, log the error. */
  status = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet )->status;

  if ( status != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_vocproc_dynamic_reconfig_result_count_rsp_fn(): Last reconfig "
           "failed with 0x%08X", status );
  }

  session_obj->vocproc_rsp_cnt++;

  ( void ) mvm_free_object( ( mvm_object_t* ) job_obj );
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

static void mvm_map_memory_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  voice_rsp_shared_mem_map_regions_t* mem_map_rsp;
  mvm_simple_job_object_t* obj;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &obj ) );
  if ( rc == APR_EOK )
  {
    obj->is_completed = TRUE;
    obj->status = APR_EOK;
    mem_map_rsp = APRV2_PKT_GET_PAYLOAD( voice_rsp_shared_mem_map_regions_t, packet );
    obj->context_handle = mem_map_rsp->mem_map_handle;
  }

  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

static void mvm_streams_group_wait_transition_result_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_session_object_t* session_obj;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &job_obj ) );
  MVM_PANIC_ON_ERROR( rc );
  rc = mvm_get_typed_object( job_obj->context_handle, MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "mvm_streams_group_wait_transition_result_rsp_fn(): received "
         "response from stream session 0x%08x ", packet->src_port );

  /* Complete the current action once all streams have responded. */
  session_obj->stream_rsp_cnt += 1;

  if ( session_obj->stream_rsp_cnt == session_obj->stream_q.size )
  {
    /* Individual stream results are ignored. We depend on logs to detect
     * errors. */
    session_obj->session_ctrl.status = APR_EOK;
    mvm_signal_run(); /* Trigger the session state control to run. */
  }

  ( void ) mvm_free_object( ( mvm_object_t* ) job_obj );
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}


static void mvm_streams_set_system_config_group_wait_transition_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  uint32_t max_sr;
  vss_icommon_rsp_set_system_config_t* set_system_config_rsp;
  mvm_simple_job_object_t* job_obj;
  mvm_session_object_t* session_obj;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &job_obj ) );
  MVM_PANIC_ON_ERROR( rc );
  rc = mvm_get_typed_object( job_obj->context_handle, MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  if ( packet->opcode == VSS_ICOMMON_RSP_SET_SYSTEM_CONFIG )
  {
    set_system_config_rsp = APRV2_PKT_GET_PAYLOAD( vss_icommon_rsp_set_system_config_t,
                                                   packet );

    { /* Find the Max enc/dec sampling rate */
     if ( set_system_config_rsp->dec_sr >= set_system_config_rsp->enc_sr )
     {
      max_sr = set_system_config_rsp->dec_sr;
     }
     else
     {
      max_sr = set_system_config_rsp->enc_sr;
     }

     /* Update the stream load management variables. */
     if ( max_sr == MVM_NB_SR )
     {
       session_obj->stream_vocproc_load.num_nb_streams += 1;
     }
     else if ( max_sr == MVM_WB_SR )
     {
       session_obj->stream_vocproc_load.num_wb_streams += 1;
     }
     else if ( max_sr == MVM_SWB_SR )
     {
       session_obj->stream_vocproc_load.num_swb_streams += 1;
     }
     else
     {
       session_obj->stream_vocproc_load.num_fb_plus_streams += 1;
     }

      session_obj->stream_vocproc_load.total_stream_kpps +=
        ( set_system_config_rsp->dec_kpps +
          set_system_config_rsp->dec_pp_kpps +
          set_system_config_rsp->enc_kpps );
    }

    if ( session_obj->stream_q.size == 1 )
    { /* Store the stream properties if there is a single stream attached to MVM. */
      session_obj->stream_vocproc_load.dec_kpps = set_system_config_rsp->dec_kpps;
      session_obj->stream_vocproc_load.dec_pp_kpps = set_system_config_rsp->dec_pp_kpps;
      session_obj->stream_vocproc_load.enc_kpps = set_system_config_rsp->enc_kpps;
      session_obj->stream_vocproc_load.stream_media_id = set_system_config_rsp->stream_media_id;
    }
  }

  /* Complete the current action once all streams have responded. */
  session_obj->stream_rsp_cnt += 1;

  if ( session_obj->stream_rsp_cnt == session_obj->stream_q.size )
  {
    /* Individual stream results are ignored. We depend on logs to detect
     * errors.*/
    session_obj->session_ctrl.status = APR_EOK;
    mvm_signal_run(); /* Trigger the session state control to run. */
  }

  ( void ) mvm_free_object( ( mvm_object_t* ) job_obj );
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

static void mvm_vocprocs_group_wait_transition_result_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_simple_job_object_t* simple_obj = NULL;
  mvm_session_object_t* session_obj = NULL;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &simple_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  rc = mvm_get_typed_object( simple_obj->context_handle, MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  /* If completed command failed, log the error. */
  if ( simple_obj->status != APR_EOK )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "MVM: VOCPROC_GROUP_WAIT: OPCODE:0x%08X STATUS:0x%08X",
           APRV2_PKT_GET_PAYLOAD(aprv2_ibasic_rsp_result_t, packet)->opcode,
           simple_obj->status );
  }

  /* Complete the current action once all VOCPROCs have responded. */
  session_obj->vocproc_rsp_cnt += 1;

  if ( session_obj->vocproc_rsp_cnt == session_obj->vocproc_q.size )
  {
    /* Individual VOCPROC results are ignored. We depend on logs to detect
     * errors. */
    session_obj->session_ctrl.status = APR_EOK;
    mvm_signal_run(); /* Trigger the session state control to run. */
  }

  ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

static void mvm_vocprocs_get_avsync_delays_group_wait_transition_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_session_object_t* session_obj;
  vss_ivocproc_rsp_get_avsync_delays_t* avsync_vocproc_delays;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &job_obj ) );
  MVM_PANIC_ON_ERROR( rc );
  rc = mvm_get_typed_object( job_obj->context_handle, MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  avsync_vocproc_delays = APRV2_PKT_GET_PAYLOAD( vss_ivocproc_rsp_get_avsync_delays_t, packet );

  /* Extract the vocproc delays from response of a particular vocproc,
   * to normalize these delays cache them to mvm _session_object_t
   * in case they exceed the already cached delays.
   */

  if ( avsync_vocproc_delays->vp_rx_algorithmic_delay >
         session_obj->avsync_vocproc_delay.max_vp_rx_algorithmic_delay )
  {
    session_obj->avsync_vocproc_delay.max_vp_rx_algorithmic_delay =
      avsync_vocproc_delays->vp_rx_algorithmic_delay;
  }

  if ( avsync_vocproc_delays->vp_tx_algorithmic_delay >
         session_obj->avsync_vocproc_delay.max_vp_tx_algorithmic_delay )
  {
    session_obj->avsync_vocproc_delay.max_vp_tx_algorithmic_delay =
      avsync_vocproc_delays->vp_tx_algorithmic_delay;
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "mvm_vocprocs_get_avsync_delays_group_wait_transition_rsp_fn(): " 
         "total_afe_and_vptx_algo_delay = %d, total_afe_and_vprx_algo_delay = %d",
         avsync_vocproc_delays->vp_tx_algorithmic_delay,
         avsync_vocproc_delays->vp_rx_algorithmic_delay );

  /* Complete the current action once all VOCPROCs have responded. */
  session_obj->vocproc_rsp_cnt += 1;

  if ( session_obj->vocproc_rsp_cnt == session_obj->vocproc_q.size )
  {
    /* Individual VOCPROC results are ignored. We depend on logs to detect
     * errors. */
    session_obj->session_ctrl.status = APR_EOK;
    mvm_signal_run(); /* Trigger the session state control to run. */
  }


  ( void ) mvm_free_object( ( mvm_object_t* ) job_obj );
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

static void mvm_vocprocs_set_system_config_group_wait_transition_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  uint32_t max_pp_sr;
  vss_icommon_rsp_set_system_config_t* set_system_config_rsp;
  mvm_simple_job_object_t* job_obj;
  mvm_session_object_t* session_obj;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &job_obj ) );
  MVM_PANIC_ON_ERROR( rc );
  rc = mvm_get_typed_object( job_obj->context_handle, MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  if ( packet->opcode == VSS_ICOMMON_RSP_SET_SYSTEM_CONFIG )
  {
    set_system_config_rsp =
      APRV2_PKT_GET_PAYLOAD( vss_icommon_rsp_set_system_config_t, packet );

    { /* Find the Max PP sampling rate */
     if ( set_system_config_rsp->rx_pp_sr >= set_system_config_rsp->tx_pp_sr )
     {
       max_pp_sr = set_system_config_rsp->rx_pp_sr;
     }
     else
     {
       max_pp_sr = set_system_config_rsp->tx_pp_sr;
     }

     /* Update the vocproc load management variables. */
     if ( max_pp_sr == MVM_NB_SR )
     {
       session_obj->stream_vocproc_load.num_nb_vocprocs += 1;
     }
     else if ( max_pp_sr == MVM_WB_SR )
     {
       session_obj->stream_vocproc_load.num_wb_vocprocs += 1;
     }
     else if ( max_pp_sr == MVM_SWB_SR )
     {
       session_obj->stream_vocproc_load.num_swb_vocprocs += 1;
     }
     else
     {
       session_obj->stream_vocproc_load.num_fb_plus_vocprocs += 1;
     }

      session_obj->stream_vocproc_load.total_vocproc_kpps +=
        ( set_system_config_rsp->vp_rx_kpps + set_system_config_rsp->vp_tx_kpps );
    }

    if ( session_obj->vocproc_q.size == 1 )
    { /* Store the vocproc properties if there is a single vocproc attached to MVM. */
      session_obj->stream_vocproc_load.vocproc_rx_topology_id = set_system_config_rsp->vocproc_rx_topology_id;
      session_obj->stream_vocproc_load.vocproc_tx_topology_id = set_system_config_rsp->vocproc_tx_topology_id;
      session_obj->stream_vocproc_load.vp_rx_kpps = set_system_config_rsp->vp_rx_kpps;
      session_obj->stream_vocproc_load.vp_tx_kpps = set_system_config_rsp->vp_tx_kpps;
      session_obj->stream_vocproc_load.tx_num_channels = set_system_config_rsp->tx_num_channels;
      session_obj->stream_vocproc_load.tx_mpps_scale_factor = set_system_config_rsp->tx_mpps_scale_factor;
      session_obj->stream_vocproc_load.tx_bw_scale_factor = set_system_config_rsp->tx_bw_scale_factor;
      session_obj->stream_vocproc_load.rx_mpps_scale_factor = set_system_config_rsp->rx_mpps_scale_factor;
      session_obj->stream_vocproc_load.rx_bw_scale_factor = set_system_config_rsp->rx_bw_scale_factor;
      session_obj->stream_vocproc_load.tx_afe_port_sr = set_system_config_rsp->tx_afe_port_sr;
      session_obj->stream_vocproc_load.rx_afe_port_sr = set_system_config_rsp->rx_afe_port_sr;
      session_obj->stream_vocproc_load.ec_ref_afe_port_sr = set_system_config_rsp->ec_ref_afe_port_sr;
      session_obj->stream_vocproc_load.vptx_frame_size_us = set_system_config_rsp->vptx_frame_size_us;
    }
  }

  /* Complete the current action once all VOCPROCs have responded. */
  session_obj->vocproc_rsp_cnt += 1;

  if ( session_obj->vocproc_rsp_cnt == session_obj->vocproc_q.size )
  {
    /* Individual VOCPROC results are ignored. We depend on logs to detect
     * errors.*/
    session_obj->session_ctrl.status = APR_EOK;
    mvm_signal_run(); /* Trigger the session state control to run. */
  }

  ( void ) mvm_free_object( ( mvm_object_t* ) job_obj );
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

static void mvm_vocprocs_hdvoice_config_group_wait_transition_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  vss_icommon_cmd_set_system_config_t prev_system_config;
  vss_ihdvoice_rsp_get_config_t* get_hdvoice_config_rsp;
  mvm_simple_job_object_t* simple_obj;
  mvm_session_object_t* session_obj;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &simple_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  rc = mvm_get_typed_object( simple_obj->context_handle, MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  prev_system_config.feature = session_obj->system_config.feature;
  prev_system_config.dec_sr = session_obj->system_config.dec_sr;

  get_hdvoice_config_rsp = 
    APRV2_PKT_GET_PAYLOAD( vss_ihdvoice_rsp_get_config_t, packet );

  /* Determine ENC/DEC/VPTX/VPRX sample rate value assuming FEATURE_NONE. */
  session_obj->system_config.feature = VSS_ICOMMON_CAL_FEATURE_NONE;
  rc = mvm_determine_target_sampling_rates( session_obj,
         session_obj->requested_var_voc_rx_sampling_rate,
         session_obj->requested_var_voc_tx_sampling_rate );
  MVM_REPORT_FATAL_ON_ERROR( rc );

  /* Evaluate the HD Voice config response, and update the sample rates
    according to enabled fearture. */
  if ( ( get_hdvoice_config_rsp->enable_mode == VSS_PARAM_FEATURE_FORCED_ON ) ||
       ( ( get_hdvoice_config_rsp->enable_mode == VSS_PARAM_FEATURE_DEFAULT_ON ) && ( session_obj->is_hdvoice_ui_enabled != MVM_HDVOICE_UI_DISABLED ) ) ||
       ( ( get_hdvoice_config_rsp->enable_mode == VSS_PARAM_FEATURE_DEFAULT_OFF ) && ( session_obj->is_hdvoice_ui_enabled == MVM_HDVOICE_UI_ENABLED ) ) )
  {
    if ( ( ( get_hdvoice_config_rsp->feature_id == VSS_ICOMMON_CAL_FEATURE_WIDEVOICE2 ) && ( session_obj->is_wv2_enabled == TRUE ) ) ||
         ( get_hdvoice_config_rsp->feature_id == VSS_ICOMMON_CAL_FEATURE_WIDEVOICE ) )
    {
      session_obj->system_config.feature = get_hdvoice_config_rsp->feature_id;
        session_obj->system_config.rx_pp_sr = get_hdvoice_config_rsp->rx_pp_sr;
      }
    else if ( ( get_hdvoice_config_rsp->feature_id == VSS_ICOMMON_CAL_FEATURE_BEAMR ) && ( session_obj->is_beamr_enabled == TRUE ) )
    {
      session_obj->system_config.feature = get_hdvoice_config_rsp->feature_id;
        session_obj->system_config.rx_pp_sr = get_hdvoice_config_rsp->rx_pp_sr;
        session_obj->system_config.dec_sr = get_hdvoice_config_rsp->rx_pp_sr;
      }
    else if ( get_hdvoice_config_rsp->feature_id == VSS_ICOMMON_CAL_FEATURE_CUSTOM_BWE )
    {
      session_obj->system_config.feature = get_hdvoice_config_rsp->feature_id;
      session_obj->system_config.rx_pp_sr = get_hdvoice_config_rsp->rx_pp_sr;
      session_obj->system_config.tx_pp_sr = get_hdvoice_config_rsp->tx_pp_sr;
    }
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: GET_HDVOICE_RSP: previous_feature:0x%08X, current_feature:0x%08X",
         prev_system_config.feature, session_obj->system_config.feature );

  /* Flag stream reconfig given any chnage in decoder sample rate and needs
   * entire RX voice chain to be reconfigured. */
  if ( prev_system_config.dec_sr != session_obj->system_config.dec_sr )
  {
    session_obj->is_stream_reconfig_required = TRUE;
  }

  /* Complete the current action once all VOCPROCs have responded. */
  session_obj->vocproc_rsp_cnt += 1;

  if ( session_obj->vocproc_rsp_cnt == session_obj->vocproc_q.size )
  {
    /* Individual VOCPROC results are ignored. We depend on logs to detect
     * errors. */
    session_obj->session_ctrl.status = APR_EOK;
    mvm_signal_run(); /* Trigger the session state control to run. */
  }

  ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

static void mvm_detach_all_wait_transition_result_rsp_fn (
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_session_object_t* session_obj;

  rc = mvm_get_typed_object( packet->token, MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                             ( ( mvm_object_t** ) &job_obj ) );
  MVM_PANIC_ON_ERROR( rc );
  rc = mvm_get_typed_object( job_obj->context_handle, MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  /* Complete the current action once all VOCPROCs have responded. */
  session_obj->detach_all_rsp_cnt++;

  if ( session_obj->detach_all_rsp_cnt ==
       ( session_obj->stream_q.size * session_obj->vocproc_q.size ) )
  {
    /* Individual results are ignored. We depend on logs to detect
     * errors. */
    session_obj->session_ctrl.status = APR_EOK;
    mvm_signal_run(); /* Trigger the session state control to run. */
  }

  ( void ) mvm_free_object( ( mvm_object_t* ) job_obj );
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}


/****************************************************************************
 * MVM ACTION FUNCTION HANDLERS                                             *
 ****************************************************************************/

static int32_t mvm_action_stream_encoder_reset (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;

  session_obj->stream_rsp_cnt = 0;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break; /* break out of for loop. */

    rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
    MVM_PANIC_ON_ERROR( rc );
    job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
      mvm_streams_group_wait_transition_result_rsp_fn;

    rc = __aprv2_cmd_alloc_send(
           mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
           mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
           job_obj->header.handle, VSS_ISTREAM_CMD_ENCODER_RESET, NULL, 0 );
    MVM_COMM_ERROR( rc, mvm_cvs_addr );
  }

  return APR_EOK;
}

static int32_t mvm_action_reinit_streams (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;

  session_obj->is_stream_reinit_required = FALSE;
  session_obj->stream_rsp_cnt = 0;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    {
      rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
      MVM_PANIC_ON_ERROR( rc );
      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_streams_group_wait_transition_result_rsp_fn;

      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
             job_obj->header.handle, VSS_ISTREAM_CMD_REINIT, NULL, 0 );
      MVM_COMM_ERROR( rc, mvm_cvs_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_set_tty_mode_to_streams (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* simple_obj;
  mvm_generic_item_t* generic_item;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    /* Note: TTY module affect the overall stream KPPS hence the TTY mode must 
     * be set on VPM before calling set_system config so that VSM can get the
     * correct KPPS from Dynamic Services.*/
    rc = mvm_create_simple_job_object( session_obj->header.handle, &simple_obj );
    MVM_PANIC_ON_ERROR( rc );

    simple_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
      mvm_simple_self_destruct_result_rsp_fn;

    rc = __aprv2_cmd_alloc_send(
           mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
           mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
           simple_obj->header.handle, VSS_ITTY_CMD_SET_TTY_MODE,
           &session_obj->tty_mode, sizeof( session_obj->tty_mode ) );
    MVM_COMM_ERROR( rc, mvm_cvs_addr );
  }

  return APR_EOK;
}

static int32_t mvm_action_set_system_config_to_streams (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;

  session_obj->stream_rsp_cnt = 0;
  session_obj->system_config.call_num = mvm_debug_call_num;

  /* Reconfigure all attached streams. */
  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;
    {
      /* Apply current system configuration settings (network id, voc class,
       * sample rates, vsid, vfr_mode) on all attached streams. Create a new
       * result response function to count the number of responses. The result
       * response function completes the current action when the number of
       * responses matches the number of attached streams. This method has
       * maximum parallelization. The solution doesn't keep track of individual
       * stream's responses so it's required search through the logs when stuck
       * in the same action for a long time.
       */
      rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
      MVM_PANIC_ON_ERROR( rc );

      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_streams_set_system_config_group_wait_transition_rsp_fn;

      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_SET_SYSTEM_CONFIG ] =
        mvm_streams_set_system_config_group_wait_transition_rsp_fn;

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "MVM: STREAM_SET_SYSTEM_CONFIG: Network: 0x%08X, Media ID: 0x%08X",
             session_obj->system_config.network_id,
             session_obj->system_config.media_id );

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, 
             "MVM: STREAM_SET_SYSTEM_CONFIG: Tx voc op mode: 0x%08X, Rx voc op "
             "mode: 0x%08X", session_obj->system_config.tx_voc_op_mode,
             session_obj->system_config.rx_voc_op_mode );

      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "MVM: STREAM_SET_SYSTEM_CONFIG: Feature:0x%08X. ENC PP SR: %d, DEC "
             "PP SR: %d", session_obj->system_config.feature,
             session_obj->system_config.enc_sr,
             session_obj->system_config.dec_sr );

      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
             job_obj->header.handle, VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG,
             &session_obj->system_config, sizeof( session_obj->system_config ) );
      MVM_COMM_ERROR( rc, mvm_cvs_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_enable_streams (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;

  if ( session_obj == NULL )
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );

  session_obj->stream_rsp_cnt = 0;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    {
      rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
      MVM_PANIC_ON_ERROR( rc );
      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_streams_group_wait_transition_result_rsp_fn;

      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
             job_obj->header.handle, VSS_ISTREAM_CMD_ENABLE, NULL, 0 );
      MVM_COMM_ERROR( rc, mvm_cvs_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_disable_streams (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;

  if ( session_obj == NULL )
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );

  session_obj->stream_rsp_cnt = 0;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    {
      rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
      MVM_PANIC_ON_ERROR( rc );
      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_streams_group_wait_transition_result_rsp_fn;

      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
             job_obj->header.handle, VSS_ISTREAM_CMD_DISABLE, NULL, 0 );
      MVM_COMM_ERROR( rc, mvm_cvs_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_reinit_vocprocs (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;

  session_obj->is_vocproc_reinit_required = FALSE;
  session_obj->vocproc_rsp_cnt = 0;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->vocproc_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    {
      rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
      MVM_PANIC_ON_ERROR( rc );
      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_vocprocs_group_wait_transition_result_rsp_fn;

      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_vpm_addr, ( ( uint16_t ) generic_item->handle ),
             job_obj->header.handle, VSS_IVOCPROC_CMD_REINIT, NULL, 0 );
      MVM_COMM_ERROR( rc, mvm_vpm_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_set_system_config_to_vocprocs (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;

  session_obj->system_config.call_num = mvm_debug_call_num;

  session_obj->vocproc_rsp_cnt = 0;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->vocproc_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
    MVM_PANIC_ON_ERROR( rc );

    job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
      mvm_vocprocs_group_wait_transition_result_rsp_fn;

    job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_SET_SYSTEM_CONFIG ] =
      mvm_vocprocs_set_system_config_group_wait_transition_rsp_fn;

    MSG_7( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "mvm_action_set_system_config_to_vocprocs: Network: 0x%08X, " \
           "Media ID: 0x%08X, Tx voc op mode: 0x%08X, Rx voc op mode: 0x%08X," \
           "Feature: 0x%08X, TX PP SR: %d, RX PP SR: %d",
           session_obj->system_config.network_id, session_obj->system_config.media_id,
           session_obj->system_config.tx_voc_op_mode, session_obj->system_config.rx_voc_op_mode,
           session_obj->system_config.feature, session_obj->system_config.tx_pp_sr,
           session_obj->system_config.rx_pp_sr );

    rc = __aprv2_cmd_alloc_send(
           mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
           mvm_vpm_addr, ( ( uint16_t ) generic_item->handle ),
           job_obj->header.handle, VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG,
           &session_obj->system_config,
           sizeof( session_obj->system_config ) );
    MVM_COMM_ERROR( rc, mvm_vpm_addr );
  }

  return APR_EOK;
}

static int32_t mvm_action_set_dymamic_system_config (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* simple_obj;
  mvm_generic_item_t* generic_item;
  vss_icommon_cmd_set_dynamic_system_config_t dynamic_system_config;

  session_obj->vocproc_rsp_cnt = 0;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->vocproc_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    rc = mvm_create_simple_job_object( session_obj->header.handle, &simple_obj );
    MVM_PANIC_ON_ERROR( rc );
    simple_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
      mvm_vocproc_dynamic_reconfig_result_count_rsp_fn;

    dynamic_system_config.rx_voc_op_mode = session_obj->system_config.rx_voc_op_mode;
    dynamic_system_config.tx_voc_op_mode = session_obj->system_config.tx_voc_op_mode;
    dynamic_system_config.feature_id = session_obj->system_config.feature;
    dynamic_system_config.rx_pp_sr = session_obj->system_config.rx_pp_sr;

    rc = __aprv2_cmd_alloc_send(
           mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
           mvm_vpm_addr, ( ( uint16_t ) generic_item->handle ),
           simple_obj->header.handle, VSS_ICOMMON_CMD_SET_DYNAMIC_SYSTEM_CONFIG,
           &dynamic_system_config, sizeof( dynamic_system_config ) );
    MVM_COMM_ERROR( rc, mvm_vpm_addr );
  }

  return APR_EOK;
}

static int32_t mvm_action_enable_vocprocs (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;

  session_obj->vocproc_rsp_cnt = 0;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->vocproc_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    {
      rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
      MVM_PANIC_ON_ERROR( rc );
      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_vocprocs_group_wait_transition_result_rsp_fn;

      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_vpm_addr, ( ( uint16_t ) generic_item->handle ),
             job_obj->header.handle,
             VSS_IVOCPROC_CMD_MVM_ENABLE, NULL, 0 );
      MVM_COMM_ERROR( rc, mvm_vpm_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_disable_vocprocs (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;

  /* Send disable command to the attached VOCPROCs. Create a new result
   * response function to count the number of responses. The result
   * response function completes the current action when the number of
   * responses matches the number of attached VOCPROCs. This method
   * has maximum parallelization. The solution doesn't keep track of
   * individual VOCPROCs responses so it's required search through the
   * logs when stuck in the same action for a long time.
   */
  session_obj->vocproc_rsp_cnt = 0; /* Restart the response counter. */

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->vocproc_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    {
      rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
      MVM_PANIC_ON_ERROR( rc );
      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_vocprocs_group_wait_transition_result_rsp_fn;

      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_vpm_addr, ( ( uint16_t ) generic_item->handle ),
             job_obj->header.handle,
             VSS_IVOCPROC_CMD_MVM_DISABLE, NULL, 0 );
      MVM_COMM_ERROR( rc, mvm_vpm_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_get_vocproc_avsync_delays (
  mvm_session_object_t *session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;

  session_obj->vocproc_rsp_cnt = 0;

  /* Clear the vocproc delays. */
  session_obj->avsync_vocproc_delay.max_vp_tx_algorithmic_delay = 0;
  session_obj->avsync_vocproc_delay.max_vp_rx_algorithmic_delay = 0;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->vocproc_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    {
      rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
      MVM_PANIC_ON_ERROR( rc );
      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_vocprocs_group_wait_transition_result_rsp_fn;
      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_GET_AVSYNC_DELAYS ] =
        mvm_vocprocs_get_avsync_delays_group_wait_transition_rsp_fn;

      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_vpm_addr, ( ( uint16_t ) generic_item->handle ),
             job_obj->header.handle, VSS_IVOCPROC_CMD_GET_AVSYNC_DELAYS,
             NULL, 0 );
      MVM_COMM_ERROR( rc, mvm_vpm_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_calculate_avsync_delays (
  mvm_session_object_t * session_obj
)
{
  uint16_t vptx_frame_size = session_obj->stream_vocproc_load.vptx_frame_size_us;

  session_obj->avsync_vocproc_delay.vp_rx_normalized_total_delay =
    session_obj->avsync_vocproc_delay.max_vp_rx_algorithmic_delay +
    session_obj->voice_proc_delays.vdec_proc +
    session_obj->voice_proc_delays.vprx_proc;

  if ( TRUE == session_obj->voice_timing.is_timing_ref_available )
  {
    session_obj->avsync_vocproc_delay.vp_rx_normalized_total_delay +=
      MVM_DEFAULT_DEC_REQ_OFFSET_MARGIN_US + MVM_DEFAULT_DEC_OFFSET_MARGIN_US;
  }

  session_obj->avsync_vocproc_delay.vp_tx_normalized_total_delay =
    session_obj->avsync_vocproc_delay.max_vp_tx_algorithmic_delay +
    session_obj->voice_proc_delays.venc_proc +
    ( session_obj->voice_proc_delays.vptx_proc/(MVM_VOICE_FRAME_SIZE_US/vptx_frame_size) );

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: CALCULATE_PATH_DELAYS: voice_tx_path_delay(excluding enc algo delay): %d us, "
         "voice_rx_path_delay(excluding dec algo delay): %d us",
         session_obj->avsync_vocproc_delay.vp_tx_normalized_total_delay,
         session_obj->avsync_vocproc_delay.vp_rx_normalized_total_delay);

  return APR_EOK;
}

static int32_t mvm_action_stream_set_avsync_delays (
  mvm_session_object_t *session_obj
)
{
  int32_t rc = APR_EOK;
  mvm_generic_item_t* generic_item;
  mvm_simple_job_object_t *job_obj;
  vss_istream_cmd_set_vocproc_avsync_delays_t avsync_delays;

  /* Get the normalised delays stored in MVM. */
  avsync_delays.vp_rx_normalized_total_delay = session_obj->avsync_vocproc_delay.vp_rx_normalized_total_delay;
  avsync_delays.vp_tx_normalized_total_delay = session_obj->avsync_vocproc_delay.vp_tx_normalized_total_delay;

  session_obj->stream_rsp_cnt = 0; /* Restart the response counter. */
  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );

  if ( session_obj->stream_q.size == 0 )
  {
    session_obj->session_ctrl.status = APR_EOK;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "mvm_action_stream_set_avsync_delays(): Attached stream count=(%d)",
           session_obj->stream_q.size );
    return rc;
  }

  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    {
      rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
      MVM_PANIC_ON_ERROR( rc );

      job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_streams_group_wait_transition_result_rsp_fn;

      /* Send the normalized vocproc delays to all attached streams. */
      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
             job_obj->header.handle, VSS_ISTREAM_CMD_SET_VOCPROC_AVSYNC_DELAYS,
             &avsync_delays, sizeof( avsync_delays ) );
      MVM_COMM_ERROR( rc, mvm_cvs_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_clean_up_stream_vocproc_lists (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_generic_item_t* generic_item;

  /* Clean up the stream and vocproc lists. */
  while ( session_obj->stream_q.size > 0 )
  {
    rc = apr_list_remove_head( &session_obj->stream_q,
                                   ( ( apr_list_node_t** ) &generic_item ) );
    MVM_PANIC_ON_ERROR( rc );
    ( void ) apr_list_add_tail( &session_obj->free_item_q, &generic_item->link );
  }

  while ( session_obj->vocproc_q.size > 0 )
  {
    rc = apr_list_remove_head( &session_obj->vocproc_q,
                               ( ( apr_list_node_t** ) &generic_item ) );
    MVM_PANIC_ON_ERROR( rc );
    ( void ) apr_list_add_tail( &session_obj->free_item_q, &generic_item->link );
  }

  return APR_EOK;
}

static int32_t mvm_action_reset_vocproc_load_count (
  mvm_session_object_t* session_obj
)
{
  session_obj->stream_vocproc_load.num_nb_vocprocs = 0;
  session_obj->stream_vocproc_load.num_wb_vocprocs = 0;
  session_obj->stream_vocproc_load.num_swb_vocprocs = 0;
  session_obj->stream_vocproc_load.num_fb_plus_vocprocs = 0;

  session_obj->stream_vocproc_load.total_vocproc_kpps = 0;

  session_obj->stream_vocproc_load.tx_num_channels = 0;

  session_obj->stream_vocproc_load.tx_mpps_scale_factor = 0;
  session_obj->stream_vocproc_load.tx_bw_scale_factor = 0;
  session_obj->stream_vocproc_load.rx_mpps_scale_factor = 0;
  session_obj->stream_vocproc_load.rx_bw_scale_factor = 0;
  
  session_obj->stream_vocproc_load.vocproc_tx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;
  session_obj->stream_vocproc_load.vocproc_rx_topology_id = VSS_IVOCPROC_TOPOLOGY_ID_NONE;

  session_obj->stream_vocproc_load.vptx_frame_size_us = 0;

  return APR_EOK;
}


static int32_t mvm_action_reset_stream_load_count (
  mvm_session_object_t* session_obj
)
{
  session_obj->stream_vocproc_load.num_nb_streams = 0;
  session_obj->stream_vocproc_load.num_wb_streams = 0;
  session_obj->stream_vocproc_load.num_swb_streams = 0;
  session_obj->stream_vocproc_load.num_fb_plus_streams = 0;

  session_obj->stream_vocproc_load.total_stream_kpps = 0;

  session_obj->stream_vocproc_load.stream_media_id = VSS_MEDIA_ID_NONE;

  return APR_EOK;
}

static int32_t mvm_action_ccm_update_session_config (
  mvm_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vss_iccm_mvm_voice_session_update_t evt_voice_session_active;

  evt_voice_session_active.num_nb_streams =
    session_obj->stream_vocproc_load.num_nb_streams;

  evt_voice_session_active.num_wb_streams =
    session_obj->stream_vocproc_load.num_wb_streams;
  
  evt_voice_session_active.num_swb_streams =
    session_obj->stream_vocproc_load.num_swb_streams;
  
  evt_voice_session_active.num_fb_plus_streams =
    session_obj->stream_vocproc_load.num_fb_plus_streams;
  
  evt_voice_session_active.num_nb_vocprocs =
    session_obj->stream_vocproc_load.num_nb_vocprocs;
  
  evt_voice_session_active.num_wb_vocprocs =
    session_obj->stream_vocproc_load.num_wb_vocprocs;
  
  evt_voice_session_active.num_swb_vocprocs =
    session_obj->stream_vocproc_load.num_swb_vocprocs;
  
  evt_voice_session_active.num_fb_plus_vocprocs =
    session_obj->stream_vocproc_load.num_fb_plus_vocprocs;
  
  evt_voice_session_active.enc_kpps = session_obj->stream_vocproc_load.enc_kpps;
  evt_voice_session_active.dec_kpps = session_obj->stream_vocproc_load.dec_kpps;
  evt_voice_session_active.dec_pp_kpps = session_obj->stream_vocproc_load.dec_pp_kpps;
  evt_voice_session_active.vp_rx_kpps = session_obj->stream_vocproc_load.vp_rx_kpps;
  evt_voice_session_active.vp_tx_kpps = session_obj->stream_vocproc_load.vp_tx_kpps;

  evt_voice_session_active.total_kpps =
    session_obj->stream_vocproc_load.total_stream_kpps +
    session_obj->stream_vocproc_load.total_vocproc_kpps;
  
  evt_voice_session_active.tx_afe_port_sr = session_obj->stream_vocproc_load.tx_afe_port_sr;

  evt_voice_session_active.tx_topology_id =
    session_obj->stream_vocproc_load.vocproc_tx_topology_id;
  
  evt_voice_session_active.rx_topology_id =
    session_obj->stream_vocproc_load.vocproc_rx_topology_id;
  
  evt_voice_session_active.media_id =
    session_obj->stream_vocproc_load.stream_media_id;

  evt_voice_session_active.vfr_mode =
    session_obj->vfr_info.vfr_mode;

  evt_voice_session_active.tx_num_channels =
    session_obj->stream_vocproc_load.tx_num_channels;

  evt_voice_session_active.tx_mpps_scale_factor =
    session_obj->stream_vocproc_load.tx_mpps_scale_factor;
  evt_voice_session_active.tx_bw_scale_factor =
    session_obj->stream_vocproc_load.tx_bw_scale_factor;
  evt_voice_session_active.rx_mpps_scale_factor =
    session_obj->stream_vocproc_load.rx_mpps_scale_factor;
  evt_voice_session_active.rx_bw_scale_factor =
    session_obj->stream_vocproc_load.rx_bw_scale_factor;
  
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "mvm_action_ccm_update_session_config(): total_stream_kpps = %d, "
         "total_vocproc_kpps = %d, total_kpps = %d",
         session_obj->stream_vocproc_load.total_stream_kpps,
         session_obj->stream_vocproc_load.total_vocproc_kpps,
         evt_voice_session_active.total_kpps);

  ( void ) mvm_ccm_update_voice_session_config( session_obj->header.handle,
                                                &evt_voice_session_active );
  
  return rc;
}

static int32_t mvm_action_csm_config_update (
  mvm_session_object_t* session_obj
)
{
  vss_icsm_mvm_voice_config_update_t csm_config;

  csm_config.voice_timing = session_obj->voice_timing;
  csm_config.processing_times = session_obj->voice_proc_delays;
  csm_config.vfr_info = session_obj->vfr_info;
  csm_config.vptx_frame_size = session_obj->stream_vocproc_load.vptx_frame_size_us;

  ( void ) mvm_csm_update_voice_session_config( session_obj->header.handle,
                                                &csm_config );
  
  return APR_EOK;
}

//<mailBox>
static int32_t mvm_action_evaluate_timing_params (
  mvm_session_object_t* session_obj
)
{
  uint32_t enc_offset, dec_req_offset, dec_offset;
  uint32_t vfr_offset, client_tx_offset, client_rx_offset;

  vfr_offset = session_obj->vfr_info.vfr_start_reference_us % session_obj->vfr_info.vfr_cycle_duration_us;
  client_tx_offset = session_obj->voice_timing.tx_ref_timestamp_us % session_obj->vfr_info.vfr_cycle_duration_us;
  client_rx_offset = session_obj->voice_timing.rx_ref_timestamp_us % session_obj->vfr_info.vfr_cycle_duration_us;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: EVALUATE_TIMING_PARAMS: vfr_offset_from cdrx:(%d), txRefTime_offset:(%d), rxRefTime_offset:(%d)",
         vfr_offset, client_tx_offset, client_rx_offset );

  { /* Encoder offset calculation. */
    if ( client_tx_offset >= vfr_offset )
    {
      enc_offset = ( client_tx_offset - vfr_offset );
    }
    else
    {
      enc_offset = ( session_obj->vfr_info.vfr_cycle_duration_us + client_tx_offset - vfr_offset );
    }

    enc_offset += session_obj->vfr_info.vfr_cycle_duration_us;
    enc_offset = ( enc_offset - MVM_DEFAULT_ENC_OFFSET_MARGIN_US ) % session_obj->vfr_info.vfr_cycle_duration_us;
  }

  { /* Decoder request offset calculation. */
    if ( client_rx_offset >= vfr_offset )
    {
      dec_req_offset = ( client_rx_offset - vfr_offset );
    }
    else
    {
      dec_req_offset =
        ( session_obj->vfr_info.vfr_cycle_duration_us + client_rx_offset - vfr_offset );
    }

    dec_req_offset += MVM_DEFAULT_DEC_REQ_OFFSET_MARGIN_US;

    if ( dec_req_offset >= session_obj->vfr_info.vfr_cycle_duration_us )
    {
      dec_req_offset -= session_obj->vfr_info.vfr_cycle_duration_us;
    }
  }

  { /* Decoder offset calculation. */
    dec_offset = dec_req_offset + MVM_DEFAULT_DEC_OFFSET_MARGIN_US;

    if ( dec_offset >= session_obj->vfr_info.vfr_cycle_duration_us )
    {
      dec_offset -= session_obj->vfr_info.vfr_cycle_duration_us;
    }
  }

  /* Populate the evaluated timing params. */
  session_obj->voice_timing.enc_offset = enc_offset;
  session_obj->voice_timing.dec_req_offset = dec_req_offset;
  session_obj->voice_timing.dec_offset = dec_offset;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: EVALUATE_TIMING_PARAMS: enc_offset:(%d) dec_req_offset:(%d) dec_offset:(%d)",
         enc_offset, dec_req_offset, dec_offset );

  return APR_EOK;
}

static int32_t mvm_action_calculate_timing_offsets (
  mvm_session_object_t* session_obj
)
{
  for ( ;; )
  {
    ( void ) mvm_calculate_timing_offsets( session_obj );

    break;
  }

  return APR_EOK;
}

static int32_t mvm_action_set_vocproc_timing (
  mvm_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vss_ivocproc_cmd_set_voice_timing_t set_voice_timing;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;
  mvm_voice_timing_t* target_timing;

  target_timing = &session_obj->voice_timing;

  /* Send set_timing command to the attached VOCPROCs. Create a new result
   * response function to count the number of responses. The result
   * response function completes the current action when the number of
   * responses matches the number of attached VOCPROCs. This method
   * has maximum parallelization. The solution doesn't keep track of
   * individual VOCPROC's response so it's required search through the
   * logs when stuck in the same action for a long time.
   */
  session_obj->vocproc_rsp_cnt = 0;

  set_voice_timing.vsid = session_obj->vfr_info.vsid;
  set_voice_timing.vfr_mode = session_obj->vfr_info.vfr_mode;
  set_voice_timing.vfr_cycle_duration_us = session_obj->vfr_info.vfr_cycle_duration_us;
  
  set_voice_timing.enc_offset = session_obj->voice_timing.enc_offset;
  set_voice_timing.dec_req_offset = session_obj->voice_timing.dec_req_offset;
  set_voice_timing.dec_offset = session_obj->voice_timing.dec_offset;
  set_voice_timing.vprx_delivery_offset = session_obj->voice_timing.vprx_delivery_offset;
  set_voice_timing.vptx_buf_start_offset = session_obj->voice_timing.vptx_buf_start_offset;
  set_voice_timing.vptx_proc_start_offset = session_obj->voice_timing.vptx_proc_start_offset;
  set_voice_timing.vptx_delivery_offset = session_obj->voice_timing.vptx_delivery_offset;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->vocproc_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
    MVM_PANIC_ON_ERROR( rc );
    job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
      mvm_vocprocs_group_wait_transition_result_rsp_fn;

    rc = __aprv2_cmd_alloc_send(
           mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
           mvm_vpm_addr, ( ( uint16_t ) generic_item->handle ),
           job_obj->header.handle, VSS_IVOCPROC_CMD_SET_VOICE_TIMING,
           &set_voice_timing, sizeof( set_voice_timing ) );
    MVM_COMM_ERROR( rc, mvm_vpm_addr );
  }

  return APR_EOK;
}

static int32_t mvm_action_set_stream_timing (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  vss_istream_cmd_set_voice_timing_t set_voice_timing;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;
  mvm_voice_timing_t* target_timing;

  target_timing = &session_obj->voice_timing;

  /* Send set_timing command to the attached streams. Create a new result
   * response function to count the number of responses. The result
   * response function completes the current action when the number of
   * responses matches the number of attached streams. This method
   * has maximum parallelization. The solution doesn't keep track of
   * individual stream's response so it's required search through the
   * logs when stuck in the same action for a long time.
   */
  session_obj->stream_rsp_cnt = 0;

  set_voice_timing.vsid = session_obj->vfr_info.vsid;
  set_voice_timing.vfr_mode = session_obj->vfr_info.vfr_mode;
  set_voice_timing.vfr_cycle_duration_us = session_obj->vfr_info.vfr_cycle_duration_us;

  set_voice_timing.enc_offset = session_obj->voice_timing.enc_offset;
  set_voice_timing.dec_req_offset = session_obj->voice_timing.dec_req_offset;
  set_voice_timing.dec_offset = session_obj->voice_timing.dec_offset;
  set_voice_timing.vprx_delivery_offset = session_obj->voice_timing.vprx_delivery_offset;
  set_voice_timing.vptx_buf_start_offset = session_obj->voice_timing.vptx_buf_start_offset;
  set_voice_timing.vptx_proc_start_offset = session_obj->voice_timing.vptx_proc_start_offset;
  set_voice_timing.vptx_delivery_offset = session_obj->voice_timing.vptx_delivery_offset;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
    MVM_PANIC_ON_ERROR( rc );
    job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
      mvm_streams_group_wait_transition_result_rsp_fn;

    rc = __aprv2_cmd_alloc_send(
           mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
           mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
           job_obj->header.handle, VSS_ISTREAM_CMD_SET_VOICE_TIMING,
           &set_voice_timing, sizeof( set_voice_timing ) );
    MVM_COMM_ERROR( rc, mvm_cvs_addr );
  }

  return APR_EOK;
}

static int32_t mvm_action_get_hdvoice_config(
  mvm_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;
  vss_ihdvoice_cmd_get_config_t get_hdvoice_config;

  session_obj->vocproc_rsp_cnt = 0;

  get_hdvoice_config.media_id = session_obj->system_config.media_id;
  get_hdvoice_config.network_id = session_obj->system_config.network_id;
  get_hdvoice_config.rx_voc_op_mode = session_obj->system_config.rx_voc_op_mode;
  get_hdvoice_config.tx_voc_op_mode = session_obj->system_config.tx_voc_op_mode;
  get_hdvoice_config.feature_id = session_obj->system_config.feature;
  get_hdvoice_config.rx_pp_sr = session_obj->system_config.dec_sr;
  get_hdvoice_config.tx_pp_sr = session_obj->system_config.enc_sr;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "mvm_action_get_hdvoice_config(): MediaID = %08x, "
         "NetworkID = %08x, Rx op mode = %08x, Tx op mode = %08x",
         session_obj->system_config.media_id,
         session_obj->system_config.network_id,
         session_obj->system_config.rx_voc_op_mode,
         session_obj->system_config.tx_voc_op_mode );

  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->vocproc_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
    MVM_PANIC_ON_ERROR( rc );

    job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
      mvm_vocprocs_group_wait_transition_result_rsp_fn;

    job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_GET_HDVOICE_CONFIG ] =
      mvm_vocprocs_hdvoice_config_group_wait_transition_rsp_fn;

    rc = __aprv2_cmd_alloc_send(
            mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
            session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
            mvm_vpm_addr, ( ( uint16_t ) generic_item->handle ),
            job_obj->header.handle, VSS_IHDVOICE_CMD_GET_CONFIG,
            &get_hdvoice_config, sizeof( get_hdvoice_config ) );
    MVM_COMM_ERROR( rc, mvm_vpm_addr );
  }

  return APR_EOK;
}

static int32_t mvm_action_set_hdvoice_to_streams ( 
  mvm_session_object_t* session_obj 
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;
  vss_ihdvoice_cmd_set_config_t set_hdvoice_config;

  session_obj->stream_rsp_cnt = 0;
  set_hdvoice_config.feature_id = session_obj->system_config.feature;

  /* Send HD Voice config to the attached streamss. */ 
  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy ); 
  for ( ;; )  
  {  
    rc = apr_list_get_next( &session_obj->stream_q, ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
    MVM_PANIC_ON_ERROR( rc );    
    job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] = 
      mvm_streams_group_wait_transition_result_rsp_fn;    

    rc = __aprv2_cmd_alloc_send(          
           mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
           mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
           job_obj->header.handle, VSS_IHDVOICE_CMD_SET_CONFIG,
           &set_hdvoice_config, sizeof( set_hdvoice_config ) );
    MVM_COMM_ERROR( rc, mvm_cvs_addr );
  } 

  return APR_EOK;
}

static int32_t mvm_action_set_hdvoice_to_vocprocs (
  mvm_session_object_t* session_obj
)
{ 
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;
  vss_ihdvoice_cmd_set_config_t set_hdvoice_config;

  session_obj->vocproc_rsp_cnt = 0;
  set_hdvoice_config.feature_id = session_obj->system_config.feature;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );
  for ( ;; )
  {    
    rc = apr_list_get_next( &session_obj->vocproc_q, ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) ); 
    if ( rc ) break; 

    rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
    MVM_PANIC_ON_ERROR( rc );
    job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] = 
      mvm_vocprocs_group_wait_transition_result_rsp_fn;

    rc = __aprv2_cmd_alloc_send(
           mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           session_obj->self_addr, ((uint16_t)session_obj->header.handle), 
           mvm_vpm_addr, ((uint16_t)generic_item->handle),  
           job_obj->header.handle, VSS_IHDVOICE_CMD_SET_CONFIG,
           &set_hdvoice_config, sizeof(set_hdvoice_config));
    MVM_COMM_ERROR(rc, mvm_vpm_addr); 
  }

  return APR_EOK;
}

static int32_t mvm_action_reconfig_stream (
  mvm_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* job_obj = NULL;

  session_obj->is_stream_reconfig_required = TRUE;

  rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
  MVM_PANIC_ON_ERROR( rc );

  job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
    mvm_simple_self_destruct_result_rsp_fn;

  rc = __aprv2_cmd_alloc_send(
         mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
         job_obj->header.handle, VSS_IMVM_CMD_RECONFIG,
         NULL, 0 );
  MVM_COMM_ERROR( rc, session_obj->self_addr );

  return APR_EOK;
}

static int32_t mvm_action_reconfig_vocproc (
  mvm_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* job_obj = NULL;

  session_obj->is_vocproc_reconfig_required = TRUE;

  rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
  MVM_PANIC_ON_ERROR( rc );

  job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
    mvm_simple_self_destruct_result_rsp_fn;

  rc = __aprv2_cmd_alloc_send(
         mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
         job_obj->header.handle, VSS_IMVM_CMD_RECONFIG,
         NULL, 0 );
  MVM_COMM_ERROR( rc, session_obj->self_addr );

  return APR_EOK;
}

static int32_t mvm_action_dynamic_reconfig (
  mvm_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* job_obj = NULL;

  rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
  MVM_PANIC_ON_ERROR( rc );

  job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =  
    mvm_simple_self_destruct_result_rsp_fn;
  
  rc = __aprv2_cmd_alloc_send(
         mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
         job_obj->header.handle, VSS_IMVM_CMD_DYNAMIC_RECONFIG,
         NULL, 0 );
  MVM_COMM_ERROR( rc, session_obj->self_addr );

  return APR_EOK;
}

static int32_t mvm_action_detach_vocprocs_from_streams (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item_1;
  mvm_generic_item_t* generic_item_2;
  vss_istream_cmd_detach_vocproc_t stream_detach_vocproc;

  session_obj->detach_all_rsp_cnt = 0;

  generic_item_1 = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                              ( ( apr_list_node_t* ) generic_item_1 ),
                              ( ( apr_list_node_t** ) &generic_item_1 ) );
    if ( rc ) break;

    generic_item_2 = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );
    for ( ;; )
    {
      rc = apr_list_get_next( &session_obj->vocproc_q,
                              ( ( apr_list_node_t* ) generic_item_2 ),
                              ( ( apr_list_node_t** ) &generic_item_2 ) );
      if ( rc ) break;

      {
        rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
        MVM_PANIC_ON_ERROR( rc );
        job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
          mvm_detach_all_wait_transition_result_rsp_fn;

        stream_detach_vocproc.handle = ( uint16_t ) generic_item_2->handle;

        rc = __aprv2_cmd_alloc_send(
               mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
               session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
               mvm_cvs_addr, ( ( uint16_t ) generic_item_1->handle ),
               job_obj->header.handle,
               VSS_ISTREAM_CMD_DETACH_VOCPROC,
               &stream_detach_vocproc, sizeof( stream_detach_vocproc ) );
        MVM_COMM_ERROR( rc, mvm_cvs_addr );
      }
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_attach_vocprocs_to_stream (
  mvm_session_object_t* session_obj,
  mvm_sequencer_job_object_t* seq_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* simple_obj = NULL;
  mvm_generic_item_t* generic_item;
  vss_istream_cmd_attach_vocproc_t stream_attach_vocproc;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );

  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->vocproc_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;
  
    {
      rc = mvm_create_simple_job_object( session_obj->header.handle, &simple_obj );
      MVM_PANIC_ON_ERROR( rc );
      simple_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_stream_attach_detach_vocproc_result_count_rsp_fn;
  
      stream_attach_vocproc.handle = ( uint16_t ) generic_item->handle;
  
      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_cvs_addr, seq_obj->use_case.attach_stream.cvs_handle,
             simple_obj->header.handle,
             VSS_ISTREAM_CMD_ATTACH_VOCPROC,
             &stream_attach_vocproc, sizeof( stream_attach_vocproc ) );
      MVM_COMM_ERROR( rc, mvm_cvs_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_detach_vocprocs_from_stream (
  mvm_session_object_t* session_obj,
  mvm_sequencer_job_object_t* seq_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* simple_obj = NULL;
  mvm_generic_item_t* generic_item;
  vss_istream_cmd_detach_vocproc_t stream_detach_vocproc;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->vocproc_q.dummy );
  
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->vocproc_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;
  
    {
      rc = mvm_create_simple_job_object( session_obj->header.handle, &simple_obj );
      MVM_PANIC_ON_ERROR( rc );
      simple_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_stream_attach_detach_vocproc_result_count_rsp_fn;
  
      stream_detach_vocproc.handle = ( uint16_t ) generic_item->handle;
  
      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_cvs_addr, seq_obj->use_case.detach_stream.cvs_handle,
             simple_obj->header.handle, VSS_ISTREAM_CMD_DETACH_VOCPROC,
             &stream_detach_vocproc, sizeof( stream_detach_vocproc ) );
      MVM_COMM_ERROR( rc, mvm_cvs_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_attach_vocproc_to_streams (
  mvm_session_object_t* session_obj,
  mvm_sequencer_job_object_t* seq_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* simple_obj = NULL;
  mvm_generic_item_t* generic_item;
  vss_istream_cmd_attach_vocproc_t stream_attach_vocproc;

  stream_attach_vocproc.handle = seq_obj->use_case.attach_vocproc.cvp_handle;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    rc = mvm_create_simple_job_object( session_obj->header.handle, &simple_obj );
    MVM_PANIC_ON_ERROR( rc );

    simple_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
      mvm_stream_attach_detach_vocproc_result_count_rsp_fn;

    rc = __aprv2_cmd_alloc_send(
           mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
           mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
           simple_obj->header.handle, VSS_ISTREAM_CMD_ATTACH_VOCPROC,
           &stream_attach_vocproc, sizeof( stream_attach_vocproc ) );
    MVM_COMM_ERROR( rc, mvm_cvs_addr );
  }

  return APR_EOK;
}

static int32_t mvm_action_detach_vocproc_from_streams (
  mvm_session_object_t* session_obj,
  mvm_sequencer_job_object_t* seq_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* simple_obj = NULL;
  mvm_generic_item_t* generic_item;
  vss_istream_cmd_detach_vocproc_t stream_detach_vocproc;

  stream_detach_vocproc.handle = seq_obj->use_case.detach_vocproc.cvp_handle;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );
  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    {
      rc = mvm_create_simple_job_object( session_obj->header.handle, &simple_obj );
      MVM_PANIC_ON_ERROR( rc );
      simple_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
        mvm_stream_attach_detach_vocproc_result_count_rsp_fn;

      rc = __aprv2_cmd_alloc_send(
             mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
             mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
             simple_obj->header.handle, VSS_ISTREAM_CMD_DETACH_VOCPROC,
             &stream_detach_vocproc, sizeof( stream_detach_vocproc ) );
      MVM_COMM_ERROR( rc, mvm_cvs_addr );
    }
  }

  return APR_EOK;
}

static int32_t mvm_action_stream_attach_mvm (
  mvm_session_object_t* session_obj,
  mvm_sequencer_job_object_t* seq_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* simple_obj = NULL;

  rc = mvm_create_simple_job_object( session_obj->header.handle, &simple_obj );
  MVM_PANIC_ON_ERROR( rc );
  seq_obj->subjob_obj = ( ( mvm_object_t* ) simple_obj );
  
  rc = __aprv2_cmd_alloc_send(
         mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
         mvm_cvs_addr, seq_obj->use_case.attach_stream.cvs_handle,
         simple_obj->header.handle,
         VSS_ISTREAM_CMD_MVM_ATTACH, NULL, 0 );
  MVM_COMM_ERROR( rc, mvm_cvs_addr );

  return APR_EOK;
}

static int32_t mvm_action_stream_detach_mvm (
  mvm_session_object_t* session_obj,
  mvm_sequencer_job_object_t* seq_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* simple_obj = NULL;

  rc = mvm_create_simple_job_object( session_obj->header.handle, &simple_obj );
  MVM_PANIC_ON_ERROR( rc );
  seq_obj->subjob_obj = ( ( mvm_object_t* ) simple_obj );
  
  rc = __aprv2_cmd_alloc_send(
         mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
         mvm_cvs_addr, seq_obj->use_case.attach_stream.cvs_handle,
         simple_obj->header.handle,
         VSS_ISTREAM_CMD_MVM_DETACH, NULL, 0 );
  MVM_COMM_ERROR( rc, mvm_cvs_addr );

  return APR_EOK;
}

static int32_t mvm_action_vocproc_attach_mvm (
  mvm_session_object_t* session_obj,
  mvm_sequencer_job_object_t* seq_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* simple_obj = NULL;

  rc = mvm_create_simple_job_object( session_obj->header.handle, &simple_obj );
  MVM_PANIC_ON_ERROR( rc );
  seq_obj->subjob_obj = ( ( mvm_object_t* ) simple_obj );

  rc = __aprv2_cmd_alloc_send(
         mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
         mvm_vpm_addr, seq_obj->use_case.attach_vocproc.cvp_handle,
         simple_obj->header.handle, VSS_IVOCPROC_CMD_MVM_ATTACH,
         NULL, 0 );
  MVM_COMM_ERROR( rc, mvm_vpm_addr );

  return APR_EOK;
}

static int32_t mvm_action_vocproc_detach_mvm (
  mvm_session_object_t* session_obj,
  mvm_sequencer_job_object_t* seq_obj
)
{
  int32_t rc = APR_EOK;
  mvm_simple_job_object_t* simple_obj = NULL;

  rc = mvm_create_simple_job_object( session_obj->header.handle, &simple_obj );
  MVM_PANIC_ON_ERROR( rc );
  seq_obj->subjob_obj = ( ( mvm_object_t* ) simple_obj );
  
  rc = __aprv2_cmd_alloc_send(
         mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
         mvm_vpm_addr, seq_obj->use_case.detach_vocproc.cvp_handle,
         simple_obj->header.handle, VSS_IVOCPROC_CMD_MVM_DETACH,
         NULL, 0 );
  MVM_COMM_ERROR( rc, mvm_vpm_addr );

  return APR_EOK;
}

static int32_t mvm_action_open_avtimer (
  mvm_session_object_t*  session_obj
)
{
  uint32_t rc = APR_EOK;

#ifndef WINSIM
  rc = vocsvc_avtimer_open_handle();
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "MVM: ACTION_OPEN_AVTIMER: FAILED with rc=(0x%08X)", rc );
  }
  else
  {
    session_obj->is_avtimer_handle_open = TRUE;
  }
#endif

  return APR_EOK;
}

static int32_t mvm_action_close_avtimer (
  mvm_session_object_t*  session_obj
)
{
  uint32_t rc = APR_EOK;

#ifndef WINSIM
    rc = vocsvc_avtimer_close_handle();
    if ( rc )
    {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "MVM: ACTION_CLOSE_AVTIMER: FAILED with rc=(0x%08X)", rc );
    }
    else
    {
      session_obj->is_avtimer_handle_open = FALSE;
    }
#endif

  return APR_EOK;
}

static int32_t mvm_action_subscribe_to_vtm (
  mvm_session_object_t*  session_obj
)
{
  int32_t rc = APR_EOK;

  if ( TRUE == session_obj->vfr_info.is_subscribed ) 
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "MVM: ACTION_SUBSCRIBE_VTM: already subscribed for VFR" );
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
  }

  rc = cvd_vfr_subscribe_to_vtm( session_obj->vfr_info.vfr_cycle_duration_us );
  if ( APR_EOK != rc )
  {
    MVM_REPORT_FATAL_ON_ERROR( rc );
    return APR_EFAILED;
  }

  ( void ) cvd_vfr_get_voice_frame_reference( &session_obj->vfr_info.vfr_start_reference_us );
  session_obj->vfr_info.is_subscribed = TRUE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: ACTION_SUBSCRIBE_TO_VTM: vfr_start_reference_us: MSW:(%d) LSW:(%d)",
         ( (uint32_t) (session_obj->vfr_info.vfr_start_reference_us>>32) ),
         (uint32_t) session_obj->vfr_info.vfr_start_reference_us );

  return rc;
}

static int32_t mvm_action_unsubscribe_to_vtm (
  mvm_session_object_t*  session_obj
)
{
  int32_t rc = APR_EOK;

  if ( FALSE == session_obj->vfr_info.is_subscribed )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: ACTION_UNSUBSCRIBE_VTM: already unsubscribed" );
    return APR_EOK;
  }

  session_obj->vfr_info.is_subscribed = FALSE;

  rc = cvd_vfr_unsubscribe_to_vtm();
  if ( APR_EOK != rc )
  {
    MVM_REPORT_FATAL_ON_ERROR( rc );
    return APR_EFAILED;
  }

  return rc;
}

/****************************************************************************
 * MVM STATE MACHINE CONTROL                                                *
 ****************************************************************************/

static int32_t mvm_state_reset_entry (
  mvm_session_object_t* session_obj
)
{
  if ( session_obj->session_ctrl.status == APR_UNDEFINED_ID_V )
  { /* Stay put. */
    return APR_EOK;
  }

  if ( session_obj->session_ctrl.status != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "MVM: STATE_RESET_ENTRY: last goal-->action failed with result=0x%08X",
           session_obj->session_ctrl.status );
  }

  session_obj->session_ctrl.state = MVM_STATE_ENUM_RESET;
  return APR_EIMMEDIATE;
}

static int32_t mvm_state_reset (
  mvm_session_object_t* session_obj
)
{
  switch ( session_obj->session_ctrl.goal )
  {
    case MVM_GOAL_ENUM_NONE:
      break;

    case MVM_GOAL_ENUM_CREATE:
    { /* (RESET to INIT transition). */
      session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
      session_obj->session_ctrl.action = MVM_ACTION_ENUM_CONTINUE;
      session_obj->session_ctrl.status = APR_EOK;

      return APR_EIMMEDIATE;
    }
    break;

    case MVM_GOAL_ENUM_DESTROY:
    {
      ( void ) mvm_action_clean_up_stream_vocproc_lists( session_obj );

      session_obj->session_ctrl.status = APR_EOK;
      ( void ) mvm_do_complete_goal( session_obj );
    }
    break;

    default:
      MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      break;
  }

  return APR_EOK;
}

static int32_t mvm_state_init_entry (
  mvm_session_object_t* session_obj
)
{
  if ( session_obj->session_ctrl.status == APR_UNDEFINED_ID_V )
  { /* Stay put. */
    return APR_EOK;
  }

  if ( session_obj->session_ctrl.status != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "MVM: STATE_INIT_ENTRY: last goal-->action failed with result=0x%08X",
           session_obj->session_ctrl.status );
  }

  session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT;
  return APR_EIMMEDIATE;
}

static int32_t mvm_state_init (
  mvm_session_object_t* session_obj
)
{
  switch ( session_obj->session_ctrl.goal )
  {
    case MVM_GOAL_ENUM_NONE:
      break;

    case MVM_GOAL_ENUM_CREATE:
    {
       /* Complete the pending command and stay in the same state.
        * TODO: Handle errors.*/
       session_obj->session_ctrl.status = APR_EOK;
       ( void ) mvm_do_complete_goal( session_obj );
    }
    break;

    case MVM_GOAL_ENUM_DESTROY:
    {
      switch ( session_obj->session_ctrl.action )
      {
      case MVM_ACTION_ENUM_NONE:
      case MVM_ACTION_ENUM_CONTINUE:
      case MVM_ACTION_ENUM_DETACH_VOCPROCS_FROM_STREAMS:
      {
        if ( ( session_obj->stream_q.size == 0 ) || 
             ( session_obj->vocproc_q.size == 0 ) )
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CONTINUE;
          session_obj->session_ctrl.status = APR_EOK;
        }
        else
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_RESET_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CONTINUE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_detach_vocprocs_from_streams( session_obj );
        }
      }
      return APR_EIMMEDIATE;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
      }
    }
    break;

    case MVM_GOAL_ENUM_STREAM_DISABLE:
    case MVM_GOAL_ENUM_VOCPROC_DISABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_CONTINUE:
        case MVM_ACTION_ENUM_COMPLETE:
        {
          /* All the voice dynamic services have stopped and unsubscribed 
             from voice timer by now.*/
          ( void ) mvm_action_unsubscribe_to_vtm( session_obj );

          /* Complete the pending command and stay in the same state.
           * TODO: Handle errors.*/
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
        }
        break;

       default:
         MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
         break;
      }
    }
    break;

    case MVM_GOAL_ENUM_VOCPROC_RECONFIG:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_CONTINUE:
        case MVM_ACTION_ENUM_VOCPROC_LOAD_COUNT_RESET:
        {
          if ( session_obj->vocproc_q.size > 0 ) 
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_VOCPROC_REINIT;
            session_obj->session_ctrl.status = APR_EOK;
          }
          else
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
            session_obj->session_ctrl.status = APR_EOK;
          }

          ( void ) mvm_action_reset_vocproc_load_count( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_VOCPROC_REINIT:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_EVALUATE_TIMING_PARAMS;
          session_obj->session_ctrl.status = APR_EOK;

          if ( TRUE == session_obj->is_vocproc_reinit_required )
          {
            session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
            ( void ) mvm_action_reinit_vocprocs( session_obj );
          }
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_EVALUATE_TIMING_PARAMS:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_VOCPROC_GET_HDVOICE;
          session_obj->session_ctrl.status = APR_EOK;
          
          if ( TRUE == session_obj->voice_timing.is_timing_ref_available )
          {
            ( void ) mvm_action_unsubscribe_to_vtm( session_obj );
            ( void ) mvm_action_subscribe_to_vtm( session_obj );
            ( void ) mvm_action_evaluate_timing_params( session_obj );
          }
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_VOCPROC_GET_HDVOICE:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_VOCPROC_SET_SYSTEM_CONFIG;
          session_obj->session_ctrl.status = APR_EOK;

          if ( VSS_MEDIA_ID_NONE != session_obj->system_config.media_id )
          {
            session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
            ( void ) mvm_action_get_hdvoice_config( session_obj );
          }
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_VOCPROC_SET_SYSTEM_CONFIG:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_set_system_config_to_vocprocs( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_COMPLETE:
        { 
          /* Complete the pending command and stay in the same state.
           * TODO: Handle errors.*/
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
        }
        break;

        default:
          MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
          break;
       }
    }
    break;

    case MVM_GOAL_ENUM_STREAM_RECONFIG:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_CONTINUE:
        case MVM_ACTION_ENUM_STREAM_LOAD_COUNT_RESET:
        {
          if ( session_obj->stream_q.size > 0 )
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_REINIT;
            session_obj->session_ctrl.status = APR_EOK;
          }
          else
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
            session_obj->session_ctrl.status = APR_EOK;
          }

          ( void ) mvm_action_reset_stream_load_count( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_STREAM_REINIT:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_SET_TTY_MODE;
          session_obj->session_ctrl.status = APR_EOK;

          if ( TRUE == session_obj->is_stream_reinit_required )
          {
            session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
            ( void ) mvm_action_reinit_streams( session_obj );
          }
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_STREAM_SET_TTY_MODE:
        { 
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_SET_SYSTEM_CONFIG;
          session_obj->session_ctrl.status = APR_EOK;

          ( void ) mvm_action_set_tty_mode_to_streams( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_STREAM_SET_SYSTEM_CONFIG:
        { 
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_set_system_config_to_streams( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_COMPLETE:
        { 
          /* Complete the pending command and stay in the same state.
           * TODO: Handle errors.*/
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
        }
        break;

        default:
          MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
          break;
       }
    }
    break;

  case MVM_GOAL_ENUM_VOCPROC_ENABLE:
  {
    switch ( session_obj->session_ctrl.action )
    {
      case MVM_ACTION_ENUM_NONE:
      {
        if ( session_obj->vocproc_q.size > 0 ) 
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CCM_KPPS_UPDATE;
          session_obj->session_ctrl.status = APR_EOK;
        }
        else
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_EOK;
        }
      }
      return APR_EIMMEDIATE;


      case MVM_ACTION_ENUM_CCM_KPPS_UPDATE:
      {
        session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
        session_obj->session_ctrl.action = MVM_ACTION_ENUM_CALCULATE_TIMING_OFFSETS;
        session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

        ( void ) mvm_action_ccm_update_session_config( session_obj );
      }
      return APR_EOK;

      case MVM_ACTION_ENUM_CALCULATE_TIMING_OFFSETS:
      {
        session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
        session_obj->session_ctrl.action = MVM_ACTION_ENUM_CSM_CONFIG_UPDATE;
        session_obj->session_ctrl.status = APR_EOK;

        ( void ) mvm_action_calculate_timing_offsets(session_obj );
      }
      return APR_EIMMEDIATE;

      case MVM_ACTION_ENUM_CSM_CONFIG_UPDATE:
      {
        session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
        session_obj->session_ctrl.action = MVM_ACTION_ENUM_VOCPROC_SET_TIMING_OFFSET;
        session_obj->session_ctrl.status = APR_EOK;

        ( void ) mvm_action_csm_config_update( session_obj );
      }
      return APR_EIMMEDIATE;

      case MVM_ACTION_ENUM_VOCPROC_SET_TIMING_OFFSET:
      {
        session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
        session_obj->session_ctrl.action = MVM_ACTION_ENUM_VOCPROC_ENABLE;
        session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

        ( void ) mvm_action_set_vocproc_timing( session_obj );
      }
      return APR_EIMMEDIATE;

      case MVM_ACTION_ENUM_VOCPROC_ENABLE:
      {
        session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
        session_obj->session_ctrl.action = MVM_ACTION_ENUM_CONTINUE;
        session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

        ( void ) mvm_action_enable_vocprocs( session_obj );
      }
      return APR_EIMMEDIATE;

      case MVM_ACTION_ENUM_COMPLETE:
      { 
        /* Complete the pending command and stay in the same state.
         * TODO: Handle errors.*/
        session_obj->session_ctrl.status = APR_EOK;
        ( void ) mvm_do_complete_goal( session_obj );
      }
      break;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
    }
  }
  break;

  case MVM_GOAL_ENUM_STREAM_ENABLE:
  { 
    switch ( session_obj->session_ctrl.action )
    {
      case MVM_ACTION_ENUM_NONE:
      {
        if ( session_obj->stream_q.size > 0 ) 
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CCM_KPPS_UPDATE;
          session_obj->session_ctrl.status = APR_EOK;
        }
        else
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_EOK;
        }
      }
      return APR_EIMMEDIATE;

      case MVM_ACTION_ENUM_CCM_KPPS_UPDATE:
      {
        session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
        session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_SET_TIMING_OFFSET;
        session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

        ( void ) mvm_action_ccm_update_session_config( session_obj );
      }
      return APR_EIMMEDIATE;

      case MVM_ACTION_ENUM_STREAM_SET_TIMING_OFFSET:
      {
        session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
        session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_ENABLE;
        session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
      
        ( void ) mvm_action_set_stream_timing( session_obj );
      }
      return APR_EIMMEDIATE;

      case MVM_ACTION_ENUM_STREAM_ENABLE:
      {
        session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
        session_obj->session_ctrl.action = MVM_ACTION_ENUM_CONTINUE;
        session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

        ( void) mvm_action_enable_streams( session_obj );
      }
      return APR_EIMMEDIATE;

      case MVM_ACTION_ENUM_COMPLETE:
      { 
        /* Complete the pending command and stay in the same state.
         * TODO: Handle errors.*/
        session_obj->session_ctrl.status = APR_EOK;
        ( void ) mvm_do_complete_goal( session_obj );
      }
      break;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
    }
  }
  break;

  default:
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
    break;
  }

  return APR_EOK;
}

static int32_t mvm_state_vocproc_run_entry (
  mvm_session_object_t* session_obj
)
{
  if ( session_obj->session_ctrl.status == APR_UNDEFINED_ID_V )
  { /* Stay put. */
    return APR_EOK;
  }

  if ( session_obj->session_ctrl.status != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "MVM: STATE_VOCPROC_RUN_ENTRY: last goal-->action failed with result=0x%08X",
           session_obj->session_ctrl.status );
  }

  session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN;
  return APR_EIMMEDIATE;
}

static int32_t mvm_state_vocproc_run (
  mvm_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;

  switch ( session_obj->session_ctrl.goal )
  {
    case MVM_GOAL_ENUM_NONE:
      break;

    case MVM_GOAL_ENUM_STREAM_DISABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_CONTINUE:
        case MVM_ACTION_ENUM_COMPLETE:
        {
          /* Complete the pending command and stay in the same state.
           * TODO: Handle errors.*/
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
        }
        break;

        default:
          MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
          break;
      }
    }
    break;

    case MVM_GOAL_ENUM_DESTROY:
    case MVM_GOAL_ENUM_VOCPROC_RECONFIG:
    case MVM_GOAL_ENUM_VOCPROC_DISABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_VOCPROC_DISABLE:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CONTINUE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = mvm_action_disable_vocprocs( session_obj );
        }
        return APR_EOK;

        default:
          MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
          break;
      }
    }
    break;

    case MVM_GOAL_ENUM_STREAM_RECONFIG:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_CONTINUE:
        case MVM_ACTION_ENUM_STREAM_LOAD_COUNT_RESET:
        {
          if ( session_obj->stream_q.size > 0 ) 
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_SET_TTY_MODE;
            session_obj->session_ctrl.status = APR_EOK;
          }
          else
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
            session_obj->session_ctrl.status = APR_EOK;
          }

          ( void ) mvm_action_reset_stream_load_count( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_STREAM_SET_TTY_MODE:
        { 
          session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_SET_SYSTEM_CONFIG;
          session_obj->session_ctrl.status = APR_EOK;

          ( void ) mvm_action_set_tty_mode_to_streams( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_STREAM_SET_SYSTEM_CONFIG:
        { 
          session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_set_system_config_to_streams( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_COMPLETE:
        {
          /* Complete the pending command and stay in the same state.
           * TODO: Handle errors.*/
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
        }
        break;

        default:
          MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
          break;
       }
    }
    break;

    case MVM_GOAL_ENUM_STREAM_ENABLE:
    { 
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        {
          if ( session_obj->stream_q.size > 0 )
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_SET_TIMING_OFFSET;
            session_obj->session_ctrl.status = APR_EOK;
          }
          else
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
            session_obj->session_ctrl.status = APR_EOK;
          }
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_STREAM_SET_TIMING_OFFSET:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_ENABLE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_set_stream_timing( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_STREAM_ENABLE:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CONTINUE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_enable_streams( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_COMPLETE:
        {
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
        }
        return APR_EOK;

        default:
          MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
          break;
      }
    }
    break;

    case MVM_GOAL_ENUM_VOCPROC_ENABLE:
    { 
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_CONTINUE:
        case MVM_ACTION_ENUM_VOCPROC_GET_AVSYNC_DELAYS:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CALCULATE_AVSYNC_DELAYS;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_get_vocproc_avsync_delays( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_CALCULATE_AVSYNC_DELAYS:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_SET_AVSYNC_DELAYS;
          session_obj->session_ctrl.status = APR_EOK;
  
          ( void ) mvm_action_calculate_avsync_delays( session_obj );
        }
        return APR_EIMMEDIATE;
  
        case MVM_ACTION_ENUM_STREAM_SET_AVSYNC_DELAYS:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_stream_set_avsync_delays( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_COMPLETE:
        {
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
        }
        return APR_EOK;
    
      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
      }
    }
    break;

  default:
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
    break;
  }

  return APR_EOK;
}

static int32_t mvm_state_stream_run_entry (
  mvm_session_object_t* session_obj
)
{
  if ( session_obj->session_ctrl.status == APR_UNDEFINED_ID_V )
  { /* Stay put. */
    return APR_EOK;
  }

  if ( session_obj->session_ctrl.status != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "MVM: STATE_STREAM_RUN_ENTRY: last goal-->action failed with result=0x%08X",
           session_obj->session_ctrl.status );
  }

  session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN;
  return APR_EIMMEDIATE;
}

static int32_t mvm_state_stream_run (
  mvm_session_object_t* session_obj
)
{
  switch ( session_obj->session_ctrl.goal )
  {
    case MVM_GOAL_ENUM_NONE:
      break;

    case MVM_GOAL_ENUM_DESTROY:
    case MVM_GOAL_ENUM_STREAM_RECONFIG:
    case MVM_GOAL_ENUM_STREAM_DISABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_STREAM_DISABLE:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_INIT_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CONTINUE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_disable_streams( session_obj );
        }
        return APR_EOK;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
      }
    }
    break;

    case MVM_GOAL_ENUM_STREAM_ENABLE:
    case MVM_GOAL_ENUM_VOCPROC_DISABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_CONTINUE:
        case MVM_ACTION_ENUM_COMPLETE:
        {
          /* Complete the pending command and stay in the same state.
           * TODO: Handle errors.*/
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
        }
        break;

        default:
          MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
          break;
      }
    }
    break;

    case MVM_GOAL_ENUM_VOCPROC_RECONFIG:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_CONTINUE:
        case MVM_ACTION_ENUM_VOCPROC_LOAD_COUNT_RESET:
        {
          if ( session_obj->vocproc_q.size > 0 ) 
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_VOCPROC_GET_HDVOICE;
            session_obj->session_ctrl.status = APR_EOK;
          }
          else
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
            session_obj->session_ctrl.status = APR_EOK;
          }

          ( void ) mvm_action_reset_vocproc_load_count( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_VOCPROC_GET_HDVOICE:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_VOCPROC_SET_SYSTEM_CONFIG;
          session_obj->session_ctrl.status = APR_EOK;

          if ( VSS_MEDIA_ID_NONE != session_obj->system_config.media_id )
          {
            session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
            ( void ) mvm_action_get_hdvoice_config( session_obj );
          }
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_VOCPROC_SET_SYSTEM_CONFIG:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_set_system_config_to_vocprocs( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_COMPLETE:
        { 
          /* Complete the pending command and stay in the same state.
           * TODO: Handle errors.*/
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
        }
        break;

        default:
          MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
          break;
       }
    }
    break;

    case MVM_GOAL_ENUM_VOCPROC_ENABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        {
          if ( session_obj->vocproc_q.size > 0 ) 
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_CCM_KPPS_UPDATE;
            session_obj->session_ctrl.status = APR_EOK;
          }
          else
          {
            session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
            session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
            session_obj->session_ctrl.status = APR_EOK;
          }
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_CCM_KPPS_UPDATE:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CALCULATE_TIMING_OFFSETS;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_ccm_update_session_config( session_obj );
        }
        return APR_EOK;

        case MVM_ACTION_ENUM_CALCULATE_TIMING_OFFSETS:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CSM_CONFIG_UPDATE;
          session_obj->session_ctrl.status = APR_EOK;

          ( void ) mvm_action_calculate_timing_offsets(session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_CSM_CONFIG_UPDATE:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_VOCPROC_SET_TIMING_OFFSET;
          session_obj->session_ctrl.status = APR_EOK;

          ( void ) mvm_action_csm_config_update( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_VOCPROC_SET_TIMING_OFFSET:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_VOCPROC_ENABLE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_set_vocproc_timing( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_VOCPROC_ENABLE:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CONTINUE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
        
          ( void ) mvm_action_enable_vocprocs( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_COMPLETE:
        { 
          /* Complete the pending command and stay in the same state.
           * TODO: Handle errors.*/
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
        }
        break;
    
        default:
          MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
          break;
       }
    }
    break;

    default:
      MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      break;
  }

  return APR_EOK;
}

static int32_t mvm_state_run_entry (
  mvm_session_object_t* session_obj
)
{
  if ( session_obj->session_ctrl.status == APR_UNDEFINED_ID_V )
  { /* Stay put. */
    return APR_EOK;
  }

  if ( session_obj->session_ctrl.status != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "MVM: STATE_RUN_ENTRY: last goal-->action failed with result=0x%08X",
           session_obj->session_ctrl.status );
  }

  session_obj->session_ctrl.state = MVM_STATE_ENUM_RUN;
  return APR_EIMMEDIATE;
}

static int32_t mvm_state_run (
  mvm_session_object_t* session_obj
)
{
  int32_t rc;

  switch ( session_obj->session_ctrl.goal )
  {
    case MVM_GOAL_ENUM_NONE:
      break;

    case MVM_GOAL_ENUM_DESTROY:
    case MVM_GOAL_ENUM_STREAM_RECONFIG:
    case MVM_GOAL_ENUM_STREAM_DISABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_STREAM_DISABLE:
        {
          /* Disabling CSM while moving out of RUN state. */
          mvm_csm_update_sleep_manager( FALSE );
          session_obj->session_ctrl.state = MVM_STATE_ENUM_VOCPROC_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CONTINUE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
          rc = mvm_action_disable_streams( session_obj );
        }
        return APR_EOK;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
      }
    }
    break;

    case MVM_GOAL_ENUM_VOCPROC_RECONFIG:
    case MVM_GOAL_ENUM_VOCPROC_DISABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_VOCPROC_DISABLE:
        {
          /* Disabling CSM while moving out of RUN state. */
          mvm_csm_update_sleep_manager( FALSE );
          session_obj->session_ctrl.state = MVM_STATE_ENUM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_ENCODER_RESET;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = mvm_action_disable_vocprocs( session_obj );
        }
        return APR_EOK;

        case MVM_ACTION_ENUM_STREAM_ENCODER_RESET:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_STREAM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CONTINUE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          rc = mvm_action_stream_encoder_reset( session_obj );
        }
        return APR_EOK;

        default:
          MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
          break;
      }
    }
    break;

    case MVM_GOAL_ENUM_STREAM_ENABLE:
    { 
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_CONTINUE:
        case MVM_ACTION_ENUM_COMPLETE:
        {
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
          /* Enabling CSM in RUN state. */
          mvm_csm_update_sleep_manager( mvm_csm_status );
        }
        return APR_EOK;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
      }
    }
    break;

    case MVM_GOAL_ENUM_VOCPROC_ENABLE:
    {
      switch ( session_obj->session_ctrl.action )
      {
        case MVM_ACTION_ENUM_NONE:
        case MVM_ACTION_ENUM_CONTINUE:
        case MVM_ACTION_ENUM_VOCPROC_GET_AVSYNC_DELAYS:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_CALCULATE_AVSYNC_DELAYS;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
        
          ( void ) mvm_action_get_vocproc_avsync_delays( session_obj );
        }
        return APR_EIMMEDIATE;
        
        case MVM_ACTION_ENUM_CALCULATE_AVSYNC_DELAYS:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_STREAM_SET_AVSYNC_DELAYS;
          session_obj->session_ctrl.status = APR_EOK;
        
          ( void ) mvm_action_calculate_avsync_delays( session_obj );
        }
        return APR_EIMMEDIATE;
        
        case MVM_ACTION_ENUM_STREAM_SET_AVSYNC_DELAYS:
        {
          session_obj->session_ctrl.state = MVM_STATE_ENUM_RUN_ENTRY;
          session_obj->session_ctrl.action = MVM_ACTION_ENUM_COMPLETE;
          session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;

          ( void ) mvm_action_stream_set_avsync_delays( session_obj );
        }
        return APR_EIMMEDIATE;

        case MVM_ACTION_ENUM_COMPLETE:
        {
          session_obj->session_ctrl.status = APR_EOK;
          ( void ) mvm_do_complete_goal( session_obj );
          /* Enabling CSM in RUN state. */
          mvm_csm_update_sleep_manager( mvm_csm_status );
        }
        return APR_EOK;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
      }
    }
    break;

    default:
      MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      break;
  }

  return APR_EOK;
}

static int32_t mvm_state_control (
  mvm_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;

  if ( session_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  do
  {
    switch ( session_obj->session_ctrl.state )
    {
      case MVM_STATE_ENUM_RESET_ENTRY:
        rc = mvm_state_reset_entry( session_obj );
        break;

      case MVM_STATE_ENUM_RESET:
        ( void ) mvm_log_state_info( session_obj );
        rc = mvm_state_reset( session_obj );
        break;
  
      case MVM_STATE_ENUM_INIT_ENTRY:
        rc = mvm_state_init_entry( session_obj );
        break;
  
      case MVM_STATE_ENUM_INIT:
        ( void ) mvm_log_state_info( session_obj );
        rc = mvm_state_init( session_obj );
        break;
  
      case MVM_STATE_ENUM_VOCPROC_RUN_ENTRY:
        rc = mvm_state_vocproc_run_entry( session_obj );
        break;
  
      case MVM_STATE_ENUM_VOCPROC_RUN:
        ( void ) mvm_log_state_info( session_obj );
        rc = mvm_state_vocproc_run( session_obj );
        break;
  
      case MVM_STATE_ENUM_STREAM_RUN_ENTRY:
        rc = mvm_state_stream_run_entry( session_obj );
        break;
  
      case MVM_STATE_ENUM_STREAM_RUN:
        ( void ) mvm_log_state_info( session_obj );
        rc = mvm_state_stream_run( session_obj );
        break;
  
      case MVM_STATE_ENUM_RUN_ENTRY:
        rc = mvm_state_run_entry( session_obj );
        break;
  
      case MVM_STATE_ENUM_RUN:
        ( void ) mvm_log_state_info( session_obj );
        rc = mvm_state_run( session_obj );
        break;
  
      default:
        rc = APR_EUNEXPECTED;
        break;
    }
  }
  while ( rc == APR_EIMMEDIATE );

  return rc;
}

/****************************************************************************
 * MVM HELPER FUNCTIONS                                                     *
 ****************************************************************************/

static int32_t mvm_helper_open_session_control (
  mvm_pending_control_t* ctrl,
  mvm_indirection_object_t** ret_indirect_obj,
  mvm_session_object_t** ret_session_obj
)
{
  int32_t rc;
  mvm_indirection_object_t* indirect_obj;
  uint16_t client_addr = ctrl->packet->src_addr;

  if ( ctrl == NULL )
  {
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
    return APR_EBADPARAM;
  }

  if ( ret_session_obj == NULL )
  {
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EFAILED );
    MVM_COMM_ERROR( rc, client_addr );
    return APR_EBADPARAM;
  }

  rc = mvm_get_typed_object( ctrl->packet->dst_port, MVM_OBJECT_TYPE_ENUM_INDIRECTION,
                             ( ( mvm_object_t** ) &indirect_obj ) );
  if ( rc )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_helper_open_session_control(): Invalid handle: cmd_opcode=0x%08X "
           "src_addr=0x%08X req_handle=0x%08X", ctrl->packet->opcode,
           ctrl->packet->src_addr, ctrl->packet->dst_port );
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EHANDLE );
    MVM_COMM_ERROR( rc, client_addr );
    return APR_EHANDLE;
  }

  rc = mvm_get_typed_object( indirect_obj->session_handle,
                             MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) ret_session_obj ) );
  if ( rc )
  {
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EHANDLE );
    MVM_COMM_ERROR( rc, client_addr );
    return APR_EHANDLE;
  }

  if ( ret_indirect_obj != NULL )
  {
    *ret_indirect_obj = indirect_obj;
  }

  return APR_EOK;
}

static int32_t mvm_helper_validate_payload_size_control (
  mvm_pending_control_t* ctrl,
  uint32_t valid_size
)
{
  int32_t rc;
  uint32_t size;
  uint16_t client_addr = ctrl->packet->src_addr;

  if ( ctrl == NULL )
  {
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
  }

  size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( ctrl->packet->header );
  if ( size != valid_size )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_helper_validate_payload_size_control(): Invalid payload size: "
           "opcode=0x%08X valid_size=%d actual_size=%d", ctrl->packet->opcode,
           valid_size, size );

    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EBADPARAM );
    MVM_COMM_ERROR( rc, client_addr );
    return APR_EBADPARAM;
  }

  return APR_EOK;
}

static int32_t mvm_helper_create_new_goal_control (
  mvm_goal_enum_t new_goal,
  mvm_session_object_t* session_obj,
  mvm_object_t** ret_job_obj
)
{
  int32_t rc;
  mvm_simple_job_object_t* simple_job;

  if ( ( session_obj == NULL ) || ( ret_job_obj == NULL ) )
  {
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
  }

  if ( session_obj->session_ctrl.goal != MVM_GOAL_ENUM_NONE )
  { /* The session shouldn't be doing anything. */
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
  }

  rc = mvm_create_simple_job_object( session_obj->header.handle, &simple_job );
  MVM_PANIC_ON_ERROR( rc );

  /* The goal is reset by the state machine on completion. */
  session_obj->session_ctrl.goal = new_goal;

  /* The statejob_handle will signal completion. The statejob_handle is
   * reset by the state machine on completion. */
  session_obj->session_ctrl.statejob_handle = simple_job->header.handle;

  /* ret_job_obj stores the job_obj to be checked for completion and to be
   * freed by the current pending command control. */
  *ret_job_obj = ( ( mvm_object_t* ) simple_job );

  return APR_EOK;
}

static bool_t mvm_helper_verify_full_control (
  mvm_pending_control_t* ctrl,
  mvm_indirection_object_t* indirect_obj
)
{
  int32_t rc = APR_EOK;
  uint16_t client_addr = ctrl->packet->src_addr;

  if ( ctrl == NULL || indirect_obj == NULL )
  {
    MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
    return FALSE;
  }

  if ( APR_GET_FIELD( MVM_INDIRECTION_ACCESS_FULL_CONTROL,
                      indirect_obj->access_bits ) )
  {
    return TRUE;
  }
  else
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
           "mvm_helper_verify_full_control(): Access denied: opcode=0x%08X "
           "src_addr=0x%08X src_port=0x%08X", ctrl->packet->opcode,
           ctrl->packet->src_addr, ctrl->packet->src_port );

    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EDENIED );
    MVM_COMM_ERROR( rc, client_addr );
    return FALSE;
  }
}

static bool_t mvm_is_stream_state_start (
  mvm_session_object_t* session_obj
)
{
  bool_t is_stream_state_start = FALSE;

  if( TRUE == session_obj->is_dual_control )
  {
    if( ( ( MVM_CLIENT_STATE_ENUM_START == session_obj->apps_state ) ||
          ( MVM_CLIENT_STATE_ENUM_PAUSE == session_obj->apps_state ) ) &&
        ( MVM_CLIENT_STATE_ENUM_START == session_obj->modem_state ) )
    {
      is_stream_state_start = TRUE;
    }
  }
  else
  {
   if ( ( MVM_CLIENT_STATE_ENUM_START == session_obj->apps_state ) ||
        ( MVM_CLIENT_STATE_ENUM_PAUSE == session_obj->apps_state ) )
   {
     is_stream_state_start = TRUE;
   }
  }

  return is_stream_state_start;
}

static bool_t mvm_is_vocproc_state_start (
  mvm_session_object_t* session_obj
)
{
  bool_t is_vocproc_state_start = FALSE;

  if ( ( MVM_CLIENT_STATE_ENUM_START == session_obj->apps_state ) ||
       ( MVM_CLIENT_STATE_ENUM_STANDBY == session_obj->apps_state ) )
  {
    is_vocproc_state_start = TRUE;
  }

  return is_vocproc_state_start;
}
 
enum {
  MVM_SEQUENCER_ENUM_UNINITIALIZED,
  MVM_SEQUENCER_ENUM_GOAL_STREAM_DISABLE,
  MVM_SEQUENCER_ENUM_GOAL_STREAM_DISABLE_WAIT,
  MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DISABLE,
  MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DISABLE_WAIT,
  MVM_SEQUENCER_ENUM_GOAL_STREAM_RECONFIG,
  MVM_SEQUENCER_ENUM_GOAL_STREAM_RECONFIG_WAIT,
  MVM_SEQUENCER_ENUM_GOAL_VOCPROC_RECONFIG,
  MVM_SEQUENCER_ENUM_GOAL_VOCPROC_RECONFIG_WAIT,
  MVM_SEQUENCER_ENUM_GOAL_VOCPROC_ENABLE,
  MVM_SEQUENCER_ENUM_GOAL_VOCPROC_ENABLE_WAIT,
  MVM_SEQUENCER_ENUM_GOAL_STREAM_ENABLE,
  MVM_SEQUENCER_ENUM_GOAL_STREAM_ENABLE_WAIT,
  MVM_SEQUENCER_ENUM_COMPLETE,
  MVM_SEQUENCER_ENUM_INVALID
 };

/**
 * Makes use of below to appropriately sequence through different goals.
 * - apps_state,
 * - modem_state,
 * - is_stream_reconfig and
 * - is_vocproc_reconfig 
 */
uint32_t mvm_helper_voice_setup_ctrl (
  mvm_sequencer_job_object_t* seq_obj,
  mvm_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  mvm_simple_job_object_t* simple_obj = NULL;

  for( ;; )
  {
    switch ( seq_obj->state )
    {
      case MVM_SEQUENCER_ENUM_GOAL_STREAM_DISABLE:
      {
        seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_STREAM_DISABLE_WAIT;
        rc = mvm_helper_create_new_goal_control( MVM_GOAL_ENUM_STREAM_DISABLE,
                                                 session_obj, &seq_obj->subjob_obj );
        MVM_PANIC_ON_ERROR(rc);
      }
      continue;

      case MVM_SEQUENCER_ENUM_GOAL_STREAM_DISABLE_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;
        if ( simple_obj->is_completed == FALSE )
        {
          ( void ) mvm_state_control( session_obj );
          rc = APR_EPENDING;
          break;
        }

        seq_obj->status = simple_obj->status;
        seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
        ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );

        session_obj->is_stream_reconfig_required = TRUE;
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "MVM: VOICE_SETUP: current seq_state=(%d), seq_status=(0x%08x)",
               seq_obj->state, seq_obj->status );

        if ( FALSE == mvm_is_vocproc_state_start( session_obj ) )
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DISABLE;
        }
      }
      continue;

      case MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DISABLE:
      {
        seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DISABLE_WAIT;
        rc = mvm_helper_create_new_goal_control( MVM_GOAL_ENUM_VOCPROC_DISABLE,
                                                 session_obj, &seq_obj->subjob_obj );
        MVM_PANIC_ON_ERROR(rc);
      }
      continue;

      case MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DISABLE_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;
        if ( simple_obj->is_completed == FALSE )
        {
          ( void ) mvm_state_control( session_obj );
          rc = APR_EPENDING;
          break;
        }

        seq_obj->status = simple_obj->status;
        seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
        ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );

        session_obj->is_vocproc_reconfig_required = TRUE;
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "MVM: VOICE_SETUP: current seq_state=(%d), seq_status=(0x%08x)",
               seq_obj->state, seq_obj->status );
      }
      continue;

      case MVM_SEQUENCER_ENUM_GOAL_VOCPROC_RECONFIG:
      {
        seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_RECONFIG_WAIT;
        rc = mvm_helper_create_new_goal_control( MVM_GOAL_ENUM_VOCPROC_RECONFIG,
                                                 session_obj, &seq_obj->subjob_obj );
        MVM_PANIC_ON_ERROR(rc);
      }
      continue;

      case MVM_SEQUENCER_ENUM_GOAL_VOCPROC_RECONFIG_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;
        if ( simple_obj->is_completed == FALSE )
        {
          ( void ) mvm_state_control( session_obj );
          rc = APR_EPENDING;
          break;
        }

        seq_obj->status = simple_obj->status;
        seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_ENABLE;
        ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "MVM: VOICE_SETUP: current seq_state=(%d), seq_status=(0x%08x)",
               seq_obj->state, seq_obj->status );

        session_obj->is_vocproc_reconfig_required = FALSE;

        if ( ( mvm_is_stream_state_start( session_obj ) ) &&
             ( TRUE == session_obj->is_stream_reconfig_required ) )
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_STREAM_RECONFIG;
        }
      }
      continue;

      case MVM_SEQUENCER_ENUM_GOAL_STREAM_RECONFIG:
      {
        seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_STREAM_RECONFIG_WAIT;
        rc = mvm_helper_create_new_goal_control( MVM_GOAL_ENUM_STREAM_RECONFIG,
                                                 session_obj, &seq_obj->subjob_obj );
        MVM_PANIC_ON_ERROR(rc);
      }
      continue;

      case MVM_SEQUENCER_ENUM_GOAL_STREAM_RECONFIG_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;
        if ( simple_obj->is_completed == FALSE )
        {
          ( void ) mvm_state_control( session_obj );
          rc = APR_EPENDING;
          break;
        }

        seq_obj->status = simple_obj->status;
        seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_STREAM_ENABLE;
        ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "MVM: VOICE_SETUP: current seq_state=(%d), seq_status=(0x%08x)",
               seq_obj->state, seq_obj->status );

        session_obj->is_stream_reconfig_required = FALSE;

        if ( mvm_is_vocproc_state_start( session_obj ) )
        {
          if ( TRUE == session_obj->is_vocproc_reconfig_required )
          {
            seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_RECONFIG;
          }
          else
          {
            seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_ENABLE;
          }
        }
      }
      continue;

      case MVM_SEQUENCER_ENUM_GOAL_VOCPROC_ENABLE:
      {
        seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_ENABLE_WAIT;
        rc = mvm_helper_create_new_goal_control( MVM_GOAL_ENUM_VOCPROC_ENABLE,
                                                 session_obj, &seq_obj->subjob_obj );
        MVM_PANIC_ON_ERROR(rc);
      }
      continue;

      case MVM_SEQUENCER_ENUM_GOAL_VOCPROC_ENABLE_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;
        if ( simple_obj->is_completed == FALSE )
        {
          ( void ) mvm_state_control( session_obj );
          rc = APR_EPENDING;
          break;
        }

        seq_obj->status = simple_obj->status;
        seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
        ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "MVM: VOICE_SETUP: current seq_state=(%d), seq_status=(0x%08x)",
               seq_obj->state, seq_obj->status );

        /** Initialize sequencer to appropiate state. */
        if ( mvm_is_stream_state_start( session_obj ) )
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_STREAM_ENABLE;
        }
      }
      continue;

      case MVM_SEQUENCER_ENUM_GOAL_STREAM_ENABLE:
      {
        seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_STREAM_ENABLE_WAIT;
        rc = mvm_helper_create_new_goal_control( MVM_GOAL_ENUM_STREAM_ENABLE,
                                                 session_obj, &seq_obj->subjob_obj );
        MVM_PANIC_ON_ERROR(rc);
      }
      continue;

      case MVM_SEQUENCER_ENUM_GOAL_STREAM_ENABLE_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;
        if ( simple_obj->is_completed == FALSE )
        {
          ( void ) mvm_state_control( session_obj );
          rc = APR_EPENDING;
          break;
        }

        seq_obj->status = simple_obj->status;
        seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
        ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "MVM: VOICE_SETUP: current seq_state=(%d), seq_status=(0x%08x)",
               seq_obj->state, seq_obj->status );
      }
      continue;

      case MVM_SEQUENCER_ENUM_COMPLETE:
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "MVM: VOICE_SETUP: cmd status=(0X%08X)", seq_obj->status );
        rc = APR_EOK;
      }
      break;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
    }

    break;
  }

  return rc;
}

/****************************************************************************
 * MVM SESSION NON-GATIMG EVENT PROCESSING ROUTINES                         *
 ****************************************************************************/

static int32_t mvm_reconfig_stream_evt_control (
  aprv2_packet_t* packet
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    rc = mvm_get_typed_object( packet->dst_port, MVM_OBJECT_TYPE_ENUM_SESSION,
                               ( ( mvm_object_t** ) &session_obj ) );
    if ( rc )
    {
      MVM_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      break;
    }

    ( void ) mvm_action_reconfig_stream( session_obj );

    break;
  }

  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );

  return rc;
}


static int32_t mvm_reconfig_vocproc_evt_control (
  aprv2_packet_t* packet
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    rc = mvm_get_typed_object( packet->dst_port, MVM_OBJECT_TYPE_ENUM_SESSION,
                               ( ( mvm_object_t** ) &session_obj ) );
    if ( rc )
    {
      MVM_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      break;
    }

    ( void ) mvm_action_reconfig_vocproc( session_obj );

    break;
  }

  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );

  return rc;
 }

static int32_t mvm_ccm_active_sessions_evt_control (
  aprv2_packet_t* packet
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;
  vss_iccm_evt_active_sessions_t* in_args = NULL;
  uint32_t payload_size = 0;
  uint16_t total_num_streams = 0;
  uint16_t total_num_vocprocs = 0;
  uint32_t total_core_kpps = 0;
  bool_t is_multi_session = TRUE;

  for ( ;; )
  {
    rc = mvm_get_typed_object( packet->dst_port, MVM_OBJECT_TYPE_ENUM_SESSION,
                               ( ( mvm_object_t** ) &session_obj ) );
    if ( rc )
    {
      MVM_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      break;
    }
  
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header );
    if ( payload_size != sizeof( vss_iccm_evt_active_sessions_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "MVM: VSS_ICCM_EVT_ACTIVE_SESSIONS: payload size, %d != %d",
             payload_size, sizeof( vss_iccm_evt_active_sessions_t ) );
      break;
    }

    in_args = APRV2_PKT_GET_PAYLOAD( vss_iccm_evt_active_sessions_t, packet );

    total_num_streams = ( in_args->num_nb_streams + in_args->num_wb_streams +
                          in_args->num_swb_streams + in_args->num_fb_plus_streams );
    total_num_vocprocs = ( in_args->num_nb_vocprocs + in_args->num_wb_vocprocs +
                           in_args->num_swb_vocprocs + in_args->num_fb_plus_vocprocs );
    total_core_kpps = in_args->total_core_kpps;

    if ( ( in_args->num_voice_sessions == 1 ) && ( total_num_streams <= 1 ) &&
         ( total_num_vocprocs <= 1 ) )
    {
      is_multi_session = FALSE;
    }

    /* An MVM session can receive the VSS_ICCM_EVT_ACTIVE_SESSIONS event under two
     * scenerios:   
     * 1. This MVM session is transitioning from INIT to VOCPROC_RUN state, where it   
     *    sends the system config update to the CCM. After updating CCM will send
     *    VSS_ICCM_EVT_ACTIVE_SESSIONS to this MVM session and all the other   
     *    active MVM sessions in the system.   
     * 2. This MVM session is in IDLE or RUN state, and another MVM session sends   
     *    the system config update to the CCM. After updating CCM will broadcast the   
     *    VSS_ICCM_EVT_ACTIVE_SESSIONS to all the active MVM sessions.
     */
    if ( ( session_obj->session_ctrl.state == MVM_STATE_ENUM_INIT_ENTRY ) ||
         ( session_obj->session_ctrl.state == MVM_STATE_ENUM_STREAM_RUN_ENTRY ) ||
         ( session_obj->session_ctrl.state == MVM_STATE_ENUM_VOCPROC_RUN_ENTRY ) )
    {
     /* In scenerio 1, this MVM session's state machine is gated in the
      * INIT_ENTRY/STREAM_RUN_ENTRY/VOCPROC_RUN_ENTRY state where it is wating 
      * for the VSS_ICCM_EVT_ACTIVE_SESSIONS event to know the current system 
      * concurrency configurations and VDSP's per hw thread KPPS in order to 
      * continue state transition to STREAM_RUN/VOCPROC_RUN/RUN states. */
      session_obj->session_ctrl.status = APR_EOK;
      mvm_signal_run();
    }
    else
    {
     /* In scenerio 2, if the clock or concurrency configurations have changed,
      * then this MVM session's state machine must transition to INIT state first
      * and back to the state where it was in prior to receiving the
      * VSS_ICCM_EVT_ACTIVE_SESSIONS, in order to re-calculate and re-apply the
      * voice timing offsets based on the new system concurrency configurations
      * and VDSP's per hw thread KPPS. */
      if ( ( is_multi_session !=
             session_obj->clock_and_concurrency_config.is_multi_session ) ||
           ( total_core_kpps !=
             session_obj->clock_and_concurrency_config.total_core_kpps ) ||
           ( session_obj->voice_timing.vptx_preemption_safety_margin !=
             in_args->vptx_preemption_safety_margin ) ||
           ( session_obj->voice_timing.vprx_preemption_safety_margin !=
             in_args->vprx_preemption_safety_margin ) ||
           ( session_obj->voice_timing.venc_preemption_safety_margin !=
             in_args->venc_preemption_safety_margin ) ||
           ( session_obj->voice_timing.vdec_preemption_safety_margin !=
             in_args->vdec_preemption_safety_margin ) )
      {
        ( void ) mvm_action_reconfig_vocproc( session_obj );
      }
    }

    session_obj->clock_and_concurrency_config.is_multi_session = is_multi_session;
    session_obj->clock_and_concurrency_config.total_core_kpps = total_core_kpps;
    session_obj->voice_timing.vptx_preemption_safety_margin = in_args->vptx_preemption_safety_margin;
    session_obj->voice_timing.vprx_preemption_safety_margin = in_args->vprx_preemption_safety_margin;
    session_obj->voice_timing.venc_preemption_safety_margin = in_args->venc_preemption_safety_margin;
    session_obj->voice_timing.vdec_preemption_safety_margin = in_args->vdec_preemption_safety_margin;
    session_obj->voice_proc_delays.venc_proc = in_args->venc_proc;
	session_obj->voice_proc_delays.vdec_proc = in_args->vdec_proc;
	session_obj->voice_proc_delays.vptx_proc = in_args->vptx_proc;
	session_obj->voice_proc_delays.vprx_proc = in_args->vprx_proc;
	
    break;
  }

  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );

  return rc;
}

static int32_t mvm_update_voc_operating_mode_evt_control (
  aprv2_packet_t* packet
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;
  vss_istream_evt_voc_operating_mode_update_t* payload = NULL;
  uint32_t payload_size = 0;

  for ( ;; )
  {
    rc = mvm_get_typed_object( packet->dst_port, MVM_OBJECT_TYPE_ENUM_SESSION,
                               ( ( mvm_object_t** ) &session_obj ) );
    MVM_PANIC_ON_ERROR( rc );

    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header );
    if ( payload_size != sizeof( vss_istream_evt_voc_operating_mode_update_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "MVM: VSS_ISTREAM_EVT_OPERATING_MODE_UPDATE: payload size, %d != %d",
             payload_size, sizeof( vss_istream_evt_voc_operating_mode_update_t ) );
      rc = APR_EBADPARAM;
      break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_voc_operating_mode_update_t,
                                     packet );

    if ( ( mvm_is_voc_op_mode_valid( payload->rx_mode ) == FALSE ) ||
         ( mvm_is_voc_op_mode_valid( payload->tx_mode ) == FALSE ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "MVM: VSS_ISTREAM_EVT_OPERATING_MODE_UPDATE: Invalid Rx mode "
             "0x%08X or Tx vocoder op mode 0x%08X.",
             payload->rx_mode, payload->tx_mode );
      rc = APR_EBADPARAM;
      break;
    }

    if ( ( session_obj->system_config.rx_voc_op_mode == payload->rx_mode ) &&
         ( session_obj->system_config.tx_voc_op_mode == payload->tx_mode ) )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "MVM: VSS_ISTREAM_EVT_OPERATING_MODE_UPDATE: UNCHANGED" );
      break;
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "MVM: VSS_ISTREAM_EVT_OPERATING_MODE_UPDATE: Rx mode 0x%08X "
           "Tx mode 0x%08X.", payload->rx_mode, payload->tx_mode );

    /* Cache the updated operating mode and Queue VSS_IMVM_CMD_DYNAMIC_RECONFIG
     * command for further processing. */
    session_obj->system_config.rx_voc_op_mode = payload->rx_mode;
    session_obj->system_config.tx_voc_op_mode = payload->tx_mode;

    if ( MVM_STATE_ENUM_RUN == session_obj->session_ctrl.state )
    {
      ( void ) mvm_action_dynamic_reconfig( session_obj );
    }
    else
    {
      /** when MVM not in RUN state vocproc reconfiguration needed as any change
       *  to vocoder operating modes requires vocproc stattic re-calibration.
       *  Scenrio: B2B call using same RABS. */
      session_obj->is_vocproc_reconfig_required = TRUE;
    }

    break;
  }

  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );

  return rc;
}


static int32_t mvm_broadcast_voice_activity_evt_control (
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  uint32_t payload_size = 0;
  mvm_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    rc = mvm_get_typed_object( packet->dst_port, MVM_OBJECT_TYPE_ENUM_SESSION,
                               ( ( mvm_object_t** ) &session_obj ) );
    if ( rc )
    {
      MVM_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      break;
    }

    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header );
    if ( payload_size != sizeof( vss_icommon_evt_voice_activity_update_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mvm_broadcast_voice_activity_event(): Unexpected payload "
             "size, %d != %d, Dropping event packet.", payload_size,
             sizeof( vss_istream_evt_voc_operating_mode_update_t ) );
      break;
    }

    if( ( TRUE == voice_activity_client.is_enabled ) &&
        ( APR_NULL_V != voice_activity_client.client_addr ) )
    {
      /* forward the voice activity event recieved from CVP/CVS to client 
       * listening for the same. */
      packet->src_addr = mvm_my_addr;
      packet->src_port = APR_NULL_V;
      packet->dst_addr = voice_activity_client.client_addr;
      packet->dst_port = voice_activity_client.client_port;

      rc = __aprv2_cmd_forward( mvm_apr_handle, packet );
      MVM_COMM_ERROR( rc, voice_activity_client.client_addr );
      return APR_EOK;
    }

    break;
  }

  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );

  return APR_EOK;
}

static int32_t mvm_toggle_csm_evt_control (
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  uint32_t payload_size = 0;
  vss_icommon_evt_toggle_csm_t *param;
  mvm_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header );
    if ( payload_size != sizeof( vss_icommon_evt_toggle_csm_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mvm_toggle_csm_evt_control(): Unexpected payload "
             "size, %d != %d, Dropping event packet.", payload_size,
             sizeof( vss_icommon_evt_toggle_csm_t ) );
      rc = APR_EBADPARAM;
      break;
    }
    param = APRV2_PKT_GET_PAYLOAD( vss_icommon_evt_toggle_csm_t,
                                     packet );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "mvm_toggle_csm_evt_control(): status %d", param->status );
    if ( mvm_csm_status == param->status ) 
    {
       break;
    }
    mvm_csm_status = param->status;
    
    rc = mvm_get_typed_object( packet->dst_port, MVM_OBJECT_TYPE_ENUM_SESSION,
                               ( ( mvm_object_t** ) &session_obj ) );
   
    if ( APR_EOK == rc )
    {
       /* If MVM is in RUN state then update the CSM right away.
          If not, cache the ststus and apply it during next MVM state transition to RUN. 
        */ 
       if ( session_obj->session_ctrl.state == MVM_STATE_ENUM_RUN )
       {
         mvm_csm_update_sleep_manager( mvm_csm_status );
       }
    }
    break;
  }
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
  return APR_EOK;
}

static int32_t mvm_csm_request_vote_cmd_control (
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  uint32_t payload_size = 0;
  vss_imvm_csm_cmd_request_vote_t *param;

  for ( ;; )
  {
    payload_size = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header );
    if ( payload_size != sizeof( vss_imvm_csm_cmd_request_vote_t ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mvm_csm_request_vote_cmd_control(): Unexpected payload "
             "size, %d != %d, Dropping cmd packet.", payload_size,
             sizeof( vss_imvm_csm_cmd_request_vote_t ) );
      rc = APR_EBADPARAM;
      break;
    }
    param = APRV2_PKT_GET_PAYLOAD( vss_imvm_csm_cmd_request_vote_t,
                                     packet );
    rc = mvm_call(VSS_IMVM_CSM_CMD_REQUEST_VOTE, param, 
             sizeof( vss_imvm_csm_cmd_request_vote_t ) );
    break;
  }
  rc =  __aprv2_cmd_end_command( mvm_apr_handle, packet, rc );
  MVM_COMM_ERROR( rc, mvm_my_addr );
  return APR_EOK;
}

uint32_t mvm_query_object_leak( aprv2_packet_t* packet )
{
  if( NULL == packet )
  {
    return APR_EBADPARAM;
  }
  return __aprv2_cmd_end_command( mvm_apr_handle, packet, is_cvd_utils_obj_freed()? APR_EOK:APR_EFAILED );
}

/****************************************************************************
 * MVM COMMAND CONTROL HANDLERS                                             *
 ****************************************************************************/

static int32_t mvm_get_avtimer_time_cmd_control (
  aprv2_packet_t* packet
)
{
  int32_t rc = APR_EFAILED;
  vss_iavtimer_rsp_get_time_t avtimer_timestamp;
  vocsvc_avtimer_timestamp_t time;
  uint16_t client_addr = packet->src_addr;

#ifndef WINSIM
  rc = vocsvc_avtimer_get_time( &time );
  avtimer_timestamp.timestamp_us = time.timestamp_us;
#endif

  if ( rc )
  {
    rc = __aprv2_cmd_end_command( mvm_apr_handle, packet, APR_EFAILED );
    MVM_COMM_ERROR( rc, client_addr);
  }
  else
  {
    rc = __aprv2_cmd_alloc_send(
           mvm_apr_handle, APRV2_PKT_MSGTYPE_CMDRSP_V,
           mvm_my_addr, APR_NULL_V,
           client_addr, packet->src_port,
           packet->token, VSS_IAVTIMER_RSP_GET_TIME,
           &avtimer_timestamp, sizeof( avtimer_timestamp ) );
    MVM_COMM_ERROR( rc, client_addr);
  }

  /* Free the incoming command packet. */
  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );

  return APR_EOK;
}

//<mailBox>
static int32_t mvm_get_avtimer_offset_cmd_control (
  aprv2_packet_t* packet
)
{
  int32_t rc = APR_EFAILED;
  vocsvc_avtimer_timestamp_t avtimer_timestamp;
  uint64_t qtimer_timestamp_us;
  vss_iavtimer_rsp_get_offset_t avtimer_offset;
  uint16_t client_addr = packet->src_addr;

  for( ;; )
  {
    rc = vocsvc_avtimer_get_time( &avtimer_timestamp );
    if ( rc )
    {
      rc = __aprv2_cmd_end_command( mvm_apr_handle, packet, APR_EFAILED );
      MVM_COMM_ERROR( rc, client_addr);
      break;
    }

    qtimer_timestamp_us = qurt_elite_timer_get_time();
    avtimer_offset.offset_us = (int64_t) (avtimer_timestamp.timestamp_us - qtimer_timestamp_us);

    MSG_5( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "MVM: avtimerTimestamp_us: MSW: 0x%08X, LSW: 0x%08X," \
           "qtimerTimestamp_us: MSW: 0x%08X, LSW: 0x%08X," \
           "avtimerTimestamp - qtimerTimestamp_us: %d",
           (uint32_t) (avtimer_timestamp.timestamp_us >> 32), (uint32_t) avtimer_timestamp.timestamp_us,
           (uint32_t) (qtimer_timestamp_us >> 32) , (uint32_t) qtimer_timestamp_us,
           (uint32_t) (avtimer_timestamp.timestamp_us - qtimer_timestamp_us) );

    rc = __aprv2_cmd_alloc_send(
           mvm_apr_handle, APRV2_PKT_MSGTYPE_CMDRSP_V,
           mvm_my_addr, APR_NULL_V,
           client_addr, packet->src_port,
           packet->token, VSS_IAVTIMER_RSP_GET_OFFSET,
           &avtimer_offset, sizeof( avtimer_offset ) );
    MVM_COMM_ERROR( rc, client_addr);

    /* Free the incoming command packet. */
    ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );

    break;
  }

  return APR_EOK;
}

/****************************************************************************
 * MVM SESSION PENDING COMMAND ROUTINES                                     *
 ****************************************************************************/

static int32_t mvm_core_create_session_cmd_control (
  mvm_pending_control_t* ctrl,
  bool_t is_full_ctrl
)
{
  int32_t rc = APR_EOK;
  uint32_t access_bits;
  mvm_indirection_object_t* indirect_obj = NULL;
  mvm_session_object_t* session_obj = NULL;
  char_t* req_session_name;
  uint32_t req_session_name_size;
  uint16_t client_addr = ctrl->packet->src_addr;

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    access_bits = APR_SET_FIELD( MVM_INDIRECTION_ACCESS_FULL_CONTROL,
                                 ( ( is_full_ctrl ) ? 1 : 0 ) );

    req_session_name = APRV2_PKT_GET_PAYLOAD( char_t, ctrl->packet );
    req_session_name_size = ( ( uint16_t ) APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( ctrl->packet->header ) );

    if ( req_session_name_size > 0 )
    {
      MSG_SPRINTF_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
                     "mvm_core_create_session_cmd_control(): Session name = %s",
                     req_session_name );
    }

    rc = mvm_create_indirection_object( client_addr, req_session_name,
                                        req_session_name_size, access_bits,
                                        &indirect_obj, &session_obj );
    if ( rc )
    {
      rc =  __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, rc );
      MVM_COMM_ERROR( rc, client_addr );
      return APR_EOK;
    }

    { /* Initialize the session object. */

      /* Setup for the first full control client. */
      if ( is_full_ctrl )
      {
        if ( session_obj->master_addr == APRV2_PKT_INIT_ADDR_V )
        {
          session_obj->master_addr = ctrl->packet->src_addr;
          session_obj->master_port = ctrl->packet->src_port;
          ( void ) mvm_action_open_avtimer( session_obj );
        }
        else
        { /* Allow only one full control session per named session. */
          ( void ) mvm_free_object( ( mvm_object_t* ) indirect_obj );
          rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EALREADY );
          MVM_COMM_ERROR( rc, client_addr );
          return APR_EOK;
        }
      }
    }

    { /* Update the packet->dst_port to the new session handle. */
      ctrl->packet->dst_port = ( ( uint16_t ) indirect_obj->header.handle );
    }

    /* If this is not the first handle being opened for this session then
     * state machine is already out of reset and there's no need to run it.
     */
    if ( session_obj->indirection_q.size >= 2 )
    {
      rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
      MVM_COMM_ERROR( rc, client_addr );
      return APR_EOK;
    }
    else
    {
      rc = mvm_helper_create_new_goal_control(
             MVM_GOAL_ENUM_CREATE, session_obj, &ctrl->pendjob_obj );
      if ( rc ) return APR_EOK;
    }
  }
  else
  {
    /* Wait until the job is done. */
    if ( ctrl->pendjob_obj->simple_job.is_completed )
    {
      if ( ctrl->pendjob_obj->simple_job.status )
      { /* Destroy the session and respond a failure. */
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        /* TODO: Need to handle error recovery on session setup failure. */
      }
      else
      { /* Respond success. */
        rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
        MVM_COMM_ERROR( rc, client_addr );
      }

      ( void ) mvm_free_object( ctrl->pendjob_obj );
      return APR_EOK;
    }
  }

  /* Run the CVD session state machine. The CVD session state machine should
   * only need to run when there are pending commands to process.
   */
  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  MVM_PANIC_ON_ERROR( rc );
  rc = mvm_state_control( session_obj );

  return APR_EPENDING;
}


static int32_t mvm_core_destroy_session_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  uint32_t status = APR_EOK;
  mvm_indirection_object_t* indirect_obj = NULL;
  mvm_session_object_t* session_obj = NULL;
  mvm_simple_job_object_t* simple_obj = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = mvm_helper_open_session_control( ctrl, &indirect_obj, &session_obj );
  if ( rc )
  {
    return APR_EOK;
  }

  for( ;; )
  {
    if( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
    {
      if ( APR_GET_FIELD( MVM_INDIRECTION_ACCESS_FULL_CONTROL,
                          indirect_obj->access_bits ) )
      {
        session_obj->master_addr = APRV2_PKT_INIT_ADDR_V;
        session_obj->master_port = APR_NULL_V;
       ( void ) mvm_action_close_avtimer( session_obj );
      }

      /* Do not run state machine when there are multiple indirections still alive. */
      if ( session_obj->indirection_q.size >= 2 )
      {
        ( void ) mvm_free_object( ( mvm_object_t* ) indirect_obj );
        status = APR_EOK;
        break;
      }
      else
      {
        rc = mvm_helper_create_new_goal_control( MVM_GOAL_ENUM_DESTROY, session_obj,
                                                 &ctrl->pendjob_obj );
        MVM_PANIC_ON_ERROR(rc)
      }
    }

    ( void ) mvm_state_control( session_obj );

    simple_obj = &ctrl->pendjob_obj->simple_job;
    if ( simple_obj->is_completed == FALSE )
    {
      rc = APR_EPENDING;
      break;
    }

    status = simple_obj->status;
    if ( APR_EOK == status )
    {
      ( void ) mvm_destroy_session_object( session_obj );
    }
    ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );

    break;
  }

  if ( APR_EOK == rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "MVM: DESTROY_SESSION: status=(%d)", status );

    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, status );
    MVM_COMM_ERROR( rc, client_addr );

    rc = APR_EOK;
  }

  return rc;
}

static int32_t mvm_start_voice_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  uint16_t client_addr = ctrl->packet->src_addr;
  mvm_session_object_t* session_obj = NULL;
  mvm_sequencer_job_object_t* seq_obj = NULL;

  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc ) return APR_EOK;

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    session_obj->apps_state = MVM_CLIENT_STATE_ENUM_START;

    /* Blanket clock voting. */
    ( void ) vccm_mmpm_request_clocks( VCCM_CLIENT_ID_MVM, MVM_SESSION_START_MPPS );
    ( void ) mvm_ccm_add_voice_session( session_obj->header.handle );

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR(rc);
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
    seq_obj->status = APR_EOK;

    if ( mvm_is_vocproc_state_start( session_obj ) )
    {
      seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_RECONFIG;
    }
  }

  seq_obj = &ctrl->pendjob_obj->sequencer_job;
  rc = mvm_helper_voice_setup_ctrl( seq_obj, session_obj );
  if ( APR_EOK == rc )
  {
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, seq_obj->status );
    MVM_COMM_ERROR( rc, client_addr );

    ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
    ( void ) vccm_mmpm_release_clocks( VCCM_CLIENT_ID_MVM );

    rc = APR_EOK;
  }

  return rc;
}

static int32_t mvm_standby_voice_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  uint16_t client_addr = ctrl->packet->src_addr;
  mvm_session_object_t* session_obj = NULL;
  mvm_sequencer_job_object_t* seq_obj = NULL;

  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc ) return APR_EOK;

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    session_obj->apps_state = MVM_CLIENT_STATE_ENUM_STANDBY;

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR(rc);
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_STREAM_DISABLE;
  }

  seq_obj = &ctrl->pendjob_obj->sequencer_job;
  rc = mvm_helper_voice_setup_ctrl( seq_obj, session_obj );
  if ( APR_EOK == rc )
  {
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, seq_obj->status );
    MVM_COMM_ERROR( rc, client_addr );

    ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
    rc = APR_EOK;
  }

  return rc;
}

static int32_t mvm_pause_voice_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint16_t client_addr = ctrl->packet->src_addr;
  mvm_session_object_t* session_obj = NULL;
  mvm_sequencer_job_object_t* seq_obj = NULL;

  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc )
  {
    return APR_EOK;
  }

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    session_obj->apps_state = MVM_CLIENT_STATE_ENUM_PAUSE;

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR(rc);
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DISABLE;
  }

  seq_obj = &ctrl->pendjob_obj->sequencer_job;
  rc = mvm_helper_voice_setup_ctrl( seq_obj, session_obj );
  if ( APR_EOK == rc )
  {
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, seq_obj->status );
    MVM_COMM_ERROR( rc, client_addr );
  
    ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
    rc = APR_EOK;
  }

  return rc;
}

static int32_t mvm_stop_voice_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint16_t client_addr = ctrl->packet->src_addr;
  mvm_session_object_t* session_obj = NULL;
  mvm_sequencer_job_object_t* seq_obj = NULL;

  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc ) return APR_EOK;

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    /* VSS_IMVM_CMD_STOP_VOICE from HLOS is assumed to be the call ends.*/
    if ( MVM_CLIENT_STATE_ENUM_STOP != session_obj->apps_state )
    {
      mvm_debug_call_num++;
      session_obj->is_stream_reinit_required = TRUE;
      session_obj->is_vocproc_reinit_required = TRUE;
    }

    session_obj->apps_state = MVM_CLIENT_STATE_ENUM_STOP;

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR(rc);
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_STREAM_DISABLE;
  }

  seq_obj = &ctrl->pendjob_obj->sequencer_job;
  rc = mvm_helper_voice_setup_ctrl( seq_obj, session_obj );
  if ( APR_EOK == rc )
  {
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, seq_obj->status );
    MVM_COMM_ERROR( rc, client_addr );

    ( void ) mvm_ccm_remove_voice_session( session_obj->header.handle );
    ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
    rc = APR_EOK;
  }

  return rc;
}

static int32_t mvm_modem_start_voice_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  uint16_t client_addr = ctrl->packet->src_addr;
  mvm_session_object_t* session_obj = NULL;
  mvm_indirection_object_t* indirect_obj = NULL;
  mvm_sequencer_job_object_t* seq_obj = NULL;

  rc = mvm_helper_open_session_control( ctrl, &indirect_obj, &session_obj );
  if ( rc ) return APR_EOK;

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    /* The modem command is allowed only with full control handle. */
    if ( mvm_helper_verify_full_control( ctrl, indirect_obj ) == FALSE )
    {
      return APR_EOK;
    }

    session_obj->modem_state = MVM_CLIENT_STATE_ENUM_START;

    if ( ( TRUE == session_obj->is_dual_control ) &&
         ( mvm_is_vocproc_state_start( session_obj ) ) )
    {
      rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
      MVM_PANIC_ON_ERROR(rc);

      seq_obj = &ctrl->pendjob_obj->sequencer_job;
      seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_RECONFIG;
      session_obj->is_stream_reconfig_required = TRUE;
      /* Blanket ClOCK voting. */
      ( void ) vccm_mmpm_request_clocks( VCCM_CLIENT_ID_MVM, MVM_SESSION_START_MPPS );
    }
    else
    {
      rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
      MVM_COMM_ERROR( rc, client_addr );
      return APR_EOK;
    }
  }

  seq_obj = &ctrl->pendjob_obj->sequencer_job;
  rc = mvm_helper_voice_setup_ctrl( seq_obj, session_obj );
  if ( APR_EOK == rc )
  {
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, seq_obj->status );
    MVM_COMM_ERROR( rc, client_addr );

    ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
    ( void ) vccm_mmpm_release_clocks( VCCM_CLIENT_ID_MVM );
    rc = APR_EOK;
  }

  return rc;
}

static int32_t mvm_modem_stop_voice_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint16_t client_addr = ctrl->packet->src_addr;
  mvm_session_object_t* session_obj = NULL;
  mvm_indirection_object_t* indirect_obj = NULL;
  mvm_sequencer_job_object_t* seq_obj = NULL;

  rc = mvm_helper_open_session_control( ctrl, &indirect_obj, &session_obj );
  if ( rc ) return APR_EOK;

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    /* The modem command is allowed only with full control handle. */
    if ( mvm_helper_verify_full_control( ctrl, indirect_obj ) == FALSE )
    {
      return APR_EOK;
    }

    session_obj->modem_state = MVM_CLIENT_STATE_ENUM_STOP;

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR(rc);
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
    seq_obj->status = APR_EOK;

    if ( TRUE == session_obj->is_dual_control )
    {
      seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_STREAM_DISABLE;
    }
  }

  seq_obj = &ctrl->pendjob_obj->sequencer_job;
  rc = mvm_helper_voice_setup_ctrl( seq_obj, session_obj );
  if ( APR_EOK == rc )
  {
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, seq_obj->status );
    MVM_COMM_ERROR( rc, client_addr );

    ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
    rc = APR_EOK;
  }

  return rc;
}

static int32_t mvm_reconfig_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;
  mvm_sequencer_job_object_t* seq_obj = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = mvm_get_typed_object( ctrl->packet->dst_port, MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "MVM: STATIC_RECONFIG Invalid src_addr=0x%08X, req_handle=0x%08X",
           ctrl->packet->src_addr, ctrl->packet->dst_port );
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EHANDLE );
    MVM_COMM_ERROR( rc, client_addr );
    return APR_EOK;
  }

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR(rc);
    seq_obj = &ctrl->pendjob_obj->sequencer_job;

    if ( ( TRUE == session_obj->is_stream_reconfig_required ) &&
         ( mvm_is_stream_state_start( session_obj ) ) )
    {
      /* Note: Stream reconfiguration needs vocproc reconfiguration as 
       * it changes vocproc timing offsets. */
      session_obj->is_vocproc_reconfig_required = TRUE;
      seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_RECONFIG;
    }
    else if ( ( TRUE == session_obj->is_vocproc_reconfig_required ) &&
              ( mvm_is_vocproc_state_start( session_obj ) ) )
    {
      seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_RECONFIG;
    }
    else
    {
      seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
      seq_obj->status = APR_EOK;
    }
  }

  seq_obj = &ctrl->pendjob_obj->sequencer_job;
  rc = mvm_helper_voice_setup_ctrl( seq_obj, session_obj );
  if ( APR_EOK == rc )
  {
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, seq_obj->status );
    MVM_COMM_ERROR( rc, client_addr );

    ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
    rc = APR_EOK;
  }

  return rc;
}

static int32_t mvm_dynamic_reconfig_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  uint16_t client_addr = ctrl->packet->src_addr;
  mvm_session_object_t* session_obj = NULL;
  mvm_sequencer_job_object_t* seq_obj = NULL;
  enum {
    MVM_SEQUENCER_ENUM_UNINITIALIZED,
    MVM_SEQUENCER_ENUM_GET_HDVOICE_CONFIG,
    MVM_SEQUENCER_ENUM_GET_HDVOICE_CONFIG_WAIT,
    MVM_SEQUENCER_ENUM_SET_HDVOICE_CONFIG,
    MVM_SEQUENCER_ENUM_SET_HDVOICE_CONFIG_WAIT,
    MVM_SEQUENCER_ENUM_SET_DYNAMIC_VOCPROC_SYSTEM_CONFIG,
    MVM_SEQUENCER_ENUM_SET_DYNAMIC_VOCPROC_SYSTEM_CONFIG_WAIT,
    MVM_SEQUENCER_ENUM_COMPLETE,
    MVM_SEQUENCER_ENUM_INVALID
  };

  rc = mvm_get_typed_object( ctrl->packet->dst_port, MVM_OBJECT_TYPE_ENUM_SESSION,
                             ( ( mvm_object_t** ) &session_obj ) );
  MVM_PANIC_ON_ERROR( rc );

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR( rc );
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    seq_obj->state = MVM_SEQUENCER_ENUM_GET_HDVOICE_CONFIG;
  }

  for ( ;; )
  {
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    switch ( seq_obj->state )
    {
      case MVM_SEQUENCER_ENUM_GET_HDVOICE_CONFIG:
      {
        seq_obj->state = MVM_SEQUENCER_ENUM_GET_HDVOICE_CONFIG_WAIT;
        session_obj->vocproc_rsp_cnt = 0;

        ( void ) mvm_action_get_hdvoice_config( session_obj );
      } /* Fall through */

      case MVM_SEQUENCER_ENUM_GET_HDVOICE_CONFIG_WAIT:
      {
        if ( session_obj->vocproc_rsp_cnt < session_obj->vocproc_q.size )
        {
          return APR_EPENDING;
        }

        seq_obj->state = MVM_SEQUENCER_ENUM_SET_HDVOICE_CONFIG;

        if ( TRUE == session_obj->is_stream_reconfig_required )
        {
          ( void ) mvm_action_reconfig_stream( session_obj );
          seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          continue;
        }
      } /* Fall through */

      case MVM_SEQUENCER_ENUM_SET_HDVOICE_CONFIG:
      {
        seq_obj->state = MVM_SEQUENCER_ENUM_SET_HDVOICE_CONFIG_WAIT;
        session_obj->stream_rsp_cnt = 0;
        session_obj->vocproc_rsp_cnt = 0;

        ( void ) mvm_action_set_hdvoice_to_streams( session_obj ); 
        ( void ) mvm_action_set_hdvoice_to_vocprocs( session_obj );
      } /* Fall through */

      case MVM_SEQUENCER_ENUM_SET_HDVOICE_CONFIG_WAIT:
      {
        if ( ( session_obj->stream_rsp_cnt < session_obj->stream_q.size ) ||
             ( session_obj->vocproc_rsp_cnt < session_obj->vocproc_q.size ) )
        {
          return APR_EPENDING;
        }

        seq_obj->state = MVM_SEQUENCER_ENUM_SET_DYNAMIC_VOCPROC_SYSTEM_CONFIG;

        if( ( TRUE == session_obj->is_stream_reconfig_required ) ||
            ( TRUE == session_obj->is_vocproc_reconfig_required ) )
        {
          /* NOTE: system re-configuration is encountered. */
          seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          continue;
        }
      } /* Fall through */

      case MVM_SEQUENCER_ENUM_SET_DYNAMIC_VOCPROC_SYSTEM_CONFIG:
      {
        seq_obj->state = MVM_SEQUENCER_ENUM_SET_DYNAMIC_VOCPROC_SYSTEM_CONFIG_WAIT;
        session_obj->vocproc_rsp_cnt = 0;

        ( void ) mvm_action_set_dymamic_system_config( session_obj );
      } /* Fall through */

      case MVM_SEQUENCER_ENUM_SET_DYNAMIC_VOCPROC_SYSTEM_CONFIG_WAIT:
      {
        if ( session_obj->vocproc_rsp_cnt < session_obj->vocproc_q.size )
        {
          return APR_EPENDING;
        }

        seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
      }

      case MVM_SEQUENCER_ENUM_COMPLETE:
      {
        rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
        MVM_COMM_ERROR( rc, client_addr );

        ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
        ctrl->pendjob_obj = NULL;
      }
      return APR_EOK;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
    }

    break;
  }

  return APR_EPENDING;;
}

static int32_t mvm_hdvoice_ui_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;
  uint32_t hdvoice_cmd = ctrl->packet->opcode;

  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc ) return APR_EOK;

  switch ( hdvoice_cmd )
  {
    case VSS_IHDVOICE_CMD_ENABLE:
    {
      session_obj->is_hdvoice_ui_enabled = MVM_HDVOICE_UI_ENABLED;
      break;
    }
    case VSS_IHDVOICE_CMD_DISABLE:
    {
      session_obj->is_hdvoice_ui_enabled = MVM_HDVOICE_UI_DISABLED;
      break;
    }
    case VSS_IHDVOICE_CMD_BEAMR_DISABLE:
    {
      session_obj->is_beamr_enabled = FALSE;
      break;
    }
    case VSS_IHDVOICE_CMD_WV2_DISABLE:
    {
      session_obj->is_wv2_enabled = FALSE;
      break;
    }
    case VSS_IHDVOICE_CMD_BEAMR_ENABLE:
    {
      session_obj->is_beamr_enabled = TRUE;
      break;
    }
    case VSS_IHDVOICE_CMD_WV2_ENABLE:
    {
      session_obj->is_wv2_enabled = TRUE;
      break;
    }
    default:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "MVM: UI_HDVOICE: invalid CMD:0x%08X.", hdvoice_cmd );
      break;
    }
  }

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: UI_HDVOICE: HD Voice UI enablement:0x%08X, BBWE-specific "
         "command 0x%08X.", session_obj->is_hdvoice_ui_enabled, hdvoice_cmd );

  if ( session_obj->session_ctrl.state == MVM_STATE_ENUM_RUN )
  {
    ( void ) mvm_action_dynamic_reconfig( session_obj );
  }

  rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
  MVM_COMM_ERROR( rc, client_addr );

  return APR_EOK;
}

static int32_t mvm_attach_stream_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;
  mvm_sequencer_job_object_t* seq_obj = NULL;
  mvm_simple_job_object_t* simple_obj = NULL;
  vss_imvm_cmd_attach_stream_t* in_args = NULL;
  mvm_generic_item_t* generic_item = NULL;
  uint16_t client_addr =  ctrl->packet->src_addr;;
  enum {
    MVM_SEQUENCER_ENUM_UNINITIALIZED,
    MVM_SEQUENCER_ENUM_ATTACH_VOCPROCS_TO_STREAM,
    MVM_SEQUENCER_ENUM_ATTACH_VOCPROCS_TO_STREAM_WAIT,
    MVM_SEQUENCER_ENUM_ADD_STREAM_TO_LIST,
    MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_STREAM,
    MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_STREAM_WAIT,
    MVM_SEQUENCER_ENUM_COMPLETE,
    MVM_SEQUENCER_ENUM_INVALID
  };

  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc ) return APR_EOK;

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    rc = mvm_helper_validate_payload_size_control( 
           ctrl, sizeof( vss_imvm_cmd_attach_stream_t ) );
    if ( rc ) return APR_EOK;

    in_args = APRV2_PKT_GET_PAYLOAD( vss_imvm_cmd_attach_stream_t, ctrl->packet );
    if ( NULL == in_args->handle )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_attach_stream_cmd_control(): NULL Stream handle" );

      rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EBADPARAM );
      MVM_COMM_ERROR( rc, client_addr );
      return APR_EOK;
    }

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR( rc );
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    seq_obj->use_case.attach_stream.cvs_handle = in_args->handle;
    seq_obj->state = MVM_SEQUENCER_ENUM_ATTACH_VOCPROCS_TO_STREAM;

    /* Check to avoid attaching same handle. */
    rc = mvm_find_object_from_session( &session_obj->stream_q,
           seq_obj->use_case.attach_stream.cvs_handle, &generic_item );
    if ( rc == APR_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "mvm_attach_stream_cmd_control(): STREAM[0x%08X] already attached",
      seq_obj->use_case.attach_stream.cvs_handle );
      seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
      seq_obj->status = APR_EALREADY;
    }
  }

  for ( ;; )
  {
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    switch ( seq_obj->state )
    {
      case MVM_SEQUENCER_ENUM_ATTACH_VOCPROCS_TO_STREAM:
      {
        /* Attach all the vocprocs that are currently under MVM's jurisdiction
         * to this stream. */
        seq_obj->state = MVM_SEQUENCER_ENUM_ATTACH_VOCPROCS_TO_STREAM_WAIT;
        session_obj->stream_attach_detach_vocproc_rsp_cnt= 0;

        ( void ) mvm_action_attach_vocprocs_to_stream( session_obj, seq_obj );
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_ATTACH_VOCPROCS_TO_STREAM_WAIT:
      {
        if ( session_obj->stream_attach_detach_vocproc_rsp_cnt < session_obj->vocproc_q.size )
        {
          rc = APR_EPENDING;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "mvm_attach_stream_cmd_control(): current seq_state=(%d), "
               "seq_status=(0x%08x)", seq_obj->state, APR_EOK );
        seq_obj->state = MVM_SEQUENCER_ENUM_ADD_STREAM_TO_LIST;
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_ADD_STREAM_TO_LIST:
      {
        rc = apr_list_remove_head( &session_obj->free_item_q,
                                   ( ( apr_list_node_t** ) &generic_item ) );
        if ( rc )
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          seq_obj->status = APR_ENORESOURCE;
          continue;
        }
        else
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_STREAM;
        }

        generic_item->handle = seq_obj->use_case.attach_stream.cvs_handle;

        ( void ) apr_list_add_tail( &session_obj->stream_q, &generic_item->link );
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_STREAM:
      {
        ( void ) mvm_action_stream_attach_mvm( session_obj, seq_obj );

        seq_obj->state = MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_STREAM_WAIT;
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_STREAM_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;
        if ( simple_obj->is_completed == FALSE )
        {
          rc = APR_EPENDING;
          break;
        }

        seq_obj->status = simple_obj->status;
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "mvm_attach_stream_cmd_control(): current seq_state=(%d), "
               "seq_status=(0x%08x)", seq_obj->state, seq_obj->status );

        seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
        ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_COMPLETE:
      { /* End the sequencer. */
        if ( seq_obj->status == APR_EOK ) 
        {
          /* Full MVM re-config is needed when a stream is being attached during
           * runtime. */
          if ( mvm_is_stream_state_start( session_obj ) )
          {
            ( void ) mvm_action_reconfig_stream( session_obj );
          }
        }
      }
      break;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
    }

    break;
  }

  if ( APR_EOK == rc  )
  {
     MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
            "mvm_attach_stream_cmd_control(): MVM[0x%08X] stream[0x%08X] "
            "attach rc = (0x%08X)", session_obj->header.handle,
            seq_obj->use_case.attach_stream.cvs_handle, seq_obj->status );
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, seq_obj->status );
    MVM_COMM_ERROR( rc, client_addr );

    ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
    ctrl->pendjob_obj = NULL;
  }

  return rc;
}

static int32_t mvm_detach_stream_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj;
  mvm_sequencer_job_object_t* seq_obj;
  mvm_simple_job_object_t* simple_obj;
  vss_imvm_cmd_detach_stream_t* in_args;
  mvm_generic_item_t* generic_item;
  uint16_t client_addr = ctrl->packet->src_addr;
  enum {
    MVM_SEQUENCER_ENUM_UNINITIALIZED,
    MVM_SEQUENCER_ENUM_GOAL_STREAM_DIASBLE,
    MVM_SEQUENCER_ENUM_GOAL_STREAM_DIASBLE_WAIT,
    MVM_SEQUENCER_ENUM_REMOVE_STREAM_FROM_LIST,
    MVM_SEQUENCER_ENUM_DETACH_VOCPROCS_FROM_STREAM,
    MVM_SEQUENCER_ENUM_DETACH_VOCPROCS_FROM_STREAM_WAIT,
    MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_STREAM,
    MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_STREAM_WAIT,
    MVM_SEQUENCER_ENUM_COMPLETE,
    MVM_SEQUENCER_ENUM_INVALID
  };

  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc ) return APR_EOK;

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    rc = mvm_helper_validate_payload_size_control(
           ctrl, sizeof( vss_imvm_cmd_detach_stream_t ) );
    if ( rc ) return APR_EOK;

    in_args = APRV2_PKT_GET_PAYLOAD( vss_imvm_cmd_detach_stream_t,
                                     ctrl->packet );
    if ( NULL == in_args->handle )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_detach_stream_cmd_control(): NULL Stream handle" );

      rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EBADPARAM );
      MVM_COMM_ERROR( rc, client_addr );
      return APR_EOK;
    }

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR( rc );
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    seq_obj->use_case.detach_stream.cvs_handle = in_args->handle;
    seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_STREAM_DIASBLE;
  }

  for ( ;; )
  {
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    switch ( seq_obj->state )
    {
      case MVM_SEQUENCER_ENUM_GOAL_STREAM_DIASBLE:
      {
        rc = mvm_find_object_from_session( &session_obj->stream_q,
               seq_obj->use_case.detach_stream.cvs_handle, &generic_item );
        if ( APR_EOK != rc )
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          seq_obj->status = APR_EFAILED;
          continue;
        }
        else
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_STREAM_DIASBLE_WAIT;
          rc = mvm_helper_create_new_goal_control( MVM_GOAL_ENUM_STREAM_DISABLE,
                                                   session_obj, &seq_obj->subjob_obj );
          MVM_PANIC_ON_ERROR(rc);
        }
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_GOAL_STREAM_DIASBLE_WAIT:
      {
         simple_obj = &seq_obj->subjob_obj->simple_job;
         if ( simple_obj->is_completed == FALSE )
         {
            ( void ) mvm_state_control( session_obj );
            rc = APR_EPENDING;
            break;
         }
         seq_obj->status = simple_obj->status;

         MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                "mvm_detach_stream_cmd_control(): current seq_state=(%d), "
                "seq_status=(0x%08x)", seq_obj->state, seq_obj->status );

         ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );
         seq_obj->state = MVM_SEQUENCER_ENUM_REMOVE_STREAM_FROM_LIST;
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_REMOVE_STREAM_FROM_LIST:
      {
        rc = mvm_find_object_from_session( &session_obj->stream_q,
        seq_obj->use_case.detach_stream.cvs_handle, &generic_item );
        if ( APR_EOK != rc )
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          seq_obj->status = APR_EFAILED;
          continue;
        }

        /* Remove the CVS from the active stream list. */
        ( void ) apr_list_delete( &session_obj->stream_q, &generic_item->link );
        ( void ) apr_list_add_tail( &session_obj->free_item_q, &generic_item->link );

        seq_obj->state = MVM_SEQUENCER_ENUM_DETACH_VOCPROCS_FROM_STREAM;
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_DETACH_VOCPROCS_FROM_STREAM:
      {
        /* Detach all the vocprocs that are currently under MVM's jurisdiction
         * from this stream. */
        seq_obj->state = MVM_SEQUENCER_ENUM_DETACH_VOCPROCS_FROM_STREAM_WAIT;
        session_obj->stream_attach_detach_vocproc_rsp_cnt= 0;

        ( void ) mvm_action_detach_vocprocs_from_stream( session_obj, seq_obj );
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_DETACH_VOCPROCS_FROM_STREAM_WAIT:
      { 
        if ( session_obj->stream_attach_detach_vocproc_rsp_cnt < session_obj->vocproc_q.size )
        {
          rc = APR_EPENDING;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "mvm_detach_stream_cmd_control(): current seq_state=(%d), "
               "seq_status=(0x%08x)", seq_obj->state, seq_obj->status );

        seq_obj->state = MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_STREAM;
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_STREAM:
      { 
        seq_obj->state = MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_STREAM_WAIT;
        ( void ) mvm_action_stream_detach_mvm( session_obj, seq_obj );
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_STREAM_WAIT:
      { 
        simple_obj = &seq_obj->subjob_obj->simple_job;
        if ( simple_obj->is_completed == FALSE )
        {
          rc = APR_EPENDING;
          break;
        }

        seq_obj->status = simple_obj->status;
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "MVM: DETACH_STREAM: seq_state=(%d), seq_status=(0x%08x)",
               seq_obj->state, seq_obj->status );

        ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );
        seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_COMPLETE:
      { /* End the sequencer. */
        if ( APR_EOK == seq_obj->status )
        {
          if ( session_obj->stream_q.size == 0 )
          {
            /* If there is no stream attached to MVM, reset the vocoder operating
             * mode book-keeping variables. */
            session_obj->system_config.rx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
            session_obj->system_config.tx_voc_op_mode = VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE;
          }
          else if ( mvm_is_stream_state_start( session_obj ) )
          {
            /* Full MVM re-config is needed if we still have atleast one stream 
             * attached during runtime. */
            ( void ) mvm_action_reconfig_stream( session_obj );
          }
        }
        rc = APR_EOK;
      }
      break;


    default:
      MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      break;
    }

    break;
  }

  if ( APR_EOK == rc  )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "MVM:[0x%08X] DETACH_STREAM:[0x%08X] rc:[0x%08X]", session_obj->header.handle,
           seq_obj->use_case.detach_stream.cvs_handle, seq_obj->status );

    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, seq_obj->status );
    MVM_COMM_ERROR( rc, client_addr );

    ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
    ctrl->pendjob_obj = NULL;
  }

  return rc;
}

static int32_t mvm_attach_vocproc_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj;
  mvm_sequencer_job_object_t* seq_obj;
  mvm_simple_job_object_t* simple_obj;
  vss_imvm_cmd_attach_vocproc_t* in_args;
  mvm_generic_item_t* generic_item;
  uint16_t client_addr = ctrl->packet->src_addr;
  enum {
    MVM_SEQUENCER_ENUM_UNINITIALIZED,
    MVM_SEQUENCER_ENUM_ATTACH_VOCPROC_TO_STREAMS,
    MVM_SEQUENCER_ENUM_ATTACH_VOCPROC_TO_STREAMS_WAIT,
    MVM_SEQUENCER_ENUM_ADD_VOCPROC_TO_LIST,
    MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_VOCPROC,
    MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_VOCPROC_WAIT,
    MVM_SEQUENCER_ENUM_COMPLETE,
    MVM_SEQUENCER_ENUM_INVALID
  };

  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc ) return APR_EOK;

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    rc = mvm_helper_validate_payload_size_control(
           ctrl, sizeof( vss_imvm_cmd_attach_vocproc_t ) );
    if ( rc ) return APR_EOK;

    in_args = APRV2_PKT_GET_PAYLOAD( vss_imvm_cmd_attach_vocproc_t, ctrl->packet );
    if ( NULL == in_args->handle )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "mvm_attach_vocproc_cmd_control(): NULL Vocproc handle" );

      rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EBADPARAM );
      MVM_COMM_ERROR( rc, client_addr );
      return APR_EOK;
    }

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR( rc );
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    seq_obj->use_case.attach_vocproc.cvp_handle = in_args->handle;
    seq_obj->state = MVM_SEQUENCER_ENUM_ATTACH_VOCPROC_TO_STREAMS;

    /* Check to avoid attaching same handle. */
    rc = mvm_find_object_from_session( &session_obj->vocproc_q,
           seq_obj->use_case.attach_stream.cvs_handle, &generic_item );
    if ( rc == APR_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "mvm_attach_vocproc_cmd_control(): VOCPROC[0x%08X] already attached",
      seq_obj->use_case.attach_vocproc.cvp_handle );
      seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
      seq_obj->status = APR_EALREADY;
    }
  }

  for ( ;; )
  {
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    switch ( seq_obj->state )
    {
      case MVM_SEQUENCER_ENUM_ATTACH_VOCPROC_TO_STREAMS:
      {
        /* Attach this vocproc to all the streams that are currently under
         * MVM's jurisdiction.*/
        session_obj->stream_attach_detach_vocproc_rsp_cnt = 0;
        ( void ) mvm_action_attach_vocproc_to_streams( session_obj, seq_obj );

        seq_obj->state = MVM_SEQUENCER_ENUM_ATTACH_VOCPROC_TO_STREAMS_WAIT;
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_ATTACH_VOCPROC_TO_STREAMS_WAIT:
      { 
        if ( session_obj->stream_attach_detach_vocproc_rsp_cnt < session_obj->stream_q.size )
        {
          rc = APR_EPENDING;
          break;
        }

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "MVM: ATTACH_VOCPROC: seq_state=(%d), seq_status=(0x%08x)",
               seq_obj->state, APR_EOK );
        seq_obj->state = MVM_SEQUENCER_ENUM_ADD_VOCPROC_TO_LIST;
      }
      /*-fallthru*/

      case MVM_SEQUENCER_ENUM_ADD_VOCPROC_TO_LIST:
      {
        rc = apr_list_remove_head( &session_obj->free_item_q,
                                   ( ( apr_list_node_t** ) &generic_item ) );
        if ( rc )
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          seq_obj->status = APR_ENORESOURCE;
          continue;
        }
        else
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_VOCPROC;
        }

        generic_item->handle = seq_obj->use_case.attach_vocproc.cvp_handle;

        ( void ) apr_list_add_tail( &session_obj->vocproc_q, &generic_item->link );
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_VOCPROC:
      { 
        ( void ) mvm_action_vocproc_attach_mvm( session_obj, seq_obj );
        seq_obj->state = MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_VOCPROC_WAIT;
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_ATTACH_MVM_TO_VOCPROC_WAIT:
      {
        simple_obj= &seq_obj->subjob_obj->simple_job;
        if ( simple_obj->is_completed == FALSE )
        {
          rc = APR_EPENDING;
          break;
        }
        seq_obj->status = simple_obj->status;
        seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "MVM: ATTACH_VOCPROC: seq_state=(%d), seq_status=(0x%08x)",
               seq_obj->state, seq_obj->status );
        ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );
      }
      /*-fallthru */

      case MVM_SEQUENCER_ENUM_COMPLETE:
      { /* End the sequencer. */
        if( seq_obj->status == APR_EOK )
        { 
          /* Vocproc Re-config is needed when a vocproc is being attached during 
           * runtime. */
          if ( MVM_CLIENT_STATE_ENUM_START == session_obj->apps_state )
          {
            ( void ) mvm_action_reconfig_vocproc( session_obj );
          }
        }
        rc = APR_EOK;
      }
      break;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
    }

    break;
  }

  if ( APR_EOK == rc  )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "MVM: ATTACH_VOCPROC:[0x%08X] completed with status:[0x%08X]",
           seq_obj->use_case.attach_vocproc.cvp_handle, seq_obj->status );

    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, seq_obj->status );
    MVM_COMM_ERROR( rc, client_addr );

    ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
    ctrl->pendjob_obj = NULL;
  }

  return rc;
}

static int32_t mvm_detach_vocproc_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj;
  mvm_sequencer_job_object_t* seq_obj;
  mvm_simple_job_object_t* simple_obj;
  vss_imvm_cmd_detach_vocproc_t* in_args;
  mvm_generic_item_t* generic_item;
  uint16_t client_addr = ctrl->packet->src_addr;
  enum {
    MVM_SEQUENCER_ENUM_UNINITIALIZED,
    MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DIASBLE,
    MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DIASBLE_WAIT,
    MVM_SEQUENCER_ENUM_REMOVE_VOCPROC_FROM_LIST,
    MVM_SEQUENCER_ENUM_DETACH_VOCPROC_FROM_STREAMS,
    MVM_SEQUENCER_ENUM_DETACH_VOCPROC_FROM_STREAMS_WAIT,
    MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_VOCPROC,
    MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_VOCPROC_WAIT,
    MVM_SEQUENCER_ENUM_COMPLETE,
    MVM_SEQUENCER_ENUM_INVALID
  };

  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc ) return APR_EOK;

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    rc = mvm_helper_validate_payload_size_control(
           ctrl, sizeof( vss_imvm_cmd_detach_vocproc_t ) );
    if ( rc ) return APR_EOK;

    in_args = APRV2_PKT_GET_PAYLOAD( vss_imvm_cmd_detach_vocproc_t,
                                     ctrl->packet );
    if ( NULL == in_args->handle )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "mvm_detach_vocproc_cmd_control(): NULL Vocproc handle" );

      rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EBADPARAM );
      MVM_COMM_ERROR( rc, client_addr );
      return APR_EOK;
    }

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR( rc );
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    seq_obj->use_case.detach_vocproc.cvp_handle = in_args->handle;
    seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DIASBLE;
  }

  for ( ;; )
  {
    seq_obj = &ctrl->pendjob_obj->sequencer_job;
    switch ( seq_obj->state )
    {
      case MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DIASBLE:
      {
        rc = mvm_find_object_from_session( &session_obj->vocproc_q,
               seq_obj->use_case.detach_vocproc.cvp_handle, &generic_item );
        if ( rc )
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          seq_obj->status = APR_EFAILED;
          continue;
        }
        else
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DIASBLE_WAIT;
          rc = mvm_helper_create_new_goal_control( MVM_GOAL_ENUM_VOCPROC_DISABLE,
                                                   session_obj, &seq_obj->subjob_obj );
          MVM_PANIC_ON_ERROR(rc);
        }
      }  /*-fallthru */

      case MVM_SEQUENCER_ENUM_GOAL_VOCPROC_DIASBLE_WAIT:
      {
         simple_obj = &seq_obj->subjob_obj->simple_job;
         if ( simple_obj->is_completed == FALSE )
         {
            ( void ) mvm_state_control( session_obj );
            rc = APR_EPENDING;
            break;
         }

         seq_obj->status = simple_obj->status;
         ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );
         MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                "mvm_detach_vocproc_cmd_control(): current seq_state=(%d), "
                "seq_status=(0x%08x)", seq_obj->state, seq_obj->status );

         seq_obj->state = MVM_SEQUENCER_ENUM_REMOVE_VOCPROC_FROM_LIST;
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_REMOVE_VOCPROC_FROM_LIST:
      {
        rc = mvm_find_object_from_session( &session_obj->vocproc_q,
               seq_obj->use_case.detach_vocproc.cvp_handle, &generic_item );
        if ( rc )
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          seq_obj->status = APR_EFAILED;
          continue;
        }
        else
        {
          seq_obj->state = MVM_SEQUENCER_ENUM_DETACH_VOCPROC_FROM_STREAMS;
        }

        ( void ) apr_list_delete( &session_obj->vocproc_q, &generic_item->link );
        ( void ) apr_list_add_tail( &session_obj->free_item_q, &generic_item->link );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_DETACH_VOCPROC_FROM_STREAMS:
      {
        seq_obj->state = MVM_SEQUENCER_ENUM_DETACH_VOCPROC_FROM_STREAMS_WAIT;
        session_obj->stream_attach_detach_vocproc_rsp_cnt = 0;

        ( void ) mvm_action_detach_vocproc_from_streams( session_obj, seq_obj );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_DETACH_VOCPROC_FROM_STREAMS_WAIT:
      {
        if ( session_obj->stream_attach_detach_vocproc_rsp_cnt < session_obj->stream_q.size )
        {
          rc = APR_EPENDING;
          break;
        }
        seq_obj->state = MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_VOCPROC;
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_VOCPROC:
      {
        ( void ) mvm_action_vocproc_detach_mvm( session_obj, seq_obj );
        seq_obj->state = MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_VOCPROC_WAIT;
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_DETACH_MVM_FROM_VOCPROC_WAIT:
      {
        simple_obj = &seq_obj->subjob_obj->simple_job;
        if ( simple_obj->is_completed == FALSE )
        {
          rc = APR_EPENDING;
          break;
        }
        seq_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
        seq_obj->status = simple_obj->status;

        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "mvm_detach_vocproc_cmd_control(): current seq_state=(%d), "
               "seq_status=(0x%08x)", seq_obj->state, seq_obj->status );

        ( void ) mvm_free_object( ( mvm_object_t* ) simple_obj );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_COMPLETE:
      { /* End the sequencer. */
        if ( APR_EOK == seq_obj->status )
        {
          if ( session_obj->vocproc_q.size > 0 )
          {
           /* Vocproc re-config is needed if we still have atleast one vocproc 
            * attached to MVM during runtime. */
            ( void ) mvm_action_reconfig_vocproc( session_obj );
          }
        }
        rc = APR_EOK;
      }
      break;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
    }

    break;
  }

  if ( APR_EOK == rc  )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "mvm_detach_vocproc_cmd_control(): vocproc[0x%08X] detach MVM "
           "completed with status = (0x%08X)",
           seq_obj->use_case.detach_vocproc.cvp_handle, seq_obj->status );

    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, seq_obj->status );
    MVM_COMM_ERROR( rc, client_addr );

    ( void ) mvm_free_object( ( mvm_object_t* ) seq_obj );
    ctrl->pendjob_obj = NULL;
  }

  return rc;
}

/* BACKWARD COMPATIBILITY */
static int32_t mvm_common_set_network_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc;
  mvm_session_object_t* session_obj;
  mvm_indirection_object_t* indirect_obj;
  vss_icommon_cmd_set_network_t* in_args;
  uint32_t network_id;
  uint32_t rx_pp_sr;
  uint32_t tx_pp_sr;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = mvm_helper_open_session_control( ctrl, &indirect_obj, &session_obj );
  if ( rc ) return APR_EOK;

  rc = mvm_helper_validate_payload_size_control(
         ctrl, sizeof( vss_icommon_cmd_set_network_t ) );
  if ( rc ) return APR_EOK;

  /* This command is allowed only with full control handle. */
  if ( mvm_helper_verify_full_control( ctrl, indirect_obj ) == FALSE )
  {
    return APR_EOK;
  }

  { /* Do work. */
    in_args = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_set_network_t, ctrl->packet );

    /* Convert the legacy network id to the new network_id.
     * (And validate the parameter in the process.)
     */
    rc = mvm_convert_legacy_network_id( in_args->network_id, &network_id,
                                        &rx_pp_sr, &tx_pp_sr );
    if ( rc )
    {
      rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EBADPARAM );
      MVM_COMM_ERROR( rc, client_addr );
      return APR_EOK;
    }

    /* Save parameters in target set to be picked up by state machine. */
    session_obj->system_config.network_id = network_id;
    session_obj->system_config.rx_pp_sr = rx_pp_sr;
    session_obj->system_config.tx_pp_sr = tx_pp_sr;

    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
    MVM_COMM_ERROR( rc, client_addr );
  }

  return APR_EOK;
}

static int32_t mvm_set_cal_network_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;
  mvm_indirection_object_t* indirect_obj = NULL;
  vss_imvm_cmd_set_cal_network_t* in_args = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = mvm_helper_open_session_control( ctrl, &indirect_obj, &session_obj );
  if ( rc ) return APR_EOK;

  rc = mvm_helper_validate_payload_size_control(
         ctrl, sizeof( vss_imvm_cmd_set_cal_network_t ) );
  if ( rc ) return APR_EOK;

  /* This command is allowed only with full control handle. */
  if ( mvm_helper_verify_full_control( ctrl, indirect_obj ) == FALSE )
  {
    return APR_EOK;
  }

  in_args = APRV2_PKT_GET_PAYLOAD( vss_imvm_cmd_set_cal_network_t, ctrl->packet );

  /* Validate parameters. */
  if ( mvm_is_network_id_valid( in_args->network_id ) == FALSE )
  {
    rc = APR_EBADPARAM;
  }
  else
  {
    /* Save parameters in target set to be picked up by state machine. */
    session_obj->system_config.network_id = in_args->network_id;
  }

  rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, rc );
  MVM_COMM_ERROR( rc, client_addr );

  return APR_EOK;
}

static int32_t mvm_set_cal_media_type_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;
  mvm_indirection_object_t* indirect_obj = NULL;
  vss_imvm_cmd_set_cal_media_type_t* in_args = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = mvm_helper_open_session_control( ctrl, &indirect_obj, &session_obj );
  if ( rc ) return APR_EOK;

  rc = mvm_helper_validate_payload_size_control(
         ctrl, sizeof( vss_imvm_cmd_set_cal_media_type_t ) );
  if ( rc ) return APR_EOK;

  /* This command is allowed only with full control handle. */
  if ( mvm_helper_verify_full_control( ctrl, indirect_obj ) == FALSE )
  {
    return APR_EOK;
  }

  in_args = APRV2_PKT_GET_PAYLOAD( vss_imvm_cmd_set_cal_media_type_t, ctrl->packet );

  /* Save parameters in target set to be picked up by state machine. */
  session_obj->system_config.media_id = in_args->media_id;

  rc = mvm_determine_target_sampling_rates( session_obj,
         session_obj->requested_var_voc_rx_sampling_rate,
         session_obj->requested_var_voc_tx_sampling_rate );

  rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, rc );
  MVM_COMM_ERROR( rc, client_addr );

  return APR_EOK;
}

static int32_t mvm_set_max_var_voc_sampling_rate_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc;
  mvm_session_object_t* session_obj;
  vss_imvm_cmd_set_max_var_voc_sampling_rate_t* in_args;
  uint16_t client_addr = ctrl->packet->src_addr;
  mvm_simple_job_object_t* job_obj;
  mvm_generic_item_t* generic_item;
  vss_icommon_cmd_set_var_voc_sampling_rate_t set_var_voc_sampling_rate;


  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc ) return APR_EOK;

  rc = mvm_helper_validate_payload_size_control(
         ctrl, sizeof( vss_imvm_cmd_set_max_var_voc_sampling_rate_t ) );
  if ( rc ) return APR_EOK;

  in_args = APRV2_PKT_GET_PAYLOAD( vss_imvm_cmd_set_max_var_voc_sampling_rate_t, ctrl->packet );

  if ( FALSE == mvm_is_var_sr_supported( session_obj->system_config.media_id ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "mvm_set_max_var_voc_sampling_rate_cmd_control(): Media type does "
         "not support variable sample rates" );
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EFAILED );
    MVM_COMM_ERROR( rc, client_addr );
    return APR_EOK;
  }

  rc = mvm_determine_target_sampling_rates( session_obj, in_args->rx, in_args->tx );
  if ( rc )
  {
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EBADPARAM );
    MVM_COMM_ERROR( rc, client_addr );
    return APR_EOK;
  }

   session_obj->requested_var_voc_rx_sampling_rate = session_obj->system_config.dec_sr;
   session_obj->requested_var_voc_tx_sampling_rate = session_obj->system_config.enc_sr;

#if 1 /* Not needed as MVM evaluates the sample rate for ENC/DEC/TXPP/RXPP. */
  set_var_voc_sampling_rate.rx_pp_sr = session_obj->requested_var_voc_rx_sampling_rate;
  set_var_voc_sampling_rate.tx_pp_sr = session_obj->requested_var_voc_tx_sampling_rate;

  generic_item = ( ( mvm_generic_item_t* ) &session_obj->stream_q.dummy );
  session_obj->stream_rsp_cnt = 0;

  for ( ;; )
  {
    rc = apr_list_get_next( &session_obj->stream_q,
                            ( ( apr_list_node_t* ) generic_item ),
                            ( ( apr_list_node_t** ) &generic_item ) );
    if ( rc ) break;

    rc = mvm_create_simple_job_object( session_obj->header.handle, &job_obj );
    MVM_PANIC_ON_ERROR( rc );

    job_obj->fn_table[ MVM_RESPONSE_FN_ENUM_RESULT ] =
      mvm_streams_group_wait_transition_result_rsp_fn;

    rc = __aprv2_cmd_alloc_send(
            mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
            session_obj->self_addr, ( ( uint16_t ) session_obj->header.handle ),
            mvm_cvs_addr, ( ( uint16_t ) generic_item->handle ),
            job_obj->header.handle, VSS_ICOMMON_CMD_SET_VAR_VOC_SAMPLING_RATE,
            &set_var_voc_sampling_rate, sizeof( vss_icommon_cmd_set_var_voc_sampling_rate_t ) );
    MVM_COMM_ERROR( rc, mvm_cvs_addr );
  }
#endif

  rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
  MVM_COMM_ERROR( rc, client_addr );

  return APR_EOK;
}

static int32_t mvm_set_voice_timing_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc;
  mvm_session_object_t* session_obj;
  mvm_indirection_object_t* indirect_obj;
  vss_icommon_cmd_set_voice_timing_t* in_args;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = mvm_helper_open_session_control( ctrl, &indirect_obj, &session_obj );
  if ( rc ) return APR_EOK;

  rc = mvm_helper_validate_payload_size_control(
         ctrl, sizeof( vss_icommon_cmd_set_voice_timing_t ) );
  if ( rc ) return APR_EOK;

  /* This command is allowed only with full control handle. */
  if ( mvm_helper_verify_full_control( ctrl, indirect_obj ) == FALSE )
  {
    return APR_EOK;
  }

  in_args = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_set_voice_timing_t, ctrl->packet );

  /* Save parameters in target set to be picked up by state machine. */
  session_obj->system_config.vsid = 0;
  session_obj->system_config.vfr_mode = in_args->mode;
  
  session_obj->vfr_info.vfr_cycle_duration_us = MVM_DEFAULT_VFR_CYCLE_DURATION_US;  
  session_obj->vfr_info.vfr_mode = in_args->mode;
  /* NOTE:
   * Hack to unify the timing interface from CVD->VCP.
   * In-order to avoid usage of OLD inetrace. */
  session_obj->vfr_info.vsid = 0x11C01000;

  session_obj->voice_timing.enc_offset = in_args->enc_offset;
  session_obj->voice_timing.dec_req_offset = in_args->dec_req_offset;
  session_obj->voice_timing.dec_offset = in_args->dec_offset;
  session_obj->voice_timing.is_timing_ref_available = FALSE;
  
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: SET_TIMING_V1: vsid:(0x%08X) vfr_mode:(%d)",
         session_obj->vfr_info.vsid, session_obj->vfr_info.vfr_mode );

  rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
  MVM_COMM_ERROR( rc, client_addr );

  return APR_EOK;
}

static int32_t mvm_set_voice_timing_v2_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;
  mvm_indirection_object_t* indirect_obj = NULL;
  vss_icommon_cmd_set_voice_timing_v2_t* in_args = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = mvm_helper_open_session_control( ctrl, &indirect_obj, &session_obj );
  if ( rc ) return APR_EOK;

  rc = mvm_helper_validate_payload_size_control(
         ctrl, sizeof( vss_icommon_cmd_set_voice_timing_v2_t ) );
  if ( rc ) return APR_EOK;

  if ( mvm_helper_verify_full_control( ctrl, indirect_obj ) == FALSE )
  {
    return APR_EOK;
  }

  in_args = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_set_voice_timing_v2_t, ctrl->packet );

  /* Save parameters in target set to be picked up by state machine. */
  session_obj->system_config.vsid = in_args->vsid;
  session_obj->system_config.vfr_mode = in_args->mode;

  session_obj->vfr_info.vfr_cycle_duration_us = MVM_DEFAULT_VFR_CYCLE_DURATION_US;
  session_obj->vfr_info.vfr_mode = in_args->mode;
  session_obj->vfr_info.vsid = in_args->vsid;

  if( 0 == session_obj->vfr_info.vsid )
  {
    /* NOTE: Hack to unify the timing interface from CVD->VCP.
    * In-order to avoid usage of OLD inetrace. */
    session_obj->vfr_info.vsid = 0x11C01000;
  }

  session_obj->voice_timing.enc_offset = in_args->enc_offset;
  session_obj->voice_timing.dec_req_offset = in_args->dec_req_offset;
  session_obj->voice_timing.dec_offset = in_args->dec_offset;
  session_obj->voice_timing.is_timing_ref_available = FALSE;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: SET_TIMING_V2: vsid:(0x%08X) vfr_mode:(%d)",
         session_obj->vfr_info.vsid, session_obj->vfr_info.vfr_mode );

  rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
  MVM_COMM_ERROR( rc, client_addr );

  return APR_EOK;
}

//<mailBox>
static int32_t mvm_set_vfr_config_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;
  mvm_indirection_object_t* indirect_obj = NULL;
  vss_icommon_cmd_set_vfr_config_t* in_args = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = mvm_helper_open_session_control( ctrl, &indirect_obj, &session_obj );
  if ( rc ) return APR_EOK;

  rc = mvm_helper_validate_payload_size_control(
         ctrl, sizeof( vss_icommon_cmd_set_vfr_config_t ) );
  if ( rc ) return APR_EOK;

  if ( mvm_helper_verify_full_control( ctrl, indirect_obj ) == FALSE )
  {
    return APR_EOK;
  }

  in_args = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_set_vfr_config_t, ctrl->packet );

  session_obj->vfr_info.vsid = in_args->vsid;
  session_obj->vfr_info.vfr_mode = in_args->vfr_mode;
  session_obj->vfr_info.vfr_cycle_duration_us = in_args->vfr_cycle_duration_ms*1000;

  if ( ( in_args->vfr_cycle_duration_ms%20 ) || ( in_args->vfr_cycle_duration_ms>40 ) )
  {
    session_obj->vfr_info.vfr_cycle_duration_us = MVM_DEFAULT_VFR_CYCLE_DURATION_US;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
           "MVM: SET_VFR_CONFIG: wrong vfr_cycle_duration_ms[%d] reseting to 20msec",
           in_args->vfr_cycle_duration_ms );
    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EBADPARAM );
    MVM_COMM_ERROR( rc, client_addr );
    return APR_EOK;
  }
  
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "MVM: SET_VFR_CONFIG: vsid:(0x%08X) vfr_mode:(%d) vfr_cycle_duration_us:(%d)",
         session_obj->vfr_info.vsid, session_obj->vfr_info.vfr_mode,
         session_obj->vfr_info.vfr_cycle_duration_us );

  rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
  MVM_COMM_ERROR( rc, client_addr );

  return APR_EOK;
}

static int32_t mvm_set_voice_timing_reference_cmd_control (
   mvm_pending_control_t* ctrl
 )
 {
   int32_t rc = APR_EOK;
   mvm_session_object_t* session_obj = NULL;
   mvm_indirection_object_t* indirect_obj = NULL;
   vss_icommon_cmd_set_voice_timing_reference_t* in_args = NULL;
   uint16_t client_addr = ctrl->packet->src_addr;
 
   rc = mvm_helper_open_session_control( ctrl, &indirect_obj, &session_obj );
   if ( rc ) return APR_EOK;
 
   rc = mvm_helper_validate_payload_size_control(
          ctrl, sizeof( vss_icommon_cmd_set_voice_timing_reference_t ) );
   if ( rc ) return APR_EOK;
 
   if ( mvm_helper_verify_full_control( ctrl, indirect_obj ) == FALSE )
   {
     return APR_EOK;
   }

   in_args = APRV2_PKT_GET_PAYLOAD( vss_icommon_cmd_set_voice_timing_reference_t, ctrl->packet );

   session_obj->voice_timing.tx_ref_timestamp_us = in_args->tx_ref_timstamp_us;
   session_obj->voice_timing.rx_ref_timestamp_us = in_args->rx_ref_timstamp_us;
   session_obj->voice_timing.is_timing_ref_available = TRUE;

   MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
          "MVM: SET_VOICE_TIMING_REFERENCE: tx_reference MSW: 0x%08X, " \
          "LSW: 0x%08X, rx_reference MSW: 0x%08X, LSW: 0x%08X",
          (uint32_t)(in_args->tx_ref_timstamp_us>>32), (uint32_t)in_args->tx_ref_timstamp_us,
          (uint32_t)(in_args->rx_ref_timstamp_us>>32), (uint32_t)in_args->rx_ref_timstamp_us );

   rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
   MVM_COMM_ERROR( rc, client_addr );

   return APR_EOK;
 }

static int32_t mvm_set_tty_mode_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_session_object_t* session_obj = NULL;
  vss_itty_cmd_set_tty_mode_t* in_args = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc ) return APR_EOK;

  rc = mvm_helper_validate_payload_size_control(
         ctrl, sizeof( vss_itty_cmd_set_tty_mode_t ) );
  if ( rc ) return APR_EOK;

  in_args = APRV2_PKT_GET_PAYLOAD( vss_itty_cmd_set_tty_mode_t, ctrl->packet );

  /* Save parameters in target set to be picked up by state machine. */
  session_obj->tty_mode = in_args->mode;

  if ( ( session_obj->session_ctrl.state == MVM_STATE_ENUM_RUN ) ||
       ( session_obj->session_ctrl.state == MVM_STATE_ENUM_STREAM_RUN ) )
  {
    ( void ) mvm_action_set_tty_mode_to_streams( session_obj );
  }

  rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
  MVM_COMM_ERROR( rc, client_addr );

  return APR_EOK;
}

/* BACKWARD COMPATIBILITY */
static int32_t mvm_widevoice_set_widevoice_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc;
  mvm_session_object_t* session_obj;
  vss_iwidevoice_cmd_set_widevoice_t* in_args;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = mvm_helper_open_session_control( ctrl, NULL, &session_obj );
  if ( rc ) return APR_EOK;

  rc = mvm_helper_validate_payload_size_control(
         ctrl, sizeof( vss_iwidevoice_cmd_set_widevoice_t ) );
  if ( rc ) return APR_EOK;

  { /* Do work. */
    in_args = APRV2_PKT_GET_PAYLOAD( vss_iwidevoice_cmd_set_widevoice_t, ctrl->packet );

    if ( in_args->enable == 1 || in_args->enable == 0 )
    {
      /* If widevoice is enabled, when configuring the vocproc in the
       * state machine, we overrride the rx sample rate with 16 KHz.*/
      //session_obj->is_wv_set_by_ui = TRUE;
      session_obj->widevoice_enable = in_args->enable;
    }
    else
    {
      rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EBADPARAM );
      MVM_COMM_ERROR( rc, client_addr );
      return APR_EOK;
    }

    rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
    MVM_COMM_ERROR( rc, client_addr );
  }

  return APR_EOK;
}

static int32_t mvm_set_policy_dual_control_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  mvm_indirection_object_t* indirect_obj = NULL;
  mvm_session_object_t* session_obj = NULL;
  vss_imvm_cmd_set_policy_dual_control_t* in_args = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = mvm_helper_open_session_control( ctrl, &indirect_obj, &session_obj );
  if ( rc ) return APR_EOK;

  in_args = APRV2_PKT_GET_PAYLOAD( vss_imvm_cmd_set_policy_dual_control_t,
                                   ctrl->packet );

  session_obj->is_dual_control = in_args->enable_flag;

  rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EOK );
  MVM_COMM_ERROR( rc, client_addr );

  return APR_EOK;
}

static int32_t mvm_memory_map_cmd_control (
  mvm_pending_control_t* ctrl,
  mvm_mem_mapping_mode_t mapping_mode
)
{
  int32_t rc;
  uint32_t cvd_mem_handle;
  uint32_t vpm_mem_handle;
  uint64_t region_base_virt_addr;
  uint32_t region_size;
  mvm_sequencer_job_object_t* seqjob_obj;
  mvm_simple_job_object_t* subjob_obj;
  vss_imemory_cmd_map_virtual_t* in_args_virtual = NULL;
  vss_imemory_cmd_map_physical_t* in_args_physical = NULL;
  vss_imemory_rsp_map_t map_memory_rsp;
  struct voice_map_args_t {
    voice_cmd_shared_mem_map_regions_t voice_map_memory;
    voice_shared_map_region_payload_t voice_mem_region;
  } voice_map_args;
  voice_cmd_shared_mem_unmap_regions_t voice_unmap_memory;
  uint16_t client_addr = ctrl->packet->src_addr;
  enum {
    MVM_SEQUENCER_ENUM_UNINITIALIZED,
    MVM_SEQUENCER_ENUM_MAP_MEMORY_WITH_CVD,
    MVM_SEQUENCER_ENUM_MAP_MEMORY_WITH_VPM,
    MVM_SEQUENCER_ENUM_WAIT_1,
    MVM_SEQUENCER_ENUM_MAP_MEMORY_WITH_VSM,
    MVM_SEQUENCER_ENUM_WAIT_2,
    MVM_SEQUENCER_ENUM_CLEAN_UP_ON_ERROR,
    MVM_SEQUENCER_ENUM_CLEAN_UP_ON_ERROR_DONE,
    MVM_SEQUENCER_ENUM_COMPLETE,
    MVM_SEQUENCER_ENUM_INVALID
  };

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    if ( mapping_mode == MVM_MEM_MAPPING_MODE_ENUM_VIRTUAL )
    {
      rc = mvm_helper_validate_payload_size_control(
             ctrl, sizeof( vss_imemory_cmd_map_virtual_t ) );
      if ( rc ) return APR_EOK;

      in_args_virtual = APRV2_PKT_GET_PAYLOAD( vss_imemory_cmd_map_virtual_t,
                                               ctrl->packet );
    }
    else
    if ( mapping_mode == MVM_MEM_MAPPING_MODE_ENUM_PHYSICAL )
    {
      rc = mvm_helper_validate_payload_size_control(
             ctrl, sizeof( vss_imemory_cmd_map_physical_t ) );
      if ( rc ) return APR_EOK;

      in_args_physical = APRV2_PKT_GET_PAYLOAD( vss_imemory_cmd_map_physical_t,
                                                ctrl->packet );
    }
    else
    {
      MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
    }

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR( rc );

    seqjob_obj = &ctrl->pendjob_obj->sequencer_job;

    if ( mapping_mode == MVM_MEM_MAPPING_MODE_ENUM_VIRTUAL )
    {
      seqjob_obj->use_case.map_unmap_memory.map_virtual_arg = in_args_virtual;
    }
    else
    {
      seqjob_obj->use_case.map_unmap_memory.map_physical_arg = in_args_physical;
    }

    ctrl->pendjob_obj->sequencer_job.state = MVM_SEQUENCER_ENUM_MAP_MEMORY_WITH_CVD;
  }

  seqjob_obj = &ctrl->pendjob_obj->sequencer_job;

  for ( ;; )
  {
    switch ( seqjob_obj->state )
    {
      case MVM_SEQUENCER_ENUM_MAP_MEMORY_WITH_CVD:
      {
        seqjob_obj->state = MVM_SEQUENCER_ENUM_MAP_MEMORY_WITH_VPM;

        if ( mapping_mode == MVM_MEM_MAPPING_MODE_ENUM_VIRTUAL )
        {
          rc = cvd_mem_mapper_map_virtual(
                 seqjob_obj->use_case.map_unmap_memory.map_virtual_arg,
                 &cvd_mem_handle );
        }
        else
        {
          rc = cvd_mem_mapper_map_physical(
                 seqjob_obj->use_case.map_unmap_memory.map_physical_arg,
                 &cvd_mem_handle );
        }

        if ( rc )
        {
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "mvm_memory_map_cmd_control(): Failed to map memory: mode=%d "
                 "result=0x%08X", mapping_mode, rc );
          seqjob_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          seqjob_obj->status = rc;
          continue;
        }

        seqjob_obj->use_case.map_unmap_memory.mem_handle = cvd_mem_handle;
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_MAP_MEMORY_WITH_VPM:
      {
        seqjob_obj->state = MVM_SEQUENCER_ENUM_WAIT_1;

        rc = mvm_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVM_PANIC_ON_ERROR( rc );

        subjob_obj->fn_table[ MVM_RESPONSE_FN_ENUM_MAP_MEMORY ] =
          mvm_map_memory_rsp_fn;
        seqjob_obj->subjob_obj = ( ( mvm_object_t* ) subjob_obj );

        cvd_mem_handle = seqjob_obj->use_case.map_unmap_memory.mem_handle;

        rc = cvd_mem_mapper_get_region_base_virt_addr(
               cvd_mem_handle, &region_base_virt_addr );
        MVM_PANIC_ON_ERROR( rc );

        rc = cvd_mem_mapper_get_region_size( cvd_mem_handle, &region_size );
        MVM_PANIC_ON_ERROR( rc );

        voice_map_args.voice_map_memory.mem_pool_id = VOICE_MAP_MEMORY_SHMEM8_4K_POOL;
        voice_map_args.voice_map_memory.num_regions = 1;
        voice_map_args.voice_map_memory.property_flag = 1; /* Indicates mapping virtual memory. */
        voice_map_args.voice_mem_region.shm_addr_lsw = ( ( uint32_t ) region_base_virt_addr );
        voice_map_args.voice_mem_region.shm_addr_msw = ( ( uint32_t ) ( region_base_virt_addr >> 32 ) );
        voice_map_args.voice_mem_region.mem_size_bytes = region_size;

        rc = __aprv2_cmd_alloc_send(
               mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
               mvm_my_addr, APR_NULL_V,
               mvm_vpm_addr, APR_NULL_V,
               subjob_obj->header.handle, VOICE_CMD_SHARED_MEM_MAP_REGIONS,
               &voice_map_args, sizeof( voice_map_args ) );
        MVM_COMM_ERROR( rc, mvm_vpm_addr );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_WAIT_1:
      {
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->status = subjob_obj->status;

        if ( seqjob_obj->status == APR_EOK )
        {
          seqjob_obj->state = MVM_SEQUENCER_ENUM_MAP_MEMORY_WITH_VSM;
          cvd_mem_handle = seqjob_obj->use_case.map_unmap_memory.mem_handle;
          rc = cvd_mem_mapper_set_vpm_mem_handle( cvd_mem_handle,
                                                  subjob_obj->context_handle );
          MVM_PANIC_ON_ERROR( rc );
          ( void ) mvm_free_object( ( mvm_object_t* ) subjob_obj );
        }
        else
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "mvm_memory_map_cmd_control(): Failed to map memory with VPM: "
                 "result=0x%08X", seqjob_obj->status );
          seqjob_obj->state = MVM_SEQUENCER_ENUM_CLEAN_UP_ON_ERROR;
          ( void ) mvm_free_object( ( mvm_object_t* ) subjob_obj );
          continue;
        }
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_MAP_MEMORY_WITH_VSM:
      {
        seqjob_obj->state = MVM_SEQUENCER_ENUM_WAIT_2;

        rc = mvm_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVM_PANIC_ON_ERROR( rc );

        subjob_obj->fn_table[ MVM_RESPONSE_FN_ENUM_MAP_MEMORY ] =
          mvm_map_memory_rsp_fn;
        seqjob_obj->subjob_obj = ( ( mvm_object_t* ) subjob_obj );

        cvd_mem_handle = seqjob_obj->use_case.map_unmap_memory.mem_handle;

        rc = cvd_mem_mapper_get_region_base_virt_addr(
               cvd_mem_handle, &region_base_virt_addr );
        MVM_PANIC_ON_ERROR( rc );

        rc = cvd_mem_mapper_get_region_size( cvd_mem_handle, &region_size );
        MVM_PANIC_ON_ERROR( rc );

        voice_map_args.voice_map_memory.mem_pool_id = VOICE_MAP_MEMORY_SHMEM8_4K_POOL;
        voice_map_args.voice_map_memory.num_regions = 1;
        voice_map_args.voice_map_memory.property_flag = 1; /* Indicates mapping virtual memory. */
        voice_map_args.voice_mem_region.shm_addr_lsw = ( ( uint32_t ) region_base_virt_addr );
        voice_map_args.voice_mem_region.shm_addr_msw = ( ( uint32_t ) ( region_base_virt_addr >> 32 ) );
        voice_map_args.voice_mem_region.mem_size_bytes = region_size;

        rc = __aprv2_cmd_alloc_send(
               mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
               mvm_my_addr, APR_NULL_V,
               mvm_vsm_addr, APR_NULL_V,
               subjob_obj->header.handle, VOICE_CMD_SHARED_MEM_MAP_REGIONS,
               &voice_map_args, sizeof( voice_map_args ) );
        MVM_COMM_ERROR( rc, mvm_vsm_addr );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_WAIT_2:
      {
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->status = subjob_obj->status;

        if ( seqjob_obj->status == APR_EOK )
        {
          seqjob_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          cvd_mem_handle = seqjob_obj->use_case.map_unmap_memory.mem_handle;
          rc = cvd_mem_mapper_set_vsm_mem_handle( cvd_mem_handle,
                                                  subjob_obj->context_handle );
          MVM_PANIC_ON_ERROR( rc );
          ( void ) mvm_free_object( ( mvm_object_t* ) subjob_obj );
          continue;
        }
        else
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "mvm_memory_map_cmd_control(): Failed to map memory with VSM: "
                 "result=0x%08X", seqjob_obj->status );
          seqjob_obj->state = MVM_SEQUENCER_ENUM_CLEAN_UP_ON_ERROR;
          ( void ) mvm_free_object( ( mvm_object_t* ) subjob_obj );
        }
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_CLEAN_UP_ON_ERROR:
      { /* Clean up on error. */
        seqjob_obj->state = MVM_SEQUENCER_ENUM_CLEAN_UP_ON_ERROR_DONE;

        rc = mvm_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVM_PANIC_ON_ERROR( rc );
        seqjob_obj->subjob_obj = ( ( mvm_object_t* ) subjob_obj );

        cvd_mem_handle = seqjob_obj->use_case.map_unmap_memory.mem_handle;

        rc = cvd_mem_mapper_get_vpm_mem_handle( cvd_mem_handle, &vpm_mem_handle );
        if ( rc == APR_ENOTEXIST )
        { /* VPM mem handle doesn't exist. */
          subjob_obj->is_completed = TRUE;
          seqjob_obj->status = APR_EOK;
        }
        else
        {
          voice_unmap_memory.mem_map_handle = vpm_mem_handle;

          rc = __aprv2_cmd_alloc_send(
                 mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                 mvm_my_addr, APR_NULL_V,
                 mvm_vpm_addr, APR_NULL_V,
                 subjob_obj->header.handle, VOICE_CMD_SHARED_MEM_UNMAP_REGIONS,
                 &voice_unmap_memory, sizeof( voice_unmap_memory ) );
          MVM_COMM_ERROR( rc, mvm_vpm_addr );
        }
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_CLEAN_UP_ON_ERROR_DONE:
      { /* Finish clean up on error. */
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;

        if ( subjob_obj->status != APR_EOK )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "mvm_memory_map_cmd_control(): Clean up error=0x%08X",
                 subjob_obj->status );
        }

        cvd_mem_handle = seqjob_obj->use_case.map_unmap_memory.mem_handle;
        rc = cvd_mem_mapper_unmap( cvd_mem_handle );
        MVM_PANIC_ON_ERROR( rc );

        ( void ) mvm_free_object( ( mvm_object_t* ) subjob_obj );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_COMPLETE:
      {
        if ( seqjob_obj->status != APR_EOK )
        {
          rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet,
                                        seqjob_obj->status );
          MVM_COMM_ERROR( rc, client_addr );
        }
        else
        {
          map_memory_rsp.mem_handle = seqjob_obj->use_case.map_unmap_memory.mem_handle;

          rc = __aprv2_cmd_alloc_send(
                 mvm_apr_handle, APRV2_PKT_MSGTYPE_CMDRSP_V,
                 mvm_my_addr, APR_NULL_V,
                 ctrl->packet->src_addr, ctrl->packet->src_port,
                 ctrl->packet->token, VSS_IMEMORY_RSP_MAP,
                 &map_memory_rsp, sizeof( map_memory_rsp ) );
          MVM_COMM_ERROR( rc, client_addr );

          /* Track the mem_handle and the client address who maps the memory,
           * in order to unmap the memory in case of SSR for the subsystem
           * where the client resides.
           */
          rc = mvm_ssr_track_mem_handle( client_addr,
                                         map_memory_rsp.mem_handle );
          if ( rc )
          {
            MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                   "mvm_memory_map_cmd_control(): Unable to track mem_handle "
                   "0x%08X, client_addr 0x%04X, rc = 0x%08X",
                   map_memory_rsp.mem_handle, client_addr, rc );
          }

          ( void ) __aprv2_cmd_free( mvm_apr_handle, ctrl->packet );
        }

        ( void ) mvm_free_object( ( mvm_object_t* ) seqjob_obj );
        ctrl->pendjob_obj = NULL;
      }

      return APR_EOK;

    default:
      MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      break;
    }

    break;
  }

  return APR_EPENDING;
}

static int32_t mvm_memory_unmap_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc;
  uint32_t cvd_mem_handle;
  uint32_t vpm_mem_handle;
  uint32_t vsm_mem_handle;
  mvm_sequencer_job_object_t* seqjob_obj;
  mvm_simple_job_object_t* subjob_obj;
  vss_imemory_cmd_unmap_t* in_args;
  voice_cmd_shared_mem_unmap_regions_t voice_unmap_memory;
  uint16_t client_addr = ctrl->packet->src_addr;
  enum {
    MVM_SEQUENCER_ENUM_UNINITIALIZED,
    MVM_SEQUENCER_ENUM_UNMAP_MEMORY_WITH_VPM,
    MVM_SEQUENCER_ENUM_WAIT_1,
    MVM_SEQUENCER_ENUM_UNMAP_MEMORY_WITH_VSM,
    MVM_SEQUENCER_ENUM_WAIT_2,
    MVM_SEQUENCER_ENUM_UNMAP_MEMORY_WITH_CVD,
    MVM_SEQUENCER_ENUM_COMPLETE,
    MVM_SEQUENCER_ENUM_INVALID
  };

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    rc = mvm_helper_validate_payload_size_control(
           ctrl, sizeof( vss_imemory_cmd_unmap_t ) );
    if ( rc ) return APR_EOK;

    in_args = APRV2_PKT_GET_PAYLOAD( vss_imemory_cmd_unmap_t,
                                     ctrl->packet );

    rc = cvd_mem_mapper_validate_handle( in_args->mem_handle );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "mvm_memory_unmap_cmd_control(): Invalid mem_handle 0x%08X.",
             in_args->mem_handle );
      rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EHANDLE );
      MVM_COMM_ERROR( rc, client_addr );
      return APR_EOK;
    }

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR( rc );

    seqjob_obj = &ctrl->pendjob_obj->sequencer_job;
    seqjob_obj->use_case.map_unmap_memory.mem_handle = in_args->mem_handle;

    ctrl->pendjob_obj->sequencer_job.state = MVM_SEQUENCER_ENUM_UNMAP_MEMORY_WITH_VPM;
  }

  seqjob_obj = &ctrl->pendjob_obj->sequencer_job;
  cvd_mem_handle = seqjob_obj->use_case.map_unmap_memory.mem_handle;

  for ( ;; )
  {
    switch ( seqjob_obj->state )
    {
      case MVM_SEQUENCER_ENUM_UNMAP_MEMORY_WITH_VPM:
      { /* Unmap virtual memory with VPM. */
        ( void ) cvd_mem_mapper_get_vpm_mem_handle( cvd_mem_handle, &vpm_mem_handle );

        seqjob_obj->state = MVM_SEQUENCER_ENUM_WAIT_1;
        rc = mvm_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVM_PANIC_ON_ERROR( rc );

        seqjob_obj->subjob_obj = ( ( mvm_object_t* ) subjob_obj );

        voice_unmap_memory.mem_map_handle = vpm_mem_handle;

        rc = __aprv2_cmd_alloc_send(
               mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
               mvm_my_addr, APR_NULL_V,
               mvm_vpm_addr, APR_NULL_V,
               subjob_obj->header.handle, VOICE_CMD_SHARED_MEM_UNMAP_REGIONS,
               &voice_unmap_memory, sizeof( voice_unmap_memory ) );
        MVM_COMM_ERROR( rc, mvm_vpm_addr );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_WAIT_1:
      {
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->status = subjob_obj->status;

        ( void ) mvm_free_object( ( mvm_object_t* ) subjob_obj );

        if ( seqjob_obj->status == APR_EOK )
        {
          seqjob_obj->state = MVM_SEQUENCER_ENUM_UNMAP_MEMORY_WITH_VSM;
        }
        else
        {
          seqjob_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          continue;
        }
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_UNMAP_MEMORY_WITH_VSM:
      { 
        ( void ) cvd_mem_mapper_get_vsm_mem_handle( cvd_mem_handle, &vsm_mem_handle );

        seqjob_obj->state = MVM_SEQUENCER_ENUM_WAIT_2;
        rc = mvm_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVM_PANIC_ON_ERROR( rc );

        seqjob_obj->subjob_obj = ( ( mvm_object_t* ) subjob_obj );

        voice_unmap_memory.mem_map_handle = vsm_mem_handle;

        rc = __aprv2_cmd_alloc_send(
               mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
               mvm_my_addr, APR_NULL_V,
               mvm_vsm_addr, APR_NULL_V,
               subjob_obj->header.handle, VOICE_CMD_SHARED_MEM_UNMAP_REGIONS,
               &voice_unmap_memory, sizeof( voice_unmap_memory ) );
        MVM_COMM_ERROR( rc, mvm_vsm_addr );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_WAIT_2:
      {
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->status = subjob_obj->status;

        ( void ) mvm_free_object( ( mvm_object_t* ) subjob_obj );

        if ( seqjob_obj->status == APR_EOK )
        {
          seqjob_obj->state = MVM_SEQUENCER_ENUM_UNMAP_MEMORY_WITH_CVD;
        }
        else
        {
          seqjob_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
          continue;
        }
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_UNMAP_MEMORY_WITH_CVD:
      {
        seqjob_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;

        rc = cvd_mem_mapper_unmap( cvd_mem_handle );
        seqjob_obj->status = rc;
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_COMPLETE:
      {
        if ( seqjob_obj->status == APR_EOK )
        { /* Untrack the mem_handle after memory unmap. */
          ( void ) mvm_ssr_untrack_mem_handle( cvd_mem_handle );
        }

        rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet,
                                      seqjob_obj->status );
        MVM_COMM_ERROR( rc, client_addr );
        ( void ) mvm_free_object( ( mvm_object_t* ) seqjob_obj );
        ctrl->pendjob_obj = NULL;
      }

      return APR_EOK;

      default:
        MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
        break;
    }

    break;
  }

  return APR_EPENDING;
}

static int32_t mvm_ssr_cleanup_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  vss_issr_cmd_cleanup_t* in_args = NULL;
  mvm_sequencer_job_object_t* seqjob_obj = NULL;
  mvm_simple_job_object_t* subjob_obj = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;
  enum {
    MVM_SEQUENCER_ENUM_UNINITIALIZED,
    MVM_SEQUENCER_ENUM_STOP_VOICE,
    MVM_SEQUENCER_ENUM_WAIT_1,
    MVM_SEQUENCER_ENUM_DETACH_STREAM,
    MVM_SEQUENCER_ENUM_WAIT_2,
    MVM_SEQUENCER_ENUM_DESTROY_SESSION,
    MVM_SEQUENCER_ENUM_WAIT_3,
    MVM_SEQUENCER_ENUM_CLEANUP_CVS,
    MVM_SEQUENCER_ENUM_WAIT_4,
    MVM_SEQUENCER_ENUM_CLEANUP_CVP,
    MVM_SEQUENCER_ENUM_WAIT_5,
    MVM_SEQUENCER_ENUM_UNMAP_MEMORY,
    MVM_SEQUENCER_ENUM_WAIT_6,
    MVM_SEQUENCER_ENUM_COMPLETE,
    MVM_SEQUENCER_ENUM_INVALID
  };

  if ( ctrl->state == MVM_PENDING_CMD_STATE_ENUM_EXECUTE )
  {
    rc = mvm_helper_validate_payload_size_control(
           ctrl, sizeof( vss_issr_cmd_cleanup_t ) );
    if ( rc ) return APR_EOK;

    in_args = APRV2_PKT_GET_PAYLOAD( vss_issr_cmd_cleanup_t, ctrl->packet );

    rc = mvm_create_sequencer_job_object(
           ( mvm_sequencer_job_object_t** ) &ctrl->pendjob_obj );
    MVM_PANIC_ON_ERROR( rc );

    seqjob_obj = &ctrl->pendjob_obj->sequencer_job;
    seqjob_obj->use_case.ssr_cleanup.domain_id = in_args->domain_id;

    ctrl->pendjob_obj->sequencer_job.state = MVM_SEQUENCER_ENUM_STOP_VOICE;
  }

  seqjob_obj = &ctrl->pendjob_obj->sequencer_job;

  for ( ;; )
  {
    switch ( seqjob_obj->state )
    {
      case MVM_SEQUENCER_ENUM_STOP_VOICE:
      {
        /* Stop each of the MVM session on behalf of MVM clients who reside in
         * a subsystem that is being restarted. */
        seqjob_obj->state = MVM_SEQUENCER_ENUM_WAIT_1;

        ( void ) mvm_ssr_stop_voice(
                   seqjob_obj->use_case.ssr_cleanup.domain_id );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_WAIT_1:
      { /* Wait for MVM to respond for each stop voice command. */
        if ( mvm_ssr_cleanup_cmd_tracking.rsp_cnt <
             mvm_ssr_cleanup_cmd_tracking.num_cmd_issued )
        {
          return APR_EPENDING;
        }

        seqjob_obj->state = MVM_SEQUENCER_ENUM_DETACH_STREAM;
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_DETACH_STREAM:
      {
        /* For each of the MVM sessions, detach the streams currently attached
         * to it on behalf of MVM clients who reside in a subsystem that is
         * being restarted.
         */
        seqjob_obj->state = MVM_SEQUENCER_ENUM_WAIT_2;

        ( void ) mvm_ssr_detach_stream(
                   seqjob_obj->use_case.ssr_cleanup.domain_id );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_WAIT_2:
      { /* Wait for MVM to respond for each detach stream command. */
        if ( mvm_ssr_cleanup_cmd_tracking.rsp_cnt <
             mvm_ssr_cleanup_cmd_tracking.num_cmd_issued )
        {
          return APR_EPENDING;
        }

        seqjob_obj->state = MVM_SEQUENCER_ENUM_DESTROY_SESSION;
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_DESTROY_SESSION:
      {
        /* Destroy the MVM session control handles (indirection handles) on
         * behalf of MVM clients who reside in a subsystem that is being
         * restarted.
         */
        seqjob_obj->state = MVM_SEQUENCER_ENUM_WAIT_3;

        ( void ) mvm_ssr_destroy_session(
                   seqjob_obj->use_case.ssr_cleanup.domain_id );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_WAIT_3:
      { /* Wait for MVM to respond for each destroy session command. */
        if ( mvm_ssr_cleanup_cmd_tracking.rsp_cnt <
             mvm_ssr_cleanup_cmd_tracking.num_cmd_issued )
        {
          return APR_EPENDING;
        }

        seqjob_obj->state = MVM_SEQUENCER_ENUM_CLEANUP_CVS;
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_CLEANUP_CVS:
      { /* Request CVS to cleanup for SSR. */
        seqjob_obj->state = MVM_SEQUENCER_ENUM_WAIT_4;

        rc = mvm_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVM_PANIC_ON_ERROR( rc );
        seqjob_obj->subjob_obj = ( ( mvm_object_t* ) subjob_obj );

        rc = __aprv2_cmd_alloc_send(
               mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
               mvm_my_addr, APR_NULL_V,
               mvm_cvs_addr, APR_NULL_V,
               subjob_obj->header.handle,
               VSS_ISSR_CMD_CLEANUP,
               APRV2_PKT_GET_PAYLOAD( void, ctrl->packet ),
               APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( ctrl->packet->header ) );
        MVM_COMM_ERROR( rc, mvm_cvs_addr );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_WAIT_4:
      { /* Wait for CVS to finish cleanup. */
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->state = MVM_SEQUENCER_ENUM_CLEANUP_CVP;

        ( void ) mvm_free_object( ( mvm_object_t* ) subjob_obj );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_CLEANUP_CVP:
      { /* Request CVP to cleanup for SSR. */
        seqjob_obj->state = MVM_SEQUENCER_ENUM_WAIT_5;

        rc = mvm_create_simple_job_object( APR_NULL_V, &subjob_obj );
        MVM_PANIC_ON_ERROR( rc );
        seqjob_obj->subjob_obj = ( ( mvm_object_t* ) subjob_obj );

        rc = __aprv2_cmd_alloc_send(
               mvm_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
               mvm_my_addr, APR_NULL_V,
               mvm_vpm_addr, APR_NULL_V,
               subjob_obj->header.handle,
               VSS_ISSR_CMD_CLEANUP,
               APRV2_PKT_GET_PAYLOAD( void, ctrl->packet ),
               APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( ctrl->packet->header ) );
        MVM_COMM_ERROR( rc, mvm_vpm_addr );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_WAIT_5:
      { /* Wait for CVP to finish cleanup. */
        subjob_obj = &seqjob_obj->subjob_obj->simple_job;

        if ( subjob_obj->is_completed == FALSE )
        {
          return APR_EPENDING;
        }

        seqjob_obj->state = MVM_SEQUENCER_ENUM_UNMAP_MEMORY;

        ( void ) mvm_free_object( ( mvm_object_t* ) subjob_obj );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_UNMAP_MEMORY:
      {
        /* Unmap memory on behalf of MVM clients who reside in a subsystem
         * that is being restarted.
         */
        seqjob_obj->state = MVM_SEQUENCER_ENUM_WAIT_6;

        ( void ) mvm_ssr_unmap_memory(
                   seqjob_obj->use_case.ssr_cleanup.domain_id );
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_WAIT_6:
      { /* Wait for MVM to respond to each memory unmap command. */
        if ( mvm_ssr_cleanup_cmd_tracking.rsp_cnt <
             mvm_ssr_cleanup_cmd_tracking.num_cmd_issued )
        {
          return APR_EPENDING;
        }

        seqjob_obj->state = MVM_SEQUENCER_ENUM_COMPLETE;
        seqjob_obj->status = APR_EOK;
      } /*-fallthru */

      case MVM_SEQUENCER_ENUM_COMPLETE:
      { /* End the sequencer. */
        rc = __aprv2_cmd_end_command(
               mvm_apr_handle, ctrl->packet, seqjob_obj->status );
        MVM_COMM_ERROR( rc, client_addr );

        ( void ) mvm_free_object( ( mvm_object_t* ) seqjob_obj );
        ctrl->pendjob_obj = NULL;
      }
      return APR_EOK;

    default:
      MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      break;
    }

    break;
  }

  return APR_EPENDING;
}

static int32_t mvm_version_get_cmd_control (
  mvm_pending_control_t* ctrl
)
{
  int32_t rc = APR_EOK;
  uint16_t client_addr = ctrl->packet->src_addr;

  rc = __aprv2_cmd_alloc_send(
         mvm_apr_handle, APRV2_PKT_MSGTYPE_CMDRSP_V,
         mvm_my_addr, APR_NULL_V,
         client_addr, ctrl->packet->src_port,
         ctrl->packet->token, VSS_IVERSION_RSP_GET,
         mvm_version, sizeof( mvm_version ) );
  MVM_COMM_ERROR( rc, client_addr );

  /* Free the incoming command packet. */
  ( void ) __aprv2_cmd_free( mvm_apr_handle, ctrl->packet );

  return APR_EOK;
}

static uint32_t mvm_listen_for_event_class_cmd_ctrl (
  mvm_pending_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint32_t status = APR_EOK;
  vss_inotify_cmd_listen_for_event_class_t* in_args = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;

  in_args = APRV2_PKT_GET_PAYLOAD( vss_inotify_cmd_listen_for_event_class_t,
                                   ctrl->packet );

  switch( in_args->class_id )
  {
    case VSS_ICOMMON_EVENT_CLASS_VOICE_ACTIVITY_UPDATE:
    {
      if( TRUE == voice_activity_client.is_enabled )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "MVM: LISTEN_EVENT_CLASS:(0x%08X) listening ALREADY",
               in_args->class_id );
        status = APR_EALREADY;
        break;
      }

      /* Cache client's address and port. */
      voice_activity_client.is_enabled = TRUE;
      voice_activity_client.client_addr = ctrl->packet->src_addr;
      voice_activity_client.client_port = ctrl->packet->src_port;
    }
    break;

    default:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "MVM: LISTEN_EVENT_CLASS:(0x%08X) UNSUPPORTED", in_args->class_id );
      status = APR_EUNSUPPORTED;
    }
    break;
  }

  rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, status );
  MVM_COMM_ERROR( rc, client_addr );

  return APR_EOK;
}

static int32_t mvm_cancel_event_class_cmd_ctrl (
  mvm_pending_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint32_t status = APR_EOK;
  vss_inotify_cmd_cancel_event_class_t* in_args = NULL;
  uint16_t client_addr = ctrl->packet->src_addr;
  
  in_args = APRV2_PKT_GET_PAYLOAD( vss_inotify_cmd_cancel_event_class_t,
                                   ctrl->packet );

  switch( in_args->class_id )
  {
    case VSS_ICOMMON_EVENT_CLASS_VOICE_ACTIVITY_UPDATE:
    {
      if( FALSE == voice_activity_client.is_enabled )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "MVM: CANCEL_EVENT_CLASS:(0x%08X) cancelled ALREADY",
               in_args->class_id  );
        status = APR_EALREADY;
        break;
      }

      voice_activity_client.is_enabled = FALSE;
      voice_activity_client.client_addr = APR_NULL_V;
      voice_activity_client.client_port = APR_NULL_V;
    }
    break;
  
    default:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "MVM: CANCEL_EVENT_CLASS:(0x%08X) UNSUPPORTED", in_args->class_id );
      status = APR_EUNSUPPORTED;
    }
    break;
  }

  rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, status );
  MVM_COMM_ERROR( rc, client_addr );

  return APR_EOK;
}

/****************************************************************************
 * CMDs/EVTs/RSPs DISPATCHING ROUTINES                                      *
 ****************************************************************************/

static int32_t mvm_isr_dispatch_fn (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  /* TODO: Queue our client and our server messages into separate lists. */
  mvm_queue_incoming_packet( packet );

  return APR_EOK;
}

static void mvm_response_fn_trampoline (
  uint32_t fn_index,
  aprv2_packet_t* packet
)
{
  int32_t rc;
  mvm_object_t* object;
  uint32_t msg_type;

  msg_type = APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header );

  if ( msg_type == APRV2_PKT_MSGTYPE_EVENT_V )
  {
    rc = mvm_get_object( packet->dst_port, &object );
  }
  else
  {
    rc = mvm_get_object( packet->token, &object );
  }

  if ( rc == APR_EOK )
  {
    switch ( object->header.type )
    {
     case MVM_OBJECT_TYPE_ENUM_SIMPLE_JOB:
       object->simple_job.fn_table[ fn_index ]( packet );
       return;

     default:
       break;
    }
  }

  ( void ) __aprv2_cmd_free( mvm_apr_handle, packet );
}

static int32_t mvm_task_process_nongating_commands ( void )
{
  mvm_work_item_t* work_item;
  aprv2_packet_t* packet;

  while ( apr_list_remove_head( &mvm_task_incoming_cmd_q,
                                ( ( apr_list_node_t** ) &work_item ) )
          == APR_EOK )
  {
    packet = ( ( aprv2_packet_t* ) work_item->packet );
    ( void ) apr_list_add_tail( &mvm_free_cmd_q, &work_item->link );

    switch ( packet->opcode )
    {
     case APRV2_IBASIC_EVT_ACCEPTED:
       /* TODO: Events use dst_port, but trampoline use token for results. */
       mvm_response_fn_trampoline( MVM_RESPONSE_FN_ENUM_ACCEPTED, packet );
       break;

     case APRV2_IBASIC_RSP_RESULT:
       mvm_response_fn_trampoline( MVM_RESPONSE_FN_ENUM_RESULT, packet );
       break;
 
     case VOICE_RSP_SHARED_MEM_MAP_REGIONS:
       mvm_response_fn_trampoline( MVM_RESPONSE_FN_ENUM_MAP_MEMORY, packet );
       break;
 
     case VSS_ICOMMON_RSP_SET_SYSTEM_CONFIG:
       mvm_response_fn_trampoline( MVM_RESPONSE_FN_ENUM_SET_SYSTEM_CONFIG, packet );
       break;
 
     case VSS_IVOCPROC_RSP_GET_AVSYNC_DELAYS:
       mvm_response_fn_trampoline( MVM_RESPONSE_FN_ENUM_GET_AVSYNC_DELAYS, packet );
       break;
 
     case VSS_IPKTEXG_RSP_GET_MODE:
       mvm_response_fn_trampoline( MVM_RESPONSE_FN_ENUM_GET_PKTEXG_MODE, packet );
       break;

     case VSS_IPKTEXG_RSP_MAILBOX_GET_TIME_REFERENCE:
       mvm_response_fn_trampoline( MVM_RESPONSE_FN_ENUM_GET_MAILBOX_PKTEXG_TIME_REFERENCE, packet );
       break;
 
     case VSS_IHDVOICE_RSP_GET_CONFIG:
       mvm_response_fn_trampoline( MVM_RESPONSE_FN_ENUM_GET_HDVOICE_CONFIG, packet );
       break;

     case VSS_IVFR_RSP_OPEN:
       mvm_response_fn_trampoline( MVM_RESPONSE_FN_ENUM_OPEN_SOFT_VFR, packet );
       break;
 
     case VSS_IAVTIMER_CMD_GET_TIME:
       ( void ) mvm_get_avtimer_time_cmd_control( packet );
       break;

     case VSS_IAVTIMER_CMD_GET_OFFSET:
       ( void ) mvm_get_avtimer_offset_cmd_control( packet );
       break;

     case VSS_IVOCPROC_EVT_RECONFIG:
       ( void ) mvm_reconfig_vocproc_evt_control( packet );
       break;

     case VSS_ISTREAM_EVT_RECONFIG:
       ( void ) mvm_reconfig_stream_evt_control( packet );
       break;

     case VSS_ISTREAM_EVT_VOC_OPERATING_MODE_UPDATE:
       ( void ) mvm_update_voc_operating_mode_evt_control( packet );
       break;
 
     case VSS_ICCM_EVT_ACTIVE_SESSIONS:
       ( void ) mvm_ccm_active_sessions_evt_control( packet );
       break;
 
     case VSS_ICOMMON_EVT_VOICE_ACTIVITY_UPDATE:
       ( void ) mvm_broadcast_voice_activity_evt_control( packet );
       break;
 
     case VSS_ISSR_CMD_CLEANUP:
       mvm_queue_pending_packet( &mvm_ssr_pending_ctrl.cmd_q, packet );
       break;
 
     case VSS_ICOMMON_EVT_TOGGLE_CSM:
       ( void ) mvm_toggle_csm_evt_control( packet );
       break;

     case VSS_IMVM_CSM_CMD_REQUEST_VOTE:
       ( void ) mvm_csm_request_vote_cmd_control( packet );
       break;

     case VSS_IMVM_CMD_QUERY_OBJECT_LEAKS:
      ( void ) mvm_query_object_leak( packet );
      break;
 
     default:
       mvm_queue_pending_packet( &mvm_task_pending_ctrl.cmd_q, packet );
       break;
    }
  }
  return APR_EOK;
}

static int32_t mvm_task_process_gating_commands ( void )
{
  int32_t rc;
  mvm_pending_control_t* ctrl = &mvm_task_pending_ctrl;
  mvm_work_item_t* work_item;
  uint16_t client_addr;

  for ( ;; )
  {
    switch ( ctrl->state )
    {
      case MVM_PENDING_CMD_STATE_ENUM_FETCH:
      {
        { /* Fetch the next pending command to execute. */
          rc = apr_list_remove_head( &ctrl->cmd_q,
                                     ( ( apr_list_node_t** ) &work_item ) );
          if ( rc )
          { /* Return when the pending command queue is empty. */
            return APR_EOK;
          }
          ctrl->packet = work_item->packet;
          ( void ) apr_list_add_tail( &mvm_free_cmd_q, &work_item->link );
        }
        ctrl->state = MVM_PENDING_CMD_STATE_ENUM_EXECUTE;
      }
      break;

      case MVM_PENDING_CMD_STATE_ENUM_EXECUTE:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "MVM: PROCESS_GATING_COMMANDs: OPCODE:(0X%08X) STARTED",
                ctrl->packet->opcode );
        /* FALL THOURGH. */
      case MVM_PENDING_CMD_STATE_ENUM_CONTINUE:
      {
        switch ( ctrl->packet->opcode )
        {
          case VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION:
            rc = mvm_core_create_session_cmd_control( ctrl, TRUE );
            break;

          case VSS_IMVM_CMD_CREATE_PASSIVE_CONTROL_SESSION:
            rc = mvm_core_create_session_cmd_control( ctrl, FALSE );
            break;
  
          case APRV2_IBASIC_CMD_DESTROY_SESSION:
            rc = mvm_core_destroy_session_cmd_control( ctrl );
            break;
  
          case VSS_IMVM_CMD_MODEM_START_VOICE:
            rc = mvm_modem_start_voice_cmd_control( ctrl );
            break;

          /* NOTE: Both the Interface MODEM_STOP/MODEM_STANDBY provides same 
           * functionality, hence MODEM_STANDBY is redudant.*/
          case VSS_IMVM_CMD_MODEM_STANDBY_VOICE:
          case VSS_IMVM_CMD_MODEM_STOP_VOICE:
            rc = mvm_modem_stop_voice_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_START_VOICE:
            rc = mvm_start_voice_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_STANDBY_VOICE:
            rc = mvm_standby_voice_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_PAUSE_VOICE:
            rc = mvm_pause_voice_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_STOP_VOICE:
            rc = mvm_stop_voice_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_ATTACH_STREAM:
            rc = mvm_attach_stream_cmd_control( ctrl );
            break;
  
          case VSS_IMVM_CMD_DETACH_STREAM:
            rc = mvm_detach_stream_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_ATTACH_VOCPROC:
          case VSS_ISTREAM_CMD_ATTACH_VOCPROC: /* BACKWARD COMPATIBILITY */
            rc = mvm_attach_vocproc_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_DETACH_VOCPROC:
          case VSS_ISTREAM_CMD_DETACH_VOCPROC: /* BACKWARD COMPATIBILITY */
            rc = mvm_detach_vocproc_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE:
            rc = mvm_set_cal_media_type_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_SET_MAX_VAR_VOC_SAMPLING_RATE:
            rc = mvm_set_max_var_voc_sampling_rate_cmd_control( ctrl );
            break;
  
          case VSS_IMVM_CMD_SET_CAL_NETWORK:
            rc = mvm_set_cal_network_cmd_control( ctrl );
            break;

          case VSS_ICOMMON_CMD_SET_NETWORK: /* BACKWARD COMPATIBILITY */
            rc = mvm_common_set_network_cmd_control( ctrl );
            break;

          case VSS_ICOMMON_CMD_SET_VOICE_TIMING: /* BACKWARD COMPATIBILITY */
           rc = mvm_set_voice_timing_cmd_control( ctrl );
           break;

          case VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2:
            rc = mvm_set_voice_timing_v2_cmd_control( ctrl );
            break;

          case VSS_ICOMMON_CMD_SET_VFR_CONFIG:
           rc = mvm_set_vfr_config_cmd_control( ctrl );
           break;

          case VSS_ICOMMON_CMD_SET_VOICE_TIMING_REFERENCE:
           rc = mvm_set_voice_timing_reference_cmd_control( ctrl );
           break;

          case VSS_ITTY_CMD_SET_TTY_MODE:
            rc = mvm_set_tty_mode_cmd_control( ctrl );
            break;

          case VSS_IWIDEVOICE_CMD_SET_WIDEVOICE: /* BACKWARD COMPATIBILITY */
            rc = mvm_widevoice_set_widevoice_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL:
          case VSS_IMVM_CMD_SET_MODEM_VOICE_CONTROL: /* BACKWARD COMPATIBILITY */
            rc = mvm_set_policy_dual_control_cmd_control( ctrl );
            break;
  
          case VSS_IMEMORY_CMD_MAP_VIRTUAL:
            rc = mvm_memory_map_cmd_control( ctrl, MVM_MEM_MAPPING_MODE_ENUM_VIRTUAL );
            break;
  
          case VSS_IMEMORY_CMD_MAP_PHYSICAL:
            rc = mvm_memory_map_cmd_control( ctrl, MVM_MEM_MAPPING_MODE_ENUM_PHYSICAL );
            break;
  
          case VSS_IMEMORY_CMD_UNMAP:
            rc = mvm_memory_unmap_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_RECONFIG:
            rc = mvm_reconfig_cmd_control( ctrl );
            break;

          case VSS_IMVM_CMD_DYNAMIC_RECONFIG:
            rc = mvm_dynamic_reconfig_cmd_control( ctrl );
            break;
  
          case VSS_IVERSION_CMD_GET:
            rc = mvm_version_get_cmd_control( ctrl );
            break;

          case VSS_IHDVOICE_CMD_ENABLE:
          case VSS_IHDVOICE_CMD_DISABLE:
          case VSS_IHDVOICE_CMD_BEAMR_DISABLE:
          case VSS_IHDVOICE_CMD_WV2_DISABLE:
          case VSS_IHDVOICE_CMD_BEAMR_ENABLE:
          case VSS_IHDVOICE_CMD_WV2_ENABLE:
            rc = mvm_hdvoice_ui_cmd_control ( ctrl );
            break;

          case VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS:
            rc = mvm_listen_for_event_class_cmd_ctrl( ctrl );
            break;
  
          case VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS:
            rc = mvm_cancel_event_class_cmd_ctrl( ctrl );
            break;

          default:
          { /* Handle error. */
            client_addr = ctrl->packet->src_addr;

            rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EUNSUPPORTED );
            MVM_COMM_ERROR( rc, client_addr );
            rc = APR_EOK;
          }
          break;
        }

        /* Evaluate the pending command completion status. */
        switch ( rc )
        {
          case APR_EOK:
           /* The current command is finished so fetch the next command. */
           MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
                  "MVM: PROCESS_GATING_COMMANDs: OPCODE:(0X%08X) ENDED",
                   ctrl->packet->opcode );
           ctrl->state = MVM_PENDING_CMD_STATE_ENUM_FETCH;
           break;

          case APR_EPENDING:
           /* Assuming the current pending command control routine returns
            * APR_EPENDING the overall progress stalls until one or more
            * external events or responses are received. */
           ctrl->state = MVM_PENDING_CMD_STATE_ENUM_CONTINUE;
           return rc;

          default:
            MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
            break;
        }
      }
      break;

    default:
      MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      return rc;
    }
  }

  return rc;
}

static int32_t mvm_thread_process_ssr_gating_commands ( void )
{
  int32_t rc;
  mvm_pending_control_t* ctrl = &mvm_ssr_pending_ctrl;
  mvm_work_item_t* work_item;
  uint16_t client_addr;

  for ( ;; )
  {
    switch ( ctrl->state )
    {
      case MVM_PENDING_CMD_STATE_ENUM_FETCH:
      {
        { /* Fetch the next pending command to execute. */
          rc = apr_list_remove_head( &ctrl->cmd_q,
                                     ( ( apr_list_node_t** ) &work_item ) );
          if ( rc )
          { /* Return when the pending command queue is empty. */
            return APR_EOK;
          }
          ctrl->packet = work_item->packet;
          ( void ) apr_list_add_tail( &mvm_free_cmd_q, &work_item->link );
        }
        ctrl->state = MVM_PENDING_CMD_STATE_ENUM_EXECUTE;
      }
      break;

      case MVM_PENDING_CMD_STATE_ENUM_EXECUTE:
      case MVM_PENDING_CMD_STATE_ENUM_CONTINUE:
      {
        switch ( ctrl->packet->opcode )
        {
          case VSS_ISSR_CMD_CLEANUP:
            rc = mvm_ssr_cleanup_cmd_control( ctrl );
            break;

          default:
            { /* Handle error. */
              client_addr = ctrl->packet->src_addr;
              rc = __aprv2_cmd_end_command( mvm_apr_handle, ctrl->packet, APR_EUNSUPPORTED );
              MVM_COMM_ERROR( rc, client_addr );
              rc = APR_EOK;
            }
            break;
        }

        /* Evaluate the pending command completion status. */
        switch ( rc )
        {
          case APR_EOK:
            /* The current command is finished so fetch the next command. */
            ctrl->state = MVM_PENDING_CMD_STATE_ENUM_FETCH;
            break;

          case APR_EPENDING:
            /* The overall progress stalls until one or more external events or 
             * responses are received. */
            ctrl->state = MVM_PENDING_CMD_STATE_ENUM_CONTINUE;
            return rc;

          default:
            MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
            break;
        }
      }
      break;

    default:
      MVM_PANIC_ON_ERROR( APR_EUNEXPECTED );
      return APR_EOK;
    }
  }

  return APR_EOK;
}

/****************************************************************************
 * TASK ROUTINES                                                            *
 ****************************************************************************/

static int32_t mvm_run_task ( void )
{
  uint32_t rc1 = APR_EOK, rc3 = APR_EOK, rc2 = APR_EOK;
  vss_imvm_csm_cmd_request_vote_t params;

  if ( FALSE == mvm_csm_vote ) 
  {
     mvm_csm_vote = TRUE;
     params.client = MVM_CSM_CLIENT_ENUM_MVM;
     params.vote = TRUE;
     ( void )mvm_call(VSS_IMVM_CSM_CMD_REQUEST_VOTE, &params, sizeof( params ) );
  }

  rc1 = mvm_task_process_nongating_commands( );
  rc2 = mvm_task_process_gating_commands( );
  rc3 = mvm_thread_process_ssr_gating_commands( );

  if ( ( TRUE == mvm_csm_vote ) && ( APR_EPENDING != rc1 ) &&
       ( APR_EPENDING != rc2 ) && ( APR_EPENDING != rc3 ) )
  {
     mvm_csm_vote = FALSE;
     params.client = MVM_CSM_CLIENT_ENUM_MVM;
     params.vote = FALSE;
     ( void )mvm_call(VSS_IMVM_CSM_CMD_REQUEST_VOTE, &params, sizeof( params ) );
  }
  return APR_EOK;
}

static int32_t mvm_task ( void* param )
{
  int32_t rc;

  rc = apr_event_create( &mvm_task_event );
  MVM_PANIC_ON_ERROR( rc );

  mvm_task_state = MVM_THREAD_STATE_ENUM_READY;
  apr_event_signal( mvm_thread_event );

  do
  {
    rc = apr_event_wait( mvm_task_event );
   ( void ) mvm_run_task( );
  }
  while ( rc == APR_EOK );

  rc = apr_event_destroy( mvm_task_event );
  MVM_PANIC_ON_ERROR( rc );

  mvm_task_state = MVM_THREAD_STATE_ENUM_EXIT;

  return APR_EOK;
}

static uint32_t mvm_run ( void )
{
  mvm_run_task( );
  return APR_EOK;
}

/****************************************************************************
 * EXTERNAL API ROUTINES                                                    *
 ****************************************************************************/

static int32_t mvm_init ( void )
{
  uint32_t rc;
#ifndef WINSIM
  RCECB_HANDLE ssr_handle;
#endif /*WINSIM */
  uint32_t i;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== mvm_init()======" );

  { /* Initialize the locks. */
    ( void ) apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &mvm_int_lock );
    ( void ) apr_lock_create( APR_LOCK_TYPE_MUTEX, &mvm_task_lock );
  }
  { /* Initialize the custom heap. */
    apr_memmgr_init_heap( &mvm_heapmgr, ( ( void* ) &mvm_heap_pool ),
                          sizeof( mvm_heap_pool ), NULL, NULL );
      /* memheap mustn't be called from interrupt context. No locking is
       * required in task context because all commands are serialized.
       */
  }
  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;

    params.table = mvm_object_table;
    params.total_bits = MVM_HANDLE_TOTAL_BITS_V;
    params.index_bits = MVM_HANDLE_INDEX_BITS_V;
    params.lock_fn = mvm_int_lock_fn;
    params.unlock_fn = mvm_int_unlock_fn;
    ( void ) apr_objmgr_construct( &mvm_objmgr, &params );
      /* TODO: When the ISR and task context contentions becomes a problem
       *       the objmgr could be split into ISR/task and task-only stores
       *       to reduce the bottleneck.
       */
  }
  { /* Initialize the session management. */
    ( void ) mvm_pending_control_init( &mvm_task_pending_ctrl );
  }
  { /* Initialize the SSR management. */
    ( void ) mvm_pending_control_init( &mvm_ssr_pending_ctrl );

    mvm_ssr_cleanup_cmd_tracking.num_cmd_issued = 0;
    mvm_ssr_cleanup_cmd_tracking.rsp_cnt = 0;

    ( void ) apr_list_init_v2( &mvm_ssr_mem_handle_tracking.free_q, NULL, NULL );
    for ( i = 0; i < MVM_SSR_MAX_TRACKED_MEM_HANDLES; ++i )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &mvm_ssr_mem_handle_tracking.pool[ i ] );
      ( void ) apr_list_add_tail(
                 &mvm_ssr_mem_handle_tracking.free_q,
                 ( ( apr_list_node_t* ) &mvm_ssr_mem_handle_tracking.pool[ i ] ) );
    }

    ( void ) apr_list_init_v2( &mvm_ssr_mem_handle_tracking.used_q, NULL, NULL );
  }

#ifndef WINSIM
  { /* Register for system monitor SSR callbacks. */
    ssr_handle = rcecb_register_name( SYS_M_SSR_MODEM_AFTER_SHUTDOWN,
                                      mvm_ssr_modem_after_shutdown_handler );
    if ( ssr_handle == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_init: Failed to register for SYS_M_SSR_MODEM_AFTER_SHUTDOWN callback" );
    }

    ssr_handle = rcecb_register_name( SYS_M_SSR_MODEM_AFTER_POWERUP,
                                      mvm_ssr_modem_after_powerup_handler );
    if ( ssr_handle == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "mvm_init: Failed to register for SYS_M_SSR_MODEM_AFTER_POWERUP callback" );
    }
  }
#endif /* !WINSIM */

  { /* Initialize the command queue management. */
    { /* Populate the free command structures */
      ( void ) apr_list_init_v2( &mvm_free_cmd_q,
                                 mvm_int_lock_fn, mvm_int_unlock_fn );
      for ( i = 0; i < MVM_NUM_COMMANDS_V; ++i )
      {
        ( void ) apr_list_init_node( ( apr_list_node_t* ) &mvm_cmd_pool[ i ] );
        ( void ) apr_list_add_tail( &mvm_free_cmd_q,
                                    ( ( apr_list_node_t* ) &mvm_cmd_pool[i] ) );
      }
    }
    ( void ) apr_list_init_v2( &mvm_task_incoming_cmd_q,
                               mvm_med_task_lock_fn, mvm_med_task_unlock_fn );
  }
  { /* Initialize the MVM session tracking list. */
    ( void ) apr_list_init_v2( &mvm_session_list_free_q, NULL, NULL );
    for ( i = 0; i < MVM_MAX_SESSIONS; ++i )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &mvm_session_list_pool[ i ] );
      ( void ) apr_list_add_tail(
                 &mvm_session_list_free_q,
                 ( ( apr_list_node_t* ) &mvm_session_list_pool[i] ) );
    }

    ( void ) apr_list_init_v2( &mvm_session_q, NULL, NULL );
  }

  { /* Create the high/meduim/low MVM tasks, sequentially. */
    rc = apr_event_create( &mvm_thread_event );
    MVM_PANIC_ON_ERROR( rc );
    {
      ( void ) apr_thread_create( &mvm_task_handle, MVM_MED_TASK_NAME,
                                  MVM_MED_TASK_PRIORITY, mvm_med_task_stack,
                                  MVM_MED_TASK_STACK_SIZE, mvm_task, NULL );
      ( void ) apr_event_wait( mvm_thread_event );
    }
    rc = apr_event_destroy( mvm_thread_event );
    MVM_PANIC_ON_ERROR( rc );
  }

  { /* Initialize the CVD shared memory utility library. */
    ( void ) cvd_mem_mapper_init( );
  }

  { /* Initialize the CVD calibration search utility library. */
    ( void ) cvd_cal_init( );
  }

  { /* Initialize the CVD devcfg parser. */
    ( void ) cvd_devcfg_parser_init( );
  }

  { /* Initialize the APR resource. */
    rc = __aprv2_cmd_register2(
           &mvm_apr_handle, mvm_my_dns, sizeof( mvm_my_dns ), 0,
           mvm_isr_dispatch_fn, NULL, &mvm_my_addr );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "mvm_init(): registered with APR rc=0x%08X", rc );
  }

  { /* Initialize the MVM-CCM */
    ( void ) mvm_ccm_init( mvm_apr_handle, mvm_my_addr );
  }

  /* MMPM client registration */
  (void)vccm_mmpm_register(VCCM_CLIENT_ID_MVM);

  return APR_EOK;
}

static int32_t mvm_post_init ( void )
{
  { /* Perform DNS look-ups now after services have registered in init. */
    ( void ) __aprv2_cmd_local_dns_lookup(
               mvm_cvs_dns, sizeof( mvm_cvs_dns ), &mvm_cvs_addr );

    ( void ) __aprv2_cmd_local_dns_lookup(
               mvm_vpm_dns, sizeof( mvm_vpm_dns ), &mvm_vpm_addr );

    ( void ) __aprv2_cmd_local_dns_lookup(
               mvm_vsm_dns, sizeof( mvm_vsm_dns ), &mvm_vsm_addr );
    ( void ) __aprv2_cmd_local_dns_lookup(
               mvm_cvd_vfr_dns, sizeof( mvm_cvd_vfr_dns ), &mvm_cvd_vfr_addr );
      /**<
        * Note that CVD VFR is a dynamic APR service whose address can only be
        * queried by client who resides on the same APR domain as CVD VFR
        * service. CVD VFR is a private module which can only be used internal
        * within CVD.
        */
  }

  return APR_EOK;
}

static int32_t mvm_pre_deinit ( void )
{
  return APR_EOK;
}

static int32_t mvm_deinit ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== mvm_deinit()======" );

  /* TODO: Ensure no race conditions on deregister. */

  { /* Deinit the MVM-CCM. */
    ( void ) mvm_ccm_deinit( );
  }

  /* Deregister from ADSP PM */
  (void)vccm_mmpm_deregister(VCCM_CLIENT_ID_MVM);

  { /* Release the APR resource. */
    ( void ) __aprv2_cmd_deregister( mvm_apr_handle );
  }

  { /* Deinit the CVD devcfg parser. */
    ( void ) cvd_devcfg_parser_deinit( );
  }

  { /* Deinit the CVD calibration search utility library. */
    ( void ) cvd_cal_deinit( );
  }

  { /* Deinit the CVD shared memory utility library. */
    ( void ) cvd_mem_mapper_deinit( );
  }

  { /* Destroy the low/medium/high MVM tasks, sequentially. */
    ( void ) apr_event_signal_abortall( mvm_task_event );

    while ( mvm_task_state != MVM_THREAD_STATE_ENUM_EXIT )
    {
      ( void ) apr_misc_sleep( 1000000 ); /* Sleep for 1ms. */
    }
    ( void ) apr_thread_destroy( mvm_task_handle );
  }

  { /* Release the session queue management. */
    ( void ) apr_list_destroy( &mvm_session_q );
    ( void ) apr_list_destroy( &mvm_session_list_free_q );
  }

  { /* Release the command queue management. */
    ( void ) apr_list_destroy( &mvm_task_incoming_cmd_q );
    ( void ) apr_list_destroy( &mvm_free_cmd_q );
  }

#ifndef WINSIM
  { /* Unregister for system monitor SSR callbacks. */
    ( void ) rcecb_unregister_name( SYS_M_SSR_MODEM_AFTER_POWERUP,
               mvm_ssr_modem_after_powerup_handler );

    ( void ) rcecb_unregister_name( SYS_M_SSR_MODEM_AFTER_SHUTDOWN,
               mvm_ssr_modem_after_shutdown_handler );
  }
#endif /* !WINSIM */

  { /* Release the SSR management. */
    ( void ) apr_list_destroy( &mvm_ssr_mem_handle_tracking.used_q );
    ( void ) apr_list_destroy( &mvm_ssr_mem_handle_tracking.free_q );

    ( void ) mvm_pending_control_destroy( &mvm_ssr_pending_ctrl );
  }

  { /* Release the session management. */
    ( void ) mvm_pending_control_destroy( &mvm_task_pending_ctrl );
  }

  { /* Release the object management. */
    ( void ) apr_objmgr_destruct( &mvm_objmgr );
  }

  { /* Release the locks. */
    ( void ) apr_lock_destroy( mvm_task_lock );
    ( void ) apr_lock_destroy( mvm_int_lock );
  }

  { /* Print out debug info. on object sizes. */
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "apr_objmgr_object_t size = %d", sizeof( apr_objmgr_object_t ) );
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "mvm_object_t size = %d", sizeof( mvm_object_t ) );
  }

  return APR_EOK;
}

APR_INTERNAL uint32_t mvm_obj_stat (
  uint32_t *objects_used
)
{
  if( NULL == objects_used )
  {
    return APR_EBADPARAM;
  }
  *objects_used = MVM_MAX_OBJECTS_V - mvm_objmgr.free_q.size;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW, "mvm_obj_stat(): Objects used=(%d)",
                                         *objects_used );
  return APR_EOK;
}


APR_EXTERNAL int32_t mvm_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  int32_t rc = APR_EOK;
  vss_imvm_cmd_cal_query_t* cal_query_params = NULL;

  switch ( cmd_id )
  {
   case DRV_CMDID_INIT:
     rc = mvm_init( );
     break;
 
   case DRV_CMDID_POSTINIT:
     rc = mvm_post_init( );
     break;
 
   case DRV_CMDID_PREDEINIT:
     rc = mvm_pre_deinit( );
     break;
 
   case DRV_CMDID_DEINIT:
     rc = mvm_deinit( );
     break;
 
   case DRV_CMDID_RUN:
     rc = mvm_run( );
     break;

   case MVM_CMDID_CAL_QUERY:
   {
     cal_query_params = ( ( vss_imvm_cmd_cal_query_t* ) params );

     if ( NULL != cal_query_params )
     {
       rc = cvd_cal_query( cal_query_params->query_handle, cal_query_params->cb_fn,
                           ( ( void* ) cal_query_params->client_data ) );
     }

     break;
   }

   case VSS_IMVM_CSM_CMD_REQUEST_VOTE:
   {
     vss_imvm_csm_cmd_request_vote_t* req_vote = ( ( vss_imvm_csm_cmd_request_vote_t* ) params );
     
     if ( NULL != req_vote )
     {
       rc = mvm_csm_cmd_request_vote( req_vote );
     }
   }
   break;

   case MVM_CMDID_OBJ_STAT:
     rc = mvm_obj_stat( params );
     break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "MVM: CMDID=(0X%08X) UNSUPPORTED", cmd_id );
     rc = APR_EUNSUPPORTED;
     break;
  }

  return rc;
}
