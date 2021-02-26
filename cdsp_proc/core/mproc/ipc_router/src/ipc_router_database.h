#ifndef IPC_ROUTER_DATABASE_H
#define IPC_ROUTER_DATABASE_H
/*===========================================================================

                    I P C    R O U T E R    D A T A B A S E

   This file provides routing data managment interface.

  ---------------------------------------------------------------------------
  Copyright (c) 2007-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/ipc_router/src/ipc_router_database.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/07   sa      Initial version

===========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_os.h"
#include "ipc_router_types.h"
#include "ipc_router_xal.h"
#include "ipc_router_protocol.h"
#include "ipc_router_internal.h"
#include "ipc_router_core.h"

/*===========================================================================
                CONSTANT / MACRO DACLARATIONS
===========================================================================*/


/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
typedef enum
{
  IPC_ROUTER_WRITE_TOKEN_INVALID = 0,
  IPC_ROUTER_WRITE_TOKEN_NORMAL,
  IPC_ROUTER_WRITE_TOKEN_LAST,
  IPC_ROUTER_WRITE_TOKEN_NONE
} ipc_router_write_token_type;

typedef enum
{
  IPC_ROUTER_CRITERIA_INVALID = 0,
  IPC_ROUTER_CRITERIA_PROCESSOR,
  IPC_ROUTER_CRITERIA_ADDRESS,
  IPC_ROUTER_CRITERIA_XPORT,
  IPC_ROUTER_CRITERIA_SERVICE,
  IPC_ROUTER_CRITERIA_ALL
} ipc_router_criteria_type;

typedef struct
{
  const char                        *desc;
  int                                ref_count;
  ipc_router_address_type            remote_router;
  ipc_router_xal_ops_type           *ops;
  uint32                             version;
  uint32                             net_id;
  uint32                             priority;
  uint64                             xport_scope;
  void                              *handle;
  boolean                            up;
  boolean                            hello_sent;
} ipc_router_xport_type;

typedef struct ipc_router_cb_q_s
{
  ipc_router_address_type receiver;
  ipc_router_address_type sender;
  LINK(struct ipc_router_cb_q_s, link);
}ipc_router_cb_q_type;

typedef enum
{
  IPC_ROUTER_EP_MODE_NULL = 0,
  IPC_ROUTER_EP_MODE_SINGLE,
  IPC_ROUTER_EP_MODE_MULTI
} ipc_router_client_mode;

struct ipc_router_client_s
{
  ipc_router_address_type addr;
  uint32                 priority;
  uint64                 scope;

  ipc_router_rx_cb_type  rx_cb;
  void                  *rx_cb_data;

  ipc_router_event_cb_type  event_cb;
  void                     *event_cb_data;

  ipc_router_xport_type *xport;

  /* flow control fields. tx_count < rx_quota for data to be sent */
  uint16                   rx_quota;
  uint16                   rx_low_quota;
  uint16                   tx_count;

  uint32                   tx_size;
  uint32                   rx_size_quota;

  ipc_router_os_sem      mode_sem;
  ipc_router_xport_type  *connected_xport;
  ipc_router_client_mode mode;
   
  /* pointer back to database record */
  void                  *prv;
};

typedef struct
{
  uint32            crit;
  union 
  {
    uint32                         proc_arg;
    ipc_router_xport_type         *xport_arg;
    ipc_router_address_type       *addr_arg;
    struct 
    {
      ipc_router_service_type         *service;
      uint32                          instance_mask;
    } svc_arg;
  } arguments;
} ipc_router_search_criteria_type;

typedef struct client_results
{
  ipc_router_address_type          addr;
  LINK(struct client_results, link);
} ipc_router_client_list_type;

typedef struct client_handle_results
{
  ipc_router_client_type *client;
  LINK(struct client_handle_results, link);
} ipc_router_client_handle_list_type;

typedef struct server_results
{
  ipc_router_address_type          addr;
  ipc_router_service_type          service;
  LINK(struct server_results, link);
} ipc_router_server_list_type;

typedef struct xport_results
{
  ipc_router_xport_type           *xport;
  LINK(struct xport_results, link);
} ipc_router_xport_list_type;

typedef struct proc_results
{
  uint32                          processor_id;
  LINK(struct proc_results, link);
} ipc_router_proc_list_type;

typedef union
{
  LIST(ipc_router_client_list_type, clients);
  LIST(ipc_router_server_list_type, servers);
  LIST(ipc_router_xport_list_type, xports);
  LIST(ipc_router_proc_list_type, procs);
} ipc_router_search_results_type;

/*===========================================================================
FUNCTION      ipc_router_database_init

DESCRIPTION   initialize router database.  

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_database_init
(
 void
);

/*===========================================================================
FUNCTION      ipc_router_database_deinit

DESCRIPTION   Deinitialize router database.  

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_database_deinit
(
 void
);

/*===========================================================================
FUNCTION      ipc_router_register_client

DESCRIPTION   Initialize router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS - Creation of client succeeded
              IPC_ROUTER_STATUS_BUSY    - Client already exists.
              IPC_ROUTER_STATUS_*       - Error

SIDE EFFECTS  If the caller is interested in the created client
              (output parameter 'client' is a valid pointer),
              then a reference is taken on STATUS_SUCCESS or STATUS_BUSY
              for the client and the caller is responsible to release the 
              reference once they are done.
===========================================================================*/
#define ipc_router_register_client(ca, xp, cs, rx, rx_low, rx_size, mode) \
  ipc_router_register_client_ext(ca, xp, cs, rx, rx_low, rx_size, NULL, NULL, NULL, NULL, 0, IPC_ROUTER_DEFAULT_SCOPE, mode)
int ipc_router_register_client_ext
(
 ipc_router_address_type      *addr,
 ipc_router_xport_type        *xport,
 ipc_router_client_type      **client,
 uint16                        rx_quota,
 uint16                        rx_low_quota,
 uint32                        rx_size_quota,
 ipc_router_rx_cb_type         rx_cb,
 void                         *rx_cb_data,
 ipc_router_event_cb_type      event_cb,
 void                         *event_data,
 uint32                        priority,
 uint64                        xport_scope,
 ipc_router_client_mode        client_mode
);

/*===========================================================================
  FUNCTION      ipc_router_register_server

  DESCRIPTION   Register a server with the router.  

  RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

  SIDE EFFECTS  None
  ===========================================================================*/
int ipc_router_register_server
(   
 ipc_router_service_type *service,
 ipc_router_address_type *addr,
 ipc_router_xport_type   *xport
);

/*===========================================================================
  FUNCTION      ipc_router_unregister_client

DESCRIPTION   Initialize router.  

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_client
(
 ipc_router_address_type *addr
);

/*===========================================================================
FUNCTION      ipc_router_unregister_server

DESCRIPTION   Register a server with the router.  

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_server
(   
 ipc_router_service_type *services,
 ipc_router_address_type *addr
);

/*===========================================================================
FUNCTION      ipc_router_find_client

DESCRIPTION   finds the destination of a server if known to the  router.  

RETURN VALUE  returns pointer to client record with ref-count

SIDE EFFECTS  None
===========================================================================*/
ipc_router_client_type *ipc_router_find_client
(   
 ipc_router_address_type *addr
);

/*===========================================================================
FUNCTION      ipc_router_register_xport

DESCRIPTION   Registers a transport.  

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_register_xport
(
 ipc_router_xport_type *xport
);

/*===========================================================================
FUNCTION      ipc_router_unregister_xport

DESCRIPTION   Unregisters a transport.  

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_xport
(
 ipc_router_xport_type *xport
);

/*===========================================================================
FUNCTION      ipc_router_free_xport

DESCRIPTION   Unregisters a transport and free the xport data structure when
              the reference count goes to 0.  

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_free_xport
(
 ipc_router_xport_type *xport
);

/*===========================================================================
FUNCTION      ipc_router_find_xport

DESCRIPTION   finds the destination xport of a given processor

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
ipc_router_xport_type *ipc_router_find_xport
(   
 uint32 processor_id
);

/*===========================================================================
FUNCTION      ipc_router_lock_xport

DESCRIPTION   Verify a xport handle and lock it 

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_lock_xport
(
 ipc_router_xport_type *xport
);

/*===========================================================================
FUNCTION      ipc_router_unlock_xport

DESCRIPTION   Unlock a xport structure. xport must be previously locked.

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_unlock_xport
(
 ipc_router_xport_type *xport
);

/*===========================================================================
FUNCTION      ipc_router_unregister_proc

DESCRIPTION   Unregister the processor from the database and free all memory

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_proc
(
 uint32 processor_id
);

/*===========================================================================
FUNCTION      ipc_router_list_clients

DESCRIPTION     

RETURN VALUE  returns a link list of client addresses matching the given criteria
              in the output results variable.

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_clients
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type *results
);

/*===========================================================================
FUNCTION      ipc_router_list_servers

DESCRIPTION     

RETURN VALUE  returns a link list of server's program,version structs matching
              the given criteria in the output results variable.

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_servers
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type *results
);

/*===========================================================================
FUNCTION      ipc_router_list_xports

DESCRIPTION     

RETURN VALUE  returns a link list of xport address structs matching
              the given criteria in the output results variable.

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_xports
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type *results
);

/*===========================================================================
FUNCTION      ipc_router_list_procs

DESCRIPTION     

RETURN VALUE  returns a link list of processor address structs matching
              the given criteria in the output results variable.

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_procs
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type *results
 );

/*===========================================================================
FUNCTION      ipc_router_get_write_token

DESCRIPTION   Gives write permission to a given client.


ARGUMENTS     addr (in)  - A pointer to client address to which a token
                           is requested
              sender (in)  - client structure of the sender

RETURN VALUE  returns ipc_router_write_token_type.

SIDE EFFECTS  None
===========================================================================*/
ipc_router_write_token_type ipc_router_get_write_token
(   
 ipc_router_client_type *client,
 ipc_router_client_type *sender,
 uint32                 pkt_size
);

/*===========================================================================
FUNCTION      ipc_router_resume_client_tx

DESCRIPTION   Signals waiting writers to proceed with with sending messages.

ARGUMENTS     addr (in)  - A pointer to client address for which tx
                                  to be resumed

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_resume_client_tx
(   
 ipc_router_client_type         *client,
 ipc_router_address_type        *addr
);

/*===========================================================================
FUNCTION      ipc_router_unlock_client

DESCRIPTION   Decrease the reference count of a client record. Clean up
              when it reaches 0.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_unlock_client(ipc_router_client_type *client);

#endif
