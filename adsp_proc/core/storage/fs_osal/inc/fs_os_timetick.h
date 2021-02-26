/***********************************************************************
 * fs_os_timetick.h
 *
 * Timetick APIs for all FS Modules
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/fs_osal/inc/fs_os_timetick.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-03-13   dks   Create

===========================================================================*/

#ifndef __FS_OS_TIMETICK_H__
#define __FS_OS_TIMETICK_H__

#include "fs_os_config_i.h"
#include "comdef.h"

typedef uint64 fs_os_timetick;
typedef uint64 fs_time_us;

/* Initialize TimeTick module. */
void fs_os_timetick_init (void);

/* Returns current timetick in microseconds, or 0 in case of any error. */
fs_os_timetick fs_os_timetick_get_timetick (void);

/* Returns time-difference in microseconds, or 0 in case of any error. */
fs_time_us fs_os_timetick_diff_to_us (fs_os_timetick timetick_difference);

#endif /* not __FS_OS_TIMETICK_H__ */
