#ifndef _MOD_EXP_H
#define _MOD_EXP_H

/*===========================================================================

                         S E C U R I T Y    S E R V I C E S

                S E C M A T H   R S A   I M P L E M E N T A T I O N
                              
FILE:  secmath_mod_exp.h

DESCRIPTION:

DEPENDENCIES/ASSUMPTIONS

EXTERNALIZED FUNCTIONS             

    Copyright © 2011 Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CryptoLib/secmath/secmath_mod_exp.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when       who            what, where, why
--------   ----           ---------------------------------------------------
05/11/11   vg             Initial Revision

===========================================================================*/


#ifdef   __cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif

// part of FIPS 140 compliance, if needed
//#define ZEROIZE_SENSITIVE_DATA

#ifdef _MSC_VER
//  #include "visualc_stdint.h"
  #define restrict  
#else
  #include <stdint.h>
#endif
#include <stddef.h>
 
int mod_exp(uint32 *restrict result, const uint32 *base, 
        const uint32 *exponent, const uint32 *modulus,
            const size_t N, const size_t expN, uint32 *restrict work[4]);

#ifdef __cplusplus
}
#endif

#endif
