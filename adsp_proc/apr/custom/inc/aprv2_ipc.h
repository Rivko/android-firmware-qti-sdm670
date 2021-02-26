#ifndef __APRV2_IPC_H__
#define __APRV2_IPC_H__
/*=============================================================================
  Copyright (C) 2010-2016 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

===============================================================================
                                 Edit History
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia2/apr/apr2/mainline/main/latest/datalink/apr_glink/src/apr_glink.c
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ------------------------------------------------------------

=============================================================================*/
#include "apr_comdef.h"
#include "aprv2_packet.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/
#define APR_IPC_PACKET_SIZE (4096)

APR_INTERNAL int32_t aprv2_ipc_init ( void );
APR_INTERNAL int32_t aprv2_ipc_deinit ( void );
APR_INTERNAL int32_t aprv2_ipc_is_domain_local ( uint16_t domain_id );
APR_INTERNAL int32_t aprv2_ipc_send ( aprv2_packet_t* packet );
APR_INTERNAL int32_t aprv2_ipc_get_default_domain_id ( uint16_t *ret_domain_id );
APR_INTERNAL int32_t aprv2_ipc_free ( aprv2_packet_t* packet );



#endif /* __APRV2_IPC_I_H__ */
