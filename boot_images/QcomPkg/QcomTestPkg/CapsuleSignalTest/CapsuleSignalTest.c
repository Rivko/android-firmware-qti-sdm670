/** @file CapsuleSignalTest.c
   
  Tests for signaling capsules have been consumed.

  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/13/12   niting  Initial version
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>

EFI_GUID qcom_fw_update_test_system_fw_guid = \
  { \
    0x28D578E6, 0xA736, 0x4A8A, { 0x88, 0xA8, 0x94, 0xC4, 0x7E, 0x8F, 0x80, 0x47 } \
  };
EFI_EVENT qcom_fw_update_test_system_fw_event;


EFI_GUID qcom_fw_system_fw_test2_guid = \
  { \
    0xb31d4953, 0x342a, 0x44e6, {0x94, 0xad, 0xcb, 0x6d, 0x3d, 0xf8, 0x4d, 0xe0 } \
  };
EFI_EVENT qcom_fw_system_fw_test2_event;

EFI_GUID qcom_fw_device_fw_test1_guid = \
  { \
    0x585322cc, 0xb7a6, 0x4568, {0x97, 0x1a, 0x5a, 0xc2, 0x38, 0x81, 0x9c, 0x52 } \
  };
EFI_EVENT qcom_fw_device_fw_test1_event;

EFI_GUID qcom_fw_device_fw_test2_guid = \
  { \
    0x115795d3, 0x38db, 0x4ba9, {0xb5, 0x1c, 0xd0, 0x86, 0xba, 0xef, 0x9e, 0xa0 } \
  };
EFI_EVENT qcom_fw_device_fw_test2_event;

/** 
   
  Empty callback function.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
EmptyCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  AsciiPrint("Callback signaled\n");
}

/**
  Entry point for the application

  @param[in] ImageHandle    Image handle 
  @param[in] SystemTable    Pointer to the System Table
  
  @retval EFI_SUCCESS       Execution successful
  @retval other             Error occurred

**/
EFI_STATUS
EFIAPI
UefiAppMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  TEST_START("CapsuleSignalTest");

  do
  {
    // Create event for capsule signaling
    AsciiPrint("Signaling qcom_fw_update_test_system_fw_event\n");
    Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                EmptyCallback,
                NULL,
                &qcom_fw_update_test_system_fw_guid,
                &qcom_fw_update_test_system_fw_event
                );
    if (Status != EFI_SUCCESS)
    {
      break;
    }

    Status = gBS->SignalEvent(qcom_fw_update_test_system_fw_event);
    if (Status != EFI_SUCCESS)
    {
      break;
    }

    // Create event for capsule signaling
    AsciiPrint("Signaling qcom_fw_system_fw_test2_event\n");
    Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                EmptyCallback,
                NULL,
                &qcom_fw_system_fw_test2_guid,
                &qcom_fw_system_fw_test2_event
                );
    if (Status != EFI_SUCCESS)
    {
      break;
    }

    Status = gBS->SignalEvent(qcom_fw_system_fw_test2_event);
    if (Status != EFI_SUCCESS)
    {
      break;
    }

    // Create event for capsule signaling
    AsciiPrint("Signaling qcom_fw_device_fw_test1_event\n");
    Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                EmptyCallback,
                NULL,
                &qcom_fw_device_fw_test1_guid,
                &qcom_fw_device_fw_test1_event
                );
    if (Status != EFI_SUCCESS)
    {
      break;
    }

    Status = gBS->SignalEvent(qcom_fw_device_fw_test1_event);
    if (Status != EFI_SUCCESS)
    {
      break;
    }

    // Create event for capsule signaling
    AsciiPrint("Signaling qcom_fw_device_fw_test2_event\n");
    Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                EmptyCallback,
                NULL,
                &qcom_fw_device_fw_test2_guid,
                &qcom_fw_device_fw_test2_event
                );
    if (Status != EFI_SUCCESS)
    {
      break;
    }

    Status = gBS->SignalEvent(qcom_fw_device_fw_test2_event);
    if (Status != EFI_SUCCESS)
    {
      break;
    }

  } while (0);

  TestStatus("CapsuleSignalTest", Status);
  TEST_STOP("CapsuleSignalTest");
  return Status;
}
