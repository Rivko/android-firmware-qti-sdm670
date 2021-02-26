#ifndef AUDIO_COMPLEX_BASIC_OP_H
#define AUDIO_COMPLEX_BASIC_OP_H
/************************************************************************/
/*
Copyright (c) 2013-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/************************************************************************/
/*============================================================================
  @file audio_complex_basic_op.h

  Declare methods for complex arithmetic operations.  These correspond
  mostly with existing QDSP6 intrinsics mappings defined in q6basic_op.h.
============================================================================*/
/* $Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/audio_complex_basic_op.h#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "AudioComdef.h"
#include "audio_basic_op.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

//----------------------------------------------------------------------
// int16 real(cint2x16 x);
// get real part of a complex cint2x16 number
//----------------------------------------------------------------------
#define  s16_real_c32(x)           (s16_extract_s32_l( (x) ))

//----------------------------------------------------------------------
// int16 s16_imag_c32(cint2x16 x);
// get imaginery part of a complex cint2x16 number
//----------------------------------------------------------------------
#define  s16_imag_c32(x)           (s16_extract_s32_h( (x) ))

//----------------------------------------------------------------------
// int32 s32_real_c64(cint2x32 x);
// get real part of complex number
//----------------------------------------------------------------------
#define  s32_real_c64(x)         ((int32)(x))

//----------------------------------------------------------------------
// int32 s32_imag_c64(cint2x32 x);
// get imaginery part of a complex cint2x32 number
//----------------------------------------------------------------------
#define  s32_imag_c64(x)         ((int32)((x)>>32))

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
#if ((defined __hexagon__) || (defined __qdsp6__))

#define c64_complex_s32_s32(xr,xi) Q6_P_combine_RR(xi,xr)

#define c32_conjugate_c32(x) (cint2x16)Q6_P_vconj_P_sat((int64)x)

#define c64_conjugate_c64(x) Q6_P_combine_RR(Q6_R_neg_R_sat(s32_imag_c64(x)),s32_real_c64(x))

#define c64_add_c64_c64_sat(x,y) Q6_P_vaddw_PP_sat(x,y)

#define c64_sub_c64_c64_sat(x,y) Q6_P_vsubw_PP_sat(x,y)

#define c64_avrg_c64_c64_rnd(x,y) Q6_P_vavgw_PP_crnd(x,y)

#define c64_avrg_c64_c64neg_rnd(x,y) Q6_P_vnavgw_PP_crnd_sat(x,y)

//#define c64_mult_c64_s16_shift_sat(x64,y16,ls16) Q6_P_combine_RR(Q6_R_asl_RR_sat(s32_imag_c64(Q6_P_vmpyweh_PP_s1_rnd_sat(x64,Q6_P_combine_RR(y16,y16))),ls16),Q6_R_asl_RR_sat(s32_real_c64(Q6_P_vmpyweh_PP_s1_rnd_sat(x64,Q6_P_combine_RR(y16,y16))),ls16))

#define cl0(x) Q6_R_cl0_R(x)

#else
/**
  @brief Form long complex number from real and imaginary part.

  @param xr: [in] Real part
  @param xi: [in] Imaginary part
  @return Complex number (Q6_P_combine_RR(xi,xr))
 */
cint2x32 c64_complex_s32_s32(int32 xr, int32 xi);

/**
  @brief Complex c32_conjugate_c32 (with saturation)

  @param x: [in] Complex input
  @return Complex c32_conjugate_c32 (Q6_P_vconj_P_sat(x))
 */
cint2x16 c32_conjugate_c32(cint2x16 x);

/**
  @brief Long complex c32_conjugate_c32 (with saturation)

  @param x: [in] Complex input
  @return Complex c32_conjugate_c32
 */
cint2x32 c64_conjugate_c64(cint2x32 x);

/**
  @brief Add two long complex numbers, with saturation.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return sat(x + y) (Q6_P_vaddw_PP_sat)
 */
cint2x32 c64_add_c64_c64_sat(cint2x32 x, cint2x32 y);

/**
  @brief Subtract two long complex numbers, with saturation.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return sat(x - y) (Q6_P_vsubw_PP_sat)
 */
cint2x32 c64_sub_c64_c64_sat(cint2x32 x, cint2x32 y);

/**
  @brief Average two long complex numbers with convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x + y)>>1) (Q6_P_vavgw_PP_crnd)
 */
cint2x32 c64_avrg_c64_c64_rnd(cint2x32 x, cint2x32 y);

/**
  @brief Average a long complex number with the negation of
         another, and apply convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x - y)>>1) (Q6_P_vnavgw_PP_crnd_sat)
 */
int64 c64_avrg_c64_c64neg_rnd(int64 x, int64 y);

/**
  @brief Count leading zero bits.

  @param x: [in] Input parameter
  @return Number of leading zero bits (Q6_R_cl0_R(x))
 */
int32 cl0(int32 x);

#endif

/**
  @brief Multiply a 64-bit complex number with 16-bit real number (fractional integer).

  @param var1: [in] Input 64-bit complex number
  @param var2: [in] Input 16-bit real number
  @param iqShiftL16: [in] Integer part of Qfactor of real number
  @return (var1*var2) << (iqShiftL16+1)
 */
cint2x32 c64_mult_c64_s16_shift_sat( cint2x32 var1, int16 var2, int16 iqShiftL16);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Form complex number from real and imaginary part.

  @param xr: [in] Real part
  @param xi: [in] Imaginary part
  @return Complex number (Q6_R_combine_RlRl(xi,xr))
 */
cint2x16 c32_complex_s16_s16(int16 xr, int16 xi);

/**
  @brief Fractionally multiply two complex numbers with
         16-bit real and imaginary parts, producing a
         product with 32-bit real and imaginary parts.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return sat((var1 * var2)<<1) (Q6_P_cmpy_RR_s1_sat(var1,var2))
 */
cint2x32 c64_mult_c32_c32(cint2x16 var1, cint2x16 var2);

/**
  @brief Fractionally multiply two complex numbers with
         16-bit real and imaginary parts, producing a
         rounded product with 16-bit real and imaginary parts.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return s16_round_s32_sat( sat((var1 * var2)<<1) )
          (Q6_R_cmpy_RR_s1_rnd_sat(var1,var2))
 */
cint2x16 c32_mult_c32_c32(cint2x16 var1, cint2x16 var2);

/**
  @brief Fractionally multiply one complex number with
         the c32_conjugate_c32 of another.  Each input has
         16-bit real and imaginary parts, and the product is
         rounded with 16-bit real and imaginary parts.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return s16_round_s32_sat( sat((var1 * conj(var2))<<1) )
          (Q6_R_cmpy_RR_conj_s1_rnd_sat(var1,var2))
 */
cint2x16 c32_mult_c32_c32conj(cint2x16 var1, cint2x16 var2);

/**
  @brief Average two complex numbers with convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x + y)>>1) (see Q6_P_vavgh_PP_crnd)
 */
cint2x16  c32_avrg_c32_c32_rnd(cint2x16 x, cint2x16 y);

/**
  @brief Average a complex number with the negation of another,
         and apply convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x - y)>>1) (see Q6_P_vnavgh_PP_crnd_sat)
 */
cint2x16 c32_avrg_c32_c32neg_rnd(cint2x16 x, cint2x16 y);

/**
  @brief Add two complex numbers, with saturation.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return sat(var1 + var2)
 */
cint2x16 c32_add_c32_c32_sat(cint2x16 var1, cint2x16 var2);

/**
  @brief Subract two complex numbers, with saturation.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return sat(var1 - var2)
 */
cint2x16 c32_sub_c32_c32_sat(cint2x16 var1, cint2x16 var2);

/**
  @brief Average a long complex number with the negation of
         another, and apply convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x - y)>>1) (Q6_P_vnavgw_PP_crnd_sat)
 */
int64 c64_avrg_c64_c64neg_rnd(int64 x, int64 y);

// Newly added function for Quad-mic ABF processing
/**
  @brief Multiply a complex number with 16-bit real number (fractional integer).

  @param var1: [in] Input complex number
  @param var2: [in] Input real number
  @param iqShiftL16: [in] Integer part of Qfactor of real number
  @return (var1*var2) << (iqShiftL16+1)
 */
cint2x16 c32_mult_c32_s16_shift_sat( cint2x16 var1, int16 var2, int16 iqShiftL16);

/**
  @brief Multiply a complex number with 32-bit real number (fractional integer).

  @param var1: [in] Input complex number
  @param var2: [in] Input real number
  @param iqShiftL16: [in] Integer part of Qfactor of real number
  @return (var1*var2) << (iqShiftL16+1)
 */
cint2x16 c32_mult_c32_s32_shift_sat( cint2x16 var1, int32 var2, int16 iqShiftL16);

/**
  @brief Multiply a complex number with 16-bit real number (fractional integer)
         and return 32-bit complex output.
  @param var1: [in] Input complex number
  @param var2: [in] Input real number
  @return (var1*var2) << 1
 */
cint2x32 c64_mult_c32_s16_sat( cint2x16 var1, int16 var2 );

/**
  @brief Fractionally multiply 64-bit complex number with
         32-bit complex number and produce a 64-bit product.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return sat((var1 * var2)<<1)
 */
cint2x32 c64_mult_c64_c32_rnd_sat(cint2x32 var1, cint2x16 var2);

/**
  @brief Fractionally multiply 64-bit complex number with
         32-bit complex number and produce a 64-bit product.
         QDSP6 bit exact version

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return sat((var1 * var2)<<1)
 */

cint2x32 c64_mult_c64_c32_rnd_sat_q6(cint2x32 x, cint2x16 y);


#endif /* #ifdef COMPLEX_BASIC_OP_H */
