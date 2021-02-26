/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>
#include <auth.h>
#include <cstring>
#include <android/log.h>
#include <sys/sysinfo.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG true
#define LOG_TAG "SampleAuthenticatorJNI"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,   LOG_TAG, __VA_ARGS__)

#if DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,   LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...) do {} while (0)
#endif

#define QSEE_TIME_OFFSET  18 /**< QSEE appears to be 18 sec behind */

static int rv = 0;
static int mVersionMajor = 1;
static int mVersionMinor = 0;
static int mResult = 0;
static long mUserId = 12345;
static long mUserEntityId = 987654321;
static uint8_t mExtData[] = { 0x03, 0x01, 0x04, 0x00, 0x01, 0x02, 0x03, 0x04 };
static const char authName[] = "nssampleauth";

static uint64_t getUptime() {
  struct sysinfo info = {0};
  int rv = 0;
  rv = sysinfo(&info);
  if (rv) {
    LOGE("Error in sysinfo: %s (%d)", strerror(errno), errno);
    return 0;
  }
  return 1000*((uint64_t)info.uptime + QSEE_TIME_OFFSET);
}

JNIEXPORT jobject Java_com_qualcomm_qti_auth_sampleauthenticatorservice_SampleAuthenticatorImpl_nativeVerifyUser(
    JNIEnv* env, jclass cls, jbyteArray jnonce, jstring jsecAppName, jbyteArray jauthId) {

  LOGD("verifyUser()+");

  jclass verifyUserRspClass =
    env->FindClass(
          "com/qualcomm/qti/auth/sampleauthenticatorservice/VerifyUserResponse");
  if(verifyUserRspClass == NULL) {
    LOGE("verifyUserRspClass NULL");
    return NULL;
  }
  jmethodID verifyUserRspConstructor = env->GetMethodID(
    verifyUserRspClass, "<init>", "()V");
  if(verifyUserRspConstructor == NULL) {
    LOGE("verifyUserRspConstructor NULL");
    return NULL;
  }
  jobject verifyUserRspObject = env->NewObject(verifyUserRspClass,
      verifyUserRspConstructor);
  if(verifyUserRspObject == NULL) {
    LOGE("verifyUserRspObject NULL");
    return NULL;
  }

  qc_authentication_token_t authenticationToken = {0};
  authenticationToken.version.major = mVersionMajor;
  authenticationToken.version.minor = mVersionMinor;
  authenticationToken.result = (qc_auth_code_t)mResult;
  authenticationToken.userId = mUserId;
  authenticationToken.uptime = getUptime();
  if (sizeof(mExtData)!=0) {
    size_t maxCopySize = !(QC_EXTENSION_DATA_MAX_LEN<sizeof(mExtData))
      ?sizeof(mExtData):QC_EXTENSION_DATA_MAX_LEN;
    memcpy(authenticationToken.extData, mExtData, maxCopySize);
    authenticationToken.extDataLength = maxCopySize;
  } else {
    authenticationToken.extDataLength = 0;
  }
  if(jnonce == NULL){
    LOGE("jnonce passed in is NULL");
    return NULL;
  }
  size_t nonceLen = env->GetArrayLength(jnonce);
  LOGD("nonce length %d. max is %d", nonceLen, QC_AUTH_NONCE);
  if (QC_AUTH_NONCE < nonceLen) {
    LOGE("nonce too long %d. max is %d", nonceLen, QC_AUTH_NONCE);
    return NULL;
  }
  jbyte* noncePtr = env->GetByteArrayElements(jnonce, NULL);
  if(noncePtr == NULL) {
    LOGE("noncePtr NULL");
    return NULL;
  }
  memcpy(authenticationToken.nonce, noncePtr, QC_AUTH_NONCE);
  env->ReleaseByteArrayElements(jnonce, noncePtr, 0);

  qc_user_verification_token_t token = {0};
  memcpy(token.authName, authName, sizeof(authName));
  token.messageLen = sizeof(token.authName)+sizeof(token.authId)+sizeof(qc_authentication_token_t)-sizeof(authenticationToken.extData)+authenticationToken.extDataLength;
  memcpy(&token.token, &authenticationToken,sizeof(qc_authentication_token_t)-sizeof(authenticationToken.extData)+authenticationToken.extDataLength);

  if(jauthId == NULL){
    LOGD("jauthId passed in is null");
    return NULL;
  }
  jbyte* authIdPtr = env->GetByteArrayElements(jauthId, NULL);
  if(authIdPtr == NULL) {
    LOGE("authIdPtr NULL");
    return NULL;
  }
  jint jauthIdLen = env->GetArrayLength(jauthId);
  if (jauthIdLen != QC_AUTH_ID_SIZE) {
    LOGD("authId is wrong length");
    env->ReleaseByteArrayElements(jauthId, authIdPtr, 0);
    return NULL;
  }
  memcpy(token.authId, authIdPtr, QC_AUTH_ID_SIZE);
  env->ReleaseByteArrayElements(jauthId, authIdPtr, 0);

  jfieldID resultField = env->GetFieldID(verifyUserRspClass, "result", "I");
  if(resultField == NULL) {
    LOGE("resultField NULL");
    return NULL;
  }
  env->SetIntField(verifyUserRspObject, resultField, rv);

  jfieldID authenticatorNameField = env->GetFieldID(verifyUserRspClass,
    "authenticatorName", "Ljava/lang/String;");
  if(authenticatorNameField == NULL) {
    LOGE("authenticatorNameField NULL");
    return NULL;
  }
  jstring authenticatorNameString = env->NewStringUTF(token.authName);
  if(authenticatorNameString == NULL) {
    LOGE("authenticatorNameString NULL");
    return NULL;
  }
  env->SetObjectField(verifyUserRspObject, authenticatorNameField,
    authenticatorNameString);

  jfieldID userIdField = env->GetFieldID(verifyUserRspClass,
    "userId", "J");
  if(userIdField == NULL) {
    LOGE("userIdField NULL");
    return NULL;
  }
  env->SetLongField(verifyUserRspObject, userIdField,
    mUserId);

  jfieldID userEntityIdField = env->GetFieldID(verifyUserRspClass,
    "userEntityId", "J");
  if(userEntityIdField == NULL) {
    LOGE("userEntityIdField NULL");
    return NULL;
  }
  env->SetLongField(verifyUserRspObject, userEntityIdField,
    mUserEntityId);

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
    sizeof(authName),
    (const jbyte*) (token.authName));
  env->SetByteArrayRegion(encapsulatedResultByteArray, sizeof(token.authName),
    sizeof(token.authId),
    (const jbyte*) &(token.authId));
  env->SetByteArrayRegion(encapsulatedResultByteArray, sizeof(token.authName)+sizeof(token.authId),
    (sizeof(qc_authentication_token_t)-sizeof(authenticationToken.extData)+authenticationToken.extDataLength),
    (const jbyte*) &(token.token));
  env->SetObjectField(verifyUserRspObject, encapsulatedResultField,
    encapsulatedResultByteArray);

  LOGD("verifyUser()-");
  return verifyUserRspObject;
}

JNIEXPORT jbyteArray Java_com_qualcomm_qti_auth_sampleauthenticatorservice_SampleAuthenticatorImpl_nativeCancel(
    JNIEnv* env, jclass cls, jbyteArray jnonce) {
  return jnonce;
}

JNIEXPORT jboolean Java_com_qualcomm_qti_auth_sampleauthenticatorservice_SampleAuthenticatorImpl_nativeGetEnrollmentStatus(
    JNIEnv* env, jclass cls, jlong juserId) {
  jboolean status = false;
  if(juserId == (jlong)mUserId){
    status = true;
  }
  return status;
}

#ifdef __cplusplus
}
#endif
