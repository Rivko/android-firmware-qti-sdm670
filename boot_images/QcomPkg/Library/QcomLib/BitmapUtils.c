/** @file BitmapUtils.c

  Contains generic Qualcomm bitmap library functions.

  Copyright (c) 2016, 2018-2019 Qualcomm Technologies Incorporated.
  All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/31/16   bh      General cleanup, SOL timestamp, and move to guided FV impl
 07/05/16   bh      Update file system call after API change
 03/07/16   ml      BGRT loading from FV, two call BGRT processing

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BootConfig.h>
#include <IndustryStandard/Acpi50.h>
#include <IndustryStandard/Bmp.h>
#include <Library/QcomBaseLib.h>
#include <Library/GuidedFvLib.h>
#include <Library/UefiCfgLib.h>


/* External references */

// Physical Partition Guids
extern EFI_GUID gEfiEmmcGppPartition1Guid;
extern EFI_GUID gEfiEmmcUserPartitionGuid;
extern EFI_GUID gEfiUfsLU4Guid;

// Partition Type
extern EFI_GUID gEfiPlatPartitionTypeGuid;


/* Local function prototype headers */
/* CalculateLogoPosition() - Calculate the location of the logo image within the screen
 */
static VOID CalculateLogoPosition(
    IN UINT32   LogoWidth, 
    IN UINT32   LogoHeight, 
    OUT UINT32 *pPosX, 
    OUT UINT32 *pPosY);

/* ConversionPixelCopy() - Pixel copy with fomat conversion
 */
static VOID ConversionPixelCopy(
    IN UINT32     SourceBitDepth,
    IN VOID      *ColorMap,
    IN UINT8     *SourceBuffer,
    IN UINT32     DestBitDepth,
    IN UINT8     *DestBuffer,
    IN UINT32     NumPixels);

/* LoadBitmapImage() - Load a bitmap image from the filesystem
 */
static
EFI_STATUS
LoadBitmapImage(
    IN CHAR8 *FileName,
    IN OUT UINT8 **Buffer,
    IN OUT UINT32 *BufferSize,
    OUT UINT32 *ImageSize);


/* Public Functions */

/**
  RenderSplashScreen - Load, process and display OEM logo bitmaps including processing needed to
                 support ACPI2.0 BGRT.

   - RenderSplashScreen is called with NULL which will render the logo (found in FV or HLOS partitions).
   - RenderSplashScreen is called with valid BGRT will load the 2nd logo and update the BGRT table.  

  @param  BGRTTable   Pointer to the BGRT table in ACPI, if NULL the primary logo is rendered with no BGRT information returned.

  @retval EFI_SUCCESS             Image loaded to BGRT location and to the screen.
  @retval EFI_INVALID_PARAMETER   Invalid input parameters passed in
  @retval EFI_UNSUPPORTED         Invalid image or image format

**/
EFI_STATUS
EFIAPI
RenderSplashScreen(
  OUT VOID *BGRTTable OPTIONAL)
{
  EFI_STATUS                                        Status          = EFI_SUCCESS;
  static EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE *pLocalBGRTTable = NULL;
  
  // Handle cases where the BGRT is not yet loaded or not applicable
  if(NULL == BGRTTable)
  {
     // Allocate space for the local cached copy of the BGRT table if it doesn't exist
     if (NULL == pLocalBGRTTable)
     {
        if (NULL == (pLocalBGRTTable  = (EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE*) AllocateZeroPool(sizeof(EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE))))
        {
            DEBUG ((EFI_D_ERROR, "RenderSplashScreen: Unable to allocate memory for local BGRT table!\n"));
            Status = EFI_OUT_OF_RESOURCES;
        }
     }

     // Load and render the logo1.bmp, cache the BGRT information till a valid BGRT is passed in.
     if (EFI_SUCCESS == Status)
     {
       UINT32        ImageSize      = 0;
       UINT8        *BGRTAddress    = NULL;
       UINT32        BGRTBufferSize = 0;     

       // Load logo1, from plat partition or the main FV
       Status = LoadBitmapImage("logo1.bmp", &BGRTAddress, &BGRTBufferSize, &ImageSize);

       if (EFI_SUCCESS == Status)
       {
         // Render logo1.bmp after loading it.
         if (EFI_SUCCESS == (Status = RenderBGRTImage(BGRTAddress, ImageSize)))
         {
           BMP_IMAGE_HEADER *ImageHeader = (BMP_IMAGE_HEADER*)BGRTAddress;
           UINT32            ImageWidth  = ImageHeader->PixelWidth;
           UINT32            ImageHeight = ((INT32)ImageHeader->PixelHeight < 0) ? ((UINT32)- ImageHeader->PixelHeight):ImageHeader->PixelHeight;

           DEBUG ((EFI_D_INFO, "RenderSplashScreen: OEM Logo1 Successfully Loaded\n"));
         
           // Update the BGRT position, store it in the cached version of the BGRT
           CalculateLogoPosition(ImageWidth, ImageHeight, &pLocalBGRTTable->ImageOffsetX, &pLocalBGRTTable->ImageOffsetY);
           
           // As logo1.bmp is rendered to the panel, set the status field to 1.
           pLocalBGRTTable->Status = 0x1;
         }      
       } 
     }
  }
  else  // Valid BGRT passed in, validate is and process it.
  {
    EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE  *pBGRTTable = (EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE *)BGRTTable;  
    
    if ((EFI_ACPI_5_0_BOOT_GRAPHICS_RESOURCE_TABLE_SIGNATURE != pBGRTTable->Header.Signature) || // "BGRT" Signature
        (EFI_ACPI_5_0_BGRT_VERSION                           != pBGRTTable->Version)          || // Valid version 1.0
        (EFI_ACPI_5_0_BGRT_IMAGE_TYPE_BMP                    != pBGRTTable->ImageType))          // Valid type BMP
    {
      DEBUG ((EFI_D_ERROR, "RenderSplashScreen: BGRT table header corrupt or invalid!\n"));
      Status = EFI_LOAD_ERROR;
    }
    else
    {
      UINT32        ImageSize      = 0;
      UINT8        *BGRTAddress    = NULL;
      UINT32        BGRTBufferSize = 0;
  
      if (EFI_SUCCESS == LoadBitmapImage("logo2.bmp", &BGRTAddress, &BGRTBufferSize, &ImageSize))
      {
        BMP_IMAGE_HEADER *ImageHeader    = (BMP_IMAGE_HEADER*)BGRTAddress;
        UINT32            ImageWidth     = ImageHeader->PixelWidth;
        UINT32            ImageHeight    = ((INT32)ImageHeader->PixelHeight < 0) ? ((UINT32)- ImageHeader->PixelHeight):ImageHeader->PixelHeight;
        
        DEBUG ((EFI_D_INFO, "RenderSplashScreen: OEM Logo2 Successfully Loaded\n"));
   
        // Update the BGRT position
        CalculateLogoPosition(ImageWidth, ImageHeight, &pBGRTTable->ImageOffsetX, &pBGRTTable->ImageOffsetY);
  
        // As logo2.bmp is loaded into the BGRT address, set the status to 0 to give an indication to OS to render this image.
        pBGRTTable->Status = 0x0;
      }
      else
      {
        // logo2 not found, pass information loaded during logo1 back to the caller's BGRT table
        if (NULL != pLocalBGRTTable)
        {
          pBGRTTable->ImageOffsetX = pLocalBGRTTable->ImageOffsetX;
          pBGRTTable->ImageOffsetY = pLocalBGRTTable->ImageOffsetY;
          pBGRTTable->Status       = pLocalBGRTTable->Status;
          pBGRTTable->ImageAddress = pLocalBGRTTable->ImageAddress;
        }
        else
        {
          DEBUG ((EFI_D_ERROR, "RenderSplashScreen: No Logo was not loaded correctly.  BGRT cannot be updated!\n"));
        }
      }
    }
  }
  
  DEBUG((EFI_D_ERROR, "Render Splash  [%5d]\n", GetTimerCountms() ));

  return Status;
}


/**
  RenderBGRTImage - Render an image on to the screen from the BGRT buffer

  @param[in]  BGRTHeader     BGRT Header block from ACPI
  @param[in]  BGRTImage      BGRT Source image buffer
  @param[in]  BGRTImageSize  BGRT image buffer size

  @retval EFI_SUCCESS            Image loaded successfully in to memory.
  @retval EFI_INVALID_PARAMETER  Invalid input parameters passed in
  @retval EFI_OUT_OF_RESOURCES   Not enough resources for buffer allocations

**/
EFI_STATUS
RenderBGRTImage(
    IN UINT8 *BGRTImage,
    IN UINT32 BGRTImageSize)
{

  EFI_STATUS                     Status         = EFI_SUCCESS;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput = NULL;
  BMP_IMAGE_HEADER              *ImageHeader    = (BMP_IMAGE_HEADER*)BGRTImage;
  EFI_HANDLE                    *GraphicsHandles;
  UINTN                          GraphicsHandleCount;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  bgPixel = {0};

  //
  // Try to open GOP(Graphics output protocol) for blt operations
  //
  Status = gBS->LocateHandleBuffer (ByProtocol,
                             &gEfiGraphicsOutputProtocolGuid,
                             NULL,
                             &GraphicsHandleCount,
                             &GraphicsHandles);

  if (EFI_ERROR (Status) || (GraphicsHandles == NULL) || (GraphicsHandleCount != 1))
  {
    DEBUG((EFI_D_ERROR, "Cannot find GraphicsHandles.\r\n"));
    return Status;
  }

  Status = gBS->HandleProtocol (GraphicsHandles[0], &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);

  // Validate all inputs
  if ((EFI_SUCCESS != Status) ||
      (NULL == GraphicsOutput) ||
      (0 == GraphicsOutput->Mode->Info->HorizontalResolution) ||
      (0 == GraphicsOutput->Mode->Info->VerticalResolution))
  {
      // GOP protocol failed
  }
  else if ((NULL == ImageHeader) ||
           (0 == BGRTImageSize) ||
           (NULL == BGRTImage) ||
           (BGRTImageSize < sizeof(BMP_IMAGE_HEADER)))
  {
      // Invalid input parameters
      Status = EFI_INVALID_PARAMETER;
  }
  else if (('B' != ImageHeader->CharB) ||
           ('M' != ImageHeader->CharM) ||
           (0 != ImageHeader->CompressionType))
  {
      // Unsupported image format
      Status = EFI_UNSUPPORTED;

  }
   else if ((32 != ImageHeader->BitPerPixel) &&
            (24 != ImageHeader->BitPerPixel) &&
            (8  != ImageHeader->BitPerPixel))
   {
      // Unsupported pixel format
      Status = EFI_UNSUPPORTED;
   }
   else if (((ImageHeader->PixelWidth * ImageHeader->PixelHeight * ImageHeader->BitPerPixel) / 8) > (BGRTImageSize - sizeof(BMP_IMAGE_HEADER)))
   {
     // Resolution cannot be larger than the file itself minus the header
     Status = EFI_UNSUPPORTED;
   }
   else
   {
      UINT32         ImageHeight;
      UINT32         ImageWidth;
      UINT32         ImageStride;
      UINT8         *SrcAddress;
      INT32          SrcStride;
      UINT32         DestStride;
      UINT32        *ConversionBuffer;
      BMP_COLOR_MAP *ColorMap        = (BMP_COLOR_MAP *) (BGRTImage + sizeof (BMP_IMAGE_HEADER));

      //
      // This section of code handles conversion from the native BMP format to a GOP compatible format, this includes
      // - Conversion from 24bpp to 32bpp
      // - Handling an inverted image
      // - Handling BMP stride alignment of 4 bytes.
      //
      // Potential optimization to perform conversion using MDPBlt() API to use hardware to perform this conversion and blt
      // directly to the frame buffer.
      //
      //

      // Populate the image width and height
      ImageWidth = ImageHeader->PixelWidth;
      // Image source is 24bpp, but is always aligned to a dword (4 bytes)
      ImageStride = (ImageWidth * (ImageHeader->BitPerPixel/8));
      // Align stride on a dword boundary
      ImageStride = ((ImageStride)+3) & ~3;

      // Populate starting address, height and stride information, the source image could be stored
      // upside down, need to handle the case of an inverted image
      if ((INT32)ImageHeader->PixelHeight < 0)
      {
        // Image height is negative, indicating the image is in raster order, setup the pointers so we can
        // copy the image directly
        ImageHeight = (UINT32)- ImageHeader->PixelHeight;
        // Calculate the starting address which starts after the header
        SrcAddress = (UINT8*)((UINTN)BGRTImage + (UINT32)ImageHeader->ImageOffset);

        // Source stride is incremented by the image stride directly
        SrcStride = ImageStride;
      }
      else
      {
        // by default BMP images are stored upside down, setup the buffers to copy in the reverse direction
        ImageHeight = (UINT32)ImageHeader->PixelHeight;
        // Image source is 24bpp, but is always aligned to a dword (4 bytes)
        // Source image starts on the last line and we copy from the last line to the top
        SrcAddress =  (UINT8*)((UINTN)BGRTImage + (UINT32)ImageHeader->ImageOffset + (UINT32)((ImageHeight-1)*ImageStride));
        // Reverse stride since we are subtracting from the bottom
        SrcStride = -ImageStride;
      }

      // Calcualte the destination output stride.
      DestStride  = ImageWidth * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);

      // Allocate a temporary buffer that is compatible with the GOP protocol
      ConversionBuffer = AllocatePool(ImageHeight * DestStride);

      if (NULL == ConversionBuffer)
      {
        // Cannot allocate a conversion buffer
        Status = EFI_OUT_OF_RESOURCES;
      }
      else
      {
        UINT32 Count;
        //
        // If the OEM supports two logos (logo1.bmp & logo2.bmp), they should use the BGRT table to override the offsets for logo2.bmp.
        //
        UINT32 LogoPosX     = 0;
        UINT32 LogoPosY     = 0;
        UINT8 *DestAddress  = (UINT8*)ConversionBuffer;

        CalculateLogoPosition(ImageWidth, ImageHeight, &LogoPosX, &LogoPosY);

        // Copy line by line to handle inverted images and images that have padding (due to odd widths)
        //
        for (Count=0;Count<ImageHeight;Count++)
        {
          // Perform version, all pixel depths are in bits, so multiply byte size by 8
          ConversionPixelCopy(ImageHeader->BitPerPixel, 
                              ColorMap,
                              SrcAddress, 
                              (sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)*8), 
                              DestAddress, 
                              ImageWidth);

          // Increment the source and destination pointers by their respective strides.
          DestAddress += DestStride;
          SrcAddress += SrcStride;
        }

        //
        // Setup blt of conversion buffer to the screen
        //
        if (EFI_SUCCESS != (Status = GraphicsOutput->Blt(GraphicsOutput,
                                                        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)&bgPixel,
                                                        EfiBltVideoFill,
                                                        0, 0,
                                                        0, 0,
                                                        ImageWidth,
                                                        ImageHeight,
                                                        0)))
        {
          DEBUG((EFI_D_ERROR, "ChargerDXE - Blt(EfiBltVideoFill) failed.\r\n"));
          return Status;
        }

        Status = GraphicsOutput->Blt (GraphicsOutput,
                                      (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)ConversionBuffer,
                                      EfiBltBufferToVideo,
                                      0,
                                      0,
                                      (UINTN) LogoPosX,
                                      (UINTN) LogoPosY,
                                      ImageWidth,
                                      ImageHeight,
                                      ImageWidth * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

        //
        // Free the conversion buffer
        //
        FreePool(ConversionBuffer);
      }
  }

  return Status;
}


/**
  ConversionPixelCopy - Convert a source 24bpp image to 32bpp with a dynamically allocated buffer

  @param[in]  SourceBitDepth  Source image bit depth
  @param[in]  ColorMap        Pointer to BMP indexed color map table
  @param[in]  SourceBuffer    Source image buffer
  @param[in]  DestBitDepth    Destination image bit depth
  @param[in]  DestBuffer      Destination image buffer
  @param[in]  NumPixels       Number of pixels to process

  @retval     None

**/
static
VOID
ConversionPixelCopy(
    IN UINT32     SourceBitDepth,
    IN VOID      *ColorMap,
    IN UINT8     *SourceBuffer,
    IN UINT32     DestBitDepth,
    IN UINT8     *DestBuffer,
    IN UINT32     NumPixels)
{

  // Check for valid input parameters
  if (SourceBitDepth == DestBitDepth)
  {
    CopyMem(DestBuffer, SourceBuffer, NumPixels*(SourceBitDepth/8));
  }
  else if ((24 == SourceBitDepth) && (32 == DestBitDepth))
  {
    UINT32  Count;
    UINT32 *Buffer32BPP = (UINT32*)DestBuffer;
    UINT8  *Buffer24BPP = (UINT8*)SourceBuffer;

    for (Count=0;Count<NumPixels;Count++)
    {
       Buffer32BPP[Count] = (UINT32) (0x00000000 | (Buffer24BPP[2]<<16) | (Buffer24BPP[1]<<8) | (Buffer24BPP[0]));

       // Increment to the next pixel
       Buffer24BPP+=3;
    }
  }
  else if ((8 == SourceBitDepth) && (32 == DestBitDepth))
  {
    UINT32  Count;
    UINT32 *Buffer32BPP = (UINT32*)DestBuffer;
    UINT8  *Buffer8BPP  = (UINT8*)SourceBuffer;
  
    for (Count=0;Count<NumPixels;Count++)
    {
       UINT8          uIndex = *Buffer8BPP;     
       BMP_COLOR_MAP *pColor = (BMP_COLOR_MAP*)ColorMap;
       
       Buffer32BPP[Count] = (UINT32) (0x00000000 | (pColor[uIndex].Red<<16) | (pColor[uIndex].Green<<8) | (pColor[uIndex].Blue));

       // Increment to the next pixel
       Buffer8BPP+=1;
    }
  }
  else
  {
    // All other conversion are not supported, fill with an solid color (gray)
    UINT32  Count;
    UINT32 *Buffer8BPP = (UINT32*)DestBuffer;

    for (Count=0;Count<NumPixels*(DestBitDepth/8);Count++)
    {
        Buffer8BPP[Count] = 0x5A;
    }
  }
}




/**
  CalculateLogoPosition - Calculate the x,y position of the logo based on logo size, screen size and placement rules.

  @param  LogoWidth   Logo width in pixels
  @param  LogoHeight  Logo height in pixels
  @param  pPosX       Horizontal placement of logo top left corder
  @param  pPoxY       Vertical placement of logo top left corder

  @retval none

**/
static 
VOID 
CalculateLogoPosition(
    IN UINT32   LogoWidth, 
    IN UINT32   LogoHeight, 
    OUT UINT32 *pPosX, 
    OUT UINT32 *pPosY)
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput = NULL;    
  EFI_HANDLE                    *GraphicsHandles;
  UINTN                          GraphicsHandleCount;

  Status = gBS->LocateHandleBuffer (ByProtocol,
                             &gEfiGraphicsOutputProtocolGuid,
                             NULL,
                             &GraphicsHandleCount,
                             &GraphicsHandles);

  if (EFI_ERROR (Status) || (GraphicsHandles == NULL) || (GraphicsHandleCount != 1))
  {
    DEBUG((EFI_D_ERROR, "Cannot find GraphicsHandles.\r\n"));
    return;
  }

  Status = gBS->HandleProtocol (GraphicsHandles[0], 
                                &gEfiGraphicsOutputProtocolGuid,
                                (VOID **) &GraphicsOutput);

  if ((EFI_SUCCESS != Status) ||
      (NULL == pPosX) ||
      (NULL == pPosY) ||
      (0    == GraphicsOutput->Mode->Info->HorizontalResolution) ||
      (0    == GraphicsOutput->Mode->Info->VerticalResolution))
  {
    // Parameters invalid, just return
  }
  else
  {
      UINT32  PanelWidth     = GraphicsOutput->Mode->Info->HorizontalResolution;
      UINT32  PanelHeight    = GraphicsOutput->Mode->Info->VerticalResolution;
      INT32   LogoPosX       = 0;
      INT32   LogoPosY       = 0;
    
      // Calculate LogoPosX and LogoPosY based on platform panel resolution.
      // Center of BGRT image should be at the horizontal center of the panel.
      if (PanelWidth > LogoWidth)
      {
        LogoPosX = ((PanelWidth - LogoWidth)/2);
      }
      
      // Center of BGRT image should be at 38.2 percent of panel height from the top.
      if (PanelHeight > LogoHeight)
      {
        LogoPosY = (0.382*PanelHeight) - (UINT32)(LogoHeight/2);
      }
    
      // If LogoPosX and LogoPosY does not have valid values, set them to (0,0)
      if ((LogoPosX < 0) ||
          (LogoPosY < 0) ||
          ((UINT32)LogoPosX > (PanelWidth-1)) ||
          ((UINT32)LogoPosY > (PanelHeight-1)))
      {
        LogoPosX = 0;
        LogoPosY = 0;
      }
    
      // Update the image offsets
      *pPosX = (UINT32)LogoPosX;
      *pPosY = (UINT32)LogoPosY;
  }
}



/**
   LoadBitmapImage - Read a file from the GPP partition in to a caller specified buffer.

  @param[in]       FileName     Filename including path in ascii.
  @param[in][out]  Buffer       Pointer to a pointer to a buffer allocated by the caller. If buffer is NULL or smaller in size than required,
                                then dynamically allocate buffer.
  @param[in][out]  BufferSize   If input buffer is provided, then BufferSize indicates the size of callers buffer in bytes.
                                If input buffer is null or buffersize is smaller than the filesize, size of a newly allocated buffer is stored here.
  @param[out]      ImageSize    The returned size of the image file that has been loaded in to memory.

  @retval EFI_SUCCESS            Image loaded successfully in to memory.
  @retval EFI_INVALID_PARAMETER  Invalid input parameters passed in
  @retval EFI_OUT_OF_RESOURCES  Input buffer is too small to hold the incoming image and dynamic allocation for a bigger memory failed.

**/
static
EFI_STATUS
LoadBitmapImage(
    IN CHAR8 *FileName,
    IN OUT UINT8 **Buffer,
    IN OUT UINT32 *BufferSize,
    OUT UINT32 *ImageSize)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_GUID   *EFIParition = NULL;
  UINT32      Setting = 0;
  CHAR16      LogoFile[128];
  UINT32      LogoFileSize = 0;

  if ((NULL == FileName) ||
      (NULL == Buffer) ||
      (NULL == BufferSize) ||
      (NULL == ImageSize))
  {
    Status = EFI_INVALID_PARAMETER;
    return Status;
  }
  else if ((NULL != *Buffer) &&
           (0 == *BufferSize))
  {
    // Buffersize cannot be zero if buffer is non-null
    Status = EFI_INVALID_PARAMETER;
    return Status;
  }
  
  Status = GetConfigValue("EnablePlatPartition", &Setting);
  if ((Status == EFI_SUCCESS) && (Setting == 1))
  {
    // Convert input ascii to unicode
    AsciiStrToUnicodeStr(FileName, (CHAR16*)&LogoFile);

    if (boot_from_emmc())
    {
      // Check for the filesize before continuing, need to search the GPP partition (WA) and User partition (WP)
      if (EFI_SUCCESS == (Status = GetFileSize((CHAR16*)&LogoFile,
                                                &gEfiEmmcGppPartition1Guid,
                                                &gEfiPlatPartitionTypeGuid,
                                                NULL,
                                                TRUE,
                                                NULL,
                                                &LogoFileSize)))
      {
        // Image found in GPP partition (WA), short cut the path and don't check the next partition.
          EFIParition = &gEfiEmmcGppPartition1Guid;
      }
      else if (EFI_SUCCESS == (Status = GetFileSize((CHAR16*)&LogoFile,
                                                     &gEfiEmmcUserPartitionGuid,
                                                     &gEfiPlatPartitionTypeGuid,
                                                     NULL,
                                                     TRUE,
                                                     NULL,
                                                     &LogoFileSize)))
      {
        // Image found in User partition (WP),
        EFIParition = &gEfiEmmcUserPartitionGuid;
      }
    }
    else if (boot_from_ufs())
    {
      if (EFI_SUCCESS == (Status = GetFileSize((CHAR16*)&LogoFile,
                                                &gEfiUfsLU4Guid,
                                                &gEfiPlatPartitionTypeGuid,
                                                NULL,
                                                TRUE,
                                                NULL,
                                                &LogoFileSize)))
      {
        // Image found in UFS LUN 4
        EFIParition = &gEfiUfsLU4Guid;
      }
    }
  }


  if (NULL != EFIParition)
  {
    //Cache the original provided buffer
    UINT8 *pOriginalBuffer     = *Buffer;
    UINT32 uOriginalBufferSize = *BufferSize;

    // If no buffer is provided or buffersize is smaller than filesize, allocate a new buffer dynamically.
    if ((NULL == pOriginalBuffer) ||
        (LogoFileSize > uOriginalBufferSize))
    {
      UINT8 *pNewBuffer;

      //Allocate memory to a new buffer
      pNewBuffer = (UINT8 *)AllocatePool(LogoFileSize);

      if (NULL != pNewBuffer)
      {
        if (NULL != pOriginalBuffer)
        {
          // Free the passed original buffer as it is too small and a bigger buffer has been allocated.
          FreePool(pOriginalBuffer);
        }

        //Update the provided buffer with new buffer address and size.
        *Buffer     = pNewBuffer;
        *BufferSize = LogoFileSize;
      }
      else
      {
        /* Not enough memory available for dynamic memory allocation */
        Status  = EFI_OUT_OF_RESOURCES;
      }
    }

    if (EFI_SUCCESS == Status)
    {
      UINTN  BytesToRead = LogoFileSize;

       // Read the entire buffer in to memory
       if (EFI_SUCCESS == (Status = ReadFile(LogoFile,
                                             EFIParition,
                                             &gEfiPlatPartitionTypeGuid,
                                             NULL,
                                             TRUE,
                                             NULL,
                                             &BytesToRead,
                                             0,
                                             *Buffer,
                                             LogoFileSize)))
       {
         // Return the result of the read
         *ImageSize = BytesToRead;
       }
    }
  }
  else
  {
    UINTN BufferSizeN = 0;
								  
    AsciiStrToUnicodeStr(FileName, (CHAR16*)&LogoFile);
    Status = ReadFromFV(LogoFile, (void **)Buffer, &BufferSizeN);

    if (EFI_ERROR(Status) || (Buffer == NULL))
      Status = EFI_LOAD_ERROR;

    *ImageSize = (UINT32) BufferSizeN;
  }

  return Status;
}
