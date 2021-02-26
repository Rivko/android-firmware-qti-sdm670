/** @file QusbTargetLibNull.c
  
  Stub functions for QusbTargetLib

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/08/14   ck      Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"           // common defines - basic types as uint32 etc

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

// ===========================================================================
/**
 * @function    qusb_pbl_dload_check
 * 
 * @brief   This function will check if D+ is grounded. And if it is connected
 *          to ground, then we go into EDL.
 * 
 * @param   None
 * 
 * @return  TRUE: if it is connected to GND
 *          FALSE: if it is not connected to GND. 
 * 
 */
// ===========================================================================
boolean qusb_pbl_dload_check(void)
{
  return FALSE;
}
