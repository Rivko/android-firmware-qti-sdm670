/* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "ims_media_jni"
#define LOG_NDEBUG 0
#define LOG_NIDEBUG 0
#define LOG_NDDEBUG 0

#include "ims_media_jni.h"
#include "ims_media_events.h"

#include <cutils/log.h>
#include <cutils/properties.h>
#include <dlfcn.h>
#include <stdlib.h>

#include <gui/Surface.h>
#include <utils/StrongPointer.h>
#include "SurfaceUtils.h"

#define DEFAULT_IMPL_LIB_PATH "lib-imsvt.so"
#define IMPL_LIB_PROPERTY_NAME "vt.impl.lib"
#define ARRAY_SIZE(myArray) (sizeof(myArray)/sizeof(myArray[0]))

extern JavaVM *g_jvmPtr;
static ImsMediaApis *imsMediaApis = NULL;
static jobject imsMediaObject = NULL;

class AttachToJavaThread {
bool threadAttached = false;
public:
    AttachToJavaThread(JNIEnv **e) {
        if (g_jvmPtr->GetEnv((void **) e, JNI_VERSION_1_6) != JNI_OK) {
            ALOGD("Attaching IMS thread");
            if (g_jvmPtr->AttachCurrentThread(e, NULL) != JNI_OK) {
                ALOGE("Attach IMS Thread error");
                return;
            }
            threadAttached = true;
        }
    }

    ~AttachToJavaThread() {
        if (threadAttached) {
            g_jvmPtr->DetachCurrentThread();
            threadAttached = false;
            ALOGD("Detached IMS thread");
        }
    }
};

static jint dpl_init(JNIEnv*, jobject) {
    ALOGD("%s", __func__);
    jint error = -1;
    if (imsMediaApis && imsMediaApis->initImsThinClient) {
        error = imsMediaApis->initImsThinClient();
    }
    return error;
}

static void dpl_deinit(JNIEnv*, jobject) {
    ALOGD("%s", __func__);
    if (imsMediaApis && imsMediaApis->deInitImsThinClient) {
        imsMediaApis->deInitImsThinClient();
    }
}

static jint dpl_setVideoImageBuffer(JNIEnv *e, jobject, jintArray pixels,
        jint width, jint height) {
    ALOGD("%s width = %d height = %d", __func__, width, height);
    jint ret = -1;

    if (!imsMediaApis || !imsMediaApis->setVideoImageBuffer) {
        return ret;
    }

    if (pixels != NULL) {
        jsize size = e->GetArrayLength(pixels);
        jint* buffer = e->GetIntArrayElements(pixels, JNI_FALSE);
        ret = imsMediaApis->setVideoImageBuffer(buffer, size, width, height);
        e->ReleaseIntArrayElements(pixels, buffer, JNI_ABORT);
    } else {
        ret = imsMediaApis->setVideoImageBuffer(NULL, 0, width, height);
    }
    return ret;
}

static jint dpl_handleRawFrame(JNIEnv* e, jobject, jbyteArray frame) {
    jint ret = 0;
    jsize size;
    jbyte *bytes;
    uint8_t *nativeframe;

    if (!imsMediaApis || !imsMediaApis->frameToEncode) return ret;
    if (frame != NULL) {
        jsize size = e->GetArrayLength(frame);
        bytes = e->GetByteArrayElements(frame, JNI_FALSE);
        imsMediaApis->frameToEncode((unsigned short *)bytes,(int)size);
        e->ReleaseByteArrayElements(frame, bytes, JNI_ABORT);
    } else {
        ALOGD("%s: Received a null frame", __func__);
        ret = -1;
    }
    return ret;
}

static int dpl_setSurface(JNIEnv *e, jobject, jobject osurface) {
    ALOGD("%s", __func__);
    if (imsMediaApis && imsMediaApis->setFarEndSurface) {
        imsMediaApis->setFarEndSurface(e, osurface);
    }
    return 0;
}

static void dpl_setDeviceOrientation(JNIEnv*, jobject, jint orientation) {
    ALOGD("%s", __func__);
    if (imsMediaApis && imsMediaApis->setDeviceOrientation) {
        imsMediaApis->setDeviceOrientation(orientation);
    }
}

static void dpl_setCameraFacing(JNIEnv*, jobject, jint facing) {
    ALOGD("%s", __func__);
    if (imsMediaApis && imsMediaApis->setCameraFacing) {
        imsMediaApis->setCameraFacing(facing);
    }
}

static void dpl_setCameraInfo(JNIEnv*, jobject, jint facing, jint mount) {
    ALOGD("%s", __func__);
    if (imsMediaApis && imsMediaApis->setCameraInfo) {
        imsMediaApis->setCameraInfo(facing, mount);
    }
}

static short dpl_getNegotiatedFPS(JNIEnv*, jobject) {
    short def = 20;
    ALOGD("%s", __func__);

    if (imsMediaApis && imsMediaApis->getNegotiatedFPS) {
        return imsMediaApis->getNegotiatedFPS();
    }
    return def;
}

static int dpl_getNegotiatedHeight(JNIEnv*, jobject) {
    int def = 240;
    ALOGD("%s", __func__);

    if (imsMediaApis && imsMediaApis->getNegotiatedHeight) {
        return imsMediaApis->getNegotiatedHeight();
    }
    return def;
}

static int dpl_getNegotiatedWidth(JNIEnv *, jobject) {
    int def = 320;
    ALOGD("%s", __func__);

    if(imsMediaApis && imsMediaApis->getNegotiatedWidth) {
        return imsMediaApis->getNegotiatedWidth();
    }
    return def;
}

static int dpl_getUIOrientationMode(JNIEnv*, jobject) {
    int def = -1;
    ALOGD("%s", __func__);

    if(imsMediaApis && imsMediaApis->getUIOrientationMode) {
        return imsMediaApis->getUIOrientationMode();
    }
    return def;
}

static int dpl_getPeerHeight(JNIEnv*, jobject) {
    int def = -1;
    ALOGD("%s", __func__);

    if (imsMediaApis && imsMediaApis->getPeerHeight) {
        return imsMediaApis->getPeerHeight();
    }
    return def;
}

static int dpl_getPeerWidth(JNIEnv*, jobject) {
    int def = -1;
    ALOGD("%s", __func__);

    if(imsMediaApis && imsMediaApis->getPeerWidth) {
        return imsMediaApis->getPeerWidth();
    }
    return def;
}

static int dpl_getVideoQualityIndication(JNIEnv*, jobject) {
    int def = -1;
    ALOGD("%s", __func__);

    if(imsMediaApis && imsMediaApis->getVideoQualityIndication) {
        return imsMediaApis->getVideoQualityIndication();
    }
    return def;
}

static jobject dpl_getRecordingSurface(JNIEnv* env, jobject)
{
    ALOGD("%s", __func__);

    if ( !imsMediaApis ) {
        ALOGE("%s ImsMedia interface is not ready.", __func__);
        return NULL;
    }

    if ( !imsMediaApis->getRecordingSurface ) {
        ALOGE("%s interface is not ready. ", __func__);
        return NULL;
    }

    android::sp<android::IGraphicBufferProducer>* const bufferProducer =
         imsMediaApis->getRecordingSurface();

    if ( !bufferProducer ) {
        ALOGE("%s failed to retrieve recording surface.", __func__);
        return NULL;
    }

    return android_view_Surface_createFromIGraphicBufferProducer(env, *bufferProducer);
}


static jint dpl_getRecorderFrameRate(JNIEnv* , jobject)
{
    ALOGD("%s", __func__);

    if ( !imsMediaApis ||  !imsMediaApis->getRecorderFrameRate ) {
        ALOGE("%s ImsMedia interface is not ready.", __func__);
        return -1;
    }

    return imsMediaApis->getRecorderFrameRate();
}

static void onMediaEvent(JNIEnv *e, int32_t eventId) {
    if(e == NULL) {
        ALOGE("%s Error: JNI env is NULL", __func__);
        return;
    }

    if(!gClassImsMedia) {
        ALOGE("%s Error: gClassImsMedia is null", __func__);
        return;
    }

    jmethodID mid = e->GetMethodID(gClassImsMedia, "onMediaEvent", "(I)V");
    if (mid == 0) {
        ALOGE("videocall callback: GetMethodID error");
        return;
    }

    e->CallVoidMethod(imsMediaObject, mid, (jint) eventId);
}

static unsigned long getDataUsage(int32_t mediaID, int rat) {
    const unsigned short UPLINK = 0;
    const unsigned short DOWNLINK = 1;
    unsigned long ulinkData = 0, dlinkData = 0;

    ulinkData = imsMediaApis->getRtpDataUsage3(UPLINK, mediaID, rat);
    dlinkData = imsMediaApis->getRtpDataUsage3(DOWNLINK, mediaID, rat);
    ALOGD("%s uplink=%ld downlink=%ld rat=%d", __func__, ulinkData, dlinkData, rat);
    return (ulinkData + dlinkData);
}

static void onDataUsageEvent(JNIEnv *e, int32_t event, int32_t mediaID) {
    ALOGD("%s event=%d mediaID=%d", __func__, event, mediaID);
    const unsigned short DATA_USAGE_LTE = 0;
    const unsigned short DATA_USAGE_WIFI = 1;

    if(e == NULL) {
        ALOGE("%s Error: JNI env is NULL", __func__);
        return;
    }

    const unsigned short ratArray[] = {DATA_USAGE_LTE, DATA_USAGE_WIFI};
    unsigned short ratCount = ARRAY_SIZE(ratArray);
    jlongArray dataUsage = e->NewLongArray(ratCount);
    if(dataUsage == NULL) {
        ALOGE("%s Error: memmory allocation failed", __func__);
        return;
    }

    jlong *elem = e->GetLongArrayElements(dataUsage, NULL);
    for (int rat = 0; rat < ratCount ; ++rat) {
        elem[rat] = getDataUsage(mediaID, rat);
    }
    e->ReleaseLongArrayElements(dataUsage, elem, 0);

    if(!gClassImsMedia) {
        ALOGE("%s Error: gClassImsMedia is null", __func__);
        return;
    }

    jmethodID mid = e->GetMethodID(gClassImsMedia, "onDataUsageEvent", "(I[J)V");
    if (mid == 0) {
        ALOGE("onDataUsageEvent: GetMethodID error");
        return;
    }
    //Send uplink and downlink data usage to upper layers
    e->CallVoidMethod(imsMediaObject, mid, (jint) mediaID, dataUsage);
}

static void onMediaEvent2(int32_t event, int32_t mediaID) {
    ALOGD("%s event=%d mediaID=%d", __func__, event, mediaID);
    JNIEnv *env = NULL;
    AttachToJavaThread thread(&env);

    switch(event) {
        case DATA_USAGE_EVT:
            onDataUsageEvent(env, event, mediaID);
            break;
        default:
            onMediaEvent(env, event);
    }
}

static int dpl_requestRtpDataUsage(JNIEnv*, jobject, jint mediaID) {
    int def = -1;
    ALOGD("%s mediaID=%d", __func__, mediaID);

    if (imsMediaApis && imsMediaApis->requestRtpDataUsage2) {
        return imsMediaApis->requestRtpDataUsage2(mediaID);
    }
    return def;
}

static void dpl_registerForImsEvent(JNIEnv* e, jobject, jobject objImsMedia) {
    ALOGD("%s", __func__);
    if (imsMediaApis) {
        if (imsMediaApis->registerAppEventCallback2) {
            imsMediaApis->registerAppEventCallback2(&onMediaEvent2);
        } else {
            ALOGE("%s: registerAppEventCallback2 is null", __func__);
        }
    } else {
        ALOGE("%s: imsMediaApis is null", __func__);
    }

    //Cache the ImsMedia instance
    imsMediaObject = (jclass) e->NewGlobalRef(objImsMedia);
}

static jboolean dpl_haveSameParent(JNIEnv *e, jobject o, jobject jSurfaceOne, jobject jSurfaceTwo) {
    ALOGD("%s", __func__);
    return android_view_Surface_haveSameParent(e, jSurfaceOne, jSurfaceTwo) ? JNI_TRUE : JNI_FALSE;
}


static JNINativeMethod sMethods[] =
{
    {"nativeInit",                         "()I",                                (void *)dpl_init},
    {"nativeDeInit",                       "()V",                                (void *)dpl_deinit},
    {"nativeHandleRawFrame",               "([B)V",                              (void *)dpl_handleRawFrame},
    {"nativeSetSurface",                   "(Landroid/view/Surface;)I",          (void *)dpl_setSurface},
    {"nativeSetDeviceOrientation",         "(I)V",                               (void *)dpl_setDeviceOrientation},
    {"nativeSetCameraFacing",              "(I)V",                               (void *)dpl_setCameraFacing},
    {"nativeSetCameraInfo",                "(II)V",                              (void *)dpl_setCameraInfo},
    {"nativeGetNegotiatedFPS",             "()S",                                (void *)dpl_getNegotiatedFPS},
    {"nativeGetNegotiatedHeight",          "()I",                                (void *)dpl_getNegotiatedHeight},
    {"nativeGetNegotiatedWidth",           "()I",                                (void *)dpl_getNegotiatedWidth},
    {"nativeGetUIOrientationMode",         "()I",                                (void *)dpl_getUIOrientationMode},
    {"nativeGetPeerHeight",                "()I",                                (void *)dpl_getPeerHeight},
    {"nativeGetPeerWidth",                 "()I",                                (void *)dpl_getPeerWidth},
    {"nativeGetVideoQualityIndication",    "()I",                                (void *)dpl_getVideoQualityIndication},
    {"nativeGetRecordingSurface",          "()Landroid/view/Surface;",           (void *)dpl_getRecordingSurface},
    {"nativeGetRecorderFrameRate",         "()I",                                (void *)dpl_getRecorderFrameRate},
    {"nativeRequestRtpDataUsage",          "(I)I",                               (void *)dpl_requestRtpDataUsage},
    {"nativeRegisterForMediaEvents",       "(Lcom/qualcomm/ims/vt/ImsMedia;)V",  (void *)dpl_registerForImsEvent},
    {"nativeHaveSameParent",               "(Landroid/view/Surface;Landroid/view/Surface;)Z", (void *)dpl_haveSameParent},
    {"nativeSetVideoImageBuffer",          "([III)I",                            (void *)dpl_setVideoImageBuffer}
};


#define IMPL_SYM_INIT                        "initImsThinClient"
#define IMPL_SYM_FRAME                       "frameToEncode"
#define IMPL_SYM_SET_SURFACE                 "setFarEndSurface"
#define IMPL_SYM_SET_DEVICE_ORIENTATION      "setDeviceOrientation"
#define IMPL_SYM_SET_CAMERA_FACING           "setCameraFacing"
#define IMPL_SYM_SET_CAMERA_INFO             "setCameraInfo"
#define IMPL_SYM_DEINIT                      "deInitImsThinClient"
#define IMPL_SYM_NEG_FPS                     "getNegotiatedFPS"
#define IMPL_SYM_NEG_HEIGHT                  "getNegotiatedHeight"
#define IMPL_SYM_NEG_WIDTH                   "getNegotiatedWidth"
#define IMPL_SYM_UI_ORIENTATION_MODE         "getUIOrientationMode"
#define IMPL_SYM_PEER_HEIGHT                 "getPeerHeight"
#define IMPL_SYM_PEER_WIDTH                  "getPeerWidth"
#define IMPL_SYM_VIDEO_QUALITY_IND           "getVideoQualityIndication"
#define IMPL_SYM_GET_RECORDING_SURFACE       "getRecordingSurface"
#define IMPL_SYM_GET_RECORDER_FRAME_RATE     "getRecorderFrameRate"
#define IMPL_SYM_REQUEST_RTP_DATA_USAGE      "requestRtpDataUsage2"
#define IMPL_SYM_GET_RTP_DATA_USAGE          "getRtpDataUsage2"
#define IMPL_SYM_GET_RTP_DATA_USAGE3         "getRtpDataUsage3"
#define IMPL_SYM_REGISTER2                   "registerAppEventCallback2"
#define IMPL_SYM_SET_VIDEO_IMAGE_BUFFER      "setVideoImageBuffer"

struct ImsMediaApis *load_ims_media_lib(const char *path)
{
    struct ImsMediaApis *ret = NULL;
    void *handle;

    if (!(handle = dlopen(path, RTLD_NOW))) {
        ALOGE("Error loading library %s: %s\n", path, dlerror());
        goto finish;
    }

    ret = (ImsMediaApis *)calloc(sizeof(struct ImsMediaApis), 1);
    if (!ret) goto close_and_finish;

    ret->initImsThinClient = (ImsMediaInitFuncPtr) dlsym(handle,IMPL_SYM_INIT);
    ret->frameToEncode = (ImsMediaFrameFuncPtr) dlsym(handle, IMPL_SYM_FRAME);
    ret->setFarEndSurface = (ImsMediaSetSurfFuncPtr) dlsym(handle, IMPL_SYM_SET_SURFACE);
    ret->setDeviceOrientation = (ImsMediaVoidUint32FuncPtr) dlsym(handle, IMPL_SYM_SET_DEVICE_ORIENTATION);
    ret->setCameraFacing
        = (ImsMediaVoidUint32FuncPtr) dlsym(handle, IMPL_SYM_SET_CAMERA_FACING);
    ret->setCameraInfo = (ImsMediaVoidUint32Uint32FuncPtr) dlsym(handle, IMPL_SYM_SET_CAMERA_INFO);
    ret->deInitImsThinClient = (ImsMediaDeinitFuncPtr) dlsym(handle, IMPL_SYM_DEINIT);
    ret->getNegotiatedFPS = (ImsMediaUint32VoidFuncPtr) dlsym(handle, IMPL_SYM_NEG_FPS);
    ret->getNegotiatedHeight = (ImsMediaUint32VoidFuncPtr) dlsym(handle, IMPL_SYM_NEG_HEIGHT);
    ret->getNegotiatedWidth = (ImsMediaUint32VoidFuncPtr) dlsym(handle, IMPL_SYM_NEG_WIDTH);
    ret->getUIOrientationMode = (ImsMediaUint32VoidFuncPtr) dlsym(handle, IMPL_SYM_UI_ORIENTATION_MODE);
    ret->getPeerHeight = (ImsMediaUint32VoidFuncPtr) dlsym(handle, IMPL_SYM_PEER_HEIGHT);
    ret->getPeerWidth = (ImsMediaUint32VoidFuncPtr) dlsym(handle, IMPL_SYM_PEER_WIDTH);
    ret->getVideoQualityIndication = (ImsMediaUint32VoidFuncPtr) dlsym(handle, IMPL_SYM_VIDEO_QUALITY_IND);
    ret->getRecordingSurface = (ImsMediaGbpFuncPtr) dlsym(handle, IMPL_SYM_GET_RECORDING_SURFACE);
    ret->getRecorderFrameRate = (ImsMediaUint32VoidFuncPtr) dlsym(handle, IMPL_SYM_GET_RECORDER_FRAME_RATE);
    ret->requestRtpDataUsage2 = (ImsMediaInt32Int32FuncPtr) dlsym(handle, IMPL_SYM_REQUEST_RTP_DATA_USAGE);
    ret->getRtpDataUsage2 = (ImsMediaDataUsageFuncPtr) dlsym(handle, IMPL_SYM_GET_RTP_DATA_USAGE);
    ret->getRtpDataUsage3 = (ImsMediaDataUsageFuncPtr3) dlsym(handle, IMPL_SYM_GET_RTP_DATA_USAGE3);
    ret->registerAppEventCallback2 = (ImsMediaRegisterCbFuncPtr2) dlsym(handle, IMPL_SYM_REGISTER2);
    ret->setVideoImageBuffer = (ImsMediaSetVideoImageBufferFuncPtr) dlsym(handle, IMPL_SYM_SET_VIDEO_IMAGE_BUFFER);

    return ret;
close_and_finish:
    if (dlclose(handle)) {
        ALOGE("Error closing library %s: %s\n", path, dlerror());
    }
finish:
    return ret;
}

int register_ims_media(JNIEnv *e) {
    char libpath[PROPERTY_VALUE_MAX];

    ALOGD("%s\n", __func__);

    jclass klass;

    klass = e->FindClass("com/qualcomm/ims/vt/ImsMedia");
    if (!klass) {
        ALOGE("%s: Unable to find java class com/qualcomm/ims/vt/ImsMedia\n",
                 __func__);
        return JNI_ERR;
    }

    //Cache the ImsMedia class reference
    gClassImsMedia = (jclass) e->NewGlobalRef(klass);

    property_get(IMPL_LIB_PROPERTY_NAME, libpath, DEFAULT_IMPL_LIB_PATH);
    imsMediaApis = load_ims_media_lib(libpath);

    const jint sMethodsSize = sizeof(sMethods) / sizeof(sMethods[0]);
    return e->RegisterNatives(klass, sMethods, sMethodsSize);
}
