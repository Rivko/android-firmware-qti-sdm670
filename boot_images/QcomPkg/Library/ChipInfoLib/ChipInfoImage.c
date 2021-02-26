/*
 ===============================================================================

FILE: ChipInfoImage.c

DESCRIPTION:  Image-specific implementations of external functions used by
              ChipInfo which are accessed using different APIs on each image

 ================================================================================
 Copyright (c) 2017 Qualcomm Technologies Incorporated.
 All Rights Reserved.
 QUALCOMM Proprietary and Confidential
 ================================================================================

 $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ChipInfoLib/ChipInfoImage.c#1 $
 $DateTime: 2017/09/18 09:26:13 $
 $Author: pwbldsvc $

 =============================================================================*/

/*=============================================================================
  Include Files
  =============================================================================*/
#include "ChipInfoImage.h"
#include "PlatformInfoDefs.h"
#include <string.h>
#include "smem.h"
#include "DALSys.h"

/*=============================================================================
  Macros
  =============================================================================*/


/*=============================================================================
  Data definitions
  =============================================================================*/


/*=============================================================================
  Functions
  =============================================================================*/
DalPlatformInfoSMemType *
ChipInfo_GetSocInfo
(
  void
)
{
  DalPlatformInfoSMemType *pSMem;
  uint32 nSize;

  pSMem = (DalPlatformInfoSMemType *)smem_get_addr(SMEM_HW_SW_BUILD_ID, &nSize);
  if (pSMem == NULL || nSize == 0)
  {
    return NULL;
  }

  return pSMem;
}

void
ChipInfo_UnmapSMem
(
  void
)
{
  // Nothing to unmap
  return;
}

void *
ChipInfo_malloc
(
  uint32 nSize
)
{
  uint32 *pOut;
  DALResult eResult;
  eResult = DALSYS_Malloc(nSize, (void **)&pOut);
  if (eResult != DAL_SUCCESS)
  {
    // DALSYS_Malloc doesn't guarantee NULL pOut like normal malloc
    pOut = NULL;
  }

  return pOut;
}


void *
ChipInfo_memcpy
(
  void *restrict pDest,
  uint32 nDestLen,
  const void *restrict pSrc,
  uint32 nSrcLen
)
{
  DALResult eResult;

  if (pDest == NULL || nDestLen ==0 || pSrc == NULL || nSrcLen == 0)
  {
    return NULL;
  }

  eResult = DALSYS_memscpy(pDest, nDestLen, (void *)pSrc, nSrcLen);
  return pDest;
}


char *
ChipInfo_strcpy
(
  char *szDest,
  const char *szSrc,
  uint32 nDestLen
)
{
  if (szDest == NULL || szSrc == NULL || nDestLen == 0)
  {
    return NULL;
  }

  strlcpy(szDest, szSrc, nDestLen);
  return szDest;
}
