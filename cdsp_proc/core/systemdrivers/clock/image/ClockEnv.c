/*
===========================================================================
*/
/**
  @file ClockEnv.c

  Environment-specific clock definitions.
*/
/*
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/image/ClockEnv.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $
  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockEnv.h"
#include "com_dtypes.h"
#include "qurt.h"
#include "DALSys.h"
#include "DALDeviceId.h"


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_SetThreadPriority
** =========================================================================*/
/**
  See ClockDriver.h
*/

void Clock_SetThreadPriority
(
  uint32  nPriority,
  uint32 *nPrevPriority
)
{
  qurt_thread_t ThreadId;
  uint32        nResult;

  ThreadId = qurt_thread_get_id();

  if (nPrevPriority)
  {
    *nPrevPriority = qurt_thread_get_priority(ThreadId);
  }

  nResult = qurt_thread_set_priority(ThreadId, nPriority);
  if (nResult != 0)
  {
    DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to set thread priority to %d.", nPriority);
    while(1);
  }

} /* END Clock_SetThreadPriority */

