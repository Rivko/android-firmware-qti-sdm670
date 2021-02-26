/** @file

Copyright (c) 2010 - 2012, 2014-2015, 2017 Qualcomm Technologies, Inc.	All Rights Reserved.
Portions Copyright (c) 2005 - 2009, Intel Corporation. All rights reserved.<BR>
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
 --------   ---      ---------------------------------------------------------
 02/01/17   pm      Port from Boot.xf.1.4
 02/11/15   amitg   Fix Compiler Warning
 04/30/14   amitg   Updates Assert with USB Assert Macro
=============================================================================*/

#include "XhciPciEmulation.h"
#include <Library/UncachedMemoryAllocationLib.h>
#include <Protocol/EFIUsbConfig.h>


//
// Maximum number of extended capabilities to read
// Cap ID uses 8 bits, so set to maximum possible
//
#define XHC_MAX_EXT_CAPS            255

//
// Definitions from MdeModulePkg/Bus/Pci/XhciDxe/XhciReg.h:
//
#define PCI_IF_XHCI                 0x30

#define XHC_HCSPARAMS1_OFFSET       0x04   // Structural Parameters 1
#define XHC_HCCPARAMS_OFFSET        0x10   // Capability Parameters
#define XHC_DBOFF_OFFSET            0x14   // Doorbell Offset
#define XHC_RTSOFF_OFFSET           0x18   // Runtime Register Space Offset
#define XHC_PORTSC_OFFSET           0x0400 // Port Status and Control Register Offset
#define XHC_PORTSC_PP               0x0200 // Port Power

#pragma pack (1)
//
// Structural Parameters 1 Register Bitmap Definition
// (from XhciReg.h)
//
typedef struct {
  UINT8                   MaxSlots;       // Number of Device Slots
  UINT16                  MaxIntrs:11;    // Number of Interrupters
  UINT16                  Rsvd:5;
  UINT8                   MaxPorts;       // Number of Ports
} HCSPARAMS1;

typedef union {
  UINT32                  Dword;
  HCSPARAMS1              Data;
} XHC_HCSPARAMS1;

//
// Capability Parameters Register Bitmap Definition
// (from XhciReg.h)
//
typedef struct {
  UINT16                  Ac64:1;        // 64-bit Addressing Capability
  UINT16                  Bnc:1;         // BW Negotiation Capability
  UINT16                  Csz:1;         // Context Size
  UINT16                  Ppc:1;         // Port Power Control
  UINT16                  Pind:1;        // Port Indicators
  UINT16                  Lhrc:1;        // Light HC Reset Capability
  UINT16                  Ltc:1;         // Latency Tolerance Messaging Capability
  UINT16                  Nss:1;         // No Secondary SID Support
  UINT16                  Pae:1;         // Parse All Event Data
  UINT16                  Rsvd:3;
  UINT16                  MaxPsaSize:4;  // Maximum Primary Stream Array Size
  UINT16                  ExtCapReg;     // xHCI Extended Capabilities Pointer
} HCCPARAMS;

typedef union {
  UINT32                  Dword;
  HCCPARAMS               Data;
} XHC_HCCPARAMS;

//
// Extended Capability Register Bitmap Definition
// (see xHCI Spec ch 7)
//
typedef struct {
  UINT8                   CapID;      // Capability ID
  UINT8                   NextExCap;  // Pointer to the next Extended Capability register
  UINT16                  CapData;    // Capability specific data
} EXT_CAP_REG;

typedef union {
  UINT32                  Dword;
  EXT_CAP_REG             Data;
} XHC_EXT_CAP_REG;

#pragma pack ()


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
} EFI_PCI_IO_PRIVATE_DATA;

#define EFI_PCI_IO_PRIVATE_DATA_SIGNATURE     SIGNATURE_32('u', 's', 'b', 'x')
#define EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(a)  CR(a, EFI_PCI_IO_PRIVATE_DATA, PciIoProtocol, EFI_PCI_IO_PRIVATE_DATA_SIGNATURE)

  
EFI_DRIVER_BINDING_PROTOCOL
gXhciPciEmulationDriverBinding = {
  XhcPciEmulationDriverBindingSupported,
  XhcPciEmulationDriverBindingStart,
  XhcPciEmulationDriverBindingStop,
  0x30,
  NULL,
  NULL
};


EFI_STATUS
PciIoPollMem (
  IN  EFI_PCI_IO_PROTOCOL          *This,
  IN  EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN  UINT8                        BarIndex,
  IN  UINT64                       Offset,
  IN  UINT64                       Mask,
  IN  UINT64                       Value,
  IN  UINT64                       Delay,
  OUT UINT64                       *Result
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
PciIoPollIo (
  IN  EFI_PCI_IO_PROTOCOL          *This,
  IN  EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN  UINT8                        BarIndex,
  IN  UINT64                       Offset,
  IN  UINT64                       Mask,
  IN  UINT64                       Value,
  IN  UINT64                       Delay,
  OUT UINT64                       *Result
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
PciIoMemRead (
  IN     EFI_PCI_IO_PROTOCOL          *This,
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
  IN     EFI_PCI_IO_PROTOCOL          *This,
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
  IN     EFI_PCI_IO_PROTOCOL          *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  USB_ASSERT_RETURN (FALSE,EFI_UNSUPPORTED);
}

EFI_STATUS
PciIoIoWrite (
  IN     EFI_PCI_IO_PROTOCOL          *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  USB_ASSERT_RETURN (FALSE,EFI_UNSUPPORTED);
}

EFI_STATUS
PciIoPciRead (
  IN     EFI_PCI_IO_PROTOCOL          *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT32                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);
  PTR InBuffer, OutBuffer;

  InBuffer.buf  = Buffer;
  OutBuffer.buf = (((UINT8 *)Private->ConfigSpace) + Offset);
  return PciRootBridgeIoMemRW ((EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width,
                               Count,
                               TRUE,
                               InBuffer,
                               TRUE,
                               OutBuffer
                              );
}

EFI_STATUS
PciIoPciWrite (
  IN     EFI_PCI_IO_PROTOCOL          *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT32                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);
  PTR InBuffer, OutBuffer;

  InBuffer.buf  = (((UINT8 *)Private->ConfigSpace) + Offset);
  OutBuffer.buf = Buffer;
  return PciRootBridgeIoMemRW ((EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) Width,
                               Count,
                               TRUE,
                               InBuffer,        
                               TRUE,
                               OutBuffer
                              );
}

EFI_STATUS
PciIoCopyMem (
  IN EFI_PCI_IO_PROTOCOL          *This,
  IN EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN UINT8                        DestBarIndex,
  IN UINT64                       DestOffset,
  IN UINT8                        SrcBarIndex,
  IN UINT64                       SrcOffset,
  IN UINTN                        Count
  )
{
  USB_ASSERT_RETURN (FALSE,EFI_UNSUPPORTED);
}

EFI_STATUS
PciIoMap (
  IN     EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_OPERATION    Operation,
  IN     VOID                             *HostAddress,
  IN OUT UINTN                            *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS             *DeviceAddress,
  OUT    VOID                             **Mapping
  )
{
  *DeviceAddress = ConvertToPhysicalAddress (HostAddress);
  *Mapping = NULL;
  return EFI_SUCCESS;
}

EFI_STATUS
PciIoUnmap (
  IN EFI_PCI_IO_PROTOCOL    *This,
  IN VOID                   *Mapping
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
PciIoAllocateBuffer (
  IN  EFI_PCI_IO_PROTOCOL    *This,
  IN  EFI_ALLOCATE_TYPE      Type,
  IN  EFI_MEMORY_TYPE        MemoryType,
  IN  UINTN                  Pages,
  OUT VOID                   **HostAddress,
  IN  UINT64                 Attributes
  )
{
  if (HostAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // The only valid memory types are EfiBootServicesData and EfiRuntimeServicesData
  //
  if (MemoryType == EfiBootServicesData) {
    *HostAddress = UncachedAllocatePages (Pages);
  } else if (MemoryType == EfiRuntimeServicesData) {
    *HostAddress = UncachedAllocateRuntimePages (Pages);
  } else {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciIoFreeBuffer (
  IN EFI_PCI_IO_PROTOCOL    *This,
  IN UINTN                  Pages,
  IN VOID                   *HostAddress
  )
{
  if (HostAddress == NULL) {
     return EFI_INVALID_PARAMETER;
  }

  FreePages (HostAddress, Pages);
  return EFI_SUCCESS;
}

EFI_STATUS
PciIoFlush (
  IN EFI_PCI_IO_PROTOCOL    *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
PciIoGetLocation (
  IN  EFI_PCI_IO_PROTOCOL    *This,
  OUT UINTN                  *SegmentNumber,
  OUT UINTN                  *BusNumber,
  OUT UINTN                  *DeviceNumber,
  OUT UINTN                  *FunctionNumber
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
  IN  EFI_PCI_IO_PROTOCOL                        *This,
  IN  EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION    Operation,
  IN  UINT64                                     Attributes,
  OUT UINT64                                     *Result OPTIONAL
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
    USB_ASSERT_RETURN (FALSE,EFI_INVALID_PARAMETER);
  };
  return EFI_SUCCESS;
}

EFI_STATUS
PciIoGetBarAttributes (
  IN  EFI_PCI_IO_PROTOCOL    *This,
  IN  UINT8                  BarIndex,
  OUT UINT64                 *Supports, OPTIONAL
  OUT VOID                   **Resources OPTIONAL
  )
{
  USB_ASSERT_RETURN (FALSE,EFI_UNSUPPORTED);
}

EFI_STATUS
PciIoSetBarAttributes (
  IN     EFI_PCI_IO_PROTOCOL    *This,
  IN     UINT64                 Attributes,
  IN     UINT8                  BarIndex,
  IN OUT UINT64                 *Offset,
  IN OUT UINT64                 *Length
  )
{
  USB_ASSERT_RETURN (FALSE,EFI_UNSUPPORTED);
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
XhciPciEmulationEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
   return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gXhciPciEmulationDriverBinding,
           ImageHandle,
           NULL,
           NULL
           );
}


EFI_STATUS
EFIAPI
XhcPciEmulationDriverBindingSupported (
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

  //check if controller is for a valid core that supports Xhci
  if ( (pUsbConfigProtocol->coreNum < USB_CORE_MAX_NUM) && (pUsbConfigProtocol->modeType == USB_HOST_MODE_XHCI) )
  {
    DEBUG(( EFI_D_INFO, "XhcPciEmulationDriverBindingSupported: This is core#%d, CoreType = (0x%x)\r\n", 
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
  Starting the Usb XHCI Driver.

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
XhcPciEmulationDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  QCOM_USB_CONFIG_PROTOCOL *pUsbConfigProtocol;
  QCOM_USB_CONFIG_CORE_TYPE coreType;
  EFI_STATUS              Status;
  EFI_PCI_IO_PRIVATE_DATA *Private;
  XHC_HCSPARAMS1             hcsparams1;
  XHC_HCCPARAMS              hccparams;
  XHC_EXT_CAP_REG            ExtCapReg;
  struct {
    UINT32                   ExCapOffset;
    UINT32                   DBOffset;
    UINT32                   RTSOffset;
    UINT32                   LastOffset;
  } Offsets;
  UINT8                   CapabilityLength;
  UINT8                   PhysicalPorts;
  UINTN                   Count;
  UINTN                   CoreBaseAddr;
  EFI_PCI_IO_DEVICE_PATH     PciIoDevicePathTemplate =
  {
    {
      { HARDWARE_DEVICE_PATH, HW_PCI_DP, {sizeof (PCI_DEVICE_PATH), 0}},
      0,
      0
    },
    { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, {sizeof (EFI_DEVICE_PATH_PROTOCOL), 0}}
  };

  DEBUG(( EFI_D_INFO, "XhcPciEmulationDriverBindingStart++\r\n"));

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
            "XhcPciEmulationDriverBindingStart: Unable to open USB Config Protocol, Status =  (0x%x)\r\n", Status));
    return Status;
  }

  DEBUG(( EFI_D_INFO, "XhcPciEmulationDriverBindingStart: This is core#%d, CoreType = (0x%x)\r\n", 
          pUsbConfigProtocol->coreNum, pUsbConfigProtocol->modeType ));
 
  // Configure USB core
  Status = pUsbConfigProtocol->ConfigUsb (pUsbConfigProtocol, USB_HOST_MODE_XHCI, pUsbConfigProtocol->coreNum);

  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_ERROR,  "XhcPciEmulationDriverBindingStart: Unable to configure USB in host mode, Status =  (0x%x)\r\n", Status));
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

  Status = pUsbConfigProtocol->GetUsbHostConfig(pUsbConfigProtocol, USB_HOST_MODE_XHCI, pUsbConfigProtocol->coreNum, &coreType);

  if (EFI_ERROR (Status)) {
    DEBUG(( EFI_D_ERROR,
            "XhcPciEmulationDriverBindingStart: Unable to get core type, Status =  (0x%x)\r\n", Status));
    return Status;
  }

  Status = pUsbConfigProtocol->GetCoreBaseAddr (pUsbConfigProtocol, coreType, &CoreBaseAddr);
  if (EFI_ERROR (Status)) {
    DEBUG(( EFI_D_ERROR,
            "XhcPciEmulationDriverBindingStart: Unable to get core base address, Status =  (0x%x)\r\n", Status));
    return Status;
  }

  Private->RootBridge.MemoryStart = (UINT32)CoreBaseAddr;
  Private->Segment                = 0;  // Default to segment zero

  // Get some info from the XHCI capability registers
  CapabilityLength = MmioRead8  (Private->RootBridge.MemoryStart);
  hcsparams1.Dword = MmioRead32 (Private->RootBridge.MemoryStart + XHC_HCSPARAMS1_OFFSET);
  PhysicalPorts    = hcsparams1.Data.MaxPorts;

  // Get the offsets of other register sections to determine which one comes last in the address space
  hccparams.Dword     = MmioRead32 (Private->RootBridge.MemoryStart + XHC_HCCPARAMS_OFFSET);
  Offsets.ExCapOffset = hccparams.Data.ExtCapReg << 2;
  Offsets.DBOffset    = MmioRead32 (Private->RootBridge.MemoryStart + XHC_DBOFF_OFFSET);
  Offsets.RTSOffset   = MmioRead32 (Private->RootBridge.MemoryStart + XHC_RTSOFF_OFFSET);

  if ((Offsets.ExCapOffset > Offsets.DBOffset) && (Offsets.ExCapOffset > Offsets.RTSOffset)) {
    //
    // Counter for extended capabilites to ensure loop termination in case
    // unexpected data is read from a capability register
    //
    UINT32 cnt = 0;

    //
    // Extended Capabilites are at the end, find last address used by them
    // (see xHCI spec ch 5.3.6 and 7)
    //
    ExtCapReg.Dword = 0;
    do {
      Offsets.ExCapOffset += ExtCapReg.Data.NextExCap << 2;
      ExtCapReg.Dword      = MmioRead32 (Private->RootBridge.MemoryStart + Offsets.ExCapOffset);
    } while ((ExtCapReg.Data.NextExCap > 0) && (++cnt < XHC_MAX_EXT_CAPS));

    Offsets.LastOffset = Offsets.ExCapOffset + 3; // ExCapOffset points to low byte of the Dword capability

  } else if ((Offsets.DBOffset > Offsets.ExCapOffset) && (Offsets.DBOffset > Offsets.RTSOffset)) {
    //
    // Doorbell register space is at the end, find its last address
    // Each doorbell is 32 bits, MaxSlots is total number of Doorbell registers
    //
    Offsets.LastOffset = Offsets.DBOffset + (hcsparams1.Data.MaxSlots << 2) - 1; // Last byte of last doorbell

  } else {
    //
    // Runtime Register Space is at the end, find its last address
    // RTS is composed of 1024 Interrupter Register sets of 32 bytes each, first one at offset of 0x20
    //
    Offsets.LastOffset = Offsets.RTSOffset + 0x801f;
  }

  // The last used address calculated above is the size of the memory that may need to be accessed
  Private->RootBridge.MemorySize = Offsets.LastOffset;

  // check if port power control is supported, if not ports are always powered
  if (hccparams.Data.Ppc) {
    // Set Port Power bit for each port
    for (Count = 0; Count < PhysicalPorts; Count++) {
      MmioOr32 ((Private->RootBridge.MemoryStart + CapabilityLength + XHC_PORTSC_OFFSET + (0x10*Count)), XHC_PORTSC_PP);
    }
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
  Private->ConfigSpace->Hdr.DeviceId = QUALCOMM_XHCI_DEVICE_ID;
  Private->ConfigSpace->Hdr.ClassCode[0] = PCI_IF_XHCI;
  Private->ConfigSpace->Hdr.ClassCode[1] = PCI_CLASS_SERIAL_USB;
  Private->ConfigSpace->Hdr.ClassCode[2] = PCI_CLASS_SERIAL;
  Private->ConfigSpace->Device.Bar[0] = Private->RootBridge.MemoryStart;

  // Unique device number
  // Set top bit to differentiate xhci cores from ehci cores
  PciIoDevicePathTemplate.PciDevicePath.Device = (0x80 | pUsbConfigProtocol->coreNum);
  // Unique device path.
  CopyMem(&Private->DevicePath, &PciIoDevicePathTemplate, sizeof(PciIoDevicePathTemplate));

  // Copy protocol structure
  CopyMem(&Private->PciIoProtocol, &PciIoTemplate, sizeof(PciIoTemplate));

  //vsb: TODO: append device path instead of installing
  Status = gBS->InstallMultipleProtocolInterfaces(&Controller,
                                                  &gEfiPciIoProtocolGuid,       &Private->PciIoProtocol,
                                                  &gEfiDevicePathProtocolGuid,  &Private->DevicePath,
                                                  NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "XhciPciEmulationDriverBindingStart: InstallMultipleProtocolInterfaces failed. Status = 0x%x\n", Status));
  }

  DEBUG ((EFI_D_INFO, "XhciPciEmulationDriverBindingStart: started for controller @ %p\n", Controller));

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
XhcPciEmulationDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  )
{
  EFI_STATUS              Status;
  EFI_PCI_IO_PROTOCOL     *PciIo;
  EFI_PCI_IO_PRIVATE_DATA *Private;
   //
  // Test whether the Controller handler passed in is a valid
  // Usb controller handle that should be supported, if not,
  // return the error status directly
  //
  
  DEBUG((EFI_D_INFO, "XhcPciEmulationDriverBindingStop++\n"));

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "XhcPciEmulationDriverBindingStop OpenProtocol failed: Status = 0x%x\n", Status));
    return Status;
  }
  
  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  PciIo
                  );

  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "XhcPciEmulationDriverBindingStop OpenProtocol failed: Status = 0x%x\n", Status));
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
      DEBUG((EFI_D_ERROR, "XhciPciEmulationEntryPoint UninstallProtocolInterfaces for device path failed.\n"));
    }
    
    gBS->FreePool (Private);
  }
  else
  {
    DEBUG((EFI_D_ERROR, "XhcPciEmulationDriverBindingStop: Error: PciIO private data == NULL\n"));
  }
  
  // Close the USB_CONFIG used to perform the supported test
  Status =  gBS->CloseProtocol (
         Controller,
         &gQcomUsbConfigProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if (EFI_ERROR (Status)) {
    DEBUG(( EFI_D_ERROR,
            "XhcPciEmulationDriverBindingStop: Unable to close USB Config Protocol, Status =  (0x%x)\r\n", Status));
    return Status;
  }
  
  return EFI_SUCCESS;
}



