#ifndef __APRV2_API_I_H__
#define __APRV2_API_I_H__

/*
  Copyright (C) 2010, 2011 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/core/inc/aprv2_api_i.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "aprv2_api.h"

/*****************************************************************************
 * Core Routines                                                             *
 ****************************************************************************/

APR_INTERNAL int32_t aprv2_drv_init ( void );
APR_INTERNAL int32_t aprv2_drv_deinit ( void );

/*****************************************************************************
 * Fundamental Controls                                                      *
 ****************************************************************************/

/**
 * Registers a service with the Packet Router.
 *
 * \return APR_EOK (0) when successful.
 *
 * \retval APR_EALREADY Service has already been registered.
 */
APR_INTERNAL int32_t aprv2_cmd_register (
  aprv2_cmd_register_t* params
);

/**
 * Registers a service with the Packet Router.
 *
 * \return APR_EOK (0) when successful.
 *
 * \retval APR_EALREADY Service has already been registered.
 */
APR_INTERNAL int32_t aprv2_cmd_register2 (
  aprv2_cmd_register2_t* params
);

/**
 * Deregisters a service from the Packet Router.
 *
 * \return APR_EOK (0) when successful.
 */
APR_INTERNAL int32_t aprv2_cmd_deregister (
  aprv2_cmd_deregister_t* params
);

/**
 * Sends an asynchronous packet.
 *
 * \return APR_EOK (0) when successful.
 */
APR_INTERNAL int32_t aprv2_cmd_async_send (
  aprv2_cmd_async_send_t* params
);

/**
 * Allocates a packet from the local Packet Router.
 *
 * \return APR_EOK (0) when successful.
 */
APR_INTERNAL int32_t aprv2_cmd_alloc (
  aprv2_cmd_alloc_t* params
);

/**
 * Returns a packet back to the local Packet Router.
 *
 * \return APR_EOK (0) when successful.
 */
APR_INTERNAL int32_t aprv2_cmd_free (
  aprv2_cmd_free_t* params
);

/*****************************************************************************
 * Service Controls                                                          *
 ****************************************************************************/

/**
 * Retrieves the service address of a registered service handle.
 *
 * \return APR_EOK (0) when successful.
 */
APR_INTERNAL int32_t aprv2_cmd_query_address (
  aprv2_cmd_query_address_t* params
);

/**
 * Performs a local DNS lookup.
 *
 * \return APR_EOK (0) when successful.
 */
APR_INTERNAL int32_t aprv2_cmd_local_dns_lookup (
  aprv2_cmd_local_dns_lookup_t* params
);

/*****************************************************************************
 * Utility Controls                                                          *
 ****************************************************************************/

/**
 * Blindly forwards an asynchronous packet.
 *
 * This convenience API automatically frees the packet to deliver when any
 * errors are encountered during forwarding.
 *
 * \return APR_EOK (0) when successful.
 *
 * \retval APR_EBADPARAM (2) when the input parameter is invalid. The packet
 *         to deliver is not freed.
 */
APR_INTERNAL int32_t aprv2_cmd_forward (
  aprv2_cmd_forward_t* params
);

/**
 * Allocates a formatted free packet from the local Packet Router.
 *
 * The packet allocation type is based on the msg_type. Command responses
 * allocate using APRV2_ALLOC_TYPE_RESPONSE. Events and commands allocate
 * using APRV2_ALLOC_TYPE_COMMAND.
 *
 * \return APR_EOK (0) when successful.
 */
APR_INTERNAL int32_t aprv2_cmd_alloc_ext (
  aprv2_cmd_alloc_ext_t* params
);

/**
 * Allocates and sends a formatted free packet from the local Packet Router.
 *
 * The packet allocation type is based on the msg_type. Command responses
 * allocate using APRV2_ALLOC_TYPE_RESPONSE. Events and commands allocate
 * using APRV2_ALLOC_TYPE_COMMAND.
 *
 * \return APR_EOK (0) when successful.
 */
APR_INTERNAL int32_t aprv2_cmd_alloc_send (
  aprv2_cmd_alloc_send_t* params
);

/**
 * Accepts a command packet.
 *
 * This convenience API sends an APRV2_IBASIC_EVT_ACCEPTED event back to the
 * client. The routing information needed is extracted from the indicated
 * command packet.
 *
 * \return APR_EOK (0) when successful.
 */
APR_INTERNAL int32_t aprv2_cmd_accept_command (
  aprv2_cmd_accept_command_t* params
);

/**
 * Ends a command packet.
 *
 * This convenience API both sends an APRV2_IBASIC_RSP_RESULT command response
 * back to the client and frees the indicated command packet. The routing
 * information needed is extracted from the indicated command packet.
 *
 * \return APR_EOK (0) when successful.
 *
 * \retval APR_EBADPARAM (2) when the input parameter is invalid. The packet
 *         to deliver is not freed.
 */
APR_INTERNAL int32_t aprv2_cmd_end_command (
  aprv2_cmd_end_command_t* params
);

#endif /* __APRV2_API_I_H__ */

