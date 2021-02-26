/*
===========================================================================
*/
/**
  @file VCSNPA.c

  NPA-related functions for the VCS driver.
*/
/*
  ====================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/vcs/src/VCSNPA.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "comdef.h"
#include "DDIPlatformInfo.h"
#include "VCSDriver.h"
#include "pmapp_npa.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "npa.h"
#include "npa_resource.h"
#include "npa_scheduler.h"
#include "VCSSWEVT.h"


/*=========================================================================
      Macros
==========================================================================*/

/*
 * Macros for defining NPA fields.
 */
#define VCS_NPA_UNITS_RAIL              "VLVL Corner"
#define VCS_NPA_NAS_TRIGGER_EVENT_NAME  "VCS NAS Event"

/*
 * Helper macros to access the various aggregations.
 * We re-use the SUPPRESSIBLE2 aggregation for the ACTIVE_FLOOR client type.
 * This will suffice as the SUPPRESSIBLE2 type isn't used for VCS resources.
 */
 #define VCS_NPA_INDEX_ACTIVE_FLOOR NPA_SUPPRESSIBLE2_INDEX

 #define VCS_NPA_REQUEST_ACTIVE_FLOOR(r) \
   MIN((r)->internal_state[VCS_NPA_INDEX_ACTIVE_FLOOR], (r)->active_max)

/*=========================================================================
      Type Definitions
==========================================================================*/

typedef boolean (*VCS_NPAStateComparator)(rail_voltage_level eCornerFrom, rail_voltage_level eCornerTo);


/*=========================================================================
      Prototypes
==========================================================================*/

static npa_resource_state VCS_NPAMaxUpdate(npa_resource *resource, npa_client_handle client);


/*=========================================================================
      Data
==========================================================================*/

static const npa_resource_plugin VCSNPAMaxPlugin =
{
  .update_fcn = VCS_NPAMaxUpdate,
  .supported_clients =
    NPA_CLIENT_REQUIRED |
    NPA_CLIENT_SUPPRESSIBLE |
    VCS_NPA_CLIENT_ACTIVE_FLOOR,
};

static const npa_client_type eSupportedTypes[] =
{
  NPA_CLIENT_REQUIRED,
  NPA_CLIENT_SUPPRESSIBLE
};

/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : VCS_NPAMapToInternalIndex
** =========================================================================*/
/**
  This function returns an index value to use when looking up client type
  overall aggregation within the internal data array.

  @param eType [in] -- Client type.

  @return
  Client type index.

  @dependencies
  None.
*/

static uint32 VCS_NPAMapToInternalIndex
(
  npa_client_type eType
)
{
  switch (eType)
  {
    case NPA_CLIENT_REQUIRED:         return NPA_REQUIRED_INDEX;
    case NPA_CLIENT_SUPPRESSIBLE:     return NPA_SUPPRESSIBLE_INDEX;
    case VCS_NPA_CLIENT_ACTIVE_FLOOR: return VCS_NPA_INDEX_ACTIVE_FLOOR;

    default: break;
  }

  DALSYS_LogEvent(
    0,
    DALSYS_LOGEVENT_FATAL_ERROR,
    "Request from unsupported client type[%lu].",
    eType);

  return 0;

} /* END of VCS_NPAMapToInternalIndex */


/* =========================================================================
**  Function : VCS_NPAMaxUpdateInternal
** =========================================================================*/
/**
  This function aggregates the client request along with the set of requests
  from all clients of the same type for the same resource and determines the
  maximum value.

  An active floor is applied if any client requested such.

  @param pResource [in] -- Resource being voted on.
  @param hClient [in] -- Client who issued the request.

  @return
  Overall resource aggregation.

  @dependencies
  None.
*/

static npa_resource_state VCS_NPAMaxUpdateInternal
(
  npa_resource      *pResource,
  npa_client_handle  hClient
)
{
  npa_resource_state  nRequest, nActiveMax, nPendingMax;
  npa_resource_state  nActiveFloor;
  uint32              i, nIndex;
  npa_client         *c;

  /*-----------------------------------------------------------------------*/
  /* If client requested NPA_MAX_STATE then map to resource max.           */
  /*-----------------------------------------------------------------------*/

  if (NPA_PENDING_REQUEST(hClient).state == NPA_MAX_STATE)
  {
    NPA_PENDING_REQUEST(hClient).state = pResource->definition->max;
  }

  /*-----------------------------------------------------------------------*/
  /* Get type aggregation.                                                 */
  /*-----------------------------------------------------------------------*/

  nIndex = VCS_NPAMapToInternalIndex(hClient->type);
  nRequest = NPA_PENDING_REQUEST(hClient).state;
  nPendingMax = nActiveMax = pResource->internal_state[nIndex];
  nActiveFloor = VCS_NPA_REQUEST_ACTIVE_FLOOR(pResource);

  /*-----------------------------------------------------------------------*/
  /* Client request is new max value.                                      */
  /*-----------------------------------------------------------------------*/

  if (nRequest > nPendingMax)
  {
    nPendingMax = nRequest;
  }

  /*-----------------------------------------------------------------------*/
  /* Reaggregate requests from clients with same type as this client if:   */
  /* 1) This client's prior request was the max overall and is now         */
  /*    adjusting the vote downward.                                       */
  /* 2) The overall aggregation for this client type was adjusted higher   */
  /*    due to the presence of an ACTIVE_FLOOR request.  Reaggregate with  */
  /*    this new client request in case the overall aggregation is OFF in  */
  /*    which case the ACTIVE_FLOOR request would be ignored in the final  */
  /*    state of this client type.                                         */
  /*-----------------------------------------------------------------------*/

  else if (nActiveMax == NPA_ACTIVE_REQUEST(hClient).state ||
           nActiveMax == nActiveFloor)
  {
    npa_client_handle c = pResource->clients;
    nPendingMax = NPA_PENDING_REQUEST(hClient).state;

    /*
     * Iterate over list of clients with same type for new max.
     */
    while (c)
    {
      if (hClient != c && hClient->type == c->type )
      {
        nRequest = NPA_ACTIVE_REQUEST(c).state;
        if (nRequest > nPendingMax )
        {
          nPendingMax = nRequest;
        }
      }
      c = c->next;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Apply BSP min floor to active floor aggregation.                      */
  /*-----------------------------------------------------------------------*/

  if (hClient->type == VCS_NPA_CLIENT_ACTIVE_FLOOR)
  {
    nPendingMax = MAX(nPendingMax, VCS_RESOURCE_MIN(pResource));
  }

  /*-----------------------------------------------------------------------*/
  /* Apply active floor to other client type aggregations.                 */
  /*-----------------------------------------------------------------------*/

  else if (nPendingMax)
  {
    nPendingMax = MAX(nPendingMax, VCS_NPA_REQUEST_ACTIVE_FLOOR(pResource));
  }

  pResource->internal_state[nIndex] = nPendingMax;

  /*-----------------------------------------------------------------------*/
  /* Any change in active floor state -> re-aggregate all requests.        */
  /*-----------------------------------------------------------------------*/

  if (nActiveFloor != VCS_NPA_REQUEST_ACTIVE_FLOOR(pResource))
  {
    for (i = 0; i < ARR_SIZE(eSupportedTypes); i++)
    {
      /*
       * Find max request for this client type.
       */
      for (nPendingMax = 0, c = pResource->clients; c != NULL; c = c->next)
      {
        if (c->type == eSupportedTypes[i] &&
            NPA_ACTIVE_REQUEST(c).state > nPendingMax)
        {
          nPendingMax = NPA_ACTIVE_REQUEST(c).state;
        }
      }

      /*
       * Apply active/min floor.
       */
      if (nPendingMax)
      {
        nPendingMax = MAX(nPendingMax, VCS_NPA_REQUEST_ACTIVE_FLOOR(pResource));
      }

      /*
       * Update internal state.
       */
      nIndex = VCS_NPAMapToInternalIndex(eSupportedTypes[i]);
      pResource->internal_state[nIndex] = nPendingMax;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Find max of all client types.                                         */
  /*-----------------------------------------------------------------------*/

  nPendingMax = 0;
  for (i = 0; i < ARR_SIZE(eSupportedTypes); i++)
  {
    nIndex = VCS_NPAMapToInternalIndex(eSupportedTypes[i]);
    nPendingMax = MAX(nPendingMax, pResource->internal_state[nIndex]);
  }

  return nPendingMax;

} /* END of VCS_NPAMaxUpdateInternal */


/* =========================================================================
**  Function : VCS_NPAMaxUpdate
** =========================================================================*/
/**
  This function performs max aggregation of all requests for this resource.

  All requests are mapped to supported values before applying the internal
  aggregation.

  @param pResource [in] -- Resource being voted on.
  @param hClient [in] -- Client who issued the request.

  @return
  Overall resource aggregation.

  @dependencies
  None.
*/

static npa_resource_state VCS_NPAMaxUpdate
(
  npa_resource      *pResource,
  npa_client_handle  hClient
)
{
  DALResult           eResult;
  VCSRailNodeType    *pRail;
  rail_voltage_level  eCorner;

  pRail = VCS_CLIENT_TO_RAIL(hClient);

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "No rail definition for resource[%s].",
      pResource->definition->name);

    return pResource->active_state;
  }

  /*-----------------------------------------------------------------------*/
  /* Get pending client request.                                           */
  /*-----------------------------------------------------------------------*/

  eCorner = NPA_PENDING_REQUEST(hClient).state;

  /*-----------------------------------------------------------------------*/
  /* Map aggregation to a supported corner for this rail.                  */
  /*-----------------------------------------------------------------------*/

  if (eCorner != RAIL_VOLTAGE_LEVEL_OFF &&
      pRail->pControl &&
      pRail->pControl->fpMapCorner)
  {
    /*
     * Invoke mapping interface for this rail type.
     */
    eResult = pRail->pControl->fpMapCorner(pRail, &eCorner);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "Failed to map request for resource[%s] to a supported corner.",
        pResource->definition->name);

      return pResource->active_state;
    }

    /*
     * Update client request to the mapped state.
     */
    NPA_PENDING_REQUEST(hClient).state = (npa_resource_state)eCorner;
  }

  /*-----------------------------------------------------------------------*/
  /* Aggregate this request using NPA max update.                          */
  /*-----------------------------------------------------------------------*/

  eCorner = (rail_voltage_level) VCS_NPAMaxUpdateInternal(pResource, hClient);

  return (npa_resource_state)eCorner;

} /* END of VCS_NPAMaxUpdate */


/* =========================================================================
**  Function : VCS_NPARequestDependency
** =========================================================================*/
/**
  This function issues to request to satisfy the dependency.
  The request attributes are propagated from the client request.
  Any resource specific data is preserved across the request.

  @param hClient [in] -- Client who issued the request.
  @param hClientDependency [in] -- Client used to issue dependency request.
  @param eCorner [in] -- Corner state to be applied towards the dependency.

  @return
  None.

  @dependencies
  None.
*/
static void VCS_NPARequestDependency
(
  npa_client_handle  hClient,
  npa_client_handle  hClientDependency,
  rail_voltage_level eCorner
)
{
  /*-----------------------------------------------------------------------*/
  /* Propagate all request attributes to dependency client.                */
  /*-----------------------------------------------------------------------*/

  npa_pass_request_attributes(hClient, hClientDependency);

  /*-----------------------------------------------------------------------*/
  /* Copy resource data for cases where rails need to share data.          */
  /*-----------------------------------------------------------------------*/

  hClientDependency->resource_data = hClient->resource_data;

  /*-----------------------------------------------------------------------*/
  /* Issue vote on dependency.                                             */
  /*-----------------------------------------------------------------------*/

  npa_issue_scalar_request(hClientDependency, eCorner);

  /*-----------------------------------------------------------------------*/
  /* Clear shared resource data after dependency satisfied.                */
  /*-----------------------------------------------------------------------*/

  hClientDependency->resource_data = NULL;

} /* END of VCS_NPARequestDependency */


/* =========================================================================
**  Function : VCS_NPASatisfyDependency
** =========================================================================*/
/**
  This function satisfies the dependency according to the comparator type.
  The client handle used to satisfy the dependency is identified from the
  requesting client.

  @param hClient [in] -- Client who issued the request.
  @param pComparator [in] -- Function pointer to comparator type.

  @return
  None.

  @dependencies
  None.
*/
static void VCS_NPASatisfyDependency
(
  npa_client_handle      hClient,
  VCS_NPAStateComparator pComparator
)
{
  uint32               i, j, nIndex;
  rail_voltage_level   eCorner;
  npa_client_handle    hDependency;
  npa_node_dependency *pDependency;
  npa_node_definition *pNode;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (hClient == NULL || pComparator == NULL)
  {
    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Nothing to do if no dependencies for this resource.                   */
  /*-----------------------------------------------------------------------*/

  pNode = hClient->resource->node;
  if (pNode->dependency_count == 0)
  {
    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Update REQUIRED and SUPPRESSIBLE dependencies with new active floor.  */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < ARR_SIZE(eSupportedTypes); i++)
  {
    /*
     * Find client handle for this supported dependency type.
     */
    hDependency = NULL;
    for (j = 0; j < pNode->dependency_count; j++)
    {
      pDependency = &pNode->dependencies[j];
      if (pDependency != NULL &&
          pDependency->handle != NULL &&
          pDependency->handle->type == eSupportedTypes[i])
      {
        hDependency = pDependency->handle;
      }
    }

    /*
     * Dependency must exist!
     */
    if (hDependency == NULL)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "Supported dependency type missing from rail[%s].",
        VCS_CLIENT_TO_RAIL(hClient));

      return;
    }

    /*
     * Compare new aggregated state to active request for this client type.
     */
    if (hClient->type == hDependency->type ||
        hClient->type == VCS_NPA_CLIENT_ACTIVE_FLOOR)
    {
      nIndex = VCS_NPAMapToInternalIndex(hDependency->type);
      eCorner = hClient->resource->internal_state[nIndex];
      if (pComparator(NPA_ACTIVE_REQUEST(hDependency).state, eCorner))
      {
        VCS_NPARequestDependency(hClient, hDependency, eCorner);
      }
    }
  }

} /* END of VCS_NPASatisfyDependency */


/* =========================================================================
**  Function : VCS_NPAStateIncreasingComparator
** =========================================================================*/
/**
  This function compares two corners and determines whether the new corner
  is greater than the previous.

  @param eCornerFrom [in] -- Current corner.
  @param eCornerTo [in] -- Destination corner.

  @return
  TRUE if new corner is greater than the previous.
  FALSE if new corner is not greater than the previous.

  @dependencies
  None.
*/
static boolean VCS_NPAStateIncreasingComparator
(
  rail_voltage_level eCornerFrom,
  rail_voltage_level eCornerTo
)
{
  return eCornerTo > eCornerFrom;

} /* END of VCS_NPAStateIncreasingComparator */


/* =========================================================================
**  Function : VCS_NPAStateDecreasingComparator
** =========================================================================*/
/**
  This function compares two corners and determines whether the new corner
  is less than the previous.

  @param eCornerFrom [in] -- Current corner.
  @param eCornerTo [in] -- Destination corner.

  @return
  TRUE if new corner is less than the previous.
  FALSE if new corner is not less than the previous.

  @dependencies
  None.
*/
static boolean VCS_NPAStateDecreasingComparator
(
  rail_voltage_level eCornerFrom,
  rail_voltage_level eCornerTo
)
{
  return eCornerTo < eCornerFrom;

} /* END of VCS_NPAStateDecreasingComparator */


/* =========================================================================
**  Function : VCS_NPAStateChangingComparator
** =========================================================================*/
/**
  This function compares two corners and determines whether the new corner
  is different than the previous.

  @param eCornerFrom [in] -- Current corner.
  @param eCornerTo [in] -- Destination corner.

  @return
  TRUE if new corner is different than the previous.
  FALSE if new corner is not different than the previous.

  @dependencies
  None.
*/
static boolean VCS_NPAStateChangingComparator
(
  rail_voltage_level eCornerFrom,
  rail_voltage_level eCornerTo
)
{
  return eCornerTo != eCornerFrom;

} /* END of VCS_NPAStateChangingComparator */

/* =========================================================================
**  Function : VCS_NPASatisfyIncreasingDependency
** =========================================================================*/
/*
  See VCSDriver.h
*/

void VCS_NPASatisfyIncreasingDependency
(
  npa_client_handle hClient
)
{
  VCS_NPASatisfyDependency(hClient, VCS_NPAStateIncreasingComparator);

} /* END of VCS_NPASatisfyIncreasingDependency */


/* =========================================================================
**  Function : VCS_NPASatisfyDecreasingDependency
** =========================================================================*/
/*
  See VCSDriver.h
*/

void VCS_NPASatisfyDecreasingDependency
(
  npa_client_handle hClient
)
{
  VCS_NPASatisfyDependency(hClient, VCS_NPAStateDecreasingComparator);

} /* END of VCS_NPASatisfyDecreasingDependency */


/* =========================================================================
**  Function : VCS_NPASatisfyChangingDependency
** =========================================================================*/
/*
  See VCSDriver.h
*/

void VCS_NPASatisfyChangingDependency
(
  npa_client_handle hClient
)
{
  VCS_NPASatisfyDependency(hClient, VCS_NPAStateChangingComparator);

} /* END of VCS_NPASatisfyChangingDependency */


/* =========================================================================
**  Function : VCS_NPARailNASEventCallback
** =========================================================================*/
/**
  Callback on wake-up for NAS requests submitted to the rail.

  This function is called by the NPA framework upon wake-up for a NAS
  request that was submitted to the rail prior to sleep.

  @param *pData [in] -- Event data.

  @return
  None.

  @dependencies
  None.
*/

static void VCS_NPARailNASEventCallback
(
  void *pData
)
{
  DALResult                eResult;
  VCSRailNodeType         *pRail;
  VCSNPARailEventDataType  RailEventData;
  npa_client_handle        hClient;

  /*-----------------------------------------------------------------------*/
  /* Validate arguments.                                                   */
  /*-----------------------------------------------------------------------*/

  hClient = (npa_client_handle)pData;
  if (hClient == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "Missing required data argument to NPA rail NAS event callback on wake up.");
  }

  /*-----------------------------------------------------------------------*/
  /* Get the rail node from the resource.                                  */
  /*-----------------------------------------------------------------------*/

  pRail = VCS_CLIENT_TO_RAIL(hClient);

  /*-----------------------------------------------------------------------*/
  /* ENTER CRITICAL SECTION.                                               */
  /*-----------------------------------------------------------------------*/

  npa_resource_lock(VCS_RAIL_HANDLE(pRail));

  /*-----------------------------------------------------------------------*/
  /* Log the NAS Callback                                                  */
  /*-----------------------------------------------------------------------*/

  ULOG_RT_PRINTF_5(
    VCS_GetDrvCtxt()->hVCSLog,
    "Rail[%s] NAS event callback for Client[0x%x] Vote[%lu] Stored NAS Corner[%lu], NAS request count[%lu]",
    VCS_RAIL_NAME(pRail),
    hClient,
    NPA_ACTIVE_REQUEST(hClient).state,
    pRail->eCornerNAS,
    pRail->nNASRequestCount);

  /*-----------------------------------------------------------------------*/
  /* NPA will potentially invoke this event trigger callback once for each */
  /* NAS request that was made.  We only take action on the first event    */
  /* notification because that was the final NAS state of the resource.    */
  /*-----------------------------------------------------------------------*/

  if (pRail->nNASRequestCount == 0)
  {
    /*
     * EXIT CRITICAL SECTION.
     */
    npa_resource_unlock(VCS_RAIL_HANDLE(pRail));

    return;
  }

  pRail->nNASRequestCount = 0;

  /*-----------------------------------------------------------------------*/
  /* If there is no pending NAS request it means that an immediate request */
  /* was already satisfied before the NPA scheduler dispatched this        */
  /* callback and thus we must ignore it.                                  */
  /*-----------------------------------------------------------------------*/

  if (pRail->eCornerNAS == RAIL_VOLTAGE_LEVEL_OFF)
  {
    /*
     * EXIT CRITICAL SECTION.
     */
    npa_resource_unlock(VCS_RAIL_HANDLE(pRail));

    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Fill out event data for rail state change.                            */
  /*-----------------------------------------------------------------------*/

  RailEventData.PreChange.eCorner = pRail->eCorner;
  RailEventData.PostChange.eCorner = pRail->eCornerNAS;

  /*-----------------------------------------------------------------------*/
  /* Set scheduled corner.                                                 */
  /*-----------------------------------------------------------------------*/

  if (pRail->pControl &&
      pRail->pControl->fpSetCorner)
  {
    eResult = pRail->pControl->fpSetCorner(hClient, pRail->eCornerNAS);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Unable to set rail[%s] to corner[%lu]",
        VCS_RAIL_NAME(pRail),
        pRail->eCornerNAS);

      /*
       * EXIT CRITICAL SECTION.
       */
      npa_resource_unlock(VCS_RAIL_HANDLE(pRail));

      return;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Notify NPA clients of post-switch event.                              */
  /*-----------------------------------------------------------------------*/

  npa_dispatch_custom_events(
    VCS_RAIL_HANDLE(pRail),
    (npa_event_type)VCS_NPA_RAIL_EVENT_POST_CHANGE,
    &RailEventData);

  /*-----------------------------------------------------------------------*/
  /* Update rail context.                                                  */
  /*-----------------------------------------------------------------------*/

  pRail->eCorner = pRail->eCornerNAS;

  /*-----------------------------------------------------------------------*/
  /* Clear NAS corner state.                                               */
  /*-----------------------------------------------------------------------*/

  pRail->eCornerNAS = RAIL_VOLTAGE_LEVEL_OFF;

  /*-----------------------------------------------------------------------*/
  /* Log the state update.                                                 */
  /*-----------------------------------------------------------------------*/

  ULOG_RT_PRINTF_2(
    VCS_GetDrvCtxt()->hVCSLog,
    "Rail[%s] change end: Post-State[%lu]",
    VCS_RAIL_NAME(pRail),
    pRail->eCorner);

  /*-----------------------------------------------------------------------*/
  /* EXIT CRITICAL SECTION.                                                */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(VCS_RAIL_HANDLE(pRail));

} /* END of VCS_NPARailNASEventCallback */


/* =========================================================================
**  Function : VCS_NPARailResourceDriverFunc
** =========================================================================*/
/**
  Handle request state changes on a rail's NPA resource.

  This is a generic NPA driver function applicable to all rail resources.
  This function does some common record keeping, and makes use of function
  pointers to invoke rail-specific functions to actually set the voltage.

  @param *pResource [in] -- Pointer to rail resource.
  @param  hClient [in] -- Client handle.
  @param  nState [in] -- New request state.

  @return
  New resource state.

  @dependencies
  None.
*/

static npa_resource_state VCS_NPARailResourceDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  DALResult                eResult;
  VCSRailNodeType         *pRail;
  rail_voltage_level       eCorner;
  VCSNPARailEventDataType  RailEventData;
  uint32                   nIndex;

  eCorner = (rail_voltage_level)nState;
  pRail = VCS_RESOURCE_TO_RAIL(pResource);

  /*-----------------------------------------------------------------------*/
  /* Initialize the corner configuration for this rail.                    */
  /*-----------------------------------------------------------------------*/

  if (hClient->type == NPA_CLIENT_INITIALIZE)
  {
    /*
     * Init corner state.
     */
    pRail->eCorner = eCorner;

    /*
     * Init active floor state.
     */
    nIndex = VCS_NPAMapToInternalIndex(VCS_NPA_CLIENT_ACTIVE_FLOOR);
    pResource->internal_state[nIndex] = VCS_RESOURCE_MIN(pResource);

    return pRail->eCorner;
  }

  /*-----------------------------------------------------------------------*/
  /* Log the corner request.                                               */
  /*-----------------------------------------------------------------------*/

  ULOG_RT_PRINTF_7(
    VCS_GetDrvCtxt()->hVCSLog,
    "Rail[%s] change start: Vote[%lu] Aggregation[%lu] Required[%lu] Suppressible[%lu] NAS[%lu] Pre-State[%lu]",
    VCS_RAIL_NAME(pRail),
    NPA_PENDING_REQUEST(hClient).state,
    eCorner,
    NPA_RESOURCE_REQUIRED_REQUEST(pResource),
    NPA_RESOURCE_SUPPRESSIBLE_REQUEST(pResource),
    npa_request_has_attribute(hClient, NPA_REQUEST_NEXT_AWAKE),
    pRail->eCorner);

  /*-----------------------------------------------------------------------*/
  /* This is an immediate request.                                         */
  /*                                                                       */
  /* The NPA framework doesn't support immediate requests that occur after */
  /* NAS requests but before going through a sleep transition.  Raise an   */
  /* error here if that case is detected.                                  */
  /*-----------------------------------------------------------------------*/

  if (pRail->eCornerNAS != RAIL_VOLTAGE_LEVEL_OFF &&
      !npa_request_has_attribute(hClient, NPA_REQUEST_NEXT_AWAKE) &&
      npa_is_next_wakeup_after_nas() != TRUE)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Rail[%s] Invalid CAS request for corner[%lu] when pending NAS request for corner[%lu].",
      VCS_RAIL_NAME(pRail),
      NPA_PENDING_REQUEST(hClient).state,
      pRail->eCornerNAS);

    return (npa_resource_state)pRail->eCorner;
  }

  /*-----------------------------------------------------------------------*/
  /* Notify NPA clients of pre-switch event.                               */
  /*-----------------------------------------------------------------------*/

  RailEventData.bIsNAS =
    npa_request_has_attribute(hClient, NPA_REQUEST_NEXT_AWAKE);

  RailEventData.PreChange.eCorner = pRail->eCorner;
  RailEventData.PreChange.nVoltageUV = 0;

  RailEventData.PostChange.eCorner = eCorner;
  RailEventData.PostChange.nVoltageUV = 0;

  npa_dispatch_custom_events(
    pResource,
    (npa_event_type)VCS_NPA_RAIL_EVENT_PRE_CHANGE,
    &RailEventData);

  /*-----------------------------------------------------------------------*/
  /* Set requested corner.                                                 */
  /*-----------------------------------------------------------------------*/

  if (pRail->pControl &&
      pRail->pControl->fpSetCorner)
  {
    eResult = pRail->pControl->fpSetCorner(hClient, eCorner);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Unable to set rail[%s] to corner[%lu]",
        VCS_RAIL_NAME(pRail),
        eCorner);

      return (npa_resource_state)pRail->eCorner;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Schedule completion of update complete events on the next wake up.    */
  /*                                                                       */
  /* Note: Rail types that must handle NAS requests locally on wakeup are  */
  /*       required to update eCornerNAS in the SetCorner function.        */
  /*-----------------------------------------------------------------------*/

  if (npa_request_has_attribute(hClient, NPA_REQUEST_NEXT_AWAKE) &&
      pRail->eCornerNAS != RAIL_VOLTAGE_LEVEL_OFF)
  {
    /*
     * Increment the NAS request counter while entering sleep.
     * This is used to match the number of NAS event callbacks invoked.
     */
    pRail->nNASRequestCount++;

    /*
     * Register wake up event.
     */
    npa_notify_next_awake(VCS_NPARailNASEventCallback, (void *)hClient);

    /*
     * Return the current active state.
     */
    return (npa_resource_state)pRail->eCorner;
  }


  /*-----------------------------------------------------------------------*/
  /* Notify NPA clients of post-switch event.                              */
  /*-----------------------------------------------------------------------*/

  RailEventData.PostChange.eCorner = pRail->eCorner;
  npa_dispatch_custom_events(
    pResource,
    (npa_event_type)VCS_NPA_RAIL_EVENT_POST_CHANGE,
    &RailEventData);

  /*-----------------------------------------------------------------------*/
  /* Log the voltage corner switch.                                        */
  /*-----------------------------------------------------------------------*/

  ULOG_RT_PRINTF_2(
    VCS_GetDrvCtxt()->hVCSLog,
    "Rail[%s] change end: Post-State[%lu]",
    VCS_RAIL_NAME(pRail),
    pRail->eCorner);

  /*-----------------------------------------------------------------------*/
  /* Clear NAS corner for cases where there are pending NAS event          */
  /* callbacks that were pre-empted by some immediate request.             */
  /*                                                                       */
  /* The immediate request gets applied and all pending NAS requests must  */
  /* be ignored.                                                           */
  /*                                                                       */
  /* This case was observed as part of handling "rude wake-up interrupts". */
  /*-----------------------------------------------------------------------*/

  pRail->eCornerNAS = RAIL_VOLTAGE_LEVEL_OFF;

  /*-----------------------------------------------------------------------*/
  /* Return the current rail corner.                                       */
  /*-----------------------------------------------------------------------*/

  return (npa_resource_state)pRail->eCorner;

} /* END VCS_NPARailResourceDriverFunc */


/* =========================================================================
**  Function : VCS_NPARailNodeDriverFunc
** =========================================================================*/
/**
  Handle request for a state change on a rail resource.

  This is a generic NPA driver function applicable to all rail resources.

  @param *pResource [in] -- Pointer to rail resource.
  @param  hClient [in] -- Client handle.
  @param  nState [in] -- New request state.

  @return
  New resource state.

  @dependencies
  None.
*/

static npa_resource_state VCS_NPARailNodeDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  VCSRailNodeType    *pRail;
  npa_resource_state  nActiveState;

  pRail = (VCSRailNodeType *)pResource->node->data;
  nActiveState = 0;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Resource[%s] contains NULL node data",
      pResource->definition->name);

    return (npa_resource_state)0;
  }

  if (pResource == VCS_RAIL_HANDLE(pRail))
  {
    nActiveState =
      VCS_NPARailResourceDriverFunc(
        pResource,
        hClient,
        nState);
  }
  else
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Unable to process NPA request for resource[%s]",
      pResource->definition->name);
  }

  return nActiveState;

} /* END VCS_NPARailNodeDriverFunc */


/* =========================================================================
**  Function : VCS_NPADefineRailNode
** =========================================================================*/
/*
  Defines the NPA structures for this rail.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pRail [in] -- Pointer to rail being initialized.

  @return
  DAL_SUCCESS -- Successfully initialized the NPA structures for this rail.
  DAL_ERROR_INVALID_PARAMETER -- Invalid pointer.

  @dependencies
  None.
*/

DALResult VCS_NPADefineRailNode
(
  VCSDrvCtxt      *pDrvCtxt,
  VCSRailNodeType *pRail
)
{
  npa_resource_state       anInitialState[VCS_NPA_RAIL_NUM_RESOURCES];
  npa_resource_definition *pResource;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Populate rail resource.                                               */
  /*-----------------------------------------------------------------------*/

  pResource = VCS_RAIL_RES_DEF_RAIL(pRail);
  pResource->units = VCS_NPA_UNITS_RAIL;
  pResource->max = VCS_RAIL_MAX(pRail);
  pResource->plugin = &VCSNPAMaxPlugin;
  pResource->data = (npa_user_data)pRail;

  anInitialState[VCS_NPA_RAIL_RES_IDX_RAIL] = VCS_RAIL_INIT(pRail);

  /*-----------------------------------------------------------------------*/
  /* Populate node information.                                            */
  /*-----------------------------------------------------------------------*/

  pRail->mNode.driver_fcn = VCS_NPARailNodeDriverFunc;
  pRail->mNode.attributes = NPA_NODE_DEFAULT;
  pRail->mNode.data = (npa_user_data)pRail;
  pRail->mNode.dependencies = pRail->aDependencies;
  pRail->mNode.resource_count = VCS_NPA_RAIL_NUM_RESOURCES;
  pRail->mNode.resources = pRail->aResources;

  /*-----------------------------------------------------------------------*/
  /* Define the NPA node.                                                  */
  /*-----------------------------------------------------------------------*/

  npa_define_node(&pRail->mNode, anInitialState, NULL);

  /*-----------------------------------------------------------------------*/
  /* Make resource available for any remote domains.                       */
  /*-----------------------------------------------------------------------*/

  if (pRail->eRemoteDomainId != 0)
  {
    npa_remote_publish_resource(pRail->eRemoteDomainId, VCS_RAIL_NAME(pRail));
  }

  return DAL_SUCCESS;

} /* END VCS_NPADefineRailNode */


/* =========================================================================
**  Function : VCS_InitNPA
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitNPA
(
  VCSDrvCtxt *pDrvCtxt
)
{
  uint32           i;
  DALResult        eResult;
  VCSBSPType      *pBSP;
  VCSRailNodeType *pRail;

  pBSP = pDrvCtxt->pBSP;

  /*-----------------------------------------------------------------------*/
  /* Iterate through all the rails to define rail resources.               */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pBSP->nNumRails; i++)
  {
    pRail = &pBSP->pRails[i];
    eResult = VCS_NPADefineRailNode(pDrvCtxt, pRail);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Failed to create NPA resource for rail[%s].",
        VCS_RAIL_NAME(pRail));

      return eResult;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Done.                                                                 */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCS_InitNPA */
