/*============================================================================
  FILE:         pdcTimestamp.c
  
  OVERVIEW:     This file implements the functions for timestamps in the PDC
 
  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/timestamp/src/client/ddr/pdcTimestamp.c#2 $
$DateTime: 2017/11/05 22:36:52 $
============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "pdcTimestamp.h"
#include "CoreVerify.h"
#include "pdc_hwio.h"
#include "DDIChipInfo.h"
#include "pdcInt.h"
#include "pdcGpio.h"
#include "pdcLog.h"
#include "pdcts_qdi.h"

/*===========================================================================
 *                            EXTERNAL VARIABLES
 *===========================================================================*/
extern pdcTimestamp g_pdcTimestamps[MAX_NUM_SUPPORTED_TIMESTAMPS];
extern boolean      g_pdcTimestampSupported;
extern uint8        g_pdcTotalTimestampUnits;

/*===========================================================================
 *                         GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/
 /**
  * pdcTimestamp_initialize
  *
  * @brief Initializes the timestamp system
  * 
  * @note called from RCINIT
  */
void pdcTimestamp_initialize(void)
{
  DalChipInfoVersionType  version = DalChipInfo_ChipVersion ();
  DalChipInfoIdType       chip    = DalChipInfo_ChipId();

  /* Always init the QDI layer */
  pdcTimestamp_QDIInit();

  /* Not supported in Napali V1 */
  if((chip == CHIPINFO_ID_SDM845) &&
     (version < DALCHIPINFO_VERSION(2, 0)))
  {
    pdcLog_printf(0, "PDC timestamps are not supported");
    return;
  }

  g_pdcTotalTimestampUnits = PDC_HWIO_INFI(PDC_PARAM_RESOURCE_DRVd, 0, BLK_NUM_TS_UNIT);

  CORE_VERIFY(g_pdcTotalTimestampUnits <= MAX_NUM_SUPPORTED_TIMESTAMPS);

  memset(g_pdcTimestamps, 0, sizeof(pdcTimestamp) * MAX_NUM_SUPPORTED_TIMESTAMPS);

  g_pdcTimestampSupported = TRUE;

  pdcLog_printf(1, "Avaiable PDC timestamp units: %u", g_pdcTotalTimestampUnits);

  return;
}

/*
 * pdcTimestamp_enable
 */
pdcTimestamp_status pdcTimestamp_enable(uint32 intVector, boolean enable)
{
  pdc_status_type pdcStatus;
  uint8           nUnit;
  uint32          bitNum;
  uint32          freeSlot = -1;

  if(TRUE != g_pdcTimestampSupported)
  {
    return PDC_TS_NO_TS;
  }

  /* Check if timestamp unit is already setup for this vector */
  for(nUnit = 0; nUnit < g_pdcTotalTimestampUnits; nUnit++)
  {
    if((TRUE == g_pdcTimestamps[nUnit].used) && 
       (intVector == g_pdcTimestamps[nUnit].pdc_int_vec))
    {
      /* Vector is already setup, just clear it if disabled, otherwise nothing to do as
       * it's already enabled */
      if(TRUE == enable)
      {
        return PDC_TS_ALREADY_ENABLED;
      }
      else
      {
        pdcLog_printf(1, "Disabling PDC timestamp for vector %u", intVector);

        /* Disable timestamp in HW */
        PDC_HWIO_OUTFI(TIMESTAMP_UNITts_EN, nUnit, ENABLE, 0);
        
        /* Release the data entry */
        g_pdcTimestamps[nUnit].used = FALSE;

        return PDC_TS_SUCCESS;
      }
    }
    else if((-1 == freeSlot) && (FALSE == g_pdcTimestamps[nUnit].used))
    {
      /* Find the first free ts unit */
      freeSlot = nUnit;
    }
  }

  if(TRUE == enable)
  {
    /* Enable the timestamp on the first one available */
    if(-1 != freeSlot)
    {
      /* Check if vector is a valid interrupt or GPIO */
      pdcStatus = pdcInt_findEntry(intVector, &bitNum);

      if(PDC_SUCCESS != pdcStatus)
      {
        pdcStatus = gpio_findEntryByVector(intVector, &bitNum);
      }
      
      if(PDC_SUCCESS == pdcStatus)
      {
        pdcLog_printf(1, "Enabling PDC timestamp for vector %u", intVector);

        CORE_VERIFY(freeSlot < MAX_NUM_SUPPORTED_TIMESTAMPS);
		
        /* Vector is a valid interrupt or GPIO vector, enable it */
        g_pdcTimestamps[freeSlot].pdc_int_vec = intVector;
        g_pdcTimestamps[freeSlot].used        = TRUE;

        PDC_HWIO_OUTFI(TIMESTAMP_UNITts_EVENT_SELECT, freeSlot, EVENT_SELECT, bitNum);
        PDC_HWIO_OUTFI(TIMESTAMP_UNITts_EN, freeSlot, ENABLE, 1);
      }
      else
      {
        return PDC_TS_INVALID_VECTOR;
      }
    }
    else
    {
      return PDC_TS_NO_TS;
    }
  }
  
  /* Never found a TS to disable or successfully enabled */
  return PDC_TS_SUCCESS;
}

/*
 * pdcTimestamp_getUnitData
 */
pdcTimestamp_status pdcTimestamp_getUnitData(pdcTimestamp_unit *unitData, uint32 intVector)
{
  uint8   nUnit;
  uint32  hi;
  uint32  lo;
  uint32  count = 0;
  
  if(TRUE != g_pdcTimestampSupported)
  {
    return PDC_TS_NO_TS;
  }

  CORE_VERIFY_PTR(unitData);

  for(nUnit = 0; nUnit < g_pdcTotalTimestampUnits; nUnit++)
  {
    /* Make sure the vector has a configured timestamp unit */
    if((intVector == g_pdcTimestamps[nUnit].pdc_int_vec) && (TRUE == g_pdcTimestamps[nUnit].used))
    {
      do
      {
        count++;

        unitData->overflow  = PDC_HWIO_INFI2(TIMESTAMP_UNITts_DRVd_OUTPUT, 0, nUnit, OVERFLOW);
        unitData->valid     = PDC_HWIO_INFI2(TIMESTAMP_UNITts_DRVd_OUTPUT, 0, nUnit, TS_VALID);

        /* Note that reading the HI timestamp value resets the overflow and valid bits */
        lo = PDC_HWIO_INFI2(TIMESTAMP_UNITts_DRVd_TIMESTAMP_LO, 0, nUnit, TS_DATA_LO);
        hi = PDC_HWIO_INFI2(TIMESTAMP_UNITts_DRVd_TIMESTAMP_HI, 0, nUnit, TS_DATA_HI);

        /* Avoid any races by reading the lo again */
      }while(lo != PDC_HWIO_INFI2(TIMESTAMP_UNITts_DRVd_TIMESTAMP_LO, 0, nUnit, TS_DATA_LO));

      unitData->timeStamp = ((uint64)hi << 32) | (uint64)lo;

      if(count > 1)
      {
        /* Force the overflow bit since the TS was updated again while trying to read it */
        unitData->overflow = 1;
        return PDC_TS_OVERFLOW;
      }

      return PDC_TS_SUCCESS;
    }
  }

  return PDC_TS_NO_TS;
}

