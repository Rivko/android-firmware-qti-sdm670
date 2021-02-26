/*******************************************************************
--------------------------------------------------------------------
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
QSEEComApi sample client app
*******************************************************************/

#ifndef __TOE_KEY_GEN_API
#define __TOE_KEY_GEN_API

#define PACKED  __attribute((__packed__))

/* LTE OTA */

enum LteOtaCmdId {
  QSEECOM_FSM_LTE_INIT_DB = 1,
  QSEECOM_FSM_LTE_STORE_KENB,
  QSEECOM_FSM_LTE_GEN_KEYS,
  QSEECOM_FSM_LTE_GET_KEY_OFFSETS,
  QSEECOM_FSM_LTE_GEN_KENB_STARS,
  QSEECOM_FSM_LTE_GET_KENB_STARS,
  QSEECOM_FSM_LTE_STORE_NH,
  QSEECOM_FSM_LTE_DELETE_NH,
  QSEECOM_FSM_LTE_DELETE_KEYS,
};

/* This struct holds all the key offsets in IMEM.
   Offsets are positive for valid offset or -1 when key is NULL.
*/
typedef PACKED struct
{
    int32_t kenbRrcIntOffset;
    int32_t kenbRrcEncOffset;
    int32_t kenbUpEncOffset;
    int32_t kenbUpIntOffset;
} LteSecImemOffsets;

/* This struct holds the parameters needed to generated kenb*   */
typedef PACKED struct
{
    uint32_t pci;
    uint32_t earfcnDl;
    uint32_t useNH;
    uint32_t kenbStarIdx;
} LteKenbStarParams;

/* KDF Algorithm types  */
typedef enum
{ LTE_SEC_ALG_ID_NULL = 0,
    AES,
    SNOW3G,
    ZUC,
    NUM_KDF_ALGS
} LteKdfAlgTypes;

typedef PACKED struct
{
    uint8_t keyType;
    uint8_t algId;
} LteGenKeysParams;



/* IKE */

#ifndef __IKE_SEC_KEY_API
#define __IKE_SEC_KEY_API
#define FACT_SZ 256
#define MOD_SZ 512
#define PCOMP_SZ 512
#define SIG_SZ 512
#define PAD_SZ 12
#define KEY_SZ 0x510
#define CIPH_SZ 0x530


enum IkeCmdId{
  QSEECOM_FSM_IKE_CMD_SIGN = 1,
  QSEECOM_FSM_IKE_CMD_PROV_KEY = 2,
  QSEECOM_FSM_IKE_CMD_ENCRYPT_PRIVATE_KEY = 3
};


/* HLOS to send the RSA private key in this format*/
typedef struct{
    uint32_t nbits;   /* Number of bits in the modulus */
    char n[MOD_SZ];   /*  Modulus */
    char e[FACT_SZ];  /*  e Public exponent */
    char d[PCOMP_SZ]; /*  Private exponent */
    char pad[PAD_SZ]; /*  Padding */
} toeRsaKeyType;

/* RSA padding type. PKCS #1 v2.1 */
typedef enum{
    /*PKCS1 v1.5 signature*/
    CE_RSA_PAD_PKCS1_V1_5_SIG = 1,
    /*PKCS1 v1.5 encryption*/
    CE_RSA_PAD_PKCS1_V1_5_ENC = 2,  /* not supported */
    /*OAEP Encryption*/
    CE_RSA_PAD_PKCS1_OAEP = 3,      /* not supported */
    /*PSS Signature*/
    CE_RSA_PAD_PKCS1_PSS = 4,
    /* No Padding */
    CE_RSA_NO_PAD = 5,
} CE_RSA_PADDING_TYPE;


/* Request Structure- Encrypt Key */
typedef struct{
    /* buffer containing the key */
    toeRsaKeyType keybuf;
} toeIkeEncryptKeyCmdType;

/* Response Structure - Encrypt Key*/
typedef struct{
    /* Encrypt key result */
    int result;
    /* Encrypted key buffer */
    uint8_t enc_keybuf[CIPH_SZ];
} toeIkeRspEncryptKeyCmdType;

/* Request Structure- Prov Key */
typedef struct{
    /* buffer containing the encrypted key */
    uint8_t keybuf[CIPH_SZ];
} toeIkeProvKeyCmdType;

/* Response Structure - Prov Key*/
typedef struct{
    int result;
} toeIkeRspProvKeyCmdType;

/* Request Structure - Sign cmd */
typedef struct{
    /* Padding scheme to use for the signature:
     * PKCS1v1.5 or PKCS1 PSS*/
    CE_RSA_PADDING_TYPE rsa_pad;
    /* Salt length (only for PKCS1 PSS) */
    int saltlen;
    /* length of the hash */
    uint32_t buflen;
    /* PKC buffer to sign */
    uint8_t buf[SIG_SZ];
} toeIkeSignCmdType;

/* Response Structure - Sign cmd */
typedef struct{
    int result;
    uint32_t siglen;
    uint8_t buf[SIG_SZ];
} toeIkeRspSignCmdType;

typedef struct {
    int cmdId;
    union {
      toeIkeEncryptKeyCmdType c1;
      toeIkeProvKeyCmdType    c2;
      toeIkeSignCmdType       c3;
    } reqBuf;
} genericRequestCmdType;

#define K_RRC_ENC_ALG_P1 0x03
#define K_RRC_INT_ALG_P1 0x04
#define K_UP_ENC_ALG_P1  0x05
#define K_UP_INT_ALG_P1  0x06

/**************  API function declaration  *****************************/

/***********************************************
INPUT: ueIdx, kenb
OUTPUT: None
RETURN: SUCCESS/FAIL
SIDE EFFECT: stores kenb in IMEM for userIdx
*/
int lteStoreKenb(
    uint32_t ueIdx,
    uint8_t * kenb);

/***********************************************
INPUT: ueIdx, algId, keyType, useKenbStar
OUTPUT: None
RETURN: SUCCESS/FAIL
SIDE EFFECT: Generates keys and stores them in the offset locations in IMEM
*/
int lteGenerateKeys(
    uint32_t ueIdx,
    uint32_t keyType,
    LteKdfAlgTypes algId);

/***********************************************
   INPUT: ueIdx
   OUTPUT: keyOffsets
   RETURN: SUCCESS/FAIL
   SIDE EFFECT: populates key offsets from IMEM. An offset of -1 indicates NULL value for the key.
*/
int lteGetKeyOffsets(
    uint32_t ueIdx,
    LteSecImemOffsets * keyOffsets);

/***********************************************
   INPUT: ueIdx, ncc, params, cell_idx
   OUTPUT: None
   RETURN: SUCCESS/FAIL
   SIDE EFFECT: Generates and stores kenb* in IMEM. If useNH =1 it uses the stored NH to calculate kenb*

*/
int lteGenerateKenbStar(
    uint32_t ueIdx,
    const LteKenbStarParams * params);

/***********************************************
INPUT: ueIdx. kenbStarIdx
OUTPUT: kenbStar
RETURN: SUCCESS/FAIL
SIDE EFFECT: populates kenbStar from IMEM for ueIdx and cell kenbStarIdx.
*/
int lteGetKenbStar(
    uint32_t ueIdx,
    uint8_t * kenbStar);

/***********************************************
INPUT: ueIdx, nh
OUTPUT: None
RETURN: SUCCESS/FAIL
SIDE EFFECT: stores NH in IMEM for userIdx
*/
int lteStoreNh(
    uint32_t ueIdx,
    uint8_t * nh);

/***********************************************
INPUT: ueIdx
OUTPUT: None
RETURN: SUCCESS/FAIL
SIDE EFFECT: Deletes NH in IMEM for userIdx.
*/
int lteDeleteNh(
    uint32_t ueIdx);

/***********************************************
   INPUT: ueIdx
   OUTPUT: None
   RETURN:SUCCESS/FAIL
  SIDE EFFECT: Deletes all keys from IMEM for useIdx.
*/
int lteDeleteKeys(
    uint32_t ueIdx);


int32_t toeIkeProvKey(toeIkeProvKeyCmdType* req,
                toeIkeRspProvKeyCmdType* rsp);

int32_t toeIkeSign(toeIkeSignCmdType* req,
            toeIkeRspSignCmdType* rsp);

int32_t toeIkeEncryptKey(toeIkeEncryptKeyCmdType* req,
            toeIkeRspEncryptKeyCmdType* rsp);

#endif
#endif  /*  */
