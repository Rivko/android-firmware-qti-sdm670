#ifndef QSEE_RSA_H
#define QSEE_RSA_H

/**
@file qsee_rsa.h
@brief Provide SECRSA API wrappers
*/

/*===========================================================================
   Copyright (c) 2012, 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
11/06/12   ah      Initial version.
05/25/16   yl      Add QSEE_RSA_OAEP_PAD_HASH_IDX and MD5 hash support

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
typedef uint32_t QSEE_BLONG;
#define QSEE_MAX_KEY_SIZE          4128                                              ///< Maximum key size in bits
#define QSEE_BLONG_SIZE            sizeof(QSEE_BLONG)                                ///< Bytes per digit
#define QSEE_BN_BITS_PER_WORD      (QSEE_BLONG_SIZE * 8)                             ///< Bit per digit
#define QSEE_BLONGS_PER_KEY        (QSEE_MAX_KEY_SIZE + 8*QSEE_BLONG_SIZE - 1)/(8*QSEE_BLONG_SIZE)  ///< Digits per key

#define QSEE_MAX_PUB_EXP_SIZE      5

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  QSEE_BLONG a[QSEE_BLONGS_PER_KEY];
  int n;
} QSEE_BigInt;

/** QSEE_S_BIGINT type */
typedef struct QSEE_S_BIGINT_ {
  QSEE_BigInt bi;
	int sign;
} QSEE_S_BIGINT;

/** RSA PKCS key */
typedef struct QSEE_RSA_KEY
{
   /** Type of key, QSEE_RSA_KEY_PUBLIC for encryption or
    *  QSEE_RSA_KEY_PRIVATE/QSEE_RSA_KEY_PRIVATE_CRT for
    *  decryption or QSEE_RSA_KEY_PRIVATE_PUBLIC or
    *  QSEE_RSA_KEY_PRIVATE_CRT_PUBLIC for key generation */
   int type;
   /** RSA key bit length **/
   int bitLength;
   /** The public exponent */
   QSEE_S_BIGINT *e;
   /** The private exponent */
   QSEE_S_BIGINT *d;
   /** The modulus */
   QSEE_S_BIGINT *N;
   /** The p factor of N */
   QSEE_S_BIGINT *p;
   /** The q factor of N */
   QSEE_S_BIGINT *q;
   /** The 1/q mod p CRT param */
   QSEE_S_BIGINT *qP;
   /** The d mod (p - 1) CRT param */
   QSEE_S_BIGINT *dP;
   /** The d mod (q - 1) CRT param */
   QSEE_S_BIGINT *dQ;
} QSEE_RSA_KEY;

/** QSEE RSA Key Type  */
typedef enum
{
/**used for ce_rsa_encrypt and ce_rsa_decrypt and other related api's */
   /** public key  */
   QSEE_RSA_KEY_PUBLIC  = 0,
   /** private key in non CRT representation */
   QSEE_RSA_KEY_PRIVATE = 1,
   /** private key in CRT representation */
   QSEE_RSA_KEY_PRIVATE_CRT = 2,
/**Used only for ce_rsa_key_gen*/
   /** private/public key pair */
   QSEE_RSA_KEY_PRIVATE_PUBLIC = 3 ,
   /** private CRT/public key pair */
   QSEE_RSA_KEY_PRIVATE_CRT_PUBLIC = 4,
   QSEE_RSA_KEY_INVALID = 0x7FFFFFFF,
}QSEE_RSA_KEY_TYPE;

/** QSEE RSA Key Size Macro */
#define QSEE_RSA_KEY_SIZE(key) ((key)->bi.n * QSEE_BLONG_SIZE)

/** Public Key algorithms in pkcs8 */
typedef enum
{
    QSEE_PKCS8_RSA_ENCRYPTION = 0,
    QSEE_PKCS8_ECC_ENCRYPTION,
    QSEE_PKCS8_ALGO_MAX  ,
    QSEE_PKCS8_INVALID = 	0x7FFFFFFF,

}   QSEE_pkcs8_algo_type;

/** RSA private key parameters */
/** refer to Sect7.2, PKCS#1: RSA Encryption Standard */
typedef struct
{
    uint8_t   version;
    uint16_t  mod_len;        /** modulus: n               */
    uint8_t*  mod_data;
    uint16_t  pub_exp_len;    /** public exponent: e       */
    uint8_t*  pub_exp_data;
    uint16_t  priv_exp_len;   /** private exponent: d      */
    uint8_t*  priv_exp_data;
    uint16_t  prime1_len;     /** prime 1: p               */
    uint8_t*  prime1_data;
    uint16_t  prime2_len;     /** prime 2: q               */
    uint8_t*  prime2_data;
    uint16_t  exp1_len;       /** exponent 1: d mod (p-1)  */
    uint8_t*  exp1_data;
    uint16_t  exp2_len;       /** exponent 1: d mod (q-1)  */
    uint8_t*  exp2_data;
    uint16_t  coef_len ;      /** coefficient: q**{-1} mod p   */
    uint8_t*  coef_data;

}   QSEE_pkcs8_rsa_privkey_type;

/** ECC private key parameters */
/** refer to ... ... */
typedef struct
{
    uint8_t   version;
    uint16_t  privkey_len;        /* modulus: n               */
    uint8_t*  privkey_data;
    uint16_t  pubkey_x_len;
    uint8_t*  pubkey_x_data;
    uint16_t  pubkey_y_len;
    uint8_t*  pubkey_y_data;
    uint8_t   compressed_pubkey;
    uint8_t   pubkey_y_flag;
    uint16_t  namedCurve;

}   QSEE_pkcs8_ecc_privkey_type;

/** DSA private key parameters */
typedef struct
{
    uint16_t  dummy_len;
    uint8_t*  dummy;

}   QSEE_pkcs8_dsa_privkey_type;

/** DH private key parameters */
typedef struct
{
    uint16_t  mod_len;
    uint8_t*  mod_data;
    uint16_t  exp_len;
    uint8_t*  exp_data;

}   QSEE_pkcs8_dh_privkey_type;


/** Union of all the private key types */
typedef struct
{
    uint8_t              ver;
    QSEE_pkcs8_algo_type    algo;
    union
    {
        QSEE_pkcs8_rsa_privkey_type  rsa;
        QSEE_pkcs8_dsa_privkey_type  dsa;
        QSEE_pkcs8_dh_privkey_type   dh;
        QSEE_pkcs8_ecc_privkey_type  ecc;

    }   key;

}   QSEE_pkcs8_privkey_type;

/** RSA padding type. PKCS #1 v2.1*/
typedef enum
{
   /*PKCS1 v1.5 signature*/
   QSEE_RSA_PAD_PKCS1_V1_5_SIG = 1,
   /*PKCS1 v1.5 encryption*/
   QSEE_RSA_PAD_PKCS1_V1_5_ENC = 2,
   /*OAEP Encryption*/
   QSEE_RSA_PAD_PKCS1_OAEP = 3,
   /*PSS Signature*/
   QSEE_RSA_PAD_PKCS1_PSS = 4,
   /* No Padding */
   QSEE_RSA_NO_PAD = 5,
   /*PSS with unknown saltlen*/
   QSEE_RSA_PAD_PKCS1_PSS_AUTORECOVER_SALTLEN = 6,
   QSEE_RSA_PAD_INVALID = 0x7FFFFFFF,
} QSEE_RSA_PADDING_TYPE;

typedef struct QSEE_RSA_OAEP_PAD_INFO
{
   /** index of Hash & Mask generation function desired */
   int hashidx;
   /** Label to add to the message */
   unsigned char *label;
   /** Length of label */
   int labellen;
} QSEE_RSA_OAEP_PAD_INFO;

typedef struct QSEE_RSA_PSS_PAD_INFO
{
   /** index of Hash & Mask generation function desired */
   int hashidx;
   /** Length of salt */
   int saltlen;
} QSEE_RSA_PSS_PAD_INFO;


/*index of hash algorithm used for generating signature */
typedef enum
{
   QSEE_HASH_IDX_NULL = 1,
   QSEE_HASH_IDX_SHA1,
   QSEE_HASH_IDX_SHA256,
   QSEE_HASH_IDX_SHA224,
   QSEE_HASH_IDX_SHA384,
   QSEE_HASH_IDX_SHA512,
   QSEE_HASH_IDX_MD5,
   QSEE_HASH_IDX_MAX,
   QSEE_HASH_IDX_INVALID = 0x7FFFFFFF,
}QSEE_HASH_IDX;

/* 
   Index of hash algorithm used for RSA OAEP padding. 
   Below indexes are kept for backward compatbility
   only. To use SHA1 for mgf hash function correctly, 
   please use QSEE_RSA_OAEP_HASH_IDX_XXX_SHA1 instead.
   QSEE_RSA_OAEP_HASH_IDX_SHA256
   QSEE_RSA_OAEP_HASH_IDX_SHA224
   QSEE_RSA_OAEP_HASH_IDX_SHA384
   QSEE_RSA_OAEP_HASH_IDX_SHA512
*/
typedef enum 
{
   QSEE_RSA_OAEP_HASH_IDX_NULL = 1,
   QSEE_RSA_OAEP_HASH_IDX_SHA1,
   QSEE_RSA_OAEP_HASH_IDX_SHA256,
   QSEE_RSA_OAEP_HASH_IDX_SHA224,
   QSEE_RSA_OAEP_HASH_IDX_SHA384,
   QSEE_RSA_OAEP_HASH_IDX_SHA512,
   QSEE_RSA_OAEP_HASH_IDX_MD5,   
   QSEE_RSA_OAEP_HASH_IDX_SHA256_SHA1,
   QSEE_RSA_OAEP_HASH_IDX_SHA224_SHA1,
   QSEE_RSA_OAEP_HASH_IDX_SHA384_SHA1,
   QSEE_RSA_OAEP_HASH_IDX_SHA512_SHA1,
   QSEE_RSA_OAEP_HASH_IDX_MAX,
   QSEE_RSA_OAEP_HASH_IDX_INVALID = 0x7FFFFFFF,
} QSEE_RSA_OAEP_PAD_HASH_IDX;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief  This function will generate RSA private/public key.
 *
 * @param[out] key         The public/private RSA key
 * @param[in] keylen       RSA key length (in Bytes)
 * @param[in] pub_exp      Public exponent array
 * @param[in] pub_exp_len  Public exponent array length
 *
 * @return 0 on success, negative on failure
*/

int qsee_rsa_key_gen(QSEE_RSA_KEY *key,
                     int keylen,
                     unsigned char *pub_exp,
                     int pub_exp_len);

/**
 * @brief  This function parses a private key in PKCS#8 format.
 *
 * @param[in] data_ptr     Pointer to the raw PKCS#8 data
 * @param[in] data_len     Length of the PKCS#8 data
 * @param[out] privkey     Pointer to the private key extracted
 *                         from the raw data
 *
 * @return 0 on success, negative on failure
*/
int qsee_secpkcs8_parse(uint8_t* data_ptr,
                        uint16_t data_len,
                        QSEE_pkcs8_privkey_type* privkey);


/**
 * @brief  This function does PKCS #1 padding then sign
 *         signature.
 *
 * @param[in] key          The private RSA key to use
 * @param[in] padding_type Type of padding
 * @param[in] padding_info PSS padding parameters
 * @param[in] hashidx      The index of the hash desired
 * @param[in] hash         The hash to sign (octets)
 * @param[in] hashlen      The length of the hash to sign
 * @param[out] signature   The signature
 * @param[in,out] siglen   The max size and resulting size of
 *                         the signature
 *
 * @return 0 on success, negative on failure
*/
int qsee_rsa_sign_hash
(
   QSEE_RSA_KEY              *key,
   QSEE_RSA_PADDING_TYPE     padding_type,
   void                      *padding_info,
   QSEE_HASH_IDX             hashidx,
   const unsigned char       *hash,
   int                       hashlen,
   unsigned char             *signature,
   int                       *siglen
);

/**
 * @brief  This function does PKCS #1 padding then verify
 *         signature.
 *
 * @param[in] key          The private RSA key to use
 * @param[in] padding_type Type of padding
 * @param[in] padding_info PSS padding parameters
 * @param[in] hashidx      The index of the hash desired
 * @param[in] hash         The hash to sign (octets)
 * @param[in] hashlen      The length of the hash to sign
 * @param[in] sig          The signature
 * @param[in] siglen       The max size and resulting size of
 *                         the signature
 *
 * @return 0 on success, negative on failure
*/
int qsee_rsa_verify_signature
(
   QSEE_RSA_KEY              *key,
   QSEE_RSA_PADDING_TYPE     padding_type,
   void                      *padding_info,
   QSEE_HASH_IDX             hashidx,
   unsigned char             *hash,
   int                       hashlen,
   unsigned char             *sig,
   int                       siglen
);

/**
 * @brief  This function does PKCS #1 v1.5 padding then encrypt.
 *
 * @param[in] key           The RSA key to encrypt to
 * @param[in] padding_type  Type of padding
 * @param[in] padding_info  OAEP padding parameters
 * @param[in] msg           The plaintext
 * @param[in] msglen        The length of the plaintext(octets)
 * @param[out] cipher       The ciphertext
 * @param[in,out] cipherlen The max size and resulting size of
 *                          the ciphertext
 *
 * @return 0 on success, negative on failure
*/
int qsee_rsa_encrypt
(
   QSEE_RSA_KEY            *key,
   QSEE_RSA_PADDING_TYPE   padding_type,
   void                    *padding_info,
   const unsigned char     *msg,
   int                     msglen,
   unsigned char           *cipher,
   int                     *cipherlen
);

/**
 * @brief  This function does PKCS #1 decrypt then v1.5 depad.
 *
 * @param[in] key           The corresponding private RSA key
 * @param[in] padding_type  Type of padding
 * @param[in] padding_info  OAEP padding parameters
 * @param[in] cipher        The ciphertext
 * @param[in] cipherlen     The length of the ciphertext(octets)
 * @param[out] msg          The plaintext
 * @param[in,out] msglen    The max size and resulting size of
 *                          the plaintext
 *
 * @return 0 on success, negative on failure
*/
int qsee_rsa_decrypt
(
   QSEE_RSA_KEY              *key,
   QSEE_RSA_PADDING_TYPE     padding_type,
   void                    *padding_info,
   unsigned char           *cipher,
   int                     cipherlen,
   unsigned char           *msg,
   int                     *msglen
);

/**
 * @brief
 *  Read an unsigned buffer of bytes into a big integer
 *
 * @param a[out]   Pointer to big integer
 * @param buf[in]  Pointer to array of bytes
 * @param len[in]  Array length
 *
 * @return 0 on success, negative on failure
*/
int qsee_BIGINT_read_unsigned_bin
(
   QSEE_BigInt * a,
   const uint8_t * buf,
   uint32_t len
);

/**
 * @brief
 *  Allocate and initialize S_BIGINT data
 *
 * @param a[in]   S_BIGINT data
 *
 * @return 0 on success, negative on failure
*/
int qsee_util_init_s_bigint
(
   QSEE_S_BIGINT **a
);

/**
 * @brief
 *  Free S_BIGINT data
 *
 * @param a[in]   S_BIGINT data
 *
 * @return 0 on success, negative on failure
*/
void qsee_util_free_s_bigint
(
   QSEE_S_BIGINT *a
);

/**
 * @brief
 *  Read a zero terminated string into a big integer
 *
 * @param a[out]     Pointer to QSEE_BigInt structure
 * @param str[in]    Pointer to zero terminated string
 * @param radix[in]  Radix
 *
 * @return 0 on success, negative on failure
 */
int qsee_BIGINT_read_radix
(
   QSEE_BigInt * a,
   const char *str,
   uint32_t radix
);

/**
 * Compute an RSA modular exponentiation
 * @param[in] key         The RSA key to use
 * @param[in] in          The input data to send into RSA
 * @param[in] inlen       The length of the input (octets)
 * @param[out] out        The destination
 * @param[in,out] outlen  The max size and resulting size of the output
 * @param[in] which       Which exponent to use, e.g. PRIVATE or PUBLIC
 *
 * @return
 *  CE_SUCCESS     - Function executes successfully.
 *  CE_ERROR_NOT_SUPPORTED - the feature is not supported.
 *  CE_ERROR_INVALID_PACKET - invalid packet.
 *  CE_ERROR_BUFFER_OVERFLOW - not enough space for output.
 *
 * @dependencies
 * None.
 *
*/
int qsee_rsa_exptmod
(
   QSEE_RSA_KEY            *key,
   const unsigned char     *in,
   int                     inlen,
   unsigned char           *out,
   int                     *outlen,
   int                     which
);

/**
 * count total byte numbers in S_BIGINT BLONG array
 * @param[in]  s     QSEE_S_BIGINT data
 *
 * @return
 *  total byte numbers
 *
 * @dependencies
 * None.
*/
int qsee_util_count_bytes
(
   QSEE_S_BIGINT *s
);

#endif /*QSEE_RSA_H*/

