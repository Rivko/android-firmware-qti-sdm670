#ifndef IPC_ROUTER_XAL_H
#define IPC_ROUTER_XAL_H
/*===========================================================================
     I P C   R O U T E R   X P O R T   A B S T R A C T I O N   L A Y E R

DESCRIPTION
    This file specifies the interface of the transport abstraction layer
    used by the IPC Router core.

  ---------------------------------------------------------------------------
  Copyright (c) 2007-2011, 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/ipc_router/src/ipc_router_xal.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/07    sa     First revision
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_packet.h"

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
typedef struct
{
  int (*xport_open) (void *xport_params, void *cb_handle, void **out_handle);
  int (*xport_close)(void *handle);
  int (*xport_write)(void *handle, ipc_router_packet_type *msg);
}ipc_router_xal_ops_type;

/* Helper macros to set the correct flags for ipc_router_xal_start_xport */
#define IPC_ROUTER_XAL_FLAG_NET_ID(net)   ((net) & 0xff)
#define IPC_ROUTER_XAL_FLAG_PRIO(prio)    (((prio) << 8) & 0xf00)
#define IPC_ROUTER_XAL_FLAG_VERSION(vers) (((vers) << 12) & 0xf000)
#define IPC_ROUTER_XAL_FLAG_TAG(tag)     (((tag) << 16) & 0xffff0000)

/*===========================================================================
                        EXPORTED XAL FUNCTIONS
===========================================================================*/
/* Up-calls into the router for receive and error on transport */
/*===========================================================================
FUNCTION      ipc_router_xal_recv

DESCRIPTION   function for transports to call when there's a packet to be 
              received.
              - this function can only be called by a single thread
                from any given transport

ARGUMENTS     
              cb_handle (in) - identifier for the transport
===========================================================================*/
void ipc_router_xal_recv
(
 ipc_router_packet_type *msg,
 void *cb_handle
 );

/*===========================================================================
FUNCTION      ipc_router_xal_down

DESCRIPTION   A callback function for transports to call in the case the link
              goes down

ARGUMENTS     
              cb_handle (in) - identifier for the transport
===========================================================================*/
void ipc_router_xal_down
(
 void *cb_handle
 );

/*===========================================================================
FUNCTION      ipc_router_xal_up

DESCRIPTION   A callback function for transports to call when a link comes 
              back up.

ARGUMENTS     
              cb_handle (in) - identifier for the transport
===========================================================================*/
void ipc_router_xal_up
(
  void *cb_handle 
);

/*===========================================================================
FUNCTION      ipc_router_xal_closed

DESCRIPTION   Signaling the transport has fully closed.

ARGUMENTS     cb_handle - handle to xport
===========================================================================*/
void ipc_router_xal_closed
(
 void *cb_handle
 );

/* Config functions */
/*===========================================================================
FUNCTION      ipc_router_xal_start_xport

DESCRIPTION   Initializes and opens a transport.  

ARGUMENTS     ops - table of xport operations
              xport_params - parameters to the xport
              flags - xport flags containing network ID and bundle ID
              desc - description of the network interface

RETURN VALUE  Handle to xport on success

SIDE EFFECTS  None
===========================================================================*/
void *ipc_router_xal_start_xport
(
 ipc_router_xal_ops_type *ops,
 void *xport_params,
 uint32 flags,
 const char *desc
 );

/*===========================================================================
FUNCTION      ipc_router_xal_stop_xport

DESCRIPTION   Stops and deinitializes a transport.  

ARGUMENTS     handle - handle to xport

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_xal_stop_xport
(
 void *handle
 );

#endif  /* IPC_ROUTER_XAL_H */
