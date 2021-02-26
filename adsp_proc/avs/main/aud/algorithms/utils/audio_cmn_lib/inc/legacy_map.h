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
@file legacy_map.h

This file contains ASM data commands and events structures definitions.

Maps a 64-bit name to a 40-bit name for q4 and q5.
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

$Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/legacy_map.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/30/10   ap      Created file.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================= */

#ifndef __LEGACY_MAP_H_
#define __LEGACY_MAP_H_ 

#include "AudioComdef.h"

/** @addtogroup math_operations
@{ */
    /* addition */
    /************/
/**
  Subtracts two signed 40-bit integers and returns the signed 40-bit 
  difference.
*/
#define s40_sub_s40_s40 s64_sub_s64_s64

/** Adds two signed 40-bit integers and returns the signed 40-bit result.
*/
#define s40_add_s40_s40 s64_add_s64_s64

/**
  Adds a signed 40-bit integer and a signed 32-bit integer and returns the 
  signed 40-bit sum.
*/
#define s40_add_s40_s32 s64_add_s64_s32

    /* multiplication */
    /******************/
/**
  Multiplies a signed 32-bit integer and a signed 16-bit integer and returns 
  the signed 40-bit product.
*/
#define s40_mult_s32_s16  s64_mult_s32_s16  
  
/**
  Multiplies an unsigned 32-bit integer and a signed 16-bit integer and 
  returns the signed 40-bit product.
*/
#define s40_mult_u32_s16  s64_mult_u32_s16 
   
/**
  Multiplies two signed 16-bit integers, shifts left the 32-bit product by 1 
  into a 40-bit integer, and returns the signed 40-bit result.
*/
#define s40_mult_s16_s16_shift   s64_mult_s16_s16_shift

/**
  Multiplies a signed 16-bit integer and an unsigned 16-bit integer, shifts 
  left the 32-bit product by 1 into a 40-bit integer, and returns the signed 
  40-bit result.
*/
#define s40_mult_s16_u16_shift   s64_mult_s16_u16_shift

/**
  Multiplies two unsigned 16-bit integers, shifts left the 32-bit product into 
  a 40-bit integer, and returns the signed 40-bit result.
*/
#define s40_mult_u16_u16_shift   s64_mult_u16_u16_shift

/**
  Multiplies an unsigned 32-bit integer and a signed 16-bit integer with 
  shift, and returns the signed 40-bit result.
*/
#define s40_mult_u32_s16_shift   s64_mult_u32_s16_shift

/**
  Multiplies a signed 32-bit integer and an unsigned 16-bit integer with 
  shift, and returns the signed 40-bit result.
*/
#define s40_mult_s32_u16_shift   s64_mult_s32_u16_shift

/**
  Multiplies a signed 32-bit integer and a signed 16-bit integer with shift, 
  and returns the signed 40-bit result.
*/
#define s40_mult_s32_s16_shift   s64_mult_s32_s16_shift

/**
  Multiplies a signed 32-bit integer and a signed 32-bit integer with shift, 
  and returns the signed 40-bit result.
*/
#define s40_mult_s32_s32_shift   s64_mult_s32_s32_shift

/**
  Multiplies a signed 32-bit integer and an unsigned 32-bit integer with 
  shift, and returns the signed 40-bit result.
*/
#define s40_mult_s32_u32_shift   s64_mult_s32_u32_shift

/**
  Performs a low-precision multiply of two signed 32-bit input numbers with 
  shift, and returns the signed 40-bit result.
*/
#define s40_mult_lp_s32_s32_shift s64_mult_lp_s32_s32_shift

/**
  Perform a low-precision multiply of a signed 32-bit number and an unsigned 
  32-bit number with shift, and returns the signed 40-bit result.
*/
#define s40_mult_lp_s32_u32_shift s64_mult_lp_s32_u32_shift

    /* arithmetic shifts */
    /*********************/
/**
  Performs an arithmetic shift left (or right, if the shift is negative) for a 
  signed 40-bit input number and returns the signed 40-bit result.
*/
#define s40_shl_s40 s64_shl_s64


    /* multiply accumulate  */
    /************************/
/**
  Multiplies two signed 16-bit numbers, shifts the product, adds that result 
  to the signed 40-bit input, and returns the signed 40-bit result.
*/
#define s40_mac_s40_s16_s16_shift      s64_mac_s64_s16_s16_shift

/**
  Multiplies two signed 16-bit numbers, shifts the product, adds that result 
  to the signed 40-bit input with saturation, and returns the signed 40-bit 
  result.
*/
#define s40_mac_s40_s16_s16_shift_sat  s64_mac_s64_s16_s16_shift_sat

    /* ALU operations */
    /******************/
/**
  Normalizes a signed 40-bit input number and returns the signed 16-bit 
  result.
*/
#define s16_norm_s40 s16_norm_s64

    /* Accumulator manipulation */
    /****************************/
/**
  Extracts the 16 LSBs of a signed 40-bit input integer and returns the signed 
  16-bit result.
*/
#define s16_extract_s40_l s16_extract_s64_l

/**
  Extracts the 16 MSBs (higher 16-bits) of a signed 40-bit input integer and 
  returns the signed 16-bit number.
*/
#define s16_extract_s40_h s16_extract_s64_h

/**
  Extracts the 16 LSBs of a signed 40-bit input integer and returns the signed 
  32-bit result.
*/
#define s32_extract_s40_l s32_extract_s64_l

    /* Saturation manipulation routines */
    /************************************/
/**
  Saturates the input signed 40-bit input number (possibly exceeding the 
  32-bit dynamic range) to the signed 32-bit number, and returns that result.
*/
#define s32_saturate_s40 s32_saturate_s64
 
/** @} */ /* end_addtogroup math_operations */

#endif /*__LEGACY_MAP_H_ */
