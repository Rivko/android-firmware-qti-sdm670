/** @file BMPLib.h

  Provides support for displaying BMP image format

 Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.<BR>
 Portions Copyright (c) 2004 - 2014, Intel Corporation. All rights reserved.<BR>
 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/08/17   bd      Added new API DrawBmpBuffer
 09/09/15   ai      Initial version

=============================================================================*/
#ifndef __BMP_LIB__H__
#define __BMP_LIB__H__

#include <Uefi.h>


/**
 * BMPLib Options
 *
 * Specifies the list of options that can be specified for rendering a BMP
 * image to override the default image rendering settings.
 *
 * Current implementation supports setting the image location or the option to
 * clear the screen prior to rendering the iamge.
 */
typedef enum {
  BmplibOptionTypeLocation      = 0, /* Location. */
  BmplibOptionTypeClearScreen   = 1, /* Clear Screen. */
//BmplibOptionTypeScaling       = 2, /* Scaling */
//BmplibOptionTypeRotation      = 3, /* Rotation */
//BmplibOptionTypeRasterOp      = 4, /* Raster operations */
  BmplibOptionTypeMax           = BmplibOptionTypeClearScreen,
  BmplibOptionType_Force_32_bit = 0x7FFFFFFF  /* Force 32-bit enums */
} BMPLIB_OPTION_TYPE;

typedef struct
{
  BMPLIB_OPTION_TYPE  Type;
  VOID                *Params;  /* Option specific parameters defined below */
} BMPLIB_OPTION;

/**
 * BMPLib Location Coordinate
 */
typedef enum {
  BmplibOptionLocationTopLeft       = 0, /* Top Left */
  BmplibOptionLocationTopCenter     = 1, /* Top Center */
  BmplibOptionLocationTopRight      = 2, /* Top Right */
  BmplibOptionLocationCenterLeft    = 3, /* Center Left */
  BmplibOptionLocationCenter        = 4, /* Center */
  BmplibOptionLocationCenterRight   = 5, /* Center Right */
  BmplibOptionLocationBottomLeft    = 6, /* Bottom Left */
  BmplibOptionLocationBottomCenter  = 7, /* Bottom Center */
  BmplibOptionLocationBottomRight   = 8, /* Bottom Right */
  BmplibOptionLocationCustom        = 9, /* (DestX,DestY) coordinate */
  BmplibOptionLocationMax           = BmplibOptionLocationCustom,
  BmplibOptionLocation_Force_32_bit = 0x7FFFFFFF  /* Force 32-bit enums */
} BMPLIB_OPTION_LOCATION_COORD;

/**
 * BMPLib Location Option Parameters
 * Default setting if not specified: Center
 */
typedef struct {
  BMPLIB_OPTION_LOCATION_COORD    Location;
  UINT32                          DestX; /* Required for custom location */
  UINT32                          DestY; /* Required for custom location */
} BMPLIB_OPTION_LOCATION_PARAMS;

/**
 * BMPLib Clear Screen Option Parameters
 * Default setting if not specified: TRUE
 */
typedef struct {
  BOOLEAN                   ClearScreen;
} BMPLIB_OPTION_CLEARSCREEN_PARAMS;


/**
 * Draws a BMP file. By deafult, the screen will be cleared and the image
 * will be drawn at the center of the screen. Additional options can be
 * specified through the Options parameter to control the rendering.
 *
 * @param  FileName BMP image file
 * @param  Options  Optional parameters to control the rendering.
 * @param  NumOpts  Number of options specified.
 *
 * @retval EFI_SUCCESS            Image was successfully drawn to the screen.
 * @retval EFI_LOAD_ERROR         Cannot open FileName.
 * @retval EFI_UNSUPPORTED        FileName is not a valid *.BMP image.
 * @retval EFI_NOT_READY          Graphics Output Protocol is not ready.
 * @retval EFI_INVALID_PARAMETER  Invalid arguments specified.
 * @retval EFI_OUT_OF_RESOURCES   Not enough memory to allocate.
 * @retval EFI_DEVICE_ERROR       Optional request to clear the screen failed.
 * @retval EFI_ABORTED            Cannot fit image in the display.
 *
 */
EFI_STATUS EFIAPI
DrawBmpFile (
  IN   CHAR8                *FileName,
  IN   BMPLIB_OPTION        Opts[]         OPTIONAL,
  IN   UINTN                NumOpts
  );

/**
 * Draws a BMP buffer. Caller has to read the file from flash and pass the buffer
 * address and size
 * By deafult, the screen will be cleared and the image
 * will be drawn at the center of the screen. Additional options can be
 * specified through the Options parameter to control the rendering.
 *
 * @param  FileBuffer Pointer to buffer where BMP file is read into
 * @param  FileSize   Size of the buffer 
 * @param  Options  Optional parameters to control the rendering.
 * @param  NumOpts  Number of options specified.
 *
 * @retval EFI_SUCCESS            Image was successfully drawn to the screen.
 * @retval EFI_LOAD_ERROR         Cannot open FileName.
 * @retval EFI_UNSUPPORTED        FileName is not a valid *.BMP image.
 * @retval EFI_NOT_READY          Graphics Output Protocol is not ready.
 * @retval EFI_INVALID_PARAMETER  Invalid arguments specified.
 * @retval EFI_OUT_OF_RESOURCES   Not enough memory to allocate.
 * @retval EFI_DEVICE_ERROR       Optional request to clear the screen failed.
 * @retval EFI_ABORTED            Cannot fit image in the display.
 *
 */

EFI_STATUS EFIAPI
DrawBmpBuffer (
  UINT8                    *FileBuffer,
  UINTN                    FileSize,
  IN   BMPLIB_OPTION       Opts[]         OPTIONAL,
  IN   UINTN               NumOpts  
  );

/**
 * Renders a GopBlt buffer obtained from ConvertBmpToGopBlt() to the
 * screen. By default, the screen will be cleared and the
 * will be drawn at the center of the screen. Additional options can be
 * specified through the Options parameter to control the rendering.
 *
 * @param  GobBlt   GOP blit-safe buffer.
 * @param  BltSize  Size of GOP blit-safe buffer. See ConvertBmpToGopBlt().
 * @param  Height   PixelHeight. See ConvertBmpToGopBlt().
 * @param  Width    PixelWidth. See ConvertBmpToGopBlt().
 * @param  Options  Optional parameters to control the rendering.
 * @param  NumOpts  Number of options specified.
 *
 * @retval EFI_SUCCESS            Image was successfully drawn to the screen.
 * @retval EFI_NOT_READY          Graphics Output Protocol is not ready.
 * @retval EFI_INVALID_PARAMETER  Invalid arguments specified.
 * @retval EFI_OUT_OF_RESOURCES   Not enough memory to allocate.
 * @retval EFI_DEVICE_ERROR       Optional request to clear the screen failed.
 * @retval EFI_ABORTED            Cannot fit image in the display.
 *
 */
EFI_STATUS EFIAPI
DrawGopBltBuffer (
  IN   VOID                *GopBlt,
  IN   UINTN               BltSize,
  IN   UINTN               Height,
  IN   UINTN               Width,
  IN   BMPLIB_OPTION       Opts[]         OPTIONAL,
  IN   UINTN               NumOpts
  );


/**
 *  Convert a *.BMP graphics image to a GOP blt buffer. If a NULL Blt buffer
 *  is passed in a GopBlt buffer will be allocated by this routine. Caller is
 *  responsible for freeing the buffer after use. If a GopBlt buffer is
 *  passed in it will be used if it is big enough.
 *
 *  This function may safely be called even if the Graphics Output protocol
 *  has not been instantiated.
 *
 *  @param  BmpImage      Pointer to BMP file
 *  @param  BmpImageSize  Number of bytes in BmpImage
 *  @param  GopBlt        Buffer containing GOP version of BmpImage.
 *  @param  GopBltSize    Size of GopBlt in bytes.
 *  @param  PixelHeight   Height of GopBlt/BmpImage in pixels
 *  @param  PixelWidth    Width of GopBlt/BmpImage in pixels
 *
 *  @retval EFI_SUCCESS           GopBlt and GopBltSize are returned.
 *  @retval EFI_UNSUPPORTED       BmpImage is not a valid *.BMP image
 *  @retval EFI_BUFFER_TOO_SMALL  The passed in GopBlt buffer is not big enough.
 *                                GopBltSize will contain the required size.
 *  @retval EFI_OUT_OF_RESOURCES  Not enough memory to allocate.
 */
EFI_STATUS EFIAPI
ConvertBmpToGopBlt (
  IN     VOID      *BmpImage,
  IN     UINTN     BmpImageSize,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
     OUT UINT32    *PixelHeight,
     OUT UINT32    *PixelWidth
  );

#endif /* __BMP_LIB__H__ */

