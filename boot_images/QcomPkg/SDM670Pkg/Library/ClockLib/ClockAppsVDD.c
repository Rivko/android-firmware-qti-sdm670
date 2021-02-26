/*
===========================================================================
*/
/**
  @file ClockAppsVDD.c

  CPR support for the Apps clock driver.
*/
/*
  ====================================================================

  Copyright (c) 2013-2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockLib/ClockAppsVDD.c#1 $
  $DateTime: 2017/09/29 23:30:49 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  02/06/13   sr      Ported from MSS.
  02/17/12   vs      Created.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "Drivers/ClockDxe/ClockDriver.h"
#include "ClockApps.h"

#ifdef WIN8EA
#include "Ppp.h"
#include "Idle_MicroPep.h"
#ifndef CONSOLE_DEBUGGING
#include "PEP_wpp.h"
#include "ClockAppsVDD.tmh"
#endif
#include "ClockQdss.h"
#endif


/*=========================================================================
      Macros
==========================================================================*/

/*=========================================================================
      Type Definitions
==========================================================================*/

/*=========================================================================
      Prototypes
==========================================================================*/

/*=========================================================================
      Data
==========================================================================*/

/*=========================================================================
      Externals
==========================================================================*/

/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_GetCornerData
** =========================================================================*/
/**
  Retrieves the data for a particular corner.

  @param pDrvCtxt          [in] -- Pointer to the driver context.
  @param eCPU              [in] -- CPU/Cluster identifier.
  @param nCornerTableIndex [in] -- Index into the Corner table for /vdd/apc.
  @param pCornerData       [in] -- Pointer to the corner data.

  @return
  None.

  @dependencies
  None.
*/

static void Clock_GetCornerData
(
  ClockDrvCtxt                    *pDrvCtxt,
  ClockAppsCPUType                 eCPU,
  uint32                           nCornerTableIndex,
  const ClockVRegCornerDataType  **pCornerData
)
{
  ClockAppsCtxtType             *pAppsCtxt = (ClockAppsCtxtType *)pDrvCtxt->pImageCtxt;
  const ClockVRegCornerDataType *pCornerDataFirst;

  /*-----------------------------------------------------------------------*/
  /* Validate the arguments.                                               */
  /*-----------------------------------------------------------------------*/

  if (pCornerData == NULL)
  {
    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the first corner voltage data.                                    */
  /*-----------------------------------------------------------------------*/

  pCornerDataFirst = pAppsCtxt->aCPU[eCPU].pBSPConfig->pVRegCornerData;
  if (pCornerDataFirst == NULL)
  {
    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Update the client's pointer.                                          */
  /*-----------------------------------------------------------------------*/

  *pCornerData = &pCornerDataFirst[nCornerTableIndex];

  return;

} /* END Clock_GetCornerData */


/* =========================================================================
**  Function : Clock_MapVDDAPCCornerToVoltage
** =========================================================================*/
/**
  Maps an APC rail corner index to a raw voltage.

  This function takes in an APC voltage rail corner index and returns a raw voltage.

  @param pDrvCtxt          [in] -- Pointer to the driver context.
  @param eCPU              [in] -- CPU/Cluster identifier.
  @param nCornerTableIndex [in] -- Index into the Corner table for /vdd/apc.

  @return
  Raw voltage in uV.

  @dependencies
  None.
*/

uint32 Clock_MapVDDAPCCornerToVoltage
(
  ClockDrvCtxt     *pDrvCtxt,
  ClockAppsCPUType  eCPU,
  uint32            nCornerTableIndex
)
{
  const ClockVRegCornerDataType *pCornerData = NULL;
  uint32                         nVoltageUV = 0;

  /*-----------------------------------------------------------------------*/
  /* Get the corner data.                                                  */
  /*-----------------------------------------------------------------------*/

  Clock_GetCornerData(pDrvCtxt, eCPU, nCornerTableIndex, &pCornerData);
  if (pCornerData != NULL)
  {
    nVoltageUV = pCornerData->nInitialVoltageUV;
  }

  return nVoltageUV;

} /* END Clock_MapVDDAPCCornerToVoltage */

