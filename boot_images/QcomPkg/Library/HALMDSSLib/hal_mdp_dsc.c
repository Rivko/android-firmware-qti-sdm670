/*! \file */

/*
===========================================================================

FILE:         hal_mdp_dsc.c

DESCRIPTION:  This file contains a list of functions to work with Display Stream
              Compression (DSC) Encoder module

===========================================================================
===========================================================================
Copyright (c) 2012 - 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/
#define HAL_MDP_DSC_ENC_OFFSET_FRACTIONAL_BITS   11
#define HAL_MDP_DSC_ENC_SLICE                    15000

typedef enum _HAL_MDP_DSC_ENC_Input_BPP
{
   HAL_MDP_DSC_INPUT_24BPP = 0x0,
   HAL_MDP_DSC_INPUT_30BPP = 0x1,
   HAL_MDP_DSC_INPUT_MAX,
   HAL_MDP_DSC_INPUT_FORCE_32BIT = 0x7FFFFFFF

} HAL_MDP_DSC_ENC_Input_BPP;

typedef enum _HAL_MDP_DSC_ENC_CompressionRatio
{
   HAL_MDP_DSC_ENC_COMPRESSION_NONE          = 0x00000000,
   HAL_MDP_DSC_ENC_2TO1_COMPRESSION          = 0x00000002,
   HAL_MDP_DSC_ENC_3TO1_COMPRESSION          = 0x00000003,
   HAL_MDP_DSC_ENC_3p75TO1_COMPRESSION        = 0x00000004,
   HAL_MDP_DSC_ENC_COMPRESSION_MAX,
   HAL_MDP_DSC_ENC_COMPRESSION_FORCE_32BIT   = 0x7FFFFFFF

} HAL_MDP_DSC_ENC_CompressionRatio;

typedef enum _HAL_MDP_DSC_ENC_GroupSize
{
   HAL_MDP_DSC_ENC_GROUP_SIZE_1               = 0x00000000,
   HAL_MDP_DSC_ENC_GROUP_SIZE_2               = 0x00000001,
   HAL_MDP_DSC_ENC_GROUP_SIZE_3               = 0x00000002,
   HAL_MDP_DSC_ENC_GROUP_SIZE_MAX,
   HAL_MDP_DSC_ENC_GROUP_SIZE_FORCE_32BIT     = 0x7FFFFFFF

} HAL_MDP_DSC_ENC_GroupSize;

typedef enum _HAL_MDP_DSC_ENC_ICHRst
{
   HAL_MDP_DSC_ENC_ICH_RST_OVERRIDE_OFF   = 0x00000000,
   HAL_MDP_DSC_ENC_ICH_RST_OVERRIDE_ON    = 0x00000001,
   HAL_MDP_DSC_ENC_ICH_RST_AUTO           = 0x00000002,
   HAL_MDP_DSC_ENC_ICH_RST_MAX,
   HAL_MDP_DSC_ENC_ICH_RST_FORCE_32BIT    = 0x7FFFFFFF

} HAL_MDP_DSC_ENC_ICHRst;

typedef struct _HAL_MDP_DSC_ENC_RateControl
{
   uint32 uInitialXmitDelay;
   uint32 uFirstLineBpgOffset;
   uint32 uInitialOffset;
   uint32 uFlatnessMinQp;
   uint32 uFlatnessMaxQp;
   uint32 uQuantIncrLimit0;
   uint32 uQuantIncrLimit1;
   uint32 uRangeMinQp[HAL_MDP_DSC_ENC_RC_RANGE_PARAMS_TABLE_SIZE];
   uint32 uRangeMaxQp[HAL_MDP_DSC_ENC_RC_RANGE_PARAMS_TABLE_SIZE];

} HAL_MDP_DSC_ENC_RateControl;

typedef struct _HAL_MDP_DSC_Info 
{
   /** Control Parameters */
   bool32                           bVideoMode;
   bool32                           bSplitDisplayEnable;
   bool32                           bHardSliceMuxEnable;

   /** Encoded Configuration */
   bool32                           bRgbInput;
   uint32                           uValInputBPC;
   uint32                           uValInputBPP;

   uint32                           uEolNvb;             // number of valid bytes in last 48-bit transmission of line
   uint32                           uTargetBPP;
   uint32                           uTargetBPPReg;
   uint32                           uInitialLines;
   uint32                           uFlatnessDetThreshold;
   HAL_MDP_DSC_ENC_Input_BPP        eInputBPP;
   HAL_MDP_DSC_ENC_CompressionRatio eCompressionRatio;
   HAL_MDP_DSC_ENC_GroupSize        eLastGroupSize;
   HAL_MDP_DSC_ENC_ICHRst           eIchRstSetting;

} HAL_MDP_DSC_Info;

/*------------------------------------------------------------------------
 * Global Data Definitions
 *------------------------------------------------------------------------ */

static HAL_MDP_DSC_ENC_RateControl gsDscEncDefaultRateControl[HAL_MDP_DSC_ENC_COMPRESSION_MAX] =
{
//    xmt  fln  init  min  max inc inc     rangeMinQp                                                            rangeMaxQp
//    dly  bpg  ofs   qp   qp  lm0 lm1    0   1   2   3   4   5   6   7   8    9   10   11   12   13   14       0   1   2   3   4   5   6   7   8    9   10   11   12   13   14
   { 000,  00, 0000,   0,  00, 00, 00, {  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,  00 }, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  00,  00,  00,  00,  00,  00 } },
   { 000,  00, 0000,   0,  00, 00, 00, {  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,  00 }, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  00,  00,  00,  00,  00,  00 } },
   { 341,  15, 2048,   3,  12, 11, 11, {  0,  0,  1,  1,  3,  3,  3,  3,  3,   3,   5,   5,   5,   7,  13 }, {  2,  4,  5,  6,  7,  7,  7,  8,  9,  10,  11,  12,  13,  13,  15 } },
   { 512,  12, 6144,   3,  12, 11, 11, {  0,  0,  1,  1,  3,  3,  3,  3,  3,   3,   5,   5,   5,   7,  13 }, {  4,  4,  5,  6,  7,  7,  7,  8,  9,  10,  11,  12,  13,  13,  15 } },
   { 512,  12, 6144,   7,  16, 15, 15, {  0,  4,  5,  5,  7,  7,  7,  7,  7,   7,   9,   9,   9,   11, 17 }, {  8,  8,  9,  10, 11, 11, 11, 12, 13, 14,  15,  16,  17,  17,  19 } }
};

static HAL_MDP_DSC_ENC_RateControl gsDscEncRateControl_Scr_1[HAL_MDP_DSC_ENC_COMPRESSION_MAX] =
{
//    xmt  fln  init  min  max inc inc     rangeMinQp                                                            rangeMaxQp
//    dly  bpg  ofs   qp   qp  lm0 lm1    0   1   2   3   4   5   6   7   8    9   10   11   12   13   14       0   1   2   3   4   5   6   7   8    9   10   11   12   13   14
   { 000,  00, 0000,   0,  00, 00, 00, {  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,  00 }, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  00,  00,  00,  00,  00,  00 } },
   { 000,  00, 0000,   0,  00, 00, 00, {  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,  00 }, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  00,  00,  00,  00,  00,  00 } },
   { 341,  15, 2048,   3,  12, 11, 11, {  0,  0,  1,  1,  3,  3,  3,  3,  3,   3,   5,   5,   5,   7,  10 }, {  2,  4,  5,  6,  7,  7,  7,  8,  8,   9,   9,   9,   9,  10,  11 } },
   { 512,  15, 6144,   3,  12, 11, 11, {  0,  0,  1,  1,  3,  3,  3,  3,  3,   3,   5,   5,   5,   9,  12 }, {  4,  4,  5,  6,  7,  7,  7,  8,  9,  10,  10,  11,  11,  12,  13 } },
   { 512,  12, 6144,   7,  16, 15, 15, {  0,  4,  5,  5,  7,  7,  7,  7,  7,   7,   9,   9,   9,   11, 17 }, {  8,  8,  9,  10, 11, 11, 11, 12, 13, 14,  15,  16,  17,  17,  19 } }
};

//                                                                  0    1     2     3     4     5     6     7     8     9     10    11    12    13
static uint32   guDscEncDefaultBufThreshold[HAL_MDP_DSC_ENC_RC_BUF_THRESHOLD_TABLE_SIZE] = { 896, 1792, 2688, 3584, 4480, 5376, 6272, 6720, 7168, 7616, 7744, 7872, 8000, 8064 };
//                                                                  0   1   2   3   4   5   6   7   8    9   10   11   12   13   14
static int8     giDscEncDdefaultBpgOffset[HAL_MDP_DSC_ENC_RC_RANGE_PARAMS_TABLE_SIZE]    = { 2,  0,  0, -2, -4, -6, -8, -8, -8, -10, -10, -12, -12, -12, -12 };

static uint32   guDscEncDefaultModelSize               = 8192;
static uint32   guDscEncDefaultEdgeFactor              = 6;
static uint32   guDscEncDefaultTgtOffsetHigh           = 3;
static uint32   guDscEncDefaultTgtOffsetLow            = 3;
static uint32   guDscEncDefaultLineBufDepth            = 9;
static uint32   guDscEncDefaultFlatnessDetThreshold    = 2;

static HAL_MDP_DSC_Info sDSCInfo;

/*------------------------------------------------------------------------
 * Private Function Definitions
 *------------------------------------------------------------------------ */

static uint32 HAL_MDP_DSC_ENC_CalcBppF(uint32 uBPC )
{
   return (uint32) (0.5 + uBPC * 16);
}

static uint32 HAL_MDP_DSC_ENC_CalcHrdDelay(uint32 uOutputBpp, 
                                           uint32 uSliceWidth, 
                                           uint32 uModelSize, 
                                           uint32 uInitOffset, 
                                           uint32 uXmitDelay, 
                                           uint32 uFirstLineOffset )
{
   float    fBitsPerPixel  = (float) uOutputBpp;
   uint32   uGroupsPerLine = (uSliceWidth + 2) / 3;
   uint32   uRbsMin        = uModelSize - uInitOffset + (int) (uXmitDelay * fBitsPerPixel) + uGroupsPerLine * uFirstLineOffset;
   uint32   uHrdDelay      = (uint32) HAL_CEIL((double) uRbsMin, fBitsPerPixel);

   return uHrdDelay;
}

static uint32 HAL_MDP_DSC_ENC_CalcRcbBits(uint32 uOutputBpp, uint32 uHrdDelay )
{
   float    fBitsPerPixel  = (float)uOutputBpp;
   uint32   uRcbBits       = (uint32) ((double)uHrdDelay * fBitsPerPixel);

   return uRcbBits;
}

static uint32 HAL_MDP_DSC_ENC_CalcDecDelay(uint32 uHrdDelay, uint32 uXmitDelay )
{
   uint32 uDecDelay = uHrdDelay - uXmitDelay;

   return uDecDelay;
}

static uint32 HAL_MDP_DSC_ENC_CalcChunkSize(uint32 uOutputBpp, uint32 uSliceWidth)
{
   float    fBitsPerPixel   = (float)uOutputBpp;
   uint32   uChunkSize      = (uint32)HAL_CEIL(uSliceWidth * fBitsPerPixel,  8.0);

   return uChunkSize;
}

static uint32 HAL_MDP_DSC_ENC_CalcInitialLines(uint32 uBPC, uint32 uSoftSlices, uint32 uSliceWidth, uint32 uXmitDelay )
{
   uint32 uSsmDelay       = (uBPC < 10 ? 83 : 91);
   uint32 uTotalPixels    = (uSsmDelay * 3 + 30 + uXmitDelay + 6);
   uint32 uInitialLines   = 0x00;

   uTotalPixels          += (uSoftSlices > 1 ? (uSsmDelay * 3) : 0);
   uInitialLines          = (uint32) HAL_CEIL( (float) uTotalPixels, uSliceWidth );

   return uInitialLines;
}

static uint32 HAL_MDP_DSC_ENC_CalcInitScaleValue(uint32 uSliceWidth, uint32 uModelSize, uint32 uInitialOffset )
{
   uint32 uGroupsPerLine    = (uSliceWidth + 2) / 3;
   uint32 uInitScaleValue   = (8 * uModelSize / (uModelSize - uInitialOffset));

   if ( uGroupsPerLine < uInitScaleValue - 8 )
   {
      uInitScaleValue = uGroupsPerLine + 8;
   }

   return uInitScaleValue;
}

static uint32 HAL_MDP_DSC_ENC_CalcScaleDecInterval(uint32 uSliceWidth, uint32 uInitScaleValue )
{
   uint32 uGroupsPerLine      = (uSliceWidth + 2) / 3;
   uint32 uScaleDecInterval   = uGroupsPerLine / (uInitScaleValue - 8);

   return uScaleDecInterval;
}

static uint32 HAL_MDP_DSC_ENC_CalcScaleIncInterval(uint32 uModelSize, uint32 uFinalOffset, uint32 uNflBpgOffset, uint32 uSliceBpgOffset )
{
   uint32 uFinalScale         = (8 * uModelSize / (uModelSize - uFinalOffset));
   uint32 uScaleIncInterval   = 0;

   // BEGIN scale_increment_interval fix
   if ( uFinalScale > 9 )
   {
      // Note: the following calculation assumes that the rcXformOffset crosses 0 at some point.  If the zero-crossing
      //   doesn't occur in a configuration, we recommend to reconfigure the rc_model_size and thresholds to be smaller
      //   for that configuration.
      uScaleIncInterval = (uint32)((double)(1 << HAL_MDP_DSC_ENC_OFFSET_FRACTIONAL_BITS) * uFinalOffset / 
                                  ((double)(uFinalScale - 9) * (uNflBpgOffset + uSliceBpgOffset)));
      // Assertf( scaleIncInterval <= 65535,
      // "ERROR: required scale increment interval is too high.  Consider using smaller slices or increase initial delay\n" );
   }
   // END scale_increment_interval fix
   return uScaleIncInterval;
}

static uint32 HAL_MDP_DSC_ENC_CalcExtraMuxBits(uint32 uConvertRgb, uint32 uBPC, uint32 uChunkSize, uint32 uSliceHeight, uint32 uMuxMode, uint32 uMuxWordSize )
{
   uint32 uExtraMuxBits  = 0;
   uint32 uSliceBits     = 0;

   if(uMuxMode == 1)
   {
      if (uConvertRgb)
      {
         uExtraMuxBits = 3 * (uMuxWordSize + (4 * uBPC + 4) - 2);
      }
      else  // YCbCr
      {
         uExtraMuxBits = 3 * uMuxWordSize + (4 * uBPC + 4) + 2 * (4 * uBPC) - 2;
      }
   }
   
   uSliceBits = 8 * uChunkSize * uSliceHeight;
   while ( (uExtraMuxBits > 0) && ((uSliceBits - uExtraMuxBits) % uMuxWordSize) )
   {
      uExtraMuxBits--;
   }
   
   return uExtraMuxBits;
}

static uint32 HAL_MDP_DSC_ENC_CalcFinalOffset(uint32 uOutputBpp, uint32 uModelSize, uint32 uXmitDelay, uint32 uExtraMuxBits )
{
   uint32 uFinalOffset = uModelSize - ((uXmitDelay * HAL_MDP_DSC_ENC_CalcBppF(uOutputBpp) + 8) >> 4) + uExtraMuxBits;

   return uFinalOffset;
}

static uint32 HAL_MDP_DSC_ENC_CalcMuxWordSize(uint32 uBPC )
{
   uint32 uMuxWordSize = (uBPC == 12 ? 64 : 48);

   return uMuxWordSize;
}

static uint32 HAL_MDP_DSC_ENC_CalcNflOffset(uint32 uFirstLineBpgOffset, uint32 uSliceHeight )
{
   uint32 uNflBpgOffset = (uint32) HAL_CEIL((double)(uFirstLineBpgOffset << HAL_MDP_DSC_ENC_OFFSET_FRACTIONAL_BITS), (uSliceHeight - 1));

   return uNflBpgOffset;
}

static uint32 HAL_MDP_DSC_ENC_CalcSliceOffset(uint32 uSliceWidth, uint32 uSliceHeight, uint32 uModelSize, uint32 uInitOffset, uint32 uExtraMuxBits )
{
   uint32 uGroupsPerLine   = (uSliceWidth + 2) / 3;
   uint32 uGroupsTotal     = uGroupsPerLine * uSliceHeight;
   uint32 uSliceBpgOffset  = (uint32) HAL_CEIL((double)(1 << HAL_MDP_DSC_ENC_OFFSET_FRACTIONAL_BITS) * (uModelSize - uInitOffset + uExtraMuxBits), uGroupsTotal);

   return uSliceBpgOffset;
}

static uint32 HAL_MDP_DSC_ENC_CalcEncodedSize(uint32 uImageWidth, uint32 uImageHeight, uint32 uSliceWidth, uint32 uSliceHeight, uint32 uChunkSize )
{
   uint32 uSliceBufSize    = (uChunkSize * uSliceHeight);                  // Total number of bytes to generate
   uint32 uSlicesPerWidth  = (uImageWidth + uSliceWidth - 1) / uSliceWidth;
   uint32 uSlicesPerHeight = (uImageHeight + uSliceHeight - 1) / uSliceHeight;
   uint32 uBufSize         = (uSlicesPerWidth * uSlicesPerHeight * uSliceBufSize);

   return uBufSize;
}

static void HAL_MDP_DSC_ENC_PrepareRegInfo(HAL_MDP_DSC_Info *pDSCInfo, HAL_MDP_DSC_EncoderConfigType *pDSCEncConfig)
{
   uint32                       uIndex                  = 0x00000000;
   uint32                       uMuxWordSize            = 0x00000000;
   uint32                       uExtraMuxBits           = 0x00000000;
   uint32                       uHrdDelay               = 0x00000000;
   uint32                       uSliceWidthRemainder    = 0x00000000;
   uint32                       uSoftSlices             = 0x00000000;
   uint32                       uRcBufferThresholdVal   = 0x00000000;
   uint32                       uRatioTblIndex          = 0x00000000;
   uint32                       uSoftSlicesPerEncoder   = 0x00000000;
   HAL_MDP_DSC_EncParamsType   *pEncParams              = pDSCEncConfig->pEncoderParams;
   HAL_MDP_DSC_ENC_RateControl *pRateList;

   // General Parameters
   pDSCInfo->uTargetBPP    = pEncParams->uBitsPerPixel;
   pDSCInfo->uValInputBPC  = pEncParams->uBitsPerComponent;

   pDSCInfo->uTargetBPPReg = (uint32)(pDSCInfo->uTargetBPP  << 4);   // format 6.4, 4 fractional bits

   if(FALSE == pDSCEncConfig->bUserProvidedEncParams)
   {
      pEncParams->bConvertRGB          = TRUE;

      // Input picture width and height  @@ TODO @@ Handling uneven input width for each DSC module
      pEncParams->uPicWidth  = pDSCEncConfig->uInputFrameWidthInPixels;
      pEncParams->uPicHeight = pDSCEncConfig->uInputFrameHeightInPixels;

      if ((0 == pEncParams->uSliceHeight) || (0 == pEncParams->uSliceWidth))
      {
          /* Configure to default slice height and width */
          pEncParams->uSliceWidth   = pEncParams->uPicWidth/2;
          pEncParams->uSliceHeight  = 16;
      }

      if((8 ==  pEncParams->uBitsPerComponent) && 
         (12 == pEncParams->uBitsPerPixel))
      {
         pDSCInfo->eInputBPP = 0;	/* 8 bits input */
         uRatioTblIndex      = (uint32)HAL_MDP_DSC_ENC_2TO1_COMPRESSION;        // 24bpp input -> 2to1 compression -> 12bpp target output
      }
      else if((8 ==  pEncParams->uBitsPerComponent) &&
              (8 == pEncParams->uBitsPerPixel))
      {
         pDSCInfo->eInputBPP = 0;	/* 8 bits input */
         uRatioTblIndex      = (uint32)HAL_MDP_DSC_ENC_3TO1_COMPRESSION;       // 24bpp input -> 3to1 compression -> 8bpp target output
      }
      else if((10 ==  pEncParams->uBitsPerComponent) &&
              (8 == pEncParams->uBitsPerPixel))
      {
         pDSCInfo->eInputBPP = 1;	/* 10 bits input */
         uRatioTblIndex      = (uint32)HAL_MDP_DSC_ENC_3p75TO1_COMPRESSION;       // 30bpp input -> 3to1 compression -> 8bpp target output
      }
      
      sDSCInfo.eCompressionRatio = uRatioTblIndex;

      // Find the number of soft slices (horizontal slices)
      if((TRUE == pDSCInfo->bSplitDisplayEnable) && (FALSE == pDSCInfo->bHardSliceMuxEnable))
      {
         uSoftSlices   = ((pEncParams->uPicWidth/2) / pEncParams->uSliceWidth);
      }
      else
      {
         uSoftSlices   = ((pEncParams->uPicWidth) / pEncParams->uSliceWidth);
      }

      // Find the number of soft slices per encoder
      uSoftSlicesPerEncoder  = uSoftSlices;

      pDSCEncConfig->uBytePerPkt = uSoftSlicesPerEncoder * pEncParams->uSliceWidth;

      if((TRUE == pDSCInfo->bHardSliceMuxEnable) && (1 < uSoftSlices))
      {
         uSoftSlicesPerEncoder  = uSoftSlices/2;
      }

      // use scr table when scr=1
      if (1 == pEncParams->uDSCVersionScr)
      {
         pRateList = gsDscEncRateControl_Scr_1;
      }
      else
      {
         pRateList = gsDscEncDefaultRateControl;
      }

      // Program the Rate Control Parameters depending on the input bpc and target bpp
      pEncParams->uInitialXmitDelay      = pRateList[uRatioTblIndex].uInitialXmitDelay;
      pEncParams->uFirstLineBpgOffset    = pRateList[uRatioTblIndex].uFirstLineBpgOffset;
      pEncParams->uInitialOffset         = pRateList[uRatioTblIndex].uInitialOffset;
      pEncParams->uFlatnessMinQp         = pRateList[uRatioTblIndex].uFlatnessMinQp;
      pEncParams->uFlatnessMaxQp         = pRateList[uRatioTblIndex].uFlatnessMaxQp;
      pEncParams->uRcQuantIncrLimit0     = pRateList[uRatioTblIndex].uQuantIncrLimit0;
      pEncParams->uRcQuantIncrLimit1     = pRateList[uRatioTblIndex].uQuantIncrLimit1;

      for(uIndex = 0; uIndex < HAL_MDP_DSC_ENC_RC_RANGE_PARAMS_TABLE_SIZE; uIndex++)
      {
         pEncParams->uRangeMinQp[uIndex]       = (uint8)pRateList[uRatioTblIndex].uRangeMinQp[uIndex];
         pEncParams->uRangeMaxQp[uIndex]       = (uint8)pRateList[uRatioTblIndex].uRangeMaxQp[uIndex];
         pEncParams->iRangeBpgOffset[uIndex]   = giDscEncDdefaultBpgOffset[uIndex];	// register is signed
      }

      // Program other default values that are input bpc/target bpp independent
      pEncParams->uRcModelSize     = guDscEncDefaultModelSize;
      pEncParams->uRcEdgeFactor    = guDscEncDefaultEdgeFactor;
      pEncParams->uRcTgtOffsetHi   = guDscEncDefaultTgtOffsetHigh;
      pEncParams->uRcTgtOffsetLo   = guDscEncDefaultTgtOffsetLow;

      for (uIndex = 0; uIndex < HAL_MDP_DSC_ENC_RC_BUF_THRESHOLD_TABLE_SIZE; uIndex++)
      {
         uRcBufferThresholdVal                = guDscEncDefaultBufThreshold[uIndex];
         //register fields are 8 bit, six 0 LSBs are appended to each value in the HW
         pEncParams->uRcBufThresh[uIndex] = (uint8)(uRcBufferThresholdVal >> 6);
      }

      // Program the linebuffer bit depth, valid range is 8 to 13
      pEncParams->uLineBufDepth    = guDscEncDefaultLineBufDepth;

      // Calculate the chunk size
      pEncParams->uChunkSize    = HAL_MDP_DSC_ENC_CalcChunkSize(pDSCInfo->uTargetBPP, pEncParams->uSliceWidth);

      // Calculate the extra mux bits
      uMuxWordSize   = HAL_MDP_DSC_ENC_CalcMuxWordSize(pDSCInfo->uValInputBPC);
      uExtraMuxBits  = HAL_MDP_DSC_ENC_CalcExtraMuxBits(pDSCInfo->bRgbInput,
                                                        pDSCInfo->uValInputBPC,
                                                        pEncParams->uChunkSize,
                                                        pEncParams->uSliceHeight,
                                                        1,                   // muxMode
                                                        uMuxWordSize);

      //// Calculate the decoder delay
      uHrdDelay      = HAL_MDP_DSC_ENC_CalcHrdDelay(pDSCInfo->uTargetBPP,
                                                    pEncParams->uSliceWidth,
                                                    pEncParams->uRcModelSize,
                                                    pEncParams->uInitialOffset,
                                                    pEncParams->uInitialXmitDelay,
                                                    pEncParams->uFirstLineBpgOffset);

      // decoder delay: number of pixels to delay the VLD on the decoder, not including SSM
      pEncParams->uInitialDecDelay  = (uint32) HAL_MDP_DSC_ENC_CalcDecDelay(uHrdDelay, pEncParams->uInitialXmitDelay);

      //// Calculate the offsets
      // Calculate the BPG Non-First-Line Offset
      pEncParams->uNflBpgOffset       = HAL_MDP_DSC_ENC_CalcNflOffset(pEncParams->uFirstLineBpgOffset, 
                                                                      pEncParams->uSliceHeight);

      // Calculate the BPG Slice Offset (BPG offset used to enforce slice bit constraint)
      pEncParams->uSliceBpgOffset     = HAL_MDP_DSC_ENC_CalcSliceOffset(pEncParams->uSliceWidth, 
                                                                      pEncParams->uSliceHeight, 
                                                                      pEncParams->uRcModelSize, 
                                                                      pEncParams->uInitialOffset, 
                                                                      uExtraMuxBits);
      // Calculate the final offset
      pEncParams->uFinalOffset        = HAL_MDP_DSC_ENC_CalcFinalOffset(pDSCInfo->uTargetBPP, 
                                                                      pEncParams->uRcModelSize, 
                                                                      pEncParams->uInitialXmitDelay, 
                                                                      uExtraMuxBits);

      //// Calculate the scale parameters
      // Calculate the value for initial scale factor
      pEncParams->uInitialScaleValue  = HAL_MDP_DSC_ENC_CalcInitScaleValue(pEncParams->uSliceWidth, 
                                                                               pEncParams->uRcModelSize, 
                                                                               pEncParams->uInitialOffset);

      // Calculate the scale decrement value (decrement scale factor every scale_decrement_interval groups)
      pEncParams->uScaleDecInterval   = HAL_MDP_DSC_ENC_CalcScaleDecInterval(pEncParams->uSliceWidth, 
                                                                             pEncParams->uInitialScaleValue);

      // Calculate the scale increment value (increment scale factor every scale_increment_interval groups)
      pEncParams->uScaleIncInterval   = HAL_MDP_DSC_ENC_CalcScaleIncInterval(pEncParams->uRcModelSize, 
                                                                             pEncParams->uFinalOffset, 
                                                                             pEncParams->uNflBpgOffset, 
                                                                             pEncParams->uSliceBpgOffset);
   }

   ////// Generic Parameter Set#2

   // Calculate the initial lines parameter
   pDSCInfo->uInitialLines = HAL_MDP_DSC_ENC_CalcInitialLines(pDSCInfo->uValInputBPC,
                                                               uSoftSlicesPerEncoder,
                                                               pEncParams->uSliceWidth,
                                                               pEncParams->uInitialXmitDelay);

   pDSCInfo->uFlatnessDetThreshold  = guDscEncDefaultFlatnessDetThreshold;

   // Calculate the size of the last group in each slice
   uSliceWidthRemainder = (pEncParams->uSliceWidth % 3);

   switch (uSliceWidthRemainder)
   {
   case 0:
      {
         pDSCInfo->eLastGroupSize   = HAL_MDP_DSC_ENC_GROUP_SIZE_3;      // last group has 3 valid pxels
      }
      break;
   case 1:
      {
         pDSCInfo->eLastGroupSize   = HAL_MDP_DSC_ENC_GROUP_SIZE_1;      // last group has 1 valid pxels
      }
      break;
   case 2:
      {
         pDSCInfo->eLastGroupSize   = HAL_MDP_DSC_ENC_GROUP_SIZE_2;      // last group has 2 valid pxels
      }
      break;
   default:
      {
      }
      break;
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_DSC_BaseOffset()
*/
/*!
* \brief
*     Returns register offset of the DSC module
*
* \param [in] eDSCEncEngineId    - none, 0, 1, 2 etc.
*
* \retval uint32
*
****************************************************************************/
static uintPtr HAL_MDP_Get_DSC_BaseOffset(HAL_MDP_DSCId  eDSCEncEngineId)
{
   uintPtr   uRegOffset  = 0x00000000;

   if((HAL_MDP_DSC_NONE        == eDSCEncEngineId) ||
      (HAL_MDP_DSC_MAX         <= eDSCEncEngineId))
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      uRegOffset = uMDPDSCRegBaseOffset[eDSCEncEngineId];
   }
   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSC_ENC_ProgramCfgReg()
*/
/*!
* \brief
*     Programs the DSC Encoder hardware module registers
*
* \param [in] eDSCEncEngineId    - none, 0, 1, 2 etc.
* \param [in] pDSCInfo           - One set of Encoder configuration parameters
* \param [in] pDSCEncConfig      - Second set of Encoder configuration parameters
*
* \retval void
*
****************************************************************************/
static void HAL_MDP_DSC_ENC_ProgramCfgReg(HAL_MDP_DSCId                  eDSCEncEngineId, 
                                          HAL_MDP_DSC_Info              *pDSCInfo, 
                                          HAL_MDP_DSC_EncoderConfigType *pDSCEncConfig)
{
   uintPtr                       uBlkRegOffset  = 0x00000000;
   uint32                        uIndex         = 0x00000000;   
   uint32                        uRegValue      = 0x00000000;
   HAL_MDP_DSC_EncParamsType    *pEncParams     = pDSCEncConfig->pEncoderParams;

   uBlkRegOffset = HAL_MDP_Get_DSC_BaseOffset(eDSCEncEngineId);

   // DSC_X_COMMAN_MODE
   uRegValue   = 0x00;
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_COMMON_MODE, SPLIT_PANEL, pDSCInfo->bSplitDisplayEnable);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_COMMON_MODE, MULTIPLEX, pDSCInfo->bHardSliceMuxEnable);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_COMMON_MODE, VIDEO, pDSCInfo->bVideoMode);
   out_dword(HWIO_MDP_DSC_0_COMMON_MODE_ADDR + uBlkRegOffset, uRegValue);

   // DSC_X_ENC
   uRegValue   = 0x00;
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_ENC, INPUT_10BITS, pDSCInfo->eInputBPP);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_ENC, CONVERT_RGB, TRUE);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_ENC, ENABLE_422, FALSE);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_ENC, LINEBUF_DEPTH, pEncParams->uLineBufDepth);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_ENC, BLOCK_PRED_ENABLE, pEncParams->bBlockPredEnable);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_ENC, BITS_PER_PIXEL, pDSCInfo->uTargetBPPReg);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_ENC, SLICE_LAST_GROUP_SIZE, pDSCInfo->eLastGroupSize);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_ENC, INITIAL_LINES, pDSCInfo->uInitialLines);
   out_dword(HWIO_MDP_DSC_0_ENC_ADDR + uBlkRegOffset, uRegValue);

   // DSC_X_PICTURE
   uRegValue   = 0x00;
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_PICTURE, WIDTH, pEncParams->uPicWidth);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_PICTURE, HEIGHT, pEncParams->uPicHeight);
   out_dword(HWIO_MDP_DSC_0_PICTURE_ADDR + uBlkRegOffset, uRegValue);

   // DSC_X_SLICE
   uRegValue   = 0x00;
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_SLICE, WIDTH, pEncParams->uSliceWidth);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_SLICE, HEIGHT, pEncParams->uSliceHeight);
   out_dword(HWIO_MDP_DSC_0_SLICE_ADDR + uBlkRegOffset, uRegValue);

   // DSC_X_CHUNK_SIZE
   uRegValue   = 0x00;
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_CHUNK_SIZE, VALUE, pEncParams->uChunkSize);
   out_dword(HWIO_MDP_DSC_0_CHUNK_SIZE_ADDR + uBlkRegOffset, uRegValue);

   // DSC_X_DELAY
   uRegValue   = 0x00;
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_DELAY, INITIAL_DEC, pEncParams->uInitialDecDelay);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_DELAY, INITIAL_XMIT, pEncParams->uInitialXmitDelay);
   out_dword(HWIO_MDP_DSC_0_DELAY_ADDR + uBlkRegOffset, uRegValue);

   // DSC_X_SCALE
   out_dword(HWIO_MDP_DSC_0_SCALE_INITIAL_ADDR + uBlkRegOffset, pEncParams->uInitialScaleValue);

   // DSC_X_SCALE_DECREMENT_INTERVAL
   out_dword(HWIO_MDP_DSC_0_SCALE_DECREMENT_INTERVAL_ADDR + uBlkRegOffset, pEncParams->uScaleDecInterval);

   // DSC_X_SCALE_INCREMENT_INTERVAL
   out_dword(HWIO_MDP_DSC_0_SCALE_INCREMENT_INTERVAL_ADDR + uBlkRegOffset, pEncParams->uScaleIncInterval);

   // DSC_X_FIRST_LINE_BPG_OFFSET
   out_dword(HWIO_MDP_DSC_0_FIRST_LINE_BPG_OFFSET_ADDR + uBlkRegOffset, pEncParams->uFirstLineBpgOffset);

   // DSC_X_BPG_OFFSET
   uRegValue   = 0x00;
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_BPG_OFFSET, NFL, pEncParams->uNflBpgOffset);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_BPG_OFFSET, SLICE, pEncParams->uSliceBpgOffset);
   out_dword(HWIO_MDP_DSC_0_BPG_OFFSET_ADDR + uBlkRegOffset, uRegValue);

   // DSC_X_DSC_OFFSET
   uRegValue   = 0x00;
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_DSC_OFFSET, INITIAL, pEncParams->uInitialOffset);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_DSC_OFFSET, FINAL, pEncParams->uFinalOffset);
   out_dword(HWIO_MDP_DSC_0_DSC_OFFSET_ADDR + uBlkRegOffset, uRegValue);

   // DSC_X_FLATNESS
   uRegValue   = 0x00;
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_FLATNESS, MIN_QP, pEncParams->uFlatnessMinQp);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_FLATNESS, MAX_QP, pEncParams->uFlatnessMaxQp);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_FLATNESS, DET_THRESH, pDSCInfo->uFlatnessDetThreshold);
   out_dword(HWIO_MDP_DSC_0_FLATNESS_ADDR + uBlkRegOffset, uRegValue);

   // DSC_X_RC_MODEL_SIZE
   out_dword(HWIO_MDP_DSC_0_RC_MODEL_SIZE_ADDR + uBlkRegOffset, pEncParams->uRcModelSize);

   // DSC_X_RC
   uRegValue   = 0x00;
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_RC, EDGE_FACTOR, pEncParams->uRcEdgeFactor);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_RC, QUANT_INCR_LIMIT0, pEncParams->uRcQuantIncrLimit0);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_RC, QUANT_INCR_LIMIT1, pEncParams->uRcQuantIncrLimit1);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_RC, TGT_OFFSET_HI, pEncParams->uRcTgtOffsetHi);
   uRegValue   = HWIO_OUT_FLD(uRegValue, MDP_DSC_0_RC, TGT_OFFSET_LO, pEncParams->uRcTgtOffsetLo);
   out_dword(HWIO_MDP_DSC_0_RC_ADDR + uBlkRegOffset, uRegValue);

   // DSC_X_RANGE_YYY
   for(uIndex = 0; uIndex < HAL_MDP_DSC_ENC_RC_RANGE_PARAMS_TABLE_SIZE; uIndex++)
   {
      out_dword((HWIO_MDP_DSC_0_RANGE_MIN_QP_n_ADDR(uIndex) + uBlkRegOffset), pEncParams->uRangeMinQp[uIndex]);
      out_dword((HWIO_MDP_DSC_0_RANGE_MAX_QP_n_ADDR(uIndex) + uBlkRegOffset), pEncParams->uRangeMaxQp[uIndex]);
      out_dword((HWIO_MDP_DSC_0_RANGE_BPG_OFFSET_n_ADDR(uIndex) + uBlkRegOffset), pEncParams->iRangeBpgOffset[uIndex]);
   }

   // DSC_X_RC_BUF_THRESH
   for(uIndex = 0; uIndex < HAL_MDP_DSC_ENC_RC_BUF_THRESHOLD_TABLE_SIZE; uIndex++)
   {
      out_dword((HWIO_MDP_DSC_0_RC_BUF_THRESH_n_ADDR(uIndex) + uBlkRegOffset), pEncParams->uRcBufThresh[uIndex]);
   }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSC_ENC_DCESelCfg()
*/
/*!
* \brief
*     Function to program the top-level data-flow configurations for DSC modules
*
* \param [in] pDSCEncConfig    - Encoder Parameter Configuration information to select the control/data flow
* \param [in] pDSCInfo         - One set of Encoder configuration parameters
*
* \retval void
*
****************************************************************************/
static void HAL_MDP_DSC_ENC_DCESelCfg(HAL_MDP_DSC_EncoderConfigType *pDSCEncConfig, HAL_MDP_DSC_Info *pDSCInfo)
{
    uint32   uRegValue = 0x00000000;

    out_dword(HWIO_MDP_DCE_SEL_ADDR, uRegValue);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSC_ENC_ContextInit()
*/
/*!
* \brief
*     Initializes the base data structures before preparing the encoder register configurations
*
* \param [in] pDSCEncConfig    - Encoder Parameter Configuration information to select the control/data flow
* \param [in] uFlags           - Reserved (must be 0x00)
*
* \retval void
*
****************************************************************************/
static void HAL_MDP_DSC_ENC_ContextInit(HAL_MDP_DSC_EncoderConfigType *pDSCEncConfig, uint32 uFlags)
{
   // Update the input configuration parameters in HAL_MDP_DSC_Info data-structure 

   // Data-Path Configuration Mode (SPLIT-MUX or DUAL-PANEL)

   // Detect whether the data-path is for SPLIT_MUX or DUAL_PANEL mode
   if(TRUE == pDSCEncConfig->bSplitMuxActive)
   {
      sDSCInfo.bSplitDisplayEnable     = TRUE;
      sDSCInfo.bHardSliceMuxEnable     = TRUE;
   }
   else
   {
      if(TRUE == pDSCEncConfig->bSplitDispActive)
      {
         sDSCInfo.bSplitDisplayEnable     = TRUE;
         sDSCInfo.bHardSliceMuxEnable     = FALSE;
      }
   }

   // Interface Mode (Video or Command)
   if(HAL_MDP_INTERFACE_MODE_VIDEO == pDSCEncConfig->eInterfaceMode)
   {
      sDSCInfo.bVideoMode  = TRUE;
   }

}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSC_EncoderCfg()
*/
/*!
* \brief
*     Initializes, prepares and configures DSC encoder parameters to the hardware block
*
* \param [in] eDSCEncEngineId    - none, 0, 1, 2 etc.
* \param [in] pDSCEncConfig      - Encoder basic configuration parameter information
* \param [in] uFlags             - Reserved
*
* \retval void
*
****************************************************************************/
static void HAL_MDP_DSC_EncoderCfg(HAL_MDP_DSCId eDSCEncEngineId, HAL_MDP_DSC_EncoderConfigType *pDSCEncConfig, uint32 uFlags)
{
   // uint32      uIndex   = 0x00000000;

   // Initialize the DSC Context
   HAL_MDP_DSC_ENC_ContextInit(pDSCEncConfig, uFlags);

   // Prepare the DSC Encoder Input Parameters
   HAL_MDP_DSC_ENC_PrepareRegInfo(&sDSCInfo, pDSCEncConfig);

   // Program the DSC Encoder Module's registers
   HAL_MDP_DSC_ENC_ProgramCfgReg(eDSCEncEngineId, &sDSCInfo, pDSCEncConfig);

   // Program the data-path configurations (MDP_DCE_SEL)
   HAL_MDP_DSC_ENC_DCESelCfg(pDSCEncConfig, &sDSCInfo);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSC_EncoderPrepPacket()
*/
/*!
* \brief
*     Prepares the 128-Byte PPS packet for the given encoder parameters
*
* \param [in/out] pDSCEncoderPacket    - Encoder configuration parameters and buffer pointer to return
*                                        128-Byte PPS packet
* \param [in] uFlags                   - Reserved
*
* \retval void
*
****************************************************************************/
static void HAL_MDP_DSC_EncoderPrepPacket(HAL_MDP_DSC_EncoderPacketType *pDSCEncoderPacket, uint32 uFlags)
{
   HAL_MDP_DSC_EncParamsType  *pEncParams = pDSCEncoderPacket->pEncoderParams;
   uint8                       uByteValue = 0x00;

   // 0 - DSC_VERSION_MAJOR [4], DSC_VERSION_MINOR [4]                         - Byte[0]
   uByteValue = 0x00;
   uByteValue = (uint8)((pEncParams->uDSCVersionMajor << 4) | (pEncParams->uDSCVersionMinor));
   pDSCEncoderPacket->pPacketData[0]   = (uint8)uByteValue;

   // 1 - PPS_IDENTIFIER [8]                                                   - Byte[1]
   pDSCEncoderPacket->pPacketData[1]   = (uint8)0x00;

   // 2 - RESERVED [8]                                                         - Byte[2]
   pDSCEncoderPacket->pPacketData[2]   = (uint8)0x00;

   // 3 - BITS_PER_COMPONENT [4], LINEBUF_DEPTH [4]                            - Byte[3]
   uByteValue  = 0x00;
   uByteValue  = (uint8)((pEncParams->uLineBufDepth)            |
                         (pEncParams->uBitsPerComponent << 4));
   pDSCEncoderPacket->pPacketData[3]   = (uint8)uByteValue;

   // 4 -RESERVED [2], BLK_PRED_EN [1], CONVERT_RGB [1], 
   //    ENABLE_422 [1], VBR_ENABLE [1], BITS_PER_PIXEL [2]                   - Byte[4]
   pDSCEncoderPacket->pPacketData[4]   = (uint8)(((pEncParams->uBitsPerPixel>>8) & 0x3)    |
                                                 (pEncParams->bVBREnable          << 2)    |
                                                 (pEncParams->bEnable422          << 3)    |
                                                 (pEncParams->bConvertRGB         << 4)    |
                                                 (pEncParams->bBlockPredEnable    << 5) );

   // 5 - BITS_PER_PIXEL [8]                                                   - Byte[5]
   pDSCEncoderPacket->pPacketData[5]   = (uint8)(pEncParams->uBitsPerPixel << 4);

   // 6 - PIC_HEIGHT [8]                                                       - Byte[6]
   pDSCEncoderPacket->pPacketData[6]   = (uint8)(pEncParams->uPicHeight >> 8);

   // 7 - PIC_HEIGHT [8]                                                       - Byte[7]
   pDSCEncoderPacket->pPacketData[7]   = (uint8)(pEncParams->uPicHeight);

   // 8 - PIC_WIDTH [8]                                                        - Byte[8]
   pDSCEncoderPacket->pPacketData[8]   = (uint8)(pEncParams->uPicWidth >> 8);

   // 9 - PIC_WIDTH [8]                                                        - Byte[9]
   pDSCEncoderPacket->pPacketData[9]   = (uint8)(pEncParams->uPicWidth);

   // 10 - SLICE_HEIGHT [8]                                                     - Byte[10]
   pDSCEncoderPacket->pPacketData[10]  = (uint8)(pEncParams->uSliceHeight >> 8);

   // 11 - SLICE_HEIGHT [8]                                                     - Byte[11]
   pDSCEncoderPacket->pPacketData[11]  = (uint8)(pEncParams->uSliceHeight);

   // 12 - SLICE_WIDTH [8]                                                      - Byte[12]
   pDSCEncoderPacket->pPacketData[12]  = (uint8)(pEncParams->uSliceWidth >> 8);

   // 13 - SLICE_WIDTH [8]                                                      - Byte[13]
   pDSCEncoderPacket->pPacketData[13]  = (uint8)(pEncParams->uSliceWidth);

   // 14 - CHUNK_SIZE [8]                                                       - Byte[14]
   pDSCEncoderPacket->pPacketData[14]  = (uint8)(pEncParams->uChunkSize >> 8);

   // 15 - CHUNK_SIZE [8]                                                       - Byte[15]
   pDSCEncoderPacket->pPacketData[15]  = (uint8)(pEncParams->uChunkSize);

   // 16 - RESERVED [6], INIT_XMIT_DELAY [2]                                    - Byte[16]
   pDSCEncoderPacket->pPacketData[16]  = (uint8)(pEncParams->uInitialXmitDelay >> 8);

   // 17 - INIT_XMIT_DELAY [8]                                                  - Byte[17]
   pDSCEncoderPacket->pPacketData[17]  = (uint8)(pEncParams->uInitialXmitDelay);

   // 18 - INIT_DEC_DELAY [8]                                                   - Byte[18]
   pDSCEncoderPacket->pPacketData[18]  = (uint8)(pEncParams->uInitialDecDelay >> 8);

   // 19 - INIT_DEC_DELAY [8]                                                   - Byte[19]
   pDSCEncoderPacket->pPacketData[19]  = (uint8)(pEncParams->uInitialDecDelay);

   // 20 - RESERVED [8]                                                         - Byte[20]
   pDSCEncoderPacket->pPacketData[20]  = 0x00;

   // 21 - RESERVED [2], INIT_SCALE_VALUE [6]                                   - Byte[21]
   pDSCEncoderPacket->pPacketData[21]  = (uint8)(pEncParams->uInitialScaleValue);

   // 22 - SCALE_INC_INTERVAL [8]                                               - Byte[22]
   pDSCEncoderPacket->pPacketData[22]  = (uint8)(pEncParams->uScaleIncInterval >> 8);

   // 23 - SCALE_INC_INTERVAL [8]                                               - Byte[23]
   pDSCEncoderPacket->pPacketData[23]  = (uint8)(pEncParams->uScaleIncInterval);

   // 24 - RESERVED [4], SCALE_DEC_INTERVAL [4]                                 - Byte[24]
   pDSCEncoderPacket->pPacketData[24]  = (uint8)(pEncParams->uScaleDecInterval >> 8);

   // 25 - SCALE_DEC_INTERVAL [4]                                               - Byte[25]
   pDSCEncoderPacket->pPacketData[25]  = (uint8)(pEncParams->uScaleDecInterval & 0xFF);

   // 26 - RESERVED [8]                                                         - Byte[26]
   pDSCEncoderPacket->pPacketData[26]  = 0x00;

   // 27 - RESERVED [3], FIRST_LINE_BPG_OFFSET [5]                              - Byte[27]
   pDSCEncoderPacket->pPacketData[27]  = (uint8)(pEncParams->uFirstLineBpgOffset);

   // 28 - NFL_BPG_OFFSET [8]                                                   - Byte[28]
   pDSCEncoderPacket->pPacketData[28]  = (uint8)(pEncParams->uNflBpgOffset >> 8);

   // 29 - NFL_BPG_OFFSET [8]                                                   - Byte[29]
   pDSCEncoderPacket->pPacketData[29]  = (uint8)(pEncParams->uNflBpgOffset);

   // 30 - SLICE_BPG_OFFSET [8]                                                 - Byte[30]
   pDSCEncoderPacket->pPacketData[30]  = (uint8)(pEncParams->uSliceBpgOffset >> 8);

   // 31 - SLICE_BPG_OFFSET [8]                                                 - Byte[31]
   pDSCEncoderPacket->pPacketData[31]  = (uint8)(pEncParams->uSliceBpgOffset);

   // 32 - INITIAL_OFFSET [8]                                                   - Byte[32]
   pDSCEncoderPacket->pPacketData[32]  = (uint8)(pEncParams->uInitialOffset >> 8);

   // 33 - INITIAL_OFFSET [8]                                                   - Byte[33]
   pDSCEncoderPacket->pPacketData[33]  = (uint8)(pEncParams->uInitialOffset);

   // 34 - FINAL_OFFSET [8]                                                     - Byte[34]
   pDSCEncoderPacket->pPacketData[34]  = (uint8)(pEncParams->uFinalOffset >> 8);

   // 35 - FINAL_OFFSET [8]                                                     - Byte[35]
   pDSCEncoderPacket->pPacketData[35]  = (uint8)(pEncParams->uFinalOffset);

   // 36 - RESERVED [3], FLATNESS_MIN_QP [5]                                    - Byte[36]
   pDSCEncoderPacket->pPacketData[36]  = (uint8)(pEncParams->uFlatnessMinQp);

   // 37 - RESERVED [3], FLATNESS_MAX_QP [5]                                    - Byte[37]
   pDSCEncoderPacket->pPacketData[37]  = (uint8)(pEncParams->uFlatnessMaxQp);

   // 38 - RC_MODEL_SIZE [8]                                                    - Byte[38]
   pDSCEncoderPacket->pPacketData[38]  = (uint8)(pEncParams->uRcModelSize >> 8);

   // 39 - RC_MODEL_SIZE [8]                                                    - Byte[39]
   pDSCEncoderPacket->pPacketData[39]  = (uint8)(pEncParams->uRcModelSize);

   // 40 - RESERVED [4], RC_EDGE_FACTOR [4]                                     - Byte[40]
   pDSCEncoderPacket->pPacketData[40]  = (uint8)(pEncParams->uRcEdgeFactor);

   // 41 - RESERVED [3], RC_QUANT_INCR_LIMIT0 [5]                               - Byte[41]
   pDSCEncoderPacket->pPacketData[41]  = (uint8)(pEncParams->uRcQuantIncrLimit0);

   // 42 - RESERVED [3], RC_QUANT_INCR_LIMIT1 [5]                               - Byte[42]
   pDSCEncoderPacket->pPacketData[42]  = (uint8)(pEncParams->uRcQuantIncrLimit1);

   // 43 - RC_TGT_OFFSET_HI [4], RC_TGT_OFFSET_LO [4]                           - Byte[43]
   pDSCEncoderPacket->pPacketData[43]  = (uint8)((pEncParams->uRcTgtOffsetLo)        | 
                                                 (pEncParams->uRcTgtOffsetHi << 4));

   // 44 - RC_BUF_THRESH[0]                                                     - Byte[44]
   pDSCEncoderPacket->pPacketData[44]  = (uint8)(pEncParams->uRcBufThresh[0]);

   // 45 - RC_BUF_THRESH[1]                                                     - Byte[45]
   pDSCEncoderPacket->pPacketData[45]  = (uint8)(pEncParams->uRcBufThresh[1]);

   // 46 - RC_BUF_THRESH[2]                                                     - Byte[46]
   pDSCEncoderPacket->pPacketData[46]  = (uint8)(pEncParams->uRcBufThresh[2]);

   // 47 - RC_BUF_THRESH[3]                                                     - Byte[47]
   pDSCEncoderPacket->pPacketData[47]  = (uint8)(pEncParams->uRcBufThresh[3]);

   // 48 - RC_BUF_THRESH[4]                                                     - Byte[48]
   pDSCEncoderPacket->pPacketData[48]  = (uint8)(pEncParams->uRcBufThresh[4]);

   // 49 - RC_BUF_THRESH[5]                                                     - Byte[49]
   pDSCEncoderPacket->pPacketData[49]  = (uint8)(pEncParams->uRcBufThresh[5]);

   // 50 - RC_BUF_THRESH[6]                                                     - Byte[50]
   pDSCEncoderPacket->pPacketData[50]  = (uint8)(pEncParams->uRcBufThresh[6]);

   // 51 - RC_BUF_THRESH[7]                                                     - Byte[51]
   pDSCEncoderPacket->pPacketData[51]  = (uint8)(pEncParams->uRcBufThresh[7]);

   // 52 - RC_BUF_THRESH[8]                                                     - Byte[52]
   pDSCEncoderPacket->pPacketData[52]  = (uint8)(pEncParams->uRcBufThresh[8]);

   // 53 - RC_BUF_THRESH[9]                                                     - Byte[53]
   pDSCEncoderPacket->pPacketData[53]  = (uint8)(pEncParams->uRcBufThresh[9]);

   // 54 - RC_BUF_THRESH[10]                                                    - Byte[54]
   pDSCEncoderPacket->pPacketData[54]  = (uint8)(pEncParams->uRcBufThresh[10]);

   // 55 - RC_BUF_THRESH[11]                                                    - Byte[55]
   pDSCEncoderPacket->pPacketData[55]  = (uint8)(pEncParams->uRcBufThresh[11]);

   // 56 - RC_BUF_THRESH[12]                                                    - Byte[56]
   pDSCEncoderPacket->pPacketData[56]  = (uint8)(pEncParams->uRcBufThresh[12]);

   // 57 - RC_BUF_THRESH[13]                                                    - Byte[57]
   pDSCEncoderPacket->pPacketData[57]  = (uint8)(pEncParams->uRcBufThresh[13]);

   // 58 - RC_RANGE_PARAMS[0]                                                   - Byte[58]
   pDSCEncoderPacket->pPacketData[58]  = (uint8)((pEncParams->uRangeMinQp[0] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[0] >> 2) & 0x07));

   // 59 - RC_RANGE_PARAMS[0]                                                   - Byte[59]
   pDSCEncoderPacket->pPacketData[59]  = (uint8)((pEncParams->uRangeMaxQp[0] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[0] & 0x3F) );

   // 60 - RC_RANGE_PARAMS[1]                                                   - Byte[60]
   pDSCEncoderPacket->pPacketData[60]  = (uint8)((pEncParams->uRangeMinQp[1] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[1] >> 2) & 0x07));

   // 61 - RC_RANGE_PARAMS[1]                                                   - Byte[61]
   pDSCEncoderPacket->pPacketData[61]  = (uint8)((pEncParams->uRangeMaxQp[1] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[1] & 0x3F));

   // 62 - RC_RANGE_PARAMS[2]                                                   - Byte[62]
   pDSCEncoderPacket->pPacketData[62]  = (uint8)((pEncParams->uRangeMinQp[2] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[2] >> 2) & 0x07));

   // 63 - RC_RANGE_PARAMS[2]                                                   - Byte[63]
   pDSCEncoderPacket->pPacketData[63]  = (uint8)((pEncParams->uRangeMaxQp[2] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[2] & 0x3F));

   // 64 - RC_RANGE_PARAMS[3]                                                   - Byte[64]
   pDSCEncoderPacket->pPacketData[64]  = (uint8)((pEncParams->uRangeMinQp[3] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[3] >> 2) & 0x07));

   // 65 - RC_RANGE_PARAMS[3]                                                   - Byte[65]
   pDSCEncoderPacket->pPacketData[65]  = (uint8)((pEncParams->uRangeMaxQp[3] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[3] & 0x3F));

   // 66 - RC_RANGE_PARAMS[4]                                                   - Byte[66]
   pDSCEncoderPacket->pPacketData[66]  = (uint8)((pEncParams->uRangeMinQp[4] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[4] >> 2) & 0x07));

   // 67 - RC_RANGE_PARAMS[4]                                                   - Byte[67]
   pDSCEncoderPacket->pPacketData[67]  = (uint8)((pEncParams->uRangeMaxQp[4] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[4] & 0x3F));

   // 68 - RC_RANGE_PARAMS[5]                                                   - Byte[68]
   pDSCEncoderPacket->pPacketData[68]  = (uint8)((pEncParams->uRangeMinQp[5] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[5] >> 2) & 0x07));

   // 69 - RC_RANGE_PARAMS[5]                                                   - Byte[69]
   pDSCEncoderPacket->pPacketData[69]  = (uint8)((pEncParams->uRangeMaxQp[5] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[5] & 0x3F));

   // 70 - RC_RANGE_PARAMS[6]                                                   - Byte[70]
   pDSCEncoderPacket->pPacketData[70]  = (uint8)((pEncParams->uRangeMinQp[6] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[6] >> 2) & 0x07));

   // 71 - RC_RANGE_PARAMS[6]                                                   - Byte[71]
   pDSCEncoderPacket->pPacketData[71]  = (uint8)((pEncParams->uRangeMaxQp[6] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[6] & 0x3F));

   // 72 - RC_RANGE_PARAMS[7]                                                   - Byte[72]
   pDSCEncoderPacket->pPacketData[72]  = (uint8)((pEncParams->uRangeMinQp[7] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[7] >> 2) & 0x07));

   // 73 - RC_RANGE_PARAMS[7]                                                   - Byte[73]
   pDSCEncoderPacket->pPacketData[73]  = (uint8)((pEncParams->uRangeMaxQp[7] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[7] & 0x3F));

   // 74 - RC_RANGE_PARAMS[8]                                                   - Byte[74]
   pDSCEncoderPacket->pPacketData[74]  = (uint8)((pEncParams->uRangeMinQp[8] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[8] >> 2) & 0x07));

   // 75 - RC_RANGE_PARAMS[8]                                                   - Byte[75]
   pDSCEncoderPacket->pPacketData[75]  = (uint8)((pEncParams->uRangeMaxQp[8] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[8] & 0x3F));

   // 76 - RC_RANGE_PARAMS[9]                                                   - Byte[76]
   pDSCEncoderPacket->pPacketData[76]  = (uint8)((pEncParams->uRangeMinQp[9] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[9] >> 2) & 0x07));

   // 77 - RC_RANGE_PARAMS[9]                                                   - Byte[77]
   pDSCEncoderPacket->pPacketData[77]  = (uint8)((pEncParams->uRangeMaxQp[9] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[9] & 0x3F));

   // 78 - RC_RANGE_PARAMS[10]                                                  - Byte[78]
   pDSCEncoderPacket->pPacketData[78]  = (uint8)((pEncParams->uRangeMinQp[10] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[10] >> 2) & 0x07));

   // 79 - RC_RANGE_PARAMS[10]                                                  - Byte[79]
   pDSCEncoderPacket->pPacketData[79]  = (uint8)((pEncParams->uRangeMaxQp[10] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[10] & 0x3F));

   // 80 - RC_RANGE_PARAMS[11]                                                  - Byte[80]
   pDSCEncoderPacket->pPacketData[80]  = (uint8)((pEncParams->uRangeMinQp[11] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[11] >> 2) & 0x07));

   // 81 - RC_RANGE_PARAMS[11]                                                  - Byte[81]
   pDSCEncoderPacket->pPacketData[81]  = (uint8)((pEncParams->uRangeMaxQp[11] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[11] & 0x3F));

   // 82 - RC_RANGE_PARAMS[12]                                                  - Byte[82]
   pDSCEncoderPacket->pPacketData[82]  = (uint8)((pEncParams->uRangeMinQp[12] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[12] >> 2) & 0x1F));

   // 83 - RC_RANGE_PARAMS[12]                                                  - Byte[83]
   pDSCEncoderPacket->pPacketData[83]  = (uint8)((pEncParams->uRangeMaxQp[12] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[12] & 0x3F));

   // 84 - RC_RANGE_PARAMS[13]                                                  - Byte[84]
   pDSCEncoderPacket->pPacketData[84]  = (uint8)((pEncParams->uRangeMinQp[13] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[13] >> 2) & 0x07));

   // 85 - RC_RANGE_PARAMS[13]                                                  - Byte[85]
   pDSCEncoderPacket->pPacketData[85]  = (uint8)((pEncParams->uRangeMaxQp[13] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[13] & 0x3F));

   // 86 - RC_RANGE_PARAMS[14]                                                  - Byte[86]
   pDSCEncoderPacket->pPacketData[86]  = (uint8)((pEncParams->uRangeMinQp[14] << 3)            |
                                                 ((pEncParams->uRangeMaxQp[14] >> 2) & 0x07));

   // 87 - RC_RANGE_PARAMS[14]                                                  - Byte[87]
   pDSCEncoderPacket->pPacketData[87]  = (uint8)((pEncParams->uRangeMaxQp[14] << 6)         |
                                                 (pEncParams->iRangeBpgOffset[14] & 0x3F));

   pDSCEncoderPacket->pPacketData[88]  = 0x00;    // RESERVED[0]                                                          - Byte[88]
   pDSCEncoderPacket->pPacketData[89]  = 0x00;    // RESERVED[0]                                                          - Byte[89]
   pDSCEncoderPacket->pPacketData[90]  = 0x00;    // RESERVED[0]                                                          - Byte[90]
   pDSCEncoderPacket->pPacketData[91]  = 0x00;    // RESERVED[0]                                                          - Byte[91]
   pDSCEncoderPacket->pPacketData[92]  = 0x00;    // RESERVED[0]                                                          - Byte[92]
   pDSCEncoderPacket->pPacketData[93]  = 0x00;    // RESERVED[0]                                                          - Byte[93]
   pDSCEncoderPacket->pPacketData[94]  = 0x00;    // RESERVED[0]                                                          - Byte[94]
   pDSCEncoderPacket->pPacketData[95]  = 0x00;    // RESERVED[0]                                                          - Byte[95]
   pDSCEncoderPacket->pPacketData[96]  = 0x00;    // RESERVED[0]                                                          - Byte[96]
   pDSCEncoderPacket->pPacketData[97]  = 0x00;    // RESERVED[0]                                                          - Byte[97]
   pDSCEncoderPacket->pPacketData[98]  = 0x00;    // RESERVED[0]                                                          - Byte[98]
   pDSCEncoderPacket->pPacketData[99]  = 0x00;    // RESERVED[0]                                                          - Byte[99]
   pDSCEncoderPacket->pPacketData[100] = 0x00;    // RESERVED[0]                                                          - Byte[100]
   pDSCEncoderPacket->pPacketData[101] = 0x00;    // RESERVED[0]                                                          - Byte[101]
   pDSCEncoderPacket->pPacketData[102] = 0x00;    // RESERVED[0]                                                          - Byte[102]
   pDSCEncoderPacket->pPacketData[103] = 0x00;    // RESERVED[0]                                                          - Byte[103]
   pDSCEncoderPacket->pPacketData[104] = 0x00;    // RESERVED[0]                                                          - Byte[104]
   pDSCEncoderPacket->pPacketData[105] = 0x00;    // RESERVED[0]                                                          - Byte[105]
   pDSCEncoderPacket->pPacketData[106] = 0x00;    // RESERVED[0]                                                          - Byte[106]
   pDSCEncoderPacket->pPacketData[107] = 0x00;    // RESERVED[0]                                                          - Byte[107]
   pDSCEncoderPacket->pPacketData[108] = 0x00;    // RESERVED[0]                                                          - Byte[108]
   pDSCEncoderPacket->pPacketData[109] = 0x00;    // RESERVED[0]                                                          - Byte[109]
   pDSCEncoderPacket->pPacketData[110] = 0x00;    // RESERVED[0]                                                          - Byte[110]
   pDSCEncoderPacket->pPacketData[111] = 0x00;    // RESERVED[0]                                                          - Byte[111]
   pDSCEncoderPacket->pPacketData[112] = 0x00;    // RESERVED[0]                                                          - Byte[112]
   pDSCEncoderPacket->pPacketData[113] = 0x00;    // RESERVED[0]                                                          - Byte[113]
   pDSCEncoderPacket->pPacketData[114] = 0x00;    // RESERVED[0]                                                          - Byte[114]
   pDSCEncoderPacket->pPacketData[115] = 0x00;    // RESERVED[0]                                                          - Byte[115]
   pDSCEncoderPacket->pPacketData[116] = 0x00;    // RESERVED[0]                                                          - Byte[116]
   pDSCEncoderPacket->pPacketData[117] = 0x00;    // RESERVED[0]                                                          - Byte[117]
   pDSCEncoderPacket->pPacketData[118] = 0x00;    // RESERVED[0]                                                          - Byte[118]
   pDSCEncoderPacket->pPacketData[119] = 0x00;    // RESERVED[0]                                                          - Byte[119]
   pDSCEncoderPacket->pPacketData[120] = 0x00;    // RESERVED[0]                                                          - Byte[120]
   pDSCEncoderPacket->pPacketData[121] = 0x00;    // RESERVED[0]                                                          - Byte[121]
   pDSCEncoderPacket->pPacketData[122] = 0x00;    // RESERVED[0]                                                          - Byte[122]
   pDSCEncoderPacket->pPacketData[123] = 0x00;    // RESERVED[0]                                                          - Byte[123]
   pDSCEncoderPacket->pPacketData[124] = 0x00;    // RESERVED[0]                                                          - Byte[124]
   pDSCEncoderPacket->pPacketData[125] = 0x00;    // RESERVED[0]                                                          - Byte[125]
   pDSCEncoderPacket->pPacketData[126] = 0x00;    // RESERVED[0]                                                          - Byte[126]
   pDSCEncoderPacket->pPacketData[127] = 0x00;    // RESERVED[0]                                                          - Byte[127]
}

/*------------------------------------------------------------------------
 * Private Function Definitions
 *------------------------------------------------------------------------ */

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSC_Setup()
*/
/*!
* \brief
*     Configures the control/data flow for the DSC encoder modules and also
*     configured the encoder configuration parameters to compress the pixel data
*
* \param [in] eDSCEncEngineId    - none, 0, 1, 2 etc.
* \param [in] pDSCConfig         - Basic configuration parameters for DSC encoder
* \param [in] uFlags             - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DSC_Setup(HAL_MDP_DSCId          eDSCEncEngineId,
                                     HAL_MDP_DSCConfigType *pDSCConfig,
                                     uint32                 uFlags)
{
   HAL_MDSS_ErrorType   eStatus        = HAL_MDSS_STATUS_SUCCESS;

   if(NULL != pDSCConfig->pDSCEncoderCfg)
   {
      HAL_MDP_DSC_EncoderCfg(eDSCEncEngineId, pDSCConfig->pDSCEncoderCfg, 0x00);
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSC_GetPropery()
*/
/*!
* \brief
*     Used to extract the various types of information from DSC encoder module
*
* \param [in]     eDSCEncEngineId    - none, 0, 1, 2 etc.
* \param [in/out] pDSCInfo           - Input inforamtion to extract the required data and
*                                      Output information prepared by HAL driver as response to 
*                                      the requested information.
* \param [in]     uFlags             - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DSC_GetPropery(HAL_MDP_DSCId          eDSCEncEngineId,
                                          HAL_MDP_DSCInfoType   *pDSCInfo,
                                          uint32                 uFlags)
{
   HAL_MDSS_ErrorType   eStatus        = HAL_MDSS_STATUS_SUCCESS;

   if(NULL != pDSCInfo->pDSCEncoderCfg)
   {
      // Initialize the DSC Context
      HAL_MDP_DSC_ENC_ContextInit(pDSCInfo->pDSCEncoderCfg, uFlags);

      // Prepare the DSC Encoder Input Parameters
      HAL_MDP_DSC_ENC_PrepareRegInfo(&sDSCInfo, pDSCInfo->pDSCEncoderCfg);
   }

   if(NULL != pDSCInfo->pDSCEncoderPacket)
   {
      // Prepare 128 Bytes Packet
      HAL_MDP_DSC_EncoderPrepPacket(pDSCInfo->pDSCEncoderPacket, 0x00);
   }

   return eStatus;
}

#ifdef __cplusplus
}
#endif
