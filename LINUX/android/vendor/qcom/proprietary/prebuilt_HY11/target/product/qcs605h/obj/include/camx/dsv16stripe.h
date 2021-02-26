//-------------------------------------------------------------------------
// Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  dsv16stripe.h
// @brief
//------------------------------------------------------------------------

#ifndef DSV16STRIPE_H
#define DSV16STRIPE_H

#define Q_FACTOR_MNDSV16 14


#include "scalers.h"
#include "modules.h"


void initDownscaleV16(const uint16_t en,
                      const uint16_t input_l_n,
                      const uint16_t output_l_m,
                      const uint16_t input_processed_length,
                      const uint16_t offsetInt,       // integer of offset
                      const uint32_t offsetDec,          // q_factor decimal part of offset
                      const uint16_t rounding_option_h,
                      const uint16_t rounding_option_v,
                      MNScaleDownInStruct_V16_1D* in_struct);

void scaleBackwardV16(const stripeROI_1D* known_output_roi,
                      stripeROI_1D* unknown_input_roi,
                      const MNScaleDownInStruct_V16_1D* frame_cfg);
void scaleForwardV16(stripeROI_1D* known_input_roi,
                     stripeROI_1D* unknown_output_roi,
                     const MNScaleDownInStruct_V16_1D* known_frame_ds_cfg,
                     MNScaleDownInStruct_V16_1D* unknown_stripe_ds_cfg);       // q_factor form
void scaleForwardV16_coordinateOnly(const stripeROI_1D* known_input_roi, stripeROI_1D* unknown_output_roi, const MNScaleDownInStruct_V16_1D* known_frame_ds_cfg);
void mndsv16StripingConfig(const stripeROI_1D* input_roi, const stripeROI_1D* output_roi,
                           const MNScaleDownInStruct_V16_1D* frameConfig, MNScaleDownInStruct_V16_1D* stripeConfig);

int scaleBackward_mndsv16_start( const int outstart, const MNScaleDownInStruct_V16_1D* frame_cfg );
int scaleBackward_mndsv16_end( const int outend, const MNScaleDownInStruct_V16_1D* frame_cfg )    ;
int mapForwardmndsV16_start(const int in_start, const MNScaleDownInStruct_V16_1D* known_frame_ds_cfg);
int mapForwardmndsV16_end(const int in_end, const MNScaleDownInStruct_V16_1D* known_frame_ds_cfg);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void StandAloneIfeMndsV16Config1d(const int16_t input_length_frame, const int16_t mnds_out_length_frame,
                                  const int16_t rounding_option_h_chroma, const int16_t rounding_option_v_chroma,
                                  const stripeROI_1D mnds_input_range, const stripeROI_1D mnds_output_range,
                                  MNScaleDownInStruct_V16_1D *mnds_config_y, MNScaleDownInStruct_V16_1D *mnds_config_c);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* DSV16STRIPE_H */
