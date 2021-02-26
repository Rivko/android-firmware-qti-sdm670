#ifndef __APR_THREAD_H__
#define __APR_THREAD_H__

/*
  Copyright (C) 2009-2010 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/osal/inc/apr_thread.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef int32_t ( *apr_thread_fn_t ) ( void* param );

typedef void* apr_thread_t;

APR_INTERNAL int32_t apr_thread_create (
  apr_thread_t* ret_thread,
  char_t* name,
  uint32_t priority,
  uint8_t* stack_base,
  uint32_t stack_size,
  apr_thread_fn_t thread_fn,
  void* thread_param
);
APR_INTERNAL int32_t apr_thread_destroy (
  apr_thread_t thread
);

#endif /* __APR_THREAD_H__ */

