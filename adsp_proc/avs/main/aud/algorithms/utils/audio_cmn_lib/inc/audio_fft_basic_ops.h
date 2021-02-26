/*
Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#ifndef _AUDIO_FFT_BASIC_OPS_H
#define _AUDIO_FFT_BASIC_OPS_H

#if defined( __qdsp6__)
#include "hexagon_protos.h"
#endif

#ifdef SPKR_PROT_UNIT_TEST

#include "comdef.h"
#include "basic_op.h"
#include "complex_basic_op.h"
#include "basic_math.h"

#else
/**
 * for elite builds, use basic ops from AudioCmdLib instead of tst/QC_BASIC_Lib
 */

#include "qurt_elite_types.h"
#include "audio_basic_op.h"
#include "audio_basic_op_ext.h"
#include "basic_math.h"

#endif
/**
 * below wordxx declarations are not done in audCmdLib.
 */
typedef int16                  Word16;
typedef uint16                 UNS_Word16;
typedef int32                  Word32;
typedef uint32                 UWord32;
typedef double Word40;                    // 40-bit Long Signed integer
typedef int64                  Word64;

typedef int32                  CWord2x16;
typedef int64                  CWord2x32;
/**
 * basic op aliases.
 */

//#define MAX_16
#define  shr                        s16_shr_s16_sat
#define  shl                        s16_shl_s16_sat
#define  max_16                     s16_max_s16_s16
#define  l_add                      s32_add_s32_s32_sat
//mult1_32_32_round -> defined below.
#define  saturate                   s16_saturate_s32
#define  L_shr                      s32_shr_s32_sat
#define  l_mac_sat(z,x,y)           (Q6_R_mpyacc_RlRl_sat((z),(x),(y)))
#define  add                        s16_add_s16_s16_sat
#define  sub                        s16_sub_s16_s16_sat
//#define  div_int32
//#define  sqrt_compute
//#define  div_s
#define  l64_shift                  s64_shl_s64
#define  extract_h                  s16_extract_s32_h
#define  extract_l(x)               ((Word16)(x))
#define  l_saturate                 s32_saturate_s64
#define  mult_r(x, y)               ((Word16)Q6_R_vmpyh_RR_s1_rnd_sat( (x), (y) ))
#define  add_sat                    s16_add_s16_s16_sat
#define  S_saturate                 s16_saturate_s32

#define  L_real(x)                  ((Word32)(x))
#define  L_imag(x)                  ((Word32)((x)>>32))

#define  abs_s(x)                   ((Word16)Q6_R_sath_R(Q6_R_abs_R( (x) )))

#define  L_complex(xr,xi)           (Q6_P_combine_RR( (xi), (xr) ))

//#define  cmult32_real16_0 -> defined below.

#define  min_32                     s32_min_s32_s32

#define  l64_add_s64_s64            s64_add_s64_s64
#define  l_add_sat                  s32_add_s32_s32_sat
#define  l_sub                      s32_sub_s32_s32_sat
#define  max_32                     s32_max_s32_s32

#define  l_abs                      u32_abs_s32_sat

#define  negate                     s16_neg_s16_sat

#define  min_16                     s16_min_s16_s16

#define  norm_l(x)                  (Q6_R_normamt_R( (x) ))
#define  L_shl                      s32_shl_s32_sat
#define  round32_16                 s16_round_s32_sat
#define  Mpy_32_16_round            s32_mult_s32_s16_rnd_sat

#define  L_negate                   s32_neg_s32_sat

#define   L_saturate(dvar1)          (Q6_R_sat_P(dvar1))
#define   L_add(x, y)                (Q6_R_add_RR_sat( (x), (y) ))
#define  extract_l(x)               ((Word16)(x))
/**
  @brief Multiply a 64-bit complex number with 16-bit real number (fractional integer).
   Assumes shift is 0.
  @param var1: [in] Input 64-bit complex number
  @param var2: [in] Input 16-bit real number

  @return (var1*var2) << (1)
 */
static inline CWord2x32 cmult32_real16_0( CWord2x32 var1, int16 var2)
{
   return Q6_P_vmpyweh_PP_s1_rnd_sat(var1, Q6_P_vsplath_R(var2));
}

static inline int32 L_shift_r_s(int32 L_var1, int16 var2)
{
   int32 L_Out, L_rnd;

   if (var2 < -31)
   {
      L_Out = 0;
   }
   else if (var2 < 0)
   {
      /* right shift */
      L_rnd = L_shl(L_var1, (int16)(var2 + 1)) & 0x1;
      L_Out = l_add_sat(L_shl(L_var1, var2), L_rnd);
   }
   else
   {
      L_Out = L_shl(L_var1, var2);
   }

   return (L_Out);
}

static inline int32 mult1_32_32_round (
                       int32 wL32,
                       int32 xL32,
                       int16 iqL16
                     )
{

  int32 outL32;
  int16 w_exp, wL16;

  w_exp = norm_l(wL32);                            // Compute norm of the first input
  wL32 = L_shl(wL32, w_exp);                       // Normalize the first input
  wL16 = round32_16(wL32);                         // Round the first input to 16 bits

  outL32 = (int32 )Mpy_32_16_round(xL32, wL16);    // Perform 32 x 16 multiplication
  outL32 = L_shift_r_s(outL32, iqL16-w_exp);         // De-normalize the result

  return outL32;
}

#endif   //_FBSP_BASIC_OPS_H
