/*
===========================================================================
*/
/**
  @file VCSRails.c 
  
  Rail-related functions for the VCS driver.
*/
/*  
  ====================================================================

  Copyright (c) 2017 QUALCOMM Technologies Incorporated. All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/vcs/src/VCSRails.c#2 $
  $DateTime: 2017/08/07 11:42:50 $
  $Author: pwbldsvc $
 
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "VCSDriver.h"
#include "DALDeviceId.h"
#include <DALSys.h>


/*=========================================================================
      Externs
==========================================================================*/

extern VCSBSPType VCSBSP;


/*=========================================================================
      Macro Definitions
==========================================================================*/


/*=========================================================================
      Prototypes
==========================================================================*/


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : VCS_GetRailNode
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult VCS_GetRailNode
(
  const char       *szName,
  VCSRailNodeType **pRail
)
{
  uint32 i;

  /*-----------------------------------------------------------------------*/
  /* Validate pointers are usable.                                         */
  /*-----------------------------------------------------------------------*/

  if (!szName || !pRail)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize output parameter                                           */
  /*-----------------------------------------------------------------------*/

  *pRail = (VCSRailNodeType *)NULL;

  /*-----------------------------------------------------------------------*/
  /* Search the array of LDO nodes and return match.                       */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < VCSBSP.nNumRails; i++)
  {
    if (strcmp(szName, VCS_RAIL_NAME(&VCSBSP.pRails[i])) == 0)
    {
      *pRail = (VCSRailNodeType *)&VCSBSP.pRails[i];

      return DAL_SUCCESS;
    }
  }

  return DAL_ERROR_NOT_FOUND;

} /* END VCS_GetRailNode */


/* =========================================================================
**  Function : VCS_RailsDetectBSPVersion
** =========================================================================*/
/**
  Detects which corner BSP configuration to use for the current HW.

  @param pRail [in]    -- Pointer node.
  @param pConfig [out] -- Pointer to corner config pointer.
  @return
  DAL_ERROR if a valid configuration was not found, other DAL_SUCCESS.

  @dependencies
  None.
*/

static DALResult VCS_RailsDetectBSPVersion
(
  VCSRailNodeType    *pRail,
  VCSRailConfigType **pConfig
)
{
  uint32             i;
  VCSRailConfigType *pIter;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || pConfig == NULL)
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

  for (i = 0; i < pRail->nNumConfigs; i++)
  {
    pIter = &pRail->pConfigs[i];
    if (VCS_IsBSPSupported(&pIter->HWVersion))
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

} /* END VCS_RailsDetectBSPVersion */


/* =========================================================================
**  Function : VCS_RailsInitConfig
** =========================================================================*/
/**
  Detects and validates the config for a given rail.

  This function detects the supported config for this HW and validates
  each field for a given rail.

  @param pRail[in]  -- Pointer to rail node.

  @return
  DAL_SUCCESS -- Sucessfully detected/validated config.
  DAL_ERROR -- Failed to detect/validate config.

  @dependencies
  None.
*/

DALResult VCS_RailsInitConfig
(
  VCSRailNodeType *pRail
)
{
  DALResult          eResult;
  VCSRailConfigType *pConfig;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Short-circuit.                                                        */
  /*-----------------------------------------------------------------------*/

  if (pRail->pActiveConfig != NULL || pRail->nNumConfigs == 0)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Detect the corner config for this HW version.                         */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_RailsDetectBSPVersion(pRail, &pConfig);
  if (eResult != DAL_SUCCESS || pConfig == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Failed to detect a valid corner config.");

    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate the min corner defined in the BSP.                           */
  /*-----------------------------------------------------------------------*/

  if (pConfig->eCornerMin > RAIL_VOLTAGE_LEVEL_MAX)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Invalid min corner [%lu].",
      pConfig->eCornerMin);

    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate the max corner defined in the BSP.                           */
  /*-----------------------------------------------------------------------*/

  if (pConfig->eCornerMax > RAIL_VOLTAGE_LEVEL_MAX)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Invalid max corner [%lu].",
      pConfig->eCornerMax);

    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate the init corner defined in the BSP.                          */
  /*-----------------------------------------------------------------------*/

  if (pConfig->eCornerInit > RAIL_VOLTAGE_LEVEL_MAX)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Invalid init corner [%lu].",
      pConfig->eCornerInit);

    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Assign the active config.                                             */
  /*-----------------------------------------------------------------------*/

  pRail->pActiveConfig = pConfig;

  return DAL_SUCCESS;

} /* END VCS_RailsInitConfig */


/* =========================================================================
**  Function : VCS_InitRails
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitRails
(
  VCSDrvCtxt *pDrvCtxt
)
{
  uint32           i;
  DALResult        eResult;
  VCSRailNodeType *pRail;

  /*-----------------------------------------------------------------------*/
  /* Nothing to do if no rails present.                                    */
  /*-----------------------------------------------------------------------*/

  if (VCSBSP.nNumRails == 0)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_WARNING,
      "DALLOG Device VCS: VCS_InitRails found 0 rails.");

    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the rail nodes.                                            */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < VCSBSP.nNumRails; i++)
  {
    pRail = &VCSBSP.pRails[i];
    if (pRail == NULL)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: VCS_InitRails found NULL rail at entry[%lu].",
        i);

      return DAL_ERROR_INTERNAL;
    }

    /*
     * Detect the supported rail configuration.
     */
    eResult = VCS_RailsInitConfig(pRail);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Unable to detect supported config for rail[%s]",
        pRail->mNode.name);
    }

    /*
     * Init RPMh impl.
     */
    if (pRail->pControl &&
        pRail->pControl->fpInit)
    {
      eResult = pRail->pControl->fpInit(pRail);
      if (eResult != DAL_SUCCESS)
      {
        DALSYS_LogEvent(
          0,
          DALSYS_LOGEVENT_FATAL_ERROR,
          "DALLOG Device VCS: Unable to init rail[%s]",
          pRail->mNode.name);
      }
    }
  }

  return DAL_SUCCESS;

} /* END VCS_InitRails */


/* =========================================================================
**  Function : VCS_GetBootCorner
** =========================================================================*/
/*
  See VCSLite.h
*/

DALResult VCS_GetBootCorner
(
  const char         *szRail,
  rail_voltage_level *pLevel
)
{
  DALResult        eResult;
  VCSRailNodeType *pRail;

  /*-----------------------------------------------------------------------*/
  /* Validate arguments aren't NULL.                                       */
  /*-----------------------------------------------------------------------*/

  if (szRail == NULL || pLevel == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the rail node associated with the requested string.               */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_GetRailNode(szRail, &pRail);
  if (eResult != DAL_SUCCESS || pRail == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the rail configuration based on this chipset.                     */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_RailsInitConfig(pRail);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the rail configuration based on this chipset.                     */
  /*-----------------------------------------------------------------------*/

  *pLevel = VCS_RAIL_INIT(pRail);

  return DAL_SUCCESS;
}