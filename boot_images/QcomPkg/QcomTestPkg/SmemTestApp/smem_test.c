/** @file smem_test.c
   
  Tiny UEFI command-line test application for SMEM

  Copyright (c) 2011, Qualcomm Technologies Inc. All rights reserved. 

**/

/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/30/11   sy      Adding TestInterface Library
04/15/11   tl      Created test application
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include "Protocol/EFISmem.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_SMEM_PROTOCOL * smem_protocol;
  EFI_STATUS status;

  UINT32 * vendor0;
  UINT32 * vendor1;
  UINT32 * vendor1_b;
  UINT32 * vendor1_c;
  UINT32 size;

  TEST_START("SMEM");

  status = gBS->LocateProtocol(&gEfiSMEMProtocolGuid, NULL,
      (void**)&smem_protocol);
  if(status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: LocateProtocol returned %d\n", status);
	TestStatus("SMEM",status);
	TEST_STOP("SMEM");
    return status;
  }

  AsciiPrint("smem protocol = %p\n", smem_protocol);

  /* SMEM_ID_VENDOR0 will not be allocated */
  status = smem_protocol->SmemGetAddr(SMEM_ID_VENDOR0, &size,
      (void **)&vendor0);
  if(status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: SmemGetAddr(SMEM_ID_VENDOR0) returned %d\n", status);
	TestStatus("SMEM",status);
	TEST_STOP("SMEM");
    return status;
  }
  if(vendor0 != NULL)
  {
    AsciiPrint("ERROR: smem_get_addr(SMEM_ID_VENDOR0) != NULL\n");
	TestStatus("SMEM",EFI_DEVICE_ERROR);
	TEST_STOP("SMEM");
    return EFI_DEVICE_ERROR;
  }
  AsciiPrint("smem_get_addr(SMEM_ID_VENDOR0) = NULL\n");

  /* Allocate SMEM_ID_VENDOR1 now */
  status = smem_protocol->SmemAlloc(SMEM_ID_VENDOR1, sizeof(UINT32),
      (void **)&vendor1);
  if(status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: SmemAlloc(SMEM_ID_VENDOR1) returned %d\n", status);
	TestStatus("SMEM",status);
	TEST_STOP("SMEM");
    return status;
  }
  if(vendor1 == NULL)
  {
    AsciiPrint("ERROR: smem_alloc(SMEM_ID_VENDOR1) != NULL\n");
	TestStatus("SMEM",EFI_DEVICE_ERROR);
	TEST_STOP("SMEM");
    return EFI_DEVICE_ERROR;
  }
  AsciiPrint("smem_get_addr(SMEM_ID_VENDOR1) = %p\n", vendor1);
  /* SMEM_ID_VENDOR1 will persist between calls to this application */
  AsciiPrint("SMEM_ID_VENDOR1 = %d\n", *vendor1);
  (*vendor1)++;

  /* Make sure we get the same pointer for SMEM_ID_VENDOR1 */
  status = smem_protocol->SmemAlloc(SMEM_ID_VENDOR1, sizeof(UINT32),
      (void **)&vendor1_b);
  if(status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: SmemAlloc(SMEM_ID_VENDOR1) returned %d\n", status);
	TestStatus("SMEM",status);
	TEST_STOP("SMEM");
    return status;
  }
  if(vendor1 != vendor1_b)
  {
    AsciiPrint("ERROR: "
        "smem_alloc(SMEM_ID_VENDOR1) != smem_alloc(SMEM_ID_VENDOR1)\n");
	TestStatus("SMEM",EFI_DEVICE_ERROR);
	TEST_STOP("SMEM");
    return EFI_DEVICE_ERROR;
  }

  /* Make sure we get the same pointer for SMEM_ID_VENDOR1 */
  status = smem_protocol->SmemGetAddr(SMEM_ID_VENDOR1, &size,
      (void **)&vendor1_c);
  if(status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: SmemGetAddr(SMEM_ID_VENDOR1) returned %d\n", status);
	TestStatus("SMEM",status);
	TEST_STOP("SMEM");
    return status;
  }
  if(vendor1 != vendor1_c)
  {
    AsciiPrint("ERROR: "
        "smem_get_addr(SMEM_ID_VENDOR1) != smem_alloc(SMEM_ID_VENDOR1)\n");
	
	TestStatus("SMEM",EFI_DEVICE_ERROR);
	TEST_STOP("SMEM");
    return EFI_DEVICE_ERROR;
  }

  AsciiPrint("SMEM test passed.\n");

  TestStatus("SMEM",EFI_SUCCESS);
  TEST_STOP("SMEM");
  return EFI_SUCCESS;
}
