#ifndef IPC_ROUTER_CORE_H
#define IPC_ROUTER_CORE_H
/*===========================================================================

                      I P C    R O U T E R    C O R E
                          H E A D E R    F I L E

   This file describes the OS-independent interface to the IPC Router core.
   This file must be compatible with all operating systems on which the 
   router will be used.

  ---------------------------------------------------------------------------
  Copyright (c) 2007-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/ipc_router/inc/ipc_router_core.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/07    sa     First revision
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_types.h"
#include "ipc_router_packet.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/
/* Init options set */
#define IPC_ROUTER_CORE_PORT_OPTIONS_INIT(_options) (_options).options_set = 0

/* Set priority */
#define IPC_ROUTER_CORE_PORT_OPTIONS_SET_PRIORITY(_options, _priority) do { \
  (_options).options_set |= IPC_ROUTER_CORE_PORT_OPTIONS_PRIORITY_VALID; \
  (_options).priority = _priority;  \
} while(0)

/* Set scope */
#define IPC_ROUTER_CORE_PORT_OPTIONS_SET_SCOPE(_options, _scope) do { \
  (_options).options_set |= IPC_ROUTER_CORE_PORT_OPTIONS_SCOPE_VALID; \
  (_options).scope = _scope;  \
} while(0)

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
struct ipc_router_client_s;
typedef struct ipc_router_client_s ipc_router_client_type;

/*===========================================================================
FUNCTION      ipc_router_core_open

DESCRIPTION   Opens a handle to the router. This handle is to be use in all 
              subsequent operations on the IPC router for this port.

ARGUMENTS     handle (out)      - a handle returned to identify this client in 
                               later operations.

              port_id (in)      - a locally unique identifier for the end-point.
                                  If 0, a port is dynamically assigned 
              rx_cb (in)        - rx callback
              rx_cb_data (in)   - rx callback data
              event_cb (in)     - event callback
              event_cb_data (in) - event callback data

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_open
(
 ipc_router_client_type **handle,
 uint32 port_id,
 ipc_router_rx_cb_type rx_cb,
 void *rx_cb_data,
 ipc_router_event_cb_type event_cb,
 void *event_cb_data
);

/*===========================================================================
FUNCTION      ipc_router_core_open_with_options

DESCRIPTION   Opens a handle to the router. This handle is to be use in all 
              subsequent operations on the IPC router for this port.

ARGUMENTS     handle (out)      - a handle returned to identify this client in 
                               later operations.

              port_id (in)      - a locally unique identifier for the end-point.
                                  If 0, a port is dynamically assigned 
              rx_cb (in)        - rx callback
              rx_cb_data (in)   - rx callback data
              event_cb (in)     - event callback
              event_cb_data (in) - event callback data
              options (in)      - Options for this port
                                  (refer type description)
                                  NULL will force ipc_router to assume 
                                  default options.

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_open_with_options
(
 ipc_router_client_type **handle,
 uint32 port_id,
 ipc_router_rx_cb_type rx_cb,
 void *rx_cb_data,
 ipc_router_event_cb_type event_cb,
 void *event_cb_data,
 ipc_router_core_options_type *options
);

/*===========================================================================
FUNCTION      ipc_router_core_close

DESCRIPTION   Closes an opened handle to the router.  

ARGUMENTS     client (in) - client handle returned from ipc_router_open

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  When close get's called, it's assumed that no client threads
              will be running in the router core
===========================================================================*/
int ipc_router_core_close
(
 ipc_router_client_type *client
);

/*===========================================================================
FUNCTION      ipc_router_core_send

DESCRIPTION   Send message from client to the destination address

ARGUMENTS     client    - client handle returned from ipc_router_open
              dest      - destination address      
              buf       - pointer to message
              len       - length of message

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success
              IPC_ROUTER_STATUS_BUSY if Tx quota exceeded. Wait for event cb.
              Other failures

SIDE EFFECTS  
===========================================================================*/
int ipc_router_core_send
(
 ipc_router_client_type *client,
 ipc_router_address_type *dest,
 const unsigned char *buf,
 uint32 len
);

/*===========================================================================
FUNCTION      ipc_router_core_get_packet

DESCRIPTION   Get packet for transmission. 

ARGUMENTS     

RETURN VALUE  Pointer to packet or NULL on failure

SIDE EFFECTS  packet is allocated with proper headroom reserved
===========================================================================*/
ipc_router_packet_type *ipc_router_core_get_packet(void);

/*===========================================================================
FUNCTION      ipc_router_core_send_packet

DESCRIPTION   Send packet to destination

ARGUMENTS     client (in)       - client handle returned from ipc_router_open
              dest (in)         - pointer to destination address
              msg (in)          - pointer to packet

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success
              IPC_ROUTER_STATUS_BUSY if Tx quota exceeded. Wait for event cb.
              Other failures

SIDE EFFECTS  
===========================================================================*/
int ipc_router_core_send_packet
(
 ipc_router_client_type *client,
 ipc_router_address_type *dest,
 ipc_router_packet_type *msg
);

/*===========================================================================
FUNCTION      ipc_router_core_send_to_name

DESCRIPTION   Multicasts packet to all services of the provided name
              in service.

ARGUMENTS     client (in)       - client handle returned from ipc_router_open
              service (in)      - Name of the service
              buf (in)          - pointer to the data to be written
              len (in)          - lenth of the message to send

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success
              IPC_ROUTER_STATUS_* failures

SIDE EFFECTS  
===========================================================================*/
int ipc_router_core_send_to_name
(
 ipc_router_client_type *client,
 ipc_router_service_type *service,
 const unsigned char *buf,
 uint32 len
);

/*===========================================================================
FUNCTION      ipc_router_core_reg_server

DESCRIPTION   Publish server

ARGUMENTS     client   - client handle
              service     - type and instance of the server

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  Clients' server event callbacks will be invoked
===========================================================================*/
int ipc_router_core_reg_server
(
 ipc_router_client_type  *client,
 ipc_router_service_type *service
);

/*===========================================================================
FUNCTION      ipc_router_core_dereg_server

DESCRIPTION   Withdraw server publication

ARGUMENTS     client - client handle
              service  - type and instance of the server

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  Clients' server event callbacks will be invoked
===========================================================================*/
int ipc_router_core_dereg_server
(
 ipc_router_client_type  *client,
 ipc_router_service_type *service
);

/*===========================================================================
FUNCTION      ipc_router_core_find_all_servers

DESCRIPTION   finds all the servers satisfying the type and specific bits
              (provided by the instance_mask) of the instance.

              client -      (Optional, in) client handle
              service -     (in) The service to find
              servers -     (out, optional) The found servers and their addresses
              num_entries   (in) size of servers, (out) size filled
              num_servers   (out, optional) total servers found
              instance_mask (in) The bits of the service:instance field 
                                 which should be used in compare operations
                                 in finding the service.
                                 Example: 
                                   0 - match only the service:type (all instances)
                                   0xffffffff - Exact match of service:instance
                                   0xff - Find all servers matching the first 8 bits
                                          in the provided service:instance

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_find_all_servers
(   
 ipc_router_client_type  *client,
 ipc_router_service_type *service,
 ipc_router_server_type  *servers,
 uint32                  *num_entries,
 uint32                  *num_servers,
 uint32                  instance_mask
);

/*===========================================================================
FUNCTION      ipc_router_core_send_conf

DESCRIPTION   This function sends a confirmation message to a transport
        
ARGUMENTS     client - pointer to client record where the conf_msg originates
              addr   - pointer to the destination address

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_send_conf
(
 ipc_router_client_type *client,
 ipc_router_address_type *addr
);

/*===========================================================================
FUNCTION      ipc_router_policy_register

DESCRIPTION   Register a policy manager's methods

ARGUMENTS     ops - Operation table of the external policy manager

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_policy_register(ipc_router_policy_ops_type *ops);


/*===========================================================================
FUNCTION      ipc_router_core_init

DESCRIPTION   Initializes the IPC router.  

ARGUMENTS     None

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  By default, the local transport is started.
===========================================================================*/
int ipc_router_core_init
(
 uint32 processor_id
);

/*===========================================================================
FUNCTION      ipc_router_core_deinit

DESCRIPTION   deintializes the IPC router.  

ARGUMENTS     None

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_core_deinit
(
 void
);


#endif

