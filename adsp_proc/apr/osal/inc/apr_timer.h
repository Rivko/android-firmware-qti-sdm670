#ifndef __APR_TIMER_H__
#define __APR_TIMER_H__

/*
  Copyright (C) 2009-2010 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/osal/inc/apr_timer.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef void ( *apr_timer_fn_t ) ( void* client_token );

typedef void* apr_timer_t;

APR_INTERNAL int32_t apr_timer_create (
  apr_timer_t* ret_timer,
  apr_timer_fn_t timer_cb,
  void* client_token
);

APR_INTERNAL int32_t apr_timer_destroy (
  apr_timer_t timer
);

APR_INTERNAL int32_t apr_timer_start (
  apr_timer_t timer,
  uint64_t time_ns
);

APR_INTERNAL int32_t apr_timer_stop (
  apr_timer_t timer
);

#endif /* __APR_TIMER_H__ */

