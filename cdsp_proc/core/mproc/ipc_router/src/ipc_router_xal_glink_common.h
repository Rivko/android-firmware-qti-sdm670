#ifndef _IPC_ROUTER_XAL_GLINK_COMMON_H_
#define _IPC_ROUTER_XAL_GLINK_COMMON_H_
/*===========================================================================

                      I P C    R O U T E R    X A L
                         G L I N K  C O M M O N

  ---------------------------------------------------------------------------
  Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/ipc_router/src/ipc_router_xal_glink_common.h#1 $ 
$DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/17   jlg     ipc_router_xal_glink_param_type moved to ipc_router_settings.h
11/06/14   aep     Initial creation
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_xal_glink.h"
#include "ipc_router_os.h"
#include "ipc_router_internal.h"
#include "ipc_router_packet.h"
#include "ipc_router_core.h"
#include "ipc_router_settings.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/

/** Max outstanding intent requests */
#define IPC_ROUTER_XAL_GLINK_MAX_REQ_INTENTS 8

/* converts an event to a mask value which can be OR'd with 
 * other events to create a set */
#define EVENT_MASK(event) (1UL << (event))

/* Checks if an event is set in an event_set  */
#define IS_EVENT_SET(event_set, event) ((event_set) & EVENT_MASK(event))


/*===========================================================================
                 PRE-DATATYPE DECLARATIONS
===========================================================================*/
struct ipc_router_xal_glink_os_priv_s;
typedef struct ipc_router_xal_glink_os_priv_s ipc_router_xal_glink_os_priv_type;

struct ipc_router_xal_glink_port_s;
typedef struct ipc_router_xal_glink_port_s ipc_router_xal_glink_port_type;

struct ipc_router_xal_glink_intent_s;
typedef struct ipc_router_xal_glink_intent_s ipc_router_xal_glink_intent_type;

/*===========================================================================
                        DATA TYPE DEFINITIONS
===========================================================================*/

/** Structure used to store the intent provided to glink */
struct ipc_router_xal_glink_intent_s
{
  LINK(ipc_router_xal_glink_intent_type, link);
  size_t                                 size;
  ipc_router_packet_type                 *pkt;
  ipc_router_xal_glink_port_type         *port;
  boolean                                 is_static;
};

/** Port structure definitions */
struct ipc_router_xal_glink_port_s
{
  /* Handle to ipc router core */
  void                              *cb_handle;

  /* Handle to the channel */
  glink_handle_type                 handle;
  /* Handle to the link state notifier */
  glink_link_handle_type            link_state_handle;

  /* Indicates whether we are currently closing the port */
  boolean                           closing;

  /* The current channel state */
  glink_channel_event_type          ch_state;
  /* The current link state */
  glink_link_state_type             link_state;

  /* The parameters provided to this port */
  ipc_router_xal_glink_param_type   params;

  /* Queue of incoming packets */
  ipc_router_packet_queue_type      rx_queue;

  /* Queue of outgoing packets */
  ipc_router_packet_queue_type      tx_queue;
  
  /* Queue of packets to be cleaned */
  ipc_router_packet_queue_type      tx_done_queue;

  /* Lock to serialize access to this structure */
  ipc_router_os_sem                 lock;

  /* Requested intent sizes */
  uint32 requested_intents[IPC_ROUTER_XAL_GLINK_MAX_REQ_INTENTS];

  /* Pending TX Packet */
  ipc_router_packet_type            *tx_leftover;    
  /* OS Private values (Defined in the OS specific code) */
  ipc_router_xal_glink_os_priv_type *os_priv;
};

/** Events supported */
typedef enum {
  /* One or more packets pending in the rx_queue */
  IPC_ROUTER_XAL_GLINK_EVENT_RX = 0,
  /* One or more intents requested by the remote router */
  IPC_ROUTER_XAL_GLINK_EVENT_RX_REQ,
    /* One or more packets pending in the tx_queue */
  IPC_ROUTER_XAL_GLINK_EVENT_TX,
  /* One or more packets are waiting to be freed */
  IPC_ROUTER_XAL_GLINK_EVENT_TX_DONE,
  /* Link is up */
  IPC_ROUTER_XAL_GLINK_EVENT_UP,
  /* Link is down */
  IPC_ROUTER_XAL_GLINK_EVENT_DOWN,
  /* Channel is now connected */
  IPC_ROUTER_XAL_GLINK_EVENT_CONNECTED,
  /* The remote side has disconnected the channel */
  IPC_ROUTER_XAL_GLINK_EVENT_RDISCONNECTED,
  /* Cleanup after glink_close() done, pending cleanup */
  IPC_ROUTER_XAL_GLINK_EVENT_LDISCONNECTED,
  /* Complete closing the router, exit worker thread if present */
  IPC_ROUTER_XAL_GLINK_EVENT_CLOSED,
  /* None */
  IPC_ROUTER_XAL_GLINK_EVENT_LAST
} ipc_router_xal_glink_event_type;

/*===========================================================================
          FUNCTIONS [OS SPECIFIC -> OS COMMON]
===========================================================================*/

/*===========================================================================
FUNCTION ipc_router_xal_glink_handle_events

DESCRIPTION
   Handle all events specified in the mask 'got_events'

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_glink_handle_events
(
  ipc_router_xal_glink_port_type *port, 
  unsigned int got_events
);

/*===========================================================================
          FUNCTIONS [OS COMMON -> OS SPECIFIC]
===========================================================================*/

/*===========================================================================
FUNCTION ipc_router_xal_glink_handle_events

DESCRIPTION
   Sets an event to be handled by the worker thread.

RETURN VALUE
   None

SIDE EFFECTS
  If the port is operating in a 'threadless' configuration, then
  this function will also handle the event.
===========================================================================*/
void ipc_router_xal_glink_set_event
(
  ipc_router_xal_glink_port_type *port, 
  ipc_router_xal_glink_event_type event
);

/*===========================================================================
FUNCTION ipc_router_xal_glink_os_init

DESCRIPTION
   Initialize OS specific data structures.

RETURN VALUE
   IPC_ROUTER_STATUS_SUCCESS on success, error code otherwise.

SIDE EFFECTS
===========================================================================*/
int ipc_router_xal_glink_os_init
(
 ipc_router_xal_glink_port_type *port
);

/*===========================================================================
FUNCTION ipc_router_xal_glink_os_deinit

DESCRIPTION
   Tear down any threads created as part of ipc_router_xal_glink_start_worker
   and cleanup anything created in ipc_router_xal_glink_os_init.

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_glink_os_deinit
(
 ipc_router_xal_glink_port_type *port
);

#endif
