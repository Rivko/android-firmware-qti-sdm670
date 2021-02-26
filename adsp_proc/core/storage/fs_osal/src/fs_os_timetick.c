/***********************************************************************
 * fs_os_timetick.c
 *
 * Timetick APIs for all FS Modules
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/fs_osal/src/fs_os_timetick.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-03-13   dks   Create

===========================================================================*/

#include "fs_os_config_i.h"
#include "fs_os_timetick.h"
#include "fs_os_msg.h"
#include "DDITimetick.h"
#include <string.h>

struct fs_os_timetick_info_type
{
  DalDeviceHandle *handle;
  uint8 init_successful;
};

static struct fs_os_timetick_info_type fs_os_timetick_info;

void
fs_os_timetick_init (void)
{
  DALResult result;

  memset (&fs_os_timetick_info, 0x0, sizeof (fs_os_timetick_info));

  result = DalTimetick_Attach ("SystemTimer", &fs_os_timetick_info.handle);
  if ((result != DAL_SUCCESS) || (fs_os_timetick_info.handle == NULL))
  {
    FS_OS_MSG_ERROR_1 ("%d DalTimetick_Attach failed", result);
    fs_os_timetick_info.handle = NULL;
    fs_os_timetick_info.init_successful = 0;
    return;
  }

  result = DalTimetick_InitTimetick64 (fs_os_timetick_info.handle);
  if ((result != DAL_SUCCESS))
  {
    FS_OS_MSG_ERROR_1 ("%d DalTimetick_InitTimetick64 failed", result);
    fs_os_timetick_info.handle = NULL;
    fs_os_timetick_info.init_successful = 0;
  }
}

fs_os_timetick
fs_os_timetick_get_timetick (void)
{
  fs_os_timetick timetick = 0;
  DALResult result;

  if (fs_os_timetick_info.handle != NULL)
  {
    result = DalTimetick_GetTimetick64 (fs_os_timetick_info.handle, &timetick);
    if (result != DAL_SUCCESS)
    {
      FS_OS_MSG_ERROR_1 ("%d DalTimetick_GetTimetick64 fail", result);
    }
  }
  return timetick;
}

fs_time_us
fs_os_timetick_diff_to_us (fs_os_timetick timetick_difference)
{
  fs_time_us time_us = 0;
  DALResult result;

  if (fs_os_timetick_info.handle != NULL)
  {
    result = DalTimetick_CvtFromTimetick64 (fs_os_timetick_info.handle,
               timetick_difference, T_USEC, &time_us);

    if (result != DAL_SUCCESS)
    {
      FS_OS_MSG_ERROR_1("%d DalTimetick_CvtFromTimetick64 fail", result);
    }
  }
  return time_us;
}

