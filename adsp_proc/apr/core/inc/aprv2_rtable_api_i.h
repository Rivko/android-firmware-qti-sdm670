#ifndef __APRV2_RTABLE_API_I_H__
#define __APRV2_RTABLE_API_I_H__

/*
  Copyright (C) 2010, 2011 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/core/inc/aprv2_rtable_api_i.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "aprv2_ids_domains.h"
#include "aprv2_ids_services.h"
#include "aprv2_api.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

#define APRV2_RTABLE_MAX_DNS_SIZE ( 31 )
  /* Includes NULL character. */

/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

typedef struct aprv2_rtable_service_entry_t aprv2_rtable_service_entry_t;
struct aprv2_rtable_service_entry_t
{
  aprv2_dispatch_fn_t dispatch_fn;
  void* dispatch_data;
  char_t name[ APRV2_RTABLE_MAX_DNS_SIZE ];
};

/*****************************************************************************
 * Prototypes                                                                *
 ****************************************************************************/

APR_INTERNAL int32_t aprv2_rtable_get_default_domain (
  uint16_t* ret_domain_id
);

APR_INTERNAL int32_t aprv2_rtable_get_num_total_domains (
  uint16_t* ret_count
);

APR_INTERNAL int32_t aprv2_rtable_get_num_static_services (
  uint16_t domain_id,
  uint16_t* ret_count
);

APR_INTERNAL int32_t aprv2_rtable_get_num_total_services (
  uint16_t domain_id,
  uint16_t* ret_count
);

APR_INTERNAL int32_t aprv2_rtable_local_dns_lookup (
  char_t* name,
  uint32_t size,
  uint16_t* ret_addr
);

APR_INTERNAL int32_t aprv2_rtable_get_service (
  uint16_t domain_id,
  uint16_t service_id,
  aprv2_rtable_service_entry_t** ret_entry
);

APR_INTERNAL int32_t aprv2_rtable_init (
  aprv2_dispatch_fn_t default_dispatch_fn,
  void* default_dispatch_data
);

APR_INTERNAL int32_t aprv2_rtable_deinit ( void );

#endif /* __APRV2_RTABLE_API_I_H__ */

