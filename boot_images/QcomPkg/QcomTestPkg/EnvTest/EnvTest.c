/** @file EnvTest.c
   
  Test for UEFI Env test.

  Copyright (c) 2015-2016 Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/02/16   vk      KW fix
 10/12/15   vk      Initial Revision
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

#include <Protocol/EFIRamPartition.h>


STATIC
EFI_STATUS
RamPartProtocolTest (VOID)
{
  EFI_STATUS                  Status = EFI_NOT_FOUND;
  EFI_RAMPARTITION_PROTOCOL   *pRamPartProtocol = NULL;
  RamPartitionEntry           *pRamPartitions = NULL;
  UINT32                      NumPartitions = 0;
  UINT32                      i = 0; 

  Status = gBS->LocateProtocol(&gEfiRamPartitionProtocolGuid, NULL, (VOID**)&pRamPartProtocol);
  if (EFI_ERROR(Status) || (&pRamPartProtocol == NULL))
  {
    DEBUG((EFI_D_ERROR, "Locate EFI_RAMPARTITION_Protocol failed, Status =  (0x%x)\r\n", Status));
    return EFI_NOT_FOUND;
  }

  Status = pRamPartProtocol->GetRamPartitions (pRamPartProtocol, NULL, &NumPartitions);
  if (Status == EFI_BUFFER_TOO_SMALL)
  {
    pRamPartitions = AllocatePool (NumPartitions * sizeof (RamPartitionEntry));
    if (pRamPartitions == NULL) 
      return EFI_OUT_OF_RESOURCES;

    Status = pRamPartProtocol->GetRamPartitions (pRamPartProtocol, pRamPartitions, &NumPartitions);
    if (EFI_ERROR (Status) || (NumPartitions < 1) )
    {
      DEBUG((EFI_D_ERROR, "Failed to get RAM partitions"));
      return EFI_NOT_FOUND;
    }
  }

  DEBUG ((EFI_D_WARN, "\n------------------\n"));
  DEBUG ((EFI_D_WARN, "+ RAM Partitions +\n"));
  DEBUG ((EFI_D_WARN, "------------------\n"));
  DEBUG((EFI_D_WARN, "Base \t\t\t Available Length\n"));

 if (pRamPartitions == NULL)
   return EFI_NOT_FOUND;

  for (i = 0; i < NumPartitions; i++)
  {
    DEBUG((EFI_D_WARN, "0x%016lx \t 0x%016lx\n", pRamPartitions[i].Base, pRamPartitions[i].AvailableLength));
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
EnvTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  TEST_START("EnvTest");

  Status = RamPartProtocolTest();

  TestStatus("EnvTest", Status);
  TEST_STOP("EnvTest");
  return EFI_SUCCESS;
}
