#ifndef __CVS_API_H__
#define __CVS_API_H__

/*
   Copyright (C) 2009-2010 Qualcomm Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvs_api.h#1 $
   $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef enum cvs_callindex_enum_t
{
  /* public */  CVS_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */  CVS_CALLINDEX_ENUM_INIT = 1,
  /* public */  CVS_CALLINDEX_ENUM_POSTINIT = 2,
  /* public */  CVS_CALLINDEX_ENUM_PREDEINIT = 3,
  /* public */  CVS_CALLINDEX_ENUM_DEINIT = 4,
  /* private */ CVS_CALLINDEX_ENUM_RUN = 5,
  /* public */  CVS_CALLINDEX_ENUM_INVALID
}
  cvs_callindex_enum_t;

APR_EXTERNAL int32_t cvs_call (
  cvs_callindex_enum_t index,
  void* params,
  uint32_t size
);

#endif /* __CVS_API_H__ */

