// NOWHINE ENTIRE FILE 
//-------------------------------------------------------------------------
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------


#ifndef __TF_REGISTERS_H__
#define __TF_REGISTERS_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "CommonDefs.h"
#include <stdio.h>

typedef struct TF_REG_TAG
{

    // Where there is no valid data at the MCTFP input - this happens for example in the first frame
    // format: 1u
    FIELD_UINT invalidMctfpImage;

    // Disable generation of output indications (in full pass, output indications are usually disabled)
    // format: 1u
    FIELD_UINT disableOutputIndications;

    // Whether to use indications from lower resolutions (should be 0 for lowest resolution pass where there is no Indications input)
    // format: 1u
    FIELD_UINT useIndications;

    // Whether ghost detection takes Luma into account. When both disableLumaGhostDetection and disableChromaGhostDetection are ‘1’, ghost detection should be bypassed (setting FS and a3 either to indication values or to zeros – depending on indicationsDominateFsDecision flag). 
    // format: 1u
    FIELD_UINT disableLumaGhostDetection;

    // Whether ghost detection takes Chroma into account. When both disableLumaGhostDetection and disableChromaGhostDetection are ‘1’, ghost detection should be bypassed (setting FS and a3 either to indication values or to zeros – depending on indicationsDominateFsDecision flag). 
    // format: 1u
    FIELD_UINT disableChromaGhostDetection;

    // Determines whether to perform low pass filter on Luma of the difference image prior to using it in SAD calculations. When this flag is ‘1’ the blendingMode should be non-zero (namely, bypass of one of inputs to TFO or constant blending), thus effectively disabling all regular blending logic and freeing line buffers required for low pass filter on Luma.
    // format: 1u
    FIELD_UINT smearInputsForDecisions;

    // Whether to use Luma from ANRO input for decisions (i.e. SAD, noise threshold calculation) instead of Luma from IMG input
    // format: 1u
    FIELD_UINT useAnrForDecisions_Y;

    // Whether to use Chroma from ANRO input for decisions (i.e. SAD, noise threshold calculation) instead of Chroma from IMG input
    // format: 1u
    FIELD_UINT useAnrForDecisions_C;

    // Whether to enable Lens roll-off induced update to noise thresholds
    // format: 1u
    FIELD_UINT enableLNR;

    // Enables dependence of noise thresholds on luma (0 è power-saving mode: noise thresholds may depend only on chroma; if enableNoiseEstByChroma is also 0 – noise thresholds are constant). When disableLumaGhostDetection is '1', enableNoiseEstByLuma should be '0'
    // format: 1u
    FIELD_UINT enableNoiseEstByLuma;

    // Enables dependence of noise thresholds on chroma (0 è power-saving mode: noise thresholds may depend only on luma; if enableNoiseEstByLuma is also 0 – noise thresholds are constant). When disableChromaGhostDetection is '1', enableNoiseEstByChroma should be '0'
    // format: 1u
    FIELD_UINT enableNoiseEstByChroma;

    // Whether the padding in the anti-aliasing filter and Luma SAD filter is by reflection. When this flag is ‘0’ (this is the default value) the padding is done by duplication of the boundary pixel. When this flag is ‘1’ the padding is done by taking the value at a coordinate mirrored relative to boundary.
    // format: 1u
    FIELD_UINT paddingByReflection;

    // Window size for SAD Y calculation.
    // 0 = 7x5 window
    // 1 = 5x5 window
    // 2 = 3x3 window
    // 3 = 1x1 window
    // format: 2u
    FIELD_UINT sadYCalcMode;

    // Window size for SAD Cb and Cr calculations.
    // 0 = 3x3 window
    // 1 = 5x3 window
    // format: 1u
    FIELD_UINT isSadC5x3;

    // Determines format of values in LnrLutY:
    // 0 = 8uQ7
    // 1 = 8uQ6
    // format: 1u
    FIELD_UINT LnrLutShiftY;

    // Determines format of values in LnrLutC:
    // 0 = 8uQ7
    // 1 = 8uQ6
    // format: 1u
    FIELD_UINT LnrLutShiftC;

    // Whether working in DCI mode – in this mode fsToA4Maps are not used – instead the a4 is calculated as a4=a1*a3
    // format: 1u
    FIELD_UINT isDCI;

    // ‘1’ means indications directly affect FS decision by taking minimum of normal FS decision and the indications value (after possible update by outOfFramePixelsConfidence and after conversion to 6 bits). If this flag is ‘1’ and ghost detection is disabled (by setting both disableLumaGhostDetection and disableChromaGhostDetection to ‘1’), the FS decision is directly set to indications value (again, after possible update by outOfFramePixelsConfidence and after conversion to 6 bits). Can be ‘1’ only when useIndications is ‘1’
    // format: 1u
    FIELD_UINT indicationsDominateFsDecision;

    // Whether to apply peak removal on the “FS image”
    // format: 1u
    FIELD_UINT applyFsRankFilter;

    // Whether to apply low pass filter on the “FS image”
    // format: 1u
    FIELD_UINT applyFsLpf;

    // 0 = OOF indications come with MCTFP
    // 1 = OOF indications come with IMG
    // format: 1u
    FIELD_UINT takeOofIndFrom;

    // Forces same blending factor both for low and for high frequencies during blending. If set to ‘1’ disables LPF calculations in blending (saving also line buffers delay), disables a3 and a4 calculations
    // format: 1u
    FIELD_UINT isSameBlendingForAllFrequencies;

    // This shift is applied to fsDecisionParamsY_c2 and fsDecisionParamsY_c4 before decision thresholds calculations
    // format: 2u
    FIELD_UINT fsDecisionFreeParamShiftY;

    // This shift is applied to fsDecisionParamsC_c2 and fsDecisionParamsC_c4 before decision thresholds calculations
    // format: 2u
    FIELD_UINT fsDecisionFreeParamShiftC;

    // This value is used to update the indications in “out of frame” regions (i.e. regions going out of frame during global motion compensation) in case indicationsDominateFsDecision is ‘1’ 
    // format: 4u
    FIELD_UINT outOfFramePixelsConfidence;

    // 0 – if TFR results go to ICA2 (Video flow)
    // 1 – if TFR results go to ICA1 (Multi-frame flow)
    // format: 1u
    FIELD_UINT trDirection;

    // Enables cropper on ANRO and IMG before entering Ghost Detection stage. For simplicity it is assumed that when img_CropInEnable==’1’ or mctfp_CropInEnable==‘1’ the blendindMode should be 1, 2 or 3.
    // format: 1u
    FIELD_UINT img_CropInEnable;

    // Enables cropper on MCTFP before entering Ghost Detection stage. For simplicity it is assumed that when img_CropInEnable==’1’ or mctfp_CropInEnable==‘1’ the blendindMode should be 1, 2 or 3.
    // format: 1u
    FIELD_UINT mctfp_CropInEnable;

    // Multiplicative factor for decreasing the indication values (valid factor range is [0-16]). Value of 16 means no change to indications. This feature may be useful for example when Global Motion Compensation model was inaccurate.
    // format: 5u
    FIELD_UINT indicationsDecreaseFactor;

    // Whether to take “1-a” instead of “a” for a1 and a4 blending weights. The actual implementation is done more efficiently (using muxes), but the logical meaning is as above – namely: take only MCTFP input as output (instead of SpatialBlend result in regular mode) when FS is low and perform temporal blending when FS is high. 
    // format: 1u
    FIELD_UINT invertTemporalBlendingWeights;

    // 0 = regular blending
    // 1 = copy IMG to TFO
    // 2 = copy ANRO to TFO
    // 3 = copy MCTFP to TFO
    // 4 = use blending with constant factor between ANRO and MCTFP (this mode can not be used when invalidMctfpImage is '1'
    // 
    // Note that all modes except “0” are trivial blending modes (namely, pixel-wise constant blending or passthru of one of inputs). When in these modes the blender does not need any information from other sub-blocks of TF effectively allowing detached operation of Ghost Detection and Blending. Note also that when blendingMode != 0 and disableOutputIndications==1, all TF blocks except blender (and morphology) can be switched off.
    // format: 3u
    FIELD_UINT blendingMode;

    // Whether to apply dithering to Y channel of final TF output
    // format: 1u
    FIELD_UINT ditherY;

    // Whether to apply dithering to Cb channel of final TF output
    // format: 1u
    FIELD_UINT ditherCb;

    // Whether to apply dithering to Cr channel of final TF output
    // format: 1u
    FIELD_UINT ditherCr;

    // Horizontal cropping at TFO output:
    // 0 = assume no horizontal filter
    // 1 = assume horizontal filter of width 5
    // 2 = assume horizontal filter of width 13.
    // The need for horizontal cropping at left and/or right is inferred from CCIF metadata coming with ANRO image.
    // format: 2u
    FIELD_UINT TfOut_CropOutHorizAssumeSize;

    // Whether to override IMG input from ANRO - used for TF-Blend feature in Still use case
    // format: 1u
    FIELD_UINT useAnroAsImgInput;

    // Used for TF-Blend feature in Still use case. Whether to override MCTFP input from IMG. The “pixel valid” indications from ICA are also routed together with the data, so when useImgAsMctfpInput is ‘1’ the ICA1 “pixel valid” indications are routed to MCTFP. Note also that IMG can be routed to MCTFP even when MCTFP input does not get data from ICA2 (i.e. when only ANRO and IMG data are available) – in this case the unit should behave as if it gets data on MCTFP as usual.
    // format: 1u
    FIELD_UINT useImgAsMctfpInput;

    // Enable of TFIE module (handled by CCIF interface)
    // format: 1u
    FIELD_UINT morph_Erode_En;

    // 0 = input pass-through (no filter)
    // 1 = 5x5 window
    // 2 = 3x3 window
    // format: 2u
    FIELD_UINT Morph_Erode_Size;

    // Whether vertical cropping at TFIE output is enabled. When disabled there is no need to configure vertical cropping parameters for TFIE. Default is 0.
    // format: 1u
    FIELD_UINT Morph_Erode_CropOutVertEnable;

    // Vertical cropping start location (at the output of filter). This is the index of first valid output row. Possible values: 0 and 2.
    // format: 2u
    FIELD_UINT Morph_Erode_CropOutVertStart;

    // Vertical cropping end location (at the output of filter). This is the index of last valid output row.
    // format: 12u
    FIELD_UINT Morph_Erode_CropOutVertEnd;

    // Whether to remove first nibble [4bit] in the first byte of each input row (at the input of filter)
    // format: 1u
    FIELD_UINT Morph_Erode_CropInHorizLeft;

    // Whether to remove last nibble [4bit] in the last byte of each input row (at the input of filter)
    // format: 1u
    FIELD_UINT Morph_Erode_CropInHorizRight;

    // Enable of TFID module (handled by CCIF interface)
    // format: 1u
    FIELD_UINT morph_Dilate_En;

    // 0 = input pass-through (no filter)
    // 1 = 5x5 window
    // 2 = 3x3 window
    // format: 2u
    FIELD_UINT Morph_Dilate_Size;

    // Whether vertical cropping at TFID output is enabled. When disabled there is no need to configure vertical cropping parameters for TFID. Default is 0.
    // format: 1u
    FIELD_UINT Morph_Dilate_CropOutVertEnable;

    // Vertical cropping start location (at the output of filter). This is the index of first valid output row.
    // Possible values: 0 and 2.
    // format: 2u
    FIELD_UINT Morph_Dilate_CropOutVertStart;

    // Vertical cropping end location (at the output of filter). This is the index of last valid output row.
    // format: 12u
    FIELD_UINT Morph_Dilate_CropOutVertEnd;

    // Horizontal cropping start location (for “IMG crop”). This is the index of first valid output pixel.
    // format: 10u
    FIELD_UINT img_CropInHorizStart;

    // Horizontal cropping start location (for “MCTFP crop”). This is the index of first valid output pixel.
    // format: 10u
    FIELD_UINT mctfp_CropInHorizStart;

    // Horizontal cropping end location (for “IMG crop”). This is the index of last valid output pixel.
    // format: 10u
    FIELD_UINT img_CropInHorizEnd;

    // Horizontal cropping end location (for “MCTFP crop”). This is the index of last valid output pixel.
    // format: 10u
    FIELD_UINT mctfp_CropInHorizEnd;

    // Initialization of horizontal part of distance to optical center [21sQ15]
    // format: 21sQ15
    FIELD_INT LnrStartIdxH;

    // Whether to remove first nibble [4bit] in the first byte of each input row (at the input of filter)
    // format: 1u
    FIELD_UINT indUpscale_CropInHorizLeft;

    // Whether to remove last nibble [4bit] in the last byte of each input row (at the input of filter)
    // format: 1u
    FIELD_UINT indUpscale_CropInHorizRight;

    // Number of output pixels (after upscaling) to remove at the beginning of each row (at the output of filter). Should be even due to assumption of 2x2 block processing at main resolution. Allowed values: 0, 2, 4
    // format: 3u
    FIELD_UINT indUpscale_CropOutHorizLeft;

    // Number of output pixels (after upscaling) to remove at the end of each row (at the output of filter). Should be even due to assumption of 2x2 block processing at main resolution. Allowed values: 0, 2, 4
    // format: 3u
    FIELD_UINT indUpscale_CropOutHorizRight;

    // Number of rows of output pixels (after upscaling) to remove at the top (at the output of filter). Should be even due to assumption of 2x2 block processing at main resolution. Allowed values: 0, 2, 4
    // format: 3u
    FIELD_UINT indUpscale_CropOutVertTop;

    // Number of rows of output pixels (after upscaling) to remove at the bottom (at the output of filter). Should be even due to assumption of 2x2 block processing at main resolution. Allowed values: 0, 2, 4 
    // format: 3u
    FIELD_UINT indUpscale_CropOutVertBottom;

    // Enable cropping at IND11 output
    // format: 1u
    FIELD_UINT OutIndications_CropEnable;

    // Horizontal cropping start location (at the IND11 output). This is the index of first valid output pixel.
    // format: 9u
    FIELD_UINT OutIndications_CropOutHorizStart;

    // Horizontal cropping end location (at the IND11 output). This is the index of last valid output pixel.
    // format: 10u
    FIELD_UINT OutIndications_CropOutHorizEnd;

    // Seed used for pseudo-random number generator - part of dithering mechanism
    // format: 30u
    FIELD_UINT prngSeed;

    // Whether to enable warping transform refinement
    // format: 1u
    FIELD_UINT trEnable;

    // Minimal FS to contribute to transform refinement
    // format: 6u
    FIELD_UINT trFsThreshold;

    // Dead-zone for transform refinement results. 0 is equivalent to no dead-zone.
    // format: 2u
    FIELD_UINT trDeadZone;

    // Minimal number of samples in a block to make its refinement valid
    // format: 18u
    FIELD_UINT trCountThreshold;

    // X coordinate of first block
    // format: 10u
    FIELD_UINT trStartX;

    // X number of blocks (1…4)
    // format: 3u
    FIELD_UINT trBlockNumX;

    // Width of blocks (valid range: [59,334], should be less than or equal to maximal stripe width - 2)
    // format: 10u
    FIELD_UINT trBlockSizeX;

    // Vertical cropping start location (for “IMG crop”). This is the index of first valid output row.
    // format: 14u
    FIELD_UINT img_CropInVertStart;

    // Vertical cropping start location (for “MCTFP crop”). This is the index of first valid output row.
    // format: 14u
    FIELD_UINT mctfp_CropInVertStart;

    // Vertical cropping end location (for “IMG crop”). This is the index of last valid output row.
    // format: 14u
    FIELD_UINT img_CropInVertEnd;

    // Vertical cropping end location (for “MCTFP crop”). This is the index of last valid output row.
    // format: 14u
    FIELD_UINT mctfp_CropInVertEnd;

    // Initialization of vertical part of distance to optical center [21sQ15]
    // format: 21sQ15
    FIELD_INT LnrStartIdxV;

    // Horizontal step between neighbor pixels – this is the increment for LnrStartIdxH. SW should configure this parameter so that LnrStartIdxH + LnrScaleH*frameW can be represented by 21 bit signed. [16uQ15]
    // format: 16uQ15
    FIELD_UINT LnrScaleH;

    // Vertical step between neighbor rows – this is the increment for LnrStartIdxV. SW should configure this parameter so that LnrStartIdxV + LnrScaleV*frameH can be represented by 21 bit signed. [16uQ15]
    // format: 16uQ15
    FIELD_UINT LnrScaleV;

    // Vertical cropping start location (at the IND11 output). This is the index of first valid output row.
    // format: 9u
    FIELD_UINT OutIndications_CropOutVertStart;

    // Vertical cropping end location (at the IND11 output). This is the index of last valid output row.
    // format: 14u
    FIELD_UINT OutIndications_CropOutVertEnd;

    // Y coordinate of first block
    // format: 14u
    FIELD_UINT trStartY;

    // Y number of blocks (1…31)
    // format: 5u
    FIELD_UINT trBlockNumY;

    // Height of blocks (>=32; must be even). 
    // format: 9u
    FIELD_UINT trBlockSizeY;

    // Contribution of luma to luma noise threshold, sampled at 17 points
    // format: 12uQ2
    FIELD_UINT noiseParamsY_YTb[17];

    // Contribution of luma to Cb noise threshold, sampled at 17 points
    // format: 10u
    FIELD_UINT noiseParamsCb_YTb[17];

    // Contribution of luma to Cr noise threshold, sampled at 17 points
    // format: 10u
    FIELD_UINT noiseParamsCr_YTb[17];

    // Contribution of chroma to luma noise threshold, specified for 8 extreme points
    // format: 11s
    FIELD_INT noiseParamsY_CTb[8];

    // Contribution of chroma to Cb noise threshold, specified for 8 extreme points
    // format: 11s
    FIELD_INT noiseParamsCb_CTb[8];

    // Contribution of chroma to Cr noise threshold, specified for 8 extreme points
    // format: 11s
    FIELD_INT noiseParamsCr_CTb[8];

    // Limit for chroma samples that affect luma noise threshold
    // format: 9u
    FIELD_UINT noiseParamsY_UVlimit;

    // Limit for chroma samples that affect Cb noise threshold
    // format: 9u
    FIELD_UINT noiseParamsCb_UVlimit;

    // Limit for chroma samples that affect Cr noise threshold
    // format: 9u
    FIELD_UINT noiseParamsCr_UVlimit;

    // Upper limit of luma noise threshold
    // format: 12uQ2
    FIELD_UINT noiseParamsY_TopLim;

    // Upper limit of Cb noise threshold
    // format: 10u
    FIELD_UINT noiseParamsCb_TopLim;

    // Upper limit of Cr noise threshold
    // format: 10u
    FIELD_UINT noiseParamsCr_TopLim;

    // Lower limit of luma noise threshold
    // format: 12uQ2
    FIELD_UINT noiseParamsY_BotLim;

    // Lower limit of Cb noise threshold
    // format: 10u
    FIELD_UINT noiseParamsCb_BotLim;

    // Lower limit of Cr noise threshold
    // format: 10u
    FIELD_UINT noiseParamsCr_BotLim;

    // The LUT of scale factors to be applied to Luma noise thresholds (this LUT is accessed using MSBs of distance from optical center)
    // format: 8u
    FIELD_UINT LnrLutY[16];

    // The LUT of scale factors to be applied to Chroma noise thresholds (this LUT is accessed using MSBs of distance from optical center)
    // format: 8u
    FIELD_UINT LnrLutC[16];

    // Multiplication factor used in Luma decision thresholds calculation to form weak threshold (multiplies noiseThr) [5uQ4]
    // format: 5uQ4
    FIELD_UINT fsDecisionParamsY_c1[9];

    // Offset used in Luma decision thresholds calculation to form weak threshold [10uQ4]
    // format: 10uQ4
    FIELD_UINT fsDecisionParamsY_c2[9];

    // Multiplication factor used in Luma decision thresholds calculation to form strong threshold (multiplies noiseThr) [6uQ4]
    // format: 6uQ4
    FIELD_UINT fsDecisionParamsY_c3[9];

    // Offset used in Luma decision thresholds calculation to form strong threshold [11uQ4]
    // format: 11uQ4
    FIELD_UINT fsDecisionParamsY_c4[9];

    // Multiplication factor used in Luma decision thresholds calculation to form weak threshold (multiplies noiseThr). Used in case of out-of-frame Luma pixel. [5uQ4]
    // format: 5uQ4
    FIELD_UINT fsDecisionParamsOofY_c1;

    // Offset used in Luma decision thresholds calculation to form weak threshold. Used in case of out-of-frame Luma pixel. [10uQ4]
    // format: 10uQ4
    FIELD_UINT fsDecisionParamsOofY_c2;

    // Multiplication factor used in Luma decision thresholds calculation to form strong threshold (multiplies noiseThr). Used in case of out-of-frame Luma pixel. [6uQ4]
    // format: 6uQ4
    FIELD_UINT fsDecisionParamsOofY_c3;

    // Offset used in Luma decision thresholds calculation to form strong threshold. Used in case of out-of-frame Luma pixel. [11uQ4]
    // format: 11uQ4
    FIELD_UINT fsDecisionParamsOofY_c4;

    // Multiplication factor used in Chroma decision thresholds calculation to form weak threshold (multiplies noiseThr) [5uQ4]
    // format: 5uQ4
    FIELD_UINT fsDecisionParamsC_c1[9];

    // Offset used in Chroma decision thresholds calculation to form weak threshold [10uQ4]
    // format: 10uQ4
    FIELD_UINT fsDecisionParamsC_c2[9];

    // Multiplication factor used in Chroma decision thresholds calculation to form strong threshold (multiplies noiseThr) [6uQ4]
    // format: 6uQ4
    FIELD_UINT fsDecisionParamsC_c3[9];

    // Offset used in Chroma decision thresholds calculation to form strong threshold [11uQ4]
    // format: 11uQ4
    FIELD_UINT fsDecisionParamsC_c4[9];

    // Multiplication factor used in Chroma decision thresholds calculation to form weak threshold (multiplies noiseThr). Used in case of out-of-frame Luma pixel. [5uQ4]
    // format: 5uQ4
    FIELD_UINT fsDecisionParamsOofC_c1;

    // Offset used in Chroma decision thresholds calculation to form weak threshold. Used in case of out-of-frame Luma pixel. [10uQ4]
    // format: 10uQ4
    FIELD_UINT fsDecisionParamsOofC_c2;

    // Multiplication factor used in Chroma decision thresholds calculation to form strong threshold (multiplies noiseThr). Used in case of out-of-frame Luma pixel. [6uQ4]
    // format: 6uQ4
    FIELD_UINT fsDecisionParamsOofC_c3;

    // Offset used in Chroma decision thresholds calculation to form strong threshold. Used in case of out-of-frame Luma pixel. [11uQ4]
    // format: 11uQ4
    FIELD_UINT fsDecisionParamsOofC_c4;

    // Scale factor used for determining weak Luma decision threshold for a3 based on weak decision threshold for FS [5uQ4]
    // format: 5uQ4
    FIELD_UINT a3T1ScaleY;

    // Scale factor used for determining weak Chroma decision threshold for a3 based on weak decision threshold for FS [5uQ4]
    // format: 5uQ4
    FIELD_UINT a3T1ScaleC;

    // Scale factor used for determining strong Luma decision threshold for a3 based on strong decision threshold for FS [6uQ4]
    // format: 6uQ4
    FIELD_UINT a3T2ScaleY;

    // Scale factor used for determining strong Chroma decision threshold for a3 based on strong decision threshold for FS [6uQ4]
    // format: 6uQ4
    FIELD_UINT a3T2ScaleC;

    // Offset used for determining weak Luma decision threshold for a3 based of weak decision threshold for FS
    // format: 10uQ5
    FIELD_UINT a3T1OffsY;

    // Offset used for determining weak Chroma decision threshold for a3 based of weak decision threshold for FS
    // format: 10uQ5
    FIELD_UINT a3T1OffsC;

    // Offset used for determining strong Luma decision threshold for a3 based on strong decision threshold for FS
    // format: 12uQ5
    FIELD_UINT a3T2OffsY;

    // Offset used for determining strong Chroma decision threshold for a3 based on strong decision threshold for FS
    // format: 12uQ5
    FIELD_UINT a3T2OffsC;

    // Lower limit for a2 blending factor calculation in Luma (this value may never be reached – depending on a2SlopeY)
    // format: 8uQ8
    FIELD_UINT a2MinY;

    // Upper limit for a2 blending factor calculation (used where FS=0) in Luma
    // format: 8uQ8
    FIELD_UINT a2MaxY;

    // Lower limit for a2 blending factor calculation in Chroma (this value may never be reached – depending on a2SlopeC)
    // format: 8uQ8
    FIELD_UINT a2MinC;

    // Upper limit for a2 blending factor calculation (used where FS=0) in Chroma
    // format: 8uQ8
    FIELD_UINT a2MaxC;

    // Slope as a function of FS (a2 starts from a2MaxY at FS=0 and goes down with slope a2SlopeY as FS increases). Used for Luma. [8uQ5]
    // format: 8uQ5
    FIELD_UINT a2SlopeY;

    // Slope as a function of FS (a2 starts from a2MaxC at FS=0 and goes down with slope a2SlopeC as FS increases). Used for Chroma. [8uQ5]
    // format: 8uQ5
    FIELD_UINT a2SlopeC;

    // Sparse mapping between FS values and a1 blending factor for Luma
    // format: 8uQ8
    FIELD_UINT fsToA1MapY[9];

    // Sparse mapping between FS values and a1 blending factor for Chroma
    // format: 8uQ8
    FIELD_UINT fsToA1MapC[9];

    // Sparse mapping between FS values and a4 blending factor for Luma
    // format: 8uQ8
    FIELD_UINT fsToA4MapY[9];

    // Sparse mapping between FS values and a4 blending factor for Chroma
    // format: 8uQ8
    FIELD_UINT fsToA4MapC[9];

    // When blendingMode==4 this blending factor is used for Luma channel blending
    // format: 8u
    FIELD_UINT constantBlendingFactorY;

    // When blendingMode==4 this blending factor is used for Chroma channels blending
    // format: 8u
    FIELD_UINT constantBlendingFactorC;

    // Enable of TF module (including upscaler). When ‘0’, CCIF interface should forward ANRO input to TFO output and should forward IND14 input to IND11 output
    // format: 1u
    FIELD_UINT en;

} TF_REG;

// ############ Functions ############
int32_t Validate_TF_REG( TF_REG* regStruct  );
void SetDefaultVal_TF_REG( TF_REG* regStruct );
// ###################################
#ifdef __cplusplus
}
#endif


#endif //__TF_REGISTERS_H__

