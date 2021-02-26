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

 $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/systemdrivers/chipinfo/src/ChipInfoImage.c#1 $
 $DateTime: 2018/02/16 04:48:18 $
 $Author: pwbldsvc $

 =============================================================================*/

/*=============================================================================
  Include Files
  =============================================================================*/
#include "msg_ram_layout.h"
#include "ChipInfoImage.h"
#include "PlatformInfoDefs.h"
#include <stdlib.h>
#include <string.h>
#include "smem_type.h"
#include "page_select.h"

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
  smem_table_err msg_ram_err;
  uint32 smem_addr;

  /*
   * Get the smem base from msg_ram
   */
  msg_ram_err = msg_ram_get_smem_addr(SMEM_HW_SW_BUILD_ID, &smem_addr);
  if (msg_ram_err != SMEM_TABLE_SUCCESS || smem_addr == 0)
  {
    /*
     * Don't try accessing socinfo if the address isn't available
     */
    pSMem = NULL;
  }
  else
  {
    /*
     * Map the SMEM address from DDR
     * This needs to be unmapped once it's no longer needed.
     */
    pSMem = (DalPlatformInfoSMemType *)set_ddr_access((uint64)smem_addr);
  }

  return pSMem;
}

void
ChipInfo_UnmapSMem
(
  void
)
{
  set_page_select(0);
}


void *
ChipInfo_malloc
(
  uint32 nSize
)
{
  return malloc(nSize);
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
  if (pDest == NULL || nDestLen ==0 || pSrc == NULL || nSrcLen == 0)
  {
    return NULL;
  }

  memcpy(pDest, pSrc, nSrcLen);

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
