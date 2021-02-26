/*======================= COPYRIGHT NOTICE ==================================*]
[* Copyright (c) 2018 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*/

#ifndef FFT2_PRIVATE_COMDEF_H
#define FFT2_PRIVATE_COMDEF_H
/*============================================================================
  @file fft2_private_comdef.h

  Declare methods for forward and inverse real-input FFT (QDSP6
  implementation reference).

============================================================================*/
/* $Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/fft2_private_comdef.h#2 $ */

#include "AEEStdDef.h"
#include "AudioComdef.h"
#include "audio_basic_op.h"
#include "audio_complex_basic_op.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
// Audio Basic Op
#define shr s16_shr_s16_sat
#define add s16_add_s16_s16
#define sub s16_sub_s16_s16
#define shl s16_shl_s16_sat
#define L_sub s32_sub_s32_s32_sat
#define L_mult s32_mult_s16_s16_shift_sat
#define L_add s32_add_s32_s32_sat
#define round32_16 s16_round_s32_sat
#define L_deposit_h s32_deposit_s16_h
#define L_shr s32_shr_s32_sat
#define L_negate s32_neg_s32_sat
#define Mpy_32_16_round s32_mult_s32_s16_rnd_sat
#define negate s16_neg_s16_sat
#define L_mult32x16r Mpy_32_16_round
#define L_sat s32_saturate_s64

// Audio Complex Basic Op
#define L_conjugate c64_conjugate_c64
#define L_Vadd c64_add_c64_c64_sat
#define L_Vsub c64_sub_c64_c64_sat
#define conjugate c32_conjugate_c32
#ifndef complex
#define complex c32_complex_s16_s16
#endif
#define imag s16_imag_c32
#define real s16_real_c32
#define cavg c32_avrg_c32_c32_rnd
#define cnavg c32_avrg_c32_c32neg_rnd
#define cmult_r c32_mult_c32_c32
#define cmult_cr c32_mult_c32_c32conj
#define cadd c32_add_c32_c32_sat
#define csub c32_sub_c32_c32_sat
#define L_complex c64_complex_s32_s32
#define L_real s32_real_c64
#define L_imag s32_imag_c64
#define L_Vavg c64_avrg_c64_c64_rnd
#define L_Vnavg c64_avrg_c64_c64neg_rnd

#if ((defined __hexagon__) || (defined __qdsp6__))
#define bitrev(x,y) Q6_R_lsr_RR(Q6_R_brev_R(x), 32-y)
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef int16 Word16;
typedef int32 Word32;
typedef int64 Word64;
typedef cint2x16 CWord2x16;
typedef cint2x32 CWord2x32;

#endif /* #ifndef FFT2_PRIVATE_COMDEF_H */
