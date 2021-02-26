/**************************************************************************
 * FILE: devprg_utils.c
 *
 * Provides utility functions for devprg.
 * For now we just have the sleep and time
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_utils.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-13-07   svl   Create.

===========================================================================*/

#include "boothw_target.h"
#include "boot_dload.h"
#include "boot_target.h"

#include "devprg_utils.h"

static uint64 devprg_timetick(void)
{
  uint32 curr_count;
  uint32 last_count;
  uint32 diff;

  // Grab current time count
  curr_count = HWIO_IN(TIMETICK_CLK);

  // Keep grabbing the time until a stable count is given
  do
  {
    last_count = curr_count;
    curr_count = HWIO_IN(TIMETICK_CLK);
    diff = curr_count - last_count;
  } while (diff > 5); /* If the tick is less than 5 then assume the clock is
  stable. This is to handle the case where the CPU instruction clock is slow
  (like RUMI) and we get two different readings between to counter reads */

  return curr_count;
}

uint64 devprg_time_usec(void)
{
  uint64 curr_time_count = devprg_timetick();
  curr_time_count = CALCULATE_TIMESTAMP(curr_time_count);
  return curr_time_count;
}

int devprg_usleep(uint64 microsecs)
{
  uint64 end_time = devprg_time_usec() + microsecs;
  while (devprg_time_usec() < end_time)
  {/* do nothing. */}
  return 0;
}
