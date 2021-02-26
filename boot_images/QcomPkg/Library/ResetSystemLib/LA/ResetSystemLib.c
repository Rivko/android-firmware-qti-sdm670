/** @file ResetSystemLib.c
   
  Library implementation to support ResetSystem Runtime call.

  Copyright (c) 2011-2018, Qualcomm Technologies, Inc. All rights reserved.
  Portions Copyright (c) 2008 - 2010, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.           
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/06/18   ps      platform specific disable fix
 10/18/16   bh      Fix KW errors
 07/19/16   vk      Fix check for reset data
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 10/14/14   na      Fix typecast mismatch for variables
 10/03/14   na      Fixing address types for 64-bit
 08/15/14   sm      Switched to new SCM API
 06/06/14   vk      Remove SPMI shutdown available check
 06/06/14   Moh     Add Mass storage fixes
 06/04/14   alal    Enable reset
 05/03/14   sv      Position of SpmiBus_init() call updated.(CR-672559)
 05/29/14   aus     Fix for warm reset when a capsule dump is performed.
 05/03/14   sv      initialising spmi during LibInitializeResetSystem. (CR-652148)
 03/14/14   aus     Allocate memory for the Wdog Sys call during initialization
 03/12/14   aus     Mass storage mode support 
 10/28/13   niting  Skip hard reset when doing an UpdateCapsule at runtime
 10/07/13   vk      Deadloop if SPMI shutdown fails
 10/01/13   vk      Shutdown on ASSERT
 09/25/13   vk      Shutdown SPMI if TZ supports it
 03/27/13   niting  Added offline crash dump support
 02/13/13   yg      Flush UART Log buffer before reset
 02/11/13   niting  Use UefiCfgLib for mass storage cookie address
 01/10/13   niting  Fixed reset issue.
 10/23/12   nk      Added support to clear MassStorage cookies when device resets
 08/01/12   niting  Added support for 8974.
 07/27/12   niting  Fixed page allocation size
 07/09/12   vishalo Remove Doad cookie handling
 07/07/12   rks     Abstract out watchdog code into UEFI lib
 06/15/12   niting  Call ExitBootServices in shutdown case
 05/08/12   yg      Disable variable sync from this module
 05/02/12   vishalo Set watchdog BITE register to known value 
 03/28/12   jz      Enable setting of Dload cookie in ExitBootServicesCallback()
 03/20/12   jz      Temporarily comment out setting Dload cookie in ExitBootServicesCallback()
 03/15/12   jz      Fix to clear Dload cookies if not in mass storage mode
 03/09/12   jz      Added support to set Dload cookies at exit boot services
 02/08/11   jdt     Added data cache flush at runtime to support UpdateCapsule.
 11/30/11   niting  Removed runtime flush.
 09/13/11   niting  Added delay before flush as workaround to runtime crash
                    during HLOS shutdown.
 08/31/11   niting  Added flushing of variable storage tables on
                    reset/shutdown.
 08/23/11   niting  Added shutdown support.
 07/07/11   niting  Cleanup.
 05/25/11   niting  Updates for Msm8960.
 01/14/11   niting  Initial revision.

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ArmLib.h>
#include <Library/PmicShutdown.h>
#include <Library/QcomLib.h>
#include <Library/SerialPortShLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/CookieLib.h>
#include <Library/SDILib.h>
 
#include <EfiResetSystemLib.h>
#include <Protocol/EFIHWIO.h>
#include <Protocol/EFIResetReason.h>
#include <Protocol/EFIPmicPwrOn.h>


UINTN    gPSHoldAddress = 0;

/**
  Shuts down EFI boot environment and frees up memory. 
   
**/
VOID
ShutdownEfi (
  VOID
  )
{
  EFI_STATUS              Status;
  UINTN                   MemoryMapSize = 0;
  EFI_MEMORY_DESCRIPTOR   *MemoryMap = NULL;
  UINTN                   MapKey = 0;
  UINTN                   DescriptorSize = 0;
  UINT32                  DescriptorVersion = 0;
  UINTN                   Pages;

  /* Contents of this function doesn't work in runtime */
  if (EfiAtRuntime() != 0)
    return;

  /* Wait until all the data from UART FIFO is drained */
  DEBUG((EFI_D_WARN, "\n"));

  do {
    Status = gBS->GetMemoryMap (
                    &MemoryMapSize,
                    MemoryMap,
                    &MapKey,
                    &DescriptorSize,
                    &DescriptorVersion
                    );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Pages = EFI_SIZE_TO_PAGES (MemoryMapSize) + 1;
      MemoryMap = AllocatePages (Pages);
      MemoryMapSize = EFI_PAGES_TO_SIZE(Pages);

      /*
       * Get System MemoryMap
       */
      Status = gBS->GetMemoryMap (
                      &MemoryMapSize,
                      MemoryMap,
                      &MapKey,
                      &DescriptorSize,
                      &DescriptorVersion
                      );
      /* Don't do anything between the GetMemoryMap() and ExitBootServices() */
      if (!EFI_ERROR (Status)) {
        Status = gBS->ExitBootServices (gImageHandle, MapKey);
        if (EFI_ERROR (Status)) {
          FreePages (MemoryMap, Pages);
          MemoryMap = NULL;
          MemoryMapSize = 0;
        }
      }
    }
  } while (EFI_ERROR (Status));
}


/**
  Updates NV Variables to keep track of the number of resets
  and shutdowns.

**/
EFI_STATUS
EFIAPI
UpdateNVVariables ( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN DataSize;
  UINT32 VarData;

  DataSize = sizeof(VarData);

  if (!EfiAtRuntime())
  {
    Status = gRT->GetVariable (L"BSPowerCycles", 
                               &gQcomTokenSpaceGuid, 
                               NULL,
                               &DataSize, 
                               &VarData);

    if (Status != EFI_SUCCESS)
      VarData = 0;

    VarData++;

    Status = gRT->SetVariable (L"BSPowerCycles", 
                               &gQcomTokenSpaceGuid, 
                               (EFI_VARIABLE_NON_VOLATILE | 
                                EFI_VARIABLE_BOOTSERVICE_ACCESS),
                               DataSize, 
                               &VarData);
  }
  
  return Status;
}


STATIC 
EFI_STATUS
ReadPmicRegisterForResetReason (UINT32 *ResetReason)
{
  #define PRIMARY_PMIC 0

  EFI_STATUS Status;
  EFI_QCOM_PMIC_PWRON_PROTOCOL *PmicPwrOnProtocol = NULL;
  UINT8 Value = 0;

  Status = gBS->LocateProtocol(&gQcomPmicPwrOnProtocolGuid, NULL, (void**)&PmicPwrOnProtocol);
  if ((EFI_SUCCESS != Status) || (PmicPwrOnProtocol == NULL))
  {
    DEBUG ((EFI_D_ERROR, "ERROR: Failed to locate PmicPwrOnProtocol: 0x%08x\n", Status));
    return EFI_UNSUPPORTED; 
  }

  Status = PmicPwrOnProtocol->GetSpareReg (PRIMARY_PMIC, EFI_PM_PON_SOFT_SPARE, &Value);
  if (EFI_SUCCESS != Status)
  {
    DEBUG ((EFI_D_ERROR, "ERROR: Failed to read reset reason: 0x%08x\n", Status));
    return EFI_DEVICE_ERROR;
  }

  /* Skip SPARE[0], use SPARE[7 6 5 4 3 2 1] */
  *ResetReason = 0x7F & (UINT32) (Value >> 1);

  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
WritePmicReg (UINT8 Value)
{
  EFI_STATUS Status;
  EFI_QCOM_PMIC_PWRON_PROTOCOL *PmicPwrOnProtocol = NULL;

  UINT8 Mask = 0xFE;
  UINT8 ShiftedVal = 0;
  
  Status = gBS->LocateProtocol(&gQcomPmicPwrOnProtocolGuid, NULL, (void**)&PmicPwrOnProtocol);
  if ((EFI_SUCCESS != Status) || (PmicPwrOnProtocol == NULL))
  {
    DEBUG ((EFI_D_ERROR, "ERROR: Failed to locate PmicPwrOnProtocol: 0x%08x\n", Status));
    return EFI_UNSUPPORTED; 
  }
  
  ShiftedVal = Value << 1;

  /* Skip SPARE[0], use SPARE[7 6 5 4 3 2 1] , Mask - 0xFE */
  Status = PmicPwrOnProtocol->SetSpareReg (PRIMARY_PMIC, EFI_PM_PON_SOFT_SPARE, ShiftedVal, Mask);
  if (EFI_SUCCESS != Status)
  {
    DEBUG ((EFI_D_ERROR, "ERROR: Failed to save reset reason: 0x%08x\n", Status));
    return EFI_DEVICE_ERROR;
  }
  
  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
UpdateResetReasonPmicRegister (VOID *ResetData, UINTN DataSize)
{
  EFI_STATUS Status;
  UINT8 Value = 0;

  if ((ResetData == NULL) || (DataSize == 0))
    return EFI_INVALID_PARAMETER;

  if (StrCmp (ResetData, STR_RESET_PARAM) == 0)
  {
    UINT8 *pResetVal = NULL;
    pResetVal = (UINT8*) (ResetData + StrSize (STR_RESET_PARAM));      
    Value = *pResetVal;
  }

  Status = WritePmicReg (Value);

  return Status;
}


STATIC
EFI_STATUS
ClearResetReasonPmicRegister (VOID)
{
  UINT8 Value = 0;    
  return WritePmicReg (Value);  
}


EFI_STATUS
EFIAPI
PlatformSpecificReset(UINTN DataSize, CHAR16 *ResetData)
{
  EFI_STATUS Status = EFI_SUCCESS;

  // Perform hard reset if no parameters provided
  if ((ResetData == NULL) || (DataSize == 0))
    return PmicReset(PM_PON_RESET_CFG_HARD_RESET);
  
  if (StrCmp(STR_RESET_PLAT_SPECIFIC_EDL, ResetData) == 0)
  {
    //Set EDL cookie and perform warm reset
    Status = ClearDLOADCookie();
    
    if (Status != EFI_SUCCESS)
    {
       DEBUG ((EFI_D_ERROR, "ERROR: ClearDLOADCookie Status: 0x%08x\n", Status));
    }
    
    Status = SetEDLCookie();
    
    if (Status != EFI_SUCCESS)
    {
       DEBUG ((EFI_D_ERROR, "ERROR: SetEDLCookie Status: 0x%08x\n", Status));
    }
    
    Status = SDIDisable();
    
    if (Status != EFI_SUCCESS)
    {
        DEBUG ((EFI_D_ERROR, "ERROR: Failed to SDIDisable reason: 0x%08x\n", Status));
    }
    
    Status = PmicReset(PM_PON_RESET_CFG_WARM_RESET);
    
    if (Status != EFI_SUCCESS)
    {
       DEBUG ((EFI_D_ERROR, "ERROR: PmicReset Status: 0x%08x\n", Status));
    }
  }
  return Status;
}    


/**
  Resets the entire platform.

  @param  ResetType             The type of reset to perform.
  @param  ResetStatus           The status code for the reset.
  @param  DataSize              The size, in bytes, of WatchdogData.
  @param  ResetData             For a ResetType of EfiResetCold, EfiResetWarm, or
                                EfiResetShutdown the data buffer starts with a Null-terminated
                                Unicode string, optionally followed by additional binary data.

**/
EFI_STATUS
EFIAPI
LibResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN CHAR16           *ResetData OPTIONAL
  )
{
  EFI_STATUS Status;

  /* Support for Boot service only */
  if (EfiAtRuntime())
  {
    CpuDeadLoop();
    return EFI_UNSUPPORTED;
  }

  /* Update any persistent uefi variables */
  UpdateNVVariables();

  DEBUG((EFI_D_WARN, "\nINFO: Received ResetSytem request.\n"));
  DEBUG((EFI_D_WARN, "INFO: Type     :0x%x \n", ResetType));
  DEBUG((EFI_D_WARN, "INFO: DataSize :0x%x \n", DataSize));
  if ( (DataSize != 0) && (ResetData != NULL))
  {
    UINTN StrSz = 0;
    StrSz = StrSize (ResetData);
    DEBUG((EFI_D_WARN, "INFO: String   :%s Length: 0x%x\n", ResetData, StrSz));

    /* At run time we do not update reset reason.
     * This should be done before callig ShutDownEfi()
     * Since we rely on gBS to update ResetReason, and it is 
     * cleared duing EFI shutdown
     * */
    Status = UpdateResetReasonPmicRegister(ResetData, DataSize);
    if (Status != EFI_SUCCESS)
    {
      DEBUG((EFI_D_WARN, "WARN: Unable to set reset reason %r !\n", Status));
    }
  }

  /* Setup PMIC and any cookies before de-asserting PS_HOLD */
  switch (ResetType) 
  {
    case EfiResetShutdown:
      Status = PmicReset(PM_PON_RESET_CFG_NORMAL_SHUTDOWN);
      break;

    case EfiResetWarm:
      Status = PmicReset(PM_PON_RESET_CFG_WARM_RESET);
      break;

    case EfiResetPlatformSpecific:
      Status = PlatformSpecificReset(DataSize, ResetData);
      break;

    case EfiResetCold:
    default:
      Status = PmicReset(PM_PON_RESET_CFG_HARD_RESET);
      break;
  }
  
  ASSERT(Status == EFI_SUCCESS);

  /* Call exit boot services */
  ShutdownEfi ();

  /* This register controls the PSHOLD value. 
     1: Asserted
     0: Deasserted */
  MmioWrite32( gPSHoldAddress, 0 );

  /* If reset, it should never get here, if we do loop ! */
  CpuDeadLoop();
  return EFI_DEVICE_ERROR;
}


STATIC
EFI_STATUS
ConvertToResetReasonStr (UINT32 *ResetReason, CHAR16 *ResetReasonStr, UINT32 *ResetReasonStrLen)
{
  #define MAX_RESET_STR_LEN 128

  EFI_STATUS Status = EFI_DEVICE_ERROR;
  UINT32 CurrentResetStrLen = 0;
  CHAR16 CurrentResetStr [MAX_RESET_STR_LEN];

  if ((ResetReason == NULL)       ||
      (ResetReasonStr == NULL)    ||
      (ResetReasonStrLen == NULL))
  {
    return EFI_INVALID_PARAMETER;	  
  }

  switch (*ResetReason)
  {
    case 0:
      CurrentResetStrLen = StrLen (STR_RESET_UNKNOWN);
      StrCpy (CurrentResetStr, STR_RESET_UNKNOWN);
      break;

    default:
      CurrentResetStrLen = StrLen (STR_RESET_PARAM);
      StrCpy (CurrentResetStr, STR_RESET_PARAM);
      break;
  }

  if (CurrentResetStrLen <= *ResetReasonStrLen)
  {
    StrCpy (ResetReasonStr, CurrentResetStr);
    Status = EFI_SUCCESS;
  }
  else
  {
    Status = EFI_BUFFER_TOO_SMALL;
  }

  if (ResetReasonStrLen)
    *ResetReasonStrLen = CurrentResetStrLen;

  return Status;
}


EFI_STATUS
LibGetResetReason (UINT32 *ResetReason, CHAR16 *ResetReasonStr, UINT32 *ResetReasonStrLen)
{
  EFI_STATUS Status;

  if (ResetReason == NULL)
    return EFI_INVALID_PARAMETER;	  

  Status = ReadPmicRegisterForResetReason (ResetReason);
  if (Status != EFI_SUCCESS)
    return EFI_UNSUPPORTED;

  if ((ResetReasonStr != NULL) && (ResetReasonStrLen != NULL))
    Status = ConvertToResetReasonStr (ResetReason, ResetReasonStr, ResetReasonStrLen);

  return Status;
}


EFI_STATUS
LibClearResetReason (VOID)
{
  EFI_STATUS Status;
  Status = ClearResetReasonPmicRegister ();
  if (Status != EFI_SUCCESS)
    DEBUG((EFI_D_ERROR, "ERROR: Unable to clear reset reason\n"));

  return Status;
}

/**
  Initialize any infrastructure required for LibResetSystem () to function.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
LibInitializeResetSystem (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_HWIO_PROTOCOL *HWIOProtocol = NULL;

  /* Setup PSHold */
  Status = gBS->LocateProtocol(&gEfiHwioProtocolGuid, NULL,
    (void**)&HWIOProtocol);
  if ((EFI_SUCCESS != Status) || (HWIOProtocol == NULL))
  {
    DEBUG ((EFI_D_ERROR, "ERROR: Failed to locate HWIO Protocol: 0x%08x\n", Status));
    return EFI_DEVICE_ERROR;
  }
  Status = HWIOProtocol->MapRegion(HWIOProtocol, "+PSHOLD", (UINT8**) &gPSHoldAddress);
  if (EFI_SUCCESS != Status)
  {
    DEBUG ((EFI_D_ERROR, "ERROR: Failed to map PSHOLD register address: 0x%08x\n", Status));
    return EFI_DEVICE_ERROR;
  }
  if (gPSHoldAddress == 0)
  {
    DEBUG ((EFI_D_ERROR, "ERROR: PSHOLD register address invalid!"));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


