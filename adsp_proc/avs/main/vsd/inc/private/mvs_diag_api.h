#ifndef __MVS_DIAG_API_H__
#define __MVS_DIAG_API_H__

/*
   Copyright (C) 2012 Qualcomm Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/mvs_diag_api.h#1 $
   $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef enum mvs_diag_callindex_enum_t
{
  /* public */ MVS_DIAG_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */ MVS_DIAG_CALLINDEX_ENUM_INIT = 1,
  /* public */ MVS_DIAG_CALLINDEX_ENUM_POSTINIT = 2,
  /* public */ MVS_DIAG_CALLINDEX_ENUM_PREDEINIT = 3,
  /* public */ MVS_DIAG_CALLINDEX_ENUM_DEINIT = 4,
  /* public */ MVS_DIAG_CALLINDEX_ENUM_INVALID
}
  mvs_diag_callindex_enum_t;

typedef PACKED struct PACKED_POST mvs_diag_voicenv_rsp_t
{
  uint16_t result;
  uint32_t bytes_written;
}
  mvs_diag_voicenv_rsp_t;
  
APR_EXTERNAL int32_t mvs_diag_call (
  mvs_diag_callindex_enum_t index,
  void* params,
  uint32_t size
);

#endif  /* MVS_DIAG_API_H */

