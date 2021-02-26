/** @file UsbInitDxe.c

The driver supports the on demand USB initialization.

Copyright (c) 2016 Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
EDIT HISTORY

when       who      what, where, why
--------   ---      ----------------------------------------------------------
04/12/16   ck       Init version
=============================================================================*/
#include "UsbInitDxe.h"

/**
* @brief Driver exit function that is called on gBS->ExitBootServices()
*
* @param  Event                   Pointer to this event
* @param  Context                 Pointer to a DEV_CTXT instance
*/
VOID
EFIAPI
UsbInitExitBootService(
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  FNC_ENTER_MSG();

  DEV_CTXT *DevCtxt = NULL;

  if (NULL == Context) {
    DBG(EFI_D_ERROR, "invalid parameter");
    goto ON_EXIT;
  }

  DevCtxt = (DEV_CTXT *)Context;

  // Close the ExitBootServices event and free the device
  gBS->CloseEvent(DevCtxt->ExitBootServiceEvent);

  // Free the DEV_CTXT object
  FreePool(DevCtxt);

ON_EXIT:
  FNC_LEAVE_MSG();
}

/**
@summary
Convert the QCOM_USB_MODE_TYPE input to EFI_USB_MODE_TYPE output.

The output of UsbCfgProtocol->GetSupUsbMode is the OR values of either one, two,
or all of the three mode type values, USB_DEVICE_MODE, USB_HOST_MODE,
USB_DUAL_ROLE_MODE.

Before returning the USB_CORE_INFO, each bit of the CoreInfo->ModeType needs to
be set.

@param [in]  QcUsbModeType   Instance of QCOM_USB_MODE_TYPE.
@param [out] EfiUsbModeType  Pointer to the EFI_USB_MODE_TYPE instance

@retval EFI_SUCCESS          The operation succeeded.
@retval Others               The operation failed.

**/
STATIC
EFI_STATUS
UsbInitPrepCoreTypeForCoreInfo(
  IN   QCOM_USB_MODE_TYPE   QcUsbModeType,
  OUT  EFI_USB_MODE_TYPE    *EfiUsbModeType
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!EfiUsbModeType) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  *EfiUsbModeType = EFI_USB_INVALID_MODE;

  if (QcUsbModeType & USB_DEVICE_MODE)
    *EfiUsbModeType |= EFI_USB_PERIPHERAL_MODE;
  if (QcUsbModeType & USB_HOST_MODE)
    *EfiUsbModeType |= EFI_USB_HOST_MODE;
  if (QcUsbModeType & USB_DUAL_ROLE_MODE)
    *EfiUsbModeType |= EFI_USB_DUAL_ROLE_MODE;

ON_EXIT:
  return Status;
}

/**
@summary
Prepare the input of QCOM_USB_MODE_TYPE for start and stop core operation.

For the input to the start or stop operation provided by the config protocol,
the only meaningful value of QCOM_USB_MODE_TYPE is USB_DUAL_ROLE_MODE,
USB_DEVICE_MODE, or USB_HOST_MODE. ORing the QCOM_USB_MODE_TYPEs is not supported
as the hardware can only be configured to one of the modes.

Therefore, if the EfiUsbModeType contains more than one mode, only one
QCOM_USB_MODE_TYPE will be selected.

@param [in]  EfiUsbModeType  Instance of the EFI_USB_MODE_TYPE instance.
@param [out] QcUsbModeType   Pointer the an instance of QCOM_USB_MODE_TYPE.

@retval EFI_SUCCESS         The operation succeeded.
@retval Others              The operation failed.

**/
STATIC
EFI_STATUS
UsbInitPrepCoreTypeForCoreOp(
  IN  EFI_USB_MODE_TYPE    EfiUsbModeType,
  OUT QCOM_USB_MODE_TYPE  *QcUsbModeType
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!QcUsbModeType) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  if (EFI_USB_DUAL_ROLE_MODE & EfiUsbModeType)
    *QcUsbModeType = USB_DUAL_ROLE_MODE;
  else if (EFI_USB_PERIPHERAL_MODE & EfiUsbModeType)
    *QcUsbModeType = USB_DEVICE_MODE;
  else if (EFI_USB_HOST_MODE & EfiUsbModeType)
    *QcUsbModeType = USB_HOST_MODE;
  else
    *QcUsbModeType = USB_INVALID_MODE;

ON_EXIT:
  return Status;
}

/**
@summary
The function implements EFI_USB_INIT_PROTOCOL_QUERY_CORE_COUNT

@see EFIUsbInitProtocol.h
**/
EFI_STATUS
EFIAPI
UsbInitQueryCoreCount(
  IN      EFI_USB_INIT_PROTOCOL *This,
  OUT     UINT8                 *Count
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  DEV_CTXT *DevCtxt = NULL;

  FNC_ENTER_MSG();

  if (!This || !Count) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  DevCtxt = DEV_CTXT_FROM_THIS(This);

  Status = DevCtxt->UsbCfgProtocol->GetCoreCount(
    DevCtxt->UsbCfgProtocol,
    Count);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}

/**
@summary
The function implements EFI_USB_INIT_PROTOCOL_QUERY_CORES_INFO

@see EFIUsbInitProtocol.h
**/
EFI_STATUS
EFIAPI
UsbInitQueryCoreInfo(
  IN      EFI_USB_INIT_PROTOCOL *This,
  IN      UINT8                 *CoreNum, OPTIONAL
  OUT     USB_CORE_INFO         *CoreInfo
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  DEV_CTXT *DevCtxt = NULL;
  QCOM_USB_MODE_TYPE QcUsbModeType;

  FNC_ENTER_MSG();

  if (!This || !CoreInfo) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  DevCtxt = DEV_CTXT_FROM_THIS(This);

  if (!CoreNum)
    CoreInfo->CoreNum = 0;
  else
    CoreInfo->CoreNum = *CoreNum;

  Status = DevCtxt->UsbCfgProtocol->GetSupUsbMode(
    DevCtxt->UsbCfgProtocol,
    CoreInfo->CoreNum,
    &QcUsbModeType);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  Status = UsbInitPrepCoreTypeForCoreInfo(QcUsbModeType, &CoreInfo->ModeType);
  if (EFI_ERROR(Status))
    goto ON_EXIT;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}

/**
@summary
The function implements EFI_USB_INIT_PROTOCOL_START_CONTROLLER

@see EFIUsbInitProtocol.h
**/
EFI_STATUS
EFIAPI
UsbInitStartController(
  IN EFI_USB_INIT_PROTOCOL  *This,
  IN UINT8                   CoreNum,
  IN EFI_USB_MODE_TYPE       ModeType
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  DEV_CTXT *DevCtxt = NULL;
  QCOM_USB_MODE_TYPE CfgModeType;

  FNC_ENTER_MSG();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  DevCtxt = DEV_CTXT_FROM_THIS(This);

  Status = UsbInitPrepCoreTypeForCoreOp(ModeType, &CfgModeType);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  Status = DevCtxt->UsbCfgProtocol->StartController(
    DevCtxt->UsbCfgProtocol,
    CoreNum,
    CfgModeType);
  if (EFI_ERROR(Status))
    goto ON_EXIT;

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}

/**
@summary
The function implements EFI_USB_INIT_PROTOCOL_STOP_CONTROLLER

@see EFIUsbInitProtocol.h
**/
EFI_STATUS
EFIAPI
UsbInitStopController(
  IN EFI_USB_INIT_PROTOCOL  *This,
  IN UINT8                   CoreNum,
  IN EFI_USB_MODE_TYPE       ModeType
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  DEV_CTXT *DevCtxt = NULL;
  QCOM_USB_MODE_TYPE CfgModeType;

  FNC_ENTER_MSG();

  if (!This) {
    Status = EFI_INVALID_PARAMETER;
    ERR_CHK("invalid parameter");
  }

  DevCtxt = DEV_CTXT_FROM_THIS(This);

  Status = UsbInitPrepCoreTypeForCoreOp(ModeType, &CfgModeType);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  Status = DevCtxt->UsbCfgProtocol->StopController(
    DevCtxt->UsbCfgProtocol,
    CoreNum,
    CfgModeType);
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}

/**
* @brief Create and initialize a DEV_CTXT instance
*
* @retval New instance on success
* @retval NULL on failure
*/
STATIC
DEV_CTXT *
CreateDeviceInstance(
  VOID
  )
{
  EFI_STATUS             Status = EFI_SUCCESS;
  DEV_CTXT              *DevCtxt = NULL;

  FNC_ENTER_MSG();

  // Allocate the software device structure
  DevCtxt = (DEV_CTXT *)AllocateZeroPool(sizeof(DEV_CTXT));
  if (!DevCtxt) {
    Status = EFI_OUT_OF_RESOURCES;
    DBG(EFI_D_ERROR, "memory allocation failed for USB FN");
    goto ON_EXIT;
  }

  DevCtxt->Signature = USB_INIT_DEV_SIGNATURE;

  DevCtxt->UsbInitProtocol.Revision = EFI_USB_INIT_PROTOCOL_REVISION;
  DevCtxt->UsbInitProtocol.QueryCoreCount = UsbInitQueryCoreCount;
  DevCtxt->UsbInitProtocol.QueryCoreInfo = UsbInitQueryCoreInfo;
  DevCtxt->UsbInitProtocol.StartController = UsbInitStartController;
  DevCtxt->UsbInitProtocol.StopController = UsbInitStopController;

  Status = gBS->LocateProtocol(
    &gQcomUsbConfigProtocolGuid,
    NULL,
    (VOID **)&DevCtxt->UsbCfgProtocol);
  ERR_CHK("Failed to get gQcomUsbConfigProtocolGuid");

  // Register for boot services exit event.
  Status = gBS->CreateEventEx(
    EVT_NOTIFY_SIGNAL,
    TPL_CALLBACK,
    UsbInitExitBootService,
    DevCtxt,
    &gEfiEventExitBootServicesGuid,
    &DevCtxt->ExitBootServiceEvent);

ON_EXIT:
  if (EFI_ERROR(Status) && DevCtxt) {
    FreePool(DevCtxt);
    DevCtxt = NULL;
  }

  FNC_LEAVE_MSG();
  return DevCtxt;
}

/**
Entry point for EFI drivers.

@param  ImageHandle       EFI_HANDLE.
@param  SystemTable       EFI_SYSTEM_TABLE.

@return EFI_SUCCESS       Success.
        EFI_DEVICE_ERROR  Fail.

**/
EFI_STATUS
EFIAPI
UsbInitDriverEntryPoint(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  DEV_CTXT   *DevCtxt = NULL;

  FNC_ENTER_MSG();

  DevCtxt = CreateDeviceInstance();
  if (!DevCtxt) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  // Install UsbfnIo protocol.
  Status = gBS->InstallMultipleProtocolInterfaces(
    &ImageHandle,
    &gEfiUsbInitProtocolGuid, &DevCtxt->UsbInitProtocol, NULL);
  ERR_CHK("unable to install USB Init Protocol");

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}