#ifndef IPC_ROUTER_PROTOCOL_H
#define IPC_ROUTER_PROTOCOL_H
/*===========================================================================

                    I P C    R O U T E R   P R O T O C O L

   This file describes the types related to the routing protocol

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/ipc_router/src/ipc_router_protocol.h#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_types.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/
#define IPC_ROUTER_V1_VERSION              (0x1UL)
#define IPC_ROUTER_V3_VERSION              (0x3UL)

/* Versions mask supported by IPC Router */
#define IPC_ROUTER_VERSION_SUPPORTED_MASK  0xA
#define IPC_ROUTER_HELLO_MAGIC             0xE110
#define IPC_ROUTER_MIN_VERSION             IPC_ROUTER_V1_VERSION
#define IPC_ROUTER_MAX_VERSION             IPC_ROUTER_V3_VERSION
#define IPC_ROUTER_MAX_HEADER_SIZE      (sizeof(ipc_router_header_type))

#define IPC_ROUTER_RESERVED_PORTS_START (0xFFFFFFF0UL)
#define IPC_ROUTER_ROUTER_PORT          (0xFFFFFFFEUL)
#define IPC_ROUTER_BROADCAST_ADDRESS    (0xFFFFFFFFUL)

#define BUILD_HEADER(header, type, src_proc, src_port, conf_rx, size, dest_proc, dest_port) \
  do { \
    header.version               = 0; \
    header.msg_type              = type; \
    header.src_addr.processor_id = src_proc; \
    header.src_addr.port_id      =  src_port; \
    header.confirm_rx            = conf_rx; \
    header.msg_size              = size; \
    header.dest_addr.processor_id = dest_proc; \
    header.dest_addr.port_id      = dest_port; \
  } while(0)


/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
typedef struct
{
   uint32 checksum;
   uint32 versions_bitmask;
   uint32 capabilities_bitmask;
}ipc_router_hello_args;

typedef struct
{
  uint32                        command;
  union
  {
    ipc_router_server_type    server_arg;
    ipc_router_address_type   client_arg;
    ipc_router_hello_args     hello_arg;
  }arguments;
}ipc_router_control_msg_body;

typedef struct 
{
  uint32                  version;
  uint32                  msg_type;
  ipc_router_address_type src_addr;
  uint32                  confirm_rx;
  uint32                  msg_size;
  ipc_router_address_type dest_addr; 
}ipc_router_header_type;

/* control message types */
typedef enum
{
  IPC_ROUTER_CONTROL_INVALID = 0,
  IPC_ROUTER_CONTROL_DATA,
  IPC_ROUTER_CONTROL_HELLO,
  IPC_ROUTER_CONTROL_BYE,
  IPC_ROUTER_CONTROL_NEW_SERVER,
  IPC_ROUTER_CONTROL_REMOVE_SERVER,
  IPC_ROUTER_CONTROL_REMOVE_CLIENT,
  IPC_ROUTER_CONTROL_RESUME_TX,
  IPC_ROUTER_CONTROL_MCAST_DATA
}ipc_router_message_type;

/*===========================================================================
                        PROTOCOL WIRE FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_protocol_inject_header

DESCRIPTION   Inject header into message

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
int ipc_router_protocol_inject_header
(
  ipc_router_header_type *header,
  ipc_router_packet_type **msg
);

/*===========================================================================
FUNCTION      ipc_router_protocol_extract_header

DESCRIPTION   Extract and remove header from message

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
int ipc_router_protocol_extract_header
(
  ipc_router_packet_type **msg,
  ipc_router_header_type *header
);

/*===========================================================================
FUNCTION      ipc_router_protocol_peek_size

DESCRIPTION   Peeks into the header of the packet and returns the expected
              message size size (header + payload + padding).

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
int ipc_router_protocol_peek_size
(
  ipc_router_packet_type *msg,
  uint32 *packet_size
);

#endif
