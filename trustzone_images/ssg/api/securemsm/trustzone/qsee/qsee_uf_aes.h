#ifndef QSEE_UF_AES_H
#define QSEE_UF_AES_H

/**
@file qsee_uf_aes.h
@brief Provide Software crypto Cipher API wrappers
*/

/*===========================================================================
Copyright (c) {2012, 2017} Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
06/15/17   ss        addition of ccm mode nonce value
12/14/16   ts        addition of XTS mode values
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
 * AES Key and IV sizes
 */
#define SW_AES128_KEY_SIZE   16
#define SW_AES_IV_SIZE    16
#define SW_AES_BLOCK_BYTE_LEN  16
#define SW_AES192_KEY_SIZE   24
#define SW_AES256_KEY_SIZE   32
#define SW_AES_MAX_KEY_SIZE  60
#define SW_AES_MAX_IV_SIZE   60
#define SW_AES_CCM_MIN_NONCE_SIZE  7 
#define SW_AES_CCM_MAX_NONCE_SIZE  13

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

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
  SW_CIPHER_PARAM_XTS_DU_SIZE = 0x12,
  SW_CIPHER_PARAM_XTS_KEY     = 0x13,
  SW_CIPHER_PARAM_XTS_TWK_SET = 0x14,
  SW_CIPHER_PARAM_MAX         = 0x7FFFFFFF
} SW_CipherParam;


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
 * Enum for Cipher mode
 */
typedef enum 
{
  SW_CIPHER_MODE_ECB          = 0x0,    ///< ECB Mode   
  SW_CIPHER_MODE_CBC          = 0x1,    ///< CBC Mode   
  SW_CIPHER_MODE_CTR          = 0x2,     ///< CTR Mode   
  SW_CIPHER_MODE_GCM          = 0x3,    ///< GCM Mode 
  SW_CIPHER_MODE_CCM          = 0x4,    ///< CCM Mode 
  SW_CIPHER_MODE_CTS          = 0x5,    ///< CCM Mode
  SW_CIPHER_MODE_XTS          = 0x6,    ///< XTS Mode
  SW_CIPHER_MODE_INVALID      = 0x7FFFFFFF
} SW_CipherModeType;

/**
 * @brief Initialize a cipher context
 *
 * @param handle       [in] Pointer of pointer to cipher context
 * @param pAlgo        [in] Cipher algorithm type 
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type qsee_SW_Cipher_Init(CryptoCntxHandle **handle, SW_Cipher_Alg_Type pAlgo);

/**
 * @brief Reset a cipher context except keys
 *
 * @param handle       [in] Pointer of pointer to cipher context
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type qsee_SW_Cipher_Reset(CryptoCntxHandle *handle);

/**
 * @brief This function encrypts/decrypts the passed message
 *        using the specified algorithm.
 *
 * @param handle       [in] Pointer of pointer to cipher context
 * @param ioVecIn      [in] Input to cipher
 * @param ioVecOut     [in] Output from cipher
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type qsee_SW_CipherData (CryptoCntxHandle *handle, IovecListType ioVecIn, 
                                              IovecListType *ioVecOut);

/**
 * @brief This functions sets the Cipher paramaters used by 
 *        SW_CRYPTO_CipherData
 *
 * @param handle    [in] Pointer of pointer to cipher context
 * @param nParamID  [in] Cipher parameter id to set
 * @param pParam    [in] Pointer to parameter data 
 * @param cParam    [in] Size of parameter data in bytes
 *
 * @return errno_enum_type
 *
 * @see  
 *
 */
sw_crypto_errno_enum_type qsee_SW_Cipher_SetParam (CryptoCntxHandle *handle, SW_CipherParam nParamID,
                                                   const void *pParam, uint32_t cParam );


/**
 * @brief This functions gets the Cipher paramaters created by 
 *        SW_CRYPTO_CipherData
 *
 * @param handle    [in] Pointer of pointer to cipher context
 * @param nParamID  [in] Cipher parameter id to get
 * @param pParam    [in] Pointer to parameter data 
 * @param cParam    [in] Size of parameter data in bytes
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see  
 *
 */
sw_crypto_errno_enum_type qsee_SW_Cipher_GetParam (CryptoCntxHandle *handle, SW_CipherParam nParamID,
                                                   void *pParam, uint32_t cParam );

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

sw_crypto_errno_enum_type qsee_SW_Cipher_DeInit(CryptoCntxHandle **handle, SW_Cipher_Alg_Type pAlgo);


sw_crypto_errno_enum_type qsee_SW_AE_UpdateData(CryptoCntxHandle *handle, IovecListType ioVecIn,
                                                IovecListType *ioVecOut);


sw_crypto_errno_enum_type qsee_SW_AE_FinalData(CryptoCntxHandle *handle, IovecListType ioVecIn,
                                                IovecListType *ioVecOut);


sw_crypto_errno_enum_type qsee_SW_AE_UpdateAAD(CryptoCntxHandle *handle, void *aad, 
                                               uint32_t aadlen);


#endif /* QSEE_UF_AES_H */
