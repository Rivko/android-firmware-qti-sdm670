#ifndef __VOCSVC_NV_API_H__
#define __VOCSVC_NV_API_H__

/*
   Copyright (C) 2012 Qualcomm Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/protected/vocsvc_nv_api.h#1 $
   $Author: pwbldsvc $
*/

#include "apr_comdef.h"

#define VOCSVC_NV_MAJOR_VERSION ( 3 )
#define VOCSVC_NV_MINOR_VERSION ( 0 )

typedef struct vocsvc_nv_eamr_config_t
{
  bool_t enable_2g;
  bool_t enable_3g;
}
  vocsvc_nv_eamr_config_t;

typedef enum vocsvc_nv_callindex_enum_t
{
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_INIT = 1,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_POSTINIT = 2,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_PREDEINIT = 3,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_DEINIT = 4,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_GET_EAMR_CONFIG = 5,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_CREATE_NV_FILE = 6,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_INVALID
}
  vocsvc_nv_callindex_enum_t;

APR_EXTERNAL int32_t vocsvc_nv_call
(
  vocsvc_nv_callindex_enum_t index,
  void* params,
  uint32_t size
);

#endif  /* __VOCSVC_NV_API_H__ */

