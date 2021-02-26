/*
===========================================================================
*/
/**
  @file ClockSources.c

  Interface implementation file for the sources management of the
  clock device driver.
*/
/*
  ====================================================================

  Copyright (c) 2010-2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Drivers/ClockDxe/ClockSources.c#3 $
  $DateTime: 2017/10/19 21:08:15 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALDeviceId.h"
#include "ClockDriver.h"
#include "HALclk.h"

#ifdef WIN8EA
  #include "ClockQdss.h"
  #ifndef CONSOLE_DEBUGGING
    #include "PEP_wpp.h"
    #include "ClockSources.tmh"
  #endif
#else
  #include "QdssSWEvent.h"
  #include "tracer_event_ids.h"
#endif

/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_SourceNPARequest
** =========================================================================*/
/**
  Thin wrapper to make requests on an NPA-controlled source.

  @param *pSource [in] -- Pointer to a source node.
  @param  nRequest [in] -- NPA request to make.
  @param  bSuppressibleRequest [in] -- TRUE if suppressible; FALSE otherwise.

  @return
  None.
*/

static void Clock_SourceNPARequest
(
  ClockSourceNodeType *pSource,
  uint32               nRequest,
  boolean              bSuppressibleRequest
)
{
  npa_client_handle hNPA;

  if (bSuppressibleRequest)
  {
    hNPA = pSource->NPAHandleSuppressible;
  }
  else
  {
    hNPA = pSource->NPAHandle;
  }

  /*
   * If the NPA handle isn't created yet, then the resource hasn't been
   * created and our callback hasn't fired to create the client handle.
   * This requirement will be satisfied at the time of handle creation.
   */
  if (hNPA)
  {
    npa_issue_scalar_request(hNPA, nRequest);
  }

} /* END Clock_SourceNPARequest */


/* =========================================================================
**  Function : Clock_EnableSourceOutputs
** =========================================================================*/
/**
  Enables all the source outputs specified in the BSP data for this source.

  @param *pSource [in] -- Pointer to a source node.

  @return
  None.
*/

static void Clock_EnableSourceOutputs
(
  ClockSourceNodeType *pSource
)
{
  uint32                  nConfigMask;
  HAL_clk_SourceDescType *pHALSource;

  /*-----------------------------------------------------------------------*/
  /* Return if source is stubbed on this platform.                         */
  /*-----------------------------------------------------------------------*/

  if (pSource->nFlags & CLOCK_FLAG_STUB_HW_RUMI)
  {
    return;
  }

  pHALSource = &pSource->HALSource;
  nConfigMask = pSource->nConfigMask;

  /*-----------------------------------------------------------------------*/
  /* Configure PLL outputs.                                                */
  /*-----------------------------------------------------------------------*/

  if (nConfigMask & CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_AUX_OUTPUT_ENABLE);
  }

  if (nConfigMask & CLOCK_CONFIG_PLL_AUX2_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_ENABLE);
  }

  if (nConfigMask & CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_ENABLE);
  }

  if (nConfigMask & CLOCK_CONFIG_PLL_XO_DOUBLER_DCC)
  {
    HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_XO_DOUBLER_DCC);
  }

  if (nConfigMask & CLOCK_CONFIG_PLL_EVEN_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_EVEN_OUTPUT_ENABLE);
  }

  if (nConfigMask & CLOCK_CONFIG_PLL_ODD_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_ODD_OUTPUT_ENABLE);
  }

  /*
   * Always enable the main output.
   */
  HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_ENABLE);

} /* END Clock_EnableSourceOutputs */


/* =========================================================================
**  Function : Clock_DisableSourceOutputs
** =========================================================================*/
/**
  Disables all the source outputs specified in the BSP data for this source.

  @param *pSource [in] -- Pointer to a source node.

  @return
  None.
*/

static void Clock_DisableSourceOutputs
(
  ClockSourceNodeType *pSource
)
{
  uint32                  nConfigMask;
  HAL_clk_SourceDescType *pHALSource;

  /*-----------------------------------------------------------------------*/
  /* Return if source is stubbed on this platform.                         */
  /*-----------------------------------------------------------------------*/

  if (pSource->nFlags & CLOCK_FLAG_STUB_HW_RUMI)
  {
    return;
  }

  nConfigMask = pSource->nConfigMask;
  pHALSource = &pSource->HALSource;

  /*-----------------------------------------------------------------------*/
  /* Configure PLL outputs.                                                */
  /*-----------------------------------------------------------------------*/

  if (nConfigMask & CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_AUX_OUTPUT_DISABLE);
  }

  if (nConfigMask & CLOCK_CONFIG_PLL_AUX2_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_DISABLE);
  }

  if (nConfigMask & CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_DISABLE);
  }

  if (nConfigMask & CLOCK_CONFIG_PLL_EVEN_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_EVEN_OUTPUT_DISABLE);
  }

  if (nConfigMask & CLOCK_CONFIG_PLL_ODD_OUTPUT_ENABLE)
  {
    HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_ODD_OUTPUT_DISABLE);
  }

  /*
   * Always disable the main output.
   */
  HAL_clk_ConfigSource(pHALSource, HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_DISABLE);

} /* END Clock_DisableSourceOutputs */


/* =========================================================================
**  Function : Clock_SourceOnWithSlew
** =========================================================================*/
/**
  Calibrates a source prior to slewing it to the active freq config.

  This function performs the calibration sequence that is necessary for sources
  that support slewing.  It places the necessary voltage vote based on the
  applied frequency configuration(s).

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param bSuppressibleRequest [in] -- TRUE if this is a source request is for a
                                      suppressible clock.

  @return
  DALResult - SUCCESS if the source was calibrated, ERROR otherwise.
*/

static DALResult Clock_SourceOnWithSlew
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bSuppressibleRequest
)
{
  const ClockSourceFreqConfigType *pCalibrationFreqConfig, *pSlewFreqConfig;
  ClockVRegRequestType             VRegRequest;
  uint32                           nCalibrationMode, nSlewMode;
  boolean                          bResult, bStub;

  /*-----------------------------------------------------------------------*/
  /* Short-circuit if the source is already calibrated.                    */
  /*-----------------------------------------------------------------------*/

  if (pSource->nFlags & CLOCK_FLAG_SOURCE_CALIBRATED)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* We cannot calibrate a source in use.                                  */
  /*-----------------------------------------------------------------------*/

  if ((pSource->nReferenceCount != 0) ||
      (pSource->nReferenceCountSuppressible != 0))
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate this source supports slewing.                                */
  /*-----------------------------------------------------------------------*/

  pCalibrationFreqConfig = pSource->pCalibrationFreqConfig;
  pSlewFreqConfig = pSource->pActiveFreqConfig;
  if ((pCalibrationFreqConfig == NULL) ||
      (pSlewFreqConfig == NULL) ||
      (pSource->nFlags & CLOCK_FLAG_SUPPORTS_SLEWING) == 0)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate the Mode's match between the calibration and slew configs.   */
  /*-----------------------------------------------------------------------*/

  nSlewMode = pSlewFreqConfig->HALConfig.nMode;
  nCalibrationMode = pCalibrationFreqConfig->HALConfig.nMode;
  if (nCalibrationMode != nSlewMode)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Check if source is stubbed on this platform.                          */
  /*-----------------------------------------------------------------------*/

  if (pSource->nFlags & CLOCK_FLAG_STUB_HW_RUMI)
  {
    bStub = TRUE;
  }
  else
  {
    bStub = FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Disable the source outputs to prevent downstream HW from seeing the   */
  /* calibration frequency prior to slewing.                               */
  /*-----------------------------------------------------------------------*/

  Clock_DisableSourceOutputs(pSource);

  /*-----------------------------------------------------------------------*/
  /* Configure the PLL to the calibration frequency.                       */
  /*-----------------------------------------------------------------------*/

  if (bStub != TRUE)
  {
    bResult =
      HAL_clk_ConfigPLL(
        &pSource->HALSource,
        &pCalibrationFreqConfig->HALConfig,
        HAL_CLK_SOURCE_CONFIG_MODE_NORMAL);
    if (bResult != TRUE)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Place voltage vote.                                                   */
  /* The vote is the higher between the calibration and the slew config so */
  /* only one vote is needed during calibration.                           */
  /*-----------------------------------------------------------------------*/

  /*
   * Do not make a voltage request if we've marked the source as having all
   * of its requirements managed by an outside agent.
   * We assume the source's voltage requirement is always satisfied.
   */
  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
    VRegRequest.eVRegLevel =
      MAX(pSlewFreqConfig->eVRegLevel, pCalibrationFreqConfig->eVRegLevel);
    VRegRequest.bSuppressible = bSuppressibleRequest;
    Clock_VoltageRequest(pDrvCtxt, pSource->pRail, &pSource->VRegRequest, &VRegRequest);
  }

  /*-----------------------------------------------------------------------*/
  /* Fully enable the PLL to initiate the calibration sequence.            */
  /*-----------------------------------------------------------------------*/

  if (bStub != TRUE)
  {
    bResult =
      HAL_clk_EnableSource(
        &pSource->HALSource,
        HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
        NULL);
    if (bResult != TRUE)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Slew the PLL to the active config if slewing supported.               */
  /*-----------------------------------------------------------------------*/

  if (pSlewFreqConfig != pCalibrationFreqConfig)
  {
    if (bStub != TRUE)
    {
      bResult =
        HAL_clk_ConfigPLL(
          &pSource->HALSource,
          &pSlewFreqConfig->HALConfig,
          HAL_CLK_SOURCE_CONFIG_MODE_SLEW);
      if (bResult != TRUE)
      {
        return DAL_ERROR_INTERNAL;
      }
    }

    /*
     * As a result of the voltage vote optimization above, we need to check
     * if the slewed configuration voltage requirement is actually lower
     * than the calibration configuration voltage requirement.
     */
    if (pSlewFreqConfig->eVRegLevel < pCalibrationFreqConfig->eVRegLevel)
    {
      /*
       * Do not make a voltage request if we've marked the source as having all
       * of its requirements managed by an outside agent.
       * We assume the source's voltage requirement is always satisfied.
       */
      if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
      {
        VRegRequest.eVRegLevel = pSlewFreqConfig->eVRegLevel;
        VRegRequest.bSuppressible = bSuppressibleRequest;
        Clock_VoltageRequest(pDrvCtxt, pSource->pRail, &pSource->VRegRequest, &VRegRequest);
      }
    }
  }

  /*
   * Re-enable source outputs post calibration.
   */
  Clock_EnableSourceOutputs(pSource);

  /* Logging */
  if ((CLOCK_FLAG_IS_SET(pSource, LOG_STATE_CHANGE) ||
      CLOCK_GLOBAL_FLAG_IS_SET(LOG_SOURCE_STATE_CHANGE)) &&
      !CLOCK_FLAG_IS_SET(pSource, SOURCE_CALIBRATED))
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                      "%s (Calibrated: 1)", pSource->szName);
  }

  /*
   * Update the source state.
   */
  pSource->nFlags |= CLOCK_FLAG_SOURCE_CALIBRATED;

  return DAL_SUCCESS;

} /* END Clock_SourceOnWithSlew */


/* =========================================================================
**  Function : Clock_SourceOn
** =========================================================================*/
/**
  Enables a source.

  This function performs the final operations required to turn on a
  clock source.  It handles deciding whether to use an NPA client request,
  a HAL source vote, or a direct HAL source enable call.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param nNPAState [in] -- State value to use in any NPA request.
  @param bSuppressibleRequest [in] -- TRUE if this is a source request is for a suppressible clock.

  @return
  None.
*/

static void Clock_SourceOn
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  uint32               nNPAState,
  boolean              bSuppressibleRequest
)
{
  ClockVRegRequestType  VRegRequest;
  uint32               *pnCalibration;
  DALResult             eResult;
  boolean               bResult, bStub;

  /*-----------------------------------------------------------------------*/
  /* Check if source is stubbed on this platform.                          */
  /*-----------------------------------------------------------------------*/

  if (pSource->nFlags & CLOCK_FLAG_STUB_HW_RUMI)
  {
    bStub = TRUE;
  }
  else
  {
    bStub = FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* If the source supports slewing but has yet to be calibrated then      */
  /* perform the calibration now and then slew to the active config.       */
  /*-----------------------------------------------------------------------*/

  if ((pSource->nFlags & CLOCK_FLAG_SUPPORTS_SLEWING) &&
      (pSource->nFlags & CLOCK_FLAG_SOURCE_CALIBRATED) == 0)
  {
    eResult = Clock_SourceOnWithSlew(pDrvCtxt, pSource, bSuppressibleRequest);

    if (eResult != DAL_SUCCESS)
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "DALSYS_LOGEVENT_FATAL_ERROR: Unable to calibrate and slew source[%s]",
        pSource->szName);
      ERR_FATAL("Clock_SourceOn", 0, 0, 0);
      return;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Enable the source using an NPA client if it exists, or HAL call       */
  /* otherwise.                                                            */
  /*-----------------------------------------------------------------------*/

  else if (CLOCK_SOURCE_NPA_CONTROLLED(pSource))
  {
    Clock_SourceNPARequest(pSource, nNPAState, bSuppressibleRequest);
  }

  else
  {
    /*-----------------------------------------------------------------------*/
    /* Verify configuration applied before turning on.                       */
    /*-----------------------------------------------------------------------*/

    if (pSource->pActiveFreqConfig == NULL)
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "DALSYS_LOGEVENT_FATAL_ERROR: Cannot turn on source[%s] that hasn't been configured.",
        pSource->szName);
      ERR_FATAL("Clock_SourceOn", 0, 0, 0);
      return;
    }

    /*
     * Assert the voltage requirement.
     *
     * Do not make a voltage request if we've marked the source as having all
     * of its requirements managed by an outside agent.
     * We assume the source's voltage requirement is always satisfied.
     */
    if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
    {
      VRegRequest.eVRegLevel = pSource->pActiveFreqConfig->eVRegLevel;
      VRegRequest.bSuppressible = bSuppressibleRequest;
      Clock_VoltageRequest(pDrvCtxt, pSource->pRail, &pSource->VRegRequest, &VRegRequest);
    }

    if (pSource->nConfigMask & CLOCK_CONFIG_PLL_FSM_MODE_ENABLE)
    {
      if (bStub != TRUE)
      {
        bResult = HAL_clk_EnableSourceVote(&pSource->HALSource);
        if (bResult != TRUE)
        {
          ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                            "DALSYS_LOGEVENT_FATAL_ERROR: Unable to enable source[%s] vote",
            pSource->szName);
          ERR_FATAL("Clock_SourceOn", 0, 0, 0);
          return;
        }
      }
    }
    else
    {
      if (pSource->nFlags & CLOCK_FLAG_SOURCE_CALIBRATED)
      {
        pnCalibration = &pSource->nCalibration;
      }
      else
      {
        pnCalibration = NULL;
      }

      if (bStub != TRUE)
      {
        bResult =
          HAL_clk_EnableSource(
            &pSource->HALSource,
            pSource->eDisableMode,
            pnCalibration);
        if (bResult != TRUE)
        {
          ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                            "DALSYS_LOGEVENT_FATAL_ERROR: Unable to enable source[%s]",
            pSource->szName);
          ERR_FATAL("Clock_SourceOn", 0, 0, 0);
          return;
        }
      }

      /*
       *  Clear stale calibration data after using it.
       */
      pSource->nCalibration = 0;

      /* Logging */
      if ((CLOCK_FLAG_IS_SET(pSource, LOG_STATE_CHANGE) ||
          CLOCK_GLOBAL_FLAG_IS_SET(LOG_SOURCE_STATE_CHANGE)) &&
          !CLOCK_FLAG_IS_SET(pSource, SOURCE_CALIBRATED))
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                          "%s (Calibrated: 1)", pSource->szName);
      }

      /*
       * The source is always calibrated after being enabled the first time.
       * If the calibration data was previously saved (e.g. by HW in standby,
       * by SW in saved) then we restored that calibration data above.
       * Either way we have a PLL that is now calibrated and we mark the flag.
       */
      pSource->nFlags |= CLOCK_FLAG_SOURCE_CALIBRATED;
    }
  }

  /* Logging */
  if (CLOCK_FLAG_IS_SET(pSource, LOG_STATE_CHANGE) ||
      CLOCK_GLOBAL_FLAG_IS_SET(LOG_SOURCE_STATE_CHANGE))
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                      "%s (Enabled: 1)", pSource->szName);
  }

  /*-----------------------------------------------------------------------*/
  /* Log source enable.                                                    */
  /*-----------------------------------------------------------------------*/

  #ifdef WIN8EA
  if (pDrvCtxt->bQdssEnabled)
  {
    PEP_CLK_TRACE_SOURCE_STATUS((uint32)&pSource->HALSource, 1);
  }
  #else
  QDSS_SWEVENT(CLOCK_EVENT_SOURCE_STATUS, &pSource->HALSource);
  #endif

} /* END Clock_SourceOn */


/* =========================================================================
**  Function : Clock_SourceOff
** =========================================================================*/
/**
  Disables a source.

  This function performs the final operations required to turn off a
  clock source.  It handles deciding whether to use an NPA client request,
  a HAL source vote, or a direct HAL source disable call.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param bSuppressibleRequest [in] -- TRUE if this is a source request is for a suppressible clock.

  @return
  None.
*/

static void Clock_SourceOff
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bSuppressibleRequest
)
{
  ClockVRegRequestType           VRegRequest;
  HAL_clk_SourceDisableModeType  eDisableMode;
  uint32                        *pnCalibration = NULL;
  boolean                        bStub;

  /*-----------------------------------------------------------------------*/
  /* Check if source is stubbed on this platform.                          */
  /*-----------------------------------------------------------------------*/

  if (pSource->nFlags & CLOCK_FLAG_STUB_HW_RUMI)
  {
    bStub = TRUE;
  }
  else
  {
    bStub = FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Disable the source using an NPA client if it exists, or HAL call      */
  /* otherwise.                                                            */
  /*-----------------------------------------------------------------------*/

  if (CLOCK_SOURCE_NPA_CONTROLLED(pSource))
  {
    Clock_SourceNPARequest(pSource, CLOCK_SOURCE_NOT_REQUIRED, bSuppressibleRequest);
  }
  else
  {
    if (pSource->nConfigMask & CLOCK_CONFIG_PLL_FSM_MODE_ENABLE)
    {
      if (bStub != TRUE)
      {
        HAL_clk_DisableSourceVote(&pSource->HALSource);
      }
    }
    else
    {
      eDisableMode = pSource->eDisableMode;

      /*
       * Get the source calibration save variable from the context.
       * Clear the calibrated flag for the NORMAL mode only.
       */
      pnCalibration = &pSource->nCalibration;
      if (eDisableMode == HAL_CLK_SOURCE_DISABLE_MODE_NORMAL)
      {
        pSource->nFlags &= ~CLOCK_FLAG_SOURCE_CALIBRATED;
      }

      if (bStub != TRUE)
      {
        HAL_clk_DisableSource(&pSource->HALSource, eDisableMode, pnCalibration);
      }
    }

    /*-----------------------------------------------------------------------*/
    /* Deassert the voltage requirement.                                     */
    /* For PLL's in standby mode:                                            */
    /*  They require no voltage vote since they operate at retention voltage.*/
    /* For PLL's in save/restore mode:                                       */
    /*  They require no voltage vote since the bias is turned off.           */
    /*-----------------------------------------------------------------------*/

    /*
     * Do not make a voltage request if we've marked the source as having all
     * of its requirements managed by an outside agent.
     * We assume the source's voltage requirement is always satisfied.
     */
    if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
    {
      VRegRequest.eVRegLevel = CLOCK_VREG_LEVEL_OFF;
      VRegRequest.bSuppressible = bSuppressibleRequest;
      Clock_VoltageRequest(pDrvCtxt, pSource->pRail, &pSource->VRegRequest, &VRegRequest);
    }
  }

  /* Logging */
  if (CLOCK_FLAG_IS_SET(pSource, LOG_STATE_CHANGE) ||
      CLOCK_GLOBAL_FLAG_IS_SET(LOG_SOURCE_STATE_CHANGE))
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                      "%s (Enabled: 0)", pSource->szName);
  }

  /*-----------------------------------------------------------------------*/
  /* Log source disable.                                                   */
  /*-----------------------------------------------------------------------*/

  #ifdef WIN8EA
  if (pDrvCtxt->bQdssEnabled)
  {
    PEP_CLK_TRACE_SOURCE_STATUS((uint32)&pSource->HALSource, 0);
  }
  #else
  QDSS_SWEVENT(CLOCK_EVENT_SOURCE_STATUS, &pSource->HALSource);
  #endif

} /* END Clock_SourceOff */


/* =========================================================================
**  Function : Clock_EnableSourceInternal
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_EnableSourceInternal
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bSuppressibleRequest
)
{
  ClockVRegRequestType VRegRequest;
  DALResult            eResult;

  /*-----------------------------------------------------------------------*/
  /* Make sure the source handle is valid.                                 */
  /*-----------------------------------------------------------------------*/

  if (pSource == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Check if source requires initial configuration before enabling.       */
  /*-----------------------------------------------------------------------*/

  if (pSource->pActiveFreqConfig == NULL && !pSource->NPAHandle)
  {
    eResult = Clock_InitSource(pDrvCtxt, pSource, 0);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Process insuppressible requests.                                      */
  /*-----------------------------------------------------------------------*/

  if (!bSuppressibleRequest)
  {
    if (pSource->nReferenceCount == 0)
    {
      /*
       * If the source has a source, then recursively enable that.
       * If the current state is enabled for a insuppressible clock, then we
       * need to release the previously registered vote.
       *
       * Note: we do not make a recursive call to enable the source's
       * parent if we've marked the source as having all of its
       * requirements managed by an outside agent. This will not work on
       * targets that have pseudo-sources. E.g. MPLL1_OUT_DIV3 on Dime.
       */
      if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES) &&
          (pSource->pSource != NULL))
      {
        eResult = Clock_EnableSourceInternal(pDrvCtxt, pSource->pSource, FALSE);
        if (eResult != DAL_SUCCESS)
        {
          return eResult;
        }
      }

      /*
       * Turn on the source if it is not already on.  If there is already
       * a suppressible reference count we just need to update the NPA state
       * and voltage request with the insuppressible requirements.
       */
      if (pSource->nReferenceCountSuppressible == 0)
      {
        Clock_SourceOn(pDrvCtxt, pSource, CLOCK_SOURCE_REQUIRED, bSuppressibleRequest);
      }
      else
      {
        if (CLOCK_SOURCE_NPA_CONTROLLED(pSource))
        {
          Clock_SourceNPARequest(pSource, CLOCK_SOURCE_REQUIRED, bSuppressibleRequest);
        }
        else
        {
          /*
           * Do not make a voltage request if we've marked the source as having all
           * of its requirements managed by an outside agent.
           * We assume the source's voltage requirement is always satisfied.
           */
          if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES) && pSource->pActiveFreqConfig != NULL)
          {
            VRegRequest.eVRegLevel = pSource->pActiveFreqConfig->eVRegLevel;
            VRegRequest.bSuppressible = FALSE;
            Clock_VoltageRequest(pDrvCtxt, pSource->pRail, &pSource->VRegRequest, &VRegRequest);
          }
        }
      }
    }

    /*
     * Increment the reference count.
     */
    pSource->nReferenceCount++;
  }

  /*-----------------------------------------------------------------------*/
  /* Process suppressible request.                                         */
  /*-----------------------------------------------------------------------*/

  else
  {
    if (pSource->nReferenceCountSuppressible == 0)
    {
      /*
       * If the source has a source, then recursively enable that.
       *
       * Note: we do not make a recursive call to enable the source's
       * parent if we've marked the source as having all of its
       * requirements managed by an outside agent. This will not work on
       * targets that have pseudo-sources. E.g. MPLL1_OUT_DIV3 on Dime.
       */
      if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES) &&
          (pSource->pSource != NULL))
      {
        eResult = Clock_EnableSourceInternal(pDrvCtxt, pSource->pSource, TRUE);
        if (eResult != DAL_SUCCESS)
        {
          return eResult;
        }
      }

      /*
       * If the source is not already enabled by an insuppressible request
       * then enable it.  Use "0" for any NPA request to indicate
       * suppressible-based and that the source is not required. If there is
       * already an insuppressible reference count, then just update the voltage
       * request with the suppressible requirement.
       */
      if (pSource->nReferenceCount == 0)
      {
        Clock_SourceOn(
          pDrvCtxt, pSource, CLOCK_SOURCE_NOT_REQUIRED, TRUE);
      }
    }

    /*
     * Increment the reference count.
     */
    pSource->nReferenceCountSuppressible++;
  }

  return DAL_SUCCESS;

} /* END Clock_EnableSourceInternal */


/* =========================================================================
**  Function : Clock_DisableSourceInternal
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_DisableSourceInternal
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bSuppressibleRequest,
  boolean              bDoNotDisable
)
{
  ClockVRegRequestType VRegRequest;
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Make sure the source handle is valid.                                 */
  /*-----------------------------------------------------------------------*/

  if (pSource == NULL)
  {
    return DAL_ERROR;
  }

  bDoNotDisable =
    bDoNotDisable ||
    CLOCK_FLAG_IS_SET(pSource, DO_NOT_DISABLE) ||
    CLOCK_GLOBAL_FLAG_IS_SET(DO_NOT_DISABLE_SOURCES);

  /*-----------------------------------------------------------------------*/
  /* Process insuppressible requests.                                      */
  /*-----------------------------------------------------------------------*/

  if (!bSuppressibleRequest)
  {
    /*
     * Decrement insuppressible reference count.
     */
    if (pSource->nReferenceCount > 0)
    {
      pSource->nReferenceCount--;
    }

    /*
     * If the count is zero and the DO_NOT_DISABLE bits aren't set, we can
     * disable the source.
     */
    if (pSource->nReferenceCount == 0)
    {
      if (!bDoNotDisable)
      {
        /*
         * If there is no suppressible reference count then disable the source.
         * If there are suppressible requests on this source then we just remove
         * the insuppressible requirement from the NPA state and voltage request.
         */
        if (pSource->nReferenceCountSuppressible == 0)
        {
          Clock_SourceOff(pDrvCtxt, pSource, FALSE);
        }
        else
        {
          if (CLOCK_SOURCE_NPA_CONTROLLED(pSource))
          {
            Clock_SourceNPARequest(pSource, CLOCK_SOURCE_NOT_REQUIRED, bSuppressibleRequest);
          }
          else
          {
            /*
             * Do not make a voltage request if we're bypassing the DAL driver for
             * this source. We assume the source's voltage requirement is always
             * satisfied.
             */
            if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES) &&
                pSource->pActiveFreqConfig != NULL)
            {
              VRegRequest.eVRegLevel = pSource->pActiveFreqConfig->eVRegLevel;
              VRegRequest.bSuppressible = TRUE;
              Clock_VoltageRequest(pDrvCtxt, pSource->pRail, &pSource->VRegRequest, &VRegRequest);
            }
          }
        }
      }

      /*
       * If the source has a source, then recursively disable that.
       *
       * Note: we do not make a recursive call to disable the source's
       * parent if we've marked the source as having all of its
       * requirements managed by an outside agent. This will not work on
       * targets that have pseudo-sources. E.g. MPLL1_OUT_DIV3 on Dime.
       */
      if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES) &&
          (pSource->pSource != NULL))
      {
        eResult = Clock_DisableSourceInternal(pDrvCtxt, pSource->pSource, FALSE, bDoNotDisable);
        if (eResult != DAL_SUCCESS)
        {
          return eResult;
        }
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Process suppressible request.                                         */
  /*-----------------------------------------------------------------------*/

  else
  {
    if (pSource->nReferenceCountSuppressible > 0)
    {
      pSource->nReferenceCountSuppressible--;
    }

    if (pSource->nReferenceCountSuppressible == 0)
    {
      if (!bDoNotDisable)
      {
        /*
         * Disable the source if there aren't any insuppressible references. If
         * there is an insuppressible reference count, then just remove the
         * suppressible voltage request.
         */
        if (pSource->nReferenceCount == 0)
        {
          Clock_SourceOff(pDrvCtxt, pSource, TRUE);
        }
      }

      /*
       * If the source has a source, then recursively disable that.
       *
       * Note: we do not make a recursive call to disable the source's
       * parent if we've marked the source as having all of its
       * requirements managed by an outside agent. This will not work on
       * targets that have pseudo-sources. E.g. MPLL1_OUT_DIV3 on Dime.
       */
      if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES) &&
          (pSource->pSource != NULL))
      {
        eResult = Clock_DisableSourceInternal(pDrvCtxt, pSource->pSource, TRUE, bDoNotDisable);
        if (eResult != DAL_SUCCESS)
        {
          return eResult;
        }
      }
    }
  }

  return DAL_SUCCESS;

} /* END Clock_DisableSourceInternal */


/* =========================================================================
**  Function : Clock_ConfigSource
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_ConfigSource
(
  ClockDrvCtxt                     *pDrvCtxt,
  ClockSourceNodeType              *pSource,
  const ClockSourceFreqConfigType  *pFreqConfig
)
{
  const ClockSourceFreqConfigType *pActiveFreqConfig;
  ClockVRegRequestType             VRegRequest;
  boolean                          bResult, bStub;

  /*-----------------------------------------------------------------------*/
  /* Make sure the source handle is valid.                                 */
  /*-----------------------------------------------------------------------*/

  if (pSource == NULL || pFreqConfig == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Short-circuit if this config is already active.                       */
  /*-----------------------------------------------------------------------*/

  if (pFreqConfig == pSource->pActiveFreqConfig)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Check if source is stubbed on this platform.                          */
  /*-----------------------------------------------------------------------*/

  if (pSource->nFlags & CLOCK_FLAG_STUB_HW_RUMI)
  {
    bStub = TRUE;
  }
  else
  {
    bStub = FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Check if this source supports slewing (dynamic configuration).        */
  /*-----------------------------------------------------------------------*/

  if ((pSource->nReferenceCount == 0) &&
      (pSource->nReferenceCountSuppressible == 0))
  {
    /*
     * Configure the PLL.
     */
    if (bStub != TRUE)
    {
      bResult =
        HAL_clk_ConfigPLL(
          &pSource->HALSource,
          &pFreqConfig->HALConfig,
          HAL_CLK_SOURCE_CONFIG_MODE_NORMAL);
      if (bResult != TRUE)
      {
        return DAL_ERROR_INTERNAL;
      }
    }
  }
  else
  {
    /*
     * Cannot configure a source while in use that doesn't support slewing.
     */
    if ((pSource->nFlags & CLOCK_FLAG_SUPPORTS_SLEWING) == 0)
    {
      return DAL_ERROR;
    }

    /*
     * Cannot slew the source if it hasn't been calibrated.
     */
    if ((pSource->nFlags & CLOCK_FLAG_SOURCE_CALIBRATED) == 0)
    {
      return DAL_ERROR;
    }

    /*
     * Verify active frequency configured to slew from.
     */
    if (pSource->pActiveFreqConfig == NULL)
    {
      return DAL_ERROR;
    }

    /*
     * Cannot slew between Mode's - this requires a switch away from the source.
     */
    pActiveFreqConfig = pSource->pActiveFreqConfig;
    if (pFreqConfig->HALConfig.nMode != pActiveFreqConfig->HALConfig.nMode)
    {
      return DAL_ERROR;
    }

    /*
     * It isn't know whether we can update the ALPHA_EN field while slewing.
     * Catch this condition until this feature is validated.
     */
    if (!pFreqConfig->HALConfig.nFracVal !=
        !pActiveFreqConfig->HALConfig.nFracVal)
    {
      return DAL_ERROR;
    }

    /*
     * Increase the voltage vote if the new config has a higher requirement.
     *
     * Do not make a voltage request if we've marked the source as having all
     * of its requirements managed by an outside agent.
     * We assume the source's voltage requirement is always satisfied.
     */
    if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES) &&
        (pFreqConfig->eVRegLevel > pActiveFreqConfig->eVRegLevel))
    {
      VRegRequest.eVRegLevel = pFreqConfig->eVRegLevel;
      VRegRequest.bSuppressible = pSource->VRegRequest.bSuppressible;
      Clock_VoltageRequest(pDrvCtxt, pSource->pRail, &pSource->VRegRequest, &VRegRequest);
    }

    /*
     * Slew the PLL.
     */
    if (bStub != TRUE)
    {
      bResult =
        HAL_clk_ConfigPLL(
          &pSource->HALSource,
          &pFreqConfig->HALConfig,
          HAL_CLK_SOURCE_CONFIG_MODE_SLEW);
      if (bResult != TRUE)
      {
        return DAL_ERROR_INTERNAL;
      }
    }

    /*
     * Decrease the voltage vote if the new config has a lower requirement.
     *
     * Do not make a voltage request if we've marked the source as having all
     * of its requirements managed by an outside agent.
     * We assume the source's voltage requirement is always satisfied.
     */
    if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES) &&
        (pFreqConfig->eVRegLevel < pActiveFreqConfig->eVRegLevel))
    {
      VRegRequest.eVRegLevel = pFreqConfig->eVRegLevel;
      VRegRequest.bSuppressible = pSource->VRegRequest.bSuppressible;
      Clock_VoltageRequest(pDrvCtxt, pSource->pRail, &pSource->VRegRequest, &VRegRequest);
    }

  }

  /*-----------------------------------------------------------------------*/
  /* Update state.                                                         */
  /*-----------------------------------------------------------------------*/

  pSource->pActiveFreqConfig = pFreqConfig;

  /* TODO: Properly handle dynamically switching between reference sources. */
  pSource->pSource = pFreqConfig->pSource;

  /*-----------------------------------------------------------------------*/
  /* Log source config change.                                             */
  /*-----------------------------------------------------------------------*/

  if (CLOCK_FLAG_IS_SET(pSource, LOG_FREQ_CHANGE) ||
      CLOCK_GLOBAL_FLAG_IS_SET(LOG_SOURCE_FREQ_CHANGE))
  {
    ULOG_RT_PRINTF_2 (pDrvCtxt->hClockLog,
                      "%s (Freq: %lu)",
                      pSource->szName, pFreqConfig->nFreqHz);
  }

  return DAL_SUCCESS;

} /* END Clock_ConfigSource */


/* =========================================================================
**  Function : Clock_SourceResourceAvailableCallback
** =========================================================================*/
/**
  Callback when source NPA resource is ready.

  @param *pContext [in] -- Context pointer, the source pointer in this case.
  @param  nEventType [in] -- 0 for this callback type.
  @param  pResourceName [in] -- Pointer to resource name string.
  @param  nResourceNameLen [in] -- Length of pResourceName string.

  @return
  NULL

  @dependencies
  None.
*/

static void Clock_SourceResourceAvailableCallback
(
  void         *pContext,
  unsigned int  nEventType,
  void         *pResourceName,
  unsigned int  nResourceNameLen
)
{
  ClockSourceNodeType *pSource = pContext;
  const char          *szResource = pResourceName;
  ClockDrvCtxt        *pDrvCtxt = Clock_GetDrvCtxt();

  /*
   * Multiple callbacks may be registered. Nothing to do after first
   * callback has fired.
   */
  if (pSource->NPAHandle)
  {
    return;
  }

  /*
   * Create client handles.
   */
  pSource->NPAHandle = npa_create_sync_client(szResource,
    CLOCK_NPA_CLIENT_NAME, NPA_CLIENT_REQUIRED);
  pSource->NPAHandleSuppressible = npa_create_sync_client(szResource,
    CLOCK_NPA_CLIENT_NAME, NPA_CLIENT_SUPPRESSIBLE);

  if (!pSource->NPAHandle || !pSource->NPAHandleSuppressible)
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                      "DALSYS_LOGEVENT_FATAL_ERROR: Unable to create NPA handles for %s.",
                      pSource->szName);
    ERR_FATAL("Clock_SourceResourceAvailableCallback", 0, 0, 0);
    return;
  }

  /*
   * Vote for current requirements.
   */
  if (pSource->nReferenceCount > 0)
  {
    npa_issue_scalar_request(pSource->NPAHandle, CLOCK_SOURCE_REQUIRED);
  }
  if (pSource->nReferenceCountSuppressible > 0)
  {
    npa_issue_scalar_request(pSource->NPAHandleSuppressible, CLOCK_SOURCE_REQUIRED);
  }

} /* END Clock_SourceResourceAvailableCallback */


/* =========================================================================
**  Function : Clock_InitSource
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_InitSource
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  uint32               nFreqHz
)
{
  boolean                          bStub;
  DALResult                        eResult;
  const ClockSourceFreqConfigType *pSourceFreqConfig;
  uint32                           i;

  /*-----------------------------------------------------------------------*/
  /* Verify configuration.                                                 */
  /*-----------------------------------------------------------------------*/

  if (pSource == NULL)
  {
    return DAL_ERROR;
  }

  /*
   * If a source is NPA-controlled, then just setup a callback for the
   * resource to become available and return.
   */
  if (CLOCK_SOURCE_NPA_CONTROLLED(pSource))
  {
    npa_resource_available_cb(
      pSource->szName,
      Clock_SourceResourceAvailableCallback,
      pSource);
    return DAL_SUCCESS;
  }

  /*
   * If a source doesn't have any BSP data (like for XO), then no
   * initialization to be performed.
   */
  if (pSource->aBSP == NULL)
  {
    return DAL_SUCCESS;
  }

  /*
   * If source is read-only, then it should already be configured by an
   * earlier SW image. Just hookup the active configuration and don't
   * touch HW.
   */
  if (CLOCK_FLAG_IS_SET(pSource, READ_ONLY))
  {
    /* TODO: Detect based on version for different voltages? */
    pSource->pActiveFreqConfig = pSource->aBSP;
    pSource->pSource = pSource->pActiveFreqConfig->pSource;
    return DAL_SUCCESS;
  }

  /*
   * If we successfully detect the configuration then don't re-initialize.
   */
  eResult = Clock_DetectSourceConfig(pDrvCtxt, pSource);
  if (eResult == DAL_SUCCESS)
  {
    return DAL_SUCCESS;
  }

  /*
   * Hookup the calibration frequency config if applicable.
   */
  if (pSource->nFlags & CLOCK_FLAG_SUPPORTS_SLEWING &&
      pSource->pCalibrationFreqConfig != NULL)
  {
    /*
     * Iterate over source freq configs until we reach the end marker.
     */
    for (i = 0; i < pSource->nCalibrationFreqConfigLen; i++)
    {
      pSourceFreqConfig = &pSource->pCalibrationFreqConfig[i];

      if (Clock_IsBSPSupported(&pSourceFreqConfig->HWVersion) == TRUE)
      {
        pSource->pCalibrationFreqConfig = pSourceFreqConfig;

        /*
         * Exit this search loop.
         */
        break;
      }
    }

    /*
     * Validate a calibration frequency was found.
     */
    if (i == pSource->nCalibrationFreqConfigLen)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  /*
   * Try to find the exact config in the BSP.
   */
  pSourceFreqConfig = NULL;
  if (nFreqHz != 0)
  {
    eResult =
      Clock_FindSourceConfig(
        pSource,
        nFreqHz,
        CLOCK_FREQUENCY_HZ_EXACT,
        &pSourceFreqConfig);

    if ((eResult != DAL_SUCCESS) || (pSourceFreqConfig == NULL))
    {
      return eResult;
    }
  }
  /*
   * Otherwise we need to figure out which config to apply.
   */
  else
  {
    /*
     * The source may have already been configured.
     * Leave the config unchanged.
     */
    if (pSource->pActiveFreqConfig != NULL)
    {
      pSourceFreqConfig = pSource->pActiveFreqConfig;
    }
    /*
     * Otherwise try to match the first supported config in the BSP.
     */
    else
    {
      eResult =
        Clock_FindSourceConfig(
            pSource,
            0,
            CLOCK_FREQUENCY_HZ_CLOSEST,
            &pSourceFreqConfig);

      if ((eResult != DAL_SUCCESS) || (pSourceFreqConfig == NULL))
      {
        return eResult;
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Check if source is stubbed on this platform.                          */
  /*-----------------------------------------------------------------------*/

  if (pSource->nFlags & CLOCK_FLAG_STUB_HW_RUMI)
  {
    bStub = TRUE;
  }
  else
  {
    bStub = FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* The FSM output is not glitch free - only enable at init.              */
  /*-----------------------------------------------------------------------*/

  /*
   * Do this before Clock_ConfigSource, since HAL_clk_ConfigPLL often relies
   * on this bit being set in the register to setup diffently for FSM mode.
   */
  if (pSource->nConfigMask & CLOCK_CONFIG_PLL_FSM_MODE_ENABLE)
  {
    if (bStub != TRUE)
    {
      HAL_clk_ConfigSource(&pSource->HALSource, HAL_CLK_CONFIG_PLL_FSM_MODE_ENABLE);
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Apply the new active config.                                          */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_ConfigSource(pDrvCtxt, pSource, pSourceFreqConfig);

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Enable the source outputs for when the source is turned on.           */
  /*-----------------------------------------------------------------------*/

  Clock_EnableSourceOutputs(pSource);

  /*-----------------------------------------------------------------------*/
  /* Done.                                                                 */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitSource */


/* =========================================================================
**  Function : Clock_FindSourceConfig
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_FindSourceConfig
(
  ClockSourceNodeType              *pSource,
  uint32                            nFreqHz,
  ClockFrequencyType                eMatch,
  const ClockSourceFreqConfigType **pMatchingConfig
)
{
  uint32                           nAtLeastFrequency = 0xFFFFFFFF;
  const ClockSourceFreqConfigType *pAtLeastConfig = NULL;
  uint32                           nAtMostFrequency = 0;
  const ClockSourceFreqConfigType *pAtMostConfig = NULL;
  const ClockSourceFreqConfigType *pBSPConfig;
  uint32 i;

  /* Make sure the list of configurations isn't empty */
  if (pSource == NULL ||
      pSource->aBSP == NULL ||
      pMatchingConfig == NULL)
  {
    return DAL_ERROR;
  }

  /*
   * Otherwise find config based on the nFreqHz field.
   * Loop over all configurations.
   */
  for (i = 0; i < pSource->nBSPLen; i++)
  {
    pBSPConfig = &pSource->aBSP[i];
    if (!Clock_IsBSPSupported(&pBSPConfig->HWVersion))
    {
      continue;
    }

    /* Stop if we find an exact match */
    if (pBSPConfig->nFreqHz == nFreqHz)
    {
      *pMatchingConfig = pBSPConfig;
      return DAL_SUCCESS;
    }

    /* Look for the lowest at-least frequency */
    if ((pBSPConfig->nFreqHz > nFreqHz) &&
        (pBSPConfig->nFreqHz < nAtLeastFrequency))
    {
      nAtLeastFrequency = pBSPConfig->nFreqHz;
      pAtLeastConfig = pBSPConfig;
    }
    /* Look for the highest at-most frequency */
    else if ((pBSPConfig->nFreqHz < nFreqHz) &&
             (pBSPConfig->nFreqHz > nAtMostFrequency))
    {
      nAtMostFrequency = pBSPConfig->nFreqHz;
      pAtMostConfig = pBSPConfig;
    }
  }

  /* Select a config based on the match criteria */
  switch (eMatch)
  {
    case CLOCK_FREQUENCY_HZ_AT_LEAST:
      if (pAtLeastConfig != NULL)
      {
        *pMatchingConfig = pAtLeastConfig;
        return DAL_SUCCESS;
      }
      break;

    case CLOCK_FREQUENCY_HZ_AT_MOST:
      if (pAtMostConfig != NULL)
      {
        *pMatchingConfig = pAtMostConfig;
        return DAL_SUCCESS;
      }
      break;

    case CLOCK_FREQUENCY_HZ_CLOSEST:
      if ((pAtLeastConfig != NULL) && (pAtMostConfig == NULL))
      {
        *pMatchingConfig = pAtLeastConfig;
        return DAL_SUCCESS;
      }
      else if ((pAtMostConfig != NULL) && (pAtLeastConfig == NULL))
      {
        *pMatchingConfig = pAtMostConfig;
        return DAL_SUCCESS;
      }
      else if ((pAtLeastConfig != NULL) && (pAtMostConfig != NULL))
      {
        /* Select the closest match, select the lowest in case of a tie */
        if ((nAtLeastFrequency - nFreqHz) < (nFreqHz - nAtMostFrequency))
        {
          *pMatchingConfig = pAtLeastConfig;
          return DAL_SUCCESS;
        }
        else
        {
          *pMatchingConfig = pAtMostConfig;
          return DAL_SUCCESS;
        }
      }
      break;

    default:
      break;
  }

  /* No match was found */
  *pMatchingConfig = NULL;

  return DAL_ERROR;

} /* END Clock_FindSourceConfig */


/* =========================================================================
**  Function : Clock_DetectSourceConfig
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_DetectSourceConfig
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource
)
{
  HAL_clk_PLLConfigType            DetectedConfig = {0};
  const ClockSourceFreqConfigType *pFreqConfig;
  uint32                           i;

  if (!pSource->aBSP || pSource->pActiveFreqConfig)
  {
    return DAL_SUCCESS;
  }

  HAL_clk_DetectPLLConfig(&pSource->HALSource, &DetectedConfig);

  for (i = 0; i < pSource->nBSPLen; i++)
  {
    pFreqConfig = &pSource->aBSP[i];
    if (!Clock_IsBSPSupported(&pFreqConfig->HWVersion))
    {
      continue;
    }

    if (HAL_clk_IsPLLConfigEqual(&pSource->HALSource,
                                 &pFreqConfig->HALConfig, &DetectedConfig))
    {
      pSource->pActiveFreqConfig = pFreqConfig;
      pSource->pSource = pFreqConfig->pSource;

      /*
       * If source has already been configured and is slewable, then assume
       * it has also been calibrated as well.
       */
      if (CLOCK_FLAG_IS_SET(pSource, SUPPORTS_SLEWING))
      {
        CLOCK_FLAG_SET(pSource, SOURCE_CALIBRATED);
      }

      return DAL_SUCCESS;
    }
  }

  return DAL_ERROR;

} /* END Clock_DetectDetectedConfig */


/* =========================================================================
**  Function : Clock_FindSourceConfigByVersion
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_FindSourceConfigByVersion
(
  ClockSourceFreqConfigType   *pBSPConfig,
  ClockSourceFreqConfigType  **pMatchingConfig
)
{

  /* Make sure the list of configurations isn't empty */
  if (pBSPConfig == NULL || pMatchingConfig == NULL)
  {
    return DAL_ERROR;
  }

  /* Loop over all configurations */
  while (pBSPConfig->nFreqHz != 0)
  {
    /*
     * Skip configurations that aren't for this chip or version.
     */
    if (Clock_IsBSPSupported(&pBSPConfig->HWVersion))
    {
      *pMatchingConfig = pBSPConfig;
      return DAL_SUCCESS;
    }

    /* Move to the next configuration */
    pBSPConfig++;
  }

  /* No match was found */
  *pMatchingConfig = NULL;

  return DAL_ERROR;

} /* END Clock_FindSourceConfigByVersion */
