#ifndef BOOT_HASH_H
#define BOOT_HASH_H
/*===========================================================================

                              ELF Image Hash Header

GENERAL DESCRIPTION
  This module supports ELF image segment hashing. 

Copyright 2012-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who        what, where, why
--------   ---        -------------------------------------------------------
11/07/13   kedara     Added api to select blocking/non-blocking mode for hash update.
03/27/13   dhaval     Add api to set hash-algorithm to support SHA256
11/08/12   kedara     Initial revision.
===========================================================================*/

/*========================================================================
  HASHING MODULE FOR SECBOOT

FILE: boot_hash.h

DESCRIPTION:

This is the secboot interface into hashing.  There is one function for
hashing a single buffer (boot_hash), and the standard triumvirate of
init, update, and final (boot_hashInit, boot_hashUpdate, and
boot_hashFinal), for multi-part hashing.

boot_hash and boot_hashInit take the hash type at an argument.  
At present the only two valid values are SBL_HASH_SHA1 and SBL_HASH_SHA256.  

boot_hashInit, boot_hashUpdate, and boot_hashFinal all take a pointer to
a context structure.  The same context structure works for all hash
types, and regardless of whether the implementation is hardware or
software.  (See below.)

EXTERNALIZED FUNCTIONS

bl_error_boot_type boot_hashInit(boot_hash_ctx_t *ctx, hash_alg_t ht);
bl_error_boot_type boot_hashUpdate(boot_hash_ctx_t *ctx, void *data, 
                               uint32 data_len);
bl_error_boot_type boot_hashFinal(boot_hash_ctx_t *ctx, uint8 *digest_ptr, 
                              uint32 digest_buf_len);
bl_error_boot_type boot_hash(hash_alg_t ht, void *data, uint32 data_len, 
                         uint8 *digest_ptr, 
                         uint32 digest_buf_len);


THREADING AND CONCURRENCY

No concurrent use of same CE hw engine is allowed. But this is okay in the boot loader,
which is single-threaded.


RETURN VALUES

E_SUCCESS: success
E_INVALID_ARG: some argument is invalid or out of range, or possibly
               the context structure is uninitialized (update and final only).
           Functions that accept a hash type (algorithm) check it and will 
           return this error code if it is invalid.  (Thus there is no
           need to check the validity of the hash type before calling.)
E_DATA_TOO_LARGE: target buffer is too small to receive the hash value.
E_FAILURE:  hardware error, or hardware was unexpecedly busy, or some
            other error, other than the above, detected in the driver.


========================================================================*/
#include "boot_comdef.h"
#include "boot_error_if.h"
#include "boot_extern_crypto_interface.h"


/*===========================================================================
               Preprocessor Definitions and Constants
===========================================================================*/ 

#define PBL_SHA1_HASH_LEN 20
#define PBL_SHA256_HASH_LEN 32
#define PBL_SHA_HASH_MAX_LEN PBL_SHA256_HASH_LEN

typedef enum 
{
  SBL_HASH_UNDEFINED_HASH_CODE = 0, 
  SBL_HASH_SHA1, 
  SBL_HASH_SHA256,
  SBL_HASH_UNSUPPORTED_HASH_CODE,
  SBL_HASH_HASH_RESERVED = 0x7fffffff /* force to 32 bits */
} hash_alg_t;

typedef struct 
{
  CeMLHashAlgoType  alg;
  CeMLCntxHandle    *ctx;
} boot_hash_ctx_t;


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

bl_error_boot_type boot_hash
(
  hash_alg_t  halg, 
  const uint8    *data, 
  uint32          data_len, 
  uint8          *digest_ptr, 
  uint32          digest_buf_len
);

/*===========================================================================
**  Function :  boot_hashInit
** ==========================================================================
*/
/*!
 *  Set up hashing.
 *
 * @param hash_ctx [IN]
 *   Hash context.
 *
 * @param ht [IN]
 *   Hash algorithm type.
 *
 * @return bl_error_boot_type [OUT]
 *  Returns error code in case of Hashing error.
 *  Returns 0 (BL_ERR_NONE) in case of success.
 *
 */
bl_error_boot_type boot_hashInit
(
  boot_hash_ctx_t *hash_ctx, 
  hash_alg_t  ht
);

/*===========================================================================
**  Function :  boot_hashUpdate
** ==========================================================================
*/
/*!
 *  Hash some data into the context structure, which must have been
 *  initialized by boot_hashInit()
 *
 * @param ctx [IN]
 *   Hash context.
 *
 * @param data [IN]
 *   pointer to data.
 *
 * @param len [IN]
 *   Data buffer length.
 *
 * @return bl_error_boot_type [OUT]
 *  Returns error code in case of Hashing error.
 *  Returns 0 (BL_ERR_NONE) in case of success.
 *
 */
bl_error_boot_type boot_hashUpdate
(
  boot_hash_ctx_t *hash_ctx, 
  const uint8    *data, 
  uint32          len
);

/*===========================================================================
**  Function :  boot_hashFinal
** ==========================================================================
*/
/*!
 *  Compute the digest value (hash) and store it via digest_ptr.
 *
 *  @param ctx [IN]
 *   Hash context.
 *
 * @param digest_ptr [IN]
 *   pointer to digest.
 *
 * @param digest_buf_len [IN]
 *   Digest buffer length.
 *
 * @return bl_error_boot_type [OUT]
 *  Returns error code in case of Hashing error.
 *  Returns 0 (BL_ERR_NONE) in case of success.
 *
 */
bl_error_boot_type boot_hashFinal
(
  boot_hash_ctx_t *hash_ctx, 
  uint8          *digest_ptr, 
  uint32          digest_buf_len
);

/*===========================================================================
**  Function :  boot_set_hash_algo
** ==========================================================================
*/
/*!
 *  Set up hashing algorithm.
 *
 * @param ht [IN]
 *   Hash algorithm type.
 *
 * @return bl_error_boot_type [OUT]
 *  Returns error code in case of error in initializing hash algorithm.
 *  Returns 0 (BL_ERR_NONE) in case of success.
 *
 */
bl_error_boot_type boot_set_hash_algo
(
  hash_alg_t hash_algo
);

/*===========================================================================
**  Function :  boot_set_hash_update_mode
** ==========================================================================
*/
/*!
 *  Set up hash update mode. Default mode is blocking.
 *
 * @param hash_update_mode [IN]
 *  Hash Update mode:
 *    NON_BLOCK_MODE_ENABLE 0x1
 *    NON_BLOCK_MODE_DISABLE 0x0
 *
 * @return 
 *  None
 *
 */
void boot_set_hash_update_mode (uint32 hash_update_mode);

#endif
