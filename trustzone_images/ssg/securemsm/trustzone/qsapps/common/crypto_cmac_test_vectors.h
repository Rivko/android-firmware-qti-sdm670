/*
@file crypto_cmac_test_vectors.h
@brief This file contains all common definitions used in apt crypto test app
*/
/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/common/crypto_cmac_test_vectors.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
8/19/2015   vinitp   created a new common header file
===========================================================================*/
#ifndef __CRYPTO_CMAC_TEST_VECTORS_H__
#define __CRYPTO_CMAC_TEST_VECTORS_H__

#include <comdef.h>
#include "crypto_common_test_vectors.h"

// CMAC vectors
// AES 128 vectors
static UINT8_A aescmac128_key_1[] =
{
   0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
   0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static UINT8_A aescmac128_key_2[] =
{
   0x36, 0x5d, 0xf1, 0x49, 0x77, 0xf5, 0x56, 0xd6,
   0xdd, 0xe6, 0x5f, 0x66, 0x70, 0xa3, 0x05, 0x18
};

// Mlen =16
static UINT8_A aescmac128_plain_text_1[] =
{
   0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
   0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

// Mlen =40
static UINT8_A aescmac128_plain_text_2[] =
{
   0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
   0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
   0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
   0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
   0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11
};

// Mlen =64
uint8 aescmac128_plain_text_3[] =
{
   0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
   0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
   0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
   0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
   0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
   0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
   0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
   0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};

// Mlen =32
uint8 aescmac128_plain_text_4[] =
{
   0x1b, 0x4e, 0x4c, 0x7f, 0x91, 0xc6, 0x98, 0xbf,
   0xa7, 0x47, 0x0f, 0xad, 0x1d, 0x60, 0x92, 0xfc,
   0xef, 0xed, 0x1d, 0x02, 0x2a, 0x9a, 0x41, 0x5e,
   0xff, 0xa2, 0xd4, 0xdd, 0x16, 0xdd, 0xe7, 0x9f
};

static UINT8_A aescmac128_cmac_text_0[] =
{
   0xbb, 0x1d, 0x69, 0x29, 0xe9, 0x59, 0x37, 0x28,
   0x7f, 0xa3, 0x7d, 0x12, 0x9b, 0x75, 0x67, 0x46
};

static UINT8_A aescmac128_cmac_text_1[] =
{
   0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44,
   0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c
};

static UINT8_A aescmac128_cmac_text_2[] =
{
   0xdf, 0xa6, 0x67, 0x47, 0xde, 0x9a, 0xe6, 0x30,
   0x30, 0xca, 0x32, 0x61, 0x14, 0x97, 0xc8, 0x27
};

static UINT8_A aescmac128_cmac_text_3[] =
{
   0x51, 0xf0, 0xbe, 0xbf, 0x7e, 0x3b, 0x9d, 0x92,
   0xfc, 0x49, 0x74, 0x17, 0x79, 0x36, 0x3c, 0xfe
};

static UINT8_A aescmac128_cmac_text_4[] =
{
   0x64, 0x4e, 0xc8, 0x02, 0x28, 0xc9, 0xc6, 0x43
};

// AES 256 vectors
static UINT8_A aescmac256_key_1[] =
{
   0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
   0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
   0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
   0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};

static UINT8_A aescmac256_cmac_text_1[] =
{
   0x28, 0xa7, 0x02, 0x3f, 0x45, 0x2e, 0x8f, 0x82,
   0xbd, 0x4b, 0xf2, 0x8d, 0x8c, 0x37, 0xc3, 0x5c
};

static UINT8_A aescmac256_cmac_text_2[] =
{
   0xaa, 0xf3, 0xd8, 0xf1, 0xde, 0x56, 0x40, 0xc2,
   0x32, 0xf5, 0xb1, 0x69, 0xb9, 0xc9, 0x11, 0xe6
};

static UINT8_A aescmac256_cmac_text_3[] =
{
   0xe1, 0x99, 0x21, 0x90, 0x54, 0x9f, 0x6e, 0xd5,
   0x69, 0x6a, 0x2c, 0x05, 0x6c, 0x31, 0x54, 0x10
};

typedef struct
{
   uint8 *msg;
   uint32 msg_len;
   uint8 *key;
   uint32 key_len;
   uint8 *cmac;
   uint32 cmac_len;
} __attribute__ ((packed)) aes_cmac_vector_t;

static aes_cmac_vector_t aes_cmac_test_vectors[] =
{
   {aescmac128_plain_text_1, sizeof(aescmac128_plain_text_1), aescmac128_key_1,   //AES-CMAC128
      16, aescmac128_cmac_text_1, sizeof(aescmac128_cmac_text_1)},
   {aescmac128_plain_text_2, sizeof(aescmac128_plain_text_2), aescmac128_key_1,   //AES-CMAC128
      16, aescmac128_cmac_text_2, sizeof(aescmac128_cmac_text_2)},
   {aescmac128_plain_text_3, sizeof(aescmac128_plain_text_3), aescmac128_key_1,   //AES-CMAC128
      16, aescmac128_cmac_text_3, sizeof(aescmac128_cmac_text_3)},
   {aescmac128_plain_text_4, sizeof(aescmac128_plain_text_4), aescmac128_key_2,   //AES-CMAC128
      16, aescmac128_cmac_text_4, sizeof(aescmac128_cmac_text_4)},
   {aescmac128_plain_text_1, sizeof(aescmac128_plain_text_1), aescmac256_key_1,   //AES-CMAC256
      32, aescmac256_cmac_text_1, sizeof(aescmac256_cmac_text_1)},
   {aescmac128_plain_text_2, sizeof(aescmac128_plain_text_2), aescmac256_key_1,   //AES-CMAC256
      32, aescmac256_cmac_text_2, sizeof(aescmac256_cmac_text_2)},
   {aescmac128_plain_text_3, sizeof(aescmac128_plain_text_3), aescmac256_key_1,   //AES-CMAC256
      32, aescmac256_cmac_text_3, sizeof(aescmac256_cmac_text_3)}
};

#endif // __CRYPTO_CMAC_TEST_VECTORS_H__
