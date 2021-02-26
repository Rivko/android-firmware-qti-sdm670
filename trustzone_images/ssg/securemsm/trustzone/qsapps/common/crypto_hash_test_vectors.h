/*
@file crypto_hash_test_vectors.h
@brief This file contains all common definitions used in apt crypto test app
*/
/*===========================================================================
   Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/common/crypto_hash_test_vectors.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
11/22/2017  jp       added SHA384 test case
8/19/2015   vinitp   created a header file for testing HASH APIs
===========================================================================*/
#ifndef __CRYPTO_HASH_TEST_VECTORS_H__
#define __CRYPTO_HASH_TEST_VECTORS_H__

#include <comdef.h>
#include "qsee_cipher.h"
#include "qsee_hash.h"
#include "qsee_uf_sha.h"
#include "crypto_common_test_vectors.h"

/* SHA 1 test vectors */
static UINT8_A sha_plain_text_1[] =
{
   "Test vector from febooti.com"
};

static UINT8_A sha1_digest_text_1[] =
{
   0xa7, 0x63, 0x17, 0x95, 0xf6, 0xd5, 0x9c, 0xd6,
   0xd1, 0x4e, 0xbd, 0x00, 0x58, 0xa6, 0x39, 0x4a,
   0x4b, 0x93, 0xd8, 0x68
};

/* SHA 256 test vectors */
static UINT8_A sha256_digest_text_1[] =
{
   0x07, 0x7b, 0x18, 0xfe, 0x29, 0x03, 0x6a, 0xda,
   0x48, 0x90, 0xbd, 0xec, 0x19, 0x21, 0x86, 0xe1,
   0x06, 0x78, 0x59, 0x7a, 0x67, 0x88, 0x02, 0x90,
   0x52, 0x1d, 0xf7, 0x0d, 0xf4, 0xba, 0xc9, 0xab
};

/* SHA 384 test vectors */
static UINT8_A sha384_digest_text_1[] = {
    0x38, 0x8b, 0xb2, 0xd4, 0x87, 0xde, 0x48, 0x74,
    0x0f, 0x45, 0xfc, 0xb4, 0x41, 0x52, 0xb0, 0xb6,
    0x65, 0x42, 0x8c, 0x49, 0xde, 0xf1, 0xaa, 0xf7,
    0xc7, 0xf0, 0x9a, 0x40, 0xc1, 0x0a, 0xff, 0x1c,
    0xd7, 0xc3, 0xfe, 0x33, 0x25, 0x19, 0x3c, 0x4d,
    0xd3, 0x5d, 0x4e, 0xaa, 0x03, 0x2f, 0x49, 0xb0
};

/* MD5 test vectors */
static UINT8_A md5_plain_text_1[] =
{
		"The quick brown fox jumps over the lazy dog"
};

static UINT8_A md5_digest_text_1[] =
{
  0x9e, 0x10, 0x7d, 0x9d, 0x37, 0x2b, 0xb6, 0x82,
  0x6b, 0xd8, 0x1d, 0x35, 0x42, 0xa4, 0x19, 0xd6
};

static UINT8_A md5_plain_text_2[] =
{
		"The quick brown fox jumps over the lazy dog."
};

static UINT8_A md5_digest_text_2[] =
{
  0xe4, 0xd9, 0x09, 0xc2, 0x90, 0xd0, 0xfb, 0x1c,
  0xa0, 0x68, 0xff, 0xad, 0xdf, 0x22, 0xcb, 0xd0
};

static UINT8_A md5_plain_text_3[] =
{
		""
};

static UINT8_A md5_digest_text_3[] =
{
  0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
  0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e,
};


typedef struct sha_vector_type
{
   QSEE_HASH_ALGO_ET  alg;
   uint8*             msg;
   uint32             msg_len;
   uint8*             digest;
} __attribute__ ((packed)) sha_vector_type_t;

/* SHA test vectors array */
static sha_vector_type_t sha_test_vectors[] =
{
   {QSEE_HASH_SHA1, sha_plain_text_1,
      sizeof(sha_plain_text_1) - 1, sha1_digest_text_1},
   {QSEE_HASH_SHA256, sha_plain_text_1,
      sizeof(sha_plain_text_1) - 1, sha256_digest_text_1},
   {QSEE_HASH_SHA384, sha_plain_text_1,
      sizeof(sha_plain_text_1) - 1, sha384_digest_text_1},
};

typedef struct uf_sha_vector_type
{
   SW_Auth_Alg_Type   alg;
   uint8*             msg;
   uint32             msg_len;
   uint8*             digest;
} __attribute__ ((packed)) uf_sha_vector_type_t;

/* SHA test vectors array */
static uf_sha_vector_type_t uf_sha_test_vectors[] =
{
   {SW_AUTH_ALG_SHA1, sha_plain_text_1,
      sizeof(sha_plain_text_1) - 1, sha1_digest_text_1},
   {SW_AUTH_ALG_SHA256, sha_plain_text_1,
      sizeof(sha_plain_text_1) - 1, sha256_digest_text_1},
   {SW_AUTH_ALG_MD5, md5_plain_text_1,
      sizeof(md5_plain_text_1) - 1, md5_digest_text_1},
   {SW_AUTH_ALG_MD5, md5_plain_text_2,
      sizeof(md5_plain_text_2) - 1, md5_digest_text_2},
   {SW_AUTH_ALG_MD5, md5_plain_text_3,
      0, md5_digest_text_3},
};

/* Simultaneous HASH/AES CBC test vectors */
static UINT8_A hash_aes_key_128[] =
{
   0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static UINT8_A hash_aes_key_256[] =
{
   0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
   0x3c, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x2b
};

static UINT8_A hash_aes_iv_1[] =
{
   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

typedef struct
{
   QSEE_HASH_ALGO_ET     hash_algo;
   QSEE_CIPHER_ALGO_ET   cipher_algo;
   QSEE_CIPHER_MODE_ET   cipher_mode;
   uint8*                pt;
   uint32                pt_len;
   uint8*                key;
   uint32                key_len;
   uint8*                iv;
   uint8*                ct;
   uint8*                ct2;
} __attribute__ ((packed)) hash_aes_vector_type;


/* Simultaneous Hash/AES AES-128/256 and SHA1/SHA256 test vectors array */
static hash_aes_vector_type hash_aes_test_vectors[] =
{
   {QSEE_HASH_SHA1, QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CTS,
      aes_large_plain_text2, sizeof(aes_large_plain_text2),
      hash_aes_key_128, sizeof(hash_aes_key_128),
      hash_aes_iv_1, aes_large_cipher_text2, aes_large_cipher_text3},

   {QSEE_HASH_SHA1, QSEE_CIPHER_ALGO_AES_128, QSEE_CIPHER_MODE_CBC,
      aes_large_plain_text2, sizeof(aes_large_plain_text2),
      hash_aes_key_128, sizeof(hash_aes_key_128),
      hash_aes_iv_1, aes_large_cipher_text2, aes_large_cipher_text3},

   {QSEE_HASH_SHA1, QSEE_CIPHER_ALGO_AES_256, QSEE_CIPHER_MODE_CBC,
      aes_large_plain_text2, sizeof(aes_large_plain_text2),
      hash_aes_key_256, sizeof(hash_aes_key_256),
      hash_aes_iv_1, aes_large_cipher_text2, aes_large_cipher_text3},

   {QSEE_HASH_SHA256, QSEE_CIPHER_ALGO_AES_128,QSEE_CIPHER_MODE_CBC,
      aes_large_plain_text2, sizeof(aes_large_plain_text2),
      hash_aes_key_128, sizeof(hash_aes_key_128),
      hash_aes_iv_1, aes_large_cipher_text2, aes_large_cipher_text3},

   {QSEE_HASH_SHA256, QSEE_CIPHER_ALGO_AES_256, QSEE_CIPHER_MODE_CBC,
      aes_large_plain_text2, sizeof(aes_large_plain_text2),
      hash_aes_key_256, sizeof(hash_aes_key_256),
      hash_aes_iv_1, aes_large_cipher_text2, aes_large_cipher_text3},
};

#endif // __CRYPTO_HASH_TEST_VECTORS_H__

