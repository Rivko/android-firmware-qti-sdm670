/** @file FontDxe.c

   This file implements NPA EFI protocol interface.

   Copyright (c) 2013-2015, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
   reserved.

 **/

#include <Uefi.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

#include <QCFonts.h>
#include "StdFont.h"


#define ARRAY_SIZE(a)           (sizeof((a))/sizeof((a)[0]))
#define NEAREST_BYTE(a)         (((a) + 7)/8)
#define MAX_FONT_NAME_LENGTH    20
#define MAX_SCALE_FACTOR        8


typedef union {
  VOID   *Data;
  UINT8 (*x2)[2];
  UINT8 (*x3)[3];
  UINT8 (*x4)[4];
  UINT8 (*x5)[5];
  UINT8 (*x6)[6];
  UINT8 (*x7)[7];
  UINT8 (*x8)[8];
} LookupTable;

typedef struct {
  CHAR16            StartUnicodeWeight;  /* Starting character code in block */
  UINT32            NumberOfGlyphs;      /* Total number of glyphs */
} FontSection;

typedef struct {
  CHAR16            *FontName;          /* Font family */
  UINT16            FontWidth;          /* Width in px */
  UINT16            FontHeight;         /* Height in px */ 
  UINT16            ScaleFactor;        /* Scaling Factor */         
  UINT8             *BitmapData;        /* Bitmap data */
  UINT32            SizeInBytes;        /* Total size of bitmap data */ 
  EFI_GUID          FontGuid;
  UINT16            NumSections;        /* Number of discontiguous sections */  
  FontSection       Section[3];         /* For fonts with missing glyphs */
} FontPkg;


/**
 * 
 */
static EFI_STATUS
GenerateLookupTable (
  LookupTable                 *Table,
  UINT16                      ScaleFactor
  )
{
  UINT64 Result = 0;
  UINT8* ResultByte = (UINT8 *)&Result;
  UINT32 Byte = 0, LoopIndex = 0;
  INT16 Bit = 0;
  UINT16 Value = 0;

  if (Table == NULL || Table->Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (Byte = 0; Byte <= MAX_UINT8; Byte++) {
    Result = 0;    
    for (Bit = 7; Bit >= 0; Bit--) {      
      Value = ((1 << Bit) & Byte) >> Bit;
      for (LoopIndex = 0; LoopIndex < ScaleFactor; LoopIndex++) {
        Result <<= 1;
        Result |= Value;        
      }
    }    

    for (LoopIndex = 0; LoopIndex < ScaleFactor; LoopIndex++) {
      switch (ScaleFactor) {
      case 2:
        Table->x2[Byte][LoopIndex] = ResultByte[ScaleFactor - LoopIndex - 1];
        break;
      case 3:
        Table->x3[Byte][LoopIndex] = ResultByte[ScaleFactor - LoopIndex - 1];
        break;
      case 4:
        Table->x4[Byte][LoopIndex] = ResultByte[ScaleFactor - LoopIndex - 1];
        break;
      case 5:
        Table->x5[Byte][LoopIndex] = ResultByte[ScaleFactor - LoopIndex - 1];
        break;
      case 6:
        Table->x6[Byte][LoopIndex] = ResultByte[ScaleFactor - LoopIndex - 1];
        break;
      case 7:
        Table->x7[Byte][LoopIndex] = ResultByte[ScaleFactor - LoopIndex - 1];
        break;  
      case 8:
        Table->x8[Byte][LoopIndex] = ResultByte[ScaleFactor - LoopIndex - 1];
        break;   
      default:
        ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);     
      }
    }
  }

  return EFI_SUCCESS;
}


/**
 * Scales glyps one section at a time
 */
static EFI_STATUS
ScaleGlyphs (
  IN FontPkg                  *Font,
  IN UINT32                   NumberOfGlyphs,
  IN UINT8                    *BitmapData,
  IN UINT8                    *ScaledBitmapData
  )
{
  EFI_STATUS Status;
  UINT32  GlyphIndex = 0, HeightIndex = 0, WidthIndex = 0, Index = 0;
  UINT8   *CurrentPtr     = NULL;
  UINT32  Size  = 0;  
  LookupTable Table;
 
  if (Font == NULL || BitmapData == NULL || ScaledBitmapData == NULL ) {
    return EFI_INVALID_PARAMETER;
  }

  Table.Data = AllocateZeroPool((MAX_UINT8 + 1) * Font->ScaleFactor * sizeof(UINT8));  
  if (Table.Data == NULL) {
    DEBUG((EFI_D_ERROR, "Out of memory !\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Status = GenerateLookupTable(&Table, Font->ScaleFactor);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  /* Scale all glyphs in section */
  for (GlyphIndex = 0; GlyphIndex < NumberOfGlyphs; GlyphIndex++) {
    /* For each row of pixels in a glyph */
    for (HeightIndex = 0; HeightIndex < Font->FontHeight; HeightIndex++) {
      CurrentPtr = ScaledBitmapData;
      /* Scale the Width of each glyph */
      for (WidthIndex = 0; WidthIndex < NEAREST_BYTE(Font->FontWidth); WidthIndex++) {
        /* Use lookup table for scaling */
        for (Index = 0; Index < Font->ScaleFactor; Index++) {
          switch (Font->ScaleFactor) {
          case 2:
            *ScaledBitmapData = Table.x2[*BitmapData][Index];
            break;           
          case 3:
            *ScaledBitmapData = Table.x3[*BitmapData][Index];
            break;
          case 4:
            *ScaledBitmapData = Table.x4[*BitmapData][Index];
            break;
          case 5:
            *ScaledBitmapData = Table.x5[*BitmapData][Index];
            break;
          case 6:
            *ScaledBitmapData = Table.x6[*BitmapData][Index];
            break;
          case 7:
            *ScaledBitmapData = Table.x7[*BitmapData][Index];
            break;
          case 8:
            *ScaledBitmapData = Table.x8[*BitmapData][Index];
            break;
          default:
            ASSERT_EFI_ERROR(EFI_INVALID_PARAMETER);
          }
          ScaledBitmapData++;
        }
        BitmapData++;
      }

      /* Scale the row of each glyph */
      Size = ScaledBitmapData - CurrentPtr;
      for (Index = 1; Index < Font->ScaleFactor; Index++) {
        CopyMem(ScaledBitmapData, CurrentPtr, Size);
        ScaledBitmapData += Size;
      }
    }  
  }

  /* Free lookup table */
  if (Table.Data) {
    FreePool(Table.Data);
    Table.Data = NULL;
  }

  return EFI_SUCCESS;

}


/**
 *
 */
static EFI_STATUS
FillGlyphData (
  IN UINT8                     *Package,
  IN FontPkg                   *Font
  )
{
  EFI_STATUS                          Status = EFI_LOAD_ERROR;
  EFI_HII_FONT_PACKAGE_HDR            *FontPkgHdr       = NULL;
  EFI_HII_GIBT_SKIP2_BLOCK            *Skip2Block       = NULL;
  EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK   *GlyphsBlock      = NULL;
  EFI_GLYPH_GIBT_END_BLOCK            *EndBlock         = NULL;
  UINT8                               *CurrentPtr       = NULL;
  UINT8                               *BitmapData       = NULL;
  UINT8                               *ScaledBitmapData = NULL;
  UINT32                              Index             = 0;
  UINT32                              GlyphBlocksSize   = 0;

  FontPkgHdr = (EFI_HII_FONT_PACKAGE_HDR *)((UINTN)Package + sizeof(UINT32));

  CurrentPtr = (UINT8*) ((UINTN)Package + sizeof(UINT32) + FontPkgHdr->GlyphBlockOffset);
  
  /* Skip unused glyphs until the first section */
  Skip2Block = (EFI_HII_GIBT_SKIP2_BLOCK *)CurrentPtr;
  Skip2Block->Header.BlockType = EFI_HII_GIBT_SKIP2;
  Skip2Block->SkipCount = Font->Section[0].StartUnicodeWeight - 1;
  CurrentPtr += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);

  /* Add glyphs for each section */
  BitmapData = Font->BitmapData;
  for (Index=0; Index<Font->NumSections; Index++) {
    GlyphsBlock = (EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK *)CurrentPtr;
    GlyphsBlock->Header.BlockType = EFI_HII_GIBT_GLYPHS_DEFAULT;
    GlyphsBlock->Count = Font->Section[Index].NumberOfGlyphs;

    /* Scale the glyphs if needed */
    if (Font->ScaleFactor > 1) {
      GlyphBlocksSize = Font->Section[Index].NumberOfGlyphs 
        * NEAREST_BYTE(Font->FontWidth * Font->ScaleFactor)
        * (Font->FontHeight * Font->ScaleFactor); 

      ScaledBitmapData = AllocateZeroPool (GlyphBlocksSize);

      Status = ScaleGlyphs(Font, Font->Section[Index].NumberOfGlyphs, BitmapData, ScaledBitmapData);
      if (EFI_ERROR(Status)) {        
        goto ExitHandler;
      }

      CopyMem(GlyphsBlock->BitmapData, ScaledBitmapData, GlyphBlocksSize);
      FreePool(ScaledBitmapData);
      ScaledBitmapData = NULL;
    }
    else {
      GlyphBlocksSize = Font->Section[Index].NumberOfGlyphs 
        * NEAREST_BYTE(Font->FontWidth) * Font->FontHeight; 

      CopyMem(GlyphsBlock->BitmapData, BitmapData, GlyphBlocksSize);
    }

    CurrentPtr += sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) 
          - sizeof(((EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK *)0)->BitmapData);
    CurrentPtr += GlyphBlocksSize;

    BitmapData += Font->Section[Index].NumberOfGlyphs 
      * NEAREST_BYTE(Font->FontWidth) * Font->FontHeight; 

    /* If any additional sections, skip unused glyphs until the next section */
    if (Index+1 != Font->NumSections) {
      Skip2Block = (EFI_HII_GIBT_SKIP2_BLOCK *)CurrentPtr;
      Skip2Block->Header.BlockType = EFI_HII_GIBT_SKIP2;
      Skip2Block->SkipCount = Font->Section[Index+1].StartUnicodeWeight 
          - (Font->Section[Index].StartUnicodeWeight + Font->Section[Index].NumberOfGlyphs);
  
      CurrentPtr += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
    }
  }

  /* Sanity check */
  ASSERT (BitmapData - Font->BitmapData == Font->SizeInBytes);
  
  EndBlock = (EFI_GLYPH_GIBT_END_BLOCK *)CurrentPtr;
  EndBlock->Header.BlockType = EFI_HII_GIBT_END;

  Status = EFI_SUCCESS;

ExitHandler:
  if (ScaledBitmapData) {
    FreePool(ScaledBitmapData);
    ScaledBitmapData = NULL;
  }
  return Status;
}


/**
 *
 */
static EFI_STATUS
CreateFontPackage (
  IN      FontPkg                   *Font,
     OUT  UINT8                     **Package
  )
{
  EFI_HII_FONT_PACKAGE_HDR    *FontPkgHdr = NULL;
  UINT32                      Length = 0;
  UINT32                      FontNameSize = 0;
  UINT32                      GlyphBlocksSize = 0;
  UINT32                      Index = 0;
  EFI_HII_GLYPH_INFO          GlyphInfo;

  if (Package == NULL || Font == NULL) {
    DEBUG((EFI_D_ERROR, "Invalid param to CreateFontPackage() !\n"));
    return EFI_INVALID_PARAMETER;
  }
  
  if (Font->ScaleFactor > MAX_SCALE_FACTOR) {
    DEBUG((EFI_D_ERROR, "Invalid ScaleFactor !\n"));
    return EFI_INVALID_PARAMETER;
  }

  GlyphInfo.Width    = Font->FontWidth * Font->ScaleFactor;
  GlyphInfo.Height   = Font->FontHeight * Font->ScaleFactor;
  GlyphInfo.OffsetX  = 0;
  GlyphInfo.OffsetY  = 0;
  GlyphInfo.AdvanceX = (INT16)(Font->FontWidth * Font->ScaleFactor);


  FontNameSize = (sizeof(CHAR16)*(StrnLenS(Font->FontName, MAX_FONT_NAME_LENGTH)+1));
  
  Length = sizeof(UINT32) +
           sizeof(EFI_HII_FONT_PACKAGE_HDR) +
           FontNameSize +
           sizeof(EFI_HII_GIBT_SKIP2_BLOCK);       
           
  for (Index=0; Index<Font->NumSections; Index++) {
    GlyphBlocksSize = Font->Section[Index].NumberOfGlyphs 
      * NEAREST_BYTE(Font->FontWidth * Font->ScaleFactor)
      * (Font->FontHeight * Font->ScaleFactor);  

    Length += sizeof(EFI_HII_GIBT_GLYPHS_DEFAULT_BLOCK) + GlyphBlocksSize;

    if (Index+1 != Font->NumSections) {
      Length += sizeof(EFI_HII_GIBT_SKIP2_BLOCK);
    }
  }

  Length += sizeof(EFI_GLYPH_GIBT_END_BLOCK);
           
  *Package = (UINT8*)AllocateZeroPool(Length);
  if (*Package == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // BinaryLength
  WriteUnaligned32((UINT32*)*Package, Length);

  // PackageHeader
  FontPkgHdr                   = (EFI_HII_FONT_PACKAGE_HDR *)((UINTN)(*Package) + sizeof(UINT32));
  FontPkgHdr->Header.Length    = Length - sizeof(UINT32);
  FontPkgHdr->Header.Type      = EFI_HII_PACKAGE_FONTS;
  FontPkgHdr->HdrSize          = sizeof(EFI_HII_FONT_PACKAGE_HDR) + FontNameSize;
  FontPkgHdr->GlyphBlockOffset = sizeof(EFI_HII_FONT_PACKAGE_HDR) + FontNameSize;
  FontPkgHdr->Cell             = GlyphInfo;
  FontPkgHdr->FontStyle        = EFI_HII_FONT_STYLE_NORMAL;
  CopyMem(FontPkgHdr->FontFamily, Font->FontName, FontNameSize);

  return EFI_SUCCESS;
}

/**
 *
 */
static EFI_STATUS
InstallFontPkgs(
  IN  FontPkg*  Fonts,
  IN  UINTN     Size
  )
  {
  EFI_STATUS                  Status        = EFI_LOAD_ERROR;
  EFI_HII_HANDLE              *FontHandles  = NULL;
  UINT8                       *Package      = NULL;
  EFI_HANDLE                  FontHiiHandle = NULL;
  UINT32                      Index;

  if (Size == 0 || Fonts == NULL) {
    DEBUG((EFI_D_ERROR, "Invalid parameters passed to InstallFontPkgs() !\n"));
    return EFI_INVALID_PARAMETER;
  }

  for (Index=0; Index<Size; Index++) {

    /* Skip font if it is already installed */
    FontHandles = HiiGetHiiHandles(&Fonts[Index].FontGuid);
    if (FontHandles) {
      DEBUG((EFI_D_WARN, "Font (%s) already installed!\n", Fonts[Index].FontName));
      FreePool(FontHandles);
      FontHandles = NULL;
      Status = EFI_SUCCESS;
      continue;
    }

    /* Setup Font package header */
    Status = CreateFontPackage(&Fonts[Index], &Package);
    if (EFI_ERROR(Status)) {
      goto ExitHandler;
    }

    /* Add font glyphs to package */
    Status = FillGlyphData(Package, &Fonts[Index]);
    if (EFI_ERROR(Status)) {
      goto ExitHandler;
    }

    Status = EFI_UNSUPPORTED;
    /* Add Font pkg */
    FontHiiHandle = HiiAddPackages (
                &Fonts[Index].FontGuid,
                NULL,
                Package,
                NULL
                );
    if (FontHiiHandle == NULL) {
      goto ExitHandler;
    }

    Status = EFI_SUCCESS;
    if (Package) {
      FreePool(Package);
      Package = NULL;
    }
  }

ExitHandler:
  if (FontHandles) {
    FreePool(FontHandles);
    FontHandles = NULL;
  }
  if (Package) {
    FreePool(Package);
    Package = NULL;
  }
  return Status;
}

/**
 *
 */
EFI_STATUS EFIAPI
FontDriverInitialize(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *EFI_SYSTEM_TABLE
  )
{
  EFI_STATUS        Status;

  /**
   * List of Fonts to be installed
   */
  FontPkg qcFonts[] = {
    {
      .FontName           = SYSFONT_2x, 
      .FontWidth          = 8,
      .FontHeight         = 19,
      .ScaleFactor        = 2,
      .BitmapData         = gStdNarrowFontData,
      .SizeInBytes        = sizeof(gStdNarrowFontData),
      .FontGuid           = {0x46c1220e, 0xbe05, 0x4375, {0x8b, 0xe6, 0x23, 0x96, 0xae, 0x7b, 0x42, 0x0a}},
      .NumSections        = 1,
      .Section            = {
        {
          .StartUnicodeWeight = 0x0020,
          .NumberOfGlyphs     = 95,
        },
        { 0,0 },
        { 0,0 }}
    },
    {
      .FontName           = SYSFONT_3x, 
      .FontWidth          = 8,
      .FontHeight         = 19,
      .ScaleFactor        = 3,
      .BitmapData         = gStdNarrowFontData,
      .SizeInBytes        = sizeof(gStdNarrowFontData),
      .FontGuid           = {0xca5765a7, 0x4aa4, 0x43a6, {0x9c, 0xd0, 0x08, 0x28, 0xb6, 0x14, 0x89, 0xef}},
      .NumSections        = 1,
      .Section            = {
        {
          .StartUnicodeWeight = 0x0020,
          .NumberOfGlyphs     = 95,
        },
        { 0,0 },
        { 0,0 }}
    },
    {
      .FontName           = SYSFONT_4x, 
      .FontWidth          = 8,
      .FontHeight         = 19,
      .ScaleFactor        = 4,
      .BitmapData         = gStdNarrowFontData,
      .SizeInBytes        = sizeof(gStdNarrowFontData),
      .FontGuid           = {0xe3a1d89b, 0xf255, 0x416c, {0x96, 0x20, 0x9b, 0x9a, 0xa3, 0xd9, 0xf0, 0x36}},
      .NumSections        = 1,
      .Section            = {
        {
          .StartUnicodeWeight = 0x0020,
          .NumberOfGlyphs     = 95,
        },
        { 0,0 },
        { 0,0 }}
    },
    {
      .FontName           = SYSFONT_5x, 
      .FontWidth          = 8,
      .FontHeight         = 19,
      .ScaleFactor        = 5,
      .BitmapData         = gStdNarrowFontData,
      .SizeInBytes        = sizeof(gStdNarrowFontData),
      .FontGuid           = {0xaadc6fa6, 0xbe52, 0x4433, {0x8a, 0x76, 0xad, 0xd4, 0xb1, 0x04, 0x21, 0x38}},
      .NumSections        = 1,
      .Section            = {
        {
          .StartUnicodeWeight = 0x0020,
          .NumberOfGlyphs     = 95,
        },
        { 0,0 },
        { 0,0 }}
    },
    {
      .FontName           = SYSFONT_6x, 
      .FontWidth          = 8,
      .FontHeight         = 19,
      .ScaleFactor        = 6,
      .BitmapData         = gStdNarrowFontData,
      .SizeInBytes        = sizeof(gStdNarrowFontData),
      .FontGuid           = {0x3a198127, 0x8460, 0x4566, {0xb6, 0x04, 0x51, 0x11, 0x1a, 0x46, 0x9d, 0x57}},
      .NumSections        = 1,
      .Section            = {
        {
          .StartUnicodeWeight = 0x0020,
          .NumberOfGlyphs     = 95,
        },
        { 0,0 },
        { 0,0 }}
    },
    {
      .FontName           = SYSFONT_7x, 
      .FontWidth          = 8,
      .FontHeight         = 19,
      .ScaleFactor        = 7,
      .BitmapData         = gStdNarrowFontData,
      .SizeInBytes        = sizeof(gStdNarrowFontData),
      .FontGuid           = {0x308b06ce, 0x7104, 0x48c4, {0xa5, 0x7d, 0x87, 0x19, 0x37, 0x1f, 0x77, 0x55}},
      .NumSections        = 1,
      .Section            = {
        {
          .StartUnicodeWeight = 0x0020,
          .NumberOfGlyphs     = 95,
        },
        { 0,0 },
        { 0,0 }}
    },
    {
      .FontName           = SYSFONT_8x, 
      .FontWidth          = 8,
      .FontHeight         = 19,
      .ScaleFactor        = 8,
      .BitmapData         = gStdNarrowFontData,
      .SizeInBytes        = sizeof(gStdNarrowFontData),
      .FontGuid           = {0xb1461c5e,0xa3cd,0x4cfa, {0x8e, 0xe1, 0x5e, 0x08, 0x04, 0xda, 0x90, 0xf4}},
      .NumSections        = 1,
      .Section            = {
        {
          .StartUnicodeWeight = 0x0020,
          .NumberOfGlyphs     = 95,
        },
        { 0,0 },
        { 0,0 }}
    },
  };

  if (gHiiDatabase == NULL) {
    DEBUG((EFI_D_ERROR, "gHiiDatabase not found!\n"));
    return EFI_LOAD_ERROR;
  }

  Status = InstallFontPkgs(qcFonts, ARRAY_SIZE(qcFonts));
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to install fonts!\n"));
    goto ExitHandler;
  }

ExitHandler:
  return Status;
}