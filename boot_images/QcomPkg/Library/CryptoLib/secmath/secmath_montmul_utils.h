#ifndef _MONTMUL_UTILS_H
#define _MONTMUL_UTILS_H
/*===========================================================================

                         S E C U R I T Y    S E R V I C E S

                S E C M A T H   R S A   I M P L E M E N T A T I O N
                              
FILE:  secmath_montmul_utils.h

DESCRIPTION:

DEPENDENCIES/ASSUMPTIONS

EXTERNALIZED FUNCTIONS             

    Copyright © 2011 Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CryptoLib/secmath/secmath_montmul_utils.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when       who            what, where, why
--------   ----           ---------------------------------------------------
05/11/11   vg             Initial Revision

===========================================================================*/


#ifdef _MSC_VER
//  #include "visualc_stdint.h"
  #define restrict  
#else
  #include <stdint.h>
#endif

#include "crypto/crypto_env_comdef.h"
#include <stddef.h> //for size_t

#ifdef	__cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif



/* utility functions -----------------------------------*/
int compare(const uint32 *a, const uint32 *b, size_t N);
int32 sub(uint32 *a, const uint32 *m, const size_t N);
int32 add(uint32 *a, const uint32 *m, const size_t N);
void right_shift(uint32 *a, size_t N, uint32 shift);
int32 addition(uint32 *z, const uint32 *a, const uint32 *b, size_t N);
void negate(uint32 *pd, int n);
int increment(uint32 *a, size_t N, uint32 dx);
int decrement(uint32 *a, size_t N, uint32 dx);

int find_bn_MSB(const uint32 *v, size_t N);
uint32 inverse32(uint32 x);

static inline uint32 is_odd(const uint32 *a)
{
  return (*a & 0x1);
}


#ifdef	__cplusplus
}
#endif

#define HI(x) ((uint32)(x>>32))
#define LO(x) ((uint32)x)
#define to64(a1, a0) (((uint64)a1<<32)+a0)


#endif

