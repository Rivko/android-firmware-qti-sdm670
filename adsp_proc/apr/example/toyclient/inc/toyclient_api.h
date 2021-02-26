#ifndef __TOYCLIENT_API_H__
#define __TOYCLIENT_API_H__

/*
  Copyright (C) 2011 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/example/toyclient/inc/toyclient_api.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef enum toyclient_callindex_enum_t
{
  /* public */ TOYCLIENT_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */ TOYCLIENT_CALLINDEX_ENUM_INIT = 1,
  /* public */ TOYCLIENT_CALLINDEX_ENUM_POSTINIT = 2,
  /* public */ TOYCLIENT_CALLINDEX_ENUM_PREDEINIT = 3,
  /* public */ TOYCLIENT_CALLINDEX_ENUM_DEINIT = 4,
  /* public */ TOYCLIENT_CALLINDEX_ENUM_RUN = 5,
  /* public */ TOYCLIENT_CALLINDEX_ENUM_INVALID
}
  toyclient_callindex_enum_t;

APR_EXTERNAL int32_t toyclient_call (
  toyclient_callindex_enum_t index,
  void* params,
  uint32_t size
);

#endif /* __TOYCLIENT_API_H__ */

