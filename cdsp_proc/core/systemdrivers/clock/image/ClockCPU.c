/*
===========================================================================
*/
/**
  @file ClockCPU.c

*/
/*
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/image/ClockCPU.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $
  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockImage.h"
#include "npa_resource.h"
#include "npa_scheduler.h"

/*=========================================================================
      Macros
==========================================================================*/

/*
 * NPA Resource handles
 */
#define NPA_RESOURCE_CLK_CPU                                              \
  Clock_aNPACPUResources[CLOCK_IMAGE_NPA_RESOURCE_CLK_CPU].handle

#define NPA_RESOURCE_CLK_CPU_TEST                                         \
  Clock_aNPACPUResources[CLOCK_IMAGE_NPA_RESOURCE_CLK_CPU_TEST].handle


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Enumeration of local CPU resources.
 */
enum
{
  CLOCK_IMAGE_NPA_RESOURCE_CLK_CPU,
  CLOCK_IMAGE_NPA_RESOURCE_CLK_CPU_TEST,
  CLOCK_IMAGE_NPA_RESOURCE_TOTAL
};


/*=========================================================================
      Prototypes
==========================================================================*/

static npa_resource_state Clock_NPACPUNodeDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
);

static npa_resource_state Clock_NPACPUNodeDriverFuncTest
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
);

static void Clock_NPACPUSetFrequency
(
  ClockDrvCtxt        *pDrvCtxt,
  uint32         nFreqHz
);

static void Clock_NPACPUSetFrequencyNAS
(
  ClockDrvCtxt        *pDrvCtxt,
  uint32         nFreqHz
);

static void Clock_NPACPUNASEventCallback
(
  void *pData
);


/*=========================================================================
      Data
==========================================================================*/

/*
 * Clock CPU NPA resources.
 */
static npa_resource_definition Clock_aNPACPUResources[CLOCK_IMAGE_NPA_RESOURCE_TOTAL] =
{
  [CLOCK_IMAGE_NPA_RESOURCE_CLK_CPU] = {
    .name       = "/clk/cpu",
    .units      = "KHz",
    .plugin     = &npa_max_plugin,
    .attributes = NPA_RESOURCE_DEFAULT,
  },

  [CLOCK_IMAGE_NPA_RESOURCE_CLK_CPU_TEST] = {
    .name       = "/clk/cpu.test",
    .units      = "KHz",
    .plugin     = &npa_max_plugin,
    .attributes = NPA_RESOURCE_SINGLE_CLIENT,
  },
};

/*
 * Clock CPU NPA node.
 */
static npa_node_definition Clock_NPACPUNode =
{
  .name           = "/node/clk/cpu",
  .driver_fcn     = Clock_NPACPUNodeDriverFunc,
  .attributes     = NPA_NODE_DEFAULT,
  .resource_count = ARR_SIZE(Clock_aNPACPUResources),
  .resources      = Clock_aNPACPUResources,
};


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_InitCPU
** =========================================================================*/
/*
  See ClockImage.h
*/

DALResult Clock_InitCPU
(
  ClockDrvCtxt *pDrvCtxt
)
{
  ClockImageCtxtType  *pImageCtxt = pDrvCtxt->pImageCtxt;
  ClockDALClientCtxt  *pClientCtxt = Clock_GetInitialClientCtxt();
  ClockNodeType       *pClock;
  ClockIdType          nClockId;
  npa_resource_state   anInitStates[CLOCK_IMAGE_NPA_RESOURCE_TOTAL] = {0};
  DALResult            eResult;
  uint32               nMaxFreqKHz;
  uint32               i;

  /*-----------------------------------------------------------------------*/
  /* Get CPU clock ID.                                                     */
  /*-----------------------------------------------------------------------*/

  Clock_GetClockId(pDrvCtxt, pImageCtxt->pBSP->szCPUClockName, &nClockId);
  pClock = Clock_GetClockNode(pDrvCtxt, nClockId);
  if (!pClock || !pClock->pDomain || !pClock->pDomain->aBSP)
  {
    return DAL_ERROR_INTERNAL;
  }

  pImageCtxt->CPUCtxt.pClock = pClock;

  /*-----------------------------------------------------------------------*/
  /* Ensure that the CPU core clock/domain/source reference counts are 1.  */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_EnableClock(pClientCtxt, pClock);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Define CPU NPA node.                                                  */
  /*-----------------------------------------------------------------------*/

  Clock_NPACPUNode.data = pDrvCtxt;
  nMaxFreqKHz = pClock->pDomain->aBSP[pClock->pDomain->nBSPLen-1].nFreqHz / 1000;
  for (i = 0; i < CLOCK_IMAGE_NPA_RESOURCE_TOTAL; i++)
  {
    Clock_aNPACPUResources[i].max = nMaxFreqKHz;
  }

  /* Set initial frequency from BSP, capped to the max frequency. */
  anInitStates[CLOCK_IMAGE_NPA_RESOURCE_CLK_CPU] =
    MIN(nMaxFreqKHz, pImageCtxt->pBSP->nInitCPUState);

  npa_define_node_cb(&Clock_NPACPUNode, anInitStates, NULL, NULL);

  /*
   * Set flag after defining node, so initial state can still take affect
   * if set in BSP. This allows for pegging the CPU to max.
   */
  if (pImageCtxt->pBSP->bDisableDCS)
  {
    pImageCtxt->CPUCtxt.nDisableDCS |= CLOCK_FLAG_DISABLED_BY_BSP;
  }

  return DAL_SUCCESS;

} /* END Clock_InitCPU */


/* =========================================================================
**  Function : Clock_NPACPUNodeDriverFunc
** =========================================================================*/
/**
  NPA node driver function for the CPU node.

  This function handles minimum frequency requests for the CPU NPA node.

  @param *pResource [in] -- Pointer to resource whose state changed.
  @param hClient    [in] -- Client handle that triggered the change.
  @param nState     [in] -- New request state.

  @return
  npa_resource_state -- The new active state of the resource.

  @dependencies
  None.
*/

static npa_resource_state Clock_NPACPUNodeDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  ClockDrvCtxt        *pDrvCtxt = (ClockDrvCtxt *)pResource->node->data;
  ClockImageCtxtType  *pImageCtxt = pDrvCtxt->pImageCtxt;
  uint32               nFreqHz = NPA_TO_HZ(nState);
  ClockDomainNodeType *pDomain = pImageCtxt->CPUCtxt.pClock->pDomain;

  /*
   * Call separate function for the test resource.
   */
  if (pResource == NPA_RESOURCE_CLK_CPU_TEST)
  {
    return Clock_NPACPUNodeDriverFuncTest(pResource, hClient, nState);
  }

  /*-----------------------------------------------------------------------*/
  /* Perform necessary setup during resource creation.                     */
  /*-----------------------------------------------------------------------*/

  if (hClient->type == NPA_CLIENT_INITIALIZE)
  {
    CLOCK_FLAG_SET(pDomain, INTERNAL_CONTROL);

    /* If initial state is 0, then keep boot configuration. */
    if (nFreqHz == 0)
    {
      if (pDomain->pActiveMuxConfig)
      {
        return pDomain->pActiveMuxConfig->nFreqHz / 1000;
      }
      else
      {
        return 0;
      }
    }
  }

  /*
   * If frequency scaling is disabled, just return the active state without
   * changing anything.
   */
  if (pImageCtxt->CPUCtxt.nDisableDCS)
  {
    return pResource->active_state;
  }

  /*-----------------------------------------------------------------------*/
  /* Update clock frequency.                                               */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  if (npa_request_has_attribute(hClient, NPA_REQUEST_NEXT_AWAKE))
  {
    Clock_NPACPUSetFrequencyNAS(pDrvCtxt, nFreqHz);
  }
  else
  {
    Clock_NPACPUSetFrequency(pDrvCtxt, nFreqHz);
  }

  DALCLOCK_FREE(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Clear the FIRE_AND_FORGET attribute because this resource makes no    */
  /* use of that resource and the NPA framework makes decisions if that    */
  /* attribute is set that is unfriendly for local NAS aware resources.    */
  /*-----------------------------------------------------------------------*/

  hClient->request_attr &= ~NPA_REQUEST_FIRE_AND_FORGET;

  return pDomain->pActiveMuxConfig->nFreqHz / 1000;

} /* END Clock_NPACPUNodeDriverFunc */


/* =========================================================================
**  Function : Clock_NPACPUSetFrequency
** =========================================================================*/
/**
  Set CPU domain configuration.

  This functions sets the CPU domain configuration.

  @param *pDrvCtxt    [in] -- Driver context.
  @param *pNewConfig  [in] -- New configuration to use.

  @return
  DAL_SUCCESS -- The domain was successfully configured.
  DAL_ERROR -- The domain configuration failed.

  @dependencies
  None.
*/

static void Clock_NPACPUSetFrequency
(
  ClockDrvCtxt        *pDrvCtxt,
  uint32         nFreqHz
)
{
  ClockImageCtxtType  *pImageCtxt = pDrvCtxt->pImageCtxt;
  ClockNodeType       *pClock = pImageCtxt->CPUCtxt.pClock;
  uint32               nPrevPriority;
  DALResult            eResult;

  /*
   * The NPA framework doesn't support immediate requests that occur after
   * NAS requests but before going through a sleep transition.  Raise an
   * error here if that case is detected.
   */
  if (pImageCtxt->CPUCtxt.pNASConfig != NULL)
  {
    if (npa_is_next_wakeup_after_nas() != TRUE)
    {
      DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Invalid request for frequency[%lu] when pending NAS request for frequency[%lu].",
        nFreqHz, pImageCtxt->CPUCtxt.pNASConfig->nFreqHz);
      while(1);
    }

    pImageCtxt->CPUCtxt.pNASConfig = NULL;
  }

  /*
   * Set highest thread priority prior to switch to prevent being preempted.
   * If thread is preempted after RCG divider is increased and before PLL is
   * slewed, then CPU can get stuck at low frequency until scheduled again.
   */
  Clock_SetThreadPriority(CLOCK_THREAD_PRIORITY_HIGHEST, &nPrevPriority);

  /*
   * Set frequency.
   */
  eResult = Clock_SetClockFrequency(pDrvCtxt, pClock,
    nFreqHz, CLOCK_FREQUENCY_HZ_AT_LEAST, NULL);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to set CPU clock to %lu", nFreqHz);
    while(1);
  }

  /*
   * Restore thread's previous priority
   */
  Clock_SetThreadPriority(nPrevPriority, NULL);

} /* END Clock_NPACPUSetFrequency */


/* =========================================================================
**  Function : Clock_NPACPUSetFrequencyNAS
** =========================================================================*/
/**
  Set CPU domain configuration for the next active set.

  This functions sets the CPU domain configuration for the next active set.

  @param *pDrvCtxt    [in] -- Driver context.
  @param *pNewConfig  [in] -- New configuration to use.

  @return
  DAL_SUCCESS -- The domain was successfully configured.
  DAL_ERROR -- The domain configuration failed.

  @dependencies
  None.
*/

static void Clock_NPACPUSetFrequencyNAS
(
  ClockDrvCtxt        *pDrvCtxt,
  uint32         nFreqHz
)
{
  ClockImageCtxtType    *pImageCtxt = pDrvCtxt->pImageCtxt;
  ClockDomainNodeType   *pDomain = pImageCtxt->CPUCtxt.pClock->pDomain;
  ClockMuxConfigType    *pNewConfig;
  ClockVRegRequestType   VRegRequest;
  npa_client_handle      hRail;
  DALResult              eResult;

  ULOG_RT_PRINTF_2(pDrvCtxt->hClockLog,
    "CPU NAS[1] switch start - Frequency[%lu -> %lu] Hz",
    pDomain->pActiveMuxConfig->nFreqHz, nFreqHz);

  /*
   * Find new frequency configuration.
   */
  eResult = Clock_FindClockConfig(pDomain, nFreqHz,
    CLOCK_FREQUENCY_HZ_AT_LEAST, &pNewConfig);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to find CPU config for %lu.", nFreqHz);
    return;
  }

  /*
   * Propagate NAS attribute.
   * This is only necessary if raising the voltage, we don't want to schedule
   * a lower voltage for NAS since the CPU gets reconfigured on wake up and
   * could be under voltage if we wake up at a lower voltage than what the
   * current CPU configuration requires.
   */
  if (pNewConfig->eVRegLevel > pDomain->pActiveMuxConfig->eVRegLevel)
  {
    hRail = pDomain->pRail->VRegConfigSuppressible.NPAHandle;
    npa_set_request_attribute(hRail, NPA_REQUEST_NEXT_AWAKE);

    VRegRequest.eVRegLevel = pNewConfig->eVRegLevel;
    VRegRequest.bSuppressible = TRUE;
    Clock_VoltageRequest(pDomain->pRail, &pDomain->VRegRequest, &VRegRequest);

    /*
     * If CPU is on a shared rail, the voltage request may short circuit and
     * therefore not clear the flag. Clear it manually to cover that case.
     * Note there is no public function to do this.
     */
    hRail->request_attr &= ~NPA_REQUEST_NEXT_AWAKE;
  }

  /*
   * Keep track of the scheduled NAS request.
   */
  pImageCtxt->CPUCtxt.pNASConfig = pNewConfig;

  /*
   * Increment the NAS request counter while entering sleep.
   * This is used to match the number of NAS event callbacks invoked.
   */
  pImageCtxt->CPUCtxt.nNASRequestCount++;

  /*
   * Post NAS custom event to NPA work loop.
   * NPA will invoke our callback to handle the NAS request on wake up.
   */
  npa_notify_next_awake(Clock_NPACPUNASEventCallback, (void *)pNewConfig);

} /* END Clock_NPACPUSetFrequencyNAS */


/* =========================================================================
**  Function : Clock_NPACPUNASEventCallback
** =========================================================================*/
/**
  Callback on wake-up for NAS requests submitted to the CPU.

  This function is called by the NPA framework upon wake-up for a NAS
  request that was submitted to the CPU prior to sleep.

  @param *pData [in] -- Context passed in npa_create_event_cb.

  @return
  None.

  @dependencies
  None.
*/

static void Clock_NPACPUNASEventCallback
(
  void *pData
)
{
  ClockDrvCtxt        *pDrvCtxt;
  ClockImageCtxtType  *pImageCtxt;
  ClockMuxConfigType  *pConfig;
  uint32               nPreviousFreqHz;
  ClockDomainNodeType *pDomain;

  /*-----------------------------------------------------------------------*/
  /* Validate arguments.                                                   */
  /*-----------------------------------------------------------------------*/

  pConfig = (ClockMuxConfigType *)pData;
  if (pConfig == NULL)
  {
    DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Missing required arguments to NPA NAS event callback on wake up.");
    return;
  }

  pDrvCtxt = Clock_GetDrvCtxtNoLock();
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pDomain = pImageCtxt->CPUCtxt.pClock->pDomain;

  /*-----------------------------------------------------------------------*/
  /* Log the NAS Callback                                                  */
  /*-----------------------------------------------------------------------*/

  ULOG_RT_PRINTF_1(pDrvCtxt->hClockLog,
    "CPU NPA NAS event callback for frequency[%lu]", pConfig->nFreqHz);

  /*-----------------------------------------------------------------------*/
  /* NPA will potentially invoke this event trigger callback once for each */
  /* NAS request that was made.  We only take action on the first event    */
  /* notification because that was the final NAS state of the resource.    */
  /*-----------------------------------------------------------------------*/

  if (pImageCtxt->CPUCtxt.nNASRequestCount == 0)
  {
    return;
  }

  pImageCtxt->CPUCtxt.nNASRequestCount = 0;

  /*-----------------------------------------------------------------------*/
  /* If there is no pending NAS request it means that an immediate request */
  /* was already satisfied before the NPA scheduler dispatched this        */
  /* callback and thus we must ignore it.                                  */
  /*-----------------------------------------------------------------------*/

  if (pImageCtxt->CPUCtxt.pNASConfig == NULL)
  {
    return;
}

  /*-----------------------------------------------------------------------*/
  /* Verify the final NAS config matches what was saved in context.        */
  /*-----------------------------------------------------------------------*/

  if (pConfig != pImageCtxt->CPUCtxt.pNASConfig)
  {
    DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unexpected CPU configuration mismatch in NAS event callback on wake up.");
    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Configure CPU to the final NAS request.                               */
  /*-----------------------------------------------------------------------*/

  nPreviousFreqHz = pDomain->pActiveMuxConfig->nFreqHz;
  Clock_NPACPUSetFrequency(pDrvCtxt, pConfig->nFreqHz);

  /*-----------------------------------------------------------------------*/
  /* Log the NAS Callback                                                  */
  /*-----------------------------------------------------------------------*/

  ULOG_RT_PRINTF_2(pDrvCtxt->hClockLog,
    "CPU NAS[1] switch end - Frequency[%lu -> %lu] Hz",
    nPreviousFreqHz, pDomain->pActiveMuxConfig->nFreqHz);

  /*-----------------------------------------------------------------------*/
  /* Clear the NAS config.                                                 */
  /*-----------------------------------------------------------------------*/

  pImageCtxt->CPUCtxt.pNASConfig = NULL;

} /* END of Clock_NPACPUNASEventCallback */


/* =========================================================================
**  Function : Clock_NPACPUNodeDriverFuncTest
** =========================================================================*/
/**
  NPA node driver function for the CPU node test resource.

  This function handles minimum frequency requests for the CPU NPA node
  test resource. Requests to this resource will override any requests
  on the primary resource. The primary resource's request state will be
  restored when the request on the test resource goes to 0.

  @param *pResource [in] -- Pointer to resource whose state changed.
  @param hClient    [in] -- Client handle that triggered the change.
  @param nState     [in] -- New request state.

  @return
  npa_resource_state -- The new active state of the resource.

  @dependencies
  None.
*/

static npa_resource_state Clock_NPACPUNodeDriverFuncTest
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  ClockDrvCtxt       *pDrvCtxt = pResource->node->data;
  ClockImageCtxtType *pImageCtxt = pDrvCtxt->pImageCtxt;

  if (hClient->type == NPA_CLIENT_INITIALIZE)
  {
    return pResource->active_state;
  }

  /*
   * Start by clearing the DISABLED_BY_CPU_TEST flag so the primary
   * resource driver function doesn't just return. It's safe to clear
   * at this point since all resources share a common node lock.
   */
  pImageCtxt->CPUCtxt.nDisableDCS &= ~CLOCK_FLAG_DISABLED_BY_CPU_TEST;

  if (nState)
  {
    /*
     * When there is an active request on the test resource, call the primary
     * resource driver function to set the frequency, then set the
     * DISABLED_BY_CPU_TEST flag to disallow any other requests not from
     * the test resource.
     */
    nState = Clock_AssignNPAState(NPA_RESOURCE_CLK_CPU, nState);
    pImageCtxt->CPUCtxt.nDisableDCS |= CLOCK_FLAG_DISABLED_BY_CPU_TEST;
  }
  else
  {
    /*
     * When the request on the test resource is removed, restore the primary
     * resource to its current request state.
     */
    Clock_RestoreNPAState(NPA_RESOURCE_CLK_CPU);
  }

  return nState;

} /* END of Clock_NPACPUNodeDriverFuncTest */

