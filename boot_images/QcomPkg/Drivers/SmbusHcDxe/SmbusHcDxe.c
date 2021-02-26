/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 */

#include "SmbusHcDxe.h"
#include <I2cMasterDxeApi.h>
#include <Protocol/EFIDALSYSProtocol.h>


typedef struct {
  VENDOR_DEVICE_PATH Vendor;
  CONTROLLER_DEVICE_PATH Controller;
  EFI_DEVICE_PATH End;
} I2CMASTER_VENDOR_DEVICE_PATH;

EFI_DRIVER_BINDING_PROTOCOL gSmbusHCDriverBinding = {
  SmbusHcDriverSupported,
  SmbusHcDriverStart,
  SmbusHcDriverStop,
  0x10,
  NULL,
  NULL
};

//
// Driver name table
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mSmbusHCDriverNameTable[] = {
  { "eng;en", L"Smbus HC Driver" },
  { NULL , NULL }
};

//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL  gSmbusHCComponentName = {
  (EFI_COMPONENT_NAME_GET_DRIVER_NAME) SmbusHcComponentNameGetDriverName,
  (EFI_COMPONENT_NAME_GET_CONTROLLER_NAME) SmbusHcComponentNameGetControllerName,
  "eng"
};

//
// EFI Component Name 2 Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL gSmbusHCComponentName2 = {
  SmbusHcComponentNameGetDriverName,
  SmbusHcComponentNameGetControllerName,
  "en"
};

/**
  Retrieves a Unicode string that is the user readable name of the driver.

  This function retrieves the user readable name of a driver in the form of a
  Unicode string. If the driver specified by This has a user readable name in
  the language specified by Language, then a pointer to the driver name is
  returned in DriverName, and EFI_SUCCESS is returned. If the driver specified
  by This does not support the language specified by Language,
  then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language. This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified
                                in RFC 4646 or ISO 639-2 language code format.

  @param  DriverName[out]       A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                driver specified by This in the language
                                specified by Language.

  @retval EFI_SUCCESS           The Unicode string for the Driver specified by
                                This and the language specified by Language was
                                returned in DriverName.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER DriverName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
SmbusHcComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  )
{
  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mSmbusHCDriverNameTable,
           DriverName,
           (BOOLEAN)(This != &gSmbusHCComponentName2)
           );
}

/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by a driver.

  This function retrieves the user readable name of the controller specified by
  ControllerHandle and ChildHandle in the form of a Unicode string. If the
  driver specified by This has a user readable name in the language specified by
  Language, then a pointer to the controller name is returned in ControllerName,
  and EFI_SUCCESS is returned.  If the driver specified by This is not currently
  managing the controller specified by ControllerHandle and ChildHandle,
  then EFI_UNSUPPORTED is returned.  If the driver specified by This does not
  support the language specified by Language, then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  ControllerHandle[in]  The handle of a controller that the driver
                                specified by This is managing.  This handle
                                specifies the controller whose name is to be
                                returned.

  @param  ChildHandle[in]       The handle of the child controller to retrieve
                                the name of.  This is an optional parameter that
                                may be NULL.  It will be NULL for device
                                drivers.  It will also be NULL for a bus drivers
                                that wish to retrieve the name of the bus
                                controller.  It will not be NULL for a bus
                                driver that wishes to retrieve the name of a
                                child controller.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language.  This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified in
                                RFC 4646 or ISO 639-2 language code format.

  @param  ControllerName[out]   A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                controller specified by ControllerHandle and
                                ChildHandle in the language specified by
                                Language from the point of view of the driver
                                specified by This.

  @retval EFI_SUCCESS           The Unicode string for the user readable name in
                                the language specified by Language for the
                                driver specified by This was returned in
                                DriverName.

  @retval EFI_INVALID_PARAMETER ControllerHandle is NULL.

  @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid
                                EFI_HANDLE.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER ControllerName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This is not currently
                                managing the controller specified by
                                ControllerHandle and ChildHandle.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
SmbusHcComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Tests to see if this driver supports a given controller. If a child device is provided,
  it further tests to see if this driver supports creating a handle for the specified child device.

  This function checks to see if the driver specified by This supports the device specified by
  ControllerHandle. Drivers will typically use the device path attached to
  ControllerHandle and/or the services from the bus I/O abstraction attached to
  ControllerHandle to determine if the driver supports ControllerHandle. This function
  may be called many times during platform initialization. In order to reduce boot times, the tests
  performed by this function must be very small, and take as little time as possible to execute. This
  function must not change the state of any hardware devices, and this function must be aware that the
  device specified by ControllerHandle may already be managed by the same driver or a
  different driver. This function must match its calls to AllocatePages() with FreePages(),
  AllocatePool() with FreePool(), and OpenProtocol() with CloseProtocol().
  Since ControllerHandle may have been previously started by the same driver, if a protocol is
  already in the opened state, then it must not be closed with CloseProtocol(). This is required
  to guarantee the state of ControllerHandle is not modified by this function.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to test. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For bus drivers, if this parameter is not NULL, then
                                   the bus driver must determine if the bus controller specified
                                   by ControllerHandle and the child controller specified
                                   by RemainingDevicePath are both supported by this
                                   bus driver.

  @retval EFI_SUCCESS              The device specified by ControllerHandle and
                                   RemainingDevicePath is supported by the driver specified by This.
  @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by the driver
                                   specified by This.
  @retval EFI_ACCESS_DENIED        The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by a different
                                   driver or an application that requires exclusive access.
                                   Currently not implemented.
  @retval EFI_UNSUPPORTED          The device specified by ControllerHandle and
                                   RemainingDevicePath is not supported by the driver specified by This.
**/
EFI_STATUS
EFIAPI
SmbusHcDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  EFI_STATUS                                    Status;

  //
  //  Locate I2C Master Protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiI2cMasterProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSmbusHcProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (!EFI_ERROR (Status)){
    return EFI_ALREADY_STARTED;
  }

  return EFI_SUCCESS;
}

/**
  Starts a device controller or a bus controller.

  The Start() function is designed to be invoked from the EFI boot service ConnectController().
  As a result, much of the error checking on the parameters to Start() has been moved into this
  common boot service. It is legal to call Start() from other locations,
  but the following calling restrictions must be followed, or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE.
  2. If RemainingDevicePath is not NULL, then it must be a pointer to a naturally aligned
     EFI_DEVICE_PATH_PROTOCOL.
  3. Prior to calling Start(), the Supported() function for the driver specified by This must
     have been called with the same calling parameters, and Supported() must have returned EFI_SUCCESS.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to start. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For a bus driver, if this parameter is NULL, then handles
                                   for all the children of Controller are created by this driver.
                                   If this parameter is not NULL and the first Device Path Node is
                                   not the End of Device Path Node, then only the handle for the
                                   child device specified by the first Device Path Node of
                                   RemainingDevicePath is created by this driver.
                                   If the first Device Path Node of RemainingDevicePath is
                                   the End of Device Path Node, no child handle is created by this
                                   driver.

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
  @retval Others                   The driver failded to start the device.

**/
EFI_STATUS
EFIAPI
SmbusHcDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  )
{
  EFI_STATUS                                          Status;
  EFI_I2C_MASTER_PROTOCOL                             *I2cMaster;
  SMBUS_HC_CONTEXT                                    *SmbusHCContext;

  I2cMaster                     = NULL;
  SmbusHCContext                = NULL;


  //
  // Locate I2C Master Protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiI2cMasterProtocolGuid,
                  (VOID **)&I2cMaster,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "I2cHost: Open I2C master error, Status = %r\n", Status));
    goto Exit;
  }

  //
  // Allocate the Smbus Host Context structure
  //
  SmbusHCContext = AllocateZeroPool (sizeof (SMBUS_HC_CONTEXT));
  if (SmbusHCContext == NULL) {
    DEBUG ((EFI_D_ERROR, "SmbusHC: there is no enough memory to allocate.\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Initialize the context structure for the current I2C Controller
  //
  SmbusHCContext->Signature                     = SMBUS_HC_SIGNATURE;
  SmbusHCContext->I2cMaster                     = I2cMaster;

  //
  // Reset the controller
  //
  Status = I2cMaster->Reset (I2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmbusHC: I2C controller reset failed!\n"));
    goto Exit;
  }

  //
  // Set the controller frequency
  //
  UINTN BusClockHertz = SMBUS_HC_DEFAULT_I2C_FREQ;
  Status = I2cMaster->SetBusFrequency(I2cMaster, &BusClockHertz);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmbusHC: I2C controller set bus frequency failed\n"));
    goto Exit;
  }

  //
  // Build the Smbus HC protocol for the current I2C controller
  //
  SmbusHCContext->SmbusHC.Execute   = Execute;
  SmbusHCContext->SmbusHC.ArpDevice = ArpDevice;
  SmbusHCContext->SmbusHC.GetArpMap = GetArpMap;
  SmbusHCContext->SmbusHC.Notify    = Notify;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiSmbusHcProtocolGuid,
                  &SmbusHCContext->SmbusHC,
                  NULL
                  );

  EFI_DEVICE_PATH_PROTOCOL    *DevicePath = NULL;
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&DevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmbusHC: OpenProtocol() failed\n"));
    goto Exit;
  }
  SmbusHCContext->Instance = (i2c_instance) ((I2CMASTER_VENDOR_DEVICE_PATH *)DevicePath)->Controller.ControllerNumber;


Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmbusHC: Start() function failed, Status = %r\n", Status));

    gBS->CloseProtocol (
                    Controller,
                    &gEfiI2cMasterProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );
    //
    //  Release the context structure upon failure
    //
    if (SmbusHCContext != NULL) {
      FreePool (SmbusHCContext);
    }
  }

  //
  //  Return the operation status.
  //
  return Status;
}

/**
  Stops a device controller or a bus controller.

  The Stop() function is designed to be invoked from the EFI boot service DisconnectController().
  As a result, much of the error checking on the parameters to Stop() has been moved
  into this common boot service. It is legal to call Stop() from other locations,
  but the following calling restrictions must be followed, or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE that was used on a previous call to this
     same driver's Start() function.
  2. The first NumberOfChildren handles of ChildHandleBuffer must all be a valid
     EFI_HANDLE. In addition, all of these handles must have been created in this driver's
     Start() function, and the Start() function must have called OpenProtocol() on
     ControllerHandle with an Attribute of EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER.

  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle must
                                support a bus specific I/O protocol for the driver
                                to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL
                                if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The device was stopped.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.

**/
EFI_STATUS
EFIAPI
SmbusHcDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
  )
{
  EFI_STATUS                  Status;
  SMBUS_HC_CONTEXT            *SmbusHCContext;
  EFI_SMBUS_HC_PROTOCOL       *SmbusHC;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSmbusHcProtocolGuid,
                  (VOID **) &SmbusHC,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  SmbusHCContext = SMBUS_HC_CONTEXT_FROM_PROTOCOL (SmbusHC);

  //
  // If there is pending request or pending bus configuration, do not stop
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  Controller,
                  &gEfiSmbusHcProtocolGuid,
                  SmbusHC,
                  NULL
                  );

  if (!EFI_ERROR (Status)) {
    gBS->CloseProtocol (
                    Controller,
                    &gEfiI2cMasterProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );
    FreePool (SmbusHCContext);
  }

  //
  //  Return the stop status
  //
  return Status;
}

/**

  The Execute() function provides a standard way to execute an
  operation as defined in the System Management Bus (SMBus)
  Specification. The resulting transaction will be either that
  the SMBus slave devices accept this transaction or that this
  function returns with error.

  @param This     A pointer to the EFI_SMBUS_HC_PROTOCOL instance.
                  SlaveAddress The SMBus slave address of the device
                  with which to communicate. Type
                  EFI_SMBUS_DEVICE_ADDRESS is defined in
                  EFI_PEI_SMBUS_PPI.Execute() in the Platform
                  Initialization SMBus PPI Specification.

  @param Command  This command is transmitted by the SMBus host
                  controller to the SMBus slave device and the
                  interpretation is SMBus slave device specific.
                  It can mean the offset to a list of functions
                  inside an SMBus slave device. Not all
                  operations or slave devices support this
                  command's registers. Type
                  EFI_SMBUS_DEVICE_COMMAND is defined in
                  EFI_PEI_SMBUS_PPI.Execute() in the Platform
                  Initialization SMBus PPI Specification.

 @param Operation Signifies the particular SMBus
                  hardware protocol instance it will use to
                  execute the SMBus transactions. This SMBus
                  hardware protocol is defined by the SMBus
                  Specification and is not related to PI
                  Architecture. Type EFI_SMBUS_OPERATION is
                  defined in EFI_PEI_SMBUS_PPI.Execute() in the
                  Platform Initialization SMBus PPI
                  Specification.

  @param PecCheck Defines if Packet Error Code (PEC) checking
                  is required for this operation. SMBus Host
                  Controller Code Definitions Version 1.0
                  August 21, 2006 13

 @param Length    Signifies the number of bytes that this operation will do.
                  The maximum number of bytes can be revision
                  specific and operation specific. This field
                  will contain the actual number of bytes that
                  are executed for this operation. Not all
                  operations require this argument.

  @param Buffer   Contains the value of data to execute to the
                  SMBus slave device. Not all operations require
                  this argument. The length of this buffer is
                  identified by Length.


  @retval EFI_SUCCESS           The last data that was returned from the
                                access matched the poll exit criteria.

  @retval EFI_CRC_ERROR         Checksum is not correct (PEC is incorrect).

  @retval EFI_TIMEOUT           Timeout expired before the operation was
                                completed. Timeout is determined by the
                                SMBus host controller device.

  @retval EFI_OUT_OF_RESOURCES  The request could not be
                                completed due to a lack of
                                resources.

  @retval EFI_DEVICE_ERROR      The request was not completed
                                because a failure that was reflected
                                in the Host Status Register bit.
                                Device errors are a result of a
                                transaction collision, illegal
                                command field, unclaimed cycle (host
                                initiated), or bus errors
                                (collisions).

  @retval EFI_INVALID_PARAMETER Operation is not defined in
                                EFI_SMBUS_OPERATION.

  @retval EFI_INVALID_PARAMETER Length/Buffer is NULL for
                                operations except for
                                EfiSmbusQuickRead and
                                EfiSmbusQuickWrite. Length is
                                outside the range of valid
                                values.

  @retval EFI_UNSUPPORTED       The SMBus operation or PEC is not
                                supported.

  @retval EFI_BUFFER_TOO_SMALL  Buffer is not sufficient for
                                this operation.

**/
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
){
  // Send request packet to I2C Host Controller to process
  SMBUS_HC_CONTEXT *SmbusHcContext = SMBUS_HC_CONTEXT_FROM_PROTOCOL(This);
  return SmbusInitiateTransfer(SmbusHcContext->Instance,
                               SlaveAddress,
                               Command,
                               Operation,
                               PecCheck,
                               Length,
                               Buffer);
}


/**

  The ArpDevice() function provides a standard way for a device driver to
  enumerate the entire SMBus or specific devices on the bus.

  @param This           A pointer to the EFI_SMBUS_HC_PROTOCOL instance.

  @param ArpAll         A Boolean expression that indicates if the
                        host drivers need to enumerate all the devices
                        or enumerate only the device that is
                        identified by SmbusUdid. If ArpAll is TRUE,
                        SmbusUdid and SlaveAddress are optional. If
                        ArpAll is FALSE, ArpDevice will enumerate
                        SmbusUdid and the address will be at
                        SlaveAddress.

  @param SmbusUdid      The Unique Device Identifier (UDID) that is
                        associated with this device. Type
                        EFI_SMBUS_UDID is defined in
                        EFI_PEI_SMBUS_PPI.ArpDevice() in the
                        Platform Initialization SMBus PPI
                        Specification.

  @param SlaveAddress   The SMBus slave address that is
                        associated with an SMBus UDID.

  @retval EFI_SUCCESS           The last data that was returned from the
                                access matched the poll exit criteria.

  @retval EFI_CRC_ERROR         Checksum is not correct (PEC is
                                incorrect).

  @retval EFI_TIMEOUT           Timeout expired before the operation was
                                completed. Timeout is determined by the
                                SMBus host controller device.

  @retval EFI_OUT_OF_RESOURCES  The request could not be
                                completed due to a lack of
                                resources.

  @retval EFI_DEVICE_ERROR      The request was not completed
                                because a failure was reflected in
                                the Host Status Register bit. Device
                                Errors are a result of a transaction
                                collision, illegal command field,
                                unclaimed cycle (host initiated), or
                                bus errors (collisions).

  @retval EFI_UNSUPPORTED       ArpDevice, GetArpMap, and Notify are
                                not implemented by this driver.

**/
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


/**
  The GetArpMap() function returns the mapping of all the SMBus devices
  that were enumerated by the SMBus host driver.

  @param This           A pointer to the EFI_SMBUS_HC_PROTOCOL instance.

  @param Length         Size of the buffer that contains the SMBus
                        device map.

  @param SmbusDeviceMap The pointer to the device map as
                        enumerated by the SMBus controller
                        driver.

  @retval EFI_SUCCESS       The SMBus returned the current device map.

  @retval EFI_UNSUPPORTED   ArpDevice, GetArpMap, and Notify are
                            not implemented by this driver.

**/
EFI_STATUS
EFIAPI
GetArpMap (
  IN CONST  EFI_SMBUS_HC_PROTOCOL   *This,
  IN OUT    UINTN                   *Length,
  IN OUT    EFI_SMBUS_DEVICE_MAP    **SmbusDeviceMap
){
  return EFI_UNSUPPORTED;
}


/**
  The notify function does some actions.

  @param SlaveAddress
  The SMBUS hardware address to which the SMBUS device is preassigned or allocated.

  @param Data
  Data of the SMBus host notify command that the caller wants to be called.

  @return EFI_STATUS
**/
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

/**
  Initialize the state information for the SMBus Host Controller Protocol

  @param  ImageHandle   of the loaded driver
  @param  SystemTable   Pointer to the System Table

  @retval EFI_SUCCESS           Protocol registered
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate protocol data structureEFI
  @retval EFI_DEVICE_ERROR      Hardware problems

**/
EFI_STATUS
EFIAPI
SmbusHcEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable)
{
  EFI_STATUS Status;
  // Init DALSys
  DALSYS_InitMod(NULL);
  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gSmbusHCDriverBinding,
             ImageHandle,
             &gSmbusHCComponentName,
             &gSmbusHCComponentName2
             );
  ASSERT_EFI_ERROR (Status);
  return Status;
}
