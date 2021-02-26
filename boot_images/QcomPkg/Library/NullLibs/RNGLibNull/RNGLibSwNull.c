/** @file RNGLibSwNull.c
  
  Stub functions for RNGLibSw

  Copyright (c) 2015, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/08/15   plc     Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "PrngML.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/**
 * @brief    This function returns random number.  
 *
 * @param random_ptr [in]Random number pointer
 * @param random_len [in]Length of random number 
 *
 * @return PrngML_Resut_Type
 *
 *
 */
PrngML_Result_Type PrngML_getdata_lite(uint8 * random_ptr,
                                       uint16 random_len)
{
  return PRNGML_ERROR_NONE;
}