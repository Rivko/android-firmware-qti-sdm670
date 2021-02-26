#ifndef __MVS_MODULE_API_H__
#define __MVS_MODULE_API_H__

/*
   Copyright (C) 2009-2010 Qualcomm Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/mvs_module_api.h#1 $
   $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef enum mvs_callindex_enum_t
{
  /* public */  MVS_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */  MVS_CALLINDEX_ENUM_INIT = 1,
  /* public */  MVS_CALLINDEX_ENUM_POSTINIT = 2,
  /* public */  MVS_CALLINDEX_ENUM_PREDEINIT = 3,
  /* public */  MVS_CALLINDEX_ENUM_DEINIT = 4,
  /* private */ MVS_CALLINDEX_ENUM_RUN = 5,
  /* public */  MVS_CALLINDEX_ENUM_INVALID
}
  mvs_callindex_enum_t;

APR_EXTERNAL int32_t mvs_call (
  mvs_callindex_enum_t index,
  void* params,
  uint32_t size
);

#endif /* __MVS_MODULE_API_H__ */

