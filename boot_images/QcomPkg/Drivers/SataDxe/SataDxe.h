/** @file SataDxe.h

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

when        who     what, where, why
----------  ---     -----------------------------------------------------
2014/11/04  rm      Initial version. 

=============================================================================*/

#ifndef _SATA_DXE_H_
#define _SATA_DXE_H_

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DmaLib.h>

#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>

#include <IndustryStandard/Pci30.h>
#include <IndustryStandard/Pci23.h>
#include <IndustryStandard/Pci22.h>
#include <IndustryStandard/Acpi.h>

#define QUALCOMM_VENDOR_ID                     0x17CB
#define QUALCOMM_HAWKER_DEVICE_ID              0x0400

#define EFI_AHCI_BAR_INDEX                     0x05
#define EFI_SATA_CONTROLLER_UNIQUE_MASK        0xF0

#define PCI_ROOT_BRIDGE_SIGNATURE     SIGNATURE_32 ('s', 'a', 't', 'a')

typedef struct {
  UINT32                             Signature;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL    Io;
  UINT32                             MemoryStart;
  UINT32                             MemorySize;
} PCI_ROOT_BRIDGE;

#define INSTANCE_FROM_PCI_ROOT_BRIDGE_IO_THIS(a)    CR (a, PCI_ROOT_BRIDGE, Io, PCI_ROOT_BRIDGE_SIGNATURE)

typedef struct {
  PCI_DEVICE_PATH           PciDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  EndDevicePath;
} EFI_PCI_IO_DEVICE_PATH;

typedef struct {
  UINT32                  Signature;
  EFI_HANDLE              ControllerHandle;
  EFI_PCI_IO_DEVICE_PATH  DevicePath;
  EFI_PCI_IO_PROTOCOL     PciIoProtocol;
  PCI_TYPE00              *ConfigSpace;
  PCI_ROOT_BRIDGE         RootBridge;
  UINTN                   Segment;
} EFI_SATA_CONTROLLER_DEVICE;

#define EFI_SATA_CONTROLLER_DEVICE_SIGNATURE     SIGNATURE_32('a', 'h', 'c', 'i')
#define EFI_SATA_CONTROLLER_DEVICE_FROM_THIS(a)  CR(a, EFI_SATA_CONTROLLER_DEVICE, PciIoProtocol, EFI_SATA_CONTROLLER_DEVICE_SIGNATURE)

typedef union {
  UINT8   volatile  *buf;
  UINT8   volatile  *ui8;
  UINT16  volatile  *ui16;
  UINT32  volatile  *ui32;
  UINT64  volatile  *ui64;
  UINTN   volatile  ui;
} PTR;

//
// PCI IO functions, documented in MdePkg/Include/Protocol/PciIo.h
//
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
  );

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
  );
  
EFI_STATUS
PciIoMemRead (
  IN     EFI_PCI_IO_PROTOCOL          *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  );

EFI_STATUS
PciIoMemWrite (
  IN     EFI_PCI_IO_PROTOCOL          *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  );

EFI_STATUS
PciIoIoRead (
  IN     EFI_PCI_IO_PROTOCOL          *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  );

EFI_STATUS
PciIoIoWrite (
  IN     EFI_PCI_IO_PROTOCOL          *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  );

EFI_STATUS
PciIoPciRead (
  IN     EFI_PCI_IO_PROTOCOL          *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT32                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  );

EFI_STATUS
PciIoPciWrite (
  IN     EFI_PCI_IO_PROTOCOL          *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT32                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  );
  
EFI_STATUS
PciIoCopyMem (
  IN EFI_PCI_IO_PROTOCOL          *This,
  IN EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN UINT8                        DestBarIndex,
  IN UINT64                       DestOffset,
  IN UINT8                        SrcBarIndex,
  IN UINT64                       SrcOffset,
  IN UINTN                        Count
  );

EFI_STATUS
PciIoMap (
  IN     EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_OPERATION    Operation,
  IN     VOID                             *HostAddress,
  IN OUT UINTN                            *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS             *DeviceAddress,
  OUT    VOID                             **Mapping
  );

EFI_STATUS
PciIoUnmap (
  IN EFI_PCI_IO_PROTOCOL    *This,
  IN VOID                   *Mapping
  );

EFI_STATUS
PciIoAllocateBuffer (
  IN  EFI_PCI_IO_PROTOCOL    *This,
  IN  EFI_ALLOCATE_TYPE      Type,
  IN  EFI_MEMORY_TYPE        MemoryType,
  IN  UINTN                  Pages,
  OUT VOID                   **HostAddress,
  IN  UINT64                 Attributes
  );

EFI_STATUS
PciIoFreeBuffer (
  IN EFI_PCI_IO_PROTOCOL    *This,
  IN UINTN                  Pages,
  IN VOID                   *HostAddress
  );

EFI_STATUS
PciIoFlush (
  IN EFI_PCI_IO_PROTOCOL    *This
  );

EFI_STATUS
PciIoGetLocation (
  IN  EFI_PCI_IO_PROTOCOL    *This,
  OUT UINTN                  *SegmentNumber,
  OUT UINTN                  *BusNumber,
  OUT UINTN                  *DeviceNumber,
  OUT UINTN                  *FunctionNumber
  );

EFI_STATUS
PciIoAttributes (
  IN  EFI_PCI_IO_PROTOCOL                        *This,
  IN  EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION    Operation,
  IN  UINT64                                     Attributes,
  OUT UINT64                                     *Result OPTIONAL
  );

EFI_STATUS
PciIoGetBarAttributes (
  IN  EFI_PCI_IO_PROTOCOL    *This,
  IN  UINT8                  BarIndex,
  OUT UINT64                 *Supports, OPTIONAL
  OUT VOID                   **Resources OPTIONAL
  );


EFI_STATUS
PciIoSetBarAttributes (
  IN     EFI_PCI_IO_PROTOCOL    *This,
  IN     UINT64                 Attributes,
  IN     UINT8                  BarIndex,
  IN OUT UINT64                 *Offset,
  IN OUT UINT64                 *Length
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoPollMem ( 
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  );
  
EFI_STATUS
EFIAPI
PciRootBridgeIoPollIo ( 
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINT64                                 Address,
  IN  UINT64                                 Mask,
  IN  UINT64                                 Value,
  IN  UINT64                                 Delay,
  OUT UINT64                                 *Result
  );
  
EFI_STATUS
EFIAPI
PciRootBridgeIoMemRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoMemWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoIoRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *UserBuffer
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoIoWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 UserAddress,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *UserBuffer
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoCopyMem (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT64                                 DestAddress,
  IN UINT64                                 SrcAddress,
  IN UINTN                                  Count
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoPciRead (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoPciWrite (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN     UINT64                                 Address,
  IN     UINTN                                  Count,
  IN OUT VOID                                   *Buffer
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoMap (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL            *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                                       *HostAddress,
  IN OUT UINTN                                      *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                       *DeviceAddress,
  OUT    VOID                                       **Mapping
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoUnmap (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN VOID                             *Mapping
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoAllocateBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  EFI_ALLOCATE_TYPE                Type,
  IN  EFI_MEMORY_TYPE                  MemoryType,
  IN  UINTN                            Pages,
  OUT VOID                             **HostAddress,
  IN  UINT64                           Attributes
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoFreeBuffer (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN  UINTN                            Pages,
  OUT VOID                             *HostAddress
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoFlush (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoGetAttributes (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT UINT64                           *Supported,
  OUT UINT64                           *Attributes
  );

EFI_STATUS
EFIAPI
PciRootBridgeIoSetAttributes (
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  IN     UINT64                           Attributes,
  IN OUT UINT64                           *ResourceBase,
  IN OUT UINT64                           *ResourceLength 
  ); 

EFI_STATUS
EFIAPI
PciRootBridgeIoConfiguration (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This,
  OUT VOID                             **Resources
  );

//
// Private Function Prototypes
//
EFI_STATUS
EFIAPI
PciRootBridgeIoMemRW (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN  UINTN                                  Count,
  IN  BOOLEAN                                InStrideFlag,
  IN  PTR                                    In,
  IN  BOOLEAN                                OutStrideFlag,
  OUT PTR                                    Out
  );

BOOLEAN
PciIoMemAddressValid (
  IN EFI_PCI_IO_PROTOCOL  *This,
  IN UINT64               Address
  );
      
#endif /* _SATA_DXE_H_ */

