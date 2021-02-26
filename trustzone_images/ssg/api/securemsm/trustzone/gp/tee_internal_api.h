// proposed merge between 1.0 and 1.1

#ifndef   __TEE_INTERNAL_API_H__
#define   __TEE_INTERNAL_API_H__

#ifndef TA_EXPORT
#define TA_EXPORT
#endif

//#define TEE_NO_TRACE
//#define TEE_INCLUDE_DEPRECATED_ECC_IDENTIFIERS

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/*----------------------------------------------------------------------------
 * Table of Contents
 * -------------------------------------------------------------------------*/
/*
 * 1. Basic type definitions
 *    a) Basic types
 * 2. TEE type definitions
 *    a) TEE Data types
 *        -TEE Core Framework data types (UUID, params, sessions, etc..)
 *        -TEE Trusted Storage types
 *        -TEE Crypto types
 *        -TEE Time types
 *        -TEE Arithmetical types
 * 3. TEE Constants
 *    a) TEE Core Framework constants
 *        -TEE Error codes
 *        -TEE Param types
 *        -TEE Login types
 *        -TEE Origin codes
 *        -TEE Property Set pseudo-handles
 *        -TEE Memory Access rights
 *        -TEE Handle values
 *    b) TEE Trusted Storage constants
 *        -TEE Object Storage constants
 *        -TEE Data Flag constants
 *        -TEE Usage constants
 *        -TEE Handle Flag constants
 *        -TEE Operation constants
 *        -TEE Miscellaneous constants
 *    c) TEE Crypto constants
 *        -TEE Crypto algorithms
 *        -TEE Transient Object types
 *        -TEE Object/Operation attributes
 *        -TEE attribute ID flags
 *
 */

/*----------------------------------------------------------------------------
 * Type definitions
 * -------------------------------------------------------------------------*/

/* Return type */
typedef uint32_t TEE_Result;
typedef TEE_Result TEEC_Result;

/* TEE Arithmetical types */
typedef uint32_t TEE_BigInt; /* placeholder for large multi-precision int */
typedef uint32_t TEE_BigIntFMMContext; /* placeholder for context */
typedef uint32_t TEE_BigIntFMM; /* placeholder for mem structure of Fast modular mult */

#ifndef NULL
#define NULL 0
#endif

typedef struct TEE_Buffer
{
  void*     buffer;
  size_t    size;
} TEE_Buffer;

typedef struct TEE_Value
{
  uint32_t a;
  uint32_t b;
} TEE_Value;

typedef union TEE_Param
{
  TEE_Buffer  memref;
  TEE_Value   value;
} TEE_Param;

/*----------------------------------------------------------------------------
 * Generic GP Constants
 * -------------------------------------------------------------------------*/
#define MAX_GP_PARAMS                               4

/*---------------------------------------------------------
    Common type definition
---------------------------------------------------------*/
typedef struct
{
   uint32_t timeLow;
   uint16_t timeMid;
   uint16_t timeHiAndVersion;
   uint8_t  clockSeqAndNode[8];
} TEE_UUID;

/*---------------------------------------------------------
    Type definition for Trusted Core Framework API
    ---------------------------------------------------------*/
typedef struct TEE_Identity
{
  uint32_t login;
  TEE_UUID uuid;
} TEE_Identity;

// Sessions
typedef struct __TEE_TASessionHandle* TEE_TASessionHandle; /* Opaque handle */

#define TA_EXPORT //__declspec(dllexport)

// Property Set
struct __TEE_PropSetHandle;
typedef struct __TEE_PropSetHandle* TEE_PropSetHandle; /* Opaque handle */

/*---------------------------------------------------------
    Type definition for Time API
    ---------------------------------------------------------*/
typedef struct
{
  uint32_t seconds;
  uint32_t millis;
} TEE_Time;

/*---------------------------------------------------------
    Type definition for Data Storage
    ---------------------------------------------------------*/
typedef struct
{
  uint32_t attributeID;
  union
  {
    struct
    {
      void* buffer; /* [inbuf] */
      size_t length;
    } ref;
    struct
    {
      uint32_t a, b;
    } value;
  } content;
} TEE_Attribute;

typedef struct
{
  uint32_t objectType;
  uint32_t objectSize;
  uint32_t maxObjectSize;
  uint32_t objectUsage;
  uint32_t dataSize;
  uint32_t dataPosition;
  uint32_t handleFlags;
} TEE_ObjectInfo;

typedef enum
{
  TEE_DATA_SEEK_SET = 0,
  TEE_DATA_SEEK_CUR,
  TEE_DATA_SEEK_END
} TEE_Whence;

// Obj
struct __TEE_ObjectHandle;
typedef struct __TEE_ObjectHandle* TEE_ObjectHandle; /* Opaque handle */

// Obj enum
struct __TEE_ObjectEnumHandle;
typedef struct __TEE_ObjectEnumHandle* TEE_ObjectEnumHandle; /* Opaque handle */

/* TEE Crypto types */

// Operation mode
typedef enum {
  TEE_MODE_ENCRYPT  = 0,
  TEE_MODE_DECRYPT  = 1,
  TEE_MODE_SIGN     = 2,
  TEE_MODE_VERIFY   = 3,
  TEE_MODE_MAC      = 4,
  TEE_MODE_DIGEST   = 5,
  TEE_MODE_DERIVE   = 6
} TEE_OperationMode;

// Operation info
typedef struct
{
  uint32_t algorithm;
  uint32_t operationClass;
  uint32_t mode;
  uint32_t digestLength;
  uint32_t maxKeySize;
  uint32_t keySize;
  uint32_t requiredKeyUsage;
  uint32_t handleState;
} TEE_OperationInfo;

typedef struct
{
  uint32_t keySize;
  uint32_t requiredKeyUsage;
} TEE_OperationInfoKey;

typedef struct
{
  uint32_t             algorithm;
  uint32_t             operationClass;
  uint32_t             mode;
  uint32_t             digestLength;
  uint32_t             maxKeySize;
  uint32_t             handleState;
  uint32_t             operationState;
  uint32_t             numberOfKeys;
  TEE_OperationInfoKey keyInformation[];
} TEE_OperationInfoMultiple; // +

// Operation Handle
struct __TEE_OperationHandle;
typedef struct __TEE_OperationHandle* TEE_OperationHandle; /* Opaque handle */

/*----------------------------------------------------------------------------
 * TEE Core framework constants
 * -------------------------------------------------------------------------*/

/* TEE Error codes */

#define TEE_SUCCESS                           0x00000000    /* Operation success */
#define TEE_ERROR_CORRUPT_OBJECT              0xF0100001
#define TEE_ERROR_CORRUPT_OBJECT_2            0xF0100002
#define TEE_ERROR_STORAGE_NOT_AVAILABLE       0xF0100003
#define TEE_ERROR_STORAGE_NOT_AVAILABLE_2     0xF0100004
#define TEE_ERROR_GENERIC                     0xFFFF0000    /* Non-specific error */
#define TEE_ERROR_ACCESS_DENIED               0xFFFF0001    /* Access privileges are not sufficient */
#define TEE_ERROR_CANCEL                      0xFFFF0002    /* Operation was cancelled */
#define TEE_ERROR_ACCESS_CONFLICT             0xFFFF0003    /* Concurrent accesses caused conflict */
#define TEE_ERROR_EXCESS_DATA                 0xFFFF0004    /* Too much data for the requested operation was provided */
#define TEE_ERROR_BAD_FORMAT                  0xFFFF0005    /* The input data was of invalid format */
#define TEE_ERROR_BAD_PARAMETERS              0xFFFF0006    /* The input parameters were invalid */
#define TEE_ERROR_BAD_STATE                   0xFFFF0007    /* The operation is not valid in current state */
#define TEE_ERROR_ITEM_NOT_FOUND              0xFFFF0008    /* The requested data could not be found */
#define TEE_ERROR_NOT_IMPLEMENTED             0xFFFF0009    /* The operation should be implemented, but does not currently exist */
#define TEE_ERROR_NOT_SUPPORTED               0xFFFF000A    /* The operation is valid, but not supported */
#define TEE_ERROR_NO_DATA                     0xFFFF000B    /* Expected data was missing */
#define TEE_ERROR_OUT_OF_MEMORY               0xFFFF000C    /* System ran out of resources */
#define TEE_ERROR_BUSY                        0xFFFF000D    /* System is busy with something else */
#define TEE_ERROR_COMMUNICATION               0xFFFF000E    /* Communication with a remote party failed */
#define TEE_ERROR_SECURITY                    0xFFFF000F    /* A security fault was detected */
#define TEE_ERROR_SHORT_BUFFER                0xFFFF0010    /* The buffer is NULL or is too small */
#define TEE_ERROR_EXTERNAL_CANCEL             0xFFFF0011    /* An external event has caused a User Interface operation to be aborted*/
#define TEE_PENDING                           0xFFFF2000    /* The operation is pending action */
#define TEE_ERROR_TIMEOUT                     0xFFFF3001    /* The operation has timed out and could not complete */
#define TEE_ERROR_OVERFLOW                    0xFFFF300F    /* An integer overflow has occurred */
#define TEE_ERROR_TARGET_DEAD                 0xFFFF3024    /* TA has shut down */
#define TEE_ERROR_STORAGE_NO_SPACE            0xFFFF3041    /* Insufficient space available to create persistent object */
#define TEE_ERROR_MAC_INVALID                 0xFFFF3071    /* The computed MAC does not correspond with passed-in param */
#define TEE_ERROR_SIGNATURE_INVALID           0xFFFF3072    /* The signature is invalid */
#define TEE_ERROR_TIME_NOT_SET                0xFFFF5000    /* Persistent object is not currently set */
#define TEE_ERROR_TIME_NEEDS_RESET            0xFFFF5001    /* Persistent object is set, but may be corrupted and can no longer be trusted */
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

/* TEE Login types */
#define TEE_LOGIN_PUBLIC                      0x00000000
#define TEE_LOGIN_USER                        0x00000001
#define TEE_LOGIN_GROUP                       0x00000002
#define TEE_LOGIN_APPLICATION                 0x00000004
#define TEE_LOGIN_APPLICATION_USER            0x00000005
#define TEE_LOGIN_APPLICATION_GROUP           0x00000006
#define TEE_LOGIN_TRUSTED_APP                 0xF0000000

/* TEE Param types */
#define TEE_PARAM_TYPE_NONE                   0x0
#define TEE_PARAM_TYPE_VALUE_INPUT            0x1
#define TEE_PARAM_TYPE_VALUE_OUTPUT           0x2
#define TEE_PARAM_TYPE_VALUE_INOUT            0x3
#define TEE_PARAM_TYPE_MEMREF_INPUT           0x5
#define TEE_PARAM_TYPE_MEMREF_OUTPUT          0x6
#define TEE_PARAM_TYPE_MEMREF_INOUT           0x7

/* TEE Memory Access rights */
#define TEE_MEMORY_ACCESS_READ                0x1
#define TEE_MEMORY_ACCESS_WRITE               0x2
#define TEE_MEMORY_ACCESS_ANY_OWNER           0x4

//Macros to retrieve Param Types
#define TEE_PARAM_TYPES(t0,t1,t2,t3)          ((t0) | ((t1) << 4) | ((t2) << 8) | ((t3) << 12))
#define TEE_PARAM_TYPE_GET(t, i)              (((t) >> (i*4)) & 0xF)
#define TEE_BigIntSizeInU32(n) ((((n)+31)/32)+2)

/* TEE Origin codes */
#define TEE_ORIGIN_API                        0x1
#define TEE_ORIGIN_COMMS                      0x2
#define TEE_ORIGIN_TEE                        0x3
#define TEE_ORIGIN_TRUSTED_APP                0x4

#define TEE_TIMEOUT_INFINITE                  0xFFFFFFFF

/* TEE Property Set pseudo-handles */
#define TEE_PROPSET_CURRENT_TA                (TEE_PropSetHandle)0xFFFFFFFF
#define TEE_PROPSET_CURRENT_CLIENT            (TEE_PropSetHandle)0xFFFFFFFE
#define TEE_PROPSET_TEE_IMPLEMENTATION        (TEE_PropSetHandle)0xFFFFFFFD

/*----------------------------------------------------------------------------
 * TEE Trusted storage constants
 * -------------------------------------------------------------------------*/
/* Object storage constants */
#define TEE_HANDLE_NULL                       (NULL)
#define TEE_OBJECT_STORAGE_PRIVATE            0x00000001

/* Data Flag constants */
#define TEE_DATA_FLAG_ACCESS_READ             0x00000001
#define TEE_DATA_FLAG_ACCESS_WRITE            0x00000002
#define TEE_DATA_FLAG_ACCESS_WRITE_META       0x00000004
#define TEE_DATA_FLAG_SHARE_READ              0x00000010
#define TEE_DATA_FLAG_SHARE_WRITE             0x00000020
#define TEE_DATA_FLAG_CREATE                  0x00000200
#define TEE_DATA_FLAG_OVERWRITE               0x00000400
#define TEE_DATA_FLAG_EXCLUSIVE               0x00000400

/* Usage constants */
#define TEE_USAGE_EXTRACTABLE                 0x00000001
#define TEE_USAGE_ENCRYPT                     0x00000002
#define TEE_USAGE_DECRYPT                     0x00000004
#define TEE_USAGE_MAC                         0x00000008
#define TEE_USAGE_SIGN                        0x00000010
#define TEE_USAGE_VERIFY                      0x00000020
#define TEE_USAGE_DERIVE                      0x00000040

/* Handle Flag constants */
#define TEE_HANDLE_FLAG_UNINITIALIZED         0x00000000
#define TEE_HANDLE_FLAG_PERSISTENT            0x00010000
#define TEE_HANDLE_FLAG_INITIALIZED           0x00020000
#define TEE_HANDLE_FLAG_KEY_SET               0x00040000
#define TEE_HANDLE_FLAG_EXPECT_TWO_KEYS       0x00080000

/* Operation constants */
#define TEE_OPERATION_CIPHER                  1
#define TEE_OPERATION_MAC                     3
#define TEE_OPERATION_AE                      4
#define TEE_OPERATION_DIGEST                  5
#define TEE_OPERATION_ASYMMETRIC_CIPHER       6
#define TEE_OPERATION_ASYMMETRIC_SIGNATURE    7
#define TEE_OPERATION_KEY_DERIVATION          8

/* Operation States */
#define TEE_OPERATION_STATE_INITIAL           0x00000000
#define TEE_OPERATION_STATE_ACTIVE            0x00000001
/* Reserved for future use                    0x00000002-0x7FFFFFFF */
/* Implementation defined                     0x80000000-0xFFFFFFFF */

/* Miscellaneous constants */
#define TEE_DATA_MAX_POSITION                 0xFFFFFFFF
#define TEE_OBJECT_ID_MAX_LEN                 64
#define TEE_STORAGE_PRIVATE                   1
#define TEE_MALLOC_FILL_ZERO                  0x00000000 // +
#define TEE_ITEM_NOT_FOUND                    0xFFFF6001          //Not defined in GP Spec continue after the last error code

/*----------------------------------------------------------------------------
 * TEE Crypto constants
 * -------------------------------------------------------------------------*/

 /* TEE Transient Object types */
#define TEE_TYPE_AES                          0xA0000010  /* 128, 192, or 256 bits */
#define TEE_TYPE_DES                          0xA0000011  /* Always 64 bits */
#define TEE_TYPE_DES3                         0xA0000013  /* 128 or 192 bits */
#define TEE_TYPE_HMAC_MD5                     0xA0000001  /* Between 64 and 512 bits, multiple of 8 bits */
#define TEE_TYPE_HMAC_SHA1                    0xA0000002  /* Between 80 and 512 bits, multiple of 8 bits */
#define TEE_TYPE_HMAC_SHA224                  0xA0000003  /* Between 112 and 512 bits, multiple of 8 bits */
#define TEE_TYPE_HMAC_SHA256                  0xA0000004  /* Between 192 and 1024 bits, multiple of 8 bits */
#define TEE_TYPE_HMAC_SHA384                  0xA0000005  /* Between 256 and 1024 bits, multiple of 8 bits */
#define TEE_TYPE_HMAC_SHA512                  0xA0000006  /* Between 256 and 1024 bits, multiple of 8 bits */
#define TEE_TYPE_RSA_PUBLIC_KEY               0xA0000030  /* Object size is the number of bits in the modulus. Min key size 256 bits, must support up to 2048 bit key sizes */
#define TEE_TYPE_RSA_KEYPAIR                  0xA1000030  /* Same as RSA public key size */
#define TEE_TYPE_DSA_PUBLIC_KEY               0xA0000031  /* Between 512 and 1024 bits, multiple of 64 bits */
#define TEE_TYPE_DSA_KEYPAIR                  0xA1000031  /* Same as DSA public key size */
#define TEE_TYPE_DH_KEYPAIR                   0xA1000032  /* From 256 to 2048 bits */
#define TEE_TYPE_ECDSA_PUBLIC_KEY             0xA0000041  /* TODO: New value - implement */
#define TEE_TYPE_ECDSA_KEYPAIR                0xA1000041  /* TODO: New value - implement */
#define TEE_TYPE_ECDH_PUBLIC_KEY              0xA0000042  /* TODO: New value - implement */
#define TEE_TYPE_ECDH_KEYPAIR                 0xA1000042  /* TODO: New value - implement */
#define TEE_TYPE_GENERIC_SECRET               0xA0000000  /* Multiple of 8 bits, up to 4096 bits */
#define TEE_TYPE_CORRUPTED_OBJECT             0xA00000BE  /* TODO: New value - implement */
#define TEE_TYPE_DATA                         0xA00000BF  /* TODO: New value - implement */

/* TEE Crypto algorithms */
#define TEE_ALG_AES_ECB_NOPAD                     0x10000010
#define TEE_ALG_AES_CBC_NOPAD                     0x10000110
#define TEE_ALG_AES_CTR                           0x10000210
#define TEE_ALG_AES_CTS                           0x10000310
#define TEE_ALG_AES_XTS                           0x10000410
#define TEE_ALG_AES_CBC_MAC_NOPAD                 0x30000110
#define TEE_ALG_AES_CBC_MAC_PKCS5                 0x30000510
#define TEE_ALG_AES_CMAC                          0x30000610
#define TEE_ALG_AES_CCM                           0x40000710
#define TEE_ALG_AES_GCM                           0x40000810
#define TEE_ALG_DES_ECB_NOPAD                     0x10000011
#define TEE_ALG_DES_CBC_NOPAD                     0x10000111
#define TEE_ALG_DES_CBC_MAC_NOPAD                 0x30000111
#define TEE_ALG_DES_CBC_MAC_PKCS5                 0x30000511
#define TEE_ALG_DES3_ECB_NOPAD                    0x10000013
#define TEE_ALG_DES3_CBC_NOPAD                    0x10000113
#define TEE_ALG_DES3_CBC_MAC_NOPAD                0x30000113
#define TEE_ALG_DES3_CBC_MAC_PKCS5                0x30000513
#define TEE_ALG_RSASSA_PKCS1_V1_5_MD5             0x70001830
#define TEE_ALG_RSASSA_PKCS1_V1_5_SHA1            0x70002830
#define TEE_ALG_RSASSA_PKCS1_V1_5_SHA224          0x70003830
#define TEE_ALG_RSASSA_PKCS1_V1_5_SHA256          0x70004830
#define TEE_ALG_RSASSA_PKCS1_V1_5_SHA384          0x70005830
#define TEE_ALG_RSASSA_PKCS1_V1_5_SHA512          0x70006830
#define TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1        0x70212930
#define TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224      0x70313930
#define TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256      0x70414930
#define TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384      0x70515930
#define TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512      0x70616930
#define TEE_ALG_RSAES_PKCS1_V1_5                  0x60000130
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1        0x60210230
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA224      0x60310230
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA256      0x60410230
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA384      0x60510230
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA512      0x60610230
#define TEE_ALG_RSA_NOPAD                         0x60000030
#define TEE_ALG_DSA_SHA1                          0x70002131
#define TEE_ALG_DSA_SHA224                        0x70003131
#define TEE_ALG_DSA_SHA256                        0x70004131
#define TEE_ALG_DH_DERIVE_SHARED_SECRET           0x80000032
#define TEE_ALG_MD5                               0x50000001
#define TEE_ALG_SHA1                              0x50000002
#define TEE_ALG_SHA224                            0x50000003
#define TEE_ALG_SHA256                            0x50000004
#define TEE_ALG_SHA384                            0x50000005
#define TEE_ALG_SHA512                            0x50000006
#define TEE_ALG_HMAC_MD5                          0x30000001
#define TEE_ALG_HMAC_SHA1                         0x30000002
#define TEE_ALG_HMAC_SHA224                       0x30000003
#define TEE_ALG_HMAC_SHA256                       0x30000004
#define TEE_ALG_HMAC_SHA384                       0x30000005
#define TEE_ALG_HMAC_SHA512                       0x30000006
#define TEE_ALG_ECDSA_SHA1                        0x70001042
#define TEE_ALG_ECDSA_SHA224                      0x70002042
#define TEE_ALG_ECDSA_SHA256                      0x70003042
#define TEE_ALG_ECDSA_SHA384                      0x70004042
#define TEE_ALG_ECDSA_SHA512                      0x70005042
#define TEE_ALG_ECDH_DERIVE_SHARED_SECRET         0x80000042

#ifdef TEE_INCLUDE_DEPRECATED_ECC_IDENTIFIERS
/* These Elliptic Curve ID values appeared in the GP Specification version 1.1, */
/* but were removed in 1.1.1 because they were deemed to be inconsistent, or in */
/* some other way incorrect.                                                    */
/*                                                                              */
/* Some ID values were removed completely; others were kept but assigned new    */
/* semantics. The latter would cause us a problem but for the fact that there   */
/* is no implementation that uses them. The only place we see them at all is in */
/* some of the GP tests, but the testing is so limited that simply aliasing the */
/* proper values should be sufficient to get through.                           */
/*                                                                              */
/* In the fullness of time the tests will switch over to the 1.1.1 values and   */
/* identifiers and then these can all be removed completely.                    */
#define TEE_ALG_ECDSA_P192                          TEE_ALG_ECDSA_SHA1    /* 0x70001042 */
#define TEE_ALG_ECDSA_P224                          TEE_ALG_ECDSA_SHA224  /* 0x70002042 */
#define TEE_ALG_ECDSA_P256                          TEE_ALG_ECDSA_SHA256  /* 0x70003042 */
#define TEE_ALG_ECDSA_P384                          TEE_ALG_ECDSA_SHA384  /* 0x70004042 */
#define TEE_ALG_ECDSA_P521                          TEE_ALG_ECDSA_SHA512  /* 0x70005042 */
#define TEE_ALG_ECDH_P192                           0x80001042            /* Same purpose as TEE_ALG_ECDH_DERIVE_SHARED_SECRET */
#define TEE_ALG_ECDH_P224                           0x80002042            /* Same purpose as TEE_ALG_ECDH_DERIVE_SHARED_SECRET */
#define TEE_ALG_ECDH_P256                           0x80003042            /* Same purpose as TEE_ALG_ECDH_DERIVE_SHARED_SECRET */
#define TEE_ALG_ECDH_P384                           0x80004042            /* Same purpose as TEE_ALG_ECDH_DERIVE_SHARED_SECRET */
#define TEE_ALG_ECDH_P521                           0x80005042            /* Same purpose as TEE_ALG_ECDH_DERIVE_SHARED_SECRET */
#define TEE_ALG_ECDH_NIST_P192_DERIVE_SHARED_SECRET TEE_ALG_ECDH_P192
#define TEE_ALG_ECDH_NIST_P224_DERIVE_SHARED_SECRET TEE_ALG_ECDH_P224
#define TEE_ALG_ECDH_NIST_P256_DERIVE_SHARED_SECRET TEE_ALG_ECDH_P256
#define TEE_ALG_ECDH_NIST_P384_DERIVE_SHARED_SECRET TEE_ALG_ECDH_P384
#define TEE_ALG_ECDH_NIST_P521_DERIVE_SHARED_SECRET TEE_ALG_ECDH_P521
#endif

/* TEE Object/Operation attributes */

#define TEE_ATTR_SECRET_VALUE                 0xC0000000
#define TEE_ATTR_LABEL                        0xC0000100  /* Proprietary - info for KDF */
#define TEE_ATTR_CONTEXT                      0xC0000200  /* Proprietary - info for KDF */
#define TEE_ATTR_INPUT_KEY                    0xC0000300  /* Proprietary - info for KDF */
#define TEE_ATTR_RSA_MODULUS                  0xD0000130
#define TEE_ATTR_RSA_PUBLIC_EXPONENT          0xD0000230
#define TEE_ATTR_RSA_PRIVATE_EXPONENT         0xC0000330
#define TEE_ATTR_RSA_PRIME1                   0xC0000430
#define TEE_ATTR_RSA_PRIME2                   0xC0000530
#define TEE_ATTR_RSA_EXPONENT1                0xC0000630
#define TEE_ATTR_RSA_EXPONENT2                0xC0000730
#define TEE_ATTR_RSA_COEFFICIENT              0xC0000830
#define TEE_ATTR_DSA_PRIME                    0xD0001031
#define TEE_ATTR_DSA_SUBPRIME                 0xD0001131
#define TEE_ATTR_DSA_BASE                     0xD0001231
#define TEE_ATTR_DSA_PUBLIC_VALUE             0xD0000131
#define TEE_ATTR_DSA_PRIVATE_VALUE            0xC0000231
#define TEE_ATTR_DH_PRIME                     0xD0001032
#define TEE_ATTR_DH_SUBPRIME                  0xD0001132
#define TEE_ATTR_DH_BASE                      0xD0001232
#define TEE_ATTR_DH_X_BITS                    0xF0001332
#define TEE_ATTR_DH_PUBLIC_VALUE              0xD0000132
#define TEE_ATTR_DH_PRIVATE_VALUE             0xC0000232
#define TEE_ATTR_RSA_OAEP_LABEL               0xD0000930
#define TEE_ATTR_RSA_PSS_SALT_LENGTH          0xF0000A30
#define TEE_ATTR_ECC_PUBLIC_VALUE_X           0xD0000141
#define TEE_ATTR_ECC_PUBLIC_VALUE_Y           0xD0000241
#define TEE_ATTR_ECC_PRIVATE_VALUE            0xC0000341
#define TEE_ATTR_ECC_CURVE                    0xF0000441

/* TEE attribute ID flags */

#define TEE_ATTR_FLAG_VALUE                   0x20000000
#define TEE_ATTR_FLAG_PUBLIC                  0x10000000

/* TEE Elliptic Curve types */

#define TEE_ECC_CURVE_NIST_P192               0x00000001
#define TEE_ECC_CURVE_NIST_P224               0x00000002
#define TEE_ECC_CURVE_NIST_P256               0x00000003
#define TEE_ECC_CURVE_NIST_P384               0x00000004
#define TEE_ECC_CURVE_NIST_P521               0x00000005
#define TEE_ECC_CURVE_RFU_START               0x00000006
#define TEE_ECC_CURVE_RFU_END                 0x7FFFFFFF
#define TEE_ECC_CURVE_PROPRIETARY_START       0x80000000
#define TEE_ECC_CURVE_PROP_BP_P160r1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 0)
#define TEE_ECC_CURVE_PROP_BP_P160t1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 1)
#define TEE_ECC_CURVE_PROP_BP_P192r1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 2)
#define TEE_ECC_CURVE_PROP_BP_P192t1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 3)
#define TEE_ECC_CURVE_PROP_BP_P224r1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 4)
#define TEE_ECC_CURVE_PROP_BP_P224t1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 5)
#define TEE_ECC_CURVE_PROP_BP_P256r1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 6)
#define TEE_ECC_CURVE_PROP_BP_P256t1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 7)
#define TEE_ECC_CURVE_PROP_BP_P320r1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 8)
#define TEE_ECC_CURVE_PROP_BP_P320t1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 9)
#define TEE_ECC_CURVE_PROP_BP_P384r1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 10)
#define TEE_ECC_CURVE_PROP_BP_P384t1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 11)
#define TEE_ECC_CURVE_PROP_BP_P512r1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 12)
#define TEE_ECC_CURVE_PROP_BP_P512t1          ((TEE_ECC_CURVE_PROPRIETARY_START) + 13)
#define TEE_ECC_CURVE_PROPRIETARY_END         0xFFFFFFFF

/*------------------------------------------------------------------------------
      Panic Function
------------------------------------------------------------------------------*/

extern void TEE_Panic(TEE_Result nPanicCode);

/*------------------------------------------------------------------------------
      Trusted Application Entry Points
------------------------------------------------------------------------------*/

extern TEE_Result TA_EXPORT TA_CreateEntryPoint(void);
extern void TA_EXPORT TA_DestroyEntryPoint(void);
extern TEE_Result TA_EXPORT TA_OpenSessionEntryPoint(
   uint32_t    nParamTypes,
   TEE_Param   pParams[4],
   void**      ppSessionContext);
extern void TA_EXPORT TA_CloseSessionEntryPoint(
   void*       pSessionContext);
extern TEE_Result TA_EXPORT TA_InvokeCommandEntryPoint(
   void*       pSessionContext,
   uint32_t    nCommandID,
   uint32_t    nParamTypes,
   TEE_Param   pParams[4]);

/*------------------------------------------------------------------------------
      Trusted Core Framework (27) functions
------------------------------------------------------------------------------*/
/**
 * @brief Get property as string.
 *
 *    The TEE_GetPropertyAsString function performs a lookup in a property set to retrieve an individual property
 * and convert its value into a printable string. When the lookup succeeds, the implementation MUST convert
 * the property into a printable string and copy the result into the buffer described by valueBuffer and valueBufferLen.
 *
 * @param[in]     propsetOrEnumerator     One of the TEE_PROPSET_XXX pseudo-handles or a handle on a property enumerator
 * @param[in]     name                    A pointer to the zero-terminated string containing the name of the property to
 *                                        retrieve. Its content is case-sensitive and it must be encoded in UTF-8.
 *                                          o If hPropSet is a property enumerator handle, name is ignored and can be NULL.
 *                                          o Otherwise, name cannot be NULL
 * @param[out]    valueBuffer             Output buffer for the property value.
 * @param[inout]  valueBufferLen          Length of the output buffer.
 *
 * @return        TEE_SUCCESS: In case of success
 *                TEE_ERROR_ITEM_NOT_FOUND: If the property is not found or if name is not a valid UTF-8 encoding
 *                TEE_ERROR_SHORT_BUFFER: If the value buffer is not large enough to hold the whole property value
 */
extern TEE_Result TEE_GetPropertyAsString(
    TEE_PropSetHandle  propsetOrEnumerator,
    char*              name,        /* [instringopt] */
    char*              valueBuffer, /* [outstring] */
    size_t*            valueBufferLen
);
/**
 * @brief Get property as bool.
 *
 *    The TEE_GetPropertyAsBool function retrieves a single property in a property set and converts its value to a Boolean.
 * If a property cannot be viewed as a Boolean, this function MUST return TEE_ERROR_BAD_FORMAT. Otherwise, if this
 * function succeeds, then calling the function TEE_GetPropertyAsString on the same name and with a sufficiently large
 * output buffer MUST also succeed and return a string equal to true or false case-insensitive, depending on the value
 * of the Boolean.
 *
 * @param[in]     propsetOrEnumerator     One of the TEE_PROPSET_XXX pseudo-handles or a handle on a property enumerator
 * @param[in]     name                    A pointer to the zero-terminated string containing the name of the property to
 *                                        retrieve. Its content is case-sensitive and it must be encoded in UTF-8.
 *                                          o If hPropSet is a property enumerator handle, name is ignored and can be NULL.
 *                                          o Otherwise, name cannot be NULL
 * @param[out]    value                   A pointer to the variable that will contain the value of the property on success or
 *                                        false on error.
 *
 * @return        TEE_SUCCESS: In case of success
 *                TEE_ERROR_ITEM_NOT_FOUND: If the property is not found or if name is not a valid UTF-8 encoding
 *                TEE_ERROR_BAD_FORMAT: If the property value cannot be converted to a Boolean
 */
extern TEE_Result TEE_GetPropertyAsBool(
    TEE_PropSetHandle  propsetOrEnumerator,
    char*              name, /* [instringopt] */
    bool*              value /* [out] */
);
/**
 * @brief Get property as uint32_t.
 *
 *    The TEE_GetPropertyAsU32 function retrieves a single property in a property set and converts its value to a 32-bit
 * unsigned integer. If a property cannot be viewed as a 32-bit unsigned integer, this function MUST return
 * TEE_ERROR_BAD_FORMAT.
 *
 * @param[in]     propsetOrEnumerator     One of the TEE_PROPSET_XXX pseudo-handles or a handle on a property enumerator
 * @param[in]     name                    A pointer to the zero-terminated string containing the name of the property to
 *                                        retrieve. Its content is case-sensitive and it must be encoded in UTF-8.
 *                                          o If hPropSet is a property enumerator handle, name is ignored and can be NULL.
 *                                          o Otherwise, name cannot be NULL
 * @param[out]    value                   A pointer to the variable that will contain the value of the property on success or
 *                                        zero on error.
 *
 * @return        TEE_SUCCESS: In case of success
 *                TEE_ERROR_ITEM_NOT_FOUND: If the property is not found or if name is not a valid UTF-8 encoding
 *                TEE_ERROR_BAD_FORMAT: If the property value cannot be converted to a 32-bit integer.
 */
extern TEE_Result TEE_GetPropertyAsU32 (
    TEE_PropSetHandle propsetOrEnumerator,
    char*             name, /* [intstringopt] */
    uint32_t*         value /* [out] */
);
/**
 * @brief Get property as base64 encoded binary block.
 *
 *    The function TEE_GetPropertyAsBinaryBlock retrieves an individual property and converts its value into a binary block.
 * If a property cannot be viewed as a binary block, this function MUST return TEE_ERROR_BAD_FORMAT. Otherwise, if this function
 * succeeds, then calling the function TEE_GetPropertyAsString on the same name and with a sufficiently large output buffer
 * MUST also succeed and return a string that is consistent with a Base64 encoding of the binary block as defined in RFC 2045 [6],
 * section 6.8 but with the following tolerance:
 *   An Implementation is allowed not to encode the final padding = characters.
 *   An Implementation is allowed to insert characters that are not in the Base64 character set.
 *
 * @param[in]     propsetOrEnumerator     One of the TEE_PROPSET_XXX pseudo-handles or a handle on a property enumerator
 * @param[in]     name                    A pointer to the zero-terminated string containing the name of the property to
 *                                        retrieve. Its content is case-sensitive and it must be encoded in UTF-8.
 *                                          o If hPropSet is a property enumerator handle, name is ignored and can be NULL.
 *                                          o Otherwise, name cannot be NULL
 * @param[out]    valueBuffer             Output buffer for the property value.
 * @param[inout]  valueBufferLen          Length of the output buffer.
 *
 * @return        TEE_SUCCESS: In case of success
 *                TEE_ERROR_ITEM_NOT_FOUND: If the property is not found or if name is not a valid UTF-8 encoding
 *                TEE_ERROR_BAD_FORMAT: If the property cannot be retrieved as a binary block
 *                TEE_ERROR_SHORT_BUFFER: If the value buffer is not large enough to hold the whole property value
 */
extern TEE_Result TEE_GetPropertyAsBinaryBlock(
    TEE_PropSetHandle  propsetOrEnumerator,
    char*              name,        /* [instringopt] */
    void*              valueBuffer, /* [outstring] */
    size_t*            valueBufferLen
);
/**
 * @brief Get property as TEE_UUID.
 *
 *    The function TEE_GetPropertyAsUUID retrieves an individual property and converts its value into a UUID.
 * If a property cannot be viewed as a UUID, this function MUST return TEE_ERROR_BAD_FORMAT. Otherwise,
 * if this function succeeds, then calling the function TEE_GetPropertyAsString on the same name and with
 * a sufficiently large output buffer MUST also succeed and return a string that is consistent with the
 * concrete syntax of UUIDs defined in RFC 4122. Note that this string may mix character cases.
 *
 * @param[in]     propsetOrEnumerator     One of the TEE_PROPSET_XXX pseudo-handles or a handle on a property enumerator
 * @param[in]     name                    A pointer to the zero-terminated string containing the name of the property to
 *                                        retrieve. Its content is case-sensitive and it must be encoded in UTF-8.
 *                                          o If hPropSet is a property enumerator handle, name is ignored and can be NULL.
 *                                          o Otherwise, name cannot be NULL
 * @param[out]    value                   A pointer filled with the UUID. Must not be NULL.
 *
 * @return        TEE_SUCCESS: In case of success
 *                TEE_ERROR_ITEM_NOT_FOUND: If the property is not found or if name is not a valid UTF-8 encoding
 *                TEE_ERROR_BAD_FORMAT: If the property cannot be converted into a UUID
 */
extern TEE_Result TEE_GetPropertyAsUUID(
    TEE_PropSetHandle  propsetOrEnumerator,
    char*              name, /* [instringopt] */
    TEE_UUID*          value /* [out] */
);
/**
 * @brief Get property as TEE_Identity.
 *
 * The function TEE_GetPropertyAsIdentity retrieves an individual property and converts its value into a TEE_Identity.
 *
 * @param[in]     propsetOrEnumerator     One of the TEE_PROPSET_XXX pseudo-handles or a handle on a property enumerator
 * @param[in]     name                    A pointer to the zero-terminated string containing the name of the property to
 *                                        retrieve. Its content is case-sensitive and it must be encoded in UTF-8.
 *                                          o If hPropSet is a property enumerator handle, name is ignored and can be NULL.
 *                                          o Otherwise, name cannot be NULL
 * @param[out]    value                   A pointer filled with the identity. Must not be NULL.
 *
 * @return        TEE_SUCCESS: In case of success
 *                TEE_ERROR_ITEM_NOT_FOUND: If the property is not found or if name is not a valid UTF-8 encoding
 *                TEE_ERROR_BAD_FORMAT: If the property cannot be converted into an Identity
 */
extern TEE_Result TEE_GetPropertyAsIdentity(
    TEE_PropSetHandle  propsetOrEnumerator,
    char*              name, /* [instringopt] */
    TEE_Identity*      value /* [out] */
);
/**
 * @brief Allocate a Property Set enumerator.
 *
 *    The function TEE_AllocatePropertyEnumerator allocates a property enumerator object. Once a handle on a property enumerator
 * has been allocated, it can be used to enumerate properties in a property set using the function TEE_StartPropertyEnumerator.
 *
 * @param[in]     enumerator    A pointer filled with an opaque handle on the property enumerator on success and with TEE_HANDLE_NULL on error
 *
 * @return        TEE_SUCCESS: In case of success
 *                TEE_ERROR_OUT_OF_MEMORY: If there are not enough resources to allocate the property enumerator
 */
extern TEE_Result TEE_AllocatePropertyEnumerator(
    TEE_PropSetHandle* enumerator
);
/**
 * @brief Free a Property Set enumerator.
 *
 * The function TEE_FreePropertyEnumerator deallocates a property enumerator object.
 *
 * @param[in]       enumerator    A handle on the enumerator to free
 */
extern void TEE_FreePropertyEnumerator(
    TEE_PropSetHandle enumerator
);
/**
 * @brief Assign a Property Set to the Enumerator.
 *
 *    The function TEE_StartPropertyEnumerator starts to enumerate the properties in an enumerator.
 * Once an enumerator is attached to a property set:
 *   Properties can be retrieved using one of the TEE_GetPropertyAsXXX functions, passing the enumerator handle as the property set and NULL as the name.
 *   The function TEE_GetPropertyName can be used to retrieve the name of the current property in the enumerator.
 *   The function TEE_GetNextProperty can be used to advance the enumeration to the next property in the property set.
 *
 *  @param[in]      enumerator    A handle on the enumerator
 *  @param[in]      propSet       A pseudo-handle on the property set to enumerate. Must be one of the TEE_PROPSET_XXX pseudo-handles.
 */
extern void TEE_StartPropertyEnumerator(
    TEE_PropSetHandle enumerator,
    TEE_PropSetHandle propSet
);
/**
 * @brief Reset Property Set enumerator to the un-assigned state.
 *
 * The function TEE_ResetPropertyEnumerator resets a property enumerate to its state immediately after allocation.
 * If an enumeration is currently started, it is abandoned.
 *
 * @param[in]       enumerator    A handle on the enumerator to reset
 */
extern void TEE_ResetPropertyEnumerator(
    TEE_PropSetHandle enumerator
);
/**
 * @brief Get the name of the current property from the enumerator as a (char *).
 *
 * The function TEE_GetPropertyName gets the name of the current property in an enumerator.
 * The property name MUST be the valid UTF-8 encoding of a Unicode string containing no U+0000 code points.
 *
 * @param[in]       enumerator    A handle on the enumerator
 * @param[out]      nameBuffer    The buffer filled with the name
 * @param[inout]    nameBufferLen The buffer length that is updated to reflect the size property name.
 *
 * @return          TEE_SUCCESS: In case of success
 *                  TEE_ERROR_ITEM_NOT_FOUND: If there is no current property either because the enumerator
 *                  has not started or because it has reached the end of the property set
 *                  TEE_ERROR_SHORT_BUFFER: If the name buffer is not large enough to contain the property name
 */
extern TEE_Result TEE_GetPropertyName(
    TEE_PropSetHandle enumerator,
    void*             nameBuffer,   /* [outstring] */
    size_t*           nameBufferLen
);
/**
 * @brief Iterate to the next property in the enumerator.
 *
 * The function TEE_GetNextProperty advances the enumerator to the next property.
 *
 * @param[in]       enumerator      A handle on the enumerator
 *
 * @return          TEE_SUCCESS: In case of success
 *                  TEE_ERROR_ITEM_NOT_FOUND: If the enumerator has reached the end of the property set or if it has not started
 */
extern TEE_Result TEE_GetNextProperty(
    TEE_PropSetHandle enumerator);

/*------------------------------------------------------------------------------
      GP Internal Internal Client APIs
------------------------------------------------------------------------------*/

extern TEE_Result TEE_OpenTASession(
  TEE_UUID* destination,
  uint32_t cancellationRequestTimeout,
  uint32_t paramTypes,
  TEE_Param params[MAX_GP_PARAMS],
  TEE_TASessionHandle* session,
  uint32_t* returnOrigin);

extern void TEE_CloseTASession( TEE_TASessionHandle session);

extern TEE_Result TEE_InvokeTACommand(
  TEE_TASessionHandle session,
  uint32_t cancellationRequestTimeout,
  uint32_t commandID,
  uint32_t paramTypes,
  TEE_Param params[MAX_GP_PARAMS],
  uint32_t* returnOrigin);

/*------------------------------------------------------------------------------
      GP Cancellations
------------------------------------------------------------------------------*/
extern bool TEE_GetCancellationFlag( void);

extern bool TEE_UnmaskCancellation( void );

extern bool TEE_MaskCancellation( void );

/*------------------------------------------------------------------------------
      GP Memory Management
------------------------------------------------------------------------------*/
/**
 * @brief Checks memory access rights on buffer address.
 *
 * The TEE_CheckMemoryAccessRights function causes the Implementation to examine a buffer of memory
 * specified in the parameters buffer and size and to determine whether the current Trusted Application
 * instance has the access rights requested in the parameter accessFlags. If the characteristics of the buffer
 * are compatible with accessFlags, then the function returns TEE_SUCCESS. Otherwise, it returns
 * TEE_ERROR_ACCESS_DENIED. Note that the buffer SHOULD NOT be accessed by the function, but the
 * Implementation SHOULD check the access rights based on the address of the buffer and internal memory
 * management information.
 *
 * @return  Upon successful completion, TEE_SUCCESS, otherwise TEE_ERROR_ACCESS_DENIED
 *
*/

extern TEE_Result TEE_CheckMemoryAccessRights(
  uint32_t accessFlags,
  void* buffer,
  size_t size );

/**
 * @brief Set an application global variable.
 *
 * The TEE_SetInstanceData and TEE_GetInstanceData functions provide an alternative to writable global data
 * (writable variables with global scope and writable static variables with global or function scope). While
 * an Implementation MUST support C global variables, using these functions may be sometimes more efficient,
 * especially if only a single instance data variable is required.
 * These two functions can be used to register and access an instance variable. Typically this instance
 * variable can be used to hold a pointer to a Trusted Application-defined memory block containing any
 * writable data that needs instance global scope, or writable static data that needs instance function scope.
 * An equivalent session context variable for managing session global and static data exists for sessions
 * (see TA_OpenSessionEntryPoint, TA_InvokeCommandEntryPoint, and TA_CloseSessionEntryPoint in section 4.3).
 * This function sets the Trusted Application instance data pointer. The data pointer can then be retrieved
 * by the Trusted Application instance by calling the TEE_GetInstanceData function.
 *
 * @param[in] instanceData    A pointer to the global Trusted Application instance data. This pointer may be NULL.
 */
extern void TEE_SetInstanceData( void* instanceData ); // [ctx]

/**
 * @brief Get the application global variable.
 *
 * The TEE_GetInstanceData function retrieves the instance data pointer set by the Trusted Application
 * using the TEE_GetInstanceData function.
 *
 * @return The value returned is the previously set pointer to the Trusted Application instance data, or NULL if
 * no instance data pointer has yet been set.
 */
extern void* TEE_GetInstanceData( void ); // [ctx]

/**
 * @brief Allocate dynamic memory.
 *
 * The parameter hint is a hint to the allocator. In this version of the specification, only one hint
 * is defined. This parameter is nonetheless included so that the Trusted Applications may refer to
 * various pools of memory or request special characteristics for the allocated memory by using an
 * implementation-defined hint. Future versions of this specification may introduce additional standard hints.
 * If the space cannot be allocated, a NULL pointer is returned.
 *
 * Calls qsee_malloc().
 *
 * @param[in] size    The size of the buffer to be allocated.
 * @param[in] hint    A hint to the allocator. Currently defined values are as follows:
 *                      o The default value, 0, guarantees that the returned block of memory is filled with zeros.
 *                      o Values in the range [0x00000001, 0x7FFFFFFF] are reserved for future version of this specification.
 *                      o Values in the range [0x80000000, 0xFFFFFFFF] can be used for implementation-defined hints.
 *
 * @return  Upon successful completion, with size not equal to zero, the function returns a pointer to the allocated space.
 *          If the space cannot be allocated, a NULL pointer is returned.
 */
extern void* TEE_Malloc(
  size_t size,
  uint32_t hint
);

extern void* TEE_Realloc(
  void* buffer,
  uint32_t newSize );

/**
 * @brief Free dynamically allocated memory.
 *
 * The TEE_Free function causes the space pointed to by buffer to be deallocated; that is, made available for further allocation.
 * If buffer is a NULL pointer, TEE_Free does nothing.
 *
 * @param[in] buffer    Upon successful completion, with size not equal to zero, the function returns a pointer
 *                      to the allocated space. If the space cannot be allocated, a NULL pointer is returned.
 */
extern void TEE_Free(  void* buffer ); // [in]

/**
 * @brief Copy bytes from source to destination.
 *
 * The TEE_MemMove function copies size bytes from the object pointed to by src into the object pointed to by dest.
 * Copying takes place as if the size bytes from the object pointed to by src are first copied into a temporary
 * array of size bytes that does not overlap the objects pointed to by dest and src, and then the size bytes
 * from the temporary array are copied into the object pointed to by dest.
 *
 * @param[in] dest    A pointer to the destination buffer
 * @param[in] src     A pointer to the source buffer
 * @param[in] size    The number of bytes to be copied
 */
extern void TEE_MemMove(
    void* dest,
    void* src,
    uint32_t size );

/**
 * @brief Compare two blocks of memory.
 *
 * The TEE_MemCompare function compares the first size bytes of the object pointed to by buffer1 to the first size
 * bytes of the object pointed to by buffer2.
 *
 * @param[in] buffer1     A pointer to the first buffer
 * @param[in] buffer2     A pointer to the second buffer
 * @param[in] size        The number of bytes to be compared
 *
 * @return The sign of a non-zero return value is determined by the sign of the difference between the values of
 *         the first pair of bytes (both interpreted as type uint8_t) that differ in the objects being compared.
 *           If the first byte that differs is higher in buffer1, then return an integer greater than zero.
 *           If the first size bytes of the two buffers are identical, then return zero.
 *           If the first byte that differs is higher in buffer2, then return an integer lower than zero.
 */
extern int32_t TEE_MemCompare(
    void* buffer1,
    void* buffer2,
    uint32_t size );

/**
 * @brief Fill memory with byte x.
 *
 * The TEE_MemFill function writes the byte x (converted to a uint8_t) into the first size bytes of the
 * object pointed to by buffer.
 *
 * @param[in] buffer    A pointer to the destination buffer
 * @param[in] x         The value to be set
 * @param[in] size      The number of bytes to be set
 */
extern void TEE_MemFill(
    void* buffer,
    uint32_t x,
    uint32_t size );

/*------------------------------------------------------------------------------
      GP Time functions  (5)
------------------------------------------------------------------------------*/
extern void TEE_GetSystemTime(
   TEE_Time* time);     //[out]

extern TEE_Result TEE_Wait(
   uint32_t timeout);

extern TEE_Result TEE_GetTAPersistentTime(
   TEE_Time* time);     // [out]

extern TEE_Result TEE_SetTAPersistentTime(
   TEE_Time* time);     //[in]

extern void TEE_GetREETime(
   TEE_Time* time);     //[out]


/*------------------------------------------------------------------------------
      GP Arithmetic functions (32)
------------------------------------------------------------------------------*/
extern size_t TEE_BigIntFMMContextSizeInU32(
   size_t modulusSizeInBits);

extern size_t TEE_BigIntFMMSizeInU32(
   size_t modulusSizeInBits);

/**
 * Initialization Functions
 */

/**
 * @brief The TEE_BigIntInit function initializes bigInt
 *        and sets its represented value to zero. This function
 *        assumes that bigInt points to a memory area of len uint32_t
 *        This can be done for example with the following memory allocation
 *        TEE_BigInt *a;
 *        size_t len;
 *        len = TEE_BigIntSizeInU32(bitSize);
 *        a =(TEE_BigInt *)TEE_Malloc(len * sizeof(TEE_BigInt));
 *        ret = TEE_BigIntInit(a,len);
 *
 * @param[out]  bigInt   A pointer to TEE_BigInt to be initialized,
 *                       memory has to be allocated by caller
 * @param[in]   len      The size in uint32_t of the memory pointed to by
 *                       bigInt
 *
 * @return
 *
 */
extern void TEE_BigIntInit(
  TEE_BigInt *bigInt,
    size_t len);

/**
 * @brief The TEE_BigIntInitFMMContext function calculates the
 *        necessary prerequisites for the fast modular
 *        multiplication and stores them in a context. This
 *        function assumes that context points to a memory area
 *        of len uint32_t This can be done for example with the
 *        following memory allocation:
 *
 *       TEE_BigIntFMMContext* ctx;
 *       size_t len = TEE_BigIntFMMContextSizeInU32(bitsize);
 *       ctx=(TEE_BigIntFMMContext)TEE_Malloc
 *       (len * sizeof(TEE_BigIntFFMContext));
 *       //Code for initializing modulus
 *       TEE_BigIntInitFMMContext (ctx, len, modulus);
 *
 *       Eventhough a fast multiplication might be
 *       mathematically defined for any modulus,
 *       normally there are restrictions in order
 *       for it to be fast on a computer. This specification
 *       mandates that all implementations
 *       MUST work for all odd moduli larger than 2 and less
 *       than 2 to the power of the implementation defined
 *       property gpd.tee.arith.maxBigIntSize.
 *
 * @param[out]  context  A pointer to the TEE_BigIntFMMContext
 *                       to be initialized, memory has to be
 *                       allocated by caller
 * @param[in]   len      The size in uint32_t of the memory pointed to by
 *                       context
 * @param[in]   modulus  The modulus, an odd integer larger than
 *                       2 and less than 2 to the power of
 *                       gpd.tee.arith.maxBigIntSize
 *
 * @return
 *
 */
extern void TEE_BigIntInitFMMContext(
  TEE_BigIntFMMContext *context, // [out]
  size_t len,
  TEE_BigInt *modulus);          // [in]

/**
 * @brief The TEE_BigIntInitFMM function initializes bigIntFMM
 * and sets its represented value to zero. This function assumes
 * that bigIntFMM points to a memory area of len uint32_t This
 * can be done for example with the following memory allocation:
 *
 * TEE_BigIntFMM *a;
 * size_t len;
 * len = TEE_BigIntFMMSizeInU32(modulusSizeinBits);
 * a = * (TEE_BigIntFMM *)TEE_Malloc
 * (len * sizeof(TEE_BigIntFMM));
 * TEE_InitFMMInt(a, len);
 *
 * @param[out]  bigIntFMM A pointer to the TEE_BigIntFMM
 *                        to be initialized, memory has to be
 *                        allocated by caller
 * @param[in]   len       The size in uint32_t of the memory
 *                        pointed to by bigIntFMM
 *
 * @return
 *
 */
extern void TEE_BigIntInitFMM(
  TEE_BigIntFMM *bigIntFMM, //[in]
  size_t len);

/**
 * Converter Functions
 */

/**
 * @brief The TEE_BigIntConvertFromOctetString function converts
 * a bufferLen byte octet string buffer into a TEE_BigInt
 * format. The octet string is in most significant byte first
 * representation. The input parameter sign will set the sign of
 * dest. It will be set to negative if sign <0 and to positive
 * if sign >=0
 *
 * @param[out]  dest      Pointer to a TEE_BigInt to hold the
 *                        result, memory has to be allocated by
 *                        caller
 * @param[in]   buffer    Pointerto the buffer containing the
 *                        octet string representation of the
 *                        integer
 * @param[in]   bufferLen The length of buffer in bytes
 * @param[in]   sign      The sign of dest is set to the sign of
 *                        sign
 *
 * @return
 * TEE_SUCCESS        : In case of success
 * TEE_ERROR_OVERFLOW : If memory allocation for the  dest is
 *                      too small
 *
 */
extern TEE_Result TEE_BigIntConvertFromOctetString(
  TEE_BigInt *dest,
  uint8_t *buffer, size_t bufferLen,
  int32_t sign);

/**
 * @brief The TEE_BigIntConvertToOctetString function converts
 *        the absolute value of an integer in TEE_BigInt format
 *        into an octet string. The octet string is written in a
 *        most significant byte first representation.
 *
 * @param[out]  buffer    Output buffer where converted octet
 *                        string representation of the integer
 *                        is written
 * @param[in]   bufferLen The length of buffer in bytes
 * @param[in]   bigInt    Pointer to the integer that will be
 *                        converted to an octet string
 *
 * @return
 * TEE_SUCCESS            : In case of success
 * TEE_ERROR_SHORT_BUFFER : If the output buffer is too small to
 *                          contain the octet string
 *
 */
extern TEE_Result TEE_BigIntConvertToOctetString(
  void *buffer, size_t *bufferLen,
  TEE_BigInt *bigInt);

/**
 * @brief The TEE_BigIntConvertFromS32 function sets *dest to
 *        the value shortVal
 *
 * @param[out]  dest      Pointer to a TEE_BigInt to store the
 *                        result, The result must have memory allocation
 *                        for holding a 32 bit signed value
 * @param[in]   shortVal  Input Value
 *
 * @return
 *
 */
extern void TEE_BigIntConvertFromS32(
  TEE_BigInt *dest,
  int32_t shortVal);

/**
 * @brief The TEE_BigIntConvertToS32 function sets *dest to the
 * value of src ,including the sign of src. If src does not fit
 * within an int32_t, the value of *dest is undefined
 *
 * @param[out]  dest      Pointer to an int32_t to store the
 *                        result
 * @param[in]   src       Pointer to an input value
 *
 * @return
 * TEE_SUCCESS            : In case of success
 * TEE_ERROR_OVERFLOW     : If the source does not fit within an
 *                          int32_t
 *
 */
extern TEE_Result TEE_BigIntConvertToS32(
  int32_t *dest,
  TEE_BigInt *src);

/**
 * Logical Operations
 */

/**
 * @brief The TEE_BigIntCmp function checks whether op1 > op2 ,
 * op1 == op2 , or op1 < op2
 *
 * @param[in]  op1      Pointer to first operand
 * @param[in]  op2      Pointer to second operand
 *
 * @return
 *     This function returns a negative number if op1 < op2,
 *                             0 if op1 == op2,
 *                             positive number if op1 > op2
 *
 */
extern int32_t TEE_BigIntCmp(
  TEE_BigInt *op1,
  TEE_BigInt *op2);

/**
 * @brief The TEE_BigIntCmpS32 function checks whether
 * op > shortVal , op == shortVal , or op < shortVal
 *
 * @param[in]  op       Pointer to first operand
 * @param[in]  shortVal Pointer to second operand
 *
 * @return
 *     This function returns a negative number if op < shortVal,
 *                             0 if op == shortVal,
 *                             positive number if op > shortVal
 *
 */
extern int32_t TEE_BigIntCmpS32(
  TEE_BigInt *op,
  int32_t shortVal);

/**
 * @brief The TEE_BigIntShiftRight function computes
 * |dest| = | op | >> bits and dest will have the same sign as
 * op. If bits is greater than the bit length of op then the
 * result is zero. It is allowed that dest and op point to the
 * same memory region.
 *
 * @param[out] dest     Pointer to TEE_BigInt to hold the
 *                      shifted result, memory allocated by
 *                      caller
 * @param[in]  op       Pointer to operand to be shifted
 * @param[in]  bits     Number of bits to shift
 *
 * @return
 *
 */
extern void TEE_BigIntShiftRight(
  TEE_BigInt *dest,
  TEE_BigInt *op,
    size_t bits);


/**
 *  @brief The TEE_BigIntGetBit function returns the bitIndexth
 *   bit of thenatural binary representation of |src|. A true
 *   return value indicates a 1 and a false return value
 *   indicates a 0 in the bitIndexth position. If bitIndex is
 *   larger than the number of bits in op , the return value is
 *   false, thus indicating a 0
 *
 * @param[in]  src      Pointer to the integer
 * @param[in]  bitIndex The offset of the bit to be read,
 *                      starting at offset 0 for the least
 *                      significant bit
 *
 * @return
 * The Boolean value of the bitIndexth bit in | src |
 * True represents a 1 and false represents a 0.
 *
 */
extern bool TEE_BigIntGetBit(
  TEE_BigInt *src,
  uint32_t bitIndex);


/**
 *  @brief The TEE_BigIntGetBitCount function returns the number
 *  of bits in the natural binary representation of |src|; that
 *  is, the magnitude of src
 *
 * @param[in]  src      Pointer to the integer
 *
 * @return
 * The number of bits in the natural binary representation of
 * |src |. If src equals zero, it will return 0
 *
 */
extern uint32_t TEE_BigIntGetBitCount(
  TEE_BigInt *src);


/**
 * Basic Arithmetic Operations
 */

/**
 *  @brief The TEE_BigIntAdd function computes dest = op1 + op2
 *  It is allowed that all or some of dest, op1, and op2 point
 *  to the same memory region.
 *  Result Size, Depending on the sign of op1 and op2, the
 *  result may be larger or smaller than op1 and op2. For the
 *  worst case, dest must have memory allocation for holding
 *  max(magnitude( op1), magnitude( op2 ))+ 1 bits
 *
 * @param[out]  dest      Pointer to TEE_BigInt to store the
 *                        result op1 + op2, pointer allocated by
 *                        caller
 * @param[in]   op1       Pointer to the first operand
 * @param[in]   op2       Pointer to the second operand
 *
 * @return
 *
 */
extern void TEE_BigIntAdd(
  TEE_BigInt *dest,
  TEE_BigInt *op1,
  TEE_BigInt *op2);

/**
 *  @brief The TEE_BigIntSub function computes dest = op1  op2.
 *  It is allowed that all or some of dest, op1, and op2 point
 *  to the same memory region.
 *  Result Size, Depending on the sign of op1 and op2, the
 *  result may be larger or smaller than op1 and op2. For the
 *  worst case, dest must have memory allocation for holding
 *  max(magnitude( op1), magnitude( op2 ))+ 1 bits
 *
 * @param[out]  dest      Pointer to TEE_BigInt to store the
 *                        result op1 - op2
 * @param[in]   op1       Pointer to the first operand
 * @param[in]   op2       Pointer to the second operand
 *
 * @return
 *
 */
extern void TEE_BigIntSub(
  TEE_BigInt *dest,
  TEE_BigInt *op1,
  TEE_BigInt *op2);

/**
 *  @brief The TEE_BigIntNeg function negates an operand:
 *  dest = -op. It is allowed that dest and op point to the
 *  same memory region
 *  ResultSize, The result must have memory allocation for
 *  magnitude(op) bits.
 *
 * @param[out]  dest      Pointer to TEE_BigInt to store the
 *                        result -op
 * @param[in]   op        Pointer to the operand to be negated
 *
 * @return
 *
 */
extern void TEE_BigIntNeg(
  TEE_BigInt *dest,
  TEE_BigInt *op);

/**
 *  @brief The TEE_BigIntMul function computes
 *  dest= op1 * op2. It is allowed that all or some of dest,
 *  op1 , and op2 point to the same memory region
 *  ResultSize, The result must have memory allocation for
 *  (magnitude(op1)+magnitude(op2)) bits.
 *
 * @param[out]  dest      Pointer to TEE_BigInt to store the
 *                        result op1 * op2
 * @param[in]   op1       Pointer to the first operand
 * @param[in]   op2       Pointer to the second operand
 *
 * @return
 *
 */
extern void TEE_BigIntMul(
  TEE_BigInt *dest,
  TEE_BigInt *op1,
  TEE_BigInt *op2);

/**
 *  @brief The TEE_BigIntSquare function computes
 *  dest = op * op. . It is allowed that dest and op point to
 *  the same memory region
 *  ResultSize, The result must have memory allocation for
 *  2*magnitude(op) bits
 *
 * @param[out]  dest      Pointer to TEE_BigInt to store the
 *                        result op * op
 * @param[in]   op        Pointer to the operand to be squared
 *
 * @return
 *
 */
extern void TEE_BigIntSquare(
  TEE_BigInt *dest,
  TEE_BigInt *op);


/**
 *  @brief The TEE_BigIntDiv function computes dest_r and dest_q
 *  such that op1 = dest_q * op2 + dest_r. It will round dest_q
 *  towards zero and dest_r will have the same sign as op1
 *
 *  If op2 equals zero this function will execute a divide by
 *  zero operation using the normal / operator for integers.
 *  This function has the limitation that the memory pointed to
 *  by dest_q and dest_r MUST NOT overlap. However it is
 *  possible that dest_q==op1 , dest_q==op2, dest_r==op1,
 *  dest_r==op2, when dest_q and dest_r do not overlap. It is
 *  allowed to pass a NULL pointer for either dest_q or dest_r
 *  in which case the implementation MAY take advantage of the
 *  fact that it is only required to calculate either dest_q or
 *  dest_r
 *  Result Sizes, The quotient, dest_q , must have memory
 *  allocation for
 *  0 bytes if |op1 | <= |op2| and
 *  magnitude(op1) magnitude(op2) if|op1| >| op2 |
 *  The remainder dest_r must have memory allocation to hold
 *  magnitude(op2) bits.
 *
 * @param[out]  dest_q      Pointer to a  TEE_BigInt  to store
 *                          the quotient. dest_q can be NULL
 * @param[out]  dest_r      Pointer to a  TEE_BigInt  to store
 *                          the remainder. dest_r can be NULL
 * @param[in]   op1         Pointer to the first operand, the
 *                          dividend
 * @param[in]   op2         Pointer to the second operand, the
 *                          divisor
 *
 * @return
 *
 */
extern void TEE_BigIntDiv(
  TEE_BigInt *dest_q,
  TEE_BigInt *dest_r,
  TEE_BigInt *op1,
  TEE_BigInt *op2);

/**
 * Modular Arithmetic Operations
 */

/**
 *  @brief The TEE_BigIntMod function computes dest = op(modn)
 *  such that 0 <= dest < n.
 *  It is allowed that dest and op point to the same memory
 *  region but n MUST point to a unique memory region. For
 *  negative op it follows the normal convention that
 *  -1 = (n-1)mod n
 *  Result Sizes, The result dest must have memory allocation
 *  for magnitude(n) bits
 *  Panic Reasons, n<2
 *
 * @param[out]  dest        Pointer to TEE_BigInt to hold the
 *                          result op * (modn). The result dest
 *                          will be in the interval [0, n - 1]
 * @param[in]   op          Pointer to the operand, to be
 *                          reduced mod n
 * @param[in]   n           Pointer to the modulus. Modulus must be larger than 1
 *
 * @return
 *
 */
extern void TEE_BigIntMod(
  TEE_BigInt *dest,
  TEE_BigInt *op,
  TEE_BigInt *n);

/**
 *  @brief The TEE_BigIntAddMod function computes
 *  dest=op1+op2(modn). It is allowed that all or some of dest,
 *  op1 , and op2 point to the same memory region but n MUST
 *  point to a unique memory region.
 *  Result Sizes, The result dest must have memory allocation
 *  for magnitude(n) bits
 *  Panic Reasons, n<2
 *
 * @param[out]  dest        Pointer to TEE_BigInt to hold the
 *                          result op1 + op2 (modn).
 * @param[in]   op1         Pointer to the first operand, must
 *                          be in the interval [0, n - 1]
 * @param[in]   op2         Pointer to the second operand, must
 *                          be in the interval [0, n - 1]
 * @param[in]   n           Pointer to the modulus. Modulus must be larger than 1
 *
 * @return
 *
 */
extern void TEE_BigIntAddMod(
  TEE_BigInt *dest,
  TEE_BigInt *op1,
  TEE_BigInt *op2,
  TEE_BigInt *n);

/**
 *  @brief The TEE_BigIntSubMod function computes
 *  dest = op1 - op2 (modn)
 *  It is allowed that all or some of dest , op1, and op2
 *  points to the same memory region but n MUST point to a
 *  unique memory region.
 *  Result Sizes, The result dest must have memory allocation
 *  for magnitude(n) bits
 *  Panic Reasons, n<2
 *
 * @param[out]  dest        Pointer to TEE_BigInt to hold the
 *                          result op1 - op2 (modn).
 * @param[in]   op1         Pointer to the first operand, must
 *                          be in the interval [0, n - 1]
 * @param[in]   op2         Pointer to the second operand, must
 *                          be in the interval [0, n - 1]
 * @param[in]   n           Pointer to the modulus. Modulus must be larger than 1
 *
 * @return
 *
 */
extern void TEE_BigIntSubMod(
  TEE_BigInt *dest,
  TEE_BigInt *op1,
  TEE_BigInt *op2,
  TEE_BigInt *n);

/**
 *  @brief The TEE_BigIntMulMod function computes
 *  dest=op1*op2(modn). It is allowed that all or some of dest,
 *  op1 , and op2 point to the same memory region but n  MUST
 *  point to a unique memory region
 *  Result Sizes, The result dest must have memory allocation
 *  for magnitude(n) bits
 *  Panic Reasons, n<2
 *
 * @param[out]  dest        Pointer to TEE_BigInt to hold the
 *                          result op1 * op2 (modn).
 * @param[in]   op1         Pointer to the first operand, must
 *                          be in the interval [0, n - 1]
 * @param[in]   op2         Pointer to the second operand, must
 *                          be in the interval [0, n - 1]
 * @param[in]   n           Pointer to the modulus. Modulus must be larger than 1
 *
 * @return
 *
 */
extern void TEE_BigIntMulMod(
  TEE_BigInt *dest,
  TEE_BigInt *op1,
  TEE_BigInt *op2,
  TEE_BigInt *n);

/**
 *  @brief The TEE_BigIntSquareMod function computes
 *  dest = op * op (modn). It is allowed that dest and op1 point
 *  to the same memory region but n MUST point to a unique
 *  memory region
 *  Result Sizes, The result dest must have memory allocation
 *  for magnitude(n) bits
 *  Panic Reasons, n<2
 *
 * @param[out]  dest        Pointer to TEE_BigInt to hold the
 *                          result op * op (modn).
 * @param[in]   op          Pointer to the operand, must
 *                          be in the interval [0, n - 1]
 * @param[in]   n           Pointer to the modulus. Modulus must be larger than 1
 *
 * @return
 *
 */
extern void TEE_BigIntSquareMod(
  TEE_BigInt *dest,
  TEE_BigInt *op,
  TEE_BigInt *n);

/**
 *  @brief The TEE_BigIntInvMod function computes dest such that
 *         dest * op = 1 (modn). It is allowed that dest and op
 *  point to the same memory region. This function assumes that
 *  gcd(op,n) is equal to 1, which can be checked by using the
 *  function in section 8.10.1. If gcd(op,n) is greater than 1
 *  then the result is unreliable
 *  Result Sizes, The result dest must have memory allocation
 *  for magnitude(n) bits
 *  Panic Reasons, n<2, op = 0
 *
 * @param[out]  dest        Pointer to TEE_BigInt to hold the
 *                          result op^-1 (modn).
 * @param[in]   op          Pointer to the operand, must
 *                          be in the interval [1, n - 1]
 * @param[in]   n           Pointer to the modulus. Modulus must be larger than 1
 *
 * @return
 *
 */
extern void TEE_BigIntInvMod(
  TEE_BigInt *dest,
    TEE_BigInt *op,
    TEE_BigInt *n);

/**
 * Other Arithmetic Operations
 */

/**
 *  @brief The TEE_BigIntRelativePrime function
 *  determines whether gcd(op1, op2)==1.
 *  It is allowed that op1 and op2 point to the same memory
 *  region
 *
 * @param[in]   op1          Pointer to the first operand
 * @param[in]   op2          Pointer to the second operand
 *
 * @return
 * true if gcd(op1,op2) == 1
 * false otherwise
 *
 */
extern bool TEE_BigIntRelativePrime(
  TEE_BigInt *op1,
  TEE_BigInt *op2);

/**
 *  @brief The TEE_BigIntComputeExtendedGcd function computes
 *  the greatest common divisor of the input parameters op1 and
 *  op2. Furthermore it computes the coefficients u and v such
 *  that u*op1+v*op2 == gcd It is allowed that op1 and op2 point
 *  to the same memory region. It is allowed that u , v , or
 *  both can be NULL. If both of them are NULL then the function
 *  only computes the gcd of op1 and op2.
 *  ResultSizes, The gcd result must be able to hold
 *  max(magnitude(op1),magnitude( op2 )) bits
 *  The absolute value of u is in the range [0, |op2/gcd|-1]
 *  The absolute value of v is in the range [0, |op2/gcd|-1]
 *
 * @param[out]   gcd          Pointer to TEE_BigInt to hold the
 *                            greatest common divisor of op1 and
 *                            op2
 * @param[out]   u            Pointer to TEE_BigInt to hold the
 *                            first coefficient
 * @param[out]   v            Pointer to TEE_BigInt to hold the
 *                            second coefficient
 * @param[in]    op1          Pointer to the first operand
 * @param[in]    op2          Pointer to the second operand
 *
 * @return
 *
 */
extern void TEE_BigIntComputeExtendedGcd(
  TEE_BigInt *gcd,
  TEE_BigInt *u,
  TEE_BigInt *v,
  TEE_BigInt *op1,
  TEE_BigInt *op2);

/**
 *  @brief The TEE_BigIntComputeExtendedGcd function computes
 *  the greatest common divisor of the input parameters op1 and
 *  op2. Furthermore it computes the coefficients u and v such
 *  that u*op1+v*op2 == gcd It is allowed that op1 and op2 point
 *  to the same memory region. It is allowed that u , v , or
 *  both can be NULL. If both of them are NULL then the function
 *  only computes the gcd of op1 and op2.
 *  ResultSizes, The gcd result must be able to hold
 *  max(magnitude(op1),magnitude( op2 )) bits
 *  The absolute value of u is in the range [0, |op2/gcd|-1]
 *  The absolute value of v is in the range [0, |op2/gcd|-1]
 *
 * @param[in] op              Candidate number that is tested
 *                            for primality
 * @param[in] confidenceLevel The desired confidence level for a
 *                            non conclusive test. This
 *                            parameter (usually) maps to the
 *                            number of iterations and thus to
 *                            the running time of the test.
 *                            Values smaller than 80 will be
 *                            treated as 80
 *
 * @return
 *  0 : If op is a composite number
 *  1 : If op is a prime number
 *  -1: If the test is nonconclusive but the probability that op
 *  is composite is less than 2^(-confidenceLevel)
 *
 */
extern int32_t TEE_BigIntIsProbablePrime(
  TEE_BigInt *op,
  uint32_t confidenceLevel);

/**
 * Fast Modular Multiplication Operations
 */

/**
 *  @brief The TEE_BigIntConvertToFMM function converts src in
 *  to a representation suitable for doing fast modular
 *  multiplication. If the operation is successful , the result
 *  will be written in implementation specific format into
 *  the buffer dest , which must have been allocated by the TA
 *  and initialized using TEE_BigIntInitFMM
 *
 * @param[out]   dest         Pointer to an initialized
 *                            TEE_BigIntFMM memory area
 * @param[in]    src          Pointer to TEE_BigInt to convert
 * @param[in]    n            Pointer to modulus
 * @param[in]    context      Pointer to the context previously
 *                            initialized using
 *                            TEE_BigIntInitFMMContext
 *
 * @return
 *
 */
extern void TEE_BigIntConvertToFMM(
  TEE_BigIntFMM *dest,
  TEE_BigInt *src,
  TEE_BigInt *n,
  TEE_BigIntFMMContext *context);

/**
 *  @brief The TEE_BigIntConvertFromFMM function converts src in
 *  the fast modular multiplication representation back to a
 *  TEE_BigIntrepresentation.
 *
 * @param[out]   dest         Pointer to an initialized
 *                            TEE_BigInt memory area to hold the
 *                            converted result
 * @param[in]    src          Pointer to a TEE_BigIntFMM holding
 *                            the value in the fast modular
 *                            multiplication representation
 * @param[in]    n            Pointer to modulus
 * @param[in]    context      Pointer to the context previously
 *                            initialized using
 *                            TEE_BigIntInitFMMContext
 *
 * @return
 *
 */
extern void TEE_BigIntConvertFromFMM(
  TEE_BigInt *dest,
  TEE_BigIntFMM *src,
  TEE_BigInt *n,
  TEE_BigIntFMMContext *context);

/**
 *  @brief The TEE_BigIntComputeFMM function calculates
 *  dest = op 1 * op2 in the fast modular multiplication
 *  representation. The pointers dest, op1 and op2 MUST all
 *  point to a TEE_BigIntFMM which has been previously
 *  initialized with the same modulus and context as used in
 *  this function call; otherwise the result is undefined. It is
 *  allowed that all or some of dest, op1, and op2 point to the
 *  same memory region.
 *
 * @param[out]   dest         Pointer to TEE_BigIntFMM to hold
 *                            the result op1 * op2 in the fast
 *                            modular multiplication
 *                            representation
 * @param[in]    op1          Pointer to the first operand
 * @param[in]    op2          Pointer to the second operand
 * @param[in]    n            Pointer to modulus
 * @param[in]    context      Pointer to the context previously
 *                            initialized using
 *                            TEE_BigIntInitFMMContext
 *
 * @return
 *
 */
extern void TEE_BigIntComputeFMM(
  TEE_BigIntFMM *dest,
  TEE_BigIntFMM *op1,
  TEE_BigIntFMM *op2,
  TEE_BigInt *n,
  TEE_BigIntFMMContext *context);

/******************************************************************************
Data Storage functions (31)
******************************************************************************/
/**
 * Generic Objects
 */

/**
 * This function replaces the TEE_GetObjectInfo function, whose use is deprecated.
 *
 * @brief Gets info about transient or persistent object.
 *
 * The TEE_GetObjectInfo1 function returns the characteristics of an object.
 * It fills in the following fields in the structure TEE_ObjectInfo (section 5.3.2):
 *    objectType: The parameter objectType passed when the object was created
 *    keySize: The current size in bits of the object as determined by its attributes.
 *     This will always be less than or equal to maxKeySize. Set to 0 for uninitialized
 *     and data only objects.
 *    maxKeySize: The maximum keySize which this object can represent.
 *     o For a persistent object, set to keySize
 *     o For a transient object, set to the parameter maxKeySize passed to
 *       TEE_AllocateTransientObject
 *    objectUsage: A bit vector of the TEE_USAGE_XXX bits defined in Table 5-4.
 *    dataSize
 *     o For a persistent object, set to the current size of the data associated with the object
 *     o For a transient object, always set to 0
 *    dataPosition
 *     o For a persistent object, set to the current position in the data for this handle.
 *       Data positions for different handles on the same object may differ.
 *     o For a transient object, set to 0
 *    handleFlags: A bit vector containing one or more of the following flags:
 *     o TEE_HANDLE_FLAG_PERSISTENT: Set for a persistent object
 *     o TEE_HANDLE_FLAG_INITIALIZED
 *    For a persistent object, always set
 *    For a transient object, initially cleared, then set when the object becomes initialized
 *     o TEE_DATA_FLAG_XXX: Only for persistent objects, the flags used to open or create the object
 *
 * @param[in]   object      Handle on an object
 * @param[out]  objectInfo  A pointer to a TEE_ObjectInfo struct who's data members we'll be populated by the function
 *
 * @return   TEE_SUCCESS                Object info was copied form object.
 *           TEE_ERROR_BAD_PARAMETERS   Parameters were invalid to complete the operation
 *           TEE_ERROR_CORRUPT_OBJECT   If the persistent object is corrupt. The object handle is closed.
 *           TEE_ERROR_STORAGE_NOT_AVAILABLE   If the persistent object is stored in a storage area which is
 *                                             currently inaccessible.
 */
extern TEE_Result TEE_GetObjectInfo1(
    TEE_ObjectHandle object,
  TEE_ObjectInfo* objectInfo /*[out]*/);

/**
 * Use of this function is DEPRECATED - new code SHOULD use the TEE_GetObjectInfo1 function instead.
 *
 * @brief Gets info about transient or persistent object.
 *
 * The TEE_GetObjectInfo function returns the characteristics of an object.
 * It fills in the following fields in the structure TEE_ObjectInfo:
 *   objectType: The parameter objectType passed when the object was created
 *   objectSize: Set to 0 for an uninitialized object
 *   maxObjectSize
 *    o For a persistent object, set to objectSize
 *    o For a transient object, set to the parameter maxObjectSize passed to TEE_AllocateTransientObject
 *   objectUsage: A bit vector of the TEE_USAGE_XXX bits defined in Table 5-3. Initially set to 0xFFFFFFFF.
 *    Can be narrowed by calling TEE_SetRestrictUsage.
 *   dataSize
 *    o For a persistent object, set to the current size of the data associated with the object
 *    o For a transient object, always set to 0
 *   dataPosition
 *    o For a persistent object, set to the current position in the data for this handle.
 *      Data positions for different handles on the same object may differ.
 *    o For a transient object, set to 0
 *   handleFlags: A bit vector containing one or more of the following flags:
 *    o TEE_HANDLE_FLAG_PERSISTENT: Set for a persistent object
 *    o TEE_HANDLE_FLAG_INITIALIZED
 *   For a persistent object, always set
 *   For a transient object, initially cleared, then set when the object becomes initialized
 *    o TEE_DATA_FLAG_XXX: Only for persistent objects, the flags used to open or create the object
 *
 * @param[in]   object      Handle on an object
 * @param[out]  objectInfo  A pointer to a TEE_ObjectInfo struct who's data members we'll be populated by the function
 *
 * @return   TEE_SUCCESS                Object info was copied form object.
 *           TEE_ERROR_BAD_PARAMETERS   Parameters were invalid to complete the operation
 */
extern void TEE_GetObjectInfo(
    TEE_ObjectHandle object,
  TEE_ObjectInfo* objectInfo /*[out]*/);

/**
 * @brief This function replaces the TEE_RestrictObjectUsage function, whose use is deprecated.
 *
 * The TEE_RestrictObjectUsage1 function restricts the object usage flags of an object handle
 * to contain at most the flags passed in the objectUsage parameter.
 * For each bit in the parameter objectUsage:
 *   If the bit is set to 1, the corresponding usage flag in the object is left unchanged.
 *   If the bit is set to 0, the corresponding usage flag in the object is cleared.
 * For example, if the usage flags of the object are set to TEE_USAGE_ENCRYPT | TEE_USAGE_DECRYPT and
 * if objectUsage is set to TEE_USAGE_ENCRYPT | TEE_USAGE_EXTRACTABLE, then the only remaining usage
 * flag in the object after calling the function TEE_RestrictObjectUsage is TEE_USAGE_ENCRYPT.
 * Note that an object usage flag can only be cleared. Once it is cleared, it cannot be set to 1 again
 * until the whole object is reset using the function TEE_ResetObject. A transient objects object usage
 * flags are reset to 1 using the TEE_ResetTransientObject function. For a persistent object,
 * setting the object usage MUST be an atomic operation.
 *
 * @param[in] object        Handle on an object
 * @param[in] objectUsage   New object usage, an OR combination of one or more of the TEE_USAGE_XXX
 *                          constants defined in Table 5-3 in GP spec
 *
 * @return   TEE_SUCCESS                      Object info was copied form object.
 *           TEE_ERROR_CORRUPT_OBJECT         If the persistent object is corrupt. The object handle is
 *                                            closed.
 *           TEE_ERROR_STORAGE_NOT_AVAILABLE  If the persistent object is stored in a storage area which
 *                                            is currently inaccessible
 */
extern TEE_Result TEE_RestrictObjectUsage1(
    TEE_ObjectHandle object,
    uint32_t objectUsage);

/**
 * Use of this function is DEPRECATED - new code SHOULD use the TEE_RestrictObjectUsage1 function instead.
 *
 * @brief Restricts the usage permissions of a transient or persistent object.
 *
 * The TEE_RestrictObjectUsage function restricts the object usage flags of an object handle
 * to contain at most the flags passed in the objectUsage parameter.
 * For each bit in the parameter objectUsage:
 *   If the bit is set to 1, the corresponding usage flag in the object is left unchanged.
 *   If the bit is set to 0, the corresponding usage flag in the object is cleared.
 * For example, if the usage flags of the object are set to TEE_USAGE_ENCRYPT | TEE_USAGE_DECRYPT and
 * if objectUsage is set to TEE_USAGE_ENCRYPT | TEE_USAGE_EXTRACTABLE, then the only remaining usage
 * flag in the object after calling the function TEE_RestrictObjectUsage is TEE_USAGE_ENCRYPT.
 * Note that an object usage flag can only be cleared. Once it is cleared, it cannot be set to 1 again
 * until the whole object is reset using the function TEE_ResetObject. For a transient object, resetting
 * the object also clears all the key material stored in the container. For a persistent object,
 * setting the object usage MUST be an atomic operation.
 *
 * @param[in] object        Handle on an object
 * @param[in] objectUsage   New object usage, an OR combination of one or more of the TEE_USAGE_XXX
 *                          constants defined in Table 5-3 in GP spec
 *
 * @return   TEE_SUCCESS                Object info was copied form object.
 *           TEE_ERROR_BAD_PARAMETERS   Parameters were invalid to complete the operation
 */
extern void TEE_RestrictObjectUsage(
    TEE_ObjectHandle object,
    uint32_t objectUsage);

/**
 * @brief Extracts an attribute containing a buffer from a transient or persistent object.
 *
 * The TEE_GetObjectBufferAttribute function extracts one buffer attribute from an object.
 * The attribute is identified by the argument attributeID. The precise meaning of this parameter
 * depends on the container type and size and is defined in section 6.11.
 * Bit [29] of the attribute identifier must be set to 0, i.e., it must denote a buffer attribute.
 * They are two kinds of object attributes:
 *   Public object attributes can always be extracted whatever the status of the container.
 *   Secret attributes can be extracted only if the objects key usage contains the TEE_USAGE_EXTRACTABLE flag.
 * See section 6.11 for a definition of all available object attributes and their level of protection.
 *
 * @param[in]   object        Handle on an object
 * @param[in]   attributeID   Identifier of the attribute to retrieve
 * @param[out]  buffer        Output buffer to get the content of the attribute
 * @param[out]  size          Size of the output buffer
 *
 * @return
 * TEE_SUCCESS: In case of success
 * TEE_ERROR_ITEM_NOT_FOUND: If the attribute is not found on this object or if the object is a transient uninitialized object
 * TEE_ERROR_SHORT_BUFFER: If buffer is NULL or too small to contain the key part
 */

extern TEE_Result TEE_GetObjectBufferAttribute(
    TEE_ObjectHandle object,
    uint32_t attributeID,
  void* buffer, /*[outbuf]*/
  size_t* size);

/**
 * @brief Extracts an attribute containing two values from a transient or persistent object.
 *
 * The TEE_GetObjectValueAttribute function extracts a value attribute from an object.
 * The attribute is identified by the argument attributeID. The precise meaning of this parameter depends
 * on the container type and size and is defined in section 6.11.
 * Bit [29] of the attribute identifier must be set to 1, i.e., it must denote a value attribute.
 * They are two kinds of object attributes:
 *   Public object attributes can always be extracted whatever the status of the container.
 *   Secret attributes can be extracted only if the objects key usage contains the TEE_USAGE_EXTRACTABLE flag.
 * See section 6.11 for a definition of all available object attributes and their level of protection.
 *
 * @param[in]   object        Handle on an object
 * @param[in]   attributeID   Identifier of the attribute to retrieve
 * @param[out]  a             Pointer on the placeholder filled with the attribute field a. Can be NULL
 *                            if the corresponding field is not of interest to the caller.
 * @param[out]  b             Pointer on the placeholder filled with the attribute field b. Can be NULL
 *                            if the corresponding field is not of interest to the caller.
 *
 * @return
 * TEE_SUCCESS: In case of success
 * TEE_ERROR_ITEM_NOT_FOUND: If the attribute is not found on this object or if the object is a transient uninitialized object
 * TEE_ERROR_ACCESS_DENIED: For an attempt to extract a secret part of a non-extractable container
 */

extern TEE_Result TEE_GetObjectValueAttribute(
    TEE_ObjectHandle object,
    uint32_t attributeID,
  uint32_t* a, /*[outopt]*/
  uint32_t* b  /*[outopt]*/);

/**
 * @brief Closes an opened object handle.
 *
 * The TEE_CloseObject function closes an opened object handle. The object can be persistent or transient.
 *
 * @param[in]  object    Handle on an object
 */

extern void TEE_CloseObject(TEE_ObjectHandle object);

/**
 * Transient Objects
 */

/**
 * @brief allocates an uninitialized transient object, i.e., a
 *        container for attributes
 *
 * Transient objects are used to hold a cryptographic object
 * (key or key-pair). The object type and the maximum object
 * characteristic size must be specified so that all the
 * container resources can be pre-allocated
 *
 *  The returned handle is used to refer to the newly-created
 *  container in all subsequent functions that require an object
 *  container: key management and operation functions. The
 *  handle remains valid until the container is deallocated
 *  using the function TEE_FreeTransientObject.
 *
 *  the object type determines the possible object size:
 *
 *  Object Type                   Possible Object Sizes
 *  TEE_TYPE_AES                   128 or 256 bits
 *  TEE_TYPE_HMAC_SHA1             multiple of 8 bits
 *  TEE_TYPE_HMAC_SHA256           multiple of 8 bits
 *  TEE_TYPE_RSA_PUBLIC_KEY        Object size is the number of
 *                                 bits in the modulus. Upto
 *                                 4096 bits
 *  TEE_TYPE_RSA_KEYPAIR           Same as for RSA public key
 *  size.
 *
 * @param[in]  objectType     Type of uninitialized object container to be created
 * @param[in]  maxObjectSize  Size of the object as described
 *                            above
 * @param[out]  object        Filled with a handle on the newly created key container
 *
 * @return   TEE_SUCCESS                handle created
 *           TEE_ERROR_OUT_OF_MEMORY   not enough resources
 *                                     are available to allocate
 *                                     the object handle
 *           TEE_ERROR_NOT_SUPPORTED   Not supported Object type
 */
extern TEE_Result TEE_AllocateTransientObject(
    uint32_t objectType,
    uint32_t maxObjectSize,
    TEE_ObjectHandle* object /*[out]*/);

/**
 * @brief deallocates a transient object previously allocated with TEE_AllocateTransientObject
 *
 * After this function has been called, the object handle is no
 *  longer valid and all resources associated with the transient
 *  object must have been reclaimed
 *
 *
 * @param[in]  object        Handle on the object to free
 *
 * @return
 *
 */
extern void TEE_FreeTransientObject(
    TEE_ObjectHandle object);

/**
 * @brief resets a transient object to its initial state after allocation
 *
 * If the object is currently initialized, the function clears
 *  the object of all its material. The object is then
 *  uninitialized again.
 *
 * @param[out]  object        handle on transient object to
 *                            reset
 *
 * @return
 *
 */
extern void TEE_ResetTransientObject(
    TEE_ObjectHandle object);

/**
 * @brief populates an uninitialized object container with object attributes passed by the TA in the attrs parameter.
 *
 *
 *  the object type determines the possible attributes:
 *
 *  Object Type                   Possible attrs
 *  TEE_TYPE_AES                   For all secret key objects, the TEE_ATTR_SECRET_VALUE must be provided.
 *  TEE_TYPE_DES                   same as TEE_TYPE_AES, buffer associated must include parity bits
 *  TEE_TYPE_DES3                  same as TEE_TYPE_AES, buffer associated must include parity bits
 *  TEE_TYPE_HMAC_MD5              same as TEE_TYPE_AES
 *  TEE_TYPE_HMAC_SHA1             same as TEE_TYPE_AES
 *  TEE_TYPE_HMAC_SHA224           same as TEE_TYPE_AES
 *  TEE_TYPE_HMAC_SHA256           same as TEE_TYPE_AES
 *  TEE_TYPE_HMAC_SHA384           same as TEE_TYPE_AES
 *  TEE_TYPE_HMAC_SHA512           same as TEE_TYPE_AES
 *  TEE_TYPE_GENERIC_KEY           same as TEE_TYPE_AES
 *  TEE_TYPE_RSA_PUBLIC_KEY        TEE_ATTR_RSA_MODULUS
 *                                 TEE_ATTR_RSA_PUBLIC_EXPONENT
 *  TEE_TYPE_RSA_KEYPAIR           TEE_ATTR_RSA_MODULUS
 *                                 TEE_ATTR_RSA_PUBLIC_EXPONENT
 *                                 TEE_ATTR_RSA_PRIVATE_EXPONENT
 *                                 The CRT parameters are optional. If any of these parts is provided,
 *                                 then all of them must be provided:
 *                                 TEE_ATTR_RSA_PRIME1 (p)
 *                                 TEE_ATTR_RSA_PRIME2 (q)
 *                                 TEE_ATTR_RSA_EXPONENT1 (dP)
 *                                 TEE_ATTR_RSA_EXPONENT2 (dQ)
 *                                 TEE_ATTR_RSA_COEFFICIENT (pQ)
 *  TEE_TYPE_ECDSA_PUBLIC_KEY      Conditional: If ECC is supported, then the following parts MUST
 *                                 be provided.
 *                                 TEE_ATTR_ECC_PUBLIC_VALUE_X
 *                                 TEE_ATTR_ECC_PUBLIC_VALUE_Y
 *                                 TEE_ATTR_ECC_CURVE
 *  TEE_TYPE_ECDSA_KEYPAIR         Conditional: If ECC is supported, then the following parts MUST
 *                                 be provided.
 *                                 TEE_ATTR_ECC_PRIVATE_VALUE
 *                                 TEE_ATTR_ECC_PUBLIC_VALUE_X
 *                                 TEE_ATTR_ECC_PUBLIC_VALUE_Y
 *                                 TEE_ATTR_ECC_CURVE
 *  TEE_TYPE_ECDH_PUBLIC_KEY       Conditional: If ECC is supported, then the following parts MUST
 *                                 be provided.
 *                                 TEE_ATTR_ECC_PUBLIC_VALUE_X
 *                                 TEE_ATTR_ECC_PUBLIC_VALUE_Y
 *                                 TEE_ATTR_ECC_CURVE
 *  TEE_TYPE_ECDH_KEYPAIR          Conditional: If ECC is supported, then the following parts MUST
 *                                 be provided.
 *                                 TEE_ATTR_ECC_PRIVATE_VALUE
 *                                 TEE_ATTR_ECC_PUBLIC_VALUE_X
 *                                 TEE_ATTR_ECC_PUBLIC_VALUE_Y
 *                                 TEE_ATTR_ECC_CURVE
 *  TEE_TYPE_DSA_PUBLIC_KEY        TEE_ATTR_DSA_PRIME
 *                                 TEE_ATTR_DSA_SUBPRIME
 *                                 TEE_ATTR_DSA_BASE
 *                                 TEE_DSA_PUBLIC_VALUE
 *  TEE_TYPE_DSA_KEYPAIR           TEE_ATTR_DSA_PRIME
 *                                 TEE_ATTR_DSA_SUBPRIME
 *                                 TEE_ATTR_DSA_BASE
 *                                 TEE_ATTR_DSA_PRIVATE_VALUE
 *                                 TEE_ATTR_DSA_PUBLIC_VALUE
 *  TEE_TYPE_DH_KEYPAIR            TEE_ATTR_DH_PRIME
 *                                 TEE_ATTR_DH_BASE
 *                                 TEE_ATTR_DH_PUBLIC_VALUE
 *                                 TEE_ATTR_DH_PRIVATE_VALUE
 *                                 Optionally, TEE_ATTR_DH_SUBPRIME may be provided
 *
 * @param[in]  object        Handle on an already created
 *                           transient and uninitialized object
 * @param[in]  attrs         Array of object attributes
 * @param[in]  attrCount     number of elements in attrs array
 *
 * @return   TEE_SUCCESS               object populated
 *           TEE_ERROR_OUT_OF_MEMORY   not enough resources
 *                                     are available to allocate
 *                                     the object handle
 *           TEE_ERROR_BAD_PARAMETERS  Incorrect input
 *                                     parameters
 */
extern TEE_Result TEE_PopulateTransientObject(
    TEE_ObjectHandle object,
  TEE_Attribute* attrs, /*[in]*/
  uint32_t attrCount);

  /**
 * @brief populate a single attribute with a reference to
 *        a buffer
 *
 * @param[out]  attr          handle on transient object to
 *                            reset
 * @param[in]   attributeID   attribute Type
 * @param[in]   buffer        buffer holding data
 * @param[in]   length        size of buffer in bytes
 *
 * @return
 *
 */
extern void TEE_InitRefAttribute(
    TEE_Attribute* attr, /*[out]*/
    uint32_t attributeID,
  void* buffer, /*[inbuf]*/
  size_t length);

/**
 * @brief populate a single attribute with integer values
 *
 * @param[out]  attr          handle on transient object to
 *                            reset
 * @param[in]   attributeID   attribute Type
 * @param[in]   a             data
 * @param[in]   b             data
 *
 * @return
 *
 */
extern void TEE_InitValueAttribute(
    TEE_Attribute* attr, /*[out]*/
    uint32_t attributeID,
  uint32_t a,
  uint32_t b);

/**
 * This function replaces the TEE_CopyObjectAttributes function, whose use is deprecated
 *
 * @brief populates an uninitialized object handle with the
 *        attributes of another object handle
 *
 * populates the attributes of destObject with the attributes of
 * srcObject. It is most useful in the following situations
 *  To extract the public key attributes from a key-pair object
 *  To copy the attributes from a persistent object into a
 * transient object
 * destObject MUST refer to an uninitialized object handle and MUST therefore
 * be a transient object
 * The source and destination objects must have compatible types
 * and sizes in the following sense:
 *   The type of destObject must be a subtype of srcObject,
 *  i.e., one of the following must be true:
 *        o The type of destObject is equal to the type of
 *        srcObject.
 *        o The type of destObject is TEE_TYPE_RSA_PUBLIC_KEY
 *        and the type of srcObject is TEE_TYPE_RSA_KEYPAIR.
 *        o The type of destObject is TEE_TYPE_DSA_PUBLIC_KEY
 *        and the type of srcObject is TEE_TYPE_DSA_KEYPAIR.
 *        o The type of destObject is TEE_TYPE_ECDSA_PUBLIC_KEY
 *        and the type of srcObject is TEE_TYPE_ECDSA_KEYPAIR.
 *        o The type of destObject is TEE_TYPE_ECDH_PUBLIC_KEY
 *        and the type of srcObject is TEE_TYPE_ECDH_KEYPAIR.
 *    The size of srcObject must be less than or equal to the
 *   maximum size of destObject.
 *
 * @param[in]  destObject    Handle on an uninitialized
 *                           transient object
 * @param[in]  srcObject     Handle on an initialized object
 *
 * @return   TEE_SUCCESS                      destObject populated
 *           TEE_ERROR_CORRUPT_OBJECT         If the persistent object is corrupt
 *                                            the object handle is closed.
 *           TEE_ERROR_STORAGE_NOT_AVAILABLE  If the persistent object is stored
 *                                            in a storage area which is currently
 *                                            inaccessible
 */
extern TEE_Result TEE_CopyObjectAttributes1(
    TEE_ObjectHandle destObject,
    TEE_ObjectHandle srcObject);

/**
 * @brief populates an uninitialized object handle with the
 *        attributes of another object handle
 *
 * populates the attributes of destObject with the attributes of
 * srcObject. It is most useful in the following situations
 *  To extract the public key attributes from a key-pair object
 *  To copy the attributes from a persistent object into a
 * transient object
 * The source and destination objects must have compatible types
 * and sizes in the following sense:
 *   The type of destObject must be a subtype of srcObject,
 *  i.e., one of the following must be true:
 *        o The type of destObject is equal to the type of
 *        srcObject.
 *        o The type of destObject is TEE_TYPE_RSA_PUBLIC_KEY
 *        and the type of srcObject is TEE_TYPE_RSA_KEYPAIR.
 *    The size of srcObject must be less than or equal to the
 *   maximum size of destObject.
 *
 * @param[in]  destObject    Handle on an uninitialized
 *                           transient object
 * @param[in]  srcObject     Handle on an initialized object
 *
 * @return   TEE_SUCCESS               destObject populated
 *           TEE_ERROR_OUT_OF_MEMORY   not enough resources
 *                                     are available to allocate
 *                                     the object handle
 *           TEE_ERROR_BAD_PARAMETERS  Incorrect input
 *                                     parameters
 */

extern void TEE_CopyObjectAttributes(
    TEE_ObjectHandle destObject,
    TEE_ObjectHandle srcObject);

/**
 * @brief generates a random key or a key-pair and
 *        populates a transient key object with the
 *        generated key material.
 *
 * The size of the desired key is passed in the keySize
 * parameter and must be less than the maximum size
 * of the transient object.
 *
 * Object Type              Details
 * TEE_TYPE_AES              No Parameters necessary. The function generates the
 *                           attribute TEE_ATTR_SECRET_VALUE. The generated value
 *                           SHALL be the full key size.
 * TEE_TYPE_DES              As TEE_TYPE_AES
 * TEE_TYPE_DES3             As TEE_TYPE_AES
 * TEE_TYPE_HMAC_MD5         As TEE_TYPE_AES
 * TEE_TYPE_HMAC_SHA1        As TEE_TYPE_AES
 * TEE_TYPE_HMAC_SHA224      As TEE_TYPE_AES
 * TEE_TYPE_HMAC_SHA256      As TEE_TYPE_AES
 * TEE_TYPE_HMAC_SHA384      As TEE_TYPE_AES
 * TEE_TYPE_HMAC_SHA512      As TEE_TYPE_AES
 * TEE_TYPE_GENERIC_SECRET   As TEE_TYPE_AES
 *
 * TEE_TYPE_RSA_KEYPAIR      No parameter required
 *                           TEE_ATTR_RSA_PUBLIC_EXPONENT maybe be specifed;
 *                           if omitted the default is 65537
 *
 * TEE_TYPE_DSA_KEYPAIR      The following domain parameters MUST be passed:
 *                           TEE_ATTR_DSA_PRIME
 *                           TEE_ATTR_DSA_SUBPRIME
 *                           TEE_ATTR_DSA_BASE
 *
 * TEE_TYPE_DH_KEYPAIR       The following domain parameters MUST be passed:
 *                           TEE_ATTR_DH_PRIME
 *                           TEE_ATTR_DH_BASE
 *                           Optionally passed:
 *                           TEE_ATTR_DH_SUBPRIME
 *                           TEE_ATTR_DH_X_BITS
 * TEE_TYPE_ECDSA_KEYPAIR    The following domain parameters MUST be passed:
 *                           TEE_ATTR_ECC_CURVE
 *
 * TEE_TYPE_ECDH_KEYPAIR     The following domain parameters MUST be passed:
 *                           TEE_ATTR_ECC_CURVE
 *
 * @param[in]  object        Handle on an uninitialized transient key to
 *                           populate with the generated key
 * @param[in]  keySize       Requested key size in bits. Must be less than or equal
 *                           to the maximum size of the object container
 * @param[in]  params        Array of attribute parameters for key generation
 * @param[in]  paramCount    Number of params
 *
 * @return   TEE_SUCCESS                Key generated
 *           TEE_ERROR_GENERIC          Hardware/Software
 *                                      failure
 *           TEE_ERROR_BAD_PARAMETERS   Incorrect input
 *                                      parameters
 */

extern TEE_Result TEE_GenerateKey(
  TEE_ObjectHandle object,
    uint32_t keySize,
    TEE_Attribute* params,
  uint32_t paramCount);

/**
 * Persistent Objects
 */

/**
 * @brief Opens a handle on an existing persistent object
 *
 * Returns a handle that can be used to access the objects attributes and data stream.
 * The storageID parameter indicates which Trusted Storage Space to access.
 * The flags parameter is a set of flags that controls the access rights and sharing
 * permissions with which the object handle is opened. The value of the flags parameter
 * is constructed by a bitwise-inclusive OR of flags from the following list:
 *
 * Access control flags:
 *   - TEE_DATA_FLAG_ACCESS_READ:
 *     The object is opened with the read access right. This allows the Trusted Application
 *     to call the function TEE_ReadObjectData.
 *   - TEE_DATA_FLAG_ACCESS_WRITE:
 *     The object is opened with the write access right. This allows the Trusted Application
 *     to call the functions TEE_WriteObjectData and TEE_TruncateObjectData.
 *   - TEE_DATA_FLAG_ACCESS_WRITE_META:
 *     The object is opened with the write-meta access right. This allows the Trusted
 *     Application to call the functions TEE_CloseAndDeletePersistentObject and
 *     TEE_RenamePersistentObject.
 *
 * Sharing permission control flags:
 *   - TEE_DATA_FLAG_SHARE_READ:
 *     The caller allows another handle on the object to be created with read access.
 *   - TEE_DATA_FLAG_SHARE_WRITE:
 *    The caller allows another handle on the object to be created with write access.
 *   - TEE_DATA_FLAG_OVERWRITE:
 *     If this flag is present and the object exists, then the object is deleted and
 *     re-created as an atomic operation: that is the TA sees either the old object or
 *     the new one. If the flag is absent and the object exists, then the function SHALL
 *     return TEE_ERROR_ACCESS_CONFLICT.
 *
 * Other flags are reserved for future use and SHALL be set to 0.
 *
 * Multiple handles may be opened on the same object simultaneously, but sharing MUST be
 * explicitly allowed.
 * The initial data position in the data stream is set to 0.
 * Every Trusted Storage implementation is expected to return TEE_ERROR_CORRUPT_OBJECT if
 * a Trusted Application attempts to open an object and the TEE determines that its contents
 * (or those of the storage itself) have been tampered with or rolled back.
 *
 * @param[in]  storageID    The storage to use. It must be TEE_STORAGE_PRIVATE.
 * @param[in]  objectID     The object identifier. Note that this cannot reside in shared
 *                          memory.
 * @param[in]  objectIDLen  Length of objectID
 * @param[in]  flags        The flags which determine the settings under which the object
 *                          is opened.
 * @param[out] object       A pointer to the handle, which contains the opened handle upon
 *                          successful completion. If this function fails for any reason,
 *                          the value pointed to by object is set to TEE_HANDLE_NULL. When
 *                          the object handle is no longer required, it MUST be closed using
 *                          a call to the TEE_CloseObject function.
 *
 * @return
 * TEE_SUCCESS:                     In case of success
 * TEE_ERROR_ITEM_NOT_FOUND:        If the storage denoted by storageID does not exist or if
 *                                 the object identifier cannot be found in the storage
 * TEE_ERROR_ACCESS_CONFLICT:       If an access right conflict was detected while opening the
 *                                object
 * TEE_ERROR_OUT_OF_MEMORY:         If there is not enough memory to complete the operation
 * TEE_ERROR_CORRUPT_OBJECT:        If the storage is corrupt
 * TEE_ERROR_STORAGE_NOT_AVAILABLE: If the persistent object is stored in a storage area which
 *                                is currently inaccessible.
 *
 */
extern TEE_Result TEE_OpenPersistentObject(
  uint32_t storageID,
  void* objectID,
  size_t objectIDLen,
  uint32_t flags,
  TEE_ObjectHandle* object);

/**
 * The TEE_CreatePersistentObject function creates a persistent object with initial attributes
 * and an initial data stream content, and optionally returns either a handle on the created
 * object, or TEE_HANDLE_NULL upon failure.
 * The storageID parameter indicates which Trusted Storage Space to access
 * The flags parameter is a set of flags that controls the access rights, sharing permissions,
 * and object creation mechanism with which the object handle is opened. The value of the flags
 * parameter is constructed by a bitwise-inclusive OR of flags from the following list:
 *
 *  Access control flags:
 *    - TEE_DATA_FLAG_ACCESS_READ:
 *      The object is opened with the read access right. This allows the Trusted Application
 *      to call the function TEE_ReadObjectData.
 *    - TEE_DATA_FLAG_ACCESS_WRITE:
 *      The object is opened with the write access right. This allows the Trusted Application
 *      to call the functions TEE_WriteObjectData and TEE_TruncateObjectData.
 *    - TEE_DATA_FLAG_ACCESS_WRITE_META:
 *      The object is opened with the write-meta access right. This allows the Trusted
 *      Application to call the functions TEE_CloseAndDeletePersistentObject and
 *      TEE_RenamePersistentObject.
 *
 *  Sharing permission control flags:
 *    - TEE_DATA_FLAG_SHARE_READ:
 *      The caller allows another handle on the object to be created with read access.
 *    - TEE_DATA_FLAG_SHARE_WRITE:
 *      The caller allows another handle on the object to be created with write access.
 *    - TEE_DATA_FLAG_OVERWRITE:
 *      If this flag is present and the object exists, then the object is deleted and
 *      re-created as an atomic operation: that is the TA sees either the old object or
 *      the new one. If the flag is absent and the object exists, then the function SHALL
 *      return TEE_ERROR_ACCESS_CONFLICT.
 *
 * Other flags are reserved for future use and SHALL be set to 0.
 *
 * The attributes of the newly created persistent object are taken from attributes, which can
 * be another persistent object or an initialized transient object. The attributes argument
 * can also be NULL for a pure data object. The object type, size, and usage are copied from
 * attributes. If attributes is NULL, the object type SHALL be set to TEE_TYPE_DATA to create
 * a pure data object.
 * Multiple handles may be opened on the same object simultaneously, but sharing MUST be
 * explicitly allowed.
 * The initial data position in the data stream is set to 0.
 *
 * @param[in] storageID      The storage to use. It must be TEE_STORAGE_PRIVATE.
 * @param[in] objectID       The object identifier. Note that this cannot reside in shared
 *                           memory.
 * @param[in] objectIDLen    Length of objectID
 * @param[in] flags          The flags which determine the settings under which the object
 *                           is opened
 * @param[in] attributes     A handle on a persistent object or an initialized transient
 *                           object from which to take the persistent object attributes.
 *                           Can be TEE_HANDLE_NULL if the persistent object contains no
 *                           attribute; for example, if it is a pure data object.
 * @param[in] initialData    The initial data content of the persistent object
 * @param[in] initialDataLen Length of initialData
 * @param[out] object        A pointer to the handle, which contains the opened handle upon
 *                           successful completion. If this function fails for any reason,
 *                           the value pointed to by object is set to TEE_HANDLE_NULL. When
 *                           the object handle is no longer required, it MUST be closed using
 *                           a call to the TEE_CloseObject function.
 *
 * @return
 * TEE_SUCCESS:                      In case of success
 * TEE_ERROR_ITEM_NOT_FOUND:         If the storage denoted by storageID does not exist or if
 *                                   the object identifier cannot be found in the storage
 * TEE_ERROR_ACCESS_CONFLICT:        If an access right conflict was detected while opening the
 *                                   object
 * TEE_ERROR_OUT_OF_MEMORY:          If there is not enough memory to complete the operation
 * TEE_ERROR_STORAGE_NO_SPACE:       If insufficient space is available to create the persistent
 *                                   object
 * TEE_ERROR_CORRUPT_OBJECT:         If the storage is corrupt
 * TEE_ERROR_STORAGE_NOT_AVAILABLE:  If the persistent object is stored in a storage area which
 *                                   is currently inaccessible.
 *
 **/
extern TEE_Result TEE_CreatePersistentObject(
  uint32_t storageID,
  void* objectID, size_t objectIDLen,
  uint32_t flags,
  TEE_ObjectHandle attributes,
  void* initialData, size_t initialDataLen,
  TEE_ObjectHandle* object);

/**
 * The TEE_CloseAndDeletePersistentObject1 function marks an object for deletion and closes
 * the object handle.
 * The object handle MUST have been opened with the write-meta access right, which means
 * access to the object is exclusive.
 * Deleting an object is atomic; once this function returns, the object is definitely deleted
 * and no more open handles for the object exist. This SHALL be the case even if the object or
 * the storage containing it have become corrupted.
 * The only reason this routine can fail is if the storage area containing the object becomes
 * inaccessible (e.g. the user removes the media holding the object). In this case
 * TEE_ERROR_STORAGE_NOT_AVAILABLE SHALL be returned.
 * If object is TEE_HANDLE_NULL, the function does nothing.
 *
 * @param[in] object   The object handle
 *
 * @return
 *
 **/
extern TEE_Result TEE_CloseAndDeletePersistentObject1( TEE_ObjectHandle object );

/**
 * Use of this function is DEPRECATED  new code SHOULD use the
 * TEE_CloseAndDeletePersistentObject1 function instead.
 *
 * The TEE_CloseAndDeletePersistentObject function marks an object for deletion and closes
 * the object handle.
 * The object handle MUST have been opened with the write-meta access right, which means
 * access to the object is exclusive.
 * Deleting an object is atomic; once this function returns, the object is definitely deleted
 * and no more open handles for the object exist. This SHALL be the case even if the object or
 * the storage containing it have become corrupted.
 * If the storage containing the object is unavailable then this routine SHALL panic.
 * If object is TEE_HANDLE_NULL, the function does nothing.
 *
 * @param[in] object   The object handle
 *
 * @return
 *
 **/
extern void TEE_CloseAndDeletePersistentObject(TEE_ObjectHandle object);

/**
 * The function TEE_RenamePersistentObject changes the identifier of an object. The object
 * handle MUST have been opened with the write-meta access right, which means access to the
 * object is exclusive.
 * Renaming an object is an atomic operation; either the object is renamed or nothing happens.
 *
 * @param[in] object         The object handle
 * @param[in] newObjectID    A buffer containing the new object identifier. The identifier
 *                           contains arbitrary bytes, including the zero byte. The buffer
 *                           containing the new object identifier cannot reside in shared
 *                           memory.
 * @param[in] newObjectIDLen The identifier length MUST be less than or equal to
 *                           TEE_OBJECT_ID_MAX_LEN and can be zero.
 *
 * @return
 * TEE_SUCCESS:                      In case of success
 * TEE_ERROR_ACCESS_CONFLICT:        If an object with the same identifier already exists
 * TEE_ERROR_CORRUPT_OBJECT:         If the storage is corrupt
 * TEE_ERROR_STORAGE_NOT_AVAILABLE:  If the persistent object is stored in a storage area which
 *                                   is currently inaccessible.
 *
 **/
extern TEE_Result TEE_RenamePersistentObject(
  TEE_ObjectHandle object,
    void* newObjectID,
  size_t newObjectIDLen);

/**
 * The TEE_AllocatePersistentObjectEnumerator function allocates a handle on an object
 * enumerator. Once an object enumerator handle has been allocated, it can be reused for
 * multiple enumerations.
 *
 * @param[out] objectEnumerator   A pointer filled with the newly-allocated object
 *                               enumerator handle on success. Set to TEE_HANDLE_NULL
 *                               in case of error.
 *
 * @return
 * TEE_SUCCESS:              In case of success
 * TEE_ERROR_OUT_OF_MEMORY:  If there is not enough memory to allocate the enumerator
 *                           handle
 *
 **/
extern TEE_Result TEE_AllocatePersistentObjectEnumerator(
  TEE_ObjectEnumHandle* objectEnumerator);

/**
 * The TEE_FreePersistentObjectEnumerator function deallocates all resources associated
 * with an object enumerator handle. After this function is called, the handle is no longer
 * valid.
 *
 * @param[in] objectEnumerator  The handle to close. If objectEnumerator is TEE_HANDLE_NULL,
 *                             then this function does nothing.
 *
 **/
extern void TEE_FreePersistentObjectEnumerator(
  TEE_ObjectEnumHandle objectEnumerator);

/**
 * The TEE_ResetPersistentObjectEnumerator function resets an object enumerator handle to
 * its initial state after allocation. If an enumeration has been started, it is stopped.
 * This function does nothing if objectEnumerator is TEE_HANDLE_NULL.
 *
 * @param[in] objectEnumerator  The handle to reset.
 *
 **/
extern void TEE_ResetPersistentObjectEnumerator(
  TEE_ObjectEnumHandle objectEnumerator);

/**
The TEE_StartPersistentObjectEnumerator function starts the enumeration of all the
persistent objects in a given Trusted Storage. The object information can be retrieved
by calling the function TEE_GetNextPersistentObject repeatedly.
The enumeration does not necessarily reflect a given consistent state of the storage:
During the enumeration, other TAs or other instances of the TA may create, delete, or
rename objects. It is not guaranteed that all objects will be returned if objects are
created or destroyed while the enumeration is in progress.
To stop an enumeration, the TA can call the function TEE_ResetPersistentObjectEnumerator,
which detaches the enumerator from the Trusted Storage. The TA can call the function
TEE_FreePersistentObjectEnumerator to completely deallocate the object enumerator.
If this function is called on an enumerator that has already been started, the enumeration
is first reset then started.

@param[in] objectEnumerator   A valid handle on an object enumerator
@param[in] storageID          The identifier of the storage in which the objects MUST be
                              enumerated.

@return
TEE_SUCCESS:                     In case of success
TEE_ERROR_ITEM_NOT_FOUND:        If the storage does not exist or if there is no object
                                 in the specified storage
TEE_ERROR_CORRUPT_OBJECT:        If the storage is corrupt
TEE_ERROR_STORAGE_NOT_AVAILABLE: If the persistent object is stored in a storage area
                                 which is currently inaccessible.
*/
extern TEE_Result TEE_StartPersistentObjectEnumerator(
  TEE_ObjectEnumHandle objectEnumerator,
  uint32_t storageID);

/**
 * The TEE_GetNextPersistentObject function gets the next object in an enumeration and
 * returns information about the object: type, size, identifier, etc.
 * If there are no more objects in the enumeration or if there is no enumeration started,
 * then the function returns TEE_ERROR_ITEM_NOT_FOUND.
 * If while enumerating objects a corrupt object is detected, then its object ID SHALL
 * be returned in objectID, objectInfo shall be zeroed, and the function SHALL return
 * TEE_ERROR_CORRUPT_OBJECT.
 *
 * @param[in]  objectEnumerator  A handle on the object enumeration
 * @param[out] objectInfo        A pointer to a TEE_ObjectInfo filled with the object
 *                               information as specified in the function TEE_GetObjectInfo1
 *                               in section 5.5.1. It may be NULL.
 * @param[out] objectID          Pointer to an array able to hold at least TEE_OBJECT_ID_MAX_LEN
 *                               bytes. On exit the object identifier is written to this location.
 * @param[out] objectIDLen       Filled with the size of the object identifier (from 0 to
 *                               TEE_OBJECT_ID_MAX_LEN)
 *
 * @return
 * TEE_SUCCESS:                     In case of success
 * TEE_ERROR_ITEM_NOT_FOUND:        If there are no more elements in the object enumeration
 *                                  or if no enumeration is started on this handle
 * TEE_ERROR_CORRUPT_OBJECT:        If the storage or returned object is corrupt
 * TEE_ERROR_STORAGE_NOT_AVAILABLE: If the persistent object is stored in a storage area
 *                                  which is currently inaccessible.
 **/
extern TEE_Result TEE_GetNextPersistentObject(
  TEE_ObjectEnumHandle objectEnumerator,
  TEE_ObjectInfo* objectInfo,
  void* objectID,
  size_t* objectIDLen);

/**
 * The TEE_ReadObjectData function attempts to read size bytes from the data stream associated
 * with the object object into the buffer pointed to by buffer.
 * The object handle MUST have been opened with the read access right.
 * The bytes are read starting at the position in the data stream currently stored in the
 * object handle. The handles position is incremented by the number of bytes actually read.
 * On completion TEE_ReadObjectData sets the number of bytes actually read in the uint32_t
 * pointed to by count. The value written to *count may be less than size if the number of
 * bytes until the end-ofstream is less than size. It is set to 0 if the position at the start
 * of the read operation is at or beyond the end-of-stream. These are the only cases where
 * *count may be less than size.
 * No data transfer can occur past the current end of stream. If an attempt is made to read
 * past the end-ofstream, the TEE_ReadObjectData function stops reading data at the
 * end-of-stream and returns the data read up to that point. This is still a success. The
 * position indicator is then set at the end-of-stream. If the position is at, or past, the end
 * of the data when this function is called, then no bytes are copied to *buffer and *count is
 * set to 0.
 *
 * @param[in]  object   The object handle
 * @param[out] buffer   A pointer to the memory which, upon successful completion, contains
 *                      the bytes read
 * @param[in]  size     The number of bytes to read
 * @param[out] count    A pointer to the variable which upon successful completion contains
 *                      the number of bytes read
 *
 * @return
 * TEE_SUCCESS                       In case of success
 * TEE_ERROR_CORRUPT_OBJECT:         If the storage is corrupt
 * TEE_ERROR_STORAGE_NOT_AVAILABLE:  If the persistent object is stored in a storage area which
 *                                   is currently inaccessible.
 **/
extern TEE_Result TEE_ReadObjectData(
  TEE_ObjectHandle object,
  void* buffer,
  size_t size,
  uint32_t* count);

/**
 * The TEE_WriteObjectData function writes size bytes from the buffer pointed to by buffer to
 * the data stream associated with the open object handle object.
 * The object handle MUST have been opened with the write access permission.
 * If the current data position points before the end-of-stream, then size bytes are written
 * to the data stream, overwriting bytes starting at the current data position. If the current
 * data position points beyond the streams end, then the data stream is first extended with
 * zero bytes until the length indicated by the data position indicator is reached, and then
 * size bytes are written to the stream. Thus, the size of the data stream can be increased as
 * a result of this operation.
 * If the operation would move the data position indicator to beyond its maximum possible value,
 * then TEE_ERROR_OVERFLOW is returned and the operation fails.
 * The data position indicator is advanced by size. The data position indicators of other
 * object handles opened on the same object are not changed.
 * Writing in a data stream is atomic; either the entire operation completes successfully or
 * no write is done.
 *
 * @param[in] object   The object handle
 * @param[in] buffer   The buffer containing the data to be written
 * @param[in] size     The number of bytes to write
 *
 * @return
 * TEE_SUCCESS:                      In case of success
 * TEE_ERROR_STORAGE_NO_SPACE:       If insufficient storage space is available
 * TEE_ERROR_OVERFLOW:               If the value of the data position indicator resulting
 *                                   from this operation would be greater than
 *                                   TEE_DATA_MAX_POSITION
 * TEE_ERROR_CORRUPT_OBJECT:         If the storage is corrupt
 * TEE_ERROR_STORAGE_NOT_AVAILABLE:  If the persistent object is stored in a storage area which
 *                                   is currently inaccessible.
 *
 **/
extern TEE_Result TEE_WriteObjectData(
  TEE_ObjectHandle object,
  void* buffer,
  size_t size);

/**
The function TEE_TruncateObjectData changes the size of a data stream. If size is less
than the current size of the data stream then all bytes beyond size are removed. If size
is greater than the current size of the data stream then the data stream is extended by
adding zero bytes at the end of the stream.
The object handle MUST have been opened with the write access permission.
This operation does not change the data position of any handle opened on the object.
Note that if the current data position of such a handle is beyond size, the data position
will point beyond the object datas end after truncation.
Truncating a data stream is atomic: Either the data stream is successfully truncated or
nothing happens.

@param[in] object   The object handle
@param[in] size     The new size of the data stream

@return
TEE_SUCCESS:                      In case of success
TEE_ERROR_STORAGE_NO_SPACE:       If insufficient storage space is available
TEE_ERROR_CORRUPT_OBJECT:         If the storage is corrupt
TEE_ERROR_STORAGE_NOT_AVAILABLE:  If the persistent object is stored in a storage area which
                                  is currently inaccessible.

*/
extern TEE_Result TEE_TruncateObjectData(
  TEE_ObjectHandle object,
  uint32_t size);

/**
 * The TEE_SeekObjectData function sets the data position indicator associated with the
 * object handle.
 * The parameter whence controls the meaning of offset:
 *     - If whence is TEE_DATA_SEEK_SET,
 *       the data position is set to offset bytes from the beginning of the data stream.
 *     - If whence is TEE_DATA_SEEK_CUR, the data position is set to its current position
 *       plus offset.
 *     - If whence is TEE_DATA_SEEK_END, the data position is set to the size of the object
 *       data plus offset.
 *
 * The TEE_SeekObjectData function may be used to set the data position beyond the end of
 * stream; this does not constitute an error. However, the data position indicator does have
 * a maximum value which is TEE_DATA_MAX_POSITION. If the value of the data position indicator
 * resulting from this operation would be greater than TEE_DATA_MAX_POSITION, the error
 * TEE_ERROR_OVERFLOW is returned.
 * If an attempt is made to move the data position before the beginning of the data stream,
 * the data position is set at the beginning of the stream. This does not constitute an error.
 *
 * @param[in] object   The object handle
 * @param[in] offset   The number of bytes to move the data position.
 *                     A positive value moves the data position forward; a negative value
 *                     moves the data position backward.
 * @param[in] whence   The position in the data stream from which to calculate the new
 *                     position
 *
 * @return
 * TEE_SUCCESS:                      In case of success
 * TEE_ERROR_OVERFLOW:               If the value of the data position indicator resulting
 *                                   from this operation would be greater than
 *                                   TEE_DATA_MAX_POSITION
 * TEE_ERROR_CORRUPT_OBJECT:         If the storage is corrupt
 * TEE_ERROR_STORAGE_NOT_AVAILABLE:  If the persistent object is stored in a storage area
 *                                   which is currently inaccessible.
 **/
extern TEE_Result TEE_SeekObjectData(
  TEE_ObjectHandle object,
  int32_t offset,
  TEE_Whence whence);

/******************************************************************************
Cryptographic Operations Functions (28)
******************************************************************************/
/**
 * @brief allocates a handle for a new cryptographic operation and sets the mode and algorithm type
 *
 * The TEE_AllocateOperation function allocates a handle for a new cryptographic operation and sets the mode and algorithm type.
 * If this function does not return with TEE_SUCCESS then there is no valid handle value.
 *     Supported algorithm                   Supported mode                         Supported maxKeySize
 *     TEE_ALG_AES_ECB_NOPAD                 TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     128, 256bits
 *     TEE_ALG_AES_CBC_NOPAD                 TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     128, 256bits
 *     TEE_ALG_AES_CTR                       TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     128, 256bits
 *     TEE_ALG_AES_CTS                       TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     128, 256bits
 *     TEE_ALG_AES_XTS                       TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     128, 256bits
 *     TEE_ALG_AES_CCM                       TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     128, 256bits
 *     TEE_ALG_AES_GCM                       TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     128, 256bits
 *     TEE_ALG_DES_ECB_NOPAD                 TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     128, 256bits
 *     TEE_ALG_DES_CBC_NOPAD                 TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     128, 256bits
 *     TEE_ALG_DES3_ECB_NOPAD                TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     128, 256bits
 *     TEE_ALG_DES3_CBC_NOPAD                TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     128, 256bits
 *     TEE_ALG_DES_CBC_MAC_NOPAD             TEE_MODE_MAC
 *     TEE_ALG_AES_CBC_MAC_NOPAD             TEE_MODE_MAC
 *     TEE_ALG_AES_CBC_MAC_PCKS5             TEE_MODE_MAC
 *     TEE_ALG_AES_CMAC                      TEE_MODE_MAC
 *     TEE_ALG_DES_CBC_MAC_PKCS5             TEE_MODE_MAC
 *     TEE_ALG_DES3_CBC_MAC_NOPAD            TEE_MODE_MAC
 *     TEE_ALG_DES3_CBC_MAC_PKCS5            TEE_MODE_MAC
 *     TEE_ALG_RSASSA_PKCS1_V1_5_MD5         TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_RSASSA_PKCS1_V1_5_SHA1        TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_RSASSA_PKCS1_V1_5_SHA224      TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_RSASSA_PKCS1_V1_5_SHA256      TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_RSASSA_PKCS1_V1_5_SHA384      TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_RSASSA_PKCS1_V1_5_SHA8512     TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1    TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224  TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256  TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384  TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512  TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_DSA_SHA1                      TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_DSA_SHA224                    TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_DSA_SHA256                    TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_ECDSA                         TEE_MODE_SIGN, TEE_MODE_VERIFY         upto 4096bits
 *     TEE_ALG_RSAES_PKCS1_V1_5              TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     upto 4096bits
 *     TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1    TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     upto 4096bits
 *     TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA224  TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     upto 4096bits
 *     TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA256  TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     upto 4096bits
 *     TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA384  TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     upto 4096bits
 *     TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA512  TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     upto 4096bits
 *     TEE_ALG_RSA_NOPAD                     TEE_MODE_ENCRYPT, TEE_MODE_DECRYPT     upto 4096bits
 *     TEE_ALG_DH_DERIVE_SHARED_SECRET       TEE_MODE_DERIVE
 *     TEE_ALG_ECDH_DERIVE_SHARED_SECRET     TEE_MODE_DERIVE
 *     TEE_ALG_MD5                           TEE_MODE_DIGEST
 *     TEE_ALG_SHA1                          TEE_MODE_DIGEST
 *     TEE_ALG_SHA224                        TEE_MODE_DIGEST
 *     TEE_ALG_SHA256                        TEE_MODE_DIGEST
 *     TEE_ALG_SHA384                        TEE_MODE_DIGEST
 *     TEE_ALG_SHA512                        TEE_MODE_DIGEST
 *     TEE_ALG_HMAC_MD5                      TEE_MODE_MAC
 *     TEE_ALG_HMAC_SHA1                     TEE_MODE_MAC
 *     TEE_ALG_HMAC_SHA224                   TEE_MODE_MAC
 *     TEE_ALG_HMAC_SHA256                   TEE_MODE_MAC
 *     TEE_ALG_HMAC_SHA384                   TEE_MODE_MAC
 *     TEE_ALG_HMAC_SHA512                   TEE_MODE_MAC
 *
 *
 * @param[out]  operation   A pointer to a TEE_OperationHandle
 * @param[in]   algorithm   One of the cipher algorithms enumerated above
 * @param[in]   mode        The operation mode
 * @param[in]   maxKeySize  Maximum key size in bits for the operation
 *
 * @return   TEE_SUCCESS                Handle is allocated
 *           TEE_ERROR_OUT_OF_MEMORY    No free memory
 *           TEE_ERROR_NOT_SUPPORTED    Incompatible mode for alg or key size, not a listed alg
 *                                      or maxKeySize not appropriate for alg
 */
extern TEE_Result TEE_AllocateOperation(
  TEE_OperationHandle *operation,
  uint32_t algorithm,
  uint32_t mode,
  uint32_t maxKeySize
);

/**
 * @brief The TEE_Free Operation function deallocates all
 *        resources associated with an operation handle. After
 *        this function is called, the operation handle is no
 *        longer valid.
 *
 * @param[in]   operation   handle allocated during
 *       TEE_AllocateOperation
 *
 * @return      none
 *
 */
extern void TEE_FreeOperation( TEE_OperationHandle operation);

/**
 * @brief returns information about an operation handle
 *
 * It fills the following fields in the structure
 * operationInfoMultiple
 *
 *   algorithm, mode, maxKeySize: The parameters passed to the
 *                        function TEE_AllocateOperation
 *   algorithmClass: One of the following constants, describing
 *                   the kind of operation:
 *              o TEE_OPERATION_CIPHER
 *              o TEE_OPERATION_MAC
 *              o TEE_OPERATION_AE
 *              o TEE_OPERATION_DIGEST
 *              o TEE_OPERATION_ASYMMETRIC_CIPHER
 *              o TEE_OPERATION_ASYMMETRIC_SIGNATURE
 *              o TEE_OPERATION_KEY_DERIVATION
 *   digestLength: For a MAC, AE, or Digest , describes
 *                 the number of bytes in the digest or tag
 *   handleState: A bit vector describing the current state of
 *                 the operation. Contains one or more of the
 *                 following flags:
 *            o TEE_HANDLE_FLAG_EXPECT_TWO_KEYS: Set if the
 *            algorithm expects two keys to be set, using
 *            TEE_SetOperationKey2. This happens only if
 *            algorithm is set to TEE_ALG_AES_XTS.
 *            o TEE_HANDLE_FLAG_KEY_SET: Set if the operation
 *            key has been set. Always set for digest operations
 *            o TEE_HANDLE_FLAG_INITIALIZED: For multi-stage
 *   operationState: One of the following constants, describing
 *                     the state of operation:
 *            o TEE_OPERATION_STATE_INITIAL
 *            o TEE_OPERATION_STATE_ACTIVE
 *   numberOfKeys: Number of TEE_OperationInfoKey contained in
 *                  keyInformation structure. Maybe 0 if
 *                  operation does not require any keys
 *   keyInformation: Array containing numberOfKeys entries
 *                  defining details of one key used by the
 *                  operation in the order they are defined.
 *                  Each element:
 *            o keySize: If a key is programmed in the
 *               operation, the actual size of this key
 *            o requiredKeyUsage: A bit vector that describes
 *                  the necessary bits in the object usage for
 *                  TEE_SetOperationKey or TEE_SetOperationKey2
 *                  to succeed without panicking.
 *
 *
 * @param[in]  operation     A pointer to a TEE_OperationHandle
 *                            allocated by TEE_AllocateOperation
 * @param[out] operationInfoMultiple Fills above operation info
 *                            at this pointer location
 * @param[in,out] operationSize
 *
 * @return   TEE_SUCCESS            OperationInfoMultiple filled
 *           TEE_ERROR_SHORT_BUFFER Buffer not large enough to
 *                                  hold OperationInfoMultiple
 */
extern TEE_Result TEE_GetOperationInfoMultiple(
  TEE_OperationHandle operation,
  TEE_OperationInfoMultiple* operationInfoMultiple,
  size_t* operationSize
);

/**
 * @brief returns information about an operation handle
 *
 * It fills the following fields in the structure operationInfo
 *
 *   algorithm, mode, maxKeySize: The parameters passed to the
 *                        function TEE_AllocateOperation
 *   algorithmClass: One of the following constants, describing
 *                   the kind of operation:
 *              o TEE_OPERATION_CIPHER
 *              o TEE_OPERATION_MAC
 *              o TEE_OPERATION_AE
 *              o TEE_OPERATION_DIGEST
 *              o TEE_OPERATION_ASYMMETRIC_CIPHER
 *              o TEE_OPERATION_ASYMMETRIC_SIGNATURE
 *  keySize: If a key is programmed in the operation, the
 *            actual size of this key
 *  requiredKeyUsage: A bit vector that describes the necessary
 *                  bits in the object usage for
 *                  TEE_SetOperationKey or TEE_SetOperationKey2
 *                  to succeed without panicking. Set to 0 for a
 *                  digest operation.
 *  digestLength: For a MAC, AE, or Digest , describes
 *                 the number of bytes in the digest or tag
 *   handleState: A bit vector describing the current state of
 *                 the operation. Contains one or more of the
 *                 following flags:
 *            o TEE_HANDLE_FLAG_EXPECT_TWO_KEYS: Set if the
 *            algorithm expects two keys to be set, using
 *            TEE_SetOperationKey2. This happens only if
 *            algorithm is set to TEE_ALG_AES_XTS.
 *            o TEE_HANDLE_FLAG_KEY_SET: Set if the operation
 *            key has been set. Always set for digest operations
 *            o TEE_HANDLE_FLAG_INITIALIZED: For multi-stage
 *            operations
 *
 * @param[in]  operation     A pointer to a TEE_OperationHandle
 *                            allocated by TEE_AllocateOperation
 * @param[out] operationInfo Fills above operation info at
 *                            this pointer location
 *
 */
extern void TEE_GetOperationInfo(
  TEE_OperationHandle operation,
  TEE_OperationInfo* operationInfo
);

/**
 * @brief Resets the operation
 *
 * For a multi-stage operation, the TEE_ResetOperation function
 * resets the operation state before initialization, but after
 * the key has been set.
 * Only for the multi-stage operations, i.e., symmetric ciphers,
 * MACs, AEs, and digests. When such a multi-stage operation is
 * active, i.e., when it has been initialized but not yet
 * successfully finalized, then the operation is reset to its
 * pre-initialization state. The operation key(s) are not
 * cleared.
 *
 *  Valid only for
 *      TEE_OPERATION_CIPHER
 *      TEE_OPERATION_DIGEST
 *      TEE_OPERATION_MAC
 *      TEE_OPERATION_AE
 *
 * @param[in]  operation     A pointer to a TEE_OperationHandle
 *                            allocated by TEE_AllocateOperation
 *
 */
extern void TEE_ResetOperation( TEE_OperationHandle operation);

/**
 * @brief associates an operation with a key
 *
 * The key material is copied from the key object handle into
 * the operation. After the key has been set, there is no longer
 * any link between the operation and the key object. The object
 * handle can be closed or reset and this will not affect the
 * operation.
 *
 * The key object type and size must be compatible with the type
 * and size of the operation. The operation mode must be
 * compatible with key usage:
 *  In general, the operation mode must be allowed in the
 * object usage.
 *  For the TEE_ALG_RSA_NOPAD algorithm:
 *        o The only supported modes are TEE_MODE_ENCRYPT and
 *        TEE_MODE_DECRYPT.
 *        o For TEE_MODE_ENCRYPT, the object usage must contain
 *        both the TEE_USAGE_ENCRYPT and TEE_USAGE_VERIFY flags.
 *        o For TEE_MODE_DECRYPT, the object usage must contain
 *        both the TEE_USAGE_DECRYPT and TEE_USAGE_SIGN flags.
 *  For a public key object, the operation mode can only be
 * TEE_MODE_ENCRYPT or TEE_MODE_VERIFY but cannot be
 * TEE_MODE_DECRYPT or TEE_MODE_SIGN as these kinds of
 * operations require the private parts of a key-pair.
 *  If the object is a key-pair then the key parts used in the
 * operation depend on the mode:
 *        o For a TEE_MODE_ENCRYPT or TEE_MODE_VERIFY operation,
 *        the public parts of the key-pair are used.
 *        o For a TEE_MODE_DECRYPT or TEE_MODE_SIGN, the private
 *        parts of the key-pair are used.
 *
 * @param[in]  operation     A pointer to a TEE_OperationHandle
 *                            allocated by TEE_AllocateOperation
 * @param[in]  key           Initialized key
 *
 * @return   TEE_SUCCESS                      Key associated with
 *                                            operation
 *           TEE_ERROR_CORRUPT_OBJECT         If object is corrupt.
 *                                            Object handle is closed.
 *           TEE_ERROR_STORAGE_NOT_AVAILABLE  Persistent object is stored
 *                                            in inaccessible storage area
 */
extern TEE_Result TEE_SetOperationKey(
  TEE_OperationHandle operation,
  TEE_ObjectHandle key
);

/**
 * @brief initializes an existing operation with two keys
 *
 * This is used only for the algorithm TEE_ALG_AES_XTS.
 * The key material is copied from the key object handle into
 * the operation.
 * This function works like TEE_SetOperationKey except that two
 * keys are set instead of a single key.
 *
 * @param[in]  operation     A pointer to a TEE_OperationHandle
 *                            allocated by TEE_AllocateOperation
 * @param[in]  key1           Initialized key 1
 * @param[in]  key2           Initialized key 2
 *
 * @return  TEE_SUCCESS                        Key associated to
 *                                             operation
 *          TEE_ERROR_CORRUPT_OBJECT           If key1 object is corrupt.
 *                                             Object handle is closed.
 *          TEE_ERROR_CORRUPT_OBJECT_2         If key2 object is corrupt.
 *                                             Object handle is closed.
 *          TEE_ERROR_STORAGE_NOT_AVAILABLE    Key1 object is stored
 *                                             in inaccessible storage area
 *          TEE_ERROR_STORAGE_NOT_AVAILABLE_2  Key2 object is stored
 *                                             in inaccessible storage area
 */
extern TEE_Result TEE_SetOperationKey2(
  TEE_OperationHandle operation,
  TEE_ObjectHandle key1,
  TEE_ObjectHandle key2
);

extern void TEE_CopyOperation(
  TEE_OperationHandle dstOperation,
  TEE_OperationHandle srcOperation
);

/**
 * @brief accumulates message data for hashing
 *
 * The message does not have to be block aligned.
 * Subsequent calls to this function are possible.
 *
 * @param[in]  operation     Handle of a running Message Digest operation
 * @param[in]  chunk         Chunk of data to be hashed
 * @param[in]  chunkSize     size of chunk in bytes
 *
 */

extern void TEE_DigestUpdate(
  TEE_OperationHandle operation,
  void* chunk,
  size_t chunkSize
);

/**
 * @brief finalizes the message digest operation
 *
 * produces the message hash. the Message Digest
 * operation can be reset and reused.
 *
 * @param[in]  operation     Handle of a running Message Digest operation
 * @param[in]  chunk         Chunk of data to be hashed
 * @param[in]  chunkSize     size of chunk in bytes
 * @param[out] hash          Output buffer filled with the
 *                           message hash
 * @param[in,out] hashLen    Length of hash buffer passed in
 *                           bytes, updated with length of hash
 *                           result.
 *
 * @return   TEE_SUCCESS                Hash final op done
 *           TEE_ERROR_SHORT_BUFFER     Output buffer too small
 */

TEE_Result TEE_DigestDoFinal(
  TEE_OperationHandle operation,
  void* chunk, size_t chunkLen,
  void* hash, size_t *hashLen
);

/**
 * @brief starts the symmetric cipher operation
 *
 * The operation must have been associated with a key.
 *
 * @param[in]  operation     A handle on an opened cipher
 *                           operation setup with a key
 * @param[in]  IV            Buffer containing the operation
 *                           Initialization Vector
 * @param[in]  IVLen         Size of IV in bytes
 *
 */
extern void TEE_CipherInit(
  TEE_OperationHandle operation,
  void* IV,
  size_t IVLen
);

/**
 * @brief function encrypts or decrypts input data
 *
 * Subsequent calls to this function are possible. The cipher
 * operation is finalized with a call to TEE_CipherDoFinal.
 *
 * @param[in]  operation     Handle of a running Cipher operation
 * @param[in]  srcData       Input data buffer to be encrypted
 *                           or decrypted operation
 * @param[in]  srcLen        length of srcData in bytes
 * @param[out] destData      Output buffer
 * @param[in,out]  destLen   length of destData in bytes
 *
 * @return   TEE_SUCCESS                Cipher update done
 *           TEE_ERROR_SHORT_BUFFER     Output buffer not large enough to
 *                                      contain output.
 *                                      Input not fed into algorithm.
 */

extern TEE_Result TEE_CipherUpdate(
  TEE_OperationHandle operation,
  void* srcData,
  size_t srcLen,
  void* destData,
  size_t *destLen
);

/**
 * @brief finalizes the cipher operation
 *
 * Input data does not have to be a multiple of block size for
 * TEE_ALG_AES_CTS and TEE_ALG_AES_CTR. The cipher operation is
 * finalized with this call.
 *
 * @param[in]  operation     Handle of a running Cipher operation
 * @param[in]  srcData       Input data buffer to be encrypted
 *                           or decrypted operation
 * @param[in]  srcLen        length of srcData in bytes
 * @param[out] destData      Output buffer
 * @param[in,out]  destLen   length of destData in bytes
 *
 * @return   TEE_SUCCESS                Cipher final done
 *           TEE_ERROR_SHORT_BUFFER     Output buffer not large enough to
 *                                      contain output.
 *
 */

extern TEE_Result TEE_CipherDoFinal(
  TEE_OperationHandle operation,
  void* srcData,
  size_t srcLen,
  void* destData,
  size_t *destLen
);

/**
 * MAC Functions
 **/

/**
 * @brief initializes a MAC operation
 *
 * The operation is started and associated with a key. If this
 * function does not return with TEE_SUCCESS, the operation is
 * not initialized.
 * If the MAC algorithm does not require an IV, the parameters
 * IV, IVLen are ignored. IV must still be a valid input buffer,
 * so, for example, if IV is NULL, then IVLen must be set to 0.
 *
 * @param[in]  operation     Operation handle
 * @param[in]  IV            Input buffer containing the
 *                           operation IV, if applicable
 * @param[in]  IVLen         Length of IV in bytes
 *
 */

extern void TEE_MACInit(
  TEE_OperationHandle operation,
  void* IV,
  size_t IVLen
);

/**
 * @brief accumulates data for a MAC calculation
 *
 * Input data does not have to be a multiple of the block size.
 * Subsequent calls to this function are possible.
 * TEE_MACComputeFinal or TEE_MACCompareFinal are called to
 * complete the MAC operation.
 *
 * @param[in]  operation     Handle of a MAC operation
 * @param[in]  chunk         Chunk of the message to be MACed
 * @param[in]  chunkSize     Size of chunk in bytes
 *
 * @note This operation not supported for :
 *          TEE_ALG_AES_CMAC
 *
 */

extern void TEE_MACUpdate(
  TEE_OperationHandle operation,
  void* chunk,
  size_t chunkSize
);

/**
 * @brief finalizes the MAC operation with a last chunk of
 *        message, and computes the MAC
 *
 * finalizes the MAC operation with a last chunk of message, and
 * computes the MAC.
 *
 * @param[in]  operation     Handle of a MAC operation
 * @param[in]  message       Chunk of the message to be MACed
 * @param[in]  messageLen    Size of chunk in bytes
 * @param[out] mac           Output buffer filled with the
 *                           computed MAC
 * @param[in,out]  macLen    Length of buffer passed, updated
 *                           with correct length after function
 *                           returns, size in bytes
 *
 *
 * @return   TEE_SUCCESS                Mac final done
 *           TEE_ERROR_SHORT_BUFFER     Output buffer not large enough to
 *                                      contain output.
 */

extern TEE_Result TEE_MACComputeFinal(
  TEE_OperationHandle operation,
  void* message,
  size_t messageLen,
  void* mac,
  size_t *macLen
);

/**
 * @brief finalizes the MAC operation and compares the MAC with
 *        the buffer passed to the function
 *
 * finalizes the MAC operation and compares the MAC with the
 * buffer passed to the function.
 *
 * @param[in]  operation     Handle of a MAC operation
 * @param[in]  message       Input buffer containing the last
 *                           message chunk to MAC
 * @param[in]  messageLen    Size of message in bytes
 * @param[in]  mac           Input buffer containing the MAC to check
 * @param[in]  macLen        Length of buffer passed, size in
 *                           bytes
 *
 * @return   TEE_SUCCESS                Mac initialized
 *           TEE_ERROR_GENERIC          Hardware/Software
 *                                      failure
 *           TEE_ERROR_MAC_INVALID      computed MAC does not
 *                                      correspond to the value
 *                                      passed in the parameter
 *                                      mac
 */

extern TEE_Result TEE_MACCompareFinal(
  TEE_OperationHandle operation,
  void* message,
  size_t messageLen,
  void* mac,
  size_t macLen
);

/**
 * AUTHENTICATED ENCRYPTION API's
 **/

/**
 * @brief initializes an Authentication Encryption operation
 *
 * @param[in]  operation     A handle on the operation
 * @param[in]  nonce         The operation nonce or IV
 * @param[in]  nonceLen      operation nonce or IV in bytes
 * @param[in]  tagLen        Size in bits of the tag length
 * @param[in]  AADLen        Length in bytes of the AAD
 *                             o Used only for AES-CCM. Ignored for AES-GCM
 * @param[in]  payloadLen    Used only for AES-CCM. Ignored for AES-GCM.bits
 *
 * @return   TEE_SUCCESS                AE intialize done
 *           TEE_ERROR_NOT_SUPPORTED    Tag length no supported
 *
 */

extern TEE_Result TEE_AEInit(
  TEE_OperationHandle operation,
  void* nonce,
  size_t nonceLen,
  uint32_t tagLen,
  uint32_t AADLen,
  uint32_t payloadLen
);

/**
 * @brief Feeds AAD chunk to the AE operation
 *
 * @param[in]  operation     Handle on the AE operation
 * @param[in]  AADdata       Input buffer containing the chunk
 *                           of AAD
 * @param[in]  AADdataLen    Length in bytes of the AAD
 *
 */

extern void TEE_AEUpdateAAD(
  TEE_OperationHandle operation,
  void* AADdata,
  size_t AADdataLen
);

/**
 * @brief accumulates data for an Authentication Encryption
 *        operation
 *
 * Subsequent calls to this function are possible.
 *
 * @param[in]  operation     Handle of a running AE operation
 * @param[in]  srcData       Input data buffer to be encrypted
 *                           or decrypted
 * @param[in]  srcLen        length of srcData in bytes
 * @param[out] destData      Output buffer
 * @param[in,out]  destLen   length of destData in bytes
 *
 * @return   TEE_SUCCESS              AE Updated
 *           TEE_ERROR_SHORT_BUFFER   Output buffer not large enough
 *
 */

extern TEE_Result TEE_AEUpdate(
  TEE_OperationHandle operation,
  void* srcData,
  size_t srcLen,
  void* destData,
  size_t *destLen
);

/**
 * @brief completes the AE operation and computes the tag
 *
 * @param[in]  operation     Handle of a running AE operation
 * @param[in]  srcData       Reference to final chunk of input
 *                           data to be encrypted
 * @param[in]  srcLen        Length in bytes of the srcData
 * @param[out] destData      Output buffer. Can be omitted if
 *                           the output is to be discarded,
 *                           e.g., because it is known to be
 *                           empty.
 * @param[in]  destLen       Length in bytes of the destData
 * @param[out] tag           Output buffer filled with the computed tag
 * @param[in]  tagLen        Length in bytes of the tag
 *
 * @return   TEE_SUCCESS              AE final done
 *           TEE_ERROR_SHORT_BUFFER   Output buffer not large enough
 *
 */

extern TEE_Result TEE_AEEncryptFinal(
  TEE_OperationHandle operation,
  void* srcData,
  size_t srcLen,
  void* destData,
  size_t* destLen,
  void* tag,
  size_t* tagLen
);

/**
 * @brief completes the AE operation and verifies the tag
 *
 * @param[in]  operation     Handle of a running AE operation
 * @param[in]  srcData       Reference to final chunk of input
 *                           data to be encrypted
 * @param[in]  srcLen        Length in bytes of the srcData
 * @param[out]  destData      Output buffer. Can be omitted if
 *                           the output is to be discarded,
 *                           e.g., because it is known to be
 *                           empty.
 * @param[in]  destLen       Length in bytes of the destData
 * @param[out]  tag          Input buffer filled with the
 *                           computed tag
 * @param[in]  tagLen        Length in bytes of the tag
 *
 * @return   TEE_SUCCESS              AE final done
 *           TEE_ERROR_SHORT_BUFFER   Output buffer not large enough
 *           TEE_ERROR_MAC_INVALID    Computed tag does not match input tag
 *
 */

extern TEE_Result TEE_AEDecryptFinal(
  TEE_OperationHandle operation,
  void* srcData,
  size_t srcLen,
  void* destData,
  size_t *destLen,
  void* tag,
  size_t tagLen
);

/**
 * Aysmmetric Cipher APIs
 **/

/**
 * @brief encrypts a message within an asymmetric operation
 *
 * These functions can be called only with an operation of the following algorithms:
 *  TEE_ALG_RSAES_PKCS1_V1_5
 *  TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1
 *  TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA224
 *  TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA256
 *  TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA384
 *  TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA512
 *  TEE_ALG_RSA_NOPAD (expects input buffer and output buffer of same size)
 *
 * The parameters params, paramCount contain the operation parameters
 *
 * Algorithm                               Possible operation parameters
 * TEE_ALG_RSAES_PKCS1_OAEP_MGF1_XXX       TEE_ATTR_RSA_OAEP_LABEL: This parameter is optional.
 *                                         If not present, an empty label is assumed.
 *
 *
 * @param[in]  operation     Handle on the operation, which must have been suitably
 *                           set up with an operation key
 * @param[in]  params        Optional operation parameters
 * @param[in]  paramCount    Optional operation parameters count
 * @param[in]  srcData       Reference to final chunk of input
 *                           data to be encrypted
 * @param[in]  srcLen        Length in bytes of the srcData
 * @param[out]  destData     Output buffer.
 * @param[in]  destLen       Length in bytes of the destData
 *
 * @return   TEE_SUCCESS                Asym  encryption done
 *           TEE_ERROR_BAD_PARAMETERS   Incorrect input
 *                                      parameters
 *           TEE_ERROR_SHORT_BUFFER     If the output buffer is
 *                                      not large enough to hold the result
 *
 */

extern TEE_Result TEE_AsymmetricEncrypt(
  TEE_OperationHandle operation,
  TEE_Attribute* params,
  uint32_t paramCount,
  void* srcData,
  size_t srcLen,
  void* destData,
  size_t *destLen
);

/**
 * @brief decrypts a message within an asymmetric operation
 *
 * These functions can be called only with an operation of the following algorithms:
 *  TEE_ALG_RSAES_PKCS1_V1_5
 *  TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1
 *  TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA224
 *  TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA256
 *  TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA384
 *  TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA512
 *  TEE_ALG_RSA_NOPAD (expects input buffer and output buffer of same size)
 *
 * The parameters params, paramCount contain the operation parameters
 *
 * Algorithm                               Possible operation parameters
 * TEE_ALG_RSAES_PKCS1_OAEP_MGF1_XXX       TEE_ATTR_RSA_OAEP_LABEL: This parameter is optional.
 *                                         If not present, an empty label is assumed.
 *
 *  All supported algo by this api         TEE_ATTR_RSA_KEY_CRT_DECRYPT:This parameter is optional.
 *                                         If not present, a
 *                                         regular asymmetric
 *                                         decryption is done
 *                                         else CRT decryption
 *                                         is done.
 *
 * @param[in]  operation     Handle on the operation, which must have been suitably
 *                           set up with an operation key
 *
 * @param[in]  params        Optional operation parameters
 * @param[in]  paramCount    Optional operation parameters count
 * @param[in]  srcData       Reference to final chunk of input
 *                           data to be encrypted
 * @param[in]  srcLen        Length in bytes of the srcData
 * @param[out] destData      Output buffer. Can be omitted if
 *                           the output is to be discarded,
 *                           e.g., because it is known to be
 *                           empty.
 * @param[in]  destLen       Length in bytes of the destData
 *
 * @return   TEE_SUCCESS                Asym  decryption done
 *           TEE_ERROR_BAD_PARAMETERS   Incorrect input
 *                                      parameters
 *           TEE_ERROR_SHORT_BUFFER     If the output buffer is
 *                                      not large enough to hold the result
 *
 */

extern TEE_Result TEE_AsymmetricDecrypt(
  TEE_OperationHandle operation,
  TEE_Attribute* params,
  uint32_t paramCount,
  void* srcData,
  size_t srcLen,
  void* destData,
  size_t *destLen
);

/**
 * @brief signs a message digest within an asymmetric operation
 *
 * Note that only an already hashed message can be signed.
 * This function can be called only with an operation of the
 * following algorithms:
 *  TEE_ALG_RSASSA_PKCS1_V1_5_MD5
 *  TEE_ALG_RSASSA_PKCS1_V1_5_SHA1
 *  TEE_ALG_RSASSA_PKCS1_V1_5_SHA224
 *  TEE_ALG_RSASSA_PKCS1_V1_5_SHA256
 *  TEE_ALG_RSASSA_PKCS1_V1_5_SHA384
 *  TEE_ALG_RSASSA_PKCS1_V1_5_SHA512
 *  TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1
 *  TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224
 *  TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256
 *  TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384
 *  TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512
 *  TEE_ALG_DSA_SHA1
 *  TEE_ALG_DSA_SHA224
 *  TEE_ALG_DSA_SHA256
 *  TEE_ALG_ECDSA_P192
 *  TEE_ALG_ECDSA_P224
 *  TEE_ALG_ECDSA_P256
 *  TEE_ALG_ECDSA_P384
 *  TEE_ALG_ECDSA_P512
 *
 * The parameters params, paramCount contain the operation
 * parameters
 *
 * Algorithm                      Possible operation parameters
 * TEE_ALG_RSASSA_PKCS1_PSS_MGF1_XX
 *                                      TEE_ATTR_RSA_PSS_SALT_LENGTH: Number of bytes in the salt.
 *                                      This parameter is
 *                                      optional. If not
 *                                      present, the salt length
 *                                      is equal to the hash
 *                                      length.
 *
 * @param[in]  operation     Handle on the operation, which must have been suitably
 *                           set up with an operation key
 * @param[in]  params        Optional operation parameters
 * @param[in]  paramCount    Optional operation parameters count
 * @param[in]  digest        Input buffer containing the input message digest
 * @param[in]  digestLen     length of message digest in bytes
 * @param[out] signature     Output buffer written with the signature of the digest
 * @param[in]  signatureLen  Length in bytes of the signature
 *                           buffer
 *
 * @return   TEE_SUCCESS                Signing done
 *           TEE_ERROR_SHORT_BUFFER     If the output buffer is
 *                                      not large enough to hold the result
 *
 */

extern TEE_Result TEE_AsymmetricSignDigest(
  TEE_OperationHandle operation,
  TEE_Attribute* params,
  uint32_t paramCount,
  void* digest,
  size_t digestLen,
  void* signature,
  size_t *signatureLen
);

/**
 * @brief verifies a message digest signature within an
 *        asymmetric operation
 *
 * Note that only an already hashed message can be signed.
 * This function can be called only with an operation of the
 * following algorithms:
 *  TEE_ALG_RSASSA_PKCS1_V1_5_MD5
 *  TEE_ALG_RSASSA_PKCS1_V1_5_SHA1
 *  TEE_ALG_RSASSA_PKCS1_V1_5_SHA224
 *  TEE_ALG_RSASSA_PKCS1_V1_5_SHA256
 *  TEE_ALG_RSASSA_PKCS1_V1_5_SHA384
 *  TEE_ALG_RSASSA_PKCS1_V1_5_SHA512
 *  TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1
 *  TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224
 *  TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256
 *  TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384
 *  TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512
 *  TEE_ALG_DSA_SHA1
 *  TEE_ALG_DSA_SHA224
 *  TEE_ALG_DSA_SHA256
 *  TEE_ALG_ECDSA_P192
 *  TEE_ALG_ECDSA_P224
 *  TEE_ALG_ECDSA_P256
 *  TEE_ALG_ECDSA_P384
 *  TEE_ALG_ECDSA_P512
 *
 * The parameters params, paramCount contain the operation
 * parameters
 *
 * Algorithm                      Possible operation parameters
 * TEE_ALG_RSASSA_PKCS1_PSS_MGF1_XXX
 *                                      TEE_ATTR_RSA_OAEP_LABEL:
 *                                         TEE_ATTR_RSA_PSS_SALT_LENGTH: Number of bytes in the salt.
 *                                         This parameter is
 *                                         optional. If not
 *                                         present, the salt
 *                                         length is equal to
 *                                         the hash length.
 *
 * @param[in]  operation     Handle on the operation, which must have been suitably
 *                           set up with an operation key
 * @param[in]  params        Optional operation parameters
 * @param[in]  paramCount    Optional operation parameters count
 * @param[in]  digest        Input buffer containing the input message digest
 * @param[in]  digestLen     length of message digest in bytes
 * @param[in]  signature     signature of the digest to be
 *                           verified
 * @param[in]  signatureLen  Length in bytes of the signature
 *                           buffer
 *
 * @return   TEE_SUCCESS                  Signature verified
 *           TEE_ERROR_SIGNATURE_INVALID  Singature is invalid
 *
 */

extern TEE_Result TEE_AsymmetricVerifyDigest(
  TEE_OperationHandle operation,
  TEE_Attribute* params,
  uint32_t paramCount,
  void* digest,
  size_t digestLen,
  void* signature,
  size_t signatureLen
);

extern void TEE_DeriveKey(
  TEE_OperationHandle operation,
  TEE_Attribute* params,
  uint32_t paramCount,
  TEE_ObjectHandle derivedKey
);

/**
 * @brief generates random data
 *
 * @param[out] randomBuffer     Reference to generated random data
 * @param[in]  randomBufferLen  Byte length of requested random data
 */

extern void TEE_GenerateRandom(
  void* randomBuffer,
  size_t randomBufferLen
);

/*------------------------------------------------------------------------------
               --- WARNING --- WARNING --- WARNING ---
               The following definitions are not part of the TEE Internal API
  ------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
      Trace Functions
  ------------------------------------------------------------------------------*/
//void _SLogTrace(
//               const char *message,
//               ... /* arguments */);
//void _SLogWarning(
//               const char *message,
//               ... /* arguments */);
//void _SLogError(
//               const char *message,
//               ... /* arguments */);

#ifdef TEE_TRACE_ALLOW_OVERLOAD

#define SLogTrace(xx_fmt, ...)    LOG_FUNC("[%s] " xx_fmt, __FUNCTION__, ##__VA_ARGS__)
#define SLogError(xx_fmt, ...)    LOG_FUNC("[%s] " xx_fmt, __FUNCTION__, ##__VA_ARGS__)

#else /* !defined(TEE_TRACE_ALLOW_OVERLOAD) */

#include "qsee_log.h"

#ifndef TEE_NO_TRACE

#define SLogTrace(xx_fmt, ...)    qsee_log(QSEE_LOG_MSG_DEBUG, "[%s] " xx_fmt, __FUNCTION__, ##__VA_ARGS__)
#define SLogError(xx_fmt, ...)    qsee_log(QSEE_LOG_MSG_FATAL, "[%s] " xx_fmt, __FUNCTION__, ##__VA_ARGS__)

#else /* defined(TEE_NO_TRACE) */

/* Note that the following code depends on the compiler's supporting variadic macros */
#define SLogTrace(...)   do {} while(0)
#define SLogError(...)   do {} while(0)

#endif /* !defined(TEE_NO_TRACE) */

#define GPLOG_ERROR(x) \
  qsee_log(QSEE_LOG_MSG_ERROR, "%s:%d retvalue=[0x%x]", __FUNCTION__, __LINE__, x)

#define GPLOG_ERROR2(s, x) \
  qsee_log(QSEE_LOG_MSG_ERROR, "%s:%d %s retvalue=[0x%x]", __FUNCTION__, __LINE__, s, x)

#define GPLOG_FATAL(x) \
  qsee_log(QSEE_LOG_MSG_FATAL, "%s:%d retvalue=[0x%x]", __FUNCTION__, __LINE__, x)

#define GPLOG_FATAL2(s, x) \
  qsee_log(QSEE_LOG_MSG_FATAL, "%s:%d %s retvalue=[0x%x]", __FUNCTION__, __LINE__, s, x)

#endif /* defined(TEE_TRACE_ALLOW_OVERLOAD) */

#ifndef S_VAR_NOT_USED
#define S_VAR_NOT_USED(variable) do{(void)(variable);}while(0);
#endif // S_VAR_NOT_USED

#endif /* __TEE_INTERNAL_API_H__ */
