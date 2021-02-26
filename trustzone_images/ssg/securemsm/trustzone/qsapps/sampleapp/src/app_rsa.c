/*
  @file app_rsa.c
  @brief This file is a sample code that show how QSEE RSA API's can be used.

*/
/*===========================================================================
  Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================


  # when       who     what, where, why
  # --------   ---     ---------------------------------------------------------
  # 5/31/2012  c_mujeeb moving RSA tests from TZBSPTest to sampleapp
  ===========================================================================*/
#include "IxErrno.h"
#include "com_dtypes.h"
#include "qsee_cipher.h"
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_hash.h"
#include "qsee_timer.h"
#include "qsee_rsa.h"
#include "secrsa_err.h"
#include "qsee_uf_sha.h"
#include "crypto_rsa_test_vectors.h"

void init_key(QSEE_S_BIGINT **key, const char *key_str, int key_len)
{
   *key = (QSEE_S_BIGINT *)qsee_malloc(sizeof(QSEE_S_BIGINT));
   if( NULL == *key )
   {
      return;
   }
   qsee_BIGINT_read_radix(&(*key)->bi, (const char*)(uintnt)key_str, 16);
   (*key)->bi.n = key_len/sizeof(QSEE_BLONG);
   (*key)->sign = 0; // Positive
}

void free_key(QSEE_S_BIGINT *key)
{
   if ( key != NULL )
      qsee_free (key);
   key = NULL;
}


int tzt_test_crypto_rsa_enc_dec_vect(  uint8                   operation,
                                       uint32                  hashidx,
                                       uint32                  padding_type,
                                       void*                   padding_info,
                                       uint32                  padding_info_len,
                                       uint32*                 _key,
                                       const unsigned char     *msg,
                                       uint32                  msg_len,
                                       unsigned char           *emsg,
                                       uint32                  emsg_len)
{
   test_rsa_key_t          *test_key = (test_rsa_key_t*)(uintnt)_key;
   int                     key_size;
   QSEE_RSA_KEY              *key = NULL;
   uint8                   *enc_msg = NULL;
   uint32                  enc_msg_len = 256;
   uint8                   *dec_msg = NULL;
   uint32                  dec_msg_len = 256;
   int                     result;
   const uint32            MSG_LEN = 256;

   key = (QSEE_RSA_KEY*)qsee_malloc(sizeof(QSEE_RSA_KEY));
   if( NULL == key )
   {
      return -E_FAILURE;
   }

   enc_msg = (uint8*)qsee_malloc(MSG_LEN);
   if( NULL == enc_msg )
   {
      return -E_FAILURE;
   }

   dec_msg = (uint8*)qsee_malloc(MSG_LEN);
   if( NULL == dec_msg )
   {
      return -E_FAILURE;
   }

   key_size = test_key->nbits/8;
   memset(key, 0, sizeof(QSEE_RSA_KEY));
   init_key(&(key->d), test_key->d, key_size);
   init_key(&(key->e), test_key->e, key_size);
   init_key(&(key->N), test_key->n, key_size);

   do   {
      if( operation == 0 )
      {
         key->type = QSEE_RSA_KEY_PUBLIC;
         result = qsee_rsa_encrypt(key, (QSEE_RSA_PADDING_TYPE)padding_type, padding_info,
                                   msg, msg_len, enc_msg, (int*)(uintnt)&enc_msg_len);
         if( CE_SUCCESS != result )
         {
           qsee_log(QSEE_LOG_MSG_ERROR, "qsee_rsa_encrypt error result=%d", result);
           break;
         }

         key->type = QSEE_RSA_KEY_PRIVATE;
         result = qsee_rsa_decrypt(key, (QSEE_RSA_PADDING_TYPE)padding_type, padding_info,
                                   enc_msg, enc_msg_len,dec_msg, (int*)(uintnt)&dec_msg_len);

         if( CE_SUCCESS != result )
         {
           qsee_log(QSEE_LOG_MSG_ERROR, "qsee_rsa_decrypt error result=%d", result);
           break;
         }

         if( 0 != memcmp(msg, dec_msg, dec_msg_len) )
         {
           qsee_log(QSEE_LOG_MSG_DEBUG, "memcmp failed");
           result = -E_FAILURE;
           break;
         }        
      }
      else
      {
         // Decrypt Test
         key->type = QSEE_RSA_KEY_PRIVATE;
         dec_msg_len = 256;
         result = qsee_rsa_decrypt(key, (QSEE_RSA_PADDING_TYPE)padding_type, padding_info,
                                   emsg, emsg_len, dec_msg, (int*)(uintnt)&dec_msg_len);

         if( CE_SUCCESS != result )
         {
           qsee_log(QSEE_LOG_MSG_ERROR, "qsee_rsa_decrypt error result2=%d", result);
           break;
         }

         if( 0 != memcmp(msg, dec_msg, dec_msg_len) )
         {
            result = -E_FAILURE;
            break;
         }
      }

   } while( 0 );

   free_key(key->d);
   free_key(key->e);
   free_key(key->N);
   qsee_free(key);
   qsee_free(enc_msg);
   qsee_free(dec_msg);

   // translate CE errors to TZ errors
   if( CE_ERROR_MESSAGE_TOO_LONG == result )
   {
      result = -E_DATA_TOO_LARGE;
   }
   else if( CE_SUCCESS != result )
   {
      result = -E_FAILURE;
   }
   else if( CE_SUCCESS == result )
   {
      result = E_SUCCESS;
   }

   return result;
}

int test_crypto_rsa_enc_dec(void)
{
   int                     k;
   int                     msg_len;
   uint8                   msg[256] = {0};
   test_rsa_key_t          *key;
   QSEE_RSA_PADDING_TYPE     padding_type;
   QSEE_RSA_OAEP_PAD_INFO    oaep_pad_info;
   void                    *padding_info = NULL;
   uint32                  test_count = 0;
   uint32                  pass_test_count = 0;
   uint32                  fail_test_count = 0;
   int                     ret = CE_SUCCESS;
   static int              operation = 2;

   memset(&oaep_pad_info, 0, sizeof(QSEE_RSA_OAEP_PAD_INFO));

   for ( k = 0; k < sizeof(oaep_key_list) / sizeof(test_rsa_key_t*); k++ )
   {
      //load the key
      key = oaep_key_list[k];
      memset(msg, 0, 256);
      //load message for the key
      msg_len = oaep_test_vectors[k].msg_len;
      memcpy(msg,oaep_test_vectors[k].msg,msg_len);

      // paddind data
      if ( RSA_PADDING_VER_1_5 == oaep_test_vectors[k].padding_version )
      {
         padding_type = QSEE_RSA_PAD_PKCS1_V1_5_ENC;
         padding_info = NULL;
      }
      else
      {
         padding_type = QSEE_RSA_PAD_PKCS1_OAEP;
         oaep_pad_info.hashidx = oaep_test_vectors[k].hashidx;
         oaep_pad_info.label = NULL;
         oaep_pad_info.labellen = 0;
         padding_info = (void*)(uintnt)&oaep_pad_info;
      }
      /*
       * For this test vectors perform Encrypt (operation = 0 ) and Decrypt (operation = 1 )
       */
      for(operation=2; operation;operation--)
      {
         if(operation == 2)
         {
            ret = tzt_test_crypto_rsa_enc_dec_vect(0,
                                                   oaep_test_vectors[k].hashidx,
                                                   padding_type,
                                                   padding_info,
                                                   sizeof(oaep_pad_info),
                                                   (uint32*)(uintnt)(key),
                                                   (const unsigned char*)(uintnt)msg,
                                                   msg_len,
                                                   NULL,
                                                   0);
         }
         else
         {
            ret = tzt_test_crypto_rsa_enc_dec_vect(1,
                                                   oaep_test_vectors[k].hashidx,
                                                   padding_type,
                                                   padding_info,
                                                   sizeof(oaep_pad_info),
                                                   (uint32*)(uintnt)(key),
                                                   (const unsigned char*)(uintnt)msg,
                                                   msg_len,
                                                   (unsigned char*)(uintnt)(oaep_test_vectors[k].encrypted_msg),
                                                   oaep_test_vectors[k].encrypted_msg_len);
         }

         test_count++;
         if( E_SUCCESS == ret )
         {
            qsee_log(QSEE_LOG_MSG_DEBUG, "%s Test #:%d bit= %d Message len: %d Padding: %s Padding hash: %d, Result: PASS",
                     OAEP_string[operation%2],test_count, key->nbits, msg_len, pad_version_string[oaep_test_vectors[k].padding_version], 
                     oaep_pad_info.hashidx);
            pass_test_count++;
         }
         else if ( E_DATA_TOO_LARGE == -ret )
         {
            qsee_log(QSEE_LOG_MSG_DEBUG, "Test #:%d Encrypt/Decrypt Message len (too long) :%d Padding Type :%s, Padding hash: %d, Seed length :%d PASSED",
                     test_count, msg_len, pad_version_string[oaep_test_vectors[k].padding_version], oaep_pad_info.hashidx, msg_len);
            pass_test_count++;
          }
          else
          {
            qsee_log(QSEE_LOG_MSG_ERROR, "Test #:%d Encrypt/Decrypt Message len :%d Padding Type :%s, Seed length :%d FAILED",
                     test_count, msg_len, pad_version_string[oaep_test_vectors[k].padding_version], oaep_test_vectors[k].label_len);
            fail_test_count++;
         }
      }
      // next time this method is called, it does decrypt for 1024 and 2048
   }
   qsee_log(QSEE_LOG_MSG_DEBUG, "**********Total Encrypt/Decrypt Tests :%d Tests Passed :%d Tests Failed :%d **********", test_count, pass_test_count, fail_test_count);
   return fail_test_count;
}

/**
   @brief
   Call QSEE HASH software crypto API's
*/
int sw_hash(SW_Auth_Alg_Type alg,
                              const uint8 *msg,
                              uint32 msg_len,
                              uint8 *digest,
                              uint32_t digest_len)
{
   int status = E_SUCCESS;

   IovecListType   ioVecIn;
   IovecListType   ioVecOut;
   IovecType       IovecIn;
   IovecType       IovecOut;

   CryptoCntxHandle *cntx = NULL;

   //Clear result digest
   memset(digest, 0, sizeof(digest_len));

   /* Input IOVEC */
   ioVecIn.size = 1;
   ioVecIn.iov = &IovecIn;
   ioVecIn.iov[0].dwLen = msg_len;
   ioVecIn.iov[0].pvBase = (void *)msg;

   /* Output IOVEC */
   ioVecOut.size = 1;
   ioVecOut.iov = &IovecOut;
   ioVecOut.iov[0].dwLen = digest_len;
   ioVecOut.iov[0].pvBase = digest;

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
         break;
      }

   } while (0);

   qsee_SW_Hash_Deinit(&cntx);

   return status;
}

int tzt_test_crypto_rsa_sign_verify_vect(uint8 operation,
                                         QSEE_HASH_IDX           hashidx,
                                         uint32                  padding_type,
                                         void*                   padding_info,
                                         uint32                  padding_info_len,
                                         uint32*                 _key,
                                         const unsigned char *   msg,
                                         uint32                  msg_len,
                                         unsigned char           *sign,
                                         uint32                  sign_len)
{
   test_rsa_key_t          *test_key = (test_rsa_key_t*)(uintnt)_key;
   int                     key_size = 0;
   QSEE_RSA_KEY              *key = NULL;
   uint8                   *signature = NULL;
   uint32                  signature_len = 384;
   uint8                   hash[QSEE_SHA256_HASH_SZ] = {0};
   uint32                  hash_len = QSEE_SHA256_HASH_SZ;
   int                     result;

   key = (QSEE_RSA_KEY*)qsee_malloc(sizeof(QSEE_RSA_KEY));
   if( NULL == key )
   {
      return -E_FAILURE;
   }

   signature = (uint8*)qsee_malloc(signature_len);
   if( NULL == signature )
   {
      return -E_FAILURE;
   }

   key_size = test_key->nbits/8;
   memset(key, 0, sizeof(QSEE_RSA_KEY));
   init_key(&(key->d), test_key->d, key_size);
   init_key(&(key->e), test_key->e, key_size);
   init_key(&(key->N), test_key->n, key_size);

   do
   {
    /* hash the message */
    if (hashidx == QSEE_HASH_IDX_MD5)
    {
        hash_len = SW_MD5_DIGEST_SIZE;
              
        result =  sw_hash((SW_Auth_Alg_Type)(hashidx), msg, msg_len, hash, hash_len);
        if( CE_SUCCESS != result )
        {
          qsee_log(QSEE_LOG_MSG_ERROR, "SW_Hash in rsa_sign_verify failed operation: %d, result=%d", operation, result);
          break;
        }        
        
    }    
    else {
    
        if(hashidx == QSEE_HASH_IDX_SHA1)
        {
            hash_len = QSEE_SHA1_HASH_SZ;
        }
        
        result = qsee_hash((QSEE_HASH_ALGO_ET)(hashidx), msg, msg_len, hash, hash_len);
        if( CE_SUCCESS != result )
        {
          qsee_log(QSEE_LOG_MSG_ERROR, "tzbsp_hash in rsa_sign_verify failed operation: %d, result=%d",operation, result);
          break;
        }
    }

      if( operation == 0 )
      {
         /* Sign Test */
         key->type = QSEE_RSA_KEY_PRIVATE;
         //val = qsee_get_uptime();
         result = qsee_rsa_sign_hash(key, (QSEE_RSA_PADDING_TYPE)padding_type, padding_info, hashidx,
                                     hash, hash_len, signature, (int*)(uintnt)&signature_len);
         if( CE_SUCCESS != result )
         {
            break;
         }
         /* now pass the signature you just generated to the signature verify method
            if that returns true, then return success.
         */
         key->type = QSEE_RSA_KEY_PUBLIC;
         result = qsee_rsa_verify_signature(key, (QSEE_RSA_PADDING_TYPE)padding_type, padding_info, hashidx,
                                            hash, hash_len, signature, signature_len);
         if( CE_SUCCESS != result )
         {
            qsee_log(QSEE_LOG_MSG_ERROR, "verify Signature fail");
            break;
         }

      }
      else
      {
         /* Verify Signature Test */
         /* here pass the signature that was passed by the caller */
         key->type = QSEE_RSA_KEY_PUBLIC;
         result = qsee_rsa_verify_signature(key, (QSEE_RSA_PADDING_TYPE)padding_type, padding_info, hashidx,
                                            hash, hash_len, sign, sign_len);
         if( CE_SUCCESS != result )
         {
            break;
         }
      }
   } while( 0 );
   free_key(key->d);
   free_key(key->e);
   free_key(key->N);
   qsee_free(key);
   qsee_free(signature);

   /* translate CE errors to TZ errors */
   if( CE_ERROR_MESSAGE_TOO_LONG == result )
   {
      result = -E_DATA_TOO_LARGE;
   }
   else if( CE_SUCCESS != result )
   {
      result = -E_FAILURE;
   }
   else if( CE_SUCCESS == result )
   {
      result = E_SUCCESS;
   }

   return result;
}

int test_crypto_rsa_sign_verify(void)
{
   int                     k;
   int                     msg_len;
   uint8                   msg[256] = {0};
   test_rsa_key_t          *key;
   QSEE_RSA_PADDING_TYPE     padding_type;
   QSEE_RSA_PSS_PAD_INFO     pss_pad_info;
   void                    *padding_info = NULL;
   uint32                  test_count = 0;
   uint32                  pass_test_count = 0;
   uint32                  fail_test_count = 0;
   int                     ret = CE_SUCCESS;
   int                     operation = 2 ;

   for ( k = 0; k < sizeof(pss_key_list) / sizeof(test_rsa_key_t*); k++ )
   {
      key = pss_key_list[k];
      memset(msg, 0, 256);
      msg_len = pss_test_vectors[k].msg_len;
      memcpy(msg,pss_test_vectors[k].msg,msg_len);

      // padding info
      if ( RSA_PADDING_VER_1_5 == pss_test_vectors[k].padding_version )
      {
         padding_type = QSEE_RSA_PAD_PKCS1_V1_5_SIG;
         padding_info = NULL;
      }
      else
      {
         padding_type = QSEE_RSA_PAD_PKCS1_PSS;
         pss_pad_info.hashidx = pss_test_vectors[k].hashidx;
         pss_pad_info.saltlen = pss_test_vectors[k].salt_len;
         padding_info = (void*)(uintnt)&pss_pad_info;
      }

      /*
       * For this test vectors perform Sign (operation = 0 ) and Verify (operation = 1 )
       */
      for(operation=2;operation;operation--)
      {
         //Sign //
         // pass null & 0 for signature & signature len //
         if(operation == 2)
         {
            ret = tzt_test_crypto_rsa_sign_verify_vect (0,
                                                        pss_test_vectors[k].hashidx,
                                                        padding_type,
                                                        padding_info,
                                                        sizeof(padding_info),
                                                        (uint32*)(uintnt)(key),
                                                        (const unsigned char *)(uintnt)(msg),
                                                        msg_len,
                                                        NULL,
                                                        0);

         }
         else
         {
            //Verify
            // pass the signature & signature len from the test vectors
            ret = tzt_test_crypto_rsa_sign_verify_vect(1,
                                                       pss_test_vectors[k].hashidx,
                                                       padding_type,
                                                       padding_info,
                                                       sizeof(padding_info),
                                                       (uint32*)(uintnt)(key),
                                                       (const unsigned char*)(uintnt)(msg),
                                                       msg_len,
                                                       (unsigned char*)(uintnt)(pss_test_vectors[k].signature_msg),
                                                       pss_test_vectors[k].signature_msg_len);
         }

            test_count++;
            if ( E_SUCCESS == ret )
            {
                qsee_log(QSEE_LOG_MSG_DEBUG, "%s Test #:%2d  bit :%d Message len :%3d Padding Type :%s, Hash idx: %d, Salt length :%2d PASS",
                PSS_string[operation%2],test_count, key->nbits, msg_len, pad_version_string[pss_test_vectors[k].padding_version], 
                pss_test_vectors[k].hashidx, pss_test_vectors[k].salt_len);
                pass_test_count++;
            }
            else if( E_DATA_TOO_LARGE == -ret )
            {
                qsee_log(QSEE_LOG_MSG_DEBUG, "%s Test #:%2d Sign/Verify Message len (too long) :%d Padding Type :%s, Hash idx: %d, Salt length :%2d PASS",
                PSS_string[operation%2],test_count, msg_len, pad_version_string[pss_test_vectors[k].padding_version], 
                pss_test_vectors[k].hashidx, pss_test_vectors[k].salt_len);
                pass_test_count++;
            }
            else
            {
                qsee_log(QSEE_LOG_MSG_DEBUG, "%s Test #:%2d  bit :%d Message len :%3d Padding Type :%s, Hash idx: %d, Salt length :%2d FAIL",
                PSS_string[operation%2],test_count, key->nbits, msg_len, pad_version_string[pss_test_vectors[k].padding_version],
                pss_test_vectors[k].hashidx, pss_test_vectors[k].salt_len);
                fail_test_count++;
            }
       }
    }
   qsee_log(QSEE_LOG_MSG_DEBUG, "**********Total Sign/Verify Tests :%d Tests Passed :%dTests Failed :%d **********", test_count, pass_test_count, fail_test_count);
   return fail_test_count;
}

int test_crypto_rsa_byte_count(void)
{
   int count = 0;
   uint32 data = 0xFF;
   QSEE_S_BIGINT *ptr;

   //Allocate memory
   ptr = (QSEE_S_BIGINT *)qsee_malloc(sizeof(QSEE_S_BIGINT));
   if(NULL == ptr)
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"RSA Byte count Test Malloc Failed");
      return -E_FAILURE;
   }

   ptr->bi.n = 2;
   ptr->bi.a[0] = data;
   ptr->bi.a[1] = data;

   count = qsee_util_count_bytes(ptr);

   qsee_free(ptr);

   if(count == 5)
   {
      qsee_log(QSEE_LOG_MSG_DEBUG,"RSA Byte count Test Passed: Count %d", count);
   }
   else
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"RSA Byte count Test Failed: Count %d", count);
   }

   return E_SUCCESS;
}

/*
 *  API to test CRT using key gen and then decrypt using CRT and without CRT and compare
 */

static  uint8 msg[256] = {0};

int test_crypto_rsa_CRT_verify(void)
{
   unsigned long long total_time;
   QSEE_RSA_KEY              *key = NULL;
   uint8                   *enc_msg_1 = NULL;
   uint32                  enc_msg_len_1 = 256;
   uint8                   *dec_msg = NULL;
   //uint32                  dec_msg_len = 256;
   uint8                   *dec_msg_1 = NULL;
   //uint32                  dec_msg_len_1 = 256;
   int                     result;
   const uint32            MSG_LEN = 256;
   uint32 len = 128;
   uint32 msg_len_act = 10;
   unsigned char pub_exp[] = {0x00, 0x00, 0x01, 0x00, 0x01};

   memset(msg, 0x1, 256);

   key = (QSEE_RSA_KEY*)qsee_malloc(sizeof(QSEE_RSA_KEY));
   if( NULL == key )
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"key malloc failed");
      return -E_NO_MEMORY;
   }

   enc_msg_1 = (uint8*)qsee_malloc(MSG_LEN);
   if( NULL == enc_msg_1 )
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"enc_msg_1 malloc failed");
      qsee_free(key);
      return -E_NO_MEMORY;
   }

   dec_msg = (uint8*)qsee_malloc(MSG_LEN);
   if( NULL == dec_msg )
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"dec_msg malloc failed");
      qsee_free(key);
      qsee_free(enc_msg_1);
      return -E_NO_MEMORY;
   }

   dec_msg_1 = (uint8*)qsee_malloc(MSG_LEN);
   if( NULL == dec_msg_1 )
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"dec_msg_1 malloc failed");
      qsee_free(key);
      qsee_free(enc_msg_1);
      qsee_free(dec_msg);
      return -E_NO_MEMORY;
   }

   if ( qsee_util_init_s_bigint(&(key->e))|| qsee_util_init_s_bigint(&(key->d)) ||
        qsee_util_init_s_bigint(&(key->N)) || qsee_util_init_s_bigint(&(key->p)) || qsee_util_init_s_bigint(&(key->q)) ||
        qsee_util_init_s_bigint(&(key->qP)) || qsee_util_init_s_bigint(&(key->dP))||qsee_util_init_s_bigint(&(key->dQ)) )
   {
      qsee_free(key);
      qsee_free(enc_msg_1);
      qsee_free(dec_msg_1);
      qsee_free(dec_msg);
      qsee_log(QSEE_LOG_MSG_ERROR,"malloc failed");
      return -E_NO_MEMORY;
   }


   key->type = QSEE_RSA_KEY_PRIVATE_PUBLIC;

   total_time = qsee_get_uptime();

   if(CE_SUCCESS != qsee_rsa_key_gen(key,
                                     256,
                                     pub_exp,
                                     sizeof(pub_exp)))
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"key gen failed");
      qsee_free(key);
      qsee_free(dec_msg);
      qsee_free(enc_msg_1);
      qsee_free(dec_msg_1);
      return -E_FAILURE;
   }

   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"time for pvt public key gen %llu",total_time);

   key->type = QSEE_RSA_KEY_PRIVATE_CRT_PUBLIC;

   total_time = qsee_get_uptime();

   if(CE_SUCCESS != qsee_rsa_key_gen(key,
                                     256,
                                     pub_exp,
                                     sizeof(pub_exp)))
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"key gen failed");
      qsee_free(key);
      qsee_free(dec_msg);
      qsee_free(enc_msg_1);
      qsee_free(dec_msg_1);
      return -E_FAILURE;
   }

   total_time = qsee_get_uptime() - total_time;
   qsee_log(QSEE_LOG_MSG_DEBUG,"time for pvt public CRT key gen %llu",total_time);


   key->type = QSEE_RSA_KEY_PUBLIC;
   result = qsee_rsa_encrypt(key, (QSEE_RSA_PADDING_TYPE)QSEE_RSA_PAD_PKCS1_V1_5_ENC, NULL,
                             msg, msg_len_act, enc_msg_1, (int*)&enc_msg_len_1);
   if( CE_SUCCESS != result )
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"public enc failed");
      qsee_free(key);
      qsee_free(dec_msg);
      qsee_free(enc_msg_1);
      qsee_free(dec_msg_1);
      return -E_FAILURE;
   }

   len = MSG_LEN;
   key->type = QSEE_RSA_KEY_PRIVATE_CRT;
   result = qsee_rsa_decrypt(key, (QSEE_RSA_PADDING_TYPE)QSEE_RSA_PAD_PKCS1_V1_5_ENC, NULL,
                             enc_msg_1, enc_msg_len_1,dec_msg, (int*)&len);

   if( CE_SUCCESS != result )
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"private CRT decrypt failed ");
      qsee_free(key);
      qsee_free(dec_msg);
      qsee_free(enc_msg_1);
      qsee_free(dec_msg_1);
      return -E_FAILURE;
   }
   len = MSG_LEN;

   key->type = QSEE_RSA_KEY_PRIVATE;
   result = qsee_rsa_decrypt(key, (QSEE_RSA_PADDING_TYPE)QSEE_RSA_PAD_PKCS1_V1_5_ENC, NULL,
                             enc_msg_1, enc_msg_len_1,dec_msg_1, (int*)&len);

   if( CE_SUCCESS != result )
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"private decrypt failed ");
      qsee_free(key);
      qsee_free(dec_msg);
      qsee_free(enc_msg_1);
      qsee_free(dec_msg_1);
      return -E_FAILURE;
   }
   if( 0 != memcmp(dec_msg_1, dec_msg, msg_len_act) )
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"compare dec with PT failed ");
   }

   if( 0 != memcmp(msg, dec_msg, msg_len_act) )
   {
      qsee_log(QSEE_LOG_MSG_ERROR,"compare dec with PT failed ");
   }
   if (key->e != NULL)
   {
      memset(key->e, 0, sizeof(QSEE_S_BIGINT));
      qsee_util_free_s_bigint(key->e);
   }

   if (key->d != NULL)
   {
      memset(key->d, 0, sizeof(QSEE_S_BIGINT));
      qsee_util_free_s_bigint(key->d);
   }

   if (key->N != NULL)
   {
      memset(key->N, 0, sizeof(QSEE_S_BIGINT));
      qsee_util_free_s_bigint(key->N);
   }

   if (key->p != NULL)
   {
      memset(key->p, 0, sizeof(QSEE_S_BIGINT));
      qsee_util_free_s_bigint(key->p);
   }

   if (key->q != NULL)
   {
      memset(key->q, 0, sizeof(QSEE_S_BIGINT));
      qsee_util_free_s_bigint(key->q);
   }

   if (key->dP != NULL)
   {
      memset(key->dP, 0, sizeof(QSEE_S_BIGINT));
      qsee_util_free_s_bigint(key->dP);
   }

   if (key->dQ != NULL)
   {
      memset(key->dQ, 0, sizeof(QSEE_S_BIGINT));
      qsee_util_free_s_bigint(key->dQ);
   }

   if (key->qP != NULL)
   {
      memset(key->qP, 0, sizeof(QSEE_S_BIGINT));
      qsee_util_free_s_bigint(key->qP);
   }

   qsee_free(key);
   qsee_free(dec_msg);
   qsee_free(enc_msg_1);
   qsee_free(dec_msg_1);

   qsee_log(QSEE_LOG_MSG_DEBUG,"test_crypto_rsa_CRT_verify Success");

   return E_SUCCESS;

}


/*
 *   Sampleclient calls this method
 */
int tz_app_crypto_rsa_test(void)
{
   int status = E_SUCCESS;
   qsee_log(QSEE_LOG_MSG_DEBUG, "**********RSA TESTS START**********");

   status |= test_crypto_rsa_byte_count();
   status |= test_crypto_rsa_enc_dec();
   status |= test_crypto_rsa_sign_verify();
   status |= test_crypto_rsa_CRT_verify();

   qsee_log(QSEE_LOG_MSG_DEBUG,"**********RSA TESTS END**********");

   return status;
}
