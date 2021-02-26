/*
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
@file
@brief SLIM processor specific implementation for time source.

@ingroup slim_Core
*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <libslimcommon/slim_internal.h>

#ifdef __ANDROID__
#include <utils/SystemClock.h>
#include <utils/Timers.h>
#endif

/**
@brief Returns the timetick count in milliseconds.

Function returns the current timetick count in milliseconds.

@return Current timetick count in milliseconds.
*/
uint64 slim_TimeTickGetMilliseconds
(
  void
)
{
#ifdef __ANDROID__
  return nanoseconds_to_milliseconds(android::elapsedRealtimeNano());
#else
  struct timespec ts;
  uint64 time_ms = 0;
  clock_gettime(CLOCK_BOOTTIME, &ts);

  time_ms += (ts.tv_sec * 1000LL);     /* Seconds to milliseconds */
  time_ms += ts.tv_nsec / 1000000LL;   /* Nanoseconds to milliseconds */

  return time_ms;
#endif
}
