#ifndef IPC_ROUTER_PACKET_H
#define IPC_ROUTER_PACKET_H
/*===========================================================================

                    I P C    R O U T E R   P A C K E T

        This file provides an abstraction to the packet buffer manager

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/ipc_router/inc/ipc_router_packet.h#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_os.h"

#define IPC_ROUTER_NATIVE_PACKET 1

#define ipc_router_packet_read_header(msg, buf, len)\
  ipc_router_packet_read(msg, buf, len)

#define ipc_router_packet_read_payload(msg, buf, len)\
  ipc_router_packet_read(msg, buf, len)

#define ipc_router_packet_copy_tail(msg, buf, len)\
  ipc_router_packet_copy_payload(msg, buf, len)

#define ipc_router_payload_length(msg)\
  ipc_router_packet_length(msg)

typedef struct ipc_router_packet_s ipc_router_packet_type;

typedef struct ipc_router_packet_queue_s ipc_router_packet_queue_type;

struct ipc_router_packet_queue_s {
  ipc_router_os_sem      lock;
  uint32                 cnt;
  ipc_router_packet_type *head;
  ipc_router_packet_type *tail;
};


/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/

typedef void (*ipc_router_packet_free_cb)(void *ptr, void *cookie);

void ipc_router_packet_init(void);

ipc_router_packet_type *ipc_router_packet_new(uint32 headroom, uint32 reserved_len);

int ipc_router_packet_attach(ipc_router_packet_type **pkt, void *buf, uint32 size, ipc_router_packet_free_cb free_cb, void *cookie);

uint32 ipc_router_packet_copy_header(ipc_router_packet_type **pkt, void *buf, uint32 len);

uint32 ipc_router_packet_copy_payload(ipc_router_packet_type **pkt, void *buf, uint32 len);

uint32 ipc_router_packet_read(ipc_router_packet_type **pkt, void *buf, uint32 len);

void *ipc_router_packet_peek(ipc_router_packet_type *pkt, uint32 len);

uint32 ipc_router_packet_length(ipc_router_packet_type *pkt);

void ipc_router_packet_trim(ipc_router_packet_type **pkt, uint32 len);

void ipc_router_packet_free(ipc_router_packet_type **pkt);

ipc_router_packet_type *ipc_router_packet_dup(ipc_router_packet_type *src);

void ipc_router_packet_queue_init(ipc_router_packet_queue_type *queue);
void ipc_router_packet_queue_deinit(ipc_router_packet_queue_type *queue);
void ipc_router_packet_queue_put(ipc_router_packet_queue_type *queue, ipc_router_packet_type *pkt);
ipc_router_packet_type *ipc_router_packet_queue_get(ipc_router_packet_queue_type *queue);


#endif
