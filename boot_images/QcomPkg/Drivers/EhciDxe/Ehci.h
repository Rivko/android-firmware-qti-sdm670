/** @file

  Provides some data struct used by EHCI controller driver.

Copyright (c) 2010 - 2015 Qualcomm Technologies, Inc.	All Rights Reserved.
Portions Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
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
 08/07/15   ck       Reduce the transfer size only for simulation platform
 07/13/15   al       Changing TPL_NOTIFY to TPL_CALLBACK
 04/27/15   ck       Add initial Hawker support
 04/30/14   amitg    Update Assert with USB Assert Macro
 02/05/14   amitg    Async USB Host Stack
 01/20/14   ck       Port EDK2 host stack
 06/14/13   mliss    Added support to provide UsbPortTest protocol
 03/30/11   sniedzie Port changes for:
                     - Support for embedded transaction translator.
                     - Cache flushing workaround.
                     - Put PHY in host mode on reset.
 03/30/11   sniedzie Integrated from MdeModulePkg for QCOM changes.

=============================================================================*/

#ifndef _EFI_EHCI_H_
#define _EFI_EHCI_H_


#include <Uefi.h>

#include <Protocol/EFIUsb2HostController.h>
#include <Protocol/PciIo.h>
#include <Protocol/EFIUsbConfig.h>
#include <Protocol/EFIPlatformInfo.h>
#include <Guid/EventGroup.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/IoLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/ArmLib.h>
#include <IndustryStandard/Pci.h>

#include "UsbError.h"

typedef struct _USB2_HC_DEV  USB2_HC_DEV;

#include "UsbHcMem.h"
#include "EhciReg.h"
#include "EhciUrb.h"
#include "EhciSched.h"
#include "EhciDebug.h"
#include "ComponentName.h"
#include "EhciPortTest.h"

//
// EHC timeout experience values
//

#define EHC_1_MICROSECOND            1
#define EHC_1_MILLISECOND            (1000 * EHC_1_MICROSECOND)
#define EHC_1_SECOND                 (1000 * EHC_1_MILLISECOND)

// 100 ns is unit for the SetTimer
#define EHC_100_NANOSECOND            1
#define EHC_1_MILLISECOND_100NS      (1000 * 10 * EHC_100_NANOSECOND)

//
// EHCI register operation timeout, set by experience
//
#define EHC_RESET_TIMEOUT            (1 * EHC_1_SECOND)
#define EHC_GENERIC_TIMEOUT          (10 * EHC_1_MILLISECOND)

//
// Wait for roothub port power stable, refers to Spec[EHCI1.0-2.3.9]
//
#define EHC_ROOT_PORT_RECOVERY_STALL (20 * EHC_1_MILLISECOND)

//
// Sync and Async transfer polling interval, set by experience,
// and the unit of Async is 100us, means 50ms as interval.
//
#define EHC_SYNC_POLL_INTERVAL       (1 * EHC_1_MILLISECOND)
#define EHC_ASYNC_POLL_INTERVAL      (40 * 10000U)
#define EHC_ASYNC_BULK_POLL_TIMEOUT  (40000 * EHC_1_MILLISECOND)
#define EHC_ASYNC_CONTROL_POLL_TIMEOUT  (1000 * EHC_1_MILLISECOND)

//
// EHCI debug port control status register bit definition
//
#define USB_DEBUG_PORT_IN_USE        BIT10
#define USB_DEBUG_PORT_ENABLE        BIT28
#define USB_DEBUG_PORT_OWNER         BIT30

//
// EHC raises TPL to TPL_CALLBACK to serialize all its operations
// to protect shared data structures.
//
#define  EHC_TPL                TPL_CALLBACK

//
// String length for PortTest protocol identity
//
#define EHC_PORT_TEST_IDENTITY_LEN    64

//
//Iterate through the doule linked list. NOT delete safe
//
#define EFI_LIST_FOR_EACH(Entry, ListHead)    \
  for(Entry = (ListHead)->ForwardLink; Entry != (ListHead); Entry = Entry->ForwardLink)

//
//Iterate through the doule linked list. This is delete-safe.
//Don't touch NextEntry
//
#define EFI_LIST_FOR_EACH_SAFE(Entry, NextEntry, ListHead)            \
  for(Entry = (ListHead)->ForwardLink, NextEntry = Entry->ForwardLink;\
      Entry != (ListHead); Entry = NextEntry, NextEntry = Entry->ForwardLink)

#define EFI_LIST_CONTAINER(Entry, Type, Field) BASE_CR(Entry, Type, Field)


#define EHC_LOW_32BIT(Addr64)     ((UINT32)(((UINTN)(Addr64)) & 0XFFFFFFFF))
#define EHC_HIGH_32BIT(Addr64)    ((UINT32)(RShiftU64((UINTN)(Addr64), 32) & 0XFFFFFFFF))
#define EHC_BIT_IS_SET(Data, Bit) ((BOOLEAN)(((Data) & (Bit)) == (Bit)))

#define EHC_REG_BIT_IS_SET(Ehc, Offset, Bit) \
          (EHC_BIT_IS_SET(EhcReadOpReg ((Ehc), (Offset)), (Bit)))

#define USB2_HC_DEV_SIGNATURE  SIGNATURE_32 ('e', 'h', 'c', 'i')
#define EHC_FROM_THIS(a)       CR(a, USB2_HC_DEV, Usb2Hc, USB2_HC_DEV_SIGNATURE)

#define EHC_FROM_PORTTEST_THIS(a)   CR(a, USB2_HC_DEV, UsbPortTest, USB2_HC_DEV_SIGNATURE)

#define IS_NOT_32_BIT_ADDR(ADDRESS) ((((UINT64) ADDRESS) & (0xFFFFFFFF00000000)) != (UINT64) (0))


struct _USB2_HC_DEV {
  UINTN                     Signature;
  EFI_USB2_HC_PROTOCOL      Usb2Hc;

  EFI_PCI_IO_PROTOCOL       *PciIo;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINT64                    OriginalPciAttributes;
  USBHC_MEM_POOL            *MemPool;

  //
  // Schedule data shared between asynchronous and periodic
  // transfers:
  // ShortReadStop, as its name indicates, is used to terminate
  // the short read except the control transfer. EHCI follows
  // the alternative next QTD point when a short read happens.
  // For control transfer, even the short read happens, try the
  // status stage.
  //
  EHC_QTD                  *ShortReadStop;
  EFI_EVENT                 PollTimer;

  EFI_EVENT                 CheckAsyncBulkInTimer;

  EFI_EVENT                 CheckAsyncControlTimer;

  //
  // ExitBootServicesEvent is used to stop the EHC DMA operation
  // after exit boot service.
  //
  EFI_EVENT                 ExitBootServiceEvent;

  //
  // Asynchronous(bulk and control) transfer schedule data:
  // ReclaimHead is used as the head of the asynchronous transfer
  // list. It acts as the reclamation header.
  //
  EHC_QH                   *ReclaimHead;

  //
  // Peroidic (interrupt) transfer schedule data:
  //
  VOID                      *PeriodFrame;     // the buffer pointed by this pointer is used to store pci bus address of the QH descriptor.
  VOID                      *PeriodFrameHost; // the buffer pointed by this pointer is used to store host memory address of the QH descriptor.
  VOID                      *PeriodFrameMap;

  EHC_QH                    *PeriodOne;
  LIST_ENTRY                AsyncIntTransfers;

  LIST_ENTRY                AsyncBulkInTransfers;

  LIST_ENTRY                AsyncControlTransfers;
  //
  // EHCI configuration data
  //
  UINT32                    HcStructParams; // Cache of HC structure parameter, EHC_HCSPARAMS_OFFSET
  UINT32                    HcCapParams;    // Cache of HC capability parameter, HCCPARAMS
  UINT32                    CapLen;         // Capability length

  //
  // Misc
  //
  EFI_UNICODE_STRING_TABLE  *ControllerNameTable;

  QCOM_USB_CONFIG_PROTOCOL  *UsbConfig;

  // PortTest protocol instance per host controller
  QCOM_USB_PORT_TEST_PROTOCOL UsbPortTest;
  CHAR16                      UsbPortTestIdentity[EHC_PORT_TEST_IDENTITY_LEN];

  //
  // EHCI debug port info
  //
  UINT16                    DebugPortOffset; // The offset of debug port mmio register
  UINT8                     DebugPortBarNum; // The bar number of debug port mmio register
  UINT8                     DebugPortNum;    // The port number of usb debug port

  // Hardware Platform
  EFI_PLATFORMINFO_PLATFORM_INFO_TYPE   PlatformInfo;
  // Device Number
  UINTN CompanionDeviceNumber;
};


extern EFI_DRIVER_BINDING_PROTOCOL      gEhciDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL      gEhciComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL     gEhciComponentName2;

/**
  Test to see if this driver supports ControllerHandle. Any
  ControllerHandle that has Usb2HcProtocol installed will
  be supported.

  @param  This                 Protocol instance pointer.
  @param  Controller           Handle of device to test.
  @param  RemainingDevicePath  Not used.

  @return EFI_SUCCESS          This driver supports this device.
  @return EFI_UNSUPPORTED      This driver does not support this device.

**/
EFI_STATUS
EFIAPI
EhcDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

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
EhcDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

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
EhcDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  );

#endif

