/*============================================================================
  FILE:         pdcProfile.c
  
  OVERVIEW:     This file implements the functions for profiling the PDC
 
  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/profile/src/os/pdcProfile.c#2 $
$DateTime: 2017/09/13 08:52:52 $
============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "pdcProfile.h"
#include "pdc_hwio.h"
#include "pdcLog.h"

/*===========================================================================
 *                          GLOBAL VARIABLES
 *===========================================================================*/
/* Total unit types supported in this target */
#define X(n,v,e) + 1
static uint32 g_pdcTotalProfileUnits = 0 + PROFILE_ENUM_MAP(X);
#undef X

#define X(n,v,e) e,
static boolean g_pdcProfileEnableMask[] = {PROFILE_ENUM_MAP(X)};
#undef X

/*===========================================================================
 *                         GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/
/**
 * pdcProfile_initialize
 *
 * @brief Initializes the profiling system
 * 
 * @note called from RCINIT
 */
void pdcProfile_initialize(void)
{
  uint32 nUnits;
  uint32 count      = 0;
  uint32 totalUnits = PDC_HWIO_INFI(PDC_PARAM_RESOURCE_DRVd, 0, BLK_NUM_PROFILING_UNIT);

  CORE_VERIFY(totalUnits == g_pdcTotalProfileUnits);

  /* Enable all the profiling units */
  for(nUnits = 0; nUnits < g_pdcTotalProfileUnits; nUnits++)
  {
    if(TRUE == g_pdcProfileEnableMask[nUnits])
    {
      /* Enable selected profiling units */
      PDC_HWIO_OUTFI(PROFILING_UNITpf_EN, nUnits, PROFILING_UNIT_ENABLE, 1);
      count++;
    }
  }

  pdcLog_printf(2, "Successfully enabled %u of %u profiling units",
                count, totalUnits);
  return;
}

/*
 * pdcProfile_getUnitData
 */
uint32 pdcProfile_getUnitData(profile_unit       *unitData, 
                              uint32             numUnits,
                              profile_unit_type  typeBitMask)
{
  uint32        nUnit;
  profile_unit  *unit;
  uint32        count = 0;

  for(nUnit = 0; (count < numUnits) && (nUnit < g_pdcTotalProfileUnits); nUnit++)
  {
    if((typeBitMask & 0x01) && 
       (TRUE == g_pdcProfileEnableMask[nUnit]))
    {
      unit = &unitData[count];

      unit->overflow  = PDC_HWIO_INFI2(PROFILING_UNITpf_DRVd_OUTPUT, 0, nUnit, PROFILING_OVERFLOW);
      unit->valid     = PDC_HWIO_INFI2(PROFILING_UNITpf_DRVd_OUTPUT, 0, nUnit, PROFILING_TS_VALID);

      /* Note that reading the HI timestamp value resets the overflow and valid bits */
      unit->timeStamp = PDC_HWIO_INFI2(PROFILING_UNITpf_DRVd_TIMESTAMP_LO, 0, nUnit, PROFILING_TS_DATA_LO);
      unit->timeStamp |= ((uint64)PDC_HWIO_INFI2(PROFILING_UNITpf_DRVd_TIMESTAMP_HI, 0, nUnit, PROFILING_TS_DATA_HI) << 32);

      count++; 
    }

    typeBitMask = typeBitMask >> 1;
  }

  return count;
}

