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
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/cdsp/hw/sdm670/ClockTarget.c#3 $
  $DateTime: 2017/12/06 09:58:14 $
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
#include "HALclkGeneric.h"
#include "icb_error.h"

/*=========================================================================
      Functions
==========================================================================*/

static boolean bAXIS2WasOn = FALSE;
static ChipInfoVersionType nChipInfoVersion;
ClockPowerDomainIdType VAPPSPwrId = 0;

void    HAL_clk_TuringPowerDomainEnable     ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );

/*
 * HAL_clk_TuringPowerDomainCtrl
 *
 * Workaround for V1 while enabling the power domain.
 */
HAL_clk_PowerDomainControlType HAL_clk_TuringPowerDomainCtrl =
{
   /* .Enable     = */ HAL_clk_TuringPowerDomainEnable,
   /* .Disable    = */ HAL_clk_GenericPowerDomainDisable,
   /* .IsEnabled  = */ HAL_clk_GenericPowerDomainIsEnabled,
   /* .IsOn       = */ HAL_clk_GenericPowerDomainIsOn
};

void Clock_PowerDomainEnable(void)
{

  HWIO_OUTF(TURING_VAPSS_GDSCR, SW_COLLAPSE, 0x0);

  while (!(HWIO_IN(TURING_VAPSS_GDSCR) & HWIO_TURING_VAPSS_GDSCR_PWR_ON_BMSK))
  {
    busywait(2);
  }
    
  while (!HWIO_INF(TURING_VAPSS_CFG_GDSCR, GDSC_POWER_UP_COMPLETE));
  busywait(5);  
  
  /*
   * Check RETAIN_FF_ENABLE and set it if it's not set.
   */

   if(0x0 == HWIO_INF(TURING_VAPSS_GDSCR, RETAIN_FF_ENABLE))
   {
     HWIO_OUTF(TURING_VAPSS_GDSCR, RETAIN_FF_ENABLE, 0x1);
   }
   
}


void HAL_clk_TuringPowerDomainEnable
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  Clock_PowerDomainEnable();
  NOC_Error_InitDeferred(VAPPSPwrId);
}


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
  bAXIS2WasOn = (HWIO_INF(TURING_Q6SS_AXIS2_CBCR, CLK_ENABLE) & 0x1) ? TRUE : FALSE;
  
  HWIO_OUTF(TURING_Q6SS_AXIS2_CBCR, CLK_ENABLE, 0x1);
  while (HWIO_IN(TURING_Q6SS_AXIS2_CBCR) & HWIO_TURING_Q6SS_AXIS2_CBCR_CLK_OFF_BMSK)
  {
    busywait(2);
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
    busywait(5);
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
   * Get the power domain ID for the VAPSS_GDSC for a v1 workaround.
   */
  eResult = Clock_GetPowerDomainId(pDrvCtxt, "vapss_gdsc", &VAPPSPwrId);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }
  
  HWIO_OUTF(TURING_VAPSS_VAP_CORE_CBCR, SLEEP, 0x1);
  HWIO_OUTF(TURING_VAPSS_GDSCR, CLK_DIS_WAIT, 0x5);
  
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

