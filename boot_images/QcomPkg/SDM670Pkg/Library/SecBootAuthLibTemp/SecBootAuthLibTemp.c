/** @file SecImgAuthLibTemp.c
  
  Stub functions for SecImgAuthLib

  Copyright (c) 2014-2015, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/SecBootAuthLibTemp/SecBootAuthLibTemp.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

 when       who      what, where, why
 --------   ---      -----------------------------------------------------------
 08/07/15   plc      Add secboot_hw_is_auth_enabled() for pre-si boot
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
secboot_hw_etype secboot_hw_is_auth_enabled
(
  uint32  code_segment,
  uint32* auth_enabled_ptr
)
{
  secboot_hw_etype ret_val = E_SECBOOT_HW_SUCCESS;
  (*auth_enabled_ptr) = FALSE;

  return ret_val;
}

secboot_hw_etype secboot_hw_get_ftbl(secboot_hw_ftbl_type* ftbl_ptr)
{
  ftbl_ptr->secboot_hw_is_auth_enabled = secboot_hw_is_auth_enabled;
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

