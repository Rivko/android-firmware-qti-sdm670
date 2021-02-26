#ifndef CE_RSA_MATH_H
#define CE_RSA_MATH_H

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  
   This header file defines the interface to all S_BITINT operation utliities.
  ===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/secrsa/shared/inc/secrsa_math.h#1 $
  $DateTime: 2018/02/06 03:00:17 $ 
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  04/11/11   qxu     initial version
=============================================================================*/

#include <stdlib.h>
#include <string.h>

/* c = [a, b] */
int ce_util_lcm(S_BIGINT *a, S_BIGINT *b, S_BIGINT *c);

/* c = 1/a (mod b) for b */
int ce_util_invmod(S_BIGINT *a, S_BIGINT *b, S_BIGINT *c);

/* c = 1/a (mod b) for b */
int ce_util_invmod_slow(S_BIGINT *a, S_BIGINT *b, S_BIGINT *c);

/*c = a + b */
int ce_util_add(S_BIGINT *a, S_BIGINT *b, S_BIGINT *c);

/* c = a - b */
int ce_util_sub(S_BIGINT *a, S_BIGINT *b, S_BIGINT *c);

/* c = a * b */
int ce_util_mul(S_BIGINT *A, S_BIGINT *B, S_BIGINT *C);

/*c = a / b */
int ce_util_div(S_BIGINT *a, S_BIGINT *b, S_BIGINT *c, S_BIGINT *d);

/* c = a mod b, 0 <= c < b  */
int ce_util_mod(S_BIGINT *a, S_BIGINT *b, S_BIGINT *c);

/* c = a * b */
int ce_util_mul_d(S_BIGINT *a, BLONG b, S_BIGINT *c);

/* c = a * 2**b */
int ce_util_mul_2d(S_BIGINT *a, int b, S_BIGINT *c);

/* c = a / 2**b */
int ce_util_div_2d(S_BIGINT *a, unsigned int b, S_BIGINT *c);

#endif

