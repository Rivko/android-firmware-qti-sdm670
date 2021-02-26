/** @file

Copyright (c) 2010 - 2014 Qualcomm Technologies, Inc.	All Rights Reserved.
Portions Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


**/

/*=============================================================================
                              EDIT HISTORY

 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 09/03/15   ck       Add fields in EhciPciEmuMapping to indicate the index and 
                     whether the mapping can be released.
 07/07/15   ck       Add workaround for Hawker to support multiple 4 ports
 05/26/15   ck       Enable DDR support
 04/27/15   ck       Add Init Hawker support
 10/18/14   jb       Fix brace issues
 04/30/14   amitg    Updates Assert with USB Assert Macro
 08/03/12   amitg    Updated as tip of  main/latest
 03/30/11   sniedzie Integrated.
 08/11/11   sbryksin Changes for 8960v2 w/ HSIC
=============================================================================*/

#include "EhciPciEmulation.h"
#include <Library/UncachedMemoryAllocationLib.h>
#include <Protocol/EFIUsbConfig.h>
#include <Library/UefiCfgLib.h>
#include "SMMULib.h"

// Definitions from MdeModulePkg/Bus/Pci/EhciDxe/EhciReg.h:
#define EHC_HCSPARAMS_OFFSET    0x04    // Structural Parameters 04-07h
#define HCSP_NPORTS             0x0F    // Number of root hub port
#define PORTSC_POWER            0x1000  // Port Power
#define EHC_PORT_STAT_OFFSET    0x44    // Port status/control offset. This is
                                        // the last set of HC operational
                                        // registers.


// The only granularity supported by SMMU
#define EHCIPCIEMUSMMU_SIZE_1GB      0x40000000
// UEFI setting in Hawker needs to guarantee:
// 1. All of the buffer created in the UEFI are from certain memory regions.
// 2. The size of all the memory regions combined is less 4GB
// 3. All of the memory region is 1GB aligned.
#define EHCIPCIEMU_ADDR_MASK         0x3FFFFFFF
#define EHCIPCIEMU_SMMU_MAX_MAPPING  4
#define EHCIPCIEMU_TRANS_0G          0x00000000
#define EHCIPCIEMU_TRANS_1G          0x40000000
#define EHCIPCIEMU_TRANS_2G          0x80000000
#define EHCIPCIEMU_TRANS_3G          0xC0000000

#define HAL_HSUSB_CAPLENGTH_ADDR     0x00000100

typedef struct _EhciPciEmuMapping {
  UINT32           Id;    // Id
  BOOLEAN          Fixed; // TRUE if mapping should not be released.
  PHYSICAL_ADDRESS Start; // The start of the physical mem to be mapped
  BOOLEAN          Used;  // TRUE if the mapping is in use
  const UINT32     Trans; // The start of the virtual mem mapped to.
} EhciPciEmuMapping;

// all 4 usb instances share the same virtual address space.
// Thus, only one global bookkeeping array is needed.
EhciPciEmuMapping gSmmuMap[EHCIPCIEMU_SMMU_MAX_MAPPING] = {
  {0,TRUE,0,FALSE,EHCIPCIEMU_TRANS_0G},
  {1,FALSE,0,FALSE,EHCIPCIEMU_TRANS_1G},
  {2,FALSE,0,FALSE,EHCIPCIEMU_TRANS_2G},
  {3,FALSE,0,FALSE,EHCIPCIEMU_TRANS_3G}
};

typedef struct {
  PCI_DEVICE_PATH           PciDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  EndDevicePath;
} EFI_PCI_IO_DEVICE_PATH;

typedef struct {
  UINT32                  Signature;
  EFI_PCI_IO_DEVICE_PATH  DevicePath;
  EFI_PCI_IO_PROTOCOL     PciIoProtocol;
  PCI_TYPE00              *ConfigSpace;
  PCI_ROOT_BRIDGE         RootBridge;
  UINTN                   Segment;
  UINT8                   MaxCores;
} EFI_PCI_IO_PRIVATE_DATA;

#define EFI_PCI_IO_PRIVATE_DATA_SIGNATURE     SIGNATURE_32('u', 's', 'b', 'h')
#define EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(a)  CR(a, EFI_PCI_IO_PRIVATE_DATA, PciIoProtocol, EFI_PCI_IO_PRIVATE_DATA_SIGNATURE)

EFI_PCI_IO_DEVICE_PATH PciIoDevicePathTemplate =
{
  {
    { HARDWARE_DEVICE_PATH, HW_PCI_DP, {sizeof (PCI_DEVICE_PATH), 0}},
    0,
    0
  },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, {sizeof (EFI_DEVICE_PATH_PROTOCOL), 0}}
};


EFI_DRIVER_BINDING_PROTOCOL
gEhciPciEmulationDriverBinding = {
  EhcPciEmulationDriverBindingSupported,
  EhcPciEmulationDriverBindingStart,
  EhcPciEmulationDriverBindingStop,
  0x30,
  NULL,
  NULL
};


EFI_STATUS
PciIoPollMem (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN  UINT8                        BarIndex,
  IN  UINT64                       Offset,
  IN  UINT64                       Mask,
  IN  UINT64                       Value,
  IN  UINT64                       Delay,
  OUT UINT64                       *Result
  )
{
  USB_ASSERT_RETURN (FALSE, EFI_UNSUPPORTED);
}

EFI_STATUS
PciIoPollIo (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN  UINT8                        BarIndex,
  IN  UINT64                       Offset,
  IN  UINT64                       Mask,
  IN  UINT64                       Value,
  IN  UINT64                       Delay,
  OUT UINT64                       *Result
  )
{
  USB_ASSERT_RETURN (FALSE, EFI_UNSUPPORTED);
}

EFI_STATUS
PciIoMemRead (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);

  return PciRootBridgeIoMemRead (&Private->RootBridge.Io,
                                (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) Width,
                                Private->ConfigSpace->Device.Bar[BarIndex] + Offset,
                                Count,
                                Buffer
                                );
}

EFI_STATUS
PciIoMemWrite (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);

  return PciRootBridgeIoMemWrite (&Private->RootBridge.Io,
                                 (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) Width,
                                 Private->ConfigSpace->Device.Bar[BarIndex] + Offset,
                                 Count,
                                 Buffer
                                 );
}

EFI_STATUS
PciIoIoRead (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  USB_ASSERT_RETURN (FALSE, EFI_UNSUPPORTED);
}

EFI_STATUS
PciIoIoWrite (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  USB_ASSERT_RETURN (FALSE, EFI_UNSUPPORTED);
}

EFI_STATUS
PciIoPciRead (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT32                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);

  return PciRootBridgeIoMemRW ((EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                               Count,
                               TRUE,
                               (PTR)(UINTN)Buffer,
                               TRUE,
                               (PTR)(UINTN)(((UINT8 *)Private->ConfigSpace) + Offset)
                              );
}

EFI_STATUS
PciIoPciWrite (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT32                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);

  return PciRootBridgeIoMemRW ((EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) Width,
                               Count,
                               TRUE,
                               (PTR)(UINTN)(((UINT8 *)Private->ConfigSpace) + Offset),
                               TRUE,
                               (PTR)(UINTN)Buffer
                               );
}

EFI_STATUS
PciIoCopyMem (
  IN     EFI_PCI_IO_PROTOCOL         *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        DestBarIndex,
  IN     UINT64                       DestOffset,
  IN     UINT8                        SrcBarIndex,
  IN     UINT64                       SrcOffset,
  IN     UINTN                        Count
  )
{
  USB_ASSERT_RETURN (FALSE, EFI_UNSUPPORTED);
}

EFI_STATUS
PciIoMap (
  IN EFI_PCI_IO_PROTOCOL                *This,
  IN     EFI_PCI_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                           *HostAddress,
  IN OUT UINTN                          *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS           *DeviceAddress,
  OUT    VOID                           **Mapping
  )
{
  EFI_STATUS Status;
  UINT8      Index;
  UINTN      Which1G;
  BOOLEAN    Mapped = FALSE;

  Which1G = ((PHYSICAL_ADDRESS) HostAddress) & (~EHCIPCIEMU_ADDR_MASK);
  
  // Due to the SMMU implementation, there are at most EHCIPCIEMU_SMMU_MAX_MAPPING
  // mappings supported, and each of them has the granularity of 1 GB.
  // gSmmuMap is the global mapping bookkeeping data structures. A new mapping 
  // is only created if the memory region has not yet been mapped and we still 
  // have an unused mapping slot.
  for (Index = 0; Index < EHCIPCIEMU_SMMU_MAX_MAPPING; Index++) {
    // check whether the memory space has been mapped already
    if (gSmmuMap[Index].Used && gSmmuMap[Index].Start == Which1G) {
      Mapped = TRUE;
      DEBUG((EFI_D_VERBOSE,
        "PciIoMap: SMMU_Map: Index %d, Start 0x%llx, Used %d, Trans 0x%x\r\n",
        Index, gSmmuMap[Index].Start, gSmmuMap[Index].Used, gSmmuMap[Index].Trans
        ));
      break;
    }
  }
  
  // if the memory region is not already mapped, find a free virtual 
  // memory region to mapped it
  if (!Mapped) {
    for (Index = 0; Index < EHCIPCIEMU_SMMU_MAX_MAPPING; Index++) {
      if (!gSmmuMap[Index].Used) {
        SMMUError_t MStatus;
        SMMUMapAttr_t MapAttr;
        SMMUClientID_t Client = SMMU_CLIENT_USB0; // The configuration is set
                                                  // in the way that SMMU_CLIENT_USB0 ~
                                                  // SMMU_CLIENT_USB3 share the same
                                                  // virtual addr space. Just use
                                                  // SMMU_CLIENT_USB0.

        gSmmuMap[Index].Start = Which1G;

        // Create Mapping
        MapAttr.size = EHCIPCIEMUSMMU_SIZE_1GB;
        MapAttr.inputAddr = gSmmuMap[Index].Trans;    // device addr to be mapped
        MapAttr.outputAddr = gSmmuMap[Index].Start;   // the base of the region to be mapped to, and it would encompass base + size-1
        MapAttr.perm = SMMU_PERM_RW;
        MapAttr.shareAttr = SMMU_SHARE_NONE;          // suggested by SMMU team. require verification.
        MapAttr.cacheAttr = SMMU_CACHE_OUTER_NC_INNER_NC; //suggested by SMMU team. require verification

        MStatus = SMMU_Map(Client, &MapAttr);
        if (MStatus != SMMU_ESUCCESS) {
          Status = EFI_DEVICE_ERROR;
          while(TRUE){
            DEBUG((EFI_D_INFO,
              "PciIoMap: SMMU_Map failed %d\r\n", MStatus));
          }
          goto ON_EXIT;
        }
        gSmmuMap[Index].Used = TRUE;
        Mapped = TRUE;
        break;
      }
    }
  }

  if (!Mapped) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG((EFI_D_ERROR, "PciIoMap: Unable to map the client buffer\r\n"));
    ASSERT(Mapped);
    goto ON_EXIT;
  }

  *DeviceAddress = ((PHYSICAL_ADDRESS) HostAddress) - gSmmuMap[Index].Start + gSmmuMap[Index].Trans;
  Status = EFI_SUCCESS;
  *Mapping = &gSmmuMap[Index];

ON_EXIT:
  return Status;
}

EFI_STATUS
PciIoUnmap (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  VOID                         *Mapping
  )
{
  EFI_STATUS        Status = EFI_SUCCESS;
  EhciPciEmuMapping *SmmuMap = NULL;
  SMMUClientID_t    Client = SMMU_CLIENT_USB0;
  SMMUError_t       MStatus;

  if (Mapping == NULL) {
    goto ON_EXIT;
  }

  SmmuMap = (EhciPciEmuMapping *)Mapping;
  
  // if the map's fixed attribute is not set, unmap it.
  if (!SmmuMap->Fixed) {    
    MStatus = SMMU_Unmap(Client, SmmuMap->Trans, EHCIPCIEMUSMMU_SIZE_1GB);

    if (MStatus != SMMU_ESUCCESS) {
      Status = EFI_DEVICE_ERROR;
      DEBUG((EFI_D_ERROR, "PciIoUnMap: failed to unmap %d\r\n", SmmuMap->Id));
      goto ON_EXIT;
    }
    SmmuMap->Used = FALSE;
  }

ON_EXIT:
  return Status;
}

EFI_STATUS
PciIoAllocateBuffer (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  EFI_ALLOCATE_TYPE            Type,
  IN  EFI_MEMORY_TYPE              MemoryType,
  IN  UINTN                        Pages,
  OUT VOID                         **HostAddress,
  IN  UINT64                       Attributes
  )
{
  EFI_STATUS               Status = EFI_SUCCESS;
  PHYSICAL_ADDRESS         Address;
  MemRegionInfo            MemoryRegion;
  UINTN                    DeviceNumber;
  UINT64                   MemSize;
  EFI_PCI_IO_PRIVATE_DATA *Private;
   
  if (HostAddress == NULL || This == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);

  if (Private == NULL) {
    return EFI_DEVICE_ERROR;
  }

  DeviceNumber = Private->DevicePath.PciDevicePath.Device; 

  // CK-DF Remove after SMMU is working
  Status  = GetMemRegionInfoByName("USB RT DDR", &MemoryRegion);
  Address = MemoryRegion.MemBase;
  MemSize = MemoryRegion.MemSize / Private->MaxCores;

  if (EFI_SUCCESS == Status) {
    *HostAddress = (VOID *)Address + DeviceNumber * MemSize;
  }
  
  return Status;
}


EFI_STATUS
PciIoFreeBuffer (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  UINTN                        Pages,
  IN  VOID                         *HostAddress
  )
{
  // CK-DF Remove after SMMU is working
  /*if (HostAddress == NULL) {
     return EFI_INVALID_PARAMETER;
  }

  FreePages (HostAddress, Pages);*/
  return EFI_SUCCESS;
}


EFI_STATUS
PciIoFlush (
  IN EFI_PCI_IO_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
PciIoGetLocation (
  IN EFI_PCI_IO_PROTOCOL          *This,
  OUT UINTN                       *SegmentNumber,
  OUT UINTN                       *BusNumber,
  OUT UINTN                       *DeviceNumber,
  OUT UINTN                       *FunctionNumber
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);

  if (SegmentNumber != NULL) {
    *SegmentNumber = Private->Segment;
  }

  if (BusNumber != NULL) {
    *BusNumber = 0xff;
  }

  if (DeviceNumber != NULL) {
    *DeviceNumber = Private->DevicePath.PciDevicePath.Device;
  }

  if (FunctionNumber != NULL) {
    *FunctionNumber = 0;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciIoAttributes (
  IN EFI_PCI_IO_PROTOCOL                       *This,
  IN  EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION  Operation,
  IN  UINT64                                   Attributes,
  OUT UINT64                                   *Result OPTIONAL
  )
{
  switch (Operation) {
  case EfiPciIoAttributeOperationGet:
  case EfiPciIoAttributeOperationSupported:
    if (Result == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    // We are not a real PCI device so just say things we kind of do
    *Result = EFI_PCI_IO_ATTRIBUTE_MEMORY | EFI_PCI_DEVICE_ENABLE;
    break;

  case EfiPciIoAttributeOperationSet:
  case EfiPciIoAttributeOperationEnable:
  case EfiPciIoAttributeOperationDisable:
    // Since we are not a real PCI device no enable/set or disable operations exist.
    return EFI_SUCCESS;

  default:
  USB_ASSERT_RETURN (FALSE, EFI_INVALID_PARAMETER);
  };
  return EFI_SUCCESS;
}

EFI_STATUS
PciIoGetBarAttributes (
  IN EFI_PCI_IO_PROTOCOL             *This,
  IN  UINT8                          BarIndex,
  OUT UINT64                         *Supports, OPTIONAL
  OUT VOID                           **Resources OPTIONAL
  )
{
  USB_ASSERT_RETURN (FALSE, EFI_UNSUPPORTED);
}

EFI_STATUS
PciIoSetBarAttributes (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     UINT64                       Attributes,
  IN     UINT8                        BarIndex,
  IN OUT UINT64                       *Offset,
  IN OUT UINT64                       *Length
  )
{
  USB_ASSERT_RETURN (FALSE, EFI_UNSUPPORTED);
}


EFI_PCI_IO_PROTOCOL PciIoTemplate =
{
  PciIoPollMem,
  PciIoPollIo,
  {
    PciIoMemRead,
    PciIoMemWrite
  },
  {
    PciIoIoRead,
    PciIoIoWrite
  },
  {
    PciIoPciRead,
    PciIoPciWrite
  },
  PciIoCopyMem,
  PciIoMap,
  PciIoUnmap,
  PciIoAllocateBuffer,
  PciIoFreeBuffer,
  PciIoFlush,
  PciIoGetLocation,
  PciIoAttributes,
  PciIoGetBarAttributes,
  PciIoSetBarAttributes,
  0,
  0
};




EFI_STATUS
EFIAPI
EhciPciEmulationEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gEhciPciEmulationDriverBinding,
           ImageHandle,
           NULL,
           NULL
           );
}


EFI_STATUS
EFIAPI
EhcPciEmulationDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  QCOM_USB_CONFIG_PROTOCOL *pUsbConfigProtocol;
  EFI_STATUS              Status;

  // Check if UsbConfig protocol is installed
  Status = gBS->OpenProtocol (
                  Controller,
                  &gQcomUsbConfigProtocolGuid,
                  (VOID **) &pUsbConfigProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //check if controller is for a valid core that supports EHCI
  if ( (pUsbConfigProtocol->coreNum < USB_CORE_MAX_NUM) && (pUsbConfigProtocol->modeType == USB_HOST_MODE_EHCI) )
  {
    DEBUG(( EFI_D_INFO, "EhcPciEmulationDriverBindingSupported: This is core#%d, CoreType = (0x%x)\r\n",
            pUsbConfigProtocol->coreNum, pUsbConfigProtocol->modeType ));
    Status = EFI_SUCCESS;
  }
  else
  {
    Status = EFI_UNSUPPORTED;
  }

  // Close the USB_CONFIG used to perform the supported test
  gBS->CloseProtocol (
         Controller,
         &gQcomUsbConfigProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}


/**
  Starting the Usb EHCI Driver.

  @param  This                 Protocol instance pointer.
  @param  Controller           Handle of device to test.
  @param  RemainingDevicePath  Not used.

  @return EFI_SUCCESS          supports this device.
  @return EFI_UNSUPPORTED      do not support this device.
  @return EFI_DEVICE_ERROR     cannot be started due to device Error.
  @return EFI_OUT_OF_RESOURCES cannot allocate resources.

**/
EFI_STATUS
EFIAPI
EhcPciEmulationDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  QCOM_USB_CONFIG_PROTOCOL *pUsbConfigProtocol;
  QCOM_USB_CONFIG_CORE_TYPE coreType;
  EFI_STATUS              Status;
  EFI_PCI_IO_PRIVATE_DATA *Private;
  UINT8                   CapabilityLength;
  UINT8                   PhysicalPorts;
  UINTN                   Count;
  UINTN                   CoreBaseAddr;
  UINT8                   MaxCores;
  EFI_PCI_IO_DEVICE_PATH     PciIoDevicePathTemplate =
  {
    {
      { HARDWARE_DEVICE_PATH, HW_PCI_DP, {sizeof (PCI_DEVICE_PATH), 0}},
      0,
      0
    },
    { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, {sizeof (EFI_DEVICE_PATH_PROTOCOL), 0}}
  };

  DEBUG(( EFI_D_INFO, "EhcPciEmulationDriverBindingStart++\r\n"));

  SMMU_Init();

  //Open the Plat Config protocol.
  Status = gBS->OpenProtocol (
                  Controller,
                  &gQcomUsbConfigProtocolGuid,
                  (VOID **) &pUsbConfigProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    DEBUG(( EFI_D_ERROR,
            "EhcPciEmulationDriverBindingStart: Unable to open USB Config Protocol, Status =  (0x%x)\r\n", Status));
    return Status;
  }

  DEBUG(( EFI_D_INFO, "EhcPciEmulationDriverBindingStart: This is core#%d, CoreType = (0x%x)\r\n",
          pUsbConfigProtocol->coreNum, pUsbConfigProtocol->modeType ));

  // Configure USB core
  Status = pUsbConfigProtocol->ConfigUsb (pUsbConfigProtocol, USB_HOST_MODE_EHCI, pUsbConfigProtocol->coreNum);

  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_ERROR,  "EhcPciEmulationDriverBindingStart: Unable to configure USB in host mode, Status =  (0x%x)\r\n", Status));
    return Status;
  }

  // Create a private structure
  Private = AllocatePool(sizeof(EFI_PCI_IO_PRIVATE_DATA));
  if (Private == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    return Status;
  }

  Private->Signature              = EFI_PCI_IO_PRIVATE_DATA_SIGNATURE;  // Fill in signature
  Private->RootBridge.Signature   = PCI_ROOT_BRIDGE_SIGNATURE;          // Fake Root Bridge structure needs a signature too

  Status = pUsbConfigProtocol->GetUsbHostConfig(pUsbConfigProtocol, USB_HOST_MODE_EHCI, pUsbConfigProtocol->coreNum, &coreType);

  if (EFI_ERROR (Status)) {
    DEBUG(( EFI_D_ERROR,
            "EhcPciEmulationDriverBindingStart: Unable to get core type, Status =  (0x%x)\r\n", Status));
    return Status;
  }

  Status = pUsbConfigProtocol->GetCoreBaseAddr (pUsbConfigProtocol, coreType, &CoreBaseAddr);
  if (EFI_ERROR (Status)) {
    DEBUG(( EFI_D_ERROR,
            "EhcPciEmulationDriverBindingStart: Unable to get core base address, Status =  (0x%x)\r\n", Status));
    return Status;
  }

  Private->RootBridge.MemoryStart = (UINT32) CoreBaseAddr + HAL_HSUSB_CAPLENGTH_ADDR;

  Private->Segment = 0;                                  // Default to segment zero

  // Find out the capability register length and number of physical ports.
  CapabilityLength = MmioRead8(Private->RootBridge.MemoryStart);
  PhysicalPorts    = (MmioRead32 (Private->RootBridge.MemoryStart + EHC_HCSPARAMS_OFFSET)) & HCSP_NPORTS;

  // Calculate the total size of the USB registers.
  Private->RootBridge.MemorySize = CapabilityLength + (EHC_PORT_STAT_OFFSET + ((4 * PhysicalPorts) - 1));

  // Enable Port Power bit in Port status and control registers in EHCI register space.
  // Port Power Control (PPC) bit in the HCSPARAMS register is already set which indicates
  // host controller implementation includes port power control.
  for (Count = 0; Count < PhysicalPorts; Count++) {
    MmioOr32 ((Private->RootBridge.MemoryStart + CapabilityLength + EHC_PORT_STAT_OFFSET + 4*Count), PORTSC_POWER);
  }

  // Create fake PCI config space.
  Private->ConfigSpace = AllocateZeroPool(sizeof(PCI_TYPE00));
  if (Private->ConfigSpace == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    FreePool(Private);
    return Status;
  }

  // Configure PCI config space
  Private->ConfigSpace->Hdr.VendorId = QUALCOMM_VENDOR_ID;
  Private->ConfigSpace->Hdr.DeviceId = QUALCOMM_EHCI_DEVICE_ID;
  Private->ConfigSpace->Hdr.ClassCode[0] = PCI_IF_EHCI;
  Private->ConfigSpace->Hdr.ClassCode[1] = PCI_CLASS_SERIAL_USB;
  Private->ConfigSpace->Hdr.ClassCode[2] = PCI_CLASS_SERIAL;
  Private->ConfigSpace->Device.Bar[0] = Private->RootBridge.MemoryStart;

  // Find the max number of EHCI core supported
  Status = pUsbConfigProtocol->GetUsbMaxHostCoreNum(pUsbConfigProtocol, pUsbConfigProtocol->modeType, &MaxCores);
  if (EFI_ERROR(Status) || MaxCores == 0) {
    Private->MaxCores = 1;
  } else {
    Private->MaxCores = MaxCores;
  }

  // Unique device number
  PciIoDevicePathTemplate.PciDevicePath.Device = pUsbConfigProtocol->coreNum;

  // Unique device path.
  CopyMem(&Private->DevicePath, &PciIoDevicePathTemplate, sizeof(PciIoDevicePathTemplate));

  // Copy protocol structure
  CopyMem(&Private->PciIoProtocol, &PciIoTemplate, sizeof(PciIoTemplate));

  Status = gBS->InstallMultipleProtocolInterfaces(&Controller,
                                                  &gEfiPciIoProtocolGuid,       &Private->PciIoProtocol,
                                                  &gEfiDevicePathProtocolGuid,  &Private->DevicePath,
                                                  NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "EhciPciEmulationDriverBindingStart: InstallMultipleProtocolInterfaces failed. Status = 0x%x\n", Status));
  }

  DEBUG((EFI_D_INFO, "EhciPciEmulationDriverBindingStart: started for controller @ %p\n", Controller));

  return Status;
}




/**
  Stop this driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param  This                 Protocol instance pointer.
  @param  Controller           Handle of device to stop driver on.
  @param  NumberOfChildren     Number of Children in the ChildHandleBuffer.
  @param  ChildHandleBuffer    List of handles for the children we need to stop.

  @return EFI_SUCCESS          Success.
  @return EFI_DEVICE_ERROR     Fail.

**/
EFI_STATUS
EFIAPI
EhcPciEmulationDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  )
{
  EFI_STATUS              Status;
  EFI_PCI_IO_PROTOCOL     *PciIo;
  EFI_PCI_IO_PRIVATE_DATA *Private;

  DEBUG((EFI_D_INFO, "EhcPciEmulationDriverBindingStop++\n"));

  // Test whether the Controller handler passed in is a valid
  // Usb controller handle that should be supported, if not,
  // return the error status directly
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "EhcPciEmulationDriverBindingStop OpenProtocol failed: Status = 0x%x\n", Status));
    return Status;
  }

  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  PciIo
                  );

  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "EhcPciEmulationDriverBindingStop OpenProtocol failed: Status = 0x%x\n", Status));
    return Status;
  }

  //get pointer to private data associated w/ protocol handle
  Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(PciIo);

  if (Private)
  {
    Status = gBS->UninstallProtocolInterface(
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  &Private->DevicePath);

    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "EhciPciEmulationEntryPoint UninstallProtocolInterfaces for device path failed.\n"));
    }

    gBS->FreePool (Private);
  }
  else
  {
    DEBUG((EFI_D_ERROR, "EhcPciEmulationDriverBindingStop: Error: PciIO private data == NULL\n"));
  }

  // Close the USB_CONFIG
  Status =  gBS->CloseProtocol (
         Controller,
         &gQcomUsbConfigProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if (EFI_ERROR (Status)) {
    DEBUG(( EFI_D_ERROR,
            "EhcPciEmulationDriverBindingStop: Unable to close USB Config Protocol, Status =  (0x%x)\r\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}



