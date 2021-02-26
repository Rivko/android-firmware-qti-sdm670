/***********************************************************************
 * fs_os_common.c
 *
 * OS common functionality
 * Copyright (C) 2015-2017 QUALCOMM Technologies, Inc.
 *
 * Verbose Description
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/fs_osal/src/fs_os_common.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-05-31   rp    Port to SLPI image.
2015-11-19   mj    Add DLL_API_GLOBAL for APIs included in Shared Objects.
2015-03-13   dks   Create

===========================================================================*/

#include "fs_os_config_i.h"
#include "fs_os_timetick.h"

#ifdef FEATURE_FS_OS_EFS_FULL_LIB
  #include "fs_public.h"
#endif


void
fs_os_init (void)
{
  fs_os_timetick_init();
}

#ifndef FEATURE_FS_OS_EFS_FULL_LIB

static int fs_os_qurt_efs_errno = 0;

int*
efs_get_errno_address (void)
{
  return &fs_os_qurt_efs_errno;
}

#endif
