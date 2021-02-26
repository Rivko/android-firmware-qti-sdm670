/***********************************************************************
 * fs_osal.c
 *
 * Stub for fs_os_self in Boot.
 *
 * Copyright (C) 2011 QUALCOMM Technologies, Inc.
 *
 * In boot rex is not present and this file provides a dummy implementation
 * of fs_os_self.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-11-04   dks   Create. Fix compilation for boot.

===========================================================================*/

#include "fs_config_i.h"
#include "fs_osal.h"

#ifdef FEATURE_EFS_COMPILE_WITHOUT_REX

static fs_os_tcb_type fs_dummy_tcb;

fs_os_tcb_type*
fs_os_self (void)
{
  return &fs_dummy_tcb;
}

#endif /* FEATURE_EFS_COMPILE_WITHOUT_REX */

