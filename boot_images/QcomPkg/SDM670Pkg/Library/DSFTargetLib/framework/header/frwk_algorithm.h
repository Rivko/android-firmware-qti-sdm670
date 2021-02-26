#ifndef FRWK_ALGORITHM_H
#define FRWK_ALGORITHM_H

/*=============================================================================

                  DDRSNS V3 Framework Library

GENERAL DESCRIPTION
  DDRSNS V3 Framework library header

Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/27/16   tw      Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "HAL_SNS_DDR.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
struct rect_eye
{
  uint8 x;
  uint8 y;
  uint8 width;
  uint8 height;
};

union read_result
{
  uint64 per_word;
  uint8 per_byte[8];
  uint8 per_bit[64];
};

typedef enum
{
    READ_PER_WORD,
    READ_PER_BYTE,
    READ_PER_BIT,
}READ_COMPARE_TYPE;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  framework_rectangle_search

** ==========================================================================
*/
/*!
* 
* @brief
*    given a 2d matrix, find the largest rectangle that fits 
* 
* @param[in] input                      input 2d matrix size of n x m
* @param[in] n                          input size of n 
* @param[in] m                          input size of m
*
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void framework_rectangle_search
(
  uint8 n,
  uint8 m,
  uint8 (*input)[n][m],  
  struct rect_eye *result
);

/*===========================================================================

**  Function :  framework_read_compare

** ==========================================================================
*/
/*!
* 
* @brief
*    perform n number of read and compare against expected pattern
* 
* @param[in] dest           Pointer to destination address on ddr
* @param[in] src            Pointer to src pattern
* @param[in] size           size of destination pointer in bytes
* @param[in] iteration      number of reads to perform
* @param[in\out] result     Pointer to the result 
*
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void framework_read_compare
(
  uint32 *dest,
  uint32 *src,
  uint32 size,
  uint32 iteration,
  union read_result *result
);

#endif  /* FRWK_ALGORITHM_H */

