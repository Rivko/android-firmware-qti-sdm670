#ifndef _SLEEP_STATS_TYPES_H_
#define _SLEEP_STATS_TYPES_H_
/*==============================================================================
  FILE:         sleep_stats_types.h
  
  OVERVIEW:     Functions & types for global interest statistic data
 
  DEPENDENCIES: None
                
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/api/power/sleep2.0/sleep_stats_types.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                                TYPE DEFINITIONS
 =============================================================================*/
/**
 * sleepStats_generic32
 *
 * @brief Basic statistics data structure with maximum uint32 values
 */
typedef struct sleepStats_generic32_s
{
  uint32  count;  /* measurement count */
  uint64  total;  /* measurement total */
  uint32  max;    /* measurement maximum value */
  uint32  min;    /* measurement minimum value*/
}sleepStats_generic32;

/**
 * sleepStats_generic
 *
 * @brief Basic statistics data structure
 */
typedef struct sleepStats_generic_s
{
  uint32  count;  /* measurement count */
  uint64  total;  /* measurement total */
  uint64  max;    /* measurement maximum value */
  uint64  min;    /* measurement minimum value*/
}sleepStats_generic;

/**
 * sleepStats_generic_signed
 *
 * @brief Basic SIGNED statistics data structure
 */
typedef struct sleepStats_generic_signed_s
{
  uint32  count;  /* measurement count */
  int64   total;  /* measurement total */
  int64   max;    /* measurement maximum value */
  int64   min;    /* measurement minimum value*/
}sleepStats_generic_signed;

/*==============================================================================
                              FUNCTION DECLARATIONS
 =============================================================================*/
/** 
 * sleepStats_updateValue
 *  
 * @brief Update data fields of the generic statistics structure
 *        
 * @param stats: Pointer to generic data structure to update
 * @param value: Value that will be used for the update
 */
void sleepStats_updateValue(sleepStats_generic *stats, uint64 value);

/** 
 * sleepStats_updateSignedValue
 *  
 * @brief Updates the data fields for the generic SIGNED statistics structure
 *        
 * @param stats: Pointer to generic SIGNED data structure to update
 * @param value: Value that will be used for the update
 */
void sleepStats_updateSignedValue(sleepStats_generic_signed *stats,
                                  int64                     value);

/** 
 * sleepStats_updateValue32
 *  
 * @brief Updates the data fields for the generic statistics structure with a 32 bit value
 *        
 * @param stats: Pointer to generic data structure to update
 * @param value: Value that will be used for the update
 */
void sleepStats_updateValue32(sleepStats_generic32 *stats, uint32 value);

/** 
 * sleepStats_resetGeneric32
 *  
 * @brief Resets the statistic data
 *        
 * @param stats: Pointer to generic data structure to update
 */
void sleepStats_resetGeneric32(sleepStats_generic32 *stats);

#endif /*_SLEEP_STATS_GLOBAL_H_*/

