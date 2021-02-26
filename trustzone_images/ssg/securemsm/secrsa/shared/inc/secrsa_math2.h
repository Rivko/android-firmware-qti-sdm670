#ifndef CE_RSA_MATH2_H
#define CE_RSA_MATH2_H

/*===========================================================================
   Copyright (c) 2012, 2021 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  
   This header file defines the interface to all S_BITINT operation utliities.
  ===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/secrsa/shared/inc/secrsa_math2.h#2 $
  $DateTime: 2021/01/25 01:22:48 $ 
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  10/07/12   bd      initial version
=============================================================================*/

#include <stdlib.h>
#include <string.h>

/*-----------------------------------------------------------
uint32 ce_bignum_add(BigNum *a, BigNum *b, BigNum *c)
Function implements regular addition.
c=a+b;
a and b can have different length.
All numbers maus be preallocated. - Function does not check for errors!
The c must have allocated length equal to maximum between two numbers.
Function return carry if there is.
-----------------------------------------------------------*/
uint32 ce_bignum_add(BigNum *a, BigNum *b, BigNum *c);

/*-----------------------------------------------------------
uint32 ce_bignum_sub(BigNum *a, BigNum *b, BigNum *c)
Function implements regular subtraction.
c=a-b;
a and b can have different length. Length of "a" must be equal or longer;
All numbers must be preallocated. - Function does not check for errors!
The c must have allocated memory equal to size of a.
Function return borrow if there is.
-----------------------------------------------------------*/
uint32 ce_bignum_sub(BigNum *a, BigNum *b, BigNum *c);

/*-----------------------------------------------------------
uint32 ce_bignum_modular_reduction(BigNum *a, BigNum *b, BigNum *c)
Function implements modular reduction.
c=a mod b;
a and b can have different length. Length of "a" must be longer (at least by one WORD)!;
All numbers must be preallocated. - Function does not check for errors!
The c must have allocated length equal to maximum between two numbers.
...
-----------------------------------------------------------*/
int ce_bignum_modular_reduction(BigNum *a, BigNum *b, BigNum *c);

// Reusing secrsa_math code, moving to BigNum type
/* generic PxQ multiplier */
int ce_bignum_util_mul(BigNum *A, BigNum *B, BigNum *C);

//The BigNum version of the BigInt version defined in the secmath.h
SECMATH_ERRNO_ET secmath_BIGNUM_modexp_cm(BigNum * r, BigNum * b, BigNum * e, BigNum * m);

#endif

