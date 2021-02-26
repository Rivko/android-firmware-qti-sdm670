/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
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

#define SECURE_AUTH_CMD_VERIFY_USER           1
#define SECURE_AUTH_CMD_GET_ENROLLMENT_STATUS 2
#define SECURE_AUTH_CMD_VERIFY_USER_BIOLIB    3

#undef LOG_TAG
#define LOG_TAG "SecureSampleAuthJNI"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define DEBUG true
#if DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,   LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...) do {} while (0)
#endif


using namespace android;

#pragma pack(push, params, 1)

struct send_cmd  {
  uint32_t cmd_id;
  union {
    /** VerifyUserCmd **/
    struct {
      char secAppName[QC_SEC_APP_NAME_LEN];
      uint8_t nonce[QC_AUTH_NONCE];
    };
    /** VerifyUserBiolibCmd **/
    // TODO: nothing now. Maybe group id?
    /** GetEnrollmentStatusCmd **/
    uint64_t userId;
  };
};
struct send_cmd_rsp {
  union {
    /** VerifyUserRsp **/
    struct {
      uint16_t result;
      uint64_t userId;
      qc_user_verification_token_t token;
    } verifyUserRsp;
    /** VerifyUserBiolibRsp **/
    struct {
      uint16_t result;
      uint64_t userId;
    } verifyUserBiolibRsp;
    /** EnrollmentStatusRsp **/
    uint32_t status;
  };
};

#pragma pack(pop, params)

static QTEE::QTEEConnectorClient *client = NULL;

#ifdef __cplusplus
extern "C" {
#endif

static long mUserEntityId = 987654321;
static const char *path = "/firmware/image";
static const char *name = "sampleauth";

int verifyUser(struct send_cmd &cmd, uint16_t &result, qc_user_verification_token_t &token, uint64_t &userId)
{
  struct send_cmd_rsp msgrsp = {0};
  int32_t ret = 1;
  int32_t req_len = 0;
  int32_t rsp_len = 0;

  do {

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
    token = msgrsp.verifyUserRsp.token;
    userId = msgrsp.verifyUserRsp.userId;
    result = msgrsp.verifyUserRsp.result;
  } while(0);
  return ret;
}

bool getEnrollmentStatus(struct send_cmd &cmd)
{
  struct send_cmd_rsp msgrsp = {0};
  bool status = false;

  int32_t ret = 1;
  int32_t req_len = 0;
  int32_t rsp_len = 0;

  do {

    cmd.cmd_id = SECURE_AUTH_CMD_GET_ENROLLMENT_STATUS;

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
    }
    status = msgrsp.status;
  } while(0);
  return status;
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

JNIEXPORT jobject Java_com_qualcomm_qti_auth_securesampleauthservice_SecureSampleAuthImpl_nativeVerifyUser(
    JNIEnv* env, jclass cls, jbyteArray jnonce, jstring jsecAppName, jboolean useBiolib) {

  int rv = 0;
  qc_user_verification_token_t token = {0};
  uint64_t userId = 0;
  uint64_t userEntityId = mUserEntityId;
  uint16_t result = 0;
  uint8_t nonce[QC_AUTH_NONCE] = {0};
  size_t nonceLen = 0;
  size_t secAppNameLength = 0;
  char const * secAppName = NULL;
  struct send_cmd cmd = {0};

  if (!useBiolib) {
    if(NULL == jnonce){
      LOGE("jnonce is NULL");
      return NULL;
    }
    nonceLen = env->GetArrayLength(jnonce);
    LOGD("nonce length %d. max is %d", nonceLen, QC_AUTH_NONCE);
    if (QC_AUTH_NONCE < nonceLen) {
      LOGE("nonce too long %d. max is %d", nonceLen, QC_AUTH_NONCE);
      return NULL;
    }

    if(NULL == jsecAppName){
      LOGE("jsecAppName is NULL");
      return NULL;
    }
    secAppNameLength = (size_t)env->GetStringLength(jsecAppName);
    LOGD("jsecAppName length %d. max is %d", secAppNameLength, QC_SEC_APP_NAME_LEN);
    if (QC_SEC_APP_NAME_LEN < secAppNameLength) {
      LOGE("jsecAppName too long %d. max is %d", secAppNameLength, QC_SEC_APP_NAME_LEN);
      return NULL;
    }
    secAppName = env->GetStringUTFChars(jsecAppName, 0);
    if(secAppName == NULL) {
      LOGE("secAppName NULL");
      return NULL;
    }

    memcpy(cmd.secAppName, secAppName, secAppNameLength);

    jbyte* noncePtr = env->GetByteArrayElements(jnonce, NULL);
    if(noncePtr == NULL) {
      LOGE("noncePtr NULL");
      return NULL;
    }
    memcpy(cmd.nonce, noncePtr, QC_AUTH_NONCE);
    env->ReleaseByteArrayElements(jnonce, noncePtr, 0);
    cmd.cmd_id = SECURE_AUTH_CMD_VERIFY_USER;
  } else {
    cmd.cmd_id = SECURE_AUTH_CMD_VERIFY_USER_BIOLIB;
  }

  rv = verifyUser(cmd, result, token, userId);
  if(rv != 0) {
    LOGE("Error from secure app");
    return NULL;
  }

  jclass verifyUserRspClass =
    env->FindClass(
          "com/qualcomm/qti/auth/securesampleauthservice/VerifyUserResponse");
  if(verifyUserRspClass == NULL) {
    LOGD("verifyUserRspClass NULL");
    return NULL;
  }
  jmethodID verifyUserRspConstructor = env->GetMethodID(
    verifyUserRspClass, "<init>", "()V");
  if(verifyUserRspConstructor == NULL) {
    LOGD("verifyUserRspConstructor NULL");
    return NULL;
  }
  jobject verifyUserRspObject = env->NewObject(verifyUserRspClass,
      verifyUserRspConstructor);
  if(verifyUserRspObject == NULL) {
    LOGD("verifyUserRspObject NULL");
    return NULL;
  }

  jfieldID resultField = env->GetFieldID(verifyUserRspClass, "result", "I");
  env->SetIntField(verifyUserRspObject, resultField, rv);
  if(resultField == NULL) {
    LOGD("resultField NULL");
    return NULL;
  }

  jfieldID userIdField = env->GetFieldID(verifyUserRspClass,
    "userId", "J");
  if(userIdField == NULL) {
    LOGD("userIdField NULL");
    return NULL;
  }
  env->SetLongField(verifyUserRspObject, userIdField,
    userId);

  jfieldID userEntityIdField = env->GetFieldID(verifyUserRspClass,
    "userEntityId", "J");
  if(userEntityIdField == NULL) {
    LOGD("userEntityIdField NULL");
    return NULL;
  }
  env->SetLongField(verifyUserRspObject, userEntityIdField,
    userEntityId);

  jfieldID authenticatorNameField = env->GetFieldID(verifyUserRspClass,
    "authenticatorName", "Ljava/lang/String;");
  if(authenticatorNameField == NULL) {
    LOGD("authenticatorNameField NULL");
    return NULL;
  }
  jstring authenticatorNameString = env->NewStringUTF(name);
  if(authenticatorNameString == NULL) {
    LOGD("authenticatorNameString NULL");
    return NULL;
  }
  env->SetObjectField(verifyUserRspObject, authenticatorNameField,
    authenticatorNameString);

  if (!useBiolib) {
    jfieldID encapsulatedResultField = env->GetFieldID(verifyUserRspClass,
      "encapsulatedResult", "[B");
    if(encapsulatedResultField == NULL) {
      LOGE("encapsulatedResultField NULL");
      return NULL;
    }
    jbyteArray encapsulatedResultByteArray = env->NewByteArray(
      token.messageLen);
    if(encapsulatedResultByteArray == NULL) {
      LOGE("encapsulatedResultByteArray NULL");
      return NULL;
    }
    env->SetByteArrayRegion(encapsulatedResultByteArray, 0,
      token.messageLen,
      (const jbyte*) (token.message));
    env->SetObjectField(verifyUserRspObject, encapsulatedResultField,
      encapsulatedResultByteArray);
  }

  return verifyUserRspObject;
}

JNIEXPORT jbyteArray Java_com_qualcomm_qti_auth_securesampleauthservice_SecureSampleAuthImpl_nativeCancel(
    JNIEnv* env, jclass cls, jbyteArray jnonce) {
  return jnonce;
}

JNIEXPORT jboolean Java_com_qualcomm_qti_auth_securesampleauthservice_SecureSampleAuthImpl_nativeGetEnrollmentStatus(
    JNIEnv* env, jclass cls, jlong juserId) {
  jboolean status = false;

  struct send_cmd cmd = {0};
  uint32_t cmd_len = sizeof(struct send_cmd) ;
  cmd.userId = (uint64_t)juserId;
  status = (jboolean)getEnrollmentStatus(cmd);

  return status;
}

#ifdef __cplusplus
}
#endif
