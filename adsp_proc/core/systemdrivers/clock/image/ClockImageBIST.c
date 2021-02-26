/*
===========================================================================
*/
/**
  @file ClockImageBIST.c

*/
/*
  ====================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/ClockImageBIST.c#2 $
  $DateTime: 2017/10/25 23:14:56 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockImage.h"
#include "npa.h"


/*=========================================================================
      Macros
==========================================================================*/

#define CLOCK_BIST_ERROR_MARGIN_PPM 100     /* Expressed in PPM */
#define ABS_DIFF(v1, v2)  ((v1 >= v2) ? (v1 - v2) : (v2 - v1))
#define CLOCK_CPU_TEST_RESOURCE "/clk/cpu.test"


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_TestCPUConfig
** =========================================================================*/
/*
  Test a single CPU frequency.
*/

static DALResult Clock_TestCPUConfig
(
  ClockDrvCtxt      *pDrvCtxt,
  npa_client_handle  hHandle,
  ClockNodeType     *pClock,
  uint32             nFreqHz
)
{
  DALResult eResult;
  uint32    nResultFreqHz;
  uint32    nFreqError;
  uint32    nFreqErrorMargin;

  ULOG_RT_PRINTF_1(pDrvCtxt->hClockBISTLog,
    "   npa_issue_scalar_request:  %d Hz", nFreqHz);

  npa_issue_scalar_request(hHandle, nFreqHz / 1000);

  eResult = Clock_CalcClockFrequency(pDrvCtxt, pClock, &nResultFreqHz);
  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   CalcClockFrequency: ERROR");
    return DAL_ERROR;
  }

  ULOG_RT_PRINTF_1(pDrvCtxt->hClockBISTLog,
    "   CalcClockFrequency:        %d Hz", nResultFreqHz);

  /* Compute the error rate between the desired and the measured values */
  nFreqError = ABS_DIFF(nResultFreqHz, nFreqHz);
  nFreqErrorMargin = nFreqHz / 1000000 * CLOCK_BIST_ERROR_MARGIN_PPM;

  if (nFreqError > nFreqErrorMargin)
  {
    ULOG_RT_PRINTF_2(pDrvCtxt->hClockBISTLog,
      "   Frequency error %d Hz exceeds error margin of %d Hz",
      nFreqError, nFreqErrorMargin);
    return DAL_ERROR;
  }

  ULOG_RT_PRINTF_2(pDrvCtxt->hClockBISTLog,
    "   Frequency error %d Hz is within error margin of %d Hz",
    nFreqError, nFreqErrorMargin);

  return DAL_SUCCESS;

} /* END Clock_TestCPUConfig */


/* =========================================================================
**  Function : Clock_ImageBIST
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_ImageBIST
(
  ClockDrvCtxt  *pDrvCtxt,
  boolean       *bBISTPassed,
  uint32        *nFailedTests
)
{
  ClockImageCtxtType  *pImageCtxt = pDrvCtxt->pImageCtxt;
  ClockNodeType       *pClock;
  ClockDomainNodeType *pDomain;
  npa_client_handle    hCPUTest;
  DALResult            eResult;
  uint32               nFailedImageTests = 0;
  uint32               nFreqHz;
  uint32               i;

  ULOG_RT_PRINTF_1(pDrvCtxt->hClockBISTLog, "Running Image BIST for %s...",
    CLOCK_CPU_TEST_RESOURCE);

  hCPUTest = npa_create_sync_client(CLOCK_CPU_TEST_RESOURCE,
    "ClockImageBIST", NPA_CLIENT_REQUIRED);
  if (!hCPUTest)
  {
    ULOG_RT_PRINTF_1(pDrvCtxt->hClockBISTLog,
      "   Failed to create NPA client for %s.", CLOCK_CPU_TEST_RESOURCE);
    nFailedImageTests++;
  }
  else
  {
    pClock = pImageCtxt->CPUCtxt.pClock;
    pDomain = pClock->pDomain;

    for (i = 0; i < pDomain->nBSPLen; i++)
    {
      nFreqHz = pDomain->aBSP[i].nFreqHz;
      eResult = Clock_TestCPUConfig(pDrvCtxt, hCPUTest, pClock, nFreqHz);
      if (eResult != DAL_SUCCESS)
      {
        nFailedImageTests++;
      }
    }

    npa_complete_request(hCPUTest);
    npa_destroy_client(hCPUTest);
  }

  if (nFailedImageTests == 0)
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hClockBISTLog, "   Clock Image BIST Passed!");
  }
  else
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hClockBISTLog, "   Clock Image BIST Failed!");
    *nFailedTests += nFailedImageTests;
    *bBISTPassed = FALSE;
  }

  return DAL_SUCCESS;

} /* END Clock_ImageBIST */

