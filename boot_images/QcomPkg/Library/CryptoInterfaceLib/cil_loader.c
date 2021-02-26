/*===========================================================================

                    BOOT EXTERN ENCRYPTION DEFINITIONS

DESCRIPTION
  Crypto Interface Library API's for XBL Loader

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2017,2018 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/17   aus     Updated to support minidump
25/04/17   bd      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/


#include <crypto/aes_shared.h>
#include <crypto/secmath.h>
#include <crypto/secrsa.h>

#include <xbl_minidump_oem_public_key.h>
#include <boot_extern_crypto_interface.h>
#include "boot_logger.h"
#include <cil_loader.h>
#include "LoaderUtils.h"

  
/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

static crypto_env_mem GCMLib_Heap[XBL_CIPHER_MAX_MEM];
static raw_key_t raw_key = {0};

/*===========================================================================
**  Function :  boot_sw_cipher_init
** ==========================================================================
*/
/**
 * @brief Initialize crypto driver
 *
 */
void boot_sw_cipher_init (void)
{
  crypto_env_init(GCMLib_Heap, XBL_CIPHER_MAX_MEM);
}

/*===========================================================================
**  Function :  boot_sw_cipher_post_init
** ==========================================================================
*/
/**
 * @brief Post crypto driver initialization routines
 *
 */
void boot_sw_cipher_post_init (void)
{
  crypto_bam_enable(); 
}

/*===========================================================================
**  Function :  boot_oem_get_pub_key
** ==========================================================================
*/
/**
 * @brief Update the OEM public key
 *
 */
void boot_oem_get_pub_key(char** pub_mod_ptr, uint32* pub_mod_len, char** pub_exp_ptr, uint32* pub_exp_len)
{
  *pub_mod_ptr = xbl_minidump_oem_public_key.n;
  *pub_exp_ptr = xbl_minidump_oem_public_key.e;
  *pub_mod_len = xbl_minidump_oem_public_key.bitLength/8;
  *pub_exp_len = xbl_minidump_oem_public_key.bitLength/8;  
}

/*===========================================================================
**  Function :  boot_encr_aes_key
** ==========================================================================
*/
/**
 * @brief Encrypt aes key
 *
 * @param in [in]      pointer to input aes key
 * @param inlen [in]   length of the input key
 * @param out [in]     pointer to output buffer to store encrypted key 
 * @param out_len [in]  output buffer length
 *
 * @return  NA
 *
 */
void boot_encr_aes_key (uint8 *in, uint32 inlen, uint8 *out, uint32 *outlen)
{
  CE_RSA_KEY               key;
  CE_RSA_OAEP_PAD_INFO     oaep_pad_info = {0};
  CE_RSA_PADDING_TYPE      padding_type  = CE_RSA_PAD_PKCS1_OAEP;
  void                    *padding_info  = NULL;
  char                   *mod = NULL;
  char                   *exp = NULL;
  uint32                   mod_len = 0;
  uint32                   exp_len = 0;  
  size_t                   keyByteLength;
  uint8                    key_e[sizeof(S_BIGINT)];
  uint8                    key_N[sizeof(S_BIGINT)];

  boot_oem_get_pub_key(&mod,&mod_len,&exp,&exp_len);

  raw_key.n = (char*)mod;
  raw_key.e = (char*)exp;

  //oaep_pad_info.hashidx  = CE_HASH_IDX_SHA1;   
  oaep_pad_info.hashidx  = CE_HASH_IDX_SHA256;   
  oaep_pad_info.label    = NULL;
  oaep_pad_info.labellen = 0;
  padding_info = (void*)&oaep_pad_info;

  memset(&key, 0, sizeof(CE_RSA_KEY));
  raw_key.bitLength =2048;
  keyByteLength = raw_key.bitLength/8;

  key.e = ( S_BIGINT *)key_e;

  if( secmath_BIGINT_read_radix(&(key.e)->bi, raw_key.e, 16) != 0)
  {
    boot_log_message("No public key provisioned \n");
    memset(out, 0, *outlen);
    return;
  }

  (key.e)->bi.n = keyByteLength/sizeof(BLONG);
  (key.e)->sign = S_BIGINT_POS; // Positive

  key.N = ( S_BIGINT *)key_N;

  if( secmath_BIGINT_read_radix(&(key.N)->bi, raw_key.n, 16) != 0)
  {
    return;
  }

  (key.N)->bi.n = keyByteLength/sizeof(BLONG);
  (key.N)->sign = S_BIGINT_POS; // Positive

  key.type = CE_RSA_KEY_PUBLIC;
  key.bitLength = raw_key.bitLength;
  ce_rsa_encrypt(&key, padding_type, padding_info, in, inlen, out, (int*)outlen);    
}

/*===========================================================================
**  Function :  boot_check_oem_key
** ==========================================================================
*/
/**
 * @brief Check if the OEM key is valid
 *
 */
boolean boot_check_oem_key(void)
{
  if(xbl_minidump_oem_public_key.n == NULL) 
	  return FALSE;
  else 
	  return TRUE;
}

/*===========================================================================
**  Function :  boot_sw_cipher
** ==========================================================================
*/
/**
 * @brief Encrypt the given buffer
 *
 * @param in [in]      pointer to input data
 * @param in_len [in]  pointer to input data length
 * @param out [in]     pointer to output encrypted data
 * @param out_len [in] pointer to output buffer length
 * @param key [in]     AES key
 * @param key_len [in]  AES key length
 * @param nonce [in]     IV key
 * @param nonce_len [in]  IV key length
 * @param tag [out]       TAG output from crypto driver   
 * @param tag_len [in]    TAG buffer length
 * @param dir [in]        dir; false: encrypt, true: decrypt
 *
 * @return  NA
 *
 */
int boot_sw_cipher
(   
   void   *in, 
   size_t  in_len, 
   void   *out, 
   size_t  out_len,
   void   *key_aes, 
   size_t  key_aes_len,
   void   *key_hmac, 
   size_t  key_hmac_len,
   void   *nonce, 
   size_t  nonce_len,
   void   *tag,
   size_t  tag_len,
   boolean    dir                    //false: encrypt, true: decrypt
)
{
  CeMLErrorType                    ret = CEML_ERROR_FAILURE;
  CeMLCntxHandle*        cntx = NULL;
  CEMLIovecListType          ioVecIn;
  CEMLIovecListType          ioVecOut;
  CEMLIovecType              IovecIn;
  CEMLIovecType              IovecOut;
  CeMLCipherDir    sw_dir;
  CeMLCipherAlgType     alg  = CEML_CIPHER_ALG_AES256;
  CeMLCipherModeType      mode = CEML_CIPHER_MODE_CTR;

   
  do
  {
    if( !in  || !out  || !key_aes || !key_hmac || !nonce  || !tag )
    {
      ret = CEML_ERROR_FAILURE;
      break;
    }     
 
    if((ret = CeMLInit()) != 0)
    {
      ret = CEML_ERROR_FAILURE;
      break;
    }    

    if((ret = CeMLCipherInit(&cntx, alg)) != 0)
    {
      ret = CEML_ERROR_FAILURE;
      break;
    }

    if( dir == FALSE)
       sw_dir = CEML_CIPHER_ENCRYPT;
    else
       sw_dir = CEML_CIPHER_DECRYPT;
    
     //Set parameters so we can do the encrypt
    if( (ret = CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_MODE,       &mode,   sizeof(CeMLCipherModeType)))   != 0  ||
        (ret = CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_DIRECTION,  &sw_dir, sizeof(CeMLCipherDir))) != 0  ||
        (ret = CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_KEY,        key_aes,     key_aes_len))                     != 0  ||
        (ret = CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_IV,         nonce,   nonce_len))                    != 0 /*TODO: memory for this shall be of 16bytes */
      )
    {  
      ret = CEML_ERROR_FAILURE;
      goto DEINIT;
    }
    // Input IOVEC 
    ioVecIn.size = 1;
    ioVecIn.iov = &IovecIn;
    ioVecIn.iov[0].dwLen  = in_len;
    ioVecIn.iov[0].pvBase = in; 
    // Output IOVEC
    ioVecOut.size = 1;
    ioVecOut.iov = &IovecOut;
    ioVecOut.iov[0].dwLen  = out_len;
    ioVecOut.iov[0].pvBase = out;

    //Now encrypt the data
    if((ret = CeMLCipherData(cntx, ioVecIn, &ioVecOut)) != 0)
    {
      ret = CEML_ERROR_FAILURE;
      goto DEINIT;
    }
  
     // Input IOVEC 
    ioVecIn.size = 1;
    ioVecIn.iov = &IovecIn;
    ioVecIn.iov[0].dwLen  = out_len;
    ioVecIn.iov[0].pvBase = out; 
	
    // Output IOVEC
    ioVecOut.size = 1;
    ioVecOut.iov = &IovecOut;
    ioVecOut.iov[0].dwLen  = tag_len;
    ioVecOut.iov[0].pvBase = tag;

    CeMLHmac(key_hmac, key_hmac_len, ioVecIn, &ioVecOut, CEML_HASH_ALGO_SHA256);
    /*if((ret = SW_Cipher_GetParam(cntx, SW_CIPHER_PARAM_TAG, tag, tag_len)) != 0)
    {
      ret = CEML_ERROR_FAILURE;
      goto DEINIT;
    }*/

DEINIT:
    if(CeMLCipherDeInit(&cntx) != 0)
    {
      ret = CEML_ERROR_FAILURE;
    }
    CeMLDeInit();
    
  }while(0);

  return (int)ret;
}

