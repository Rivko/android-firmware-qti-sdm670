/*
===========================================================================
*/
/**
  @file ClockImage.c

*/
/*
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/ClockImage.c#2 $
  $DateTime: 2017/09/01 02:44:54 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockImage.h"
#include "npa_resource.h"
#include "sys_m_smsm.h"
#include "rcecb.h"
#include "rcinit.h"

/*=========================================================================
      Externs
==========================================================================*/

extern ClockImageBSPType ClockImageBSP;


/*=========================================================================
      Prototypes
==========================================================================*/

void Clock_ImagePostInit (void);


/*=========================================================================
      Data
==========================================================================*/

/*
 * Main image context.
 */
ClockImageCtxtType Clock_ImageCtxt;


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_InitImage
** =========================================================================*/
/*
  See ClockImage.h
*/

DALResult Clock_InitImage
(
  ClockDrvCtxt        *pDrvCtxt
)
{
  ClockImageCtxtType *pImageCtxt = &Clock_ImageCtxt;
  DALResult eResult;
  RCECB_HANDLE         hRCECB;

  /*
   * Hookup references.
   */
  pDrvCtxt->pImageCtxt = pImageCtxt;
  pImageCtxt->pBSP = &ClockImageBSP;

  /*
   * Intialize the CPU.
   */
  eResult = Clock_InitCPU(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  /*
   * Intialize QDSS.
   */
  eResult = Clock_InitQDSS(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  /*
   * Register callback for when RCINIT has finished.
   */
  hRCECB = rcecb_register_name(RCINIT_RCECB_INITGROUPS, Clock_ImagePostInit);
  if (hRCECB == RCECB_NULL)
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;

} /* END Clock_InitImage */


/* =========================================================================
**  Function : Clock_ImagePostInit
** =========================================================================*/
/*
  Perform any necessary actions once image initialization has completed
  (i.e. end of rcinit).
*/

void Clock_ImagePostInit (void)
{
  int32 nSMSMResult;

  /*
   * Notify APSS that initialization is done and it can remove its
   * proxy votes.
   */
  nSMSMResult = sys_m_smsm_apps_set(SYS_M_SUBSYS2AP_SMSM_PWR_CLK_RDY);
  if (nSMSMResult != SYS_M_SMP2P_SUCCESS)
  {
    DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
      "Unable to signal that the clock driver is done initial votes.");
  }

} /* END Clock_ImagePostInit */


/* =========================================================================
**  Function : Clock_SetCPUMaxFrequencyAtCurrentVoltage
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SetCPUMaxFrequencyAtCurrentVoltage
(
  ClockDrvCtxt *pDrvCtxt,
  boolean bRequired,
  uint32 *pnResultFreqHz
)
{
  return DAL_SUCCESS;

} /* END Clock_SetCPUMaxFrequencyAtCurrentVoltage */


/* =========================================================================
**  Function : Clock_EnableDCVS
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnableDCVS
(
  ClockDrvCtxt *pDrvCtxt
)
{
  return DAL_SUCCESS;

} /* END Clock_EnableDCVS */


/* =========================================================================
**  Function : Clock_LoadNV
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_LoadNV
(
  ClockDrvCtxt *pDrvCtxt
)
{
  return DAL_SUCCESS;

} /* END Clock_LoadNV */
