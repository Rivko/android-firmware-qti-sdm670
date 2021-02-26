/**
  @file PlatformInfo.c
*/
/*
  ====================================================================
  Copyright (c) 2016 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  QUALCOMM Proprietary and Confidential.
  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "PlatformInfoInternal.h"
#include "PlatformInfoHWIO.h"
#include "HALhwio.h"


/*=========================================================================
      Data
==========================================================================*/

/*
 * Driver context.
 */
PlatformInfoDrvCtxt PlatformInfo_DriverCtxt;


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : PlatformInfo_Platform
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DalPlatformInfoPlatformType PlatformInfo_Platform
(
  void
)
{
  PlatformInfoDrvCtxt *pDrvCtxt = PlatformInfo_GetDrvCtxt();
  return pDrvCtxt->PlatformInfo.platform;

} /* END PlatformInfo_Platform */


/* =========================================================================
**  Function : PlatformInfo_Subtype
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

uint32 PlatformInfo_PlatformSubtype
(
  void
)
{
  PlatformInfoDrvCtxt *pDrvCtxt = PlatformInfo_GetDrvCtxt();
  return pDrvCtxt->PlatformInfo.subtype;

} /* END PlatformInfo_PlatformSubtype */


/* =========================================================================
**  Function : PlatformInfo_PlatformVersion
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

uint32 PlatformInfo_PlatformVersion
(
  void
)
{
  PlatformInfoDrvCtxt *pDrvCtxt = PlatformInfo_GetDrvCtxt();
  return pDrvCtxt->PlatformInfo.version;

} /* END PlatformInfo_PlatformVersion */


/* =========================================================================
**  Function : PlatformInfo_IsFusion
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

boolean PlatformInfo_IsFusion
(
  void
)
{
  PlatformInfoDrvCtxt *pDrvCtxt = PlatformInfo_GetDrvCtxt();
  return pDrvCtxt->PlatformInfo.fusion;

} /* END PlatformInfo_IsFusion */


/* =========================================================================
**  Function : PlatformInfo_GetPlatformInfo
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_GetPlatformInfo
(
  DalPlatformInfoPlatformInfoType *pInfo
)
{
  PlatformInfoDrvCtxt *pDrvCtxt = PlatformInfo_GetDrvCtxt();

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
  DalPlatformInfoKeyType   eKey,
  uint32                  *nValue
)
{
  PlatformInfoDrvCtxt *pDrvCtxt = PlatformInfo_GetDrvCtxt();
  uint32 i;

  if (eKey > DALPLATFORMINFO_NUM_KEYS || nValue == NULL)
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
**  Function : PlatformInfo_GetDrvCtxt
** =========================================================================*/
/*
  See PlatformInfoInternal.h
*/

PlatformInfoDrvCtxt *PlatformInfo_GetDrvCtxt (void)
{
  static boolean bInitialized = FALSE;

  if (!bInitialized)
  {
    bInitialized = TRUE;
    PlatformInfo_Init();
  }

  return &PlatformInfo_DriverCtxt;

} /* END PlatformInfo_GetDrvCtxt */


/* =========================================================================
**  Function : PlatformInfo_DetectPlatformType
** =========================================================================*/
/*
  See PlatformInfoInternal.h
*/

DalPlatformInfoPlatformType PlatformInfo_DetectPlatformType (void)
{
  uint32 nVal;

  if (HWIO_ADDR(PLATFORMINFO_TCSR_SOC_EMULATION_TYPE) == 0)
  {
    return DALPLATFORMINFO_TYPE_UNKNOWN;
  }

  nVal = HWIO_INF(PLATFORMINFO_TCSR_SOC_EMULATION_TYPE, SOC_EMULATION_TYPE);
  switch (nVal)
  {
    case 1:   return DALPLATFORMINFO_TYPE_RUMI;
    case 3:   return DALPLATFORMINFO_TYPE_VIRTIO;
    default:
      return DALPLATFORMINFO_TYPE_UNKNOWN;
  }

} /* END PlatformInfo_DetectPlatformType */
