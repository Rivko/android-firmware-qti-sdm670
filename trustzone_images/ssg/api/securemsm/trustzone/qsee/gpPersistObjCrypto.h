#ifndef _GP_PERSIST_OBJ_CRYPTO_H_
#define _GP_PERSIST_OBJ_CRYPTO_H_

/**
@file   gpPersistObjCrypto.h
@brief  Temporary GP Crypto stubs
*/

/*=============================================================================
Copyright (c) 2017 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

#include <stddef.h>
#include <stdint.h>
#include "gpPersistObjFileIO.h"   /* Included to appease indirect includers */
#include "qsee_fts_crypto.h"

/* Crypto Attribute Sizes */
#define GPCRYPTO_IV_SIZE    QSEE_FTS_CRYPTO_IV_SIZE
#define GPCRYPTO_HMAC_SIZE  QSEE_FTS_CRYPTO_HMAC_SIZE
#define GPCRYPTO_HASH_SIZE  QSEE_FTS_CRYPTO_HASH_SIZE

#define UNUSED __attribute__((unused))

static inline TEE_Result
gpCrypto_Encrypt(UNUSED void* handle, void* data, size_t dataLen,
                 void* iv, size_t ivLen) {
  return qsee_fts_encrypt(data, dataLen, iv, ivLen);
}

static inline TEE_Result
gpCrypto_Decrypt(UNUSED void* handle, void* data, size_t dataLen,
                 void* iv, size_t ivLen) {
  return qsee_fts_decrypt(data, dataLen, iv, ivLen);
}

static inline TEE_Result
gpCrypto_Integrity_Protect(UNUSED void* handle, void* data, size_t dataLen,
                           void* hash, size_t hashLen,
                           void* hmac, size_t hmacLen) {
  return qsee_fts_integrity_protect(data, dataLen, hash, hashLen,
                                    hmac, hmacLen);
}

static inline TEE_Result
gpCrypto_Integrity_Verify(UNUSED void* handle, void* data, size_t dataLen,
                          void* hash, size_t hashLen,
                          void* hmac, size_t hmacLen) {
  return qsee_fts_integrity_verify(data, dataLen, hash, hashLen,
                                   hmac, hmacLen);
}

#endif //_GP_PERSIST_OBJ_CRYPTO_H_

