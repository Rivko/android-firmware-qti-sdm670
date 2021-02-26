/***********************************************************************
 * fs_global_lock.c
 *
 * FS_GLOBAL_LOCK critical section
 * Copyright (C) 2007-2013 QUALCOMM Technologies, Inc.
 *
 * The FS_GLOBAL_LOCK is used to protect the file system data from
 * concurrent access.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/src/fs_global_lock.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-02-28   nr    Wait for the rcinit handshake before initializing EFS.
2012-03-20   wek   Cleanup customer header file inclusion.
2011-03-08   dks   Add device setup calls.
2010-11-08   wek   Remove impractical and unused paranoia module.
2010-06-16   rp    Featurize the memory protection code for boot.
2009-06-12   ebb   Moved rex_xxx calls to fs_osal.h abstraction.
2009-04-01   ebb   Exposed memory protect/unprotect fns for format/chkfat.
2009-03-27   ebb   Removed #include fs_efs2.h
2009-03-23   ebb   Fix L4/Iguana dependencies.
2009-03-03   ebb   Added kernel protection domain extensions.
2008-12-15   yg    Do not include pd.h unless memory protection is defined.
2008-07-14   np    Added support for enabling l4 memory protection.
2007-12-27   yg    Added memory protection related header file
2007-12-05   rp    Moved FS_PARANOIA_CHECKS feature to fs_paranoia.c
2007-11-16   sh    Rename TIMETEST2 calls to Babylon calls
2007-10-26   rp    Corrected featurization of FS_PARANOIA_CHECKS.
2007-08-19   rp    Create

===========================================================================*/

#include "fs_config_i.h"
#include "fs_global_lock.h"
#include "fs_osal.h"

static fs_os_crit_sect_type fs_crit_sect;

void
fs_global_lock_init(void)
{
  fs_os_init_crit_sect (&fs_crit_sect);
}

void
fs_global_lock (void)
{
  fs_os_enter_crit_sect (&fs_crit_sect);
}

void
fs_global_unlock (void)
{
  fs_os_leave_crit_sect (&fs_crit_sect);
}
