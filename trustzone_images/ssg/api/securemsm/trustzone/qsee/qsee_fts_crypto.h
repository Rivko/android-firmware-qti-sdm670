#ifndef _QSEE_FTS_CRYPTO_H_
#define _QSEE_FTS_CRYPTO_H_

/**
@file   qsee_fts_crypto.h
@brief  Defintions for cryptogrpahic module in the FTS implementation.
*/

/*=============================================================================
Copyright (c) 2017 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================
===
=== The qsee_fts_crypto module provides an interface between the
=== cryptographic resources needed by the FTS implementation
=== and the low level cryptographic services provided by the Trustzone
=== crypto driver.
===
=== This module provides the following protection and verification of the
=== below security features:
===     - Confidentiality (AES256 CBC)
===     - Integrity       (HMAC SHA256, HASH SHA256)
=== through the interface defined in this file.
===
=== Version 1.0
===   As part of version 1.0, there are some restrictions on the usage of
===   these APIs.
===     1. The IV and HMAC/HASH sizes currently only support usage with
===        AES256 and SHA256 algorithms. Use the macro definitions below to
===        guarentee that you are allocating the correc sizes.
===
===========================================================================*/

#include <stddef.h>
#include "qsee_cipher.h"
#include "qsee_hmac.h"
#include "qsee_hash.h"
#include "tee_internal_api.h"

/* Crypto Attribute Sizes */
#define QSEE_FTS_CRYPTO_IV_SIZE    QSEE_AES256_IV_SIZE
#define QSEE_FTS_CRYPTO_HMAC_SIZE  QSEE_SHA256_HASH_SZ
#define QSEE_FTS_CRYPTO_HASH_SIZE  QSEE_SHA256_HASH_SZ

/*===========================================================================
=== qsee_fts_encrypt
===
=== Encrypt data specified using iv provided by the caller and unique key
=== generated internally for application. Encryption operation will be done
=== in place within the user allocated buffer provided.
===
=== Parameters:
===   - [in/out]data  Pointer to buffer containing data to encrypt, which will
===                   also contain the output ciphertext after encryption.
===                   Caller is responsible for memory management of buffer.
===   - [in]dataLen   Length of the data buffer allocated by caller.
===   - [out]iv       Pointer to the iv (initialization vector) container in
===                   which a random value will be generated.
===   - [in]ivLen     Length of the iv buffer allocated by the caller.
===
=== Returns
===   - TEE_SUCCESS        When encrypt operation is successfull.
===   - TEE_ERROR_GENERIC  When encrypt operation fails.
===
===========================================================================*/
TEE_Result qsee_fts_encrypt
(
  void*  data,
  size_t dataLen,
  void*  iv,
  size_t ivLen
);

/*===========================================================================
=== qsee_fts_integrity_protect
===
=== Protect the data specified by generating a hash and/or hmac into the
=== specified containers.
===
=== Parameters:
===   - [in]data      Pointer to buffer containing data to hash and/or hmac.
===                   Caller is responsible for memory management of buffer.
===   - [in]dataLen   Length of the data buffer allocated by caller.
===   - [out]hash     Pointer to the hash output container where the generated
===                   hash over the data will be stored. Caller is responsible
===                   for memory management of buffer.
===   - [in]hashLen   Length of the hash container allocated by the caller.
===   - [out]hmac     Pointer to the hmac output container where the generated
===                   hmac over the data will be stored. Caller is responsible
===                   for memory management of buffer.
===   - [in]hmacLen   Length of the hmac container allocated by the caller.
===
=== Returns
===   - TEE_SUCCESS        When hash and/or hmac operation is successfull.
===   - TEE_ERROR_GENERIC  When hash and/or hmac operation fails.
===
===========================================================================*/
TEE_Result qsee_fts_integrity_protect
(
  void*  data,
  size_t dataLen,
  void*  hash,
  size_t hashLen,
  void*  hmac,
  size_t hmacLen
);

/*===========================================================================
=== qsee_fts_decrypt
===
=== Decrypt data specified using iv provided by the caller and unique key
=== generated internally for application. Decryption operation will be done
=== in place within the user allocated buffer provided.
===
=== Parameters:
===   - [in/out]data  Pointer to buffer containing data to decrypt, which will
===                   also contain the output plaintext after decryption. Caller
===                   is responsible for memory management of buffer.
===   - [in]dataLen   Length of the data buffer allocated by caller.
===   - [in]iv        Pointer to the iv (initialization vector) container which
===                   includes valid information set by the caller.
===   - [in]ivLen     Length of the iv buffer allocated by the caller.
===
=== Returns
===   - TEE_SUCCESS        When decrypt operation is successfull.
===   - TEE_ERROR_GENERIC  When decrypt operation fails.
===
===========================================================================*/
TEE_Result qsee_fts_decrypt
(
  void*  data,
  size_t dataLen,
  void*  iv,
  size_t ivLen
);

/*===========================================================================
=== qsee_fts_integrity_verify
===
=== Verifies the data specified by comparing the hash and/or hmac provided in
=== the specified containers with the ones generated from the data.
===
=== Parameters:
===   - [in]data      Pointer to buffer containing data to hash and/or hmac.
===                   Caller is responsible for memory management of buffer.
===   - [in]dataLen   Length of the data buffer allocated by caller.
===   - [in]hash      Pointer to the hash input container which will be compared
===                   to the generated hash over the data. Caller is responsible
===                   for memory management of buffer.
===   - [in]hashLen   Length of the hash container allocated by the caller.
===   - [in]hmac      Pointer to the hmac input container which will be compared
===                   to the generated hmac over the data. Caller is responsible
===                   for memory management of buffer.
===   - [in]hmacLen   Length of the hmac container allocated by the caller.
===
=== Returns
===   - TEE_SUCCESS        When hash and/or hmac comparison is successfull.
===   - TEE_ERROR_GENERIC  When hash and/or hmac comparison fails.
===
===========================================================================*/
TEE_Result qsee_fts_integrity_verify
(
  void*  data,
  size_t dataLen,
  void*  hash,
  size_t hashLen,
  void*  hmac,
  size_t hmacLen
);

#endif //_QSEE_FTS_CRYPTO_H_
