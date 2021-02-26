/** @file SataPciEmulation.c
  
  XBL SATA driver code
  
  Copyright (c) 2014 Qualcomm Technologies, Inc.	
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
  
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

when        who     what, where, why
----------  ---     -----------------------------------------------------
2014/11/04  rm      Initial version(Integrated). 

=============================================================================*/

#include "SataDxe.h"
#include <Library/UncachedMemoryAllocationLib.h>
  
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
  EFI_SATA_CONTROLLER_DEVICE *Private = EFI_SATA_CONTROLLER_DEVICE_FROM_THIS(This);

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
  EFI_SATA_CONTROLLER_DEVICE *Private = EFI_SATA_CONTROLLER_DEVICE_FROM_THIS(This);

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
  return EFI_UNSUPPORTED;
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
  return EFI_UNSUPPORTED;
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
  EFI_SATA_CONTROLLER_DEVICE *Private = EFI_SATA_CONTROLLER_DEVICE_FROM_THIS(This);

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
  IN     EFI_PCI_IO_PROTOCOL          *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT32                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  EFI_SATA_CONTROLLER_DEVICE *Private = EFI_SATA_CONTROLLER_DEVICE_FROM_THIS(This);

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
  IN EFI_PCI_IO_PROTOCOL          *This,
  IN EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN UINT8                        DestBarIndex,
  IN UINT64                       DestOffset,
  IN UINT8                        SrcBarIndex,
  IN UINT64                       SrcOffset,
  IN UINTN                        Count
  )
{
  return EFI_UNSUPPORTED;
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
  DMA_MAP_OPERATION   DmaOperation;

  if (Operation == EfiPciIoOperationBusMasterRead) {
    DmaOperation = MapOperationBusMasterRead;
  } else if (Operation == EfiPciIoOperationBusMasterWrite) {
    DmaOperation = MapOperationBusMasterWrite;
  } else if (Operation == EfiPciIoOperationBusMasterCommonBuffer) {
    DmaOperation = MapOperationBusMasterCommonBuffer;
  } else {
    return EFI_INVALID_PARAMETER;
  }
  return DmaMap (DmaOperation, HostAddress, NumberOfBytes, DeviceAddress, Mapping);
}

EFI_STATUS
PciIoUnmap (
  IN EFI_PCI_IO_PROTOCOL    *This,
  IN VOID                   *Mapping
  )
{
  return DmaUnmap (Mapping);
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
  if (Attributes & EFI_PCI_ATTRIBUTE_INVALID_FOR_ALLOCATE_BUFFER) {
    // Check this
    return EFI_UNSUPPORTED;
  }

  return DmaAllocateBuffer (MemoryType, Pages, HostAddress);
}

EFI_STATUS
PciIoFreeBuffer (
  IN EFI_PCI_IO_PROTOCOL    *This,
  IN UINTN                  Pages,
  IN VOID                   *HostAddress
  )
{
  return DmaFreeBuffer (Pages, HostAddress);
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
  EFI_SATA_CONTROLLER_DEVICE *Private = EFI_SATA_CONTROLLER_DEVICE_FROM_THIS(This);

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
      return EFI_UNSUPPORTED;
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
  return EFI_UNSUPPORTED;
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
  return EFI_UNSUPPORTED;
}






