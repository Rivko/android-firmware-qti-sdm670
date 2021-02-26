#ifndef GPTYPES_H
#define GPTYPES_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/*----------------------------------------------------------------------------
 * TEE Core framework constants
 * -------------------------------------------------------------------------*/
#define TEEC_SUCCESS                          TEE_SUCCESS
#define TEEC_ERROR_GENERIC                    TEE_ERROR_GENERIC
#define TEEC_ERROR_ACCESS_DENIED              TEE_ERROR_ACCESS_DENIED
#define TEEC_ERROR_CANCEL                     TEE_ERROR_CANCEL
#define TEEC_ERROR_ACCESS_CONFLICT            TEE_ERROR_ACCESS_CONFLICT
#define TEEC_ERROR_EXCESS_DATA                TEE_ERROR_EXCESS_DATA
#define TEEC_ERROR_BAD_FORMAT                 TEE_ERROR_BAD_FORMAT
#define TEEC_ERROR_BAD_PARAMETERS             TEE_ERROR_BAD_PARAMETERS
#define TEEC_ERROR_BAD_STATE                  TEE_ERROR_BAD_STATE
#define TEEC_ERROR_ITEM_NOT_FOUND             TEE_ERROR_ITEM_NOT_FOUND
#define TEEC_ERROR_NOT_IMPLEMENTED            TEE_ERROR_NOT_IMPLEMENTED
#define TEEC_ERROR_NOT_SUPPORTED              TEE_ERROR_NOT_SUPPORTED
#define TEEC_ERROR_NO_DATA                    TEE_ERROR_NO_DATA
#define TEEC_ERROR_OUT_OF_MEMORY              TEE_ERROR_OUT_OF_MEMORY
#define TEEC_ERROR_BUSY                       TEE_ERROR_BUSY
#define TEEC_ERROR_COMMUNICATION              TEE_ERROR_COMMUNICATION
#define TEEC_ERROR_SECURITY                   TEE_ERROR_SECURITY
#define TEEC_ERROR_SHORT_BUFFER               TEE_ERROR_SHORT_BUFFER
#define TEEC_ERROR_TARGET_DEAD                TEE_ERROR_TARGET_DEAD

/* ------------------------------------------------------------------------
** GP Arithmetic Typedefs
** ------------------------------------------------------------------------ */

#define QTEE_ERR_OSSL  0X1
#define QTEE_ERR_LEN   0x2

#define BIGINT_DATA_OFFSET 1
#define TEE_TAIL_SIZE 1
#define DATA_LEN_MAX ((unsigned int)(0x0FFE))

typedef struct
{
  int err            :  3;  /* 3 bits */
  unsigned int neg   :  1;  /* 0 is positive, 1 is negative */
  unsigned int flags :  4;
  unsigned int top   : 12;
  unsigned int dmax  : 12;  /* this means at most 32 * (1<<12-1) bits, i.e. 131040 */
} TEE_BigIntHead;

/* TEE crypto GP macros */

//size in bytes
#define TEE_ALG_MD5_DIGEST_LENGTH             (128 / 8)
#define TEE_ALG_SHA1_DIGEST_LENGTH            (160 / 8)
#define TEE_ALG_SHA224_DIGEST_LENGTH          (224 / 8)
#define TEE_ALG_SHA256_DIGEST_LENGTH          (256 / 8)
#define TEE_ALG_SHA384_DIGEST_LENGTH          (384 / 8)
#define TEE_ALG_SHA512_DIGEST_LENGTH          (512 / 8)

//size in bytes
#define TEE_ALG_MD5_BLK_SIZE             (64)
#define TEE_ALG_SHA1_BLK_SIZE            (64)
#define TEE_ALG_SHA224_BLK_SIZE          (64)
#define TEE_ALG_SHA256_BLK_SIZE          (64)
#define TEE_ALG_SHA384_BLK_SIZE          (128)
#define TEE_ALG_SHA512_BLK_SIZE          (128)

//size in bytes
#define TEE_ALG_AES_BLK_SIZE                  16
#define TEE_ALG_DES_BLK_SIZE                  8
#define TEE_ALG_DES3_BLK_SIZE                 TEE_ALG_DES_BLK_SIZE

//size in bits
#define TEE_ALG_AES128_LENGTH                 (16*8)
#define TEE_ALG_AES192_LENGTH                 (24*8)
#define TEE_ALG_AES256_LENGTH                 (32*8)

//size in bits
#define TEE_ALG_DES_LENGTH                    64
#define TEE_ALG_DES3_LENGTH_128               128
#define TEE_ALG_DES3_LENGTH_192               192

//size in bits
#define TEE_ALG_HMAC_MD5_KEY_MIN              64
#define TEE_ALG_HMAC_MD5_KEY_MAX              512

#define TEE_ALG_HMAC_SHA1_KEY_MIN             80
#define TEE_ALG_HMAC_SHA1_KEY_MAX             512

#define TEE_ALG_HMAC_SHA256_KEY_MIN           192
#define TEE_ALG_HMAC_SHA256_KEY_MAX           1024

#define TEE_ALG_HMAC_SHA224_KEY_MIN           112
#define TEE_ALG_HMAC_SHA224_KEY_MAX           512

#define TEE_ALG_HMAC_SHA512_KEY_MIN           256
#define TEE_ALG_HMAC_SHA512_KEY_MAX           1024

#define TEE_ALG_HMAC_SHA384_KEY_MIN           256
#define TEE_ALG_HMAC_SHA384_KEY_MAX           1024

#define TEE_ALG_RSA_MIN_KEY_SIZE              256
#define TEE_ALG_RSA_MAX_KEY_SIZE              QSEE_MAX_KEY_SIZE

#define TEE_ALG_GENERIC_SECRET_MAX_KEY_SIZE   4096
#define TEE_DH_MIN_KEY_SIZE                   256
#define TEE_DH_MAX_KEY_SIZE                   2048

#define TEE_DSA_SHA1_MIN_KEY_SIZE             512
#define TEE_DSA_SHA1_MAX_KEY_SIZE             1024
#define TEE_DSA_SHA224_KEY_SIZE               2048
#define TEE_DSA_SHA256_MIN_KEY_SIZE           2048
#define TEE_DSA_SHA256_MAX_KEY_SIZE           3072

#define TEE_ECC_CURVE_NIST_P192_SIZE          192
#define TEE_ECC_CURVE_NIST_P224_SIZE          224
#define TEE_ECC_CURVE_NIST_P256_SIZE          256
#define TEE_ECC_CURVE_NIST_P384_SIZE          384
#define TEE_ECC_CURVE_NIST_P521_SIZE          521

/*----------------------------------------------------------------------------
 * Implementation specific errors
 * -------------------------------------------------------------------------*/
#define TEE_ERROR_UNUSED                            0x80000000
#define TEE_ERROR_GENERATE_RANDOM_FAILED            0x80000001
#define TEE_ERROR_CURRENT_SESSION_NOT_SET           0x80000002
#define TEE_ERROR_INVALID_SESSION_ID                0x80000003
#define TEE_ERROR_APP_INIT_FAILED                   0x80000004
#define TEE_ERROR_QSEECOM_REQUEST_INCORRECT_SIZE    0x80000005
#define TEE_ERROR_QSEECOM_RESPONSE_INCORRECT_SIZE   0x80000006
#define TEE_ERROR_QSEECOM_INVALID_GP_COMMAND        0x80000007
#define TEE_ERROR_INVALID_LOGIN_METHOD              0x80000008
#define TEE_ERROR_NON_MONOTONIC_TIME                0x80000009

/*----------------------------------------------------------------------------
 * Structure of TEE_ALG_* and TEE_TYPE_* identifier values
 * - described in the TEE Internal Core API Specification section 6.10.1
 *
 * Bits 31-28: Algorithm or Object class                  Mask 0xF0000000
 * Bits 27-25: RFU (should be zero)                       Mask 0x0E000000
 * Bit  24   : Object Type Key Pair indicator             Mask 0x01000000
 * Bits 23-20: Hash used by MGF for RSA OAEP              Mask 0x00F00000
 * Bits 19-16: MGF indicator                              Mask 0x000F0000
 * Bits 15-12: Message Digest for Asymmetric Signatures   Mask 0x0000F000
 * Bits 11-8 : Chaining Mode or Padding (maybe!)          Mask 0x00000F00
 * Bits  7-0 : Underlying main algorithm                  Mask 0x000000FF
 * -------------------------------------------------------------------------*/
#define TEE_OBJECT_CLASS_MASK                       0xF0000000
#define TEE_OBJECT_CLASS(id)                        ( (id) & TEE_OBJECT_CLASS_MASK )
#define TEE_OBJECT_CLASS_BLOCKCIPHER                0x10000000
#define TEE_OBJECT_CLASS_MAC                        0x30000000
#define TEE_OBJECT_CLASS_AECIPHER                   0x40000000
#define TEE_OBJECT_CLASS_DIGEST                     0x50000000
#define TEE_OBJECT_CLASS_ASYMMETRICCIPHER           0x60000000
#define TEE_OBJECT_CLASS_ASYMMETRICSIGNATURE        0x70000000
#define TEE_OBJECT_CLASS_KEYDERIVATION              0x80000000
#define TEE_OBJECT_CLASS_OBJECTHANDLE               0xA0000000

#define TEE_OBJECT_RFU_MASK                         0x0E000000
#define TEE_OBJECT_RFU(id)                          ( (id) & TEE_OBJECT_RFU_MASK )

#define TEE_OBJECT_KEYPAIR_MASK                     0x01000000
#define TEE_OBJECT_KEYPAIR(id)                      ( (id) & TEE_OBJECT_KEYPAIR_MASK )
#define TEE_OBJECT_KEYPAIR_NO                       0x00000000
#define TEE_OBJECT_KEYPAIR_YES                      TEE_OBJECT_KEYPAIR_MASK

#define TEE_OBJECT_MGFHASH_MASK                     0x00F00000  /* If bits 19-16 indicate MGF */
#define TEE_OBJECT_MGFHASH(id)                      ( (id) & TEE_OBJECT_MGFHASH_MASK )
#define TEE_OBJECT_MGFHASH_MD5                      0x00100000
#define TEE_OBJECT_MGFHASH_SHA1                     0x00200000
#define TEE_OBJECT_MGFHASH_SHA224                   0x00300000
#define TEE_OBJECT_MGFHASH_SHA256                   0x00400000
#define TEE_OBJECT_MGFHASH_SHA384                   0x00500000
#define TEE_OBJECT_MGFHASH_SHA512                   0x00600000

#define TEE_OBJECT_MGF_MASK                         0x000F0000
#define TEE_OBJECT_MGF(id)                          ( (id) & TEE_OBJECT_MGF_MASK )
#define TEE_OBJECT_MGF_NO                           0x00000000
#define TEE_OBJECT_MGF_YES                          0x00010000

#define TEE_OBJECT_ASIG_DIGEST_MASK                 0x0000F000  /* If bits 31-28 are 0x7 */
#define TEE_OBJECT_ASIG_DIGEST(id)                  ( (id) & TEE_OBJECT_DIGEST_MASK )
#define TEE_OBJECT_ASIG_DIGEST_NOTECC_MD5           0x00001000  /* If bits 7-0 are not 0x41 or 0x42 */
#define TEE_OBJECT_ASIG_DIGEST_NOTECC_SHA1          0x00002000  /* If bits 7-0 are not 0x41 or 0x42 */
#define TEE_OBJECT_ASIG_DIGEST_NOTECC_SHA224        0x00003000  /* If bits 7-0 are not 0x41 or 0x42 */
#define TEE_OBJECT_ASIG_DIGEST_NOTECC_SHA256        0x00004000  /* If bits 7-0 are not 0x41 or 0x42 */
#define TEE_OBJECT_ASIG_DIGEST_NOTECC_SHA384        0x00005000  /* If bits 7-0 are not 0x41 or 0x42 */
#define TEE_OBJECT_ASIG_DIGEST_NOTECC_SHA512        0x00006000  /* If bits 7-0 are not 0x41 or 0x42 */
#define TEE_OBJECT_ASIG_DIGEST_ECC_SHA1             0x00001000  /* If bits 7-0 are 0x41 or 0x42 */
#define TEE_OBJECT_ASIG_DIGEST_ECC_SHA224           0x00002000  /* If bits 7-0 are 0x41 or 0x42 */
#define TEE_OBJECT_ASIG_DIGEST_ECC_SHA256           0x00003000  /* If bits 7-0 are 0x41 or 0x42 */
#define TEE_OBJECT_ASIG_DIGEST_ECC_SHA384           0x00004000  /* If bits 7-0 are 0x41 or 0x42 */
#define TEE_OBJECT_ASIG_DIGEST_ECC_SHA512           0x00005000  /* If bits 7-0 are 0x41 or 0x42 */

#define TEE_OBJECT_CHAIN_PAD_MASK                   0x00000F00  /* Bits 31-28 0x7 or 0x8 */
#define TEE_OBJECT_CHAIN_PAD(id)                    ( (id) & TEE_OBJECT_CHAIN_PAD_MASK )
#define TEE_OBJECT_CHAIN_PAD_ECBNOPAD               0x00000000
#define TEE_OBJECT_CHAIN_PAD_CBCNOPAD               0x00000100
#define TEE_OBJECT_CHAIN_PAD_PKCS1OAEP              0x00000200
#define TEE_OBJECT_CHAIN_PAD_CTS                    0x00000300
#define TEE_OBJECT_CHAIN_PAD_XTS                    0x00000400
#define TEE_OBJECT_CHAIN_PAD_PKCS5CBC               0x00000500
#define TEE_OBJECT_CHAIN_PAD_CMAC                   0x00000600
#define TEE_OBJECT_CHAIN_PAD_CCM                    0x00000700
#define TEE_OBJECT_CHAIN_PAD_GCM                    0x00000800  /* Bits 31-28 0x4 */
#define TEE_OBJECT_CHAIN_PAD_PKCS1V1P5              0x00000800  /* Bits 31-28 0x7 */
#define TEE_OBJECT_CHAIN_PAD_PKCS1PSS               0x00000900

#define TEE_OBJECT_CRYPTO_ALG_MASK                  0x000000FF
#define TEE_OBJECT_CRYPTO_ALG(id)                   ( (id) & TEE_OBJECT_CRYPTO_ALG_MASK )
#define TEE_OBJECT_CRYPTO_ALG_GENERICSECRET         0x00000000
#define TEE_OBJECT_CRYPTO_ALG_MD5                   0x00000001
#define TEE_OBJECT_CRYPTO_ALG_SHA1                  0x00000002
#define TEE_OBJECT_CRYPTO_ALG_SHA224                0x00000003
#define TEE_OBJECT_CRYPTO_ALG_SHA256                0x00000004
#define TEE_OBJECT_CRYPTO_ALG_SHA384                0x00000005
#define TEE_OBJECT_CRYPTO_ALG_SHA512                0x00000006
#define TEE_OBJECT_CRYPTO_ALG_AES                   0x00000010
#define TEE_OBJECT_CRYPTO_ALG_DES                   0x00000011
#define TEE_OBJECT_CRYPTO_ALG_DES3                  0x00000013
#define TEE_OBJECT_CRYPTO_ALG_RSA                   0x00000030
#define TEE_OBJECT_CRYPTO_ALG_DSA                   0x00000031
#define TEE_OBJECT_CRYPTO_ALG_DH                    0x00000032
#define TEE_OBJECT_CRYPTO_ALG_ALG_ECC               0x00000042  /* for Operations */
#define TEE_OBJECT_CRYPTO_ALG_OBJ_ECDSA             0x00000041  /* for Objects */
#define TEE_OBJECT_CRYPTO_ALG_OBJ_ECDH              0x00000042  /* for Objects */
#define TEE_OBJECT_CRYPTO_ALG_OBJECTINVALID         0x000000BE
#define TEE_OBJECT_CRYPTO_ALG_NOAPPROPRIATE         0x000000BF

/*----------------------------------------------------------------------------
 * Utility macros
 * -------------------------------------------------------------------------*/

/* Convert between bit and byte counts */
#define TEE_BITS2BYTES(bits)                        ( ((bits) + 7) / 8 )
#define TEE_BYTES2BITS(bytes)                       ( (bytes) * 8 )

/*----------------------------------------------------------------------------
 * Include Files for various GP APIs
 * -------------------------------------------------------------------------*/

#include "tee_internal_api.h"
#include "gpAppLibMain.h"
#include "qsee_log.h"

#ifdef _SIMULATOR_ENV
#include "SimulatorEnv.h"
#endif

#endif /* GPTYPES_H */
