#ifndef AUDIO_DSP32_H
#define AUDIO_DSP32_H
/*============================================================================
  @file audio_dsp32.h

  Header file for 32 bit basic mem manipulations and filters

Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#include "AEEStdDef.h"
#include "audio_dsp.h"
#include "audpp_mathlib.h"
#include "audpp_converter.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
   Constants
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
   Typdefs
----------------------------------------------------------------------------*/
/* circular delay line (32 bit */
typedef struct delayline32 {
    int32         idx;                   // index to "now"
    int32         buf_size;              // delay buf size
    int32         *buf;                  // buf pointer
} delayline32_t; 

/*----------------------------------------------------------------------------
   Fcuntion Prototypes
----------------------------------------------------------------------------*/
/* buffers */
void buffer32_empty_v2(int32 *dest, int32 samples);

void buffer32_copy_v2(int32 *dest, int32 *src, int32 samples);

void buffer32_copy16(int32 *dest, int16 *src, int32 samples);

void buffer32_fill32(int32 *dest, int32 *src, int32 gain, int16 q_factor, int32 samples);

void buffer32_mix32(int32 *dest, int32 *src, int32 gain, int16 q_factor, int32 samples);

void buffer32_fill16(int32 *dest, int32 *src, int16 gain, int32 samples);

void buffer32_mix16(int32 *dest, int32 *src, int16 gain, int32 samples);

void buffer16_fill32(int16 *dest, int16 *src, int32 gain, int16 q_factor, int32 samples);

void buffer32_fill16src(int32 *dest, int16 *src, int16 gain, int16 q_factor, int32 samples);

void buffer32_mix16src(int32 *dest, int16 *src, int16 gain, int16 q_factor, int32 samples);

void buffer32_fill_panner(int32 *dest, int32 *src, pannerStruct *panner, int32 samples);

void buffer32_mix_panner(int32 *dest, int32 *src, pannerStruct *panner, int32 samples);

void buffer32_crossmix_panner(int32 *dest, int32 *src1, int32 *src2, pannerStruct *panner, int32 samples);

void buffer_delay_mix32
(
    int32           *destBuf,           /* output buffer                     */
    int32           *srcBuf,            /* input buffer                      */
    int16            gainL16Q15,        /* gain to the samples               */
    delayline32_t   *delayline,         /* delayline struct                  */
    int32            delay,             /* amount of delay in sample         */
    int32            samples            /* number of samples to process      */
);

void buffer_delay_fill32(int32 *destBuf, int32 *srcBuf, int16 gainL16Q15, delayline32_t *delayline, int32 delay, int32 samples);


/* delayline */
void delayline32_set(delayline32_t *delayline, int32 delayLen);

void delayline32_reset(delayline32_t *delayline);

void delayline32_update(delayline32_t *delayline, int32 *src, int32 samples);

void delayline32_inplace_delay(int32 *inplace_buf, delayline32_t *delayline, int32 samples);

void delayline32_read(int32 *dest, int32 *src, delayline32_t *delayline, int32 delay, int32 samples);

void delayline32_panner_mix_out(int32 *dest, int32 *src, delayline32_t *delayline, pannerStruct *panner, int32 delay, int32 samples);

void delayline32_panner_mix_in(delayline32_t *delayline, int32 *src, pannerStruct *panner, int32 delay, int32 samples);

void delayline32_copy ( delayline32_t *dest, delayline32_t *source);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* AUDIO_DSP32_H */
