/***********************************************************************
 * fs_os_posix.h
 *
 * FS Posix abstraction
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/fs_osal/inc/fs_os_posix.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-02-06   dks   Create

===========================================================================*/

#ifndef __FS_OS_POSIX_H__
#define __FS_OS_POSIX_H__

#include "fs_os_config_i.h"

#ifdef FEATURE_FS_OS_FOR_POSIX

#include "pthread.h"
#include "errno.h"

#ifdef FS_OS_SIMULATOR_BUILD
  #include <limits.h>
  #define FS_OS_THREAD_DEFAULT_STACK_SIZE PTHREAD_STACK_MIN
#else
  #define FS_OS_THREAD_DEFAULT_STACK_SIZE 4096
#endif

typedef pthread_t fs_os_thread_t;
typedef pthread_mutex_t fs_os_mutex_t;
typedef void* fs_os_thread_return_type;

#define fs_os_thread_exit( )      \
  do                              \
  {                               \
    pthread_exit((void *)NULL);   \
    return NULL;                  \
  } while (0)

#endif /* FEATURE_FS_OS_FOR_POSIX */

#endif /* not __FS_OS_POSIX_H__ */
