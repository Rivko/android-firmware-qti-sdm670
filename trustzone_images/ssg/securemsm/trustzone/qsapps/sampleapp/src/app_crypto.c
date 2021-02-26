/*
  @file app_crypto.c
  @brief This file is a sample code that show how QSEE crypto API's can be used.

*/
/*===========================================================================
Copyright (c) {2011, 2016-2020} Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

#include "IxErrno.h"
#include "qsee_cipher.h"
#include "qsee_hash.h"
#include "qsee_hmac.h"
#include "qsee_cmac.h"
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_timer.h"
#include "qsee_ecc.h"
#include "qsee_kdf.h"
#include "qsee_uf_aes.h"
#include "qsee_uf_des.h"
#include "qsee_uf_sha.h"
#include "qsee_clk.h"

#include "comdef.h"
#include "crypto_cipher_test_vectors.h"
#include "crypto_ecc_test_vectors.h"
#include "crypto_hash_test_vectors.h"
#include "crypto_hmac_test_vectors.h"
#include "crypto_cmac_test_vectors.h"
#include "ICrypto.h"
#include "CCrypto.h"
#include "qsee_env.h"
#include <stringl.h>
#include "CCipher.h"
#include "ICipher.h"

#define ECC_TEST_COUNT 10
#define TEST_CRYPTO_PERF_MAX_ITERATIONS      100
#define TEST_CRYPTO_GCM_PERF_MAX_ITERATIONS  2
#define TZBSP_TEST_CRYPTO_PERF_SIZE_8K       8*1024
#define TZBSP_TEST_CRYPTO_PERF_SIZE_32K      32*1024

#define TZBSP_TEST_CRYPTO_PERF_START_SIZE TZBSP_TEST_CRYPTO_PERF_SIZE_8K
#define TZBSP_TEST_CRYPTO_PERF_END_SIZE   TZBSP_TEST_CRYPTO_PERF_SIZE_32K

#define ECC_MAX_MODULUS_BITS (521 + 8)
#define _ECC_BIGNUM_WORDS ((ECC_MAX_MODULUS_BITS + 24 + 31) / 32)
#define QRLBN_ECC_MODULUS_CHECK_FAIL(modulus_struct_p)                  \
   ((modulus_struct_p) == NULL ||                                       \
    (modulus_struct_p)->md.modulus != (modulus_struct_p)->modulus_storage.data || \
    (modulus_struct_p)->md.montgomery_R != (modulus_struct_p)->montgomery_R_storage.data || \
    (modulus_struct_p)->md.montgomery_R2 != (modulus_struct_p)->montgomery_R2_storage.data || \
    (modulus_struct_p)->md.allocated_words != _ECC_BIGNUM_WORDS)

#define CRYPTO_TEST_GUARD_ELSE_EXIT(c)                                \
  if (!(c))                                                           \
  {                                                                   \
    qsee_log(QSEE_LOG_MSG_DEBUG, "%s %d", __func__,  __LINE__);       \
    ret = -E_FAILURE;                                                 \
    goto Exit;                                                        \
  }

extern int timesafe_memcmp(const void* ptr1, const void* ptr2, size_t len);

static int app_crypto_timesafe_memcmp(const void* ptr1, const void* ptr2, size_t len)
{
  if (!ptr1 || !ptr2)
    return 1;
  else
    return timesafe_memcmp(ptr1, ptr2, len);
}
static uint8 hash_binary[40];

/**
   @brief
   Sample code for how to call QSEE AES crypto API's
*/
int tz_app_crypto_aes_func(QSEE_CIPHER_ALGO_ET alg,
                           QSEE_CIPHER_MODE_ET mode,
                           uint8 *pt,
                           uint32 pt_len,
                           uint8 *key,
                           uint32 key_len,
                           uint8 *iv,
                           uint8 *ct,
                           uint8 verify)
{
   uint32 ct_len = pt_len;
   qsee_cipher_ctx *ctx = 0;
   uint8 *ct_tmp = 0;
   uint8 *pt_tmp = 0;
   int status = E_SUCCESS;
   QSEE_CIPHER_MODE_ET mode2;
   uint32 mode2_len = sizeof(mode2);
   char name[] = "CE_TEST";
   QSEE_RESOURCE ce_clk = CE1;
   QSEE_CLKLEV voteon_freq = MEDIUM;
   QSEE_CLKLEV voteoff_freq = INACTIVE;

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if (qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------
     Allocate temp ct buffer
     ------------------------------------------------------------------------*/
   if (E_SUCCESS == status && (verify == 1) &&
       (ct_tmp = qsee_malloc(pt_len)) == NULL)
   {
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Clear temp buffer
     ------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1))
   {
      memset(ct_tmp, 0, pt_len);
   }

   /*-----------------------------------------------------------------------
     If verify is 0,  it means we are not using the predefined test case and
     cannot verify the encrypted packet
     -------------------------------------------------------------------------*/
   if (verify == 0)
      ct_tmp = ct;

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_encrypt(ctx, pt, pt_len, ct_tmp, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_encrypt API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------------
     If NULL key pointer then we are using HW key so don't compare encrypted result
     -----------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1) && (NULL != key) &&
      0 != app_crypto_timesafe_memcmp(ct, ct_tmp, pt_len))
   {
      status = -E_FAILURE;
   }

   /*---------------------------------------------------------------------------
     Reset cipher context (Optional).
     ----------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_reset(ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_reset API failed");
      status = -E_FAILURE;
   }
   /*------------------------------------------------------------------------------
     Allocate temp pt buffer
     --------------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1) &&
       (pt_tmp = qsee_malloc(pt_len)) == NULL)
   {
      status = -E_FAILURE;
   }

   if ((E_SUCCESS == status)&& (verify == 1))
   {
      memset(pt_tmp, 0, pt_len);
   }

   /*-----------------------------------------------------------------------
     If verify is 0,  it means we are not using the predefined test case and
     cannot verify the encrypted packet
     -------------------------------------------------------------------------*/
   if (verify == 0)
      pt_tmp = pt;

   /*--------------------------------------------------------------------------------
     We must set parameters again so we can do the decrypt
     ---------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     Set IV if not NULL
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     vote for crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/
   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-on");
   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteon_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-on failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     Now decrypt the data
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_decrypt(ctx, ct_tmp, pt_len, pt_tmp, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_decrypt API failed");
      status = -E_FAILURE;
   }


   /*----------------------------------------------------------------------------------
     vote off crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/
   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-off");
   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteoff_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-off failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------------
     Now compare decrypted results
     ---------------------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1) &&
      0 != app_crypto_timesafe_memcmp(pt, pt_tmp, pt_len))
  {  
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------------
     Now read back Cipher Mode parameter
     ---------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_get_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode2, &mode2_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_get_parm API failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   /*--------------------------------------------------------------------------------
     Free malloc data
     ---------------------------------------------------------------------------------*/
   if ((ct_tmp) && (verify == 1))
   {
      qsee_free(ct_tmp);
      ct_tmp = 0;
   }

   if ((pt_tmp)&& (verify == 1))
   {
      qsee_free(pt_tmp);
      pt_tmp = 0;
   }

   return status;
}

/**
   @brief
   Test for setting AES CTR IV SIZE
*/
int tz_app_crypto_aes_ctr_sz(QSEE_CIPHER_ALGO_ET alg,
                           QSEE_CIPHER_MODE_ET mode,
                           uint8_t *pt,
                           uint32_t pt_len,
                           uint8_t *key,
                           uint32_t key_len,
                           uint8_t *iv,
                           uint8_t *ct,
                           uint8_t verify)
{
   Object ctx = Object_NULL;
   uint8_t *pt1 = NULL;
   uint8_t *pt2 = NULL;
   uint8_t *ct1 = NULL;
   uint8_t *ct2 = NULL;
   uint32_t pt1_len = 32;
   uint32_t pt2_len = 48;
   int i;
   int ret = E_SUCCESS;
   uint8_t iv1[QSEE_AES128_IV_SIZE] = {0};
   uint8_t iv_exp[QSEE_AES128_IV_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2};
   size_t outlen = 0;

   if (mode != QSEE_CIPHER_MODE_CTR) {
      qsee_log(QSEE_LOG_MSG_ERROR,"Invalid mode. Test only applicable to CTR mode");
      goto Exit;
   }

   // Allocate and set pt1 and pt2 buffers
   if ((pt1 = qsee_malloc(pt1_len)) != NULL
      && (ct1 = qsee_malloc(pt1_len)) != NULL
      && (pt2 = qsee_malloc(pt2_len)) != NULL
      && (ct2 = qsee_malloc(pt2_len)) != NULL )
   {
      memset(pt1, 1, pt1_len);
      memset(pt2, 0, 16);
      memset(pt2+16, 1, pt1_len);
   }
   else {
      qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_aes_ctr_sz() qsee_malloc failed");
      ret = -E_FAILURE;
      goto Exit;
   }

   if (alg == QSEE_CIPHER_ALGO_AES_128) {
    CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(qsee_open(CCipherAES128_UID, &ctx)));
   }
   else if (alg == QSEE_CIPHER_ALGO_AES_256) {
    CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(qsee_open(CCipherAES256_UID, &ctx)));
   }
   else
    goto Exit;

   for (uint32_t ctr_sz = 32; ctr_sz <=128; ctr_sz+=16) {
     // Set IV to 00
     memset(iv1, 0, 16);
     memset(ct1, 0, pt1_len);
     outlen = pt1_len;
     CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(ICipher_setParamAsU32(ctx, ICipher_PARAM_MODE, ICipher_MODE_CTR)));
     CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(ICipher_setParamAsData(ctx, ICipher_PARAM_KEY, key, key_len)));
     // Set iv1
     CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(ICipher_setParamAsData(ctx, ICipher_PARAM_IV, iv1, QSEE_AES128_IV_SIZE)));
     // Set Counter size
     CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(ICipher_setParamAsData(ctx, ICipher_PARAM_CTR_SZ, &ctr_sz, sizeof(ctr_sz))));
     qsee_log(QSEE_LOG_MSG_ERROR,"CTR_SZ set: %d",ctr_sz);
     // Perform cipher for 2 blocks data
     CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(ICipher_encrypt(ctx, pt1, pt1_len, ct1, outlen, &outlen)));
     CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(ICipher_getParamAsData(ctx, ICipher_PARAM_IV, &iv1, QSEE_AES128_IV_SIZE, &outlen)));
     CRYPTO_TEST_GUARD_ELSE_EXIT(app_crypto_timesafe_memcmp(iv1, iv_exp, QSEE_AES128_IV_SIZE) == 0);

     // Set IV to max
     memset(iv1+(QSEE_AES128_IV_SIZE - ctr_sz/8), 0xFF, ctr_sz/8);
     memset(ct2, 0, pt2_len);
     outlen = pt2_len;
     CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(ICipher_setParamAsData(ctx, ICipher_PARAM_IV, iv1, QSEE_AES128_IV_SIZE)));
     CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(ICipher_setParamAsData(ctx, ICipher_PARAM_CTR_SZ, &ctr_sz, sizeof(ctr_sz))));
     // Perform cipher for 3 blocks data
     CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(ICipher_encrypt(ctx, pt2, pt2_len, ct2, outlen, &outlen)));
     CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(ICipher_getParamAsData(ctx, ICipher_PARAM_IV, &iv1, QSEE_AES128_IV_SIZE, &outlen)));
     CRYPTO_TEST_GUARD_ELSE_EXIT(app_crypto_timesafe_memcmp(iv1, iv_exp, QSEE_AES128_IV_SIZE) == 0);

     CRYPTO_TEST_GUARD_ELSE_EXIT(Object_isOK(ICipher_reset(ctx)));
   }
Exit:
   Object_ASSIGN_NULL(ctx);
   // Free malloc data
   if (pt1)  qsee_free(pt1);
   if (pt2)  qsee_free(pt2);
   if (ct1)  qsee_free(ct1);
   if (ct2)  qsee_free(ct2);

   return ret;
}

/**
   @brief
   Sample code for how to call QSEE Triple DES crypto API's
*/
int tz_app_crypto_triple_des_func(QSEE_CIPHER_ALGO_ET alg,
                                  QSEE_CIPHER_MODE_ET mode,
                                  uint8 *pt,
                                  uint32 pt_len,
                                  uint8 *key,
                                  uint8 *iv,
                                  uint8 *ct,
                                  uint8 verify)
{
   uint32 ct_len = pt_len;
   qsee_cipher_ctx *ctx = 0;
   uint8 *ct_tmp = 0;
   uint8 *pt_tmp = 0;
   int status = E_SUCCESS;

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if (qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, QSEE_TRIPLE_DES_KEY_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set Cipher mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_TRIPLE_DES_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------
     Allocate temp ct buffer
     ------------------------------------------------------------------------*/
   if (E_SUCCESS == status && (verify == 1) &&
       (ct_tmp = qsee_malloc(pt_len)) == NULL)
   {
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Clear temp buffer
     ------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1))
   {
      memset(ct_tmp, 0, pt_len);
   }

   /*-----------------------------------------------------------------------
     If verify is 0,  it means we are not using the predefined test case and
     cannot verify the encrypted packet
     -------------------------------------------------------------------------*/
   if (verify == 0)
      ct_tmp = ct;

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_encrypt(ctx, pt, pt_len, ct_tmp, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_encrypt API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------------
     If NULL key pointer then we are using HW key so don't compare encrypted result
     -----------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1) && (NULL != key) &&
      0 != app_crypto_timesafe_memcmp(ct, ct_tmp, pt_len))
   {
      status = -E_FAILURE;
   }

   /*------------------------------------------------------------------------------
     Allocate temp pt buffer
     --------------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1) &&
       (pt_tmp = qsee_malloc(pt_len)) == NULL)
   {
      status = -E_FAILURE;
   }

   if ((E_SUCCESS == status)&& (verify == 1))
   {
      memset(pt_tmp, 0, pt_len);
   }

   /*-----------------------------------------------------------------------
     If verify is 0,  it means we are not using the predefined test case and
     cannot verify the encrypted packet
     -------------------------------------------------------------------------*/
   if (verify == 0)
      pt_tmp = pt;

   /*--------------------------------------------------------------------------------
     We must set parameters again so we can do the decrypt
     ---------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, QSEE_TRIPLE_DES_KEY_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     Set IV if not NULL
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_TRIPLE_DES_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     Now decrypt the data
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_decrypt(ctx, ct_tmp, pt_len, pt_tmp, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_decrypt API failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------------
     Now compare decrypted results
     ---------------------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1) &&
      0 != app_crypto_timesafe_memcmp(pt, pt_tmp, pt_len))
  {  
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   /*--------------------------------------------------------------------------------
     Free malloc data
     ---------------------------------------------------------------------------------*/
   if ((ct_tmp) && (verify == 1))
   {
      qsee_free(ct_tmp);
      ct_tmp = 0;
   }

   if ((pt_tmp)&& (verify == 1))
   {
      qsee_free(pt_tmp);
      pt_tmp = 0;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE Triple DES crypto API's
*/
int tz_app_crypto_triple_des_large_func(QSEE_CIPHER_ALGO_ET alg,
                                        QSEE_CIPHER_MODE_ET mode,
                                        uint8 *pt,
                                        uint32 pt_len,
                                        uint8 *key,
                                        uint8 *iv,
                                        uint8 *ct,
                                        uint8 *ct2)
{
   uint32 ct_len = pt_len;
   qsee_cipher_ctx *ctx = 0;
   int status = E_SUCCESS;
   CryptoCntxHandle *cntx = NULL;
   IovecListType   ioVecIn;
   IovecListType   ioVecOut;
   IovecType       IovecIn;
   IovecType       IovecOut;
   SW_CipherDESEncryptDir sw_dir_des  = SW_CIPHER_DES_ENCRYPT;
   SW_CipherDESModeType   sw_mode = SW_CIPHER_DES_MODE_CBC;

   //Set SW mode
   if (mode == QSEE_CIPHER_MODE_ECB)
   {
      sw_mode = SW_CIPHER_DES_MODE_ECB;
   }

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if (qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, QSEE_TRIPLE_DES_KEY_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set Cipher mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_TRIPLE_DES_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_encrypt(ctx, pt, pt_len, ct, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_encrypt API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now encrypt the data using SW algo
     -------------------------------------------------------------------------*/

   //Now perform encryption using SW
   if(E_SUCCESS == status && qsee_SW_Cipher_DES_Init(&cntx, SW_CIPHER_ALG_DES3) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DES_Init API failed");
      status = -E_FAILURE;
   }

   //Set parameters so we can do the encrypt
   if(E_SUCCESS == status &&
      qsee_SW_Cipher_DES_SetParam(cntx, SW_CIPHER_DES_PARAM_DIRECTION, &sw_dir_des, sizeof(SW_CipherDESEncryptDir)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DES_SetParm API failed");
      status = -E_FAILURE;
   }

   if(E_SUCCESS == status &&
      qsee_SW_Cipher_DES_SetParam(cntx, SW_CIPHER_DES_PARAM_MODE, &sw_mode, sizeof(SW_CipherDESModeType)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DES_SetParm API API failed");
      status = -E_FAILURE;
   }

   if(E_SUCCESS == status &&
      qsee_SW_Cipher_DES_SetParam(cntx, SW_CIPHER_DES_PARAM_KEY, key, SW_3DES_KEY_BYTE_LEN) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DES_SetParm API API failed");
      status = -E_FAILURE;
   }

   //Check for NULL IV pointer
   if (iv != NULL)
   {
      if(E_SUCCESS == status &&
         qsee_SW_Cipher_DES_SetParam(cntx, SW_CIPHER_DES_PARAM_IV, iv, SW_DES_IV_SIZE) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DES_SetParm API API failed");
         status = -E_FAILURE;
      }
   }

   /* Input IOVEC */
   ioVecIn.size = 1;
   ioVecIn.iov = &IovecIn;
   ioVecIn.iov[0].dwLen  = pt_len;
   ioVecIn.iov[0].pvBase = pt;

   /* Output IOVEC */
   ioVecOut.size = 1;
   ioVecOut.iov = &IovecOut;
   ioVecOut.iov[0].dwLen  = pt_len;
   ioVecOut.iov[0].pvBase = ct2;

   //Now encrypt the data
   if(E_SUCCESS == status && qsee_SW_CipherDESData(cntx, ioVecIn, &ioVecOut) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_CipherDESData API failed");
      status = -E_FAILURE;
   }

   if(E_SUCCESS == status && qsee_SW_Cipher_DES_DeInit(&cntx) !=0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DES_DeInit API failed");
      status = -E_FAILURE;
   }

   //Now compare encrypted results from SW and HW
  if (E_SUCCESS == status && app_crypto_timesafe_memcmp(ct, ct2, pt_len) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"SW and HW encrypted results compare failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------------------
     We must set parameters again so we can do the decrypt
     ---------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, QSEE_TRIPLE_DES_KEY_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     Set IV if not NULL
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_TRIPLE_DES_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     Now decrypt the data
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status && qsee_cipher_decrypt(ctx, ct, pt_len, ct, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_decrypt API failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------------
     Now compare decrypted results
     ---------------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && app_crypto_timesafe_memcmp(pt, ct, pt_len)!= 0)
  {  

     qsee_log(QSEE_LOG_MSG_ERROR," Decrypted results compare failed ");
     status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE Triple DES crypto API's
*/
int tz_app_crypto_aes_large_func(QSEE_CIPHER_ALGO_ET alg,
                                 QSEE_CIPHER_MODE_ET mode,
                                 uint8 *pt,
                                 uint32 pt_len,
                                 uint8 *key,
                                 uint32 key_len,
                                 uint8 *iv,
                                 uint8 *ct,
                                 uint8 *ct2)
{
   uint32              ct_len = pt_len;
   qsee_cipher_ctx     *ctx = 0;
   int                 status = E_SUCCESS;
   CryptoCntxHandle    *cntx = NULL;
   IovecListType       ioVecIn;
   IovecListType       ioVecOut;
   IovecType           IovecIn;
   IovecType           IovecOut;
   uint32  sw_dir  = (SW_CipherEncryptDir)SW_CIPHER_ENCRYPT;
   volatile uint32  sw_alg = (SW_Cipher_Alg_Type)SW_CIPHER_ALG_AES128;
   uint32  sw_mode = (SW_CipherModeType)SW_CIPHER_MODE_ECB;


   //Determine SW algorithm
   if (alg == QSEE_CIPHER_ALGO_AES_128)
   {
      sw_alg = (SW_Cipher_Alg_Type)SW_CIPHER_ALG_AES128;
   }
   else
   {
      sw_alg = (SW_Cipher_Alg_Type)SW_CIPHER_ALG_AES256;
   }

   //Set SW mode
   if (mode == QSEE_CIPHER_MODE_ECB)
   {
      sw_mode = (SW_CipherModeType)SW_CIPHER_MODE_ECB;
   }
   else if (mode == QSEE_CIPHER_MODE_CBC)
   {
      sw_mode = (SW_CipherModeType)SW_CIPHER_MODE_CBC;
   }
   else
   {
      sw_mode = (SW_CipherModeType)SW_CIPHER_MODE_CTR;
   }


   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if (qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set Cipher mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_encrypt(ctx, pt, pt_len, ct, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_encrypt API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now encrypt the data using SW algo
     -------------------------------------------------------------------------*/

   //Now perform encryption using SW
   if(E_SUCCESS == status && qsee_SW_Cipher_Init(&cntx, sw_alg) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
      status = -E_FAILURE;
   }

   sw_dir  = (SW_CipherEncryptDir)SW_CIPHER_ENCRYPT;

   //Set parameters so we can do the encrypt
   if(E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &sw_dir, sizeof(SW_CipherEncryptDir)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParm API failed");
      status = -E_FAILURE;
   }

   if(E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &sw_mode, sizeof(SW_CipherModeType)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParm API API failed");
      status = -E_FAILURE;
   }

   if(E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParm API API failed");
      status = -E_FAILURE;
   }

   //Check for NULL IV pointer
   if (iv != NULL)
   {
      if(E_SUCCESS == status &&
         qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, SW_AES_IV_SIZE) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParm API API failed");
         status = -E_FAILURE;
      }
   }

   /* Input IOVEC */
   ioVecIn.size = 1;
   ioVecIn.iov = &IovecIn;
   ioVecIn.iov[0].dwLen  = pt_len;
   ioVecIn.iov[0].pvBase = pt;

   /* Output IOVEC */
   ioVecOut.size = 1;
   ioVecOut.iov = &IovecOut;
   ioVecOut.iov[0].dwLen  = pt_len;
   ioVecOut.iov[0].pvBase = ct2;

   //Now encrypt the data
   if(E_SUCCESS == status && qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_CipherData API failed");
      status = -E_FAILURE;
   }

   //Determine SW algorithm
   if (alg == QSEE_CIPHER_ALGO_AES_128)
   {
      sw_alg = SW_CIPHER_ALG_AES128;
   }
   else
   {
      sw_alg = SW_CIPHER_ALG_AES256;
   }

   if(E_SUCCESS == status && qsee_SW_Cipher_DeInit(&cntx, sw_alg) !=0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
      status = -E_FAILURE;
   }

   //Now compare encrypted results from SW and HW
  if (E_SUCCESS == status && app_crypto_timesafe_memcmp(ct, ct2, pt_len) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"SW and HW encrypted results compare failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------------------
     We must set parameters again so we can do the decrypt
     ---------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     Set IV if not NULL
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     Now decrypt the data
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status && qsee_cipher_decrypt(ctx, ct, pt_len, ct, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_decrypt API failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------------
     Now compare decrypted results
     ---------------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && app_crypto_timesafe_memcmp(pt, ct, pt_len)!= 0)
  {  
    qsee_log(QSEE_LOG_MSG_ERROR,"Decrypted results compare failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE AES XTS crypto API's
*/
int tz_app_crypto_aes_xts_func(QSEE_CIPHER_ALGO_ET alg,
                               QSEE_CIPHER_MODE_ET mode,
                               uint8 *pt,
                               uint32 pt_len,
                               uint8 *key,
                               uint32 key_len,
                               uint8 *xts_key,
                               uint32 xts_key_len,
                               uint32 xts_du_size,
                               uint8 *iv,
                               uint8 *ct,
                               uint8 verify)
{
   uint32 ct_len = pt_len;
   qsee_cipher_ctx *ctx = 0;
   uint8 *ct_tmp = 0;
   uint8 *pt_tmp = 0;
   int status = E_SUCCESS;

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if (qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set XTS Key
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_KEY, xts_key, xts_key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set XTS DU Size
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_DU_SIZE, &xts_du_size, sizeof(uint32)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------
     Allocate temp ct buffer
     ------------------------------------------------------------------------*/
   if (E_SUCCESS == status && (verify == 1) &&
       (ct_tmp = qsee_malloc(pt_len)) == NULL)
   {
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Clear temp buffer
     ------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1))
   {
      memset(ct_tmp, 0, pt_len);
   }

   /*-----------------------------------------------------------------------
     If verify is 0,  it means we are not using the predefined test case and
     cannot verify the encrypted packet
     -------------------------------------------------------------------------*/
   if (verify == 0)
      ct_tmp = ct;

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_encrypt(ctx, pt, pt_len, ct_tmp, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_encrypt API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------------
     If NULL key pointer then we are using HW key so don't compare encrypted result
     -----------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1) && (NULL != key) &&
      0 != app_crypto_timesafe_memcmp(ct, ct_tmp, pt_len))
   {
      status = -E_FAILURE;
   }

   /*------------------------------------------------------------------------------
     Allocate temp pt buffer
     --------------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1) &&
       (pt_tmp = qsee_malloc(pt_len)) == NULL)
   {
      status = -E_FAILURE;
   }

   if ((E_SUCCESS == status)&& (verify == 1))
   {
      memset(pt_tmp, 0, pt_len);
   }

   /*-----------------------------------------------------------------------
     If verify is 0,  it means we are not using the predefined test case and
     cannot verify the encrypted packet
     -------------------------------------------------------------------------*/
   if (verify == 0)
      pt_tmp = pt;

   /*--------------------------------------------------------------------------------
     We must set parameters again so we can do the decrypt
     ---------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_KEY, xts_key, xts_key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_DU_SIZE, &xts_du_size, sizeof(uint32)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     Set IV if not NULL
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     Now decrypt the data
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_decrypt(ctx, ct_tmp, pt_len, pt_tmp, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_decrypt API failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------------
     Now compare decrypted results
     ---------------------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1) &&
      0 != app_crypto_timesafe_memcmp(pt, pt_tmp, pt_len))
  {  
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   /*--------------------------------------------------------------------------------
     Free malloc data
     ---------------------------------------------------------------------------------*/
   if ((ct_tmp) && (verify == 1))
   {
      qsee_free(ct_tmp);
      ct_tmp = 0;
   }

   if ((pt_tmp)&& (verify == 1))
   {
      qsee_free(pt_tmp);
      pt_tmp = 0;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE AES CCM crypto API's
*/
int tz_app_crypto_aes_ccm_func(QSEE_CIPHER_ALGO_ET alg,
                               QSEE_CIPHER_MODE_ET mode,
                               uint8 *pt,
                               uint32 pt_len,
                               uint8 *key,
                               uint32 key_len,
                               uint8 *iv,
                               uint8 *nonce,
                               uint32 nonce_len,
                               uint32 payload_len,
                               uint8 *ct,
                               uint32 ct_len,
                               uint8* ct2,
                               uint32 header_len,
                               uint32 sw_verify)
{
   qsee_cipher_ctx *ctx = 0;
   uint32 mac_len = ct_len - pt_len;
   uint32 cmp_offset;
   int status = E_SUCCESS;
   IovecListType   ioVecIn;
   IovecListType   ioVecOut;
   IovecType       IovecIn;
   IovecType       IovecOut;
   uint8           tag_auth[16] = {0};
   uint32          header_data = 0;
   CryptoCntxHandle *  sw_cntx = 0;
   uint32 sw_ct_len = ct_len - mac_len;
   uint32  sw_dir  = (SW_CipherEncryptDir)SW_CIPHER_ENCRYPT;
   uint32  sw_alg = (SW_Cipher_Alg_Type)SW_CIPHER_ALG_AES128;
   uint32  sw_mode = (SW_CipherModeType)SW_CIPHER_MODE_CCM;

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if (qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set CCM Payload len
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_PAYLOAD_LEN, &payload_len, sizeof(payload_len)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set CCM Nonce
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_NONCE, nonce, nonce_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set CCM Header len
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_HDR_LEN, &header_len, sizeof(header_len)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set CCM MAC len
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_MAC_LEN, &mac_len, sizeof(mac_len)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_encrypt(ctx, pt, pt_len, ct2, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_encrypt API failed");
      status = -E_FAILURE;
   }

   //Check if we need to do SW verify
   if (sw_verify)
   {
      //Determine Algo
      if (alg == QSEE_CIPHER_ALGO_AES_128)
      {
         sw_alg = SW_CIPHER_ALG_AES128;
      }
      else
      {
         sw_alg = SW_CIPHER_ALG_AES256;
      }

      /* Input IOVEC */
      ioVecIn.size = 1;
      ioVecIn.iov = &IovecIn;
      ioVecIn.iov[0].dwLen = pt_len;
      ioVecIn.iov[0].pvBase = (void*)(uintnt)pt;

      /* Output IOVEC */
      ioVecOut.size = 1;
      ioVecOut.iov = &IovecOut;
      ioVecOut.iov[0].dwLen = sw_ct_len;
      ioVecOut.iov[0].pvBase = (void*)(uintnt)ct;

      /*--------------------------------------------------------------------
        Init context
        ----------------------------------------------------------------------*/
      if(0 != qsee_SW_Cipher_Init(&sw_cntx, sw_alg))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
         status = -E_FAILURE;
      }

      /*--------------------------------------------------------------------
        Set direction to encryption
        ----------------------------------------------------------------------*/

      sw_dir  = SW_CIPHER_ENCRYPT;

      if (E_SUCCESS == status &&
          qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_DIRECTION, &sw_dir, sizeof(SW_CipherEncryptDir)) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
         status = -E_FAILURE;
      }

      /*--------------------------------------------------------------------
        Set AES mode
        ----------------------------------------------------------------------*/
      if (E_SUCCESS == status &&
          qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_MODE, &sw_mode, sizeof(sw_mode)) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
         status = -E_FAILURE;
      }

      /*--------------------------------------------------------------------
        Set key for encryption
        ----------------------------------------------------------------------*/
      if (E_SUCCESS == status &&
          qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
         status = -E_FAILURE;
      }

      /*--------------------------------------------------------------------
        Set IV
        ----------------------------------------------------------------------*/
      if(mode != SW_CIPHER_MODE_ECB)
      {
         if(E_SUCCESS == status &&
            qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_IV, nonce, nonce_len) != 0)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
            status = -E_FAILURE;
         }
      }

      /*--------------------------------------------------------------------
        Set AAD
        ----------------------------------------------------------------------*/
      if (E_SUCCESS == status &&
          qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_AAD, &header_data, 0) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
         status = -E_FAILURE;
      }

      /*--------------------------------------------------------------------
        Set TAG
        ----------------------------------------------------------------------*/
      if(E_SUCCESS == status &&
         qsee_SW_Cipher_SetParam(sw_cntx, SW_CIPHER_PARAM_TAG, tag_auth, mac_len) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
         status = -E_FAILURE;
      }

      /*-----------------------------------------------------------------------
        Now encrypt the data
        -------------------------------------------------------------------------*/
      if (E_SUCCESS == status &&
          qsee_SW_CipherData(sw_cntx, ioVecIn, &ioVecOut) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_CipherData encrypt API failed");
         status = -E_FAILURE;
      }

      /*-----------------------------------------------------------------------
        Get the auth tag
        -------------------------------------------------------------------------*/
      if (E_SUCCESS == status &&
          0 != qsee_SW_Cipher_GetParam(sw_cntx, SW_CIPHER_PARAM_TAG, tag_auth, mac_len))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_GetParam API failed");
         status = -E_FAILURE;
      }

      //Now compare the encrypted results HW vs SW
    if (app_crypto_timesafe_memcmp(ct, ct2, sw_ct_len) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"SW vs HW cipher text compare failed");
         status = -E_FAILURE;
      }

      //Now compare the MAC results HW vs SW
    if(app_crypto_timesafe_memcmp(tag_auth, ct2 + sw_ct_len, mac_len) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"SW vs HW MAC compare failed");
         status = -E_FAILURE;
      }

      /*--------------------------------------------------------------------
        Free context
        ----------------------------------------------------------------------*/
      if(0 != qsee_SW_Cipher_DeInit(&sw_cntx, sw_alg))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
         status = -E_FAILURE;
      }
   }
   else
   {
      /*--------------------------------------------------------------------------
        Now compare encrypted results
        -----------------------------------------------------------------------------*/
      cmp_offset = pt_len - payload_len;
      if (E_SUCCESS == status &&
        0 != app_crypto_timesafe_memcmp(ct + cmp_offset, ct2 + cmp_offset, pt_len - cmp_offset))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"HW cipher text & MAC compare failed");
         status = -E_FAILURE;
      }
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if (qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------------------
     We must set parameters again so we can do the decrypt
     ---------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_PAYLOAD_LEN, &payload_len, sizeof(payload_len)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_NONCE, nonce, nonce_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_HDR_LEN, &header_len, sizeof(header_len)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_CCM_MAC_LEN, &mac_len, sizeof(mac_len)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*----------------------------------------------------------------------------------
     Now decrypt the data
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_decrypt(ctx, ct2, ct_len, ct2, &pt_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_decrypt API failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------------
     Now compare decrypted results
     ---------------------------------------------------------------------------------------*/
   cmp_offset = pt_len - payload_len;
   if ((E_SUCCESS == status) &&
      0 != app_crypto_timesafe_memcmp(pt + cmp_offset, ct2 + cmp_offset, pt_len - cmp_offset))
  {  
    qsee_log(QSEE_LOG_MSG_ERROR,"ccm memcompare failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE AES crypto API's
*/
int tz_app_crypto_aes_block_func(QSEE_CIPHER_ALGO_ET alg,
                                 QSEE_CIPHER_MODE_ET mode,
                                 uint8 *pt,
                                 uint32 pt_len,
                                 uint8 *key,
                                 uint32 key_len,
                                 uint8 *iv,
                                 uint8 *ct)
{
   uint32 ct_len = pt_len;
   uint32 pt_len2 = pt_len / 2;
   uint32 ct_len2 = pt_len2;
   qsee_cipher_ctx *ctx = 0;
   uint8 *pt_tmp = 0;
   int status = E_SUCCESS;

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if (qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for decryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now decrypt the entire data buffer
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_decrypt(ctx, ct, pt_len, pt, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_decrypt API failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   /*------------------------------------------------------------------------------
     Allocate temp pt buffer
     --------------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (pt_tmp = qsee_malloc(pt_len)) == NULL)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status)
   {
      memset(pt_tmp, 0, pt_len);
   }

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for decryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now decrypt the the first half of the data buffer
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_decrypt(ctx, ct, pt_len2, pt_tmp, &ct_len2) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_decrypt API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for again for another decryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now decrypt the the second half of the data buffer
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_decrypt(ctx, ct + pt_len2, pt_len2, pt_tmp + pt_len2, &ct_len2) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_decrypt API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------------
     Now compare decrypted results
     -----------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(pt, pt_tmp, pt_len)))
   {
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   /*--------------------------------------------------------------------------------
     Free malloc data
     ---------------------------------------------------------------------------------*/
   if (pt_tmp)
   {
      qsee_free(pt_tmp);
      pt_tmp = 0;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE HASH crypto API's
*/
int tz_app_crypto_hash_func(QSEE_HASH_ALGO_ET alg,
                            uint8 *msg,
                            uint32 msg_len,
                            uint8 *digest)
{
   int status = E_SUCCESS;
   uint8 result_digest[QSEE_SHA384_HASH_SZ];
   uint32 digest_len = 0;

   //Clear result digest
   memset(result_digest, 0, sizeof(result_digest));

   //Set digest length
   if (alg == QSEE_HASH_SHA1)
      digest_len = QSEE_SHA1_HASH_SZ;
   else if (alg == QSEE_HASH_SHA256)
      digest_len = QSEE_SHA256_HASH_SZ;
   else if (alg == QSEE_HASH_SHA384)
      digest_len = QSEE_SHA384_HASH_SZ;
   else
      return -E_NOT_SUPPORTED;

   //Calculate HASH
   if (qsee_hash(alg, msg, msg_len, result_digest, digest_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash API failed");
      status = -E_FAILURE;
   }

  //Compare rsults 
  if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(result_digest, digest, digest_len)))
   {
      status = -E_FAILURE;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE HASH crypto API's
*/
int tz_app_crypto_hash_block_func(QSEE_HASH_ALGO_ET alg,
                                  uint8 *msg,
                                  uint32 msg_len)
{
   int status = E_SUCCESS;
   uint8 result_digest[QSEE_SHA256_HASH_SZ];
   uint8 result_digest2[QSEE_SHA256_HASH_SZ];
   uint32 digest_len = 0;
   uint32 msg_len2 = msg_len / 2;
   qsee_hash_ctx *hash_ctx = NULL;

   //Clear result digest
   memset(result_digest, 0, sizeof(result_digest));
   memset(result_digest2, 0, sizeof(result_digest2));

   //Set digest length
   if (alg == QSEE_HASH_SHA1)
      digest_len = QSEE_SHA1_HASH_SZ;
   else
      digest_len = QSEE_SHA256_HASH_SZ;

   //Calculate HASH of the entire data buffer
   if (qsee_hash(alg, msg, msg_len, result_digest, digest_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash API failed");
      status = -E_FAILURE;
   }

   //Create hash context
   if (E_SUCCESS == status && qsee_hash_init(alg, &hash_ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_init API failed");
      status = -E_FAILURE;
   }

   if (!hash_ctx)
   {
      status = -E_FAILURE;
      return status;
   }

   //HASH first half of the data buffer
   if (E_SUCCESS == status && qsee_hash_update(hash_ctx, msg, msg_len2) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_update API failed");
      status = -E_FAILURE;
   }

   //HASH second half of data buffer
   if (E_SUCCESS == status && qsee_hash_update(hash_ctx, msg + msg_len2, msg_len2) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_update API failed");
      status = -E_FAILURE;
   }

   //Call HASH final to complete hash operation
   if (E_SUCCESS == status && qsee_hash_final(hash_ctx, result_digest2, digest_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_final API failed");
      status = -E_FAILURE;
   }

  //Compare rsults 
  if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(result_digest, result_digest2, digest_len)))
   {
      status = -E_FAILURE;
   }

   //Call hash reset (Optional Step).
   if (E_SUCCESS == status && qsee_hash_reset(hash_ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_reset API failed");
      status = -E_FAILURE;
   }

   //Free HASH context
   if (hash_ctx != NULL)
   {
      if (qsee_hash_free_ctx(hash_ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_free_ctx API failed");
         status = -E_FAILURE;
      }
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE Simultaneous HASH/AES
   crypto API's
*/
int tz_app_crypto_hash_aes_func(QSEE_HASH_ALGO_ET   hash_algo,
                                QSEE_CIPHER_ALGO_ET cipher_algo,
                                QSEE_CIPHER_MODE_ET cipher_mode,
                                uint8 *pt,
                                uint32 pt_len,
                                uint8 *key,
                                uint32 key_len,
                                uint8 *iv,
                                uint8 *ct,
                                uint8 *ct2)
{
   int               status = E_SUCCESS;
   uint32            digest_len = 0;
   uint32            ct_len  = pt_len;
   QSEE_HASH_MODE_ET hash_mode = QSEE_HASH_MODE_HASH;
   QSEE_HASH_SEQ_ET  hash_seq;
   qsee_cipher_ctx   *cipher_ctx = NULL;
   qsee_hash_ctx     *hash_ctx = NULL;
   uint8             result_digest1[QSEE_SHA256_HASH_SZ];
   uint8             result_digest2[QSEE_SHA256_HASH_SZ];

   //Clear result digest
   memset(result_digest1, 0, sizeof(result_digest1));
   memset(result_digest2, 0, sizeof(result_digest2));

   //Set digest length
   if (hash_algo == QSEE_HASH_SHA1)
      digest_len = QSEE_SHA1_HASH_SZ;
   else
      digest_len = QSEE_SHA256_HASH_SZ;

   /*------------------------------------------------------------------
     Perform independent Hash on plain text data
     ------------------------------------------------------------------*/
   if (qsee_hash(hash_algo, pt, pt_len, result_digest1, digest_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash API failed");
      status = -E_FAILURE;
   }

   /*------------------------------------------------------------------
     Init cipher ctx
     ------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && qsee_cipher_init(cipher_algo, &cipher_ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(cipher_ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(cipher_ctx, QSEE_CIPHER_PARAM_MODE, &cipher_mode, sizeof(cipher_mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status && NULL != iv &&
       qsee_cipher_set_param(cipher_ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now perform independent encrypt of data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_encrypt(cipher_ctx, pt, pt_len, ct, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_encrypt API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Create hash context
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status && qsee_hash_init(hash_algo, &hash_ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_init API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Set hash mode
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_hash_set_param(hash_ctx, QSEE_HASH_PARAM_MODE, &hash_mode, sizeof(QSEE_HASH_MODE_ET)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_set_param API for mode failed = 0x%x",hash_mode);
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Set hash sequence
     -------------------------------------------------------------------------*/
   hash_seq = QSEE_HASH_MODE_FIRST;
   if (E_SUCCESS == status &&
       qsee_hash_set_param(hash_ctx, QSEE_HASH_PARAM_SEQ, &hash_seq, sizeof(QSEE_HASH_SEQ_ET)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_set_param API for seq failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status && NULL != iv &&
       qsee_cipher_set_param(cipher_ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API for IV failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now do the simutaneous Hash/Cipher encrypt operation
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_hashcipher_encrypt(cipher_ctx, hash_ctx, pt, pt_len, ct2, ct_len, result_digest2, digest_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hashcipher_encrypt API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Compare Cipher rsults
  -------------------------------------------------------------------------*/  
  if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(ct, ct2, ct_len)))
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hashcipher_encrypt CT mem compare failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Compare Hash results
  -------------------------------------------------------------------------*/  
  if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(result_digest1, result_digest2, digest_len)))
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hashcipher_encrypt HASH mem compare failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Free hash context
     -------------------------------------------------------------------------*/
   if (hash_ctx)
   {
      if (qsee_hash_free_ctx(hash_ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_free_ctx API failed");
         status = -E_FAILURE;
      }
      hash_ctx = NULL;
   }

   /*-----------------------------------------------------------------------
     Create hash context again
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status && qsee_hash_init(hash_algo, &hash_ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_init API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Set hash mode
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_hash_set_param(hash_ctx, QSEE_HASH_PARAM_MODE, &hash_mode, sizeof(QSEE_HASH_MODE_ET)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_set_param API for mode2 failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Set hash sequence
     -------------------------------------------------------------------------*/
   hash_seq = QSEE_HASH_MODE_LAST;
   if (E_SUCCESS == status &&
       qsee_hash_set_param(hash_ctx, QSEE_HASH_PARAM_SEQ, &hash_seq, sizeof(QSEE_HASH_SEQ_ET)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_set_param API for seq2 failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status && NULL != iv &&
       qsee_cipher_set_param(cipher_ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API for IV2 failed");
      status = -E_FAILURE;
   }

   //Clear digest result
   memset(result_digest2, 0, sizeof(result_digest2));

   /*-----------------------------------------------------------------------
     Now do the simutaneous Hash/Cipher decrypt operation
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_hashcipher_decrypt(cipher_ctx, hash_ctx, ct2, ct_len, ct, pt_len, result_digest2, digest_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hashcipher_decrypt API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Compare Plain text rsults
  -------------------------------------------------------------------------*/  
  if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(pt, ct, pt_len)))
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hashcipher_decrypt PT mem compare failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Compare Hash results
  -------------------------------------------------------------------------*/  
  if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(result_digest1, result_digest2, digest_len)))
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hashcipher_decrypt HASH mem compare failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Free hash context
     -------------------------------------------------------------------------*/
   if (hash_ctx)
   {
      if (qsee_hash_free_ctx(hash_ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_free_ctx API failed");
         status = -E_FAILURE;
      }
   }

   /*-----------------------------------------------------------------------
     Free cipher context
     -------------------------------------------------------------------------*/
   if (cipher_ctx)
   {
      if (qsee_cipher_free_ctx(cipher_ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE HMAC crypto API's
*/
int tz_app_crypto_hmac_func(uint8 *msg,
                            uint32 msg_len,
                            uint8* key,
                            uint32 key_len,
                            uint8* hmac1,
                            uint8* hmac256)
{
   int status = E_SUCCESS;
   uint8 result_digest[QSEE_SHA256_HASH_SZ];

   //Clear result digest
   memset(result_digest, 0, sizeof(result_digest));

   //Calculate HASH SHA1
   if (qsee_hmac(QSEE_HMAC_SHA1, msg, msg_len, key, key_len, result_digest) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hmac API failed");
      status = -E_FAILURE;
   }

  //Compare rsults 
  if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(result_digest, hmac1, QSEE_SHA1_HASH_SZ)))
   {
      status = -E_FAILURE;
   }

   //Clear result digest
   memset(result_digest, 0, sizeof(result_digest));

   //Calculate HASH SHA256
   if ((E_SUCCESS == status) &&
       qsee_hmac(QSEE_HMAC_SHA256, msg, msg_len, key, key_len, result_digest) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hmac API failed");
      status = -E_FAILURE;
   }

  //Compare rsults 
  if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(result_digest, hmac256, QSEE_SHA256_HASH_SZ)))
   {
      status = -E_FAILURE;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE CMAC crypto API's
*/
int tz_app_crypto_cmac_func(uint8 *msg,
                            uint32 msg_len,
                            uint8* key,
                            uint32 key_len,
                            uint8* cmac,
                            uint32 cmac_len)
{
   int status = E_SUCCESS;
   uint8 cmac_digest[QSEE_CMAC_DIGEST_SIZE];
   QSEE_CMAC_ALGO_ET alg;

   //Determine algorithm
   if (key_len == 16)
   {
      alg = QSEE_CMAC_ALGO_AES_128;
   }
   else
   {
      alg = QSEE_CMAC_ALGO_AES_256;
   }

   //Clear result digest
   memset(cmac_digest, 0, sizeof(cmac_digest));

   //Calculate CMAC
   if (qsee_cmac(alg, msg, msg_len, key, key_len, cmac_digest, cmac_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cmac API failed");
      status = -E_FAILURE;
   }

  //Compare rsults 
  if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(cmac_digest, cmac, cmac_len)))
   {
      status = -E_FAILURE;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE AES crypto API's
*/
int tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_ET alg,
                           QSEE_CIPHER_MODE_ET mode,
                           uint8 *pt,
                           uint32 pt_len,
                           uint8 *key,
                           uint32 key_len,
                           uint8 *iv)
{
   uint32 ct_len = pt_len;
   uint8 *ct = pt;
   qsee_cipher_ctx *ctx = 0;
   int status = E_SUCCESS;

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if (qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_encrypt(ctx, pt, pt_len, ct, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_encrypt API failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE Triple DES crypto API's
*/
int tz_app_crypto_triple_des_perf(QSEE_CIPHER_ALGO_ET alg,
                                  QSEE_CIPHER_MODE_ET mode,
                                  uint8 *pt,
                                  uint32 pt_len,
                                  uint8 *key,
                                  uint8 *iv)
{
   uint32 ct_len = pt_len;
   uint8 *ct = pt;
   qsee_cipher_ctx *ctx = 0;
   int status = E_SUCCESS;

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if (qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, QSEE_TRIPLE_DES_KEY_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_TRIPLE_DES_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_encrypt(ctx, pt, pt_len, ct, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_encrypt API failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE AES XTS crypto API's
*/
int tz_app_crypto_aes_xts_perf(QSEE_CIPHER_ALGO_ET alg,
                               QSEE_CIPHER_MODE_ET mode,
                               uint8 *pt,
                               uint32 pt_len,
                               uint8 *key,
                               uint32 key_len,
                               uint8 *xts_key,
                               uint32 xts_key_len,
                               uint32 xts_du_size,
                               uint8 *iv)
{
   uint32 ct_len = pt_len;
   uint8 *ct = pt;
   qsee_cipher_ctx *ctx = 0;
   int status = E_SUCCESS;

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/
   if (qsee_cipher_init(alg, &ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_MODE, &mode, sizeof(mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set XTS Key
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_KEY, xts_key, xts_key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set XTS DU Size
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_XTS_DU_SIZE, &xts_du_size, sizeof(uint32)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       NULL != iv &&
       qsee_cipher_set_param(ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_encrypt(ctx, pt, pt_len, ct, &ct_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_encrypt API failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (ctx)
   {
      if (qsee_cipher_free_ctx(ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
      ctx = 0;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE HASH crypto API's
*/
int tz_app_crypto_hash_perf(QSEE_HASH_ALGO_ET alg,
                            uint8 *msg,
                            uint32 msg_len)
{
   int status = E_SUCCESS;
   uint8 result_digest[QSEE_SHA256_HASH_SZ];
   uint32 digest_len = 0;

   //Set digest length
   if (alg == QSEE_HASH_SHA1)
      digest_len = QSEE_SHA1_HASH_SZ;
   else
      digest_len = QSEE_SHA256_HASH_SZ;

   //Calculate HASH
   if (qsee_hash(alg, msg, msg_len, result_digest, digest_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash API failed");
      status = -E_FAILURE;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE Simultaneous HASH/AES
   Performance test
*/
int tz_app_crypto_hash_aes_perf(QSEE_HASH_ALGO_ET   hash_algo,
                                QSEE_CIPHER_ALGO_ET cipher_algo,
                                QSEE_CIPHER_MODE_ET cipher_mode,
                                uint8 *pt,
                                uint32 pt_len,
                                uint8 *key,
                                uint32 key_len,
                                uint8 *iv)
{
   int               status = E_SUCCESS;
   uint32            digest_len = 0;
   uint32            ct_len  = pt_len;
   QSEE_HASH_MODE_ET hash_mode = QSEE_HASH_MODE_HASH;
   QSEE_HASH_SEQ_ET  hash_seq;
   qsee_cipher_ctx   *cipher_ctx = NULL;
   qsee_hash_ctx     *hash_ctx = NULL;
   uint8             result_digest1[QSEE_SHA256_HASH_SZ];

   //Clear result digest
   memset(result_digest1, 0, sizeof(result_digest1));

   //Set digest length
   if (hash_algo == QSEE_HASH_SHA1)
      digest_len = QSEE_SHA1_HASH_SZ;
   else
      digest_len = QSEE_SHA256_HASH_SZ;

   /*-----------------------------------------------------------------------
     Create hash context
     -------------------------------------------------------------------------*/
   if (qsee_hash_init(hash_algo, &hash_ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_init API failed");
      status = -E_FAILURE;
   }

   if (!hash_ctx)
   {
      status = -E_FAILURE;
      return status;
   }
   /*-----------------------------------------------------------------------
     Set hash mode
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_hash_set_param(hash_ctx, QSEE_HASH_PARAM_MODE, &hash_mode, sizeof(QSEE_HASH_MODE_ET)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_set_param API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Set hash sequence
     -------------------------------------------------------------------------*/
   hash_seq = QSEE_HASH_MODE_FIRST;
   if (E_SUCCESS == status &&
       qsee_hash_set_param(hash_ctx, QSEE_HASH_PARAM_SEQ, &hash_seq, sizeof(QSEE_HASH_SEQ_ET)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_set_param API failed");
      status = -E_FAILURE;
   }

   /*------------------------------------------------------------------
     Init cipher ctx
     ------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && qsee_cipher_init(cipher_algo, &cipher_ctx) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_init API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(cipher_ctx, QSEE_CIPHER_PARAM_KEY, key, key_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_cipher_set_param(cipher_ctx, QSEE_CIPHER_PARAM_MODE, &cipher_mode, sizeof(cipher_mode)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV only if not NULL
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status && NULL != iv &&
       qsee_cipher_set_param(cipher_ctx, QSEE_CIPHER_PARAM_IV, iv, QSEE_AES128_IV_SIZE) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now do the simutaneous Hash/Cipher encrypt operation
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_hashcipher_encrypt(cipher_ctx, hash_ctx, pt, pt_len, pt, ct_len, result_digest1, digest_len) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hashcipher_encrypt API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Free hash context
     -------------------------------------------------------------------------*/
   if (hash_ctx)
   {
      if (qsee_hash_free_ctx(hash_ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hash_free_ctx API failed");
         status = -E_FAILURE;
      }
   }

   /*-----------------------------------------------------------------------
     Free cipher context
     -------------------------------------------------------------------------*/
   if (cipher_ctx)
   {
      if (qsee_cipher_free_ctx(cipher_ctx) < 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_free_ctx API failed");
         status = -E_FAILURE;
      }
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE HMAC crypto API's
*/
int tz_app_crypto_hmac_perf(QSEE_HMAC_ALGO_ET alg,
                            uint8 *msg,
                            uint32 msg_len,
                            uint8* key,
                            uint32 key_len)
{
   int status = E_SUCCESS;
   uint8 result_digest[QSEE_SHA256_HASH_SZ];

   //Calculate HMAC
   if (qsee_hmac(alg, msg, msg_len, key, key_len, result_digest) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_hmac API failed");
      status = -E_FAILURE;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE CMAC crypto API's
*/
int tz_app_crypto_cmac_perf(QSEE_CMAC_ALGO_ET alg,
                            uint8 *msg,
                            uint32 msg_len,
                            uint8* key,
                            uint32 key_len)
{
   int status = E_SUCCESS;
   uint8 cmac_digest[QSEE_CMAC_DIGEST_SIZE];

   //Calculate CMAC
   if (qsee_cmac(alg, msg, msg_len, key, key_len, cmac_digest, sizeof(cmac_digest)) < 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cmac API failed");
      status = -E_FAILURE;
   }

   return status;
}

/**
   @brief
   Sample code for AES functional tests
*/
int tz_app_aes_func(void)
{
   int idx=0, jdx=0;
   int num=0;
   int status = E_SUCCESS;
   uint32 plain_text_len = sizeof(aes_large_plain_text2);
   num = sizeof(aes_test_vectors) / sizeof(aes_vector_type_t);

   /*---------------------------------------------------------------
     Run through AES128/256 ECB/CBC/CTR/CTS test vectors
     -------------------------------------------------------------------*/
   for (idx = 0; idx < num; idx++)
   {
      status = tz_app_crypto_aes_func(aes_test_vectors[idx].alg,
                                      aes_test_vectors[idx].mode,
                                      aes_test_vectors[idx].pt,
                                      aes_test_vectors[idx].pt_len,
                                      aes_test_vectors[idx].key,
                                      aes_test_vectors[idx].key_len,
                                      aes_test_vectors[idx].iv,
                                      aes_test_vectors[idx].ct, 1);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto AES Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto AES Functional Test - Vect Num:%u Failed", idx);
         return status;
      }
   }

   /*---------------------------------------------------------------
     AES128/256 CTR tests for setting CTR IV SIZE
     -------------------------------------------------------------------*/
   num = sizeof(aes_test_vectors_ctr) / sizeof(aes_vector_type_t);
   for (idx = 0; idx < num; idx++)
   {
      status = tz_app_crypto_aes_ctr_sz(aes_test_vectors_ctr[idx].alg,
                                      aes_test_vectors_ctr[idx].mode,
                                      aes_test_vectors_ctr[idx].pt,
                                      aes_test_vectors_ctr[idx].pt_len,
                                      aes_test_vectors_ctr[idx].key,
                                      aes_test_vectors_ctr[idx].key_len,
                                      aes_test_vectors_ctr[idx].iv,
                                      aes_test_vectors_ctr[idx].ct, 1);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto AES CTR size Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto AES CTR size Test - Vect Num:%u Failed", idx);
         return status;
      }
   }

   /*---------------------------------------------------------------
     Run AES multiple block test vector
     -------------------------------------------------------------------*/
   memset(aes_large_plain_text_1, 0, sizeof(aes_large_plain_text_1));
   memset(aes_large_cipher_text_1, 0xed, sizeof(aes_large_cipher_text_1));

   status = tz_app_crypto_aes_block_func(QSEE_CIPHER_ALGO_AES_128,
                                         QSEE_CIPHER_MODE_CBC,
                                         aes_large_plain_text_1,
                                         sizeof(aes_large_plain_text_1),
                                         aes_key_2,
                                         sizeof(aes_key_2),
                                         aes_iv_2,
                                         aes_large_cipher_text_1);

   if (status == E_SUCCESS)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto AES Functional Block Test Passed");
   }
   else
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"Crypto AES Functional Block Test Failed");
   }

   /*---------------------------------------------------------------
     Run through AES ECB, CBC and CTR large test vectors
     ----------------------------------------------------------------*/

   memset(aes_large_plain_text2, 0xED, plain_text_len);

   aes_test_vectors_2.pt  = aes_large_plain_text2;
   aes_test_vectors_2.ct  = aes_large_cipher_text2;
   aes_test_vectors_2.ct2 = aes_performance;

   //Test AES CBC with large vectors
   for (idx = 16; idx <= plain_text_len; idx *= 2)
   {
      aes_test_vectors_2.pt_len = idx;

      status = tz_app_crypto_aes_large_func(aes_test_vectors_2.alg,
                                            QSEE_CIPHER_MODE_CBC,
                                            aes_test_vectors_2.pt,
                                            aes_test_vectors_2.pt_len,
                                            aes_test_vectors_2.key,
                                            aes_test_vectors_2.key_len,
                                            aes_test_vectors_2.iv,
                                            aes_test_vectors_2.ct,
                                            aes_test_vectors_2.ct2);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto AES CBC Large Vector Functional Test - Vect Size:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto AES CBC Large Vector Functional Test - Vect Size:%u Failed", idx);
         return status;
      }
   }

   //Test AES ECB with large vectors
   for (idx = 16; idx <= plain_text_len; idx *= 2)
   {
      aes_test_vectors_2.pt_len = idx;

      status = tz_app_crypto_aes_large_func(aes_test_vectors_2.alg,
                                            QSEE_CIPHER_MODE_ECB,
                                            aes_test_vectors_2.pt,
                                            aes_test_vectors_2.pt_len,
                                            aes_test_vectors_2.key,
                                            aes_test_vectors_2.key_len,
                                            NULL,
                                            aes_test_vectors_2.ct,
                                            aes_test_vectors_2.ct2);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto AES ECB Large Vector Functional Test - Vect Size:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto AES ECB Large Vector Functional Test - Vect Size:%u Failed", idx);
         return status;
      }
   }

   //Test AES CTR with large vectors
   for (idx = 8; idx <= plain_text_len; idx *= 2)
   {
      aes_test_vectors_2.pt_len = idx;

      status = tz_app_crypto_aes_large_func(aes_test_vectors_2.alg,
                                            QSEE_CIPHER_MODE_CTR,
                                            aes_test_vectors_2.pt,
                                            aes_test_vectors_2.pt_len,
                                            aes_test_vectors_2.key,
                                            aes_test_vectors_2.key_len,
                                            aes_test_vectors_2.iv,
                                            aes_test_vectors_2.ct,
                                            aes_test_vectors_2.ct2);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto AES CTR Large Vector Functional Test - Vect Size:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto AES CTR Large Vector Functional Test - Vect Size:%u Failed", idx);
         return status;
      }
   }

   /*---------------------------------------------------------------
     Run through AES CTR large vector test with rotating IV vector
     ----------------------------------------------------------------*/

   //Zero IV vector
   memset(aes_iv_4, 0, sizeof(aes_iv_4));

   //Loop over different IVs starting with IV = all zeros to IV = all 0xFF
   for (jdx = 0; jdx < sizeof(aes_iv_4) - 1; jdx++)
   {
      //Test AES CTR with large vectors
      for (idx = 8; idx <= plain_text_len; idx *= 2)
      {
         aes_test_vectors_2.pt_len = idx;

         status = tz_app_crypto_aes_large_func(aes_test_vectors_2.alg,
                                               QSEE_CIPHER_MODE_CTR,
                                               aes_test_vectors_2.pt,
                                               aes_test_vectors_2.pt_len,
                                               aes_test_vectors_2.key,
                                               aes_test_vectors_2.key_len,
                                               aes_iv_4,
                                               aes_test_vectors_2.ct,
                                               aes_test_vectors_2.ct2);

         if (status == E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto AES CTR Large Vector Rotating IV Functional Test - Vect Size:%u Passed",idx);
         }
         else
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"Crypto AES CTR Large Vector Rotating IV Functional Test - Vect Size:%u Failed",idx);
            return status;
         }
      }

      //Increment IV vector
      aes_iv_4[jdx] = 0xff;
   }

   return status;
}

/**
   @brief
   Sample code for DES functional tests
*/
int tz_app_des_func(void)
{
   int idx=0;
   int num=0;
   int status = E_SUCCESS;
   uint32 plain_text_len = sizeof(aes_large_plain_text2);

   num = sizeof(triple_des_test_vectors) / sizeof(triple_des_vector_t);

   /*---------------------------------------------------------------
     Run through Triple DES ECB and CBC test vectors
     -------------------------------------------------------------------*/
   for (idx = 0; idx < num; idx++)
   {
      status = tz_app_crypto_triple_des_func(triple_des_test_vectors[idx].alg,
                                             triple_des_test_vectors[idx].mode,
                                             triple_des_test_vectors[idx].pt,
                                             triple_des_test_vectors[idx].pt_len,
                                             triple_des_test_vectors[idx].key,
                                             triple_des_test_vectors[idx].iv,
                                             triple_des_test_vectors[idx].ct, 1);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto Triple DES Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto Triple DES Functional Test - Vect Num:%u Failed", idx);
         return status;
      }
   }

   /*---------------------------------------------------------------
     Run through Triple DES ECB and CBC large test vectors
     -------------------------------------------------------------------*/

   memset(aes_large_plain_text2, 0xED, plain_text_len);

   triple_des_test_vectors_2.pt  = aes_large_plain_text2;
   triple_des_test_vectors_2.ct  = aes_large_cipher_text2;
   triple_des_test_vectors_2.ct2 = aes_performance;

   //Test Triple DES CBC with large vectors
   for (idx = 8; idx <= plain_text_len; idx *= 2)
   {
      triple_des_test_vectors_2.pt_len = idx;

      status = tz_app_crypto_triple_des_large_func(triple_des_test_vectors_2.alg,
                                                   QSEE_CIPHER_MODE_CBC,
                                                   triple_des_test_vectors_2.pt,
                                                   triple_des_test_vectors_2.pt_len,
                                                   triple_des_test_vectors_2.key,
                                                   triple_des_test_vectors_2.iv,
                                                   triple_des_test_vectors_2.ct,
                                                   triple_des_test_vectors_2.ct2);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto Triple DES CBC Large Vector Functional Test - Vect Size:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto Triple DES CBC Large Vector Functional Test - Vect Size:%u Failed", idx);
         return status;
      }
   }

   //Test Triple DES ECB with large vectors
   for (idx = 8; idx <= plain_text_len; idx *= 2)
   {
      triple_des_test_vectors_2.pt_len = idx;

      status = tz_app_crypto_triple_des_large_func(triple_des_test_vectors_2.alg,
                                                   QSEE_CIPHER_MODE_ECB,
                                                   triple_des_test_vectors_2.pt,
                                                   triple_des_test_vectors_2.pt_len,
                                                   triple_des_test_vectors_2.key,
                                                   NULL,
                                                   triple_des_test_vectors_2.ct,
                                                   triple_des_test_vectors_2.ct2);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto Triple DES ECB Large Vector Functional Test - Vect Size:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto Triple DES ECB Large Vector Functional Test - Vect Size:%u Failed", idx);
         return status;
      }
   }

   return status;
}

/**
   @brief
   Sample code for DES functional tests
*/
int tz_app_aes_xts_func(void)
{
   int idx=0;
   int num=0;
   int status = E_SUCCESS;
   num = sizeof(aes_xts_test_vectors) / sizeof(aes_xts_vector_t);

   /*---------------------------------------------------------------
     Run through AES 126 & 256 XTS test vectors
     -------------------------------------------------------------------*/
   for (idx = 0; idx < num; idx++)
   {
      status = tz_app_crypto_aes_xts_func(aes_xts_test_vectors[idx].alg,
                                          aes_xts_test_vectors[idx].mode,
                                          aes_xts_test_vectors[idx].pt,
                                          aes_xts_test_vectors[idx].pt_len,
                                          aes_xts_test_vectors[idx].key,
                                          aes_xts_test_vectors[idx].key_len,
                                          aes_xts_test_vectors[idx].xts_key,
                                          aes_xts_test_vectors[idx].xts_key_len,
                                          aes_xts_test_vectors[idx].xts_du_size,
                                          aes_xts_test_vectors[idx].iv,
                                          aes_xts_test_vectors[idx].ct, 1);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto AES XTS Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto AES XTS Functional Test - Vect Num:%u Failed", idx);
         return status;
      }
   }

   return status;
}

/**
   @brief
   Sample code for AES CCM functional tests
*/
int tz_app_aes_ccm_func(void)
{
   int idx=0;
   int num=0;
   int status = E_SUCCESS;

   uint32 plain_text_len = sizeof(aes_large_plain_text2);

   num = sizeof(aes_ccm_test_vectors) / sizeof(aes_ccm_vector_type);

   /*---------------------------------------------------------------
     Run through AES 126 & 256 XTS test vectors
     -------------------------------------------------------------------*/
   for (idx = 0; idx < num; idx++)
   {
      status = tz_app_crypto_aes_ccm_func(aes_ccm_test_vectors[idx].alg,
                                          aes_ccm_test_vectors[idx].mode,
                                          aes_ccm_test_vectors[idx].pt,
                                          aes_ccm_test_vectors[idx].pt_len,
                                          aes_ccm_test_vectors[idx].key,
                                          aes_ccm_test_vectors[idx].key_len,
                                          aes_ccm_test_vectors[idx].iv,
                                          aes_ccm_test_vectors[idx].nonce,
                                          aes_ccm_test_vectors[idx].nonce_len,
                                          aes_ccm_test_vectors[idx].payload_len,
                                          aes_ccm_test_vectors[idx].ct,
                                          aes_ccm_test_vectors[idx].ct_len,
                                          aes_ccm_test_vectors[idx].ct2,
                                          aes_ccm_test_vectors[idx].header_len,
                                          aes_ccm_test_vectors[idx].sw_verify);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto AES CCM Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto AES CCM Functional Test - Vect Num:%u Failed", idx);
         return status;
      }
   }

   //Test AES CCM with large vectors
   for (idx = 16; idx <= plain_text_len; idx *= 2)
   {
      //Set the length
      aes_ccm_test_vectors_1.pt_len      = idx;
      aes_ccm_test_vectors_1.payload_len = idx;
      aes_ccm_test_vectors_1.ct_len      = idx + 8;

      status = tz_app_crypto_aes_ccm_func(aes_ccm_test_vectors_1.alg,
                                          aes_ccm_test_vectors_1.mode,
                                          aes_ccm_test_vectors_1.pt,
                                          aes_ccm_test_vectors_1.pt_len,
                                          aes_ccm_test_vectors_1.key,
                                          aes_ccm_test_vectors_1.key_len,
                                          aes_ccm_test_vectors_1.iv,
                                          aes_ccm_test_vectors_1.nonce,
                                          aes_ccm_test_vectors_1.nonce_len,
                                          aes_ccm_test_vectors_1.payload_len,
                                          aes_ccm_test_vectors_1.ct,
                                          aes_ccm_test_vectors_1.ct_len,
                                          aes_ccm_test_vectors_1.ct2,
                                          aes_ccm_test_vectors_1.header_len,
                                          aes_ccm_test_vectors_1.sw_verify);
      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto AES CCM Large Vector Functional Test - Vect Size:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto AES CCM Large Vector Functional Test - Vect Size:%u Failed", idx);
         return status;
      }
   }
   return status;
}

/**
   @brief
   Sample code for HASH functional tests
*/
int tz_app_hash_func(void)
{
   int idx=0;
   int num=0;
   int status = E_SUCCESS;
   num = sizeof(sha_test_vectors) / sizeof(sha_vector_type_t);

   /*---------------------------------------------------------------
     Run through HASH test vectors
     -------------------------------------------------------------------*/
   for (idx = 0; idx < num; idx++)
   {
      status = tz_app_crypto_hash_func(sha_test_vectors[idx].alg,
                                       sha_test_vectors[idx].msg,
                                       sha_test_vectors[idx].msg_len,
                                       sha_test_vectors[idx].digest);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto Hash Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto Hash Functional Test - Vect Num:%u Failed", idx);
         return status;
      }
   }

   /*---------------------------------------------------------------
     Run HASH multiple block test vector
     -------------------------------------------------------------------*/
   memset(aes_large_plain_text_1, 0xed, sizeof(aes_large_plain_text_1));

   status = tz_app_crypto_hash_block_func(QSEE_HASH_SHA1,
                                          aes_large_plain_text_1,
                                          sizeof(aes_large_plain_text_1));

   if (status == E_SUCCESS)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto Hash Functional Block Test Passed");
   }
   else
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"Crypto Hash Functional Block Test Failed");
   }

   return status;
}

/**
   @brief
   Sample code for simultaneous HASH/AES functional tests
*/
int tz_app_hash_aes_func(void)
{
   int idx=0;
   int num=0;
   int j=0;
   int status = E_SUCCESS;

   num = sizeof(hash_aes_test_vectors) / sizeof(hash_aes_vector_type);

   memset(aes_large_plain_text2, 0xED, sizeof(aes_large_plain_text2));

   /*---------------------------------------------------------------
     Run through Simultaneous HASH/AES test vectors
     -------------------------------------------------------------------*/
   for (idx = 0; idx < num; idx++)
   {
      //Perform test vector over large data vector size
      for (j = 16; j <= sizeof(aes_large_plain_text2); j *= 2)
      {
         //Set plain text lenght
         hash_aes_test_vectors[idx].pt_len = j;

         status = tz_app_crypto_hash_aes_func(hash_aes_test_vectors[idx].hash_algo,
                                              hash_aes_test_vectors[idx].cipher_algo,
                                              hash_aes_test_vectors[idx].cipher_mode,
                                              hash_aes_test_vectors[idx].pt,
                                              hash_aes_test_vectors[idx].pt_len,
                                              hash_aes_test_vectors[idx].key,
                                              hash_aes_test_vectors[idx].key_len,
                                              hash_aes_test_vectors[idx].iv,
                                              hash_aes_test_vectors[idx].ct,
                                              hash_aes_test_vectors[idx].ct2);

         if (status == E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto Simultaneous Hash/AES Functional Test - Vect Num:%u Vect Size:%u Passed", idx, j);
         }
         else
         {
	    qsee_log(QSEE_LOG_MSG_ERROR,"Crypto Simultaneous Hash/AES Functional Test - Vect Num:%u Vect Size:%u Failed", idx, j);
            return status;
         }
      }
   }

   return status;
}

/**
   @brief
   Sample code for HMAC functional tests
*/
int tz_app_hmac_func(void)
{
   int idx=0;
   int num=0;
   int status = E_SUCCESS;
   num = sizeof(sha_hmac_test_vectors) / sizeof(sha_hmac_vector_type_t);

   /*---------------------------------------------------------------
     Run through HMAC test vectors
     -------------------------------------------------------------------*/
   for (idx = 0; idx < num; idx++)
   {
      status = tz_app_crypto_hmac_func(sha_hmac_test_vectors[idx].msg,
                                       sha_hmac_test_vectors[idx].msg_len,
                                       sha_hmac_test_vectors[idx].hmac_key,
                                       sha_hmac_test_vectors[idx].hmac_key_len,
                                       sha_hmac_test_vectors[idx].msg_hmac1,
                                       sha_hmac_test_vectors[idx].msg_hmac256);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto Hmac Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto Hmac Functional Test - Vect Num:%u Failed", idx);
      }
   }

   return status;
}

/**
   @brief
   Sample code for CMAC functional tests
*/
int tz_app_cmac_func(void)
{
   int idx=0;
   int num=0;
   int status = E_SUCCESS;
   num = sizeof(aes_cmac_test_vectors) / sizeof(aes_cmac_vector_t);

   /*---------------------------------------------------------------
     Run through CMAC test vectors
     -------------------------------------------------------------------*/
   for (idx = 0; idx < num; idx++)
   {
      status = tz_app_crypto_cmac_func(aes_cmac_test_vectors[idx].msg,
                                       aes_cmac_test_vectors[idx].msg_len,
                                       aes_cmac_test_vectors[idx].key,
                                       aes_cmac_test_vectors[idx].key_len,
                                       aes_cmac_test_vectors[idx].cmac,
                                       aes_cmac_test_vectors[idx].cmac_len);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto CMAC Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto CMAC Functional Test - Vect Num:%u Failed", idx);
      }
   }

   return status;
}

/**
   @brief
   Sample code for AES performance tests
*/
int tz_app_aes_perf(void)
{
   uint32 size;
   uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
   uint32 i = 0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;

   char name[] = "CE_TEST";
   QSEE_RESOURCE ce_clk = CE1;
   QSEE_CLKLEV voteon_freq = HIGH;
   QSEE_CLKLEV voteoff_freq = INACTIVE;

   /*----------------------------------------------------------------------------------
     vote for crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/

   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-on");
   total_time = qsee_get_uptime();
   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteon_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-on failed");
      status = -E_FAILURE;
      return status;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

   for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance AES128 Test - AES Algo: ECB Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_128,
                                         QSEE_CIPHER_MODE_ECB,
                                         aes_performance,
                                         size,
                                         aes_key_1,
                                         sizeof(aes_key_1),
                                         NULL);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance AES128 Test - AES Algo: CBC Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_128,
                                         QSEE_CIPHER_MODE_CBC,
                                         aes_performance,
                                         size,
                                         aes_key_2,
                                         sizeof(aes_key_2),
                                         aes_iv_2);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance AES128 Test - AES Algo: CTR Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_128,
                                         QSEE_CIPHER_MODE_CTR,
                                         aes_performance,
                                         size,
                                         aes_key_2,
                                         sizeof(aes_key_2),
                                         aes_iv_2);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance AES256 Test - AES Algo: ECB Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_256,
                                         QSEE_CIPHER_MODE_ECB,
                                         aes_performance,
                                         size,
                                         aes256_key_1,
                                         sizeof(aes256_key_1),
                                         NULL);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance AES256 Test - AES Algo: CBC Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_256,
                                         QSEE_CIPHER_MODE_CBC,
                                         aes_performance,
                                         size,
                                         aes256_key_2,
                                         sizeof(aes256_key_2),
                                         aes256_iv_2);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance AES256 Test - AES Algo: CTR Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_aes_perf(QSEE_CIPHER_ALGO_AES_256,
                                         QSEE_CIPHER_MODE_CTR,
                                         aes_performance,
                                         size,
                                         aes256_key_2,
                                         sizeof(aes256_key_2),
                                         aes256_iv_2);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
   }


   /*----------------------------------------------------------------------------------
     vote off crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/
   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-off");
   total_time = qsee_get_uptime();

   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteoff_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-off failed");
      status = -E_FAILURE;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);

   return status;
}

/**
   @brief
   Sample code for DES performance tests
*/
int tz_app_des_perf(void)
{
   uint32 size;
   uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
   uint32 i = 0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;

   char name[] = "CE_TEST";
   QSEE_RESOURCE ce_clk = CE1;
   QSEE_CLKLEV voteon_freq = HIGH;
   QSEE_CLKLEV voteoff_freq = INACTIVE;

   /*----------------------------------------------------------------------------------
     vote for crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/

   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-on");
   total_time = qsee_get_uptime();
   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteon_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-on failed");
      status = -E_FAILURE;
      return status;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

   for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Triple DES Test - DES Algo: ECB Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_triple_des_perf(QSEE_CIPHER_ALGO_TRIPLE_DES,
                                                QSEE_CIPHER_MODE_ECB,
                                                aes_performance,
                                                size,
                                                triple_des_key_1_ecb,
                                                NULL);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_triple_des_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Triple DES Test - DES Algo: CBC Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_triple_des_perf(QSEE_CIPHER_ALGO_TRIPLE_DES,
                                                QSEE_CIPHER_MODE_CBC,
                                                aes_performance,
                                                size,
                                                triple_des_key_1_cbc,
                                                triple_des_iv_1_cbc);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_triple_des_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
   }
   /*----------------------------------------------------------------------------------
     vote off crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/
   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-off");
   total_time = qsee_get_uptime();

   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteoff_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-off failed");
      status = -E_FAILURE;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);

   return status;
}

/**
   @brief
   Sample code for AES XTS performance tests
*/
int tz_app_aes_xts_perf(void)
{
   uint32 size;
   uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
   uint32 i = 0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;

   char name[] = "CE_TEST";
   QSEE_RESOURCE ce_clk = CE1;
   QSEE_CLKLEV voteon_freq = HIGH;
   QSEE_CLKLEV voteoff_freq = INACTIVE;

   /*----------------------------------------------------------------------------------
     vote for crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/

   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-on");
   total_time = qsee_get_uptime();
   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteon_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-on failed");
      status = -E_FAILURE;
      return status;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

   for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance AES128 Test - AES Algo: XTS Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_aes_xts_perf(QSEE_CIPHER_ALGO_AES_128,
                                             QSEE_CIPHER_MODE_XTS,
                                             aes_performance,
                                             size,
                                             aesxts128_key1_1,
                                             sizeof(aesxts128_key1_1),
                                             aesxts128_key2_1,
                                             sizeof(aesxts128_key2_1),
                                             0,
                                             aesxts128_iv_1);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance AES256 Test - AES Algo: XTS Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_aes_xts_perf(QSEE_CIPHER_ALGO_AES_256,
                                             QSEE_CIPHER_MODE_XTS,
                                             aes_performance,
                                             size,
                                             aesxts256_key1_4,
                                             sizeof(aesxts256_key1_4),
                                             aesxts256_key2_4,
                                             sizeof(aesxts256_key2_4),
                                             0,
                                             aesxts256_iv_4);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
   }

   /*----------------------------------------------------------------------------------
     vote off crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/
   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-off");
   total_time = qsee_get_uptime();

   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteoff_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-off failed");
      status = -E_FAILURE;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);

   return status;
}

/**
   @brief
   Sample code for HASH performance tests
*/
int tz_app_hash_perf(void)
{
   uint32 size;
   uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
   uint32 i = 0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;

   char name[] = "CE_TEST";
   QSEE_RESOURCE ce_clk = CE1;
   QSEE_CLKLEV voteon_freq = HIGH;
   //QSEE_CLKLEV voteoff_freq = INACTIVE;

   /*----------------------------------------------------------------------------------
     vote for crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/

   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-on");
   total_time = qsee_get_uptime();
   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteon_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-on failed");
      status = -E_FAILURE;
      return status;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

   for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance HASH Test - HASH Algo: SHA128 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_hash_perf(QSEE_HASH_SHA1,
                                          aes_performance,
                                          size);

         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_hash_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance HASH Test - HASH Algo: SHA256 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_hash_perf(QSEE_HASH_SHA256,
                                          aes_performance,
                                          size);

         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_hash_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
   }

   return status;
}

/**
   @brief
   Sample code for Simultaneous HASH/AES performance tests
*/
int tz_app_hash_aes_perf(void)
{
   uint32 size;
   uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
   uint32 i = 0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;

   char name[] = "CE_TEST";
   QSEE_RESOURCE ce_clk = CE1;
   QSEE_CLKLEV voteon_freq = HIGH;
   QSEE_CLKLEV voteoff_freq = INACTIVE;

   /*----------------------------------------------------------------------------------
     vote for crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/

   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-on");
   total_time = qsee_get_uptime();
   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteon_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-on failed");
      status = -E_FAILURE;
      return status;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

   //Init input PT data
   memset(aes_performance, 0xed, sizeof(aes_performance));

   for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Simultaneous AES128 CBC & HASH SHA1 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_hash_aes_perf(QSEE_HASH_SHA1,
                                              QSEE_CIPHER_ALGO_AES_128,
                                              QSEE_CIPHER_MODE_CBC,
                                              aes_performance,
                                              size,
                                              hash_aes_key_128,
                                              sizeof(hash_aes_key_128),
                                              hash_aes_iv_1);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_hash_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Simultaneous AES128 CBC & HASH SHA256 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_hash_aes_perf(QSEE_HASH_SHA256,
                                              QSEE_CIPHER_ALGO_AES_128,
                                              QSEE_CIPHER_MODE_CBC,
                                              aes_performance,
                                              size,
                                              hash_aes_key_128,
                                              sizeof(hash_aes_key_128),
                                              hash_aes_iv_1);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_hash_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Simultaneous AES256 CBC & HASH SHA1 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_hash_aes_perf(QSEE_HASH_SHA1,
                                              QSEE_CIPHER_ALGO_AES_256,
                                              QSEE_CIPHER_MODE_CBC,
                                              aes_performance,
                                              size,
                                              hash_aes_key_256,
                                              sizeof(hash_aes_key_256),
                                              hash_aes_iv_1);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_hash_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Simultaneous AES256 CBC & HASH SHA256 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_hash_aes_perf(QSEE_HASH_SHA256,
                                              QSEE_CIPHER_ALGO_AES_256,
                                              QSEE_CIPHER_MODE_CBC,
                                              aes_performance,
                                              size,
                                              hash_aes_key_256,
                                              sizeof(hash_aes_key_256),
                                              hash_aes_iv_1);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_hash_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
   } //end for

   /*----------------------------------------------------------------------------------
     vote off crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/
   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-off");
   total_time = qsee_get_uptime();

   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteoff_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-off failed");
      status = -E_FAILURE;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);

   return status;
}

/**
   @brief
   Sample code for HMAC performance tests
*/
int tz_app_hmac_perf(void)
{
   uint32 size;
   uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
   uint32 i = 0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;

   char name[] = "CE_TEST";
   QSEE_RESOURCE ce_clk = CE1;
   QSEE_CLKLEV voteon_freq = HIGH;
   QSEE_CLKLEV voteoff_freq = INACTIVE;

   /*----------------------------------------------------------------------------------
     vote for crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/

   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-on");
   total_time = qsee_get_uptime();
   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteon_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-on failed");
      status = -E_FAILURE;
      return status;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

   for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance HMAC Test - HMAC Algo: SHA128 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_hmac_perf(QSEE_HMAC_SHA1,
                                          aes_performance,
                                          size,
                                          sha_hmac_key_1,
                                          sizeof(sha_hmac_key_1));

         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_hmac_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance HMAC Test - HMAC Algo: SHA256 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_hmac_perf(QSEE_HMAC_SHA256,
                                          aes_performance,
                                          size,
                                          sha_hmac_key_1,
                                          sizeof(sha_hmac_key_1));

         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_hmac_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
   }
   /*----------------------------------------------------------------------------------
     vote off crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/
   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-off");
   total_time = qsee_get_uptime();

   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteoff_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-off failed");
      status = -E_FAILURE;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);

   return status;
}

/**
   @brief
   Sample code for CMAC performance tests
*/
int tz_app_cmac_perf(void)
{
   uint32 size;
   uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
   uint32 i = 0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;

   char name[] = "CE_TEST";
   QSEE_RESOURCE ce_clk = CE1;
   QSEE_CLKLEV voteon_freq = HIGH;
   QSEE_CLKLEV voteoff_freq = INACTIVE;

   /*----------------------------------------------------------------------------------
     vote for crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/

   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-on");
   total_time = qsee_get_uptime();
   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteon_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-on failed");
      status = -E_FAILURE;
      return status;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

   for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance CMAC Test - CMAC Algo: AES128 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_cmac_perf(QSEE_CMAC_ALGO_AES_128,
                                          aes_performance,
                                          size,
                                          aescmac128_key_1,
                                          sizeof(aescmac128_key_1));

         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_cmac_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance CMAC Test - CMAC Algo: AES256 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_crypto_cmac_perf(QSEE_CMAC_ALGO_AES_256,
                                          aes_performance,
                                          size,
                                          aescmac256_key_1,
                                          sizeof(aescmac256_key_1));

         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_cmac_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
   }

   /*----------------------------------------------------------------------------------
     vote off crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/
   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-off");
   total_time = qsee_get_uptime();

   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteoff_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-off failed");
      status = -E_FAILURE;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);

   return status;
}

/**
   @brief
   Sample code for KDF performance tests
*/
int tz_app_kdf_perf(void)
{
   uint32 size;
   uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
   uint32 i = 0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;
   uint8 *hmac_key = NULL;
   uint32 key_sizes[] = {16,32};
   uint32 data_set = sizeof(key_sizes)/4;
   static char key_label[] = {"I am deriving a key"};
   static char key_context[] = {"To verify the kdf working"};
   uint8 key_input[] = {0x12,0x23,0x56,0x73,
                        0xA3,0xff,0xEf,0xAB,
                        0xCD,0x1F,0x56,0xEF,
                        0xA3,0xff,0xEf,0xAB,
                        0xCD,0x1F,0x56,0xEF,
                        0x12,0x23,0x56,0x73,
                        0xCD,0x1F,0x56,0xEF,
                        0xCD,0x1F,0x56,0xEF,
                        0x12,0x23,0x56,0x73,
                        0xCD,0x1F,0x56,0xEF,
                        0xCD,0x1F,0x56,0xEF,
                        0xA3,0xff,0xEf,0xAB};

   char name[] = "CE_TEST";
   QSEE_RESOURCE ce_clk = CE1;
   QSEE_CLKLEV voteon_freq = HIGH;
   QSEE_CLKLEV voteoff_freq = INACTIVE;

   /*----------------------------------------------------------------------------------
     vote for crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/

   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-on");
   total_time = qsee_get_uptime();
   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteon_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-on failed");
      status = -E_FAILURE;
      return status;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning on clocks: %u ms", total_time);

   for(size = 0; size < data_set; size++)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance KDF Test, NULL key - Data Size: %u", key_sizes[size]);

      hmac_key = qsee_malloc(key_sizes[size]);
      if(NULL == hmac_key)
      {
         status = E_NO_MEMORY;
         return status;
      }

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = qsee_kdf(NULL, key_sizes[size],
                           (void*)(uintnt)(&key_label), strlen(key_label),
                           (void*)(uintnt)(&key_context), strlen(key_context),
                           hmac_key, key_sizes[size]);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_kdf_perf test FAILED for iteration for NULL key: %u", i);
            qsee_free(hmac_key);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)key_sizes[size] / ((float)total_time / (float)num)) * 1000.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u Bps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance KDF Test, user input key - Data Size: %u", key_sizes[size]);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = qsee_kdf((void*)(uintnt)key_input, key_sizes[size],
                           (void*)(uintnt)(&key_label), strlen(key_label),
                           (void*)(uintnt)(&key_context), strlen(key_context),
                           hmac_key, key_sizes[size]);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_kdf_perf test FAILED for iteration for user input key: %u", i);
            qsee_free(hmac_key);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)key_sizes[size] / ((float)total_time / (float)num)) * 1000.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u Bps", tput);
      qsee_free(hmac_key);
   }

   /*----------------------------------------------------------------------------------
     vote off crypto/snoc/bimc bandwidth
     ------------------------------------------------------------------------------------*/
   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_set_bandwidth vote-off");
   total_time = qsee_get_uptime();

   if (E_SUCCESS == status &&
       qsee_set_bandwidth(name, (sizeof(name)/sizeof(name[0]))-1, (uint32)ce_clk,
                          (uint32)voteoff_freq, 0) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_set_bandwidth vote-off failed");
      status = -E_FAILURE;
   }
   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for turning off clocks: %u ms", total_time);

   return status;
}


/**
 * @brief 
 *  Sample test code for device_KDF 
 *  Following test are conducted :
 *  1. For same user context generated the data twice and compare.
 *  2. generate the key for different user context , output should not be same.
 */
int tz_app_crypto_device_kdf(void)
{
  uint32 status = E_FAILURE;
  uint8 *derived_key = NULL, *derived_key_ver = NULL,*derived_key_kdf= NULL;
  uint32 i = 0,err=0;
  uint32 key_sizes[] = {16,32};
  uint32 num = sizeof(key_sizes)/4;
  static char key_context[] = {"To verify the kdf working"};
  static char key_context1[] = {"To verify th kdf working"};
  size_t output_lenout;
  Object s_crypto={NULL ,NULL} ;
  
  for(i = 0; i < num; i++)
  {
    derived_key = qsee_malloc(key_sizes[i]);
    if(NULL == derived_key)
    {
      status = E_NO_MEMORY;
      return status;
    }

    derived_key_ver = qsee_malloc(key_sizes[i]);
    if(NULL == derived_key_ver)
    {
      qsee_free(derived_key);
      status = E_NO_MEMORY;
      return status;
    }

    derived_key_kdf = qsee_malloc(key_sizes[i]);
    if(NULL == derived_key_kdf)
    {
      qsee_free(derived_key_ver);
      qsee_free(derived_key);
      status = E_NO_MEMORY;
      return status;
    }
    err = qsee_open(CCrypto_UID, &s_crypto);
    if (!Object_isOK(err) || Object_isNull(s_crypto))
    {
      qsee_log(QSEE_LOG_MSG_DEBUG, "tz_app_crypto_device_kdf() qsee_open() failed");
      qsee_free(derived_key);
      qsee_free(derived_key_ver);
      qsee_free(derived_key_kdf);
      return E_FAILURE;
    }

    /*generating key first time*/
    status = ICrypto_device_kdf(s_crypto,(void*)(uintnt)(&key_context), strlen(key_context),
                                derived_key, key_sizes[i],
                                &output_lenout);
    if (status != Object_OK) 
    {
      qsee_log(QSEE_LOG_MSG_DEBUG,"round1 : create Device_KDF test FAILED for iteration: %u", i);
      goto err;
    }

    qsee_log(QSEE_LOG_MSG_DEBUG,"Device_KDF key derived_key\n");
    {
      uint32 j = 0;
      for(j = 0; j < key_sizes[i]; j++)
      {
        qsee_log(QSEE_LOG_MSG_DEBUG,"0x%x,",derived_key[j]);
      }
    }

    /*generating key second time for same input*/
    status = ICrypto_device_kdf(s_crypto,(void*)(uintnt)(&key_context), strlen(key_context),
                                derived_key_ver, key_sizes[i],
                                &output_lenout);
    if (status != Object_OK) 
    {
      qsee_log(QSEE_LOG_MSG_DEBUG,"round1 : create1 Device_KDF test FAILED for iteration: %u", i);
      goto err;
    }

    /*generating key with different context string */
    status = ICrypto_device_kdf(s_crypto,(void*)(uintnt)(&key_context1), strlen(key_context1),
                               derived_key_kdf, key_sizes[i],
                               &output_lenout);
    if (status != Object_OK) 
    {
      qsee_log(QSEE_LOG_MSG_DEBUG,"round1 :create2 Device_KDF test FAILED for iteration: %u", i);
      goto err;
    }
    qsee_log(QSEE_LOG_MSG_DEBUG,"Device_KDF key derived_key_kdf \n");
    {
      uint32 j = 0;
      for(j = 0; j < key_sizes[i]; j++)
      {
        qsee_log(QSEE_LOG_MSG_DEBUG,"0x%x,",derived_key_kdf[j]);
      }
    }

    if(E_SUCCESS != app_crypto_timesafe_memcmp(derived_key,derived_key_ver,key_sizes[i]))
    {
      status = E_FAILURE;
      qsee_log(QSEE_LOG_MSG_DEBUG,"round1 : Device_KDF verify test FAILED for iteration: %u", i);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Device_KDF key derived_key_ver\n");
      {
        uint32 j = 0;
        for(j = 0; j < key_sizes[i]; j++)
        {
          qsee_log(QSEE_LOG_MSG_DEBUG,"0x%x,",derived_key_ver[j]);
        }
      }
		
      qsee_log(QSEE_LOG_MSG_DEBUG,"Device_KDF key derived_key \n");
      {
        uint32 j = 0;
        for(j = 0; j < key_sizes[i]; j++)
        {
          qsee_log(QSEE_LOG_MSG_DEBUG,"0x%x,",derived_key[j]);
        }
      }
      goto err;
    }

    /*this will compare that key should be different for different user context*/
    if(E_SUCCESS == app_crypto_timesafe_memcmp(derived_key,derived_key_kdf,key_sizes[i]))
    {
      status = E_FAILURE;
      qsee_log(QSEE_LOG_MSG_DEBUG,"round1 :  Device_KDF verify test FAILED for iteration: %u", i);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Device_KDF key derived_key_ver\n");
      {
        uint32 j = 0;
        for(j = 0; j < key_sizes[i]; j++)
        {
          qsee_log(QSEE_LOG_MSG_DEBUG,"0x%x,",derived_key_ver[j]);
        }
      }
		
      qsee_log(QSEE_LOG_MSG_DEBUG,"Device_KDF key derived_key_kdf \n");
      {
        uint32 j = 0;
        for(j = 0; j < key_sizes[i]; j++)
        {
          qsee_log(QSEE_LOG_MSG_DEBUG,"0x%x,",derived_key_kdf[j]);
        }
      }
      goto err;
    }
 
    qsee_log(QSEE_LOG_MSG_DEBUG,"Device_KDF round1 passed for iteration: %u",i);
    ICrypto_release(s_crypto) ;
    qsee_free(derived_key);
    qsee_free(derived_key_ver);
    qsee_free(derived_key_kdf);
  }  
  return status;
err:
  qsee_free(derived_key);
  qsee_free(derived_key_ver);
  qsee_free(derived_key_kdf);
  ICrypto_release(s_crypto) ;
  return status;
}


/**
 * @brief
 *  Sample code for KDF
 */

int tz_app_crypto_kdf(void)
{
   uint32 status = E_FAILURE;
   uint8 *derived_key = NULL, *derived_key_ver = NULL;
   uint32 i = 0;
   uint32 key_sizes[] = {16,32};
   uint32 num = sizeof(key_sizes)/4;
   static char key_label[] = {"I am deriving a key"};
   static char key_context[] = {"To verify the kdf working"};
   uint8 key_input[] = {0x12,0x23,0x56,0x73,
                        0xA3,0xff,0xEf,0xAB,
                        0xCD,0x1F,0x56,0xEF,
                        0xA3,0xff,0xEf,0xAB,
                        0xCD,0x1F,0x56,0xEF,
                        0x12,0x23,0x56,0x73,
                        0xCD,0x1F,0x56,0xEF,
                        0xCD,0x1F,0x56,0xEF,
                        0x12,0x23,0x56,0x73,
                        0xCD,0x1F,0x56,0xEF,
                        0xCD,0x1F,0x56,0xEF,
                        0xA3,0xff,0xEf,0xAB};

   for(i = 0; i < num; i++)
   {
      derived_key = qsee_malloc(key_sizes[i]);
      if(NULL == derived_key)
      {
         status = E_NO_MEMORY;
         return status;
      }

      derived_key_ver = qsee_malloc(key_sizes[i]);
      if(NULL == derived_key_ver)
      {
         qsee_free(derived_key_ver);
         status = E_NO_MEMORY;
         return status;
      }

      status = qsee_kdf(NULL, key_sizes[i],
                        (void*)(uintnt)(&key_label), strlen(key_label),
                        (void*)(uintnt)(&key_context), strlen(key_context),
                        derived_key, key_sizes[i]);
      if (status != E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"round1 : create1 KDF test FAILED for iteration: %u", i);
         goto err;
      }

      status = qsee_kdf(NULL, key_sizes[i],
                        (void*)(uintnt)(&key_label), strlen(key_label),
                        (void*)(uintnt)(&key_context), strlen(key_context),
                        derived_key_ver, key_sizes[i]);
      if (status != E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"round1 : create2 KDF test FAILED for iteration: %u", i);
         goto err;
      }

    if(E_SUCCESS != app_crypto_timesafe_memcmp(derived_key,derived_key_ver,key_sizes[i]))
      {
         status = E_FAILURE;
         qsee_log(QSEE_LOG_MSG_ERROR,"round1 : KDF verify test FAILED for iteration: %u", i);
         goto err;
      }

      qsee_log(QSEE_LOG_MSG_DEBUG,"KDF key\n");
      {
         uint32 j = 0;
         for(j = 0; j < key_sizes[i]; j++)
         {
            qsee_log(QSEE_LOG_MSG_DEBUG,"0x%x,",derived_key[j]);
         }
      }

      qsee_log(QSEE_LOG_MSG_DEBUG,"KDF round1 passed for iteration: %u", i);

      qsee_free(derived_key);
      qsee_free(derived_key_ver);
   }


   for(i = 0; i < num; i++)
   {
      derived_key = qsee_malloc(key_sizes[i]);
      if(NULL == derived_key)
      {
         status = E_NO_MEMORY;
         return status;
      }

      derived_key_ver = qsee_malloc(key_sizes[i]);
      if(NULL == derived_key_ver)
      {
         qsee_free(derived_key_ver);
         status = E_NO_MEMORY;
         return status;
      }

      status = qsee_kdf((void*)(uintnt)key_input, key_sizes[i],
                        (void*)(uintnt)(&key_label), strlen(key_label),
                        (void*)(uintnt)(&key_context), strlen(key_context),
                        derived_key, key_sizes[i]);
      if (status != E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"round2 : create1 KDF test FAILED for iteration: %u", i);
         goto err;
      }

      status = qsee_kdf((void*)(uintnt)key_input, key_sizes[i],
                        (void*)(uintnt)(&key_label), strlen(key_label),
                        (void*)(uintnt)(&key_context), strlen(key_context),
                        derived_key_ver, key_sizes[i]);
      if (status != E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"round2 : create2 KDF test FAILED for iteration: %u", i);
         goto err;
      }

    if(E_SUCCESS != app_crypto_timesafe_memcmp(derived_key,derived_key_ver,key_sizes[i]))
      {
         status = E_FAILURE;
         qsee_log(QSEE_LOG_MSG_ERROR,"round2 : KDF verify test FAILED for iteration: %u", i);
         goto err;
      }

      qsee_log(QSEE_LOG_MSG_DEBUG,"KDF round2 passed for iteration: %u", i);

      qsee_free(derived_key);
      qsee_free(derived_key_ver);
   }
   return status;
err:
   qsee_free(derived_key);
   qsee_free(derived_key_ver);
   return status;
}


/**
   @brief
   Sample code for how to call ECDH crypto API's
*/
int tz_app_crypto_ecdh_func(boolean injecterror)
{
   uint32 j;
   boolean error_occurred = FALSE;
   int rv;
   QSEE_bigval_t alicePrivate, bobPrivate;
   QSEE_affine_point_t alicePublic, bobPublic, aliceFinal, bobFinal;


   rv = qsee_SW_ECC_PubPrivate_Key_generate(&alicePublic, &alicePrivate );
   if (rv) {
      return -E_FAILURE;
   }

   rv = qsee_SW_ECC_PubPrivate_Key_generate(&bobPublic, &bobPrivate);
   if (rv) {
      return -E_FAILURE;
   }

   rv = qsee_SW_ECDH_Shared_Key_Derive (&aliceFinal, &alicePrivate, &bobPublic);
   if (rv) {
      return -E_FAILURE;
   }

   if (injecterror) {
      if (qsee_toggle_random_bit(&bobPrivate, sizeof(bobPrivate) - sizeof(int))) {
         return -E_FAILURE;
      }
   }

   rv = qsee_SW_ECDH_Shared_Key_Derive(&bobFinal, &bobPrivate, &alicePublic);
   if (rv) {
      return -E_FAILURE;
   }

   if (!qsee_in_curveP(&aliceFinal)) {
      return -E_FAILURE;
   }
   if (!qsee_in_curveP(&bobFinal)) {
      return -E_FAILURE;
   }

   for (j = 0; j < sizeof (QSEE_affine_point_t) / sizeof (int); ++j) {
      if (((int *)(uintnt)&aliceFinal)[j] != ((int *)(uintnt)&bobFinal)[j]) {
         error_occurred = TRUE;
      }
   }

   if (injecterror) {
      if (!error_occurred) {
         return -E_FAILURE;
      }
   } else {
      if (error_occurred) {
         return -E_FAILURE;
      }
   }

   return E_SUCCESS;
}

/**
   @brief
   Sample code for how to call ECDSA crypto API's
*/
int tz_app_crypto_ecdsa_func(boolean injecterror, int dgstlen)
{
   uint8 *dgst;
   int rv;
   int rv_2;
   QSEE_affine_point_t dmjpubkey;
   QSEE_bigval_t       msgdgst;
   QSEE_bigval_t       dmjprivkey;
   QSEE_ECDSA_sig_t    dmjsig;

   rv = qsee_SW_ECC_PubPrivate_Key_generate(&dmjpubkey, &dmjprivkey);

   if (rv) {
      return -E_FAILURE;
   }

   /* generate message hash */
   dgst = (uint8 *)qsee_malloc(dgstlen);

   if (dgst == NULL) {
      return -E_NO_MEMORY;
   }

   rv_2 = qsee_get_random_bytes(dgst, dgstlen);
   if (rv_2 != 0) {
      return -E_FAILURE;
   }

   qsee_ECC_hash_to_bigval(&msgdgst, dgst, dgstlen);

   rv = qsee_SW_ECDSA_Sign(&msgdgst, &dmjprivkey, &dmjsig);
   if (rv) {
      return -E_FAILURE;
   }

   if (injecterror) {
      qsee_toggle_random_bit(dgst, MIN(dgstlen, 4*(QSEE_BIGLEN-1)-1));
      qsee_ECC_hash_to_bigval(&msgdgst, dgst, dgstlen);
   }

   rv = qsee_SW_ECDSA_Verify(&msgdgst, &dmjpubkey, &dmjsig);

   if ((rv == -1)^injecterror) {
      if (!injecterror) {
         return -E_FAILURE;
      } else {
         return -E_FAILURE;
      }
   }

   qsee_free(dgst);

   return E_SUCCESS;
}

static int tzp_app_ecies_get_ecc_generic_keys(generic_ecc_vector_type *data_ptr, QSEE_qrlbn_ecc_affine_point_t *public_key, QSEE_qrlbn_ecc_bigval_t *private_key)
{
	QSEE_qrlbn_ecc_domain_t        domain;
	int                            rv  = 0;

	//Init ECC curve paramters
	rv = qsee_SW_GENERIC_ECC_init(&domain, data_ptr->m, data_ptr->a, data_ptr->b, data_ptr->G_x, data_ptr->G_y, data_ptr->n, 1);
	if (rv)
	{
	  qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_init API failed");
	  return -E_FAILURE;
	}

	//Generate Public/Private key pair b
	rv = qsee_SW_GENERIC_ECC_keypair_generate(private_key, public_key, &domain);
	if (rv)
	{
	   qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_SW_GENERIC_ECC_keypair_generate API failed to generate pair b");
	   return -E_FAILURE;
	}

	return rv;
}

#define CIPH_SIZE 380

/**
  @brief 
    Sample code for how to call ECIES crypto API's
*/
static int tz_app_crypto_single_ecies_func(QSEE_ecies_params_t *params)
{
	int            err = 0;
	int            i = 0;
	uint8_t      * deciphered = NULL;
	uint32_t       deciphered_len = 0;
	uint8_t      * encrypted = NULL;
	uint32_t       encrypted_len = 0;
	uint8_t      * outAD = NULL;
	uint32_t       outAD_len = 0;
	QSEE_ecies_ctx_t ecies_ctx;
	QSEE_ecies_key_t ecies_key;


	QSEE_qrlbn_ecc_bigval_t  private_key;
	QSEE_qrlbn_ecc_affine_point_t public_key;

	uint8_t        message[] = "What do you call an alligator in a vest? An investigator.";
	uint32_t       message_len = strlen((char*)&(message[0]));
	uint8_t        AD[] = "Knock knock!";
	uint32_t       AD_len = strlen((char*)&(AD[0]));

	uint8_t        cryptogram[CIPH_SIZE];
	uint32_t       cryptogram_len = CIPH_SIZE;
	uint8_t        ecc_to_ecies_vector[4] = {2, 3, 4, 5}; /* convert generic_ecc_test_vector[] index to QSEE_ecies_curve_t index */

	err = tzp_app_ecies_get_ecc_generic_keys(&generic_ecc_test_vectors[ecc_to_ecies_vector[params->ecies_curve]], &public_key, &private_key);

    encrypted = qsee_malloc(cryptogram_len);
    if (encrypted == NULL)
    {
        qsee_log(QSEE_LOG_MSG_ERROR,"Error: message malloc failed.{%d}", cryptogram_len);
        err = -1;
        goto test_exit;
    }
    encrypted_len = cryptogram_len;

    // encryption

	err = qsee_SW_ECIES_init(&ecies_ctx, params);

	if (err != 0) {
		qsee_log(QSEE_LOG_MSG_ERROR,"Error in ECIES init (%x)", err);
		return err;
	}

	ecies_key.enc_dec.recipient_public_key = & public_key;

	// encrypt

	err = qsee_SW_ECIES_update(
		  & ecies_ctx,
		  & ecies_key,        // sender's public key
		  QSEE_ECIES_PURPOSE_ENCRYPT,
		  message,
		  message_len,
		  & outAD,
		  & outAD_len,
		  encrypted,           // pointer to received cryptogram
		  &encrypted_len);       // address of location where associated data length will be stored
	if (err)
	{
		qsee_log(QSEE_LOG_MSG_ERROR,"Error (%d) while encrypting!\n", err);
		goto test_exit;
	}

	err = qsee_SW_ECIES_finish(&ecies_ctx);
	if (err)
	{
		qsee_log(QSEE_LOG_MSG_ERROR,"Error (%d) while finishing!\n", err);
		goto test_exit;
	}

    deciphered = qsee_malloc(cryptogram_len);
    if (deciphered == NULL)
    {
        qsee_log(QSEE_LOG_MSG_ERROR,"Error: message malloc failed.{%d}", cryptogram_len);
        err = -1;
        goto test_exit;
    }
    deciphered_len = cryptogram_len;

	err = qsee_SW_ECIES_init(&ecies_ctx, params);

	if (err != 0) {
		qsee_log(QSEE_LOG_MSG_ERROR,"Error in ECIES init (%x)", err);
		return err;
	}

	// decrypt

	ecies_key.enc_dec.private_key = & private_key;

	err = qsee_SW_ECIES_update(
		  & ecies_ctx,
		  & ecies_key,        // recipient's private key
		  QSEE_ECIES_PURPOSE_DECRYPT,
		  encrypted,
		  encrypted_len,
		  & outAD,
		  & outAD_len,
		  deciphered,           // pointer to received cryptogram
		  &deciphered_len);       // address of location where associated data length will be stored
	if (err)
	{
		qsee_log(QSEE_LOG_MSG_ERROR,"Error (%x) while decrypting!\n", err);
		goto test_exit;
	}

	err = qsee_SW_ECIES_finish(&ecies_ctx);
	if (err)
	{
		qsee_log(QSEE_LOG_MSG_ERROR,"Error (%d) while finishing!\n", err);
		goto test_exit;
	}

	// qsee_log(QSEE_LOG_MSG_ERROR,"decrypted msg: %s", deciphered);

test_exit:

    if (encrypted)
    {
        qsee_free (encrypted);
        encrypted = NULL;
    }

    if (err)
    {
        qsee_log(QSEE_LOG_MSG_ERROR,"ecies with a given input Failed. {%d}", err);
    }

    if (deciphered)
    {
        qsee_free (deciphered);
        deciphered = NULL;
    }

	return err;
}

/**
  @brief
    Sample code for how to call ECIES crypto API's
*/
int tz_app_crypto_ecies_func(void)
{
	int            err = 0;
	int            i_kdf, i_digest, i_aes, i_ecurve;
	QSEE_ecies_params_t params;

	for (i_kdf = QSEE_ECIES_KDF_RFC5869_SHA256; i_kdf <= QSEE_ECIES_KDF_ISO18033_2_KDF2_SHA256; i_kdf++) {
		for (i_digest = QSEE_ECIES_DIGEST_MD5; i_digest <= QSEE_ECIES_DIGEST_SHA_2_512; i_digest ++) {
			for (i_aes = QSEE_ECIES_AES_128; i_aes <= QSEE_ECIES_AES_256; i_aes ++) {
				for (i_ecurve = QSEE_ECIES_CURVE_P_224; i_ecurve <= QSEE_ECIES_CURVE_P_521; i_ecurve ++) {
					params.balg = i_aes;
					params.digest = i_digest;
					params.ecies_curve = i_ecurve;
					params.kdf = i_kdf;
					err = tz_app_crypto_single_ecies_func(&params);
					if (err) {
						qsee_log(QSEE_LOG_MSG_ERROR,"fail of ecies, err = %x balg = %d digest = %d ecurve = %d kdf = %d",
								err, params.balg, params.digest, params.ecies_curve, params.kdf);
						break;
					}
				}
			}
		}
	}

	if (!err)
		qsee_log(QSEE_LOG_MSG_DEBUG, "ECIES test passed\n");

	return err;
}

static uint8_t *get_invalid_pvt_key(uint32_t key_size, uint32_t index) {
  switch(key_size)
  {
    case 160:
      return ecc_160_private_key_invalid[index];
    case 192:
      return ecc_192_private_key_invalid[index];
    case 224:
      return ecc_224_private_key_invalid[index];
    case 256:
      return ecc_256_private_key_invalid[index];
    case 384:
      return ecc_384_private_key_invalid[index];
    case 521:
      return ecc_521_private_key_invalid[index];
    default:
      break;
  }
  return NULL;
}

/**
   @brief
   Sample code for how to call Generic ECDSA PubKey Generate crypto API
*/
static int tz_app_crypto_generic_ecdsa_pubkey_gen(generic_ecc_vector_type *data_ptr)
{
   QSEE_qrlbn_ecc_affine_point_t  public_key = {0};
   QSEE_qrlbn_ecc_bigval_t        private_key = {0};
   QSEE_qrlbn_ECDSA_sig_t         signature_data = {0};
   QSEE_qrlbn_ecc_domain_t        *domain = NULL;
   int                            rv  = 0;
   int                            rv2 = 0;
   int                            rv3 = 0;
   uint32_t                       temp;
   uint32_t                       i;
   uint32_t                       t;
   size_t                         key_size_in_bytes = 0;
   uint8_t                        pvtkey[_ECC_BIGNUM_WORDS * 4] = {0};
   void                          *rp = NULL; 
   generic_ecc_vector_type        *binary_to_compare = NULL;

   if (!data_ptr || !data_ptr->public_key_x || !data_ptr->public_key_y || !data_ptr->private_key || !data_ptr->n ||
      !data_ptr->hash || !data_ptr->m || !data_ptr->a || !data_ptr->b || !data_ptr->G_x || !data_ptr->G_y ) {
     qsee_log(QSEE_LOG_MSG_ERROR, "Invalid input test vector");
     return -E_FAILURE;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG, "ECDSA with keylen: %u", data_ptr->key_size);
   key_size_in_bytes = (data_ptr->key_size + 7) / 8;

   do 
   {
      domain = (QSEE_qrlbn_ecc_domain_t *)qsee_malloc(sizeof(QSEE_qrlbn_ecc_domain_t));
      if (domain == NULL) {
         qsee_log(QSEE_LOG_MSG_ERROR, "domain malloc failed");
         rv = -E_FAILURE;
         break;
      }

      //Init ECC curve parameters
      rv = qsee_SW_GENERIC_ECC_init(domain, data_ptr->m, data_ptr->a, data_ptr->b, data_ptr->G_x, data_ptr->G_y, data_ptr->n, 1);
      if (rv) {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_init API failed");
         rv = -E_FAILURE;
         break;
      }

      // Validate what is in the domain
      if (QRLBN_ECC_MODULUS_CHECK_FAIL(&domain->base_point_order)) {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_init returned domain failed validation");
         rv = -E_FAILURE;
         break;
      }

      //Convert private_key binary data to private.key.data Bigval
      rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&private_key, data_ptr->private_key, key_size_in_bytes,
                                                domain, QSEE_qrlbn_tag_privkey);
      if (rv) {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed");
         rv = -E_FAILURE;
         break;
      }

      //Generate signature
      rv = qsee_SW_GENERIC_ECDSA_sign(data_ptr->hash, data_ptr->hash_size, &private_key, &signature_data, domain);
      if (rv) {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDSA_Sign API failed to generate signature");
         rv = -E_FAILURE;
         break;
      }

      // Generate public key (need to be in bigval format to use for verify)
      rv = qsee_SW_GENERIC_ECC_pubkey_generate(&private_key, &public_key, domain);
      if (rv) {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_pubkey_generate API failed to generate pubkey");
         rv = -E_FAILURE;
         break;
      }

      binary_to_compare = (generic_ecc_vector_type *) qsee_malloc(sizeof(generic_ecc_vector_type));
      if (binary_to_compare == NULL) {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_bigval_to_binary failed to malloc test comparison");
         rv = -E_FAILURE;
         break;
      }

      binary_to_compare->key_size = data_ptr->key_size;
      binary_to_compare->public_key_x = qsee_malloc(key_size_in_bytes);
      binary_to_compare->public_key_y = qsee_malloc(key_size_in_bytes);
      if (binary_to_compare->public_key_x == NULL || binary_to_compare->public_key_y == NULL) {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_bigval_to_binary failed to malloc x or y for binary_to_compare");
         rv = -E_FAILURE;
         break;
      }
      // Convert Bigval pubkey_y to binary and compare
      rv = qsee_SW_GENERIC_ECC_bigval_to_binary(binary_to_compare->public_key_x, key_size_in_bytes, &public_key.x, domain, QSEE_qrlbn_tag_X);
      if (rv) {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_bigval_to_binary API failed on key x");
         rv = -E_FAILURE;
         break;
      }
      rv = qsee_SW_GENERIC_ECC_bigval_to_binary(binary_to_compare->public_key_y, key_size_in_bytes, &public_key.y, domain, QSEE_qrlbn_tag_Y);
      if (rv) {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_bigval_to_binary API failed on key y");
         rv = -E_FAILURE;
         break;
      }

      // compare the generated and provided public key
      if(binary_to_compare->public_key_x == NULL || app_crypto_timesafe_memcmp(binary_to_compare->public_key_x, data_ptr->public_key_x, key_size_in_bytes)) {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_bigval_to_binary API failed on key y binary comparison");
         rv = -E_FAILURE;
         break;
      }
      if(binary_to_compare->public_key_y == NULL || app_crypto_timesafe_memcmp(binary_to_compare->public_key_y, data_ptr->public_key_y, key_size_in_bytes)) {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_bigval_to_binary API failed on key y binary comparison");
         rv = -E_FAILURE;
         break;
      }

      qsee_log(QSEE_LOG_MSG_DEBUG,"Comparing generated public key - pass");

      //Now verify the generated signature
      rv = qsee_SW_GENERIC_ECDSA_verify(data_ptr->hash, data_ptr->hash_size, &public_key, &signature_data, domain);
      if (rv) {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDSA_Verify API failed to verify signature");
         rv = -E_FAILURE;
         break;
      }

      //Adverse scenarios using invalid private keys
      // j=0 - private key greater than base point order
      // j=1 - private key zero
      // j=2 - only 2nd word of private key finite num, all other words zero
      for (int j=0; j<3; j++) {
        memset(pvtkey, 0, sizeof(pvtkey)); 
        rp =(void *)get_invalid_pvt_key(data_ptr->key_size, j);
        if(rp == NULL) 
        {
         qsee_log(QSEE_LOG_MSG_ERROR,"get_invalid_pvt_key failed");
         rv = -E_FAILURE;
         break; 
        }

        memscpy(pvtkey, sizeof(pvtkey), rp, key_size_in_bytes);

        rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&private_key, pvtkey, key_size_in_bytes,
                                                domain, QSEE_qrlbn_tag_privkey);
        if (rv) {
           qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed");
           rv = -E_FAILURE;
           break;
        }
        if (data_ptr->key_size == 160 && j == 0)
          private_key.data[data_ptr->key_size/32] = 1;
        
        // Generate public key (must fail)
        rv2 = qsee_SW_GENERIC_ECC_pubkey_generate(&private_key, &public_key, domain);
        if (!rv2) {
           qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_pubkey_generate generated pubkey for invalid pvtkey - fail");
           rv = -E_FAILURE;
           break;
        }
        qsee_log(QSEE_LOG_MSG_DEBUG, "ecc generate pubkey found invalid private_key - pass");
      }
    } while(0);

   // cleanup
   if (binary_to_compare) {
  	   if (binary_to_compare->public_key_x)
         qsee_free(binary_to_compare->public_key_x);
      if (binary_to_compare->public_key_y)
         qsee_free(binary_to_compare->public_key_y);
      qsee_free(binary_to_compare);
   }
   if (domain)
      qsee_free(domain);
   return rv;
}  

/**
   @brief
   Sample code for how to call Generic ECDSA crypto API's
*/
int tz_app_crypto_generic_ecdsa_func(generic_ecc_vector_type *data_ptr)
{
   QSEE_qrlbn_ecc_affine_point_t  public_key = {0};
   QSEE_qrlbn_ecc_bigval_t        private_key = {0};
   QSEE_qrlbn_ECDSA_sig_t         signature_data = {0};
   QSEE_qrlbn_ecc_domain_t        *domain = NULL;
   int                            rv  = 0;
   int                            rv2 = 0;
   int                            rv3 = 0;
   uint32                         temp;
   uint32                         i;
   uint32                         t;
   generic_ecc_vector_type        *binary_to_compare = NULL;

   qsee_log(QSEE_LOG_MSG_DEBUG, "ECDSA with keylen: %u", data_ptr->key_size);

   do
   {
      domain = (QSEE_qrlbn_ecc_domain_t *)qsee_malloc(sizeof(QSEE_qrlbn_ecc_domain_t));
      if (domain == NULL)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "domain malloc failed");
         rv = -E_FAILURE;
         break;
      }

      //Init ECC curve parameters
      rv = qsee_SW_GENERIC_ECC_init(domain, data_ptr->m, data_ptr->a, data_ptr->b, data_ptr->G_x, data_ptr->G_y, data_ptr->n, 1);
      if (rv)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_init API failed");
         rv = -E_FAILURE;
         break;
      }

      // Validate what is in the domain
      if (QRLBN_ECC_MODULUS_CHECK_FAIL(&domain->base_point_order))
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_init returned domain failed validation");
         rv = -E_FAILURE;
         break;
      }

      //Convert public_key_x binary data to public_key.x Bigval
      rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&public_key.x, data_ptr->public_key_x, (data_ptr->key_size + 7) / 8,
                                                domain, QSEE_qrlbn_tag_X);
      if (rv)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed on x");
         rv = -E_FAILURE;
         break;
      }

      //Convert public_key_y binary binary to public_key.y Bigval
      rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&public_key.y, data_ptr->public_key_y, (data_ptr->key_size + 7) / 8, domain, QSEE_qrlbn_tag_Y);
      if (rv)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed on y");
         rv = -E_FAILURE;
         break;
      }

      binary_to_compare = (generic_ecc_vector_type *) qsee_malloc(sizeof(generic_ecc_vector_type));
      if (binary_to_compare == NULL)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_bigval_to_binary failed to malloc test comparison");
         rv = -E_FAILURE;
         break;
      }

      binary_to_compare->key_size = data_ptr->key_size;
      binary_to_compare->public_key_y = qsee_malloc((binary_to_compare->key_size + 7) / 8);
      if (binary_to_compare->public_key_y == NULL)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_bigval_to_binary failed to malloc test comparison y");
         rv = -E_FAILURE;
         break;
      }

      // Convert Bigval back to binary and compare
      rv = qsee_SW_GENERIC_ECC_bigval_to_binary(binary_to_compare->public_key_y, (binary_to_compare->key_size + 7) / 8, &public_key.y, domain, QSEE_qrlbn_tag_Y);
      if (rv)
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_bigval_to_binary API failed on key y");
         rv = -E_FAILURE;
         break;
      }

    if(binary_to_compare->public_key_y == NULL || app_crypto_timesafe_memcmp(binary_to_compare->public_key_y, data_ptr->public_key_y, (data_ptr->key_size + 7) / 8))
      {
         qsee_log(QSEE_LOG_MSG_ERROR, "qsee_SW_GENERIC_ECC_bigval_to_binary API failed on key y binary comparison");
         rv = -E_FAILURE;
         break;
      }

      //Convert signature binary data to signature_data.r Bigval. Signature r in first half of signature input data
      rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&signature_data.r, data_ptr->signature, data_ptr->sig_size / 2, domain, QSEE_qrlbn_tag_r);
      if (rv)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed first half");
         rv = -E_FAILURE;
         break;
      }

      //Convert signature binary data to signature_data.s Bigval. Signature s in second half of signature input data
      rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&signature_data.s, data_ptr->signature + (data_ptr->sig_size / 2),
                                                data_ptr->sig_size / 2, domain, QSEE_qrlbn_tag_s);
      if (rv)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed second half");
         rv = -E_FAILURE;
         break;
      }

      //Execute verify routine and verify signature - verify expects hash to be in binary format
      rv = qsee_SW_GENERIC_ECDSA_verify(data_ptr->hash, data_ptr->hash_size, &public_key, &signature_data, domain);
      if (rv)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDSA_Verify API failed to verify vector signature. Error %d", rv);
         rv = -E_FAILURE;
         break;
      }

      // Now twiddle one bit in the hash value. This should cause a failure.
      temp = data_ptr->hash[0];
      data_ptr->hash[0] ^= 1;

      //Verify vector data again. This time is should fail
      rv2 = qsee_SW_GENERIC_ECDSA_verify(data_ptr->hash, data_ptr->hash_size, &public_key, &signature_data, domain);
      if (!rv2)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDSA_Verify API passed where it should have failed");
         rv = -E_FAILURE;
         break;
      }

      //Restore hash data
      data_ptr->hash[0] = temp;

      //Convert private_key binary data to private.key.data Bigval
      rv = qsee_SW_GENERIC_ECC_binary_to_bigval(&private_key, data_ptr->private_key, (data_ptr->key_size + 7) / 8,
                                                domain, QSEE_qrlbn_tag_privkey);
      if (rv)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_Binary_to_Bigval API failed");
         rv = -E_FAILURE;
         break;
      }

      //Generate signature
      rv = qsee_SW_GENERIC_ECDSA_sign(data_ptr->hash, data_ptr->hash_size, &private_key, &signature_data, domain);
      if (rv)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDSA_Sign API failed to generate signature");
         rv = -E_FAILURE;
         break;
      }

      //Now verify the previous generated signature
      rv = qsee_SW_GENERIC_ECDSA_verify(data_ptr->hash, data_ptr->hash_size, &public_key, &signature_data, domain);
      if (rv)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDSA_Verify API failed to verify signature");
         rv = -E_FAILURE;
         break;
      }

      // Now twiddle one bit in the hash value.
      data_ptr->hash[0] ^= 1;

      //Verify the previous signnature again. This time it should fail
      rv2 = qsee_SW_GENERIC_ECDSA_verify(data_ptr->hash, data_ptr->hash_size, &public_key, &signature_data, domain);
      if (!rv2)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDSA_Verify API passed when it should have failed to verify signature");
         rv = -E_FAILURE;
         break;
      }

      //Restore hash data
      data_ptr->hash[0] = temp;

      //Run ECC Random Hash Vector Test
      //Loop over number of test
      for (i=0; i < 10; i++)
      {
         //Generate random hash -- using current hash size to mix it up each run -- make sure buffer is big enough
         rv3 = qsee_get_random_bytes(hash_binary, data_ptr->hash_size);
         if (rv3)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_get_random_bytes API failed pass %u", i);
            rv = -E_FAILURE;
            break;
         }

         //Generate Public/Private key pair
         rv = qsee_SW_GENERIC_ECC_keypair_generate(&private_key, &public_key, domain);
         if (rv)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_PubPrivate_Key_generate API failed pass %u", i);
            rv = -E_FAILURE;
            break;
         }

         //Generate signature using Random hash vecotr
         rv = qsee_SW_GENERIC_ECDSA_sign(hash_binary, data_ptr->hash_size, &private_key, &signature_data, domain);
         if (rv)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDSA_Sign API failed pass %u", i);
            rv = -E_FAILURE;
            break;
         }

         //Now verify the previous generated signature
         rv = qsee_SW_GENERIC_ECDSA_verify(hash_binary, data_ptr->hash_size, &public_key, &signature_data, domain);
         if (rv)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDSA_Verify API failed pass %u", i);
            rv = -E_FAILURE;
            break;
         }

         // Now twiddle one bit in the hash value.
         temp = hash_binary[0];
         hash_binary[0] ^= 1;

         //Now verify the previous generated signature. This time it should fail
         rv2 = qsee_SW_GENERIC_ECDSA_verify(hash_binary, data_ptr->hash_size, &public_key, &signature_data, domain);
         if (!rv2)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDSA_Verify API failed pass %u", i);
            rv = -E_FAILURE;
            break;
         }

         // twiddle back
         hash_binary[0] = temp;
         for(t = 0; t < QSEE_ECC_BIGNUM_WORDS; ++t) {
            signature_data.r.data[t] = 0x00;
         }
         // Now mess with the signature
         // Now verify the previous generated signature. This time it should fail
         rv2 = qsee_SW_GENERIC_ECDSA_verify(hash_binary, data_ptr->hash_size, &public_key, &signature_data, domain);
         if (!rv2)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDSA_Verify API failed pass %u", i);
            rv = -E_FAILURE;
            break;
         }
      }
   } while(0);

   if (binary_to_compare)
   {
      if (binary_to_compare->public_key_y)
      {
         qsee_free(binary_to_compare->public_key_y);
      }

      qsee_free(binary_to_compare);
   }
   if (domain)
   {
      qsee_free(domain);
   }

   return rv;
}

/**
   @brief
   Sample code for how to call Generic ECDH crypto API's
*/
int tz_app_crypto_generic_ecdh_func(generic_ecc_vector_type *data_ptr)
{
   QSEE_qrlbn_ecc_domain_t        domain;
   QSEE_qrlbn_ecc_affine_point_t  bob_public_key, alice_public_key;
   QSEE_qrlbn_ecc_bigval_t        bob_private_key, alice_private_key;
   QSEE_qrlbn_ecc_bigval_t        bob_shared_key, alice_shared_key;
   int                            rv  = 0;

   //Init ECC curve paramters
   rv = qsee_SW_GENERIC_ECC_init(&domain, data_ptr->m, data_ptr->a, data_ptr->b, data_ptr->G_x, data_ptr->G_y, data_ptr->n, 1);
   if (rv)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_init API failed");
      return -E_FAILURE;
   }

   //Generate Public/Private key pair b
   rv = qsee_SW_GENERIC_ECC_keypair_generate(&bob_private_key, &bob_public_key, &domain);
   if (rv)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_keypair_generate API failed to generate pair b");
      return -E_FAILURE;
   }

   //Generate Public/Private key pair a
   rv = qsee_SW_GENERIC_ECC_keypair_generate(&alice_private_key, &alice_public_key, &domain);
   if (rv)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_keypair_generate API failed to generate pair a");
      return -E_FAILURE;
   }

   rv = qsee_SW_GENERIC_ECDH_shared_key_derive(&alice_shared_key, &alice_private_key, &bob_public_key, &domain);
   if(rv)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDH_shared_key_derive API failed shared a");
      return -E_FAILURE;
   }

   rv = qsee_SW_GENERIC_ECDH_shared_key_derive(&bob_shared_key, &bob_private_key, &alice_public_key, &domain);
   if(rv)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECDH_shared_key_derive API failed shared b");
      return -E_FAILURE;
   }

   if(qsee_SW_GENERIC_ECC_compare(alice_shared_key.data, bob_shared_key.data, &((&domain)->modulus.md)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_compare API failed");
      return -E_FAILURE;
   }

  // Test affine point on curve
  if(!qsee_SW_GENERIC_ECC_affine_point_on_curve(&bob_public_key, &domain))
  {
    qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_affine_point_on_curve API failed on bob: not on curve!");
    return -E_FAILURE;
  } 
  
  if(!qsee_SW_GENERIC_ECC_affine_point_on_curve(&alice_public_key, &domain))
  {
    qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_affine_point_on_curve API failed on alice: not on curve!");
    return -E_FAILURE;
  } 
  
  // change the public key and check for negative case
  bob_public_key.x.data[0] = !bob_public_key.x.data[0];
  if(qsee_SW_GENERIC_ECC_affine_point_on_curve(&bob_public_key, &domain))
  {
    qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_affine_point_on_curve API failed on modified bob: on curve!");
    return -E_FAILURE;
  } 

   return rv;

}

/**
   @brief
   Sample code for ECDH functional tests
*/
int tz_app_ecdh_func(void)
{
   int idx=0;
   int status = E_SUCCESS;

   /*---------------------------------------------------------------
     Run through ECDH test
     -------------------------------------------------------------------*/
   for (idx = 0; idx < ECC_TEST_COUNT; idx++)
   {
      status = tz_app_crypto_ecdh_func(FALSE);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto ECDH Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto ECDH Functional Test - Vect Num:%u Failed", idx);
         return status;
      }

      status = tz_app_crypto_ecdh_func(TRUE);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto ECDH Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto ECDH Functional Test - Vect Num:%u Failed", idx);
         return status;
      }
   }

   return status;
}

/**
   @brief
   Sample code for ECDH functional tests
*/
int tz_app_ecdsa_func(void)
{
   int status = E_SUCCESS;
   int i = 0;
   int len;

   /*---------------------------------------------------------------
     Run through ECDH test
     -------------------------------------------------------------------*/
   for (i = 0; i < ECC_TEST_COUNT; i++)
   {

      switch (i % 17) {
         case 1:
	    /* test lengths shorter than full size */
	    len = sizeof (uint32) * (QSEE_BIGLEN - 1) - i % 5;
	    break;
         case 2:
	    /* test lengths longer than full size */
	    len = sizeof (uint32) * (QSEE_BIGLEN - 1) + i % 5;
	    break;
         case 3:
	    /* test shortest length */
	    len = 1;
	    break;
         default:
	    len = sizeof (uint32) * (QSEE_BIGLEN - 1);
      }

      status = tz_app_crypto_ecdsa_func(FALSE, len);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto ECDSA Functional Test - Vect Num:%u Passed", i);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto ECDSA Functional Test - Vect Num:%u Failed", i);
         return status;
      }

      status = tz_app_crypto_ecdsa_func(TRUE, len);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto ECDSA Functional Test - Vect Num:%u Passed", i);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto ECDSA Functional Test - Vect Num:%u Failed", i);
         return status;
      }

   }

   return status;
}

/**
   @brief
   Sample code for Generic ECDH functional tests
*/
int tz_app_generic_ecdh_func(void)
{
   int status = E_SUCCESS;
   int i = 0;
   int num = 0;

   /*---------------------------------------------------------------
     Run through Generic ECDH test
     -------------------------------------------------------------------*/
   num = sizeof(generic_ecc_test_vectors) / sizeof(generic_ecc_vector_type);

   //Set up loop over known ECDSA test vectors and test ECDH
   for (i=0; i < num; i++)
   {
      status = tz_app_crypto_generic_ecdh_func(&(generic_ecc_test_vectors[i]));

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto Generic ECDH Functional Test - Vect Num:%u Passed", i);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto Generic ECDH Functional Test - Vect Num:%u Failed", i);
         return status;
      }
   }

   return status;
}

/**
   @brief
   Sample code for Generic ECDSA functional tests
*/
int tz_app_generic_ecdsa_func(void)
{
   int status = E_SUCCESS;
   int i = 0;
   int num = 0;
   uint32 passed = 0;

   /*---------------------------------------------------------------
     Run through Generic ECDSA test
     -------------------------------------------------------------------*/
   num = sizeof(generic_ecc_test_vectors) / sizeof(generic_ecc_vector_type);

   qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto Generic ECDSA Functional Test has %u Vectors", num);

   //Set up loop over known ECDSA test vectors
   for (i=0; i < num; i++)
   {
      status = tz_app_crypto_generic_ecdsa_pubkey_gen(&(generic_ecc_test_vectors[i]));

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto Generic ECDSA Functional PubkeyGen Test - Vect Num:%u Passed", i);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto Generic ECDSA Functional PubkeyGen Test - Vect Num:%u Failed", i);
      }

      status = tz_app_crypto_generic_ecdsa_func(&(generic_ecc_test_vectors[i]));

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto Generic ECDSA Functional Test - Vect Num:%u Passed", i);
         ++passed;
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"Crypto Generic ECDSA Functional Test - Vect Num:%u Failed", i);
      }
   }

   if (passed == num)
   {
      status = E_SUCCESS;
   } else {
      status = -E_FAILURE;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE AES software crypto API's
*/
int tz_app_ufcrypto_aes_func(SW_Cipher_Alg_Type alg,
                             SW_CipherModeType mode,
                             uint8 *pt,
                             uint32 pt_len,
                             uint8 *key,
                             uint32 key_len,
                             uint8 *iv,
                             uint8 *ct,
                             uint8 verify)
{
   CryptoCntxHandle *cntx = 0;
   uint8 *ct_tmp = 0;
   uint8 *pt_tmp = 0;
   volatile int status = E_SUCCESS;

   IovecListType   ioVecIn;
   IovecListType   ioVecOut;
   IovecType       IovecIn;
   IovecType       IovecOut;
   SW_CipherEncryptDir dir = SW_CIPHER_ENCRYPT;

   /*----------------------------------------------------------------------
     Allocate temp ct buffer
     ------------------------------------------------------------------------*/
   if (E_SUCCESS == status && (verify == 1) &&
       (ct_tmp = qsee_malloc(pt_len)) == NULL)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_ufcrypto_aes_func ct_tmp malloc failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     If verify is 0,  it means we are not using the predefined test case and
     cannot verify the encrypted packet
     -------------------------------------------------------------------------*/
   if (verify == 0)
      ct_tmp = ct;

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/

   /* Input IOVEC */
   ioVecIn.size = 1;
   ioVecIn.iov = &IovecIn;
   ioVecIn.iov[0].dwLen = pt_len;
   ioVecIn.iov[0].pvBase = pt;

   /* Output IOVEC */
   ioVecOut.size = 1;
   ioVecOut.iov = &IovecOut;
   ioVecOut.iov[0].dwLen = pt_len;
   ioVecOut.iov[0].pvBase = ct_tmp;

   //Determine algorithm
   if(key_len == 16)
   {
      if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES128))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
         status = -E_FAILURE;
      }
   }
   else
   {
      if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES256))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
         status = -E_FAILURE;
      }
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set direction to encryption
     ----------------------------------------------------------------------*/

   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &dir, sizeof(SW_CipherEncryptDir)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
   }
   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV
     ----------------------------------------------------------------------*/
   if(mode != SW_CIPHER_MODE_ECB)
   {
      if(E_SUCCESS == status &&
         qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, SW_AES_IV_SIZE) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
         status = -E_FAILURE;
      }
   }

   /*-----------------------------------------------------------------------
     Clear temp buffer
     ------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1))
   {
      memset(ct_tmp, 0, pt_len);
   }

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_CipherData encrypt API failed");
      status = -E_FAILURE;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG,"Reached memcompare 0x%x", status);

   /*--------------------------------------------------------------------------
     If NULL key pointer then we are using HW key so don't compare encrypted result
     -----------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) && 0 != app_crypto_timesafe_memcmp(ct, ct_tmp, pt_len))
   {
    qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher app_crypto_timesafe_memcmp encrypt failed");
      status = -E_FAILURE;
   }

   /*------------------------------------------------------------------------------
     Allocate temp pt buffer
     --------------------------------------------------------------------------------*/
   if ((E_SUCCESS == status) && (verify == 1) &&
       (pt_tmp = qsee_malloc(pt_len)) == NULL)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_ufcrypto_aes_func pt_tmp malloc failed");
      status = -E_FAILURE;
   }

   if ((E_SUCCESS == status)&& (verify == 1))
   {
      memset(pt_tmp, 0, pt_len);
   }

   /*-----------------------------------------------------------------------
     If verify is 0,  it means we are not using the predefined test case and
     cannot verify the encrypted packet
     -------------------------------------------------------------------------*/
   if (verify == 0)
      pt_tmp = pt;


   /* Input IOVEC */
   ioVecIn.size = 1;
   ioVecIn.iov = &IovecIn;
   ioVecIn.iov[0].dwLen = pt_len;
   ioVecIn.iov[0].pvBase = ct_tmp;

   /* Output IOVEC */
   ioVecOut.size = 1;
   ioVecOut.iov = &IovecOut;
   ioVecOut.iov[0].dwLen = pt_len;
   ioVecOut.iov[0].pvBase = pt_tmp;

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set direction to decryption
     ----------------------------------------------------------------------*/

   dir = SW_CIPHER_DECRYPT;
   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &dir, sizeof(SW_CipherEncryptDir)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV
     ----------------------------------------------------------------------*/
   if(mode != SW_CIPHER_MODE_ECB)
   {
      if(E_SUCCESS == status &&
         qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, SW_AES_IV_SIZE) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
         status = -E_FAILURE;
      }
   }

   /*----------------------------------------------------------------------------------
     Now decrypt the data
     ------------------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_CipherData decrypt API failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------------
     Now compare decrypted results
     ---------------------------------------------------------------------------------------*/
  if ((E_SUCCESS == status) && (verify == 1) && 0 != app_crypto_timesafe_memcmp(pt, pt_tmp, pt_len))
  {  
    qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_SW_Cipher app_crypto_timesafe_memcmp decrypt failed");
      status = -E_FAILURE;
   }

   /*-------------------------------------------------------------------------------
     Free ctx
     --------------------------------------------------------------------------------*/
   if (cntx)
   {
      if(key_len == 16)
      {
         if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES128))
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
            status = -E_FAILURE;
         }
      }
      else
      {
         if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES256))
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
            status = -E_FAILURE;
         }
      }
      cntx = 0;
   }

   /*--------------------------------------------------------------------------------
     Free malloc data
     ---------------------------------------------------------------------------------*/
   if ((ct_tmp) && (verify == 1))
   {
      qsee_free(ct_tmp);
      ct_tmp = 0;
   }

   if ((pt_tmp)&& (verify == 1))
   {
      qsee_free(pt_tmp);
      pt_tmp = 0;
   }

   return status;
}

/**
   @brief
   Sample code for how to call QSEE AES GCM software crypto
   API's
*/
int tz_app_ufcrypto_aes_gcm_func(uintnt *pt,
                                 uint32 pt_len,
                                 uint8 *key,
                                 uint32 key_len,
                                 uint8 *iv,
                                 uint32 iv_len,
                                 uintnt *ptr_tmp,
                                 uintnt *header_data,
                                 uintnt *auth_struct)
{

   CryptoCntxHandle *cntx = 0;
   int status = E_SUCCESS;

   IovecListType   ioVecIn;
   IovecListType   ioVecOut;
   IovecType       IovecIn;
   IovecType       IovecOut;
   uint8          tag_auth[16] = {0};

   uint32 ct_len  = pt_len;
   SW_CipherEncryptDir dir = SW_CIPHER_ENCRYPT;
   SW_CipherModeType mode = SW_CIPHER_MODE_GCM;

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/

   /* Input IOVEC */
   ioVecIn.size = 1;
   ioVecIn.iov = &IovecIn;
   ioVecIn.iov[0].dwLen = pt_len;
   ioVecIn.iov[0].pvBase = (void*)((uintnt)pt[0]);

   /* Output IOVEC */
   ioVecOut.size = 1;
   ioVecOut.iov = &IovecOut;
   ioVecOut.iov[0].dwLen = ct_len;
   ioVecOut.iov[0].pvBase = (void*)((uintnt)ptr_tmp[1]);

   //Determine algorithm
   if(key_len == 16)
   {
      if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES128))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
         status = -E_FAILURE;
      }
   }
   else
   {
      if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES256))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
         status = -E_FAILURE;
      }
   }

   /*--------------------------------------------------------------------
     Set direction to encryption
     ----------------------------------------------------------------------*/

   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &dir, sizeof(SW_CipherEncryptDir)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam direction API failed");
      status = -E_FAILURE;
   }
   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam mode API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm key API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV
     ----------------------------------------------------------------------*/
   if(mode != SW_CIPHER_MODE_ECB)
   {
      if(E_SUCCESS == status &&
         qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, iv_len) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam iv API failed");
         status = -E_FAILURE;
      }
   }

   /*--------------------------------------------------------------------
     Set AAD if present
     ----------------------------------------------------------------------*/
   if(E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_AAD, (void*)((uintnt)header_data[0]), header_data[1]) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam AAD API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_CipherData encrypt API failed");
      status = -E_FAILURE;
   }

   /*-----------------------------------------------------------------------
     Get the auth tag
     -------------------------------------------------------------------------*/
   if(E_SUCCESS == status &&
      0 != qsee_SW_Cipher_GetParam(cntx, SW_CIPHER_PARAM_TAG, &tag_auth[0], 16))
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_GetParam auth tag API failed");
      status = -E_FAILURE;
   }


   if (cntx)
   {
      if(key_len == 16)
      {
         if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES128))
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
            status = -E_FAILURE;
         }
      }
      else
      {
         if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES256))
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
            status = -E_FAILURE;
         }
      }
      cntx = 0;
   }

   /*--------------------------------------------------------------------------
     If NULL key pointer then we are using HW key so don't compare encrypted result
     -----------------------------------------------------------------------------*/
   if ((E_SUCCESS == status))
   {
      if(pt[2] == 0 || pt[3] == 1)
      {
         if(pt[3] == 0)
         {
            //Now compare encrypted results
        if (app_crypto_timesafe_memcmp((void*)((uintnt)pt[1]), (void*)((uintnt)ptr_tmp[1]), ct_len) != 0)
            {
               qsee_log(QSEE_LOG_MSG_ERROR,"encrypted data compare failed");
               status = -E_FAILURE;
            }

        if(app_crypto_timesafe_memcmp(tag_auth, (void*)((uintnt)auth_struct[0]), auth_struct[1]) != 0)
            {
               qsee_log(QSEE_LOG_MSG_ERROR,"tag compare failed");
               status = -E_FAILURE;
            }
         }

         /* Input IOVEC */
         ioVecIn.size = 1;
         ioVecIn.iov = &IovecIn;
         ioVecIn.iov[0].dwLen = ct_len;
         ioVecIn.iov[0].pvBase = (void*)((uintnt)ptr_tmp[1]);

         /* Output IOVEC */
         ioVecOut.size = 1;
         ioVecOut.iov = &IovecOut;
         ioVecOut.iov[0].dwLen = pt_len;
         ioVecOut.iov[0].pvBase = (void*)((uintnt)ptr_tmp[0]);


         //Determine algorithm
         if(key_len == 16)
         {
            if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES128))
            {
               qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
               status = -E_FAILURE;
            }
         }
         else
         {
            if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES256))
            {
               qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
               status = -E_FAILURE;
            }
         }

         /*--------------------------------------------------------------------
           Set direction to decryption
           ----------------------------------------------------------------------*/

         dir = SW_CIPHER_DECRYPT;
         if (E_SUCCESS == status &&
             qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &dir, sizeof(SW_CipherEncryptDir)) != 0)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam direction API failed");
            status = -E_FAILURE;
         }

         /*--------------------------------------------------------------------
           Set AES mode
           ----------------------------------------------------------------------*/
         if (E_SUCCESS == status &&
             qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam mode API failed");
            status = -E_FAILURE;
         }

         /*--------------------------------------------------------------------
           Set key for encryption
           ----------------------------------------------------------------------*/
         if (E_SUCCESS == status &&
             qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_param key API failed");
            status = -E_FAILURE;
         }

         /*--------------------------------------------------------------------
           Set IV
           ----------------------------------------------------------------------*/
         if(mode != SW_CIPHER_MODE_ECB)
         {
            if(E_SUCCESS == status &&
               qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, iv_len) != 0)
            {
               qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam iv API failed");
               status = -E_FAILURE;
            }
         }

         /*--------------------------------------------------------------------
           Set AAD
           ----------------------------------------------------------------------*/
         if(E_SUCCESS == status &&
            0 != qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_AAD, (void*)((uintnt)header_data[0]), header_data[1]))
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam AAD API failed");
            status = -E_FAILURE;
         }

         /*--------------------------------------------------------------------
           Set Tag
           ----------------------------------------------------------------------*/
         if(E_SUCCESS == status &&
            0 != qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_TAG, tag_auth, 16))
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam auth tag API failed");
            status = -E_FAILURE;
         }

         /*----------------------------------------------------------------------------------
           Now decrypt the data
           ------------------------------------------------------------------------------------*/
         if (E_SUCCESS == status &&
             qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_CipherData decrypt API failed");
            status = -E_FAILURE;
         }

         /*-------------------------------------------------------------------------------------
           Now compare decrypted results
           ---------------------------------------------------------------------------------------*/
         if ((E_SUCCESS == status))
         {
            //Now compare decrypted results
        if (app_crypto_timesafe_memcmp((void*)((uintnt)pt[0]), (void*)((uintnt)ptr_tmp[0]), pt_len) != 0)
            {
               qsee_log(QSEE_LOG_MSG_ERROR,"decrypted data memcompare failed");
               status = -E_FAILURE;
            }
         }

         /*-------------------------------------------------------------------------------
           Free ctx
           --------------------------------------------------------------------------------*/
         if (cntx)
         {
            if(key_len == 16)
            {
               if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES128))
               {
                  qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
                  status = -E_FAILURE;
               }
            }
            else
            {
               if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES256))
               {
                  qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
                  status = -E_FAILURE;
               }
            }
            cntx = 0;
         }
      }
   }

   return status;
}


/**
   @brief
   Sample code for how to call QSEE HASH software crypto API's
*/
int tz_app_ufcrypto_hash_func(SW_Auth_Alg_Type alg,
                              uint8 *msg,
                              uint32 msg_len,
                              uint8 *digest)
{
   int status = E_SUCCESS;
   uint8 result_digest[SW_SHA256_DIGEST_SIZE];
   uint32 digest_len = 0;
   uint32 len = 0;

   IovecListType   ioVecIn;
   IovecListType   ioVecOut;
   IovecType       IovecIn;
   IovecType       IovecOut;

   CryptoCntxHandle *cntx = NULL;

   //Clear result digest
   memset(result_digest, 0, sizeof(result_digest));

   if (alg == SW_AUTH_ALG_SHA1)
      len = SW_SHA1_DIGEST_SIZE;
  else if (alg == SW_AUTH_ALG_SHA256)
      len = SW_SHA256_DIGEST_SIZE;
  else if (alg == SW_AUTH_ALG_MD5)
	len = SW_MD5_DIGEST_SIZE;
  else
    {
      qsee_log(QSEE_LOG_MSG_ERROR,"Invalid ALG");
      return -E_FAILURE;
    }

   /* Input IOVEC */
   ioVecIn.size = 1;
   ioVecIn.iov = &IovecIn;
   ioVecIn.iov[0].dwLen = msg_len;
   ioVecIn.iov[0].pvBase = msg;

   /* Output IOVEC */
   ioVecOut.size = 1;
   ioVecOut.iov = &IovecOut;
   ioVecOut.iov[0].dwLen = len;
   ioVecOut.iov[0].pvBase = result_digest;

   do
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Before qsee_sw_hash_initAPI");
      if(0 != qsee_SW_Hash_Init(&cntx, alg))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_sw_hash_initAPI failed");
         status = -E_FAILURE;
         break;
      }

      if(0 != qsee_SW_Hash_Update(cntx, ioVecIn))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Hash_Update API failed");
         status = -E_FAILURE;
         break;
      }


      if(0 != qsee_SW_Hash_Final(cntx, &ioVecOut))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Hash_Final API failed");
         status = -E_FAILURE;
         break;
      }

      //Compare results
    if (0 != app_crypto_timesafe_memcmp(result_digest, digest, len))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Hash_Final output failed!");
         status = -E_FAILURE;
      }

   } while (0);

   qsee_SW_Hash_Deinit(&cntx);

   return status;
}



/**
   @brief
   Sample code for how to call QSEE HMAC software crypto API's
*/
int tz_app_ufcrypto_hmac_func(uint8 *msg,
                              uint32 msg_len,
					          uint8* key,
                              uint32 key_len,
                              uint8* hmac1,
                              uint8* hmac256)
{
   int status = E_SUCCESS;
   uint8 result_digest[SW_SHA256_DIGEST_SIZE];

   IovecListType   ioVecIn;
   IovecListType   ioVecOut;
   IovecType       IovecIn;
   IovecType       IovecOut;

   //Clear result digest
   memset(result_digest, 0, sizeof(result_digest));

   /* Input IOVEC */
   ioVecIn.size = 1;
   ioVecIn.iov = &IovecIn;
   ioVecIn.iov[0].dwLen = msg_len;
   ioVecIn.iov[0].pvBase = msg;


   if(NULL != hmac1)
   {
      /* Output IOVEC */
      ioVecOut.size = 1;
      ioVecOut.iov = &IovecOut;
      ioVecOut.iov[0].dwLen = SW_SHA1_DIGEST_SIZE;
      ioVecOut.iov[0].pvBase = result_digest;

      if(0 != qsee_SW_Hmac(key, key_len, ioVecIn, &ioVecOut, SW_AUTH_ALG_SHA1))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Hmac API failed");
         status = -E_FAILURE;
      }

    //Compare rsults 
    if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(result_digest, hmac1, QSEE_SHA1_HASH_SZ)))
      {
         status = -E_FAILURE;
      }
   }

   //Clear result digest
   memset(result_digest, 0, sizeof(result_digest));

   //Calculate HASH SHA256
   if (NULL != hmac256)
   {
      /* Output IOVEC */
      ioVecOut.size = 1;
      ioVecOut.iov = &IovecOut;
      ioVecOut.iov[0].dwLen = SW_SHA256_DIGEST_SIZE;
      ioVecOut.iov[0].pvBase = result_digest;

      if ((E_SUCCESS == status) &&
          (0 != qsee_SW_Hmac(key, key_len, ioVecIn, &ioVecOut, SW_AUTH_ALG_SHA256)))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Hmac API failed");
         status = -E_FAILURE;
      }

    //Compare rsults 
    if ((E_SUCCESS == status) && (0 != app_crypto_timesafe_memcmp(result_digest, hmac256, QSEE_SHA256_HASH_SZ)))
      {
         status = -E_FAILURE;
      }
   }

   return status;
}


/**
   @brief
   Sample code for SW AES functional tests
*/
int tz_app_sw_aes_func(void)
{
   int idx=0;
   int num=0;
   int status = E_SUCCESS;
   num = sizeof(sw_aes_test_vectors) / sizeof(uf_aes_vector_type_t);

   /*---------------------------------------------------------------
     Run through AES128/256 ECB/CBC/CTR test vectors
     -------------------------------------------------------------------*/
   for (idx = 0; idx < num; idx++)
   {
      status = tz_app_ufcrypto_aes_func(sw_aes_test_vectors[idx].alg,
                                        sw_aes_test_vectors[idx].mode,
                                        sw_aes_test_vectors[idx].pt,
                                        sw_aes_test_vectors[idx].pt_len,
                                        sw_aes_test_vectors[idx].key,
                                        sw_aes_test_vectors[idx].key_len,
                                        sw_aes_test_vectors[idx].iv,
                                        sw_aes_test_vectors[idx].ct, 1);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"SW AES Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"SW AES Functional Test - Vect Num:%u Failed", idx);
      }
   }

   /* AES GCM test */
   {
      int i;
      int j;
      uintnt ptr_struct[4] = {0,0,0,0};
      uintnt header_struct[2];
      uintnt tag_struct[2];
      uintnt ptr_tmp_struct[2];
      uint32 key_len = 16;

      ptr_tmp_struct[0] = (uintnt)aes_large_plain_text_1;
      ptr_tmp_struct[1] = (uintnt)aes_large_cipher_text_1;
      //for( j = 0; j < 3; j+=2 )
      j = 0;
      {
         key_len = 16 + 8 * j;

         for( i = 0; i < MAX_TESTS; i++ )
         {
            ptr_struct[0] = (uintnt)&pt_gcm[pt_index_gcm[i]];
            ptr_struct[1] = (uintnt)&ct_gcm[j * 6 + i];
            header_struct[0] = (uintnt)&additional_gcm[add_index_gcm[i]];
            header_struct[1] = (uintnt)add_len_gcm[i];
            tag_struct[0] = (uintnt)&tag_gcm[j * 6 + i];
            tag_struct[1] = 16;

            qsee_log(QSEE_LOG_MSG_DEBUG,"Crypto AES GCM Vect Test - Vect Num: %u", i);
            status = tz_app_ufcrypto_aes_gcm_func(ptr_struct, pt_len_gcm[i], key_gcm[key_index_gcm[i]], key_len,
                                                  iv_gcm[iv_index_gcm[i]], iv_len_gcm[i], ptr_tmp_struct,
                                                  header_struct, tag_struct);
            if (status == E_SUCCESS)
            {
               qsee_log(QSEE_LOG_MSG_DEBUG,"SW AES GCM Functional Test - Vect Num:%u Passed", i);
            }
            else
            {
               qsee_log(QSEE_LOG_MSG_ERROR,"SW AES GCM Functional Test - Vect Num:%u Failed", i);
               return status;
            }
         }
      }
   }

   return status;
}


/**
   @brief
   Sample code for SW HASH functional tests
*/
int tz_app_sw_hash_func(void)
{
   int idx=0;
   int num=0;
   int status = E_SUCCESS;
   num = sizeof(uf_sha_test_vectors) / sizeof(uf_sha_vector_type_t);
   qsee_log(QSEE_LOG_MSG_DEBUG,"SW Hash Functional Test %d Vectors", num);

   /*---------------------------------------------------------------
     Run through HASH test vectors
     -------------------------------------------------------------------*/
   for (idx = 0; idx < num; idx++)
   {
      status = tz_app_ufcrypto_hash_func(uf_sha_test_vectors[idx].alg,
                                         uf_sha_test_vectors[idx].msg,
                                         uf_sha_test_vectors[idx].msg_len,
                                         uf_sha_test_vectors[idx].digest);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"SW Hash Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"SW Hash Functional Test - Vect Num:%u Failed", idx);
         return status;
      }
   }
   return status;
}


/**
   @brief
   Sample code for SW HMAC functional tests
*/
int tz_app_sw_hmac_func(void)
{
   int idx=0;
   int num=0;
   int status = E_SUCCESS;
   num = sizeof(sha_hmac_test_vectors) / sizeof(sha_hmac_vector_type_t);

   /*---------------------------------------------------------------
     Run through HMAC test vectors
     -------------------------------------------------------------------*/
   for (idx = 0; idx < num; idx++)
   {
      status = tz_app_ufcrypto_hmac_func(sha_hmac_test_vectors[idx].msg,
                                         sha_hmac_test_vectors[idx].msg_len,
                                         sha_hmac_test_vectors[idx].hmac_key,
                                         sha_hmac_test_vectors[idx].hmac_key_len,
                                         sha_hmac_test_vectors[idx].msg_hmac1,
                                         sha_hmac_test_vectors[idx].msg_hmac256);

      if (status == E_SUCCESS)
      {
         qsee_log(QSEE_LOG_MSG_DEBUG,"SW Hmac Functional Test - Vect Num:%u Passed", idx);
      }
      else
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"SW Hmac Functional Test - Vect Num:%u Failed", idx);
      }
   }

   return status;
}



/**
   @brief
   Sample code for how to call QSEE AES software crypto API's
*/
int tz_app_ufcrypto_aes_perf(SW_Cipher_Alg_Type alg,
                             SW_CipherModeType mode,
                             uint8 *pt,
                             uint32 pt_len,
                             uint8 *key,
                             uint32 key_len,
                             uint8 *iv)
{
   CryptoCntxHandle *cntx = 0;
   uint8 *ct_tmp = pt;
   int status = E_SUCCESS;

   IovecListType   ioVecIn;
   IovecListType   ioVecOut;
   IovecType       IovecIn;
   IovecType       IovecOut;
   SW_CipherEncryptDir dir = SW_CIPHER_ENCRYPT;

   /*------------------------------------------------------------------
     Init ctx
     ------------------------------------------------------------------*/

   /* Input IOVEC */
   ioVecIn.size = 1;
   ioVecIn.iov = &IovecIn;
   ioVecIn.iov[0].dwLen = pt_len;
   ioVecIn.iov[0].pvBase = pt;

   /* Output IOVEC */
   ioVecOut.size = 1;
   ioVecOut.iov = &IovecOut;
   ioVecOut.iov[0].dwLen = pt_len;
   ioVecOut.iov[0].pvBase = ct_tmp;

   //Determine algorithm
   if(key_len == 16)
   {
      if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES128))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
         status = -E_FAILURE;
      }
   }
   else
   {
      if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES256))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
         status = -E_FAILURE;
      }
   }

   /*--------------------------------------------------------------------
     Set key for encryption
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_KEY, key, key_len) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_cipher_set_parm API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set direction to encryption
     ----------------------------------------------------------------------*/

   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_DIRECTION, &dir, sizeof(SW_CipherEncryptDir)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
   }
   /*--------------------------------------------------------------------
     Set AES mode
     ----------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
      status = -E_FAILURE;
   }

   /*--------------------------------------------------------------------
     Set IV
     ----------------------------------------------------------------------*/
   if(mode != SW_CIPHER_MODE_ECB)
   {
      if(E_SUCCESS == status &&
         qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, SW_AES_IV_SIZE) != 0)
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam API failed");
         status = -E_FAILURE;
      }
   }

   /*-----------------------------------------------------------------------
     Now encrypt the data
     -------------------------------------------------------------------------*/
   if (E_SUCCESS == status &&
       qsee_SW_CipherData(cntx, ioVecIn, &ioVecOut) != 0)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_CipherData encrypt API failed");
      status = -E_FAILURE;
   }

   if (cntx)
   {
      if(key_len == 16)
      {
         if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES128))
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
            status = -E_FAILURE;
         }
      }
      else
      {
         if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES256))
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
            status = -E_FAILURE;
         }
      }
      cntx = 0;
   }

   return status;
}


/**
   @brief
   Sample code for how to call QSEE HASH software crypto API's
*/
int tz_app_ufcrypto_hash_perf(SW_Auth_Alg_Type alg,
                              uint8 *msg,
                              uint32 msg_len)
{
   int status = E_SUCCESS;
   uint8 result_digest[SW_SHA256_DIGEST_SIZE];
   uint32 len;

   IovecListType   ioVecIn;
   IovecListType   ioVecOut;
   IovecType       IovecIn;
   IovecType       IovecOut;

   CryptoCntxHandle *cntx = NULL;

   //Clear result digest
   memset(result_digest, 0, sizeof(result_digest));

   if (alg == SW_AUTH_ALG_SHA1)
      len = SW_SHA1_DIGEST_SIZE;
   else
      len = SW_SHA256_DIGEST_SIZE;

   /* Input IOVEC */
   ioVecIn.size = 1;
   ioVecIn.iov = &IovecIn;
   ioVecIn.iov[0].dwLen = msg_len;
   ioVecIn.iov[0].pvBase = msg;

   /* Output IOVEC */
   ioVecOut.size = 1;
   ioVecOut.iov = &IovecOut;
   ioVecOut.iov[0].dwLen = len;
   ioVecOut.iov[0].pvBase = result_digest;

   do
   {
      if(0 != qsee_SW_Hash_Init(&cntx, alg))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_sw_hash_initAPI failed");
         status = -E_FAILURE;
         break;
      }

      if(0 != qsee_SW_Hash_Update(cntx, ioVecIn))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Hash_Update API failed");
         status = -E_FAILURE;
         break;
      }


      if(0 != qsee_SW_Hash_Final(cntx, &ioVecOut))
      {
         qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Hash_Final API failed");
         status = -E_FAILURE;
      }

   } while(0);

   qsee_SW_Hash_Deinit(&cntx);

   return status;
}


/**
   @brief
   Sample code for how to call QSEE HMAC software crypto API's
*/
int tz_app_ufcrypto_hmac_perf(SW_Auth_Alg_Type alg,
                              uint8 *msg,
                              uint32 msg_len,
                              uint8* key,
                              uint32 key_len)
{
   int status = E_SUCCESS;
   uint8 result_digest[SW_SHA256_DIGEST_SIZE];
   uint8 result_len;

   IovecListType   ioVecIn;
   IovecListType   ioVecOut;
   IovecType       IovecIn;
   IovecType       IovecOut;

   //Clear result digest
   memset(result_digest, 0, sizeof(result_digest));

   if (alg == SW_AUTH_ALG_SHA1)
      result_len = SW_SHA1_DIGEST_SIZE;
   else if (alg == SW_AUTH_ALG_SHA256)
      result_len = SW_SHA256_DIGEST_SIZE;
   else
      return -E_FAILURE;

   /* Input IOVEC */
   ioVecIn.size = 1;
   ioVecIn.iov = &IovecIn;
   ioVecIn.iov[0].dwLen = msg_len;
   ioVecIn.iov[0].pvBase = msg;

   /* Output IOVEC */
   ioVecOut.size = 1;
   ioVecOut.iov = &IovecOut;
   ioVecOut.iov[0].dwLen = result_len;
   ioVecOut.iov[0].pvBase = result_digest;

   if(0 != qsee_SW_Hmac(key, key_len, ioVecIn, &ioVecOut, alg))
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Hmac API failed");
      status = -E_FAILURE;
   }

   return status;
}

/**
   @brief
   Sample code for AES performance tests
*/
int tz_app_sw_aes_perf(void)
{
   uint32 size;
   uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
   uint32 i = 0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;

   for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Software AES128 Test - AES Algo: ECB Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_ufcrypto_aes_perf(SW_CIPHER_ALG_AES128,
                                           SW_CIPHER_MODE_ECB,
                                           aes_performance,
                                           size,
                                           aes_key_1,
                                           sizeof(aes_key_1),
                                           NULL);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_ufcrypto_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Software AES128 Test - AES Algo: CBC Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_ufcrypto_aes_perf(SW_CIPHER_ALG_AES128,
                                           SW_CIPHER_MODE_CBC,
                                           aes_performance,
                                           size,
        					                      aes_key_2,
                                           sizeof(aes_key_2),
                                           aes_iv_2);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_ufcrypto_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Software AES128 Test - AES Algo: CTR Data Size: %u", size);


      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
         status = tz_app_ufcrypto_aes_perf(SW_CIPHER_ALG_AES128,
                                           SW_CIPHER_MODE_CTR,
                                           aes_performance,
                                           size,
        					                      aes_key_2,
                                           sizeof(aes_key_2),
                                           aes_iv_2);
         if (status != E_SUCCESS)
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_ufcrypto_aes_perf test FAILED for iteration: %u", i);
            return status;
         }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
      qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

   }

   return status;
}

/**
   @brief
   Sample code for AES GCM performance tests
*/

int tz_app_sw_aes_gcm_perf(void)
{

   uint32 size;
   uint32 num1  = TEST_CRYPTO_GCM_PERF_MAX_ITERATIONS;
   uint32 i = 0, j=0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;
   uintnt ptr_struct[5] = {0,0,1,0,0};
   uintnt header_struct[2];
   uintnt tag_struct[2];
   uintnt ptr_tmp_struct[2];
   int key_len = 16;

   /* Performance for AES128, AES-GCM */
   for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
   {
      for( j = 0; j < MAX_TESTS; j++ )
      {
         ptr_tmp_struct[0] = (uintnt)aes_performance;
         ptr_tmp_struct[1] = (uintnt)aes_performance;

         ptr_struct[0] = (uintnt)aes_performance;
         ptr_struct[1] = (uintnt)aes_performance;
         header_struct[0] = (uintnt)&additional_gcm[add_index_gcm[j]];
         header_struct[1] = (uintnt)add_len_gcm[j];
         tag_struct[0] = (uintnt)&tag_gcm[j];
         tag_struct[1] = 16;
         /* AES128 */
         qsee_log(QSEE_LOG_MSG_DEBUG,"Performance for GCM-128 ; Data Size %u", size);
         total_time = 0;
         ptr_struct[4] = 0x0;
         memset(aes_performance, 0x1, size);
         total_time = qsee_get_uptime();
         for (i = 0; i < num1; i++)
         {
            status = tz_app_ufcrypto_aes_gcm_func(ptr_struct, size, key_gcm[key_index_gcm[i]], key_len, iv_gcm[iv_index_gcm[i]],
                                                  iv_len_gcm[i], ptr_tmp_struct,header_struct, tag_struct);
            if (status != E_SUCCESS)
            {
               qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_crypto_aes_perf test FAILED for gcm iteration: %u", i);
               return status;
            }
         }
         total_time = qsee_get_uptime() - total_time;
         /*
          * average_time_in_milli_sec = total_time/num_of_iterations
          * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
          * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
          */
         tput = ( ((float)size/((float)total_time/(float)num1)) * 1000.00 ) / (1024.00);
         qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for %u iterations  %u ms", num1, total_time);
         qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u KBps", tput);
      }
   }

   return status;

}


/**
   @brief
   Sample code for HASH performance tests
*/
int tz_app_sw_hash_perf(void)
{
   uint32 size;
   uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
   uint32 i = 0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;

	for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Software HASH Test - HASH Algo: SHA128 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
	    status = tz_app_ufcrypto_hash_perf(SW_AUTH_ALG_SHA1,
                                            aes_performance,
                                            size);

	    if (status != E_SUCCESS) 
	    {
           qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_ufcrypto_hash_perf test FAILED for iteration: %u", i);
	       return status;
	    }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
		qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
		qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Software HASH Test - HASH Algo: SHA256 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
	    status = tz_app_ufcrypto_hash_perf(SW_AUTH_ALG_SHA256,
                                            aes_performance,
                                            size);

	    if (status != E_SUCCESS) 
	    {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_ufcrypto_hash_perf test FAILED for iteration: %u", i);
	      return status;
	    }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
		qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
		qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
   }

   return status;
}

/**
   @brief
   Sample code for HMAC performance tests
*/
int tz_app_sw_hmac_perf(void)
{
   uint32 size;
   uint32 num  = TEST_CRYPTO_PERF_MAX_ITERATIONS;
   uint32 i = 0;
   int    tput;
   int    status = E_SUCCESS;
   unsigned long long total_time;

   for (size = TZBSP_TEST_CRYPTO_PERF_START_SIZE; size <= TZBSP_TEST_CRYPTO_PERF_END_SIZE; size = size*2)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Software HMAC Test - HMAC Algo: SHA128 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
	    status = tz_app_ufcrypto_hmac_perf(SW_AUTH_ALG_SHA1,
                                            aes_performance,
                                            size,
                                            sha_hmac_key_1,
  	  								                   sizeof(sha_hmac_key_1));

	    if (status != E_SUCCESS) 
	    {
           qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_ufcrypto_hmac_perf test FAILED for iteration: %u", i);
	       return status;
	    }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
		qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
		qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);

      qsee_log(QSEE_LOG_MSG_DEBUG,"Performance Software HMAC Test - HMAC Algo: SHA256 Data Size: %u", size);

      total_time = qsee_get_uptime();

      for (i = 0; i < num; i++)
      {
	    status = tz_app_ufcrypto_hmac_perf(SW_AUTH_ALG_SHA256,
                                            aes_performance,
                                            size,
                                            sha_hmac_key_1,
                                            sizeof(sha_hmac_key_1));

	    if (status != E_SUCCESS) 
         {
            qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_ufcrypto_hmac_perf test FAILED for iteration: %u", i);
	      return status;
	    }
      }

      total_time = qsee_get_uptime() - total_time;

      /*
       * average_time_in_milli_sec = total_time/num_of_iterations
       * Tput_in_Bytes_per_sec = ( size/average_time_in_milli_sec ) * 1000
       * Tput_in_MBps = (Tput_in_Bytes_per_sec) / (1024 * 1024)
       */
      tput = (((float)size / ((float)total_time / (float)num)) * 1000.00) / (1024.00 * 1024.00);
		qsee_log(QSEE_LOG_MSG_DEBUG,"Total time for 100 iterations: %u ms", total_time);
		qsee_log(QSEE_LOG_MSG_DEBUG,"Throughput: %u MBps", tput);
   }
   return status;
}

/**
  @brief 
    tz_app_generic_p256k1_func
*/
int tz_app_generic_p256k1_func(void)
{
    int rv = 0, rv_final = 0, rv1;
    QSEE_qrlbn_ecc_domain_t domain;
    QSEE_qrlbn_field_tag_t tag = QSEE_qrlbn_tag_hash; 

   // Domain parameters for secp256k1,  
    char m[] = 
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F";
    char a[] = 
        "0000000000000000000000000000000000000000000000000000000000000000";
    char b[] = 
        "00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000007";
    char G_x[] = 
        "79BE667E F9DCBBAC 55A06295 CE870B07 029BFCDB 2DCE28D9 59F2815B 16F81798";	 
    char G_y[] = 
        "483ADA77 26A3C465 5DA4FBFC 0E1108A8 FD17B448 A6855419 9C47D08F FB10D4B8";
    char n[] = 
        "FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE BAAEDCE6 AF48A03B BFD25E8C D0364141";

    // Only public key is used by verify
    char signer_pub_x[] =
        "bcccd639ae271f599dbc68853c6f1b0a61bd65debcd2a220a595d2a8cbd5678b";

    char signer_pub_y[] =
        "762fc7a3412eff0697f8fc6760d1edeccd79ccaa7bf1e4f45e7bfbd005fd7475";

    char signer_private_key[] =
        "dc072e084ed171eb856a1711cc9eafd9a46d14610ff8f55d118fb365d8e5dd12";

    char testcase_r[] =
        "19a51040b8dfa92406aed0aba5a5d62dc54203b204d3e6f6cdfc5d0db22eca29";

    char testcase_s[] = 
        "e8b9506e3ce249977e179c464277ed96bba3ff54d80e3d9e5f9e9ca9dccc4b77";  

    char testcase_hash1[] =
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    // No spaces allowed in the hash value. 
    char testcase_hash[] =
        "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08";

    QSEE_qrlbn_ecc_affine_point_t signer_pub_key;
    QSEE_qrlbn_ecc_bigval_t h, d;
    QSEE_qrlbn_ECDSA_sig_t sig;

    uint8_t hash_binary[32];

    int is_valid ;

    // Initialize domain and public key
    rv = qsee_SW_GENERIC_ECC_init(&domain, m, a, b, G_x, G_y, n, 1);
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_init API failed");
        rv = -E_FAILURE;
        return rv;
    }

    rv1 = qsee_SW_GENERIC_ECC_convert_input_to_bigval(&signer_pub_key.x, signer_pub_x, 32, 16);
    rv |= rv1 < 0;
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"error converting signer_pub_key x\n");
        rv = -E_FAILURE;
        return rv;
    }

    rv1 = qsee_SW_GENERIC_ECC_convert_input_to_bigval(&signer_pub_key.y, signer_pub_y, 32, 16);
    rv |= rv1 < 0;
    signer_pub_key.flags = 0;  // not the point at infinity
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"error converting signer_pub_key y\n");
        rv = -E_FAILURE;
        return rv;
    }
    // r and s are sized according to the order of the base point, not the modulus.
    rv1 = qsee_SW_GENERIC_ECC_convert_input_to_bigval(&sig.r, testcase_r, 32, 16);
    rv |= rv1 < 0;
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"error converting signature r to big number\n");
        rv = -E_FAILURE;
        return rv;
    }
    rv1 = qsee_SW_GENERIC_ECC_convert_input_to_bigval(&sig.s, testcase_s, 32, 16); 
    rv |= rv1 < 0;

    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"error converting signature s to big number\n");
        rv = -E_FAILURE;
        return rv;
    }

    memset(hash_binary, 0, sizeof(hash_binary));

    rv1 = qsee_SW_GENERIC_ECC_convert_input_to_bigval(&h, testcase_hash, 32, 16);
    rv |= rv1 < 0;
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"ERROR: error converting testcase_hash into bigval for sign\n");
    }
    rv1 = qsee_SW_GENERIC_ECC_bigval_to_binary(hash_binary, 32, &h, &domain, tag);

    rv |= rv1 < 0;
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"ERROR: error converting testcase_hash into binary for sign\n");
    }
    // Now do the ECDSA verify
    is_valid = qsee_SW_GENERIC_ECDSA_verify(hash_binary, 32, &signer_pub_key, &sig, &domain );

    if (!is_valid) {
        qsee_log(QSEE_LOG_MSG_DEBUG,"SUCCESS: signature correctly verified %d\n", is_valid);
    } else {
        qsee_log(QSEE_LOG_MSG_ERROR,"ERROR:  signature did not verify. %d\n", is_valid);
        rv |= -1;
    }

    // Now twiddle one bit in the hash value.
    hash_binary[0] ^= 1;
    // Now do the ECDSA verify again
    is_valid = qsee_SW_GENERIC_ECDSA_verify(hash_binary, 32, &signer_pub_key, &sig, &domain );

    if (!is_valid) {
        qsee_log(QSEE_LOG_MSG_ERROR,"ERROR: bad signature verified when it should not have.\n");
        rv |= -1;
    } else {
        qsee_log(QSEE_LOG_MSG_DEBUG,"SUCCESS: bad signature correctly rejected.\n");
   }

   // above is that verify is working.  Let us sign an arbitrary message, and see 
   // if we will verify.  That will be evidence the sign is working.
   // Use arith layer function to put strings direclty into bigval. Signer's private key

    rv_final |= rv;
    rv = 0;
    memset(hash_binary, 0, sizeof(hash_binary));
    rv1 = qsee_SW_GENERIC_ECC_convert_input_to_bigval(&d, signer_private_key, 32, 16);
    rv |= rv1 < 0;
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"ERROR: error converting signer_private_key into bigval\n");
    }

    // arbitrary value of hash of message
    rv1 = qsee_SW_GENERIC_ECC_convert_input_to_bigval(&h, testcase_hash1, 32, 16);
    rv |= rv1 < 0;
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"ERROR: error converting testcase_hash1 into bigval for sign\n");
    }

    rv1 = qsee_SW_GENERIC_ECC_bigval_to_binary(hash_binary, 32, &h, &domain, tag);
    rv |= rv1 < 0;
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"ERROR: error converting testcase_hash1 into binary for sign\n");
    }

    // Sign
    rv |= qsee_SW_GENERIC_ECDSA_sign(hash_binary, 32, &d, &sig, &domain);
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"ERROR: error occurred doing sign for arbitrary message\n");
    }

    // Now do the ECDSA verify
    is_valid = qsee_SW_GENERIC_ECDSA_verify(hash_binary, 32, &signer_pub_key, &sig, &domain);

    if (!is_valid) {
        qsee_log(QSEE_LOG_MSG_DEBUG,"SUCCESS: sign result correctly verified for arbitrary message.\n");
    } else {
        qsee_log(QSEE_LOG_MSG_ERROR,"ERROR:  sign result did not verify for arbitrary message.\n");
        rv |= -1;
    }

    // Now twiddle one bit in the hash value.
    hash_binary[0] ^= 1;
    // Now do the ECDSA verify again
    is_valid = qsee_SW_GENERIC_ECDSA_verify(hash_binary, 32, &signer_pub_key, &sig, &domain );

    if (!is_valid) {
        qsee_log(QSEE_LOG_MSG_ERROR,"ERROR: bad sign value verified when it should not have for arbitrary message %d\n", is_valid);
        rv |= -1;
    } else {
        qsee_log(QSEE_LOG_MSG_DEBUG,"SUCCESS: bad sign value correctly rejected for arbitrary message.\n");
    }
    rv_final |= rv;

    return (rv_final);
}

/**
  @brief 
    tz_app_generic_p256r1_func
*/
int tz_app_generic_p256r1_point_on_curve_func(void)
{
    int rv = 0, rv1 = 0;
    QSEE_qrlbn_ecc_domain_t domain;
    QSEE_affine_point_t point;

    // Point to be test on curve
    uint8 q_x_bin[] =
    {
     0x9c,0x83,0x8c,0x4f,0x1a,0x03,0x7a,0x00,0xf4,0x98,0x69,0x09,0xb2,0xbc,0x5c,0x11,0x0e,0xa5,0x92,0xb3,0x71,0x36,0x9c,0xaa,0x12,0x36,0xd8,0x03,0xcd,0x2b,0xad,0x8a
    };
    
    uint8 q_y_bin[] =
    {
     0xdb,0x69,0x56,0x57,0xc4,0x25,0x41,0x56,0x2c,0x21,0xf0,0x56,0x8b,0x0c,0xcc,0x0a,0x79,0xdc,0x04,0xa0,0xa3,0xef,0x87,0xc5,0x5d,0x96,0xc3,0xc7,0x38,0x57,0xef,0x3d
    };    
        

    QSEE_qrlbn_ecc_affine_point_t affine_point_q;

    bool is_on_curve = false, is_on_curve2 = false;

    // Initialize domain and public key
    rv = qsee_SW_GENERIC_ECC_init(&domain, generic_ecc_test_vectors[3].m, 
                            generic_ecc_test_vectors[3].a, 
                            generic_ecc_test_vectors[3].b, 
                            generic_ecc_test_vectors[3].G_x, 
                            generic_ecc_test_vectors[3].G_y, 
                            generic_ecc_test_vectors[3].n, 1);
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_GENERIC_ECC_init API failed");
        rv = -E_FAILURE;
        return rv;
    }

    // Alternative function call below to read into affine_point_q 
    rv1 = qsee_SW_GENERIC_ECC_convert_input_to_bigval(&affine_point_q.x, q_x_bin, 32, 256);
    //rv1 = qsee_SW_GENERIC_ECC_binary_to_bigval(&affine_point_q.x, q_x_bin, 32, &domain, QSEE_qrlbn_tag_X);
    rv |= rv1 < 0;
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"error converting affine_point_q x\n");
        rv = -E_FAILURE;
        return rv;
    }

    rv1 = qsee_SW_GENERIC_ECC_convert_input_to_bigval(&affine_point_q.y, q_y_bin, 32, 256);
    //rv1 = qsee_SW_GENERIC_ECC_binary_to_bigval(&affine_point_q.y, q_y_bin, 32, &domain, QSEE_qrlbn_tag_Y);
    rv |= rv1 < 0;
    affine_point_q.flags = 0;  // not the point at infinity
    if (rv) {
        qsee_log(QSEE_LOG_MSG_ERROR,"error converting affine_point_q y\n");
        rv = -E_FAILURE;
        return rv;
    }

    // test point on curve
    is_on_curve = qsee_SW_GENERIC_ECC_affine_point_on_curve(&affine_point_q, &domain);

    if (!is_on_curve) {
        qsee_log(QSEE_LOG_MSG_ERROR,"ECC_Generic ERROR: point is not on curve!\n");
        rv1 = -E_FAILURE;
    } 

    //compare to qsee_in_curveP
    point.infinity = false;
    
    qsee_ECC_hash_to_bigval(&point.x, q_x_bin, 32);

    qsee_ECC_hash_to_bigval(&point.y, q_y_bin, 32);
    
    is_on_curve2 = qsee_in_curveP(&point);

    if (!is_on_curve2) {
        qsee_log(QSEE_LOG_MSG_ERROR,"CurveP ERROR: point is not on curve!\n");
        rv1 = -E_FAILURE;
    } 
    
    if (is_on_curve != is_on_curve2) {
        qsee_log(QSEE_LOG_MSG_ERROR, "Error: P256 on curve comparison failed!\n");
        rv1 = -E_FAILURE;
    } else {
        qsee_log(QSEE_LOG_MSG_DEBUG, "SUCCESS: P256 on curve comparison succeeded!\n");
    }
    
    rv |= rv1;
    
    return (rv);
}

/**
   @brief
   Sample code for AES functional tests
*/
int tz_app_crypto_test(uint32 src, uint32 dst, uint32 length)
{
	qsee_log(QSEE_LOG_MSG_DEBUG,"Start tz_app_crypto_test packet size = 0x%x", length);
	return tz_app_crypto_aes_func(aes_test_vectors[0].alg,
                                 aes_test_vectors[0].mode,
                                 (uint8 *)(uintnt)src,
                                 length,
                                 aes_test_vectors[0].key,
                                 aes_test_vectors[0].key_len,
                                 aes_test_vectors[0].iv,
                                 (uint8 *)(uintnt)dst, 0);
}


int tz_app_ufcrypto_ivoverflow_test(void)

{
   uint8 iv[SW_AES_MAX_IV_SIZE+10] ;
   uint32 iv_len = SW_AES_MAX_IV_SIZE+1; 
   CryptoCntxHandle *cntx = 0;
   int status = E_SUCCESS;
 
  SW_CipherModeType mode = SW_CIPHER_MODE_GCM;

  qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_ufcrypto_ivoverflow_test API Started");
  /*------------------------------------------------------------------
     Init ctx
  ------------------------------------------------------------------*/
  if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES128))
  {
    qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
    status = -E_FAILURE;
  }
  
  /*--------------------------------------------------------------------
     Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
  {
    qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam mode API failed");
    status = -E_FAILURE;
  }
   
  /*--------------------------------------------------------------------
     Set IV : Set IV should return error;
  ----------------------------------------------------------------------*/
  if(E_SUCCESS == status &&
     qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, iv_len) == 0)
  {
    qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam iv buffer overflown");
    status = -E_FAILURE;
  }
  /*-------------------------------------------------------------------------------
    Free ctx
  --------------------------------------------------------------------------------*/  
  if (cntx)
  {
    if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES128))
    {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
      return -E_FAILURE;
    }
	cntx = 0;
  }
	  
  /***********************************************************************************
   Test for CCM mode
   **********************************************************************************/   
  mode = SW_CIPHER_MODE_CCM;

  /*------------------------------------------------------------------
    Init ctx
  ------------------------------------------------------------------*/
  if(0 != qsee_SW_Cipher_Init(&cntx, SW_CIPHER_ALG_AES128))
  {
    qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_Init API failed");
    status = -E_FAILURE;
  }
  
  /*--------------------------------------------------------------------
     Set AES mode
  ----------------------------------------------------------------------*/
  if (E_SUCCESS == status &&
      qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_MODE, &mode, sizeof(mode)) != 0)
  {
    qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam mode API failed");
    status = -E_FAILURE;
  }
   
  /*--------------------------------------------------------------------
     Set IV : Set IV should return error;
          1: try to set IV more than Max value i.e 13
	  2: try to set IV less than min value i.e 7
  ----------------------------------------------------------------------*/
  /*test for maximum iv size supported in ccm mode*/
  iv_len = SW_AES_CCM_MAX_NONCE_SIZE + 1;
  if(E_SUCCESS == status &&
     qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, iv_len) == 0)
  {
    qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam iv buffer overflown");
    status = -E_FAILURE;
  }
  /* test for min iv size supported in ccm mode*/
  iv_len = SW_AES_CCM_MIN_NONCE_SIZE - 1;
  if(E_SUCCESS == status &&
    qsee_SW_Cipher_SetParam(cntx, SW_CIPHER_PARAM_IV, iv, iv_len) == 0)
  {
    qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_SetParam iv buffer overflown");
    status = -E_FAILURE;
  }
  /*-------------------------------------------------------------------------------
    Free ctx
  --------------------------------------------------------------------------------*/  
  if (cntx)
  {
    if(0 != qsee_SW_Cipher_DeInit(&cntx, SW_CIPHER_ALG_AES128))
    {
      qsee_log(QSEE_LOG_MSG_ERROR,"qsee_SW_Cipher_DeInit API failed");
      status = -E_FAILURE;
    }
  }  

  if(E_SUCCESS == status)
  {
    qsee_log(QSEE_LOG_MSG_ERROR,"tz_app_ufcrypto_ivoverflow_test API passed");
  } 
	  
  return status ;	  
	  
}	

/**
   @brief
   Sample code to call crypto functional tests
*/
int tz_app_crypto_func_test(void)
{
   int status = E_SUCCESS;

   qsee_log(QSEE_LOG_MSG_DEBUG,"----------Start Sample App Crypto Funtional Tests ------------");

   if (tz_app_aes_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_crypto_kdf() < 0)
   {
      status = -E_FAILURE;
   }

  if (E_SUCCESS == status && tz_app_crypto_device_kdf() < 0)
  {  
    status = -E_FAILURE;
  }

   if (E_SUCCESS == status && tz_app_des_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_aes_xts_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_hash_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_hmac_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_cmac_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_hash_aes_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_aes_ccm_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_ecdh_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_ecdsa_func() < 0)
   {
      status = -E_FAILURE;
   }

  if (E_SUCCESS == status && tz_app_crypto_ecies_func() < 0)
  {
    status = -E_FAILURE;
  }

   if (E_SUCCESS == status && tz_app_sw_aes_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_sw_hash_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_sw_hmac_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_generic_ecdsa_func() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_generic_ecdh_func() < 0)
   {
      status = -E_FAILURE;
   }

  if (E_SUCCESS == status && tz_app_generic_p256k1_func() < 0)
  {  
    status = -E_FAILURE;
  }
  
  if (E_SUCCESS == status && tz_app_generic_p256r1_point_on_curve_func() < 0)
  {  
    status = -E_FAILURE;
  }  

  if (E_SUCCESS == status && tz_app_ufcrypto_ivoverflow_test() < 0)
  {  
    status = -E_FAILURE;
  } 
  
   qsee_log(QSEE_LOG_MSG_DEBUG,"----------End Sample App Crypto Funtional Tests --------------");

   return status;
}

/**
   @brief
   Sample code to call crypto performance test
*/
int tz_app_crypto_perf_test(void)
{
   int status = E_SUCCESS;

   //Init the data buffer
   memset(aes_performance, 0xed, sizeof(aes_performance));

	qsee_log(QSEE_LOG_MSG_DEBUG,"----------Start Sample App Crypto Performance Tests ------------");

   if (tz_app_aes_perf() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_des_perf() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_aes_xts_perf() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_hash_perf() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_hmac_perf() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_cmac_perf() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_hash_aes_perf() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_sw_aes_perf() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_sw_aes_gcm_perf() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_sw_hash_perf() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_sw_hmac_perf() < 0)
   {
      status = -E_FAILURE;
   }

   if (E_SUCCESS == status && tz_app_kdf_perf() < 0)
   {
      status = -E_FAILURE;
   }

   qsee_log(QSEE_LOG_MSG_DEBUG,"----------End Sample App Crypto Performance Tests --------------");

   return status;
}
