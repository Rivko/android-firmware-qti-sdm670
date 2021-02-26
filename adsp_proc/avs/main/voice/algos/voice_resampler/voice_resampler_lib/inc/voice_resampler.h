/*-----------------------------------------------------------------------
Copyright (c) 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 -----------------------------------------------------------------------*/
#ifndef __VOICE_RESAMP_H__
#define __VOICE_RESAMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"

#define VOICE_RESAMPLE_SUCCESS (0L)
#define VOICE_RESAMPLE_FAILURE (1L)

/* KPPS numbers for various modes. These are required for evaluating the offsets in various modules */
#define VOICE_RESAMPLER_KPPS_8K_TO_16K  (499 )
#define VOICE_RESAMPLER_KPPS_8K_TO_24K  (644 )
#define VOICE_RESAMPLER_KPPS_8K_TO_48K  (1227)
#define VOICE_RESAMPLER_KPPS_8K_TO_32K  (864 )
#define VOICE_RESAMPLER_KPPS_8K_MAX     (1227)

#define VOICE_RESAMPLER_KPPS_16K_TO_8K  (496 )
#define VOICE_RESAMPLER_KPPS_16K_TO_24K (1234)
#define VOICE_RESAMPLER_KPPS_16K_TO_48K (1390)
#define VOICE_RESAMPLER_KPPS_16K_TO_32K (872 )
#define VOICE_RESAMPLER_KPPS_16K_MAX    (1390)

#define VOICE_RESAMPLER_KPPS_48K_TO_8K  (1209)
#define VOICE_RESAMPLER_KPPS_48K_TO_16K (1375)
#define VOICE_RESAMPLER_KPPS_48K_TO_24K (1121)
#define VOICE_RESAMPLER_KPPS_48K_TO_32K (2562)
#define VOICE_RESAMPLER_KPPS_48K_MAX    (2562)

#define VOICE_RESAMPLER_KPPS_32K_TO_8K  (852 )
#define VOICE_RESAMPLER_KPPS_32K_TO_16K (864 )
#define VOICE_RESAMPLER_KPPS_32K_TO_24K (2125)
#define VOICE_RESAMPLER_KPPS_32K_TO_48K (2571)
#define VOICE_RESAMPLER_KPPS_32K_MAX    (2571)

#define VOICE_RESAMPLER_KPPS_24K_TO_8K  (636 )
#define VOICE_RESAMPLER_KPPS_24K_TO_16K (1230)
#define VOICE_RESAMPLER_KPPS_24K_TO_32K (2129)
#define VOICE_RESAMPLER_KPPS_24K_TO_48K (1133)
#define VOICE_RESAMPLER_KPPS_24K_MAX    (2129)

/* Delay numbers for various modes. These are required for evaluating the offsets in various modules */
#define VOICE_RESAMPLER_DELAY_8K_TO_16K   400
#define VOICE_RESAMPLER_DELAY_8K_TO_24K   230
#define VOICE_RESAMPLER_DELAY_8K_TO_32K   462
#define VOICE_RESAMPLER_DELAY_8K_TO_48K   460
#define VOICE_RESAMPLER_DELAY_8K_MAX      462

#define VOICE_RESAMPLER_DELAY_16K_TO_8K   400
#define VOICE_RESAMPLER_DELAY_16K_TO_24K  230
#define VOICE_RESAMPLER_DELAY_16K_TO_32K  205
#define VOICE_RESAMPLER_DELAY_16K_TO_48K  230
#define VOICE_RESAMPLER_DELAY_16K_MAX     400

#define VOICE_RESAMPLER_DELAY_48K_TO_8K   460
#define VOICE_RESAMPLER_DELAY_48K_TO_16K  230
#define VOICE_RESAMPLER_DELAY_48K_TO_24K  400
#define VOICE_RESAMPLER_DELAY_48K_TO_32K  117
#define VOICE_RESAMPLER_DELAY_48K_MAX     460

#define VOICE_RESAMPLER_DELAY_24K_TO_8K   230
#define VOICE_RESAMPLER_DELAY_24K_TO_16K  230
#define VOICE_RESAMPLER_DELAY_24K_TO_32K  462
#define VOICE_RESAMPLER_DELAY_24K_TO_48K  400
#define VOICE_RESAMPLER_DELAY_24K_MAX     462

#define VOICE_RESAMPLER_DELAY_32K_TO_8K   462
#define VOICE_RESAMPLER_DELAY_32K_TO_16K  205
#define VOICE_RESAMPLER_DELAY_32K_TO_24K  462
#define VOICE_RESAMPLER_DELAY_32K_TO_48K  117
#define VOICE_RESAMPLER_DELAY_32K_MAX     462


#define RS_MAX_STAGES (10)

#define RS_PRECISION_COEFF 16

typedef struct voice_resampler_config_struct_t {
   uint32 input_sampling_rate;
   uint32 output_sampling_rate;
   uint32 input_dynamic_range;
   uint32 input_frame_samples;
   uint32 output_frame_samples;
   uint32 filter_sampling_rate;
   uint32 filter_cut_off_freq;
   int32 up_sample_flag;
   int32 dn_sample_flag;
   uint32 up_sample_factor;
   uint32 dn_sample_factor;
   uint32 stages;
#if (RS_PRECISION_COEFF==32)
   const int32 *f_x[RS_MAX_STAGES];
   const int32 *f_y[RS_MAX_STAGES];
#else
   const int16 *f_x[RS_MAX_STAGES];
   const int16 *f_y[RS_MAX_STAGES];
#endif
   int32 gain;
   int32 q_in;
   int32 q_out;
   int32 q_coeff;
   int32 q_g;
   int32 q_x;
   int32 q_y;
   int32 q_accu;
   uint32 total_channel_mem_size;       // used for determining the scratch buffer size
   uint32 filter_scratch_size;       // used for determining the scratch buffer size
   uint32 filter_frame_samples;      // number of filtering samples per frame
} voice_resampler_config_struct_t;

/*
This function returns the memory size required by resampler to
store configuration information.
Arguments:
resampler_config_memory_size_ptr:output: memory size in bytes required
                                         for resampler configuration data
return value:
none
 */
void voice_resampler_get_config_mem(
      uint32 *resampler_config_memory_size_ptr
      );

/*
This function populates the config memory of voice resampler
Configuration supported are
8k to 16k
8k to 48k
16k to 8k
16k to 48k
48k to 8k
48k to 8k
8k to 8k
16k to 16k
48k to 48k
Arguments:
voice_resampler_config_ptr:intput: resampler config memory
input_sampling_rate:input: sampling rate of input signal
output_samling_rate:input: required output sampling rate
input_dynamic_range:input: determines the dynamic range of the input signal
                           respresented in bits, only 16 bit input dynamic
                           range is supported
num_in_samples:input: number of input samples for processing per frame
                      and should be multiple of 8
num_out_samples_ptr:output: pointer to number of output samples, filled
                            by the voice resampler
return value:
VOICE_RESAMPLE_SUCCESS - success
VOICE_RESAMPLE_FAILURE - failure - returned if the input config is not supported
 */
int32 voice_resampler_set_config(
      void *voice_resampler_config_ptr,
      uint32 input_sampling_rate,
      uint32 output_sampling_rate,
      uint32 input_dynamic_range,
      uint32 num_in_samples,
      uint32 *num_out_samples_ptr
      );

/*
This function returns the memory size required per channel
for the resampler. This api should be called after
voice_resampler_set_config(...)
Arguments:
voice_resampler_config_ptr:input: pointer to the re-sampler config with
                                  information on resampler configuration
resampler_channel_memory_size_ptr:output: memory size in bytes required
                                          for each channel of resampler
return value:
VOICE_RESAMPLE_SUCCESS - success
VOICE_RESAMPLE_FAILURE - failure - returned if the config in
                                   voice_resampler_config_ptr is invalid
 */
int32 voice_resampler_get_channel_mem(
      void *voice_resampler_config_ptr,
      uint32 *resampler_channel_memory_size_ptr
      );

/*
This function initializes the channel memory
Arguments:
voice_resampler_config_ptr:input: pointer to the re-sampler config with
                                  information on resampler configuration
voice_resampler_ptr:intput: resampler config memory
voice_resampler_channel_memory_ptr:intput: channel memory for resampler
resampler_channel_memory_size:intput: size of the memory availble in the mem_ptr
Return value:
VOICE_RESAMPLE_SUCCESS - success
VOICE_RESAMPLE_FAILURE - failure - returned if memory size is not enough
 */
int32 voice_resampler_channel_init(
      void *voice_resampler_config_ptr,
      void *voice_resampler_channel_memory_ptr,
      uint32 resampler_channel_memory_size
      );

/*
This function resamples the input signal and fills the output buffer.
This function cannot do inplace processing.
Arguments:
voice_resampler_config_ptr:input: pointer to the re-sampler config with
                                  information on resampler configuration
voice_resampler_channel_memory_ptr:intput: channel memory for resampler
in_ptr:input: pointer to the input signal buffer, library will typecast
              this field to int16* becuase only 16 bit input is supported.
num_in_samples:input: number of int16 samples in the input buffer,
                      this length should not be greater than frame_len
                      configured in voice_resampler_set_config
out_ptr:output: pointer to the output buffer for writing the resampled data,
                library will typecast this field to int16* becuase only 16 bit
                input is supported.

num_out_samples:input: length of the output buffer available, incorrect output
                       buffer length will result in error return
                       eg: if resmapler is configured for upsampling from 8k
                       to 16k and num_in_samples is 8, the num_out_samples
                       should be more than 16, otherwise error is returned.
Return value:
VOICE_RESAMPLE_SUCCESS - success
VOICE_RESAMPLE_FAILURE - failure - if pointers are null or input/ouput sizes
                                   dont match the resampler configuration
 */
int32 voice_resampler_process(
      void *voice_resampler_config_ptr,
      void *voice_resampler_channel_memory_ptr,
      int8 *in_ptr,
      uint32 num_in_samples,
      int8 *out_ptr,
      uint32 num_out_samples
      );

/*
   PSUEDO CODE FOR USING RESAMPLER APIS
1. Get the size of the resampler config memory using the api -
   voice_resampler_set_config(...)
2. Allocate the memory for resampler config
3. Configure the resampler config as required
   using the api - voice_resampler_set_config(...)
4. Get memory required for channel memory for the resmapler config
   using api - voice_resampler_get_channel_mem(...)
5. Allocate memory for channel/s
6. Initialize each channel memory using api - voice_resampler_channel_init(...)
7. Resample data by calling the api - voice_resampler_process(...)
8. Free the allocated memory when done
 */

#ifdef __cplusplus
}
#endif

#endif//#ifndef __VOICE_RESAMP_H__
