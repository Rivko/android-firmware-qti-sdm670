/** @file
*  Header containing the structure specific to Qualcomm PCIe Root Complex
*
*  Copyright (c) 2016, Qualcomm Technologies Inc. All rights reserved.
*  Copyright (c) 2011-2015, ARM Ltd. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
*  EDIT HISTORY
*
*  when      who      what, where, why
*  --------  -------  --------------------------------------------------------
*  07/28/16  tselvam  Included I/O port access support
*  07/08/16  tselvam  Derived from ARMJuno PciHostBridgeDxe for Qualcomm PCIe
**/

#ifndef __PCIHOSTBRIDGE_H
#define __PCIHOSTBRIDGE_H

#include <PiDxe.h>

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcieConfigLib.h>

#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/DevicePath.h>

#define PCI_TRACE(txt)  DEBUG((EFI_D_VERBOSE, "QCOM_PCI: " txt "\n"))

#define MAX_PCI_DEVICE_NUMBER      31
#define MAX_PCI_FUNCTION_NUMBER    7
#define MAX_PCI_REG_ADDRESS        (SIZE_4KB - 1)

/**
 * PCI Root Bridge Device Path (ACPI Device Node + End Node)
 */
typedef struct {
  ACPI_HID_DEVICE_PATH          Acpi;
  EFI_DEVICE_PATH_PROTOCOL      End;
} EFI_PCI_ROOT_BRIDGE_DEVICE_PATH;

typedef enum {
  ResTypeIo = 0,
  ResTypeMem32,
  ResTypePMem32,
  ResTypeMem64,
  ResTypePMem64,
  ResTypeBus,
  ResTypeMax
} PCI_RESOURCE_TYPE;

typedef enum {
  ResNone = 0,
  ResSubmitted,
  ResRequested,
  ResAllocated,
  ResStatusMax
} RES_STATUS;

typedef enum {
  IoOperation,
  MemOperation,
  PciOperation
} OPERATION_TYPE;

#define ACPI_SPECFLAG_PREFETCHABLE      0x06
#define EFI_RESOURCE_NONEXISTENT        0xFFFFFFFFFFFFFFFFULL
#define EFI_RESOURCE_LESS               0xFFFFFFFFFFFFFFFEULL

typedef struct {
  UINT64  Base;
  UINT64  Length;
  UINT64  Alignment;
  RES_STATUS Status;
} PCI_RESOURCE_ALLOC;

typedef struct {
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION  Operation;
  UINTN                                      NumberOfBytes;
  UINTN                                      NumberOfPages;
  EFI_PHYSICAL_ADDRESS                       HostAddress;
  EFI_PHYSICAL_ADDRESS                       MappedHostAddress;
} MAP_INFO;

/**
 * PCI Host Bridge Instance structure
 **/
typedef struct _PCI_HOST_BRIDGE_INSTANCE {
  UINTN                                             Signature;
  EFI_HANDLE                                        Handle;
  EFI_HANDLE                                        ImageHandle;
  UINT32                                            HostBridgeIndex;
  /*
   * The enumeration cannot be restarted after the process goes into the non initial
   * enumeration phase.
   */
  BOOLEAN                                           ResourceSubmitted;
  BOOLEAN                                           CanRestarted;
  UINT32                                            NumberOfRootBridges;
  LIST_ENTRY                                        RbHead;
  LIST_ENTRY                                        HbList;
  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  ResAlloc;
}PCI_HOST_BRIDGE_INSTANCE;

/**
 * PCI Root Bridge Instance structure
 **/
typedef struct {
  UINTN                             Signature;
  EFI_HANDLE                        Handle;
  PCI_HOST_BRIDGE_INSTANCE*         HostBridge;
  LIST_ENTRY                        Link;
  /*
   * Set Type of memory allocation (could be EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM
   * and EFI_PCI_HOST_BRIDGE_MEM64_DECODE).
   */
  UINT64                            MemAllocAttributes;
  PCI_RESOURCE_ALLOC                ResAlloc[ResTypeMax];
  PCI_ROOT_BRIDGE_RESOURCE_APERTURE ResAperture[PciRbResTypeMax];
  UINTN                             BusStart;
  UINTN                             BusLength;
  UINT64                            Supports;
  UINT64                            Attributes;
  EFI_PCI_ROOT_BRIDGE_DEVICE_PATH   DevicePath;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   Io;
} PCI_ROOT_BRIDGE_INSTANCE;

#define PCI_HOST_BRIDGE_SIGNATURE                   SIGNATURE_32 ('e', 'h', 's', 't')
#define PCI_ROOT_BRIDGE_SIGNATURE                   SIGNATURE_32 ('e', '2', 'p', 'b')
#define INSTANCE_FROM_RESOURCE_ALLOCATION_THIS(a) \
  CR (a, PCI_HOST_BRIDGE_INSTANCE, ResAlloc, PCI_HOST_BRIDGE_SIGNATURE)
#define INSTANCE_FROM_ROOT_BRIDGE_IO_THIS(a) \
  CR (a, PCI_ROOT_BRIDGE_INSTANCE, Io, PCI_ROOT_BRIDGE_SIGNATURE)
#define HOST_BRIDGE_INSTANCE_FROM_LIST_ENTRY(a) \
  CR (a, PCI_HOST_BRIDGE_INSTANCE, HbList, PCI_HOST_BRIDGE_SIGNATURE)
#define ROOT_BRIDGE_INSTANCE_FROM_LIST_ENTRY(a) \
  CR (a, PCI_ROOT_BRIDGE_INSTANCE, Link, PCI_ROOT_BRIDGE_SIGNATURE)

#define SYS_ADDR_TO_PCI_ADDR(addr, r)   ((addr) - r.Start + r.Base)
#define PCI_ADDR_TO_SYS_ADDR(addr, r)   ((addr) + r.Start - r.Base)
#define RESOURCE_BASE(r)                (r.Base)
#define RESOURCE_LIMIT(r)               (r.End - r.Start + r.Base)

/**
 * PCI Host Bridge Resource Allocator Functions
 **/
EFI_STATUS PciHbRaNotifyPhase (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE     Phase
  );

EFI_STATUS PciHbRaGetNextRootBridge (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN OUT EFI_HANDLE                                    *RootBridgeHandle
  );

EFI_STATUS PciHbRaGetAllocAttributes (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN  EFI_HANDLE                                        RootBridgeHandle,
  OUT UINT64                                           *Attributes
  );

EFI_STATUS PciHbRaStartBusEnumeration (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN  EFI_HANDLE                                        RootBridgeHandle,
  OUT VOID                                            **Configuration
  );

EFI_STATUS PciHbRaSetBusNumbers (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN EFI_HANDLE                                         RootBridgeHandle,
  IN VOID                                              *Configuration
  );

EFI_STATUS PciHbRaSubmitResources (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN EFI_HANDLE                                         RootBridgeHandle,
  IN VOID                                              *Configuration
  );

EFI_STATUS PciHbRaGetProposedResources (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN  EFI_HANDLE                                        RootBridgeHandle,
  OUT VOID                                            **Configuration
  );

EFI_STATUS PciHbRaPreprocessController (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN  EFI_HANDLE                                        RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS       PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE      Phase
  );


/**
 * PCI Root Bridge
 **/
EFI_STATUS PciRbPollMem (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL           *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN  UINT64                                   Address,
  IN  UINT64                                   Mask,
  IN  UINT64                                   Value,
  IN  UINT64                                   Delay,
  OUT UINT64                                   *Result
  );

EFI_STATUS PciRbPollIo (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL           *This,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN  UINT64                                   Address,
  IN  UINT64                                   Mask,
  IN  UINT64                                   Value,
  IN  UINT64                                   Delay,
  OUT UINT64                                   *Result
  );

EFI_STATUS PciRbMemRead (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL              *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN     UINT64                                   Address,
  IN     UINTN                                    Count,
  IN OUT VOID                                     *Buffer
  );

EFI_STATUS PciRbMemWrite (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL              *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN     UINT64                                   Address,
  IN     UINTN                                    Count,
  IN OUT VOID                                     *Buffer
  );

EFI_STATUS PciRbIoRead (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL              *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN     UINT64                                   Address,
  IN     UINTN                                    Count,
  IN OUT VOID                                     *Buffer
  );

EFI_STATUS PciRbIoWrite (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL              *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN     UINT64                                   Address,
  IN     UINTN                                    Count,
  IN OUT VOID                                     *Buffer
  );

EFI_STATUS PciRbPciRead (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL              *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN     UINT64                                   Address,
  IN     UINTN                                    Count,
  IN OUT VOID                                     *Buffer
  );

EFI_STATUS PciRbPciWrite (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL              *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN     UINT64                                   Address,
  IN     UINTN                                    Count,
  IN OUT VOID                                     *Buffer
  );

EFI_STATUS PciRbCopyMem (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL              *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
  IN     UINT64                                   DestAddress,
  IN     UINT64                                   SrcAddress,
  IN     UINTN                                    Count
  );

EFI_STATUS PciRbMap (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                *This,
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                                       *HostAddress,
  IN OUT UINTN                                      *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS                       *DeviceAddress,
  OUT    VOID                                       **Mapping
  );

EFI_STATUS PciRbUnMap (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL           *This,
  IN  VOID                                     *Mapping
  );

EFI_STATUS PciRbAllocateBuffer (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL              *This,
  IN     EFI_ALLOCATE_TYPE                        Type,
  IN     EFI_MEMORY_TYPE                          MemoryType,
  IN     UINTN                                    Pages,
  IN OUT VOID                                     **HostAddress,
  IN     UINT64                                   Attributes
  );

EFI_STATUS PciRbFreeBuffer (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL           *This,
  IN  UINTN                                    Pages,
  IN  VOID                                     *HostAddress
  );

EFI_STATUS PciRbFlush (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *This
  );

EFI_STATUS PciRbSetAttributes (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL              *This,
  IN     UINT64                                   Attributes,
  IN OUT UINT64                                   *ResourceBase,
  IN OUT UINT64                                   *ResourceLength
  );

EFI_STATUS PciRbGetAttributes (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL           *This,
  OUT UINT64                                   *Supports,
  OUT UINT64                                   *Attributes
  );

EFI_STATUS PciRbConfiguration (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL          *This,
  OUT VOID                                     **Resources
  );

/**
 * PCI Root Bridge Functions
 **/
EFI_STATUS
PciRbConstructor (
  IN  PCI_HOST_BRIDGE_INSTANCE *HostBridge,
  IN  UINT32 PciAcpiUid
  );

EFI_STATUS
PciRbDestructor (
  IN  PCI_ROOT_BRIDGE_INSTANCE* RootBridge
  );

#endif
