////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file camxformats.h
///
/// @brief Defines the format parameters for images.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXFORMATS_H
#define CAMXFORMATS_H

// Camera dependencies
#include "camxtypes.h"
#include "camxdefs.h"

CAMX_NAMESPACE_BEGIN

/// @todo (CAMX-22) Define UBWC formats.

/// Maximum number of planes for all formats.
static const SIZE_T FormatsMaxPlanes = 3;

/// @brief This enumerates pixel formats.
enum class Format
{
    Jpeg,            ///< JPEG format.
    Y8,              ///< Luma only, 8 bits per pixel.
    Y16,             ///< Luma only, 16 bits per pixel.
    YUV420NV12,      ///< YUV 420 format as described by the NV12 fourcc.
    YUV420NV21,      ///< YUV 420 format as described by the NV21 fourcc.
    YUV422NV16,      ///< YUV 422 format as described by the NV16 fourcc
    Blob,            ///< Any non image data
    RawYUV8BIT,      ///< Packed YUV/YVU raw format. 16 bpp: 8 bits Y and 8 bits UV. U and V are interleaved as YUYV or YVYV.
    RawPrivate,      ///< Private RAW formats where data is packed into 64bit word.
                     ///  8BPP:  64-bit word contains 8 pixels p0-p7, p0 is stored at LSB.
                     ///  10BPP: 64-bit word contains 6 pixels p0-p5, most significant 4 bits are set to 0. P0 is stored at LSB.
                     ///  12BPP: 64-bit word contains 5 pixels p0-p4, most significant 4 bits are set to 0. P0 is stored at LSB.
                     ///  14BPP: 64-bit word contains 4 pixels p0-p3, most significant 8 bits are set to 0. P0 is stored at LSB.
    RawMIPI,         ///< MIPI RAW formats based on MIPI CSI-2 specification.
                     ///  8BPP: Each pixel occupies one bytes, starting at LSB. Output width of image has no restrictions.
                     ///  10BPP: 4 pixels are held in every 5 bytes. The output width of image must be a multiple of 4 pixels.
                     ///  12BPP: 2 pixels are held in every 3 bytes. The output width of image must be a multiple of 2 pixels.
                     ///  14BPP: 4 pixels are held in every 7 bytes. The output width of image must be a multiple of 4 pixels.
    RawPlain16,      ///< Plain16 RAW format. Single pixel is packed into two bytes, little endian format. Not all bits may be
                     ///  used as RAW data is generally 8, 10, or 12 bits per pixel. Lower order bits are filled first.
    RawMeta8BIT,     ///< Generic 8-bit raw meta data for internal camera usage.
    UBWCTP10,        ///< UBWC TP10 format (as per UBWC2.0 design specification)
    UBWCNV12,        ///< UBWC NV12 format (as per UBWC2.0 design specification)
    UBWCNV124R,      ///< UBWC NV12-4R format (as per UBWC2.0 design specification)
    YUV420NV12TP10,  ///< YUV 420 format 10bits per comp tight packed format.
    YUV420NV21TP10,  ///< YUV 420 format 10bits per comp tight packed format.
    YUV422NV16TP10,  ///< YUV 422 format 10bits per comp tight packed format.
    PD10,            ///< PD10 format
    RawMIPI8,        ///< 8BPP: Each pixel occupies one bytes, starting at LSB. Output width of image has no restrictions.
    FDY8,            ///< Luma only, 8 bits per pixel for FD.
    FDYUV420NV12,    ///< YUV 420 format as described by the NV12 fourcc FD.
    P010,            ///< P010 format.
};


/// @brief This enumerates degrees of rotation in a clockwise direction. The specific variable or struct member must declare the
/// semantics of the rotation (e.g. the image HAS BEEN rotated or MUST BE rotated).
enum class Rotation
{
    CW0Degrees,     ///< Zero degree rotation.
    CW90Degrees,    ///< 90 degree clockwise rotation.
    CW180Degrees,   ///< 180 degree clockwise rotation.
    CW270Degrees    ///< 270 degree clockwise rotation.
};

/// Enumeration of the color filter pattern for RAW outputs
enum class ColorFilterPattern
{
    Y,      ///< Monochrome pixel pattern.
    YUYV,   ///< YUYV pixel pattern.
    YVYU,   ///< YVYU pixel pattern.
    UYVY,   ///< UYVY pixel pattern.
    VYUY,   ///< VYUY pixel pattern.
    RGGB,   ///< RGGB pixel pattern.
    GRBG,   ///< GRBG pixel pattern.
    GBRG,   ///< GBRG pixel pattern.
    BGGR,   ///< BGGR pixel pattern.
    RGB     ///< RGB pixel pattern.
};

/// @brief This enumerates color space specifications
enum class ColorSpace
{
    Unknown,    ///< Default-assumption data space, when not explicitly specified.
    BT601Full,  ///< ITU-R Recommendation 601 (BT.601) - Full range.
    BT601625,   ///< ITU-R Recommendation 601 (BT.601) - 625-line  SDTV, 625 Lines (PAL).
    BT601525,   ///< ITU-R Recommendation 601 (BT.601) - 525-line SDTV, 525 Lines (NTSC).
    BT709,      ///< ITU-R Recommendation 709 (BT.709) HDTV.
    Depth       ///< The buffer contains depth ranging measurements from a depth camera per HAL definition.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure defines the raw format.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct RawFormat
{
    UINT32              bitsPerPixel;       ///< Bits per pixel.
    UINT32              stride;             ///< Stride in bytes.
    UINT32              sliceHeight;        ///< The number of lines in the plane which can be equal to or larger than actual
                                            ///  frame height.
    ColorFilterPattern  colorFilterPattern; ///< Color filter pattern of the RAW format.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure defines the YUV formats.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct YUVFormat
{
    UINT32 width;           ///< Width of the YUV plane in pixels.
                            ///  Tile aligned width in bytes for UBWC
    UINT32 height;          ///< Height of the YUV plane in pixels.
    UINT32 planeStride;     ///< The number of bytes between the first byte of two sequential lines on plane 1. It may be
                            ///  greater than nWidth * nDepth / 8 if the line includes padding.
                            ///  Macro-tile width aligned for UBWC
    UINT32 sliceHeight;     ///< The number of lines in the plane which can be equal to or larger than actual frame height.
                            ///  Tile height aligned for UBWC

    UINT32 metadataStride;  ///< Aligned meta data plane stride in bytes, used for UBWC formats
    UINT32 metadataHeight;  ///< Aligned meta data plane height in bytes, used for UBWC formats
    UINT32 metadataSize;    ///< Aligned metadata plane size in bytes, used for UBWC formats
    UINT32 pixelPlaneSize;  ///< Aligned pixel plane size in bytes, calculated once for UBWC formats
                            ///< and stored thereafter, since the calculations are expensive
    SIZE_T planeSize;       ///< Size in pixels for this plane.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure defines the JPEG format.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct JPEGFormat
{
    UINT32              maxJPEGSizeInBytes;       ///< Size of JPEG for max resolution in Bytes.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This union contains parameters only specified in certain output format, YUV, raw or Jpeg.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
union FormatParams
{
    YUVFormat   yuvFormat[FormatsMaxPlanes];  ///< YUV format specific properties.
    RawFormat   rawFormat;                    ///< RAW format specific properties.
    JPEGFormat  jpegFormat;                   ///< JPEG format specific properties.
};

struct AlignmentInfo
{
    UINT32 strideAlignment;      ///< Stride alignment
    UINT32 scanlineAlignment;    ///< Scanline alignment
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Defines the format of an image
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ImageFormat
{
    UINT32        width;                             ///< Width of the image in pixels.
    UINT32        height;                            ///< Height of the image in pixels.
    Format        format;                            ///< Format of the image.
    ColorSpace    colorSpace;                        ///< Color space of the image.
    Rotation      rotation;                          ///< Rotation applied to the image.
    FormatParams  formatParams;                      ///< Format specific definitions.
    SIZE_T        alignment;                         ///< The required alignment in bytes of the
                                                     ///  starting address of the allocated buffer
    AlignmentInfo planeAlignment[FormatsMaxPlanes];  ///< Stride and scanline alignment for each plane
};

CAMX_NAMESPACE_END

#endif // CAMXFORMATS_H
