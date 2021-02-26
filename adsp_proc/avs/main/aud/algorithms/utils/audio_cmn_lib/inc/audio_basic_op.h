/*-------------------------------------------------------------------*/
/*                                                                   */
/* Conexant Systems, Inc., Qualcomm Technologies Inc., Nokia Inc,. Nortel         */
/* Networks, Lucent Technologies, and Motorola Inc. grants a free,   */
/* irrevocable license to 3GPP2 and its Organizational Partners to   */
/* incorporate text or other copyrightable material contained in the */
/* contribution and any modifications thereof in the creation of     */
/* 3GPP2 publications; to copyright and sell in Organizational       */
/* Partner's name any Organizational Partner's standards publication */
/* even though it may include portions of the contribution; and at   */
/* the Organizational Partner's sole discretion to permit others to  */
/* reproduce in whole or in part such contributions or the resulting */
/* Organizational Partner's standards publication. The contributor   */
/* must also be willing to grant licenses under such contributor     */
/* copyrights to third parties on reasonable, non-discriminatory     */
/* terms and conditions, as appropriate.                             */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Copyright 2001 Conexant Systems, Inc.  This document is provided  */
/* as a discussion instrument only and is not binding on Conexant    */
/* Systems, Inc. The contributor reserves the right to add to, amend */
/* or otherwise modify the contents thereof.                         */
/*                                                                   */
/* Conexant Systems, Inc. grants a free, irrevocable license to 3GPP2*/
/* and its Organization Partners to incorporate, for any legitimate  */
/* 3GPP2 purpose, any copyrightable material contained in this       */
/* contribution and any revisions thereof, in publications of 3GPP2. */
/* The contributor may hold one or more patents or applications for  */
/* letters patent that cover the information contained in this       */
/* contribution.  Noting contained in this copyright statement shall */
/* be construed as conferring by implication or estoppel, or         */
/* otherwise a license or any other right under any existing  or     */
/* later issuing patent, whether or not the use of information       */
/* herein necessarily employs an invention of any such patent or     */
/* later issuing patent.                                             */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Copyright Qualcomm Technologies Incorporated. All rights reserved.             */
/* The information contained in this contribution is provided for    */
/* the sole purpose of promoting discussion within the 3GPP2 and its */
/* Organization Partners and is not binding on the contributor.      */
/* The contributor reserves the right to add to, amend, or withdraw  */
/* the statements contained herein.                                  */
/* Qualcomm Technologies Incorporated grants a free, irrevocable license to 3GPP2 */
/* and its Organization Partners to incorporate text or other        */
/* copyrightable material contained in the  contribution and any     */
/* modifications thereof in the creation of 3GPP2 publications;      */
/* to copyright and sell in Organizational Partner's name any        */
/* Organizational Partner's standards publication even though it may */
/* include portions of the contribution; and at the Organization     */
/* Partner's sole discretion to permit others to reproduce in whole  */
/* or in part such contributions or the resulting Organizational     */
/* Partner's standards publication.  The contributor may hold one or */
/* more patents or copyrights that cover information contained in    */
/* this contribution. A license will be made available to applicants */
/* under reasonable terms and conditions that are demonstrably free  */
/* of any unfair discrimination.                                     */
/* Nothing contained herein shall be construed as conferring by      */
/* implication, estoppel, or otherwise any license or right under    */
/* any patent, whether or not the use of information herein          */
/* necessarily employs an invention of any existing or later issued  */
/* patent, or copyright. The contributor reserves the right to use   */
/* all material submitted in this contribution for his own purposes, */
/* including republication and distribution to others.               */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* The proposals in this submission have been formulated by Nokia,   */
/* Inc. (Nokia), to assist the 3GPP2 Standards Committee. This       */
/* document is offered  to the committee as a basis for discussion   */
/* and is not binding on Nokia. The requirements are subject to      */
/* change in form and in numerical values after more study. Nokia    */
/* specifically reserves the right to add to, or amend, the          */
/* quantitative statements made herein. Nothing contained herein     */
/* shall be construed as conferring by implication, estoppel, or     */
/* otherwise any license or right under any patent, whether or not   */
/* the use of information herein necessarily employs an invention of */
/* any existing or later issued patent.                              */
/* Copyright 2001 Nokia, Inc. All Rights Reserved. Nokia hereby      */
/* gives permission for copying this submission for the legitimate   */
/* purposes of the 3GPP2 Standards Committee, provided Nokia, Inc.   */
/* is credited on all copies. Distribution or reproduction of this   */
/* document, by any means, electronic, mechanical, or otherwise, in  */
/* its entirety, or any portion thereof, for monetary gain or any    */
/*  non-3GPP2 purposes is expressly prohibited.                      */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Nortel Networks grants a free, irrevocable license to 3GPP2 and   */
/* its Organizational Partners to incorporate text or other          */
/* copyrightable material contained in the contribution and any      */
/* modifications thereof in the creation of 3GPP2 publications; to   */
/* copyright and sell in Organizational Partner's name any           */
/* Organizational Partner's standards publication even though it may */
/* include portions of the contribution; and at the Organizational   */
/* Partner's sole discretion to permit others to reproduce in whole  */
/* or in part such contributions or the resulting Organizational     */
/* Partner's standards publication. The contributor must also be     */
/* willing to grant licenses under such contributor copyrights to    */
/* third parties on reasonable, non-discriminatory terms and         */
/* conditions, as appropriate.                                       */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Notice:                                                           */
/* This contribution has been prepared by the contributor to assist  */
/* 3GPP2 Technical Specifications Group C. This document is offered  */
/* to 3GPP2 Technical Specifications Group C as a basis for          */
/* discussion and should not be construed as a binding proposal on   */
/* Lucent Technologies Inc. or any other company.  Specifically,     */
/* Lucent Technologies Inc. reserves the right to modify, amend, or  */
/* withdraw the statements contained herein.                         */
/*                                                                   */
/* Permission is granted to 3GPP2 Technical Specifications Group C   */
/* participants to copy any portion of this document for legitimate  */
/* purposes of 3GPP2 Technical Specifications Group C. Copying for   */
/* monetary gain or other non-3GPP2 Technical Specifications Group C */
/* purposes is prohibited.                                           */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Grant of license Motorola Inc. grants a free, irrevocable license */
/* to 3GPP2 and its organizational partners to incorporate Motorola- */
/* supplied text or other copyrightable material contained in the    */
/* contribution and any modifications thereof in the creation of     */
/* 3GPP2 publications, to copyright and sell in organizational       */
/* partners name any organizational partners standards publications  */
/* even though it may include portions of the contribution; and at   */
/* the organizational partners sole discretion to permit others      */
/* to reproduce in whole or in part such contributions or the        */
/* resulting organizational partners standards publication. Motorola */
/* is also willing to grant licenses under such Motorola copyrights  */
/* to third parties on reasonable, non-discriminatory terms and      */
/* conditions, as appropriate.                                       */
/*                                                                   */
/* Notice:                                                           */
/* This document has been prepared by Motorola Inc. to assist the    */
/* 3GPP2 standards committee. This document is offered to the        */
/* committee as a basis for discussion and should not be considered  */
/* as a binding proposal on Motorola Inc.  or any other company.     */
/* Specifically, Motorola Inc. reserves the right to modify, amend,  */
/* or withdraw the statement contained herein. Permission is granted */
/* to 3GPP2 and its organizational partners to copy any portion of   */
/* this document for the legitimate purposes of the 3GPP2.  Copying  */
/* this document for monetary gain or other non-3GPP2 purpose is     */
/* prohibited.  Motorola Inc. may hold one or more patents of        */
/* copyrights that cover information contained in this contribution, */
/* and agrees that a license under those rights will be made         */
/* available on reasonable and non-discriminatory terms and          */
/* conditions, subject to receiving a reciprocal license in return.  */
/* Nothing contained herein shall be construed as conferring by      */
/* implication, estoppel, or otherwise any license or right under    */
/* any patent, whether or not the use of information herein          */
/* necessarily employs an invention of any existing or later issued  */
/* patent, or copyright.                                             */
/*                                                                   */
/* Notice                                                            */
/* Permission is granted to 3GPP2 participants to copy any portion of*/
/* this contribution for the legitimate purpose of the 3GPP2.        */
/* Copying this contribution for monetary gain or other non-3GPP2    */
/* purpose is prohibited.                                            */
/*                                                                   */
/*-------------------------------------------------------------------*/

/*========================================================================*/
/**
@file audio_basic_op.h

@brief Macros for math basic ops.

This file contains ASM data commands and events structures definitions.

This file is essentially identical to mathevrc.h.  It contains modified
TIA basic operations.  Bit-exact simulation of a generic 32-bit accumulator
DSP chip (fractional math).  This version has a latching overflow bit
(giOverflow) and non-compound MAC's (one where initial multiplication
does not saturate).
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The elite_audio_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/


/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/audio_basic_op.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/09/10   rkc      Created file.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================== */


#ifndef __AUDIO_BASICOP_H_
#define __AUDIO_BASICOP_H_

#include "AudioComdef.h"

#ifdef round
#undef round
#endif // round

#if ((defined __hexagon__) || (defined __qdsp6__))
#include <hexagon_protos.h>
#include <hexagon_types.h>
#endif

/** @addtogroup math_operations
@{ */

/** Maximum value of a signed 32-bit integer.
*/
static const int32 MAX_32 = 0x7FFFFFFFL;

/** Maximum value of an unsigned 32-bit integer.
*/
static const uint32 UMAX_32 = 0xFFFFFFFFL;

/** Minimum value of a signed 32-bit integer.
*/
static const int32 MIN_32 = 0x80000000L;

/** Half of the maximum value of a signed 32-bit integer.
*/
static const int32 LONGWORD_HALF = 0x40000000L;

/** Bitmask for the sign bit in a signed 32-bit integer.
*/
#define LONGWORD_SIGN  MIN_32       /* sign bit */

/** Minimum value of a signed 32-bit integer.
*/
#define LONGWORD_MIN  MIN_32

/** Maximum value of a signed 32-bit integer.
*/
#define LONGWORD_MAX  MAX_32

/** Maximum value of a signed 16-bit integer.
*/
static const int16 MAX_16 = 0x7FFF;

/** Maximum value of an unsigned 16-bit integer.
*/
static const uint16 UMAX_16 = 0xFFFF;

/** Minimum value of a signed 16-bit integer.
*/
static const int16 MIN_16 = -32768; // 0x8000; // this removes truncation warning

/** Half of the maximum value of a signed 16-bit integer.
*/
static const int16 SHORTWORD_HALF = 0x4000;

/** Bitmask for the sign bit in a signed 16-bit integer.
*/
#define SHORTWORD_SIGN  MIN_16          /* sign bit for Word16 type */

/** Minimum value of a signed 16-bit integer.
*/
#define SHORTWORD_MIN  MIN_16           /* smallest Ram */

/** Maximum value of a signed 16-bit integer.
*/
#define SHORTWORD_MAX  MAX_16           /* largest Ram */

/** @} */ /* end_addtogroup math_operations */
/** @addtogroup math_operations
@{ */

/*_________________________________________________________________________
|                                                                         |
|                            Function Prototypes                          |
|_________________________________________________________________________|
*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#if ((defined __hexagon__) || (defined __qdsp6__))
    /* addition */
    /************/
/** Adds two signed 16-bit integers and returns the signed 16-bit sum.
*/
#define s16_add_s16_s16(var1, var2)  (int16) Q6_R_add_RlRl(var1, var2)  /* 1 ops */

/**
  Adds two signed 16-bit integers and returns the signed, saturated
  16-bit sum.
*/
#define s16_add_s16_s16_sat(var1, var2)  (int16) Q6_R_add_RlRl_sat(var1, var2)

/** Adds two signed 32-bit integers and returns the signed 32-bit sum.
 */
#define s32_add_s32_s32(var1, var2)  Q6_R_add_RR(var1, var2)  /* 2 ops */

/**
  Adds two signed 32-bit integers and returns the signed, saturated
  32-bit sum.
*/
#define s32_add_s32_s32_sat(var1, var2)  Q6_R_add_RR_sat(var1, var2)

/** Adds two signed 32-bit integers and returns the signed 64-bit sum.
*/
#define s64_add_s32_s32(var1, var2)  Q6_P_add_PP((int64) var1, (int64) var2)

/**
  Adds a signed 32-bit integer and an unsigned 32-bit integer, and returns the
  signed 64-bit sum.
*/
#define s64_add_s32_u32(var1, var2)  Q6_P_add_PP((int64) var1, (uint32) var2)

/**
  Adds a signed 64-bit integer and a signed 32-bit integer, and returns the
  signed 64-bit sum.
*/
#define s64_add_s64_s32(var1, var2)  Q6_P_add_PP((int64) var1, (int64) var2)

/** Adds two signed 64-bit integers and returns the signed 64-bit sum.
*/
#define s64_add_s64_s64(var1, var2)  Q6_P_add_PP((int64) var1, (int64) var2)

/**
  Subtracts two signed 16-bit integers and returns the signed
  16-bit difference.
*/
#define s16_sub_s16_s16(var1, var2)  (int16) Q6_R_sub_RlRl(var1, var2)

/**
  Subtracts two signed 16-bit integers and returns the signed, saturated
  16-bit difference.
*/
#define s16_sub_s16_s16_sat(var1, var2)  (int16) Q6_R_sub_RlRl_sat(var1, var2)

/**
  Subtracts two signed 32-bit integers and returns the signed 32-bit
  difference.
*/
#define s32_sub_s32_s32(var1, var2)  Q6_R_sub_RR(var1, var2)

/**
  Subtracts two signed 32-bit integers and returns the signed, saturated
  32-bit difference.
*/
#define s32_sub_s32_s32_sat(var1, var2)  Q6_R_sub_RR_sat(var1,var2)

/**
  Subtracts two signed 64-bit integers and returns the signed 64-bit 
  difference.
*/
#define s64_sub_s64_s64(var1, var2)  Q6_P_sub_PP((int64) var1, (int64) var2)

    /* multiplication */
/**
  Multiplies two signed 16-bit integers and returns the signed 32-bit
  product.
*/
#define s32_mult_s16_s16(var1, var2)  Q6_R_mpy_RlRl(var1, var2)

/**
  Multiplies two unsigned 16-bit integers and returns the unsigned 32-bit
  product.
*/
#define u32_mult_u16_u16(var1, var2)  (uint32) Q6_R_mpyu_RlRl(var1, var2)

/**
   Multiplies a signed 16-bit integer and an unsigned 16-bit integer, and
   returns the signed 32-bit product.
*/
#define s32_mult_s16_u16(var1, var2)  (int32) Q6_P_mpy_RR((int32)var1, Q6_R_zxth_R(var2))

/**
  Multiplies two signed 16-bit integers, shifts left the 32-bit product to
  remove the extra sign bit, saturates if necessary, and returns the
  signed 32-bit result.
*/
#define s32_mult_s16_s16_shift_sat(var1, var2)  Q6_R_mpy_RlRl_s1_sat(var1, var2)

/**
  Multiplies two unsigned 16-bit integers, shifts left the 32-bit product
  into a 64-bit integer, and returns the signed 64-bit result.
*/
#define s64_mult_u16_u16_shift(var1,var2,shift)  Q6_P_asl_PR(u32_mult_u16_u16(var1, var2), shift)

/**
  Multiplies a signed 16-bit integer and an unsigned 16-bit integer, shifts
  left the 32-bit product by 1 into a 64-bit integer, and returns the
  signed 64-bit result.
*/
#define s64_mult_s16_u16_shift(var1,var2,shift)  Q6_P_asl_PR(Q6_P_mpy_RR((int32)var1, Q6_R_zxth_R(var2)),shift)

/**
  Multiplies two signed 16-bit integers, shifts left the 32-bit product by 1
  into a 64-bit integer, and returns the signed 64-bit result.
*/
#define s64_mult_s16_s16_shift(var1,var2,shift)  Q6_P_asl_PR(Q6_R_mpy_RlRl(var1, var2), shift)

/**
  Multiplies a signed 32-bit integer and a signed 16-bit integer, rounds and
  saturates the intermediate result to a signed 32-bit integer, and returns it.
*/
#define s32_mult_s32_s16_rnd_sat(var1, var2) Q6_R_mpy_RRl_s1_rnd_sat(var1, var2)

/**
  Multiplies a signed 32-bit integer and a signed 16-bit integer, and returns 
  the signed 64-bit product.
*/
#define s64_mult_s32_s16(var1,var2)  Q6_P_mpy_RR(var1,(int32) var2)

/**
  Multiplies a signed 32-bit integer and an unsigned 16-bit 
  integer, and returns the signed 64-bit product. 
*/
#define s64_mult_s32_u16(var1,var2)  Q6_P_mpy_RR(var1, var2)

/**
  Multiplies an unsigned 32-bit integer and a signed 16-bit integer, and
  returns the signed 64-bit product.
*/
#define s64_mult_u32_s16(var1,var2)  s64_mult_u32_s16_shift(var1, var2, 16)

/**
  Multiplies an unsigned 32-bit integer and a signed 16-bit integer, shifts 
  left (if the shift value is positive) or right (if the value is negative), 
  and returns the signed 64-bit result.
*/
#define s64_mult_u32_s16_shift(var1,var2,shift)  ((var2 >= 0)? (Q6_P_asl_PR(Q6_P_mpyu_RR(var1,var2),(shift-16))) : \
    (Q6_P_asl_PR(Q6_P_neg_P(Q6_P_mpyu_RR(var1,-var2)),(shift-16))))

/**
  Multiplies a signed 32-bit integer and an unsigned 16-bit integer, shifts 
  left (if the shift value is positive) or right (if the value is negative), 
  and returns the signed 64-bit result.
*/
#define s64_mult_s32_u16_shift(var1,var2,shift)  Q6_P_asl_PR(Q6_P_mpy_RR(var1, Q6_R_zxth_R(var2)),shift-16)

/**
  Multiplies a signed 32-bit integer and a signed 16-bit integer with shift,
  and returns the signed 64-bit result.
*/
#define s64_mult_s32_s16_shift(var1,var2,shift)  Q6_P_asl_PR(Q6_P_mpy_RR(var1,(int32) var2), shift-16)

/**
  Multiplies a signed 32-bit integer and a signed 32-bit integer, rounds and
  saturates the intermediate result to a signed 32-bit integer, and returns it.
*/
#define s32_mult_s32_s32_rnd_sat(var1, var2)  Q6_R_mpy_RR_rnd(var1, var2)

/**
  Multiplies two signed 32-bit integers and returns the signed 64-bit 
  result.
*/
#define s64_mult_s32_s32(var1,var2) Q6_P_mpy_RR(var1 , var2 )

/**
  Multiplies a signed 32-bit integer and a signed 32-bit integer with shift,
  and returns the signed 64-bit result.
*/
#define s64_mult_s32_s32_shift(var1, var2, shift)  Q6_P_asl_PR(Q6_P_mpy_RR(var1,var2), shift-32)

/**
  Multiplies a signed 32-bit integer and an unsigned 32-bit integer with
  shift, and returns the signed 64-bit result.
*/
#define s64_mult_s32_u32_shift(var1, var2, shift)  ((var1 >= 0) ? (Q6_P_asl_PR(Q6_P_mpyu_RR(var1,var2),(shift-32))) : \
    (Q6_P_asl_PR(Q6_P_neg_P(Q6_P_mpyu_RR(-var1,var2)),(shift-32))))

//There is an error with the following macro, please test locally before this macro is used. Currently no module in audio is using this
#define s64_mult_s32_u32(var1, var2)  (var1 >= 0) ? Q6_P_mpyu_RR(var1,var2) : Q6_P_neg_P(Q6_P_mpyu_RR(-var1,var2))

/**
  Performs a low-precision multiply of the two signed 32-bit input numbers
  with shift, and returns the signed 64-bit result.
*/
#define s64_mult_lp_s32_s32_shift(var1, var2, shift)  (Q6_P_asl_PR(Q6_P_mpy_RR(var1,var2), shift-32))

/**
  Performs a low-precision multiply of a signed 32-bit number and an unsigned
  32-bit number with shift, and returns the signed 64-bit result.
*/
#define s64_mult_lp_s32_u32_shift(var1, var2, shift)  s64_mult_s32_u32_shift(var1, var2, shift)

    /* arithmetic shifts */
    /*********************/
/**
  Performs a 16-bit saturation for a signed input number towards
  the maximum value (positive).
*/
#define    SAT_MAX16(x)      (x > MAX_16 ? MAX_16 : x)

/**
  Performs a 16-bit saturation for a signed input number towards 
  the minimum value (negative). 
*/
#define    SAT_MIN16(x)      (x < MIN_16 ? MIN_16 : x)

/**
  Performs an arithmetic shift right (or left if the shift is negative) for a
  signed 16-bit input number, saturates it, and returns the signed 16-bit
  result.
*/
#define s16_shr_s16_sat(var1, shift)   (int16) SAT_MIN16(SAT_MAX16(Q6_R_asr_RR_sat(var1, shift)))

/**
  Performs an arithmetic shift left (or right if the shift is
  negative) for a signed 16-bit input number, saturates it, and
  returns the signed 16-bit result.
*/
#define s16_shl_s16_sat(var1, shift)  (int16) SAT_MIN16(SAT_MAX16(Q6_R_asl_RR_sat(var1, shift)))

/**
  Performs an arithmetic shift right (or left if the shift is negative)
  for a signed 32-bit input number, saturates it, and returns the
  signed 32-bit result.
*/
#define s32_shr_s32_sat(var1, shift)  Q6_R_asr_RR_sat(var1, shift)

/**
  Performs an arithmetic shift left (or right if the shift is negative) 
  for a signed 32-bit input number, saturates it, and returns the signed 
  32-bit result.
*/
#define s32_shl_s32_sat(var1, shift)  Q6_R_asl_RR_sat(var1, shift)
#define s32_shr_s32(var1, shift)      Q6_R_asr_RI(var1, shift)
#define s32_shl_s32(var1, shift)      Q6_R_asl_RI(var1, shift)

/**
  Performs an arithmetic shift left (or right if the shift is negative)
  for a signed 32-bit input number, saturates it, rounds it, and returns
  the signed 32-bit result.
*/
#define s32_shl_s32_rnd_sat(var1,shift)  ({  int32 var2 = var1;  ((shift >= 0) ?  \
                         (s32_shl_s32_sat(var2, shift)) : \
                         (s32_add_s32_s32_sat(s32_shl_s32_sat(var2, shift),s32_and_s32_s32(s32_shl_s32_sat(var2, (shift + 1)), 0x1)))); })

/**
  Performs an arithmetic shift left (or right if the shift is negative)
  for a signed 64-bit input number and returns the signed 64-bit result.
*/
#define s64_shl_s64(var1, shift)  Q6_P_asl_PR(var1, shift)

/**
 Performs an arithmetic shift left (or right if the shift is negative)
 for a signed 16-bit input number and returns the signed 16-bit result.
*/
#define s16_shl_s16(var1, shift)  (int16) Q6_R_asl_RR(var1, shift)

/**
 Performs an arithmetic shift right (or left if the shift is negative)
 for a signed 16-bit input number and returns the signed 16-bit result.
*/
#define s16_shr_s16(var1, shift)  (int16) Q6_R_asr_RR(var1, shift)

    /* logical operations */
    /**********************/

/** ANDs two signed 16-bit integers and returns the signed 16-bit result.
*/
#define s16_and_s16_s16(var1, var2)  (int16) Q6_R_and_RR((int32) var1, (int32) var2)

/** ANDs two signed 32-bit integers and returns the signed 32-bit result.
*/
#define s32_and_s32_s32(var1, var2)  Q6_R_and_RR(var1, var2)

    /* absolute value  */
    /*******************/
/**
  Computes the absolute value of the signed 16-bit input, saturates it, and 
  returns the unsigned 16-bit result.
*/
#define u16_abs_s16_sat(var1)  (uint16)   Q6_R_asrh_R(Q6_R_abs_R_sat(Q6_R_aslh_R(var1)))//more efficient intrinsic?

/**
  Computes the absolute value of the signed 32-bit input, saturates it, and 
  returns the unsigned 32-bit result.
*/
#define u32_abs_s32_sat(var1)  (uint32)   Q6_R_abs_R_sat(var1)

    /* multiply accumulate  */
    /************************/
/**
  Multiplies two signed 16-bit numbers together with saturation,
  adds that result to the signed 32-bit input with saturation,
  rounds the result into a signed 16-bit number, and returns it.
*/
#define s16_mac_s32_s16_s16_sat_rnd(var3,var1,var2)  (int16) Q6_R_asrh_R(Q6_R_add_RR_sat(Q6_R_mpyacc_RlRl_sat(var3,var1,var2),0x08000))

/**
  Multiplies two signed 16-bit numbers together with saturation,
  subtracts that result from the signed 32-bit input with saturation, 
  rounds the result into a signed 16-bit number, and returns it.
*/
#define s16_msu_s32_s16_s16_sat_rnd(var3,var1,var2)  (int16) Q6_R_asrh_R(Q6_R_add_RR_sat(Q6_R_mpynac_RlRl_s1_sat(var3,var1,var2),0x08000))

/**
  Multiplies two signed 16-bit numbers together with saturation,
  subtracts that result from the signed 32-bit input with saturation,
  and returns the signed 32-bit result.
*/
#define s32_msu_s32_s16_s16_sat(var3,var1,var2)  Q6_R_mpynac_RlRl_s1_sat(var3, var1, var2)

/**
  Multiplies two signed 16-bit numbers, shifts the result and
  adds that result to the signed 64-bit input with
  saturation, and returns the signed 64-bit result.
*/
#define s64_mac_s64_s16_s16_shift_sat(var3,var1,var2,shift)  Q6_R_add_RR_sat(var3, Q6_R_asl_RR_sat(Q6_R_mpy_RlRl(var1,var2), shift))

/**
  Multiplies two signed 16-bit numbers, shifts the result, adds that result
  to the signed 64-bit input, and returns the signed 64-bit result.
*/
#define s64_mac_s64_s16_s16_shift  s64_mac_s64_s16_s16_shift_sat

/**
  Multiplies two signed 16-bit numbers, shifts the result, adds that result
  to the signed 64-bit input, and returns the signed 64-bit result without saturation.
*/
#define s64_mac_s64_s16_s16_shift_nosat(var3,var1,var2,shift)  Q6_P_add_PP(var3, Q6_P_asl_PR(Q6_R_mpy_RlRl(var1,var2), shift))

/**
  Multiplies two signed 16-bit integers and accumulates the result in a 
  signed 64-bit integer.
*/
#define s64_mac_s64_s16_s16_s1(var3, var1, var2) Q6_P_mpyacc_RlRl_s1(var3, var1, var2)

#define s32_mac_s32_s16_s16_sat(var3, var1, var2)   Q6_R_mpyacc_RlRl_sat(var3, var1, var2)
    /*Additional operators*/
    /**********************/

    /* negation  */
    /*************/
/**
  Negates a signed 16-bit input number, saturates it, and returns the signed 
  16-bit result.
*/
#define s16_neg_s16_sat(var1)  (int16) Q6_R_sath_R(Q6_R_neg_R( Q6_R_sxth_R(var1)))

/**
  Negates a signed 32-bit input number, saturates it, and returns the signed 
  32-bit result.
*/
#define s32_neg_s32_sat(var1)  Q6_R_neg_R_sat(var1)

    /* ALU operations */
    /******************/
/**
  Finds the one's complement of a signed 16-bit number and returns
  the signed 16-bit result.
*/
#define s16_complement_s16(var1)  (int16) Q6_R_not_R(var1)

/**
  Finds the maximum of two signed 16-bit input numbers
  and returns the signed 16-bit result.
*/
#define s16_max_s16_s16(var1, var2)  (int16) Q6_R_max_RR(var1,var2)

/**
  Finds the minimum of two signed 16-bit input numbers and returns
  the signed 16-bit result.
*/
#define s16_min_s16_s16(var1, var2)  (int16) Q6_R_min_RR(var1,var2)

/**
  Finds the minimum of two signed 32-bit input numbers
  and returns the signed 32-bit result.
*/
#define s32_min_s32_s32(var1, var2)  Q6_R_min_RR(var1,var2)

/**
  Finds the maximum of two signed 32-bit input numbers
  and returns the signed 32-bit result.
*/
#define s32_max_s32_s32(var1, var2)  Q6_R_max_RR(var1,var2)

    /* Accumulator manipulation */
    /****************************/
/**
  Puts the signed 16-bit input into the 16 LSBs of the output 32-bit and 
  returns the signed 32-bit result.
*/
#define s32_deposit_s16_l(var1)  Q6_R_sxth_R(var1)

/**
  Puts the signed 16-bit input into the 16 MSBs of the output 32-bit and 
  returns the signed 32-bit result.
*/
#define s32_deposit_s16_h(var1)  Q6_R_aslh_R(var1)

/**
  Extracts the 16 LSBs of a signed 32-bit input integer
  and returns the signed 16-bit result.
*/
#define s16_extract_s32_l(var1)  (int16) Q6_R_sxth_R(var1)

/**
  Extracts the 16 LSBs of a signed 32-bit input integer
  and returns the unsigned 16-bit result.
*/
#define u16_extract_s32_l(var1)  (uint16) Q6_R_zxth_R(var1)

/**
  Extracts the 16 MSBs of a signed 32-bit input integer
  and returns the signed 16-bit result.
*/
#define s16_extract_s32_h(var1)  (int16) Q6_R_asrh_R(var1)

/**
  Extracts the 16 MSBs of a signed 64-bit input integer
  and return a signed 16-bit result.
*/
#define s16_extract_s64_h(var1)  (int16)(Q6_P_asr_PR(var1, 16))

/**
  Extracts the 16 MSBs of a signed 64-bit input integer
  and return a signed 16-bit result.
*/
#define s16_extract_s64_h_rnd(var1)  (int16)(Q6_P_asr_PI_rnd(var1, 15))

/**
  Extracts the 16 LSBs of a signed 64-bit input integer
  and returns the signed 16-bit result.
*/
#define s16_extract_s64_l(var1)  (int16)(var1)

/**
  Saturates a signed 64-bit input number to a signed 32-bit value, extracts 
  the higher 16 bits, and returns the output.
*/
#define s16_extract_s64_h_sat(var1)  (int16)Q6_R_asrh_R(Q6_R_sat_P(var1))

/**
  Extracts the 16 LSBs of a signed 64-bit input integer
  and returns the signed 32-bit result.
*/
#define s32_extract_s64_l(var1)  (int32)(var1)

/**
  Extracts the 16 LSBs  of a signed 64-bit input integer
  and returns the unsigned 32-bit result.
*/
#define u32_extract_s64_l(var1)  (uint32)(var1)

    /* Round */
    /*********/
/**
  Rounds a signed 32-bit input number into a signed 16-bit number,
  saturates it, and returns the signed 16-bit result.
*/
#define s16_round_s32_sat(var1)  (int16) Q6_R_asrh_R(Q6_R_add_RR_sat(var1,0x00008000L))

    /* Normalization */
    /*****************/
/**
  Calculates the shift required to normalize a signed 64-bit input
  number and returns the signed 16-bit shift value.
*/
#define s16_norm_s64(var1)  (int16) ((var1 == 0) ? (0) : (Q6_R_sub_RR(Q6_R_clb_P(var1), 33)))

/**
  Calculates the shift required to normalize a unsigned 32-bit
  input number and returns the signed 16-bit shift value.
*/
#define s16_norm_s32(var1)  (int16) ((var1 == 0) ? (0) : (Q6_R_sub_RR(Q6_R_clb_R(var1),1)))

/**
  Calculates the shift required to normalize a unsigned 16-bit
  input number and returns the signed 16-bit shift value.
*/
#define s16_norm_s16(var1)  s16_norm_s32(s32_deposit_s16_h(var1))

/**
  Takes the signed 32-bit number input (possibly unnormalized) and returns the 
  number of consecutive 1s, starting with the most significant bit.
*/
#define s32_cl1_s32(var1)  (Q6_R_cl1_R(var1))

    /* Saturation manipulation routines */
    /************************************/
/**
  Saturates the input signed 64-bit input number (possibly exceeding the 
  32-bit dynamic range) to the signed 32-bit number and returns the result.
*/
#define s32_saturate_s64(var1)  Q6_R_sat_P(var1)

/**
  Saturates the input signed 32-bit input number (possibly exceeding the 
  16-bit dynamic range) to the signed 16-bit number and returns the result.
*/
#define s16_saturate_s32(var1)  (int16) Q6_R_sath_R(var1)




#else // #if !defined(__qdsp6__)

    /* addition */
    /************/
    /** Adds two signed 16-bit integers and returns the signed 16-bit sum.
    */
    int16 s16_add_s16_s16(int16 var1, int16 var2);        /* 1 ops */

    /**
      Adds two signed 16-bit integers and returns the signed, saturated
      16-bit sum.
    */
    int16 s16_add_s16_s16_sat(int16 var1, int16 var2);

    /** Adds two signed 32-bit integers and return the signed 32-bit sum.
    */
    int32 s32_add_s32_s32(int32 var1, int32 var2);        /* 2 ops */

    /**
      Adds two signed 32-bit integers and returns the signed, saturated
      32-bit sum.
    */
    int32 s32_add_s32_s32_sat(int32 var1, int32 var2);

    /** Adds two signed 64-bit integers and returns the signed 64-bit sum.
    */
    int64 s64_add_s64_s64(int64 var1, int64 var2);

    /**
      Adds a signed 64-bit integer and a signed 32-bit integer, and
      returns the signed 64-bit sum.
    */
    int64 s64_add_s64_s32(int64 var1, int32 var2);

    /** Adds two signed 32-bit integers and returns the signed 64-bit sum.
    */
    int64 s64_add_s32_s32(int32 var1, int32 var2);

    /**
      Adds a signed 32-bit integer and an unsigned 32-bit integer
      and returns the signed 64-bit sum.
    */
    int64 s64_add_s32_u32(int32 var1, uint32 var2);

    /**
      Performs subtraction between two signed 16-bit integers and returns the 
      signed 16-bit difference.
    */
    int16 s16_sub_s16_s16(int16 var1, int16 var2);        /* 1 ops */

    /**
      Performs subtraction between two signed 16-bit integers and returns the 
      signed, saturated 16-bit difference.
    */
    int16 s16_sub_s16_s16_sat(int16 var1, int16 var2);

    /**
      Performs subtraction between two signed 32-bit integers and returns
      the signed 32-bit difference.
    */
    int32 s32_sub_s32_s32(int32 var1, int32 var2);

    /**
      Performs subtraction between two signed 32-bit integers and returns
      the signed, saturated 32-bit difference.
    */
    int32 s32_sub_s32_s32_sat(int32 var1, int32 var2);        /* 2 ops */

    /**
      Subtracts two signed 64-bit integers and returns the signed 64-bit 
      difference.
    */
    int64 s64_sub_s64_s64(int64 var1, int64 var2);

    /* multiplication */
    /******************/
    /**
      Multiplies two signed 16-bit integers and returns the signed 32-bit 
      product.
    */
    int32 s32_mult_s16_s16(int16 var1, int16 var2);

    /**
      Multiplies a signed 16-bit integer and an unsigned 16-bit
      integer, and returns the signed 32-bit product.
    */
    int32 s32_mult_s16_u16(int16 var1, uint16 var2);

    /**
      Multiplies a signed 32-bit integer and a signed 32-bit
      integer, rounds and saturates the intermediate result to a
      signed 32-bit integer, and returns it.
    */
    int32 s32_mult_s32_s32_rnd_sat(int32 var1, int32 var2);

    /**
      Multiplies two unsigned 16-bit integers and returns the
      unsigned 32-bit product.
    */
    uint32 u32_mult_u16_u16(uint16 var1, uint16 var2);

    /**
      Multiplies a signed 32-bit integer and a signed 16-bit
      integer, and returns the signed 64-bit product.
    */
    int64 s64_mult_s32_s16(int32 var1, int16 var2);

    /**
      Multiplies an unsigned 32-bit integer and a signed 16-bit
      integer, and returns the signed 64-bit product.
    */
    int64 s64_mult_u32_s16(uint32 var1, int16 var2);

    /** 
      Multiplies a signed 32-bit integer and an unsigned 16-bit integer, and 
      returns the signed 64-bit result.
    */
    int64 s64_mult_s32_u16(int32 var1, uint16 var2);

    /**
      Multiplies two signed 32-bit integers and returns the signed 64-bit 
      result.
    */
    int64 s64_mult_s32_s32(int32 var1, int32 var2);

    /**
      Multiplies two signed 16-bit integers, shifts left the 32-bit product 
      by 1 into a 64-bit integer, and returns the signed 64-bit result.
    */
    int64 s64_mult_s16_s16_shift(int16 var1, int16 var2, int16 shift);

    /**
      Multiplies an unsigned 16-bit integer and a signed 16-bit
      integer, shifts left the 32-bit product by 1 into a 64-bit
      integer, and returns the signed 64-bit result.
    */
    int64 s64_mult_s16_u16_shift(int16 var1, uint16 var2, int16 shift);

    /**
      Multiplies two unsigned 16-bit integers, shifts left the 32-bit product 
      into a 64-bit integer, and returns the signed 64-bit result.
    */
    int64 s64_mult_u16_u16_shift(uint16 var1, uint16 var2, int16 shift);

    /**
      Multiplies an unsigned 32-bit integer and a signed 16-bit integer, 
      shifts left (if the shift value is positive) or right (if the value 
      is negative) and returns the signed 64-bit result.
    */
    int64 s64_mult_u32_s16_shift(uint32 var1, int16 var2, int16 shift);

    /**
      Multiplies a signed 32-bit integer and an unsigned 16-bit integer, 
      shifts left (if the shift value is positive) or right (if the value 
      is negative) and returns the signed 64-bit result.
    */
    int64 s64_mult_s32_u16_shift(int32 var1, uint16 var2, int16 shift);

    /**
      Multiplies a signed 32-bit integer and a signed 16-bit
      integer with shift, and returns the signed 64-bit result.
    */
    int64 s64_mult_s32_s16_shift(int32 var1, int16 var2, int16 shift);

    /**
      Multiplies a signed 32-bit integer and a signed 32-bit
      integer with shift, and returns the signed 64-bit result.
    */
    int64 s64_mult_s32_s32_shift(int32 var1, int32 var2, int16 shift);

    /**
      Multiplies a signed 32-bit integer and an unsigned 32-bit
      integer with shift, and returns the signed 64-bit result.
    */
    int64 s64_mult_s32_u32_shift(int32 var1, uint32 var2, int16 shift);

    /**
      Performs a low precision multiply of the two signed 32-bit
      input numbers with shift, and returns the signed 64-bit result.
    */
    int64 s64_mult_lp_s32_s32_shift(int32 var1, int32 var2, int16 shift);

    /**
      Performs a low precision multiply of a signed 32-bit number and an 
      unsigned 32-bit number with shift, and returns the signed 64-bit result.
    */
    int64 s64_mult_lp_s32_u32_shift(int32 var1, uint32 var2, int16 shift);

    /**
      Multiplies two signed 16-bit integers, shifts left the 32-bit
      product to remove the extra sign bit, saturates if
      necessary, and returns the signed 32-bit result.
    */
    int32 s32_mult_s16_s16_shift_sat(int16 var1, int16 var2);

    /**
      Multiplies a signed 32-bit integer and a signed 16-bit
      integer, rounds and saturates the intermediate result to a
      signed 32-bit integer, and returns it.
    */
    int32 s32_mult_s32_s16_rnd_sat(int32 var1, int16 var2);

    int64 s64_mult_s32_u32(int32 var1, uint32 var2);

    /* arithmetic shifts with saturation */
    /*********************/
    /**
      Performs an arithmetic shift right (or left if the shift is negative)
      for a signed 16-bit input number, saturates it, and returns the
      signed 16-bit result.
    */
    int16 s16_shr_s16_sat(int16 var1, int16 shift);       /* 1 ops */

    /**
      Performs an arithmetic shift left (or right if the shift is negative)
      for a signed 16-bit input number, saturates it, and returns the
      signed 16-bit result.
    */
    int16 s16_shl_s16_sat(int16 var1, int16 shift);       /* 1 ops */

    /**
      Performs an arithmetic shift left (or right if the shift is negative)
      for a signed 16-bit input number, saturates it, rounds it, and returns
      the signed 16-bit result.
    */
    int16 s16_shl_s16_sat_rnd(int16 var, int16 var2);       /* 2 ops */

    /**
      Performs an arithmetic shift right (or left if the shift is negative)
      for a signed 32-bit input number, saturates it, and returns the
      signed 32-bit result.
    */
    int32 s32_shr_s32_sat(int32 var1, int16 shift);           /* 2 ops */

    /**
      Performs an arithmetic shift left (or right if the shift is negative)
      for a signed 32-bit input number, saturates it, and returns the
      signed 32-bit result.
    */
    int32 s32_shl_s32_sat(int32 var1, int16 shift);           /* 2 ops */

    /**
      Performs an arithmetic shift left (or right if the shift is negative)
      for a signed 32-bit input number, saturates it, rounds it, and returns
      the signed 32-bit result.
    */
    int32 s32_shl_s32_rnd_sat(int32 L_var, int16 shift);      /* 3 ops */

    /* arithmetic shifts without saturation */
    int16 s16_shr_s16(int16 var1, int16 shift);             /* 1 ops */
    int16 s16_shl_s16(int16 var1, int16 shift);             /* 1 ops */
    int32 s32_shr_s32(int32 var1, int16 shift);             /* 2 ops */
    int32 s32_shl_s32(int32 var1, int16 shift);             /* 2 ops */

    /**
      Performs an arithmetic shift left (or right, if the shift is negative)
      for a signed 64-bit input number and returns the signed 64-bit result.
    */
    int64 s64_shl_s64(int64 var1, int16 shift);

    /* logical operations */
    /**********************/
    /** ANDs two signed 16-bit integers and returns the signed 16-bit result.
    */
    int16 s16_and_s16_s16(int16 var1, int16 var2);

    /** ANDs two signed 32-bit integers and returns the signed 32-bit result.
    */
    int32 s32_and_s32_s32(int32 var1, int32 var2);

    /* absolute value  */
    /*******************/
    /**
      Computes the absolute value of the signed 16-bit input,
      saturates it, and returns the unsigned 16-bit result.
    */
    uint16 u16_abs_s16_sat(int16 var1);     /* 1 ops */

    /**
      Computes the absolute value of the signed 32-bit input,
      saturates it, and returns the unsigned 32-bit result.
    */
    uint32 u32_abs_s32_sat(int32 var1);     /* 3 ops */

    /* multiply accumulate  */
    /************************/
    /**
      Multiplies two signed 16-bit numbers together with saturation,
      adds that result to the signed 32-bit input with saturation,
      rounds the result into a signed 16-bit number, and returns the result.
    */
    int16 s16_mac_s32_s16_s16_sat_rnd(int32 var3,
        int16 var1, int16 var2); /* 2 op */

    /**
      Multiplies two signed 16-bit numbers together with saturation,
      subtracts that result from the signed 32-bit input with saturation,
      and returns the signed 32-bit result.
    */
    int32 s32_msu_s32_s16_s16_sat(int32 var3,
        int16 var1, int16 var2);  /* 1 op */

    /**
      Multiplies two signed 16-bit numbers together with saturation, subtracts 
      that result from the signed 32-bit input with saturation, rounds the 
      result into a unsigned 16-bit number, and returns the result.
    */
    int16 s16_msu_s32_s16_s16_sat_rnd(int32 var3,
        int16 var1, int16 var2); /* 2 op */

    /**
      Multiplies two signed 16-bit numbers, shifts the product, adds it to the 
      signed 64-bit input, and returns the signed 64-bit result.
    */
    int64 s64_mac_s64_s16_s16_shift(int64 var3,
        int16 var1, int16 var2, int16 shift);

    /**
      Multiplies two signed 16-bit numbers, shifts the result, adds that result
      to the signed 64-bit input, and returns the signed 64-bit result without saturation.
    */
    int64 s64_mac_s64_s16_s16_shift_nosat(int64 var3,
        int16 var1, int16 var2, int16 shift);

    /**
      Multiplies two signed 16-bit numbers, shifts the product, adds it to the 
      signed 64-bit input with saturation, and returns the signed 64-bit 
      result.
    */
    int64 s64_mac_s64_s16_s16_shift_sat(int64 var3,
        int16 var1, int16 var2, int16 shift);

    /** 
      Multiplies two signed 16-bit integers and accumulates the result in a 
      signed 64-bit integer.
    */
    int64 s64_mac_s64_s16_s16_s1(int64 var3, int16 var1, int16 var2);

   /** 
     Multiplies two signed 16-bit integers and accumulates the result in a 
     signed 32-bit integer with saturation.
   */
    int32 s32_mac_s32_s16_s16_sat(int32 var3, int16 var1, int16 var2);

    /*
    Additional operators
    */
    /**
      Performs a special case of multiplication of a unsigned 32-bit
      number with a signed 16-bit number, and returns the signed 32-bit result.
    */
    int32   s32_mls_s32_s16_sat(int32 var1, int16 var2);

    /* negation  */
    /*************/
    /**
      Negates a signed 16-bit input number, saturates it, and returns
      the signed 16-bit result.
    */
    int16 s16_neg_s16_sat(int16 var1);      /* 1 ops */

    /**
      Negates the signed 32-bit input number, saturates it, and returns
      the signed 32-bit result.
    */
    int32 s32_neg_s32_sat(int32 var1);      /* 2 ops */

    /* ALU operations */
    /******************/
    /**
      Computes the one's complement of a signed 16-bit number and returns
      the signed 16-bit result.
    */
    int16 s16_complement_s16(int16 var1);

    /**
      Finds the maximum of two signed 16-bit input numbers
      and returns the signed 16-bit result.
    */
    int16 s16_max_s16_s16(int16 var1, int16 var2);

    /**
      Finds the minimum of two signed 16-bit input numbers
      and returns the signed 16-bit result.
    */
    int16 s16_min_s16_s16(int16 var1, int16 var2);

    /**
      Normalizes a signed 64-bit input number and returns the signed 16-bit 
      result.
    */
    int16 s16_norm_s64(int64 var1);

    /**
      Finds the minimum of two signed 32-bit input numbers
      and returns the signed 32-bit result.
    */
    int32 s32_min_s32_s32(int32 var1, int32 var2);

    /**
      Finds the maximum of two signed 32-bit input numbers
      and returns the signed 32-bit result.
    */
    int32 s32_max_s32_s32(int32 var1, int32 var2);

    /* Accumulator manipulation */
    /****************************/
    /**
      Extracts the 16 LSBs of a signed 32-bit input
      integer and return the signed 16-bit result.
    */
    int16 s16_extract_s32_l(int32 var1);    /* 1 ops */

    /**
      Extracts the 16 LSBs of a signed 32-bit input integer
      and returns the unsigned 16-bit result.
    */
    uint16 u16_extract_s32_l(int32 var1);

    /**
      Extracts the 16 MSBs of a signed 32-bit input integer and returns the 
      signed 16-bit number.
    */
    int16 s16_extract_s32_h(int32 var1);    /* 1 ops */

    /**
      Extracts the 16 LSBs of a signed 64-bit input integer
      and returns the signed 16-bit result.
    */
    int16 s16_extract_s64_l(int64 var1);

    /**
      Extracts the 16 MSBs of a signed 64-bit input integer and returns the 
      signed 16-bit result.
    */
    int16 s16_extract_s64_h(int64 var1);
    
     /**
      Extracts the 16 MSBs of a signed 64-bit input integer and returns the 
      signed 16-bit result.
    */
    int16 s16_extract_s64_h_rnd(int64 var1);

    /**
      Saturates a signed 64-bit input number to a signed 32-bit value, extracts 
      the higher 16 bits, and returns the output.
    */
    int16 s16_extract_s64_h_sat(int64 var1);

    /**
      Extracts the 16 LSBs of a signed 64-bit input integer
      and returns the signed 32-bit result.
    */
    int32 s32_extract_s64_l(int64 var1);

    /**
      Extracts the 16 LSBs of a signed 64-bit input integer
      and returns the unsigned 32-bit result.
    */
    uint32 u32_extract_s64_l(int64 var1);

    /**
      Puts the signed 16-bit input into the 16 LSBs of the output
      32-bit and returns the signed 32-bit result.
    */
    int32 L_deposit_l(int16 var1);          /* 1 ops */

    /**
      Puts the signed 16-bit input into the 16 MSBs of the output 32-bit
      and returns the signed 32-bit result.
    */
    int32 s32_deposit_s16_h(int16 var1);    /* 1 ops */

    /* Round */
    /*********/
    /**
      Rounds the signed 32-bit input number into a signed 16-bit number,
      saturates it, and returns the signed 16-bit result.
    */
    int16 s16_round_s32_sat(int32 var1);    /* 1 ops */

    /* Normalization */
    /*****************/
    /**
      Calculates the shift required to normalize a unsigned 32-bit 
      input number and returns the signed 16-bit shift value.
    */
    int16 s16_norm_s32(int32 var1);         /* 30 ops */

    /**
      Calculates the shift required to normalize a unsigned 16-bit 
      input number and returns the signed 16-bit shift value.
    */
    int16 s16_norm_s16(int16 var1);         /* 15 ops */

    /**
      Takes a signed 32-bit number as input (possibly unnormalized) and returns 
      the number of consecutive 1s starting with the most significant bit.
    */
    int32 s32_cl1_s32(int32 var1);

    /* Saturation manipulation routines */
    /************************************/
    /** Clears the overflow flag.
    */
    int  clearOverflow(void);

    /** Checks to see if an overflow/saturation/limiting has occurred.
    */
    int  isOverflow(void);

    /**
      Pulls the old overflow state from the stack and replaces the current 
      overflow status with its predecessor.
    */
    int  popOverflow(void);

    /** Sets the overflow flag.
    */
    int  setOverflow(void);

    /**
      Saturates the input signed 32-bit input number (possibly exceeding the 
      16-bit dynamic range) to the signed 16-bit number and returns that 
      result.
    */
    int16 s16_saturate_s32(int32 var1);

    /**
      Saturates the input signed 64-bit input number (possibly exceeding the 
      32-bit dynamic range) to the signed 32-bit number and returns that 
      result.
    */
    int32 s32_saturate_s64(int64 var1);

    /* Non-saturating instructions */
    /*******************************/
    /**
      Adds two signed 32-bit input numbers with carry and returns the signed 
      32-bit result.
    */
    int32 L_add_c(int32 var1, int32 var2);    /* 2 ops */

    /**
      Performs subtraction between two signed 32-bit input numbers with carry 
      and returns the signed 32-bit result.
    */
    int32 L_sub_c(int32 var1, int32 var2);  /* 2 ops */

    /**
      Multiplies two signed 16-bit numbers, adds the result to the signed 
      32-bit input, and returns the signed 32-bit result.
    */
    int32 L_macNs(int32 var3,
        int16 var1, int16 var2);            /* 1 ops */

    /**
      Multiplies two signed 16-bit numbers, subtracts the product from the 
      signed 32-bit input, and returns the signed 32-bit result.
    */
    int32 L_msuNs(int32 var3,
        int16 var1, int16 var2);            /* 1 ops */

#endif // #if !defined(__qdsp6__)

/** @} */ /* end_addtogroup math_operations */


/** @cond
*/
/* Determines if two values are different. */
boolean changed(int value1, int value2);

/* If the new value is valid, assign it to the register.

  @param[in] originalValue  Value to be changed.
  @param[in] newVAlue       New value, could be invalid.
  @return  None.
  @dependencies  None.
*/
int change_if_valid
(
    int             originalValue,          /* value to be changed           */
    int             newValue                /* new value, could be invalid   */
);
/** @endcond */


#ifdef __cplusplus
}
#endif /* __cplusplus */

// FOR BACKWARDS COMPATIBILITY of certain modules
// the follow header file is included.
#include "legacy_map.h"
#endif /*__BASICOP_H */
