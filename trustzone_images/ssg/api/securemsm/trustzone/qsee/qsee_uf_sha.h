#ifndef QSEE_UF_SHA_H
#define QSEE_UF_SHA_H

/**
@file qsee_uf_sha.h
@brief Provide Software crypto Hash and Hmac API wrappers
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
03/05/13   amen      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/**
 * SHA DIGEST and Block sizes
  */
#define SW_SHA_BLOCK_SIZE               64
#define SW_SHA1_BLOCK_SIZE              SW_SHA_BLOCK_SIZE
#define SW_SHA1_DIGEST_SIZE             (160 / 8)

#define SW_SHA256_BLOCK_SIZE            SW_SHA1_BLOCK_SIZE
#define SW_SHA256_DIGEST_SIZE           (256 / 8)

#define SW_SHA224_BLOCK_SIZE            SW_SHA256_BLOCK_SIZE
#define SW_SHA224_DIGEST_SIZE           (224 / 8)

#define SW_SHA512_BLOCK_SIZE            128
#define SW_SHA512_DIGEST_SIZE           (512 / 8)

#define SW_SHA384_BLOCK_SIZE            SW_SHA512_BLOCK_SIZE
#define SW_SHA384_DIGEST_SIZE           (384 / 8)

#define SW_MD5_BLOCK_SIZE               (512 / 8)
#define SW_MD5_DIGEST_SIZE              16
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * Enum for Hash algorithm type
 */
typedef enum
{
  SW_AUTH_ALG_NULL               = 0x1,
  SW_AUTH_ALG_SHA1               = 0x2,
  SW_AUTH_ALG_SHA256,
  SW_AUTH_ALG_SHA224,
  SW_AUTH_ALG_SHA384,
  SW_AUTH_ALG_SHA512,
  SW_AUTH_ALG_MD5,
  SW_AUTH_ALG_INVALID            = 0x7FFFFFFF
} SW_Auth_Alg_Type;

typedef enum 
{ 
  SW_HASH_PARAM_MODE              = 0x01,
  SW_HASH_PARAM_IV                = 0x02,
  SW_HASH_PARAM_HMAC_KEY          = 0x03,
  SW_HASH_PARAM_AUTH_KEY          = 0x04,
  SW_HASH_PARAM_MAX               = 0xFFFFFFFF  
} SW_Auth_Param_Type;

#ifndef SW_CRYPTO_TYPEDEF
#define SW_CRYPTO_TYPEDEF
typedef uint32_t sw_crypto_errno_enum_type;

#ifndef IOVECDEF
typedef struct
{
  void                              *pvBase;
  uint32_t                            dwLen;
} __attribute__((__packed__)) IovecType;

typedef struct
{
  IovecType                     *iov;
  uint32_t                        size;
} __attribute__((__packed__)) IovecListType;

#define IOVECDEF

#endif

#ifndef _DEFINED_CRYPTOTOCNTXHANDLE
#define _DEFINED_CRYPTOTOCNTXHANDLE
typedef void CryptoCntxHandle;
#endif

#endif

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param auth_alg     [in] see SW_Auth_Alg_Type
 *
 * @return errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Init(CryptoCntxHandle **handle, SW_Auth_Alg_Type auth_alg);


/**
 * @brief Reset a hash context
 *
 * @param handle       [in] Pointer of pointer to cipher context
 *
 * @return errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Reset(CryptoCntxHandle *handle);

/**
 * @brief Update function for sha1/sha256 for intermediate data
 *        blocks hash
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param ioVecIn      [in] Input to cipher
 *
 * @return errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Update(CryptoCntxHandle *handle,IovecListType ioVecIn);

/**
 * @brief Sha1/Sha256 last block hash update
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param ioVecOut     [in] Output from cipher
 *
 * @return errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Final (CryptoCntxHandle *handle,IovecListType *ioVecOut);

/**
 * @brief deinitialize a hash context
 *
 * @param handle       [in] Pointer of pointer to cipher context
 *
 * @return errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Deinit(CryptoCntxHandle** cntx);

/**
 * @brief  This function will hash some data into the hash context
 *         structure, which Must have been initialized by
 *         SW_Hash_Init(...).
 *
 * @param handle[in]    The hash context
 * @param[in]             Pointer to the msg to hash
 * @param[in]             Length of the msg to hash
 *
 * @return 0 on success, negative on failure
 *
 * @see SW_Hash_Init
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Update_Ez(CryptoCntxHandle* handle, const uint8_t *msg, uint32_t msg_len);

/**
 * @brief  Compute the digest hash value
 *
 * @param handle[in]     The hash context
 * @param digest[in]       Pointer to output message digest hash
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes.
 *
 * @return 0 on success, negative on failure
 *
 * @see SW_Hash_Init
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Final_Ez(CryptoCntxHandle* handle, uint8_t *digest, uint32_t digest_len);

/**
 * @brief This functions sets the Hash paramaters - Mode and Key 
 *        for HMAC.
 *
 * @param cntx       [in] Pointer to cipher context handle
 * @param nParamID   [in] HMAC parameter id to set
 * @param pParam     [in] Pointer to parameter data 
 * @param cParam     [in] Size of parameter data in bytes
 * @param Algo      [in] Algorithm type
 *
 * @return CeMLErrorType 
 *
 */ 

sw_crypto_errno_enum_type qsee_SW_Hash_SetParam (CryptoCntxHandle*   cntx,SW_Auth_Param_Type  nParamID, 
                                                const void         *pParam, 
                                                uint32_t             cParam,  
                                                SW_Auth_Alg_Type   pAlgo);

/**
 * @brief Main function for sha1/sha256 hmac
 *
 * @param key_ptr      [in] Pointer of Key for HMAC
 * @param keylen       [in] key length (16bytes for SHA1 /
 *                          32bytes for SHA256)
 * @param ioVecIn      [in] Input to cipher
 * @param ioVecOut     [in] Output from cipher
 * @param pAlgo        [in] See enum SW_Auth_Alg_Type
 *
 * @return errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hmac (uint8_t * key_ptr, uint32_t keylen, IovecListType ioVecIn,
                              IovecListType * ioVecOut, SW_Auth_Alg_Type  pAlgo);

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param auth_alg     [in] see SW_Auth_Alg_Type
 *
 * @return errno_enum_type
 *
 * @see
 *
 */							  
sw_crypto_errno_enum_type qsee_SW_Hmac_Init(CryptoCntxHandle ** cntx, SW_Auth_Alg_Type  pAlgo);

/**
 * @brief Update function for sha1/sha256 for intermediate data
 *        blocks hash
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param ioVecIn      [in] Input to cipher
 *
 * @return errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hmac_Update(CryptoCntxHandle*  cntx, IovecListType ioVecIn);

/**
 * @brief Sha1/Sha256 last block hash update
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param ioVecOut     [in] Output from cipher
 *
 * @return errno_enum_type
 *
 * @see
 *
 */
 sw_crypto_errno_enum_type qsee_SW_Hmac_Final(CryptoCntxHandle* cntx, IovecListType * ioVecOut);
 
/**
 * @brief deinitialize hmac context
 *
 * @param handle       [in] Pointer of pointer to cipher context
 *
 * @return errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hmac_Deinit(CryptoCntxHandle** cntx);

#endif /* QSEE_UF_SHA_H */
