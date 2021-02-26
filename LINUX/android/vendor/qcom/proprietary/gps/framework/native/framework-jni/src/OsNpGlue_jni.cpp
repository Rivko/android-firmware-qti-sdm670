/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "LocSvc_HIDL_OsNpGlue_jni"
#define LOG_NDEBUG 0

#include <inttypes.h>
#include <string>
#include <jni.h>
#include <JNIHelp.h>
#include "utils_jni.h"
#include <android_runtime/AndroidRuntime.h>
#include <utils/Log.h>
#include <map>

#include <vendor/qti/gnss/1.0/ILocHidlGnss.h>
#include <vendor/qti/gnss/1.0/ILocHidlIzatOsNpGlue.h>
#include <vendor/qti/gnss/1.0/ILocHidlIzatOsNpGlueCallback.h>

using ::vendor::qti::gnss::V1_0::ILocHidlGnss;
using ::vendor::qti::gnss::V1_0::ILocHidlIzatOsNpGlue;
using ::vendor::qti::gnss::V1_0::ILocHidlIzatOsNpGlueCallback;
using ::vendor::qti::gnss::V1_0::LocHidlIzatStreamType;
using ::vendor::qti::gnss::V1_0::LocHidlIzatHorizontalAccuracy;
using ::vendor::qti::gnss::V1_0::LocHidlIzatProviderStatus;
using ::vendor::qti::gnss::V1_0::LocHidlIzatRequest;
using ::vendor::qti::gnss::V1_0::LocHidlIzatLocation;
using ::vendor::qti::gnss::V1_0::LocHidlNetworkPositionSourceType;

using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;
using ::android::sp;

using namespace std;
using namespace android;

static jmethodID method_onSetRequest;
static jmethodID method_onStopRequest;
static jobject callBackJavaObj;

/* ===================================================================
 *   Callbacks to Java layer
 * ===================================================================*/
static void onSetRequestJNI(const LocHidlIzatRequest& request);
static void onStopRequestJNI();

/* ===========================================================
 *   HIDL Callbacks :
 * ===========================================================*/
struct IzatOsNpGlueCallback : public ILocHidlIzatOsNpGlueCallback {
    Return<void> onSetRequest(const LocHidlIzatRequest& request) override;
    Return<void> onStopRequest() override;
};

Return<void> IzatOsNpGlueCallback::onSetRequest(const LocHidlIzatRequest& request) {

    FROM_HIDL_SERVICE();
    onSetRequestJNI(request);
    return Void();
}

Return<void> IzatOsNpGlueCallback::onStopRequest() {

    FROM_HIDL_SERVICE();
    onStopRequestJNI();
    return Void();
}

/* ===================================================================
 *   HIDL Interface
 * ===================================================================*/
static sp<ILocHidlIzatOsNpGlue> gnssOsNpGlueIface = nullptr;

struct LocHidlDeathRecipient : virtual public hidl_death_recipient
{
    // hidl_death_recipient interface
    virtual void serviceDied(uint64_t cookie, const wp<IBase>& /*who*/) override {
        gnssOsNpGlueIface = nullptr;
        ALOGE("%s] ILocHidlIzatOsNpGlue died with cookie: %" PRIu64, __FILE__, cookie);
    }
};
static sp<LocHidlDeathRecipient> locHidlDeathRecipient = nullptr;

static sp<ILocHidlIzatOsNpGlue>& getOsNpGlueIface() {
    static uint64_t deathCount = 0;
    if (gnssOsNpGlueIface == nullptr) {
        auto gnssVendorHal = ILocHidlGnss::getService(GNSS_VENDOR_SERVICE_INSTANCE);
        if (gnssVendorHal == nullptr) {
            ALOGE("Unable to get GnssVendor service\n");
        } else {
            ALOGD("Got GnssVendor Service.");
            auto osnp = gnssVendorHal->getExtensionLocHidlIzatOsNpGlue();

            if (!osnp.isOk()) {
                ALOGE("Unable to get a handle to OsNpGlue extension !");
            } else {

                ALOGD("Got OsNpGlue Extension.");
                if (locHidlDeathRecipient == nullptr) {
                    locHidlDeathRecipient = new LocHidlDeathRecipient();
                }
                gnssOsNpGlueIface = osnp;
                auto r = gnssOsNpGlueIface->linkToDeath(locHidlDeathRecipient, deathCount++);
                if (!r.isOk() || r == false) {
                    ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
                    gnssOsNpGlueIface = nullptr;
                }
            }
        }
    }
    return gnssOsNpGlueIface;
}

// Called during class load time
static void onJavaClassLoad (JNIEnv* env, jclass clazz) {
    method_onSetRequest = env->GetMethodID(clazz, "onSetRequest", "(JFI)V");
    method_onStopRequest = env->GetMethodID(clazz, "onStopRequest", "()V");

    // HIDL Service
    getOsNpGlueIface();
}

// Called when java object is instantiated
static void onJavaInstanceInit (JNIEnv *env, jobject obj) {
    // cache the Java callback object
    if (!callBackJavaObj) {
        callBackJavaObj = env->NewGlobalRef(obj);

        TO_HIDL_SERVICE();
        auto iface = getOsNpGlueIface();
        if (iface != nullptr) {
            auto r = iface->setCallback(new IzatOsNpGlueCallback());
            if (!r.isOk()) {
                ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
            }
        }
    }
}

// Called when java object is destroyed
static void onJavaInstanceDeinit (JNIEnv* env, jobject) {
    // clear Global References if any
    if (callBackJavaObj) {
        env->DeleteGlobalRef(callBackJavaObj);
        callBackJavaObj = NULL;
    }
}

// java to native calls
static void onJavaLocationChanged(JNIEnv*, jobject,
                    jboolean hasTime, jlong utcTime,
                    jboolean hasElapsedRealTimeNanos, jlong elapsedRealTimeNanos,
                    jboolean hasLatitude, jdouble latitude,
                    jboolean hasLongitude, jdouble longitude,
                    jboolean hasAccuracy, jfloat accuracy,
                    jboolean hasVerticalAccuracy, jfloat verticalAccuracy,
                    jboolean hasAltitude, jdouble altitude,
                    jboolean hasBearing, jfloat bearing,
                    jboolean hasSpeed, jfloat speed,
                    jboolean hasNetworkPositionSource, jint networkPositionSource) {
    auto iface = getOsNpGlueIface();
    if (iface != nullptr) {
        LocHidlIzatLocation location = {};

        location.hasUtcTimestampInMsec = hasTime;
        location.hasElapsedRealTimeInNanoSecs = hasElapsedRealTimeNanos;
        location.hasLatitude = hasLatitude;
        location.hasLongitude = hasLongitude;
        location.hasHorizontalAccuracy = hasAccuracy;
        location.hasAltitudeWrtEllipsoid = hasAltitude;
        location.hasBearing = hasBearing;
        location.hasSpeed = hasSpeed;
        location.hasNetworkPositionSource = hasNetworkPositionSource;
        location.hasVertUnc = hasVerticalAccuracy;

        location.utcTimestampInMsec = utcTime;
        location.elapsedRealTimeInNanoSecs = elapsedRealTimeNanos;
        location.latitude = latitude;
        location.longitude = longitude;
        location.horizontalAccuracy = accuracy;
        location.altitudeWrtEllipsoid = altitude;
        location.bearing = bearing;
        location.speed = speed;
        location.networkPositionSource =
                static_cast<LocHidlNetworkPositionSourceType>(networkPositionSource);
        location.vertUnc = verticalAccuracy;

        TO_HIDL_SERVICE();
        auto r = iface->locationChanged(location);
        if (!r.isOk()) {
            ALOGE("Error invoking HIDL API [%s]", r.description().c_str());
        }
    }
}

/* ===================================================================
 *   Callbacks to Java layer
 * ===================================================================*/
void onSetRequestJNI (const LocHidlIzatRequest& request) {
    JNIEnv* env = android::AndroidRuntime::getJNIEnv ();
    if (callBackJavaObj == NULL || env == NULL) {
        ALOGE("JNIEnv or callBackJavaObj is NULL");
        return;
    }

    jlong interval = request.timeIntervalBetweenFixes;
    jint numUpdates = request.numUpdates;
    jfloat smallestDisplacement = request.smallestDistanceBetweenFixes;

    env->CallVoidMethod(callBackJavaObj, method_onSetRequest,
                        interval, smallestDisplacement, numUpdates);
}

void onStopRequestJNI () {
    JNIEnv* env = android::AndroidRuntime::getJNIEnv ();
    if (callBackJavaObj == NULL || env == NULL) {
        ALOGE("JNIEnv or callBackJavaObj is NULL");
        return;
    }

    env->CallVoidMethod(callBackJavaObj, method_onStopRequest);
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"nativeOnClassLoad", "()V", (void*)onJavaClassLoad},
    {"nativeOnInstanceInit", "()V", (void*)onJavaInstanceInit},
    {"nativeOnInstanceDeinit", "()V", (void*)onJavaInstanceDeinit},
    {"nativeOnLocationChanged", "(ZJZJZDZDZFZFZDZFZFZI)V", (void*)onJavaLocationChanged}
};

int register_OsNpGlue (JNIEnv* env) {
    return jniRegisterNativeMethods(env, "com/qualcomm/location/izatprovider/NpProxy",
                                    sMethods, NELEM(sMethods));
}
