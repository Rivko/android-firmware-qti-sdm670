/***********************************************************************
 * fs_os_posix.c
 *
 * FS Posix Abstraction
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/fs_osal/src/fs_os_posix.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-02-06   dks   Create

===========================================================================*/

#include "fs_os_config_i.h"
#include "fs_os.h"

#ifdef FEATURE_FS_OS_FOR_POSIX

#include "fs_os_err.h"
#include "qmi_client.h"
#include <stdio.h>
#include <string.h>
#include "errno.h"
#include "pthread.h"

#ifdef FS_OS_SIMULATOR_BUILD
  extern int pthread_attr_setthreadname(pthread_attr_t *attr, char *name);
#endif

void
fs_os_mutex_init (fs_os_mutex_t *hdl_ptr)
{
  int result;
  pthread_mutexattr_t attr;

  FS_OS_ASSERT (hdl_ptr != NULL);

  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

  result = pthread_mutex_init ((pthread_mutex_t *)hdl_ptr, &attr);
  if (result != 0)
  {
    FS_OS_ERR_FATAL ("pthread_mutex_init failed %d,%d,%d", result, 0, 0);
  }

  pthread_mutexattr_destroy (&attr);
}

void
fs_os_mutex_lock (fs_os_mutex_t *hdl_ptr)
{
  int result;

  FS_OS_ASSERT (hdl_ptr != NULL);

  result = pthread_mutex_lock ((pthread_mutex_t *)hdl_ptr);
  if (result != 0)
  {
    FS_OS_ERR_FATAL ("fs_os_mutex_lock failed %d,%d,%d", result, 0, 0);
  }
}

void
fs_os_mutex_unlock (fs_os_mutex_t *hdl_ptr)
{
  int result;

  FS_OS_ASSERT (hdl_ptr != NULL);

  result = pthread_mutex_unlock ((pthread_mutex_t *)hdl_ptr);
  if (result != 0)
  {
    FS_OS_ERR_FATAL ("fs_os_mutex_unlock failed %d,%d,%d", result, 0, 0);
  }
}

void
fs_os_thread_attr_init (fs_os_thread_attr_t *thread_attr)
{
  FS_OS_ASSERT (thread_attr);

  memset (thread_attr, 0x0, sizeof (fs_os_thread_attr_t));
}

int
fs_os_thread_create(fs_os_thread_t *thread, fs_os_thread_attr_t *attr,
                    fs_os_thread_return_type (*thread_start) (void *),
                    void *args)
{
  int result;
  pthread_attr_t posix_attr;

  FS_OS_ASSERT (thread != NULL);
  FS_OS_ASSERT (thread_start != NULL);
  FS_OS_ASSERT (attr != NULL);

  result = pthread_attr_init (&posix_attr);
  if (result != 0)
  {
    return result;
  }

  if (attr->thread_name != NULL)
  {
    result = pthread_attr_setthreadname (&posix_attr, attr->thread_name);
    if (result != 0)
    {
      goto End;
    }
  }

  if (attr->stack_size != 0)
  {
    result = pthread_attr_setstacksize (&posix_attr, attr->stack_size);
    if (result != 0)
    {
      goto End;
    }
  }
#ifdef FEATURE_FS_OS_FOR_POSIX_ON_TN
  pthread_attr_setdetachstate (&posix_attr, 1);
#endif

  result = pthread_create (thread, &posix_attr, thread_start, args);
  if (result != 0)
  {
    goto End;
  }

  result = 0;

End:
  (void) pthread_attr_destroy (&posix_attr);
  return result;
}

fs_os_thread_t
fs_os_thread_self (void)
{
  pthread_t thread_handle = (pthread_t) NULL;

  thread_handle = pthread_self ();

  return (fs_os_thread_t) thread_handle;
}

int
fs_os_thread_join (fs_os_thread_t thread_handle, void **value_ptr)
{
  (void) thread_handle;
  (void) value_ptr;
#ifndef FEATURE_FS_OS_FOR_POSIX_ON_TN // not detached pthread
  pthread_join (thread_handle, value_ptr);
#endif
  return 0;
}

#endif

