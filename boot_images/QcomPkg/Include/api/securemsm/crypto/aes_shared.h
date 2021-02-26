#ifndef AES_SHARED_H
#define AES_SHARED_H
/*===========================================================================

                             Unified Crypto

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/securemsm/crypto/aes_shared.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/12   yk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "crypto_env_comdef.h"
//#include "env.h"
#include <Library/CryptoLib/aes/aes_core.h>
#include <Library/CryptoLib/aes/gcm.h>
//#include "ccm.h"

/**
 * AES Key and IV sizes
 */
#define SW_AES128_KEY_SIZE   16
#define SW_AES_IV_SIZE    16
#define SW_AES_BLOCK_BYTE_LEN  16
#define SW_AES192_KEY_SIZE   24
#define SW_AES256_KEY_SIZE   32
#define SW_AES_MAX_IV_SIZE     60


#ifndef IOVECDEF
typedef  struct  {
  void                              *pvBase;
  uint32                            dwLen;
} IovecType/* TODO: have to resolve compiler warning properly __attribute__((packed)) */;

typedef  struct  {
  IovecType                     *iov;
  uint32                            size;
} IovecListType/* TODO: have to resolve compiler warning properly __attribute__((packed)) */;

#define IOVECDEF
#endif


/**
 * Enum for Cipher mode
 */
typedef enum
{
  SW_CIPHER_MODE_ECB          = 0x0,    ///< ECB Mode
  SW_CIPHER_MODE_CBC          = 0x1,    ///< CBC Mode
  SW_CIPHER_MODE_CTR          = 0x2,    ///< CTR Mode
  SW_CIPHER_MODE_GCM          = 0x3,    ///< GCM Mode
  SW_CIPHER_MODE_CCM          = 0x4,    ///< CCM Mode
  SW_CIPHER_MODE_CTS          = 0x5,    ///< CTS Mode
  SW_CIPHER_MODE_MAX          = 0x7FFFFFFF
} SW_CipherModeType;


/**
 * Enum for Cipher direction
 */
typedef enum
{
  SW_CIPHER_ENCRYPT           = 0x00,
  SW_CIPHER_DECRYPT           = 0x01,
  SW_CIPHER_MAX               = 0x7FFFFFFF
} SW_CipherEncryptDir;

/**
 * Enum for Cipher parameters
 */
typedef enum
{
  SW_CIPHER_PARAM_DIRECTION   = 0x01,
  SW_CIPHER_PARAM_KEY         = 0x02,
  SW_CIPHER_PARAM_IV          = 0x03,
  SW_CIPHER_PARAM_MODE        = 0x04,
  SW_CIPHER_PARAM_BLOCKSIZE   = 0x05,
  SW_CIPHER_PARAM_KEY_SIZE    = 0x06,
  SW_CIPHER_PARAM_IV_SIZE     = 0x07,
  SW_CIPHER_PARAM_AAD         = 0x08,     //Additional plaintext data for AES-GCM
  SW_CIPHER_PARAM_TAG         = 0x09,     //Calculated TAG by AES-GCM
  SW_CIPHER_PARAM_NONCE       = 0x10,
  SW_CIPHER_PARAM_PAYLOAD_LEN = 0x11,
  SW_CIPHER_PARAM_MAX         = 0x7FFFFFFF
} SW_CipherParam;

/**
 * Enum for Cipher algorithm type
 */
typedef enum {
  SW_CIPHER_ALG_AES128            = 0x0,
  SW_CIPHER_ALG_AES256            = 0x1,
  SW_CIPHER_ALG_AES192            = 0x2,
  SW_CIPHER_ALG_MAX               = 0x7FFFFFFF
} SW_Cipher_Alg_Type;
/**
 * Enum for Cipher key size
 */
typedef enum
{
  SW_CIPHER_KEY_SIZE_AES128      = 0x0,
  SW_CIPHER_KEY_SIZE_AES256      = 0x1,
  SW_CIPHER_KEY_SIZE_AES192      = 0x2,
  SW_CIPHER_KEY_SIZE_MAX         = 0x7FFFFFFF
} SW_Cipher_Key_Size;

/**
 * Cipher Context definition
 */
typedef struct
{
  SW_Cipher_Alg_Type      algo;
  uint8                    key[SW_AES256_KEY_SIZE];
  uint8                    iv[SW_AES_MAX_IV_SIZE];
  uint8                    nonce[SW_AES_MAX_IV_SIZE];
  SW_Cipher_Key_Size   keysize;
  SW_CipherEncryptDir  dir;
  gcm_ctx                  ctx;
  uint8*                   header_data;
  uint32                   header_data_len;
  gcm_buf_t                cipher_auth;
  gcm_buf_t                header_auth;
  gcm_buf_t                tag;
  SW_CipherModeType    mode;
  boolean_t                  bInit;
  uint32                   payloadLn;
  uint32                   tag_len;
  uint32                   iv_len;
  //ccm_ctx                 ctx_ccm;
  uint32                  init_done;        //if an algo has an init function,
                                            //this flag can be set to indicate
                                            //it has been called, will help with
                                            //update operations
} SW_AES_Ctx;

#ifndef _DEFINED_CRYPTOCNTXHANDLE
#define _DEFINED_CRYPTOCNTXHANDLE
typedef void CryptoCntxHandle;
#endif

/**
 * @brief Intialize a cipher context
 *
 * @param handle       [in] Pointer of pointer to cipher context
 * @param pAlgo        [in] Cipher algorithm type
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */

sw_crypto_errno_enum_type SW_Cipher_Init(CryptoCntxHandle **handle, SW_Cipher_Alg_Type pAlgo);

/**
 * @brief Deintialize a cipher context
 *
 * @param handle       [in] Pointer to pointer to cipher context
 * @param pAlgo        [in] Cipher algorithm type
 *
 * @return errno_enum_type
 *
 * @see
 *
 */

sw_crypto_errno_enum_type SW_Cipher_DeInit(CryptoCntxHandle **handle, SW_Cipher_Alg_Type pAlgo);

/**
 * @brief This function encrypts/decrypts the passed message
 *        using the specified algorithm.
 *
 * @param handle       [in] Pointer of pointer to cipher context
 * @param ioVecIn      [in] Input to cipher
 * @param ioVecOut     [in] Output from cipher
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */

sw_crypto_errno_enum_type SW_CipherData (CryptoCntxHandle        *handle,
                              IovecListType    ioVecIn,
                              IovecListType    *ioVecOut);



/**
 * @brief This functions sets the Cipher paramaters used by
 *        SW_CRYPTO_CipherData
 *
 * @param handle    [in] Pointer of pointer to cipher context
 * @param nParamID  [in] Cipher parameter id to set
 * @param pParam    [in] Pointer to parameter data
 * @param cParam    [in] Size of parameter data in bytes
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see SW_CRYPTO_Cipher_ProcessBlocks
 *
 */
sw_crypto_errno_enum_type SW_Cipher_SetParam (CryptoCntxHandle       *handle,
                                  SW_CipherParam  nParamID,
                                  const void           *pParam,
                                  uint32               cParam );

/**
 * @brief This functions gets the Cipher paramaters created by
 *        SW_CRYPTO_CipherData
 *
 * @param handle    [in] Pointer of pointer to cipher context
 * @param nParamID  [in] Cipher parameter id to get
 * @param pParam      [in] Pointer to parameter data
 * @param cParam    [in] Size of parameter data in bytes
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_Cipher_GetParam (CryptoCntxHandle     *handle,
                                              SW_CipherParam       nParamID,
                                              void                 *pParam,
                                              uint32               cParam );

/**
 * @brief This function updates the Additional Authenticated
 *        Data(AAD) using the specified algorithm.
 *
 * @param handle       [in] Pointer to AE context
 * @param aad          [in] Pointer to AAD
 * @param aad_len      [in] Length of AAD
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_AE_UpdateAAD (CryptoCntxHandle  *handle,
                                            void              *aad,
                                            uint32            aad_len);

/**
 * @brief This function updates the Authenticated Encryption(AE)
 *        operation with the passed message using the specified
 *        algorithm.
 *
 * @param handle       [in] Pointer to AE context
 * @param ioVecIn      [in] Input to AE
 * @param ioVecOut     [in] Output from AE
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_AE_UpdateData (CryptoCntxHandle  *handle,
                                            IovecListType     ioVecIn,
                                            IovecListType     *ioVecOut);

/**
 * @brief This function finalizes the Authenticated
 *        Encryption(AE) operation with the passed message using
 *        the specified algorithm. and computes the tag which
 *        can be obtained by calling SW_Cipher_GetParam
 *
 * @param handle       [in] Pointer to AE context
 * @param ioVecIn      [in] Input to AE
 * @param ioVecOut     [in] Output from AE
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_AE_FinalData (CryptoCntxHandle   *handle,
                                           IovecListType      ioVecIn,
                                           IovecListType      *ioVecOut);

/**
 * @brief Reset cipher context
 *
 * @param handle       [in] pointer to cipher context
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see
 *
 */
sw_crypto_errno_enum_type SW_Cipher_Reset(CryptoCntxHandle *handle);

#endif /* AES_SHARED */
