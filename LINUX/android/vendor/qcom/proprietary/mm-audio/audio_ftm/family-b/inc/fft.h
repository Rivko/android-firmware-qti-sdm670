/**

@file   fft.h
@brief  AUDIO FTM External Loopback
====================================================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

**/
#ifndef AUDIO_LOOPBACK_FFT_H
#define AUDIO_LOOPBACK_FFT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#define PI 3.1415926
#define FFT_STAGE 9 /* 2^9 = 512 */
typedef struct
{
    float real,imag;
}complex_for_fft;

extern int32_t get_max_pos(complex_for_fft *spectrum, uint32_t length);
extern int32_t create_sin_table(float* sin_table_buffer, uint32_t n_of_fft);
extern float sin_find(float x, float* sin_table_buffer, uint32_t n_of_fft);
extern float cos_find(float x, float* sin_table_buffer, uint32_t n_of_fft);
extern int32_t bit_reverse(complex_for_fft *input_data, uint32_t n_of_fft);
extern int32_t float_fft(complex_for_fft *workspace, uint32_t n_of_fft, uint32_t fft_stage);

#ifdef __cplusplus
}
#endif

#endif
