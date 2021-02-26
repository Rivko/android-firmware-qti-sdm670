/*
===========================================================================
*/
/**
  @file VCSRPMhRailImpl.c 
  
  RPMh impl type functions for the VCS driver.
*/
/*  
  ====================================================================

  Copyright (c) 2017 QUALCOMM Technologies Incorporated. All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/vcs/src/RailImpls/VCSRPMhRailImpl.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "VCSDriver.h"
#include "VCSRPMhRailImpl.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "npa.h"
#include "npa_resource.h"
#include "cmd_db.h"
#include "pwr_utils_lvl.h"
#include "rpmh_client.h"
#include "voltage_level.h"


/*=========================================================================
      Macro Definitions
==========================================================================*/

/*
 * Helper macro to initialize a command set.
 */
#define VCS_RPMH_RAIL_IMPL_INIT_CMD_SET(x) \
  do { \
    (x)[RPMH_SET_ACTIVE].set = RPMH_SET_ACTIVE; \
    (x)[RPMH_SET_ACTIVE].num_commands = 0; \
    (x)[RPMH_SET_SLEEP].set = RPMH_SET_SLEEP; \
    (x)[RPMH_SET_SLEEP].num_commands = 0; \
    (x)[RPMH_SET_WAKE].set = RPMH_SET_WAKE; \
    (x)[RPMH_SET_WAKE].num_commands = 0; \
  } while(0)


/*=========================================================================
      Prototypes
==========================================================================*/

static DALResult VCS_RPMhRailImplInitImpl(VCSRailNodeType *pRail);
static DALResult VCS_RPMhRailImplSetCorner(npa_client_handle hClient, rail_voltage_level eCorner);
static DALResult VCS_RPMhRailImplMapCorner(VCSRailNodeType *pRail, rail_voltage_level *eCorner);


/*=========================================================================
      Data
==========================================================================*/

VCSRailControlType VCSRPMhRailImplControl =
{
  .fpInit      = VCS_RPMhRailImplInitImpl,
  .fpSetCorner = VCS_RPMhRailImplSetCorner,
  .fpMapCorner = VCS_RPMhRailImplMapCorner,
};


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : VCS_RPMhRailImplFlushSet
** =========================================================================*/
/**
  Flush the specified command set to RPMh.

  @param *pSet [in] -- Pointer command set.
  @param hRPMh [in] -- RPMh client handle.

  @return
  DAL_SUCCESS if command set flushed successfully.
  DAL_ERROR_INVALID_PARAMETER if NULL arguments provided.

  @dependencies
  None.
*/

static DALResult VCS_RPMhRailImplFlushSet
(
  rpmh_command_set_t *pSet,
  rpmh_client_handle  hRPMh
)
{
  uint32 nCommandId;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (!pSet || !hRPMh)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Flush and clear each set.                                             */
  /*-----------------------------------------------------------------------*/

  if (pSet->num_commands)
  {
    nCommandId = rpmh_issue_command_set(hRPMh, pSet);

    /*
     * Barrier if command wants to wait for completion.
     */
    if (pSet->commands[0].completion)
    {
      rpmh_barrier_single(hRPMh, nCommandId);
    }

    /*
     * Invalidate commands.
     */
    pSet->num_commands = 0;
  }

  return DAL_SUCCESS;

} /* END of VCS_RPMhRailImplFlushSet */


/* =========================================================================
**  Function : VCS_RPMhRailImplQueueCommand
** =========================================================================*/
/**
  Flush the specified command set to RPMh.

  @param hClient [in] -- NPA client handle.
  @param *pSet [in] -- Pointer command set.
  @param nAddress [in] -- Destination address for data.
  @param nData [in] -- Data to queue.

  @return
  DAL_SUCCESS if command successfully queued.
  DAL_ERROR_INVALID_PARAMETER if NULL arguments provided.

  @dependencies
  None.
*/

static DALResult VCS_RPMhRailImplQueueCommand
(
  npa_client_handle   hClient,
  rpmh_command_set_t *pSet,
  uint32              nAddress,
  int                 nData
)
{
  rpmh_command_t *pCommand;
  boolean         bCompletion;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pSet == NULL ||
      nAddress == 0 ||
      pSet->num_commands == ARR_SIZE(pSet->commands))
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Determine if this command should wait for completion.                 */
  /*-----------------------------------------------------------------------*/

  if (hClient != NULL &&
      pSet->set == RPMH_SET_ACTIVE &&
      !npa_resource_has_attribute(hClient->resource, NPA_REQUEST_FIRE_AND_FORGET))
  {
    bCompletion = TRUE;
  }
  else
  {
    bCompletion = FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Queue command.                                                        */
  /*-----------------------------------------------------------------------*/

  pCommand = &pSet->commands[pSet->num_commands];
  pCommand->address = nAddress;
  pCommand->data = (uint32)nData;
  pCommand->completion = bCompletion;

  /*
   * Increment command count.
   */
  pSet->num_commands++;

  return DAL_SUCCESS;

} /* END of VCS_RPMhRailImplQueueCommand */


/* =========================================================================
**  Function : VCS_RPMhRailImplMapCorner
** =========================================================================*/
/**
  Map corner to an RPMh supported corner for this rail.

  This function will map the specified corner to an RPMh supported corner for
  this rail.

  @param *pRail   [in]     -- Pointer rail node.
  @param *eCorner [in/out] -- Request corner that gets mapped to supported corner.

  @return
  DAL_SUCCESS -- Sucessfully mapped corner to a supported corner.
  DAL_ERROR -- Failed to map corner to a supported corner.

  @dependencies
  None.
*/

static DALResult VCS_RPMhRailImplMapCorner(VCSRailNodeType *pRail, rail_voltage_level *eCorner)
{
  int                  nHLVL;
  int                  nCornerRequest, nCornerMapped;
  VCSRPMhRailImplType *pImpl;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || pRail->pImpl == NULL || eCorner == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pImpl = (VCSRPMhRailImplType *)pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Map request to a supported corner.                                    */
  /*-----------------------------------------------------------------------*/

  nCornerRequest = (int)*eCorner;
  nHLVL = pwr_utils_hlvl_named_resource(pImpl->szName, nCornerRequest, &nCornerMapped);
  if (nHLVL < 0)
  {
    return DAL_ERROR;
  }

  *eCorner = (rail_voltage_level)nCornerMapped;

  return DAL_SUCCESS;

} /* END of VCS_RPMhRailImplMapCorner */


/* =========================================================================
**  Function : VCS_RPMhRailImplSetCorner
** =========================================================================*/
/**
  Set RPMh rail to a requested corner.

  This function will set the rail to the requested corner.
  The dependency will be satisfied as specified by the bBatchCommand field
  of the rail impl data.

  In batched mode:
    This function will group commands for this rail with the set of commands
    to update the dependency rail as well so that RPMh can update them together
    atomically.

  In synchronous mode:
    This function will issue commands for this rail in a synchronous order with
    the commands issued for the dependency.

  @param hClient [in] -- Pointer to client making request.
  @param eCorner [in] -- Requested corner.

  @return
  DAL_SUCCESS -- Sucessfully configured rail to requested corner.
  DAL_ERROR -- Failed to configure rail to requested corner.

  @dependencies
  None.
*/

static DALResult VCS_RPMhRailImplSetCorner
(
  npa_client_handle  hClient,
  rail_voltage_level eCorner
)
{
  DALResult            eResult;
  VCSRailNodeType     *pRail;
  VCSRPMhRailImplType *pImpl;
  rpmh_command_set_t  *pCommandSets, *pSet;
  int                  nHLVL;
  uint32               i, nAddress;

  pRail = VCS_CLIENT_TO_RAIL(hClient);

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || pRail->pImpl == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  pImpl = (VCSRPMhRailImplType *)pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Share command sets from this rail with those in dependency set.       */
  /*-----------------------------------------------------------------------*/

  if (pImpl->bBatchCommand)
  {
    if (hClient->resource_data == NULL)
    {
      hClient->resource_data = (npa_user_data)pImpl->aSets;
    }

    pCommandSets = (rpmh_command_set_t *)hClient->resource_data;
  }

  /*-----------------------------------------------------------------------*/
  /* Otherwise use command sets from this rail.                            */
  /*-----------------------------------------------------------------------*/

  else
  {
    pCommandSets = (rpmh_command_set_t *)pImpl->aSets;
  }

  /*-----------------------------------------------------------------------*/
  /* Update sleep set if request is not for the next active set.           */
  /*-----------------------------------------------------------------------*/

  if (hClient->type == VCS_NPA_CLIENT_ACTIVE_FLOOR ||
      (hClient->type == NPA_CLIENT_REQUIRED &&
      !npa_request_has_attribute(hClient, NPA_REQUEST_NEXT_AWAKE)))
  {
    pSet = &pCommandSets[RPMH_SET_SLEEP];
    nAddress = pImpl->anAddress[VCS_NPA_RAIL_RES_IDX_RAIL],

    /*
     * Get aggregation for REQUIRED client type.
     */
    nHLVL =
      pwr_utils_hlvl_named_resource(
        pImpl->szName,
        NPA_RESOURCE_REQUIRED_REQUEST(hClient->resource),
        NULL);

    eResult = VCS_RPMhRailImplQueueCommand(hClient, pSet, nAddress, nHLVL);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update active or wake set if max aggregation has changed.             */
  /*-----------------------------------------------------------------------*/

  if (eCorner != pRail->eCorner)
  {
    if (npa_request_has_attribute(hClient, NPA_REQUEST_NEXT_AWAKE))
    {
      pSet = &pCommandSets[RPMH_SET_WAKE];
    }
    else
    {
      pSet = &pCommandSets[RPMH_SET_ACTIVE];
    }

    nAddress = pImpl->anAddress[VCS_NPA_RAIL_RES_IDX_RAIL],
    nHLVL = pwr_utils_hlvl_named_resource(pImpl->szName, eCorner, NULL);

    eResult = VCS_RPMhRailImplQueueCommand(hClient, pSet, nAddress, nHLVL);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Satisfy dependency for batched vs. non-batched mode.                  */
  /*   [Batched] Send dependency update in same command set being used to  */
  /*   update this rail. This applies whether the change is increasing or  */
  /*   decreasing the dependency from its current state.                   */
  /*                                                                       */
  /*   [Non-Batched] Satisfy increasing dependency first before applying   */
  /*   the change to this rail state.                                      */
  /*-----------------------------------------------------------------------*/

  if (pImpl->bBatchCommand)
  {
    VCS_NPASatisfyChangingDependency(hClient);
  }
  else
  {
    VCS_NPASatisfyIncreasingDependency(hClient);
  }

  /*-----------------------------------------------------------------------*/
  /* Flush all commands added to the set by this point.                    */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < RPMH_NUM_SETS; i++)
  {
    eResult = VCS_RPMhRailImplFlushSet(&pCommandSets[i], pImpl->hRPMh);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Satisfy dependency for batched vs. non-batched mode.                  */
  /*   [Non-Batched] Satisfy dereasing dependency second after applying    */
  /*   the change to this rail state.                                      */
  /*-----------------------------------------------------------------------*/

  if (!pImpl->bBatchCommand)
  {
    VCS_NPASatisfyDecreasingDependency(hClient);
  }

  /*-----------------------------------------------------------------------*/
  /* Clear data that might have been shared with rails in dependency set.  */
  /*-----------------------------------------------------------------------*/

  hClient->resource_data = (npa_user_data)NULL;

  /*-----------------------------------------------------------------------*/
  /* Update context.                                                       */
  /*-----------------------------------------------------------------------*/

  pRail->eCorner = eCorner;

  return DAL_SUCCESS;

} /* END of VCS_RPMhRailImplSetCorner */


/* =========================================================================
**  Function : VCS_RPMhRailImplResourceQuery
** =========================================================================*/
/**
  NPA rail resource query function.

  This function is called to get the following rail information:
    -- Number of corners.
    -- Voltage table for each corner.
    -- Current corner (id / table index).

  @param *pResource  [in]  -- Pointer to the resource in question
  @param  nID        [in]  -- ID of the query.
  @param *pResult    [out] -- Pointer to the data to be filled by this function.

  @return
  npa_query_status - NPA_QUERY_SUCCESS, if query supported.
                   - NPA_QUERY_UNSUPPORTED_QUERY_ID, if query not supported.

  @dependencies
  None.
*/

static npa_query_status VCS_RPMhRailImplResourceQuery
(
  npa_resource   *pResource,
  unsigned int    nID,
  npa_query_type *pResult
)
{
  VCSRailNodeType     *pRail;
  VCSRPMhRailImplType *pImpl;
  uint32               nLevel;

  /*-----------------------------------------------------------------------*/
  /* Validate parameters.                                                  */
  /*-----------------------------------------------------------------------*/

  if (pResource == NULL || pResult == NULL)
  {
    return NPA_QUERY_NULL_POINTER;
  }

  pRail = VCS_RESOURCE_TO_RAIL(pResource);
  if (pRail == NULL || pRail->pImpl == NULL)
  {
    return NPA_QUERY_NULL_POINTER;
  }

  pImpl = (VCSRPMhRailImplType *)pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Check for number of corners supported.                                */
  /*-----------------------------------------------------------------------*/

  if (nID == VCS_NPA_QUERY_VOLTAGE_NUM_ENTRIES)
  {
    pResult->data.value = pwr_utils_named_resource_lvls_count(pImpl->szName);
    pResult->type = NPA_QUERY_TYPE_VALUE;

    return NPA_QUERY_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Check for current corner in VLVL.                                     */
  /*-----------------------------------------------------------------------*/

  if(nID == VCS_NPA_QUERY_VOLTAGE_CURRENT_LEVEL_ID)
  {
    pResult->data.value = pRail->eCorner;
    pResult->type = NPA_QUERY_TYPE_VALUE;

    return NPA_QUERY_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Check for corner VLVL at specified performance level.                 */
  /*-----------------------------------------------------------------------*/

  if (nID >= VCS_NPA_QUERY_VOLTAGE_LEVEL_ID && nID < VCS_NPA_QUERY_END_MARKER)
  {
    nLevel = nID - VCS_NPA_QUERY_VOLTAGE_LEVEL_ID;
    if (nLevel < pwr_utils_named_resource_lvls_count(pImpl->szName))
    {
      pResult->data.value = pwr_utils_vlvl_named_resource(pImpl->szName, nLevel);
      pResult->type = NPA_QUERY_TYPE_VALUE;

      return NPA_QUERY_SUCCESS;
    }
  }

  return NPA_QUERY_UNSUPPORTED_QUERY_ID;

} /* END VCS_RPMhRailImplResourceQuery */


/* =========================================================================
**  Function : VCS_RPMhRailImplInitImpl
** =========================================================================*/
/**
  Initializes the RPMh data for this rail.

  @param *pRail [in] -- Pointer rail node.

  @return
  DAL_ERROR if a corner list not initialized, other DAL_SUCCESS.

  @dependencies
  None.
*/

static DALResult VCS_RPMhRailImplInitImpl
(
  VCSRailNodeType *pRail
)
{
  VCSRPMhRailImplType *pImpl;
  uint32               nAddress;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL ||
      pRail->pImpl == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pImpl = (VCSRPMhRailImplType *)pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Validate name defined.                                                */
  /*-----------------------------------------------------------------------*/

  if (pImpl->szName == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Create client handle to RPMh.                                         */
  /*-----------------------------------------------------------------------*/

  pImpl->hRPMh = rpmh_create_handle(pImpl->eDRVId, "VCS");
  if (pImpl->hRPMh == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Failed to create client handle for RSC DRV.");

    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Query CMD DB for resource address.                                    */
  /*-----------------------------------------------------------------------*/

  nAddress = cmd_db_query_addr(pImpl->szName);
  if (!nAddress)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Failed to get valid RSC address for rail[%s]",
      VCS_RAIL_NAME(pRail));

    return DAL_ERROR;
  }

  pImpl->anAddress[VCS_NPA_RAIL_RES_IDX_RAIL] = nAddress;

  /*-----------------------------------------------------------------------*/
  /* Install function pointer for rail resource query.                     */
  /*-----------------------------------------------------------------------*/

  VCS_RAIL_RES_DEF_RAIL(pRail)->query_fcn = VCS_RPMhRailImplResourceQuery;

  /*-----------------------------------------------------------------------*/
  /* Setup command sets.                                                   */
  /*-----------------------------------------------------------------------*/

  VCS_RPMH_RAIL_IMPL_INIT_CMD_SET(pImpl->aSets);

  /*-----------------------------------------------------------------------*/
  /* Initialize SLEEP set vote to lowest state.                            */
  /*-----------------------------------------------------------------------*/

  rpmh_issue_command(pImpl->hRPMh, RPMH_SET_SLEEP, FALSE, nAddress, 0);

  return DAL_SUCCESS;

} /* END of VCS_RPMhRailImplInitImpl */
