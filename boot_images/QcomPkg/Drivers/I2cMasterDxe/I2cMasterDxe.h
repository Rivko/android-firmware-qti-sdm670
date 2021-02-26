/** @file I2cMasterDxe.h

  I2cMaster Driver header file

  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential

**/


#ifndef _I2CMASTERDXE_H_
#define _I2CMASTERDXE_H_

#include <Protocol/EFII2C.h>

// {1161201C-BCF2-4E75-A5C0-92C3EF48F85F}
#define EFI_I2C_MASTER_GUID \
  { 0x1161201c, 0xbcf2, 0x4e75, { 0xa5, 0xc0, 0x92, 0xc3, 0xef, 0x48, 0xf8, 0x5f } }

extern EFI_GUID gQcomI2cMasterGuid;
extern EFI_GUID gQcomI2CProtocolGuid;

#define I2C_MASTER_SIGNATURE            SIGNATURE_32 ('I', '2', 'C', 'M')
#define I2C_MASTER_CONTEXT_FROM_PROTOCOL(a) CR (a, I2C_MASTER_CONTEXT, I2cMaster, I2C_MASTER_SIGNATURE)

//
// Smbus host context
//
typedef struct {
  //
  // Structure identification
  //
  UINTN Signature;

  //
  // I2C Handle
  //
  VOID                         *I2cHandle;

  //
  // I2C Configuration
  //
  i2c_config                   *I2cConfig;

  //
  // Upper level API
  //
  EFI_I2C_MASTER_PROTOCOL      I2cMaster;

  //
  // Lower level API for I2C
  //
  EFI_QCOM_I2C_PROTOCOL       *I2cIo;
} I2C_MASTER_CONTEXT;


EFI_STATUS
EFIAPI
I2cMasterComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
);


EFI_STATUS
EFIAPI
I2cMasterComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
);


EFI_STATUS
EFIAPI
I2cMasterDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
);


EFI_STATUS
EFIAPI
I2cMasterDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
);


EFI_STATUS
EFIAPI
I2cMasterDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
);

EFI_STATUS
EFIAPI
I2cMasterSetBusFrequency (
  IN CONST EFI_I2C_MASTER_PROTOCOL   *This,
  IN OUT UINTN                       *BusClockHertz
);

EFI_STATUS
EFIAPI
I2cMasterReset (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This
);

EFI_STATUS
EFIAPI
I2cMasterStartRequest (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This,
  IN UINTN                         SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET        *RequestPacket,
  IN EFI_EVENT                     Event      OPTIONAL,
  OUT EFI_STATUS                   *I2cStatus OPTIONAL
);

#endif /* _I2CMASTERDXE_H_ */
