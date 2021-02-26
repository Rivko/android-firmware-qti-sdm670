/** @file

    Usb bus enumeration support.

Copyright (c) 2010-2017 Qualcomm Technologies, Inc. All rights reserved.
Portions Copyright (c) 2007 - 2013 Intel Corporation. All rights reserved.<BR>
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
 08/24/17   pm       Fix issue for Type C headset as Audio class not only present at interface 0
 07/12/17   rt       Not doing hub reset on failure
 06/28/17   pm       Added check for Valid HubIF
 02/01/17   pm       Port changes from BOOT.XF.1.4
 04/27/15   ck       Removed unused p4 tags
 04/30/14   ar       Cleanup for NDEBUG build  
 02/05/14   amitg    Async USB Host Stack
 01/20/14   ck       Port EDK2 host stack
 03/30/11   sniedzie Port fix for enumeration failure.
 03/30/11   sniedzie Integrated from MdeModulePkg for QCOM changes.

=============================================================================*/

#include "UsbBus.h"

/**
  Return the endpoint descriptor in this interface.

  @param  UsbIf                 The interface to search in.
  @param  EpAddr                The address of the endpoint to return.

  @return The endpoint descriptor or NULL.

**/
USB_ENDPOINT_DESC *
UsbGetEndpointDesc (
  IN USB_INTERFACE        *UsbIf,
  IN UINT8                EpAddr
  )
{
  USB_ENDPOINT_DESC       *EpDesc;
  UINT8                   Index;
  UINT8                   NumEndpoints;

  NumEndpoints = UsbIf->IfSetting->Desc.NumEndpoints;

  for (Index = 0; Index < NumEndpoints; Index++) {
    EpDesc = UsbIf->IfSetting->Endpoints[Index];

    if (EpDesc->Desc.EndpointAddress == EpAddr) {
      return EpDesc;
    }
  }

  return NULL;
}


/**
  Free the resource used by USB interface.

  @param  UsbIf                 The USB interface to free.

**/
VOID
UsbFreeInterface (
  IN USB_INTERFACE        *UsbIf
  )
{
  UsbCloseHostProtoByChild (UsbIf->Device->Bus, UsbIf->Handle);

  gBS->UninstallMultipleProtocolInterfaces (
         UsbIf->Handle,
         &gEfiDevicePathProtocolGuid,
         UsbIf->DevicePath,
         &gEfiUsbIoProtocolGuid,
         &UsbIf->UsbIo,
         NULL
         );

  if (UsbIf->DevicePath != NULL) {
    FreePool (UsbIf->DevicePath);
  }

  FreePool (UsbIf);
}


/**
  Create an interface for the descriptor IfDesc. Each
  device's configuration can have several interfaces.

  @param  Device                The device has the interface descriptor.
  @param  IfDesc                The interface descriptor.

  @return The created USB interface for the descriptor, or NULL.

**/
USB_INTERFACE *
UsbCreateInterface (
  IN USB_DEVICE           *Device,
  IN USB_INTERFACE_DESC   *IfDesc
  )
{
  USB_DEVICE_PATH         UsbNode;
  USB_INTERFACE           *UsbIf;
  USB_INTERFACE           *HubIf;
  EFI_STATUS              Status;

  UsbIf = AllocateZeroPool (sizeof (USB_INTERFACE));

  if (UsbIf == NULL) {
    return NULL;
  }

  UsbIf->Signature  = USB_INTERFACE_SIGNATURE;
  UsbIf->Device     = Device;
  UsbIf->IfDesc     = IfDesc;
  USB_ASSERT_GOTO ((IfDesc->ActiveIndex < USB_MAX_INTERFACE_SETTING), ON_ERROR);
  UsbIf->IfSetting  = IfDesc->Settings[IfDesc->ActiveIndex];

  CopyMem (
    &(UsbIf->UsbIo),
    &mUsbIoProtocol,
    sizeof (EFI_USB_IO_PROTOCOL)
    );

  //
  // Install protocols for USBIO and device path
  //
  UsbNode.Header.Type       = MESSAGING_DEVICE_PATH;
  UsbNode.Header.SubType    = MSG_USB_DP;
  UsbNode.ParentPortNumber  = Device->ParentPort;
  UsbNode.InterfaceNumber   = UsbIf->IfSetting->Desc.InterfaceNumber;

  SetDevicePathNodeLength (&UsbNode.Header, sizeof (UsbNode));

  HubIf = Device->ParentIf;
  USB_ASSERT_GOTO ((HubIf != NULL), ON_ERROR);

  UsbIf->DevicePath = AppendDevicePathNode (HubIf->DevicePath, &UsbNode.Header);

  if (UsbIf->DevicePath == NULL) {
    DEBUG ((EFI_D_ERROR, "UsbCreateInterface: failed to create device path\n"));

    Status = EFI_OUT_OF_RESOURCES;
    goto ON_ERROR;
  }

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &UsbIf->Handle,
                  &gEfiDevicePathProtocolGuid,
                  UsbIf->DevicePath,
                  &gEfiUsbIoProtocolGuid,
                  &UsbIf->UsbIo,
                  NULL
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "UsbCreateInterface: failed to install UsbIo - %r\n", Status));
    goto ON_ERROR;
  }

  //
  // Open USB Host Controller Protocol by Child
  //
  Status = UsbOpenHostProtoByChild (Device->Bus, UsbIf->Handle);

  if (EFI_ERROR (Status)) {
    gBS->UninstallMultipleProtocolInterfaces (
           &UsbIf->Handle,
           &gEfiDevicePathProtocolGuid,
           UsbIf->DevicePath,
           &gEfiUsbIoProtocolGuid,
           &UsbIf->UsbIo,
           NULL
           );

    DEBUG ((EFI_D_ERROR, "UsbCreateInterface: failed to open host for child - %r\n", Status));
    goto ON_ERROR;
  }

  return UsbIf;

ON_ERROR:
  if (UsbIf->DevicePath != NULL) {
    FreePool (UsbIf->DevicePath);
  }

  FreePool (UsbIf);
  return NULL;
}


/**
  Free the resource used by this USB device.

  @param  Device                The USB device to free.

**/
VOID
UsbFreeDevice (
  IN USB_DEVICE           *Device
  )
{
  if (Device->DevDesc != NULL) {
    UsbFreeDevDesc (Device->DevDesc);
  }

  gBS->FreePool (Device);
}


/**
  Create a device which is on the parent's ParentPort port.

  @param  ParentIf              The parent HUB interface.
  @param  ParentPort            The port on the HUB this device is connected to.

  @return Created USB device, Or NULL.

**/
USB_DEVICE *
UsbCreateDevice (
  IN USB_INTERFACE        *ParentIf,
  IN UINT8                ParentPort
  )
{
  USB_DEVICE              *Device;

  USB_ASSERT_RETURN ((ParentIf != NULL), NULL);

  Device = AllocateZeroPool (sizeof (USB_DEVICE));

  if (Device == NULL) {
    return NULL;
  }

  Device->Bus         = ParentIf->Device->Bus;
  Device->MaxPacket0  = 8;
  Device->ParentAddr  = ParentIf->Device->Address;
  Device->ParentIf    = ParentIf;
  Device->ParentPort  = ParentPort;
  Device->Tier        = (UINT8)(ParentIf->Device->Tier + 1);
  return Device;
}


/**
  Connect the USB interface with its driver. EFI USB bus will
  create a USB interface for each separate interface descriptor.

  @param  UsbIf             The interface to connect driver to.
  @param  AsyncFlag         Flag to define async control transfer
  @param  AsyncConext       Async control transfer context

  @return EFI_SUCCESS       Interface is managed by some driver.
  @return Others            Failed to locate a driver for this interface.

**/
EFI_STATUS
UsbConnectDriver (
  IN USB_INTERFACE               *UsbIf,
  IN BOOLEAN                      AsyncFlag,
  IN  VOID                       *AsyncConext
  )
{
  EFI_STATUS              Status;
  EFI_TPL                 OldTpl;

  Status = EFI_SUCCESS;

  //
  // Hub is maintained by the USB bus driver. Otherwise try to
  // connect drivers with this interface
  //
  if (UsbIsHubInterface (UsbIf)) {
    DEBUG ((EFI_D_INFO, "UsbConnectDriver: found a hub device\n"));
    Status = mUsbHubApi.Init (UsbIf, AsyncFlag, AsyncConext);

  } else {
    //
    // This function is called in both UsbIoControlTransfer and
    // the timer callback in hub enumeration. So, at least it is
    // called at TPL_CALLBACK. Some driver sitting on USB has
    // twisted TPL used. It should be no problem for us to connect
    // or disconnect at CALLBACK.
    //

    //
    // Only recursively wanted usb child device
    //
    if (UsbBusIsWantedUsbIO (UsbIf->Device->Bus, UsbIf)) {
      OldTpl            = UsbGetCurrentTpl ();
      DEBUG ((EFI_D_INFO, "UsbConnectDriver: TPL before connect is %d, %p\n", (UINT32)OldTpl, UsbIf->Handle));

      gBS->RestoreTPL (TPL_CALLBACK);

      Status            = gBS->ConnectController (UsbIf->Handle, NULL, NULL, TRUE);
      UsbIf->IsManaged  = (BOOLEAN)!EFI_ERROR (Status);

      DEBUG ((EFI_D_INFO, "UsbConnectDriver: TPL after connect is %d\n", (UINT32)UsbGetCurrentTpl()));
      ASSERT (UsbGetCurrentTpl () == TPL_CALLBACK);

      gBS->RaiseTPL (OldTpl);
    }
  }

  return Status;
}


/**
  Select an alternate setting for the interface.
  Each interface can have several mutually exclusive
  settings. Only one setting is active. It will
  also reset its endpoints' toggle to zero.

  @param  IfDesc                The interface descriptor to set.
  @param  Alternate             The alternate setting number to locate.

  @retval EFI_NOT_FOUND         There is no setting with this alternate index.
  @retval EFI_SUCCESS           The interface is set to Alternate setting.

**/
EFI_STATUS
UsbSelectSetting (
  IN USB_INTERFACE_DESC   *IfDesc,
  IN UINT8                Alternate
  )
{
  USB_INTERFACE_SETTING   *Setting;
  UINTN                   Index;

  //
  // Locate the active alternate setting
  //
  Setting = NULL;

  for (Index = 0; Index < IfDesc->NumOfSetting; Index++) {
    USB_ASSERT_RETURN ((Index < USB_MAX_INTERFACE_SETTING), EFI_DEVICE_ERROR);
    Setting = IfDesc->Settings[Index];

    if (Setting->Desc.AlternateSetting == Alternate) {
      break;
    }
  }

  if (Index == IfDesc->NumOfSetting) {
    return EFI_NOT_FOUND;
  }

  IfDesc->ActiveIndex = Index;

  USB_ASSERT_RETURN ((Setting != NULL), EFI_DEVICE_ERROR);
  DEBUG ((EFI_D_INFO, "UsbSelectSetting: setting %d selected for interface %d\n",
              Alternate, Setting->Desc.InterfaceNumber));

  //
  // Reset the endpoint toggle to zero
  //
  for (Index = 0; Index < Setting->Desc.NumEndpoints; Index++) {
    Setting->Endpoints[Index]->Toggle = 0;
  }

  return EFI_SUCCESS;
}


/**
  Select a new configuration for the device. Each
  device may support several configurations.

  @param  Device                The device to select configuration.
  @param  ConfigValue           The index of the configuration ( != 0).
  @param  AsyncFlag           Flag to define async control transfer
  @param  AsyncConext       Async control transfer context

  @retval EFI_NOT_FOUND         There is no configuration with the index.
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate resource.
  @retval EFI_SUCCESS           The configuration is selected.

**/
EFI_STATUS
UsbSelectConfig (
  IN USB_DEVICE           *Device,
  IN UINT8                ConfigValue,
  IN BOOLEAN              AsyncFlag,
  IN  VOID                *AsyncConext
  )
{
  USB_DEVICE_DESC         *DevDesc;
  USB_CONFIG_DESC         *ConfigDesc;
  USB_INTERFACE_DESC      *IfDesc;
  USB_INTERFACE           *UsbIf;
  EFI_STATUS              Status;
  UINT8                   Index;

  //
  // Locate the active config, then set the device's pointer
  //
  DevDesc     = Device->DevDesc;
  ConfigDesc  = NULL;

  for (Index = 0; Index < DevDesc->Desc.NumConfigurations; Index++) {
    ConfigDesc = DevDesc->Configs[Index];

    if (ConfigDesc->Desc.ConfigurationValue == ConfigValue) {
      break;
    }
  }

  if (Index == DevDesc->Desc.NumConfigurations) {
    return EFI_NOT_FOUND;
  }

  Device->ActiveConfig = ConfigDesc;

  DEBUG ((EFI_D_INFO, "UsbSelectConfig: config %d selected for device %d\n",
              ConfigValue, Device->Address));

  //
  // Create interfaces for each USB interface descriptor.
  //
  for (Index = 0; Index < ConfigDesc->Desc.NumInterfaces; Index++) {
    //
    // First select the default interface setting, and reset
    // the endpoint toggles to zero for its endpoints.
    //
    IfDesc = ConfigDesc->Interfaces[Index];
    UsbSelectSetting (IfDesc, IfDesc->Settings[0]->Desc.AlternateSetting);

    //
    // Create a USB_INTERFACE and install USB_IO and other protocols
    //
    UsbIf = UsbCreateInterface (Device, ConfigDesc->Interfaces[Index]);

    if (UsbIf == NULL) {
      Device->NumOfInterface = Index;
      return EFI_OUT_OF_RESOURCES;
    }

    USB_ASSERT_RETURN ((Index < USB_MAX_INTERFACE), EFI_DEVICE_ERROR);
    Device->Interfaces[Index] = UsbIf;

    //
    // Connect the device to drivers, if it failed, ignore
    // the error. Don't let the unsupported interfaces to block
    // the supported interfaces.
    //
    Status = UsbConnectDriver (UsbIf, AsyncFlag, AsyncConext);

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "UsbSelectConfig: failed to connect driver %r, ignored\n", Status));
    }
  }

  Device->NumOfInterface = Index;

  return EFI_SUCCESS;
}


/**
  Disconnect the USB interface with its driver.

  @param  UsbIf                 The interface to disconnect driver from.

  @retval EFI_SUCCESS           The USB interface successfully disconnected
  @retval OTHERS                The operation fails.

**/
EFI_STATUS
UsbDisconnectDriver (
  IN USB_INTERFACE        *UsbIf
  )
{
  EFI_TPL                 OldTpl;
  EFI_STATUS              Status;

  //
  // Release the hub if it's a hub controller, otherwise
  // disconnect the driver if it is managed by other drivers.
  //
  Status = EFI_SUCCESS;
  if (UsbIf->IsHub) {
    Status = UsbIf->HubApi->Release (UsbIf);

  } else if (UsbIf->IsManaged) {
    //
    // This function is called in both UsbIoControlTransfer and
    // the timer callback in hub enumeration. So, at least it is
    // called at TPL_CALLBACK. Some driver sitting on USB has
    // twisted TPL used. It should be no problem for us to connect
    // or disconnect at CALLBACK.
    //
    OldTpl           = UsbGetCurrentTpl ();
    DEBUG ((EFI_D_INFO, "UsbDisconnectDriver: old TPL is %d, %p\n", (UINT32)OldTpl, UsbIf->Handle));

    gBS->RestoreTPL (TPL_CALLBACK);

    Status = gBS->DisconnectController (UsbIf->Handle, NULL, NULL);
    if (!EFI_ERROR (Status)) {
      UsbIf->IsManaged = FALSE;
    }

    DEBUG (( EFI_D_INFO, "UsbDisconnectDriver: TPL after disconnect is %d, %d\n", (UINT32)UsbGetCurrentTpl(), Status));
    ASSERT (UsbGetCurrentTpl() == TPL_CALLBACK);

    gBS->RaiseTPL (OldTpl);
  }

  return Status;
}


/**
  Remove the current device configuration.

  @param  Device                The USB device to remove configuration from.

  @retval EFI_SUCCESS           The USB interface successfully disconnected
  @retval OTHERS                The operation fails.

**/
EFI_STATUS
UsbRemoveConfig (
  IN USB_DEVICE           *Device
  )
{
  USB_INTERFACE           *UsbIf;
  UINTN                   Index;
  EFI_STATUS              Status;
  EFI_STATUS              ReturnStatus;

  //
  // Remove each interface of the device
  //
  ReturnStatus = EFI_SUCCESS;
  for (Index = 0; Index < Device->NumOfInterface; Index++) {
    USB_ASSERT_RETURN ((Index <  USB_MAX_INTERFACE), EFI_DEVICE_ERROR);
    UsbIf = Device->Interfaces[Index];

    if (UsbIf == NULL) {
      continue;
    }

    Status = UsbDisconnectDriver (UsbIf);
    if (!EFI_ERROR (Status)) {
      UsbFreeInterface (UsbIf);
      Device->Interfaces[Index] = NULL;
    } else {
      ReturnStatus = Status;
    }
  }

  Device->ActiveConfig    = NULL;
  return ReturnStatus;
}


/**
  Remove the device and all its children from the bus.

  @param  Device                The device to remove.

  @retval EFI_SUCCESS           The device is removed.

**/
EFI_STATUS
UsbRemoveDevice (
  IN USB_DEVICE           *Device
  )
{
  USB_BUS                 *Bus;
  USB_DEVICE              *Child;
  EFI_STATUS              Status;
  EFI_STATUS              ReturnStatus;
  UINTN                   Index;

  Bus = Device->Bus;

  //
  // Remove all the devices on its downstream ports. Search from devices[1].
  // Devices[0] is the root hub.
  //
  ReturnStatus = EFI_SUCCESS;
  for (Index = 1; Index < Bus->MaxDevices; Index++) {
    Child = Bus->Devices[Index];

    if ((Child == NULL) || (Child->ParentAddr != Device->Address)) {
      continue;
    }

    Status = UsbRemoveDevice (Child);

    if (!EFI_ERROR (Status)) {
      Bus->Devices[Index] = NULL;
    } else {
      Bus->Devices[Index]->DisconnectFail = TRUE;
      ReturnStatus = Status;
      DEBUG ((EFI_D_ERROR, "UsbRemoveDevice: failed to remove child %p at parent %p\n", Child, Device));
    }
  }

  if (EFI_ERROR (ReturnStatus)) {
    return ReturnStatus;
  }

  Status = UsbRemoveConfig (Device);

  if (!EFI_ERROR (Status)) {
    DEBUG (( EFI_D_INFO, "UsbRemoveDevice: device %d removed\n", Device->Address));

    USB_ASSERT_RETURN ((Device->Address < Bus->MaxDevices), EFI_DEVICE_ERROR);
    Bus->Devices[Device->Address] = NULL;
    UsbFreeDevice (Device);
  } else {
    Bus->Devices[Device->Address]->DisconnectFail = TRUE;
    DEBUG ((EFI_D_ERROR, "UsbRemoveConfig: failed to remove child %p at parent %p\n", Child, Device));
  }
  return Status;
}


/**
  Find the child device on the hub's port.

  @param  HubIf                 The hub interface.
  @param  Port                  The port of the hub this child is connected to.

  @return The device on the hub's port, or NULL if there is none.

**/
USB_DEVICE *
UsbFindChild (
  IN USB_INTERFACE        *HubIf,
  IN UINT8                Port
  )
{
  USB_DEVICE              *Device;
  USB_BUS                 *Bus;
  UINTN                   Index;

  Bus = HubIf->Device->Bus;

  //
  // Start checking from device 1, device 0 is the root hub
  //
  for (Index = 1; Index < Bus->MaxDevices; Index++) {
    Device = Bus->Devices[Index];

    if ((Device != NULL) && (Device->ParentAddr == HubIf->Device->Address) &&
        (Device->ParentPort == Port)) {

      return Device;
    }
  }

  return NULL;
}


/**
  Process the events on the port.

  @param  HubIfC              The HUB Interface Context that has the device connected.
  @param  Port                  The port index of the hub (started with zero).

  @retval EFI_SUCCESS           The device is enumerated (added or removed).
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate resource for the device.
  @retval Others                Failed to enumerate the device.

**/

EFI_STATUS
UsbEnumeratePort (
  IN USB_INTERFACE_CONTEXT   *HubIfC,
  IN UINT8                   Port
  )
{
  USB_HUB_API             *HubApi;
  USB_DEVICE              *Child;
  EFI_USB_PORT_STATUS     *PortState;
  EFI_STATUS              Status = EFI_SUCCESS;
  USB_INTERFACE           *HubIf;
  USB_CONTEXT             *HubC;
  USB_DEVICE_INFO         *DevInfo;

  USB_ASSERT_RETURN ((HubIfC != NULL), EFI_INVALID_PARAMETER);

  HubIf  = HubIfC->usbif;
  USB_ASSERT_RETURN ((HubIf != NULL), EFI_DEVICE_ERROR);

  HubC  = HubIfC->usb_contxt;
  USB_ASSERT_RETURN ((HubC != NULL), EFI_DEVICE_ERROR);


  Child   = NULL;
  HubApi  = HubIf->HubApi;

  PortState = AllocateZeroPool (sizeof(EFI_USB_PORT_STATUS));

  if (PortState == NULL) {
    DEBUG ((EFI_D_ERROR, "UsbEnumeratePort: out of resources\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  DevInfo = HubC->deviceinfo;

  //
  // Host learns of the new device by polling the hub for port changes.
  //
  Status = HubApi->GetPortStatus (HubIf, Port, PortState, FALSE, NULL);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "UsbEnumeratePort: failed to get state of port %d\n", Port));
    return Status;
  }

  //
  // Only handle connection/enable/overcurrent/reset change.
  // Usb super speed hub may report other changes, such as warm reset change. Ignore them.
  //
  if ((PortState->PortChangeStatus & (USB_PORT_STAT_C_CONNECTION | USB_PORT_STAT_C_ENABLE | USB_PORT_STAT_C_OVERCURRENT | USB_PORT_STAT_C_RESET)) == 0) {
    return EFI_SUCCESS;
  }

  DEBUG (( EFI_D_INFO, "UsbEnumeratePort: port %d state - %02x, change - %02x on %p\n",
              Port, PortState->PortStatus, PortState->PortChangeStatus, HubIf));

  //
  // This driver only process two kinds of events now: over current and
  // connect/disconnect. Other three events are: ENABLE, SUSPEND, RESET.
  // ENABLE/RESET is used to reset port. SUSPEND isn't supported.
  //

  if (USB_BIT_IS_SET (PortState->PortChangeStatus, USB_PORT_STAT_C_OVERCURRENT)) {

    if (USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_OVERCURRENT)) {
      //
      // Case1:
      //   Both OverCurrent and OverCurrentChange set, means over current occurs,
      //   which probably is caused by short circuit. It has to wait system hardware
      //   to perform recovery.
      //
      DEBUG (( EFI_D_ERROR, "UsbEnumeratePort: Critical Over Current\n", Port));
      return EFI_DEVICE_ERROR;

    }
    //
    // Case2:
    //   Only OverCurrentChange set, means system has been recoveried from
    //   over current. As a result, all ports are nearly power-off, so
    //   it's necessary to detach and enumerate all ports again.
    //
    DEBUG (( EFI_D_ERROR, "UsbEnumeratePort: 2.0 device Recovery Over Current\n", Port));
  }

  if (USB_BIT_IS_SET (PortState->PortChangeStatus, USB_PORT_STAT_C_ENABLE)) {
    //
    // Case3:
    //   1.1 roothub port reg doesn't reflect over-current state, while its counterpart
    //   on 2.0 roothub does. When over-current has influence on 1.1 device, the port
    //   would be disabled, so it's also necessary to detach and enumerate again.
    //
    DEBUG (( EFI_D_ERROR, "UsbEnumeratePort: 1.1 device Recovery Over Current\n", Port));
  }

  if (USB_BIT_IS_SET (PortState->PortChangeStatus, USB_PORT_STAT_C_CONNECTION)) {
    //
    // Case4:
    //   Device connected or disconnected normally.
    //
    DEBUG ((EFI_D_INFO, "UsbEnumeratePort: Device Connect/Disconnect Normally\n", Port));
  }

  //
  // Following as the above cases, it's safety to remove and create again.
  //
  Child = UsbFindChild (HubIf, Port);

  if (Child != NULL) {
    DEBUG (( EFI_D_INFO, "UsbEnumeratePort: device at port %d removed from root hub %p\n", Port, HubIf));
    UsbRemoveDevice (Child);
  }

  if (USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_CONNECTION)) {
    //
    // Now, new device connected, enumerate and configure the device
    //
    DEBUG (( EFI_D_ERROR, "UsbEnumeratePort: new device connected at port %d, HubIfc =0x%x \n", Port, HubIfC));

    if (HubC->callfrom_roothub == TRUE)
    {// New Device connected to root hub port
        if (DevInfo->syncstatus->syncvariable == TRUE)
        {
             DevInfo->Port = Port;
             DevInfo->syncstatus->syncvariable = FALSE;

             // Initialize the device state
             DevInfo->usbdev_state = USB_DEVICE_STABPORT;

             Status = UsbEnumNewDevRootHub ((void *)PortState, sizeof(EFI_USB_PORT_STATUS),(void *) HubIfC, Status, 0);
             if (!(EFI_ERROR(Status)) && (DevInfo->usbdev_state != USB_DEVICE_ENUMCOMPLETE))
             {
                 goto Exit;
             }
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: failed to get thread, port= %d, devstate = 0x%x \n", Port, DevInfo->usbdev_state));
            Status = EFI_DEVICE_ERROR;
            goto Exit;
        }
   }
   else
   {// New Device connected to hub port
         if (DevInfo->syncstatus->syncvariable == TRUE)
         {
             DevInfo->syncstatus->syncvariable = FALSE;
             DevInfo->Port = Port;

             // Initialize the device state
             DevInfo->usbdev_state = USB_DEVICE_STABPORT;
             Status = UsbEnumNewDevHub ((void *)PortState, sizeof(EFI_USB_PORT_STATUS),(void *) HubIfC, Status, 0);
             if (!(EFI_ERROR(Status)) && (DevInfo->usbdev_state != USB_DEVICE_ENUMCOMPLETE))
             {
                 goto Exit;
             }
         }
         else
         {
             DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to get thread, port= %d, devstate = 0x%x \n", Port, DevInfo->usbdev_state));
             Status = EFI_DEVICE_ERROR;
             goto Exit;
         }
    }
  }else {

    DEBUG (( EFI_D_INFO, "UsbEnumeratePort: device disconnected event on port %d\n", Port));
  }

  HubApi->ClearPortChange (HubIf, Port, FALSE, NULL);

  // Free PortState if we did not see any new device,
  // Otherwise we pass the PortState to UsbEnumerateNewDev_* and free it there on enum completion
  if (PortState != NULL)
  {
      gBS->FreePool (PortState);
  }

  Exit:
  return Status;
}

/**
  Enumerate and configure the new device on the port of this Root HUB interface.

  @param  Data                 A pointer to a buffer that is filled with key data which is
                                       retrieved via asynchronous control transfer.
  @param  DataLength       Indicates the size of the data buffer.
  @param  Context            Pointing to USB_KB_DEV instance.
  @param  Result              Indicates the result of the asynchronous control transfer.
  @param  Toggle             Not used, need to keep for reusage of the async callback function format

  @retval EFI_SUCCESS      Asynchronous bulk in transfer is handled successfully.
  @retval EFI_DEVICE_ERROR Hardware error occurs.

**/

EFI_STATUS
EFIAPI
UsbEnumNewDevRootHub (
  IN  VOID        *Data,
  IN  UINTN       DataLength,
  IN  VOID        *Context,
  IN  UINT32      Result,
  IN  UINT8       Toggle
  )
{
  USB_DEVICE                *Child;
  USB_INTERFACE_CONTEXT     *HubIfCont;
  USB_INTERFACE             *HubIf;
  USB_CONTEXT               *HubCont;
  EFI_USB_PORT_STATUS       *PortState = NULL;
  USB_HUB_SYNC_STATUS       *SyncState = NULL;
  UINT8                     Port;
  EFI_STATUS                Status = EFI_SUCCESS;
  USB_DEVICE                *Parent;
  UINT8                     Config = 0;
  usb_device_state_type     DevState;
  USB_HUB_API               *HubApi;
  UINTN                     Index;
  UINT8                     Index_c = 0;
  UINT8                     Index_i = 0;
  UINTN                     IsAudioClass = 0;
  USB_BUS                   *Bus;
  UINTN                     Address=0;
  USB_DEVICE_INFO           *DevInfo;

  
  USB_ASSERT_RETURN_STR ((Context != NULL), EFI_INVALID_PARAMETER,
                         "UsbEnumNewDevRootHub: Assert since Hub Context is NULL");

  HubIfCont = (USB_INTERFACE_CONTEXT *) Context;

  HubCont   = HubIfCont->usb_contxt;
  USB_ASSERT_RETURN ((HubCont != NULL), EFI_DEVICE_ERROR);

  HubIf     = HubIfCont->usbif;
  USB_ASSERT_RETURN ((HubIf != NULL), EFI_DEVICE_ERROR);

  DevInfo   = HubCont->deviceinfo;
  USB_ASSERT_RETURN ((DevInfo != NULL), EFI_DEVICE_ERROR);

  //Check for valid HubIf in case of forcefully calling StopUsbController which will remove the hub interface
  if(HubIf->Signature != USB_INTERFACE_SIGNATURE)
  {
    DEBUG ((EFI_D_ERROR, "Invalid Signature for RootHub = 0x%x \n", HubIf->Signature));
    Status = EFI_ACCESS_DENIED;
    return Status;
  }
  
  DevState  = DevInfo->usbdev_state;
  Parent    = HubIf->Device;
  Bus       = Parent->Bus;
  Child     = HubCont->Child;
  Port      = DevInfo->Port;
  PortState = DevInfo->PortState;
  SyncState = DevInfo->syncstatus;
  HubApi    = HubIf->HubApi;

  DEBUG ((EFI_D_INFO, "UsbEnumNewDevRootHub: Entry point port %d, Root Hub Device state = %d \n\r", Port, DevState));

  switch (DevState)
  {
    case USB_DEVICE_STABPORT:
    { // Wait for port stablization timeout to make sure device is connected properly before perfoming the port reset
      PortState = (EFI_USB_PORT_STATUS *)Data;

      DevInfo->PortState = PortState;
      DevInfo->usbdev_state = USB_DEVICE_RSTPORT;

      Status = USBRootHubReset_Stall (USB_WAIT_PORT_STABLE_STALL, (void *)HubIfCont);
      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: failed to schedule USB_WAIT_PORT_STABLE_STALL for port %d - %r\n \r", Port, Status));
        goto ON_ERROR;
      }
      break;
    }
    case USB_DEVICE_RSTPORT:
    { // Perform port reset
      //
      // Hub resets the device for at least 10 milliseconds.
      // Host learns device speed. If device is of low/full speed
      // and the hub is a EHCI root hub, ResetPort will release
      // the device to its companion UHCI and return an error.
      //
      // Notice: although EHCI requires that ENABLED bit be cleared
      // when reset the port, we don't need to care that here. It
      // should be handled in the EHCI driver.
      //
      UINT64 ResetWaitTime;
      Status = UsbHcGetRootHubPortStatus (Bus, Port, PortState);

      if (EFI_ERROR (Status)) {
        goto ON_ERROR;
      } else if (USB_BIT_IS_SET (PortState->PortChangeStatus, USB_PORT_STAT_C_RESET)) {
        DEBUG (( EFI_D_ERROR, "UsbRootHubResetPort: skip reset on root port %d\n", Port));
        DevInfo->usbdev_state = USB_DEVICE_ENUMCOMPLETE;
        ResetWaitTime = 0;
      } else {
        Status  = UsbHcSetRootHubPortFeature (Bus, Port, EfiUsbPortReset);
        if (EFI_ERROR (Status)) {
          DEBUG (( EFI_D_ERROR, "UsbRootHubResetPort: failed to start reset on port %d\n\r", Port));
          goto ON_ERROR;
        }
        DevInfo->usbdev_state = USB_DEVICE_RSTPORT1;
        ResetWaitTime = USB_SET_ROOT_PORT_RESET_STALL;
      }
      //
      // Drive the reset signal for at least 50ms. Check USB 2.0 Spec
      // section 7.1.7.5 for timing requirements.
      //
      Status = USBRootHubReset_Stall(ResetWaitTime, HubIfCont);
      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: failed to schedule USB_SET_ROOT_PORT_RESET_STALL for port %d - %r\n \r", Port, Status));
        goto ON_ERROR;
      }
      break;
    }
    case USB_DEVICE_RSTPORT1:
    { // Clear port reset
      Status = UsbHcClearRootHubPortFeature (Bus, Port, EfiUsbPortReset);

      if (EFI_ERROR (Status)) {
        DEBUG (( EFI_D_ERROR, "UsbRootHubResetPort: failed to clear reset on port %d\n \r", Port));
        goto ON_ERROR;
      }

      DevInfo->usbdev_state = USB_DEVICE_RSTPORT2;

      Status = USBRootHubReset_Stall(USB_CLR_ROOT_PORT_RESET_STALL, HubIfCont);
      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: failed to schedule USB_CLR_ROOT_PORT_RESET_STALL for port %d - %r\n \r", Port, Status));
        goto ON_ERROR;
      }
      break;
    }
    case USB_DEVICE_RSTPORT2:
    { // Wait for the clear port reset Stall and check if port reset actually finished
      //
      // USB host controller won't clear the RESET bit until
      // reset is actually finished.
      //
      for (Index = 0; Index < USB_WAIT_PORT_STS_CHANGE_LOOP; Index++) {
        Status = UsbHcGetRootHubPortStatus (Bus, Port, PortState);

        if (EFI_ERROR (Status)) {
          goto ON_ERROR;
        }

        if (!USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_RESET)) {
         break;
        }

        gBS->Stall (USB_WAIT_PORT_STS_CHANGE_STALL);
      }

      if (Index == USB_WAIT_PORT_STS_CHANGE_LOOP) {
        DEBUG ((EFI_D_INFO, "UsbRootHubResetPort: reset not finished in time on port %d\n", Port));
        Status = EFI_TIMEOUT;
        goto ON_ERROR;
      }

      if (!USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_ENABLE)) {
        //
        // OK, the port is reset. If root hub is of high speed and
        // the device is of low/full speed, release the ownership to
        // companion UHCI. If root hub is of full speed, it won't
        // automatically enable the port, we need to enable it manually.
        //
        if (HubIfCont->usbif->MaxSpeed == EFI_USB_SPEED_HIGH) {
          DEBUG (( EFI_D_INFO, "UsbRootHubResetPort: release low/full speed device (%d) to UHCI\n", Port));

          HubApi->SetPortFeature (HubIfCont->usbif, Port, EfiUsbPortOwner, FALSE, NULL);
          Status =  EFI_NOT_FOUND;
          goto ON_ERROR;
        } else {

          Status = HubApi->SetPortFeature (HubIfCont->usbif, Port, EfiUsbPortEnable, FALSE, NULL);

          if (EFI_ERROR (Status)) {
            DEBUG (( EFI_D_ERROR, "UsbRootHubResetPort: failed to enable port %d for UHCI\n \r", Port));
            goto ON_ERROR;
            }

          DevInfo->usbdev_state= USB_DEVICE_ENUMCOMPLETE;

          //gBS->Stall (USB_SET_ROOT_PORT_ENABLE_STALL);
          Status = USBRootHubReset_Stall(USB_SET_ROOT_PORT_ENABLE_STALL, HubIfCont);
          if (EFI_USB_ERROR (Status)) {
            DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: failed to schedule USB_SET_ROOT_PORT_ENABLE_STALL for port %d - %r\n \r", Port, Status));
            goto ON_ERROR;
          }
          break;
        }
      }
      DevInfo->usbdev_state= USB_DEVICE_ENUMCOMPLETE;
    }
    case USB_DEVICE_ENUMCOMPLETE:
    { // Perform rest of enumeration till the completion
      DEBUG (( EFI_D_INFO, "UsbEnumNewDevRootHub: hub port %d is reset\n", Port));

      Child = UsbCreateDevice (HubIfCont->usbif, Port);

      if (Child == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto ON_ERROR;
      }
      HubCont->Child = Child;
      //
      // OK, now identify the device speed. After reset, hub
      // fully knows the actual device speed.
      //
      // this is synchronous call
      Status = HubApi->GetPortStatus (HubIfCont->usbif, Port, PortState, FALSE, NULL);

      DEBUG ((EFI_D_INFO, "UsbEnumNewDevRootHub: port %d, Device State - USB_DEVICE_ASSIGNADD \n", Port));

      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: failed to get speed of port %d\n\r", Port));
        goto ON_ERROR;
      }
      if (!USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_CONNECTION)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumerateNewDev: No device present at port %d after reset, returning \n", Port));
        goto ON_ERROR;
      }
      // Check Port/Device Speed
      if (USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_SUPER_SPEED)){
        Child->Speed      = EFI_USB_SPEED_SUPER;
        Child->MaxPacket0 = 512;
      } else if (USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_HIGH_SPEED)) {
        Child->Speed      = EFI_USB_SPEED_HIGH;
        Child->MaxPacket0 = 64;
      } else if (USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_LOW_SPEED)) {
        Child->Speed      = EFI_USB_SPEED_LOW;
        Child->MaxPacket0 = 8;
      } else {
        Child->Speed      = EFI_USB_SPEED_FULL;
        Child->MaxPacket0 = 8;
      }

      DEBUG (( EFI_D_INFO, "UsbEnumNewDevRootHub: device is of %d speed\n", Child->Speed));

      if (((Child->Speed == EFI_USB_SPEED_LOW) || (Child->Speed == EFI_USB_SPEED_FULL)) &&
          (Parent->Speed == EFI_USB_SPEED_HIGH)) {
        //
        // If the child is a low or full speed device, it is necessary to
        // set the transaction translator. Port TT is 1-based.
        // This is quite simple:
        //  1. if parent is of high speed, then parent is our translator
        //  2. otherwise use parent's translator.
        //
        Child->Translator.TranslatorHubAddress  = Parent->Address;
        Child->Translator.TranslatorPortNumber  = (UINT8) (Port + 1);
      } else {
        Child->Translator = Parent->Translator;
      }
      DEBUG (( EFI_D_INFO, "UsbEnumNewDevRootHub: device uses translator (%d, %d)\n",
               Child->Translator.TranslatorHubAddress,
               Child->Translator.TranslatorPortNumber));

      //
      // After port is reset, hub establishes a signal path between
      // the device and host (DEFAULT state). Device's registers are
      // reset, use default address 0 (host enumerates one device at
      // a time) , and ready to respond to control transfer at EP 0.
      //

      //
      // Host assigns an address to the device. Device completes the
      // status stage with default address, then switches to new address.
      // ADDRESS state. Address zero is reserved for root hub.
      //
      USB_ASSERT_GOTO ((Bus->MaxDevices <= 256), ON_ERROR);
      for (Address = 1; Address < Bus->MaxDevices; Address++) {
        if (Bus->Devices[Address] == NULL) {
          break;
        }
      }

      if (Address >= Bus->MaxDevices) {
        DEBUG ((EFI_D_INFO, "UsbEnumNewDevRootHub: address pool is full for port %d\n", Port));
        Status = EFI_ACCESS_DENIED;
        goto ON_ERROR;
      }

      DevInfo->Address = Address;
      Status                = UsbSetAddress (Child, (UINT8)Address, FALSE, NULL);
      DEBUG ((EFI_D_INFO, "UsbEnumNewDevRootHub: port %d, Device State - USB_DEVICE_GETPKTSIZE \n", Port));

      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: failed to set device address - %r\n\r", Status));
        goto ON_ERROR;
      }
      Child->Address        = (UINT8)Address;
      Bus->Devices[Address] = Child;
      gBS->Stall (USB_SET_DEVICE_ADDRESS_STALL);

      DEBUG ((EFI_D_INFO, "UsbEnumNewDevRootHub: device is now ADDRESSED at %d\n", Address));

      //
      // Host sends a Get_Descriptor request to learn the max packet
      // size of default pipe (only part of the device's descriptor).
      //
      // Setting this to async
      Status = UsbGetMaxPacketSize0 (Child, FALSE, NULL);

      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_INFO, "UsbEnumNewDevRootHub 0: failed to get max packet for EP 0 - %r\n", Status));
        goto ON_ERROR;
      }

      DEBUG (( EFI_D_INFO, "UsbEnumNewDevRootHub: max packet size for EP 0 is %d\n", Child->MaxPacket0));

      //
      // Host learns about the device's abilities by requesting device's
      // entire descriptions.
      //
      Status = UsbBuildDescTable (Child, FALSE, NULL);

      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: failed to build descriptor table - %r\n", Status));
        goto ON_ERROR;
      }

      //
      //Drop audio devices as they are not fully parsed(leading to crash during set_config) and also
      //no class driver support in the stack
      // 
      if (Child && Child->DevDesc) {
        for (Index_c = 0; Index_c < Child->DevDesc->Desc.NumConfigurations && Child->DevDesc->Configs[Index_c]; Index_c++) {
          for (Index_i = 0; Index_i < Child->DevDesc->Configs[Index_c]->Desc.NumInterfaces; Index_i++) {
            if (Child->DevDesc->Configs[Index_c]->Interfaces[Index_i] && Child->DevDesc->Configs[Index_c]->Interfaces[Index_i]->Settings[0]) {
              if(Child->DevDesc->Configs[Index_c]->Interfaces[Index_i]->Settings[0]->Desc.InterfaceClass == USB_AUDIO_CLASS) {
                DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: dropping audio device on port %d\n", Port));
                IsAudioClass = 1;
                goto ON_ERROR;
              }
            }
          }
        }
      }
      //
      // Select a default configuration: UEFI must set the configuration
      // before the driver can connect to the device.
      //
      Config = Child->DevDesc->Configs[0]->Desc.ConfigurationValue;
      Status = UsbSetConfig (Child, Config, FALSE, NULL);

      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: failed to set configure %d - %r\n", Config, Status));
        goto ON_ERROR;
      }

      //
      // Host assigns and loads a device driver.
      // Again making it synchronous, need to pass async conext so as to pass on the sync variable to child hub
      Status = UsbSelectConfig (Child, Config, FALSE, HubIfCont);

      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: failed to select config port %d\n", Port));
        goto ON_ERROR;
      }

      //
      // Report Status Code to indicate USB device has been detected by hotplug
      //
      REPORT_STATUS_CODE_WITH_DEVICE_PATH (
          EFI_PROGRESS_CODE,
          (EFI_IO_BUS_USB | EFI_IOB_PC_HOTPLUG),
          Bus->DevicePath
          );

      HubApi->ClearPortChange ((HubIfCont->usbif), Port, FALSE, NULL);

      //Setting the sync variable to be TRUE for the re-use of the root hub enumeration polling
      SyncState->syncvariable= TRUE;
      break;
    }

    // No action required, cases included to silence compiler
    case USB_DEVICE_GETSPEED:
    case USB_DEVICE_SETCONFIG:
    case USB_DEVICE_ENUMFAIL:
      break;
  }

  DEBUG ((EFI_D_INFO, "UsbEnumNewDevRootHub --: port %d, devstate = 0x%d \n", Port, DevInfo->usbdev_state));
  return Status;

ON_ERROR:
  //
  // If reach here, it means the enumeration process on a given port is interrupted due to error.
  // The s/w resources, including the assigned address(Address) and the allocated usb device data
  // structure(Bus->Devices[Address]), will NOT be freed here. These resources will be freed when
  // the device is unplugged from the port or DriverBindingStop() is invoked.
  //
  // This way is used to co-work with the lower layer EDKII UHCI/EHCI/XHCI host controller driver.
  // It's mainly because to keep UEFI spec unchanged EDKII XHCI driver have to maintain a state machine
  // to keep track of the mapping between actual address and request address. If the request address
  // (Address) is freed here, the Address value will be used by next enumerated device. Then EDKII XHCI
  // host controller driver will have wrong information, which will cause further transaction error.
  //
  // EDKII UHCI/EHCI doesn't get impacted as it's make sense to reserve s/w resource till it gets unplugged.
  //

  DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub --:Enumeration Failure port %d, devstate = 0x%d \n", Port, DevInfo->usbdev_state));

  DevInfo->usbdev_state = USB_DEVICE_ENUMFAIL;

  //
  //Do not perform port reset for only Audio class devices, as that would lead to a re-enumeration try  
  //and possible failure of other connected device enumeration on the hub 
  //
  if (IsAudioClass != 1) {
    Status = HubApi->ResetPort (HubIf, Port, FALSE, NULL);
  }

  if (EFI_USB_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "UsbEnumNewDevRootHub: failed to reset port %d - %r\n", Port, Status));
  }
  if (Address != Bus->MaxDevices) {
    Bus->Devices[Address] = NULL;
  }
  if (Child != NULL) {
    UsbFreeDevice (Child);
  }

  HubApi->ClearPortChange (HubIf, Port, FALSE, NULL);

  if (PortState != NULL) {
    gBS->FreePool (PortState);
  }

  //Setting the sync variable to be TRUE for the re-use of the root hub enumeration
  SyncState->syncvariable = TRUE;

  DEBUG ((EFI_D_INFO, "UsbEnumNewDevRootHub -- Error: port %d, devstate = 0x%d \n", Port, DevState));

  return EFI_DEVICE_ERROR;
}


/**
  Enumerate and configure the new device on the port of this HUB interface.
  Handler function for USB  asynchronous control in transfer.

  @param  Data             A pointer to a buffer that is filled with key data which is
                           retrieved via asynchronous control transfer.
  @param  DataLength       Indicates the size of the data buffer.
  @param  Context          Pointing to USB_KB_DEV instance.
  @param  Result           Indicates the result of the asynchronous control transfer.
  @param  Toggle           Not used, need to keep for reusage of the async callback function format

  @retval EFI_SUCCESS      Asynchronous bulk in transfer is handled successfully.
  @retval EFI_DEVICE_ERROR Hardware error occurs.

**/

EFI_STATUS
EFIAPI
UsbEnumNewDevHub (
  IN  VOID        *Data,
  IN  UINTN       DataLength,
  IN  VOID        *Context,
  IN  UINT32      Result,
  IN UINT8        Toggle
  )
{
  USB_DEVICE                *Child;
  USB_INTERFACE_CONTEXT     *HubIfCont;
  USB_CONTEXT               *HubCont;
  USB_INTERFACE             *HubIf;
  EFI_USB_PORT_STATUS       *PortState = NULL;
  USB_HUB_SYNC_STATUS       *SyncState = NULL;
  UINT8                     Port;
  EFI_STATUS                Status = EFI_SUCCESS;
  USB_DEVICE                *Parent;
  UINT8                     Config = 0;
  usb_device_state_type     DevState;
  USB_HUB_API               *HubApi;
  USB_BUS                   *Bus;
  UINTN                     Address = 0;
  UINTN                     Index;
  UINT8                     Index_c = 0;
  UINT8                     Index_i = 0;
  UINTN                     IsAudioClass = 0;
  USB_DEVICE_INFO           *DevInfo;

  USB_ASSERT_RETURN_STR ((Context != NULL), EFI_INVALID_PARAMETER,
                         "UsbEnumNewDevHub: Assert since Hub Context is NULL");

  HubIfCont = (USB_INTERFACE_CONTEXT *) Context;
  HubIf     = HubIfCont->usbif;
  USB_ASSERT_RETURN ((HubIf != NULL), EFI_DEVICE_ERROR);

  HubCont   = HubIfCont->usb_contxt;
  USB_ASSERT_RETURN ((HubCont != NULL), EFI_DEVICE_ERROR);

  DevInfo   = HubCont->deviceinfo;
  USB_ASSERT_RETURN ((DevInfo != NULL), EFI_DEVICE_ERROR);

  //Check for valid HubIf in case of forcefully calling StopUsbController which will remove the hub interface
  if(HubIf->Signature != USB_INTERFACE_SIGNATURE)
  {
    DEBUG ((EFI_D_ERROR, "Invalid Signature for Hub = 0x%x \n", HubIf->Signature));
    Status = EFI_ACCESS_DENIED;
    return Status;
  }
  
  DevState  = DevInfo->usbdev_state;
  Parent    = HubIf->Device;
  Bus       = Parent->Bus;
  Child     = HubCont->Child;
  HubApi    = HubIfCont->usbif->HubApi;
  Port      = DevInfo->Port;
  PortState = DevInfo->PortState;
  SyncState = DevInfo->syncstatus;

  DEBUG ((EFI_D_INFO, "UsbEnumNewDevHub: Entry point port %d, Hub Device State=0x%r \n", Port, DevState ));

  switch (DevState)
  {
    case USB_DEVICE_STABPORT:
    { // Wait for port stablization timeout to make sure device is connected properly before perfoming the port reset
      PortState = (EFI_USB_PORT_STATUS *)Data;
      DevInfo->PortState = PortState;
      DevInfo->usbdev_state = USB_DEVICE_RSTPORT;
      Status = USBHubReset_Stall (USB_WAIT_PORT_STABLE_STALL, (void *)HubIfCont);
      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to set USB_WAIT_PORT_STABLE_STALL for port %d, Error = 0x%r \n", Port, Status));
        goto ON_ERROR;
      }
      break;
    }
    case USB_DEVICE_RSTPORT:
    { // Perform port reset
      UINT64 ResetWaitTime;
      Status = HubApi->GetPortStatus (HubIf, Port, PortState, FALSE, NULL);

      if (EFI_ERROR (Status)) {
        goto ON_ERROR;
      } else if (USB_BIT_IS_SET (PortState->PortChangeStatus, USB_PORT_STAT_C_RESET)) {
        DEBUG (( EFI_D_INFO, "UsbHubResetPort: skip reset on hub %p port %d\n", HubIf, Port));
        DevInfo->usbdev_state = USB_DEVICE_ENUMCOMPLETE;
        ResetWaitTime = 0;
      } else {
        // Set PORT_RESET feature
        Status  = HubApi->SetPortFeature (HubIf, Port, (EFI_USB_PORT_FEATURE) USB_HUB_PORT_RESET, FALSE, NULL);
        if (EFI_USB_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to SetPortFeature for port %d\n", Port));
          goto ON_ERROR;
        }
      
        //
        // Drive the reset signal for at least 10ms. Check USB 2.0 Spec
        // section 7.1.7.5 for timing requirements.
        //
        DevInfo->usbdev_state = USB_DEVICE_RSTPORT1;
        ResetWaitTime = USB_SET_PORT_RESET_STALL;
      }
      Status = USBHubReset_Stall (ResetWaitTime, (void *)HubIfCont);
      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to set USB_SET_PORT_RESET_STALL for port %d\n", Port));
        goto ON_ERROR;
      }
      break;
    }
    case USB_DEVICE_RSTPORT1:
    { 
      // Clear port reset
      //
      // USB hub will clear RESET bit if reset is actually finished.
      for (Index = 0; Index < USB_WAIT_PORT_STS_CHANGE_LOOP; Index++) {
        Status = HubApi->GetPortStatus(HubIf, Port, PortState, FALSE, NULL);

        if (EFI_ERROR (Status)) {
          goto ON_ERROR;
        }
        if (USB_BIT_IS_SET (PortState->PortChangeStatus, USB_PORT_STAT_C_RESET)) {
          break;
        }
        gBS->Stall (USB_WAIT_PORT_STS_CHANGE_STALL);
      }

      if (Index == USB_WAIT_PORT_STS_CHANGE_LOOP) {
        Status = EFI_TIMEOUT;
        goto ON_ERROR;
      }
      
      DevInfo->usbdev_state = USB_DEVICE_ENUMCOMPLETE;
      Status = USBHubReset_Stall (USB_SET_PORT_RECOVERY_STALL, (void *)HubIfCont);
      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to set USB_SET_PORT_RECOVERY_STALL for port %d\n", Port));
        goto ON_ERROR;
      }
      break;
    }
    case USB_DEVICE_ENUMCOMPLETE:
    { // Perform rest of enumeration till the completion
      DEBUG (( EFI_D_INFO, "UsbEnumNewDevHub: hub port %d is reset\n", Port));
      Child = UsbCreateDevice (HubIf, Port);
      if (Child == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto ON_ERROR;
      }
      HubIfCont->usb_contxt->Child = Child;
      //
      // OK, now identify the device speed. After reset, hub
      // fully knows the actual device speed.
      //
      DevInfo->usbdev_state = USB_DEVICE_ENUMCOMPLETE;
      Status = HubApi->GetPortStatus (HubIfCont->usbif, Port, PortState, FALSE, NULL);
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to GetPortStatus port %d, Duration \n", Port));
        goto ON_ERROR;
      }

      if (!USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_CONNECTION)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: No device present at port %d\n", Port));
        goto ON_ERROR;
      }
      // Check Port/Device Speed
      if (USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_SUPER_SPEED)){
        Child->Speed      = EFI_USB_SPEED_SUPER;
        Child->MaxPacket0 = 512;
      } else if (USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_HIGH_SPEED)) {
        Child->Speed      = EFI_USB_SPEED_HIGH;
        Child->MaxPacket0 = 64;
      } else if (USB_BIT_IS_SET (PortState->PortStatus, USB_PORT_STAT_LOW_SPEED)) {
        Child->Speed      = EFI_USB_SPEED_LOW;
        Child->MaxPacket0 = 8;
      } else {
        Child->Speed      = EFI_USB_SPEED_FULL;
        Child->MaxPacket0 = 8;
      }

      DEBUG (( EFI_D_INFO, "UsbEnumNewDevHub: device is of %d speed\n", Child->Speed));
      if (((Child->Speed == EFI_USB_SPEED_LOW) || (Child->Speed == EFI_USB_SPEED_FULL)) &&
          (Parent->Speed == EFI_USB_SPEED_HIGH)) {
        //
        // If the child is a low or full speed device, it is necessary to
        // set the transaction translator. Port TT is 1-based.
        // This is quite simple:
        //  1. if parent is of high speed, then parent is our translator
        //  2. otherwise use parent's translator.
        //
        Child->Translator.TranslatorHubAddress  = Parent->Address;
        Child->Translator.TranslatorPortNumber  = (UINT8) (Port + 1);
      } else {
        Child->Translator = Parent->Translator;
      }
      DEBUG (( EFI_D_INFO, "UsbEnumNewDevHub: device uses translator (%d, %d)\n",
              Child->Translator.TranslatorHubAddress,
              Child->Translator.TranslatorPortNumber));
      //
      // After port is reset, hub establishes a signal path between
      // the device and host (DEFALUT state). Device's registers are
      // reset, use default address 0 (host enumerates one device at
      // a time) , and ready to respond to control transfer at EP 0.
      //

      //
      // Host assigns an address to the device. Device completes the
      // status stage with default address, then switches to new address.
      // ADDRESS state. Address zero is reserved for root hub.
      //
      USB_ASSERT_GOTO ((Bus->MaxDevices <= 256), ON_ERROR);
      for (Address = 1; Address < Bus->MaxDevices; Address++) {
        if (Bus->Devices[Address] == NULL) {
          break;
        }
      }
      if (Address >= Bus->MaxDevices) {
        DEBUG ((EFI_D_INFO, "UsbEnumNewDevHub: address pool is full for port %d\n", Port));
        Status = EFI_ACCESS_DENIED;
        goto ON_ERROR;
      }
      DevInfo->Address      = Address;
      Status                = UsbSetAddress (Child, (UINT8)Address, FALSE, NULL);
      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to set device address - %r\n", Status));
        goto ON_ERROR;
      }

      Child->Address        = (UINT8)Address;
      Bus->Devices[Address] = Child;
      gBS->Stall (USB_SET_DEVICE_ADDRESS_STALL);
      DEBUG ((EFI_D_INFO, "UsbEnumerateNewDev_ToHub: device is now ADDRESSED at %d\n", Address));
      Status = UsbGetMaxPacketSize0 (Child, FALSE, NULL);
      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to get max packet for EP 0 - %r\n", Status));
        goto ON_ERROR;
      }
      DEBUG (( EFI_D_INFO, "UsbEnumNewDevHub: max packet size for EP 0 is %d\n", Child->MaxPacket0));
      //
      // Host learns about the device's abilities by requesting device's
      // entire descriptions.
      //
      Status = UsbBuildDescTable (Child, FALSE, NULL);
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to build descriptor table - %r\n", Status));
        goto ON_ERROR;
      }
     
      //
      //Drop audio devices as they are not fully parsed(leading to crash during set_config) and also
      //no class driver support in the stack
      //
      if (Child && Child->DevDesc) {
        for (Index_c = 0; Index_c < Child->DevDesc->Desc.NumConfigurations && Child->DevDesc->Configs[Index_c]; Index_c++) {
          for (Index_i = 0; Index_i < Child->DevDesc->Configs[Index_c]->Desc.NumInterfaces; Index_i++) {
            if (Child->DevDesc->Configs[Index_c]->Interfaces[Index_i] && Child->DevDesc->Configs[Index_c]->Interfaces[Index_i]->Settings[0]) {
              if(Child->DevDesc->Configs[Index_c]->Interfaces[Index_i]->Settings[0]->Desc.InterfaceClass == USB_AUDIO_CLASS) {
                DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: dropping audio device on port %d\n", Port));
                IsAudioClass = 1;
                goto ON_ERROR;
              }
            }
          }
        }
      }
 
      //
      // Select a default configuration: UEFI must set the configuration
      // before the driver can connect to the device.
      //
      
      Config = Child->DevDesc->Configs[0]->Desc.ConfigurationValue;
      Status = UsbSetConfig (Child, Config, FALSE, NULL);
      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to UsbSetConfig - %r\n", Status));
        goto ON_ERROR;
      }
      Config = Child->DevDesc->Configs[0]->Desc.ConfigurationValue;
      //
      // Host assigns and loads a device driver.
      // Again making it synchronous, need to pass async conext so as to pass on the sync variable to child hub
      Status = UsbSelectConfig (Child, Config, FALSE, HubIfCont);
      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to select config port %d\n", Port));
        goto ON_ERROR;
      }

      //
      // Report Status Code to indicate USB device has been detected by hotplug
      //
      REPORT_STATUS_CODE_WITH_DEVICE_PATH (
        EFI_PROGRESS_CODE,
        (EFI_IO_BUS_USB | EFI_IOB_PC_HOTPLUG),
        Bus->DevicePath
        );

      HubApi->ClearPortChange (HubIf, Port, FALSE, NULL);
      Status = UsbHubAckHubStatus (HubIf->Device, FALSE, NULL);
      if (EFI_USB_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to UsbHubAckHubStatus port %d\n", Port));
        goto ON_ERROR;
      }
      //Setting the sync variable to be TRUE for the re-use of the root hub enumeration polling
      SyncState->syncvariable = TRUE;
      break;
    }

    // No action required, cases included to silence compiler
    case USB_DEVICE_RSTPORT2:
    case USB_DEVICE_GETSPEED:
    case USB_DEVICE_SETCONFIG:
    case USB_DEVICE_ENUMFAIL:
      break;
  }
  DEBUG ((EFI_D_INFO, "UsbEnumNewDevHub --: port %d, devstate = 0x%d \n", Port, DevInfo->usbdev_state));
  return Status;

ON_ERROR:
  //
  // If reach here, it means the enumeration process on a given port is interrupted due to error.
  // The s/w resources, including the assigned address(Address) and the allocated usb device data
  // structure(Bus->Devices[Address]), will NOT be freed here. These resources will be freed when
  // the device is unplugged from the port or DriverBindingStop() is invoked.
  //
  // This way is used to co-work with the lower layer EDKII UHCI/EHCI/XHCI host controller driver.
  // It's mainly because to keep UEFI spec unchanged EDKII XHCI driver have to maintain a state machine
  // to keep track of the mapping between actual address and request address. If the request address
  // (Address) is freed here, the Address value will be used by next enumerated device. Then EDKII XHCI
  // host controller driver will have wrong information, which will cause further transaction error.
  //
  // EDKII UHCI/EHCI doesn't get impacted as it's make sense to reserve s/w resource till it gets unplugged.
  //

  DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub --:Enumeration Failure port %d, devstate = 0x%d \n", Port, DevInfo->usbdev_state));

  DevInfo->usbdev_state = USB_DEVICE_ENUMFAIL;
  
  //
  //Do not perform port reset for only Audio class devices, as that would lead to a re-enumeration try  
  //and possible failure of other connected device enumeration on the hub 
  // 
  if (IsAudioClass != 1) {
    Status = HubApi->ResetPort (HubIf, Port, FALSE, NULL);
  }

  if (EFI_USB_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "UsbEnumNewDevHub: failed to reset port %d - %r\n", Port, Status));
  }

  //vsb: Do not dealloc here. Comment above
  /*if (Address != Bus->MaxDevices) {
    Bus->Devices[Address] = NULL;
  }
  if (Child != NULL) {
    UsbFreeDevice (Child);
  }*/

  if (PortState != NULL) {
    gBS->FreePool (PortState);
  }

  HubApi->ClearPortChange (HubIf, Port, FALSE, NULL);

  //Setting the sync variable to be TRUE for the re-use of the root hub enumeration
  SyncState->syncvariable = TRUE;

  DEBUG ((EFI_D_INFO, "UsbEnumNewDevHub -- Error: port %d, devstate = 0x%d \n", Port, DevState));

  return EFI_DEVICE_ERROR;
}

/**
  Enumerate all the changed hub ports.

  @param  Event                 The event that is triggered.
  @param  Context               The context to the event.

**/
VOID
EFIAPI
UsbHubEnumeration (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  )
{
  USB_INTERFACE_CONTEXT           *HubIf;
  UINT8                            Byte;
  UINT8                            Bit;
  UINT8                            Index;
  UINT8                            NumPort;
  UINT8                           *ChMap;
  USB_DEVICE_INFO                 *DevInfo;
  USB_DEVICE                      *Child;

  if (Context == NULL)
  {
      DEBUG ((EFI_D_ERROR, "UsbHubEnumeration: Error Hub Context is NULL \n"));
      return;
  }

  HubIf = (USB_INTERFACE_CONTEXT *) Context;
  USB_ASSERT (HubIf != NULL);

  USB_ASSERT (HubIf->usb_contxt != NULL);
  USB_ASSERT (HubIf->usbif != NULL);
  
  HubIf->usb_contxt->callfrom_roothub = FALSE;
  NumPort = HubIf->usbif->NumOfPort;
  DevInfo = HubIf->usb_contxt->deviceinfo;

  for (Index = 0; Index < NumPort; Index++) {
    Child = UsbFindChild (HubIf->usbif, Index);
    if ((Child != NULL) && (Child->DisconnectFail == TRUE)) {
      DEBUG (( EFI_D_ERROR, "UsbHubEnumeration: The device disconnect fails at port %d from hub %p, try again\n", Index, HubIf->usbif));
      UsbRemoveDevice (Child);
    }
  }


  if (HubIf->usbif->ChangeMap == NULL) {
    return ;
  }

  ChMap = HubIf->usbif->ChangeMap;

  //
  // HUB starts its port index with 1.
  //
  Byte  = 0;
  Bit   = 1;

  for (Index = 0; Index < NumPort; Index++) {
    if (USB_BIT_IS_SET (ChMap[Byte], USB_BIT (Bit))) {
        DEBUG ((EFI_D_INFO, "UsbEnumNewDevHub ++: port %d, HubIf=0x%x  \n", Index, HubIf));
        if (DevInfo->syncstatus->syncvariable == TRUE)
          UsbEnumeratePort (HubIf, Index);
    }

    USB_NEXT_BIT (Byte, Bit);
  }

  gBS->FreePool (ChMap);
  ChMap = NULL;

  return ;
}


/**
  Enumerate all the changed hub ports.

  @param  Event                 The event that is triggered.
  @param  Context               The context to the event.

**/
VOID
EFIAPI
UsbRootHubEnumeration (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  )
{
  USB_INTERFACE_CONTEXT           *RootHub;
  UINT8                           NumPort;
  USB_DEVICE_INFO                 *DevInfo;
  UINT8                           Index;
  USB_DEVICE                      *Child;

  if (Context == NULL)
  {
      DEBUG ((EFI_D_ERROR, "UsbRootHubEnumeration: Error Root Hub Context is NULL \n"));
      return;
  }


  RootHub = (USB_INTERFACE_CONTEXT *) Context;

  USB_ASSERT (RootHub->usbif != NULL);
  USB_ASSERT (RootHub->usb_contxt != NULL);

  NumPort = RootHub->usbif->NumOfPort;
  DevInfo =   RootHub->usb_contxt->deviceinfo;

  RootHub->usb_contxt->callfrom_roothub = TRUE;

  for (Index = 0; Index < NumPort; Index++) {
    Child = UsbFindChild (RootHub->usbif, Index);
    if ((Child != NULL) && (Child->DisconnectFail == TRUE)) {
      DEBUG (( EFI_D_ERROR, "UsbEnumeratePort: The device disconnect fails at port %d from root hub %p, try again\n", Index, RootHub->usbif));
      UsbRemoveDevice (Child);
    }
  }

  for (RootHub->usb_contxt->Index = 0; RootHub->usb_contxt->Index < NumPort; RootHub->usb_contxt->Index++) {
    if (DevInfo->syncstatus->syncvariable == TRUE)
      UsbEnumeratePort (RootHub, (RootHub->usb_contxt->Index));
  }
}
