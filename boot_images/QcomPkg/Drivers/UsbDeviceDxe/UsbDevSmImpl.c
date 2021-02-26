/**
 * @file UsbDevSmImpl.c
 *
 * @brief State function implementation
 *
 * This file implements the state entry and transition functions for use in the
 * transfer state machine.
 *
 * @copyright Copyright (c) 2014-2017 by Qualcomm Technologies, Inc. All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/06/17   vsb     Handle Debug type GET_DESCRIPTOR
 11/01/16   ck      Handle SET_INTERFACE with alternate value equal to zero
 09/22/16   ck      Fix ch9 test failure
 07/26/16   ck      Fix the incorrect USB interface size calculation
 05/23/16   ck      Add UsbfnChgDxe support
 01/25/16   ck      Define USB_STATUS as UINT16
 11/06/15   ck      Add USB SS and USBCV test support
 01/15/15   ck      Added support of USB enumeration for charger app
 09/17/14   ck      Update for the new USB Device header
 08/18/14   ck      Add comments and remove unused code
 08/15/14   ml      Clean up
 04/30/14   ar      Cleanup for NDEBUG build
 02/06/14   ml      Initial implementation
 =============================================================================*/


#include "UsbError.h"
#include "UsbDevSm.h"
#include "UsbDevSmImpl.h"
#include "UsbDeviceDxe.h"
#include "UsbDevUtil.h"

// wLength for GET_STATUS request
#define GET_STATUS_REQUEST_DATA_LEN 2
#define USB_FEATURE_U1_ENABLE_SHFT  2
#define USB_FEATURE_U1_ENABLE_BMSK  ((0x1)<<(USB_FEATURE_U1_ENABLE_SHFT))
#define USB_FEATURE_U2_ENABLE_SHFT  3
#define USB_FEATURE_U2_ENABLE_BMSK  ((0x1)<<(USB_FEATURE_U2_ENABLE_SHFT))


/*******************************************************************************
 * Helper functions
 ******************************************************************************/

/**
 * Update the MaxPacketSize field in the Endpoint descriptors with the
 * same type in the DescriptorSet
 *
 * @param[in]  DescriptorSet Descriptor set which contains the Endpoints to be updated
 * @param[in]  TransferType  Endpoint Descriptor Transfer Type
 * @param[in]  MaxPktSize    The new MaxPacket size for the Endpoints
 *
 * @retval EFI_SUCCESS      MaxPacketSize for some endpoints are updated
 * @retval Others           No endpoint is updated
 */
STATIC
EFI_STATUS
UpdateEpMaxPktSize(
  IN USB_DESCRIPTOR_SET  *DescriptorSet,
  IN UINT8                TransferType,
  IN UINT16               MaxPktSize
)
{
  EFI_STATUS                    Status  = EFI_NOT_FOUND;
  USB_DEVICE_DESCRIPTOR        *DevDesc = NULL;
  EFI_USB_CONFIG_DESCRIPTOR    *CfgDesc = NULL;
  EFI_USB_INTERFACE_DESCRIPTOR *IfcDesc = NULL;
  EFI_USB_ENDPOINT_DESCRIPTOR  *EpDesc  = NULL;
  UINT8 i, j, k, NumCfg, NumIfc, NumEp;

  if (!DescriptorSet) {
    DBG(EFI_D_ERROR, "Invalid Parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  DevDesc = &DescriptorSet->DeviceDescriptor;

  NumCfg = DevDesc->NumConfigurations;
  for (i = 0; i < NumCfg; i++) {
    CfgDesc = (EFI_USB_CONFIG_DESCRIPTOR *)DescriptorSet->Descriptors[i];
    NumIfc = CfgDesc->NumInterfaces;
    IfcDesc = (EFI_USB_INTERFACE_DESCRIPTOR *)(CfgDesc + 1);
    while (IfcDesc->DescriptorType != USB_DESC_TYPE_INTERFACE) {
      DBG(EFI_D_WARN, "Nonstandard Layout: ConfigDesc followed by Desc with Type 0x%x", IfcDesc->DescriptorType);
      IfcDesc = (EFI_USB_INTERFACE_DESCRIPTOR *)(((UINT8*)IfcDesc) + IfcDesc->Length);
    }
    for (j = 0; j < NumIfc; j++) {
      NumEp = IfcDesc->NumEndpoints;
      EpDesc = (EFI_USB_ENDPOINT_DESCRIPTOR *)(IfcDesc + 1);
      while (EpDesc->DescriptorType != USB_DESC_TYPE_ENDPOINT) {
        DBG(EFI_D_WARN, "Nonstandard layout: IfcDesc followed by Desc with Type 0x%x", EpDesc->DescriptorType);
        EpDesc = (EFI_USB_ENDPOINT_DESCRIPTOR *)(((UINT8 *)EpDesc) + EpDesc->Length);
      }

      for (k = 0; k < NumEp; k++){
        if (((EpDesc->Attributes) & USB_ENDPOINT_TYPE_MASK) == TransferType) {
          DBG(EFI_D_INFO, "EndpointAddr 0x%x: PktSize %d -> %d",
              EpDesc->EndpointAddress,
              EpDesc->MaxPacketSize,
              MaxPktSize
              );
          EpDesc->MaxPacketSize = MaxPktSize;
          Status = EFI_SUCCESS;
        }
        EpDesc++;
      }
      IfcDesc = (EFI_USB_INTERFACE_DESCRIPTOR *)EpDesc;
    }
  }

ON_EXIT:
  return Status;
}


/**
 * Return string description of bus speed.
 *
 * @param[in]  BusSpeed     Desired USB bus speed
 *
 * @return Ascii string representation of BusSpeed
 */
STATIC
CHAR8 CONST *
PrintBusSpeed (
  IN  EFI_USB_BUS_SPEED     BusSpeed
  )
{
  switch (BusSpeed) {
    case UsbBusSpeedLow:
      return "Low";
    case UsbBusSpeedFull:
      return "Full";
    case UsbBusSpeedHigh:
      return "High";
    case UsbBusSpeedSuper:
      return "Super";
    default:
      return "Unknown";
  }
}


/**
 * Return string description of USB port type.
 *
 * @param[in]  PortType     Desired USB port type
 *
 * @return Ascii string representation of PortType
 */
STATIC
CHAR8 CONST *
PrintPortType (
  IN  EFI_USBFN_PORT_TYPE   PortType
  )
{
  switch (PortType) {
    case EfiUsbStandardDownstreamPort:
      return "SDP";
    case EfiUsbChargingDownstreamPort:
      return "CDP";
    case EfiUsbDedicatedChargingPort:
      return "DCP";
    case EfiUsbInvalidDedicatedChargingPort:
      return "IDCP";
    default:
      return "Unknown Port";
  }
}

/**
 * Validate the transfer result endpoint and buffer pointer.
 *
 * @param[in]  DevSm        Usbfn state machine instance
 * @param[in]  Utr          UsbfnIo transfer result
 * @param[in]  Ep           Expected endpoint
 *
 * @retval EFI_SUCCESS      Transfer result valid
 * @retval EFI_UNSUPPORTED  Transfer result on unexpected direction
 * @retval EFI_DEVICE_ERROR Invalid buffer returned in transfer result
 */
STATIC
EFI_STATUS
ValidateEpBuffer(
  IN  DEV_STATE_MACHINE         *DevSm,
  IN  EFI_USBFN_TRANSFER_RESULT *Utr,
  IN  UINT8                     Ep
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  // Verify expected endpoint in event
  if (Ep != Utr->EndpointIndex) {
    DBG(EFI_D_VERBOSE, "Unexpected endpoint 0x%02x not handled",
        ENDPOINT_ADDR(Utr->EndpointIndex, Utr->EndpointDirection));
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  switch (Ep) {
    // Verify the control buffer was returned in the event (just print warning, don't halt test)
    case USBDEV_CONTROL_EP:
      if (Utr->Buffer != DevSm->CtrlXfer.Buffer) {
        DBG(EFI_D_WARN, "UsbfnIo Protocol Violation: invalid buffer returned in transfer result\n"
            "    expected: %p\n"
            "    actual:   %p",
            DevSm->CtrlXfer.Buffer,
            Utr->Buffer);
      }
      break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * Validate the feature selection request against the requirements
 * defined in the spec.
 *
 * @param[in]  DevSm        Usbfn state machine instance
 * @param[in]  Req          USB setup packet
 * @param[in]  IsSet        TRUE if the request is for set feature
 *                          FALSE if the request if for clear feature
 *
 * @retval EFI_SUCCESS      Validation is successful
 * @retval EFI_UNSUPPORTED  Transfer result on unexpected directions
 */
STATIC
EFI_STATUS
VerifyFeatureSelection (
  IN DEV_STATE_MACHINE      *DevSm,
  IN EFI_USB_DEVICE_REQUEST *Req,
  IN BOOLEAN                IsSet
)
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  UINT16 FeatureSelector;

  FNC_ENTER_MSG();

  FeatureSelector = Req->Value;

  if (USB_SETUP_RT_RCP(Req->RequestType)  ==  USB_SETUP_RT_RCP_DEV){

    //0. For SET_FEATURE targeting Device, device must be configured
    if (IsSet == TRUE && DevSm->CurrentState != DEV_STATE_CONFIGURED){
      DBG(EFI_D_ERROR, "Device is not in Configured State");
      goto ON_EXIT;
    }

    // For CLEAR_FEATURE targeting Device, device must be in address state
    if (IsSet == FALSE && DevSm->BusAddress == 0){
      DBG(EFI_D_ERROR, "Device is not in Address State");
      goto ON_EXIT;
    }

    //1 . wIndex must be zero
    if (Req->Index != 0){
      DBG(
        EFI_D_ERROR,
        "Recipient is Device, but wIndoex is not zero (%d)",
        Req->Index);
      goto ON_EXIT;
    }

    //2. For device, only the following feature selectors are valid
    if (UsbBusSpeedSuper == DevSm->BusSpeed){
      switch (FeatureSelector){
      case USB_FEATURE_U1_ENABLE:
      case USB_FEATURE_U2_ENABLE:
      case USB_FEATURE_LTM_ENABLE:
        // low-power link state
        break;
      default:
        DBG(
          EFI_D_ERROR,
          "Recipient: Device, USB V3, featureSelector %d is not valid",
          FeatureSelector);
        goto ON_EXIT;
      }
    }
    else {
      switch (FeatureSelector){
      case USB_FEATURE_HS_TEST_MODE:
        break;
      case USB_FEATURE_DEV_REMOTE_WAKEUP:
        break;
      default:
        DBG(
          EFI_D_ERROR,
          "Recipient: Device, USB V2, featureSelector %d is not valid",
          FeatureSelector);
        goto ON_EXIT;
      }
    }
  }
  else if (USB_SETUP_RT_RCP(Req->RequestType)  ==  USB_SETUP_RT_RCP_IFC){

    //0. Must be in configured state
    if (DevSm->BusAddress == 0 || DevSm->CurrentState != DEV_STATE_CONFIGURED){
      DBG(EFI_D_ERROR, "Device is not in Configured State");
      goto ON_EXIT;
    }

    if (UsbBusSpeedSuper == DevSm->BusSpeed){
      switch (FeatureSelector){
      case USB_FEATURE_FUNCTION_SUSPEND:
        // (1) The lower byte of wInidex shall be set to the
        // FIRST interface of a function.  Currently support
        // only one interface per function.

        // (2) Bit 2-7 in higher byte of wIndex should be zero
        if (0 != (USB_GET_HIGH_BYTE(Req->Index) >> 2)){
          DBG(
            EFI_D_ERROR,
            "Bit 2-7 in higher byte of wIndex should be zero (0x%x)",
            USB_GET_HIGH_BYTE(Req->Index));
          goto ON_EXIT;
        }
        break;
      default:
        DBG(
          EFI_D_ERROR,
          "Recipient: Interface, USB V3, featureSelector %d is not valid",
          FeatureSelector);
        goto ON_EXIT;
      }
    }
    else {
      switch (FeatureSelector){
      default:
        DBG(
          EFI_D_ERROR,
          "Recipient: Interface, USB V2, featureSelector %d is not valid",
          FeatureSelector);
        goto ON_EXIT;
      }
    }
  }
  else if (USB_SETUP_RT_RCP(Req->RequestType)  ==  USB_SETUP_RT_RCP_EP){

    //0. Must be in configured state except when targeting control endpoint
    if ((DevSm->BusAddress == 0 || DevSm->CurrentState != DEV_STATE_CONFIGURED) &&
      (USB_GET_LOW_BYTE(Req->Index) != 0x00)){
      DBG(EFI_D_ERROR, "Device is not in Configured State");
      goto ON_EXIT;
    }

    switch (FeatureSelector){
    case USB_FEATURE_ENDPOINT_HALT:
      break;
    default:
      DBG(
        EFI_D_ERROR,
        "Recipient: Endpoint, featureSelector %d is not valid",
        FeatureSelector);
      goto ON_EXIT;
    }
  }
  else {
    DBG(EFI_D_ERROR, "Recipient: Invalid");
    goto ON_EXIT;
  }

  Status = EFI_SUCCESS;
ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}

/**
 * Handle SET_FEATURE/CLEAR_FEATURE requests
 *
 * @param[in]  DevSm       Usbfn state machine instance
 * @param[in]  Req         USB setup packet
 * @param[in]  IsSet       Flag for set/clear operation
 *
 * @return EFI_SUCCESS     Operation succeeds.
 * @return others          Operation fails.
 */
static
EFI_STATUS
HandleSetFeature(
  IN DEV_STATE_MACHINE      *DevSm,
  IN EFI_USB_DEVICE_REQUEST *Req,
  IN BOOLEAN                IsSet )
{
  UINT16 FeatureSelector;
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  Status = VerifyFeatureSelection(DevSm, Req, IsSet);
  if (EFI_ERROR(Status)){
    goto ON_EXIT;
  }

  FeatureSelector = Req->Value;
  Status = EFI_UNSUPPORTED;

  if (USB_SETUP_RT_RCP(Req->RequestType)  ==  USB_SETUP_RT_RCP_DEV){
    if (UsbBusSpeedSuper == DevSm->BusSpeed){
      switch (FeatureSelector){
      case USB_FEATURE_U1_ENABLE:
        if(IsSet)
          DevSm->UsbDevStatus |= USB_FEATURE_U1_ENABLE_BMSK;
        else
          DevSm->UsbDevStatus &= ~USB_FEATURE_U1_ENABLE_BMSK;
        break;
      case USB_FEATURE_U2_ENABLE:
        if (IsSet)
          DevSm->UsbDevStatus |= USB_FEATURE_U2_ENABLE_BMSK;
        else
          DevSm->UsbDevStatus &= ~USB_FEATURE_U2_ENABLE_BMSK;
        break;
      case USB_FEATURE_LTM_ENABLE:
        DBG(EFI_D_WARN, "SS Device:SetFeature [IsSet: %d](%d) not handled", IsSet, FeatureSelector);
        goto ON_EXIT;
      }
    }
    else {
      switch (FeatureSelector){
      case USB_FEATURE_HS_TEST_MODE:
      case USB_FEATURE_DEV_REMOTE_WAKEUP:
        DBG(EFI_D_WARN, "HS Device:SetFeature[IsSet: %d](%d) not handled", IsSet, FeatureSelector);
        goto ON_EXIT;
      }
    }
  }
  else if (USB_SETUP_RT_RCP(Req->RequestType)  ==  USB_SETUP_RT_RCP_IFC){
    if (UsbBusSpeedSuper == DevSm->BusSpeed){
      switch (FeatureSelector){
      case USB_FEATURE_FUNCTION_SUSPEND:
        DBG(EFI_D_INFO, "SS Ifc:SetFeature (USB_FEATURE_FUNCTION_SUSPEND)[IsSet: %d]", IsSet);
      }
    }
  }
  else if (USB_SETUP_RT_RCP(Req->RequestType)  ==  USB_SETUP_RT_RCP_EP){
    switch (FeatureSelector){
      case USB_FEATURE_ENDPOINT_HALT:
      {
        DBG(EFI_D_INFO, "%a Feature Endpoint Halt on ep 0x%x",
            (Req->Request == USB_DEV_SET_FEATURE) ? "Set" : "Clear",
            Req->Index);
        Status = DevSm->UsbfnIo->SetEndpointStallState(
           DevSm->UsbfnIo,
           USB_INDEX_TO_EP(Req->Index),
           USB_INDEX_TO_EPDIR(Req->Index),
           IsSet);
        if (EFI_ERROR(Status)) {
          DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
          goto ON_EXIT;
        }
      }
    }
  }
  else {
    DBG(EFI_D_ERROR, "Recipient: Invalid");
    goto ON_EXIT;
  }

  Status = EFI_SUCCESS;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief Validate GET_STATUS request
 *
 * Validate the fields in SETUP PACKET for a GET_STATUS request
 *
 * @param[in]  DevSm       Usbfn state machine instance
 * @param[in]  Req         USB setup packet
 *
 * @return EFI_SUCCESS     GET_STATUS request has correct fields
 * @return others          Some field in GET_STATUS request is not valid
 */
EFI_STATUS
VerifyGetStatus(
    IN  DEV_STATE_MACHINE      *DevSm,
    IN  EFI_USB_DEVICE_REQUEST *Req )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  FNC_ENTER_MSG();

  if (0 != Req->Value){
    DBG(EFI_D_ERROR, "GET_STATUS: wValue is %d, but should be zero", Req->Value);
    goto ON_EXIT;
  }

  if (GET_STATUS_REQUEST_DATA_LEN != Req->Length){
    DBG(EFI_D_ERROR, "GET_STATUS: wLength is %d, but should be two", Req->Length);
    goto ON_EXIT;
  }

  if (USB_SETUP_RT_RCP(Req->RequestType) == USB_SETUP_RT_RCP_DEV){
    if (0 != Req->Index) {
      DBG(EFI_D_ERROR,
      "GET_STATUS: wIndex is %d, but should be zero for a device status request",
      Req->Index);
      goto ON_EXIT;
    }
  } else if (USB_SETUP_RT_RCP(Req->RequestType) == USB_SETUP_RT_RCP_IFC){
    if (USB_GET_LOW_BYTE(Req->Index) >= ((EFI_USB_CONFIG_DESCRIPTOR *)DevSm->DescriptorSet.Descriptors[0])->NumInterfaces) {
      DBG(EFI_D_ERROR, "Interface number is out of range");
      goto ON_EXIT;
    }
    // Must be in configured state except when targeting control endpoint
    if ((DevSm->CurrentState != DEV_STATE_CONFIGURED) && (USB_GET_LOW_BYTE(Req->Index) != 0x00)){
      DBG(EFI_D_ERROR, "Device is not in Configured State");
      goto ON_EXIT;
    }
  }
  Status = EFI_SUCCESS;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief Handle GET_STATUS request
 *
 * Routine to handle GET_STATUS request from host.
 *
 * @param[in]  DevSm          Usbfn state machine instance
 * @param[in]  Req            USB setup packet
 * @param[out] Buffer         Pointer to a buffer pointer
 * @param[out] BufSiz         Size of the buffer pointed by *Buffer
 *
 * @return EFI_SUCESS      Operation succeeds.
 * @return others             Operation fails.
 */
EFI_STATUS
HandleGetStatus(
  IN  DEV_STATE_MACHINE      *DevSm,
  IN  EFI_USB_DEVICE_REQUEST *Req,
  OUT VOID                   **Buffer,
  OUT UINTN                  *BufSiz)
{
  EFI_STATUS Status;
  static const UINT16 IfcStatus = 0;
  static UINT16 EpStatus = 0;
  BOOLEAN State;

  FNC_ENTER_MSG();

  Status = VerifyGetStatus(DevSm, Req);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "VerifyGetStatus failed: %r", Status);
    goto ON_EXIT;
  }

  Status = EFI_UNSUPPORTED;

  if (USB_SETUP_RT_RCP(Req->RequestType) == USB_SETUP_RT_RCP_DEV){
    *Buffer = (VOID *)&DevSm->UsbDevStatus;
    *BufSiz = sizeof (DevSm->UsbDevStatus);
  }
  else if (USB_SETUP_RT_RCP(Req->RequestType) == USB_SETUP_RT_RCP_IFC) {
    *Buffer = (VOID *)&IfcStatus;
    *BufSiz = sizeof (IfcStatus);
  }
  else if (USB_SETUP_RT_RCP(Req->RequestType) == USB_SETUP_RT_RCP_EP) {
    Status = DevSm->UsbfnIo->GetEndpointStallState(
      DevSm->UsbfnIo,
      USB_INDEX_TO_EP(Req->Index),
      USB_INDEX_TO_EPDIR(Req->Index),
      &State);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "GetEndpointStallState failed: %r", Status);
      goto ON_EXIT;
    }
    EpStatus = State;
    *Buffer = (VOID *) &EpStatus;
    *BufSiz = sizeof (EpStatus);
  }

  Status = EFI_SUCCESS;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * Handles setup packets containing standard requests.
 *
 * @param[in]  DevSm        Usbfn state machine instance
 * @param[in]  Req          Setup packet data
 * @param[out] NextState    UsbfnIo message payload
 *
 * @retval EFI_SUCCESS      Next transfer started
 * @retval EFI_UNSUPPORTED  Request not handled
 * @retval Others           Error processing request
 */
STATIC
EFI_STATUS
ProcessStdRequest(
  IN  DEV_STATE_MACHINE       *DevSm,
  IN  EFI_USB_DEVICE_REQUEST  *Req,
  OUT DEV_STATE               *NextState
  )
{
  EFI_STATUS  Status     = EFI_UNSUPPORTED;
  VOID       *TxSrcBuf   = NULL;
  UINTN       XferSize   = 0;
  UINT8       ConfigVal  = 0;
  UINT8       AltSetting = 0;

  FNC_ENTER_MSG();

  // Should move up KAI
  *NextState = DEV_STATE_SAME;

  // Only handle standard requests
  if (USB_SETUP_RT_TYPE(Req->RequestType) != USB_SETUP_RT_TYPE_STD) {
    DBG(EFI_D_VERBOSE, "Not handling non-standard request 0x%02x", USB_SETUP_RT_TYPE(Req->RequestType));
    goto ON_EXIT;
  }

  // Switch on setup request
  switch (Req->Request) {

    case USB_REQ_CLEAR_FEATURE:
      Status = HandleSetFeature(DevSm, Req, FALSE);
      if (EFI_ERROR(Status)) {
        goto ON_EXIT;
      }
      break;

    case USB_REQ_SET_FEATURE:
      Status = HandleSetFeature(DevSm, Req, TRUE);
      if (EFI_ERROR(Status)) {
        goto ON_EXIT;
      }
      break;

    case USB_REQ_GET_INTERFACE:
      TxSrcBuf = &AltSetting;
      XferSize = sizeof(AltSetting);
      break;
    case USB_REQ_SET_INTERFACE:
      // do not support alternative interfaces which
      // are not default(0)
      if (Req->Value == 0) {
        break;
      }
      else {
        goto ON_EXIT;
      }

    case USB_REQ_GET_STATUS:
      Status = HandleGetStatus(DevSm, Req, &TxSrcBuf, &XferSize);
      if (EFI_ERROR(Status)) {
        goto ON_EXIT;
      }
      DBG(EFI_D_VERBOSE, "Get Status: %x. XferSize %d", *((UINT16 *) TxSrcBuf), XferSize);
      break;

    case USB_REQ_SET_ADDRESS:

      DevSm->BusAddress = Req->Value & 0xff;
      *NextState = DEV_STATE_DEFAULT_ADDRESSED;
      DBG(EFI_D_VERBOSE, "Set Address: %d", DevSm->BusAddress);
      break;

    case USB_REQ_SET_ISOCH_DELAY:
      // nothing to do, just return status
      break;

    case USB_REQ_SET_SEL:
      // nothing to do, receive handled below and data ignored
      break;

    case USB_REQ_GET_DESCRIPTOR: {

      UINT8 DescType   =  USB_VALUE_TO_DESC_TYPE(Req->Value);
      UINT8 DescIndex  =  USB_VALUE_TO_DESC_IDX(Req->Value);
      DBG(EFI_D_VERBOSE, "USB_REQ_GET_DESCRIPTOR DescType: 0x%x", DescType);

      switch (DescType) {

        case USB_DESC_TYPE_DEVICE:
          if (DevSm->BusSpeed == UsbBusSpeedSuper) {
            TxSrcBuf = &DevSm->DescriptorSet.SSDeviceDescriptor;
            // need to change it to 0x0310 to pass ch9 test
            if (((EFI_USB_DEVICE_DESCRIPTOR *) TxSrcBuf)->BcdUSB == 0x0300) {
              ((EFI_USB_DEVICE_DESCRIPTOR *) TxSrcBuf)->BcdUSB = 0x0310;
            }
            XferSize = DevSm->DescriptorSet.SSDeviceDescriptor.Length;
          }
          else {
            TxSrcBuf = &DevSm->DescriptorSet.DeviceDescriptor;
            XferSize = DevSm->DescriptorSet.DeviceDescriptor.Length;
          }
        break;

        case USB_DESC_TYPE_DEVICE_QUALIFIER:
          TxSrcBuf = &DevSm->DescriptorSet.DeviceQualifierDescriptor;
          XferSize = DevSm->DescriptorSet.DeviceQualifierDescriptor.Length;
        break;

        case USB_DESC_TYPE_CONFIG:
          if (DevSm->BusSpeed == UsbBusSpeedSuper) {
            TxSrcBuf = DevSm->DescriptorSet.SSDescriptors[0];
            XferSize = ((EFI_USB_CONFIG_DESCRIPTOR *)DevSm->DescriptorSet.SSDescriptors[0])->TotalLength;
          }
          else {
            TxSrcBuf = DevSm->DescriptorSet.Descriptors[0];
            XferSize = ((EFI_USB_CONFIG_DESCRIPTOR *)DevSm->DescriptorSet.Descriptors[0])->TotalLength;
          }
        break;

        case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
        {
          EFI_USB_BUS_SPEED AltBusSpeed;
          UINT16 MaxPacketSize;

          if (DevSm->BusSpeed == UsbBusSpeedSuper) {
            DBG(EFI_D_ERROR, "USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION is not for devices in SS");
            goto ON_EXIT;
          }

          if (DevSm->BusSpeed == UsbBusSpeedHigh) {
            AltBusSpeed = UsbBusSpeedFull;
          } else {
            AltBusSpeed = UsbBusSpeedHigh;
          }
          // Get the other speed's max packet size
          Status = DevSm->UsbfnIo->GetEndpointMaxPacketSize(DevSm->UsbfnIo, UsbEndpointBulk, AltBusSpeed, &MaxPacketSize);
          if (EFI_ERROR(Status) || (DevSm->BusSpeed == UsbBusSpeedUnknown)) {
            DBG(EFI_D_ERROR, "GetEndpointMaxPacketSize failed: %r", Status);
            goto ON_EXIT;
          }

          // update the max packet size based on the alt speed
          UpdateEpMaxPktSize(&DevSm->DescriptorSet, USB_ENDPOINT_BULK, MaxPacketSize);
          // update the descriptor type
          ((EFI_USB_CONFIG_DESCRIPTOR *)DevSm->DescriptorSet.Descriptors[0])->DescriptorType = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;

          // send the updated information to the host
          TxSrcBuf = DevSm->DescriptorSet.Descriptors[0];
          XferSize = ((EFI_USB_CONFIG_DESCRIPTOR *)DevSm->DescriptorSet.Descriptors[0])->TotalLength;

          // since there is only one copy of the descriptors, the value needs to be reverted later.
        }
        break;

        case USB_DESC_TYPE_STRING:
          if (DescIndex < DevSm->DescriptorSet.StrDescCount) {
            TxSrcBuf = DevSm->DescriptorSet.StrDescriptors[DescIndex];
            XferSize = DevSm->DescriptorSet.StrDescriptors[DescIndex]->Length;
          }
          else
          {
            DBG(EFI_D_WARN, "String Index (0x%x) Out Of Bound", DescIndex);
            goto ON_EXIT;
          }
        break;

        case USB_DESC_TYPE_BOS:
          TxSrcBuf = DevSm->DescriptorSet.BinaryDeviceOjectStore;
          XferSize = ((EFI_USB_BOS_DESCRIPTOR *) (DevSm->DescriptorSet.BinaryDeviceOjectStore))->TotalLength;
        break;

        default: // unsupported descriptor type
          DBG(EFI_D_WARN, "Unsupported descriptor type 0x%02x", DescType);
          goto ON_EXIT;
      }
    }
    break;

    case USB_REQ_GET_CONFIG:

      if (DevSmGetCurrentState() == DEV_STATE_CONFIGURED) {
        ConfigVal = 1;
      } else {
        ConfigVal = 0;
      }
      TxSrcBuf = &ConfigVal;
      XferSize = sizeof(ConfigVal);
      DBG(EFI_D_VERBOSE, "Get Configuration: %d", ConfigVal);
    break;

    case USB_REQ_SET_CONFIG:
    {
      UINT8 NewConfig;
      UINT8 NumConfig;

      if (DevSm->BusSpeed == UsbBusSpeedUnknown) {
        DBG(EFI_D_ERROR, "USB_REQ_SET_CONFIG: Unknown USB Speed");
        goto ON_EXIT;
      }

      NumConfig = (DevSm->BusSpeed == UsbBusSpeedSuper) ?
        DevSm->DescriptorSet.SSDeviceDescriptor.NumConfigurations :
        DevSm->DescriptorSet.DeviceDescriptor.NumConfigurations;
      NewConfig = USB_GET_LOW_BYTE(Req->Value);

      if (NewConfig > NumConfig) {
         DBG(EFI_D_ERROR, "USB_REQ_SET_CONFIG: New config (%d) out of the range", NewConfig);
         goto ON_EXIT;
      }

      *NextState = (NewConfig == 0) ? DEV_STATE_DEFAULT_ADDRESSED : DEV_STATE_CONFIGURED;
      DBG(EFI_D_VERBOSE, "Set Configuration: %d", Req->Value);
    }
    break;

    default:
      DBG(EFI_D_ERROR, "Request not handled, 0x%x",Req->Request);
      goto ON_EXIT;
  }

  USB_ASSERT_GOTO((DevSm->CtrlXfer.XferState == XFER_FREE), ON_EXIT);

  // Just queue a zero length IN transfer for status stage
  if (0 == Req->Length) {
    DevSm->CtrlXfer.Direction    = EfiUsbEndpointDirectionDeviceTx;
    DevSm->CtrlXfer.BufferOffset = 0;
    DevSm->CtrlXfer.XferSize     = 0;
    DBG(EFI_D_VERBOSE, "No data stage");
  } else if (Req->RequestType & USB_ENDPOINT_DIR_IN) { // Device needs to send data to host

    DevSm->CtrlXfer.Direction    = EfiUsbEndpointDirectionDeviceTx;
    DevSm->CtrlXfer.BufferOffset = 0;
    DevSm->CtrlXfer.XferSize     = (Req->Length < XferSize) ? Req->Length : XferSize;
    CopyMem(DevSm->CtrlXfer.Buffer, TxSrcBuf, DevSm->CtrlXfer.XferSize);
    DBG(EFI_D_VERBOSE, "Sending data to host, length = %d", DevSm->CtrlXfer.XferSize);

    if ((USB_REQ_GET_DESCRIPTOR == Req->Request) &&
        (USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION == USB_VALUE_TO_DESC_TYPE(Req->Value))){
      UINT16 MaxPacketSize;
      // revert the max packet size based on the current operating speed, and descriptor type
      DevSm->UsbfnIo->GetEndpointMaxPacketSize(DevSm->UsbfnIo, UsbEndpointBulk, DevSm->BusSpeed, &MaxPacketSize);
      UpdateEpMaxPktSize(&DevSm->DescriptorSet, USB_ENDPOINT_BULK, MaxPacketSize);
      ((EFI_USB_CONFIG_DESCRIPTOR *)DevSm->DescriptorSet.Descriptors[0])->DescriptorType = USB_DESC_TYPE_CONFIG;
    }
  } else { // Device needs to receive data from host

    // Stall if we don't have enough room for data
    if (Req->Length > DevSm->CtrlXfer.BufferSize) {
      Status = DevSm->UsbfnIo->SetEndpointStallState (
          DevSm->UsbfnIo,
          USBDEV_CONTROL_EP,
          EfiUsbEndpointDirectionDeviceRx,
          TRUE);
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
        goto ON_EXIT;
      }
      DBG(EFI_D_ERROR, "Not enough room for CTRL RX transfer");
      goto ON_EXIT;
    } else { // receive the data
      DevSm->CtrlXfer.Direction    = EfiUsbEndpointDirectionDeviceRx;
      DevSm->CtrlXfer.BufferOffset = 0;
      DevSm->CtrlXfer.XferSize     = Req->Length;
      DBG(EFI_D_VERBOSE, "Receiving data from host, length = %d", DevSm->CtrlXfer.XferSize);
    }
  }

  // Submit the data or status stage
  Status = IssueXfer(DevSm->UsbfnIo, &DevSm->CtrlXfer);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}

/**
 * Handles setup packets containing class requests.
 *
 * @param[in]  DevSm        Usbfn state machine instance
 * @param[in]  Req          Setup packet data
 * @param[out] NextState    UsbfnIo message payload
 *
 * @retval EFI_SUCCESS      Next transfer started
 * @retval EFI_UNSUPPORTED  Request not handled
 * @retval Others           Error processing request
 */
STATIC
EFI_STATUS
ProcessClsRequest(
  IN  DEV_STATE_MACHINE       *DevSm,
  IN  EFI_USB_DEVICE_REQUEST  *Req,
  OUT DEV_STATE               *NextState
  )
{
  EFI_STATUS  Status    = EFI_SUCCESS;
  FNC_ENTER_MSG();

  *NextState = DEV_STATE_SAME;

  // non-standard request. Pass to client
  DevSm->ClsVndReq = TRUE;

  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief Handles Rx events for standard control requests.
 *
 * This function simply needs to process the status for the endpoint and
 * update the control transfer. If any failure occurs an error status is
 * returned to the caller which is responsible for stalling the appropriate
 * endpoint.
 *
 * @param[in]  DevSm        Usbfn state machine instance
 * @param[in]  Utr          UsbfnIo transfer result
 * @param[out] NextState    UsbfnIo message payload
 *
 * @retval EFI_SUCCESS      Event handled successfully
 * @retval EFI_UNSUPPORTED  Not a supported endpoint
 * @retval Others           Error processing event
 */
STATIC
EFI_STATUS
ProcessCtrlEpStatusChangedRx(
  IN  DEV_STATE_MACHINE         *DevSm,
  IN  EFI_USBFN_TRANSFER_RESULT *Utr,
  OUT DEV_STATE                 *NextState
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  *NextState = DEV_STATE_SAME;

  DBG(EFI_D_VERBOSE, "EfiUsbMsgEndpointStatusChangedRx: EP: 0x%02x: Status %d: BytesTransferred: %d",
      ENDPOINT_ADDR(Utr->EndpointIndex, Utr->EndpointDirection),
      Utr->TransferStatus,
      Utr->BytesTransferred);

  Status = ValidateEpBuffer(DevSm, Utr, USBDEV_CONTROL_EP);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  DevSm->CtrlXfer.XferState = XFER_FREE;

  // Switch on the transfer status
  switch (Utr->TransferStatus) {

    case UsbTransferStatusComplete:
      if (0 == Utr->BytesTransferred) {

        // Control transfer Status Stage is done
        DBG(EFI_D_VERBOSE, "Status OUT stage complete");

      } else {
        DBG(EFI_D_INFO, "Just recieved %d bytes", DevSm->CtrlXfer.XferSize);

        // Queue a zero length Tx Transfer to complete the Status Stage
        DBG(EFI_D_VERBOSE, "Data OUT stage complete, submitting status IN stage");
        DevSm->CtrlXfer.Direction    = EfiUsbEndpointDirectionDeviceTx;
        DevSm->CtrlXfer.BufferOffset = 0;
        DevSm->CtrlXfer.XferSize     = 0;

        Status = IssueXfer(DevSm->UsbfnIo, &DevSm->CtrlXfer);

      }
      break;

    case UsbTransferStatusAborted:
      DBG(EFI_D_VERBOSE, "Control OUT transfer aborted");
      break;

    default: // Other statuses should not occur
      DBG(EFI_D_VERBOSE, "Unhandled transfer status on CTRL OUT: %d", Utr->TransferStatus);
      Status = EFI_DEVICE_ERROR;
      break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * @brief Handles Tx events for standard control requests.
 *
 * This function simply needs to process the status for the endpoint and
 * update the control transfer. If any failure occurs an error status is
 * returned to the caller which is responsible for stalling the appropriate
 * endpoint.
 *
 * @param[in]  DevSm        Usbfn state machine instance
 * @param[in]  Utr          UsbfnIo transfer result
 * @param[out] NextState    UsbfnIo message payload
 *
 * @retval EFI_SUCCESS      Event handled successfully
 * @retval EFI_UNSUPPORTED  Not a supported endpoint
 * @retval Others           Error processing event
 */
STATIC
EFI_STATUS
ProcessCtrlEpStatusChangedTx(
  IN  DEV_STATE_MACHINE         *DevSm,
  IN  EFI_USBFN_TRANSFER_RESULT *Utr,
  OUT DEV_STATE                 *NextState
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  FNC_ENTER_MSG();

  *NextState = DEV_STATE_SAME;

  DBG(EFI_D_VERBOSE, "EfiUsbMsgEndpointStatusChangedTx: EP: 0x%02x: BytesTransferred: %d",
      ENDPOINT_ADDR(Utr->EndpointIndex, Utr->EndpointDirection),
      Utr->BytesTransferred);

  Status = ValidateEpBuffer(DevSm, Utr, USBDEV_CONTROL_EP);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  DevSm->CtrlXfer.XferState = XFER_FREE;

  // Switch on the transfer status
  switch (Utr->TransferStatus) {

    case UsbTransferStatusComplete:
      if (0 == Utr->BytesTransferred) {

        // Control transfer Status Stage is done
        DBG(EFI_D_VERBOSE, "Status IN stage complete");

      } else {

        // Control transfer Data Stage is done
        // Queue a zero length Rx Transfer to complete the Status Stage
        DBG(EFI_D_VERBOSE, "Data IN stage complete, submitting status OUT stage");
        DevSm->CtrlXfer.Direction    = EfiUsbEndpointDirectionDeviceRx;
        DevSm->CtrlXfer.BufferOffset = 0;
        DevSm->CtrlXfer.XferSize     = 0;

        Status = IssueXfer(DevSm->UsbfnIo, &DevSm->CtrlXfer);

      }
      break;

    case UsbTransferStatusAborted:
      DBG(EFI_D_VERBOSE, "Control IN transfer aborted");
      break;

    default: // Other statuses should not occur
      DBG(EFI_D_VERBOSE, "Unhandled transfer status on CTRL IN: %d", Utr->TransferStatus);
      Status = EFI_DEVICE_ERROR;
      break;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*******************************************************************************
 * Functions common to multiple states
 ******************************************************************************/

/**
 * @brief The event does not need to be handled in the current state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_SAME
 */
DEV_STATE Dev_EventNotHandled (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DBG(EFI_D_VERBOSE, "%a not handled in state %a",
      DevSmGetEventName(Message),
      DevSmGetStateName(DevSmGetCurrentState()));
  return DEV_STATE_SAME;
}


/**
 * @brief Logs that the event should not occur in the current state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_SAME
 */
DEV_STATE Dev_EventError (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DBG(EFI_D_ERROR, "%a not allowed in state %a",
      DevSmGetEventName(Message),
      DevSmGetStateName(DevSmGetCurrentState()));
  return DEV_STATE_SAME;
}


/**
 * @brief Always transition to default state on reset.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_DEFAULT_ADDRESSED
 */
DEV_STATE Dev_Common_Reset (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  FNC_ENTER_MSG();
  DBG(EFI_D_INFO, "Device Reset");

  (void) AbortXfer(DevSm->UsbfnIo, &DevSm->CtrlXfer);
  DevSm->BusAddress = 0;
  DevSm->BusSpeed = UsbBusSpeedUnknown;
  DevSm->UsbDevStatus = 0;

  FNC_LEAVE_MSG();
  return DEV_STATE_DEFAULT_ADDRESSED;
}


/**
 * @brief Transisiton to suspend state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_SUSPENDED
 */
DEV_STATE Dev_Common_Suspend (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  return DEV_STATE_SUSPENDED;
}


/**
 * @brief Transition to detached state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_DETACHED;
 */
DEV_STATE Dev_Common_Detach (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DBG(EFI_D_INFO, "Cable Detached");
  return DEV_STATE_DETACHED;
}


/**
 * @brief Process standard USB requests.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
DEV_STATE Dev_Common_SetupPkt (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DEV_STATE  NextState = DEV_STATE_SAME;
  EFI_STATUS Status;
  EFI_USB_DEVICE_REQUEST  *Req = NULL;

  FNC_ENTER_MSG();

  Req = &Payload->udr;

  // DebugPrint won't print the long string any more. Need to break it up.
  DBG(EFI_D_VERBOSE, "EfiUsbMsgSetupPacket:");
  DBG(EFI_D_VERBOSE, "  bmRequestType: 0x%02x", Req->RequestType);
  DBG(EFI_D_VERBOSE, "  bRequest:      0x%02x", Req->Request);
  DBG(EFI_D_VERBOSE, "  wValue:        0x%04x", Req->Value);
  DBG(EFI_D_VERBOSE, "  wIndex:        0x%04x", Req->Index);
  DBG(EFI_D_VERBOSE, "  wLength:       0x%04x", Req->Length);

  Status = ProcessStdRequest(DevSm, Req, &NextState);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Stall EfiUsbMsgSetupPacket:");
    DBG(EFI_D_ERROR, "  bmRequestType: 0x%02x", Req->RequestType);
    DBG(EFI_D_ERROR, "  bRequest:      0x%02x", Req->Request);
    DBG(EFI_D_ERROR, "  wValue:        0x%04x", Req->Value);
    DBG(EFI_D_ERROR, "  wIndex:        0x%04x", Req->Index);
    DBG(EFI_D_ERROR, "  wLength:       0x%04x", Req->Length);

    // Stall on error or unhandled request
    Status = DevSm->UsbfnIo->SetEndpointStallState(
        DevSm->UsbfnIo,
        USBDEV_CONTROL_EP,
        USB_REQUEST_EPDIR(Payload->udr.RequestType),
        TRUE);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
    }
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/**
 * @brief Process control transfer Rx events.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
DEV_STATE Dev_Common_EpStatusChangedRx (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE           Message,
  IN UINTN                       PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  EFI_STATUS Status    = EFI_SUCCESS;
  DEV_STATE  NextState = DEV_STATE_SAME;
  FNC_ENTER_MSG();

  USB_ASSERT_RETURN((Message == EfiUsbMsgEndpointStatusChangedRx), DEV_STATE_SAME);

  // Process control ep events
  switch (Payload->utr.EndpointIndex) {

  case USBDEV_CONTROL_EP:
    // Only handle standard request on control endpoint
    if (!DevSm->ClsVndReq) {
      Status = ProcessCtrlEpStatusChangedRx(DevSm, &Payload->utr, &NextState);

      // Stall Control Endpoint
      // For CI, both dir needs to be stalled
      // For SNPS, always stall physical endpoint zero
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Failed to update control transfer: %r", Status);
        Status = DevSm->UsbfnIo->SetEndpointStallState(
          DevSm->UsbfnIo,
          USBDEV_CONTROL_EP,
          EfiUsbEndpointDirectionDeviceRx,
          TRUE);
        if (EFI_ERROR(Status)) {
          DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
        }
      }
    }
    break;

  default:
    DBG(EFI_D_INFO, "EfiUsbMsgEndpointStatusChangedRx received on ep: 0x%02x",
      ENDPOINT_ADDR(Payload->utr.EndpointIndex, Payload->utr.EndpointDirection));
    break;
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/**
 * @brief Process control transfer Tx events.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
DEV_STATE Dev_Common_EpStatusChangedTx (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  EFI_STATUS Status    = EFI_SUCCESS;
  DEV_STATE  NextState = DEV_STATE_SAME;
  FNC_ENTER_MSG();

  USB_ASSERT_RETURN((Message == EfiUsbMsgEndpointStatusChangedTx), DEV_STATE_SAME);

  // Process control ep events
  switch (Payload->utr.EndpointIndex) {

  case USBDEV_CONTROL_EP:
    // Only handle standard request on control endpoint
    if (!DevSm->ClsVndReq) {
      Status = ProcessCtrlEpStatusChangedTx(DevSm, &Payload->utr, &NextState);

      // Stall Control Endpoint
      // For CI, both dir needs to be stalled
      // For SNPS, always stall physical endpoint zero
      if (EFI_ERROR(Status)) {
        DBG(EFI_D_ERROR, "Failed to update control transfer: %r", Status);
        Status = DevSm->UsbfnIo->SetEndpointStallState(
          DevSm->UsbfnIo,
          USBDEV_CONTROL_EP,
          EfiUsbEndpointDirectionDeviceRx,
          TRUE);
        if (EFI_ERROR(Status)) {
          DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
        }
      }
    }
    break;

  default:
    DBG(EFI_D_INFO, "EfiUsbMsgEndpointStatusChangedTx received on ep: 0x%02x",
      ENDPOINT_ADDR(Payload->utr.EndpointIndex, Payload->utr.EndpointDirection));
    break;
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/**
 * @brief Save the bus speed.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on speed
 */
DEV_STATE Dev_Common_Speed (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE           Message,
  IN UINTN                       PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DEV_STATE  NextState     = DEV_STATE_SAME;
  UINT16     MaxPacketSize = 0;
  EFI_STATUS Status        = EFI_SUCCESS;
  FNC_ENTER_MSG();

  USB_ASSERT_RETURN((Message == EfiUsbMsgBusEventSpeed), DEV_STATE_SAME);

  DevSm->BusSpeed = Payload->ubs;
  DBG(EFI_D_INFO, "Bus Speed: %a", PrintBusSpeed(DevSm->BusSpeed));

  // update the bulk ep size based on connected speed
  Status = DevSm->UsbfnIo->GetEndpointMaxPacketSize(DevSm->UsbfnIo, UsbEndpointBulk, DevSm->BusSpeed, &MaxPacketSize);
  if (EFI_ERROR(Status) || (DevSm->BusSpeed == UsbBusSpeedUnknown)) {
    DBG(EFI_D_ERROR, "GetEndpointMaxPacketSize failed: %r", Status);
  } else if (DevSm->BusSpeed != UsbBusSpeedSuper){
    UpdateEpMaxPktSize(&DevSm->DescriptorSet, USB_ENDPOINT_BULK, MaxPacketSize);
  } else {
    DBG(EFI_D_VERBOSE, "Use Default SS USB Pkt Size");
  }

  // Only advance from ATTACHED to DEFAULT/ADDRESSED on SS connection
  if (DevSm->BusSpeed == UsbBusSpeedSuper) {
    NextState = DEV_STATE_DEFAULT_ADDRESSED;
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/*******************************************************************************
 * DEV_STATE_DETACHED functions
 ******************************************************************************/

/**
 * @brief Initialize/clear internal state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] PrevState     State we are coming from
 */
VOID
Dev_Detached_Entry (
  IN DEV_STATE_MACHINE *DevSm,
  IN DEV_STATE PrevState)
{
  FNC_ENTER_MSG();

  DevSm->PortType = EfiUsbUnknownPort;
  DevSm->BusSpeed = UsbBusSpeedUnknown;
  DevSm->ResumeState = DEV_INITIAL_STATE;

  FNC_LEAVE_MSG();
}


/**
 * @brief Just move to attached state.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return DEV_STATE_ATTACHED
 */
DEV_STATE Dev_Detached_Attach (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DBG(EFI_D_INFO, "Cable Attached");
  return DEV_STATE_ATTACHED;
}


/*******************************************************************************
 * DEV_STATE_ATTACHED functions
 ******************************************************************************/

/**
 * @brief Detect the port type when cable is attached.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] PrevState     State we are coming from
 */
VOID
Dev_Attached_Entry (
  IN DEV_STATE_MACHINE *DevSm,
  IN DEV_STATE          PrevState)
{
  EFI_STATUS Status;
  FNC_ENTER_MSG();

  Status = DevSm->UsbfnIo->DetectPort(DevSm->UsbfnIo, &DevSm->PortType);
  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Failed to detect port type: %r", Status);
  } else {
    DBG(EFI_D_INFO, "Port Type is %a", PrintPortType(DevSm->PortType));
  }

  FNC_LEAVE_MSG();
}


/*******************************************************************************
 * DEV_STATE_DEFAULT_ADDRESSED functions
 ******************************************************************************/

/*******************************************************************************
 * DEV_STATE_CONFIGURED functions
 ******************************************************************************/

/**
 * @brief Check if the standard request need to be handled by the client.
 *
 * For HID class, it will use GET_DESCRIPTOR to get the class specific
 * descriptor.
 *
 *
 * @param[in] Req         The request to be checked
 *
 * @return TRUE if the request should be handled by the client
 * @return FALSE if the request can be handled by UsbDeviceDxe
 */
BOOLEAN
IsClassSpecificStdRequest (
  IN EFI_USB_DEVICE_REQUEST  *Req
)
{
  BOOLEAN IsClsSpecStdReq = FALSE;

  FNC_ENTER_MSG();

  // Type must be STD
  if (USB_SETUP_RT_TYPE(Req->RequestType) != USB_SETUP_RT_TYPE_STD) {
    DBG(EFI_D_ERROR, "Not handling non-standard request 0x%02x", USB_SETUP_RT_TYPE(Req->RequestType));
    goto ON_EXIT;
  }

  // Switch on setup request
  switch (Req->Request)
  {
    case USB_REQ_GET_DESCRIPTOR:
    {
      UINT8 DescType   =  USB_VALUE_TO_DESC_TYPE(Req->Value);
      switch (DescType)
      {
        case USB_DESC_TYPE_DEVICE:
        case USB_DESC_TYPE_CONFIG:
        case USB_DESC_TYPE_STRING:
        case USB_DESC_TYPE_DEVICE_QUALIFIER:
        case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
        case USB_DESC_TYPE_BOS:
        case USB_DESC_TYPE_DEBUG:
        case 0XFE: // special case for ch9 test.
          // not the best, but we should consider to write
          // a client for CH9 testing so we can remove
          // the special case here.
          IsClsSpecStdReq = FALSE;
        break;
        default:
          IsClsSpecStdReq = TRUE;
      }
    }
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return IsClsSpecStdReq;
 }


/**
 * @brief Process standard and class USB requests.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return Next state dependent on control transfer
 */
DEV_STATE Dev_Configured_SetupPkt (
  IN DEV_STATE_MACHINE         *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD *Payload )
{
  EFI_STATUS               Status    = EFI_SUCCESS;
  DEV_STATE                NextState = DEV_STATE_SAME;
  EFI_USB_DEVICE_REQUEST   *Req      = NULL;

  FNC_ENTER_MSG();

  Req = &Payload->udr;

  DBG(EFI_D_VERBOSE, "EfiUsbMsgSetupPacket:");
  DBG(EFI_D_VERBOSE, "  bmRequestType: 0x%02x", Req->RequestType);
  DBG(EFI_D_VERBOSE, "  bRequest:      0x%02x", Req->Request);
  DBG(EFI_D_VERBOSE, "  wValue:        0x%04x", Req->Value);
  DBG(EFI_D_VERBOSE, "  wIndex:        0x%04x", Req->Index);
  DBG(EFI_D_VERBOSE, "  wLength:       0x%04x", Req->Length);

  if (IsClassSpecificStdRequest(Req)) {
    Status = ProcessClsRequest(DevSm, Req, &NextState);
  }
  else if (USB_SETUP_RT_TYPE_STD == USB_SETUP_RT_TYPE(Req->RequestType)) {
    Status = ProcessStdRequest(DevSm, Req, &NextState);
  }
  else if (USB_SETUP_RT_TYPE_CLS == USB_SETUP_RT_TYPE(Req->RequestType)) {
    Status = ProcessClsRequest(DevSm, Req, &NextState);
  }
  else {
    Status = EFI_UNSUPPORTED;
  }

  if (EFI_ERROR(Status)) {
    DBG(EFI_D_ERROR, "Stall EfiUsbMsgSetupPacket:");
    DBG(EFI_D_ERROR, "  bmRequestType: 0x%02x", Req->RequestType);
    DBG(EFI_D_ERROR, "  bRequest:      0x%02x", Req->Request);
    DBG(EFI_D_ERROR, "  wValue:        0x%04x", Req->Value);
    DBG(EFI_D_ERROR, "  wIndex:        0x%04x", Req->Index);
    DBG(EFI_D_ERROR, "  wLength:       0x%04x", Req->Length);

    // Stall on error or unhandled request
    Status = DevSm->UsbfnIo->SetEndpointStallState(
      DevSm->UsbfnIo,
      USBDEV_CONTROL_EP,
      (Payload->udr.RequestType >> 7) & 1
      ? EfiUsbEndpointDirectionHostIn : EfiUsbEndpointDirectionHostOut,
      TRUE);
    if (EFI_ERROR(Status)) {
      DBG(EFI_D_ERROR, "SetEndpointStallState failed: %r", Status);
    }
  }

  FNC_LEAVE_MSG();
  return NextState;
}


/*******************************************************************************
 * DEV_STATE_SUSPENDED functions
 ******************************************************************************/

/**
 * @brief Save the previous state on suspend.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] PrevState     State we are coming from
 */
VOID
Dev_Suspended_Entry (
  IN DEV_STATE_MACHINE *DevSm,
  IN DEV_STATE          PrevState)
{
  DevSm->ResumeState = PrevState;
  DBG(EFI_D_INFO, "Bus suspended in state %a", DevSmGetStateName(PrevState));
}


/**
 * @brief Return to state saved on suspend entry.
 *
 * @param[in] DevSm         Usbfn state machine instance
 * @param[in] Message       UsbfnIo message
 * @param[in] PayloadSize   Size of data in Payload
 * @param[in] Payload       UsbfnIo message payload
 *
 * @return State saved on suspend
 */
DEV_STATE Dev_Suspended_Resume (
  IN DEV_STATE_MACHINE          *DevSm,
  IN EFI_USBFN_MESSAGE          Message,
  IN UINTN                      PayloadSize,
  IN EFI_USBFN_MESSAGE_PAYLOAD  *Payload )
{
  DBG(EFI_D_INFO, "Resuming to state %a", DevSmGetStateName(DevSm->ResumeState));
  return DevSm->ResumeState;
}
