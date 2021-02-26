#ifndef _CeML
#define _CeML

/*===========================================================================

                    Crypto Engine Module API

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2020 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <stdio.h>
#include <string.h>

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
     ===========================================================================*/

typedef enum
{
    CEML_ERROR_SUCCESS                = 0x0,
    CEML_ERROR_FAILURE                = 0x1,
    CEML_ERROR_INVALID_PARAM          = 0x2,
    CEML_ERROR_NOT_SUPPORTED          = 0x3,
    CEML_ERROR_SHK_NOT_BLOWN          = 0x4
} CeMLErrorType;

typedef struct CeMLHashAlgoCntxType CeMLHashAlgoCntxType;

typedef struct CeMLCipherAlgoCntxType CeMLCipherAlgoCntxType;

typedef struct 
{
  void                              *pvBase; 
  uint32                            dwLen;  
}CEMLIovecType;

typedef struct 
{
  CEMLIovecType                     *iov;                 
  uint32                            size;                 
}CEMLIovecListType;

typedef enum
{
   CEML_HASH_ALGO                    = 0x1,
   CEML_CIPHER_ALGO                  = 0x2
} CeMLAlgoType;

typedef enum
{
    CEML_HASH_ALGO_SHA1               = 0x1,
    CEML_HASH_ALGO_SHA256             = 0x2,
    CEML_HASH_ALGO_SHA384             = 0x3,
    CEML_HASH_ALGO_INVALID            = 0x7FFFFFFF
} CeMLHashAlgoType;

typedef enum
{
    CEML_HASH_MODE_HASH               = 0x0, // Plain SHA
    CEML_HASH_MODE_HMAC               = 0x1,  // HMAC SHA
    CEML_HASH_MODE_INVALID            = 0x7FFFFFFF
} CeMLHashModeType;

typedef enum 
{ 
    CEML_HASH_FIRST                   = 0, 
    CEML_HASH_LAST                    = 1,
    CEML_HASH_SEQ_INVALID             = 0x7FFFFFFF
  } CeMLHashSeqType;
typedef enum 
{ 
    XblSec_CEML_HASH_PARAM_MODE              = 0x01,
    XblSec_CEML_HASH_PARAM_SEQ               = 0x02,
    XblSec_CEML_HASH_PARAM_IV                = 0x03,
    XblSec_CEML_HASH_NONBLOCK_MODE           = 0x04
} XblSec_CeMLHashParamType;

typedef enum 
{ 
    CEML_HASH_PARAM_MODE              = 0x01,
    CEML_HASH_PARAM_HMAC_KEY          = 0x02,
    CEML_HASH_PARAM_AUTH_KEY          = 0x03,
    CEML_HASH_PARAM_SEQ               = 0x04,
    CEML_HASH_PARAM_INVALID           = 0x7FFFFFFF
} CeMLHashParamType;

#define CEML_HASH_DIGEST_SIZE_SHA1    20
#define CEML_HASH_DIGEST_SIZE_SHA256  32
#define CEML_HASH_DIGEST_SIZE_SHA384  48
#define CEML_HASH_DIGEST_BLOCK_SIZE   64
#define CEML_CMAC_DIGEST_SIZE         16
#define CEML_CCM_DIGEST_SIZE          16

#define CEML_CCM_MAC_MIN_LEN 4
#define CEML_CCM_MAC_MAX_LEN 16
#define CEML_NONCE_MIN_LEN   7
#define CEML_NONCE_MAX_LEN   13


typedef enum 
{
    CEML_CIPHER_MODE_ECB              = 0x0,    
    CEML_CIPHER_MODE_CBC              = 0x1,    
    CEML_CIPHER_MODE_CTR              = 0x2,
    CEML_CIPHER_MODE_XTS              = 0x3,     
    CEML_CIPHER_MODE_CCM              = 0x4,     
    CEML_CIPHER_MODE_CTS              = 0x5,
    CEML_CIPHER_MODE_INVALID = 0x6,
} CeMLCipherModeType;

typedef enum 
{
    CEML_CIPHER_ENCRYPT               = 0x00, 
    CEML_CIPHER_DECRYPT               = 0x01 
} CeMLCipherDir;

typedef enum 
{ 
    CEML_BAM_GENERIC        = 0x00,  // All TZ use-cases
    CEML_BAM_CPB_CPB        = 0x01,  // This is for Pattern use-case and other use-cases 
                                     // where scratch memory is used in the app
    CEML_BAM_HLOS_CPB       = 0x02,  // Video DRM Decrypt
    CEML_BAM_CPB_HLOS       = 0x03,  // HDCP Encryption 
    CEML_BAM_INVALID
} CeMLBamPipe;

typedef enum 
{ 
    CEML_CIPHER_PARAM_DIRECTION       = 0x01,
    CEML_CIPHER_PARAM_KEY             = 0x02,
    CEML_CIPHER_PARAM_IV              = 0x03,
    CEML_CIPHER_PARAM_MODE            = 0x04,
    CEML_CIPHER_PARAM_NONCE           = 0x05,
    CEML_CIPHER_PARAM_XTS_KEY         = 0x06,
    CEML_CIPHER_PARAM_XTS_DU_SIZE     = 0x07,
    CEML_CIPHER_PARAM_CCM_PAYLOAD_LEN = 0x08,
    CEML_CIPHER_PARAM_CCM_MAC_LEN     = 0x09,
    CEML_CIPHER_PARAM_HW_KEY_TYPE     = 0x0A,
    CEML_CIPHER_PARAM_CCM_HDR_LEN     = 0x10,
    CEML_CIPHER_PARAM_BAM_PIPE        = 0x11,
    CEML_CIPHER_PARAM_VA_IN           = 0x12,
    CEML_CIPHER_PARAM_VA_IN_LEN       = 0x13,
    CEML_CIPHER_PARAM_VA_OUT          = 0x14,
    CEML_CIPHER_PARAM_VA_OUT_LEN      = 0x15,
    CEML_CIPHER_PARAM_COPY            = 0x16,
    CEML_CIPHER_PARAM_PATT_SIZE       = 0x17,
    CEML_CIPHER_PARAM_BLOCK_OFFSET    = 0x18,
    CEML_CIPHER_PARAM_CTR_SZ          = 0x19,
    CEML_CIPHER_PARAM_INVALID
} CeMLCipherParamType;

#define CEML_AES128_IV_SIZE         16
#define CEML_AES128_KEY_SIZE        16
#define CEML_AES256_IV_SIZE         16
#define CEML_AES256_KEY_SIZE        32
#define CEML_TRIPLE_DES_KEY_SIZE    24
#define CEML_TRIPLE_DES_IV_SIZE     8
#define CEML_HMAC_KEY_SIZE          64
#define CEML_AUTH_KEY_SIZE          32
#define CEML_AES_BLOCK_SIZE         16

typedef enum 
{
    CEML_CIPHER_ALG_AES128            = 0x0,
    CEML_CIPHER_ALG_AES256            = 0x1,
    CEML_CIPHER_ALG_TRIPLE_DES        = 0x2,
    CEML_CIPHER_ALG_INVALID           = 0xFF, 
} CeMLCipherAlgType;

typedef enum
{
    CEML_CIPHER_HW_KEY_OEM            = 0,
    CEML_CIPHER_HW_KEY_QC             = 1
} CeMLCipherHWKeyType;

typedef struct 
{
  void * pClientCtxt;
} CeMLCntxHandle;


/* Define crypto shared data structure */
typedef struct crypto_hash_cipher_shared_info_type
{
  uint32_t shared_data_version;
  CeMLErrorType (*CeMLInit)(void);
  CeMLErrorType (*CeMLDeInit)(void);
  CeMLErrorType (*CeMLHashSetParam)(CeMLCntxHandle *_h,  
                                     XblSec_CeMLHashParamType nParamID, 
                                     const void *pParam, 
                                     uint64 cParam, 
                                     CeMLHashAlgoType pAlgo );
  CeMLErrorType (*CeMLHashGetParam)(CeMLCntxHandle *ceMlHandle,
                                    XblSec_CeMLHashParamType nParamID, 
                                    void *pParam,  
                                    uint32  cParam);
  CeMLErrorType (*CeMLHashInit) (CeMLCntxHandle  ** _h,
                                 CeMLHashAlgoType  pAlgo);
  CeMLErrorType (*CeMLHashDeInit)(CeMLCntxHandle  ** _h);
  CeMLErrorType (*CeMLHashUpdate)(CeMLCntxHandle * _h, CEMLIovecListType ioVecIn); 
  CeMLErrorType (*CeMLHashFinal) (CeMLCntxHandle * _h, CEMLIovecListType *ioVecOut);
  CeMLErrorType (*CeMLHashAtomic)(CeMLCntxHandle  * _h, CEMLIovecListType ioVecIn,
                                  CEMLIovecListType *ioVecOut);
  CeMLErrorType (*CeMLCipherInit)(CeMLCntxHandle **_h,
                                  CeMLCipherAlgType  pAlgo);
  CeMLErrorType (*CeMLCipherDeInit)(CeMLCntxHandle ** _h);
  CeMLErrorType (*CeMLCipherSetParam)(CeMLCntxHandle *_h,
                                      CeMLCipherParamType  nParamID,
                                      const void  *pParam,
                                      uint32   cParam );
  CeMLErrorType (*CeMLCipherGetParam)(CeMLCntxHandle *_h,
                                      CeMLCipherParamType  nParamID, 
                                      const void  *pParam,
                                      uint32   *cParam );
  CeMLErrorType (*CeMLCipherData)(CeMLCntxHandle *_h,
                                   CEMLIovecListType ioVecIn, 
                                   CEMLIovecListType *ioVecOut);
  CeMLErrorType (*CeMLHashCipherData)(CeMLCntxHandle *_h1,
                                      CeMLCntxHandle *_h2,
                                      CEMLIovecListType  ioVecIn,
                                      CEMLIovecListType  * ioVecOut,
                                      uint8*  hash_out, uint64   hash_out_len);

}crypto_hash_cipher_shared_info_type;

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
CeMLInit            (void);

/**
 * @brief This function deinitializes the CE
 *
 * @param void
 *
 * @return CeMLErrorType
 *
 * @return CeMLErrorType
 *
 * @see 
 */

CeMLErrorType 
CeMLDeInit          (void);

/**
 * @brief This function resets the state of the cipher operation
 *
 * @param void
 *
 * @return CeMLErrorType
 *
 * @return CeMLErrorType
 *
 * @see 
 */
CeMLErrorType CeMLCipherReset(CeMLCntxHandle *ceMlHandle);


/**
 * @brief This function resets the state of the hash operation
 *
 * @param void
 *
 * @return CeMLErrorType
 *
 * @return CeMLErrorType
 *
 * @see 
 */
CeMLErrorType CeMLHashReset(CeMLCntxHandle *ceMlHandle);

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
 * @brief This function will create a Cmac message digest using 
 *        the algorithm specified.
 *
 * @param key_ptr       [in]  Pointer to key 
 * @param keylen        [in]  Length of input key in bytes 
 * @param ioVecIn       [in]  Pointer to input data to hash
 * @param ioVecOut      [out] Pointer to output data 
 * @param pAlgo         [in]  Algorithm type
 *
 * @return CeMLErrorType
 *
 * @see 
 *
 */

CeMLErrorType 
CeMLCmac            (uint8                *key_ptr, 
                     uint32               keylen,
                     CEMLIovecListType    ioVecIn,
                     CEMLIovecListType    *ioVecOut,
                     CeMLCipherAlgType    pAlgo);

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
 * @brief This functions sets the Hash paramaters for uint32 types
 *
 * @param _h        [in] Pointer to cipher context handle
 * @param nParamID  [in] Cipher parameter id to set
 * @param pParam    [in] Pointer to parameter data 
 *
 * @return CeMLErrorType
 *
 */ 

CeMLErrorType CeMLHashSetParamAsU32(CeMLCntxHandle *_h,
                                    CeMLHashParamType  nParamID,
                                    const  void                *pParam);


/**
 * @brief This functions sets the Hash paramaters for data types
 *
 * @param _h        [in] Pointer to cipher context handle
 * @param nParamID  [in] Cipher parameter id to set
 * @param pParam    [in] Pointer to parameter data 
 * @param cParam    [in] Size of parameter data in bytes
 * @param palgo     [in]  Algorithm type
 *
 * @return CeMLErrorType
 */ 

CeMLErrorType CeMLHashSetParamAsData(CeMLCntxHandle    *_h,
                                       CeMLHashParamType  nParamID,
                                       const void                *pParam,
                                       size_t              cParam,
                                       CeMLHashAlgoType   pAlgo);
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
 *
 * @return CeMLErrorType
 *
 * @see CeMLCipherData 
 *
 */
CeMLErrorType CeMLCipherSetParamAsU32(CeMLCntxHandle *_h, 
	                                                                        CeMLCipherParamType  nParamID, 
	                                                                        void *pParam);
/**
 * @brief This functions sets the Cipher paramaters used by 
 *        CeMLCipherData
 *
 * @param _h             [in] Pointer to cipher context handle
 * @param nParamID  [in] Cipher parameter id to set
 * @param pParam     [in] Pointer to parameter data 
 * @param cParam     [in] Size of parameter data in bytes
 *
 * @return CeMLErrorType
 *
 * @see CeMLCipherData 
 *
 */
CeMLErrorType CeMLCipherSetParamAsData(CeMLCntxHandle    *_h, 
	                                                                         CeMLCipherParamType  nParamID, 
	                                                                         const void *pParam, 
	                                                                         size_t cParam);
/**
 * @brief This functions gets the Cipher paramaters used by 
 *        CeMLCipherData
 *
 * @param _h             [in] Pointer to cipher context handle
 * @param nParamID  [in] Cipher parameter id to get
 * @param pnOut        [out] Pointer to parameter data 
 *
 * @return CeMLErrorType
 *
 * @see CeMLCipherData 
 *
 */
CeMLErrorType CeMLCipherGetParamAsU32(CeMLCntxHandle *_h, 
	                                                                        CeMLCipherParamType  nParamID, 
	                                                                        uint32 *pnOut);
/**
 * @brief This functions gets the Cipher paramaters used by 
 *        CeMLCipherData
 *
 * @param _h             [in] Pointer to cipher context handle
 * @param nParamID  [in] Cipher parameter id to get
 * @param pParam     [in] Pointer to parameter data 
 * @param cParam     [in] Size of parameter data in bytes
 *
 * @return CeMLErrorType
 *
 * @see CeMLCipherData 
 *
 */
CeMLErrorType CeMLCipherGetParamAsData(CeMLCntxHandle    *_h, 
	                                                                          CeMLCipherParamType  nParamID, 
	                                                                          void *pParam, 
	                                                                          size_t *cParam);


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

CeMLErrorType CeMLCipherSetParam (CeMLCntxHandle       * ceMlHandle,
                                  CeMLCipherParamType  nParamID, 
                                  void                 *pParam, 
                                  size_t               cParam);
 
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

CeMLErrorType  CeMLCipherGetParam    (CeMLCntxHandle       * _h,
                       CeMLCipherParamType  nParamID, 
                       void                  *pParam, 
                       size_t                *cParam );

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

/**
 * @brief This function will init for a Hmac message digest using 
 *        the algorithm specified.
 *
 * @param cntx          [in]  Pointer to pointer for hmac context
 * @param palgo         [in]  Algorithm type
 *
 * @return CeMLErrorType
 *
 * @see 
 *
 */

CeMLErrorType CeMLHmacInit(CeMLCntxHandle**  cntx,
                           CeMLHashAlgoType    pAlgo);

/**
 * @brief This function will update a Hmac message digest using 
 *        the algorithm specified. It is callable multiple times
 *
 * @param cntx          [in]  Pointer to hmac context
 * @param ioVecIn       [in]  Pointer to input data to hmac
 *
 * @return CeMLErrorType
 *
 * @see 
 *
 */
CeMLErrorType CeMLHmacUpdate(CeMLCntxHandle* cntx,
                             CEMLIovecListType   ioVecIn);


/**
 * @brief This function will finalize a Hmac message digest using 
 *        the algorithm specified.
 *
 * @param cntx          [in]  Pointer to hmac context
 * @param ioVecOut      [out] Pointer to output data 
 *
 * @return CeMLErrorType
 *
 * @see 
 *
 */
CeMLErrorType CeMLHmacFinal(CeMLCntxHandle* cntx,
                            CEMLIovecListType  *ioVecOut);

/**
 * @brief This function performs a Hash/Cipher operation on the 
 *        passed message using the specified algorithm.
 *
 * @param _h1          [in] Pointer to cipher context handle
 * @param _h2          [in] Pointer to hash context handle
 * @param ioVecIn      [in] Pointer to input data. Input data 
 *                     length must be a multiple of 16 bytes
 * @param ioVecOut     [in] Pointer to output data
 * @param hash_out     [out] Pointer to hash output
 * @param hash_out_len [in] hash output length
 
 *
 * @return CeMLErrorType
 *
 */

CeMLErrorType 
CeMLHashCipherData(CeMLCntxHandle     *_h1, 
                   CeMLCntxHandle     *_h2,
                   CEMLIovecListType  ioVecIn,
                   CEMLIovecListType  * ioVecOut,
                   uint8              * hash_out,
                   uint32             hash_out_len);


#endif
