/*
===========================================================================
*/
/**
  @file ClockTarget.c

*/
/*
  ====================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/image/cdsp/hw/sdm855/ClockTarget.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockDriver.h"
#include "ClockImage.h"
#include "HALclkHWIO.h"
#include "HALhwio.h"
#include "DDIChipInfo.h"
#include "busywait.h"

/*=========================================================================
      Functions
==========================================================================*/

static boolean bAXIS2WasOn = FALSE;
static ChipInfoVersionType nChipInfoVersion;

/* =========================================================================
**  Function : Clock_ProcessorSleep
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ProcessorSleep
(
  ClockDrvCtxt *pDrvCtxt,
  ClockSleepModeType eMode,
  uint32 nFlags
)
{
  /*
   * Ensure AXIS2 clock is ON before entering sleep.
   */
  if (0x0 == HWIO_INF(TURING_Q6SS_AXIS2_CBCR, CLK_ENABLE))
  {
    HWIO_OUTF(TURING_Q6SS_AXIS2_CBCR, CLK_ENABLE, 0x1);
    busywait(5);
    bAXIS2WasOn = FALSE;
  }
  else
  {
    bAXIS2WasOn = TRUE;
  }
  
  return DAL_SUCCESS;

} /* END Clock_ProcessorSleep */


/* =========================================================================
**  Function : Clock_ProcessorRestore
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ProcessorRestore
(
  ClockDrvCtxt *pDrvCtxt,
  ClockSleepModeType eMode,
  uint32 nFlags
)
{
  if (FALSE == bAXIS2WasOn)
  {
    HWIO_OUTF(TURING_Q6SS_AXIS2_CBCR, CLK_ENABLE, 0x0);
  }
  else
  {
    bAXIS2WasOn = FALSE;  
  }
  
  return DAL_SUCCESS;

} /* END Clock_ProcessorRestore */


/* =========================================================================
**  Function : Clock_InitTarget
** =========================================================================*/
/*
  See ClockDriver.h.
*/

DALResult Clock_InitTarget (ClockDrvCtxt *pDrvCtxt)
{
  DALResult eResult;

  nChipInfoVersion = DalChipInfo_ChipVersion();
  
  eResult = Clock_InitImage(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }
  
  /*
   * Need to set the RETAIN_FF_ENABLE bit for the GDHS for retention control.
   */
   HWIO_OUTF(TURING_VAPSS_GDSCR, RETAIN_FF_ENABLE, 0x1);

  return DAL_SUCCESS;

} /* END Clock_InitTarget */

/* =========================================================================
**  Function : ClockStub_InitTarget
** =========================================================================*/
/*
  See ClockDriver.h.
*/

DALResult ClockStub_InitTarget (ClockDrvCtxt *pDrvCtxt)
{
  return DAL_SUCCESS;

} /* END ClockStub_InitTarget */

