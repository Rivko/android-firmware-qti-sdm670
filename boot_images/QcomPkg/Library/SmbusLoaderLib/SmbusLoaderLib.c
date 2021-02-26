/** @file SmbusLoaderLib.c

  Smbus Loader Lib code

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential

**/
#include <SmbusSharedLib.h>

EFI_STATUS
EFIAPI
Execute (
  IN              i2c_instance                  Instance,
  IN              EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN              EFI_SMBUS_DEVICE_COMMAND      Command,
  IN              EFI_SMBUS_OPERATION           Operation,
  IN              BOOLEAN                       PecCheck,
  IN OUT  UINTN                                 *Length,
  IN OUT  VOID                                  *Buffer
){
  return SmbusInitiateTransfer(Instance,
                               SlaveAddress,
                               Command,
                               Operation,
                               PecCheck,
                               Length,
                               Buffer);
}


EFI_STATUS
EFIAPI
ArpDevice(
  IN CONST    EFI_SMBUS_HC_PROTOCOL     *This,
  IN          BOOLEAN                   ArpAll,
  IN          EFI_SMBUS_UDID            *SmbusUdid, OPTIONAL
  IN OUT      EFI_SMBUS_DEVICE_ADDRESS  *SlaveAddress OPTIONAL
){
  return EFI_UNSUPPORTED;
}


EFI_STATUS
EFIAPI
GetArpMap (
  IN CONST  EFI_SMBUS_HC_PROTOCOL   *This,
  IN OUT    UINTN                   *Length,
  IN OUT    EFI_SMBUS_DEVICE_MAP    **SmbusDeviceMap
){
  return EFI_UNSUPPORTED;
}


EFI_STATUS
EFIAPI
Notify (
  IN  CONST EFI_SMBUS_HC_PROTOCOL       *This,
  IN        EFI_SMBUS_DEVICE_ADDRESS    SlaveAddress,
  IN        UINTN                       Data,
  IN        EFI_SMBUS_NOTIFY_FUNCTION   NotifyFunction
){
  return EFI_UNSUPPORTED;
}

