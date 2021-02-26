/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 */
#include <SmbusSharedLib.h>

#define SMBUS_HC_SIGNATURE            SIGNATURE_32 ('S', 'M', 'B', 'H')
#define SMBUS_HC_CONTEXT_FROM_PROTOCOL(a) CR (a, SMBUS_HC_CONTEXT, SmbusHC, SMBUS_HC_SIGNATURE)

#define SMBUS_HC_DEFAULT_I2C_FREQ 100000


//
// Smbus host context
//
typedef struct {
  //
  // Structure identification
  //
  UINTN Signature;

  //
  // Upper level API
  //
  EFI_SMBUS_HC_PROTOCOL       SmbusHC;

  //
  // Lower level API for I2C master (controller)
  //
  EFI_I2C_MASTER_PROTOCOL *I2cMaster;

  // Instance
  i2c_instance             Instance;
} SMBUS_HC_CONTEXT;


EFI_STATUS
EFIAPI
SmbusHcComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
);


EFI_STATUS
EFIAPI
SmbusHcComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
);


EFI_STATUS
EFIAPI
SmbusHcDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
);


EFI_STATUS
EFIAPI
SmbusHcDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
);


EFI_STATUS
EFIAPI
SmbusHcDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
);


EFI_STATUS
EFIAPI
Execute (
  IN CONST        EFI_SMBUS_HC_PROTOCOL         *This,
  IN              EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN              EFI_SMBUS_DEVICE_COMMAND      Command,
  IN              EFI_SMBUS_OPERATION           Operation,
  IN              BOOLEAN                       PecCheck,
  IN OUT  UINTN                                 *Length,
  IN OUT  VOID                                  *Buffer
);


EFI_STATUS
EFIAPI
ArpDevice(
  IN CONST    EFI_SMBUS_HC_PROTOCOL     *This,
  IN          BOOLEAN                   ArpAll,
  IN          EFI_SMBUS_UDID            *SmbusUdid, OPTIONAL
  IN OUT      EFI_SMBUS_DEVICE_ADDRESS  *SlaveAddress OPTIONAL
);


EFI_STATUS
EFIAPI
GetArpMap (
  IN CONST  EFI_SMBUS_HC_PROTOCOL   *This,
  IN OUT    UINTN                   *Length,
  IN OUT    EFI_SMBUS_DEVICE_MAP    **SmbusDeviceMap
);


EFI_STATUS
EFIAPI
Notify (
  IN  CONST EFI_SMBUS_HC_PROTOCOL       *This,
  IN        EFI_SMBUS_DEVICE_ADDRESS    SlaveAddress,
  IN        UINTN                       Data,
  IN        EFI_SMBUS_NOTIFY_FUNCTION   NotifyFunction
);

/**
  SMBus DXE Driver Entry Point
 */
EFI_STATUS
SmbusProtocolInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
);
