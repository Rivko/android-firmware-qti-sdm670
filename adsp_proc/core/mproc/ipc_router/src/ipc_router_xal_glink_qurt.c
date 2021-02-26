/*===========================================================================

               I P C    R O U T E R   X A L  G L I N K  Q U R T

        This file provides QuRT OS specific functionality of the glink
                          XAL for the IPC Router

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/ipc_router/src/ipc_router_xal_glink_qurt.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/14   aep     Initial creation
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/

#include "ipc_router_types.h"
#include "ipc_router_xal_glink_common.h"
#include "timer.h"
#include "qurt.h"

/*===========================================================================
                  CONSTANT / MACRO DECLARATIONS
===========================================================================*/

#define NAME_LEN 12

/*===========================================================================
                        PRIVATE TYPE DEFINITIONS 
===========================================================================*/

/** The OS specific private structure of the glink port */
struct ipc_router_xal_glink_os_priv_s {
  /* Has the worker started? */
  boolean                 worker_started;
  /* holder to handle signals before the worker has started */
  unsigned int            pre_sigs;
  /* Thread ID of the worker thread */
  qurt_thread_t           tid;
  /* Attributes of the worker thread */
  qurt_thread_attr_t      thread_attr;
  /* Signalling object which the worker thread will wait on */
  qurt_anysignal_t        signal;
  /* Stack of the worker thread */
  void                    *thread_stack;
  /* Name of the worker thread */
  char                    name[NAME_LEN];
};

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_xal_glink_worker_thread

DESCRIPTION   The Worker thread dedicated to handle RX and TX events
              on this GLINK port.

              priv - Private parameter (port)

RETURN VALUE  None

SIDE EFFECTS  This function never returns.
===========================================================================*/
static void ipc_router_xal_glink_worker_thread(void *priv)
{
  ipc_router_xal_glink_port_type *port = (ipc_router_xal_glink_port_type *)priv;
  unsigned int got_sigs;
  unsigned int wait_sigs = EVENT_MASK(IPC_ROUTER_XAL_GLINK_EVENT_LAST) - 1;

  /* Handle events which may have arrived before the worker was
   * started */
  ipc_router_os_sem_lock(&port->lock);
  port->os_priv->worker_started = TRUE;
  got_sigs = port->os_priv->pre_sigs;
  ipc_router_os_sem_unlock(&port->lock);

  while(1)
  {
    ipc_router_xal_glink_handle_events(port, got_sigs);

    if(got_sigs & EVENT_MASK(IPC_ROUTER_XAL_GLINK_EVENT_CLOSED))
    {
      port->os_priv->worker_started = FALSE;
      break;
    }

    qurt_anysignal_wait(&port->os_priv->signal, wait_sigs);
    got_sigs = qurt_anysignal_get(&port->os_priv->signal);
    qurt_anysignal_clear(&port->os_priv->signal, got_sigs);
  }
  qurt_thread_exit(0);
}

/*===========================================================================
                        EXPORTED FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_xal_glink_os_init

DESCRIPTION   Initializes the OS Private structure

              port - The port structure for this GLINK interface.

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success,
              error code on failure.
===========================================================================*/
int ipc_router_xal_glink_os_init
(
  ipc_router_xal_glink_port_type *port
)
{
  ipc_router_xal_glink_os_priv_type *os_priv;

  os_priv = ipc_router_os_calloc(1, sizeof(*os_priv));
  if(!os_priv)
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }

  /* Do not set signals on worker when it has not been started */
  os_priv->worker_started = FALSE;
  port->os_priv = os_priv;

  /* 0 size stack implies that we will be operating in a threadless fashion
   * Hijacking the callback functions of GLink. Else we need to create
   * the stack for the worker thread */
  if(port->params.stack_size > 0)
  {
    os_priv->thread_stack = ipc_router_os_malloc(port->params.stack_size);
    if(!os_priv->thread_stack)
    {
      ipc_router_os_free(os_priv);
      port->os_priv = NULL;
      return IPC_ROUTER_STATUS_NO_MEM;
    }
    strlcpy(os_priv->name, "ipcr_", sizeof(os_priv->name));
    strlcat(os_priv->name, port->params.remote_ss, sizeof(os_priv->name));
    os_priv->name[NAME_LEN - 1] = '\0';

    qurt_anysignal_init(&os_priv->signal);

    qurt_thread_attr_init(&os_priv->thread_attr);
    qurt_thread_attr_set_name(&os_priv->thread_attr, os_priv->name);
    qurt_thread_attr_set_priority(&os_priv->thread_attr, port->params.priority);
    qurt_thread_attr_set_stack_size(&os_priv->thread_attr, port->params.stack_size);
    qurt_thread_attr_set_stack_addr(&os_priv->thread_attr, os_priv->thread_stack);

    qurt_thread_create(&port->os_priv->tid,
                       &port->os_priv->thread_attr,
                       ipc_router_xal_glink_worker_thread,
                       (void *)port);
  }
  else
  {
    port->os_priv->worker_started = TRUE;
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}


/*===========================================================================
FUNCTION      ipc_router_xal_glink_os_deinit

DESCRIPTION   De-initialize the OS specific elements associated with this port.

              port - The port structure for this GLINK interface.

RETURN VALUE  None

SIDE EFFECTS  Blocks till the worker thread has exited
===========================================================================*/
void ipc_router_xal_glink_os_deinit
(
  ipc_router_xal_glink_port_type *port
)
{
  if(!port || !port->os_priv)
  {
    return;
  }
  /* Assumes that thread_join works like pthread_join() and blocks till
   * the thread has exited. */
  if(port->params.stack_size > 0)
  {
    int status;
    qurt_thread_join(port->os_priv->tid, &status);
    ipc_router_os_free((void *)port->os_priv->thread_stack);
    qurt_anysignal_destroy(&port->os_priv->signal);
  }
  else
  {
    /* Wait till GLink has set a LOCAL_DISCONNECTED which in turn
     * sets a CLOSED event which is handled to set worker_started 
     * to be FALSE */
    while(port->os_priv->worker_started == TRUE)
    {
      timer_sleep(50, T_MSEC, TRUE);
    }
  }
  ipc_router_os_free(port->os_priv);
  port->os_priv = NULL;
}

/*===========================================================================
FUNCTION      ipc_router_xal_glink_set_event

DESCRIPTION   Signal the worker to handle an event

              port - The port structure for this GLINK interface.
              event - The event to be handled

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_xal_glink_set_event
(
  ipc_router_xal_glink_port_type *port, 
  ipc_router_xal_glink_event_type event
)
{
  /* If the worker has not started, just set the pre_mask
   * and return */
  ipc_router_os_sem_lock(&port->lock);
  if(port->os_priv->worker_started == FALSE)
  {
    port->os_priv->pre_sigs |= EVENT_MASK(event);
    ipc_router_os_sem_unlock(&port->lock);
    return;
  }
  ipc_router_os_sem_unlock(&port->lock);

  if(port->params.stack_size > 0)
  {
    qurt_anysignal_set(&port->os_priv->signal, EVENT_MASK(event));
  }
  else
  {
    ipc_router_xal_glink_handle_events(port, EVENT_MASK(event));
    if(event == IPC_ROUTER_XAL_GLINK_EVENT_CLOSED)
    {
      port->os_priv->worker_started = FALSE;
    }
  }
}

