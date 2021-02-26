/*===========================================================================
            Unified Image Encryption (UIE) Crypto Engine Interface

GENERAL DESCRIPTION
 Provide an interface for cryptodriver configuration for UIE algorithms 

Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                                 INCLUDE FILES
===========================================================================*/

#include <stdbool.h>
//#include <stddef.h>
#include <string.h>
#include <comdef.h>
#include "IxErrno.h"
#include "CeML.h"
#include "UIE.h"
#include "UIEPriv.h"
#include "UIEEnv.h"

/*===========================================================================
                          MACROS AND TYPE DECLARATIONS
===========================================================================*/

#define BREAKIF(cond) if (cond) break
#define RESIDUE_POWER_2(addr, size) ((addr) & ((size) - 1))
#define ROUND_UP_POWER_2(addr, size)                  \
  RESIDUE_POWER_2(addr, size)                         \
    ? ((addr) + (size) - RESIDUE_POWER_2(addr, size)) \
    : (addr)
#define MAX_ADATA_LEN_ENCODING_LENGTH 6 // Excluding the 10 octet case

/*===========================================================================
                   UIE CRYPTOGRAPHIC FUNCTION IMPLEMENTATIONS
===========================================================================*/

static uint32 format_adata_len(uint32 adata_len, uint8 *buffer_adata_len)
{
  uint32 formatted_len;

  /* Add associated data length formatting
   * NOTE: in order to maintain compatibility with the Crypto 5 driver for CCM,
   *       do not populate the adata length field with the actual length.
   *       Instead, fill in with dummy zero bytes.
   */
  if (adata_len == 0)
  {
    formatted_len = 0;
  }
  else if (adata_len < ((1 << 16) - (1 << 8))) // 0 < a < 2**16 - 2**8
  {
    formatted_len = 2;
    buffer_adata_len[0] = 0;
    buffer_adata_len[1] = 0;
  }
  else // a < 2**32, we don't worry about the case where adata_len is too big for a uint32
  {
    formatted_len = 6;
    buffer_adata_len[0] = 0;
    buffer_adata_len[1] = 0;
    buffer_adata_len[2] = 0;
    buffer_adata_len[3] = 0;
    buffer_adata_len[4] = 0;
    buffer_adata_len[5] = 0;
  }

  return formatted_len;
}

int uie_decrypt_ccm(uie_key_src_t key_src,
                         uint8 *key,     uint32 key_len,
                         uint8 *nonce,   uint32 nonce_len,
                         uint8 *payload, uint32 payload_len,
                         uint8 *adata,   uint32 adata_len,
                         uint8 *mac,     uint32 mac_len,
                         uint8 *decrypted_payload,
            uint32 decrypted_payload_len) 
{

  CeMLCipherModeType  mode = CEML_CIPHER_MODE_CCM;
  CeMLCipherDir       dir  = CEML_CIPHER_DECRYPT;
  CeMLCntxHandle*     cntx = NULL;
  CEMLIovecListType   ioVecIn;
  CEMLIovecListType   ioVecOut;
  CEMLIovecType       IovecIn;
  CEMLIovecType       IovecOut;
  uint8               *ct = NULL;
  uint32              ct_len;
  uint8               *pt = NULL;
  uint32              pt_len;
  uint32              adjusted_adata_len = adata_len;
  uint32              buffer_a_len;
  uint8               buffer_a[MAX_ADATA_LEN_ENCODING_LENGTH];
  CeMLErrorType       retval = CEML_ERROR_INVALID_PARAM;
  CeMLErrorType       retval_teardown;
  bool                crypto_init = false;

  do {
    BREAKIF(nonce == NULL || payload == NULL || adata == NULL ||
            mac == NULL || decrypted_payload == NULL ||
            key_src != UIE_KEY_SRC_PROVIDED);

    buffer_a_len = format_adata_len(adata_len, buffer_a);
    adjusted_adata_len += buffer_a_len;
    adjusted_adata_len = ROUND_UP_POWER_2(adjusted_adata_len, CEML_AES_BLOCK_SIZE);

    ct_len = adjusted_adata_len + payload_len + mac_len;
    pt_len = ct_len;

    // Setup
    pt = UIE_MALLOC(pt_len); 
    ct = UIE_MALLOC(ct_len);
    BREAKIF(pt == NULL || ct == NULL);
    memset(pt, 0, pt_len);
    memset(ct, 0, ct_len);
    UIE_MEMSCPY(ct, ct_len, buffer_a, buffer_a_len);
    UIE_MEMSCPY(ct + buffer_a_len, ct_len, adata, adata_len);
    UIE_MEMSCPY(ct + adjusted_adata_len, ct_len, payload, payload_len);
    UIE_MEMSCPY(ct + adjusted_adata_len + payload_len, ct_len, mac, mac_len);

    ioVecIn.size = 1;
    ioVecIn.iov = &IovecIn;
    ioVecIn.iov[0].dwLen = ct_len;
    ioVecIn.iov[0].pvBase = ct; 

    ioVecOut.size = 1;
    ioVecOut.iov = &IovecOut;
    ioVecOut.iov[0].dwLen = pt_len;
    ioVecOut.iov[0].pvBase = pt;

    retval = CeMLInit();
    crypto_init = true;
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    if (key_len == CEML_AES128_IV_SIZE)
    {
      retval = CeMLCipherInit(&cntx, CEML_CIPHER_ALG_AES128);
    }
    else
    {
      retval = CeMLCipherInit(&cntx, CEML_CIPHER_ALG_AES256);
    }
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_MODE,
                                 &mode, sizeof(CeMLCipherModeType));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_DIRECTION,
                                 &dir, sizeof(CeMLCipherDir));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_KEY,
                                 key, key_len);
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_CCM_HDR_LEN,
                                 &adata_len, sizeof(uint32));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_CCM_PAYLOAD_LEN,
                                 &payload_len, sizeof(uint32));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_CCM_MAC_LEN,
                                 &mac_len, sizeof(uint32));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_NONCE,
                                 nonce, nonce_len);
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    // Decryption
    retval = CeMLCipherData(cntx, ioVecIn, &ioVecOut);
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    UIE_MEMSCPY(decrypted_payload, decrypted_payload_len,
                pt + adjusted_adata_len, payload_len);

    retval = CEML_ERROR_SUCCESS;
  } while(0);

  // Teardown
  if (cntx != NULL)
  {
    retval_teardown = CeMLCipherDeInit(&cntx);
    if (retval == CEML_ERROR_SUCCESS)
    {
      retval = retval_teardown;
    }
  }
  if (crypto_init)
  {
    retval_teardown = CeMLDeInit();
    if (retval == CEML_ERROR_SUCCESS)
    {
      retval = retval_teardown;
    }
  }

  if (pt != NULL)
  {
    UIE_FREE(pt);
  }
  if (ct != NULL)
  {
    UIE_FREE(ct);
  }

  return retval;
}

int uie_decrypt_cts(uint8 *key, uint32 key_len,
                    uint8 *iv, uint32 iv_len,
                    uint8 *ciphertext, uint32 ciphertext_len,
                    uint8 *plaintext, uint32 plaintext_len,
                    uint8 *digest)
{
  CeMLCipherAlgType  algo = CEML_CIPHER_ALG_AES128;
  CeMLCipherModeType mode = CEML_CIPHER_MODE_CTS;
  CeMLCipherDir      dir  = CEML_CIPHER_DECRYPT;
  CeMLCntxHandle*    cntx = NULL;
  CeMLCntxHandle*    hash_cntx = NULL;
  CEMLIovecListType  ioVecIn;
  CEMLIovecListType  ioVecOut;
  CEMLIovecType      IovecIn;
  CEMLIovecType      IovecOut;
  CeMLErrorType      retval = CEML_ERROR_INVALID_PARAM;
  CeMLErrorType      retval_teardown;
  bool               crypto_init = false;

  do {
    // Parameter checks
    BREAKIF(key == NULL || iv == NULL ||
            ciphertext == NULL || plaintext == NULL);

    if (ciphertext_len == 0 && plaintext_len == 0)
    {
      retval = CEML_ERROR_SUCCESS;
      break;
    }

    // Setup
    if (key_len == CEML_AES256_KEY_SIZE)
      algo = CEML_CIPHER_ALG_AES256;

    ioVecIn.size = 1;
    ioVecIn.iov = &IovecIn;
    ioVecIn.iov[0].dwLen = ciphertext_len;
    ioVecIn.iov[0].pvBase = ciphertext; 

    ioVecOut.size = 1;
    ioVecOut.iov = &IovecOut;
    ioVecOut.iov[0].dwLen = plaintext_len;
    ioVecOut.iov[0].pvBase = plaintext;

    retval = CeMLInit();
    crypto_init = true;
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    // Cipher Configuration
    retval = CeMLCipherInit(&cntx, algo);
    BREAKIF(retval != CEML_ERROR_SUCCESS);
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_MODE,
                                 &mode, sizeof(mode));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_DIRECTION,
                                 &dir, sizeof(dir));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_IV, iv, iv_len);
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_KEY, key, key_len);
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    if (digest == NULL)
    {
      // Decryption
      retval = CeMLCipherData(cntx, ioVecIn, &ioVecOut);
      BREAKIF(retval != CEML_ERROR_SUCCESS);
    }
    else
    {
      // Hash Configuration
      CeMLHashAlgoType hash_algo = CEML_HASH_ALGO_SHA256;
      CeMLHashModeType hash_mode = CEML_HASH_MODE_HASH;
      CeMLHashSeqType  hash_seq  = CEML_HASH_LAST;

      retval = CeMLHashInit(&hash_cntx, hash_algo);
      BREAKIF(retval != CEML_ERROR_SUCCESS);
      retval |= CeMLHashSetParam(hash_cntx, CEML_HASH_PARAM_MODE, &hash_mode,
                                 sizeof(CeMLHashModeType), hash_algo);
      retval |= CeMLHashSetParam(hash_cntx, CEML_HASH_PARAM_SEQ,  &hash_seq,
                                 sizeof(CeMLHashSeqType),  hash_algo);
      BREAKIF(retval != CEML_ERROR_SUCCESS);

      // Decryption + Hash
      retval |= CeMLHashCipherData(cntx, hash_cntx, ioVecIn, &ioVecOut,
                                   digest, CEML_HASH_DIGEST_SIZE_SHA256);
      BREAKIF(retval != CEML_ERROR_SUCCESS);
    }

    retval = CEML_ERROR_SUCCESS;
  } while(0);

  // Teardown
  if (cntx != NULL)
  {
    retval_teardown = CeMLCipherDeInit(&cntx);
    if (retval == CEML_ERROR_SUCCESS)
    {
      retval = retval_teardown;
    }
  }
  if (hash_cntx != NULL)
  {
    retval_teardown = CeMLHashDeInit(&hash_cntx);
    if (retval == CEML_ERROR_SUCCESS)
    {
      retval = retval_teardown;
    }
  }
  if (crypto_init)
  {
    retval_teardown = CeMLDeInit();
    if (retval == CEML_ERROR_SUCCESS)
    {
      retval = retval_teardown;
    }
  }

  return retval;
}

int uie_hash_sha256(uint8 *msg, uint32 msg_len,
                    uint8 *digest)
{
  CeMLHashAlgoType  algo = CEML_HASH_ALGO_SHA256;
  uint32            digest_len = CEML_HASH_DIGEST_SIZE_SHA256;
  CeMLCntxHandle*   cntx = NULL;
  CEMLIovecListType ioVecIn;
  CEMLIovecListType ioVecOut;
  CEMLIovecType     IovecIn;
  CEMLIovecType     IovecOut;
  CeMLErrorType     retval = CEML_ERROR_INVALID_PARAM;
  CeMLErrorType     retval_teardown;
  bool              crypto_init = false;

  do {
    // Parameter checks
    BREAKIF(msg == NULL || digest == NULL);
  
    // Setup
    ioVecIn.size = 1;
    ioVecIn.iov = &IovecIn;
    ioVecIn.iov[0].dwLen = msg_len;
    ioVecIn.iov[0].pvBase = msg; 

    ioVecOut.size = 1;
    ioVecOut.iov = &IovecOut;
    ioVecOut.iov[0].dwLen = digest_len;
    ioVecOut.iov[0].pvBase = digest;
    
    retval = CeMLInit();
    crypto_init = true;
    BREAKIF(retval != CEML_ERROR_SUCCESS);
    retval = CeMLHashInit(&cntx, algo);
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    // Hashing
    retval = CeMLHashAtomic(cntx, ioVecIn, &ioVecOut);
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    retval = CEML_ERROR_SUCCESS;
  } while(0);

  // Teardown
  if (cntx != NULL)
  {
    retval_teardown = CeMLHashDeInit(&cntx);
    if (retval == CEML_ERROR_SUCCESS)
    {
      retval = retval_teardown;
    }
  }
  if (crypto_init)
  {
    retval_teardown = CeMLDeInit();
    if (retval == CEML_ERROR_SUCCESS)
    {
      retval = retval_teardown;
    }
  }

  return retval;
}

#ifdef UIE_CRYPTO_TEST
/*===========================================================================
                        UIE CRYPTOGRAPHIC UNIT TESTS
===========================================================================*/

// Test vector from RFC 3962 page 11
static uint8 key[] =
{
  0x63, 0x68, 0x69, 0x63, 0x6b, 0x65, 0x6e, 0x20,
  0x74, 0x65, 0x72, 0x69, 0x79, 0x61, 0x6b, 0x69
};
static uint8 iv[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static uint8 pt[] =
{
  0x49, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20,
  0x6c, 0x69, 0x6b, 0x65, 0x20, 0x74, 0x68, 0x65,
  0x20
};
static uint8 ct[] =
{
  0xc6, 0x35, 0x35, 0x68, 0xf2, 0xbf, 0x8c, 0xb4,
  0xd8, 0xa5, 0x80, 0x36, 0x2d, 0xa7, 0xff, 0x7f,
  0x97
};
static uint8 sha[] =
{
  0xad, 0x98, 0x7c, 0x81, 0x2c, 0x84, 0x64, 0x4e,
  0x34, 0x89, 0x51, 0x41, 0x87, 0xe5, 0x21, 0x90,
  0x04, 0x07, 0x1e, 0xeb, 0x9d, 0x8d, 0xbf, 0x9f,
  0xc4, 0xec, 0x4e, 0xb3, 0x33, 0x41, 0x9f, 0xa1
};

// Test vector from RFC 3962 page 12
static uint8 key2[] =
{
  0x63, 0x68, 0x69, 0x63, 0x6b, 0x65, 0x6e, 0x20,
  0x74, 0x65, 0x72, 0x69, 0x79, 0x61, 0x6b, 0x69
};
static uint8 iv2[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static uint8 pt2[] =
{
  0x49, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20,
  0x6c, 0x69, 0x6b, 0x65, 0x20, 0x74, 0x68, 0x65,
  0x20, 0x47, 0x65, 0x6e, 0x65, 0x72, 0x61, 0x6c,
  0x20, 0x47, 0x61, 0x75, 0x27, 0x73, 0x20, 0x43,
  0x68, 0x69, 0x63, 0x6b, 0x65, 0x6e, 0x2c, 0x20,
  0x70, 0x6c, 0x65, 0x61, 0x73, 0x65, 0x2c
};
static uint8 ct2[] =
{
  0x97, 0x68, 0x72, 0x68, 0xd6, 0xec, 0xcc, 0xc0,
  0xc0, 0x7b, 0x25, 0xe2, 0x5e, 0xcf, 0xe5, 0x84,
  0xb3, 0xff, 0xfd, 0x94, 0x0c, 0x16, 0xa1, 0x8c,
  0x1b, 0x55, 0x49, 0xd2, 0xf8, 0x38, 0x02, 0x9e,
  0x39, 0x31, 0x25, 0x23, 0xa7, 0x86, 0x62, 0xd5,
  0xbe, 0x7f, 0xcb, 0xcc, 0x98, 0xeb, 0xf5
};

static void test_cts(void)
{
  int status = -1;

  uint8 plaintext[sizeof(pt)];
  uint8 digest[sizeof(sha)];
  memset(plaintext, 0x00, sizeof(plaintext));
  memset(digest, 0x00, sizeof(digest));
  status = uie_decrypt_cts(key, sizeof(key),
                           iv,  sizeof(iv),
                           ct, sizeof(ct),
                           plaintext, sizeof(pt),
                           digest);

  if (status || memcmp(plaintext, pt, sizeof(pt)) ||
      memcmp(digest, sha, sizeof(sha)))
  {
    UIE_LOG_MSG("UIE CBC hash-cipher unit test failed");
    UIE_LOG_MSG("  status: %d", status);
    UIE_LOG_HEX("  expected pt", pt, sizeof(pt));
    UIE_LOG_HEX("  expected digest", sha, sizeof(sha));
    UIE_LOG_HEX("  actual pt", plaintext, sizeof(pt));
    UIE_LOG_HEX("  actual digest", digest, sizeof(digest));
    return;
  }

  uint8 plaintext2[sizeof(pt2)];
  memset(plaintext2, 0x00, sizeof(plaintext2));
  status = uie_decrypt_cts(key2, sizeof(key2),
                           iv2,  sizeof(iv2),
                           ct2, sizeof(ct2),
                           plaintext2, sizeof(pt2),
                           NULL);
  if (status || memcmp(plaintext2, pt2, sizeof(pt2)))
  {
    UIE_LOG_MSG("UIE CBC cipher unit test failed");
    UIE_LOG_MSG("  status: %d", status);
    UIE_LOG_HEX("  expected pt", pt2, sizeof(pt2));
    UIE_LOG_HEX("  actual pt", plaintext2, sizeof(pt2));
    return;
  }

  UIE_LOG_MSG("UIE CBC unit tests passed");
}

/* SHA 256 test vector from smetest/test_crypto */
static uint8 sha_plaintext[] = 
{
  0x54, 0x65, 0x73, 0x74, 0x20, 0x76, 0x65, 0x63,
  0x74, 0x6f, 0x72, 0x20, 0x66, 0x72, 0x6f, 0x6d,
  0x20, 0x66, 0x65, 0x62, 0x6f, 0x6f, 0x74, 0x69,
  0x2e, 0x63, 0x6f, 0x6d
};

static uint8 sha_digest[] =
{
  0x07, 0x7b, 0x18, 0xfe, 0x29, 0x03, 0x6a, 0xda,
  0x48, 0x90, 0xbd, 0xec, 0x19, 0x21, 0x86, 0xe1,
  0x06, 0x78, 0x59, 0x7a, 0x67, 0x88, 0x02, 0x90,
  0x52, 0x1d, 0xf7, 0x0d, 0xf4, 0xba, 0xc9, 0xab
};

static void test_sha(void)
{
  int status = -1;

  uint8 digest[sizeof(sha_digest)];
  memset(digest, 0x00, sizeof(sha_digest));
  status = uie_hash_sha256(sha_plaintext, sizeof(sha_plaintext), digest);
  if (status || memcmp(digest, sha_digest, sizeof(sha_digest)))
  {
    UIE_LOG_MSG("UIE SHA unit test failed");
    UIE_LOG_MSG("  status: %d", status);
    UIE_LOG_HEX("  expected", sha_digest, sizeof(sha_digest));
    UIE_LOG_HEX("  actual", digest, sizeof(digest));
    return;
  }

  UIE_LOG_MSG("UIE SHA unit test passed");
};

// NIST SP 800-38C C.1 example vector
static uint8 ccm_key[] = 
{
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
};

static uint8 ccm_nonce[] = 
{
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16
};

static uint8 ccm_adata[] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};

static uint8 ccm_plaintext[] = 
{
  0x20, 0x21, 0x22, 0x23
};

static uint8 ccm_ciphertext[] =
{
  0x71, 0x62, 0x01, 0x5b
};

static uint8 ccm_mac[] =
{
  0x4d, 0xac, 0x25, 0x5d
};

static void test_ccm(void)
{
  int status = -1;
  uint8 decrypted_payload[sizeof(ccm_plaintext)];
  memset(decrypted_payload, 0x00, sizeof(decrypted_payload));

  status = 
  uie_decrypt_ccm(UIE_KEY_SRC_PROVIDED,
                  ccm_key,        sizeof(ccm_key),
                  ccm_nonce,      sizeof(ccm_nonce),
                  ccm_ciphertext, sizeof(ccm_ciphertext),
                  ccm_adata,      sizeof(ccm_adata),
                  ccm_mac,        sizeof(ccm_mac),
                  decrypted_payload, sizeof(ccm_ciphertext));
  if (status || memcmp(decrypted_payload, ccm_plaintext,
                       sizeof(ccm_plaintext)))
  {
    UIE_LOG_MSG("UIE CCM unit test failed");
    UIE_LOG_MSG("  status: %d", status);
    UIE_LOG_HEX("  expected", ccm_plaintext, sizeof(ccm_plaintext));
    UIE_LOG_HEX("  actual", decrypted_payload, sizeof(decrypted_payload));
    return;
  }

  UIE_LOG_MSG("UIE CCM unit test passed");
}

void uie_crypto_test(void)
{
  test_cts();
  test_sha();
  test_ccm();
}
#endif
