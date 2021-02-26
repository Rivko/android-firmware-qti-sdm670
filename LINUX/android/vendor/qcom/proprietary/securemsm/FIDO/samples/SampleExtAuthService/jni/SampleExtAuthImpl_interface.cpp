/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <jni.h>
#include <android/log.h>
#include <auth.h>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG true
#define LOG_TAG "SampleExtAuthJNI"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,   LOG_TAG, __VA_ARGS__)

#if DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,   LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...) do {} while (0)
#endif

static int rv = 0;
static int mVersionMajor = 1;
static int mVersionMinor = 0;
static int mResult = 0;
static long mUserId = 0;
static long mUptime = 0;
static long mUserEntityId = 0;
static uint8_t mExtData[] = { 0x52, 0x41, 0x4E, 0x44, 0x4F, 0x4D, 0x44, 0x41, 0x54, 0x41 };
static const char authName[] = "nsecureextauth";

JNIEXPORT jobject Java_com_qualcomm_qti_auth_sampleextauthservice_SampleExtAuthImpl_nativeGetToken(
    JNIEnv* env, jclass cls, jbyteArray jnonce, jstring jsecAppName, jbyteArray jauthId) {

  LOGD("nativeGetToken()+");

  jclass getTokenRspClass =
    env->FindClass(
          "com/qualcomm/qti/auth/sampleextauthservice/GetTokenResponse");
  if(getTokenRspClass == NULL) {
    LOGE("getTokenRspClass NULL");
    return NULL;
  }
  jmethodID getTokenRspConstructor = env->GetMethodID(
    getTokenRspClass, "<init>", "()V");
  if(getTokenRspConstructor == NULL) {
    LOGE("getTokenRspConstructor NULL");
    return NULL;
  }
  jobject getTokenRspObject = env->NewObject(getTokenRspClass,
      getTokenRspConstructor);
  if(getTokenRspObject == NULL) {
    LOGE("getTokenRspObject NULL");
    return NULL;
  }

  qc_authentication_token_t authenticationToken = {0};
  authenticationToken.version.major = mVersionMajor;
  authenticationToken.version.minor = mVersionMinor;
  authenticationToken.result = (qc_auth_code_t)mResult;
  authenticationToken.userId = mUserId;
  authenticationToken.uptime = mUptime;
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

  jfieldID resultField = env->GetFieldID(getTokenRspClass, "result", "I");
  if(resultField == NULL) {
    LOGE("resultField NULL");
    return NULL;
  }
  env->SetIntField(getTokenRspObject, resultField, rv);

  jfieldID authenticatorNameField = env->GetFieldID(getTokenRspClass,
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
  env->SetObjectField(getTokenRspObject, authenticatorNameField,
    authenticatorNameString);

  LOGD("authName %x %x %x %x", token.authName[0], token.authName[1],
    token.authName[2], token.authName[3]);

  jfieldID encapsulatedResultField = env->GetFieldID(getTokenRspClass,
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
  env->SetObjectField(getTokenRspObject, encapsulatedResultField,
    encapsulatedResultByteArray);

  LOGD("nativeGetToken()-");
  return getTokenRspObject;
}

JNIEXPORT jbyteArray Java_com_qualcomm_qti_auth_sampleextauthservice_SampleExtAuthImpl_nativeCancel(
    JNIEnv* env, jclass cls, jbyteArray jnonce) {
  return jnonce;
}

#ifdef __cplusplus
}
#endif
