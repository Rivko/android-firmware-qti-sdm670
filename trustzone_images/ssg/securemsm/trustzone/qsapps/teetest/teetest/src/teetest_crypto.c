
/*===========================================================================
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/
#include "teetest.h"

#include "IEnv.h"
#include "CEnv_open.h"
#include "ICrypto.h"
#include "CCrypto.h"
#include "IHmac.h"
#include "CHmac.h"
#include "IHash.h"
#include "CHash.h"

#include <string.h>

/* TEST VECTORS */

/* KDF */
static uint8_t kdf_key_in [] = {
  0x12,0x23,0x56,0x73,
  0xA3,0xff,0xEf,0xAB,
  0xCD,0x1F,0x56,0xEF,
  0xA3,0xff,0xEf,0xAB,
  0xCD,0x1F,0x56,0xEF,
  0x12,0x23,0x56,0x73,
  0xCD,0x1F,0x56,0xEF,
  0xCD,0x1F,0x56,0xEF
};

#define KDF_OUT_LEN 16

/* CMAC */
static uint8_t cmac_key [] = {
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
  0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static uint8_t cmac_plain_txt [] = {
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

static uint8_t cmac_cmac_txt [] = {
  0xbb, 0x1d, 0x69, 0x29, 0xe9, 0x59, 0x37, 0x28,
  0x7f, 0xa3, 0x7d, 0x12, 0x9b, 0x75, 0x67, 0x46
};

#define CMAC_DIGEST_LEN 16

/* HMAC */
static uint8_t hmac_msg [] = { 0x5f, 0xd4 };

static uint8_t hmac_key [] = {
  0x4a, 0x65, 0x66, 0x65
};

static uint8_t hmac_msg_hmac256 [] = {
  0xA0, 0xCC, 0x63, 0x97, 0x95, 0x25, 0x59, 0xB6,
  0x0E, 0x31, 0x83, 0x00, 0xD3, 0x94, 0x69, 0x85,
  0xCD, 0xD4, 0x5A, 0xA9, 0xCF, 0xF8, 0xAB, 0x0D,
  0xA2, 0xA5, 0x05, 0x71, 0x07, 0xC8, 0x08, 0x9C
};

#define HMAC_DIGEST_LEN 32

/* HASH */

static uint8_t hash_msg [] = {
  "Test vector from febooti.com"
};

/* SHA 256 test vectors */
static uint8_t hash_msg_sha256 [] = 
{
  0x07, 0x7b, 0x18, 0xfe, 0x29, 0x03, 0x6a, 0xda,
  0x48, 0x90, 0xbd, 0xec, 0x19, 0x21, 0x86, 0xe1,
  0x06, 0x78, 0x59, 0x7a, 0x67, 0x88, 0x02, 0x90,
  0x52, 0x1d, 0xf7, 0x0d, 0xf4, 0xba, 0xc9, 0xab
};

#define HASH_DIGEST_LEN 32


/**
 * Test the KDF MINK API
 * 1. Twice with the same inputs using the hardware key and compare the 2
 *    results
 * 2. Twice with the same inputs using a provided key and compare the 2 results
 */
static int tee_test_crypto_kdf(Object crypto)
{
  uint8_t nullkey;
  static char key_label[] = {"I am deriving a key"};
  static char key_context[] = {"To verify the kdf working"};
  uint8_t  kdf_out [KDF_OUT_LEN]   = {0};
  uint8_t  kdf_out_2 [KDF_OUT_LEN] = {0};
  size_t   kdf_out_len;
  uint8_t *key_input     = kdf_key_in;
  size_t   key_input_len = sizeof(kdf_key_in);

  TEST_ENTER;

  /* 1. TEST WITH THE HARDWARE KEY */
  TEST_OK(ICrypto_kdf(crypto,
        (void*)&nullkey,      0,
        (void*)(key_label),   strlen(key_label),
        (void*)(key_context), strlen(key_context),
        (void*)kdf_out,       sizeof(kdf_out),
        &kdf_out_len));

  TEST_OK(ICrypto_kdf(crypto,
        (void*)&nullkey,      0,
        (void*)(key_label),   strlen(key_label),
        (void*)(key_context), strlen(key_context),
        (void*)kdf_out_2,     sizeof(kdf_out_2),
        &kdf_out_len));

  TEST_TRUE(memcmp(kdf_out, kdf_out_2, sizeof(kdf_out)) == 0);

  /* 2. TEST WITH A USER PROVIDED KEY */
  TEST_OK(ICrypto_kdf(crypto,
        (void*)key_input,     key_input_len,
        (void*)(key_label),   strlen(key_label),
        (void*)(key_context), strlen(key_context),
        (void*)(kdf_out),     sizeof(kdf_out),
        &kdf_out_len));

  TEST_OK(ICrypto_kdf(crypto,
        (void*)key_input,     key_input_len,
        (void*)(key_label),   strlen(key_label),
        (void*)(key_context), strlen(key_context),
        (void*)kdf_out_2,     sizeof(kdf_out_2),
        &kdf_out_len));

  TEST_TRUE(memcmp(kdf_out, kdf_out_2, sizeof(kdf_out)) == 0);

  TEST_EXIT;
}

/**
 * Test the CMAC MINK API
 */ 
static int tee_test_crypto_cmac(Object crypto)
{
  uint8_t *msg = cmac_plain_txt;
  size_t msg_len = sizeof(cmac_plain_txt);
  uint8_t *key = cmac_key;
  size_t key_len = sizeof(cmac_key);
  uint8_t digest [CMAC_DIGEST_LEN];
  size_t digest_len;
  TEST_ENTER;

  memset(digest, 0, sizeof(digest));

  TEST_OK(ICrypto_cmac(crypto,
        ICrypto_CMAC_ALGO_AES_128,
        (void*)(msg),     msg_len,
        (void*)(key),     key_len,
        (void*)(digest),  sizeof(digest),
        &digest_len));

  TEST_TRUE(memcmp(digest, cmac_cmac_txt, sizeof(digest)));

  TEST_EXIT;
}

/**
 * Test the HMAC MINK API
 */
static int tee_test_hmac(Object hmac)
{
  TEST_ENTER;

  uint8_t *msg = hmac_msg;
  size_t msg_len = sizeof(hmac_msg);
  uint8_t *key = hmac_key;
  size_t key_len = sizeof(hmac_key);
  uint8_t digest [HMAC_DIGEST_LEN];
  size_t digest_len;
  
  memset(digest, 0, sizeof(digest));

  TEST_OK(IHmac_setParamAsData(hmac, IHmac_PARAM_KEY, key, key_len));

  TEST_OK(IHmac_update(hmac, msg, msg_len));

  TEST_OK(IHmac_final(hmac, digest, sizeof(digest), &digest_len));

  TEST_TRUE(memcmp(digest, hmac_msg_hmac256, sizeof(digest)) == 0); 

  TEST_EXIT;
}

/**
 * Test the HASH MINK API
 */
static int tee_test_hash(Object hash)
{
  TEST_ENTER;

  uint8_t *msg = hash_msg;
  size_t   msg_len = sizeof(hash_msg);
  uint8_t  digest[HASH_DIGEST_LEN] = {0};
  size_t   digest_len;

  TEST_OK(IHash_update(hash, msg, msg_len));

  TEST_OK(IHash_final(hash, digest, sizeof(digest), &digest_len));

  TEST_TRUE(memcmp(digest, hash_msg_sha256, sizeof(digest)));

  TEST_EXIT;
}

/**
 * Main Crypto test function
 */
int tee_test_crypto(void)
{
  Object TEE_Outbound_Object;
  
  TEST_ENTER;

  /* Tests with the Crypto MINK Interface */
  TEST_IOPEN(Crypto, TEE_Outbound_Object);

  /* TEST the KDF MINK API */
  TEST_OK(tee_test_crypto_kdf(TEE_Outbound_Object));

  /* TEST the CMAC MINK API */
  TEST_OK(tee_test_crypto_cmac(TEE_Outbound_Object));

  TEST_ICLOSE(Crypto, TEE_Outbound_Object);

  /* TEST the Hmac MINK Interface */
  TEST_IOPEN(HmacSHA256, TEE_Outbound_Object);

  TEST_OK(tee_test_hmac(TEE_Outbound_Object));

  TEST_ICLOSE(Hmac, TEE_Outbound_Object);

  /* TEST the Hash MINK Interface */
  TEST_IOPEN(HashSHA256, TEE_Outbound_Object);

  TEST_OK(tee_test_hash(TEE_Outbound_Object));

  TEST_ICLOSE(Hash, TEE_Outbound_Object);

  TEST_EXIT;
}
