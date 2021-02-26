// NOWHINE ENTIRE FILE 
//-------------------------------------------------------------------------
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------


#ifndef __ICA_CHROMATIX_H__
#define __ICA_CHROMATIX_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "CommonDefs.h"

typedef struct ICA_TOP_TAG
{
    // 0: 4x4 kernel
    // 1: Bi-linear (2x2)
    // format: 1u
    PARAM_UINT y_interpolation_type;

} ICA_TOP;

typedef struct ICA_CTC_TAG
{
    // 0: Disabled, 1: Enabled  
    // format: 1u
    PARAM_UINT ctc_transform_grid_enable;

    // Grid value for x
    // format: 17sQ3
    PARAM_INT ctc_grid_x[945];

    // Grid value for y
    // format: 17sQ3
    PARAM_INT ctc_grid_y[945];

} ICA_CTC;

typedef struct ICA_OPG_TAG
{
    // 0: Out-of-frame pixel is populated with a predefined value.
    // 1: Out-of-frame pixel is populated using duplication.
    // format: 1u
    PARAM_UINT opg_invalid_output_treatment_calculate;

    // Y Output sample values for out-of-input-frame pixels, in case calculate (bit 0) is equal to 0
    // format: 10u
    PARAM_UINT opg_invalid_output_treatment_y;

    // Cb Output sample values for out-of-input-frame pixels, in case calculate (bit 0) is equal to 0
    // format: 10u
    PARAM_UINT opg_invalid_output_treatment_cb;

    // Cr Output sample values for out-of-input-frame pixels, in case calculate (bit 0) is equal to 0
    // format: 10u
    PARAM_UINT opg_invalid_output_treatment_cr;

    // LUT(0)
    // format: 14s
    PARAM_INT opg_interpolation_lut_0[16];

    // LUT(1)
    // format: 14s
    PARAM_INT opg_interpolation_lut_1[16];

    // LUT(2) 
    // format: 14s
    PARAM_INT opg_interpolation_lut_2[16];

} ICA_OPG;

typedef struct ICA_Chromatix_TAG
{
    ICA_TOP top;
    ICA_CTC ctc;
    ICA_OPG opg;
    // Grid value for x
    // format: 17sQ3
    PARAM_INT distorted_input_to_undistorted_ldc_grid_x[945];

    // Grid value for y
    // format: 17sQ3
    PARAM_INT distorted_input_to_undistorted_ldc_grid_y[945];

    // Grid value for x
    // format: 17sQ3
    PARAM_INT undistorted_to_lens_distorted_output_ld_grid_x[945];

    // Grid value for y
    // format: 17sQ3
    PARAM_INT undistorted_to_lens_distorted_output_ld_grid_y[945];

    // is grid valid
    // format: 1u
    PARAM_UINT distorted_input_to_undistorted_ldc_grid_valid;

    // is grid valid
    // format: 1u
    PARAM_UINT undistorted_to_lens_distorted_output_ld_grid_valid;

} ICA_Chromatix;

// ############ Functions ############
int32_t Validate_ICA_Chromatix( ICA_Chromatix* regStruct  );
void SetDefaultVal_ICA_Chromatix( ICA_Chromatix* regStruct );
// ###################################
#ifdef __cplusplus
}
#endif


#endif //__ICA_CHROMATIX_H__

