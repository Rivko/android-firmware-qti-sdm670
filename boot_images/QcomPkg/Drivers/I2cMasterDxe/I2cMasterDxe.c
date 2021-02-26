/** @file I2cMasterDxe.c

  I2cMasterDxe UEFI Driver code

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential

**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/DevicePath.h>
#include <Protocol/EFIClock.h>
#include <Protocol/I2cMaster.h>
#include <Guid/EventGroup.h>

#include <SmbusSharedLib.h>
#include "I2cMasterDxe.h"
#include <I2cMasterDxeApi.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define I2C_TIMEOUT_MS 5

/* I2C Vendor Device Path */
typedef struct {
  VENDOR_DEVICE_PATH  Vendor;
  EFI_DEVICE_PATH     End;
} I2CMASTER_VENDOR_DEVICE_PATH;

//
// Template for vendor device path node.
//
I2CMASTER_VENDOR_DEVICE_PATH gVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ( (sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    EFI_I2C_MASTER_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      sizeof (EFI_DEVICE_PATH_PROTOCOL),
      0
    }
  }
};

//
// Template for controller device path node.
//
CONTROLLER_DEVICE_PATH gControllerDeviceNodeTemplate = {
  {
    HARDWARE_DEVICE_PATH,
    HW_CONTROLLER_DP,
    {
      (UINT8) (sizeof (CONTROLLER_DEVICE_PATH)),
      (UINT8) ( (sizeof (CONTROLLER_DEVICE_PATH)) >> 8)
    }
  },
  0
};


EFI_DRIVER_BINDING_PROTOCOL gI2cMasterDriverBinding = {
  I2cMasterDriverSupported,
  I2cMasterDriverStart,
  I2cMasterDriverStop,
  0x10,
  NULL,
  NULL
};

//
// Driver name table
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mI2cMasterDriverNameTable[] = {
  { "eng;en", L"I2c Master Protocol Driver" },
  { NULL , NULL }
};

//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL  gI2cMasterComponentName = {
  (EFI_COMPONENT_NAME_GET_DRIVER_NAME) I2cMasterComponentNameGetDriverName,
  (EFI_COMPONENT_NAME_GET_CONTROLLER_NAME) I2cMasterComponentNameGetControllerName,
  "eng"
};

//
// EFI Component Name 2 Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL gI2cMasterComponentName2 = {
  I2cMasterComponentNameGetDriverName,
  I2cMasterComponentNameGetControllerName,
  "en"
};

/**
  Get the I2c_Handle from the protocol
  */
EFI_STATUS
I2cMasterGetHandle(
  IN  EFI_I2C_MASTER_PROTOCOL *This,
  OUT VOID                    *I2cHandle)
{
  I2C_MASTER_CONTEXT *I2cMasterContext = I2C_MASTER_CONTEXT_FROM_PROTOCOL(This);
  I2cHandle = I2cMasterContext->I2cHandle;
  return EFI_SUCCESS;
}
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
I2cMasterComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  )
{
  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mI2cMasterDriverNameTable,
           DriverName,
           (BOOLEAN)(This != &gI2cMasterComponentName2)
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
I2cMasterComponentNameGetControllerName (
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
I2cMasterDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  EFI_STATUS                                    Status;
  UINTN                                         i;
  EFI_DEVICE_PATH_PROTOCOL                      *DevicePath;
  EFI_HANDLE                                    Handle;

  //
  //  Locate I2C Protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gQcomI2CProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (i=0; i<I2C_INSTANCE_MAX; i++){
    gControllerDeviceNodeTemplate.ControllerNumber = i;
    DevicePath = AppendDevicePathNode ( (EFI_DEVICE_PATH_PROTOCOL *) &gVendorDevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &gControllerDeviceNodeTemplate);
    Status = gBS->LocateDevicePath(&gEfiI2cMasterProtocolGuid, &DevicePath, &Handle);
    if(!EFI_ERROR(Status)){
        return EFI_ALREADY_STARTED;
    }
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
I2cMasterDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  )
{
  EFI_STATUS                                          Status;
  EFI_DEVICE_PATH_PROTOCOL                            *DevicePath;
  EFI_QCOM_I2C_PROTOCOL                               *I2cIo;
  EFI_I2C_MASTER_PROTOCOL                             *I2cMaster;
  I2C_MASTER_CONTEXT                                  *I2cMasterContext;
  i2c_config                                          *I2cConfig;
  VOID                                                *I2cHandle;
  EFI_HANDLE                                          ChildHandle;
  UINTN                                               i;
  i2c_status                                          I2cStatus;

  //
  // Loop through the list of instances
  //
  for (i = 0; i < I2C_INSTANCE_MAX; i++) {
    I2cIo                           = NULL;
    I2cMasterContext                = NULL;
    I2cConfig                       = NULL;
    I2cMaster                       = NULL;
    I2cHandle                       = NULL;
    ChildHandle                     = NULL;

    //
    // Allocate the I2cMaster Context structure
    //
    I2cMasterContext = AllocateZeroPool (sizeof (I2C_MASTER_CONTEXT));
    if (I2cMasterContext == NULL) {
      DEBUG ((EFI_D_ERROR, "I2cMaster: there is no enough memory to allocate.\n"));
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }

    //
    // Allocate the I2cConfig structure
    //
    I2cConfig = AllocateZeroPool (sizeof (i2c_config));
    if (I2cConfig == NULL) {
      DEBUG ((EFI_D_ERROR, "I2cMaster: there is no enough memory to allocate.\n"));
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }

    //
    // Initialize the context structure for the current I2C Controller
    //
    I2cMasterContext->Signature                     = I2C_MASTER_SIGNATURE;
    I2cMasterContext->I2cConfig                     = I2cConfig;

    //
    // Set the default controller frequency, slave address, and slave type
    //
    I2cConfig->bus_frequency_khz    = QUPE_DEFAULT_BUS_FREQ_KHZ;
    I2cConfig->slave_address        = 0;
    I2cConfig->slave_address_type   = I2C_07_BIT_SLAVE_ADDRESS;

    //
    // Build the I2c Master protocol for the current I2C controller
    //
    I2cMasterContext->I2cMaster.SetBusFrequency = I2cMasterSetBusFrequency;
    I2cMasterContext->I2cMaster.Reset           = I2cMasterReset;
    I2cMasterContext->I2cMaster.StartRequest    = I2cMasterStartRequest;


    //
    // Create the new device path
    //
    gControllerDeviceNodeTemplate.ControllerNumber = i;
    DevicePath = AppendDevicePathNode ( (EFI_DEVICE_PATH_PROTOCOL *) &gVendorDevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &gControllerDeviceNodeTemplate);

   //
    // Install the protocol on a new handle
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
        &ChildHandle,
        &gEfiI2cMasterProtocolGuid, &I2cMasterContext->I2cMaster,
        &gEfiDevicePathProtocolGuid, DevicePath,
        NULL
        );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "I2cHost: Unable to install I2cMaster and DevicePathProtocol onto child handle, Status = %r\n", Status));
      goto Exit;
    }

    //
    // Locate I2C Protocol
    //
    Status = gBS->OpenProtocol (
        Controller,
        &gQcomI2CProtocolGuid,
        (VOID **)&I2cMasterContext->I2cIo,
        This->DriverBindingHandle,
        ChildHandle,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
        );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "I2cHost: Open I2C protocol on child handle error, Status = %r\n", Status));
      goto Exit;
    }

    //
    // Open an I2cHandle
    //
    I2cStatus = I2cMasterContext->I2cIo->open (i, &I2cMasterContext->I2cHandle);
    if(I2cStatus != I2C_SUCCESS) {
      DEBUG ((EFI_D_INFO, "I2cHost: Unable to open device %d", i));
      gBS->CloseProtocol (
               Controller,
               &gQcomI2CProtocolGuid,
               This->DriverBindingHandle,
               ChildHandle
               );
      gBS->UninstallMultipleProtocolInterfaces (
               ChildHandle,
               &gEfiI2cMasterProtocolGuid, &I2cMasterContext->I2cMaster,
               &gEfiDevicePathProtocolGuid, DevicePath,
               NULL
               );
      FreePool(I2cConfig);
      FreePool(I2cMasterContext);
      continue;
    }
    else {
      //Successfully created protocol, call ConnectController on the ChildHandle to connect dependencies
      Status = gBS->ConnectController (ChildHandle, NULL, NULL, TRUE);
    }

  }

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "I2cMaster: Start() function failed, Status = %r\n", Status));

    gBS->CloseProtocol (
                    Controller,
                    &gEfiI2cMasterProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );
    //
    //  Release the context structure upon failure
    //
    if (I2cMasterContext != NULL) {
      FreePool (I2cMasterContext);
    }
    if (I2cConfig != NULL) {
      FreePool (I2cConfig);
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
I2cMasterDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    ChildHandle;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  I2C_MASTER_CONTEXT            *I2cMasterContext;
  EFI_I2C_MASTER_PROTOCOL       *I2cMaster;
  UINTN                         i;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gQcomI2CProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  if (!EFI_ERROR (Status)) {
    for (i=0; i<I2C_INSTANCE_MAX; i++){
      gControllerDeviceNodeTemplate.ControllerNumber = i;
      DevicePath = AppendDevicePathNode ( (EFI_DEVICE_PATH_PROTOCOL *) &gVendorDevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &gControllerDeviceNodeTemplate);
      Status = gBS->LocateDevicePath(&gEfiI2cMasterProtocolGuid, &DevicePath, &ChildHandle);
      if(EFI_ERROR(Status)){
        continue;
      }
      Status = gBS->HandleProtocol(ChildHandle, &gEfiI2cMasterProtocolGuid, (VOID **)&I2cMaster);
      if(EFI_ERROR(Status)){
        continue;
      }
      I2cMasterContext = I2C_MASTER_CONTEXT_FROM_PROTOCOL (I2cMaster);
      gBS->CloseProtocol (
               Controller,
               &gQcomI2CProtocolGuid,
               This->DriverBindingHandle,
               ChildHandle
               );
      gBS->UninstallMultipleProtocolInterfaces (
               ChildHandle,
               &gEfiI2cMasterProtocolGuid, &I2cMasterContext->I2cMaster,
               &gEfiDevicePathProtocolGuid, DevicePath,
               NULL
               );
      FreePool (I2cMasterContext->I2cConfig);
      FreePool (I2cMasterContext);
    }
  }

  //
  //  Return the stop status
  //
  return EFI_SUCCESS;
}



/**
  Set the frequency for the I2C clock line.

  This routine must be called at or below TPL_CALLBACK.

  The software and controller do a best case effort of using the specified
  frequency for the I2C bus.  If the frequency does not match exactly then
  the I2C master protocol selects the next lower frequency to avoid
  exceeding the operating conditions for any of the I2C devices on the bus.
  For example if 400 KHz was specified and the controller's divide network
  only supports 402 KHz or 398 KHz then the I2C master protocol selects 398
  KHz.  If there are not lower frequencies available, then return
  EFI_UNSUPPORTED.

  @param[in] This           Pointer to an EFI_I2C_MASTER_PROTOCOL structure
  @param[in] BusClockHertz  Pointer to the requested I2C bus clock frequency
          in Hertz.  Upon return this value contains the
          actual frequency in use by the I2C controller.

  @retval EFI_SUCCESS           The bus frequency was set successfully.
  @retval EFI_ALREADY_STARTED   The controller is busy with another transaction.
  @retval EFI_INVALID_PARAMETER BusClockHertz is NULL
  @retval EFI_UNSUPPORTED       The controller does not support this frequency.

**/
EFI_STATUS
EFIAPI
I2cMasterSetBusFrequency (
  IN CONST EFI_I2C_MASTER_PROTOCOL   *This,
  IN OUT UINTN                       *BusClockHertz
)
{
  if (BusClockHertz == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  else if ((UINT32) *BusClockHertz != *BusClockHertz) {
    return EFI_UNSUPPORTED;
  }

  I2C_MASTER_CONTEXT *I2cMasterContext = I2C_MASTER_CONTEXT_FROM_PROTOCOL(This);
  I2cMasterContext->I2cConfig->bus_frequency_khz  = *BusClockHertz/1000;

  return EFI_SUCCESS;
}

/**
  Reset the I2C controller and configure it for use

  This routine must be called at or below TPL_CALLBACK.

  The I2C controller is reset.  The caller must call SetBusFrequench() after
  calling Reset().

  @param[in]     This       Pointer to an EFI_I2C_MASTER_PROTOCOL structure.

  @retval EFI_SUCCESS         The reset completed successfully.
  @retval EFI_ALREADY_STARTED The controller is busy with another transaction.
  @retval EFI_DEVICE_ERROR    The reset operation failed.

**/
EFI_STATUS
EFIAPI
I2cMasterReset (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This
)
{
  return EFI_SUCCESS;
}

/**
  Start an I2C transaction on the host controller.

  This routine must be called at or below TPL_CALLBACK.  For synchronous
  requests this routine must be called at or below TPL_CALLBACK.

  This function initiates an I2C transaction on the controller.  To
  enable proper error handling by the I2C protocol stack, the I2C
  master protocol does not support queuing but instead only manages
  one I2C transaction at a time.  This API requires that the I2C bus
  is in the correct configuration for the I2C transaction.

  The transaction is performed by sending a start-bit and selecting the
  I2C device with the specified I2C slave address and then performing
  the specified I2C operations.  When multiple operations are requested
  they are separated with a repeated start bit and the slave address.
  The transaction is terminated with a stop bit.

  When Event is NULL, StartRequest operates synchronously and returns
  the I2C completion status as its return value.

  When Event is not NULL, StartRequest synchronously returns EFI_SUCCESS
  indicating that the I2C transaction was started asynchronously.  The
  transaction status value is returned in the buffer pointed to by
  I2cStatus upon the completion of the I2C transaction when I2cStatus
  is not NULL.  After the transaction status is returned the Event is
  signaled.

  Note: The typical consumer of this API is the I2C host protocol.
  Extreme care must be taken by other consumers of this API to prevent
  confusing the third party I2C drivers due to a state change at the
  I2C device which the third party I2C drivers did not initiate.  I2C
  platform specific code may use this API within these guidelines.

  @param[in] This           Pointer to an EFI_I2C_MASTER_PROTOCOL structure.
  @param[in] SlaveAddress   Address of the device on the I2C bus.  Set the
          I2C_ADDRESSING_10_BIT when using 10-bit addresses,
          clear this bit for 7-bit addressing.  Bits 0-6
          are used for 7-bit I2C slave addresses and bits
          0-9 are used for 10-bit I2C slave addresses.
  @param[in] RequestPacket  Pointer to an EFI_I2C_REQUEST_PACKET
          structure describing the I2C transaction.
  @param[in] Event          Event to signal for asynchronous transactions,
          NULL for asynchronous transactions
  @param[out] I2cStatus     Optional buffer to receive the I2C transaction
          completion status

  @retval EFI_SUCCESS           The asynchronous transaction was successfully
        started when Event is not NULL.
  @retval EFI_SUCCESS           The transaction completed successfully when
        Event is NULL.
  @retval EFI_ALREADY_STARTED   The controller is busy with another transaction.
  @retval EFI_BAD_BUFFER_SIZE   The RequestPacket->LengthInBytes value is too
        large.
  @retval EFI_DEVICE_ERROR      There was an I2C error (NACK) during the
        transaction.
  @retval EFI_INVALID_PARAMETER RequestPacket is NULL
  @retval EFI_NOT_FOUND         Reserved bit set in the SlaveAddress parameter
  @retval EFI_NO_RESPONSE       The I2C device is not responding to the slave
        address.  EFI_DEVICE_ERROR will be returned if
        the controller cannot distinguish when the NACK
        occurred.
  @retval EFI_OUT_OF_RESOURCES  Insufficient memory for I2C transaction
  @retval EFI_UNSUPPORTED       The controller does not support the requested
        transaction.

**/
EFI_STATUS
EFIAPI
I2cMasterStartRequest (
  IN CONST EFI_I2C_MASTER_PROTOCOL *This,
  IN UINTN                         SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET        *RequestPacket,
  IN EFI_EVENT                     Event      OPTIONAL,
  OUT EFI_STATUS                   *I2cStatus OPTIONAL
)
{
  EFI_STATUS         Status;
  i2c_status         I2c_Status;
  UINTN              i;
  UINTN              OperationCount;
  UINT32             Written;
  UINT32             Read;
  i2c_transfer_list  Transfers[2];
  UINTN              Length;

  if (NULL == RequestPacket){
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }

  I2C_MASTER_CONTEXT *I2cMasterContext = I2C_MASTER_CONTEXT_FROM_PROTOCOL(This);
  OperationCount = RequestPacket->OperationCount;
  ASSERT(OperationCount <= 2);

  ConvertRequestPacketToTransferPacket(RequestPacket, (i2c_transfer_list*)Transfers);

  //Set the slave address
  I2cMasterContext->I2cConfig->slave_address = SlaveAddress;

  I2c_Status = I2cMasterContext->I2cIo->transfer(
                            I2cMasterContext->I2cHandle,
                            I2cMasterContext->I2cConfig,
                            Transfers,
                            OperationCount,
                            &Written,
                            &Read,
                            I2C_TIMEOUT_MS);

  // Convert from i2c_status to EFI_STATUS
  Status = I2cStatusToEfiStatus(I2c_Status);
  if(!EFI_ERROR(Status)){
    if (OperationCount != 1) {
      for (i = 0; i < OperationCount; i++){
        Length = Transfers[i].desc->length;
        RequestPacket->Operation[1].LengthInBytes = min(Length, max(Written, Read));
      }
    }
  }

  if (NULL != I2cStatus) {
    *I2cStatus = Status;
  }

  if (NULL != Event && Status == EFI_SUCCESS) {
    gBS->SignalEvent(Event);
  }

Done:
  return Status;
}

/**
  Initialize the state information for the I2C Master Protocol

  @param  ImageHandle   of the loaded driver
  @param  SystemTable   Pointer to the System Table

  @retval EFI_SUCCESS           Protocol registered
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate protocol data structureEFI
  @retval EFI_DEVICE_ERROR      Hardware problems

**/
EFI_STATUS
EFIAPI
I2cMasterEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable)
{
  EFI_STATUS Status;
  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gI2cMasterDriverBinding,
             ImageHandle,
             &gI2cMasterComponentName,
             &gI2cMasterComponentName2
             );
  ASSERT_EFI_ERROR (Status);
  return Status;
}
