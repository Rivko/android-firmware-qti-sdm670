#ifndef CE_RSA_UTILS_H
#define CE_RSA_UTILS_H

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  
   This header file defines the interface to all S_BITINT operation utliities.
  ===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CryptoLib/secrsa/secrsa_utils.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  04/11/11   qxu     initial version
=============================================================================*/

#include <stdlib.h>
#include <string.h>
#include "crypto/crypto_env_comdef.h"
#include "crypto/secmath.h"

/*RSA key data structure with sign.
  sign is needed when the second form of private key
  is provided */
typedef struct S_BIGINT {
    BigInt bi;
	int sign;
} S_BIGINT;

/*sign*/
#define S_BIGINT_POS  0   /* positive */
#define S_BIGINT_NEG  1   /* negative */

/* equalities */
#define S_BIGINT_LT  -1   /* less than */
#define S_BIGINT_EQ   0   /* equal to */
#define S_BIGINT_GT   1   /* greater than */

/**
  initialize S_BIGINT data
  @param[in]  s         S_BIGINT data

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NO_MEMORY - Out of memory. \n

  @dependencies
  None. 
*/
int ce_util_init_s_bigint(S_BIGINT **a); 

/**
  delete S_BIGINT data
  @param[in]  s         S_BIGINT data

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NO_MEMORY - Out of memory. \n

  @dependencies
  None. 
*/
void ce_util_free_s_bigint(S_BIGINT *a);

/**
  count total bits numbers in S_BIGINT BLONG array
  @param[in]  s         S_BIGINT data

  @return 
   total bits numbers
   
  @dependencies
  None. 
*/
int ce_util_count_bits (S_BIGINT * bi);

/**
  count total byte numbers in S_BIGINT BLONG array
  @param[in]  s         S_BIGINT data

  @return 
   total byte numbers
   
  @dependencies
  None. 
*/
int ce_util_count_bytes (S_BIGINT * bi);

/**
  compare two S_BIGINT type data
  @param[in]  s1        pointer of s1
  @param[in]  s2        pointer of s2

  @return 
   S_BIGINT_GT - s1 > s2
   S_BIGINT_LT - s1 < s2
   S_BIGINT_EQ - s1 = s2

  @dependencies
  None. 
*/
int ce_util_cmp(S_BIGINT *s1, S_BIGINT *s2);

/**
  compare against a single digit
  @param[in]  a        pointer of a
  @param[in]  b        digit b

  @return 
   S_BIGINT_GT - a > b
   S_BIGINT_LT - a < b
   S_BIGINT_EQ - a = b

  @dependencies
  None. 
*/
int ce_util_cmp_d(S_BIGINT *a, BLONG b);

/**
  reverse an array, used for radix code
  @param[in]  s         pointer of the string to be reversed
  @param[in]  len       length of the string

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NO_MEMORY - Out of memory. \n

  @dependencies
  None. 
*/
void ce_util_reverse_str (unsigned char *s, int len);
/**
  convert octet string to S_BIGINT type data
  @param[out] a        S_BIGINT data a
  @param[in]  b        byte string which hold the data to be converted
  @param[in]  c        byte length

  @return 
   CE_SUCCESS     - Function executes successfully. 

  @dependencies
  None. 
*/
int ce_util_read_unsigned_bin(S_BIGINT *a, unsigned char *b, int c);

/**
  convert S_BIGINT type data to octet string
  @param[in]  a        S_BIGINT data a
  @param[out] b        byte string which hold the converted data
  @param[out] c        pointer to string length

  @return 
   CE_SUCCESS     - Function executes successfully. 
   CE_ERROR_NO_MEMORY - input string has no enough memory.

  @dependencies
  None. 
*/
int ce_util_to_unsigned_bin(S_BIGINT *a, unsigned char *b, int *c);

/**
  replace all zero's in the buffer with random data
  @param[in/out] buff       i/p buffer
  @param[in]     buff_len   buffer length

  @return 

  @dependencies
  None. 
*/
void ce_util_replace_zeros(uint8 *buff, uint32 buff_len);
#endif

