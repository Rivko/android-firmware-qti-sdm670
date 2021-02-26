/** @file  
                        secmath_mod_exp.h

  Header for mod_exp prototype

  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                EDIT HISTORY


 when       who    what, where, why
 --------   ---     ----------------------------------------------------------
 09/16/16   SA    Initial version

=============================================================================*/

#ifndef _MOD_EXP_H
#define _MOD_EXP_H

#ifdef __cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif

// part of FIPS 140 compliance, if needed
//#define ZEROIZE_SENSITIVE_DATA

#if 0
#ifdef _MSC_VER
//  #include "visualc_stdint.h"
#define restrict
#else
#include <stdint.h>
#endif
#include <stddef.h>
#endif
INT32 mod_exp(UINT32 *restrict result, CONST UINT32 *base,
              CONST UINT32 *exponent, CONST UINT32 *modulus,
              CONST UINTN N, CONST UINTN expN, UINT32 *restrict work[4]);

#ifdef __cplusplus
}
#endif

#endif
