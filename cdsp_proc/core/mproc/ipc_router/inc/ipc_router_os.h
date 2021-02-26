#ifndef IPC_ROUTER_OS_H
#define IPC_ROUTER_OS_H

/*===========================================================================

                    I P C    R O U T E R   O S   R E X

   This file describes the QURT interface to the IPC Router.
   This file must be compatible with all operating systems on which the 
   router will be used.

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/ipc_router/inc/ipc_router_os.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/04/09   ap      Klockwork fixes
07/01/09   ap      Changed IPC_ROUTER_OS_MESSAGE macro to directly call MSG_* 
05/07/09   sa      Removed flow control ioctl, switched to ping mechanism
02/03/09   sa      Moved in network order macros from DS header
10/16/08   sa      Added ipc_router_get_fc_status function
04/08/08   sa      Switched back to using IPC_ROUTER_OS_MESSAGE macro
03/20/08   sa      Switched to using ipc_router_os_message function
                   and added smem logging 
10/31/07   rr      Fix signals so we only use ONCIPC_REPLY_SIG
08/24/07   bfc     Increased the size of the IPC_ROUTER_CLIENT_QUEUE_SIZE.
04/10/07   rr      Initial Draft
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "stdlib.h"
#include "string.h" 
#include "stringl.h" 
#include "qurt.h"
#include "err.h"
#include "msg.h"

/*===========================================================================
                CONSTANT / MACRO DACLARATIONS
===========================================================================*/

/* msg.h defines MSG_0() as MSG() */
#define MSG_0 MSG

/* Allow var arg style calls to MSG */
#define _MSGPASTE(a,b) _MSGPASTE_(a,b)
#define _MSGPASTE_(a,b) a##b
#define _MSGCNT(...) _MSGCNT_(__VA_ARGS__,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define _MSGCNT_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,cnt,...) cnt


#define IPC_ROUTER_MSG_EMERG   MSG_LEGACY_ERROR
#define IPC_ROUTER_MSG_CRIT    MSG_LEGACY_ERROR
#define IPC_ROUTER_MSG_ERR     MSG_LEGACY_ERROR
#define IPC_ROUTER_MSG_ALERT   MSG_LEGACY_ERROR
#define IPC_ROUTER_MSG_NOTICE  MSG_LEGACY_MED
#define IPC_ROUTER_MSG_WARNING MSG_LEGACY_MED
#define IPC_ROUTER_MSG_DEBUG   MSG_LEGACY_LOW
#define IPC_ROUTER_MSG_INFO    MSG_LEGACY_LOW

#define IPC_ROUTER_OS_MESSAGE(level, fmt, ...)  \
  _MSGPASTE(MSG_,_MSGCNT(MSG_SSID_ONCRPC, level,fmt,##__VA_ARGS__))(MSG_SSID_ONCRPC,level,fmt, ## __VA_ARGS__)

/*===========================================================================
                      TYPE DECLARATIONS                     
===========================================================================*/

/*===========================================================================
                         MACRO DACLARATIONS
===========================================================================*/
#define ipc_router_os_sem qurt_mutex_t

#define ipc_router_os_malloc(size)  \
            malloc(size)

#define ipc_router_os_free(ptr)  \
            free(ptr)

#define ipc_router_os_calloc(num,size)  \
            calloc(num,size)

#define ipc_router_os_mem_set(ptr,value,size)\
            memset(ptr,value,size)
/*=============================================================================
  MACRO  ipc_router_os_mem_copy
=============================================================================*/
/*!
@brief
  Copy over from source buffer to destination buffer.

@param[in]  dst     Pointer to the destination buffer
@param[in]  d_len   Size in bytes of the destination buffer
@param[in]  src     Pointer to the source buffer
@param[in]  c_len   Size in bytes to copy from source to destination

@retval   Total number of bytes which were copied from source to destination
*/
/*=========================================================================*/
#define ipc_router_os_mem_copy(d,ds,s,ss) memscpy(d,ds,s,ss)

#define ipc_router_os_sem_init(sem) qurt_pimutex_init(sem)
#define ipc_router_os_sem_lock(sem) qurt_pimutex_lock(sem)
#define ipc_router_os_sem_unlock(sem) qurt_pimutex_unlock(sem)
#define ipc_router_os_sem_deinit(sem) qurt_pimutex_destroy(sem)


#endif /* IPC_ROUTER_OS_H */

