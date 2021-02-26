/** @file SecImgAuthLibNull.c
  
  Stub functions for SecImgAuthLib

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      -----------------------------------------------------------
 11/24/14   wkimberl Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "secboot_hw.h"
#include "secboot.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


secboot_hw_etype secboot_hw_get_ftbl(secboot_hw_ftbl_type* ftbl_ptr)
{
  return E_SECBOOT_HW_SUCCESS;
}

secboot_hw_etype secboot_hw_get_serial_num
(
  uint32*  serial_num_ptr
)
{
  *serial_num_ptr = 0;
  return E_SECBOOT_HW_SUCCESS;
}

secboot_error_type secboot_get_ftbl(secboot_ftbl_type* ftbl_ptr)
{
  if (NULL == ftbl_ptr)
    return E_SECBOOT_INVALID_PARAM;

  return E_SECBOOT_SUCCESS;
}

