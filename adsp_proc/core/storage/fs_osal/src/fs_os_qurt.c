/***********************************************************************
 * fs_os_qurt.c
 *
 * FS Qurt Abstraction
 * Copyright (C) 2015-2016 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/fs_osal/src/fs_os_qurt.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-10-08   as    Migrate from qurt rmutex api to qurt pimutex api
2015-02-06   dks   Create

===========================================================================*/

#include "fs_os_config_i.h"
#include "fs_os.h"

#ifdef FEATURE_FS_OS_FOR_QURT

#error "Qurt wrapper for fs_osal is incomplete. Use Posix"
/* The Qurt implementation of fs_osal is currently incomplete because efs_errno
   cannot be implemented in a clean fashion. When EFS library is present, EFS
   provides efs_get_errno_addr implemenation and fs_osal cannot redefine it.
   EFS library's efs_get_errno_addr implementation uses rex tcb's err_num
   field. When a thread is created using fs_osal's qurt implementation, the
   qurt thread will fault if tries to access it's tcb and the err_num field.
   Enable fs_osal on Qurt only after efs_get_errno_addr can be implementd
   properly */

#include "fs_os_qurt.h"
#include "fs_os_err.h"
#include "fs_os_malloc.h"
#include "fs_os_msg.h"
#include "qurt_error.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

void
fs_os_mutex_init (fs_os_mutex_t *hdl_ptr)
{
  FS_OS_ASSERT (hdl_ptr != NULL);
  qurt_pimutex_init ((qurt_pimutex_t *)hdl_ptr);
}

void
fs_os_mutex_lock (fs_os_mutex_t *hdl_ptr)
{
  FS_OS_ASSERT (hdl_ptr != NULL);
  qurt_pimutex_lock ((qurt_pimutex_t *)hdl_ptr);
}

void
fs_os_mutex_unlock (fs_os_mutex_t *hdl_ptr)
{
  FS_OS_ASSERT (hdl_ptr != NULL);
  qurt_pimutex_unlock ((qurt_pimutex_t *)hdl_ptr);
}

void
fs_os_thread_attr_init (fs_os_thread_attr_t *thread_attr)
{
  FS_OS_ASSERT (thread_attr);
  memset (thread_attr, 0x0, sizeof (fs_os_thread_attr_t));
}

int
fs_os_thread_create (fs_os_thread_t *thread, fs_os_thread_attr_t *attr,
                     fs_os_thread_return_type (*thread_start) (void *),
                     void *args)
{
  int result;
  qurt_thread_attr_t qurt_attr;
  void *stack_addr = NULL;

  FS_OS_ASSERT (thread != NULL);
  FS_OS_ASSERT (thread_start != NULL);
  FS_OS_ASSERT (attr != NULL);
  FS_OS_ASSERT (attr->stack_size > 0);

  qurt_thread_attr_init (&qurt_attr);

  if (attr->thread_name != NULL)
  {
    qurt_thread_attr_set_name (&qurt_attr, attr->thread_name);
  }

  /* Qurt needs stack to be specified by caller. */
  stack_addr = fs_os_malloc (attr->stack_size);
  if (stack_addr == NULL)
  {
    FS_OS_MSG_ERROR_1 ("%d, malloc failed", attr->stack_size);
    return ENOMEM;
  }

  qurt_thread_attr_set_stack_size (&qurt_attr, attr->stack_size);
  qurt_thread_attr_set_stack_addr (&qurt_attr, stack_addr);
  qurt_thread_attr_set_priority (&qurt_attr, FS_OS_QURT_DEFAULT_PRIORITY);

  result = qurt_thread_create (thread, &qurt_attr, thread_start, args);
  if (result != QURT_EOK)
  {
    FS_OS_MSG_ERROR_1 ("qurt_thread_create failed %d", result);
    result = result;
    goto End;
  }

  result = 0;

End:
  return result;
}

fs_os_thread_t
fs_os_thread_self (void)
{
  fs_os_thread_t thread_handle = (fs_os_thread_t) NULL;

  thread_handle = qurt_thread_get_id ();

  return thread_handle;
}

static int
fs_os_qurt_attr_getstackaddr (qurt_thread_attr_t *attr, void** stack_addr)
{
  FS_OS_ASSERT (attr != NULL);
  FS_OS_ASSERT (stack_addr != NULL);
  if (attr == NULL || stack_addr == NULL)
  {
    return EINVAL;
  }

#ifdef FS_OS_SIMULATOR_BUILD
  sim_qurt_attr_getstackaddr (attr, stack_addr);
#else
  *stack_addr = attr->stack_addr;
#endif

  return 0;
}

int
fs_os_thread_join (fs_os_thread_t thread_handle, void **value_ptr)
{
  int result;
  int status;
  qurt_thread_attr_t qurt_attr;
  void *stack_addr = NULL;

  /* qurt doesnt provide exit status address. No way to return an address */
  (void) value_ptr;

  result = qurt_thread_attr_get (thread_handle, &qurt_attr);
  if (result != QURT_EOK)
  {
    FS_OS_MSG_ERROR_1 ("qurt_thread_attr_get failed %d", result);
    return result;
  }

  result = fs_os_qurt_attr_getstackaddr (&qurt_attr, &stack_addr);
  if (result != 0)
  {
    FS_OS_MSG_ERROR_1 ("fs_os_qurt_thread_get_stack failed %d", result);
    return result;
  }
  FS_OS_ASSERT (stack_addr != NULL);

  result = qurt_thread_join (thread_handle, &status);
  if (result != QURT_ENOTHREAD && result != QURT_EOK)
  {
    FS_OS_MSG_ERROR_1 ("qurt_thread_join failed %d", result);
    return result;
  }

  if (stack_addr != NULL)
  {
    fs_os_free (stack_addr);
  }

  return 0;
}

#endif
