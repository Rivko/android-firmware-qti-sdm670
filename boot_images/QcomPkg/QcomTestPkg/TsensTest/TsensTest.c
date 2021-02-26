/*============================================================================
  FILE:         TsensTest.c

  OVERVIEW:     Exercises TSENS EFI.

  DEPENDENCIES: None

                Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/21/14   jjo     Updated for XBL.
 01/31/12   jjo     Added a sanity check on the temperature.
 10/08/12   jjo     Initial version.

=============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/EFITsens.h>
#include <Library/TestInterface.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define TSENS_TEST_TEMP_MIN 200  // deci deg C
#define TSENS_TEST_TEMP_MAX 700  // deci deg C

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
TsensTestMain(
   IN EFI_HANDLE ImageHandle,
   IN EFI_SYSTEM_TABLE *SystemTable
   )
{
   EFI_STATUS status = EFI_SUCCESS;
   EFI_TSENS_PROTOCOL *Tsens = NULL;
   UINT32 uNumSensors;
   UINT32 uSensor;
   INT32 nTempDeciDegC;

   TEST_START("TsensTest");
   status = gBS->LocateProtocol(&gEfiTsensProtocolGuid,
                                NULL,
                                (VOID**)&Tsens);
   if (status != EFI_SUCCESS)
   {
      AsciiPrint("ERROR: TsensTestMain: Tsens protocol load failed\n");
      TestStatus("TsensTest", status);
      TEST_STOP("TsensTest");
      return status;
   }

   AsciiPrint("TsensTestMain: Tsens protocol loaded\n");

   /* Get the number of sensors */
   status = Tsens->GetNumSensors(&uNumSensors);
   if (status != EFI_SUCCESS || uNumSensors == 0)
   {
      AsciiPrint("ERROR: TsensTestMain: Tsens protocol GetNumChannels failed\n");
      TestStatus("TsensTest", status);
      TEST_STOP("TsensTest");
      return status;
   }

   /* Read the temperature of each sensor */
   for (uSensor = 0; uSensor < uNumSensors; uSensor++)
   {
      status = Tsens->GetTemp(uSensor, &nTempDeciDegC);
      if (status != EFI_SUCCESS)
      {
         if (status == EFI_UNSUPPORTED)
         {
            AsciiPrint("ERROR: TsensTestMain: Tsens sensor 0x%x is dead\n", uSensor);
         }
         else
         {
            AsciiPrint("ERROR: TsensTestMain: Tsens protocol GetTemp failed for sensor 0x%x\n", uSensor);
            TestStatus("TsensTest", status);
            TEST_STOP("TsensTest");
            return status;
         }
      }

      AsciiPrint("TsensTestMain: Tsens sensor 0x%x temp = %d deci deg C\n", uSensor, nTempDeciDegC);

      if (nTempDeciDegC < TSENS_TEST_TEMP_MIN)
      {
         status = EFI_UNSUPPORTED;
         AsciiPrint("ERROR: TsensTestMain: Tsens sensor 0x%x temp %d deci deg C < min temp %d deci deg C\n",
                    uSensor,
                    nTempDeciDegC,
                    TSENS_TEST_TEMP_MIN);
         TestStatus("TsensTest", status);
         TEST_STOP("TsensTest");
         return status;
      }

      if (nTempDeciDegC > TSENS_TEST_TEMP_MAX)
      {
         status = EFI_UNSUPPORTED;
         AsciiPrint("ERROR: TsensTestMain: Tsens sensor 0x%x temp %d deci deg C > max temp %d deci deg C\n",
                    uSensor,
                    nTempDeciDegC,
                    TSENS_TEST_TEMP_MAX);
         TestStatus("TsensTest", status);
         TEST_STOP("TsensTest");
         return status;
      }
   }

   /* Get the maximum temperature */
   status = Tsens->GetMaxTemp(&nTempDeciDegC);
   if (status != EFI_SUCCESS)
   {
      AsciiPrint("ERROR: TsensTestMain: Tsens protocol GetMaxTemp failed\n");
      TestStatus("TsensTest", status);
      TEST_STOP("TsensTest");
      return status;
   }

   AsciiPrint("TsensTestMain: Tsens max temp = %d deci deg C\n", nTempDeciDegC);

   if (nTempDeciDegC < TSENS_TEST_TEMP_MIN)
   {
      status = EFI_UNSUPPORTED;
      AsciiPrint("ERROR: TsensTestMain: Tsens max temp %d deci deg C < min temp %d deci deg C\n",
                 nTempDeciDegC,
                 TSENS_TEST_TEMP_MIN);
      TestStatus("TsensTest", status);
      TEST_STOP("TsensTest");
      return status;
   }

   if (nTempDeciDegC > TSENS_TEST_TEMP_MAX)
   {
      status = EFI_UNSUPPORTED;
      AsciiPrint("ERROR: TsensTestMain: Tsens max temp %d deci deg C > max temp %d deci deg C\n",
                 nTempDeciDegC,
                 TSENS_TEST_TEMP_MAX);
      TestStatus("TsensTest", status);
      TEST_STOP("TsensTest");
      return status;
   }

   TestStatus("TsensTest", status);
   TEST_STOP("TsensTest");

   return EFI_SUCCESS;
}

