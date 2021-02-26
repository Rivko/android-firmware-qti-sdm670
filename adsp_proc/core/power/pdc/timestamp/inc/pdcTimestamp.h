#ifndef _PDCTIMESTAMP_H_
#define _PDCTIMESTAMP_H_
/*============================================================================
  FILE:         pdcTimestamp.h
  
  OVERVIEW:     This file provides the types for the PDC timestamp driver
 
  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/timestamp/inc/pdcTimestamp.h#2 $
$DateTime: 2017/11/05 22:36:52 $
============================================================================*/
#include "comdef.h"

/*===========================================================================
 *                          TYPES AND DEFINITIONS
 *===========================================================================*/
/* Data assocated with each timestamp unit */
typedef struct pdcTimestamp_unit_s
{
  uint64  timeStamp;
  boolean valid;
  boolean overflow;
}pdcTimestamp_unit;

typedef enum 
{
  PDC_TS_SUCCESS          = 0,  /* Function success */
  PDC_TS_ALREADY_ENABLED,       /* TS is already enabled for given interrupt */
  PDC_TS_OVERFLOW,              /* During read, one or more interrupts happened */

  PDC_TS_FAIL             = 16, /* General function failure */
  PDC_TS_NOT_IN_ISLAND,         /* Called micro version API function outside of island */
  PDC_TS_NO_TS,                 /* No timestamps available - HW is unsupported */
  PDC_TS_INVALID_VECTOR,        /* Invalid interrupt vector, unsupported or not registered */
}pdcTimestamp_status;

/* Main internal data structure */
typedef struct pdcTimestamp_s
{
  pdcTimestamp_unit data;
  uint32            pdc_int_vec;
  boolean           used;
}pdcTimestamp;

/*===========================================================================
 *                         GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/
/**
 * pdcTimestamp_getUnitData
 *
 * @brief Gets the desired timestamp unit data which will be populated after the interrupt
 *        is asserted
 * 
 * @note Once a unit is read, it's status bits will be reset, however the actual timestamp
 *       value should be readable multiple times.
 * 
 * @param unitData:   Data structure pointer where the requested unit data will be populated
 * @param intVector:  Interrupt vector for desired timestamp information
 */
pdcTimestamp_status pdcTimestamp_getUnitData(pdcTimestamp_unit *unitData, uint32 intVector);

/**
 * pdcTimestamp_enable
 *
 * @brief Enables or disables the desired timestamp unit data capture for the given interrupt
 * 
 * @note Once a unit is read, it's status bits will be reset, however the actual timestamp
 *       value should be readable multiple times.
 *  
 *       This function will continue to read the timestamps until a steady state is acheived 
 * 
 * @param intVector:  Interrupt vector for desired timestamp information 
 * @param enable:     Flag to enable or disable the timestamp
 */
pdcTimestamp_status pdcTimestamp_enable(uint32 intVector, boolean enable);

#endif /* _PDCTIMESTAMP_H_ */

