/** @file

    USB bus enumeration interface.

Portions copyright (c) 2010 - 2017 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
Copyright (c) 2007, Intel Corporation. All rights reserved.<BR>
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
 07/31/17   pm       Added USB Audio Class Macro 
 02/05/14   amitg    Async USB Host Stack
 01/20/14   ck       Port EDK2 host stack
 03/30/11   sniedzie Port fix for enumeration failure.
 03/30/11   sniedzie Integrated from MdeModulePkg for QCOM changes.

=============================================================================*/

#ifndef _USB_ENUMERATION_H_
#define _USB_ENUMERATION_H_


#define EFI_USB_ERROR(x) (x != EFI_SUCCESS)
#define USB_AUDIO_CLASS  0x1


//
// Advance the byte and bit to the next bit, adjust byte accordingly.
//
#define USB_NEXT_BIT(Byte, Bit)   \
          do {                \
            (Bit)++;          \
            if ((Bit) > 7) {  \
              (Byte)++;       \
              (Bit) = 0;      \
            }                 \
          } while (0)

//
// USB device state machine 
// for the slave device enumeraiton connected to root hub / hub
//
typedef enum
{
    USB_DEVICE_STABPORT,
    USB_DEVICE_RSTPORT,
    USB_DEVICE_RSTPORT1,
    USB_DEVICE_RSTPORT2,
    USB_DEVICE_GETSPEED,
    USB_DEVICE_SETCONFIG,
    USB_DEVICE_ENUMCOMPLETE,
    USB_DEVICE_ENUMFAIL
} usb_device_state_type;

//
//Sync variable is used to resolve race condition for multiple slave devices on hub
//Sync variable is inherited from the root hub to the child hub in case of actual async control transfers
// Otherwise we can set this on per hub basis
//
typedef struct {
  BOOLEAN         syncvariable;        /**< Current sync variable state. */
} USB_HUB_SYNC_STATUS;

//
// USB slave device info
//
typedef struct
{
  usb_device_state_type usbdev_state;
  UINT8                 Port;
  UINTN                 Address;
  EFI_USB_PORT_STATUS   *PortState;
  USB_HUB_SYNC_STATUS   *syncstatus;
} USB_DEVICE_INFO;

//
// USB context having hub and slave device specific info
//
typedef struct _USB_CONTEXT
{
    USB_DEVICE                               *Child;
    BOOLEAN                                  callfrom_roothub;
    UINT8                                    Index;
    USB_DEVICE_INFO                          *deviceinfo;
    EFI_EVENT                                USBHubResetTimer;
    EFI_EVENT                                USBRootHubResetTimer;
    EFI_ASYNC_USB_TRANSFER_CALLBACK_INT_BULK CallBackFunction;
} USB_CONTEXT;


typedef struct _USB_INTERFACE_CONTEXT
{
    USB_INTERFACE   *usbif;
    USB_CONTEXT     *usb_contxt;
    UINT8           index;
} USB_INTERFACE_CONTEXT;

//
// Common interface used by usb bus enumeration process.
// This interface is defined to mask the difference between
// the root hub and normal hub. So, bus enumeration code
// can be shared by both root hub and normal hub
//
typedef
EFI_STATUS
(*USB_HUB_INIT) (
  IN USB_INTERFACE                *UsbIf,
  IN BOOLEAN                      AsyncFlag,
  IN  VOID                        *AsyncConext
  );

//
// Get the port status. This function is required to
// ACK the port change bits although it will return
// the port changes in PortState. Bus enumeration code
// doesn't need to ACK the port change bits.
//
typedef
EFI_STATUS
(*USB_HUB_GET_PORT_STATUS) (
  IN  USB_INTERFACE       *UsbIf,
  IN  UINT8                        Port,
  OUT EFI_USB_PORT_STATUS          *PortState,
  IN BOOLEAN                       AsyncFlag,
  IN  VOID                         *AsyncConext
  );

typedef
VOID
(*USB_HUB_CLEAR_PORT_CHANGE) (
  IN USB_INTERFACE        *HubIf,
  IN UINT8                        Port,
  IN BOOLEAN                      AsyncFlag,
  IN  VOID                       *AsyncConext
  );

typedef
EFI_STATUS
(*USB_HUB_SET_PORT_FEATURE) (
  IN USB_INTERFACE                *UsbIf,
  IN UINT8                         Port,
  IN EFI_USB_PORT_FEATURE          Feature,
  IN BOOLEAN                       AsyncFlag,
  IN  VOID                         *AsyncConext
  );

typedef
EFI_STATUS
(*USB_HUB_CLEAR_PORT_FEATURE) (
  IN USB_INTERFACE               *UsbIf,
  IN UINT8                        Port,
  IN EFI_USB_PORT_FEATURE         Feature,
  IN BOOLEAN                      AsyncFlag,
  IN  VOID                       *AsyncConext
  );

typedef
EFI_STATUS
(*USB_HUB_RESET_PORT) (
  IN USB_INTERFACE                *UsbIf,
  IN UINT8                         Port,
  IN BOOLEAN                       AsyncFlag,
  IN  VOID                        *AsyncConext
  );

typedef
EFI_STATUS
(*USB_HUB_RELEASE) (
  IN USB_INTERFACE        *UsbIf
  );

/**
  Return the endpoint descriptor in this interface.

  @param  UsbIf                 The interface to search in.
  @param  EpAddr                The address of the endpoint to return.

  @return The endpoint descriptor or NULL.

**/
USB_ENDPOINT_DESC*
UsbGetEndpointDesc (
  IN USB_INTERFACE        *UsbIf,
  IN UINT8                EpAddr
  );

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
  );

/**
  Select a new configuration for the device. Each
  device may support several configurations.

  @param  Device                The device to select configuration.
  @param  ConfigIndex           The index of the configuration ( != 0).

  @retval EFI_NOT_FOUND         There is no configuration with the index.
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate resource.
  @retval EFI_SUCCESS           The configuration is selected.

**/
EFI_STATUS
UsbSelectConfig (
  IN USB_DEVICE           *Device,
  IN UINT8                ConfigIndex,
  IN BOOLEAN              AsyncFlag,
  IN  VOID                *AsyncConext
  );

/**
  Remove the current device configuration.

  @param  Device                The USB device to remove configuration from.

  @retval EFI_SUCCESS           The USB interface successfully disconnected
  @retval OTHERS                The operation fails.

**/
EFI_STATUS
UsbRemoveConfig (
  IN USB_DEVICE           *Device
  );

/**
  Remove the device and all its children from the bus.

  @param  Device                The device to remove.

  @retval EFI_SUCCESS           The device is removed.

**/
EFI_STATUS
UsbRemoveDevice (
  IN USB_DEVICE           *Device
  );

/**
  Enumerate all the changed hub ports.

  @param  Event                 The event that is triggered.
  @param  Context               The context to the event.

  @return None.

**/
VOID
EFIAPI
UsbHubEnumeration (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  );

/**
  Enumerate all the changed hub ports.

  @param  Event                 The event that is triggered.
  @param  Context               The context to the event.

  @return None.

**/
VOID
EFIAPI
UsbRootHubEnumeration (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  );

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
  IN  UINT8        Toggle
  );

/**
  Handler function for USB  asynchronous control in transfer.

  This function is the handler function for USB Ethernet's asynchronous bulk in transfer.
   It parses data returned from asynchronous bulk in transfer.

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
  IN  UINT8        Toggle
  );

/**
  Set the timer relatively for the duration of the Stall for the  hub

  @param  MicroSeconds                 Duration of Stall
  @param  Context                         Context that needs to be passed after the reset timer completion 

  @retval EFI_SUCCESS                 Set Timer scheduled successfully

**/

EFI_STATUS
USBHubReset_Stall(
  IN UINTN  MicroSeconds,
  IN VOID   *Context
);

/**
   Callback funciton on completion of reset timer stall for the hub

  @param  Event                 Timer Event Signalled on completion of relative timer
  @param  Context              Context returned

  @retval EFI_SUCCESS           The root hub's control of the interface is
                                released.

**/

VOID
EFIAPI
UsbHubResetCompletion (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  );

/**
  Set the timer relatively for the duration of the Stall for the root hub

  @param  MicroSeconds                 Duration of Stall
  @param  Context                         Context that needs to be passed after the reset timer completion 

  @retval EFI_SUCCESS                 Set Timer scheduled successfully

**/

EFI_STATUS
USBRootHubReset_Stall(
  IN UINTN  MicroSeconds,
  IN VOID   *Context
);

/**
   Callback funciton on completion of reset timer stall for the root hub

  @param  Event                 Timer Event Signalled on completion of relative timer
  @param  Context              Context returned

  @retval EFI_SUCCESS           The root hub's control of the interface is
                                released.

**/

VOID
EFIAPI
UsbRootHubResetCompletion (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  );

#endif