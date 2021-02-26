#ifndef __TOYSERVER_API_H__
#define __TOYSERVER_API_H__

/*
  Copyright (C) 2011 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/example/toyserver/inc/toyserver_api.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

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

APR_EXTERNAL int32_t toyserver_call (
  toyserver_callindex_enum_t index,
  void* params,
  uint32_t size
);

#endif /* __TOYSERVER_API_H__ */

