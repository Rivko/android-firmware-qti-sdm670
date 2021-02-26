/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <utils/Log.h>
#include <cstring>
#include <string>
#include <mutex>
#include <cstdlib>
#include <cstdint>
#include <jni.h>
#include <ese.h>

ESE_INCLUDE_HW(GPQESE_BACKEND);

#define LOG_TAG "eSEWrapperJNI : "
#define DEBUG true

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#if DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,   LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...) do {} while (0)
#endif

#define GPQESE_MAX_CAPDU_SIZE 65546
#define GPQESE_MAX_RAPDU_SIZE 65539

struct EseInterface my_ese;
void *ese_hw_open_data = NULL;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    return JNI_VERSION_1_6;
}

JNIEXPORT jint Java_com_qualcomm_qti_libesewrapper_eSEWrapper_nativeOpen
  (JNIEnv * /*env*/, jclass /*jc*/) {

  my_ese = ESE_INITIALIZER(GPQESE_BACKEND);
  if (!my_ese.ops) {
    LOGE("ESE_INITIALIZER FAILED");
    return 1;
  }
  LOGD("ESE_INITIALIZER - ESENAME : '%s'", ese_name(&my_ese));
  if (ese_open(&my_ese, ese_hw_open_data) < 0) {
    LOGE("Cannot open hw");
    if (ese_error(&my_ese))
      LOGE("eSE error (%d): %s", ese_error_code(&my_ese), ese_error_message(&my_ese));
    return 1;
  }
  return 0;
}

JNIEXPORT jbyteArray JNICALL Java_com_qualcomm_qti_libesewrapper_eSEWrapper_nativeTransceive (JNIEnv * env, jclass /*jc*/, jbyteArray txApdu) {

    int recvd;
    uint32_t cmdLen = env->GetArrayLength(txApdu);
    uint8_t tApdu[GPQESE_MAX_CAPDU_SIZE] = {};
    uint8_t rApdu[GPQESE_MAX_RAPDU_SIZE] = {};

    if (GPQESE_MAX_CAPDU_SIZE < cmdLen) {
        LOGE("cmdLen too long %zu. max is %d", cmdLen, GPQESE_MAX_CAPDU_SIZE);
        return NULL;
    }

    jbyte* cmdPtr = env->GetByteArrayElements(txApdu, NULL);

    if(cmdPtr == NULL) {
        LOGE("cmdPtr NULL");
        return NULL;
    }
    memcpy(tApdu, cmdPtr, cmdLen);
    env->ReleaseByteArrayElements(txApdu, cmdPtr, 0);

    const struct EseSgBuffer tx = {
        .c_base = tApdu, .len = cmdLen,
    };
    struct EseSgBuffer rx = {
        .base = rApdu, .len = GPQESE_MAX_RAPDU_SIZE,
    };

    recvd = ese_transceive_sg(&my_ese, &tx, 0, &rx, 0);
    if (ese_error(&my_ese)) {
      LOGE("An error (%d) occurred: %s", ese_error_code(&my_ese),
             ese_error_message(&my_ese));
      return NULL;
    }

    if (recvd < 0) {
      LOGE("An error (%d) occurred", recvd);
      return NULL;
    }

    jbyteArray respApdu = env->NewByteArray(recvd);
    if (respApdu != NULL) {
        env->SetByteArrayRegion (respApdu, 0,  recvd,  (const jbyte*) (rApdu));
    }
    return respApdu;
}

JNIEXPORT jint Java_com_qualcomm_qti_libesewrapper_eSEWrapper_nativeClose
  (JNIEnv * /*env*/, jclass /*jc*/) {

    ese_close(&my_ese);
    if (ese_error(&my_ese)) {
        LOGE("An error (%d) occurred: %s", ese_error_code(&my_ese),
             ese_error_message(&my_ese));
        return 1;
    }
    return 0;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {

}


#ifdef __cplusplus
}
#endif
