#ifndef IPC_ROUTER_TYPES_H
#define IPC_ROUTER_TYPES_H
/*===========================================================================

                       I P C   R O U T E R  T Y P E S

                           H E A D E R    F I L E

DESCRIPTION
  This file contains general data types and definitions for IPC Router
  
  ---------------------------------------------------------------------------
  Copyright (c) 2007-2013, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/ipc_router/inc/ipc_router_types.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/17   jlg     Moved settings-related structure definitions to ipc_router_settings.h
04/30/07   sa      First revision   
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_os.h"
#include "ipc_router_packet.h"

/*===========================================================================
                CONSTANT / MACRO DACLARATIONS
===========================================================================*/
/*---------------------------------------------------------------------------
  Error Codes
---------------------------------------------------------------------------*/
#define IPC_ROUTER_STATUS_UNKNOWN        (-1)
#define IPC_ROUTER_STATUS_SUCCESS          0
#define IPC_ROUTER_STATUS_FAILURE          1
#define IPC_ROUTER_STATUS_INVALID_PARAM    2
#define IPC_ROUTER_STATUS_TIMEOUT          3
#define IPC_ROUTER_STATUS_INVALID_MEM      4
#define IPC_ROUTER_STATUS_NO_MEM           5
#define IPC_ROUTER_STATUS_IO_ERROR         6
#define IPC_ROUTER_STATUS_INTERRUPTED      7
#define IPC_ROUTER_STATUS_BUSY             8
#define IPC_ROUTER_STATUS_INCOMP_SERVER    9
#define IPC_ROUTER_STATUS_NOPERM           10
#define IPC_ROUTER_STATUS_INVALID_ENDPOINT 11
#define IPC_ROTUER_STATUS_LAST             12

/*---------------------------------------------------------------------------
  IPC Router options valid mask values 
---------------------------------------------------------------------------*/
#define IPC_ROUTER_CORE_PORT_OPTIONS_PRIORITY_VALID (0x1)
#define IPC_ROUTER_CORE_PORT_OPTIONS_SCOPE_VALID    (0x2)

/*---------------------------------------------------------------------------
  SMEM LOG EVENTS
---------------------------------------------------------------------------*/
#define IPC_ROUTER_LOG_EVENT_UNKNOWN         (0)
#define IPC_ROUTER_LOG_EVENT_MSG_READ        (IPC_ROUTER_LOG_EVENT_UNKNOWN + 1)
#define IPC_ROUTER_LOG_EVENT_MSG_WRITTEN     (IPC_ROUTER_LOG_EVENT_UNKNOWN + 2)
#define IPC_ROUTER_LOG_EVENT_MSG_CFM_REQ     (IPC_ROUTER_LOG_EVENT_UNKNOWN + 3)
#define IPC_ROUTER_LOG_EVENT_MSG_CFM_SNT     (IPC_ROUTER_LOG_EVENT_UNKNOWN + 4)
#define IPC_ROUTER_LOG_EVENT_MID_READ        (IPC_ROUTER_LOG_EVENT_UNKNOWN + 5)
#define IPC_ROUTER_LOG_EVENT_MID_WRITTEN     (IPC_ROUTER_LOG_EVENT_UNKNOWN + 6)
#define IPC_ROUTER_LOG_EVENT_MID_CFM_REQ     (IPC_ROUTER_LOG_EVENT_UNKNOWN + 7)
#define IPC_ROUTER_LOG_EVENT_CTL_PING        (IPC_ROUTER_LOG_EVENT_UNKNOWN + 8)
#define IPC_ROUTER_LOG_EVENT_SERV_PENDING    (IPC_ROUTER_LOG_EVENT_UNKNOWN + 9)
#define IPC_ROUTER_LOG_EVENT_SERV_REGISTERED (IPC_ROUTER_LOG_EVENT_UNKNOWN + 10)

/*---------------------------------------------------------------------------
  Xport network IDs (bitmaps) are used to support routing. Packets will only i
  be forwarded if the incoming link and outgoing links have different bitmaps.
  The "LOCAL" bit further denote the link belonging to the local cluster.
  It is is used to support server precedence when there are duplicate servers
  on the network, where local node > local cluster > rest of the network.
  Whenever there is a tie, the lowest processor ID wins out.
---------------------------------------------------------------------------*/
#define IPC_ROUTER_XPORT_NET_LOCAL     (IPC_ROUTER_XPORT_NET_TIER_1)
#define IPC_ROUTER_XPORT_NET_TIER_1    (0x01)
#define IPC_ROUTER_XPORT_NET_TIER_2    (0x02)
#define IPC_ROUTER_XPORT_NET_TIER_3    (0x04)
#define IPC_ROUTER_XPORT_NET_TIER_4    (0x08)
#define IPC_ROUTER_XPORT_NET_TIER_5    (0x10)
#define IPC_ROUTER_XPORT_NET_TIER_6    (0x20)
#define IPC_ROUTER_XPORT_NET_TIER_7    (0x40)
#define IPC_ROUTER_XPORT_NET_TIER_8    (0x80)
#define IPC_ROUTER_XPORT_NET_ALL       (0xFF)
#define IPC_ROUTER_XPORT_NET_DEFAULT   (IPC_ROUTER_XPORT_NET_LOCAL)

/*---------------------------------------------------------------------------
  Xport priorities are used to prioritize links when multiple links are
  available.
---------------------------------------------------------------------------*/
#define IPC_ROUTER_XPORT_PRIORITY_DEFAULT (0x00)
#define IPC_ROUTER_XPORT_PRIORITY_LOW     (0x01)
#define IPC_ROUTER_XPORT_PRIORITY_MEDIUM  (0x02)
#define IPC_ROUTER_XPORT_PRIORITY_HIGH    (0x03)
#define IPC_ROUTER_XPORT_PRIORITY_MASK    (0x03)

#define IPC_ROUTER_PORT_PRIORITY_DEFAULT (0x00)
#define IPC_ROUTER_PORT_PRIORITY_LOW     (0x01)
#define IPC_ROUTER_PORT_PRIORITY_MEDIUM  (0x02)
#define IPC_ROUTER_PORT_PRIORITY_HIGH    (0x03)
#define IPC_ROUTER_PORT_PRIORITY_MASK    (0x03)


/*---------------------------------------------------------------------------
  Policy interface defines
---------------------------------------------------------------------------*/
#define IPC_ROUTER_DEFAULT_SCOPE (0xffffffffffffffffULL)

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
struct ipc_router_client_s;

typedef struct 
{
  uint32   options_set; /** Bitmask indicating which of the members are valid */
  uint32   priority;    /** Priority */  
  uint64   scope;       /** Scope */
} ipc_router_core_options_type;

typedef struct 
{
  uint32 processor_id;
  uint32 port_id;
}ipc_router_address_type;

typedef struct
{
  uint32 type;
  uint32 instance;
}ipc_router_service_type;

typedef void (*ipc_router_rx_cb_type)
(
 struct ipc_router_client_s *client, 
 void                       *cb_data,
 ipc_router_packet_type     *msg, 
 ipc_router_address_type    *src_addr,
 uint32                      len,
 boolean                     confirm_rx
 ); 

typedef enum
{
  IPC_ROUTER_EVENT_NONE = 0,
  IPC_ROUTER_EVENT_NEW_SERVER,
  IPC_ROUTER_EVENT_REMOVE_SERVER,
  IPC_ROUTER_EVENT_REMOVE_CLIENT,
  IPC_ROUTER_EVENT_RESUME_TX,
  IPC_ROUTER_EVENT_CLOSED
} ipc_router_event_type;

typedef struct
{
  ipc_router_service_type service;
  ipc_router_address_type addr;
}ipc_router_server_type;

typedef union 
{
  ipc_router_server_type    server;
  ipc_router_address_type   client;
} ipc_router_event_info_type;

typedef void (*ipc_router_event_cb_type)
(
 struct ipc_router_client_s *client,
 void                       *data,
 ipc_router_event_type       event,
 ipc_router_event_info_type *info
 );

/** Return boolean 0 or 1 for allowing send from a scope to a tag */
typedef int (*ipc_router_send_policy_cb_type)(uint64 *from_scope, uint32 to_proc, uint64 *to_xport_scope);

/** Return boolean 0 or 1 for allowing recv from a scope to a tag */
typedef int (*ipc_router_recv_policy_cb_type)(uint64 *from_xport_scope, uint32 from_proc, uint64 *to_scope);

/** Return boolean 0 or 1 and the scope (if 1) if the type is allowed
 * on the host */
typedef int (*ipc_router_get_scope_policy_cb_type)(uint32 type, uint32 instance, uint32 host, uint64 *scope);

/** Return the scope of the xport */
typedef uint64 (*ipc_router_get_link_scope_policy_cb_type)(uint32 xport_tag);

/** IPC Policy operations table. An external policy manager shall 
 * provide the following functions */
typedef struct
{
  ipc_router_send_policy_cb_type    send;
  ipc_router_recv_policy_cb_type    recv;
  ipc_router_get_scope_policy_cb_type get_scope;
  ipc_router_get_link_scope_policy_cb_type get_xport_scope;
} ipc_router_policy_ops_type;

#endif
