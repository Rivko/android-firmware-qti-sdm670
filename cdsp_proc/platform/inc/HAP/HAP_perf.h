#ifndef HAP_PERF_H
#define HAP_PERF_H
/*==============================================================================
  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "AEEStdDef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
  HAP_perf_get_time_us 
   
  Gets the current micro-seconds based on the 56 bit, 19.2MHz global hardware
  clock count.
 
  Returns the current value of a 56-bit hardware counter in micro-seconds.  This
  hardware counter is shared amonst all the cores on the device.
 
  The value wraps around to zero when it exceeds the maximum value. This
  operation must be used with care because of the wrap-around behavior and
  should be treated as relative rather than absolute.
 
  Returns:
  Integer -- Current value of 32-bit hardware counter converted to micro-seconds
*/
uint64 HAP_perf_get_time_us(void);

/* 
  HAP_perf_get_qtimer_count 
   
  Gets the current 56 bit, 19.2MHz global hardware clock count.
   
  Returns the current value of a 56-bit hardware counter.  This hardware counter
  is shared amonst all the cores on the device.
 
  The value wraps around to zero when it exceeds the maximum value. This
  operation must be used with care because of the wrap-around behavior and
  should be treated as relative rather than absolute.

  Returns:
  Integer -- Current count of global hardware clock
*/
uint64 HAP_perf_get_qtimer_count(void);

/* 
  HAP_perf_qtimer_count_to_us 
   
  Converts from a 19.2 MHz glocal hardware count to micro-seconds.
   
  Returns:
  Integer -- micro-second conversion of the supplied count
*/
uint64 HAP_perf_qtimer_count_to_us(uint64 count);

/* 
  HAP_perf_get_pcycles 
   
  Gets the current 64-bit processor cycle count
   
  The processor cycle count is the current number of processor cycles executed
  since the Hexagon processor was last reset. 
	
  Returns:
  Integer -- Current count of Hexagon processor cycle count.
*/
uint64 HAP_perf_get_pcycles(void);

/* 
  HAP_timer_sleep 

  Cause the calling thread to be suspended from execution
  until the specified duration has elapsed.
  
  The sleep duration is specified in microseconds
	
  Returns:
  Integer -- Returns 0 on success
*/
int HAP_timer_sleep(unsigned long long sleep_duration);

#ifdef __cplusplus
}
#endif

#endif // HAP_PERF_H
