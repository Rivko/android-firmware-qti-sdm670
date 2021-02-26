#ifndef __APRV2_API_INLINE_H__
#define __APRV2_API_INLINE_H__

/** @file aprv2_api_inline.h
*/

/*===========================================================================
Copyright (c) 2010-2012 QUALCOMM Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - QUALCOMM Technologies, Inc.
======================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/apr.common/2.0/api/aprv2_api_inline.h#1 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2012/04/16 sw      (Tech Pubs) Removed Doxygen comments because of revised
                   organization. Comments are in the #defines.
2011/11/18 sw      (Tech Pubs) Added Doxygen comments for Interface Spec.
===========================================================================*/

#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "aprv2_api.h"
#include "aprv2_msg_if.h"

/*
 * Inline APIs allows forward compatible linking.
 */

/*****************************************************************************
 * Inline Fundamental Controls                                               *
 ****************************************************************************/

static inline int32_t __aprv2_cmd_register (
  uint32_t* ret_handle,
  uint16_t addr,
  aprv2_dispatch_fn_t dispatch_fn,
  void* dispatch_data
)
{
  aprv2_cmd_register_t args;

  args.ret_handle = ret_handle;
  args.addr = addr;
  args.dispatch_fn = dispatch_fn;
  args.dispatch_data = dispatch_data;

  return apr_call( APRV2_CMDID_REGISTER, &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_register2 (
  uint32_t* ret_handle,
  char_t* name,
  uint32_t name_size,
  uint16_t addr,
  aprv2_dispatch_fn_t dispatch_fn,
  void* dispatch_data,
  uint16_t* ret_addr
)
{
  aprv2_cmd_register2_t args;

  args.ret_handle = ret_handle;
  args.name = name;
  args.name_size = name_size;
  args.addr = addr;
  args.dispatch_fn = dispatch_fn;
  args.dispatch_data = dispatch_data;
  args.ret_addr = ret_addr;

  return apr_call( APRV2_CMDID_REGISTER2, &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_deregister (
  uint32_t handle
)
{
  aprv2_cmd_deregister_t args;

  args.handle = handle;

  return apr_call( APRV2_CMDID_DEREGISTER, &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_async_send (
  uint32_t handle,
  aprv2_packet_t* packet
)
{
  aprv2_cmd_async_send_t args;

  args.handle = handle;
  args.packet = packet;

  return apr_call( APRV2_CMDID_ASYNC_SEND, &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_alloc (
  uint32_t handle,
  uint32_t alloc_type,
  uint32_t alloc_size,
  aprv2_packet_t** ret_packet
)
{
  aprv2_cmd_alloc_t args;

  args.handle = handle;
  args.alloc_type = alloc_type;
  args.alloc_size = alloc_size;
  args.ret_packet = ret_packet;

  return apr_call( APRV2_CMDID_ALLOC, &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_free (
  uint32_t handle,
  aprv2_packet_t* packet
)
{
  aprv2_cmd_free_t args;

  args.handle = handle;
  args.packet = packet;

  return apr_call( APRV2_CMDID_FREE, &args, sizeof( args ) );
}

/*****************************************************************************
 * Inline Service Controls                                                   *
 ****************************************************************************/

static inline int32_t __aprv2_cmd_query_address (
  uint32_t handle,
  uint16_t* ret_addr
)
{
  aprv2_cmd_query_address_t args;

  args.handle = handle;
  args.ret_addr = ret_addr;

  return apr_call( APRV2_CMDID_QUERY_ADDRESS, &args, sizeof( args ) );
}

/* Returns the service address of an indicated service name string. */
static inline int32_t __aprv2_cmd_local_dns_lookup (
  char_t* name,
  uint32_t name_size,
  uint16_t* ret_addr
)
{
  aprv2_cmd_local_dns_lookup_t args;

  args.name = name;
  args.name_size = name_size;
  args.ret_addr = ret_addr;

  return apr_call( APRV2_CMDID_LOCAL_DNS_LOOKUP, &args, sizeof( args ) );
}

/*****************************************************************************
 * Inline Utility Controls                                                   *
 ****************************************************************************/

static inline int32_t __aprv2_cmd_forward (
  uint32_t handle,
  aprv2_packet_t* packet
)
{
  aprv2_cmd_forward_t args;

  args.handle = handle;
  args.packet = packet;

  return apr_call( APRV2_CMDID_FORWARD, &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_alloc_ext (
  uint32_t handle,
  uint8_t msg_type,
  uint16_t src_addr,
  uint16_t src_port,
  uint16_t dst_addr,
  uint16_t dst_port,
  uint32_t token,
  uint32_t opcode,
  uint32_t payload_size,
  aprv2_packet_t** ret_packet
)
{
  aprv2_cmd_alloc_ext_t args;

  args.handle = handle;
  args.msg_type = msg_type;
  args.src_addr = src_addr;
  args.src_port = src_port;
  args.dst_addr = dst_addr;
  args.dst_port = dst_port;
  args.token = token;
  args.opcode = opcode;
  args.payload_size = payload_size;
  args.ret_packet = ret_packet;

  return apr_call( APRV2_CMDID_ALLOC_EXT, &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_alloc_send (
  uint32_t handle,
  uint8_t msg_type,
  uint16_t src_addr,
  uint16_t src_port,
  uint16_t dst_addr,
  uint16_t dst_port,
  uint32_t token,
  uint32_t opcode,
  void* payload,
  uint32_t payload_size
)
{
  aprv2_cmd_alloc_send_t args;

  args.handle = handle;
  args.msg_type = msg_type;
  args.src_addr = src_addr;
  args.src_port = src_port;
  args.dst_addr = dst_addr;
  args.dst_port = dst_port;
  args.token = token;
  args.opcode = opcode;
  args.payload = payload;
  args.payload_size = payload_size;

  return apr_call( APRV2_CMDID_ALLOC_SEND, &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_accept_command (
  uint32_t handle,
  aprv2_packet_t* packet
)
{
  aprv2_cmd_accept_command_t args;

  args.handle = handle;
  args.packet = packet;

  return apr_call( APRV2_CMDID_ACCEPT_COMMAND, &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_end_command (
  uint32_t handle,
  aprv2_packet_t* packet,
  uint32_t status
)
{
  aprv2_cmd_end_command_t args;

  args.handle = handle;
  args.packet = packet;
  args.status = status;

  return apr_call( APRV2_CMDID_END_COMMAND, &args, sizeof( args ) );
}

#endif /* __APRV2_API_INLINE_H__ */

