/*===========================================================================

                    I P C    R O U T E R   Q D I

   This file provides the stubs to access the IPC Router QDI Driver

  ---------------------------------------------------------------------------
  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/ipc_router/src/ipc_router_qdi_user.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/12   aep     Initial Creation
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include <stddef.h>
#include "ipc_router_qdi_driver.h"
#include "qurt_qdi_driver.h"
#include "ipc_router_core.h"
#include "stdint.h"
#include "timer.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/
#ifndef DLL_API_GLOBAL
#define DLL_API_GLOBAL
#endif

#define WORKER_STACK_SIZE (2048 * 2)

typedef struct ipc_router_qdi_struct ipc_router_qdi_handle;

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
struct ipc_router_qdi_struct {
  uint32                       handle;
  uint32                       port_id;
  ipc_router_rx_cb_type        rx_cb;
  void                         *rx_cb_data;
  ipc_router_event_cb_type     event_cb;
  void                         *event_cb_data;
  /* TODO: Priority not supported at the moment */
};

/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/

/* NOTE: The fact that these are globals means that we currently 
 * can support only 1 rdr thread touple per process. Thus can probably
 * be changed in the future and make the users get a handle when
 * they call init (Obviously then the APIs change to take in 1 extra
 * handle) */

static volatile int ipc_router_qdi_device = -1;
static int ipc_router_qdi_group_sig = -1;
static int ipc_router_qdi_data_sig = -1;
static int ipc_router_qdi_event_sig = -1;

static qurt_thread_t worker_tid;

static qurt_thread_attr_t worker_tattr;

static uint8_t worker_stack[WORKER_STACK_SIZE];

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      ipc_router_qdi_handle_event

DESCRIPTION   Handles a event signal

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_handle_event(void)
{
  int rc;
  ipc_router_event_info_type info;
  uint32 event;
  void *cookie;
  ipc_router_qdi_handle *handle;
  
  while(1)
  {
    rc = qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_EVENT_READ,
                                 &event, &info, &cookie);
    if(rc <= 0)
    {
      break;
    }
    handle = (ipc_router_qdi_handle *)cookie;
    if(handle && handle->event_cb)
    {
      handle->event_cb((ipc_router_client_type *)handle, 
                          handle->event_cb_data, event, &info);
    }

    /* Now it is safe to free up the handle */
    if(event == IPC_ROUTER_EVENT_CLOSED)
    {
      ipc_router_os_free(handle);
    }
  }
}

/*===========================================================================
FUNCTION      ipc_router_qdi_handle_data

DESCRIPTION   Handles a data signal

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_handle_data(void)
{
  int rc;
  void *cookie;
  void *buf = NULL;
  uint32 len = 0;
  ipc_router_qdi_handle *handle;
  ipc_router_address_type src_addr;
  unsigned int confirm_rx;

  while(1)
  {
    rc = qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_READ,
                      NULL, 0, NULL, NULL, &len, NULL,
                      IPC_ROUTER_QDI_RDFLAG_DONTWAIT | 
                      IPC_ROUTER_QDI_RDFLAG_MSGPEEK);
    if(rc != 0 || len == 0)
    {
      break;
    }
    buf = ipc_router_os_malloc(len);
    if(!buf)
    {
      /* TODO: Log error */
      break;
    }

    rc = qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_READ,
                      buf, len, &src_addr, &confirm_rx, &len, &cookie, 
                      IPC_ROUTER_QDI_RDFLAG_DONTWAIT);
    if(rc != 0)
    {
      ipc_router_os_free(buf);
      /* The head has changed, peek again */
      if(rc == IPC_ROUTER_QDI_NO_SPACE_ERR)
      {
        continue;
      }
      break;
    }

    handle = (ipc_router_qdi_handle *)cookie;

    if(handle && handle->rx_cb)
    {
      handle->rx_cb((ipc_router_client_type *)handle, handle->rx_cb_data, 
                     (ipc_router_packet_type *)buf, &src_addr, len, 
                     (boolean)confirm_rx);
      /* buf is freed by the rx to mimic exactly what ipc_router does */
    }
    else
    {
      /* TODO: Log error */
      ipc_router_os_free(buf);
    }
  }
}

/*===========================================================================
FUNCTION      ipc_router_qdi_worker

DESCRIPTION   IPC Router worker thread

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_worker(void *unused)
{
  while(ipc_router_qdi_device >= 0)
  {
    qurt_qdi_signal_group_wait(ipc_router_qdi_group_sig);

    if(ipc_router_qdi_device >= 0 && qurt_qdi_signal_poll(ipc_router_qdi_event_sig))
    {
      qurt_qdi_signal_clear(ipc_router_qdi_event_sig);
      ipc_router_qdi_handle_event();
    }

    if(ipc_router_qdi_device >= 0 && qurt_qdi_signal_poll(ipc_router_qdi_data_sig))
    {
      qurt_qdi_signal_clear(ipc_router_qdi_data_sig);
      ipc_router_qdi_handle_data();
    }
  }
  /* TEMP: Currently MP QuRT cannot handle thread exits */
  while(1)
    timer_sleep(1000, T_SEC, FALSE);
  /* TODO: Log exiting from reader thread */
}

/*===========================================================================
                        PUBLIC FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_qdi_init

DESCRIPTION   Initialize the client framework.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  A reader thread will be started. This function may sleep
              till a successful open on the ipc_router device is performed.
===========================================================================*/
DLL_API_GLOBAL void ipc_router_qdi_init(void)
{
  int rc = ipc_router_qdi_device;

  while(rc < 0)
  {
    rc = qurt_qdi_open(IPC_ROUTER_QDI_DEV_NAME,
        &ipc_router_qdi_group_sig, &ipc_router_qdi_event_sig, &ipc_router_qdi_data_sig);
    if(rc < 0)
    {
      timer_sleep(10, T_SEC, FALSE);
    }
  }
  ipc_router_qdi_device = rc;

  qurt_thread_attr_init(&worker_tattr);
  qurt_thread_attr_set_name(&worker_tattr, "IPCRTR_RDR");
  qurt_thread_attr_set_priority(&worker_tattr, 204);
  qurt_thread_attr_set_stack_size(&worker_tattr, WORKER_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&worker_tattr, (void *)worker_stack);
  qurt_thread_create(&worker_tid, &worker_tattr, ipc_router_qdi_worker, NULL);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_deinit

DESCRIPTION   De-initialize the client framework.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  This function will sleep/block till the reader thread is
              fully torn down.
===========================================================================*/
void ipc_router_qdi_deinit(void)
{
  int handle = ipc_router_qdi_device;
  int status;

  ipc_router_qdi_device = -1;

  if(handle >= 0)
  {
    qurt_qdi_close(handle);
    qurt_thread_join(worker_tid, &status);
  }
}

/*===========================================================================
FUNCTION      ipc_router_packet_free

DESCRIPTION   We just hand out the pointer, so free the pointer

ARGUMENTS     pkt - the packet handed in the rx_cb callback

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_packet_free(ipc_router_packet_type **pkt)
{
  if(pkt)
  {
    void *buf = (void *)*pkt;
    ipc_router_os_free(buf);
    *pkt = NULL;
  }
}

/*===========================================================================
FUNCTION      ipc_router_packet_peek

DESCRIPTION   We just hand out the pointer to the contiguous memory so just
              return the pointer

ARGUMENTS     pkt - packet handed by rx_cb callback,
              len - the contiguous bytes required

RETURN VALUE  pointer to the packet payload

SIDE EFFECTS  None
===========================================================================*/
void *ipc_router_packet_peek(ipc_router_packet_type *pkt, uint32 len)
{
  return (void *)pkt;
}

/*===========================================================================
FUNCTION      ipc_router_packet_read

DESCRIPTION   We just hand out the pointer, so read from the pointer to the next.

ARGUMENTS     pkt - the packet handed in rx_cb
              buf - the buffer to copy the data to
              len - the bytes to copy.

RETURN VALUE  bytes copied

SIDE EFFECTS  None

LIMITATIONS   Multiple reads are not possible. The user is expected to read 
              the entire packet at once or none at all.
===========================================================================*/
uint32 ipc_router_packet_read(ipc_router_packet_type **pkt, void *buf, uint32 len)
{
  if(!pkt || !buf || !len)
  {
    return 0;
  }
  ipc_router_os_mem_copy(buf, len, (void *)*pkt, len);
  return len;
}

/*===========================================================================
FUNCTION      ipc_router_core_open_with_options

DESCRIPTION   Open an endpoint (ep) on the router.

ARGUMENTS     out_handle - Return handle if the open is successful.
              port_id - Optional port ID.
              rx_cb - Callback to be called on incoming packets in this ep
              rx_cb_data - Cookie to be used on the RX callback.
              event_cb - Callback to be called on incoming events on this ep
              event_cb_data - Cookie to be used on the Event callback.
              options - options

RETURN VALUE  0 on success, error code on failure

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_open_with_options
(
  ipc_router_client_type **out_handle,
  uint32 port_id,
  ipc_router_rx_cb_type rx_cb,
  void *rx_cb_data,
  ipc_router_event_cb_type event_cb,
  void *event_cb_data,
  ipc_router_core_options_type *options
)
{
  ipc_router_qdi_handle *handle;
  ipc_router_qdi_port_options port_options = {0};
  int rc;

  if(!out_handle || !rx_cb || !event_cb)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  if(ipc_router_qdi_device < 0)
  {
    return IPC_ROUTER_STATUS_IO_ERROR;
  }

  handle = ipc_router_os_malloc(sizeof(*handle));
  if(!handle)
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }
  handle->rx_cb         = rx_cb;
  handle->rx_cb_data    = rx_cb_data;
  handle->event_cb      = event_cb;
  handle->event_cb_data = event_cb_data;
  handle->port_id       = port_id;
  handle->handle        = -1;

  rc = qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_OPEN, 
                              &handle->handle, port_id, options, handle);
  if(rc)
  {
    ipc_router_os_free(handle);
    return rc;
  }

  port_options.allowed_valid = 1;
  port_options.allowed_flag = DATA_ALLOWED | EVENT_ALLOWED | TRANSMIT_ALLOWED;

  rc = qurt_qdi_handle_invoke(ipc_router_qdi_device, 
      IPC_ROUTER_QDI_SET_PORT_OPTIONS, handle->handle, &port_options);

  *out_handle = (ipc_router_client_type *)handle;

  return 0;
}

/*===========================================================================
FUNCTION      ipc_router_core_open

DESCRIPTION   Open an endpoint (ep) on the router.

ARGUMENTS     out_handle - Return handle if the open is successful.
              port_id - Optional port ID.
              rx_cb - Callback to be called on incoming packets in this ep
              rx_cb_data - Cookie to be used on the RX callback.
              event_cb - Callback to be called on incoming events on this ep
              event_cb_data - Cookie to be used on the Event callback.
              options - options

RETURN VALUE  0 on success, error code on failure

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_open
(
  ipc_router_client_type **out_handle,
  uint32 port_id,
  ipc_router_rx_cb_type rx_cb,
  void *rx_cb_data,
  ipc_router_event_cb_type event_cb,
  void *event_cb_data
)
{
  return ipc_router_core_open_with_options(out_handle, port_id, rx_cb, rx_cb_data,
                                            event_cb, event_cb_data, NULL);
}

/*===========================================================================
FUNCTION      ipc_router_core_close

DESCRIPTION   Close a previously opened endpoint.

ARGUMENTS     handle - Handle provided for this endpoint.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_close
(
  ipc_router_client_type *client
)
{
  ipc_router_qdi_handle *handle = (ipc_router_qdi_handle *)client;

  if(ipc_router_qdi_device < 0 || !handle)
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }

  /* Memory freed when we get a CLOSED event */
  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_CLOSE, handle->handle);
}

/*===========================================================================
FUNCTION      ipc_router_core_send

DESCRIPTION   Send a packet to a remote endpoint.

ARGUMENTS     handle - handle for this endpoint.
              dest_addr - address for the remote endpoint.
              buf - the buffer to send.
              len - the length of the buffer.

RETURN VALUE  0 on success, error code on failure. 
              If IPC_ROUTER_STATUS_BUSY is returned, then the caller
              must wait till a resume_tx event is received for this EP.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_send
(
  ipc_router_client_type *client,
  ipc_router_address_type *dest_addr,
  const unsigned char *buf,
  uint32 len
)
{
  ipc_router_qdi_handle *handle = (ipc_router_qdi_handle *)client;

  if(!handle || !dest_addr || !buf || !len)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }
  
  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_SEND,
      handle->handle, dest_addr, buf, len, IPC_ROUTER_QDI_WRFLAG_DONTWAIT);
}

/*===========================================================================
FUNCTION      ipc_router_core_send_to_name

DESCRIPTION   Multicast packet to a name.

ARGUMENTS     handle - handle for this endpoint.
              service - name of the dest services
              buf - the buffer to send.
              len - the length of the buffer.

RETURN VALUE  0 on success, error code on failure. 
              If IPC_ROUTER_STATUS_BUSY is returned, then the caller
              must wait till a resume_tx event is received for this EP.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_send_to_name
(
  ipc_router_client_type *client,
  ipc_router_service_type *service,
  const unsigned char *buf,
  uint32 len
)
{
  ipc_router_qdi_handle *handle = (ipc_router_qdi_handle *)client;

  if(!handle || !service || !buf || !len)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }
  
  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_SEND_TO_NAME,
      handle->handle, service, buf, len);
}

/*===========================================================================
FUNCTION      ipc_router_core_send_conf

DESCRIPTION   Sends a confirm TX control packet to a destination EP.

ARGUMENTS     handle - The handle for this endpoint.
              dest_addr - The address of the destination endpoint.

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_send_conf
(
  ipc_router_client_type *client,
  ipc_router_address_type *dest_addr
)
{
  ipc_router_qdi_handle *handle = (ipc_router_qdi_handle *)client;
  if(!handle || !dest_addr)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }
  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_SEND_CONF,
      handle->handle, dest_addr);
}

/*===========================================================================
FUNCTION      ipc_router_core_reg_server

DESCRIPTION   Register a service on this endpoint.

ARGUMENTS     handle - handle for this endpoint.
              service - The service description.

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_reg_server
(
 ipc_router_client_type *client,
 ipc_router_service_type *service
)
{
  ipc_router_qdi_handle *handle = (ipc_router_qdi_handle *)client;
  if(!handle || !service)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }
  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_REGISTER_SERVER,
      handle->handle, service);
}

/*===========================================================================
FUNCTION      ipc_router_core_dereg_server

DESCRIPTION   De-register a previously registered service on this EP.

ARGUMENTS     handle - handle for this EP.
              service - the service description

RETURN VALUE  0 on success, error code on failure

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_dereg_server
(
 ipc_router_client_type *client,
 ipc_router_service_type *service
)
{
  ipc_router_qdi_handle *handle = (ipc_router_qdi_handle *)client;
  if(!handle || !service)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }
  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_UNREGISTER_SERVER,
      handle->handle, service);
}

/*===========================================================================
FUNCTION      ipc_router_core_find_all_servers

DESCRIPTION   Query the router for services matching the required description

ARGUMENTS     handle - the handle for this EP.
              service - the service description to find.
              servers - (Optional) The array to place the found services.
              num_entries - (Optional) In: The number of services 'servers' can hold
                                       Out: The number of servers populated in 'servers'
              num_servers - Total number of servers which exist.
              mask - The mask to apply on the instance ID of the service before
                     matching. A mask of 0xffffffff forces the router to find
                     services with the same instance ID.

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_find_all_servers
(
  ipc_router_client_type *client,
  ipc_router_service_type *service,
  ipc_router_server_type *servers,
  uint32 *num_entries,
  uint32 *num_servers,
  uint32 mask
)
{
  int req_handle = -1;
  ipc_router_qdi_handle *handle = (ipc_router_qdi_handle *)client;

  if(!num_servers || !mask)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  /* We support calls to this API without providing a handle, so 
   * dereference only if possible. The driver can handle
   * without this (only for this API) */

  if(handle)
  {
    /* Default to invalid handle. */
    req_handle = handle->handle;
  }

  if((num_entries && !servers) || (!num_entries && servers))
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_FIND_ALL_SERVERS,
      req_handle, service, servers, num_entries, num_servers, mask);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_log

DESCRIPTION   Write a log to the driver

ARGUMENTS     desc - The log description.
              d1 - d6 - Log data

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_qdi_log
(
  uint32 desc,
  uint32 d1,
  uint32 d2,
  uint32 d3,
  uint32 d4,
  uint32 d5,
  uint32 d6
)
{
  if(ipc_router_qdi_device >= 0)
  {
    (void)qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_LOG,
                                 desc, d1, d2, d3, d4, d5, d6);
  }
}


