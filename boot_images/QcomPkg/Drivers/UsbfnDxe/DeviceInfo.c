/** @file DeviceInfo.c

  Helper functions to convert and store Usb device information

  Copyright (c) 2010-2014, QUALCOMM Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/30/14   ar      Cleanup for NDEBUG build  
 06/24/11   cching  port the UsbfnIo protocol

=============================================================================*/


#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include "Usbfn.h"
#include "qhsusb_dcd.h"
#include "qhsusb_ch9.h"
#include "UsbUrb.h"

// TODO: we want macros for getting addresses of device descriptors. The +1 +i
// is confusing at first glance.

typedef struct LinkType {
  struct LinkType *Next;
} LinkType;

/**
  Init singly-linked list

  @param  Head              A pointer to the head node of a singly-linked list.

**/
STATIC
EFI_STATUS
SllInit (
  VOID **Head
  )
{
  USB_ASSERT_RETURN(Head, EFI_INVALID_PARAMETER);

  *Head = NULL;
  
  return EFI_SUCCESS;
}


/**
  Adds a node to the beginning of a singly-linked list, and returns the pointer
  to the head node of the singly-linked list.

  @param  Head              A pointer to the head node of a singly-linked list.
  @param  Link              A pointer to a node that is to be inserted at the beginning
                            of a singly-linked list.


**/
STATIC
VOID *
SllPush (
  VOID *Head,
  VOID *Link
  )
{
  LinkType *h = Head;
  LinkType *l = Link;

  if (!l)
    return Head;

  l->Next = h;
  return Link;
}

/**
  Copy a USB endpoint

  @param  ifc                   A pointer to a USB interface the endpoint belongs to
  @param  endpoint              A pointer to the endpoint that belong to the ifc
  @param  EndpointDescriptor    A pointer to an endpoint to be copied.

  @retval EFI_SUCCESS           The endpoint is successfully copied.
  @retval EFI_INVALID_PARAMETER The parameters passed in are not valid.
  @retval EFI_OUT_OF_RESOURCES  No memory left to copy the new endpoint.

**/
EFI_STATUS
CopyEndpoint (
  IN   struct qhsusb_dcd_dsc_interface  *ifc,
  OUT  struct qhsusb_dcd_dsc_endpoint   **endpoint,
  IN   EFI_USB_ENDPOINT_DESCRIPTOR      *EndpointDescriptor,
  UINTN i
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if(!ifc || !endpoint || !EndpointDescriptor){
    DBG(EFI_D_ERROR,"Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  ALLOC (*endpoint, sizeof (struct qhsusb_dcd_dsc_endpoint));

  (*endpoint)->next           = NULL;
  (*endpoint)->descriptor = (usb_desc_endpoint_t *) (ifc->descriptor + 1) + i;
  CopyMem (
    (*endpoint)->descriptor,
    EndpointDescriptor,
    sizeof (EFI_USB_ENDPOINT_DESCRIPTOR)
    );
  (*endpoint)->ifc         =  ifc;
  (*endpoint)->setup       =  ifc->configuration->device->setup_ep;
  (*endpoint)->is_halt     =  0;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Copy all the endpoints passed by a user in the device information

  @param  ifc                A pointer to a USB interface the endponts belong to
  @param  endpoints          A pointer to endpoints that belong to the ifc
  @param  EndpointDescriptor A pointer to endpoints to be copied.

  @retval EFI_SUCCESS           The endpoints are successfully copied.
  @retval EFI_INVALID_PARAMETER The parameters passed in are not valid.
  @retval EFI_OUT_OF_RESOURCES  No memory left to copy the new endpoints.

**/
EFI_STATUS
CopyEndpoints (
  IN   struct qhsusb_dcd_dsc_interface  *ifc,
  OUT  struct qhsusb_dcd_dsc_endpoint   **endpoints,
  IN   EFI_USB_ENDPOINT_DESCRIPTOR      **EndpointDescriptorTable
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;
  UINTN       i       =  0;

  FNC_ENTER_MSG ();

  if(!ifc || !ifc->descriptor || !EndpointDescriptorTable || !endpoints){
    DBG(EFI_D_ERROR,"Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Status = SllInit ((VOID **)endpoints);
  if (EFI_ERROR(Status)){
    DBG(EFI_D_ERROR,"Invalid Parameters");
    goto ON_EXIT;
  }
    
  for (i = 0; i < ifc->descriptor->bNumEndpoints; ++i) {
    struct qhsusb_dcd_dsc_endpoint *endpoint = NULL;

    if(!EndpointDescriptorTable[i]){
        DBG(EFI_D_ERROR,"EndpointDescriptorTable[%d] (0x%p) is null", i, EndpointDescriptorTable[i]);
        Status = EFI_INVALID_PARAMETER;
        goto ON_EXIT;
    }

    Status = CopyEndpoint (ifc, &endpoint, EndpointDescriptorTable[i], i);
    ERR_CHK ("failed to copy endpoint x%02x", EndpointDescriptorTable[i]->EndpointAddress);

    *endpoints = SllPush (*endpoints, endpoint);
  }

  Status = EFI_SUCCESS;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Copy a USB interface

  @param  configuration         A pointer to a USB configuration the
                                the interface belongs to
  @param  interface             A pointer to a USB interface that
                                belong to the configuration
  @param  InterfaceInfo         A pointer to an interface to be copied

  @retval EFI_SUCCESS           The interface is successfully copied.
  @retval EFI_INVALID_PARAMETER The parameters passed in are not valid.
  @retval EFI_OUT_OF_RESOURCES  No memory left to copy the new interface.

**/
EFI_STATUS
CopyInterface (
  IN   struct qhsusb_dcd_dsc_configuration  *configuration,
  OUT  struct qhsusb_dcd_dsc_interface      **interface,
  IN   EFI_USB_INTERFACE_INFO               *InterfaceInfo,
  UINTN i
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if(!configuration || !interface || !InterfaceInfo || !InterfaceInfo->InterfaceDescriptor){
    DBG(EFI_D_ERROR,"Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  ALLOC (*interface, sizeof (struct qhsusb_dcd_dsc_interface));

  (*interface)->next                 =  NULL;

  (*interface)->descriptor = (usb_desc_interface_t *) (configuration->descriptor + 1) + i;
  CopyMem (
    (*interface)->descriptor,
    InterfaceInfo->InterfaceDescriptor,
    sizeof (EFI_USB_INTERFACE_DESCRIPTOR)
    );

  (*interface)->configuration        =  configuration;

  (*interface)->endpoints            =  NULL;
  Status = CopyEndpoints (
             *interface,
             &(*interface)->endpoints,
             InterfaceInfo->EndpointDescriptorTable
             );
  ERR_CHK (
    "failed to copy endpoints for interface x%02",
    InterfaceInfo->InterfaceDescriptor->InterfaceNumber
    );

  (*interface)->setup                =  configuration->device->setup_ifc;
  (*interface)->current_altsetting   =  0;

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Copy all the USB interfaces passed by a user in the device information

  @param  configuration         A pointer to a USB configuration the interfaces
                                belong to
  @param  interfaces            A pointer to the interfaces that belong to
                                the configuration
  @param  InterfaceInfo         A pointer to the interfaces to be copied

  @retval EFI_SUCCESS           The interfaces are successfully copied.
  @retval EFI_INVALID_PARAMETER The parameters passed in are not valid.
  @retval EFI_OUT_OF_RESOURCES  No memory left to copy the new interfaces.

**/
EFI_STATUS
CopyInterfaces (
  IN   struct qhsusb_dcd_dsc_configuration  *configuration,
  OUT  struct qhsusb_dcd_dsc_interface      **interfaces,
  IN   EFI_USB_INTERFACE_INFO               **InterfaceInfo
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;
  UINTN       i       =  0;

  FNC_ENTER_MSG ();

  if(!configuration || !interfaces || !InterfaceInfo){
    DBG(EFI_D_ERROR,"Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Status = SllInit ((VOID **)interfaces);
  if (EFI_ERROR(Status)){
    DBG(EFI_D_ERROR,"Invalid Parameters");
    goto ON_EXIT;
  }

  for (i = 0; i < configuration->descriptor->bNumInterfaces; ++i) {
    struct qhsusb_dcd_dsc_interface *interface = NULL;

    if(!InterfaceInfo[i] || !InterfaceInfo[i]->InterfaceDescriptor){
        DBG(EFI_D_ERROR,"Invalid InterfaceInfo");
        Status = EFI_INVALID_PARAMETER;
        goto ON_EXIT;
    }

    Status = CopyInterface (
               configuration,
               &interface,
               InterfaceInfo[i],
               i
               );
    ERR_CHK ("failed to copy interface x%02x", InterfaceInfo[i]->InterfaceDescriptor->InterfaceNumber);

    *interfaces = SllPush (*interfaces, interface);
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Copy a USB configuration.

  @param  device         A pointer to a USB device the configuration belongs to
  @param  configuration  A pointer to a USB configuration that belongs to the
                         device
  @param  ConfigInfo     A pointer to the configuration to be copied.

  @retval EFI_SUCCESS           The configuration is successfully copied.
  @retval EFI_INVALID_PARAMETER The parameters passed in are not valid.
  @retval EFI_OUT_OF_RESOURCES  No memory left to copy the new configuration.

**/
EFI_STATUS
CopyConfiguration (
  IN   struct qhsusb_dcd_dsc_device         *device,
  OUT  struct qhsusb_dcd_dsc_configuration  **configuration,
  IN   EFI_USB_CONFIG_INFO                  *ConfigInfo
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if(!device || !configuration || !ConfigInfo){
    DBG(EFI_D_ERROR,"Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  ALLOC (*configuration, sizeof (struct qhsusb_dcd_dsc_configuration));

  (*configuration)->next                 =  NULL;

  ALLOC ((*configuration)->descriptor, ConfigInfo->ConfigDescriptor->TotalLength); // whole thing's gotta be contig :(
  CopyMem (
    (*configuration)->descriptor,
    ConfigInfo->ConfigDescriptor,
    sizeof (EFI_USB_CONFIG_DESCRIPTOR)
    );

  (*configuration)->device                =  device;

  (*configuration)->interfaces            =  NULL;
  Status = CopyInterfaces (
             *configuration,
             &(*configuration)->interfaces,
             ConfigInfo->InterfaceInfoTable
             );
  ERR_CHK (
    "failed to copy interfaces for configuration x%02",
    ConfigInfo->ConfigDescriptor->ConfigurationValue
    );

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Copy all the USB configurations passed by a user in the device information

  @param  device         A pointer to a USB device the configurations belong to
  @param  configurations A pointer to USB configurations that belong to the
                         device
  @param  ConfigInfo     A pointer to the configurations to be copied.

  @retval EFI_SUCCESS           The configurations are successfully copied.
  @retval EFI_INVALID_PARAMETER The parameters passed in are not valid.
  @retval EFI_OUT_OF_RESOURCES  No memory left to copy the new configurations.

**/
EFI_STATUS
CopyConfigurations (
  IN   struct qhsusb_dcd_dsc_device         *device,
  OUT  struct qhsusb_dcd_dsc_configuration  **configurations,
  IN   EFI_USB_CONFIG_INFO                  **ConfigInfo
  )
{
  EFI_STATUS  Status  =  EFI_SUCCESS;
  UINTN       i       =  0;

  FNC_ENTER_MSG ();

  if(!device || !device->descriptor || !configurations || !ConfigInfo){
    DBG(EFI_D_ERROR,"Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Status = SllInit ((VOID **)configurations);
  if (EFI_ERROR(Status)){
    DBG(EFI_D_ERROR,"Invalid Parameters");
    goto ON_EXIT;
  }

  for (i = 0; i < device->descriptor->bNumConfigurations; ++i) {
    struct qhsusb_dcd_dsc_configuration *configuration = NULL;

    if(!ConfigInfo[i] || !ConfigInfo[i]->ConfigDescriptor){
        DBG(EFI_D_ERROR,"Invalid Config Info");
        Status = EFI_INVALID_PARAMETER;
        goto ON_EXIT;
    }

    Status = CopyConfiguration (
               device,
               &configuration,
               ConfigInfo[i]
               );
    ERR_CHK ("failed to copy configuration x%02x", ConfigInfo[i]->ConfigDescriptor->ConfigurationValue);

    *configurations = SllPush (*configurations, configuration);
  }

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}



/**
  Copy a USB configuration to a USB device

  @param  device         A pointer to a USB device to store the device
                         information passed by a user.
  @param  DeviceInfo     A pointer to a user supplied USB device information

  @retval EFI_SUCCESS           The configuration is successfully copied.
  @retval EFI_INVALID_PARAMETER The parameters passed in are not valid.
  @retval EFI_OUT_OF_RESOURCES  No memory left to copy the new configuration.

**/
EFI_STATUS
CopyDevice (
  OUT  struct qhsusb_dcd_dsc_device  *device,
  IN   EFI_USB_DEVICE_INFO           *DeviceInfo
  )
{
  // TODO: we need better parameter checking on DeviceInfo throughout. There's a
  // lot of opportunity for unexpected values. However, the protocol doesn't
  // require it.

  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  if(!device || !DeviceInfo){
    DBG(EFI_D_ERROR,"Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  ALLOC (device->descriptor, sizeof (EFI_USB_DEVICE_DESCRIPTOR));
  CopyMem (
    device->descriptor,
    DeviceInfo->DeviceDescriptor,
    sizeof (EFI_USB_DEVICE_DESCRIPTOR)
    );

  device->configurations            =  NULL;
  Status = CopyConfigurations (
             device,
             &device->configurations,
             DeviceInfo->ConfigInfoTable
             );
  WRN_CHK ("failed to copy configurations");

ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


/**
  Free the resources allocated for storing the USB information

  @param  device        A pointer to a USB device whose resources are to be
                        released.

  @retval EFI_SUCCESS   The device resources are released successfully.

**/
EFI_STATUS
FreeDevice (
  IN struct qhsusb_dcd_dsc_device *device
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG ();

  UINT32 i;

  ARG_CHK (device);

  // free device descriptor
  FREE(device->descriptor);

  // free al the qhsusb_dcd_dsc_configuration struct
  while (device->configurations) {
    struct qhsusb_dcd_dsc_configuration *curr_configuration = device->configurations;
    struct qhsusb_dcd_dsc_interface *curr_ifc = curr_configuration->interfaces;

    // free all the qhsusb_dcd_dsc_interface struct
    while (curr_ifc) {
      struct qhsusb_dcd_dsc_interface *free_ifc = curr_ifc;
      struct qhsusb_dcd_dsc_endpoint *curr_endpt = curr_ifc->endpoints;

      // free all the qhsusb_dcd_dsc_endpoint struct
      while (curr_endpt) {
        struct qhsusb_dcd_dsc_endpoint *free_endpt = curr_endpt;
        curr_endpt = curr_endpt->next;
        FREE(free_endpt);
      }

      curr_ifc = curr_ifc->next;
      FREE(free_ifc);
    }

    device->configurations = device->configurations->next;

    // free the configuration descriptor(s) + interface descriptor(s) +
    // endpoint descriptor(s)
    FREE (curr_configuration->descriptor);

    FREE (curr_configuration);
  }

  //
  // Free the buffer associated with the urbs in the Device.
  //
  for(i = 0; i < MAX_URBS; ++i){
    if(device->Urb[i].transfer_length_dTD){
      FreePool(device->Urb[i].transfer_length_dTD);
      device->Urb[i].transfer_length_dTD = NULL;
    }
  }


ON_EXIT:
  FNC_LEAVE_MSG ();
  return Status;
}


