 /***********************************************************************
 * time_osal_qurt.h
 * This file maps the timer osal types to native QURT types.
 *
 * Copyright (C) 2012 Qualcomm Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
===========================================================================*/

#ifndef __TIME_OSAL_QURT_H__
#define __TIME_OSAL_QURT_H__


#include "qurt.h"

typedef void *                           time_osal_notify_obj_ptr;

typedef unsigned long int                time_osal_notify_data;

typedef qurt_thread_t					     time_osal_thread_id;

#endif /* __TIME_OSAL_QURT_H__*/

