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
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/cdsp/hw/sdm845/ClockTarget.c#1 $
  $DateTime: 2017/07/21 22:10:47 $
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
static boolean bGDSCWasOn = FALSE;
static boolean bGDSCSWCollapsed = TRUE;
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


void Clock_SetPreDependencies(void)
{
  ClockDrvCtxt *pDrvCtxt = Clock_GetDrvCtxtNoLock();
  
  if (HWIO_IN(TURING_Q6SS_AHBM_AON_CBCR) & 0x80000000)
  {
    if (pDrvCtxt->hClockLog)
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "Clock Debug:  TURING_Q6SS_AHBM_AON_CBCR is OFF.");
    }
    
    HWIO_OUTF(TURING_Q6SS_AHBM_AON_CBCR, CLK_ENABLE, 0x1);
    while(HWIO_IN(TURING_Q6SS_AHBM_AON_CBCR) & 0x80000000) busywait(2);
  }
  
  /*
   *****************************************************************************************
   */
  if (HWIO_IN(TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR) & 0x80000000)
  {
    if (pDrvCtxt->hClockLog)
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "Clock Debug:  TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR is OFF.");
    }
    HWIO_OUTF(TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR, CLK_ENABLE, 0x1);
    while(HWIO_IN(TURING_TURING_WRAPPER_CDSP_NOC_AON_CBCR) & 0x80000000) busywait(2);
  }
}

void Clock_SetPostDependencies(void)
{
  ClockDrvCtxt *pDrvCtxt = Clock_GetDrvCtxtNoLock();
  
  /*
   *****************************************************************************************
   */
  if (HWIO_IN(TURING_Q6SS_AXIS2_CBCR) & 0x80000000)
  {
    if (pDrvCtxt->hClockLog)
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "Clock Workaround Debug:  TURING_Q6SS_AXIS2_CBCR is OFF.");
    }
    HWIO_OUTF(TURING_Q6SS_AXIS2_CBCR, CLK_ENABLE, 0x1);
    while(HWIO_IN(TURING_Q6SS_AXIS2_CBCR) & 0x80000000) busywait(2);
  }
  
  /*
   *****************************************************************************************
   */
  if (HWIO_IN(TURING_VAPSS_AXI_CBCR) & 0x80000000)
  {
    if (pDrvCtxt->hClockLog)
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "Clock Workaround Debug:  TURING_VAPSS_AXI_CBCR is OFF.");
    }
    HWIO_OUTF(TURING_VAPSS_AXI_CBCR, CLK_ENABLE, 0x1);
    while(HWIO_IN(TURING_VAPSS_AXI_CBCR) & 0x80000000) busywait(2);
  }
  
  /*
   *****************************************************************************************
   */
  if (HWIO_IN(TURING_VAPSS_AHBS_AON_CBCR) & 0x80000000)
  {
    if (pDrvCtxt->hClockLog)
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "Clock Workaround Debug:  TURING_VAPSS_AHBS_AON_CBCR is OFF.");
    }
    HWIO_OUTF(TURING_VAPSS_AHBS_AON_CBCR, CLK_ENABLE, 0x1);
    while(HWIO_IN(TURING_VAPSS_AHBS_AON_CBCR) & 0x80000000) busywait(2);
  }  
  
  /*
   *****************************************************************************************
   */
  if (HWIO_IN(TURING_VAPSS_VAP_CORE_CBCR) & 0x80000000)
  {
    if (pDrvCtxt->hClockLog)
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "Clock Workaround Debug:  TURING_VAPSS_VAP_CORE_CBCR is OFF.");
    }
    HWIO_OUTF(TURING_VAPSS_VAP_CORE_CBCR, CLK_ENABLE, 0x1);
    while(HWIO_IN(TURING_VAPSS_VAP_CORE_CBCR) & 0x80000000) busywait(2);
  }  
  /*
   *****************************************************************************************
   */
  if (HWIO_IN(TURING_VAPSS_VAP_TCMS_CBCR) & 0x80000000)
  {
    if (pDrvCtxt->hClockLog)
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                        "Clock Workaround Debug:  TURING_VAPSS_VAP_TCMS_CBCR is OFF.");
    }
    HWIO_OUTF(TURING_VAPSS_VAP_TCMS_CBCR, CLK_ENABLE, 0x1);
    while(HWIO_IN(TURING_VAPSS_VAP_TCMS_CBCR) & 0x80000000) busywait(2);
  }  
  /*
   *****************************************************************************************
   */  
}


void Clock_PowerDomainEnable(void)
{
  Clock_SetPreDependencies();
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
   
   Clock_SetPostDependencies();
}


void HAL_clk_TuringPowerDomainEnable
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc
)
{
  Clock_PowerDomainEnable();
  
  /*
   * Overriding the HAL GDSC structure so that on V1 we can program NOC registers which
   * are required every time the GDSC is powered on.  This is a workaround for a 
   * HW issue on V1 only.
   */
  if (nChipInfoVersion < CHIPINFO_VERSION(2, 0))
  {
    NOC_Error_InitDeferred(VAPPSPwrId); 
  }
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
  uint32 nGDSCRegVal = HWIO_IN(TURING_VAPSS_GDSCR);
  
  if (nChipInfoVersion < CHIPINFO_VERSION(2, 0))
  {
    if (0x0 == (nGDSCRegVal & HWIO_TURING_VAPSS_GDSCR_PWR_ON_BMSK))
    {
      bGDSCWasOn = FALSE;
      if (TRUE == bAXIS2WasOn)
      {
        HWIO_OUTF(TURING_Q6SS_AXIS2_CBCR, CLK_ENABLE, 0x0);
    busywait(5);
      }
  }
  else
  {
      bGDSCWasOn = TRUE;
    }
    
    bGDSCSWCollapsed = (nGDSCRegVal & HWIO_TURING_VAPSS_GDSCR_SW_COLLAPSE_BMSK) ? TRUE : FALSE;
    
    Clock_PowerDomainEnable();
  }

  /*
   * AXIS2 clock enablement is applicable to at least HW revisions 1 and 2.
   */
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
  
  /*
   * GDSC workaround is only applicable to V1 hardware.
   */
  if (nChipInfoVersion < CHIPINFO_VERSION(2, 0))
  {
    if (TRUE == bGDSCSWCollapsed)
    {
      /*
       * GDSC was not on before entering power collapse.  Enable it here and
       * mark the boolean flag as having been off.
       */
      HWIO_OUTF(TURING_VAPSS_GDSCR, SW_COLLAPSE, 0x1);
      bGDSCSWCollapsed = TRUE;
    }
    else
    {
      /*
       * GDSC was on before entering power collapse.  Mark the boolean flag 
       * and proceed.
       */
      bGDSCSWCollapsed = FALSE;
    }
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

