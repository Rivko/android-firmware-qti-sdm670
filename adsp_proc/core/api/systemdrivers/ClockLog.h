#ifndef __CLOCKLOG_H__
#define __CLOCKLOG_H__
/*
  ===========================================================================
*/
/**
  @file ClockLog.h
  @brief Public interface include file for dumping the clock driver log.
  ===========================================================================

*/

/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/api/systemdrivers/ClockLog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/13   dcf     Ported from baseline.
  ===========================================================================
*/

#include "DDIClock.h"
#include "ULog.h"

/*=========================================================================
      Interface Declarations
==========================================================================*/

/** @addtogroup clock_log_functions
@{ */

/* =========================================================================
**  Function : DalClock_LogState
** =========================================================================*/
/**
  Logs the state of the clock driver.

  This function logs the state of all clocks and power domains into the
  provided ULog handle in CSV format.
 
  @param _h     [in]  Handle to the DAL device.
  @param hULog  [in]  ULog in which to log data.
  @param nFlags [in]  Bitmask of options (listed in ClockLogStateFlags).

  @return
  DAL_SUCCESS -- State logged successfully.
  DAL_ERROR_INTERNAL   -- There was an error writing to the ULog.
  DAL_ERROR_INVALID_PARAMETER -- ULog handle is NULL.
*/

static __inline DALResult
DalClock_LogState(DalDeviceHandle * _h,  ULogHandle hULog,  uint32 nFlags)
{
   return ((DalClockHandle *)_h)->pVtbl->LogState(DALDEVICE_LOCAL_CLIENTID, _h, (void *)hULog, nFlags);
}
/** @} */ /* end_addtogroup clock_log_functions */

#endif /* __CLOCKLOG_H__ */

