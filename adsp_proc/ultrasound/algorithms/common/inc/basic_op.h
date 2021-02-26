/*----------------------------------------------------------------------
 Copyright (c) 2013 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ---------------------------------------------------------------------*/
#include "typedef_ultrasound.h"

/*___________________________________________________________________________
  | NOTE:                                                                    |
  |  The following functions will not match the ETSI standard definitions    | 
  |  at certain corner cases.                                                |
  |   - L_mac, L_msu, mac_r, msu_r                                           |
  |                                                                          |
  |  All 32-bit multiplications yield more precision than ETSI standard      |
  |  definitions                                                             |
  |__________________________________________________________________________|
 */

/*___________________________________________________________________________
  |                                                                          |
  |   Constants and Globals                                                  |
  |                                                                          |
  |__________________________________________________________________________|
 */

#define MAX_64      (Word64)0x7fffffffffffffffLL
#define MIN_64      (Word64)0x8000000000000000LL

#define MAX_32      (Word32)0x7fffffffL
#define MIN_32      (Word32)0x80000000L

#define MAX_16      (Word16)+32767 /* 0x7fff */
#define MIN_16      (Word16)-32768 /* 0x8000 */

/*___________________________________________________________________________
  |                                                                          |
  |   Prototypes for basic arithmetic operators                              |
  |__________________________________________________________________________|
 */
Word16 extract_h (Word32 L_var1);                                           /* Extract high                     */
Word16 extract_l (Word32 L_var1);                                           /* Extract low                      */
Word16 add (Word16 var1, Word16 var2);                                      /* Short add                        */
Word16 sub (Word16 var1, Word16 var2);                                      /* Short sub                        */
Word16 negate (Word16 var1);                                                /* Short negate                     */
Word16 avg (Word16 var1, Word16 var2);                                      /* Short average                    */
Word16 navg (Word16 var1, Word16 var2);                                     /* Short average negative           */
Word16 shl (Word16 var1, Word16 var2);                                      /* Short shift left                 */
Word16 shr (Word16 var1, Word16 var2);                                      /* Short shift right                */
Word16 shr_r (Word16 var1, Word16 var2);                                    /* Shift right with round           */
Word16 mult (Word16 var1, Word16 var2);                                     /* Short mult                       */
Word16 mult_r (Word16 var1, Word16 var2);                                   /* Mult with round                  */
Word16 mac_r (Word32 L_var3, Word16 var1, Word16 var2);                     /* Mac with rounding                */
Word16 msu_r (Word32 L_var3, Word16 var1, Word16 var2);                     /* Msu with rounding                */
Word16 abs_s (Word16 var1);                                                 /* Short abs                        */
Word16 rnd (Word32 L_var1);                                                 /* Round                            */
Word16 saturate (Word32 L_var1);                                            /* Short saturation                 */
Word16 norm_s (Word16 var1);                                                /* Short norm                       */
Word16 max (Word16 var1, Word16 var2);                                      /* Short maximum                    */
Word16 min (Word16 var1, Word16 var2);                                      /* Short minimum                    */

Word32 L_deposit_h (Word16 var1);                                           /* 16 bit var1 -> MSB               */
Word32 L_deposit_l (Word16 var1);                                           /* 16 bit var1 -> LSB               */
Word32 combine(Word16 var_h, Word16 var_l);                                 /* Construct 32-bit number          */
Word32 L_add (Word32 L_var1, Word32 L_var2);                                /* Long add                         */
Word32 L_sub (Word32 L_var1, Word32 L_var2);                                /* Long sub                         */
Word32 L_negate (Word32 L_var1);                                            /* Long negate                      */
Word32 L_avg (Word32 L_var1, Word32 L_var2);                                /* Long average                     */
Word32 L_navg (Word32 L_var1, Word32 L_var2);                               /* Long average negative            */
Word32 L_mult (Word16 var1, Word16 var2);                                   /* Long mult                        */
Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2);                     /* Mac                              */
Word32 L_msu (Word32 L_var3, Word16 var1, Word16 var2);                     /* Msu                              */
Word32 L_dmac(Word32 L_var5, Word16 var1, Word16 var2,
                             Word16 var3, Word16 var4);                     /* dual Mac                         */
Word32 L_shl (Word32 L_var1, Word16 var2);                                  /* Long shift left                  */
Word32 L_shr (Word32 L_var1, Word16 var2);                                  /* Long shift right                 */
Word32 L_shr_r (Word32 L_var1, Word16 var2);                                /* Long shift right with round      */
Word32 L_asl (Word32 L_var1, Word16 var2);                                  /* Long arithmetic shift left       */
Word32 L_asr (Word32 L_var1, Word16 var2);                                  /* Long arithmetic shift right      */
Word32 L_abs (Word32 L_var1);                                               /* Long abs                         */
Word16 norm_l (Word32 L_var1);                                              /* Long norm                        */
Word32 L_max (Word32 L_var1, Word32 L_var2);                                /* Long maximum                     */
Word32 L_min (Word32 L_var1, Word32 L_var2);                                /* Long minimum                     */
Word64 LL_add32 (Word32 L_var1, Word32 L_var2);                             /* add 2 Longs with LongLong result */
Word64 LL_add (Word64 L_var1, Word64 L_var2);                               /* add 2 LongLongs                  */
Word64 LL_sub (Word64 L_var1, Word64 L_var2);                               /* sub 2 LongLongs                  */
Word64 L_Vavg(Word64 x, Word64 y);                                          /* Average two long complex numbers with convergent rounding */
Word64 L_Vnavg(Word64 x, Word64 y);                                         /* Average a long complex number with the negation of
                                                                               another, and apply convergent rounding.*/

/*___________________________________________________________________________
  |                                                                          |
  |   Prototypes for basic 32-bit multiplication operators                   |
  |__________________________________________________________________________|
 */
Word32  L_mult32x16(Word32 L_var1, Word16 var2);                            /* 32x16 fractional mult            */
Word32  L_mult32x16r(Word32 L_var1, Word16 var2);                           /* 32x16 frac mult  with rounding   */
Word32  L_mult32(Word32 L_var1, Word32 L_var2);                             /* 32x32 fractional mult            */
Word32  L_mac32(Word32 L_var3, Word32 L_var1, Word32 L_var2);               /* 32-bit MAC                       */
Word32  L_msu32(Word32 L_var3, Word32 L_var1, Word32 L_var2);               /* 32-bit MSU                       */
Word32  L_imult32h(Word32 L_var1, Word32 L_var2);                           /* 32x32 mult, taking MSB           */
Word32  L_imult32hr(Word32 L_var1, Word32 L_var2);                          /* 32x32 mult, taking MSB with round*/
Word64  LL_imult32(Word32 L_var1, Word32 L_var2);                           /* signed 32x32 MPY                 */
UWord64 LL_imult32u(UWord32 L_var1, UWord32 L_var2);                        /* unsigned 32x32 MPY               */



/*___________________________________________________________________________
  |                                                                          |
  |   Prototypes for basic 64-bit arithmetic operators                       |
  |__________________________________________________________________________|
 */
Word32 L_sat (Word64 LL_var1);                                              /* Long saturation                  */
Word32 L_extract_h32(Word64 LL_var1);                                       /* Extract high 32-bits             */
Word32 L_extract_l32(Word64 LL_var1);                                       /* Extract low 32-bits              */
Word64 L_combine(Word32 L_var_h, Word32 L_var_l);                           /* Construct a 64-bits              */
Word64 LL_shl(Word64 LL_var1, Word32 L_var2);                               /* long long shift left             */
Word64 LL_shr(Word64 LL_var1, Word32 L_var2);                               /* long long shift right            */

/*___________________________________________________________________________
  |                                                                          |
  |   Prototypes for complex arithmetic operators                            |
  |__________________________________________________________________________|
 */
#define real(L_var1)                (extract_l((L_var1)))                   /* Extract real part -- low  16bits */
#define imag(L_var1)                (extract_h((L_var1)))                   /* Extract imag part -- high 16bits */
#define complex(var_r, var_i)       (combine((var_i),(var_r)))              /* Construct complex number         */
#define L_real(LL_var1)             (L_extract_l32((LL_var1)))              /* Extract real part -- low  32bits */
#define L_imag(LL_var1)             (L_extract_h32((LL_var1)))              /* Extract real part -- low  32bits */
#define L_complex(L_var_r,L_var_i)  (L_combine((L_var_i),(L_var_r)))        /* Construct long complex number    */

CWord2x16 conjugate (CWord2x16 L_var1);                                     /* conjugate complex number         */
CWord2x16 cadd (CWord2x16 L_var1, CWord2x16 L_var2);                        /* complex add                      */
CWord2x16 csub (CWord2x16 L_var1, CWord2x16 L_var2);                        /* complex sub                      */
CWord2x16 cmult_r(CWord2x16 L_var1, CWord2x16 L_var2);                      /* complex mult with round          */
CWord2x16 cmult_cr(CWord2x16 L_var1, CWord2x16 L_var2);                     /* complex mult conjugate with round*/
CWord2x32 L_cmult(CWord2x16 L_var1, CWord2x16 L_var2);                      /* Long cmplx mult                  */
CWord2x32 L_cmult_c(CWord2x16 L_var1, CWord2x16 L_var2);                    /* Long cmplx mult conjugate        */
CWord2x32 L_cmac(CWord2x32 LL_var3, CWord2x16 L_var1, CWord2x16 L_var2);    /* cmplx MAC                        */
CWord2x32 L_cmac_c(CWord2x32 LL_var3, CWord2x16 L_var1, CWord2x16 L_var2);  /* cmplx MAC conjugate              */
CWord2x32 L_cmsu(CWord2x32 LL_var3, CWord2x16 L_var1, CWord2x16 L_var2);    /* cmplx MSU                        */
CWord2x32 L_cmsu_c(CWord2x32 LL_var3, CWord2x16 L_var1, CWord2x16 L_var2);  /* cmplx MSU conjugate              */
CWord2x32 L_conjugate(CWord2x32 x);                                         /* Long complex conjugate (with saturation) */
CWord2x16  cavg(CWord2x16 x, CWord2x16 y);                                  /* Average two complex numbers with convergent rounding */
CWord2x16 cnavg(CWord2x16 x, CWord2x16 y);                                  /* Average a complex number with the negation of another */
                                                                            /* and apply convergent rounding. */


/*___________________________________________________________________________
  |                                                                          |
  |   Prototypes for spefical operators                                      |
  |__________________________________________________________________________|
 */
Word16 clb (Word32 L_var1);                                                 /* count leading bits               */
Word16 cl0 (Word32 L_var1);                                                 /* count leading 0s                 */
UWord32 bitreverse(UWord32 L_var1);                                         /* Long bit reverse                 */
UWord32 parity(UWord64 LL_va1, UWord64 LL_var2);                            /* parity                           */


/*___________________________________________________________________________
  |                                                                          |
  |   Prototypes for vector operators                                        |
  |__________________________________________________________________________|
 */
Word64 L_Vadd (Word64 LL_var1, Word64 LL_var2);                             /* vector add                       */
Word64 L_Vsub (Word64 LL_var1, Word64 LL_var2);                             /* vector add                       */
Word64 L_Vasr (Word64 LL_var1, Word16 var2);                                /* vector arithmetic shift right    */
Word64 L_Vasl (Word64 LL_var1, Word16 var2);                                /* vector arithmetic shift left     */
Word64 L_Vsxt (Word32 L_var1);                                              /* vector sign extend               */
Word32 Vrnd (Word64 LL_var1);                                               /* vector round                     */
Word32 Vsat (Word64 LL_var1);                                               /* vector saturation                */


