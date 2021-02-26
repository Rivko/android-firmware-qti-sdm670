#ifndef __SVCUEFIAUTH_H_
#define __SVCUEFIAUTH_H_
/*===========================================================================
  Copyright (c) 2011 - 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/* Notes:
  

*/ 



/*===========================================================================

                      EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup tzexec
  @} */
#include "com_dtypes.h"

/* Common Definitions for Service A */
//#define SVC_UEFI_AUTH_TEMP_MAX_SIZE    128

typedef enum
{
  KEY_TYPE_OTHER                      = 0,
  KEY_TYPE_X509_CERT                  = 1,
  KEY_TYPE_RSA_MODULUS                = 2,

  //Add new types above this one
   KEY_TYPE_COUNT,
   KEY_TYPE_MAX                       =0x7FFFFFFF /*force to 32 bits*/
} KEY_TYPE;

#define UEFI_AUTH_KEY_HEADERSIZE      2*sizeof(uint32)
typedef struct {
  uint32        type;
  uint32        length;

  /* This is a placeholder to the start of the key data */
  uint8         key_data[1];
  uint8         padding[3]; /* To avoid alignment issues */
} UEFI_AUTH_KEY;

typedef struct {
  uint32        num_keys;

  /* This is a placeholder for an array of UEFI_AUTH_KEY structures */
  UEFI_AUTH_KEY pkey[1];
} UEFI_AUTH_KEY_LIST;

typedef enum
{
  VARIABLE_TYPE_OTHER                 = 0,
  VARIABLE_TYPE_PK                    = 1,
  VARIABLE_TYPE_KEK                   = 2, 
  VARIABLE_TYPE_DB                    = 3,
  VARIABLE_TYPE_DBX                   = 4,

  //Add new types above this one
   VARIABLE_TYPE_COUNT,
   VARIABLE_TYPE_MAX                  =0x7FFFFFFF /*force to 32 bits*/
} VARIABLE_TYPE;


/**
  @brief This function can be called to register pk. The key 
  data is copied and stored internally. pk can only have Max one
  key. If pk have zero keys in the list then during 
  update_secure_variable rooting to that specific key (ifneeded) 
  would be skipped silently. If this api is called again; 
  previous buffers will be deleted and new buffers will be 
  created and new key copied over. pk is a stream with following
  format UINT32 NumKeys, [UINT32 KeyType, UINT32 KeyLen, UINT8 
  keyData[KeyLen], <UINT32 KeyType2, UINT32 KeyLen2, UINT8
  keyData2[KeyLen2] ....>]
  KeyType is one of KEY_TYPE_XXX.

  @param  pk               Pointer to a stream of pk data
  
  @retval E_SUCCESS           success
  @retval -E_FAILURE          Failed to execute cmd

**/

int register_pkkey(uint8* pk);


/**
  @brief This function can be called kek 
  keys. The key data is copied and stored internally. 
  kek can have multiple keys. If kek have zero keys in the 
  list then during update_secure_variable rooting to that 
  specific key (ifneeded) would be skipped silently. If this 
  api is called again; previous buffers will be deleted and new 
  buffers will be created and new keys copied over.Kek 
  is a stream with following format 
  UINT32 NumKeys, [UINT32 KeyType, UINT32 KeyLen, UINT8
  keyData[KeyLen], <UINT32 KeyType2, UINT32 KeyLen2, UINT8
  keyData2[KeyLen2] ....>]
  KeyType is one of KEY_TYPE_XXX.

  @param  kek              pointer to a stream of kek data
  
  @retval E_SUCCESS           success
  @retval -E_FAILURE          Failed to execute cmd

**/

int register_kekkeys(uint8* kek);

/**
  @brief api validates the pkcs7; computes the certificate chain and 
  matches it with appropriate PK/KEK and returns the root key of the certificate
  used to sign the pkcs7.


  @param  auth_info        Pointer to a auth info(pkcs7 blob)
  @param  auth_info_size   Size of auth info
  @param  data             Serialized data to hash as described in UEFI spec
  @param  data_size        size of serialized data
  @param  variable_type    One of PK/KEK/DB/DBX/Other type (VARIABLE_TYPE_XXX)
  @param  public_key       if this api is successfull; then it copy the key out.
                           Buffer is allocated by the caller; input/output length is in
                           *public_key_size. 
  @param  public_key_size  [in] specifies the public_key buffer size; [out] on SUCCESS specifes 
                           number of bytes copied to public_key. [out] on fail; this 
                           might get updated with expected size if the input size was too small.
   @param signer_pubkey_sha256hash 32 byte user provided buffer 
                                   to copy the hash of the
                                   signers modulus
  @retval E_SUCCESS           success
  @retval -E_FAILURE          Failed to execute cmd

**/

int validate_pkcs7_and_get_public_key(uint8* auth_info, uint32 auth_info_size,
                                      uint8* data, uint32 data_size, VARIABLE_TYPE variable_type,
                                      uint8* public_key, uint32 *public_key_size,
                                      uint8* signer_pubkey_sha256hash);


/**
 * @brief Gen Key for RPMBUpdate Secure Table. AES128/256 Encrypts 
    some random data using HW key and returns the encrpted data as the key
    for rpmb to use. The key_out buffer is allocated by caller and should be either
    16 or 32 bytes long.

 * @param[out] key_out        Pointer to the buffer where key will be copied
 * @param[in] key_out_len     Length of key (16 or 32 bytes)
 *
 * @return 0 on success, negative on failure
 */

int gen_key_for_rpmb(uint8 *key_out, uint32 key_out_len);

#endif /* __SVCUEFIAUTH_H_ */ 
