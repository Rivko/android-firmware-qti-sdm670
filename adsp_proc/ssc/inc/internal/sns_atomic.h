#pragma once
/*=============================================================================
  @file sns_atomic.h

  Atomic function support for both Hexagon and simulation.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

#if defined(SSC_TARGET_HEXAGON)

#include "qurt_atomic_ops.h"

#define atomic_init( ptr, val ) *ptr = val
#define atomic_compare_exchange_strong( ptr, old_val, new_val ) qurt_atomic_compare_and_set( ptr, *old_val, new_val )
#define atomic_store( ptr, val ) qurt_atomic_set( ptr, val )
#define atomic_load( ptr ) *ptr
#define atomic_fetch_add( ptr, val ) qurt_atomic_add_return( ptr, val ) - val
#define atomic_fetch_sub( ptr, val ) qurt_atomic_sub_return( ptr, val ) + val
#define _Atomic

#else

#include <stdatomic.h>

#endif /* defined(SSC_TARGET_HEXAGON) */
