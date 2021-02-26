/**
  @file secmath_utils.h

  @brief Utility functions for crypto_math.h
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CryptoLib/secmath/secmath_utils.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/11/11   vg             Initial Revision

===========================================================================*/


#ifndef SECMATH_UTILS_H
#define SECMATH_UTILS_H

#include "crypto/secmath.h"

#ifdef __cplusplus
extern "C"
{
#endif

 
SECMATH_ERRNO_ET secmath_hex_str_to_bin(uint8 * buffer, uint32 buflen,
                                const char *str, uint32 * total_len);

SECMATH_ERRNO_ET secmath_bin_to_hex_str(char *str, uint32 len,
                                const uint8 * buf, uint32 buflen);

#ifdef __cplusplus
}
#endif

#endif
