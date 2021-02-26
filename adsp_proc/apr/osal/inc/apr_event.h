#ifndef __APR_EVENT_H__
#define __APR_EVENT_H__

/*
  Copyright (C) 2009-2010 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/osal/inc/apr_event.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef void* apr_event_t;

APR_INTERNAL int32_t apr_event_create ( apr_event_t* ret_event );
APR_INTERNAL int32_t apr_event_destroy ( apr_event_t event );
APR_INTERNAL int32_t apr_event_wait ( apr_event_t event );
APR_INTERNAL int32_t apr_event_signal ( apr_event_t event );
APR_INTERNAL int32_t apr_event_signal_abortall ( apr_event_t event );
APR_INTERNAL int32_t apr_event_cancel_abortall ( apr_event_t event );

#endif /* __APR_EVENT_H__ */

