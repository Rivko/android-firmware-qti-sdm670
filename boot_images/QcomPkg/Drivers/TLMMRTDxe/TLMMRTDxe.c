/** @file TLMMRTDxe.c
   
  This file implements TLMM EFI protocol interface for runtime
  (RT) UEFI environment. 

  Copyright (c) 2011-2015, Qualcomm Technologies, Inc.  All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 09/02/11   dcf     Created.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <PiDxe.h>
#include <Guid/EventGroup.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesTableLib.h>

#include <Protocol/EFITlmm.h>

#include "TLMMRTDriver.h"



/*=========================================================================
      Type Definitions
==========================================================================*/

/* Round off to 4KB pages */
#define ROUND_TO_PAGE(x) ((x) & 0xfffff000)

/*=========================================================================
      Function Prototypes
==========================================================================*/

VOID EFIAPI TLMMVirtualAddressChangeCallBack (IN EFI_EVENT Event,
                                               IN VOID      *Context);




/*=========================================================================
      Functions
==========================================================================*/


/**
  Virtual address change notification call back. It converts global pointer
  to virtual address.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
TLMMVirtualAddressChangeCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  TLMM_HWIOBaseType  *paHWIOBases = NULL;

  TLMM_GetHWIOBases(&paHWIOBases);
  while (paHWIOBases->pnBaseAddr != NULL)
  {
    gRT->ConvertPointer(0, (VOID**)paHWIOBases->pnBaseAddr);
    paHWIOBases++;
  }
}


EFI_STATUS
EFI_TLMM_ConfigGpioGroup(IN UINT32 enable, IN UINT32 *config_group, IN UINT32 size)
{
  if(TLMM_ConfigGpios((uint32*)config_group, (uint32)size))
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_TLMM_ConfigGpioGroup */



/*==============================================================================

  FUNCTION      EFI_TLMM_ConfigGpio

  DESCRIPTION   See EFI_TLMM_CONFIG_GPIO in Protocol/TLMM.h

==============================================================================*/

EFI_STATUS
EFI_TLMM_ConfigGpio(IN UINT32 config, IN UINT32 enable)
{
  if(EFI_TLMM_ConfigGpioGroup(1, &config, 1))
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_TLMM_ConfigGpio */





/*==============================================================================

  FUNCTION      EFI_TLMM_GpioIn

  DESCRIPTION   See EFI_TLMM_GPIO_IN in Protocol/TLMM.h

==============================================================================*/ 

EFI_STATUS
EFI_TLMM_GpioIn(IN UINT32 config, OUT UINT32 *value)
{
  if(TLMM_ReadGpio(config, value))
  {
      *value = 1;
  }
  else
  {
    *value = 0;
  }
  return(EFI_SUCCESS);


} /* EFI_TLMM_GpioIn */



/*==============================================================================

  FUNCTION      EFI_TLMM_GpioOut

  DESCRIPTION   See EFI_TLMM_GPIO_OUT in Protocol/TLMM.h

==============================================================================*/ 

EFI_STATUS
EFI_TLMM_GpioOut(IN UINT32 config, IN UINT32 value)
{
  if(TLMM_WriteGpio(config, value))
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_TLMM_GpioOut */



/*==============================================================================

  FUNCTION      EFI_TLMM_SetInactiveConfig

  DESCRIPTION   See EFI_TLMM_SET_INACTIVE_CONFIG in Protocol/TLMM.h

==============================================================================*/ 

EFI_STATUS
EFI_TLMM_SetInactiveConfig(IN UINT32 gpio_number, IN UINT32 config)
{
  return(EFI_UNSUPPORTED);

} /* EFI_TLMM_SetInactiveConfig */



/**
  TLMM DXE driver entry point. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/



/*=========================================================================
      Data Declarations
==========================================================================*/

// TLMM Public Protocol 
static EFI_TLMM_PROTOCOL TLMMRTProtocol =
{
   EFI_TLMM_RT_PROTOCOL_VERSION,
   EFI_TLMM_ConfigGpio,
   EFI_TLMM_ConfigGpioGroup,
   EFI_TLMM_GpioIn,
   EFI_TLMM_GpioOut,
   EFI_TLMM_SetInactiveConfig
};


EFI_EVENT TLMMVirtualAddressChangeEvent = NULL;


EFI_STATUS
EFIAPI
TLMMRTDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_HANDLE                       handle = NULL;
  BOOLEAN                          bResult;
  EFI_STATUS                       eStatus;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR  mHWIOBaseMemoryDesc;
  UINT32                           nOffset = 0;
  UINT32                           nStartAddress = 0;
  UINT32                           nSize = 0;
  TLMM_HWIOBaseType                *paHWIOBases = NULL;

  /* 
   * Initialize TLMM driver
   */ 
  bResult = TLMM_DriverInit();

  if(bResult == TRUE)
  {
    eStatus = gBS->CreateEventEx (
                     EVT_NOTIFY_SIGNAL,
                     TPL_CALLBACK,
                     TLMMVirtualAddressChangeCallBack,
                     NULL,
                     &gEfiEventVirtualAddressChangeGuid,
                     &TLMMVirtualAddressChangeEvent
                    );

    ASSERT_EFI_ERROR (eStatus);

    /*
     * Mark runtime memory for required HWIO bases
     */
    TLMM_GetHWIOBases(&paHWIOBases);
    while (paHWIOBases->pnBaseAddr != NULL)
    {
      /*
       * Map start address + physical size region - 4KB at a time.
       */
      nStartAddress = ROUND_TO_PAGE(paHWIOBases->nPhysAddr);
      nSize = paHWIOBases->nSize + paHWIOBases->nPhysAddr - nStartAddress;
      while(nOffset < nSize)
      {
        /*
         * Get the GCD Memory Descriptor specified by (nStartAddress + nOffset) page boundary
         */
        eStatus = gDS->GetMemorySpaceDescriptor ((nStartAddress + nOffset), 
                                                 &mHWIOBaseMemoryDesc);

        ASSERT_EFI_ERROR (eStatus);

        /*
         * Mark the 4KB region as EFI_RUNTIME_MEMORY so the OS will allocate
         * a virtual address range.
         */
        eStatus = gDS->SetMemorySpaceAttributes (
                      (nStartAddress + nOffset), 
                      EFI_PAGE_SIZE, 
                      mHWIOBaseMemoryDesc.Attributes | EFI_MEMORY_RUNTIME);

        ASSERT_EFI_ERROR (eStatus);

        nOffset += EFI_PAGE_SIZE;
      }

      paHWIOBases++;
    }

    // Publish the TLMM RT Protocol
    eStatus = gBS->InstallMultipleProtocolInterfaces( 
                     &handle, 
                     &gEfiTLMMRTProtocolGuid, 
                     (void **)&TLMMRTProtocol, NULL );
  }
  else
  {
    eStatus = EFI_PROTOCOL_ERROR;
  }

  ASSERT_EFI_ERROR (eStatus);

  return eStatus;
}





