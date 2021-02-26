/*===========================================================================

                      I P C    R O U T E R    L O G

   This file describes the logging interface

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/ipc_router/src/ipc_router_log.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_os.h"
#include "ipc_router_types.h"
#include "ipc_router_log.h"
#include "ipc_router_packet.h"
#include "ipc_router_protocol.h"
#include "msg.h"
#include "smem_log.h"
#include "ULogFront.h"

size_t strlcpy(char *dst, const char *src, size_t siz);

/* IPC router events */
#define IPC_ROUTER_LOG_EVENT_ERROR      0x00

/* Define this feature in scons for engineering builds */
#ifdef FEATURE_IPC_ROUTER_SMEM_LOG
#define LOG_EVENT3(event, d1, d2, d3) \
  SMEM_LOG_EVENT(SMEM_LOG_IPC_ROUTER_EVENT_BASE | event, d1, d2, d3)

#define LOG_EVENT6(event, d1, d2, d3, d4, d5, d6) \
  SMEM_LOG_EVENT6(SMEM_LOG_IPC_ROUTER_EVENT_BASE | event, d1, d2, d3, \
      d4, d5, d6)

#else
#define LOG_EVENT3(event, d1, d2, d3)
#define LOG_EVENT6(event, d1, d2, d3, d4, d5, d6)
#endif

/* size of ulog buffer */
#define IPCR_ULOG_BUFFER_SIZE  ( 16384 )

/* ULOG handle */
ULogHandle ipcr_ulog_handle = NULL;

#define IPCR_CTRL_ULOG_MSG(dir, count, format, args...) \
   do { \
   if(ipcr_ulog_handle) {\
      if(IPC_ROUTER_LOG_TX  == dir)\
         ULogFront_RealTimePrintf(ipcr_ulog_handle, count, ("[CTRL] [TX]" format),  ##args); \
      else \
         ULogFront_RealTimePrintf(ipcr_ulog_handle, count, ("[CTRL] [RX]" format),  ##args); \
     }\
   } while(0)

#define IPCR_PKT_ULOG_MSG(dir, count, format, args...) \
   do { \
   if(ipcr_ulog_handle) {\
      if(IPC_ROUTER_LOG_TX  == dir)\
         ULogFront_RealTimePrintf(ipcr_ulog_handle, count, ("[PKT] [TX]" format),  ##args); \
      else \
         ULogFront_RealTimePrintf(ipcr_ulog_handle, count, ("[PKT] [RX]" format),  ##args); \
      } \
   } while(0)

#define IPCR_ERR_ULOG_MSG(count, format, args...) \
      do { \
      if(ipcr_ulog_handle) \
         ULogFront_RealTimePrintf(ipcr_ulog_handle, count, ("[ERR]" format),  ##args); \
      } while(0)

void ipc_router_log_ctrl
(
 ipc_router_log_dir dir,
 ipc_router_xport_type *xport,
 ipc_router_control_msg_body *cntl_msg
 )
{
  uint32 addr;
  uint32 type = cntl_msg->command;
  switch(type)
  {
    case IPC_ROUTER_CONTROL_NEW_SERVER:
    case IPC_ROUTER_CONTROL_REMOVE_SERVER:
      addr = (cntl_msg->arguments.server_arg.addr.processor_id << 24)
        | (cntl_msg->arguments.server_arg.addr.port_id & 0xffffff);
      LOG_EVENT3(dir | (type << 8), 
          addr, 
          cntl_msg->arguments.server_arg.service.type,
          cntl_msg->arguments.server_arg.service.instance);
      IPCR_CTRL_ULOG_MSG(dir, 4 ,"type:%d,addr:0x%x,svc:0x%x,inst:0x%x", 
         type,
         addr,
         cntl_msg->arguments.server_arg.service.type,
         cntl_msg->arguments.server_arg.service.instance);
      break;
    case IPC_ROUTER_CONTROL_REMOVE_CLIENT:
    case IPC_ROUTER_CONTROL_RESUME_TX:
       addr = (cntl_msg->arguments.client_arg.processor_id << 24)
         | (cntl_msg->arguments.client_arg.port_id & 0xffffff);
      LOG_EVENT3(dir | (type << 8), 
          cntl_msg->arguments.client_arg.processor_id,
          cntl_msg->arguments.client_arg.port_id, type);
      IPCR_CTRL_ULOG_MSG(dir, 2 ,"type:%d,addr:0x%x", 
         type,
         addr);
      break;
    default:
      break;
  }
}
void ipc_router_log_packet
(
  ipc_router_log_dir dir,
  ipc_router_xport_type *xport,
  ipc_router_header_type *header
)
{
  uint32 desc = 0, name = 0;
  const char *desc_str = xport && xport->desc ? xport->desc : "LOCL";
  uint32 src, dst;
  char thread_name[16];

  qurt_thread_get_name(thread_name, 16);
  thread_name[15] = '\0';
  ipc_router_os_mem_copy(&name, sizeof(uint32), thread_name, sizeof(uint32));

  ipc_router_os_mem_copy(&desc, sizeof(uint32), desc_str, sizeof(uint32));
  
  src = (header->src_addr.processor_id << 24)
      | (header->src_addr.port_id & 0xffffff);
  dst = (header->dest_addr.processor_id << 24)
      | (header->dest_addr.port_id & 0xffffff);

LOG_EVENT6(dir,
      src,                                      /* 1 */
      dst,                                      /* 2 */
      (header->msg_type << 24) | (header->confirm_rx << 16) 
      | (header->msg_size & 0xffff),            /* 3 */
      desc,                                     /* 4 */
      (uint32)qurt_thread_get_id(),             /* 5 */
      name);

IPCR_PKT_ULOG_MSG(dir, 6 ,"src:0x%x,dst:0x%x,msg type:%d,msg size:%d,conf rx:%d,tid:0x%x",
      src,
      dst,
      header->msg_type,
      header->msg_size,
      header->confirm_rx,
      (uint32)qurt_thread_get_id());

}

/*===========================================================================
FUNCTION      ipc_router_core_log_error

DESCRIPTION   Log runtime error

ARGUMENTS     filename - filename
              line - line number

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_log_error
(
 char *filename,
 unsigned int line
 )
{
  uint32 name[5];
  char *last;
  last = strrchr(filename, '/');
  if(!last)
    last = strrchr(filename, '\\');
  last = last ? (last+1) : filename;
  strlcpy((char *)name, last, sizeof(name));
  LOG_EVENT6(IPC_ROUTER_LOG_EVENT_ERROR, name[0], name[1], name[2], name[3], 
      name[4], line);
  MSG_2(MSG_SSID_ONCRPC, MSG_LEGACY_ERROR,
        "Runtime error. File 0x%s, Line: %d", filename, line);

}

void ipc_router_log_init()
{
   if( NULL == ipcr_ulog_handle)
   {
     /* Initialize uLOG buffer */
     ULogFront_RealTimeInit( &ipcr_ulog_handle,
                             "IPCR ULOG",
                             IPCR_ULOG_BUFFER_SIZE,
                             ULOG_MEMORY_LOCAL,
                             ULOG_LOCK_OS );
}
}

