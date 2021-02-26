#ifndef QSEE_ECC_H
#define QSEE_ECC_H

/**
@file qsee_ecc.h
@brief Provide ECC API wrappers
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
12/21/12   ah      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/**
 * Define either ECC_P256 or ECC_P192 to control which polynomial is used.
 * Of course, the binary must have been compiled with the same .h file
 * contents as are used by applications.
 *
 * Arguably, to avoid branching in version control systems, this
 * should be supplied by a -DP256 or -DP192 at compile time. It would be
 * good to include a run-time check to make sure the
 * library was compiled with the same modulus as the include file
 * specifies, but it seems require that the main "functions" be
 * macros, and I'm not sure I want to do that.
 */

#define ECC_P256

#ifdef ECC_P256
#define QSEE_BIGLEN 9
#endif

#ifdef ECC_P192
#define QSEE_BIGLEN 7
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct {
  /**
   * The representation is little endian by word and native endian within
   * each word.
   */
  uint32_t data[QSEE_BIGLEN];
} QSEE_bigval_t;


typedef struct {
  QSEE_bigval_t x;
  QSEE_bigval_t y;
  bool infinity;
} QSEE_affine_point_t;

typedef struct {
  QSEE_bigval_t r;
  QSEE_bigval_t s;
} QSEE_ECDSA_sig_t;

/* Generic ECC parameters type */

#define QSEE_ECC_MAX_MODULUS_BITS (521 + 8) 
#define QSEE_ECC_BIGNUM_WORDS ((QSEE_ECC_MAX_MODULUS_BITS + 24 + 31) / 32)

typedef struct {
  unsigned flags;
  size_t allocated_words;
  size_t num_words;  /* used words in bignums */
  size_t modulus_bits;
  uint32_t *modulus;
  uint32_t *montgomery_R;
  uint32_t *montgomery_R2;
  uint32_t m_inv32; /* used in computing qi for montgomery multiplication */
  /* Number of bits to shift {a[hi], a[hi-1]} in reduce()
   * so that the binary point is just left of bit 7.
   * i.e. quo_num_shift = modulusbits + 24 - 32 * hi
   */
  int quo_num_shift;
  uint64_t m_recip;  /* Assuming binary point of modulus just left of */
} QSEE_qrlbn_modulus_data_t;

typedef struct {
  uint32_t data[QSEE_ECC_BIGNUM_WORDS];
} QSEE_qrlbn_ecc_bigval_t;

typedef struct {
  QSEE_qrlbn_ecc_bigval_t X;
  QSEE_qrlbn_ecc_bigval_t Y;
  QSEE_qrlbn_ecc_bigval_t Z;
  int flags;
} QSEE_qrlbn_ecc_point_t;

typedef struct {
  QSEE_qrlbn_modulus_data_t md;
  QSEE_qrlbn_ecc_bigval_t modulus_storage;
  QSEE_qrlbn_ecc_bigval_t montgomery_R_storage;
  QSEE_qrlbn_ecc_bigval_t montgomery_R2_storage;
} QSEE_qrlbn_ecc_modulus_data_t;

typedef struct {
  QSEE_qrlbn_ecc_bigval_t x;
  QSEE_qrlbn_ecc_bigval_t y;
  int flags;
} QSEE_qrlbn_ecc_affine_point_t;

typedef struct {
  int version;
  QSEE_qrlbn_ecc_modulus_data_t modulus;
  QSEE_qrlbn_ecc_modulus_data_t base_point_order;
  QSEE_qrlbn_ecc_bigval_t a;
  QSEE_qrlbn_ecc_bigval_t b;
  QSEE_qrlbn_ecc_bigval_t a_mont;  // montgomery
  QSEE_qrlbn_ecc_bigval_t b_mont;  // montgomery
  QSEE_qrlbn_ecc_affine_point_t affine_base_point; 
  QSEE_qrlbn_ecc_point_t base_point; // montgomery
  QSEE_qrlbn_ecc_bigval_t ts_z;  // montgomery, for Tonelli Shanks sqrt
  uint64_t cofactor;
} QSEE_qrlbn_ecc_domain_t;

typedef struct {
  QSEE_qrlbn_ecc_bigval_t r;
  QSEE_qrlbn_ecc_bigval_t s;
} QSEE_qrlbn_ECDSA_sig_t;

typedef enum {
  QSEE_qrlbn_tag_m,       /* limited to 32 * mod_words - 24 bits */
  QSEE_qrlbn_tag_X,       /* limited to 32 * mod_words - 24 bits */
  QSEE_qrlbn_tag_Y,       /* limited to 32 * mod_words - 24 bits */
  QSEE_qrlbn_tag_a,       /* limited to 32 * mod_words - 24 bits */
  QSEE_qrlbn_tag_b,       /* limited to 32 * mod_words - 24 bits */
  QSEE_qrnbn_tag_n,       /* limited to 32 * mod_words - 24 bits */
  QSEE_qrlbn_tag_privkey, /* 32 * base_point_order_num_words - 24 bits */
  QSEE_qrlbn_tag_hash,    /* no limit */
  QSEE_qrlbn_tag_r,       /* 32 * base_point_order_num_words - 24 bits */
  QSEE_qrlbn_tag_s,       /* 32 * base_point_order_num_words - 24 bits */
  QSEE_qrlbn_tag_p,
  QSEE_qrlbn_tag_q,
} QSEE_qrlbn_field_tag_t;

/**
 * ECIES block cipher algorithms
 */
typedef enum {
  QSEE_ECIES_AES_128 = 0,
  QSEE_ECIES_AES_192 = 1,
  QSEE_ECIES_AES_256 = 2,
} QSEE_ecies_balg_t;

/**
 * Possible purposes of a key (or pair).
 */
typedef enum {
  QSEE_ECIES_PURPOSE_ENCRYPT = 0,    /* Usable with RSA, EC and AES keys. */
  QSEE_ECIES_PURPOSE_DECRYPT = 1,    /* Usable with RSA, EC and AES keys. */
} QSEE_ecies_purpose_t;

/**
 * Supported EC curves, used in ECDSA/ECIES.
 */
typedef enum {
  QSEE_ECIES_CURVE_P_224 = 0,
  QSEE_ECIES_CURVE_P_256 = 1,
  QSEE_ECIES_CURVE_P_384 = 2,
  QSEE_ECIES_CURVE_P_521 = 3,
} QSEE_ecies_curve_t;

/**
 * Digests provided by keymaster implementations.
 */
typedef enum {
  QSEE_ECIES_DIGEST_NONE = 0,
  /* Optional, may not be implemented in HW, will be handled in SW if needed. */
  QSEE_ECIES_DIGEST_MD5 = 1,
  QSEE_ECIES_DIGEST_SHA1 = 2,
  QSEE_ECIES_DIGEST_SHA_2_224 = 3,
  QSEE_ECIES_DIGEST_SHA_2_256 = 4,
  QSEE_ECIES_DIGEST_SHA_2_384 = 5,
  QSEE_ECIES_DIGEST_SHA_2_512 = 6,
} QSEE_ecies_digest_t;

/*
 * Key derivation functions, mostly used in ECIES.
 */
typedef enum {
  /* Do not apply a key derivation function; use the raw agreed key */
  QSEE_ECIES_KDF_NONE = 0,
  /* HKDF defined in RFC 5869 with SHA256 */
  QSEE_ECIES_KDF_RFC5869_SHA256 = 1,
  /* KDF1 defined in ISO 18033-2 with SHA1 */
  QSEE_ECIES_KDF_ISO18033_2_KDF1_SHA1 = 2,
  /* KDF1 defined in ISO 18033-2 with SHA256 */
  QSEE_ECIES_KDF_ISO18033_2_KDF1_SHA256 = 3,
  /* KDF2 defined in ISO 18033-2 with SHA1 */
  QSEE_ECIES_KDF_ISO18033_2_KDF2_SHA1 = 4,
  /* KDF2 defined in ISO 18033-2 with SHA256 */
  QSEE_ECIES_KDF_ISO18033_2_KDF2_SHA256 = 5,
} QSEE_ecies_kdf_t;

typedef struct _QSEE_ecies_key {
  union {
   QSEE_qrlbn_ecc_affine_point_t * recipient_public_key;
   QSEE_qrlbn_ecc_bigval_t * private_key;
  } enc_dec;
} QSEE_ecies_key_t;

typedef struct _QSEE_ecies_params {
  QSEE_ecies_curve_t ecies_curve;    /* ECC curve */
  QSEE_ecies_kdf_t kdf;              /* kdf */
  QSEE_ecies_digest_t digest;        /* digest hash function */
  QSEE_ecies_balg_t balg;            /* symetric algorithm (AES) scheme */
} QSEE_ecies_params_t;

typedef struct {
  QSEE_qrlbn_ecc_domain_t domain;    /* ECC domain */
  QSEE_ecies_params_t params;        /* params for ECIES */
} QSEE_ecies_ctx_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief generate public key and private key of ECC
 *        same key is used for ECDH and ECDSA
 *
 * @param pubkey       [out] pointer to ECC public key
 * @param privkey      [out] pointer to ECC private key
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_SW_ECC_PubPrivate_Key_generate(QSEE_affine_point_t *pubkey,
                                        QSEE_bigval_t *privkey);

/**
 * @brief generate shared key out of Alice' public key and Bob's private key
 *
 * @param shared_key   [out] shared key bewteen Alice and Bob
 * @param pubkey       [in] pointer to ECC public key
 * @param privkey      [in] pointer to ECC private key
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_SW_ECDH_Shared_Key_Derive(QSEE_affine_point_t *shared_key,
                                   QSEE_bigval_t *privkey,
                                   QSEE_affine_point_t *pubkey);

/**
 * @brief sign data with ECC private key
 *
 * @param msgdgst   [in] msg that needs to be signed
 * @param privkey   [in] pointer to private key to be used for signing
 * @param sig       [out] pointer to the buff that the signed message will be
 *                        stored
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_SW_ECDSA_Sign(QSEE_bigval_t const *msgdgst,
                       QSEE_bigval_t const *privkey,
                       QSEE_ECDSA_sig_t *sig);

/**
 * @brief verify data with ECC public key
 *
 * @param msgdgst   [in] msg that needs to be authenticated
 * @param pubvkey   [in] pointer to public key to be used for signing
 * @param sig       [in] pointer to the buff that the signed message will be stored
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_SW_ECDSA_Verify(QSEE_bigval_t const *msgdgst,
                         QSEE_affine_point_t const *pubkey,
                         QSEE_ECDSA_sig_t const *sig);

/**
 * @brief Converts a hash value to a bigval_t.
 *
 * @param tgt         [in] Pointer to destination buffer
 * @param hashp       [in] Pointer to hash buffer
 * @param hashlen     [in] Hash buffer size
 *
 * @return 
 *
 */
void qsee_ECC_hash_to_bigval(QSEE_bigval_t *tgt,
                             void const *hashp,
                             unsigned int hashlen);

/**
 * @brief inject error for testing 
 *
 * @param buf          [in] Data to inject error
 * @param len          [in] Length of data
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_toggle_random_bit(void *buf,
                           size_t len);

/**
 * @brief software base random bytes generator
 *
 * @param buf         [in] Pointer to random bytes buffer
 * @param len         [in] Buffer size
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_get_random_bytes(void *buf,
                          int len);

/**
 * @brief in_curveP
 *
 * @param P         [in] Pointer to affine point type var.
 *
 * @return TRUE on success, FALSE otherwise
 *
 */
bool qsee_in_curveP(QSEE_affine_point_t const *P);

 /**
 * @Initializes a domain from curve hex strings and the cofactor.
 *
 * @param dp             [out] pointer to ECC domain context
 * @param modulus        [in]  pointer to modulus 
 * @param a              [in]  pointer to a 
 * @param b              [in]  pointer to b  
 * @param x              [in]  pointer to x  
 * @param y              [in]  pointer to y   
 * @param n              [in]  pointer to n    
 * @param cofactor       [in] cofactor    
 *
 * @return 0 on success, -1 on error
 *
 */

int qsee_SW_GENERIC_ECC_init(QSEE_qrlbn_ecc_domain_t *dp,
                             char *modulus,
                             char *a,
                             char* b,
                             char *x,
                             char *y,
                             char *n,
                             unsigned cofactor);

/**
 * @brief sign data with ECC private key
 *
 * @param msgdgst           [in]  pointer to msg that needs to be signed
 * @param msgdgst_len       [in]  msg length in Bytes
 * @param privkey           [in]  pointer to private key for signing
 * @param sig               [out] pointer to signed msg, which is signature
 * @param sig_len           [out] signature size in Bytes
 * @param dp                [in]  ECC domain context
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_SW_GENERIC_ECDSA_sign(uint8_t *msgdgst,
                               uint32_t msgdgst_len,
                               QSEE_qrlbn_ecc_bigval_t *privkey,
                               QSEE_qrlbn_ECDSA_sig_t *sig,
                               QSEE_qrlbn_ecc_domain_t *dp);

/**
 * @brief verify data with ECC public key
 *
 * @param msgdgst           [in]  pointer to msg that needs to be signed
 * @param msgdgst_len       [in]  msg length in Bytes
 * @param pubkey            [in]  pointer to public key for verification
 * @param sig               [out] pointer to signed msg to be verfied
 * @param sig_len           [out] sig_len in Bytes
 * @param dp                [in]  ECC domain context
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_SW_GENERIC_ECDSA_verify(uint8_t *msgdgst,
                                 uint32_t msgdgst_len,
                                 QSEE_qrlbn_ecc_affine_point_t *pubkey,
                                 QSEE_qrlbn_ECDSA_sig_t *sig,
                                 QSEE_qrlbn_ecc_domain_t *dp);

/**
 * @brief generate shared key out of Alice' public key and Bob's private key
 *
 * @param shared_key   [out] shared key bewteen Alice and Bob
 * @param pubkey       [in] pointer to ECC public key
 * @param privkey      [in] pointer to ECC private key
 *
 * @return 0 on success, -1 on error
 *
 */

int qsee_SW_GENERIC_ECDH_shared_key_derive(QSEE_qrlbn_ecc_bigval_t *shared_key,
                                          QSEE_qrlbn_ecc_bigval_t *privkey,
                                          QSEE_qrlbn_ecc_affine_point_t *pubkey,
                                          QSEE_qrlbn_ecc_domain_t *dp);

/**
 * @brief generate public key and private key of ECC
 *
 * @param privkey              [out] pointer to private key 
 * @param pubkey               [out] pointer to public key
 * @param dp                   [in]  ECC domain context
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_SW_GENERIC_ECC_keypair_generate(QSEE_qrlbn_ecc_bigval_t *privkey,
                                        QSEE_qrlbn_ecc_affine_point_t  *pubkey,
                                        QSEE_qrlbn_ecc_domain_t *dp);

/**
  Generates an ECC public key for a given private key.

  @param[in] privkey               Pointer to private key.
  @param[out] pubkey               Pointer to public key.
  @param[in]  dp                   Pointer to ECC domain context.

  @return
  SUCCESS -- 0 \n
  FAILURE -- Negative
 */
int qsee_SW_GENERIC_ECC_pubkey_generate(QSEE_qrlbn_ecc_bigval_t const *privkey,
                                        QSEE_qrlbn_ecc_affine_point_t  *pubkey,
                                        QSEE_qrlbn_ecc_domain_t const *dp);

 /**
 * @brief Convert data from Binary to Bigval
 *
 * @param dst              [out] pointer to destination data 
 * @param src              [in]  pointer to source data
 * @param srclen           [in]  length of source data in bytes 
 * @param dp               [in]  ECC domain context 
 * @param tag              [in]  Data type tag
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_SW_GENERIC_ECC_binary_to_bigval(QSEE_qrlbn_ecc_bigval_t *dst,
                                        const void *src,
                                        size_t srclen,
                                        const QSEE_qrlbn_ecc_domain_t *dp,
                                        QSEE_qrlbn_field_tag_t tag);

/**
 * @brief Convert data from Bigval to Binary
 *
 * @param dst              [out] pointer to destination data
 * @param dstlen           [in]  length of destination data in bytes 
 * @param src              [in]  pointer to source data 
 * @param dp               [in]  ECC domain context 
 * @param tag              [in]  Data type tag
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_SW_GENERIC_ECC_bigval_to_binary(uint8_t *dst,
                                        size_t dstlen,
                                        const QSEE_qrlbn_ecc_bigval_t *src,
                                        const QSEE_qrlbn_ecc_domain_t *dp,
                                        QSEE_qrlbn_field_tag_t tag);

/**
 * @brief Compare mem. It is not constant time comparison and intended for
 * only testing purpose
 *
 * @param a              [in]  pointer to data
 * @param b              [in]  pointer to data
 * @param mdp            [in]  ECC modulus data
 *
 * @return 0 on success, -1 on error *
 */
int qsee_SW_GENERIC_ECC_compare(const uint32_t *a,
                                const uint32_t *b,
                                const QSEE_qrlbn_modulus_data_t *mdp);

/* 
 * @brief Converts input to bigval.  Input can be network order binary, ascii
 * hex, or ascii decimal, as indicated by radix (256, 16, 10,
 * respectively).  When mode is 256 (binary), srclen indicates the
 * length.  Otherwise, srclen is ignored and the input must be null
 * terminated.  Returns 0 on success, -1 on error.  If the data cannot
 * be represented, an error is indicated.
 *
 * @param dst           [out]  pointer to destination
 * @param src           [in]   pointer to source
 * @param srclen        [in]   length of source in bytes
 * @param base          [in]   base for conversion mode
 *
 * @return 0 on success, -1 on error *
 */
int qsee_SW_GENERIC_ECC_convert_input_to_bigval(QSEE_qrlbn_ecc_bigval_t *dst,
                                                const void* src,
                                                size_t srclen,
                                                int base);

int qsee_SW_ECIES_init(QSEE_ecies_ctx_t *ctx,
                       QSEE_ecies_params_t *params);

int qsee_SW_ECIES_update(QSEE_ecies_ctx_t *ctx,
                         QSEE_ecies_key_t *key,
                         QSEE_ecies_purpose_t purpose,
                         const uint8_t *msg,
                         const uint32_t msg_len,
                         uint8_t **ppAD,
                         uint32_t *pAD_len,
                         uint8_t *out,
                         uint32_t *out_len);

int qsee_SW_ECIES_finish(QSEE_ecies_ctx_t *ctx);

/**
 * @brief qsee_SW_GENERIC_ECC_affine_point_on_curve
 *
 * @param Q         [in] Pointer to affine point type var.
 * @param dp        [in] Pointer to the domain curve 
 * 
 * @return TRUE on success, FALSE otherwise
 *
 */
bool qsee_SW_GENERIC_ECC_affine_point_on_curve(
                                        QSEE_qrlbn_ecc_affine_point_t const *Q,
                                        QSEE_qrlbn_ecc_domain_t *dp);
#endif /*QSEE_ECC_H*/
