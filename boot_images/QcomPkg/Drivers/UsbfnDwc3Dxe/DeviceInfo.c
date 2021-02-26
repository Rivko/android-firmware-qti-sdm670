/** @file DeviceInfo.c

  Helper functions to convert and store Usb device information

  Copyright (c) 2010-2014 Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/20/14   ml      Migrate to official SuperSpeed support
 04/30/14   ar      Cleanup for NDEBUG build  
 08/20/13   mliss   added USB 3.0 descriptor functions
 06/24/11   cching  port the UsbfnIo protocol

=============================================================================*/


#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "DeviceInfo.h"
#include "UsbfnDwc3Util.h"


#define DESC_CHECK(descPtr, descType)                 \
  if (NULL == (descPtr)) {                            \
    Status = EFI_INVALID_PARAMETER;                   \
    DBG(EFI_D_ERROR, "bad %a descriptor", descType);  \
    goto ON_EXIT;                                     \
  }


/*
 * USB 3.0 descriptors used to support SS enumeration even without explicit
 * client driver support.  Relevant fields from the supplied 2.0 descriptor
 * set will be copied over these in that case.
 */
#pragma pack(push)
#pragma pack(1)

STATIC
struct {
  EFI_USB_BOS_DESCRIPTOR                BosDescriptor;
  EFI_USB_USB_20_EXTENSION_DESCRIPTOR   Usb2ExtDescriptor;
  EFI_USB_SUPERSPEED_USB_DESCRIPTOR     SsUsbDescriptor;
} BinaryObjectStore = {
  // BOS Descriiptor
  {
    sizeof(EFI_USB_BOS_DESCRIPTOR), // Descriptor Size
    USB_DESC_TYPE_BOS,              // Descriptor Type
    sizeof(BinaryObjectStore),      // Length of this descriptor and all device capabilities
    2                               // Number of device capabilities
  },
  // USB2 Extension Desc
  {
    sizeof(EFI_USB_USB_20_EXTENSION_DESCRIPTOR),  // Descriptor Size
    USB_DESC_TYPE_DEVICE_CAPABILITY,              // Device Capability Type descriptor
    USB_DEV_CAP_TYPE_USB_20_EXTENSION,            // USB 2.0 Extension Capability Type
    0x2                                           // Supported device level features
  },
  // Super Speed Device Capability Desc
  {
    sizeof(EFI_USB_SUPERSPEED_USB_DESCRIPTOR),    // Descriptor Size
    USB_DESC_TYPE_DEVICE_CAPABILITY,              // Device Capability Type descriptor
    USB_DEV_CAP_TYPE_SUPERSPEED_USB,              // SuperSpeed Device Capability Type
    0x00,                                         // Supported device level features
    0x0E,                                         // Speeds Supported by the device: SS, HS and FS
    0x01,                                         // Functionality support
    0x07,                                         // U1 Device Exit Latency
    0x65                                          // U2 Device Exit Latency
  }
};

#pragma pack(pop)


/*
 * See header for description.
 */
EFI_STATUS
UpdateDeviceInfo(
  IN  USBFN_DEV                       *Usbfn,
  IN  EFI_USB_DEVICE_INFO             *DevInfo2,
  OUT EFI_USB_SUPERSPEED_DEVICE_INFO  **DevInfo3
  )
{
  EFI_STATUS                                Status = EFI_SUCCESS;
  UINTN                                     TotalConfigLen = 0;
  UINTN                                     TotalDescLen = 0;
  UINTN                                     TotalInfoLen = 0;
  UINTN                                     NumIfcs = 0, NumEps = 0;
  UINTN                                     i, j;
  EFI_USB_DEVICE_DESCRIPTOR                 *DeviceDesc = NULL;
  EFI_USB_CONFIG_DESCRIPTOR                 *TotalConfigDesc;
  EFI_USB_INTERFACE_DESCRIPTOR              *IfcDesc;
  EFI_USB_ENDPOINT_DESCRIPTOR               *EpDesc;
  EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR  *SsEpCompDesc;
  EFI_USB_SUPERSPEED_DEVICE_INFO            *NewDevInfo = NULL;
  EFI_USB_SUPERSPEED_CONFIG_INFO            *ConfigInfo;
  EFI_USB_SUPERSPEED_INTERFACE_INFO         *IfcInfo;
  EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR    **SSEpDescTable;
  EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR    *SSEpDesc;

  FNC_ENTER_MSG();

  // check parameters
  if (NULL == DevInfo2 || NULL == DevInfo3) {
    DBG(EFI_D_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }
  DESC_CHECK(DevInfo2->DeviceDescriptor, "device");
  
  // fail if device has more than 1 configuration
  if (DevInfo2->DeviceDescriptor->NumConfigurations != 1) {
    DBG(EFI_D_ERROR, "ONLY 1 CONFIGURATION SUPPORTED!");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  /*
   * The entire new descriptor set is managed with two allocations to simplify
   * freeing of the memory. One allocation contains the *_INFO structures, back
   * to back, in the following order: EFI_USB_SUPERSPEED_DEVICE_INFO,
   * EFI_USB_SUPERSPEED_CONFIG_INFO, EFI_USB_SUPERSPEED_INTERFACE_INFO(s),
   * EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR(s). The EFI_USB_BOS_DESCRIPTOR
   * and its sub-structures are statically allocated above as they do not need to
   * change. The second allocation contains the actual descriptor structures,
   * starting with the device descriptor and followed by the total configuration
   * descriptor. All the structures are linked together in the *_INFO stuctures
   * such that only a pointer to the USB3_DEVICE_INFO is needed to free them.
   */

  // start with the length from the original config descriptor
  DESC_CHECK(DevInfo2->ConfigInfoTable, "config info table");
  DESC_CHECK(DevInfo2->ConfigInfoTable[0], "config info");
  DESC_CHECK(DevInfo2->ConfigInfoTable[0]->ConfigDescriptor, "config");
  TotalConfigLen = DevInfo2->ConfigInfoTable[0]->ConfigDescriptor->TotalLength;

  // save number of interfaces in configuration
  NumIfcs = DevInfo2->ConfigInfoTable[0]->ConfigDescriptor->NumInterfaces;
  DESC_CHECK(DevInfo2->ConfigInfoTable[0]->InterfaceInfoTable, "interface info table");

  // sum up total number of endpoints in configuration, validate pointers in proceess
  for (i = 0; i < NumIfcs; i++) {
    UINTN IfcNumEps;

    DESC_CHECK(DevInfo2->ConfigInfoTable[0]->InterfaceInfoTable[i], "interface info");
    DESC_CHECK(DevInfo2->ConfigInfoTable[0]->InterfaceInfoTable[i]->InterfaceDescriptor, "interface");
    DESC_CHECK(DevInfo2->ConfigInfoTable[0]->InterfaceInfoTable[i]->EndpointDescriptorTable, "endpoint table");

    IfcNumEps = DevInfo2->ConfigInfoTable[0]->InterfaceInfoTable[i]->InterfaceDescriptor->NumEndpoints;
    NumEps += IfcNumEps;

    // validate endpoint descriptor pointers
    for (j = 0; j < IfcNumEps; j++) {
      DESC_CHECK(DevInfo2->ConfigInfoTable[0]->InterfaceInfoTable[i]->EndpointDescriptorTable[j], "endpoint");
    }
  }

  // add length of ep companion descriptors to the new total config length
  TotalConfigLen += NumEps * sizeof(EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR);

  // calculate total lengths for the two allocations described above
  TotalDescLen = TotalConfigLen + sizeof(EFI_USB_DEVICE_DESCRIPTOR);
  TotalInfoLen = sizeof(EFI_USB_SUPERSPEED_DEVICE_INFO) +
                 sizeof(EFI_USB_SUPERSPEED_CONFIG_INFO*) +
                 sizeof(EFI_USB_SUPERSPEED_CONFIG_INFO) +
                 sizeof(EFI_USB_SUPERSPEED_INTERFACE_INFO*) * NumIfcs +
                 sizeof(EFI_USB_SUPERSPEED_INTERFACE_INFO) * NumIfcs +
                 sizeof(EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR) * NumEps;

  // allocate memory for new descriptor and info structures
  DeviceDesc = UncachedAllocateAlignedPool(TotalDescLen, USBFN_XFER_BUFFER_ALIGNMENT);
  if (NULL == DeviceDesc) {
    DBG(EFI_D_ERROR, "memory allocation failed");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  NewDevInfo = AllocatePool(TotalInfoLen);
  if (NULL == NewDevInfo) {
    DBG(EFI_D_ERROR, "memory allocation failed");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  // fill in device info
  NewDevInfo->DeviceDescriptor = DeviceDesc;
  NewDevInfo->ConfigInfoTable  = (EFI_USB_SUPERSPEED_CONFIG_INFO**)(NewDevInfo + 1);
  NewDevInfo->BosDescriptor    = (EFI_USB_BOS_DESCRIPTOR *)&BinaryObjectStore;

  // copy device descriptor and update
  CopyMem(DeviceDesc, DevInfo2->DeviceDescriptor, sizeof(EFI_USB_DEVICE_DESCRIPTOR));
  DeviceDesc->BcdUSB         = 0x0300;
  DeviceDesc->MaxPacketSize0 = 9;

  // fill in config info table
  ConfigInfo = (EFI_USB_SUPERSPEED_CONFIG_INFO*)(NewDevInfo->ConfigInfoTable + 1);
  NewDevInfo->ConfigInfoTable[0] = ConfigInfo;

  // fill in config info
  TotalConfigDesc = (EFI_USB_CONFIG_DESCRIPTOR*)(DeviceDesc + 1);
  ConfigInfo->ConfigDescriptor   = TotalConfigDesc;
  ConfigInfo->InterfaceInfoTable = (EFI_USB_SUPERSPEED_INTERFACE_INFO**)(ConfigInfo + 1);

  // copy config descriptor and update
  CopyMem(TotalConfigDesc, DevInfo2->ConfigInfoTable[0]->ConfigDescriptor,
      sizeof(EFI_USB_CONFIG_DESCRIPTOR));
  TotalConfigDesc->TotalLength = TotalConfigLen;

  /*
   * Pre interface loop initialization
   */

  // place first interface info structure after the table of pointers to them
  IfcInfo = (EFI_USB_SUPERSPEED_INTERFACE_INFO*)((UINTN)ConfigInfo->InterfaceInfoTable +
      sizeof(EFI_USB_SUPERSPEED_INTERFACE_INFO*) * NumIfcs);

  // place first interface descriptor after the config descriptor
  IfcDesc = (EFI_USB_INTERFACE_DESCRIPTOR*)(TotalConfigDesc + 1);

  // place the first interface's endpoint descriptor after last interface info
  SSEpDescTable = (EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR**)(IfcInfo + NumIfcs);

  /*
   * loop through the interfaces in the configuration
   */
  for (i = 0; i < NumIfcs; i++) {
    UINTN IfcNumEps = DevInfo2->ConfigInfoTable[0]->InterfaceInfoTable[i]->InterfaceDescriptor->NumEndpoints;

    // link interface info and interface descriptors
    ConfigInfo->InterfaceInfoTable[i] = IfcInfo;
    IfcInfo->InterfaceDescriptor = IfcDesc;
    IfcInfo->EndpointDescriptorTable = SSEpDescTable;

    // copy interface descriptor
    CopyMem(IfcDesc, DevInfo2->ConfigInfoTable[0]->InterfaceInfoTable[i]->InterfaceDescriptor,
        sizeof(EFI_USB_INTERFACE_DESCRIPTOR));

    // place this interface's first endpoint descriptor after its endpoint descriptor table
    SSEpDesc = (EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR*)((UINTN)SSEpDescTable +
        sizeof(EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR*) * IfcNumEps);

    // place first endpoint's descriptors after the interface descriptor
    EpDesc       = (EFI_USB_ENDPOINT_DESCRIPTOR*)(IfcDesc + 1);
    SsEpCompDesc = (EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR*)(EpDesc + 1);

    // loop through the endpoints in the interface
    for (j = 0; j < IfcNumEps; j++) {

      // store SS endpoint descriptor in the SS endpoint descriptor table
      SSEpDescTable[j] = SSEpDesc;

      // link endpoint descriptors to endpoint info
      SSEpDesc->EndpointDescriptor          = EpDesc;
      SSEpDesc->EndpointCompanionDescriptor = SsEpCompDesc;

      // copy the original endpoint descriptor
      CopyMem(EpDesc, DevInfo2->ConfigInfoTable[0]->InterfaceInfoTable[i]->EndpointDescriptorTable[j],
          sizeof(EFI_USB_ENDPOINT_DESCRIPTOR));

      // update the max packet size on bulk endpoints
      if (EpDesc->Attributes == USB_ENDPOINT_BULK) {
        EpDesc->MaxPacketSize = 1024;
      }

      // fill in the superspeed endpoint companion descriptor
      SsEpCompDesc->Length           = sizeof(EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR);
      SsEpCompDesc->DescriptorType   = USB_DESC_TYPE_SS_ENDPOINT_COMPANION;
      SsEpCompDesc->MaxBurst         = 4;
      SsEpCompDesc->Attributes       = 0;
      SsEpCompDesc->BytesPerInterval = 0;

      // increment endpoint info
      SSEpDesc++;

      // place next endpoint's descriptors after the last
      EpDesc       = (EFI_USB_ENDPOINT_DESCRIPTOR*)(SsEpCompDesc + 1);
      SsEpCompDesc = (EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR*)(EpDesc + 1);
    }

    /*
     * Prepair for next interface loop iteration
     */

    // increment interface info and interface descriptor pointers
    IfcInfo++;
    IfcDesc += 1; // first endpoint descriptor in interface
    IfcDesc += (sizeof(EFI_USB_ENDPOINT_DESCRIPTOR) +
                sizeof(EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR)) * IfcNumEps;

    // place the next interface's endpoint info table
    SSEpDescTable += sizeof(EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR*) * IfcNumEps; // first SS endpoint descriptor in interface
    SSEpDescTable += sizeof(EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR) * IfcNumEps;
  }

  *DevInfo3 = NewDevInfo;

ON_EXIT:
  // clean up on error
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "ERROR: %r, freeing descriptor memory", Status);
    if (NULL != DeviceDesc) {
      FreePool(DeviceDesc);
    }
    if (NULL != NewDevInfo) {
      FreePool(NewDevInfo);
    }
    if (NULL != DevInfo3) {
      *DevInfo3 = NULL;
    }
  }
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for description.
 */
VOID
FreeDeviceInfo(
  IN  EFI_USB_SUPERSPEED_DEVICE_INFO  *DevInfo3
  )
{
  if (NULL != DevInfo3) {
    if (NULL != DevInfo3->DeviceDescriptor) {
      UncachedFreeAlignedPool(DevInfo3->DeviceDescriptor);
    }
    FreePool(DevInfo3);
  }
}


/*
 * See header for description.
 */
EFI_STATUS
ValidateDeviceInfo(
  IN  EFI_USB_DEVICE_INFO             *DevInfo,
  IN  EFI_USB_SUPERSPEED_DEVICE_INFO  *SSDevInfo OPTIONAL
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN       NumIfcs, IfcNumEps, i, j;

  FNC_ENTER_MSG();

  if (NULL == DevInfo) {
    Status = EFI_INVALID_PARAMETER;
    DBG(EFI_D_ERROR, "invalid paramter");
    goto ON_EXIT;
  }

  // validate USB 2.0 Device Info
  DESC_CHECK(DevInfo->DeviceDescriptor, "device");
  
  if (DevInfo->DeviceDescriptor->NumConfigurations != 1) {
    DBG(EFI_D_ERROR, "ONLY 1 CONFIGURATION SUPPORTED!");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  DESC_CHECK(DevInfo->ConfigInfoTable, "config info table");
  DESC_CHECK(DevInfo->ConfigInfoTable[0], "config info");
  DESC_CHECK(DevInfo->ConfigInfoTable[0]->ConfigDescriptor, "config");
  DESC_CHECK(DevInfo->ConfigInfoTable[0]->InterfaceInfoTable, "interface info table");

  NumIfcs = DevInfo->ConfigInfoTable[0]->ConfigDescriptor->NumInterfaces;
  for (i = 0; i < NumIfcs; i++) {
    DESC_CHECK(DevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i], "interface info");
    DESC_CHECK(DevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i]->InterfaceDescriptor, "interface");
    DESC_CHECK(DevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i]->EndpointDescriptorTable, "endpoint table");

    IfcNumEps = DevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i]->InterfaceDescriptor->NumEndpoints;
    for (j = 0; j < IfcNumEps; j++) {
      DESC_CHECK(DevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i]->EndpointDescriptorTable[j], "endpoint");
    }
  }

  // validate USB 3.0 Device Info
  if (SSDevInfo) {
    DESC_CHECK(SSDevInfo->DeviceDescriptor, "SS device");
    DESC_CHECK(SSDevInfo->BosDescriptor, "SS BOS");
    
    if (SSDevInfo->DeviceDescriptor->NumConfigurations != 1) {
      DBG(EFI_D_ERROR, "ONLY 1 CONFIGURATION SUPPORTED!");
      Status = EFI_INVALID_PARAMETER;
      goto ON_EXIT;
    }

    DESC_CHECK(SSDevInfo->ConfigInfoTable, "SS config info table");
    DESC_CHECK(SSDevInfo->ConfigInfoTable[0], "SS config info");
    DESC_CHECK(SSDevInfo->ConfigInfoTable[0]->ConfigDescriptor, "SS config");
    DESC_CHECK(SSDevInfo->ConfigInfoTable[0]->InterfaceInfoTable, "SS interface info table");

    NumIfcs = SSDevInfo->ConfigInfoTable[0]->ConfigDescriptor->NumInterfaces;
    for (i = 0; i < NumIfcs; i++) {
      DESC_CHECK(SSDevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i], "SS interface info");
      DESC_CHECK(SSDevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i]->InterfaceDescriptor, "SS interface");
      DESC_CHECK(SSDevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i]->EndpointDescriptorTable, "SS endpoint table");

      IfcNumEps = SSDevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i]->InterfaceDescriptor->NumEndpoints;
      for (j = 0; j < IfcNumEps; j++) {
        DESC_CHECK(SSDevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i]->EndpointDescriptorTable[j],
            "SS endpoint desc");
        DESC_CHECK(SSDevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i]->EndpointDescriptorTable[j]->EndpointDescriptor,
            "SS endpoint");
        DESC_CHECK(SSDevInfo->ConfigInfoTable[0]->InterfaceInfoTable[i]->EndpointDescriptorTable[j]->EndpointCompanionDescriptor,
            "SS endpoint companion");
      }
    }

    // verify congruence between 2.0 & 3.0 Device Infos
    if (DevInfo->DeviceDescriptor->DeviceClass !=
        SSDevInfo->DeviceDescriptor->DeviceClass) {
      DBG(EFI_D_ERROR, "DeviceClass mismatch");
      Status = EFI_UNSUPPORTED;
      goto ON_EXIT;
    }

    if (DevInfo->DeviceDescriptor->DeviceSubClass !=
        SSDevInfo->DeviceDescriptor->DeviceSubClass) {
      DBG(EFI_D_ERROR, "DeviceSubClass mismatch");
      Status = EFI_UNSUPPORTED;
      goto ON_EXIT;
    }

    if (DevInfo->DeviceDescriptor->IdVendor !=
        SSDevInfo->DeviceDescriptor->IdVendor) {
      DBG(EFI_D_ERROR, "Vendor ID mismatch");
      Status = EFI_UNSUPPORTED;
      goto ON_EXIT;
    }

    if (DevInfo->DeviceDescriptor->IdProduct !=
        SSDevInfo->DeviceDescriptor->IdProduct) {
      DBG(EFI_D_ERROR, "Product ID mismatch");
      Status = EFI_UNSUPPORTED;
      goto ON_EXIT;
    }
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


