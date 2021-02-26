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

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/ipc_router/src/ipc_router_database.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/07   sa      Initial version

===========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_database.h"
#include "ipc_router_log.h"

/*===========================================================================
                CONSTANT / MACRO DECLARATIONS
===========================================================================*/
#define IPC_ROUTER_PROC_TABLE_SIZE         4
#define IPC_ROUTER_SERVER_TABLE_SIZE       32
#define IPC_ROUTER_CLIENT_TABLE_SIZE       32


/* Quota for the number of messages that can be 
   received from a single processor. A confirmation
   control message will have to be received by the
   sending processor before it resumes sending messages

   RX Quota (H) = Maximum number of packets which can be sent without a RESUME_TX
   RX Low Quota (L) = the number of packets after which a CONF is requested.

   Total number of packets in flight is limited to (2H-L)
*/
#define IPC_ROUTER_DEFAULT_RX_QUOTA 10
#define IPC_ROUTER_DEFAULT_RX_LOW_QUOTA 5

/* Limit the total number of bytes transmitted to the destination by a fixed 
 * margin. The limit is approximate since IPC Router is a packet based IPC
 *
 * Abbreviating the limit to S (H & L is borrowed from above)
 *
 * Average outstanding allocations: S * (2H - L) / H  (Default: 120KB)
 * Worst case outstanding allocations: 2 * S          (Default: 160KB)
 *
 * Since the system requirements change based on deployment, this is
 * allowed to be compile-time configurable
 * */
#ifndef IPC_ROUTER_DEFAULT_RX_SIZE_QUOTA
#define IPC_ROUTER_DEFAULT_RX_SIZE_QUOTA  (1024 * 80)
#endif

#define CALLOC_BAIL(ptr, num, size, err_label) \
        if(NULL == ((ptr) = ipc_router_os_calloc((num), (size)))) goto err_label;
#define MALLOC_BAIL(ptr, size, err_label) \
        if(NULL == ((ptr) = ipc_router_os_malloc((size)))) goto err_label;

#define LOG_MEM_ERROR() log_mem_error(__LINE__)

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
typedef struct server_record
{
  ipc_router_service_type       service;
  ipc_router_address_type       server;
  ipc_router_xport_type         *xport;
  LINK(struct server_record, link);
} ipc_router_server_record_type;

typedef struct client_record
{
  ipc_router_client_type        client;
  int                           ref_count;
  LINK(struct client_record, link);
} ipc_router_client_record_type;

typedef struct xport_record
{
  ipc_router_xport_type         *xport;
  LINK(struct xport_record, link);
} ipc_router_xport_record_type;

typedef LIST(ipc_router_client_record_type, ipc_router_client_table_type)[IPC_ROUTER_CLIENT_TABLE_SIZE];

typedef struct proc_record
{
  uint32                        processor_id;
  ipc_router_xport_type         *xport;
  ipc_router_client_table_type  clients;
  LINK(struct proc_record, link);
} ipc_router_proc_record_type;

/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/
extern uint32 ipc_router_local_processor_id;

static LIST(ipc_router_proc_record_type,   g_client_tbls)[IPC_ROUTER_PROC_TABLE_SIZE];
static LIST(ipc_router_server_record_type, g_server_tbl)[IPC_ROUTER_SERVER_TABLE_SIZE];
static LIST(ipc_router_xport_record_type,  g_xport_list);
static LIST(ipc_router_cb_q_type,          g_flow_waiters);

static ipc_router_os_sem client_table_semaphore;
static ipc_router_os_sem server_table_semaphore;
static ipc_router_os_sem xport_table_semaphore;
static ipc_router_os_sem flow_waiters_semaphore;

#define find_proc_slot(processor_id) ((processor_id) & (IPC_ROUTER_PROC_TABLE_SIZE - 1))
#define find_server_slot(type) ((type) & (IPC_ROUTER_SERVER_TABLE_SIZE - 1))
#define find_client_slot(port_id) ((port_id) & (IPC_ROUTER_CLIENT_TABLE_SIZE - 1))

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/
static ipc_router_client_record_type *ipc_router_remove_client
(
 ipc_router_client_table_type *client_table,
 uint32 port_id
 );

static void ipc_router_cleanup_client(ipc_router_client_record_type *i);

static void log_mem_error(uint32 line)
{
  IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR, "ipc_router_database.c:%d malloc() error", line);
}

/*===========================================================================
FUNCTION      get_client_table

DESCRIPTION   Return cilent table given a processor ID

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_client_table_type *get_client_table(uint32 processor_id)
{
  ipc_router_proc_record_type *proc; 
  LIST_FIND(g_client_tbls[find_proc_slot(processor_id)],
            proc, link, proc->processor_id == processor_id);
  return proc ? &proc->clients : NULL;
}

/*===========================================================================
FUNCTION      add_client_table

DESCRIPTION   Add a new client table for processor and return pointer to it

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_client_table_type *add_client_table
(
 uint32 processor_id,
 ipc_router_xport_type *xport
 )
{
  ipc_router_proc_record_type *proc;
  proc = ipc_router_os_calloc(1, sizeof(ipc_router_proc_record_type));
  if(!proc)
    return NULL;

  proc->processor_id = processor_id;  
  proc->xport = xport;
  LIST_ADD(g_client_tbls[find_proc_slot(processor_id)], proc, link);

  return &proc->clients;
}

/*===========================================================================
FUNCTION      remove_proc_record

DESCRIPTION   Remove processor record

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static int remove_proc_record(uint32 processor_id)
{
  int rc = IPC_ROUTER_STATUS_FAILURE;
  ipc_router_proc_record_type *proc;

  LIST_FIND(g_client_tbls[find_proc_slot(processor_id)],
            proc, link, proc->processor_id == processor_id);
  if(proc)
  {
    int j;
    for(j = 0; j < IPC_ROUTER_CLIENT_TABLE_SIZE; j++)
    {
      while(proc->clients[j].head)
      {
        ipc_router_client_record_type *record = 
           ipc_router_remove_client(&proc->clients, 
                    proc->clients[j].head->client.addr.port_id);
        if(record && record->ref_count < 0)
          ipc_router_cleanup_client(record);
      }
    }

    LIST_REMOVE(g_client_tbls[find_proc_slot(processor_id)], proc, link);

    ipc_router_os_free(proc);
    rc = IPC_ROUTER_STATUS_SUCCESS;
  }
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_put_server

DESCRIPTION   finds the destination of a server if known to the  router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_put_server
(
 ipc_router_address_type *addr,
 ipc_router_service_type *service,
 ipc_router_xport_type *xport
 )
{
  uint32 index = 0;
  ipc_router_server_record_type *i = NULL;
  ipc_router_server_record_type *new;

  index = find_server_slot(service->type);
  LIST_FIND(g_server_tbl[index], i, link,
            i->service.type == service->type &&
            i->service.instance == service->instance &&
            i->server.processor_id == addr->processor_id &&
            i->server.port_id == addr->port_id);
  if(i)
  {
    return IPC_ROUTER_STATUS_INTERRUPTED;
  }

  new = ipc_router_os_calloc(1, sizeof(*new));
  if(!new)
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }
  new->xport = xport;
  new->server = *addr;
  new->service = *service;

  LIST_FIND(g_server_tbl[index], i, link,
                 i->service.type == service->type);

  if(i)
  {
    if(!i->xport)
    {
      LIST_ADD(g_server_tbl[index], new, link);
    }
    else
    {
      LIST_PREPEND(g_server_tbl[index], new, link);
    }
  }
  else
  {
    LIST_ADD(g_server_tbl[index], new, link);
  }

  return IPC_ROUTER_STATUS_SUCCESS;

}

/*===========================================================================
FUNCTION      ipc_router_remove_server

DESCRIPTION   remove server with type and address

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
static int  ipc_router_remove_server
(
 ipc_router_service_type *service,
 ipc_router_address_type *addr
 )
{ 
  uint32 index = 0;
  ipc_router_server_record_type *i = NULL;

  index = find_server_slot(service->type);

  LIST_FIND(g_server_tbl[index], i, link,
            i->service.type == service->type && 
            i->service.instance == service->instance &&
            i->server.processor_id == addr->processor_id &&
            i->server.port_id == addr->port_id);
  if(i)
  {
    LIST_REMOVE(g_server_tbl[index], i, link);
    ipc_router_os_free(i);
    return IPC_ROUTER_STATUS_SUCCESS;
  }
  return IPC_ROUTER_STATUS_FAILURE;
}

/*===========================================================================
FUNCTION      ipc_router_get_client

DESCRIPTION   finds the destination of a server if known to the router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_client_record_type *ipc_router_get_client
(
 ipc_router_client_table_type   *client_table,
 uint32                          port_id
 )
{
  uint32 index = 0;
  ipc_router_client_record_type *i = NULL;

  index = find_client_slot(port_id);
  LIST_FIND((*client_table)[index], i, link, i->client.addr.port_id == port_id);
  return i;
}

/*===========================================================================
FUNCTION      ipc_router_notify_tx_resume

DESCRIPTION   Notify clients that are waiting for a resume Tx event

RETURN VALUE  None

SIDE EFFECTS  Waiting clients will be notified with client lock held
===========================================================================*/
static void ipc_router_notify_tx_resume
(
 ipc_router_client_type *client,
 ipc_router_address_type *addr
)
{
  ipc_router_event_info_type info;
  ipc_router_cb_q_type *node;
  LIST(ipc_router_cb_q_type, notify_list);

  LIST_INIT(notify_list);

  /* resume transmission */
  ipc_router_os_sem_lock(&flow_waiters_semaphore);

  client->tx_count = 0;
  client->tx_size  = 0;

  /* Move all entries matching this receiver from the global
   * g_flow_waiters list into a local notify_list */
  node = LIST_HEAD(g_flow_waiters);
  while(node)
  {
    ipc_router_cb_q_type *next_node = node->link.next;
    if(0 == memcmp(&node->receiver, &client->addr, sizeof(client->addr)))
    {
      LIST_REMOVE(g_flow_waiters, node, link);
      LINK_INIT(node->link);
      LIST_ADD(notify_list, node, link);
    }
    node = next_node;
  }
  ipc_router_os_sem_unlock(&flow_waiters_semaphore);

  /* notify client of resume tx event and free the node */
  info.client = *addr;
  while(NULL != (node = LIST_HEAD(notify_list)))
  {
    ipc_router_client_type *sender = ipc_router_find_client(&node->sender);

    if(sender)
    {
      if(sender->event_cb)
      {
        sender->event_cb(
            sender, 
            sender->event_cb_data,
            IPC_ROUTER_EVENT_RESUME_TX,
            &info
            );
      }
      /* unlock sender's client record */
      ipc_router_unlock_client(sender);
    }

    LIST_REMOVE(notify_list, node, link);
    ipc_router_os_free(node);
  }
}

/*===========================================================================
FUNCTION      ipc_router_dec_ref_count

DESCRIPTION   Decrease the reference count of a client record.

              Client table must be locked

RETURN VALUE  Reference count

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_dec_ref_count(ipc_router_client_record_type *i)
{
  i->ref_count--;
  return i->ref_count;
}

/*===========================================================================
FUNCTION      ipc_router_cleanup_client

DESCRIPTION   Clean up when client reference count reaches 0.

              Client table needs not be locked

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_cleanup_client(ipc_router_client_record_type *i)
{
  ipc_router_cb_q_type *node;

  /* wake up clients waiting to write */
  ipc_router_notify_tx_resume(&i->client, &i->client.addr);

  /* call client's event callback with the IPC_ROUTER_EVENT_CLOSED event */
  if(i->client.event_cb)
  {
    ipc_router_event_info_type info;

    info.client = i->client.addr;
    i->client.event_cb(&i->client, i->client.event_cb_data, 
                          IPC_ROUTER_EVENT_CLOSED, &info);
  }

  /* Cleanup global waiter list of this particular sender */
  ipc_router_os_sem_lock(&flow_waiters_semaphore);
  node = LIST_HEAD(g_flow_waiters);
  while(node)
  {
    ipc_router_cb_q_type *next_node = node->link.next;
    if(0 == memcmp(&i->client.addr, &node->sender, sizeof(node->sender)))
    {
      LIST_REMOVE(g_flow_waiters, node, link);
      ipc_router_os_free(node);
    }
    node = next_node;
  }
  ipc_router_os_sem_unlock(&flow_waiters_semaphore);

  ipc_router_os_sem_deinit(&i->client.mode_sem);
  ipc_router_os_free(i);
}

/*===========================================================================
FUNCTION      ipc_router_unlock_client

DESCRIPTION   Decrease the reference count of a client record. Clean up
              when it reaches 0.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_unlock_client(ipc_router_client_type *client)
{
  int ref_count;
  ipc_router_os_sem_lock(&client_table_semaphore);
  ref_count = ipc_router_dec_ref_count((ipc_router_client_record_type *)client->prv);
  ipc_router_os_sem_unlock(&client_table_semaphore);
  if(ref_count < 0)
    ipc_router_cleanup_client((ipc_router_client_record_type *)client->prv);
}

/*===========================================================================
FUNCTION      ipc_router_remove_client

DESCRIPTION   finds the destination of a server if known to the  router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_client_record_type *ipc_router_remove_client
(
 ipc_router_client_table_type   *client_table,
 uint32                          port_id
 )
{ 
  uint32 index = 0;
  ipc_router_client_record_type *i = NULL;

  index = find_client_slot(port_id);
  LIST_FIND((*client_table)[index], i, link, i->client.addr.port_id == port_id);
  if(i)
  {
    LIST_REMOVE((*client_table)[index], i, link);
    ipc_router_dec_ref_count(i);
  }
  return i;
}

/*===========================================================================
FUNCTION      ipc_router_event_notify

DESCRIPTION   Notify clients on the local processor of events (new server,
              remove server, resume tx). This is a broadcast too all clients.

RETURN VALUE  Invoke event callback for each client.

SIDE EFFECTS  Client record is locked during callbacks
===========================================================================*/
static void ipc_router_event_notify
(
 ipc_router_event_type event,
 ipc_router_event_info_type *info,
 ipc_router_address_type *source
 )
{
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;
  ipc_router_client_list_type *client_list;

  /* We must not be holding the client table lock when making callbacks to
   * prevent deadlocks - get a list of clients and access them individually
   */
  cr.crit = IPC_ROUTER_CRITERIA_XPORT;
  cr.arguments.xport_arg = NULL; /* Local processor */
  ipc_router_list_clients(&cr, &results);

  while(NULL != (client_list = LIST_HEAD(results.clients)))
  {
    /* Avoid notifying the source */
    if(client_list->addr.processor_id != source->processor_id ||
       client_list->addr.port_id != source->port_id)
    {
      ipc_router_client_type *client;
      client = ipc_router_find_client(&client_list->addr);
      if(client)
      {
        if(client->event_cb)
        {
          client->event_cb(client, client->event_cb_data, event, info);
        }
        ipc_router_unlock_client(client);
      }
    }
    LIST_REMOVE(results.clients, client_list, link);
    ipc_router_os_free(client_list);
  }
}

/*===========================================================================
FUNCTION      ipc_router_database_init

DESCRIPTION   Initialize router database.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_database_init(void)
{  
  ipc_router_os_sem_init(&client_table_semaphore);
  ipc_router_os_sem_init(&server_table_semaphore);
  ipc_router_os_sem_init(&xport_table_semaphore);
  ipc_router_os_sem_init(&flow_waiters_semaphore);

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_database_deinit

DESCRIPTION   Deinitialize router database.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_database_deinit
(
)
{
  /* cleanup global semaphores */
  ipc_router_os_sem_deinit(&client_table_semaphore);
  ipc_router_os_sem_deinit(&server_table_semaphore);
  ipc_router_os_sem_deinit(&xport_table_semaphore);
  ipc_router_os_sem_deinit(&flow_waiters_semaphore);

  return IPC_ROUTER_STATUS_SUCCESS;;
}

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
int ipc_router_register_client_ext
(
 ipc_router_address_type       *addr,
 ipc_router_xport_type         *xport,
 ipc_router_client_type       **client,
 uint16                        rx_quota,
 uint16                        rx_low_quota,
 uint32                        rx_size_quota,
 ipc_router_rx_cb_type         rx_cb,
 void                         *rx_cb_data,
 ipc_router_event_cb_type      event_cb,
 void                         *event_cb_data,
 uint32                        priority,
 uint64                        xport_scope,
 ipc_router_client_mode        client_mode
 )
{
  int ret = IPC_ROUTER_STATUS_NO_MEM;
  ipc_router_client_type *new_client;
  ipc_router_client_table_type *tbl;
  ipc_router_client_record_type *record = NULL;

  ipc_router_os_sem_lock(&client_table_semaphore);

  tbl = get_client_table(addr->processor_id);
  if(tbl)
  {
    if(NULL != (record = ipc_router_get_client(tbl, addr->port_id)))
    {
      /* Client record already exists */
      ret = IPC_ROUTER_STATUS_BUSY;
      if(xport && record->client.xport && 
          xport->priority > record->client.xport->priority)
      {
        /* update xport */
        record->client.xport = xport;
        /* Adjust client's scope to be the lowest 
         * common denominator */
        record->client.scope &= xport_scope;

        ret = IPC_ROUTER_STATUS_SUCCESS;
      }
      goto set_client_unlock_bail;
    }
  }
  else
  {
    /* Create a table for this new node */
    if(NULL == (tbl = add_client_table(addr->processor_id, xport)))
    {
      goto unlock_bail;
    }
  }

  CALLOC_BAIL(record, 1, sizeof(*record), unlock_bail);
  ret = IPC_ROUTER_STATUS_SUCCESS;
  new_client = &record->client;
  new_client->addr = *addr;
  new_client->prv = record;
  new_client->rx_cb = rx_cb;
  new_client->rx_cb_data = rx_cb_data;
  new_client->event_cb = event_cb;
  new_client->event_cb_data = event_cb_data;
  new_client->xport = xport;
  new_client->rx_quota = rx_quota ? rx_quota : IPC_ROUTER_DEFAULT_RX_QUOTA;  
  new_client->rx_low_quota = rx_low_quota ? rx_low_quota : IPC_ROUTER_DEFAULT_RX_LOW_QUOTA;
  new_client->rx_size_quota = rx_size_quota ? rx_size_quota : IPC_ROUTER_DEFAULT_RX_SIZE_QUOTA;
  new_client->priority = priority;
  new_client->scope = xport_scope;
  new_client->mode = client_mode;
  new_client->connected_xport = NULL;
  ipc_router_os_sem_init(&new_client->mode_sem);
  LIST_ADD((*tbl)[find_client_slot(addr->port_id)], record, link);
set_client_unlock_bail:
  if(client)
  {
    /* Take a reference for the caller */
    record->ref_count++;
    *client = &record->client;
  }
unlock_bail:
  ipc_router_os_sem_unlock(&client_table_semaphore);

  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_register_server

DESCRIPTION   Register a server with the router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  Clients will be notified of the event
===========================================================================*/
int ipc_router_register_server
(   
 ipc_router_service_type *service,
 ipc_router_address_type *addr,
 ipc_router_xport_type   *xport
 )
{
  int ret = IPC_ROUTER_STATUS_FAILURE;
  ipc_router_event_info_type info;

  if(!service || !addr)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  ipc_router_os_sem_lock(&server_table_semaphore);

  ret = ipc_router_put_server(addr, service, xport);

  ipc_router_os_sem_unlock(&server_table_semaphore);

  if(ret == IPC_ROUTER_STATUS_SUCCESS)
  {
    info.server.service = *service;
    info.server.addr = *addr;

    ipc_router_event_notify(
        IPC_ROUTER_EVENT_NEW_SERVER,
        &info,
        addr
        );
  }
  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_unregister_client

DESCRIPTION   Initialize router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_client
(
 ipc_router_address_type *addr
 )
{
  ipc_router_event_info_type info;
  ipc_router_client_table_type *tbl;
  ipc_router_client_record_type *record = NULL;
  int ref_count = 0;

  info.client = *addr;

  ipc_router_os_sem_lock(&client_table_semaphore);

  tbl = get_client_table(addr->processor_id);
  if(tbl)
  {
    record = ipc_router_remove_client(tbl, addr->port_id);

    /* save ref_count before unlocking table */
    if(record)
      ref_count = record->ref_count;
  }

  ipc_router_os_sem_unlock(&client_table_semaphore);

  /* clean up client with client_table_semaphore unlocked because it invokes
   * client's event callback
   */
  if(ref_count < 0)
    ipc_router_cleanup_client(record);

  /* Notify everyone of the event */
  ipc_router_event_notify(IPC_ROUTER_EVENT_REMOVE_CLIENT, &info, addr);

  return record ? IPC_ROUTER_STATUS_SUCCESS : IPC_ROUTER_STATUS_FAILURE;
}

/*===========================================================================
FUNCTION      ipc_router_unregister_server

DESCRIPTION   Register a server with the router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_server
(   
 ipc_router_service_type *service,
 ipc_router_address_type *addr
 )
{
  ipc_router_event_info_type info;

  info.server.service = *service;
  info.server.addr = *addr;

  ipc_router_os_sem_lock(&server_table_semaphore);

  if(ipc_router_remove_server(service, addr) != 
      IPC_ROUTER_STATUS_SUCCESS)
  {
    ipc_router_os_sem_unlock(&server_table_semaphore);
    return IPC_ROUTER_STATUS_FAILURE;
  }

  ipc_router_os_sem_unlock(&server_table_semaphore);

  ipc_router_event_notify(
      IPC_ROUTER_EVENT_REMOVE_SERVER,
      &info,
      addr
      );

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_find_client

DESCRIPTION   finds the destination of a server if known to the  router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  Record of the client is locked
===========================================================================*/
ipc_router_client_type *ipc_router_find_client
(   
 ipc_router_address_type *addr
 )
{
  ipc_router_client_record_type *temp = NULL;
  ipc_router_client_table_type *tbl;

  ipc_router_os_sem_lock(&client_table_semaphore);

  tbl = get_client_table(addr->processor_id);
  if(tbl)
    temp = ipc_router_get_client(tbl, addr->port_id);

  /* Increase reference count so we don't nuke it */
  if(temp)
    temp->ref_count++;

  ipc_router_os_sem_unlock(&client_table_semaphore);

  return( temp ? &temp->client : NULL );
}

/*===========================================================================
FUNCTION      ipc_router_register_xport

DESCRIPTION   Registers a transport.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_register_xport
(
 ipc_router_xport_type *xport
 )
{
  ipc_router_xport_record_type *xport_record;

  CALLOC_BAIL(xport_record, 1, sizeof(ipc_router_xport_record_type),
      register_xport_err0);

  xport_record->xport = xport;

  ipc_router_os_sem_lock(&xport_table_semaphore);
  LIST_ADD(g_xport_list, xport_record, link);
  ipc_router_os_sem_unlock(&xport_table_semaphore);

  return IPC_ROUTER_STATUS_SUCCESS;

register_xport_err0:
  return IPC_ROUTER_STATUS_NO_MEM;
}

/*===========================================================================
FUNCTION      ipc_router_unregister_xport

DESCRIPTION   Unregister the xport from the database but not free the xport

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_xport
(
 ipc_router_xport_type *xport
 )
{
  ipc_router_xport_record_type *i;
  int rc = IPC_ROUTER_STATUS_INVALID_PARAM;

  ipc_router_os_sem_lock(&xport_table_semaphore);
  LIST_FIND(g_xport_list, i, link, i->xport == xport);
  if(i)
  {
    LIST_REMOVE(g_xport_list, i, link);
    ipc_router_os_free(i);
    rc = IPC_ROUTER_STATUS_SUCCESS;
  }
  ipc_router_os_sem_unlock(&xport_table_semaphore);
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_xport_dec_ref

DESCRIPTION   Unregisters a transport and free the xport data structure when
              the reference count goes to 0.  

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_xport_dec_ref
(
 ipc_router_xport_type *xport
 )
{
  if(xport->ref_count)
    xport->ref_count--;
  else
    ipc_router_os_free(xport);
}

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
 )
{
  if(!xport)
    return;

  ipc_router_unregister_xport(xport);

  ipc_router_os_sem_lock(&xport_table_semaphore);
  ipc_router_xport_dec_ref(xport);
  ipc_router_os_sem_unlock(&xport_table_semaphore);
}

/*===========================================================================
FUNCTION      ipc_router_find_xport

DESCRIPTION   finds the destination xport of a given processor

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
ipc_router_xport_type *ipc_router_find_xport
(   
 uint32 processor_id
)
{
  ipc_router_xport_type *xport = NULL;
  ipc_router_proc_record_type *proc; 
  uint32 index = find_proc_slot(processor_id);

  ipc_router_os_sem_lock(&client_table_semaphore);
  LIST_FIND(g_client_tbls[index], proc, link, proc->processor_id == processor_id);
  if(proc)
  {
    xport = proc->xport;
  }
  ipc_router_os_sem_unlock(&client_table_semaphore);

  return xport;
}

/*===========================================================================
FUNCTION      ipc_router_lock_xport

DESCRIPTION   Verify a xport handle and lock it 

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_lock_xport
(
 ipc_router_xport_type *xport
)
{
  ipc_router_xport_record_type * i;
  int rc = IPC_ROUTER_STATUS_INVALID_PARAM;

  if(!xport)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  ipc_router_os_sem_lock(&xport_table_semaphore);
  LIST_FIND(g_xport_list, i, link, i->xport == xport);
  if(i)
  {
    rc = IPC_ROUTER_STATUS_SUCCESS;
    xport->ref_count++;
  }
  ipc_router_os_sem_unlock(&xport_table_semaphore);
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_unlock_xport

DESCRIPTION   Unlock a xport structure. xport must be previously locked.

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_unlock_xport
(
 ipc_router_xport_type *xport
)
{
  if(!xport)
    return;

  ipc_router_os_sem_lock(&xport_table_semaphore);
  ipc_router_xport_dec_ref(xport);
  ipc_router_os_sem_unlock(&xport_table_semaphore);
}

/*===========================================================================
FUNCTION      ipc_router_unregister_proc

DESCRIPTION   Unregister the processor from the database and free all memory

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_proc
(
 uint32 processor_id
 )
{
  int rc;
  ipc_router_os_sem_lock(&client_table_semaphore);
  rc = remove_proc_record(processor_id);
  ipc_router_os_sem_unlock(&client_table_semaphore);
  return rc;
}

/*===========================================================================
FUNCTION      create_client_list_node

DESCRIPTION   allocate and initialize client list node

RETURN VALUE  pointer to new client list node

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_client_list_type* create_client_list_node
(
 uint32 processor_id,
 uint32 port_id
)
{
  ipc_router_client_list_type *node;
  node = ipc_router_os_calloc(1, sizeof(ipc_router_client_list_type));
  if(node)
  {
    node->addr.processor_id = processor_id;
    node->addr.port_id = port_id;
  }
  return node;
}

/*===========================================================================
FUNCTION      ipc_router_list_clients

DESCRIPTION     

RETURN VALUE  returns a link list of client addresses matching the given criteria

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_clients
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type   *results
 )
{
  uint32 j,k;
  ipc_router_client_record_type *i = NULL;
  ipc_router_client_list_type *node = NULL;
  ipc_router_proc_record_type *proc;

  LIST_INIT(results->clients);

  ipc_router_os_sem_lock(&client_table_semaphore);

  switch(cr->crit)
  {
    case IPC_ROUTER_CRITERIA_XPORT:
      for(j = 0 ; j < IPC_ROUTER_PROC_TABLE_SIZE; j++)
      {
        proc = g_client_tbls[j].head;
        while(proc)
        {
          for(k = 0 ; k < IPC_ROUTER_CLIENT_TABLE_SIZE ; k++)
          {
            i = proc->clients[k].head;
            while(i)
            {
              if(i->client.xport == cr->arguments.xport_arg)
              {
                node = create_client_list_node(proc->processor_id, i->client.addr.port_id);
                if(!node)
                {
                  LOG_MEM_ERROR();
                  goto list_clients_bail;
                }
                LIST_ADD(results->clients, node, link);
              }
              i = i->link.next;
            }
          }
          proc = proc->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_ALL:
      for(j = 0 ; j < IPC_ROUTER_PROC_TABLE_SIZE ; j++)
      {
        proc = g_client_tbls[j].head;
        while(proc)
        {
          for(k = 0 ; k < IPC_ROUTER_CLIENT_TABLE_SIZE ; k++)
          {
            i = proc->clients[k].head;
            while(i)
            {
              node = create_client_list_node(proc->processor_id, i->client.addr.port_id);
              
              if(!node)
              {
                LOG_MEM_ERROR();
                goto list_clients_bail;
              }
              LIST_ADD(results->clients, node, link);
              i = i->link.next;
            }
          }
          proc = proc->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_PROCESSOR:
    case IPC_ROUTER_CRITERIA_ADDRESS:
    case IPC_ROUTER_CRITERIA_INVALID:   /* Fall through */
    default:
      break;
  }

list_clients_bail:
  ipc_router_os_sem_unlock(&client_table_semaphore);
}

/*===========================================================================
FUNCTION      create_server_list_node

DESCRIPTION   allocate and initialize server list node

RETURN VALUE  pointer to new server list node

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_server_list_type *create_server_list_node
(
 ipc_router_address_type *addr,
 ipc_router_service_type *service
)
{
  ipc_router_server_list_type *node;
  node = ipc_router_os_calloc(1, sizeof(ipc_router_server_list_type));
  if(node)
  {
    node->addr = *addr;
    node->service = *service;
  }
  return node;
}

/*===========================================================================
FUNCTION      ipc_router_list_servers

DESCRIPTION     

RETURN VALUE  returns a link list of server's program,version structs matching
              the given criteria

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_servers
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type   *results
 )
{
  uint32                            k;
  ipc_router_server_record_type    *i            = NULL;
  ipc_router_server_list_type      *node         = NULL;
  uint32                           mask;
  ipc_router_service_type          *service;

  LIST_INIT(results->servers);

  ipc_router_os_sem_lock(&server_table_semaphore);
  switch(cr->crit)
  {
    case IPC_ROUTER_CRITERIA_ADDRESS:
      for(k = 0 ; k < IPC_ROUTER_SERVER_TABLE_SIZE ; k++)
      {
        i = g_server_tbl[k].head;
        while(i)
        {
          if((i->server.processor_id == cr->arguments.addr_arg->processor_id) &&
              (i->server.port_id == cr->arguments.addr_arg->port_id))
          {
            node = create_server_list_node(&i->server, &i->service);
            if(!node)
            {
              LOG_MEM_ERROR();
              goto list_servers_bail;
            }
            LIST_ADD(results->servers, node, link);
          }
          i = i->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_SERVICE:
      mask      = cr->arguments.svc_arg.instance_mask;
      service   = cr->arguments.svc_arg.service;
      i = LIST_HEAD(g_server_tbl[find_server_slot(service->type)]);
      while(i)
      {
        if(i->service.type == service->type &&
           (i->service.instance & mask) == (service->instance & mask))
        {
          node = create_server_list_node(&i->server, &i->service);
          if(!node)
          {
            LOG_MEM_ERROR();
            goto list_servers_bail;
          }
          LIST_ADD(results->servers, node, link);
        }
        i = i->link.next;
      }
      break;

    case IPC_ROUTER_CRITERIA_XPORT:
      for(k = 0 ; k < IPC_ROUTER_SERVER_TABLE_SIZE ; k++)
      {
        i = g_server_tbl[k].head;
        while(i)
        {
          /* If this is a dummy record, skip it */
          if(!(i->xport))
          {
            i = i->link.next;
            continue;
          }

          if(i->xport == cr->arguments.xport_arg) 
          {
            node = create_server_list_node(&i->server, &i->service);
            if(!node)
            {
              LOG_MEM_ERROR();
              goto list_servers_bail;
            }
            LIST_ADD(results->servers, node, link);
          }
          i = i->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_ALL:
      for(k = 0 ; k < IPC_ROUTER_SERVER_TABLE_SIZE ; k++)
      {
        i = g_server_tbl[k].head;
        while(i)
        {
          node = create_server_list_node(&i->server, &i->service);
          if(!node)
          {
            LOG_MEM_ERROR();
            goto list_servers_bail;
          }
          LIST_ADD(results->servers, node, link);
          i = i->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_PROCESSOR:
    case IPC_ROUTER_CRITERIA_INVALID:   /* Fall through */
    default:
      break;
  }

list_servers_bail:
  ipc_router_os_sem_unlock(&server_table_semaphore);
}

/*===========================================================================
FUNCTION      ipc_router_list_xports

DESCRIPTION     

RETURN VALUE  returns a link list of xport address structs matching
              the given criteria

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_xports
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type   *results
 )
{
  ipc_router_xport_record_type *i = NULL;
  ipc_router_xport_list_type   *node = NULL;

  LIST_INIT(results->xports);
  ipc_router_os_sem_lock(&xport_table_semaphore);
  switch(cr->crit)
  {
    case IPC_ROUTER_CRITERIA_ALL:      
      i = g_xport_list.head;
      while(i)
      {
        node = ipc_router_os_calloc(1, sizeof(ipc_router_xport_list_type));
        if(!node)
        {
          LOG_MEM_ERROR();
          goto list_xports_bail;
        }
        node->xport = i->xport;
        LIST_ADD(results->xports, node, link);
        i = i->link.next;
      }
      break;

    case IPC_ROUTER_CRITERIA_PROCESSOR:   /* Fall through */
    case IPC_ROUTER_CRITERIA_ADDRESS:     /* Fall through */
    case IPC_ROUTER_CRITERIA_XPORT:       /* Fall through */
    case IPC_ROUTER_CRITERIA_INVALID:     /* Fall through */
    default:
      break;
  }
list_xports_bail:
  ipc_router_os_sem_unlock(&xport_table_semaphore);
}

/*===========================================================================
FUNCTION      create_proc_list_node

DESCRIPTION   allocate and initialize proc list node

RETURN VALUE  pointer to new proc list node

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_proc_list_type *create_proc_list_node
(
 int32 processor_id
)
{
  ipc_router_proc_list_type *node; 
  node = ipc_router_os_calloc(1, sizeof(ipc_router_proc_list_type));
  if(node)
  {
    node->processor_id = processor_id;
  }
  return node;
}

/*===========================================================================
FUNCTION      ipc_router_list_procs

DESCRIPTION     

RETURN VALUE  returns a link list of processor address structs matching
              the given criteria

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_procs
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type   *results
 )
{
  uint32 j;
  ipc_router_proc_record_type *proc;
  ipc_router_proc_list_type *node; 

  LIST_INIT(results->procs);

  ipc_router_os_sem_lock(&client_table_semaphore);

  switch(cr->crit){
    case IPC_ROUTER_CRITERIA_ALL:
      for(j = 0 ; j < IPC_ROUTER_PROC_TABLE_SIZE ; j++)
      {
        proc = g_client_tbls[j].head;
        while(proc)
        {
          node = create_proc_list_node(proc->processor_id);
          if(!node)
          {
            LOG_MEM_ERROR();
            goto list_procs_bail;
          }
          LIST_ADD(results->procs, node, link);
          proc = proc->link.next;
        }
      }
      break;
    case IPC_ROUTER_CRITERIA_XPORT:       /* Fall through */
      for(j = 0 ; j < IPC_ROUTER_PROC_TABLE_SIZE ; j++)
      {
        proc = g_client_tbls[j].head;
        while(proc)
        {
          if(proc->xport == cr->arguments.xport_arg)
          {
            node = create_proc_list_node(proc->processor_id);
            if(!node)
            {
              LOG_MEM_ERROR();
              goto list_procs_bail;
            }
            LIST_ADD(results->procs, node, link);
          }
          proc = proc->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_PROCESSOR:   /* Fall through */
    case IPC_ROUTER_CRITERIA_ADDRESS:     /* Fall through */
    case IPC_ROUTER_CRITERIA_INVALID:     /* Fall through */
    default:
      break;
  }

list_procs_bail:
  ipc_router_os_sem_unlock(&client_table_semaphore);
}
/*===========================================================================
FUNCTION      ipc_router_get_write_token

DESCRIPTION   Gives write permission to a given client.


ARGUMENTS     client (in)  - A pointer to client structure to which a token
                             is requested
              sender (in)  - client structure of the sender

RETURN VALUE  returns ipc_router_write_token_type.

SIDE EFFECTS  Requires exclusive access to client structure
===========================================================================*/
ipc_router_write_token_type ipc_router_get_write_token
(   
 ipc_router_client_type *client, 
 ipc_router_client_type *sender,
 uint32                 pkt_size
 )
{
  ipc_router_write_token_type ret;
  ipc_router_cb_q_type *node;

  if(!client || client->rx_quota <= 0)
  {
    return IPC_ROUTER_WRITE_TOKEN_INVALID;
  }

  ipc_router_os_sem_lock(&flow_waiters_semaphore);

  /* Check to see if we've exceeded the client's rx quota */
  if((int)client->tx_count < client->rx_quota && client->tx_size < client->rx_size_quota)
  {
    client->tx_size += pkt_size;
    client->tx_count++;

    /* Request for confirmation if we either exceed the low count
     * quota or the size quota */
    if(client->tx_count == client->rx_low_quota || 
        (client->tx_count < client->rx_low_quota && 
          client->tx_size >= client->rx_size_quota))
    {
      ret = IPC_ROUTER_WRITE_TOKEN_LAST;
    }
    else
    {
      ret = IPC_ROUTER_WRITE_TOKEN_NORMAL;
    }
  }
  else
  {
    ret = IPC_ROUTER_WRITE_TOKEN_NONE;

    /* Do not add the sender again if it is already
     * present in the tx_cb_q */
    LIST_FIND(g_flow_waiters, node, link, 
                0 == memcmp(&node->sender, &sender->addr, sizeof(node->sender)) &&
                0 == memcmp(&node->receiver, &client->addr, sizeof(node->receiver)));
    if(!node)
    {
      node = ipc_router_os_calloc(1, sizeof(ipc_router_cb_q_type));
      if(!node)
      {
        LOG_MEM_ERROR();
      }
      else
      {
        /* Use sender's address since the client reference can be freed up by the
         * time we get the confirmation
         */
        node->sender = sender->addr;
        node->receiver = client->addr;

        /* Add client to destination client's callback queue */
        LIST_ADD(g_flow_waiters, node, link);
      }
    }
  }

  ipc_router_os_sem_unlock(&flow_waiters_semaphore);
  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_resume_client_tx

DESCRIPTION   Signals waiting writers to proceed with with sending messages.

ARGUMENTS     addr (in)  - A pointer to client address for which tx
                                  to be resumed

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success
              IPC_ROUTER_STATUS_FAILURE if client is not found locally

SIDE EFFECTS  Client record locked during event callback
===========================================================================*/
void ipc_router_resume_client_tx
(   
 ipc_router_client_type *client,
 ipc_router_address_type *addr
 )
{
  if(!client)
    return;

  ipc_router_notify_tx_resume(client, addr);
  return;
}
