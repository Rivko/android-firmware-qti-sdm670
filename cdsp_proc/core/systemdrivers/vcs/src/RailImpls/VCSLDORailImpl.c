/*
===========================================================================
*/
/**
  @file VCSLDORailImpl.c
  
  LDO-related functions for the VCS driver.
*/
/*  
  ====================================================================

  Copyright (c) 2017 QUALCOMM Technologies Incorporated. All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/vcs/src/RailImpls/VCSLDORailImpl.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "VCSDriver.h"
#include "VCSLDORailImpl.h"
#include <DALSys.h>
#include "HALvcs.h"
#include "DALDeviceId.h"
#include "VCSSWEVT.h"
#include "HALhwio.h"
#include "npa_scheduler.h"
#include "cpr_image_api.h"
#include <stdlib.h>


/*=========================================================================
      Prototypes
==========================================================================*/

static DALResult VCS_LDORailImplInitImpl(VCSRailNodeType *);
static DALResult VCS_LDORailImplSetCorner(npa_client_handle, rail_voltage_level);
static DALResult VCS_LDORailImplMapCorner(VCSRailNodeType *pRail, rail_voltage_level *eCorner);


/*=========================================================================
      Data
==========================================================================*/

VCSRailControlType VCSLDORailImplControl =
{
  .fpInit      = VCS_LDORailImplInitImpl,
  .fpSetCorner = VCS_LDORailImplSetCorner,
  .fpMapCorner = VCS_LDORailImplMapCorner,
};


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : VCS_LDORailImplFindCornerItem
** =========================================================================*/
/**

  This function gets a corner item from the corner list.  A linear search
  is performed through the list.

  @param  *pRail  [in] -- Pointer to LDO rail.
  @param   eCorner[in] -- Corner to search for.
  @param  *pCorner[in] -- Corner item retrieved.

  @return
  None.


  @dependencies
  None.
*/

void VCS_LDORailImplFindCornerItem
(
  VCSRailNodeType    *pRail,
  rail_voltage_level  eCorner,
  rail_voltage_level *pCorner
)
{
  uint32              i;
  VCSLDORailImplType *pImpl;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || pRail->pImpl == NULL || pCorner == NULL)
  {
    return;
  }

  pImpl = (VCSLDORailImplType *)pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Initialize the argument.                                              */
  /*-----------------------------------------------------------------------*/

  *pCorner = RAIL_VOLTAGE_LEVEL_OFF;

  /*-----------------------------------------------------------------------*/
  /* Perform linear search.                                                */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pImpl->nNumCorners; i++)
  {
    if (pImpl->aCorners[i] >= eCorner)
    {
      *pCorner = pImpl->aCorners[i];
      break;
    }
  }

} /* END of VCS_LDORailImplFindCornerItem */


/* =========================================================================
**  Function : VCS_LDORailImplMapCorner
** =========================================================================*/
/**
  Map corner to an LDO supported corner for this rail.

  This function will map the specified corner to an LDO supported corner for
  this rail.

  @param *pRail   [in]     -- Pointer rail node.
  @param *eCorner [in/out] -- Request corner that gets mapped to supported corner.

  @return
  DAL_SUCCESS -- Sucessfully mapped corner to a supported corner.
  DAL_ERROR -- Failed to map corner to a supported corner.

  @dependencies
  None.
*/

static DALResult VCS_LDORailImplMapCorner(VCSRailNodeType *pRail, rail_voltage_level *eCorner)
{
  rail_voltage_level eCornerMapped;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || eCorner == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Check if the LDO can support this corner.                             */
  /*-----------------------------------------------------------------------*/

  eCornerMapped = RAIL_VOLTAGE_LEVEL_OFF;
  if (eCorner != RAIL_VOLTAGE_LEVEL_OFF)
  {
    VCS_LDORailImplFindCornerItem(pRail, *eCorner, &eCornerMapped);

    /*
     * The supported corner might be higher than the original request.
     * Make sure to use this elevated corner when satisfying dependencies.
     */
    if (eCornerMapped != RAIL_VOLTAGE_LEVEL_OFF)
    {
      *eCorner = eCornerMapped;
    }
  }

  return DAL_SUCCESS;

} /* END of VCS_LDORailImplMapCorner */


/* =========================================================================
**  Function : VCS_LDODetectBSPVersion
** =========================================================================*/
/**
  Detects which rail BSP configuration to use for the current HW.

  @param pLDO [in]     -- Pointer LDO node.
  @param eMode [in]    -- Mode identifier.
  @param pConfig [out] -- Pointer to LDO config pointer.

  @return
  DAL_ERROR if a valid configuration was not found, other DAL_SUCCESS.

  @dependencies
  None.
*/

static DALResult VCS_LDODetectBSPVersion
(
  VCSLDORailImplType  *pImpl,
  VCSLDOModeType       eMode,
  VCSLDOConfigType   **pConfig
)
{
  uint32            i;
  VCSLDOConfigType *pIter;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pImpl == NULL || pConfig == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize return parameter.                                          */
  /*-----------------------------------------------------------------------*/

  *pConfig = NULL;

  /*-----------------------------------------------------------------------*/
  /* Detect which BSP data to use for this HW version.                     */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pImpl->nNumConfigs; i++)
  {
    pIter = &pImpl->pConfigs[i];
    if (VCS_IsBSPSupported(&pIter->HWVersion) && pIter->eMode == eMode)
    {
      *pConfig = pIter;
      break;
    }
  }

  if (*pConfig == NULL)
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;

}  /*  END VCS_LDODetectBSPVersion */


/* =========================================================================
**  Function : VCS_LDORailImplSetVoltage
** =========================================================================*/
/**
  Get the recommended voltage for the eLDO from CPR and set it.

  @param *pCPU [in]   -- Handle to the LDO node.
  @param eCorner [in] -- Requested corner.

  @return
  DAL_SUCCESS -- Sucessfully set the voltage.
  DAL_ERROR -- Failed to set the voltage.

  @dependencies
  None.
*/

static DALResult VCS_LDORailImplSetVoltage
(
  VCSLDORailImplType *pImpl,
  uint32              nVoltageUV
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pImpl == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Update the LDO voltage.                                               */
  /*-----------------------------------------------------------------------*/

  HAL_vcs_SetLDOVoltage(&pImpl->HALdesc, nVoltageUV);

  /*-----------------------------------------------------------------------*/
  /* Update the context.                                                   */
  /*-----------------------------------------------------------------------*/

  pImpl->nVoltageUV = nVoltageUV;

  return DAL_SUCCESS;

} /* End VCS_LDORailImplSetVoltage */


/* =========================================================================
**  Function : VCS_LDORailImplSetCornerCPR
** =========================================================================*/
/**
  Set LDO to a requested corner.

  @param pRail[in] -- Pointer to rail.
  @param eCorner [in] -- Requested corner.

  @return
  DAL_SUCCESS -- Sucessfully configured LDO to requested corner.
  DAL_ERROR -- Failed to configure LDO to requested corner.

  @dependencies
  None.
*/

static DALResult VCS_LDORailImplSetCornerCPR
(
  VCSRailNodeType    *pRail,
  rail_voltage_level  eCorner
)
{
  VCSLDORailImplType *pImpl;
  uint32              nVoltageUV;
  DALResult           eResult;
  rail_voltage_level  eCornerLDO;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || pRail->pImpl == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pImpl = (VCSLDORailImplType *)pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Short-circuit with SUCCESS under following conditions.                */
  /*-----------------------------------------------------------------------*/

  if (pImpl->nDisable ||
      pImpl->pActiveConfig == NULL ||
      (pImpl->bIsEnabled && eCorner == pRail->eCorner) ||
      (!pImpl->bIsEnabled && eCorner == RAIL_VOLTAGE_LEVEL_OFF))
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Get LDO corner.                                                       */
  /*-----------------------------------------------------------------------*/

  eCornerLDO = RAIL_VOLTAGE_LEVEL_OFF;
  if (eCorner != RAIL_VOLTAGE_LEVEL_OFF)
  {
    VCS_LDORailImplFindCornerItem(pRail, eCorner, &eCornerLDO);
  }

  /*-----------------------------------------------------------------------*/
  /* Enable LDO if request is supported by active config.                  */
  /*-----------------------------------------------------------------------*/

  if (eCornerLDO != RAIL_VOLTAGE_LEVEL_OFF &&
      pImpl->pActiveConfig->eCornerMin <= eCornerLDO &&
      pImpl->pActiveConfig->eCornerMax >= eCornerLDO)
  {
    /*
     * Disable CPR sensor chain while updating.
     */
    if (pImpl->bIsEnabled)
    {
      eResult = CPR_Disable(pImpl->eDomainId);
      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }

    /*
     * Get voltage from CPR.
     */
    eResult =
      CPR_GetRailVoltageRecommendation(
        pImpl->eDomainId,
        eCornerLDO,
        &nVoltageUV);
    if (eResult != DAL_SUCCESS)
    {
      return eResult;
    }

    /*
     * Set the LDO voltage.
     */
    eResult = VCS_LDORailImplSetVoltage(pImpl, nVoltageUV);
    if (eResult != DAL_SUCCESS)
    {
      return eResult;
    }

    /*
     * Notify CPR of corner change.
     */
    eResult = CPR_SetNextModeCorner(pImpl->eDomainId, eCornerLDO);
    if (eResult != DAL_SUCCESS)
    {
      return eResult;
    }

    /*
     * Enable LDO output voltage.
     */
    if (!pImpl->bIsEnabled)
    {
      HAL_vcs_EnableLDO(&pImpl->HALdesc);

      pImpl->bIsEnabled = TRUE;
    }

    /*
     * Enable CPR sensor chain.
     */
    eResult = CPR_Enable(pImpl->eDomainId);
    if (eResult != DAL_SUCCESS)
    {
      return eResult;
    }

    /*
     * Log the state change.
     */
    ULOG_RT_PRINTF_3(
      VCS_GetDrvCtxt()->hVCSLog,
      "LDO[%s]: Mode[LDO(enabled)] Corner[%lu] Voltage[%lu].",
      VCS_RAIL_NAME(pRail),
      eCornerLDO,
      pImpl->nVoltageUV);
  }

  /*-----------------------------------------------------------------------*/
  /* Disable the LDO.                                                      */
  /*-----------------------------------------------------------------------*/

  else if (pImpl->bIsEnabled)
  {
    /*
     * Disable CPR sensor chain.
     */
    eResult = CPR_Disable(pImpl->eDomainId);
    if (eResult != DAL_SUCCESS)
    {
      return eResult;
    }

    /*
     * Disable LDO output voltage.
     */
    HAL_vcs_DisableLDO(&pImpl->HALdesc);

    pImpl->bIsEnabled = FALSE;
    pImpl->nVoltageUV = 0;

    /*
     * Log the state change.
     */
    ULOG_RT_PRINTF_1(
      VCS_GetDrvCtxt()->hVCSLog,
      "LDO[%s]: Mode[BHS(disabled)].",
      VCS_RAIL_NAME(pRail));
  }

  /*-----------------------------------------------------------------------*/
  /* Log state change.                                                     */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END of VCS_LDORailImplSetCornerCPR */


/* =========================================================================
**  Function : VCS_LDORailImplSetCorner
** =========================================================================*/
/**
  Set LDO to a requested corner.

  @param hClient     [in] -- Pointer to client making request.
  @param eCornerRail [in] -- Requested corner.

  @return
  DAL_SUCCESS -- Sucessfully configured LDO to requested corner.
  DAL_ERROR -- Failed to configure LDO to requested corner.

  @dependencies
  None.
*/

static DALResult VCS_LDORailImplSetCorner
(
  npa_client_handle  hClient,
  rail_voltage_level eCorner
)
{
  DALResult           eResult;
  VCSRailNodeType    *pRail;

  pRail = VCS_CLIENT_TO_RAIL(hClient);

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Increase dependency before increasing this LDO.                       */
  /*-----------------------------------------------------------------------*/

  VCS_NPASatisfyIncreasingDependency(hClient);

  /*-----------------------------------------------------------------------*/
  /* Schedule NAS requests for next wake up event.                         */
  /*                                                                       */
  /* Note: We schedule this event regardless of whether the request was to */
  /* increase or decrease the LDO. However we cannot satisfy the           */
  /* dependency if we are decreasing because we need the existing          */
  /* dependency to be satisfied when we wake up to handle this NAS request.*/
  /*-----------------------------------------------------------------------*/

  if (npa_request_has_attribute(hClient, NPA_REQUEST_NEXT_AWAKE))
  {
    pRail->eCornerNAS = eCorner;

    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Update LDO state.                                                     */
  /*-----------------------------------------------------------------------*/

  if (eCorner != pRail->eCorner)
  {
    eResult = VCS_LDORailImplSetCornerCPR(pRail, eCorner);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Decrease dependency after decreasing this LDO.                        */
  /*-----------------------------------------------------------------------*/

  VCS_NPASatisfyDecreasingDependency(hClient);

  /*-----------------------------------------------------------------------*/
  /* Update context.                                                       */
  /*-----------------------------------------------------------------------*/

  pRail->eCorner = eCorner;

  return DAL_SUCCESS;

} /* END of VCS_LDORailImplSetCorner */


/* =========================================================================
**  Function : VCS_LDORailImplCPRVoltageRecommendation
** =========================================================================*/
/**
  Callback for event sent by CPR when recommending an intra-corner voltage
  update to the eLDO.

  @param pContext   [in] -- Pointer to LDO node type.
  @param nEventType [in] -- NPA event identifier.
  @param pData      [in] -- Pointer to event data.
  @param nDataSize  [in] -- Size of data payload.

  @return
  None.

  @dependencies
  None.
*/

static void VCS_LDORailImplCPRVoltageRecommendation
(
  void         *pContext,
  unsigned int  nEventType,
  void         *pData,
  unsigned int  nDataSize
)
{
  VCSRailNodeType              *pRail;
  VCSLDORailImplType           *pImpl;
  cpr_npa_rail_event_data_type *pEventData;
  DALResult                     eResult;

  /*-----------------------------------------------------------------------*/
  /* Validate context.                                                     */
  /*-----------------------------------------------------------------------*/

  pRail = (VCSRailNodeType *)pContext;
  if (pRail == NULL ||
      pRail->pImpl == NULL ||
      pData == NULL ||
      nEventType != CPR_NPA_RAIL_EVENT_INTRACORNER_VOLTAGE_UPDATE)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Invalid context provided to CPR node available callback.");
  }

  pImpl = (VCSLDORailImplType *)pRail->pImpl;
  pEventData = (cpr_npa_rail_event_data_type *)pData;

  /*-----------------------------------------------------------------------*/
  /* Set LDO voltage.                                                      */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_LDORailImplSetVoltage(pImpl, pEventData->voltage_uv);
  if (eResult == DAL_SUCCESS)
  {
    /*
     * Ack the voltage update.
     */
    pEventData->ack = TRUE;
  }

  /*-----------------------------------------------------------------------*/
  /* Failed voltage update.                                                */
  /*-----------------------------------------------------------------------*/

  else
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Failed to apply LDO voltage recommendation from CPR.");
  }

} /* END VCS_LDORailImplCPRVoltageRecommendation */


/* =========================================================================
**  Function : VCS_LDORailImplCPRNodeAvailableCallback
** =========================================================================*/
/**
  Callback after the CPR node is created.

  This function is called by the NPA framework after the CPR node
  is created.  The creation is delayed until all dependencies are also
  created.

  @param *pContext     [in] -- Context passed in npa_define_node_cb
  @param nEventType    [in] -- Zero.
  @param *pNodeName    [in] -- Name of the node being created.
  @param nNodeNameSize [in] -- Length of the name.

  @return
  None.

  @dependencies
  None.
*/

static void VCS_LDORailImplCPRNodeAvailableCallback
(
  void         *pContext,
  unsigned int  nEventType,
  void         *pNodeName,
  unsigned int  nNodeNameSize
)
{
  VCSRailNodeType    *pRail;
  VCSLDORailImplType *pImpl;

  /*-----------------------------------------------------------------------*/
  /* Validate context.                                                     */
  /*-----------------------------------------------------------------------*/

  pRail = (VCSRailNodeType *)pContext;
  if (pRail == NULL || pRail->pImpl == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Invalid context provided to CPR node available callback.");
  }

  pImpl = (VCSLDORailImplType *)pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Register for intra-corner voltage adjustment recommendations by CPR.  */
  /*-----------------------------------------------------------------------*/

  pImpl->hEventCPR =
    npa_create_event_cb(
      pImpl->szNameCPR,
      "VCS",
      (npa_event_trigger_type)CPR_NPA_RAIL_EVENT_INTRACORNER_VOLTAGE_UPDATE,
      VCS_LDORailImplCPRVoltageRecommendation,
      pContext);
  if (!pImpl->hEventCPR)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Failed to create an NPA event handle for CPR LDO resource.");
  }

} /* END of VCS_LDORailImplCPRNodeAvailableCallback */


/* =========================================================================
**  Function : VCS_LDORailImplInitCornerList
** =========================================================================*/
/**
  Initializes the voltage table for this LDO.

  @param pImpl [in] -- Pointer rail node.

  @return
  DAL_ERROR if a voltage table not initialized, other DAL_SUCCESS.

  @dependencies
  None.
*/

static DALResult VCS_LDORailImplInitCornerList
(
  VCSLDORailImplType *pImpl
)
{
  DALResult             eResult;
  rail_voltage_level    eCorner;
  rail_voltage_level   *pCorners;
  uint32                nAddr, nMask, nNumCorners;
  uint32                i, j;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pImpl == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate each corner in the fuse table.                               */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pImpl->nNumCornerFuses; i++)
  {
    eCorner = pImpl->pCornerFuses[i].eCorner;
    if (eCorner >= RAIL_VOLTAGE_LEVEL_NUM_LEVELS)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Query CPR for set of supported corners.                               */
  /* NOTE: This APR dynamically allocates memory for the corner list.      */
  /*       We must free this memory once no longer needed.                 */
  /*-----------------------------------------------------------------------*/

  eResult = CPR_GetSupportedCorners(pImpl->eDomainId, &pCorners, &nNumCorners);
  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Only read the efuse register if the register mask is non-zero.        */
  /*                                                                       */
  /* Enable support for the corner for either of the below cases:          */
  /*  - Fuse address or mask is zero (i.e. no assigned fuse). This will    */
  /*    allow to test on parts that haven't been fused.                    */
  /*                                                                       */
  /*  - Fuse register and mask are non-zero and the enablement field is    */
  /*    non-zero after a bitwise AND operation with the bit mask.          */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < nNumCorners; i++)
  {
    eCorner = pCorners[i];

    /*
     * Find matching fuse entry for this corner.
     */
    for (j = 0; j < pImpl->nNumCornerFuses; j++)
    {
      if (eCorner == pImpl->pCornerFuses[j].eCorner)
      {
        nAddr = pImpl->pCornerFuses[j].nAddr;
        nMask = pImpl->pCornerFuses[j].nMask;

        /*
         * Add corner to supported list.
         */
        if (!nAddr || !nMask ||
            (nAddr && nMask && in_dword_masked(nAddr, nMask)))
        {
          pImpl->aCorners[pImpl->nNumCorners++] = eCorner;
        }

        break;
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Free memory dynamically allocated by CPR.                             */
  /*-----------------------------------------------------------------------*/

  if (pCorners)
  {
    free(pCorners);
  }

  return DAL_SUCCESS;

} /* END VCS_LDORailImplInitCornerList */


/* =========================================================================
**  Function : VCS_LDORailImplResourceQuery
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

static npa_query_status VCS_LDORailImplResourceQuery
(
  npa_resource   *pResource,
  unsigned int    nID,
  npa_query_type *pResult
)
{
  VCSRailNodeType    *pRail;
  VCSLDORailImplType *pImpl;

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

  pImpl = (VCSLDORailImplType *)pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Check for number of corners supported.                                */
  /*-----------------------------------------------------------------------*/

  if (nID == VCS_NPA_QUERY_VOLTAGE_NUM_ENTRIES)
  {
    pResult->data.value = pImpl->nNumCorners;
    pResult->type = NPA_QUERY_TYPE_VALUE;

    return NPA_QUERY_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Check for current voltage in uV.                                      */
  /*-----------------------------------------------------------------------*/

  if (nID == VCS_NPA_QUERY_VOLTAGE_CURRENT_VOLTAGE_UV)
  {
    uint32 nVoltageUV;

    HAL_vcs_GetLDOVoltage(&pImpl->HALdesc, &nVoltageUV, NULL);
    pResult->data.value = nVoltageUV;
    pResult->type = NPA_QUERY_TYPE_VALUE;

    return NPA_QUERY_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Check for current voltage in HW format.                               */
  /*-----------------------------------------------------------------------*/

  if (nID == VCS_NPA_QUERY_VOLTAGE_CURRENT_VOLTAGE_HW)
  {
    uint32 nVoltageHW;

    HAL_vcs_GetLDOVoltage(&pImpl->HALdesc, NULL, &nVoltageHW);
    pResult->data.value = nVoltageHW;
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
    uint32 nLevel;

    nLevel = nID - VCS_NPA_QUERY_VOLTAGE_LEVEL_ID;
    if (nLevel < pImpl->nNumCorners)
    {
      pResult->data.value = pImpl->aCorners[nLevel];
      pResult->type = NPA_QUERY_TYPE_VALUE;

      return NPA_QUERY_SUCCESS;
    }
  }

  return NPA_QUERY_UNSUPPORTED_QUERY_ID;

} /* END VCS_LDORailImplResourceQuery */


/* =========================================================================
**  Function : VCS_LDORailImplInitImpl
** =========================================================================*/
/**
  Initializes the LDO data for this rail.

  @param *pRail [in] -- Pointer rail node.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

static DALResult VCS_LDORailImplInitImpl
(
  VCSRailNodeType *pRail
)
{
  DALResult           eResult;
  VCSLDORailImplType *pImpl;
  VCSDrvCtxt         *pDrvCtxt;
  VCSLDOConfigType   *pConfig;
  uint32              i;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL ||
      pRail->pImpl == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pImpl = (VCSLDORailImplType *)pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Map register addresses.                                               */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = VCS_GetDrvCtxt();
  if (pDrvCtxt->pBSP->nNumBases)
  {
    eResult = VCS_MapAddress(pDrvCtxt, &pImpl->HALdesc.nAddr);
    if (eResult != DAL_SUCCESS)
    {
      return eResult;
    }

    for (i = 0; i < pImpl->nNumCornerFuses; i++)
    {
      if (pImpl->pCornerFuses[i].nAddr)
      {
        eResult = VCS_MapAddress(pDrvCtxt, &pImpl->pCornerFuses[i].nAddr);
        if (eResult != DAL_SUCCESS)
        {
          return eResult;
        }
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Init internal fields.                                                 */
  /*-----------------------------------------------------------------------*/

  pImpl->bIsEnabled = FALSE;
  pImpl->nVoltageUV = 0;
  pImpl->nNumRestrictions = 0;

  /*-----------------------------------------------------------------------*/
  /* Install function pointer for rail resource query.                     */
  /*-----------------------------------------------------------------------*/

  VCS_RAIL_RES_DEF_RAIL(pRail)->query_fcn = VCS_LDORailImplResourceQuery;

  /*-----------------------------------------------------------------------*/
  /* Init CPR if any configurations are enabled.                           */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < VCS_LDO_MODE_MAX_NUM; i++)
  {
    pConfig = NULL;
    eResult = VCS_LDODetectBSPVersion(pImpl, i, &pConfig);
    if (eResult == DAL_SUCCESS &&
        pConfig != NULL &&
        pConfig->bEnable)
    {
      /*
       * Initialize the corner list.
       */
      eResult = VCS_LDORailImplInitCornerList(pImpl);
      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }

      /*
       * Wait for CPR node to come up for future voltage update events.
       */
      npa_resource_available_cb(
        pImpl->szNameCPR,
        VCS_LDORailImplCPRNodeAvailableCallback,
        pRail);

      break;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Try to detect and setup for the standalone configuration.             */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_LDODetectBSPVersion(pImpl, VCS_LDO_MODE_STANDALONE, &pConfig);
  if (eResult != DAL_SUCCESS || pConfig == NULL)
  {
    pImpl->pActiveConfig = NULL;

    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Apply configuration.                                                  */
  /*-----------------------------------------------------------------------*/

  if (pConfig->bEnable)
  {
    HAL_vcs_ConfigLDO(&pImpl->HALdesc, &pConfig->HALConfig);
  }
  else
  {
    pImpl->nDisable |= VCS_FLAG_DISABLED_BY_BSP;
  }

  /*-----------------------------------------------------------------------*/
  /* Update context.                                                       */
  /*-----------------------------------------------------------------------*/

  pImpl->pActiveConfig = pConfig;

  return DAL_SUCCESS;

} /* END VCS_LDORailImplInitImpl */


/* =========================================================================
**  Function : VCS_LDORailImplAddRestriction
** =========================================================================*/
/**
  See VCSLDORailImpl.h
*/

DALResult VCS_LDORailImplAddRestriction
(
  VCSRailNodeType *pRail
)
{
  VCSLDORailImplType *pImpl;
  npa_resource       *pResource;
  DALResult           eResult = DAL_SUCCESS;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || pRail->pImpl == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pImpl = pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Protect operation on rail/LDO with a critical section.                */
  /*-----------------------------------------------------------------------*/

  pResource = VCS_RAIL_HANDLE(pRail);
  npa_resource_lock(pResource);

  /*-----------------------------------------------------------------------*/
  /* Disallow usage of the LDO on addition of first restriction.           */
  /*-----------------------------------------------------------------------*/

  if (pImpl->nNumRestrictions == 0)
  {
    /*
     * Disable the LDO.
     */
    eResult = VCS_LDORailImplSetCornerCPR(pRail, RAIL_VOLTAGE_LEVEL_OFF);

    /*
     * Update the disable flag.
     */
    pImpl->nDisable |= VCS_FLAG_DISABLED_BY_SWCLIENT;
  }

  /*-----------------------------------------------------------------------*/
  /* Increment the number of restrictions on the LDO.                      */
  /*-----------------------------------------------------------------------*/

  pImpl->nNumRestrictions += 1;

  /*-----------------------------------------------------------------------*/
  /* Release lock.                                                         */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(pResource);

  return eResult;

} /* END VCS_LDORailImplAddRestriction */


/* =========================================================================
**  Function : VCS_LDORailImplRemoveRestriction
** =========================================================================*/
/**
  See VCSLDORailImpl.h
*/

DALResult VCS_LDORailImplRemoveRestriction
(
  VCSRailNodeType *pRail
)
{
  VCSLDORailImplType *pImpl;
  DALResult           eResult;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || pRail->pImpl == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pImpl = pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Protect operation on rail/ldo with a critical section.                */
  /*-----------------------------------------------------------------------*/

  npa_resource_lock(VCS_RAIL_HANDLE(pRail));

  /*-----------------------------------------------------------------------*/
  /* Reallow usage of the LDO on removal of last restriction.              */
  /*-----------------------------------------------------------------------*/

  if (pImpl->nNumRestrictions == 1)
  {
    /*
     * Update the disable flag.
     */
    pImpl->nDisable &= ~VCS_FLAG_DISABLED_BY_SWCLIENT;

    /*
     * Set LDO voltage if supported by CPR.
     */
    eResult = VCS_LDORailImplSetCornerCPR(pRail, pRail->eCorner);
    if (eResult != DAL_SUCCESS)
    {
      return eResult;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Decrement the number of restrictions on the LDO.                      */
  /*-----------------------------------------------------------------------*/

  if (pImpl->nNumRestrictions > 0)
  {
    pImpl->nNumRestrictions--;
  }

  /*-----------------------------------------------------------------------*/
  /* Release lock.                                                         */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(VCS_RAIL_HANDLE(pRail));

  return DAL_SUCCESS;

} /* END VCS_LDORailImplRemoveRestriction */


/* =========================================================================
**  Function : VCS_LDORailImplSetMode
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_LDORailImplSetMode
(
  VCSRailNodeType *pRail,
  VCSLDOModeType   eMode
)
{
  DALResult           eResult;
  VCSLDORailImplType *pImpl;

  /*-----------------------------------------------------------------------*/
  /* Validate arguments.                                                   */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || pRail->pImpl == NULL || eMode >= VCS_LDO_MODE_MAX_NUM)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pImpl = (VCSLDORailImplType *)pRail->pImpl;

  /*-----------------------------------------------------------------------*/
  /* Short-circuit if mode already active.                                 */
  /*-----------------------------------------------------------------------*/

  if (pImpl->pActiveConfig && pImpl->pActiveConfig->eMode == eMode)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Disable LDO using restriction.                                        */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_LDORailImplAddRestriction(pRail);
  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Search for configuration based on this mode.                          */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_LDODetectBSPVersion(pImpl, eMode, &pImpl->pActiveConfig);
  if (eResult != DAL_SUCCESS)
  {
    pImpl->pActiveConfig = NULL;
  }

  /*-----------------------------------------------------------------------*/
  /* Apply configuration.                                                  */
  /*-----------------------------------------------------------------------*/

  if (pImpl->pActiveConfig)
  {
    /*
     * Disable if specified in BSP.
     */
    if (pImpl->pActiveConfig->bEnable != TRUE)
    {
      pImpl->nDisable |= VCS_FLAG_DISABLED_BY_BSP;
    }
    else
    {
      pImpl->nDisable &= ~VCS_FLAG_DISABLED_BY_BSP;

      /*
       * Configure the LDO hardware settings.
       */
      HAL_vcs_ConfigLDO(&pImpl->HALdesc, &pImpl->pActiveConfig->HALConfig);
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update context.                                                       */
  /*-----------------------------------------------------------------------*/

  pImpl->eMode = eMode;

  /*-----------------------------------------------------------------------*/
  /* Reallow usage of LDO by removing restriction.                         */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_LDORailImplRemoveRestriction(pRail);

  return eResult;

} /* END of VCS_LDORailImplSetMode */
