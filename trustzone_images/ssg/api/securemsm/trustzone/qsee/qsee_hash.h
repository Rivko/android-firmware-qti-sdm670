#ifndef QSEE_HASH_H
#define QSEE_HASH_H

/**
@file qsee_hash.h
@brief Provide hash API wrappers
*/

/*===========================================================================
   Copyright (c) 2010-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
11/22/17   jp       Added SHA384 support
08/20/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include <stdint.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QSEE_SHA1_HASH_SZ    20
#define QSEE_SHA256_HASH_SZ  32
#define QSEE_SHA384_HASH_SZ  48

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef void qsee_hash_ctx;
#ifndef QSEE_CIPHER_CTX
#define QSEE_CIPHER_CTX
typedef void qsee_cipher_ctx;
#endif

/** Support for SHA1, SHA256 and SHA384 for hash */
typedef enum
{
  QSEE_HASH_NULL         = 1, ///< Do not perform any hashing
  QSEE_HASH_SHA1         = 2,
  QSEE_HASH_SHA256       = 3,
  QSEE_HASH_SHA384       = 4,
  QSEE_HASH_INVALID = 0x7FFFFFFF,
} QSEE_HASH_ALGO_ET;

typedef enum
{
  QSEE_HASH_PARAM_MODE      = 0,
  QSEE_HASH_PARAM_HMAC_KEY  = 1,
  QSEE_HASH_PARAM_SEQ       = 2,
  QSEE_HASH_PARAM_INVALID = 0x7FFFFFFF,
} QSEE_HASH_PARAM_ET;

typedef enum 
{ 
  QSEE_HASH_MODE_HASH    = 0, // Plain SHA
  QSEE_HASH_MODE_HMAC    = 1,  // HMAC SHA
  QSEE_HASH_MODE_INVALID = 0x7FFFFFFF,
} QSEE_HASH_MODE_ET;

typedef enum 
{ 
  QSEE_HASH_MODE_FIRST   = 0, 
  QSEE_HASH_MODE_LAST    = 1,
  QSEE_HASH_MODE_INVALID2 = 0x7FFFFFFF,
} QSEE_HASH_SEQ_ET; 

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief  This function will create a message digest hash using the
 *         algorithm specified.
 *
 * @param[in] hash         The hash algorithm
 * @param[in] msg          The message to hash
 * @param[in] msg_len      The length of the message
 * @param[in,out] digest   The digest to store
 * @param[in] digest_len   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 */
int qsee_hash(QSEE_HASH_ALGO_ET alg,
              const uint8_t    *msg,
              uint32_t          msg_len,
              uint8_t          *digest,
              uint32_t          digest_len);

/**
 * @brief Intialize a hash context for update and final functions.
 *
 * @param alg[in]       The algorithm standard to use
 * @param hash_ctx[out  The hash context
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_hash_init(QSEE_HASH_ALGO_ET alg,
                   qsee_hash_ctx **hash_ctx);


/**
 * @brief Resets hash context, will not reset keys.
 *
 * @param hash_ctx[in] The hash context
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_hash_reset(qsee_hash_ctx *hash_ctx);

/**
 * @brief  This function will hash some data into the hash context
 *         structure, which Must have been initialized by
 *         qsee_hash_init().
 *
 * @param hash_ctx[in]    The hash context
 * @param msg[in]         Pointer to the msg to hash
 * @param msg_len[in]     Length of the msg to hash
 *
 * @return 0 on success, negative on failure
 *
 * @see qsee_hash_init
 */
int qsee_hash_update(const qsee_hash_ctx    *hash_ctx,
                     const uint8_t          *msg,
                     uint32_t                msg_len);

/**
 * @brief  Compute the digest hash value
 *
 * @param[in] hash_ctx     The hash context
 * @param[in] digest       Pointer to output message digest hash
 * @param[in] digest_len   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 *
 * @see qsee_hash_init
 */
int qsee_hash_final(const qsee_hash_ctx  *hash_ctx,
                    uint8_t                 *digest,
                    uint32_t                digest_len);

/**
 * @brief Release all resources with a given hash context.
 *
 * @param[in] hash_ctx Hash context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_hash_free_ctx(qsee_hash_ctx *hash_ctx);

/**
 * @brief  This function will perform a simultaneous Hash/Cipher
 *         Encrypt operation. 
 *
 * @param cipher_ctx[in]   The cipher context 
 * @param hash_ctx[in]     The hash context
 * @param pt[in]           The input plaintext buffer
 * @param pt_len[in]       The input plaintext buffer length 
 * @param ct[in,out]       The output ciphertext buffer
 * @param ct_len[in]       The output ciphertext buffer length. 
 * @param digest[in,out]   The digest to store 
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 */
int qsee_hashcipher_encrypt(const qsee_cipher_ctx *cipher_ctx,
                            const qsee_hash_ctx   *hash_ctx,
                            uint8_t                 *pt,
                            uint32_t                 pt_len,
                            uint8_t                 *ct,
                            uint32_t                 ct_len,
                            uint8_t                 *digest,
                            uint32_t                 digest_len);


/**
 * @brief  This function will perform a simultaneous Hash/Cipher
 *         Decrypt operation. 
 *
 * @param cipher_ctx[in]   The cipher context
 * @param hash_ctx[in]     The hash context 
 * @param ct[in]           The input ciphertext buffer
 * @param ct_len[in]       The input ciphertext buffer length. 
 * @param pt[in,out]       The output plaintext buffer
 * @param pt_len[in]       The output plaintext buffer length 
 * @param digest[in,out]   The digest to store 
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 */
int qsee_hashcipher_decrypt(const qsee_cipher_ctx   *cipher_ctx,
                            const qsee_hash_ctx     *hash_ctx,
                            uint8_t                 *ct,
                            uint32_t                 ct_len,
                            uint8_t                 *pt,
                            uint32_t                 pt_len,
                            uint8_t                 *digest,
                            uint32_t                 digest_len);

/**
 * @brief Modify the parameters for a given hash operation.
 *
 * @param hash_ctx[in]   Hash context
 * @param param_id[in]   The parameter to modify
 * @param param[in]      The parameter value to set.
 * @param param_len[in]  The length of the param (in bytes).
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_hash_set_param(const qsee_hash_ctx *hash_ctx,
                        QSEE_HASH_PARAM_ET   param_id,
                        const void          *param,
                        uint32_t             param_len);

#endif /*QSEE_HASH_H*/

