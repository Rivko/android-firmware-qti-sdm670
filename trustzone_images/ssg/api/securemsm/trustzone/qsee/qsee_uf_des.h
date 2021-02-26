#ifndef QSEE_UF_DES_H
#define QSEE_UF_DES_H
/*===========================================================================

                             Unified Crypto 

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_uf_des.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/16/13   ejt     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdint.h>

/**
 * DES Key and IV sizes
 */
#define SW_DES128_KEY_SIZE        16
#define SW_DES_IV_SIZE            8
#define SW_DES_BLOCK_BYTE_LEN     8    /**< DES cipher block length. */
#define SW_3DES_BLOCK_BYTE_LEN    8    /**< 3DES cipher block length. */
#define SW_DES192_KEY_SIZE        24
#define SW_DES256_KEY_SIZE        32
#define SW_DES_MAX_KEY_SIZE       60

#define SW_3DES_KEY_BYTE_LEN      24   /**< 3DES cipher key length. */
#define SW_DES_KEY_BYTE_LEN       8    /**< DES cipher key length. */

#ifndef IOVECDEF  
typedef  struct 
{
  void    *pvBase;
  uint32_t  dwLen;
} __attribute__((__packed__)) IovecType;

typedef  struct 
{
  IovecType    *iov;
  uint32_t       size;
} __attribute__((__packed__)) IovecListType; 

#define IOVECDEF

#endif

/**
 * Enum for Cipher DES mode
 */
typedef enum 
{
  SW_CIPHER_DES_MODE_ECB          = 0x0,    ///< ECB Mode
  SW_CIPHER_DES_MODE_CBC          = 0x1    ///< CBC Mode
} SW_CipherDESModeType;  

/**
 * Enum for Cipher DES direction
 */
typedef enum 
{
  SW_CIPHER_DES_ENCRYPT           = 0x00,
  SW_CIPHER_DES_DECRYPT           = 0x01,
  SW_CIPHER_DES_MAX               = 0x7FFFFFFF
} SW_CipherDESEncryptDir;

/**
 * Enum for Cipher DES parameters
 */
typedef enum 
{ 
  SW_CIPHER_DES_PARAM_DIRECTION   = 0x01,
  SW_CIPHER_DES_PARAM_KEY         = 0x02,
  SW_CIPHER_DES_PARAM_IV          = 0x03,
  SW_CIPHER_DES_PARAM_MODE        = 0x04,
  SW_CIPHER_DES_PARAM_BLOCKSIZE   = 0x05,
  SW_CIPHER_DES_PARAM_KEY_SIZE    = 0x06,
  SW_CIPHER_DES_PARAM_IV_SIZE     = 0x07,
  SW_CIPHER_DES_PARAM_MAX         = 0x7FFFFFFF
} SW_CipherDESParam;

/**
 * Enum for Cipher DES algorithm type
 */
typedef enum {
  SW_CIPHER_ALG_DES               = 0x0,
  SW_CIPHER_ALG_DES3              = 0x1,
  SW_CIPHER_ALG_DES_MAX           = 0x7FFFFFFF
} SW_Cipher_DES_Alg_Type;

/**
 * Enum for Cipher key size
 */
typedef enum 
{
  SW_CIPHER_KEY_SIZE_DES          = 0x0,
  SW_CIPHER_KEY_SIZE_3DES         = 0x1,
  SW_CIPHER_KEY_SIZE_DES_MAX      = 0x7FFFFFFF
} SW_Cipher_DES_Key_Size;

#ifndef _DEFINED_CRYPTOTOCNTXHANDLE
#define _DEFINED_CRYPTOTOCNTXHANDLE
typedef void CryptoCntxHandle;
#endif

#ifndef SW_CRYPTO_TYPEDEF
#define SW_CRYPTO_TYPEDEF
typedef uint32_t sw_crypto_errno_enum_type;
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

sw_crypto_errno_enum_type qsee_SW_Cipher_DES_Init(CryptoCntxHandle **handle, SW_Cipher_DES_Alg_Type pAlgo);

/**
 * @brief Deintialize a cipher context 
 *
 * @param handle       [in] Pointer to pointer to cipher context
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type qsee_SW_Cipher_DES_DeInit(CryptoCntxHandle  **handle);

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

sw_crypto_errno_enum_type qsee_SW_CipherDESData(CryptoCntxHandle  *handle,
                                                IovecListType      ioVecIn,
                                                IovecListType     *ioVecOut);   

/**
 * @brief This functions sets the Cipher paramaters 
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
sw_crypto_errno_enum_type qsee_SW_Cipher_DES_SetParam(CryptoCntxHandle    *handle,
                                                      SW_CipherDESParam    nParamID,
                                                      const void          *pParam,
                                                      uint32_t               cParam );


#endif /* QSEE_UF_DES_H */

