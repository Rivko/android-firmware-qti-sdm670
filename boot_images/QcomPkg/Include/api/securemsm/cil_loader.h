#ifndef _CIL_LOADER_H
#define _CIL_LOADER_H
/*===========================================================================

                   Crypto Interface Library API's for XBL Loader

DESCRIPTION
  Contains wrapper definition Crypto Interface Library

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
#include "boot_comdef.h"
#include "PrngML.h"

#define XBL_CIPHER_MAX_MEM  1 

/*===========================================================================
**  Function :  boot_sw_cipher_init
** ==========================================================================
*/
/**
 * @brief Initialize crypto driver
 *
 */
void boot_sw_cipher_init(void);

/*===========================================================================
**  Function :  boot_sw_cipher_post_init
** ==========================================================================
*/
/**
 * @brief Post crypto driver initialization routines
 *
 */
void boot_sw_cipher_post_init(void);

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
void boot_encr_aes_key (uint8 *in, uint32 inlen, uint8 *out, uint32 *outlen);

/*===========================================================================
**  Function :  boot_check_oem_key
** ==========================================================================
*/
/**
 * @brief Check if the OEM key is valid
 *
 * @return  boolean
 *
 */
boolean boot_check_oem_key(void);

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
int boot_sw_cipher (
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
);
#endif /* _CIL_LOADER_H */
