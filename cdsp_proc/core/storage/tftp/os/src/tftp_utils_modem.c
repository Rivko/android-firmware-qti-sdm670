/***********************************************************************
 * tftp_utils_modem.c
 *
 * NON_HLOS Utils Module.
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/tftp/os/src/tftp_utils_modem.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-12-30   dks   Fixes to config and log module.
2014-09-19   dks   Add hooks to extract performance numbers.
2014-08-26   rp    Bring in changes from target-unit-testing.
2014-07-28   rp    Add debug_info to measure timings.
2014-06-04   rp    Create

===========================================================================*/

#include "tftp_config_i.h"
#include "tftp_comdef.h"
#include "tftp_utils.h"

#if !defined (TFTP_NHLOS_BUILD)
  #error "This file should be included only for NHLOS Builds"
#endif

#include "DDITimetick.h"
#include "tftp_log.h"

/*--------------------------------------------------------------------------
  Host-To-Network and Network-To-Host functions.
--------------------------------------------------------------------------*/
uint16
tftp_htons (uint16 hostshort)
{
  return (((hostshort & 0xFF) << 8) | ((hostshort & 0xFF00) >> 8));
}

uint16
tftp_ntohs (uint16 netshort)
{
  return tftp_htons (netshort);
}

/*--------------------------------------------------------------------------
  Time Diff functions.
--------------------------------------------------------------------------*/

static DalDeviceHandle *tftp_timetick_handle = NULL;

static void
tftp_timetick_init (void)
{
  DALResult result;
  tftp_timetick_handle = NULL;

  result = DalTimetick_Attach ("SystemTimer", &tftp_timetick_handle);
  if ((result != DAL_SUCCESS) || (tftp_timetick_handle == NULL))
  {
    TFTP_LOG_ERR ("[%d] EFS: DalTimetick_Attach fail", result);
    tftp_timetick_handle = NULL;
    return;
  }

  result = DalTimetick_InitTimetick64 (tftp_timetick_handle);
  if ((result != DAL_SUCCESS))
  {
    TFTP_LOG_ERR ("[%d] DalTimetick_InitTimetick64 fail", result);
    tftp_timetick_handle = NULL;
  }
}

uint64
tftp_timetick_get (void)
{
  DalTimetickTime64Type timetick = 0;
  DALResult result;

  if (tftp_timetick_handle != NULL)
  {
    result = DalTimetick_GetTimetick64 (tftp_timetick_handle, &timetick);
    if (result != DAL_SUCCESS)
    {
      TFTP_LOG_ERR ("[%d] DalTimetick_GetTimetick64 fail", result);
      timetick = 0;
    }
  }

  return (uint64)timetick;
}

uint64
tftp_timetick_diff_in_usec (uint64 start_time, uint64 end_time)
{
  DalTimetickTime64Type timetick_difference, time_us = 0;
  DALResult result;
  DalTimetickTime64Type max_time;

  max_time = ~(0);

  if (end_time >= start_time)
  {
    timetick_difference = (uint64)(end_time - start_time);
  }
  else
  {
    timetick_difference = (max_time - start_time) + (end_time);
  }

  if (tftp_timetick_handle != NULL)
  {
    result = DalTimetick_CvtFromTimetick64 (tftp_timetick_handle,
               timetick_difference, T_USEC, &time_us);
    if (result != DAL_SUCCESS)
    {
      TFTP_LOG_ERR ("[%d] DalTimetick_CvtFromTimetick64 fail", result);
      time_us = 0;
    }
  }

  return (uint64)time_us;
}



void
tftp_utils_init (void)
{
  tftp_timetick_init ();
}

