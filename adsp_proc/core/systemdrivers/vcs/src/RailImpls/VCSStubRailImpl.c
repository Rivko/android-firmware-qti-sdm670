/*
===========================================================================
*/
/**
  @file VCSStubRailImpl.c
  
  Stub functions for NPA rail resource.
*/
/*  
  ====================================================================

  Copyright (c) 2017 QUALCOMM Technologies Incorporated. All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/vcs/src/RailImpls/VCSStubRailImpl.c#2 $
  $DateTime: 2017/08/07 11:42:50 $
  $Author: pwbldsvc $
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "VCSDriver.h"
#include "VCSStubRailImpl.h"
#include <DALSys.h>
#include "DALDeviceId.h"
#include <npa_resource.h>
#include "npa_scheduler.h"
#include "voltage_level.h"
#include "npa.h"


/*=========================================================================
      Prototypes
==========================================================================*/

static DALResult VCS_StubRailImplInitImpl(VCSRailNodeType *);
static DALResult VCS_StubRailImplSetCorner(npa_client_handle, rail_voltage_level);


/*=========================================================================
      Data
==========================================================================*/

VCSRailControlType VCSStubRailImplControl =
{
  .fpInit      = VCS_StubRailImplInitImpl,
  .fpSetCorner = VCS_StubRailImplSetCorner,
};


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : VCS_StubRailImplSetCorner
** =========================================================================*/
/**
  Set stub to a requested corner.

  @param hClient     [in] -- Pointer to client making request.
  @param eCornerRail [in] -- Requested corner.

  @return
  DAL_SUCCESS -- Sucessfully configured stub to requested corner.
  DAL_ERROR -- Failed to configure stub to requested corner.

  @dependencies
  None.
*/

static DALResult VCS_StubRailImplSetCorner
(
  npa_client_handle  hClient,
  rail_voltage_level eCorner
)
{
  VCSRailNodeType *pRail;

  pRail = VCS_CLIENT_TO_RAIL(hClient);

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Increase dependency before increasing this stub.                      */
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
  /* Update context.                                                       */
  /*-----------------------------------------------------------------------*/

  pRail->eCorner = eCorner;

  /*-----------------------------------------------------------------------*/
  /* Decrease dependency after decreasing this stub.                       */
  /*-----------------------------------------------------------------------*/

  VCS_NPASatisfyDecreasingDependency(hClient);

  return DAL_SUCCESS;

} /* END of VCS_StubRailImplSetCorner */


/* =========================================================================
**  Function : VCS_StubRailImplInitImpl
** =========================================================================*/
/**
  Initializes the stub data for this rail.

  @param *pRail [in] -- Pointer rail node.

  @return
  DAL_ERROR if a corner list not initialized, other DAL_SUCCESS.

  @dependencies
  None.
*/

static DALResult VCS_StubRailImplInitImpl
(
  VCSRailNodeType *pRail
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  return DAL_SUCCESS;

} /* END VCS_StubRailImplInitImpl */
