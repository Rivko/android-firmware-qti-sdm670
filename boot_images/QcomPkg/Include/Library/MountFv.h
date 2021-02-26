/** @file MountFv.h
   
  Contains Qualcomm library functions for loading and mounting a Fv
  from partition.

  Copyright (c) 2017, Qualcomm Technologies, Inc. All rights reserved. 
  Qualcomm Technologies Proprietary and Confidential.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/08/17   kpa     Initial Revision

=============================================================================*/
#ifndef __MOUNTFV_H__
#define __MOUNTFV_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include "com_dtypes.h"



/**
  This function mounts a FV (firmware volume) passed as an argument.  
  It locates the partition containing the FV and loads the data. if needed it is
  decompressed and then mounted.

  @param[in]  FvName            String name of the FV to be mounted
  @param[out] None
  
  @retval  EFI_SUCCESS          The Firmware volume is found and successfully mounted.
  @retval  EFI_NOT_FOUND        Firmware volume not found.
  @retval  EFI_OUT_OF_RESOURCES There were not enough resources to allocate the 
                                output data buffer or complete the operations.
  @retval  EFI_ACCESS_DENIED    The firmware volume containing the searched 
                                firmware file is configured to disallow reads.
**/
EFI_STATUS 
MountFvPartition (
  IN  CONST CHAR8 *FvName
  );
  
#endif /* __MOUNTFV_H__ */
