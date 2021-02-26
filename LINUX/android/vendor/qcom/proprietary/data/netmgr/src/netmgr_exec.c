/******************************************************************************

                          N E T M G R _ E X E C . C

******************************************************************************/

/******************************************************************************

  @file    netmgr_exec.c
  @brief    Network Manager executive

  DESCRIPTION
  Implementation of NetMgr executive control module.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
02/17/10   ar         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <pthread.h>
#include <stdlib.h>
#include <assert.h>

#include "stm2.h"
#include "ds_list.h"
#include "ds_cmdq.h"

#include "netmgr_defs.h"
#include "netmgr_platform.h"
#include "netmgr_sm.h"
#include "netmgr_kif.h"
#include "netmgr_qmi.h"
#include "netmgr_util.h"
#include "netmgr_exec.h"
#include "netmgr_cb.h"
#include "netmgr_qmi_cb.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/


/*---------------------------------------------------------------------------
   Constant representing maximum number of command buffers used by this
   module
---------------------------------------------------------------------------*/
#define NETMGR_EXEC_MAX_CMDS 100

/*---------------------------------------------------------------------------
  Asynchronous event names for debug use.  This array must macth order of
  netmgr_sm_events_t in netmgr_sm.h
---------------------------------------------------------------------------*/

#define NETMGR_EVENT_NAME_SIZ 28
#define NETMGR_EVENT_NAME_CNT 34
LOCAL const char netmgr_exec_cmd_names[NETMGR_EVENT_NAME_CNT][NETMGR_EVENT_NAME_SIZ] = {
  "NETMGR_IS_COMPLETE_MSG",
  "NETMGR_KIF_MSG_CMD",
  "NETMGR_QMI_MSG_CMD",
  "NETMGR_RESET_MSG_CMD",
  "NETMGR_USER_MSG_CMD",
  "NETMGR_INITED_EV",
  "NETMGR_MODEM_IS_EV",
  "NETMGR_MODEM_OOS_EV",
  "NETMGR_WDS_CONNECTED_EV",
  "NETMGR_WDS_DISCONNECTED_EV",
  "NETMGR_KIF_OPENED_EV",
  "NETMGR_KIF_CLOSED_EV",
  "NETMGR_KIF_CONFIGURED_EV",
  "NETMGR_KIF_RECONFIGURED_EV",
  "NETMGR_QOS_ACTIVATE_EV",
  "NETMGR_QOS_MODIFY_EV",
  "NETMGR_QOS_DELETE_EV",
  "NETMGR_QOS_SUSPEND_EV",
  "NETMGR_QOS_FLOCNTRL_EV",
  "NETMGR_NETD_RESTART_EV",
  "NETMGR_NETD_LINK_RESTART_EV",
  "NETMGR_WLAN_OPENED_EV",
  "NETMGR_WLAN_CLOSED_EV",
  "NETMGR_OOS_COMPLETE_MSG"
};

/*---------------------------------------------------------------------------
   Executive control state information
---------------------------------------------------------------------------*/
LOCAL struct netmgr_exec_state_s  netmgr_exec_state_info;

/*===========================================================================
                            LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  netmgr_exec_cmd_free
===========================================================================*/
/*!
@brief
 Virtual function registered with the Command Thread to free a
 command buffer, after execution of the command is complete.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_exec_cmd_free( ds_cmd_t * cmd, void * data )
{
  netmgr_exec_cmd_t * cmd_buf;

  NETMGR_LOG_FUNC_ENTRY;

  /* Get qmi cmd ptr from user data ptr */
  cmd_buf = (netmgr_exec_cmd_t *)data;

  /* Double check for debug purposes that the command is legit */
  NETMGR_ASSERT( &cmd_buf->cmd == cmd );

  /* Unset tracker for debug purposes */
  cmd_buf->tracker = 0;

  /* Release dynamic memory */
  netmgr_free( cmd_buf );

  NETMGR_LOG_FUNC_EXIT;

  return;
}

/*===========================================================================
  FUNCTION  netmgr_exec_cmd_process
===========================================================================*/
/*!
@brief
 Virtual function registered with the Command Thread to process a
 command buffer.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_exec_cmd_process (ds_cmd_t * cmd, void * data)
{
  netmgr_exec_cmd_t * cmd_buf;
  stm_status_t        sm_result;

  NETMGR_LOG_FUNC_ENTRY;

  /* Get qmi cmd ptr from user data ptr */
  cmd_buf = (netmgr_exec_cmd_t *)data;

  /* Double check for debug purposes that the command is legit */
  NETMGR_ASSERT(&cmd_buf->cmd == cmd);

  netmgr_log_med("Process command: ID=%s, link=%d, state=%d\n",
                 netmgr_exec_cmd_names[cmd_buf->data.type],
                 cmd_buf->data.link,
                 (int)stm_get_state(&NETMGR_SM[cmd_buf->data.link]));

  /* Process based on command type */
  sm_result = stm_instance_process_input( NULL,
                                          NETMGR_SM,
                                          (uint32)cmd_buf->data.link,
                                          (stm_input_t)cmd_buf->data.type,
                                          (void*)cmd_buf );
  if( STM_SUCCESS !=  sm_result )
  {
    if( STM_ENOTPROCESSED == sm_result )
    {
      netmgr_log_err("Command not processed %d. Current state %d\n",
                     cmd_buf->data.type, (int) stm_get_state(&NETMGR_SM[cmd_buf->data.link]));

    }
    else
    {
      netmgr_log_err("Command processing error %d\n", sm_result);
    }

    /* Cleanup dynamic memory for error cases */
    switch( cmd_buf->data.type )
    {
      case NETMGR_WDS_CONNECTED_EV:
        if( cmd_buf->data.info.connect_msg.addr_info_ptr )
        {
          ds_free( cmd_buf->data.info.connect_msg.addr_info_ptr );
        }
        break;

      default:
        /* Do nothing */
        break;
    }
  }

  NETMGR_LOG_FUNC_EXIT;

  return;
}


/*===========================================================================
  FUNCTION  netmgr_exec_cleanup
===========================================================================*/
/*!
@brief
  Performs cleanup of executive module.  Invoked at process termination.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL void
netmgr_exec_cleanup
(
  void
)
{
  NETMGR_LOG_FUNC_ENTRY;

  /* Purge command queue to release heap memory */
  (void)ds_cmdq_deinit( &netmgr_exec_state_info.cmdq );

  NETMGR_LOG_FUNC_EXIT;
}

/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  netmgr_exec_get_cmd
===========================================================================*/
/*!
@brief
  Function to get a command buffer for asynchronous processing

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Initializes the QMI Driver
*/
/*=========================================================================*/
netmgr_exec_cmd_t * netmgr_exec_get_cmd ( void )
{
  netmgr_exec_cmd_t * cmd_buf = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  /* Allocate command buffer */
  if((cmd_buf = netmgr_malloc(sizeof(netmgr_exec_cmd_t))) == NULL ) {
    NETMGR_ABORT("%s ","netmgr_exec_cmd_alloc: netmgr_malloc failed\n");
    return NULL;
  }
  memset(cmd_buf, 0x0, sizeof(netmgr_exec_cmd_t));

  /* Assign self-reference in DS cmd payload */
  cmd_buf->cmd.data      = (void*)cmd_buf;
  cmd_buf->tracker       = 1;
  cmd_buf->repost_count  = 0;

  /* Asssign default execution and free handlers */
  cmd_buf->cmd.execute_f = netmgr_exec_cmd_process;
  cmd_buf->cmd.free_f    = netmgr_exec_cmd_free;

  NETMGR_LOG_FUNC_EXIT;

  return cmd_buf;
}

/*===========================================================================
  FUNCTION  netmgr_exec_release_cmd
===========================================================================*/
/*!
@brief
  Function to release a command buffer

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Returns memory to heap
*/
/*=========================================================================*/
void netmgr_exec_release_cmd ( netmgr_exec_cmd_t * cmd_buf )
{
  NETMGR_ASSERT( cmd_buf );

  NETMGR_LOG_FUNC_ENTRY;

  if( cmd_buf->cmd.free_f ) {
    cmd_buf->cmd.free_f( &cmd_buf->cmd, cmd_buf->cmd.data );
  } else {
    netmgr_log_err("Specified buffer not valid, ignoring\n");
  }

  NETMGR_LOG_FUNC_EXIT;

  return;
}

/*===========================================================================
  FUNCTION  netmgr_exec_put_cmd
===========================================================================*/
/*!
@brief
  Function to post a command buffer for asynchronous processing

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - Initializes the QMI Driver
*/
/*=========================================================================*/
int netmgr_exec_put_cmd ( const netmgr_exec_cmd_t * cmdbuf )
{
  int result = NETMGR_SUCCESS;

  NETMGR_ASSERT( cmdbuf );

  NETMGR_LOG_FUNC_ENTRY;
  /* Check to verify that command obtained is a valid command*/
  if (cmdbuf->data.type < NETMGR_INVALID_EV)
  {
    netmgr_log_med("Received command: ID=%s, link=%d\n",
                   netmgr_exec_cmd_names[cmdbuf->data.type],
                   cmdbuf->data.link);

  /* Append command buffer to the command queue */
  result = ds_cmdq_enq( &netmgr_exec_state_info.cmdq, &cmdbuf->cmd );
  }
  else
  {
    netmgr_log_err("Invalid command type received.");
    result = NETMGR_FAILURE;
    /* Do we need to assert here*/
  }
  NETMGR_LOG_FUNC_EXIT;

  return result;
}

/*===========================================================================
  FUNCTION  netmgr_exec_wait
===========================================================================*/
/*!
@brief
  Forces calling thread to wait on exit of command processing thread.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Calling thread is blocked indefinitely
*/
/*=========================================================================*/
void netmgr_exec_wait ( void )
{
  NETMGR_LOG_FUNC_ENTRY;
  ds_cmdq_join_thread( &netmgr_exec_state_info.cmdq );
  NETMGR_LOG_FUNC_EXIT;
}


/*===========================================================================
  FUNCTION  netmgr_exec_init
===========================================================================*/
/*!
@brief
 Main initialization routine of the executive control module.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Initializes the QMI Driver
*/
/*=========================================================================*/
void
netmgr_exec_init (int nlink, netmgr_ctl_port_config_type links[])
{
  unsigned int i=0;
  unsigned int count = (unsigned int)nlink;

  NETMGR_LOG_FUNC_ENTRY;

  /* Register process termination cleanup handler */
  atexit( netmgr_exec_cleanup );

  /*-------------------------------------------------------------------------
    Initialize the state machine instances.  There is one per network link.
  -------------------------------------------------------------------------*/
  for( i=0; i < NETMGR_MAX_LINK; i++ )
  {
    /* Skip already disabled links */
    if( !links[i].enabled )
      continue;

    /* Initialize only up to number of links requested */
    if( count ) {

      if( STM_SUCCESS !=
          stm_instance_activate( NETMGR_SM, i, (void*)(uintptr_t)i ) )
      {
        netmgr_log_err("Failed to initialize state machine: %d", i);
      }

      netmgr_exec_state_info.links[i].sm = &NETMGR_SM[i];
      count--;
    } else {
      links[i].enabled = FALSE;
    }
  } /* for (DSUMTSPS_MAX_PS_IFACES */

  netmgr_exec_state_info.nlink = nlink;

  /*-------------------------------------------------------------------------
    Initialize command queue for asynch processing
  -------------------------------------------------------------------------*/
  ds_cmdq_init( &netmgr_exec_state_info.cmdq, NETMGR_EXEC_MAX_CMDS );

  NETMGR_LOG_FUNC_EXIT;

  return;
}

/*===========================================================================
  FUNCTION  netmgr_exec_process_is_complete_msg
===========================================================================*/
/*!
@brief
  Process IS complete event when there is at least one link that is in the
  INITED state.

@return
  None
*/
/*=========================================================================*/
void
netmgr_exec_process_is_complete_msg ()
{
  int i = 0;

  NETMGR_LOG_FUNC_ENTRY

  for(i=0; i < netmgr_main_cfg.nint; ++i)
  {
    if( NETMGR_STATE_DOWN != (int)stm_get_state(&NETMGR_SM[i]) )
    {
      break;
    }
  }

  if ( NETMGR_MODEM_OOS_STATE == NETMGR_GET_MODEM_SSR_STATE())
  {
    netmgr_log_err("%s: Modem is in OOS state,do not process ready reqs yet",
                   __func__);
  }
  else if ( i == netmgr_main_cfg.nint )
  {
    netmgr_log_err("%s: No links are in inited state",__func__);
  }
  /* Start responding to ready requests from clients.*/
  NETMGR_INCR_PROCESS_READY_REQ();

  if( NETMGR_GET_PROCESS_READY_REQ() > 0){
    netmgr_cb_runall(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_MODEM_IS_COMPLETE, NULL);
  }

  /* By this time netmgr state machine has been initialized and we would have captured all the
     required bootup logs */
  log_to_file = FALSE;

  NETMGR_LOG_FUNC_EXIT

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
  netmgr_util_wakelock_release();
#endif
}

/*===========================================================================
  FUNCTION  netmgr_exec_process_oos_complete_msg
===========================================================================*/
/*!
@brief
  Process IS complete event when there is at least one link that is in the
  INITED state.

@return
  None
*/
/*=========================================================================*/
void
netmgr_exec_process_oos_complete_msg()
{
  netmgr_log_med("%s(): ENTRY", __func__);

  netmgr_cb_runall(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_MODEM_OOS_COMPLETE, NULL);

  netmgr_log_med("%s(): EXIT", __func__);

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
  netmgr_util_wakelock_release();
#endif
}

/*===========================================================================
  FUNCTION  netmgr_exec_process_msg
===========================================================================*/
/*!
@brief
  Process messages that need to be run in the executive thread context. These
  messages cannot be processed as part of the state machine.

@return
  None
*/
/*=========================================================================*/
void
netmgr_exec_process_msg (ds_cmd_t * cmd, void * data)
{
  netmgr_exec_cmd_t *exec_cmd = NULL;
  NETMGR_LOG_FUNC_ENTRY

  NETMGR_ASSERT(cmd);
  exec_cmd = (netmgr_exec_cmd_t*)data;

  netmgr_log_med("Process msg: ID=%s, link=%d, state=%d\n",
                 netmgr_exec_cmd_names[exec_cmd->data.type],
                 exec_cmd->data.link,
                 (int)stm_get_state(&NETMGR_SM[exec_cmd->data.link]));

  switch (exec_cmd->data.type)
  {
    case NETMGR_IS_COMPLETE_MSG:
      netmgr_exec_process_is_complete_msg();
      break;
    case NETMGR_OOS_COMPLETE_MSG:
      netmgr_exec_process_oos_complete_msg();
      break;
    default:
      netmgr_log_med("%s: unsupported cmd [%d], ignoring",
                     __func__, exec_cmd->data.type);
      break;
  }
  NETMGR_LOG_FUNC_EXIT
}


