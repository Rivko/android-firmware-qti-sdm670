/**
* @file UsbDevDesc.c
*
* Functions to copy and create local copy of the client provided descriptors
*
* Copyright (c) 2014-2017 Qualcomm Technologies, Inc. All rights reserved.
*/
/*=============================================================================
EDIT HISTORY

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/01/17   pm      Port from Boot.xf.1.4, Fix the incorrect USB interface size calculation
05/23/16   ck      Add UsbfnChgDxe support
12/01/15   ck      Add USB SS and USBCV test support
01/15/15   ck      Added support of USB enumeration for charger app
12/18/14   amitg   Fix KW Errors
09/17/14   ck      Update for the new USB Device header
08/21/14   ck      Initial revision
=============================================================================*/

#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "UsbDevUtil.h"
#include "UsbDevDesc.h"


/*
* @brief Free the resources within EndpointDescriptorTable as defined in EFI_USB_INTERFACE_INFO
*
* @param[in]  UsbBcd                   USB BCD version number.
*
* @param[in]  EndpointCount            Number of pointers to the Endpoint
*                                       Descriptors in EndpointDescriptorTable
* @param[in]  EndpointDescriptorTable  Array of pointers to Endpoint Descriptors
*
*/
STATIC
VOID
UsbDevDescFreeEndpointDescriptorTable(
  IN UINT16                        UsbBcd,
  IN UINT8                         EndpointCount,
  IN EFI_USB_ENDPOINT_DESCRIPTOR **EndpointDescriptorTable
)
{
  UINT8      Index = 0;

  if (!EndpointDescriptorTable) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    goto ON_EXIT;
  }

  if (USB_BCD_MAJOR_VER(UsbBcd) == 2) {
    for (Index = 0; Index < EndpointCount; Index++) {
      if (EndpointDescriptorTable[Index]) {
        FreePool(EndpointDescriptorTable[Index]);
        EndpointDescriptorTable[Index] = NULL;
      }
    }
  }
  else {
    for (Index = 0; Index < EndpointCount; Index++) {
      if (EndpointDescriptorTable[Index]) {
        EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR* pSSEpDesc = (EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR*) (EndpointDescriptorTable[Index]);
        if (pSSEpDesc->EndpointDescriptor) {
          FreePool(pSSEpDesc->EndpointDescriptor);
          pSSEpDesc->EndpointDescriptor = NULL;
        }
        if (pSSEpDesc->EndpointCompanionDescriptor) {
          FreePool(pSSEpDesc->EndpointCompanionDescriptor);
          pSSEpDesc->EndpointCompanionDescriptor = NULL;
        }
        FreePool(EndpointDescriptorTable[Index]);
        EndpointDescriptorTable[Index] = NULL;
      }
    }
  }

ON_EXIT:
  return;
}

/*
* @brief Free the resources allocated within EFI_USB_INTERFACE_INFO struct
*
* @param[in]  UsbBcd         USB BCD version number.
*
* @param[in]  InterfaceInfo  pointer to EFI_USB_INTERFACE_INFO
*
*/
STATIC
VOID
UsbDevDescFreeInterfaceInfo(
  IN UINT16                  UsbBcd,
  IN EFI_USB_INTERFACE_INFO  *InterfaceInfo
)
{
  UINT8 NumEp = 0;

  if (!InterfaceInfo) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    goto ON_EXIT;
  }

  NumEp = InterfaceInfo->InterfaceDescriptor->NumEndpoints;

  UsbDevDescFreeEndpointDescriptorTable(UsbBcd, NumEp, InterfaceInfo->EndpointDescriptorTable);

  FreePool(InterfaceInfo->InterfaceDescriptor);
  InterfaceInfo->InterfaceDescriptor = NULL;
  FreePool(InterfaceInfo->EndpointDescriptorTable);
  InterfaceInfo->EndpointDescriptorTable = NULL;

ON_EXIT:
  return;
}


/*
* @brief Free the resources allocated within the EFI_USB_CONFIG_INFO structure
*
* @param[in]  UsbBcd             USB BCD version number.
*
* @param[in]  ConfigInfo         Pointer to EFI_USB_CONFIG_INFO struct
*
*/
STATIC
VOID
UsbDevDescFreeConfigInfo(
  IN    UINT16                  UsbBcd,
  IN    EFI_USB_CONFIG_INFO     *ConfigInfo
)
{
  UINT8 Index = 0, NumIfc = 0;

  if (!ConfigInfo) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    goto ON_EXIT;
  }

  NumIfc = ConfigInfo->ConfigDescriptor->NumInterfaces;

  for (Index = 0; Index < NumIfc; Index++) {
    UsbDevDescFreeInterfaceInfo(UsbBcd, ConfigInfo->InterfaceInfoTable[Index]);
  }

  FreePool(ConfigInfo->ConfigDescriptor);
  ConfigInfo->ConfigDescriptor = NULL;
  FreePool(ConfigInfo->InterfaceInfoTable);
  ConfigInfo->InterfaceInfoTable = NULL;

ON_EXIT:
  return;
}


/*
* @brief Free the resources allocated within DeviceInfo structure
*
* @param[in]  DeviceInfo         Pointer to EFI_USB_DEVICE_INFO struct
*
*/
EFI_STATUS
UsbDevDescFreeDevInfo(
  IN    EFI_USB_DEVICE_INFO     *DeviceInfo
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 Index = 0, NumCfg = 0;

  if (!DeviceInfo || !DeviceInfo->DeviceDescriptor || !DeviceInfo->ConfigInfoTable) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  NumCfg = DeviceInfo->DeviceDescriptor->NumConfigurations;

  for (Index = 0; Index < NumCfg; Index++) {
    UsbDevDescFreeConfigInfo(DeviceInfo->DeviceDescriptor->BcdUSB, DeviceInfo->ConfigInfoTable[Index]);
  }

  FreePool(DeviceInfo->DeviceDescriptor);
  DeviceInfo->DeviceDescriptor = NULL;
  FreePool(DeviceInfo->ConfigInfoTable);
  DeviceInfo->ConfigInfoTable = NULL;

ON_EXIT:
  return Status;
}


/*
* @brief Initialize the Endpoint Descriptor Table
*
* @param[in]  UsbBcd                     USB BCD version number.
*
* @param[in]  InterfaceTree              The data array for the interface descriptor to
*                                        endpoint descriptors
*
* @param[in]  CfgDescTotalLength         wTotalLength in the configuration descripotr
*
* @param[out] TotalLength                The TotalLength in bytes of endpoint descriptors
*  (optional) Default to NULL
* @param[out] EndpointDescriptorTable    Endpoint descriptor table to be created
*
* @retval     EFI_SUCCESS                Operation succeeded.
* @retval     Others                     Operation failed
*
*/
STATIC
EFI_STATUS
UsbDevDescInitEndpointDescriptorTable(
  IN  UINT16                         UsbBcd,
  IN  VOID                          *InterfaceTree,
  IN  UINT16                         CfgDescTotalLength,
  OUT UINT16                        *TotalLength,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR ***EndpointDescriptorTable
)
{
  EFI_STATUS                      Status   = EFI_SUCCESS;
  UINT8                           NumEp    = 0, Index = 0;
  VOID                          **EpTable  = NULL;
  EFI_USB_ENDPOINT_DESCRIPTOR    *CEp      = NULL;

  if (!InterfaceTree || !EndpointDescriptorTable || !TotalLength) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  NumEp   = ((EFI_USB_INTERFACE_DESCRIPTOR *) InterfaceTree)->NumEndpoints;

  EpTable = AllocateZeroPool(NumEp * sizeof(EFI_USB_ENDPOINT_DESCRIPTOR *));
  if (!EpTable) {
    DBG(EFI_D_ERROR, "EpTable: Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  // Locate the first Ep.
  CEp = (EFI_USB_ENDPOINT_DESCRIPTOR *) (((UINT8 *)InterfaceTree) + sizeof(EFI_USB_INTERFACE_DESCRIPTOR));
  while (CEp->DescriptorType != USB_DESC_TYPE_ENDPOINT) {
    if (*TotalLength + CEp->Length <= CfgDescTotalLength) {
      *TotalLength += CEp->Length;
      CEp = (EFI_USB_ENDPOINT_DESCRIPTOR *)(((UINT8 *)CEp) + CEp->Length);
    }
    else {
      Status = EFI_INVALID_PARAMETER;
      DBG(EFI_D_ERROR, "Invalid USB descriptor layout");
      goto ON_EXIT;
    }
  }

  for(Index = 0; Index < NumEp; Index++) {
    if (USB_BCD_MAJOR_VER (UsbBcd) == 2) {
      EFI_USB_ENDPOINT_DESCRIPTOR *pEpDesc = AllocateZeroPool(sizeof(EFI_USB_ENDPOINT_DESCRIPTOR));
      EpTable[Index] = pEpDesc;

      if (!pEpDesc) {
        DBG(EFI_D_ERROR, "EpTable[%d]: Out of Resources", Index);
        Status = EFI_OUT_OF_RESOURCES;
        goto ON_EXIT;
      }
      gBS->CopyMem(pEpDesc, CEp, sizeof(EFI_USB_ENDPOINT_DESCRIPTOR));
      DBG(EFI_D_INFO, "=========== EP Desc ===========");
      DBG(EFI_D_INFO, "Len %d", pEpDesc->Length);
      DBG(EFI_D_INFO, "DescType %d", pEpDesc->DescriptorType);
      DBG(EFI_D_INFO, "EndpointAddr 0x%x", pEpDesc->EndpointAddress);
      DBG(EFI_D_INFO, "=========== EP Desc ===========");
      CEp++; //Advance to the next EP
    }
    else {
      EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR *pEpSsDesc = AllocateZeroPool(sizeof(EFI_USB_SUPERSPEED_ENDPOINT_DESCRIPTOR));
      EFI_USB_ENDPOINT_DESCRIPTOR *pEpDesc = AllocateZeroPool(sizeof(EFI_USB_ENDPOINT_DESCRIPTOR));
      EFI_USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR *pEpCompDesc = AllocateZeroPool(sizeof(EFI_USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR));

      if (!pEpSsDesc || !pEpDesc || !pEpCompDesc) {
        DBG(EFI_D_ERROR, "EpTable[%d]: Out of Resources. pEpSsDesc %p, pEpDesc %p, pEpCompDesc %p",
          Index, pEpSsDesc, pEpDesc, pEpCompDesc);
        Status = EFI_OUT_OF_RESOURCES;
        goto ON_EXIT;
      }

	  EpTable[Index] = pEpSsDesc;
      pEpSsDesc->EndpointDescriptor = pEpDesc;
      pEpSsDesc->EndpointCompanionDescriptor = pEpCompDesc;
      gBS->CopyMem(pEpDesc, CEp, sizeof(EFI_USB_ENDPOINT_DESCRIPTOR));
      gBS->CopyMem(pEpCompDesc, CEp + 1, sizeof(EFI_USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR));

      DBG(EFI_D_INFO, "========== EP Desc ===========");
      DBG(EFI_D_INFO, "Len %d", pEpDesc->Length);
      DBG(EFI_D_INFO, "DescType %d", pEpDesc->DescriptorType);
      DBG(EFI_D_INFO, "EndpointAddr 0x%x", pEpDesc->EndpointAddress);

      DBG(EFI_D_INFO, "========== EP Comp Desc ===========");
      DBG(EFI_D_INFO, "Len %d", pEpCompDesc->Length);
      DBG(EFI_D_INFO, "DescType %d", pEpCompDesc->DescriptorType);
      DBG(EFI_D_INFO, "EndpointAddr 0x%x", pEpCompDesc->MaxBurst);

      CEp = (EFI_USB_ENDPOINT_DESCRIPTOR *)(((UINT8 *)CEp) + pEpDesc->Length + pEpCompDesc->Length);
    }
  }

  if (TotalLength) {
    if (USB_BCD_MAJOR_VER (UsbBcd) == 2)
      *TotalLength += NumEp * sizeof(EFI_USB_ENDPOINT_DESCRIPTOR);
    else
      *TotalLength += NumEp * (sizeof(EFI_USB_ENDPOINT_DESCRIPTOR) + sizeof(EFI_USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR));
  }

  *EndpointDescriptorTable = (EFI_USB_ENDPOINT_DESCRIPTOR **) EpTable;

ON_EXIT:

  if (EFI_ERROR(Status)) {
    // free the allocated buffer due to error
    UsbDevDescFreeEndpointDescriptorTable(UsbBcd, NumEp, (EFI_USB_ENDPOINT_DESCRIPTOR **) EpTable);
    FreePool(EpTable);
    EpTable = NULL;
  }

  return Status;
}


/*
* @brief Initialize the EFI_USB_INTERFACE_INFO data structure
*
* @param[in]  UsbBcd                     USB BCD version number.
*
* @param[in]  InterfaceTree              Data array for the interface descriptor
*                                        to endpoint descriptors
*
* @param[in]  CfgDescTotalLength         wTotalLength in the configuration descriptor
*
* @param[out] TotalLength                The TotalLength in bytes of data for interface
*                                        descriptor and endpoint descriptors
*  (optional) Default to NULL
* @param[out] InterfaceInfo              The interfaceInfo to be created
*
* @retval     EFI_SUCCESS                Operation succeeded.
* @retval     Others                     Operation failed
*
*/
STATIC
EFI_STATUS
UsbDevDescInitInterfaceInfo(
  IN  UINT16                          UsbBcd,
  IN  VOID                           *InterfaceTree,
  IN  UINT16                          CfgDescTotalLength,
  OUT UINT16                         *TotalLength,
  OUT EFI_USB_INTERFACE_INFO        **InterfaceInfo
)
{
  EFI_STATUS                     Status  = EFI_SUCCESS;
  EFI_USB_INTERFACE_INFO        *IfcInfo = NULL;

  if (!InterfaceTree || !InterfaceInfo || !TotalLength) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  IfcInfo = AllocateZeroPool(sizeof(EFI_USB_INTERFACE_INFO));
  if (!IfcInfo) {
    DBG(EFI_D_ERROR, "IfcInfo: Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  IfcInfo->InterfaceDescriptor = AllocateZeroPool(sizeof(EFI_USB_INTERFACE_DESCRIPTOR));
  if (!IfcInfo->InterfaceDescriptor) {
    DBG(EFI_D_ERROR, "IfcInfo->InterfaceDescriptor: Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  gBS->CopyMem(IfcInfo->InterfaceDescriptor, InterfaceTree, sizeof(EFI_USB_INTERFACE_DESCRIPTOR));

  DBG(EFI_D_INFO, "=========== Ifc Desc ===========");
  DBG(EFI_D_INFO, "Len %d", IfcInfo->InterfaceDescriptor->Length);
  DBG(EFI_D_INFO, "DescType %d", IfcInfo->InterfaceDescriptor->DescriptorType);
  DBG(EFI_D_INFO, "NumEp %d", IfcInfo->InterfaceDescriptor->NumEndpoints);
  DBG(EFI_D_INFO, "=========== Ifc Desc ===========");

  Status = UsbDevDescInitEndpointDescriptorTable(UsbBcd, InterfaceTree, CfgDescTotalLength, TotalLength, &IfcInfo->EndpointDescriptorTable);

  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  *TotalLength += sizeof(EFI_USB_INTERFACE_DESCRIPTOR);

  *InterfaceInfo = IfcInfo;

ON_EXIT:

  if (EFI_ERROR(Status)) {
    if (IfcInfo) {
      if (IfcInfo->InterfaceDescriptor) {
        FreePool(IfcInfo->InterfaceDescriptor);
        IfcInfo->InterfaceDescriptor = NULL;
      }
      FreePool(IfcInfo);
      IfcInfo = NULL;
    }
  }

  return Status;
}


/*
* @brief Initialize the Interface Table
*
* @param[in]  ConfigurationTree  The configuration tree associated with InterfaceTable
*                                 It contains the configuration descriptors and all
*                                 other descriptors under the configuration tree.
* @param[in]  InterfaceTable     The interface table to be created
*
* @retval     EFI_SUCCESS           Operation succeeded.
* @retval     Others                Operation failed
*
*/
STATIC
EFI_STATUS
UsbDevDescInitInterfaceInfoTable(
  IN   UINT16                          UsbBcd,
  IN   VOID                           *ConfigurationTree,
  OUT  EFI_USB_INTERFACE_INFO       ***InterfaceTable
)
{
  EFI_STATUS                   Status         = EFI_SUCCESS;
  UINT8                        NumIfc         = 0, Index = 0;
  EFI_USB_INTERFACE_INFO     **IfcTable       = NULL;
  EFI_USB_CONFIG_DESCRIPTOR   *ConfigDesc     = NULL;
  UINT16                       TotalLength    = 0;
  VOID                        *IfcTree        = NULL;
  UINT16                       ConfigTotalLen = 0;

  if (!ConfigurationTree || !InterfaceTable) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  ConfigDesc     = ConfigurationTree;
  NumIfc         = ConfigDesc->NumInterfaces;
  IfcTable       = AllocateZeroPool(NumIfc * sizeof(EFI_USB_INTERFACE_INFO*));
  ConfigTotalLen = ConfigDesc->TotalLength;
  if (!IfcTable) {
    DBG(EFI_D_ERROR, "IfcTable: Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  IfcTree = (VOID *)(ConfigDesc + 1);
  for (Index = 0; Index < NumIfc; Index++) {
    Status = UsbDevDescInitInterfaceInfo(UsbBcd, IfcTree, ConfigTotalLen, &TotalLength, &IfcTable[Index]);
    if (EFI_ERROR(Status)){
      goto ON_EXIT;
    }
    IfcTree = ((UINT8 *)IfcTree) + TotalLength;
  }

*InterfaceTable = IfcTable;

ON_EXIT:
  if (EFI_ERROR(Status)) {
    if (IfcTable) {
      FreePool(IfcTable);
      IfcTable = NULL;
    }
  }
  return Status;
}


/*
* @brief Initialize the ConfigurationInfo data structure
*
* @see UsbDeviceStart
*
* @param[in]  ConfigurationTree   The configuration tree associated with InterfaceTable
*                                  It contains the configuration descriptors and all
*                                  other descriptors under the configuration tree.
* @param[out] ConfigurationInfo   The configuration info to be created

*
* @retval     EFI_SUCCESS           Operation succeeded.
* @retval     Others                Operation failed
*
*/
STATIC
EFI_STATUS
UsbDevDescInitConfigInfo(
   IN  UINT16                      UsbBcd,
   IN  VOID                       *ConfigurationTree,
   OUT EFI_USB_CONFIG_INFO       **ConfigurationInfo
)
{
  EFI_STATUS             Status     = EFI_SUCCESS;
  EFI_USB_CONFIG_INFO   *ConfigInfo = NULL;

  if (!ConfigurationTree || !ConfigurationInfo) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  ConfigInfo = AllocateZeroPool(sizeof(EFI_USB_CONFIG_INFO));
  if (!ConfigInfo) {
    DBG(EFI_D_ERROR, "ConfigInfo: Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  ConfigInfo->ConfigDescriptor = AllocateZeroPool(sizeof(EFI_USB_CONFIG_DESCRIPTOR));
  if (!(ConfigInfo->ConfigDescriptor)) {
    DBG(EFI_D_ERROR, "ConfigInfo->ConfigDescriptor: Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  gBS->CopyMem(ConfigInfo->ConfigDescriptor, ConfigurationTree, sizeof(EFI_USB_CONFIG_DESCRIPTOR));

  DBG(EFI_D_INFO, "=========== Cfg Desc ===========");
  DBG(EFI_D_INFO, "Len %d", ConfigInfo->ConfigDescriptor->Length);
  DBG(EFI_D_INFO, "DescType %d", ConfigInfo->ConfigDescriptor->DescriptorType);
  DBG(EFI_D_INFO, "NumIfc %d", ConfigInfo->ConfigDescriptor->NumInterfaces);
  DBG(EFI_D_INFO, "=========== Cfg Desc ===========");

  Status = UsbDevDescInitInterfaceInfoTable(UsbBcd, ConfigurationTree, &ConfigInfo->InterfaceInfoTable);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  *ConfigurationInfo = ConfigInfo;

ON_EXIT:
  if (EFI_ERROR(Status)) {
    if (ConfigInfo) {
      if (ConfigInfo->ConfigDescriptor) {
        FreePool(ConfigInfo->ConfigDescriptor);
        ConfigInfo->ConfigDescriptor = NULL;
      }
      FreePool(ConfigInfo);
      ConfigInfo = NULL;
    }
  }
  return Status;
}


/*
* @brief Initialize the configuration tabled.
*
* @see UsbDeviceStart
*
* @param[in]  DeviceDescriptor    Pointer to the device descriptor associated with
*                                  the Descriptors.
* @param[in]  Descriptors         The descriptor trees as defined in UsbDeviceStart
* @param[out] ConfigurationTable  The configuration table to be created
*
* @retval     EFI_SUCCESS           Operation succeeded.
* @retval     Others                Operation failed
*
*/
STATIC
EFI_STATUS
UsbDevDescInitConfigInfoTable(
  IN     EFI_USB_DEVICE_DESCRIPTOR  *DeviceDescriptor,
  IN     VOID                      **Descriptors,
  OUT    EFI_USB_CONFIG_INFO      ***ConfigurationTable
)
{
  EFI_STATUS             Status      = EFI_SUCCESS;
  UINT8                  NumCfg      = 0, Index = 0;
  EFI_USB_CONFIG_INFO  **ConfigTable = NULL;
  UINT16                 UsbBcd      = 0;


  if (!DeviceDescriptor || !Descriptors || !ConfigurationTable) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  UsbBcd = DeviceDescriptor->BcdUSB;

  NumCfg = DeviceDescriptor->NumConfigurations;

  ConfigTable = AllocateZeroPool(NumCfg * sizeof(EFI_USB_CONFIG_INFO *));

  if (!ConfigTable) {
    DBG(EFI_D_ERROR, "ConfigInfoTable: Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  for (Index = 0; Index < NumCfg; Index++) {
    Status = UsbDevDescInitConfigInfo(UsbBcd, Descriptors[Index], &ConfigTable[Index]);
    if (EFI_ERROR(Status)) {
      goto ON_EXIT;
    }
  }

  *ConfigurationTable = ConfigTable;

ON_EXIT:

  if (EFI_ERROR(Status)) {
    if (ConfigTable) {
      FreePool(ConfigTable);
      ConfigTable = NULL;
    }
  }
  return Status;
}


/*
* @brief Initialize the EFI_USB_DEVICE_INFO structure based on
* USB_DEVICE_DESCRIPTOR and Descriptors array
*
* The function initializes the EFI_USB_DEVICE_INFO structure used in
* EfiUsbfnIoProtocl based on Device Descriptor and the Configuration tree
* defined in EfiUsbDeviceProtocol
*
* @see EFIUsbIo.h
* @see EFIUsbDevice.h
*
* @param[in]  DeviceDescriptor      see EFIUsbIo.h
* @param[in]  Descriptors           see EFIUsbDevice.h
* @param[in]  DeviceInfo            see EFIUsbDevice.h
*
* @retval     EFI_SUCCESS           Operation succeeded.
* @retval     Others                Operation failed
*/
STATIC
EFI_STATUS
UsbDevDescInitDevInfo(
  IN     USB_DEVICE_DESCRIPTOR   *DeviceDescriptor,
  IN     VOID                   **Descriptors,
  IN     EFI_USB_DEVICE_INFO     *DeviceInfo
)
{
  EFI_STATUS                      Status;

  if (!DeviceInfo || !DeviceDescriptor || !Descriptors) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // allocate and copy device descriptor
  DeviceInfo->DeviceDescriptor = AllocateZeroPool(sizeof(EFI_USB_DEVICE_DESCRIPTOR));
  if (!DeviceInfo->DeviceDescriptor) {
    DBG(EFI_D_ERROR, "DeviceDescriptor: Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  gBS->CopyMem(DeviceInfo->DeviceDescriptor, DeviceDescriptor, sizeof(EFI_USB_DEVICE_DESCRIPTOR));

  DBG(EFI_D_INFO, "=========== Dev Desc ===========");
  DBG(EFI_D_INFO, "Len %d", DeviceInfo->DeviceDescriptor->Length);
  DBG(EFI_D_INFO, "DescType %d", DeviceInfo->DeviceDescriptor->DescriptorType);
  DBG(EFI_D_INFO, "BcdUSB 0x%x", DeviceInfo->DeviceDescriptor->BcdUSB);
  DBG(EFI_D_INFO, "=========== Dev Desc ===========");


  Status = UsbDevDescInitConfigInfoTable(DeviceDescriptor, Descriptors, &DeviceInfo->ConfigInfoTable);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

ON_EXIT:
  if (EFI_ERROR(Status)) {
    if ((DeviceInfo) && (DeviceInfo->DeviceDescriptor)) {
      FreePool(DeviceInfo->DeviceDescriptor);
    }
  }
  return Status;
}


/*
* @brief Initialize the EFI_USB_SUPERSPEED_DEVICE_INFO structure.
*
* The function initializes the EFI_USB_SUPERSPEED_DEVICE_INFO structure used in
* EfiUsbfnIoProtocl based on Device Descriptor and the Configuration tree
* defined in EfiUsbDeviceProtocol
*
* @see EFIUsbIo.h
* @see EFIUsbDevice.h
*
* @param[in]  DeviceDescriptor      see EFIUsbIo.h
* @param[in]  Descriptors           see EFIUsbDevice.h
* @param[in]  SSDeviceInfo          see EFIUsbDevice.h
* @param[in]  BinaryObjectStore     see EFIUsbDevice.h
*
* @retval     EFI_SUCCESS           Operation succeeded.
* @retval     Others                Operation failed
*/
STATIC
EFI_STATUS
UsbDevDescInitSSDevInfo(
  IN     USB_DEVICE_DESCRIPTOR              *DeviceDescriptor,
  IN     VOID                              **Descriptors,
  IN     EFI_USB_SUPERSPEED_DEVICE_INFO     *SSDeviceInfo,
  IN     VOID                               *BinaryObjectStore
)
{
  EFI_STATUS                      Status;

  if (!SSDeviceInfo || !DeviceDescriptor || !Descriptors) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // allocate and copy device descriptor
  SSDeviceInfo->DeviceDescriptor = AllocateZeroPool(sizeof(EFI_USB_DEVICE_DESCRIPTOR));
  if (!SSDeviceInfo->DeviceDescriptor) {
    DBG(EFI_D_ERROR, "DeviceDescriptor: Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  gBS->CopyMem(SSDeviceInfo->DeviceDescriptor, DeviceDescriptor, sizeof(EFI_USB_DEVICE_DESCRIPTOR));

  DBG(EFI_D_INFO, "=========== Dev Desc ===========");
  DBG(EFI_D_INFO, "Len %d", SSDeviceInfo->DeviceDescriptor->Length);
  DBG(EFI_D_INFO, "DescType %d", SSDeviceInfo->DeviceDescriptor->DescriptorType);
  DBG(EFI_D_INFO, "BcdUSB 0x%x", SSDeviceInfo->DeviceDescriptor->BcdUSB);
  DBG(EFI_D_INFO, "=========== Dev Desc ===========");


  Status = UsbDevDescInitConfigInfoTable(DeviceDescriptor, Descriptors, (EFI_USB_CONFIG_INFO ***) &SSDeviceInfo->ConfigInfoTable);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  SSDeviceInfo->BosDescriptor = BinaryObjectStore;

ON_EXIT:
  if (EFI_ERROR(Status)) {
    if ((SSDeviceInfo) && (SSDeviceInfo->DeviceDescriptor)) {
      FreePool(SSDeviceInfo->DeviceDescriptor);
    }
  }
  return Status;
}


/*
* @brief Free the descriptor buffers within Descriptors
*
* The function frees the descriptors and the array of pointers to the descriptors
* allocated with Allocate*Pool method.
*
*
* @param[in]  DescriptorCount   Number pointers in the Descriptors
* @param[in]  Descriptors       Array of pointers to the buffers to be freed
*
* @retval     EFI_SUCCESS           Operation succeeded.
* @retval     Others                Operation failed
*/
STATIC
EFI_STATUS
UsbDevDescFreeDescriptorArray(
  IN    UINT8               DescriptorCount,
  IN    VOID              **Descriptors
)
{
  UINT8      Index = 0;
  EFI_STATUS Status = EFI_SUCCESS;

  if (0 == DescriptorCount || NULL == Descriptors) {
    DBG(EFI_D_ERROR, "Invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  for (Index = 0; Index < DescriptorCount; Index++) {
    if (Descriptors[Index]) {
      FreePool(Descriptors[Index]);
      Descriptors[Index] = NULL;
    }
  }

ON_EXIT:
  return Status;
}


/*
* @brief Create a local copy of the client's string descriptors
*
* The function creates a local copy of the string descriptors passed in by the
* client. It is the caller's responsibility to free the local copy by calling
* UsbDevFreeStrDescriptors.
*
* @see UsbDevFreeStrDescriptors
*
* @param[in]  StringDescriptorCount  Number of string descriptor pointers
*                                    in CStrDescriptors.
* @param[in]  CStrDescriptors        Client provided array of string descriptor
*                                    pointers, where CStrDescriptors[n] represents
*                                    the GET_DESCRIPTOR request for string
*                                    descriptor n.
* @param[out] LStrDescriptors        Local copy of CStrDescriptors
*
* @retval     EFI_SUCCESS            Operation succeeded.
* @retval     Others                 Operation failed
*/
STATIC
EFI_STATUS
UsbDevDescInitStrDescriptors(
  IN    UINT8                         StringDescriptorCount,
  IN    EFI_USB_STRING_DESCRIPTOR   **CStrDescriptors,
  OUT   EFI_USB_STRING_DESCRIPTOR  ***LStrDescriptors)
{
  EFI_STATUS                    Status            = EFI_SUCCESS;
  UINT8                         Index             = 0;
  EFI_USB_STRING_DESCRIPTOR   **StringDescriptors = NULL;

  if (0 == StringDescriptorCount || !CStrDescriptors || !LStrDescriptors) {
    DBG(EFI_D_ERROR, "Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  StringDescriptors = AllocateZeroPool(
    StringDescriptorCount * sizeof(EFI_USB_STRING_DESCRIPTOR *));
  if (NULL == StringDescriptors) {
    DBG(EFI_D_ERROR, "Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  for (Index = 0; Index < StringDescriptorCount; Index++) {
    StringDescriptors[Index] = AllocateZeroPool(CStrDescriptors[Index]->Length);
    if (NULL == StringDescriptors[Index]) {
      DBG(EFI_D_ERROR, "Descs[%d]: Out of Resources", Index);
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }
    gBS->CopyMem(StringDescriptors[Index], CStrDescriptors[Index], CStrDescriptors[Index]->Length);
  }

ON_EXIT:

  if (!EFI_ERROR(Status)) {
    *LStrDescriptors = StringDescriptors;
  }
  else {
    if (NULL != StringDescriptors) {
      UsbDevDescFreeDescriptorArray(StringDescriptorCount, (VOID **) StringDescriptors);
      FreePool(StringDescriptors);
    }
  }
  return Status;
}


/*
* @brief Create a local copy of the usb configurations
*
* The function creates a local copy of the USB configurations passed in by the
* client. It is the caller's responsibility to free the local copy by calling
* UsbDevFreeStrDescriptors.
*
* @see UsbDevFreeStrDescriptors
*
* @param[in]  ConfigurationCount     Number of configurations in CConfigurations
* @param[in]  CConfigurations        Client USB configuration(s)
* @param[out] LStrDescriptors        Local copy of USB configuration(s) to be crated
*
* @retval     EFI_SUCCESS            Operation succeeded.
* @retval     Others                 Operation failed
*/
STATIC
EFI_STATUS
UsbDevDescInitConfigurations(
  IN    UINT8        ConfigurationCount,
  IN    VOID       **CConfigurations,
  OUT   VOID      ***LConfigurations)
{
  EFI_STATUS                  Status         = EFI_SUCCESS;
  UINT8                       Index          = 0;
  VOID                      **Configurations = NULL;
  UINT16                      TotalLen       = 0;

  if (0 == ConfigurationCount || !CConfigurations || !LConfigurations) {
    DBG(EFI_D_ERROR, "Invalid Parameters");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  Configurations = AllocateZeroPool(ConfigurationCount * sizeof(VOID *));
  if (NULL == Configurations) {
    DBG(EFI_D_ERROR, "Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }


  for (Index = 0; Index < ConfigurationCount; Index++) {
    TotalLen = ((EFI_USB_CONFIG_DESCRIPTOR *) CConfigurations[Index])->TotalLength;
    Configurations[Index] = AllocateZeroPool(TotalLen);
    if (NULL == Configurations[Index]) {
      DBG(EFI_D_ERROR, "Configurations[%d]: Out of Resources", Index);
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }
    gBS->CopyMem(Configurations[Index], CConfigurations[Index], TotalLen);
  }

ON_EXIT:

  if (!EFI_ERROR(Status)) {
    *LConfigurations = Configurations;
  } else {
    if (NULL != Configurations) {
      UsbDevDescFreeDescriptorArray(ConfigurationCount, Configurations);
      FreePool(Configurations);
    }
  }
  return Status;
}


/*
* @brief Cache a local copy of the client provided Binary Object Store.
*
* The function creates a local copy of the BOS passed in by the
* client. It is the caller's responsibility to free the local copy by calling
* UsbDevUtilFreeBos.
*
* @see UsbDevUtilFreeBos
*
* @param[in]  CBos           pointer to Binary Object Store passed in by the client
* @param[in]  LBos           Local Binary Object Store to be created
*
* @retval     EFI_SUCCESS    Operation succeeded.
* @retval     Others         Operation failedy
*/
STATIC
EFI_STATUS
UsbDevDescInitBos(
  IN    VOID   *CBos,
  OUT   VOID  **LBos)
{
  EFI_STATUS                  Status     = EFI_SUCCESS;
  EFI_USB_BOS_DESCRIPTOR      *pCBosDesc = NULL;

  if (!CBos || !LBos) {
    DBG(EFI_D_ERROR, "Invalid Parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  pCBosDesc = (EFI_USB_BOS_DESCRIPTOR *) CBos;

  *LBos = AllocateZeroPool(pCBosDesc->TotalLength);

  if (*LBos == NULL) {
    DBG(EFI_D_ERROR, "Out of Resources");
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  gBS->CopyMem(*LBos, CBos, pCBosDesc->TotalLength);

ON_EXIT:
  return Status;
}


/*
* @brief Dump the descriptor tree from DeviceInfo.
*
* The function prints certain fields from Device, Configure, Interface, and
* Endpoint descriptors within the DeviceInfo.
*
* @param[in]  DeviceInfo             DeviceInfo containing the descriptors
*
*/
VOID
UsbDevDescPrintDeviceInfo(
  IN EFI_USB_DEVICE_INFO *DeviceInfo
)
{
  USB_DEVICE_DESCRIPTOR        *DevDesc = NULL;
  EFI_USB_CONFIG_DESCRIPTOR    *CfgDesc = NULL;
  EFI_USB_INTERFACE_DESCRIPTOR *IfcDesc = NULL;
  EFI_USB_ENDPOINT_DESCRIPTOR  *EpDesc = NULL;
  EFI_USB_CONFIG_INFO          *CfgInfo = NULL;
  EFI_USB_INTERFACE_INFO       *IfcInfo = NULL;
  UINT8 i, j, k, NumCfg, NumIfc, NumEp;

  if (!DeviceInfo) {
    DBG(EFI_D_ERROR, "Invalid Parameter");
    goto ON_EXIT;
  }

  DevDesc = DeviceInfo->DeviceDescriptor;

  /* Device Descriptor */
  DBG(EFI_D_ERROR, "Device Desc:\n, Length %d\n, Type %d\n, BcdUSB 0x%x\n, NumCfg %d\n",
      DevDesc->Length,
      DevDesc->DescriptorType,
      DevDesc->BcdUSB,
      DevDesc->NumConfigurations);

  NumCfg = DevDesc->NumConfigurations;
  for (i = 0; i < NumCfg; i++) {
    CfgInfo = DeviceInfo->ConfigInfoTable[i];
    CfgDesc = CfgInfo->ConfigDescriptor;

    /* Configuration Descriptor */
    DBG(EFI_D_ERROR, "Cfg Desc:\n, Length %d\n, Type %d\n, Length %d\n, MaxPower %d\n",
        CfgDesc->Length,
        CfgDesc->DescriptorType,
        CfgDesc->TotalLength,
        CfgDesc->MaxPower);

    NumIfc = CfgDesc->NumInterfaces;
    for (j = 0; j < NumIfc; j++) {
      IfcInfo = CfgInfo->InterfaceInfoTable[j];
      IfcDesc = IfcInfo->InterfaceDescriptor;

      /* Interface Descriptor */
      DBG(EFI_D_ERROR, "IFC Desc:\n, Length %d\n, Type %d\n, IfcNum %d\n, IfcStrIdx %d\n",
          IfcDesc->Length,
          IfcDesc->DescriptorType,
          IfcDesc->InterfaceNumber,
          IfcDesc->Interface);

      NumEp = IfcDesc->NumEndpoints;
      for (k = 0; k < NumEp; k++){
        EpDesc = IfcInfo->EndpointDescriptorTable[k];

        // Endpoint Descriptor
        DBG(EFI_D_ERROR, "Ep Desc:\n, Length %d\n, Type %d\n, EpAddr 0x%x\n, MaxPacketSize %d\n, Interval %d\n",
            EpDesc->Length,
            EpDesc->DescriptorType,
            EpDesc->EndpointAddress,
            EpDesc->MaxPacketSize,
            EpDesc->Interval);
      }
    }
  }

ON_EXIT:
  return;
}


/*
* @brief Dump the descriptor tree from Descriptors as defined in USB_DEVICE_START.
*
* The function prints certain fields from Device, Configure, Interface, and
* Endpoint descriptors within the USB configuration.
*
* @see EfiUsbDevice.h
*
* @param[in]  Descriptors             see EFIUsbDevice.h
*
*/
VOID
UsbDevDescPrintUsbConfigurations(
  IN USB_DESCRIPTOR_SET *DescriptorSet
)
{
  USB_DEVICE_DESCRIPTOR        *DevDesc = NULL;
  EFI_USB_CONFIG_DESCRIPTOR    *CfgDesc = NULL;
  EFI_USB_INTERFACE_DESCRIPTOR *IfcDesc = NULL;
  EFI_USB_ENDPOINT_DESCRIPTOR  *EpDesc = NULL;
  UINT8 i, j, k, NumCfg, NumIfc, NumEp;

  if (!DescriptorSet) {
    DBG(EFI_D_ERROR, "Invalid Parameter");
    goto ON_EXIT;
  }

  /* Device Descriptor */
  DevDesc = &DescriptorSet->DeviceDescriptor;
  DBG(EFI_D_ERROR, "HS Device Desc:\n, Length %d\n, Type %d\n, BcdUSB 0x%x\n, NumCfg %d\n",
    DevDesc->Length,
    DevDesc->DescriptorType,
    DevDesc->BcdUSB,
    DevDesc->NumConfigurations);

  NumCfg = DevDesc->NumConfigurations;
  for (i = 0; i < NumCfg; i++) {
    CfgDesc = (EFI_USB_CONFIG_DESCRIPTOR *)DescriptorSet->Descriptors[i];
    NumIfc = CfgDesc->NumInterfaces;

    /* Configuration Descriptor */
    DBG(EFI_D_ERROR, "Cfg Desc:\n, Length %d\n, Type %d\n, Length %d\n, MaxPower %d\n",
      CfgDesc->Length,
      CfgDesc->DescriptorType,
      CfgDesc->TotalLength,
      CfgDesc->MaxPower);

    IfcDesc = (EFI_USB_INTERFACE_DESCRIPTOR *)(CfgDesc + 1);
    for (j = 0; j < NumIfc; j++) {
      NumEp = IfcDesc->NumEndpoints;

      /* Interface Descriptor */
      DBG(EFI_D_ERROR, "IFC Desc:\n, Length %d\n, Type %d\n, IfcNum %d\n, IfcStrIdx %d\n",
        IfcDesc->Length,
        IfcDesc->DescriptorType,
        IfcDesc->InterfaceNumber,
        IfcDesc->Interface);

      EpDesc = (EFI_USB_ENDPOINT_DESCRIPTOR *)(IfcDesc + 1);
      for (k = 0; k < NumEp; k++){
        // Endpoint Descriptor
        DBG(EFI_D_ERROR, "Ep Desc:\n, Length %d\n, Type %d\n, EpAddr 0x%x\n, MaxPacketSize %d\n, Interval %d\n",
          EpDesc->Length,
          EpDesc->DescriptorType,
          EpDesc->EndpointAddress,
          EpDesc->MaxPacketSize,
          EpDesc->Interval);
        EpDesc++;
      }
      IfcDesc = (EFI_USB_INTERFACE_DESCRIPTOR *)EpDesc;
    }
  }

  /* SS Device Descriptor */
  DevDesc = &DescriptorSet->SSDeviceDescriptor;
  DBG(EFI_D_ERROR, "SS Device Desc:\n, Length %d\n, Type %d\n, BcdUSB 0x%x\n, NumCfg %d\n",
    DevDesc->Length,
    DevDesc->DescriptorType,
    DevDesc->BcdUSB,
    DevDesc->NumConfigurations);

  NumCfg = DevDesc->NumConfigurations;
  for (i = 0; i < NumCfg; i++) {
    CfgDesc = (EFI_USB_CONFIG_DESCRIPTOR *)DescriptorSet->SSDescriptors[i];

    if (CfgDesc) {
      NumIfc = CfgDesc->NumInterfaces;

      /* Configuration Descriptor */
      DBG(EFI_D_ERROR, "Cfg Desc:\n, Length %d\n, Type %d\n, Length %d\n, MaxPower %d\n",
        CfgDesc->Length,
        CfgDesc->DescriptorType,
        CfgDesc->TotalLength,
        CfgDesc->MaxPower);

      IfcDesc = (EFI_USB_INTERFACE_DESCRIPTOR *)(CfgDesc + 1);
      for (j = 0; j < NumIfc; j++) {
        NumEp = IfcDesc->NumEndpoints;

        /* Interface Descriptor */
        DBG(EFI_D_ERROR, "IFC Desc:\n, Length %d\n, Type %d\n, IfcNum %d\n, IfcStrIdx %d\n",
          IfcDesc->Length,
          IfcDesc->DescriptorType,
          IfcDesc->InterfaceNumber,
          IfcDesc->Interface);

        EpDesc = (EFI_USB_ENDPOINT_DESCRIPTOR *)(IfcDesc + 1);
        for (k = 0; k < NumEp; k++){
          // Endpoint Descriptor
          EFI_USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR *EpCompDesc = (EFI_USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR *) (EpDesc + 1);
          DBG(EFI_D_ERROR, "Ep Desc:\n, Length %d\n, Type %d\n, EpAddr 0x%x\n, MaxPacketSize %d\n, Interval %d\n",
            EpDesc->Length,
            EpDesc->DescriptorType,
            EpDesc->EndpointAddress,
            EpDesc->MaxPacketSize,
            EpDesc->Interval,
            EpCompDesc->Length,
            EpCompDesc->DescriptorType,
            EpCompDesc->MaxBurst,
            EpCompDesc->Attributes,
            EpCompDesc->BytesPerInterval
            );
          EpDesc = (EFI_USB_ENDPOINT_DESCRIPTOR *) (((EFI_USB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR *)(EpDesc + 1)) + 1);
        }
        IfcDesc = (EFI_USB_INTERFACE_DESCRIPTOR *)EpDesc;
      }
    }
  }

ON_EXIT:
  return;
}

/*  See header for documentation. */
EFI_STATUS
UsbDevDescInitDescriptors(
  IN  USB_DEVICE_DESCRIPTOR                       *DeviceDescriptor,
  IN  VOID                                       **Descriptors,
  IN  EFI_USB_DEVICE_QUALIFIER_DESCRIPTOR         *DeviceQualifierDescriptor,
  IN  VOID                                        *BinaryDeviceOjectStore, OPTIONAL
  IN  UINT8                                        StringDescriptorCount,
  IN  EFI_USB_STRING_DESCRIPTOR                  **StringDescritors, OPTIONAL
  OUT EFI_USB_DEVICE_INFO                         *DeviceInfo,
  OUT USB_DESCRIPTOR_SET                          *DescriptorSet
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!DeviceDescriptor || !Descriptors || !DeviceQualifierDescriptor ||
      (StringDescriptorCount > 0 && !StringDescritors) || !DescriptorSet) {
    DBG(EFI_D_ERROR, "Invalid Parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // convert the data from device descriptor and configuration tree to DeviceInfo
  Status = UsbDevDescInitDevInfo(DeviceDescriptor, Descriptors, DeviceInfo);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  // copy device descriptor
  gBS->CopyMem(&DescriptorSet->DeviceDescriptor, DeviceDescriptor, sizeof(EFI_USB_DEVICE_DESCRIPTOR));

  // copy device qualifier descriptor
  gBS->CopyMem(&DescriptorSet->DeviceQualifierDescriptor, DeviceQualifierDescriptor, sizeof (EFI_USB_DEVICE_QUALIFIER_DESCRIPTOR));

  // copy usb configurations
  if (DeviceDescriptor->NumConfigurations > 0) {
    Status = UsbDevDescInitConfigurations(
      DeviceDescriptor->NumConfigurations,
      Descriptors,
      &DescriptorSet->Descriptors
      );
    if (EFI_ERROR(Status)) {
      goto ON_EXIT;
    }
  }

  // copy string descriptors, if provided
  if (StringDescriptorCount > 0) {
    Status = UsbDevDescInitStrDescriptors(StringDescriptorCount, StringDescritors, &DescriptorSet->StrDescriptors);
    DescriptorSet->StrDescCount = StringDescriptorCount;
    if (EFI_ERROR(Status)) {
      goto ON_EXIT;
    }
  }

  // copy BOS, if provided
  if (BinaryDeviceOjectStore) {
    Status = UsbDevDescInitBos(BinaryDeviceOjectStore, &DescriptorSet->BinaryDeviceOjectStore);
    if (EFI_ERROR(Status)) {
      goto ON_EXIT;
    }
  }

ON_EXIT:
  return Status;
}

/*  See header for documentation. */
EFI_STATUS
UsbDevDescInitDescriptorsEx(
  IN  USB_DEVICE_DESCRIPTOR_SET          *UsbDevDescSet,
  OUT EFI_USB_DEVICE_INFO                *DeviceInfo,
  OUT EFI_USB_SUPERSPEED_DEVICE_INFO     *SSDeviceInfo,
  OUT USB_DESCRIPTOR_SET                 *DescriptorSet
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!UsbDevDescSet) {
    DBG(EFI_D_ERROR, "Invalid Parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  // convert the data from device descriptor and configuration tree to DeviceInfo
  Status = UsbDevDescInitDevInfo(UsbDevDescSet->DeviceDescriptor, UsbDevDescSet->Descriptors, DeviceInfo);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  // convert the data from ss device descriptor and configuration tree to SSDeviceInfo
  Status = UsbDevDescInitSSDevInfo(UsbDevDescSet->SSDeviceDescriptor, UsbDevDescSet->SSDescriptors, SSDeviceInfo, UsbDevDescSet->BinaryDeviceOjectStore);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  // copy device descriptor
  gBS->CopyMem(&DescriptorSet->DeviceDescriptor,   UsbDevDescSet->DeviceDescriptor, sizeof(EFI_USB_DEVICE_DESCRIPTOR));
  gBS->CopyMem(&DescriptorSet->SSDeviceDescriptor, UsbDevDescSet->SSDeviceDescriptor, sizeof(EFI_USB_DEVICE_DESCRIPTOR));

  // copy device qualifier descriptor
  gBS->CopyMem(&DescriptorSet->DeviceQualifierDescriptor, UsbDevDescSet->DeviceQualifierDescriptor, sizeof (EFI_USB_DEVICE_QUALIFIER_DESCRIPTOR));

  // copy usb configurations
  if (UsbDevDescSet->DeviceDescriptor->NumConfigurations > 0) {
    Status = UsbDevDescInitConfigurations(
      UsbDevDescSet->DeviceDescriptor->NumConfigurations,
      UsbDevDescSet->Descriptors,
      &DescriptorSet->Descriptors
      );
    if (EFI_ERROR(Status)) {
      goto ON_EXIT;
    }
  }

  // copy ss usb configurations
  if (UsbDevDescSet->SSDeviceDescriptor->NumConfigurations > 0) {
    Status = UsbDevDescInitConfigurations(
      UsbDevDescSet->SSDeviceDescriptor->NumConfigurations,
      UsbDevDescSet->SSDescriptors,
      &DescriptorSet->SSDescriptors
      );
    if (EFI_ERROR(Status)) {
      goto ON_EXIT;
    }
  }

  // copy string descriptors, if provided
  if (UsbDevDescSet->StringDescriptorCount > 0) {
    Status = UsbDevDescInitStrDescriptors(UsbDevDescSet->StringDescriptorCount, UsbDevDescSet->StringDescritors, &DescriptorSet->StrDescriptors);
    DescriptorSet->StrDescCount = UsbDevDescSet->StringDescriptorCount;
    if (EFI_ERROR(Status)) {
      goto ON_EXIT;
    }
  }

  // copy BOS, if provided
  if (UsbDevDescSet->BinaryDeviceOjectStore) {
    Status = UsbDevDescInitBos(UsbDevDescSet->BinaryDeviceOjectStore, &DescriptorSet->BinaryDeviceOjectStore);
    if (EFI_ERROR(Status)) {
      goto ON_EXIT;
    }

    if (USB_BCD_MAJOR_VER(UsbDevDescSet->DeviceDescriptor->BcdUSB) == 3) {
      ((EFI_USB_SUPERSPEED_DEVICE_INFO *)DeviceInfo)->BosDescriptor = DescriptorSet->BinaryDeviceOjectStore;
    }
  }

ON_EXIT:
  return Status;
}


/*  See header for documentation. */
EFI_STATUS
UsbDevDescFreeDescriptors(
  IN USB_DESCRIPTOR_SET *DescriptorSet
)
{
  EFI_STATUS Status   = EFI_SUCCESS;

  if (!DescriptorSet) {
    DBG(EFI_D_ERROR, "Invalid Parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  if (DescriptorSet->Descriptors) {
    UsbDevDescFreeDescriptorArray(
      DescriptorSet->DeviceDescriptor.NumConfigurations,
      (VOID **) DescriptorSet->Descriptors);
    FreePool(DescriptorSet->Descriptors);
    DescriptorSet->Descriptors = NULL;
  }

  if (DescriptorSet->SSDescriptors) {
    UsbDevDescFreeDescriptorArray(
      DescriptorSet->SSDeviceDescriptor.NumConfigurations,
      (VOID **)DescriptorSet->SSDescriptors);
    FreePool(DescriptorSet->SSDescriptors);
    DescriptorSet->SSDescriptors = NULL;
  }

  if (DescriptorSet->BinaryDeviceOjectStore) {
    FreePool(DescriptorSet->BinaryDeviceOjectStore);
    DescriptorSet->BinaryDeviceOjectStore = NULL;
  }

  if (DescriptorSet->StrDescriptors) {
    UsbDevDescFreeDescriptorArray(
      DescriptorSet->StrDescCount,
      (VOID **) DescriptorSet->StrDescriptors);
    FreePool(DescriptorSet->StrDescriptors);
    DescriptorSet->StrDescriptors = NULL;
  }

ON_EXIT:
  return Status;
}
