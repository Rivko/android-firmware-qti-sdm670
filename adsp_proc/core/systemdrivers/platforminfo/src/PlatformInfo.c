/**
  @file PlatformInfo.c

  Interface implementation file for the PlatformInfo driver.
*/
/*
  ====================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/platforminfo/src/PlatformInfo.c#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "PlatformInfo.h"
#include <stringl/stringl.h>


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : PlatformInfo_DriverInit
** =========================================================================*/
/**
  Initialize the PlatformInfo driver.

  This function initializes the PlatformInfo driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult PlatformInfo_DriverInit
(
  PlatformInfoDrvCtxt *pDrvCtxt
)
{
  DALResult eResult;
  uint32 nSize;

  DALGLBCTXT_Init();
  DALGLBCTXT_FindCtxt("DAL_PLATFORM_INFO", (void **)&pDrvCtxt->pGlbCtxt);

  if (pDrvCtxt->pGlbCtxt == NULL)
  {
    return DAL_SUCCESS;
  }

  pDrvCtxt->PlatformInfo = pDrvCtxt->pGlbCtxt->PlatformInfo;
  pDrvCtxt->nNumKVPS = pDrvCtxt->pGlbCtxt->nNumKVPS;

  if (pDrvCtxt->nNumKVPS > 0)
  {
    nSize = sizeof(PlatformInfoKVPSType) * pDrvCtxt->nNumKVPS;
    eResult = DALSYS_Malloc(nSize, (void **)&pDrvCtxt->aKVPS);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }

    memscpy(pDrvCtxt->aKVPS, nSize, pDrvCtxt->pGlbCtxt->aKVPS, nSize);
  }

  return DAL_SUCCESS;

} /* END PlatformInfo_DriverInit */


/* =========================================================================
**  Function : PlatformInfo_GetPlatform
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_GetPlatform
(
  PlatformInfoDrvCtxt *pDrvCtxt,
  DalPlatformInfoPlatformType *pType
)
{
  if (pType != NULL)
  {
    *pType = pDrvCtxt->PlatformInfo.platform;
    return DAL_SUCCESS;
  }

  return DAL_ERROR;

} /* END PlatformInfo_GetPlatform */


/* =========================================================================
**  Function : PlatformInfo_GetPlatformInfo
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_GetPlatformInfo
(
  PlatformInfoDrvCtxt *pDrvCtxt,
  DalPlatformInfoPlatformInfoType *pInfo
)
{
  if (pInfo != NULL)
  {
    *pInfo = pDrvCtxt->PlatformInfo;
    return DAL_SUCCESS;
  }

  return DAL_ERROR;

} /* END PlatformInfo_GetPlatformInfo */


/* =========================================================================
**  Function : PlatformInfo_GetKeyValue
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_GetKeyValue
(
  PlatformInfoDrvCtxt     *pDrvCtxt,
  DalPlatformInfoKeyType   eKey,
  uint32                  *nValue
)
{
  uint32 i;

  if (nValue == NULL)
  {
    return DAL_ERROR;
  }

  for (i = 0; i < pDrvCtxt->nNumKVPS; i++)
  {
    if (eKey == pDrvCtxt->aKVPS[i].nKey)
    {
      *nValue = pDrvCtxt->aKVPS[i].nValue;
      return DAL_SUCCESS;
    }
  }

  return DAL_ERROR;

} /* END PlatformInfo_GetKeyValue */


/* =========================================================================
**  Function : PlatformInfo_CDTConfigPreDDR
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_CDTConfigPreDDR
(
  PlatformInfoDrvCtxt *pDrvCtxt,
  void                *pPlatformInfo
)
{
  /*
   * Nothing to do on this image.
   */
  return DAL_SUCCESS;

} /* END PlatformInfo_CDTConfigPreDDR*/


/* =========================================================================
**  Function : PlatformInfo_CDTConfigPostDDR
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_CDTConfigPostDDR
(
  PlatformInfoDrvCtxt *pDrvCtxt,
  void                *pPlatformInfo
)
{
  /*
   * Nothing to do on this image.
   */
  return DAL_SUCCESS;

} /* END PlatformInfo_CDTConfigPostDDR */

