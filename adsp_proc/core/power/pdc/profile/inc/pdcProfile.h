#ifndef _PDCPROFILE_H_
#define _PDCPROFILE_H_
/*============================================================================
  FILE:         pdcProfile.h
  
  OVERVIEW:     This file provides the types for the PDC profiling driver
 
  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/profile/inc/pdcProfile.h#1 $
$DateTime: 2017/07/21 22:10:47 $
============================================================================*/
#include "comdef.h"
#include "profile_config.h"

/* Data assocated with each timestamp unit */
typedef struct profile_unit_s
{
  uint64  timeStamp;
  boolean valid;
  boolean overflow;
}profile_unit;

/* Profling unit enum type which is specific to each target configuration */
#define X(n,v,e) n = v,
typedef enum profile_unit_type_s 
{
  PROFILE_ENUM_MAP(X)

  PDC_PROFILE_LAST,     /* Internal enum for last entry - do not reference directly */
  PDC_PROFILE_ALL_MASK = (((PDC_PROFILE_LAST - 1) << 1) - 1)
}profile_unit_type;
#undef X

/*===========================================================================
 *                         GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/
/**
 * pdcProfile_getUnitData
 *
 * @brief Gets the desired profiling unit data which will be populated after resuming
 *        from power collapse
 * 
 * @note Once a unit is read, it's status bits will be reset, however the actual timestamp
 *       value should be readable multiple times.
 * 
 *       HW bug QCTDD03439727: Unit 0 valid/overflow bits cleared on ANY timestamp read
 * 
 *                             This function can be called at any time, and multiple times.
 *                             Higher level code must ensure it reads Unit 0 (if it cares about
 *                             status) before reading any other units when resuming from PC.
 * 
 *       HW bug QCTDD03447497: Unit 4 can be overwritten when RSC sends wakeup request
 * 
 *                             This unit should not be used, and instead, spare unit 5 will be 
 *                             configured to timestamp after WFI
 * 
 * @param unitData:     Data structure pointer where the requested unit data will be populated
 *                      Must be large enough to hold all of the requested units
 * @param numUnits:     Number of units the structure above can hold
 * @param typeBitMask:  Bit mask of the desired profiling units to get 
 */
uint32 pdcProfile_getUnitData(profile_unit       *unitData, 
                              uint32             numUnits,
                              profile_unit_type  typeBitMask);
#endif /* _PDCPROFILE_H_ */

