/** @file BMPTest.c

  Run through list of BMPs

  Copyright (c) 2015, 2017 Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/08/17   bd      Support for authenticated FV load testing
 09/09/15   ai      Initial version


=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BMPLib.h>
#include <Library/GuidedFvLib.h>

#define  ARRAY_SIZE(a) (sizeof((a))/sizeof((a)[0]))

CHAR8* FilePaths[] = {
   "fv1:\\battery_symbol_NoBattery.bmp",
   "fv1:\\battery_symbol_Nocharger.bmp",
   "fv1:\\battery_symbol_Soc10.bmp",
   "fv1:\\battery_symbol_Soc25.bmp",
   "fv1:\\battery_symbol_Soc50.bmp",
   "fv1:\\battery_symbol_Soc75.bmp",
   "fv1:\\battery_symbol_Soc100.bmp",
   "fv1:\\battery_symbol_LowBatteryCharging.bmp"
};

/**
 *  Make non-const duplicate of strings.
 *  Caller must FreePool the new string.
 */
static CHAR8 * AsciiStrnDup(
  CONST CHAR8               *Source,
  UINTN                     Length
)
{
  CHAR8 *Dest = NULL;

  if(Source == NULL || Length == 0) {
    return NULL;
  }

  Dest = AllocatePool (Length + 1);
  if (Dest == NULL) {
    return NULL;
  }

  AsciiStrnCpy(Dest, Source, Length + 1);

  return Dest;
}

/**
 *
 */
EFI_STATUS
BmpTestNoOptions()
{
  UINTN           Index        = 0;
  UINTN           NumFiles     = ARRAY_SIZE(FilePaths);
  EFI_STATUS      Status;
  CHAR8           *FileName    = NULL;
  UINTN           FileSize     = 0;
  UINT8           *FileBuffer  = NULL;   

  TEST_START("BmpTest-NoOptions");

  Status = LoadAndAuthenticateFV(&gImageFvGuid, L"ImageFv");
  if(EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "LoadAndAuthenticateFV failed %x\n", Status));
  }
  else {
    DEBUG ((EFI_D_INFO, "LoadAndAuthenticateFV successfull %x\n", Status));
  }
  
  do {
    /* Make non-const copy of FileName string */
    FileName = AsciiStrnDup(
      FilePaths[Index],
      AsciiStrLen(FilePaths[Index])
      );
    if (!FileName) {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }

    /* Draw bitmap */
    Status =  LoadFileFromGuidedFv (NULL, FileName, &FileBuffer, &FileSize);         
    if (EFI_ERROR(Status) || (FileBuffer == NULL))
    {
      DEBUG((EFI_D_ERROR, "ChargerLib:: %a LoadFileFromGuidedFv() returned error = %r\n\r", __FUNCTION__,Status));
    }
    else
    {
      Status = DrawBmpBuffer(FileBuffer, FileSize, NULL, 0);
      if (EFI_ERROR(Status))
      {
        DEBUG((EFI_D_ERROR, "ChargerLib:: %a DrawBmpBuffer() returned error = %r\n\r", __FUNCTION__,Status));
      }
    }

    /* Stall for 1.5s */
    gBS->Stall(1500000);

    /* Free resources */
    FreePool(FileName);
    if (FileBuffer != NULL) {
      FreePool(FileBuffer);
    }
    FileName   = NULL;
  } while (Index++ < NumFiles-1);


  if (FileName != NULL) {
    FreePool(FileName);
  }

  TestStatus("BmpTest-NoOptions", Status);
  TEST_STOP("BmpTest-NoOptions");
  return Status;
}

/**
 *
 */
EFI_STATUS
BmpTestLocation()
{
  EFI_STATUS                    Status;
  CHAR8                         *FileName  = NULL;
  UINTN                         Index      = 0;
  BMPLIB_OPTION_LOCATION_COORD  Location[] = {
    BmplibOptionLocationTopLeft,
    BmplibOptionLocationTopCenter,
    BmplibOptionLocationTopRight,
    BmplibOptionLocationCenterLeft,
    BmplibOptionLocationCenter,
    BmplibOptionLocationCenterRight,
    BmplibOptionLocationBottomLeft,
    BmplibOptionLocationBottomCenter,
    BmplibOptionLocationBottomRight,
    BmplibOptionLocationCustom,
  };
  UINTN                         NumLocations = ARRAY_SIZE(Location);

  BMPLIB_OPTION_LOCATION_PARAMS Params = {BmplibOptionLocationCustom, 200, 200};
  BMPLIB_OPTION                 Options[1];

  Options[0].Type = BmplibOptionTypeLocation;
  Options[0].Params = &Params;

  TEST_START("BmpTest-Location");

  FileName = AsciiStrnDup(
                FilePaths[0],
                AsciiStrLen(FilePaths[0])
                );
  if (!FileName) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ExitHandler;
  }

  Index = 0;
  do {
    Params.Location = Location[Index];

    /* Draw bitmap */
    Status = DrawBmpFile(FileName, Options, ARRAY_SIZE(Options));
    if(EFI_ERROR (Status)) {
      break;
    }

    /* Stall for 1.5s */
    gBS->Stall(1500000);
  } while (Index++ < NumLocations-1);

/* Legal for GCC; Illegal for RVCT */
#if 0
  /* Test invalid Location */
  Params.Location = 999;

  /* Draw bitmap */
  Status = DrawBmpFile(FileName, Options, ARRAY_SIZE(Options));
  if(!EFI_ERROR (Status)) {
    Status = EFI_ABORTED;
  }
  else {
    Status = EFI_SUCCESS;
  }
#endif

ExitHandler:
  if (FileName != NULL) {
    FreePool(FileName);
  }

  TestStatus("BmpTest-Location", Status);
  TEST_STOP("BmpTest-Location");
  return Status;
}

/**
 *
 */
EFI_STATUS
BmpTestClearScreen()
{
  EFI_STATUS                    Status;
  CHAR8                         *FileName  = NULL;
  UINTN                         Index      = 0;
  BMPLIB_OPTION_LOCATION_COORD  Location[] = {
    BmplibOptionLocationTopLeft,
    BmplibOptionLocationTopRight,
    BmplibOptionLocationCenter,
    BmplibOptionLocationBottomLeft,
    BmplibOptionLocationBottomRight,
    BmplibOptionLocationCustom,
  };
  UINTN                         NumLocations = ARRAY_SIZE(Location);

  BMPLIB_OPTION_LOCATION_PARAMS     LocationParams = {BmplibOptionLocationCustom, 200, 200};
  BMPLIB_OPTION_CLEARSCREEN_PARAMS  ScreenParams   = {FALSE};

  BMPLIB_OPTION                 Options[2];

  Options[0].Type = BmplibOptionTypeLocation;
  Options[0].Params = &LocationParams;

  Options[1].Type = BmplibOptionTypeClearScreen;
  Options[1].Params = &ScreenParams;

  TEST_START("BmpTest-ClearScreen");

  FileName = AsciiStrnDup(
                FilePaths[0],
                AsciiStrLen(FilePaths[0])
                );
  if (!FileName) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ExitHandler;
  }

  Index = 0;
  do {
    LocationParams.Location = Location[Index];

    /* Draw bitmap */
    Status = DrawBmpFile(FileName, Options, ARRAY_SIZE(Options));
    if(EFI_ERROR (Status)) {
      break;
    }

    /* Stall for 1.5s */
    gBS->Stall(1500000);
  } while (Index++ < NumLocations-1);

ExitHandler:
  if (FileName != NULL) {
    FreePool(FileName);
  }

  TestStatus("BmpTest-ClearScreen", Status);
  TEST_STOP("BmpTest-ClearScreen");
  return Status;
}

/**
 *
 */
typedef EFI_STATUS (* BMPTESTCASE)(VOID);
BMPTESTCASE BmpTests[] = {
  BmpTestNoOptions,
  BmpTestLocation,
  BmpTestClearScreen
};

/**
 *
 */
EFI_STATUS
EFIAPI
BMPTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  UINTN      NumTests = ARRAY_SIZE(BmpTests);
  UINTN      Index    = 0;

  do {
    Status = BmpTests[Index]();
    if(EFI_ERROR (Status)) {
      break;
    }
  } while (Index++ < NumTests-1);

  return Status;
}


