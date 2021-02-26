/**
 * @file UsbfnIoTest.c
 *
 * Provides test functions to verify the proper operation of the UsbfnIo
 * policy protocol functions.
 *
 * Copyright (c) 2014-2015, QUALCOMM Technologies Inc. All rights reserved.
 */

/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/20/15   ck      Remove deprecated functions
 02/06/14   ml      Initial revision
=============================================================================*/


#include "UsbfnIoTest.h"


/**
 * Helper function to get a UsbfnIo endpoint policy.
 *
 * @param[in]     UsbfnIo        Usbfn protocol instance
 * @param[in]     EndpointIndex  Logical endpoint number
 * @param[in]     Direction      Endpoint direction
 * @param[in]     Policy         Endpoint policy to get
 * @param[in,out] BufferSize     Size of buffer on input, size of data written on output
 * @param[in]     Buffer         Buffer to hold Policy value
 *
 * @retval EFI_SUCCESS           Policy successfully returned
 * @retval other                 Failed to get endpoint policy
 */
EFI_STATUS
GetUsbFnEndpointPolicy (
  IN      EFI_USBFN_IO_PROTOCOL         *UsbfnIo,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN      EFI_USBFN_POLICY_TYPE         Policy,
  IN OUT  UINTN                         *BufferSize,
  OUT     VOID                          *Buffer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG();

  if (NULL == UsbfnIo || NULL == BufferSize || NULL == Buffer) {
    PRINT(APL_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  PRINT(APL_VERB, "Ep %d, EpDir %d, Policy %d, BufferSize %d, Buffer 0x%p",
      EndpointIndex, Direction, Policy, *BufferSize, Buffer);

  Status = UsbfnIo->GetEndpointPolicy(
      UsbfnIo,
      EndpointIndex,
      Direction,
      Policy,
      BufferSize,
      Buffer
      );
  if (EFI_ERROR (Status)) {
    PRINT(APL_ERROR, "Ep %d, EpDir %d, Policy %d, Failed with status %r (0x%x)",
        EndpointIndex, Direction, Policy, Status, Status);
    goto ON_EXIT;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/**
 * Helper function to set a UsbfnIo endpoint policy.
 *
 * @param[in]     UsbfnIo        Usbfn protocol instance
 * @param[in]     EndpointIndex  Logical endpoint number
 * @param[in]     Direction      Endpoint direction
 * @param[in]     Policy         Endpoint policy to get
 * @param[in]     BufferSize     Size of buffer
 * @param[in]     Buffer         Buffer to hold Policy value
 *
 * @retval EFI_SUCCESS           Policy successfully applied
 * @retval other                 Failed to apply endpoint policy
 */
EFI_STATUS
SetUsbFnEndpointPolicy (
  IN      EFI_USBFN_IO_PROTOCOL         *UsbfnIo,
  IN      UINT8                         EndpointIndex,
  IN      EFI_USBFN_ENDPOINT_DIRECTION  Direction,
  IN      EFI_USBFN_POLICY_TYPE         Policy,
  IN      UINTN                         BufferSize,
  IN      VOID                          *Buffer
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  FNC_ENTER_MSG();

  if (NULL == UsbfnIo || NULL == Buffer) {
    PRINT(APL_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  PRINT(APL_VERB, "Ep %d, EpDir %d, Policy %d, BufferSize %d, Buffer 0x%p",
      EndpointIndex, Direction, Policy, BufferSize, Buffer);

  Status = UsbfnIo->SetEndpointPolicy(
      UsbfnIo,
      EndpointIndex,
      Direction,
      Policy,
      BufferSize,
      Buffer
      );
  if (EFI_ERROR (Status)) {
    PRINT(APL_ERROR, "Ep %d, EpDir %d, Policy %d, Failed with status %r (0x%x)",
        EndpointIndex, Direction, Policy, Status, Status);
    goto ON_EXIT;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for function documentation.
 */
EFI_STATUS
TestUsbfnIoSetEndpointPolicy (
  IN  EFI_USBFN_IO_PROTOCOL         *UsbfnIo
  )
{
  EFI_STATUS  Status     = EFI_DEVICE_ERROR;
  UINTN       Buffer     = 0;
  UINTN       BufferSize = sizeof(UINTN);

  FNC_ENTER_MSG();

  if (NULL == UsbfnIo) {
    PRINT(APL_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  //
  // Set Endpoint Policy
  //

  // EFI_ERROR(Status) must be true since EfiUsbPolicyUndefined is used
  Buffer= 0;
  BufferSize = sizeof(BOOLEAN);
  Status = SetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,                              // Ep
      EfiUsbEndpointDirectionDeviceTx,            // EpDir
      EfiUsbPolicyUndefined,                      // Policy
      BufferSize,                                 // BufferSize
      (VOID *) &Buffer);                          // Buffer

  if (!EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "TEST: SetUsbFnEndpointPolicy (EfiUsbPolicyUndefined) failed");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  PRINT(APL_ALWAYS, "TEST: SetUsbFnEndpointPolicy (EfiUsbPolicyUndefined) passed");

  // EFI_ERROR(Status) must be true
  // since EfiUsbPolicyZeroLengthTermination+1 is not an supported policy
  Buffer = 0;
  BufferSize = sizeof(BOOLEAN);
  Status = SetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceTx,
      (EFI_USBFN_POLICY_TYPE)((UINT8)EfiUsbPolicyZeroLengthTermination+1),
      BufferSize,
      (VOID *) &Buffer);

  if (!EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "TEST: SetUsbFnEndpointPolicy (EfiUsbPolicyZeroLengthTermination+1) failed");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  PRINT(APL_ALWAYS, "TEST: SetUsbFnEndpointPolicy (EfiUsbPolicyZeroLengthTermination+1) passed");

  //// Set EfiUsbPolicyZeroLengthTerminationSupport is not supported
  Buffer = 0;
  BufferSize = sizeof(BOOLEAN);
  Status = SetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceTx,
      EfiUsbPolicyZeroLengthTerminationSupport,
      BufferSize,
      (VOID *) &Buffer);

  if (!EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "TEST: SetUsbFnEndpointPolicy TX (EfiUsbPolicyZeroLengthTerminationSupport) failed");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "TEST: SetUsbFnEndpointPolicy TX (EfiUsbPolicyZeroLengthTerminationSupport) passed");

  Buffer = 0;
  BufferSize = sizeof(BOOLEAN);
  Status = SetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceRx,
      EfiUsbPolicyZeroLengthTerminationSupport,
      BufferSize,
      (VOID *) &Buffer);

  if (!EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "TEST: SetUsbFnEndpointPolicy RX (EfiUsbPolicyZeroLengthTerminationSupport) failed");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "TEST: SetUsbFnEndpointPolicy RX (EfiUsbPolicyZeroLengthTerminationSupport) passed");

  //// ZLT Enabled or not
  // Set EfiUsbPolicyZeroLengthTermination on TX endpoint is supported
  // Set EfiUsbPolicyZeroLengthTermination on RX endpoint is supported
  Buffer = TRUE;
  BufferSize = sizeof(BOOLEAN);
  Status = SetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceTx,
      EfiUsbPolicyZeroLengthTermination,
      BufferSize,
      (VOID *) &Buffer);

  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "SetUsbFnEndpointPolicy TX (EfiUsbPolicyZeroLengthTermination) failed with status 0x%x", Status);
    goto ON_EXIT;
  }

  Buffer = FALSE;
  BufferSize = sizeof(BOOLEAN);
  Status = GetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceTx,
      EfiUsbPolicyZeroLengthTermination,
      &BufferSize,
      (VOID *) &Buffer);

  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "GetUsbFnEndpointPolicy TX (EfiUsbPolicyZeroLengthTermination) failed with status 0x%x", Status);
    goto ON_EXIT;
  }

  if (TRUE == Buffer) {
    PRINT(APL_ALWAYS, "TEST: SetUsbFnEndpointPolicy TX (EfiUsbPolicyZeroLengthTermination) passed");
  }

  Buffer = TRUE;
  BufferSize = sizeof(BOOLEAN);
  Status = SetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceRx,
      EfiUsbPolicyZeroLengthTermination,
      BufferSize,
      (VOID *) &Buffer);

  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "SetUsbFnEndpointPolicy RX (EfiUsbPolicyZeroLengthTermination) failed with status 0x%x", Status);
    PRINT(APL_ALWAYS, "TEST: SetUsbFnEndpointPolicy RX (EfiUsbPolicyZeroLengthTermination) passed");
  } else {
    PRINT(APL_ALWAYS, "TEST: SetUsbFnEndpointPolicy RX (EfiUsbPolicyZeroLengthTermination) succeeded");
  }

  //// Max Transfer Size
  // Max Transfer Size can only be Queried but cannot be set
  Buffer = 16 * 1024 * 1024 + 512;
  BufferSize = sizeof(UINTN);
  Status = SetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceTx,
      EfiUsbPolicyMaxTransactionSize,
      BufferSize,
      (VOID *) &Buffer);

  if (!EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "TEST: SetUsbFnEndpointPolicy TX (EfiUsbPolicyMaxTransactionSize) failed");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "TEST: SetUsbFnEndpointPolicy TX (EfiUsbPolicyMaxTransactionSize) passed");

  Buffer = 16 * 1024 * 1024 + 512;
  BufferSize = sizeof(UINTN);
  Status = SetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceRx,
      EfiUsbPolicyMaxTransactionSize,
      BufferSize,
      (VOID *) &Buffer);

  if (!EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "TEST: SetUsbFnEndpointPolicy RX (EfiUsbPolicyMaxTransactionSize) failed");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "TEST: SetUsbFnEndpointPolicy RX (EfiUsbPolicyMaxTransactionSize) passed");

  Status = EFI_SUCCESS;
ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for function documentation.
 */
EFI_STATUS
TestUsbfnIoGetEndpointPolicy (
  IN  EFI_USBFN_IO_PROTOCOL         *UsbfnIo
  )
{
  EFI_STATUS      Status     = EFI_SUCCESS;
  UINTN           Buffer     = 0;
  UINTN           BufferSize = sizeof(UINTN);
  const UINTN     DEFAULT_MAX_TRANSACTION_SIZE = 16 * 1024 * 1024;

  FNC_ENTER_MSG();

  if (NULL == UsbfnIo) {
    PRINT(APL_ERROR, "invalid parameter");
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }

  //
  // Get Endpoint Policy
  //

  // EFI_ERROR(Status) must be true
  // since EfiUsbPolicyUndefined is used
  BufferSize = sizeof(UINTN);
  Buffer     = 0;
  Status = GetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,                              // Ep
      EfiUsbEndpointDirectionDeviceTx,            // EpDir
      EfiUsbPolicyUndefined,                      // Policy
      &BufferSize,                                // BufferSize
      (VOID *) &Buffer);                          // Buffer

  if (!EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "TEST: GetUsbFnEndpointPolicy (EfiUsbPolicyUndefined) failed");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  PRINT(APL_ALWAYS, "TEST: GetUsbFnEndpointPolicy (EfiUsbPolicyUndefined) passed");

  // EFI_ERROR(Status) must be true
  // since EfiUsbPolicyZeroLengthTermination+1 is not an supported policy
  BufferSize = sizeof(UINTN);
  Buffer     = 0;

  Status = GetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceTx,
      (EFI_USBFN_POLICY_TYPE) ((UINT8)EfiUsbPolicyZeroLengthTermination+1),
      &BufferSize,
      (VOID *) &Buffer);

  if (!EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "TEST: GetUsbFnEndpointPolicy (EfiUsbPolicyZeroLengthTermination+1) failed");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  PRINT(APL_ALWAYS, "TEST: GetUsbFnEndpointPolicy (EfiUsbPolicyZeroLengthTermination + 1) passed");

  //// ZLT Supported or not by HW
  // HW supported ZLT on both direction
  BufferSize = sizeof(UINTN);
  Buffer     = 0;

  Status = GetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceTx,
      EfiUsbPolicyZeroLengthTerminationSupport,
      &BufferSize,
      (VOID *) &Buffer);

  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "GetUsbFnEndpointPolicy TX (EfiUsbPolicyZeroLengthTerminationSupport) failed with status 0x%x", Status);
    goto ON_EXIT;
  }

  if (TRUE == Buffer) {
    PRINT(APL_ALWAYS, "TEST: GetUsbFnEndpointPolicy  TX (EfiUsbPolicyZeroLengthTerminationSupport) passed");
  } else {
    PRINT(APL_ERROR, "TEST: GetUsbFnEndpointPolicy TX (EfiUsbPolicyZeroLengthTerminationSupport) failed");
    goto ON_EXIT;
  }

  BufferSize = sizeof(UINTN);
  Buffer     = 0;

  Status = GetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceRx,
      EfiUsbPolicyZeroLengthTerminationSupport,
      &BufferSize,
      (VOID *) &Buffer);

  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "GetUsbFnEndpointPolicy RX (EfiUsbPolicyZeroLengthTerminationSupport) failed with status 0x%x", Status);
    goto ON_EXIT;
  }

  if (TRUE == Buffer) {
    PRINT(APL_ALWAYS, "TEST: GetUsbFnEndpointPolicy  RX (EfiUsbPolicyZeroLengthTerminationSupport) passed");
  } else {
    PRINT(APL_ERROR, "TEST: GetUsbFnEndpointPolicy RX (EfiUsbPolicyZeroLengthTerminationSupport) failed");
    goto ON_EXIT;
  }

  //// ZLT Enabled or not
  // By default, ZLT support are disabled for all endpoints
  Status = GetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceTx,
      EfiUsbPolicyZeroLengthTermination,
      &BufferSize,
      (VOID *) &Buffer);

  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "GetUsbFnEndpointPolicy TX (EfiUsbPolicyZeroLengthTermination) TX failed with status 0x%x", Status);
    goto ON_EXIT;
  }

  if (FALSE == Buffer) {
    PRINT(APL_ALWAYS, "TEST: GetUsbFnEndpointPolicy  TX (EfiUsbPolicyZeroLengthTermination) passed");
  } else {
    PRINT(APL_ERROR, "TEST: GetUsbFnEndpointPolicy TX (EfiUsbPolicyZeroLengthTermination) failed");
    goto ON_EXIT;
  }

  BufferSize = sizeof(UINTN);
  Buffer     = 0;

  Status = GetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceRx,
      EfiUsbPolicyZeroLengthTermination,
      &BufferSize,
      (VOID *) &Buffer);

  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "GetUsbFnEndpointPolicy RX (EfiUsbPolicyZeroLengthTermination) RX failed with status 0x%x", Status);
    goto ON_EXIT;
  }

  if (FALSE == Buffer) {
    PRINT(APL_ALWAYS, "TEST: GetUsbFnEndpointPolicy  RX (EfiUsbPolicyZeroLengthTermination) passed");
  } else {
    PRINT(APL_ERROR, "TEST: GetUsbFnEndpointPolicy RX (EfiUsbPolicyZeroLengthTermination) failed");
    goto ON_EXIT;
  }

  //// Max Transfer Size
  // 16 MB is the default max transfer size

  // TX
  BufferSize = sizeof(UINTN);
  Buffer     = 0;

  Status = GetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceTx,
      EfiUsbPolicyMaxTransactionSize,
      &BufferSize,
      (VOID *) &Buffer);

  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "GetUsbFnEndpointPolicy TX (EfiUsbPolicyMaxTransactionSize) RX failed with status 0x%x", Status);
    goto ON_EXIT;
  }

  if (DEFAULT_MAX_TRANSACTION_SIZE == Buffer) {
    PRINT(APL_ALWAYS, "TEST: GetUsbFnEndpointPolicy  TX (EfiUsbPolicyMaxTransactionSize) passed");
  } else {
    PRINT(APL_ERROR, "TEST: GetUsbFnEndpointPolicy TX (EfiUsbPolicyMaxTransactionSize) failed");
    goto ON_EXIT;
  }

  // RX
  BufferSize = sizeof(UINTN);
  Buffer     = 0;

  Status = GetUsbFnEndpointPolicy(
      UsbfnIo,
      USBLB_BULK_EP,
      EfiUsbEndpointDirectionDeviceRx,
      EfiUsbPolicyMaxTransactionSize,
      &BufferSize,
      (VOID *) &Buffer);

  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "GetUsbFnEndpointPolicy RX (EfiUsbPolicyMaxTransactionSize) RX failed with status 0x%x", Status);
    goto ON_EXIT;
  }

  if (DEFAULT_MAX_TRANSACTION_SIZE == Buffer) {
    PRINT(APL_ALWAYS, "TEST: GetUsbFnEndpointPolicy  RX (EfiUsbPolicyMaxTransactionSize) passed");
  } else {
    PRINT(APL_ERROR, "TEST: GetUsbFnEndpointPolicy RX (EfiUsbPolicyMaxTransactionSize) failed");
    goto ON_EXIT;
  }

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}


/*
 * See header for function documentation.
 */
EFI_STATUS
TestUsbfnIoQueryDeviceInformation (
  IN  EFI_USBFN_IO_PROTOCOL         *UsbfnIo
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT16     MaxPktSize = 0;
  UINT16     Pid = 0;
  UINT16     Vid = 0;
  UINT8      Buffer[128];
  UINTN      BufferSize;

  FNC_ENTER_MSG();

  // Get Max Packet Size supported for high speed control eps
  Status = UsbfnIo->GetEndpointMaxPacketSize(
      UsbfnIo,
      UsbEndpointControl,
      UsbBusSpeedHigh,
      &MaxPktSize);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to get control max packet size: %r", Status);
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "Maximum packet size for control: %d", MaxPktSize);

  // Get Max Packet Size supported for high speed bulk eps
  Status = UsbfnIo->GetEndpointMaxPacketSize(
      UsbfnIo,
      UsbEndpointBulk,
      UsbBusSpeedHigh,
      &MaxPktSize);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to get bulk max packet size: %r", Status);
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "Maximum packet size for bulk: %d", MaxPktSize);

  // Query the VID/PID
  Status = UsbfnIo->GetVendorIdProductId(UsbfnIo, &Vid, &Pid);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "GetVendorIdProductId failed with status %r", Status);
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "Vid 0x%04x, Pid 0x%04x", Vid, Pid);

  // Verify correct VID/PID
  if (!(Vid == USBLB_VENDOR_ID && Pid == USBLB_PRODUCT_ID)) {
    PRINT(APL_ERROR, "VID/PID don't match expected values");
    Status = EFI_DEVICE_ERROR;
    goto ON_EXIT;
  }

  // Query the device manufacturer name
  BufferSize = sizeof(Buffer);
  SetMem(Buffer, BufferSize, 0);
  Status = UsbfnIo->GetDeviceInfo(
      UsbfnIo,
      EfiUsbDeviceInfoManufacturerName,
      &BufferSize,
      Buffer);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to query manufacturer name: %r", Status);
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "Manufacturer: %s", Buffer);

  // Query the device product name
  BufferSize = sizeof(Buffer);
  SetMem(Buffer, BufferSize, 0);
  Status = UsbfnIo->GetDeviceInfo(
      UsbfnIo,
      EfiUsbDeviceInfoProductName,
      &BufferSize,
      Buffer);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to query product name: %r", Status);
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "Product Name: %s", Buffer);

  // Query the device serial number
  BufferSize = sizeof(Buffer);
  SetMem(Buffer, BufferSize, 0);
  Status = UsbfnIo->GetDeviceInfo(
      UsbfnIo,
      EfiUsbDeviceInfoSerialNumber,
      &BufferSize,
      Buffer);
  if (EFI_ERROR(Status)) {
    PRINT(APL_ERROR, "Failed to query serial number: %r", Status);
    goto ON_EXIT;
  }
  PRINT(APL_ALWAYS, "Serial Number: %s", Buffer);

ON_EXIT:
  FNC_LEAVE_MSG();
  return Status;
}

