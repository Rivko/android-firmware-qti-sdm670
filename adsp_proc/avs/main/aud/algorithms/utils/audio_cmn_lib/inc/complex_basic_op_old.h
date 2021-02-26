#ifndef COMPLEX_BASIC_OP_H
#define COMPLEX_BASIC_OP_H
/*============================================================================
  @file quartet_complex_op.h

  Declare methods for complex arithmetic operations.  These correspond
  mostly with existing QDSP6 intrinsics mappings defined in q6basic_op.h.

Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#ifndef __qdsp6__
#include "comdef.h"
#include "basic_op_old.h"
#endif

#include "AEEStdDef.h"
#include "audio_fft_basic_ops.h"

#if defined( __qdsp6__)
#include "hexagon_protos.h"
#endif

#if 1 //defined (__qdsp6__)

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
//----------------------------------------------------------------------
// Word32 cl0(Word32 x);
// count leading zero bits
//----------------------------------------------------------------------
#define   cl0(x)             (Q6_R_cl0_R( (x) ))

//----------------------------------------------------------------------
// CWord2x16 complex(Word16 xr, Word16 xi);
// get imag part of complex number
//----------------------------------------------------------------------

#define  complex(xr,xi)      (Q6_R_combine_RlRl( (xi), (xr) ))

//----------------------------------------------------------------------
// CWord2x16 conjugate(CWord2x16 x);
// Complex add (with sat)
//----------------------------------------------------------------------
#define   conjugate(x)         ((CWord2x16)Q6_P_vconj_P_sat( (x) ))

//----------------------------------------------------------------------
// CWord2x16 cadd(CWord2x16 var1, CWord2x16 var2);
// Complex add (with sat)
//----------------------------------------------------------------------
#define   cadd(x, y)         (Q6_R_vaddh_RR_sat( (x), (y) ))

//----------------------------------------------------------------------
// CWord2x16 csub(CWord2x16 var1, CWord2x16 var2);
// Complex sub (with sat)
//----------------------------------------------------------------------
#define   csub(x, y)         (Q6_R_vsubh_RR_sat( (x), (y) ))

//----------------------------------------------------------------------
// Word2x16 cmult_r(Word2x16 var1, Word2x16 var2);
// Complex mult and round
//----------------------------------------------------------------------
#define   cmult_r(x, y)      (Q6_R_cmpy_RR_s1_rnd_sat( (x), (y) ))

//----------------------------------------------------------------------
// Word2x16 cmult_cr(Word2x16 var1, Word2x16 var2);
// Complex mult
//----------------------------------------------------------------------
#define   cmult_cr(x, y)     (Q6_R_cmpy_RR_conj_s1_rnd_sat( (x), (y) ))
//----------------------------------------------------------------------
// CWord2x32 complex(Word32 xr, Word16 32);
// get imag part of complex number
//----------------------------------------------------------------------
#define  L_complex(xr,xi)      (Q6_P_combine_RR( (xi), (xr) ))

//----------------------------------------------------------------------
// CWord2x16 conjugate(CWord2x16 x);
// Complex add (with sat)
//----------------------------------------------------------------------
#define   conjugate(x)         ((CWord2x16)Q6_P_vconj_P_sat( (x) ))

/*___________________________________________________________________________
 |                                                                           |
 |   Operators prototypes  for Vector (extended)                             |
 |___________________________________________________________________________|
*/
//----------------------------------------------------------------------
// Word64 L_Vadd(Word64 x, Word64 y);
// Vector add long
//----------------------------------------------------------------------
#define   L_Vadd             Q6_P_vaddw_PP_sat

//----------------------------------------------------------------------
// Word64 L_Vadd(Word64 x, Word64 y);
// Vector sub long
//----------------------------------------------------------------------
#define   L_Vsub             Q6_P_vsubw_PP_sat
/**
  @brief Average two complex numbers with convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x + y)>>1) (see Q6_P_vavgh_PP_crnd)
 */
#define cavg(x,y)            Q6_P_vavgh_PP_crnd(x,y)

/**
  @brief Average a complex number with the negation of another,
         and apply convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x - y)>>1) (see Q6_P_vnavgh_PP_crnd_sat)
 */
#define  cnavg(x,y)           Q6_P_vnavgh_PP_crnd_sat(x,y)
/**
  @brief Average two long complex numbers with convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x + y)>>1) (Q6_P_vavgw_PP_crnd)
 */
#define L_Vavg(x,y)           Q6_P_vavgw_PP_crnd(x,y)

/**
  @brief Average a long complex number with the negation of
         another, and apply convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x - y)>>1) (Q6_P_vnavgw_PP_crnd_sat)
 */
#define L_Vnavg(x,y)            Q6_P_vnavgw_PP_crnd_sat(x,y)

//CWord2x16 cmult_real(CWord2x16 var1, int16 var2, int16 iqShiftL16)

#define cmult_real(x,y,z)    Q6_R_vrndwh_P_sat ((Q6_P_vaslw_PR( (Q6_P_vmpyh_RR_s1_sat((x),(Q6_R_combine_RlRl((y),(y))))),(z) )))



/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

//----------------------------------------------------------------------
// Word16 sature(Word32 L_var1); 
// Saturate 32-bit integer to 16-bit
//----------------------------------------------------------------------
#define  sature(x)         ((Word16) saturate( (x) ))

//----------------------------------------------------------------------
// Word16 real(CWord2x16 x);     
// get real part of complex number
//----------------------------------------------------------------------
#define  real(x)           (extract_l( (x) ))

//----------------------------------------------------------------------
// Word16 imag(CWord2x16 x);     
// get imag part of complex number
//----------------------------------------------------------------------
#define  imag(x)           (extract_h( (x) ))

//----------------------------------------------------------------------
// Word32 real(CWord2x32 x);     
// get real part of complex number
//----------------------------------------------------------------------
#define  L_real(x)         ((Word32)(x))

//----------------------------------------------------------------------
// Word32 imag(CWord2x32 x);     
// get imag part of complex number
//----------------------------------------------------------------------
#define  L_imag(x)         ((Word32)((x)>>32))

//----------------------------------------------------------------------
// Word32 L_sat(Word64 L_var);              
// saturate 64 bit to 32 bit               
//----------------------------------------------------------------------
#define  L_sat(x)          (l_saturate( (x) ))

//----------------------------------------------------------------------
// Word32 L_mult32x16r(Word32 var1, Word16 var2);
// 32x16 multiply (fractional)
//----------------------------------------------------------------------
//#define   L_mult32x16r(x, y)   (Q6_R_mpy_RRl_s1_rnd_sat( (x), (y) ))    
#define  L_mult32x16r(x, y)   ((Word32) L_saturate(Mpy_32_16_round( (x), (y) )))

#define L_conjugate(x) ( L_complex( L_real(x), L_negate(L_imag(x)) ) )

#else

/**
 * below wordxx declarations are not done in audCmdLib.
 */
typedef int16                  Word16;
typedef uint16                 UNS_Word16;
typedef int32                  Word32;
typedef uint32                 UWord32;
typedef double Word40;                    // 40-bit Long Signed integer
typedef int64                  Word64;

//typedef int32                  CWord2x16;
//typedef int64                  CWord2x32;
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
//----------------------------------------------------------------------
// Word32 real(CWord2x32 x);     
// get real part of complex number
//----------------------------------------------------------------------
#define  L_real(x)         ((Word32)(x))

//----------------------------------------------------------------------
// Word32 imag(CWord2x32 x);     
// get imag part of complex number
//----------------------------------------------------------------------
#define  L_imag(x)         ((Word32)((x)>>32))

//----------------------------------------------------------------------
// Word16 real(CWord2x16 x);     
// get real part of complex number
//----------------------------------------------------------------------
#define  real(x)           (extract_l( (x) ))

//----------------------------------------------------------------------
// Word16 imag(CWord2x16 x);     
// get imag part of complex number
//----------------------------------------------------------------------
#define  imag(x)           (extract_h( (x) ))
#define  L_mult32x16r(x, y)   ((Word32) L_saturate(Mpy_32_16_round( (x), (y) )))
#define  L_sat(x)          (l_saturate( (x) ))
#define  sature(x)         ((Word16) saturate( (x) ))
/**
  @brief Count leading zero bits.

  @param x: [in] Input parameter
  @return Number of leading zero bits (Q6_R_cl0_R(x))
 */
Word32 cl0(Word32 x);

/**
  @brief Form complex number from real and imaginary part.

  @param xr: [in] Real part
  @param xi: [in] Imaginary part
  @return Complex number (Q6_R_combine_RlRl(xi,xr))
 */
CWord2x16 complex(Word16 xr, Word16 xi);

/**
  @brief Complex conjugate (with saturation)

  @param x: [in] Complex input
  @return Complex conjugate (Q6_P_vconj_P_sat(x))
 */
CWord2x16 conjugate(CWord2x16 x);

/**
  @brief Fractionally multiply two complex numbers with
         16-bit real and imaginary parts, producing a
         product with 32-bit real and imaginary parts.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return sat((var1 * var2)<<1) (Q6_P_cmpy_RR_s1_sat(var1,var2))
 */
CWord2x32 cmult(CWord2x16 var1, CWord2x16 var2);

/**
  @brief Fractionally multiply two complex numbers with
         16-bit real and imaginary parts, producing a
         rounded product with 16-bit real and imaginary parts.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return round( sat((var1 * var2)<<1) )
          (Q6_R_cmpy_RR_s1_rnd_sat(var1,var2))
 */
CWord2x16 cmult_r(CWord2x16 var1, CWord2x16 var2);

/**
  @brief Fractionally multiply one complex number with
         the conjugate of another.  Each input has
         16-bit real and imaginary parts, and the product is
         rounded with 16-bit real and imaginary parts.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return round( sat((var1 * conj(var2))<<1) )
          (Q6_R_cmpy_RR_conj_s1_rnd_sat(var1,var2))
 */
CWord2x16 cmult_cr(CWord2x16 var1, CWord2x16 var2);

/**
  @brief Average two complex numbers with convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x + y)>>1) (see Q6_P_vavgh_PP_crnd)
 */
CWord2x16  cavg(CWord2x16 x, CWord2x16 y);

/**
  @brief Average a complex number with the negation of another,
         and apply convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x - y)>>1) (see Q6_P_vnavgh_PP_crnd_sat)
 */
CWord2x16 cnavg(CWord2x16 x, CWord2x16 y);

/**
  @brief Add two complex numbers, with saturation.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return sat(var1 + var2)
 */
CWord2x16 cadd(CWord2x16 var1, CWord2x16 var2);

/**
  @brief Subract two complex numbers, with saturation.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return sat(var1 - var2)
 */
CWord2x16 csub(CWord2x16 var1, CWord2x16 var2);

/**
  @brief Form long complex number from real and imaginary part.

  @param xr: [in] Real part
  @param xi: [in] Imaginary part
  @return Complex number (Q6_P_combine_RR(xi,xr))
 */
CWord2x32 L_complex(Word32 xr, Word32 xi);

/**
  @brief Long complex conjugate (with saturation)

  @param x: [in] Complex input
  @return Complex conjugate
 */
CWord2x32 L_conjugate(CWord2x32 x);

/**
  @brief Add two long complex numbers, with saturation.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return sat(x + y) (Q6_P_vaddw_PP_sat)
 */
Word64 L_Vadd(Word64 x, Word64 y);

/**
  @brief Subtract two long complex numbers, with saturation.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return sat(x - y) (Q6_P_vsubw_PP_sat)
 */
Word64 L_Vsub(Word64 x, Word64 y);

/**
  @brief Average two long complex numbers with convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x + y)>>1) (Q6_P_vavgw_PP_crnd)
 */
Word64 L_Vavg(Word64 x, Word64 y);

/**
  @brief Average a long complex number with the negation of
         another, and apply convergent rounding.

  @param x: [in] First complex number
  @param y: [in] Second complex number
  @return convround((x - y)>>1) (Q6_P_vnavgw_PP_crnd_sat)
 */
Word64 L_Vnavg(Word64 x, Word64 y);

/**
  @brief Multiply a 64-bit complex number with 16-bit real number (fractional integer).
   Assumes shift is 0.
  @param var1: [in] Input 64-bit complex number
  @param var2: [in] Input 16-bit real number

  @return (var1*var2) << (1)
 */
CWord2x32 cmult32_real16_0( CWord2x32 var1, int16 var2);
#endif //__qdsp6__

// Newly added function for Quad-mic ABF processing
/**
  @brief Multiply a complex number with 16-bit real number (fractional integer).

  @param var1: [in] Input complex number
  @param var2: [in] Input real number
  @param iqShiftL16: [in] Integer part of Qfactor of real number
  @return (var1*var2) << (iqShiftL16+1)
 */
CWord2x16 cmult_real16( CWord2x16 var1, int16 var2, int16 iqShiftL16);

/**
  @brief Multiply a complex number with 32-bit real number (fractional integer).

  @param var1: [in] Input complex number
  @param var2: [in] Input real number
  @param iqShiftL16: [in] Integer part of Qfactor of real number
  @return (var1*var2) << (iqShiftL16+1)
 */
CWord2x16 cmult_real32( CWord2x16 var1, int32 var2, int16 iqShiftL16);

/**
  @brief Multiply a complex number with 16-bit real number (fractional integer)
         and return 32-bit complex output.
  @param var1: [in] Input complex number
  @param var2: [in] Input real number
  @return (var1*var2) << 1
 */
CWord2x32 cmult_real16_out32( CWord2x16 var1, int16 var2 );

/**
  @brief Multiply a 64-bit complex number with 16-bit real number (fractional integer).

  @param var1: [in] Input 64-bit complex number
  @param var2: [in] Input 16-bit real number
  @param iqShiftL16: [in] Integer part of Qfactor of real number
  @return (var1*var2) << (iqShiftL16+1)
 */
CWord2x32 cmult32_real16( CWord2x32 var1, int16 var2, int16 iqShiftL16);

/**
  @brief Fractionally multiply 64-bit complex number with
         32-bit complex number and produce a 64-bit product.

  @param var1: [in] First complex number
  @param var2: [in] Second complex number
  @return sat((var1 * var2)<<1)
 */
CWord2x32 cmult32_16(CWord2x32 var1, CWord2x16 var2);

#endif /* #ifdef COMPLEX_BASIC_OP_H */
