#ifndef SHA_CORE_H
#define SHA_CORE_H

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CryptoLib/sha/sha_core.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/12   yk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "crypto_env_comdef.h"

#define SW_SHA1_DIGEST_SIZE             20
#define SW_SHA256_DIGEST_SIZE             32
#define SW_SHA_BLOCK_SIZE               64


typedef enum
{
  SW_CORE_AUTH_ALG_SHA1               = 0x1,
  SW_CORE_AUTH_ALG_SHA256             = 0x2
} HashAlgoType;

struct __sechsh_ctx_s
{
   uint32  iv[16];  // is 64 byte for SHA2-512
   uint32  counter[2];
   HashAlgoType auth_alg;
   uint32  auth_iv[8]; 
   uint8   leftover[SW_SHA_BLOCK_SIZE];
   uint32  leftover_size;
};


typedef struct __sechsh_ctx_s hashctxt;

void sechsharm_sha1_init(   struct __sechsh_ctx_s* );
void sechsharm_sha1_update( struct __sechsh_ctx_s*, uint8*, uint32*, uint8*, uint32 );
void sechsharm_sha1_final(  struct __sechsh_ctx_s*, uint8*, uint32*, uint8* );
void sechsharm_sha1_transform( uint32*, unsigned char* );
#define SHA256_BLOCK_LENGTH    64
void sechsharm_sha256_init(   struct __sechsh_ctx_s* );
void sechsharm_sha256_update( struct __sechsh_ctx_s*, uint8*, uint32*, uint8*, uint32 );
void sechsharm_sha256_final(  struct __sechsh_ctx_s*, uint8*, uint32*, uint8* );
static void sechsharm_sha256_transform( uint32 state[8], const uint8 data[SHA256_BLOCK_LENGTH]);



/*** ENDIAN SPECIFIC COPY MACROS **************************************/
#define BE_8_TO_32(dst, cp) do {				\
      (dst) = (uint32)(cp)[3] | ((uint32)(cp)[2] << 8) |	\
	 ((uint32)(cp)[1] << 16) | ((uint32)(cp)[0] << 24);	\
   } while(0)

#define BE_8_TO_64(dst, cp) do {					\
      (dst) = (uint64_t)(cp)[7] | ((uint64_t)(cp)[6] << 8) |		\
	 ((uint64_t)(cp)[5] << 16) | ((uint64_t)(cp)[4] << 24) |	\
	 ((uint64_t)(cp)[3] << 32) | ((uint64_t)(cp)[2] << 40) |	\
	 ((uint64_t)(cp)[1] << 48) | ((uint64_t)(cp)[0] << 56);		\
   } while (0)

#define BE_64_TO_8(cp, src) do {		\
      (cp)[0] = (uint8)((src) >> 56);		\
      (cp)[1] = (uint8)((src) >> 48);		\
      (cp)[2] = (uint8)((src) >> 40);		\
      (cp)[3] = (uint8)((src) >> 32);		\
      (cp)[4] = (uint8)((src) >> 24);		\
      (cp)[5] = (uint8)((src) >> 16);		\
      (cp)[6] = (uint8)((src) >> 8);		\
      (cp)[7] = (uint8)((src));			\
   } while (0)

#define BE_32_TO_8(cp, src) do {		\
      (cp)[0] = (uint8)((src) >> 24);		\
      (cp)[1] = (uint8)((src) >> 16);		\
      (cp)[2] = (uint8)((src) >> 8);		\
      (cp)[3] = (uint8)((src));			\
   } while (0)

#define R(b,x) 		((x) >> (b))
/* 32-bit Rotate-right (used in SHA-256): */
#define SHA_S32(b,x)	(((x) >> (b)) | ((x) << (32 - (b))))
/* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define SHA_S64(b,x)	(((x) >> (b)) | ((x) << (64 - (b))))

/* Two of six logical functions used in SHA-256, SHA-384, and SHA-512: */
#define Ch(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))


/* Four of six logical functions used in SHA-256: */
#define Sigma0_256(x)	(SHA_S32(2,  (x)) ^ SHA_S32(13, (x)) ^ SHA_S32(22, (x)))
#define Sigma1_256(x)	(SHA_S32(6,  (x)) ^ SHA_S32(11, (x)) ^ SHA_S32(25, (x)))
#define sigma0_256(x)	(SHA_S32(7,  (x)) ^ SHA_S32(18, (x)) ^ R(3 ,   (x)))
#define sigma1_256(x)	(SHA_S32(17, (x)) ^ SHA_S32(19, (x)) ^ R(10,   (x)))

static void sechsharm_sha256_transform
(
  uint32 state[8], 
  const uint8 data[SHA256_BLOCK_LENGTH]
);
/* Initial hash value H for SHA-256: */
static const uint32 sha256_initial_hash_value[8] = {
   0x6a09e667UL,
   0xbb67ae85UL,
   0x3c6ef372UL,
   0xa54ff53aUL,
   0x510e527fUL,
   0x9b05688cUL,
   0x1f83d9abUL,
   0x5be0cd19UL
};
#define HSHSHA_COUNTER_SIZE 8
/*** SHA-256 Various Length Definitions ***********************/
#define SHA256_BLOCK_LENGTH    64
#define SHA256_SHORT_BLOCK_LENGTH	(SHA256_BLOCK_LENGTH - 8)
#define SECAPI_HSH_SHA_BLOCK_BYTE_LEN      64
/* Hash constant words K for SHA-256: */
static const uint32 K256[64] = {
   0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
   0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
   0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
   0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
   0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
   0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
   0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
   0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
   0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
   0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
   0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
   0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
   0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
   0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
   0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
   0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

#endif // SHA_CORE_H
