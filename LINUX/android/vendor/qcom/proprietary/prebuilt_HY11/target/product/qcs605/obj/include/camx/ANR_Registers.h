// NOWHINE ENTIRE FILE 
//-------------------------------------------------------------------------
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------


#ifndef __ANR_REGISTERS_H__
#define __ANR_REGISTERS_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "CommonDefs.h"
#include <stdio.h>

typedef struct TOP_REG_TAG
{

    // Enable anr for current pass
    // format: 1u
    FIELD_UINT EN;

} TOP_REG;

// ############ Functions ############
int32_t Validate_TOP_REG( TOP_REG* regStruct  );
void SetDefaultVal_TOP_REG( TOP_REG* regStruct );
// ###################################
typedef struct DCUPSCALER_REG_TAG
{

    // 0 - Enable
    // 1 - bypass
    // format: 1u
    FIELD_UINT DCUS_bypass;

    // PDI read horizontal crop (before upscaling) - 
    // input start crop location, pointing to the first pixel to take, counting from zero.
    // Given in down-scaled resolution pixels.
    // Must be an even number due to 2x2 block resolution of the PDI format.
    // format: 4u
    FIELD_UINT DCUS_IN_CROP_X_START;

    // PDI read horizontal crop (before upscaling) - input end crop location, 
    // counting from zero before start cropping, pointing to the last pixel to take. 
    // Given in down-scaled resolution pixels
    // Must be an odd number due to 2x2 block resolution of the PDI format.
    // format: 8u
    FIELD_UINT DCUS_IN_CROP_X_END;

    // PDI read vertical size (before upscaling) - input end location (cropping cannot applied vertically) 
    // counting from zero, pointing to the last pixel to take. 
    // Given in down-scaled resolution pixels
    // Must be an odd number due to 2x2 block resolution of the PDI format.
    // format: 12u
    FIELD_UINT SIM_DCUS_IN_CROP_Y_END;

    // Horizontal crop after upscaling.
    // Start crop location in pixel resolution, pointing to the first pixel to take, counting from zero. 
    // Must be an even number due to 2x2 format constraint.
    // format: 4u
    FIELD_UINT DCUS_OUT_CROP_X_START;

    // Horizontal crop after upscaling.
    // End crop location in pixel resolution, pointing to the last pixel to take, counting from zero before start crop.
    // Must be an odd number due to 2x2 format constraint.
    // 
    // Number of right cropped out pixels cannot exceed 10 columns. Ie: 
    // (OUT_Crop_x_End+1) >= (In_Crop_x_end - In_crop_x_start +1) * 4 - 10
    // format: 10u
    FIELD_UINT DCUS_OUT_CROP_X_END;

    // Vertical crop after upscaling.
    // Start crop location in pixel resolution, pointing to the first pixel to take, counting from zero.
    // Must be an even number due to 2x2 format constraint
    // format: 4u
    FIELD_UINT DCUS_OUT_CROP_Y_START;

    // Vertical crop after upscaling.
    // End crop location in pixel resolution, pointing to the last pixel to take, 
    // counting from zero before start crop.
    // Must be an odd number due to 2x2 format constraint.
    // 
    // Number of bottom cropped out pixels cannot exceed 10 lines. Ie: 
    // (OUT_Crop_Y_End+1) >= (vertical input size) * 4 - 10
    // format: 14u
    FIELD_UINT DCUS_OUT_CROP_Y_END;

    // Dithering enable for luma image 
    // format: 1u
    FIELD_UINT DCUS_DITHERING_Y_En;

    // Dithering enable for chroma image 
    // format: 1u
    FIELD_UINT DCUS_DITHERING_C_En;

    // Random generator initial value, Must be odd number
    // format: 30u
    FIELD_UINT DCUS_DITHERING_seed;

} DCUPSCALER_REG;

// ############ Functions ############
int32_t Validate_DCUPSCALER_REG( DCUPSCALER_REG* regStruct  );
void SetDefaultVal_DCUPSCALER_REG( DCUPSCALER_REG* regStruct );
// ###################################
typedef struct DCBLEND1_REG_TAG
{

    ////////////////////////////
    // Bypass & mode register for the DCBlend1 block.
    // 0 - Operational
    // 1 - Bypass
    // 2 - Pass indications
    ////////////////////////////
    // (dcblend1_enable == 1 ) => (DCBlend1_Bypass = 0) otherwise depends on current pass (for dc64 =>1 else 2), will be set by llclib 
    // format: 2u
    FIELD_UINT DCBlend1_Bypass;

} DCBLEND1_REG;

// ############ Functions ############
int32_t Validate_DCBLEND1_REG( DCBLEND1_REG* regStruct  );
void SetDefaultVal_DCBLEND1_REG( DCBLEND1_REG* regStruct );
// ###################################
typedef struct CNR_REG_TAG
{

    // 0 - enable operation
    // 1 -  bypass unit
    // format: 1u
    FIELD_UINT CNR_Bypass;

    ////////////////////////////
    // CNR configuration register
    ////////////////////////////
    // 0 - select image from CYLPF
    // 1 - select DCNF image 
    // format: 1u
    FIELD_UINT CNR_Input_Select;

    // Defines the actual number of  treated colors
    // format: 3u
    FIELD_UINT CNR_Num_Colors;

    // 0 - disable external color regions treatment
    // 1 - enable color regions treatment to (U is less than or equal to 0) && (V is greater than or equal to 0)
    // format: 1u
    FIELD_UINT CNR_Ext_Enable;

    // 0 - don’t limit external color regions treatment
    // 1 - limit external color 
    // format: 1u
    FIELD_UINT CNR_Ext_Limit;

    ////////////////////////////
    // CNR debug mode
    ////////////////////////////
    // 0 - regular mode
    // 1 - CNR gets CYLPF input via DCNF
    // format: 1u
    FIELD_UINT CNR_Debug;

    ////////////////////////////
    // 0 - Bypass filter
    // 1 - 1x3 filter
    ////////////////////////////
    // format: 1u
    FIELD_UINT CNR_GainOffset_Filter_Mode_Y;

    // format: 1u
    FIELD_UINT CNR_GainOffset_Filter_Mode_C;

    // format: 1u
    FIELD_UINT CNR_BlendFactor_Filter_Mode_Y;

    // format: 1u
    FIELD_UINT CNR_BlendFactor_Filter_Mode_C;

    ////////////////////////////
    // Color0 adiitional parameters
    ////////////////////////////
    // 0 - Color #0 saturation is calculated by C/Y
    // 1 - Color #0 saturation is calculated by C
    // format: 2u
    FIELD_UINT CNR_Color0_Sat_Mode;

    // Color 0 Saturation calculation parameter, is calculated as:
    // (SlY_min - ShY_min) / (Ymax - Ymin)
    // format: (8+e)u
    FIELD_UINT CNR_Color0_CSMin_para;

    // Color 0 Saturation calculation parameter, is calculated as:
    // (SlY_max - ShY_max) / (Ymax - Ymin)
    // format: (8+e)u
    FIELD_UINT CNR_Color0_CSMax_para;

    // (1/slope) of smooth range transition function of external color regions
    // format: 8uQ8
    FIELD_UINT CNR_QExt;

    ////////////////////////////
    // 0 - C is calculated by abs(U) + abs(V)
    // 1 - C is calculated by Radius= 1.64676*(U^2+V^2)^0.5     
    ////////////////////////////
    // format: 1u
    FIELD_UINT CNR_C_Mode[5];

    ////////////////////////////
    // Angle(U,V) (hue) low limit
    ////////////////////////////
    // format: 11uQ2
    FIELD_UINT CNR_AMin[5];

    ////////////////////////////
    // Angle(U,V) (hue) high limit
    ////////////////////////////
    // format: 11uQ2
    FIELD_UINT CNR_AMax[5];

    ////////////////////////////
    // Angle internal/external  to range treatment (inside/outside the range)
    ////////////////////////////
    // format: 1u
    FIELD_UINT CNR_AInternal[5];

    ////////////////////////////
    // Color saturation low limit
    ////////////////////////////
    // If CNR_Color0_Sat_Mode = 0 or 2, CNR_CSMin[0] range is 0:2^(8+e)-1 
    // Else  CNR_CSMin[0] range is 0:2^(9+e)-1 
    // format: (9+e)u
    FIELD_UINT CNR_CSMin[5];

    ////////////////////////////
    // Color saturation high limit 
    ////////////////////////////
    // If CNR_Color0_Sat_Mode = 0 or 2, CNR_CSMax[0] range is 0:2^(8+e)-1 
    // Else  CNR_CSMax[0] range is 0:2^(9+e)-1 
    // format: (9+e)u
    FIELD_UINT CNR_CSMax[5];

    ////////////////////////////
    // Y component low limit
    ////////////////////////////
    // format: (8+e)u
    FIELD_UINT CNR_YMin[5];

    ////////////////////////////
    // Y component high limit
    ////////////////////////////
    // format: (8+e)u
    FIELD_UINT CNR_YMax[5];

    ////////////////////////////
    // Weight value at Color Range Borders
    ////////////////////////////
    // format: 4uQ4
    FIELD_UINT CNR_Boundary_Probability[5];

    ////////////////////////////
    // (1/slope) of smooth range transition function of each color region
    ////////////////////////////
    // format: 8uQ8
    FIELD_UINT CNR_Q[5];

    ////////////////////////////
    // Gain factor YLPF Y-Base Threshold
    ////////////////////////////
    // format: 11uQ7
    FIELD_UINT CNR_GainFactor_Y_Y[5];

    ////////////////////////////
    // Gain factor for YLPF U/V-Base Thresholds 
    ////////////////////////////
    // format: 11uQ7
    FIELD_UINT CNR_GainFactor_UV_Y[5];

    ////////////////////////////
    // Offset for YLPF Y-Base Threshold 
    ////////////////////////////
    // format: (9+e)s
    FIELD_INT CNR_Offset_Y_Y[5];

    ////////////////////////////
    // Offset for YLPF U-Base Threshold 
    ////////////////////////////
    // format: 9s
    FIELD_INT CNR_Offset_U_Y[5];

    ////////////////////////////
    // Offset for YLPF V-Base Threshold
    ////////////////////////////
    // format: 9s
    FIELD_INT CNR_Offset_V_Y[5];

    ////////////////////////////
    // Gain factor for CLPF Y-Base Threshold
    ////////////////////////////
    // format: 11uQ7
    FIELD_UINT CNR_GainFactor_Y_C[5];

    ////////////////////////////
    // Gain factor for CLPF U/V- Base Threshold 
    ////////////////////////////
    // format: 11uQ7
    FIELD_UINT CNR_GainFactor_UV_C[5];

    ////////////////////////////
    // Offset for CLPF Y-Base Threshold
    ////////////////////////////
    // format: (9+e)s
    FIELD_INT CNR_Offset_Y_C[5];

    ////////////////////////////
    // Offset for CLPF U-Base Threshold
    ////////////////////////////
    // format: 9s
    FIELD_INT CNR_Offset_U_C[5];

    ////////////////////////////
    // Offset for CLPF V-Base Threshold
    ////////////////////////////
    // format: 9s
    FIELD_INT CNR_Offset_V_C[5];

    ////////////////////////////
    // Factor for DCBlend2 Y_Dc blend factor modification
    ////////////////////////////
    // format: 8uQ7
    FIELD_UINT CNR_BlendFactor_Y_Dc[5];

    ////////////////////////////
    // Factor for DCBlend2 Y_Alt blend factor modification
    ////////////////////////////
    // format: 8uQ7
    FIELD_UINT CNR_BlendFactor_Y_Alt[5];

    ////////////////////////////
    // Factor for DCBlend2 Chroma blend factor modification
    ////////////////////////////
    // format: 8uQ7
    FIELD_UINT CNR_BlendFactor_C[5];

    ////////////////////////////
    // CNR FD configuration register
    ////////////////////////////
    // 0 - disable Face Detection treatment
    // 1 - enable Face Detection  treatment 
    // format: 1u
    FIELD_UINT CNR_FD_Enable;

    // Defines the actual number of  treated Face detection regions
    // format: 3u
    FIELD_UINT CNR_FD_Num_Regions;

    // X-Offset for absolute (x,y) coordinate calculation in chroma resolution
    // format: 16u
    FIELD_UINT CNR_FD_XOffset;

    // Y-Offset for absolute (x,y) coordinate calculation in chroma resolution
    // format: 16u
    FIELD_UINT CNR_FD_YOffset;

    // Horizontal center of face cyclic region
    // format: 16u
    FIELD_UINT CNR_FD_Regions_XCenter[5];

    // Vertical center of face cyclic region
    // format: 16u
    FIELD_UINT CNR_FD_Regions_YCenter[5];

    ////////////////////////////
    // CNR_FD_Regions_Rboundary << CNR_FD_Regions_Rboundary_shift 
    ////////////////////////////
    // represents Radius Boundary
    // format: 8u
    FIELD_UINT CNR_FD_Regions_RBoundary[5];

    // RBoubndary shift size
    // format: 4u
    FIELD_UINT CNR_FD_Regions_RBoundary_shift[5];

    ////////////////////////////
    // CNR_FD_Regions_Rslope/(2^CNR_FD_Regions_Rslope_shift) represents
    ////////////////////////////
    // the transition area slope
    // format: 8u
    FIELD_UINT CNR_FD_Regions_RSlope[5];

    // Slope shift size
    // format: 3u
    FIELD_UINT CNR_FD_Regions_RSlope_shift[5];

    // Defines for each color whether it's Color_W should be combined with FD_W
    // format: 1u
    FIELD_UINT CNR_FD_Combine[5];

} CNR_REG;

// ############ Functions ############
int32_t Validate_CNR_REG( CNR_REG* regStruct, uint32_t mE  );
void SetDefaultVal_CNR_REG( CNR_REG* regStruct, uint32_t mE );
// ###################################
typedef struct CYLPF_REG_TAG
{

    // Bypass register for the whole CYLPF block
    // format: 1u
    FIELD_UINT CYLPF_Bypass;

    ////////////////////////////
    // Bypass register for the CYLPF sub-blocks
    ////////////////////////////
    // Bypass register for YLPF sub-block
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__YLPF_FILTERING;

    // Bypass register for CLPF sub-block.
    // 0 - the edge sensitive CLPF logic is operational
    // 1 - the edge sensitive CLPF logic is bypassed
    // 2 - Low power isotropic mode 3x3
    // 3 - Low power isotropic mode 5x5
    // 4 - Low power isotropic mode 7x7
    // format: 3u
    FIELD_UINT CYLPF_Bypass_Sub__CLPF_FILTERING;

    // Bypass register for Pre-calc U, V threshold calculation logic.
    // When this sub register is set to '1',CLPF and YLPF logics are 
    // adjusted to work with constant U and V base threshold functions.
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__PreCalcThrUV;

    // Bypass register for Pre-calc Y derivatives calculation logic.
    // When this sub register is set to '1', all Y derivative values are set to zero.
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__PreCalcYder;

    // Bypass register for Pre-calc U,V derivatives calculation logic.
    // When this sub register is set to '1', all U,V derivative values are set to zero. 
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__PreCalcUVder;

    // Bypass register for the lens gain calculations
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__LensGain;

    // Bypass chromaticity calculations in the pre-calc logic
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__PreCalcChr;

    // Bypass flag for enabling/disabling the YLPF threshold comparisons
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__YLPF_DECISIONS;

    // Bypass flag for enabling/disabling the CLPF threshold comparisons
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__CLPF_DECISIONS;

    // Bypass flag for disabling the input indications line buffer
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__IND_BUFFER;

    // Make base threshold functions dependent exclusively on Y average.
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__PreCalcThrYDep;

    // Disable UV threshold comparison for YLPF
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__YLPF_UVcomp;

    // Disabled UV threshold comparison for CLPF
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__CLPF_UVcomp;

    // Disables YLPF DCRadius calculation and threshold modification
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__YLPF_DCIndThrMod;

    // Disables CLPF DCRadius calculation and threshold modification
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__CLPF_DCIndThrMod;

    // Disable Second order derivative threshold modification
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__YLPF_Der2DCIndThrMod;

    // Bypass register for Transition detection stage
    // format: 1u
    FIELD_UINT CYLPF_Bypass_Sub__TCF_TransDetection;

    // Debug register for CYLPF.
    // When enabled, replaces valid output indications with pixel values
    // format: 1u
    FIELD_UINT CYLPF_DEBUG__output;

    // Debug register for CYLPF.
    // When enabled, replaces indication input with pixel values
    // format: 1u
    FIELD_UINT CYLPF_DEBUG__input;

    ////////////////////////////
    // Configuration register for the entire CYLPF block
    ////////////////////////////
    // Controls the window size used to calculate the chromaticity average
    // 0-5x5
    // 1-3x3
    // format: 1u
    FIELD_UINT CYLPF_Cfg__ChromaAvgRadius;

    // Controls the window size used to calculate the derivative thresholds:
    // 0-luma3x3/chroma1x1;
    // 1-L5x5/C3x3;
    // 2-L9x9/C5x5 
    // format: 2u
    FIELD_UINT CYLPF_Cfg__ThrAvgSupport;

    // Controls which luma derivatives are used to calculate YMaxDer3
    // format: 1u
    FIELD_UINT CYLPF_Cfg__YMaxDerMode;

    // Toggles between slim and regular 3x3 derivatives of CLPF and YLPF.
    // format: 1u
    FIELD_UINT CYLPF_Cfg__Slim3x3Derivatives;

    ////////////////////////////
    // TCF control register
    ////////////////////////////
    // 0 - normal detection mode
    // 1- Ignore transition detection and  set all transition flags
    // format: 1u
    FIELD_UINT CYLPF_TCF_Cfg__SetAllTransitions;

    // Controls the window size used to calculate the derivative thresholds:
    // 0-luma3x3
    // 1-L5x5
    // 2-L9x9
    // format: 2u
    FIELD_UINT CYLPF_TCF_Cfg__ThrAvgSupport;

    // Controls sign check of 2 half trandsition radiuses
    // format: 1u
    FIELD_UINT CYLPF_TCF_Cfg__CheckSignMode;

    ////////////////////////////
    // TCF Radius related parameters register 
    ////////////////////////////
    // Defines the Hor/Ver Transition width for detection stage
    // format: 4u
    FIELD_UINT CYLPF_TCF_Radius_Thr__SumHorVer;

    // Defines the Dg1/Dg2 Transition width for detection stage
    // format: 3u
    FIELD_UINT CYLPF_TCF_Radius_Thr__SumDg1Dg2;

    // Defines the Hor/Ver minimum transition width in each half-direction 
    // format: 3u
    FIELD_UINT CYLPF_TCF_Radius_Thr__MinHorVer;

    // Defines the Dg1/Dg2 minimum transition width in each half-direction
    // format: 2u
    FIELD_UINT CYLPF_TCF_Radius_Thr__MinDg1Dg2;

    ////////////////////////////
    // Write register for loading the TCF transition low threshold function LUT (9 entries x (8+e) bits)
    ////////////////////////////
    // format: (8+e)u
    FIELD_UINT CYLPF_TCF_Transition_Thr_Low_LUT[9];

    ////////////////////////////
    // Write register for loading the TCF transition low threshold function LUT (9 entries x (8+e) bits)
    ////////////////////////////
    // format: (8+e)u
    FIELD_UINT CYLPF_TCF_Transition_Thr_High_LUT[9];

    ////////////////////////////
    // TCF transition threshold low function limits  
    ////////////////////////////
    // format: (8+e)u
    FIELD_UINT CYLPF_TCF_Transition_Thr_Low_Limits__TopLim;

    // format: (8+e)u
    FIELD_UINT CYLPF_TCF_Transition_Thr_Low_Limits__BotLim;

    ////////////////////////////
    // TCF transition threshold high function limits  
    ////////////////////////////
    // format: (8+e)u
    FIELD_UINT CYLPF_TCF_Transition_Thr_High_Limits__TopLim;

    // format: (8+e)u
    FIELD_UINT CYLPF_TCF_Transition_Thr_High_Limits__BotLim;

    // Normalized Y coordinate of the first pixel relative to the optical center. (1,8,15)
    // format: 24s
    FIELD_INT CYLPF_LG_Ystart;

    // Normalized cross element value of the ellipse in the first pixel relative to the optical center (1,8,15)
    // format: 24s
    FIELD_INT CYLPF_LG_YExtStart;

    // X coordinate of the first pixel in the strip relative to the optical center. (1,8,15)
    // format: 24s
    FIELD_INT CYLPF_LG_Xstart;

    // The normalized relative increment in the X axis when moving two pixel to the left.
    // Can be negative when using straight lines. (1,1,15)
    // format: 17sQ15
    FIELD_INT CYLPF_LG_XRelInc;

    // The normalized relative increment in the Y axis when moving one pixel downwards.
    // Can be negative when using straight lines. (1,1,15)
    // format: 17sQ15
    FIELD_INT CYLPF_LG_YRelInc;

    // The normalized relative increment in the ellipse cross element when moving one pixel downwards (1,1,15)
    // format: 17sQ15
    FIELD_INT CYLPF_LG_YExtRelInc;

    // This register describes the closest power of 2, which result in XrelInc or YrelInc that are smaller than 1 (in absolute value).
    // format: 3u
    FIELD_UINT CYLPF_LG_Radius_Rng;

    // Determines the distance function for the radius dependent blend factor calculation.  
    // format: 1u
    FIELD_UINT CYLPF_LG_Radial_Shape;

    // Write register for loading the lens gain threshold LUT (17 entries x 11bits). 
    // format: 11u
    FIELD_UINT CYLPF_LG_YFilter_LUT_Thr_Y[17];

    // Write register for loading the lens gain threshold LUT (17 entries x 11bits). 
    // format: 11u
    FIELD_UINT CYLPF_LG_YFilter_LUT_Thr_C[17];

    // Write register for loading the lens gain threshold LUT (17 entries x 11bits). 
    // format: 11u
    FIELD_UINT CYLPF_LG_CFilter_LUT_Thr_Y[17];

    // Write register for loading the lens gain threshold LUT (17 entries x 11bits). 
    // format: 11u
    FIELD_UINT CYLPF_LG_CFilter_LUT_Thr_C[17];

    // Write register for loading the lens gain blend factor LUT (33 entries x 8bits). 
    // format: 8u
    FIELD_UINT CYLPF_LG_LUT_Blend[33];

    // limit value for UV-Average input for Luma base threshold caluclation
    // format: 7u
    FIELD_UINT CYLPF_YFilter_UVlimit;

    // Blend factor for CNR gain level for the Far base Luma thresholds 
    // 0-no CNR
    // 4-full CNR operation
    // format: 3u
    FIELD_UINT CYLPF_YFilter_CnrBaseBlendFar_Y;

    // Blend factor for CNR gain level for the Far base Chroma thresholds
    // 0-no CNR
    // 4-full CNR operation
    // format: 3u
    FIELD_UINT CYLPF_YFilter_CnrBaseBlendFar_C;

    // format: (8+e)u
    FIELD_UINT CYLPF_YFilter_YYTb[17];

    // format: (9+e)s
    FIELD_INT CYLPF_YFilter_YCTb[8];

    ////////////////////////////
    // Yfilter: Top limit and bottom limit of Y threshold calculation result
    ////////////////////////////
    // format: (8+e)u
    FIELD_UINT CYLPF_YFilter_YTb_Limits__TopLim;

    // format: (8+e)u
    FIELD_UINT CYLPF_YFilter_YTb_Limits__BotLim;

    // format: 8u
    FIELD_UINT CYLPF_YFilter_UYTb[17];

    // format: 9s
    FIELD_INT CYLPF_YFilter_UCTb[8];

    ////////////////////////////
    // Yfilter: Top limit and bottom limit of U threshold calculation result
    ////////////////////////////
    // format: 8u
    FIELD_UINT CYLPF_YFilter_UTb_Limits__TopLim;

    // format: 8u
    FIELD_UINT CYLPF_YFilter_UTb_Limits__BotLim;

    // format: 8u
    FIELD_UINT CYLPF_YFilter_VYTb[17];

    // format: 9s
    FIELD_INT CYLPF_YFilter_VCTb[8];

    ////////////////////////////
    // Yfilter: Top limit and bottom limit of V threshold calculation result
    ////////////////////////////
    // format: 8u
    FIELD_UINT CYLPF_YFilter_VTb_Limits__TopLim;

    // format: 8u
    FIELD_UINT CYLPF_YFilter_VTb_Limits__BotLim;

    // limit value for UV-Average input for Chroma base threshold caluclation
    // format: 7u
    FIELD_UINT CYLPF_CFilter_UVlimit;

    // Blend factor for CNR gain level for the Far base Luma thresholds
    // 0-no CNR
    // 4-full CNR operation
    // format: 3u
    FIELD_UINT CYLPF_CFilter_CnrBaseBlendFar_Y;

    // Blend factor for CNR gain level for the Far base Chroma thresholds
    // 0-no CNR
    // 4-full CNR operation
    // format: 3u
    FIELD_UINT CYLPF_CFilter_CnrBaseBlendFar_C;

    // format: (8+e)u
    FIELD_UINT CYLPF_CFilter_YYTb[17];

    // format: (9+e)s
    FIELD_INT CYLPF_CFilter_YCTb[8];

    ////////////////////////////
    // Cfilter: Top limit and bottom limit of Y primary threshold calculation result
    ////////////////////////////
    // format: (8+e)u
    FIELD_UINT CYLPF_CFilter_YTb_Limits__TopLim;

    // format: (8+e)u
    FIELD_UINT CYLPF_CFilter_YTb_Limits__BotLim;

    // format: 8u
    FIELD_UINT CYLPF_CFilter_UYTb[17];

    // format: 9s
    FIELD_INT CYLPF_CFilter_UCTb[8];

    ////////////////////////////
    // Cfilter: Top limit and bottom limit of Y primary threshold calculation result
    ////////////////////////////
    // format: 8u
    FIELD_UINT CYLPF_CFilter_UTb_Limits__TopLim;

    // format: 8u
    FIELD_UINT CYLPF_CFilter_UTb_Limits__BotLim;

    // format: 8u
    FIELD_UINT CYLPF_CFilter_VYTb[17];

    // format: 9s
    FIELD_INT CYLPF_CFilter_VCTb[8];

    ////////////////////////////
    // Cfilter: Top limit and bottom limit of V primary threshold calculation result
    ////////////////////////////
    // format: 8u
    FIELD_UINT CYLPF_CFilter_VTb_Limits__TopLim;

    // format: 8u
    FIELD_UINT CYLPF_CFilter_VTb_Limits__BotLim;

    // Controls the radius of the luma filter applied when no edge is detected 
    // in the current pixel
    // format: 3u
    FIELD_UINT CYLPF_YFilter_Cfg__FilterSize;

    // Controls how pixels outside the four main axis of the filtering window can 
    // join the weighted sum, when no edge is detected
    // format: 2u
    FIELD_UINT CYLPF_YFilter_Cfg__EnableComp;

    // Determines the reduction of YDCInd radii factors
    // format: 1u
    FIELD_UINT CYLPF_YFilter_Cfg__DCIndThr_Shift;

    // Determines the offset of YDCInd radii factors
    // format: 2u
    FIELD_UINT CYLPF_YFilter_Cfg__DCIndThr_Offset;

    // Controls the radius of the luma filter applied when edges are detected in the current pixel
    // format: 3u
    FIELD_UINT CYLPF_YFilter_Cfg__AltFilterSize;

    // Controls how pixels outside the four main axis of the filtering window can join
    // the weighted sum, when edges are detected
    // format: 2u
    FIELD_UINT CYLPF_YFilter_Cfg__EnableAltComp;

    ////////////////////////////
    // Filtering kernel of non-edge luma pixels. 
    // This register describes the FIR filter shape used by the YLPF.
    // The total weighted sum of the weights should always be limited to 256
    ////////////////////////////
    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W1x1_Center;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W3x3_HV;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W3x3_Dg;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W5x5_HV;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W5x5_Dg;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W5x5_Comp;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W7x7_HV;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W7x7_Dg;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W7x7_Comp;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W9x9_HV;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W9x9_Dg;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_Kernel__W9x9_Comp;

    // Defines an addition to the weight of the center pixel
    // format: 4u
    FIELD_UINT CYLPF_YFilter_Kernel__W1x1Add;

    ////////////////////////////
    // Filtering kernel of edge luma pixels.
    // This register describes the FIR filter shape used by the YLPF.
    // The total weighted sum of the weights should always be limited to 256
    ////////////////////////////
    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W1x1_Center;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W3x3_HV;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W3x3_Dg;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W5x5_HV;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W5x5_Dg;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W5x5_Comp;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W7x7_HV;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W7x7_Dg;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W7x7_Comp;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W9x9_HV;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W9x9_Dg;

    // format: 2u
    FIELD_UINT CYLPF_YFilter_AltKernel__W9x9_Comp;

    // Defines an addition to the weight of the center pixel
    // format: 4u
    FIELD_UINT CYLPF_YFilter_AltKernel__W1x1Add;

    ////////////////////////////
    // Condition flags for calculating the luma dc indication flags
    // 0-consider
    // 1-ignore
    ////////////////////////////
    // D3_3      StartBit:0   Default:0
    // D3_5      StartBit:1   Default:1
    // D3_5X     StartBit:2   Default:1
    // D3_7      StartBit:3   Default:1
    // D3_9      StartBit:4   Default:1
    // D3_9X     StartBit:5   Default:1
    // D5_3      StartBit:6   Default:0
    // D5_5      StartBit:7   Default:0
    // D5_5X     StartBit:8   Default:0
    // D5_7      StartBit:9   Default:1
    // D5_9      StartBit:10   Default:1
    // D5_9X     StartBit:11   Default:1
    // D7_3      StartBit:12   Default:0
    // D7_5      StartBit:13   Default:0
    // D7_5X     StartBit:14   Default:0
    // D7_7      StartBit:15   Default:0
    // D7_9      StartBit:16   Default:1
    // D7_9X     StartBit:17   Default:1
    // D9_3      StartBit:18   Default:0
    // D9_5      StartBit:19   Default:0
    // D9_5X     StartBit:20   Default:0
    // D9_7      StartBit:21   Default:0
    // D9_9      StartBit:22   Default:0
    // D9_9X     StartBit:23   Default:0
    // format: 24u
    FIELD_UINT CYLPF_YFilter_DCIndDerFlags;

    ////////////////////////////
    // Condition flags for calculating the luma filtering flags
    ////////////////////////////
    // D3_3      StartBit:0   Default:0
    // D3_5      StartBit:1   Default:1
    // D3_5X     StartBit:2   Default:1
    // D3_7      StartBit:3   Default:1
    // D3_9      StartBit:4   Default:1
    // D3_9X     StartBit:5   Default:1
    // D5_3      StartBit:6   Default:0
    // D5_5      StartBit:7   Default:0
    // D5_5X     StartBit:8   Default:0
    // D5_7      StartBit:9   Default:1
    // D5_9      StartBit:10   Default:1
    // D5_9X     StartBit:11   Default:1
    // D7_3      StartBit:12   Default:0
    // D7_5      StartBit:13   Default:0
    // D7_5X     StartBit:14   Default:0
    // D7_7      StartBit:15   Default:0
    // D7_9      StartBit:16   Default:1
    // D7_9X     StartBit:17   Default:1
    // D9_3      StartBit:18   Default:0
    // D9_5      StartBit:19   Default:0
    // D9_5X     StartBit:20   Default:0
    // D9_7      StartBit:21   Default:0
    // D9_9      StartBit:22   Default:0
    // D9_9X     StartBit:23   Default:0
    // format: 24u
    FIELD_UINT CYLPF_YFilter_FiltDerFlags;

    ////////////////////////////
    // Der2 condition flags for calculating the luma filtering flags required for TCF
    // 
    // 0-ignore, 1-consider
    ////////////////////////////
    // DCInd_D3_3      StartBit:0     Default:0
    // DCInd_D3_5      StartBit:1     Default:0
    // DCInd_D3_7      StartBit:2     Default:0
    // DCInd_D3_9      StartBit:3     Default:0
    // Filt_D3_3       StartBit:4     Default:0
    // Filt_D3_5       StartBit:5     Default:0
    // Filt_D3_7       StartBit:6     Default:0
    // Filt_D3_9       StartBit:7     Default:0
    // format: 8u
    FIELD_UINT CYLPF_YFilter_Der2Flags;

    ////////////////////////////
    // Specifies YLPF masks for directional filtering.
    // Bits 15..0 allow forcing an 'edge' label to specific directions of derivativeóthreshold comparisons.
    // Bits 31..16 allow forcing a 'flat' label to the same directions. 
    // Note: Forcing a 'flat' label overrides forcing an 'edge' label
    // 0- don’t force
    // 1-force 'flat'
    ////////////////////////////
    // Hor3Flat    Startbit:0     Default:0
    // Hor5Flat    Startbit:1     Default:0
    // Hor7Flat    Startbit:2     Default:0
    // Hor9Flat    Startbit:3     Default:0
    // Ver3Flat    Startbit:4     Default:0
    // Ver5Flat    Startbit:5     Default:0
    // Ver7Flat    Startbit:6     Default:0
    // Ver9Flat    Startbit:7     Default:0
    // Dg13Flat    Startbit:8     Default:0
    // Dg15Flat    Startbit:9     Default:0
    // Dg17Flat    Startbit:10     Default:0
    // Dg19Flat    Startbit:11     Default:0
    // Dg23Flat    Startbit:12     Default:0
    // Dg25Flat    Startbit:13     Default:0
    // Dg27Flat    Startbit:14     Default:0
    // Dg29Flat    Startbit:15     Default:0
    // Hor3Edge    Startbit:16     Default:1
    // Hor5Edge    Startbit:17     Default:1
    // Hor7Edge    Startbit:18     Default:1
    // Hor9Edge    Startbit:19     Default:1
    // Ver3Edge    Startbit:20     Default:1
    // Ver5Edge    Startbit:21     Default:1
    // Ver7Edge    Startbit:22     Default:1
    // Ver9Edge    Startbit:23     Default:1
    // Dg13Edge    Startbit:24     Default:1
    // Dg15Edge    Startbit:25     Default:1
    // Dg17Edge    Startbit:26     Default:1
    // Dg19Edge    Startbit:27     Default:1
    // Dg23Edge    Startbit:28     Default:1
    // Dg25Edge    Startbit:29     Default:1
    // Dg27Edge    Startbit:30     Default:1
    // Dg29Edge    Startbit:31     Default:1
    // format: 32u
    FIELD_UINT CYLPF_YFilter_DirectionalCfg;

    ////////////////////////////
    // Specifies YLPF masks for external directional filtering.
    // Bits 7..0 allow forcing an 'edge' label to specific directions of derivativeóthreshold comparisons.
    // Bits 15:8 allow forcing a 'flat' label to the same directions. 
    // Note: Forcing a 'flat' label overrides forcing an 'edge' label
    // 
    // 0- don’t force / 1-force 'flat'
    ////////////////////////////
    // Hor5XFlat        StartBit:0        Default:0
    // Hor9XFlat        StartBit:1        Default:0
    // Ver5XFlat        StartBit:2        Default:0
    // Ver9XFlat        StartBit:3        Default:0
    // Dg15XFlat        StartBit:4        Default:0
    // Dg19XFlat        StartBit:5        Default:0
    // Dg25XFlat        StartBit:6        Default:0
    // Dg29XFlat        StartBit:7        Default:0
    // Hor5XEdge        StartBit:8        Default:1
    // Hor9XEdge        StartBit:9        Default:1
    // Ver5XEdge        StartBit:10       Default:1
    // Ver9XEdge        StartBit:11       Default:1
    // Dg15XEdge        StartBit:12       Default:1
    // Dg19XEdge        StartBit:13       Default:1
    // Dg25XEdge        StartBit:14       Default:1
    // Dg29XEdge        StartBit:15       Default:1
    // format: 16u
    FIELD_UINT CYLPF_YFilter_DirectionalCfgX;

    ////////////////////////////
    // Luma filter, 3x3 luma derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_YFilter_YThrClose3Mod__Scale;

    // 11 bits signed (1,10,0).
    // format: (9+e)s
    FIELD_INT CYLPF_YFilter_YThrClose3Mod__Offset;

    ////////////////////////////
    // Luma filter, 3x3 luma second order derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_YFilter_YThrDer2Close3Mod__Scale;

    // 11 bits signed (1,10,0).
    // format: (9+e)s
    FIELD_INT CYLPF_YFilter_YThrDer2Close3Mod__Offset;

    ////////////////////////////
    // Luma filter, 7x7/9x9 luma derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_YFilter_YThrFarMod__Scale;

    // 11 bits signed (1,10,0).
    // format: (9+e)s
    FIELD_INT CYLPF_YFilter_YThrFarMod__Offset;

    ////////////////////////////
    // Luma filter, 5x5 U derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_YFilter_UThrFarMod__Scale;

    // 9 bits signed (1,8,0).
    // format: 9s
    FIELD_INT CYLPF_YFilter_UThrFarMod__Offset;

    ////////////////////////////
    // Luma filter, 5x5 V derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_YFilter_VThrFarMod__Scale;

    // 9 bits signed (1,8,0).
    // format: 9s
    FIELD_INT CYLPF_YFilter_VThrFarMod__Offset;

    ////////////////////////////
    // Luma filter, 5x5 external luma derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_YFilter_YThrCloseXMod__Scale;

    // 13 bits signed (1,12,0).
    // format: (9+e)s
    FIELD_INT CYLPF_YFilter_YThrCloseXMod__Offset;

    ////////////////////////////
    // Luma filter, 9x9 external luma derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_YFilter_YThrFarXMod__Scale;

    // 13 bits signed (1,12,0).
    // format: (9+e)s
    FIELD_INT CYLPF_YFilter_YThrFarXMod__Offset;

    ////////////////////////////
    // Luma filter, 5x5 external U derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_YFilter_UThrFarXMod__Scale;

    // 9 bits signed (1,8,0).
    // format: 9s
    FIELD_INT CYLPF_YFilter_UThrFarXMod__Offset;

    ////////////////////////////
    // Luma filter, 5x5 external V derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_YFilter_VThrFarXMod__Scale;

    // 9 bits signed (1,8,0).
    // format: 9s
    FIELD_INT CYLPF_YFilter_VThrFarXMod__Offset;

    ////////////////////////////
    // Threshold modifiers depending on DC indications.
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // DC4       StartBit:24     Default:42
    // format: 6u
    FIELD_UINT CYLPF_YDCIndY_Gain_Der_3[5];

    ////////////////////////////
    // Threshold modifiers depending on DC indications.
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // DC4       StartBit:24     Default:42
    // format: 6u
    FIELD_UINT CYLPF_YDCIndY_Gain_Der_5[5];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // DC4       StartBit:24     Default:42
    // format: 6u
    FIELD_UINT CYLPF_YDCIndY_Gain_Der_7[5];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // DC4       StartBit:24     Default:42
    // format: 6u
    FIELD_UINT CYLPF_YDCIndY_Gain_Der_9[5];

    ////////////////////////////
    // Threshold modifiers depending on DC indications.
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // DC4       StartBit:24     Default:42
    // format: 6u
    FIELD_UINT CYLPF_YDCIndY_Gain_Der_5X[5];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // DC4       StartBit:24     Default:42
    // format: 6u
    FIELD_UINT CYLPF_YDCIndY_Gain_Der_9X[5];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // DC4       StartBit:24     Default:42
    // format: 6u
    FIELD_UINT CYLPF_YDCIndUV_Gain_Der_3[5];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // DC4       StartBit:24     Default:42
    // format: 6u
    FIELD_UINT CYLPF_YDCIndUV_Gain_Der_5[5];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // DC4       StartBit:24     Default:42
    // format: 6u
    FIELD_UINT CYLPF_YDCIndUV_Gain_Der_5X[5];

    ////////////////////////////
    // 0
    ////////////////////////////
    // Controls the radius of the chroma filter applied when no edge is detected in the current pixel
    // 0 - Cfilter is bypassed
    // 1 - 3x3 kernel included
    // 2 - 5x5 kernel included 
    // 3 - 7x7 kernel included
    // format: 2u
    FIELD_UINT CYLPF_CFilter_Cfg__FilterSize;

    // Controls whether the chroma filters operates in symmetric or a-symmetric mode
    // format: 1u
    FIELD_UINT CYLPF_CFilter_Cfg__FilteringMode;

    // Determines whether the external chroma filter thresholds undergo grey modification or not
    // format: 1u
    FIELD_UINT CYLPF_CFilter_Cfg__ExternalDerivGreyMode;

    // Controls the support size of the grey filter result: 
    // 0-3x3;
    // 1-5x5
    // 2-dynamic
    // 3-7x7
    // format: 2u
    FIELD_UINT CYLPF_CFilter_Cfg__GreyBlendAvgSize;

    // Determines whether lower chromaticity is a condition for performing grey blending
    // format: 1u
    FIELD_UINT CYLPF_CFilter_Cfg__GreyBlendChromaCondition;

    // Determines whether larger support size is a condition for performing grey blending
    // format: 1u
    FIELD_UINT CYLPF_CFilter_Cfg__GreyBlendGreySizeCondition;

    // Determines the reduction of CDCInd radii factors
    // format: 1u
    FIELD_UINT CYLPF_CFilter_Cfg__DCIndThr_Shift;

    // Controls internal limiting of grey ch and grey yder functions
    // format: 1u
    FIELD_UINT CYLPF_CFilter_Cfg__GreyFactorFncLimitsMode;

    // Controls the logical symmetry calculation of detail identification
    // format: 1u
    FIELD_UINT CYLPF_CFilter_Cfg__DetailSymmetryMode;

    // enable grey threshold treatment 
    // format: 1u
    FIELD_UINT CYLPF_CFilter_Cfg__GreyFactorThrEn ;

    // enable grey blend treatment
    // format: 1u
    FIELD_UINT CYLPF_CFilter_Cfg__GreyFactorBlendEn;

    // Determines the offset of CDCInd radii factors
    // format: 2u
    FIELD_UINT CYLPF_CFilter_Cfg__DCIndThr_Offset;

    ////////////////////////////
    // Contains chromaticity threshold and y derivative threshold used to identify details
    ////////////////////////////
    // format: 8u
    FIELD_UINT CYLPF_CFilter_Detail_Cond__Chromaticity;

    // format: 8u
    FIELD_UINT CYLPF_CFilter_Detail_Cond__YDerMax;

    ////////////////////////////
    // Threshold for chroma detail detection
    ////////////////////////////
    // format: 12u
    FIELD_UINT CYLPF_CThresholds__UD;

    // format: 12u
    FIELD_UINT CYLPF_CThresholds__X;

    ////////////////////////////
    // Describes the slope and offset of the grey factor luma derivative function
    ////////////////////////////
    // (0,6,6)
    // format: 12uQ6
    FIELD_UINT CYLPF_CFilter_Grey_YDerFnc__Slope;

    // (1,11,0)
    // format: 12s
    FIELD_INT CYLPF_CFilter_Grey_YDerFnc__Offset;

    ////////////////////////////
    // Describes the slope and offset of the grey factor chromaticity function
    ////////////////////////////
    // (1,6,6)
    // format: 13sQ6
    FIELD_INT CYLPF_CFilter_Grey_ChFnc__Slope;

    // (0,11,0)
    // format: 11u
    FIELD_UINT CYLPF_CFilter_Grey_ChFnc__Offset;

    ////////////////////////////
    // Describes the top and bottom limits of the grey factor
    ////////////////////////////
    // format: 7u
    FIELD_UINT CYLPF_CFilter_Grey_Limits__TopLim;

    // format: 7u
    FIELD_UINT CYLPF_CFilter_Grey_Limits__BotLim;

    // Scale factor for grey blending thredshold
    // format: 6u
    FIELD_UINT CYLPF_CFilter_Grey_BlendScale;

    // Offset value for grey blending thredshold
    // format: 7s
    FIELD_INT CYLPF_CFilter_Grey_BlendOffset;

    ////////////////////////////
    // Thresholds used to calculate the grey dc size output indication
    ////////////////////////////
    // format: 8u
    FIELD_UINT CYLPF_CFilter_GreySizeThr_3_5__GreyDC3x3AvgThr;

    // format: 8u
    FIELD_UINT CYLPF_CFilter_GreySizeThr_3_5__GreyDC3x3RingThr;

    // format: 8u
    FIELD_UINT CYLPF_CFilter_GreySizeThr_3_5__GreyDC5x5RingThr;

    ////////////////////////////
    // Thresholds used to calculate the grey dc size output indication
    ////////////////////////////
    // format: 10u
    FIELD_UINT CYLPF_CFilter_GreySizeThr_7_9__GreyDC7x7RingThr;

    ////////////////////////////
    // Target thresholds for the 3x3 and 5x5 thresholds, used when 
    // opening noise thresholds due to pixel 'greyness'
    ////////////////////////////
    // format: (8+e)u
    FIELD_UINT CYLPF_CFilter_Grey_ThrModVal_Close__YT;

    // format: 8u
    FIELD_UINT CYLPF_CFilter_Grey_ThrModVal_Close__UT;

    // format: 8u
    FIELD_UINT CYLPF_CFilter_Grey_ThrModVal_Close__VT;

    ////////////////////////////
    // Target thresholds for the 7x7 and 9x9 thresholds, used when 
    // opening noise thresholds due to pixel 'greyness'
    ////////////////////////////
    // format: (8+e)u
    FIELD_UINT CYLPF_CFilter_Grey_ThrModVal_Far__YT;

    // format: 8u
    FIELD_UINT CYLPF_CFilter_Grey_ThrModVal_Far__UT;

    // format: 8u
    FIELD_UINT CYLPF_CFilter_Grey_ThrModVal_Far__VT;

    ////////////////////////////
    // Condition flags for calculating the chroma dc indication flags
    // 0-consider
    // 1-ignore
    ////////////////////////////
    // D3_3        StartBit:0     Default:0
    // D3_5        StartBit:1     Default:1
    // D3_5X       StartBit:2     Default:1
    // D3_7        StartBit:3     Default:1
    // D5_3        StartBit:6     Default:0
    // D5_5        StartBit:7     Default:0
    // D5_5X       StartBit:8     Default:1
    // D5_7        StartBit:9     Default:1
    // D7_3        StartBit:12    Default:0
    // D7_5        StartBit:13    Default:0
    // D7_5X       StartBit:14    Default:1
    // D7_7        StartBit:15    Default:0
    // format: 16u
    FIELD_UINT CYLPF_CFilter_DCIndDerFlags;

    ////////////////////////////
    // Condition flags for calculating the chroma filtering flags
    // 0-consider 
    // 1-ignore
    ////////////////////////////
    // D3_3        StartBit:0     Default:0
    // D3_5        StartBit:1     Default:1
    // D3_5X       StartBit:2     Default:1
    // D3_7        StartBit:3     Default:1
    // D5_3        StartBit:6     Default:0
    // D5_5        StartBit:7     Default:0
    // D5_5X       StartBit:8     Default:1
    // D5_7        StartBit:9     Default:1
    // D7_3        StartBit:12    Default:0
    // D7_5        StartBit:13    Default:0
    // D7_5X       StartBit:14    Default:1
    // D7_7        StartBit:15    Default:0
    // format: 16u
    FIELD_UINT CYLPF_CFilter_FiltDerFlags;

    ////////////////////////////
    // Specifies CLPF masks for directional filtering.
    // Bits 15..0 allow forcing an 'edge' label to specific directions of derivative threshold comparisons.
    // Bits 31..16 allow forcing a 'flat' label to the same directions. 
    // Note: Forcing a 'flat' label overrides forcing an 'edge' label
    // 0- don’t force / 1-force 'flat'
    ////////////////////////////
    // Hor3Flat    Startbit:0     Default:0
    // Hor5Flat    Startbit:1     Default:0
    // Hor7Flat    Startbit:2     Default:0
    // Hor9Flat    Startbit:3     Default:0
    // Ver3Flat    Startbit:4     Default:0
    // Ver5Flat    Startbit:5     Default:0
    // Ver7Flat    Startbit:6     Default:0
    // Ver9Flat    Startbit:7     Default:0
    // Dg13Flat    Startbit:8     Default:0
    // Dg15Flat    Startbit:9     Default:0
    // Dg17Flat    Startbit:10     Default:0
    // Dg19Flat    Startbit:11     Default:0
    // Dg23Flat    Startbit:12     Default:0
    // Dg25Flat    Startbit:13     Default:0
    // Dg27Flat    Startbit:14     Default:0
    // Dg29Flat    Startbit:15     Default:0
    // Hor3Edge    Startbit:16     Default:1
    // Hor5Edge    Startbit:17     Default:1
    // Hor7Edge    Startbit:18     Default:1
    // Hor9Edge    Startbit:19     Default:1
    // Ver3Edge    Startbit:20     Default:1
    // Ver5Edge    Startbit:21     Default:1
    // Ver7Edge    Startbit:22     Default:1
    // Ver9Edge    Startbit:23     Default:1
    // Dg13Edge    Startbit:24     Default:1
    // Dg15Edge    Startbit:25     Default:1
    // Dg17Edge    Startbit:26     Default:1
    // Dg19Edge    Startbit:27     Default:1
    // Dg23Edge    Startbit:28     Default:1
    // Dg25Edge    Startbit:29     Default:1
    // Dg27Edge    Startbit:30     Default:1
    // Dg29Edge    Startbit:31     Default:1
    // format: 32u
    FIELD_UINT CYLPF_CFilter_DirectionalCfg;

    ////////////////////////////
    // Specifies CLPF masks for external directional filtering.
    // Bits 7..0 allow forcing an 'edge' label to specific directions of derivativeóthreshold comparisons.
    // Bits 15:8 allow forcing a 'flat' label to the same directions.
    // Note: Forcing a 'flat' label overrides forcing an 'edge' label
    ////////////////////////////
    // Hor5XFlat        StartBit:0        Default:0
    // Hor9XFlat        StartBit:1        Default:0
    // Ver5XFlat        StartBit:2        Default:0
    // Ver9XFlat        StartBit:3        Default:0
    // Dg15XFlat        StartBit:4        Default:0
    // Dg19XFlat        StartBit:5        Default:0
    // Dg25XFlat        StartBit:6        Default:0
    // Dg29XFlat        StartBit:7        Default:0
    // Hor5XEdge        StartBit:8        Default:1
    // Hor9XEdge        StartBit:9        Default:1
    // Ver5XEdge        StartBit:10       Default:1
    // Ver9XEdge        StartBit:11       Default:1
    // Dg15XEdge        StartBit:12       Default:1
    // Dg19XEdge        StartBit:13       Default:1
    // Dg25XEdge        StartBit:14       Default:1
    // Dg29XEdge        StartBit:15       Default:1
    // format: 16u
    FIELD_UINT CYLPF_CFilter_DirectionalCfgX;

    ////////////////////////////
    // Chroma filter, 7x7/9x9 luma derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_CFilter_YThrFarMod__Scale;

    // 13 bits signed (1,12,0).
    // format: (9+e)s
    FIELD_INT CYLPF_CFilter_YThrFarMod__Offset;

    ////////////////////////////
    // Chroma filter, 5x5 U derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_CFilter_UThrFarMod__Scale;

    // 9 bits signed (1,8,0).
    // format: 9s
    FIELD_INT CYLPF_CFilter_UThrFarMod__Offset;

    ////////////////////////////
    // Chroma filter, 5x5 V derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_CFilter_VThrFarMod__Scale;

    // 9 bits signed (1,8,0).
    // format: 9s
    FIELD_INT CYLPF_CFilter_VThrFarMod__Offset;

    ////////////////////////////
    // Chroma filter, 7x7 U derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_CFilter_UThrDistMod__Scale;

    // 9 bits signed (1,8,0).
    // format: 9s
    FIELD_INT CYLPF_CFilter_UThrDistMod__Offset;

    ////////////////////////////
    // Chroma filter, 7x7 V derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_CFilter_VThrDistMod__Scale;

    // 9 bits signed (1,8,0).
    // format: 9s
    FIELD_INT CYLPF_CFilter_VThrDistMod__Offset;

    ////////////////////////////
    // Chroma filter, 5x5 external luma derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_CFilter_YThrCloseXMod__Scale;

    // 13 bits signed (1,12,0).
    // format: (9+e)s
    FIELD_INT CYLPF_CFilter_YThrCloseXMod__Offset;

    ////////////////////////////
    // Chroma filter, 9x9 external luma derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_CFilter_YThrFarXMod__Scale;

    // 13 bits signed (1,12,0).
    // format: (9+e)s
    FIELD_INT CYLPF_CFilter_YThrFarXMod__Offset;

    ////////////////////////////
    // Chroma filter, 5x5 external U derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_CFilter_UThrFarXMod__Scale;

    // 9 bits signed (1,8,0).
    // format: 9s
    FIELD_INT CYLPF_CFilter_UThrFarXMod__Offset;

    ////////////////////////////
    // Chroma filter, 5x5 external V derivative scale/offset factors
    ////////////////////////////
    // 6 bits unsigned (0,2,4)
    // format: 6uQ4
    FIELD_UINT CYLPF_CFilter_VThrFarXMod__Scale;

    // 9 bits signed (1,8,0).
    // format: 9s
    FIELD_INT CYLPF_CFilter_VThrFarXMod__Offset;

    ////////////////////////////
    // Threshold modifiers depending on DC indications.
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // format: 6u
    FIELD_UINT CYLPF_CDCIndY_Gain_Der_3[4];

    ////////////////////////////
    // Threshold modifiers depending on DC indications.
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // format: 6u
    FIELD_UINT CYLPF_CDCIndY_Gain_Der_5[4];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // format: 6u
    FIELD_UINT CYLPF_CDCIndY_Gain_Der_7[4];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // format: 6u
    FIELD_UINT CYLPF_CDCIndY_Gain_Der_9[4];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // format: 6u
    FIELD_UINT CYLPF_CDCIndY_Gain_Der_5X[4];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // format: 6u
    FIELD_UINT CYLPF_CDCIndY_Gain_Der_9X[4];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // format: 6u
    FIELD_UINT CYLPF_CDCIndUV_Gain_Der_3[4];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // format: 6u
    FIELD_UINT CYLPF_CDCIndUV_Gain_Der_5[4];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // format: 6u
    FIELD_UINT CYLPF_CDCIndUV_Gain_Der_7[4];

    ////////////////////////////
    // Threshold modifiers depending on DC indications. 
    // For field range definitions, see Table 6.
    ////////////////////////////
    // DC0       StartBit:0      Default:42
    // DC1       StartBit:6      Default:42
    // DC2       StartBit:12     Default:42
    // DC3       StartBit:18     Default:42
    // format: 6u
    FIELD_UINT CYLPF_CDCIndUV_Gain_Der_5X[4];

    // Enable dithering for Luma filter
    // format: 1u
    FIELD_UINT CYLPF_DITHERING_YFIlterEn;

    // Enable dithering for Chroma filter
    // format: 1u
    FIELD_UINT CYLPF_DITHERING_CFIlterEn;

    // Pseudo random seed.
    // Should be calculated for each stripe to ensure match between stripes and single stripe run 
    // format: 30u
    FIELD_UINT CYLPF_DITHERING_seed;

} CYLPF_REG;

// ############ Functions ############
int32_t Validate_CYLPF_REG( CYLPF_REG* regStruct, uint32_t mE  );
void SetDefaultVal_CYLPF_REG( CYLPF_REG* regStruct, uint32_t mE );
// ###################################
typedef struct CYLPF_FILTER2_REG_TAG
{

    // Bypass register for the CYLPF_Filter2 block.  Affected by CYLPF_Filter2_Enable
    // format: 1u
    FIELD_UINT CYLPF_Filter2_Bypass;

    // Debug mode for filter2 block
    // format: 1u
    FIELD_UINT CYLPF_Filter2_Debug;

    ////////////////////////////
    // Bypass Sum modes register
    ////////////////////////////
    // Bypass register for the indications Filtering block. Affected by CYLPF_Filter2_Enable_ChromaInd
    // format: 1u
    FIELD_UINT CYLPF_Filter2_Bypass_Sub__Ind;

    // Bypass register for the Chroma indications Filtering block. Affected by CYLPF_Filter2_Enable_ChromaInd & CYLPF_Filter2_Enable_GreyInd
    // format: 1u
    FIELD_UINT CYLPF_Filter2_Bypass_Sub__ChromaInd;

    // Bypass register for the Grey indication block.  Affected by CYLPF_Filter2_Enable_GreyInd
    // format: 1u
    FIELD_UINT CYLPF_Filter2_Bypass_Sub__GreyInd;

    // Bypass register for the Chroma Bilateral filter block .  Affected by CYLPF_Filter2_Enable_ChromaBilat
    // format: 1u
    FIELD_UINT CYLPF_Filter2_Bypass_Sub__ChromaBilat;

    // Bypass register for the Chroma Median filter block .  Affected by CYLPF_Filter2_Enable_ChromaMedian
    // format: 1u
    FIELD_UINT CYLPF_Filter2_Bypass_Sub__ChromaMedian;

    ////////////////////////////
    // Bypass register for the CYLPF_Filter2 sub-blocks.
    ////////////////////////////
    // format: 3u
    FIELD_UINT CYLPF_Filter2_ChromaFiltering__MinimalSize;

    // Threshold register for chroma indication filter
    // format: 4u
    FIELD_UINT CYLPF_Filter2_ChromaInd_THR9;

    // Threshold register for chroma indication filter
    // format: 4u
    FIELD_UINT CYLPF_Filter2_ChromaInd_THR11;

    // Threshold register for grey indication filter
    // format: 5u
    FIELD_UINT CYLPF_Filter2_GreyInd_THR9;

    // Threshold register for chroma indication filter
    // format: 6u
    FIELD_UINT CYLPF_Filter2_GreyInd_THR11;

    ////////////////////////////
    // Configuration register for the chroma bilateral filter block filter size
    ////////////////////////////
    // format: 2u
    FIELD_UINT CYLPF_Filter2_Chroma__FilterSize;

    // Configuration register for the chroma bilateral filter block symmetrical/asymmetrical decisions for filter
    // format: 1u
    FIELD_UINT CYLPF_Filter2_Chroma__Asym;

    ////////////////////////////
    // Configuration register for MedianRadius 1x1.
    // In order to get a MedainRadius = 0 (1x1), at least one of the following conditions must be met.
    // (Only 3x3inner field is signed, the rest of the fields are unsigned)
    ////////////////////////////
    // Note: By choosing (-1) the condition will always be met.
    // format: 2s
    FIELD_INT CYLPF_Filter2_Median_Cnt1Thr__T3x3inner;

    // The DetailCnt in the 3x3 environment around the current pixel,
    // need to be bigger than the threshold in order to get MedianRadius = 0 (1x1)
    // format: 4u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1Thr__T3x3;

    // The DetailCnt in the 3x3 window corners, 
    // need to be bigger than the threshold in order to get MedianRadius = 0 (1x1)
    // format: 3u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1Thr__T3x3corners;

    // The DetailCnt in the 5x5 environment around the current pixel, 
    // need to be bigger than the threshold in order to get MedianRadius = 0 (1x1)
    // format: 5u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1Thr__T5x5;

    // The DetailCnt in one of the sides of the 5x5 window around the current pixel,
    // need to be bigger than the threshold in order to get MedianRadius = 0 (1x1)
    // format: 3u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1Thr__T5x5sides;

    // The DetailCnt in the 5x5 window corners,
    // need to be bigger than the threshold in order to get MedianRadius = 0 (1x1)
    // format: 3u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1Thr__T5x5corners;

    ////////////////////////////
    // Configuration register for MedianRadiusHor and MedianRadiusVer 1x1.
    // In order to get a MedainRadius[Hor/Ver] = 0 (1x1), at least one of 
    // the following conditions must be met.
    // All fields are unsigned.
    ////////////////////////////
    // The DetailCnt must be >= than the 1x1 threshold in order to get MedianRadius[Hor/Ver] = 0 (1x1)
    // format: 2u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrHV__T1x1;

    // The DetailCnt in the 3x1 environment around the current pixel,
    // need to be >= than the threshold in order to get MedianRadius[Hor/Ver] = 0 (1x1)
    // format: 3u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrHV__T3x1;

    // The DetailCnt in the 5x1 environment around the current pixel,
    // need to be >= than the threshold in order to get MedianRadius[Hor/Ver] = 0 (1x1)
    // format: 3u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrHV__T5x1;

    // The DetailCnt in the 3x3 environment around the current pixel,
    // need to be >= than the threshold in order to get MedianRadius[Hor/Ver] = 0 (1x1)
    // format: 4u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrHV__T3x3;

    // The DetailCnt in the 5x3 environment around the current pixel,
    // need to be >= than the threshold in order to get MedianRadius[Hor/Ver] = 0 (1x1)
    // format: 5u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrHV__T5x3;

    // The DetailCnt in the 5x5 environment around the current pixel,
    // need to be >= than the threshold in order to get MedianRadius[Hor/Ver] = 0 (1x1)
    // format: 5u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrHV__T5x5;

    ////////////////////////////
    // Configuration register for MedianRadiusDg1 and MedianRadiusDg2 1x1.
    // In order to get a MedainRadius[Dg1/Dg2] = 0 (1x1), 
    // at least one of the following conditions must be met.
    // All fields are unsigned.
    ////////////////////////////
    // The DetailCnt must be >= than the 1x1 threshold in order to get MedianRadius[Dg1/Dg2] = 0 (1x1)
    // format: 2u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrDg12__T1x1;

    // The DetailCnt in the 3x1 environment around the current pixel, 
    // need to be >= than the threshold in order to get MedianRadius[Dg1/Dg2] = 0 (1x1)
    // format: 3u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrDg12__T3x1;

    // The DetailCnt in the 5x1 environment around the current pixel, 
    // need to be >= than the threshold in order to get MedianRadius[Dg1/Dg2] = 0 (1x1)
    // format: 3u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrDg12__T5x1;

    // The DetailCnt in the 3x3 environment around the current pixel, 
    // need to be >= than the threshold in order to get MedianRadius[Dg1/Dg2] = 0 (1x1)
    // format: 4u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrDg12__T3x3;

    // The DetailCnt in the 5x3 environment around the current pixel, 
    // need to be >= than the threshold in order to get MedianRadius[Dg1/Dg2] = 0 (1x1)
    // format: 5u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrDg12__T5x3;

    // The DetailCnt in the 5x5 environment around the current pixel, 
    // need to be >= than the threshold in order to get MedianRadius[Dg1/Dg2] = 0 (1x1)
    // format: 5u
    FIELD_UINT CYLPF_Filter2_Median_Cnt1ThrDg12__T5x5;

} CYLPF_FILTER2_REG;

// ############ Functions ############
int32_t Validate_CYLPF_FILTER2_REG( CYLPF_FILTER2_REG* regStruct  );
void SetDefaultVal_CYLPF_FILTER2_REG( CYLPF_FILTER2_REG* regStruct );
// ###################################
typedef struct DCBLEND2_REG_TAG
{

    // Bypass & mode register for the Luma DCBlend2 block
    // format: 1u
    FIELD_UINT DCBlend2_Bypass_Luma;

    // Bypass & mode register for the Chroma DCBlend2 block
    // format: 1u
    FIELD_UINT DCBlend2_Bypass_Chroma;

    // debug mode for luma channel
    // format: 1u
    FIELD_UINT DCBlend2_Y_debug;

    // debug mode for chroma channel 
    // format: 1u
    FIELD_UINT DCBlend2_C_debug;

    ////////////////////////////
    // Configuration register for the DCBlend2 block
    ////////////////////////////
    // Used to control edge map output of the DCBlend2 unit
    // format: 2u
    FIELD_UINT DCBlend2_Luma_Cfg__EdgeMapMode;

    // Controls the blending calculation mode – add highpass / blend, Ydc
    // format: 1u
    FIELD_UINT DCBlend2_Luma_Cfg__YDC_HP_MODE;

    // Controls the blending calculation mode – add highpass / blend, Yin
    // format: 1u
    FIELD_UINT DCBlend2_Luma_Cfg__YIN_HP_MODE;

    // Allows bypassing the DCNF luma input
    // format: 1u
    FIELD_UINT DCBlend2_Luma_Cfg__LumaDCNF_Bypass;

    // Allows bypassing the Luma InBlend factor interpolation
    // format: 1u
    FIELD_UINT DCBlend2_Luma_Cfg__YInBlendInter_Bypass;

    // Allows bypassing the Luma DCBlend factor interpolation
    // format: 1u
    FIELD_UINT DCBlend2_Luma_Cfg__YDcBlendInter_Bypass;

    ////////////////////////////
    // Configuration register for the DCBlend2 block
    ////////////////////////////
    // Enable/disable DC-Blending according to GreyDCSize
    // format: 1u
    FIELD_UINT DCBlend2_Chroma_Cfg__CBlend_GreyMode;

    // Enable/disable size condition when selecting between chroma DC-blending according to MainDCSize and chroma DC-blending according to GreyDCSize,
    // meaning that DC-blending according to GreyDCSize is performed only if GreyDCSize is bigger than MainDCSize
    // format: 1u
    FIELD_UINT DCBlend2_Chroma_Cfg__CBlend_SizeCond;

    // Enable/disable chroma condition when selecting between chroma DC-blending according to MainDCSize and chroma DC-blending according to GreyDCSize, 
    // meaning that DC-blending according to GreyDCSize is performed only if  the DC pixel has lower chromatisity then current pixel
    // format: 1u
    FIELD_UINT DCBlend2_Chroma_Cfg__CBlend_ChromaCond;

    // Determines which value is used to calculate the 1-bit DC indication generated by the DCBlend2
    // format: 2u
    FIELD_UINT DCBlend2_Chroma_Cfg__CDCInd_Cfg;

    // Minimum value of CDCSize_Ind, above which CDCInd will be turned on
    // format: 3u
    FIELD_UINT DCBlend2_Chroma_Cfg__CDCInd_SizeThr;

    // Used to control edge map output of the DCBlend2 unit
    // format: 2u
    FIELD_UINT DCBlend2_Chroma_Cfg__EdgeMapMode;

    // Controls the blending calculation mode – add highpass / blend, Cdc
    // format: 1u
    FIELD_UINT DCBlend2_Chroma_Cfg__CDC_HP_MODE;

    // Allows bypassing the Chroma DCBlend factor interpolation
    // format: 1u
    FIELD_UINT DCBlend2_Chroma_Cfg__CDcBlendInter_Bypass;

    ////////////////////////////
    // Luma Ydc blend factors for 1x1 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_YBlend_1x1__LNR0_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_1x1__LNR1_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_1x1__LNR0_CNR1;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_1x1__LNR1_CNR1;

    ////////////////////////////
    // Luma Ydc blend factors for 3x3 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_YBlend_3x3__LNR0_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_3x3__LNR1_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_3x3__LNR0_CNR1;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_3x3__LNR1_CNR1;

    ////////////////////////////
    // Luma Ydc blend factors for 5x5 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_YBlend_5x5__LNR0_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_5x5__LNR1_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_5x5__LNR0_CNR1;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_5x5__LNR1_CNR1;

    ////////////////////////////
    // Luma Ydc blend factors for 7x7 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_YBlend_7x7__LNR0_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_7x7__LNR1_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_7x7__LNR0_CNR1;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_7x7__LNR1_CNR1;

    ////////////////////////////
    // Luma Ydc blend factors for 9x9 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_YBlend_9x9__LNR0_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_9x9__LNR1_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_9x9__LNR0_CNR1;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_9x9__LNR1_CNR1;

    ////////////////////////////
    // Luma Yalt blend factors for 1x1 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_YBlend_1x1_alt__LNR0_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_1x1_alt__LNR1_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_1x1_alt__LNR0_CNR1;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_1x1_alt__LNR1_CNR1;

    ////////////////////////////
    // Luma Yalt blend factors for 3x3 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_YBlend_3x3_alt__LNR0_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_3x3_alt__LNR1_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_3x3_alt__LNR0_CNR1;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_3x3_alt__LNR1_CNR1;

    ////////////////////////////
    // Luma Yalt blend factors for 5x5 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_YBlend_5x5_alt__LNR0_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_5x5_alt__LNR1_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_5x5_alt__LNR0_CNR1;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_5x5_alt__LNR1_CNR1;

    ////////////////////////////
    // Luma Yalt blend factors for 7x7 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_YBlend_7x7_alt__LNR0_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_7x7_alt__LNR1_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_7x7_alt__LNR0_CNR1;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_7x7_alt__LNR1_CNR1;

    ////////////////////////////
    // Luma Yalt blend factors for 9x9 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_YBlend_9x9_alt__LNR0_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_9x9_alt__LNR1_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_9x9_alt__LNR0_CNR1;

    // format: 8u
    FIELD_UINT DCBlend2_YBlend_9x9_alt__LNR1_CNR1;

    ////////////////////////////
    // Chroma blend factor for 1x1 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_CBlend_1x1__LNR_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_CBlend_1x1__LNR_CNR1;

    ////////////////////////////
    // Chroma blend factor for 3x3 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_CBlend_3x3__LNR_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_CBlend_3x3__LNR_CNR1;

    ////////////////////////////
    // Chroma blend factor for 5x5 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_CBlend_5x5__LNR_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_CBlend_5x5__LNR_CNR1;

    ////////////////////////////
    // Chroma blend factor for 7x7 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_CBlend_7x7__LNR_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_CBlend_7x7__LNR_CNR1;

    ////////////////////////////
    // Chroma blend factor for 9x9 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_CBlend_9x9__LNR_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_CBlend_9x9__LNR_CNR1;

    ////////////////////////////
    // Chroma blend factor for 11x11 dc pixels
    ////////////////////////////
    // format: 8u
    FIELD_UINT DCBlend2_CBlend_11x11__LNR_CNR0;

    // format: 8u
    FIELD_UINT DCBlend2_CBlend_11x11__LNR_CNR1;

} DCBLEND2_REG;

// ############ Functions ############
int32_t Validate_DCBLEND2_REG( DCBLEND2_REG* regStruct  );
void SetDefaultVal_DCBLEND2_REG( DCBLEND2_REG* regStruct );
// ###################################
typedef struct RNF_REG_TAG
{

    ////////////////////////////
    // Bypass register for the whole RNF block
    ////////////////////////////
    // Affected by RNF_Enable_LUMA
    // format: 1u
    FIELD_UINT RNF_BYPASS__LUMA;

    // Affected by RNF_Enable_CHROMA
    // format: 1u
    FIELD_UINT RNF_BYPASS__CHROMA;

    ////////////////////////////
    // Inner bypass controls for logics inside the RNF
    ////////////////////////////
    // Affected by RNF_Enable_PEAK_LUMA
    // format: 1u
    FIELD_UINT RNF_BYPASS_SUB__PEAK_LUMA;

    // Affected by RNF_Enable_FIR_LUMA
    // format: 1u
    FIELD_UINT RNF_BYPASS_SUB__FIR_LUMA;

    // Affected by RNF_Enable_IND_RANK_LUMA
    // format: 1u
    FIELD_UINT RNF_BYPASS_SUB__IND_RANK_LUMA;

    // Affected by RNF_Enable_PEAK_CHROMA
    // format: 1u
    FIELD_UINT RNF_BYPASS_SUB__PEAK_CHROMA;

    // Affected by RNF_Enable_FIR_CHROMA
    // format: 1u
    FIELD_UINT RNF_BYPASS_SUB__FIR_CHROMA;

    // Affected by RNF_Enable_IND_RANK_CHROMA
    // format: 1u
    FIELD_UINT RNF_BYPASS_SUB__IND_RANK_CHROMA;

    ////////////////////////////
    // Debug register for RNF
    ////////////////////////////
    // When enabled, replaces valid output indications with pixel values for Luma
    // format: 1u
    FIELD_UINT RNF_DEBUG__OUTIND_LUMA;

    // When enabled, replaces valid output indications with pixel values for Chroma
    // format: 1u
    FIELD_UINT RNF_DEBUG__OUTIND_CHROMA;

    // When enabled, replaces valid input indications with pixel values for Luma
    // format: 2u
    FIELD_UINT RNF_DEBUG__ININD_LUMA;

    // When enabled, replaces valid input indications with pixel values for Chroma
    // format: 2u
    FIELD_UINT RNF_DEBUG__ININD_CHROMA;

    // Length of the indications rank filter
    // format: 1u
    FIELD_UINT RNF_INDRANK_Y_CFG__LENGTH;

    // Width of the indications rank filter
    // format: 1u
    FIELD_UINT RNF_INDRANK_Y_CFG__WIDTH;

    // Threshold for comparing against the dcsize indications parallel to the current direction
    // format: 3u
    FIELD_UINT RNF_INDRANK_Y_CFG__PAR_THR;

    // Threshold for comparing against the dcsize indications perpendicular to the current direction
    // format: 3u
    FIELD_UINT RNF_INDRANK_Y_CFG__PRP_THR;

    // When analyzing diagonal directions, this threshold is compared against the H/V dcsize indications
    // format: 3u
    FIELD_UINT RNF_INDRANK_Y_CFG__DG_PRP_HV_THR;

    // Allows bypassing logic of luma indications rank filter
    // format: 1u
    FIELD_UINT RNF_INDRANK_Y_CFG__BYPASS;

    // Threshold for performing OR over the rank result with center pixel
    // format: 3u
    FIELD_UINT RNF_INDRANK_Y_CFG__CENTER_PAR_THR;

    // Count threshold for comparison against the amount of dc indications in a 5x1, or 3x1 window (depending on LENGTH)
    // format: 3u
    FIELD_UINT RNF_INDRANK_Y_HV__PAR_CNT0;

    // Count threshold for comparison agains the amount of dc indications in a 3x3, or a 5x3slim (11 pixels) window, depending on LENGTH
    // format: 4u
    FIELD_UINT RNF_INDRANK_Y_HV__PAR_CNT1;

    // Count threshold for comparison against the amount of dc indications in a 5x3 window
    // format: 5u
    FIELD_UINT RNF_INDRANK_Y_HV__PAR_CNT2;

    // Threshold bit for comparing against the perpendicular indication of the center pixel, required for wide rank filtering
    // format: 1u
    FIELD_UINT RNF_INDRANK_Y_HV__PRP_CNT0;

    // Count threshold for comparing against the perpendicular cumulative sum of 3 pixels, required for wide rank filtering
    // format: 2u
    FIELD_UINT RNF_INDRANK_Y_HV__PRP_CNT1;

    // Count threshold for comparing against the perpendicular cumulative sum of 5 pixels, required for wide rank filtering
    // format: 3u
    FIELD_UINT RNF_INDRANK_Y_HV__PRP_CNT2;

    // Count threshold for comparison against the amount of dc indications in a 5x1, or 3x1 window (depending on LENGTH)
    // format: 3u
    FIELD_UINT RNF_INDRANK_Y_DG__PAR_CNT0;

    // Count threshold for comparison agains the amount of dc indications in a 3x3, or a 5x3slim (11 pixels) window, depending on LENGTH
    // format: 4u
    FIELD_UINT RNF_INDRANK_Y_DG__PAR_CNT1;

    // Count threshold for comparison against the amount of dc indications in a 5x3 window
    // format: 5u
    FIELD_UINT RNF_INDRANK_Y_DG__PAR_CNT2;

    // Threshold bit for comparing against the perpendicular indication of the center pixel, required for wide rank filtering
    // format: 1u
    FIELD_UINT RNF_INDRANK_Y_DG__PRP_CNT0;

    // Count threshold for comparing against the perpendicular cumulative sum of 3 pixels, required for wide rank filtering
    // format: 2u
    FIELD_UINT RNF_INDRANK_Y_DG__PRP_CNT1;

    // Count threshold for comparing against the perpendicular cumulative sum of 5 pixels, required for wide rank filtering
    // format: 3u
    FIELD_UINT RNF_INDRANK_Y_DG__PRP_CNT2;

    // Width of the indications rank filter
    // format: 1u
    FIELD_UINT RNF_INDRANK_C_CFG__WIDTH;

    // Threshold for comparing against the dcsize indications parallel to the current direction
    // format: 3u
    FIELD_UINT RNF_INDRANK_C_CFG__PAR_THR;

    // Threshold for comparing against the dcsize indications perpendicular to the current direction
    // format: 3u
    FIELD_UINT RNF_INDRANK_C_CFG__PRP_THR;

    // When analyzing diagonal directions, this threshold is compared against the H/V dcsize indications
    // format: 3u
    FIELD_UINT RNF_INDRANK_C_CFG__DG_PRP_HV_THR;

    // Allows bypassing logic of chroma indications rank filter
    // format: 1u
    FIELD_UINT RNF_INDRANK_C_CFG__BYPASS;

    // Threshold for performing OR over the rank result with center pixel
    // format: 3u
    FIELD_UINT RNF_INDRANK_C_CFG__CENTER_PAR_THR;

    // Count threshold for comparison against the amount of dc indications in a 3x1 window
    // format: 3u
    FIELD_UINT RNF_INDRANK_C_HV__PAR_CNT0;

    // Count threshold for comparison agains the amount of dc indications in a 3x3
    // format: 4u
    FIELD_UINT RNF_INDRANK_C_HV__PAR_CNT1;

    // format: 1u
    FIELD_UINT RNF_INDRANK_C_HV__PRP_CNT0;

    // format: 2u
    FIELD_UINT RNF_INDRANK_C_HV__PRP_CNT1;

    // format: 3u
    FIELD_UINT RNF_INDRANK_C_DG__PAR_CNT0;

    // format: 4u
    FIELD_UINT RNF_INDRANK_C_DG__PAR_CNT1;

    // format: 1u
    FIELD_UINT RNF_INDRANK_C_DG__PRP_CNT0;

    // format: 2u
    FIELD_UINT RNF_INDRANK_C_DG__PRP_CNT1;

    ////////////////////////////
    // Configuration register for the RNF luma PEAK logic
    ////////////////////////////
    // format: 1u
    FIELD_UINT RNF_PEAK_CFG_Y__PEAK_CORR_MODE;

    // Determines the rank result chosen when using rank filter mode
    // format: 2u
    FIELD_UINT RNF_PEAK_CFG_Y__PEAK_CORR_RANK;

    // 0=none
    // 1=(1/8)
    // 2=(1/4)
    // 3=(1/2)
    // Determines whether the center pixel is averaged into Y peak result, and how much
    // format: 2u
    FIELD_UINT RNF_PEAK_CFG_Y__PEAK_CENTER_W;

    ////////////////////////////
    // Configuration register for the RNF chroma PEAK logic
    ////////////////////////////
    // format: 1u
    FIELD_UINT RNF_PEAK_CFG_C__PEAK_CORR_MODE;

    // Determines the rank result chosen when using rank filter mode
    // format: 2u
    FIELD_UINT RNF_PEAK_CFG_C__PEAK_CORR_RANK;

    // 0=none
    // 1=(1/8)
    // 2=(1/4)
    // 3=(1/2)
    // Determines whether the center pixel is averaged into U/V peak result, and how much
    // format: 2u
    FIELD_UINT RNF_PEAK_CFG_C__PEAK_CENTER_W;

    ////////////////////////////
    // Condition thresholds for peak detection on luma channel, to use with conditions #1, #2
    ////////////////////////////
    // format: 8u
    FIELD_UINT RNF_PEAK_IMGCONDS0_Y__LEVEL_THR;

    // format: 8u
    FIELD_UINT RNF_PEAK_IMGCONDS0_Y__DC_THR;

    // format: 8u
    FIELD_UINT RNF_PEAK_IMGCONDS0_Y__DCLEVEL_THR;

    ////////////////////////////
    // Set the relative conditions of declaring a relative luma peak between the environment activity and the pixel distance from average.
    // A peak will be identified if:
    // ( pixel-average ) > max(MINVAL, SLOPE*(envmax-envmin)+OFFSET)
    ////////////////////////////
    // Minimal condition for relative peak identification
    // format: 8u
    FIELD_UINT RNF_PEAK_IMGCONDS1_Y__MINVAL;

    // Condition offset for relative peak ident.
    // format: 8u
    FIELD_UINT RNF_PEAK_IMGCONDS1_Y__OFFSET;

    // Condition slope for relative peak identification (0,5,5)
    // format: 10uQ5
    FIELD_UINT RNF_PEAK_IMGCONDS1_Y__SLOPE;

    ////////////////////////////
    // Condition thresholds for peak detection on luma channel, to use with conditions #1, #2
    ////////////////////////////
    // Threshold for peak condition #1 –
    // center pixel must be larger/smaller than all its' neighbours by at least this amount
    // format: 8u
    FIELD_UINT RNF_PEAK_IMGCONDS0_C__LEVEL_THR;

    // DC threshold for peak condition #3 –
    // environment activity (max-min) must be smaller than this amount
    // format: 8u
    FIELD_UINT RNF_PEAK_IMGCONDS0_C__DC_THR;

    // Threshold for peak condition #2 – 
    // center pixel must be larger/smaller than environment average by at least this amount
    // format: 8u
    FIELD_UINT RNF_PEAK_IMGCONDS0_C__DCLEVEL_THR;

    ////////////////////////////
    // Set the relative conditions of declaring a relative luma peak between the environment activity and the pixel distance from average.
    // A peak will be identified if:
    // ( pixel-average ) > max(MINVAL, SLOPE*(envmax-envmin)+OFFSET)
    ////////////////////////////
    // Minimal condition for relative peak identification
    // format: 8u
    FIELD_UINT RNF_PEAK_IMGCONDS1_C__MINVAL;

    // Condition offset for relative peak ident.
    // format: 8u
    FIELD_UINT RNF_PEAK_IMGCONDS1_C__OFFSET;

    // Condition slope for relative peak identification (0,5,5)
    // format: 10uQ5
    FIELD_UINT RNF_PEAK_IMGCONDS1_C__SLOPE;

    // format: 3u
    FIELD_UINT RNF_PEAK_DCCONDS0_Y__C1x1_DCTHR;

    // format: 4u
    FIELD_UINT RNF_PEAK_DCCONDS0_Y__C1x1_DCCNT3;

    // format: 5u
    FIELD_UINT RNF_PEAK_DCCONDS0_Y__C1x1_DCCNT5;

    // format: 3u
    FIELD_UINT RNF_PEAK_DCCONDS0_Y__C3x3_DCTHR;

    // format: 4u
    FIELD_UINT RNF_PEAK_DCCONDS0_Y__C3x3_DCCNT3;

    // format: 5u
    FIELD_UINT RNF_PEAK_DCCONDS0_Y__C3x3_DCCNT5;

    // format: 3u
    FIELD_UINT RNF_PEAK_DCCONDS1_Y__C5x5_DCTHR;

    // format: 4u
    FIELD_UINT RNF_PEAK_DCCONDS1_Y__C5x5_DCCNT3;

    // format: 5u
    FIELD_UINT RNF_PEAK_DCCONDS1_Y__C5x5_DCCNT5;

    // format: 3u
    FIELD_UINT RNF_PEAK_DCCONDS1_Y__C7x7_DCTHR;

    // format: 4u
    FIELD_UINT RNF_PEAK_DCCONDS1_Y__C7x7_DCCNT3;

    // format: 5u
    FIELD_UINT RNF_PEAK_DCCONDS1_Y__C7x7_DCCNT5;

    // format: 3u
    FIELD_UINT RNF_PEAK_DCCONDS0_C__C1x1_DCTHR;

    // format: 4u
    FIELD_UINT RNF_PEAK_DCCONDS0_C__C1x1_DCCNT3;

    // format: 3u
    FIELD_UINT RNF_PEAK_DCCONDS0_C__C3x3_DCTHR;

    // format: 4u
    FIELD_UINT RNF_PEAK_DCCONDS0_C__C3x3_DCCNT3;

    // format: 3u
    FIELD_UINT RNF_PEAK_DCCONDS1_C__C5x5_DCTHR;

    // format: 4u
    FIELD_UINT RNF_PEAK_DCCONDS1_C__C5x5_DCCNT3;

    // format: 3u
    FIELD_UINT RNF_PEAK_DCCONDS1_C__C7x7_DCTHR;

    // format: 4u
    FIELD_UINT RNF_PEAK_DCCONDS1_C__C7x7_DCCNT3;

    ////////////////////////////
    // Thresholds for performing isotropic peak correction
    ////////////////////////////
    // Isotropic peak correction Y threshold
    // format: 8u
    FIELD_UINT RNF_PEAK_LUMA_ISOTROPIC_THRS__YTHR;

    ////////////////////////////
    // Thresholds for performing isotropic peak correction
    ////////////////////////////
    // Isotropic peak correction V thresh
    // format: 8u
    FIELD_UINT RNF_PEAK_CHROMA_ISOTROPIC_THRS__UTHR;

    // Isotropic peak correction U thresh
    // format: 8u
    FIELD_UINT RNF_PEAK_CHROMA_ISOTROPIC_THRS__VTHR;

    ////////////////////////////
    // Configuration register for RNF FIR logic
    ////////////////////////////
    // Determines whether a complete 5x5 filtering can be decided by the YFIR directional logic
    // format: 1u
    FIELD_UINT RNF_FIR_CFG_Y__FIR_DIR_ISOTROPIC;

    // Determines the relative weight of 3x3 pixels in the YFIR filter
    // format: 2u
    FIELD_UINT RNF_FIR_CFG_Y__FIR_3X3_WEIGHT;

    // Determines the relative weight of 5x5H/V pixels in the YFIR filter
    // format: 1u
    FIELD_UINT RNF_FIR_CFG_Y__FIR_5X5HV_WEIGHT;

    // Determines the operational window of the YFIR transition logic
    // format: 2u
    FIELD_UINT RNF_FIR_CFG_Y__FIR_TRANS_SIZE;

    ////////////////////////////
    // Configuration register for RNF FIR logic
    ////////////////////////////
    // Determines whether a complete 3x3 filtering can be decided by the CFIR directional logic
    // format: 1u
    FIELD_UINT RNF_FIR_CFG_C__FIR_DIR_ISOTROPIC;

    // Determines the relative weight of 1x1 pixel in the CFIR filter
    // format: 2u
    FIELD_UINT RNF_FIR_CFG_C__FIR_1X1_WEIGHT;

    // Determines the relative weight of 3x3 pixels in the CFIR filter
    // format: 2u
    FIELD_UINT RNF_FIR_CFG_C__FIR_3X3_WEIGHT;

    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_Y__PAR_CEN_THR;

    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_Y__PAR_LINE_THR;

    // format: 2u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_Y__PAR3_LINE_CNT;

    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_Y__PAR5_LINE_CNT;

    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_Y__PRP_CEN_THR;

    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_Y__PRP_LINE_THR;

    // format: 2u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_Y__PRP3_LINE_CNT;

    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_Y__PRP5_LINE_CNT;

    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_Y__DG_CEN_THR;

    // 6 value is used to bypass the threshold comparison
    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_C__PAR_CEN_THR;

    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_C__PAR_LINE_THR;

    // format: 2u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_C__PAR3_LINE_CNT;

    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_C__PRP_CEN_THR;

    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_C__PRP_LINE_THR;

    // format: 2u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_C__PRP3_LINE_CNT;

    // format: 3u
    FIELD_UINT RNF_FIR_DIRECTIONAL_THR_C__DG_CEN_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_0__C1;

    // format: 3u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_0__P1_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_0__P1_CNT;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_0__C3;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_0__M3;

    // format: 3u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_0__P3_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_0__P3_CNT;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_1__C5;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_1__M5;

    // format: 3u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_1__P5_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_1__P5_CNT;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_2__C7;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_2__M7;

    // format: 3u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_2__P7_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_2__P7_CNT;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_2__P9_CNT;

    // format: 3u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_2__P3X3_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_Y_CNT_2__P3X3_CNT;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_0__C1;

    // format: 3u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_0__P1_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_0__P1_CNT;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_0__C3;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_0__M3;

    // format: 3u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_0__P3_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_0__P3_CNT;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_1__C5;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_1__M5;

    // format: 3u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_1__P5_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_1__P5_CNT;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_2__C7;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_2__M7;

    // format: 3u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_2__P7_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_2__P7_CNT;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_2__P9_CNT;

    // format: 3u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_2__P3X3_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_2__P3X3_CNT;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_3__C9;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_3__M9;

    // format: 3u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_3__P9_THR;

    // format: 5u
    FIELD_UINT RNF_FIR_TRANS_C_CNT_3__P11_CNT;

} RNF_REG;

// ############ Functions ############
int32_t Validate_RNF_REG( RNF_REG* regStruct  );
void SetDefaultVal_RNF_REG( RNF_REG* regStruct );
// ###################################
typedef struct PDI_PACK_REG_TAG
{

    // 0 - Enable; 1 - bypass 
    // format: 1u
    FIELD_UINT PDI_Pack_Bypass;

    // PDI Pack horizontal crop start - counting from start of DCNF input.
    // Pointing to the first pixel to take.
    // Must be an even number due to 2x2 block resolution of the PDI format.
    // format: 10u
    FIELD_UINT PDI_Pack_Crop_HorizStart;

    // PDI Pack horizontal crop end - counting from start of DCNF input (before start cropping).
    // Pointing to the last pixel to take.
    // Must be an odd number due to 2x2 block resolution of the PDI format.
    // format: 10u
    FIELD_UINT PDI_Pack_Crop_HorizEnd;

    // Define which 16B unit inside of 64B chunk is the first to be written
    // format: 2u
    FIELD_UINT PDI_Pack_StartPhase;

    // PDI Pack vertical crop start.
    // Pointing to the first line of pixels to take.
    // Must be an even number due to 2x2 block resolution of the PDI format.
    // format: 14u
    FIELD_UINT PDI_Pack_Crop_VertStart;

    // PDI Pack vertical crop end - counting from start of input (before start cropping).
    // Pointing to the last line of pixels to take.
    // Must be an odd number due to 2x2 block resolution of the PDI format.
    // format: 14u
    FIELD_UINT PDI_Pack_Crop_VertEnd;

} PDI_PACK_REG;

// ############ Functions ############
int32_t Validate_PDI_PACK_REG( PDI_PACK_REG* regStruct  );
void SetDefaultVal_PDI_PACK_REG( PDI_PACK_REG* regStruct );
// ###################################

typedef struct ANR_REG_PASS_TAG
{
    TOP_REG top;
    DCUPSCALER_REG dcupscaler;
    DCBLEND1_REG dcblend1;
    CNR_REG cnr;
    CYLPF_REG cylpf;
    CYLPF_FILTER2_REG cylpf_filter2;
    DCBLEND2_REG dcblend2;
    RNF_REG rnf;
    PDI_PACK_REG pdi_pack;
} ANR_REG_PASS;

// Validates the settings of the registers
// Returns 0 if success
// Else, returns the number of invalid settings
int32_t Validate_ANR (ANR_REG_PASS* reg, uint32_t mE );

// Set the default values of struct
// This function set the internal struct fields and not the packed register
// Therefore, the function PackREG should be called before using the final value of the registers
void SetDefaultVal_ANR(ANR_REG_PASS* reg, uint32_t mE );

#ifdef __cplusplus
}
#endif


#endif //__ANR_REGISTERS_H__

