/*===========================================================================

                    I P C    R O U T E R   C O R E

   This file describes the OS-independent interface to the IPC Router.
   This file must be compatible with all operating systems on which the 
   router will be used.

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/ipc_router/src/ipc_router_debug.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_os.h"
#include "ipc_router_database.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/
/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/
/*===========================================================================
                        EXPORTED FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      ipc_router_core_list_servers

DESCRIPTION   List all the servers

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_core_list_servers
(
 void
 )
{   
  ipc_router_search_criteria_type  cr;
  ipc_router_search_results_type   results;
  ipc_router_server_list_type     *server_list;

  /* Get a list of servers from this client */
  cr.crit = IPC_ROUTER_CRITERIA_ALL;
  ipc_router_list_servers(&cr, &results);

  IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO,"List of servers:\n");
  while(NULL != (server_list = LIST_HEAD(results.servers)))
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO,"\t(0x%x,0x%x)",
        server_list->service.type, server_list->service.instance);
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO,"\t @ 0x%x:0x%x\n",
        server_list->addr.processor_id, server_list->addr.port_id);

    LIST_REMOVE(results.servers, server_list, link);
    ipc_router_os_free(server_list);
  }
}

/*===========================================================================
FUNCTION      ipc_router_core_list_clients

DESCRIPTION   List all the clients

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_core_list_clients
(
 void
 )
{   
  ipc_router_search_criteria_type  cr;
  ipc_router_search_results_type   results;
  ipc_router_client_list_type     *client_list;
  ipc_router_client_type          *client;

  /* Get a list of servers from this client */
  cr.crit = IPC_ROUTER_CRITERIA_ALL;
  ipc_router_list_clients(&cr, &results);

  IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO,"List of clients:\n");
  while(NULL != (client_list = LIST_HEAD(results.clients)))
  {
    client = ipc_router_find_client(&client_list->addr);

    if(client)
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO, "\t0x%x:0x%x @ xport 0x%p\n",
          client_list->addr.processor_id, 
          client_list->addr.port_id, 
          client->xport);

      ipc_router_unlock_client(client);
    }

    LIST_REMOVE(results.clients, client_list, link);
    ipc_router_os_free(client_list);
  }
}

/*===========================================================================
FUNCTION      ipc_router_core_list_xports

DESCRIPTION   List all the transports

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_core_list_xports
(
 void
 )
{   
  ipc_router_search_criteria_type  cr;
  ipc_router_search_results_type   results;
  ipc_router_xport_list_type *xport_list;

  cr.crit = IPC_ROUTER_CRITERIA_ALL;
  ipc_router_list_xports(&cr, &results);

  IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO,"List of xports:\n");

  while(NULL != (xport_list = LIST_HEAD(results.xports)))
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO, "\t%p: %s pri:%x\n",
        xport_list->xport, xport_list->xport->desc,
        xport_list->xport->priority);

    LIST_REMOVE(results.xports, xport_list, link);
    ipc_router_os_free(xport_list);
  }
}

/*===========================================================================
FUNCTION      ipc_router_core_list_procs

DESCRIPTION   List all the transports

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_core_list_procs
(
 void
 )
{   
  ipc_router_search_criteria_type  cr;
  ipc_router_search_results_type   results;
  ipc_router_proc_list_type *proc_list;

  cr.crit = IPC_ROUTER_CRITERIA_ALL;
  ipc_router_list_procs(&cr, &results);

  IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO,"List of procs:\n");

  while(NULL != (proc_list = LIST_HEAD(results.procs)))
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO, "\t0x%x\n",
        proc_list->processor_id);

    LIST_REMOVE(results.procs, proc_list, link);
    ipc_router_os_free(proc_list);
  }
}
