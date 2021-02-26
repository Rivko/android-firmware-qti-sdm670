#ifndef LIMITER_CALIBRATION_API_H
#define LIMITER_CALIBRATION_API_H
/*============================================================================
  @file limiter_calibration_api.h

  Calibration (Public) API for Low Distortion Limiter, including zero-crossing algorithm
  and peak history (lower distortion) algorithm

Copyright (c) 2013-2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#include "AudioComdef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
   Parameters with IDs
----------------------------------------------------------------------------*/
#define LIMITER_PARAM_LIB_VER    (0)   // ** param: library version
typedef int32 limiter_version_t;       //    access: get only

#define LIMITER_PARAM_MODE       (1)   // ** param: processing mode
typedef enum limiter_mode_t {          //    access: get & set
   MAKEUPGAIN_ONLY = 0,                //    0: apply makeup gain, no delay
   NORMAL_PROC,                        //    1: normal processing
} limiter_mode_t;

#define LIMITER_PARAM_BYPASS     (2)   // ** param: bypass
typedef int32 limiter_bypass_t;        //    access: get & set
                                       //    range: [1: bypass 0: normal]

#define LIMITER_PARAM_TUNING     (3)   // ** param: per channel tuning param
                                       //    access: get & set
typedef struct limiter_tuning_t {      //    note: runtime ok, no mem flush
   int32             ch_idx;           //    channel index for this set 
   int32             threshold;        //    threshold (16bit: Q15; 32bit:Q27)
                                       //    range: 16bit - 0 ~ -96 dB; 24bit - 24 ~ -120dB; 32bit - 0 ~ -120dB
                                       //    default: corresponds to -3 dB value
   int32             makeup_gain;      //    make up gain (Q8)
                                       //    range: -48 ~ 42 dB
                                       //    default: 0 dB
   int32             gc;               //    recovery const (Q15)
                                       //    range: 0 ~ 1
                                       //    default: 0.99
   int32             max_wait;         //    max wait (Q15 value of sec)
                                       //    range: 0 ~ 10 miliseconds
                                       //    default: 2ms
} limiter_tuning_t;

#define LIMITER_PARAM_RESET      (4)   // ** param: reset internal memories 
                                       //    access: set only

#define LIMITER_PARAM_TUNING_V2     (5)   // ** param: per channel tuning param
                                          //    access: get & set
typedef struct limiter_tuning_v2_t {      //    note: runtime ok, no mem flush
   int32             ch_idx;           //    channel index for this set 
   int32             threshold;        //    threshold (16bit: Q15; 32bit:Q27)
                                       //    range: 16bit - 0 ~ -96 dB; 24bit - 24 ~ -120dB; 32bit - 0 ~ -120dB
                                       //    default: corresponds to -3 dB value
   int32             makeup_gain;      //    make up gain (Q8)
                                       //    range: -48 ~ 42 dB
                                       //    default: 0 dB
   int32             gc;               //    recovery const (Q15)
                                       //    range: 0 ~ 1
                                       //    default: 0.99
   int32             max_wait;         //    max wait (Q15 value of sec)
                                       //    range: 0 ~ 10 miliseconds
                                       //    default: 2ms

   uint32            gain_attack;      //    limiter gain attack time const (Q31)
                                       //    range: corresponds to 0 ~ 5 seconds attack time
                                       //    default: corresponds to 0.5 ms attack time
   uint32            gain_release;     //    limiter gain release time const(Q31)
                                       //    range: corresponds to 0 ~ 5 seconds release time
                                       //    default: corresponds to 3 ms release time
   uint32            attack_coef;      //    limiter gain attack time speed coef (Q15)
                                       //    range: 1 ~ 100
                                       //    default: 1
   uint32            release_coef;     //    limiter gain release time speed coef (Q15)
                                       //    range: 1 ~ 100
                                       //    default: 1
   int32             hard_threshold;   //    threshold for hard limiter (16bit: Q15; 32bit:Q27)
                                       //    range: 16bit - 0 ~ -96 dB; 24bit - 24 ~ -120dB; 32bit - 0 ~ -120dB
                                       //    default: corresponds to -3 dB value
} limiter_tuning_v2_t;



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIMITER_CALIBRATION_API_H */
