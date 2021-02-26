/** @file SataUtil.h
   
  XBL SATA driver header file
  
  Copyright (c) 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php
   
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   -----   ----------------------------------------------------------- 
2014/11/17   rm      Initial Version

=============================================================================*/

#ifndef _SATA_UTIL_H_
#define _SATA_UTIL_H_

/**                                                                 
  Get AHCI controller base address
          
  @param  ControllerNum             AHCI controller number.
  @param  Address                   The base address of the memory operations.                                  
                                    
  @retval EFI_SUCCESS               The valid address is returned.  
  @retval EFI_INVALID_PARAMETER     Controller number is out of range 
                                    or Address is NULL
                                   
**/
EFI_STATUS
GetAhciBaseAddress (
  IN  UINT32         ControllerNum,
  OUT UINTN         *Address
  );

/**                                                                 
  Initialize SATA PHY
          
  @retval EFI_SUCCESS               The valid address is returned.  
  @retval EFI_DEVICE_ERROR          Failed to Initialize PHY 
                                   
**/  
EFI_STATUS
SataPhyInitialization (VOID);

#endif /* _SATA_UTIL_H_ */














