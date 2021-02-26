#ifndef SHA_SHARED_H
#define SHA_SHARED_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/securemsm/crypto/sha_shared.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/10   yk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "crypto_env_comdef.h"
//#include "secrsa_env.h"
#include <Library/CryptoLib/sha/sha_core.h>

/**
 * SHA-2 Context definition
 */


typedef enum
{
  SW_AUTH_ALG_SHA1               = 0x1,
  SW_AUTH_ALG_SHA256,             
  SW_AUTH_ALG_INVALID            = 0x7FFFFFFF
} SW_Auth_Alg_Type;

typedef struct 
{
	uint32  counter[2];
	uint32  iv[16];
	uint32  auth_iv[8]; 
	uint8   leftover[SW_SHA_BLOCK_SIZE];
	uint32  leftover_size;
	SW_Auth_Alg_Type auth_alg;
} SW_SHA_Ctx;

#ifndef CRYPTOCNTXHANDLE
#define CRYPTOCNTXHANDLE
typedef void CryptoCntxHandle;
#endif

#ifndef IOVECDEF
typedef struct __attribute__((packed)){
  void                              *pvBase; 
  uint32                            dwLen;  
} IovecType;

typedef  struct __attribute__((packed)){
  IovecType                     *iov;                 
  uint32                            size;                 
} IovecListType;
#define IOVECDEF
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Init(CryptoCntxHandle **handle, SW_Auth_Alg_Type auth_alg); 

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Update(CryptoCntxHandle *handle,IovecListType ioVecIn);

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Final (CryptoCntxHandle *handle,IovecListType *ioVecOut);

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
sw_crypto_errno_enum_type SW_Hash_Update_Ez(CryptoCntxHandle* handle, const uint8_t *msg, uint32_t msg_len);

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
sw_crypto_errno_enum_type SW_Hash_Final_Ez(CryptoCntxHandle* handle, uint8_t *digest, uint32_t digest_len);


/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
//sw_crypto_errno_enum_type SW_Hmac (uint8 * key_ptr, uint32 keylen, IovecListType ioVecIn, IovecListType * ioVecOut, SW_Auth_Alg_Type  pAlgo);

sw_crypto_errno_enum_type  SW_Hash
(
   SW_Auth_Alg_Type   alg,
   const uint8_t     *msg,
   uint32_t           msg_len,
   uint8_t           *digest,
   uint32_t           digest_len
);

#endif /* AES_SHARED */
