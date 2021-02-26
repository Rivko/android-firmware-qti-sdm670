/** @file SataUtil.c
   
  XBL SATA driver code
  
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

#include "SataPriv.h"
#include "SataUtil.h"

#define SPEED_GEN1   0
#define SPEED_GEN2   1

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
  )
{
  if(Address == NULL || ControllerNum >= SATA_CONTROLLER_NUMBER)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  *Address = HAL_Sata_GetAhciBaseAddress(ControllerNum);

  return EFI_SUCCESS;
}

/**                                                                 
  Initialize SATA PHY
          
  @retval EFI_SUCCESS               The valid address is returned.  
  @retval EFI_DEVICE_ERROR          Failed to Initialize PHY 
                                   
**/
EFI_STATUS
SataPhyInitialization (VOID)
{
  UINT32 Count = 0;
  UINT32 Sp = 0;
  UINT32 Ln = 0;
  UINT32 SpeedGen = PcdGet32(SATASpeedGen);
    
  //TODO: For real HW, turn on clock
  HAL_Sata_SetClocks();

  /* Set speed mode */
  if (SpeedGen == SPEED_GEN1 || SpeedGen == SPEED_GEN2)
  {
    for(Ln = 6; Ln < SATA_CONTROLLER_NUMBER; Ln++)
    {
      HAL_Sata_SetSpeedMode(Ln, SpeedGen);
    }
  }
  
  /* Initialize SP 0 with 4 lanes */
  HAL_Sata_InitializePhy(Sp);
  // poll qserver_com_c_ready_status bit[0]
  Count = SATA_PHY_POLL_MAX;
  while(Count -= 100)
  {
    if (HAL_Sata_GetCReadyStatus(Sp)) break;
    mdelay(100);
  }
  //if(Count == 0)
    //return EFI_DEVICE_ERROR;

  /* Initialize SP 1 with 4 lanes */
  Sp = 1;
  HAL_Sata_InitializePhy(Sp);
  // poll qserver_com_c_ready_status bit[0]
  Count = SATA_PHY_POLL_MAX;
  while(Count -= 100)
  {
    if (HAL_Sata_GetCReadyStatus(Sp)) break;
    mdelay(100);
  }
  
  /* Set vendor specific capabilities */
  for(Ln = 0; Ln < SATA_CONTROLLER_NUMBER; Ln++)
  {
    Sata_Bsp_SetCaps(Ln);
  }
  //if(Count == 0)
    //return EFI_DEVICE_ERROR;    
    
  //Disable lane that not used
  
  return EFI_SUCCESS;
}