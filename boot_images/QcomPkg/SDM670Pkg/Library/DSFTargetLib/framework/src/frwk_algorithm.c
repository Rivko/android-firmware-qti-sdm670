/*=============================================================================

                    ddrsns framework common api

GENERAL DESCRIPTION
  This file contains the main control functions for ddrsns framework

Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/09/16   tw     Initial creation
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "frwk_algorithm.h"
#include "frwk_training.h"
#include "frwk_helper.h"
#include "ddr_external.h"
#include "ddr_log.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

typedef struct
{
	uint8 n_result;
	uint8 m_result;
	uint8 max_n;
	uint8 min_n;
	uint8 max_m;
	uint8 min_m;
}common_histogram_result;

/*===========================================================================

**  Function :  common_histogram_process
** ==========================================================================
*/
/*!
* 
* @brief
*    Processes a given n x m 2d histogram and return the data eye center
* 
* @param[in] n, m			matrix n x m dimentions
* @param[in] procs          Pointer to the NxM matrix
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
void common_histogram_process
(
  uint32 n,
  uint32 m,
  common_histogram_result *result
)
{
}

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
)
{
  uint8 i, j;
  //uint8 height[n];
  uint8 count = 0;
  /* sanity check */
  if(input == NULL || n == 0 || m == 0)
  {
    return;
  }
  
  for(i = 0; i < n; i++)
  {
    for(j = 0; j < m; j++)
    {
      if(input[n][m])
        count++;
    }
  }
}

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
)

{}


