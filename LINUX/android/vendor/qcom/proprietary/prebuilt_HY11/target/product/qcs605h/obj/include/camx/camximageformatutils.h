////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   camximageformatutils.h
///
/// @brief  Utility functions for camera image formats.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXIMAGEFORMATUTILS_H
#define CAMXIMAGEFORMATUTILS_H

#include "camxcommontypes.h"
#include "camxformats.h"

CAMX_NAMESPACE_BEGIN

/// @brief UBWC tile info structure for a UBWC format
struct UBWCTileInfo
{
    UINT widthPixels;       ///< Tile width in pixels
    UINT widthBytes;        ///< Tile width in pixels
    UINT height;            ///< Tile height
    UINT widthMacroTile;    ///< Macro tile width
    UINT heightMacroTile;   ///< Macro tile height
    UINT BPPNumerator;      ///< Bytes per pixel (numerator)
    UINT BPPDenominator;    ///< Bytes per pixel (denominator)
};

/// @brief UBWC Paartial tile info structure for a UBWC format
struct UBWCPartialTileInfo
{
    UINT partialTileBytesLeft;     ///< partial tile residual bytes on left
    UINT partialTileBytesRight;    ///< partial tile residual bytes on right
    UINT horizontalInit;           ///< Init / start on horizontal direction
    UINT verticalInit;             ///< Init / start on vertical direction
};

/// @brief UBWC plane mode for a UBWC format
struct UBWCPlaneModeConfig
{
    UINT highestValueBit;     ///< highest bank value bit
    UINT highestBankL1Enable; ///< highest bank l1 enable
    UINT highestBankEnable;   ///< highest bank enable
    UINT bankSpreadEnable;    ///< bank spread enable
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Static utility class for image format information.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ImageFormatUtils
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InitializeFormatParams
    ///
    /// @brief  Initialize format params
    ///
    /// @param  pFormat          The image format
    /// @param  pFormatParamInfo Format param info
    /// @param  DT               image format bits
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID InitializeFormatParams(
        ImageFormat*       pFormat,
        FormatParamInfo*   pFormatParamInfo,
        UINT32             DT);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetRawSize
    ///
    /// @brief  Get the size of a RAW buffer given a specific format.
    ///
    /// @param  pRawFormat  The RAW image format.
    ///
    /// @return The size of the RAW buffer in bytes.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SIZE_T GetRawSize(
        const ImageFormat* pRawFormat);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetTotalSize
    ///
    /// @brief  Get the total number of bytes required to hold a complete image buffer of a given format.
    ///
    /// @param  pFormat The image format.
    ///
    /// @return The total number of bytes required to hold a complete image buffer of a given format.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SIZE_T GetTotalSize(
        const ImageFormat* pFormat);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetNumberOfPlanes
    ///
    /// @brief  Get the number of planes in an image of a given format.
    ///
    /// @param  pFormat The image format.
    ///
    /// @Return The number of planes in an image of a given format.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT GetNumberOfPlanes(
        const ImageFormat* pFormat);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetPlaneSize
    ///
    /// @brief  Get the number of bytes required to hold a given plane of an image buffer of a given format.
    ///
    /// @param  pFormat     The image format.
    /// @param  planeIndex  The plane index.
    ///
    /// @return The number of bytes required to hold a given plane of an image buffer of a given format.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SIZE_T GetPlaneSize(
        const ImageFormat* pFormat,
        UINT planeIndex);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetupUBWCPlanes
    ///
    /// @brief  Setup the UBWC plane elements as per UBWC specification
    ///
    /// @param  pFormat The UBWC image format
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID SetupUBWCPlanes(
        ImageFormat* pFormat);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsYUV
    ///
    /// @brief  Test if an image format is a YUV format.
    ///
    /// @param  pFormat The image format.
    ///
    /// @return TRUE if a YUV Format.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsYUV(
        const ImageFormat* pFormat);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsRAW
    ///
    /// @brief  Test if an image format is a RAW format.
    ///
    /// @param  pFormat The image format.
    ///
    /// @return TRUE if a RAW Format.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsRAW(
        const ImageFormat* pFormat);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsUBWC
    ///
    /// @brief  Test if an image format is a UBWC format.
    ///
    /// @param  format The image format.
    ///
    /// @return TRUE if a UBWC Format.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsUBWC(
        Format format);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Is10BitFormat
    ///
    /// @brief  Test if an image format is a 10 bit format.
    ///
    /// @param  format The image format.
    ///
    /// @return TRUE if a 10 bit Format.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL Is10BitFormat(
        Format format);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetUBWCTileInfo
    ///
    /// @brief  Return the UBWC tile info
    ///
    /// @param  pFormat The image format.
    ///
    /// @return UBWC tile info pointer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static const UBWCTileInfo* GetUBWCTileInfo(
        const ImageFormat* pFormat);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetUBWCPartialTileInfo
    ///
    /// @brief  Calculate the UBWC partial tile information
    ///
    /// @param  pTileInfo               UBWC tile info pointer.
    /// @param  pUBWCPartialTileParam   UBWC partial tile parameter.
    /// @param  startOffsetPixel        start offset of frame or stripe in pixels.
    /// @param  widthPixels             width in pixels for the stripe.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static VOID GetUBWCPartialTileInfo(
        const  UBWCTileInfo*           pTileInfo,
        struct UBWCPartialTileInfo*    pUBWCPartialTileParam,
        UINT                           startOffsetPixel,
        UINT                           widthPixels);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CalcPlaneOffset
    ///
    /// @brief  Calculates the base offset for a given plane and batchId
    ///
    /// @param  pFormat             Pointer to an ImageFormat structure
    /// @param  numBatchedFrames    Total number of batched frames...will be 1 if not batching
    /// @param  batchFrameIndex     Batch index...will be 0 if not batching
    /// @param  planeIndex          Plane index (for planar formats)
    ///
    /// @return Offset from the base address for the specified plane and image within the batch.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE static SIZE_T CalcPlaneOffset(
        const ImageFormat*  pFormat,
        UINT                numBatchedFrames,
        UINT                batchFrameIndex,
        UINT                planeIndex)
    {
        SIZE_T offset = 0;
        if (IsYUV(pFormat) || IsRAW(pFormat) || IsUBWC(pFormat->format))
        {
            SIZE_T planeOffset = (planeIndex == 0) ? 0 :
                (pFormat->formatParams.yuvFormat[planeIndex - 1].planeSize *  numBatchedFrames);
            offset = planeOffset + (batchFrameIndex * pFormat->formatParams.yuvFormat[planeIndex].planeSize);
        }
        return offset;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetUBWCModeConfig
    ///
    /// @brief  Calculate the UBWC partial tile information
    ///
    /// @param  pFormat         UBWC tile info pointer.
    /// @param  planeIndex      Y or UV plane index
    ///
    /// @return UBWC Mode config value
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static UINT32 GetUBWCModeConfig(
        const ImageFormat* pFormat,
        UINT               planeIndex);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetUBWCModeConfig
    ///
    /// @brief  Set the UBWC UBWC plane mode information
    ///
    /// @param  pPlaneMode     Point to UBWC plane mode information.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE static VOID SetUBWCModeConfig(
        struct UBWCPlaneModeConfig*    pPlaneMode)
    {
        s_UBWCPlaneConfig = *pPlaneMode;
    }
    static UINT32    s_DSDBDataBits;    ///< default sensor data bits


private:
    ImageFormatUtils()                                          = delete;
    ImageFormatUtils(const ImageFormatUtils& other)             = delete;
    ImageFormatUtils& operator=(const ImageFormatUtils& other)  = delete;

    static UBWCPlaneModeConfig s_UBWCPlaneConfig; ///< UBWC plane config

};

CAMX_NAMESPACE_END

#endif // CAMXIMAGEFORMATUTILS_H
