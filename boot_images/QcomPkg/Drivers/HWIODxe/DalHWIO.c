/*============================================================================

FILE:      DalHWIO.c

DESCRIPTION:
  This file contains the init and common routines for the HWIO DAL.

PUBLIC CLASSES:
  HWIO_DriverInit
  HWIO_DriverDeInit
  HWIO_DeviceInit
  HWIO_DeviceDeInit
  HWIO_PowerEvent
  HWIO_Open
  HWIO_Close
  HWIO_Info

============================================================================
        Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "DalHWIO.h"
#include "DALDeviceId.h"


/*=========================================================================
      Function Definitions
==========================================================================*/


/*==========================================================================

  FUNCTION      HWIO_DriverInit

  DESCRIPTION   See DDIHWIO.h

==========================================================================*/

DALResult HWIO_DriverInit
(
  HWIODrvCtxt *pCtxt
)
{
  DALSYS_PROPERTY_HANDLE_DECLARE(hProps);
  DALSYSPropertyVar PropVar;
  DALResult eResult; 
  uint32 nSize;
  HWIOPhysRegionType *pPhysEntry;

  /*-----------------------------------------------------------------------*/
  /* Create our map mutex object.                                          */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncCreate(
    DALSYS_SYNC_ATTR_RESOURCE, &pCtxt->hMapSync, &pCtxt->mMapSyncObj);

  /*-----------------------------------------------------------------------*/
  /* Get the HWIO map tables.                                              */
  /*-----------------------------------------------------------------------*/

  eResult = DALSYS_GetDALPropertyHandle(DALDEVICEID_HWIO, hProps);

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  eResult = DALSYS_GetPropertyValue(hProps, "BaseMap", 0, &PropVar);

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  pCtxt->PhysMap = (HWIOPhysRegionType *)PropVar.Val.pStruct;

  /*-----------------------------------------------------------------------*/
  /* Create the HWIO memory table.                                         */
  /*-----------------------------------------------------------------------*/

  nSize = 0;
  pPhysEntry = pCtxt->PhysMap;
  while (pPhysEntry->szName != NULL)
  {
    pPhysEntry++;
    nSize++;
  }

  nSize *= sizeof(HWIOVirtRegionType);

  eResult = DALSYS_Malloc(nSize, (void **)&pCtxt->VirtMap);

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }
  memset(pCtxt->VirtMap, 0, nSize);

  return DAL_SUCCESS;

} /* END HWIO_DriverInit */


/*==========================================================================

  FUNCTION      HWIO_DriverDeInit

  DESCRIPTION   See DDIHWIO.h

==========================================================================*/

DALResult HWIO_DriverDeInit
(
  HWIODrvCtxt *pCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Destroy the mutex objects                                             */
  /*-----------------------------------------------------------------------*/

  DALSYS_DestroyObject(pCtxt->hMapSync);
  DALSYS_Free(pCtxt->VirtMap);

	return DAL_SUCCESS;

} /* END HWIO_DriverDeInit */



DALResult 
HWIO_DeviceInit(HWIOClientCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult 
HWIO_DeviceDeInit(HWIOClientCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult 
HWIO_PowerEvent(HWIOClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
  return DAL_SUCCESS;
}

DALResult 
HWIO_Open(HWIOClientCtxt *pCtxt, uint32 dwaccessMode )
{
  return DAL_SUCCESS;
}

DALResult 
HWIO_Close(HWIOClientCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult 
HWIO_Info(HWIOClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
  return DAL_SUCCESS;
}



