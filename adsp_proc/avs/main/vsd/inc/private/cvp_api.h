#ifndef __CVP_API_H__
#define __CVP_API_H__

/*
   Copyright (C) 2009 Qualcomm Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvp_api.h#1 $
   $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef enum cvp_callindex_enum_t
{
  /* public */  CVP_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */  CVP_CALLINDEX_ENUM_INIT = 1,
  /* public */  CVP_CALLINDEX_ENUM_POSTINIT = 2,
  /* public */  CVP_CALLINDEX_ENUM_PREDEINIT = 3,
  /* public */  CVP_CALLINDEX_ENUM_DEINIT = 4,
  /* private */ CVP_CALLINDEX_ENUM_RUN = 5,
  /* public */  CVP_CALLINDEX_ENUM_INVALID
}
  cvp_callindex_enum_t;

APR_EXTERNAL int32_t cvp_call (
  cvp_callindex_enum_t index,
  void* params,
  uint32_t size
);

#endif /* __CVP_API_H__ */

