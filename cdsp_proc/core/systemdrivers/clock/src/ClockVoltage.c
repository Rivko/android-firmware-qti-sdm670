/*
===========================================================================
*/
/**
  @file ClockVoltage.c

  Digital voltage management interface implementation for the clock
  device driver.
*/
/*
  ====================================================================

  Copyright (c) 2010-2017 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header $
  $DateTime $
  $Author $

  when       who     what, where, why
  --------   ---     -------------------------------------------------

  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "ClockDriver.h"
#include "ClockSWEVT.h"
#include "VCS.h"


/*=========================================================================
      Constants and Macros
==========================================================================*/

/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_UpdateVoltageRequests
** =========================================================================*/
/**
  Updates the current voltage request.

  This function updates the current voltage request for a particular voltage
  regulator configuration.

  @param *pDrvCtxt [in] -- Context pointer, the driver context in this case.
  @param pVRegConfig -- The voltage regulator configuration to update.
  @param eNewLevel -- New voltage level to configure.

  @return
  DAL_SUCCESS -- Request was successful.
  DAL_ERROR   -- Request failed.

  @dependencies
  None.
*/

static DALResult Clock_UpdateVoltageRequests
(
  ClockRailType           *pRail,
  ClockVRegConfigType *pVRegConfig,
  rail_voltage_level        eNewLevel
)
{
  ClockCornerRefCountType *pRefCount;
  rail_voltage_level eMinLevel;
  uint32 i;

  /*-----------------------------------------------------------------------*/
  /* Check if new min has changed.                                         */
  /*-----------------------------------------------------------------------*/

  if (eNewLevel > pVRegConfig->eMinLevel)
  {
    eMinLevel = eNewLevel;
  }
  else
  {
    eMinLevel = 0;
    for (i = 0; i < RAIL_VOLTAGE_LEVEL_NUM_LEVELS; i++)
    {
      pRefCount = &pVRegConfig->aCornerRefCounts[i];
      if (pRefCount->eCorner == 0)
      {
        break;
      }

      if (pRefCount->nRefCount > 0 && pRefCount->eCorner > eMinLevel)
      {
        eMinLevel = pRefCount->eCorner;
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update request if minimum level is different.                         */
  /*-----------------------------------------------------------------------*/

  if (eMinLevel != pVRegConfig->eMinLevel)
  {
    if (pVRegConfig->NPAHandle == NULL)
    {
      if (eMinLevel > pRail->eBootCorner)
      {
        DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
          "Voltage request greater than boot default before %s is available (0x%x > 0x%x).",
          pRail->szName, eMinLevel, pRail->eBootCorner);
      }
    }
    else if (!pRail->bSuspendVoltageRequest)
    {
      /*
       * Set Fire and Forget attribute for the NPA request if we are
       * requesting for a lower voltage. This attribute is only valid for
       * one request and gets cleared as soon as the NPA receives it.
       */
      if (eMinLevel < pVRegConfig->eMinLevel)
      {
        npa_set_request_attribute(
          pVRegConfig->NPAHandle,
          NPA_REQUEST_FIRE_AND_FORGET);
      }

      npa_issue_scalar_request(pVRegConfig->NPAHandle, eMinLevel);
    }

    pVRegConfig->eMinLevel = eMinLevel;
  }

  return DAL_SUCCESS;

} /* END Clock_UpdateVoltageRequests */


/* =========================================================================
**  Function : Clock_GetCornerRefCount
** =========================================================================*/
/*
  Get the reference count structure for a given voltage corner.
*/

static ClockCornerRefCountType *Clock_GetCornerRefCount
(
  ClockVRegConfigType *pVRegConfig,
  rail_voltage_level   eCorner
)
{
  ClockCornerRefCountType *pRefCount;
  uint32 i;

  for (i = 0; i < RAIL_VOLTAGE_LEVEL_NUM_LEVELS; i++)
  {
    pRefCount = &pVRegConfig->aCornerRefCounts[i];
    if (pRefCount->eCorner == eCorner)
    {
      return pRefCount;
    }
    else if (pRefCount->eCorner == 0)
    {
      pRefCount->eCorner = eCorner;
      return pRefCount;
    }
  }

  DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
    "Not enough space to aggregate voltage request.");

  return NULL;

} /* END Clock_GetCornerRefCount */


/* =========================================================================
**  Function : Clock_VoltageRequest
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_VoltageRequest
(
  ClockRailType         *pRail,
  ClockVRegRequestType  *pCurrentRequest,
  ClockVRegRequestType  *pNewRequest
)
{
  ClockVRegConfigType *pCurrentVRegConfig, *pNewVRegConfig;
  ClockCornerRefCountType *pCurrentRefCount, *pNewRefCount;

  /*-----------------------------------------------------------------------*/
  /* Validate arguments are usable.                                        */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || pCurrentRequest == NULL || pNewRequest == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Find new minimum level.                                               */
  /*-----------------------------------------------------------------------*/

  if (pCurrentRequest->eVRegLevel == pNewRequest->eVRegLevel &&
      pCurrentRequest->bSuppressible == pNewRequest->bSuppressible)
  {
    return DAL_SUCCESS;
  }

  if (pCurrentRequest->bSuppressible)
  {
    pCurrentVRegConfig = &pRail->VRegConfigSuppressible;
  }
  else
  {
    pCurrentVRegConfig = &pRail->VRegConfig;
  }

  if (pNewRequest->bSuppressible)
  {
    pNewVRegConfig = &pRail->VRegConfigSuppressible;
  }
  else
  {
    pNewVRegConfig = &pRail->VRegConfig;
  }

  /*-----------------------------------------------------------------------*/
  /* All domains default to OFF at init, but the ClientCount for OFF will  */
  /* not reflect this.  So we cannot track clients for the OFF voltage     */
  /* level, but this is ok since it is the "do not care" level.            */
  /*-----------------------------------------------------------------------*/

  if (pCurrentRequest->eVRegLevel != RAIL_VOLTAGE_LEVEL_OFF)
  {
    pCurrentRefCount =
      Clock_GetCornerRefCount(pCurrentVRegConfig, pCurrentRequest->eVRegLevel);
    if (pCurrentRefCount->nRefCount > 0)
    {
      pCurrentRefCount->nRefCount--;
    }
  }

  if (pNewRequest->eVRegLevel != RAIL_VOLTAGE_LEVEL_OFF)
  {
    pNewRefCount =
      Clock_GetCornerRefCount(pNewVRegConfig, pNewRequest->eVRegLevel);
    pNewRefCount->nRefCount++;
  }

  pCurrentRequest->eVRegLevel = pNewRequest->eVRegLevel;
  pCurrentRequest->bSuppressible = pNewRequest->bSuppressible;

  /*-----------------------------------------------------------------------*/
  /* Update voltage requests.                                              */
  /*-----------------------------------------------------------------------*/

  Clock_UpdateVoltageRequests(pRail, pNewVRegConfig, pNewRequest->eVRegLevel);
  Clock_UpdateVoltageRequests(pRail, pCurrentVRegConfig, RAIL_VOLTAGE_LEVEL_OFF);

  /*-----------------------------------------------------------------------*/
  /* Success.                                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_VoltageRequest */


/* =========================================================================
**  Function : Clock_VoltageNodeReadyCallback
** =========================================================================*/
/**
  Callback when the PMIC NPA node is ready.

  This function is invoked by the NPA framework when the PMIC NPA node
  is ready to be used.  It will create a client and enter the current
  driver minimum voltage request.

  @param *pContext [in] -- Context pointer, the driver context in this case.

  @return
  NULL

  @dependencies
  None.
*/

void Clock_VoltageNodeReadyCallback
(
  void         *pContext,
  unsigned int  nEventType,
  void         *pResourceName,
  unsigned int  nResourceNameLen
)
{
  ClockRailType *pRail= (ClockRailType *)pContext;
  ClockDrvCtxt *pDrvCtxt = Clock_GetDrvCtxtNoLock();

  /*-----------------------------------------------------------------------*/
  /* Ensure atomicity                                                      */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Create NPA client handle for VDD dig core if it hasn't been created.  */
  /*-----------------------------------------------------------------------*/

  if (pRail->VRegConfig.NPAHandle == NULL)
  {
    pRail->VRegConfig.NPAHandle =
      npa_create_sync_client(
        pRail->szName,
        CLOCK_NPA_CLIENT_NAME,
        NPA_CLIENT_REQUIRED);
  }

  if (pRail->VRegConfigSuppressible.NPAHandle == NULL)
  {
    pRail->VRegConfigSuppressible.NPAHandle =
      npa_create_sync_client(
        pRail->szName,
        CLOCK_NPA_CLIENT_NAME,
        NPA_CLIENT_SUPPRESSIBLE);
  }

  /*-----------------------------------------------------------------------*/
  /* Make a request for the currently required voltage.                    */
  /*-----------------------------------------------------------------------*/

  if (pRail->VRegConfig.NPAHandle &&
      pRail->VRegConfig.eMinLevel != RAIL_VOLTAGE_LEVEL_OFF)
  {
    npa_issue_scalar_request(
      pRail->VRegConfig.NPAHandle,
      pRail->VRegConfig.eMinLevel);
  }

  if (pRail->VRegConfigSuppressible.NPAHandle &&
      pRail->VRegConfigSuppressible.eMinLevel != RAIL_VOLTAGE_LEVEL_OFF)
  {
    npa_issue_scalar_request(
      pRail->VRegConfigSuppressible.NPAHandle,
      pRail->VRegConfigSuppressible.eMinLevel);
  }

  /*-----------------------------------------------------------------------*/
  /* Free mutex.                                                           */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_FREE(pDrvCtxt);

} /* END Clock_VoltageNodeReadyCallback */


/* =========================================================================
**  Function : Clock_VoltageSuspend
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_VoltageSuspend
(
  ClockRailType *pRail
)
{
  pRail->bSuspendVoltageRequest = TRUE;
  return DAL_SUCCESS;

} /* END Clock_VoltageSuspend */


/* =========================================================================
**  Function : Clock_VoltageResume
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_VoltageResume
(
  ClockRailType     *pRail,
  boolean        bUpdate
)
{
  pRail->bSuspendVoltageRequest = FALSE;

  /*-----------------------------------------------------------------------*/
  /* Make a voltage request when resuming from suspension.                 */
  /*-----------------------------------------------------------------------*/

  if (bUpdate)
  {
    if (pRail->VRegConfig.NPAHandle != NULL)
    {
      npa_issue_scalar_request(
        pRail->VRegConfig.NPAHandle,
        pRail->VRegConfig.eMinLevel);
    }

    if (pRail->VRegConfigSuppressible.NPAHandle != NULL)
    {
      npa_issue_scalar_request(
        pRail->VRegConfigSuppressible.NPAHandle,
        pRail->VRegConfigSuppressible.eMinLevel);
    }
  }

  return DAL_SUCCESS;

} /* END Clock_VoltageResume */


/* =========================================================================
**  Function : Clock_InitVoltage
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_InitVoltage
(
  ClockDrvCtxt *pDrvCtxt
)
{
  ClockRailType *pRail;
  DALResult      eResult;
  uint32 i;

  /*-----------------------------------------------------------------------*/
  /* Create callback event to initialize the remote node client.           */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->pBSP->nNumRails; i++)
  {
    pRail = pDrvCtxt->pBSP->aRails[i];

    eResult = VCS_GetBootCorner(pRail->szName, &pRail->eBootCorner);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to determine default boot voltage for %s.", pRail->szName);
      return DAL_ERROR;
    }

    npa_resource_available_cb(
        pRail->szName,
        Clock_VoltageNodeReadyCallback,
        pRail);
  }

  return DAL_SUCCESS;

} /* END Clock_InitVoltage */

