// NOWHINE ENTIRE FILE 
//-------------------------------------------------------------------------
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------


#ifndef __ANR_CHROMATIX_H__
#define __ANR_CHROMATIX_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "CommonDefs.h"

typedef struct ANR_TOP_TAG
{
    // Enable Luma filtering for entire ANR
    // format: 1u
    PARAM_UINT enable_luma_noise_reduction;

    // Enable Chroma filtering for entire ANR
    // format: 1u
    PARAM_UINT enable_chroma_noise_reduction;

    // Enable Luma filtering for current PASS
    // format: 1u
    PARAM_UINT enable_luma_noise_reduction_pass;

    // Enable Chroma filtering for current PASS
    // format: 1u
    PARAM_UINT enable_chroma_noise_reduction_pass;

} ANR_TOP;

typedef struct ANR_POWER_CONTROL_TAG
{
    // 1 - Standard mode. 
    // 0 - Low power mode, disable chroma filter extension: turn off median, second bilateral and chroma indicaton extention. 
    // format: 1u
    PARAM_UINT enable_chroma_filter_extension;

    // if set to 0, luma_smoothing_treatment_and_peak_treatment will be turned off.
    // format: 1u
    PARAM_UINT enable_luma_smoothing_treatment_and_peak_treatment;

    // if set to 0, chroma_smoothing_treatment_and_peak_treatment will be turned off.
    // format: 1u
    PARAM_UINT enable_chroma_smoothing_treatment_and_peak_treatment;

    // if set to 0, y,u,v thresholds will be dependent only on Ylut (function of y) and not on Uvlut (function of u,v).
    // format: 1u
    PARAM_UINT enable_luma_chroma_filter_all_thresholds_per_uv;

    // if set to 0, u,v thresholds will be a constant and will not be dependent on y,u,v. Constant will be taken from first cell of lut_per_y.
    // format: 1u
    PARAM_UINT enable_luma_chroma_filter_uv_thresholds_per_yuv;

    // if set to 0, luma filter will be dependent only on y derivatives and not u,v derivativs (no need to compare them)
    // format: 1u
    PARAM_UINT enable_luma_filter_uv_thresholds;

} ANR_POWER_CONTROL;

typedef struct ANR_LUMA_CHROMA_FILTER_CONFIG_TAG
{
    // yuv average block size for controling the threshold lut entry. 
    // 0-Y3x3/UV1x1 threshold avg support
    // 1-Y5x5/UV3x3 threshold avg support
    // 2-Y9x9/UV5x5 threshold avg support
    // format: 2u
    PARAM_UINT threshold_lut_control_avg_block_size;

} ANR_LUMA_CHROMA_FILTER_CONFIG;

typedef struct ANR_LUMA_FILTER_CONFIG_TAG
{
    // 0 - normal mode (depends on iisotropic_min, enable_external).
    // 1 - manual settings ( manual mode, config FiltDerFlags register as user like. (can be configured to disconnect the link betwee 5 depnds on 3 for example ... )).
    // format: 1u
    PARAM_UINT filter_decision_mode;

    // 0 - normal mode, each distance depends on distences shorter than him and himself. ( ==  Isotropic mode 1x1 (minimum filter size) )
    // 1 - Isotropic mode 3x3 (minimum filter size)
    // 2 - Isotropic mode 5x5 (minimum filter size)
    // 3 - Isotropic mode 7x7 (minimum filter size)
    // 4 - Isotropic mode 9x9 (minimum filter size)
    // format: 3u
    PARAM_UINT filter_isotropic_min_filter_size;

    // is consider dx derivatives for y filtering
    // 0: don't use
    // 1:use
    // format: 1u
    PARAM_UINT filter_enable_external_derivatives;

    // Condition flags for calculating the luma filtering. Di_j == 0 means that filtering of length i is controlled by detection of length j. (0-consider, 1-ignore)
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
    PARAM_UINT filter_manual_derivatives_flags;

    // 0 - normal mode.
    // 1 - manual settings ( manual mode, config dcindDerFlags register as user like ).
    // format: 1u
    PARAM_UINT dcind_decision_mode;

    // 0 - normal mode, each distance depends on distences shorter than him and himself. ( ==  Isotropic mode 1x1 (minimum dc indication size) )
    // 1 - Isotropic mode 3x3 (minimum dc indication size)
    // 2 - Isotropic mode 5x5 (minimum dc indication size)
    // 3 - Isotropic mode 7x7 (minimum dc indication size)
    // 4 - Isotropic mode 9x9 (minimum dc indication size)
    // format: 3u
    PARAM_UINT dcind_isotropic_min_size;

    // is consider dx derivatives for y dc indications
    // 0: don't use
    // 1:use
    // format: 1u
    PARAM_UINT dcind_enable_external_derivatives;

    // Condition flags for calculating the luma dc indications. Di_j == 0 means that dc indication of length i is controlled by detection of length j. (0-consider, 1-ignore)
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
    PARAM_UINT dcind_manual_derivatives_flags;

    // use 2'nd derivative for 3x3 comparison decision for luma filtering and dcind and preform OR with regular derivative comparison of regular derivatives.
    // format: 1u
    PARAM_UINT enable_use_second_derivative_for_luma_3x3;

    // Controls the influence radius of the 2'nd derivative on the filtering (consider der2 evaluation upto this radius).
    // Demux to registers: FULL_IPU_CYLPF_YFilter_Der2FlagsDCInd_D3_*
    // format: 3u
    PARAM_UINT second_derivative_max_influence_radius_filtering;

    // Controls the influence radius of the 2'nd derivative on the DC Indications (consider der2 evaluation upto this radius).
    // Demux to registers: FULL_IPU_CYLPF_YFilter_Der2FlagsFilt_D3_3*
    // format: 3u
    PARAM_UINT second_derivative_max_influence_radius_dc_indication;

} ANR_LUMA_FILTER_CONFIG;

typedef struct ANR_CHROMA_FILTER_CONFIG_TAG
{
    // 0 - normal mode.
    // 1 - manual settings ( manual mode, config FiltDerFlags register as user like. (can be configured to disconnect the link betwee 5 depnds on 3 for example ... )).
    // format: 1u
    PARAM_UINT filter_decision_mode;

    // 0 - normal mode, each distance depends on distences shorter than him and himself. ( ==  Isotropic mode 1x1 (minimum filter size) )
    // 1 - Isotropic mode 3x3 (minimum filter size)
    // 2 - Isotropic mode 5x5 (minimum filter size)
    // 3 - Isotropic mode 7x7 (minimum filter size)
    // format: 2u
    PARAM_UINT filter_isotropic_min_filter_size;

    // Condition flags for calculating the chroma filtering. Di_j == 0 means that filtering of length i is controlled by detection of length j. (0-consider, 1-ignore)
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
    PARAM_UINT filter_manual_derivatives_flags;

    // 0 - normal mode.
    // 1 - manual settings ( manual mode, config dcindDerFlags register as user like ).
    // format: 1u
    PARAM_UINT dcind_decision_mode;

    // 0 - normal mode, each distance depends on distences shorter than him and himself. ( ==  Isotropic mode 1x1 (minimum dc indication size) )
    // 1 - Isotropic mode 3x3 (minimum dc indication size)
    // 2 - Isotropic mode 5x5 (minimum dc indication size)
    // 3 - Isotropic mode 7x7 (minimum dc indication size)
    // 4 - Isotropic mode 9x9 (minimum dc indication size)
    // 5 - Isotropic mode 11x11 (minimum dc indication size)
    // format: 2u
    PARAM_UINT dcind_isotropic_min_size;

    // Condition flags for calculating the chroma dc indications. Di_j == 0 means that dc indication of length i is controlled by detection of length j. (0-consider, 1-ignore)
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
    PARAM_UINT dcind_manual_derivatives_flags;

} ANR_CHROMA_FILTER_CONFIG;

typedef struct ANR_LUMA_FILTER_KERNEL_TAG
{
    // size of edge filter, configured both for autoconf and manual configuration.
    // 0 - 1x1 ( luma filter bypass).
    // 1 - 3x3.
    // 2 - 5x5
    // 3 - 7x7
    // 4 - 9x9
    // format: 3u
    PARAM_UINT edge_kernel_size;

    // 0 - use abstraction (auto config) for deciding on kernels based on granularity and filter size.
    // 1- use manual configured kernels by the user inside the kernels struct. User MUST set edge kernel, flat weight and flat kernel if weight is greater than zero.
    // format: 1u
    PARAM_UINT kernel_definition_mode;

    // luma filter kernel non manual mode, ratio between center and sides.
    // Lower granularity is more smoothing kernel (more rectangle).
    // Higher granularity is less smoothing kernel (more picky).
    // format: 2u
    PARAM_UINT automatic_definition_granularity;

    // luma filter middle pixel weight.
    // format: 6u
    PARAM_UINT manual_edge_kernel_1x1_center_coefficient;

    // luma filter 3x3 horizontal/vertical pixel shift weights. Actual weight is 2^shift
    // 0 - Weight 1.
    // 1 - Weight 2.
    // 2 - Weight 4
    // 3 - Weight 8
    // format: 2u
    PARAM_UINT manual_edge_kernel_3x3_horver_shift;

    // luma filter 3x3 diagonal pixel shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // 3 - Weight 8
    // format: 2u
    PARAM_UINT manual_edge_kernel_3x3_diag_shift;

    // luma filter 5x5 horizontal/vertical shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_edge_kernel_5x5_horver_shift;

    // luma filter 5x5 diagonal pixel weights.
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_edge_kernel_5x5_diag_shift;

    // luma filter 5x5 off-diagonal (complementary) shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_edge_kernel_5x5_complement_shift;

    // luma filter horizontal/vertical pixel shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_edge_kernel_7x7_horver_shift;

    // luma filter diagonal pixel shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_edge_kernel_7x7_diag_shift;

    // luma filter 7x7 off-diagonal shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_edge_kernel_7x7_complement_shift;

    // luma filter horizontal/vertical shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_edge_kernel_9x9_horver_shift;

    // luma filter 9x9 diagonal shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_edge_kernel_9x9_diag_shift;

    // luma filter 9x9 off-diagonal shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_edge_kernel_9x9_complement_shift;

    // 0: don't include pixels in between main directions, 1 include them only if all directions agree. 2- include them only if the two relevant directions (the direction the pixel is in between) agree.
    // format: 2u
    PARAM_UINT manual_edge_kernel_complement_mode;

    // Max Blend Weight of the flat kernel in regards to Edge kernel. (blend is linear as a function of pixel smoothness)
    // format: 8uQ7
    PARAM_UINT manual_flat_kernel_blend_weight;

    // 0 - 1x1 (luma filter bypass).
    // 1 - 3x3
    // 2 - 5x5
    // 3 - 7x7
    // 4 - 9x9
    // format: 3u
    PARAM_UINT manual_flat_kernel_size;

    // luma filter middle pixel weight.
    // format: 6u
    PARAM_UINT manual_flat_kernel_1x1_center_coefficient;

    // luma filter 3x3 horizontal/vertical pixel shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // 3 - Weight 8
    // format: 2u
    PARAM_UINT manual_flat_kernel_3x3_horver_shift;

    // luma filter 3x3 diagonal pixel shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // 3 - Weight 8
    // format: 2u
    PARAM_UINT manual_flat_kernel_3x3_diag_shift;

    // luma filter 5x5 horizontal/vertical shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_flat_kernel_5x5_horver_shift;

    // luma filter 5x5 diagonal pixel shift weights. Actual weight is 2^shift 
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_flat_kernel_5x5_diag_shift;

    // luma filter 5x5 off-diagonal (complementary) shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_flat_kernel_5x5_complement_shift;

    // luma filter 7x7 horizontal/vertical pixel shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_flat_kernel_7x7_horver_shift;

    // luma filter 7x7 diagonal pixel shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_flat_kernel_7x7_diag_shift;

    // luma filter 7x7 off-diagonal shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_flat_kernel_7x7_complement_shift;

    // luma filter horizontal/vertical shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_flat_kernel_9x9_horver_shift;

    // luma filter 9x9 diagonal shift weights. Actual weight is 2^shift
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_flat_kernel_9x9_diag_shift;

    // luma filter 9x9 off-diagonal shift weights. Actual weight is 2^shift 
    // 0 - Weight 1
    // 1 - Weight 2
    // 2 - Weight 4
    // format: 2u
    PARAM_UINT manual_flat_kernel_9x9_complement_shift;

    // 0: don't include pixels in between main directions, 1 include them only if all directions agree. 2- include them only if the two relevant directions (the direction the pixel is in between) agree.
    // format: 2u
    PARAM_UINT manual_flat_kernel_complement_mode;

} ANR_LUMA_FILTER_KERNEL;

typedef struct ANR_CHROMA_FILTER_KERNEL_TAG
{
    // 0 - 1x1 (YLPF1 bypass).
    // 1 - 3x3.
    // 2 - 5x5
    // 3 - 7x7
    // format: 2u
    PARAM_UINT kernel_size;

} ANR_CHROMA_FILTER_KERNEL;

typedef struct ANR_LUMA_PEAK_MANAGEMENT_TAG
{
    // enable luma peak treatment
    // format: 1u
    PARAM_UINT enable_luma_peak_management;

    // The maximum environment activity of the pixel (the difference between the maximum and minimum Y component values in the pixel neighborhood, 
    // excluding the candidate pixel) is less than LUMA_DCTHRSH.
    // Decreasing this parameter results in fewer peak identifications.
    // 0 = Condition is disabled.
    // format: 8u
    PARAM_UINT detect_hard_decision_environment_activity;

    // The difference between the Y component of the candidate pixel and the average Y component value of its 
    // neighboring pixels (excluding candidate pixel) is at least LUMA_DCLEVEL. 
    // Increasing this parameter results in fewer peak identifications.
    // 255 = Condition is disabled.
    // format: 8u
    PARAM_UINT detect_hard_decision_distance_from_average;

    // soft decision function for distance_from_avg vs activity. 
    // distance_from_avg (for peak)  >=  min ( max ( ( (slope * activity ) +  offset , lower_limit ) , 255);
    // format: 8u
    PARAM_UINT detect_soft_decision_distance_from_average_lower_limit;

    // soft decision function for distance_from_avg vs activity. 
    // distance_from_avg (for peak)  >=  min ( max ( ( (slope * activity ) +  offset , lower_limit ) , 255);
    // 255 is disabled
    // format: 8u
    PARAM_UINT detect_soft_decision_distance_from_average_offset;

    // soft decision function for distance_from_avg vs activity. 
    // distance_from_avg (for peak)  >=  min ( max ( ( (slope * activity ) +  offset , lower_limit ) , 255);
    // format: 10uQ5
    PARAM_UINT detect_soft_decision_distance_from_average_slope;

    // The maximum difference between the Y component of the candidate pixel and the Y component of its neighboring pixels is at least detect_extreme_decision_distance_from_maxmin. 
    // Increasing this parameter results in fewer peak identifications.
    // Max( pixel_val - env_max, env_min-pixel_val )
    // 255 = Condition is disabled.
    // format: 8u
    PARAM_UINT detect_extreme_decision_distance_from_maxmin;

    // Rank peak identification flags a peak when a sufficient number of neighbors around a 
    // pixel are identified as flat (regardless of whether the current pixel is flat or not).
    // The following parameters control the minimum number of flat neighbors for identifying a peak in each length.
    // 128 is all neighbores.
    // 129 is disable.
    // format: 8uQ7
    PARAM_UINT detect_dcsize_decision_sensitivity;

    // 0 - Correct using area smart selection. (closest pixel value in the 3x3 environment)
    // 1 - Correct using flattest direction.
    // area smart is recommended.
    // This parameter should be modified only in FULL pass. DC4, DC16,DC64  Passes should always use area smart selection
    // format: 1u
    PARAM_UINT correction_mode;

    // Area smart selection can be made more robust to larger peaks by increasing correction_nl_mode_thr. This value will result taking a pixel value closer to the median of 3x3. 0 - take the closest pixel value. 3- take the median.
    // format: 2u
    PARAM_UINT correction_area_smart_min_inner_distance;

    // Non edge-preserving is enabled based on following thresholds.
    // Peaks with environment activity below these thresholds are corrected using non edge-preserving filtering.
    // Peaks with environment activity above these thresholds are corrected using edge preserving filtering. (either area smart or flattest direction based on user settings of correction_mode )
    // format: 8u
    PARAM_UINT correction_isotropic_activity_threshold;

} ANR_LUMA_PEAK_MANAGEMENT;

typedef struct ANR_CHROMA_PEAK_MANAGEMENT_TAG
{
    // enable chroma peak treatment
    // format: 1u
    PARAM_UINT enable_chroma_peak_management;

    // The maximum environment activity of the pixel (the difference between the maximum and minimum UV component values in the pixel neighborhood, 
    // excluding the candidate pixel) is less than DCTHRSH.
    // Decreasing this parameter results in fewer peak identifications.
    // 0 = Condition is disabled.
    // format: 8u
    PARAM_UINT detect_hard_decision_environment_activity;

    // The difference between the Y component of the candidate pixel and the average UV component value of its 
    // neighboring pixels (excluding candidate pixel) is at least  DCLEVEL. 
    // Increasing this parameter results in fewer peak identifications.
    // 255 = Condition is disabled.
    // format: 8u
    PARAM_UINT detect_hard_decision_distance_from_average;

    // soft decision function for distance_from_avg vs activity. 
    // distance_from_avg (for peak)  >=  min ( max ( ( (slope * activity ) +  offset , lower_limit ) , 255);
    // format: 8u
    PARAM_UINT detect_soft_decision_distance_from_average_lower_limit;

    // soft decision function for distance_from_avg vs activity. 
    // distance_from_avg (for peak)  >=  min ( max ( ( (slope * activity ) +  offset , lower_limit ) , 255);
    // format: 8u
    PARAM_UINT detect_soft_decision_distance_from_average_offset;

    // soft decision function for distance_from_avg vs activity. 
    // distance_from_avg (for peak)  >=  min ( max ( ( (slope * activity ) +  offset , lower_limit ) , 255);
    // format: 10uQ5
    PARAM_UINT detect_soft_decision_distance_from_average_slope;

    // The maximum difference between the Y component of the candidate pixel and the Y component of its neighboring pixels is at least detect_extreme_decision_distance_from_maxmin. 
    // Increasing this parameter results in fewer peak identifications.
    // Max( pixel_val - env_max, env_min-pixel_val )
    // 255 = Condition is disabled.
    // format: 8u
    PARAM_UINT detect_extreme_decision_distance_from_maxmin;

    // Rank peak identification flags a peak when a sufficient number of neighbors around a 
    // pixel are identified as flat (regardless of whether the current pixel is flat or not).
    // The following parameters control the minimum number of flat neighbors for identifying a peak in each length.
    // 128 is all neighbores.
    // 129 is disable.
    // format: 8uQ7
    PARAM_UINT detect_dcsize_decision_sensitivity;

    // 0 - Correct using area smart selection. (closest pixel value in the 3x3 environment)
    // 1 - Correct using flattest direction.
    // area smart is recommended.
    // This parameter should be modified only in FULL pass. DC4, DC16,DC64  Passes should always use area smart selection
    // format: 1u
    PARAM_UINT correction_mode;

    // Area smart selection can be made more robust to larger peaks by increasing correction_nl_mode_thr. This value will result taking a pixel value closer to the median of 3x3. 0 - take the closest pixel value. 3- take the median.
    // format: 2u
    PARAM_UINT correction_area_smart_min_inner_distance;

    // Non edge-preserving is enabled based on following thresholds.
    // Peaks with environment activity below these thresholds are corrected using non edge-preserving filtering.
    // Peaks with environment activity above these thresholds are corrected using edge preserving filtering. (either area smart or flattest direction based on user settings of correction_mode )
    // format: 8u
    PARAM_UINT correction_isotropic_activity_threshold;

} ANR_CHROMA_PEAK_MANAGEMENT;

typedef struct ANR_INTER_LENGTH_THR_MODIFICATION_TAG
{
    // The strength is the maximal thresh gain factor on current PASS assuming fully flat pixel detected by input indication from the lower pass.
    // The thresh gain factor for every pixel will be based on the maximal value and the flatness of the pixel detected by indication of the lower pass.
    // 
    // 7Q4, example 16 == X1
    // Actual Possible Values are: [16,17,18,20,24,28,30,32,40,48,56,64,72,80] ( == X [ 1, 1.0625, 1.125, 1.25, 1.5, 1.75, 1.875, 2, 2.5, 3, 3.5, 4, 4.5, 5 ] )
    // all other values will be quantized later to one of the Actual Possible Values.
    // format: 7uQ4
    PARAM_UINT luma_input_indication_thr_modification_scale;

    // The strength is the maximal thresh gain factor on current PASS assuming fully flat pixel detected by input indication from the lower pass.
    // The thresh gain factor for every pixel will be based on the maximal value and the flatness of the pixel detected by indication of the lower pass.
    // 
    // 7Q4, example 16 == X1
    // Actual Possible Values are: [16,17,18,20,24,28,30,32,40,48,56,64,72,80] ( == X [ 1, 1.0625, 1.125, 1.25, 1.5, 1.75, 1.875, 2, 2.5, 3, 3.5, 4, 4.5, 5 ] )
    // all other values will be quantized later to one of the Actual Possible Values.
    // format: 7uQ4
    PARAM_UINT chroma_input_indication_thr_modification_scale;

} ANR_INTER_LENGTH_THR_MODIFICATION;

typedef struct ANR_INTER_LENGTH_OUTPUT_INDICATION_TAG
{
    // in case center pixel is flatter than center_binarization_minflatval it will indicate the pixel as flat without any regard to neighbores. If not, the decision will be done according to neighbores decision.
    // format: 3u
    PARAM_UINT luma_center_binarization_minflatval;

    // if set to 1, pixel can be dependant on its neighbores for indicating as flat for higher pass.
    // format: 1u
    PARAM_UINT luma_neighbours_impact_enable;

    // neighbors are counted as flat if they are flatter than neighbours_binarization_minflatval.
    // format: 3u
    PARAM_UINT luma_neighbours_binarization_minflatval;

    // perpendicular neighbourhood size can be 3 or 4.
    // 1- size =3 
    // 2- size =5
    // format: 2u
    PARAM_UINT luma_neighbours_parallel_dist;

    // perpendicular neighbourhood size can be 1 or 3.
    // 0- size =1 
    // 1- size =3
    // format: 1u
    PARAM_UINT luma_neighbours_perpendicular_dist;

    // if percentage of neighbores are determind as flat (according to comparison with neighbours_binarization_minflatval) is greater than neighbours_agreement_sensitivity, center pixel will be indicated as flat for higher pass.
    // format: 8uQ7
    PARAM_UINT luma_neighbours_agreement_sensitivity;

    // in case center pixel is flatter than center_binarization_minflatval it will indicate the pixel as flat without any regard to neighbores. If not, the decision will be done according to neighbores decision.
    // format: 3u
    PARAM_UINT chroma_center_binarization_minflatval;

    // if set to 1, pixel can be dependant on its neighbores for indicating as flat for higher pass.
    // format: 1u
    PARAM_UINT chroma_neighbours_impact_enable;

    // neighbors are counted as flat if they are flatter than neighbours_binarization_minflatval.
    // format: 3u
    PARAM_UINT chroma_neighbours_binarization_minflatval;

    // perpendicular neighbourhood size can be 1 or 3.
    // 0- size =1 
    // 1- size =3
    // format: 1u
    PARAM_UINT chroma_neighbours_perpendicular_dist;

    // if percentage of neighbores are determind as flat (according to comparison with neighbours_binarization_minflatval) is greater than neighbours_agreement_sensitivity, center pixel will be indicated as flat for higher pass.
    // format: 8uQ7
    PARAM_UINT chroma_neighbours_agreement_sensitivity;

} ANR_INTER_LENGTH_OUTPUT_INDICATION;

typedef struct ANR_GREY_TREATMENT_TAG
{
    // increase threshold torwards the Thr_Modification_Target_*_DER values. If fully grey, thresholds will assume the target.
    // format: 1u
    PARAM_UINT enable_grey_treatment_thr_modification;

    // blend with isotropic filtering
    // format: 1u
    PARAM_UINT enable_grey_treatment_isotropic_filter_blend;

    // dcblend2 will blend as a function of max(greyDCSize,cDCSize) instead of just cDCSize in case that the chromaticity of the lower pass is lower than the chromaticity of the current pass for the current pixel.
    // format: 1u
    PARAM_UINT enable_grey_treatment_dcblend2_chroma_modification;

    // chromaticity is estimated using : 0 - 3x3 circle, 1- 5x5 circle
    // format: 1u
    PARAM_UINT detect_grey_condition_chromaticity_radius;

    // Define the conditions for identifying areas as 'grey' - chroma threshold p1 (100% weight), Linear in between p1,p2 (start,end)
    // format: 8u
    PARAM_UINT detect_grey_condition_chromaticity_thr_low;

    // Define the conditions for identifying areas as 'grey' - chroma threshold p2 (0% weight), Linear in between p1,p2 (start,end)
    // format: 8u
    PARAM_UINT detect_grey_condition_chromaticity_thr_high;

    // y max derivative is calculated using : 0- 3x3 , 1- 5x5.
    // format: 1u
    PARAM_UINT detect_grey_condition_y_max_derivative_radius;

    // Define the conditions for identifying areas as 'grey' - derivative threshold p1 (0% weight), Linear in between p1,p2 (start,end)
    // format: 8u
    PARAM_UINT detect_grey_condition_y_max_derivative_thr_low;

    // Define the conditions for identifying areas as 'grey' - derivative threshold p2 (100% weight), Linear in between p1,p2 (start,end)
    // format: 8u
    PARAM_UINT detect_grey_condition_y_max_derivative_thr_high;

    // pixels Neighborhood should have chromaticity lower than this threshold in order to be defined as grey for the calculation of the greydcsize.
    // format: 8u
    PARAM_UINT detect_greydcsize_neighbors_chromaticity_thr;

    // if pixel has a higher chromaticity than this threshold it will be defined a chromatc detail and will set greydcsize to zero.
    // format: 8u
    PARAM_UINT detect_greydcsize_center_detail_chromaticity_thr;

    // Increase the chroma filter thresholds prior to the CLPF filtering algorithm.  If target_y,u,v ==0  then it won't change the thresholds, actually if the target threshold < base threshold, then no change will be made.
    // format: (8+e)u
    PARAM_UINT thr_modification_target_y;

    // Increase the chroma filter thresholds prior to the CLPF filtering algorithm.  If target_y,u,v ==0  then it won't change the thresholds, actually if the target threshold < base threshold, then no change will be made.
    // format: 8u
    PARAM_UINT thr_modification_target_u;

    // Increase the chroma filter thresholds prior to the CLPF filtering algorithm.  If target_y,u,v ==0  then it won't change the thresholds, actually if the target threshold < base threshold, then no change will be made.
    // format: 8u
    PARAM_UINT thr_modification_target_v;

    // controls how much blend will be done with the isotropic chroma filtering upto 7x7, depends on GreyDCSize and Isotropic_Filter_Size.
    // blend (with isotropic filtering 7x7) factor fucntion scale as a function of grey edge detection.
    // Function values are 0:64 and as a function of grey edge detection which is also 0:64.
    // scale values are Q4,  7Q4, example 16 == X1 slope
    // to disable blend with isotropic filter : isotropic_filter_blend_factor_scale == isotropic_filter_blend_factor_offset == 0 
    // format: 6uQ4
    PARAM_UINT isotropic_filter_blend_factor_scale;

    // controls how much blend will be done with the isotropic chroma filtering upto 7x7, depends on GreyDCSize and Isotropic_Filter_Size.
    // blend (with isotropic filtering 7x7) factor fucntion scale as a function of grey edge detection.
    // Function values are 0:64 and as a function of grey edge detection which is also 0:64.
    // to disable blend with isotropic filter : isotropic_filter_blend_factor_scale == isotropic_filter_blend_factor_offset == 0 
    // format: 7s
    PARAM_INT isotropic_filter_blend_factor_offset;

    // 0- isotropic filter is 3x3
    // 1- isotropic filter is 5x5
    // 2- isotropic filter is adaptive (1x1/3x3/5x5/7x7) depends on GreyDCSize (0/1/2/3)
    // 3- isotropic filter is 7x7
    // format: 2u
    PARAM_UINT isotropic_filter_size;

} ANR_GREY_TREATMENT;

typedef struct ANR_CHROMA_FILTER_EXTENSION_TAG
{
    // enable current pass CLPF Edge filtering bypass
    // format: 1u
    PARAM_UINT enable_chroma_filter_extension_median;

    // The maximum chromaticity value for which details are disabled.
    // format: 8u
    PARAM_UINT median_detect_override_detail_condition_chromaticity_thr;

    // The minimum luma derivative value for which details are disabled
    // format: 8u
    PARAM_UINT median_detect_override_detail_condition_y_max_derivative_thr;

    // The local detail sensitivity controls the local conditions logic:  
    // High value - more local details are identified in the image ( = less filtering is performed) 
    // Low value - less local details are identified in the image ( = more filtering is performed)
    // this will multiply the chroma x modifers (scale and offset). Q4 example 16 == X1
    // 65 to disable median filter.
    // format: 7uQ5
    PARAM_UINT median_detect_corner_detail_sensitivity_y;

    // format: 7uQ4
    PARAM_UINT median_detect_corner_detail_sensitivity_uv;

    // Threshold for chroma detail detection, chromaticity difference is calculated from chromaticity 8bits. Higher chromaticity difference than this value is a detail.
    // format: 10u
    PARAM_UINT median_detect_triple_chromaticities_detail_thr_up_down;

    // Threshold for chroma detail detection, chromaticity difference is calculated from chromaticity 8bits. Higher chromaticity difference than this value could be a detail depends on UD Th comparison.
    // format: 10u
    PARAM_UINT median_detect_triple_chromaticities_detail_thr_external;

    // 1- if the pixel detected a detail than median won't work on the pixel in the problematic directions, or all directions for isotropic.
    // 0 - pixel self detail won't enforce median not working. Now it depends on the environment decision.
    // format: 1u
    PARAM_UINT median_detect_isotropic_self_decision_enforce_detail;

    // The environment detail sensitivity values determine the importance of the local indications environment in generating a detail flag. 
    // High value - few local detail indications are required to generate detail flag ( = more details, less filtering)
    //  Low value - many local detail indications are required to generate detail flag ( = less details, more filtering)
    // format: 8uQ7
    PARAM_UINT median_detect_isotropic_neighbors_detail_sensitivity;

    // scale on the sensativity to get sensativity for bigger environment  
    // format: 6uQ4
    PARAM_UINT median_detect_isotropic_neighbors_detail_sensitivity_far_scale;

    // 1- if the pixel detected a detail than median won't work on the pixel in the problematic directions, or all directions for isotropic.
    // 0 - pixel self detail won't enforce median not working. Now it depends on the environment decision.
    // format: 1u
    PARAM_UINT median_detect_directional_self_decision_enforce_detail;

    // The environment detail sensitivity values determine the importance of the local indications environment in generating a detail flag. 
    // High value - few local detail indications are required to generate detail flag ( = more details, less filtering)
    //  Low value - many local detail indications are required to generate detail flag ( = less details, more filtering)
    // format: 8uQ7
    PARAM_UINT median_detect_directional_neighbors_detail_sensitivity;

    // scale on the sensativity to get sensativity for bigger environment  
    // format: 6uQ4
    PARAM_UINT median_detect_directional_neighbors_detail_sensitivity_far_scale;

    // enable for the Chroma Bilateral filter block .
    // format: 1u
    PARAM_UINT enable_chroma_filter_extension_bilateral;

    // if both bilateral and median are on. The mux that chooses the final result from one of the two is controled by Bilateral_Decision_MinimalSize.
    // If ( chromaOutputDCindications_CDCMinSize >= Bilateral_Decision_MinimalSize) then take bilateral output, otherwise thake median output.
    // chromaOutputDCindications_CDCMinSize is in range 0:5 . 
    // 6 is bypass
    // if the pixel is flatter than this value then use the bilateral filter on it, otherwise just use median filter.
    // format: 3u
    PARAM_UINT bilateral_decision_minimalsize;

    // 0- bilateral size 1x1 (disabled)
    // 1- bilateral size 3x3
    // 2- bilateral size 5x5
    // format: 2u
    PARAM_UINT bilateral_filtersize;

    // extension of the flatness indication from 0:3 (1x1 : 7x7) to 0:5 (1x1 : 11x11)
    // format: 4u
    PARAM_UINT chroma_flat_indication_extension_threshold_9x9;

    // format: 4u
    PARAM_UINT chroma_flat_indication_extension_threshold_11x11;

    // format: 5u
    PARAM_UINT chroma_grey_indication_extension_threshold_9x9;

    // format: 6u
    PARAM_UINT chroma_grey_indication_extension_threshold_11x11;

} ANR_CHROMA_FILTER_EXTENSION;

typedef struct ANR_LUMA_SMOOTHING_TREATMENT_TAG
{
    // If set to 1,  Luma edge smoothing is enabled
    // format: 1u
    PARAM_UINT enable_luma_edge_smoothing;

    // min flat val in parallel to the edge in order to smooth it. Flat enough means a large edge and could be smoothed.
    // 5 - to bypasse.
    // format: 3u
    PARAM_UINT edge_smoothing_binarization_minflatval;

    // max edge val in perpendicular to the edge in order to smooth the edge. Edge small enough means a strong edge that could be smoothed.
    // format: 3u
    PARAM_UINT edge_smoothing_binarization_maxedgeval;

    // how many neighbores out of 5 should fulfill the parrallel minflatval condition and the perpendicular maxedgeval condition.
    // This affects also how many neighbores out of 3 should fulfill the conditions. 
    // format: 3u
    PARAM_UINT edge_smoothing_agreement_number;

    // If set to 1,  Luma transition smoothing is enabled
    // format: 1u
    PARAM_UINT enable_luma_transition_smoothing;

    // 1- 3x3 , 2-5x5
    // format: 2u
    PARAM_UINT transition_smoothing_filter_size;

    // binarization_minflatval, lower value means more areas will be defined as flat which in turn can give filtering in more areas. range [1,4].
    // format: 3u
    PARAM_UINT transition_isotropic_neighbours_binarization_minflatval;

    // binarization_minedgeval, lower value means more areas will be defined as edge which in turn can give filtering in more areas. range [0,3].
    // format: 2u
    PARAM_UINT transition_isotropic_neighbours_binarization_minedgeval;

    // binarization_maxedgeval, higher value means more areas will be defined as edge which in turn can give filtering in more areas. range [0,3].
    // format: 2u
    PARAM_UINT transition_isotropic_neighbours_binarization_maxedgeval;

    // agreement_sensitivity percentage (out of 128) will set the effective neighbours percetage needs to pass all condtions.
    // 86/128 value is 6/9 or 16/25 depends on transition_smoothing_filter_size.
    // 129 to disable feature
    // format: 8uQ7
    PARAM_UINT transition_isotropic_neighbours_agreement_sensitivity;

    // percentage (out of 128) of flats voters out of all voters, rest of needed voters should be edges. total number of needed voters is determind by transition_isotropic_neighbours_agreement_sensitivity.
    // format: 8uQ7
    PARAM_UINT transition_isotropic_neighbours_agreement_flat_vs_voters;

    // mininum flat value to define flat for 3x3 additional isotropic filtering feature.
    // format: 3u
    PARAM_UINT flat_isotropic_3x3_neighbours_binarization_minflatval;

    // agreement_sensitivity of the neighbours, size is 3x3 / 5x5 according to transition_smoothing_filter_size.
    // Sensativity 128 requires 100% of neighbours to be flat according to upper threshold. Sensativity 129 is disable feature.
    // format: 8uQ7
    PARAM_UINT flat_isotropic_3x3_neighbours_agreement_sensitivity;

    // agreement_sensitivity of the neighbours 5x5 according only if transition_smoothing_filter_size ==2, otherwise not operational..
    // Sensativity 128 requires 100% of neighbours to be flat according to upper threshold. Sensativity 129 is disable feature.
    // binarization_minflatval is always 4 for this condition.
    // format: 8uQ7
    PARAM_UINT flat_isotropic_5x5_neighbours_agreement_sensitivity;

    // 3x3 (including center) coefficients weight  = 2^kernel_3x3_shift
    // format: 2u
    PARAM_UINT smoothing_kernel_3x3_shift;

    // 5x5 horizontal, vertical and off-diagonals only coefficients weight  = 2^kernel_5x5_horver_shift.
    // 5x5 main diagonals are weight = 1.
    // format: 1u
    PARAM_UINT smoothing_kernel_5x5_horver_shift;

} ANR_LUMA_SMOOTHING_TREATMENT;

typedef struct ANR_CHROMA_SMOOTHING_TREATMENT_TAG
{
    // If set to 1,  Chroma edge smoothing is enabled
    // format: 1u
    PARAM_UINT enable_chroma_edge_smoothing;

    // min flat val in parallel to the edge in order to smooth it. Flat enough means a large edge and could be smoothed.
    // 6 - to bypasse.
    // format: 3u
    PARAM_UINT edge_smoothing_binarization_minflatval;

    // max edge val in perpendicular to the edge in order to smooth the edge. Edge small enough means a strong edge that could be smoothed.
    // format: 3u
    PARAM_UINT edge_smoothing_binarization_maxedgeval;

    // how many neighbores out of 3 should fulfill the parrallel minflatval condition and the perpendicular maxedgeval condition.
    // format: 2u
    PARAM_UINT edge_smoothing_agreement_number;

    // If set to 1,  chroma transition smoothing is enabled
    // format: 1u
    PARAM_UINT enable_chroma_transition_smoothing;

    // binarization_minflatval, lower value means more areas will be defined as flat which in turn can give filtering in more areas.
    // format: 3u
    PARAM_UINT transition_isotropic_neighbours_binarization_minflatval;

    // binarization_minedgeval, lower value means more areas will be defined as edge which in turn can give filtering in more areas.
    // format: 3u
    PARAM_UINT transition_isotropic_neighbours_binarization_minedgeval;

    // binarization_maxedgeval, higher value means more areas will be defined as edge which in turn can give filtering in more areas.
    // format: 3u
    PARAM_UINT transition_isotropic_neighbours_binarization_maxedgeval;

    // agreement_sensitivity percentage (out of 128) will set the effective neighbours percetage needs to pass all condtions.
    // 86/128 value is 6/9 or 16/25 depends on transition_smoothing_filter_size.
    // 129 to disable feature
    // format: 8uQ7
    PARAM_UINT transition_isotropic_neighbours_agreement_sensitivity;

    // percentage (out of 128) of flats voters out of all voters, rest of needed voters should be edges. total number of needed voters is determind by transition_isotropic_neighbours_agreement_sensitivity.
    // format: 8uQ7
    PARAM_UINT transition_isotropic_neighbours_agreement_flat_vs_voters;

    // mininum flat value to define flat for 3x3 additional isotropic filtering feature.
    // format: 3u
    PARAM_UINT flat_isotropic_3x3_neighbours_binarization_minflatval;

    // agreement_sensitivity of the neighbours, size is 3x3.
    // Sensativity 128 requires 100% of neighbours to be flat according to upper threshold.. Sensativity 129 is disable feature.
    // format: 8uQ7
    PARAM_UINT flat_isotropic_3x3_neighbours_agreement_sensitivity;

    // center coefficient weight  = 2^kernel_1x1_shift
    // format: 2u
    PARAM_UINT smoothing_kernel_1x1_shift;

    // 3x3 (not including center) coefficients weight  = 2^kernel_3x3_shift
    // format: 2u
    PARAM_UINT smoothing_kernel_3x3_shift;

} ANR_CHROMA_SMOOTHING_TREATMENT;

typedef struct ANR_LNR_TAG
{
    // format: 1u
    PARAM_UINT enable_lnr;

    // XC in the formula: 
    // R(x,y) = (A*(x-XC))^2 + (B*(y-YC))^2
    // logic value 0 is start of the image, 16384 is the end of the image.
    // format: 15uQ14
    PARAM_UINT elliptic_xc;

    // YC in the formula:
    // R(x,y) = (A*(x-XC))^2 + (B*(y-YC))^2
    // logic value 0 is start of the image, 16384 is the end of the image.
    // format: 15uQ14
    PARAM_UINT elliptic_yc;

    // A in the formula:
    // R(x,y) = (A*(x-XC))^2 + (B*(y-YC))^2
    // B will be detemind by B = (2^16 - A). That way we keep A,B normilized for a valid linear interpulation.
    // format: 16uQ16
    PARAM_UINT elliptic_a;

    // luma filter Y threshold modifier LUT.
    // Modifier threshold as a function of radius^2 and elipses
    // 17 entries, each in (0,4,7) bits. 
    // The actual Y threshold will be YbaseThr*luma_filter_lut_thr_y[elliptic radius]
    // format: 11uQ7
    PARAM_UINT luma_filter_lut_thr_y[17];

    // luma filter uv threshold modifier LUT.
    // Modifier threshold as a function of radius^2 and elipses
    // 17 entries, each in (0,4,7) bits. 
    // The actual uv threshold will be U/VbaseThr*luma_filter_lut_thr_uv[elliptic radius]
    // format: 11uQ7
    PARAM_UINT luma_filter_lut_thr_uv[17];

    // chroma filter Y threshold modifier LUT.
    // Modifier threshold as a function of radius^2 and elipses
    // 17 entries, each in (0,4,7) bits. 
    // The actual Y threshold will be YbaseThr*chroma_filter_lut_thr_y[elliptic radius]
    // format: 11uQ7
    PARAM_UINT chroma_filter_lut_thr_y[17];

    // chroma filter uv threshold modifier LUT.
    // Modifier threshold as a function of radius^2 and elipses
    // 17 entries, each in (0,4,7) bits. 
    // The actual uv threshold will be U/VbaseThr*chroma_filter_lut_thr_uv[elliptic radius]
    // format: 11uQ7
    PARAM_UINT chroma_filter_lut_thr_uv[17];

    // LNR Strength Modifier LUT. 
    // Blend factor as a function of radius, the blend factor will be used to blend between original dcblend2_lut (or flat weight)  and the dcblend2_lut * target_factor.
    // 17 Entries, values in 0:128 with 7 fraction bits. 128 = don't change ThMult. 0 = blend fully torwards  .
    // format: 8uQ7
    PARAM_UINT strength_modifier_radius_blend_lut[17];

    // LNR Luma blend factor (128 is factor 1)
    // format: 12uQ7
    PARAM_UINT luma_lnr_dcblend2_target_factor;

    // LNR Luma flat blend factor (128 is factor 1).
    // Factor on the weight of the flat kernel for lnr in its maximal target strenght (lut blend ==0).
    // format: 12uQ7
    PARAM_UINT luma_lnr_flat_kernel_weight_target_factor;

    // LNR CNR Chroma blend factor (128 is factor 1) (max factor).
    // This target factor also serves as the maximal scale factor that lnr and cnr can apply together.
    // format: 12uQ7
    PARAM_UINT chroma_lnr_cnr_dcblend2_target_factor;

    // automatic mode : if set to 1, take the luts from the Full pass configuration and compensate with influence factor. 
    // manual      mode : if set to 0, take the luts from the current pass configuration.
    // 
    // influence factor == 1 take the same lut,
    // influence factor == 0 take lut ofall 128,
    // in between interpulate 
    // format: 1u
    PARAM_UINT use_luts_from_full_pass_configuration;

    // if use_luts_from_full_pass_configuration == 1 then
    // luma_filter_luts = ( full_pass_luma_filter_luts * automatic_influence_luma_luts  + (128-automatic_influence_luma_luts ) * [128,128,...,128] ) >> 7
    // format: 8uQ7
    PARAM_UINT automatic_influence_luma_luts ;

    // if use_luts_from_full_pass_configuration == 1 then
    // chroma_filter_luts = ( full_pass_chroma_filter_luts * automatic_influence_chroma_luts  + (128-automatic_influence_chroma_luts ) * [128,128,...,128] ) >> 7
    // format: 8uQ7
    PARAM_UINT automatic_influence_chroma_luts;

    // if use_luts_from_full_pass_configuration == 1 then
    // strength_modifier_radius_blend_lut = ( full_pass_strength_modifier_radius_blend_lut * automatic_influence_chroma_luts  + (128-automatic_influence_chroma_luts ) * [128,128,...,128] ) >> 7
    // format: 8uQ7
    PARAM_UINT automatic_influence_modifier_radius_blend_lut;

} ANR_LNR;

typedef struct ANR_CNR_TAG
{
    // 0 - enable operation
    // 1 -  bypass unit
    // format: 1u
    PARAM_UINT enable_cnr;

    // 0 - select image from CYLPF
    // 1 - select DCNF image 
    // format: 1u
    PARAM_UINT input_select;

    // Defines the actual number of  treated colors
    // format: 3u
    PARAM_UINT number_of_colors;

    // Angle(U,V) (hue) low limit
    // format: 11uQ2
    PARAM_UINT detect_angle_start[5];

    // Angle(U,V) (hue) high limit
    // format: 11uQ2
    PARAM_UINT detect_angle_end[5];

    // 0 - chromaticity is calculated by (abs(U) + abs(V))/2
    // 1 - chromaticity is calculated by Radius = (U^2+V^2)^0.5
    // format: 1u
    PARAM_UINT detect_chromaticity_calc_mode[5];

    // Color chromaticity low limit.
    // 1024 is maximal chromaticity chosen according to calc mode.
    // format: 11u
    PARAM_UINT detect_chromaticity_start[5];

    // Color chromaticity high limit.
    // 1024 is maximal chromaticity chosen according to calc mode.
    // format: 11u
    PARAM_UINT detect_chromaticity_end[5];

    // Y component low limit
    // format: (8+e)u
    PARAM_UINT detect_luma_start[5];

    // Y component high limit
    // format: (8+e)u
    PARAM_UINT detect_luma_end[5];

    // 0 - Color#0 saturation is calculated by Saturation = Chromaticity/Y (HSY color space). Taken from detect_color0_skin_saturation_*
    // 1 - Color#0 saturation is calculated by Saturation = Chromaticity (YUV color space). Taken from detect_chromaticity_start[0], detect_chromaticity_end[0]
    // format: 1u
    PARAM_UINT detect_color0_saturation_mode;

    // Minumum saturation value for skin when Y=Y_min.
    // format: 11u
    PARAM_UINT detect_color0_skin_saturation_min_y_min;

    // Maximum saturation value for skin when Y=Y_min.
    // format: 11u
    PARAM_UINT detect_color0_skin_saturation_max_y_min;

    // Minumum saturation value for skin when Y=Y_max.
    // format: 11u
    PARAM_UINT detect_color0_skin_saturation_min_y_max;

    // Maximum saturation value for skin when Y=Y_max.
    // format: 11u
    PARAM_UINT detect_color0_skin_saturation_max_y_max;

    // 0 - disable external color regions treatment
    // 1 - enable color regions treatment to U<=0 && V>=0
    // format: 1u
    PARAM_UINT detect_color0_external_enable;

    // Weight value at Color Range Borders
    // format: 4uQ4
    PARAM_UINT boundary_weight[5];

    // smooth range transition ratio (with regards to color range) of each color region.
    // format: 8uQ8
    PARAM_UINT transition_ratio[5];

    // smooth range transition ratio (with regards to color 0 range) of each external color regions.
    // q_ext = transition_ratio_external / boundery_probability[0]
    // format: 8uQ8
    PARAM_UINT color0_transition_ratio_external;

    // Gain factor YLPF Y-Base Threshold
    // format: 11uQ7
    PARAM_UINT luma_filter_threshold_scale_y[5];

    // Gain factor for YLPF U/V-Base Thresholds 
    // format: 11uQ7
    PARAM_UINT luma_filter_threshold_scale_uv[5];

    // Offset for YLPF Y-Base Threshold 
    // format: (9+e)s
    PARAM_INT luma_filter_offset_y[5];

    // Offset for YLPF U-Base Threshold 
    // format: 9s
    PARAM_INT luma_filter_offset_u[5];

    // Offset for YLPF V-Base Threshold
    // format: 9s
    PARAM_INT luma_filter_offset_v[5];

    // Gain factor for CLPF Y-Base Threshold
    // format: 11uQ7
    PARAM_UINT chroma_filter_threshold_scale_y[5];

    // Gain factor for CLPF U/V- Base Threshold 
    // format: 11uQ7
    PARAM_UINT chroma_filter_threshold_scale_uv[5];

    // Offset for CLPF Y-Base Threshold
    // format: (9+e)s
    PARAM_INT chroma_filter_offset_y[5];

    // Offset for CLPF U-Base Threshold
    // format: 9s
    PARAM_INT chroma_filter_offset_u[5];

    // Offset for CLPF V-Base Threshold
    // format: 9s
    PARAM_INT chroma_filter_offset_v[5];

    // is dependent of Face Detection for this color.
    // Demand FD in addition to color detection.
    // format: 1u
    PARAM_UINT face_detection_dependency[5];

    // actual face radius will be face_detection_boundary * face_detected_radius
    // format: 14uQ10
    PARAM_UINT face_detection_boundary;

    // actual face transition radius (from this point and outer face weight == 0 ) will be face_detection_transition * face_detected_radius 
    // restriction : face_detection_boundary <=  face_detection_transition < = 2 * face_detection_boundary (will be forced otherwise)
    // format: 14uQ10
    PARAM_UINT face_detection_transition;

    // YLPF Blend factor for CNR gain level for the Far base Luma thresholds
    // 0-no CNR modification on far thresholds at all
    // 4-full CNR modification on far with same scale and offset as in close
    // in between we blend.
    // format: 3u
    PARAM_UINT luma_filter_base_far_modifier_y;

    // YLPF Blend factor for CNR gain level for the Far base Chroma thresholds
    // 0-no CNR modification on far thresholds at all
    // 4-full CNR modification on far with same scale and offset as in close
    // in between we blend.
    // format: 3u
    PARAM_UINT luma_filter_base_far_modifier_uv;

    // CLPF Blend factor for CNR gain level for the Far base Luma thresholds
    // 0-no CNR modification on far thresholds at all
    // 4-full CNR modification on far with same scale and offset as in close
    // in between we blend.
    // format: 3u
    PARAM_UINT chroma_filter_base_far_modifier_y;

    // CLPF Blend factor for CNR gain level for the Far base Chroma thresholds
    // 0-no CNR modification on far thresholds at all
    // 4-full CNR modification on far with same scale and offset as in close
    // in between we blend.
    // format: 3u
    PARAM_UINT chroma_filter_base_far_modifier_uv;

    // Scale Factor for user defined DCBlend2 Y_Dc blend factor. (2048 == X16)
    // format: 12uQ7
    PARAM_UINT luma_dcblend2_weight_scale[5];

    // Scale Factor for user defined DCBlend2 C_Dc blend factor. (2048 == X16)
    // this scale is limited by chroma_lnr_cnr_dcblend2_target_factor with the following: 
    // chroma_dcblend2_weight_scale (actual cnr scale influence of dcblend 2 blend factor ) = MIN( MAX(chroma_dcblend2_weight_restricted_scale , 2*128 - chroma_lnr_cnr_dcblend2_target_factor ) , chroma_lnr_cnr_dcblend2_target_factor )
    // format: 12uQ7
    PARAM_UINT chroma_dcblend2_weight_restricted_scale[5];

    // Scale Factor for user defined flat kernel blend factor. (2048 == X16)
    // format: 12uQ7
    PARAM_UINT luma_flat_kernel_blend_weight_scale[5];

} ANR_CNR;

typedef struct ANR_LUMA_FILTER_DETECTION_THRESHOLDS_TAG
{
    // CTb points location in the UV space, 127 is at external limits, 64 is half limit, ...  
    // ( HW implementation is : Limit value for UV-Average input for Luma base threshold caluclation )
    // format: 7u
    PARAM_UINT control_per_uv_limit;

    // luma_filter, _Y threshold LutY
    // format: (8+e)u
    PARAM_UINT y_threshold_per_y[17];

    // threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut interpulation are limited with control_per_uv_limit.
    // u_limited = min(u, control_per_uv_limit), v_limited = min(u, control_per_uv_limit)
    // Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) )
    // format: (6+e)s
    PARAM_INT y_threshold_per_uv[8];

    // luma_filter, _Y threshold Top Limit of (LutY+LutUV)
    // format: (8+e)u
    PARAM_UINT y_threshold_top_limit;

    // luma_filter, _Y threshold Bottom, Limit of (LutY+LutUV)
    // format: (8+e)u
    PARAM_UINT y_threshold_bottom_limit;

    // Luma filter, 3x3 luma derivative , 6 bits unsigned (0,2,4)
    // format: 6uQ4
    PARAM_UINT y_threshold_close3_mod_scale;

    // Luma filter, 3x3 luma derivative , 11 bits signed (1,10,0)
    // format: (9+e)s
    PARAM_INT y_threshold_close3_mod_offset;

    // Luma filter, 3x3 luma second order derivative scale, 6 bits unsigned (0,2,4)
    // format: 6uQ4
    PARAM_UINT y_threshold_der2_close3_mod_scale;

    // Luma filter, 3x3 luma second order derivative offset 11 bits signed (1,10,0).
    // format: (9+e)s
    PARAM_INT y_threshold_der2_close3_mod_offset;

    // luma_filter, 7x7/9x9 _Ythr modifier Far(7,9) scale/offset factors
    // format: 6uQ4
    PARAM_UINT y_threshold_far_mod_scale;

    // luma_filter, 7x7/9x9 _Ythr modifier  Far(7,9) scale/offset factors
    // format: (9+e)s
    PARAM_INT y_threshold_far_mod_offset;

    // luma_filter, 3x3/5x5 _Ythr modifier CloseX (X==External) scale/offset factors
    // format: 6uQ4
    PARAM_UINT y_threshold_close_external_mod_scale;

    // luma_filter, 3x3/5x5 _Ythr modifier CloseX (X==External) scale/offset factors
    // format: (9+e)s
    PARAM_INT y_threshold_close_external_mod_offset;

    // luma_filter, 7x7/9x9 _Ythr modifier FarX (X==External) scale/offset factors
    // format: 6uQ4
    PARAM_UINT y_threshold_far_external_mod_scale;

    // luma_filter, 7x7/9x9 _Ythr modifier FarX (X==External) scale/offset factors
    // format: (9+e)s
    PARAM_INT y_threshold_far_external_mod_offset;

    // luma_filter, _U threshold LutY
    // format: 8u
    PARAM_UINT u_threshold_per_y[17];

    // threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut interpulation are limited with control_per_uv_limit.
    // u_limited = min(u, control_per_uv_limit), v_limited = min(u, control_per_uv_limit)
    // Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) )
    // format: 6s
    PARAM_INT u_threshold_per_uv[8];

    // luma_filter, _U threshold Top Limit of (LutY+LutUV)
    // format: 8u
    PARAM_UINT u_threshold_top_limit;

    // luma_filter, _U threshold Bottom, Limit of (LutY+LutUV)
    // format: 8u
    PARAM_UINT u_threshold_bottom_limit;

    // luma_filter, 7x7/9x9(luma) _Uthr modifier Far(5) scale/offset factors
    // format: 6uQ4
    PARAM_UINT u_threshold_far_mod_scale;

    // luma_filter, 7x7/9x9(luma) _Uthr modifier Far(5) scale/offset factors
    // format: 9s
    PARAM_INT u_threshold_far_mod_offset;

    // luma_filter, 7x7/9x9 _Uthr modifier FarX (X==External) scale/offset factors
    // format: 6uQ4
    PARAM_UINT u_threshold_far_external_mod_scale;

    // luma_filter, 7x7/9x9 _Uthr modifier FarX (X==External) scale/offset factors
    // format: 9s
    PARAM_INT u_threshold_far_external_mod_offset;

    // luma_filter, _V threshold LutY
    // format: 8u
    PARAM_UINT v_threshold_per_y[17];

    // threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut interpulation are limited with control_per_uv_limit.
    // u_limited = min(u, control_per_uv_limit), v_limited = min(u, control_per_uv_limit)
    // Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) )
    // format: 6s
    PARAM_INT v_threshold_per_uv[8];

    // luma_filter, _V threshold Top Limit of (LutY+LutUV)
    // format: 8u
    PARAM_UINT v_threshold_top_limit;

    // luma_filter, _V threshold Bottom, Limit of (LutY+LutUV)
    // format: 8u
    PARAM_UINT v_threshold_bottom_limit;

    // luma_filter, 7x7/9x9(luma) _Vthr modifier Far(5) scale/offset factors
    // format: 6uQ4
    PARAM_UINT v_threshold_far_mod_scale;

    // luma_filter, 7x7/9x9(luma) _Vthr modifier Far(5) scale/offset factors
    // format: 9s
    PARAM_INT v_threshold_far_mod_offset;

    // luma_filter, 7x7/9x9 _Vthr modifier FarX (X==External) scale/offset factors
    // format: 6uQ4
    PARAM_UINT v_threshold_far_external_mod_scale;

    // luma_filter, 7x7/9x9 _Vthr modifier FarX (X==External) scale/offset factors
    // format: 9s
    PARAM_INT v_threshold_far_external_mod_offset;

} ANR_LUMA_FILTER_DETECTION_THRESHOLDS;

typedef struct ANR_CHROMA_FILTER_DETECTION_THRESHOLDS_TAG
{
    // CTb points location in the UV space, 127 is at external limits, 64 is half limit, ...  
    // ( HW implementation is : Limit value for UV-Average input for Luma base threshold caluclation )
    // format: 7u
    PARAM_UINT control_per_uv_limit;

    // chroma_filter, _Y threshold LutY
    // format: (8+e)u
    PARAM_UINT y_threshold_per_y[17];

    // threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut interpulation are limited with control_per_uv_limit.
    // u_limited = min(u, control_per_uv_limit), v_limited = min(u, control_per_uv_limit)
    // Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) )
    // format: (6+e)s
    PARAM_INT y_threshold_per_uv[8];

    // chroma_filter, _Y threshold Top Limit of (LutY+LutUV)
    // format: (8+e)u
    PARAM_UINT y_threshold_top_limit;

    // chroma_filter, _Y threshold Bottom, Limit of (LutY+LutUV)
    // format: (8+e)u
    PARAM_UINT y_threshold_bottom_limit;

    // chroma_filter, 5x5 _Ythr modifier Far(5 chroma blocks but 7x7/9x9 for y pixels) scale/offset factors
    // format: 6uQ4
    PARAM_UINT y_threshold_far_mod_scale;

    // chroma_filter, 5x5 _Ythr modifier Far(5 chroma blocks but 7x7/9x9 for y pixels) scale/offset factors
    // format: (9+e)s
    PARAM_INT y_threshold_far_mod_offset;

    // chroma_filter, _U threshold LutY
    // format: 8u
    PARAM_UINT u_threshold_per_y[17];

    // threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut interpulation are limited with control_per_uv_limit.
    // u_limited = min(u, control_per_uv_limit), v_limited = min(u, control_per_uv_limit)
    // Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) )
    // format: 6s
    PARAM_INT u_threshold_per_uv[8];

    // chroma_filter, _U threshold Top Limit of (LutY+LutUV)
    // format: 8u
    PARAM_UINT u_threshold_top_limit;

    // chroma_filter, _U threshold Bottom, Limit of (LutY+LutUV)
    // format: 8u
    PARAM_UINT u_threshold_bottom_limit;

    // chroma_filter, 5x5 _Uthr modifier Far(5) scale/offset factors
    // format: 6uQ4
    PARAM_UINT u_threshold_far_mod_scale;

    // chroma_filter, 5x5 _Uthr modifier Far(5) scale/offset factors
    // format: 9s
    PARAM_INT u_threshold_far_mod_offset;

    // chroma_filter,_Uthr modifier Dist (7) scale/offset factors
    // format: 6uQ4
    PARAM_UINT u_threshold_distant_mod_scale;

    // chroma_filter,_Uthr modifier Dist (7) scale/offset factors
    // format: 9s
    PARAM_INT u_threshold_distant_mod_offset;

    // chroma_filter, _V threshold LutY
    // format: 8u
    PARAM_UINT v_threshold_per_y[17];

    // threshold LutUV, points are located at the end of uv space (-128,127). Input uv values to the lut interpulation are limited with control_per_uv_limit.
    // u_limited = min(u, control_per_uv_limit), v_limited = min(u, control_per_uv_limit)
    // Output value =interp2( y_threshold_per_uv, (u_limited,v_limited) )
    // format: 6s
    PARAM_INT v_threshold_per_uv[8];

    // chroma_filter, _V threshold Top Limit of (LutY+LutUV)
    // format: 8u
    PARAM_UINT v_threshold_top_limit;

    // chroma_filter, _V threshold Bottom, Limit of (LutY+LutUV)
    // format: 8u
    PARAM_UINT v_threshold_bottom_limit;

    // chroma_filter, 5x5 _Vthr modifier Far(5) scale/offset factors
    // format: 6uQ4
    PARAM_UINT v_threshold_far_mod_scale;

    // chroma_filter, 5x5 _Vthr modifier Far(5) scale/offset factors
    // format: 9s
    PARAM_INT v_threshold_far_mod_offset;

    // chroma_filter, 7x7 _Vthr modifier Dist (7) scale/offset factors
    // format: 6uQ4
    PARAM_UINT v_threshold_distant_mod_scale;

    // chroma_filter, 7x7 _Vthr modifier Dist (7) scale/offset factors
    // format: 9s
    PARAM_INT v_threshold_distant_mod_offset;

} ANR_CHROMA_FILTER_DETECTION_THRESHOLDS;

typedef struct ANR_DCBLEND1_TAG
{
    // enable DCblend1 between lower pass and current one (lower pass has to be processed)
    // format: 1u
    PARAM_UINT enable_dcblend1_chroma;

} ANR_DCBLEND1;

typedef struct ANR_DCBLEND2_TAG
{
    // enable DCblend2 between lower pass and current one (lower pass has to be processed)
    // format: 1u
    PARAM_UINT enable_dcblend2_luma;

    // enable DCblend2 between lower pass and current one (lower pass has to be processed)
    // format: 1u
    PARAM_UINT enable_dcblend2_chroma;

    // Controls the strength of DCBlend2. Lut of how much to blend as a function YDCSize.
    // Higher values reduce luminance noise significantly, at the expense of texture, granularity and sharpness.
    // Lower values maintain the luminance noise levels, while preserving texture, granularity and sharpness.
    // format: 8uQ7
    PARAM_UINT dcblend2_luma_strength_function[5];

    // Controls the strength of DCBlend2. Lut of how much to blend as a function CDCSize. (in case of filter2 ind extenstion interp is off, these values will be interpulated to [0,3] instead of [0,5])
    // Higher values reduce chromatic noise significantly, at the expense of texture, granularity and sharpness.
    // Lower values maintain the chromatic noise levels, while preserving texture, granularity and sharpness.
    // format: 8uQ7
    PARAM_UINT dcblend2_chroma_strength_function[6];

} ANR_DCBLEND2;

typedef struct ANR_DITHERING_TAG
{
    // Dithering enable for luma image
    // format: 1u
    PARAM_UINT dithering_y_en;

    // Dithering enable for chroma image
    // format: 1u
    PARAM_UINT dithering_c_en;

} ANR_DITHERING;

typedef struct ANR_Chromatix_TAG
{
    ANR_TOP top;
    ANR_POWER_CONTROL power_control;
    ANR_LUMA_CHROMA_FILTER_CONFIG luma_chroma_filter_config;
    ANR_LUMA_FILTER_CONFIG luma_filter_config;
    ANR_CHROMA_FILTER_CONFIG chroma_filter_config;
    ANR_LUMA_FILTER_KERNEL luma_filter_kernel;
    ANR_CHROMA_FILTER_KERNEL chroma_filter_kernel;
    ANR_LUMA_PEAK_MANAGEMENT luma_peak_management;
    ANR_CHROMA_PEAK_MANAGEMENT chroma_peak_management;
    ANR_INTER_LENGTH_THR_MODIFICATION inter_length_thr_modification;
    ANR_INTER_LENGTH_OUTPUT_INDICATION inter_length_output_indication;
    ANR_GREY_TREATMENT grey_treatment;
    ANR_CHROMA_FILTER_EXTENSION chroma_filter_extension;
    ANR_LUMA_SMOOTHING_TREATMENT luma_smoothing_treatment;
    ANR_CHROMA_SMOOTHING_TREATMENT chroma_smoothing_treatment;
    ANR_LNR lnr;
    ANR_CNR cnr;
    ANR_LUMA_FILTER_DETECTION_THRESHOLDS luma_filter_detection_thresholds;
    ANR_CHROMA_FILTER_DETECTION_THRESHOLDS chroma_filter_detection_thresholds;
    ANR_DCBLEND1 dcblend1;
    ANR_DCBLEND2 dcblend2;
    ANR_DITHERING dithering;
} ANR_Chromatix;

// ############ Functions ############
int32_t Validate_ANR_Chromatix( ANR_Chromatix* regStruct, uint32_t mE  );
void SetDefaultVal_ANR_Chromatix( ANR_Chromatix* regStruct, uint32_t mE, PASS_TYPE passType );
// ###################################
#ifdef __cplusplus
}
#endif


#endif //__ANR_CHROMATIX_H__

