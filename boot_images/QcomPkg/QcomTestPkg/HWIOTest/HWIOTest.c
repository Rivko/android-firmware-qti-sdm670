/** @file HWIOTest.c
   
  This file contains test HWIO test driver entry point.

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 07/30/11   sy      Adding TestInterface Library
 03/18/11   gfr     Created.

=============================================================================*/



/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFIHWIO.h>


/*=========================================================================
      Functions
==========================================================================*/

/**
  Test HWIO application region mapping/unmapping test.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

EFI_STATUS
EFIAPI
HWIOTestMap (
  IN EFI_HWIO_PROTOCOL  *HWIOProtocol,
  IN CONST CHAR8        *szBaseName
  )
{
  EFI_STATUS          eResult;
  UINT8              *nPhysAddress = NULL;
  UINT8              *nAddress = NULL;

  eResult = gBS->LocateProtocol (
                  &gEfiHwioProtocolGuid,
                  NULL,
                  (VOID **) &HWIOProtocol
                  );
  ASSERT_EFI_ERROR(eResult);

  AsciiPrint ("\n======================================\n");
  AsciiPrint ("HWIO Mapping Test                     \n");
  AsciiPrint ("========================================\n");

  TEST_START("HWIOTESTMap");
  /*
   * Map the region
   */
  eResult = HWIOProtocol->MapRegion(HWIOProtocol, szBaseName, &nAddress);

  if (eResult != EFI_SUCCESS) 
  {
    AsciiPrint ("WARNING: HWIO region \"%a\" not mapped: %d\n",
            szBaseName, eResult);
	TestStatus("HWIOTESTMap",eResult);
	TEST_STOP("HWIOTESTMap");
    return eResult;
  }

  AsciiPrint ("HWIO region \"%a\" mapped to 0x%08x\n",
          szBaseName, nAddress);

  /*
   * UnMap the region
   */
  eResult = HWIOProtocol->UnMapRegion(HWIOProtocol, nAddress);

  if (eResult != EFI_SUCCESS) 
  {
    AsciiPrint ("WARNING: HWIO region \"%a\" not unmapped!\n", szBaseName);
	TestStatus("HWIOTESTMap",eResult);
	TEST_STOP("HWIOTESTMap");
    return eResult;
  }

  AsciiPrint ("HWIO region \"%a\" unmapped successfully\n", szBaseName);

  /*
   * Map the region - use the address we got above since it will be
   * the physical address in UEFI.
   */
  nPhysAddress = nAddress;
  eResult = HWIOProtocol->MapRegionByAddress(HWIOProtocol, nPhysAddress, &nAddress);

  if (eResult != EFI_SUCCESS) 
  {
    AsciiPrint ("WARNING: HWIO physical region 0x%08x not mapped!\n", nPhysAddress);
	TestStatus("HWIOTESTMap",eResult);
	TEST_STOP("HWIOTESTMap");
    return eResult;
  }

  AsciiPrint ("HWIO region 0x%08x mapped to 0x%08x\n",
          nPhysAddress, nAddress);

  /*
   * UnMap the region
   */
  eResult = HWIOProtocol->UnMapRegion(HWIOProtocol, nAddress);

  if (eResult != EFI_SUCCESS) 
  {
    AsciiPrint ("WARNING: HWIO physical region 0x%08x not unmapped!\n",
            nPhysAddress);
	TestStatus("HWIOTESTMap",eResult);
	TEST_STOP("HWIOTESTMap");
    return eResult;
  }

  AsciiPrint ("HWIO physical region 0x%08x unmapped successfully\n",
          nPhysAddress);

  AsciiPrint ("\n");

  TestStatus("HWIOTESTMap",EFI_SUCCESS);
  TEST_STOP("HWIOTESTMap");

  return EFI_SUCCESS;

}


/**
  Test HWIO application entry point. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

EFI_STATUS
EFIAPI
HWIOTestMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          eResult;
  EFI_HWIO_PROTOCOL  *HWIOProtocol;

  eResult = gBS->LocateProtocol (
                  &gEfiHwioProtocolGuid,
                  NULL,
                  (VOID **) &HWIOProtocol
                  );
  ASSERT_EFI_ERROR(eResult);

  AsciiPrint ("\n======================================\n");
  AsciiPrint ("HWIO Application Driver               \n");
  AsciiPrint ("========================================\n");

  return HWIOTestMap(HWIOProtocol, "CLK_CTL");

}

