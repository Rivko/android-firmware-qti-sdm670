/** @file FontTest.c 

  Demo various scaled fonts 

  Copyright (c) 2017 Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------

 3/28/17    pawans  GraphicsOutputProtocol not found ASSERT ISSUE
 
=============================================================================*/


#include <Uefi.h>
#include <QCFonts.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TestInterface.h>
#include <Library/UefiHiiServicesLib.h>

#include <Protocol/EFIRng.h>
#include <Protocol/GraphicsOutput.h>


#define ARRAY_SIZE(a)           (sizeof((a))/sizeof((a)[0]))
#define NEAREST_BYTE(a)         (((a) + 7)/8)
#define MAX_FONT_NAME_LENGTH    20
#define MAX_SCALE_FACTOR        8


EFI_GRAPHICS_OUTPUT_PROTOCOL        *GraphicsOutputProtocol = NULL;
EFI_QCOM_RNG_PROTOCOL               *RngProtocol = NULL;



static EFI_STATUS
TestFont (
  CHAR16        *FontName,
  UINT32        BltY,
  UINTN         *Height
  )
{
  EFI_STATUS Status;

  EFI_FONT_DISPLAY_INFO               *FontDisplayInfo = NULL;
  UINT32                              HorizontalResolution = 0;
  UINT32                              VerticalResolution = 0;
  UINT32                              Index = 0;
  EFI_IMAGE_OUTPUT                    *BltBuffer = NULL;
  EFI_HII_ROW_INFO                    *RowInfoArray = NULL;
  UINTN                               RowInfoArraySize;
  EFI_HANDLE                          ConsoleHandle = (EFI_HANDLE)NULL;
  UINT8                               Colors[6] = {0};
  UINT8                               RNGValue;
  CHAR16                              StringBuffer[] = L"the quick brown fox jumps over the lazy dog";
  /* CHAR16 chinese[] ={ 0x4e16, 0x754c, 0x60a8, 0x597d, 0x0000 }; */
  /* CHAR16 japanese[]={ 0x4eca, 0x65e5, 0x306f, 0x4e16, 0x754c, 0x0000 }; */

  if (GraphicsOutputProtocol == NULL) {
    ConsoleHandle = gST->ConsoleOutHandle;
    ASSERT( ConsoleHandle != NULL);

    Status = gBS->HandleProtocol (
                    ConsoleHandle,
                    &gEfiGraphicsOutputProtocolGuid,
                    (VOID **) &GraphicsOutputProtocol
                    );
  
    if(GraphicsOutputProtocol == NULL){
      AsciiPrint("GraphicsOutputProtocol not found\n");
      return Status;
    }
  }

  if (RngProtocol == NULL) {
    Status = gBS->LocateProtocol (
                  &gQcomRngProtocolGuid,
                  NULL,
                  (VOID **) &RngProtocol
                  );
  }
  if (RngProtocol) {
    for (Index=0; Index<6; Index++) {
      Status = RngProtocol->GetRNG(RngProtocol, NULL, sizeof(RNGValue), &RNGValue);
      ASSERT_EFI_ERROR(Status);
      Colors[Index] = RNGValue;
    }
  }
  else {
    /* Set Foreground to White */
    Colors[0] = Colors[1] = Colors[2] = 0xFF;
  }

  HorizontalResolution = GraphicsOutputProtocol->Mode->Info->HorizontalResolution;
  VerticalResolution = GraphicsOutputProtocol->Mode->Info->VerticalResolution;
  /* If using Bitmap mode, set VerticalResolution as needed
     VerticalResolution = 100;
  */
  ASSERT ((HorizontalResolution != 0) && (VerticalResolution !=0));

  BltBuffer = (EFI_IMAGE_OUTPUT *) AllocateZeroPool (sizeof (EFI_IMAGE_OUTPUT));
  if (BltBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  BltBuffer->Width        = (UINT16) (HorizontalResolution);
  BltBuffer->Height       = (UINT16) (VerticalResolution);
  BltBuffer->Image.Screen = GraphicsOutputProtocol;
  /* If using Bitmap mode, allocate memory instead
     BltBuffer->Image.Bitmap = AllocateZeroPool (BltBuffer->Width * BltBuffer->Height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  */
  
  FontDisplayInfo = (EFI_FONT_DISPLAY_INFO *) AllocateZeroPool(
    sizeof (EFI_FONT_DISPLAY_INFO) + 100);
  if (FontDisplayInfo == NULL) {
    return EFI_OUT_OF_RESOURCES; 
  }

  FontDisplayInfo->ForegroundColor.Blue = Colors[0];
  FontDisplayInfo->ForegroundColor.Green = Colors[1];
  FontDisplayInfo->ForegroundColor.Red = Colors[2];
  FontDisplayInfo->BackgroundColor.Blue = Colors[3];
  FontDisplayInfo->BackgroundColor.Green = Colors[4];
  FontDisplayInfo->BackgroundColor.Red = Colors[5];
  FontDisplayInfo->FontInfoMask = EFI_FONT_INFO_ANY_SIZE | EFI_FONT_INFO_ANY_STYLE;
  CopyMem(&FontDisplayInfo->FontInfo.FontName, FontName, StrSize(FontName));


  Status = gHiiFont->StringToImage (
                         gHiiFont,
                         /* Set to 0 for Bitmap mode */
                         EFI_HII_DIRECT_TO_SCREEN | EFI_HII_OUT_FLAG_WRAP,
                         StringBuffer,
                         FontDisplayInfo,
                         &BltBuffer,
                         0,      /* BltX */
                         BltY,   /* BltY */
                         &RowInfoArray,
                         &RowInfoArraySize,
                         NULL
                         );
  ASSERT_EFI_ERROR (Status);

  if (Height) {
    *Height = RowInfoArraySize * RowInfoArray[0].LineHeight;
  }

  if (BltBuffer == NULL)
    return EFI_NOT_READY;

  /* For Bitmap mode, use EfiBltBufferToVideo, and set DestX,DestY as needed */
  GraphicsOutputProtocol->Blt(
    GraphicsOutputProtocol,
    BltBuffer->Image.Bitmap,
    EfiBltVideoToVideo,
    0,    /* SrcX */
    0,    /* SrcY */
    0,    /* DestX */
    0,    /* DestY */
    BltBuffer->Width,
    BltBuffer->Height,
    BltBuffer->Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
    );

  if (BltBuffer) {
    FreePool(BltBuffer);
    BltBuffer = NULL;
  }

  return EFI_SUCCESS;
}


/**
 *
 */
EFI_STATUS EFIAPI
FontTestInitialize(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *EFI_SYSTEM_TABLE
  )
{
  EFI_STATUS        Status;
  UINTN             Location     = 125;
  UINTN             Height       = 0;
  UINTN             Index        = 0;
  CHAR16            *TestFonts[] = {
    SYSFONT_2x,
    SYSFONT_3x,
    SYSFONT_4x,
    SYSFONT_5x,
    SYSFONT_6x,
    SYSFONT_7x,
    SYSFONT_8x,
  };
  CHAR8            AsciiFontName[32];

  for (Index = 0; Index < ARRAY_SIZE(TestFonts); Index++) {
    UnicodeStrToAsciiStr(TestFonts[Index], AsciiFontName); 

    TEST_START(AsciiFontName);
 
    Status = TestFont(TestFonts[Index], Location, &Height);
    TestStatus(AsciiFontName, Status);
 
    TEST_STOP(AsciiFontName);    

    Location += Height + 10;
  }

  return Status;
}
