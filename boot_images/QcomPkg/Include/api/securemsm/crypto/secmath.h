#ifndef SECMATH_H
#define SECMATH_H

/**
  @file secmath.h

  @brief This header file defines the interface to all crypto math
         operations.

   The approach to crypto math operations can be broken into two types: provide
   big integer operations and let the caller construct the operations they need
   from such primitives, or make a crypto math library that is tuned for the type
   of operations that the crypto library is performing. This library is performing
   the latter
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/securemsm/crypto/secmath.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/11/11   vg             Initial Revision

===========================================================================*/

#include "crypto_env_comdef.h"


/**
 * @addtogroup SecMath
 * @{
 */
#ifdef __cplusplus
extern "C"
{
#endif

/* version */
#define E_SECMATH   0x0001
#define S_SECMATH   "0.01"

/* error codes */
typedef enum
{
  E_SECMATH_SUCCESS = 0,         ///< Result OK
  E_SECMATH_FAILURE,             ///< Generic Error */
  E_SECMATH_NOP,                 ///< Not a failure but no operation was performed */
  E_SECMATH_FAIL_TESTVECTOR,     ///< Algorithm failed test vectors */
  E_SECMATH_BUFFER_OVERFLOW,     ///< Not enough space for output */
  E_SECMATH_MEM,                 ///< Out of memory */
  E_SECMATH_INVALID_ARG,         ///< Generic invalid argument */
  E_SECMATH_INVALID_PRIME_SIZE,  ///< Invalid size of prime requested */
  E_SECMATH_NOT_SUPPORTED        ///< Operation not supported
} SECMATH_ERRNO_ET;

/** Common input/output type */
typedef struct common_in_out_s
{
  uint8 *ptr;
  uint32 len;
} common_in_out_t;


typedef uint32 BLONG;
#define MAX_KEY_SIZE 4128                          ///< Maximum key size in bits
#define BLONG_SIZE sizeof(BLONG)                   ///< Bytes per digit
#define BN_BITS_PER_WORD (BLONG_SIZE * 8)          ///< Bit per digit
#define BLONGS_PER_KEY  (MAX_KEY_SIZE + 8*BLONG_SIZE - 1)/(8*BLONG_SIZE)  ///< Digits per key
#define BYTES_TO_BLONG_SHIFT 2                     ///< bytes/sizeof(BLONG)

#define BLONG_MAX_ELEMENTS BLONGS_PER_KEY

#define NUMBER_OF_RANDOM_WORDS		3  // n. of 32 bit words for the random values 96 bits /32 (size of bit word); // number of random 32 bit words
#define WORD_SIZE_32 BLONG_SIZE
#define MASKED_LENGTH (WORD_SIZE_32*NUMBER_OF_RANDOM_WORDS) // 96 bits (masked by multiplication)
#define MASKED_MAX_ELEMENT (BLONG_MAX_ELEMENTS+MASKED_LENGTH) // ELEMENT:= message or key

typedef struct
{
  BLONG a[BLONGS_PER_KEY];             ///< Make room for multiplication
  int n;                                         ///< Current length of a
} BigInt;

// Redefining basic number to optimize memory consumption
typedef struct
{
  BLONG *a;                            //< Pointer to the value
  int n;                               //< Current length of a
} BigNum;

/**
 * @brief
 *  Read an unsigned buffer of bytes into a big integer
 */
SECMATH_ERRNO_ET secmath_BIGINT_read_unsigned_bin(BigInt * a, const uint8 * buf,
                                                  uint32 len);

 /**
 * @brief
 *  Read a zero terminated string into a big integer
 */
SECMATH_ERRNO_ET secmath_BIGINT_read_radix(BigInt * a, const char *str,
                                           uint32 radix);

/**
 * @brief
 *  Write a big integer into NULL terminate string
 */
SECMATH_ERRNO_ET secmath_BIGINT_to_radix(const BigInt * a, char *str, uint32 *len,
                                         uint32 radix);

/**
 * @brief
 *  Write a big integer into a buffer
 */
SECMATH_ERRNO_ET secmath_BIGINT_to_unsigned_bin(const BigInt * a, uint8 * buf,
                                                uint32 size);

/**
 * @brief
 *  r = b^e mod m
 */
SECMATH_ERRNO_ET secmath_BIGINT_modexp(BigInt * r, BigInt * b,
                                       BigInt * e, BigInt * m);


SECMATH_ERRNO_ET secmath_BIGINT_modexp_cm(BigInt * r,
		                           BigInt * b,
                                           BigInt * e,
					   BigInt * m);

/**
  @brief Implements the Miller-Rabin primality test, as described in the
  Handbook of Applied Cryptography, section 4.24

  Currently, NIST recommends that when testing a candidate prime for use in
  DSA and RSA signature schemes.

  When testing the RSA p and q primes, 8 rounds of Miller-Rabin (M-R) are
  performed followed by an optional single Lucas test.  For the smaller primes
  p1, p2, q1, and  2, that are used as part of the assurance that the p and q
  primes are strong, 50 Miller-Rabin tests are required.  These recommendations
  are in accordance with the American National Standard (ANS) X9.31 standard.

  For a detailed value of choosen t please read FIPS 186-3 DSS.

  In Summary minimal t values provided below:
  RSA 1024 Pr[Error]=1/2^80 then t = 5 for p,q and t = 28 for p1, p2, q1, q2
  RSA 2048 Pr[Error]=1/2^112 then t = 5 for p,q and t = 38 for p1, p2, q1, q2

  @param n      The number to test for primality
  @param t      The number of rounds to test
  @oaram  prime Set to TRUE if the number under test is prime; otherwise FALSE
*/

SECMATH_ERRNO_ET secmath_BIGINT_is_prime(const BigInt * n, uint32 t, boolean *prime);

SECMATH_ERRNO_ET secmath_BIGINT_prime_test(BigInt *n, uint32 t, int *result);

#ifdef __cplusplus
}
#endif

/// @}

#endif
