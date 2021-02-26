#ifndef CE_ML_H
#define CE_ML_H
/*===========================================================================

                    Crypto Engine Module API

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.qdsp6/2.1/api/securemsm/crypto/CeML.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/10   bm      initial version
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/

typedef enum
{
  CEML_ERROR_SUCCESS                = 0x0,
  CEML_ERROR_FAILURE                = 0x1,
  CEML_ERROR_INVALID_PARAM          = 0x2,
  CEML_ERROR_NOT_SUPPORTED          = 0x3
} CeMLErrorType;

typedef PACKED struct 
{
  void                              *pvBase; 
  uint32                            dwLen;  
} CEMLIovecType;

typedef PACKED struct 
{
  CEMLIovecType                     *iov;                 
  uint32                            size;                 
} CEMLIovecListType;

typedef enum
{
  CEML_HASH_ALGO                    = 0x1,
  CEML_CIPHER_ALGO                  = 0x2
} CeMLAlgoType;

typedef enum
{
  CEML_HASH_ALGO_SHA1               = 0x1,
  CEML_HASH_ALGO_SHA256             = 0x2
} CeMLHashAlgoType;

typedef enum
{
  CEML_HASH_MODE_HASH               = 0x0, // Plain SHA
  CEML_HASH_MODE_HMAC               = 0x1  // HMAC SHA
} CeMLHashModeType;

typedef enum 
{ 
  CEML_HASH_PARAM_MODE              = 0x01,
  CEML_HASH_PARAM_HMAC_KEY          = 0x02
} CeMLHashParamType;

#define CEML_HASH_DIGEST_SIZE_SHA1    20
#define CEML_HASH_DIGEST_SIZE_SHA256  32
#define CEML_HASH_DIGEST_BLOCK_SIZE   64

typedef enum 
{
  CEML_CIPHER_MODE_ECB              = 0x0,    
  CEML_CIPHER_MODE_CBC              = 0x1,    
  CEML_CIPHER_MODE_CTR              = 0x2,
  CEML_CIPHER_MODE_XTS              = 0x3,     
  CEML_CIPHER_MODE_CCM              = 0x4,     
  CEML_CIPHER_MODE_CMAC             = 0x5     
} CeMLCipherModeType;

typedef enum 
{
  CEML_CIPHER_ENCRYPT               = 0x00, 
  CEML_CIPHER_DECRYPT               = 0x01 
} CeMLCipherDir;

typedef enum 
{ 
  CEML_CIPHER_PARAM_DIRECTION       = 0x01,
  CEML_CIPHER_PARAM_KEY             = 0x02,
  CEML_CIPHER_PARAM_IV              = 0x03,
  CEML_CIPHER_PARAM_MODE            = 0x04,
  CEML_CIPHER_PARAM_NONCE           = 0x05,
  CEML_CIPHER_PARAM_XTS_KEY         = 0x06,
  CEML_CIPHER_PARAM_XTS_DU_SIZE     = 0x07,
  CEML_CIPHER_PARAM_CCM_PAYLOAD_LEN = 0x08
} CeMLCipherParamType;

#define CEML_AES128_IV_SIZE         16
#define CEML_AES128_KEY_SIZE        16
#define CEML_AES256_IV_SIZE         16
#define CEML_AES256_KEY_SIZE        32
#define CEML_HMAC_KEY_SIZE          64

typedef enum 
{
  CEML_CIPHER_ALG_AES128            = 0x0,
  CEML_CIPHER_ALG_AES256            = 0x1
} CeMLCipherAlgType;

typedef PACKED struct 
{
  void * pClientCtxt;
}  CeMLCntxHandle;

/**
 * @brief This function initializes the CE
 *
 * @param void
 *
 * @return CeMLErrorType
 *
 * @see 
 *
 */

CeMLErrorType 
CeMLInit ( void );

/**
 * @brief This function deinitializes the CE
 *
 * @param void
 *
 * @return CeMLErrorType
 *
 * @see 
 *
 */

CeMLErrorType 
CeMLDeInit ( void );

/**
 * @brief Intialize a hash context for Hash update and final functions
 *
 * @param _h      [in] Pointer to a pointer to the hash context
 * @param pAlgo   [in] Algorithm type
 *
 * @return CeMLErrorType
 *
 * @see CeMLHashUpdate and CeMLHashfinal
 *
 */

CeMLErrorType 
CeMLHashInit        (CeMLCntxHandle       ** _h, 
                     CeMLHashAlgoType     pAlgo);

/**
 * @brief Deintialize a hash context  
 *
 * @param _h      [in] Pointer to a pointer to the hash context
 *
 * @return CeMLErrorType
 *
 * @see CeMLDeInit
 *
 */

CeMLErrorType 
CeMLHashDeInit      (CeMLCntxHandle       ** _h);

/**
 * @brief This function will hash data into the hash context
 *        structure, which must have been initialized by
 *        CeMLHashInit.
 *
 * @param _h          [in] Pointer to Hash context
 * @param ioVecIn     [in] Pointer to input message to be
 *                     hashed
 * @return CeMLErrorType 
 *
 * @see CeMLHashInit 
 *
 */

CeMLErrorType 
CeMLHashUpdate      (CeMLCntxHandle       * _h, 
                     CEMLIovecListType    ioVecIn); 

/**
 * @brief Compute the final digest hash value.
 *
 * @param _h         [in] Pointer to Hash context
 * @param ioVecOut   [out] Pointer to output digest
 
 * @return CeMLErrorType 
 *
 * @see CeMLHashInit
 *
 */

CeMLErrorType 
CeMLHashFinal       (CeMLCntxHandle       * _h,
                     CEMLIovecListType    * ioVecOut);

/**
 * @brief This function will hash data into the hash context
 *        structure and compute the final digest hash value.
 *
 * @param _h       [in] Pointer to Hash context 
 * @param ioVecIn  [in] Pointer to input message to be hashed 
 * @param ioVecOut [Out] Pointer to output digest
 *
 * @return CeMLErrorType  
 *
 * @see CeMLHashInit
 *
 */

CeMLErrorType 
CeMLHashAtomic      (CeMLCntxHandle       * _h, 
                     CEMLIovecListType    ioVecIn,
                     CEMLIovecListType    * ioVecOut);

/**
 * @brief This function will create a Hmac message digest using 
 *        the algorithm specified.
 *
 * @param key_ptr       [in]  Pointer to key 
 * @param keylen        [in]  Length of input key in bytes
 * @param ioVecIn       [in]  Pointer to input data to hash
 * @param ioVecOut      [out] Pointer to output data 
 * @param palgo         [in]  Algorithm type
 *
 * @return CeMLErrorType
 *
 * @see 
 *
 */

CeMLErrorType 
CeMLHmac            (uint8                * key_ptr, 
                     uint32               keylen, 
                     CEMLIovecListType    ioVecIn,
                     CEMLIovecListType    * ioVecOut,
                     CeMLHashAlgoType     pAlgo);

/**
 * @brief This functions sets the Hash paramaters - Mode and Key for HMAC
 *
 * @param _h        [in] Pointer to cipher context handle
 * @param nParamID  [in] Cipher parameter id to set
 * @param pParam    [in] Pointer to parameter data 
 * @param cParam    [in] Size of parameter data in bytes
 * @param palgo     [in]  Algorithm type
 *
 * @return CeMLErrorType
 *
 */

CeMLErrorType  
CeMLHashSetParam    (CeMLCntxHandle       * _h,
                     CeMLHashParamType    nParamID, 
                     const void           *pParam, 
                     uint32               cParam,
                     CeMLHashAlgoType     pAlgo );

/**
 * @brief Intialize a cipher context 
 *
 * @param _h       [in] Pointer to a pointer to the cipher 
 *                 context structure
 * @param pAlgo    [in] Cipher algorithm type 
 *
 * @return CeMLErrorType
 *
 * @see 
 *
 */

CeMLErrorType 
CeMLCipherInit      (CeMLCntxHandle       ** _h, 
                     CeMLCipherAlgType    pAlgo);

/**
 * @brief Deintialize a cipher context 
 *
 * @param _h       [in] Pointer to a pointer to the cipher 
 *                 context structure
 * @return CeMLErrorType
 *
 * @see 
 *
 */

CeMLErrorType 
CeMLCipherDeInit    (CeMLCntxHandle       ** _h);

/**
 * @brief This functions sets the Cipher paramaters used by 
 *        CeMLCipherData
 *
 * @param _h        [in] Pointer to cipher context handle
 * @param nParamID  [in] Cipher parameter id to set
 * @param pParam    [in] Pointer to parameter data 
 * @param cParam    [in] Size of parameter data in bytes
 *
 * @return CeMLErrorType
 *
 * @see CeMLCipherData 
 *
 */

CeMLErrorType  
CeMLCipherSetParam    (CeMLCntxHandle       * _h,
                       CeMLCipherParamType  nParamID, 
                       const void           *pParam, 
                       uint32               cParam );
 
/**
 * @brief This functions gets the Cipher paramaters used by 
 *        CeMLCipherData
 *
 * @param _h        [in] Pointer to cipher context handle
 * @param nParamID  [in]  Cipher parameter id to get
 * @param pParam    [out] Pointer to parameter data 
 * @param pcParam   [out] Pointer to size of data 
 *
 * @return CeMLErrorType
 *
 * @see CeMLCipherData
 *
 */

CeMLErrorType  
CeMLCipherGetParam    (CeMLCntxHandle       * _h,
                       CeMLCipherParamType  nParamID, 
                       const void           *pParam, 
                       uint32               *cParam );

/**
 * @brief This function encrypts/decrypts the passed message
 *        using the specified algorithm.
 *
 * @param _h        [in] Pointer to cipher context handle
 * @param ioVecIn   [in] Pointer to input data. Input data 
 *                  length must be a multiple of 16 bytes
 * @param ioVecOut  [in] Pointer to output data
 *
 * @return CeMLErrorType
 *
 * @see  
 *
 */

CeMLErrorType 
CeMLCipherData      (CeMLCntxHandle       * _h, 
                     CEMLIovecListType    ioVecIn,
                     CEMLIovecListType    * ioVecOut);

#endif //CE_ML_H
