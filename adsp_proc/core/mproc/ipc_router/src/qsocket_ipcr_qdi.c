/*===========================================================================

                 Q S O C K E T   I P C R   Q D I

   This file provides the qsocket stubs to access the IPC Router QDI Driver

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/ipc_router/src/qsocket_ipcr_qdi.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/14   aep     Initial Creation
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include <stddef.h>
#include "qsocket_ipcr.h"
#include "ipc_router_qdi_driver.h"
#include "qurt_qdi_driver.h"
#include "ipc_router_core.h"
#include "stdint.h"
#include "timer.h"

/*===========================================================================
                      MACROS AND DEFINES
===========================================================================*/
typedef struct qsockaddr_ipcr ipcr_addr_t;

/* Each FD opened by the user to IPC Router's QDI driver, may have
 * multiple addressable handles (Port on IPC Router). Even though it
 * is allowed, Qsockets will open on one handle per FD. Thus, to simplify
 * (use the FD as the handle handed out to the user on socket()),
 * we take advantage of the knowledge that the first handle will always
 * be 0x1. */
#define IPC_ROUTER_QDI_EXP_HANDLE 0x1

#define IPCR_ADDR_PORT_SUPP   0x1
#define IPCR_ADDR_NAME_SUPP   0x2
#define IPCR_ADDR_SNAME_SUPP  0x4

#ifdef DLL_API_GLOBAL
#define QSOCKET_EXPORT DLL_API_GLOBAL
#else
#define QSOCKET_EXPORT
#endif

/*===========================================================================
                      PRIVATE METHODS
===========================================================================*/
static int validate_addr(ipcr_addr_t *addr, qsocklen_t addrlen,
                         unsigned int supported_addrtypes, 
                         int null_supported)
{
  if(addr)
  {
    if(addrlen != sizeof(*addr))
    {
      return QEINVAL;
    }
    if(addr->sa_family != AF_IPC_ROUTER)
    {
      return QEAFNOSUPPORT;
    }

    switch(addr->address.addrtype)
    {
      case IPCR_ADDR_PORT:
        if(!(supported_addrtypes & IPCR_ADDR_PORT_SUPP))
        {
          return QEOPNOTSUPP;
        }
        break;

      case IPCR_ADDR_NAME:
        if(!(supported_addrtypes & IPCR_ADDR_NAME_SUPP))
        {
          return QEOPNOTSUPP;
        }
        break;

      default:
        return QEOPNOTSUPP;
        break;
    }
  }
  else
  {
    if(addrlen > 0)
    {
      return QEINVAL;
    }
    if(!null_supported)
    {
      return QEDESTADDRREQ;
    }
  }
  return 0;
}

static int err_ipcr_to_sock(int ipcr_err)
{
  int ret;
  switch(ipcr_err)
  {
    case IPC_ROUTER_STATUS_SUCCESS:
      ret = 0;
      break;
    case IPC_ROUTER_STATUS_NO_MEM:
    case IPC_ROUTER_QDI_NO_MEM_ERR:
      ret = QENOMEM;
      break;
    case IPC_ROUTER_STATUS_BUSY:
      ret = QEAGAIN;
      break;
    case IPC_ROUTER_QDI_HUNGUP_ERR:
      ret = QECONNRESET;
      break;
    case IPC_ROUTER_STATUS_INVALID_ENDPOINT:
      ret = QEHOSTUNREACH;
      break;
    case IPC_ROUTER_QDI_PARAM_ERR:
      ret = QEINVAL;
      break;
    case IPC_ROUTER_QDI_NO_SPACE_ERR:
      ret = QEMSGSIZE;
      break;
    case IPC_ROUTER_QDI_PERM_ERR:
      ret = QEACCES;
      break;
    case IPC_ROUTER_QDI_NOT_SUPP_ERR:
      ret = QEOPNOTSUPP;
      break;
    case IPC_ROUTER_QDI_BADF_ERR:
      ret = QEBADF;
      break;
    case IPC_ROUTER_QDI_TIMEOUT_ERR:
      ret = QETIMEDOUT;
      break;
    case IPC_ROUTER_QDI_ADDR_REQ_ERR:
      ret = QEDESTADDRREQ;
      break;
    case IPC_ROUTER_QDI_NO_MSG_ERR:
      ret = QENOMSG;
      break;
    default:
      ret = QEFAULT;
      break;
  }
  return ret;
}

/*===========================================================================
  FUNCTION  qsocket
===========================================================================*/
QSOCKET_EXPORT int qsocket(int domain, int type, int protocol)
{
  int fd, rc, handle = 0;

  if(domain != AF_IPC_ROUTER)
  {
    return QEAFNOSUPPORT;
  }

  if(type != QSOCK_DGRAM && type != QSOCK_STREAM)
  {
    return QEOPNOTSUPP;
  }

  fd = qurt_qdi_open(IPC_ROUTER_QDI_DEV_NAME, NULL, NULL, NULL);
  if(fd < 0)
  {
    return err_ipcr_to_sock(fd);
  }

  /* each fd can have multiple sub-handles. This is used by ipc_router_qdi_user
   * but for sockets, we will have just 1 sub handle for each fd. So, we 
   * are expecting the returned handle to be 1 */
  rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_OPEN, &handle, 0, NULL, NULL);
  if(rc < 0 || handle != IPC_ROUTER_QDI_EXP_HANDLE)
  {
    qurt_qdi_close(fd);
    if(rc < 0)
      rc = err_ipcr_to_sock(rc);
    else
      rc = QEFAULT;
    return rc;
  }

  return fd;
}

/*===========================================================================
  FUNCTION  qconnect
===========================================================================*/
QSOCKET_EXPORT int qconnect(int fd, struct qsockaddr *addr, qsocklen_t addrlen)
{
  ipcr_addr_t *dest = (ipcr_addr_t *)addr;
  ipc_router_address_type ipc_addr;
  int rc;

  if(0 != (rc = validate_addr(dest, addrlen, IPCR_ADDR_PORT_SUPP, 0)))
  {
    return rc;
  }

  ipc_addr.processor_id = (uint32)dest->address.addr.port_addr.node_id;
  ipc_addr.port_id = (uint32)dest->address.addr.port_addr.port_id;

  rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_CONNECT, 
                                IPC_ROUTER_QDI_EXP_HANDLE, &ipc_addr);
  
  return err_ipcr_to_sock(rc);
}

/*===========================================================================
  FUNCTION  qclose
===========================================================================*/
QSOCKET_EXPORT int qclose(int fd)
{
  int rc;
  rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_CLOSE, 
                                IPC_ROUTER_QDI_EXP_HANDLE);
  if(rc != 0)
  {
    return err_ipcr_to_sock(rc);
  }
  rc = qurt_qdi_close(fd);
  return rc == 0 ? 0 : QEBADF;
}

/*===========================================================================
  FUNCTION  qshutdown
===========================================================================*/
QSOCKET_EXPORT int qshutdown(int fd, int how)
{
  /* We do not support other forms of shutdown.
   * If a need presents itself, add the support */
  if(how != QSHUT_RDWR)
  {
    return QEOPNOTSUPP;
  }
  return qclose(fd);
}

/*===========================================================================
  FUNCTION  qbind
===========================================================================*/
QSOCKET_EXPORT int qbind(int fd, struct qsockaddr *_addr, qsocklen_t addrlen)
{
  ipcr_addr_t *addr = (ipcr_addr_t *)_addr;
  int rc = 0;

  if(0 != (rc = validate_addr(addr, addrlen, 
          IPCR_ADDR_NAME_SUPP | IPCR_ADDR_SNAME_SUPP, 0)))
  {
    return rc;
  }

  if(addr->address.addrtype == IPCR_ADDR_NAME)
  {
    ipc_router_service_type service;
    service.type = addr->address.addr.port_name.service;
    service.instance = addr->address.addr.port_name.instance;

    rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_REGISTER_SERVER, 
                              IPC_ROUTER_QDI_EXP_HANDLE, &service);
  }
  else
  {
    char *name = addr->address.addr.port_sname;
    rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_REGISTER_NAME, 
                              IPC_ROUTER_QDI_EXP_HANDLE, name, strlen(name) + 1);
  }
  return err_ipcr_to_sock(rc);
}


/*===========================================================================
  FUNCTION  ipcr_find_name
===========================================================================*/
QSOCKET_EXPORT int ipcr_find_name(int fd, ipcr_name_t *name, struct qsockaddr_ipcr *addrs, 
                    unsigned int *instances, unsigned int *io_num_entries, 
                    unsigned int flags)
{
  int rc;
  uint32_t mask = (flags & IPCR_FLAGS_ANY_INSTANCE) ? 0 : 0xffffffff;
  ipc_router_service_type service;
  uint32_t num_entries = 0, num_servers, i;
  ipc_router_server_type *servers = NULL;

  if(!name)
  {
    return QEINVAL;
  }

  service.type = (uint32)name->service;
  service.instance = (uint32)name->instance;

  if(io_num_entries)
  {
    num_entries = (unsigned int)*io_num_entries;
    if(num_entries > 0)
    {
      if(!addrs)
      {
        return QEINVAL;
      }
      servers = ipc_router_os_malloc(num_entries * sizeof(*servers));
      if(!servers)
      {
        return QENOMEM;
      }
    }
  }

  rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_FIND_ALL_SERVERS, IPC_ROUTER_QDI_EXP_HANDLE,
                              &service, servers, &num_entries, &num_servers, mask);
  if(rc == 0)
  {
    rc = (int)num_servers;
    if(addrs && servers)
    {
      for(i = 0; i < num_entries; i++)
      {
        addrs[i].sa_family = AF_IPC_ROUTER;
        addrs[i].address.addrtype = IPCR_ADDR_PORT;
        addrs[i].address.addr.port_addr.node_id = (uint32_t)servers[i].addr.processor_id;
        addrs[i].address.addr.port_addr.port_id = (uint32_t)servers[i].addr.port_id;
        if(instances)
          instances[i] = servers[i].service.instance;
      }
    }
  }
  else
  {
    rc = 0;
  }
  if(io_num_entries)
  {
    *io_num_entries = num_entries;
  }
  if(servers)
  {
    ipc_router_os_free(servers);
  }

  return rc;
}


/*===========================================================================
  FUNCTION  ipcr_find_sname
===========================================================================*/
QSOCKET_EXPORT int ipcr_find_sname(int fd, char *name, struct qsockaddr_ipcr *out_addrs, 
                    unsigned int *io_num_entries, unsigned int flags)
{
  uint32_t num_entries = 0, num_servers = 0;
  ipc_router_address_type *addrs = NULL;
  int rc, i;

  if(!name)
  {
    return QEINVAL;
  }

  if(io_num_entries)
  {
    num_entries = (unsigned int)*io_num_entries;
    if(num_entries > 0)
    {
      if(!out_addrs)
      {
        return QEINVAL;
      }
      addrs = ipc_router_os_malloc(num_entries * sizeof(*addrs));
      if(!addrs)
      {
        return QENOMEM;
      }
    }
  }

  rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_FIND_NAME, IPC_ROUTER_QDI_EXP_HANDLE,
                              name, strlen(name) + 1, addrs, &num_entries, &num_servers);
  if(rc == 0)
  {
    rc = (int)num_servers;
    if(out_addrs && addrs)
    {
      for(i = 0; i < num_entries; i++)
      {
        out_addrs[i].sa_family = AF_IPC_ROUTER;
        out_addrs[i].address.addrtype = IPCR_ADDR_PORT;
        out_addrs[i].address.addr.port_addr.node_id = (uint32_t)addrs[i].processor_id;
        out_addrs[i].address.addr.port_addr.port_id = (uint32_t)addrs[i].port_id;
      }
    }
  }
  else
  {
    rc = 0;
  }
  if(io_num_entries)
  {
    *io_num_entries = num_entries;
  }
  if(addrs)
  {
    ipc_router_os_free(addrs);
  }
  return rc;
}


QSOCKET_EXPORT int qgetsockopt(int fd, int level, int optname, 
                  void *optval, qsocklen_t *optlen)
{
  return QEOPNOTSUPP;
}



QSOCKET_EXPORT int qsetsockopt(int fd, int level, int optname, 
                 void *optval, qsocklen_t optlen)
{
  ipc_router_qdi_port_options options = {0};
  int rc = 0;

  switch(level)
  {
    case QSOL_SOCKET:
      switch(optname)
      {
        case QSO_SNDTIMEO:
          if(!optval || optlen < sizeof(int))
          {
            rc = QEINVAL;
            break;
          }
          options.sendtimeo_valid = 1;
          options.sendtimeo = *((uint32_t *)optval);
          rc = 0;
          break;
        case QSO_RCVTIMEO:
          if(!optval || optlen < sizeof(int))
          {
            rc = QEINVAL;
            break;
          }
          options.rcvtimeo_valid = 1;
          options.rcvtimeo = *((uint32_t *)optval);
          rc = 0;
          break;
        default:
          rc = QEOPNOTSUPP;
          break;
      }
      break;
    case QSOL_IPC_ROUTER:
      switch(optname)
      {
        case QSO_IPCR_SET_CONTROL_PORT:
          options.ctrl_port_valid = 1;
          options.allowed_valid = 1;
          options.allowed_flag = EVENT_ALLOWED;
          rc = 0;
          break;
        case QSO_IPCR_SET_SERVICE_PREF:
          options.type_pref_valid = 1;
          options.type_pref = *((uint32_t *)optval);
          rc = 0;
          break;
        default:
          rc = QEOPNOTSUPP;
          break;
      }
      break;
    default:
      rc = QEOPNOTSUPP;
      break;
  }
  if(rc != 0)
    return rc;
  rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_SET_PORT_OPTIONS, 
                                IPC_ROUTER_QDI_EXP_HANDLE, &options);
  return err_ipcr_to_sock(rc);
}


/*===========================================================================
  FUNCTION  qsendto
===========================================================================*/
QSOCKET_EXPORT int qsendto(int fd, void *buf, size_t len, int flags, 
              struct qsockaddr *_addr, qsocklen_t addrlen)
{
  ipcr_addr_t *addr = (ipcr_addr_t *)_addr;
  int rc;
  uint32 drv_flags = 0;

  if(!buf || !len)
  {
    return QEINVAL;
  }

  if(0 != (rc = validate_addr(addr, addrlen, 
          IPCR_ADDR_PORT_SUPP | IPCR_ADDR_NAME_SUPP | IPCR_ADDR_SNAME_SUPP, 1)))
  {
    return rc;
  }

  if(flags & QMSG_DONTWAIT)
  {
    drv_flags |= IPC_ROUTER_QDI_WRFLAG_DONTWAIT;
  }

  if(addr)
  {
    if(addr->address.addrtype == IPCR_ADDR_NAME)
    {
      ipc_router_service_type service;

      service.type = (uint32)addr->address.addr.port_name.service;
      service.instance = (uint32)addr->address.addr.port_name.instance;

      rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_SEND_TO_NAME, 
                          IPC_ROUTER_QDI_EXP_HANDLE, &service, buf, len);
    }
    else if(addr->address.addrtype == IPCR_ADDR_SNAME)
    {
      char *name = addr->address.addr.port_sname;
      rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_SEND_TO_SNAME,
                                    IPC_ROUTER_QDI_EXP_HANDLE, name, strlen(name) + 1,
                                    buf, len);
    }
    else if(addr->address.addrtype == IPCR_ADDR_PORT)
    {
      ipc_router_address_type iaddr;
      iaddr.processor_id = (uint32)addr->address.addr.port_addr.node_id;
      iaddr.port_id = (uint32)addr->address.addr.port_addr.port_id;
      rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_SEND, IPC_ROUTER_QDI_EXP_HANDLE,
                                    &iaddr, buf, len, drv_flags);
    }
    else
    {
      return QEINVAL;
    }
  }
  else
  {
    rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_SEND, IPC_ROUTER_QDI_EXP_HANDLE,
                                 NULL, buf, len, drv_flags);
  }

  return rc == IPC_ROUTER_STATUS_SUCCESS ? (int)len : err_ipcr_to_sock(rc);
}

/*===========================================================================
  FUNCTION  qsend
===========================================================================*/
QSOCKET_EXPORT int qsend(int fd, void *buf, size_t len, int flags)
{
  return qsendto(fd, buf, len, flags, NULL, 0);
}

/*===========================================================================
  FUNCTION  qrecvfrom
===========================================================================*/
QSOCKET_EXPORT int qrecvfrom(int fd, void *buffer, size_t len, int flags, 
               struct qsockaddr *_addr, qsocklen_t *addrlen)
{
  ipcr_addr_t *addr = (ipcr_addr_t *)_addr;
  ipc_router_address_type src_addr;
  int rc;
  uint32_t drv_flags = 0;

  if((len > 0 && !buffer) ||
      (addrlen && *addrlen < sizeof(*addr)) ||
      (addrlen && !addr) || 
      (!addrlen && addr))
  {
    return QEINVAL;
  }

  if(flags & QMSG_PEEK)
  {
    drv_flags |= IPC_ROUTER_QDI_RDFLAG_MSGPEEK;
  }
  if(flags & QMSG_DONTWAIT)
  {
    drv_flags |= IPC_ROUTER_QDI_RDFLAG_DONTWAIT;
  }
  if(flags & QMSG_TRUNC)
  {
    drv_flags |= IPC_ROUTER_QDI_RDFLAG_TRUNC;
  }

  rc = qurt_qdi_handle_invoke(fd, IPC_ROUTER_QDI_READ, buffer, len, 
                              addr ? &src_addr : NULL, NULL, &len, NULL, drv_flags);
  if(addr)
  {
    *addrlen = sizeof(ipcr_addr_t);
    addr->sa_family = AF_IPC_ROUTER;
    if(rc == IPC_ROUTER_STATUS_SUCCESS)
    {
      addr->address.addrtype = IPCR_ADDR_PORT;
      addr->address.addr.port_addr.node_id = (uint32_t)src_addr.processor_id;
      addr->address.addr.port_addr.port_id = (uint32_t)src_addr.port_id;
    }
  }
  return rc == IPC_ROUTER_STATUS_SUCCESS ? (int)len : err_ipcr_to_sock(rc);
}

/*===========================================================================
  FUNCTION  qrecv
===========================================================================*/
QSOCKET_EXPORT int qrecv(int fd, void *buf, size_t len, int flags)
{
  return qrecvfrom(fd, buf, len, flags, NULL, NULL);
}


/*===========================================================================
  FUNCTION  qpoll
===========================================================================*/
QSOCKET_EXPORT int qpoll(struct qpollfd *pfd, qnfds_t num, int timeout_ms)
{
  int rc = 0, i;

  if(!pfd || !num)
  {
    return QEINVAL;
  }
  for(i = 0; i < num; i++)
  {
    pfd[i].revents = 0;
    if(pfd[i].fd >= 0)
    {
      rc = qurt_qdi_handle_invoke(pfd[i].fd, IPC_ROUTER_QDI_POLL, pfd, num, timeout_ms);
      break;
    }
  }
  if(i == num)
  {
    /* All FDs were negative, so just sleep for the required duration */
    if(timeout_ms >= 0)
    {
      timer_sleep(timeout_ms, T_MSEC, TRUE);
    }
    else
    {
      return QEWOULDBLOCK;
    }
  }

  return rc < 0 ? err_ipcr_to_sock(rc) : rc;
}

void qsocket_init(void)
{
  /* Dummy function, nothing to do */
}

