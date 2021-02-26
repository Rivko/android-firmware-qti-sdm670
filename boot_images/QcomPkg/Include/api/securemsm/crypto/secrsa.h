#ifndef CE_RSA_H
#define CE_RSA_H

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

   This file contains the definitions of the constants, data structures
   and interfaces that provide RSA PKCS #1 v2.1 Alogrithms support. It
   defines the external interfaces for RSA encrpytion/decryption, signature/
   verification, key generation/import/export.
  ===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/securemsm/crypto/secrsa.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  04/11/11   qxu     initial version
  05/25/16   yl      Support MD5 hash in CE_HASH_IDX
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include <Library/CryptoLib/secrsa/secrsa_err.h>
#include <Library/CryptoLib/secrsa/secrsa_utils.h>

/*RSA padding type. PKCS #1 v2.1*/
typedef enum
{
   /*PKCS1 v1.5 signature*/
   CE_RSA_PAD_PKCS1_V1_5_SIG = 1,
   /*PKCS1 v1.5 encryption*/
   CE_RSA_PAD_PKCS1_V1_5_ENC = 2,
   /*OAEP Encryption*/
   CE_RSA_PAD_PKCS1_OAEP = 3,
   /*PSS Signature*/
   CE_RSA_PAD_PKCS1_PSS = 4,
   /* No Padding */
   CE_RSA_NO_PAD = 5,
   /*PSS with unknown saltlen*/
   CE_RSA_PAD_PKCS1_PSS_AUTORECOVER_SALTLEN = 6,
   CE_RSA_PAD_INVALID = 0x7FFFFFFF,
} CE_RSA_PADDING_TYPE;

typedef struct CE_RSA_OAEP_PAD_INFO
{
   /** index of Hash & Mask generation function desired */
   int hashidx;
   /** Label to add to the message */
   unsigned char *label;
   /** Length of label */
   int labellen;
} CE_RSA_OAEP_PAD_INFO;


/*index of hash algorithm used for generating signature */
typedef enum
{
   CE_HASH_IDX_NULL        = 0x1,
   CE_HASH_IDX_SHA1        = 0x2,
   CE_HASH_IDX_SHA256      = 0x3,
   CE_HASH_IDX_SHA224      = 0x4,
   CE_HASH_IDX_SHA384      = 0x5,
   CE_HASH_IDX_SHA512      = 0x6,
   CE_HASH_IDX_MD5         = 0x7,
   CE_HASH_IDX_MAX         = 0x8,
   CE_HASH_IDX_INVALID     = 0x7FFFFFFF
}CE_HASH_IDX;

typedef enum 
{
   CE_RSA_OAEP_HASH_IDX_NULL = 1,
   CE_RSA_OAEP_HASH_IDX_SHA1,
   CE_RSA_OAEP_HASH_IDX_SHA256,
   CE_RSA_OAEP_HASH_IDX_SHA224,
   CE_RSA_OAEP_HASH_IDX_SHA384,
   CE_RSA_OAEP_HASH_IDX_SHA512,
   CE_RSA_OAEP_HASH_IDX_MD5,   
   CE_RSA_OAEP_HASH_IDX_SHA256_SHA1,
   CE_RSA_OAEP_HASH_IDX_SHA224_SHA1,
   CE_RSA_OAEP_HASH_IDX_SHA384_SHA1,
   CE_RSA_OAEP_HASH_IDX_SHA512_SHA1,
   CE_RSA_OAEP_HASH_IDX_MAX,
   CE_RSA_OAEP_HASH_IDX_INVALID = 0x7FFFFFFF,
} CE_RSA_OAEP_PAD_HASH_IDX;


typedef struct CE_RSA_PSS_PAD_INFO
{
   /** index of Hash & Mask generation function desired */
   int hashidx;
   /** Length of salt */
   int saltlen;
} CE_RSA_PSS_PAD_INFO;

/** RSA PKCS key */
typedef struct CE_RSA_KEY
{
   /** Type of key, CE_RSA_PRIVATE or CE_RSA_PUBLIC */
   int type;
   /** RSA key bit length **/
   int bitLength;
   /** The public exponent */
   S_BIGINT *e;
   /** The private exponent */
   S_BIGINT *d;
   /** The modulus */
   S_BIGINT *N;
   /** The p factor of N */
   S_BIGINT *p;
   /** The q factor of N */
   S_BIGINT *q;
   /** The 1/q mod p CRT param */
   S_BIGINT *qP;
   /** The d mod (p - 1) CRT param */
   S_BIGINT *dP;
   /** The d mod (q - 1) CRT param */
   S_BIGINT *dQ;
} CE_RSA_KEY;

typedef struct CE_RSA_KEY_BIGNUM
{
   /** Type of key, CE_RSA_KEY_PUBLIC for encryption or
    *  CE_RSA_KEY_PRIVATE/CE_RSA_KEY_PRIVATE_CRT for
    *  decryption or CE_RSA_KEY_PRIVATE_PUBLIC or
    *  CE_RSA_KEY_PRIVATE_CRT_PUBLIC for key generation */
   int type;
   /** RSA key bit length **/
   int bitLength;
   /** The public exponent */
   BigNum e;
   /** The private exponent */
   BigNum d;
   /** The modulus */
   BigNum N;
   /** The p factor of N */
   BigNum p;
   /** The q factor of N */
   BigNum q;
   /** The 1/q mod p CRT param */
   BigNum qP;
   /** The d mod (p - 1) CRT param */
   BigNum dP;
   /** The d mod (q - 1) CRT param */
   BigNum dQ;
} CE_RSA_KEY_BIGNUM;


/*CE RSA Key Type*/
typedef enum
{
   CE_RSA_KEY_PUBLIC  = 0, /*public key*/
   CE_RSA_KEY_PRIVATE = 1,    /*private key in non CRT representation */
   CE_RSA_KEY_PRIVATE_CRT = 2, /*private key in CRT representation*/
   CE_RSA_KEY_PRIVATE_PUBLIC = 3, /*private/public key pair*/
   CE_RSA_KEY_PRIVATE_CRT_PUBLIC = 4,    /*private CRT/public key pair*/
   CE_RSA_KEY_INVALID = 0x7FFFFFFF
}CE_RSA_KEY_TYPE;

/*CE RSA Key Size*/
#define CE_RSA_KEY_SIZE(key) ((key)->bi.n * BLONG_SIZE)

/**
  initialize RSA key: create an empty keysets

  @return
   CE_SUCCESS     - Function executes successfully.

  @dependencies
  None.
*/
CE_RSA_KEY *ce_rsa_keyset_init(void);


/**
  Destroy RSA keysets

  @param[in]  keys       The RSA key sets

  @return
   CE_SUCCESS     - Function executes successfully.

  @dependencies
  None.
*/
void ce_rsa_keyset_del(CE_RSA_KEY *key);

/**
  PKCS #1 pad then sign signature
  @param[in]  key             The private RSA key to use
  @param[in]  padding_type    Type of padding (CE_RSA_PAD_PKCS1_V1_5_SIG or CE_RSA_PAD_PKCS1_PSS)
  @param[in]  padding_info    The parameters for PSS padding
  @param[in]  hash_idx        The index of the hash desired
  @param[in]  hashlen         The hash to sign (octets)
  @param[in]  hashlen         The length of the hash to sign (octets)
  @param[out] signature       The signature
  @param[in, out] signlen     The max size and resulting size of the signature

  @return
   CE_SUCCESS     - Function executes successfully.
   CE_ERROR_FAILURE - Generic Error.
   CE_ERROR_NOT_SUPPORTED - the feature is not supported.
   CE_ERROR_INVALID_ARG - Generic invalid argument.
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output.
   CE_ERROR_NO_MEMORY - Out of memory.
*/
int ce_rsa_sign_hash
(
   CE_RSA_KEY                *key,
   CE_RSA_PADDING_TYPE       padding_type,
   void                      *padding_info,
   int                       hashidx,
   const unsigned char       *hash,
   int                       hashlen,
   unsigned char             *signature,
   int                       *siglen
);

/**
  PKCS #1 pad then verify signature
  @param[in]  key          The private RSA key to use
  @param[in]  padding_type Type of padding (CE_RSA_PAD_PKCS1_V1_5_SIG or CE_RSA_PAD_PKCS1_PSS)
  @param[in]  padding_info The parameters for PSS padding
  @param[in]  hashidx      The index of the hash desired
  @param[in]  hash         The hash to sign (octets)
  @param[in]  hashlen      The length of the hash to sign (octets)
  @param[in]  sig          The signature
  @param[in]  signlen      The max size and resulting size of the signature

  @return
   CE_SUCCESS     - Function executes successfully.
   CE_ERROR_FAILURE - Generic Error.
   CE_ERROR_NOT_SUPPORTED - the feature is not supported.
   CE_ERROR_INVALID_ARG - Generic invalid argument.
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output.
   CE_ERROR_NO_MEMORY - Out of memory.
   CE_ERROR_INVALID_SIGNATURE - invalid signature.
*/
int ce_rsa_verify_signature
(
   CE_RSA_KEY                *key,
   CE_RSA_PADDING_TYPE       padding_type,
   void                      *padding_info,
   int                       hashidx,
   unsigned char             *hash,
   int                       hashlen,
   unsigned char             *sig,
   int                       siglen
);

/**
   (PKCS #1 v1.5) PKCS1 pad then encrypt
  @param[in]  key          The RSA key to encrypt to
  @param[in]  padding_type Type of padding (CE_RSA_PAD_PKCS1_V1_5_ENC or CE_RSA_PAD_PKCS1_OAEP)
  @param[in]  padding_info The parameters for OAEP padding
  @param[in]  msg          The plaintext
  @param[in]  msglen       The length of the plaintext (octets)
  @param[out] cipher       The ciphertext
  @param[in,out] cipherlen The max size and resulting size of the ciphertext

  @return
   CE_SUCCESS     - Function executes successfully.
   CE_ERROR_NOT_SUPPORTED - the feature is not supported.
   CE_ERROR_INVALID_PACKET - invalid packet.
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output.

  @dependencies
  None.
*/
int ce_rsa_encrypt
(
   CE_RSA_KEY              *key,
   CE_RSA_PADDING_TYPE     padding_type,
   void                    *padding_info,
   const unsigned char     *msg,
   int                     msglen,
   unsigned char           *cipher,
   int                     *cipherlen
);

/**
  PKCS #1 decrypt then v1.5 depad
  @param[in] key          The corresponding private RSA key
  @param[in] padding_type Type of padding (CE_RSA_PAD_PKCS1_V1_5_ENC or CE_RSA_PAD_PKCS1_OAEP)
  @param[in] padding_info The parameters for OAEP padding
  @param[in] cipher       The ciphertext
  @param[in] cipherlen    The length of the ciphertext (octets)
  @param[out] msg         The plaintext
  @param[in,out] msglen   The max size and resulting size of the plaintext (octets)

  @return
   CE_SUCCESS     - Function executes successfully.
   CE_ERROR_NOT_SUPPORTED - the feature is not supported.
   CE_ERROR_INVALID_PACKET - invalid packet.
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output.

  @dependencies
  None.
*/
int ce_rsa_decrypt
(
   CE_RSA_KEY              *key,
   CE_RSA_PADDING_TYPE     padding_type,
   void                    *padding_info,
   unsigned char           *cipher,
   int                     cipherlen,
   unsigned char           *msg,
   int                     *msglen
);

/**
  RSA RAW CRT function
  @param[in] key          The corresponding private RSA key
  @param[in] DataIn       Input data
  @param[out] ResultOut   signed/decrypted result

  @return
   CE_SUCCESS     - Function executes successfully.
   CE_ERROR_NOT_SUPPORTED - the feature is not supported.
   CE_ERROR_INVALID_PACKET - invalid packet.
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output.

  @dependencies
  None.
*/
SECMATH_ERRNO_ET RSA_CRT_exponent (
   CE_RSA_KEY              *key,
   S_BIGINT                *DataIn,
   S_BIGINT                *ResultOut
   );

#ifdef __cplusplus
}
#endif

#endif
