/**
  @file PlatformInfo.c

  Interface implementation file for the PlatformInfo driver.
*/
/*
  ====================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================


  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "PlatformInfoInternal.h"
#include "LoaderUtils.h"


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : PlatformInfo_Init
** =========================================================================*/
/**
  See PlatformInfo.h
*/

DALResult PlatformInfo_Init
(
  void
)
{
  PlatformInfoDrvCtxt *pDrvCtxt = PlatformInfo_GetDrvCtxt();
  DALResult eResult;
  uint32 nSize;

  if (pDrvCtxt->bInitialized)
  {
    return DAL_SUCCESS;
  }

  DALGLBCTXT_Init();
  DALGLBCTXT_FindCtxt("DAL_PLATFORM_INFO", (void **)&pDrvCtxt->pGlbCtxt);

  if (pDrvCtxt->pGlbCtxt != NULL)
  {
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
  }

  if (pDrvCtxt->PlatformInfo.platform == DALPLATFORMINFO_TYPE_UNKNOWN)
  {
    pDrvCtxt->PlatformInfo.platform = PlatformInfo_DetectPlatformType();
  }

  pDrvCtxt->bInitialized = TRUE;
  return DAL_SUCCESS;

} /* END PlatformInfo_Init */


/* =========================================================================
**  Function : PlatformInfo_CDTConfigPreDDR
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_CDTConfigPreDDR
(
  void *pPlatformInfo
)
{
  /*
   * Nothing to do for XBL Core.
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
  void *pPlatformInfo
)
{
  /*
   * Nothing to do for XBL Core.
   */
  return DAL_SUCCESS;

} /* END PlatformInfo_CDTConfigPostDDR */

