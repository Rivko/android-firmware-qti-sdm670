// Copyright (c) 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.

#include "IKeyGen_invoke.h"
#include "qsee_services.h"
#include "qsee_env.h"
#include "qsee_log.h"
#include "stringl.h"
#include "ICrypto.h"
#include "CCrypto.h"

static inline int32_t
CKeyGen_generateKey(void *cxt,
                    int32_t keyType_val,
                    void *key_ptr,
                    size_t key_len,
                    size_t *key_lenout)
{
  // IPC guarantees non-NULL pointers

  int32_t err = Object_ERROR;
  uint8_t nullkey;
  char key_context[] = {"Key Context to Generate app key"};
  char key_label[] = {"Key Label to Generate app key"};
  Object s_crypto = Object_NULL;

  do {

    if ((IKeyGen_AES256 != keyType_val) || (key_len < IKeyGen_AES256_LEN))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "KeyGenTest invalid keyType: %d, key_len: 0x%x ", keyType_val, key_len);
      break;
    }

    err = qsee_open(CCrypto_UID, &s_crypto);
    if (!Object_isOK(err) || Object_isNull(s_crypto))
    {
      /* s_crypto contents undefined value for error case, set it back to NULL object
         to avoid Object_RELEASE invoke on a garbage object. */
      s_crypto = Object_NULL;
      qsee_log(QSEE_LOG_MSG_ERROR, "KeyGenTest failed to open CCrypto Object uid: %d, err: %x", CCrypto_UID, err);
      break;
    }

    qsee_log(QSEE_LOG_MSG_DEBUG, "KeyGenTest CCrypto Object uid: %d oppened ", CCrypto_UID);

    /*generating key. */
    err = ICrypto_kdf(s_crypto,
                      (void*)&nullkey, 0,
                      (void*)(key_label), strlen(key_label),
                      (void*)(key_context), strlen(key_context),
                      key_ptr, IKeyGen_AES256_LEN, key_lenout);
    if (Object_OK != err)
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "KeyGenTest ICrypto_kdf returned error, err: %x", err);
      break;
    }

    qsee_log(QSEE_LOG_MSG_DEBUG, "KeyGenTest CCrypto Key Gen Success, key_lenout: %d ", *key_lenout);

    err = Object_OK;
  } while(0);

  Object_RELEASE_IF(s_crypto);
  return err;
}


// This implementation does not require a context record, so `retain` and
// `release` are no-ops.

#define CKeyGen_release(ctx)   Object_OK
#define CKeyGen_retain(ctx)    Object_OK

static IKeyGen_DEFINE_INVOKE(CKeyGen_invoke, CKeyGen_, void*)

int32_t CKeyGen_open(Object cred, Object *objOut)
{
   *objOut = (Object) { CKeyGen_invoke, NULL };
   return Object_OK;
}
