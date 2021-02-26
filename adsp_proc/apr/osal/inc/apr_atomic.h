#ifndef __APR_ATOMIC_H__
#define __APR_ATOMIC_H__

/*
  Copyright (C) 2010 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/osal/inc/apr_atomic.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

APR_INTERNAL int32_t apr_atomic_init ( void );
APR_INTERNAL int32_t apr_atomic_deinit ( void );

APR_INTERNAL int32_t apr_atomic_int16_add (
  volatile int16_t* const variable,
  const int16_t value,
  int16_t* const ret_result
);
APR_INTERNAL int32_t apr_atomic_int16_cmpxchg (
  volatile int16_t* const variable,
  const int16_t compare,
  const int16_t value,
  int16_t* const ret_result
);
APR_INTERNAL int32_t apr_atomic_int32_add (
  volatile int32_t* const variable,
  const int32_t value,
  int32_t* const ret_result
);
APR_INTERNAL int32_t apr_atomic_int32_cmpxchg (
  volatile int32_t* const variable,
  const int32_t compare,
  const int32_t value,
  int32_t* const ret_result
);

#endif /* __APR_ATOMIC_H__ */

