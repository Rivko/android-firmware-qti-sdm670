/*===========================================================================

                I P C    R O U T E R   P R O T O C O L

   This file describes the OS-independent interface to to write and
   extract the header in the respective protocol wire format.
   This file must be compatible with all operating systems on which the 
   router will be used.

  ---------------------------------------------------------------------------
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/ipc_router/src/ipc_router_protocol.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_protocol.h"
#include "ipc_router_types.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/

/* Flags in the V3 header control flags field */
#define IPC_ROUTER_V3_CTRL_FLAG_CONF_RX       0x1

/* Mcast header helper */
#define MCAST_OPT_HDR_INIT(opt, _type, _instance) \
  do {  \
    (opt).len = 3;  \
    (opt).type = IPC_ROUTER_CONTROL_MCAST_DATA; \
    (opt).ctl_flags = 0;  \
    (opt).svc.type = _type; \
    (opt).svc.instance = _instance; \
  } while(0)

#define PAD_LEN(len) ((4 - ((len) & 3)) & 3)

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                               VERSION 1

  HEADER:
      0         1         2       3
  |--------|--------|--------|--------|
  |VERSION |     RESERVED             | Word 1
  |--------|--------|--------|--------|
  |MSG_TYPE|     RESERVED             | Word 2
  |--------|--------|--------|--------|
  |       SOURCE PROCESSOR ID         | Word 3
  |--------|--------|--------|--------|
  |         SOURCE PORT ID            | Word 4
  |--------|--------|--------|--------|
  |CONF RX |     RESERVED             | Word 5
  |--------|--------|--------|--------|
  |           MESSAGE SIZE            | Word 6
  |--------|--------|--------|--------|
  |     DESTINATION PROCESSOR ID      | Word 7
  |--------|--------|--------|--------|
  |       DESTINATION PORT ID         | Word 8
  |--------|--------|--------|--------|
===========================================================================*/
typedef ipc_router_header_type ipc_router_v1_header_type;


/*===========================================================================
                               VERSION 3
  HEADER: 
      0         1         2       3
  |--------|--------|--------|--------|
  |VERSION |MSG_TYPE|CONTROL |OPT_HDR_|
  |                 |FLAGS   | LEN    | Word 1 
  |--------|--------|--------|--------|
  |              MSG_SIZE             | Word 2
  |--------|--------|--------|--------|
  |   SRC PROC ID   |   SRC PORT ID   | Word 3
  |--------|--------|--------|--------|
  |   DST PROC ID   |   DST PORT ID   | Word 3
  |--------|--------|--------|--------|

  CONTROL FLAGS:
         Bit 0    : Confirm RX
         Bits 2-15: Reserved
 
  OPTIONAL HEADER
  (Present if Optional Header Length field is not zero.)
 
      0         1         2       3
  |--------|--------|--------|--------|
  |OPT_LEN |OPT_TYPE|    OPT FLAGS    | Word 1 
  |--------|--------|--------|--------|
  .........OPT_LEN - 1 Words...........
 
  OPT_LEN:
  Length of the optional header in words
  (Including self, so it is 1 at the minimum)
 
  OPT_TYPE:
  Type of the optional header. 
 
  OPT FLAGS:
  Optional flags. 
       Bit 0-15 : Reserved
===========================================================================*/
typedef struct
{
  uint16 processor_id;
  uint16 port_id;
} ipc_router_v3_address_type;

typedef struct
{
  uint8                      version;
  uint8                      type;
  uint8                      flags;
  uint8                      opt_hdr_len;
  uint32                     msg_size;
  ipc_router_v3_address_type src_addr;
  ipc_router_v3_address_type dest_addr;
} ipc_router_v3_header_type;

typedef struct
{
  uint8  len;
  uint8  type;
  uint16 flags;
} ipc_router_v3_optional_header_type;

typedef struct
{
  uint8                   len;
  uint8                   type;
  uint16                  ctl_flags;
  ipc_router_service_type svc;
} ipc_router_mcast_opt_header;

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_peek_version

DESCRIPTION   Peeks into the message to get to the version

RETURN VALUE  Version number
===========================================================================*/
static uint32 ipc_router_peek_version(ipc_router_packet_type *msg)
{
  uint8 *version_peek;

  version_peek = (uint8 *)ipc_router_packet_peek(msg, sizeof(uint8));
  if(!version_peek)
  {
    return 0;
  }
  return (uint32)(*version_peek);
}

/*===========================================================================
                               VERSION 1
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_inject_header_v1

DESCRIPTION   Injects a V1 header to the start of a packet with the contents
              from the generic ipc_router_header_type header

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
static int ipc_router_inject_header_v1
(
  ipc_router_header_type *header,
  ipc_router_packet_type **msg
)
{
  if(ipc_router_packet_copy_header(msg, header, sizeof(*header)) != sizeof(*header))
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_extract_header_v1

DESCRIPTION   Extracts a V1 header (If present at the start of the packet) 
              and puts the contents in the generic ipc_router_header_type
              if successful.

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
static int ipc_router_extract_header_v1
(
  ipc_router_packet_type **msg, 
  ipc_router_header_type *header
)
{
  if(ipc_router_packet_read_header(msg, header, sizeof(*header)) != sizeof(*header))
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }
  header->version = IPC_ROUTER_V1_VERSION;

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_peek_size_v1

DESCRIPTION   Peeks into the header of the packet and returns the message
              size.

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
static int ipc_router_peek_size_v1
(
  ipc_router_packet_type *msg,
  uint32 *packet_size
)
{
  ipc_router_v1_header_type *hdr;
  uint32 len = 0;
  hdr = (ipc_router_v1_header_type *)ipc_router_packet_peek(msg, sizeof(*hdr));
  if(!hdr)
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }
  len = hdr->msg_size;
  len += PAD_LEN(len) + sizeof(ipc_router_v1_header_type);
  *packet_size = len;

  return IPC_ROUTER_STATUS_SUCCESS;
}


/*===========================================================================
                               VERSION 3
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_inject_header_v3

DESCRIPTION   Injects a V3 header to the start of a packet with the contents
              from the generic ipc_router_header_type header

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
static int ipc_router_inject_header_v3
(
  ipc_router_header_type *header, 
  ipc_router_packet_type **msg
)
{
  struct {
    ipc_router_v3_header_type rtr;
    ipc_router_mcast_opt_header opt;
  } w_header;
  uint32 len;

  w_header.rtr.version                = (uint8)IPC_ROUTER_V3_VERSION;
  w_header.rtr.type                   = (uint8)header->msg_type;
  w_header.rtr.flags                  = header->confirm_rx ? IPC_ROUTER_V3_CTRL_FLAG_CONF_RX : 0x0;
  w_header.rtr.msg_size               = header->msg_size;
  w_header.rtr.src_addr.processor_id  = (uint16)header->src_addr.processor_id;
  w_header.rtr.src_addr.port_id       = (uint16)header->src_addr.port_id;

  /* Use the optional header only if we cannot fit the dest address in the 
   * router header's dest_addr field */
  if(header->msg_type == IPC_ROUTER_CONTROL_MCAST_DATA && 
      (header->dest_addr.processor_id > 0xffff ||
      header->dest_addr.port_id > 0xffff))
  {
    w_header.rtr.dest_addr.processor_id = 0xffff;
    w_header.rtr.dest_addr.port_id = 0xffff;
    MCAST_OPT_HDR_INIT(w_header.opt, header->dest_addr.processor_id, header->dest_addr.port_id);
    w_header.rtr.opt_hdr_len = 3;
    len = sizeof(w_header);
  }
  else
  {
    w_header.rtr.dest_addr.processor_id = (uint16)header->dest_addr.processor_id;
    w_header.rtr.dest_addr.port_id      = (uint16)header->dest_addr.port_id;
    w_header.rtr.opt_hdr_len = 0;
    len = sizeof(w_header.rtr);
  }

  if(ipc_router_packet_copy_header(msg, &w_header, len) != len)
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }
  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_extract_header_v3

DESCRIPTION   Extracts a V3 header (If present at the start of the packet) 
              and puts the contents in the generic ipc_router_header_type
              if successful.

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
static int ipc_router_extract_header_v3
(
  ipc_router_packet_type **msg, 
  ipc_router_header_type *header
)
{
  ipc_router_v3_header_type w_header;
  ipc_router_v3_optional_header_type opt_header;
  boolean optional_present = FALSE;
  uint32 junk;

  if(ipc_router_packet_read_header(msg, &w_header, sizeof(w_header)) != sizeof(w_header))
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }

  header->version                = IPC_ROUTER_V3_VERSION;
  header->confirm_rx             = w_header.flags & IPC_ROUTER_V3_CTRL_FLAG_CONF_RX ? 1 : 0;
  header->msg_type               = (uint32)w_header.type;
  header->msg_size               = w_header.msg_size;
  header->src_addr.processor_id  = (uint32)w_header.src_addr.processor_id;
  header->src_addr.port_id       = (uint32)w_header.src_addr.port_id;
  header->dest_addr.processor_id = (uint32)w_header.dest_addr.processor_id;
  header->dest_addr.port_id      = (uint32)w_header.dest_addr.port_id;
  optional_present               = w_header.opt_hdr_len ? TRUE : FALSE;

  /* Discard optional header */
  while(optional_present)
  {
    if(ipc_router_packet_read_header(msg, &opt_header, sizeof(opt_header)) 
        != sizeof(opt_header))
    {
      return IPC_ROUTER_STATUS_NO_MEM;
    }
    if(opt_header.len < 1)
    {
      return IPC_ROUTER_STATUS_INVALID_MEM;
    }

    opt_header.len--; /* We have already extracted 1 word */
    w_header.opt_hdr_len--; // Decrement the optional header_len in the main header too,
                            // so that we know whether there are any more optional headers
                            // after processing the current one.

    /* We expect an mcast header and this is the expected one */
    if(opt_header.type == IPC_ROUTER_CONTROL_MCAST_DATA && 
        header->msg_type == IPC_ROUTER_CONTROL_MCAST_DATA && opt_header.len >= 2)
    {
      if(ipc_router_packet_read_header(msg, &header->dest_addr, 8) != 8)
      {
        return IPC_ROUTER_STATUS_NO_MEM;
      }
      opt_header.len -= 2;
      w_header.opt_hdr_len -= 2;
    }

    optional_present = w_header.opt_hdr_len ? TRUE : FALSE;

    /* Discard the whole optional header */
    while(opt_header.len > 0)
    {
      if(ipc_router_packet_read_header(msg, &junk, sizeof(junk)) != sizeof(junk))
      {
        return IPC_ROUTER_STATUS_NO_MEM;
      }
      opt_header.len--;
      w_header.opt_hdr_len --;
    }
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_peek_size_v3

DESCRIPTION   Peeks into the header of the packet and returns the message
              size.

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
static int ipc_router_peek_size_v3
(
  ipc_router_packet_type *msg,
  uint32 *packet_size
)
{
  ipc_router_v3_header_type *hdr;
  uint32 len;

  hdr = (ipc_router_v3_header_type *)ipc_router_packet_peek(msg, sizeof(*hdr));
  if(!hdr)
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }
  len = hdr->msg_size;
  len += PAD_LEN(len) + sizeof(ipc_router_v3_header_type) + (hdr->opt_hdr_len*4);
  *packet_size = len;

  return IPC_ROUTER_STATUS_SUCCESS;
}


/*===========================================================================
FUNCTION      ipc_router_protocol_inject_header

DESCRIPTION   Inject provided header into start of message

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
int ipc_router_protocol_inject_header
(
  ipc_router_header_type *header,
  ipc_router_packet_type **msg
)
{
  int rc = IPC_ROUTER_STATUS_FAILURE;
  if(!msg || !*msg || !header)
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }
  switch(header->version)
  {
    case IPC_ROUTER_V1_VERSION:
      rc = ipc_router_inject_header_v1(header, msg);
      break;
    case IPC_ROUTER_V3_VERSION:
      rc = ipc_router_inject_header_v3(header, msg);
      break;
    default:
      rc = IPC_ROUTER_STATUS_FAILURE;
  }
  return rc;
}


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
)
{
  uint32 version;
  int rc = IPC_ROUTER_STATUS_FAILURE;
  if(!msg || !*msg || !header)
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }
  version = ipc_router_peek_version(*msg);
  switch(version)
  {
    case IPC_ROUTER_V1_VERSION:
      rc = ipc_router_extract_header_v1(msg, header);
      break;
    case IPC_ROUTER_V3_VERSION:
      rc = ipc_router_extract_header_v3(msg, header);
      break;
    default:
      rc = IPC_ROUTER_STATUS_FAILURE;
  }
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_protocol_peek_size

DESCRIPTION   Peeks into the header of the packet and returns the message
              size.

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
int ipc_router_protocol_peek_size
(
  ipc_router_packet_type *msg,
  uint32 *packet_size
)
{
  uint32 version;
  int rc = IPC_ROUTER_STATUS_FAILURE;
  if(!msg || !packet_size)
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }
  version = ipc_router_peek_version(msg);
  switch(version)
  {
    case IPC_ROUTER_V1_VERSION:
      rc = ipc_router_peek_size_v1(msg, packet_size);
      break;
    case IPC_ROUTER_V3_VERSION:
      rc = ipc_router_peek_size_v3(msg, packet_size);
      break;
    default:
      rc = IPC_ROUTER_STATUS_FAILURE;
  }
  return rc;
}
