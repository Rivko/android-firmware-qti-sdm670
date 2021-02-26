/**
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <stdint.h>

#ifdef __RVCT__
#pragma anon_unions
#endif

#define QC_USERNAME_MAX_LEN 128 /**< In line with FIDO UAFV1 */
#define QC_SEC_APP_NAME_LEN 32 /**< As in lkthread.h */
#define QC_APP_ID_MAX_LEN   512 /**< In line with FIDO UAFV1 */
#define QC_AES_IV_SIZE      16
#define QC_AUTH_NONCE       32
#define QC_AUTH_ID_SIZE     32
#define QC_EXTENSION_DATA_MAX_LEN 4096
#define QC_UVT_MAX_LEN      (5 * 1024) // 5k
#define QC_UVT_VERSION_MAJOR    1
#define QC_UVT_VERSION_MINOR   0


#pragma pack(push, auth, 1)

typedef enum {
  QC_AUTH_CODE_OK  = 0, /**< Authentication was ok */
  QC_AUTH_CODE_ERR = 1, /**< Generic error */
  QC_AUTH_CODE_CANCEL = 2, /**< Cancelled by user */
  QC_AUTH_CODE_MAX = QC_AUTH_CODE_CANCEL,
  QC_AUTH_CODE_NA = 0x7FFFFFFF
} qc_auth_code_t;

/** Authentication token
*/
typedef struct {
  struct {
    uint16_t major;
    uint16_t minor;
  } version;
  qc_auth_code_t result;
  uint64_t userId;
  uint64_t uptime;
  uint8_t nonce[QC_AUTH_NONCE]; /**< Sha256(finalChallenge) for FIDO */
  uint32_t extDataLength;
  uint8_t extData[QC_EXTENSION_DATA_MAX_LEN];
} qc_authentication_token_t;

typedef struct {
  uint64_t messageLen;
  union {
    /** Secure UVT */
    uint8_t message[QC_UVT_MAX_LEN];
    /** Non-secure UVT */
    struct {
      char authName[QC_SEC_APP_NAME_LEN]; /**< Non-secure authenticator name, \0 terminated */
      uint8_t authId[QC_AUTH_ID_SIZE]; /**< Non-secure authenticator ID */
      qc_authentication_token_t token;
    };
  };
} qc_user_verification_token_t;

#pragma pack(pop, auth)
