#pragma once
/*============================================================================
  @file fx_iir.h

  IIR filter utility header file


  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary

============================================================================*/

#include <stdint.h>
#include "matrix.h"

#define FX_IIR_FILTER_ORDER 2

typedef struct
{
  int32_t fc; // LPF cutoff frequency (Q16)
  int32_t fs; // sample frequency (Q16)
  int32_t channels;
} fx_iir_config_s;

typedef struct
{
  int32_t *filter_input; // pointer to an array of length channels
} fx_iir_input_s;

typedef struct
{
  int32_t *filter_output; // pointer to an array of length channels
} fx_iir_output_s;

typedef struct
{
  fx_iir_config_s config;
  int32_t num[FX_IIR_FILTER_ORDER+1];
  int32_t den[FX_IIR_FILTER_ORDER+1];
  matrix_type input;
  matrix_type output;
  int32_t settling_samples;
  int32_t filter_count;
} fx_iir_state_s;

int32_t fx_iir_lpf_mem_req(fx_iir_config_s *config_data);
fx_iir_state_s* fx_iir_lpf_state_reset(fx_iir_config_s *config_data,
                                       void *mem);
void fx_iir_lpf_update(fx_iir_state_s *fx_iir,
                       fx_iir_input_s *input,
                       fx_iir_output_s *output);
int32_t filter_settled(fx_iir_state_s *fx_iir);


