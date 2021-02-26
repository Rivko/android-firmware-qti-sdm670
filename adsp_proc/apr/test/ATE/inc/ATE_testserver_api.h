#ifndef __TESTSERVER_API_H__
#define __TESTSERVER_API_H__

/*
  Copyright (C) 2011 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/test/ATE/inc/ATE_testserver_api.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef struct testserver_init_param_s {
  char_t name[31]; // APRV2_RTABLE_MAX_DNS_SIZE
  uint32_t name_len;
  uint16_t addr;
} testserver_init_param;

typedef enum toyserver_callindex_enum_t
{
  /* public */ TOYSERVER_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */ TOYSERVER_CALLINDEX_ENUM_INIT = 1,
  /* public */ TOYSERVER_CALLINDEX_ENUM_POSTINIT = 2,
  /* public */ TOYSERVER_CALLINDEX_ENUM_PREDEINIT = 3,
  /* public */ TOYSERVER_CALLINDEX_ENUM_DEINIT = 4,
  /* public */ TOYSERVER_CALLINDEX_ENUM_INVALID
}
  toyserver_callindex_enum_t;

APR_EXTERNAL int32_t testserver_call (
  toyserver_callindex_enum_t index,
  void* params,
  uint32_t size
);

#endif /* __TESTSERVER_API_H__ */
