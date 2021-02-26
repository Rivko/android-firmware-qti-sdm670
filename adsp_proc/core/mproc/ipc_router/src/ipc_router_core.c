/*===========================================================================

                    I P C    R O U T E R   C O R E

   This file describes the OS-independent interface to the IPC Router.
   This file must be compatible with all operating systems on which the 
   router will be used.

  ---------------------------------------------------------------------------
  Copyright (c) 2007-2013,2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/ipc_router/src/ipc_router_core.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_os.h"
#include "ipc_router_database.h"
#include "ipc_router_packet.h"
#include "ipc_router_protocol.h"
#include "ipc_router_log.h"

/*==========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/
#define IPC_ROUTER_POLICY_CAN_SEND(scope, proc, xp_scope) \
  (ipc_router_policy.send ? ipc_router_policy.send(scope, proc, xp_scope) : 1)

#define IPC_ROUTER_POLICY_CAN_RECV(xp_scope, proc, scope) \
  (ipc_router_policy.recv ? ipc_router_policy.recv(xp_scope, proc, scope) : 1)

#define IPC_ROUTER_POLICY_GET_SCOPE(type, instance, host, scope) \
  (ipc_router_policy.get_scope ? ipc_router_policy.get_scope(type, instance, host, scope) : 1)

#define IPC_ROUTER_POLICY_GET_LINK_SCOPE(xport_tag) \
  (ipc_router_policy.get_xport_scope && xport_tag ? ipc_router_policy.get_xport_scope(xport_tag) : IPC_ROUTER_DEFAULT_SCOPE)

#define ADJUST_CLIENT_MODE_ON_SEND(client, dest_xport) \
  do { \
    ipc_router_os_sem_lock(&((client)->mode_sem));  \
    if((client)->mode == IPC_ROUTER_EP_MODE_NULL) \
    { \
      (client)->mode = IPC_ROUTER_EP_MODE_SINGLE; \
      (client)->connected_xport = dest_xport; \
    } \
    else if( (client)->mode == IPC_ROUTER_EP_MODE_SINGLE && \
            (client)->connected_xport != dest_xport) \
    { \
      (client)->mode = IPC_ROUTER_EP_MODE_MULTI; \
      (client)->connected_xport = NULL; \
    } \
    ipc_router_os_sem_unlock(&((client)->mode_sem));  \
  } while(0)

#define IPC_ROUTER_MAX_TRIES 0xfffeL

/* Padding to make the packets word aligned */
#define PAD_LEN(len) ((4 - ((len) & 3)) & 3)

/* Return the net mask (color) of the transport */
#define IPC_ROUTER_XPORT_NET(xport) ((xport) ? (xport)->net_id : 0)

/* Return a bit mask where all bits are set except the bit set in xport->net_id.
 * If xport was NULL, then it returns a bitmask with all bits set to 1 */
#define IPC_ROUTER_OTHER_XPORT_MASK(xport) (IPC_ROUTER_XPORT_NET(xport) ^ IPC_ROUTER_XPORT_NET_ALL)

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/
uint32 ipc_router_local_processor_id = 0xffffffff;

/* port semaphore and auto port are used to assign port numbers */
static ipc_router_os_sem port_semaphore;
static uint16 ipc_router_auto_port;
ipc_router_policy_ops_type ipc_router_policy = {NULL, NULL, NULL, NULL};

/* protocol semaphore is used to enforce compliance with protocol in concurrent
 * scenarios such as receiving a hello message while a server is being 
 * registered.
 */
static ipc_router_os_sem protocol_semaphore;

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/
static int ipc_router_send_cntl_msg
(
 ipc_router_control_msg_body   *cntl_msg,
 ipc_router_xport_type         *xport,
 ipc_router_address_type       *addr
 );

static int ipc_router_bcast_cntl_msg
(
 ipc_router_control_msg_body *cntl_msg,
 uint32 net_id_mask,
 uint32 priority_pref,
 uint64 scope
 );

static int ipc_router_remove_client(ipc_router_address_type *addr);

static uint64 get_client_scope(ipc_router_address_type *addr)
{
  uint64 scope = 0;
  ipc_router_client_type *client;
  client = ipc_router_find_client(addr);
  if(client)
  {
    scope = client->scope;
    ipc_router_unlock_client(client);
  }
  return scope;
}

static uint32 ipc_router_compute_checksum(void* cntl_msg, size_t len)
{
   uint32 sum =0;
   uint16 *msg = (uint16*)cntl_msg; 
   int i;
   // The sum of all 16 bit nibbles of the control message body is computed.
   for (i =0; i<len/2; i++)
   {
      sum += msg[i];
   }
   // The upper nibble of the computed sum is added to the lower nibble.
   while ( sum >>16 )
   {
      sum  = (sum & 0xFFFF) + (sum >>16);
   }
   // The lower 16 bits are inverted.
   sum = sum ^ 0xFFFF;

   //The upper 16 bits are cleared.
   sum &= 0xFFFF;
   return sum;
}

/*===========================================================================
FUNCTION      get_next_port

DESCRIPTION   Get the next available port to use

RETURN VALUE  Next port number
===========================================================================*/
static uint32 get_next_port(void)
{
  uint32 port;
  ipc_router_os_sem_lock(&port_semaphore);
  ipc_router_auto_port++;
  /* skip over port 0 */
  if(ipc_router_auto_port == 0)
    ipc_router_auto_port = 1;
  port = (uint32)ipc_router_auto_port;
  ipc_router_os_sem_unlock(&port_semaphore);
  
  return port;
}

/*===========================================================================
FUNCTION      ipc_router_core_send_hello

DESCRIPTION   Send hello message to xport

ARGUMENTS     xport - transport to send the message to

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h
===========================================================================*/
static int ipc_router_core_send_hello
(
  ipc_router_xport_type *xport
)
{
  ipc_router_control_msg_body      cntl_msg = {0};

  cntl_msg.command = IPC_ROUTER_CONTROL_HELLO;
  cntl_msg.arguments.hello_arg.checksum = IPC_ROUTER_HELLO_MAGIC;
  cntl_msg.arguments.hello_arg.versions_bitmask = IPC_ROUTER_VERSION_SUPPORTED_MASK;
  cntl_msg.arguments.hello_arg.capabilities_bitmask = 0;
  cntl_msg.arguments.hello_arg.checksum = ipc_router_compute_checksum(&cntl_msg, sizeof(cntl_msg));
  return ipc_router_send_cntl_msg(&cntl_msg, xport, NULL);
}

/*===========================================================================
FUNCTION      ipc_router_core_send_msg_local

DESCRIPTION   Send message to local client

ARGUMENTS     client    - pointer to local client
              msg       - pointer to message NOT including header
              header    - pointer to header

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  Client's Rx callback will be invoked, packet is not freed 
===========================================================================*/
static int ipc_router_core_send_msg_local
(
 ipc_router_client_type *client,
 ipc_router_packet_type *msg,
 ipc_router_header_type *header
)
{
  if(!client->xport && client->rx_cb) 
  {
    /* remove alignment padding */
    ipc_router_packet_trim(&msg, header->msg_size);
    client->rx_cb(client, client->rx_cb_data, msg, &header->src_addr, 
        header->msg_size, (boolean)header->confirm_rx); 
    return IPC_ROUTER_STATUS_SUCCESS;
  }

  return IPC_ROUTER_STATUS_FAILURE;
}

/*===========================================================================
FUNCTION      ipc_router_core_build_body

DESCRIPTION   Build packet from payload buffer

ARGUMENTS     buffer and len

RETURN VALUE  Packet pointer

SIDE EFFECTS  Padding will be added if required
===========================================================================*/
static ipc_router_packet_type *ipc_router_core_build_body
(
  unsigned char *payload,
  uint32 len
)
{
  uint32 pad_data = 0;
  uint32 pad_len = PAD_LEN(len);
  ipc_router_packet_type *pkt;

  pkt = ipc_router_packet_new(IPC_ROUTER_MAX_HEADER_SIZE, len + pad_len);
  if(!pkt)
  {
    return NULL;
  }

  if(ipc_router_packet_copy_payload(&pkt, payload, len) != len)
  {
    ipc_router_packet_free(&pkt);
    return NULL;
  }

  if(pad_len)
  {
    if(ipc_router_packet_copy_tail(&pkt, &pad_data, pad_len) != pad_len)
    {
      ipc_router_packet_free(&pkt);
      return NULL;
    }
  }
  return pkt;
}

/*===========================================================================
FUNCTION      ipc_router_send_to_xport

DESCRIPTION   Prepend header supported by xport and send

ARGUMENTS     xport, Header, pointer to payload

RETURN VALUE  Packet pointer

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_send_to_xport
(
  ipc_router_xport_type *xport,
  ipc_router_header_type *header,
  ipc_router_packet_type **msg
)
{
  int rc;

  header->version = xport->version;
  rc = ipc_router_protocol_inject_header(header, msg);
  if(rc == IPC_ROUTER_STATUS_SUCCESS)
  {
    rc = xport->ops->xport_write(xport->handle, *msg);
    if(rc == IPC_ROUTER_STATUS_SUCCESS)
    {
      ipc_router_log_packet(IPC_ROUTER_LOG_TX, xport, header);
    }
  }
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_xport_up

DESCRIPTION   This function brings up a transport and send list of servers
              to the remote router.

ARGUMENTS     

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_xport_up(ipc_router_xport_type *xport)
{
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;
  ipc_router_server_list_type *server_list;
  ipc_router_control_msg_body cntl_msg = {0};

  /* Mark transport as up now we've heard from the other router */
  xport->up = 1;

  /* If we haven't sent a hello message, send one as in the
     case of modem restart
   */
  if(!(xport->hello_sent)) {
    xport->hello_sent = 1;
    if(IPC_ROUTER_STATUS_SUCCESS != ipc_router_core_send_hello(xport))
    {
      xport->hello_sent = 0;
      return;
    }
  }

  /* Get a list of known servers */
  cr.crit = IPC_ROUTER_CRITERIA_ALL;
  ipc_router_list_servers(&cr, &results);

  /* if have any servers to advertise, send them one at a time */
  cntl_msg.command = IPC_ROUTER_CONTROL_NEW_SERVER;

  while(NULL != (server_list = LIST_HEAD(results.servers)))
  {
    ipc_router_client_type *client;
    /* Use the same control message we received, after updating the fields */
    cntl_msg.arguments.server_arg.addr = server_list->addr;
    cntl_msg.arguments.server_arg.service = server_list->service;

    client = ipc_router_find_client(&server_list->addr);

    if(client)
    {
      /* Servers on this processor, or servers on processors of a different net,
       * and if the priority matches our requirement */
      if((client->xport == NULL || client->xport->net_id != xport->net_id) &&
         (client->priority == 0 || (client->priority >= xport->priority)) &&
         IPC_ROUTER_POLICY_CAN_SEND(&client->scope, xport->remote_router.processor_id, &xport->xport_scope))
      {
        ipc_router_send_cntl_msg(&cntl_msg, xport, NULL);
      }
      ipc_router_unlock_client(client);
    }

    LIST_REMOVE(results.servers, server_list, link);
    ipc_router_os_free(server_list);
  }
}

/*===========================================================================
FUNCTION      ipc_router_xport_down

DESCRIPTION   This function removes the clients and servers associated with
              the xport.
ARGUMENTS     

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_xport_down(ipc_router_xport_type *xport)
{
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;
  ipc_router_client_list_type *client_list;
  ipc_router_proc_list_type *proc_list;

  if(xport)
  {
    xport->hello_sent = 0;
    xport->up = 0;
  }

  /* remove all clients and servers known over this transport */
  cr.crit = IPC_ROUTER_CRITERIA_XPORT;
  cr.arguments.xport_arg = xport;

  /* Get a list of all clients known over this transport */
  ipc_router_list_clients(&cr, &results);

  /* remove all clients known over this transport */
  while(NULL != (client_list = LIST_HEAD(results.clients)))
  {
    ipc_router_remove_client(&client_list->addr);
    LIST_REMOVE(results.clients, client_list, link);
    ipc_router_os_free(client_list);
  }

  /* remove all processors known over this transport */
  ipc_router_list_procs(&cr, &results);

  while(NULL != (proc_list = LIST_HEAD(results.procs)))
  {
    /* construct a remove proc message for other xports */
    ipc_router_unregister_proc(proc_list->processor_id);
    LIST_REMOVE(results.procs, proc_list, link);
    ipc_router_os_free(proc_list);
  }
}

/*===========================================================================
FUNCTION      ipc_router_remove_client

DESCRIPTION   This function removes the clients at a given address

ARGUMENTS     

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_remove_client(ipc_router_address_type *addr)
{
  ipc_router_client_type *client;
  ipc_router_control_msg_body cntl_msg = {0};
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;
  ipc_router_server_list_type *server_list;
  int ret;

  client = ipc_router_find_client(addr);
  if(!client)
  {
    return IPC_ROUTER_STATUS_INVALID_ENDPOINT;
  }

  /* construct a remove client message for other xports */
  cntl_msg.command = IPC_ROUTER_CONTROL_REMOVE_CLIENT;
  cntl_msg.arguments.client_arg = *addr;

  if(client->mode == IPC_ROUTER_EP_MODE_MULTI)
  {
    ipc_router_bcast_cntl_msg(&cntl_msg, 
        IPC_ROUTER_OTHER_XPORT_MASK(client->xport),
        client->priority, client->scope);
  }
  else if(client->mode == IPC_ROUTER_EP_MODE_SINGLE && 
          client->connected_xport != NULL)
  {
    if(IPC_ROUTER_STATUS_SUCCESS == ipc_router_lock_xport(client->connected_xport))
    {
      ipc_router_send_cntl_msg(&cntl_msg, client->connected_xport, NULL);
      ipc_router_unlock_xport(client->connected_xport);
    }
  }
  ipc_router_unlock_client(client);

  /* try to remove client */
  ret = ipc_router_unregister_client(addr);

  if(IPC_ROUTER_STATUS_SUCCESS != ret)
  {
    return ret;
  }

  /* Get a list of servers from this client */
  cr.crit = IPC_ROUTER_CRITERIA_ADDRESS;
  cr.arguments.addr_arg = addr;
  ipc_router_list_servers(&cr, &results);

  while(NULL != (server_list = LIST_HEAD(results.servers)))
  {
    ipc_router_unregister_server(&server_list->service, &server_list->addr);
    LIST_REMOVE(results.servers, server_list, link);
    ipc_router_os_free(server_list);
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_resume_tx

DESCRIPTION   This function removes the clients at a given address

ARGUMENTS     

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_resume_tx(ipc_router_address_type *addr)
{
  ipc_router_client_type *client;
  client = ipc_router_find_client(addr);
  if(!client)
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }
  ipc_router_resume_client_tx(client, addr);
  ipc_router_unlock_client(client);
  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_handle_reg_server

DESCRIPTION   This function handles new server messages/operations
        
ARGUMENTS     xport    - pointer to transport where the message was received
              addr     - address of the hosting port
              service  - name of the service

RETURN VALUE  None

SIDE EFFECTS  Messages can be (re-)broadcast to all permissible links
===========================================================================*/
static int ipc_router_handle_reg_server
(
  ipc_router_xport_type *xport,
  ipc_router_address_type *addr,
  ipc_router_service_type *service
)
{
  ipc_router_client_type *client;
  uint64 scope = IPC_ROUTER_DEFAULT_SCOPE;
  ipc_router_control_msg_body cntl_msg;
  int rc;

  if(!IPC_ROUTER_POLICY_GET_SCOPE(service->type, service->instance,
                                        addr->processor_id, &scope) ||
     (xport && !IPC_ROUTER_POLICY_CAN_RECV(NULL, ipc_router_local_processor_id, &scope)))
  {
    return IPC_ROUTER_STATUS_NOPERM;
  }

  rc = ipc_router_register_client_ext(addr, xport,
                          &client,0,0,0,NULL,NULL,NULL,NULL,
                          0, scope, IPC_ROUTER_EP_MODE_MULTI);
  if(rc != IPC_ROUTER_STATUS_SUCCESS && rc != IPC_ROUTER_STATUS_BUSY)
  {
    return rc;
  }
  client->scope &= scope;

  ipc_router_os_sem_lock(&client->mode_sem);
  client->mode = IPC_ROUTER_EP_MODE_MULTI;
  ipc_router_os_sem_unlock(&client->mode_sem);

  /* Prevent race condition for new link additions with new server
   * bcasts */
  ipc_router_os_sem_lock(&protocol_semaphore);
  rc = ipc_router_register_server(service, addr, xport);
  if(rc == IPC_ROUTER_STATUS_SUCCESS)
  {
    cntl_msg.command = IPC_ROUTER_CONTROL_NEW_SERVER;
    cntl_msg.arguments.server_arg.addr = *addr;
    cntl_msg.arguments.server_arg.service = *service;

    rc = ipc_router_bcast_cntl_msg(&cntl_msg,
        IPC_ROUTER_OTHER_XPORT_MASK(xport),
        client->priority, client->scope);
  }
  ipc_router_os_sem_unlock(&protocol_semaphore);
  ipc_router_unlock_client(client);
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_handle_control_msg

DESCRIPTION   This function handles router control messages
        
ARGUMENTS     cntl_msg - pointer to control message body
              xport    - pointer to transport where the message was received
              header   - pointer to header

RETURN VALUE  None

SIDE EFFECTS  Messages can be forwarded if income link type != outgoing link
===========================================================================*/
static void ipc_router_handle_control_msg
(
 ipc_router_control_msg_body *cntl_msg,
 ipc_router_xport_type *xport,
 ipc_router_header_type *rx_header 
)
{
  int                               ret;
  uint64                            scope = IPC_ROUTER_DEFAULT_SCOPE;
  switch(cntl_msg->command) {
    case IPC_ROUTER_CONTROL_NEW_SERVER:
      ret = ipc_router_handle_reg_server(xport, &cntl_msg->arguments.server_arg.addr,
                        &cntl_msg->arguments.server_arg.service);
      if(ret != IPC_ROUTER_STATUS_SUCCESS)
      {
        IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"register server %u:%u failure with %d\n",
                              cntl_msg->arguments.server_arg.service.type,
                              cntl_msg->arguments.server_arg.service.instance, ret);
      }
      break;

    case IPC_ROUTER_CONTROL_REMOVE_SERVER:
      /* remove the server from the servers table */
      scope = get_client_scope(&cntl_msg->arguments.server_arg.addr);
      if(IPC_ROUTER_STATUS_SUCCESS != 
          ipc_router_unregister_server(
          &(cntl_msg->arguments.server_arg.service), 
          &(cntl_msg->arguments.server_arg.addr))
        )
      {
        break;
      }
      ipc_router_bcast_cntl_msg(cntl_msg, 
          IPC_ROUTER_OTHER_XPORT_MASK(xport), 0, scope);
      break;

    case IPC_ROUTER_CONTROL_REMOVE_CLIENT:
      ipc_router_remove_client(&(cntl_msg->arguments.client_arg));
      break;

    case IPC_ROUTER_CONTROL_HELLO:
      /* Save the address of the router on the other end of the transport */
      xport->remote_router = rx_header->src_addr;
      // Check for version negotiation information.
      // check checksum == 0xe110
      if( IPC_ROUTER_HELLO_MAGIC == ipc_router_compute_checksum( cntl_msg, sizeof(ipc_router_control_msg_body) ) )
      {
         // checksum is valid, check the versions field to determine the highest supported version
         uint32 bitmask ;
         bitmask = (cntl_msg->arguments).hello_arg.versions_bitmask & IPC_ROUTER_VERSION_SUPPORTED_MASK;
         if( !bitmask )
         {
            // No common versions supported. Bailout!!
            IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"hello version bitmask %d\n",(cntl_msg->arguments).hello_arg.versions_bitmask);
            break;
         }
         // Needs generalization on adding subsequent versions
         if (bitmask & (1<<IPC_ROUTER_V3_VERSION))
         {
            xport->version = IPC_ROUTER_V3_VERSION;
         }
      }
      /* this step needs to be atomic with server registration */
      ipc_router_os_sem_lock(&protocol_semaphore);
      ipc_router_xport_up(xport);
      ipc_router_os_sem_unlock(&protocol_semaphore);
      break;

    case IPC_ROUTER_CONTROL_BYE:
      ipc_router_xport_down(xport);
      break;

    case IPC_ROUTER_CONTROL_RESUME_TX:
      ipc_router_resume_tx(&(cntl_msg->arguments.client_arg));
      break;

    default:
      break;
  }
}

/*===========================================================================
FUNCTION      ipc_router_send_cntl_msg

DESCRIPTION   Sends a router control message over a given transport.  

ARGUMENTS     cntl_msg (in)       - Buffer containing the control message to 
                                    be sent

              xport (in)         - the transport over which the 
                                    control message to be sent.

              dest_addr (in)      - address of the destination. If NULL the
                                    address of the other router on the other
                                    end of the xport is used
              
RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_send_cntl_msg
(
 ipc_router_control_msg_body   *cntl_msg,
 ipc_router_xport_type         *xport,
 ipc_router_address_type       *dest_addr
 )
{
  ipc_router_header_type tx_header; 
  ipc_router_packet_type *msg;
  int rc = IPC_ROUTER_STATUS_SUCCESS;

  /* hello message not received before a control message is sent - skip over
   * this transport
   */
  if(cntl_msg->command != IPC_ROUTER_CONTROL_HELLO && !xport->up)
  {
    return IPC_ROUTER_STATUS_SUCCESS;
  }

  /* Compose control message */
  BUILD_HEADER(
      tx_header,
      cntl_msg->command,
      ipc_router_local_processor_id,
      IPC_ROUTER_ROUTER_PORT,
      0,
      sizeof(ipc_router_control_msg_body),
      0, 0);

  if(dest_addr)
  {
    tx_header.dest_addr.processor_id = dest_addr->processor_id;
    tx_header.dest_addr.port_id    = IPC_ROUTER_ROUTER_PORT;
  }
  else
  {
    tx_header.dest_addr = xport->remote_router;
  }

  msg = ipc_router_core_build_body((unsigned char *)cntl_msg, sizeof(*cntl_msg));
  if(!msg)
  {
    rc = IPC_ROUTER_STATUS_NO_MEM;
  }
  else
  {
    rc = ipc_router_send_to_xport(xport, &tx_header, &msg);
    if(rc == IPC_ROUTER_STATUS_SUCCESS)
    {
      ipc_router_log_ctrl(IPC_ROUTER_LOG_TX, xport, cntl_msg);
    }
  }

  if(rc != IPC_ROUTER_STATUS_SUCCESS)
  {
    ipc_router_packet_free(&msg);
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
        "ipc_router_core: failed to write control message type=%u to %u\n",
        cntl_msg->command, tx_header.dest_addr.processor_id);
  }
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_bcast_cntl_msg

DESCRIPTION   Sends a router control message over a given transport.  

ARGUMENTS     cntl_msg (in)       - Buffer containing the control message to 
                                    be sent

              net_id_mask         - mask of the type of link to send msg to
              priority_pref       - priority preference
              
RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_bcast_cntl_msg
(
 ipc_router_control_msg_body *cntl_msg,
 uint32 net_id_mask,
 uint32 priority_pref,
 uint64 scope
 )
{
  ipc_router_xport_type *xport;
  ipc_router_xport_list_type *xport_list;
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;

  if(priority_pref == 0)
    priority_pref = IPC_ROUTER_XPORT_PRIORITY_MASK;

  /* Get a list of unique tranports */
  cr.crit = IPC_ROUTER_CRITERIA_ALL;
  ipc_router_list_xports(&cr, &results);

  while(NULL != (xport_list = LIST_HEAD(results.xports)))
  {
    xport = xport_list->xport;

    /* Try to lock the xport - it may have been unregistered and freed by
     * another thread, so the xport handle is invalid.
     */
    if(IPC_ROUTER_STATUS_SUCCESS == ipc_router_lock_xport(xport))
    {
      if((xport->net_id & net_id_mask) && 
          xport->priority <= priority_pref &&
          IPC_ROUTER_POLICY_CAN_SEND(&scope, 
          xport->remote_router.processor_id, &xport->xport_scope))
      {
        ipc_router_send_cntl_msg(cntl_msg, xport, NULL);
      }
      ipc_router_unlock_xport(xport);
    }
    LIST_REMOVE(results.xports, xport_list, link);
    ipc_router_os_free(xport_list);
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_forward_msg

DESCRIPTION   This function forwards messages (data & control)
        
ARGUMENTS     msg - message NOT including header
              xport - xport of where the message came from 
              rx_header - header to the message

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_forward_msg
(
 ipc_router_packet_type *msg,
 ipc_router_xport_type *xport,
 ipc_router_header_type *rx_header
)
{
  int ret = IPC_ROUTER_STATUS_FAILURE;
  ipc_router_xport_type  *dest_xport;

  if(rx_header->msg_type == IPC_ROUTER_CONTROL_DATA)
  {
    ipc_router_client_type *client;
    client = ipc_router_find_client(&rx_header->dest_addr);
    if(!client)
    {
      ret = IPC_ROUTER_STATUS_INVALID_ENDPOINT;
      goto forward_free_msg_bail;
    }

    dest_xport = client->xport;
    ipc_router_unlock_client(client);

    /* Find (and register if not present) the source endpoint.
       Adjust its mode if necessary */
    ret = ipc_router_register_client(&rx_header->src_addr, xport, &client, 0,
                                     0, 0, IPC_ROUTER_EP_MODE_NULL);
    if(ret != IPC_ROUTER_STATUS_SUCCESS && ret != IPC_ROUTER_STATUS_BUSY)
    {
      goto forward_free_msg_bail;
    }
    ADJUST_CLIENT_MODE_ON_SEND(client, dest_xport);
    ipc_router_unlock_client(client);
  }
  else
  {
    dest_xport = ipc_router_find_xport(rx_header->dest_addr.processor_id);
  }

  if(!dest_xport || 
      IPC_ROUTER_STATUS_SUCCESS != ipc_router_lock_xport(dest_xport))
  {
    ret = IPC_ROUTER_STATUS_INTERRUPTED;
    goto forward_free_msg_bail;
  }

  /* only forward if link types are different */
  if(dest_xport->net_id == xport->net_id) 
  {
    ret = IPC_ROUTER_STATUS_INVALID_PARAM;
    goto foward_unlock_xport_bail;
  }

  ret = ipc_router_send_to_xport(dest_xport, rx_header, &msg);

foward_unlock_xport_bail:
  ipc_router_unlock_xport(dest_xport);
forward_free_msg_bail:
  if(ret != IPC_ROUTER_STATUS_SUCCESS)
  {
    ipc_router_packet_free(&msg);
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
        "forward 0x%d->%d failed due to %d\n",
        rx_header->src_addr.processor_id, 
        rx_header->dest_addr.processor_id, ret);
  }
}

/*===========================================================================
FUNCTION      ipc_router_send_to_client

DESCRIPTION   Sends a packet to the destination client
        
ARGUMENTS     header - header of the message
              msg - message NOT including header
              src_client - source client
              dest_client - destination client

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success, 
              IPC_ROUTER_STATUS* on failure

SIDE EFFECTS  caller must free the packet on failure.
===========================================================================*/
static int ipc_router_send_to_client
(
  ipc_router_header_type *header,
  ipc_router_packet_type **pkt,
  ipc_router_client_type *src_client,
  ipc_router_client_type *dest_client
)
{
  int ret = IPC_ROUTER_STATUS_FAILURE;

  if(dest_client->xport)
  {
    if(IPC_ROUTER_STATUS_SUCCESS != ipc_router_lock_xport(dest_client->xport))
    {
      return IPC_ROUTER_STATUS_FAILURE;
    }

    if(!IPC_ROUTER_POLICY_CAN_SEND(&src_client->scope, 
          header->msg_type == IPC_ROUTER_CONTROL_MCAST_DATA ? 
          dest_client->xport->remote_router.processor_id : header->dest_addr.processor_id,
          &dest_client->xport->xport_scope))
    {
      ipc_router_unlock_xport(dest_client->xport);
      return IPC_ROUTER_STATUS_FAILURE;
    }

    ret = ipc_router_send_to_xport(dest_client->xport, header, pkt);
    if(ret == IPC_ROUTER_STATUS_SUCCESS)
    {
      ADJUST_CLIENT_MODE_ON_SEND(src_client, dest_client->xport);
    }
    ipc_router_unlock_xport(dest_client->xport);
  }
  else
  {
    ret = ipc_router_core_send_msg_local(dest_client, *pkt, header);
    ADJUST_CLIENT_MODE_ON_SEND(src_client, NULL);
  }

  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_handle_mcast_data_msg

DESCRIPTION   This function handles multi-cast data messages
        
ARGUMENTS     msg - message NOT including headers
              xport - xport of where the message came from 
              rx_header - header to the message

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_handle_mcast_data_msg
(
 ipc_router_packet_type *msg,
 ipc_router_xport_type *src_xport,
 ipc_router_header_type *rx_header
)
{
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;
  ipc_router_server_list_type *i;
  ipc_router_service_type service;
  LIST(ipc_router_client_handle_list_type, clients);
  ipc_router_client_handle_list_type *port;
  int ret = IPC_ROUTER_STATUS_FAILURE;
  ipc_router_client_type *src_client;

  ret = ipc_router_register_client(&rx_header->src_addr, src_xport, &src_client, 0,
                                    0, 0, IPC_ROUTER_EP_MODE_NULL);
  if(ret != IPC_ROUTER_STATUS_SUCCESS && ret != IPC_ROUTER_STATUS_BUSY)
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
                    "mcast: register client 0x%x:0x%x failed ret = %d!\n",
                    rx_header->src_addr.processor_id, rx_header->src_addr.port_id, ret);
    ipc_router_packet_free(&msg);
    return ret;
  }

  /* The dest addr is used to pass the type and instance as the actual
   * address is irrelevant */
  service.type = rx_header->dest_addr.processor_id;
  service.instance = rx_header->dest_addr.port_id;

  cr.crit = IPC_ROUTER_CRITERIA_SERVICE;
  cr.arguments.svc_arg.service = &service;
  cr.arguments.svc_arg.instance_mask = 0xffffffff;

  ipc_router_list_servers(&cr, &results);

  LIST_INIT(clients);

  while(NULL != (i = LIST_HEAD(results.servers)))
  {
    ipc_router_client_type *client = NULL;

    LIST_REMOVE(results.servers, i, link);

    do {
      /* Do not consider source */
      if(i->addr.processor_id == rx_header->src_addr.processor_id &&
         i->addr.port_id == rx_header->src_addr.port_id)
      {
        break;
      }

      client = ipc_router_find_client(&i->addr);
      if(!client)
      {
        break;
      }

      if(!IPC_ROUTER_POLICY_CAN_RECV(NULL, rx_header->src_addr.processor_id,
            &client->scope))
      {
        break;
      }

      if(client->xport)
      {
        /* Make checks on whether we can send on the xport or not */
        if(src_xport != NULL && client->xport->net_id == src_xport->net_id)
        {
          break;
        }

        /* Check if we already plan to send to this xport */
        LIST_FIND(clients, port, link, port->client->xport == client->xport);
        if(port)
        {
          break;
        }
      }

      /* Put this client in the to-send list */
      port = ipc_router_os_malloc(sizeof(*port));
      if(port)
      {
        LINK_INIT(port->link);
        port->client = client;
        LIST_ADD(clients, port, link);
        client = NULL; /* Unlocked when we send */
      }
    } while(0);

    if(client)
      ipc_router_unlock_client(client);
    ipc_router_os_free(i);
  }

  if(LIST_CNT(clients) == 0)
  {
    ipc_router_packet_free(&msg);
  }

  while(NULL != (port = LIST_HEAD(clients)))
  {
    ipc_router_client_type *client = port->client;
    ipc_router_packet_type *out_msg;

    LIST_REMOVE(clients, port, link);
    ipc_router_os_free(port);

    if(LIST_CNT(clients) == 0)
    {
      out_msg = msg;
    }
    else
    {
      out_msg = ipc_router_packet_dup(msg);
      if(!out_msg)
      {
        ipc_router_unlock_client(client);
        continue;
      }
    }
    if(IPC_ROUTER_STATUS_SUCCESS == ipc_router_send_to_client(rx_header, 
              &out_msg, src_client, client))
    {
      ret = IPC_ROUTER_STATUS_SUCCESS;
    }
    else
    {
      ipc_router_packet_free(&out_msg);
    }
    ipc_router_unlock_client(client);
  }

  ipc_router_unlock_client(src_client);
  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_handle_data_msg

DESCRIPTION   This function handles data messages
        
ARGUMENTS     msg - message NOT including header
              xport - xport of where the message came from 
              rx_header - header to the message

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_handle_data_msg
(
 ipc_router_packet_type *msg,
 ipc_router_xport_type *xport,
 ipc_router_header_type *rx_header 
)
{
  int ret;
  ipc_router_client_type *client;
  ipc_router_client_type *src_client;

  client = ipc_router_find_client(&(rx_header->dest_addr));
  if(!client)
  {
    ret = IPC_ROUTER_STATUS_INVALID_ENDPOINT;
    goto bail;
  }
  
  ret = ipc_router_register_client(&rx_header->src_addr, xport, &src_client,
                                   0, 0, 0, IPC_ROUTER_EP_MODE_NULL);
  if(ret != IPC_ROUTER_STATUS_SUCCESS && ret != IPC_ROUTER_STATUS_BUSY)
  {
    goto unlock_bail;
  }
  /* Receive a packet on the endpoint on the current 
   * processor (xport == NULL) */
  ADJUST_CLIENT_MODE_ON_SEND(src_client, NULL);

  ipc_router_unlock_client(src_client);

  if(!IPC_ROUTER_POLICY_CAN_RECV(NULL, 
           rx_header->src_addr.processor_id, &client->scope))
  {
    ret = IPC_ROUTER_STATUS_NOPERM;
    goto unlock_bail;
  }

  ret = ipc_router_core_send_msg_local(client, msg, rx_header);
unlock_bail:
  ipc_router_unlock_client(client);
bail:
  if(ret != IPC_ROUTER_STATUS_SUCCESS)
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
        "handle data from %d->L:%d failed with ret = %d\n",
        rx_header->src_addr.processor_id,
        rx_header->dest_addr.port_id, ret);
    ipc_router_packet_free(&msg);
  }
}

/*===========================================================================
                        EXPORTED XAL FUNCTIONS
===========================================================================*/
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
)
{
  ipc_router_xport_type *xport = (ipc_router_xport_type *)cb_handle;
  ipc_router_header_type rx_header; 
  uint32 pkt_len;

  if(ipc_router_protocol_extract_header(&msg, &rx_header) != IPC_ROUTER_STATUS_SUCCESS)
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
         "ipc_router_core: illegible message received from proc:%d\n",
          xport->remote_router.processor_id);
    ipc_router_packet_free(&msg);
    return; 
  }

  ipc_router_log_packet(IPC_ROUTER_LOG_RX, xport, &rx_header);

  pkt_len = ipc_router_packet_length(msg);

  /* Validate packet length else drop it. The packet cannot be smaller than the
   * expected size, and cannot be larger than expected size accounting
   * for any bytes added for padding */
  if(pkt_len < rx_header.msg_size || pkt_len > (rx_header.msg_size + 4))
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
             "ipc_router_core: Unexpected msg of size %d instead of %d received from proc:%d\n",
              pkt_len, rx_header.msg_size, xport->remote_router.processor_id);
    ipc_router_packet_free(&msg);
    return;
  }

  /* Check if the xport can receive this message */
  if((!IPC_ROUTER_POLICY_CAN_RECV(&xport->xport_scope, 
              rx_header.src_addr.processor_id, NULL)) ||
     (rx_header.src_addr.processor_id == ipc_router_local_processor_id))
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
             "ipc_router_core: Xport cannot receive from processor: %d\n",
              rx_header.src_addr.processor_id);
    ipc_router_packet_free(&msg);
    return;
  }

  if(rx_header.msg_type == IPC_ROUTER_CONTROL_MCAST_DATA)
  {
    (void)ipc_router_handle_mcast_data_msg(msg, xport, &rx_header);
  }
  else if(rx_header.msg_type != IPC_ROUTER_CONTROL_HELLO &&
     rx_header.dest_addr.processor_id != ipc_router_local_processor_id)
  {
    /* packet does not belong on this node, forward it */
    ipc_router_forward_msg(msg, xport, &rx_header);
  }
  else if(rx_header.msg_type == IPC_ROUTER_CONTROL_DATA)
  {
    /* process data packet */
    ipc_router_handle_data_msg(msg, xport, &rx_header);
  }
  else
  {
    ipc_router_control_msg_body cntl_data;

    /* process control message */
    if(ipc_router_packet_read_payload(&msg, &cntl_data, 
          sizeof(ipc_router_control_msg_body))
        == sizeof(ipc_router_control_msg_body))
    {
      ipc_router_log_ctrl(IPC_ROUTER_LOG_RX, xport, &cntl_data);
      ipc_router_handle_control_msg(&cntl_data, xport, &rx_header);
    }
    else
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
          "ipc_router_core: control message too short!\n");
    }
    ipc_router_packet_free(&msg);
  }
}

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
 )
{
  ipc_router_xport_type *xport = (ipc_router_xport_type *)cb_handle;

  /* Is the transport even up? */
  if(!xport->up)
  {
    return;
  }

  ipc_router_xport_down(xport);
}

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
)
{
  ipc_router_xport_type *xport = (ipc_router_xport_type *)cb_handle;
  if(xport->up)
  {
    return;
  }
  /* Send a hello in case the link comes back up. The
   * XAL should queue it up, if it cannot be sent
   */
  xport->hello_sent = 1;
  if(IPC_ROUTER_STATUS_SUCCESS != ipc_router_core_send_hello(xport))
  {
    xport->hello_sent = 0;
    return;
  }
}

/*===========================================================================
FUNCTION      ipc_router_xal_closed

DESCRIPTION   Signaling the transport has fully closed.

ARGUMENTS     cb_handle - handle to xport
===========================================================================*/
void ipc_router_xal_closed
(
 void *cb_handle
 )
{
  ipc_router_xport_type *xport = (ipc_router_xport_type *)cb_handle;
  ipc_router_free_xport(xport);
}

/*===========================================================================
FUNCTION      ipc_router_xal_start_xport

DESCRIPTION   Initializes and opens a transport.  

ARGUMENTS     ops - table of xport operations
              xport_params - parameters to the xport
              flags - xport specific flags
              desc - description of the network interface

              FLAGS
               31                            0
               |__TAG__|_VERS_|_PRIO_|_NET_ID_|
                   16     4      4       8

RETURN VALUE  Handle to xport on success

SIDE EFFECTS  None
===========================================================================*/
void *ipc_router_xal_start_xport
(
 ipc_router_xal_ops_type *ops,
 void *xport_params,
 uint32 flags,
 const char *desc
 )
{
  ipc_router_xport_type *xport;
  uint32 net_id = flags & IPC_ROUTER_XPORT_NET_ALL;
  uint8 priority = (flags >> 8) & IPC_ROUTER_XPORT_PRIORITY_MASK;
  uint32 version = (flags >> 12) & 0xf;
  uint32 tag = (uint32)((flags >> 16) & 0xffff);

  if(!ops)
    return NULL;

  xport = ipc_router_os_calloc(1, sizeof(ipc_router_xport_type)); 
  if(!xport)
  { 
    return NULL;
  }

  /* Configure the net_id of the link before we do anything to elminate race */
  if(net_id == 0)
    net_id = IPC_ROUTER_XPORT_NET_DEFAULT;

  /* If bundle ID is specified but priority is not, set it to medium */
  if(priority == 0)
    priority = IPC_ROUTER_XPORT_PRIORITY_MEDIUM;

  /* Default to IPC Router */
  if(version == 0)
    version = IPC_ROUTER_V1_VERSION;

  if(version < IPC_ROUTER_MIN_VERSION || version > IPC_ROUTER_MAX_VERSION)
  {
    ipc_router_os_free(xport);
    return NULL;
  }

  xport->desc = desc;
  xport->ops = ops;
  xport->net_id = net_id;
  xport->priority = priority;
  xport->xport_scope = IPC_ROUTER_POLICY_GET_LINK_SCOPE(tag);
  xport->version = version;

  /* default remote router to the broadcast address */
  xport->remote_router.processor_id = IPC_ROUTER_BROADCAST_ADDRESS;
  xport->remote_router.port_id = IPC_ROUTER_ROUTER_PORT;

  if(ipc_router_register_xport(xport) != IPC_ROUTER_STATUS_SUCCESS)
  {
    ipc_router_os_free(xport);
    return NULL;
  }

  /* Try to open and start the transport */
  if(IPC_ROUTER_STATUS_SUCCESS != xport->ops->xport_open(xport_params, xport, &xport->handle))
  {
    ipc_router_unregister_xport(xport);
    ipc_router_os_free(xport);
    return NULL;
  }
  return xport;
}

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
 )
{
  ipc_router_xport_type *xport = (ipc_router_xport_type *)handle;    
  ipc_router_control_msg_body     cntl_msg = {0};

  if(!xport)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  /* Send bye message to the other router */
  cntl_msg.command = IPC_ROUTER_CONTROL_BYE;
  ipc_router_send_cntl_msg(&cntl_msg, xport, NULL);

  /* Send bye message locally to handle the event */
  ipc_router_xport_down(xport);

  ipc_router_unregister_xport(xport);

  xport->ops->xport_close(xport->handle);

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_policy_register

DESCRIPTION   Register a policy manager's methods

ARGUMENTS     ops - Operation table of the external policy manager

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_policy_register(ipc_router_policy_ops_type *ops)
{
  if(ops)
  {
    ipc_router_policy = *ops;
  }
}


/*===========================================================================
                        EXPORTED FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      ipc_router_core_send_conf

DESCRIPTION   This function sends a confirmation message to a transport
        
ARGUMENTS     client - pointer to client record where the conf_msg originates
              addr   - pointer to the destination address

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_send_conf
(
 ipc_router_client_type *client,
 ipc_router_address_type *addr
)
{
  ipc_router_xport_type *xport;
  ipc_router_control_msg_body cntl_msg = {0};
  int ret;

  if(!client || !addr)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }
 
  cntl_msg.arguments.client_arg = client->addr;

  if(addr->processor_id == ipc_router_local_processor_id)
  {
    ret = ipc_router_resume_tx(&cntl_msg.arguments.client_arg);
  }
  else
  {
    cntl_msg.command = IPC_ROUTER_CONTROL_RESUME_TX;
    ret = IPC_ROUTER_STATUS_IO_ERROR;
    xport = ipc_router_find_xport(addr->processor_id);
    if(xport)
    {
      if(IPC_ROUTER_STATUS_SUCCESS == ipc_router_lock_xport(xport))
      {
        ret = ipc_router_send_cntl_msg(&cntl_msg, xport, addr);
        ipc_router_unlock_xport(xport);
      }
    }
  }

  return ret;
}


/*===========================================================================
FUNCTION      ipc_router_core_open_with_options

DESCRIPTION   Opens a handle to the router. This handle is to be use in all 
              subsequent operations on the IPC router for this port.

ARGUMENTS     handle (out)   - a handle returned to identify this client in 
                               later operations.

              port_id (in) - a locally unique identifier for the end-point.
                             If 0, a port is dynamically assigned (recommended)

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
)
{
  ipc_router_client_type *client = NULL;
  ipc_router_address_type ClientAddr;
  uint64 scope = IPC_ROUTER_DEFAULT_SCOPE;
  uint32 priority = IPC_ROUTER_PORT_PRIORITY_DEFAULT;
  int to_return = IPC_ROUTER_STATUS_UNKNOWN;
  int i;

  if(!handle || port_id >= IPC_ROUTER_RESERVED_PORTS_START || !rx_cb || 
      !event_cb)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  *handle = NULL;

  if(options)
  {
    if(options->options_set & IPC_ROUTER_CORE_PORT_OPTIONS_PRIORITY_VALID)
    {
      if(options->priority <= IPC_ROUTER_PORT_PRIORITY_HIGH)
      {
        priority = options->priority;
      }
      else
      {
        IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_WARNING,
            "Invalid priority option: %d using default value\n",
            options->priority);
      }
    }

    if(options->options_set & IPC_ROUTER_CORE_PORT_OPTIONS_SCOPE_VALID)
    {
      scope = options->scope;
    }
  }

  ClientAddr.processor_id = ipc_router_local_processor_id;
  ClientAddr.port_id = port_id ?  port_id : get_next_port(); 

  priority = priority ? priority : IPC_ROUTER_PORT_PRIORITY_MEDIUM;

  for(i = 0; i < IPC_ROUTER_MAX_TRIES; i++)
  {
    to_return = ipc_router_register_client_ext(
                    &ClientAddr, 
                    NULL, 
                    &client,
                    0,
                    0,
                    0,
                    rx_cb,
                    rx_cb_data,
                    event_cb,
                    event_cb_data, 
                    priority,
                    scope,
                    IPC_ROUTER_EP_MODE_NULL);
    if(to_return != IPC_ROUTER_STATUS_BUSY || port_id != 0)
    {
      break;
    }
    ipc_router_unlock_client(client);
    ClientAddr.port_id = get_next_port();
  }

  /* client's ref count must be released if we got
   * success or busy. */
  switch(to_return)
  {
    case IPC_ROUTER_STATUS_SUCCESS:
      *handle = client;
      /* Fall through */
    case IPC_ROUTER_STATUS_BUSY:
      ipc_router_unlock_client(client);
      break;
  }
  return to_return;
}

/*===========================================================================
FUNCTION      ipc_router_core_open

DESCRIPTION   Opens a handle to the router. This handle is to be use in all 
              subsequent operations on the IPC router for this port.

ARGUMENTS     handle (out)   - a handle returned to identify this client in 
                               later operations.

              port_id (in) - a locally unique identifier for the end-point.
                             If 0, a port is dynamically assigned (recommended)

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
)
{
  return ipc_router_core_open_with_options(handle, port_id, rx_cb, rx_cb_data, 
      event_cb, event_cb_data, NULL);
}

/*===========================================================================
FUNCTION      ipc_router_core_send_internal

DESCRIPTION   Send buffer or packet to destination
              

ARGUMENTS     client (in)       - client handle returned from ipc_router_open
              dest (in)         - pointer to destination address
              buf (in)          - pointer to the data to be written
              len (in)          - lenth of the message to send
              msg (in)          - pointer to packet

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success
              IPC_ROUTER_STATUS_BUSY if Tx quota exceeded. Wait for event cb.
              Other failures

SIDE EFFECTS  
===========================================================================*/
static int ipc_router_core_send_internal
(
 ipc_router_client_type *client,
 ipc_router_address_type *dest,
 const unsigned char *buf,
 uint32 len,
 ipc_router_packet_type *msg
)
{
  ipc_router_header_type header;
  ipc_router_client_type *dest_client = NULL;
  ipc_router_write_token_type token;
  ipc_router_packet_type *send_msg = NULL;
  int ret = IPC_ROUTER_STATUS_IO_ERROR;
   
  if(!client || (!buf && !msg)) 
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  if(msg)
    len = ipc_router_payload_length(msg);

  /* Find client record */
  dest_client = ipc_router_find_client(dest);

  /* The client record is now locked so it doesn't get freed/modified while
   * we are using it.
   */

  if(!dest_client)
  {
    return IPC_ROUTER_STATUS_INVALID_ENDPOINT;
  }

  if(!msg)
  {
    send_msg = ipc_router_core_build_body((unsigned char *)buf, len);
    if(!send_msg)
    {
      ret = IPC_ROUTER_STATUS_NO_MEM;
      goto ipc_router_send_bail;
    }
  }
  else
  {
    send_msg = msg;
  }

  /* Get write token. If none avail, an event callback will be queued */
  token = ipc_router_get_write_token(dest_client, client, len);
  
  if(token == IPC_ROUTER_WRITE_TOKEN_NONE)
  {
    ret = IPC_ROUTER_STATUS_BUSY;
    goto ipc_router_send_bail;
  }

  BUILD_HEADER(
      header, 
      IPC_ROUTER_CONTROL_DATA, 
      client->addr.processor_id,
      client->addr.port_id, 
      (token == IPC_ROUTER_WRITE_TOKEN_LAST), 
      len, 
      dest->processor_id,
      dest->port_id);

  ret = ipc_router_send_to_client(&header, &send_msg, client, dest_client);
ipc_router_send_bail:
  ipc_router_unlock_client(dest_client);
  if(ret != IPC_ROUTER_STATUS_SUCCESS && send_msg != msg)
    ipc_router_packet_free(&send_msg);

  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_core_send

DESCRIPTION   Send message to destination 
             

ARGUMENTS     client (in)       - client handle returned from ipc_router_open
              dest (in)         - pointer to destination address
              buf (in)          - pointer to the data to be written
              len (in)          - lenth of the message to send

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
)
{
  return ipc_router_core_send_internal(client, dest, buf, len, NULL);
}

/*===========================================================================
FUNCTION      ipc_router_core_get_packet

DESCRIPTION   Get packet for transmission. 

ARGUMENTS     

RETURN VALUE  Pointer to packet or NULL on failure

SIDE EFFECTS  Packet is allocated with proper headroom reserved
===========================================================================*/
ipc_router_packet_type *ipc_router_core_get_packet(void)
{
  return ipc_router_packet_new(sizeof(ipc_router_header_type), 0);
}

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
)
{
  return ipc_router_core_send_internal(client, dest, NULL, 0, msg);
}

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
)
{
  ipc_router_packet_type *pkt = NULL;
  ipc_router_header_type header;

  if(!client || !buf || !service || !len)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  BUILD_HEADER(
      header,
      IPC_ROUTER_CONTROL_MCAST_DATA,
      client->addr.processor_id,
      client->addr.port_id,
      0,
      len,
      service->type,
      service->instance);

  pkt = ipc_router_core_build_body((unsigned char *)buf, len);
  if(!pkt)
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }

  return ipc_router_handle_mcast_data_msg(pkt, NULL, &header);
}

/*===========================================================================
FUNCTION      ipc_router_reg_server

DESCRIPTION   Publish server

ARGUMENTS     client - client handle
              service - type and instance of the server

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  Clients' server event callbacks will be invoked
===========================================================================*/
int ipc_router_core_reg_server
(
 ipc_router_client_type *client,
 ipc_router_service_type *service
)
{
  if(!client || !service)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  return ipc_router_handle_reg_server(NULL, &client->addr, service);
}

/*===========================================================================
FUNCTION      ipc_router_core_dereg_server

DESCRIPTION   Withdraw server publication

ARGUMENTS     client - client handle
              service - type and instance of the server

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  Clients' server event callbacks will be invoked
===========================================================================*/
int ipc_router_core_dereg_server
(
 ipc_router_client_type *client,
 ipc_router_service_type *service
)
{
  ipc_router_control_msg_body cntl_msg = {0};
  int ret;

  if(!client || !service)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  cntl_msg.command = IPC_ROUTER_CONTROL_REMOVE_SERVER;
  cntl_msg.arguments.server_arg.addr = client->addr;
  cntl_msg.arguments.server_arg.service.type = service->type;
  cntl_msg.arguments.server_arg.service.instance  = service->instance;

  ret = ipc_router_unregister_server(
      &(cntl_msg.arguments.server_arg.service),
      &(cntl_msg.arguments.server_arg.addr));
  if(ret != IPC_ROUTER_STATUS_SUCCESS)
    return ret;

  /* broadcast message over all transports */
  ret = ipc_router_bcast_cntl_msg(&cntl_msg, IPC_ROUTER_XPORT_NET_ALL, 0, client->scope);
  if(ret != IPC_ROUTER_STATUS_SUCCESS)
    return ret;

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_core_find_all_servers

DESCRIPTION   finds all the servers satisfying the type and specific bits
              (provided by the instance_mask) of the instance.

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
)
{
  ipc_router_search_criteria_type  cr;
  ipc_router_search_results_type   results;
  ipc_router_server_list_type     *server_list;
  uint32 to_fill = 0, filled = 0;
  int rc;

  if(!service || !num_servers)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  if(num_entries && servers)
  {
    to_fill = *num_entries;
  }

  /* Get a list of servers from this client */
  cr.crit = IPC_ROUTER_CRITERIA_SERVICE;
  cr.arguments.svc_arg.service = service;
  cr.arguments.svc_arg.instance_mask = instance_mask;

  ipc_router_list_servers(&cr, &results);
  if(num_servers)
  {
    *num_servers = LIST_CNT(results.servers);
  }

  rc = LIST_CNT(results.servers) > 0 ? IPC_ROUTER_STATUS_SUCCESS : IPC_ROUTER_STATUS_FAILURE;

  while(NULL != (server_list = LIST_HEAD(results.servers)))
  {
    if(filled < to_fill)
    {
      /* fill output array */
      servers[filled].service = server_list->service;
      servers[filled].addr = server_list->addr;
      filled++;
    }

    LIST_REMOVE(results.servers, server_list, link);
    ipc_router_os_free(server_list);
  }
  
  if(num_entries)
  {
    *num_entries = filled;
  }

  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_core_close

DESCRIPTION   Closes an opened handle to the router.  

ARGUMENTS     handle (in) - client handle returned from ipc_router_open

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  When close get's called, it's assumed that no client threads
              will be running in the router core
===========================================================================*/
int ipc_router_core_close
(
 ipc_router_client_type *client
)
{
  ipc_router_address_type addr;

  if(!client)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  addr = client->addr;

  return ipc_router_remove_client(&addr);
}

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
 )
{
  ipc_router_local_processor_id = processor_id;

  /* Initialize logging */
  ipc_router_log_init();

  ipc_router_packet_init();
  ipc_router_os_sem_init(&port_semaphore);
  ipc_router_os_sem_init(&protocol_semaphore);

  if(IPC_ROUTER_STATUS_SUCCESS != ipc_router_database_init()){
    /* Failed to initialize router database error message */
    return IPC_ROUTER_STATUS_UNKNOWN;
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_core_deinit

DESCRIPTION   deintializes the IPC router.  

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_core_deinit
(
 void
)
{   
  ipc_router_xport_list_type *xport_list;
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;

  /* Get a list of currently open tranports */
  cr.crit = IPC_ROUTER_CRITERIA_ALL;
  ipc_router_list_xports(&cr, &results);

  while(NULL != (xport_list = LIST_HEAD(results.xports)))
  {
    ipc_router_xal_stop_xport(xport_list->xport);
    LIST_REMOVE(results.xports, xport_list, link);
    ipc_router_os_free(xport_list);
  }

  /* Cleanup local ports/servers */
  ipc_router_xport_down(NULL);

  ipc_router_database_deinit();

  ipc_router_os_sem_deinit(&port_semaphore);
  ipc_router_os_sem_deinit(&protocol_semaphore);
}
