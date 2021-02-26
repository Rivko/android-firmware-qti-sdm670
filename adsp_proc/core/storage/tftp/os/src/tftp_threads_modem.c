/***********************************************************************
 * tftp_threads_modem.c
 *
 * NON-HLOS Posix Threads API Abstraction
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/tftp/os/src/tftp_threads_modem.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-05-06   dks   Add OS abstraction layer
2015-01-05   dks   Compile server for TN Apps
2014-12-30   dks   Fixes to config and log module.
2014-10-14   rp    Use asserts for control-logic, debug-asserts for data-logic
2014-08-04   rp    Debug checks to ensure mutex calls succeed.
2014-07-29   dks   Featurize creating Detached threads.
2014-07-28   rp    Add api to get thread-id.
2014-06-11   rp    Renamed DEBUG_ASSERT as TFTP_DEBUG_ASSERT
2014-06-04   rp    Create

===========================================================================*/

#include "tftp_config_i.h"

#if !defined (TFTP_NHLOS_BUILD)
  #error "This file should be included only for NHLOS Builds"
#endif

#include "tftp_comdef.h"
#include "tftp_threads.h"
#include "tftp_threads_modem.h"
#include "tftp_assert.h"
#include "tftp_log.h"
#include "fs_os.h"
#include <stdio.h>

static int tftp_modem_thread_stack_size = FS_OS_THREAD_DEFAULT_STACK_SIZE;
static int tftp_thread_modem_count = 0;

int
tftp_thread_mutex_init (tftp_mutex_handle *hdl_ptr)
{

  fs_os_mutex_init ((fs_os_mutex_t*) hdl_ptr);

  return 0;
}

int
tftp_thread_lock (tftp_mutex_handle *hdl_ptr)
{
  fs_os_mutex_lock ((fs_os_mutex_t*) hdl_ptr);

  return 0;
}

int
tftp_thread_unlock (tftp_mutex_handle *hdl_ptr)
{

  fs_os_mutex_unlock ((fs_os_mutex_t*) hdl_ptr);

  return 0;
}

tftp_thread_handle
tftp_thread_self (void)
{
  fs_os_thread_t thread_handle = (fs_os_thread_t)NULL;

  thread_handle = fs_os_thread_self ();

  return (tftp_thread_handle) thread_handle;
}

int
tftp_thread_create(tftp_thread_handle *thread,
                   tftp_thread_return_type (*thread_start) (void *),
                   void *args)
{
  int result;
  fs_os_thread_attr_t thread_attr;
  char threadname[12];

  tftp_thread_modem_count++;
  snprintf (threadname, sizeof (threadname), "TFTP_%d",
            tftp_thread_modem_count);

  fs_os_thread_attr_init (&thread_attr);

  thread_attr.stack_size = tftp_modem_thread_stack_size;
  thread_attr.thread_name = threadname;

  result = fs_os_thread_create (thread, &thread_attr, thread_start, args);
  if (result != 0)
  {
    TFTP_LOG_ERR ("tftp_thread_create failed for count %d, error: %d %s",
                  tftp_thread_modem_count, result, strerror (result));
    result = -1;
  }

  return result;
}
