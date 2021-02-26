/*
  Copyright (C) 2009-2010 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/osal/variant/qurt/src/apr_misc.c#1 $
  $Author: pwbldsvc $
*/

#include "qube.h"
#include "timer.h"
#include "apr_errcodes.h"
#include "apr_misc.h"
#include "timetick.h"
#include "DDITimetick.h"

static DalDeviceHandle        *phTimeTick = NULL;


APR_INTERNAL int32_t apr_misc_sleep (
  uint64_t time_ns
)
{

  timer_sleep( (time_ns / 1000), T_USEC, FALSE );

  return APR_EOK;
}

APR_INTERNAL uint64_t apr_get_clock_ticks ( void )
{
  DalTimetickTime64Type count64 = 0;

  /* Check if the handle for timetick DAL already exists */
  if ( phTimeTick == NULL )
  {
    if ( DAL_SUCCESS == DalTimetick_Attach( "SystemTimer", &phTimeTick ) )
    {
       DalTimetick_GetTimetick64( phTimeTick, &count64 );
    }
  }
  else
  {
    DalTimetick_GetTimetick64( phTimeTick, &count64 );
  }

  return count64;
}

APR_INTERNAL uint64_t apr_get_clock_in_us ( void )
{
  uint32_t freq;
  uint64_t ticks = 0;
  uint64_t time_us = 0;

  /* Get ticks */
  ticks = apr_get_clock_ticks();
  
  /* Convert ticks to us */
  if ( ticks != 0 )
  {
     if ( DAL_SUCCESS == DalTimetick_GetFreq( phTimeTick, &freq ) )
     {
        time_us = ticks / (( double ) freq / 1000000 );
     }
  }

  return time_us;
}
