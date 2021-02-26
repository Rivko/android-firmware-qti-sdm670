/** @file Halsata.h
   
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

#ifndef _HAL_SATA_H_
#define _HAL_SATA_H_

#include <Uefi.h>

#define  SATA_CONTROLLER_NUMBER             8
#define  SATA_AHCI_MEMORY_SIZE              1024

/**                                                                 
  Get AHCI controller base address
          
  @param  ControllerNum             AHCI controller number.
  @retval Address                   The base address of the memory
                                    operations.                                                            
**/
UINTN
HAL_Sata_GetAhciBaseAddress(UINT32 ControllerNum);

/**                                                                 
  Follow HPG to initialize PHY                             
**/
VOID
HAL_Sata_InitializePhy(UINT8 SpNum);

/**                                                                 
  Return SP c_ready status
  
  @param  Sp                        Sp number.
  @retval Value                     c_ready value.
**/
UINT32
HAL_Sata_GetCReadyStatus(UINT8 SpNum);

/**                                                                 
  Set Port Implemented register (PI)
  Regname: PI
  
  @param  Sp                        Sp number.
  @retval Value                     c_ready value.
**/
VOID
HAL_Sata_SetPiRegister(UINT32 ControllerNum);

/**                                                                 
  Set Supports Staggered Spin-up (SSS)
  Regname: CAP
  
  @param  ControllerNum             Controller number.
  @param  Enable                    Enable or disable.
**/
VOID
HAL_Sata_Cap_Sss(UINT32 ControllerNum, BOOLEAN Enable);

/**                                                                 
  Set Supports Mechanical Presence Switch (SMPS)
  Regname: CAP
  
  @param  ControllerNum             Controller number.
  @param  Enable                    Enable or disable.
**/
VOID
HAL_Sata_Cap_Smps(UINT32 ControllerNum, BOOLEAN Enable);

/**                                                                 
  Set Hot Plug Capable Port (HPCP)
  Regname: PxCMD
  
  @param  ControllerNum             Controller number.
  @param  Enable                    Enable or disable.
**/
VOID
HAL_Sata_PxCmd_Hpcp(UINT32 ControllerNum, BOOLEAN Enable);

/**                                                                 
  Set Mechanical Presence Switch Attached to Port (MPSP)
  Regname: PxCMD
  
  @param  ControllerNum             Controller number.
  @param  Enable                    Enable or disable.
**/
VOID
HAL_Sata_PxCmd_Mpsp(UINT32 ControllerNum, BOOLEAN Enable);

/**                                                                 
  Set Cold Presence Detection (CPD)
  Regname: PxCMD
  
  @param  ControllerNum             Controller number.
  @param  Enable                    Enable or disable.
**/
VOID
HAL_Sata_PxCmd_Cpd(UINT32 ControllerNum, BOOLEAN Enable);

/**                                                                 
  Turn on clocks for SATA
**/
VOID
HAL_Sata_SetClocks();

/**                                                                 
  Set speed gen limit for SATA
**/
VOID
HAL_Sata_SetSpeedMode(UINT8 LNum, UINT8 Gen);

/**                                                                 
  Set SDFE bit in BISTAFR to force link stable
  Regname: BISTAFR
  
  @param  ControllerNum             Controller number.
  @param  Enable                    Enable or disable.
**/
VOID
HAL_Sata_SetSdfe(UINT32 ControllerNum);

/**                                                                 
  Leave it here temporarily
**/
VOID
Sata_Bsp_Clock_Set ();

/**                                                                 
  Vendor specific firmware initialization
          
  @param  ControllerNum             AHCI controller number.
                              
**/
VOID
Sata_Bsp_SetCaps(UINT32 ControllerNum);

#endif /* _HAL_SATA_H_ */