/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define QSEE_CONNECTOR_SERVICE_NAME com.qualcomm.qti.qseeproxy

#include <android/log.h>
#include <cstring>
#include <jni.h>
#include <QSEEConnectorClient.h>
#include <eSEPowerManager.h>
#include <stdlib.h>
#include <string>

#define DEBUG true
#define QPAY_OPEN_CONNECTION                    1
#define QPAY_TRANSCEIVE                         2
#define QPAY_CLOSE_CONNECTION                   3
#define QPAY_SWITCHOFF_ESE                      4
#define QPAY_MAX_APDU_SIZE                      2200

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#if DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,   LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...) do {} while (0)
#endif

using namespace android;

struct send_cmd {
    uint32_t session_handle;
    uint32_t channel_number;
    uint32_t cmd_id;
    uint32_t cmd_len;
    unsigned char tApdu[QPAY_MAX_APDU_SIZE];
};

struct send_cmd_rsp {
    uint32_t session_handle;
    uint32_t channel_number;
    uint32_t rsp_len;
    unsigned char rApdu[QPAY_MAX_APDU_SIZE];
};

static uint32_t handle = 0;
static uint32_t sessionHandle = 0;
static uint32_t channelNumber = 0;
static const char *path = "/system/etc/firmware";
static const char *name = "qpay";
static sp<IeSEPowerManager> proxy;
static QSEEConnectorClient *client = NULL;

static sp<IeSEPowerManager> getServ(std::string const &name) {

  do {
    sp<IServiceManager> sm = defaultServiceManager();
    if (sm == 0) {
      ALOGE("NULL sm");
      break;
    }
    sp<IBinder> binder = sm->getService(String16(name.c_str()));
    if (binder == 0) {
      ALOGE("NULL binder for service %s", name.c_str());
      break;
    }
    proxy = interface_cast<IeSEPowerManager>(binder);
    if (proxy == 0) {
      ALOGE("NULL proxy");
      break;
    }
  } while (0);
  ALOGD("Returning eSE proxy");
  return proxy;
}

static int qpay_init() {

    int reqLen = sizeof(struct send_cmd);
    int rspLen = sizeof(struct send_cmd_rsp);
    int buf_size = 0;
    status_t rv = OK;
    int ret = 0;

    /* let's load the TZ QSEE APP in TZ */
    buf_size = reqLen + rspLen;
    do {
        if (client != NULL) {
          LOGE("Application already loaded?");
          break;
        }

        client = new QSEEConnectorClient(path,name,buf_size);

        if (client == NULL) {
            LOGE("Error creating client");
            break;
        }
        client->load();
    } while(0);
    return JNI_VERSION_1_2;
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {

    return qpay_init();
}

static int eSE_switchedoff(JNIEnv * env, jclass jc) {

    LOGD("eSEswitchedoff");
    struct send_cmd cmd = {0};
    struct send_cmd_rsp msgrsp = {0};
    int32_t ret = 1;
    int32_t reqLen = 0;
    int32_t rspLen = 0;
    cmd.cmd_id = QPAY_SWITCHOFF_ESE;
    jbyte success[] = {0};
    reqLen = sizeof(struct send_cmd);
    rspLen = sizeof(struct send_cmd_rsp);

    if (client == NULL)
        return -1;

    ret = client->sendCommand((uint8_t*)&cmd, reqLen, (uint8_t*)&msgrsp, rspLen);
    if (ret) {
        LOGE("send command failed with ret = %d\n", ret);
        return -1;
    }
    ret = -1;
    /* Extract the answer from QSEE AP */
    jbyteArray switchOff = env->NewByteArray(msgrsp.rsp_len);

    if (switchOff != NULL) {

        env->SetByteArrayRegion (switchOff, 0,  msgrsp.rsp_len,  (const jbyte*) (msgrsp.rApdu));

        jbyte* switchOffPtr = env->GetByteArrayElements(switchOff, NULL);

        if(switchOffPtr != NULL) {
            if (memcmp(switchOffPtr, success, msgrsp.rsp_len) == 0) {
                ret = 0;
            } else {
                LOGE("eSE SwitchOFF failed");
            }
            env->ReleaseByteArrayElements(switchOff, switchOffPtr, 0);
        } else {
            LOGE("cmdPtr NULL");
        }
    }
    return ret;
}

JNIEXPORT jint Java_com_android_qti_qpay_QPayAdapter_nativeGetSessionHandle
  (JNIEnv * env, jclass jc) {

    return sessionHandle;
}

JNIEXPORT jint Java_com_android_qti_qpay_QPayAdapter_nativeGetChannelHandle
  (JNIEnv * env, jclass jc){

    return channelNumber;
}

JNIEXPORT jint Java_com_android_qti_qpay_QPayAdapter_nativeeSEPowerOn
  (JNIEnv * env, jclass jc) {

    int32_t ret = -1;
    sp<eSEPowerManagerCb> cb = new eSEPowerManagerCb();
    if (proxy == 0) {
        proxy = getServ("eSEPowerManagerService");
        if (proxy == 0) {
          ALOGE("NULL proxy");
          return ret;
        }
    }
    return (proxy->powerOn(cb));
}

JNIEXPORT jint Java_com_android_qti_qpay_QPayAdapter_nativeeSEPowerOff
  (JNIEnv * env, jclass jc) {

    int32_t ret = -1;
    if (proxy == 0) {
        proxy = getServ("eSEPowerManagerService");
        if (proxy == 0) {
          ALOGE("NULL proxy");
          return ret;
        }
    }
    if (eSE_switchedoff(env,jc) != 0) {
        ALOGE("eSE_switchedoff Failed");
    }
    return (proxy->powerOff());
}

JNIEXPORT jint Java_com_android_qti_qpay_QPayAdapter_nativeeSEGetState
  (JNIEnv * env, jclass jc) {

    int32_t ret = -1;
    if (proxy == 0) {
        proxy = getServ("eSEPowerManagerService");
        if (proxy == 0) {
          ALOGE("NULL proxy");
          return ret;
        }
    }
    return (proxy->getState());
}


JNIEXPORT jint Java_com_android_qti_qpay_QPayAdapter_nativeGetApduMaxSize
  (JNIEnv * env, jclass jc) {

    return QPAY_MAX_APDU_SIZE - 1;
}

JNIEXPORT void Java_com_android_qti_qpay_QPayAdapter_nativeUninit
  (JNIEnv * env, jclass jc) {

    if (client != NULL) {
        LOGD("UnLoading the TA APP ");
        client->unload();
        delete client;
        client = 0;
    }
}

JNIEXPORT jint Java_com_android_qti_qpay_QPayAdapter_nativeInit
  (JNIEnv * env, jclass jc) {

    return qpay_init();
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {

    Java_com_android_qti_qpay_QPayAdapter_nativeUninit(NULL, NULL);
}

JNIEXPORT jbyteArray JNICALL Java_com_android_qti_qpay_QPayAdapter_nativeOpen
  (JNIEnv * env, jclass jc) {

    LOGD("NativeOpen");
    struct send_cmd cmd = {0};
    struct send_cmd_rsp msgrsp = {0};
    int32_t ret = 1;
    int32_t reqLen = 0;
    int32_t rspLen = 0;
    cmd.cmd_id = QPAY_OPEN_CONNECTION;

    reqLen = sizeof(struct send_cmd);
    rspLen = sizeof(struct send_cmd_rsp);

    if (client == NULL)
        return NULL;

    ret = client->sendCommand((uint8_t*)&cmd, reqLen, (uint8_t*)&msgrsp, rspLen);
    if (ret) {
        LOGE("send command failed with ret = %d\n", ret);
        return NULL;
    }
    /* Extract the answer from QSEE AP */
    jbyteArray open = env->NewByteArray(msgrsp.rsp_len);
    if (open != NULL)
        env->SetByteArrayRegion (open, 0,  msgrsp.rsp_len,  (const jbyte*) (msgrsp.rApdu));

    return open;
}

JNIEXPORT jbyteArray JNICALL Java_com_android_qti_qpay_QPayAdapter_nativeClose
  (JNIEnv * env, jclass jc) {

    LOGD("NativeClose");
    struct send_cmd cmd = {0};
    struct send_cmd_rsp msgrsp = {0};
    int32_t ret = 1;
    int32_t reqLen = 0;
    int32_t rspLen = 0;
    cmd.cmd_id = QPAY_CLOSE_CONNECTION;

    reqLen = sizeof(struct send_cmd);
    rspLen = sizeof(struct send_cmd_rsp);

    if (client == NULL)
        return NULL;

    ret = client->sendCommand((uint8_t*)&cmd, reqLen, (uint8_t*)&msgrsp, rspLen);
    if (ret) {
        LOGE("send command failed with ret = %d\n", ret);
        return NULL;
    }
     /* Extract the answer from QSEE AP */
    jbyteArray close = env->NewByteArray(msgrsp.rsp_len);
    if (close != NULL)
        env->SetByteArrayRegion (close, 0,  msgrsp.rsp_len,  (const jbyte*) (msgrsp.rApdu));

    return close;
}

JNIEXPORT jbyteArray JNICALL Java_com_android_qti_qpay_QPayAdapter_nativeTransceive
  (JNIEnv * env, jclass jc, jint cmdId, jbyteArray cmd, jint sHandle, jint cNumber) {

    LOGD("NativeTransceive");

    struct send_cmd apdu_to_transmit;
    struct send_cmd_rsp msgrsp = {0};
    int32_t ret = 1;
    int32_t reqLen = 0;
    int32_t rspLen = 0;

    apdu_to_transmit.cmd_id = (uint32_t) cmdId;
    LOGD("NativeTransceive - cmd.id is %d", apdu_to_transmit.cmd_id);
    if (cmd == NULL){
        LOGE("cmd NULL");
        return NULL;
    }
    size_t cmdLen = env->GetArrayLength(cmd);
    if (QPAY_MAX_APDU_SIZE < cmdLen) {
        LOGE("cmdLen too long %d. max is %d", cmdLen, QPAY_MAX_APDU_SIZE);
        return NULL;
    }
    apdu_to_transmit.cmd_len = cmdLen;
    jbyte* cmdPtr = env->GetByteArrayElements(cmd, NULL);
    if(cmdPtr == NULL) {
        LOGE("cmdPtr NULL");
        return NULL;
    }
    memcpy(apdu_to_transmit.tApdu, cmdPtr, cmdLen);
    env->ReleaseByteArrayElements(cmd, cmdPtr, 0);
    if (sHandle > 0){
        apdu_to_transmit.session_handle = sHandle;
    }
    apdu_to_transmit.channel_number = cNumber;

    reqLen = sizeof(struct send_cmd);
    rspLen = sizeof(struct send_cmd_rsp);

    if (client == NULL)
        return NULL;

    ret = client->sendCommand((uint8_t*)&apdu_to_transmit, reqLen, (uint8_t*)&msgrsp, rspLen);

    if (ret)
        return NULL;

    jbyteArray respApdu = env->NewByteArray(msgrsp.rsp_len);
    if (respApdu != NULL) {
        env->SetByteArrayRegion (respApdu, 0,  msgrsp.rsp_len,  (const jbyte*) (msgrsp.rApdu));
        sessionHandle = msgrsp.session_handle;
        channelNumber = msgrsp.channel_number;
    }
    return respApdu;
}

#ifdef __cplusplus
}
#endif
