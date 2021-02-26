/*===========================================================================

                              ELF Image Hash

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
11/05/13   kedara     Remove null pointer check for data ptr, 0x0 is a valid addr.
10/15/13   kedara     Added boot_CeMLHashSetParam
03/27/13   dhaval     Add api to set hash-algorithm to support SHA256
11/08/12   kedara     Initial revision.
===========================================================================*/

/*========================================================================
  HASHING MODULE FOR SECBOOT

FILE: boot_hash.c

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

bl_error_type boot_hashInit(boot_hash_ctx_t *ctx, hash_alg_t ht);
bl_error_type boot_hashUpdate(boot_hash_ctx_t *ctx, void *data, 
                               uint32 data_len);
bl_error_type boot_hashFinal(boot_hash_ctx_t *ctx, uint8 *digest_ptr, 
                              uint32 digest_buf_len);
bl_error_type boot_hash(hash_alg_t ht, void *data, uint32 data_len, 
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


MACROS

PBL_HASH_LEN(hash_type) returns the size of the digest value in bytes,
or zero if the hash_type is invalid.

========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "boot_hash.h"
#include "boot_util.h"

/* global hash algorithm and length defines, default to SHA256 and it can
   be overrided by calling boot_set_hash_algo api */
hash_alg_t sbl_hash_algorithm = SBL_HASH_SHA256;
uint32 sbl_hash_len = PBL_SHA256_HASH_LEN;
uint8 non_block_mode = NON_BLOCK_MODE_DISABLE;

/*===========================================================================
**  Function :  boot_check_digest_len
** ==========================================================================
*/
/*!
 *  Verify buffer length for hashing algorithm.
 *
 * @param CeMLHashAlgoType [IN]
 *   Crypto driver, hash algo type.
 *
 * @param CeMLHashAlgoType [IN]
 *   CE, crypto driver, hash algo type.
 *
 * @return boolean [OUT]
 *  Returns TRUE in case of success.
 *
 */
static boolean boot_check_digest_len(CeMLHashAlgoType alg,
                                           uint32 *digest_len)
{
  boolean status = TRUE;
  do
  {
    if (digest_len == NULL)
    {
      status = FALSE;
      break;    
    } 

    if ((alg == CEML_HASH_ALGO_SHA1) &&
      (*digest_len < PBL_SHA1_HASH_LEN))
    {
      status = FALSE;
      break;    
    } 
    else if ((alg == CEML_HASH_ALGO_SHA256 ) &&
             (*digest_len < PBL_SHA256_HASH_LEN))
    {
      status = FALSE;
      break;    
    } 
    else if ((alg != CEML_HASH_ALGO_SHA1) &&
           (alg != CEML_HASH_ALGO_SHA256))
    {
      status = FALSE;
      break;    
    } 

    *digest_len = (alg == CEML_HASH_ALGO_SHA1) ?
                     PBL_SHA1_HASH_LEN : PBL_SHA256_HASH_LEN;
    
  } while(0);
  return status;
  
}

/*===========================================================================
**  Function :  boot_map_hash_enum
** ==========================================================================
*/
/*!
 *  Map boot hash algorithm type to equivalent crypto driver enums.
 *
 * @param hash_alg_t [IN]
 *   Boot Hash algo type.
 *
 * @param CeMLHashAlgoType [OUT]
 *   Crypto driver hash algo type.
 *
 * @return boolean [OUT]
 *  Returns TRUE in case of success.
 *
 */
static inline boolean boot_map_hash_enum(hash_alg_t alg1,
                                        CeMLHashAlgoType   *alg2)
{
  boolean status = TRUE;
  do
  {
    if (alg2 == NULL)
    {  
      status = FALSE;
      break;
    }

    if (alg1 == SBL_HASH_SHA1)
	{
      *alg2 = CEML_HASH_ALGO_SHA1;
	  sbl_hash_len = PBL_SHA1_HASH_LEN;	  
	}
    else if (alg1 == SBL_HASH_SHA256)
	{
      *alg2 = CEML_HASH_ALGO_SHA256;
	  sbl_hash_len = PBL_SHA256_HASH_LEN;	  
	}
    else
	{
      status = FALSE;
	}
  
  } while(0);
  return status;
}

/*===========================================================================

**  Function :  boot_hash

** ==========================================================================
*/
/*!
 *  Single part hash.  
 *
 * @param halg [IN]
 *   Hash algorithm type
 *
 * @param data [IN]
 *   pointer to data to hash.
 *
 * @param data_len [IN]
 *   data buffer length.
 *
 * @param digest_ptr [IN]
 *   pointer to digest.
 *
 * @param digest_buf_len [IN]
 *   digest buffer length.
 *
 * @return bl_error_type [OUT]
 *  Returns error code in case of Hashing error.
 *  Returns 0 (BL_ERR_NONE) in case of success.
 *
 */
bl_error_boot_type boot_hash
(
  hash_alg_t  alg, 
  const uint8    *data, 
  uint32          data_len, 
  uint8          *digest_ptr, 
  uint32          digest_buf_len
)
{
  boot_hash_ctx_t ctx;
  bl_error_boot_type status = BL_ERR_MAX;  

  do
  {
    if ((status = boot_hashInit(&ctx, alg)) != BL_ERR_NONE)
      break;

    if ((status = boot_hashUpdate(&ctx, data, data_len)) != BL_ERR_NONE)
      break;

    if ((status = boot_hashFinal(&ctx, digest_ptr, digest_buf_len)) != BL_ERR_NONE)
      break;

  } while (0);

  return status;
}


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
)
{
  bl_error_boot_type status = BL_ERR_NONE;
  CeMLErrorType ceml_init, ceml_hash_init, ceml_hash_set_params;  
  CeMLHashAlgoType alg2;
  
  do
  {
    if (hash_ctx == NULL)
    {
      status = BL_ERR_NULL_PTR_PASSED;
      break;
    }
  
    if ( FALSE == boot_map_hash_enum(ht, &alg2))
    {
      status = (bl_error_boot_type) CEML_ERROR_INVALID_PARAM;
      break;
    }

    if((ceml_init=boot_CeMLInit())!= CEML_ERROR_SUCCESS)
    {
      status = (bl_error_boot_type)ceml_init;
      break;
    }
  
    hash_ctx->alg = alg2;
    hash_ctx->ctx = NULL;  

    if ((ceml_hash_init=boot_CeMLHashInit(&hash_ctx->ctx, hash_ctx->alg))!=CEML_ERROR_SUCCESS)
    {
      status = (bl_error_boot_type)ceml_hash_init;
      break;
    }
  
    /*Set the crypto hash update api to desired mode blocking/nonblocking */
	ceml_hash_set_params = boot_CeMLHashSetParam(hash_ctx->ctx,
                                                     CEML_HASH_NONBLOCK_MODE,
                                                     (const void*)&non_block_mode, 
                                                     sizeof(non_block_mode),                                                     
                                                     hash_ctx->alg);
    if (CEML_ERROR_SUCCESS != ceml_hash_set_params)
    {
      status = (bl_error_boot_type)ceml_hash_set_params;
      break;
    }
    
  
  } while(0);
  return status;
}


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
)
{
  bl_error_boot_type status = BL_ERR_NONE;
  CEMLIovecListType    iovec;
  CEMLIovecType        iovec_buf; 
  CeMLErrorType ceml_hash_Update;
  do
  {
    iovec.size  = 1;
    iovec.iov   = &iovec_buf;
    iovec.iov[0].pvBase = (void *)data;
    iovec.iov[0].dwLen = len;

    /* data is not checked against null because 0 is
       valid address
    */
    if (hash_ctx == NULL)
    {
      status = BL_ERR_NULL_PTR_PASSED;
      break;
    }

  if((ceml_hash_Update = boot_CeMLHashUpdate(hash_ctx->ctx, iovec)) != CEML_ERROR_SUCCESS)
    status = (bl_error_boot_type)ceml_hash_Update;
    
  }while(0);
  return status;  
}

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
)
{
  bl_error_boot_type status = BL_ERR_NONE;
  CEMLIovecListType    iovec;
  CEMLIovecType        iovec_buf;
  uint8                buf[CEML_HASH_DIGEST_SIZE_SHA256];
  CeMLErrorType ceml_hash_final,ceml_deinit;  
  
  do
  {
    iovec.size = 1;
    iovec.iov = &iovec_buf;
    iovec.iov[0].pvBase = buf;
    iovec.iov[0].dwLen = sizeof(buf);

    if (hash_ctx == NULL || digest_ptr == NULL)
    {
      status = BL_ERR_NULL_PTR_PASSED;
      break;
    }
	
	BL_VERIFY(TRUE == boot_check_digest_len(hash_ctx->alg, &digest_buf_len),BL_ERR_IMG_SECURITY_FAIL|BL_ERROR_GROUP_BOOT);
    
    if((ceml_hash_final = boot_CeMLHashFinal(hash_ctx->ctx, &iovec)) != CEML_ERROR_SUCCESS ) 
    {   
      status = (bl_error_boot_type)ceml_hash_final;
      break;
    }
  
    qmemcpy(digest_ptr, (uint8 *)(iovec.iov[0].pvBase), digest_buf_len);
  
    /* clear hash on stack */
    qmemset((void *) buf, 0, sizeof(buf));
	
    boot_CeMLHashDeInit(&hash_ctx->ctx);
  
    if((ceml_deinit = boot_CeMLDeInit()) != CEML_ERROR_SUCCESS)
    {
      status = (bl_error_boot_type)ceml_deinit;
      break;
    }
  
  }while(0);
  return status;  
}   

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
)
{
  bl_error_boot_type status = BL_ERR_NONE;

  do
  {
    if (hash_algo == SBL_HASH_SHA1)
    {
      sbl_hash_algorithm = hash_algo;
      sbl_hash_len = PBL_SHA1_HASH_LEN;
    }
    else if (hash_algo == SBL_HASH_SHA256)
    {
      sbl_hash_algorithm = hash_algo;
      sbl_hash_len = PBL_SHA256_HASH_LEN;
    }
    else
    {
      status = BL_ERR_UNSUPPORTED_HASH_ALGO;
      break;
    }
  
  } while(0);
  return status;
}

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
void boot_set_hash_update_mode ( uint32 hash_update_mode)
{
  non_block_mode = hash_update_mode;
}
