/*==============================================================================
*  @file WFDNative.cpp
*
*  @par DESCRIPTION:
*
*
*
*  Copyright (c) 2012 - 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/


#define LOG_TAG "WFDNative_CPP"

#define LOG_NDEBUG 0
#define LOG_NDDEBUG 0
#include <utils/Log.h>
#ifndef WFD_ICS
#include "common_log.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <unistd.h>
#include <Surface.h>
#include <input/Input.h>
#include <android_view_MotionEvent.h>
#include <android_view_KeyEvent.h>
#include <android_runtime/android_view_Surface.h>

#include "UIBCDefs.h"
#include "com_qualcomm_wfd_WFDSession.h"
#include "WiFiDisplayClient.h"

#define WFD_NATIVE_STRING_LEN 256
#define WFD_NATIVE_STRING_ARR_SIZE 50
#define UNUSED(x) ((void)x)

#define MM_UPDATE_EVENT         "MMEvent"
#define UIBC_ROTATE_EVENT       "UIBCRotateEvent"
#define WIFIDISPLAY_STRING_SIZE     256
#include "WFDDefs.h"

static inline nsecs_t curTime() {
    return systemTime(SYSTEM_TIME_MONOTONIC);
}


#define FIND_CLASS(var, className) do{\
        var = env->FindClass(className); \
        LOG_FATAL_IF(! var, "Unable to find class " className); \
        var = jclass(env->NewGlobalRef(var));\
}while(0)

#define GET_METHOD_ID(var, clazz, methodName, methodDescriptor) do{\
        var = env->GetMethodID(clazz, methodName, methodDescriptor); \
        LOG_FATAL_IF(! var, "Unable to find method " methodName);\
}while(0)

#define GET_FIELD_ID(var, clazz, fieldName, fieldDescriptor) do{\
         var = env->GetFieldID(clazz, fieldName, fieldDescriptor); \
         if(!var)LOGE("Unable to find field ");\
}while(0)

#define DETACH_THREAD(isAttached) do {\
    if (isAttached)\
        gJavaVM->DetachCurrentThread();\
}while(0)

using namespace android;
using namespace std;

static struct {
    jclass clazz;

    jfieldID deviceType;
    jfieldID netType;
    jfieldID macAddress;
    jfieldID deviceName;
    jfieldID ipAddress;
    jfieldID rtspPort;
    jfieldID decoderLatency;
    jfieldID extSupport;
} gWfdDeviceClassInfo;

static struct {
    jclass clazz;
    jfieldID surface;
} gSurface;

static struct {
    jclass wfdSessiionClazz;
    jclass surfacePropClazz;
    jfieldID surfaceCoord;
    jfieldID screenProps;
} gWFDSurfacePropClassInfo;

static struct {
    jclass wfdSessionClazz;
    jmethodID eventCallbackMethod;
    jfieldID nativePtr;
} gWFDSessionClassInfo;

static void wfdDeviceToNative(JNIEnv* env, jobject wfdDeviceObj, WfdDevice* outRawDevice);
static void jni_stringarray_callback(const char* eName, int numObjects, char
strArray[][256],const jobject& wfdSessionObj);

static JavaVM *gJavaVM;

struct EventCallbackArgs {
    jclass uibcMgrCls;
    jfieldID uibcMgrFieldID;
    jmethodID HIDeventCbackmethod;
    JavaVM *cached_VM;
    boolean isAttached;
}gEventCallbackArgs;

WiFiDisplayClient::screenDimensions gScreenDimensions;

struct {
    int32_t height;
    int32_t width;
    float aspectRatio;
} gResolution;// TODO: MI Task Get rid of this for source to support MI

//Specially for caching VM and UIBC events
struct EventDispatcherClassInfo {
    jclass clazz;
    jmethodID addEvent;
}gUIBCEventDispatcherClassInfo;

static const float X_OFFSET = 0.0f;
static const float Y_OFFSET = 0.0f;
static const float TOUCH_PRESSURE = 0.8f;
static const float TOUCH_SIZE     = 0.04f;
static const int MOTION_EVENT_DEVICE_ID = 0;
static const int KEY_EVENT_DEVICE_ID = 0;
/* list of native methods */
static JNINativeMethod gMethods[] = {
        // nameOfNativeMethod,
        // methodSignature,
        // methodPointer
        {   "nativeInit",
            "()I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeInit},
        {   "nativeCreateWfdSession",
            "(Lcom/qualcomm/wfd/WfdDevice;Lcom/qualcomm/wfd/WfdDevice;J)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeCreateWfdSession},
        {   "nativeStopWfdSession",
            "(IJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeStopWfdSession},
        {   "nativePlay",
            "(IZJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativePlay},
        {   "nativePause",
            "(IZJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativePause},
        {   "nativeTeardown",
            "(IZJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeTeardown},
        {   "nativeStandby",
            "(IJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeStandby},
        {   "nativeEnableUIBC",
            "(IJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeEnableUIBC},
        {   "nativeDisableUIBC",
            "(IJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeDisableUIBC},
        {   "nativeStartUIBC",
            "(Ljava/lang/Object;J)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeStartUIBC},
        {   "nativeStopUIBC",
            "(J)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeStopUIBC},
        {   "nativeSetUIBC",
            "(IJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeSetUIBC},
        {   "nativeSetVideoSurface",
            "(Landroid/view/Surface;J)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeSetVideoSurface},
        {   "nativeSendUIBCKeyEvent",
            "(Landroid/view/KeyEvent;J)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeSendUIBCKeyEvent},
        {   "nativeSendUIBCMotionEvent",
            "(Landroid/view/MotionEvent;J)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeSendUIBCMotionEvent},
        {   "nativeSendUIBCRotateEvent",
            "(IJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeSendUIBCRotateEvent},
        {   "nativeSetResolution",
            "(II[IJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeSetResolution},
        {   "nativeSetCodecResolution",
            "(IIIII[IJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeSetCodecResolution},
        {   "nativeSetDecoderLatency",
            "(IJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeSetDecoderLatency},
        {   "nativeSetSurfaceProp",
            "(Lcom/qualcomm/wfd/WFDSession$surfaceProperties;J)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeSetSurfaceProp},
        {   "nativeGetConfigItems",
            "([IJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeGetConfigItems},
        {   "nativeExecuteRuntimeCommand",
            "(IJ)I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeExecuteRuntimeCommand},
        {   "nativeGetNegotiatedRes",
            "([IJ)[I",
            (void*) Java_com_qualcomm_wfd_WFDSession_nativeGetNegotiatedRes}
};

/** =======================================================================
**               JNI OnLoad and OnUnload
** ======================================================================= */
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    UNUSED(reserved);
    JNIEnv *env;

    //store java virtual machine in global variable
    gJavaVM = vm;
    gEventCallbackArgs.cached_VM = vm;

    LOGI("JNI_OnLoad called");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("Failed to get the environment using GetEnv()");
        return JNI_FALSE;
    }

    FIND_CLASS(gUIBCEventDispatcherClassInfo.clazz, "com/qualcomm/wfd/UIBCManager$EventDispatcher");
    GET_METHOD_ID(gUIBCEventDispatcherClassInfo.addEvent, gUIBCEventDispatcherClassInfo.clazz,
                  "addEvent", "(Landroid/view/InputEvent;)V");
    /*Populate WfdDevice class info*/
    FIND_CLASS(gWfdDeviceClassInfo.clazz, "com/qualcomm/wfd/WfdDevice");
    gWfdDeviceClassInfo.clazz = jclass(env->NewGlobalRef(gWfdDeviceClassInfo.clazz));

    GET_FIELD_ID(gWfdDeviceClassInfo.deviceName, gWfdDeviceClassInfo.clazz,
                 "deviceName", "Ljava/lang/String;");
    GET_FIELD_ID(gWfdDeviceClassInfo.deviceType, gWfdDeviceClassInfo.clazz,
                 "deviceType", "I");
    GET_FIELD_ID(gWfdDeviceClassInfo.netType, gWfdDeviceClassInfo.clazz,
                 "netType", "I");
    GET_FIELD_ID(gWfdDeviceClassInfo.ipAddress, gWfdDeviceClassInfo.clazz,
                 "ipAddress", "Ljava/lang/String;");
    GET_FIELD_ID(gWfdDeviceClassInfo.macAddress, gWfdDeviceClassInfo.clazz,
                 "macAddress", "Ljava/lang/String;");
    GET_FIELD_ID(gWfdDeviceClassInfo.rtspPort, gWfdDeviceClassInfo.clazz,
                 "rtspPort", "I");
    GET_FIELD_ID(gWfdDeviceClassInfo.decoderLatency, gWfdDeviceClassInfo.clazz,
                 "decoderLatency", "I");
    GET_FIELD_ID(gWfdDeviceClassInfo.extSupport, gWfdDeviceClassInfo.clazz,
                 "extSupport", "I");

    /* Native function registration */
    FIND_CLASS(gWFDSessionClassInfo.wfdSessionClazz,
                "com/qualcomm/wfd/WFDSession");
    if (env->RegisterNatives(gWFDSessionClassInfo.wfdSessionClazz,
        gMethods, sizeof(gMethods)/sizeof(gMethods[0])) < 0) {
        LOGE("RegisterNatives failed for '%s'",
            "com/qualcomm/wfd/WFDSession");
        return JNI_FALSE;
    }
    FIND_CLASS(gSurface.clazz, "android/view/Surface");
    GET_FIELD_ID(gSurface.surface, gSurface.clazz, "mNativeSurface", "I");

    if(gSurface.surface == NULL) {
      env->ExceptionClear();
      GET_FIELD_ID(gSurface.surface, gSurface.clazz, "mNativeObject", "I");
    }

    if(gSurface.surface == NULL) {
      env->ExceptionClear();
      GET_FIELD_ID(gSurface.surface, gSurface.clazz, "mNativeObject", "J");
    }

    FIND_CLASS(gWFDSurfacePropClassInfo.surfacePropClazz,
        "com/qualcomm/wfd/WFDSession$surfaceProperties");
    GET_FIELD_ID(gWFDSurfacePropClassInfo.screenProps,
                gWFDSurfacePropClassInfo.surfacePropClazz,
                "screenProps","[I");
    GET_FIELD_ID(gWFDSurfacePropClassInfo.surfaceCoord,
                gWFDSurfacePropClassInfo.surfacePropClazz,
                "surfaceCoord","[I");

    FIND_CLASS(gEventCallbackArgs.uibcMgrCls,"com/qualcomm/wfd/UIBCManager");
    GET_FIELD_ID(gEventCallbackArgs.uibcMgrFieldID ,
                gWFDSessionClassInfo.wfdSessionClazz, "mUIBCManager",
                "Lcom/qualcomm/wfd/UIBCManager;");
    gEventCallbackArgs.HIDeventCbackmethod= env->GetMethodID(gEventCallbackArgs.uibcMgrCls,"HIDEventCallback", "([BI)V");
    /*Cache WFDSession.java methods*/
    gWFDSessionClassInfo.eventCallbackMethod = env->GetMethodID(
        gWFDSessionClassInfo.wfdSessionClazz, "eventCallback",
        "(Ljava/lang/String;[Ljava/lang/Object;)V");
    gWFDSessionClassInfo.nativePtr = env->GetFieldID(
        gWFDSessionClassInfo.wfdSessionClazz,"mNativePtr","J");
    return JNI_VERSION_1_4;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved)
{
    UNUSED(vm);
    UNUSED(reserved);
    gJavaVM = 0;
}

int keysym2keycode(unsigned int key, int* metaState)
{
    int keycode = 0;
    int code = (int)key;

    if (code>='0' && code<='9') {
        keycode = (code & 0xF);
        keycode += AKEYCODE_0;
    }
    else if ((code>='A' && code<='Z') || (code>='a' && code<='z')) {
      static const uint16_t map[] = {
          AKEYCODE_A, AKEYCODE_B, AKEYCODE_C, AKEYCODE_D, AKEYCODE_E ,
          AKEYCODE_F, AKEYCODE_G, AKEYCODE_H, AKEYCODE_I, AKEYCODE_J,
          AKEYCODE_K, AKEYCODE_L, AKEYCODE_M, AKEYCODE_N, AKEYCODE_O,
          AKEYCODE_P, AKEYCODE_Q, AKEYCODE_R, AKEYCODE_S, AKEYCODE_T,
          AKEYCODE_U, AKEYCODE_V, AKEYCODE_W, AKEYCODE_X, AKEYCODE_Y,
          AKEYCODE_Z };
      int index = static_cast<int>((code|0x20) - 'a');
      if(index >=0 && index < (int)(sizeof(map)/sizeof(map[0]))) {
         keycode = map[index];
      }
      if (code>='A' && code<='Z') {
         (*metaState)|= AMETA_SHIFT_ON;
      }
    }
    else {
      switch (code) {
        case 0x0008:  keycode = AKEYCODE_DEL;                break;
        case 0x0009:  keycode = AKEYCODE_TAB;                break;
        case 0x000D:  keycode = AKEYCODE_ENTER;              break;
        case 0x001B:  keycode = AKEYCODE_ESCAPE;             break;
        case 0x0020:  keycode = AKEYCODE_SPACE;              break;
        case 0x0021:  keycode = AKEYCODE_1;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x0022:  keycode = AKEYCODE_APOSTROPHE ;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x0023:  keycode = AKEYCODE_POUND ;             break;
        case 0x0024:  keycode = AKEYCODE_4;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x0025:  keycode = AKEYCODE_5;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x0026:  keycode = AKEYCODE_7;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x0027:  keycode = AKEYCODE_APOSTROPHE;         break;
        case 0x0028:  keycode = AKEYCODE_NUMPAD_LEFT_PAREN;  break;
        case 0x0029:  keycode = AKEYCODE_NUMPAD_RIGHT_PAREN; break;
        case 0x002B:  keycode = AKEYCODE_PLUS;               break;
        case 0x002C:  keycode = AKEYCODE_COMMA;              break;
        case 0x002D:  keycode = AKEYCODE_MINUS;              break;
        case 0x002E:  keycode = AKEYCODE_PERIOD;             break;
        case 0x002F:  keycode = AKEYCODE_SLASH ;             break;
        case 0x003B:  keycode = AKEYCODE_SEMICOLON ;         break;
        case 0x003A:  keycode = AKEYCODE_SEMICOLON ;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x003C:  keycode = AKEYCODE_COMMA ;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x002A:  keycode = AKEYCODE_STAR;               break;
        case 0x003D:  keycode = AKEYCODE_EQUALS;             break;
        case 0x003E:  keycode = AKEYCODE_PERIOD;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x003F:  keycode = AKEYCODE_SLASH;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x0040:  keycode = AKEYCODE_AT;                 break;
        case 0x005B:  keycode = AKEYCODE_LEFT_BRACKET;       break;
        case 0x005C:  keycode = AKEYCODE_BACKSLASH;          break;
        case 0x005D:  keycode = AKEYCODE_RIGHT_BRACKET;      break;
        case 0x005E:  keycode = AKEYCODE_6;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x005F:  keycode = AKEYCODE_MINUS;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x0060:  keycode = AKEYCODE_GRAVE;              break;
        case 0x007B:  keycode = AKEYCODE_LEFT_BRACKET;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x007C:  keycode = AKEYCODE_BACKSLASH;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x007D:  keycode = AKEYCODE_RIGHT_BRACKET;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x007E:  keycode = AKEYCODE_GRAVE;
                      (*metaState)|= AMETA_SHIFT_ON;         break;
        case 0x007F:  keycode = AKEYCODE_FORWARD_DEL;        break;
        default    :  keycode = 0;
      }
    }

    LOGD("keysym2keycode key: %d, keycode: %d meta state%d", key, keycode, (*metaState));
    return keycode;
}


/** =======================================================
*@brief Function for adjusting difference between WFD
        session resolution and device screen resolution
*
*@params xin normalized x co-ord captured from sink device
         yin normalized y co-ord captured from sink device

Logic:

If the device screen aspect ratio is not matching with that
of the WFD session then either x or y while injecting needs
to be modified accordingly since one of them will remain
invariant while transforming the co-ordiantes.Essentially,
the adjustment can be derived by reconstructing sink side
screen and calculating where the source side screen edges
will be at the sink capturing surface.

** =======================================================*/
void adjustForAspectRatio(double& xin, double& yin)
{
    LOGD(" AR %f original xin = %lf yin = %lf",gResolution.aspectRatio,
         xin, yin);
    if(gScreenDimensions.aspectRatio < gResolution.aspectRatio)
    {
        //Pillar boxes on screen so adjust for x
        if(gResolution.aspectRatio != 0)
        {
            xin = (xin - 0.5)/(gScreenDimensions.aspectRatio/(gResolution.aspectRatio*2));
            //This will yield values in the range of [-1,1]
            //Need to transform it to the range [0,1]
            xin = 0.5*(1 +  xin);
            if (!(xin>=0 && xin <= 1))
            {
                xin = yin = -1 ;//Do not inject rogue touches
            }
            LOGD("Pillar boxes x = %lf",xin);
        }
        else
        {
            LOGE("gResolution.aspectRatio is zero");
        }
    }
    else if (gScreenDimensions.aspectRatio > gResolution.aspectRatio)
    {
        //Letter boxes on screen so adjust for y
        if(gScreenDimensions.aspectRatio!=0)
        {
            yin = (yin - 0.5)/(gResolution.aspectRatio/(gScreenDimensions.aspectRatio*2));
            //This will yield values in the range of [-1,1]
            //Need to transform it to the range [0,1]
            yin = 0.5*(1 + yin);
            if (!(yin>=0 && yin <= 1))
            {
                xin = yin = -1 ;//Do not inject rogue touches
            }
            LOGD("Letter boxes y = %lf",yin);
        }
        else
        {
            LOGE("gScreenDimensions.aspectRatio is zero");
        }
    }
    //No need to make adjustments if both the aspect ratios are equal
}
/**
 * Function to convert UIBC event data structure defined in MM
 * lib to Android event data structure
 * @param ev
 *
 * @return jobject
 */

static bool SendEvent(WFD_uibc_event_t* const  ev, const void * const pClientData,
        const jobject& wfdSessionObj, const jobject& eventDispatcherObj)
{

    const WiFiDisplayClient* const client =
        static_cast<const WiFiDisplayClient* >((pClientData));

    uibc_Fields *uibcParam = NULL;

    if(!(client && (uibcParam = client->getUIBCField())))
    {
        LOGE("Invalid arguments for %s",__FUNCTION__);
        return false;
    }
    JNIEnv *jniEnv ;
    nsecs_t eventTime = 0;
    int angle = 0;
    bool isAttached = false;
    bool injectEvent = true;
    jint status = gJavaVM->GetEnv((void **) &jniEnv, JNI_VERSION_1_4);
    if (status == JNI_EDETACHED) {
        if (gJavaVM->AttachCurrentThread(&jniEnv, NULL) != 0) {
            LOGE("Failed to attach thread to JVM!!!");
            return false;
        }
        isAttached = true;
    } else if (status == JNI_OK) {
        //The thread is already attached, JVM will take care of GC'ng it
    } else if (status == JNI_EVERSION) {
        LOGE("GetEnv: version JNI_VERSION_1_4 not supported");
        return false;
    }
    jobject eventDispatcherObject = eventDispatcherObj;
    if (!ev || !jniEnv || !eventDispatcherObject) {
        LOGE("Invalid argument");
        return FALSE;
    }
    jobject eventObj = NULL;
    LOGD("Event type = %d", ev->type);
    switch (ev->type)
    {
    case WFD_UIBC_TOUCH:
     {
        MotionEvent *newEv = new MotionEvent();
        if (newEv == NULL){
          LOGE ("Failed to obtain a native MotionEvent object");
          DETACH_THREAD(isAttached);
          return FALSE;
        }
        int eventActionType = -1;
        if(!(uibcParam->multiMode) && (ev->parms.touch_event.num_pointers > 1)) {
            //This is the first time a multi touch event is received after a
            //stream of single touch events. Set the multi mode flag to true
            uibcParam->multiMode = true;
            LOGD("Multi touch mode is ON");
        }

        //For multi touch events its the Action type that needs to be taken care of

        if(uibcParam->multiMode) {
            if(uibcParam->prevEv->getPointerCount() < ev->parms.touch_event.num_pointers) {
                //The pointer count of the incoming event is > last event
                //Hence this is definitely a POINTER_DOWN event
                LOGD("AMOTION_EVENT_ACTION_POINTER_DOWN");
                //1. Set the LSB to the correct action
                eventActionType = AMOTION_EVENT_ACTION_POINTER_DOWN;
                //2. Clear everything except LSB
                eventActionType &= AMOTION_EVENT_ACTION_MASK;
                //3. Now search for which pointer index actually went down
                for(int i = 0; i< ev->parms.touch_event.num_pointers ;i++) {
                   if(uibcParam->prevEv->findPointerIndex(ev->parms.touch_event.pointer_id[i])== -1){
                    //Previous touch event did not have this pointer ID
                    //This pointer ID's index needs to go down.
                    //4. Set the correct index in action in the correct byte
                     eventActionType|= i<<AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                     break;//break out of loop since only 1 id can differ
                   }
                }
            } else if(uibcParam->prevEv->getPointerCount() > ev->parms.touch_event.num_pointers) {
                //The pointer count of the incoming event is < last event
                //So the sink has had one of its pointers lifted.
                //This is very Android specific. At the sink side, POINTER_UP
                //(with the same pointer count as the previous event) will be
                //delivered to the source. The source has no way of discerning
                //between a POINTER_MOVE and POINTER_UP as such since
                //these events are not supported in WFD specification. So when
                //actually the pointer count decreases, the source can inject a
                //filler event mimicking a POINTER_UP
                LOGD("AMOTION_EVENT_ACTION_POINTER_UP");
                //Construct a dummy event from the previous event and fake it
                //to be a pointer up with the correct index. Then continue ahead
                //with the processing of the event
                MotionEvent *fillerEv = new MotionEvent();
                int fillerAction = -1;
                list<int> pointerID;
                for(size_t i = 0; i< uibcParam->prevEv->getPointerCount();i++) {
                   pointerID.push_back(uibcParam->prevEv->getPointerId(i));
                }
                for(int i = 0; i< ev->parms.touch_event.num_pointers ;i++) {
                   pointerID.remove(ev->parms.touch_event.pointer_id[i]);
                }
                for(list<int>::iterator beg = pointerID.begin();beg != pointerID.end();beg++)
                   LOGD("Remaining Pointer ID = %d",*beg);
                if(pointerID.size() != 1 ) {
                   //reset the multi-touch mode
                   uibcParam->multiMode = false;
                   LOGD("Abrupt change in pointer co-ords, change Multi-"\
                                "touch mode to OFF");
                   DETACH_THREAD(isAttached);
                   delete fillerEv;
                   delete newEv;
                   return FALSE;
                }

               //This list now contains the pointer ID that differs between the
               //previously injected event and the incoming event, hence this
               //yields the pointer up index that need to be filled in

               //1. Set the LSB to the correct action
               fillerAction= AMOTION_EVENT_ACTION_POINTER_UP;
               //2. Clear everything except LSB
               fillerAction&=AMOTION_EVENT_ACTION_MASK;
               //3. Now search for which pointer index actually went up
               int upIndex = uibcParam->prevEv->findPointerIndex(pointerID.back());
               fillerAction|= (upIndex)<<AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
               //The entire initialize rigmarole is necessary because simply
               //using copyFrom to copy from prevEv won't help. The correct
               //timestamp needs to be supplied for the event and unfortunately
               //there is no API to set the event time of a motion event
               nsecs_t fillerEvTime = curTime();
               int fillerEvPtrCount = uibcParam->prevEv->getPointerCount();
               fillerEv->initialize(MOTION_EVENT_DEVICE_ID,
                            AINPUT_SOURCE_TOUCHSCREEN, fillerAction,
                            0, // actionButton value TBD
                            AMOTION_EVENT_FLAG_WINDOW_IS_OBSCURED,
                            AMOTION_EVENT_EDGE_FLAG_NONE,
                            AMETA_NONE,AMOTION_EVENT_BUTTON_PRIMARY,
                            X_OFFSET, Y_OFFSET, 2.0f, 2.1f,
                            (uibcParam->prevEv)->getDownTime(), fillerEvTime,
                            fillerEvPtrCount, uibcParam->prevPtrProp,
                            uibcParam->prevPtrCoords);
               jobject fillerEvent = android_view_MotionEvent_obtainAsCopy(jniEnv, (const MotionEvent*) fillerEv);
               LOGD("Injecting filler event with action %d",fillerAction);
               jniEnv->CallVoidMethod(eventDispatcherObject, gUIBCEventDispatcherClassInfo.addEvent, fillerEvent);
               jniEnv->DeleteLocalRef(fillerEvent);
               delete fillerEv;
               if(ev->parms.touch_event.num_pointers == 1) {
                 //Multi-mode is still on and the incoming event has only one
                 //pointer, so the stream of multi-touch has ended, turn off
                 // multi mode
                 uibcParam->multiMode = false;
                 LOGD("Multi touch mode is OFF");
                 //Set the event action type as that of the incoming event
                 if (ev->parms.touch_event.type == WFD_UIBC_TOUCH_DOWN) {
                     eventActionType = AMOTION_EVENT_ACTION_DOWN;
                 } else if (ev->parms.touch_event.type == WFD_UIBC_TOUCH_UP) {
                     eventActionType = AMOTION_EVENT_ACTION_UP;
                 } else {
                     eventActionType = AMOTION_EVENT_ACTION_MOVE;
                 }
               } else {
                 //The pointer count is still > 1, hence continue injecting
                 //with MOVE action as in normal multi mode
                 eventActionType = AMOTION_EVENT_ACTION_MOVE;
               }
          } else {
             //no change in number of pointers in multi-touch events
             //since multi mode is on, inject it simply as a MOVE
             eventActionType = AMOTION_EVENT_ACTION_MOVE;
          }
        } else {//take the normal single touch route
            if (ev->parms.touch_event.type == WFD_UIBC_TOUCH_DOWN) {
                eventActionType = AMOTION_EVENT_ACTION_DOWN;
            } else if (ev->parms.touch_event.type == WFD_UIBC_TOUCH_UP) {
                eventActionType = AMOTION_EVENT_ACTION_UP;
            } else {
                eventActionType = AMOTION_EVENT_ACTION_MOVE;
            }
        }

        if (ev->parms.touch_event.num_pointers > 0 && ev->parms.touch_event.num_pointers <= UIBC_MAX_TOUCH_INPUTS) {
            PointerCoords pointerCoords[UIBC_MAX_TOUCH_INPUTS];
            PointerProperties pointerProps[UIBC_MAX_TOUCH_INPUTS];
            double point_x=0,point_y=0;
            for (int i = 0, j=0; i < ev->parms.touch_event.num_pointers; i++) {
                point_x = ev->parms.touch_event.coordinate_x[i];
                point_y = ev->parms.touch_event.coordinate_y[i];
                if((point_x>=0)&&(point_x<=1)&&(point_y>=0)&&(point_x<=1)) {
                   pointerProps[j].clear();
                   pointerProps[j].id = ev->parms.touch_event.pointer_id[i];
                   pointerCoords[j].clear();
                   adjustForAspectRatio(point_x, point_y);
                   if(!(point_x >= 0 && point_x <=1 && point_y >= 0 && point_y <= 1)) {
                        injectEvent = false;
                        LOGD("Not injecting rogue touches");
                   }
                   ev->parms.touch_event.coordinate_x[j] =(int)(point_x * gScreenDimensions.width);
                   ev->parms.touch_event.coordinate_y[j] =(int ) (point_y * gScreenDimensions.height);
                   pointerCoords[j].setAxisValue(AMOTION_EVENT_AXIS_X,\
                                ev->parms.touch_event.coordinate_x[i]);
                   pointerCoords[j].setAxisValue(AMOTION_EVENT_AXIS_Y,\
                             ev->parms.touch_event.coordinate_y[i]);
                   pointerCoords[j].setAxisValue(AMOTION_EVENT_AXIS_PRESSURE,\
                             TOUCH_PRESSURE);
                   j++;
                }
            }
            eventTime = curTime();
            if(eventActionType == AMOTION_EVENT_ACTION_DOWN) {
                uibcParam->downTime= eventTime;
            }
            LOGD(" Event time = %zd",eventTime);
            newEv->initialize(MOTION_EVENT_DEVICE_ID, AINPUT_SOURCE_TOUCHSCREEN, eventActionType,
                              0, // actionButton value TBD
                              AMOTION_EVENT_FLAG_WINDOW_IS_OBSCURED,
                              AMOTION_EVENT_EDGE_FLAG_NONE, AMETA_NONE,AMOTION_EVENT_BUTTON_PRIMARY,
                              X_OFFSET, Y_OFFSET, 2.0f, 2.1f, uibcParam->downTime, eventTime,
                              ev->parms.touch_event.num_pointers, pointerProps, pointerCoords);
            uibcParam->prevEv->copyFrom(newEv,true);
            for(int i = 0; i < ev->parms.touch_event.num_pointers; i++) {
                uibcParam->prevPtrCoords[i].copyFrom(pointerCoords[i]);
                uibcParam->prevPtrProp[i].copyFrom(pointerProps[i]);
            }
        } else {
            LOGE("Invalid number of inputs [%d] or mouse events are NULL", ev->parms.touch_event.num_pointers);
              delete newEv;
            break;
        }
        eventObj = android_view_MotionEvent_obtainAsCopy(jniEnv, (const MotionEvent*) newEv);
        delete newEv;
        break;
     }
    case WFD_UIBC_KEY:
      {
        KeyEvent *newEvent = new KeyEvent();
        if (newEvent == NULL) {
          LOGE("Failed to get a native KeyEvent object");
          DETACH_THREAD(isAttached);
          return FALSE;
        }
        int eventActionType=0;
        if (ev->parms.key_event.type == WFD_UIBC_KEY_DOWN) {
            eventActionType = AKEY_EVENT_ACTION_DOWN;
        } else if (ev->parms.key_event.type == WFD_UIBC_KEY_UP) {
            eventActionType = AKEY_EVENT_ACTION_UP;
        }
        //Ignoring Key Code 2 for now
        eventTime = curTime();
        int metaState = 0;
        unsigned int keyCode = keysym2keycode(ev->parms.key_event.key_code_1, &metaState);
        newEvent->initialize(KEY_EVENT_DEVICE_ID, AINPUT_SOURCE_KEYBOARD,
                             eventActionType,0,keyCode,0,metaState,0,eventTime,eventTime - 1 );
        eventObj = android_view_KeyEvent_fromNative(jniEnv,(const KeyEvent*)newEvent);
        delete newEvent;
        break;
     }
    case WFD_UIBC_ROTATE:
    {
      // Convert into degrees and then rotate to the nearest angle
      angle = (int) (((float)ev->parms.rotate_event.num_rotate_int +
      (float)ev->parms.rotate_event.num_rotate_fraction/256.0) * (180/3.141));
      LOGD("Angle extracted from sink = %d", angle);
      if (angle > 0 && angle < 45)
          angle = 0;
      else if (angle >=45 && angle < 135)
          angle = 90;
      else if (angle >=135 && angle < 225)
          angle = 180;
      else if (angle >=225 && angle < 315)
          angle = 270;
      else
          angle = 0;
      const int numObjects = 1;
      char strarray[numObjects][WIFIDISPLAY_STRING_SIZE];
      snprintf(strarray[0],256,"%d", angle);
      LOGD("Sending Rotate event");
      jni_stringarray_callback(UIBC_ROTATE_EVENT, numObjects, strarray,wfdSessionObj);
      break;
    }
    default: //do nothing
        LOGE("Unsupported event type: %d", ev->type);
    }
    if (!eventObj && WFD_UIBC_ROTATE != ev->type) {
        LOGE("Failed to create event object");
    }
    else
    {
        if(injectEvent) {
          jniEnv->CallVoidMethod(eventDispatcherObject,
            gUIBCEventDispatcherClassInfo.addEvent, eventObj);
        }
        //Don't forget to delete the local reference
        jniEnv->DeleteLocalRef(eventObj);
    }
    DETACH_THREAD(isAttached);
    return TRUE;

}

static bool sendHIDEvent(const uint8* const HIDPack, uint8 len,
    const HIDDataType& type, const jobject& uibcMgrObj)
{
    LOGD("sendHIDEvent");
    JNIEnv *env;
    boolean isAttached = false;
    jint status = gJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4);
    if (status == JNI_EDETACHED) {
        LOGE("GetEnv: not attached Attaching thread");
        if (gJavaVM->AttachCurrentThread(&env, NULL) != 0) {
            LOGE("Failed to attach thread to JVM!!!");
            return false;
        }
        isAttached = true;
    } else if (status == JNI_OK) {
        //The thread is already attached, JVM will take care of GC'ng it
    } else if (status == JNI_EVERSION) {
        LOGE("GetEnv: version JNI_VERSION_1_4 not supported");
        return false;
    }

    jsize length = len;
    //create a native array of jbytes before sending it across
    jbyte* tempArr;
    tempArr = new jbyte[length];

    if(!tempArr)
    {
        LOGE("sendHIDEvent: tempArr memory allocation failed");
        DETACH_THREAD(isAttached);
        return false;
    }

    for(jint i = 0; i<length;i++)
    {
        tempArr[i] = HIDPack[i];
    }
    jbyteArray HIDPacket = env->NewByteArray(length);
    if(HIDPacket)
    {
        env->SetByteArrayRegion(HIDPacket,0,length,tempArr);
    }
    else
    {
        LOGE("Failed to get an array");
        delete[] tempArr;
        DETACH_THREAD(isAttached);
        return false;
    }
    jint hidDataType = (jint)type;
    LOGD("Callback for HID event for type %d with payload %d", hidDataType,len);
    env->CallVoidMethod(uibcMgrObj, gEventCallbackArgs.HIDeventCbackmethod,
        HIDPacket,hidDataType);
    env->DeleteLocalRef(HIDPacket);
    delete[] tempArr;
    DETACH_THREAD(isAttached);
    return true;
}

/*
 * WfdDevice (jobject) <-> Device (native) conversion
 */
static void wfdDeviceToNative(JNIEnv* env, jobject wfdDeviceObj, WfdDevice* outRawDevice){
    const char *str;
    jstring jstr;

    //memset(outRawDevice, 0, sizeof(*outRawDevice));
    jstr = jstring(env->GetObjectField(wfdDeviceObj, gWfdDeviceClassInfo.ipAddress));

    if (jstr) {
        str = env->GetStringUTFChars(jstr, NULL);
        LOG_FATAL_IF(! str, "Out of memory when retrieving jstring chars");
        strlcpy(outRawDevice->ipaddress, str, sizeof(outRawDevice->ipaddress));
        env->ReleaseStringUTFChars(jstr, str);
        env->DeleteLocalRef(jstr);
    } else {
        strlcpy(outRawDevice->ipaddress, "", sizeof(outRawDevice->ipaddress));
    }
    jstr = jstring(env->GetObjectField(wfdDeviceObj, gWfdDeviceClassInfo.macAddress));
    if (jstr) {
        str = env->GetStringUTFChars(jstr, NULL);
        LOG_FATAL_IF(! str, "Out of memory when retrieving jstring chars");
        strlcpy(outRawDevice->macaddress, str, sizeof(outRawDevice->macaddress));
        env->ReleaseStringUTFChars(jstr, str);
        env->DeleteLocalRef(jstr);
    } else {
        strlcpy(outRawDevice->macaddress, "", sizeof(outRawDevice->macaddress));
    }

    outRawDevice->SMControlPort = env->GetIntField(wfdDeviceObj, gWfdDeviceClassInfo.rtspPort);
    outRawDevice->decoderLatency = env->GetIntField(wfdDeviceObj, gWfdDeviceClassInfo.decoderLatency);
    outRawDevice->extSupport = env->GetIntField(wfdDeviceObj, gWfdDeviceClassInfo.extSupport);
    outRawDevice->deviceType = env->GetIntField(wfdDeviceObj, gWfdDeviceClassInfo.deviceType);
    outRawDevice->netType = env->GetIntField(wfdDeviceObj, gWfdDeviceClassInfo.netType);

}

/*
 * Class:     com_qualcomm_wfd_WFDSession
 * Method:    nativeCreateWfdSession
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeInit
  (JNIEnv * env, jobject wfdSessionObj)
{
    WiFiDisplayClient* wfdClient = new
    WiFiDisplayClient(&jni_stringarray_callback,
                &SendEvent,&sendHIDEvent,env,wfdSessionObj);
    if(!wfdClient)
    {
        LOGE("Failed to instantiate client!");
        return -1;
    }
    jlong client = reinterpret_cast<jlong>(wfdClient);
    env->SetLongField(wfdSessionObj, gWFDSessionClassInfo.nativePtr,client);
    LOGE("native client obj on heap = %p and in java long is %zd",
        wfdClient,client);
    gScreenDimensions = wfdClient->screenDims();
    return 0;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeCreateWfdSession
 * Signature: (Lcom/qualcomm/wfd/WfdDevice;Lcom/qualcomm/wfd/WfdDevice;J)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeCreateWfdSession
      (JNIEnv * env, jobject wfdSessionObj, jobject thisDevice, jobject peerDevice,jlong nativePtr)
{
    (void)wfdSessionObj;
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        // These objects will be deleted later in ~WiFiDisplayClient
        WfdDevice* thisDeviceNative = new WfdDevice();
        if(!thisDeviceNative)
        {
            LOGE("Failed to allocate local device!");
            return -1;
        }
        WfdDevice* peerDeviceNative = new WfdDevice();
        if(!peerDeviceNative)
        {
            LOGE("Failed to allocate peer device!");
            delete thisDeviceNative;
            return -1;
        }
        wfdDeviceToNative(env,thisDevice,thisDeviceNative);
        wfdDeviceToNative(env,peerDevice,peerDeviceNative);
        return client->createWfdSession(thisDeviceNative,peerDeviceNative);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeStopWfdSession
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeStopWfdSession
  (JNIEnv *env, jobject wfdSessionObj, jint sessionId, jlong nativePtr)
{
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client){
        int ret = client->stopWfdSession(sessionId,env);
        delete client;
        env->SetLongField(wfdSessionObj, gWFDSessionClassInfo.nativePtr,0);
        return ret;
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativePlay
 * Signature: (IZJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativePlay
  (JNIEnv * env, jobject wfdSessionObj, jint sessionId, jboolean isRTSP, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client){
        return client->play_rtsp(sessionId,isRTSP);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativePause
 * Signature: (IZJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativePause
  (JNIEnv *env, jobject wfdSessionObj, jint sessionId, jboolean isRTSP, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client){
        return client->pause_rtsp(sessionId,isRTSP);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeTeardown
 * Signature: (IZJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeTeardown
  (JNIEnv * env, jobject wfdSessionObj, jint sessionId, jboolean isRTSP, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client){
        return client->teardown_rtsp(sessionId,isRTSP);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_WFDSession
 * Method:    nativeStandby
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeStandby
  (JNIEnv *env, jobject wfdSessionObj, jint sessionId, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client){
        return client->standby_rtsp(sessionId);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeEnableUIBC
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeEnableUIBC
  (JNIEnv *env, jobject wfdSessionObj, jint sessionId, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client){
        return client->enableUIBC(sessionId);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeDisableUIBC
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeDisableUIBC
  (JNIEnv *env, jobject wfdSessionObj, jint sessionId, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client){
        return client->disableUIBC(sessionId);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeStartUIBC
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeStartUIBC
  (JNIEnv *env, jobject wfdSessionObj, jobject jobj, jlong nativePtr)
{
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        if(client->getResolution((&gResolution.width), &(gResolution.height)) !=1)
        {
            LOGE("Could not obtain session resolution");
        }
        else
        {
            LOGD(" obtained session resolution");
            gResolution.aspectRatio = gResolution.width *1.0/gResolution.height;
            return client->startUIBC(env,gEventCallbackArgs.uibcMgrFieldID, wfdSessionObj,jobj);
        }
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeStopUIBC
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeStopUIBC
  (JNIEnv *env, jobject wfdSessionObj, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client){
        return client->stopUIBC();
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeSetUIBC
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSetUIBC
  (JNIEnv *env, jobject wfdSessionObj, jint sessionId, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client){
        return client->setUIBC(sessionId);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeSetVideoSurface
 * Signature: (Landroid/view/Surface;J)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSetVideoSurface
  (JNIEnv * env, jobject wfdSessionObj,jobject javaSurface, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        Surface *nativeSurface = NULL;
        if (javaSurface) {
            nativeSurface = reinterpret_cast<Surface*>
                            (env->GetLongField(javaSurface, gSurface.surface));
            LOGD("Native surface pointer %p", nativeSurface);
        } else {
            LOGD("Java surface is null");
        }
        return client->setSurface(nativeSurface);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeSendUIBCKeyEvent
 * Signature: (Landroid/view/KeyEvent;J)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSendUIBCKeyEvent
  (JNIEnv * env, jobject wfdSessionObj, jobject keyEvent, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        KeyEvent ke;
        android_view_KeyEvent_toNative(env,keyEvent,&ke);
        return client->sendUIBCEvent(&ke,NULL);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeSendUIBCMotionEvent
 * Signature: (Landroid/view/MotionEvent;J)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSendUIBCMotionEvent
  (JNIEnv * env, jobject wfdSessionObj, jobject motionEvent, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        MotionEvent *me = android_view_MotionEvent_getNativePtr(env, motionEvent);
        if (me == NULL)
        {
            LOGE("Failed to get Native Motion Event object");
            return -1;
        }
        return client->sendUIBCEvent(me,NULL);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeSendUIBCRotateEvent
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSendUIBCRotateEvent
  (JNIEnv * env, jobject wfdSessionObj, jint rotVal, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        WFD_uibc_event_t rotationEvent;
        rotationEvent.type = WFD_UIBC_ROTATE;
        rotationEvent.parms.rotate_event.num_rotate_int =
            (int)((rotVal * 3.141)/180);
        rotationEvent.parms.rotate_event.num_rotate_fraction =
            (int)(((rotVal * 3.141)/180)-
                rotationEvent.parms.rotate_event.num_rotate_int) * 256;
        return client->sendUIBCEvent(NULL,&rotationEvent);
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_WFDSession
 * Method:    nativeSetResolution
 * Signature: (II[IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSetResolution
  (JNIEnv * env, jobject wfdSessionObj, jint formatType, jint value, jintArray
  resParams, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        jsize len = env->GetArrayLength(resParams);
        jint *nativeResParamArr = env->GetIntArrayElements(resParams, 0);
        int ret = client->setResolution(formatType, value,
            nativeResParamArr,len);
        env->ReleaseIntArrayElements(resParams , nativeResParamArr, 0);
        return ret;
    }
    return -1;
}


/*
 * Class:     com_qualcomm_wfd_WFDSession
 * Method:    nativeSetCodecResolution
 * Signature: (IIIII[IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSetCodecResolution
  (JNIEnv * env, jobject wfdSessionObj, jint codec, jint profile,
   jint level, jint formatType, jint value, jintArray
  resParams, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        jsize len = env->GetArrayLength(resParams);
        jint *nativeResParamArr = env->GetIntArrayElements(resParams, 0);

        int ret = client->sendAvFormatChange(codec - 1, 1 << profile, 1 << level,
                                 formatType, value, nativeResParamArr, len);
        env->ReleaseIntArrayElements(resParams , nativeResParamArr, 0);
        return ret;
    }
    return -1;
}


/*
 * Class:     com_qualcomm_wfd_WFDSession
 * Method:    nativeSetBitRate
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSetBitRate
  (JNIEnv *env, jobject wfdSessionObj, jint bitrate, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);

    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        return client->setBitrate(bitrate);
    }
    return -1;

}


#if 0
/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeQueryTCPTransportSupport
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeQueryTCPTransportSupport
  (JNIEnv *, jobject);
#endif
/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeNegotiateRtpTransport
 * Signature: (IIIJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeNegotiateRtpTransport
  (JNIEnv *env, jobject wfdSessionObj, jint transportType, jint bufferLengthMs, jint port, jlong nativePtr){

    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        return client->negotiateRtpTransport(transportType, bufferLengthMs, port);
    }
    return -1;
}

#if 0
/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeSetRtpTransport
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSetRtpTransport
  (JNIEnv *, jobject, jint);
#endif
/*
 * Class:     Java_com_qualcomm_wfd_WFDSession_nativeTcpPlaybackControl
 * Method:    nativeTcpPlaybackControl
 * Signature: (IIJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeTcpPlaybackControl
  (JNIEnv *env, jobject wfdSessionObj, jint cmdType, jint cmdVal, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        return client->tcpPlaybackControl(cmdType, cmdVal);
    }
    return -1;
}


/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeSetDecoderLatency
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSetDecoderLatency
  (JNIEnv *env, jobject wfdSessionObj, jint latency, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        client->setDecoderLatency(latency);
        return 0;
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_WFDSession
 * Method:    nativeSetSurfaceProp
 * Signature: (Lcom/qualcomm/wfd/WFDSession$surfaceProperties;J)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSetSurfaceProp
  (JNIEnv *env, jobject wfdSessionObj, jobject surfPropObj, jlong nativePtr)
{
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        /*Get the arrays (objects in Java lingo)*/
        jobject scArr = env->GetObjectField(surfPropObj,
            gWFDSurfacePropClassInfo.surfaceCoord);
        jobject spArr = env->GetObjectField(surfPropObj,
            gWFDSurfacePropClassInfo.screenProps);

        //Know what thou doeth ! cast into primtiveType Arrays
        jintArray* scArray = reinterpret_cast<jintArray*>(&scArr);
        jsize sclen = env->GetArrayLength(*scArray);
        jintArray* spArray = reinterpret_cast<jintArray*>(&spArr);
        jsize splen = env->GetArrayLength(*spArray);

        /*Finally get the raw primitive arrays from the JNI Arrays*/
        const jint* surfaceCoords = const_cast<jint*>
            (env->GetIntArrayElements(*scArray,NULL));
        const jint* screenParams = const_cast<jint*>
            (env->GetIntArrayElements(*spArray,NULL));

        if(surfaceCoords && screenParams)
        {
            client->setSurfaceProp(surfaceCoords,sclen,screenParams,splen);
            gScreenDimensions = client->screenDims();
        }
        if(surfaceCoords)
        {
            env->ReleaseIntArrayElements(*scArray,
                const_cast<jint*>(surfaceCoords),0);
        }
        if(screenParams)
        {
            env->ReleaseIntArrayElements(*spArray,
                const_cast<jint*>(screenParams),0);
        }
        env->DeleteLocalRef(scArr);
        env->DeleteLocalRef(spArr);
        return 0;
    }
    return -1;
}

#if 0
/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeSetAvPlaybackMode
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSetAvPlaybackMode
  (JNIEnv *, jobject, jint);

#endif
/*
 * Class:     com_qualcomm_wfd_WFDSession
 * Method:    nativeGetConfigItems
 * Signature: ([IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeGetConfigItems
  (JNIEnv * env, jobject wfdSessionObj, jintArray configItems, jlong nativePtr)
{
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if (client)
    {
        int* cfgItems = NULL;
        size_t length = 0;
        int ret = client->getConfigItems(&cfgItems,&length);
        LOGD("Calling native WiFiDisplayClient::getConfigItems returned %d", ret);
        if(0 != ret || !cfgItems || 0 == length)
        {
            LOGE("Something horrible transpired!");
            return -1;
        }
        jsize len = length;
        //Create jint array to tide over platform dependent
        //definition of ints
        jint* tempArr = new jint[len];

        if(!tempArr)
        {
            LOGE("Java_com_qualcomm_wfd_WFDSession_nativeGetConfigItems:\
                  tempArr memory allocation failed");
            return -1;
        }

        for(jint i=0;i< len;i++) {
            tempArr[i] = cfgItems[i];
            LOGI("cfgItems is %d",tempArr[i]);
        }
        env->SetIntArrayRegion(configItems, 0, len, tempArr);
        //Clear any array out of bound access exceptions if any
        env->ExceptionClear();
        delete[] tempArr;
        delete[] cfgItems;
    }
    return -1;
}

/*
 * Class:     com_qualcomm_wfd_WFDSession
 * Method:    nativeExecuteRuntimeCommand
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeExecuteRuntimeCommand
  (JNIEnv * env, jobject wfdSessionObj, jint cmd, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    if(client)
    {
        return client->executeRuntimeCommand(cmd);
    }
    return -1;
}

#if 0
/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeGetCommonRes
 * Signature: ([I)[I
 */
JNIEXPORT jintArray JNICALL Java_com_qualcomm_wfd_WFDSession_nativeGetCommonRes
  (JNIEnv *, jobject, jintArray);
#endif

/*
 * Class:     com_qualcomm_wfd_service_WFDSession
 * Method:    nativeGetNegotiatedRes
 * Signature: ([IJ)[I
 */
JNIEXPORT jintArray JNICALL Java_com_qualcomm_wfd_WFDSession_nativeGetNegotiatedRes
  (JNIEnv * env, jobject wfdSessionObj, jintArray result, jlong nativePtr)
{
    UNUSED(wfdSessionObj);
    if(!result)
    {
        LOGE("result array passed is NULL");
        return NULL;
    }

    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    jint nativeResult[1] = {-1};
    jintArray retArr = NULL;

    if (client)
    {
        uint32_t* negRes = NULL;
        size_t len = 0;
        int ret = client->getNegotiatedResolution(NULL, len);

        if (ret == 0 && len > 0)
        {
            negRes = new uint32_t[len];
            if (negRes)
            {
                ret = client->getNegotiatedResolution(negRes, len);
            }
            else
            {
                LOGE("Unable to alloacate memory!");
            }
        }

        LOGD("Calling WiFiDisplayClient::getNegotiatedResolution returned %d", ret);

        if (ret == 0 && negRes != NULL)
        {
            jint* tempNegRes = new jint[len*sizeof(jint)];
            //Create an array of jints to tide over platform-dependent defn. of ints
            if (tempNegRes)
            {
                for(jint i = 0 ; i< (jint)len ; i++ )
                {
                    tempNegRes[i] = negRes[i];
                }

                retArr = env->NewIntArray(len);
                if (retArr)
                {
                    env->SetIntArrayRegion(retArr,0,len,tempNegRes);
                    nativeResult[0] = 0;
                }
                else
                {
                    LOGE("Unable to alloacate memory!");
                    env->ExceptionClear();
                }

                if (tempNegRes)
                {
                    delete[] tempNegRes;
                }
            }
            else
            {
                LOGE("Unable to alloacate memory!");
            }
        }

        if (negRes)
        {
            delete[] negRes;
        }
    }
    else
    {
        LOGE("native client handle is NULL");
    }

    env->SetIntArrayRegion(result,0,1,nativeResult);
    return retArr;
}

/*
 * Class:     com_qualcomm_wfd_WFDSession
 * Method:    nativeSetAvPlaybackMode
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_wfd_WFDSession_nativeSetAvPlaybackMode
  (JNIEnv *env, jobject wfdSessionObj, jint mode, jlong nativePtr)
{
    UNUSED(env);
    UNUSED(wfdSessionObj);
    LOGD("setAvPlaybackMode: %d", mode);
    WiFiDisplayClient* client = reinterpret_cast<WiFiDisplayClient*>(nativePtr);
    int ret = client->setAVPlaybackMode(mode);
    LOGD("Calling WiFiDisplayClient::setAVPlaybackMode = %d",ret);
    return (ret == 1) ? JNI_TRUE : JNI_FALSE;
}

#if 0
/** =======================================================================
*                Supported JNI native functions
** ======================================================================= */

/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    setUIBC
 * Signature: (I)Z
 */

JNIEXPORT jboolean JNICALL Java_com_qualcomm_wfd_WFDNative_setUIBC
  (JNIEnv *env, jclass cls, jint sessionId)

{
    UNUSED(env);
    UNUSED(cls);
    LOGE("setUIBC with session ID %d ", sessionId);
    if (WiFiDisplayClient::setUIBC(sessionId))
        return JNI_TRUE;
    else
        return JNI_FALSE;
}
/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    standby
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_qualcomm_wfd_WFDNative_standby
  (JNIEnv *env, jclass cls, jint sessionId)

{
    UNUSED(env);
    UNUSED(cls);
    LOGD("standby called");
    if (WiFiDisplayClient::standby_rtsp(sessionId)) {
        return JNI_TRUE ;
    } else {
        return JNI_FALSE;
    }
 }

/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    setBitrate
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_wfd_WFDNative_setBitrate
  (JNIEnv *env, jclass cls, jint bitrate)
{
    UNUSED(env);
    UNUSED(cls);
    LOGD("setBitrate bitrate: %d", bitrate);
    int res = WiFiDisplayClient::setBitrate(bitrate);
    LOGE("Calling WiFiDisplayClient:::setBitrate returned %d",res);
}

/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    negotiateRtpTransport
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_wfd_WFDNative_negotiateRtpTransport
  (JNIEnv *env, jclass cls, jint transportType, jint bufferLengthMs, jint port)
{
    UNUSED(env);
    UNUSED(cls);
    LOGD("negotiateRtpTransport[transportType: %d, bufferLengthMs: %d, port: %d]", transportType, bufferLengthMs, port);
    int res = WiFiDisplayClient::negotiateRtpTransport(transportType,bufferLengthMs,port);
    LOGE("Calling WiFiDisplayClient::negotiateRtpTransport returned %d", res);
}

/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    setRtpTransport
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_wfd_WFDNative_setRtpTransport
  (JNIEnv *env, jclass cls, jint transportType)
{
    UNUSED(env);
    UNUSED(cls);
    LOGD("setRtpTransport: %d", transportType);
    int res = WiFiDisplayClient::setRtpTransport(transportType);
    LOGE("Calling WiFiDisplayClient::setRtpTransport returned %d", res);
}

/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    queryTCPTransportSupport
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_wfd_WFDNative_queryTCPTransportSupport
  (JNIEnv *, jclass)
{
    LOGD("queryTCPTransportSupport in native");
    int res = WiFiDisplayClient::queryTCPTransportSupport();
    LOGE("Calling WiFiDisplayClient::queryTCPTransportSupport returned %d", res);
}


/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    setDecoderLatency
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_wfd_WFDNative_setDecoderLatency
  (JNIEnv *env, jclass cls, jint latency)
{
    UNUSED(env);
    UNUSED(cls);
    LOGD("setDecoderLatency: %d", latency);
    int res = WiFiDisplayClient::setDecoderLatency(latency);
    LOGE("Calling WiFiDisplayClient::setDecoderLatency returned %d", res);
}

/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    tcpPlaybackControl
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_wfd_WFDNative_tcpPlaybackControl
  (JNIEnv *env, jclass cls, jint cmdType, jint cmdVal)
{
    UNUSED(env);
    UNUSED(cls);
    LOGD("tcpPlaybackControl: %d %d", cmdType, cmdVal);
    int res = WiFiDisplayClient::tcpPlaybackControl(cmdType,cmdVal);
    LOGE("Calling WiFiDisplayClient::tcpPlaybackControl returned %d", res);
}

/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    setSurfaceProp
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_wfd_WFDNative_setSurfaceProp
  (JNIEnv *env, jclass cls, jint arg1, jint arg2, jint orientation)
{
        UNUSED(env);
        UNUSED(cls);
        gScreenDimensions.width  = arg1;
        gScreenDimensions.height = arg2;
        gScreenDimensions.Orientation = orientation;
        if (arg2!=0) {
        gScreenDimensions.aspectRatio = arg1*1.0/arg2;
        LOGD("setSurfaceProp width %d height %d orientation %d screen aspect ratio is %lf",
             gScreenDimensions.width, gScreenDimensions.height,orientation,
             gScreenDimensions.aspectRatio);
        } else {
            LOGE("height of screen resolution is zero");
        }
        (void)WiFiDisplayClient::setSurfaceProp(gScreenDimensions.width,
                                                gScreenDimensions.height,
                                                gScreenDimensions.Orientation);
}


/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    getConfigItems
 * Signature: ([I)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_wfd_WFDNative_getConfigItems
  (JNIEnv *env, jclass cls, jintArray configItems)
{
    UNUSED(cls);
    int* cfgItems = NULL;
    size_t length = 0;
    int ret = WiFiDisplayClient::getConfigItems(&cfgItems,&length);
    LOGE("Calling native WiFiDisplayClient::getConfigItems returned %d", ret);
    if(0 != ret || !cfgItems || 0 == length)
    {
        LOGE("Something horrible transpired!");
        return;
    }
    jsize len = length;
    //Create jint array to tide over platform dependent
    //definition of ints
    jint* tempArr = new jint[len];
    for(jint i=0;i< len;i++) {
        tempArr[i] = cfgItems[i];
        LOGI("cfgItems is %d",tempArr[i]);
    }
    env->SetIntArrayRegion(configItems, 0, len, tempArr);
    //Clear any array out of bound access exceptions if any
    env->ExceptionClear();
    delete[] tempArr;
    delete[] cfgItems;
}

/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    executeRuntimeCommand
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_qualcomm_wfd_WFDNative_executeRuntimeCommand
  (JNIEnv * env, jclass cls, jint cmd)
{
    UNUSED(env);
    UNUSED(cls);
    int ret = WiFiDisplayClient::executeRuntimeCommand(cmd);
    LOGE("Calling WiFiDisplayClient::executeRuntimeCommand returned %d", ret);
    if(ret == 0)
      return JNI_FALSE;
    else
      return JNI_TRUE;
}

/*
 * Class:     com_qualcomm_wfd_WFDNative
 * Method:    getCommonRes
 * Signature: ([I)[I
 */
JNIEXPORT jintArray JNICALL Java_com_qualcomm_wfd_WFDNative_getCommonRes
  (JNIEnv * env, jclass cls, jintArray result)
{
    UNUSED(cls);
    if(!result)
    {
        return NULL;
    }
    jint nativeResult[1] = {0};
    jint numProf = 0;
    uint32_t* comRes = NULL;
    int ret = WiFiDisplayClient::getCommonResolution(&comRes,&numProf);
    LOGD("Calling WiFiDisplayClient::getCommonResolution returned %d", ret);
    if(!comRes)
    {
        nativeResult[0]= -1;
        env->SetIntArrayRegion(result,0,1,nativeResult);
        return NULL;
    }
    jsize len = 4*numProf;
    /*For each of the H264 profiles there will be 3 values to consider CEA,
     *VESA and HH. So create an array with a size of multiple of
      * 4 = [1Profile + its 3 corresponding bitmaps]
     */
    jint* tempComRes = new jint[len*sizeof(jint)];
    //Create an array of jints to tide over platform dependent defn. of ints
    for(jint i = 0 ; i< len ; i++ )
    {
        tempComRes[i] = comRes[i];
    }
    jintArray retArr = env->NewIntArray(len);
    if(retArr)
    {
        env->SetIntArrayRegion(retArr,0,len,tempComRes);
    }
    else
    {
        LOGE("Unable to alloacate memory!");
        env->ExceptionClear();
        //Clear memory exception, WFD shouldn't fail
        nativeResult[0]= -1;
    }
    if (comRes)
    {
        delete[] comRes;
    }
    if(tempComRes)
    {
        delete[] tempComRes;
    }
    env->SetIntArrayRegion(result,0,1,nativeResult);
    if(nativeResult[0] == -1)
    {
        return NULL;
    }
    return retArr;
}

#endif
/** =======================================================================
*                Callback functions from C++ to Java
** ======================================================================= */

/**
 * callback_handler is used as the callback function from .cpp to .java.
 * This function can be used in both Java threads and native threads.
 */
static void callback_handler(jstring eventName, jobjectArray oarr,
    jobject wfdSession)
{
    int status;
    JNIEnv *env;
    bool isAttached = false;

    status = gJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4);
    if (status < 0) {
        LOGE("callback_handler: failed to get JNI environment, assuming native thread");
        status = gJavaVM->AttachCurrentThread(&env, NULL);
        if (status < 0) {
            LOGE("callback_handler: failed to attach current thread");
            return;
        }
        isAttached = true;
    }
    /* Find the callback method ID */
    jmethodID method = env->GetMethodID(gWFDSessionClassInfo.wfdSessionClazz,
            "eventCallback", "(Ljava/lang/String;[Ljava/lang/Object;)V");
    if (!method) {
        LOGE("callback_handler: failed to get method ID");
        if (isAttached) {
            gJavaVM->DetachCurrentThread();
        }
        return;
    }
    const char* eName = env->GetStringUTFChars(eventName, NULL);
    LOGD("callback_handler: call Java method....   event=%s  oarr_size=%d", eName, env->GetArrayLength(oarr));
    env->ReleaseStringUTFChars(eventName, eName);
    env->CallVoidMethod(wfdSession, method, eventName, oarr);

    if (isAttached) {
        gJavaVM->DetachCurrentThread();
    }
}


/**
 * Function to trigger call back function to WFDNative.
 * message is comprised of: eventName, eventObjectArray={specified string array}.
 */
static void jni_stringarray_callback(const char* eName, int numObjects,
    char strArray[][256],const jobject& wfdSessionObj)
{
    int status;
    JNIEnv *env;
    bool isAttached = false;
    bool isSurfaceUpdate = false;
    LOGD("stringarray_callback  eName=%s  numObjects=%d", eName, numObjects);
    if(numObjects >= 4 && !strcmp(eName,MM_UPDATE_EVENT)) {
       //Make sure that is a MM_UPDATE_EVENT with the requisite number of objects
       //and then check if its a MMStreamStarted event to handle it a bit differently
       if(!strcmp(strArray[0],"MMStreamStarted")) {
          isSurfaceUpdate = true;
          LOGD("Received MMStreamStarted");
       }
    }
    for (int i=0; i<numObjects; i++) {
        LOGD("\t strArray[%d] = \"%s\"", i, strArray[i]);
    }

    status = gJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4);
    if (status < 0) {
    LOGE("callback_handler: failed to get JNI environment, assuming native thread");
    status = gJavaVM->AttachCurrentThread(&env, NULL);
    if (status < 0) {
        LOGE("callback_handler: failed to attach current thread");
        return;
    }
    isAttached = true;
    }

    jstring eventName = env->NewStringUTF(eName);
    jclass objArrCls = env->FindClass("java/lang/Object");
    jobjectArray oarr = env->NewObjectArray(numObjects, objArrCls, NULL);
    if(!isSurfaceUpdate) {
       jstring strArr[numObjects];
       for (int i=0;i<numObjects;i++) {
            strArr[i] = env->NewStringUTF((char*)strArray[i]);
            env->SetObjectArrayElement(oarr, i, strArr[i]);
            env->DeleteLocalRef(strArr[i]);
       }
       callback_handler(eventName, oarr,wfdSessionObj);
    } else {
        jstring strArr[numObjects - 1];
        int i = 0;
        for (;i<numObjects-1;i++) {
            strArr[i] = env->NewStringUTF((char*)strArray[i]);
            env->SetObjectArrayElement(oarr, i, strArr[i]);
            env->DeleteLocalRef(strArr[i]);
        }
        long surface = atol((char*)strArray[i]);
        IGraphicBufferProducer* gphBuff = reinterpret_cast<IGraphicBufferProducer*>(surface);
        jobject jSurface = android_view_Surface_createFromIGraphicBufferProducer(env,gphBuff);
        env->SetObjectArrayElement(oarr, i, jSurface);
        callback_handler(eventName, oarr,wfdSessionObj);
        env->DeleteLocalRef(jSurface);
     }
    env->DeleteLocalRef(eventName);
    env->DeleteLocalRef(oarr);
    env->DeleteLocalRef(objArrCls);
    if (isAttached)
        gJavaVM->DetachCurrentThread();
}


/*
 *
 *
[SM_A -> SM_B]
-	enableWfd(devType)
-	disableWfd()
-	getWfdIeInfo()
-	startWfdSession(peer_mac_addr)
-	startWfdCoupling(peer_mac_addr)
-	play(sessionId)
-	pause(sessionId)
-	teardown()
-	audioReroute()
  (Power management)
-   suspend()
-   resume()
  (Exception handling)
-   eventWiFiStateChanged()

[SM_B -> SM_A]
-	getLocalWiFiInfo()
-	getWfdDeviceInfo(peer_mac_addr)
-	eventServiceStateChanged(serviceState)
-	eventSessionStateChanged(mac_addr, sessionState, sessionId0, sessionId1)
-	eventStreamControlCompleted(controlCmd, sessionId, status)
  (Link info)
-   getWiFiState()
  (set WiFi)
-   setTxPower
  (Power management)
-   setWakeLock()
 *
 */
