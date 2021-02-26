/*==============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/


#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <setjmp.h>

#include "HAP_farf.h"

#include "qurt_signal.h"
#include "uTimetick.h"
#ifdef UIMAGE
#include "utimer.h"
#else
// full path to include the slpi timer.h, not the chre timer.h
#include "timer.h"
#define UT_NONE T_NONE
#define UT_USEC T_USEC
#define UTE_SUCCESS TE_SUCCESS
#define UTIMER_FUNC1_CB_TYPE TIMER_FUNC1_CB_TYPE
#define utimer_type timer_type
#define utimer_cb_data_type timer_cb_data_type
#define utimer_error_type timer_error_type
#define utimer_error_type timer_error_type
#define utimer_def_osal timer_def_osal
#define utimer_set_64 timer_set_64
#define utimer_clr_64 timer_clr_64
#define utimer_undef timer_undef
#endif

/**
 * Gets the number of nanoseconds since some stable reference point in the past.
 *
 * @return
 */
uint64_t chrePalGetTime(void)
{
  /*
  // this function returns the current time in ns from ap epoch
  uint32_t qmiDspTicks;
  uint64_t qmiApNs;
  uint64_t qmiDspNs;
  int64_t epochOffset;

  // TODO requires qmi to ap, too expensive, we probably don't have to do this and instead base on arbitrary epoch and correct when we sent to ap?
  //  getClockInfo(&qmiDspTicks, &qmiApNs); // read correlated dsp <--> ap times, this offset can fluctuate or drift so query every time
  //  qmiDspNs = (qmiDspTicks * NS_PER_SEC) / DSP_HZ; // convert dsp ticks to ns
  //  epochOffset = qmiApNs - qmiDspNs; // calc epoch offset between ap and dsp
  //  return uTimetick_CvtFromTicks(uTimetick_Get(), T_USEC) * 1000 + epochOffset; // get current dsp, convert to ns, apply epoch offset
*/
  return uTimetick_CvtFromTicks(uTimetick_Get(), T_USEC) * 1000;
}

// TODO move to pal_heap.c

void* chrePalAlloc(uint32_t sz)
{
  return malloc(sz);
}

void chrePalFree(void* ptr)
{
  free(ptr);
}

