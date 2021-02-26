/*======================= COPYRIGHT NOTICE ==================================*]
Copyright (c) 2008-2009, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   audpp_mathlib.h                   TYPE: C-header file              *]
[* DESCRIPTION: Contains constants and math function decalarations for       *]
[*              post processor.                                              *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   03/17/06   xyp     Initial revision                                     *]
[*****************************************************************************/
#ifndef _MATHLIB_H
#define _MATHLIB_H

#include "audio_basic_op.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*=============================================================================
      Constants
=============================================================================*/
#define Q8_ONE          ((int16)256)
#define Q9_ONE          ((int16)512)       
#define Q13_HALF        ((int16)4096)

#define Q15_ZERO        ((int16)0)
#define Q15_ONE         ((int16)32767)
#define Q15_MINUSONE    ((int16)(-32768))
#define Q15_HALF        ((int16)16384)
#define Q15_atten3db    ((int16)23170)

#define Q16_ONE         (65536L)            // (1L<<16)
#define Q16_HALF        (32768L)               
#define Q16_TWO         ((int32)0x20000L)

#define Q23_ONE         (8388608L )         // (1L<<23)
#define Q23_MINUSONE    (-8388608L)         // (-1L<<23)
#define Q23_PI_2        (13176795L)
#define Q23_PI          (26353589L)
#define Q23_PI3_2       (39530384L)
#define Q23_TWOPI       (52707179L)
#define LONG_MAX        (2147483647L)   /* maximum (signed) long value */
#define Q23_MAX		     LONG_MAX

#define Q31_ONE         ((int32)0x7fffffffL)

/*=============================================================================
      Q15 math
=============================================================================*/
int16 Q15_initRatio(int32 numer, int32 denom);
int16 Q15_initQ16(int32 initL32Q16);
int16 Q15_initQ23(int32 initL32Q16);
int16 Q15_mult  (int16 aL16Q15,  int16 bL16Q15);
void  Q15_multBy(int16 *aL16Q15, int16 bL16Q15);
int16 Q15_mult3 (int16 aL16Q15,  int16 bL16Q15, int16 cL16Q15);
int16 Q15_scaleByInt16(int16 scaleL16Q15, int16 xL16);
int16 Q15_negate(int16 scaleL16Q15);
int16 Q15_initMB(int16 mB);
int16 Q15_getMB(int16 scaleL16Q15);

/*=============================================================================
      Q16 math
=============================================================================*/
int32 Q16_mult(int32 aL16Q16, int32 bL16Q16);
int32 Q16_square(int32 x);
int32 Q16_sqrt(int32 x);
int32 Q16_reciprocal(int32 denom);
int32 Q16_reciprocalU(int32 denom);
int32 Q16_divide_truncated(int32 numerL32Q16, int32 denomL32Q16);

/*=============================================================================
      Q23 math
=============================================================================*/
int32 Q23_initMB(int16 mB);
int32 Q23_mult(int32 aL32Q23, int32 bL32Q23);
int32 Q23_square(int32 xL32Q23);
int32 Q23_sqrt(int32 xL32Q23);
int32 Q23_reciprocal(int32 divisor);
int32 Q23_reciprocalU(int32 divisor);
int32 Q23_sine(int32 radiansL32Q23);
int32 Q23_cosine(int32 radiansL32Q23);
int32 Q23_sine0(int32 radiansL32Q23);         
            // fast version (no normalization of argument), so 0<=radians<pi/2
int32 Q23_cosine0(int32 radiansL32Q23);       
            // fast version (no normalization of argument), so 0<=radians<pi/2
int32 Q23_exp(int32 xL32Q23);
int32 Q23_ln(int32 xL32Q23);
int32 Q23_pow(int32 xL32Q23, int32 yL32Q23);
int32 Q23_getMB(int32 scaleL32Q23);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MATHLIB_H */
