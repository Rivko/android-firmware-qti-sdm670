/** @file SataDxe.c
   
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
2014/11/04   rm      Initial Version

=============================================================================*/

#include "SataDxe.h"
#include "SataIdeInit.h"
#include "SataUtil.h" 
#include "SataPriv.h"

EFI_IDE_CONTROLLER_INIT_PROTOCOL gIdeControllerInit =
{
  IdeInitGetChannelInfo,                       /* GetChannelInfo */
  IdeInitNotifyPhase,                          /* NotifyPhase */
  IdeInitSubmitData,                           /* SubmitData */
  IdeInitDisqualifyMode,                       /* DisqualifyMode */
  IdeInitCalculateMode,                        /* CalculateMode */
  IdeInitSetTiming,                            /* SetTiming */
  FALSE,                                       /* EnumAll */
  0                                            /* ChannelCount */
};

static EFI_PCI_IO_DEVICE_PATH SataDevicePathTemplate = 
{                                              /* DevicePath */
  {                                            /* PciDevicePath */
    { 
      HARDWARE_DEVICE_PATH,                    /* Type */
      HW_PCI_DP,                               /* SubType */ 
      {                                       
        (UINT8)(sizeof (PCI_DEVICE_PATH)),     /* Length[0] */
        0                                      /* Length[1] */
      }                                   
    },                                      
    0,                                         /* Function */
    0                                          /* Device */
  },                                           
  {                                            /* EndDevicePath */
    END_DEVICE_PATH_TYPE, 
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8)(sizeof (EFI_DEVICE_PATH_PROTOCOL)), 
      0
    }  
  }
};

static EFI_PCI_IO_PROTOCOL PciIoTemplate =
{                                              /* PciIoProtocol */ 
  PciIoPollMem,                                /* PollMem */
  PciIoPollIo,                                 /* PollIo */
  {                                            /* Mem */
    PciIoMemRead,                              
    PciIoMemWrite                              
  },                                           
  {                                            /* Io */
    PciIoIoRead,                               
    PciIoIoWrite                               
  },                                           
  {                                            /* Pci */
    PciIoPciRead,                              
    PciIoPciWrite                              
  },                                           
  PciIoCopyMem,                                /* CopyMem */
  PciIoMap,                                    /* Map */
  PciIoUnmap,                                  /* Unmap */
  PciIoAllocateBuffer,                         /* AllocateBuffer */
  PciIoFreeBuffer,                             /* FreeBuffer */
  PciIoFlush,                                  /* Flush */
  PciIoGetLocation,                            /* GetLocation */
  PciIoAttributes,                             /* Attributes */
  PciIoGetBarAttributes,                       /* GetBarAttributes */
  PciIoSetBarAttributes,                       /* SetBarAttributes */
  0,                                           /* RomSize */
  0                                            /* RomImage */
};
  
EFI_SATA_CONTROLLER_DEVICE    SataController[SATA_CONTROLLER_NUMBER]; 

/**                                                                 
  SataDxe Entry Function                                   
**/  
EFI_STATUS
EFIAPI
SataDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  UINT32           Num = 0;
  UINTN            BaseAddress = 0;
  
  /* Initialize SATA PHY */
  Status = SataPhyInitialization ();
  if (Status != EFI_SUCCESS)
  {
      DEBUG ((EFI_D_ERROR, "Failed to initialize SATA PHY\n"));    
      //return Status;
  }
  
  for (Num = 0; Num < SATA_CONTROLLER_NUMBER; Num ++ )
  {
    CopyMem(&SataController[Num].DevicePath, 
            &SataDevicePathTemplate, sizeof(SataDevicePathTemplate));
            
    CopyMem(&SataController[Num].PciIoProtocol, 
            &PciIoTemplate, sizeof(PciIoTemplate));
                      
    /* Each device number must be unique among all PCI devices */                  
    SataController[Num].DevicePath.PciDevicePath.Device = 
                                  EFI_SATA_CONTROLLER_UNIQUE_MASK | Num;        
    
    SataController[Num].Signature   = EFI_SATA_CONTROLLER_DEVICE_SIGNATURE;
    SataController[Num].RootBridge.Signature   = PCI_ROOT_BRIDGE_SIGNATURE;
    
    
    // Create fake PCI space.
    SataController[Num].ConfigSpace = AllocateZeroPool(sizeof(PCI_TYPE00));
    if (SataController[Num].ConfigSpace == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }
    
    SataController[Num].ConfigSpace->Hdr.VendorId = QUALCOMM_VENDOR_ID;
    SataController[Num].ConfigSpace->Hdr.DeviceId = QUALCOMM_HAWKER_DEVICE_ID;
    SataController[Num].ConfigSpace->Hdr.ClassCode[0] = PCI_IF_MASS_STORAGE_AHCI;
    SataController[Num].ConfigSpace->Hdr.ClassCode[1] = PCI_CLASS_MASS_STORAGE_SATADPA;
    SataController[Num].ConfigSpace->Hdr.ClassCode[2] = PCI_CLASS_MASS_STORAGE;
    SataController[Num].Segment = 0; 
    
    //get AHCI base address
    Status = GetAhciBaseAddress(Num, &BaseAddress);
    if (Status != EFI_SUCCESS)
    {
        DEBUG ((EFI_D_ERROR, "Failed to get AHCI base address, does controller[%d] exist?\n",
                Num));    
        break;
    }
    
    SataController[Num].RootBridge.MemoryStart = BaseAddress;
    SataController[Num].RootBridge.MemorySize = SATA_AHCI_MEMORY_SIZE;
    SataController[Num].ConfigSpace->Device.Bar[EFI_AHCI_BAR_INDEX] = BaseAddress;
    
    Status = gBS->InstallMultipleProtocolInterfaces(
                  &SataController[Num].ControllerHandle,
                  &gEfiPciIoProtocolGuid,             &SataController[Num].PciIoProtocol,
                  &gEfiDevicePathProtocolGuid,        &SataController[Num].DevicePath,
                  &gEfiIdeControllerInitProtocolGuid, &gIdeControllerInit,
                  NULL);  
    
    if (Status != EFI_SUCCESS) break;
    
    DEBUG ((EFI_D_INFO, "InstallMultipleProtocolInterfaces Controller = %x\n", 
    SataController[Num].ControllerHandle));    
  }  
  
  return Status;
}
