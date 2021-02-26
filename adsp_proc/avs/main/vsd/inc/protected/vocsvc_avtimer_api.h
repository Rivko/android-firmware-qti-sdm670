#ifndef __VOCSVC_AVTIMER_API_H__
#define __VOCSVC_AVTIMER_API_H__

/*
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/protected/vocsvc_avtimer_api.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef struct vocsvc_avtimer_timestamp_t
{
  uint64_t timestamp_us;                
}
  vocsvc_avtimer_timestamp_t;

APR_EXTERNAL int32_t vocsvc_avtimer_open_handle
(
  void
);

APR_EXTERNAL int32_t vocsvc_avtimer_close_handle
(
  void
);

APR_EXTERNAL int32_t vocsvc_avtimer_get_time
(
  vocsvc_avtimer_timestamp_t* avtimer_timestamp
);

#endif  /* __VOCSVC_AVTIMER_API_H__ */

