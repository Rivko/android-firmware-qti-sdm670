/*========================================================================*/
/**
@file audio_divide_qx.h

This file contains declarations for optimized division functions
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
Copyright (c) 2008, 2010, 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/audio_divide_qx.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/21/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================== */

#ifndef _DIVIDE_QX
#define _DIVIDE_QX

#include "AudioComdef.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup math_operations
@{ */

/**
  Divides a 16-bit integer by a 16-bit integer.

  @param[in] var1   Numerator; number to be divided.
  @param[in] var2   Denominator; number to divide by.

  @return
  int16_t quotient.
  
  @dependencies
  None.
*/
int16_t s16_div_s16_s16_sat(int16_t var1, 
                            int16_t var2);

/**
  Divides a 32-bit integer by a 32-bit integer and returns
  a normalized result.

  @param[in]  var1          Numerator; number to be divided.
  @param[in]  var2          Denominator; number to divide by.
  @param[out] iqOutL16Ptr   Q factor for the quotient.

  @return
  int32_t quotient.
  
  @dependencies
  None.
*/
int32_t s32_div_s32_s32_normalized(int32_t var1,
                                   int32_t var2, 
                                   int16_t *iqOutL16Ptr);

/**
  Divides a 32-bit integer by a 32-bit integer, both of which are in same Q 
  format. The output quotient is in the Q format requested by the 16-bit signed 
  value, iqL16.

  The two inputs are normalized so they are represented with full 32-bit 
  precision in the range [0,1). The normalized inputs are truncated to the 
  upper 16 bits and divided. The normalized quotient is returned with 16-bit 
  precision, where the 16 bits are in the LSBs. The quotient is renormalized 
  back to the requested Q format.

  The input argument, iqL16, is the number of integer bits in the quotient. This 
  version of division only has 16 bits of precision.

  @param[in] var1    Numerator; number to be divided.
  @param[in] var2    Denominator; number to divide by.
  @param[in] iqL16   Q factor for the quotient.

  @return
  int32_t quotient.
  
  @dependencies
  None.
*/
int32_t s32_div_s32_s32_sat(int32_t var1,
                            int32_t var2,
                            int16_t iqL16);

/**
  Divides a 32-bit integer by a 32-bit integer. The numerator and denominator 
  are to be in the same Q format, and the output quotient is in the Q 
  format requested by outputQL16.

  The input argument, outputQL16, is the number of fractional bits in the quotient. 
  This version of division has 32 bits of precision.

  @param[in] numerator     Number to be divided.
  @param[in] denominator   Number to divide by.
  @param[in] outputQL16    Q factor for output.

  @return
  int32_t quotient.
  
  @dependencies
  None.
*/
int32_t divide_qx(int32_t numerator,
                  int32_t denominator,
                  int16_t outputQL16);

/**
  Divides a 32-bit integer by a 32-bit integer. The numerator and denominator 
  are to be in the same Q format, and the output quotient is in the Q factor defined 
  by the number of fractional bits in outputQ. The precision of 
  the division is 32 bits.

  @param[in] numerator     Number to be divided.
  @param[in] denominator   Number to divide by.
  @param[in] outputQ       Q factor for the output.

  @return
  int32_t quotient.
  
  @dependencies
  None.
*/
int32_t divide_int32_qx(int32_t numerator,
                        int32_t denominator,
                        int16_t outputQ);

/**
  Divides a 32-bit integer by a 32-bit integer. The numerator and denominator 
  are to be in the same Q format, the output quotient is in Q0, and the 
  division has 32 bits of precision.


  @param[in] numeratorL32     Number to be divided.
  @param[in] denominatorL32   Number to divide by.

  @return
  int32_t quotient.
  
  @dependencies
  None.
*/
int32_t divide_int32(int32_t numeratorL32,
                     int32_t denominatorL32);

/**
  Divides a 40-bit integer by a 40-bit integer.

  @param[in] Lnum     Number to be divided.
  @param[in] Ldenom   Number to divide by.
  @param[in] Qadj     Q factor.

  @return
  int40 quotient.
  
  @dependencies
  None.
*/
int40 audio_divide_dp(int40 Lnum, int40 Ldenom, int16_t Qadj);

/**
  Approximates the inversion of a 32-bit positive number. If the input is in 
  Qn and the output in Qm, m&nbsp;=&nbsp;30&nbsp;-&nbsp;SF&nbsp;-&nbsp;n. For 
  example, if the input is 0x0F000000 in Q26 (i.e., 3.75), the return values are:

  - Result = 0x44730000
  - SF = -28

  Therefore, m = 30-(-28)-26 = 32, i.e., inversion is (0x44730000)/2^32 =
  0.266464.

  @note1hang The approximation is performed with a linearly interpolated 
             lookup table.

  The cycle count of this function is 6 cycles.

  @param x   Number to be inverted.

  @return
  A 64-bit integer where the high 32 bits are the shift factor and the low 32 
  bits are the Q-shifted inverse.

  @dependencies
  None.
*/
 int64_t dsplib_approx_invert(int32_t x);

/**
  Approximates the division of a 32-bit integer by a 32-bit integer. If the 
  input is in Q0, the output is in Q(-SF). For example, if numer = 5297 
  and denom = 13555, the return values are:
  
  - Result = 839214304
  - SF = -31
  
  Therefore, the division value is 839214304/2^31 = 0.3907896.

  @note1hang The inversion is approximated by a linear interpolation of the 
  lookup table.

  The cycle count of this function is 7 cycles.

  @param[in] numer   Number to be divided.
  @param[in] denom   Number to divide by.

  @return
  int64_t quotient.
  
  @dependencies
  None.
*/
int64_t dsplib_approx_divide(int32_t numer,
                             int32_t denom);

/** 
  Inverts a 32-bit positive number using Newton's method. If the input is in 
  Q0, the output is in Q(45-SF). For example, if the input is 14767, the 
  return values are:

  - Result = 18179
  - SF = 17
  
  Therefore, the inversion value is 18179/2^(45-17) = 6.7722 x 10^-5.

  The cycle count of this function is 18 cycles.

  @param[in] x   Value to be inverted.

  @return
  int64_t inverse.
  
  @dependencies
  None.
*/
int64_t dsplib_newtons_invert(int32_t x);

/**
  Divides a 32-bit integer by a 32-bit integer.

  @param[in] Lnum     Number to be divided.
  @param[in] Ldenom   Number to divide by.
  @param[in] Qadj     Q factor.

  @return
  int40 quotient.

  @dependencies
  None.
*/
int32 audio_divide_sp(int32 Lnum, int32 Ldenom, int16 Qadj);

/** @} */ /* end_addtogroup math_operations */

#ifdef __cplusplus
}
#endif /* __cplusplus*/

#endif /* _DIVIDE_QX */
