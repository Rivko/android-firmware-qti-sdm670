/*============================================================================
  FILE:         pdcTimestamp_cmn.c
  
  OVERVIEW:     This file implements the functions for timestamps in the PDC
                for island mode
 
  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/timestamp/src/client/pdcTimestamp.c#2 $
$DateTime: 2017/11/05 22:36:52 $
============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "pdcTimestamp.h"
#include "urpmh.h"
#include "pdc_hwio.h"

/*===========================================================================
 *                            TYPES AND MACROS
 *===========================================================================*/
#define SENSORS_SLAVE_ID                0x10000   /* Assume to be fixed for now */
#define URPMH_TS_READ(name, idx1, idx2) (SENSORS_SLAVE_ID | PDC_HWIO_REG_OFFSETI2(name, idx1, idx2))

/*===========================================================================
 *                            GLOBAL VARIABLES
 *===========================================================================*/
/* Main data structure for handling timestamps */
pdcTimestamp  g_pdcTimestamps[MAX_NUM_SUPPORTED_TIMESTAMPS];

/* Flag to indicate if timestamps are supported */
boolean       g_pdcTimestampSupported = FALSE;

/* Number of timestamps supported */
uint8         g_pdcTotalTimestampUnits = 0;

/*===========================================================================
 *                         GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/
/*
 * uPdcTimestamp_getUnitData
 */
pdcTimestamp_status uPdcTimestamp_getUnitData(pdcTimestamp_unit *unitData, uint32 intVector)
{
  uint8           nUnit;
  uint32          tsState;
  urpmh_read_enum status;
  uint32          temp;
  uint32          hi;
  uint32          lo;
  uint32          count = 0;
  
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

        /* Timestamp state bits */
        status = urpmh_issue_read(URPMH_TS_READ(TIMESTAMP_UNITts_DRVd_OUTPUT, 0, nUnit), &tsState);

        if(URPMH_READ_SUCCESS == status)
        {
          unitData->overflow  = PDC_HWIO_FIELD_VAL(TIMESTAMP_UNITts_DRVd_OUTPUT, OVERFLOW, tsState);
          unitData->valid     = PDC_HWIO_FIELD_VAL(TIMESTAMP_UNITts_DRVd_OUTPUT, TS_VALID, tsState);

          /* Note that reading the HI timestamp value resets the overflow and valid bits */
          urpmh_issue_read(URPMH_TS_READ(TIMESTAMP_UNITts_DRVd_TIMESTAMP_LO, 0, nUnit), &lo);
          urpmh_issue_read(URPMH_TS_READ(TIMESTAMP_UNITts_DRVd_TIMESTAMP_HI, 0, nUnit), &hi); 

          /* Avoid any races by reading the lo again */
          status = urpmh_issue_read(URPMH_TS_READ(TIMESTAMP_UNITts_DRVd_TIMESTAMP_LO, 0, nUnit), &temp);
        }

        if(URPMH_READ_SUCCESS != status)
        {
          if(URPMH_READ_FAIL_NOT_IN_ISLAND == status)
          {
            return PDC_TS_NOT_IN_ISLAND;
          }
          else
          {
            return PDC_TS_FAIL;
          }
        }
      }while(temp != lo);

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

