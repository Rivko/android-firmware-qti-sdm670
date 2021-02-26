/*
===========================================================================
*/
/**
  @file ClockMSSLite.c
  
  This is a lightweight, non-DAL, implementation for clocks that must
  be available prior to the DAL clock driver being initialized.
*/
/*  
  ====================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/adsp/hw/sdm845/ClockLPASSLite.c#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/


#include "DALDeviceId.h"
#include "DALSys.h"
#include "DDIClock.h"
#include "ClockDriver.h"
#include "HALhwio.h"
#include "HALclkHWIO.h"
#include "qurt_anysignal.h"
#include "qurt_sclk.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/


#define CLOCK_NAME_PRNG_AHB                 "gcc_prng_ahb_clk"
#define CLOCK_LITE_MAX_ENABLE_READ_ATTEMPTS 150


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/


/*
 * Handle to the clock DAL.
 */
static DalDeviceHandle *hClock;

/*
 * PRNG clock identifier.
 */
static ClockIdType nPRNGClockId;

/*
 * Reference count for prng enable history in lite implementation.
 */
static uint32 nPRNGReferenceCount;


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_EnablePRNG
** =========================================================================*/

DALResult Clock_EnablePRNG(void)
{
  ClockDrvCtxt *pDrvCtxt;
  DALResult     eResult;
  
  /*-----------------------------------------------------------------------*/
  /* ENTER CRITICAL SECTION.                                               */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = Clock_GetDrvCtxt();

  /*-----------------------------------------------------------------------*/
  /* Use DAL API if the clock driver has initialized.                      */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->pImageCtxt != NULL)
  {
    /*
     * Sanity.
     */
    if (hClock == NULL || nPRNGClockId == 0)
    {
      Clock_ReleaseDrvCtxt();
      return DAL_ERROR_INTERNAL;
    }

    /*
     * Invoke DAL API to enable the PRNG clock.
     */
    eResult = DalClock_EnableClock(hClock, nPRNGClockId);
    if(eResult != DAL_SUCCESS)
    {
      Clock_ReleaseDrvCtxt();
      return DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Otherwise, enable the PRNG clock directly using HWIO.                 */
  /*-----------------------------------------------------------------------*/

  else
  {
    if (nPRNGReferenceCount == 0)
    {
      uint32 nTimeout = 0;
      unsigned long long currentTime, waitInterval;

      HWIO_OUTF(GCC_PROC_CLK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA, 1);
      while (HWIO_INF(GCC_PRNG_AHB_CBCR, CLK_OFF))
      {
        /*
         * Return error for timeout.
         */
        if (nTimeout++ == CLOCK_LITE_MAX_ENABLE_READ_ATTEMPTS)
        {
          Clock_ReleaseDrvCtxt();
          return DAL_ERROR;
        }

        /*
         * Wait 1 us between read attempts
         */
        currentTime = qurt_sysclock_get_hw_ticks();
        waitInterval = qurt_sysclock_timetick_from_us(1);
        while (qurt_sysclock_get_hw_ticks() < currentTime + waitInterval)
        {
        }
      }
    }

    nPRNGReferenceCount++;
  }

  /*-----------------------------------------------------------------------*/
  /* EXIT CRITICAL SECTION.                                                */
  /*-----------------------------------------------------------------------*/

  Clock_ReleaseDrvCtxt();

  return DAL_SUCCESS;

} /* END Clock_EnablePRNG */


/* =========================================================================
**  Function : Clock_DisablePRNG
** =========================================================================*/

DALResult Clock_DisablePRNG(void)
{
  ClockDrvCtxt *pDrvCtxt;
  DALResult     eResult;

  /*-----------------------------------------------------------------------*/
  /* ENTER CRITICAL SECTION.                                               */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = Clock_GetDrvCtxt();

  /*-----------------------------------------------------------------------*/
  /* Use DAL API if the clock driver has initialized.                      */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->pImageCtxt != NULL)
  {
    /*
     * Sanity.
     */
    if (hClock == NULL || nPRNGClockId == 0)
    {
      Clock_ReleaseDrvCtxt();
      return DAL_ERROR_INTERNAL;
    }

    eResult = DalClock_DisableClock(hClock, nPRNGClockId);
    if(eResult != DAL_SUCCESS)
    {
      Clock_ReleaseDrvCtxt();
      return DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Otherwise, disable the PRNG clock directly using HWIO.                */
  /*-----------------------------------------------------------------------*/

  else
  {
    if (nPRNGReferenceCount == 1)
    {
      HWIO_OUTF(GCC_PROC_CLK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA, 0);
    }

    if (nPRNGReferenceCount > 0)
    {
      nPRNGReferenceCount--;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* EXIT CRITICAL SECTION.                                                */
  /*-----------------------------------------------------------------------*/

  Clock_ReleaseDrvCtxt();

  return DAL_SUCCESS;

} /* END Clock_DisablPRNG */


/* =========================================================================
**  Function : Clock_TransferReferenceCount
** =========================================================================*/

DALResult Clock_TransferReferenceCount(void)
{
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Sanity                                                                */
  /*-----------------------------------------------------------------------*/

  if (hClock && nPRNGClockId)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize handle for DAL API.                                        */
  /*-----------------------------------------------------------------------*/

  eResult = DAL_DeviceAttach(DALDEVICEID_CLOCK, &hClock);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_ERROR,
      "Unable to attach to clock DAL: %d",
      eResult);

    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize PRNG clock id.                                             */
  /*-----------------------------------------------------------------------*/

  eResult = DalClock_GetClockId(hClock, CLOCK_NAME_PRNG_AHB, &nPRNGClockId);
  if(eResult != DAL_SUCCESS)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Transfer enable reference counts to the DAL driver.                   */
  /*-----------------------------------------------------------------------*/

  for ( ; nPRNGReferenceCount > 0; nPRNGReferenceCount--)
  {
    eResult = DalClock_EnableClock(hClock, nPRNGClockId);
    if(eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_TransferReferenceCount */

