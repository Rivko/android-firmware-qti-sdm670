/** @file  RFSLibNull.c

  RFS library implements the functions to dynamically populate the DSDT with base
  address and size parameters for Remotefs and RFSA Shared buffers. But for
  targets where this feature is not enabled, this Null library function
  just returns.
  
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                EDIT HISTORY


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 07/22/14   vm      Created initial version.
 =============================================================================*/
 
#include <Uefi.h>

EFI_STATUS 
RFSLib_AMLVariableRegister (VOID)
{
  return EFI_SUCCESS;
}
