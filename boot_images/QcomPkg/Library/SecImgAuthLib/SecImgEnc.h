#ifndef SEC_PIL_ENCI_H
#define SEC_PIL_ENCI_H

/**
@file sec_img_enci.h
@brief Secure Image Auth Security implementation
*/

/*===========================================================================
   Copyright (c) 2012-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE


when       who      what, where, why
--------   ---      ------------------------------------
02/26/14   mm       Support for Image Encryption in ROM
===========================================================================*/
#include "comdef.h"
#include "CeML.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SEC_IMG_AES_BLOCK_SIZE 16
#define SEC_IMG_TAG_LEN 16
#define SEC_IMG_KEY_SIZE 16
#define SEC_IMG_NONCE_LEN 13
#define SEC_IMG_NONCE_LEN_ALIGN 16
#define SEC_IMG_ASSOCIATED_DATA_LEN 16
#define SEC_IMG_MAGIC_ENCRYPTION 0x89FECDAB
#define SEC_IMG_ENCRYPTION_VERSION 1
#define SEC_IMG_KEY_SOURCE_ID_1 1
#define SEC_IMG_KEY_LADDER_LEN 3

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef PACKED struct sec_img_enc_key_s
{
   uint8   nonce[SEC_IMG_NONCE_LEN_ALIGN];                 /* Unique nonce */
   uint8   auth_data[SEC_IMG_ASSOCIATED_DATA_LEN];   /* Auth data */
   uint8   cipher_val[SEC_IMG_KEY_SIZE];             /* Key Size */
   uint8   mac_tag[SEC_IMG_TAG_LEN];                 /* MAC Tag */
}sec_img_enc_key_t;

typedef PACKED struct sec_img_enc_s
{
   uint32  magic;                             /* Magic number */
   uint32  version;                           /* Version number */
   uint32  size;                              /* Size of the data */
   uint32  key_source_id;                     /* Key Source Id */
   uint32  key_ladder_len;                    /* Key ladder length */
   uint32    key_ladder_algo;     /* Key ladder Algorithm */
   uint32    key_ladder_mode; /* Key Ladder Mode */

   sec_img_enc_key_t l2_enc_key;              /* L2 Key Encrypted data by L1 */
   sec_img_enc_key_t l3_enc_key;              /* L3 key encrypted by L2 */
   uint8   l3_image_iv[SEC_IMG_AES_BLOCK_SIZE]; /* Image/L3 encoding IV */
}sec_img_enc_t;

typedef struct sec_img_enc_info_s
{
  sec_img_enc_t *enc_hdr; /* Encryption Parameters */
  uint8 l3_key[16];
  uint8 l3_iv[16];
  uint8 l3_auth_data[16];
}sec_img_enc_info_t;

typedef enum sec_img_enc_error_s
{
  SEC_IMG_ENC_SUCCESS =0x0,
  SEC_IMG_ENC_HW_INVALID_PARAM = 0x200,
  SEC_IMG_ENC_HW_REVERSE_ARRAY_ERROR,
  SEC_IMG_ENC_HW_FEC_ERROR,
  SEC_IMG_ENC_FAILURE = 0x7FFFFFFF,
  SEC_IMG_ENC_MAX                    = 0x7FFFFFFF /**< Force to 32 bits */
}sec_img_enc_error_t;

/**
 * @brief update the L1 key from hardware to internal global value.
 *
 * @return 0 on success, negative on failure
 *
 */
sec_img_enc_error_t
sec_img_auth_update_l1_key_for_image();

/**
 * @brief get decryption key.
 *
 * @param[in/out] enc_info The encryption context and data
 *
 * @return 0 on success, negative on failure
 *
 */
sec_img_enc_error_t
sec_img_auth_decrypt_key
(
  const sec_img_enc_info_t *enc_info
);

/**
 * @brief hash and decrypt the encrypted data
 *
 * @param[in] enc_info      The encryption context and data
 * @param[in] hash_algo     The hash algorithm
 * @param[in] data1_in_ptr  The pointer to hash data
 * @param[in] data1_len     The length of the hash data
 * @param[in] digest_len    The length of the digest
 * @param[in] segment_num   The index to the segment
 *
 * @param[in/out] digest_ptr The pointer to digest data
 *
 * @return 0 on success, negative on failure
 *
 */
sec_img_enc_error_t
sec_img_auth_hash_cipher_data(
  
  const sec_img_enc_info_t *enc_info,
  CeMLHashAlgoType                  hash_algo,
  const uint8*                      data1_in_ptr,
  uint32                            data1_len,
  uint8*                            digest_ptr,
  uint32                            digest_len,
  uint32                          segment_num
);

#endif
