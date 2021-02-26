#ifndef __APR_LOCK_H__
#define __APR_LOCK_H__

/*
  Copyright (C) 2009-2010 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/osal/inc/apr_lock.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef void* apr_lock_t;

#define APR_LOCK_TYPE_MUTEX     ( 0 )
#define APR_LOCK_TYPE_INTERRUPT ( 1 )

APR_INTERNAL int32_t apr_lock_create ( uint32_t lock_type, apr_lock_t* ret_lock );
APR_INTERNAL int32_t apr_lock_destroy ( apr_lock_t lock );
APR_INTERNAL int32_t apr_lock_enter ( apr_lock_t lock );
APR_INTERNAL int32_t apr_lock_leave ( apr_lock_t lock );

#endif /* __APR_LOCK_H__ */

