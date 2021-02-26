#ifndef _SLEEPUTILS_H_
#define _SLEEPUTILS_H_
/*==============================================================================
  FILE:         sleep_utils.h
  
  OVERVIEW:     Provides sleep global general interest functions
 
  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/util/sleep_utils.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "DALStdDef.h"
#include "sleep.h"

/*=============================================================================
                        GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * safe_signed_addition
 *
 * @brief Adds two signed 64 bit numbers and if the total is greater than the
 *        maximum value 64 bit number can hold (or is less than minimum value)
 *        it clips to max (or min) value.
 *
 * @param a: First integer
 * @param b: Second integer
 *
 * @return Total of two input integers (with clipping if requried).
 */
int64 safe_signed_addition(int64 a, int64 b);

/**
 * safe_unsigned_addition
 *
 * @brief Adds two unsigned integers of 64 bit and if their total exceeds the
 *        maximum value 64 bits can hold, it will clip the result.
 *
 * @param a: First unsigned integer.
 * @param b: Second unsigned integer.
 *
 * @return Total of two input unsigned numbers (with clipping if required).
 */
uint64 safe_unsigned_addition(uint64 a, uint64 b);

/**
 * safe_signed_truncate
 *
 * @brief Converts the 64 bit integer to 32 bit integer and in case the input
 *        integer lies outside of range of 32 bit, it clips the number to
 *        the maximum or minimum limit.
 *
 * @param a: input 64 bit integer
 *
 * @return 32 bit integer equivalent of input number.
 */
int32 safe_signed_truncate(int64 a);

/**
 * safe_unsigned_truncate
 *
 * @brief Converts the 64 bit unsigned integer to 32 bit unsigned integer and in
 *        case the input integer is greater than the maximum value 32 bit can
 *        hold, it clips.
 *
 * @param a: input 64 bit unsigned integer.
 *
 * @return 32 bit unsigned integer equivalent of input number.
 */
uint32 safe_unsigned_truncate(uint64 a);

#endif /* _SLEEPUTILS_H_ */

