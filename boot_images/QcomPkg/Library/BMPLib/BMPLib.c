/** @file

  Provides support for displaying BMP image format
  Does not support compressed BMPs.

Copyright (c) 2015, 2017 Qualcomm Technologies, Inc. All rights reserved.<BR>
Portions Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <Uefi.h>

#include <IndustryStandard/Bmp.h>

#include <Library/BMPLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomUtilsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/GraphicsOutput.h>
#include <Library/GuidedFvLib.h>

EFI_GRAPHICS_OUTPUT_PROTOCOL  *mGOP              = NULL;

/**
 *
 */
EFI_STATUS EFIAPI
ConvertBmpToGopBlt (
  IN     VOID      *BmpImage,
  IN     UINTN     BmpImageSize,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
     OUT UINT32    *PixelHeight,
     OUT UINT32    *PixelWidth
  )
{
  UINT8                         *Image;
  UINT8                         *ImageHeader;
  BMP_IMAGE_HEADER              *BmpHeader;
  BMP_COLOR_MAP                 *BmpColorMap;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  UINT64                        BltBufferSize;
  UINTN                         Index;
  UINT32                        Height;
  UINT32                        Width;
  UINTN                         ImageIndex;
  UINT32                        DataSizePerLine;
  BOOLEAN                       IsAllocated;
  //UINT32                        ColorMapNum;

  if (sizeof (BMP_IMAGE_HEADER) > BmpImageSize) {
    return EFI_INVALID_PARAMETER;
  }

  BmpHeader = (BMP_IMAGE_HEADER *) BmpImage;

  if (BmpHeader->CharB != 'B' || BmpHeader->CharM != 'M') {
    return EFI_UNSUPPORTED;
  }

  //
  // Doesn't support compress.
  //
  if (BmpHeader->CompressionType != 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Only support BITMAPINFOHEADER format.
  // BITMAPFILEHEADER + BITMAPINFOHEADER = BMP_IMAGE_HEADER
  //
  if (BmpHeader->HeaderSize != sizeof (BMP_IMAGE_HEADER) - OFFSET_OF(BMP_IMAGE_HEADER, HeaderSize)) {
    return EFI_UNSUPPORTED;
  }

  //
  // The data size in each line must be 4 byte alignment.
  //
  DataSizePerLine = ((BmpHeader->PixelWidth * BmpHeader->BitPerPixel + 31) >> 3) & (~0x3);
  BltBufferSize = MultU64x32 (DataSizePerLine, BmpHeader->PixelHeight);
  if (BltBufferSize > (UINT32) ~0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((BmpHeader->Size != BmpImageSize) ||
      (BmpHeader->Size < BmpHeader->ImageOffset) ||
      (BmpHeader->Size - BmpHeader->ImageOffset <  BmpHeader->PixelHeight * DataSizePerLine)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Calculate Color Map offset in the image.
  //
  Image       = BmpImage;
  BmpColorMap = (BMP_COLOR_MAP *) (Image + sizeof (BMP_IMAGE_HEADER));
  if (BmpHeader->ImageOffset < sizeof (BMP_IMAGE_HEADER)) {
    return EFI_INVALID_PARAMETER;
  }

  if (BmpHeader->ImageOffset > sizeof (BMP_IMAGE_HEADER)) {
    //switch (BmpHeader->BitPerPixel) {
    //  case 1:
    //    ColorMapNum = 2;
    //    break;
    //  case 4:
    //    ColorMapNum = 16;
    //    break;
    //  case 8:
    //    ColorMapNum = 256;
    //    break;
    //  default:
    //    ColorMapNum = 0;
    //    break;
    //  }
    //
    // BMP file may has padding data between the bmp header section and the bmp data section.
    //
    if (BmpHeader->ImageOffset - sizeof (BMP_IMAGE_HEADER) < sizeof (BMP_COLOR_MAP) * BmpHeader->NumberOfColors) {
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Calculate graphics image data address in the image
  //
  Image         = ((UINT8 *) BmpImage) + BmpHeader->ImageOffset;
  ImageHeader   = Image;

  //
  // Calculate the BltBuffer needed size.
  //
  BltBufferSize = MultU64x32 ((UINT64) BmpHeader->PixelWidth, BmpHeader->PixelHeight);
  //
  // Ensure the BltBufferSize * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) doesn't overflow
  //
  if (BltBufferSize > DivU64x32 ((UINTN) ~0, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) {
    return EFI_UNSUPPORTED;
  }
  BltBufferSize = MultU64x32 (BltBufferSize, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

  IsAllocated   = FALSE;
  if (*GopBlt == NULL) {
    //
    // GopBlt is not allocated by caller.
    //
    *GopBltSize = (UINTN) BltBufferSize;
    *GopBlt     = AllocatePool (*GopBltSize);
    IsAllocated = TRUE;
    if (*GopBlt == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    //
    // GopBlt has been allocated by caller.
    //
    if (*GopBltSize < (UINTN) BltBufferSize) {
      *GopBltSize = (UINTN) BltBufferSize;
      return EFI_BUFFER_TOO_SMALL;
    }
  }

  *PixelWidth   = BmpHeader->PixelWidth;
  *PixelHeight  = BmpHeader->PixelHeight;

  //
  // Convert image from BMP to Blt buffer format
  //
  BltBuffer = *GopBlt;
  for (Height = 0; Height < BmpHeader->PixelHeight; Height++) {
    Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
    for (Width = 0; Width < BmpHeader->PixelWidth; Width++, Image++, Blt++) {
      switch (BmpHeader->BitPerPixel) {
      case 1:
        //
        // Convert 1-bit (2 colors) BMP to 24-bit color
        //
        for (Index = 0; Index < 8 && Width < BmpHeader->PixelWidth; Index++) {
          Blt->Red    = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Red;
          Blt->Green  = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Green;
          Blt->Blue   = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Blue;
          Blt++;
          Width++;
        }

        Blt--;
        Width--;
        break;

      case 4:
        //
        // Convert 4-bit (16 colors) BMP Palette to 24-bit color
        //
        Index       = (*Image) >> 4;
        Blt->Red    = BmpColorMap[Index].Red;
        Blt->Green  = BmpColorMap[Index].Green;
        Blt->Blue   = BmpColorMap[Index].Blue;
        if (Width < (BmpHeader->PixelWidth - 1)) {
          Blt++;
          Width++;
          Index       = (*Image) & 0x0f;
          Blt->Red    = BmpColorMap[Index].Red;
          Blt->Green  = BmpColorMap[Index].Green;
          Blt->Blue   = BmpColorMap[Index].Blue;
        }
        break;

      case 8:
        //
        // Convert 8-bit (256 colors) BMP Palette to 24-bit color
        //
        Blt->Red    = BmpColorMap[*Image].Red;
        Blt->Green  = BmpColorMap[*Image].Green;
        Blt->Blue   = BmpColorMap[*Image].Blue;
        break;

      case 24:
        //
        // It is 24-bit BMP.
        //
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image;
        break;

      default:
        //
        // Other bit format BMP is not supported.
        //
        if (IsAllocated) {
          FreePool (*GopBlt);
          *GopBlt = NULL;
        }
        return EFI_UNSUPPORTED;
        //break;
      };

    }

    ImageIndex = (UINTN) (Image - ImageHeader);
    if ((ImageIndex % 4) != 0) {
      //
      // Bmp Image starts each row on a 32-bit boundary!
      //
      Image = Image + (4 - (ImageIndex % 4));
    }
  }

  return EFI_SUCCESS;
}

static EFI_STATUS
CheckGOPAvailable(
  VOID
  )
{
  EFI_STATUS Status;

  /* Ensure Graphics Output protocol is up */
  if (mGOP == NULL) {
    Status = gBS->LocateProtocol (
      &gEfiGraphicsOutputProtocolGuid,
      NULL,
      (VOID**)&mGOP
      );
    if (EFI_ERROR(Status)) {
      mGOP = NULL;
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
 *
 */
static EFI_STATUS
BmpLibBltGopBuffer(
  IN VOID*          Blt,
  IN UINTN          BltSize,
  IN UINT32         Height,
  IN UINT32         Width,
  IN BMPLIB_OPTION_LOCATION_PARAMS    *OptLocParams
  )
{
  EFI_STATUS  Status;
  INT32       DestX = 0;
  INT32       DestY = 0;
  UINT32      SizeOfX = mGOP->Mode->Info->HorizontalResolution;
  UINT32      SizeOfY = mGOP->Mode->Info->VerticalResolution;

  /* Null location option defaults to screen center */
  BMPLIB_OPTION_LOCATION_COORD Location =
    (OptLocParams) ? OptLocParams->Location : BmplibOptionLocationCenter;

  /* Determine location */
  switch (Location) {
    case BmplibOptionLocationTopLeft:
      DestX = 0;
      DestY = 0;
      break;

    case BmplibOptionLocationTopCenter:
      DestX = (SizeOfX - Width) / 2;
      DestY = 0;
      break;

    case BmplibOptionLocationTopRight:
      DestX = (SizeOfX - Width);
      DestY = 0;
      break;

    case BmplibOptionLocationCenterLeft:
      DestX = 0;
      DestY = (SizeOfY - Height) / 2;
      break;

    case BmplibOptionLocationCenter:
      DestX = (SizeOfX - Width) / 2;
      DestY = (SizeOfY - Height) / 2;
      break;

    case BmplibOptionLocationCenterRight:
      DestX = (SizeOfX - Width);
      DestY = (SizeOfY - Height) / 2;
      break;

    case BmplibOptionLocationBottomLeft:
      DestX = 0;
      DestY = (SizeOfY - Height);
      break;

    case BmplibOptionLocationBottomCenter:
      DestX = (SizeOfX - Width) / 2;
      DestY = (SizeOfY - Height);
      break;

    case BmplibOptionLocationBottomRight:
      DestX = (SizeOfX - Width);
      DestY = (SizeOfY - Height);
      break;

    case BmplibOptionLocationCustom:
      DestX = OptLocParams->DestX;
      DestY = OptLocParams->DestY;
      break;

    default:
      return EFI_INVALID_PARAMETER;
  }

  /* Blitting partial images not supported - abort */
  if (DestX < 0 || DestY < 0) {
    return EFI_ABORTED;
  }

  /* Blit to screen */
  Status = mGOP->Blt (
                    mGOP,
                    Blt,
                    EfiBltBufferToVideo,
                    0,
                    0,
                    (UINTN) DestX,
                    (UINTN) DestY,
                    Width,
                    Height,
                    Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                    );

  return Status;
}

/**
 * Default: Clear Screen
 */
static EFI_STATUS
BmpLibClearScreen(
  BMPLIB_OPTION_CLEARSCREEN_PARAMS  *OptClsParams
  )
{
  /* Skip clearing screen if user requested this option */
  if (OptClsParams && OptClsParams->ClearScreen == FALSE) {
    return EFI_SUCCESS;
  }

  /* Clear screen */
  return gST->ConOut->Reset(gST->ConOut, FALSE);
}

/**
 *
 */
static EFI_STATUS
BmpLibGetOption(
  IN     BMPLIB_OPTION       Opts[],
  IN     UINTN               NumOpts,
  IN     BMPLIB_OPTION_TYPE  BmplibOptionType,
     OUT VOID                **OptParams
  )
{
  INTN Index = 0;

  /* Sanity check */
  if ((OptParams == NULL) ||
      (Opts && NumOpts == 0) ||
      (!Opts && NumOpts > 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  /* A NULL option implies a request to
     use default settings */
  if (Opts == NULL) {
    *OptParams = NULL;
    return EFI_SUCCESS;
  }

  /* Check if option is specified */
  Index = 0;
  do {
    if (Opts[Index].Type == BmplibOptionType) {
      break;
    }
    Index++;
  } while (Index < NumOpts);

  /* Option not specified */
  if (Index == NumOpts) {
    *OptParams = NULL;
    return EFI_SUCCESS;
  }

  /* Option was specified, but no parameters were specified.
     This is likely a user error. */
  if (Opts[Index].Params == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  /* Validate the parameter based on option type */
  switch(Opts[Index].Type) {

    /* Validate Location option */
    case BmplibOptionTypeLocation: {
      BMPLIB_OPTION_LOCATION_PARAMS *Params = Opts[Index].Params;
      if (Params->Location > BmplibOptionLocationMax) {
        return EFI_INVALID_PARAMETER;
      }
      break;
    }

    /* Non-null cls params is always valid */
    case BmplibOptionTypeClearScreen:
      break;

    default:
      return EFI_INVALID_PARAMETER;
  }

  /* Valid parameters */
  *OptParams = Opts[Index].Params;
  return EFI_SUCCESS;
}

/**
 * See header file
 */
EFI_STATUS EFIAPI
DrawGopBltBuffer(
  IN   VOID            *GopBlt,
  IN   UINTN           BltSize,
  IN   UINTN           Height,
  IN   UINTN           Width,
  IN   BMPLIB_OPTION   Opts[]         OPTIONAL,
  IN   UINTN           NumOpts
  )
{
  EFI_STATUS                        Status;
  BMPLIB_OPTION_CLEARSCREEN_PARAMS  *OptClsParams = NULL;
  BMPLIB_OPTION_LOCATION_PARAMS     *OptLocParams = NULL;

  /* Sanitize input parameters */
  if (GopBlt == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  /* Ensure Graphics Output protocol is up */
  Status = CheckGOPAvailable();
  if (EFI_ERROR(Status)) {
    return Status;
  }

  /* Validate and Get Option: ClearScreen */
  Status = BmpLibGetOption(Opts, NumOpts, BmplibOptionTypeClearScreen, (VOID**)&OptClsParams);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  /* Validate and Get Option: Location */
  Status = BmpLibGetOption(Opts, NumOpts, BmplibOptionTypeLocation, (VOID**)&OptLocParams);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  /* Clear screen unless overriden */
  Status = BmpLibClearScreen(OptClsParams);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  /* Blit GOP buffer as specified */
  Status = BmpLibBltGopBuffer(
              GopBlt,
              BltSize,
              Height,
              Width,
              OptLocParams
              );
  return Status;
}

/**
 * See header file
 */
EFI_STATUS EFIAPI
DrawBmpBuffer (
  UINT8                    *FileBuffer,
  UINTN                    FileSize,
  IN   BMPLIB_OPTION       Opts[]         OPTIONAL,
  IN   UINTN               NumOpts
  )
{
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt         = NULL;
  UINTN                         BltSize;
  UINT32                        Height;
  UINT32                        Width;

  /* Sanitize input parameters */
  if (FileBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  /* Ensure Graphics Output protocol is up */
  Status = CheckGOPAvailable();
  if (EFI_ERROR(Status)) {
    return Status;
  }

  /* Convert bitmap to GOP blit safe format */
  Status = ConvertBmpToGopBlt (
              FileBuffer,
              FileSize,
              (VOID **) &Blt,
              &BltSize,
              &Height,
              &Width
              );
  if (EFI_ERROR(Status)) {
    goto ExitHandler;
  }

  Status = DrawGopBltBuffer(
              Blt,
              BltSize,
              Height,
              Width,
              Opts,
              NumOpts
              );

ExitHandler:
  if (Blt) {
    FreePool(Blt);
    Blt = NULL;
  }
  return Status;
}

/**
 * See header file
 */
EFI_STATUS EFIAPI
DrawBmpFile (
  IN   CHAR8               *FileName,
  IN   BMPLIB_OPTION       Opts[]         OPTIONAL,
  IN   UINTN               NumOpts
  )
{
  EFI_STATUS                    Status;
  UINTN                         FileSize     = 0;
  UINT8                         *FileBuffer  = NULL;

  /* Sanitize input parameters */
  if (FileName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  /* Ensure Graphics Output protocol is up */
  Status = CheckGOPAvailable();
  if (EFI_ERROR(Status)) {
    return Status;
  }

  /* Load file into buffer */
  Status = LoadFromFV(FileName, &FileBuffer, &FileSize);
  if(EFI_ERROR (Status) || FileBuffer == NULL) {
    goto ExitHandler;
  }

  Status = DrawBmpBuffer (FileBuffer, FileSize, Opts, NumOpts);
  
ExitHandler:
  if (FileBuffer) {
    FreePool(FileBuffer);
    FileBuffer = NULL;
  }
  
  return Status;
}
