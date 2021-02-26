/***********************************************************************
 * fs_os.h
 *
 * OS abstraction layer for all Filesystem Modules.
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/fs_osal/inc/fs_os.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-02-06   dks   Create

===========================================================================*/

#ifndef __FS_OS_H__
#define __FS_OS_H__

#include "fs_os_config_i.h"
#include "comdef.h"
#include "qmi_client.h"

#if defined (FEATURE_FS_OS_FOR_QURT)
  #include "fs_os_qurt.h"
#elif  defined (FEATURE_FS_OS_FOR_POSIX)
  #include "fs_os_posix.h"
#endif

typedef struct
{
  uint32 stack_size;
  char *thread_name;
} fs_os_thread_attr_t;

typedef qmi_client_os_params fs_os_qmi_client_os_params;

void fs_os_mutex_init (fs_os_mutex_t *hdl_ptr);

void fs_os_mutex_lock (fs_os_mutex_t *hdl_ptr);

void fs_os_mutex_unlock (fs_os_mutex_t *hdl_ptr);

void fs_os_thread_attr_init (fs_os_thread_attr_t *thread_attr);

int fs_os_thread_create (fs_os_thread_t *thread, fs_os_thread_attr_t *attr,
                         fs_os_thread_return_type (*thread_start) (void *),
                         void *args);

fs_os_thread_t fs_os_thread_self (void);

int fs_os_thread_join (fs_os_thread_t thread_handle, void **value_ptr);

#endif /* not __FS_OSAL_H__ */

