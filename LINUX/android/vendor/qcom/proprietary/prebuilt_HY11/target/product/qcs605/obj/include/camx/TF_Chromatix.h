// NOWHINE ENTIRE FILE 
//-------------------------------------------------------------------------
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------


#ifndef __TF_CHROMATIX_H__
#define __TF_CHROMATIX_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "CommonDefs.h"

typedef struct TF_Chromatix_TAG
{

    // Whether to use indications from lower resolutions (should be 0 for lowest resolution pass where there is no indications input)
    // format: 1u
    PARAM_UINT use_indications;

    // Whether ghost detection takes Luma into account.
    // When both disable_luma_ghost_detection and disable_chroma_ghost_detection are '1', ghost detection is bypassed
    // (setting filter strength either to indication values or to zeros - depending on indications_affect_fs_decision_also_directly flag). 
    // format: 1u
    PARAM_UINT disable_luma_ghost_detection;

    // Whether ghost detection takes Chroma into account.
    // When both disable_luma_ghost_detection and disable_chroma_ghost_detection are '1', ghost detection is bypassed
    // (setting filter strength either to indication values or to zeros - depending on indications_affect_fs_decision_also_directly flag). 
    // format: 1u
    PARAM_UINT disable_chroma_ghost_detection;

    // Determines whether to perform anti-aliasing (low pass) filter on Luma of the difference image prior to using it in ghost detection.
    // When this flag is '1',  blending sub-module will be forced to bypass one of inputs to TFO
    // (thus, anti-aliasing should be enabled only in low resolutions).
    // format: 1u
    PARAM_UINT smear_inputs_for_decisions;

    // Whether to use spatially filtered Luma for ghost detection decisions instead of unfiltered Luma
    // format: 1u
    PARAM_UINT use_anr_for_decisions_y;

    // Whether to use spatially filtered Chroma for ghost detection decisions instead of unfiltered Chroma
    // format: 1u
    PARAM_UINT use_anr_for_decisions_c;

    // Whether to enable radial update of noise thresholds (due to lens roll-off)
    // format: 1u
    PARAM_UINT enable_lnr;

    // Enables dependence of noise thresholds on luma
    // (0 for power-saving mode: all noise thresholds will depend only on chroma; if enable_noise_est_by_chroma is also 0 - noise thresholds will be constant).
    // CONSTRAINT: When disable_luma_ghost_detection is '1', enable_noise_est_by_luma should be '0'
    // format: 1u
    PARAM_UINT enable_noise_est_by_luma;

    // Enables dependence of noise thresholds on chroma
    // (0 for power-saving mode: noise thresholds will depend only on luma; if enable_noise_est_by_luma is also 0 - noise thresholds will be constant).
    // CONSTRAINT: When disable_chroma_ghost_detection is '1', enable_noise_est_by_chroma should be '0'
    // format: 1u
    PARAM_UINT enable_noise_est_by_chroma;

    // Whether the padding in Luma anti-aliasing and SAD filters, is by reflection.
    // When this flag is '0' (this is the default value) the padding is done by duplication of the boundary pixel.
    // When this flag is '1' the padding is done by taking the value at a coordinate mirrored relative to the boundary.
    // format: 1u
    PARAM_UINT padding_by_reflection;

    // Window size for SAD Y calculation. 0 = 7x5 window, 1 = 5x5 window, 2 = 3x3 window, 3 = 1x1 window
    // format: 2u
    PARAM_UINT sad_y_calc_mode;

    // Window size for SAD Cb and Cr calculations. 0 = 3x3 window, 1 = 5x3 window
    // format: 1u
    PARAM_UINT sad_c_calc_mode;

    // Whether TF is working in DCI mode - in this mode fs_to_a4_maps are not used
    // format: 1u
    PARAM_UINT is_dci_mode;

    // When this flag is '1', indications affect FS decision also directly, by taking minimum of normal FS decision (i.e. at current resolution)
    // and the indications value converted to 6 bits (i.e. from lower resolution; for "out of frame" pixels the indication value is clipped to out_of_frame_pixels_confidence before conversion).
    // Evidently, in this mode, if ghost detection is disabled (by setting both disable_luma_ghost_detection and disable_chroma_ghost_detection to '1'),
    // normal FS decisions are ignored and only indications affect the FS decision.
    // CONSTRAINT: Can be '1' only when use_indications is '1'
    // format: 1u
    PARAM_UINT indications_affect_fs_decision_also_directly;

    // Whether to apply peak removal on the "FS image" (i.e. on the result of FS decision)
    // format: 1u
    PARAM_UINT apply_fs_rank_filter;

    // Whether to apply low pass filter on the "FS image" (i.e. on the result of FS decision)
    // format: 1u
    PARAM_UINT apply_fs_lpf;

    // Forces same blending factor both for low and for high frequencies during blending.
    // format: 1u
    PARAM_UINT is_same_blending_for_all_frequencies;

    // When indications_affect_fs_decision_also_directly is '1', this value is used to update the indications in "out of frame" regions (i.e. pixels going out of frame during global motion compensation) - as upper limit 
    // format: 4u
    PARAM_UINT out_of_frame_pixels_confidence;

    // Multiplicative factor for decreasing the indication values (valid factor range is [0-16]). Value of 16 means no change to indications. 
    // format: 5u
    PARAM_UINT indications_premultiply_factor;

    // Whether to take "1-a" instead of "a" for a1 and a4 blending weights.
    // format: 1u
    PARAM_UINT invert_temporal_blending_weights;

    // 0 = regular blending
    // 1 = copy IMG to TFO
    // 2 = copy ANRO to TFO
    // 3 = copy MCTFP to TFO
    // 4 = use blending with constant factor between ANRO and MCTFP (this mode can not be used only when both inputs are available to TF)
    // format: 3u
    PARAM_UINT blending_mode;

    // Whether to apply dithering to Y channel of final TF output
    // format: 1u
    PARAM_UINT dither_y;

    // Whether to apply dithering to Cb channel of final TF output
    // format: 1u
    PARAM_UINT dither_cb;

    // Whether to apply dithering to Cr channel of final TF output
    // format: 1u
    PARAM_UINT dither_cr;

    // Enable morphological erode filter of indications (located before morphological dilate filter)
    // format: 1u
    PARAM_UINT morph_erode_en;

    // 0 = functional bypass (no filter), 1 = 5x5 window, 2 = 3x3 window
    // format: 2u
    PARAM_UINT morph_erode_size;

    // Enable morphological dilate filter of indications (located after morphological erode filter)
    // format: 1u
    PARAM_UINT morph_dilate_en;

    // 0 = functional bypass (no filter), 1 = 5x5 window, 2 = 3x3 window
    // format: 2u
    PARAM_UINT morph_dilate_size;

    // Whether to enable warping transform refinement
    // format: 1u
    PARAM_UINT tr_enable;

    // Minimal FS to contribute to transform refinement
    // format: 6u
    PARAM_UINT tr_fs_threshold;

    // Dead-zone for transform refinement results. 0 is equivalent to no dead-zone.
    // format: 2u
    PARAM_UINT tr_dead_zone;

    // refinement grid: number of rows (1...31)
    // format: 5u
    PARAM_UINT tr_block_num_y;

    // Contribution of luma to Y noise threshold, sampled at 17 points
    // format: 12uQ2
    PARAM_UINT noise_params_y_ytb[17];

    // Contribution of luma to Cb noise threshold, sampled at 17 points
    // format: 10u
    PARAM_UINT noise_params_cb_ytb[17];

    // Contribution of luma to Cr noise threshold, sampled at 17 points
    // format: 10u
    PARAM_UINT noise_params_cr_ytb[17];

    // Contribution of chroma to Y noise threshold, specified for 8 extreme points
    // format: 11s
    PARAM_INT noise_params_y_ctb[8];

    // Contribution of chroma to Cb noise threshold, specified for 8 extreme points
    // format: 11s
    PARAM_INT noise_params_cb_ctb[8];

    // Contribution of chroma to Cr noise threshold, specified for 8 extreme points
    // format: 11s
    PARAM_INT noise_params_cr_ctb[8];

    // Limit for chroma values that affect Luma noise threshold
    // format: 9u
    PARAM_UINT noise_params_y_uv_limit;

    // Limit for chroma values that affect Cb noise threshold
    // format: 9u
    PARAM_UINT noise_params_cb_uv_limit;

    // Limit for chroma values that affect Cr noise threshold
    // format: 9u
    PARAM_UINT noise_params_cr_uv_limit;

    // Upper limit of Luma noise threshold
    // format: 12uQ2
    PARAM_UINT noise_params_y_top_lim;

    // Upper limit of Cb noise threshold
    // format: 10u
    PARAM_UINT noise_params_cb_top_lim;

    // Upper limit of Cr noise threshold
    // format: 10u
    PARAM_UINT noise_params_cr_top_lim;

    // Lower limit of luma noise threshold
    // format: 12uQ2
    PARAM_UINT noise_params_y_bot_lim;

    // Lower limit of Cb noise threshold
    // format: 10u
    PARAM_UINT noise_params_cb_bot_lim;

    // Lower limit of Cr noise threshold
    // format: 10u
    PARAM_UINT noise_params_cr_bot_lim;

    // Multiplication factor used in Luma thresholds calculation to form weak threshold (multiplies basic noise threshold)
    // format: 5uQ4
    PARAM_UINT fs_decision_params_y_c1[9];

    // Multiplication factor used in Luma thresholds calculation to form strong threshold (multiplies basic noise threshold)
    // format: 6uQ4
    PARAM_UINT fs_decision_params_y_c3[9];

    // Multiplication factor used in Luma thresholds calculation to form weak threshold (multiplies basic noise threshold). Used in case of out-of-frame Luma pixel.
    // format: 5uQ4
    PARAM_UINT fs_decision_params_oof_y_c1;

    // Multiplication factor used in Luma thresholds calculation to form strong threshold (multiplies basic noise threshold). Used in case of out-of-frame Luma pixel.
    // format: 6uQ4
    PARAM_UINT fs_decision_params_oof_y_c3;

    // Multiplication factor used in Chroma thresholds calculation to form weak threshold (multiplies basic noise threshold) 
    // format: 5uQ4
    PARAM_UINT fs_decision_params_c_c1[9];

    // Multiplication factor used in Chroma thresholds calculation to form strong threshold (multiplies basic noise threshold)
    // format: 6uQ4
    PARAM_UINT fs_decision_params_c_c3[9];

    // Multiplication factor used in Chroma thresholds calculation to form weak threshold (multiplies basic noise threshold). Used in case of out-of-frame Luma pixel. 
    // format: 5uQ4
    PARAM_UINT fs_decision_params_oof_c_c1;

    // Multiplication factor used in Chroma thresholds calculation to form strong threshold (multiplies basic noise threshold). Used in case of out-of-frame Luma pixel.
    // format: 6uQ4
    PARAM_UINT fs_decision_params_oof_c_c3;

    // Multiplication factor used for determining weak Luma threshold for a3 (based on weak decision threshold for FS)
    // format: 5uQ4
    PARAM_UINT a3_t1_scale_y;

    // Multiplication factor used for determining weak Chroma threshold for a3 (based on weak decision threshold for FS)
    // format: 5uQ4
    PARAM_UINT a3_t1_scale_c;

    // Multiplication factor used for determining strong Luma threshold for a3 (based on strong decision threshold for FS)
    // format: 6uQ4
    PARAM_UINT a3_t2_scale_y;

    // Multiplication factor used for determining strong Chroma threshold for a3 (based on strong decision threshold for FS)
    // format: 6uQ4
    PARAM_UINT a3_t2_scale_c;

    // Offset used for determining weak Luma threshold for a3 (based on weak decision threshold for FS)
    // format: 10uQ5
    PARAM_UINT a3_t1_offs_y;

    // Offset used for determining weak Chroma threshold for a3 (based on weak decision threshold for FS)
    // format: 10uQ5
    PARAM_UINT a3_t1_offs_c;

    // Offset used for determining strong Luma threshold for a3 (based on strong decision threshold for FS)
    // format: 12uQ5
    PARAM_UINT a3_t2_offs_y;

    // Offset used for determining strong Chroma threshold for a3 (based on strong decision threshold for FS)
    // format: 12uQ5
    PARAM_UINT a3_t2_offs_c;

    // Lower limit for a2 blending factor calculation in Luma (this value may never be reached if a2_slope_y is too small)
    // format: 8uQ8
    PARAM_UINT a2_min_y;

    // Upper limit for a2 blending factor calculation (used in case FS=0) in Luma
    // format: 8uQ8
    PARAM_UINT a2_max_y;

    // Lower limit for a2 blending factor calculation in Chroma (this value may never be reached if a2_slope_c is too small)
    // format: 8uQ8
    PARAM_UINT a2_min_c;

    // Upper limit for a2 blending factor calculation (used in case FS=0) in Chroma
    // format: 8uQ8
    PARAM_UINT a2_max_c;

    // a2 slope as a function of FS (a2 starts from a2_max_y at FS=0 and goes down with slope a2_slope_y as FS increases). Used for Luma
    // format: 8uQ5
    PARAM_UINT a2_slope_y;

    // a2 slope as a function of FS (a2 starts from a2_max_c at FS=0 and goes down with slope a2_slope_c as FS increases). Used for Chroma
    // format: 8uQ5
    PARAM_UINT a2_slope_c;

    // Sparse mapping between FS values [0:8:64] and a1 blending factor for Luma
    // format: 8uQ8
    PARAM_UINT fs_to_a1_map_y[9];

    // Sparse mapping between FS values [0:8:64] and a1 blending factor for Chroma
    // format: 8uQ8
    PARAM_UINT fs_to_a1_map_c[9];

    // Sparse mapping between FS values [0:8:64] and a4 blending factor for Luma
    // format: 8uQ8
    PARAM_UINT fs_to_a4_map_y[9];

    // Sparse mapping between FS values [0:8:64] and a4 blending factor for Chroma
    // format: 8uQ8
    PARAM_UINT fs_to_a4_map_c[9];

    // Blending factor for Luma channel in case of constant blending mode
    // format: 8u
    PARAM_UINT constant_blending_factor_y;

    // Blending factor for Chroma channels in case of constant blending mode
    // format: 8u
    PARAM_UINT constant_blending_factor_c;

    // Enable of TF module (including upscaler) per pass. When '0', ANR image is forwarded to TF output
    // format: 1u
    PARAM_UINT en;

    // Master enable of TF module. When this variable is 0, all passes are disabled irrespective of 'en' parameters. When this variable is 1, the enable of each pass is controlled by its 'en' parameter.
    // format: 1u
    PARAM_UINT master_en;

    // Optical center coordinate (Q15) in virtual coordinate system of [0,1]x[0,1]. 
    // format: 15uQ14
    PARAM_UINT lnr_opt_center_x;

    // Optical center coordinate (Q15) in virtual coordinate system of [0,1]x[0,1]. 
    // format: 15uQ14
    PARAM_UINT lnr_opt_center_y;

    // Defines the minimal bounding rectangle of LNR geometry (in virtual coordinate system of [0,1]x[0,1]). LNR gains are defined on uniformly sampled ellipses around the optical center, with lnr_ellipse_bounding_rect being the minimal bounding rectangle of the biggest ellipse. CONSTRAINT: the whole [0,1]x[0,1] image should be fully contained in the rectangle twice bigger than lnr_ellipse_bounding_rect.
    // format: 16uQ14
    PARAM_UINT lnr_ellipses_bounding_rect_w;

    // Defines the minimal bounding rectangle of LNR geometry (in virtual coordinate system of [0,1]x[0,1]). LNR gains are defined on uniformly sampled ellipses around the optical center, with lnr_ellipse_bounding_rect being the minimal bounding rectangle of the biggest ellipse. CONSTRAINT: the whole [0,1]x[0,1] image should be fully contained in the rectangle twice bigger than lnr_ellipse_bounding_rect.
    // format: 16uQ14
    PARAM_UINT lnr_ellipses_bounding_rect_h;

    // The LUT of scale factors to be applied to Luma noise thresholds (this LUT is accessed by normalized distance to optical center)
    // format: 9uQ7
    PARAM_UINT lnr_lut_y[16];

    // The LUT of scale factors to be applied to Chroma noise thresholds (this LUT is accessed by normalized distance to optical center)
    // format: 9uQ7
    PARAM_UINT lnr_lut_c[16];

    // Minimal percentage of good samples in a block to make its refinement valid
    // format: 7u
    PARAM_UINT tr_count_percentage_threshold;

    // Offset used in Luma thresholds calculation to form weak threshold
    // format: 13uQ4
    PARAM_UINT fs_decision_params_y_c2[9];

    // Offset used in Luma thresholds calculation to form weak threshold. Used in case of out-of-frame Luma pixel.
    // format: 13uQ4
    PARAM_UINT fs_decision_params_oof_y_c2;

    // Offset used in Luma thresholds calculation to form strong threshold 
    // format: 14uQ4
    PARAM_UINT fs_decision_params_y_c4[9];

    // Offset used in Luma thresholds calculation to form strong threshold. Used in case of out-of-frame Luma pixel. 
    // format: 14uQ4
    PARAM_UINT fs_decision_params_oof_y_c4;

    // Offset used in Chroma thresholds calculation to form weak threshold
    // format: 13uQ4
    PARAM_UINT fs_decision_params_c_c2[9];

    // Offset used in Chroma thresholds calculation to form weak threshold. Used in case of out-of-frame Luma pixel.
    // format: 13uQ4
    PARAM_UINT fs_decision_params_oof_c_c2;

    // Offset used in Chroma thresholds calculation to form strong threshold
    // format: 14uQ4
    PARAM_UINT fs_decision_params_c_c4[9];

    // Offset used in Chroma thresholds calculation to form strong threshold. Used in case of out-of-frame Luma pixel.
    // format: 14uQ4
    PARAM_UINT fs_decision_params_oof_c_c4;

    // refinement grid: number of columns (1...10)
    // format: 4u
    PARAM_UINT tr_block_num_x;

    // 0 = Flow (i.e. SW) decisions may override chromatix value of padding_by_reflection, 1 = Chromatix value of padding_by_reflection is used irrespective of flow decisions
    // format: 1u
    PARAM_UINT padding_by_reflection_override;

    // In video flow, in first frame: the spatial NR is configured somewhere between a2_max and a2_min. This parameter defines the linearly increasing percentage: 0 => a2_min, 100 => a2_max
    // format: 7u
    PARAM_UINT video_first_frame_spatial_nr_percentage;

    // Affects the algorithm of faster convergence in case of scene cut.
    // When detectable scene-cut (e.g. exposure change) happens, TF will usually significantly decrease temporal filtering.
    // In such case temporal weights should be kept small for some time to allow faster temporal noise convergence.
    // This parameter defines the allowed period in which the temporal weights should return to normal weights.
    // When 0 - scene cut recovery is disabled.
    // When 128 - default algorithm behavior is used.
    // When 255 -  slowest recovery (according to the algorithm) will be used.
    // format: 8u
    PARAM_UINT scene_cut_recovery_time;

    // Whether to use TF unit as a strength blender for ANR.
    // When '1' or '2' the TF is used as ANR post-processing block and can not be used for temporal noise reduction. Usually used in still processing or MFNR Post.
    // 0 - use regular mode for Temporal Filtering.
    // 1 - use as strength blender for ANR according to the anr_final_blender parameters. (Basic mode)
    // 2 - use as strength blender for ANR according to regular TF parameters and not based on anr_final_blender parameters. (Advanced mode) (Example: Use this mode if one wants to blend as a function of brighness or as a function of LNR)
    // format: 2u
    PARAM_UINT is_anr_strength_blender_mode;

    // Controls adaptive blending of ANR output with ANR input for High Frequencies of Luma (see description of anr_final_blender_strength_decision_ythr_low parameter for details).
    // Effective strength = 0 means taking only ANR input.
    // Effective strength = 256 means taking only ANR output.
    // format: 9uQ8
    PARAM_UINT anr_final_blender_luma_min_strength_high_freq;

    // Controls adaptive blending of ANR output with ANR input for Low Frequencies of Luma (see description of anr_final_blender_strength_decision_ythr_low parameter for details).
    // Effective strength = 0 means taking only ANR input.
    // Effective strength = 256 means taking only ANR output.
    // format: 9uQ8
    PARAM_UINT anr_final_blender_luma_min_strength_low_freq;

    // Controls adaptive blending of ANR output with ANR input for High Frequencies of Chroma (see description of anr_final_blender_strength_decision_ythr_low parameter for details).
    // Effective strength = 0 means taking only ANR input.
    // Effective strength = 256 means taking only ANR output.
    // format: 9uQ8
    PARAM_UINT anr_final_blender_chroma_min_strength_high_freq;

    // Controls adaptive blending of ANR output with ANR input for Low Frequencies of Chroma (see description of anr_final_blender_strength_decision_ythr_low parameter for details).
    // Effective strength = 0 means taking only ANR input.
    // Effective strength = 256 means taking only ANR output.
    // format: 9uQ8
    PARAM_UINT anr_final_blender_chroma_min_strength_low_freq;

    // Strength decision is based on dissimilarity decision between ANR input and ANR output.
    // Similar pixels will get strength=0, dissimilar pixels will get strength = 256; in-between, linear interpolation is used.
    // Similarity is calculated as min(Similarity_Y, Similarity_Cb, Similarity_Cr).
    // If ABS(ANR_Y_IN-ANR_Y_OUT) < anr_final_blender_strength_decision_ythr_low, then Similarity_Y = max;
    // if ABS(ANR_Y_IN-ANR_Y_OUT) >= anr_final_blender_strength_decision_ythr_high, then Similarity_Y = min;
    // otherwise, linear interpolation is used.
    // And similarly for Cb and Cr (using cthr values): e.g.
    // if ABS(ANR_Cb_IN-ANR_Cb_OUT) < anr_final_blender_strength_decision_cthr_low, then Similarity_Cb = max;
    // if ABS(ANR_Cb_IN-ANR_Cb_OUT) >= anr_final_blender_strength_decision_cthr_high, then Similarity_Cb = min;
    // otherwise, linear interpolation is used
    // This feature is designed to prevent peak noises back to the final image
    // format: 9u
    PARAM_UINT anr_final_blender_strength_decision_ythr_low;

    // see description of anr_final_blender_strength_decision_ythr_low parameter
    // format: 10u
    PARAM_UINT anr_final_blender_strength_decision_ythr_high;

    // see description of anr_final_blender_strength_decision_ythr_low parameter
    // format: 9u
    PARAM_UINT anr_final_blender_strength_decision_cthr_low;

    // see description of anr_final_blender_strength_decision_ythr_low parameter
    // format: 10u
    PARAM_UINT anr_final_blender_strength_decision_cthr_high;

} TF_Chromatix;

// ############ Functions ############
int32_t Validate_TF_Chromatix( TF_Chromatix* regStruct  );
void SetDefaultVal_TF_Chromatix( TF_Chromatix* regStruct, PASS_TYPE passType );
// ###################################
#ifdef __cplusplus
}
#endif


#endif //__TF_CHROMATIX_H__

