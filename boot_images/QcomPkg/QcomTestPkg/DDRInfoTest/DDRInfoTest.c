/** @file DDRInfoTest.c
   
  Test for UEFI DDR Info test.

  Copyright (c) 2017 Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when         who       what, where, why
 --------     ---       -----------------------------------------------------------
 04/19/2017   daisond   Initial Revision
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TestInterface.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Protocol/EFIDDRGetConfig.h>


STATIC
EFI_STATUS
DDRInfoProtocolTest (VOID)
{
  EFI_STATUS                  Status = EFI_NOT_FOUND;
  EFI_DDRGETINFO_PROTOCOL   *pDDRGetInfoProtocol = NULL;
  ddr_details_entry           *pDDREntry = NULL;
  UINT32 ddr_channel = 0, rank = 0;

  Status = gBS->LocateProtocol(&gEfiDDRGetInfoProtocolGuid, NULL, (VOID**)&pDDRGetInfoProtocol);
  if (EFI_ERROR(Status) || (&pDDRGetInfoProtocol == NULL))
  {
    DEBUG((EFI_D_ERROR, "Locate EFI_DDRGETINFO_PROTOCOL failed, Status =  (0x%x)\r\n", Status));
    return EFI_NOT_FOUND;
  }

  pDDREntry = AllocatePool (sizeof (ddr_details_entry));
  if (pDDREntry == NULL) 
      return EFI_OUT_OF_RESOURCES;

  Status = pDDRGetInfoProtocol->GetDDRDetails(pDDRGetInfoProtocol, pDDREntry);
  if (Status != EFI_SUCCESS)
  {
    if (EFI_ERROR (Status))
    {
      DEBUG((EFI_D_WARN, "Failed to get DDR details"));
    }
	return Status;
  }


  AsciiPrint("Manufacturer Id                  :");
  AsciiPrint("0x%016lx\n", pDDREntry->manufacturer_id);
  AsciiPrint("Device Type                      :");
  AsciiPrint("0x%016lx\n", pDDREntry->device_type);

  for(ddr_channel=0; ddr_channel<DDR_MAX_NUM_CH; ddr_channel++)
  {
    for(rank=0; rank < 2; rank++)
    {
	  	AsciiPrint("Revision Id1 (CH %d, Rank %d)     :", ddr_channel, rank);
		AsciiPrint("0x%016lx\n", pDDREntry->ddr_params[ddr_channel].revision_id1[rank]);

		AsciiPrint("Revision Id2 (CH %d, Rank %d)     :", ddr_channel, rank);
		AsciiPrint("0x%016lx\n", pDDREntry->ddr_params[ddr_channel].revision_id2[rank]);

		AsciiPrint("Width (CH %d, Rank %d)            :", ddr_channel, rank);
		AsciiPrint("0x%016lx\n", pDDREntry->ddr_params[ddr_channel].width[rank]);

		AsciiPrint("Density (CH %d, Rank %d)          :", ddr_channel, rank);
		AsciiPrint("0x%016lx\n", pDDREntry->ddr_params[ddr_channel].density[rank]);

    }
  } 

  return EFI_SUCCESS;
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
DDRInfoTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  TEST_START("DDRInfoTest");

  Status = DDRInfoProtocolTest();

  TestStatus("DDRInfoTest", Status);
  TEST_STOP("DDRInfoTest");
  return EFI_SUCCESS;
}
