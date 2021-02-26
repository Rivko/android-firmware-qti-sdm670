/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define QSEE_CONNECTOR_SERVICE_NAME com.qualcomm.qti.qseeproxysample

#include <jni.h>
#include <android/log.h>
#include <auth.h>
#include <cstring>
#include <QSEEComAPI.h>
#include <QTEEConnectorClient.h>
#include <stdlib.h>
#include <string>

#define SECURE_EXT_AUTH_CMD_GET_TOKEN           1

#undef LOG_TAG
#define LOG_TAG "SecureExtAuthJNI"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define DEBUG true
#if DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,   LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...) do {} while (0)
#endif


using namespace android;

struct send_cmd{
  uint32_t cmd_id;
  union {
    /** GetTokenCmd **/
    struct {
      char secAppName[QC_SEC_APP_NAME_LEN];
      uint8_t nonce[QC_AUTH_NONCE];
    };
  };
} __attribute__ ((packed));
struct send_cmd_rsp{
  union {
    /** GetTokenRsp **/
    struct {
      uint16_t result;
      qc_user_verification_token_t token;
    };
  };
} __attribute__ ((packed));

static QTEE::QTEEConnectorClient *client = NULL;

#ifdef __cplusplus
extern "C" {
#endif

static long mUserEntityId = 987654321;
static const char *path = "/firmware/image";
static const char *name = "sampleextauth";

int getToken(char const *secAppName, uint8_t const (*nonce)[QC_AUTH_NONCE], uint16_t &result, qc_user_verification_token_t &token)
{
  struct send_cmd cmd = {0};
  struct send_cmd_rsp msgrsp = {0};
  int32_t ret = 1;
  int32_t req_len = 0;
  int32_t rsp_len = 0;

  do {
    if (strlen(secAppName) > sizeof(cmd.secAppName)-1) {
      ALOGE("secAppName too long");
      break;
    }
    cmd.cmd_id = SECURE_EXT_AUTH_CMD_GET_TOKEN;
    memcpy(cmd.secAppName, secAppName, strlen(secAppName));
    memcpy(cmd.nonce, *nonce, QC_AUTH_NONCE);


    /* populate the data in shared buffer */
    if (client == NULL) {
      break;
    }

    req_len = sizeof(cmd);
    rsp_len = sizeof(msgrsp);

    /* send request from HLOS to QSEApp */
    ret = client->sendCommand(&cmd, req_len, &msgrsp, rsp_len);
    if (ret) {
      LOGE("send command %d failed with ret = %d\n", cmd.cmd_id,ret);
      break;
    }
    token = msgrsp.token;
    result = msgrsp.result;
  } while(0);
  return ret;
}


JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
  int req_len = sizeof(struct send_cmd);
  int rsp_len = sizeof(struct send_cmd_rsp);
  int buf_size = 0;

  if (req_len & QSEECOM_ALIGN_MASK)
      req_len = QSEECOM_ALIGN(req_len);

  if (rsp_len & QSEECOM_ALIGN_MASK)
      rsp_len = QSEECOM_ALIGN(rsp_len);

  buf_size = req_len + rsp_len;

  do {
    if (client != NULL) {
      LOGE("Application already loaded?");
      break;
    }

    client = new QTEE::QTEEConnectorClient(path,name,buf_size);

    if (client == NULL) {
      LOGE("Error creating client");
      break;
    }

    client->load();

  } while(0);
  return JNI_VERSION_1_2;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved)
{
  int rv = 0;

  if (client != NULL) {
    client->unload();
    delete client;
    client = 0;
  }
}

JNIEXPORT jobject Java_com_qualcomm_qti_auth_secureextauthservice_SecureExtAuthImpl_nativeGetToken(
    JNIEnv* env, jclass cls, jbyteArray jnonce, jstring jsecAppName) {

  int rv = 0;
  qc_user_verification_token_t token = {0};
  uint16_t result = 0;
  uint8_t nonce[QC_AUTH_NONCE] = {0};

  if(NULL == jnonce){
    LOGE("jnonce is NULL");
    return NULL;
  }
  size_t nonceLen = env->GetArrayLength(jnonce);
  LOGD("nonce length %d. max is %d", nonceLen, QC_AUTH_NONCE);
  if (QC_AUTH_NONCE < nonceLen) {
    LOGE("nonce too long %d. max is %d", nonceLen, QC_AUTH_NONCE);
    return NULL;
  }

  if(NULL == jsecAppName){
    LOGE("jsecAppName is NULL");
    return NULL;
  }
  size_t secAppNameLength = (size_t)env->GetStringLength(jsecAppName);
  LOGD("jsecAppName length %d. max is %d", secAppNameLength, QC_SEC_APP_NAME_LEN);
  if (QC_SEC_APP_NAME_LEN < secAppNameLength) {
    LOGE("jsecAppName too long %d. max is %d", secAppNameLength, QC_SEC_APP_NAME_LEN);
    return NULL;
  }
  const char* secAppName = env->GetStringUTFChars(jsecAppName, 0);
  if(secAppName == NULL) {
    LOGD("secAppName NULL");
    return NULL;
  }

  jbyte* noncePtr = env->GetByteArrayElements(jnonce, NULL);
  if(noncePtr == NULL) {
    LOGD("noncePtr NULL");
    return NULL;
  }
  memcpy(nonce, noncePtr, QC_AUTH_NONCE);
  env->ReleaseByteArrayElements(jnonce, noncePtr, 0);

  rv = getToken(secAppName, &nonce, result, token);
  env->ReleaseStringUTFChars(jsecAppName, secAppName);
  if(rv != 0) {
    LOGE("Error from secure app");
    return NULL;
  }

  jclass getTokenRspClass =
    env->FindClass(
          "com/qualcomm/qti/auth/secureextauthservice/GetTokenResponse");
  if(getTokenRspClass == NULL) {
    LOGD("getTokenRspClass NULL");
    return NULL;
  }
  jmethodID getTokenRspConstructor = env->GetMethodID(
    getTokenRspClass, "<init>", "()V");
  if(getTokenRspConstructor == NULL) {
    LOGD("getTokenRspConstructor NULL");
    return NULL;
  }
  jobject getTokenRspObject = env->NewObject(getTokenRspClass,
      getTokenRspConstructor);
  if(getTokenRspObject == NULL) {
    LOGD("getTokenRspObject NULL");
    return NULL;
  }

  jfieldID resultField = env->GetFieldID(getTokenRspClass, "result", "I");
  env->SetIntField(getTokenRspObject, resultField, rv);
  if(resultField == NULL) {
    LOGD("resultField NULL");
    return NULL;
  }

  jfieldID authenticatorNameField = env->GetFieldID(getTokenRspClass,
    "authenticatorName", "Ljava/lang/String;");
  if(authenticatorNameField == NULL) {
    LOGD("authenticatorNameField NULL");
    return NULL;
  }
  jstring authenticatorNameString = env->NewStringUTF(token.authName);
  if(authenticatorNameString == NULL) {
    LOGD("authenticatorNameString NULL");
    return NULL;
  }
  env->SetObjectField(getTokenRspObject, authenticatorNameField,
    authenticatorNameString);

  jfieldID encapsulatedResultField = env->GetFieldID(getTokenRspClass,
    "encapsulatedResult", "[B");
  if(encapsulatedResultField == NULL) {
    LOGD("encapsulatedResultField NULL");
    return NULL;
  }
  jbyteArray encapsulatedResultByteArray = env->NewByteArray(
    token.messageLen);
  if(encapsulatedResultByteArray == NULL) {
    LOGD("encapsulatedResultByteArray NULL");
    return NULL;
  }
  env->SetByteArrayRegion(encapsulatedResultByteArray, 0,
    token.messageLen,
    (const jbyte*) (token.message));
  env->SetObjectField(getTokenRspObject, encapsulatedResultField,
    encapsulatedResultByteArray);

  return getTokenRspObject;
}

JNIEXPORT jbyteArray Java_com_qualcomm_qti_auth_secureextauthservice_SecureExtAuthImpl_nativeCancel(
    JNIEnv* env, jclass cls, jbyteArray jnonce) {
  return jnonce;
}

#ifdef __cplusplus
}
#endif
