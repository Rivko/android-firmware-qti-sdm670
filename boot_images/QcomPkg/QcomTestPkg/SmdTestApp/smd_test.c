/** @file smem_test.c
   
  Tiny UEFI command-line test application for SMD

  Copyright (c) 2011, Qualcomm Technologies Inc. All rights reserved. 

**/

/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/21/13   bm      Test SMDLite Read/Write APIs as well.
03/04/13   bm      Update test port name and test CLOSE event as well.
02/11/13   bm      Created test application
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFISmd.h>

void SmdlCallback(smdl_handle_type port, smdl_event_type  event, void *data)
{
  /* Ignore the events for now */
  switch(event) {
    case SMDL_EVENT_OPEN:
      AsciiPrint("SMDL_EVENT_OPEN\n");
      break;
    case SMDL_EVENT_CLOSE_COMPLETE:
      AsciiPrint("SMDL_EVENT_CLOSE_COMPLETE\n");
      break;
    case SMDL_EVENT_READ:
      AsciiPrint("SMDL_EVENT_READ\n");
      break;
    case SMDL_EVENT_WRITE:
      AsciiPrint("SMDL_EVENT_WRITE\n");
      break;      
    default:
      break;
  }
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_SMD_PROTOCOL * smd_protocol;
  EFI_STATUS status;
  smdl_handle_type handle;
  int32 ret;
  uint32 bytes_written;
  int32 bytes_in_fifo, bytes_read;
  uint32 isRpmPortOpen = 0;
  int buf[256];

  TEST_START("SMD");

  status = gBS->LocateProtocol(&gEfiSMDProtocolGuid, NULL,
      (void**)&smd_protocol);
  if(status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: LocateProtocol returned %d\n", status);
    TestStatus("SMD",status);
    TEST_STOP("SMD");
    return status;
  }

  AsciiPrint("smd protocol = %p\n", smd_protocol);

  /* Wait until RPM has come up and opened its end of the port */
  while (isRpmPortOpen == 0) {
    /* wait for a sec and then check for RPM port status */
    gBS->Stall(1000000);    
    
    smd_protocol->SmdlIsPortOpen("rpm_requests",
                                 SMD_APPS_RPM,
                                 &isRpmPortOpen);
    AsciiPrint("STATUS: SmdlIsPortOpen returned %d\n", isRpmPortOpen);
  }
  /* Open Apps(UEFI)-RPM port */
  status = smd_protocol->SmdlOpen("rpm_requests", 
                                   SMD_APPS_RPM,
                                   SMDL_OPEN_FLAGS_MODE_PACKET,
                                   1024,
                                   SmdlCallback,
                                   NULL,
                                   &handle);
  if(status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: SmdlOpen returned %d\n", status);
    TestStatus("SMD",status);
    TEST_STOP("SMD");
    return status;
  }

  if(handle == NULL)
  {
    AsciiPrint("ERROR: SmdlOpen returned NULL handle \n");
    TestStatus("SMD",EFI_DEVICE_ERROR);
    TEST_STOP("SMD");
    return status;
  }

  /* wait for a sec and then close the port */
  gBS->Stall(1000000);

  /* Send test data to RPM */
  {
    smdl_iovec_type header1, header2, header3;
    unsigned  data1[2] = {0x00716572, 0x0000002C};
    unsigned  data2[5] = {0x00000001, 0x00000000, 0x74736574, 0x00000000,0x00000018};
    unsigned  data3[6] = {0x00DDBA11, 0x00000004, 0xF005BA11, 0x00FAB1E5,0x00000004,0x00FEEB1E};

    header3.next = NULL;
    header3.buffer = &data3;
    header3.length = sizeof(data3);

    header2.next = &header3;
    header2.buffer = &data2;
    header2.length = sizeof(data2);

    header1.next = &header2;
    header1.buffer = &data1;
    header1.length = sizeof(data1);

    smd_protocol->SmdlWritev(handle,  &header1, 0, &bytes_written);

    AsciiPrint("SMD: SmdlWritev returned %d \n", bytes_written);

    /* Wait for RPMs response */
    do {
      smd_protocol->SmdlRxPeek(handle, &bytes_in_fifo);
    } while(bytes_in_fifo == 0);

    do {
      /* read out the packet */
      smd_protocol->SmdlRead(handle, bytes_in_fifo, buf, 0, &bytes_read);
      AsciiPrint("SMD: SmdlRead returned %d \n", bytes_read);

      smd_protocol->SmdlRxPeek(handle, &bytes_in_fifo);
    } while(bytes_in_fifo > 0);
    
  }
  

  /* Close the port */
  status = smd_protocol->SmdlClose(handle, &ret);
  if(status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: SmdlClose returned %d\n", status);
    TestStatus("SMD",status);
    TEST_STOP("SMD");
    return status;
  }

  if(ret != SMD_STATUS_SUCCESS)
  {
    AsciiPrint("ERROR: SmdlClose returned code %d \n", ret);
    TestStatus("SMD",EFI_DEVICE_ERROR);
    TEST_STOP("SMD");
    return status;
  }

  /* wait for a sec to wait for CLOSE_COMPLETE */
  gBS->Stall(1000000);

  AsciiPrint("SMD test passed.\n");

  TestStatus("SMD",EFI_SUCCESS);
  TEST_STOP("SMD");
  return EFI_SUCCESS;
}
